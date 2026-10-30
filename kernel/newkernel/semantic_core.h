/* NovaOS C-Kernel - Nova Semantic Core Bootstrap (Port aus
   semantic_core32.inc, NPSPECs zu ObjectID/Namespace/CapabilityID/
   SemanticTypeID/Registry). Grundlage fuer stabile IDs, eine Kernel-
   Registry (welche ID-Art bedeutet was) und den ersten Namespace-Baum
   ("/", "/System", "/Benutzer", "/Apps", "/Volumes", "/Boot",
   "/Solutions"). Bewusst statisch, laeuft vor Userspace/Scheduler-
   Mutationen.

   WICHTIG: NICHT zu verwechseln mit den bereits vorhandenen Modulen
   gleicher/aehnlicher Begriffe - dies ist bereits das VIERTE
   eigenstaendige Objekt-/Capability-Konzept im Kernel (siehe
   project_c_kernel_migration_status.md, Module object.c/handle.c
   (memory32.inc), nobj.c/uobj.c (object32.inc/security32.inc §102),
   cap.c (security32.inc §103 Capability Framework), program.c (eigene
   Registry), semantic.c (semantic32.inc, Typsystem/Query - ANDERE Datei
   als semantic_core32.inc trotz aehnlichem Namen!)): semantic_core.c
   fuehrt eine EIGENE, fruehe Bootstrap-Registry mit eigenen Namespace-/
   Projection-/Capability-/Handle-Tabellen - keine dieser Tabellen teilt
   sich Speicher oder Funktionen mit den anderen vier Konzepten. IDs
   bleiben wie im alten ASM ein (kind-Tag, Zaehler)-Paar aus zwei
   uint32_t (id_high/id_low) statt eines einzelnen 64-Bit-Werts - spart
   jede 64-Bit-Arithmetik (keine Divisionen noetig, nur Gleichheits-
   vergleiche) und bleibt 1:1 am ASM-Original.

   Namespace-Pfadaufloesung (semantic_core_namespace_resolve_path/
   semantic_core_object_resolve_path) ist eine BEWUSST reduzierte
   "fruehe Bootstrap-Variante": nur "/" und genau EIN Segment direkt
   unter Root (z.B. "/System", "/Apps") - kein mehrstufiger Pfad-Walker
   (identisch zum alten ASM-Kommentar). */
#ifndef NOVA_SEMANTIC_CORE_H
#define NOVA_SEMANTIC_CORE_H

#include <stdint.h>

#define NOVA_ID_KIND_OBJECT        1u
#define NOVA_ID_KIND_VOLUME        2u
#define NOVA_ID_KIND_NAMESPACE     3u
#define NOVA_ID_KIND_CAPABILITY    4u
#define NOVA_ID_KIND_SEMANTIC_TYPE 5u

#define NOVA_REGISTRY_CAPACITY    16u
#define NOVA_OBJECT_CAPACITY      16u
#define NOVA_NAMESPACE_CAPACITY    8u
#define NOVA_PROJECTION_CAPACITY   8u
#define NOVA_CAPABILITY_CAPACITY  16u
#define NOVA_HANDLE_CAPACITY      16u
#define NOVA_VOLUME_CAPACITY       8u

/* Feste Namespace-Slots (identisch zur Bootstrap-Reihenfolge in
   semantic_core_initialize). */
#define NOVA_NAMESPACE_ROOT      0u
#define NOVA_NAMESPACE_SYSTEM    1u
#define NOVA_NAMESPACE_BENUTZER  2u
#define NOVA_NAMESPACE_APPS      3u
#define NOVA_NAMESPACE_VOLUMES   4u
#define NOVA_NAMESPACE_BOOT      5u
#define NOVA_NAMESPACE_SOLUTIONS 6u

#define NOVA_NS_FLAG_STABLE       1u
#define NOVA_NS_FLAG_SYSTEM       2u
#define NOVA_NS_FLAG_USER_VISIBLE 4u

