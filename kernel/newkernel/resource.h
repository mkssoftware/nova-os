/* NovaOS C-Kernel - Resource-Management (Port aus resource32.inc, NPSPEC-RESOURCE-*). */
#ifndef NOVA_RESOURCE_H
#define NOVA_RESOURCE_H

#include <stdint.h>

#define RES_TYPE_CPU     0u
#define RES_TYPE_MEMORY  1u
#define RES_TYPE_IO      2u
#define RES_TYPE_NETWORK 3u
#define RES_TYPE_GPU     4u
#define RES_TYPE_NPU     5u
#define RES_TYPE_ENERGY  6u
#define RES_TYPE_THERMAL 7u
#define RES_TYPE_COUNT   8u

#define RES_GUAR_NONE        0u
#define RES_GUAR_BEST_EFFORT 1u
#define RES_GUAR_SOFT        2u
#define RES_GUAR_HARD        3u

/* Legt Standard-Buckets fuer CPU/Memory/IO/Network an (GPU/NPU/Energy/Thermal
   bleiben wie im alten ASM ungenutzt/leer, bis sie gebraucht werden). */
void resource_initialize(void);
int resource_self_test(void); /* 0 = ok */

/* 0 = erlaubt (Admission-Check: used+amount <= total*threshold/100),
   -1 = abgelehnt oder type >= RES_TYPE_COUNT. */
int res_request(uint32_t type, uint32_t amount);

/* Bucht amount gegen "used" zurueck (clampt auf 0 bei Underflow, wie im ASM).
   -1 nur bei ungueltigem type. */
int res_release(uint32_t type, uint32_t amount);

/* 0 = Erfolg, -1 wenn keine Kapazitaet mehr frei oder type ungueltig. */
int res_reserve(uint32_t type, uint32_t amount, uint32_t guarantee);

#endif
