#ifndef NOVA_UEFI_RUNTIME_BRIDGE_H
#define NOVA_UEFI_RUNTIME_BRIDGE_H

#include "uefi_min.h"
#include "boot_control.h"
#include "../../include/nova_boot_protocol.h"
#include <stdbool.h>
#include <stdint.h>

#define NOVA_UEFI_RUNTIME_BRIDGE_VERSION 1u
#define NOVA_UEFI_RUNTIME_BRIDGE_SIZE 192u

#pragma pack(push, 1)
typedef struct nova_uefi_runtime_gdtr32 {
    uint16_t limit;
    uint32_t base;
} nova_uefi_runtime_gdtr32_t;

typedef struct nova_uefi_runtime_far32 {
    uint32_t offset;
    uint16_t selector;
} nova_uefi_runtime_far32_t;

typedef struct nova_uefi_runtime_bridge_context {
    uint32_t size;
    uint32_t version;
    uint32_t capabilities;
    uint32_t flags;
    uint64_t firmware_cr3;
    uint64_t set_variable;
    uint64_t variable_name;
    uint64_t vendor_guid;
    uint64_t last_status;
    uint32_t saved_stack;
    uint32_t reserved0;
    nova_boot_health_wire_t evidence;
    uint64_t gdt[4];
    nova_uefi_runtime_gdtr32_t gdtr;
    nova_uefi_runtime_far32_t enter64;
    nova_uefi_runtime_far32_t return32;
    uint8_t reserved1[14];
} nova_uefi_runtime_bridge_context_t;
#pragma pack(pop)

_Static_assert(sizeof(nova_uefi_runtime_bridge_context_t)==NOVA_UEFI_RUNTIME_BRIDGE_SIZE,
               "UEFI-Runtime-Bridge-Kontext muss 192 Byte gross sein");

bool uefi_runtime_bridge_prepare(EFI_SYSTEM_TABLE *system_table);
bool uefi_runtime_bridge_descriptor(nova_bib_firmware_runtime_t *descriptor);

extern void uefi_runtime_bridge_entry32(void);
extern void uefi_runtime_bridge_entry64(void);
extern void uefi_runtime_bridge_return32(void);

#endif
