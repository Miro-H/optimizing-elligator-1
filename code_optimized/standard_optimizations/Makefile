IDIR    	= ./include
LDIR		= ./lib
BDIR		= ./bin
SDIR		= ./src
ODIR    	= ./$(SDIR)/obj
TESTS_DIR 	= ../../tests
BIN_FILES   = $(filter-out $(wildcard $(BDIR)/.*), $(wildcard $(BDIR)/*))

# common scripts
ROOT_SCRIPTS_DIR = ../../scripts

CC      = gcc
CFLAGS  = -I$(IDIR)

# Add additional compilation flags to improve code quality
CFLAGS += -Wall -Werror

# Forward COLLECT_STATS env variable to compiled binaries
ifneq ($(COLLECT_STATS),)
	CFLAGS += -DCOLLECT_STATS=${COLLECT_STATS}
endif

# Add libraries to this macro, if needed
LIBS    =

LIB_NAME 	= $(LDIR)/elligator.a
# Add the name of every .c file that you want to compile here
_LIB_SRCS		= bigint_curve1174.c bigint.c elligator.c
LIB_SRCS 		= $(patsubst %,$(SDIR)/%,$(_LIB_SRCS))
# This assumes the library depends on all header files in IDIR
LIB_HEADERS = $(wildcard $(IDIR)/*.h)
LIB_OBJS 	= $(patsubst %,$(ODIR)/%,$(_LIB_SRCS:.c=.o))


# This target is run whenever make is called without any additional
# arguments. Make sure it always compiles the main project.
all: gen-bigint-types $(LIB_NAME) build-lib

$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

# Fresh build
rebuild: clean all

# Build library
build-lib:
	ar rcs $(LIB_NAME) $(LIB_HEADERS) $(LIB_OBJS)
	ranlib $(LIB_NAME)


.PHONY: gen-bigint-types
gen-bigint-types:
	${ROOT_SCRIPTS_DIR}/gen_types.py \
		--src_files ${SDIR}/bigint_curve1174.c ${SDIR}/bigint.c \
		--tar_file ${IDIR}/bigint_types.h \
		--add_translation \
		--array_name "big_int_type_names" \
		--strip_prefix "BIGINT_TYPE_"

$(LIB_NAME): $(LIB_OBJS)

# This target first compiles all targets in this directory and then runs the tests
# in $REPO/tests
tests: all
	make -C $(TESTS_DIR) $(subst tests,,$(MAKECMDGOALS))

.PHONY: clean
clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ $(BIN_FILES)