#define NOVA_OBJECT_FLAG_STABLE       1u
#define NOVA_OBJECT_FLAG_SYSTEM       2u
#define NOVA_OBJECT_FLAG_USER_VISIBLE 4u
#define NOVA_OBJECT_FLAG_NAMESPACE    8u

#define NOVA_PROJ_FLAG_PRIMARY 1u
#define NOVA_PROJ_FLAG_STABLE  2u

#define NOVA_CAP_FLAG_ACTIVE 1u
#define NOVA_CAP_FLAG_STABLE 2u

#define NOVA_HANDLE_FLAG_ACTIVE   1u
#define NOVA_HANDLE_RIGHT_READ    1u
#define NOVA_HANDLE_RIGHT_WRITE   2u
#define NOVA_HANDLE_RIGHT_EXECUTE 4u
#define NOVA_HANDLE_RIGHT_MANAGE  8u

#define NOVA_VOLUME_FLAG_MOUNTED  1u
#define NOVA_VOLUME_FLAG_READONLY 2u
#define NOVA_VOLUME_FLAG_TRUSTED  4u
#define NOVA_VOLUME_FLAG_BOOT     8u

#define NOVA_VOLUME_FS_BOOT 1u

#define NOVA_ID_OBJECT_HIGH     0x4F424A49u /* "OBJI" */
#define NOVA_ID_VOLUME_HIGH     0x564F4C49u /* "VOLI" */
#define NOVA_ID_NAMESPACE_HIGH  0x4E534944u /* "NSID" */
#define NOVA_ID_CAPABILITY_HIGH 0x43415049u /* "CAPI" */
#define NOVA_ID_SEMANTIC_HIGH   0x53454D49u /* "SEMI" */

typedef struct {
    uint32_t kind;
    uint32_t id_high, id_low;
    const char *name;
    uint32_t flags;
} semantic_core_registry_record_t;

typedef struct {
    uint32_t id_high, id_low;
    uint32_t ns_slot;
    const char *name;
    uint32_t flags;
    uint32_t sem_high, sem_low;
} semantic_core_object_record_t;

typedef struct {
    uint32_t id_high, id_low;
    uint32_t parent; /* 0xFFFFFFFFu bei Root */
    const char *name;
    uint32_t kind;
    uint32_t flags;
    uint32_t object_high, object_low;
} semantic_core_namespace_record_t;

typedef struct {
    uint32_t object_high, object_low;
    uint32_t ns_slot;
    const char *name;
    uint32_t flags;
    uint32_t target_high, target_low;
} semantic_core_projection_record_t;

typedef struct {
    uint32_t id_high, id_low;
    uint32_t object_high, object_low;
    uint32_t ns_slot;
    uint32_t rights;
    uint32_t flags;
} semantic_core_capability_record_t;

typedef struct {
    uint32_t id;
    uint32_t object_high, object_low;
    uint32_t ns_slot;
    uint32_t rights;
    uint32_t flags;
    uint32_t generation;
} semantic_core_handle_record_t;

typedef struct {
    uint32_t id_high, id_low;
    uint32_t ns_slot;
    uint32_t object_high, object_low;
    uint32_t flags;
    uint32_t fs_type;
} semantic_core_volume_record_t;

/* Setzt alle Tabellen zurueck und legt die Bootstrap-Registry, den
   Root-Namespace-Baum, das fruehe Boot-Volume und die festen frueh-
   Capabilities an (siehe semantic_core.c). Mehrfacher Aufruf ist sicher
   (voller Reset). */
void semantic_core_initialize(void);

int semantic_core_self_test(void); /* 0 = ok */

int semantic_core_registry_lookup(uint32_t id_high, uint32_t id_low, semantic_core_registry_record_t **out);
int semantic_core_registry_add(uint32_t id_high, uint32_t id_low, uint32_t kind, const char *name);

