/* NovaOS C-Kernel - Nova Memory Pressure (NPSPEC-MEMORY-PRESSURE-0001).
   Expliziter, diskreter Pressure-Zustand ueber dem bestehenden pmm.h -
   NICHT der Reclaim-Mechanismus selbst (siehe reclaim.h/oom.h fuer die
   eigentlichen Gegenmassnahmen), sondern nur die Erkennung/Klassifikation
   des Zustands, auf die Reclaim/OOM/Allokations-Drosselung reagieren
   koennen.

   BEWUSSTE SCOPE-REDUKTION: "Global vs. NUMA-lokal" (Anforderung #4) ist
   nur als API-Unterscheidung vorhanden - `pressure_numa_level()` liefert
   aktuell denselben Wert wie `pressure_level()`, weil der Bootstrap-PMM
   fuer praktisch alle Frames `PMM_NUMA_UNKNOWN` meldet (keine echte
   NUMA-Topologie vorhanden) und deshalb keine sinnvolle per-Node-Zaehlung
   moeglich ist. Die Baseline ("Gesamtspeicher") ist eine RELATIVE
   Momentaufnahme zum Zeitpunkt von `pressure_initialize()` - zu diesem
   Zeitpunkt haben Heap/Paging/Pools bereits Frames verbraucht, die
   Prozentwerte sind also relativ zur "ab hier verfuegbaren" Menge, nicht
   zur physischen Gesamtkapazitaet der Maschine. */
#ifndef NOVA_PRESSURE_H
#define NOVA_PRESSURE_H

#include <stdint.h>

#define PRESSURE_LEVEL_NONE     0u
#define PRESSURE_LEVEL_LOW      1u
#define PRESSURE_LEVEL_MEDIUM   2u
#define PRESSURE_LEVEL_HIGH     3u
#define PRESSURE_LEVEL_CRITICAL 4u

void pressure_initialize(void);
int pressure_self_test(void); /* 0 = ok */

/* Misst den aktuellen freien PMM-Framebestand neu und aktualisiert den
   gecachten Pressure-Level. Gibt den neuen Level zurueck. */
uint32_t pressure_update(void);

/* Gecachter Level seit dem letzten pressure_update(). */
uint32_t pressure_level(void);

/* Siehe Scope-Hinweis oben - aktuell identisch zu pressure_level(). */
uint32_t pressure_numa_level(uint32_t numa_node);

/* 1 = nichtkritische Allokationen sollten bei diesem Level gedrosselt
   oder abgelehnt werden (Anforderung #9, Level >= HIGH). */
int pressure_should_throttle(void);

/* 1 = Speichererschoepfung als expliziter Fehlerzustand (Anforderung #10,
   Level == CRITICAL). */
int pressure_is_exhausted(void);

uint32_t pressure_update_count(void);

#endif
