/* NovaOS C-Kernel - Nova Adaptive Cache Manager (Port aus adaptive32.inc,
   NPSPEC-ADAPTIVE-CACHE-0001). Verfolgt Cache-Line-Nutzung und passt
   Eviction-Entscheidungen anhand einer EWMA-Trefferrate an. Faellt die
   Trefferrate unter einen Schwellwert, signalisiert es `aschedsw.c`
   (Klasse 3, feste Delta -48) eine Gewichtsreduktion - echte Cross-
   Modul-Verdrahtung, 1:1 wie im alten ASM. */
#ifndef NOVA_ACACHE_H
#define NOVA_ACACHE_H

#include <stdint.h>

#define ACACHE_CAPACITY  8u
#define ACACHE_ALPHA     32u  /* EWMA-Glaettungsfaktor (von 256) */
#define ACACHE_THRESHOLD 64u  /* Trefferrate darunter -> Feedback */

void acache_initialize(void);
int acache_self_test(void); /* 0 = ok */

/* 0 = Erfolg (Startwert hit_rate=128, 50%), -1 wenn line_id==0, bereits
   registriert, oder Tabelle voll. */
int acache_register(uint32_t line_id);

/* is_hit: 1 = Treffer, 0 = Fehlzugriff. Aktualisiert die EWMA-Trefferrate
   und ruft bei Unterschreiten von ACACHE_THRESHOLD `aschedsw_feedback`
   auf. 0 = Erfolg, -1 wenn line_id unbekannt. */
int acache_record(uint32_t line_id, uint32_t is_hit);

/* 0 = Erfolg (out_hit_rate/out_hits/out_misses gesetzt), -1 wenn line_id
   unbekannt. */
int acache_query(uint32_t line_id, uint32_t *out_hit_rate, uint32_t *out_hits, uint32_t *out_misses);

#endif
