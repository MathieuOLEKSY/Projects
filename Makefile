#OLEKSY_MATHIEU 02/06/23
CC = gcc
CFLAGS = -Wall

all: main_interface

main_interface: main.o interface.o fonction.o lodepng.o
	$(CC) $(CFLAGS) $^ -o $@ -lm $(shell sdl2-config --cflags --libs) -lSDL2_image

fonction.o: headers/fonction.c
	$(CC) $(CFLAGS) -c $< -o $@

interface.o: headers/interface.c
	$(CC) $(CFLAGS) -c $< -o $@

lodepng.o: lib/lodepng.c
	$(CC) $(CFLAGS) -c $< -o $@

main.o: src/main.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf *.o

mrproper: clean
	rm -rf hello
