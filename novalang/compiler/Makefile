# NovaLang v0.1 Compiler (novac) - Host-Build (Windows/MinGW gcc).
# Siehe ../specifications/NOVALANG_SPEC_v0.1.md fuer den Scope.
# Build: make   (erzeugt novac.exe)

CC     = gcc
CFLAGS = -std=c11 -O2 -Wall -Wextra -Wno-unused-parameter

SRCS = src/main.c src/lexer.c src/parser.c src/codegen_x86.c
TARGET = novac.exe

all: $(TARGET)

$(TARGET): $(SRCS) src/lexer.h src/parser.h src/ast.h src/codegen_x86.h
	$(CC) $(CFLAGS) -o $@ $(SRCS)
	@echo "Gebaut: $(TARGET)"

clean:
	-rm -f $(TARGET)

.PHONY: all clean
