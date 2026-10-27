/* NovaOS C-Kernel - Restriktiver Kernel Module Loader (Port aus
   sync32.inc, NPSPEC-KERNEL-0025). Validiert ein vollstaendiges, bereits
   aus einer verifizierten Quelle gelesenes Modulpaket (Magic/Header-
   Groesse/Image-Groesse/Architektur/ABI-Bereich/Pflicht-Flags/Additions-
   Rotations-Digest/ueberlappungsfreie Code-/Datensektionen/einfache
   Abhaengigkeitskette), kopiert Code/Daten in feste 64-Byte-Bereiche und
   fuehrt einen Lade-/Entlade-Zustandsautomaten. Unterstuetzt bewusst nur
   EINEN aktiven Modul-Slot (Bootstrap-Einschraenkung, 1:1 wie im alten
   ASM). */
#ifndef NOVA_MODLOADER_H
#define NOVA_MODLOADER_H

#include <stdint.h>

#define MODULE_MAGIC          0x444D564Eu /* "NVMD" */
#define MODULE_HEADER_SIZE    64u
#define MODULE_MAX_IMAGE_SIZE 4096u
#define MODULE_ARCH_X86_32    1u
#define MODULE_KERNEL_ABI     0x00010000u

#define MODULE_FLAG_TRUSTED     0x00000001u
#define MODULE_FLAG_STACK_GUARD 0x00000002u
#define MODULE_FLAG_UNLOADABLE  0x00000004u
#define MODULE_REQUIRED_FLAGS   (MODULE_FLAG_TRUSTED | MODULE_FLAG_STACK_GUARD)

#define MODULE_STATE_DISCOVERED   0u
#define MODULE_STATE_VALIDATING   1u
#define MODULE_STATE_LOADING      2u
#define MODULE_STATE_RELOCATING   3u
#define MODULE_STATE_INITIALIZING 4u
#define MODULE_STATE_ACTIVE       5u
#define MODULE_STATE_QUIESCING    6u
#define MODULE_STATE_UNLOADING    7u
#define MODULE_STATE_FAILED       8u
#define MODULE_STATE_UNLOADED     9u

void module_loader_initialize(void);
int module_loader_self_test(void); /* 0 = ok */

/* package zeigt auf ein vollstaendiges Modulpaket (Header + Code- +
   Datensektion). 0 = Erfolg (Modul jetzt ACTIVE), -1 = abgelehnt
   (module_state = FAILED, failed_count erhoeht). */
int module_load(const uint8_t *package);

/* 0 = Erfolg (Modul jetzt UNLOADED), -1 wenn nicht ACTIVE, nicht
   UNLOADABLE-markiert oder active_calls != 0. */
int module_unload(void);

#endif
