NASM ?= C:\Users\Matze\AppData\Local\bin\NASM\nasm.exe
QEMU ?= C:\Program Files\qemu\qemu-system-i386.exe
QEMU64 ?= C:\Program Files\qemu\qemu-system-x86_64.exe
HOST_CC ?= /ucrt64/bin/gcc

BUILD_DIR := build
BOOT_BUILD_DIR := boot/bootloader/build
KERNEL_BUILD_DIR := kernel/build

STAGE1 := $(BOOT_BUILD_DIR)/boot1.bin
STAGE2 := $(BOOT_BUILD_DIR)/boot2.bin
KERNEL := $(KERNEL_BUILD_DIR)/kernel.bin
KERNEL_ELF := $(KERNEL_BUILD_DIR)/kernel.elf
KERNEL_IMAGE := $(BUILD_DIR)/kernel.nki
DISK_IMAGE := $(BUILD_DIR)/nova-bios.img
SERIAL_LOG := $(BUILD_DIR)/qemu-serial.log
DEBUG_LOG := $(BUILD_DIR)/qemu-debug.log
CORRUPT_IMAGE := $(BUILD_DIR)/nova-bios-corrupt.img
CORRUPT_DEBUG_LOG := $(BUILD_DIR)/qemu-corrupt-debug.log
DIRECT_ELF_IMAGE := $(BUILD_DIR)/nova-bios-direct-elf.img
DIRECT_ELF_SERIAL_LOG := $(BUILD_DIR)/qemu-direct-elf-serial.log
INVALID_ELF_IMAGE := $(BUILD_DIR)/nova-bios-invalid-elf.img
INVALID_ELF_DEBUG_LOG := $(BUILD_DIR)/qemu-invalid-elf-debug.log
BUILD_ID_MISMATCH_IMAGE := $(BUILD_DIR)/nova-bios-build-id-mismatch.img
BUILD_ID_MISMATCH_LOG := $(BUILD_DIR)/qemu-build-id-mismatch.log
ELF64_RAW := $(KERNEL_BUILD_DIR)/kernel64-test.bin
ELF64_TEST := $(KERNEL_BUILD_DIR)/kernel64-test.elf
ELF64_TEST_IMAGE := $(BUILD_DIR)/nova-bios-elf64.img
ELF64_TEST_LOG := $(BUILD_DIR)/qemu-elf64-serial.log
ELF64_TEST_DEBUG := $(BUILD_DIR)/qemu-elf64-debug.log

IMAGE_SECTORS := 2880
KERNEL_LBA := 49

.PHONY: all abi-check artifact-check bootloader kernel image run test test-elf test-elf64 test-elf-invalid test-elf-validation test-build-id test-corrupt clean

all: image

abi-check:
	PATH=/ucrt64/bin:/usr/bin "$(HOST_CC)" -std=c11 -Wall -Wextra -Werror -fsyntax-only \
		tests/boot_protocol_layout.c

bootloader:
	$(MAKE) -C boot/bootloader NASM=$(NASM)

kernel:
	$(MAKE) -C kernel NASM=$(NASM)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(KERNEL_ELF): $(KERNEL) scripts/build-elf32.ps1
	powershell.exe -NoProfile -ExecutionPolicy Bypass \
		-File scripts/build-elf32.ps1 \
		-InputFile $(KERNEL) \
		-OutputFile $(KERNEL_ELF)

$(KERNEL_IMAGE): $(KERNEL_ELF) scripts/build-nki.ps1 | $(BUILD_DIR)
	powershell.exe -NoProfile -ExecutionPolicy Bypass \
		-File scripts/build-nki.ps1 \
		-InputFile $(KERNEL_ELF) \
		-OutputFile $(KERNEL_IMAGE)

artifact-check: $(KERNEL_IMAGE) scripts/validate-kernel-artifacts.ps1
	powershell.exe -NoProfile -ExecutionPolicy Bypass \
		-File scripts/validate-kernel-artifacts.ps1 \
		-ElfFile $(KERNEL_ELF) \
		-NkiFile $(KERNEL_IMAGE)

$(ELF64_RAW): kernel/arch/x86_64/entry64.asm
	"$(NASM)" -Wall -Werror -w-reloc-abs-dword \
		-Iboot/bootloader/include/ -Iboot/include/ -f bin $< -o $@