int semantic_core_object_add(uint32_t id_high, uint32_t id_low, uint32_t ns_slot, const char *name, uint32_t flags);
int semantic_core_object_lookup(uint32_t id_high, uint32_t id_low, semantic_core_object_record_t **out);
uint32_t semantic_core_object_count_by_flags(uint32_t required_flags);
int semantic_core_object_at_by_flags(uint32_t required_flags, uint32_t ordinal, semantic_core_object_record_t **out);
/* Prueft, dass Object Registry und Projection Map denselben sichtbaren
   Namespace-/Semantic-Zielbezug fuer das Objekt tragen. */
int semantic_core_object_validate_projection(uint32_t id_high, uint32_t id_low, semantic_core_projection_record_t **out);
int semantic_core_object_resolve_path(const char *path, semantic_core_object_record_t **out);

int semantic_core_volume_add(uint32_t id_high, uint32_t id_low, uint32_t ns_slot,
                              uint32_t object_high, uint32_t object_low, uint32_t flags, uint32_t fs_type);
int semantic_core_volume_lookup(uint32_t id_high, uint32_t id_low, semantic_core_volume_record_t **out);
int semantic_core_volume_lookup_by_namespace(uint32_t ns_slot, semantic_core_volume_record_t **out);

int semantic_core_namespace_add(uint32_t id_high, uint32_t id_low, uint32_t parent, const char *name,
                                 uint32_t object_high, uint32_t object_low, uint32_t flags);
int semantic_core_namespace_lookup(uint32_t id_high, uint32_t id_low, semantic_core_namespace_record_t **out);
int semantic_core_namespace_resolve_path(const char *path, semantic_core_namespace_record_t **out);
int semantic_core_namespace_count_children(uint32_t parent_slot, uint32_t required_flags, uint32_t *out_count);
int semantic_core_namespace_child_at(uint32_t parent_slot, uint32_t required_flags, uint32_t ordinal,
                                      semantic_core_namespace_record_t **out);
int semantic_core_namespace_record(uint32_t slot, semantic_core_namespace_record_t **out);

int semantic_core_projection_add(uint32_t object_high, uint32_t object_low, uint32_t ns_slot, const char *name,
                                  uint32_t target_high, uint32_t target_low, uint32_t flags);
int semantic_core_projection_lookup_by_object(uint32_t object_high, uint32_t object_low, semantic_core_projection_record_t **out);
int semantic_core_projection_count_by_namespace(uint32_t ns_slot, uint32_t *out_count);
int semantic_core_projection_at_namespace(uint32_t ns_slot, uint32_t required_flags, uint32_t ordinal,
                                           semantic_core_projection_record_t **out);

int semantic_core_capability_add(uint32_t id_high, uint32_t id_low, uint32_t object_high, uint32_t object_low,
                                  uint32_t ns_slot, uint32_t rights, uint32_t flags);
int semantic_core_capability_lookup(uint32_t id_high, uint32_t id_low, semantic_core_capability_record_t **out);
int semantic_core_capability_revoke(uint32_t id_high, uint32_t id_low);
int semantic_core_capability_check_object_rights(uint32_t object_high, uint32_t object_low, uint32_t rights,
                                                  semantic_core_capability_record_t **out);

/* Legt (oder - falls ein vorher geschlossenes Handle wiederverwendbar
   ist - recycelt) ein Handle auf ein Objekt an, NACHDEM Projection UND
   Capability-Rechte geprueft wurden. *out_handle_id ist bei Erfolg
   gesetzt. */
int semantic_core_handle_open_by_object(uint32_t object_high, uint32_t object_low, uint32_t rights,
                                         uint32_t *out_handle_id, semantic_core_handle_record_t **out);
int semantic_core_handle_lookup(uint32_t handle_id, semantic_core_handle_record_t **out);
int semantic_core_handle_validate_rights(uint32_t handle_id, uint32_t rights, semantic_core_handle_record_t **out);
int semantic_core_handle_close(uint32_t handle_id);
int semantic_core_handle_open_by_path(const char *path, uint32_t rights,
                                       uint32_t *out_handle_id, semantic_core_handle_record_t **out);

#endif
