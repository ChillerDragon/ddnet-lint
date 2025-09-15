#include <clang-c/Index.h>
#include <stdio.h>

void print_function_names(CXCursor cursor, CXClientData client_data) {
    if (clang_getCursorKind(cursor) == CXCursor_FunctionDecl ||
        clang_getCursorKind(cursor) == CXCursor_CXXMethod) {
        CXString name = clang_getCursorSpelling(cursor);
        printf("%s\n", clang_getCString(name));
        clang_disposeString(name);
    }
    clang_visitChildren(cursor, print_function_names, NULL);
}

int main(int argc, char **argv) {
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
