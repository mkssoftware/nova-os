/* NovaOS C-Kernel - Compatibility Layer (Port aus compat32.inc,
   NPSPEC-COMPAT-ABI-0001 und verwandte NPSPEC-COMPAT-Spezifikationen). */
#ifndef NOVA_COMPAT_H
#define NOVA_COMPAT_H

#include <stdint.h>

#define PERS_NONE         0u
#define PERS_POSIX        1u
#define PERS_LINUX        2u
#define PERS_LINUX_UBUNTU 3u
#define PERS_WIN32        4u
#define PERS_LEGACY_NOVA  5u

#define PERS_FLAG_ACTIVE      0x01u
#define PERS_FLAG_SANDBOX     0x02u
#define PERS_FLAG_SYSCALL_TBL 0x04u
#define PERS_FLAG_FILEFORMAT  0x08u

#define ABI_NOVA       0u
#define ABI_SYSV_AMD64 1u
#define ABI_WIN64      2u
#define ABI_IA32       3u
#define ABI_ARM64      4u

#define SCTRANS_OK          0u
#define SCTRANS_PASSTHROUGH 1u
#define SCTRANS_UNSUPPORTED 2u
#define SCTRANS_BLOCKED     3u
#define SCTRANS_ERROR       4u

#define FILEFMT_UNKNOWN 0u
#define FILEFMT_ELF32   1u
#define FILEFMT_ELF64   2u
#define FILEFMT_PE32    3u
#define FILEFMT_PE64    4u
#define FILEFMT_NOVABIN 5u
#define FILEFMT_SCRIPT  6u

#define PERS_CAPACITY   4u
#define SC_MAP_CAPACITY 32u

#define LINUX_SYS_READ       0u
#define LINUX_SYS_WRITE      1u
#define LINUX_SYS_OPEN       2u
#define LINUX_SYS_CLOSE      3u
#define LINUX_SYS_EXIT       60u
#define LINUX_SYS_EXIT_GROUP 231u
#define LINUX_SYS_GETPID     39u
#define LINUX_SYS_MMAP       9u
#define LINUX_SYS_MUNMAP     11u
#define LINUX_SYS_BRK        12u

#define NOVA_SYS_READ   0x10u
#define NOVA_SYS_WRITE  0x11u
#define NOVA_SYS_OPEN   0x12u
#define NOVA_SYS_CLOSE  0x13u
#define NOVA_SYS_EXIT   0x1Eu
#define NOVA_SYS_GETPID 0x20u
#define NOVA_SYS_MMAP   0x30u

/* Initialisiert und registriert zusaetzlich die Linux-Personality mit
   ihren Syscall-Mappings (NPSPEC-COMPAT-LINUX-0001). */
void compat_initialize(void);
int compat_self_test(void); /* 0 = ok */

/* *out_nova_nr und *out_result (SCTRANS_*) werden immer gesetzt (0 = Erfolg,
   identisch zum alten ASM - CF=0 auch bei UNSUPPORTED/BLOCKED). */
void compat_translate_syscall(uint32_t foreign_syscall_nr, uint32_t personality_id,
                               uint32_t *out_nova_nr, uint32_t *out_result);

/* Erkennt ELF32/64, PE, Nova-NKI oder Shebang-Script anhand der Magic-Bytes. */
uint32_t compat_detect_file_format(const uint8_t *header, uint32_t len);

/* 0 = Erfolg (*out_record erhaelt den Record-Zeiger), -1 wenn nicht gefunden. */
int compat_get_personality(uint32_t pers_id, void **out_record);

/* 0 = Erfolg (*out_sc_count, *out_pers_flags gesetzt), -1 wenn unbekannt. */
int compat_introspect(uint32_t pers_id, uint32_t *out_sc_count, uint32_t *out_pers_flags);

#endif
