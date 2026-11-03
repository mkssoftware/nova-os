/* NovaOS C-Kernel - Application Model (Port aus app32.inc). */
#include "app.h"
#include "global.h"

typedef struct {
    uint32_t id;
    uint32_t type;
    uint32_t state;
    uint32_t sandbox;
    uint32_t flags;
    uint32_t process;
    uint32_t capabilities;
    uint32_t parent;
    uint32_t comp_type;
    uint32_t intent_hash;
    uint32_t doc_type;
    uint32_t task_id;
    uint32_t locale;
} app_record_t;

typedef struct {
    uint32_t hash;
    uint32_t sender;
    uint32_t target;
    uint32_t data_ptr;
    uint32_t data_len;
    uint32_t flags;
} app_intent_t;

typedef struct {
    uint32_t id;
    uint32_t app;
    uint32_t api_version;
    uint32_t flags;
    uint32_t endpoint;
} svc_record_t;

static app_record_t app_records[APP_CAPACITY];
static app_intent_t app_intents[INTENT_CAPACITY];
static svc_record_t svc_records[SVC_CAPACITY];
static uint32_t app_count = 0;
static uint32_t intent_count = 0;
static uint32_t svc_count = 0;
static int app_mgr_ready = 0;

void app_manager_initialize(void) {
    for (uint32_t i = 0; i < APP_CAPACITY; ++i) {
        app_records[i] = (app_record_t){0};
    }
    for (uint32_t i = 0; i < INTENT_CAPACITY; ++i) {
        app_intents[i] = (app_intent_t){0};
    }
    for (uint32_t i = 0; i < SVC_CAPACITY; ++i) {
        svc_records[i] = (svc_record_t){0};
    }
    app_count = 0;
    intent_count = 0;
    svc_count = 0;
    app_mgr_ready = 1;
}

int app_register(uint32_t app_id, uint32_t app_type, uint32_t sandbox_level, uint32_t flags,
                  void **out_record) {
    if (app_count >= APP_CAPACITY) {
        return -1;
    }
    uint32_t slot;
    for (slot = 0; slot < APP_CAPACITY; ++slot) {
        if (app_records[slot].state == APP_STATE_INACTIVE) {
            break;
        }
    }
    if (slot >= APP_CAPACITY) {
        return -1;
    }

    app_record_t *rec = &app_records[slot];
    rec->id = app_id;
    rec->type = app_type;
    rec->state = APP_STATE_STOPPED;
    rec->sandbox = sandbox_level;
    rec->flags = flags;
    rec->process = 0;
    rec->capabilities = 0;
    rec->parent = 0;
    rec->comp_type = COMP_TYPE_STATIC;
    rec->intent_hash = 0;
    rec->doc_type = 0;
    rec->task_id = 0;
    rec->locale = LANG_DE_DE;
    ++app_count;

    if (out_record) {
        *out_record = rec;
    }
    return 0;
}

int app_start(void *record) {
    app_record_t *rec = (app_record_t *)record;
    if (rec->state != APP_STATE_STOPPED) {
        return -1;
    }
    rec->state = APP_STATE_STARTING;
    return 0;
}

int app_stop(void *record) {
    app_record_t *rec = (app_record_t *)record;
    if (rec->state != APP_STATE_RUNNING && rec->state != APP_STATE_SUSPENDED &&
        rec->state != APP_STATE_STARTING) {
        return -1;
    }
    rec->state = APP_STATE_STOPPING;
    return 0;
}

int app_suspend(void *record) {
    app_record_t *rec = (app_record_t *)record;
    if (rec->state != APP_STATE_RUNNING) {
        return -1;
    }
    rec->state = APP_STATE_SUSPENDED;
    return 0;
}

int app_resume(void *record) {
    app_record_t *rec = (app_record_t *)record;
    if (rec->state != APP_STATE_SUSPENDED) {
        return -1;
    }
    rec->state = APP_STATE_RUNNING;
    return 0;
}

int app_find_by_id(uint32_t app_id, void **out_record) {
    for (uint32_t i = 0; i < APP_CAPACITY; ++i) {
        if (app_records[i].id == app_id) {
            if (out_record) {
                *out_record = &app_records[i];
            }
            return 0;
        }
    }
    return -1;
}

