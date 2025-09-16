CC = gcc

all: floats ddnet_lint

floats: src/floats.c
	$(CC) src/floats.c -g -lclang -o floats -I /usr/lib/llvm-20/include/ -L /usr/lib/llvm-20/lib/

ddnet_lint: src/lint.c
	$(CC) src/lint.c -g -lclang -o ddnet_lint -I /usr/lib/llvm-20/include/ -L /usr/lib/llvm-20/lib/

clean:
	rm -f ddnet_lint
	rm -f floats
