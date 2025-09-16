#include <clang-c/Index.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* read_line(const char* filename, unsigned line_num) {
	FILE* file = fopen(filename, "r");
	if (!file) return NULL;
	size_t bufsize = 4096;
	char* buffer = malloc(bufsize);
	unsigned current = 1;
	while (fgets(buffer, bufsize, file)) {
		if (current == line_num) {
			fclose(file);
			return buffer;
		}
		current++;
	}
	fclose(file);
	free(buffer);
	return NULL;
}

enum CXChildVisitResult visitor(CXCursor cursor, CXCursor parent, CXClientData client_data) {
	CXSourceLocation location = clang_getCursorLocation(cursor);
	if (!clang_Location_isFromMainFile(location))
		return CXChildVisit_Recurse;

	if (clang_getCursorKind(cursor) == CXCursor_FloatingLiteral) {
		// Get line number and file
		CXFile file;
		unsigned line, column, offset;
		clang_getExpansionLocation(location, &file, &line, &column, &offset);
		CXString file_name_cx = clang_getFileName(file);
		const char* file_name = clang_getCString(file_name_cx);

		// Read the line from the file
		char* code_line = read_line(file_name, line);

		// Get float literal string
		CXToken* tokens = NULL;
		unsigned int numTokens = 0;
		CXTranslationUnit tu = (CXTranslationUnit)client_data;
		CXSourceRange range = clang_getCursorExtent(cursor);
		clang_tokenize(tu, range, &tokens, &numTokens);

		for (unsigned int i = 0; i < numTokens; i++) {
			CXString spelling = clang_getTokenSpelling(tu, tokens[i]);
			printf("Float literal: %s at %s:%u\n", clang_getCString(spelling), file_name, line);
			if (code_line) {
				printf("    Code line: %s", code_line);
				if (code_line[strlen(code_line)-1] != '\n') printf("\n");
				free(code_line);
			}
			clang_disposeString(spelling);
		}
		clang_disposeTokens(tu, tokens, numTokens);
		clang_disposeString(file_name_cx);
	}
	return CXChildVisit_Recurse;
}

void ddl_check_floats(const char *source_filename) {
	CXIndex index = clang_createIndex(0, 0);
	CXTranslationUnit tu = clang_parseTranslationUnit(
		index, source_filename, NULL, 0, NULL, 0, CXTranslationUnit_None);

	if (tu == NULL) {
		fprintf(stderr, "Failed to parse translation unit: %s\n", source_filename);
		exit(1);
	}

	CXCursor rootCursor = clang_getTranslationUnitCursor(tu);
	clang_visitChildren(rootCursor, visitor, tu);

	clang_disposeTranslationUnit(tu);
	clang_disposeIndex(index);
}

int main(int argc, char** argv) {
	if (argc < 2) {
		fprintf(stderr, "Usage: %s file.cpp\n", argv[0]);
		return 1;
	}
	ddl_check_floats(argv[1]);

	float foo = 4.20f;
	float bar = 4.2f;

	return 0;
}
