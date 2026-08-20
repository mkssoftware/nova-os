#ifndef NOVA_BOOT_NAVIGATION_H
#define NOVA_BOOT_NAVIGATION_H

#include <stdint.h>
#include <stdbool.h>
#include "motion.h"

#define NOVA_NAVIGATION_CAPACITY 64u
#define NOVA_NAVIGATION_TEST_CAPACITY 10u

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

typedef enum {NOVA_NAVIGATION_TEST_PASSED,NOVA_NAVIGATION_TEST_FAILED,
    NOVA_NAVIGATION_TEST_SKIPPED} nova_navigation_test_outcome_t;
typedef enum {NOVA_NAV_TEST_BOOT_MANAGER,NOVA_NAV_TEST_RECOVERY,
    NOVA_NAV_TEST_INSTALLER,NOVA_NAV_TEST_DIAGNOSTICS,NOVA_NAV_TEST_DIALOG,
    NOVA_NAV_TEST_LIST,NOVA_NAV_TEST_MENU,NOVA_NAV_TEST_KEYBOARD,
    NOVA_NAV_TEST_POINTER,NOVA_NAV_TEST_TOUCH} nova_navigation_test_route_t;
typedef struct {uint32_t route_id,step_count;nova_navigation_test_outcome_t status;
    uint64_t duration_us;uint16_t start_page,target_page,start_focus,target_focus;
    uint16_t focus_path;uint8_t input_device;uint32_t detected_errors;
    bool back_restored,dead_end,loop_detected,configuration_changed;
} nova_navigation_test_result_t;
typedef struct {uint8_t count,passed,failed,skipped;uint32_t reports;
    bool initialized,isolated,deterministic,configuration_unchanged;
} nova_navigation_test_summary_t;

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
bool nova_navigation_test_initialize(void);
bool nova_navigation_test_execute(uint32_t route_id);
const nova_navigation_test_result_t *nova_navigation_test_results(void);
const nova_navigation_test_summary_t *nova_navigation_test_summary(void);
bool nova_navigation_test_generate_report(bool authorized,uint8_t *output,
                                          uint32_t capacity,uint32_t *written);

#endif
