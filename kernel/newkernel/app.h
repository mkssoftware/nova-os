/* NovaOS C-Kernel - Application Model (Port aus app32.inc, NPSPEC-APP-*). */
#ifndef NOVA_APP_H
#define NOVA_APP_H

#include <stdint.h>

#define APP_STATE_INACTIVE  0u
#define APP_STATE_STARTING  1u
#define APP_STATE_RUNNING   2u
#define APP_STATE_SUSPENDED 3u
#define APP_STATE_STOPPING  4u
#define APP_STATE_STOPPED   5u
#define APP_STATE_FAILED    6u

#define APP_TYPE_FOREGROUND 0u
#define APP_TYPE_BACKGROUND 1u
#define APP_TYPE_DOCUMENT   2u
#define APP_TYPE_OBJECT     3u
#define APP_TYPE_INTENT     4u
#define APP_TYPE_TASK       5u
#define APP_TYPE_SERVICE    6u

#define APP_SANDBOX_NONE     0u
#define APP_SANDBOX_STANDARD 1u
#define APP_SANDBOX_STRICT   2u
#define APP_SANDBOX_ISOLATED 3u

#define COMP_TYPE_STATIC  0u
#define COMP_TYPE_DYNAMIC 1u

#define APP_FLAG_PORTABLE   0x01u
#define APP_FLAG_SIGNED     0x02u
#define APP_FLAG_SYSTEM     0x04u
#define APP_FLAG_CAPABILITY 0x08u

#define APP_CAPACITY    8u
#define INTENT_CAPACITY 16u
#define SVC_CAPACITY    8u

void app_manager_initialize(void);
int app_manager_self_test(void); /* 0 = ok */

/* 0 = Erfolg (*out_record erhaelt den Record-Zeiger), -1 wenn voll. */
int app_register(uint32_t app_id, uint32_t app_type, uint32_t sandbox_level, uint32_t flags,
                  void **out_record);

/* record = Zeiger aus app_register()/app_find_by_id(). */
int app_start(void *record);   /* 0 = Erfolg, -1 wenn nicht STOPPED */
int app_stop(void *record);    /* 0 = Erfolg, -1 wenn nicht RUNNING/SUSPENDED/STARTING */
int app_suspend(void *record); /* 0 = Erfolg, -1 wenn nicht RUNNING */
int app_resume(void *record);  /* 0 = Erfolg, -1 wenn nicht SUSPENDED */

/* 0 = Erfolg (*out_record erhaelt den Record-Zeiger), -1 wenn nicht gefunden. */
int app_find_by_id(uint32_t app_id, void **out_record);

/* 0 = Erfolg, -1 wenn Warteschlange voll. */
int app_dispatch_intent(uint32_t intent_hash, uint32_t sender_id, uint32_t target_id);

/* 0 = Erfolg, -1 wenn voll. */
int svc_register(uint32_t svc_id, uint32_t app_id, uint32_t api_version, uint32_t flags);

#endif
