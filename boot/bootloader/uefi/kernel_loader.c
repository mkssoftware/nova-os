#include "kernel_loader.h"
#include "runtime_bridge.h"
#include "../bootmenu/graphics.h"
#include "firmware.h"
#include "boot_control.h"
#include "../../include/nova_boot_protocol.h"

#define EFI_ALLOCATE_ADDRESS 2u
#define EFI_ALLOCATE_MAX_ADDRESS 1u
#define EFI_LOADER_DATA 2u
#define EFI_FILE_MODE_READ 1ull
#define EFI_CONVENTIONAL_MEMORY 7u
#define BIB_ADDRESS 0x5000ull
#define MEMORY_MAP_ADDRESS 0x5800ull
#define KERNEL_ADDRESS 0x100000ull
#define KERNEL64_ADDRESS 0x200000ull
#define KERNEL_STACK_BASE 0x80000ull
#define PAGE_SIZE 4096ull
#define UEFI_PLATFORM 2u
#define MEMORY_ENTRY_SIZE 24u
#define MEMORY_ENTRY_CAPACITY 256u

typedef EFI_STATUS (EFIAPI *efi_handle_protocol_fn)(EFI_HANDLE,EFI_GUID *,VOID **);
typedef EFI_STATUS (EFIAPI *efi_allocate_pages_fn)(uint32_t,uint32_t,UINTN,EFI_PHYSICAL_ADDRESS *);
typedef EFI_STATUS (EFIAPI *efi_free_pages_fn)(EFI_PHYSICAL_ADDRESS,UINTN);
typedef EFI_STATUS (EFIAPI *efi_allocate_pool_fn)(uint32_t,UINTN,VOID **);
typedef EFI_STATUS (EFIAPI *efi_get_memory_map_fn)(UINTN *,VOID *,UINTN *,UINTN *,uint32_t *);
typedef EFI_STATUS (EFIAPI *efi_exit_boot_services_fn)(EFI_HANDLE,UINTN);

typedef struct {
    uint32_t Revision; EFI_HANDLE ParentHandle; EFI_SYSTEM_TABLE *SystemTable;
    EFI_HANDLE DeviceHandle; VOID *FilePath; VOID *Reserved;
    uint32_t LoadOptionsSize; VOID *LoadOptions; VOID *ImageBase;
    uint64_t ImageSize; uint32_t ImageCodeType,ImageDataType;
    EFI_STATUS (EFIAPI *Unload)(EFI_HANDLE);
} efi_loaded_image_protocol;

typedef struct efi_file_protocol efi_file_protocol;
struct efi_file_protocol {
    uint64_t Revision;
    EFI_STATUS (EFIAPI *Open)(efi_file_protocol *,efi_file_protocol **,CHAR16 *,uint64_t,uint64_t);
    EFI_STATUS (EFIAPI *Close)(efi_file_protocol *);
    VOID *Delete;
    EFI_STATUS (EFIAPI *Read)(efi_file_protocol *,UINTN *,VOID *);
    VOID *Write;
    EFI_STATUS (EFIAPI *GetPosition)(efi_file_protocol *,uint64_t *);
    EFI_STATUS (EFIAPI *SetPosition)(efi_file_protocol *,uint64_t);
    VOID *GetInfo,*SetInfo,*Flush;
};
typedef struct {uint64_t Revision;EFI_STATUS (EFIAPI *OpenVolume)(VOID *,efi_file_protocol **);} efi_simple_fs;
typedef struct {uint32_t Type;uint32_t Pad;uint64_t PhysicalStart,VirtualStart,NumberOfPages,Attribute;} efi_memory_descriptor;
typedef struct {uint64_t Address,Length;uint32_t Type,Attributes;} nova_e820_entry;
typedef struct {uint8_t ident[16];uint16_t type,machine;uint32_t version,entry,phoff,shoff,flags;uint16_t ehsize,phentsize,phnum,shentsize,shnum,shstrndx;} elf32_header;
typedef struct {uint32_t type,offset,vaddr,paddr,filesz,memsz,flags,align;} elf32_program_header;
typedef struct {uint8_t ident[16];uint16_t type,machine;uint32_t version;uint64_t entry,phoff,shoff;uint32_t flags;uint16_t ehsize,phentsize,phnum,shentsize,shnum,shstrndx;} elf64_header;
typedef struct {uint32_t type,flags;uint64_t offset,vaddr,paddr,filesz,memsz,align;} elf64_program_header;

extern void EFIAPI uefi_enter_kernel32(uint32_t entry,uint32_t bib,uint32_t stack_top);
extern void EFIAPI uefi_enter_kernel64(uint64_t entry,uint32_t bib,uint32_t stack_top);

static EFI_GUID loaded_image_guid={0x5b1b31a1,0x9562,0x11d2,{0x8e,0x3f,0x00,0xa0,0xc9,0x69,0x72,0x3b}};
static EFI_GUID simple_fs_guid={0x964e5b22,0x6459,0x11d2,{0x8e,0x39,0x00,0xa0,0xc9,0x69,0x72,0x3b}};
static CHAR16 nki_path[]={'\\','N','O','V','A','.','N','K','I',0};
static CHAR16 elf_path[]={'\\','K','E','R','N','E','L','.','E','L','F',0};
static CHAR16 elf64_path[]={'\\','K','E','R','N','E','L','6','4','.','E','L','F',0};
static CHAR16 backup_nki_path[]={'\\','B','A','C','K','U','P','.','N','K','I',0};
static CHAR16 recovery_nki_path[]={'\\','R','E','C','O','V','E','R','Y','.','N','K','I',0};
static CHAR16 bootbackground_path[]={'\\','B','A','C','K','G','R','N','D','.','N','B','S',0};
static CHAR16 bootlogo_path[]={'\\','L','O','G','O','.','N','B','S',0};

typedef struct {
    uint8_t magic[4];
    uint32_t header_size,width,height,stride,format,payload_size,payload_crc32;
} nova_bootsplash_header;

static void bytes_zero(void *target,UINTN length){uint8_t *p=target;while(length--)*p++=0;}
static void bytes_copy(void *target,const void *source,UINTN length){uint8_t *d=target;const uint8_t *s=source;while(length--)*d++=*s++;}
static bool bytes_equal(const void *left,const void *right,UINTN length)
{const uint8_t *a=left,*b=right;while(length--)if(*a++!=*b++)return false;return true;}
static uint32_t crc32_with_zero(const uint8_t *data,UINTN size,UINTN zero_offset,UINTN zero_size)
{uint32_t crc=0xffffffffu;for(UINTN i=0;i<size;++i){uint8_t value=(i>=zero_offset&&i<zero_offset+zero_size)?0:data[i];crc^=value;for(uint32_t bit=0;bit<8;++bit)crc=(crc>>1)^((crc&1)?0xedb88320u:0);}return ~crc;}
static bool range_valid(UINTN offset,UINTN length,UINTN total){return offset<=total&&length<=total-offset;}
static EFI_STATUS read_kernel_file(EFI_HANDLE image,EFI_SYSTEM_TABLE *st,
                                   CHAR16 *path,uint8_t **data,UINTN *size);

static void bootsplash_write_pixel(const nova_graphics_context_t *graphics,
                                   uint32_t x,uint32_t y,uint32_t rgba)
{
    uint32_t bytes=(graphics->bits_per_pixel+7u)/8u;
    uint8_t *row=(uint8_t *)graphics->framebuffer+(uint64_t)y*graphics->pitch;
    uint32_t native=nova_graphics_convert_pixel(rgba,graphics->pixel_format,
        graphics->red_mask,graphics->green_mask,graphics->blue_mask,graphics->alpha_mask);
    if(bytes==2)((uint16_t *)row)[x]=(uint16_t)native;
    else if(bytes==3){uint8_t *pixel=row+x*3u;pixel[0]=(uint8_t)native;
        pixel[1]=(uint8_t)(native>>8);pixel[2]=(uint8_t)(native>>16);}
    else ((uint32_t *)row)[x]=native;
}

static uint32_t bootsplash_read_pixel(const nova_bootsplash_header *header,
                                      const uint8_t *pixels,uint32_t x,uint32_t y)
{
    UINTN offset=(UINTN)y*header->stride;
    if(header->format==2u){
        offset+=(UINTN)x*3u;
        return 0xff000000u|(uint32_t)pixels[offset]<<16|
            (uint32_t)pixels[offset+1]<<8|pixels[offset+2];
    }
    offset+=(UINTN)x*2u;
    uint16_t rgb565=(uint16_t)(pixels[offset]|(uint16_t)pixels[offset+1]<<8);
    uint8_t red=(uint8_t)((((rgb565>>11)&31u)*255u+15u)/31u);
    uint8_t green=(uint8_t)((((rgb565>>5)&63u)*255u+31u)/63u);
    uint8_t blue=(uint8_t)(((rgb565&31u)*255u+15u)/31u);
    return 0xff000000u|(uint32_t)red<<16|(uint32_t)green<<8|blue;
}

static uint8_t bootsplash_lerp_channel(uint32_t first,uint32_t second,
                                       uint32_t shift,uint32_t fraction)
{
    uint32_t a=(first>>shift)&0xffu,b=(second>>shift)&0xffu;
    return (uint8_t)((a*(65536u-fraction)+b*fraction+32768u)>>16);
}

