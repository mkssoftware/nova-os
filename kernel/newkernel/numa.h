/* NovaOS C-Kernel - Nova NUMA Topology (NPSPEC-MEMORY-NUMA-0001). Node-
   Registry + Allokations-Policy-Schicht ueber dem bestehenden pmm.h
   (`pmm_alloc_page_strict`/`pmm_alloc_page_preferred`), die STRICT
   (Anforderung #4 - harte Node-Constraints duerfen nicht automatisch
   umgangen werden) von PREFERRED (Anforderung #5 - Remote-Fallback nur
   wenn die Policy es erlaubt) unterscheidet, sowie Interleaving
   (Anforderung #6).

   BEWUSSTE SCOPE-REDUKTION: es gibt noch KEIN portiertes ACPI/SRAT-
   Subsystem (siehe pmm.c's eigener, seit Projektbeginn bestehender
   Kommentar zu `acpi_srat_memory_domain_lookup`) - echte NUMA-Topologie
   kann deshalb nicht erkannt werden. `numa_initialize()` registriert
   deshalb IMMER genau einen Node (Node 0, "lokal fuer alles") - das ist
   fuer die Zielumgebung dieses Kernels (QEMU/UEFI-Testsystem ohne
   konfigurierte NUMA-Nodes) sachlich korrekt, nicht nur ein Platzhalter.
   Die Policy-Durchsetzung (STRICT/PREFERRED/INTERLEAVED) UND die
   Introspection-API sind echt und werden bereits jetzt gegen die
   Registry geprueft - sobald ACPI/SRAT portiert ist, muss nur
   `numa_initialize()` zusaetzliche Nodes registrieren, ohne dass sich
   an Policy/Introspection etwas aendert. Device-Locality (Anforderung
   #9), Hotplug (Anforderung #11) und Speicher-/Task-Migration
   (Anforderung #7) sind nicht umgesetzt. Per-Node-Pressure (Anforderung
   #10) delegiert fuer den einzigen vorhandenen Node an pressure.h -
   echte Mehrknoten-Unterscheidung ist erst mit echter Topologie
   sinnvoll moeglich. */
#ifndef NOVA_NUMA_H
#define NOVA_NUMA_H

#include <stdint.h>

#define NUMA_NODE_CAPACITY 8u

#define NUMA_POLICY_STRICT      0u /* nur exakt preferred_node, kein Fallback */
#define NUMA_POLICY_PREFERRED   1u /* preferred_node bevorzugt, Fallback erlaubt */
#define NUMA_POLICY_INTERLEAVED 2u /* round-robin ueber alle registrierten Nodes */

void numa_initialize(void);
int numa_self_test(void); /* 0 = ok */

uint32_t numa_node_count(void);
int numa_node_exists(uint32_t node);

/* 0 = Erfolg (*out_page, *out_numa_node gesetzt), -1 = Policy verletzt
   (STRICT auf nicht existierenden/erschoepften Node) oder PMM
   erschoepft. */
int numa_alloc_page(uint32_t preferred_node, uint32_t policy, uint32_t *out_page, uint32_t *out_numa_node);

/* Introspection (Anforderung #12). */
uint32_t numa_interleave_cursor(void);
uint32_t numa_strict_denial_count(void);

#endif
