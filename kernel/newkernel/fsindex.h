/* NovaOS C-Kernel - Filesystem-Index und System-Verzeichnis (Port aus
   fsindex32.inc, NPSPEC-INDEX- und NPSPEC-SYSTEM-Spezifikationen).
   TODO: FS_NS_SYSTEM/TRUST_SYSTEM/TRUST_USER/FSOBJ_TYPE_DIR hier lokal
   definiert (Werte aus fs32.inc uebernommen) - vereinheitlichen, sobald
   fs32.inc portiert ist. */
#ifndef NOVA_FSINDEX_H
#define NOVA_FSINDEX_H

#include <stdint.h>

#define SYSDIR_KERNEL     0u
#define SYSDIR_BOOT       1u
#define SYSDIR_DRIVERS    2u
#define SYSDIR_MODULES    3u
#define SYSDIR_FRAMEWORK  4u
#define SYSDIR_HAL        5u
#define SYSDIR_LIBRARIES  6u
#define SYSDIR_LOCALE     7u
#define SYSDIR_RESOURCES  8u
#define SYSDIR_RUNTIME    9u
#define SYSDIR_SECURITY   10u
#define SYSDIR_SERVICES   11u
#define SYSDIR_TRUST      12u
#define SYSDIR_RECOVERY   13u
#define SYSDIR_POLICY     14u
#define SYSDIR_REGISTRY   15u
#define SYSDIR_UPDATES    16u
#define SYSDIR_FOUNDATION 17u
#define SYSDIR_PROTECTION 18u
#define SYSDIR_COUNT      19u

#define SYSDIR_PROT_NONE      0u
#define SYSDIR_PROT_READ_ONLY 1u
#define SYSDIR_PROT_IMMUTABLE 2u
#define SYSDIR_PROT_SIGNED    3u

#define IDX_FLAG_VALID     0x01u
#define IDX_FLAG_DIRTY     0x02u
#define IDX_FLAG_WORKSPACE 0x04u
#define IDX_FLAG_RELATION  0x08u

#define IDX_CAPACITY 32u

/* TODO: aus fs32.inc uebernommene Werte (siehe Kommentar oben). */
#define FS_NS_SYSTEM    1u
#define TRUST_SYSTEM    1u
#define TRUST_USER      2u
#define FSOBJ_TYPE_DIR  2u

void fsindex_initialize(void);
int fsindex_self_test(void); /* 0 = ok */

/* 0 = Erfolg, -1 wenn sysdir_id >= SYSDIR_COUNT. */
int sysdir_register(uint32_t sysdir_id, uint32_t protection);

/* 0 = Erfolg, -1 wenn Index voll. */
int fsindex_insert(uint32_t inode_lo, uint32_t vol_id, uint32_t ns_id, uint32_t type);

/* 0 = Erfolg (*out_inode_lo gesetzt), -1 = nicht gefunden. */
int fsindex_search(uint32_t ns_id, uint32_t type, uint32_t *out_inode_lo);

/* Markiert alle Eintraege als frisch validiert und aktualisiert die Pruefsumme. */
void fsindex_rebuild(void);

#endif
