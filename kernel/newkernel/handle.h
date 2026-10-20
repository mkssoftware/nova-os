/* NovaOS C-Kernel - Prozesslokaler Handle Manager (Port aus memory32.inc,
   NPSPEC-KERNEL-0013). Baut auf dem Object Manager auf. */
#ifndef NOVA_HANDLE_H
#define NOVA_HANDLE_H

#include <stdint.h>

#define HANDLE_CAPACITY 16u

#define HANDLE_RIGHT_QUERY   0x00000001u
#define HANDLE_RIGHT_WAIT    0x00000002u
#define HANDLE_RIGHT_SEND    0x00000004u
#define HANDLE_RIGHT_RECEIVE 0x00000008u
#define HANDLE_RIGHT_BIND    0x00000010u
#define HANDLE_RIGHT_CONNECT 0x00000020u
#define HANDLE_FLAG_PROTECTED 0x00000010u

void handle_manager_initialize(void);
int handle_manager_self_test(void); /* 0 = ok */

/* Gibt einen prozesslokalen, generationsgeschuetzten Handle zurueck, oder 0
   bei Fehler (falsche PID, Typ-Mismatch, Tabelle voll). */
uint32_t handle_create(uint32_t pid, uint32_t object_handle, uint32_t type,
                        uint32_t rights, uint32_t flags);

/* Gibt die interne Objektadresse zurueck (nur fuer Kernel-Gebrauch), oder
   NULL wenn PID/Handle/Typ/Rechte nicht passen. */
void *handle_resolve(uint32_t pid, uint32_t handle, uint32_t expected_type,
                      uint32_t required_rights);

/* Geschuetzte Handles (HANDLE_FLAG_PROTECTED) koennen nicht geschlossen
   werden. 0 = Erfolg. */
int handle_close(uint32_t pid, uint32_t handle);

uint32_t handle_active_count(void);

#endif
