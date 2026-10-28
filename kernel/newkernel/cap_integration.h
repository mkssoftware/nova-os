/* NovaOS C-Kernel - CAP-Integration (Port aus cap_integration32.inc,
   §103<->§102, §103<->IPC, §103<->VFS). Verbindet das Capability
   Framework (cap.h) mit der Unified Object API (uobj.h), IPC (ipc.h) und
   dem Bootstrap-VFS-Root-Lookup (ipc.h's vfs_lookup_root).

   WICHTIG (Abweichung vom alten ASM): das Original haelt eine PARALLELE
   Tabelle `uobj_hdl_cap_tab`, indiziert per Zeiger-Differenz-Trick
   ((hdl_ptr - uobj_handles) >> 4) auf uobj.c's internes Handle-Array.
   Dieser Trick setzt voraus, dass der Aufrufer die exakte Speicherlayout-
   Struktur von uobj.c kennt - genau die Art Kopplung, die dieser C-Port
   bewusst vermeidet (uobj.c bleibt ein Modul mit eigenstaendiger
   Kapselung). Stattdessen wird hier eine simple, per handle_id linear
   durchsuchte Tabelle {handle_id, cap_id} verwendet - funktional
   aequivalent, ohne uobj.c's interne Speicherorganisation offenzulegen. */
#ifndef NOVA_CAP_INTEGRATION_H
#define NOVA_CAP_INTEGRATION_H

#include <stdint.h>

void cap_integration_initialize(void);
int cap_integration_self_test(void); /* 0 = ok */

/* Erzeugt ein neues UOBJ-Handle fuer ein bereits bestehendes Objekt UND
   einen begleitenden CAP-Deskriptor (delegable_rights = rights &
   NP_CAP_QUERY, monoton reduziert). 0 = Erfolg (*out_handle_id und
   *out_cap_id gesetzt), -1 bei unbekanntem obj_id, voller Tabelle oder
   cap_create-Fehler. */
int cap_uobj_create_handle(uint32_t obj_id, uint32_t rights, uint32_t domain,
                            uint32_t *out_handle_id, uint32_t *out_cap_id);

/* Loest ein Handle via dessen begleitendem CAP-Deskriptor auf (bridge
   §103<->§102): prueft erst cap_check(required_rights), dann
   uobj_from_handle. 0 = Erfolg (*out_obj_ptr gesetzt), -1 wenn kein
   begleitender CAP-Deskriptor existiert, die Pruefung fehlschlaegt, oder
   das Handle ungueltig/veraltet ist. */
int cap_uobj_access(uint32_t handle_id, uint32_t required_rights, void **out_obj_ptr);

/* IPC-Send/Receive mit vorgeschalteter Capability-Pruefung
   (NP_CAP_SIGNAL bzw. NP_CAP_SUBSCRIBE). 0 = Erfolg, -1 bei verweigerter
   Capability oder voller/leerer Queue. */
int ipc_send_secure(uint32_t cap_id, const void *msg_16_bytes);
int ipc_receive_secure(uint32_t cap_id, void *out_buf_16_bytes);

/* VFS-Root-Lookup mit vorgeschalteter NP_CAP_QUERY-Pruefung. 0 = Erfolg
   (*out_handle gesetzt), -1 bei verweigerter Capability oder unbekanntem
   Pfad. */
int vfs_lookup_secure(uint32_t cap_id, const char *path, uint32_t path_len,
                       uint32_t *out_handle);

#endif
