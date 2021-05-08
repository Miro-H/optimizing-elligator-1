IDIR    	= include
ODIR    	= src/obj
SDIR  		= src
BDIR		= bin
SCRIPTS_DIR = scripts
BIN_FILES   = $(filter-out $(wildcard $(BDIR)/.*), $(wildcard $(BDIR)/*))
LOG_DIR 	= logs/

ifeq ($(BENCHMARKS),)
	BENCHMARKS = -1
endif

# src directory of the benchmarked code
SRC_BENCHED		= ../src
ODIR_BENCHED 	= $(SRC_BENCHED)/obj
IDIR_BENCHED	= $(SRC_BENCHED)/../include

# common scripts
ROOT_SCRIPTS_DIR = ../scripts

CC      	= gcc
CFLAGS  	= -I$(IDIR_BENCHED) -I$(IDIR)

# Add additional compilation flags to improve code quality
CFLAGS += -Wall -Werror

# Add macros for compiled C files
CFLAGS += -DLOG_PATH=\"$(abspath $(LOG_DIR))\"

# Add default number of sets and reps for benchmarks
ifeq ($(SETS),)
	SETS = 10
endif

ifeq ($(REPS),)
	REPS = 10000
endif

CFLAGS += -DSETS=$(SETS) -DREPS=$(REPS)

# Forward COLLECT_STATS env variable to compiled binaries
ifneq ($(COLLECT_STATS),)
	CFLAGS += -DCOLLECT_STATS=${COLLECT_STATS}
endif

# Assume that the executable depends on all header files.
DEPS = $(wildcard $(IDIR)/*.h)

LIBS =

# Main target, building all benchmarks
all: compile-runtime-benchmark

.PHONY: run-all
run-all: run-runtime-benchmark

.PHONY: all-benched
ifeq ($(COLLECT_STATS),)
	make -C $(SRC_BENCHED) all
else
	COLLECT_STATS=${COLLECT_STATS} make -C $(SRC_BENCHED) all
endif

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
compile-runtime-benchmark: all-benched gen-bench-types runtime-benchmark

.PHONY: run-runtime-benchmark
run-runtime-benchmark: compile-runtime-benchmark
	./$(BDIR)/runtime-benchmark $(BENCHMARKS)

runtime-benchmark: $(OBJ_BENCHED) $(OBJ)
	$(CC) -o $(BDIR)/$@ $^ $(CFLAGS) $(LIBS)

.PHONY: plots
plots: run-all
	COLLECT_STATS=$(COLLECT_STATS) SETS=$(SETS) REPS=$(REPS) $(SCRIPTS_DIR)/make_all_plots.sh

.PHONY: publish-plots
publish-plots: run-all
	PUBLISH=1 ${SCRIPTS_DIR}/make_all_plots.sh

.PHONY: clean
clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ $(BIN_FILES)
