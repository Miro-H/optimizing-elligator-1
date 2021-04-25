IDIR    	= include
ODIR    	= src/obj
SDIR  		= src
BDIR		= bin
SCRIPTS_DIR = scripts
BIN_FILES   = $(filter-out $(wildcard $(BDIR)/.*), $(wildcard $(BDIR)/*))
LOG_DIR 	= logs/

# src directory of the benchmarked code
SRC_BENCHED		= ../src
ODIR_BENCHED 	= $(SRC_BENCHED)/obj
IDIR_BENCHED	= $(SRC_BENCHED)/../include

CC      	= gcc
CFLAGS  	= -I$(IDIR_BENCHED) -I$(IDIR)

# Add additional compilation flags to improve code quality
CFLAGS += -Wall -Werror

# Add macros for compiled C files
CFLAGS += -DLOG_PATH=\"$(abspath $(LOG_DIR))\"

# Assume that the executable depends on all header files.
DEPS = $(wildcard $(IDIR)/*.h)

LIBS =

# Main target, building all benchmarks
all: compile-bigint-benchmark

.PHONY: all-benched
all-benched:
	make -C $(SRC_BENCHED) all

# Fresh build
.PHONY: rebuild
rebuild: clean all

$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

# Specify the object files constituting the test
_OBJ	= benchmark_helpers.o bigint_benchmark.o
OBJ 	= $(patsubst %,$(ODIR)/%,$(_OBJ))
# Specify the object files of the benched functions
_OBJ_BENCHED 	= bigint.o
OBJ_BENCHED		= $(patsubst %,$(ODIR_BENCHED)/%,$(_OBJ_BENCHED))

.PHONY: compile-bigint-benchmark
compile-bigint-benchmark: all-benched bigint-benchmark

.PHONY: run-bigint-benchmark
run-bigint-benchmark: compile-bigint-benchmark
	./$(BDIR)/bigint-benchmark

bigint-benchmark: $(OBJ_BENCHED) $(OBJ)
	$(CC) -o $(BDIR)/$@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean
clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ $(BIN_FILES)
