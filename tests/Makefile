IDIR    	= include
ODIR    	= src/obj
SDIR  		= src
BDIR		= bin
SCRIPTS_DIR = scripts
BIN_FILES   = $(filter-out $(wildcard $(BDIR)/.*), $(wildcard $(BDIR)/*))

# src directory of the code to be tested

#
# The following versions exist:
#	- Version 1.0: Straightforward C, unoptimized
#	- Version 2.0: Uptimized C using standard techniques
#	- Version 3.0 (in the future): C including AVX optimizations
#
ifeq ($(VERSION),)
	VERSION = 1.0
endif

ifeq ($(VERSION), 1.0)
	TESTED_ROOT_DIR = ../code_unoptimized
else ifeq ($(VERSION), 2.0)
	TESTED_ROOT_DIR = ../code_optimized/standard_optimizations
endif

SRC_TESTED	= $(TESTED_ROOT_DIR)/src
ODIR_TESTED = $(SRC_TESTED)/obj
IDIR_TESTED	= $(TESTED_ROOT_DIR)/include

CC      	= gcc
CFLAGS  	= -I$(IDIR_TESTED) -I$(IDIR)

# Add additional compilation flags to improve code quality
CFLAGS += -Wall -Werror

ifneq ($(DEBUG),)
	CFLAGS += -Wno-unused-variable
endif

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
all: compile-example-test compile-bigint-test compile-elligator-test

.PHONY: all-tested
all-tested:
ifeq ($(TESTED_ROOT_DIR),)
	$(error Unsupported version $(VERSION) (check Makefile comments))
endif
	@echo "\n------------------------------------------------------------------------"
	@echo "Compile tests for using $(TESTED_ROOT_DIR)"
	@echo "------------------------------------------------------------------------\n\n"
	make -C $(TESTED_ROOT_DIR) all

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

# Specify the object files constituting the test
_OBJ	= bigint_test.o
OBJ 	= $(patsubst %,$(ODIR)/%,$(_OBJ))
# Specify the object files of the tested functions
_OBJ_TESTED = bigint.o
OBJ_TESTED 	= $(patsubst %,$(ODIR_TESTED)/%,$(_OBJ_TESTED))

.PHONY: compile-bigint-test
compile-bigint-test: all-tested bigint-test

bigint-test: $(OBJ_TESTED) $(OBJ)
	$(CC) -o $(BDIR)/$@ $^ $(CFLAGS) $(LIBS)

# Specify the object files constituting the test
_OBJ	= elligator_test.o
OBJ 	= $(patsubst %,$(ODIR)/%,$(_OBJ))
# Specify the object files of the tested functions
_OBJ_TESTED = bigint.o elligator.o
OBJ_TESTED 	= $(patsubst %,$(ODIR_TESTED)/%,$(_OBJ_TESTED))

.PHONY: compile-elligator-test
compile-elligator-test: all-tested elligator-test

elligator-test: $(OBJ_TESTED) $(OBJ)
	$(CC) -o $(BDIR)/$@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean
clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ $(BIN_FILES)
