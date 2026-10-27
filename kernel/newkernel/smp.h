/* NovaOS C-Kernel - SMP/AP-Aktivierung (Port aus security32.inc, §124 /
   NPSPEC-KERNEL-0027). INIT-SIPI-SIPI-Sequenz ueber den Local APIC,
   AP-Trampolin (Real->Protected Mode, ap_trampoline.asm), minimaler
   AP-Lebenszeichen-Nachweis.

   STARK REDUZIERT gegenueber dem alten ASM: KEINE volle CPU-Topologie
   (Package/Die/Cluster/NUMA - NPSPEC-KERNEL-0026 CPU Manager haengt an
   einem komplett unportierten Topology-Subsystem), KEINE LAPIC-Timer-
   Kalibrierung pro AP, KEIN Scheduler-Beitritt des APs (scheduler.c ist
   fuer Single-CPU ausgelegt - ein AP, der scheduler_on_tick parallel
   zum BSP aufruft, braeuchte echtes Locking, das hier bewusst NICHT
   nachgezogen wird). Der AP kommt hoch, meldet sich per Atomic-Zaehler
   als lebendig, und idled DAUERHAFT MIT DEAKTIVIERTEN INTERRUPTS (IF=0)
   - das ist der sicherste Zustand fuer einen zweiten Kern, der sonst
   nichts zu tun hat.

   WICHTIG - architektonische Vorsichtsmassnahme: `interrupt.c` deaktiviert
   den Local APIC global (vgl. dessen eigenen Kommentar zu UEFI/OVMF-
   Altlasten), weil IPI-Versand an APs ohne LAPIC technisch unmoeglich
   ist, MUSS `smp_initialize` ihn wieder aktivieren - aber NUR im
   "Virtual-Wire"-Kompatibilitaetsmodus (LINT0=ExtINT, unmaskiert), der
   die bestehende 8259-PIC-Zustellung fuer IRQ0 (Timer)/IRQ1 (Tastatur)
   unveraendert durchreicht. Ohne diese Vorsichtsmassnahme wuerde das
   Aktivieren des LAPIC exakt das Problem reproduzieren, das die
   urspruengliche Deaktivierung vermeiden sollte. */
#ifndef NOVA_SMP_H
#define NOVA_SMP_H

#include <stdint.h>

/* 0 = Erfolg. Bei <=1 von ACPI entdeckter CPU: reines No-Op (UP-Betrieb,
   LAPIC bleibt deaktiviert - kein Risiko fuer den PIC-Interruptpfad).
   Sonst: aktiviert den Local APIC im Virtual-Wire-Modus, kopiert den
   AP-Trampolin nach physisch 0x8000 und sendet INIT-SIPI-SIPI an jede
   zusaetzliche von ACPI entdeckte CPU. Einzelne AP-Timeouts (500ms) sind
   ein Soft-Fail (1:1 wie im alten ASM) und fuehren NICHT zu einem
   Fehler-Rueckgabewert. */
int smp_initialize(void);
int smp_self_test(void); /* 0 = ok */

/* Anzahl der CPUs, die sich tatsaechlich per ap_entry_c als lebendig
   gemeldet haben (OHNE den BSP - 0 im reinen UP-Betrieb). */
uint32_t smp_ap_online_count(void);

/* Heartbeat-Zaehler des AP an Index ap_index (0-basiert, in der
   Reihenfolge, in der die APs online kamen) - erhoeht sich fortlaufend,
   solange dieser AP laeuft. 0 wenn ap_index ausserhalb des gueltigen
   Bereichs liegt oder dieser AP nie online kam. */
uint32_t smp_ap_heartbeat_count(uint32_t ap_index);

#endif
