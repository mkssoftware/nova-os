NASM ?= C:\Users\Matze\AppData\Local\bin\NASM\nasm.exe
QEMU ?= C:\Program Files\qemu\qemu-system-i386.exe
QEMU64 ?= C:\Program Files\qemu\qemu-system-x86_64.exe
HOST_CC ?= /ucrt64/bin/gcc
UEFI_EXTRA_CFLAGS ?=

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
MOUSE_SERIAL_LOG := $(BUILD_DIR)/qemu-mouse-serial.log
MOUSE_DEBUG_LOG := $(BUILD_DIR)/qemu-mouse-debug.log
THEME_SERIAL_LOG := $(BUILD_DIR)/qemu-theme-serial.log
THEME_DEBUG_LOG := $(BUILD_DIR)/qemu-theme-debug.log
FLOWS_SERIAL_LOG := $(BUILD_DIR)/qemu-flows-serial.log
FLOWS_DEBUG_LOG := $(BUILD_DIR)/qemu-flows-debug.log
RECOVERY_SERIAL_LOG := $(BUILD_DIR)/qemu-recovery-serial.log
RECOVERY_DEBUG_LOG := $(BUILD_DIR)/qemu-recovery-debug.log
PLATFORM_SERIAL_LOG := $(BUILD_DIR)/qemu-platform-serial.log
PLATFORM_DEBUG_LOG := $(BUILD_DIR)/qemu-platform-debug.log
UEFI_DIR := $(BUILD_DIR)/uefi
UEFI_APP := $(UEFI_DIR)/EFI/BOOT/BOOTX64.EFI
UEFI_FIRMWARE := $(UEFI_DIR)/edk2-x86_64.fd
UEFI_DEBUG_LOG := $(BUILD_DIR)/qemu-uefi-debug.log
UEFI_INPUT_DEBUG_LOG := $(BUILD_DIR)/qemu-uefi-input-debug.log
UEFI_POWER_DEBUG_LOG := $(BUILD_DIR)/qemu-uefi-power-debug.log
UEFI_THEME_DEBUG_LOG := $(BUILD_DIR)/qemu-uefi-theme-debug.log
UEFI_RESOLUTION_DEBUG_LOG := $(BUILD_DIR)/qemu-uefi-resolution-debug.log
FONT_C_HEADER := $(BUILD_DIR)/generated/boot_font_data.h
ICON_C_HEADER := $(BUILD_DIR)/generated/boot_icon_data.h
ART_C_HEADER := $(BUILD_DIR)/generated/boot_art_data.h
EDK2_CODE ?= C:/Program Files/qemu/share/edk2-x86_64-code.fd
EDK2_VARS ?= C:/Program Files/qemu/share/edk2-i386-vars.fd
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
KERNEL_LBA := 65

.PHONY: all abi-check boot-ui-runtime-check uefi-firmware-runtime-check artifact-check bootloader kernel image uefi uefi-image test-uefi-image run test test-uefi test-uefi-input test-uefi-dialog test-uefi-context test-uefi-tooltip-breadcrumb test-uefi-settings-controls test-uefi-list-controls test-uefi-help-search test-uefi-firmware test-uefi-progress test-uefi-scrollview test-uefi-recovery-tiles test-uefi-ui-recovery test-uefi-power test-uefi-themes test-uefi-resolutions test-mouse test-theme test-ui-flows test-recovery test-platform test-elf test-elf64 test-elf-invalid test-elf-validation test-build-id test-corrupt clean

all: image

abi-check:
	PATH=/ucrt64/bin:/usr/bin "$(HOST_CC)" -std=c11 -Wall -Wextra -Werror -fsyntax-only \
		tests/boot_protocol_layout.c

$(FONT_C_HEADER): boot/bootloader/include/boot-font-aa.inc scripts/convert-boot-font-to-c.ps1 | $(BUILD_DIR)
	powershell.exe -NoProfile -ExecutionPolicy Bypass \
		-File scripts/convert-boot-font-to-c.ps1 -InputFile $< -OutputFile $@

$(ICON_C_HEADER): boot/bootloader/include/boot-icons-aa.inc scripts/convert-boot-icons-to-c.ps1 | $(BUILD_DIR)
	powershell.exe -NoProfile -ExecutionPolicy Bypass \
		-File scripts/convert-boot-icons-to-c.ps1 -InputFile $< -OutputFile $@

$(ART_C_HEADER): boot/bootloader/include/nova-art.inc scripts/convert-boot-art-to-c.ps1 | $(BUILD_DIR)
	powershell.exe -NoProfile -ExecutionPolicy Bypass \
		-File scripts/convert-boot-art-to-c.ps1 -InputFile $< -OutputFile $@

boot-ui-runtime-check: $(FONT_C_HEADER) $(ICON_C_HEADER) $(ART_C_HEADER) | $(BUILD_DIR)
	PATH=/ucrt64/bin:/usr/bin TMP=$(abspath $(BUILD_DIR)) TEMP=$(abspath $(BUILD_DIR)) \
		"$(HOST_CC)" -O2 -std=c11 -Wall -Wextra -Werror \
		-Iboot/bootloader/bootmenu -I$(BUILD_DIR)/generated tests/boot_ui_runtime.c \
		boot/bootloader/bootmenu/motion.c boot/bootloader/bootmenu/compositor.c \
		boot/bootloader/bootmenu/controls.c boot/bootloader/bootmenu/text.c \
		boot/bootloader/bootmenu/unicode.c boot/bootloader/bootmenu/resources.c \
		boot/bootloader/bootmenu/icons.c \
		boot/bootloader/bootmenu/branding.c \
		boot/bootloader/bootmenu/theme.c \
		boot/bootloader/bootmenu/layout.c \
		boot/bootloader/bootmenu/input.c \
		boot/bootloader/bootmenu/diagnostics.c \
		boot/bootloader/bootmenu/recovery.c \
		boot/bootloader/bootmenu/memory.c \
		boot/bootloader/bootmenu/navigation.c \
		boot/bootloader/bootmenu/dialog.c \
		boot/bootloader/bootmenu/page.c \
		-o $(BUILD_DIR)/boot-ui-runtime-test.exe
	$(BUILD_DIR)/boot-ui-runtime-test.exe

