/* NovaOS C-Kernel - Nova Resource Reservation (Port aus adaptive32.inc,
   NPSPEC-RESOURCE-RESERVATION-0001). Pre-Admit-Resource-Holds: ein
   Aufrufer reserviert CPU+Memory bevor die eigentliche Arbeit beginnt,
   um Kontention zu reduzieren. Verbindet konzeptionell rguarant.h
   (Untergrenze) mit radmit.h (Obergrenze) - eine Reservierung ist ein
   weicher Hold, der zu einer vollen Admission befoerdert werden kann.
   Diese Verdrahtung selbst ist NICHT portiert. */
#ifndef NOVA_RRESV_H
#define NOVA_RRESV_H

#include <stdint.h>

#define RRESV_CAPACITY 8u

#define RRESV_PENDING  0u
#define RRESV_ACTIVE   1u
#define RRESV_RELEASED 2u

void rresv_initialize(void);
int rresv_self_test(void); /* 0 = ok */

/* 0 = Erfolg (*out_resv_id gesetzt, Status PENDING), -1 wenn owner_id==0
   oder Tabelle voll. */
int rresv_reserve(uint32_t owner_id, uint32_t cpu_hold, uint32_t mem_hold, uint32_t *out_resv_id);

/* 0 = befoerdert auf ACTIVE, -1 wenn resv_id unbekannt oder nicht PENDING. */
int rresv_activate(uint32_t resv_id);

/* 0 = Erfolg (Status RELEASED), -1 wenn resv_id unbekannt. */
int rresv_release(uint32_t resv_id);

#endif
