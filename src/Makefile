# CFLAGS=-Wall
masker: main.c masker.c imagelib/imagelib.c
	gcc -fopenmp -o ../masker main.c masker.c imagelib/imagelib.c -I imagelib -lm

clean:
	rm -f masker
