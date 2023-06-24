CC=gcc
CFLAGS = -Wall -std=c99 -g

OBJECT_FILES = *.o */*.o */*/*.o
EXECUTABLE_FILES = concurrent solver

all: concurrent solver

.PHONY: clean
clean:
	-rm -f $(OBJECT_FILES) $(EXECUTABLE_FILES) $(testFiles)

universal = main.o $(CUBE_FILES) Solver/cubestate.o
non_thread_solver = Solver/solver.o Storage/queue.o Storage/arraylist.o
thread_solver = Solver/concurrent_solver.o Storage/concurrent_queue.o Storage/concurrent_arraylist.o

#executables
concurrent: $(universal) $(thread_solver) test
	$(CC) -c $(CFLAGS) $(universal) $(thread_solver) -latomic -o concurrent
solver: $(universal) $(non_thread_solver) test
	$(CC) -c $(CFLAGS) $(universal) $(non_thread_solver) -latomic -o solver


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
#Stored in a variable since both needed when including cube.o
CUBE_FILES = Cube/cube.o Cube/subcube.o
$(CUBE_FILES): Cube/cube.h Cube/subcube.h


#Testing

#Runs all above targets as tests
empty:=
space:= $(empty) $(empty)
# Extracts the test programs (given by their path) and runs each test in order.
runTests = $(subst $(space), && ,$(strip $(foreach test,$(filter Testing/%,$^),./$(test))))

testGroups = subcubeTest cubeTest storageTest solverTest
.PHONY: test $(testGroups)
test: $(testGroups)
$(testGroups):
	$(runTests)

#Specific tests and dependencies for each group
subcubeTest: Testing/Cube/testSubcube

cubeTest: subcubeTest Testing/Cube/testCube

storageTest: Testing/Storage/testStorage

solverTest: cubeTest storageTest Testing/Solver/testSolver


testFiles = Testing/Cube/testCube Testing/Cube/testSubcube Testing/Storage/testStorage Testing/Solver/testSolver
Testing/test.o $(testFiles): Testing/test.h
$(testFiles): %: Testing/test.o %.o
Testing/Cube/testCube: $(CUBE_FILES)
Testing/Cube/testSubcube: Cube/subcube.o
