/* NovaOS C-Kernel - Nova Adaptive Storage Manager (Port aus
   adaptive32.inc, NPSPEC-ADAPTIVE-STORAGE-0001). Beobachtet die I/O-Rate
   je Volume (EWMA) und waehlt daraus eine Speicher-Tier
   (HOT=schnelles NVMe / WARM=SSD / COLD=HDD/remote). */
#ifndef NOVA_ASTORAGE_H
#define NOVA_ASTORAGE_H

#include <stdint.h>

#define ASTOR_CAPACITY 8u
#define ASTOR_ALPHA    32u

#define ASTOR_HOT  0u
#define ASTOR_WARM 1u
#define ASTOR_COLD 2u

void astorage_initialize(void);
int astorage_self_test(void); /* 0 = ok */

/* 0 = Erfolg (Start-Tier WARM, io_rate=50), -1 wenn vol_id==0 oder
   Tabelle voll. */
int astorage_register(uint32_t vol_id);

/* 0 = Erfolg (io_rate-EWMA aktualisiert, Tier neu berechnet,
   total_ops aufaddiert), -1 wenn vol_id unbekannt. */
int astorage_record_io(uint32_t vol_id, uint32_t ops_this_tick);

/* 0 = Erfolg (out_tier/out_io_rate/out_total_ops gesetzt), -1 wenn vol_id
   unbekannt. */
int astorage_query(uint32_t vol_id, uint32_t *out_tier, uint32_t *out_io_rate,
                    uint32_t *out_total_ops);

#endif
