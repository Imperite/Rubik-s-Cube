CC=gcc
CFLAGS = -Wall -std=c99 -g

OBJECT_FILES = *.o */*.o */*/*.o
EXECUTABLE_FILES = concurrent solver

.PHONY: clean all concurrent_deps solver_deps
all: concurrent_solver solver

clean:
	-rm -f $(OBJECT_FILES) $(EXECUTABLE_FILES) $(testFiles)

#Stored in a variable since both needed when including cube.o

CUBE_FILES = Cube/cube.o Cube/subcube.o
universal = main.o $(CUBE_FILES) Solver/cubestate.o


#executables

solver = Solver/solver.o Storage/queue.o Storage/arraylist.o
solver_deps: $(solver)

concurrent = $(patsubst /%.o,/concurrent_%.o,$(solver))
concurrent_deps: $(concurrent)
concurrent: CFLAGS+= -latomic

solver concurrent: %: $(universal) %_deps
	$(CC) $(CFLAGS) $(universal) $($@) -o $@

# Object files
main.o: Solver/solver.h Storage/storage.h Storage/queue.h

#solver implementation
Solver/concurrent_solver.o Solver/solver.o:: Solver/solver.h
Solver/*.o:: Solver/cubestate.h
Solver/cubestate.h:: Cube/cube.h

#visited state list implementation
Storage/hashtable.o Storage/arraylist.o Storage/concurrent_arraylist.o:: Storage/storage.h

#Queue implementation
Storage/queue.o Storage/concurrent_queue.o:: Storage/queue.h

# Cube storage
$(CUBE_FILES):: Cube/cube.h Cube/subcube.h


#Testting
# If testing is defined, add an extra dependency for its test file
ifdef TEST
tests = $(CUBE_FILES) $(concurrent) $(solver) Solver/cubestate.o
$(tests) :: %.o: test/%test

test/Cube/cubetest: %: Cube/subcube.o %.o
	$(CC) $(CFLAGS) test/test.o $@.o Cube/cube.o Cube/subcube.o -o $@
	./$@
	rm $@
# test/Cube/subcubetest: test/%test: test/%test.o
# 	$(CC) $(CFLAGS) test/test.o $@.o $*.o -o $@
# 	./$@
# 	rm $@


$(foreach name,$(basename $(tests)),test/$(name)test): test/%test: test/test.o %.o
	$(CC) $(CFLAGS) test/test.o $@.o $*.o -o $@
	./$@
	rm $@

endif

#Runs all above targets as tests
empty:=
space:= $(empty) $(empty)
# Extracts the test programs (given by their path) and runs each test in order.
# testExecs = $(foreach test,$(filter test/%,$^),./$(test))
# runTests = $(subst $(space), && ,$(strip $(testExecs)))

# testGroups = subcubeTest cubeTest storageTest solverTest
# .PHONY: test $(testGroups)
# test: $(testGroups)
# $(testGroups):
# 	$(runTests)
# 	rm -rf ${testExecs}

# #Specific tests and dependencies for each group
# subcubeTest: test/Cube/testSubcube

# cubeTest: subcubeTest test/Cube/testCube

# storageTest: test/Storage/testStorage

# solverTest: cubeTest storageTest test/Solver/testSolver


# testFiles = test/Cube/testCube test/Cube/testSubcube test/Storage/testStorage test/Solver/testSolver
test/test.o : test/test.h
# $(testFiles): %: test/test.o %.o
# test/Cube/testcube: $(CUBE_FILES)
# test/Cube/testSubcube: Cube/subcube.o
