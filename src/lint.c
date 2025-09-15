#include <clang-c/Index.h>
#include <stdio.h>

enum CXChildVisitResult print_function_names(CXCursor cursor, CXCursor parent, CXClientData client_data) {
	enum CXCursorKind cursor_kind = clang_getCursorKind(cursor);
	if (cursor_kind != CXCursor_FunctionDecl || cursor_kind != CXCursor_CXXMethod) {
		return CXChildVisit_Continue;
	}

	CXString name = clang_getCursorSpelling(cursor);
	printf("%s\n", clang_getCString(name));
	clang_disposeString(name);
	return CXChildVisit_Continue;
}

int main(int argc, const char **argv) {
	if (argc < 2) {
		printf("Usage: %s <source-file.cpp> [clang_args...]\n", argv[0]);
		return 1;
	}

	CXIndex index = clang_createIndex(0, 0);
	CXTranslationUnit unit = clang_parseTranslationUnit(
		index,
		argv[1],
		argv + 2,
		argc - 2,
		NULL, 0,
		CXTranslationUnit_None);

	if (unit == NULL) {
		fprintf(stderr, "Unable to parse translation unit. Quitting.\n");
		return 2;
	}

	CXCursor cursor = clang_getTranslationUnitCursor(unit);
	clang_visitChildren(cursor, print_function_names, NULL);

	clang_disposeTranslationUnit(unit);
	clang_disposeIndex(index);
	return 0;
}