static uint32_t bootsplash_sample_cover(const nova_bootsplash_header *header,
    const uint8_t *pixels,uint32_t x,uint32_t y,uint32_t target_width,
    uint32_t target_height)
{
    if(!target_width||!target_height)return 0xff000000u;
    uint32_t crop_x=0,crop_y=0,crop_width=header->width,crop_height=header->height;
    if((uint64_t)header->width*target_height>(uint64_t)header->height*target_width){
        crop_width=(uint32_t)((uint64_t)header->height*target_width/target_height);
        if(crop_width<1u)crop_width=1u;
        crop_x=(header->width-crop_width)/2u;
    }else if((uint64_t)header->width*target_height<(uint64_t)header->height*target_width){
        crop_height=(uint32_t)((uint64_t)header->width*target_height/target_width);
        if(crop_height<1u)crop_height=1u;
        crop_y=(header->height-crop_height)/2u;
    }
    uint64_t x_fixed=(uint64_t)crop_x*65536u+
        (target_width>1u?(uint64_t)x*(crop_width-1u)*65536u/(target_width-1u):0);
    uint64_t y_fixed=(uint64_t)crop_y*65536u+
        (target_height>1u?(uint64_t)y*(crop_height-1u)*65536u/(target_height-1u):0);
    uint32_t x0=(uint32_t)(x_fixed>>16),y0=(uint32_t)(y_fixed>>16);
    uint32_t x1=x0+1u<header->width?x0+1u:x0;
    uint32_t y1=y0+1u<header->height?y0+1u:y0;
    uint32_t fx=(uint32_t)x_fixed&0xffffu,fy=(uint32_t)y_fixed&0xffffu;
    uint32_t p00=bootsplash_read_pixel(header,pixels,x0,y0);
    uint32_t p10=bootsplash_read_pixel(header,pixels,x1,y0);
    uint32_t p01=bootsplash_read_pixel(header,pixels,x0,y1);
    uint32_t p11=bootsplash_read_pixel(header,pixels,x1,y1);
    uint32_t top_r=bootsplash_lerp_channel(p00,p10,16,fx);
    uint32_t top_g=bootsplash_lerp_channel(p00,p10,8,fx);
    uint32_t top_b=bootsplash_lerp_channel(p00,p10,0,fx);
    uint32_t bottom_r=bootsplash_lerp_channel(p01,p11,16,fx);
    uint32_t bottom_g=bootsplash_lerp_channel(p01,p11,8,fx);
    uint32_t bottom_b=bootsplash_lerp_channel(p01,p11,0,fx);
    uint32_t red=(top_r*(65536u-fy)+bottom_r*fy+32768u)>>16;
    uint32_t green=(top_g*(65536u-fy)+bottom_g*fy+32768u)>>16;
    uint32_t blue=(top_b*(65536u-fy)+bottom_b*fy+32768u)>>16;
    return 0xff000000u|red<<16|green<<8|blue;
}

static bool show_boot_image(EFI_HANDLE image,EFI_SYSTEM_TABLE *st,CHAR16 *path)
{
    uint8_t *file=0;UINTN size=0;
    EFI_STATUS status=read_kernel_file(image,st,path,&file,&size);
    if(EFI_ERROR(status)||!file)return false;
    bool valid=false;
    if(size>=sizeof(nova_bootsplash_header)){
        const nova_bootsplash_header *header=(const nova_bootsplash_header *)file;
        const uint8_t expected[4]={'N','B','S','1'};
        uint32_t source_bytes=header->format==1u?2u:header->format==2u?3u:0u;
        uint64_t expected_payload=(uint64_t)header->width*header->height*source_bytes;
        valid=bytes_equal(header->magic,expected,4)&&header->header_size==32u&&
            header->width>=64u&&header->width<=1920u&&
            header->height>=64u&&header->height<=1080u&&
            source_bytes&&header->stride==header->width*source_bytes&&
            expected_payload==header->payload_size&&
            range_valid(header->header_size,header->payload_size,size)&&
            header->header_size+header->payload_size==size&&
            crc32_with_zero(file+header->header_size,header->payload_size,
                            header->payload_size,0)==header->payload_crc32;
        const nova_graphics_context_t *graphics=nova_graphics_context();
        valid=valid&&graphics&&graphics->initialized&&graphics->framebuffer;
        if(valid){
            for(uint32_t y=0;y<graphics->height;++y)
                for(uint32_t x=0;x<graphics->width;++x)
                    bootsplash_write_pixel(graphics,x,y,0xff020713u);
            const uint8_t *pixels=file+header->header_size;
            for(uint32_t y=0;y<graphics->height;++y){
                for(uint32_t x=0;x<graphics->width;++x){
                    uint32_t rgba=bootsplash_sample_cover(header,pixels,x,y,
                                                          graphics->width,graphics->height);
                    bootsplash_write_pixel(graphics,x,y,rgba);
                }
            }
        }
    }
    st->BootServices->FreePool(file);
    return valid;
}

typedef enum {NOVA_BOOT_VIEW_SPLASH,NOVA_BOOT_VIEW_CONSOLE} nova_boot_view_t;
typedef enum {NOVA_BOOT_LOG_INFO,NOVA_BOOT_LOG_WARN,NOVA_BOOT_LOG_ERROR} nova_boot_log_level_t;
typedef struct {nova_boot_log_level_t level;const char *message;} nova_boot_log_entry_t;
static nova_boot_view_t boot_view=NOVA_BOOT_VIEW_SPLASH;
static uint16_t boot_progress_per_mille=0;
static nova_boot_log_entry_t boot_log[18];
static uint8_t boot_log_count=0;

static uint32_t boot_sx(const nova_graphics_context_t *g,uint32_t x){return (uint32_t)((uint64_t)x*g->width/1680u);}
static uint32_t boot_sy(const nova_graphics_context_t *g,uint32_t y){return (uint32_t)((uint64_t)y*g->height/945u);}
static uint32_t boot_ss(const nova_graphics_context_t *g,uint32_t v)
{uint32_t sx=boot_sx(g,v),sy=boot_sy(g,v);return sx<sy?sx:sy;}

static uint32_t boot_read_framebuffer_pixel(const nova_graphics_context_t *graphics,uint32_t x,uint32_t y)
{
    uint32_t bytes=(graphics->bits_per_pixel+7u)/8u;
    uint8_t *row=(uint8_t *)graphics->framebuffer+(uint64_t)y*graphics->pitch;
    if(bytes==2){
        uint16_t rgb565=((uint16_t *)row)[x];
        uint8_t red=(uint8_t)((((rgb565>>11)&31u)*255u+15u)/31u);
        uint8_t green=(uint8_t)((((rgb565>>5)&63u)*255u+31u)/63u);
        uint8_t blue=(uint8_t)(((rgb565&31u)*255u+15u)/31u);
        return 0xff000000u|(uint32_t)red<<16|(uint32_t)green<<8|blue;
    }
    if(bytes==3){uint8_t *p=row+x*3u;return 0xff000000u|(uint32_t)p[2]<<16|(uint32_t)p[1]<<8|p[0];}
    uint32_t native=((uint32_t *)row)[x];
    if(graphics->pixel_format==NOVA_PIXEL_BGRA8888)return 0xff000000u|((native&0xffu)<<16)|(native&0xff00u)|((native>>16)&0xffu);
    return 0xff000000u|(native&0x00ffffffu);
}

static void boot_blend_pixel(const nova_graphics_context_t *graphics,uint32_t x,uint32_t y,uint32_t rgba)
{
    if(!graphics||x>=graphics->width||y>=graphics->height)return;
    uint32_t alpha=(rgba>>24)&0xffu;
    if(alpha==255u){bootsplash_write_pixel(graphics,x,y,rgba);return;}
    uint32_t dst=boot_read_framebuffer_pixel(graphics,x,y);
    uint32_t sr=(rgba>>16)&0xffu,sg=(rgba>>8)&0xffu,sb=rgba&0xffu;
    uint32_t dr=(dst>>16)&0xffu,dg=(dst>>8)&0xffu,db=dst&0xffu;
    uint32_t r=(sr*alpha+dr*(255u-alpha)+127u)/255u;
    uint32_t g=(sg*alpha+dg*(255u-alpha)+127u)/255u;
    uint32_t b=(sb*alpha+db*(255u-alpha)+127u)/255u;
    bootsplash_write_pixel(graphics,x,y,0xff000000u|r<<16|g<<8|b);
}

static void boot_add_pixel(const nova_graphics_context_t *graphics,uint32_t x,uint32_t y,
                           uint32_t red,uint32_t green,uint32_t blue)
{
    if(!graphics||x>=graphics->width||y>=graphics->height)return;
    uint32_t dst=boot_read_framebuffer_pixel(graphics,x,y);
    uint32_t dr=(dst>>16)&0xffu,dg=(dst>>8)&0xffu,db=dst&0xffu;
    uint32_t r=dr+red;r=r>255u?255u:r;
    uint32_t g=dg+green;g=g>255u?255u:g;
    uint32_t b=db+blue;b=b>255u?255u:b;
    bootsplash_write_pixel(graphics,x,y,0xff000000u|r<<16|g<<8|b);
}

static void boot_fill_rect(const nova_graphics_context_t *g,uint32_t x,uint32_t y,uint32_t w,uint32_t h,uint32_t rgba)
{for(uint32_t yy=0;yy<h;++yy)for(uint32_t xx=0;xx<w;++xx)boot_blend_pixel(g,x+xx,y+yy,rgba);}

static void boot_stroke_rect(const nova_graphics_context_t *g,uint32_t x,uint32_t y,uint32_t w,uint32_t h,uint32_t t,uint32_t rgba)
{boot_fill_rect(g,x,y,w,t,rgba);boot_fill_rect(g,x,y+h-t,w,t,rgba);boot_fill_rect(g,x,y,t,h,rgba);boot_fill_rect(g,x+w-t,y,t,h,rgba);}

static void boot_line(const nova_graphics_context_t *g,int32_t x0,int32_t y0,int32_t x1,int32_t y1,uint32_t rgba,uint32_t thick)
{
    int32_t dx=x1>x0?x1-x0:x0-x1,sx=x0<x1?1:-1,dy=y1>y0?y0-y1:y1-y0,sy=y0<y1?1:-1,err=dx+dy;
    for(;;){
        for(uint32_t ty=0;ty<thick;++ty)
            for(uint32_t tx=0;tx<thick;++tx)
                boot_blend_pixel(g,(uint32_t)(x0+(int32_t)tx),
                                 (uint32_t)(y0+(int32_t)ty),rgba);
        if(x0==x1&&y0==y1)break;
        int32_t e2=2*err;
        if(e2>=dy){err+=dy;x0+=sx;}
        if(e2<=dx){err+=dx;y0+=sy;}
    }
}

