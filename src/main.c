#include <checks/func_order.h>
#include <checks/floats.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, const char **argv) {
	if (argc < 1) {
		// printf("Usage: %s [clang_args...]\n", argv[0]);
		printf("Usage: %s\n", argv[0]);
		return 1;
	}

	// // pass on cli args
	// const char *const *command_line_args = argv + 1;
	// int num_command_line_args = argc - 1;

	const char *const cli_args[] = {
		"-x",
		"c++-header",
		"-std=c++20",
		"-Isrc"
	};
	int num_cli_args = sizeof(cli_args) / sizeof(const char *);

	const char *filenames[] = {
		"src/base/str",
		"src/base/fs",
		"src/base/windows"
	};

	int errors = 0;

	for(int i = 0; i < sizeof(filenames) / sizeof(const char *); i++) {
		const char *base_filename = filenames[i];
		char source_filename[512];
		char header_filename[512];
		snprintf(header_filename, sizeof(header_filename), "%s.h", base_filename);
		snprintf(source_filename, sizeof(source_filename), "%s.cpp", base_filename);
		ddl_check_src_and_header(header_filename, source_filename, cli_args, num_cli_args);

		if(!ddl_check_floats(source_filename)) {
			errors++;
		}
		if(!ddl_check_floats(header_filename)) {
			errors++;
		}
	}

	if(errors) {
		fprintf(stderr, "Failed with %d errors!\n", errors);
		exit(1);
	}

	puts("OK");
	return 0;
}