uefi-firmware-runtime-check: | $(BUILD_DIR)
	PATH=/ucrt64/bin:/usr/bin TMP=$(abspath $(BUILD_DIR)) TEMP=$(abspath $(BUILD_DIR)) \
		"$(HOST_CC)" -O2 -std=c11 -Wall -Wextra -Werror -DNOVA_HOST_TEST \
		-Iboot/bootloader/uefi tests/uefi_firmware_runtime.c \
		boot/bootloader/uefi/firmware.c -o $(BUILD_DIR)/uefi-firmware-runtime-test.exe
	$(BUILD_DIR)/uefi-firmware-runtime-test.exe

bootloader:
	$(MAKE) -C boot/bootloader NASM=$(NASM)

kernel:
	$(MAKE) -C kernel NASM=$(NASM)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(UEFI_APP): boot/bootloader/uefi/main.c boot/bootloader/uefi/graphics.c boot/bootloader/uefi/pointer.c boot/bootloader/uefi/power.c boot/bootloader/uefi/firmware.c \
		boot/bootloader/uefi/uefi_min.h boot/bootloader/bootmenu/ui.c \
		boot/bootloader/bootmenu/ui.h \
		boot/bootloader/bootmenu/motion.c boot/bootloader/bootmenu/motion.h \
		boot/bootloader/bootmenu/compositor.c boot/bootloader/bootmenu/compositor.h \
		boot/bootloader/bootmenu/controls.c boot/bootloader/bootmenu/controls.h \
		boot/bootloader/bootmenu/text.c boot/bootloader/bootmenu/text.h \
		boot/bootloader/bootmenu/unicode.c boot/bootloader/bootmenu/unicode.h \
		boot/bootloader/bootmenu/resources.c boot/bootloader/bootmenu/resources.h \
		boot/bootloader/bootmenu/icons.c boot/bootloader/bootmenu/icons.h \
		boot/bootloader/bootmenu/branding.c boot/bootloader/bootmenu/branding.h \
		boot/bootloader/bootmenu/theme.c boot/bootloader/bootmenu/theme.h \
		boot/bootloader/bootmenu/layout.c boot/bootloader/bootmenu/layout.h \
		boot/bootloader/bootmenu/input.c boot/bootloader/bootmenu/input.h \
		boot/bootloader/bootmenu/diagnostics.c boot/bootloader/bootmenu/diagnostics.h \
		boot/bootloader/bootmenu/recovery.c boot/bootloader/bootmenu/recovery.h \
		boot/bootloader/bootmenu/memory.c boot/bootloader/bootmenu/memory.h \
		boot/bootloader/bootmenu/navigation.c boot/bootloader/bootmenu/navigation.h \
		boot/bootloader/bootmenu/dialog.c boot/bootloader/bootmenu/dialog.h \
		boot/bootloader/bootmenu/page.c boot/bootloader/bootmenu/page.h \
		$(FONT_C_HEADER) $(ICON_C_HEADER) $(ART_C_HEADER) \
		boot/bootloader/uefi/startup.nsh | $(BUILD_DIR)
	mkdir -p $(UEFI_DIR)/EFI/BOOT
	PATH=/ucrt64/bin:/usr/bin TMP=$(abspath $(UEFI_DIR)) TEMP=$(abspath $(UEFI_DIR)) \
		"$(HOST_CC)" -O2 -std=c11 -Wall -Wextra -Werror -ffreestanding \
		-fno-stack-protector -fno-asynchronous-unwind-tables -mno-red-zone \
		$(UEFI_EXTRA_CFLAGS) \
		-nostdlib -Iboot/bootloader/uefi -I$(BUILD_DIR)/generated \
		boot/bootloader/uefi/main.c \
		boot/bootloader/uefi/graphics.c boot/bootloader/uefi/pointer.c boot/bootloader/uefi/power.c boot/bootloader/uefi/firmware.c boot/bootloader/bootmenu/ui.c \
		boot/bootloader/bootmenu/motion.c boot/bootloader/bootmenu/compositor.c \
		boot/bootloader/bootmenu/controls.c boot/bootloader/bootmenu/text.c \
		boot/bootloader/bootmenu/unicode.c boot/bootloader/bootmenu/resources.c \
		boot/bootloader/bootmenu/icons.c \
		boot/bootloader/bootmenu/branding.c \
		boot/bootloader/bootmenu/theme.c \
		boot/bootloader/bootmenu/layout.c \
		boot/bootloader/bootmenu/input.c \
		boot/bootloader/bootmenu/diagnostics.c \
		boot/bootloader/bootmenu/recovery.c \
		boot/bootloader/bootmenu/memory.c \
		boot/bootloader/bootmenu/navigation.c \
		boot/bootloader/bootmenu/dialog.c \
		boot/bootloader/bootmenu/page.c \
		-Wl,--subsystem,10 -Wl,--entry,efi_main -Wl,--image-base,0x400000 \
		-Wl,--dynamicbase -o $(UEFI_APP)
	cp boot/bootloader/uefi/startup.nsh $(UEFI_DIR)/startup.nsh

