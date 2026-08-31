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
#include "../boot/bootloader/bootmenu/vector_geometry.h"
#include "../boot/bootloader/bootmenu/svg_renderer.h"
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
#include "../boot/bootloader/bootmenu/test_architecture.h"

static uint32_t runtime_task_order[4],runtime_task_count;
static uint32_t state_callback_count;
static uint16_t scene_visit_order[8],scene_visit_count;
static uint16_t control_event_order[8],control_event_count;
static nova_control_event_type_t control_event_types[8];
static uint32_t input_scan_count;
static bool input_scan_probe(void *context)
{++input_scan_count;return context==(void *)&input_scan_count;}
typedef struct {uint32_t command,calls;const nova_input_event_t *event;bool recurse;} shortcut_probe_t;
static bool shortcut_command_probe(uint32_t command,void *context)
{shortcut_probe_t *probe=(shortcut_probe_t *)context;probe->command=command;++probe->calls;
 if(probe->recurse)return nova_input_shortcut_execute(probe->event);
 return true;}
static bool control_event_handler(nova_control_t *control,nova_control_event_t *event,void *context)
{bool handle=*(bool *)context;if(control_event_count<8){control_event_order[control_event_count]=control->id;
 control_event_types[control_event_count++]=event->type;}
 return handle&&event->type==NOVA_CONTROL_EVENT_CLICK;}
static void runtime_task(uint32_t id,void *context)
{(void)context;if(runtime_task_count<4)runtime_task_order[runtime_task_count++]=id;}
static void state_callback(const nova_state_object_t *object,uint8_t old_state,
                           uint8_t new_state,void *context)
{(void)object;(void)old_state;(void)new_state;(void)context;++state_callback_count;}
static uint32_t graphics_frontbuffer[80];
static uint64_t present_test_clock;
static uint64_t architecture_test_clock,architecture_test_state=0x4e4f5641u;
typedef struct {uint32_t actual,error;nova_test_status_t status;bool mutate;} architecture_probe_t;
static uint64_t architecture_clock(void){architecture_test_clock+=25;return architecture_test_clock;}
static uint64_t architecture_state(void){return architecture_test_state;}
static nova_test_status_t architecture_execute(void *context,uint32_t *actual,uint32_t *error)
{architecture_probe_t *probe=(architecture_probe_t *)context;*actual=probe->actual;*error=probe->error;
 if(probe->mutate)++architecture_test_state;
 return probe->status;}
static uint64_t test_clock_us(void *context)
{(void)context;present_test_clock+=50;return present_test_clock;}
static uint8_t test_vector_mask(int32_t x,int32_t y,void *context)
{(void)context;return ((x+y)&1)?128u:255u;}

static int check(int condition, const char *message)
{
    if (!condition) { fprintf(stderr, "FAIL: %s\n", message); return 1; }
    return 0;
}
static void put16(uint8_t *p,uint16_t v){p[0]=(uint8_t)v;p[1]=(uint8_t)(v>>8);}
static void put32(uint8_t *p,uint32_t v)
{for(uint8_t i=0;i<4;++i)p[i]=(uint8_t)(v>>(i*8));}
static void put64(uint8_t *p,uint64_t v)
{for(uint8_t i=0;i<8;++i)p[i]=(uint8_t)(v>>(i*8));}
static uint32_t crc_zero(const uint8_t *p,uint32_t size,uint32_t zero)
{uint32_t crc=0xffffffffu;for(uint32_t i=0;i<size;++i){uint8_t v=i>=zero&&i<zero+4?0:p[i];
 crc^=v;for(uint8_t bit=0;bit<8;++bit)crc=(crc>>1)^(0xedb88320u&(0u-(crc&1u)));}return ~crc;}
