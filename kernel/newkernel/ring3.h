/* NovaOS C-Kernel - Ring-3-Bootstrap (Port der Kernidee aus
   io_qos32.inc's `userspace_initialize`/`userspace_enter`, stark
   reduziert: nur der Mechanismus, kein vollstaendiges Explorer/Shell-
   Programm - das braucht Thread-Manager/VFS/Security, die noch nicht
   portiert sind). Baut eine GDT-Erweiterung um Ring-3-Code/Daten-
   Deskriptoren + TSS, laedt die TSS (ltr), mappt ein minimales
   Ring-3-Testprogramm + Stack user-zugaenglich (PAGING_PAGE_USER) und
   registriert den zugehoerigen Prozess. `ring3_enter` fuehrt den
   eigentlichen Ring0->Ring3-Sprung per `iret` aus - das ist, wie bei
   jedem echten Kernel beim allerersten Usermode-Handoff, eine EINWEG-
   Operation (kehrt nie normal zu C zurueck). Das Testprogramm sendet
   fortlaufend einen Heartbeat-Syscall (int 0x80, EAX="RIN3"-Magic), den
   `interrupt_dispatch_c` beim ersten Empfang einmalig auf die serielle
   Konsole protokolliert - das ist der eigentliche Beweis, dass Ring 3
   erreicht wurde (siehe interrupt.c). */
#ifndef NOVA_RING3_H
#define NOVA_RING3_H

#include <stdint.h>

/* 0 = Erfolg (GDT/TSS geladen, Code+Stack-Seite user-gemappt, Prozess
   registriert), -1 wenn eine Seite/der Prozess-Slot nicht verfuegbar ist. */
int ring3_initialize(void);

/* Statische Pruefung von GDT/TSS/Prozess-Zustand (per sgdt/str sowie
   gespeichertem Status) - die eigentliche Verhaltenspruefung liefert der
   Heartbeat-Syscall im Boot-Log, siehe oben. 0 = ok. */
int ring3_self_test(void);

/* EINWEG: baut den Ring0->Ring3-IRET-Frame und springt in das
   Testprogramm. Kehrt nicht zurueck. */
void ring3_enter(void) __attribute__((noreturn));

#endif
