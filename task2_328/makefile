# Compiler and flags
CC = gcc
CFLAGS = -O2 -include stdio.h #optimaliserer kode 
CFLAGS += -I$(INCLDIR)

# Include directory
INCLDIR = ./include
# Include src
SRC = ./src
# Object directory
OBJDIR = ./obj

# Dependencies
_DEPS = debugger.h task2_count.h task2_sum.h task2_hash.h
DEPS = $(patsubst %,$(INCLDIR)/%,$(_DEPS))

# Object files
_OBJS = main.o debugger.o task2_count.o task2_sum.o task2_hash.o
OBJS = $(patsubst %,$(OBJDIR)/%,$(_OBJS))

# Rule for compiling object files
$(OBJDIR)/%.o: $(SRC)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

# Rule for building the final executable
task2: $(OBJS)
	$(CC)  -o $@ $^ $(CFLAGS)


# Clean rule
.PHONY: clean
clean:
	rm -f $(OBJDIR)/*.o *~ core $(INCLDIR)/*~

