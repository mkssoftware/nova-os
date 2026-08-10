#include "controls.h"
#include "text.h"
#include "unicode.h"
#include "icons.h"
#include "branding.h"
#include "scene_graph.h"
#include "rounded_geometry.h"
#include "render_quality.h"

static nova_control_t controls[NOVA_CONTROL_CAPACITY];
static bool used[NOVA_CONTROL_CAPACITY];
static nova_control_style_t default_style;
static uint16_t focused_id = NOVA_CONTROL_NONE;
static nova_control_diagnostics_t diagnostics;
static uint16_t list_entries[NOVA_CONTROL_CAPACITY][NOVA_LIST_ITEM_CAPACITY];
static uint16_t list_entry_count[NOVA_CONTROL_CAPACITY];
static uint64_t list_selection_mask[NOVA_CONTROL_CAPACITY];
static nova_style_descriptor_t styles[NOVA_STYLE_CAPACITY];
static nova_control_template_t templates[NOVA_TEMPLATE_CAPACITY];
static nova_scene_node_t *scene_nodes[NOVA_CONTROL_CAPACITY];

static void copy_text(char *destination, const char *source)
{
    uint32_t i = 0;
    if (source) for (; i + 1 < NOVA_CONTROL_TEXT_CAPACITY && source[i]; ++i)
        destination[i] = source[i];
    destination[i] = 0;
}

static bool text_equal(const char *left,const char *right)
{
    if(!left||!right)return false;
    while(*left&&*right)if(*left++!=*right++)return false;
    return *left==*right;
}

static uint32_t default_template_parts(nova_control_type_t type)
{
    switch(type){
    case NOVA_CONTROL_BUTTON:return NOVA_TEMPLATE_PART_BACKGROUND|NOVA_TEMPLATE_PART_BORDER|
        NOVA_TEMPLATE_PART_CONTENT|NOVA_TEMPLATE_PART_FOCUS;
    case NOVA_CONTROL_ICON_BUTTON:return NOVA_TEMPLATE_PART_BACKGROUND|NOVA_TEMPLATE_PART_BORDER|
        NOVA_TEMPLATE_PART_ICON|NOVA_TEMPLATE_PART_FOCUS;
    case NOVA_CONTROL_MENU_BUTTON:return NOVA_TEMPLATE_PART_BACKGROUND|NOVA_TEMPLATE_PART_BORDER|
        NOVA_TEMPLATE_PART_TEXT|NOVA_TEMPLATE_PART_ICON|NOVA_TEMPLATE_PART_FOCUS;
    case NOVA_CONTROL_LABEL:return NOVA_TEMPLATE_PART_TEXT;
    case NOVA_CONTROL_ICON:return NOVA_TEMPLATE_PART_ICON;
    case NOVA_CONTROL_IMAGE:return NOVA_TEMPLATE_PART_IMAGE;
    case NOVA_CONTROL_LIST:return NOVA_TEMPLATE_PART_CONTENT|NOVA_TEMPLATE_PART_FOCUS;
    case NOVA_CONTROL_LIST_ITEM:return NOVA_TEMPLATE_PART_BACKGROUND|NOVA_TEMPLATE_PART_ICON|
        NOVA_TEMPLATE_PART_TEXT|NOVA_TEMPLATE_PART_STATUS|NOVA_TEMPLATE_PART_FOCUS;
    case NOVA_CONTROL_SCROLLBAR:return NOVA_TEMPLATE_PART_TRACK|NOVA_TEMPLATE_PART_THUMB;
    case NOVA_CONTROL_SLIDER:return NOVA_TEMPLATE_PART_TRACK|NOVA_TEMPLATE_PART_THUMB|
        NOVA_TEMPLATE_PART_FOCUS;
    case NOVA_CONTROL_PROGRESS:return NOVA_TEMPLATE_PART_TRACK|NOVA_TEMPLATE_PART_STATUS;
    case NOVA_CONTROL_STATUS_BADGE:return NOVA_TEMPLATE_PART_BACKGROUND|
        NOVA_TEMPLATE_PART_BORDER|NOVA_TEMPLATE_PART_ICON|NOVA_TEMPLATE_PART_TEXT;
    case NOVA_CONTROL_SEPARATOR:return NOVA_TEMPLATE_PART_BORDER|NOVA_TEMPLATE_PART_TEXT;
    default:return NOVA_TEMPLATE_PART_BACKGROUND|NOVA_TEMPLATE_PART_CONTENT;
    }
}

void nova_controls_initialize(const nova_control_style_t *style)
{
    nova_scene_initialize();
    for (uint16_t i = 0; i < NOVA_CONTROL_CAPACITY; ++i) {
        used[i] = false;scene_nodes[i]=0;
    }
    for(uint16_t i=0;i<NOVA_STYLE_CAPACITY;++i)styles[i]=(nova_style_descriptor_t){0};
    for(uint16_t i=0;i<NOVA_TEMPLATE_CAPACITY;++i)templates[i]=(nova_control_template_t){0};
    default_style = style ? *style : (nova_control_style_t){
        0xff22262au,0xfff4f6f8u,0xff4cc2ffu,0xff66686au,
        0xff46515cu,0xff4cc2ffu,0xff48d597u,0xffffb020u,0xffff4f64u,6,1,8
    };
    focused_id = NOVA_CONTROL_NONE;
    diagnostics = (nova_control_diagnostics_t){0};
    nova_style_define(0,NOVA_CONTROL_NONE,&default_style,NOVA_STYLE_ALL,
        NOVA_CONTROL_MATERIAL_OPAQUE);
    for(uint16_t type=0;type<=NOVA_CONTROL_BREADCRUMB;++type){
        uint32_t parts=default_template_parts((nova_control_type_t)type);
        nova_control_template_define((uint16_t)(type+1),NOVA_CONTROL_NONE,
            (nova_control_type_t)type,parts,parts,4);
    }
}

nova_control_t *nova_control_create(nova_control_type_t type)
{
    if (type > NOVA_CONTROL_BREADCRUMB) return 0;
    for (uint16_t i = 0; i < NOVA_CONTROL_CAPACITY; ++i) if (!used[i]) {
        used[i] = true;
        controls[i]=(nova_control_t){0};
        controls[i].id=i;controls[i].parent=NOVA_CONTROL_NONE;
        controls[i].first_child=NOVA_CONTROL_NONE;controls[i].next_sibling=NOVA_CONTROL_NONE;
        controls[i].type=type;controls[i].state=NOVA_CONTROL_CREATED;
        controls[i].flags=NOVA_CONTROL_FLAG_ENABLED|NOVA_CONTROL_FLAG_DIRTY;
        controls[i].style=default_style;controls[i].maximum=1000;controls[i].step=1;
        controls[i].style_id=0;controls[i].visual_template_id=(uint16_t)(type+1);
        if(type==NOVA_CONTROL_BUTTON)controls[i].minimum=-1;
        controls[i].template_id=(uint16_t)type;
        if(type==NOVA_CONTROL_LABEL)controls[i].template_id=NOVA_LABEL_STANDARD;
        if(type==NOVA_CONTROL_IMAGE){controls[i].template_id=NOVA_IMAGE_FIT;
            controls[i].value=NOVA_IMAGE_BRANDING_LOGO;}
        if(type==NOVA_CONTROL_SEPARATOR){controls[i].template_id=NOVA_SEPARATOR_HORIZONTAL;
            controls[i].flags|=NOVA_CONTROL_FLAG_DECORATIVE;
            controls[i].flags&=~NOVA_CONTROL_FLAG_ENABLED;}
        if(type==NOVA_CONTROL_LIST){controls[i].template_id=NOVA_LIST_SELECTION_SINGLE;
            controls[i].value=NOVA_CONTROL_NONE;controls[i].minimum=0;
            controls[i].maximum=NOVA_LIST_ITEM_CAPACITY;}
        if(type==NOVA_CONTROL_STATUS_BADGE){controls[i].template_id=NOVA_BADGE_INFORMATION;
            controls[i].minimum=-1;controls[i].flags|=NOVA_CONTROL_FLAG_READONLY;
            controls[i].flags&=~NOVA_CONTROL_FLAG_ENABLED;}
        controls[i].maximum_length=NOVA_CONTROL_TEXT_CAPACITY-1;
        controls[i].input_mode=type==NOVA_CONTROL_PASSWORD_FIELD?
                               NOVA_TEXT_INPUT_PASSWORD:NOVA_TEXT_INPUT_STANDARD;
        if(type==NOVA_CONTROL_PASSWORD_FIELD)controls[i].flags|=NOVA_CONTROL_FLAG_PASSWORD;
        if(!nova_control_template_apply(&controls[i],(uint16_t)(type+1))){used[i]=false;return 0;}
        scene_nodes[i]=nova_scene_create(NOVA_SCENE_CONTROL);
        if(!scene_nodes[i]||!nova_scene_attach(nova_scene_root(),scene_nodes[i])){
            used[i]=false;return 0;}
        ++diagnostics.created; ++diagnostics.active;
        return &controls[i];
    }
    return 0;
}

static bool can_transition(nova_control_state_t from, nova_control_state_t to)
{
    if (to == NOVA_CONTROL_DESTROYED) return from != NOVA_CONTROL_DESTROYED;
    if (from == NOVA_CONTROL_CREATED) return to == NOVA_CONTROL_INITIALIZED;
    if (from == NOVA_CONTROL_INITIALIZED) return to == NOVA_CONTROL_VISIBLE ||
                                                to == NOVA_CONTROL_DISABLED;
    if (from == NOVA_CONTROL_VISIBLE) return to == NOVA_CONTROL_ACTIVE ||
                                            to == NOVA_CONTROL_DISABLED;
    if (from == NOVA_CONTROL_ACTIVE) return to == NOVA_CONTROL_VISIBLE ||
                                           to == NOVA_CONTROL_DISABLED;
    if (from == NOVA_CONTROL_DISABLED) return to == NOVA_CONTROL_VISIBLE;
    return false;
}

