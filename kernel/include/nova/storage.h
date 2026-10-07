#ifndef NOVA_STORAGE_H
#define NOVA_STORAGE_H

/*
 * Nova Storage Bootstrap ABI 1.0 (storage32.inc): AHCI im Polling-Betrieb,
 * Block-Device-Datensaetze und synchrone Sektor-I/O.
 */

#include <stdint.h>

#define NOVA_STORAGE_ABI_MAJOR 1u
#define NOVA_STORAGE_ABI_MINOR 0u
#define NOVA_STORAGE_CAPACITY 4u

#define NOVA_STORAGE_STATE_UNKNOWN 0u
#define NOVA_STORAGE_STATE_READY 1u
#define NOVA_STORAGE_STATE_FAILED 2u

#define NOVA_STORAGE_FLAG_LBA48 0x00000001u
#define NOVA_STORAGE_FLAG_WRITE_CACHE 0x00000002u

#define NOVA_DEVICE_CLASS_STORAGE_CONTROLLER 4u
#define NOVA_DEVICE_CLASS_BLOCK 5u

typedef struct NovaStorageRecordV1 {
    uint32_t DeviceId;       /* stabil: CRC32C(Seriennummer || Modell) */
    uint32_t State;
    uint32_t Port;
    uint32_t PortMmio;
    uint32_t CommandPage;
    uint32_t SectorsLow;
    uint32_t SectorsHigh;
    uint32_t SectorSize;
    uint32_t KernelDeviceId;
    uint32_t Reads;
    uint32_t Writes;
    uint32_t Errors;
    uint32_t LastTaskFile;
    uint32_t Flags;
    uint32_t Reserved[2];
} NovaStorageRecordV1;

typedef struct NovaStorageApiV1 {
    uint32_t StructSize;
    uint16_t AbiMajor;
    uint16_t AbiMinor;
    uint32_t Capacity;
    uint32_t ReadEntry;
    uint32_t WriteEntry;
    uint32_t FlushEntry;
    uint32_t CountAddress;
    uint32_t RecordsAddress;
} NovaStorageApiV1;

_Static_assert(sizeof(NovaStorageRecordV1) == 64, "NovaStorageRecordV1 ABI size changed");
_Static_assert(sizeof(NovaStorageApiV1) == 32, "NovaStorageApiV1 ABI size changed");

#endif