static const uint8_t *boot_glyph(char c)
{
    static const uint8_t sp[7]={0,0,0,0,0,0,0},q[7]={14,17,1,2,4,0,4};
    static const uint8_t a[7]={14,17,17,31,17,17,17},b[7]={30,17,17,30,17,17,30},c_[7]={14,17,16,16,16,17,14},d[7]={30,17,17,17,17,17,30};
    static const uint8_t e[7]={31,16,16,30,16,16,31},f[7]={31,16,16,30,16,16,16},g[7]={14,17,16,23,17,17,15},h[7]={17,17,17,31,17,17,17};
    static const uint8_t i[7]={14,4,4,4,4,4,14},j[7]={1,1,1,1,17,17,14},k[7]={17,18,20,24,20,18,17},l[7]={16,16,16,16,16,16,31};
    static const uint8_t m[7]={17,27,21,21,17,17,17},n[7]={17,25,21,19,17,17,17},o[7]={14,17,17,17,17,17,14},p[7]={30,17,17,30,16,16,16};
    static const uint8_t qg[7]={14,17,17,17,21,18,13},r[7]={30,17,17,30,20,18,17},s[7]={15,16,16,14,1,1,30},t[7]={31,4,4,4,4,4,4};
    static const uint8_t u[7]={17,17,17,17,17,17,14},v[7]={17,17,17,17,17,10,4},w[7]={17,17,17,21,21,21,10},x[7]={17,17,10,4,10,17,17};
    static const uint8_t y[7]={17,17,10,4,4,4,4},z[7]={31,1,2,4,8,16,31};
    static const uint8_t n0[7]={14,17,19,21,25,17,14},n1[7]={4,12,4,4,4,4,14},n2[7]={14,17,1,2,4,8,31},n3[7]={30,1,1,14,1,1,30};
    static const uint8_t n4[7]={2,6,10,18,31,2,2},n5[7]={31,16,30,1,1,17,14},n6[7]={6,8,16,30,17,17,14},n7[7]={31,1,2,4,8,8,8};
    static const uint8_t n8[7]={14,17,17,14,17,17,14},n9[7]={14,17,17,15,1,2,12},colon[7]={0,4,4,0,4,4,0},dash[7]={0,0,0,31,0,0,0};
    static const uint8_t dot[7]={0,0,0,0,0,12,12},slash[7]={1,2,2,4,8,8,16},bar[7]={4,4,4,4,4,4,4};
    if(c>='a'&&c<='z')c=(char)(c-'a'+'A');
    if(c>='A'&&c<='Z'){const uint8_t *letters[]={a,b,c_,d,e,f,g,h,i,j,k,l,m,n,o,p,qg,r,s,t,u,v,w,x,y,z};return letters[c-'A'];}
    if(c>='0'&&c<='9'){const uint8_t *digits[]={n0,n1,n2,n3,n4,n5,n6,n7,n8,n9};return digits[c-'0'];}
    switch(c){case ' ':return sp;case ':':return colon;case '-':return dash;case '.':return dot;case '/':return slash;case '|':return bar;default:return q;}
}

static void boot_draw_text(const nova_graphics_context_t *g,uint32_t x,uint32_t y,const char *text,uint32_t rgba,uint32_t scale)
{if(!scale)scale=1;for(const char *p=text;*p;++p){const uint8_t *rows=boot_glyph(*p);for(uint32_t row=0;row<7;++row)for(uint32_t col=0;col<5;++col)if(rows[row]&(1u<<(4u-col)))boot_fill_rect(g,x+col*scale,y+row*scale,scale,scale,rgba);x+=6u*scale;}}

static void boot_log_add(nova_boot_log_level_t level,const char *message)
{if(boot_log_count<sizeof(boot_log)/sizeof(boot_log[0])){boot_log[boot_log_count].level=level;boot_log[boot_log_count].message=message;++boot_log_count;}else{for(uint8_t i=1;i<boot_log_count;++i)boot_log[i-1]=boot_log[i];boot_log[boot_log_count-1].level=level;boot_log[boot_log_count-1].message=message;}}
static void boot_set_progress(uint16_t per_mille){if(per_mille>boot_progress_per_mille)boot_progress_per_mille=per_mille>1000u?1000u:per_mille;}
static bool boot_draw_background(EFI_HANDLE image,EFI_SYSTEM_TABLE *st){return show_boot_image(image,st,bootbackground_path);}

static bool boot_draw_logo_asset(EFI_HANDLE image,EFI_SYSTEM_TABLE *st,uint32_t left,uint32_t top,
                                 uint32_t max_width,uint32_t max_height)
{
    uint8_t *file=0;UINTN size=0;
    EFI_STATUS status=read_kernel_file(image,st,bootlogo_path,&file,&size);
    if(EFI_ERROR(status)||!file)return false;
    bool valid=false;
    if(size>=sizeof(nova_bootsplash_header)){
        const nova_bootsplash_header *header=(const nova_bootsplash_header *)file;
        const uint8_t expected[4]={'N','B','S','1'};
        uint32_t source_bytes=header->format==1u?2u:header->format==2u?3u:0u;
        uint64_t expected_payload=(uint64_t)header->width*header->height*source_bytes;
        valid=bytes_equal(header->magic,expected,4)&&header->header_size==32u&&
            header->width>=64u&&header->width<=1920u&&header->height>=64u&&header->height<=1080u&&
            source_bytes&&header->stride==header->width*source_bytes&&
            expected_payload==header->payload_size&&range_valid(header->header_size,header->payload_size,size)&&
            header->header_size+header->payload_size==size&&
            crc32_with_zero(file+header->header_size,header->payload_size,header->payload_size,0)==header->payload_crc32;
        const nova_graphics_context_t *graphics=nova_graphics_context();
        valid=valid&&graphics&&graphics->initialized&&graphics->framebuffer&&max_width&&max_height;
        if(valid){
            uint32_t width=max_width;
            uint32_t height=(uint32_t)((uint64_t)width*header->height/header->width);
            if(!height)height=1u;
            if(height>max_height){
                height=max_height;
                width=(uint32_t)((uint64_t)height*header->width/header->height);
                if(!width)width=1u;
            }
            uint32_t draw_left=left+(max_width-width)/2u;
            uint32_t draw_top=top+(max_height-height)/2u;
            const uint8_t *pixels=file+header->header_size;
            for(uint32_t y=0;y<height;++y){
                uint64_t y_fixed=height>1u?(uint64_t)y*(header->height-1u)*65536u/(height-1u):0;
                uint32_t y0=(uint32_t)(y_fixed>>16),y1=y0+1u<header->height?y0+1u:y0;
                uint32_t fy=(uint32_t)y_fixed&0xffffu;
                for(uint32_t x=0;x<width;++x){
                    uint64_t x_fixed=width>1u?(uint64_t)x*(header->width-1u)*65536u/(width-1u):0;
                    uint32_t x0=(uint32_t)(x_fixed>>16),x1=x0+1u<header->width?x0+1u:x0;
                    uint32_t fx=(uint32_t)x_fixed&0xffffu;
                    uint32_t p00=bootsplash_read_pixel(header,pixels,x0,y0);
                    uint32_t p10=bootsplash_read_pixel(header,pixels,x1,y0);
                    uint32_t p01=bootsplash_read_pixel(header,pixels,x0,y1);
                    uint32_t p11=bootsplash_read_pixel(header,pixels,x1,y1);
                    uint32_t top_r=bootsplash_lerp_channel(p00,p10,16,fx);
                    uint32_t top_g=bootsplash_lerp_channel(p00,p10,8,fx);
                    uint32_t top_b=bootsplash_lerp_channel(p00,p10,0,fx);
                    uint32_t bottom_r=bootsplash_lerp_channel(p01,p11,16,fx);
                    uint32_t bottom_g=bootsplash_lerp_channel(p01,p11,8,fx);
                    uint32_t bottom_b=bootsplash_lerp_channel(p01,p11,0,fx);
                    uint32_t red=(top_r*(65536u-fy)+bottom_r*fy+32768u)>>16;
                    uint32_t green=(top_g*(65536u-fy)+bottom_g*fy+32768u)>>16;
                    uint32_t blue=(top_b*(65536u-fy)+bottom_b*fy+32768u)>>16;
                    uint32_t max=red>green?red:green;if(blue>max)max=blue;
                    if(max<6u)continue;
                    boot_add_pixel(graphics,draw_left+x,draw_top+y,red,green,blue);
                }
            }
        }
    }
    st->BootServices->FreePool(file);
    return valid;
}

static void boot_draw_nova_mark(const nova_graphics_context_t *g,uint32_t cx,uint32_t cy,uint32_t size)
{
    uint32_t blue=0xff35a9ffu,cyan=0xff65d7ffu,white=0xffeaf7ffu,th=boot_ss(g,3);
    boot_line(g,(int32_t)cx,(int32_t)(cy-size/2),(int32_t)(cx+size/7),(int32_t)(cy-size/8),cyan,th);
    boot_line(g,(int32_t)(cx+size/7),(int32_t)(cy-size/8),(int32_t)(cx+size/2),(int32_t)cy,blue,th);
    boot_line(g,(int32_t)cx,(int32_t)(cy-size/2),(int32_t)(cx-size/7),(int32_t)(cy-size/8),cyan,th);
    boot_line(g,(int32_t)(cx-size/7),(int32_t)(cy-size/8),(int32_t)(cx-size/2),(int32_t)cy,blue,th);
    boot_line(g,(int32_t)(cx-size/2),(int32_t)cy,(int32_t)(cx-size/8),(int32_t)(cy+size/8),blue,th);
    boot_line(g,(int32_t)(cx-size/8),(int32_t)(cy+size/8),(int32_t)cx,(int32_t)(cy+size/2),white,th);
    boot_line(g,(int32_t)cx,(int32_t)(cy+size/2),(int32_t)(cx+size/8),(int32_t)(cy+size/8),white,th);
    boot_line(g,(int32_t)(cx+size/8),(int32_t)(cy+size/8),(int32_t)(cx+size/2),(int32_t)cy,blue,th);
}

static void boot_draw_progress(const nova_graphics_context_t *g,uint32_t x,uint32_t y,uint32_t w,uint32_t h)
{
    uint32_t inset=boot_ss(g,2);boot_fill_rect(g,x,y,w,h,0x66030a1cu);boot_stroke_rect(g,x,y,w,h,boot_ss(g,1),0xaa2d9cffu);
    uint32_t fill=(uint32_t)((uint64_t)(w-inset*2u)*boot_progress_per_mille/1000u);
    if(fill)boot_fill_rect(g,x+inset,y+inset,fill,h-inset*2u,0xff2ba8ffu);
}

static bool boot_render_splash(EFI_HANDLE image,EFI_SYSTEM_TABLE *st)
{
    if(!boot_draw_background(image,st))return false;
    const nova_graphics_context_t *g=nova_graphics_context();
    if(!g||!g->initialized)return false;
    uint32_t logo_size=boot_ss(g,420);
    uint32_t logo_x=(g->width-logo_size)/2u;
    uint32_t logo_y=boot_sy(g,80);
    if(!boot_draw_logo_asset(image,st,logo_x,logo_y,logo_size,logo_size)){
        uint32_t cx=boot_sx(g,840),cy=boot_sy(g,390),mark=boot_ss(g,170);
        boot_draw_nova_mark(g,cx,cy,mark);
        boot_draw_text(g,boot_sx(g,724),boot_sy(g,496),"NovaOS",0xffdff7ffu,boot_ss(g,8));
    }
    boot_draw_progress(g,boot_sx(g,560),boot_sy(g,662),boot_sx(g,560),boot_sy(g,8));return true;
}

