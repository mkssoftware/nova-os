#include "icons.h"
#include "resources.h"
#include "boot_icon_data.h"

typedef struct { const char *uri; const uint8_t *bitmap; } icon_entry_t;
static uint8_t generated_icons[9][100];
static const icon_entry_t entries[NOVA_ICON_COUNT] = {
    [NOVA_ICON_BACK] = {"boot://icons/back", generated_icons[0]},
    [NOVA_ICON_FORWARD] = {"boot://icons/forward", generated_icons[1]},
    [NOVA_ICON_HOME] = {"boot://icons/home", bm_icon_home},
    [NOVA_ICON_INSTALL] = {"boot://icons/install", bm_icon_install},
    [NOVA_ICON_SETTINGS] = {"boot://icons/settings", bm_icon_settings},
    [NOVA_ICON_DIAGNOSTICS] = {"boot://icons/diagnostics", bm_icon_diagnostics},
    [NOVA_ICON_RECOVERY] = {"boot://icons/recovery", bm_icon_recover},
    [NOVA_ICON_POWER] = {"boot://icons/power", bm_icon_power},
    [NOVA_ICON_RESTART] = {"boot://icons/restart", generated_icons[2]},
    [NOVA_ICON_WARNING] = {"boot://icons/warning", generated_icons[3]},
    [NOVA_ICON_ERROR] = {"boot://icons/error", generated_icons[4]},
    [NOVA_ICON_INFORMATION] = {"boot://icons/information", generated_icons[5]},
    [NOVA_ICON_SUCCESS] = {"boot://icons/success", generated_icons[6]},
    [NOVA_ICON_LOCK] = {"boot://icons/lock", generated_icons[7]},
    [NOVA_ICON_SEARCH] = {"boot://icons/search", generated_icons[8]}
};
static nova_icon_diagnostics_t diagnostics;

static void icon_pixel(uint8_t *bitmap,int32_t x,int32_t y,uint8_t coverage)
{
    if(!bitmap||x<0||y<0||x>=20||y>=20)return;
    uint32_t pixel=(uint32_t)y*20u+(uint32_t)x;
    uint8_t shift=(uint8_t)((pixel&3u)*2u);
    uint8_t old=(uint8_t)((bitmap[pixel/4u]>>shift)&3u);
    if(coverage>old)bitmap[pixel/4u]=(uint8_t)((bitmap[pixel/4u]&~(3u<<shift))|
                                               (coverage<<shift));
}

static void icon_line(uint8_t *bitmap,int32_t x0,int32_t y0,int32_t x1,int32_t y1)
{
    int32_t dx=x1>x0?x1-x0:x0-x1,sx=x0<x1?1:-1;
    int32_t dy=y1>y0?y0-y1:y1-y0,sy=y0<y1?1:-1,error=dx+dy;
    for(;;){
        icon_pixel(bitmap,x0,y0,3);icon_pixel(bitmap,x0-1,y0,1);
        icon_pixel(bitmap,x0+1,y0,1);icon_pixel(bitmap,x0,y0-1,1);
        icon_pixel(bitmap,x0,y0+1,1);
        if(x0==x1&&y0==y1)break;
        int32_t twice=error*2;
        if(twice>=dy){error+=dy;x0+=sx;}
        if(twice<=dx){error+=dx;y0+=sy;}
    }
}

static void icon_circle(uint8_t *bitmap)
{
    static const int8_t points[17][2]={{10,2},{14,3},{17,6},{18,10},{17,14},
        {14,17},{10,18},{6,17},{3,14},{2,10},{3,6},{6,3},{10,2},
        {14,3},{17,6},{18,10},{17,14}};
    for(uint8_t i=0;i<12;++i)icon_line(bitmap,points[i][0],points[i][1],
                                      points[i+1][0],points[i+1][1]);
}

