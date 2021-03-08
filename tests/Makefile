IDIR    	= include
ODIR    	= src/obj
SDIR  		= src
BDIR		= bin
SCRIPTS_DIR = scripts
BIN_FILES   = $(filter-out $(wildcard $(BDIR)/.*), $(wildcard $(BDIR)/*))

# src directory of the code to be tested
SRC_TESTED	= ../src
ODIR_TESTED = $(SRC_TESTED)/obj
IDIR_TESTED	= $(SRC_TESTED)/../include

CC      	= gcc
CFLAGS  	= -I$(IDIR_TESTED) -I$(IDIR)

# Add additional compilation flags to improve code quality
CFLAGS += -Wall -Werror

# Assume that the executable depends on all header files.
DEPS = $(wildcard $(IDIR)/*.h)

LIBS = -lcheck

ifeq ($(OS),Windows_NT)
	# Do something windows specific if necessary
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Darwin)
		# Do something specific to MacOS if necessary
	else
		# Do something linux specific.
		# libcheck is installed in /usr/local/lib, which is not checked by
		# default in gcc (but -L is not used on MacOS)
		CFLAGS += -L/usr/local/lib
	endif
endif

# Main target, building all tests
all: compile-example-test

.PHONY: all-tested
all-tested:
	make -C $(SRC_TESTED) all

# Fresh build
.PHONY: rebuild
rebuild: clean all

# Run all tests
.PHONY: run-tests
run-tests: all
	$(SCRIPTS_DIR)/run_all_tests.sh

$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

# Specify the object files constituting the test
_OBJ	= example_test.o
OBJ 	= $(patsubst %,$(ODIR)/%,$(_OBJ))
# Specify the object files of the tested functions
_OBJ_TESTED = example.o
OBJ_TESTED 	= $(patsubst %,$(ODIR_TESTED)/%,$(_OBJ_TESTED))

.PHONY: compile-example-test
compile-example-test: all-tested example-test

example-test: $(OBJ_TESTED) $(OBJ)
	$(CC) -o $(BDIR)/$@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean
clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ $(BIN_FILES)
