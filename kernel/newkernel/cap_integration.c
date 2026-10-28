/* NovaOS C-Kernel - CAP-Integration (siehe cap_integration.h fuer Scope). */
#include "cap_integration.h"
#include "cap.h"
#include "uobj.h"
#include "ipc.h"
#include "observability.h"
#include "kog.h"

/* Virtuelle Objekt-IDs fuer IPC-Endpoint-/VFS-Root-Caps (keine echten
   UOBJ-Objekte dahinter - cap.c validiert obj_id nicht gegen uobj.c,
   siehe cap_integration32.inc Kommentar "virtuelles Objekt-ID"). */
#define CAP_INT_IPC_OBJ_ID 200u
#define CAP_INT_VFS_OBJ_ID 201u

#define CAP_INT_HANDLE_CAP_CAPACITY 32u

typedef struct {
    uint32_t handle_id; /* 0 = frei */
    uint32_t cap_id;
} cap_int_handle_cap_t;

static cap_int_handle_cap_t cap_int_handle_cap_tab[CAP_INT_HANDLE_CAP_CAPACITY];
static uint32_t ipc_cap_send_id = 0;
static uint32_t ipc_cap_recv_id = 0;
static uint32_t vfs_cap_query_id = 0;

static int cap_int_store(uint32_t handle_id, uint32_t cap_id) {
    for (uint32_t i = 0; i < CAP_INT_HANDLE_CAP_CAPACITY; ++i) {
        if (cap_int_handle_cap_tab[i].handle_id == 0u) {
            cap_int_handle_cap_tab[i].handle_id = handle_id;
            cap_int_handle_cap_tab[i].cap_id = cap_id;
            return 0;
        }
    }
    return -1;
}

static int cap_int_lookup(uint32_t handle_id, uint32_t *out_cap_id) {
    for (uint32_t i = 0; i < CAP_INT_HANDLE_CAP_CAPACITY; ++i) {
        if (cap_int_handle_cap_tab[i].handle_id == handle_id) {
            *out_cap_id = cap_int_handle_cap_tab[i].cap_id;
            return 0;
        }
    }
    return -1;
}

int cap_uobj_create_handle(uint32_t obj_id, uint32_t rights, uint32_t domain,
                            uint32_t *out_handle_id, uint32_t *out_cap_id) {
    uint32_t handle_id;
    if (uobj_create_handle_for_object(obj_id, rights, &handle_id) != 0) return -1;

    uint32_t cap_id;
    if (cap_create(obj_id, rights, rights & NP_CAP_QUERY, domain, &cap_id) != 0) return -1;

    if (cap_int_store(handle_id, cap_id) != 0) return -1;

    *out_handle_id = handle_id;
    *out_cap_id = cap_id;
    return 0;
}

int cap_uobj_access(uint32_t handle_id, uint32_t required_rights, void **out_obj_ptr) {
    uint32_t cap_id;
    if (cap_int_lookup(handle_id, &cap_id) != 0) return -1;
    if (cap_check(cap_id, required_rights) != 0) return -1;

    /* required_capabilities=0: die CAP-Pruefung oben deckt den Zugriff
       bereits ab, uobj_from_handle soll nicht doppelt gegen die (davon
       unabhaengigen) UOBJ_CAP_*-Inline-Rechte pruefen. */
    return uobj_from_handle(handle_id, 0u, out_obj_ptr);
}

int ipc_send_secure(uint32_t cap_id, const void *msg_16_bytes) {
    if (cap_check(cap_id, NP_CAP_SIGNAL) != 0) return -1;
    return ipc_send((const ipc_message_t *)msg_16_bytes) != 0 ? 0 : -1;
}

int ipc_receive_secure(uint32_t cap_id, void *out_buf_16_bytes) {
    if (cap_check(cap_id, NP_CAP_SUBSCRIBE) != 0) return -1;
    return ipc_receive((ipc_message_t *)out_buf_16_bytes) != 0 ? 0 : -1;
}

int vfs_lookup_secure(uint32_t cap_id, const char *path, uint32_t path_len,
                       uint32_t *out_handle) {
    if (cap_check(cap_id, NP_CAP_QUERY) != 0) return -1;
    return vfs_lookup_root(path, path_len, out_handle);
}

void cap_integration_initialize(void) {
    for (uint32_t i = 0; i < CAP_INT_HANDLE_CAP_CAPACITY; ++i) {
        cap_int_handle_cap_tab[i] = (cap_int_handle_cap_t){0};
    }

    cap_create(CAP_INT_IPC_OBJ_ID, NP_CAP_SIGNAL, 0u, CAP_DOMAIN_KERNEL, &ipc_cap_send_id);
    cap_create(CAP_INT_IPC_OBJ_ID, NP_CAP_SUBSCRIBE, 0u, CAP_DOMAIN_KERNEL, &ipc_cap_recv_id);
    cap_create(CAP_INT_VFS_OBJ_ID, NP_CAP_QUERY, NP_CAP_QUERY, CAP_DOMAIN_KERNEL, &vfs_cap_query_id);

    diag_log(DIAG_SRC_CAP, DIAG_LEVEL_INFO, 0u, 0u);
}

int cap_integration_self_test(void) {
    uint32_t handle0, obj1;
    if (uobj_create(NP_OBJTYPE_GENERIC, UOBJ_CAP_QUERY | UOBJ_CAP_MODIFY, &handle0, &obj1) != 0) {
        return -1;
    }

    uint32_t handle1, cap1;
    if (cap_uobj_create_handle(obj1, NP_CAP_QUERY | NP_CAP_MODIFY, CAP_DOMAIN_KERNEL,
                                &handle1, &cap1) != 0) {
        return -1;
    }

    void *obj_ptr;
    if (cap_uobj_access(handle1, NP_CAP_QUERY, &obj_ptr) != 0) return -1;
    if (cap_uobj_access(handle1, NP_CAP_ADMIN, &obj_ptr) == 0) return -1; /* kein ADMIN-Recht */

    uint32_t test_msg[4] = {0xCAF10001u, 0xCAF10002u, 0xCAF10003u, 0xCAF10004u};
    if (ipc_send_secure(ipc_cap_send_id, test_msg) != 0) return -1;

    uint32_t recv_buf[4] = {0};
    if (ipc_receive_secure(ipc_cap_recv_id, recv_buf) != 0) return -1;
    if (recv_buf[0] != test_msg[0]) return -1;

    uint32_t vfs_handle;
    if (vfs_lookup_secure(vfs_cap_query_id, "/", 1u, &vfs_handle) != 0) return -1;

    if (vfs_lookup_secure(0xDEADu, "/", 1u, &vfs_handle) == 0) return -1; /* ungueltige Cap */

    return 0;
}