$(UEFI_FIRMWARE): scripts/compose-edk2-firmware.ps1 | $(BUILD_DIR)
	powershell.exe -NoProfile -ExecutionPolicy Bypass \
		-File scripts/compose-edk2-firmware.ps1 \
		-VariablesFile "$(EDK2_VARS)" -CodeFile "$(EDK2_CODE)" \
		-OutputFile $(UEFI_FIRMWARE)

uefi: $(UEFI_APP) $(UEFI_FIRMWARE)

uefi-image: uefi
	powershell.exe -NoProfile -ExecutionPolicy Bypass -File scripts/build-uefi-image.ps1 \
		-EfiApplication $(UEFI_APP) -OutputImage build/nova-uefi.img

test-uefi-image: uefi
	powershell.exe -NoProfile -ExecutionPolicy Bypass -File scripts/build-uefi-image.ps1 \
		-EfiApplication $(UEFI_APP) -OutputImage build/nova-uefi-test.img
	powershell.exe -NoProfile -ExecutionPolicy Bypass -File scripts/test-uefi-image.ps1 \
		-Qemu "$(QEMU64)" -Firmware $(UEFI_FIRMWARE) -Image build/nova-uefi-test.img \
		-DebugLog build/qemu-uefi-image-debug.log
	grep -F "UEFI:NOVA-ENTRY" build/qemu-uefi-image-debug.log
	grep -F "UEFI:COUNTDOWN-FRAME-READY" build/qemu-uefi-image-debug.log
	grep -F "UEFI:STYLE-TEMPLATE-FRAME-READY" build/qemu-uefi-image-debug.log
	@echo "Aktuelles GPT/FAT32-UEFI-IMG bootet erfolgreich"

test-uefi: boot-ui-runtime-check uefi
	rm -f $(UEFI_DEBUG_LOG)
	status=0; TMP=$(abspath $(BUILD_DIR)) TEMP=$(abspath $(BUILD_DIR)) \
		timeout 45s "$(QEMU64)" -machine q35 \
		-drive if=pflash,format=raw,snapshot=on,file=$(UEFI_FIRMWARE) \
		-drive format=raw,file=fat:rw:$(UEFI_DIR) \
		-display none -monitor none -serial none \
		-debugcon file:$(UEFI_DEBUG_LOG) -global isa-debugcon.iobase=0xe9 \
		-no-reboot -no-shutdown || status=$$?; \
		test "$$status" -eq 0 -o "$$status" -eq 124
	grep -F "UEFI:NOVA-ENTRY" $(UEFI_DEBUG_LOG)
	grep -F "UEFI:GOP-READY" $(UEFI_DEBUG_LOG)
	grep -F "UEFI:MENU-DRAWN" $(UEFI_DEBUG_LOG)
	grep -F "UEFI:COMPOSITOR-READY" $(UEFI_DEBUG_LOG)
	grep -F "UEFI:MOTION-READY" $(UEFI_DEBUG_LOG)
	grep -F "UEFI:CONTROLS-READY" $(UEFI_DEBUG_LOG)
	grep -F "UEFI:RESOURCES-READY" $(UEFI_DEBUG_LOG)
	grep -F "UEFI:BRANDING-READY" $(UEFI_DEBUG_LOG)
	grep -F "UEFI:INPUT-READY" $(UEFI_DEBUG_LOG)
	grep -F "UEFI:POINTER-READY" $(UEFI_DEBUG_LOG)
	grep -F "UEFI:POWER-READY" $(UEFI_DEBUG_LOG)
	grep -F "UEFI:DIAGNOSTICS-READY" $(UEFI_DEBUG_LOG)
	grep -F "UEFI:NAVIGATION-READY" $(UEFI_DEBUG_LOG)
	grep -F "UEFI:DIALOG-READY" $(UEFI_DEBUG_LOG)
	grep -F "UEFI:PAGES-READY" $(UEFI_DEBUG_LOG)
	grep -F "UEFI:COUNTDOWN-5" $(UEFI_DEBUG_LOG)
	grep -F "UEFI:COUNTDOWN-4" $(UEFI_DEBUG_LOG)
	grep -F "UEFI:COUNTDOWN-3" $(UEFI_DEBUG_LOG)
	grep -F "UEFI:COUNTDOWN-2" $(UEFI_DEBUG_LOG)
	grep -F "UEFI:COUNTDOWN-1" $(UEFI_DEBUG_LOG)
	grep -F "UEFI:START" $(UEFI_DEBUG_LOG)
	@echo "QEMU UEFI Bootmanager- und Countdown-Test erfolgreich"

