#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "../boot/bootloader/bootmenu/motion.h"
#include "../boot/bootloader/bootmenu/compositor.h"
#include "../boot/bootloader/bootmenu/graphics.h"
#include "../boot/bootloader/bootmenu/resolution.h"
#include "../boot/bootloader/bootmenu/controls.h"
#include "../boot/bootloader/bootmenu/text.h"
#include "../boot/bootloader/bootmenu/unicode.h"
#include "../boot/bootloader/bootmenu/resources.h"
#include "../boot/bootloader/bootmenu/resource_version.h"
#include "../boot/bootloader/bootmenu/font_resources.h"
#include "../boot/bootloader/bootmenu/animation_resources.h"
#include "../boot/bootloader/bootmenu/icons.h"
#include "../boot/bootloader/bootmenu/input.h"
#include "../boot/bootloader/bootmenu/diagnostics.h"
#include "../boot/bootloader/bootmenu/recovery.h"
#include "../boot/bootloader/bootmenu/memory.h"
#include "../boot/bootloader/bootmenu/configuration.h"
#include "../boot/bootloader/bootmenu/runtime.h"
#include "../boot/bootloader/bootmenu/state_model.h"
#include "../boot/bootloader/bootmenu/branding.h"
#include "../boot/bootloader/bootmenu/design.h"
#include "../boot/bootloader/bootmenu/architecture.h"
#include "../boot/bootloader/bootmenu/scene_graph.h"
#include "../boot/bootloader/bootmenu/render_queue.h"
#include "../boot/bootloader/bootmenu/surface_manager.h"
#include "../boot/bootloader/bootmenu/layer_manager.h"
#include "../boot/bootloader/bootmenu/framebuffer_backend.h"
#include "../boot/bootloader/bootmenu/present_scheduler.h"
#include "../boot/bootloader/bootmenu/dirty_manager.h"
#include "../boot/bootloader/bootmenu/clip_mask.h"
#include "../boot/bootloader/bootmenu/transform2d.h"
#include "../boot/bootloader/bootmenu/rounded_geometry.h"
#include "../boot/bootloader/bootmenu/effects.h"
#include "../boot/bootloader/bootmenu/background_blur.h"
#include "../boot/bootloader/bootmenu/image_renderer.h"
#include "../boot/bootloader/bootmenu/render_quality.h"
#include "../boot/bootloader/bootmenu/software_renderer.h"
#include "../boot/bootloader/bootmenu/gop_backend.h"
#include "../boot/bootloader/bootmenu/theme.h"
#include "../boot/bootloader/bootmenu/layout.h"
#include "../boot/bootloader/bootmenu/navigation.h"
#include "../boot/bootloader/bootmenu/dialog.h"
#include "../boot/bootloader/bootmenu/page.h"

static uint32_t runtime_task_order[4],runtime_task_count;
static uint32_t state_callback_count;
static uint16_t scene_visit_order[8],scene_visit_count;
static void runtime_task(uint32_t id,void *context)
{(void)context;if(runtime_task_count<4)runtime_task_order[runtime_task_count++]=id;}
static void state_callback(const nova_state_object_t *object,uint8_t old_state,
                           uint8_t new_state,void *context)
{(void)object;(void)old_state;(void)new_state;(void)context;++state_callback_count;}
static uint32_t graphics_frontbuffer[80];
static uint64_t present_test_clock;
static uint64_t test_clock_us(void *context)
{(void)context;present_test_clock+=50;return present_test_clock;}
static uint8_t test_vector_mask(int32_t x,int32_t y,void *context)
{(void)context;return ((x+y)&1)?128u:255u;}

static int check(int condition, const char *message)
{
    if (!condition) { fprintf(stderr, "FAIL: %s\n", message); return 1; }
    return 0;
}

static bool page_event_handler(nova_view_t *view,uint32_t event,void *context)
{
    uint32_t *visits=(uint32_t *)context;++*visits;
    return event==77 && view->type==NOVA_VIEW_CONTENT;
}
static bool scene_visit(nova_scene_node_t *node,void *context)
{
    (void)context;
    if(scene_visit_count<8)scene_visit_order[scene_visit_count++]=node->id;
    return true;
}

