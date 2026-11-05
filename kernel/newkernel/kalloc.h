/* NovaOS C-Kernel - Nova General Kernel Allocator. Antwort auf den
   wiederkehrenden Architektur-Punkt "viele feste Kapazitaetsgrenzen
   koennen nicht dynamisch geloest werden" (Nutzer, 2026-11-05): fast
   jedes bisherige Modul dieses Kernels (Scene Graph/Layer/Dependency
   Graph/Material-Instanzen/Prozesstabelle/...) verwendet feste statische
   Arrays, weil kein General-Purpose-Allocator existierte - die
   vorhandenen `heap.c` (Bootstrap-Bump-Allocator, NIE Free, max. EINE
   PMM-Seite pro Allokation) und `slab.c` (vier feste Groessenklassen bis
   128 Byte) sind dafuer zu eingeschraenkt.

   `kalloc()`/`kfree()` sind der hier bewusst ERSTE Schritt: ein echter,
   generischer Allocator (beliebige Groesse, mit Free und Coalescing),
   der auf `pmm_alloc_contiguous()` waechst. Er macht noch KEIN bisheriges
   Modul selbst dynamisch (das bleibt eine spaetere, modulweise
   Entscheidung) - er ist die Voraussetzung dafuer.

   Architektur: klassischer Freilisten-Allocator mit intrusiven, nach
   Adresse sortierten Block-Headern (First-Fit, Split beim Allozieren,
   Coalescing mit physisch direkt angrenzenden Nachbarn beim Freigeben).
   SMP-/IRQ-sicher (sync.h-Spinlock um jede Operation, gleiches Muster
   wie serial.c). Waechst bei Bedarf um exakt so viele zusammenhaengende
   PMM-Seiten, wie fuer die aktuelle Anfrage noetig sind (keine
   Vorab-Reservierung/kein Shrink der Arena - einmal von PMM bezogene
   Seiten bleiben dauerhaft Teil des kalloc-Pools, auch wenn alles
   darin wieder freigegeben wurde). */
#ifndef NOVA_KALLOC_H
#define NOVA_KALLOC_H

#include <stdint.h>

#define KALLOC_ALIGNMENT 16u

void kalloc_initialize(void);
int kalloc_self_test(void); /* 0 = ok */

/* NULL bei size==0, Ueberlauf, oder PMM-Erschoepfung (keine zusammen-
   haengenden Seiten mehr verfuegbar). Ergebnis ist KALLOC_ALIGNMENT-
   ausgerichtet. */
void *kalloc(uint32_t size);

/* No-Op bei ptr==NULL. ptr MUSS von kalloc()/kcalloc()/krealloc()
   stammen - kein Schutz gegen Doppel-Free oder fremde Zeiger (wie bei
   jedem einfachen malloc/free-Paar, siehe Dateikopf: bewusst einfach
   gehalten). */
void kfree(void *ptr);

/* kalloc(count*size), vorab auf 0 gesetzt. NULL bei Ueberlauf von
   count*size oder wie kalloc(). */
void *kcalloc(uint32_t count, uint32_t size);

/* Neuallokation + Kopie der ersten min(alte, neue Groesse) Bytes +
   Freigabe des alten Blocks - KEINE In-Place-Erweiterung (bewusste
   Vereinfachung, siehe Dateikopf-SOLL/MUSS-Reduktionsmuster dieses
   Projekts). ptr==NULL verhaelt sich wie kalloc(new_size), new_size==0
   wie kfree(ptr) (liefert NULL). NULL bei Erschoepfung - der ALTE Block
   bleibt in diesem Fall unveraendert gueltig (wie bei realloc() ueblich). */
void *krealloc(void *ptr, uint32_t new_size);

/* Introspektion fuer Selbsttest/Diagnose. */
uint32_t kalloc_bytes_in_use(void);
uint32_t kalloc_allocation_count(void);

#endif
