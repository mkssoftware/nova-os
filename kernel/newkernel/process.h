/* NovaOS C-Kernel - Kernel Process Manager (Port aus process32.inc,
   ADR-2011 / NPSPEC-KERNEL-0004). */
#ifndef NOVA_PROCESS_H
#define NOVA_PROCESS_H

#include <stdint.h>

/* War 4, per echtem Boot-Test (ushell_process.c's zweiter Prozess,
   Startmenue) als zu knapp entlarvt: process.c bietet keine Freigabe-
   Funktion (Eintraege sind dauerhaft), und `process_manager_self_test()`,
   `vfsapi_self_test()` sowie `ring3_initialize()`s Demoprozess belegen
   zusammen bereits 3 der 4 Slots dauerhaft, BEVOR ueberhaupt ein
   Userspace-Shell-Prozess entsteht - fuer zwei davon (Taskbar+Startmenue)
   reichte das nicht mehr. Auf 6 angehoben (3 bestehende + 2 Shell-
   Prozesse + 1 Reserve). */
#define PROCESS_CAPACITY 6u
#define OBJECT_TYPE_PROCESS 4u

void process_manager_initialize(void);
int process_manager_self_test(void); /* 0 = ok */

/* Gibt die neue PID zurueck, oder 0 wenn die Tabelle voll ist. */
uint32_t process_create(uint32_t flags, uint32_t page_directory_cr3);

/* Gibt den internen Prozessdatensatz zurueck, oder NULL wenn PID unbekannt. */
void *process_lookup(uint32_t pid);

uint32_t process_count(void);

/* Liefert das bei process_create() hinterlegte Page-Directory (physische
   Adresse, siehe paging.h) eines Prozesses, oder 0 wenn pid unbekannt ist.
   process_lookup() gibt bewusst keinen typisierten Zeiger auf den
   internen Datensatz heraus (process_record_t ist privat in process.c) -
   dieser Getter ist der oeffentliche Zugriffsweg fuer Aufrufer, die vor
   einem Taskwechsel das richtige CR3 laden muessen (siehe ring3.c). */
uint32_t process_get_cr3(uint32_t pid);

#endif
