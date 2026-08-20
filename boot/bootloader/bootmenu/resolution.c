#include "resolution.h"

static nova_viewport_t viewport;
static nova_resolution_diagnostics_t diagnostics;
static nova_resolution_test_result_t resolution_tests[NOVA_RESOLUTION_TEST_CAPACITY];
static nova_resolution_test_status_t resolution_test_status;
static nova_safe_area_policy_t safe_policy;
static nova_safe_area_diagnostics_t safe_diagnostics;
static nova_scaling_info_t scaling_info;
static nova_scaling_diagnostics_t scaling_diagnostics;

static uint32_t clamp_scale(uint32_t value)
{if(value<1000)return 1000;if(value>3000)return 3000;return value;}
static uint32_t quantize_scale(uint32_t value)
{value=clamp_scale(value);return ((value+125u)/250u)*250u;}
static uint32_t automatic_scale(uint32_t width,uint32_t height,uint32_t dpi,bool reliable)
{
    if(reliable&&dpi>=48&&dpi<=768)return quantize_scale((dpi*1000u+48u)/96u);
    uint32_t horizontal=(uint32_t)(((uint64_t)width*1000u+960u)/1920u);
    uint32_t vertical=(uint32_t)(((uint64_t)height*1000u+540u)/1080u);
    return quantize_scale(horizontal<vertical?horizontal:vertical);
}
static nova_resolution_class_t classify(uint32_t width,uint32_t height)
{
    (void)height;
    if(width<=640)return NOVA_RESOLUTION_XS;
    if(width<=800)return NOVA_RESOLUTION_S;
    if(width<=1024)return NOVA_RESOLUTION_M;
    if(width<1920)return NOVA_RESOLUTION_L;
    if(width<2560)return NOVA_RESOLUTION_XL;
    if(width<3840)return NOVA_RESOLUTION_XXL;
    return NOVA_RESOLUTION_ULTRA;
}
static uint64_t difference(uint64_t left,uint64_t right)
{return left>right?left-right:right-left;}
static nova_aspect_ratio_t aspect(uint32_t width,uint32_t height)
{
    static const uint8_t ratios[][2]={{4,3},{5,4},{16,9},{16,10},{21,9},{32,9}};
    uint64_t best=UINT64_MAX;uint8_t index=0;
    for(uint8_t i=0;i<6;++i){uint64_t d=difference((uint64_t)width*ratios[i][1],
                                                  (uint64_t)height*ratios[i][0]);
        if(d<best){best=d;index=i;}}
    uint64_t base=(uint64_t)height*ratios[index][0];
    return best*100u<=base*3u?(nova_aspect_ratio_t)index:NOVA_ASPECT_CUSTOM;
}
static void calculate_safe_area(void)
{
 uint32_t width=viewport.physical_width,height=viewport.physical_height;
 uint16_t left=safe_policy.left_permille,top=safe_policy.top_permille,
    right=safe_policy.right_permille,bottom=safe_policy.bottom_permille;
 if(safe_policy.display_kind==NOVA_DISPLAY_BIOS_VBE&&safe_policy.overscan){
    if(left<70)left=70;
    if(top<70)top=70;
    if(right<70)right=70;
    if(bottom<70)bottom=70;}
 else if(safe_policy.display_kind==NOVA_DISPLAY_REMOTE&&safe_policy.overscan){
    if(left<60)left=60;
    if(top<60)top=60;
    if(right<60)right=60;
    if(bottom<60)bottom=60;}
 if(safe_policy.accessibility){left=left*4/5;top=top*4/5;
    right=right*4/5;bottom=bottom*4/5;}
 uint32_t ml=safe_policy.reserve_enabled||safe_policy.overscan?width*left/1000u:0,
    mt=safe_policy.reserve_enabled||safe_policy.overscan?height*top/1000u:0,
    mr=safe_policy.reserve_enabled||safe_policy.overscan?width*right/1000u:0,
    mb=safe_policy.reserve_enabled||safe_policy.overscan?height*bottom/1000u:0;
 if(safe_policy.overscan){
    if((uint32_t)safe_policy.firmware_pixels.left>ml)ml=(uint32_t)safe_policy.firmware_pixels.left;
    if((uint32_t)safe_policy.firmware_pixels.top>mt)mt=(uint32_t)safe_policy.firmware_pixels.top;
    if((uint32_t)safe_policy.firmware_pixels.right>mr)mr=(uint32_t)safe_policy.firmware_pixels.right;
    if((uint32_t)safe_policy.firmware_pixels.bottom>mb)mb=(uint32_t)safe_policy.firmware_pixels.bottom;}
 bool invalid=ml+mr>=width||mt+mb>=height;
 if(invalid){ml=mt=mr=mb=0;++safe_diagnostics.fallbacks;
    safe_diagnostics.fallback_active=true;}
 else safe_diagnostics.fallback_active=false;
 viewport.safe_pixels=(nova_rect_t){(int32_t)ml,(int32_t)mt,
    (int32_t)(width-ml-mr),(int32_t)(height-mt-mb)};
 viewport.safe_dlu=(nova_rect_t){nova_pixel_to_logical(viewport.safe_pixels.x),
    nova_pixel_to_logical(viewport.safe_pixels.y),
    nova_pixel_to_logical(viewport.safe_pixels.width),
    nova_pixel_to_logical(viewport.safe_pixels.height)};
 safe_diagnostics.width=width;safe_diagnostics.height=height;safe_diagnostics.dpi=viewport.dpi;
 safe_diagnostics.scale_milli=viewport.scale_milli;
 safe_diagnostics.effective_pixels=(nova_safe_insets_t){(int32_t)ml,(int32_t)mt,
    (int32_t)mr,(int32_t)mb};safe_diagnostics.display_kind=safe_policy.display_kind;
 safe_diagnostics.overscan=safe_policy.overscan;
 safe_diagnostics.accessibility=safe_policy.accessibility;
 ++safe_diagnostics.updates;++safe_diagnostics.invalidations;
}
static void recalculate(void)
{
    uint32_t scale=viewport.automatic?viewport.automatic_scale:viewport.user_scale_milli;
    viewport.scale_milli=clamp_scale(scale);
    viewport.logical_width_dlu=(int32_t)(((uint64_t)viewport.physical_width*1000u+
                                          viewport.scale_milli/2u)/viewport.scale_milli);
    viewport.logical_height_dlu=(int32_t)(((uint64_t)viewport.physical_height*1000u+
                                           viewport.scale_milli/2u)/viewport.scale_milli);
    calculate_safe_area();
    viewport.resolution_class=classify(viewport.physical_width,viewport.physical_height);
    viewport.aspect_ratio=aspect(viewport.physical_width,viewport.physical_height);
    (void)nova_scaling_update(scaling_info.accessibility_scale_milli>1000);
    ++diagnostics.recalculations;++diagnostics.generation;
}
void nova_resolution_initialize(void)
{
    diagnostics=(nova_resolution_diagnostics_t){.initializations=1};
    safe_policy=(nova_safe_area_policy_t){.display_kind=NOVA_DISPLAY_VIRTUAL,
        .left_permille=50,.top_permille=50,.right_permille=50,.bottom_permille=50,
        .reserve_enabled=true};
    safe_diagnostics=(nova_safe_area_diagnostics_t){.initializations=1};
    scaling_diagnostics=(nova_scaling_diagnostics_t){.initializations=1};
    viewport=(nova_viewport_t){.physical_width=1920,.physical_height=1080,.dpi=96,
        .automatic_scale=1000,.user_scale_milli=1000,.automatic=true};recalculate();
}
bool nova_safe_area_initialize(void)
{safe_policy=(nova_safe_area_policy_t){.display_kind=NOVA_DISPLAY_VIRTUAL,
 .left_permille=50,.top_permille=50,.right_permille=50,.bottom_permille=50,
 .reserve_enabled=true};safe_diagnostics=(nova_safe_area_diagnostics_t){.initializations=1};
 calculate_safe_area();return true;}
