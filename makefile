all: threads

threads: threads.c
	gcc -pthread threads.c -o threads

clean:
	rm -f threads
