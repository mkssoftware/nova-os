NASM      ?= nasm
POWERSHELL ?= powershell.exe
BUILD_DIR ?= build

NASM_FLAGS := -Wall -Werror -w-reloc-abs-dword \
	-I../boot/bootloader/include/ -I../boot/include/

KERNEL_SOURCE  := arch/x86_64/entry32.asm
KERNEL_BINARY  := $(BUILD_DIR)/kernel.bin
KERNEL_ELF     := $(BUILD_DIR)/kernel.elf
LAYOUT_INCLUDE    := ../boot/bootloader/include/layout.inc
PROTOCOL_INCLUDE  := ../boot/include/nova_boot_protocol.inc
ART_INCLUDE       := ../boot/bootloader/include/nova-art.inc
KERNEL_INCLUDES   := $(wildcard arch/x86_64/*.inc) $(wildcard arch/x86_64/modules/*.inc)
BUILD_ELF_SCRIPT  := ../scripts/build-elf32.ps1

.PHONY: all clean verify

all: $(KERNEL_ELF) verify

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(KERNEL_BINARY): $(KERNEL_SOURCE) $(KERNEL_INCLUDES) $(LAYOUT_INCLUDE) $(PROTOCOL_INCLUDE) $(ART_INCLUDE) | $(BUILD_DIR)
	$(NASM) $(NASM_FLAGS) -f bin $< -o $@

$(KERNEL_ELF): $(KERNEL_BINARY)
	$(POWERSHELL) -NonInteractive -ExecutionPolicy Bypass -File $(BUILD_ELF_SCRIPT) -InputFile $(KERNEL_BINARY) -OutputFile $(KERNEL_ELF)

verify: $(KERNEL_BINARY)
	test "$$(wc -c < $(KERNEL_BINARY))" -gt 0
	# 1 MiB Kernelcode-Budget; BIOS-Slot (layout.inc) und UEFI-Loader setzen keine engere Grenze.
	test "$$(wc -c < $(KERNEL_BINARY))" -le 1048576

clean:
	rm -f $(KERNEL_BINARY) $(KERNEL_ELF)
