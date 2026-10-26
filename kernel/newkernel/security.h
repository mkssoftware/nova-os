/* NovaOS C-Kernel - Kernel Security / Capability Manager (Port aus
   security32.inc, ADR-2013 / NPSPEC-KERNEL-0020). PID-basierte
   Capability-Masken, getrennt von program.c's Capability-Registry,
   cap.c's §103-Framework und verify.c's cap_query-Stub (vier
   unterschiedliche Capability-Konzepte, die schon im alten ASM
   nebeneinander existierten). */
#ifndef NOVA_SECURITY_H
#define NOVA_SECURITY_H

#include <stdint.h>

#define SECURITY_CAP_MEMORY             0x00000001u
#define SECURITY_CAP_IO                 0x00000002u
#define SECURITY_CAP_SERVICE            0x00000004u
#define SECURITY_CAP_ADMIN              0x00000008u
#define SECURITY_CAP_IPC                0x00000010u
#define SECURITY_CAP_POWER_QUERY        0x00000020u
#define SECURITY_CAP_POWER_WAKE         0x00000040u
#define SECURITY_CAP_POWER_SHUTDOWN     0x00000080u
#define SECURITY_CAP_POWER_PROFILE      0x00000100u
#define SECURITY_CAP_NET_QUERY          0x00000200u
#define SECURITY_CAP_NET_RAW            0x00000400u
#define SECURITY_CAP_NET_CONNECT        0x00000800u
#define SECURITY_CAP_NET_LISTEN         0x00001000u
#define SECURITY_CAP_LOG_READ           0x00002000u
#define SECURITY_CAP_DISPLAY_SYSTEM_UI  0x00004000u
#define SECURITY_CAP_BOOT_HEALTH_REPORT 0x00008000u
#define SECURITY_CAP_BOOT_HEALTH_COMMIT 0x00010000u
#define SECURITY_CAP_FS_READ            0x00020000u
#define SECURITY_CAP_FS_WRITE           0x00040000u
#define SECURITY_CAP_FS_SYSTEM_WRITE    0x00080000u
#define SECURITY_KERNEL_CAPS            0x000FFFFFu

#define SECURITY_CAPACITY 4u /* = PROCESS_CAPACITY */

/* Gewaehrt PID 1 (Kernel) alle SECURITY_KERNEL_CAPS. */
void security_initialize(void);
int security_self_test(void); /* 0 = ok */

/* 0 = Erfolg, -1 wenn caps==0, pid unbekannt (process_lookup) oder Tabelle voll. */
int security_grant(uint32_t pid, uint32_t caps);
/* 0 = Erfolg, -1 wenn pid nicht in der Tabelle. */
int security_revoke(uint32_t pid, uint32_t caps);
/* 1 = erlaubt, 0 = verweigert (pid unbekannt oder Caps fehlen). */
int security_check(uint32_t pid, uint32_t required_caps);

#endif
