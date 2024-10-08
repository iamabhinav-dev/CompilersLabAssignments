# Makefile for Bison and Flex project

# Compiler
CC = gcc

# Flex and Bison tools
FLEX = flex
BISON = bison

# Flags
CFLAGS = -Wall -g

# Output executable
EXEC = poly

# Source files
LEX_FILE = poly.l
BISON_FILE = poly.y
UTILS_FILE = polyutils.c

# Generated files
LEX_OUTPUT = lex.yy.c
BISON_OUTPUT = poly.tab.c
BISON_HEADER = poly.tab.h

# Header files
HEADERS = poly.h $(BISON_HEADER)

# Input and output files
INPUT_FILE = sample.txt
OUTPUT_FILE = output.txt

# Rule to build the executable
$(EXEC): $(LEX_OUTPUT) $(BISON_OUTPUT) $(UTILS_FILE)
	$(CC) $(CFLAGS) -o $(EXEC) $(LEX_OUTPUT) $(BISON_OUTPUT) $(UTILS_FILE) -lfl

# Rule to generate the Lex output file
$(LEX_OUTPUT): $(LEX_FILE)
	$(FLEX) $(LEX_FILE)

# Rule to generate the Bison output file and header file
$(BISON_OUTPUT) $(BISON_HEADER): $(BISON_FILE)
	$(BISON) -d $(BISON_FILE)

# Add dependency for polyutils.c to ensure it includes the Bison header
$(UTILS_FILE): $(BISON_HEADER)

# Rule to run the program
run: $(EXEC)
	./$(EXEC) < $(INPUT_FILE) > $(OUTPUT_FILE)

# Clean rule to remove generated files
clean:
	rm -f $(EXEC) $(LEX_OUTPUT) $(BISON_OUTPUT) $(BISON_HEADER) $(OUTPUT_FILE)

.PHONY: clean run
