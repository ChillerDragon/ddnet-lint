#pragma once

#include <clang-c/Index.h>

char* ddl_read_line(const char* filename, unsigned line_num);
enum CXChildVisitResult ddl_float_callback(CXCursor cursor, CXCursor parent, CXClientData client_data);
bool ddl_check_floats(const char *source_filename);