$(ELF64_TEST): $(ELF64_RAW) scripts/build-elf64.ps1
	powershell.exe -NoProfile -ExecutionPolicy Bypass \
		-File scripts/build-elf64.ps1 -InputFile $(ELF64_RAW) -OutputFile $(ELF64_TEST)

image: abi-check bootloader kernel $(KERNEL_IMAGE) artifact-check | $(BUILD_DIR)
	dd if=/dev/zero of=$(DISK_IMAGE) bs=512 count=$(IMAGE_SECTORS) status=none
	dd if=$(STAGE1) of=$(DISK_IMAGE) bs=512 seek=0 conv=notrunc status=none
	dd if=$(STAGE2) of=$(DISK_IMAGE) bs=512 seek=1 conv=notrunc status=none
	dd if=$(KERNEL_IMAGE) of=$(DISK_IMAGE) bs=512 seek=$(KERNEL_LBA) conv=notrunc status=none
	test "$$(wc -c < $(DISK_IMAGE))" -eq 1474560
	@echo "Nova BIOS image: $(DISK_IMAGE)"

run: image
	"$(QEMU)" \
		-drive format=raw,file=$(DISK_IMAGE),if=ide \
		-vga std \
		-serial stdio \
		-no-reboot \
		-no-shutdown

test: image
	rm -f $(SERIAL_LOG) $(DEBUG_LOG)
	status=0; (sleep 2; echo "sendkey d"; sleep 1; echo "sendkey r"; \
		sleep 1; echo "sendkey ret"; sleep 3; echo "quit") | \
		timeout 8s "$(QEMU)" \
		-drive format=raw,file=$(DISK_IMAGE),if=ide \
		-vga std \
		-display none \
		-monitor stdio \
		-serial file:$(SERIAL_LOG) \
		-debugcon file:$(DEBUG_LOG) \
		-no-reboot \
		-no-shutdown || status=$$?; \
		test "$$status" -eq 0 -o "$$status" -eq 124
	grep -F "NOVA: NBHP/BIB v1 validiert" $(SERIAL_LOG)
	grep -F "NOVA: Panic Reporter ABI 1.0 bereit" $(SERIAL_LOG)
	grep -F "NOVA: PMM ABI 1.0 und Seitentest bereit" $(SERIAL_LOG)
	grep -F "NOVA: Heap ABI 1.0 und Schreibtest bereit" $(SERIAL_LOG)
	grep -F "NOVA: Object Manager ABI 1.0 bereit" $(SERIAL_LOG)
	grep -F "NOVA: Component Manager ABI 1.0 bereit" $(SERIAL_LOG)
	grep -F "NOVA: Paging ABI 1.0 und Speichertest bereit" $(SERIAL_LOG)
	grep -F "NOVA: IDT, PIC und PIT 100 Hz aktiv" $(SERIAL_LOG)
	grep -F "NOVA: IPC ABI 1.0 FIFO bereit" $(SERIAL_LOG)
	grep -F "NOVA: Service Manager ABI 1.0 bereit" $(SERIAL_LOG)
	grep -F "NOVA: Process Manager ABI 1.0 bereit" $(SERIAL_LOG)
	grep -F "NOVA: Security ABI 1.0 Capabilities aktiv" $(SERIAL_LOG)
	grep -F "NOVA: Thread Manager ABI 1.0 bereit" $(SERIAL_LOG)
	grep -F "NOVA: Scheduler ABI 1.0 und zwei Threads aktiv" $(SERIAL_LOG)
	grep -F "NOVA_KERNEL_READY" $(SERIAL_LOG)
	grep -F "BM:DIAGNOSTICS" $(DEBUG_LOG)
	grep -F "BM:RECOVERY-UNAVAILABLE" $(DEBUG_LOG)
	grep -F "BM:START" $(DEBUG_LOG)
	@echo "QEMU BIOS smoke test erfolgreich"

