#ifndef NOVA_BOOT_LAYOUT_H
#define NOVA_BOOT_LAYOUT_H

#include <stdint.h>
#include <stdbool.h>
#include "compositor.h"
#include "text.h"

typedef enum { NOVA_LAYOUT_COMPACT, NOVA_LAYOUT_STANDARD, NOVA_LAYOUT_WIDE }
    nova_layout_class_t;
typedef enum {NOVA_ALIGN_LEFT,NOVA_ALIGN_CENTER,NOVA_ALIGN_RIGHT,
    NOVA_ALIGN_STRETCH} nova_horizontal_alignment_t;
typedef enum {NOVA_ALIGN_TOP,NOVA_ALIGN_CENTER_VERTICAL,NOVA_ALIGN_BOTTOM,
    NOVA_ALIGN_STRETCH_VERTICAL,NOVA_ALIGN_BASELINE} nova_vertical_alignment_t;
typedef enum {NOVA_DISTRIBUTE_NONE,NOVA_DISTRIBUTE_EQUAL_SPACE,
    NOVA_DISTRIBUTE_SPACE_AROUND,NOVA_DISTRIBUTE_SPACE_BETWEEN,
    NOVA_DISTRIBUTE_SPACE_EVENLY} nova_distribution_t;
typedef struct {int32_t left,top,right,bottom;} nova_layout_insets_t;
typedef struct {nova_horizontal_alignment_t horizontal;
    nova_vertical_alignment_t vertical;nova_layout_insets_t margin,padding;
    int32_t spacing_dlu,baseline_dlu;nova_distribution_t distribution;
    bool accessibility;} nova_alignment_spec_t;
typedef struct {uint32_t applications,spacing_applications,invalidations,
    fallbacks,errors;int32_t last_spacing_pixels;uint32_t scale_milli;
    nova_rect_t last_container,last_result;} nova_alignment_diagnostics_t;
typedef struct {
    uint32_t min_width, min_height, preferred_width, preferred_height,
             max_width, max_height;
} nova_size_constraints_t;
typedef enum {NOVA_SIZE_AUTO,NOVA_SIZE_FIXED,NOVA_SIZE_PERCENT,NOVA_SIZE_STRETCH,
    NOVA_SIZE_CONTENT,NOVA_SIZE_REMAINING} nova_size_mode_t;
typedef struct {nova_size_mode_t width_mode,height_mode;int32_t width_dlu,height_dlu;
    int32_t min_width_dlu,min_height_dlu,preferred_width_dlu,preferred_height_dlu,
    max_width_dlu,max_height_dlu,content_width_dlu,content_height_dlu;
    uint16_t width_percent_milli,height_percent_milli,stretch_width_weight,
    stretch_height_weight,aspect_width,aspect_height;nova_layout_insets_t margin,padding;
    bool accessibility,interactive;} nova_constraint_spec_t;
typedef struct {uint32_t validations,resolutions,invalidations,violations,fallbacks,
    minimum_wins,maximum_wins,aspect_adjustments,preferred_wins,stretch_uses,
    percent_uses,accessibility_adjustments,errors;uint32_t scale_milli;
    nova_rect_t last_available,last_result;} nova_constraint_diagnostics_t;
#define NOVA_STACK_CHILD_CAPACITY 16u
#define NOVA_STACK_DEPTH_LIMIT 16u
typedef enum {NOVA_STACK_VERTICAL,NOVA_STACK_HORIZONTAL} nova_stack_orientation_t;
typedef struct nova_stack nova_stack_t;
typedef struct {uint32_t id;nova_rect_t desired,bounds;nova_size_constraints_t constraints;
    nova_constraint_spec_t adaptive_constraints;bool use_adaptive_constraints;
    nova_text_layout_request_t text_layout;nova_text_layout_metrics_t text_metrics;
    int32_t icon_width_dlu,icon_gap_dlu;bool use_text_layout;
    nova_horizontal_alignment_t horizontal;nova_vertical_alignment_t vertical;
    nova_stack_t *nested;bool visible,stretch;} nova_stack_child_t;
struct nova_stack {nova_stack_orientation_t orientation;nova_rect_t bounds,measured;
    nova_layout_insets_t padding;int32_t spacing_dlu;
    nova_distribution_t distribution;uint32_t scale_milli;bool accessibility,dirty;
    nova_stack_t *parent;nova_stack_child_t children[NOVA_STACK_CHILD_CAPACITY];
    uint16_t child_count;};
typedef struct {uint32_t measures,arranges,updates,invalidations,errors,
    skipped,cycles,duplicates;uint16_t last_child_count,max_depth;
    nova_stack_orientation_t last_orientation;uint64_t measured_area;} nova_stack_diagnostics_t;
