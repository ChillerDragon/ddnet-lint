#pragma once

#include <stddef.h>
#include <stdbool.h>

#include <clang-c/Index.h>

typedef struct {
	char functions[2048][512];
	int num_functions;

	// the path to the source or header file
	// that is currently being parsed
	// expected example values:
	//
	//  "src/base/str.cpp"
	//  "src/base/str.h"
	const char *filename;
} DDNetLintCtx;

void ddl_push_func(DDNetLintCtx *ctx, const char *funcname);
void ddl_print_funcs(DDNetLintCtx *ctx);
bool ddl_has_same_func_order(DDNetLintCtx *ctx_header, DDNetLintCtx *ctx_source);
enum CXChildVisitResult fetch_func_callback(CXCursor cursor, CXCursor parent, CXClientData client_data);
void ddl_get_funcs(const char *const *command_line_args, int num_command_line_args, DDNetLintCtx *ctx);
void ddl_check_src_and_header(const char *header_filename, const char *source_filename, const char *const *command_line_args, int num_command_line_args);
