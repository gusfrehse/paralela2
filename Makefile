FLAGS=-O3 -Wall -g

CC=mpicc

RM=rm -f

EXEC=tsp

all: $(EXEC)

$(EXEC): $(EXEC).c
	$(CC) $(FLAGS) $(EXEC).c -c -o $(EXEC).o
	$(CC) $(FLAGS) $(EXEC).o -lm -o $(EXEC)

run:
	./$(EXEC)

clean:
	$(RM) $(EXEC).o $(EXEC)