bool nova_safe_area_update(const nova_safe_area_policy_t *policy)
{
 bool valid=policy&&policy->display_kind<=NOVA_DISPLAY_REMOTE&&
    policy->left_permille<=200&&policy->top_permille<=200&&
    policy->right_permille<=200&&policy->bottom_permille<=200&&
    policy->firmware_pixels.left>=0&&policy->firmware_pixels.top>=0&&
    policy->firmware_pixels.right>=0&&policy->firmware_pixels.bottom>=0;
 if(!valid){safe_policy=(nova_safe_area_policy_t){.display_kind=NOVA_DISPLAY_VIRTUAL};
    ++safe_diagnostics.errors;++safe_diagnostics.fallbacks;calculate_safe_area();
    safe_diagnostics.fallback_active=true;return false;}
 if(safe_policy.display_kind!=policy->display_kind)++safe_diagnostics.display_changes;
 if(safe_policy.overscan!=policy->overscan)++safe_diagnostics.overscan_changes;
 if(safe_policy.accessibility!=policy->accessibility)++safe_diagnostics.accessibility_changes;
 safe_policy=*policy;calculate_safe_area();return true;
}
const nova_rect_t *nova_safe_area_get(void){return &viewport.safe_pixels;}
bool nova_safe_area_contains(nova_rect_t rect)
{++safe_diagnostics.contains_checks;nova_rect_t safe=viewport.safe_pixels;
 bool valid=rect.width>=0&&rect.height>=0&&rect.x>=safe.x&&rect.y>=safe.y&&
    rect.x+rect.width<=safe.x+safe.width&&rect.y+rect.height<=safe.y+safe.height;
 if(!valid)++safe_diagnostics.rejected_rects;
 return valid;}
