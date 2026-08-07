#include "page.h"

static nova_page_t pages[NOVA_PAGE_CAPACITY];
static nova_view_t views[NOVA_VIEW_CAPACITY];
static bool page_used[NOVA_PAGE_CAPACITY], view_used[NOVA_VIEW_CAPACITY];
static nova_page_diagnostics_t diagnostics;

static void copy_text(char *target, uint32_t capacity, const char *source)
{
    uint32_t i=0; if(!source)source="";
    while(i+1<capacity&&source[i]){target[i]=source[i];++i;} target[i]=0;
}
static nova_page_t *find_page(uint16_t id)
{ for(uint16_t i=0;i<NOVA_PAGE_CAPACITY;++i)if(page_used[i]&&pages[i].id==id)return &pages[i];return 0; }
static nova_view_t *find_view(uint16_t id)
{ for(uint16_t i=0;i<NOVA_VIEW_CAPACITY;++i)if(view_used[i]&&views[i].id==id)return &views[i];return 0; }
static uint16_t view_index(const nova_view_t *view)
{ return view?(uint16_t)(view-views):NOVA_VIEW_NONE; }

void nova_page_model_initialize(void)
{
    for(uint16_t i=0;i<NOVA_PAGE_CAPACITY;++i)page_used[i]=false;
    for(uint16_t i=0;i<NOVA_VIEW_CAPACITY;++i)view_used[i]=false;
    diagnostics=(nova_page_diagnostics_t){.active_page=NOVA_PAGE_NONE};
}

nova_page_t *nova_page_create(uint16_t id,const char *title,uint32_t context,bool modal)
{
    if(id==NOVA_PAGE_NONE||find_page(id)){++diagnostics.errors;return 0;}
    for(uint16_t i=0;i<NOVA_PAGE_CAPACITY;++i)if(!page_used[i]){
        page_used[i]=true;pages[i]=(nova_page_t){id,NOVA_VIEW_NONE,NOVA_VIEW_NONE,
            NOVA_PAGE_CREATED,context,0,{0},modal};copy_text(pages[i].title,sizeof(pages[i].title),title);
        ++diagnostics.pages;if(diagnostics.pages>diagnostics.peak_pages)diagnostics.peak_pages=diagnostics.pages;
        return &pages[i];
    }
    ++diagnostics.errors;return 0;
}

nova_view_t *nova_view_create(nova_page_t *page,uint16_t id,nova_view_type_t type,
                              const char *name,uint16_t role,bool focusable)
{
    if(!page||page->state!=NOVA_PAGE_CREATED||id==NOVA_VIEW_NONE||find_view(id)||type>NOVA_VIEW_CUSTOM){
        ++diagnostics.errors;return 0;
    }
    for(uint16_t i=0;i<NOVA_VIEW_CAPACITY;++i)if(!view_used[i]){
        view_used[i]=true;views[i]=(nova_view_t){id,page->id,NOVA_VIEW_NONE,NOVA_VIEW_NONE,
            NOVA_VIEW_NONE,type,NOVA_VIEW_CREATED,NOVA_VISIBILITY_VISIBLE,{0},0,role,{0},focusable};
        copy_text(views[i].accessibility_name,sizeof(views[i].accessibility_name),name);
        ++diagnostics.views;if(diagnostics.views>diagnostics.peak_views)diagnostics.peak_views=diagnostics.views;
        return &views[i];
    }
    ++diagnostics.errors;return 0;
}

bool nova_page_set_root(nova_page_t *page,nova_view_t *root)
{
    if(!page||!root||page->root_view!=NOVA_VIEW_NONE||root->owner_page!=page->id||
       root->type!=NOVA_VIEW_ROOT||root->parent!=NOVA_VIEW_NONE){++diagnostics.errors;return false;}
    page->root_view=view_index(root);return true;
}

static bool descendant(const nova_view_t *node,const nova_view_t *target)
{
    uint16_t stack[NOVA_VIEW_CAPACITY],depth=0;
    if(node->first_child!=NOVA_VIEW_NONE)stack[depth++]=node->first_child;
    while(depth){uint16_t index=stack[--depth];if(index>=NOVA_VIEW_CAPACITY||!view_used[index])return true;
        nova_view_t *view=&views[index];if(view==target)return true;
        if(view->next_sibling!=NOVA_VIEW_NONE&&depth<NOVA_VIEW_CAPACITY)stack[depth++]=view->next_sibling;
        if(view->first_child!=NOVA_VIEW_NONE&&depth<NOVA_VIEW_CAPACITY)stack[depth++]=view->first_child;}
    return false;
}

bool nova_view_add_child(nova_view_t *parent,nova_view_t *child)
{
    if(!parent||!child||parent==child||parent->owner_page!=child->owner_page||
       child->parent!=NOVA_VIEW_NONE||descendant(child,parent)){++diagnostics.errors;return false;}
    uint16_t child_index=view_index(child);child->parent=view_index(parent);
    if(parent->first_child==NOVA_VIEW_NONE)parent->first_child=child_index;
    else{nova_view_t *tail=&views[parent->first_child];while(tail->next_sibling!=NOVA_VIEW_NONE)tail=&views[tail->next_sibling];tail->next_sibling=child_index;}
    return true;
}