static bool boot_render_console(EFI_HANDLE image,EFI_SYSTEM_TABLE *st)
{
    if(!boot_draw_background(image,st))return false;
    const nova_graphics_context_t *g=nova_graphics_context();
    if(!g||!g->initialized)return false;
    uint32_t x=boot_sx(g,180),y=boot_sy(g,110),w=boot_sx(g,1320),h=boot_sy(g,700);boot_fill_rect(g,x,y,w,h,0xdd030915u);boot_stroke_rect(g,x,y,w,h,boot_ss(g,2),0xaa2c98ffu);
    boot_draw_text(g,x+boot_ss(g,38),y+boot_ss(g,34),"NovaOS Boot Console",0xffeaf7ffu,boot_ss(g,5));boot_draw_text(g,x+boot_ss(g,40),y+boot_ss(g,88),"System Initialization and Diagnostics",0xff8aa6c4u,boot_ss(g,3));
    uint32_t log_y=y+boot_ss(g,155),line_h=boot_ss(g,34);
    for(uint8_t index=0;index<boot_log_count;++index){uint32_t color=boot_log[index].level==NOVA_BOOT_LOG_ERROR?0xffff4b7du:boot_log[index].level==NOVA_BOOT_LOG_WARN?0xffffb84du:0xff5fe38au;
        boot_draw_text(g,x+boot_ss(g,44),log_y+index*line_h,boot_log[index].level==NOVA_BOOT_LOG_ERROR?"ERROR":boot_log[index].level==NOVA_BOOT_LOG_WARN?"WARN":"INFO",color,boot_ss(g,3));
        boot_draw_text(g,x+boot_ss(g,185),log_y+index*line_h,boot_log[index].message,0xffd7e4f2u,boot_ss(g,3));}
    boot_draw_progress(g,x+boot_ss(g,44),y+h-boot_ss(g,80),w-boot_ss(g,88),boot_ss(g,10));boot_draw_text(g,x+boot_ss(g,44),y+h-boot_ss(g,48),"F3 Console | ESC Splash",0xff88a6c8u,boot_ss(g,3));return true;
}

static bool show_bootsplash(EFI_HANDLE image,EFI_SYSTEM_TABLE *st)
{boot_view=NOVA_BOOT_VIEW_SPLASH;return boot_render_splash(image,st);}

static bool show_bootconsole(EFI_HANDLE image,EFI_SYSTEM_TABLE *st)
{boot_view=NOVA_BOOT_VIEW_CONSOLE;return boot_render_console(image,st);}

static void boot_refresh_boot_status(EFI_HANDLE image,EFI_SYSTEM_TABLE *st)
{
    if(boot_view==NOVA_BOOT_VIEW_CONSOLE)(void)boot_render_console(image,st);
    else{
        (void)image;(void)st;
        const nova_graphics_context_t *g=nova_graphics_context();
        if(g&&g->initialized)
            boot_draw_progress(g,boot_sx(g,560),boot_sy(g,662),boot_sx(g,560),boot_sy(g,8));
    }
}

static void boot_view_switch_window(EFI_HANDLE image,EFI_SYSTEM_TABLE *st,unsigned polls)
{
    if(!st||!st->BootServices||!st->ConIn)return;
    for(unsigned index=0;index<polls;++index){
        EFI_INPUT_KEY key;bytes_zero(&key,sizeof(key));
        EFI_STATUS status=st->ConIn->ReadKeyStroke(st->ConIn,&key);
        if(!EFI_ERROR(status)){
            if(boot_view!=NOVA_BOOT_VIEW_CONSOLE&&key.ScanCode==13u){
                if(show_bootconsole(image,st)){
                    nova_debug_string("UEFI:BOOT-VIEW-SWITCH-F3\n");
                    nova_debug_string("UEFI:BOOT-CONSOLE-READY\n");
                }
            }else if(boot_view==NOVA_BOOT_VIEW_CONSOLE&&(key.ScanCode==23u||key.UnicodeChar==27u)){
                if(show_bootsplash(image,st)){
                    nova_debug_string("UEFI:BOOT-VIEW-SWITCH-ESC\n");
                    nova_debug_string("UEFI:BOOT-VIEW-SPLASH-READY\n");
                }
            }
        }
        st->BootServices->Stall(50000);
    }
}

static EFI_STATUS allocate_fixed(EFI_BOOT_SERVICES *bs,uint64_t address,UINTN pages)
{EFI_PHYSICAL_ADDRESS value=address;efi_allocate_pages_fn fn=(efi_allocate_pages_fn)bs->AllocatePages;return fn(EFI_ALLOCATE_ADDRESS,EFI_LOADER_DATA,pages,&value);}

static bool allocate_kernel_stack(EFI_BOOT_SERVICES *bs,uint32_t *stack_top)
{
    if(!EFI_ERROR(allocate_fixed(bs,KERNEL_STACK_BASE,16))){*stack_top=KERNEL_STACK_BASE+16*PAGE_SIZE;return true;}
    EFI_PHYSICAL_ADDRESS base=0x007fffffull;
    efi_allocate_pages_fn fn=(efi_allocate_pages_fn)bs->AllocatePages;
    if(EFI_ERROR(fn(EFI_ALLOCATE_MAX_ADDRESS,EFI_LOADER_DATA,16,&base))||
       base<0x10000ull||base+16*PAGE_SIZE>0x00800000ull)return false;
    *stack_top=(uint32_t)(base+16*PAGE_SIZE);
    nova_debug_string("UEFI:KERNEL-STACK-RELOCATED\n");
    return true;
}

static EFI_STATUS read_kernel_file(EFI_HANDLE image,EFI_SYSTEM_TABLE *st,CHAR16 *path,uint8_t **data,UINTN *size)
{
    efi_handle_protocol_fn handle=(efi_handle_protocol_fn)st->BootServices->HandleProtocol;
    efi_loaded_image_protocol *loaded=0;efi_simple_fs *fs=0;efi_file_protocol *root=0,*file=0;
    EFI_STATUS status=handle(image,&loaded_image_guid,(VOID **)&loaded);
    if(EFI_ERROR(status)||!loaded)return status;
    status=handle(loaded->DeviceHandle,&simple_fs_guid,(VOID **)&fs);
    if(EFI_ERROR(status)||!fs)return status;
    status=fs->OpenVolume(fs,&root);if(EFI_ERROR(status)||!root)return status;
    status=root->Open(root,&file,path,EFI_FILE_MODE_READ,0);root->Close(root);
    if(EFI_ERROR(status)||!file)return status;
    uint64_t end=~0ull;status=file->SetPosition(file,end);
    if(!EFI_ERROR(status))status=file->GetPosition(file,&end);
    if(!EFI_ERROR(status))status=file->SetPosition(file,0);
    if(EFI_ERROR(status)||!end||end>8u*1024u*1024u){file->Close(file);return 1;}
    efi_allocate_pool_fn allocate=(efi_allocate_pool_fn)st->BootServices->AllocatePool;
    status=allocate(EFI_LOADER_DATA,(UINTN)end,(VOID **)data);
    if(!EFI_ERROR(status)){*size=(UINTN)end;status=file->Read(file,size,*data);}
    file->Close(file);return status;
}

static UINTN align4(UINTN value){return (value+3u)&~(UINTN)3u;}

static bool read_elf32_metadata(const uint8_t *payload,UINTN size,const elf32_header *elf,uint8_t build_id[20])
{
    bool have_build_id=false,have_requirements=false;
    uint32_t cpuid_edx=0,cpuid_ecx=0,a,b,c,d;
    __asm__ volatile("cpuid":"=a"(a),"=b"(b),"=c"(c),"=d"(d):"a"(1),"c"(0));
    cpuid_edx=d;cpuid_ecx=c;
    for(uint16_t i=0;i<elf->phnum;++i){
        const elf32_program_header *ph=(const elf32_program_header *)(payload+elf->phoff+(UINTN)i*elf->phentsize);
        if(ph->type!=4)continue;
        if(!range_valid(ph->offset,ph->filesz,size))return false;
        UINTN at=ph->offset,end=ph->offset+ph->filesz;
        while(at+12u<=end){
            uint32_t namesz,descsz,type;bytes_copy(&namesz,payload+at,4);bytes_copy(&descsz,payload+at+4,4);bytes_copy(&type,payload+at+8,4);
            UINTN name_padded=align4(namesz),desc_padded=align4(descsz);
            if(name_padded<namesz||desc_padded<descsz||name_padded>end-at-12u||desc_padded>end-at-12u-name_padded)return false;
            const uint8_t *name=payload+at+12u,*desc=name+name_padded;
            if(type==3&&namesz==4&&descsz==20&&bytes_equal(name,"GNU\0",4)){bytes_copy(build_id,desc,20);have_build_id=true;}
            if(type==0x4e4f5601u&&namesz==5&&descsz==16&&bytes_equal(name,"NOVA\0",5)){
                uint32_t metadata_version,minimum_abi,required_edx,required_ecx;
                bytes_copy(&metadata_version,desc,4);bytes_copy(&minimum_abi,desc+4,4);
                bytes_copy(&required_edx,desc+8,4);bytes_copy(&required_ecx,desc+12,4);
                if(metadata_version!=1||minimum_abi>NOVA_LOADER_ABI_VERSION||
                   (cpuid_edx&required_edx)!=required_edx||(cpuid_ecx&required_ecx)!=required_ecx)return false;
                have_requirements=true;
            }
            at+=12u+name_padded+desc_padded;
        }
        if(at!=end)return false;
    }
    return have_build_id&&have_requirements;
}