test-elf: image $(KERNEL_ELF)
	cp $(DISK_IMAGE) $(DIRECT_ELF_IMAGE)
	dd if=$(KERNEL_ELF) of=$(DIRECT_ELF_IMAGE) bs=512 seek=$(KERNEL_LBA) conv=notrunc status=none
	rm -f $(DIRECT_ELF_SERIAL_LOG)
	status=0; (sleep 18; echo "quit") | \
		timeout 20s "$(QEMU)" \
		-drive format=raw,file=$(DIRECT_ELF_IMAGE),if=ide \
		-vga std -display none -monitor stdio \
		-serial file:$(DIRECT_ELF_SERIAL_LOG) \
		-no-reboot -no-shutdown || status=$$?; \
		test "$$status" -eq 0 -o "$$status" -eq 124
	sleep 1
	grep -F "NOVA: NBHP/BIB v1 validiert" $(DIRECT_ELF_SERIAL_LOG)
	grep -F "NOVA_KERNEL_READY" $(DIRECT_ELF_SERIAL_LOG)
	@echo "Direkter ELF32-Boot erfolgreich"

test-elf64: image $(ELF64_TEST)
	cp $(DISK_IMAGE) $(ELF64_TEST_IMAGE)
	dd if=$(ELF64_TEST) of=$(ELF64_TEST_IMAGE) bs=512 seek=$(KERNEL_LBA) conv=notrunc status=none
	rm -f $(ELF64_TEST_LOG) $(ELF64_TEST_DEBUG)
	status=0; (sleep 18; echo "quit") | \
		timeout 20s "$(QEMU64)" \
		-drive format=raw,file=$(ELF64_TEST_IMAGE),if=ide \
		-cpu qemu64 \
		-vga std -display none -monitor stdio \
		-serial file:$(ELF64_TEST_LOG) \
		-debugcon file:$(ELF64_TEST_DEBUG) \
		-no-reboot -no-shutdown || status=$$?; \
		test "$$status" -eq 0 -o "$$status" -eq 124
	sleep 1
	grep -F "NOVA_ELF64_LONG_MODE_READY" $(ELF64_TEST_DEBUG)
	@echo "Direkter ELF64-Long-Mode-Boot erfolgreich"

test-elf-invalid: test-elf
	powershell.exe -NoProfile -ExecutionPolicy Bypass \
		-File scripts/corrupt-image.ps1 \
		-InputFile $(DIRECT_ELF_IMAGE) \
		-OutputFile $(INVALID_ELF_IMAGE) \
		-Offset $$(( $(KERNEL_LBA) * 512 + 18 ))
	rm -f $(INVALID_ELF_DEBUG_LOG)
	status=0; timeout 5s "$(QEMU)" \
		-drive format=raw,file=$(INVALID_ELF_IMAGE),if=ide \
		-vga std -display none -monitor none -serial none \
		-debugcon file:$(INVALID_ELF_DEBUG_LOG) \
		-no-reboot -no-shutdown || status=$$?; \
		test "$$status" -eq 0 -o "$$status" -eq 124
	grep -F "BOOT-1002: Kernel-Header ungueltig" $(INVALID_ELF_DEBUG_LOG)
	@echo "Ungültiges ELF32 kontrolliert abgewiesen"

test-elf-validation: image $(KERNEL_ELF)
	@set -e; for mode in overlap bounds wx note cpu; do \
		case "$$mode" in \
			overlap) mutation=Overlap ;; \
			bounds) mutation=Bounds ;; \
			wx) mutation=WritableExecutable ;; \
			note) mutation=MalformedNote ;; \
			cpu) mutation=UnsupportedCpu ;; \
		esac; \
		powershell.exe -NoProfile -ExecutionPolicy Bypass \
			-File scripts/mutate-elf32.ps1 \
			-InputFile $(KERNEL_ELF) \
			-OutputFile $(BUILD_DIR)/invalid-$$mode.elf \
			-Mode $$mutation; \
		cp $(DISK_IMAGE) $(BUILD_DIR)/invalid-$$mode.img; \
		dd if=$(BUILD_DIR)/invalid-$$mode.elf \
			of=$(BUILD_DIR)/invalid-$$mode.img bs=512 seek=$(KERNEL_LBA) conv=notrunc status=none; \
		rm -f $(BUILD_DIR)/invalid-$$mode.log; \
		status=0; timeout 5s "$(QEMU)" \
			-drive format=raw,file=$(BUILD_DIR)/invalid-$$mode.img,if=ide \
			-vga std -display none -monitor none -serial none \
			-debugcon file:$(BUILD_DIR)/invalid-$$mode.log \
			-no-reboot -no-shutdown || status=$$?; \
		test "$$status" -eq 0 -o "$$status" -eq 124; \
		grep -F "BOOT-1002: Kernel-Header ungueltig" $(BUILD_DIR)/invalid-$$mode.log; \
	done
	@echo "ELF32-Grenzen, Segmentueberlappung, W^X, Notes und CPU-Anforderungen geprueft"

