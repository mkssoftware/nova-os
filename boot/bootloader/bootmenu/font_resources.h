#ifndef NOVA_BOOT_FONT_RESOURCES_H
#define NOVA_BOOT_FONT_RESOURCES_H

#include <stdbool.h>
#include <stdint.h>

#define NOVA_FONT_RESOURCE_CAPACITY 16u
#define NOVA_FONT_COVERAGE_CAPACITY 16u
#define NOVA_SYSTEM_FONT_ID 0x4e4f5641464f4e54ull

typedef enum {NOVA_FONT_FAMILY_SYSTEM,NOVA_FONT_FAMILY_DISPLAY,
    NOVA_FONT_FAMILY_MONOSPACE,NOVA_FONT_FAMILY_SYMBOLS,NOVA_FONT_FAMILY_UI,
    NOVA_FONT_FAMILY_THEME} nova_font_family_t;
typedef enum {NOVA_FONT_WEIGHT_LIGHT=300,NOVA_FONT_WEIGHT_REGULAR=400,
    NOVA_FONT_WEIGHT_MEDIUM=500,NOVA_FONT_WEIGHT_SEMIBOLD=600,
    NOVA_FONT_WEIGHT_BOLD=700} nova_font_weight_t;
typedef enum {NOVA_FONT_ROLE_PRIMARY,NOVA_FONT_ROLE_HEADING,NOVA_FONT_ROLE_MONOSPACE,
    NOVA_FONT_ROLE_SYMBOL,NOVA_FONT_ROLE_FALLBACK,NOVA_FONT_ROLE_COUNT} nova_font_role_t;
typedef struct {uint32_t first,last;} nova_font_coverage_t;
typedef struct {
    uint64_t font_id,resource_id,fallback_id;
    const char *name,*family,*style;
    uint32_t version,resource_version;
    nova_font_family_t family_type;
    nova_font_weight_t weight;
    uint8_t priority;
    const nova_font_coverage_t *coverage;
    uint8_t coverage_count;
} nova_font_resource_descriptor_t;
typedef struct {
    uint64_t font_id,resource_id,fallback_id;
    char name[48],family[32],style[24];
    uint32_t version,resource_version,references;
    nova_font_family_t family_type;
    nova_font_weight_t weight;
    uint8_t priority,coverage_count;
    nova_font_coverage_t coverage[NOVA_FONT_COVERAGE_CAPACITY];
    bool loaded,valid;
} nova_font_resource_t;
typedef struct {
    uint32_t initializations,registered,loads,cache_hits,releases,resolutions;
    uint32_t fallback_steps,missing_glyphs,duplicates,invalid_fonts,cycles;
    uint64_t active_roles[NOVA_FONT_ROLE_COUNT];
    bool initialized;
} nova_font_resource_diagnostics_t;

bool nova_font_resource_initialize(void);
bool nova_font_resource_register(const nova_font_resource_descriptor_t *descriptor);
const nova_font_resource_t *nova_font_resource_load(uint64_t font_id);
const nova_font_resource_t *nova_font_resource_find(uint64_t font_id);
bool nova_font_resource_release(uint64_t font_id);
bool nova_font_resource_validate(const nova_font_resource_t *font);
bool nova_font_resource_has_glyph(const nova_font_resource_t *font,uint32_t codepoint);
const nova_font_resource_t *nova_font_resource_resolve(uint64_t primary,uint32_t codepoint);
bool nova_font_theme_set(nova_font_role_t role,uint64_t font_id);
uint64_t nova_font_theme_get(nova_font_role_t role);
const nova_font_resource_diagnostics_t *nova_font_resource_diagnostics(void);

#endif
