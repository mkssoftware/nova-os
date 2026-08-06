#include <stdint.h>
#include <stdio.h>
#include "../boot/bootloader/bootmenu/motion.h"
#include "../boot/bootloader/bootmenu/compositor.h"

static uint32_t writes;
void pixel_set(uint64_t x, uint64_t y, uint32_t color)
{
    (void)x; (void)y; (void)color; ++writes;
}

static int check(int condition, const char *message)
{
    if (!condition) { fprintf(stderr, "FAIL: %s\n", message); return 1; }
    return 0;
}

int main(void)
{
    int failed = 0;
    int32_t value = 0;
    nova_motion_initialize();
    nova_animation_t description = {
        &value, 0, 1000, 100, 20, 200, 7, 2, 0,
        NOVA_PROPERTY_X, NOVA_EASE_LINEAR, NOVA_MOTION_CREATED,
        false, false, true
    };
    nova_animation_t *animation = nova_motion_create(&description);
    failed |= check(animation != 0, "Animation anlegen");
    nova_motion_update(119);
    failed |= check(value == 0, "Startverzoegerung");
    nova_motion_update(220);
    failed |= check(value == 500, "zeitbasierte Interpolation");
    failed |= check(nova_motion_interrupt(animation), "Unterbrechung");
    nova_motion_update(260);
    failed |= check(value == 500, "Pause haelt Property");
    failed |= check(nova_motion_resume(animation, 260), "Fortsetzung");
    failed |= check(nova_motion_redirect(animation, 1500, 260), "Zielumleitung");
    nova_motion_update(460);
    failed |= check(value == 1500, "umgeleitetes Ziel");

    nova_motion_set_reduced(true);
    int32_t reduced_value = 0;
    nova_animation_t reduced = description;
    reduced.target = &reduced_value;
    reduced.start_ms = 500;
    reduced.delay_ms = 0;
    reduced.easing = NOVA_EASE_SPRING;
    nova_animation_t *reduced_animation = nova_motion_create(&reduced);
    failed |= check(reduced_animation && reduced_animation->easing == NOVA_EASE_OUT_CUBIC,
                    "Reduced Motion ersetzt Spring");
    failed |= check(nova_motion_create(&reduced) == 0, "doppelte Property abweisen");
    nova_motion_update(1000);

    nova_dialog_motion_t dialog = {0};
    failed |= check(nova_dialog_enter(&dialog), "Dialog Enter Motion");
    nova_motion_update(1200);
    failed |= check(dialog.opacity == 255 && dialog.visible, "Dialog eingeblendet");
    failed |= check(nova_dialog_exit(&dialog), "Dialog Exit Motion");
    nova_motion_update(1400);
    failed |= check(dialog.opacity == 0, "Dialog ausgeblendet");

    nova_navigation_motion_t navigation = {0};
    failed |= check(nova_navigation_begin(&navigation, true) && navigation.target_x == 0,
                    "Reduced-Motion-Navigation als Cross Fade");
    nova_focus_motion_t focus = {0};
    failed |= check(nova_focus_set(&focus, true, true), "Fokus- und Auswahlmotion");
    nova_progress_motion_t progress = {0};
    failed |= check(nova_progress_set(&progress, 750), "determinate Progress Motion");
    failed |= check(!nova_progress_set(&progress, 1001) && progress.indeterminate,
                    "ungueltiger Progress faellt sicher zurueck");
    nova_motion_budget_update(20000, 1200);
    failed |= check(nova_motion_budget()->violations == 1 &&
                    nova_motion_budget()->quality == 3, "Budget-Degradation");

    failed |= check(nova_compositor_initialize(8, 8), "Compositor initialisieren");
    nova_surface_t *base = nova_surface_acquire();
    nova_surface_t *overlay = nova_surface_acquire();
    failed |= check(base && overlay, "feste Surface-Pools");
    nova_surface_clear(base, 0xff101010u);
    nova_surface_clear(overlay, 0x00000000u);
    nova_surface_rect(overlay, (nova_rect_t){2, 2, 3, 3}, 0x80267cc1u);
    nova_layer_t lower = { 1, base, {0,0,8,8}, {0,0,8,8}, 0, 255,
                           NOVA_MATERIAL_NONE, true, false, false, true };
    nova_layer_t modal = { 2, overlay, {0,0,8,8}, {0,0,8,8}, 10, 255,
                           NOVA_MATERIAL_ACRYLIC, true, true, true, true };
    failed |= check(nova_compositor_begin_frame(), "Frame beginnen");
    failed |= check(nova_compositor_submit_layer(&modal), "Modal Layer einreichen");
    failed |= check(nova_compositor_submit_layer(&lower), "stabile Z-Sortierung");
    failed |= check(!nova_compositor_input_allowed(1) && nova_compositor_input_allowed(2),
                    "modale Eingabesperre");
    failed |= check(nova_compositor_compose() && writes != 0, "Damage compositing");
    nova_compositor_set_fallback(3);
    failed |= check(nova_compositor_diagnostics()->fallback_level == 3,
                    "Material-Fallback");
    failed |= check(nova_compositor_diagnostics()->frames == 1,
                    "Compositor-Diagnose");

    if (!failed) puts("BOOT_UI_RUNTIME_OK");
    return failed ? 1 : 0;
}