const nova_safe_area_policy_t *nova_safe_area_policy(void){return &safe_policy;}
const nova_safe_area_diagnostics_t *nova_safe_area_diagnostics(void)
{return &safe_diagnostics;}
bool nova_resolution_configure(uint32_t width,uint32_t height,uint32_t dpi,bool reliable)
{
    if(width<320||height<240||width>7680||height>4320){++diagnostics.invalid_resolutions;return false;}
    if(dpi<48||dpi>768){dpi=96;reliable=false;++diagnostics.fallbacks;}
    viewport.physical_width=width;viewport.physical_height=height;
    if(width!=viewport.physical_width||height!=viewport.physical_height)
        ++safe_diagnostics.resolution_changes;
    viewport.dpi=dpi;viewport.dpi_reliable=reliable;viewport.automatic=true;
    viewport.automatic_scale=automatic_scale(width,height,dpi,reliable);recalculate();return true;
}
bool nova_resolution_set_resolution(uint32_t width,uint32_t height)
{
    if(width<320||height<240||width>7680||height>4320){++diagnostics.invalid_resolutions;return false;}
    if(width==viewport.physical_width&&height==viewport.physical_height)return true;
    ++safe_diagnostics.resolution_changes;
    viewport.physical_width=width;viewport.physical_height=height;
    viewport.automatic_scale=automatic_scale(width,height,viewport.dpi,viewport.dpi_reliable);
    ++diagnostics.resolution_changes;recalculate();return true;
}
bool nova_resolution_set_dpi(uint32_t dpi,bool reliable)
{
    if(dpi<48||dpi>768){++diagnostics.fallbacks;dpi=96;reliable=false;}
    if(dpi==viewport.dpi&&reliable==viewport.dpi_reliable)return true;
    viewport.dpi=dpi;viewport.dpi_reliable=reliable;
    viewport.automatic_scale=automatic_scale(viewport.physical_width,
        viewport.physical_height,dpi,reliable);++diagnostics.dpi_changes;recalculate();return true;
}
bool nova_resolution_set_scale(uint32_t scale_milli)
{
    if(scale_milli<1000||scale_milli>3000){++diagnostics.invalid_scales;return false;}
    viewport.user_scale_milli=scale_milli;viewport.automatic=false;
    ++diagnostics.scale_changes;recalculate();return true;
}
bool nova_resolution_set_automatic(void)
{if(viewport.automatic)return true;viewport.automatic=true;++diagnostics.scale_changes;recalculate();return true;}
int32_t nova_logical_to_pixel(int32_t dlu)
{
    int64_t value=(int64_t)dlu*viewport.scale_milli;++diagnostics.conversions;
    return value>=0?(int32_t)((value+500)/1000):(int32_t)((value-500)/1000);
}
int32_t nova_pixel_to_logical(int32_t pixel)
{
    if(!viewport.scale_milli)return pixel;
    int64_t value=(int64_t)pixel*1000;
    ++diagnostics.conversions;
    return value>=0?(int32_t)((value+viewport.scale_milli/2)/viewport.scale_milli):
                    (int32_t)((value-viewport.scale_milli/2)/viewport.scale_milli);
}
uint32_t nova_scaling_effective(uint32_t global_scale,bool accessibility)
{
 if(global_scale<1000||global_scale>3000){++scaling_diagnostics.fallbacks;
    global_scale=1000;}
 if(!accessibility)return global_scale;
 uint64_t effective=(uint64_t)global_scale*1250u/1000u;
 return effective>3000u?3000u:(uint32_t)effective;
}
bool nova_scaling_initialize(void)
{scaling_diagnostics=(nova_scaling_diagnostics_t){.initializations=1};
 scaling_info=(nova_scaling_info_t){0};return nova_scaling_update(false);}
