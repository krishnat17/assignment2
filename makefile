all: threads

threads: threads.c
	gcc -pthread threads.c -w -o threads

clean:
	rm -f threads