static void make_bap(uint8_t package[192])
{memset(package,0,192);memcpy(package,"NOVABAP",7);put16(package+8,1);put16(package+10,64);
 put32(package+12,192);put64(package+16,0x1234);put32(package+28,1);
 put32(package+32,64);put32(package+36,112);put32(package+40,176);put32(package+44,8);
 put32(package+48,184);put32(package+52,8);
 uint8_t *index=package+64;memcpy(index,"NOVAIDX",7);put16(index+8,1);put16(index+10,40);
 put16(index+12,64);put16(index+14,1);put32(index+16,2);put32(index+20,40);put32(index+24,104);
 uint64_t id=0x1001;uint8_t *entry=index+40;put64(entry,id);put32(entry+8,NOVA_RESOURCE_ICON);
 put32(entry+16,1);put32(entry+20,NOVA_COMPRESSION_NONE);put64(entry+24,176);
 put64(entry+32,4);put64(entry+40,4);package[176]=1;package[177]=2;package[178]=3;package[179]=4;
 put32(entry+48,nova_resource_checksum(package+176,4));put32(index+104,UINT32_MAX);
 put32(index+108,UINT32_MAX);put32(index+104+((uint32_t)id&1u)*4u,0);
 put32(index+28,crc_zero(index,112,28));put32(package+56,crc_zero(package,192,56));}

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
    static const char svg_ok[]="<svg viewBox='0 0 4 4'><path fill='#FFFFFFFF' d='M0 0 L4 0 L4 4 L0 4 Z'/></svg>";
    static const char svg_bad[]="<svg><script/></svg>";uint32_t svg_pixels[16]={0};
    static const char svg_shapes[]="<svg viewBox='0 0 8 8'><rect x='0' y='0' width='4' height='4' fill='#FF000080'/><circle cx='6' cy='6' r='2' fill='var(--nova-accent)'/></svg>";
    nova_surface_t svg_surface={.pixels=svg_pixels,.width=4,.height=4,.stride=4};
    failed|=check(nova_svg_initialize()&&nova_svg_validate(svg_ok,sizeof(svg_ok)-1)&&
        !nova_svg_validate(svg_bad,sizeof(svg_bad)-1)&&
        nova_svg_render(svg_ok,sizeof(svg_ok)-1,&svg_surface,(nova_rect_t){0,0,4,4},0)&&
        svg_pixels[0]==0xffffffffu&&nova_svg_diagnostics()->renders==1,
        "SVG-Untermenge validiert und rasterisiert Pfade deterministisch");
    for(uint8_t i=0;i<16;++i)svg_pixels[i]=0;
    failed|=check(nova_svg_validate(svg_shapes,sizeof(svg_shapes)-1)&&
        nova_svg_render(svg_shapes,sizeof(svg_shapes)-1,&svg_surface,(nova_rect_t){0,0,4,4},0)&&
        svg_pixels[0]==0x80800000u&&svg_pixels[15]!=0,
        "SVG viewBox skaliert Rechteck, Kreis, Alpha und Themefarbe");
    uint8_t bap[192];nova_bap_info_t bap_info;uint16_t bap_entries=0;make_bap(bap);
    failed|=check(nova_bap_validate(bap,sizeof(bap),&bap_info)&&bap_info.resource_count==1&&
        nova_bap_index_validate(bap+64,112,&bap_entries)&&bap_entries==1,
        "BAP-1-Header, Index, Hash und Ressourcendaten werden validiert");
    bap[64+104+((0x1001u)&1u)*4u]=0xff;
    failed|=check(!nova_bap_index_validate(bap+64,112,0),
        "Ein fehlender Resource-Index-Hash-Eintrag wird abgelehnt");
    nova_vector_path_t vector_path;
    nova_vector_path_reset(&vector_path);
    failed|=check(nova_vector_move_to(&vector_path,0,0)&&
        nova_vector_line_to(&vector_path,2<<16,0)&&
        nova_vector_line_to(&vector_path,2<<16,2<<16)&&
        nova_vector_line_to(&vector_path,0,2<<16)&&nova_vector_close(&vector_path)&&
        nova_vector_coverage(&vector_path,0,0,NOVA_VECTOR_NONZERO)==255&&
        nova_vector_coverage(&vector_path,3,3,NOVA_VECTOR_NONZERO)==0,
        "Vector Geometry rasterisiert einen geschlossenen 16.16-DLU-Pfad");
    nova_vector_path_reset(&vector_path);
    failed|=check(nova_vector_move_to(&vector_path,0,0)&&
        nova_vector_quad_to(&vector_path,1<<16,2<<16,2<<16,0)&&
        nova_vector_cubic_to(&vector_path,3<<16,-(2<<16),4<<16,2<<16,5<<16,0)&&
        vector_path.count==40&&!vector_path.overflow,
        "Quadratische und kubische Kurven werden deterministisch abgeflacht");
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
        NOVA_BLUR_KERNEL_DUAL_PASS_GAUSSIAN,&blur_rounding,0,1};
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
    ++blur_region.source_generation;blur_source_pixels[6*16+6]=0xff808080u;
    failed|=check(nova_blur_render(&blur_region,&blur_source,&blur_destination)==NOVA_BLUR_OK&&
        nova_blur_diagnostics()->cache_misses==2,
        "Neue Basisebenengeneration verwirft geometrisch passenden Blur-Cache");
    nova_rect_t unrelated_blur_dirty={0,0,1,1},related_blur_dirty={5,5,2,2};
    nova_blur_invalidate(&unrelated_blur_dirty);
    failed|=check(nova_blur_diagnostics()->cache_valid,
        "Fremde Dirty Region behaelt Blur-Cache");
    nova_blur_invalidate(&related_blur_dirty);
    failed|=check(!nova_blur_diagnostics()->cache_valid&&
       nova_blur_render(&blur_region,&blur_source,&blur_destination)==NOVA_BLUR_OK&&
        nova_blur_diagnostics()->cache_misses==3,
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
                    configuration->mouse_speed_percent==100&&
                    configuration->double_click_time_ms==500&&
                    nova_configuration_validate(configuration)&&
                    configuration->checksum==nova_configuration_checksum(configuration),
                    "Versionierte sichere Konfigurationsdefaults");
    failed |= check(nova_configuration_begin()&&
                    nova_configuration_set(NOVA_CONFIG_THEME,NOVA_THEME_LIGHT)&&
                    nova_configuration_set(NOVA_CONFIG_TOOLTIP_DELAY,1000)&&
                    nova_configuration_set(NOVA_CONFIG_MOUSE_SPEED,175)&&
                    nova_configuration_set(NOVA_CONFIG_DOUBLE_CLICK_TIME,600)&&
                    !nova_configuration_begin()&&nova_configuration_commit()&&
                    nova_configuration_get()->theme==NOVA_THEME_LIGHT&&
                    nova_configuration_get()->tooltip_delay_ms==1000&&
                    nova_configuration_get()->mouse_speed_percent==175&&
                    nova_configuration_get()->double_click_time_ms==600&&
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

    failed|=check(nova_navigation_test_initialize(),"Navigationstestsuite initialisieren");
    for(uint32_t route=0;route<NOVA_NAVIGATION_TEST_CAPACITY;++route)
        failed|=check(nova_navigation_test_execute(route),"Navigationsroute isoliert pruefen");
    const nova_navigation_test_summary_t *navigation_suite=nova_navigation_test_summary();
    failed|=check(navigation_suite->count==NOVA_NAVIGATION_TEST_CAPACITY&&
        navigation_suite->passed==8&&!navigation_suite->failed&&navigation_suite->skipped==2&&
        navigation_suite->isolated&&navigation_suite->deterministic&&
        navigation_suite->configuration_unchanged,
        "Acht Navigationsrouten bestanden und zwei Backends ehrlich uebersprungen");
    static uint8_t navigation_test_report[2048];uint32_t navigation_test_report_size=0;
    failed|=check(!nova_navigation_test_generate_report(false,navigation_test_report,
            sizeof(navigation_test_report),&navigation_test_report_size)&&
        nova_navigation_test_generate_report(true,navigation_test_report,
            sizeof(navigation_test_report),&navigation_test_report_size)&&
        navigation_test_report_size>300&&nova_navigation_test_summary()->reports==1,
        "Navigationstestbericht nur autorisiert und vollstaendig");

    nova_dialog_initialize();
    nova_dialog_t *confirm = nova_confirmation_dialog_show(NOVA_CONFIRM_CRITICAL,
        "Ausschalten", "NovaOS jetzt sicher herunterfahren?", "Ausschalten",
        NOVA_DIALOG_RESULT_YES,17);
    failed |= check(confirm && confirm->state == NOVA_DIALOG_ACTIVE &&
                    confirm->confirmation_validated&&confirm->confirmation_level==NOVA_CONFIRM_CRITICAL&&
                    confirm->button_count==2&&confirm->focus==0&&
                    confirm->button_results[0]==NOVA_DIALOG_RESULT_CANCEL&&
                    nova_modal_dialog_active()&&nova_modal_dialog_backdrop_active()&&
                    nova_modal_dialog_input_allowed(confirm->id)&&
                    !nova_modal_dialog_input_allowed(confirm->id+1)&&
                    !nova_modal_dialog_navigation_allowed(),
                    "modaler Dialog mit vollstaendigem Lifecycle");
    failed |= check(nova_dialog_focus_move(1) && confirm->focus == 1,
                    "Dialog Fokusfang");
    nova_dialog_result_t dialog_result = NOVA_DIALOG_RESULT_NONE;
    failed |= check(!nova_dialog_activate(&dialog_result) &&
                    confirm->destructive_armed == 1 &&
                    dialog_result == NOVA_DIALOG_RESULT_NONE,
                    "zerstoerende Aktion verlangt zweite Bestaetigung");
    failed |= check(nova_dialog_activate(&dialog_result) &&
                    dialog_result == NOVA_DIALOG_RESULT_YES &&
                    nova_dialog_result(confirm)==NOVA_DIALOG_RESULT_YES&&
                    nova_confirmation_dialog_result()==NOVA_DIALOG_RESULT_YES&&
                    nova_dialog_diagnostics()->confirmations==1&&
                    nova_dialog_diagnostics()->confirmation_decisions==1&&
                    confirm->lifecycle_mask==0x7fu&&
                    !nova_dialog_diagnostics()->modal_active,
                    "Dialog liefert genau ein Ergebnis");
    failed |= check(!nova_dialog_activate(&dialog_result)&&
                    nova_confirmation_dialog_result()==NOVA_DIALOG_RESULT_YES,
                    "Bestätigung kann nicht automatisch oder doppelt ausgelöst werden");
    failed |= check(!nova_confirmation_dialog_show((nova_confirmation_level_t)4,
                    "Ungültig","Ungültige Stufe","OK",NOVA_DIALOG_RESULT_OK,0),
                    "Ungültige Bestätigungsstufe wird sicher abgewiesen");
    for(uint8_t level=0;level<4;++level){
        nova_dialog_t *level_dialog=nova_confirmation_dialog_show(
            (nova_confirmation_level_t)level,"Sicherheitsprüfung",
            "Auswirkungen vor der Entscheidung prüfen.",
            level?"Fortfahren":"OK",level?NOVA_DIALOG_RESULT_CONTINUE:
            NOVA_DIALOG_RESULT_OK,19);
        failed|=check(level_dialog&&level_dialog->confirmation_validated&&
            level_dialog->confirmation_level==(nova_confirmation_level_t)level&&
            level_dialog->focus==0&&
            level_dialog->button_results[0]==(level?NOVA_DIALOG_RESULT_CANCEL:
                                                   NOVA_DIALOG_RESULT_OK)&&
            nova_modal_dialog_back(&dialog_result)&&
            dialog_result==NOVA_DIALOG_RESULT_CANCEL,
            "Alle Warnstufen besitzen modale Sperre und sichere Standardaktion");
    }
    failed|=check(nova_dialog_diagnostics()->confirmations==5&&
        nova_dialog_diagnostics()->confirmation_decisions==5&&
        nova_dialog_diagnostics()->confirmation_errors==1&&
        nova_dialog_diagnostics()->last_confirmation_level==NOVA_CONFIRM_SECURITY&&
        nova_confirmation_dialog_result()==NOVA_DIALOG_RESULT_CANCEL,
        "Bestätigungsdiagnose erfasst Stufe, Entscheidung und Fehler");
    nova_dialog_tick(10);
    nova_dialog_t *nonmodal_first=nova_dialog_open(NOVA_DIALOG_INFORMATION,
        "Hinweis 1","Nichtmodal",false,true,31);
    nova_dialog_t *nonmodal_second=0;
    failed |= check(nonmodal_first&&nonmodal_first->root_view_id&&
        nonmodal_first->navigation_context_id&&
        nova_dialog_create(NOVA_DIALOG_CUSTOM,false,true,32,&nonmodal_second)&&
        nonmodal_second->state==NOVA_DIALOG_CREATED&&nonmodal_second->lifecycle_mask==1u&&
        nova_dialog_show(nonmodal_second)&&nova_dialog_active()==nonmodal_second&&
        nova_dialog_diagnostics()->depth==2&&!nova_dialog_diagnostics()->modal_active,
        "Nichtmodale Dialoge besitzen Root View, Navigationskontext und eigenen Stack");
    nova_dialog_tick(15);
    failed |= check(nova_dialog_close(nonmodal_second,NOVA_DIALOG_RESULT_OK,&dialog_result)&&
        dialog_result==NOVA_DIALOG_RESULT_OK&&nonmodal_second->closed_ms==25&&
        nova_dialog_active()==nonmodal_first&&
        nova_dialog_close(nonmodal_first,NOVA_DIALOG_RESULT_CANCEL,&dialog_result)&&
        !nova_dialog_active()&&nova_dialog_diagnostics()->nonmodal_opened==2,
        "Nichtmodaler Stack schließt O(1) von oben und stellt Vorgänger wieder her");
    nova_dialog_t *warning = nova_warning_dialog_show(NOVA_WARNING_HIGH,
        NOVA_WARNING_STORAGE,"Nicht verfuegbar","Backend fehlt",
        "Datenträger unverändert lassen.",1205,true,22);
    nova_dialog_t *blocked_nonmodal=0;
    failed |= check(warning&&warning->warning_validated&&warning->warning_level==NOVA_WARNING_HIGH&&
        warning->warning_icon==NOVA_WARNING_STORAGE&&warning->warning_code==1205&&
        warning->focus==0&&warning->button_results[0]==NOVA_DIALOG_RESULT_CANCEL&&
        !nova_dialog_create(NOVA_DIALOG_INFORMATION,false,true,0,
                    &blocked_nonmodal)&&nova_dialog_active()==warning,
                    "Aktiver modaler Dialog behält höchste Stack- und Fokuspriorität");
    failed |= check(nova_modal_dialog_back(&dialog_result) &&
                    dialog_result == NOVA_DIALOG_RESULT_CANCEL&&
                    nova_warning_dialog_result()==NOVA_DIALOG_RESULT_CANCEL&&
                    !nova_modal_dialog_backdrop_active(),
                    "Back schließt ausschließlich den obersten modalen Dialog und entfernt Backdrop");
    for(uint8_t level=0;level<4;++level){
        nova_dialog_t *level_warning=nova_warning_dialog_show((nova_warning_level_t)level,
            (nova_warning_icon_t)(level+1),"Warnstufentest","Risiko erkannt.",
            "Sichere Maßnahme wählen.",2000u+level,(level&1u)!=0,23);
        failed|=check(level_warning&&level_warning->warning_validated&&
            level_warning->focus==0&&level_warning->button_results[0]==
                (level?NOVA_DIALOG_RESULT_CANCEL:NOVA_DIALOG_RESULT_OK),
            "Warnstufe besitzt sichere Standardaktion");
        failed|=check(nova_dialog_close(level_warning,
            level?NOVA_DIALOG_RESULT_CANCEL:NOVA_DIALOG_RESULT_OK,&dialog_result),
            "Modale und nichtmodale Warnung schließen deterministisch");
    }
    nova_dialog_t *fallback_warning=nova_warning_dialog_show((nova_warning_level_t)9,
        (nova_warning_icon_t)9,0,0,0,99,true,24);
    failed|=check(fallback_warning&&fallback_warning->warning_validated&&
        fallback_warning->warning_level==NOVA_WARNING_MEDIUM&&
        fallback_warning->warning_icon==NOVA_WARNING_GENERAL&&
        nova_dialog_cancel(&dialog_result)&&nova_dialog_diagnostics()->warnings==6&&
        nova_dialog_diagnostics()->warning_decisions==6&&
        nova_dialog_diagnostics()->warning_errors==1,
        "Beschädigte Warning-Daten erzeugen sichere generische Warnung und Diagnose");
    nova_dialog_t *progress_dialog = nova_dialog_open(NOVA_DIALOG_PROGRESS,
        "Diagnose", "Komponenten werden geprüft.", true, false, 22);
    failed |= check(progress_dialog &&
                    nova_dialog_progress_update(progress_dialog,-10,false,"Start") == false &&
                    progress_dialog->progress_per_mille == 0 &&
                    nova_dialog_progress_update(progress_dialog,1000,false,"Abgeschlossen") &&
                    progress_dialog->progress_per_mille == 1000,
                    "Progress wird begrenzt und aktualisiert");
    failed |= check(!nova_modal_dialog_back(&dialog_result)&&
                    nova_dialog_active()==progress_dialog&&
                    nova_dialog_diagnostics()->blocked_navigation>=2,
                    "Nicht abbrechbarer Sicherheitsdialog blockiert Back und Seitennavigation");
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
    failed|=check(nova_dialog_test_initialize(),"Dialogtestsuite initialisieren");
    for(uint8_t type=NOVA_DIALOG_INFORMATION;type<=NOVA_DIALOG_CUSTOM;++type)
        failed|=check(nova_dialog_test_execute((nova_dialog_type_t)type),
                      "Dialogtyp isoliert pruefen");
    const nova_dialog_test_summary_t *dialog_suite=nova_dialog_test_summary();
    failed|=check(dialog_suite->count==NOVA_DIALOG_TEST_CAPACITY&&
        dialog_suite->passed==NOVA_DIALOG_TEST_CAPACITY&&!dialog_suite->failed&&
        !dialog_suite->skipped&&dialog_suite->isolated&&dialog_suite->deterministic&&
        dialog_suite->configuration_unchanged&&!nova_dialog_active(),
        "Zehn Dialogtypen deterministisch und zustandsneutral");
    static uint8_t dialog_test_report[2048];uint32_t dialog_test_report_size=0;
    failed|=check(!nova_dialog_test_generate_report(false,dialog_test_report,
            sizeof(dialog_test_report),&dialog_test_report_size)&&
        nova_dialog_test_generate_report(true,dialog_test_report,sizeof(dialog_test_report),
            &dialog_test_report_size)&&dialog_test_report_size>300&&
        nova_dialog_test_summary()->reports==1,
        "Dialogtestbericht nur autorisiert und vollstaendig");
    failed|=check(nova_control_test_initialize(),"Control-Interaction-Suite initialisieren");
    for(uint32_t control_case=0;control_case<NOVA_CONTROL_TEST_CAPACITY;++control_case){
        bool control_case_ok=nova_control_test_execute(control_case);
        if(!control_case_ok)printf("CONTROL_TEST_CASE_FAILED:%u errors=%u\n",control_case,
            nova_control_test_results()[control_case].detected_errors);
        failed|=check(control_case_ok,"Control-Interaktion isoliert pruefen");
    }
    const nova_control_test_summary_t *control_suite=nova_control_test_summary();
    failed|=check(control_suite->count==NOVA_CONTROL_TEST_CAPACITY&&control_suite->passed==10&&
        !control_suite->failed&&control_suite->skipped==2&&control_suite->isolated&&
        control_suite->deterministic&&control_suite->configuration_unchanged,
        "Zehn Control-Faelle bestanden und zwei fehlende Typen uebersprungen");
    static uint8_t control_test_report[2048];uint32_t control_test_report_size=0;
    failed|=check(!nova_control_test_generate_report(false,control_test_report,
            sizeof(control_test_report),&control_test_report_size)&&
        nova_control_test_generate_report(true,control_test_report,sizeof(control_test_report),
            &control_test_report_size)&&control_test_report_size>350&&
        nova_control_test_summary()->reports==1,
        "Control-Testbericht nur autorisiert und vollstaendig");
    nova_layout_initialize();
    nova_resolution_initialize();
    failed|=check(nova_resolution_configure(1920,1080,96,false)&&
                  nova_resolution_viewport()->scale_milli==1000&&
                  nova_resolution_viewport()->resolution_class==NOVA_RESOLUTION_XL&&
                  nova_resolution_viewport()->aspect_ratio==NOVA_ASPECT_16_9&&
                  nova_resolution_viewport()->logical_width_dlu==1920,
                  "Full-HD-Referenz verwendet 100 Prozent und DLU-Viewport");
    failed|=check(nova_safe_area_get()->x==96&&nova_safe_area_get()->y==54&&
        nova_safe_area_get()->width==1728&&nova_safe_area_get()->height==972&&
        nova_safe_area_contains((nova_rect_t){96,54,100,100})&&
        !nova_safe_area_contains((nova_rect_t){0,0,100,100}),
        "Standard-Safe-Area reserviert an allen Kanten exakt fünf Prozent");
    nova_safe_area_policy_t safe_policy={.display_kind=NOVA_DISPLAY_UEFI_GOP,
        .left_permille=50,.top_permille=50,.right_permille=50,.bottom_permille=50};
    failed|=check(nova_safe_area_update(&safe_policy)&&nova_safe_area_get()->x==0&&
        nova_safe_area_get()->y==0&&nova_safe_area_get()->width==1920&&
        nova_safe_area_get()->height==1080,
        "UEFI kann die optionale Sicherheitsreserve kontrolliert deaktivieren");
    failed|=check(nova_resolution_configure(800,600,96,false),
        "Safe-Area-Test wechselt auf BIOS-Referenzmodus");
    safe_policy=(nova_safe_area_policy_t){.display_kind=NOVA_DISPLAY_BIOS_VBE,
        .firmware_pixels={80,20,10,10},.left_permille=50,.top_permille=50,
        .right_permille=50,.bottom_permille=50,.reserve_enabled=true,.overscan=true};
    failed|=check(nova_safe_area_update(&safe_policy)&&nova_safe_area_get()->x==80&&
        nova_safe_area_get()->y==42&&nova_safe_area_get()->width==664&&
        nova_safe_area_get()->height==516&&nova_safe_area_diagnostics()->overscan,
        "BIOS-Overscan kombiniert sieben Prozent mit asymmetrischen Firmware-Insetdaten");
    safe_policy.accessibility=true;
    failed|=check(nova_safe_area_update(&safe_policy)&&nova_safe_area_get()->x==80&&
        nova_safe_area_get()->y==33&&nova_safe_area_get()->width==676&&
        nova_safe_area_get()->height==534&&
        nova_safe_area_diagnostics()->accessibility_changes>=1,
        "Accessibility erweitert die nutzbare Safe Area ohne Firmware-Overscan zu verletzen");
    safe_policy.firmware_pixels.left=-1;
    failed|=check(!nova_safe_area_update(&safe_policy)&&nova_safe_area_get()->x==0&&
        nova_safe_area_get()->y==0&&nova_safe_area_diagnostics()->fallback_active,
        "Beschädigte Safe-Area-Daten fallen auf die vollständige Bildschirmfläche zurück");
    failed|=check(nova_safe_area_initialize()&&
        nova_resolution_configure(1920,1080,96,false),
        "Standard-Safe-Area wird nach isolierten Policytests wiederhergestellt");
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
    nova_boot_layout_t responsive_layout;nova_rect_t fitted_dialog;
    failed|=check(nova_resolution_configure(640,480,96,false)&&
        nova_resolution_set_scale(1000)&&nova_layout_compute(640,480,false,&responsive_layout)&&
        nova_responsive_class()==NOVA_LAYOUT_COMPACT&&
        nova_responsive_state()->panel_flow==NOVA_RESPONSIVE_STACKED&&
        nova_responsive_state()->spacing_dlu==6&&
        nova_responsive_fit_dialog((nova_rect_t){0,0,2000,2000},&fitted_dialog)&&
        fitted_dialog.x>=nova_resolution_viewport()->safe_pixels.x&&
        fitted_dialog.y>=nova_resolution_viewport()->safe_pixels.y&&
        fitted_dialog.x+fitted_dialog.width<=nova_resolution_viewport()->safe_pixels.x+
            nova_resolution_viewport()->safe_pixels.width&&
        fitted_dialog.y+fitted_dialog.height<=nova_resolution_viewport()->safe_pixels.y+
            nova_resolution_viewport()->safe_pixels.height,
        "Compact-Reflow und Dialog-Fit halten kleine Displays bedienbar");
    failed|=check(nova_resolution_configure(800,600,96,false)&&
        nova_resolution_set_scale(1000)&&nova_responsive_update(false)&&
        nova_responsive_class()==NOVA_LAYOUT_STANDARD&&
        nova_responsive_state()->panel_flow==NOVA_RESPONSIVE_SIDE_BY_SIDE&&
        nova_resolution_configure(1280,720,96,false)&&nova_resolution_set_scale(1000)&&
        nova_responsive_update(false)&&nova_responsive_class()==NOVA_LAYOUT_WIDE,
        "DLU-Breakpoints wechseln exakt bei 800 und 1280");
    failed|=check(nova_resolution_configure(900,700,96,false)&&
        nova_resolution_set_scale(1000)&&nova_responsive_update(true)&&
        nova_responsive_class()==NOVA_LAYOUT_STANDARD&&
        nova_responsive_state()->panel_flow==NOVA_RESPONSIVE_STACKED&&
        nova_responsive_state()->spacing_dlu==10&&
        nova_responsive_diagnostics()->accessibility_changes>=1,
        "Accessibility bleibt responsiv und erzwingt Reflow statt eigenes Layout");
    failed|=check(nova_resolution_configure(2560,1080,96,false)&&
        nova_resolution_set_scale(1000)&&nova_responsive_update(false)&&
        nova_responsive_state()->ultrawide&&
        nova_responsive_state()->panel_flow==NOVA_RESPONSIVE_SIDE_BY_SIDE&&
        nova_responsive_diagnostics()->breakpoint_changes>=2,
        "Ultrawide verwendet dieselbe responsive Policy und stabile Breakpoints");
    failed|=check(nova_resolution_configure(3840,2160,96,false)&&
        nova_resolution_set_automatic(),
        "Responsive Einzeltests stellen den UHD-Automatikzustand wieder her");
    const uint32_t compatibility_resolutions[][2]={{640,480},{800,600},{1024,768},
        {1280,720},{1280,800},{1366,768},{1600,900},{1920,1080},{2560,1440},
        {3840,2160},{1280,1024},{2560,1080},{3840,1080}};
    const uint32_t compatibility_scales[]={100,125,150,175,200};
    failed|=check(nova_resolution_test_initialize(),"Resolution-Testpool initialisieren");
    for(uint8_t resolution=0;resolution<sizeof(compatibility_resolutions)/
        sizeof(compatibility_resolutions[0]);++resolution)
        for(uint8_t scale=0;scale<sizeof(compatibility_scales)/sizeof(compatibility_scales[0]);++scale)
            failed|=check(nova_resolution_test_execute(compatibility_resolutions[resolution][0],
                compatibility_resolutions[resolution][1],compatibility_scales[scale]),
                "Aufloesung und Skalierung ohne Clipping oder unerreichbare Controls");
    const nova_resolution_test_status_t *resolution_test=nova_resolution_test_status();
    failed|=check(resolution_test->executed==65&&resolution_test->passed==65&&
        !resolution_test->failed&&resolution_test->isolated&&
        resolution_test->deterministic&&resolution_test->configuration_unchanged,
        "65er Matrix fuer Pflichtaufloesungen, 5:4, 21:9, 32:9 und High DPI");
    static uint8_t resolution_report[16384];uint32_t resolution_report_size=0;
    failed|=check(!nova_resolution_test_generate_report(false,resolution_report,
            sizeof(resolution_report),&resolution_report_size)&&
        nova_resolution_test_generate_report(true,resolution_report,sizeof(resolution_report),
            &resolution_report_size)&&resolution_report_size>1000&&
        nova_resolution_test_status()->reports==1,
        "Autorisierter CI-Bericht fuer die Resolution-Kompatibilitaetsmatrix");
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
    failed|=check(nova_resolution_configure(1920,1080,120,true)&&
        nova_scaling_get()->dpi_x==120&&nova_scaling_get()->dpi_y==120&&
        nova_scaling_get()->global_scale_milli==1250&&
        nova_scale_to_pixels(10)==13&&nova_scaling_update(true)&&
        nova_scaling_get()->accessibility_scale_milli==1250&&
        nova_scaling_get()->effective_scale_milli==1562&&nova_scale_to_pixels(10)==16,
        "Firmware-DPI und nachgelagerte Accessibility-Skalierung werden getrennt geführt");
    int32_t snapped_start=0,snapped_extent=0,next_start=0,next_extent=0;
    failed|=check(nova_scale_interval(1,1,&snapped_start,&snapped_extent)&&
        nova_scale_interval(2,1,&next_start,&next_extent)&&
        snapped_start+snapped_extent==next_start&&snapped_extent>=0&&next_extent>=0,
        "Kantenbasiertes Pixel-Snapping erzeugt keine Lücken zwischen Nachbarn");
    static const uint32_t scaling_steps[]={1000,1250,1500,1750,2000,2500,3000,1375};
    bool scaling_steps_valid=true;
    for(uint8_t i=0;i<sizeof(scaling_steps)/sizeof(scaling_steps[0]);++i)
        if(!nova_resolution_set_scale(scaling_steps[i])||
           nova_scaling_get()->global_scale_milli!=scaling_steps[i])scaling_steps_valid=false;
    failed|=check(scaling_steps_valid&&nova_scaling_update(false)&&
        nova_scale_to_pixels(INT32_MAX)==INT32_MAX&&
        nova_scaling_diagnostics()->overflows>=1&&
        nova_scaling_diagnostics()->rounding_events>=1,
        "Normative und benutzerdefinierte Skalierungsstufen erkennen Rundung und Überlauf");
    failed|=check(nova_resolution_configure(1920,1080,900,true)&&
        nova_scaling_get()->dpi_x==96&&!nova_scaling_get()->dpi_reliable&&
        nova_scaling_get()->fallback_active,
        "Unglaubwürdige Firmware-DPI fällt explizit auf 96 DPI zurück");
    failed|=check(nova_resolution_configure(1920,1080,96,false)&&
        nova_resolution_set_automatic(),
        "Skalierungstests stellen den automatischen Referenzzustand wieder her");
    failed |= check(nova_dlu_to_pixels(10,1250)==13 &&
                    nova_dlu_to_pixels(-10,1250)==-13,
                    "deterministische DLU-Rundung");
    nova_rect_t align_container={10,20,200,100},align_measured={0,8,40,20},aligned;
    nova_alignment_spec_t align_spec={.horizontal=NOVA_ALIGN_CENTER,
        .vertical=NOVA_ALIGN_CENTER_VERTICAL,.margin={4,4,4,4},
        .padding={8,8,8,8},.spacing_dlu=4};
    failed|=check(nova_layout_apply_alignment(&align_container,&align_measured,
        &align_spec,1000,&aligned)&&aligned.x==90&&aligned.y==60&&
        aligned.width==40&&aligned.height==20,
        "Alignment trennt Margin, Padding und Zentrierung in DLU");
    align_spec.horizontal=NOVA_ALIGN_STRETCH;
    align_spec.vertical=NOVA_ALIGN_STRETCH_VERTICAL;align_spec.accessibility=true;
    failed|=check(nova_layout_apply_alignment(&align_container,&align_measured,
        &align_spec,1000,&aligned)&&aligned.x==25&&aligned.y==35&&
        aligned.width==170&&aligned.height==70&&
        nova_alignment_diagnostics()->scale_milli==1250,
        "Stretch und Accessibility skalieren Insets ohne Pixelwerte");
    align_spec.horizontal=NOVA_ALIGN_LEFT;align_spec.vertical=NOVA_ALIGN_BASELINE;
    align_spec.accessibility=false;align_spec.baseline_dlu=18;
    failed|=check(nova_layout_apply_alignment(&align_container,&align_measured,
        &align_spec,1000,&aligned)&&aligned.y==42,
        "Gemeinsame Textbaseline wird deterministisch ausgerichtet");
    int32_t spacing_positions[3];nova_rect_t spacing_content={0,0,100,20};
    failed|=check(nova_layout_apply_spacing(&spacing_content,3,10,4,
        NOVA_DISTRIBUTE_SPACE_BETWEEN,true,1000,spacing_positions,3)&&
        spacing_positions[0]==0&&spacing_positions[1]==45&&spacing_positions[2]==90&&
        nova_layout_apply_spacing(&spacing_content,3,10,4,
        NOVA_DISTRIBUTE_SPACE_EVENLY,true,1000,spacing_positions,3)&&
        spacing_positions[0]==17&&spacing_positions[1]==44&&spacing_positions[2]==71,
        "Space Between und Space Evenly verteilen Elemente reproduzierbar");
    align_spec.margin.left=-1;
    failed|=check(!nova_layout_apply_alignment(&align_container,&align_measured,
        &align_spec,1000,&aligned)&&nova_alignment_diagnostics()->errors>=1,
        "Negative Insets und beschaedigte Alignmentdaten werden abgewiesen");
    nova_stack_t vertical_stack,nested_stack;
    nova_stack_child_t stack_a={.id=1,.desired={0,0,40,20},
        .horizontal=NOVA_ALIGN_LEFT,.vertical=NOVA_ALIGN_TOP,.visible=true};
    nova_stack_child_t stack_b={.id=2,.desired={0,0,30,10},
        .horizontal=NOVA_ALIGN_STRETCH,.vertical=NOVA_ALIGN_TOP,
        .visible=true,.stretch=true};
    nova_stack_child_t hidden={.id=3,.desired={0,0,90,90},
        .horizontal=NOVA_ALIGN_LEFT,.vertical=NOVA_ALIGN_TOP,.visible=false};
    failed|=check(nova_stack_initialize(&vertical_stack,NOVA_STACK_VERTICAL,
        (nova_rect_t){10,20,100,100},(nova_layout_insets_t){5,5,5,5},4,
        NOVA_DISTRIBUTE_NONE,1000,false)&&nova_stack_add(&vertical_stack,&stack_a)&&
        nova_stack_add(&vertical_stack,&stack_b)&&nova_stack_add(&vertical_stack,&hidden)&&
        nova_stack_update(&vertical_stack)&&
        vertical_stack.children[0].bounds.x==15&&vertical_stack.children[0].bounds.y==25&&
        vertical_stack.children[1].bounds.x==15&&vertical_stack.children[1].bounds.y==49&&
        vertical_stack.children[1].bounds.width==90&&
        vertical_stack.children[1].bounds.height==66,
        "Vertical Stack ordnet sichtbare Kinder mit Padding, Spacing und Stretch an");
    failed|=check(nova_stack_initialize(&nested_stack,NOVA_STACK_HORIZONTAL,
        (nova_rect_t){0},(nova_layout_insets_t){0},8,NOVA_DISTRIBUTE_NONE,1000,true)&&
        nova_stack_add(&nested_stack,&stack_a),
        "Horizontaler Accessibility-Stack besitzt feste Kindliste");
    nova_stack_child_t nested_child={.id=4,.desired={0,0,40,20},
        .horizontal=NOVA_ALIGN_STRETCH,.vertical=NOVA_ALIGN_STRETCH_VERTICAL,
        .nested=&nested_stack,.visible=true,.stretch=true};
    nova_stack_t root_stack;
    failed|=check(nova_stack_initialize(&root_stack,NOVA_STACK_VERTICAL,
        (nova_rect_t){0,0,120,80},(nova_layout_insets_t){0},0,
        NOVA_DISTRIBUTE_NONE,1000,false)&&nova_stack_add(&root_stack,&nested_child)&&
        nova_stack_update(&root_stack)&&nested_stack.parent==&root_stack&&
        nested_stack.children[0].bounds.width==40&&
        nova_stack_diagnostics()->max_depth>=2,
        "Verschachtelte Stacks behalten Reihenfolge und werden rekursionsbegrenzt berechnet");
    failed|=check(!nova_stack_add(&root_stack,&nested_child)&&
        nova_stack_diagnostics()->duplicates>=1&&!nova_stack_add(&nested_stack,
        &(nova_stack_child_t){.id=5,.nested=&root_stack,.visible=true,
          .horizontal=NOVA_ALIGN_LEFT,.vertical=NOVA_ALIGN_TOP})&&
        nova_stack_diagnostics()->cycles>=1,
        "Doppelte Kinder und zyklische Stackstrukturen werden abgewiesen");
    nova_grid_t grid;
    nova_grid_child_t grid_a={.id=10,.desired={0,0,40,18},.row=0,.column=0,
        .row_span=1,.column_span=1,.horizontal=NOVA_ALIGN_STRETCH,
        .vertical=NOVA_ALIGN_STRETCH_VERTICAL,.visible=true};
    nova_grid_child_t grid_b={.id=11,.desired={0,0,20,20},.row=1,.column=2,
        .row_span=1,.column_span=1,.horizontal=NOVA_ALIGN_STRETCH,
        .vertical=NOVA_ALIGN_STRETCH_VERTICAL,.visible=true};
    failed|=check(nova_grid_initialize(&grid,(nova_rect_t){0,0,200,100},1000,4,4,false)&&
        nova_grid_add_row(&grid,NOVA_GRID_FIXED,20)&&
        nova_grid_add_row(&grid,NOVA_GRID_PERCENT,500)&&
        nova_grid_add_column(&grid,NOVA_GRID_AUTO,0)&&
        nova_grid_add_column(&grid,NOVA_GRID_FIXED,50)&&
        nova_grid_add_column(&grid,NOVA_GRID_STAR,1)&&
        nova_grid_add(&grid,&grid_a)&&nova_grid_add(&grid,&grid_b)&&
        nova_grid_update(&grid)&&grid.columns[0].pixels==40&&
        grid.columns[1].pixels==50&&grid.columns[2].pixels==102&&
        grid.rows[0].pixels==20&&grid.rows[1].pixels==48&&
        grid.children[1].bounds.x==98&&grid.children[1].bounds.y==24&&
        grid.children[1].bounds.width==102&&grid.children[1].bounds.height==48,
        "Grid berechnet Auto, Fixed, Star und Prozent deterministisch");
    nova_grid_t span_grid;
    nova_grid_child_t span_child={.id=20,.desired={0,0,75,30},.row=0,.column=0,
        .row_span=2,.column_span=2,.horizontal=NOVA_ALIGN_STRETCH,
        .vertical=NOVA_ALIGN_STRETCH_VERTICAL,.visible=true};
    failed|=check(nova_grid_initialize(&span_grid,(nova_rect_t){0,0,80,40},1000,2,2,true)&&
        nova_grid_add_row(&span_grid,NOVA_GRID_AUTO,0)&&nova_grid_add_row(&span_grid,NOVA_GRID_STAR,1)&&
        nova_grid_add_column(&span_grid,NOVA_GRID_AUTO,0)&&nova_grid_add_column(&span_grid,NOVA_GRID_STAR,1)&&
        nova_grid_add(&span_grid,&span_child)&&nova_grid_update(&span_grid)&&
        span_grid.children[0].bounds.width==80&&span_grid.children[0].bounds.height==40&&
        nova_grid_diagnostics()->spans>=1,
        "RowSpan und ColumnSpan berücksichtigen Accessibility und Restfläche");
    nova_grid_child_t overlapping={.id=21,.desired={0,0,10,10},.row=1,.column=1,
        .row_span=1,.column_span=1,.horizontal=NOVA_ALIGN_LEFT,
        .vertical=NOVA_ALIGN_TOP,.visible=true};
    failed|=check(!nova_grid_add(&span_grid,&overlapping)&&
        nova_grid_diagnostics()->overlaps>=1,
        "Überlappende Gridzellen werden vor dem Layout abgewiesen");
    nova_overlay_t overlay_layout;
    nova_overlay_child_t overlay_background={.id=30,.desired={0},
        .horizontal=NOVA_ALIGN_STRETCH,.vertical=NOVA_ALIGN_STRETCH_VERTICAL,
        .layer=NOVA_OVERLAY_BACKGROUND,.material=NOVA_MATERIAL_ACRYLIC,
        .visibility=NOVA_OVERLAY_VISIBLE,.opacity=255};
    nova_overlay_child_t overlay_dialog={.id=31,.desired={0,0,80,40},
        .horizontal=NOVA_ALIGN_CENTER,.vertical=NOVA_ALIGN_CENTER_VERTICAL,
        .layer=NOVA_OVERLAY_DIALOG,.material=NOVA_MATERIAL_GLASS,
        .visibility=NOVA_OVERLAY_VISIBLE,.z_index=2,.opacity=230,
        .modal=true,.interactive=true};
    nova_overlay_child_t overlay_tooltip={.id=32,.desired={0,0,30,20},
        .horizontal=NOVA_ALIGN_LEFT,.vertical=NOVA_ALIGN_TOP,
        .layer=NOVA_OVERLAY_TOOLTIP,.material=NOVA_MATERIAL_SOLID,
        .visibility=NOVA_OVERLAY_VISIBLE,.z_index=1,.opacity=255,
        .absolute=true,.offset_x_dlu=160,.offset_y_dlu=70,.interactive=true};
    failed|=check(nova_overlay_initialize(&overlay_layout,(nova_rect_t){0,0,200,100},
        (nova_layout_insets_t){10,10,10,10},1000,true,false)&&
        nova_overlay_add(&overlay_layout,&overlay_tooltip)&&
        nova_overlay_add(&overlay_layout,&overlay_background)&&
        nova_overlay_add(&overlay_layout,&overlay_dialog)&&nova_overlay_update(&overlay_layout)&&
        overlay_layout.order[0]==1&&overlay_layout.order[1]==2&&overlay_layout.order[2]==0&&
        overlay_layout.children[1].bounds.width==180&&overlay_layout.children[1].bounds.height==80&&
        overlay_layout.children[2].bounds.x==60&&overlay_layout.children[2].bounds.y==30&&
        overlay_layout.children[0].bounds.x==170&&overlay_layout.children[0].bounds.y==80&&
        overlay_layout.children[0].bounds.width==20&&overlay_layout.children[0].bounds.height==10&&
        overlay_layout.modal_active&&overlay_layout.modal_id==31,
        "Overlay sortiert Layer/Z, richtet aus und clippt absolute Kinder");
    failed|=check(!nova_overlay_input_allowed(&overlay_layout,30)&&
        nova_overlay_input_allowed(&overlay_layout,31)&&nova_overlay_input_allowed(&overlay_layout,32)&&
        nova_overlay_hit_test(&overlay_layout,70,40)->id==31,
        "Modale Ebene blockiert niedrigere Eingaben und erhält Fokuspriorität");
    failed|=check(nova_overlay_set_visibility(&overlay_layout,32,NOVA_OVERLAY_HIDDEN)&&
        nova_overlay_update(&overlay_layout)&&!nova_overlay_hit_test(&overlay_layout,175,85)&&
        nova_overlay_set_visibility(&overlay_layout,32,NOVA_OVERLAY_COLLAPSED)&&
        nova_overlay_update(&overlay_layout)&&nova_overlay_diagnostics()->visibility_changes>=2,
        "Hidden bleibt im Layout, Collapsed wird ausgeschlossen und beide sind nicht interaktiv");
    nova_overlay_child_t modal_conflict=overlay_dialog;modal_conflict.id=33;
    failed|=check(!nova_overlay_add(&overlay_layout,&modal_conflict)&&
        nova_overlay_diagnostics()->modal_conflicts>=1,
        "Gleich priorisierte modale Overlays werden abgewiesen");
    nova_size_constraints_t valid_constraints = {40,30,100,80,200,160};
    uint32_t constrained_width=0, constrained_height=0;
    failed |= check(nova_constraints_resolve(&valid_constraints,90,70,
                    &constrained_width,&constrained_height) &&
                    constrained_width==90 && constrained_height==70,
                    "priorisierte Größenconstraints");
    nova_constraint_spec_t adaptive={.width_mode=NOVA_SIZE_PERCENT,
        .height_mode=NOVA_SIZE_CONTENT,.width_percent_milli=500,
        .content_height_dlu=20,.margin={10,10,10,10},.padding={5,5,5,5}};
    nova_rect_t constraint_result;
    failed|=check(nova_constraints_resolve_dlu(&adaptive,(nova_rect_t){10,20,200,100},
        &constraint_result,1000)&&constraint_result.x==20&&constraint_result.y==30&&
        constraint_result.width==100&&constraint_result.height==30,
        "Prozent und Content werden nach Margin und mit Padding in DLU aufgelöst");
    adaptive=(nova_constraint_spec_t){.width_mode=NOVA_SIZE_STRETCH,
        .height_mode=NOVA_SIZE_STRETCH,.aspect_width=16,.aspect_height=9};
    failed|=check(nova_constraints_resolve_dlu(&adaptive,(nova_rect_t){0,0,200,100},
        &constraint_result,1000)&&constraint_result.width==177&&
        constraint_result.height==100&&nova_constraint_diagnostics()->aspect_adjustments>=1,
        "Seitenverhältnis wird nach Min/Max und vor niedrigeren Größenregeln angewendet");
    adaptive=(nova_constraint_spec_t){.width_mode=NOVA_SIZE_PERCENT,
        .height_mode=NOVA_SIZE_FIXED,.width_percent_milli=500,.height_dlu=10,
        .preferred_width_dlu=80,.accessibility=true,.interactive=true};
    failed|=check(nova_constraints_resolve_dlu(&adaptive,(nova_rect_t){0,0,200,100},
        &constraint_result,1000)&&constraint_result.width==100&&
        constraint_result.height==55&&nova_constraint_diagnostics()->preferred_wins>=1&&
        nova_constraint_diagnostics()->accessibility_adjustments>=1,
        "Accessibility-Mindestfläche gewinnt und bevorzugte Größe schlägt Prozent");
    adaptive.width_percent_milli=1001;
    failed|=check(!nova_constraints_resolve_dlu(&adaptive,(nova_rect_t){0,0,200,100},
        &constraint_result,1000)&&constraint_result.width==200&&
        nova_constraint_diagnostics()->fallbacks>=1,
        "Ungültige Constraints liefern sichere Standard-Constraints");
    nova_stack_t constrained_stack;
    nova_stack_child_t constrained_child={.id=40,.horizontal=NOVA_ALIGN_LEFT,
        .vertical=NOVA_ALIGN_TOP,.visible=true,.use_adaptive_constraints=true,
        .adaptive_constraints={.width_mode=NOVA_SIZE_FIXED,.height_mode=NOVA_SIZE_FIXED,
            .width_dlu=30,.height_dlu=20}};
    failed|=check(nova_stack_initialize(&constrained_stack,NOVA_STACK_VERTICAL,
        (nova_rect_t){0,0,100,80},(nova_layout_insets_t){0},0,
        NOVA_DISTRIBUTE_NONE,1000,false)&&nova_stack_add(&constrained_stack,&constrained_child)&&
        nova_stack_update(&constrained_stack)&&constrained_stack.children[0].bounds.width==30&&
        constrained_stack.children[0].bounds.height==20,
        "Stackcontainer verwendet die DLU-Constraint-Engine im Measure-Pass");
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

    nova_motion_initialize();
    int32_t traced_value=0;
    nova_animation_t traced={.target=&traced_value,.from=0,.to=100,
        .start_ms=10,.duration_ms=100,.group=4,.repeats=1,.priority=2,
        .property=NOVA_PROPERTY_OPACITY,.easing=NOVA_EASE_LINEAR,
        .state=NOVA_MOTION_CREATED,.interruptible=true};
    nova_animation_t *traced_animation=nova_motion_create(&traced);
    nova_animation_diag_frame(16000,5000,1500,1200,true,2,1);
    nova_motion_update(10);nova_motion_update(110);nova_motion_update(211);
    nova_animation_diag_frame(40000,8000,2500,2500,true,3,0);
    nova_animation_diag_filter_t animation_filter={.type=NOVA_ANIMATION_GENERAL,
        .use_type=true,.priority=2,.use_priority=true,.minimum_duration_us=100000,
        .maximum_duration_us=250000,.use_duration=true};
    const nova_animation_trace_t *completed_trace=nova_animation_diag_query(&animation_filter,0);
    const nova_animation_diag_status_t *animation_status=nova_animation_diag_status();
    failed|=check(traced_animation&&traced_value==100&&completed_trace&&
        animation_status->initialized&&animation_status->passive&&
        animation_status->read_only&&animation_status->boot_unaffected&&
        animation_status->started==1&&animation_status->repeated==1&&
        animation_status->completed==1&&animation_status->budget_violations==1&&
        animation_status->dropped_frames>=2&&animation_status->jitter_events==1,
        "Animationsdiagnose zeichnet Lifecycle, Filter und Frameabweichungen passiv auf");
    uint8_t animation_export[8192];uint32_t animation_written=0;
    failed|=check(!nova_animation_diag_export(NOVA_ANIMATION_EXPORT_JSON,false,
            animation_export,sizeof(animation_export),&animation_written)&&
        nova_animation_diag_export(NOVA_ANIMATION_EXPORT_NDF,true,animation_export,
            sizeof(animation_export),&animation_written)&&animation_written>0&&
        nova_animation_diag_export(NOVA_ANIMATION_EXPORT_JSON,true,animation_export,
            sizeof(animation_export),&animation_written)&&animation_written>0&&
        nova_animation_diag_export(NOVA_ANIMATION_EXPORT_CSV,true,animation_export,
            sizeof(animation_export),&animation_written)&&animation_written>0&&
        nova_animation_diag_export(NOVA_ANIMATION_EXPORT_BINARY,true,animation_export,
            sizeof(animation_export),&animation_written)&&animation_written>0&&
        nova_animation_diag_status()->exports==4&&
        nova_animation_diag_status()->denied_exports==1,
        "Animationsdiagnose exportiert autorisiert als NDF, JSON, CSV und binaer");
    nova_animation_diag_reset();
    failed|=check(!nova_animation_diag_status()->count&&
        nova_animation_diag_status()->initialized,"Animationsdiagnose deterministisch leeren");

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
    static const uint32_t render_reference[16]={
        0xff101112u,0xff202122u,0xff303132u,0xff404142u,
        0xff505152u,0xff606162u,0xff707172u,0xff808182u,
        0xff909192u,0xffa0a1a2u,0xffb0b1b2u,0xffc0c1c2u,
        0xffd0d1d2u,0xffe0e1e2u,0xfff0f1f2u,0xff010203u};
    uint32_t render_actual[16],render_difference[16];
    uint8_t reference_mask[16];
    for(uint8_t i=0;i<16;++i){render_actual[i]=render_reference[i];reference_mask[i]=1;}
    nova_render_reference_metadata_t render_metadata={.reference_id=2002,.version=1,
        .width=4,.height=4,.dpi=96,.theme=NOVA_REFERENCE_DARK,
        .profile=NOVA_REFERENCE_SOFTWARE,.created_epoch=1,.read_only=true,.validated=true};
    failed|=check(nova_render_reference_initialize()&&
        nova_render_reference_execute(1,&render_metadata,render_reference,render_actual,0,4,
                                      render_difference,4),
        "Rendering-Referenz pixel- und alphagenau vergleichen");
    render_actual[6]=render_reference[6]+1u;
    failed|=check(nova_render_reference_execute(2,&render_metadata,render_reference,render_actual,0,4,
                                      render_difference,4),
        "Rendering-Referenz toleriert eine Kanalabweichung von eins");
    render_actual[6]=render_reference[5];
    failed|=check(nova_render_reference_execute(3,&render_metadata,render_reference,render_actual,0,4,
                                      render_difference,4)&&
        nova_render_reference_results()[2].position_tolerance_used,
        "Rendering-Referenz toleriert eine Position um ein Pixel");
    render_actual[6]=0xffffffffu;
    failed|=check(!nova_render_reference_execute(4,&render_metadata,render_reference,render_actual,0,4,
                                      render_difference,4)&&
        render_difference[6]==0xffff00ffu&&
        nova_render_reference_results()[3].first_difference_x==2&&
        nova_render_reference_results()[3].first_difference_y==1,
        "Rendering-Regression erzeugt Differenzpixel und Statistik");
    reference_mask[6]=0;
    failed|=check(nova_render_reference_execute(5,&render_metadata,render_reference,render_actual,
                                      reference_mask,4,render_difference,4)&&
        nova_render_reference_summary()->passed==4&&
        nova_render_reference_summary()->failed==1,
        "Maskierter Renderingvergleich blendet freigegebene Bereiche aus");
    uint8_t render_report[1024];uint32_t render_report_size=0;
    failed|=check(!nova_render_reference_generate_report(false,render_report,sizeof(render_report),
                                                         &render_report_size)&&
        nova_render_reference_generate_report(true,render_report,sizeof(render_report),
                                               &render_report_size)&&render_report_size>200&&
        nova_render_reference_summary()->isolated&&nova_render_reference_summary()->deterministic,
        "Rendering-Referenzbericht ist autorisiert, isoliert und deterministisch");
    architecture_probe_t architecture_probes[4]={{7,0,NOVA_TEST_PASSED,false},
        {9,0,NOVA_TEST_PASSED,false},{0,0,NOVA_TEST_SKIPPED,false},
        {8,0,NOVA_TEST_PASSED,false}};
    nova_boot_test_case_t architecture_cases[4]={
        {1001,1,1,"Unit Probe","Deterministischer Unit-Test",7,1,1,1,0,0,
            NOVA_TEST_UNIT,NOVA_TEST_FUNCTIONAL,architecture_execute,&architecture_probes[0]},
        {1002,1,1,"UI Probe","Deterministischer UI-Test",9,2,5,2,1,1,
            NOVA_TEST_UI,NOVA_TEST_GRAPHICAL,architecture_execute,&architecture_probes[1]},
        {1003,1,1,"Installer Probe","Nicht vorhandenes Backend",0,3,5,3,1,1,
            NOVA_TEST_SYSTEM,NOVA_TEST_ROBUSTNESS,architecture_execute,&architecture_probes[2]},
        {1004,2,1,"Mismatch Probe","Erwartungsfehler",7,4,9,1,0,0,
            NOVA_TEST_REGRESSION,NOVA_TEST_FUNCTIONAL,architecture_execute,&architecture_probes[3]}};
    architecture_test_clock=0;
    failed|=check(nova_boot_test_initialize(architecture_clock,architecture_state)&&
        nova_boot_test_register(&architecture_cases[0])&&
        nova_boot_test_register(&architecture_cases[1])&&
        nova_boot_test_register(&architecture_cases[2])&&
        nova_boot_test_register(&architecture_cases[3])&&
        !nova_boot_test_register(&architecture_cases[0])&&nova_boot_test_run(1)&&
        nova_boot_test_summary()->executed==3&&nova_boot_test_summary()->passed==2&&
        nova_boot_test_summary()->skipped==1&&nova_boot_test_summary()->failed==0&&
        nova_boot_test_summary()->total_duration_us==75&&nova_boot_test_summary()->isolated,
        "Zentrale Test Registry plant Suite deterministisch und isoliert");
    uint8_t architecture_report[1024];uint32_t architecture_report_size=0;
    failed|=check(!nova_boot_test_generate_report(false,architecture_report,
            sizeof(architecture_report),&architecture_report_size)&&
        nova_boot_test_generate_report(true,architecture_report,sizeof(architecture_report),
            &architecture_report_size)&&architecture_report_size>180&&
        !nova_boot_test_run(2)&&nova_boot_test_summary()->failed==1&&
        nova_boot_test_results()[0].error_code==0x1002u,
        "Zentraler Test Runner erkennt Fehler und erzeugt autorisierten Bericht");
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
        NOVA_BLUR_KERNEL_BOX,&masked_blur_geometry,effect_mask_scope,1};
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
    const nova_resource_diag_status_t *resource_diag=nova_resource_diag_status();
    nova_resource_diag_filter_t resource_filter={.type=NOVA_RESOURCE_BINARY,.use_type=true,
        .cache_hits_only=true};
    failed|=check(resource_diag->initialized&&resource_diag->passive&&
        resource_diag->read_only&&resource_diag->boot_unaffected&&
        resource_diag->requests>=4&&resource_diag->loads>=2&&
        resource_diag->validations>=2&&resource_diag->decompressions>=1&&
        resource_diag->cache_hits>=1&&resource_diag->cache_misses>=2&&
        resource_diag->errors>=1&&nova_resource_diag_get(0)&&
        nova_resource_diag_query(&resource_filter,0),
        "Ressourcendiagnose zeichnet Lifecycle, Cache, Integritaet und Fehler passiv auf");
    uint8_t resource_export[16384];uint32_t resource_written=0;
    failed|=check(!nova_resource_diag_export(NOVA_RESOURCE_EXPORT_JSON,false,
            resource_export,sizeof(resource_export),&resource_written)&&
        nova_resource_diag_export(NOVA_RESOURCE_EXPORT_NDF,true,resource_export,
            sizeof(resource_export),&resource_written)&&resource_written>0&&
        nova_resource_diag_export(NOVA_RESOURCE_EXPORT_JSON,true,resource_export,
            sizeof(resource_export),&resource_written)&&resource_written>0&&
        nova_resource_diag_export(NOVA_RESOURCE_EXPORT_CSV,true,resource_export,
            sizeof(resource_export),&resource_written)&&resource_written>0&&
        nova_resource_diag_export(NOVA_RESOURCE_EXPORT_BINARY,true,resource_export,
            sizeof(resource_export),&resource_written)&&resource_written>0&&
        nova_resource_diag_status()->exports==4&&
        nova_resource_diag_status()->denied_exports==1,
        "Ressourcendiagnose exportiert autorisiert als NDF, JSON, CSV und binaer");
    nova_resource_corruption_initialize();
    static const nova_corruption_type_t corruption_types[]={NOVA_CORRUPTION_BIT_ERROR,
        NOVA_CORRUPTION_TRUNCATED,NOVA_CORRUPTION_EMPTY,NOVA_CORRUPTION_INVALID_HEADER,
        NOVA_CORRUPTION_INVALID_SIGNATURE,NOVA_CORRUPTION_INVALID_CHECKSUM,
        NOVA_CORRUPTION_INVALID_VERSION,NOVA_CORRUPTION_INVALID_METADATA};
    for(uint8_t i=0;i<sizeof(corruption_types)/sizeof(corruption_types[0]);++i)
        failed|=check(nova_resource_corruption_execute(compressed_resource->id,
            corruption_types[i]),"Synthetische Ressourcenkorruption sicher erkennen");
    const nova_resource_corruption_status_t *corruption_status=
        nova_resource_corruption_status();
    failed|=check(corruption_status->executed==8&&corruption_status->detected==8&&
        corruption_status->continued==8&&corruption_status->isolated&&
        corruption_status->deterministic&&corruption_status->heap_free&&
        corruption_status->productive_data_unchanged,
        "Korruptionssuite isoliert Produktivdaten und haelt den Objektpool konsistent");
    uint8_t corruption_report[4096];uint32_t corruption_written=0;
    failed|=check(!nova_resource_corruption_generate_report(false,corruption_report,
            sizeof(corruption_report),&corruption_written)&&
        nova_resource_corruption_generate_report(true,corruption_report,
            sizeof(corruption_report),&corruption_written)&&corruption_written>100&&
        nova_resource_corruption_status()->reports==1,
        "Autorisierter CI-Bericht fuer alle Ressourcenkorruptionen");
    nova_resource_diag_reset();
    failed|=check(!nova_resource_diag_status()->count&&nova_resource_diag_status()->initialized,
        "Ressourcendiagnose deterministisch leeren");
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
    nova_surface_clear(overlay,0x40000040u);
    image_options=(nova_image_render_options_t){.destination={0,0,2,2},
        .clip={0,0,8,8},.sampling=NOVA_IMAGE_SAMPLE_NEAREST,
        .transform=nova_transform_fixed_identity(),.opacity=1000,.tint=0xffffffffu};
    failed|=check(nova_image_render(raw_image,overlay,&image_options)==NOVA_IMAGE_OK&&
        overlay->pixels[1*overlay->stride+1]==0xa08080a0u,
        "Premultipliziertes SrcOver bleibt auch auf transparentem Ziel premultipliziert");
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
        nova_image_diagnostics()->cache_hits>=1&&nova_image_diagnostics()->renders==3,
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
    failed|=check(nova_boot_quality_initialize()&&
        nova_boot_quality_status()->current_quality==NOVA_BOOT_QUALITY_HIGH&&
        nova_boot_quality_status()->automatic&&
        nova_boot_quality_status()->state_preserved&&
        nova_boot_quality_status()->security_preserved,
        "Normative AQM-API startet automatisch ohne Funktions- oder Sicherheitsverlust");
    nova_quality_telemetry_t ultra_telemetry={.frame_time_us=10000,.width=1920,.height=1080,
        .available_memory=192ull*1024u*1024u,.cache_used=2u*1024u*1024u,
        .cache_budget=12u*1024u*1024u,.cpu_score=90,.gpu_load_percent=20,
        .gpu_available=true,.software_renderer=false,.firmware_limited=false};
    failed|=check(nova_render_quality_observe(&ultra_telemetry)&&nova_boot_quality_update(),
        "CPU, GPU, Speicher, Cache, Framezeit und Aufloesung fliessen in AQM ein");
    for(uint16_t stable=0;stable<119;++stable)nova_render_quality_report_frame(10000);
    failed|=check(nova_render_quality_get()==NOVA_RENDER_QUALITY_HIGH,
        "AQM wertet kurze Erholungsphase nicht als Profilwechsel");
    nova_render_quality_report_frame(10000);
    failed|=check(nova_render_quality_get()==NOVA_RENDER_QUALITY_ULTRA&&
        nova_render_quality_diagnostics()->recoveries==1&&
        nova_boot_quality_status()->parameters.svg_raster_percent==100&&
        nova_boot_quality_status()->parameters.glyph_atlas_percent==100&&
        nova_boot_quality_status()->parameters.icon_raster_percent==100,
        "Stabile Last erholt auf Ultra und exponiert alle Qualitaetsparameter");
    nova_quality_telemetry_t software_telemetry=ultra_telemetry;
    software_telemetry.gpu_available=false;software_telemetry.software_renderer=true;
    failed|=check(nova_render_quality_observe(&software_telemetry)&&
        nova_boot_quality_update()&&nova_render_quality_get()==NOVA_RENDER_QUALITY_PERFORMANCE&&
        nova_render_quality_diagnostics()->last_reason==NOVA_QUALITY_REASON_GRAPHICS_ERROR&&
        nova_boot_quality_status()->parameters.animation_frame_step==3,
        "GPU-Ausfall und Software-Rendering wechseln deterministisch auf Low");
    uint32_t quality_generation=nova_render_quality_diagnostics()->atomic_generation;
    failed|=check(nova_boot_quality_set(NOVA_BOOT_QUALITY_BALANCED)&&
        !nova_boot_quality_status()->automatic&&nova_render_quality_observe(&software_telemetry)&&
        nova_boot_quality_update()&&nova_render_quality_get()==NOVA_RENDER_QUALITY_STANDARD&&
        nova_render_quality_diagnostics()->atomic_generation==quality_generation+1,
        "Festes Benutzerprofil bleibt trotz Telemetrie aktiv und wechselt atomar");
    failed|=check(!nova_render_quality_observe(&(nova_quality_telemetry_t){.width=0,.height=1})&&
        !nova_boot_quality_set((nova_boot_quality_t)99),
        "Ungueltige AQM-Telemetrie und Profile werden abgewiesen");
    failed|=check(nova_low_end_profile_initialize()&&
        nova_low_end_profile_detect(4,3200,128ull*1024u*1024u,true,false)&&
        !nova_low_end_profile_status()->enabled&&
        nova_low_end_profile_detect(1,1000,32ull*1024u*1024u,false,false)&&
        nova_low_end_profile_status()->enabled&&nova_low_end_profile_status()->automatic&&
        nova_low_end_profile_status()->software_renderer&&
        nova_low_end_profile_status()->reduced_effects&&
        nova_low_end_profile_status()->reduced_animations&&
        nova_low_end_profile_status()->dirty_regions&&
        nova_low_end_profile_status()->double_buffered&&
        nova_low_end_profile_status()->event_driven&&
        nova_low_end_profile_status()->layout_preserved&&
        nova_low_end_profile_status()->functionality_preserved&&
        nova_low_end_profile_status()->security_preserved&&
        nova_low_end_profile_status()->target_fps==30&&
        nova_render_quality_get()==NOVA_RENDER_QUALITY_PERFORMANCE,
        "LEHP erkennt 1 GHz/32 MiB/ohne GPU und erhaelt Funktion, Layout und Sicherheit");
    uint64_t low_end_sum=0;for(uint8_t area=0;area<NOVA_MEMORY_AREA_COUNT;++area)
        low_end_sum+=nova_memory_budget_status()->area_budget[area];
    failed|=check(nova_memory_budget_status()->total_budget==32ull*1024u*1024u&&
        low_end_sum==32ull*1024u*1024u&&
        nova_memory_budget_status()->area_budget[NOVA_MEMORY_AREA_RESOURCE_CACHE]==6ull*1024u*1024u&&
        nova_memory_budget_status()->area_budget[NOVA_MEMORY_AREA_GLYPH_CACHE]==2ull*1024u*1024u&&
        nova_memory_budget_status()->area_budget[NOVA_MEMORY_AREA_SVG_CACHE]==2ull*1024u*1024u&&
        nova_memory_budget_status()->area_budget[NOVA_MEMORY_AREA_THEME_CACHE]==512ull*1024u&&
        nova_memory_budget_status()->area_budget[NOVA_MEMORY_AREA_ANIMATION_POOL]==512ull*1024u&&
        nova_memory_budget_status()->area_budget[NOVA_MEMORY_AREA_CONTROL_POOL]==2ull*1024u*1024u,
        "LEHP besitzt exaktes 32-MiB-Budget und normative Teilbudgets");
    failed|=check(nova_memory_budget_set_runtime(true)&&
        nova_low_end_profile_update(30,33333,16ull*1024u*1024u,3ull*1024u*1024u)&&
        nova_low_end_profile_status()->runtime_heap_locked&&
        nova_low_end_profile_status()->last_fps==30&&
        nova_low_end_profile_status()->last_frame_us==33333&&
        nova_low_end_profile_disable()&&!nova_low_end_profile_status()->enabled&&
        nova_memory_budget_status()->total_budget==64ull*1024u*1024u,
        "LEHP-Diagnose, Runtime-Heapsperre und manuelles Deaktivieren");
    failed|=check(nova_low_end_profile_initialize()&&
        nova_low_end_profile_detect(8,4000,128ull*1024u*1024u,true,true)&&
        nova_render_quality_get()==NOVA_RENDER_QUALITY_SAFE,
        "Recovery/Safe Mode aktiviert LEHP mit Safe-Qualitaet");
    failed|=check(nova_low_end_profile_disable(),"LEHP kann auf moderner Hardware manuell beendet werden");
    failed|=check(nova_high_quality_profile_initialize()&&
        !nova_high_quality_profile_detect(8,16ull*1024u*1024u*1024u,
            512ull*1024u*1024u,true,true)&&
        !nova_high_quality_profile_status()->enabled&&
        nova_high_quality_profile_status()->software_fallback_available,
        "HQHP behauptet ohne registriertes GPU-Backend keine Beschleunigung");
    nova_high_quality_profile_set_gpu_backend(true);
    failed|=check(nova_high_quality_profile_detect(8,16ull*1024u*1024u*1024u,
            512ull*1024u*1024u,true,true)&&
        nova_high_quality_profile_status()->enabled&&
        nova_high_quality_profile_status()->automatic&&
        nova_high_quality_profile_status()->gpu_acceleration&&
        nova_high_quality_profile_status()->advanced_materials&&
        nova_high_quality_profile_status()->high_quality_shadows&&
        nova_high_quality_profile_status()->advanced_blur&&
        nova_high_quality_profile_status()->layer_caching&&
        nova_high_quality_profile_status()->dirty_regions&&
        nova_high_quality_profile_status()->double_buffered&&
        nova_high_quality_profile_status()->layout_preserved&&
        nova_high_quality_profile_status()->functionality_preserved&&
        nova_high_quality_profile_status()->security_preserved&&
        nova_high_quality_profile_status()->target_fps==60&&
        nova_render_quality_get()==NOVA_RENDER_QUALITY_ULTRA,
        "HQHP erkennt 4+ Kerne, 8 GiB, 256 MiB UI-RAM, GPU und schnellen Datentraeger");
    uint64_t high_quality_sum=0;for(uint8_t area=0;area<NOVA_MEMORY_AREA_COUNT;++area)
        high_quality_sum+=nova_memory_budget_status()->area_budget[area];
    failed|=check(nova_memory_budget_status()->total_budget==256ull*1024u*1024u&&
        high_quality_sum==256ull*1024u*1024u&&
        nova_memory_budget_status()->area_budget[NOVA_MEMORY_AREA_RESOURCE_CACHE]==64ull*1024u*1024u&&
        nova_memory_budget_status()->area_budget[NOVA_MEMORY_AREA_GLYPH_CACHE]==16ull*1024u*1024u&&
        nova_memory_budget_status()->area_budget[NOVA_MEMORY_AREA_SVG_CACHE]==16ull*1024u*1024u&&
        nova_memory_budget_status()->area_budget[NOVA_MEMORY_AREA_THEME_CACHE]==4ull*1024u*1024u&&
        nova_memory_budget_status()->area_budget[NOVA_MEMORY_AREA_ANIMATION_POOL]==16ull*1024u*1024u&&
        nova_memory_budget_status()->area_budget[NOVA_MEMORY_AREA_CONTROL_POOL]==16ull*1024u*1024u,
        "HQHP besitzt exaktes 256-MiB-Budget und normative Teilbudgets");
    failed|=check(nova_memory_budget_set_runtime(true)&&
        nova_high_quality_profile_update(60,16667,80ull*1024u*1024u,
            32ull*1024u*1024u,45)&&
        nova_high_quality_profile_status()->runtime_heap_locked&&
        nova_high_quality_profile_status()->last_fps==60&&
        nova_high_quality_profile_status()->gpu_load_percent==45,
        "HQHP-Diagnose und Runtime-Heapsperre bei 60 FPS");
    failed|=check(nova_high_quality_profile_report_gpu_failure()&&
        !nova_high_quality_profile_status()->enabled&&
        !nova_high_quality_profile_status()->gpu_acceleration&&
        nova_high_quality_profile_status()->fallback_active&&
        nova_high_quality_profile_status()->gpu_failures==1&&
        nova_memory_budget_status()->total_budget==64ull*1024u*1024u,
        "GPU-Ausfall wechselt jederzeit in vollstaendigen Software-Fallback");
    nova_high_quality_profile_set_gpu_backend(true);
    nova_theme_set_reduced_motion(true);
    failed|=check(nova_high_quality_profile_enable()&&nova_motion_is_reduced(),
        "HQHP respektiert die barrierefreie Einstellung Reduzierte Bewegung");
    nova_theme_set_reduced_motion(false);
    failed|=check(nova_high_quality_profile_disable(),"HQHP ist manuell deaktivierbar");
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
    bool child_handles=false,parent_handles=true;
    failed|=check(nova_control_set_event_handler(item,control_event_handler,&child_handles)&&
        nova_control_set_event_handler(list,control_event_handler,&parent_handles),
        "Routed-Event-Handler am Controlbaum registrieren");
    control_event_count=0;
    failed|=check(nova_control_set_bounds(item,(nova_rect_t){2,2,6,3})&&
        control_event_count==2&&control_event_types[0]==NOVA_CONTROL_EVENT_LAYOUT_CHANGED&&
        control_event_types[1]==NOVA_CONTROL_EVENT_LAYOUT_CHANGED,
        "LayoutChanged wird automatisch vom Target zum Parent geroutet");
    control_event_count=0;
    failed |= check(nova_control_invoke(item, &action) && action == 42 &&
                    !nova_control_invoke(item, &action)&&control_event_count==2&&
                    control_event_order[0]==item->id&&control_event_order[1]==list->id&&
                    nova_control_diagnostics()->events_handled>=1&&
                    nova_control_diagnostics()->events_bubbled>=1,
                    "Aktion exakt einmal und unbehandeltes Click zum Parent routen");
    nova_control_release(item);
    static const nova_control_event_type_t routed_input_types[]={
        NOVA_CONTROL_EVENT_MOUSE_ENTER,NOVA_CONTROL_EVENT_MOUSE_MOVE,
        NOVA_CONTROL_EVENT_MOUSE_DOWN,NOVA_CONTROL_EVENT_MOUSE_UP,
        NOVA_CONTROL_EVENT_MOUSE_LEAVE,NOVA_CONTROL_EVENT_KEY_DOWN,
        NOVA_CONTROL_EVENT_CHARACTER_INPUT};
    bool routed_inputs=true;
    for(uint8_t i=0;i<sizeof(routed_input_types)/sizeof(routed_input_types[0]);++i){
        control_event_count=0;
        nova_control_event_t input={.type=routed_input_types[i],.device_id=i<5?2u:1u,
            .key=4,.character='A',.x=3,.y=2};
        if(!nova_control_dispatch_event(item,&input)||control_event_count!=2||
           control_event_order[0]!=item->id||control_event_order[1]!=list->id||
           control_event_types[0]!=routed_input_types[i]||
           control_event_types[1]!=routed_input_types[i])routed_inputs=false;
    }
    failed|=check(routed_inputs,
        "Pointer- und Tastaturereignisse laufen Target-zu-Bubble in stabiler Reihenfolge");
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
    nova_control_t *password=nova_password_field_create();
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
                    nova_password_field_set_placeholder(password,"Passwort")&&
                    !nova_control_set_text(password,"Klartext")&&
                    nova_control_set_accessibility(password,9,"Geheimnis",false)&&
                    strcmp(password->accessibility_name,"Passwortfeld")==0&&
                    nova_text_field_insert(password,'N')&&nova_text_field_insert(password,'o')&&
                    nova_text_field_insert(password,'v')&&nova_text_field_insert(password,'a')&&
                    nova_text_field_insert(password,'7')&&password->text_length==5&&
                    nova_password_field_validate(password,5,true,true,true),
                    "Passwortfeld mit geschütztem Festpuffer");
    failed |= check(!nova_password_field_set_revealed(password,true,1000)&&
                    nova_password_field_enable_reveal(password,true)&&
                    nova_password_field_set_revealed(password,true,1000)&&
                    (password->flags&NOVA_CONTROL_FLAG_REVEALED)&&
                    nova_password_field_tick(password,999)&&
                    (password->flags&NOVA_CONTROL_FLAG_REVEALED)&&
                    nova_password_field_tick(password,1)&&
                    !(password->flags&NOVA_CONTROL_FLAG_REVEALED),
                    "Password Reveal ist opt-in und automatisch zeitbegrenzt");
    nova_control_render(field,overlay);nova_control_render(password,overlay);
    nova_password_field_clear(password);
    failed |= check(nova_password_field_empty(password)&&password->text[0]==0&&
                    nova_password_field_diagnostics(password)->clears>=1&&
                    !nova_password_field_diagnostics(password)->content_exposed_to_accessibility,
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
    nova_text_layout_initialize();
    nova_text_layout_begin(1);
    nova_text_layout_request_t aware_request={.utf8="NovaOS startet mit einer längeren Übersetzung",
        .available_width=110,.letter_spacing_dlu=1,.scale_milli=1000,.maximum_lines=3,
        .wrap=NOVA_TEXT_WRAP_WORD,.ellipsis=NOVA_TEXT_ELLIPSIS_END};
    nova_text_layout_metrics_t aware_metrics,aware_cached,accessible_metrics;
    failed |= check(nova_text_layout_measure(&aware_request,&aware_metrics)&&
                    aware_metrics.lines>1&&aware_metrics.height==
                    aware_metrics.line_height*aware_metrics.lines&&
                    aware_metrics.baseline>0&&aware_metrics.minimum_width>0&&
                    aware_metrics.maximum_width>=aware_metrics.minimum_width,
                    "Text-Aware Layout misst Wrap, intrinsische Größe und Baseline");
    failed |= check(nova_text_layout_measure(&aware_request,&aware_cached)&&
                    aware_cached.width==aware_metrics.width&&
                    nova_text_layout_diagnostics()->cache_hits==1,
                    "Text-Aware Layout misst identischen Text nur einmal pro Pass");
    aware_request.accessibility=true;
    failed |= check(nova_text_layout_measure(&aware_request,&accessible_metrics)&&
                    accessible_metrics.line_height>aware_metrics.line_height&&
                    accessible_metrics.height>=aware_metrics.height,
                    "Accessibility skaliert Textmetriken und Layout neu");
    aware_request=(nova_text_layout_request_t){.utf8="Start nicht möglich",
        .available_width=45,.scale_milli=1500,.maximum_lines=1,
        .wrap=NOVA_TEXT_WRAP_NONE,.ellipsis=NOVA_TEXT_ELLIPSIS_MIDDLE};
    failed |= check(nova_text_layout_measure(&aware_request,&aware_metrics)&&
                    aware_metrics.truncated&&aware_metrics.width==45,
                    "No-Wrap und Ellipsis begrenzen lange lokalisierte Texte");
    aware_request=(nova_text_layout_request_t){.utf8="A\xc3(",.available_width=100,
        .scale_milli=1000,.wrap=NOVA_TEXT_WRAP_CHARACTER};
    failed |= check(nova_text_layout_measure(&aware_request,&aware_metrics)&&
                    aware_metrics.invalid_unicode&&aware_metrics.fallback_font,
                    "Beschädigtes UTF-8 aktiviert deterministischen Ersatzfont");
    nova_surface_clear(overlay,0xff000000u);
    nova_text_draw_ellipsized_scaled(overlay,0,0,70,"Anfang-Mitte-Ende",
        0xffffffffu,NOVA_TEXT_LEFT,NOVA_TEXT_ELLIPSIS_START,1000);
    uint64_t start_ellipsis_hash=0;
    for(uint32_t pixel=0;pixel<70u*20u;++pixel)
        start_ellipsis_hash=start_ellipsis_hash*33u+overlay->pixels[pixel];
    nova_surface_clear(overlay,0xff000000u);
    nova_text_draw_ellipsized_scaled(overlay,0,0,70,"Anfang-Mitte-Ende",
        0xffffffffu,NOVA_TEXT_LEFT,NOVA_TEXT_ELLIPSIS_MIDDLE,1000);
    uint64_t middle_ellipsis_hash=0;
    for(uint32_t pixel=0;pixel<70u*20u;++pixel)
        middle_ellipsis_hash=middle_ellipsis_hash*33u+overlay->pixels[pixel];
    failed |= check(start_ellipsis_hash&&middle_ellipsis_hash&&
                    start_ellipsis_hash!=middle_ellipsis_hash,
                    "Start- und Middle-Ellipsis besitzen echte unterschiedliche Rendererpfade");
    nova_stack_t text_stack;
    nova_stack_child_t text_child={.id=90,.horizontal=NOVA_ALIGN_LEFT,
        .vertical=NOVA_ALIGN_BASELINE,.visible=true,.use_text_layout=true,
        .text_layout={.utf8="NovaOS installieren",.available_width=180,
            .scale_milli=1000,.maximum_lines=2,.wrap=NOVA_TEXT_WRAP_WORD,
            .ellipsis=NOVA_TEXT_ELLIPSIS_END},.icon_width_dlu=20,.icon_gap_dlu=8,
        .adaptive_constraints={.padding={8,6,8,6}}};
    failed |= check(nova_stack_initialize(&text_stack,NOVA_STACK_VERTICAL,
                    (nova_rect_t){0,0,240,80},(nova_layout_insets_t){0},0,
                    NOVA_DISTRIBUTE_NONE,1000,false)&&
                    nova_stack_add(&text_stack,&text_child)&&
                    nova_stack_measure(&text_stack)&&
                    text_stack.children[0].desired.width>
                    text_stack.children[0].text_metrics.width&&
                    text_stack.children[0].desired.y==6+
                    text_stack.children[0].text_metrics.baseline,
                    "Stack berechnet Text, Icon, Padding und Baseline vor dem Arrange-Pass");
    nova_grid_t text_grid;
    nova_grid_child_t text_grid_child={.id=91,.row=0,.column=0,.row_span=1,
        .column_span=1,.horizontal=NOVA_ALIGN_LEFT,.vertical=NOVA_ALIGN_TOP,
        .visible=true,.use_text_layout=true,.padding={5,4,5,4},
        .text_layout={.utf8="Grid-Text",.available_width=120,.scale_milli=1000,
            .maximum_lines=1,.wrap=NOVA_TEXT_WRAP_NONE,.ellipsis=NOVA_TEXT_ELLIPSIS_END}};
    failed |= check(nova_grid_initialize(&text_grid,(nova_rect_t){0,0,180,60},1000,0,0,false)&&
                    nova_grid_add_row(&text_grid,NOVA_GRID_AUTO,0)&&
                    nova_grid_add_column(&text_grid,NOVA_GRID_AUTO,0)&&
                    nova_grid_add(&text_grid,&text_grid_child)&&nova_grid_measure(&text_grid)&&
                    text_grid.children[0].desired.width>
                    text_grid.children[0].text_metrics.width,
                    "Grid-Auto-Track übernimmt intrinsische Text- und Paddinggröße");
    nova_overlay_t text_overlay;
    nova_overlay_child_t text_overlay_child={.id=92,.horizontal=NOVA_ALIGN_LEFT,
        .vertical=NOVA_ALIGN_TOP,.layer=NOVA_OVERLAY_TOOLTIP,
        .visibility=NOVA_OVERLAY_VISIBLE,.opacity=255,.use_text_layout=true,
        .text_padding={7,5,7,5},.text_layout={.utf8="Overlay-Hinweis",
            .available_width=150,.scale_milli=1000,.maximum_lines=2,
            .wrap=NOVA_TEXT_WRAP_WORD,.ellipsis=NOVA_TEXT_ELLIPSIS_END}};
    failed |= check(nova_overlay_initialize(&text_overlay,(nova_rect_t){0,0,200,80},
                    (nova_layout_insets_t){0},1000,true,false)&&
                    nova_overlay_add(&text_overlay,&text_overlay_child)&&
                    nova_overlay_measure(&text_overlay)&&
                    text_overlay.children[0].desired.height>
                    text_overlay.children[0].text_metrics.height,
                    "Overlay misst Tooltiptext vor Clipping und Arrange");
    nova_icon_draw(overlay, NOVA_ICON_SETTINGS, 0, 0, 20, 0xffffffffu);
    failed |= check(nova_icon_diagnostics()->renders == 1 &&
                    nova_icon_exists(NOVA_ICON_POWER), "semantische Icon-Tokens und Cache");

    nova_input_initialize();
    input_scan_count=0;
    failed|=check(!nova_input_device_scan()&&
        !nova_input_device_scanner_set(0,0)&&
        nova_input_device_scanner_set(input_scan_probe,&input_scan_count)&&
        nova_input_device_scan()&&nova_input_device_scan()&&input_scan_count==2,
        "Hardwareunabhaengiger deterministischer Device Scanner");
    failed |= check(nova_input_device_set(1, NOVA_DEVICE_KEYBOARD, true) &&
                    nova_input_device_set(2, NOVA_DEVICE_MOUSE, true),
                    "Input Device Hot Detection");
    nova_input_event_t received;
    uint32_t hot_events=0;bool hot_order=true;
    const uint32_t expected_hot[]={NOVA_DEVICE_EVENT_ADDED,NOVA_DEVICE_EVENT_CHANGED,
        NOVA_DEVICE_EVENT_READY,NOVA_DEVICE_EVENT_ADDED,NOVA_DEVICE_EVENT_CHANGED,
        NOVA_DEVICE_EVENT_READY};
    while(nova_input_next(&received)){hot_order&=hot_events<6&&
        received.type==NOVA_EVENT_SYSTEM&&received.system.command==expected_hot[hot_events];
        ++hot_events;}
    failed|=check(hot_order&&hot_events==6&&nova_input_device_count()==2&&
        nova_input_device_connected(1)&&nova_input_device_connected(2)&&
        nova_input_device_get(2)->state==NOVA_DEVICE_STATE_READY&&
        nova_input_device_get(2)->changes==3,
        "Hot Detection Lebenszyklus und Unified Device Events");
    failed|=check(nova_input_pointer_scale_delta(80,25)==20&&
        nova_input_pointer_scale_delta(80,100)==80&&
        nova_input_pointer_scale_delta(80,400)==320&&
        nova_input_pointer_scale_delta(1,25)==1&&
        nova_input_pointer_scale_delta(-1,25)==-1&&
        nova_input_pointer_scale_delta(80,24)==0&&
        nova_input_pointer_scale_delta(80,401)==0,
        "Konfigurierbare lineare Pointergeschwindigkeit mit sicheren Grenzen");
    failed|=check(!nova_input_pointer_click_update(2,0,item,3,2,100,false,500,4)&&
        nova_input_pointer_click_update(2,0,item,4,3,550,true,500,4)&&
        !nova_input_pointer_click_update(2,0,item,4,3,560,false,500,4)&&
        !nova_input_pointer_click_update(2,0,item,4,3,600,true,500,4)&&
        !nova_input_pointer_click_update(2,0,item,4,3,610,false,500,4)&&
        !nova_input_pointer_click_update(2,0,item,20,20,700,true,500,4)&&
        nova_input_diagnostics()->double_clicks==1,
        "Doppelklick nur fuer gleiches Ziel innerhalb Zeit- und Distanzgrenze");
    nova_input_event_t key_event = {.timestamp=10,.device_id=1,.type=NOVA_EVENT_KEY_DOWN};
    key_event.key.key = 40;
    nova_input_set_secure(true);
    failed |= check(nova_input_post(key_event), "Event Queue Post");
    failed |= check(nova_input_next(&received) && received.sequence == 7 &&
                    (received.flags & 0x80000000u), "deterministische sichere Event Queue");
    failed |= check(nova_input_shortcut_register(40,0,0,77) &&
                    !nova_input_shortcut_register(40,0,0,88), "Shortcut-Konfliktpruefung");
    uint32_t command = 0;
    failed |= check(nova_input_shortcut_resolve(&received,&command) && command==77,
                    "Shortcut erzeugt Command");
    nova_shortcut_t context_shortcut={.key=40,.command=88,.scope=NOVA_SHORTCUT_CONTEXT,
        .context_id=5,.focus_id=NOVA_CONTROL_NONE,.enabled=true};
    nova_shortcut_t dialog_shortcut={.key=40,.command=99,.scope=NOVA_SHORTCUT_DIALOG,
        .context_id=7,.focus_id=NOVA_CONTROL_NONE,.enabled=true};
    nova_shortcut_t developer_shortcut={.key=41,.modifiers=2,.command=111,
        .scope=NOVA_SHORTCUT_DEVELOPER,.context_id=5,.focus_id=NOVA_CONTROL_NONE,
        .enabled=true};
    failed|=check(nova_input_shortcut_register_ex(&context_shortcut)&&
        nova_input_shortcut_register_ex(&dialog_shortcut)&&
        nova_input_shortcut_register_ex(&developer_shortcut)&&
        !nova_input_shortcut_register_ex(&context_shortcut),
        "Globale, Kontext-, Dialog- und Entwickler-Shortcutregistrierung");
    nova_input_shortcut_set_context(5,0);
    failed|=check(nova_input_shortcut_resolve(&received,&command)&&command==88,
        "Kontext-Shortcut gewinnt gegen globalen Shortcut");
    nova_input_shortcut_set_context(5,7);
    failed|=check(nova_input_shortcut_resolve(&received,&command)&&command==99,
        "Dialog-Shortcut besitzt hoechste Prioritaet");
    nova_input_event_t developer_event=received;developer_event.key.key=41;
    developer_event.key.modifiers=2;
    nova_input_shortcut_set_context(5,0);nova_input_shortcut_set_developer(true,false);
    failed|=check(!nova_input_shortcut_resolve(&developer_event,&command)&&
        nova_input_shortcut_diagnostics()->unauthorized==1,
        "Nicht autorisierte Entwickler-Shortcuts bleiben gesperrt");
    nova_input_shortcut_set_developer(true,true);
    shortcut_probe_t shortcut_probe={.event=&developer_event};
    failed|=check(nova_input_shortcut_set_handler(shortcut_command_probe,&shortcut_probe)&&
        nova_input_shortcut_execute(&developer_event)&&shortcut_probe.command==111&&
        shortcut_probe.calls==1,"Shortcut dispatcht ausschliesslich einen Command");
    shortcut_probe.recurse=true;
    failed|=check(!nova_input_shortcut_execute(&developer_event)&&
        nova_input_shortcut_diagnostics()->recursive==1,
        "Rekursive Shortcut-Ausfuehrung wird verhindert");
    shortcut_probe.recurse=false;
    failed|=check(nova_input_shortcut_unregister(40,0,NOVA_SHORTCUT_DIALOG,7,
            NOVA_CONTROL_NONE)&&
        !nova_input_shortcut_unregister(40,0,NOVA_SHORTCUT_DIALOG,7,NOVA_CONTROL_NONE),
        "Shortcut wird deterministisch aus der Hash-Registry entfernt");
    nova_control_t *item2 = nova_control_create(NOVA_CONTROL_LIST_ITEM);
    nova_control_set_state(item2,NOVA_CONTROL_INITIALIZED);
    nova_control_set_state(item2,NOVA_CONTROL_VISIBLE);
    nova_control_set_bounds(item2,(nova_rect_t){1,5,6,2});
    failed |= check(nova_input_focus_set(item) &&
                    nova_input_neighbor_set(item,NOVA_DIRECTION_DOWN,item2) &&
                    nova_input_focus_navigate(NOVA_DIRECTION_DOWN) &&
                    nova_input_focus_get()==item2, "Directional Focus Navigation");
    nova_focus_scope_t main_focus_scope,dialog_focus_scope;
    nova_control_t *dialog_focus_control=nova_control_create(NOVA_CONTROL_BUTTON);
    nova_control_t *right_focus_control=nova_control_create(NOVA_CONTROL_BUTTON);
    nova_control_set_state(dialog_focus_control,NOVA_CONTROL_INITIALIZED);
    nova_control_set_state(dialog_focus_control,NOVA_CONTROL_VISIBLE);
    nova_control_set_bounds(dialog_focus_control,(nova_rect_t){10,10,8,3});
    nova_control_set_state(right_focus_control,NOVA_CONTROL_INITIALIZED);
    nova_control_set_state(right_focus_control,NOVA_CONTROL_VISIBLE);
    nova_control_set_bounds(right_focus_control,(nova_rect_t){10,2,6,3});
    failed|=check(nova_input_focus_scope_configure(&main_focus_scope,1,item)&&
        nova_input_focus_scope_register(&main_focus_scope,item)&&
        nova_input_focus_scope_register(&main_focus_scope,item2)&&
        nova_input_focus_scope_register(&main_focus_scope,right_focus_control)&&
        nova_input_focus_scope_build_navigation(&main_focus_scope,NOVA_NAV_EDGE_WRAP)&&
        nova_input_focus_enter_scope(&main_focus_scope)&&nova_input_focus_get()==item&&
        nova_input_focus_next()&&nova_input_focus_get()==item2&&
        nova_input_focus_previous()&&nova_input_focus_get()==item,
        "Focus Scope mit Default und O(1)-Vor-/Zuruecknavigation");
    failed|=check(nova_input_focus_set(item)&&
        nova_input_neighbor_find(item,NOVA_DIRECTION_DOWN)==item2&&
        nova_input_focus_navigate(NOVA_DIRECTION_DOWN)&&nova_input_focus_get()==item2,
        "Explizite Richtungsnavigation hat Vorrang");
    failed|=check(nova_input_focus_set(item)&&nova_input_focus_navigate(NOVA_DIRECTION_RIGHT)&&
        nova_input_focus_get()==right_focus_control,
        "Geometrische Navigation nach rechts");
    failed|=check(nova_input_focus_navigate(NOVA_DIRECTION_LEFT)&&
        nova_input_focus_get()==item,"Geometrische Navigation nach links");
    failed|=check(nova_input_neighbor_find(item,NOVA_DIRECTION_UP)==item2&&
        nova_input_focus_navigate(NOVA_DIRECTION_UP)&&
        nova_input_focus_get()==item2,"Vorberechnetes Wrap am oberen Rand");
    failed|=check(nova_input_directional_diagnostics()->explicit_hits==2&&
        nova_input_directional_diagnostics()->geometric_hits==3&&
        nova_input_directional_diagnostics()->wraps==1,
        "Diagnosen fuer explizite, geometrische und Wrap-Navigation");
    failed|=check(nova_input_focus_scope_configure(&dialog_focus_scope,2,
            dialog_focus_control)&&
        nova_input_focus_scope_register(&dialog_focus_scope,dialog_focus_control)&&
        nova_input_focus_get()==item2&&
        nova_input_focus_enter_scope(&dialog_focus_scope)&&
        nova_input_focus_get()==dialog_focus_control&&
        !nova_input_focus_set(item)&&nova_input_focus_leave_scope()&&
        nova_input_focus_get()==item2&&nova_input_focus_leave_scope()&&
        nova_input_focus_get()==item2&&nova_input_focus_scope()==0&&
        nova_input_focus_diagnostics()->restores==2&&
        nova_input_focus_diagnostics()->announcements>=7&&
        nova_input_focus_visible(),
        "Modale Focus Scope Isolation und atomare Wiederherstellung");
    dialog_focus_control->flags|=NOVA_CONTROL_FLAG_DECORATIVE;
    failed|=check(!nova_input_focus_set(dialog_focus_control),
        "Dekorative Controls erhalten niemals Fokus");
    dialog_focus_control->flags&=~NOVA_CONTROL_FLAG_DECORATIVE;
    failed |= check(nova_input_pointer_capture(0,item2)&&
                    nova_input_pointer_is_captured(0)&&
                    nova_input_pointer_capture_owner(0)==item2&&
                    nova_input_pointer_capture_state(0)==NOVA_CAPTURE_ACTIVE&&
                    !nova_input_pointer_capture(0,item)&&
                    !nova_input_pointer_capture(NOVA_INPUT_POINTER_CAPACITY,item),
                    "O(1)-Pointer Capture mit exklusivem validiertem Slot");
    nova_input_event_t pointer={.type=NOVA_EVENT_POINTER_MOVE};
    pointer.pointer.pointer_id=0;pointer.pointer.x=2;pointer.pointer.y=2;
    failed |= check(nova_input_pointer_target(&pointer)==item2, "Capture vor Hit Test");
    nova_input_advance_time(25);
    nova_input_pointer_release(0);
    failed |= check(!nova_input_pointer_is_captured(0)&&
                    nova_input_pointer_capture_state(0)==NOVA_CAPTURE_RELEASED&&
                    nova_input_pointer_capture_diagnostics(0)->owner_id==item2->id&&
                    nova_input_pointer_capture_diagnostics(0)->duration_ms==25&&
                    nova_input_pointer_capture_diagnostics(0)->releases==1&&
                    nova_input_pointer_target(&pointer)==item,
                    "Hit Test nach definierter Capture-Freigabe");
    control_event_count=0;
    failed|=check(nova_input_pointer_capture(0,item)&&
                  (nova_input_advance_time(40),true)&&
                  nova_input_device_set(2,NOVA_DEVICE_MOUSE,false)&&
                  nova_input_pointer_capture_state(0)==NOVA_CAPTURE_CANCELLED&&
                  nova_input_pointer_capture_diagnostics(0)->owner_id==item->id&&
                  nova_input_pointer_capture_diagnostics(0)->duration_ms==40&&
                  nova_input_pointer_capture_diagnostics(0)->cancels==1&&
                  control_event_count==2&&
                  control_event_types[0]==NOVA_CONTROL_EVENT_CAPTURE_CANCEL&&
                  control_event_types[1]==NOVA_CONTROL_EVENT_CAPTURE_CANCEL&&
                  nova_input_diagnostics()->capture_releases==1&&
                  nova_input_diagnostics()->capture_cancels==1&&
                  nova_input_diagnostics()->capture_errors==2,
                  "Geraeteverlust bricht Capture deterministisch ab");
    failed|=check(nova_input_device_set(2,NOVA_DEVICE_MOUSE,true),
                  "Maus nach Capture-Abbruch erneut anmelden");
    control_event_count=0;
    nova_input_advance_time(10);
    nova_dialog_t *capture_dialog=0;
    nova_dialog_result_t capture_dialog_result=NOVA_DIALOG_RESULT_NONE;
    failed|=check(nova_input_pointer_capture(0,item)&&
        (nova_input_advance_time(15),true)&&
        (capture_dialog=nova_dialog_open(NOVA_DIALOG_INFORMATION,"Capture",
            "Dialogwechsel",true,true,item->id))!=0&&
        nova_input_pointer_capture_state(0)==NOVA_CAPTURE_CANCELLED&&
        nova_input_pointer_capture_diagnostics(0)->duration_ms==15&&
        nova_input_pointer_capture_diagnostics(0)->cancels==2&&
        control_event_count==2&&nova_dialog_cancel(&capture_dialog_result)&&
        capture_dialog_result==NOVA_DIALOG_RESULT_CANCEL,
        "Dialogoeffnung cancelt Capture vor dem Modalwechsel");
    control_event_count=0;nova_recovery_initialize();
    failed|=check(nova_input_pointer_capture(0,item)&&
        (nova_input_advance_time(20),true)&&
        nova_recovery_report(0x80010022u,NOVA_UI_SUBSYSTEM_INPUT,
            NOVA_UI_ERROR_RECOVERABLE,NOVA_RECOVERY_LOCAL_FALLBACK,100)&&
        nova_input_pointer_capture_state(0)==NOVA_CAPTURE_CANCELLED&&
        nova_input_pointer_capture_diagnostics(0)->duration_ms==20&&
        nova_input_pointer_capture_diagnostics(0)->cancels==3&&
        control_event_count==2,
        "Recovery-Eintritt cancelt Capture und routet das Ereignis");
    while(nova_input_next(&received)){}
    failed |= check(nova_input_repeat_start(40,100), "Input Repeat starten");
    nova_input_repeat_update(500);
    failed |= check(nova_input_next(&received) && received.key.repeat&&
        nova_input_repeat_context(40)->state==NOVA_REPEAT_REPEATING&&
        nova_input_repeat_context(40)->count==1,
        "deterministisches Input Repeat nach Initial Delay");
    nova_input_repeat_stop(40);
    failed|=check(!nova_input_repeat_active(40)&&
        nova_input_repeat_context(40)->state==NOVA_REPEAT_STOPPED&&
        nova_input_repeat_context(40)->duration_ms==400,
        "Repeat Stop speichert Zustand und Dauer");
    failed|=check(nova_input_repeat_configure(600,100,true)&&
        !nova_input_repeat_configure(99,100,true),
        "Zentrale Repeat- und Accessibility-Zeitkonfiguration");
    nova_input_event_t navigation_repeat={.device_id=1,.type=NOVA_EVENT_NAVIGATION};
    navigation_repeat.navigation.direction=NOVA_DIRECTION_DOWN;
    nova_input_event_t character_repeat={.device_id=1,.type=NOVA_EVENT_CHARACTER};
    character_repeat.key.key=41;character_repeat.key.unicode='A';
    failed|=check(nova_input_repeat_start_event(41,&navigation_repeat,600,false)&&
        nova_input_repeat_start_event(42,&character_repeat,600,false)&&
        !nova_input_repeat_start_event(43,&character_repeat,600,true)&&
        nova_input_repeat_pause(42)&&!nova_input_repeat_pause(42),
        "Mehrere Repeat-Kontexte, Pause und Schutz kritischer Aktionen");
    nova_input_repeat_update(1200);
    failed|=check(nova_input_next(&received)&&received.type==NOVA_EVENT_NAVIGATION&&
        !nova_input_next(&received)&&nova_input_repeat_context(41)->count==1&&
        nova_input_repeat_context(42)->state==NOVA_REPEAT_PAUSED&&
        nova_input_repeat_resume(42,1200),
        "Navigation wiederholt, pausierter Kontext bleibt still");
    failed|=check(nova_input_focus_set(item)&&!nova_input_repeat_active(41)&&
        !nova_input_repeat_active(42)&&nova_input_repeat_diagnostics()->focus_stops==2,
        "Fokuswechsel stoppt alle zielgebundenen Wiederholungen");
    nova_input_event_t wheel_repeat={.device_id=2,.type=NOVA_EVENT_POINTER_WHEEL};
    wheel_repeat.pointer.wheel_y=-1;
    failed|=check(nova_input_repeat_start_event(44,&wheel_repeat,1300,false)&&
        nova_input_device_set(2,NOVA_DEVICE_MOUSE,false)&&
        !nova_input_repeat_active(44)&&nova_input_repeat_diagnostics()->device_stops>=1,
        "Geraeteverlust stoppt zugehoerigen Repeat-Kontext");
    failed|=check(nova_input_repeat_configure(600,100,false)&&
        !nova_input_repeat_start_event(45,&character_repeat,1400,false)&&
        nova_input_repeat_configure(400,50,true),
        "Accessibility kann Repeat vollstaendig deaktivieren");
    failed |= check(!nova_input_device_connected(2),"Hot Removal beendet Geraetezustand");
    nova_input_event_t disconnected_pointer={.device_id=2,.type=NOVA_EVENT_POINTER_MOVE};
    failed|=check(!nova_input_device_connected(2)&&
        nova_input_device_get(2)->state==NOVA_DEVICE_STATE_DISCONNECTED&&
        nova_input_focus_get()==item&&!nova_input_post(disconnected_pointer)&&
        !nova_input_device_set(2,NOVA_DEVICE_KEYBOARD,true)&&
        !nova_input_device_set(18,NOVA_DEVICE_MOUSE,true)&&
        nova_input_diagnostics()->device_errors>=2,
        "Hot Removal erhaelt Fokus und sperrt ungueltige Geraete");
    nova_input_initialize();
    failed|=check(nova_input_device_set(1,NOVA_DEVICE_KEYBOARD,true)&&
        nova_input_device_set(2,NOVA_DEVICE_MOUSE,true)&&
        nova_input_trace_status()->initialized&&!nova_input_trace_enabled()&&
        !nova_input_trace_set_enabled(true,false)&&nova_input_trace_set_enabled(true,true)&&
        nova_input_trace_status()->passive&&nova_input_trace_status()->boot_unaffected,
        "Input Tracing ist vorallokiert, passiv und nur nutzergesteuert aktiv");
    while(nova_input_next(&received)){}
    nova_input_set_secure(true);
    nova_input_event_t traced_key={.timestamp=100,.device_id=1,.type=NOVA_EVENT_KEY_DOWN};
    traced_key.key.key=30;traced_key.key.unicode='X';traced_key.key.modifiers=2;
    failed|=check(nova_input_post(traced_key)&&nova_input_next(&received)&&
        nova_input_trace_complete(received.sequence,110,120,item->id,1,0,true)&&
        nova_input_trace_get(0)->masked&&nova_input_trace_get(0)->unicode==0&&
        nova_input_trace_get(0)->target_object==item->id&&
        nova_input_trace_get(0)->handler_duration_us==10&&
        nova_input_trace_get(0)->render_triggered,
        "Secure Keyboard Trace maskiert Text und speichert den Lebenszyklus");
    nova_input_set_secure(false);
    nova_input_event_t traced_pointer={.timestamp=200,.device_id=2,.type=NOVA_EVENT_POINTER_MOVE};
    traced_pointer.pointer.pointer_id=4;traced_pointer.pointer.x=12;traced_pointer.pointer.y=34;
    failed|=check(nova_input_post(traced_pointer)&&nova_input_next(&received)&&
        nova_input_trace_complete(received.sequence,205,210,item2->id,1,2,false)&&
        nova_input_trace_get(1)->type==NOVA_TRACE_MOUSE_MOVE&&
        nova_input_trace_get(1)->x==12&&nova_input_trace_get(1)->dialog_id==2,
        "Maus Trace mit Ziel, Fenster, Dialog und Timing");
    nova_input_trace_filter_t input_filter={.device_type=NOVA_DEVICE_MOUSE,
        .type=NOVA_TRACE_MOUSE_MOVE,.target_object=item2->id,.dialog_id=2,
        .use_device_type=true,.use_type=true,.use_target=true,.use_dialog=true};
    failed|=check(nova_input_trace_query(&input_filter,0)==nova_input_trace_get(1)&&
        nova_input_trace_query(&input_filter,1)==0,
        "Input Trace Filter fuer Geraet, Typ, Control und Dialog");
    nova_input_trace_reset();
    for(uint32_t i=0;i<NOVA_INPUT_TRACE_CAPACITY+4u;++i){
        nova_input_event_t load={.timestamp=1000+i,.device_id=1,.type=NOVA_EVENT_KEY_DOWN};
        load.key.key=i+1;(void)nova_input_post(load);(void)nova_input_next(&received);
    }
    static uint8_t input_export[16384];uint32_t input_export_size=0;
    failed|=check(nova_input_trace_status()->overwritten==4&&
        nova_input_trace_get(0)->scan_code==5&&
        !nova_input_trace_export(NOVA_INPUT_TRACE_JSON,false,input_export,
            sizeof(input_export),&input_export_size)&&
        nova_input_trace_export(NOVA_INPUT_TRACE_NDF,true,input_export,
            sizeof(input_export),&input_export_size)&&input_export_size>10&&
        nova_input_trace_export(NOVA_INPUT_TRACE_JSON,true,input_export,
            sizeof(input_export),&input_export_size)&&input_export_size>20&&
        nova_input_trace_export(NOVA_INPUT_TRACE_CSV,true,input_export,
            sizeof(input_export),&input_export_size)&&input_export_size>60&&
        nova_input_trace_export(NOVA_INPUT_TRACE_BINARY,true,input_export,
            sizeof(input_export),&input_export_size)&&input_export_size==2056,
        "Input Ringrotation und NDF-, JSON-, CSV-, Binaerexport nach Benutzeraktion");
    (void)nova_input_trace_set_enabled(false,true);

    nova_diag_initialize();
    for(uint32_t i=0;i<NOVA_DIAG_CAPACITY+3u;++i)
        nova_diag_log((nova_diag_event_t){i,NOVA_DIAG_INFO,1,i,0,0});
    failed |= check(nova_diag_statistics()->overwritten==3 &&
                    nova_diag_get(0)->event_id==3, "heapfreier Diagnose-Ringpuffer");
    nova_diag_initialize();
    failed|=check(nova_diag_framework_status()->initialized&&
        nova_diag_framework_status()->registered_modules==NOVA_DIAG_AREA_COUNT&&
        !nova_diag_framework_status()->live_enabled&&
        nova_diag_framework_status()->read_only&&
        nova_diag_framework_status()->boot_unaffected&&
        nova_diag_configure_capacity(8)&&
        nova_diag_register_module(0x8000u,"Installer"),
        "modulares Diagnoseframework und konfigurierbarer Ringpuffer");
    nova_diag_event_descriptor_t structured={
        .event={100,NOVA_DIAG_ERROR,NOVA_DIAG_AREA_RENDERING,0x1002u,7,9},
        .module="Compositor",.source="Present",.message="Frame rejected",
        .metadata="surface=boot",.cause="Invalid frame",.recommendation="Use safe renderer",
        .phase=NOVA_DIAG_PHASE_UI,.error_class=0x44u};
    nova_diag_event_descriptor_t secret_event={
        .event={110,NOVA_DIAG_CRITICAL,NOVA_DIAG_AREA_SECURITY,0x2001u,0,0},
        .module="Security",.source="TPM",.message="private-key-material",
        .metadata="recovery-secret",.phase=NOVA_DIAG_PHASE_FIRMWARE,
        .error_class=0x55u,.sensitive=true};
    failed|=check(nova_diag_log_extended(&structured)&&nova_diag_log_extended(&secret_event)&&
        nova_diag_get_record(0)->sequence==1&&
        nova_diag_get_record(0)->phase==NOVA_DIAG_PHASE_UI&&
        nova_diag_get_record(1)->redacted&&
        nova_diag_get_record(1)->message[0]=='['&&
        nova_diag_framework_status()->redactions==1,
        "strukturierte Ereignisse, Fehlerkontext, Chronologie und Datenschutz");
    nova_diag_filter_t filter={.minimum_level=NOVA_DIAG_ERROR,
        .maximum_level=NOVA_DIAG_CRITICAL,.phase=NOVA_DIAG_PHASE_UI,
        .category=NOVA_DIAG_AREA_RENDERING,.error_class=0x44u,.module="Compositor",
        .use_level=true,.use_phase=true,.use_category=true,.use_error_class=true,
        .use_module=true};
    failed|=check(nova_diag_query(&filter,0)==nova_diag_get_record(0)&&
        nova_diag_query(&filter,1)==0,
        "Diagnosefilter fuer Modul, Schweregrad, Kategorie, Phase und Fehlerklasse");
    static uint8_t diagnostic_export[4096];uint32_t export_size=0;
    failed|=check(!nova_diag_export(NOVA_DIAG_EXPORT_JSON,false,diagnostic_export,
        sizeof(diagnostic_export),&export_size)&&
        nova_diag_export(NOVA_DIAG_EXPORT_NDF,true,diagnostic_export,
            sizeof(diagnostic_export),&export_size)&&export_size>6&&
        nova_diag_export(NOVA_DIAG_EXPORT_JSON,true,diagnostic_export,
            sizeof(diagnostic_export),&export_size)&&export_size>20&&
        nova_diag_export(NOVA_DIAG_EXPORT_TEXT,true,diagnostic_export,
            sizeof(diagnostic_export),&export_size)&&export_size>10&&
        nova_diag_export(NOVA_DIAG_EXPORT_BINARY,true,diagnostic_export,
            sizeof(diagnostic_export),&export_size)&&export_size==32&&
        nova_diag_framework_status()->exports==4&&
        nova_diag_framework_status()->denied_exports==1,
        "NDF-, JSON-, Text- und Binaerexport nur nach Benutzeraktion");
    failed|=check(!nova_diag_set_live(true,false)&&
        nova_diag_set_live(true,true)&&nova_diag_framework_status()->live_enabled&&
        nova_diag_set_live(false,true)&&!nova_diag_framework_status()->live_enabled,
        "Live-Diagnose standardmaessig aus und nur nutzergesteuert");
    nova_diag_reset();
    failed|=check(!nova_diag_get_record(0)&&nova_diag_framework_status()->count==0,
        "Diagnose-Reset leert Protokoll deterministisch");
    failed|=check(nova_boot_perf_record_startup(30000,10000,20000,50000,100000)&&
        nova_boot_perf_startup()->valid&&
        !nova_boot_perf_record_startup(30001,10000,20000,50000,100000),
        "UI-, Theme-, Resource-, Window- und First-Frame-Startbudgets");
    failed|=check(nova_boot_budget_initialize()&&
        nova_boot_budget_set_start_type(NOVA_BOOT_START_STANDARD,3),
        "Startup-Budget fuer Standardstart und VM-Hardwareklasse");
    uint64_t boot_now=0,ui_complete_time=0;
    for(uint8_t phase=0;phase<NOVA_BOOT_PHASE_COUNT;++phase){
        const nova_boot_time_budget_t *record=nova_boot_budget_phase((nova_boot_phase_t)phase);
        failed|=check(record&&record->budget_us&&
            nova_boot_budget_begin_at((nova_boot_phase_t)phase,boot_now)&&
            nova_boot_budget_end_at((nova_boot_phase_t)phase,boot_now+record->budget_us),
            "Bootphase besitzt ein festes Budget und akzeptiert den Grenzwert");
        boot_now+=record->budget_us;
        if(phase==NOVA_BOOT_PHASE_UI)ui_complete_time=boot_now;
    }
    failed|=check(nova_boot_budget_first_frame(ui_complete_time+50000)&&
        nova_boot_budget_diagnostics()->ui_to_first_frame_us==50000&&
        nova_boot_budget_diagnostics()->completed_phases==NOVA_BOOT_PHASE_COUNT&&
        nova_boot_budget_diagnostics()->violations==0&&
        nova_boot_budget_diagnostics()->performance_class==NOVA_BOOT_TIME_CLASS_A&&
        nova_boot_budget_diagnostics()->within_budget&&
        nova_boot_budget_diagnostics()->security_checks_preserved,
        "Alle Startphasen, 50-ms-Erstframe, Klasse A und Sicherheitsvorrang");
    failed|=check(nova_boot_budget_initialize()&&
        nova_boot_budget_begin_at(NOVA_BOOT_PHASE_GRAPHICS,100)&&
        nova_boot_budget_end_at(NOVA_BOOT_PHASE_GRAPHICS,100101)&&
        nova_boot_budget_phase(NOVA_BOOT_PHASE_GRAPHICS)->exceeded&&
        nova_boot_budget_phase(NOVA_BOOT_PHASE_GRAPHICS)->violations==1&&
        nova_boot_budget_diagnostics()->violations==1&&
        nova_boot_budget_diagnostics()->optimization_hints==1&&
        !nova_boot_budget_diagnostics()->within_budget,
        "Startup-Ueberschreitung erzeugt Phase, Dauer, Diagnose und Hinweis");
    static const uint64_t startup_class_times[]={1000000,1500000,2000000,3000000,3000001};
    static const nova_boot_time_class_t startup_classes[]={NOVA_BOOT_TIME_CLASS_A,
        NOVA_BOOT_TIME_CLASS_B,NOVA_BOOT_TIME_CLASS_C,NOVA_BOOT_TIME_CLASS_D,
        NOVA_BOOT_TIME_CLASS_E};
    for(uint8_t i=0;i<5;++i){
        (void)nova_boot_budget_initialize();
        failed|=check(nova_boot_budget_begin_at(NOVA_BOOT_PHASE_STAGE1,0)&&
            nova_boot_budget_end_at(NOVA_BOOT_PHASE_STAGE1,startup_class_times[i])&&
            nova_boot_budget_diagnostics()->performance_class==startup_classes[i],
            "Startup-Performanceklasse A bis E an exakter Grenze");
    }
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
    nova_diag_snapshot();
    const nova_render_statistics_t *render_current=nova_render_statistics_current();
    const nova_render_statistics_t *render_session=
        nova_render_statistics_average(NOVA_RENDER_WINDOW_SESSION);
    failed|=check(nova_render_statistics_frame_count()==2&&
        nova_render_statistics_frame(0)->frame_id==1&&
        nova_render_statistics_frame(1)->duration_us==40000&&
        render_current->minimum_frame_us==16000&&render_current->maximum_frame_us==40000&&
        render_current->average_frame_us==28000&&render_current->software_renderer&&
        render_current->total_memory>=render_current->used_memory&&
        render_session->frame_count==2&&render_session->average_frame_us==28000,
        "Rendering Statistics fuer Frame, Sitzung, Speicher und Softwarepfad");
    static uint8_t render_export[4096];uint32_t render_export_size=0;
    failed|=check(!nova_render_statistics_export(NOVA_RENDER_EXPORT_CSV,false,
        render_export,sizeof(render_export),&render_export_size)&&
        nova_render_statistics_export(NOVA_RENDER_EXPORT_NDF,true,render_export,
            sizeof(render_export),&render_export_size)&&render_export_size>10&&
        nova_render_statistics_export(NOVA_RENDER_EXPORT_JSON,true,render_export,
            sizeof(render_export),&render_export_size)&&render_export_size>20&&
        nova_render_statistics_export(NOVA_RENDER_EXPORT_CSV,true,render_export,
            sizeof(render_export),&render_export_size)&&render_export_size>70&&
        nova_render_statistics_export(NOVA_RENDER_EXPORT_BINARY,true,render_export,
            sizeof(render_export),&render_export_size)&&render_export_size==16,
        "Rendering Statistics als NDF, JSON, CSV und Binaer nur nach Benutzeraktion");
    failed|=check(nova_performance_regression_initialize("host-test","1.0",1,1)&&
        nova_performance_regression_set_baseline(NOVA_BENCH_BOOT_TIME,100,true,500)&&
        nova_performance_regression_set_baseline(NOVA_BENCH_FPS,100,false,300)&&
        nova_performance_regression_set_baseline(NOVA_BENCH_FRAME_TIME,100,true,500)&&
        nova_performance_regression_set_baseline(NOVA_BENCH_MEMORY,100,true,500)&&
        nova_performance_regression_measure(NOVA_BENCH_BOOT_TIME,105)&&
        nova_performance_regression_measure(NOVA_BENCH_FPS,96)&&
        nova_performance_regression_measure(NOVA_BENCH_FRAME_TIME,111)&&
        nova_performance_regression_measure(NOVA_BENCH_MEMORY,121)&&
        nova_performance_regression_results()[NOVA_BENCH_BOOT_TIME].level==NOVA_REGRESSION_NONE&&
        nova_performance_regression_results()[NOVA_BENCH_FPS].level==NOVA_REGRESSION_MINOR&&
        nova_performance_regression_results()[NOVA_BENCH_FRAME_TIME].level==NOVA_REGRESSION_MAJOR&&
        nova_performance_regression_results()[NOVA_BENCH_MEMORY].level==NOVA_REGRESSION_CRITICAL&&
        nova_performance_regression_status()->deterministic&&
        nova_performance_regression_status()->read_only&&
        nova_performance_regression_status()->ci_ready,
        "Performance-Baselines und Regressionsklassen an normativen Schwellen");
    static uint8_t regression_report[2048];uint32_t regression_report_size=0;
    failed|=check(!nova_performance_regression_generate_report(false,regression_report,
            sizeof(regression_report),&regression_report_size)&&
        nova_performance_regression_generate_report(true,regression_report,
            sizeof(regression_report),&regression_report_size)&&regression_report_size>80&&
        nova_performance_regression_status()->reports==1,
        "CI-lesbarer Performance-Regressionsbericht nur nach Autorisierung");
    failed|=check(nova_fallback_test_initialize(),"Fallback-Testpool initialisieren");
    static const char *fallback_names[]={"Software Renderer","Standard Theme","Standard Font",
        "Placeholder Graphic","Standard Animation","Recovery Mode","Safe Mode",
        "Minimal Mode","Low End Profile"};
    for(uint8_t mode=0;mode<NOVA_FALLBACK_MODE_COUNT;++mode)
        failed|=check(nova_fallback_test_execute((nova_fallback_mode_t)mode),fallback_names[mode]);
    const nova_fallback_test_status_t *fallback_test=nova_fallback_test_status();
    failed|=check(fallback_test->executed==NOVA_FALLBACK_MODE_COUNT&&
        fallback_test->successful==NOVA_FALLBACK_MODE_COUNT&&!fallback_test->failed&&
        fallback_test->deterministic&&fallback_test->heap_free&&
        fallback_test->security_preserved&&fallback_test->interactive,
        "Alle Fallbackpfade bleiben sicher und bedienbar");
    static uint8_t fallback_report[4096];uint32_t fallback_report_size=0;
    failed|=check(!nova_fallback_test_generate_report(false,fallback_report,
            sizeof(fallback_report),&fallback_report_size)&&
        nova_fallback_test_generate_report(true,fallback_report,sizeof(fallback_report),
            &fallback_report_size)&&fallback_report_size>150&&
        nova_fallback_test_status()->reports==1,
        "Autorisierter CI-Bericht fuer alle Fallbackmodi");
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
