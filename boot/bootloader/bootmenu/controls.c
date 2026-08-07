#include "controls.h"
#include "text.h"
#include "unicode.h"

static nova_control_t controls[NOVA_CONTROL_CAPACITY];
static bool used[NOVA_CONTROL_CAPACITY];
static nova_control_style_t default_style;
static uint16_t focused_id = NOVA_CONTROL_NONE;
static nova_control_diagnostics_t diagnostics;

static void copy_text(char *destination, const char *source)
{
    uint32_t i = 0;
    if (source) for (; i + 1 < NOVA_CONTROL_TEXT_CAPACITY && source[i]; ++i)
        destination[i] = source[i];
    destination[i] = 0;
}

void nova_controls_initialize(const nova_control_style_t *style)
{
    for (uint16_t i = 0; i < NOVA_CONTROL_CAPACITY; ++i) used[i] = false;
    default_style = style ? *style : (nova_control_style_t){
        0xff22262au, 0xfff4f6f8u, 0xff4cc2ffu, 0xff66686au, 6, 1, 8
    };
    focused_id = NOVA_CONTROL_NONE;
    diagnostics = (nova_control_diagnostics_t){0};
}

nova_control_t *nova_control_create(nova_control_type_t type)
{
    if (type > NOVA_CONTROL_BREADCRUMB) return 0;
    for (uint16_t i = 0; i < NOVA_CONTROL_CAPACITY; ++i) if (!used[i]) {
        used[i] = true;
        controls[i] = (nova_control_t){
            i, NOVA_CONTROL_NONE, NOVA_CONTROL_NONE, NOVA_CONTROL_NONE, type,
            NOVA_CONTROL_CREATED, NOVA_CONTROL_FLAG_ENABLED | NOVA_CONTROL_FLAG_DIRTY,
            {0}, default_style, 0, 0, 1000, 1, 0, (uint16_t)type, 0, false,
            {0}, {0}, {0}, 0, 0, 0, 0, 0, NOVA_TEXT_INPUT_STANDARD
        };
        controls[i].maximum_length=NOVA_CONTROL_TEXT_CAPACITY-1;
        controls[i].input_mode=type==NOVA_CONTROL_PASSWORD_FIELD?
                               NOVA_TEXT_INPUT_PASSWORD:NOVA_TEXT_INPUT_STANDARD;
        if(type==NOVA_CONTROL_PASSWORD_FIELD)controls[i].flags|=NOVA_CONTROL_FLAG_PASSWORD;
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
    if (state == NOVA_CONTROL_DISABLED)
        control->flags &= ~NOVA_CONTROL_FLAG_ENABLED;
    control->flags |= NOVA_CONTROL_FLAG_DIRTY;
    return true;
}

bool nova_control_destroy(nova_control_t *control)
{
    if (!control || !used[control->id] || !nova_control_set_state(control, NOVA_CONTROL_DESTROYED))
        return false;
    if(control->type==NOVA_CONTROL_PASSWORD_FIELD)nova_text_field_clear(control);
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
    return true;
}

bool nova_control_set_bounds(nova_control_t *control, nova_rect_t bounds)
{
    if (!control || bounds.width <= 0 || bounds.height <= 0) return false;
    control->bounds = bounds; control->flags |= NOVA_CONTROL_FLAG_DIRTY; return true;
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
    return true;
}

bool nova_control_set_checked(nova_control_t *control, bool checked)
{
    if (!control || (control->type != NOVA_CONTROL_CHECKBOX &&
                     control->type != NOVA_CONTROL_SWITCH)) return false;
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

bool nova_control_set_style(nova_control_t *control,
                            const nova_control_style_t *style)
{
    if (!control || !style) return false;
    control->style = *style;
    return nova_control_invalidate(control);
}

bool nova_control_invalidate(nova_control_t *control)
{
    if (!control || !used[control->id] || control->state == NOVA_CONTROL_DESTROYED)
        return false;
    control->flags |= NOVA_CONTROL_FLAG_DIRTY;
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
    if (control->type == NOVA_CONTROL_CHECKBOX || control->type == NOVA_CONTROL_SWITCH)
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
    if (!radius || rect.width <= 0 || rect.height <= 0) {
        nova_surface_rect(surface, rect, color);
        return;
    }
    if ((int32_t)radius * 2 > rect.height) radius = (uint16_t)(rect.height / 2);
    if ((int32_t)radius * 2 > rect.width) radius = (uint16_t)(rect.width / 2);
    for (int32_t row = 0; row < rect.height; ++row) {
        int32_t edge = row < radius ? row : rect.height - 1 - row;
        int32_t inset = 0;
        if (edge < radius) {
            int32_t dy = (int32_t)radius - 1 - edge;
            int32_t extent = 0;
            while ((extent + 1) * (extent + 1) + dy * dy <= radius * radius)
                ++extent;
            inset = (int32_t)radius - extent;
        }
        nova_surface_rect(surface, (nova_rect_t){rect.x + inset, rect.y + row,
                          rect.width - inset * 2, 1}, color);
    }
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
        uint32_t border=(control->flags&NOVA_CONTROL_FLAG_ERROR)?0xffff4f64u:
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
                                  0x664cc2ffu);}
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
    case NOVA_CONTROL_SCROLLBAR:
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
    case NOVA_CONTROL_SEPARATOR:
        nova_surface_rect(surface,(nova_rect_t){b.x,b.y+b.height/2,b.width,thickness},control->style.disabled);
        break;
    case NOVA_CONTROL_LABEL:
    case NOVA_CONTROL_ICON:
    case NOVA_CONTROL_IMAGE:
        break;
    default:
        control_rounded_rect(surface,b,(uint16_t)radius,color);
        if (control->type > NOVA_CONTROL_BREADCRUMB) ++diagnostics.fallback_renders;
        break;
    }
    if (control->flags & (NOVA_CONTROL_FLAG_FOCUSED | NOVA_CONTROL_FLAG_SELECTED)) {
        nova_rect_t marker = control->bounds;
        marker.width = control->style.border_dlu ? control->style.border_dlu * 4 : 4;
        nova_surface_rect(surface, marker, control->style.accent);
    }
    control->flags &= ~NOVA_CONTROL_FLAG_DIRTY;
    ++diagnostics.renders;
}

const nova_control_diagnostics_t *nova_control_diagnostics(void) { return &diagnostics; }
