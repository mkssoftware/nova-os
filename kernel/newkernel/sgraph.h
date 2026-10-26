/* NovaOS C-Kernel - Nova System State Graph (Port aus adaptive32.inc,
   NPSPEC-OBSERVABILITY-STATEGRAPH-0001). Registriert Systemobjekte mit
   Zustand + Vorgaenger-Zustand + Parent (depended-on object). Ermoeglicht
   Fragen: welcher Zustand, von wem abhaengig, wie entstanden. */
#ifndef NOVA_SGRAPH_H
#define NOVA_SGRAPH_H

#include <stdint.h>

#define SGRAPH_CAPACITY 16u

void sgraph_initialize(void);
int sgraph_self_test(void); /* 0 = ok */

/* 0 = Erfolg, -1 wenn Tabelle voll. */
int sgraph_register(uint32_t obj_id, uint32_t initial_state, uint32_t parent_obj_id);

/* 0 = Erfolg (Vorgaenger-Zustand gespeichert), -1 wenn obj_id unbekannt. */
int sgraph_transition(uint32_t obj_id, uint32_t new_state);

/* 0 = Erfolg (out_state, out_prev_state gesetzt), -1 wenn obj_id unbekannt. */
int sgraph_query(uint32_t obj_id, uint32_t *out_state, uint32_t *out_prev_state);

#endif
