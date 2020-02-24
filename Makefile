all:
	@gcc \
		-std=c99 -pedantic \
		-march=native \
		-Wall -Wextra \
		-O3 -funroll-loops \
		rng.c \
		u512.s fp.s \
		mont.c \
		csidh.c \
		main.c \
		-o main

debug:
	gcc \
		-Wall -Wextra \
		-g \
		rng.c \
		u512.s fp.s \
		mont.c \
		csidh.c \
		main.c \
		-o main


bench:
	@gcc \
		-std=c99 -pedantic \
		-march=native \
		-Wall -Wextra \
		-O3 \
		rng.c \
		u512.s fp.s \
		mont.c \
		csidh.c \
		bench.c \
		-o bench	

clean:
	rm -f main
	rm -f bench