test-uefi-input: uefi
	powershell.exe -NoProfile -ExecutionPolicy Bypass \
		-File scripts/test-uefi-input.ps1 -Qemu "$(QEMU64)" \
		-Firmware $(UEFI_FIRMWARE) -FatDirectory $(UEFI_DIR) \
		-DebugLog $(UEFI_INPUT_DEBUG_LOG)
	grep -F "UEFI:COUNTDOWN-CANCELLED" $(UEFI_INPUT_DEBUG_LOG)
	grep -F "UEFI:INSTALL-UNAVAILABLE" $(UEFI_INPUT_DEBUG_LOG)
	grep -F "UEFI:SETTINGS" $(UEFI_INPUT_DEBUG_LOG)
	grep -F "UEFI:DIAGNOSTICS-VIEW" $(UEFI_INPUT_DEBUG_LOG)
	grep -F "UEFI:RECOVERY-VIEW" $(UEFI_INPUT_DEBUG_LOG)
	grep -F "UEFI:POWER-VIEW" $(UEFI_INPUT_DEBUG_LOG)
	grep -F "UEFI:POWER-TARGET-UNAVAILABLE" $(UEFI_INPUT_DEBUG_LOG)
	grep -F "UEFI:DIALOG-OPEN" $(UEFI_INPUT_DEBUG_LOG)
	grep -F "UEFI:NAV-RESTORED" $(UEFI_INPUT_DEBUG_LOG)
	grep -F "UEFI:NAV-SLIDE-LEFT" $(UEFI_INPUT_DEBUG_LOG)
	grep -F "UEFI:NAV-SLIDE-RIGHT" $(UEFI_INPUT_DEBUG_LOG)
	grep -F "UEFI:NAV-RECOVERY-FADE" $(UEFI_INPUT_DEBUG_LOG)
	grep -F "UEFI:NAV-TRANSFORMED-FRAME" $(UEFI_INPUT_DEBUG_LOG)
	grep -F "UEFI:PAGE-ACTIVE" $(UEFI_INPUT_DEBUG_LOG)
	grep -F "UEFI:PAGE-RESTORED" $(UEFI_INPUT_DEBUG_LOG)
	grep -F "UEFI:DIALOG-PAGE-ACTIVE" $(UEFI_INPUT_DEBUG_LOG)
	grep -F "UEFI:HELP-VIEW" $(UEFI_INPUT_DEBUG_LOG)
	grep -F "UEFI:BACK" $(UEFI_INPUT_DEBUG_LOG)
	test -s build/uefi-help.ppm
	@echo "QEMU UEFI Event-Queue-, Navigation-, Unteransichten- und Hilfe-Test erfolgreich"

test-uefi-power: uefi
	powershell.exe -NoProfile -ExecutionPolicy Bypass \
		-File scripts/test-uefi-power.ps1 -Qemu "$(QEMU64)" \
		-Firmware $(UEFI_FIRMWARE) -FatDirectory $(UEFI_DIR) \
		-DebugLog $(UEFI_POWER_DEBUG_LOG)
	grep -F "UEFI:POWER-READY" $(UEFI_POWER_DEBUG_LOG)
	grep -F "UEFI:POWER-CONFIRM" $(UEFI_POWER_DEBUG_LOG)
	grep -F "UEFI:DIALOG-DESTRUCTIVE-ARMED" $(UEFI_POWER_DEBUG_LOG)
	grep -F "UEFI:DIALOG-RESULT-YES" $(UEFI_POWER_DEBUG_LOG)
	grep -F "UEFI:POWER-SHUTDOWN" $(UEFI_POWER_DEBUG_LOG)
	@echo "QEMU UEFI bestätigter Shutdown-Test erfolgreich"

test-uefi-dialog: uefi
	powershell.exe -NoProfile -ExecutionPolicy Bypass \
		-File scripts/test-uefi-dialog.ps1 -Qemu "$(QEMU64)" \
		-Firmware $(UEFI_FIRMWARE) -FatDirectory $(UEFI_DIR) \
		-DebugLog build/qemu-uefi-dialog-debug.log
	grep -F "UEFI:DIALOG-READY" build/qemu-uefi-dialog-debug.log
	grep -F "UEFI:DIALOG-OPEN" build/qemu-uefi-dialog-debug.log
	grep -F "UEFI:DIALOG-FRAME-READY" build/qemu-uefi-dialog-debug.log
	grep -F "UEFI:DIALOG-PAGE-ACTIVE" build/qemu-uefi-dialog-debug.log
	grep -F "UEFI:PAGE-RESTORED" build/qemu-uefi-dialog-debug.log
	grep -F "UEFI:DIALOG-RESULT-CANCEL" build/qemu-uefi-dialog-debug.log
	test -s build/uefi-dialog.ppm
	@echo "QEMU UEFI Modal-, Fokus- und Abbruchdialog-Test erfolgreich"

test-uefi-context: uefi
	powershell.exe -NoProfile -ExecutionPolicy Bypass \
		-File scripts/test-uefi-context.ps1 -Qemu "$(QEMU64)" \
		-Firmware $(UEFI_FIRMWARE) -FatDirectory $(UEFI_DIR) \
		-DebugLog build/qemu-uefi-context-debug.log
	grep -F "UEFI:CONTEXT-OPEN" build/qemu-uefi-context-debug.log
	grep -F "UEFI:CONTEXT-FRAME-READY" build/qemu-uefi-context-debug.log
	grep -F "UEFI:CONTEXT-ACTION" build/qemu-uefi-context-debug.log
	grep -F "UEFI:CONTEXT-ADVANCED" build/qemu-uefi-context-debug.log
	test -s build/uefi-context.ppm
	@echo "QEMU UEFI Context-Menu- und Tastaturtest erfolgreich"

test-uefi-tooltip-breadcrumb: uefi
	powershell.exe -NoProfile -ExecutionPolicy Bypass \
		-File scripts/test-uefi-tooltip-breadcrumb.ps1 -Qemu "$(QEMU64)" \
		-Firmware $(UEFI_FIRMWARE) -FatDirectory $(UEFI_DIR) \
		-DebugLog build/qemu-uefi-tooltip-debug.log
	grep -F "UEFI:TOOLTIP-SHOW" build/qemu-uefi-tooltip-debug.log
	grep -F "UEFI:TOOLTIP-FRAME-READY" build/qemu-uefi-tooltip-debug.log
	grep -F "UEFI:BREADCRUMB-FRAME-READY" build/qemu-uefi-tooltip-debug.log
	grep -F "UEFI:BREADCRUMB-FOCUS" build/qemu-uefi-tooltip-debug.log
	grep -F "UEFI:BREADCRUMB-ROOT" build/qemu-uefi-tooltip-debug.log
	grep -F "UEFI:ICONBUTTON-FRAME-READY" build/qemu-uefi-tooltip-debug.log
	test -s build/uefi-tooltip.ppm
	test -s build/uefi-breadcrumb.ppm
	test -s build/uefi-breadcrumb-focus.ppm
	@echo "QEMU UEFI Tooltip- und Breadcrumb-Test erfolgreich"

