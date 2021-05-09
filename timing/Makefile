IDIR    	= include
ODIR    	= src/obj
SDIR  		= src
BDIR		= bin
SCRIPTS_DIR = scripts
BIN_FILES   = $(filter-out $(wildcard $(BDIR)/.*), $(wildcard $(BDIR)/*))
LOG_DIR 	= logs

ifeq ($(BENCHMARKS),)
	BENCHMARKS = -1
endif

# src directory of the benchmarked code

#
# The following versions exist:
#	- Version 1.0: Straightforward C, unoptimized
#	- Version 2.0: Uptimized C using standard techniques
#	- Version 3.0 (in the future): C including AVX optimizations
#
ifeq ($(VERSION),)
	VERSION = 2.0
endif

ifeq ($(VERSION), 1.0)
	BENCHED_ROOT_DIR = ../code_unoptimized
else ifeq ($(VERSION), 2.0)
	BENCHED_ROOT_DIR = ../code_optimized/standard_optimizations
endif

SRC_BENCHED	 = $(BENCHED_ROOT_DIR)/src
ODIR_BENCHED = $(SRC_BENCHED)/obj
IDIR_BENCHED = $(BENCHED_ROOT_DIR)/include

# common scripts
ROOT_SCRIPTS_DIR = ../scripts

CC      	= gcc
CFLAGS  	= -I$(IDIR_BENCHED) -I$(IDIR)

# Add additional compilation flags to improve code quality
CFLAGS += -Wall -Werror

# Add default number of sets and reps for benchmarks
ifeq ($(SETS),)
	SETS = 10
endif

ifeq ($(REPS),)
	REPS = 10000
endif

CFLAGS += -DSETS=$(SETS) -DREPS=$(REPS)

# Forward COLLECT_STATS env variable to compiled binaries
LOG_PATH := $(abspath $(LOG_DIR))
ifneq ($(COLLECT_STATS),)
	LOG_PATH := $(LOG_PATH)/stats
	CFLAGS += -DCOLLECT_STATS=${COLLECT_STATS}
else
	LOG_PATH := $(LOG_PATH)/runtime
endif

LOG_PATH := $(LOG_PATH)/$(shell date "+%Y-%m-%d_%H-%M-%S")

# Add macros for compiled C files
CFLAGS += -DLOG_PATH=\"$(LOG_PATH)\"

# Assume that the executable depends on all header files.
DEPS = $(wildcard $(IDIR)/*.h)

LIBS =

# Main target, building all benchmarks
all: compile-runtime-benchmark compile-gmp-benchmark

.PHONY: prepare-logs
prepare-logs:
	mkdir -p $(LOG_PATH)

# Stats can only be collected for our BigInt library
.PHONY: run-all
ifeq ($(COLLECT_STATS),)
run-all: run-runtime-benchmark run-gmp-benchmark
else
run-all: run-runtime-benchmark
endif

.PHONY: all-benched
all-benched:
ifeq ($(BENCHED_ROOT_DIR),)
	$(error Unsupported version $(VERSION) (check Makefile comments))
endif
	@echo "\n------------------------------------------------------------------------"
	@echo "Compile timing for using $(BENCHED_ROOT_DIR)"
	@echo "------------------------------------------------------------------------\n\n"
	make -C $(BENCHED_ROOT_DIR) clean
	COLLECT_STATS=$(COLLECT_STATS) make -C $(BENCHED_ROOT_DIR) all

# Fresh build
.PHONY: rebuild
rebuild: clean all

$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

# Specify the object files constituting the test
_OBJ	= benchmark_helpers.o runtime_benchmark.o
OBJ 	= $(patsubst %,$(ODIR)/%,$(_OBJ))
# Specify the object files of the benched functions
_OBJ_BENCHED 	= bigint.o elligator.o
OBJ_BENCHED		= $(patsubst %,$(ODIR_BENCHED)/%,$(_OBJ_BENCHED))

.PHONY: gen-bench-types
gen-bench-types:
	${ROOT_SCRIPTS_DIR}/gen_types.py \
		--src_file ${SDIR}/runtime_benchmark.c \
		--tar_file ${IDIR}/benchmark_types.h

.PHONY: compile-runtime-benchmark
compile-runtime-benchmark: prepare-logs all-benched gen-bench-types runtime-benchmark

.PHONY: run-runtime-benchmark
run-runtime-benchmark: compile-runtime-benchmark
	./$(BDIR)/runtime-benchmark $(BENCHMARKS)

runtime-benchmark: $(OBJ_BENCHED) $(OBJ)
	$(CC) -o $(BDIR)/$@ $^ $(CFLAGS) $(LIBS)


# Specify the object files constituting the test
_OBJ	= benchmark_helpers.o gmp_benchmark.o
OBJ 	= $(patsubst %,$(ODIR)/%,$(_OBJ))

# Specify the object files of the benched functions
_OBJ_BENCHED 	=
OBJ_BENCHED		= $(patsubst %,$(ODIR_BENCHED)/%,$(_OBJ_BENCHED))

.PHONY: compile-gmp-benchmark
compile-gmp-benchmark: gmp-benchmark

.PHONY: run-gmp-benchmark
run-gmp-benchmark: compile-gmp-benchmark
	./$(BDIR)/gmp-benchmark

gmp-benchmark: $(OBJ_BENCHED) $(OBJ)
	$(CC) -o $(BDIR)/$@ $^ $(CFLAGS) $(LIBS) -lgmp

.PHONY: plots
plots: run-all
	PUBLISH=$(PUBLISH) COLLECT_STATS=$(COLLECT_STATS) LOGS_DIR=$(LOG_PATH) SETS=$(SETS) \
	REPS=$(REPS) $(SCRIPTS_DIR)/make_all_plots.sh

.PHONY: clean
clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ $(BIN_FILES)
