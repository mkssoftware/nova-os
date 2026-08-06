NASM ?= nasm
BUILD_DIR ?= build

# Flat 16-Bit-Binaries mit ORG erzeugen bei NASM 2.16 eine bekannte
# reloc-abs-word-Warnung für lokale absolute Adressen. Für diesen Binärtyp
# ist sie beabsichtigt; alle anderen Warnungen bleiben Fehler.
NASM_FLAGS := -Wall -Werror -w-reloc-abs-word -w-reloc-abs-dword -Iinclude/ -I../include/

STAGE1_SOURCE := bios/boot1.asm
STAGE2_SOURCE := bios/boot2.asm
LAYOUT_INCLUDE := include/layout.inc
PROTOCOL_INCLUDE := ../include/nova_boot_protocol.inc
FONT_INCLUDE := include/boot-font-aa.inc
ART_INCLUDE := include/nova-art.inc
UI_TOKENS_INCLUDE := include/boot-ui-tokens.inc

STAGE1_BINARY := $(BUILD_DIR)/boot1.bin
STAGE2_BINARY := $(BUILD_DIR)/boot2.bin

.PHONY: all clean verify

all: $(STAGE1_BINARY) $(STAGE2_BINARY) verify

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(STAGE1_BINARY): $(STAGE1_SOURCE) $(LAYOUT_INCLUDE) $(PROTOCOL_INCLUDE) | $(BUILD_DIR)
	$(NASM) $(NASM_FLAGS) -f bin $< -o $@

$(STAGE2_BINARY): $(STAGE2_SOURCE) $(LAYOUT_INCLUDE) $(PROTOCOL_INCLUDE) $(FONT_INCLUDE) $(ART_INCLUDE) $(UI_TOKENS_INCLUDE) | $(BUILD_DIR)
	$(NASM) $(NASM_FLAGS) -f bin $< -o $@

verify: $(STAGE1_BINARY) $(STAGE2_BINARY)
	test "$$(wc -c < $(STAGE1_BINARY))" -eq 512
	test "$$(wc -c < $(STAGE2_BINARY))" -eq 32768
	test "$$(od -An -tx1 -j510 -N2 $(STAGE1_BINARY) | tr -d ' \\n')" = "55aa"

clean:
	rm -f $(STAGE1_BINARY) $(STAGE2_BINARY)