test-uefi-settings-controls: uefi
	powershell.exe -NoProfile -ExecutionPolicy Bypass \
		-File scripts/test-uefi-settings-controls.ps1 -Qemu "$(QEMU64)" \
		-Firmware $(UEFI_FIRMWARE) -FatDirectory $(UEFI_DIR) \
		-DebugLog build/qemu-uefi-settings-controls-debug.log
	grep -F "UEFI:SETTINGS-TOOLTIPS-OFF" build/qemu-uefi-settings-controls-debug.log
	grep -F "UEFI:SETTINGS-SLIDER-UPDATED" build/qemu-uefi-settings-controls-debug.log
	grep -F "UEFI:SETTINGS-SLIDER-HOME" build/qemu-uefi-settings-controls-debug.log
	grep -F "UEFI:SETTINGS-SLIDER-END" build/qemu-uefi-settings-controls-debug.log
	grep -F "UEFI:SETTINGS-CONTROLS-STABLE" build/qemu-uefi-settings-controls-debug.log
	grep -F "UEFI:LABEL-FRAME-READY" build/qemu-uefi-settings-controls-debug.log
	grep -F "UEFI:ICON-CONTROL-FRAME-READY" build/qemu-uefi-settings-controls-debug.log
	grep -F "UEFI:IMAGE-CONTROL-FRAME-READY" build/qemu-uefi-settings-controls-debug.log
	grep -F "UEFI:SEPARATOR-FRAME-READY" build/qemu-uefi-settings-controls-debug.log
	test -s build/uefi-settings-controls.ppm
	@echo "QEMU UEFI Checkbox-, Slider- und Tastaturtest erfolgreich"

test-uefi-list-controls: uefi
	powershell.exe -NoProfile -ExecutionPolicy Bypass -File scripts/test-uefi-list-controls.ps1 \
		-Qemu "$(QEMU64)" -Firmware $(UEFI_FIRMWARE) -FatDirectory $(UEFI_DIR) \
		-DebugLog build/qemu-uefi-list-controls-debug.log
	grep -F "UEFI:LIST-CONTROL-FRAME-READY" build/qemu-uefi-list-controls-debug.log
	grep -F "UEFI:LIST-HOME-END" build/qemu-uefi-list-controls-debug.log
	grep -F "UEFI:LIST-PAGE-NAVIGATION" build/qemu-uefi-list-controls-debug.log
	grep -F "UEFI:INSTALL-UNAVAILABLE" build/qemu-uefi-list-controls-debug.log
	test -s build/uefi-list-controls.ppm
	@echo "QEMU UEFI List-/ListItem-Navigation und Space-Aktivierung erfolgreich"

test-uefi-help-search: uefi
	powershell.exe -NoProfile -ExecutionPolicy Bypass \
		-File scripts/test-uefi-help-search.ps1 -Qemu "$(QEMU64)" \
		-Firmware $(UEFI_FIRMWARE) -FatDirectory $(UEFI_DIR) \
		-DebugLog build/qemu-uefi-help-search-debug.log
	grep -F "UEFI:HELP-SEARCH-FOCUS" build/qemu-uefi-help-search-debug.log
	grep -F "UEFI:HELP-SEARCH-INPUT" build/qemu-uefi-help-search-debug.log
	grep -F "UEFI:HELP-SEARCH-COMPLETE" build/qemu-uefi-help-search-debug.log
	grep -F "UEFI:HELP-RESULT-OPEN" build/qemu-uefi-help-search-debug.log
	test -s build/uefi-help-search.ppm
	@echo "QEMU UEFI TextField-, Such- und Hilfetest erfolgreich"

test-uefi-firmware: uefi
	powershell.exe -NoProfile -ExecutionPolicy Bypass \
		-File scripts/test-uefi-firmware.ps1 -Qemu "$(QEMU64)" \
		-Firmware $(UEFI_FIRMWARE) -FatDirectory $(UEFI_DIR) \
		-DebugLog build/qemu-uefi-firmware-debug.log
	grep -F "UEFI:FIRMWARE-READY" build/qemu-uefi-firmware-debug.log
	grep -F "UEFI:FIRMWARE-VIEW" build/qemu-uefi-firmware-debug.log
	grep -F "UEFI:FIRMWARE-VIEW-STABLE" build/qemu-uefi-firmware-debug.log
	test -s build/uefi-firmware.ppm
	@echo "QEMU UEFI Firmwarestatus- und Setup-Ziel-Test erfolgreich"

test-uefi-progress: uefi
	powershell.exe -NoProfile -ExecutionPolicy Bypass \
		-File scripts/test-uefi-progress.ps1 -Qemu "$(QEMU64)" \
		-Firmware $(UEFI_FIRMWARE) -FatDirectory $(UEFI_DIR) \
		-DebugLog build/qemu-uefi-progress-debug.log
	grep -F "UEFI:PROGRESS-OPEN" build/qemu-uefi-progress-debug.log
	grep -F "UEFI:ACTIVITY-FRAME-READY" build/qemu-uefi-progress-debug.log
	grep -F "UEFI:PROGRESS-COMPLETE" build/qemu-uefi-progress-debug.log
	grep -F "UEFI:DIALOG-RESULT-OK" build/qemu-uefi-progress-debug.log
	test -s build/uefi-progress.ppm
	test -s build/uefi-activity.ppm
	@echo "QEMU UEFI bestimmter/unbestimmter Progress- und Abschluss-Test erfolgreich"

