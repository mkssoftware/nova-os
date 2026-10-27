/* NovaOS C-Kernel - Nova Resource Arbitration (Port aus adaptive32.inc,
   NPSPEC-RESOURCE-ARBITRATION-0001). Prioritaetsbasierter Arbiter:
   mehrere Anfragende konkurrieren um eine gemeinsame Ressource; der
   Anfragende mit der hoechsten Prioritaet gewinnt. Verbindet sich
   konzeptionell mit radmit.h/rguarant.h - Verdrahtung selbst NICHT
   portiert. */
#ifndef NOVA_RARBIT_H
#define NOVA_RARBIT_H

#include <stdint.h>

#define RARBIT_CAPACITY 8u

void rarbit_initialize(void);
int rarbit_self_test(void); /* 0 = ok */

/* 0 = Erfolg (*out_slot gesetzt), -1 wenn req_id==0 oder Tabelle voll. */
int rarbit_request(uint32_t req_id, uint32_t resource_id, uint32_t priority, uint32_t *out_slot);

/* Vergibt die Ressource an die hoechste noch nicht vergebene Prioritaet
   fuer resource_id. 0 = Erfolg (*out_granted_req_id gesetzt), -1 wenn
   keine passende offene Anfrage existiert. */
int rarbit_arbitrate(uint32_t resource_id, uint32_t *out_granted_req_id);

#endif
