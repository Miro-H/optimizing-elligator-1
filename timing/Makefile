# Switches between Makefiles for different versions

TARGETS := 								\
	all 								\
	all-benched 						\
	rebuild 							\
	run-all 							\
	gen-bench-types 					\
	run-runtime-benchmark				\
	plot-runtime-benchmark 				\
	runtime-benchmark 					\
	run-runtime-benchmark-curve1174 	\
	plot-runtime-benchmark-curve1174	\
	runtime-benchmark-curve1174			\
	run-gmp-benchmark 					\
	plot-gmp-benchmark 					\
	gmp-benchmark 						\
	run-memcpy-benchmark				\
	plot-memcpy-benchmark				\
	memcpy-benchmark 					\
	plot-comparison 					\
	plots								\
	clean

#
# The following versions exist:
#	- Version 1: Straightforward C, unoptimized
#	- Version 2: Uptimized C using standard techniques
#	- Version 3 (in the future): C including AVX optimizations
#
ifeq ($(VERSION),)
	VERSION = 2
endif
export VERSION

# Set shared variables
export IDIR				= include
export SDIR				= src
export BDIR				= bin
export SCRIPTS_DIR		= scripts
export BIN_FILES		= $(filter-out $(wildcard $(BDIR)/.*), $(wildcard $(BDIR)/*))
export LOG_DIR			= logs
export ROOT_SCRIPTS_DIR	= ../scripts

export LATEST_LOG_PATH = ${LOG_DIR}/latest_log_path.txt

ifeq ($(BENCHMARKS),)
	BENCHMARKS = -1
endif
export BENCHMARKS

export CC = gcc

#
# Add various shared CFLAGS
#

# Add additional compilation flags to improve code quality
CFLAGS += -Wall -Werror

# Add default number of sets and reps for benchmarks
ifeq ($(SETS),)
	SETS = 10
endif

ifeq ($(REPS),)
	REPS = 10000
endif
export SETS
export REPS
CFLAGS += -DSETS=$(SETS) -DREPS=$(REPS) -DVERSION=$(VERSION)

# Forward COLLECT_STATS env variable to compiled binaries
export LOG_BASE_PATH = $(abspath $(LOG_DIR))
ifneq ($(COLLECT_STATS),)
	LOG_SUBDIRS = stats
	CFLAGS += -DCOLLECT_STATS=${COLLECT_STATS}
else
	LOG_SUBDIRS = runtime
endif

LOG_SUBDIRS := $(LOG_SUBDIRS)/$(shell date "+%Y-%m-%d_%H-%M-%S")
export LOG_SUBDIRS

# Add standard optimization flags if no others are specified
ifeq ($(OPT_FLAGS),)
	OPT_FLAGS = -Ofast
endif
CFLAGS += $(OPT_FLAGS)

export CFLAGS

#
# Switch between different Makefiles
#
ifeq ($(VERSION),1)

.PHONY: $(TARGETS)
$(TARGETS):
	make -f Makefile.V1 $(MAKECMDGOALS)

else ifeq ($(VERSION), 2)

.PHONY: $(TARGETS)
$(TARGETS):
	make -f Makefile.V2 $(MAKECMDGOALS)

endif
