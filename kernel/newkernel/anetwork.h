/* NovaOS C-Kernel - Nova Adaptive Network Manager (Port aus
   adaptive32.inc, NPSPEC-ADAPTIVE-NETWORK-0001). Beobachtet die
   Bandbreitennutzung je Schnittstelle (EWMA) und passt die QoS-Klasse
   an: >=80% Kapazitaet -> REALTIME, >=50% -> INTERACTIVE, >=20% -> BULK,
   sonst BEST_EFFORT. NICHT zu verwechseln mit scheduler.c's QOS_CLASS_*
   (CPU-Scheduling) - ein eigenstaendiges, schon im alten ASM paralleles
   Subsystem fuer Netzwerk-Bandbreite. */
#ifndef NOVA_ANETWORK_H
#define NOVA_ANETWORK_H

#include <stdint.h>

#define ANET_CAPACITY 4u
#define ANET_ALPHA    48u

#define ANET_BEST_EFFORT 0u
#define ANET_BULK        1u
#define ANET_INTERACTIVE 2u
#define ANET_REALTIME    3u

void anetwork_initialize(void);
int anetwork_self_test(void); /* 0 = ok */

/* 0 = Erfolg (Start-QoS BEST_EFFORT), -1 wenn iface_id==0 oder Tabelle
   voll. */
int anetwork_register(uint32_t iface_id, uint32_t bw_cap);

/* 0 = Erfolg (bw_used-EWMA aktualisiert, QoS-Klasse neu berechnet aus der
   Auslastung in Prozent von bw_cap), -1 wenn iface_id unbekannt. */
int anetwork_update_bw(uint32_t iface_id, uint32_t bw_sample);

/* 0 = Erfolg (out_qos/out_bw_used/out_bw_cap gesetzt), -1 wenn iface_id
   unbekannt. */
int anetwork_query(uint32_t iface_id, uint32_t *out_qos, uint32_t *out_bw_used,
                    uint32_t *out_bw_cap);

#endif
