CC = gcc

ddnet_lint: src/lint.c
	$(CC) src/lint.c -g -lclang -o ddnet_lint -I /usr/lib/llvm-20/include/ -L /usr/lib/llvm-20/lib/

clean:
	rm -f ddnet_lint