typedef struct {
    nova_rect_t safe, panel, top_bar, title, status, brand, list;
    int32_t item_height, item_gap, icon_size, item_padding, text_gap;
    uint32_t scale_milli, dpi;
    nova_layout_class_t layout_class;
    bool brand_compact, accessibility;
} nova_boot_layout_t;
typedef struct {
    uint32_t measures, arranges, finalizes, invalidations, constraint_errors,
             fallbacks;
    uint32_t width, height, scale_milli;
    nova_layout_class_t layout_class;
} nova_layout_diagnostics_t;
typedef enum {NOVA_RESPONSIVE_STACKED,NOVA_RESPONSIVE_SIDE_BY_SIDE}
    nova_responsive_flow_t;
typedef struct {uint32_t physical_width,physical_height,dpi,scale_milli,generation;
    int32_t logical_width_dlu,logical_height_dlu,spacing_dlu,max_control_width_dlu,
    dialog_max_width_dlu; nova_layout_class_t layout_class;
    nova_responsive_flow_t panel_flow;bool accessibility,ultrawide,valid;}
    nova_responsive_state_t;
typedef struct {uint32_t updates,applies,invalidations,breakpoint_changes,
    resolution_changes,dpi_changes,scale_changes,accessibility_changes,
    compact_count,standard_count,wide_count,stacked_count,side_by_side_count,
    dialog_fits,fallbacks,errors;uint32_t last_generation;}
    nova_responsive_diagnostics_t;

void nova_layout_initialize(void);
int32_t nova_dlu_to_pixels(int32_t dlu, uint32_t scale_milli);
bool nova_constraints_resolve(const nova_size_constraints_t *constraints,
                              uint32_t available_width, uint32_t available_height,
                              uint32_t *width, uint32_t *height);
bool nova_constraints_validate(const nova_constraint_spec_t *constraints);
bool nova_constraints_resolve_dlu(const nova_constraint_spec_t *constraints,
    nova_rect_t available,nova_rect_t *result,uint32_t scale_milli);
const nova_constraint_diagnostics_t *nova_constraint_diagnostics(void);
bool nova_layout_compute(uint32_t width, uint32_t height, bool accessibility,
                         nova_boot_layout_t *layout);
bool nova_responsive_update(bool accessibility);
nova_layout_class_t nova_responsive_class(void);
bool nova_responsive_fit_dialog(nova_rect_t desired,nova_rect_t *result);
const nova_responsive_state_t *nova_responsive_state(void);
const nova_responsive_diagnostics_t *nova_responsive_diagnostics(void);
bool nova_layout_apply_alignment(const nova_rect_t *container,
    const nova_rect_t *measured,const nova_alignment_spec_t *spec,
    uint32_t scale_milli,nova_rect_t *result);
bool nova_layout_apply_spacing(const nova_rect_t *content,uint16_t item_count,
    int32_t item_extent_dlu,int32_t spacing_dlu,nova_distribution_t distribution,
    bool horizontal,uint32_t scale_milli,int32_t *positions,uint16_t capacity);
bool nova_layout_apply_text(const nova_text_layout_request_t *request,
    nova_layout_insets_t padding,int32_t icon_width_dlu,int32_t icon_gap_dlu,
    nova_rect_t *desired,nova_text_layout_metrics_t *metrics);
bool nova_stack_initialize(nova_stack_t *stack,nova_stack_orientation_t orientation,
    nova_rect_t bounds,nova_layout_insets_t padding,int32_t spacing_dlu,
    nova_distribution_t distribution,uint32_t scale_milli,bool accessibility);
bool nova_stack_add(nova_stack_t *stack,const nova_stack_child_t *child);
bool nova_stack_remove(nova_stack_t *stack,uint16_t index);
bool nova_stack_measure(nova_stack_t *stack);
bool nova_stack_arrange(nova_stack_t *stack);
bool nova_stack_update(nova_stack_t *stack);
const nova_stack_diagnostics_t *nova_stack_diagnostics(void);

#define NOVA_GRID_TRACK_CAPACITY 8u
#define NOVA_GRID_CHILD_CAPACITY 32u
#define NOVA_GRID_DEPTH_LIMIT 12u
typedef enum {NOVA_GRID_AUTO,NOVA_GRID_FIXED,NOVA_GRID_STAR,NOVA_GRID_PERCENT}
    nova_grid_unit_t;
typedef struct nova_grid nova_grid_t;
typedef struct {nova_grid_unit_t unit;uint16_t value;int32_t pixels;} nova_grid_track_t;
typedef struct {uint32_t id;nova_rect_t desired,bounds;nova_size_constraints_t constraints;
    nova_constraint_spec_t adaptive_constraints;bool use_adaptive_constraints;
    nova_text_layout_request_t text_layout;nova_text_layout_metrics_t text_metrics;
    int32_t icon_width_dlu,icon_gap_dlu;bool use_text_layout;
    nova_layout_insets_t margin,padding;nova_horizontal_alignment_t horizontal;
    nova_vertical_alignment_t vertical;nova_grid_t *nested;uint8_t row,column,row_span,
    column_span;int16_t z_index;bool visible,reserve_when_hidden;} nova_grid_child_t;
