CC = gcc
CFLAGS = -c -Wall -Werror -Wextra

all: Labs

Labs: main.o Data_Analysis.o Print_Graph.o
	$(CC) main.o Data_Analysis.o Print_Graph.o -lm -o Labs.out
	rm *.o
	
main.o:
	$(CC) $(CFLAGS) main.c -o main.o

Data_Analysis.o:
	$(CC) $(CFLAGS) Data_Analysis.c -o Data_Analysis.o

Print_Graph.o:
	$(CC) $(CFLAGS) Print_Graph.c -o Print_Graph.o

run:
	./Labs.out

clean:
	rm Labs.out

graph:
	python3 Graph.py
	