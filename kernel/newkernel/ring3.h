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
   Operation (kehrt nie normal zu C zurueck). Das Testprogramm ruft zwei
   echte Syscalls ueber syscall.c's Dispatch auf: zuerst
   SYSCALL_CORE_READY (Heartbeat, einmalig auf der seriellen Konsole
   protokolliert - der Beweis, dass Ring 3 erreicht wurde und iretd
   korrekt dorthin zurueckkehrt), danach SYSCALL_CORE_EXIT (der Dispatch
   schreibt den Trap-Frame so um, dass iretd stattdessen nach Ring 0
   zurueckkehrt - der Beweis, dass die Privilegstufen-Rueckkehr per
   Frame-Rewrite funktioniert, siehe syscall.c). */
#ifndef NOVA_RING3_H
#define NOVA_RING3_H

#include <stdint.h>

/* Feste, user-zugaengliche Pufferseite fuer den einzigen bisher
   existierenden Ring-3-Client (GUI-Syscall-Service, siehe syscall.h/.c) -
   bereits in ring3_initialize() mitgemappt (PRESENT|WRITE|USER), damit der
   eigentliche Syscall-Dispatch (laeuft mit IF=0 im ISR-Kontext) NIE selbst
   pmm_alloc_page/paging_map_page aufrufen muss. Liegt innerhalb des
   bestehenden Bootstrap-Userspace-Fensters [USER_ADDRESS_MIN,
   USER_ADDRESS_MAX) aus syscall.h. Mehrere Clients/Surfaces teilen sich
   vorerst diese EINE physische Seite (reicht fuer den Einzel-Client-Beweis
   dieser Sitzung) - echte Pro-Surface-Puffer brauchen dynamische
   Seitenzuteilung pro Client, bewusst nicht in dieser Sitzung geloest. */
#define RING3_GUI_BUFFER_VADDR 0x00402000u

/* GDT-Selektoren der von ring3_initialize() einmalig aufgebauten GDT-
   Erweiterung (RPL=3 bereits in den unteren 2 Bits enthalten) - oeffentlich,
   da JEDER kuenftige Ring-3-Prozess (nicht nur ring3.c's eigenes
   Testprogramm) dieselbe GDT/TSS wiederverwendet, um nach Ring 3 zu
   springen (siehe scheduler_claim_dynamic_user_slot in scheduler.h und
   ushell_process.c fuer den ersten weiteren Verwender). */
/* KEIN 'u'-Suffix - ring3.c's STR()-Makro stringifiziert diese Werte
   direkt in Inline-Assembler-Operanden ("push $" STR(...)), wo ein
   C-Suffix den Assembler-Parser bricht. */
#define RING3_CODE_SELECTOR 0x1B
#define RING3_DATA_SELECTOR 0x23

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
