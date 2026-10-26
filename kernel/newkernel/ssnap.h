/* NovaOS C-Kernel - Nova State Snapshot (Port aus adaptive32.inc,
   NPSPEC-STATE-SNAPSHOT-0001). Erfasst und stellt versionierte Snapshots
   von Kernel-State-Dwords wieder her. Verbindet das txroll.h-Save/Restore-
   Muster mit einer State-Manager-Versionierung (hier vereinfacht: nur
   Version=1 pro Capture, wie im alten ASM). */
#ifndef NOVA_SSNAP_H
#define NOVA_SSNAP_H

#include <stdint.h>

#define SSNAP_CAPACITY 4u

void ssnap_initialize(void);
int ssnap_self_test(void); /* 0 = ok */

/* 0 = Erfolg (*out_snap_id gesetzt), -1 wenn Tabelle voll. */
int ssnap_capture(uint32_t *address, uint32_t *out_snap_id);
/* 0 = Erfolg (Originalwert wiederhergestellt, Slot invalidiert), -1 wenn
   snap_id unbekannt. */
int ssnap_restore(uint32_t snap_id);

#endif
