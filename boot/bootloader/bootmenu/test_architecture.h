#ifndef NOVA_BOOT_TEST_ARCHITECTURE_H
#define NOVA_BOOT_TEST_ARCHITECTURE_H
#include <stdbool.h>
#include <stdint.h>

#define NOVA_BOOT_TEST_CAPACITY 64u
#define NOVA_BOOT_TEST_ALL_SUITES UINT32_MAX

typedef enum {NOVA_TEST_PENDING,NOVA_TEST_RUNNING,NOVA_TEST_PASSED,
    NOVA_TEST_FAILED,NOVA_TEST_SKIPPED} nova_test_status_t;
typedef enum {NOVA_TEST_UNIT,NOVA_TEST_COMPONENT,NOVA_TEST_INTEGRATION,
    NOVA_TEST_SYSTEM,NOVA_TEST_UI,NOVA_TEST_PERFORMANCE,NOVA_TEST_RECOVERY,
    NOVA_TEST_REGRESSION,NOVA_TEST_COMPATIBILITY,NOVA_TEST_STRESS}
    nova_test_level_t;
typedef enum {NOVA_TEST_FUNCTIONAL,NOVA_TEST_GRAPHICAL,NOVA_TEST_LAYOUT,
    NOVA_TEST_PERFORMANCE_KIND,NOVA_TEST_MEMORY,NOVA_TEST_ANIMATION,
    NOVA_TEST_RESOURCE,NOVA_TEST_SECURITY,NOVA_TEST_ROBUSTNESS}
    nova_test_kind_t;
typedef nova_test_status_t (*nova_boot_test_execute_fn)(void *context,
    uint32_t *actual,uint32_t *error_code);
typedef uint64_t (*nova_boot_test_clock_fn)(void);
typedef uint64_t (*nova_boot_test_state_fn)(void);

typedef struct {uint32_t test_id,suite_id,data_version;const char *name,*description;
    uint32_t expected,module,boot_phase,priority,hardware_profile,quality_profile;
    nova_test_level_t level;nova_test_kind_t kind;nova_boot_test_execute_fn execute;
    void *context;} nova_boot_test_case_t;
typedef struct {uint32_t test_id,suite_id,expected,actual,error_code,module;
    nova_test_status_t status;uint64_t duration_us,state_before,state_after;
    bool isolated;} nova_test_result_t;
typedef struct {uint32_t registered,executed,passed,failed,skipped,repetitions;
    uint64_t total_duration_us,average_duration_us;bool initialized,deterministic,
    isolated,boot_data_unchanged,security_preserved;} nova_boot_test_summary_t;

bool nova_boot_test_initialize(nova_boot_test_clock_fn clock,
                               nova_boot_test_state_fn state_probe);
bool nova_boot_test_register(const nova_boot_test_case_t *test_case);
bool nova_boot_test_run(uint32_t test_suite);
const nova_test_result_t *nova_boot_test_results(void);
const nova_boot_test_summary_t *nova_boot_test_summary(void);
bool nova_boot_test_generate_report(bool authorized,uint8_t *output,
                                    uint32_t capacity,uint32_t *written);
#endif
