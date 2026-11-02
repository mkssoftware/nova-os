/* NovaOS C-Kernel - §103 Capability Framework 1.0 (Port aus
   security32.inc). DUENNER WRAPPER UEBER cap_core.c (023-CAPABILITY-
   Konsolidierung, siehe cap_core.h fuer die vollstaendige Scope-
   Begruendung) - die eigentliche Rechte-Tabelle/-Pruefung lebt jetzt in
   cap_core.c; dieses Modul behaelt nur noch seine eigene Policy-Schicht
   (evbus-Events, Bootstrap-Capabilities fuer Kernel-Root/Boot-Kontext)
   und die §103-spezifische Fehlercode-/Konstanten-Fassade (cap.h bleibt
   fuer cap_integration.c UNVERAENDERT - diese Aenderung ist fuer dessen
   Aufrufer vollstaendig transparent). */
#include "cap.h"
#include "cap_core.h"
#include "evbus.h"

#define NP_EVENT_CAP_CREATED 9u
#define NP_EVENT_CAP_REVOKED 10u
#define NP_EVENT_CAP_DENIED  11u

static uint32_t cap_boot_kernel_id = 0;
static uint32_t cap_boot_id = 0;

int cap_create(uint32_t obj_id, uint32_t rights, uint32_t delegable_rights, uint32_t domain,
               uint32_t *out_cap_id) {
    int rc = cap_core_create(obj_id, rights, delegable_rights, domain, out_cap_id);
    if (rc != 0) return rc;
    evbus_publish(NP_EVENT_CAP_CREATED, *out_cap_id, 0, 0, 0);
    return 0;
}

int cap_check(uint32_t cap_id, uint32_t required_rights) {
    int rc = cap_core_check(cap_id, required_rights);
    if (rc == CAP_CORE_ERR_RIGHTS) {
        evbus_publish(NP_EVENT_CAP_DENIED, 0, 0, 0, 0);
    }
    return rc;
}

int cap_derive(uint32_t src_cap_id, uint32_t new_rights, uint32_t new_delegable, uint32_t *out_cap_id) {
    int rc = cap_core_derive(src_cap_id, new_rights, new_delegable, out_cap_id);
    if (rc != 0) return rc;
    evbus_publish(NP_EVENT_CAP_CREATED, *out_cap_id, 0, 0, 0);
    return 0;
}

int cap_revoke(uint32_t cap_id) {
    int rc = cap_core_revoke(cap_id);
    if (rc != 0) return rc;
    evbus_publish(NP_EVENT_CAP_REVOKED, cap_id, 0, 0, 0);
    return 0;
}

int cap_initialize(void) {
    cap_core_initialize();
    cap_boot_kernel_id = 0;
    cap_boot_id = 0;

    if (evbus_register_schema(NP_EVENT_CAP_CREATED, NP_EVENT_CLASS_LIFECYCLE,
                               0, NP_EVENT_SYNCHRONOUS | NP_EVENT_KERNEL_ONLY) != 0) return -1;
    if (evbus_register_schema(NP_EVENT_CAP_REVOKED, NP_EVENT_CLASS_STATE,
                               0, NP_EVENT_SYNCHRONOUS | NP_EVENT_KERNEL_ONLY) != 0) return -1;
    if (evbus_register_schema(NP_EVENT_CAP_DENIED, NP_EVENT_CLASS_SECURITY,
                               0, NP_EVENT_SYNCHRONOUS | NP_EVENT_KERNEL_ONLY) != 0) return -1;

    uint32_t cap_id;
    if (cap_create(1, NP_CAP_ADMIN | NP_CAP_QUERY | NP_CAP_MODIFY, NP_CAP_QUERY,
                    CAP_DOMAIN_KERNEL, &cap_id) != 0) return -1;
    cap_boot_kernel_id = cap_id;

    if (cap_create(2, NP_CAP_QUERY | NP_CAP_SUBSCRIBE, 0, CAP_DOMAIN_BOOT, &cap_id) != 0) return -1;
    cap_boot_id = cap_id;

    return 0;
}

int cap_self_test(void) {
    uint32_t cap1;
    if (cap_create(42, NP_CAP_QUERY | NP_CAP_MODIFY, NP_CAP_QUERY, CAP_DOMAIN_KERNEL, &cap1) != 0) return -1;

    if (cap_check(cap1, NP_CAP_QUERY) != 0) return -1;

    if (cap_check(cap1, NP_CAP_ADMIN) != NP_ERR_CAP_RIGHTS) return -1;

    uint32_t cap2;
    if (cap_derive(cap1, NP_CAP_QUERY, NP_CAP_QUERY, &cap2) != 0) return -1;

    uint32_t dummy;
    if (cap_derive(cap1, NP_CAP_QUERY | NP_CAP_MODIFY | NP_CAP_ADMIN, NP_CAP_QUERY, &dummy) == 0) return -1;

    if (cap_revoke(cap1) != 0) return -1;

    if (cap_check(cap1, NP_CAP_QUERY) != NP_ERR_CAP_REVOKED) return -1;

    if (cap_check(0xDEADu, NP_CAP_QUERY) != NP_ERR_CAP_INVALID) return -1;

    uint32_t cap3;
    if (cap_derive(cap2, NP_CAP_QUERY, 0, &cap3) != 0) return -1;

    if (cap_check(cap3, NP_CAP_MODIFY) != NP_ERR_CAP_RIGHTS) return -1;

    return 0;
}
