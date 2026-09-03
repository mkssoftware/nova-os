#ifndef NOVA_VFS_H
#define NOVA_VFS_H

#include <stdint.h>

#define NOVA_VFS_ABI_MAJOR 1u
#define NOVA_VFS_ABI_MINOR 0u

#define NOVA_VFS_FLAG_BOOTSTRAP_ROOT 0x00000001u
#define NOVA_VFS_FLAG_READ_ONLY      0x00000002u

typedef struct NovaVfsStateV1 {
    uint32_t Initialized;
    uint32_t RootReady;
    uint32_t FilesystemHandle;
    uint32_t NamespaceHandle;
    uint32_t MountHandle;
    uint32_t RootNodeHandle;
    uint32_t Flags;
    uint32_t Reserved;
} NovaVfsStateV1;

typedef struct NovaVfsApiV1 {
    uint32_t StructSize;
    uint16_t AbiMajor;
    uint16_t AbiMinor;
    uint32_t StateAddress;
    uint32_t LookupRootEntry;
    uint32_t RootPathAddress;
    uint32_t RootPathLength;
    uint32_t Capabilities;
    uint32_t Reserved;
} NovaVfsApiV1;

_Static_assert(sizeof(NovaVfsStateV1) == 32,
               "NovaVfsStateV1 ABI size changed");
_Static_assert(sizeof(NovaVfsApiV1) == 32,
               "NovaVfsApiV1 ABI size changed");

#endif
