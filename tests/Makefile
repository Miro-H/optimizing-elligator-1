IDIR    	= include
ODIR    	= src/obj
SDIR  		= src
BDIR		= bin
SCRIPTS_DIR = scripts
BIN_FILES   = $(filter-out $(wildcard $(BDIR)/.*), $(wildcard $(BDIR)/*))

# src directory of the code to be tested

#
# The following versions exist:
#	- Version 1: Straightforward C, unoptimized
#	- Version 2: Uptimized C using standard techniques
#	- Version 3 (in the future): C including AVX optimizations
#
ifeq ($(VERSION),)
	VERSION = 2
endif

ifeq ($(VERSION), 1)
	TESTED_ROOT_DIR = ../code_unoptimized
else ifeq ($(VERSION), 2)
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

# Transfer variables to compiled C files
CFLAGS += -DVERSION=$(VERSION)

# Assume that the executable depends on all header files.
DEPS = $(wildcard $(IDIR)/*.h)

LIBS = -lcheck -lm

ifeq ($(OS),Windows_NT)
	# Do something windows specific if necessary
else
	IS_WIN_SUBSYSTEM = "$(shell grep -q Microsoft /proc/version && echo -n linux_subsys)"
	
	ifeq ($(IS_WIN_SUBSYSTEM), "linux_subsys")
		LIBS += -lpthread -lrt -lsubunit
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
endif

# Main target, building all tests
ifneq ($(VERSION), 1)
all: compile-bigint-test compile-elligator-test compile-bigint-curve1174-test
else
all: compile-bigint-test compile-elligator-test
endif

.PHONY: all-tested
all-tested:
	echo $(LIBS)
	echo $(IS_WIN_SUBSYSTEM)
	
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
run-tests: clean all
	
	$(SCRIPTS_DIR)/run_all_tests.sh

$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

################################################################################
#                             BigInt Tests                                     #
################################################################################

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

################################################################################
#                       BigInt Curve1774 Tests                                 #
################################################################################

# only exists from version 2 onwards
ifneq ($(VERSION), 1)
# Specify the object files constituting the test
_OBJ	= bigint_curve1174_test.o
OBJ 	= $(patsubst %,$(ODIR)/%,$(_OBJ))
# Specify the object files of the tested functions
_OBJ_TESTED = bigint_curve1174.o bigint.o
OBJ_TESTED 	= $(patsubst %,$(ODIR_TESTED)/%,$(_OBJ_TESTED))

.PHONY: compile-bigint-curve1174-test
compile-bigint-curve1174-test: all-tested bigint-curve1174-test

bigint-curve1174-test: $(OBJ_TESTED) $(OBJ)
	$(CC) -o $(BDIR)/$@ $^ $(CFLAGS) $(LIBS)
endif

################################################################################
#                           Elligator Tests                                    #
################################################################################

# Specify the object files constituting the test
_OBJ	= elligator_test.o
OBJ 	= $(patsubst %,$(ODIR)/%,$(_OBJ))
# Specify the object files of the tested functions
_OBJ_TESTED = bigint_curve1174.o bigint.o elligator.o
OBJ_TESTED 	= $(patsubst %,$(ODIR_TESTED)/%,$(_OBJ_TESTED))

.PHONY: compile-elligator-test
compile-elligator-test: all-tested elligator-test

elligator-test: $(OBJ_TESTED) $(OBJ)
	$(CC) -o $(BDIR)/$@ $^ $(CFLAGS) $(LIBS)

################################################################################
#                            Other targets                                     #
################################################################################

.PHONY: clean
clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ $(BIN_FILES)
