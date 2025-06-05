# Include directory
INCLDIR = ./include

# Compiler and flags
CC = gcc
CFLAGS = -O2 -include stdio.h 
CFLAGS += -I$(INCLDIR)
CFLAGS += -lpthread

# Object directory
OBJDIR = obj

# Dependencies
_DEPS = dbj2.h tea.h
DEPS = $(patsubst %,$(INCLDIR)/%,$(_DEPS))

# Object files
_OBJS = task4_threads.o dbj2.o tea.o
OBJS = $(patsubst %,$(OBJDIR)/%,$(_OBJS))

# Rule for compiling object files
$(OBJDIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

# Rule for building the final executable
task4: $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS)


# Clean rule
.PHONY: clean
clean:
	rm -f $(OBJDIR)/*.o *~ core $(INCLDIR)/*~

