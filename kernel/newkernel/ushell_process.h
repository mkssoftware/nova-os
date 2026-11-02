/* NovaOS C-Kernel - Userspace-Taskbar-Prozess-Lader (erster Schritt der
   schrittweisen Desktop-Shell-Portierung nach Ring 3, siehe
   kernel/newkernel/ushell/shell_main.c fuer das eigentliche Programm).

   Baut, analog zu ring3.c's Demoprozess, aber mit einem ECHTEN, in
   ushell/shell_main.c kompilierten Programm statt handgeschriebenem
   Shellcode: einen eigenstaendigen Hardware-Adressraum (paging_create_
   address_space), mappt Code/Stack/Args/GUI-Puffer hinein und
   registriert einen ECHTEN, praeemptiv geplanten Scheduler-Slot (siehe
   scheduler_claim_dynamic_user_slot in scheduler.h) - anders als der
   Ring-3-Demoprozess in ring3.c (der EINMALIG direkt per iret einspringt
   und bis SYSCALL_CORE_EXIT blockiert) muss die Taskbar DAUERHAFT
   nebenlaeufig neben allen anderen Kernel-Threads laufen, um Uhrzeit/
   Eingaben fortlaufend abzufragen.

   Scope-Grenze: nur die Taskbar (dieser erste Schritt) - Startmenue/
   Icons/Notifications bleiben vorerst im Kernel (desktop_shell.c), die
   Taskbar-eigene Erzeugung/Neuzeichnung dort wurde entsprechend entfernt
   (siehe desktop_shell.c Scope-Hinweis). */
#ifndef NOVA_USHELL_PROCESS_H
#define NOVA_USHELL_PROCESS_H

/* 0 = Erfolg (eigener Adressraum, Code/Stack/Args/GUI-Puffer gemappt,
   Prozess registriert, Scheduler-Slot fuer den praeemptiven Betrieb
   reserviert - laeuft automatisch mit, sobald scheduler_enable()
   aufgerufen wird), -1 bei Fehler (z.B. kein freier PMM-Rahmen/
   Scheduler-Slot mehr, oder display_width()/_height() noch 0). Muss VOR
   ring3_enter() aufgerufen werden (siehe kmain.c) - danach kehrt die
   Boot-Sequenz nicht mehr normal zu C zurueck (siehe ring3.h). */
int ushell_process_initialize(void);

int ushell_process_self_test(void); /* 0 = ok */

#endif
