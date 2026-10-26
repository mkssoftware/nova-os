/* NovaOS C-Kernel - Nova State History Ring (Port aus adaptive32.inc,
   NPSPEC-STATE-HISTORY-0001). Zeichnet einen Ring der letzten
   SHIST_CAPACITY Zustandsuebergaenge fuer Audit/Debug auf. */
#ifndef NOVA_SHIST_H
#define NOVA_SHIST_H

#include <stdint.h>

#define SHIST_CAPACITY 8u

void shist_initialize(void);
int shist_self_test(void); /* 0 = ok */

void shist_record(uint32_t obj_id, uint32_t old_state, uint32_t new_state);
uint32_t shist_count_transitions(void);

#endif