bool nova_control_set_state(nova_control_t *control, nova_control_state_t state)
{
    if (!control || !used[control->id] || !can_transition(control->state, state)) {
        ++diagnostics.invalid_transitions; return false;
    }
    control->state = state;
    if (state == NOVA_CONTROL_VISIBLE || state == NOVA_CONTROL_ACTIVE)
        control->flags |= NOVA_CONTROL_FLAG_VISIBLE | NOVA_CONTROL_FLAG_ENABLED;
    if(control->type==NOVA_CONTROL_LABEL||control->type==NOVA_CONTROL_ICON||
       control->type==NOVA_CONTROL_IMAGE||control->type==NOVA_CONTROL_SEPARATOR||
       control->type==NOVA_CONTROL_STATUS_BADGE)
        control->flags&=~NOVA_CONTROL_FLAG_ENABLED;
    if (state == NOVA_CONTROL_DISABLED)
        control->flags &= ~NOVA_CONTROL_FLAG_ENABLED;
    control->flags |= NOVA_CONTROL_FLAG_DIRTY;
    nova_scene_set_visibility(scene_nodes[control->id],
        state==NOVA_CONTROL_VISIBLE||state==NOVA_CONTROL_ACTIVE?
        NOVA_SCENE_VISIBLE:state==NOVA_CONTROL_DESTROYED?
        NOVA_SCENE_COLLAPSED:NOVA_SCENE_HIDDEN);
    nova_scene_set_enabled(scene_nodes[control->id],
        (control->flags&NOVA_CONTROL_FLAG_ENABLED)!=0);
    return true;
}

bool nova_control_destroy(nova_control_t *control)
{
    if (!control || !used[control->id] || control->first_child!=NOVA_CONTROL_NONE||
        !nova_control_set_state(control, NOVA_CONTROL_DESTROYED))
        return false;
    if(control->type==NOVA_CONTROL_PASSWORD_FIELD)nova_text_field_clear(control);
    if(!nova_scene_destroy(scene_nodes[control->id]))return false;
    scene_nodes[control->id]=0;
    if (focused_id == control->id) focused_id = NOVA_CONTROL_NONE;
    used[control->id] = false; --diagnostics.active; ++diagnostics.destroyed;
    return true;
}

bool nova_control_set_parent(nova_control_t *child, nova_control_t *parent)
{
    if (!child || !parent || child == parent || !used[child->id] || !used[parent->id] ||
        child->parent != NOVA_CONTROL_NONE) return false;
    for (nova_control_t *p = parent; p && p->parent != NOVA_CONTROL_NONE;
         p = &controls[p->parent]) if (p->parent == child->id) return false;
    child->parent = parent->id;
    child->next_sibling = parent->first_child;
    parent->first_child = child->id;
    if(!nova_scene_attach(scene_nodes[parent->id],scene_nodes[child->id]))return false;
    return true;
}

bool nova_control_set_bounds(nova_control_t *control, nova_rect_t bounds)
{
    if (!control || !used[control->id] || bounds.width <= 0 || bounds.height <= 0) return false;
    control->bounds = bounds; control->flags |= NOVA_CONTROL_FLAG_DIRTY;
    return nova_scene_set_bounds(scene_nodes[control->id],bounds);
}

bool nova_control_set_text(nova_control_t *control, const char *text)
{
    if (!control || !text) return false;
    uint16_t source_length=0;
    while(text[source_length]&&source_length<NOVA_CONTROL_TEXT_CAPACITY)++source_length;
    if(text[source_length]||((control->type==NOVA_CONTROL_TEXT_FIELD||
       control->type==NOVA_CONTROL_PASSWORD_FIELD)&&source_length>control->maximum_length))
        return false;
    if(control->type==NOVA_CONTROL_TEXT_FIELD||control->type==NOVA_CONTROL_PASSWORD_FIELD){
        const char *cursor=text;uint32_t cp;
        while(*cursor)if(!nova_unicode_next(&cursor,&cp))return false;
    }
    copy_text(control->text, text);
    control->text_length=control->caret=source_length;
    control->selection_start=control->selection_end=source_length;
    control->flags |= NOVA_CONTROL_FLAG_DIRTY; return true;
}

static bool is_text_field(const nova_control_t *control)
{
    return control&&(control->type==NOVA_CONTROL_TEXT_FIELD||
                     control->type==NOVA_CONTROL_PASSWORD_FIELD);
}

bool nova_text_field_set_placeholder(nova_control_t *control,const char *placeholder)
{
    if(!is_text_field(control)||!placeholder)return false;
    copy_text(control->placeholder,placeholder);return nova_control_invalidate(control);
}

bool nova_text_field_set_mode(nova_control_t *control,nova_text_input_mode_t mode)
{
    if(!is_text_field(control)||mode>NOVA_TEXT_INPUT_SEARCH)return false;
    if(control->type==NOVA_CONTROL_PASSWORD_FIELD&&mode!=NOVA_TEXT_INPUT_PASSWORD)return false;
    control->input_mode=mode;return nova_control_invalidate(control);
}

bool nova_text_field_set_maximum(nova_control_t *control,uint16_t maximum)
{
    if(!is_text_field(control)||!maximum||maximum>=NOVA_CONTROL_TEXT_CAPACITY||
       control->text_length>maximum)return false;
    control->maximum_length=maximum;return true;
}

static uint8_t encode_utf8(uint32_t cp,char out[4])
{
    if(cp<=0x7f){out[0]=(char)cp;return 1;}
    if(cp<=0x7ff){out[0]=(char)(0xc0|(cp>>6));out[1]=(char)(0x80|(cp&63));return 2;}
    if(cp>=0xd800&&cp<=0xdfff)return 0;
    if(cp<=0xffff){out[0]=(char)(0xe0|(cp>>12));out[1]=(char)(0x80|((cp>>6)&63));
        out[2]=(char)(0x80|(cp&63));return 3;}
    if(cp<=0x10ffff){out[0]=(char)(0xf0|(cp>>18));out[1]=(char)(0x80|((cp>>12)&63));
        out[2]=(char)(0x80|((cp>>6)&63));out[3]=(char)(0x80|(cp&63));return 4;}
    return 0;
}

static bool accepts_codepoint(const nova_control_t *control,uint32_t cp)
{
    if(cp<0x20||cp==0x7f)return false;
    if(control->input_mode==NOVA_TEXT_INPUT_NUMERIC)return cp>='0'&&cp<='9';
    if(control->input_mode==NOVA_TEXT_INPUT_HEXADECIMAL)
        return (cp>='0'&&cp<='9')||(cp>='a'&&cp<='f')||(cp>='A'&&cp<='F');
    if(control->input_mode==NOVA_TEXT_INPUT_FILENAME)
        return cp!='/'&&cp!='\\'&&cp!=':'&&cp!='*'&&cp!='?'&&cp!='"'&&cp!='<'&&cp!='>'&&cp!='|';
    return true;
}

static uint16_t previous_boundary(const nova_control_t *control,uint16_t position)
{
    if(!position)return 0;
    --position;
    while(position&&(control->text[position]&0xc0)==0x80)--position;
    return position;
}

static uint16_t next_boundary(const nova_control_t *control,uint16_t position)
{
    if(position>=control->text_length)return control->text_length;
    ++position;
    while(position<control->text_length&&(control->text[position]&0xc0)==0x80)++position;
    return position;
}

static void erase_range(nova_control_t *control,uint16_t start,uint16_t end)
{
    if(start>end){uint16_t swap=start;start=end;end=swap;}
    if(end>control->text_length)end=control->text_length;
    uint16_t remove=(uint16_t)(end-start);
    for(uint16_t i=start;i+remove<=control->text_length;++i)
        control->text[i]=control->text[i+remove];
    control->text_length=(uint16_t)(control->text_length-remove);
    control->caret=start;control->selection_start=control->selection_end=start;
}

bool nova_text_field_insert(nova_control_t *control,uint32_t codepoint)
{
    char encoded[4];
    if(!is_text_field(control)||(control->flags&(NOVA_CONTROL_FLAG_READONLY|
       NOVA_CONTROL_FLAG_LOCKED|NOVA_CONTROL_FLAG_BUSY))||!accepts_codepoint(control,codepoint))
        return false;
    uint8_t bytes=encode_utf8(codepoint,encoded);if(!bytes)return false;
    if(control->selection_start!=control->selection_end)
        erase_range(control,control->selection_start,control->selection_end);
    if(control->text_length+bytes>control->maximum_length)return false;
    for(uint16_t i=control->text_length+1;i>control->caret;--i)
        control->text[i+bytes-1]=control->text[i-1];
    for(uint8_t i=0;i<bytes;++i)control->text[control->caret+i]=encoded[i];
    control->caret=(uint16_t)(control->caret+bytes);
    control->text_length=(uint16_t)(control->text_length+bytes);
    control->selection_start=control->selection_end=control->caret;
    ++diagnostics.value_changes;return nova_control_invalidate(control);
}

bool nova_text_field_backspace(nova_control_t *control)
{
    if(!is_text_field(control)||(control->flags&(NOVA_CONTROL_FLAG_READONLY|
       NOVA_CONTROL_FLAG_LOCKED|NOVA_CONTROL_FLAG_BUSY)))return false;
    if(control->selection_start!=control->selection_end)
        erase_range(control,control->selection_start,control->selection_end);
    else if(control->caret){uint16_t start=previous_boundary(control,control->caret);
        erase_range(control,start,control->caret);}else return false;
    ++diagnostics.value_changes;return nova_control_invalidate(control);
}

bool nova_text_field_delete(nova_control_t *control)
{
    if(!is_text_field(control)||(control->flags&(NOVA_CONTROL_FLAG_READONLY|
       NOVA_CONTROL_FLAG_LOCKED|NOVA_CONTROL_FLAG_BUSY)))return false;
    if(control->selection_start!=control->selection_end)
        erase_range(control,control->selection_start,control->selection_end);
    else if(control->caret<control->text_length)
        erase_range(control,control->caret,next_boundary(control,control->caret));
    else return false;
    ++diagnostics.value_changes;return nova_control_invalidate(control);
}