static bool load_elf32(EFI_BOOT_SERVICES *bs,const uint8_t *payload,UINTN size,
                       uint32_t *entry,uint32_t *image_size,uint8_t build_id[20])
{
    if(size<sizeof(elf32_header))return false;
    const elf32_header *elf=(const elf32_header *)payload;
    if(elf->ident[0]!=0x7f||elf->ident[1]!='E'||elf->ident[2]!='L'||elf->ident[3]!='F'||
       elf->ident[4]!=1||elf->ident[5]!=1||elf->ident[6]!=1||elf->type!=2||elf->machine!=3||elf->version!=1||
       elf->ehsize!=sizeof(elf32_header)||elf->phentsize!=sizeof(elf32_program_header)||!elf->phnum||elf->phnum>64||
       elf->phnum>size/elf->phentsize||!range_valid(elf->phoff,(UINTN)elf->phnum*elf->phentsize,size))return false;
    if(!read_elf32_metadata(payload,size,elf,build_id))return false;
    uint32_t lowest=0xffffffffu,highest=0;bool executable_entry=false,have_load=false;
    for(uint16_t i=0;i<elf->phnum;++i){const elf32_program_header *ph=(const elf32_program_header *)(payload+elf->phoff+(UINTN)i*elf->phentsize);
        if(ph->type!=1)continue;
        have_load=true;
        if(ph->filesz>ph->memsz||!ph->memsz||!range_valid(ph->offset,ph->filesz,size)||
           ph->paddr<0x100000u||ph->paddr+ph->memsz<ph->paddr||(ph->flags&3u)==3u||
           (ph->align&&((ph->align&(ph->align-1u))||(ph->paddr&(ph->align-1u))!=(ph->offset&(ph->align-1u)))))return false;
        for(uint16_t j=0;j<i;++j){const elf32_program_header *other=(const elf32_program_header *)(payload+elf->phoff+(UINTN)j*elf->phentsize);
            if(other->type==1&&ph->paddr<other->paddr+other->memsz&&other->paddr<ph->paddr+ph->memsz)return false;}
        if(ph->paddr<lowest)lowest=ph->paddr;
        if(ph->paddr+ph->memsz>highest)highest=ph->paddr+ph->memsz;
        if((ph->flags&1)&&elf->entry>=ph->paddr&&elf->entry<ph->paddr+ph->memsz)executable_entry=true;
    }
    if(!have_load||lowest!=KERNEL_ADDRESS||highest<=lowest||!executable_entry)return false;
    UINTN pages=(highest-lowest+PAGE_SIZE-1)/PAGE_SIZE;
    if(EFI_ERROR(allocate_fixed(bs,lowest,pages)))return false;
    for(uint16_t i=0;i<elf->phnum;++i){const elf32_program_header *ph=(const elf32_program_header *)(payload+elf->phoff+(UINTN)i*elf->phentsize);
        if(ph->type!=1)continue;
        bytes_zero((void *)(UINTN)ph->paddr,ph->memsz);
        bytes_copy((void *)(UINTN)ph->paddr,payload+ph->offset,ph->filesz);}
    *entry=elf->entry;*image_size=highest-lowest;return true;
}

static bool read_elf64_metadata(const uint8_t *payload,UINTN size,const elf64_header *elf,uint8_t build_id[20])
{
    bool have_build_id=false,have_requirements=false;
    uint32_t a,b,c,d;__asm__ volatile("cpuid":"=a"(a),"=b"(b),"=c"(c),"=d"(d):"a"(1),"c"(0));
    for(uint16_t i=0;i<elf->phnum;++i){
        const elf64_program_header *ph=(const elf64_program_header *)(payload+(UINTN)elf->phoff+(UINTN)i*elf->phentsize);
        if(ph->type!=4)continue;
        if(ph->offset>(uint64_t)size||ph->filesz>(uint64_t)size-ph->offset)return false;
        UINTN at=(UINTN)ph->offset,end=at+(UINTN)ph->filesz;
        while(at+12u<=end){
            uint32_t namesz,descsz,type;bytes_copy(&namesz,payload+at,4);bytes_copy(&descsz,payload+at+4,4);bytes_copy(&type,payload+at+8,4);
            UINTN name_padded=align4(namesz),desc_padded=align4(descsz);
            if(name_padded<namesz||desc_padded<descsz||name_padded>end-at-12u||desc_padded>end-at-12u-name_padded)return false;
            const uint8_t *name=payload+at+12u,*desc=name+name_padded;
            if(type==3&&namesz==4&&descsz==20&&bytes_equal(name,"GNU\0",4)){bytes_copy(build_id,desc,20);have_build_id=true;}
            if(type==0x4e4f5601u&&namesz==5&&descsz==16&&bytes_equal(name,"NOVA\0",5)){
                uint32_t metadata_version,minimum_abi,required_edx,required_ecx;
                bytes_copy(&metadata_version,desc,4);bytes_copy(&minimum_abi,desc+4,4);
                bytes_copy(&required_edx,desc+8,4);bytes_copy(&required_ecx,desc+12,4);
                if(metadata_version!=1||minimum_abi>NOVA_LOADER_ABI_VERSION||
                   (d&required_edx)!=required_edx||(c&required_ecx)!=required_ecx)return false;
                have_requirements=true;
            }
            at+=12u+name_padded+desc_padded;
        }
        if(at!=end)return false;
    }
    return have_build_id&&have_requirements;
}

static bool load_elf64(EFI_BOOT_SERVICES *bs,const uint8_t *payload,UINTN size,
                       uint32_t *entry,uint32_t *load_address,uint32_t *image_size,uint8_t build_id[20])
{
    if(size<sizeof(elf64_header))return false;
    const elf64_header *elf=(const elf64_header *)payload;
    if(elf->ident[0]!=0x7f||elf->ident[1]!='E'||elf->ident[2]!='L'||elf->ident[3]!='F'||
       elf->ident[4]!=2||elf->ident[5]!=1||elf->ident[6]!=1||elf->type!=2||elf->machine!=62||elf->version!=1||
       elf->ehsize!=sizeof(elf64_header)||elf->phentsize!=sizeof(elf64_program_header)||!elf->phnum||elf->phnum>64||
       elf->phoff>(uint64_t)size||elf->phnum>(size-(UINTN)elf->phoff)/elf->phentsize)return false;
    if(!read_elf64_metadata(payload,size,elf,build_id))return false;
    uint64_t lowest=~0ull,highest=0;bool executable_entry=false,have_load=false;
    for(uint16_t i=0;i<elf->phnum;++i){const elf64_program_header *ph=(const elf64_program_header *)(payload+(UINTN)elf->phoff+(UINTN)i*elf->phentsize);
        if(ph->type!=1)continue;
        have_load=true;
        if(ph->filesz>ph->memsz||!ph->memsz||ph->offset>(uint64_t)size||ph->filesz>(uint64_t)size-ph->offset||
           ph->paddr<KERNEL64_ADDRESS||ph->paddr+ph->memsz<ph->paddr||ph->paddr+ph->memsz>0x100000000ull||
           (ph->flags&3u)==3u||(ph->align&&((ph->align&(ph->align-1u))||
           (ph->paddr&(ph->align-1u))!=(ph->offset&(ph->align-1u)))))return false;
        for(uint16_t j=0;j<i;++j){const elf64_program_header *other=(const elf64_program_header *)(payload+(UINTN)elf->phoff+(UINTN)j*elf->phentsize);
            if(other->type==1&&ph->paddr<other->paddr+other->memsz&&other->paddr<ph->paddr+ph->memsz)return false;}
        if(ph->paddr<lowest)lowest=ph->paddr;
        if(ph->paddr+ph->memsz>highest)highest=ph->paddr+ph->memsz;
        if((ph->flags&1)&&elf->entry>=ph->paddr&&elf->entry<ph->paddr+ph->memsz)executable_entry=true;
    }
    if(!have_load||lowest!=KERNEL64_ADDRESS||highest<=lowest||highest-lowest>0xffffffffull||
       elf->entry>0xffffffffull||!executable_entry)return false;
    UINTN pages=(UINTN)((highest-lowest+PAGE_SIZE-1)/PAGE_SIZE);
    if(EFI_ERROR(allocate_fixed(bs,lowest,pages)))return false;
    for(uint16_t i=0;i<elf->phnum;++i){const elf64_program_header *ph=(const elf64_program_header *)(payload+(UINTN)elf->phoff+(UINTN)i*elf->phentsize);
        if(ph->type!=1)continue;
        bytes_zero((void *)(UINTN)ph->paddr,(UINTN)ph->memsz);
        bytes_copy((void *)(UINTN)ph->paddr,payload+(UINTN)ph->offset,(UINTN)ph->filesz);}
    *entry=(uint32_t)elf->entry;*load_address=(uint32_t)lowest;*image_size=(uint32_t)(highest-lowest);return true;
}

/* §113: LZ4-Block-Decompressor (kein Frame-Format).
   Payload-Layout bei compression=LZ4: [0..3] uncompressed_size LE, [4..] LZ4-Block-Daten.
   Gibt die Anzahl geschriebener Bytes zurueck oder 0 bei Fehler. */
static uint32_t lz4_block_decompress(const uint8_t *src,uint32_t src_len,
                                     uint8_t *dst,uint32_t dst_cap)
{
    if(src_len<1)return 0;
    const uint8_t *s=src,*s_end=src+src_len;
    uint8_t *d=dst,*d_end=dst+dst_cap;
    while(s<s_end){
        uint8_t token=*s++;
        /* Literale */
        uint32_t lit_len=(uint32_t)(token>>4);
        if(lit_len==15u){
            uint8_t extra;
            do{if(s>=s_end)return 0;extra=*s++;lit_len+=extra;}while(extra==255);
        }
        if((uint32_t)(s_end-s)<lit_len||(uint32_t)(d_end-d)<lit_len)return 0;
        for(uint32_t i=0;i<lit_len;++i)*d++=*s++;
        /* Letztes Segment: kein Match-Teil */
        if(s>=s_end)break;
        /* Match-Offset (2 Bytes LE) */
        if((uint32_t)(s_end-s)<2u)return 0;
        uint32_t offset=(uint32_t)*s|(uint32_t)(*(s+1u))<<8;s+=2u;
        if(offset==0||(uint32_t)(d-dst)<offset)return 0;
        uint32_t match_len=(uint32_t)(token&0xfu)+4u;
        if((token&0xfu)==15u){
            uint8_t extra;
            do{if(s>=s_end)return 0;extra=*s++;match_len+=extra;}while(extra==255);
        }
        if((uint32_t)(d_end-d)<match_len)return 0;
        const uint8_t *match=d-offset;
        for(uint32_t i=0;i<match_len;++i)*d++=match[i];
    }
    return (uint32_t)(d-dst);
}

/* §108: load_nki_elf32 akzeptiert NKI v1 und v2.
   Bei v2 mit sig_size==64: DevSign-Block (NKTS) nach Payload pruefen.
   *sig_verified=true wenn DevSign korrekt, sonst false. */
