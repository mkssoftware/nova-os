#include "controls.h"

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
            {0}, default_style, 0, 0, 1000, 0, (uint16_t)type, 0, false, {0}, {0}
        };
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
        control->flags |= NOVA_CONTROL_FLAG_VISIBLE;
    if (state == NOVA_CONTROL_DISABLED)
        control->flags &= ~NOVA_CONTROL_FLAG_ENABLED;
    control->flags |= NOVA_CONTROL_FLAG_DIRTY;
    return true;
}

bool nova_control_destroy(nova_control_t *control)
{
    if (!control || !used[control->id] || !nova_control_set_state(control, NOVA_CONTROL_DESTROYED))
        return false;
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
    copy_text(control->text, text); control->flags |= NOVA_CONTROL_FLAG_DIRTY; return true;
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
        control->action_fired) { ++diagnostics.rejected_actions; return false; }
    control->action_fired = true; control->flags |= NOVA_CONTROL_FLAG_PRESSED;
    if (action) *action = control->action;
    return true;
}

void nova_control_release(nova_control_t *control)
{
    if (control) { control->action_fired = false; control->flags &= ~NOVA_CONTROL_FLAG_PRESSED; }
}

void nova_control_render(nova_control_t *control, nova_surface_t *surface)
{
    if (!control || !surface || !(control->flags & NOVA_CONTROL_FLAG_VISIBLE)) return;
    uint32_t color = (control->flags & NOVA_CONTROL_FLAG_ENABLED) ?
                     control->style.background : control->style.disabled;
    nova_surface_rect(surface, control->bounds, color);
    if (control->flags & (NOVA_CONTROL_FLAG_FOCUSED | NOVA_CONTROL_FLAG_SELECTED)) {
        nova_rect_t marker = control->bounds;
        marker.width = control->style.border_dlu ? control->style.border_dlu * 4 : 4;
        nova_surface_rect(surface, marker, control->style.accent);
    }
    control->flags &= ~NOVA_CONTROL_FLAG_DIRTY;
}

const nova_control_diagnostics_t *nova_control_diagnostics(void) { return &diagnostics; }
