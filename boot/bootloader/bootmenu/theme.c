#include "theme.h"
#include "resources.h"
#include "motion.h"
#include "compositor.h"

static const nova_theme_tokens_t themes[NOVA_THEME_COUNT] = {
    [NOVA_THEME_DARK] = {
        0xff101113u, 0xff191b1eu, 0xff22262au, 0xfff4f6f8u,
        0xffaeb2b7u, 0xff267cc1u, 0xff4cc2ffu, 0xd022262au,
        0xff3a3e43u, 0xff66686au, 0xff44b878u, 0xffffb547u,
        0xffc52b58u, 0xff4cc2ffu, 4, 8, 8, 16, 210, false, false, false
    },
    [NOVA_THEME_LIGHT] = {
        0xfff2f4f7u, 0xffffffffu, 0xffe7ebf0u, 0xff17191cu,
        0xff555b63u, 0xff1769aau, 0xff005fccu, 0xd9d9e8f5u,
        0xffb2bac4u, 0xff8b929bu, 0xff16794bu, 0xff8a5700u,
        0xffa3123fu, 0xff005fccu, 4, 8, 8, 16, 235, false, false, false
    },
    [NOVA_THEME_HIGH_CONTRAST] = {
        0xff000000u, 0xff000000u, 0xff101010u, 0xffffffffu,
        0xffffffffu, 0xff00b7ffu, 0xffffff00u, 0xff003f75u,
        0xffffffffu, 0xffb0b0b0u, 0xff00ff70u, 0xffffff00u,
        0xffff4060u, 0xff00ffffu, 0, 2, 10, 18, 255, true, true, true
    }
};
static nova_theme_id_t active;
static bool reduced_override;
static nova_theme_diagnostics_t diagnostics;

static uint32_t brightness(uint32_t color)
{
    return (((color >> 16) & 255u) * 299u + ((color >> 8) & 255u) * 587u +
            (color & 255u) * 114u) / 1000u;
}

bool nova_theme_validate(nova_theme_id_t id)
{
    if (id >= NOVA_THEME_COUNT) return false;
    const nova_theme_tokens_t *t = &themes[id];
    uint32_t bg = brightness(t->background), fg = brightness(t->text_primary);
    uint32_t difference = bg > fg ? bg - fg : fg - bg;
    return t->background && t->surface && t->text_primary && t->accent &&
           t->focus && difference >= 100u;
}

bool nova_theme_initialize(void)
{
    diagnostics = (nova_theme_diagnostics_t){0};
    reduced_override = false;
    for (uint32_t i = 0; i < NOVA_THEME_COUNT; ++i) {
        if (!nova_theme_validate((nova_theme_id_t)i)) {
            ++diagnostics.validation_errors;
            return false;
        }
    }
    if (!nova_resource_register("boot://themes/dark", NOVA_RESOURCE_THEME, 1,
                                &themes[0], sizeof(themes[0]), 0, 0) ||
        !nova_resource_register("boot://themes/light", NOVA_RESOURCE_THEME, 1,
                                &themes[1], sizeof(themes[1]), 0, 0) ||
        !nova_resource_register("boot://themes/high-contrast", NOVA_RESOURCE_THEME, 1,
                                &themes[2], sizeof(themes[2]), 0, 0)) return false;
    active = NOVA_THEME_DARK;
    diagnostics.active = active;
    return true;
}

bool nova_theme_activate(nova_theme_id_t id)
{
    if (!nova_theme_validate(id)) { ++diagnostics.validation_errors; return false; }
    active = id;
    diagnostics.active = id;
    ++diagnostics.activations;
    if (themes[id].high_contrast) ++diagnostics.accessibility_overrides;
    nova_motion_set_reduced(themes[id].reduced_motion || reduced_override);
    nova_compositor_set_fallback(themes[id].opaque_materials ? 3 : 0);
    return true;
}

nova_theme_id_t nova_theme_active(void) { return active; }
const nova_theme_tokens_t *nova_theme_tokens(void) { return &themes[active]; }
void nova_theme_set_reduced_motion(bool enabled)
{
    reduced_override = enabled;
    nova_motion_set_reduced(themes[active].reduced_motion || enabled);
    ++diagnostics.accessibility_overrides;
}
bool nova_theme_reduced_motion(void) { return themes[active].reduced_motion || reduced_override; }
const nova_theme_diagnostics_t *nova_theme_diagnostics(void) { return &diagnostics; }