static void generate_semantic_icons(void)
{
    for(uint32_t i=0;i<sizeof(generated_icons);++i)((uint8_t *)generated_icons)[i]=0;
    icon_line(generated_icons[0],13,4,7,10);icon_line(generated_icons[0],7,10,13,16);
    icon_line(generated_icons[0],7,10,17,10);
    icon_line(generated_icons[1],7,4,13,10);icon_line(generated_icons[1],13,10,7,16);
    icon_line(generated_icons[1],3,10,13,10);
    icon_circle(generated_icons[2]);icon_line(generated_icons[2],14,2,18,3);
    icon_line(generated_icons[2],18,3,17,7);
    icon_line(generated_icons[3],10,2,2,17);icon_line(generated_icons[3],2,17,18,17);
    icon_line(generated_icons[3],18,17,10,2);icon_line(generated_icons[3],10,7,10,12);
    icon_pixel(generated_icons[3],10,15,3);
    icon_circle(generated_icons[4]);icon_line(generated_icons[4],6,6,14,14);
    icon_line(generated_icons[4],14,6,6,14);
    icon_circle(generated_icons[5]);icon_line(generated_icons[5],10,8,10,14);
    icon_pixel(generated_icons[5],10,5,3);
    icon_circle(generated_icons[6]);icon_line(generated_icons[6],5,10,9,14);
    icon_line(generated_icons[6],9,14,16,6);
    icon_line(generated_icons[7],6,9,6,6);icon_line(generated_icons[7],6,6,8,3);
    icon_line(generated_icons[7],8,3,12,3);icon_line(generated_icons[7],12,3,14,6);
    icon_line(generated_icons[7],14,6,14,9);
    icon_line(generated_icons[7],4,9,16,9);icon_line(generated_icons[7],4,9,4,17);
    icon_line(generated_icons[7],4,17,16,17);icon_line(generated_icons[7],16,17,16,9);
    icon_circle(generated_icons[8]);icon_line(generated_icons[8],15,15,19,19);
}

bool nova_icons_initialize(void)
{
    diagnostics = (nova_icon_diagnostics_t){0};
    generate_semantic_icons();
    for (uint32_t i = 0; i < NOVA_ICON_COUNT; ++i) if (entries[i].bitmap) {
        if (!nova_resource_register(entries[i].uri, NOVA_RESOURCE_ICON, 1,
                                    entries[i].bitmap, 100, 0, 0)) return false;
        if(!nova_resource_cache_set_policy(nova_resource_id(entries[i].uri),
                                           NOVA_CACHE_PERMANENT))return false;
    }
    return true;
}

bool nova_icon_exists(nova_icon_token_t token)
{
    return token < NOVA_ICON_COUNT && entries[token].bitmap;
}

static uint32_t blend(uint32_t background, uint32_t foreground, uint8_t alpha)
{
    uint32_t inverse = 255u - alpha;
    uint32_t r = ((((foreground >> 16) & 255u) * alpha) +
                  (((background >> 16) & 255u) * inverse)) / 255u;
    uint32_t g = ((((foreground >> 8) & 255u) * alpha) +
                  (((background >> 8) & 255u) * inverse)) / 255u;
    uint32_t b = (((foreground & 255u) * alpha) + ((background & 255u) * inverse)) / 255u;
    return 0xff000000u | (r << 16) | (g << 8) | b;
}

void nova_icon_draw(nova_surface_t *surface, nova_icon_token_t token,
                    int32_t x, int32_t y, uint16_t size, uint32_t color)
{
    if (!surface || !size) return;
    ++diagnostics.lookups;
    const uint8_t *bitmap = token < NOVA_ICON_COUNT ? entries[token].bitmap : 0;
    if (!bitmap) { bitmap = bm_icon_home; ++diagnostics.fallbacks; }
    else ++diagnostics.cache_hits;
    for (uint16_t dy = 0; dy < size; ++dy) for (uint16_t dx = 0; dx < size; ++dx) {
        uint32_t sx = (uint32_t)dx * 20u / size;
        uint32_t sy = (uint32_t)dy * 20u / size;
        uint32_t pixel = sy * 20u + sx;
        uint8_t coverage = (bitmap[pixel / 4] >> ((pixel & 3u) * 2)) & 3u;
        if (!coverage) continue;
        int32_t px = x + dx, py = y + dy;
        if (px < 0 || py < 0 || (uint32_t)px >= surface->width || (uint32_t)py >= surface->height) continue;
        uint32_t *destination = &surface->pixels[py * surface->stride + px];
        *destination = blend(*destination, color, (uint8_t)(coverage * 85u));
    }
    nova_damage_add(surface, (nova_rect_t){x, y, size, size});
    ++diagnostics.renders;
}

const nova_icon_diagnostics_t *nova_icon_diagnostics(void) { return &diagnostics; }