bool nova_scaling_update(bool accessibility)
{
 uint32_t dpi=viewport.dpi;
 bool invalid=dpi<48||dpi>768||viewport.scale_milli<1000||viewport.scale_milli>3000;
 bool inferred=!viewport.dpi_reliable;
 if(invalid){dpi=96;++scaling_diagnostics.fallbacks;}
 uint32_t accessibility_scale=accessibility?1250:1000;
 if(scaling_info.dpi_x!=dpi)++scaling_diagnostics.dpi_updates;
 if(scaling_info.global_scale_milli!=viewport.scale_milli)++scaling_diagnostics.scale_updates;
 if(scaling_info.accessibility_scale_milli!=accessibility_scale)
    ++scaling_diagnostics.accessibility_updates;
 scaling_info=(nova_scaling_info_t){.dpi_x=dpi,.dpi_y=dpi,
    .global_scale_milli=invalid?1000:viewport.scale_milli,
    .accessibility_scale_milli=accessibility_scale,
    .effective_scale_milli=nova_scaling_effective(invalid?1000:viewport.scale_milli,
       accessibility),.generation=scaling_info.generation+1,
    .dpi_reliable=!invalid&&viewport.dpi_reliable,.fallback_active=invalid||inferred};
 ++scaling_diagnostics.updates;++scaling_diagnostics.invalidations;return !invalid;
}
static int32_t scale_with(uint32_t scale,int32_t logical)
{
 int64_t product=(int64_t)logical*scale;
 if(product%1000)++scaling_diagnostics.rounding_events;
 int64_t rounded=product>=0?(product+500)/1000:(product-500)/1000;
 if(rounded>INT32_MAX){++scaling_diagnostics.overflows;return INT32_MAX;}
 if(rounded<INT32_MIN){++scaling_diagnostics.overflows;return INT32_MIN;}
 return (int32_t)rounded;
}
int32_t nova_scale_to_pixels(int32_t logical_value)
{++scaling_diagnostics.conversions;return scale_with(scaling_info.effective_scale_milli?
 scaling_info.effective_scale_milli:1000,logical_value);}
