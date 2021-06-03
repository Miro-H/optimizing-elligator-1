IDIR    	= include
SDIR  		= src
BDIR		= bin
SCRIPTS_DIR = scripts
BIN_FILES   = $(filter-out $(wildcard $(BDIR)/.*), $(wildcard $(BDIR)/*))

# src directory of the code to be tested

#
# The following versions exist:
#	- Version 1: Straightforward C, unoptimized
#	- Version 2: Uptimized C using standard techniques
#	- Version 3: C including AVX optimizations
#
ifeq ($(VERSION),)
	VERSION = 3
endif

ifeq ($(VERSION), 1)
	TESTED_ROOT_DIR = ../code_unoptimized
else ifeq ($(VERSION), 2)
	TESTED_ROOT_DIR = ../code_optimized/standard_optimizations
else ifeq ($(VERSION), 3)
	TESTED_ROOT_DIR = ../code_optimized/avx_optimizations
endif

SRC_TESTED	= $(TESTED_ROOT_DIR)/src
IDIR_TESTED	= $(TESTED_ROOT_DIR)/include

CC      	= gcc
CFLAGS  	= -I$(IDIR_TESTED) -I$(IDIR)

# Add additional compilation flags to improve code quality
CFLAGS += -Wall -Werror 

ifeq ($(VERSION),3)
	CFLAGS += -mavx2 -march=native
endif



ifneq ($(DEBUG),)
	CFLAGS += -Wno-unused-variable
endif

# Transfer variables to compiled C files
CFLAGS += -DVERSION=$(VERSION)

LIBS = -lcheck -lm

ifeq ($(OS),Windows_NT)
	# Do something windows specific if necessary
else
	IS_WIN_SUBSYSTEM = "$(shell grep -q Microsoft /proc/version 2> /dev/null && echo -n linux_subsys)"

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
all: compile-bigint-test compile-elligator-test compile-bigint-curve1174-test compile-gmp-elligator-test
else
all: compile-bigint-test compile-elligator-test compile-gmp-elligator-test
endif

.PHONY: print-compile-msg
print-compile-msg:
ifeq ($(TESTED_ROOT_DIR),)
	$(error Unsupported version $(VERSION) (check Makefile comments))
endif
	@echo "\n------------------------------------------------------------------------"
	@echo "Compile tests for using $(TESTED_ROOT_DIR)"
	@echo "------------------------------------------------------------------------\n\n"

# Fresh build
.PHONY: rebuild
rebuild: clean all

# Run all tests
.PHONY: run-tests
run-tests: clean all
	$(SCRIPTS_DIR)/run_all_tests.sh

################################################################################
#                             BigInt Tests                                     #
################################################################################

# Specify the C files constituting the test
_BIGINT_SRC	= bigint_test.c
BIGINT_SRC	= $(patsubst %,$(SDIR)/%,$(_BIGINT_SRC))

_BIGINT_TESTED_SRC	= bigint.c
BIGINT_TESTED_SRC	= $(patsubst %,$(SRC_TESTED)/%,$(_BIGINT_TESTED_SRC))

.PHONY: compile-bigint-test
compile-bigint-test: print-compile-msg bigint-test

bigint-test: $(BIGINT_SRC) $(BIGINT_TESTED_SRC)
	$(CC) -o $(BDIR)/$@ $^ $(CFLAGS) $(LIBS)

################################################################################
#                       BigInt Curve1774 Tests                                 #
################################################################################

# only exists from version 2 onwards
ifneq ($(VERSION), 1)

# Specify the C files constituting the test
_BIGINT_CURVE1174_SRC	= bigint_curve1174_test.c
BIGINT_CURVE1174_SRC	= $(patsubst %,$(SDIR)/%,$(_BIGINT_CURVE1174_SRC))

_BIGINT_CURVE1174_TESTED_SRC	= bigint_curve1174.c bigint.c
BIGINT_CURVE1174_TESTED_SRC	= $(patsubst %,$(SRC_TESTED)/%,$(_BIGINT_CURVE1174_TESTED_SRC))

.PHONY: compile-bigint-curve1174-test
compile-bigint-curve1174-test: print-compile-msg bigint-curve1174-test

bigint-curve1174-test: $(BIGINT_CURVE1174_SRC) $(BIGINT_CURVE1174_TESTED_SRC)
	$(CC) -o $(BDIR)/$@ $^ $(CFLAGS) $(LIBS)

endif

################################################################################
#                           Elligator Tests                                    #
################################################################################

# Specify the C files constituting the test
_ELLIGATOR_SRC	= elligator_test.c
ELLIGATOR_SRC	= $(patsubst %,$(SDIR)/%,$(_ELLIGATOR_SRC))

ifeq ($(VERSION), 1)
_ELLIGATOR_TESTED_SRC	= bigint.c elligator.c
else
_ELLIGATOR_TESTED_SRC	= bigint_curve1174.c bigint.c elligator.c
endif
ELLIGATOR_TESTED_SRC	= $(patsubst %,$(SRC_TESTED)/%,$(_ELLIGATOR_TESTED_SRC))

.PHONY: compile-elligator-test
compile-elligator-test: print-compile-msg elligator-test

elligator-test: $(ELLIGATOR_SRC) $(ELLIGATOR_TESTED_SRC)
	$(CC) -o $(BDIR)/$@ $^ $(CFLAGS) $(LIBS)

################################################################################
#                           GMP Elligator Tests                                #
################################################################################

# Specify the object files constituting the test
_OBJ	= gmp_elligator_test.o
OBJ 	= $(patsubst %,$(ODIR)/%,$(_OBJ))
# Specify the object files of the tested functions
_OBJ_TESTED = gmp_elligator.o
OBJ_TESTED 	= $(patsubst %,$(ODIR_TESTED)/%,$(_OBJ_TESTED))

.PHONY: compile-gmp-elligator-test
compile-gmp-elligator-test: all-tested gmp-elligator-test

gmp-elligator-test: $(OBJ_TESTED) $(OBJ)
	$(CC) -lgmp -o $(BDIR)/$@ $^ $(CFLAGS) $(LIBS)

################################################################################
#                            Other targets                                     #
################################################################################

.PHONY: clean
clean:
	rm -f *~ core $(INCDIR)/*~ $(BIN_FILES)
