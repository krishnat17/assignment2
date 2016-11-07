all: threads

threads: threads.c
	gcc -pthread threads.c -w -o threads

tar: 
	tar cf threads.tar threads.c makefile

clean:
	rm -f threads
