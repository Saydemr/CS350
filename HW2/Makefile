CC=gcc
DEPS=meal.h
LDFLAGS=-pthread

make: kitchen.c meal.h
	CC -o kitchen kitchen.c -w LDFLAGS
clean:
	make M=$(PWD) clean