bool nova_text_field_move(nova_control_t *control,int16_t codepoints,bool extend)
{
    if(!is_text_field(control))return false;
    uint16_t original=control->caret;
    while(codepoints<0&&control->caret){control->caret=previous_boundary(control,control->caret);++codepoints;}
    while(codepoints>0&&control->caret<control->text_length){control->caret=next_boundary(control,control->caret);--codepoints;}
    if(extend){if(control->selection_start==control->selection_end)control->selection_start=original;
        control->selection_end=control->caret;}
    else control->selection_start=control->selection_end=control->caret;
    return nova_control_invalidate(control);
}

bool nova_text_field_select_all(nova_control_t *control)
{
    if(!is_text_field(control))return false;
    control->selection_start=0;control->selection_end=control->text_length;
    control->caret=control->text_length;return nova_control_invalidate(control);
}

void nova_text_field_clear(nova_control_t *control)
{
    if(!is_text_field(control))return;
    volatile char *secret=(volatile char *)control->text;
    for(uint16_t i=0;i<NOVA_CONTROL_TEXT_CAPACITY;++i)secret[i]=0;
    control->text_length=control->caret=control->selection_start=control->selection_end=0;
    nova_control_invalidate(control);
}

bool nova_control_set_accessibility(nova_control_t *control, uint16_t role,
                                    const char *name, bool decorative)
{
    if (!control || (!decorative && (!name || !*name))) return false;
    control->accessibility_role = role;
    copy_text(control->accessibility_name, name);
    if (decorative) control->flags |= NOVA_CONTROL_FLAG_DECORATIVE;
    else control->flags &= ~NOVA_CONTROL_FLAG_DECORATIVE;
    return true;
}

bool nova_control_set_range(nova_control_t *control, int32_t minimum,
                            int32_t maximum, int32_t value)
{
    if (!control || minimum >= maximum || value < minimum || value > maximum) return false;
    control->minimum = minimum; control->maximum = maximum; control->value = value;
    control->flags |= NOVA_CONTROL_FLAG_DIRTY; return true;
}

bool nova_control_set_step(nova_control_t *control, int32_t step)
{
    if (!control || step <= 0 || step > control->maximum - control->minimum) return false;
    control->step = step;
    return nova_control_invalidate(control);
}

bool nova_control_set_value(nova_control_t *control, int32_t value)
{
    if (!control || value < control->minimum || value > control->maximum) return false;
    control->value = value;
    ++diagnostics.value_changes;
    return nova_control_invalidate(control);
}

bool nova_control_adjust(nova_control_t *control, int32_t steps)
{
    int64_t value;
    if (!control || (control->flags & (NOVA_CONTROL_FLAG_LOCKED |
                                       NOVA_CONTROL_FLAG_READONLY))) return false;
    if (control->type != NOVA_CONTROL_SLIDER && control->type != NOVA_CONTROL_SCROLLBAR &&
        control->type != NOVA_CONTROL_PROGRESS) return false;
    value = (int64_t)control->value + (int64_t)steps * control->step;
    if (value < control->minimum) value = control->minimum;
    if (value > control->maximum) value = control->maximum;
    return nova_control_set_value(control, (int32_t)value);
}

bool nova_control_set_flags(nova_control_t *control, uint32_t flags)
{
    if (!control || !used[control->id] || control->state == NOVA_CONTROL_DESTROYED)
        return false;
    control->flags = flags | NOVA_CONTROL_FLAG_DIRTY;
    nova_scene_set_visibility(scene_nodes[control->id],
        flags&NOVA_CONTROL_FLAG_VISIBLE?NOVA_SCENE_VISIBLE:NOVA_SCENE_HIDDEN);
    nova_scene_set_enabled(scene_nodes[control->id],
        (flags&NOVA_CONTROL_FLAG_ENABLED)!=0);
    return true;
}

bool nova_control_set_checked(nova_control_t *control, bool checked)
{
    if(!control||(control->type!=NOVA_CONTROL_CHECKBOX&&
       control->type!=NOVA_CONTROL_SWITCH&&
       !(control->type==NOVA_CONTROL_BUTTON&&control->template_id==NOVA_BUTTON_TOGGLE)))return false;
    bool current = (control->flags & NOVA_CONTROL_FLAG_CHECKED) != 0;
    if (current == checked && !(control->flags & NOVA_CONTROL_FLAG_INDETERMINATE))
        return true;
    if (checked) control->flags |= NOVA_CONTROL_FLAG_CHECKED;
    else control->flags &= ~NOVA_CONTROL_FLAG_CHECKED;
    control->flags &= ~NOVA_CONTROL_FLAG_INDETERMINATE;
    ++diagnostics.toggles;
    return nova_control_invalidate(control);
}

bool nova_control_toggle(nova_control_t *control)
{
    if (!control || (control->flags & (NOVA_CONTROL_FLAG_LOCKED |
                                       NOVA_CONTROL_FLAG_BUSY))) return false;
    return nova_control_set_checked(control,
        !(control->flags & NOVA_CONTROL_FLAG_CHECKED));
}

bool nova_activity_start(nova_control_t *control)
{
    if(!control||control->type!=NOVA_CONTROL_SPINNER||
       (control->flags&(NOVA_CONTROL_FLAG_LOCKED|NOVA_CONTROL_FLAG_ERROR)))return false;
    control->flags|=NOVA_CONTROL_FLAG_BUSY|NOVA_CONTROL_FLAG_DIRTY;return true;
}
bool nova_activity_stop(nova_control_t *control)
{
    if(!control||control->type!=NOVA_CONTROL_SPINNER)return false;
    control->flags&=~NOVA_CONTROL_FLAG_BUSY;control->flags|=NOVA_CONTROL_FLAG_DIRTY;return true;
}
bool nova_activity_set_style(nova_control_t *control,nova_activity_style_t style)
{
    if(!control||control->type!=NOVA_CONTROL_SPINNER||style>NOVA_ACTIVITY_ARC)return false;
    control->template_id=(uint16_t)style;control->flags|=NOVA_CONTROL_FLAG_DIRTY;return true;
}
bool nova_activity_set_phase(nova_control_t *control,uint16_t phase_per_mille)
{
    if(!control||control->type!=NOVA_CONTROL_SPINNER||phase_per_mille>1000)return false;
    control->value=phase_per_mille;control->flags|=NOVA_CONTROL_FLAG_DIRTY;return true;
}
bool nova_activity_running(const nova_control_t *control)
{
    return control&&control->type==NOVA_CONTROL_SPINNER&&
           (control->flags&NOVA_CONTROL_FLAG_BUSY)!=0;
}

static void sync_scrollbars(nova_control_t *view)
{
    for(uint16_t i=0;i<NOVA_CONTROL_CAPACITY;++i)if(used[i]&&controls[i].parent==view->id&&
       controls[i].type==NOVA_CONTROL_SCROLLBAR){nova_control_t *bar=&controls[i];
        bool vertical=bar->template_id==NOVA_SCROLLBAR_VERTICAL;
        bar->minimum=0;bar->maximum=vertical?view->content_height-view->viewport_height:
            view->content_width-view->viewport_width;
        if(bar->maximum<0)bar->maximum=0;
        bar->value=vertical?view->scroll_y:view->scroll_x;
        bar->viewport_width=view->viewport_width;bar->viewport_height=view->viewport_height;
        bar->content_width=view->content_width;bar->content_height=view->content_height;
        bar->flags|=NOVA_CONTROL_FLAG_DIRTY;
    }
}

bool nova_scroll_view_configure(nova_control_t *view,int32_t viewport_width,
    int32_t viewport_height,int32_t content_width,int32_t content_height)
{
    if(!view||view->type!=NOVA_CONTROL_SCROLL_VIEW||viewport_width<=0||viewport_height<=0||
       content_width<viewport_width||content_height<viewport_height)return false;
    view->viewport_width=viewport_width;view->viewport_height=viewport_height;
    view->content_width=content_width;view->content_height=content_height;
    if(view->scroll_x>content_width-viewport_width)view->scroll_x=content_width-viewport_width;
    if(view->scroll_y>content_height-viewport_height)view->scroll_y=content_height-viewport_height;
    view->flags|=NOVA_CONTROL_FLAG_DIRTY;sync_scrollbars(view);return true;
}

bool nova_scroll_view_scroll_to(nova_control_t *view,int32_t x,int32_t y)
{
    if(!view||view->type!=NOVA_CONTROL_SCROLL_VIEW||view->viewport_width<=0)return false;
    int32_t max_x=view->content_width-view->viewport_width;
    int32_t max_y=view->content_height-view->viewport_height;
    if(x<0)x=0;
    if(y<0)y=0;
    if(x>max_x)x=max_x;
    if(y>max_y)y=max_y;
    bool changed=x!=view->scroll_x||y!=view->scroll_y;
    view->scroll_x=x;view->scroll_y=y;view->flags|=NOVA_CONTROL_FLAG_DIRTY;
    sync_scrollbars(view);return changed;
}
bool nova_scroll_view_scroll_by(nova_control_t *view,int32_t dx,int32_t dy)
{return view?nova_scroll_view_scroll_to(view,view->scroll_x+dx,view->scroll_y+dy):false;}

bool nova_scroll_view_scroll_into_view(nova_control_t *view,const nova_rect_t *child)
{
    if(!view||!child||child->width<=0||child->height<=0)return false;
    int32_t x=view->scroll_x,y=view->scroll_y;
    if(child->x<x)x=child->x;else if(child->x+child->width>x+view->viewport_width)
        x=child->x+child->width-view->viewport_width;
    if(child->y<y)y=child->y;else if(child->y+child->height>y+view->viewport_height)
        y=child->y+child->height-view->viewport_height;
    return nova_scroll_view_scroll_to(view,x,y);
}