int app_dispatch_intent(uint32_t intent_hash, uint32_t sender_id, uint32_t target_id) {
    /* NPSPEC-APP-SANDBOX-0001: sandbox_level wurde bisher nur gespeichert,
       nie durchgesetzt. APP_SANDBOX_ISOLATED darf per Definition keine
       Intents an eine ANDERE App senden (reine Selbstkommunikation bleibt
       erlaubt) - Standard/Strict/None schraenken das Dispatchen selbst
       nicht ein (das waere Aufgabe des jeweiligen Capability-Checks beim
       Empfaenger, nicht hier). */
    if (sender_id != target_id) {
        void *sender_record = 0;
        if (app_find_by_id(sender_id, &sender_record) == 0) {
            app_record_t *sender = (app_record_t *)sender_record;
            if (sender->sandbox == APP_SANDBOX_ISOLATED) {
                return -1;
            }
        }
    }

    if (intent_count >= INTENT_CAPACITY) {
        return -1;
    }
    uint32_t slot;
    for (slot = 0; slot < INTENT_CAPACITY; ++slot) {
        if (app_intents[slot].hash == 0) {
            break;
        }
    }
    if (slot >= INTENT_CAPACITY) {
        return -1;
    }

    app_intent_t *intent = &app_intents[slot];
    intent->hash = intent_hash;
    intent->sender = sender_id;
    intent->target = target_id;
    intent->data_ptr = 0;
    intent->data_len = 0;
    intent->flags = 0;
    ++intent_count;
    return 0;
}

int svc_register(uint32_t svc_id, uint32_t app_id, uint32_t api_version, uint32_t flags) {
    if (svc_count >= SVC_CAPACITY) {
        return -1;
    }
    uint32_t slot;
    for (slot = 0; slot < SVC_CAPACITY; ++slot) {
        if (svc_records[slot].id == 0) {
            break;
        }
    }
    if (slot >= SVC_CAPACITY) {
        return -1;
    }

    svc_record_t *svc = &svc_records[slot];
    svc->id = svc_id;
    svc->app = app_id;
    svc->api_version = api_version;
    svc->flags = flags;
    svc->endpoint = 0;
    ++svc_count;
    return 0;
}

uint32_t app_sandbox_level(void *record) {
    if (!record) {
        return APP_SANDBOX_NONE;
    }
    return ((app_record_t *)record)->sandbox;
}

int app_manager_self_test(void) {
    if (app_mgr_ready != 1) return -1;

    void *record = 0;
    if (app_register(0xDEADBEEFu, APP_TYPE_FOREGROUND, APP_SANDBOX_STANDARD,
                      APP_FLAG_PORTABLE, &record) != 0) {
        return -1;
    }

    void *found = 0;
    if (app_find_by_id(0xDEADBEEFu, &found) != 0) {
        app_record_t *rec = (app_record_t *)record;
        rec->state = APP_STATE_INACTIVE;
        --app_count;
        return -1;
    }

    if (app_start(found) != 0) {
        app_record_t *rec = (app_record_t *)found;
        rec->state = APP_STATE_INACTIVE;
        --app_count;
        return -1;
    }

    app_record_t *rec = (app_record_t *)found;
    rec->state = APP_STATE_RUNNING;
    if (app_stop(found) != 0) {
        rec->state = APP_STATE_INACTIVE;
        --app_count;
        return -1;
    }

    rec->state = APP_STATE_INACTIVE;
    --app_count;

    /* NPSPEC-APP-SANDBOX-0001: sandbox_level muss introspizierbar UND
       durchgesetzt sein - eine ISOLATED-App darf keine Intents an eine
       ANDERE App senden, Selbstkommunikation bleibt erlaubt. */
    void *isolated_record = 0;
    if (app_register(0xAAAAAAAAu, APP_TYPE_BACKGROUND, APP_SANDBOX_ISOLATED, 0u,
                      &isolated_record) != 0) {
        return -1;
    }
    if (app_sandbox_level(isolated_record) != APP_SANDBOX_ISOLATED) {
        ((app_record_t *)isolated_record)->state = APP_STATE_INACTIVE;
        --app_count;
        return -1;
    }
    if (app_dispatch_intent(0x1u, 0xAAAAAAAAu, 0xBBBBBBBBu) == 0) {
        ((app_record_t *)isolated_record)->state = APP_STATE_INACTIVE;
        --app_count;
        return -1; /* ISOLATED -> fremde App haette verweigert werden muessen */
    }
    if (app_dispatch_intent(0x1u, 0xAAAAAAAAu, 0xAAAAAAAAu) != 0) {
        ((app_record_t *)isolated_record)->state = APP_STATE_INACTIVE;
        --app_count;
        return -1; /* Selbstkommunikation bleibt erlaubt */
    }
    ((app_record_t *)isolated_record)->state = APP_STATE_INACTIVE;
    --app_count;

    if (app_sandbox_level(0) != APP_SANDBOX_NONE) return -1;

    return 0;
}
