#include <clang-c/CXErrorCode.h>
#include <clang-c/CXString.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <clang-c/Index.h>

#include "func_order.h"

#include <base/math.h>


void ddl_push_func(DDNetLintCtx *ctx, const char *funcname) {
	strncpy(ctx->functions[ctx->num_functions++], funcname, sizeof(ctx->functions[0]));
}

void ddl_print_funcs(DDNetLintCtx *ctx) {
	for(int i = 0; i < ctx->num_functions; i++) {
		printf(" %s\n", ctx->functions[i]);
	}
}

bool ddl_has_same_func_order(DDNetLintCtx *ctx_header, DDNetLintCtx *ctx_source) {
	bool same_amount = true;
	if(ctx_header->num_functions != ctx_source->num_functions) {
		fprintf(stderr, "Error header has %d functions while source has %d\n", ctx_header->num_functions, ctx_source->num_functions);
		same_amount = false;
	}

	int num_functions = min(ctx_header->num_functions, ctx_source->num_functions);
	int i;
	for(i = 0; i < num_functions; i++) {
		if(strncmp(ctx_header->functions[i], ctx_source->functions[i], sizeof(ctx_source->functions[i])) == 0) {
			continue;
		}

		fprintf(stderr, "Error wrong order! The %d function in the header is %s and in the source file it is %s\n", i, ctx_header->functions[i], ctx_source->functions[i]);
		return false;
	}

	if(!same_amount) {
		const char *header_name = ctx_header->functions[i];
		const char *source_name = ctx_source->functions[i];
		fprintf(stderr, "Error missing function! At offset %d Header file: %s, Source file: %s\n", i, header_name, source_name);
		return false;
	}

	return true;
}

enum CXChildVisitResult fetch_func_callback(CXCursor cursor, CXCursor parent, CXClientData client_data) {
	enum CXCursorKind cursor_kind = clang_getCursorKind(cursor);
	if (cursor_kind != CXCursor_FunctionDecl && cursor_kind != CXCursor_CXXMethod) {
		return CXChildVisit_Continue;
	}
	if (clang_Cursor_getStorageClass(cursor) == CX_SC_Static) {
		return CXChildVisit_Continue;
	}

	CXString name = clang_getCursorSpelling(cursor);
	DDNetLintCtx *ctx = client_data;

	// Get source location
	CXSourceLocation location = clang_getCursorLocation(cursor);
	// Translate location into file
	CXFile file;
	unsigned line, column;
	clang_getSpellingLocation(location, &file, &line, &column, NULL);
	CXString fileNameCX = clang_getFileName(file);
	const char *fileName = clang_getCString(fileNameCX);
	if(strcmp(fileName, ctx->filename)) {
		// printf(" skip func from other header %s %s\n", clang_getCString(name), fileName);
	} else {
		ddl_push_func(ctx, clang_getCString(name));
	}
	clang_disposeString(name);
	clang_disposeString(fileNameCX);
	return CXChildVisit_Continue;
}

void ddl_get_funcs(const char *const *command_line_args, int num_command_line_args, DDNetLintCtx *ctx) {
	int excludeDeclarationsFromPCH = 1;
	int displayDiagnostics = 1;
	CXIndex index = clang_createIndex(excludeDeclarationsFromPCH, displayDiagnostics);


	CXTranslationUnit unit;
	enum CXErrorCode code = clang_parseTranslationUnit2(
		index,
		ctx->filename,
		command_line_args,
		num_command_line_args,
		NULL, 0,
		CXTranslationUnit_None,
		&unit);

	if (code != CXError_Success) {
		fprintf(stderr, "Error unable to parse translation unit: %s error %d\n", ctx->filename, code);
		exit(1);
	}

	unsigned int numErrors = clang_getNumDiagnostics(unit);
	if (numErrors) {
		unsigned displayOptions = clang_defaultDiagnosticDisplayOptions();
		for (unsigned i=0; i < numErrors; ++i) {
			CXDiagnostic diag = clang_getDiagnostic(unit, i);
			CXString str = clang_formatDiagnostic(diag, displayOptions);

			fprintf(stderr, " %s\n", clang_getCString(str));
			clang_disposeString(str);
			clang_disposeDiagnostic(diag);
		}
	}

	if (unit == NULL) {
		fprintf(stderr, "Error unable to parse translation unit: %s\n", ctx->filename);
		exit(1);
	}

	CXCursor cursor = clang_getTranslationUnitCursor(unit);
	clang_visitChildren(cursor, fetch_func_callback, ctx);
	clang_disposeTranslationUnit(unit);
	clang_disposeIndex(index);
}

void ddl_check_src_and_header(const char *header_filename, const char *source_filename, const char *const *command_line_args, int num_command_line_args) {
	DDNetLintCtx ctx_source = {
		.filename = source_filename
	};
	DDNetLintCtx ctx_header = {
		.filename = header_filename
	};

	ddl_get_funcs(command_line_args, num_command_line_args, &ctx_header);
	ddl_get_funcs(command_line_args, num_command_line_args, &ctx_source);

	// printf("funcs in %s\n", header_filename);
	// ddl_print_funcs(&ctx_header);
	// printf("funcs in %s\n", source_filename);
	// ddl_print_funcs(&ctx_source);

	if(!ddl_has_same_func_order(&ctx_header, &ctx_source)) {
		fprintf(stderr, "Error files %s and %s do not have the same function order.\n", header_filename, source_filename);
		exit(1);
	} else {
		printf("[+] functions match in %s,cpp .. OK\n", header_filename);
		printf("[+]   functions found: %d\n", ctx_header.num_functions);
	}
}