static bool load_nki_elf32(EFI_BOOT_SERVICES *bs,const uint8_t *file,UINTN size,
                           uint32_t *entry,uint32_t *image_size,uint8_t build_id[20],
                           bool *sig_verified)
{
    *sig_verified=false;
    if(size<sizeof(nova_nki_header_t))return false;
    const nova_nki_header_t *nki=(const nova_nki_header_t *)file;
    static const uint8_t magic[8]={'N','O','V','A','N','K','I',0};
    for(UINTN i=0;i<8;++i)if(nki->magic[i]!=magic[i])return false;
    /* Version 1 oder 2 akzeptieren */
    if((nki->version!=NOVA_NKI_VERSION&&nki->version!=NOVA_NKI_VERSION_2)||
       nki->header_size!=64||nki->architecture!=NOVA_BOOT_ARCH_X86_32||
       (nki->flags&3u)!=3u||
       (nki->compression!=NOVA_NKI_COMPRESSION_NONE&&
        nki->compression!=NOVA_NKI_COMPRESSION_LZ4))return false;
    /* v1: sig_size muss 0 sein; v2: 0 oder NOVA_NKI_SIG_SIZE_DEVSIGN */
    if(nki->version==NOVA_NKI_VERSION&&nki->sig_size!=0)return false;
    if(nki->version==NOVA_NKI_VERSION_2&&
       nki->sig_size!=0&&nki->sig_size!=NOVA_NKI_SIG_SIZE_DEVSIGN)return false;
    /* Payload-Groesse und Datei-Grenzen pruefen (inkl. optionalem Sig-Block) */
    UINTN required=64u+(UINTN)nki->image_size+(UINTN)nki->sig_size;
    if(!range_valid(64,nki->image_size,size)||size<required)return false;
    const uint8_t *payload=file+64;
    if(crc32_with_zero(payload,nki->image_size,nki->image_size,0)!=nki->payload_crc32)return false;
    /* §113: LZ4-Dekompression wenn compression==LZ4 */
    uint8_t *decompressed_buf=NULL;
    uint32_t decompressed_size=0;
    if(nki->compression==NOVA_NKI_COMPRESSION_LZ4){
        if(nki->image_size<5u)return false;
        uint32_t uncomp_size=0;
        bytes_copy(&uncomp_size,payload,4);
        if(uncomp_size==0||uncomp_size>4u*1024u*1024u)return false;
        typedef EFI_STATUS (EFIAPI *efi_allocate_pool_fn2)(uint32_t,UINTN,VOID **);
        efi_allocate_pool_fn2 alloc_pool=(efi_allocate_pool_fn2)bs->AllocatePool;
        if(EFI_ERROR(alloc_pool(EFI_LOADER_DATA,uncomp_size,(VOID **)&decompressed_buf)))return false;
        uint32_t written=lz4_block_decompress(payload+4u,nki->image_size-4u,decompressed_buf,uncomp_size);
        if(written!=uncomp_size){
            typedef EFI_STATUS (EFIAPI *efi_free_pool_fn)(VOID *);
            efi_free_pool_fn free_pool=(efi_free_pool_fn)bs->FreePool;
            free_pool(decompressed_buf);
            return false;
        }
        payload=decompressed_buf;
        decompressed_size=uncomp_size;
        nova_debug_string("UEFI:KERNEL-LZ4-DECOMPRESSED\n");
    }
    /* §108 DevSign-Verifikation fuer NKI v2 mit sig_size==64.
       Sig-Block liegt im Originalbild (file+64+image_size), nicht im Dekomprimierungs-Puffer. */
    const uint8_t *file_payload=file+64; /* Original-Payload fuer Sig-Block-Lokalisierung */
    if(nki->version==NOVA_NKI_VERSION_2&&nki->sig_size==NOVA_NKI_SIG_SIZE_DEVSIGN){
        const nova_nki_signature_t *sig=(const nova_nki_signature_t *)(file_payload+nki->image_size);
        static const uint8_t nkts[4]={'N','K','T','S'};
        if(bytes_equal(sig->magic,nkts,4)&&sig->scheme==NOVA_NKI_SCHEME_DEVSIGN&&
           sig->key_id==NOVA_NKI_DEVSIGN_KEY_ID){
            /* §111 Revocation-Policy: revocation_gen != 0 wird hart abgewiesen */
            if(sig->revocation_gen!=0){
                nova_debug_string("UEFI:KERNEL-DEVSIGN-REVOKED\n");
                boot_log_add(NOVA_BOOT_LOG_ERROR,"KERNEL DEVSIGN REVOKED (revocation_gen != 0)");
                return false;
            }
            /* dev_mac = payload_crc32 XOR "NOVD" */
            uint32_t expected_mac=nki->payload_crc32^NOVA_NKI_DEVSIGN_XOR_MASK;
            uint32_t stored_mac=0;
            bytes_copy(&stored_mac,sig->sig_data,4);
            if(stored_mac==expected_mac&&bytes_equal(sig->build_id,nki->build_id,16))
                *sig_verified=true;
        }
    }
    uint8_t elf_build_id[20];
    uint32_t elf_payload_size=decompressed_buf?decompressed_size:nki->image_size;
    bool elf_ok=load_elf32(bs,payload,elf_payload_size,entry,image_size,elf_build_id);
    if(decompressed_buf){
        typedef EFI_STATUS (EFIAPI *efi_free_pool_fn)(VOID *);
        efi_free_pool_fn free_pool=(efi_free_pool_fn)bs->FreePool;
        free_pool(decompressed_buf);
    }
    if(!elf_ok)return false;
    if(*entry!=nki->entry_point||nki->load_address!=KERNEL_ADDRESS||!bytes_equal(nki->build_id,elf_build_id,16)){
        efi_free_pages_fn free_pages=(efi_free_pages_fn)bs->FreePages;
        free_pages(KERNEL_ADDRESS,(*image_size+PAGE_SIZE-1u)/PAGE_SIZE);
        return false;
    }
    bytes_copy(build_id,elf_build_id,20);return true;
}

static uint8_t *append_tlv(uint8_t *at,uint16_t type,uint16_t flags,uint32_t length)
{nova_bib_tlv_header_t *header=(nova_bib_tlv_header_t *)at;header->type=type;header->flags=flags;header->length=length;bytes_zero(at+8,length);return at+8;}

static uint8_t acpi_checksum(const uint8_t *bytes,UINTN length)
{uint8_t sum=0;for(UINTN i=0;i<length;++i)sum=(uint8_t)(sum+bytes[i]);return sum;}

static uint32_t find_acpi_rsdp(const EFI_SYSTEM_TABLE *st)
{
    static const EFI_GUID acpi20={0x8868e871,0xe4f1,0x11d3,{0xbc,0x22,0,0x80,0xc7,0x3c,0x88,0x81}};
    static const EFI_GUID acpi10={0xeb9d2d30,0x2d88,0x11d3,{0x9a,0x16,0,0x90,0x27,0x3f,0xc1,0x4d}};
    const EFI_CONFIGURATION_TABLE *tables=st->ConfigurationTable;
    if(!tables||st->NumberOfTableEntries>4096)return 0;
    for(unsigned pass=0;pass<2;++pass)for(UINTN i=0;i<st->NumberOfTableEntries;++i){
        const EFI_GUID *wanted=pass?&acpi10:&acpi20;
        if(!bytes_equal(&tables[i].VendorGuid,wanted,sizeof(*wanted)))continue;
        UINTN address=(UINTN)tables[i].VendorTable;
        if(!address||address>0xffffffffull-36)continue;
        const uint8_t *rsdp=(const uint8_t *)address;
        if(!bytes_equal(rsdp,"RSD PTR ",8)||acpi_checksum(rsdp,20))continue;
        if(rsdp[15]>=2){uint32_t length;
            bytes_copy(&length,rsdp+20,4);
            if(length<36||length>4096||address>0xffffffffull-length||acpi_checksum(rsdp,length))continue;}
        return (uint32_t)address;
    }
    return 0;
}

static uint32_t current_secure_boot_state(uint32_t *security_flags)
{
    const nova_uefi_firmware_status_t *firmware=uefi_firmware_status();
    if(!firmware)return NOVA_SECURE_BOOT_UNKNOWN;
    if(firmware->setup_mode_known&&firmware->setup_mode){
        *security_flags|=NOVA_BOOT_SECURITY_FIRMWARE_STATE_KNOWN;
        return NOVA_SECURE_BOOT_SETUP_MODE;
    }
    if(!firmware->secure_boot_known)return NOVA_SECURE_BOOT_UNKNOWN;
    *security_flags|=NOVA_BOOT_SECURITY_FIRMWARE_STATE_KNOWN;
    return firmware->secure_boot?NOVA_SECURE_BOOT_ENABLED:NOVA_SECURE_BOOT_DISABLED;
}

/* Kept allocation-free between the successful GetMemoryMap call and
     ExitBootServices. The descriptor slack absorbs the pool allocation's own
     map entries; a firmware-requested resize is followed by a fresh query. */
static EFI_STATUS refresh_memory_map(EFI_BOOT_SERVICES *bs,VOID **map,UINTN *capacity,
                                     UINTN *map_size,UINTN *map_key,UINTN *descriptor_size,
                                     uint32_t *descriptor_version)
{
    efi_allocate_pool_fn allocate=(efi_allocate_pool_fn)bs->AllocatePool;
    efi_get_memory_map_fn getmap=(efi_get_memory_map_fn)bs->GetMemoryMap;
    for(unsigned attempt=0;attempt<4;++attempt){
        *map_size=*capacity;
        EFI_STATUS status=getmap(map_size,*map,map_key,descriptor_size,descriptor_version);
        if(!EFI_ERROR(status))return EFI_SUCCESS;
        if(status!=EFI_BUFFER_TOO_SMALL)return status;
        UINTN stride=*descriptor_size?*descriptor_size:sizeof(efi_memory_descriptor);
        if(stride>~(UINTN)0/8u||*map_size>~(UINTN)0-stride*8u)return 1;
        UINTN wanted=*map_size+stride*8u;
        if(*map){bs->FreePool(*map);*map=0;*capacity=0;}
        status=allocate(EFI_LOADER_DATA,wanted,map);
        if(EFI_ERROR(status))return status;
        *capacity=wanted;
    }
    return EFI_BUFFER_TOO_SMALL;
}

