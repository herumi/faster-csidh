MCL=mcl.o -lmcl -L ./mcl/lib -lstdc++
all: mcl/lib/libmcl.a mcl.o
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
		$(MCL) \
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
		$(MCL) \
		-o bench	

mcl/lib/libmcl.a:
	$(MAKE) -C mcl lib/libmcl.a
mcl.o: mcl.hpp mcl.h mcl.cpp
	$(CXX) -c mcl.cpp -O3 -DNDEBUG -Wall -Wextra -I mcl/include -I mcl/src

clean:
	rm -f main
	rm -f bench