bool nova_scrollbar_attach(nova_control_t *scrollbar,nova_control_t *view,
    nova_scrollbar_orientation_t orientation)
{
    if(!scrollbar||scrollbar->type!=NOVA_CONTROL_SCROLLBAR||!view||
       view->type!=NOVA_CONTROL_SCROLL_VIEW||orientation>NOVA_SCROLLBAR_VERTICAL)return false;
    if(scrollbar->parent!=NOVA_CONTROL_NONE||!nova_control_set_parent(scrollbar,view))return false;
    scrollbar->template_id=(uint16_t)orientation;sync_scrollbars(view);return true;
}
bool nova_card_set_type(nova_control_t *card,nova_card_type_t type)
{
    if(!card||card->type!=NOVA_CONTROL_CARD||type>NOVA_CARD_CUSTOM)return false;
    card->template_id=(uint16_t)type;card->flags|=NOVA_CONTROL_FLAG_DIRTY;return true;
}
bool nova_card_add_child(nova_control_t *card,nova_control_t *child)
{return card&&card->type==NOVA_CONTROL_CARD&&nova_control_set_parent(child,card);}
bool nova_tile_set_type(nova_control_t *tile,nova_boot_tile_type_t type)
{
    if(!tile||tile->type!=NOVA_CONTROL_TILE||type>NOVA_TILE_CUSTOM)return false;
    tile->template_id=(uint16_t)type;tile->flags|=NOVA_CONTROL_FLAG_DIRTY;return true;
}
bool nova_tile_set_description(nova_control_t *tile,const char *description)
{
    if(!tile||tile->type!=NOVA_CONTROL_TILE||!description)return false;
    copy_text(tile->placeholder,description);tile->flags|=NOVA_CONTROL_FLAG_DIRTY;return true;
}
bool nova_tile_set_status(nova_control_t *tile,const char *status)
{
    if(!tile||tile->type!=NOVA_CONTROL_TILE||!status)return false;
    copy_text(tile->status_text,status);tile->flags|=NOVA_CONTROL_FLAG_DIRTY;return true;
}
bool nova_icon_button_set_icon(nova_control_t *button,uint16_t icon_id)
{
    if(!button||button->type!=NOVA_CONTROL_ICON_BUTTON||icon_id>=NOVA_ICON_COUNT)return false;
    button->value=icon_id;button->flags|=NOVA_CONTROL_FLAG_DIRTY;return true;
}
bool nova_icon_button_set_tooltip(nova_control_t *button,const char *tooltip)
{
    if(!button||button->type!=NOVA_CONTROL_ICON_BUTTON||!tooltip||!*tooltip)return false;
    copy_text(button->placeholder,tooltip);button->flags|=NOVA_CONTROL_FLAG_DIRTY;return true;
}
bool nova_icon_button_set_action(nova_control_t *button,uint32_t action)
{
    if(!button||button->type!=NOVA_CONTROL_ICON_BUTTON)return false;
    button->action=action;button->flags|=NOVA_CONTROL_FLAG_DIRTY;return true;
}
bool nova_button_set_type(nova_control_t *button,nova_button_type_t type)
{
    if(!button||button->type!=NOVA_CONTROL_BUTTON||type>NOVA_BUTTON_TOGGLE)return false;
    button->template_id=(uint16_t)type;button->flags|=NOVA_CONTROL_FLAG_DIRTY;return true;
}
bool nova_button_set_icon(nova_control_t *button,uint16_t icon_id)
{
    if(!button||button->type!=NOVA_CONTROL_BUTTON||icon_id>=NOVA_ICON_COUNT)return false;
    button->minimum=(int32_t)icon_id;button->flags|=NOVA_CONTROL_FLAG_DIRTY;return true;
}
bool nova_button_set_action(nova_control_t *button,uint32_t action)
{
    if(!button||button->type!=NOVA_CONTROL_BUTTON)return false;
    button->action=action;button->flags|=NOVA_CONTROL_FLAG_DIRTY;return true;
}
bool nova_button_pressed(const nova_control_t *button)
{
    return button&&button->type==NOVA_CONTROL_BUTTON&&
           (button->flags&NOVA_CONTROL_FLAG_PRESSED)!=0;
}
bool nova_menu_button_bind(nova_control_t *button,nova_control_t *menu)
{
    if(!button||button->type!=NOVA_CONTROL_MENU_BUTTON||!menu||
       menu->type!=NOVA_CONTROL_CONTEXT_MENU||!used[menu->id])return false;
    button->value=(int32_t)menu->id;button->flags&=~NOVA_CONTROL_FLAG_EXPANDED;
    button->flags|=NOVA_CONTROL_FLAG_DIRTY;return true;
}
bool nova_menu_button_open(nova_control_t *button)
{
    if(!button||button->type!=NOVA_CONTROL_MENU_BUTTON||
       !(button->flags&NOVA_CONTROL_FLAG_ENABLED)||
       (button->flags&NOVA_CONTROL_FLAG_EXPANDED)||button->value<0||
       button->value>=(int32_t)NOVA_CONTROL_CAPACITY||!used[button->value]||
       controls[button->value].type!=NOVA_CONTROL_CONTEXT_MENU)return false;
    button->flags|=NOVA_CONTROL_FLAG_EXPANDED|NOVA_CONTROL_FLAG_DIRTY;return true;
}
bool nova_menu_button_close(nova_control_t *button)
{
    if(!button||button->type!=NOVA_CONTROL_MENU_BUTTON)return false;
    bool changed=(button->flags&NOVA_CONTROL_FLAG_EXPANDED)!=0;
    button->flags&=~NOVA_CONTROL_FLAG_EXPANDED;button->flags|=NOVA_CONTROL_FLAG_DIRTY;
    return changed;
}
bool nova_menu_button_expanded(const nova_control_t *button)
{
    return button&&button->type==NOVA_CONTROL_MENU_BUTTON&&
           (button->flags&NOVA_CONTROL_FLAG_EXPANDED)!=0;
}
bool nova_label_set_type(nova_control_t *label,nova_label_type_t type)
{
    if(!label||label->type!=NOVA_CONTROL_LABEL||type>NOVA_LABEL_INFORMATION)return false;
    label->template_id=(uint16_t)type;label->flags|=NOVA_CONTROL_FLAG_DIRTY;return true;
}
bool nova_label_set_alignment(nova_control_t *label,nova_control_alignment_t alignment)
{
    if(!label||label->type!=NOVA_CONTROL_LABEL||alignment>NOVA_ALIGN_RIGHT_BOTTOM)return false;
    label->value=(int32_t)alignment;label->flags|=NOVA_CONTROL_FLAG_DIRTY;return true;
}
bool nova_label_set_scale(nova_control_t *label,uint16_t scale_milli)
{
    if(!label||label->type!=NOVA_CONTROL_LABEL||scale_milli<500||scale_milli>3000)
        return false;
    label->step=(int32_t)scale_milli;label->flags|=NOVA_CONTROL_FLAG_DIRTY;return true;
}
const char *nova_label_get_text(const nova_control_t *label)
{return label&&label->type==NOVA_CONTROL_LABEL?label->text:0;}
bool nova_icon_control_set(nova_control_t *icon,uint16_t icon_id)
{
    if(!icon||icon->type!=NOVA_CONTROL_ICON||icon_id>=NOVA_ICON_COUNT)return false;
    icon->value=(int32_t)icon_id;icon->flags|=NOVA_CONTROL_FLAG_DIRTY;return true;
}
uint16_t nova_icon_control_get(const nova_control_t *icon)
{return icon&&icon->type==NOVA_CONTROL_ICON?(uint16_t)icon->value:NOVA_CONTROL_NONE;}
bool nova_image_set_resource(nova_control_t *image,nova_image_resource_t resource)
{
    if(!image||image->type!=NOVA_CONTROL_IMAGE||resource>NOVA_IMAGE_BRANDING_LOGO)return false;
    image->value=(int32_t)resource;image->flags|=NOVA_CONTROL_FLAG_DIRTY;return true;
}
bool nova_image_set_scaling(nova_control_t *image,nova_image_scaling_mode_t mode)
{
    if(!image||image->type!=NOVA_CONTROL_IMAGE||mode>NOVA_IMAGE_CENTER)return false;
    image->template_id=(uint16_t)mode;image->flags|=NOVA_CONTROL_FLAG_DIRTY;return true;
}
bool nova_image_set_tint(nova_control_t *image,uint32_t color)
{
    if(!image||image->type!=NOVA_CONTROL_IMAGE)return false;
    image->style.foreground=color;image->flags|=NOVA_CONTROL_FLAG_DIRTY;return true;
}
bool nova_separator_set_orientation(nova_control_t *separator,
                                    nova_separator_orientation_t orientation)
{
    if(!separator||separator->type!=NOVA_CONTROL_SEPARATOR||
       orientation>NOVA_SEPARATOR_VERTICAL)return false;
    separator->template_id=(uint16_t)orientation;
    separator->flags|=NOVA_CONTROL_FLAG_DIRTY|NOVA_CONTROL_FLAG_DECORATIVE;
    separator->flags&=~NOVA_CONTROL_FLAG_ENABLED;return true;
}
bool nova_separator_set_title(nova_control_t *separator,const char *title)
{
    if(!separator||separator->type!=NOVA_CONTROL_SEPARATOR||!title)return false;
    return nova_control_set_text(separator,title);
}
bool nova_list_add_item(nova_control_t *list,nova_control_t *item)
{
    if(!list||list->type!=NOVA_CONTROL_LIST||!item||item->type!=NOVA_CONTROL_LIST_ITEM||
       item->parent!=NOVA_CONTROL_NONE||list_entry_count[list->id]>=NOVA_LIST_ITEM_CAPACITY)
        return false;
    uint16_t count=list_entry_count[list->id];
    if(!nova_control_set_parent(item,list))return false;
    list_entries[list->id][count]=item->id;list_entry_count[list->id]=(uint16_t)(count+1);
    list->content_height=list_entry_count[list->id];list->flags|=NOVA_CONTROL_FLAG_DIRTY;
    return true;
}
bool nova_list_remove_item(nova_control_t *list,uint16_t index)
{
    if(!list||list->type!=NOVA_CONTROL_LIST||index>=list_entry_count[list->id])return false;
    uint16_t id=list_entries[list->id][index],previous=NOVA_CONTROL_NONE;
    uint16_t child=list->first_child;
    while(child!=NOVA_CONTROL_NONE&&child!=id){previous=child;child=controls[child].next_sibling;}
    if(child==NOVA_CONTROL_NONE)return false;
    if(previous==NOVA_CONTROL_NONE)list->first_child=controls[id].next_sibling;
    else controls[previous].next_sibling=controls[id].next_sibling;
    controls[id].parent=NOVA_CONTROL_NONE;controls[id].next_sibling=NOVA_CONTROL_NONE;
    uint16_t count=list_entry_count[list->id];
    for(uint16_t i=index;i+1<count;++i)list_entries[list->id][i]=list_entries[list->id][i+1];
    list_entry_count[list->id]=(uint16_t)(count-1);list->content_height=count-1;
    uint64_t low=index?list_selection_mask[list->id]&((1ull<<index)-1ull):0;
    uint64_t high=index<63?(list_selection_mask[list->id]>>(index+1))<<index:0;
    list_selection_mask[list->id]=low|high;
    if(list->value==(int32_t)index)list->value=NOVA_CONTROL_NONE;
    else if(list->value>(int32_t)index&&list->value!=NOVA_CONTROL_NONE)--list->value;
    list->flags|=NOVA_CONTROL_FLAG_DIRTY;return true;
}
bool nova_list_set_selection_mode(nova_control_t *list,nova_list_selection_mode_t mode)
{
    if(!list||list->type!=NOVA_CONTROL_LIST||mode>NOVA_LIST_SELECTION_MULTIPLE)return false;
    list->template_id=(uint16_t)mode;
    if(mode==NOVA_LIST_SELECTION_NONE){list_selection_mask[list->id]=0;list->value=NOVA_CONTROL_NONE;}
    else if(mode==NOVA_LIST_SELECTION_SINGLE&&list->value!=NOVA_CONTROL_NONE)
        list_selection_mask[list->id]=1ull<<(uint16_t)list->value;
    list->flags|=NOVA_CONTROL_FLAG_DIRTY;return true;
}
bool nova_list_select(nova_control_t *list,uint16_t index)
{
    if(!list||list->type!=NOVA_CONTROL_LIST||index>=list_entry_count[list->id]||
       list->template_id==NOVA_LIST_SELECTION_NONE)return false;
    nova_control_t *item=&controls[list_entries[list->id][index]];
    if(!(item->flags&NOVA_CONTROL_FLAG_VISIBLE)||!(item->flags&NOVA_CONTROL_FLAG_ENABLED))return false;
    if(list->template_id==NOVA_LIST_SELECTION_SINGLE){
        if(list->value!=NOVA_CONTROL_NONE){nova_control_t *old=&controls[
            list_entries[list->id][(uint16_t)list->value]];
            old->flags&=~NOVA_CONTROL_FLAG_SELECTED;old->flags|=NOVA_CONTROL_FLAG_DIRTY;}
        list_selection_mask[list->id]=1ull<<index;
    }else list_selection_mask[list->id]|=1ull<<index;
    item->flags|=NOVA_CONTROL_FLAG_SELECTED|NOVA_CONTROL_FLAG_DIRTY;
    list->value=index;list->flags|=NOVA_CONTROL_FLAG_DIRTY;return true;
}
uint16_t nova_list_selected_index(const nova_control_t *list)
{return list&&list->type==NOVA_CONTROL_LIST?(uint16_t)list->value:NOVA_CONTROL_NONE;}
uint16_t nova_list_count(const nova_control_t *list)
{return list&&list->type==NOVA_CONTROL_LIST?list_entry_count[list->id]:0;}
bool nova_list_set_virtual_window(nova_control_t *list,uint16_t first,uint16_t count)
{
    if(!list||list->type!=NOVA_CONTROL_LIST||first>list_entry_count[list->id]||
       count>NOVA_LIST_ITEM_CAPACITY-first)return false;
    list->minimum=first;list->maximum=count;list->flags|=NOVA_CONTROL_FLAG_DIRTY;return true;
}
bool nova_list_item_visible(const nova_control_t *list,uint16_t index)
{
    return list&&list->type==NOVA_CONTROL_LIST&&index<list_entry_count[list->id]&&
        index>=(uint16_t)list->minimum&&index<(uint16_t)(list->minimum+list->maximum);
}
bool nova_list_item_set_subtitle(nova_control_t *item,const char *subtitle)
{
    if(!item||item->type!=NOVA_CONTROL_LIST_ITEM||!subtitle)return false;
    copy_text(item->placeholder,subtitle);item->flags|=NOVA_CONTROL_FLAG_DIRTY;return true;
}
bool nova_list_item_set_status(nova_control_t *item,const char *status)
{
    if(!item||item->type!=NOVA_CONTROL_LIST_ITEM||!status)return false;
    copy_text(item->status_text,status);item->flags|=NOVA_CONTROL_FLAG_DIRTY;return true;
}
bool nova_list_item_set_state(nova_control_t *item,nova_list_item_state_t state)
{
    if(!item||item->type!=NOVA_CONTROL_LIST_ITEM||state>NOVA_LIST_ITEM_ERROR)return false;
    item->flags&=~(NOVA_CONTROL_FLAG_HOVERED|NOVA_CONTROL_FLAG_FOCUSED|
        NOVA_CONTROL_FLAG_SELECTED|NOVA_CONTROL_FLAG_ERROR);
    if(state==NOVA_LIST_ITEM_HOVER)item->flags|=NOVA_CONTROL_FLAG_HOVERED;
    else if(state==NOVA_LIST_ITEM_FOCUS)item->flags|=NOVA_CONTROL_FLAG_FOCUSED;
    else if(state==NOVA_LIST_ITEM_SELECTED)item->flags|=NOVA_CONTROL_FLAG_SELECTED;
    else if(state==NOVA_LIST_ITEM_ERROR)item->flags|=NOVA_CONTROL_FLAG_ERROR;
    if(state==NOVA_LIST_ITEM_DISABLED)item->flags&=~NOVA_CONTROL_FLAG_ENABLED;
    else item->flags|=NOVA_CONTROL_FLAG_ENABLED;
    item->template_id=(uint16_t)state;item->flags|=NOVA_CONTROL_FLAG_DIRTY;return true;
}
bool nova_status_badge_set_type(nova_control_t *badge,nova_badge_type_t type)
{
    if(!badge||badge->type!=NOVA_CONTROL_STATUS_BADGE||type>NOVA_BADGE_CUSTOM)return false;
    badge->template_id=(uint16_t)type;badge->flags|=NOVA_CONTROL_FLAG_DIRTY;return true;
}
bool nova_status_badge_set_icon(nova_control_t *badge,uint16_t icon_id)
{
    if(!badge||badge->type!=NOVA_CONTROL_STATUS_BADGE||icon_id>=NOVA_ICON_COUNT)return false;
    badge->minimum=(int32_t)icon_id;badge->flags|=NOVA_CONTROL_FLAG_DIRTY;return true;
}
bool nova_status_badge_set_visible(nova_control_t *badge,bool visible)
{
    if(!badge||badge->type!=NOVA_CONTROL_STATUS_BADGE)return false;
    if(visible)badge->flags|=NOVA_CONTROL_FLAG_VISIBLE;
    else badge->flags&=~NOVA_CONTROL_FLAG_VISIBLE;
    badge->flags|=NOVA_CONTROL_FLAG_DIRTY;return true;
}

