CC=gcc
CFLAGS = -Wall -std=c99 -g

OBJECT_FILES = *.o */*.o
EXECUTABLE_FILES = concurrent solver

all: concurrent solver

.PHONY: clean
clean:
	-rm -f $(OBJECT_FILES) $(EXECUTABLE_FILES)

universal = main.o Cube/cube.o Cube/subcube.o Solver/cubestate.o
non_thread_solver = Solver/solver.o Storage/queue.o Storage/arraylist.o
thread_solver = Solver/concurrent_solver.o Storage/concurrent_queue.o Storage/concurrent_arraylist.o

#executables
concurrent: $(universal) $(thread_solver)
	gcc $(universal) $(thread_solver) -latomic -o concurrent
solver: $(universal) $(non_thread_solver)
	gcc $(universal) $(non_thread_solver) -latomic -o solver


main.o: Solver/solver.h Storage/storage.h Storage/queue.h

#solver implementation
Solver/concurrent_solver.o Solver/solver.o: Solver/solver.h Solver/cubestate.h
Solver/cubestate.o: Solver/cubestate.h
Solver/cubestate.h: Cube/cube.h

#visited state list implementation
Storage/hashtable.o Storage/arraylist.o Storage/concurrent_arraylist.o: Storage/storage.h

#Queue implementation
Storage/queue.o Storage/concurrent_queue.o: Storage/queue.h

# Cube storage
Cube/cube.o Cube/subcube.o: Cube/cube.h Cube/subcube.h