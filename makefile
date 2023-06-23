CC=gcc
CFLAGS = -Wall -std=c99 -g

OBJECT_FILES = *.o ./*.o
EXECUTABLE_FILES = concurrent solver

all: concurrent solver

clean:
	rm -f OBJECT_FILES
	rm -f EXECUTABLE_FILES

#executables
concurrent: main.o Cube/cube.o Solver/concurrent_solver.o Storage/concurrent_queue.o Storage/concurrent_arraylist.o
solver: main.o Cube/cube.o Solver/solver.o Storage/queue.o Storage/arraylist.o

main.o: main.c Solver/solver.h Storage/storage.h Storage/queue.h

#solver implementation
Solver/concurrent_solver.o: Solver/concurrent_solver.c Solver/solver.h
Solver/solver.o: Solver/solver.c Solver/solver.h
Solver/solver.h: Solver/cubestate.o
Solver/cubestate.o: Cube/cube.h Solver/cubestate.c

#visited state list implementation
Storage/hashtable.o: Storage/storage.h Storage/hashtable.c
Storage/arraylist.o: Storage/storage.h Storage/arraylist.c
Storage/concurrent_arraylist.o: Storage/storage.h Storage/concurrent_arraylist.c

#Queue implementation
Storage/queue.o: Storage/queue.h Storage/queue.c
Storage/concurrent_queue.o: Storage/queue.h Storage/concurrent_queue.c

# Cube storage
Cube/cube.o: Cube/cube.h Cube/cube.c
Cube/subcube.o: Cube/cube.h Cube/subcube.c