static UINTN build_bib(const EFI_SYSTEM_TABLE *st,const VOID *map,UINTN map_size,UINTN descriptor_size,uint32_t entry,
                       uint32_t load_address,uint32_t image_size,const uint8_t build_id[20],uint32_t architecture,
                       uint32_t kernel_format,bool nki_container,uint32_t verification_state,
                       uint32_t boot_mode,uint32_t boot_options_flags,uint32_t selected_generation,
                       uint32_t fallback_level)
{
    if(!map||descriptor_size<sizeof(efi_memory_descriptor)||map_size<descriptor_size)return 0;
    uint8_t *base=(uint8_t *)(UINTN)BIB_ADDRESS;bytes_zero(base,0x800);
    nova_bib_header_t *header=(nova_bib_header_t *)base;
    const uint8_t magic[8]={'N','B','H','P','B','I','B',0};bytes_copy(header->magic,magic,8);
    header->version_major=1;header->version_minor=2;header->header_size=32;
    header->architecture=architecture;
    uint8_t *at=base+32,*value;
    value=append_tlv(at,NOVA_BIB_TLV_FIRMWARE,NOVA_BIB_TLV_FLAG_REQUIRED,16);((uint32_t *)value)[0]=UEFI_PLATFORM;at=value+16;
    value=append_tlv(at,NOVA_BIB_TLV_MEMORY,NOVA_BIB_TLV_FLAG_REQUIRED,16);nova_bib_memory_t *memory=(nova_bib_memory_t *)value;
    memory->map_address=MEMORY_MAP_ADDRESS;memory->entry_size=MEMORY_ENTRY_SIZE;at=value+16;
    nova_e820_entry *out=(nova_e820_entry *)(UINTN)MEMORY_MAP_ADDRESS;UINTN count=0;
    for(UINTN off=0;off+descriptor_size<=map_size;off+=descriptor_size){const efi_memory_descriptor *in=(const efi_memory_descriptor *)((const uint8_t *)map+off);
        if(!in->NumberOfPages)continue;
        if(count>=MEMORY_ENTRY_CAPACITY||in->NumberOfPages>~0ull/PAGE_SIZE)return 0;
        uint64_t length=in->NumberOfPages*PAGE_SIZE;if(in->PhysicalStart+length<in->PhysicalStart)return 0;
        out[count].Address=in->PhysicalStart;out[count].Length=length;
        out[count].Type=in->Type==EFI_CONVENTIONAL_MEMORY?1u:2u;out[count].Attributes=(uint32_t)in->Attribute;++count;}
    memory->entry_count=(uint32_t)count;
    const nova_graphics_context_t *graphics=nova_graphics_context();
    if(graphics&&graphics->initialized&&graphics->framebuffer_address<=0xffffffffull){value=append_tlv(at,NOVA_BIB_TLV_GRAPHICS,0,32);nova_bib_graphics_t *g=(nova_bib_graphics_t *)value;
        g->framebuffer_address=(uint32_t)graphics->framebuffer_address;g->pitch=graphics->pitch;g->width=graphics->width;g->height=graphics->height;g->bits_per_pixel=graphics->bits_per_pixel;g->pixel_format=1;header->flags|=NOVA_BOOT_FLAG_FRAMEBUFFER;at=value+32;}
    value=append_tlv(at,NOVA_BIB_TLV_KERNEL,NOVA_BIB_TLV_FLAG_REQUIRED,32);nova_bib_kernel_t *kernel=(nova_bib_kernel_t *)value;
    kernel->load_address=load_address;kernel->image_size=image_size;kernel->entry_point=entry;at=value+32;
    value=append_tlv(at,NOVA_BIB_TLV_SECURITY,NOVA_BIB_TLV_FLAG_REQUIRED,16);nova_bib_security_t *security=(nova_bib_security_t *)value;
    security->verification_state=verification_state;
    security->flags=NOVA_BOOT_SECURITY_ELF_BUILD_ID_VALID|(nki_container?NOVA_BOOT_SECURITY_NKI_CRC32_VALID:0u)|
                   (verification_state>=NOVA_BOOT_VERIFICATION_SIGNATURE_VERIFIED?NOVA_BOOT_SECURITY_SIGNATURE_PRESENT:0u);
    security->secure_boot_state=current_secure_boot_state(&security->flags);
    security->entropy_quality=1;at=value+16;
    value=append_tlv(at,NOVA_BIB_TLV_BOOT_OPTIONS,NOVA_BIB_TLV_FLAG_REQUIRED,16);
    nova_bib_boot_options_t *options=(nova_bib_boot_options_t *)value;
    options->boot_mode=boot_mode;options->flags=boot_options_flags;
    options->selected_generation=selected_generation;options->fallback_level=fallback_level;at=value+16;
    value=append_tlv(at,NOVA_BIB_TLV_CPU,NOVA_BIB_TLV_FLAG_REQUIRED,32);uint32_t a,b,c,d;__asm__ volatile("cpuid":"=a"(a),"=b"(b),"=c"(c),"=d"(d):"a"(0),"c"(0));
    bytes_copy(value,&b,4);bytes_copy(value+4,&d,4);bytes_copy(value+8,&c,4);((uint32_t *)value)[3]=a;__asm__ volatile("cpuid":"=a"(a),"=b"(b),"=c"(c),"=d"(d):"a"(1),"c"(0));((uint32_t *)value)[4]=d;((uint32_t *)value)[5]=c;at=value+32;
    value=append_tlv(at,NOVA_BIB_TLV_ENTROPY,NOVA_BIB_TLV_FLAG_REQUIRED,32);uint64_t tsc;uint32_t lo,hi;__asm__ volatile("rdtsc":"=a"(lo),"=d"(hi));tsc=((uint64_t)hi<<32)|lo;bytes_copy(value,&tsc,8);tsc^=0x4e6f76614f535545ull;bytes_copy(value+8,&tsc,8);((uint32_t *)value)[4]=1;((uint32_t *)value)[5]=1;((uint32_t *)value)[6]=16;at=value+32;
    value=append_tlv(at,NOVA_BIB_TLV_SYSTEM,NOVA_BIB_TLV_FLAG_REQUIRED,16);
    nova_bib_system_t *system=(nova_bib_system_t *)value;
    const nova_boot_control_record_t *control=uefi_boot_control_state();
    system->generation=control&&selected_generation<=NOVA_BOOT_GENERATION_BACKUP?
        control->slot_generation[selected_generation]:selected_generation;
    system->boot_attempt=control?control->attempt_count:0u;
    system->flags=uefi_boot_control_persistent()?1u:0u;at=value+16;
    uint32_t rsdp=find_acpi_rsdp(st);
    if(rsdp){value=append_tlv(at,NOVA_BIB_TLV_ACPI,0,16);((nova_bib_pointer_info_t *)value)->address=rsdp;at=value+16;}
    nova_bib_firmware_runtime_t runtime_descriptor;
    if(uefi_runtime_bridge_descriptor(&runtime_descriptor)){
        value=append_tlv(at,NOVA_BIB_TLV_FIRMWARE_RUNTIME,0,sizeof(runtime_descriptor));
        bytes_copy(value,&runtime_descriptor,sizeof(runtime_descriptor));at=value+sizeof(runtime_descriptor);
    }
    value=append_tlv(at,NOVA_BIB_TLV_KERNEL_IDENTITY,0,32);bytes_copy(value,build_id,20);((uint32_t *)value)[5]=kernel_format;((uint32_t *)value)[6]=nki_container?1u:0u;at=value+32;
    header->total_size=(uint32_t)(at-base);header->checksum=crc32_with_zero(base,header->total_size,20,4);return count;
}

