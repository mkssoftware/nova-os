/* NovaOS C-Kernel - Nova Progressive Readiness (Port aus integration32.inc,
   §37/§38, NPSPEC-KERNEL-0002/-0038). Initialisiert "aufschiebbare"
   Subsysteme (Netzwerk, Power-Management) asynchron in einem eigenen
   Hintergrund-Thread, parallel zum Ring-3-Rundweg, statt den sequenziellen
   Boot-Pfad dafuer zu blockieren - mit einem synchronen Fallback, falls
   der Thread nicht rechtzeitig zum Zug kommt.

   WICHTIG (Abweichung vom alten ASM): das Original ruft fuer den
   Netzwerk-Teil `network_manager_initialize`/`_self_test` - das ist der
   GROSSE, mit unserem bereits portierten `network.c` (aus `network32.inc`)
   ueberlappende ZWEITE Netzwerk-Stack aus `integration32.inc` selbst
   (ICMP/TCP/UDP/IPv6/Firewall/Loopback), der bewusst NICHT portiert ist
   (Namenskollisionsrisiko, siehe project_c_kernel_migration_status). Dieser
   Port verwendet stattdessen den bereits vorhandenen, echten
   `network_stack_initialize()`/`network_stack_self_test()` (network.h) -
   funktional aequivalent fuer den Zweck dieses Moduls (ein echtes,
   aufschiebbares Subsystem asynchron hochfahren), ohne den riskanten
   zweiten Stack extra zu portieren. Der Power-Teil nutzt unveraendert
   das bereits portierte `power_manager_initialize()`/`_self_test()`
   (powermgr.h) - exakter Namens-Treffer mit dem Original. */
#ifndef NOVA_READINESS_H
#define NOVA_READINESS_H

#include <stdint.h>

#define COMP_STATE_NOT_STARTED  0u
#define COMP_STATE_INITIALIZING 1u
#define COMP_STATE_READY        2u
#define COMP_STATE_DEGRADED     3u
#define COMP_STATE_FAILED       4u
#define COMP_STATE_WAITING      5u

void readiness_initialize(void);
int readiness_self_test(void); /* 0 = ok */

/* Registriert den Hintergrund-Thread (thread_create_dynamic) - der Thread
   wartet zunaechst (busy-wait auf readiness_trigger()), tut also noch
   nichts. 0 = Erfolg, -1 wenn kein freier Scheduler-Slot mehr frei ist
   (dann muss der Aufrufer readiness_sync_fallback() definitiv selbst
   aufrufen, da kein Hintergrund-Thread existiert, der es je tun koennte). */
int readiness_start_background_init(void);

/* Gibt den Hintergrund-Thread frei, tatsaechlich mit der Initialisierung
   zu beginnen (setzt bg_init_trigger). Erst ab hier verbraucht der Thread
   echte Scheduler-Zeit fuer echte Arbeit. */
void readiness_trigger(void);

/* Vom Hauptthread aufzurufen, NACHDEM `scheduler_enable()` gelaufen ist
   (also der Hintergrund-Thread ueberhaupt eine Chance hatte zu laufen) -
   siehe syscall.c's syscall_userspace_exit(). Fuehrt die Initialisierung
   synchron zu Ende, falls der Hintergrund-Thread (noch) nicht fertig war
   (bereits fertig abgeschlossene Teile werden nicht wiederholt). Idempotent -
   mehrfacher Aufruf nach Abschluss ist ein No-Op. */
void readiness_sync_fallback(void);

int readiness_is_complete(void);
uint32_t readiness_network_state(void);
uint32_t readiness_power_state(void);
/* Tick-Snapshot (timer_get_ticks()), zu dem die Initialisierung
   abgeschlossen wurde - 0 solange readiness_is_complete() falsch ist. */
uint32_t readiness_core_services_ticks(void);

#endif