test-uefi-scrollview: uefi
	powershell.exe -NoProfile -ExecutionPolicy Bypass \
		-File scripts/test-uefi-scrollview.ps1 -Qemu "$(QEMU64)" \
		-Firmware $(UEFI_FIRMWARE) -FatDirectory $(UEFI_DIR) \
		-DebugLog build/qemu-uefi-scrollview-debug.log
	grep -F "UEFI:SCROLL-KEY" build/qemu-uefi-scrollview-debug.log
	grep -F "UEFI:SCROLLVIEW-FRAME-READY" build/qemu-uefi-scrollview-debug.log
	test -s build/uefi-scrollview.ppm
	@echo "QEMU UEFI ScrollView-, Scrollbar- und Tastaturtest erfolgreich"

test-uefi-recovery-tiles: uefi
	powershell.exe -NoProfile -ExecutionPolicy Bypass -File scripts/test-uefi-recovery-tiles.ps1 \
		-Qemu "$(QEMU64)" -Firmware $(UEFI_FIRMWARE) -FatDirectory $(UEFI_DIR) \
		-DebugLog build/qemu-uefi-recovery-tiles-debug.log
	grep -F "UEFI:TILE-FRAME-READY" build/qemu-uefi-recovery-tiles-debug.log
	grep -F "UEFI:TILE-NAVIGATION" build/qemu-uefi-recovery-tiles-debug.log
	grep -F "UEFI:STATUS-BADGE-FRAME-READY" build/qemu-uefi-recovery-tiles-debug.log
	test -s build/uefi-recovery-tiles.ppm
	@echo "QEMU UEFI Recovery-Tile-, Grid- und Space-Aktivierungstest erfolgreich"

test-uefi-ui-recovery: uefi
	powershell.exe -NoProfile -ExecutionPolicy Bypass -File scripts/test-uefi-ui-recovery.ps1 \
		-Qemu "$(QEMU64)" -Firmware $(UEFI_FIRMWARE) -FatDirectory $(UEFI_DIR) \
		-DebugLog build/qemu-uefi-ui-recovery-debug.log
	grep -F "UEFI:RECOVERY-MANAGER-READY" build/qemu-uefi-ui-recovery-debug.log
	grep -F "UEFI:MEMORY-MANAGER-READY" build/qemu-uefi-ui-recovery-debug.log
	grep -F "UEFI:MEMORY-SELF-TEST" build/qemu-uefi-ui-recovery-debug.log
	grep -F "UEFI:MEMORY-SELF-TEST-FRAME" build/qemu-uefi-ui-recovery-debug.log
	grep -F "UEFI:MEMORY-FRAME-READY" build/qemu-uefi-ui-recovery-debug.log
	grep -F "UEFI:RECOVERY-SAFE-MODE" build/qemu-uefi-ui-recovery-debug.log
	grep -F "UEFI:RECOVERY-FRAME-READY" build/qemu-uefi-ui-recovery-debug.log
	grep -F "UEFI:TEXT-FALLBACK-SELF-TEST" build/qemu-uefi-ui-recovery-debug.log
	grep -F "UEFI:TEXT-FALLBACK" build/qemu-uefi-ui-recovery-debug.log
	grep -F "UEFI:TEXT-CONTINUE" build/qemu-uefi-ui-recovery-debug.log
	grep -F "UEFI:MENU-DRAWN" build/qemu-uefi-ui-recovery-debug.log
	test -s build/uefi-ui-recovery.ppm
	test -s build/uefi-memory-self-test.ppm
	@echo "QEMU UEFI Error-Recovery-, Safe-Mode- und Continue-Boot-Test erfolgreich"

test-uefi-themes: uefi
	powershell.exe -NoProfile -ExecutionPolicy Bypass \
		-File scripts/test-uefi-themes.ps1 -Qemu "$(QEMU64)" \
		-Firmware $(UEFI_FIRMWARE) -FatDirectory $(UEFI_DIR) \
		-DebugLog $(UEFI_THEME_DEBUG_LOG)
	grep -F "UEFI:THEME-LIGHT" $(UEFI_THEME_DEBUG_LOG)
	grep -F "UEFI:THEME-HIGH-CONTRAST" $(UEFI_THEME_DEBUG_LOG)
	grep -F "UEFI:THEME-DARK" $(UEFI_THEME_DEBUG_LOG)
	grep -F "UEFI:MENU-BUTTON-OPEN" $(UEFI_THEME_DEBUG_LOG)
	grep -F "UEFI:MENU-BUTTON-SELECTION" $(UEFI_THEME_DEBUG_LOG)
	grep -F "UEFI:REDUCED-MOTION-ON" $(UEFI_THEME_DEBUG_LOG)
	grep -F "UEFI:NAV-REDUCED-FADE" $(UEFI_THEME_DEBUG_LOG)
	test -s build/uefi-theme-light.ppm
	test -s build/uefi-theme-high-contrast.ppm
	test -s build/uefi-menu-button.ppm
	@echo "QEMU UEFI Theme- und Accessibility-Test erfolgreich"

