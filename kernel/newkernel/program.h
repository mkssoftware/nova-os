/* NovaOS C-Kernel - Program-Manager, Registry und Capability-System (Port
   aus program32.inc, NPSPEC-PROGRAM- und NPSPEC-REGISTRY- und
   NPSPEC-CAPABILITY-Spezifikationen). */
#ifndef NOVA_PROGRAM_H
#define NOVA_PROGRAM_H

#include <stdint.h>

#define PROG_SCOPE_SYSTEM    0u
#define PROG_SCOPE_USER      1u
#define PROG_SCOPE_WORKSPACE 2u

#define PROG_STATE_ABSENT    0u
#define PROG_STATE_INSTALLED 1u
#define PROG_STATE_UPDATING  2u
#define PROG_STATE_ROLLBACK  3u
#define PROG_STATE_PENDING   4u
#define PROG_STATE_BROKEN    5u

#define PROG_FLAG_SYSTEM   0x01u
#define PROG_FLAG_LEGACY   0x02u
#define PROG_FLAG_OVERLAY  0x04u
#define PROG_FLAG_TRUSTED  0x08u
#define PROG_FLAG_HAS_CAPS 0x10u

#define PROG_CAPACITY 8u

#define REG_TYPE_UNKNOWN    0u
#define REG_TYPE_ALGORITHM  1u
#define REG_TYPE_CAPABILITY 2u
#define REG_TYPE_CODEC      3u
#define REG_TYPE_DEVICE     4u
#define REG_TYPE_SERVICE    5u
#define REG_TYPE_CUSTOM     6u

#define REG_CAPACITY 32u

#define CAP_CAT_PLATFORM 0u
#define CAP_CAT_INPUT    1u
#define CAP_CAT_OUTPUT   2u
#define CAP_CAT_RUNTIME  3u
#define CAP_CAT_SANDBOX  4u
#define CAP_CAT_SECURITY 5u
#define CAP_CAT_COUNT    6u

#define CAP_FLAG_ACTIVE       0x01u
#define CAP_FLAG_REVOKED      0x02u
#define CAP_FLAG_SIGNED       0x04u
#define CAP_FLAG_TRANSFERABLE 0x08u
#define CAP_FLAG_SANDBOX      0x10u
#define CAP_FLAG_COMPAT       0x20u

#define CAP_REGISTRY_CAPACITY 16u

/* Registriert Platform- und Sandbox-Capabilities sowie den CRC32-Algorithmus
   in der Registry. */
void program_manager_initialize(void);
int program_manager_self_test(void); /* 0 = ok */

/* 0 = Erfolg, -1 wenn Tabelle voll. */
int prog_install(uint32_t prog_id, uint32_t flags, uint32_t scope, uint32_t version);
int prog_uninstall(uint32_t prog_id); /* 0 = Erfolg, -1 wenn prog_id unbekannt */

/* 0 = Erfolg, -1 wenn prog_id unbekannt. Sichert die alte Version fuer Rollback. */
int prog_update(uint32_t prog_id, uint32_t new_version);
int prog_rollback(uint32_t prog_id); /* 0 = Erfolg, -1 wenn prog_id unbekannt */

/* 0 = Erfolg, -1 wenn Tabelle voll. key_lo ist ein kompakter 4-Byte-Schluessel. */
int reg_insert(uint32_t type, uint32_t key_lo, uint32_t value_lo);
/* 0 = Erfolg (out_value_lo gesetzt), -1 wenn nicht gefunden. */
int reg_query(uint32_t type, uint32_t key_lo, uint32_t *out_value_lo);

/* 0 = Erfolg (out_cap_id gesetzt), -1 wenn Tabelle voll. */
int cap_register(uint32_t category, uint32_t flags, uint32_t program_id, uint32_t *out_cap_id);
int cap_activate(uint32_t cap_id); /* 0 = Erfolg, -1 wenn cap_id unbekannt */
int pkg_cap_revoke(uint32_t cap_id); /* 0 = Erfolg, -1 wenn cap_id unbekannt */

/* 0 = Erfolg (out_category/out_flags gesetzt), -1 wenn cap_id unbekannt. */
int cap_query(uint32_t cap_id, uint32_t *out_category, uint32_t *out_flags);

/* 0 = erlaubt, -1 = verweigert (widerrufen, nicht aktiv, oder fehlende Permission). */
int cap_check_permission(uint32_t cap_id, uint32_t required_perm);

#endif