test-corrupt: image
	powershell.exe -NoProfile -ExecutionPolicy Bypass \
		-File scripts/corrupt-image.ps1 \
		-InputFile $(DISK_IMAGE) \
		-OutputFile $(CORRUPT_IMAGE) \
		-Offset $$(( $(KERNEL_LBA) * 512 + 128 ))
	rm -f $(CORRUPT_DEBUG_LOG)
	status=0; timeout 5s "$(QEMU)" \
		-drive format=raw,file=$(CORRUPT_IMAGE),if=ide \
		-vga std \
		-display none \
		-monitor none \
		-serial none \
		-debugcon file:$(CORRUPT_DEBUG_LOG) \
		-no-reboot \
		-no-shutdown || status=$$?; \
		test "$$status" -eq 0 -o "$$status" -eq 124
	grep -F "BOOT-1002: Kernel-Pruefsumme ungueltig" $(CORRUPT_DEBUG_LOG)
	@echo "QEMU Korruptionstest erfolgreich"

test-build-id: image
	powershell.exe -NoProfile -ExecutionPolicy Bypass \
		-File scripts/corrupt-image.ps1 \
		-InputFile $(DISK_IMAGE) \
		-OutputFile $(BUILD_ID_MISMATCH_IMAGE) \
		-Offset $$(( $(KERNEL_LBA) * 512 + 44 ))
	rm -f $(BUILD_ID_MISMATCH_LOG)
	status=0; timeout 5s "$(QEMU)" \
		-drive format=raw,file=$(BUILD_ID_MISMATCH_IMAGE),if=ide \
		-vga std -display none -monitor none -serial none \
		-debugcon file:$(BUILD_ID_MISMATCH_LOG) \
		-no-reboot -no-shutdown || status=$$?; \
		test "$$status" -eq 0 -o "$$status" -eq 124
	grep -F "BOOT-1002: Kernel-Header ungueltig" $(BUILD_ID_MISMATCH_LOG)
	@echo "Abweichende NKI/ELF-Build-ID kontrolliert abgewiesen"

clean:
	$(MAKE) -C boot/bootloader clean
	$(MAKE) -C kernel clean
	rm -f $(KERNEL_IMAGE) $(DISK_IMAGE) $(SERIAL_LOG) $(DEBUG_LOG) \
		$(KERNEL_ELF) $(CORRUPT_IMAGE) $(CORRUPT_DEBUG_LOG) \
		$(DIRECT_ELF_IMAGE) $(DIRECT_ELF_SERIAL_LOG) \
		$(INVALID_ELF_IMAGE) $(INVALID_ELF_DEBUG_LOG) \
		$(BUILD_ID_MISMATCH_IMAGE) $(BUILD_ID_MISMATCH_LOG) \
		$(ELF64_RAW) $(ELF64_TEST) $(ELF64_TEST_IMAGE) $(ELF64_TEST_LOG) $(ELF64_TEST_DEBUG) \
		$(BUILD_DIR)/invalid-overlap.elf $(BUILD_DIR)/invalid-overlap.img $(BUILD_DIR)/invalid-overlap.log \
		$(BUILD_DIR)/invalid-bounds.elf $(BUILD_DIR)/invalid-bounds.img $(BUILD_DIR)/invalid-bounds.log \
		$(BUILD_DIR)/invalid-wx.elf $(BUILD_DIR)/invalid-wx.img $(BUILD_DIR)/invalid-wx.log \
		$(BUILD_DIR)/invalid-note.elf $(BUILD_DIR)/invalid-note.img $(BUILD_DIR)/invalid-note.log \
		$(BUILD_DIR)/invalid-cpu.elf $(BUILD_DIR)/invalid-cpu.img $(BUILD_DIR)/invalid-cpu.log