bool nova_scale_interval(int32_t start_dlu,int32_t extent_dlu,
 int32_t *pixel_start,int32_t *pixel_extent)
{
 if(!pixel_start||!pixel_extent||extent_dlu<0){++scaling_diagnostics.errors;return false;}
 uint32_t scale=scaling_info.effective_scale_milli?scaling_info.effective_scale_milli:1000;
 int32_t start=scale_with(scale,start_dlu),end=scale_with(scale,start_dlu+extent_dlu);
 if(end<start){++scaling_diagnostics.errors;return false;}
 *pixel_start=start;*pixel_extent=end-start;++scaling_diagnostics.interval_snaps;return true;
}
const nova_scaling_info_t *nova_scaling_get(void){return &scaling_info;}
const nova_scaling_diagnostics_t *nova_scaling_diagnostics(void)
{return &scaling_diagnostics;}
const nova_viewport_t *nova_resolution_viewport(void){return &viewport;}
const nova_resolution_diagnostics_t *nova_resolution_diagnostics(void){return &diagnostics;}
bool nova_resolution_test_initialize(void)
{
    for(uint8_t i=0;i<NOVA_RESOLUTION_TEST_CAPACITY;++i)
        resolution_tests[i]=(nova_resolution_test_result_t){0};
    resolution_test_status=(nova_resolution_test_status_t){.initialized=true,.isolated=true,
        .deterministic=true,.configuration_unchanged=true};return true;
}
bool nova_resolution_test_execute(uint32_t width,uint32_t height,uint32_t scale_percent)
{
    if(!resolution_test_status.initialized||resolution_test_status.count==NOVA_RESOLUTION_TEST_CAPACITY||
       width<320||height<240||width>7680||height>4320||scale_percent<100||scale_percent>200||
       scale_percent%25)return false;
    nova_viewport_t saved=viewport;nova_resolution_diagnostics_t saved_diagnostics=diagnostics;
    bool configured=nova_resolution_configure(width,height,96,false)&&
        nova_resolution_set_scale(scale_percent*10u);
    const nova_viewport_t *current=&viewport;
    bool safe=current->safe_pixels.x>=0&&current->safe_pixels.y>=0&&
        current->safe_pixels.width>0&&current->safe_pixels.height>0&&
        (uint32_t)(current->safe_pixels.x+current->safe_pixels.width)<=width&&
        (uint32_t)(current->safe_pixels.y+current->safe_pixels.height)<=height;
    bool logical=current->logical_width_dlu>=320&&current->logical_height_dlu>=240;
    bool controls=current->safe_dlu.width>=280&&current->safe_dlu.height>=200;
    nova_resolution_test_result_t result={.width=width,.height=height,
        .scale_percent=scale_percent,.aspect_ratio=current->aspect_ratio,
        .resolution_class=current->resolution_class,
        .logical_width_dlu=current->logical_width_dlu,
        .logical_height_dlu=current->logical_height_dlu,.safe_area_valid=safe,
        .layout_valid=configured&&safe&&logical,.rendering_valid=configured&&safe,
        .controls_reachable=controls,.dialog_visible=controls,.text_readable=logical};
    result.detected_errors=(!result.layout_valid)+(!result.rendering_valid)+
        (!result.controls_reachable)+(!result.dialog_visible)+(!result.text_readable);
    viewport=saved;diagnostics=saved_diagnostics;
    resolution_tests[resolution_test_status.count++]=result;++resolution_test_status.executed;
    if(!result.detected_errors)++resolution_test_status.passed;else ++resolution_test_status.failed;
    resolution_test_status.configuration_unchanged=viewport.physical_width==saved.physical_width&&
        viewport.physical_height==saved.physical_height&&viewport.scale_milli==saved.scale_milli;
    return !result.detected_errors;
}
const nova_resolution_test_result_t *nova_resolution_test_results(void){return resolution_tests;}
const nova_resolution_test_status_t *nova_resolution_test_status(void){return &resolution_test_status;}
bool nova_resolution_test_generate_report(bool authorized,uint8_t *output,uint32_t capacity,
    uint32_t *written)
{
    if(written)*written=0;
    if(!authorized||!output||!written)return false;
    const char *header="NOVA-RESOLUTION-COMPATIBILITY-1\nwidth height scale aspect class logical_width logical_height layout render controls dialog text errors\n";
    uint32_t position=0;while(*header){if(position>=capacity)return false;output[position++]=(uint8_t)*header++;}
    for(uint8_t i=0;i<resolution_test_status.count;++i){nova_resolution_test_result_t *record=&resolution_tests[i];
        uint64_t values[13]={record->width,record->height,record->scale_percent,
            record->aspect_ratio,record->resolution_class,(uint32_t)record->logical_width_dlu,
            (uint32_t)record->logical_height_dlu,record->layout_valid,record->rendering_valid,
            record->controls_reachable,record->dialog_visible,record->text_readable,
            record->detected_errors};
        for(uint8_t v=0;v<13;++v){char digits[24];uint8_t count=0;
            do{digits[count++]=(char)('0'+values[v]%10u);values[v]/=10u;}while(values[v]);
            while(count){if(position>=capacity)return false;output[position++]=(uint8_t)digits[--count];}
            if(position>=capacity)return false;
            output[position++]=(uint8_t)(v==12?'\n':' ');}}
    *written=position;++resolution_test_status.reports;return true;
}
