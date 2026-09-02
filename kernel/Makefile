NASM ?= nasm
BUILD_DIR ?= build

NASM_FLAGS := -Wall -Werror -w-reloc-abs-dword \
	-I../boot/bootloader/include/ -I../boot/include/

KERNEL_SOURCE := arch/x86_64/entry32.asm
KERNEL_BINARY := $(BUILD_DIR)/kernel.bin
LAYOUT_INCLUDE := ../boot/bootloader/include/layout.inc
PROTOCOL_INCLUDE := ../boot/include/nova_boot_protocol.inc
ART_INCLUDE := ../boot/bootloader/include/nova-art.inc

.PHONY: all clean verify

all: $(KERNEL_BINARY) verify

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(KERNEL_BINARY): $(KERNEL_SOURCE) $(LAYOUT_INCLUDE) $(PROTOCOL_INCLUDE) $(ART_INCLUDE) | $(BUILD_DIR)
	$(NASM) $(NASM_FLAGS) -f bin $< -o $@

verify: $(KERNEL_BINARY)
	test "$$(wc -c < $(KERNEL_BINARY))" -gt 0
	test "$$(wc -c < $(KERNEL_BINARY))" -le 61440

clean:
	rm -f $(KERNEL_BINARY)