int main(void)
{
    int failed = 0;
    nova_transform2d_t transform=nova_transform_identity();
    nova_point2d_t transformed_point_value={0},round_trip={0};
    failed|=check(nova_transform_scale(&transform,2.0f,3.0f)&&
        nova_transform_rotate(&transform,1.57079632679f)&&
        nova_transform_translate(&transform,10.0f,20.0f)&&
        nova_transform_point(&transform,(nova_point2d_t){1,0},
                             &transformed_point_value)&&
        transformed_point_value.x>9.99f&&transformed_point_value.x<10.01f&&
        transformed_point_value.y>21.99f&&transformed_point_value.y<22.01f,
        "Scale, Rotation und Translation in fester Reihenfolge");
    nova_rect_t transformed_bounds={0};
    failed|=check(nova_transform_bounds(&transform,(nova_rect_t){0,0,2,1},
        &transformed_bounds)&&transformed_bounds.x==7&&transformed_bounds.y==20&&
        transformed_bounds.width==3&&transformed_bounds.height==4,
        "Rotierte Bounding Box aus allen vier Ecken berechnen");
    nova_transform2d_t inverse={0};
    failed|=check(nova_transform_inverse(&inverse,&transform)&&
        nova_transform_point(&inverse,transformed_point_value,&round_trip)&&
        round_trip.x>0.99f&&round_trip.x<1.01f&&
        round_trip.y>-0.01f&&round_trip.y<0.01f,
        "Affine Matrix sicher invertieren");
    nova_transform2d_t mirror=nova_transform_identity();
    nova_transform2d_t shear=nova_transform_identity();
    nova_transform2d_t pivot_operation=nova_transform_identity();
    nova_transform2d_t pivot_target=nova_transform_identity();
    nova_point2d_t pivot_result={0},mirror_result={0},shear_result={0};
    failed|=check(nova_transform_mirror(&mirror,true,false)&&
        nova_transform_shear(&shear,0.5f,0.25f)&&
        nova_transform_rotate(&pivot_operation,3.14159265359f)&&
        nova_transform_pivot(&pivot_target,&pivot_operation,5,5)&&
        nova_transform_point(&mirror,(nova_point2d_t){1,0},&mirror_result)&&
        nova_transform_point(&shear,(nova_point2d_t){2,4},&shear_result)&&
        nova_transform_point(&pivot_target,(nova_point2d_t){4,5},&pivot_result)&&
        mirror_result.x<-0.99f&&shear_result.x>3.99f&&
        pivot_result.x>5.98f&&pivot_result.x<6.02f,
        "Spiegelung, Shear und Pivot kombinieren");
    nova_fixed_transform2d_t fixed_transform={0},fixed_inverse={0};
    nova_fixed_point2d_t fixed_point={0},fixed_round_trip={0};
    failed|=check(nova_transform_to_fixed(&transform,&fixed_transform)&&
        nova_transform_fixed_inverse(&fixed_inverse,fixed_transform)&&
        nova_transform_fixed_point(fixed_transform,
            (nova_fixed_point2d_t){65536,0},&fixed_point)&&
        nova_transform_fixed_point(fixed_inverse,fixed_point,&fixed_round_trip)&&
        fixed_round_trip.x>65530&&fixed_round_trip.x<65542,
        "Float-API deterministisch in 16.16-Ausfuehrung ueberfuehren");
    union {uint32_t u;float f;} not_a_number={0x7fc00000u};
    nova_transform2d_t invalid_transform=nova_transform_identity();
    invalid_transform.tx=not_a_number.f;
    nova_transform_stack_reset();
    failed|=check(!nova_transform_valid(&invalid_transform)&&
        nova_transform_stack_push(&transform)&&
        nova_transform_stack_current().tx>9.99f&&
        nova_transform_stack_pop()&&!nova_transform_stack_pop(),
        "NaN abweisen und Transformationsstack gegen Underflow sichern");
    nova_rounded_reset_diagnostics();
    nova_round_rect_t rounded={0},asymmetric={0},scaled_rounded={0},animated={0};
    failed|=check(nova_round_rect_create(&rounded,2,2,10,8,99)&&
        rounded.corners.top_left==4&&rounded.corners.bottom_right==4&&
        nova_round_rect_create_corners(&asymmetric,(nova_rect_t){2,2,10,8},
            (nova_corner_radius_t){0,2,3,4})&&
        asymmetric.corners.top_left==0&&asymmetric.corners.bottom_left==4,
        "Rounded Geometry begrenzt uniforme und individuelle Radien");
    failed|=check(nova_round_rect_contains(&rounded,6,5)&&
        !nova_round_rect_contains(&rounded,2,2)&&
        nova_round_rect_coverage(&rounded,2,3,NOVA_ROUNDED_AA_HIGH)>0&&
        nova_round_rect_coverage(&rounded,2,3,NOVA_ROUNDED_AA_HIGH)<255,
        "Formgenaues Hit Testing und analytisches Kanten-AA");
    failed|=check(nova_round_rect_scale(&asymmetric,98304,&scaled_rounded)&&
        scaled_rounded.bounds.width==15&&scaled_rounded.corners.bottom_left==6&&
        nova_round_rect_lerp(&asymmetric,&scaled_rounded,500,&animated)&&
        animated.bounds.width==13&&animated.corners.bottom_left==5,
        "DLU-Skalierung und animierbare Geometrieparameter");
    uint32_t rounded_pixels[16*16]={0};
    nova_surface_t rounded_surface={.pixels=rounded_pixels,.width=16,.height=16,.stride=16};
    failed|=check(nova_round_rect_render(&rounded_surface,&asymmetric,0xff267cc1u,
        NOVA_ROUNDED_AA_HIGH)&&rounded_pixels[5*16+6]==0xff267cc1u&&
        nova_round_rect_render_border(&rounded_surface,&asymmetric,1,
            NOVA_ROUNDED_BORDER_INSIDE,0xffffffffu,NOVA_ROUNDED_AA_STANDARD)&&
        nova_rounded_diagnostics()->rendered==2&&
        nova_rounded_diagnostics()->anti_aliasing,
        "Heapfreies Rounded Rendering mit AA und Innenrahmen");
    nova_effect_initialize(NOVA_EFFECT_QUALITY_HIGH);nova_effect_begin_frame();
    nova_effect_object_t effect_object={0};
    nova_effect_t effect_variants[8]={
        {NOVA_EFFECT_DROP_SHADOW,0xff000000u,3,0.5f,1,1},
        {NOVA_EFFECT_INNER_SHADOW,0xff000000u,2,0.4f,0,0},
        {NOVA_EFFECT_AMBIENT_SHADOW,0xff000000u,4,0.2f,0,0},
        {NOVA_EFFECT_CONTACT_SHADOW,0xff000000u,1,0.7f,0,1},
        {NOVA_EFFECT_OUTER_GLOW,0xff267cc1u,3,0.6f,0,0},
        {NOVA_EFFECT_INNER_GLOW,0xffffffffu,2,0.3f,0,0},
        {NOVA_EFFECT_FOCUS_GLOW,0xff267cc1u,2,0.8f,0,0},
        {NOVA_EFFECT_ACCENT_GLOW,0xffffa52bu,2,0.5f,0,0}};
    failed|=check(nova_effect_object_create(&effect_object,&rounded),
        "Effektobjekt aus Rounded-Kontur erzeugen");
    for(uint8_t i=0;i<4;++i)failed|=check(nova_shadow_set(&effect_object,&effect_variants[i]),
        "Schatteneffekt in stabiler Reihenfolge speichern");
    for(uint8_t i=4;i<8;++i)failed|=check(nova_glow_set(&effect_object,&effect_variants[i]),
        "Gloweffekt unabhaengig speichern");
    nova_effect_t invalid_effect=effect_variants[0];invalid_effect.radius=-1;
    nova_effect_t animated_effect={0};
    failed|=check(!nova_shadow_set(&effect_object,&invalid_effect)&&
        nova_effect_lerp(&effect_variants[0],&(nova_effect_t){NOVA_EFFECT_DROP_SHADOW,
            0xffffffffu,5,1,3,3},500,&animated_effect)&&
        animated_effect.radius>3.99f&&animated_effect.radius<4.01f&&
        !nova_glow_set(&effect_object,&effect_variants[4]),
        "Effektvalidierung, Animation und feste Achtfachkapazitaet");
    for(uint16_t i=0;i<16*16;++i)rounded_pixels[i]=0;
    failed|=check(nova_effect_render(&effect_object,&rounded_surface,
        NOVA_EFFECT_PHASE_SHADOW,(nova_rect_t){0,0,16,16},0)&&
        nova_effect_render(&effect_object,&rounded_surface,
        NOVA_EFFECT_PHASE_GLOW,(nova_rect_t){0,0,16,16},0)&&
        nova_effect_diagnostics()->shadows==4&&nova_effect_diagnostics()->glows==4&&
        nova_effect_diagnostics()->rendered_pixels>0,
        "Alle Shadow-/Glowtypen konturtreu rendern");
    nova_effect_set_quality(NOVA_EFFECT_QUALITY_LOW);
    failed|=check(nova_effect_diagnostics()->quality==NOVA_EFFECT_QUALITY_LOW,
        "Effektqualitaet deterministisch reduzieren");
    nova_effect_clear(&effect_object);
    uint32_t blur_source_pixels[16*16],blur_destination_pixels[16*16]={0};
    for(uint16_t i=0;i<16*16;++i)blur_source_pixels[i]=0xff000000u;
    blur_source_pixels[6*16+6]=0xffffffffu;
    nova_surface_t blur_source={.pixels=blur_source_pixels,.width=16,.height=16,.stride=16};
    nova_surface_t blur_destination={.pixels=blur_destination_pixels,.width=16,.height=16,.stride=16};
    nova_round_rect_t blur_rounding={0};
    failed|=check(nova_round_rect_create(&blur_rounding,2,2,8,8,2)&&
        nova_blur_initialize(16,16,1024*1024),
        "Vorreservierte Blur-Laufzeit initialisieren");
    nova_blur_region_t blur_region={{2,2,8,8},2.0f,NOVA_BLUR_QUALITY_HIGH,
        NOVA_BLUR_KERNEL_DUAL_PASS_GAUSSIAN,&blur_rounding,0};
    nova_blur_result_t blur_first=nova_blur_render(&blur_region,&blur_source,&blur_destination);
    failed|=check(blur_first==NOVA_BLUR_OK&&
        blur_destination_pixels[6*16+6]!=0xffffffffu&&
        (blur_destination_pixels[6*16+5]&0x00ffffffu)!=0&&
        (blur_destination_pixels[2*16+2]>>24)>0&&
        (blur_destination_pixels[2*16+2]>>24)<255,
        "Dual-Pass-Blur verarbeitet nur gerundete Hintergrundregion");
    for(uint16_t i=0;i<16*16;++i)blur_destination_pixels[i]=0;
    failed|=check(nova_blur_render(&blur_region,&blur_source,&blur_destination)==NOVA_BLUR_OK&&
        nova_blur_diagnostics()->cache_hits==1,
        "Unveraenderte Blur-Flaeche aus Cache wiederverwenden");
    nova_rect_t unrelated_blur_dirty={0,0,1,1},related_blur_dirty={5,5,2,2};
    nova_blur_invalidate(&unrelated_blur_dirty);
    failed|=check(nova_blur_diagnostics()->cache_valid,
        "Fremde Dirty Region behaelt Blur-Cache");
    nova_blur_invalidate(&related_blur_dirty);
    failed|=check(!nova_blur_diagnostics()->cache_valid&&
        nova_blur_render(&blur_region,&blur_source,&blur_destination)==NOVA_BLUR_OK&&
        nova_blur_diagnostics()->cache_misses==2,
        "Ueberlappende Dirty Region invalidiert Blur-Cache");
    failed|=check(nova_blur_initialize(16,16,1)&&nova_blur_diagnostics()->degraded,
        "Speichermangel degradiert Blur automatisch auf Low");
    nova_blur_shutdown();
    nova_controls_initialize(0);
    nova_scene_node_t *scene_parent=nova_scene_create(NOVA_SCENE_CONTAINER);
    nova_scene_node_t *scene_child=nova_scene_create(NOVA_SCENE_CONTROL);
    nova_scene_matrix_t translated=nova_scene_identity();translated.tx=5*65536;
    failed|=check(scene_parent&&scene_child&&
        nova_scene_attach(nova_scene_root(),scene_parent)&&
        nova_scene_attach(scene_parent,scene_child)&&
        nova_scene_set_bounds(scene_parent,(nova_rect_t){10,20,100,80})&&
        nova_scene_set_bounds(scene_child,(nova_rect_t){3,4,20,10})&&
        nova_scene_set_transform(scene_child,translated)&&
        nova_scene_set_opacity(scene_parent,500)&&nova_scene_set_opacity(scene_child,500),
        "Retained Scene Graph mit Hierarchie und Eigenschaften");
    failed|=check(!nova_scene_attach(scene_child,scene_parent)&&
        nova_scene_diagnostics()->rejected_cycles==1,
        "Scene Graph verhindert Zyklen");
    scene_visit_count=0;
    failed|=check(nova_scene_traverse(scene_visit,0,false)&&scene_visit_count==3&&
        scene_visit_order[0]==NOVA_SCENE_ROOT&&scene_visit_order[1]==scene_parent->id&&
        scene_visit_order[2]==scene_child->id&&scene_child->world_bounds.x==18&&
        scene_child->world_bounds.y==24&&scene_child->world_opacity==250,
        "Scene Graph traversiert deterministisch und vererbt Transform/Opacity");
    nova_scene_matrix_t quarter_turn={0,-65536,65536,0,0,0};
    failed|=check(nova_scene_set_transform(scene_child,quarter_turn)&&
        nova_scene_traverse(0,0,false)&&scene_child->world_bounds.x==3&&
        scene_child->world_bounds.y==24&&scene_child->world_bounds.width==10&&
        scene_child->world_bounds.height==20,
        "Scene Graph akzeptiert 90-Grad-Matrix und berechnet World Bounds");
    failed|=check(!nova_scene_destroy(scene_parent)&&nova_scene_destroy(scene_child)&&
        nova_scene_destroy(scene_parent),"Scene Graph zerstoert Kind vor Eltern");
    nova_diag_initialize();
    nova_recovery_initialize();
    failed |= check(!nova_recovery_report(0,NOVA_UI_SUBSYSTEM_RENDERING,
                    NOVA_UI_ERROR_WARNING,NOVA_RECOVERY_RETRY,0),
                    "Recovery weist ungueltigen Fehlercode ab");
    failed |= check(nova_recovery_report(0x80010010u,NOVA_UI_SUBSYSTEM_LAYOUT,
                    NOVA_UI_ERROR_RECOVERABLE,NOVA_RECOVERY_RETRY,1)&&
                    nova_recovery_diagnostics()->state==NOVA_RECOVERY_VALIDATING&&
                    nova_recovery_validate(false,2)&&
                    nova_recovery_diagnostics()->level==NOVA_RECOVERY_LOCAL_FALLBACK&&
                    nova_recovery_diagnostics()->state==NOVA_RECOVERY_HEALTHY,
                    "Recovery eskaliert Retry deterministisch zum lokalen Fallback");
    nova_recovery_initialize();
    failed |= check(nova_recovery_report(0x80010011u,NOVA_UI_SUBSYSTEM_RENDERING,
                    NOVA_UI_ERROR_CRITICAL,NOVA_RECOVERY_SAFE_MODE,3)&&
                    nova_recovery_safe_mode()&&nova_recovery_continue_boot()&&
                    !nova_recovery_feature_enabled(NOVA_RECOVERY_FEATURE_MOTION)&&
                    !nova_recovery_feature_enabled(NOVA_RECOVERY_FEATURE_GLASS)&&
                    nova_diag_quality()->quality==NOVA_QUALITY_SAFE,
                    "Safe Mode deaktiviert Effekte und setzt sicheres Qualitaetsprofil");
    nova_recovery_initialize();
    failed |= check(nova_recovery_watchdog_configure(NOVA_UI_SUBSYSTEM_INPUT,10,100)&&
                    nova_recovery_watchdog_check(109)&&
                    !nova_recovery_watchdog_check(110)&&
                    nova_recovery_diagnostics()->watchdog_timeouts==1&&
                    nova_recovery_diagnostics()->level==NOVA_RECOVERY_DISABLE_SUBSYSTEM,
                    "Watchdog erkennt Timeout genau einmal und isoliert Subsystem");
    failed |= check(nova_recovery_report(0x8001ffffu,NOVA_UI_SUBSYSTEM_RESOURCES,
                    NOVA_UI_ERROR_FATAL,NOVA_RECOVERY_RETRY,120)&&
                    nova_recovery_text_mode()&&nova_recovery_continue_boot()&&
                    nova_recovery_record_count()>=2&&nova_recovery_record(0)!=0&&
                    nova_recovery_record(nova_recovery_record_count())==0,
                    "Fataler UI-Fehler wechselt in Textmodus und laesst Boot weiterlaufen");
    nova_recovery_initialize();nova_memory_initialize();
    void *memory16=nova_memory_allocate(NOVA_MEMORY_RUNTIME,33,0x101u,16);
    void *memory64=nova_memory_allocate(NOVA_MEMORY_RENDER,65,0x202u,64);
    failed |= check(memory16&&memory64&&((uintptr_t)memory16&15u)==0&&
                    ((uintptr_t)memory64&63u)==0&&
                    nova_memory_validate_pointer(memory16,NOVA_MEMORY_RUNTIME,33)&&
                    nova_memory_validate_pointer(memory64,NOVA_MEMORY_RENDER,65)&&
                    nova_memory_object(memory64)->owner==0x202u,
                    "Memory Manager allokiert und validiert 16/64-Byte ausgerichtet");
    failed |= check(nova_memory_retain(memory16)&&nova_memory_release(memory16)&&
                    nova_memory_object(memory16)->references==1&&
                    nova_memory_release(memory16)&&!nova_memory_release(memory16)&&
                    nova_memory_statistics()->double_frees==1,
                    "Referenzzaehlung und Double-Free-Erkennung");
    void *frame_a=nova_memory_allocate(NOVA_MEMORY_FRAME,31,0x303u,16);
    void *frame_b=nova_memory_allocate(NOVA_MEMORY_FRAME,77,0x303u,64);
    failed |= check(frame_a&&frame_b&&nova_memory_reset_frame()&&
                    nova_memory_pool_statistics(NOVA_MEMORY_FRAME)->used==0&&
                    !nova_memory_validate_pointer(frame_a,NOVA_MEMORY_FRAME,1)&&
                    nova_memory_statistics()->frame_resets==1,
                    "Frame-Arena wird atomar und ohne Einzelfreigaben geleert");
    _Alignas(64) static uint8_t tracked_static[128];
    failed |= check(nova_memory_track_static(NOVA_MEMORY_PERMANENT,tracked_static,
                    sizeof(tracked_static),0x404u,64)&&
                    nova_memory_object(tracked_static)->state==NOVA_MEMORY_OBJECT_STATIC&&
                    !nova_memory_release(tracked_static),
                    "Statische Permanent-Ressource wird zentral bilanziert");
    failed |= check(!nova_memory_allocate(NOVA_MEMORY_CACHE,600u*1024u,0x505u,64)&&
                    nova_memory_pool_statistics(NOVA_MEMORY_CACHE)->overflows==1&&
                    nova_memory_statistics()->recovery_requests==1&&
                    nova_recovery_safe_mode()&&nova_recovery_continue_boot(),
                    "Pool-Overflow aktiviert Recovery statt Speicher zu ueberschreiben");
    uint64_t area_budget_sum=0;
    failed|=check(nova_memory_budget_configure(NOVA_MEMORY_PROFILE_MINIMAL)&&
        nova_memory_budget_status()->total_budget==32ull*1024u*1024u&&
        nova_memory_budget_status()->profile==NOVA_MEMORY_PROFILE_MINIMAL&&
        nova_memory_budget_configure(NOVA_MEMORY_PROFILE_STANDARD)&&
        nova_memory_budget_status()->total_budget==64ull*1024u*1024u&&
        nova_memory_budget_configure(NOVA_MEMORY_PROFILE_COMFORT)&&
        nova_memory_budget_status()->total_budget==128ull*1024u*1024u&&
        !nova_memory_budget_configure((nova_memory_profile_t)48)&&
        nova_memory_budget_configure(NOVA_MEMORY_PROFILE_STANDARD),
        "Deterministische 32/64/128-MiB-Speicherprofile");
    for(uint8_t area=0;area<NOVA_MEMORY_AREA_COUNT;++area)
        area_budget_sum+=nova_memory_budget_status()->area_budget[area];
    failed|=check(area_budget_sum==nova_memory_budget_status()->total_budget&&
        nova_memory_budget_report(NOVA_MEMORY_AREA_GLYPH_CACHE,4ull*1024u*1024u)&&
        !nova_memory_budget_report(NOVA_MEMORY_AREA_GLYPH_CACHE,4ull*1024u*1024u+1)&&
        nova_memory_budget_status()->budget_overruns==1,
        "Alle Speicherbereiche besitzen feste Budgets und Ueberlaufdiagnose");
    for(uint8_t step=0;step<5;++step)(void)nova_memory_budget_apply_pressure();
    failed|=check(nova_memory_budget_status()->overload_step==5&&
        nova_memory_budget_status()->pressure_events==5&&
        !nova_memory_budget_apply_pressure(),
        "Fuenfstufiges deterministisches Verhalten bei Speicherdruck");
    uint8_t secret[8]={1,2,3,4,5,6,7,8};nova_memory_secure_zero(secret,sizeof(secret));
    bool secret_erased=true;for(uint8_t i=0;i<sizeof(secret);++i)secret_erased&=secret[i]==0;
    failed|=check(secret_erased&&nova_memory_budget_set_runtime(true)&&
        !nova_memory_allocate(NOVA_MEMORY_RUNTIME,16,0x606u,16)&&
        nova_memory_allocate(NOVA_MEMORY_FRAME,16,0x606u,16)!=0&&
        nova_memory_budget_set_runtime(false),
        "Sichere Loeschung und Heap-Sperre waehrend der regulaeren UI-Laufzeit");
    failed|=check(nova_memory_budget_reset()&&
        nova_memory_budget_status()->cache_evictions==0&&
        nova_memory_budget_available(1024),
        "Speicherbudget und Diagnosezustand lassen sich definiert zuruecksetzen");
    nova_configuration_initialize();
    const nova_boot_configuration_t *configuration=nova_configuration_get();
    failed |= check(configuration->theme==NOVA_THEME_DARK&&configuration->tooltips&&
                    configuration->tooltip_delay_ms==750&&
                    nova_configuration_validate(configuration)&&
                    configuration->checksum==nova_configuration_checksum(configuration),
                    "Versionierte sichere Konfigurationsdefaults");
    failed |= check(nova_configuration_begin()&&
                    nova_configuration_set(NOVA_CONFIG_THEME,NOVA_THEME_LIGHT)&&
                    nova_configuration_set(NOVA_CONFIG_TOOLTIP_DELAY,1000)&&
                    !nova_configuration_begin()&&nova_configuration_commit()&&
                    nova_configuration_get()->theme==NOVA_THEME_LIGHT&&
                    nova_configuration_get()->tooltip_delay_ms==1000&&
                    nova_configuration_diagnostics()->generation==2,
                    "Atomarer Konfigurationscommit und Benachrichtigung");
    failed |= check(nova_configuration_begin()&&
                    nova_configuration_set(NOVA_CONFIG_TOOLTIP_DELAY,333)&&
                    !nova_configuration_commit()&&
                    nova_configuration_get()->tooltip_delay_ms==1000&&
                    nova_configuration_diagnostics()->validation_errors==1,
                    "Ungueltige Transaktion wird vollstaendig verworfen");
    failed |= check(nova_configuration_begin()&&
                    nova_configuration_set(NOVA_CONFIG_TOOLTIPS,0)&&
                    nova_configuration_rollback()&&nova_configuration_get()->tooltips,
                    "Expliziter Konfigurationsrollback");
    failed |= check(nova_configuration_override(NOVA_CONFIG_SAFE_MODE,1)&&
                    nova_configuration_effective()->safe_mode&&
                    !nova_configuration_get()->safe_mode&&
                    nova_configuration_clear_overrides()&&
                    !nova_configuration_effective()->safe_mode,
                    "Temporaerer Runtime-Override bleibt von gespeicherten Werten getrennt");
    failed |= check(nova_configuration_reset_defaults()&&
                    nova_configuration_get()->theme==NOVA_THEME_DARK&&
                    nova_configuration_get()->tooltip_delay_ms==750&&
                    nova_configuration_diagnostics()->resets==1,
                    "Konfiguration sicher auf Defaults zuruecksetzen");
    nova_boot_configuration_t corrupt=*nova_configuration_get();
    corrupt.theme=NOVA_THEME_LIGHT;
    failed |= check(!nova_configuration_validate(&corrupt),
                    "Beschaedigte Runtime-Pruefsumme erkennen");
    nova_runtime_create();
    failed |= check(!nova_runtime_run()&&nova_runtime_begin_initialization()&&
                    !nova_runtime_subsystem_ready(NOVA_RUNTIME_GRAPHICS)&&
                    nova_runtime_subsystem_ready(NOVA_RUNTIME_MEMORY)&&
                    nova_runtime_subsystem_ready(NOVA_RUNTIME_PLATFORM)&&
                    nova_runtime_subsystem_ready(NOVA_RUNTIME_GRAPHICS)&&
                    nova_runtime_subsystem_ready(NOVA_RUNTIME_DIAGNOSTICS)&&
                    nova_runtime_subsystem_ready(NOVA_RUNTIME_CONFIGURATION)&&
                    nova_runtime_loading()&&
                    nova_runtime_subsystem_ready(NOVA_RUNTIME_RESOURCES)&&
                    nova_runtime_building_scene()&&
                    nova_runtime_subsystem_ready(NOVA_RUNTIME_SCENE)&&
                    nova_runtime_layout()&&
                    nova_runtime_subsystem_ready(NOVA_RUNTIME_LAYOUT_ENGINE)&&
                    nova_runtime_subsystem_ready(NOVA_RUNTIME_MOTION)&&
                    nova_runtime_subsystem_ready(NOVA_RUNTIME_INPUT)&&
                    nova_runtime_subsystem_ready(NOVA_RUNTIME_RENDERER)&&
                    nova_runtime_ready()&&nova_runtime_run()&&
                    nova_runtime_input_allowed(),
                    "Runtime initialisiert Subsysteme und Lifecycle strikt geordnet");
    failed |= check(nova_runtime_frame_begin()&&
                    nova_runtime_frame_step(NOVA_FRAME_INPUT)&&
                    !nova_runtime_frame_step(NOVA_FRAME_CONTROL_UPDATE)&&
                    nova_runtime_frame_abort(),
                    "Frame-Pipeline weist uebersprungene Phase ab");
    failed |= check(nova_runtime_frame_begin(),"Vollstaendigen Runtime-Frame beginnen");
    for(uint8_t stage=0;stage<NOVA_FRAME_STAGE_COUNT;++stage)
        failed|=check(nova_runtime_frame_step((nova_runtime_frame_stage_t)stage),
                      "Runtime-Framephase in fester Reihenfolge");
    failed|=check(nova_runtime_frame_end()&&
                  nova_runtime_diagnostics()->completed_frames==1,
                  "Runtime-Frame atomar abschliessen");
    runtime_task_count=0;
    nova_runtime_task_t idle={1,0,0,runtime_task,0,NOVA_TASK_IDLE,true,false,false};
    nova_runtime_task_t critical={2,0,0,runtime_task,0,NOVA_TASK_CRITICAL,true,false,false};
    failed|=check(nova_runtime_schedule(&idle)&&nova_runtime_schedule(&critical),
                  "Priorisierte Runtime-Aufgaben registrieren");
    nova_runtime_tick(1);
    failed|=check(runtime_task_count==2&&runtime_task_order[0]==2&&runtime_task_order[1]==1,
                  "Scheduler fuehrt Critical vor Idle deterministisch aus");
    state_callback_count=0;
    nova_state_object_t *scene=nova_state_create(100,NOVA_STATE_DOMAIN_SCENE,0,1,1);
    failed|=check(scene&&nova_state_set_callbacks(scene,state_callback,state_callback,
                  state_callback,0)&&nova_state_transition(scene,1)&&
                  nova_state_checkpoint(scene)&&nova_state_transition(scene,2)&&
                  nova_state_rollback(scene)&&scene->current==1&&
                  nova_state_transition(scene,2)&&nova_state_transition(scene,3)&&
                  nova_state_transition(scene,4),
                  "Hierarchischer Scene-Automat mit Events und Rollback");
    nova_state_object_t *state_control=nova_state_create(101,NOVA_STATE_DOMAIN_CONTROL,0,1,100);
    failed|=check(state_control&&nova_state_transition(state_control,1)&&
                  nova_state_transition(state_control,2)&&nova_state_transition(state_control,3)&&
                  !nova_state_transition(state_control,5)&&nova_state_transition(state_control,4)&&
                  nova_state_transition(state_control,5)&&nova_state_transition(state_control,6)&&
                  nova_state_transition(state_control,4),
                  "Control-Automat verwirft Spruenge und folgt Press/Release");
    nova_state_snapshot_t snapshot;
    failed|=check(nova_state_serialize(state_control,&snapshot),
                  "Versionierten State-Snapshot serialisieren");
    failed|=check(nova_state_transition(state_control,7),
                  "Control vor Snapshot-Restore veraendern");
    failed|=check(nova_state_deserialize(state_control,&snapshot)&&state_control->current==4,
                  "Versionierten State-Snapshot wiederherstellen");
    nova_state_snapshot_t corrupt_snapshot=snapshot;corrupt_snapshot.current=15;
    failed|=check(!nova_state_deserialize(state_control,&corrupt_snapshot)&&
                  state_callback_count>=15&&nova_state_diagnostics()->invalid_transitions>=1&&
                  nova_state_event(0)!=0,
                  "Korrupte Snapshots, Transitionsevents und Diagnostik");
    failed|=check(nova_state_transition(state_control,7)&&
                  nova_state_transition(state_control,2)&&
                  nova_state_transition(state_control,8)&&nova_state_destroy(state_control)&&
                  nova_state_transition(scene,5)&&nova_state_transition(scene,6)&&
                  nova_state_destroy(scene),
                  "Kind vor Elternteil deterministisch zerstoeren");
    failed|=check(nova_runtime_suspend()&&!nova_runtime_input_allowed()&&
                  nova_runtime_resume()&&nova_runtime_input_allowed()&&
                  nova_runtime_enter_recovery()&&!nova_runtime_input_allowed()&&
                  nova_runtime_leave_recovery(true)&&nova_runtime_input_allowed()&&
                  nova_runtime_shutdown()&&nova_runtime_destroy()&&
                  nova_runtime_state()==NOVA_RUNTIME_DESTROYED&&!nova_runtime_run(),
                  "Suspend, Recovery, Shutdown und Destroy Lifecycle");
    nova_page_model_initialize();
    nova_page_t *main_page=nova_page_create(1,"Bootmanager",11,false);
    nova_view_t *root_view=nova_view_create(main_page,100,NOVA_VIEW_ROOT,"Bootmanager",1,false);
    nova_view_t *header_view=nova_view_create(main_page,101,NOVA_VIEW_HEADER,"Kopfbereich",2,false);
    nova_view_t *content_view=nova_view_create(main_page,102,NOVA_VIEW_CONTENT,"Startauswahl",3,true);
    nova_view_t *footer_view=nova_view_create(main_page,103,NOVA_VIEW_FOOTER,"Status",4,false);
    failed |= check(main_page&&root_view&&header_view&&content_view&&footer_view&&
                    nova_page_set_root(main_page,root_view)&&
                    nova_view_add_child(root_view,header_view)&&
                    nova_view_add_child(root_view,content_view)&&
                    nova_view_add_child(root_view,footer_view),
                    "Page mit eindeutigem Root-View-Baum");
    failed |= check(!nova_view_add_child(content_view,root_view),"View-Zyklus abweisen");
    failed |= check(nova_view_initialize(root_view)&&nova_view_initialize(header_view)&&
                    nova_view_initialize(content_view)&&nova_view_initialize(footer_view)&&
                    nova_view_set_visibility(root_view,NOVA_VISIBILITY_VISIBLE)&&
                    nova_view_set_visibility(header_view,NOVA_VISIBILITY_VISIBLE)&&
                    nova_view_set_visibility(content_view,NOVA_VISIBILITY_VISIBLE)&&
                    nova_view_set_visibility(footer_view,NOVA_VISIBILITY_VISIBLE)&&
                    nova_page_load(main_page)&&nova_page_activate(main_page)&&
                    nova_page_set_focus(main_page,content_view),
                    "deterministische Page-/View-Lebenszyklen und Fokus");
    uint32_t page_visits=0;
    failed |= check(nova_page_dispatch(main_page,77,page_event_handler,&page_visits)&&page_visits>=2,
                    "iterative Eventweiterleitung durch View Tree");
    nova_page_t *dialog_page=nova_page_create(2,"Dialog",22,true);
    nova_view_t *dialog_root=nova_view_create(dialog_page,200,NOVA_VIEW_ROOT,"Dialog",5,false);
    nova_view_t *dialog_content=nova_view_create(dialog_page,201,NOVA_VIEW_DIALOG_CONTENT,"Dialoginhalt",6,true);
    failed |= check(dialog_page&&dialog_root&&dialog_content&&nova_page_set_root(dialog_page,dialog_root)&&
                    nova_view_add_child(dialog_root,dialog_content)&&nova_view_initialize(dialog_root)&&
                    nova_view_initialize(dialog_content)&&nova_view_set_visibility(dialog_root,NOVA_VISIBILITY_VISIBLE)&&
                    nova_view_set_visibility(dialog_content,NOVA_VISIBILITY_VISIBLE)&&nova_page_load(dialog_page)&&
                    nova_page_activate(dialog_page)&&main_page->state==NOVA_PAGE_SUSPENDED&&
                    nova_page_set_focus(dialog_page,dialog_content)&&nova_page_activate(main_page)&&
                    dialog_page->state==NOVA_PAGE_SUSPENDED&&nova_page_close(dialog_page),
                    "Dialog als eigene Page und Wiederaktivierung");
    nova_navigation_entry_t root = {0,2,17,48,0x1234};
    nova_navigation_initialize(root);
    failed |= check(!nova_navigation_can_go_back() &&
                    nova_navigation_current()->selection == 2,
                    "Navigation Root State");
    failed |= check(nova_navigation_push((nova_navigation_entry_t){1,0,22,0,9},
                                         NOVA_NAV_PUSH) &&
                    nova_navigation_diagnostics()->transition_running,
                    "Navigation Push mit Transition");
    nova_navigation_transition_complete();
    failed |= check(nova_navigation_update(4,26,72,10),
                    "Navigation State aktualisieren");
    nova_navigation_entry_t restored;
    failed |= check(nova_navigation_back(&restored) && restored.page == 0 &&
                    restored.selection == 2 && restored.focus_id == 17 &&
                    restored.scroll == 48 && restored.context == 0x1234,
                    "Back stellt Seite Fokus Scroll und Kontext wieder her");
    nova_navigation_transition_complete();
    failed |= check(!nova_navigation_back(&restored), "Back unter Root abweisen");
    failed |= check(nova_navigation_push((nova_navigation_entry_t){3,2,9,44,7},NOVA_NAV_PUSH),
                    "Navigation vor Reset erweitern");
    nova_navigation_transition_complete();
    failed |= check(nova_navigation_reset()&&!nova_navigation_can_go_back()&&
                    nova_navigation_current()->page==0&&nova_navigation_diagnostics()->resets==1,
                    "Navigation Reset behaelt ausschliesslich Root");

    nova_dialog_initialize();
    nova_dialog_t *confirm = nova_dialog_open(NOVA_DIALOG_CONFIRMATION,
        "Ausschalten", "NovaOS jetzt sicher herunterfahren?", true, true, 17);
    failed |= check(confirm && confirm->state == NOVA_DIALOG_ACTIVE &&
                    nova_dialog_diagnostics()->modal_active,
                    "modaler Dialog mit vollstaendigem Lifecycle");
    failed |= check(nova_dialog_add_button(confirm,"Abbrechen",NOVA_DIALOG_RESULT_CANCEL,false) &&
                    nova_dialog_add_button(confirm,"Ausschalten",NOVA_DIALOG_RESULT_YES,true),
                    "Dialogaktionen anlegen");
    failed |= check(nova_dialog_focus_move(1) && confirm->focus == 1,
                    "Dialog Fokusfang");
    nova_dialog_result_t dialog_result = NOVA_DIALOG_RESULT_NONE;
    failed |= check(!nova_dialog_activate(&dialog_result) &&
                    confirm->destructive_armed == 1 &&
                    dialog_result == NOVA_DIALOG_RESULT_NONE,
                    "zerstoerende Aktion verlangt zweite Bestaetigung");
    failed |= check(nova_dialog_activate(&dialog_result) &&
                    dialog_result == NOVA_DIALOG_RESULT_YES &&
                    !nova_dialog_diagnostics()->modal_active,
                    "Dialog liefert genau ein Ergebnis");
    nova_dialog_t *warning = nova_dialog_open(NOVA_DIALOG_WARNING,
        "Nicht verfuegbar", "Backend fehlt", true, true, 22);
    nova_dialog_add_button(warning,"OK",NOVA_DIALOG_RESULT_OK,false);
    failed |= check(nova_dialog_cancel(&dialog_result) &&
                    dialog_result == NOVA_DIALOG_RESULT_CANCEL,
                    "Escape-Abbruch schliesst obersten Dialog");
    nova_dialog_t *progress_dialog = nova_dialog_open(NOVA_DIALOG_PROGRESS,
        "Diagnose", "Komponenten werden geprüft.", true, false, 22);
    failed |= check(progress_dialog &&
                    nova_dialog_progress_update(progress_dialog,-10,false,"Start") == false &&
                    progress_dialog->progress_per_mille == 0 &&
                    nova_dialog_progress_update(progress_dialog,1000,false,"Abgeschlossen") &&
                    progress_dialog->progress_per_mille == 1000,
                    "Progress wird begrenzt und aktualisiert");
    nova_control_t *activity=nova_control_create(NOVA_CONTROL_SPINNER);
    failed |= check(activity&&nova_control_set_state(activity,NOVA_CONTROL_INITIALIZED)&&
        nova_control_set_state(activity,NOVA_CONTROL_VISIBLE)&&
        nova_activity_start(activity)&&nova_activity_running(activity),"Activity Indicator startet");
    for(uint8_t style=0;style<4;++style)
        failed |= check(nova_activity_set_style(activity,(nova_activity_style_t)style)&&
            nova_activity_set_phase(activity,(uint16_t)(style*250)),"Activity-Stil und Phase");
    failed |= check(!nova_activity_set_style(activity,(nova_activity_style_t)4)&&
        !nova_activity_set_phase(activity,1001)&&nova_activity_stop(activity)&&
        !nova_activity_running(activity),"Activity-Validierung und Stop");
    nova_control_t *scroll_view=nova_control_create(NOVA_CONTROL_SCROLL_VIEW);
    nova_control_t *vertical_bar=nova_control_create(NOVA_CONTROL_SCROLLBAR);
    nova_control_t *horizontal_bar=nova_control_create(NOVA_CONTROL_SCROLLBAR);
    failed |= check(scroll_view&&vertical_bar&&horizontal_bar&&
        nova_scroll_view_configure(scroll_view,200,100,500,400)&&
        nova_scrollbar_attach(vertical_bar,scroll_view,NOVA_SCROLLBAR_VERTICAL)&&
        nova_scrollbar_attach(horizontal_bar,scroll_view,NOVA_SCROLLBAR_HORIZONTAL),
        "ScrollView und Scrollbars konfigurieren");
    failed |= check(nova_scroll_view_scroll_to(scroll_view,999,999)&&
        scroll_view->scroll_x==300&&scroll_view->scroll_y==300&&
        vertical_bar->value==300&&horizontal_bar->value==300,
        "Scrollposition und gekoppelte Scrollbars begrenzen");
    nova_rect_t scroll_child={40,50,20,20};
    failed |= check(nova_scroll_view_scroll_into_view(scroll_view,&scroll_child)&&
        scroll_view->scroll_x==40&&scroll_view->scroll_y==50,
        "Kind-Control automatisch sichtbar machen");
    failed |= check(!nova_scroll_view_configure(scroll_view,200,100,199,400)&&
        !nova_scrollbar_attach(vertical_bar,scroll_view,NOVA_SCROLLBAR_HORIZONTAL),
        "ungültige Scrollkonfiguration und Mehrfachbindung ablehnen");
    nova_control_t *card=nova_control_create(NOVA_CONTROL_CARD);
    nova_control_t *card_child=nova_control_create(NOVA_CONTROL_LABEL);
    failed |= check(card&&card_child&&nova_card_add_child(card,card_child),
        "Glass Card gruppiert Kind-Control");
    for(uint8_t type=0;type<=NOVA_CARD_CUSTOM;++type)
        failed|=check(nova_card_set_type(card,(nova_card_type_t)type),"Glass-Card-Typen");
    failed|=check(!nova_card_set_type(card,(nova_card_type_t)(NOVA_CARD_CUSTOM+1))&&
        !nova_card_add_child(card,card),"Glass Card validiert Typ und verhindert Rekursion");
    nova_control_t *tile=nova_control_create(NOVA_CONTROL_TILE);
    failed|=check(tile&&nova_control_set_text(tile,"Recovery")&&
        nova_tile_set_description(tile,"System sicher reparieren")&&
        nova_tile_set_status(tile,"Backend fehlt"),"Boot Option Tile Metadaten");
    for(uint8_t type=0;type<=NOVA_TILE_CUSTOM;++type)
        failed|=check(nova_tile_set_type(tile,(nova_boot_tile_type_t)type),"Boot Option Tile Typen");
    failed|=check(!nova_tile_set_type(tile,(nova_boot_tile_type_t)(NOVA_TILE_CUSTOM+1)),
        "Boot Option Tile validiert Typ");
    nova_control_t *icon_button=nova_control_create(NOVA_CONTROL_ICON_BUTTON);
    failed|=check(icon_button&&nova_icon_button_set_icon(icon_button,NOVA_ICON_HOME)&&
        nova_icon_button_set_tooltip(icon_button,"Zum Hauptmenü")&&
        nova_icon_button_set_action(icon_button,77)&&
        nova_control_set_accessibility(icon_button,4,"Zum Hauptmenü",false),
        "Icon Button Icon, Tooltip, Aktion und Accessibility");
    failed|=check(!nova_icon_button_set_icon(icon_button,NOVA_ICON_COUNT)&&
        !nova_icon_button_set_tooltip(icon_button,""),"Icon Button validiert Icon und Tooltip");
    nova_control_t *button=nova_control_create(NOVA_CONTROL_BUTTON);
    failed|=check(button&&nova_control_set_state(button,NOVA_CONTROL_INITIALIZED)&&
        nova_control_set_state(button,NOVA_CONTROL_VISIBLE)&&
        nova_button_set_action(button,91),"Button initialisieren und Aktion setzen");
    for(uint8_t type=0;type<=NOVA_BUTTON_TOGGLE;++type)
        failed|=check(nova_button_set_type(button,(nova_button_type_t)type),"Button-Typen");
    uint32_t button_action=0;
    failed|=check(nova_control_invoke(button,&button_action)&&button_action==91&&
        (button->flags&NOVA_CONTROL_FLAG_CHECKED)&&
        !nova_control_invoke(button,&button_action),"Toggle-Button genau einmal aktivieren");
    nova_control_release(button);button->flags|=NOVA_CONTROL_FLAG_BUSY;
    failed|=check(!nova_control_invoke(button,&button_action)&&
        !nova_button_set_type(button,(nova_button_type_t)(NOVA_BUTTON_TOGGLE+1)),
        "Busy- und Typvalidierung des Buttons");
    nova_control_t *bound_menu=nova_control_create(NOVA_CONTROL_CONTEXT_MENU);
    nova_control_t *menu_button=nova_control_create(NOVA_CONTROL_MENU_BUTTON);
    failed|=check(bound_menu&&menu_button&&
        nova_control_set_state(bound_menu,NOVA_CONTROL_INITIALIZED)&&
        nova_control_set_state(bound_menu,NOVA_CONTROL_VISIBLE)&&
        nova_control_set_state(menu_button,NOVA_CONTROL_INITIALIZED)&&
        nova_control_set_state(menu_button,NOVA_CONTROL_VISIBLE)&&
        nova_menu_button_bind(menu_button,bound_menu)&&
        nova_menu_button_open(menu_button)&&nova_menu_button_expanded(menu_button)&&
        !nova_menu_button_open(menu_button)&&nova_menu_button_close(menu_button)&&
        !nova_menu_button_expanded(menu_button),"Menu Button Bindung und Expanded-Zustand");
    failed|=check(!nova_menu_button_bind(menu_button,button)&&
        !nova_menu_button_open(0)&&!nova_menu_button_expanded(0),
        "Menu Button Fehlerfälle");
    nova_control_t *label=nova_control_create(NOVA_CONTROL_LABEL);
    failed|=check(label&&nova_control_set_state(label,NOVA_CONTROL_INITIALIZED)&&
        nova_control_set_state(label,NOVA_CONTROL_VISIBLE)&&nova_control_set_text(label,"Status bereit")&&
        nova_label_set_alignment(label,NOVA_ALIGN_RIGHT_BOTTOM)&&
        nova_label_set_scale(label,1500)&&nova_label_get_text(label)&&
        !nova_label_set_alignment(label,(nova_control_alignment_t)9)&&
        !nova_label_set_scale(label,499),"Label Text, Ausrichtung und DPI-Skalierung");
    for(uint8_t type=0;type<=NOVA_LABEL_INFORMATION;++type)
        failed|=check(nova_label_set_type(label,(nova_label_type_t)type),"Label-Typen");
    nova_control_t *icon_control=nova_control_create(NOVA_CONTROL_ICON);
    failed|=check(icon_control&&nova_control_set_state(icon_control,NOVA_CONTROL_INITIALIZED)&&
        nova_control_set_state(icon_control,NOVA_CONTROL_VISIBLE)&&
        nova_icon_control_set(icon_control,NOVA_ICON_SUCCESS)&&
        nova_icon_control_get(icon_control)==NOVA_ICON_SUCCESS&&
        !nova_icon_control_set(icon_control,NOVA_ICON_COUNT),"Icon-Control und Tokenvalidierung");
    nova_control_t *image_control=nova_control_create(NOVA_CONTROL_IMAGE);
    failed|=check(image_control&&nova_control_set_state(image_control,NOVA_CONTROL_INITIALIZED)&&
        nova_control_set_state(image_control,NOVA_CONTROL_VISIBLE)&&
        nova_image_set_resource(image_control,NOVA_IMAGE_BRANDING_LOGO)&&
        nova_image_set_tint(image_control,0xff218bd1u),"Image-Control und zentrale Ressource");
    for(uint8_t mode=0;mode<=NOVA_IMAGE_CENTER;++mode)
        failed|=check(nova_image_set_scaling(image_control,(nova_image_scaling_mode_t)mode),
                      "Image-Skalierungsmodi");
    failed|=check(!nova_image_set_scaling(image_control,(nova_image_scaling_mode_t)5),
                  "Image-Modusvalidierung");
    nova_control_t *separator=nova_control_create(NOVA_CONTROL_SEPARATOR);
    failed|=check(separator&&nova_control_set_state(separator,NOVA_CONTROL_INITIALIZED)&&
        nova_control_set_state(separator,NOVA_CONTROL_VISIBLE)&&
        nova_separator_set_orientation(separator,NOVA_SEPARATOR_HORIZONTAL)&&
        nova_separator_set_title(separator,"System")&&
        nova_separator_set_orientation(separator,NOVA_SEPARATOR_VERTICAL)&&
        !(separator->flags&NOVA_CONTROL_FLAG_ENABLED)&&
        !nova_separator_set_orientation(separator,(nova_separator_orientation_t)2),
        "Separator Ausrichtungen, Titel und Interaktionsfreiheit");
    nova_control_t *badge=nova_control_create(NOVA_CONTROL_STATUS_BADGE);
    failed|=check(badge&&nova_control_set_state(badge,NOVA_CONTROL_INITIALIZED)&&
        nova_control_set_state(badge,NOVA_CONTROL_VISIBLE)&&
        nova_control_set_text(badge,"Bereit")&&
        nova_status_badge_set_icon(badge,NOVA_ICON_SUCCESS)&&
        nova_status_badge_set_visible(badge,false)&&
        nova_status_badge_set_visible(badge,true)&&
        !(badge->flags&NOVA_CONTROL_FLAG_ENABLED),"StatusBadge Inhalt und Interaktionsfreiheit");
    for(uint8_t type=0;type<=NOVA_BADGE_CUSTOM;++type)
        failed|=check(nova_status_badge_set_type(badge,(nova_badge_type_t)type),"StatusBadge-Typen");
    failed|=check(!nova_status_badge_set_type(badge,(nova_badge_type_t)7)&&
        !nova_status_badge_set_icon(badge,NOVA_ICON_COUNT),"StatusBadge-Validierung");
    nova_control_style_t inherited_style=badge->style;
    inherited_style.accent=0xff123456u;inherited_style.corner_dlu=12;
    failed|=check(nova_style_define(10,0,&inherited_style,
        NOVA_STYLE_ACCENT|NOVA_STYLE_GEOMETRY,NOVA_CONTROL_MATERIAL_GLASS)&&
        nova_style_apply(badge,10)&&badge->style.accent==0xff123456u&&
        badge->style.foreground==nova_style_get(0)->resolved.foreground&&
        nova_style_get(10)->material==NOVA_CONTROL_MATERIAL_GLASS,
        "Style-Vererbung, Material und O(1)-Bindung");
    nova_style_theme_changed();
    failed|=check(badge->style_id==10&&badge->style.corner_dlu==12&&
        !nova_style_define(11,11,&inherited_style,NOVA_STYLE_ALL,NOVA_CONTROL_MATERIAL_OPAQUE),
        "Style-Neubindung und Rekursionsschutz");
    uint16_t badge_template=(uint16_t)(NOVA_CONTROL_STATUS_BADGE+1);
    failed|=check(nova_control_template_define(40,badge_template,
        NOVA_CONTROL_STATUS_BADGE,NOVA_TEMPLATE_PART_STATUS,
        NOVA_TEMPLATE_PART_BACKGROUND|NOVA_TEMPLATE_PART_TEXT,5)&&
        nova_control_template_apply(badge,40)&&
        nova_control_template_has_part(nova_control_template_get(40),"Background")&&
        nova_control_template_has_part(nova_control_template_get(40),"Status")&&
        !nova_control_template_has_part(nova_control_template_get(40),"Thumb"),
        "Template-Vererbung, Pflicht-Parts und Bindung");
    failed|=check(!nova_control_template_define(41,41,NOVA_CONTROL_STATUS_BADGE,
        NOVA_TEMPLATE_PART_TEXT,NOVA_TEMPLATE_PART_TEXT,1)&&
        !nova_control_template_apply(label,40),"Template-Rekursion und Typkompatibilitaet");
    nova_control_t *typed_list=nova_control_create(NOVA_CONTROL_LIST);
    nova_control_t *typed_items[3];
    failed|=check(typed_list&&nova_control_set_state(typed_list,NOVA_CONTROL_INITIALIZED)&&
        nova_control_set_state(typed_list,NOVA_CONTROL_VISIBLE),"List-Control initialisieren");
    for(uint8_t i=0;i<3;++i){
        typed_items[i]=nova_control_create(NOVA_CONTROL_LIST_ITEM);
        failed|=check(typed_items[i]&&nova_control_set_state(typed_items[i],NOVA_CONTROL_INITIALIZED)&&
            nova_control_set_state(typed_items[i],NOVA_CONTROL_VISIBLE)&&
            nova_list_item_set_subtitle(typed_items[i],"Beschreibung")&&
            nova_list_item_set_status(typed_items[i],"Bereit")&&
            nova_list_add_item(typed_list,typed_items[i]),"List Item einfügen");
    }
    failed|=check(nova_list_count(typed_list)==3&&nova_list_select(typed_list,1)&&
        nova_list_selected_index(typed_list)==1&&
        (typed_items[1]->flags&NOVA_CONTROL_FLAG_SELECTED),"O(1)-Einzelauswahl");
    failed|=check(nova_list_set_selection_mode(typed_list,NOVA_LIST_SELECTION_MULTIPLE)&&
        nova_list_select(typed_list,2)&&(typed_items[1]->flags&NOVA_CONTROL_FLAG_SELECTED)&&
        (typed_items[2]->flags&NOVA_CONTROL_FLAG_SELECTED),"Mehrfachauswahl");
    failed|=check(nova_list_set_virtual_window(typed_list,1,2)&&
        !nova_list_item_visible(typed_list,0)&&nova_list_item_visible(typed_list,1)&&
        nova_list_item_visible(typed_list,2),"List-Virtualisierungsfenster");
    failed|=check(nova_list_remove_item(typed_list,1)&&nova_list_count(typed_list)==2&&
        !nova_list_remove_item(typed_list,5)&&
        nova_list_item_set_state(typed_items[0],NOVA_LIST_ITEM_ERROR)&&
        (typed_items[0]->flags&NOVA_CONTROL_FLAG_ERROR),"Entfernung und List-Item-Zustände");
    nova_dialog_add_button(progress_dialog,"Schließen",NOVA_DIALOG_RESULT_OK,false);
    failed |= check(nova_dialog_activate(&dialog_result),"Progressdialog abschliessen");
    nova_dialog_t *credential = nova_dialog_open(NOVA_DIALOG_CREDENTIAL,
        "Volume entsperren", "Passphrase", true, true, 22);
    failed |= check(credential &&
                    nova_dialog_credential_configure(credential,NOVA_CREDENTIAL_PASSPHRASE) &&
                    nova_dialog_credential_input(credential,'N') &&
                    nova_dialog_credential_input(credential,0x00e4) &&
                    credential->credential_glyphs == 2 && credential->credential_bytes == 3 &&
                    nova_dialog_credential_backspace(credential) &&
                    credential->credential_glyphs == 1,
                    "maskierte UTF-8-Credential-Eingabe");
    nova_dialog_add_button(credential,"Abbrechen",NOVA_DIALOG_RESULT_CANCEL,false);
    failed |= check(nova_dialog_cancel(&dialog_result) && credential->credential_bytes == 0 &&
                    credential->credential[0] == 0,
                    "Credential-Puffer beim Schliessen sicher geloescht");
    nova_layout_initialize();
    nova_resolution_initialize();
    failed|=check(nova_resolution_configure(1920,1080,96,false)&&
                  nova_resolution_viewport()->scale_milli==1000&&
                  nova_resolution_viewport()->resolution_class==NOVA_RESOLUTION_XL&&
                  nova_resolution_viewport()->aspect_ratio==NOVA_ASPECT_16_9&&
                  nova_resolution_viewport()->logical_width_dlu==1920,
                  "Full-HD-Referenz verwendet 100 Prozent und DLU-Viewport");
    const uint32_t resolutions[][2] = {
        {640,480},{800,600},{1024,768},{1280,720},{1280,800},
        {1366,768},{1600,900},{1920,1080},{2560,1440},{3840,2160}
    };
    for (uint32_t i = 0; i < sizeof(resolutions)/sizeof(resolutions[0]); ++i) {
        nova_boot_layout_t layout;
        failed |= check(nova_layout_compute(resolutions[i][0], resolutions[i][1],
                                            false, &layout),
                        "responsive Safe-Area-Berechnung");
        failed |= check(layout.panel.x >= layout.safe.x &&
                        layout.panel.y >= layout.safe.y &&
                        layout.panel.x + layout.panel.width <= layout.safe.x + layout.safe.width &&
                        layout.status.y + layout.status.height <= layout.safe.y + layout.safe.height,
                        "interaktive Flächen innerhalb der Safe Area");
    }
    failed|=check(nova_resolution_viewport()->resolution_class==NOVA_RESOLUTION_ULTRA&&
                  nova_resolution_viewport()->scale_milli==2000&&
                  nova_resolution_viewport()->logical_width_dlu==1920&&
                  nova_resolution_viewport()->safe_pixels.width<3840,
                  "UHD skaliert automatisch auf 200 Prozent mit Safe Area");
    failed|=check(nova_resolution_set_resolution(3440,1440)&&
                  nova_resolution_viewport()->aspect_ratio==NOVA_ASPECT_21_9&&
                  nova_resolution_viewport()->resolution_class==NOVA_RESOLUTION_XXL,
                  "UltraWide-Seitenverhaeltnis und Aufloesungsklasse");
    failed|=check(nova_resolution_set_scale(1750)&&
                  nova_logical_to_pixel(10)==18&&nova_pixel_to_logical(18)==10&&
                  !nova_resolution_set_scale(999)&&nova_resolution_set_automatic(),
                  "Benutzerskalierung, Pixelalignment und Rueckkehr zu Automatik");
    failed|=check(nova_resolution_set_resolution(1920,1080)&&
                  nova_resolution_set_dpi(144,true)&&
                  nova_resolution_viewport()->scale_milli==1500&&
                  nova_resolution_set_dpi(0,true)&&
                  !nova_resolution_viewport()->dpi_reliable,
                  "Verlaessliche DPI und sicherer DPI-Fallback");
    failed |= check(nova_dlu_to_pixels(10,1250)==13 &&
                    nova_dlu_to_pixels(-10,1250)==-13,
                    "deterministische DLU-Rundung");
    nova_size_constraints_t valid_constraints = {40,30,100,80,200,160};
    uint32_t constrained_width=0, constrained_height=0;
    failed |= check(nova_constraints_resolve(&valid_constraints,90,70,
                    &constrained_width,&constrained_height) &&
                    constrained_width==90 && constrained_height==70,
                    "priorisierte Größenconstraints");
    int32_t value = 0;
    nova_motion_initialize();
    failed |= check(nova_navigation_visual_begin(NOVA_NAV_PUSH,true,0) &&
                    nova_navigation_visual()->offset_dlu == 32 &&
                    nova_navigation_diagnostics()->focus_locked,
                    "Navigation Enter sperrt Fokus und startet bei 32 DLU");
    nova_motion_update(90);
    failed |= check(nova_navigation_visual()->offset_dlu > 0 &&
                    nova_navigation_visual()->offset_dlu < 32 &&
                    nova_navigation_visual()->opacity > 0 &&
                    nova_navigation_visual()->opacity < 255,
                    "Slide und Fade laufen synchron");
    nova_motion_update(181);
    nova_navigation_visual_complete();
    failed |= check(nova_navigation_visual()->offset_dlu == 0 &&
                    nova_navigation_visual()->opacity == 255 &&
                    !nova_navigation_diagnostics()->focus_locked,
                    "Navigation gibt Fokus erst nach Enter frei");
    failed |= check(nova_navigation_visual_begin(NOVA_NAV_POP,true,182) &&
                    nova_navigation_visual()->offset_dlu == -32,
                    "Back Transition ist spiegelbildlich");
    nova_navigation_visual_cancel();
    failed |= check(nova_navigation_diagnostics()->visual_cancelled == 1 &&
                    !nova_navigation_diagnostics()->focus_locked,
                    "Navigation Transition unterbrechen");
    failed |= check(nova_navigation_visual_begin(NOVA_NAV_REPLACE,true,183) &&
                    nova_navigation_visual()->offset_dlu == 0,
                    "Replace verwendet Cross Fade ohne Richtung");
    nova_motion_update(364); nova_navigation_visual_complete();
    failed |= check(nova_navigation_visual_begin(NOVA_NAV_RECOVERY,true,365) &&
                    nova_navigation_visual()->offset_dlu == 0 &&
                    nova_navigation_visual()->duration_ms == 240,
                    "Recovery verwendet ruhigen Fade");
    nova_motion_update(606); nova_navigation_visual_complete();
    nova_motion_initialize();
    nova_animation_t description = {
        &value, 0, 1000, 100, 20, 200, 7, 2, 0,
        NOVA_PROPERTY_X, NOVA_EASE_LINEAR, NOVA_MOTION_CREATED,
        false, false, true, 0, 0, 0
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
    nova_motion_update(470);
    failed |= check(value == 1500, "umgeleitetes Ziel");
    failed |= check(nova_motion_diagnostics()->interruptions==1&&
                    nova_motion_diagnostics()->resumes==1&&
                    nova_motion_diagnostics()->redirects==1,
                    "Interrupt-Diagnose zählt Pause, Resume und Redirect");
    int32_t reverse_value=0;
    nova_animation_t reversible=description;
    reversible.target=&reverse_value;reversible.start_ms=500;reversible.delay_ms=0;
    nova_animation_t *reverse_animation=nova_motion_create(&reversible);
    nova_motion_update(600);
    int32_t reverse_origin=reverse_value;
    failed|=check(reverse_animation&&reverse_origin>0&&
        nova_motion_reverse(reverse_animation,600),"laufende Animation am Zwischenwert umkehren");
    nova_motion_update(710);
    failed|=check(reverse_value==0&&nova_motion_diagnostics()->reversals==1,
        "Richtungswechsel läuft ohne Sprung zum Ursprung zurück");

    nova_motion_set_reduced(true);
    failed |= check(nova_navigation_visual_begin(NOVA_NAV_POP,true,500) &&
                    nova_navigation_visual()->offset_dlu == 0 &&
                    nova_navigation_visual()->reduced_motion,
                    "Reduced Motion entfernt seitliche Navigation");
    nova_motion_update(651);
    nova_navigation_visual_complete();
    int32_t reduced_value = 0;
    nova_animation_t reduced = description;
    reduced.target = &reduced_value;
    reduced.start_ms = 500;
    reduced.delay_ms = 0;
    reduced.easing = NOVA_EASE_SPRING;
    nova_animation_t *reduced_animation = nova_motion_create(&reduced);
    failed |= check(reduced_animation && reduced_animation->easing == NOVA_EASE_OUT_CUBIC,
                    "Reduced Motion ersetzt Spring");
    failed |= check(reduced_value==1000&&
                    reduced_animation->state==NOVA_MOTION_COMPLETED&&
                    nova_motion_policy(NOVA_PROPERTY_X)==NOVA_MOTION_POLICY_REPLACED&&
                    nova_motion_policy(NOVA_PROPERTY_OPACITY)==NOVA_MOTION_POLICY_ALLOWED&&
                    nova_motion_policy(NOVA_PROPERTY_BLUR)==NOVA_MOTION_POLICY_DISABLED,
                    "Reduced-Motion-Policy ersetzt Bewegung und erhält Fade");
    nova_motion_update(1000);

    nova_motion_initialize();
    int32_t running_position=0,running_opacity=0,running_blur=0;
    nova_animation_t running_policy={.target=&running_position,.from=0,.to=100,
        .duration_ms=200,.property=NOVA_PROPERTY_X,.easing=NOVA_EASE_LINEAR,
        .state=NOVA_MOTION_CREATED,.interruptible=true};
    nova_animation_t running_fade=running_policy;
    running_fade.target=&running_opacity;running_fade.to=255;
    running_fade.property=NOVA_PROPERTY_OPACITY;
    nova_animation_t running_material=running_policy;
    running_material.target=&running_blur;running_material.to=12;
    running_material.property=NOVA_PROPERTY_BLUR;
    nova_animation_t *position_policy=nova_motion_create(&running_policy);
    nova_animation_t *fade_policy=nova_motion_create(&running_fade);
    nova_animation_t *material_policy=nova_motion_create(&running_material);
    nova_motion_update(50);
    failed|=check(position_policy&&fade_policy&&material_policy&&
        nova_motion_set_reduced(true)&&running_position==100&&running_blur==12&&
        position_policy->state==NOVA_MOTION_COMPLETED&&
        material_policy->state==NOVA_MOTION_COMPLETED&&
        fade_policy->state==NOVA_MOTION_RUNNING&&fade_policy->duration_ms==150&&
        nova_motion_diagnostics()->policy_replacements>=1&&
        nova_motion_diagnostics()->policy_disables>=1,
        "laufende Animationen im selben Frame auf Reduced-Motion-Policy umstellen");
    nova_motion_initialize();
    nova_motion_set_reduced(true);
    nova_dialog_motion_t dialog = {0};
    failed |= check(nova_dialog_enter(&dialog), "Dialog Enter Motion");
    failed |= check(dialog.scale==1000,"Reduced Motion entfernt Dialog-Scale");
    nova_motion_update(0);
    failed |= check(nova_dialog_motion_running(),"Reduced-Motion-Fade laeuft");
    nova_motion_update(1200);
    failed |= check(dialog.opacity == 255 && dialog.visible, "Dialog eingeblendet");
    failed |= check(nova_dialog_exit(&dialog), "Dialog Exit Motion");
    nova_motion_update(1400);
    failed |= check(dialog.opacity == 0, "Dialog ausgeblendet");
    nova_motion_set_reduced(false);
    dialog=(nova_dialog_motion_t){0};
    failed |= check(nova_dialog_enter(&dialog)&&dialog.scale==950,
                    "Standarddialog startet mit Fade und Scale");
    nova_dialog_motion_cancel(&dialog);
    failed |= check(!nova_dialog_motion_running(),"Dialogmotion sicher abbrechen");
    dialog=(nova_dialog_motion_t){0};
    failed |= check(nova_dialog_enter(&dialog),"Dialogmotion nach Abbruch erneut starten");
    nova_motion_update(1600);
    failed |= check(dialog.opacity==255&&dialog.scale==1000,
                    "Standarddialog beendet Enter fokussiert");
    failed |= check(nova_dialog_exit(&dialog),"Standarddialog Exit Motion");
    nova_motion_update(1800);
    failed |= check(dialog.opacity==0&&dialog.scale==950,
                    "Standarddialog beendet Exit verkleinert");

    nova_motion_set_reduced(true);
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
                    nova_motion_budget()->quality == 3&&
                    nova_motion_budget()->fallback_step==1&&
                    nova_motion_budget()->memory_bytes<=512u*1024u&&
                    !nova_motion_budget_can_allocate(NOVA_ANIMATION_SPRING)&&
                    !nova_motion_budget_can_allocate(NOVA_ANIMATION_TYPE_COUNT),
                    "Budget-Degradation und feste Speicher-/Typgrenzen");
    for(uint8_t fallback=1;fallback<6;++fallback)
        nova_motion_budget_update(20000,1200);
    failed|=check(nova_motion_budget_get()->violations==6&&
        nova_motion_budget_get()->fallback_step==6&&
        nova_motion_budget_get()->safe_mode&&
        !nova_motion_budget_get()->glow_enabled&&!nova_motion_budget_get()->shadow_enabled&&
        !nova_motion_budget_get()->blur_enabled&&!nova_motion_budget_get()->spring_enabled&&
        !nova_motion_budget_get()->material_enabled&&
        !nova_motion_budget_get()->decorative_enabled,
        "Fallbackreihenfolge endet deterministisch im Motion Safe Mode");

    nova_gop_reset();
    nova_gop_mode_candidate_t gop_modes[]={
        {0,800,600,832,NOVA_GOP_PIXEL_RGB_RESERVED,0,0,0,0},
        {1,1920,1080,1920,NOVA_GOP_PIXEL_BLT_ONLY,0,0,0,0},
        {2,1280,720,1280,NOVA_GOP_PIXEL_BGR_RESERVED,0,0,0,0},
        {3,1024,768,1024,NOVA_GOP_PIXEL_BIT_MASK,0x00ff0000u,0x0000ff00u,0x000000ffu,0xff000000u}};
    uint32_t selected_gop=99;
    failed|=check(nova_gop_select_mode(gop_modes,4,0,0,0,&selected_gop)==NOVA_GOP_OK&&
        selected_gop==0&&nova_gop_select_mode(gop_modes,4,0,1280,720,&selected_gop)==NOVA_GOP_OK&&
        selected_gop==2&&nova_gop_select_mode(gop_modes,4,1,0,0,&selected_gop)==NOVA_GOP_OK&&
        selected_gop==2&&nova_gop_select_mode(gop_modes,4,0,1366,768,&selected_gop)==NOVA_GOP_INVALID_MODE,
        "GOP-Modus deterministisch aktuell, bevorzugt oder als Fallback waehlen");
    nova_gop_descriptor_t gop_descriptor;
    failed|=check(!nova_gop_validate_candidate(&gop_modes[1])&&
        nova_gop_initialize(&gop_modes[2],4,0x100000u,1280u*4u*720u)==NOVA_GOP_OK&&
        nova_gop_is_available()&&nova_gop_get_descriptor(&gop_descriptor)==NOVA_GOP_OK&&
        gop_descriptor.width==1280&&gop_descriptor.pitch==5120&&
        gop_descriptor.pixel_format==NOVA_PIXEL_BGRA8888,
        "GOP-Framebufferdescriptor validieren und bereitstellen");
    failed|=check(nova_gop_shutdown()==NOVA_GOP_OK&&!nova_gop_is_available(),
        "GOP-Backend kontrolliert herunterfahren");

    nova_graphics_context_t invalid_graphics={.width=8,.height=8,.pitch=28,
        .bits_per_pixel=32,.pixel_format=NOVA_PIXEL_BGRA8888,
        .firmware=NOVA_GRAPHICS_FIRMWARE_TEST,.framebuffer=graphics_frontbuffer,
        .framebuffer_size=sizeof(graphics_frontbuffer)};
    failed|=check(!nova_graphics_initialize(&invalid_graphics),
                  "Graphics Context weist zu kleinen Pitch ab");
    nova_graphics_context_t graphics={.width=8,.height=8,.pitch=40,
        .bits_per_pixel=32,.pixel_format=NOVA_PIXEL_BGRA8888,
        .firmware=NOVA_GRAPHICS_FIRMWARE_TEST,.framebuffer=graphics_frontbuffer,
        .framebuffer_size=sizeof(graphics_frontbuffer),
        .capabilities=NOVA_GRAPHICS_LINEAR_FRAMEBUFFER};
    failed|=check(nova_graphics_initialize(&graphics)&&
                  nova_graphics_context()->scale_milli==1000&&
                  (nova_graphics_context()->capabilities&NOVA_GRAPHICS_DOUBLE_BUFFER),
                  "Validierter gemeinsamer Graphics Context");
    failed|=check(nova_graphics_convert_pixel(0xff112233,NOVA_PIXEL_RGBA8888,0,0,0,0)==0xff332211&&
                  nova_graphics_convert_pixel(0xff112233,NOVA_PIXEL_BGRA8888,0,0,0,0)==0xff112233&&
                  nova_graphics_convert_pixel(0xff112233,NOVA_PIXEL_RGB888,0,0,0,0)==0x00332211&&
                  nova_graphics_convert_pixel(0xff112233,NOVA_PIXEL_BGR888,0,0,0,0)==0x00112233&&
                  nova_graphics_convert_pixel(0xff112233,NOVA_PIXEL_RGB565,0,0,0,0)==0x1106&&
                  nova_graphics_convert_pixel(0xff112233,NOVA_PIXEL_BIT_MASK,0xf800,0x07e0,0x001f,0)==0x1106,
                  "RGBA-Konvertierung fuer RGB, BGR, RGB565 und Bitmask");
    failed|=check(nova_framebuffer_initialize()==NOVA_FB_OK&&
        nova_framebuffer_info()->backend==NOVA_FB_BACKEND_TEST&&
        nova_framebuffer_info()->pitch==40&&nova_framebuffer_info()->mapped,
        "Einheitliches Framebuffer Backend initialisieren und mappen");
    uint32_t read_color=0,copy_pixels[4]={0xff010203u,0xff112233u,0xff445566u,0xff778899u};
    failed|=check(nova_framebuffer_write_pixel(1,1,0xff267cc1u)==NOVA_FB_OK&&
        nova_framebuffer_read_pixel(1,1,&read_color)==NOVA_FB_OK&&read_color==0xff267cc1u&&
        nova_framebuffer_horizontal_line(0,2,4,0xffabcdefu)==NOVA_FB_OK&&
        nova_framebuffer_vertical_line(4,0,4,0xff123456u)==NOVA_FB_OK&&
        nova_framebuffer_rectangle((nova_rect_t){5,1,2,2},0xff654321u)==NOVA_FB_OK&&
        nova_framebuffer_copy((nova_rect_t){0,4,2,2},copy_pixels,2)==NOVA_FB_OK&&
        nova_framebuffer_write_pixel(8,0,0)==NOVA_FB_BOUNDS_ERROR,
        "Pitch-sichere Pixel-, Linien-, Rechteck- und Blockzugriffe");
    static uint32_t partial_frame[64];
    for(uint8_t i=0;i<64;++i)partial_frame[i]=0xff010101u+i;
    for(uint8_t i=0;i<80;++i)graphics_frontbuffer[i]=0xdeadbeefu;
    nova_state_set_phase(NOVA_STATE_PHASE_PRESENT);
    failed|=check(nova_framebuffer_begin_frame()==NOVA_FB_OK&&
        nova_framebuffer_damage((nova_rect_t){2,2,2,2})==NOVA_FB_OK&&
        nova_framebuffer_present(partial_frame,8,8,8)==NOVA_FB_OK&&
        graphics_frontbuffer[2+2*10]==partial_frame[2+2*8]&&
        graphics_frontbuffer[0]==0xdeadbeefu&&
        nova_framebuffer_diagnostics()->partial_presents==1,
        "Dirty Region atomar und ohne fremde Pixel praesentieren");
    nova_present_configuration_t present_configuration={.width=8,.height=8,
        .source_stride=8,.available_memory=0,.request_triple_buffer=false,
        .page_flip_available=false,.vsync=NOVA_PRESENT_VSYNC_EMULATED,
        .clock=test_clock_us};
    present_test_clock=0;
    failed|=check(nova_present_initialize(&present_configuration)==NOVA_PRESENT_OK&&
        nova_present_diagnostics()->buffering==NOVA_PRESENT_DOUBLE_BUFFER,
        "Present Scheduler startet standardmaessig mit Double Buffer");
    failed|=check(nova_present_begin(partial_frame)==NOVA_PRESENT_OK&&
        nova_present_is_busy()&&
        nova_present_begin(partial_frame)==NOVA_PRESENT_INVALID_STATE&&
        nova_present_full()==NOVA_PRESENT_OK&&!nova_present_is_busy()&&
        nova_present_diagnostics()->forced_presents==1,
        "Present Lock und erzwungenes Full Present");
    failed|=check(nova_present_begin(partial_frame)==NOVA_PRESENT_OK&&
        nova_present_damage((nova_rect_t){1,1,2,2})==NOVA_PRESENT_OK&&
        nova_present_damage((nova_rect_t){2,2,2,2})==NOVA_PRESENT_OK&&
        nova_present_frame()==NOVA_PRESENT_OK&&
        nova_present_diagnostics()->partial_presents==1&&
        nova_present_diagnostics()->merged_regions==1&&
        nova_present_diagnostics()->last_present_us==50,
        "Dirty Regions mergen und Partial Present zeitlich messen");
    nova_present_shutdown();
    static uint32_t triple_buffer[64];
    present_configuration.available_memory=sizeof(partial_frame);
    present_configuration.triple_buffer=triple_buffer;
    present_configuration.triple_buffer_pixels=64;
    present_configuration.request_triple_buffer=true;
    failed|=check(nova_present_initialize(&present_configuration)==NOVA_PRESENT_OK&&
        nova_present_diagnostics()->buffering==NOVA_PRESENT_TRIPLE_BUFFER&&
        nova_present_begin(partial_frame)==NOVA_PRESENT_OK&&
        nova_present_full()==NOVA_PRESENT_OK&&
        nova_present_diagnostics()->triple_buffer_frames==1&&
        nova_present_diagnostics()->pending_copies==1,
        "Optionalen Triple Buffer ueber Pending-Zustand praesentieren");
    nova_present_shutdown();
    present_configuration.source_stride=NOVA_SURFACE_WIDTH;
    present_configuration.available_memory=0;
    present_configuration.triple_buffer=0;
    present_configuration.triple_buffer_pixels=0;
    present_configuration.request_triple_buffer=false;
    failed|=check(nova_present_initialize(&present_configuration)==NOVA_PRESENT_OK,
        "Produktiven Double-Buffer-Scheduler fuer Compositor aktivieren");
    uint32_t presents_before_compositor=nova_graphics_diagnostics()->presents;
    nova_surface_manager_initialize();
    nova_surface_descriptor_t surface_description={NOVA_SURFACE_OFFSCREEN,8,8,
        NOVA_PIXEL_BGRA8888,0,NOVA_SURFACE_CLEAR_TRANSPARENT,0,77,
        NOVA_SURFACE_POOL_SCENE,4096};
    nova_surface_handle_t surface_handle=0,reused_handle=0,frame_handle=0;
    nova_surface_mapping_t surface_mapping;
    failed|=check(nova_managed_surface_create(&surface_description,&surface_handle)==NOVA_SURFACE_OK&&
        surface_handle&&nova_managed_surface_lock(surface_handle,&surface_mapping)==NOVA_SURFACE_OK&&
        surface_mapping.width==8&&surface_mapping.pitch>=32&&
        nova_managed_surface_lock(surface_handle,&surface_mapping)==NOVA_SURFACE_ERR_LOCKED&&
        nova_managed_surface_release(surface_handle)==NOVA_SURFACE_ERR_LOCKED&&
        nova_managed_surface_unlock(surface_handle,&(nova_rect_t){1,1,2,2})==NOVA_SURFACE_OK,
        "Surface Manager validiert Create, Pitch, Lock und Damage");
    failed|=check(nova_managed_surface_retain(surface_handle)==NOVA_SURFACE_OK&&
        nova_managed_surface_release(surface_handle)==NOVA_SURFACE_OK&&
        nova_managed_surface_release(surface_handle)==NOVA_SURFACE_OK,
        "Surface Referenzzaehlung fuehrt kontrolliert zu Recycling");
    surface_description.width=4;surface_description.height=4;
    failed|=check(nova_managed_surface_create(&surface_description,&reused_handle)==NOVA_SURFACE_OK&&
        reused_handle!=surface_handle&&nova_managed_surface_native(surface_handle)==0&&
        nova_surface_manager_diagnostics()->reuses==1,
        "Generation Handle erkennt recycelte veraltete Surface-ID");
    nova_surface_descriptor_t too_large=surface_description;too_large.width=8193;
    failed|=check(nova_managed_surface_create(&too_large,&frame_handle)==NOVA_SURFACE_ERR_SIZE_OVERFLOW,
        "Surface-Dimension und Groessenoverflow abweisen");
    too_large=surface_description;too_large.width=1024;too_large.height=1024;
    too_large.maximum_size=NOVA_MANAGED_SURFACE_DEFAULT_MAX_SIZE;
    failed|=check(nova_managed_surface_create(&too_large,&frame_handle)==NOVA_SURFACE_ERR_BUDGET_EXCEEDED,
        "Surface-Poolbudget hart begrenzen");
    surface_description.pool=NOVA_SURFACE_POOL_FRAME;
    failed|=check(nova_managed_surface_create(&surface_description,&frame_handle)==NOVA_SURFACE_OK&&
        nova_surface_manager_reset_frame()&&nova_managed_surface_retain(frame_handle)==NOVA_SURFACE_ERR_INVALID_STATE&&
        nova_surface_manager_emergency()!=0,"Frame-Pool resetten und Emergency-Surface vorhalten");

    failed|=check(nova_dirty_initialize(100,100,4,500)&&
        (nova_dirty_clear(),true)&&
        nova_dirty_add(&(nova_rect_t){-10,-10,20,20})&&
        nova_dirty_add(&(nova_rect_t){-10,-10,20,20})&&
        nova_dirty_add(&(nova_rect_t){5,5,20,20})&&
        nova_dirty_add_expanded(&(nova_rect_t){50,50,10,10},2,3,1)&&
        nova_dirty_get()->count==2&&nova_dirty_diagnostics()->duplicates==1&&
        nova_dirty_diagnostics()->merges==1&&
        nova_dirty_intersects((nova_rect_t){48,48,2,2}),
        "Dirty Manager clippt, dedupliziert, merged und erweitert Effekte");
    nova_dirty_clear();
    failed|=check(nova_dirty_add(&(nova_rect_t){0,0,80,80})&&
        nova_dirty_is_full()&&
        nova_dirty_diagnostics()->full_reason==NOVA_DIRTY_FULL_THRESHOLD,
        "Konfigurierbarer Dirty-Flaechenschwellwert erzwingt Full Damage");
    failed|=check(nova_dirty_initialize(100,100,2,1000)&&
        (nova_dirty_clear(),true)&&
        nova_dirty_add(&(nova_rect_t){0,0,1,1})&&
        nova_dirty_add(&(nova_rect_t){10,10,1,1})&&
        nova_dirty_add(&(nova_rect_t){20,20,1,1})&&nova_dirty_is_full()&&
        nova_dirty_diagnostics()->full_reason==NOVA_DIRTY_FULL_CAPACITY&&
        !nova_dirty_add_expanded(&(nova_rect_t){0,0,1,1},UINT32_MAX,UINT32_MAX,UINT32_MAX),
        "Kapazitaet und Erweiterungsoverflow sicher behandeln");
    failed |= check(nova_compositor_initialize(8, 8), "Compositor initialisieren");
    nova_surface_t *base = nova_surface_acquire();
    nova_surface_t *overlay = nova_surface_acquire();
    failed |= check(base && overlay, "feste Surface-Pools");
    nova_surface_handle_t imported=0;
    failed|=check(nova_managed_surface_import(base,NOVA_SURFACE_BACKBUFFER,
        NOVA_SURFACE_POOL_PERSISTENT,1,&imported)==NOVA_SURFACE_OK&&
        nova_managed_surface_native(imported)->pixels==base->pixels,
        "Firmware-/Compositor-Surface validiert importieren");
    nova_layer_manager_initialize();
    nova_layer_descriptor_t layer_description={.type=NOVA_LAYER_CONTENT,
        .parent=nova_layer_manager_root(),.z_index=2,.bounds={0,0,8,8},
        .clip={0,0,8,8},.opacity=1000,.transform={65536,0,0,65536,0,0},
        .flags=NOVA_LAYER_VISIBLE|NOVA_LAYER_ENABLED,
        .surface_policy=NOVA_LAYER_SURFACE_NONE};
    nova_layer_handle_t content_layer=0,dialog_layer=0,cursor_layer=0;
    failed|=check(nova_managed_layer_create(&layer_description,&content_layer),
        "Generation-sicheren Content-Layer erzeugen");
    layer_description.type=NOVA_LAYER_DIALOG;layer_description.parent=content_layer;
    layer_description.opacity=700;layer_description.surface_policy=NOVA_LAYER_SURFACE_AUTO;
    failed|=check(nova_managed_layer_create(&layer_description,&dialog_layer)&&
        (nova_managed_layer_get(dialog_layer)->flags&NOVA_LAYER_ISOLATED)&&
        !nova_managed_layer_attach(dialog_layer,content_layer),
        "Layerhierarchie verhindert Zyklus und isoliert Gruppenopacity");
    layer_description.type=NOVA_LAYER_CURSOR;layer_description.parent=nova_layer_manager_root();
    layer_description.opacity=1000;layer_description.surface_policy=NOVA_LAYER_SURFACE_NONE;
    failed|=check(nova_managed_layer_create(&layer_description,&cursor_layer)&&
        nova_managed_layer_bind_surface(dialog_layer,reused_handle)&&
        nova_managed_layer_mark_dirty(dialog_layer,(nova_rect_t){1,1,2,2})&&
        nova_layer_manager_diagnostics()->damage_propagations>=2,
        "Layer-Surface-Bindung und Damage-Propagation");
    nova_layer_handle_t ordered_layers[8];
    uint16_t ordered_count=nova_layer_manager_build_order(ordered_layers,8);
    failed|=check(ordered_count==3&&ordered_layers[0]==content_layer&&
        ordered_layers[1]==dialog_layer&&ordered_layers[2]==cursor_layer&&
        nova_managed_layer_get(dialog_layer)->effective_opacity==700,
        "Layer stabil nach Klasse, Z, Creation und ID sortieren");
    nova_layer_manager_set_phase(NOVA_LAYER_PHASE_COMPOSITING);
    failed|=check(!nova_managed_layer_set_visible(content_layer,false)&&
        nova_layer_manager_diagnostics()->rejected_mutations==1,
        "Layer-Mutation waehrend Compositing sperren");
    nova_layer_manager_set_phase(NOVA_LAYER_PHASE_STATE_UPDATE);
    failed|=check(nova_managed_layer_destroy(dialog_layer)&&
        nova_managed_layer_destroy(content_layer)&&nova_managed_layer_destroy(cursor_layer),
        "Layer kontrolliert Kind vor Parent abbauen");
    nova_rect_t visible_clip={0};
    failed|=check(nova_clip_mask_initialize(8,8)&&nova_clip_mask_begin_frame()&&
        nova_clip_current().x==0&&nova_clip_current().y==0&&
        nova_clip_current().width==8&&nova_clip_current().height==8&&
        nova_clip_push((nova_rect_t){1,1,6,6})&&
        nova_clip_push((nova_rect_t){3,0,5,5})&&
        nova_clip_current().x==3&&nova_clip_current().y==1&&
        nova_clip_current().width==4&&nova_clip_current().height==4&&
        nova_clip_test(nova_clip_current_scope(),(nova_rect_t){2,2,3,3},
                       &visible_clip)==NOVA_CLIP_PARTIAL&&
        visible_clip.x==3&&visible_clip.width==2&&
        nova_clip_pop()&&nova_clip_pop()&&!nova_clip_pop()&&
        nova_clip_mask_end_frame(),
        "Root-, Dialog-, Scroll- und verschachtelte Clips schneiden");
    static const uint8_t bitmap_mask[2]={0x80u,0x40u};
    static const uint8_t alpha_mask[4]={0u,64u,128u,255u};
    nova_mask_handle_t rectangle_mask=0,bitmap_handle=0,alpha_handle=0;
    nova_mask_handle_t rounded_handle=0,vector_handle=0;
    failed|=check(
        nova_mask_create(&(nova_mask_descriptor_t){.type=NOVA_MASK_RECTANGLE,
            .bounds={0,0,4,4},.cacheable=true},&rectangle_mask)&&
        nova_mask_create(&(nova_mask_descriptor_t){.type=NOVA_MASK_BITMAP,
            .bounds={0,0,2,2},.pixels=bitmap_mask,.stride=1,.size=2},
            &bitmap_handle)&&
        nova_mask_create(&(nova_mask_descriptor_t){.type=NOVA_MASK_ALPHA,
            .bounds={1,1,2,2},.pixels=alpha_mask,.stride=2,.size=4},
            &alpha_handle)&&
        nova_mask_create(&(nova_mask_descriptor_t){.type=NOVA_MASK_ROUNDED_RECTANGLE,
            .bounds={0,0,4,4},.radius=2},&rounded_handle)&&
        nova_mask_create(&(nova_mask_descriptor_t){.type=NOVA_MASK_VECTOR,
            .bounds={0,0,4,4},.vector_callback=test_vector_mask,.cacheable=true},
            &vector_handle)&&nova_clip_mask_begin_frame()&&
        nova_mask_push(rectangle_mask)&&!nova_mask_push(rectangle_mask)&&
        nova_mask_push(alpha_handle)&&nova_mask_coverage(nova_mask_current_scope(),1,1)==0&&
        nova_mask_coverage(nova_mask_current_scope(),2,2)==255&&
        nova_mask_pop()&&nova_mask_pop()&&nova_mask_push(bitmap_handle)&&
        nova_mask_coverage(nova_mask_current_scope(),0,0)==255&&
        nova_mask_coverage(nova_mask_current_scope(),1,0)==0&&nova_mask_pop()&&
        nova_mask_push(rounded_handle)&&nova_mask_coverage(nova_mask_current_scope(),0,0)==0&&
        nova_mask_coverage(nova_mask_current_scope(),2,2)==255&&nova_mask_pop()&&
        nova_mask_push(vector_handle)&&nova_mask_coverage(nova_mask_current_scope(),0,0)==255&&
        nova_mask_coverage(nova_mask_current_scope(),1,0)==128&&
        nova_mask_coverage(nova_mask_current_scope(),1,0)==128&&nova_mask_pop()&&
        nova_clip_mask_end_frame()&&nova_clip_mask_diagnostics()->cache_hits>=1,
        "Rechteck-, Bitmap-, Alpha-, Rounded- und Vektormasken kombinieren");
    failed|=check(nova_mask_destroy(rectangle_mask)&&nova_mask_destroy(bitmap_handle)&&
        nova_mask_destroy(alpha_handle)&&nova_mask_destroy(rounded_handle)&&
        nova_mask_destroy(vector_handle),
        "Masken generationensicher ausserhalb aktiver Scopes freigeben");

    static uint8_t render_alpha_mask[64];
    for(uint8_t i=0;i<64;++i)render_alpha_mask[i]=0;
    render_alpha_mask[6]=128;
    nova_mask_handle_t render_mask=0;
    failed|=check(nova_clip_mask_initialize(8,8)&&
        nova_mask_create(&(nova_mask_descriptor_t){.type=NOVA_MASK_ALPHA,
            .bounds={0,0,8,8},.pixels=render_alpha_mask,.stride=8,.size=64},
            &render_mask),"Render-Maske vorbereiten");
    nova_render_initialize();
    nova_transform2d_t render_transform=nova_transform_identity();
    nova_round_rect_t render_rounded={0};
    failed|=check(nova_transform_scale(&render_transform,2,1)&&
        nova_transform_translate(&render_transform,2,0)&&
        nova_round_rect_create(&render_rounded,3,3,3,3,1),
        "Rendertransformation vorbereiten");
    failed|=check(nova_render_begin_frame()&&
        nova_render_clear(base,0xff000000u,NOVA_RENDER_BACKGROUND)&&
        nova_render_rect(base,(nova_rect_t){0,1,2,1},0xff112233u,NOVA_RENDER_SHAPE,2)&&
        nova_render_rect(base,(nova_rect_t){2,1,2,1},0xff112233u,NOVA_RENDER_SHAPE,2)&&
        nova_mask_push(render_mask)&&
        nova_render_rect(base,(nova_rect_t){6,0,1,1},0xff00ff00u,NOVA_RENDER_SHAPE,3)&&
        nova_mask_pop()&&
        nova_render_push_transform(&render_transform)&&
        nova_render_rect(base,(nova_rect_t){0,0,1,1},0xffff0000u,
                         NOVA_RENDER_SHAPE,4)&&
        nova_render_pop_transform()&&
        nova_render_push_clip((nova_rect_t){2,2,4,4})&&
        nova_render_line(base,0,0,7,7,0xffffffffu,NOVA_RENDER_OVERLAY,0)&&
        nova_render_circle(base,4,4,1,0xff267cc1u,NOVA_RENDER_OVERLAY,1)&&
        nova_render_rounded_rect(base,&render_rounded,0xff336699u,
                                 NOVA_RENDER_OVERLAY,2)&&
        nova_render_pop_clip(),"Unveraenderliche Render Commands erfassen");
    failed|=check(!nova_render_rect(base,(nova_rect_t){0,0,0,1},0,
        NOVA_RENDER_SHAPE,0),"Ungueltigen Render Command verwerfen");
    failed|=check(nova_render_validate()&&nova_render_sort_optimize()&&
        nova_render_command_count()==7&&nova_render_diagnostics()->batches==1&&
        nova_render_execute()&&nova_render_end_frame()&&
        base->pixels[1*base->stride+1]==0xff112233u&&
        base->pixels[6]==0x8000ff00u&&
        base->pixels[2]==0xffff0000u&&base->pixels[3]==0xffff0000u&&
        base->pixels[2*base->stride+2]==0xffffffffu&&
        base->pixels[4*base->stride+4]==0xff336699u,
        "Render Queue sortiert, batcht, clippt und rastert deterministisch");
    nova_round_rect_t masked_effect_geometry={0};
    nova_effect_object_t masked_effect_object={0};
    nova_effect_t masked_shadow={NOVA_EFFECT_DROP_SHADOW,0xff000000u,1,0.5f,0,0};
    failed|=check(nova_round_rect_create(&masked_effect_geometry,5,0,1,1,0)&&
        nova_effect_object_create(&masked_effect_object,&masked_effect_geometry)&&
        nova_shadow_set(&masked_effect_object,&masked_shadow)&&
        nova_clip_mask_begin_frame()&&nova_mask_push(render_mask),
        "Effekt-Clip- und Maskscope vorbereiten");
    uint16_t effect_mask_scope=nova_mask_current_scope();
    base->pixels[6]=0;base->pixels[7]=0;
    failed|=check(nova_effect_render(&masked_effect_object,base,NOVA_EFFECT_PHASE_SHADOW,
        (nova_rect_t){6,0,1,1},effect_mask_scope)&&base->pixels[6]==0x20000000u&&
        base->pixels[7]==0,
        "Shadow respektiert echten Alpha-Maskscope und harten Clip");
    nova_surface_clear(overlay,0);
    nova_round_rect_t masked_blur_geometry={0};
    failed|=check(nova_round_rect_create(&masked_blur_geometry,6,0,1,1,0),
        "Gerundete Blurmaske vorbereiten");
    nova_blur_region_t masked_blur={{6,0,1,1},0,NOVA_BLUR_QUALITY_LOW,
        NOVA_BLUR_KERNEL_BOX,&masked_blur_geometry,effect_mask_scope};
    failed|=check(nova_blur_initialize(8,8,1024*1024)&&
        nova_blur_render(&masked_blur,base,overlay)==NOVA_BLUR_OK&&
        overlay->pixels[6]==0x10000000u&&overlay->pixels[7]==0&&
        nova_mask_pop()&&nova_clip_mask_end_frame()&&nova_mask_destroy(render_mask),
        "Background Blur wendet Alpha-Maske erst nach dem Kernel an");

    static const uint8_t lz4_abc_stream[]={0x35,'a','b','c',0x03,0x00};
    static const uint8_t lz4_abc_expected[]={'a','b','c','a','b','c','a','b','c','a','b','c'};
    static const uint8_t lz4_literal_stream[]={0x30,'N','O','V'};
    static const uint8_t lz4_bad_offset[]={0x10,'x',0x00,0x00};
    static const uint8_t lz4_truncated[]={0xf0};
    uint8_t decompressed[32]={0};uint64_t decompressed_size=0;
    failed|=check(nova_compression_initialize()&&
        nova_resource_decompress(NOVA_COMPRESSION_LZ4,lz4_abc_stream,
            sizeof(lz4_abc_stream),decompressed,sizeof(lz4_abc_expected),
            &decompressed_size)==NOVA_COMPRESSION_OK&&
        decompressed_size==sizeof(lz4_abc_expected)&&
        nova_resource_checksum(decompressed,decompressed_size)==
            nova_resource_checksum(lz4_abc_expected,sizeof(lz4_abc_expected)),
        "LZ4-Block dekodiert ueberlappende Matches deterministisch");
    failed|=check(nova_resource_decompress(NOVA_COMPRESSION_NONE,lz4_literal_stream+1,3,
        decompressed,3,&decompressed_size)==NOVA_COMPRESSION_OK&&decompressed_size==3&&
        decompressed[0]=='N'&&decompressed[2]=='V',"Unkomprimierte Ressource kopieren");
    failed|=check(nova_resource_decompress(NOVA_COMPRESSION_LZ4,lz4_bad_offset,
        sizeof(lz4_bad_offset),decompressed,5,&decompressed_size)==NOVA_COMPRESSION_CORRUPT&&
        nova_resource_decompress(NOVA_COMPRESSION_LZ4,lz4_truncated,
        sizeof(lz4_truncated),decompressed,16,&decompressed_size)==
            NOVA_COMPRESSION_SOURCE_TRUNCATED&&
        nova_resource_decompress(NOVA_COMPRESSION_LZ4,lz4_literal_stream,
        sizeof(lz4_literal_stream),decompressed,2,&decompressed_size)==
            NOVA_COMPRESSION_DESTINATION_TOO_SMALL&&
        nova_resource_decompress(NOVA_COMPRESSION_LZ4,lz4_literal_stream,
        sizeof(lz4_literal_stream),decompressed,4,&decompressed_size)==
            NOVA_COMPRESSION_SIZE_MISMATCH&&
        nova_resource_decompress(NOVA_COMPRESSION_ZSTD,lz4_literal_stream,
        sizeof(lz4_literal_stream),decompressed,3,&decompressed_size)==
            NOVA_COMPRESSION_UNSUPPORTED,
        "Kompressionsfehler und optionales Zstd werden typisiert behandelt");

    static const uint8_t sha256_abc[32]={
        0xba,0x78,0x16,0xbf,0x8f,0x01,0xcf,0xea,0x41,0x41,0x40,0xde,0x5d,0xae,0x22,0x23,
        0xb0,0x03,0x61,0xa3,0x96,0x17,0x7a,0x9c,0xb4,0x10,0xff,0x61,0xf2,0x00,0x15,0xad};
    uint8_t sha256_result[32]={0};
    failed|=check(nova_integrity_initialize(NOVA_INTEGRITY_STANDARD)&&
        nova_integrity_sha256("abc",3,sha256_result)&&
        nova_resource_checksum(sha256_result,32)==nova_resource_checksum(sha256_abc,32),
        "SHA-256 entspricht dem bekannten FIPS-Testvektor abc");
    nova_integrity_descriptor_t integrity_descriptor={.resource_id=1,.type=1,.version=1,
        .data="abc",.size=3,.expected_crc32=nova_integrity_crc32("abc",3),
        .expected_sha256=sha256_abc,.signature=NOVA_SIGNATURE_NONE};
    nova_integrity_report_t integrity_report={0};
    failed|=check(nova_resource_verify(&integrity_descriptor,&integrity_report)==
        NOVA_INTEGRITY_OK&&integrity_report.trust==NOVA_RESOURCE_TRUST_VALID&&
        integrity_report.crc_checked&&integrity_report.sha256_checked,
        "Standardrichtlinie prueft CRC32 und SHA-256");
    integrity_descriptor.expected_sha256=0;integrity_descriptor.expected_crc32=0;
    failed|=check(nova_resource_verify(&integrity_descriptor,&integrity_report)==
        NOVA_INTEGRITY_CHECKSUM_MISMATCH&&
        nova_integrity_set_policy(NOVA_INTEGRITY_PERMISSIVE)&&
        nova_resource_verify(&integrity_descriptor,&integrity_report)==NOVA_INTEGRITY_OK&&
        nova_integrity_set_policy(NOVA_INTEGRITY_STRICT)&&
        nova_resource_verify(&integrity_descriptor,&integrity_report)==
            NOVA_INTEGRITY_SIGNATURE_REQUIRED,
        "Permissiv, Standard und Strikt erzwingen unterschiedliche Vertrauensregeln");
    integrity_descriptor.expected_sha256=sha256_abc;
    integrity_descriptor.signature=NOVA_SIGNATURE_VERIFIED;
    failed|=check(nova_resource_verify(&integrity_descriptor,&integrity_report)==
        NOVA_INTEGRITY_OK&&integrity_report.trust==NOVA_RESOURCE_TRUST_SIGNED&&
        integrity_report.signature_valid,"Strikt akzeptiert vorgelagert verifizierte Signatur");
    integrity_descriptor.signature=NOVA_SIGNATURE_INVALID;
    failed|=check(nova_resource_verify(&integrity_descriptor,&integrity_report)==
        NOVA_INTEGRITY_SIGNATURE_INVALID,"Ungueltige Signatur wird immer abgewiesen");

    failed|=check(nova_resource_loader_initialize(10),
        "Resource Loader reserviert feste Registry und Cachebudget");
    static const uint8_t resource_dependency_data[2]={1,2};
    static const uint8_t resource_parent_data[3]={3,4,5};
    nova_resource_descriptor_t dependency_descriptor={.uri="test://resource/dependency",
        .type=NOVA_RESOURCE_BINARY,.version=1,.data=resource_dependency_data,
        .size=sizeof(resource_dependency_data),.origin=NOVA_RESOURCE_ORIGIN_EMBEDDED,
        .priority=NOVA_RESOURCE_PRIORITY_HIGH,.compression=NOVA_COMPRESSION_NONE};
    nova_resource_descriptor_t parent_descriptor={.uri="test://resource/parent",
        .type=NOVA_RESOURCE_CONFIGURATION,.version=1,.data=resource_parent_data,
        .size=sizeof(resource_parent_data),.origin=NOVA_RESOURCE_ORIGIN_EMBEDDED,
        .priority=NOVA_RESOURCE_PRIORITY_CRITICAL,.compression=NOVA_COMPRESSION_NONE};
    nova_resource_t *dependency_resource=0,*parent_resource=0;
    failed|=check(nova_resource_register_descriptor(&dependency_descriptor,&dependency_resource)==
        NOVA_RESOURCE_OK&&nova_resource_register_descriptor(&parent_descriptor,&parent_resource)==
        NOVA_RESOURCE_OK&&nova_resource_add_dependency(parent_resource->id,dependency_resource->id),
        "Ressourcenmodell speichert Name, Herkunft, Prioritaet und Abhaengigkeit");
    const nova_resource_t *loaded_parent=0;
    failed|=check(nova_resource_load_mode(parent_resource->id,NOVA_LOAD_LAZY,&loaded_parent)==
        NOVA_RESOURCE_OK&&loaded_parent==parent_resource&&dependency_resource->reference_count==1&&
        nova_resource_release(parent_resource->id)&&dependency_resource->reference_count==0,
        "Lazy Loading loest Abhaengigkeiten auf und gibt sie transitiv frei");
    static const uint8_t cycle_data[1]={9};
    failed|=check(nova_resource_register("test://resource/cycle-a",NOVA_RESOURCE_BINARY,1,
        cycle_data,sizeof(cycle_data),0,0)&&
        nova_resource_register("test://resource/cycle-b",NOVA_RESOURCE_BINARY,1,
        cycle_data,sizeof(cycle_data),0,0),"Zyklusressourcen registrieren");
    uint64_t cycle_a=nova_resource_id("test://resource/cycle-a");
    uint64_t cycle_b=nova_resource_id("test://resource/cycle-b");
    failed|=check(nova_resource_add_dependency(cycle_a,cycle_b)&&
        nova_resource_add_dependency(cycle_b,cycle_a)&&!nova_resource_load(cycle_a)&&
        nova_resource_diagnostics()->cycles==1,"Rekursive Abhaengigkeiten sicher erkennen");
    uint8_t corruptible_resource[2]={6,7};
    static const uint8_t fallback_resource_data[2]={8,9};
    failed|=check(nova_resource_register("test://resource/fallback",NOVA_RESOURCE_BINARY,1,
        fallback_resource_data,sizeof(fallback_resource_data),0,0),"Fallback registrieren");
    uint64_t fallback_resource_id=nova_resource_id("test://resource/fallback");
    failed|=check(nova_resource_register("test://resource/primary",NOVA_RESOURCE_BINARY,1,
        corruptible_resource,sizeof(corruptible_resource),0,fallback_resource_id),
        "Primaerressource mit Fallback registrieren");
    corruptible_resource[0]^=1;
    const nova_resource_t *fallback_loaded=nova_resource_load(
        nova_resource_id("test://resource/primary"));
    failed|=check(fallback_loaded&&fallback_loaded->id==fallback_resource_id&&
        nova_resource_diagnostics()->fallback_uses==1&&nova_resource_release(fallback_resource_id),
        "Integritaetsfehler aktiviert verifizierte Default-Ressource");
    static const uint8_t eviction_a[6]={1,1,1,1,1,1};
    static const uint8_t eviction_b[6]={2,2,2,2,2,2};
    failed|=check(nova_resource_register("test://resource/evict-a",NOVA_RESOURCE_BINARY,1,
        eviction_a,sizeof(eviction_a),0,0)&&nova_resource_register(
        "test://resource/evict-b",NOVA_RESOURCE_BINARY,1,eviction_b,sizeof(eviction_b),0,0),
        "LRU-Testressourcen registrieren");
    uint64_t evict_a=nova_resource_id("test://resource/evict-a");
    uint64_t evict_b=nova_resource_id("test://resource/evict-b");
    failed|=check(nova_resource_load(evict_a)&&nova_resource_release(evict_a)&&
        nova_resource_load(evict_b)&&nova_resource_diagnostics()->evictions>=1&&
        nova_resource_find(evict_a)->state==NOVA_RESOURCE_UNLOADED&&
        nova_resource_unload(evict_b)==NOVA_RESOURCE_BUSY&&nova_resource_release(evict_b)&&
        nova_resource_unload(evict_b)==NOVA_RESOURCE_OK,
        "Cachebudget erzwingt LRU und Busy-Ressourcen bleiben geschuetzt");
    failed|=check(nova_resource_load_mode(parent_resource->id,NOVA_LOAD_BACKGROUND,
        &loaded_parent)==NOVA_RESOURCE_UNSUPPORTED,"Optionales Background Loading typisiert abweisen");
    failed|=check(nova_resource_preload(parent_resource->id)&&
        parent_resource->state==NOVA_RESOURCE_RELEASED&&parent_resource->reference_count==0,
        "Preloading legt verifizierte Ressource ohne aktive Referenz in Cache");
    nova_resource_manager_shutdown();
    failed|=check(!nova_resource_load(parent_resource->id)&&
        !nova_resource_diagnostics()->initialized,"Shutdown sperrt weitere Ladezugriffe");
    failed|=check(nova_resource_loader_initialize(64)&&
        nova_resource_cache_configure(4,64,8,48)&&
        !nova_resource_cache_configure(65,64,0,64),
        "Cachegrenzen fuer Minimum, Maximum, Reserve und kritischen Bereich validieren");
    static const uint8_t cache_lfu_a[4]={1,2,3,4},cache_lfu_b[4]={5,6,7,8};
    static const uint8_t cache_permanent[4]={9,10,11,12};
    static const uint8_t cache_pressure[56]={0};
    failed|=check(nova_resource_register("test://cache/lfu-a",NOVA_RESOURCE_BINARY,1,
        cache_lfu_a,sizeof(cache_lfu_a),0,0)&&
        nova_resource_register("test://cache/lfu-b",NOVA_RESOURCE_BINARY,1,
        cache_lfu_b,sizeof(cache_lfu_b),0,0)&&
        nova_resource_register("test://cache/permanent",NOVA_RESOURCE_BINARY,1,
        cache_permanent,sizeof(cache_permanent),0,0)&&
        nova_resource_register("test://cache/pressure",NOVA_RESOURCE_BINARY,1,
        cache_pressure,sizeof(cache_pressure),0,0),
        "Cache-Testressourcen genau einmal registrieren");
    uint64_t cache_a=nova_resource_id("test://cache/lfu-a");
    uint64_t cache_b=nova_resource_id("test://cache/lfu-b");
    uint64_t cache_p=nova_resource_id("test://cache/permanent");
    uint64_t cache_big=nova_resource_id("test://cache/pressure");
    failed|=check(nova_resource_cache_set_policy(cache_a,NOVA_CACHE_LFU)&&
        nova_resource_cache_set_policy(cache_b,NOVA_CACHE_LFU)&&
        nova_resource_cache_set_policy(cache_p,NOVA_CACHE_PERMANENT)&&
        nova_resource_load(cache_a)&&nova_resource_release(cache_a)&&
        nova_resource_load(cache_b)&&nova_resource_load(cache_b)&&
        nova_resource_release(cache_b)&&nova_resource_release(cache_b)&&
        nova_resource_load(cache_p)&&nova_resource_release(cache_p)&&
        nova_resource_load(cache_big)&&
        nova_resource_find(cache_a)->state==NOVA_RESOURCE_UNLOADED&&
        nova_resource_find(cache_b)->state==NOVA_RESOURCE_RELEASED&&
        nova_resource_find(cache_p)->state==NOVA_RESOURCE_RELEASED&&
        nova_resource_diagnostics()->lfu_evictions==1,
        "LFU entfernt unter Druck den seltensten Eintrag und schuetzt Permanent");
    failed|=check(nova_resource_release(cache_big)&&nova_resource_cache_collect()==2&&
        nova_resource_find(cache_p)->state==NOVA_RESOURCE_RELEASED&&
        nova_resource_diagnostics()->permanent_skips==1&&
        nova_resource_diagnostics()->current_references==0,
        "deterministische Bereinigung erhaelt permanente Ressourcen");
    nova_resource_manager_shutdown();
    failed|=check(nova_resource_loader_initialize(NOVA_RESOURCE_DEFAULT_CACHE_BUDGET),
        "Resource Loader fuer komprimierte Ressourcen neu initialisieren");
    nova_resource_descriptor_t compressed_descriptor={
        .uri="test://resource/lz4",.type=NOVA_RESOURCE_BINARY,.version=1,
        .data=lz4_abc_stream,.size=sizeof(lz4_abc_stream),
        .origin=NOVA_RESOURCE_ORIGIN_EMBEDDED,.priority=NOVA_RESOURCE_PRIORITY_NORMAL,
        .compression=NOVA_COMPRESSION_LZ4,.original_size=sizeof(lz4_abc_expected),
        .original_checksum=0};
    compressed_descriptor.original_checksum=nova_resource_checksum(lz4_abc_expected,
        sizeof(lz4_abc_expected));
    nova_resource_t *compressed_resource=0;
    failed|=check(nova_resource_register_descriptor(&compressed_descriptor,
        &compressed_resource)==NOVA_RESOURCE_OK&&compressed_resource&&!compressed_resource->data,
        "Komprimierte Ressource bleibt bis zum Erstzugriff gepackt");
    const nova_resource_t *decoded_resource=nova_resource_load(compressed_resource->id);
    failed|=check(decoded_resource&&decoded_resource->data&&
        nova_resource_checksum(decoded_resource->data,decoded_resource->size)==
            compressed_descriptor.original_checksum&&
        nova_resource_diagnostics()->decompressions==1&&
        nova_resource_load(compressed_resource->id)==decoded_resource&&
        nova_resource_diagnostics()->decompressions==1&&
        nova_integrity_diagnostics()->cache_skips==1&&
        nova_resource_release(compressed_resource->id)&&
        nova_resource_release(compressed_resource->id),
        "Lazy Dekomprimierung wird validiert und danach aus dem Cache wiederverwendet");
    static const uint8_t compressed_fallback_data[]={'o','k'};
    failed|=check(nova_resource_register("test://resource/lz4-fallback",NOVA_RESOURCE_BINARY,1,
        compressed_fallback_data,sizeof(compressed_fallback_data),0,0),
        "Dekomprimierungs-Fallback registrieren");
    nova_resource_descriptor_t corrupt_compressed={
        .uri="test://resource/lz4-corrupt",.type=NOVA_RESOURCE_BINARY,.version=1,
        .data=lz4_bad_offset,.size=sizeof(lz4_bad_offset),
        .fallback_id=nova_resource_id("test://resource/lz4-fallback"),
        .origin=NOVA_RESOURCE_ORIGIN_EMBEDDED,.priority=NOVA_RESOURCE_PRIORITY_NORMAL,
        .compression=NOVA_COMPRESSION_LZ4,.original_size=5,.original_checksum=1};
    failed|=check(nova_resource_register_descriptor(&corrupt_compressed,0)==NOVA_RESOURCE_OK&&
        nova_resource_load(nova_resource_id("test://resource/lz4-corrupt"))->id==
            corrupt_compressed.fallback_id&&nova_resource_diagnostics()->decompression_errors==1&&
        nova_resource_release(corrupt_compressed.fallback_id),
        "Beschaedigter LZ4-Datenstrom faellt auf sichere Ressource zurueck");
    nova_resource_descriptor_t oversized_compressed=compressed_descriptor;
    oversized_compressed.uri="test://resource/lz4-oversized";
    oversized_compressed.original_size=(uint64_t)NOVA_RESOURCE_DECODE_BLOCK_SIZE*
        NOVA_RESOURCE_DECODE_BLOCK_COUNT+1;oversized_compressed.original_checksum=1;
    failed|=check(nova_resource_register_descriptor(&oversized_compressed,0)==NOVA_RESOURCE_OK&&
        nova_resource_load_mode(nova_resource_id(oversized_compressed.uri),NOVA_LOAD_LAZY,
            &decoded_resource)==NOVA_RESOURCE_NO_MEMORY,
        "Statischer Dekompressionspool weist uebergrosse Ausgaben sicher ab");
    nova_resource_manager_shutdown();
    nova_resource_manager_initialize();
    failed|=check(nova_image_initialize(),"Image Renderer reserviert Cache-Pixelpool");
    static const uint8_t raw_rgba[16]={
        255,0,0,255, 0,255,0,255, 0,0,255,255, 255,255,255,128};
    nova_image_t *raw_image=0,*raw_cached=0;
    uint64_t raw_id=nova_resource_id("test://image/raw");
    failed|=check(nova_image_create_raw(raw_id,2,2,8,NOVA_IMAGE_RGBA8888,
        raw_rgba,sizeof(raw_rgba),&raw_image)==NOVA_IMAGE_OK&&raw_image&&
        raw_image->pixels[0]==0xffff0000u&&raw_image->pixels[3]==0x80808080u&&
        nova_image_create_raw(raw_id,2,2,8,NOVA_IMAGE_RGBA8888,
            raw_rgba,sizeof(raw_rgba),&raw_cached)==NOVA_IMAGE_OK&&raw_cached==raw_image&&
        raw_image->references==2,"RAW RGBA wird premultipliziert und gecacht");
    uint32_t image_width=0,image_height=0;
    failed|=check(nova_image_get_size(raw_image,&image_width,&image_height)&&
        image_width==2&&image_height==2,"Imagegroesse typisiert abfragen");
    nova_surface_clear(overlay,0xff101010u);
    nova_image_render_options_t image_options={.destination={1,1,4,4},.clip={2,1,3,4},
        .sampling=NOVA_IMAGE_SAMPLE_BILINEAR,.transform=nova_transform_fixed_identity(),
        .opacity=1000,.tint=0xffffffffu};
    failed|=check(nova_image_render(raw_image,overlay,&image_options)==NOVA_IMAGE_OK&&
        overlay->pixels[1*overlay->stride+1]==0xff101010u&&
        overlay->pixels[2*overlay->stride+2]!=0xff101010u,
        "Bilineares Image Rendering respektiert Clip und Alpha");
    nova_transform2d_t image_transform=nova_transform_identity();
    nova_fixed_transform2d_t fixed_image_transform={0};
    failed|=check(nova_transform_translate(&image_transform,2,1)&&
        nova_transform_to_fixed(&image_transform,&fixed_image_transform),
        "Bildtransformation vorbereiten");
    image_options=(nova_image_render_options_t){.destination={0,0,2,2},.clip={0,0,8,8},
        .sampling=NOVA_IMAGE_SAMPLE_NEAREST,.transform=fixed_image_transform,
        .opacity=1000,.tint=0xffffffffu,.mirror_x=true};
    failed|=check(nova_image_render(raw_image,overlay,&image_options)==NOVA_IMAGE_OK&&
        overlay->pixels[1*overlay->stride+2]==0xff00ff00u,
        "Nearest Image wird gespiegelt, verschoben und layerneutral gerendert");
    static const uint8_t bmp_2x2[70]={
        0x42,0x4d,70,0,0,0,0,0,0,0,54,0,0,0,40,0,0,0,
        2,0,0,0,2,0,0,0,1,0,24,0,0,0,0,0,16,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        255,0,0, 255,255,255, 0,0,
        0,0,255, 0,255,0, 0,0};
    failed|=check(nova_resource_register("test://image/bmp",NOVA_RESOURCE_IMAGE,1,
        bmp_2x2,sizeof(bmp_2x2),0,0),"Interne BMP-Ressource registrieren");
    nova_image_t *bmp_image=0;
    failed|=check(nova_image_load("test://image/bmp",&bmp_image)==NOVA_IMAGE_OK&&
        bmp_image&&bmp_image->pixels[0]==0xffff0000u&&bmp_image->pixels[1]==0xff00ff00u,
        "Validiertes BMP24 bottom-up dekodieren");
    static const uint8_t png_rgba[78]={
        0x89,0x50,0x4e,0x47,0x0d,0x0a,0x1a,0x0a,0,0,0,0x0d,0x49,0x48,0x44,0x52,
        0,0,0,2,0,0,0,2,8,6,0,0,0,0x72,0xb6,0x0d,0x24,0,0,0,0x15,0x49,0x44,
        0x41,0x54,0x78,0x9c,0x63,0xf8,0xcf,0xc0,0xf0,0x1f,8,0x1b,0x18,0x80,0x34,
        8,0x38,0,0,0x44,0x13,8,0xb9,0x6d,0xe6,0x3e,0x21,0,0,0,0,0x49,0x45,
        0x4e,0x44,0xae,0x42,0x60,0x82};
    static const uint8_t png_rgb_stored[75]={
        0x89,0x50,0x4e,0x47,0x0d,0x0a,0x1a,0x0a,0,0,0,0x0d,0x49,0x48,0x44,0x52,
        0,0,0,2,0,0,0,1,8,2,0,0,0,0x7b,0x40,0xe8,0xdd,0,0,0,0x12,0x49,0x44,
        0x41,0x54,0x78,1,1,7,0,0xf8,0xff,0,1,2,3,4,5,6,0,0x3f,0,0x16,0x68,
        0x41,0x5f,0x8d,0,0,0,0,0x49,0x45,0x4e,0x44,0xae,0x42,0x60,0x82};
    static const uint8_t png_indexed[100]={
        0x89,0x50,0x4e,0x47,0x0d,0x0a,0x1a,0x0a,0,0,0,0x0d,0x49,0x48,0x44,0x52,
        0,0,0,2,0,0,0,1,8,3,0,0,0,0xc3,0xfc,0x8f,0xb8,0,0,0,6,0x50,0x4c,
        0x54,0x45,0xff,0,0,0,0,0xff,0x6c,0xa1,0xfd,0x8e,0,0,0,2,0x74,0x52,
        0x4e,0x53,0xff,0x40,0x93,0x6b,0x71,0xda,0,0,0,0x0b,0x49,0x44,0x41,0x54,
        0x78,0x9c,0x63,0x60,0x60,4,0,0,4,0,2,0xbf,0x7a,0x3f,0x4a,0,0,0,0,
        0x49,0x45,0x4e,0x44,0xae,0x42,0x60,0x82};
    static const uint8_t png_interlaced[70]={
        0x89,0x50,0x4e,0x47,0x0d,0x0a,0x1a,0x0a,0,0,0,0x0d,0x49,0x48,0x44,0x52,
        0,0,0,1,0,0,0,1,8,6,0,0,1,0x68,0x12,0xf4,0x1f,0,0,0,0x0d,0x49,0x44,
        0x41,0x54,0x78,0x9c,0x63,0x60,0x64,0x62,0x66,1,0,0,0x19,0,0x0b,0xe7,
        0x5a,0x46,0xa4,0,0,0,0,0x49,0x45,0x4e,0x44,0xae,0x42,0x60,0x82};
    static const uint8_t png_gray1[81]={
        0x89,0x50,0x4e,0x47,0x0d,0x0a,0x1a,0x0a,0,0,0,0x0d,0x49,0x48,0x44,0x52,
        0,0,0,4,0,0,0,1,1,0,0,0,0,0xd1,0x47,0x32,0x60,0,0,0,2,0x74,0x52,
        0x4e,0x53,0,1,1,0x94,0xfd,0xae,0,0,0,0x0a,0x49,0x44,0x41,0x54,0x78,
        0x9c,0x63,0x48,0,0,0,0x62,0,0x61,0xb1,0x98,0x79,0xf2,0,0,0,0,0x49,
        0x45,0x4e,0x44,0xae,0x42,0x60,0x82};
    static const uint8_t png_indexed2[107]={
        0x89,0x50,0x4e,0x47,0x0d,0x0a,0x1a,0x0a,0,0,0,0x0d,0x49,0x48,0x44,0x52,
        0,0,0,4,0,0,0,1,2,3,0,0,0,0x84,0x52,0xe7,0x5e,0,0,0,0x0c,0x50,0x4c,
        0x54,0x45,0xff,0,0,0,0xff,0,0,0,0xff,0xff,0xff,0xff,0xfb,0,0x60,0xf6,
        0,0,0,4,0x74,0x52,0x4e,0x53,0xff,0x80,0x40,0,0x7c,0xda,0x34,0xee,0,0,
        0,0x0a,0x49,0x44,0x41,0x54,0x78,0x9c,0x63,0x90,6,0,0,0x1d,0,0x1c,0x8e,
        0xf4,0xf5,0x21,0,0,0,0,0x49,0x45,0x4e,0x44,0xae,0x42,0x60,0x82};
    static const uint8_t png_grayalpha[]={
        0x89,0x50,0x4e,0x47,0x0d,0x0a,0x1a,0x0a,0,0,0,0x0d,0x49,0x48,0x44,0x52,
        0,0,0,2,0,0,0,1,8,4,0,0,0,0x5e,0x2b,0xb7,1,0,0,0,0x0d,0x49,0x44,
        0x41,0x54,0x78,0x9c,0x63,0x68,0xf8,0xff,0xdf,1,0,7,0x40,2,0xbf,0xd9,
        0xc4,0x24,0xb8,0,0,0,0,0x49,0x45,0x4e,0x44,0xae,0x42,0x60,0x82};
    static const uint8_t png_rgb_trns_multi[102]={
        0x89,0x50,0x4e,0x47,0x0d,0x0a,0x1a,0x0a,0,0,0,0x0d,0x49,0x48,0x44,0x52,
        0,0,0,2,0,0,0,1,8,2,0,0,0,0x7b,0x40,0xe8,0xdd,0,0,0,6,0x74,0x52,
        0x4e,0x53,0,1,0,2,0,3,0xc9,0x4b,0xab,0xf5,0,0,0,7,0x49,0x44,0x41,
        0x54,0x78,0x9c,0x63,0x60,0x64,0x62,0x66,0xa5,5,0xc7,0x7a,0,0,0,8,0x49,
        0x44,0x41,0x54,0x61,0x65,3,0,0,0x3f,0,0x16,0x76,0xf0,0xfb,0xb2,0,0,
        0,0,0x49,0x45,0x4e,0x44,0xae,0x42,0x60,0x82};
    static const uint8_t png_filters[75]={
        0x89,0x50,0x4e,0x47,0x0d,0x0a,0x1a,0x0a,0,0,0,0x0d,0x49,0x48,0x44,0x52,
        0,0,0,1,0,0,0,5,8,0,0,0,0,0xa1,0xef,0xd9,0x43,0,0,0,0x12,0x49,0x44,
        0x41,0x54,0x78,0x9c,0x63,0xe0,0x62,0x14,0x61,0xe2,0x62,0x96,0x64,0xe1,
        2,0,1,0x9f,0,0x56,0xfe,0x1e,0xc8,0x20,0,0,0,0,0x49,0x45,0x4e,0x44,
        0xae,0x42,0x60,0x82};
    nova_image_t *png_image=0;
    failed|=check(nova_resource_register("test://image/png-rgba",NOVA_RESOURCE_IMAGE,1,
        png_rgba,sizeof(png_rgba),0,0)&&nova_image_load("test://image/png-rgba",&png_image)==NOVA_IMAGE_OK&&
        png_image->pixels[0]==0xffff0000u&&png_image->pixels[1]==0x80008000u&&
        png_image->pixels[2]==0xff0000ffu&&png_image->pixels[3]==0x40404040u,
        "PNG RGBA mit Deflate und Premultiplikation dekodieren");
    failed|=check(nova_resource_register("test://image/png-rgb-stored",NOVA_RESOURCE_IMAGE,1,
        png_rgb_stored,sizeof(png_rgb_stored),0,0)&&
        nova_image_load("test://image/png-rgb-stored",&png_image)==NOVA_IMAGE_OK&&
        png_image->pixels[0]==0xff010203u&&png_image->pixels[1]==0xff040506u,
        "PNG RGB mit gespeichertem Deflate-Block dekodieren");
    failed|=check(nova_resource_register("test://image/png-indexed",NOVA_RESOURCE_IMAGE,1,
        png_indexed,sizeof(png_indexed),0,0)&&
        nova_image_load("test://image/png-indexed",&png_image)==NOVA_IMAGE_OK&&
        png_image->pixels[0]==0xffff0000u&&png_image->pixels[1]==0x40000040u,
        "Indiziertes PNG mit PLTE und tRNS dekodieren");
    failed|=check(nova_png_initialize()&&nova_png_validate(png_gray1,sizeof(png_gray1))&&
        nova_resource_register("test://image/png-gray1",NOVA_RESOURCE_IMAGE,1,
            png_gray1,sizeof(png_gray1),0,0)&&
        nova_image_load("test://image/png-gray1",&png_image)==NOVA_IMAGE_OK&&
        png_image->pixels[0]==0xff000000u&&png_image->pixels[1]==0&&
        png_image->pixels[2]==0&&png_image->pixels[3]==0xff000000u,
        "PNG-Graustufen 1 Bit und tRNS werden auf RGBA konvertiert");
    failed|=check(nova_resource_register("test://image/png-indexed2",NOVA_RESOURCE_IMAGE,1,
        png_indexed2,sizeof(png_indexed2),0,0)&&
        nova_image_load("test://image/png-indexed2",&png_image)==NOVA_IMAGE_OK&&
        png_image->pixels[0]==0xffff0000u&&png_image->pixels[1]==0x80008000u&&
        png_image->pixels[2]==0x40000040u&&png_image->pixels[3]==0,
        "PNG-Indexed 2 Bit entpackt PLTE und tRNS bitgenau");
    failed|=check(nova_resource_register("test://image/png-grayalpha",NOVA_RESOURCE_IMAGE,1,
        png_grayalpha,sizeof(png_grayalpha),0,0)&&
        nova_image_load("test://image/png-grayalpha",&png_image)==NOVA_IMAGE_OK&&
        png_image->pixels[0]==0xff808080u&&png_image->pixels[1]==0x40404040u,
        "Optionales PNG-Graustufen-Alpha wird premultipliziert");
    failed|=check(nova_resource_register("test://image/png-rgb-trns",NOVA_RESOURCE_IMAGE,1,
        png_rgb_trns_multi,sizeof(png_rgb_trns_multi),0,0)&&
        nova_image_load("test://image/png-rgb-trns",&png_image)==NOVA_IMAGE_OK&&
        png_image->pixels[0]==0&&png_image->pixels[1]==0xff040506u,
        "RGB-tRNS und mehrere aufeinanderfolgende IDAT-Chunks funktionieren");
    failed|=check(nova_resource_register("test://image/png-filters",NOVA_RESOURCE_IMAGE,1,
        png_filters,sizeof(png_filters),0,0)&&
        nova_image_load("test://image/png-filters",&png_image)==NOVA_IMAGE_OK&&
        png_image->pixels[0]==0xff0a0a0au&&png_image->pixels[1]==0xff141414u&&
        png_image->pixels[2]==0xff1e1e1eu&&png_image->pixels[3]==0xff282828u&&
        png_image->pixels[4]==0xff323232u,
        "PNG-Filter None, Sub, Up, Average und Paeth werden zeilenweise rekonstruiert");
    failed|=check(nova_resource_register("test://image/png-interlaced",NOVA_RESOURCE_IMAGE,1,
        png_interlaced,sizeof(png_interlaced),0,0)&&
        nova_image_load("test://image/png-interlaced",&png_image)==NOVA_IMAGE_UNSUPPORTED,
        "Nicht unterstuetztes interlaced PNG definiert abweisen");
    uint8_t png_bad_crc[sizeof(png_rgba)];memcpy(png_bad_crc,png_rgba,sizeof(png_rgba));
    png_bad_crc[45]^=1;
    failed|=check(nova_resource_register("test://image/png-bad-crc",NOVA_RESOURCE_IMAGE,1,
        png_bad_crc,sizeof(png_bad_crc),0,0)&&
        nova_image_load("test://image/png-bad-crc",&png_image)==NOVA_IMAGE_CORRUPT,
        "PNG mit ungueltiger Chunk-Pruefsumme abweisen");
    failed|=check(!nova_png_validate(png_bad_crc,sizeof(png_bad_crc))&&
        nova_image_diagnostics()->png_crc_errors>=2&&
        nova_image_diagnostics()->png_validations>=1&&
        nova_image_diagnostics()->png_idat_chunks>=7,
        "Oeffentliche PNG-Validierung und Chunkdiagnose erkennen Korruption");
    static const uint8_t corrupt_bmp[16]={'B','M'};
    failed|=check(nova_resource_register("test://image/corrupt",NOVA_RESOURCE_IMAGE,1,
        corrupt_bmp,sizeof(corrupt_bmp),0,0)&&
        nova_image_load("test://image/corrupt",&bmp_image)==NOVA_IMAGE_UNSUPPORTED,
        "Beschaedigten BMP-Header sicher abweisen");
    failed|=check(nova_image_destroy(raw_image)&&nova_image_destroy(raw_cached)&&
        nova_image_diagnostics()->cache_hits>=1&&nova_image_diagnostics()->renders==2,
        "Image-Referenzen und Diagnosen verwalten");
    failed|=check(nova_render_quality_initialize(false,64ull*1024u*1024u)&&
        nova_render_quality_get()==NOVA_RENDER_QUALITY_STANDARD&&
        nova_render_quality_parameters()->image_sampling==NOVA_IMAGE_SAMPLE_BILINEAR,
        "Quality Manager startet deterministisch mit Standardprofil");
    for(uint8_t profile=0;profile<NOVA_RENDER_QUALITY_COUNT;++profile)
        failed|=check(nova_render_quality_is_supported((nova_render_quality_t)profile),
                      "Alle fuenf Renderingprofile werden unterstuetzt");
    failed|=check(nova_render_quality_set(NOVA_RENDER_QUALITY_ULTRA)&&
        nova_render_quality_parameters()->anti_aliasing==NOVA_ROUNDED_AA_HIGH&&
        nova_render_quality_parameters()->effect_density==8&&nova_dirty_is_full(),
        "Ultra setzt maximale Parameter und erzwingt Full Redraw");
    failed|=check(nova_render_quality_set(NOVA_RENDER_QUALITY_SAFE)&&
        !nova_render_quality_parameters()->blur_enabled&&
        !nova_render_quality_parameters()->shadows_enabled&&
        !nova_render_quality_parameters()->animations_enabled&&nova_motion_is_reduced()&&
        nova_compositor_diagnostics()->fallback_level==3,
        "Safe deaktiviert Blur, Schatten, Transparenz und Animationen");
    failed|=check(!nova_render_quality_set(NOVA_RENDER_QUALITY_COUNT)&&
        nova_render_quality_diagnostics()->rejected_profiles==1,
        "Ungueltiges Renderingprofil wird abgewiesen");
    failed|=check(nova_render_quality_set_auto(64ull*1024u*1024u,false,false)&&
        nova_render_quality_get()==NOVA_RENDER_QUALITY_HIGH,
        "Auto waehlt High anhand des Hardwarebudgets");
    nova_render_quality_report_frame(40000);nova_render_quality_report_frame(40000);
    failed|=check(nova_render_quality_get()==NOVA_RENDER_QUALITY_HIGH,
        "Einzelne langsame Frames loesen keinen Profilwechsel aus");
    nova_render_quality_report_frame(40000);
    failed|=check(nova_render_quality_get()==NOVA_RENDER_QUALITY_STANDARD&&
        nova_render_quality_diagnostics()->automatic_adjustments==1,
        "Drei dauerhafte Budgetverletzungen degradieren atomar");
    nova_render_quality_report_memory(15ull*1024u*1024u);
    failed|=check(nova_render_quality_get()==NOVA_RENDER_QUALITY_SAFE&&
        nova_render_quality_diagnostics()->memory_degradations==1,
        "Speichermangel degradiert Auto auf Safe");
    nova_sw_renderer_configuration_t software_configuration={8,8,
        64ull*1024u*1024u,true,true,true,false};
    failed|=check(nova_sw_renderer_initialize(&software_configuration)==NOVA_SW_RENDERER_OK&&
        nova_sw_renderer_is_available()&&nova_sw_renderer_begin_frame()==NOVA_SW_RENDERER_OK&&
        nova_sw_renderer_render_frame(nova_scene_root())==NOVA_SW_RENDERER_OK,
        "Software Renderer initialisiert CPU-Pipeline und traversiert Scene Graph");
    failed|=check(nova_sw_renderer_complete_frame(true,true)==NOVA_SW_RENDERER_OK&&
        nova_sw_renderer_diagnostics()->rendered_frames>=2,
        "Software Renderer schliesst erfolgreichen Frame diagnostisch ab");
    failed|=check(nova_sw_renderer_report_memory(15ull*1024u*1024u)==
        NOVA_SW_RENDERER_RECOVERED&&
        nova_sw_renderer_diagnostics()->state==NOVA_SW_RENDERER_SAFE,
        "Software Renderer degradiert bei Speichermangel ohne Bedienverlust");
    failed|=check(nova_sw_renderer_report_failure(false)==NOVA_SW_RENDERER_RECOVERED&&
        nova_sw_renderer_diagnostics()->resets==1&&
        nova_sw_renderer_report_failure(true)==NOVA_SW_RENDERER_RECOVERED&&
        nova_sw_renderer_diagnostics()->safe_entries==1&&
        nova_sw_renderer_report_failure(true)==NOVA_SW_RENDERER_TEXT_REQUIRED&&
        nova_sw_renderer_text_required(),
        "Renderfehler eskalieren Reset, Full Repaint, Safe und Text");
    failed|=check(nova_sw_renderer_shutdown()==NOVA_SW_RENDERER_OK&&
        nova_sw_renderer_shutdown()==NOVA_SW_RENDERER_INVALID_STATE,
        "Software Renderer verhindert doppelten Shutdown");
    software_configuration.framebuffer_available=false;
    failed|=check(nova_sw_renderer_initialize(&software_configuration)==
        NOVA_SW_RENDERER_NOT_AVAILABLE&&nova_sw_renderer_text_required()&&
        nova_sw_renderer_shutdown()==NOVA_SW_RENDERER_OK,
        "Fehlender Framebuffer fordert funktionalen Textfallback an");
    static const uint8_t version_dep_data[]={1},version_app_100_data[]={2};
    static const uint8_t version_app_110_data[]={3},version_app_200_data[]={4};
    failed|=check(nova_resource_register("test://version/dep/1.0.0.7",NOVA_RESOURCE_FONT,1,
        version_dep_data,sizeof(version_dep_data),0,0)&&
        nova_resource_register("test://version/app/1.0.0.2",NOVA_RESOURCE_THEME,1,
        version_app_100_data,sizeof(version_app_100_data),0,0)&&
        nova_resource_register("test://version/app/1.1.0.9",NOVA_RESOURCE_THEME,1,
        version_app_110_data,sizeof(version_app_110_data),0,0)&&
        nova_resource_register("test://version/app/2.0.0.1",NOVA_RESOURCE_THEME,2,
        version_app_200_data,sizeof(version_app_200_data),0,0)&&
        nova_resource_version_initialize(),"Versionierungsressourcen registrieren");
    uint64_t version_dep=nova_resource_id("logical://version/dep");
    uint64_t version_app=nova_resource_id("logical://version/app");
    nova_resource_version_descriptor_t dep_version={.logical_id=version_dep,
        .backing_resource_id=nova_resource_id("test://version/dep/1.0.0.7"),
        .version={1,0,0,7},.minimum={1,0,0,0},.maximum={1,9,9,65535}};
    nova_resource_version_dependency_t app_dependencies[]={{version_dep,{1,0,0,0}}};
    nova_resource_version_descriptor_t app_100={.logical_id=version_app,
        .backing_resource_id=nova_resource_id("test://version/app/1.0.0.2"),
        .version={1,0,0,2},.minimum={1,0,0,0},.maximum={1,9,9,65535},
        .dependencies=app_dependencies,.dependency_count=1};
    nova_resource_version_descriptor_t app_110=app_100;
    app_110.backing_resource_id=nova_resource_id("test://version/app/1.1.0.9");
    app_110.version=(nova_resource_version_t){1,1,0,9};
    nova_resource_version_descriptor_t app_200=app_100;
    app_200.backing_resource_id=nova_resource_id("test://version/app/2.0.0.1");
    app_200.version=(nova_resource_version_t){2,0,0,1};
    app_200.minimum=(nova_resource_version_t){2,0,0,0};
    app_200.maximum=(nova_resource_version_t){2,9,9,65535};
    failed|=check(nova_resource_version_register(&dep_version)&&
        nova_resource_version_register(&app_100)&&nova_resource_version_register(&app_110)&&
        nova_resource_version_register(&app_200)&&!nova_resource_version_register(&app_110),
        "parallele Versionen zulassen und identisches Versionsduplikat abweisen");
    nova_resource_version_t v1000={1,0,0,0},v199max={1,9,9,65535};
    nova_resource_version_t v2000={2,0,0,0},v3000={3,0,0,0};
    failed|=check(nova_resource_version_compatible(&v1000,&app_110.version)&&
        !nova_resource_version_compatible(&v1000,&v2000)&&
        nova_resource_version_compare((nova_resource_version_t){1,1,0,1},
                                      (nova_resource_version_t){1,1,0,9})<0,
        "Major inkompatibel, Minor/Patch kompatibel und Build reproduzierbar ordnen");
    failed|=check(nova_resource_version_select(version_dep,&v1000,&v199max)&&
        nova_resource_version_select(version_app,&v1000,&v199max)&&
        nova_resource_version(version_app)->version.minor==1&&
        nova_resource_version_dependencies(nova_resource_version(version_app)),
        "hoechste kompatible Version samt Mindestabhaengigkeit aktivieren");
    failed|=check(nova_resource_version_rollback(version_app)&&
        nova_resource_version(version_app)->version.minor==0&&
        nova_resource_version_select(version_app,&v2000,&v2000)&&
        nova_resource_version(version_app)->version.build==1&&
        !nova_resource_version_select(version_app,&v3000,&v3000)&&
        nova_resource_version(version_app)->version.major==2&&
        nova_resource_version_diagnostics()->rollbacks==1,
        "Rollback waehlt aelteren kompatiblen Stand und Konflikt bleibt atomar");
    failed |= check(nova_theme_initialize() &&
                    nova_theme_validate(NOVA_THEME_DARK) &&
                    nova_theme_validate(NOVA_THEME_LIGHT) &&
                    nova_theme_validate(NOVA_THEME_HIGH_CONTRAST) &&
                    !nova_theme_current() &&
                    nova_theme_tokens()->background == 0xff101113u,
                    "drei vollständige Theme-Token-Tabellen und sicherer Startzustand");
    failed |= check(nova_theme_default()&&nova_theme_default()->is_default&&
                    nova_theme_default()->is_dark&&nova_theme_default()->version==1&&
                    !nova_theme_is_dark()&&nova_theme_diagnostics()->contrast_checks>=12,
                    "Dark-Theme-Descriptor und Kontrastvalidierung vor Aktivierung");
    failed |= check(nova_theme_light()&&!nova_theme_light()->is_default&&
                    !nova_theme_light()->is_dark&&nova_theme_light()->version==1&&
                    nova_theme_light()->theme_id==nova_resource_id("boot://themes/light")&&
                    !nova_theme_is_light(),
                    "Light-Theme-Descriptor ist vollständig und nicht Standard");
    const nova_accessibility_theme_t *high_contrast=nova_theme_high_contrast();
    failed |= check(high_contrast&&high_contrast->accessibility_theme&&
                    high_contrast->high_contrast&&high_contrast->reduced_motion&&
                    high_contrast->opaque_materials&&high_contrast->minimum_focus_dlu>=2&&
                    !nova_theme_is_high_contrast(),
                    "High-Contrast-Descriptor erzwingt Accessibility-Overrides");
    const nova_theme_resource_t *light_theme=nova_theme_resource_find(
        nova_resource_id("boot://themes/light"));
    failed |= check(light_theme && light_theme->valid && light_theme->version == 1 &&
                    light_theme->token_count == 22 && light_theme->references == 0,
                    "Theme-Ressourcen in O(1)-Registry mit Metadaten finden");
    failed |= check(nova_theme_activate_light() && nova_theme_is_light() &&
                    nova_theme_tokens()->background == 0xfff2f4f7u &&
                    nova_theme_current() == light_theme && light_theme->references == 1 &&
                    !nova_theme_resource_release(NOVA_THEME_LIGHT),
                    "atomarer Light-API-Wechsel hält aktive Theme-Ressource referenziert");
    failed |= check(nova_theme_activate(NOVA_THEME_LIGHT) && light_theme->references == 1 &&
                    nova_theme_diagnostics()->cache_hits >= 1,
                    "erneute Theme-Aktivierung nutzt Cache ohne Referenzleck");
    failed |= check(nova_theme_activate_high_contrast() &&
                    nova_theme_is_high_contrast() &&
                    nova_theme_reduced_motion() &&
                    nova_compositor_diagnostics()->fallback_level == 3 &&
                    light_theme->references == 0,
                    "High Contrast überschreibt Motion und Materialien");
    const nova_theme_resource_t *before_invalid=nova_theme_current();
    failed |= check(!nova_theme_activate(NOVA_THEME_COUNT) &&
                    nova_theme_current() == before_invalid,
                    "ungültiges Theme lässt aktives Theme atomar unverändert");
    failed |= check(nova_theme_activate_default() && nova_theme_is_dark() &&
                    !nova_theme_reduced_motion() &&
                    nova_compositor_diagnostics()->fallback_level == 0 &&
                    nova_theme_current()->references == 1 &&
                    nova_theme_diagnostics()->registered == NOVA_THEME_COUNT &&
                    nova_theme_diagnostics()->atomic_switches == 3&&
                    nova_theme_diagnostics()->token_lookups>=2,
                    "Dark Theme per Default-API, O(1)-Tokens und genau eine aktive Ressource");
    static const uint8_t resource_data[] = {1,2,3,4};
    failed |= check(nova_resource_register("boot://test/data", NOVA_RESOURCE_CONFIGURATION,
                    1, resource_data, sizeof(resource_data), 0, 0), "Ressource registrieren");
    uint64_t resource_id = nova_resource_id("boot://test/data");
    failed |= check(nova_resource_load(resource_id) && nova_resource_load(resource_id),
                    "Lazy Load und Cache Hit");
    failed |= check(nova_resource_release(resource_id) && nova_resource_release(resource_id),
                    "Referenzzaehlung und Freigabe");
    failed |= check(!nova_resource_register("boot://test/broken", NOVA_RESOURCE_IMAGE,
                    1, resource_data, sizeof(resource_data), 123, 0),
                    "ungueltige Pruefsumme abweisen");
    failed |= check(nova_text_register_font_resource() && nova_icons_initialize() &&
                    nova_branding_initialize(),
                    "Font-, Icon- und Brandingressourcen zentral registrieren");
    const nova_font_resource_t *system_font=nova_font_resource_find(NOVA_SYSTEM_FONT_ID);
    failed|=check(system_font&&nova_font_resource_validate(system_font)&&
        system_font->family_type==NOVA_FONT_FAMILY_UI&&
        system_font->weight==NOVA_FONT_WEIGHT_SEMIBOLD&&
        nova_font_resource_load(NOVA_SYSTEM_FONT_ID)&&
        nova_font_resource_load(NOVA_SYSTEM_FONT_ID)&&
        nova_font_resource_resolve(NOVA_SYSTEM_FONT_ID,'A')==system_font&&
        !nova_font_resource_resolve(NOVA_SYSTEM_FONT_ID,0x2603)&&
        nova_font_resource_release(NOVA_SYSTEM_FONT_ID)&&
        nova_font_resource_release(NOVA_SYSTEM_FONT_ID),
        "Font Registry, Unicode-Abdeckung, Cache und Missing Glyph verwalten");
    static const uint8_t fallback_font_data[]={0x4e,0x46,0x50,1};
    static const nova_font_coverage_t snow_coverage[]={{0x2603,0x2603}};
    failed|=check(nova_resource_register("test://font/snow",NOVA_RESOURCE_FONT,1,
        fallback_font_data,sizeof(fallback_font_data),0,0),"Fallback-Fontressource registrieren");
    nova_font_resource_descriptor_t snow_font={.font_id=0x534e4f57464f4e54ull,
        .resource_id=nova_resource_id("test://font/snow"),.fallback_id=NOVA_SYSTEM_FONT_ID,
        .name="Snow Symbols",.family="Symbols",.style="Regular",.version=1,
        .resource_version=1,.family_type=NOVA_FONT_FAMILY_SYMBOLS,
        .weight=NOVA_FONT_WEIGHT_REGULAR,.priority=100,.coverage=snow_coverage,
        .coverage_count=1};
    failed|=check(nova_font_resource_register(&snow_font)&&
        !nova_font_resource_register(&snow_font)&&
        nova_font_resource_resolve(snow_font.font_id,0x2603)->font_id==snow_font.font_id&&
        nova_font_resource_resolve(snow_font.font_id,'A')->font_id==NOVA_SYSTEM_FONT_ID&&
        nova_font_theme_set(NOVA_FONT_ROLE_SYMBOL,snow_font.font_id)&&
        nova_font_theme_get(NOVA_FONT_ROLE_SYMBOL)==snow_font.font_id&&
        nova_font_resource_diagnostics()->duplicates==1,
        "Deterministische Font-Fallbackkette, Theme-Rolle und Duplicate-Schutz");
    static const nova_animation_keyframe_t animation_frames[]={
        {0,NOVA_PROPERTY_OPACITY,0,NOVA_EASE_LINEAR},
        {100,NOVA_PROPERTY_OPACITY,1000,NOVA_EASE_LINEAR},
        {0,NOVA_PROPERTY_SCALE,900,NOVA_EASE_LINEAR},
        {50,NOVA_PROPERTY_SCALE,950,NOVA_EASE_IN_OUT},
        {100,NOVA_PROPERTY_SCALE,1000,NOVA_EASE_OUT_CUBIC}};
    failed|=check(nova_resource_register("test://animation/dialog",NOVA_RESOURCE_ANIMATION,1,
        animation_frames,sizeof(animation_frames),0,0)&&nova_animation_resource_initialize(),
        "Animationsressourcenmanager initialisieren");
    nova_animation_resource_descriptor_t animation_descriptor={
        .animation_id=0x414e494d4449414cull,
        .resource_id=nova_resource_id("test://animation/dialog"),.name="Dialog Enter",
        .version=1,.resource_version=1,.duration_ms=100,.repeat=NOVA_ANIMATION_REPEAT_COUNT,
        .repeat_count=1,.trigger=NOVA_ANIMATION_TRIGGER_DIALOG_OPEN,
        .category=NOVA_ANIMATION_CATEGORY_DIALOG,.priority=3,
        .keyframes=animation_frames,.keyframe_count=5};
    nova_animation_sample_t animation_sample={0};
    failed|=check(nova_animation_resource_register(&animation_descriptor)&&
        !nova_animation_resource_register(&animation_descriptor)&&
        nova_animation_resource_load(animation_descriptor.animation_id)&&
        nova_animation_resource_load(animation_descriptor.animation_id)&&
        nova_animation_resource_sample(animation_descriptor.animation_id,50,&animation_sample)&&
        animation_sample.values[NOVA_PROPERTY_OPACITY]==500&&
        animation_sample.values[NOVA_PROPERTY_SCALE]==950&&
        nova_animation_resource_for_trigger(NOVA_ANIMATION_TRIGGER_DIALOG_OPEN,
            NOVA_ANIMATION_CATEGORY_DIALOG)->animation_id==animation_descriptor.animation_id&&
        nova_animation_theme_bind(0,animation_descriptor.animation_id)&&
        nova_animation_theme_get(0)==animation_descriptor.animation_id&&
        nova_animation_resource_release(animation_descriptor.animation_id)&&
        nova_animation_resource_release(animation_descriptor.animation_id),
        "Deklarative kombinierte Timeline, Trigger, Cache und Themebindung");
    failed|=check(nova_animation_theme_speed(2000)&&
        nova_animation_resource_sample(animation_descriptor.animation_id,25,&animation_sample)&&
        animation_sample.values[NOVA_PROPERTY_OPACITY]==500,
        "Theme-Geschwindigkeit skaliert Timeline deterministisch");
    nova_animation_resource_set_reduced(true);
    failed|=check(nova_animation_resource_sample(animation_descriptor.animation_id,1,
        &animation_sample)&&animation_sample.values[NOVA_PROPERTY_OPACITY]==1000&&
        animation_sample.values[NOVA_PROPERTY_SCALE]==1000&&animation_sample.complete&&
        nova_motion_is_reduced(),"Reduced Motion springt deklarativ auf Endzustand");
    nova_animation_resource_set_reduced(false);
    nova_animation_resource_descriptor_t invalid_animation=animation_descriptor;
    invalid_animation.duration_ms=40;
    failed|=check(!nova_animation_resource_register(&invalid_animation)&&
        nova_animation_resource_diagnostics()->duplicates==1&&
        nova_animation_resource_diagnostics()->invalid_resources>=1,
        "Beschaedigte Timeline und doppelte ID abweisen");
    failed |= check(nova_resource_find(nova_resource_id("boot://branding/novaos/logo")) != 0,
                    "NovaOS-Logo im Resource Manager");
    bool all_icons=true;
    for(uint32_t icon=0;icon<NOVA_ICON_COUNT;++icon)
        if(!nova_icon_exists((nova_icon_token_t)icon))all_icons=false;
    failed |= check(all_icons,"alle semantischen Icon-Tokens besitzen Ressourcen");
    failed|=check(nova_design_initialize()&&nova_design_diagnostics()->compatible&&
                  nova_design_manifest()->design.major==NOVA_DESIGN_VERSION_MAJOR&&
                  nova_design_components()->button_height_dlu==48&&
                  nova_design_components()->button_radius_dlu==12&&
                  nova_design_typography()->heading_dlu==32&&
                  nova_design_motion()->dialog_ms==180&&
                  nova_design_effects()->glass_blur_dlu==12&&
                  nova_design_validate_resources(),
                  "Versionierte Nova Design Language und gemeinsame DLU-Tokens");
    const nova_visual_continuity_context_t *continuity=
        nova_visual_continuity_current();
    nova_component_descriptor_t continuity_component={
        .design_language_version=continuity?continuity->design_language_version:0,
        .registry_signature=continuity?continuity->registry_signature:0,
        .typography_count=NOVA_TYPOGRAPHY_COUNT,.spacing_count=NOVA_SPACING_COUNT,
        .radius_count=NOVA_RADIUS_COUNT,.icon_count=NOVA_ICON_COUNT,
        .material_count=NOVA_DESIGN_MATERIAL_COUNT,.motion_count=NOVA_TRANSITION_COUNT,
        .interaction_count=NOVA_INTERACTION_COUNT,.semantic_tokens_only=true};
    failed|=check(continuity&&continuity->visual_continuity_enabled&&
        continuity->accessibility_semantics_preserved&&continuity->theme_id!=0&&
        nova_visual_continuity_validate(&continuity_component),
        "Visual-Continuity-Kontext bindet gemeinsame versionierte Registries");
    nova_component_descriptor_t divergent_component=continuity_component;
    divergent_component.motion_count--;
    failed|=check(!nova_visual_continuity_validate(&divergent_component)&&
        nova_visual_continuity_diagnostics()->violations==1&&
        nova_visual_continuity_reload()&&
        nova_visual_continuity_diagnostics()->consistent,
        "abweichende Modulregistry abweisen und Kontext atomar neu laden");
    bool typography_complete=true,spacing_complete=true,radius_complete=true;
    for(uint8_t i=0;i<NOVA_TYPOGRAPHY_COUNT;++i){
        const nova_typography_style_t *style=nova_typography_get((nova_typography_role_t)i);
        if(!nova_typography_exists((nova_typography_role_t)i)||!style||
           style->line_min_dlu>style->line_dlu||style->line_dlu>style->line_max_dlu)
            typography_complete=false;}
    for(uint8_t i=0;i<NOVA_SPACING_COUNT;++i)
        if(!nova_spacing_exists((nova_spacing_token_t)i))spacing_complete=false;
    for(uint8_t i=0;i<NOVA_RADIUS_COUNT;++i)
        if(!nova_radius_exists((nova_radius_token_t)i))radius_complete=false;
    failed|=check(typography_complete&&spacing_complete&&radius_complete&&
        nova_typography_get(NOVA_TYPOGRAPHY_TITLE)->weight==600&&
        nova_spacing_get(NOVA_SPACING_M)==12&&nova_radius_get(NOVA_RADIUS_LARGE)==12,
        "elf Typography-, acht Spacing- und acht Radiusrollen O(1) aufloesen");
    failed|=check(nova_theme_activate(NOVA_THEME_HIGH_CONTRAST)&&
        nova_design_tokens_reload()&&nova_typography_get(NOVA_TYPOGRAPHY_BODY)->size_dlu==16&&
        nova_spacing_get(NOVA_SPACING_M)==14&&nova_radius_get(NOVA_RADIUS_LARGE)==6&&
        nova_theme_activate(NOVA_THEME_DARK)&&nova_design_tokens_reload()&&
        nova_typography_get(NOVA_TYPOGRAPHY_BODY)->size_dlu==14,
        "Theme und Accessibility wechseln semantische Designtokens ohne Controls");
    bool shadows_complete=true,materials_complete=true,motion_complete=true;
    for(uint8_t i=0;i<NOVA_ELEVATION_COUNT;++i)
        if(!nova_shadow_exists((nova_elevation_token_t)i)||
           !nova_shadow_get((nova_elevation_token_t)i))shadows_complete=false;
    for(uint8_t i=0;i<NOVA_DESIGN_MATERIAL_COUNT;++i)
        if(!nova_material_exists((nova_material_token_t)i)||
           !nova_material_get((nova_material_token_t)i))materials_complete=false;
    for(uint8_t i=0;i<NOVA_TRANSITION_COUNT;++i)
        if(!nova_design_motion_exists((nova_transition_token_t)i)||
           !nova_design_motion_get((nova_transition_token_t)i))motion_complete=false;
    failed|=check(shadows_complete&&materials_complete&&motion_complete&&
        nova_shadow_get(NOVA_ELEVATION_LEVEL4)->layer_count==2&&
        nova_material_get(NOVA_DESIGN_MATERIAL_DIALOG)->blur_dlu==12&&
        nova_duration_get(NOVA_DURATION_NORMAL)==180&&
        nova_design_motion_get(NOVA_TRANSITION_FOCUS)->duration_ms==120,
        "Elevation, Glass und Motion Tokens vollstaendig O(1) aufloesen");
    bool states_complete=true;
    for(uint8_t i=0;i<NOVA_INTERACTION_COUNT;++i)
        if(!nova_state_exists((nova_interaction_state_t)i)||
           !nova_state_get((nova_interaction_state_t)i))states_complete=false;
    failed|=check(states_complete&&
        nova_state_get(NOVA_INTERACTION_FOCUSED)->border==nova_theme_tokens()->focus&&
        nova_state_get(NOVA_INTERACTION_DISABLED)->input_blocked&&
        !nova_state_get(NOVA_INTERACTION_READONLY)->input_blocked&&
        nova_state_get(NOVA_INTERACTION_ERROR)->indicator_required&&
        nova_state_transition_allowed(NOVA_INTERACTION_HOVER,NOVA_INTERACTION_PRESSED)&&
        nova_state_transition_allowed(NOVA_INTERACTION_EXPANDED,NOVA_INTERACTION_COLLAPSED)&&
        !nova_state_transition_allowed(NOVA_INTERACTION_HOVER,NOVA_INTERACTION_CHECKED),
        "zwoelf Interaction States, semantische Stile und validierte Uebergaenge");
    failed|=check(nova_theme_activate(NOVA_THEME_HIGH_CONTRAST)&&
        nova_design_tokens_reload()&&nova_shadow_get(NOVA_ELEVATION_LEVEL4)->disabled&&
        nova_material_get(NOVA_DESIGN_MATERIAL_DIALOG)->opaque&&
        nova_design_motion_get(NOVA_TRANSITION_DIALOG)->disabled&&
        nova_duration_get(NOVA_DURATION_VERY_SLOW)==1&&
        nova_theme_activate(NOVA_THEME_DARK)&&nova_design_tokens_reload(),
        "Accessibility deaktiviert Schatten, Glas und Bewegung semantisch");
    nova_design_manifest_t incompatible=*nova_design_manifest();
    incompatible.tokens.major=2;incompatible.checksum=0;
    failed|=check(!nova_design_validate_manifest(&incompatible),
                  "Inkompatible Design-Major-Version sicher abweisen");
    nova_oem_design_t invalid_oem={.accent=0x80267cc1u};
    nova_oem_design_t valid_oem={.accent=0xff267cc1u};
    failed|=check(!nova_design_apply_oem(&invalid_oem)&&
                  nova_design_apply_oem(&valid_oem)&&
                  nova_design_accent()==0xff267cc1u,
                  "OEM-Anpassung auf validierte Marke und Akzent begrenzen");

    nova_architecture_initialize();
    for(uint8_t i=0;i<NOVA_ARCH_SUBSYSTEM_COUNT;++i)
        failed|=check(nova_architecture_register((nova_architecture_subsystem_t)i),
                      "Architekturdeskriptor registrieren");
    failed|=check(!nova_architecture_ready(NOVA_ARCH_APPLICATION),
                  "Architekturabhaengigkeit abweisen");
    static const nova_architecture_subsystem_t architecture_order[]={
        NOVA_ARCH_PLATFORM,NOVA_ARCH_RESOURCE,NOVA_ARCH_GRAPHICS,
        NOVA_ARCH_RENDERER,NOVA_ARCH_SCENE,NOVA_ARCH_LAYOUT,NOVA_ARCH_MOTION,
        NOVA_ARCH_CONTROL,NOVA_ARCH_NAVIGATION,NOVA_ARCH_DIALOG,
        NOVA_ARCH_APPLICATION,NOVA_ARCH_DIAGNOSTICS};
    for(uint8_t i=0;i<sizeof(architecture_order)/sizeof(architecture_order[0]);++i)
        failed|=check(nova_architecture_ready(architecture_order[i]),
                      "Architekturabhaengigkeiten deterministisch aufloesen");
    failed|=check(nova_architecture_validate()&&
                  nova_architecture_diagnostics()->complete&&
                  nova_architecture_descriptor(NOVA_ARCH_RENDERER)->platform_neutral,
                  "Modulares Architekturmanifest validieren");

    nova_unicode_initialize();
    const char *unicode = "AΩЖאあ中";
    uint32_t decoded = 0, cp;
    while (*unicode) { nova_unicode_next(&unicode, &cp); ++decoded; }
    failed |= check(decoded == 6, "UTF-8 ueber mehrere Unicode-Bloecke");
    const char invalid_utf8[] = {(char)0xc0, (char)0xaf, 0};
    const char *invalid_cursor = invalid_utf8;
    failed |= check(!nova_unicode_next(&invalid_cursor, &cp) &&
                    cp == NOVA_UNICODE_REPLACEMENT, "ungueltiges UTF-8 ersetzt");

    nova_controls_initialize(0);
    nova_control_t *state_probe=nova_control_create(NOVA_CONTROL_BUTTON);
    failed|=check(state_probe&&nova_control_set_state(state_probe,NOVA_CONTROL_INITIALIZED)&&
        nova_control_set_state(state_probe,NOVA_CONTROL_VISIBLE)&&
        !nova_control_set_interaction(state_probe,NOVA_INTERACTION_HOVER,true,false)&&
        nova_control_set_interaction(state_probe,NOVA_INTERACTION_HOVER,true,true)&&
        nova_control_interaction(state_probe)==NOVA_INTERACTION_HOVER&&
        nova_control_set_interaction(state_probe,NOVA_INTERACTION_PRESSED,true,false)&&
        nova_control_interaction(state_probe)==NOVA_INTERACTION_PRESSED&&
        nova_control_set_interaction(state_probe,NOVA_INTERACTION_PRESSED,false,false)&&
        nova_control_set_interaction(state_probe,NOVA_INTERACTION_HOVER,false,true)&&
        nova_control_set_interaction(state_probe,NOVA_INTERACTION_EXPANDED,true,false)&&
        nova_control_set_interaction(state_probe,NOVA_INTERACTION_COLLAPSED,true,false)&&
        nova_control_interaction(state_probe)==NOVA_INTERACTION_COLLAPSED,
        "pointergebundener Hover, Pressed und Expanded/Collapsed State");
    failed|=check(nova_control_set_interaction(state_probe,NOVA_INTERACTION_DISABLED,true,false)&&
        !nova_control_invoke(state_probe,0)&&
        nova_control_set_interaction(state_probe,NOVA_INTERACTION_DISABLED,false,false)&&
        nova_control_set_interaction(state_probe,NOVA_INTERACTION_NORMAL,true,false)&&
        nova_control_set_interaction(state_probe,NOVA_INTERACTION_READONLY,true,false)&&
        !nova_control_invoke(state_probe,0)&&
        nova_control_set_interaction(state_probe,NOVA_INTERACTION_NORMAL,true,false)&&
        nova_control_set_interaction(state_probe,NOVA_INTERACTION_LOADING,true,false)&&
        !nova_control_invoke(state_probe,0)&&
        nova_control_set_interaction(state_probe,NOVA_INTERACTION_NORMAL,true,false)&&
        nova_control_set_interaction(state_probe,NOVA_INTERACTION_ERROR,true,false)&&
        state_probe->status_text[0],
        "Disabled, ReadOnly und Loading sperren Eingabe; Error besitzt Textindikator");
    nova_control_t *list = nova_control_create(NOVA_CONTROL_LIST);
    nova_control_t *item = nova_control_create(NOVA_CONTROL_LIST_ITEM);
    failed |= check(list && item && nova_control_set_parent(item, list),
                    "Control-Pool und Parent/Child");
    failed |= check(!nova_control_set_parent(list, item), "Control-Zyklus abweisen");
    failed |= check(nova_control_set_state(item, NOVA_CONTROL_INITIALIZED) &&
                    nova_control_set_state(item, NOVA_CONTROL_VISIBLE),
                    "Control-Lebenszyklus");
    failed |= check(!nova_control_set_state(item, NOVA_CONTROL_CREATED),
                    "ungueltigen Zustandswechsel abweisen");
    failed |= check(nova_control_set_bounds(item, (nova_rect_t){1,1,6,3}) &&
                    nova_control_set_text(item, "NovaOS starten") &&
                    nova_control_set_accessibility(item, 1, "NovaOS starten", false),
                    "Layout, Text und Accessibility");
    failed |= check(nova_control_focus(item) && nova_control_hit_test(2,2) == item,
                    "Fokus und Hit-Test");
    item->action = 42;
    uint32_t action = 0;
    failed |= check(nova_control_invoke(item, &action) && action == 42 &&
                    !nova_control_invoke(item, &action), "Aktion exakt einmal");
    nova_control_release(item);
    failed |= check(nova_control_set_range(item, 0, 1000, 500), "Control-Wertebereich");
    nova_control_render(item, overlay);
    nova_control_t *sw = nova_control_create(NOVA_CONTROL_SWITCH);
    nova_control_t *slider = nova_control_create(NOVA_CONTROL_SLIDER);
    failed |= check(sw && slider &&
                    nova_control_set_state(sw,NOVA_CONTROL_INITIALIZED) &&
                    nova_control_set_state(sw,NOVA_CONTROL_VISIBLE) &&
                    nova_control_set_bounds(sw,(nova_rect_t){10,1,20,10}) &&
                    nova_control_toggle(sw) &&
                    (sw->flags & NOVA_CONTROL_FLAG_CHECKED),
                    "Switch-Zustand und Toggle");
    failed |= check(nova_control_set_state(slider,NOVA_CONTROL_INITIALIZED) &&
                    nova_control_set_state(slider,NOVA_CONTROL_VISIBLE) &&
                    nova_control_set_bounds(slider,(nova_rect_t){10,12,40,12}) &&
                    nova_control_set_range(slider,0,100,50) &&
                    nova_control_set_step(slider,10) &&
                    nova_control_adjust(slider,2) && slider->value==70,
                    "Slider-Schrittweite und begrenzter Wert");
    nova_control_render(sw,overlay);
    nova_control_render(slider,overlay);
    sw->flags |= NOVA_CONTROL_FLAG_LOCKED;
    failed |= check(!nova_control_toggle(sw) &&
                    nova_control_diagnostics()->toggles==1 &&
                    nova_control_diagnostics()->value_changes==1,
                    "gesperrte Controls und Diagnosedaten");
    nova_control_t *field=nova_control_create(NOVA_CONTROL_TEXT_FIELD);
    nova_control_t *password=nova_control_create(NOVA_CONTROL_PASSWORD_FIELD);
    failed |= check(field&&password&&nova_control_set_state(field,NOVA_CONTROL_INITIALIZED)&&
                    nova_control_set_state(field,NOVA_CONTROL_VISIBLE)&&
                    nova_control_set_bounds(field,(nova_rect_t){2,26,70,24})&&
                    nova_text_field_set_placeholder(field,"Suchtext")&&
                    nova_text_field_set_maximum(field,20)&&
                    nova_text_field_insert(field,'N')&&nova_text_field_insert(field,0x00e4)&&
                    field->text_length==3&&field->caret==3&&
                    nova_text_field_move(field,-1,false)&&field->caret==1&&
                    nova_text_field_delete(field)&&field->text_length==1,
                    "UTF-8-Textfeld, Caret und codepointweises Löschen");
    failed |= check(nova_text_field_set_mode(field,NOVA_TEXT_INPUT_NUMERIC)&&
                    !nova_text_field_insert(field,'x')&&nova_text_field_insert(field,'7')&&
                    nova_text_field_select_all(field)&&nova_text_field_backspace(field)&&
                    field->text_length==0,"Textfeldfilter und Auswahl");
    const char invalid_field_text[]={(char)0xc0,(char)0xaf,0};
    failed |= check(!nova_control_set_text(field,invalid_field_text),
                    "Textfeld weist ungültiges UTF-8 ab");
    failed |= check(nova_control_set_state(password,NOVA_CONTROL_INITIALIZED)&&
                    nova_control_set_state(password,NOVA_CONTROL_VISIBLE)&&
                    nova_control_set_bounds(password,(nova_rect_t){2,52,70,24})&&
                    nova_text_field_insert(password,'s')&&password->text_length==1,
                    "Passwortfeld mit geschütztem Festpuffer");
    nova_control_render(field,overlay);nova_control_render(password,overlay);
    nova_text_field_clear(password);
    failed |= check(password->text_length==0&&password->text[0]==0,
                    "Passwortpuffer sicher leeren");
    nova_text_metrics_t text_metrics = nova_text_measure("Ärger mit Öl und Größe", 300);
    failed |= check(text_metrics.width > 0 && !text_metrics.truncated,
                    "UTF-8-Textmessung mit Umlauten");
    nova_text_metrics_t scaled_metrics = nova_text_measure_scaled("NovaOS",300,1500);
    failed |= check(scaled_metrics.width > nova_text_measure("NovaOS",300).width &&
                    scaled_metrics.height == 30,
                    "DLU-skalierte Textmessung");
    nova_text_draw(overlay, 0, 0, 8, "NovaOS", 0xffffffffu,
                   NOVA_TEXT_LEFT, true);
    failed |= check(nova_text_draw_wrapped_scaled(overlay,0,0,40,3,
                    "NovaOS erklärt einen verständlichen Fehlertext",
                    0xffffffffu,1000) >= 2,
                    "mehrzeiliger UTF-8-Dialogtext");
    failed |= check(nova_text_measure("NovaOS", 8).truncated,
                    "Text-Clipping und Ellipsis");
    nova_icon_draw(overlay, NOVA_ICON_SETTINGS, 0, 0, 20, 0xffffffffu);
    failed |= check(nova_icon_diagnostics()->renders == 1 &&
                    nova_icon_exists(NOVA_ICON_POWER), "semantische Icon-Tokens und Cache");

    nova_input_initialize();
    failed |= check(nova_input_device_set(1, NOVA_DEVICE_KEYBOARD, true) &&
                    nova_input_device_set(2, NOVA_DEVICE_MOUSE, true),
                    "Input Device Hot Detection");
    nova_input_event_t key_event = {.timestamp=10,.device_id=1,.type=NOVA_EVENT_KEY_DOWN};
    key_event.key.key = 40;
    nova_input_set_secure(true);
    failed |= check(nova_input_post(key_event), "Event Queue Post");
    nova_input_event_t received;
    failed |= check(nova_input_next(&received) && received.sequence == 1 &&
                    (received.flags & 0x80000000u), "deterministische sichere Event Queue");
    failed |= check(nova_input_shortcut_register(40,0,0,77) &&
                    !nova_input_shortcut_register(40,0,0,88), "Shortcut-Konfliktpruefung");
    uint32_t command = 0;
    failed |= check(nova_input_shortcut_resolve(&received,&command) && command==77,
                    "Shortcut erzeugt Command");
    nova_control_t *item2 = nova_control_create(NOVA_CONTROL_LIST_ITEM);
    nova_control_set_state(item2,NOVA_CONTROL_INITIALIZED);
    nova_control_set_state(item2,NOVA_CONTROL_VISIBLE);
    nova_control_set_bounds(item2,(nova_rect_t){1,5,6,2});
    failed |= check(nova_input_focus_set(item) &&
                    nova_input_neighbor_set(item,NOVA_DIRECTION_DOWN,item2) &&
                    nova_input_focus_navigate(NOVA_DIRECTION_DOWN) &&
                    nova_input_focus_get()==item2, "Directional Focus Navigation");
    failed |= check(nova_input_pointer_capture(9,item2), "Pointer Capture");
    nova_input_event_t pointer={.type=NOVA_EVENT_POINTER_MOVE};
    pointer.pointer.pointer_id=9;pointer.pointer.x=2;pointer.pointer.y=2;
    failed |= check(nova_input_pointer_target(&pointer)==item2, "Capture vor Hit Test");
    nova_input_pointer_release(9);
    failed |= check(nova_input_pointer_target(&pointer)==item, "Hit Test nach Capture Release");
    failed |= check(nova_input_repeat_start(40,100), "Input Repeat starten");
    nova_input_repeat_update(500);
    failed |= check(nova_input_next(&received) && received.key.repeat,
                    "deterministisches Input Repeat");
    failed |= check(nova_input_device_set(2,NOVA_DEVICE_MOUSE,false),
                    "Hot Removal beendet Geraetezustand");

    nova_diag_initialize();
    for(uint32_t i=0;i<NOVA_DIAG_CAPACITY+3u;++i)
        nova_diag_log((nova_diag_event_t){i,NOVA_DIAG_INFO,1,i,0,0});
    failed |= check(nova_diag_statistics()->overwritten==3 &&
                    nova_diag_get(0)->event_id==3, "heapfreier Diagnose-Ringpuffer");
    failed|=check(nova_boot_perf_record_startup(30000,10000,20000,50000,100000)&&
        nova_boot_perf_startup()->valid&&
        !nova_boot_perf_record_startup(30001,10000,20000,50000,100000),
        "UI-, Theme-, Resource-, Window- und First-Frame-Startbudgets");
    nova_diag_frame_extended(16000,400,400,1800,1800,1400,5500,1800,1400);
    failed|=check(!nova_frame_budget_exceeded()&&
        nova_boot_perf_metrics()->frame_class==NOVA_FRAME_CLASS_A&&
        nova_boot_perf_metrics()->fps==62&&nova_boot_perf_metrics()->within_budget,
        "vollständiger 60-FPS-Frame innerhalb aller Teilbudgets");
    nova_diag_frame(40000,1000,1200,20000,8000);
    failed |= check(nova_diag_frame_budget()->violations==1 &&
                    nova_diag_quality()->low_end&&nova_frame_budget_exceeded()&&
                    nova_boot_perf_metrics()->frame_class==NOVA_FRAME_CLASS_E&&
                    nova_boot_perf_metrics()->hard_violations==1&&
                    nova_boot_perf_metrics()->phase_violations==1&&
                    nova_boot_perf_metrics()->minimum_frame_us==16000&&
                    nova_boot_perf_metrics()->maximum_frame_us==40000&&
                    nova_boot_perf_metrics()->average_frame_us==28000,
                    "Framebudget, Phasenverletzung und adaptive Qualitaet");
    nova_diag_set_quality(NOVA_QUALITY_SAFE,false);
    failed |= check(nova_diag_quality()->quality==NOVA_QUALITY_SAFE &&
                    nova_motion_is_reduced() &&
                    nova_compositor_diagnostics()->fallback_level==3,
                    "Safe-Profil reduziert Motion und Materialien");
    failed |= check(nova_diag_memory_budget()->used_memory <
                    nova_diag_memory_budget()->total_budget,
                    "32-MiB-Low-End-Speicherbudget");
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
    failed |= check(nova_compositor_compose(), "Damage compositing im Offscreen-Buffer");
    nova_state_set_phase(NOVA_STATE_PHASE_RENDER);
    failed|=check(!nova_compositor_present()&&
                  nova_present_diagnostics()->recovery_attempts==1&&
                  nova_present_diagnostics()->safe_mode_entries==1,
                  "Presentfehler mit Full-Retry und Safe Mode eskalieren");
    nova_state_set_phase(NOVA_STATE_PHASE_PRESENT);
    failed|=check(nova_compositor_present()&&
                  nova_graphics_diagnostics()->presents==presents_before_compositor+1&&
                  graphics_frontbuffer[2+2*10]!=0,
                  "Pitch-sicheres Present am Frameende");
    nova_compositor_set_fallback(3);
    failed |= check(nova_compositor_diagnostics()->fallback_level == 3,
                    "Material-Fallback");
    failed |= check(nova_compositor_diagnostics()->frames == 1,
                    "Compositor-Diagnose");
    nova_compositor_set_fallback(0);
    nova_surface_clear(base, 0xff000000u);
    nova_surface_clear(overlay, 0x00000000u);
    nova_surface_rect(overlay, (nova_rect_t){0,0,1,1}, 0x80ff0000u);
    modal.material = NOVA_MATERIAL_NONE;
    modal.modal = false;
    failed |= check(nova_compositor_begin_frame() &&
                    nova_compositor_submit_layer(&lower) &&
                    nova_compositor_submit_layer(&modal) &&
                    nova_compositor_compose(), "Alpha-Testframe");
    nova_state_set_phase(NOVA_STATE_PHASE_PRESENT);
    failed|=check(nova_compositor_present(),"Alpha-Testframe praesentieren");
    failed |= check(graphics_frontbuffer[0] == 0xff800000u,
                    "kanalgetreues Alpha-Compositing");

    if (!failed) puts("BOOT_UI_RUNTIME_OK");
    return failed ? 1 : 0;
}
