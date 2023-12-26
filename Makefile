all: main

main:
	gcc main.c -o boids -w -lSDL2 -lSDL2_image -lm

clean:
	rm boids