bool nova_style_define(uint16_t id,uint16_t parent,const nova_control_style_t *style,
    uint32_t override_mask,nova_control_material_t material)
{
    if(id>=NOVA_STYLE_CAPACITY||!style||override_mask&~NOVA_STYLE_ALL||
       material>NOVA_CONTROL_MATERIAL_TRANSPARENT||parent==id||
       (parent!=NOVA_CONTROL_NONE&&(parent>=NOVA_STYLE_CAPACITY||!styles[parent].valid)))return false;
    nova_control_style_t resolved=parent==NOVA_CONTROL_NONE?default_style:styles[parent].resolved;
    if(override_mask&NOVA_STYLE_BACKGROUND)resolved.background=style->background;
    if(override_mask&NOVA_STYLE_FOREGROUND)resolved.foreground=style->foreground;
    if(override_mask&NOVA_STYLE_ACCENT)resolved.accent=style->accent;
    if(override_mask&NOVA_STYLE_DISABLED)resolved.disabled=style->disabled;
    if(override_mask&NOVA_STYLE_BORDER)resolved.border=style->border;
    if(override_mask&NOVA_STYLE_INFORMATION)resolved.information=style->information;
    if(override_mask&NOVA_STYLE_SUCCESS)resolved.success=style->success;
    if(override_mask&NOVA_STYLE_WARNING)resolved.warning=style->warning;
    if(override_mask&NOVA_STYLE_ERROR)resolved.error=style->error;
    if(override_mask&NOVA_STYLE_GEOMETRY){resolved.corner_dlu=style->corner_dlu;
        resolved.border_dlu=style->border_dlu;resolved.padding_dlu=style->padding_dlu;}
    styles[id]=(nova_style_descriptor_t){id,parent,override_mask,material,resolved,true};return true;
}
const nova_style_descriptor_t *nova_style_get(uint16_t id)
{return id<NOVA_STYLE_CAPACITY&&styles[id].valid?&styles[id]:0;}
bool nova_style_apply(nova_control_t *control,uint16_t style_id)
{
    const nova_style_descriptor_t *style=nova_style_get(style_id);
    if(!control||!style)return false;
    control->style=style->resolved;control->style_id=style_id;
    control->flags|=NOVA_CONTROL_FLAG_DIRTY;return true;
}
void nova_style_theme_changed(void)
{
    for(uint16_t i=0;i<NOVA_CONTROL_CAPACITY;++i)if(used[i]&&
       controls[i].style_id<NOVA_STYLE_CAPACITY&&styles[controls[i].style_id].valid){
        controls[i].style=styles[controls[i].style_id].resolved;
        controls[i].flags|=NOVA_CONTROL_FLAG_DIRTY;
    }
}
bool nova_control_template_define(uint16_t id,uint16_t parent,
    nova_control_type_t control_type,uint32_t parts,uint32_t required_parts,
    uint16_t visual_nodes)
{
    if(!id||id>=NOVA_TEMPLATE_CAPACITY||control_type>NOVA_CONTROL_BREADCRUMB||
       !visual_nodes||parent==id||(required_parts&~(parts|(parent!=NOVA_CONTROL_NONE&&
       parent<NOVA_TEMPLATE_CAPACITY&&templates[parent].valid?templates[parent].resolved_parts:0)))||
       (parent!=NOVA_CONTROL_NONE&&(parent>=NOVA_TEMPLATE_CAPACITY||
        !templates[parent].valid||templates[parent].control_type!=control_type)))return false;
    uint32_t resolved=parts|(parent==NOVA_CONTROL_NONE?0:templates[parent].resolved_parts);
    templates[id]=(nova_control_template_t){id,parent,control_type,parts,resolved,
        required_parts,visual_nodes,true};return true;
}
const nova_control_template_t *nova_control_template_get(uint16_t id)
{return id<NOVA_TEMPLATE_CAPACITY&&templates[id].valid?&templates[id]:0;}
bool nova_control_template_apply(nova_control_t *control,uint16_t template_id)
{
    const nova_control_template_t *templ=nova_control_template_get(template_id);
    if(!control||!templ||templ->control_type!=control->type||
       (templ->required_parts&~templ->resolved_parts))return false;
    control->visual_template_id=template_id;control->flags|=NOVA_CONTROL_FLAG_DIRTY;return true;
}
bool nova_control_template_has_part(const nova_control_template_t *templ,
                                    const char *part_name)
{
    if(!templ||!templ->valid||!part_name)return false;
    uint32_t part=0;
    if(text_equal(part_name,"Background"))part=NOVA_TEMPLATE_PART_BACKGROUND;
    else if(text_equal(part_name,"Border"))part=NOVA_TEMPLATE_PART_BORDER;
    else if(text_equal(part_name,"Icon"))part=NOVA_TEMPLATE_PART_ICON;
    else if(text_equal(part_name,"Text"))part=NOVA_TEMPLATE_PART_TEXT;
    else if(text_equal(part_name,"ContentPresenter"))part=NOVA_TEMPLATE_PART_CONTENT;
    else if(text_equal(part_name,"FocusIndicator"))part=NOVA_TEMPLATE_PART_FOCUS;
    else if(text_equal(part_name,"Track"))part=NOVA_TEMPLATE_PART_TRACK;
    else if(text_equal(part_name,"Thumb"))part=NOVA_TEMPLATE_PART_THUMB;
    else if(text_equal(part_name,"Status"))part=NOVA_TEMPLATE_PART_STATUS;
    else if(text_equal(part_name,"Image"))part=NOVA_TEMPLATE_PART_IMAGE;
    return part&&(templ->resolved_parts&part)!=0;
}

