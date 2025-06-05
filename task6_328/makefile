# Compiler and flags
CC = gcc
CFLAGS = -O2 #optimaliserer kode 
CFLAGS += -I$(INCLDIR)

# Include directory
INCLDIR = ./include
# Include src
SRC = ./src
# Object directory
OBJDIR = ./obj

# Dependencies
_DEPS = debugger.h TEA.h httpClient.h decrypt.h
DEPS = $(patsubst %,$(INCLDIR)/%,$(_DEPS))

# Object files
_OBJS = main.o debugger.o TEA.o httpClient.o decrypt.o
OBJS = $(patsubst %,$(OBJDIR)/%,$(_OBJS))

# Rule for compiling object files
$(OBJDIR)/%.o: $(SRC)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

# Rule for building the final executable
task6: $(OBJS)
	$(CC)  -o $@ $^ $(CFLAGS)


# Clean rule
.PHONY: clean
clean:
	rm -f $(OBJDIR)/*.o *~ core $(INCLDIR)/*~

