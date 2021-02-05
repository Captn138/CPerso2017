all: compile execute

compile: snake.o
	gcc -Wall snake.o -o exec

snake.o: snake.c
	gcc -Wall snake.c -c

execute: exec
	./exec

clean:
	rm *.o exec

install:
	sudo apt-get install gcc