bool nova_control_set_style(nova_control_t *control,
                            const nova_control_style_t *style)
{
    if (!control || !style) return false;
    control->style = *style;control->style_id=NOVA_CONTROL_NONE;
    return nova_control_invalidate(control);
}

bool nova_control_invalidate(nova_control_t *control)
{
    if (!control || !used[control->id] || control->state == NOVA_CONTROL_DESTROYED)
        return false;
    control->flags |= NOVA_CONTROL_FLAG_DIRTY;
    return nova_scene_mark_dirty(scene_nodes[control->id],NOVA_SCENE_DIRTY_RENDER|
        NOVA_SCENE_DIRTY_STATE);
}

bool nova_controls_sync_scene(void)
{
    for(uint16_t i=0;i<NOVA_CONTROL_CAPACITY;++i)if(used[i]){
        nova_scene_visibility_t visibility=(controls[i].flags&NOVA_CONTROL_FLAG_VISIBLE)?
            NOVA_SCENE_VISIBLE:NOVA_SCENE_HIDDEN;
        if(!scene_nodes[i]||!nova_scene_set_bounds(scene_nodes[i],controls[i].bounds)||
           !nova_scene_set_visibility(scene_nodes[i],visibility)||
           !nova_scene_set_enabled(scene_nodes[i],
                (controls[i].flags&NOVA_CONTROL_FLAG_ENABLED)!=0))return false;
        if(controls[i].flags&NOVA_CONTROL_FLAG_DIRTY)
            nova_scene_mark_dirty(scene_nodes[i],NOVA_SCENE_DIRTY_RENDER|
                NOVA_SCENE_DIRTY_STATE);
    }
    return true;
}

bool nova_control_focus(nova_control_t *control)
{
    if (!control || !(control->flags & NOVA_CONTROL_FLAG_VISIBLE) ||
        !(control->flags & NOVA_CONTROL_FLAG_ENABLED)) return false;
    if (focused_id != NOVA_CONTROL_NONE && used[focused_id])
        controls[focused_id].flags &= ~NOVA_CONTROL_FLAG_FOCUSED;
    focused_id = control->id; control->flags |= NOVA_CONTROL_FLAG_FOCUSED |
                                             NOVA_CONTROL_FLAG_DIRTY;
    diagnostics.focused = focused_id;
    return true;
}

nova_control_t *nova_control_hit_test(int32_t x, int32_t y)
{
    ++diagnostics.hit_tests;
    for (uint16_t i = NOVA_CONTROL_CAPACITY; i; --i) if (used[i - 1]) {
        nova_control_t *c = &controls[i - 1]; nova_rect_t b = c->bounds;
        if ((c->flags & (NOVA_CONTROL_FLAG_VISIBLE | NOVA_CONTROL_FLAG_ENABLED)) ==
            (NOVA_CONTROL_FLAG_VISIBLE | NOVA_CONTROL_FLAG_ENABLED) &&
            x >= b.x && y >= b.y && x < b.x + b.width && y < b.y + b.height) return c;
    }
    return 0;
}

bool nova_control_invoke(nova_control_t *control, uint32_t *action)
{
    if (!control || !(control->flags & NOVA_CONTROL_FLAG_ENABLED) ||
        (control->flags & (NOVA_CONTROL_FLAG_BUSY | NOVA_CONTROL_FLAG_LOCKED |
                           NOVA_CONTROL_FLAG_READONLY)) ||
        control->action_fired) { ++diagnostics.rejected_actions; return false; }
    if(control->type==NOVA_CONTROL_CHECKBOX||control->type==NOVA_CONTROL_SWITCH||
       (control->type==NOVA_CONTROL_BUTTON&&control->template_id==NOVA_BUTTON_TOGGLE))
        nova_control_toggle(control);
    control->action_fired = true; control->flags |= NOVA_CONTROL_FLAG_PRESSED;
    if (action) *action = control->action;
    return true;
}

void nova_control_release(nova_control_t *control)
{
    if (control) { control->action_fired = false; control->flags &= ~NOVA_CONTROL_FLAG_PRESSED; }
}

static void control_rounded_rect(nova_surface_t *surface, nova_rect_t rect,
                                 uint16_t radius, uint32_t color)
{
    nova_round_rect_t geometry;
    if(nova_round_rect_create(&geometry,rect.x,rect.y,rect.width,rect.height,radius))
        (void)nova_round_rect_render(surface,&geometry,color,
            nova_render_quality_parameters()->anti_aliasing);
}

static int32_t text_prefix_width(const nova_control_t *control,uint16_t bytes)
{
    char prefix[NOVA_CONTROL_TEXT_CAPACITY];
    if(bytes>control->text_length)bytes=control->text_length;
    for(uint16_t i=0;i<bytes;++i)prefix[i]=control->text[i];
    prefix[bytes]=0;
    return nova_text_measure(prefix,32767).width;
}