test-uefi-resolutions: $(UEFI_FIRMWARE)
	$(MAKE) uefi UEFI_DIR=build/uefi-800 UEFI_FIRMWARE=$(UEFI_FIRMWARE) \
		UEFI_EXTRA_CFLAGS="-DNOVA_GOP_PREFERRED_WIDTH=800 -DNOVA_GOP_PREFERRED_HEIGHT=600"
	powershell.exe -NoProfile -ExecutionPolicy Bypass -File scripts/test-uefi-resolution.ps1 \
		-Qemu "$(QEMU64)" -Firmware $(UEFI_FIRMWARE) -FatDirectory build/uefi-800 \
		-DebugLog $(UEFI_RESOLUTION_DEBUG_LOG) -Screenshot build/uefi-800x600.ppm
	grep -F "UEFI:GOP-PREFERRED-READY" $(UEFI_RESOLUTION_DEBUG_LOG)
	grep -F "UEFI:LAYOUT-READY" $(UEFI_RESOLUTION_DEBUG_LOG)
	test -s build/uefi-800x600.ppm
	$(MAKE) uefi UEFI_DIR=build/uefi-1280 UEFI_FIRMWARE=$(UEFI_FIRMWARE) \
		UEFI_EXTRA_CFLAGS="-DNOVA_GOP_PREFERRED_WIDTH=1280 -DNOVA_GOP_PREFERRED_HEIGHT=720"
	powershell.exe -NoProfile -ExecutionPolicy Bypass -File scripts/test-uefi-resolution.ps1 \
		-Qemu "$(QEMU64)" -Firmware $(UEFI_FIRMWARE) -FatDirectory build/uefi-1280 \
		-DebugLog $(UEFI_RESOLUTION_DEBUG_LOG) -Screenshot build/uefi-1280x720.ppm
	grep -F "UEFI:GOP-PREFERRED-READY" $(UEFI_RESOLUTION_DEBUG_LOG)
	grep -F "UEFI:LAYOUT-READY" $(UEFI_RESOLUTION_DEBUG_LOG)
	test -s build/uefi-1280x720.ppm
	$(MAKE) uefi UEFI_DIR=build/uefi-1920 UEFI_FIRMWARE=$(UEFI_FIRMWARE) \
		UEFI_EXTRA_CFLAGS="-DNOVA_GOP_PREFERRED_WIDTH=1920 -DNOVA_GOP_PREFERRED_HEIGHT=1080"
	powershell.exe -NoProfile -ExecutionPolicy Bypass -File scripts/test-uefi-resolution.ps1 \
		-Qemu "$(QEMU64)" -Firmware $(UEFI_FIRMWARE) -FatDirectory build/uefi-1920 \
		-DebugLog $(UEFI_RESOLUTION_DEBUG_LOG) -Screenshot build/uefi-1920x1080.ppm
	grep -F "UEFI:GOP-PREFERRED-READY" $(UEFI_RESOLUTION_DEBUG_LOG)
	grep -F "UEFI:LAYOUT-READY" $(UEFI_RESOLUTION_DEBUG_LOG)
	test -s build/uefi-1920x1080.ppm
	@echo "QEMU UEFI 800x600-, 1280x720- und 1920x1080-Layout-Test erfolgreich"

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
	status=0; (sleep 4; echo "sendkey f1"; sleep 0.8; echo "sendkey esc"; sleep 0.5; \
		echo "sendkey down"; sleep 0.3; echo "sendkey down"; \
		sleep 0.3; echo "sendkey ret"; sleep 1; echo "sendkey esc"; \
		sleep 0.5; echo "sendkey d"; sleep 1; echo "sendkey r"; \
		sleep 1; echo "sendkey esc"; sleep 0.3; echo "sendkey up"; \
		sleep 0.3; echo "sendkey up"; sleep 0.3; echo "sendkey ret"; \
		sleep 3; echo "quit") | \
		timeout 16s "$(QEMU)" \
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
	grep -F "BM:SETTINGS" $(DEBUG_LOG)
	grep -F "BM:HELP" $(DEBUG_LOG)
	grep -F "BM:BACK" $(DEBUG_LOG)
	grep -F "BM:START" $(DEBUG_LOG)
	@echo "QEMU BIOS smoke test erfolgreich"

test-mouse: image
	rm -f $(MOUSE_SERIAL_LOG) $(MOUSE_DEBUG_LOG)
	status=0; (sleep 4; echo "mouse_move -100 -60"; sleep 0.8; \
		echo "mouse_button 1"; echo "mouse_button 0"; sleep 1; \
		echo "sendkey esc"; sleep 0.7; echo "mouse_move 0 -136"; \
		sleep 0.7; echo "mouse_button 1"; echo "mouse_button 0"; \
		sleep 3; echo "quit") | timeout 12s "$(QEMU)" \
		-drive format=raw,file=$(DISK_IMAGE),if=ide -vga std -display none \
		-monitor stdio -serial file:$(MOUSE_SERIAL_LOG) \
		-debugcon file:$(MOUSE_DEBUG_LOG) -no-reboot -no-shutdown || status=$$?; \
		test "$$status" -eq 0 -o "$$status" -eq 124
	grep -F "BM:SETTINGS" $(MOUSE_DEBUG_LOG)
	grep -F "BM:BACK" $(MOUSE_DEBUG_LOG)
	grep -F "BM:START" $(MOUSE_DEBUG_LOG)
	grep -F "NOVA_KERNEL_READY" $(MOUSE_SERIAL_LOG)
	@echo "QEMU PS/2 mouse GUI test erfolgreich"

test-theme: image
	rm -f $(THEME_SERIAL_LOG) $(THEME_DEBUG_LOG)
	status=0; (sleep 4; echo "sendkey h"; sleep 0.8; echo "sendkey h"; \
		sleep 0.8; echo "sendkey ret"; sleep 3; echo "quit") | \
		timeout 9s "$(QEMU)" -drive format=raw,file=$(DISK_IMAGE),if=ide \
		-vga std -display none -monitor stdio \
		-serial file:$(THEME_SERIAL_LOG) -debugcon file:$(THEME_DEBUG_LOG) \
		-no-reboot -no-shutdown || status=$$?; \
		test "$$status" -eq 0 -o "$$status" -eq 124
	grep -F "BM:THEME-HIGH-CONTRAST" $(THEME_DEBUG_LOG)
	grep -F "BM:THEME-DARK" $(THEME_DEBUG_LOG)
	grep -F "NOVA_KERNEL_READY" $(THEME_SERIAL_LOG)
	@echo "QEMU Theme- und Accessibility-Test erfolgreich"

