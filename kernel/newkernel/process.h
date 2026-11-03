/* NovaOS C-Kernel - Kernel Process Manager (Port aus process32.inc,
   ADR-2011 / NPSPEC-KERNEL-0004). */
#ifndef NOVA_PROCESS_H
#define NOVA_PROCESS_H

#include <stdint.h>

/* War 4, per echtem Boot-Test (ushell_process.c's zweiter Prozess,
   Startmenue) als zu knapp entlarvt: `process_manager_self_test()`,
   `vfsapi_self_test()` sowie `ring3_initialize()`s Demoprozess belegen
   zusammen bereits 3 der 4 Slots dauerhaft, BEVOR ueberhaupt ein
   Userspace-Shell-Prozess entsteht - fuer zwei davon (Taskbar+Startmenue)
   reichte das nicht mehr. Auf 6 angehoben (3 bestehende + 2 Shell-
   Prozesse + 1 Reserve), spaeter auf 7 (3 bestehende + 3 Shell-Prozesse
   Taskbar+Startmenue+Icons + 1 Reserve), als ushell_process.c einen
   dritten dauerhaften Prozess (Desktop-Icons) bekam. Seit
   NPSPEC-PROZESS-LIFECYCLE-0001 (process_terminate()/process_reap(),
   siehe unten) koennen Slots wieder freigegeben werden - die aktuell
   laufenden Kernel-Prozesse terminieren im normalen Boot-Betrieb aber nie
   von selbst, die Kapazitaetsrechnung oben bleibt also weiterhin gueltig. */
#define PROCESS_CAPACITY 7u
#define OBJECT_TYPE_PROCESS 4u

/* NPSPEC-PROZESS-LIFECYCLE-0001: explizite Lebenszyklus-Zustaende statt
   nur belegt/frei. EMPTY = Slot frei/wiederverwendbar. */
#define PROCESS_STATE_EMPTY      0u
#define PROCESS_STATE_RUNNING    1u
#define PROCESS_STATE_SUSPENDED  2u
#define PROCESS_STATE_TERMINATED 3u

void process_manager_initialize(void);
int process_manager_self_test(void); /* 0 = ok */

/* Gibt die neue PID zurueck, oder 0 wenn die Tabelle voll ist. */
uint32_t process_create(uint32_t flags, uint32_t page_directory_cr3);

/* PROCESS-LIFECYCLE-0001 #1: aktueller Zustand (PROCESS_STATE_*), oder
   PROCESS_STATE_EMPTY wenn pid unbekannt. */
uint32_t process_state(uint32_t pid);

/* #6: RUNNING -> SUSPENDED. 0 = Erfolg, -1 wenn pid unbekannt oder nicht
   RUNNING. */
int process_suspend(uint32_t pid);

/* Gegenstueck zu process_suspend: SUSPENDED -> RUNNING. 0 = Erfolg,
   -1 wenn pid unbekannt oder nicht SUSPENDED. */
int process_resume(uint32_t pid);

/* #7/#12: RUNNING|SUSPENDED -> TERMINATED, hinterlegt exit_status (per
   process_exit_status() abrufbar). Der Slot bleibt bis process_reap()
   belegt (Exit-Status bleibt abrufbar). 0 = Erfolg, -1 wenn pid unbekannt
   oder bereits TERMINATED. */
int process_terminate(uint32_t pid, uint32_t exit_status);

/* #12: hinterlegter Exit-Status eines TERMINATED-Prozesses, 0 sonst/wenn
   unbekannt. */
uint32_t process_exit_status(uint32_t pid);

/* #8: gibt den Slot eines TERMINATED-Prozesses fuer process_create() frei
   (PROCESS_STATE_EMPTY) - erst HIER verschwindet der Exit-Status. 0 =
   Erfolg, -1 wenn pid unbekannt oder nicht TERMINATED. */
int process_reap(uint32_t pid);

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

/* Umkehrfunktion zu process_get_cr3(): findet die PID, deren Page-
   Directory AKTUELL (per paging_root_physical()) das gegebene cr3 ist -
   der kanonische Weg, aus einem Syscall-Trap-Frame heraus herauszufinden,
   WELCHER Ring-3-Prozess gerade aufgerufen hat (jeder Prozess laeuft unter
   seinem eigenen Adressraum, der Syscall-ISR schaltet NICHT auf den
   Kernel-Basis-Adressraum zurueck, siehe ring3_enter()-Kommentar). 0 wenn
   kein RUNNING/SUSPENDED-Prozess dieses cr3 traegt. */
uint32_t process_find_by_cr3(uint32_t cr3);

#endif