static EFI_STATUS boot_kernel(EFI_HANDLE image_handle,EFI_SYSTEM_TABLE *st,bool recovery_only)
{
    (void)uefi_firmware_refresh();
    boot_view=NOVA_BOOT_VIEW_SPLASH;boot_progress_per_mille=0;boot_log_count=0;
    boot_log_add(NOVA_BOOT_LOG_INFO,"UEFI BOOT LOADER STARTED");
    boot_set_progress(40);
    if(show_bootsplash(image_handle,st)){
        nova_debug_string("UEFI:BOOTSPLASH-READY\n");
        nova_debug_string("UEFI:BOOT-VIEW-SPLASH-READY\n");
    }
    else nova_debug_string("UEFI:BOOTSPLASH-ERROR\n");
    uint8_t *file=0;UINTN size=0;bool nki_container=true;
    bool metadata_recovery=uefi_boot_control_requires_recovery();
    bool recovery_mode=recovery_only||metadata_recovery;
    bool automatic_recovery=metadata_recovery,automatic_rollback=false;
    const nova_boot_control_record_t *control=uefi_boot_control_state();
    uint32_t requested_slot=recovery_mode?NOVA_BOOT_GENERATION_RECOVERY:uefi_boot_control_select();
    bool candidate_boot=!recovery_mode&&control&&control->candidate_slot==requested_slot;
    uint32_t selected_generation=requested_slot;
    uint32_t fallback_level=metadata_recovery?2u:0u;
    uint32_t architecture=NOVA_BOOT_ARCH_X86_32,kernel_format=NOVA_KERNEL_FORMAT_ELF32;
    CHAR16 *initial_path=recovery_mode?recovery_nki_path:
        (requested_slot==NOVA_BOOT_GENERATION_BACKUP?backup_nki_path:nki_path);
    EFI_STATUS status=read_kernel_file(image_handle,st,initial_path,&file,&size);
    if(!recovery_mode&&requested_slot==NOVA_BOOT_GENERATION_PRIMARY&&!candidate_boot&&EFI_ERROR(status)){
        nki_container=false;file=0;size=0;
        status=read_kernel_file(image_handle,st,elf_path,&file,&size);
        if(EFI_ERROR(status)){
            file=0;size=0;architecture=NOVA_BOOT_ARCH_X86_64;kernel_format=NOVA_KERNEL_FORMAT_ELF64;
            status=read_kernel_file(image_handle,st,elf64_path,&file,&size);
        }
    }
    uint32_t entry=0,load_address=KERNEL_ADDRESS,image_size=0;uint8_t build_id[20];bytes_zero(build_id,sizeof(build_id));
    bool loaded=false;bool sig_verified=false;
    if(!EFI_ERROR(status)){
        if(nki_container)loaded=load_nki_elf32(st->BootServices,file,size,&entry,&image_size,build_id,&sig_verified);
        else if(kernel_format==NOVA_KERNEL_FORMAT_ELF32)loaded=load_elf32(st->BootServices,file,size,&entry,&image_size,build_id);
        else loaded=load_elf64(st->BootServices,file,size,&entry,&load_address,&image_size,build_id);
    }
    if(!loaded&&recovery_mode){
        if(file)st->BootServices->FreePool(file);
        nova_debug_string(EFI_ERROR(status)?"UEFI:RECOVERY-KERNEL-FILE-ERROR\n":"UEFI:KERNEL-VALIDATION-ERROR\n");
        boot_log_add(NOVA_BOOT_LOG_ERROR,EFI_ERROR(status)?"RECOVERY KERNEL FILE NOT READABLE":"RECOVERY KERNEL VALIDATION FAILED");
        boot_set_progress(1000);
        if(show_bootconsole(image_handle,st))nova_debug_string("UEFI:BOOT-CONSOLE-ERROR-VIEW\n");
        return 1;
    }
    if(!loaded){
        if(file)st->BootServices->FreePool(file);
        file=0;size=0;
        nova_debug_string(EFI_ERROR(status)?"UEFI:PRIMARY-KERNEL-FILE-ERROR\n":"UEFI:PRIMARY-KERNEL-VALIDATION-ERROR\n");
        nki_container=true;architecture=NOVA_BOOT_ARCH_X86_32;kernel_format=NOVA_KERNEL_FORMAT_ELF32;
        entry=0;load_address=KERNEL_ADDRESS;image_size=0;bytes_zero(build_id,sizeof(build_id));
        if(candidate_boot)(void)uefi_boot_control_artifact_failed(requested_slot);
        uint32_t fallback_slot=candidate_boot&&control?control->known_good_slot:
            (requested_slot==NOVA_BOOT_GENERATION_PRIMARY?NOVA_BOOT_GENERATION_BACKUP:NOVA_BOOT_GENERATION_PRIMARY);
        CHAR16 *fallback_path=fallback_slot==NOVA_BOOT_GENERATION_BACKUP?backup_nki_path:nki_path;
        status=read_kernel_file(image_handle,st,fallback_path,&file,&size);
        if(!EFI_ERROR(status))loaded=load_nki_elf32(st->BootServices,file,size,&entry,&image_size,build_id,&sig_verified);
        if(loaded){automatic_rollback=true;selected_generation=fallback_slot;fallback_level=1;}
        else{
            if(file)st->BootServices->FreePool(file);
            file=0;size=0;
            if(!EFI_ERROR(status))nova_debug_string("UEFI:BACKUP-KERNEL-VALIDATION-ERROR\n");
        }
    }
    if(!loaded){
        recovery_mode=true;automatic_recovery=true;
        selected_generation=NOVA_BOOT_GENERATION_RECOVERY;fallback_level=2;
        entry=0;load_address=KERNEL_ADDRESS;image_size=0;bytes_zero(build_id,sizeof(build_id));
        architecture=NOVA_BOOT_ARCH_X86_32;kernel_format=NOVA_KERNEL_FORMAT_ELF32;
        status=read_kernel_file(image_handle,st,recovery_nki_path,&file,&size);
        if(!EFI_ERROR(status))loaded=load_nki_elf32(st->BootServices,file,size,&entry,&image_size,build_id,&sig_verified);
    }
    if(!loaded){
        if(file)st->BootServices->FreePool(file);
        nova_debug_string("UEFI:KERNEL-VALIDATION-ERROR\n");
        boot_log_add(NOVA_BOOT_LOG_ERROR,"NO VALID PRIMARY BACKUP OR RECOVERY KERNEL");
        boot_set_progress(1000);
        if(show_bootconsole(image_handle,st))nova_debug_string("UEFI:BOOT-CONSOLE-ERROR-VIEW\n");
        return 1;
    }
    if(candidate_boot&&selected_generation==requested_slot){
        if(!uefi_boot_control_begin_attempt(requested_slot))nova_debug_string("UEFI:BOOT-CONTROL-ATTEMPT-VOLATILE\n");
        else nova_debug_string("UEFI:BOOT-CONTROL-CANDIDATE-ATTEMPT\n");
    }
    if(EFI_ERROR(allocate_fixed(st->BootServices,BIB_ADDRESS,2))){nova_debug_string("UEFI:BIB-MEMORY-ERROR\n");return 1;}
    /* §108: Verifikationsstufe haengt davon ab, ob DevSign erfolgreich war */
    uint32_t verification_state=sig_verified?NOVA_BOOT_VERIFICATION_SIGNATURE_VERIFIED:
                                (nki_container?NOVA_BOOT_VERIFICATION_INTEGRITY_VERIFIED:
                                               NOVA_BOOT_VERIFICATION_STRUCTURE_VALIDATED);
    uint32_t stack_top=0;
    if(!allocate_kernel_stack(st->BootServices,&stack_top)){nova_debug_string("UEFI:KERNEL-STACK-MEMORY-ERROR\n");return 1;}
    st->BootServices->FreePool(file);
    if(candidate_boot&&!automatic_rollback&&!recovery_mode)
        nova_debug_string("UEFI:CANDIDATE-KERNEL-SELECTED\n");
    if(automatic_rollback){
        if(selected_generation==NOVA_BOOT_GENERATION_BACKUP){
            nova_debug_string("UEFI:AUTOMATIC-BACKUP-SELECTED\n");
            nova_debug_string("UEFI:BACKUP-NKI-VALIDATED\n");
        }else{
            nova_debug_string("UEFI:AUTOMATIC-KNOWN-GOOD-SELECTED\n");
            nova_debug_string("UEFI:KNOWN-GOOD-NKI-VALIDATED\n");
        }
    }else if(recovery_mode){
        nova_debug_string(automatic_recovery?"UEFI:AUTOMATIC-RECOVERY-SELECTED\n":"UEFI:MANUAL-RECOVERY-SELECTED\n");
        nova_debug_string("UEFI:RECOVERY-NKI-VALIDATED\n");
    }else if(nki_container)nova_debug_string("UEFI:NKI-VALIDATED\n");
    else nova_debug_string(kernel_format==NOVA_KERNEL_FORMAT_ELF32?"UEFI:ELF32-DIRECT-VALIDATED\n":"UEFI:ELF64-DIRECT-VALIDATED\n");
    nova_debug_string(nki_container?"UEFI:KERNEL-INTEGRITY-VERIFIED\n":"UEFI:KERNEL-STRUCTURE-VALIDATED\n");
    /* §108: Signaturmeldung nur ausgeben wenn kein DevSign verifiziert */
    if(sig_verified){
        nova_debug_string("UEFI:KERNEL-DEVSIGN-VERIFIED\n");
        boot_log_add(NOVA_BOOT_LOG_INFO,"KERNEL DEVSIGN SIGNATURE VERIFIED");
    }else{
        nova_debug_string("UEFI:KERNEL-SIGNATURE-NOT-PRESENT\n");
        boot_log_add(NOVA_BOOT_LOG_WARN,"KERNEL SIGNATURE NOT PRESENT");
    }
    boot_log_add(NOVA_BOOT_LOG_INFO,nki_container?"NKI KERNEL IMAGE VALIDATED":"ELF KERNEL IMAGE VALIDATED");
    boot_set_progress(450);
    boot_refresh_boot_status(image_handle,st);
    boot_view_switch_window(image_handle,st,24);
    if(uefi_runtime_bridge_prepare(st)){
        nova_debug_string("UEFI:FIRMWARE-RUNTIME-BRIDGE-READY\n");
        boot_log_add(NOVA_BOOT_LOG_INFO,"FIRMWARE RUNTIME BRIDGE READY");
    }else{
        nova_debug_string("UEFI:FIRMWARE-RUNTIME-BRIDGE-UNAVAILABLE\n");
        boot_log_add(NOVA_BOOT_LOG_WARN,"FIRMWARE RUNTIME BRIDGE UNAVAILABLE");
    }
    boot_set_progress(620);boot_refresh_boot_status(image_handle,st);
    UINTN map_capacity=0,map_size=0,map_key=0,descriptor_size=0;uint32_t descriptor_version=0;VOID *map=0;
    efi_exit_boot_services_fn exit_bs=(efi_exit_boot_services_fn)st->BootServices->ExitBootServices;
    for(unsigned attempt=0;attempt<3;++attempt){
        status=refresh_memory_map(st->BootServices,&map,&map_capacity,&map_size,&map_key,&descriptor_size,&descriptor_version);
        if(EFI_ERROR(status)||!descriptor_size){nova_debug_string("UEFI:MEMORY-MAP-ERROR\n");return status;}
        uint32_t boot_flags=automatic_rollback?NOVA_BOOT_OPTION_AUTOMATIC_ROLLBACK:
            (automatic_recovery?NOVA_BOOT_OPTION_AUTOMATIC_RECOVERY:
            (recovery_mode?NOVA_BOOT_OPTION_MANUAL_RECOVERY:0u));
        if(!build_bib(st,map,map_size,descriptor_size,entry,load_address,image_size,build_id,architecture,kernel_format,nki_container,verification_state,
                      recovery_mode?NOVA_BOOT_MODE_RECOVERY:NOVA_BOOT_MODE_NORMAL,boot_flags,selected_generation,
                      fallback_level)){nova_debug_string("UEFI:BIB-ERROR\n");return 1;}
        nova_debug_string("UEFI:NBHP-BIB-READY\n");
        boot_log_add(NOVA_BOOT_LOG_INFO,"NBHP BIB HANDOFF BLOCK READY");
        boot_set_progress(820);boot_refresh_boot_status(image_handle,st);
        status=exit_bs(image_handle,map_key);
        if(!EFI_ERROR(status))break;
        nova_debug_string("UEFI:EXIT-BOOT-SERVICES-RETRY\n");
    }
    if(EFI_ERROR(status)){nova_debug_string("UEFI:EXIT-BOOT-SERVICES-ERROR\n");return status;}
    boot_log_add(NOVA_BOOT_LOG_INFO,"EXIT BOOT SERVICES READY");
    boot_log_add(NOVA_BOOT_LOG_INFO,"HANDOFF TO KERNEL");
    boot_set_progress(950);boot_refresh_boot_status(image_handle,st);
    nova_debug_string("UEFI:EXIT-BOOT-SERVICES-READY\n");nova_debug_string("UEFI:KERNEL-HANDOFF-READY\n");
    if(kernel_format==NOVA_KERNEL_FORMAT_ELF64)uefi_enter_kernel64(entry,(uint32_t)BIB_ADDRESS,stack_top);
    else uefi_enter_kernel32(entry,(uint32_t)BIB_ADDRESS,stack_top);
    for(;;)__asm__ volatile("hlt");
}

EFI_STATUS uefi_boot_kernel(EFI_HANDLE image_handle,EFI_SYSTEM_TABLE *st)
{return boot_kernel(image_handle,st,false);}

EFI_STATUS uefi_boot_recovery_kernel(EFI_HANDLE image_handle,EFI_SYSTEM_TABLE *st)
{return boot_kernel(image_handle,st,true);}
