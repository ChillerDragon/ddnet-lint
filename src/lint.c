#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <clang-c/CXString.h>
#include <clang-c/Index.h>

typedef struct {
	char functions[2048][512];
	int num_functions;
} DDNetLintCtx;

void ddl_push_func(DDNetLintCtx *ctx, const char *funcname) {
	strncpy(ctx->functions[ctx->num_functions++], funcname, sizeof(ctx->functions[0]));
}

void ddl_print_funcs(DDNetLintCtx *ctx) {
	for(int i = 0; i < ctx->num_functions - 1; i++) {
		puts(ctx->functions[i]);
	}
}

enum CXChildVisitResult print_function_names(CXCursor cursor, CXCursor parent, CXClientData client_data) {
	enum CXCursorKind cursor_kind = clang_getCursorKind(cursor);
	if (cursor_kind != CXCursor_FunctionDecl && cursor_kind != CXCursor_CXXMethod) {
		return CXChildVisit_Continue;
	}
	if (clang_Cursor_getStorageClass(cursor) == CX_SC_Static) {
		return CXChildVisit_Continue;
        }

	CXString name = clang_getCursorSpelling(cursor);
	// printf("%s\n", clang_getCString(name));

	DDNetLintCtx *ctx = client_data;
	ddl_push_func(ctx, clang_getCString(name));

	clang_disposeString(name);
	return CXChildVisit_Continue;
}

void ddl_get_funcs(const char *source_filename, const char *const *command_line_args, int num_command_line_args, DDNetLintCtx *ctx) {
	CXIndex index = clang_createIndex(0, 0);
	CXTranslationUnit unit = clang_parseTranslationUnit(
		index,
		source_filename,
		command_line_args,
		num_command_line_args,
		NULL, 0,
		CXTranslationUnit_None);

	if (unit == NULL) {
		fprintf(stderr, "Error unable to parse translation unit: %s\n", source_filename);
		exit(1);
	}

	CXCursor cursor = clang_getTranslationUnitCursor(unit);
	clang_visitChildren(cursor, print_function_names, ctx);
	clang_disposeTranslationUnit(unit);
	clang_disposeIndex(index);
}

int main(int argc, const char **argv) {
	if (argc < 2) {
		printf("Usage: %s <source-file.cpp> [clang_args...]\n", argv[0]);
		return 1;
	}

	const char *source_filename = argv[1];
	const char *const *command_line_args = argv + 2;
	int num_command_line_args = argc - 2;

	DDNetLintCtx ctx = {};
	ddl_get_funcs(source_filename, command_line_args, num_command_line_args, &ctx);
	ddl_print_funcs(&ctx);

	return 0;
}
