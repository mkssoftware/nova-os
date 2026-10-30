/* NovaOS C-Kernel - Nova Memory Reclamation (NPSPEC-MEMORY-RECLAIM-0001).
   Generisches, budgetbegrenztes Reclaim-Framework: Subsysteme registrieren
   sich mit einer Klassifikation (Discardable/Reconstructible/Active) und
   einer Callback-Funktion, die auf Anfrage eine begrenzte Anzahl Einheiten
   freigibt. `reclaim_run()` ruft registrierte Quellen in Prioritaetsreihen-
   folge (Discardable zuerst, siehe Anforderung #4) auf, bis das Budget
   erschoepft ist oder keine Quelle mehr etwas freigeben kann (Anforderung
   #10 - begrenzte Schleife).

   BEWUSSTE SCOPE-REDUKTION: "Einheiten" sind bewusst abstrakt (kein
   Frame-/Byte-Zwang) - dieses Modul ist die generische Klammer, konkrete
   Quellen (Caches, Buffer Pools, etc.) existieren in diesem Kernel noch
   nicht und werden erst bei Bedarf via reclaim_register_source()
   angebunden (z.B. ein spaeterer Page-/Buffer-Cache). Direct vs.
   Background Reclamation (Anforderungen #6/#7) sind technisch IDENTISCH
   implementiert - es gibt noch keinen echten asynchronen Hintergrund-
   Worker (kein Scheduler-Hook); der Aufrufer von `reclaim_run_direct()`
   ist dafuer verantwortlich, nicht aus einem Kontext heraufzurufen, in
   dem eine registrierte Quelle blockieren koennte (Anforderung #7 bleibt
   damit eine Aufrufer-Disziplin, keine erzwungene Eigenschaft). NUMA-
   lokale Priorisierung (Anforderung #8) ist nicht umgesetzt (siehe
   pressure.h-Scope-Hinweis: keine echte NUMA-Topologie vorhanden). */
#ifndef NOVA_RECLAIM_H
#define NOVA_RECLAIM_H

#include <stdint.h>

#define RECLAIM_SOURCE_CAPACITY 8u

#define RECLAIM_CLASS_DISCARDABLE   0u /* z.B. Caches - verwerfbar ohne Datenverlust */
#define RECLAIM_CLASS_RECONSTRUCTIBLE 1u /* rekonstruierbar, aber mit Kosten */
#define RECLAIM_CLASS_ACTIVE        2u /* NICHT reclaimbar ohne explizite Freigabe */

/* Gibt bis zu `budget` Einheiten frei und liefert die tatsaechlich
   freigegebene Anzahl zurueck (0 = nichts freigebbar). */
typedef uint32_t (*reclaim_callback_t)(uint32_t budget);

void reclaim_initialize(void);
int reclaim_self_test(void); /* 0 = ok */

/* 0 = Erfolg (*out_slot gesetzt), -1 = RECLAIM_SOURCE_CAPACITY erschoepft
   oder reclaim_class == RECLAIM_CLASS_ACTIVE (Anforderung #3 - aktive
   Daten werden niemals als Reclaim-Quelle registriert). */
int reclaim_register_source(uint32_t reclaim_class, reclaim_callback_t callback, uint32_t *out_slot);

int reclaim_unregister_source(uint32_t slot);

/* Ruft registrierte Quellen klassenpriorisiert (DISCARDABLE vor
   RECONSTRUCTIBLE) auf, bis `budget` Einheiten freigegeben wurden oder
   keine Quelle mehr etwas liefert. Liefert die Summe der tatsaechlich
   freigegebenen Einheiten. Aktualisiert die Thrashing-Erkennung
   (Anforderung #11). */
uint32_t reclaim_run(uint32_t budget);

/* Siehe Scope-Hinweis oben - technisch identisch zu reclaim_run(). */
uint32_t reclaim_run_direct(uint32_t budget);
uint32_t reclaim_run_background(uint32_t budget);

/* 1 = die letzten RECLAIM_THRASHING_WINDOW Laeufe haben kumuliert fast
   nichts freigegeben (Anforderung #11). */
int reclaim_is_thrashing(void);

uint32_t reclaim_run_count(void);
uint32_t reclaim_total_reclaimed(void);

#endif