test-ui-flows: image
	rm -f $(FLOWS_SERIAL_LOG) $(FLOWS_DEBUG_LOG)
	status=0; (sleep 4; echo "sendkey f2"; sleep 5; echo "sendkey esc"; \
		sleep 4; echo "sendkey f3"; sleep 5; echo "sendkey esc"; \
		sleep 4; echo "sendkey end"; sleep 4; echo "sendkey ret"; \
		sleep 5; echo "sendkey esc"; sleep 4; echo "sendkey home"; \
		sleep 4; echo "sendkey ret"; sleep 3; echo "quit") | \
		timeout 52s "$(QEMU)" -drive format=raw,file=$(DISK_IMAGE),if=ide \
		-vga std -display none -monitor stdio \
		-serial file:$(FLOWS_SERIAL_LOG) -debugcon file:$(FLOWS_DEBUG_LOG) \
		-no-reboot -no-shutdown || status=$$?; \
		test "$$status" -eq 0 -o "$$status" -eq 124
	grep -F "BM:DETAILS" $(FLOWS_DEBUG_LOG)
	grep -F "BM:ADVANCED" $(FLOWS_DEBUG_LOG)
	grep -F "BM:POWER-DIALOG" $(FLOWS_DEBUG_LOG)
	grep -F "BM:START" $(FLOWS_DEBUG_LOG)
	grep -F "NOVA_KERNEL_READY" $(FLOWS_SERIAL_LOG)
	@echo "QEMU Bootmanager-Dialog- und Optionsfluss erfolgreich"

test-recovery: image
	rm -f $(RECOVERY_SERIAL_LOG) $(RECOVERY_DEBUG_LOG)
	status=0; (sleep 4; echo "sendkey r"; sleep 3; echo "sendkey ret"; \
		sleep 3; echo "sendkey esc"; sleep 3; echo "sendkey down"; sleep 2; \
		echo "sendkey ret"; sleep 3; echo "sendkey esc"; sleep 3; \
		echo "sendkey down"; sleep 2; echo "sendkey ret"; sleep 3; \
		echo "sendkey ret"; sleep 3; echo "sendkey esc"; sleep 3; \
		echo "sendkey down"; sleep 2; echo "sendkey ret"; sleep 3; \
		echo "sendkey esc"; sleep 3; echo "sendkey esc"; sleep 3; \
		echo "sendkey home"; sleep 2; echo "sendkey ret"; sleep 3; echo "quit") | \
		timeout 58s "$(QEMU)" -drive format=raw,file=$(DISK_IMAGE),if=ide \
		-vga std -display none -monitor stdio \
		-serial file:$(RECOVERY_SERIAL_LOG) -debugcon file:$(RECOVERY_DEBUG_LOG) \
		-no-reboot -no-shutdown || status=$$?; \
		test "$$status" -eq 0 -o "$$status" -eq 124
	grep -F "BM:SELF-HEALING" $(RECOVERY_DEBUG_LOG)
	grep -F "BM:SNAPSHOTS" $(RECOVERY_DEBUG_LOG)
	grep -F "BM:MEMORY-TEST" $(RECOVERY_DEBUG_LOG)
	grep -F "BM:MEMORY-PASSED" $(RECOVERY_DEBUG_LOG)
	grep -F "BM:DISK-TOOLS" $(RECOVERY_DEBUG_LOG)
	grep -F "BM:START" $(RECOVERY_DEBUG_LOG)
	grep -F "NOVA_KERNEL_READY" $(RECOVERY_SERIAL_LOG)
	@echo "QEMU Recovery-, Snapshot-, Speicher- und Datentraegerfluss erfolgreich"

test-platform: image
	rm -f $(PLATFORM_SERIAL_LOG) $(PLATFORM_DEBUG_LOG)
	status=0; (sleep 4; echo "sendkey f4"; sleep 2; echo "sendkey esc"; \
		sleep 2; echo "sendkey f5"; sleep 2; echo "sendkey esc"; \
		sleep 2; echo "sendkey f6"; sleep 2; echo "sendkey esc"; \
		sleep 2; echo "sendkey home"; sleep 2; echo "sendkey ret"; sleep 3; echo "quit") | \
		timeout 26s "$(QEMU)" -drive format=raw,file=$(DISK_IMAGE),if=ide \
		-vga std -display none -monitor stdio \
		-serial file:$(PLATFORM_SERIAL_LOG) -debugcon file:$(PLATFORM_DEBUG_LOG) \
		-no-reboot -no-shutdown || status=$$?; \
		test "$$status" -eq 0 -o "$$status" -eq 124
	grep -F "BM:NETWORK" $(PLATFORM_DEBUG_LOG)
	grep -F "BM:FIRMWARE" $(PLATFORM_DEBUG_LOG)
	grep -F "BM:ENCRYPTION" $(PLATFORM_DEBUG_LOG)
	grep -F "BM:START" $(PLATFORM_DEBUG_LOG)
	grep -F "NOVA_KERNEL_READY" $(PLATFORM_SERIAL_LOG)
	@echo "QEMU Netzwerk-, Firmware- und Verschluesselungsstatus erfolgreich"

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
		$(BUILD_DIR)/invalid-cpu.elf $(BUILD_DIR)/invalid-cpu.img $(BUILD_DIR)/invalid-cpu.log \
		$(UEFI_DEBUG_LOG)
	rm -rf $(UEFI_DIR)
