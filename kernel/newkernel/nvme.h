/* NovaOS C-Kernel - NVMe PCIe Controller Driver (Port aus nvme32.inc,
   NPSPEC-NVME-ARCHITECTURE-0001 und verwandte NPSPEC-NVME-Spezifikationen). */
#ifndef NOVA_NVME_H
#define NOVA_NVME_H

#include <stdint.h>

#define NVME_STATE_UNKNOWN    0u
#define NVME_STATE_DISABLED   1u
#define NVME_STATE_ADMINREADY 2u
#define NVME_STATE_RUNNING    3u
#define NVME_STATE_DEGRADED   4u
#define NVME_STATE_FAILED     5u

#define NVME_CAPACITY    2u
#define NVME_ADMIN_DEPTH 16u
#define NVME_IO_DEPTH    32u

/* Durchsucht die Driver-Discovery-Tabelle (driver.c) nach NVMe-Controllern
   und initialisiert jeden vollstaendig (Admin-Queue, Identify, I/O-Queues,
   Namespace 1 als Block-Device). Fehlende NVMe-Hardware ist kein Fehler
   (immer 0). */
int nvme_initialize(void);
int nvme_self_test(void); /* 0 = ok (kein NVMe = ok, NPSPEC-konform) */

#endif
