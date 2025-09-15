CC = gcc

ddnet_lint: src/lint.c
	$(CC) src/lint.c -g -lclang -o ddnet_lint

clean:
	rm -f ddnet_lint
