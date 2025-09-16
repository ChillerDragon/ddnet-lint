CC = gcc

ddnet_lint:
	$(CC) \
		src/checks/*.c \
		src/base/*.c \
		src/main.c \
		-g \
		-lclang \
		-o ddnet_lint \
		-I src \
		-I /usr/lib/llvm-20/include/ -L /usr/lib/llvm-20/lib/

clean:
	rm -f ddnet_lint
	rm -f floats