bool nova_view_initialize(nova_view_t *view)
{ if(!view||view->state!=NOVA_VIEW_CREATED){++diagnostics.errors;return false;}view->state=NOVA_VIEW_INITIALIZED;return true; }
bool nova_view_set_visibility(nova_view_t *view,nova_visibility_t visibility)
{
    if(!view||visibility>NOVA_VISIBILITY_COLLAPSED||view->state==NOVA_VIEW_DESTROYED){++diagnostics.errors;return false;}
    view->visibility=visibility;if(visibility==NOVA_VISIBILITY_VISIBLE&&view->state==NOVA_VIEW_INITIALIZED)view->state=NOVA_VIEW_VISIBLE;return true;
}
bool nova_view_set_layout(nova_view_t *view,nova_rect_t bounds)
{
    if(!view||bounds.width<0||bounds.height<0||view->state==NOVA_VIEW_DESTROYED){++diagnostics.errors;return false;}
    view->layout_box=bounds;++diagnostics.layout_invalidations;return true;
}

static bool validate_tree(nova_page_t *page)
{
    if(!page||page->root_view==NOVA_VIEW_NONE||page->root_view>=NOVA_VIEW_CAPACITY||!view_used[page->root_view])return false;
    bool seen[NOVA_VIEW_CAPACITY]={0};uint16_t stack[NOVA_VIEW_CAPACITY],depth=0;stack[depth++]=page->root_view;
    while(depth){uint16_t index=stack[--depth];if(index>=NOVA_VIEW_CAPACITY||!view_used[index]||seen[index]||views[index].owner_page!=page->id)return false;
        seen[index]=true;nova_view_t *view=&views[index];if(view->state==NOVA_VIEW_CREATED||view->state==NOVA_VIEW_DESTROYED)return false;
        if(view->next_sibling!=NOVA_VIEW_NONE){if(depth>=NOVA_VIEW_CAPACITY)return false;stack[depth++]=view->next_sibling;}
        if(view->first_child!=NOVA_VIEW_NONE){if(depth>=NOVA_VIEW_CAPACITY)return false;stack[depth++]=view->first_child;}}
    return true;
}

bool nova_page_load(nova_page_t *page)
{ if(!page||page->state!=NOVA_PAGE_CREATED||!validate_tree(page)){++diagnostics.errors;return false;}page->state=NOVA_PAGE_LOADED;return true; }

static void set_tree_active(nova_page_t *page,bool active)
{
    uint16_t stack[NOVA_VIEW_CAPACITY],depth=0;stack[depth++]=page->root_view;
    while(depth){nova_view_t *view=&views[stack[--depth]];
        if(view->visibility==NOVA_VISIBILITY_VISIBLE)view->state=active?NOVA_VIEW_ACTIVE:NOVA_VIEW_INACTIVE;
        if(view->next_sibling!=NOVA_VIEW_NONE)stack[depth++]=view->next_sibling;
        if(view->first_child!=NOVA_VIEW_NONE)stack[depth++]=view->first_child;}
}

bool nova_page_activate(nova_page_t *page)
{
    if(!page||(page->state!=NOVA_PAGE_LOADED&&page->state!=NOVA_PAGE_SUSPENDED)){++diagnostics.errors;return false;}
    if(diagnostics.active_page!=NOVA_PAGE_NONE){nova_page_t *old=find_page(diagnostics.active_page);
        if(!old||old->state!=NOVA_PAGE_ACTIVE){++diagnostics.errors;return false;}
        set_tree_active(old,false);old->state=NOVA_PAGE_SUSPENDED;++diagnostics.suspensions;}
    page->state=NOVA_PAGE_ACTIVE;diagnostics.active_page=page->id;set_tree_active(page,true);
    ++diagnostics.activations;if(page->focus_view!=NOVA_VIEW_NONE)++diagnostics.focus_restores;return true;
}

bool nova_page_set_focus(nova_page_t *page,nova_view_t *view)
{
    if(!page||!view||page->state!=NOVA_PAGE_ACTIVE||view->owner_page!=page->id||
       !view->focusable||view->state!=NOVA_VIEW_ACTIVE){++diagnostics.errors;return false;}
    page->focus_view=view_index(view);return true;
}

bool nova_page_close(nova_page_t *page)
{
    if(!page||page->state==NOVA_PAGE_CLOSED){++diagnostics.errors;return false;}
    if(page->state==NOVA_PAGE_ACTIVE)diagnostics.active_page=NOVA_PAGE_NONE;
    for(uint16_t i=0;i<NOVA_VIEW_CAPACITY;++i)if(view_used[i]&&views[i].owner_page==page->id){views[i].state=NOVA_VIEW_DESTROYED;view_used[i]=false;--diagnostics.views;}
    page->state=NOVA_PAGE_CLOSED;++diagnostics.closes;
    uint16_t index=(uint16_t)(page-pages);if(index<NOVA_PAGE_CAPACITY&&page_used[index]){page_used[index]=false;--diagnostics.pages;}
    return true;
}

bool nova_page_dispatch(nova_page_t *page,uint32_t event,nova_view_event_handler_t handler,void *context)
{
    if(!page||page->state!=NOVA_PAGE_ACTIVE||!handler){++diagnostics.errors;return false;}
    uint16_t stack[NOVA_VIEW_CAPACITY],depth=0;stack[depth++]=page->root_view;++diagnostics.traversals;
    while(depth){nova_view_t *view=&views[stack[--depth]];if(view->state==NOVA_VIEW_ACTIVE&&handler(view,event,context))return true;
        if(view->next_sibling!=NOVA_VIEW_NONE)stack[depth++]=view->next_sibling;
        if(view->first_child!=NOVA_VIEW_NONE)stack[depth++]=view->first_child;}
    return false;
}

nova_page_t *nova_page_active(void){return diagnostics.active_page==NOVA_PAGE_NONE?0:find_page(diagnostics.active_page);}
nova_view_t *nova_page_root_view(const nova_page_t *page){return page&&page->root_view<NOVA_VIEW_CAPACITY&&view_used[page->root_view]?&views[page->root_view]:0;}
const nova_page_diagnostics_t *nova_page_diagnostics(void){return &diagnostics;}