struct nova_grid {nova_rect_t bounds,measured;nova_grid_track_t rows[NOVA_GRID_TRACK_CAPACITY],
    columns[NOVA_GRID_TRACK_CAPACITY];nova_grid_child_t children[NOVA_GRID_CHILD_CAPACITY];
    nova_grid_t *parent;uint32_t scale_milli;int32_t row_spacing_dlu,column_spacing_dlu;
    uint8_t row_count,column_count,child_count;bool accessibility,dirty;};
typedef struct {uint32_t measures,arranges,updates,invalidations,errors,overlaps,
    cycles,duplicates,hidden_excluded,spans;uint8_t last_rows,last_columns,
    last_children,max_depth;} nova_grid_diagnostics_t;
bool nova_grid_initialize(nova_grid_t *grid,nova_rect_t bounds,uint32_t scale_milli,
    int32_t row_spacing_dlu,int32_t column_spacing_dlu,bool accessibility);
bool nova_grid_add_row(nova_grid_t *grid,nova_grid_unit_t unit,uint16_t value);
bool nova_grid_add_column(nova_grid_t *grid,nova_grid_unit_t unit,uint16_t value);
bool nova_grid_add(nova_grid_t *grid,const nova_grid_child_t *child);
bool nova_grid_measure(nova_grid_t *grid);
bool nova_grid_arrange(nova_grid_t *grid);
bool nova_grid_update(nova_grid_t *grid);
const nova_grid_diagnostics_t *nova_grid_diagnostics(void);

#define NOVA_OVERLAY_CHILD_CAPACITY 16u
#define NOVA_OVERLAY_DEPTH_LIMIT 12u
typedef enum {NOVA_OVERLAY_BACKGROUND,NOVA_OVERLAY_CONTENT,NOVA_OVERLAY_OVERLAY,
    NOVA_OVERLAY_DIALOG,NOVA_OVERLAY_POPUP,NOVA_OVERLAY_TOOLTIP,NOVA_OVERLAY_DEBUG}
    nova_overlay_layer_t;
typedef enum {NOVA_OVERLAY_VISIBLE,NOVA_OVERLAY_HIDDEN,NOVA_OVERLAY_COLLAPSED}
    nova_overlay_visibility_t;
typedef struct nova_overlay nova_overlay_t;
typedef struct {uint32_t id;nova_rect_t desired,bounds;nova_layout_insets_t margin;
    nova_constraint_spec_t adaptive_constraints;bool use_adaptive_constraints;
    nova_text_layout_request_t text_layout;nova_text_layout_metrics_t text_metrics;
    nova_layout_insets_t text_padding;int32_t icon_width_dlu,icon_gap_dlu;
    bool use_text_layout;
    nova_horizontal_alignment_t horizontal;nova_vertical_alignment_t vertical;
    nova_overlay_layer_t layer;nova_material_kind_t material;
    nova_overlay_visibility_t visibility;nova_overlay_t *nested;int32_t offset_x_dlu,
    offset_y_dlu;int16_t z_index;uint8_t opacity;bool absolute,modal,interactive;}
    nova_overlay_child_t;
struct nova_overlay {nova_rect_t bounds,clip;nova_layout_insets_t padding;
    nova_overlay_child_t children[NOVA_OVERLAY_CHILD_CAPACITY];nova_overlay_t *parent;
    uint32_t scale_milli;uint8_t child_count,order[NOVA_OVERLAY_CHILD_CAPACITY];
    bool clipping,accessibility,dirty,modal_active;uint32_t modal_id;};
typedef struct {uint32_t measures,arranges,updates,invalidations,errors,duplicates,
    modal_conflicts,cycles,clips,visibility_changes,hit_tests,input_blocks;
    uint8_t last_children,last_layers,max_depth;int16_t minimum_z,maximum_z;}
    nova_overlay_diagnostics_t;
bool nova_overlay_initialize(nova_overlay_t *overlay,nova_rect_t bounds,
    nova_layout_insets_t padding,uint32_t scale_milli,bool clipping,bool accessibility);
bool nova_overlay_add(nova_overlay_t *overlay,const nova_overlay_child_t *child);
bool nova_overlay_set_visibility(nova_overlay_t *overlay,uint32_t child_id,
    nova_overlay_visibility_t visibility);
bool nova_overlay_measure(nova_overlay_t *overlay);
bool nova_overlay_arrange(nova_overlay_t *overlay);
bool nova_overlay_update(nova_overlay_t *overlay);
const nova_overlay_child_t *nova_overlay_hit_test(nova_overlay_t *overlay,
    int32_t x,int32_t y);
bool nova_overlay_input_allowed(const nova_overlay_t *overlay,uint32_t child_id);
const nova_overlay_diagnostics_t *nova_overlay_diagnostics(void);
const nova_alignment_diagnostics_t *nova_alignment_diagnostics(void);
const nova_layout_diagnostics_t *nova_layout_diagnostics(void);

#endif