void nova_control_render(nova_control_t *control, nova_surface_t *surface)
{
    if (!control || !surface || !(control->flags & NOVA_CONTROL_FLAG_VISIBLE)) return;
    uint32_t color = (control->flags & NOVA_CONTROL_FLAG_ENABLED) ?
                     control->style.background : control->style.disabled;
    nova_rect_t b = control->bounds;
    int32_t radius = control->style.corner_dlu;
    int32_t thickness = control->style.border_dlu ? control->style.border_dlu : 1;
    int32_t range = control->maximum - control->minimum;
    int32_t fraction = range > 0 ? (int32_t)(((int64_t)(control->value-control->minimum)*1000)/range) : 0;
    switch (control->type) {
    case NOVA_CONTROL_TEXT_FIELD:
    case NOVA_CONTROL_PASSWORD_FIELD: {
        control_rounded_rect(surface,b,(uint16_t)radius,color);
        uint32_t border=(control->flags&NOVA_CONTROL_FLAG_ERROR)?control->style.error:
                        (control->flags&NOVA_CONTROL_FLAG_FOCUSED)?control->style.accent:
                        control->style.disabled;
        nova_surface_rect(surface,(nova_rect_t){b.x,b.y,b.width,thickness},border);
        nova_surface_rect(surface,(nova_rect_t){b.x,b.y+b.height-thickness,b.width,thickness},border);
        int32_t pad=control->style.padding_dlu?control->style.padding_dlu:8;
        int32_t text_y=b.y+(b.height-20)/2;
        if(!control->text_length&&control->placeholder[0])
            nova_text_draw(surface,b.x+pad,text_y,b.width-pad*2,control->placeholder,
                           control->style.disabled,NOVA_TEXT_LEFT,true);
        else if(control->type==NOVA_CONTROL_PASSWORD_FIELD||
                (control->flags&NOVA_CONTROL_FLAG_PASSWORD)){
            int32_t x=b.x+pad;
            const char *cursor=control->text;uint32_t cp;
            while(*cursor){const char *before=cursor;
                if(!nova_unicode_next(&cursor,&cp)&&cursor==before)break;
                control_rounded_rect(surface,(nova_rect_t){x,text_y+7,6,6},3,
                                     control->style.foreground);x+=10;
                if(x>b.x+b.width-pad-6)break;
            }
        }else{
            uint16_t start=control->selection_start,end=control->selection_end;
            if(start>end){uint16_t swap=start;start=end;end=swap;}
            if(start!=end){int32_t sx=b.x+pad+text_prefix_width(control,start);
                int32_t ex=b.x+pad+text_prefix_width(control,end);
                nova_surface_rect(surface,(nova_rect_t){sx,b.y+3,ex-sx,b.height-6},
                                  (control->style.accent&0x00ffffffu)|0x66000000u);}
            nova_text_draw(surface,b.x+pad,text_y,b.width-pad*2,control->text,
                           control->style.foreground,NOVA_TEXT_LEFT,true);
        }
        if((control->flags&NOVA_CONTROL_FLAG_FOCUSED)&&
           !(control->flags&NOVA_CONTROL_FLAG_READONLY)){
            int32_t caret_x=b.x+pad+(control->type==NOVA_CONTROL_PASSWORD_FIELD?
                (int32_t)control->caret*10:text_prefix_width(control,control->caret));
            if(caret_x<b.x+b.width-pad)
                nova_surface_rect(surface,(nova_rect_t){caret_x,b.y+5,1,b.height-10},
                                  control->style.accent);
        }
        break;
    }
    case NOVA_CONTROL_CHECKBOX: {
        int32_t side = b.height < b.width ? b.height : b.width;
        control_rounded_rect(surface,(nova_rect_t){b.x,b.y,side,side},radius,color);
        if (control->flags & NOVA_CONTROL_FLAG_CHECKED) {
            int32_t pad=side/4;
            nova_surface_rect(surface,(nova_rect_t){b.x+pad,b.y+side/2,side/4,thickness+1},control->style.accent);
            nova_surface_rect(surface,(nova_rect_t){b.x+side/2-1,b.y+pad,thickness+1,side/2},control->style.accent);
        } else if (control->flags & NOVA_CONTROL_FLAG_INDETERMINATE)
            nova_surface_rect(surface,(nova_rect_t){b.x+side/4,b.y+side/2,side/2,thickness+1},control->style.accent);
        break;
    }
    case NOVA_CONTROL_SWITCH: {
        int32_t track_h=b.height, knob=track_h-4;
        uint32_t track=(control->flags & NOVA_CONTROL_FLAG_CHECKED)?control->style.accent:color;
        control_rounded_rect(surface,b,(uint16_t)(track_h/2),track);
        int32_t knob_x=(control->flags & NOVA_CONTROL_FLAG_CHECKED)?b.x+b.width-knob-2:b.x+2;
        control_rounded_rect(surface,(nova_rect_t){knob_x,b.y+2,knob,knob},(uint16_t)(knob/2),control->style.foreground);
        break;
    }
    case NOVA_CONTROL_SLIDER:
    case NOVA_CONTROL_PROGRESS: {
        int32_t track_h=b.height<8?b.height:8;
        int32_t track_y=b.y+(b.height-track_h)/2;
        control_rounded_rect(surface,(nova_rect_t){b.x,track_y,b.width,track_h},(uint16_t)(track_h/2),color);
        int32_t fill=(int32_t)((int64_t)b.width*fraction/1000);
        if (fill>0) control_rounded_rect(surface,(nova_rect_t){b.x,track_y,fill,track_h},(uint16_t)(track_h/2),control->style.accent);
        if (control->type != NOVA_CONTROL_PROGRESS) {
            int32_t knob=b.height<16?b.height:16;
            int32_t x=b.x+(int32_t)((int64_t)(b.width-knob)*fraction/1000);
            control_rounded_rect(surface,(nova_rect_t){x,b.y+(b.height-knob)/2,knob,knob},(uint16_t)(knob/2),control->style.foreground);
        }
        break;
    }
    case NOVA_CONTROL_SCROLLBAR: {
        bool vertical=control->template_id==NOVA_SCROLLBAR_VERTICAL;
        int32_t viewport=vertical?control->viewport_height:control->viewport_width;
        int32_t content=vertical?control->content_height:control->content_width;
        int32_t track=vertical?b.height:b.width;
        int32_t thumb=content>0?(int32_t)((int64_t)track*viewport/content):track;
        int32_t minimum_thumb=vertical?b.width*2:b.height*2;
        if(thumb<minimum_thumb)thumb=minimum_thumb;
        if(thumb>track)thumb=track;
        int32_t travel=track-thumb;
        int32_t offset=control->maximum>0?(int32_t)((int64_t)travel*control->value/control->maximum):0;
        control_rounded_rect(surface,b,(uint16_t)((vertical?b.width:b.height)/2),color);
        nova_rect_t thumb_rect=vertical?(nova_rect_t){b.x,b.y+offset,b.width,thumb}:
            (nova_rect_t){b.x+offset,b.y,thumb,b.height};
        control_rounded_rect(surface,thumb_rect,(uint16_t)((vertical?b.width:b.height)/2),
                             control->style.accent);
        break;
    }
    case NOVA_CONTROL_CARD: {
        control_rounded_rect(surface,b,(uint16_t)radius,control->style.disabled);
        int32_t border=thickness>0?thickness:1;
        nova_rect_t inner={b.x+border,b.y+border,b.width-border*2,b.height-border*2};
        if(inner.width>0&&inner.height>0)
            control_rounded_rect(surface,inner,(uint16_t)(radius>border?radius-border:0),color);
        uint32_t semantic=control->style.accent;
        if(control->template_id==NOVA_CARD_WARNING)semantic=control->style.warning;
        else if(control->template_id==NOVA_CARD_ERROR)semantic=control->style.error;
        else if(control->template_id==NOVA_CARD_SUCCESS)semantic=control->style.success;
        if(control->template_id>=NOVA_CARD_INFORMATION&&control->template_id<=NOVA_CARD_SUCCESS)
            control_rounded_rect(surface,(nova_rect_t){b.x,b.y,4,b.height},2,semantic);
        break;
    }
    case NOVA_CONTROL_LIST_ITEM: {
        uint32_t background=control->style.background;
        if(control->flags&NOVA_CONTROL_FLAG_PRESSED)background=control->style.disabled;
        control_rounded_rect(surface,b,(uint16_t)radius,background);
        if(control->flags&NOVA_CONTROL_FLAG_ERROR)
            nova_surface_rect(surface,(nova_rect_t){b.x,b.y,4,b.height},control->style.error);
        break;
    }
    case NOVA_CONTROL_TILE: {
        uint32_t background=(control->flags&NOVA_CONTROL_FLAG_SELECTED)?
            control->style.disabled:color;
        control_rounded_rect(surface,b,(uint16_t)radius,background);
        uint32_t marker=control->style.accent;
        if(control->template_id==NOVA_TILE_RECOVERY)marker=control->style.information;
        else if(control->template_id==NOVA_TILE_DIAGNOSTIC)marker=control->style.success;
        else if(control->template_id==NOVA_TILE_DESTRUCTIVE)marker=control->style.error;
        else if(control->template_id==NOVA_TILE_MAINTENANCE)marker=control->style.warning;
        nova_surface_rect(surface,(nova_rect_t){b.x,b.y,4,b.height},marker);
        if(control->flags&NOVA_CONTROL_FLAG_SELECTED){
            nova_surface_rect(surface,(nova_rect_t){b.x,b.y,b.width,2},control->style.accent);
            nova_surface_rect(surface,(nova_rect_t){b.x,b.y+b.height-2,b.width,2},control->style.accent);
            nova_surface_rect(surface,(nova_rect_t){b.x+b.width-2,b.y,2,b.height},control->style.accent);
        }
        break;
    }
    case NOVA_CONTROL_ICON_BUTTON: {
        uint32_t background=(control->flags&(NOVA_CONTROL_FLAG_HOVERED|
            NOVA_CONTROL_FLAG_FOCUSED|NOVA_CONTROL_FLAG_SELECTED))?control->style.background:0;
        if(background)control_rounded_rect(surface,b,(uint16_t)radius,background);
        if(control->flags&NOVA_CONTROL_FLAG_FOCUSED){
            nova_surface_rect(surface,(nova_rect_t){b.x,b.y,b.width,2},control->style.accent);
            nova_surface_rect(surface,(nova_rect_t){b.x,b.y+b.height-2,b.width,2},control->style.accent);
        }
        break;
    }
    case NOVA_CONTROL_BUTTON: {
        uint32_t background=control->style.background;
        if(control->template_id==NOVA_BUTTON_TOGGLE&&
           (control->flags&NOVA_CONTROL_FLAG_CHECKED))background=control->style.accent;
        if(control->flags&(NOVA_CONTROL_FLAG_FOCUSED|NOVA_CONTROL_FLAG_SELECTED)){
            int32_t ring=thickness>1?thickness:2;
            control_rounded_rect(surface,b,(uint16_t)radius,control->style.accent);
            nova_rect_t inner={b.x+ring,b.y+ring,b.width-ring*2,b.height-ring*2};
            if(inner.width>0&&inner.height>0)control_rounded_rect(surface,inner,
                (uint16_t)(radius>ring?radius-ring:0),background);
        }else control_rounded_rect(surface,b,(uint16_t)radius,background);
        int32_t pad=control->style.padding_dlu?control->style.padding_dlu:8;
        int32_t text_x=b.x+pad,text_width=b.width-pad*2;
        if(control->minimum>=0&&control->minimum<NOVA_ICON_COUNT){
            int32_t icon_size=b.height-12;if(icon_size<12)icon_size=12;
            nova_icon_draw(surface,(nova_icon_token_t)control->minimum,text_x,
                           b.y+(b.height-icon_size)/2,(uint16_t)icon_size,
                           control->style.foreground);
            text_x+=icon_size+pad;text_width-=icon_size+pad;
        }
        if(control->text[0])nova_text_draw(surface,text_x,b.y+(b.height-20)/2,
            text_width,control->text,control->style.foreground,NOVA_TEXT_CENTER,true);
        break;
    }
    case NOVA_CONTROL_MENU_BUTTON: {
        uint32_t background=(control->flags&(NOVA_CONTROL_FLAG_HOVERED|
            NOVA_CONTROL_FLAG_FOCUSED|NOVA_CONTROL_FLAG_EXPANDED))?
            control->style.background:control->style.disabled;
        if(control->flags&NOVA_CONTROL_FLAG_FOCUSED){
            int32_t ring=thickness>1?thickness:2;
            control_rounded_rect(surface,b,(uint16_t)radius,control->style.accent);
            nova_rect_t inner={b.x+ring,b.y+ring,b.width-ring*2,b.height-ring*2};
            if(inner.width>0&&inner.height>0)control_rounded_rect(surface,inner,
                (uint16_t)(radius>ring?radius-ring:0),background);
        }else control_rounded_rect(surface,b,(uint16_t)radius,background);
        int32_t pad=control->style.padding_dlu?control->style.padding_dlu:8;
        int32_t arrow_w=10,arrow_x=b.x+b.width-pad-arrow_w;
        nova_text_draw(surface,b.x+pad,b.y+(b.height-20)/2,
            b.width-pad*3-arrow_w,control->text,control->style.foreground,
            NOVA_TEXT_LEFT,true);
        int32_t center_y=b.y+b.height/2;
        bool expanded=(control->flags&NOVA_CONTROL_FLAG_EXPANDED)!=0;
        for(int32_t i=0;i<5;++i){int32_t inset=expanded?4-i:i;
            nova_surface_rect(surface,(nova_rect_t){arrow_x+inset,
                center_y-2+i,arrow_w-inset*2,1},control->style.foreground);}
        break;
    }
    case NOVA_CONTROL_STATUS_BADGE: {
        uint32_t semantic=control->style.information;
        if(control->template_id==NOVA_BADGE_SUCCESS)semantic=control->style.success;
        else if(control->template_id==NOVA_BADGE_WARNING)semantic=control->style.warning;
        else if(control->template_id==NOVA_BADGE_ERROR||
                control->template_id==NOVA_BADGE_CRITICAL)semantic=control->style.error;
        else if(control->template_id==NOVA_BADGE_NEUTRAL)semantic=control->style.border;
        else if(control->template_id==NOVA_BADGE_CUSTOM)semantic=control->style.accent;
        int32_t ring=control->style.border_dlu?control->style.border_dlu:1;
        control_rounded_rect(surface,b,(uint16_t)(b.height/2),semantic);
        nova_rect_t inner={b.x+ring,b.y+ring,b.width-ring*2,b.height-ring*2};
        if(inner.width>0&&inner.height>0)control_rounded_rect(surface,inner,
            (uint16_t)(inner.height/2),(semantic&0x00ffffffu)|0x33000000u);
        int32_t pad=control->style.padding_dlu?control->style.padding_dlu:8;
        int32_t text_x=b.x+pad,text_width=b.width-pad*2;
        if(control->minimum>=0&&control->minimum<NOVA_ICON_COUNT){
            int32_t icon_size=b.height-8;if(icon_size<10)icon_size=10;
            nova_icon_draw(surface,(nova_icon_token_t)control->minimum,text_x,
                b.y+(b.height-icon_size)/2,(uint16_t)icon_size,control->style.foreground);
            text_x+=icon_size+pad/2;text_width-=icon_size+pad/2;
        }
        const char *badge_text=control->text;
        if(control->template_id==NOVA_BADGE_CRITICAL&&!badge_text[0])badge_text="Kritisch";
        if(badge_text[0])nova_text_draw(surface,text_x,b.y+(b.height-20)/2,text_width,
            badge_text,control->style.foreground,NOVA_TEXT_CENTER,true);
        break;
    }
    case NOVA_CONTROL_SEPARATOR: {
        bool vertical=control->template_id==NOVA_SEPARATOR_VERTICAL;
        if(vertical)nova_surface_rect(surface,(nova_rect_t){b.x+b.width/2,b.y,
            thickness,b.height},control->style.disabled);
        else if(!control->text[0])nova_surface_rect(surface,(nova_rect_t){b.x,
            b.y+b.height/2,b.width,thickness},control->style.disabled);
        else{
            int32_t title_width=nova_text_measure(control->text,b.width/2).width;
            int32_t gap=control->style.padding_dlu?control->style.padding_dlu:8;
            int32_t left=(b.width-title_width)/2-gap;
            if(left>0)nova_surface_rect(surface,(nova_rect_t){b.x,b.y+b.height/2,
                left,thickness},control->style.disabled);
            int32_t right_x=b.x+(b.width+title_width)/2+gap;
            if(right_x<b.x+b.width)nova_surface_rect(surface,(nova_rect_t){right_x,
                b.y+b.height/2,b.x+b.width-right_x,thickness},control->style.disabled);
            nova_text_draw(surface,b.x,b.y+(b.height-20)/2,b.width,control->text,
                control->style.foreground,NOVA_TEXT_CENTER,true);
        }
        break;
    }
    case NOVA_CONTROL_SPINNER: {
        if(!(control->flags&NOVA_CONTROL_FLAG_BUSY))break;
        static const int8_t px[8]={0,5,7,5,0,-5,-7,-5};
        static const int8_t py[8]={-7,-5,0,5,7,5,0,-5};
        int32_t size=b.width<b.height?b.width:b.height;
        int32_t dot=size/7;if(dot<2)dot=2;
        int32_t cx=b.x+b.width/2,cy=b.y+b.height/2;
        uint8_t head=(uint8_t)(((uint32_t)control->value*8u/1001u)&7u);
        if(control->template_id==NOVA_ACTIVITY_DOTS){
            for(uint8_t i=0;i<3;++i){bool active=(uint8_t)((head/3u)%3u)==i;
                control_rounded_rect(surface,(nova_rect_t){cx+((int32_t)i-1)*dot*3-dot/2,
                    cy-dot/2,dot,dot},(uint16_t)(dot/2),active?control->style.accent:color);}
        }else for(uint8_t i=0;i<8;++i){
            bool visible=control->template_id==NOVA_ACTIVITY_RING||
                (control->template_id==NOVA_ACTIVITY_ARC?((i+8-head)&7u)<4u:((i+8-head)&7u)<6u);
            if(!visible)continue;
            bool active=i==head||control->template_id==NOVA_ACTIVITY_ARC;
            int32_t x=cx+px[i]*size/18-dot/2,y=cy+py[i]*size/18-dot/2;
            control_rounded_rect(surface,(nova_rect_t){x,y,dot,dot},(uint16_t)(dot/2),
                                 active?control->style.accent:color);
        }
        break;
    }
    case NOVA_CONTROL_LABEL: {
        uint32_t foreground=control->state!=NOVA_CONTROL_DISABLED?
            control->style.foreground:control->style.disabled;
        if(control->template_id==NOVA_LABEL_ERROR)foreground=control->style.error;
        else if(control->template_id==NOVA_LABEL_WARNING)foreground=control->style.warning;
        else if(control->template_id==NOVA_LABEL_SUCCESS)foreground=control->style.success;
        else if(control->template_id==NOVA_LABEL_INFORMATION)foreground=control->style.accent;
        if(control->style.background>>24)control_rounded_rect(surface,b,
            (uint16_t)radius,control->style.background);
        uint8_t horizontal=(uint8_t)(control->value%3),vertical=(uint8_t)(control->value/3);
        nova_text_align_t alignment=horizontal==1?NOVA_TEXT_CENTER:
                                    horizontal==2?NOVA_TEXT_RIGHT:NOVA_TEXT_LEFT;
        uint16_t scale=(uint16_t)(control->step>=500?control->step:1000);
        int32_t line_height=(int32_t)(24u*scale/1000u);
        uint16_t max_lines=(uint16_t)(b.height/line_height);if(!max_lines)max_lines=1;
        int32_t y=b.y;
        int32_t text_height=(int32_t)(20u*scale/1000u);
        if(max_lines==1){if(vertical==1)y=b.y+(b.height-text_height)/2;
            else if(vertical==2)y=b.y+b.height-text_height;
            nova_text_draw_scaled(surface,b.x,y,b.width,control->text,foreground,alignment,true,scale);
        }else nova_text_draw_wrapped_scaled(surface,b.x,y,b.width,max_lines,
                                             control->text,foreground,scale);
        break;
    }
    case NOVA_CONTROL_ICON: {
        uint32_t tint=control->state!=NOVA_CONTROL_DISABLED?
            control->style.foreground:control->style.disabled;
        if(control->flags&NOVA_CONTROL_FLAG_ERROR)tint=control->style.error;
        else if(control->flags&NOVA_CONTROL_FLAG_CHECKED)tint=control->style.success;
        int32_t side=b.width<b.height?b.width:b.height;
        if(side>0&&control->value>=0&&control->value<NOVA_ICON_COUNT)
            nova_icon_draw(surface,(nova_icon_token_t)control->value,
                b.x+(b.width-side)/2,b.y+(b.height-side)/2,(uint16_t)side,tint);
        break;
    }
    case NOVA_CONTROL_IMAGE: {
        if(control->value!=NOVA_IMAGE_BRANDING_LOGO)break;
        int32_t iw=nova_branding_intrinsic_width(),ih=nova_branding_intrinsic_height();
        int32_t width=iw,height=ih;
        if(control->template_id==NOVA_IMAGE_STRETCH){width=b.width;height=b.height;}
        else if(control->template_id==NOVA_IMAGE_FIT){
            if((int64_t)b.width*ih<(int64_t)b.height*iw){width=b.width;height=iw?b.width*ih/iw:0;}
            else{height=b.height;width=ih?b.height*iw/ih:0;}
        }else if(control->template_id==NOVA_IMAGE_FILL){
            if((int64_t)b.width*ih>(int64_t)b.height*iw){width=b.width;height=iw?b.width*ih/iw:0;}
            else{height=b.height;width=ih?b.height*iw/ih:0;}
        }else if(control->template_id==NOVA_IMAGE_ORIGINAL){
            if(width>b.width)width=b.width;
            if(height>b.height)height=b.height;
        }
        int32_t x=b.x+(b.width-width)/2,y=b.y+(b.height-height)/2;
        if(width>0&&height>0)nova_branding_draw_clipped(surface,x,y,(uint16_t)width,
            (uint16_t)height,control->style.foreground,b);
        break;
    }
    default:
        control_rounded_rect(surface,b,(uint16_t)radius,color);
        if (control->type > NOVA_CONTROL_BREADCRUMB) ++diagnostics.fallback_renders;
        break;
    }
    if (control->type!=NOVA_CONTROL_BUTTON&&control->type!=NOVA_CONTROL_MENU_BUTTON&&
        (control->flags & (NOVA_CONTROL_FLAG_FOCUSED | NOVA_CONTROL_FLAG_SELECTED))) {
        nova_rect_t marker = control->bounds;
        marker.width = control->style.border_dlu ? control->style.border_dlu * 4 : 4;
        nova_surface_rect(surface, marker, control->style.accent);
    }
    control->flags &= ~NOVA_CONTROL_FLAG_DIRTY;
    ++diagnostics.renders;
}

const nova_control_diagnostics_t *nova_control_diagnostics(void) { return &diagnostics; }
