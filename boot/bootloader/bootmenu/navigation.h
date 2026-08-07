#ifndef NOVA_BOOT_NAVIGATION_H
#define NOVA_BOOT_NAVIGATION_H

#include <stdint.h>
#include <stdbool.h>
#include "motion.h"

#define NOVA_NAVIGATION_CAPACITY 64u

typedef enum {
    NOVA_NAV_PUSH,
    NOVA_NAV_POP,
    NOVA_NAV_REPLACE,
    NOVA_NAV_DIALOG,
    NOVA_NAV_RECOVERY
} nova_navigation_transition_t;

typedef struct {
    uint16_t page;
    uint16_t selection;
    uint16_t focus_id;
    int32_t scroll;
    uint32_t context;
} nova_navigation_entry_t;

typedef struct {
    uint16_t depth;
    uint16_t peak_depth;
    uint32_t pushes;
    uint32_t pops;
    uint32_t replaces;
    uint32_t resets;
    uint32_t focus_restores;
    uint32_t rejected;
    nova_navigation_transition_t last_transition;
    bool transition_running;
    bool focus_locked;
    uint32_t visual_started;
    uint32_t visual_completed;
    uint32_t visual_cancelled;
} nova_navigation_diagnostics_t;

typedef struct {
    int32_t offset_dlu;
    int32_t opacity;
    uint32_t duration_ms;
    nova_navigation_transition_t type;
    bool entering;
    bool running;
    bool reduced_motion;
} nova_navigation_visual_t;

void nova_navigation_initialize(nova_navigation_entry_t root);
bool nova_navigation_push(nova_navigation_entry_t entry,
                          nova_navigation_transition_t transition);
bool nova_navigation_replace(nova_navigation_entry_t entry);
bool nova_navigation_reset(void);
bool nova_navigation_back(nova_navigation_entry_t *restored);
bool nova_navigation_can_go_back(void);
const nova_navigation_entry_t *nova_navigation_current(void);
bool nova_navigation_update(uint16_t selection, uint16_t focus_id,
                            int32_t scroll, uint32_t context);
void nova_navigation_transition_complete(void);
bool nova_navigation_visual_begin(nova_navigation_transition_t type, bool entering,
                                  uint64_t start_ms);
void nova_navigation_visual_complete(void);
void nova_navigation_visual_cancel(void);
const nova_navigation_visual_t *nova_navigation_visual(void);
const nova_navigation_diagnostics_t *nova_navigation_diagnostics(void);

#endif
