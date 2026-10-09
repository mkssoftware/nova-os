# NovaLang Compiler – Makefile
# Build: make   (erzeugt novalang.exe)
# Ergebnis wird auch in toolchain/NovaStudio/ kopiert

CC      = gcc
CFLAGS  = -std=c11 -O2 -Wall -Wextra \
          -Wno-missing-field-initializers \
          -Wno-unused-parameter \
          -DNOVALANG_VERSION=\"0.1.0\"

SRCS = src/main.c \
       src/compiler.c \
       src/lexer.c \
       src/ast.c \
       src/parser.c \
       src/diagnostic.c \
       src/typechecker.c \
       src/ir.c \
       src/bytecode.c

TARGET  = novalang.exe
STUDIO  = ../NovaStudio/$(TARGET)

all: $(TARGET) copy-to-studio

$(TARGET): $(SRCS) src/compiler.h
	$(CC) $(CFLAGS) -Wno-sign-compare -o $@ $(SRCS)
	@echo "Gebaut: $(TARGET)"

copy-to-studio: $(TARGET)
	@cp -f $(TARGET) $(STUDIO) && echo "Kopiert nach: $(STUDIO)" || echo "Hinweis: Kopie nach $(STUDIO) fehlgeschlagen"

clean:
	-rm -f $(TARGET)

.PHONY: all copy-to-studio clean
