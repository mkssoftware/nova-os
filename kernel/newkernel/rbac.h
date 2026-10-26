/* NovaOS C-Kernel - Nova Role-Based Access Control (Port aus
   security32.inc, NPSPEC-SECURITY-RBAC-0001). Rollen mit
   Permission-Masken; Subjects werden Rollen zugewiesen. */
#ifndef NOVA_RBAC_H
#define NOVA_RBAC_H

#include <stdint.h>

#define RBAC_ROLE_COUNT   8u
#define RBAC_ASSIGN_COUNT 16u

void rbac_initialize(void);
int rbac_self_test(void); /* 0 = ok */

int rbac_define_role(uint32_t role_id, uint32_t permission_mask); /* 0 = Erfolg, -1 wenn voll */
int rbac_assign_role(uint32_t subject_id, uint32_t role_id);      /* 0 = Erfolg, -1 wenn voll */

/* 0 = erlaubt, -1 = verboten (keine Zuweisung oder Rolle fehlt die Rechte). */
int rbac_check(uint32_t subject_id, uint32_t requested_perm);

#endif
