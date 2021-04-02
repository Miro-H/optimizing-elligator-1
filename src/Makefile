IDIR    	= ../include
LDIR		= ../lib
BDIR		= ../bin
ODIR    	= obj
SDIR		= .
TESTS_DIR 	= ../tests
BIN_FILES   = $(filter-out $(wildcard $(BDIR)/.*), $(wildcard $(BDIR)/*))

CC      = gcc
CFLAGS  = -I$(IDIR)

# Add additional compilation flags to improve code quality
CFLAGS += -Wall -Werror

# Add libraries to this macro, if needed
LIBS    =

LIB_NAME 	= $(LDIR)/elligator.a
# Add the name of every .c file that you want to compile here
_LIB_SRCS	= elligator.c
LIB_SRCS 	= $(patsubst %,$(SDIR)/%,$(_LIB_SRCS))
# This assumes the library depends on all header files in IDIR
LIB_HEADERS = $(wildcard $(IDIR)/*.h)
LIB_OBJS 	= $(patsubst %,$(ODIR)/%,$(_LIB_SRCS:.c=.o))


# This target is run whenever make is called without any additional
# arguments. Make sure it always compiles the main project.
all: $(LIB_NAME) build-lib

$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

# Fresh build
rebuild: clean all

# Build library
build-lib:
	ar rcs $(LIB_NAME) $(LIB_HEADERS) $(LIB_OBJS)
	ranlib $(LIB_NAME)

$(LIB_NAME): $(LIB_OBJS)

# This target first compiles all targets in this directory and then runs the tests
# in $REPO/tests
tests: all
	make -C $(TESTS_DIR) $(subst tests,,$(MAKECMDGOALS))

.PHONY: clean
clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ $(BIN_FILES)
