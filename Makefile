NASM ?= C:\Users\Matze\AppData\Local\bin\NASM\nasm.exe
QEMU ?= C:\Program Files\qemu\qemu-system-i386.exe

BUILD_DIR := build
BOOT_BUILD_DIR := boot/bootloader/build
KERNEL_BUILD_DIR := kernel/build

STAGE1 := $(BOOT_BUILD_DIR)/boot1.bin
STAGE2 := $(BOOT_BUILD_DIR)/boot2.bin
KERNEL := $(KERNEL_BUILD_DIR)/kernel.bin
KERNEL_IMAGE := $(BUILD_DIR)/kernel.nki
DISK_IMAGE := $(BUILD_DIR)/nova-bios.img
SERIAL_LOG := $(BUILD_DIR)/qemu-serial.log
DEBUG_LOG := $(BUILD_DIR)/qemu-debug.log
CORRUPT_IMAGE := $(BUILD_DIR)/nova-bios-corrupt.img
CORRUPT_DEBUG_LOG := $(BUILD_DIR)/qemu-corrupt-debug.log

IMAGE_SECTORS := 2880
KERNEL_LBA := 41

.PHONY: all bootloader kernel image run test test-corrupt clean

all: image

bootloader:
	$(MAKE) -C boot/bootloader NASM=$(NASM)

kernel:
	$(MAKE) -C kernel NASM=$(NASM)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(KERNEL_IMAGE): $(KERNEL) scripts/build-nki.ps1 | $(BUILD_DIR)
	powershell.exe -NoProfile -ExecutionPolicy Bypass \
		-File scripts/build-nki.ps1 \
		-InputFile $(KERNEL) \
		-OutputFile $(KERNEL_IMAGE)

image: bootloader kernel $(KERNEL_IMAGE) | $(BUILD_DIR)
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

clean:
	$(MAKE) -C boot/bootloader clean
	$(MAKE) -C kernel clean
	rm -f $(KERNEL_IMAGE) $(DISK_IMAGE) $(SERIAL_LOG) $(DEBUG_LOG) \
		$(CORRUPT_IMAGE) $(CORRUPT_DEBUG_LOG)
