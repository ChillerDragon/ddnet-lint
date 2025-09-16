#include <clang-c/Index.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Callback for visiting AST nodes
enum CXChildVisitResult visitor(CXCursor cursor, CXCursor parent, CXClientData client_data) {
	CXSourceLocation location = clang_getCursorLocation(cursor);
	if (clang_Location_isFromMainFile(location) == 0)
		return CXChildVisit_Recurse;

	// Check if it's a floating literal
	if (clang_getCursorKind(cursor) == CXCursor_FloatingLiteral) {
		CXToken* tokens = NULL;
		unsigned int numTokens = 0;
		CXTranslationUnit tu = (CXTranslationUnit)client_data;
		CXSourceRange range = clang_getCursorExtent(cursor);
		clang_tokenize(tu, range, &tokens, &numTokens);

		for (unsigned int i = 0; i < numTokens; i++) {
			CXString spelling = clang_getTokenSpelling(tu, tokens[i]);
			printf("Float literal: %s\n", clang_getCString(spelling));
			clang_disposeString(spelling);
		}
		clang_disposeTokens(tu, tokens, numTokens);
	}
	return CXChildVisit_Recurse;
}

int main(int argc, char** argv) {
	if (argc < 2) {
		fprintf(stderr, "Usage: %s file.cpp\n", argv[0]);
		return 1;
	}

	float meta = 4.20f;
	float foo = 2.f;

	CXIndex index = clang_createIndex(0, 0);
	CXTranslationUnit tu = clang_parseTranslationUnit(
		index, argv[1], NULL, 0, NULL, 0, CXTranslationUnit_None);

	if (tu == NULL) {
		fprintf(stderr, "Failed to parse translation unit\n");
		return 1;
	}

	CXCursor rootCursor = clang_getTranslationUnitCursor(tu);
	clang_visitChildren(rootCursor, visitor, tu);

	clang_disposeTranslationUnit(tu);
	clang_disposeIndex(index);
	return 0;
}
