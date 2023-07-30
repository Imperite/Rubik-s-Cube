CC=gcc
CFLAGS = -Wall -std=c99 -g
VPATH = Testing/Cube


OBJECT_FILES = *.o */*.o */*/*.o
EXECUTABLE_FILES = concurrent solver

.PHONY: clean all
all: concurrent solver

clean:
	-rm -f $(OBJECT_FILES) $(EXECUTABLE_FILES) $(testFiles)

#Stored in a variable since both needed when including cube.o

CUBE_FILES = Cube/cube.o Cube/subcube.o
universal = main.o $(CUBE_FILES) Solver/cubestate.o
non_thread_solver = Solver/solver.o Storage/queue.o Storage/arraylist.o
threaded = $(patsubst /%.o,/concurrent_%.o,$(non_thread_solver))
# thread_solver = Solver/concurrent_solver.o Storage/concurrent_queue.o Storage/concurrent_arraylist.o


#executables
concurrent: CFLAGS+= -latomic
concurrent: $(universal) $(threaded)
solver: $(universal) $(non_thread_solver)

concurrent solver: %:
	$(CC) $(CFLAGS) $(universal) $(threaded) -o $@

main.o: Solver/solver.h Storage/storage.h Storage/queue.h

#solver implementation
Solver/concurrent_solver.o Solver/solver.o: Solver/solver.h
Solver/*.o: Solver/cubestate.h
Solver/cubestate.h: Cube/cube.h

#visited state list implementation
Storage/hashtable.o Storage/arraylist.o Storage/concurrent_arraylist.o: Storage/storage.h

#Queue implementation
Storage/queue.o Storage/concurrent_queue.o: Storage/queue.h

# Cube storage
$(CUBE_FILES):: Cube/cube.h Cube/subcube.h

Cube/cube = Cube/subcube.o



#Testing
# If testing is defined, add an extra dependency for its test file
ifdef TST
Cube/cube.o:: %.o: Testing/%test
#	  $(CC) $(CFLAGS) -c -o $*.o $*.c
#	 $(CC) $(CFLAGS) $*.o $($*) Testing/$*test.o -o Testing/$*test
	./Testing/$*test
endif

Testing/Cube/cubetest:Cube/subcube.o
	$(CC) $(CFLAGS) Testing/test.o $@.o Cube/cube.o Cube/subcube.o -o $@
Testing/Cube/subcubetest: Testing/%test:
	$(CC) $(CFLAGS) Testing/test.o $@.o $*.o -o $@

Testing/Cube/subcubetest.o: Testing/test.o
Testing/*/*:  Testing/test.o


#Runs all above targets as tests
empty:=
space:= $(empty) $(empty)
# Extracts the test programs (given by their path) and runs each test in order.
# testExecs = $(foreach test,$(filter Testing/%,$^),./$(test))
# runTests = $(subst $(space), && ,$(strip $(testExecs)))

# testGroups = subcubeTest cubeTest storageTest solverTest
# .PHONY: test $(testGroups)
# test: $(testGroups)
# $(testGroups):
# 	$(runTests)
# 	rm -rf ${testExecs}

# #Specific tests and dependencies for each group
# subcubeTest: Testing/Cube/testSubcube

# cubeTest: subcubeTest Testing/Cube/testCube

# storageTest: Testing/Storage/testStorage

# solverTest: cubeTest storageTest Testing/Solver/testSolver


# testFiles = Testing/Cube/testCube Testing/Cube/testSubcube Testing/Storage/testStorage Testing/Solver/testSolver
Testing/test.o : Testing/test.h
# $(testFiles): %: Testing/test.o %.o
# Testing/Cube/testcube: $(CUBE_FILES)
# Testing/Cube/testSubcube: Cube/subcube.o
