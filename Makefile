CC =cc
CFlags =-c -Wall -g

all: first

first: main.o func.o 
	$(CC) main.o func.o -o project -lncurses
	
main.o: main.c
	$(CC) $(CFlags) main.c
	
func.o: func.c
	$(CC) $(CFlags) func.c
	
#Deleting all object files
clean:
	rm -rf *o project
