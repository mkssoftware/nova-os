/* NovaOS C-Kernel - §102 Unified Object API (Port aus security32.inc,
   NPSPEC-KERNEL-0102). Einheitliche Zugangsschicht fuer verwaltete
   Kernelobjekte. Bootstrap: Phase 1 (Typregistrierung, Objekterzeugung,
   Handle-Auflösung, Referenzzaehlung, Capability-Pruefung). NICHT zu
   verwechseln mit object.c (Kernel Object Manager aus memory32.inc) oder
   nobj.c (Nova Object-Subsystem aus object32.inc) - drei unterschiedliche,
   bereits im alten ASM nebeneinander existierende Objektsysteme. */
#ifndef NOVA_UOBJ_H
#define NOVA_UOBJ_H

#include <stdint.h>

#define UOBJ_STATE_CREATING   0u
#define UOBJ_STATE_ACTIVE     1u
#define UOBJ_STATE_QUIESCING  2u
#define UOBJ_STATE_CLOSING    3u
#define UOBJ_STATE_ZOMBIE     4u
#define UOBJ_STATE_DESTROYING 5u
#define UOBJ_STATE_DESTROYED  6u

#define UOBJ_CAP_QUERY     0x001u
#define UOBJ_CAP_MODIFY    0x002u
#define UOBJ_CAP_SIGNAL    0x004u
#define UOBJ_CAP_WAIT      0x008u
#define UOBJ_CAP_DUPLICATE 0x010u
#define UOBJ_CAP_TRANSFER  0x020u
#define UOBJ_CAP_SUBSCRIBE 0x040u
#define UOBJ_CAP_DELETE    0x080u
#define UOBJ_CAP_ADMIN     0x100u

#define UOBJ_MAX_TYPES   16u
#define UOBJ_MAX_OBJECTS 32u
#define UOBJ_MAX_HANDLES 32u

typedef int (*uobj_init_fn_t)(void *obj_ptr); /* 0 = ok */
typedef void (*uobj_destroy_fn_t)(void *obj_ptr);

void uobj_initialize(void); /* registriert die 15 eingebauten Objekttypen */
int uobj_self_test(void); /* 0 = ok */

/* 0 = Erfolg, -1 wenn doppelt oder Pool voll. init_fn/destroy_fn: 0 = keine. */
int uobj_register_type(uint32_t type_id, uint32_t abi_version, uint32_t flags,
                        uobj_init_fn_t init_fn, uobj_destroy_fn_t destroy_fn);

/* 0 = Erfolg (Handle-ID und Objekt-ID gesetzt), -1 bei unbekanntem Typ,
   vollem Pool oder fehlgeschlagenem init_fn. */
int uobj_create(uint32_t type_id, uint32_t capabilities, uint32_t *out_handle_id, uint32_t *out_obj_id);

/* 0 = Erfolg (*out_obj_ptr gesetzt), -1 bei fehlenden Rechten/ungueltigem
   Handle/veralteter Generation/sterbendem Objekt. required_capabilities=0
   ueberspringt die Capability-Pruefung (fuer interne Bridges). */
int uobj_from_handle(uint32_t handle_id, uint32_t required_capabilities, void **out_obj_ptr);

int uobj_retain(uint32_t obj_id);  /* 0 = Erfolg, -1 wenn nicht gefunden */
int uobj_release(uint32_t obj_id); /* 0 = Erfolg, -1 wenn nicht gefunden; zerstoert bei Refcount 0 */
int uobj_close_handle(uint32_t handle_id); /* 0 = Erfolg, -1 wenn nicht gefunden */

/* Nur query_class=0 (Basisidentitaet) implementiert: schreibt 16 Bytes
   (obj_id, type_id, generation, state) in *out_buf. Gibt die Anzahl
   geschriebener Bytes zurueck, oder -1 bei Fehler. */
int uobj_query(uint32_t handle_id, uint32_t query_class, void *out_buf);

#endif
