/* NovaOS C-Kernel - Nova Adaptive Preload Manager (Port aus
   adaptive32.inc, NPSPEC-ADAPTIVE-PRELOAD-0001). Reiht Objekte nach
   Prioritaet zum Preload ein; hoechste Prioritaet wird zuerst geladen.
   `apreload_complete` registriert/aktualisiert konzeptionell `acache.c`
   (obj_id als Cache-Line) als Treffer - echte Cross-Modul-Verdrahtung,
   1:1 wie im alten ASM. */
#ifndef NOVA_APRELOAD_H
#define NOVA_APRELOAD_H

#include <stdint.h>

#define APRELOAD_CAPACITY 8u

#define APRELOAD_PENDING 0u
#define APRELOAD_LOADING 1u
#define APRELOAD_READY   2u

void apreload_initialize(void);
int apreload_self_test(void); /* 0 = ok */

/* 0 = Erfolg, -1 wenn obj_id==0 oder Tabelle voll. */
int apreload_enqueue(uint32_t obj_id, uint32_t priority);

/* 0 = Erfolg (*out_obj_id gesetzt, Status->LOADING), -1 wenn keine
   PENDING-Objekte vorhanden sind. */
int apreload_dequeue_highest(uint32_t *out_obj_id);

/* 0 = Erfolg (Status->READY, `acache.c` ueber obj_id als Treffer
   informiert), -1 wenn obj_id unbekannt. */
int apreload_complete(uint32_t obj_id);

#endif
