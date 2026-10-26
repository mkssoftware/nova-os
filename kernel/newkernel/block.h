/* NovaOS C-Kernel - Storage Block Layer (Port aus block32.inc,
   NPSPEC-STORAGE-BLOCK-0001 / NPSPEC-STORAGE-HOTPLUG-0001 /
   NPSPEC-STORAGE-INTEGRATION-0001). */
#ifndef NOVA_BLOCK_H
#define NOVA_BLOCK_H

#include <stdint.h>

#define BLOCK_TRANSPORT_AHCI 0u
#define BLOCK_TRANSPORT_NVME 1u
#define BLOCK_TRANSPORT_USB  2u
#define BLOCK_TRANSPORT_VIRT 3u

#define BLOCK_FLAG_READONLY  0x00000001u
#define BLOCK_FLAG_REMOVABLE 0x00000002u
#define BLOCK_FLAG_FLUSH_CAP 0x00000004u
#define BLOCK_FLAG_ONLINE    0x00000008u
#define BLOCK_FLAG_DEGRADED  0x00000010u

#define BLOCK_HEALTH_OK      0u
#define BLOCK_HEALTH_WARN    1u
#define BLOCK_HEALTH_FAIL    2u
#define BLOCK_HEALTH_UNKNOWN 3u

#define BLOCK_CAPACITY 8u

#define BLK_EVT_ADDED         1u
#define BLK_EVT_REMOVED       2u
#define BLK_EVT_MEDIA_CHANGED 3u
#define BLK_EVT_OFFLINE       4u
#define BLK_EVT_RECOVERED     5u

#define BLK_IO_READ  0u
#define BLK_IO_WRITE 1u
#define BLK_IO_FLUSH 2u

/* record = Zeiger auf den Block-Device-Record (siehe block_device_lookup_by_handle). */
typedef int (*block_io_fn_t)(void *record, uint32_t lba_lo, uint32_t lba_hi,
                              uint32_t count, void *buffer);
typedef int (*block_flush_fn_t)(void *record);
typedef int (*block_offline_fn_t)(void *record);

typedef struct {
    uint32_t id;
    uint32_t flags;
    uint32_t block_size;
    uint32_t block_count_lo;
    uint32_t block_count_hi;
    uint32_t transport;
    uint32_t driver_handle;
} block_register_params_t;

void block_layer_initialize(void);
int block_layer_self_test(void); /* 0 = ok */

/* 0 = Erfolg (*out_record erhaelt den Record-Zeiger), -1 wenn voll. Setzt
   BLOCK_FLAG_ONLINE automatisch (wie im alten ASM). */
int block_device_register(const block_register_params_t *params, block_io_fn_t read_fn,
                           block_io_fn_t write_fn, block_flush_fn_t flush_fn,
                           block_offline_fn_t offline_fn, void **out_record);

/* Setzt das Geraet offline (BLOCK_FLAG_ONLINE geloescht, DEGRADED gesetzt). */
void block_device_offline(void *record);

/* 0 = Erfolg, -1 = Fehler (Offline, Readonly, Out-of-Range, fehlende
   Funktion, I/O-Fehler aus der jeweiligen Funktion). */
int block_io_submit(void *record, uint32_t lba_lo, uint32_t lba_hi, uint32_t count,
                     void *buffer, uint32_t direction);

/* 0 = Erfolg (*out_record erhaelt den Record-Zeiger), -1 wenn nicht gefunden. */
int block_device_lookup_by_handle(uint32_t driver_handle, void **out_record);

/* Liest das driver_handle-Feld eines Record-Zeigers zurueck (z.B. fuer
   Block-I/O-Callbacks, die aus dem opaken record* ihren Treiber-Kontext
   wiederfinden muessen, wie im alten ASM via BLK_DRIVER_HANDLE). */
uint32_t block_record_driver_handle(void *record);

#endif
