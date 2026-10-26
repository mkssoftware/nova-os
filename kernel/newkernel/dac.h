/* NovaOS C-Kernel - Nova Discretionary Access Control (Port aus
   security32.inc, NPSPEC-SECURITY-DAC-0001). Besitzer-basierte
   Zugriffsrechte: Read/Write/Execute-Bits pro Eintrag. */
#ifndef NOVA_DAC_H
#define NOVA_DAC_H

#include <stdint.h>

#define DAC_ENTRY_COUNT 16u

#define DAC_PERM_READ  0x01u
#define DAC_PERM_WRITE 0x02u
#define DAC_PERM_EXEC  0x04u

void dac_initialize(void);
int dac_self_test(void); /* 0 = ok */

/* 0 = Erfolg, -1 wenn Tabelle voll. */
int dac_register(uint32_t obj_id, uint32_t owner, uint32_t owner_perms, uint32_t others_perms);

/* 0 = erlaubt, -1 = verboten/unbekannt. */
int dac_check(uint32_t obj_id, uint32_t caller_id, uint32_t requested_perms);

#endif
