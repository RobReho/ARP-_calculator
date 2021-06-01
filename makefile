all: main sum sub pro div

main: main.c
	gcc -o main main.c

g1: sum.c
	gcc -o sum sum.c

g2: sub.c
	gcc -o sub sub.c

r: pro.c
	gcc -o pro pro.c

m: div.c
	gcc -o div div.c


.PHONY: clean

clean:
		rm -f main.o
		rm -f sum.o
		rm -f sub.o
		rm -f pro.o
		rm -f div.o



