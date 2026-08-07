#ifndef NOVA_BOOT_PAGE_H
#define NOVA_BOOT_PAGE_H

#include <stdint.h>
#include <stdbool.h>
#include "compositor.h"

#define NOVA_PAGE_CAPACITY 16u
#define NOVA_VIEW_CAPACITY 64u
#define NOVA_PAGE_NONE 0xffffu
#define NOVA_VIEW_NONE 0xffffu

typedef enum {
    NOVA_PAGE_CREATED, NOVA_PAGE_LOADED, NOVA_PAGE_ACTIVE,
    NOVA_PAGE_SUSPENDED, NOVA_PAGE_CLOSED
} nova_page_state_t;
typedef enum {
    NOVA_VIEW_CREATED, NOVA_VIEW_INITIALIZED, NOVA_VIEW_VISIBLE,
    NOVA_VIEW_ACTIVE, NOVA_VIEW_INACTIVE, NOVA_VIEW_DESTROYED
} nova_view_state_t;
typedef enum { NOVA_VISIBILITY_VISIBLE, NOVA_VISIBILITY_HIDDEN,
               NOVA_VISIBILITY_COLLAPSED } nova_visibility_t;
typedef enum { NOVA_VIEW_ROOT, NOVA_VIEW_HEADER, NOVA_VIEW_NAVIGATION,
               NOVA_VIEW_CONTENT, NOVA_VIEW_FOOTER, NOVA_VIEW_DIALOG_CONTENT,
               NOVA_VIEW_CUSTOM } nova_view_type_t;

typedef struct {
    uint16_t id, owner_page, parent, first_child, next_sibling;
    nova_view_type_t type;
    nova_view_state_t state;
    nova_visibility_t visibility;
    nova_rect_t layout_box;
    uint16_t focus_order, accessibility_role;
    char accessibility_name[64];
    bool focusable;
} nova_view_t;

typedef struct {
    uint16_t id, root_view, focus_view;
    nova_page_state_t state;
    uint32_t navigation_context;
    int32_t scroll_position;
    char title[64];
    bool modal;
} nova_page_t;

typedef struct {
    uint16_t pages, views, active_page, peak_pages, peak_views;
    uint32_t activations, suspensions, closes, traversals,
             focus_restores, layout_invalidations, errors;
} nova_page_diagnostics_t;

typedef bool (*nova_view_event_handler_t)(nova_view_t *view, uint32_t event,
                                          void *context);

void nova_page_model_initialize(void);
nova_page_t *nova_page_create(uint16_t id, const char *title,
                              uint32_t navigation_context, bool modal);
nova_view_t *nova_view_create(nova_page_t *page, uint16_t id,
                              nova_view_type_t type, const char *accessibility_name,
                              uint16_t accessibility_role, bool focusable);
bool nova_page_set_root(nova_page_t *page, nova_view_t *root);
bool nova_view_add_child(nova_view_t *parent, nova_view_t *child);
bool nova_view_initialize(nova_view_t *view);
bool nova_view_set_visibility(nova_view_t *view, nova_visibility_t visibility);
bool nova_view_set_layout(nova_view_t *view, nova_rect_t bounds);
bool nova_page_load(nova_page_t *page);
bool nova_page_activate(nova_page_t *page);
bool nova_page_set_focus(nova_page_t *page, nova_view_t *view);
bool nova_page_close(nova_page_t *page);
bool nova_page_dispatch(nova_page_t *page, uint32_t event,
                        nova_view_event_handler_t handler, void *context);
nova_page_t *nova_page_active(void);
nova_view_t *nova_page_root_view(const nova_page_t *page);
const nova_page_diagnostics_t *nova_page_diagnostics(void);

#endif
