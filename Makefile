
CC = gcc
LDFLAGS = -lraylib -lm -ldl -lrt -lpthread -lX11

SRC = main.c
OBJDIR = build
OBJ = $(OBJDIR)/$(SRC:.c=.o)
TARGET = $(OBJDIR)/game

.PHONY: all clean

all: $(TARGET)

# Link final binary
$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

# Compile .c into .o inside build/
$(OBJDIR)/%.o: %.c | $(OBJDIR)
	$(CC) -c $< -o $@

# Create build directory if it doesn't exist
$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	rm -rf $(OBJDIR)

