#include "kernel_loader.h"
#include "../bootmenu/graphics.h"
#include "../../include/nova_boot_protocol.h"

#define EFI_ALLOCATE_ADDRESS 2u
#define EFI_LOADER_DATA 2u
#define EFI_FILE_MODE_READ 1ull
#define EFI_CONVENTIONAL_MEMORY 7u
#define BIB_ADDRESS 0x5000ull
#define MEMORY_MAP_ADDRESS 0x5800ull
#define KERNEL_ADDRESS 0x100000ull
#define KERNEL_STACK_BASE 0x80000ull
#define PAGE_SIZE 4096ull
#define UEFI_PLATFORM 2u
#define MEMORY_ENTRY_SIZE 24u
#define MEMORY_ENTRY_CAPACITY 64u

typedef EFI_STATUS (EFIAPI *efi_handle_protocol_fn)(EFI_HANDLE,EFI_GUID *,VOID **);
typedef EFI_STATUS (EFIAPI *efi_allocate_pages_fn)(uint32_t,uint32_t,UINTN,EFI_PHYSICAL_ADDRESS *);
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

extern void EFIAPI uefi_enter_kernel32(uint32_t entry,uint32_t bib);

static EFI_GUID loaded_image_guid={0x5b1b31a1,0x9562,0x11d2,{0x8e,0x3f,0x00,0xa0,0xc9,0x69,0x72,0x3b}};
static EFI_GUID simple_fs_guid={0x964e5b22,0x6459,0x11d2,{0x8e,0x39,0x00,0xa0,0xc9,0x69,0x72,0x3b}};
static CHAR16 nki_path[]={'\\','N','O','V','A','.','N','K','I',0};

static void bytes_zero(void *target,UINTN length){uint8_t *p=target;while(length--)*p++=0;}
static void bytes_copy(void *target,const void *source,UINTN length){uint8_t *d=target;const uint8_t *s=source;while(length--)*d++=*s++;}
static uint32_t crc32_with_zero(const uint8_t *data,UINTN size,UINTN zero_offset,UINTN zero_size)
{uint32_t crc=0xffffffffu;for(UINTN i=0;i<size;++i){uint8_t value=(i>=zero_offset&&i<zero_offset+zero_size)?0:data[i];crc^=value;for(uint32_t bit=0;bit<8;++bit)crc=(crc>>1)^((crc&1)?0xedb88320u:0);}return ~crc;}
static bool range_valid(UINTN offset,UINTN length,UINTN total){return offset<=total&&length<=total-offset;}

static EFI_STATUS allocate_fixed(EFI_BOOT_SERVICES *bs,uint64_t address,UINTN pages)
{EFI_PHYSICAL_ADDRESS value=address;efi_allocate_pages_fn fn=(efi_allocate_pages_fn)bs->AllocatePages;return fn(EFI_ALLOCATE_ADDRESS,EFI_LOADER_DATA,pages,&value);}

static EFI_STATUS read_nki(EFI_HANDLE image,EFI_SYSTEM_TABLE *st,uint8_t **data,UINTN *size)
{
    efi_handle_protocol_fn handle=(efi_handle_protocol_fn)st->BootServices->HandleProtocol;
    efi_loaded_image_protocol *loaded=0;efi_simple_fs *fs=0;efi_file_protocol *root=0,*file=0;
    EFI_STATUS status=handle(image,&loaded_image_guid,(VOID **)&loaded);
    if(EFI_ERROR(status)||!loaded)return status;
    status=handle(loaded->DeviceHandle,&simple_fs_guid,(VOID **)&fs);
    if(EFI_ERROR(status)||!fs)return status;
    status=fs->OpenVolume(fs,&root);if(EFI_ERROR(status)||!root)return status;
    status=root->Open(root,&file,nki_path,EFI_FILE_MODE_READ,0);root->Close(root);
    if(EFI_ERROR(status)||!file)return status;
    uint64_t end=~0ull;status=file->SetPosition(file,end);
    if(!EFI_ERROR(status))status=file->GetPosition(file,&end);
    if(!EFI_ERROR(status))status=file->SetPosition(file,0);
    if(EFI_ERROR(status)||!end||end>1024u*1024u){file->Close(file);return 1;}
    efi_allocate_pool_fn allocate=(efi_allocate_pool_fn)st->BootServices->AllocatePool;
    status=allocate(EFI_LOADER_DATA,(UINTN)end,(VOID **)data);
    if(!EFI_ERROR(status)){*size=(UINTN)end;status=file->Read(file,size,*data);}
    file->Close(file);return status;
}

static bool load_nki_elf32(EFI_BOOT_SERVICES *bs,const uint8_t *file,UINTN size,
                           uint32_t *entry,uint32_t *image_size,uint8_t build_id[16])
{
    if(size<sizeof(nova_nki_header_t))return false;
    const nova_nki_header_t *nki=(const nova_nki_header_t *)file;
    static const uint8_t magic[8]={'N','O','V','A','N','K','I',0};
    for(UINTN i=0;i<8;++i)if(nki->magic[i]!=magic[i])return false;
    if(nki->version!=1||nki->header_size!=64||nki->architecture!=NOVA_BOOT_ARCH_X86_32||
       nki->compression||!range_valid(64,nki->image_size,size))return false;
    const uint8_t *payload=file+64;
    if(crc32_with_zero(payload,nki->image_size,nki->image_size,0)!=nki->payload_crc32)return false;
    if(nki->image_size<sizeof(elf32_header))return false;
    const elf32_header *elf=(const elf32_header *)payload;
    if(elf->ident[0]!=0x7f||elf->ident[1]!='E'||elf->ident[2]!='L'||elf->ident[3]!='F'||
       elf->ident[4]!=1||elf->ident[5]!=1||elf->type!=2||elf->machine!=3||
       elf->phentsize!=sizeof(elf32_program_header)||!elf->phnum||
       !range_valid(elf->phoff,(UINTN)elf->phnum*elf->phentsize,nki->image_size))return false;
    uint32_t lowest=0xffffffffu,highest=0;bool executable_entry=false;
    for(uint16_t i=0;i<elf->phnum;++i){const elf32_program_header *ph=(const elf32_program_header *)(payload+elf->phoff+(UINTN)i*elf->phentsize);
        if(ph->type!=1)continue;
        if(ph->filesz>ph->memsz||!range_valid(ph->offset,ph->filesz,nki->image_size)||
           ph->paddr<0x100000u||ph->paddr+ph->memsz<ph->paddr)return false;
        if(ph->paddr<lowest)lowest=ph->paddr;
        if(ph->paddr+ph->memsz>highest)highest=ph->paddr+ph->memsz;
        if((ph->flags&1)&&elf->entry>=ph->paddr&&elf->entry<ph->paddr+ph->memsz)executable_entry=true;
    }
    if(lowest!=KERNEL_ADDRESS||highest<=lowest||!executable_entry)return false;
    UINTN pages=(highest-lowest+PAGE_SIZE-1)/PAGE_SIZE;
    if(EFI_ERROR(allocate_fixed(bs,lowest,pages)))return false;
    for(uint16_t i=0;i<elf->phnum;++i){const elf32_program_header *ph=(const elf32_program_header *)(payload+elf->phoff+(UINTN)i*elf->phentsize);
        if(ph->type!=1)continue;
        bytes_zero((void *)(UINTN)ph->paddr,ph->memsz);
        bytes_copy((void *)(UINTN)ph->paddr,payload+ph->offset,ph->filesz);}
    *entry=elf->entry;*image_size=highest-lowest;bytes_copy(build_id,nki->build_id,16);return true;
}

static uint8_t *append_tlv(uint8_t *at,uint16_t type,uint16_t flags,uint32_t length)
{nova_bib_tlv_header_t *header=(nova_bib_tlv_header_t *)at;header->type=type;header->flags=flags;header->length=length;bytes_zero(at+8,length);return at+8;}

static UINTN build_bib(const VOID *map,UINTN map_size,UINTN descriptor_size,uint32_t entry,
                       uint32_t image_size,const uint8_t build_id[16])
{
    uint8_t *base=(uint8_t *)(UINTN)BIB_ADDRESS;bytes_zero(base,0x800);
    nova_bib_header_t *header=(nova_bib_header_t *)base;
    const uint8_t magic[8]={'N','B','H','P','B','I','B',0};bytes_copy(header->magic,magic,8);
    header->version_major=1;header->version_minor=2;header->header_size=32;
    header->architecture=NOVA_BOOT_ARCH_X86_32;
    uint8_t *at=base+32,*value;
    value=append_tlv(at,NOVA_BIB_TLV_FIRMWARE,NOVA_BIB_TLV_FLAG_REQUIRED,16);((uint32_t *)value)[0]=UEFI_PLATFORM;at=value+16;
    value=append_tlv(at,NOVA_BIB_TLV_MEMORY,NOVA_BIB_TLV_FLAG_REQUIRED,16);nova_bib_memory_t *memory=(nova_bib_memory_t *)value;
    memory->map_address=MEMORY_MAP_ADDRESS;memory->entry_size=MEMORY_ENTRY_SIZE;at=value+16;
    nova_e820_entry *out=(nova_e820_entry *)(UINTN)MEMORY_MAP_ADDRESS;UINTN count=0;
    for(UINTN off=0;off+descriptor_size<=map_size&&count<MEMORY_ENTRY_CAPACITY;off+=descriptor_size){const efi_memory_descriptor *in=(const efi_memory_descriptor *)((const uint8_t *)map+off);
        if(!in->NumberOfPages)continue;
        out[count].Address=in->PhysicalStart;out[count].Length=in->NumberOfPages*PAGE_SIZE;
        out[count].Type=in->Type==EFI_CONVENTIONAL_MEMORY?1u:2u;out[count].Attributes=(uint32_t)in->Attribute;++count;}
    memory->entry_count=(uint32_t)count;
    const nova_graphics_context_t *graphics=nova_graphics_context();
    if(graphics&&graphics->initialized&&graphics->framebuffer_address<=0xffffffffull){value=append_tlv(at,NOVA_BIB_TLV_GRAPHICS,0,32);nova_bib_graphics_t *g=(nova_bib_graphics_t *)value;
        g->framebuffer_address=(uint32_t)graphics->framebuffer_address;g->pitch=graphics->pitch;g->width=graphics->width;g->height=graphics->height;g->bits_per_pixel=graphics->bits_per_pixel;g->pixel_format=1;header->flags|=NOVA_BOOT_FLAG_FRAMEBUFFER;at=value+32;}
    value=append_tlv(at,NOVA_BIB_TLV_KERNEL,NOVA_BIB_TLV_FLAG_REQUIRED,32);nova_bib_kernel_t *kernel=(nova_bib_kernel_t *)value;
    kernel->load_address=KERNEL_ADDRESS;kernel->image_size=image_size;kernel->entry_point=entry;at=value+32;
    value=append_tlv(at,NOVA_BIB_TLV_SECURITY,NOVA_BIB_TLV_FLAG_REQUIRED,16);((uint32_t *)value)[2]=1;at=value+16;
    value=append_tlv(at,NOVA_BIB_TLV_CPU,NOVA_BIB_TLV_FLAG_REQUIRED,32);uint32_t a,b,c,d;__asm__ volatile("cpuid":"=a"(a),"=b"(b),"=c"(c),"=d"(d):"a"(0),"c"(0));
    bytes_copy(value,&b,4);bytes_copy(value+4,&d,4);bytes_copy(value+8,&c,4);((uint32_t *)value)[3]=a;__asm__ volatile("cpuid":"=a"(a),"=b"(b),"=c"(c),"=d"(d):"a"(1),"c"(0));((uint32_t *)value)[4]=d;((uint32_t *)value)[5]=c;at=value+32;
    value=append_tlv(at,NOVA_BIB_TLV_ENTROPY,NOVA_BIB_TLV_FLAG_REQUIRED,32);uint64_t tsc;uint32_t lo,hi;__asm__ volatile("rdtsc":"=a"(lo),"=d"(hi));tsc=((uint64_t)hi<<32)|lo;bytes_copy(value,&tsc,8);tsc^=0x4e6f76614f535545ull;bytes_copy(value+8,&tsc,8);((uint32_t *)value)[4]=1;((uint32_t *)value)[5]=1;((uint32_t *)value)[6]=16;at=value+32;
    value=append_tlv(at,NOVA_BIB_TLV_SYSTEM,NOVA_BIB_TLV_FLAG_REQUIRED,16);((uint32_t *)value)[2]=1;at=value+16;
    value=append_tlv(at,NOVA_BIB_TLV_KERNEL_IDENTITY,0,32);bytes_copy(value,build_id,16);((uint32_t *)value)[5]=NOVA_KERNEL_FORMAT_ELF32;at=value+32;
    header->total_size=(uint32_t)(at-base);header->checksum=crc32_with_zero(base,header->total_size,20,4);return count;
}

EFI_STATUS uefi_boot_kernel(EFI_HANDLE image_handle,EFI_SYSTEM_TABLE *st)
{
    uint8_t *file=0;UINTN size=0;EFI_STATUS status=read_nki(image_handle,st,&file,&size);
    if(EFI_ERROR(status)){nova_debug_string("UEFI:KERNEL-FILE-ERROR\n");return status;}
    if(EFI_ERROR(allocate_fixed(st->BootServices,BIB_ADDRESS,2))||EFI_ERROR(allocate_fixed(st->BootServices,KERNEL_STACK_BASE,16))){nova_debug_string("UEFI:KERNEL-MEMORY-ERROR\n");return 1;}
    uint32_t entry=0,image_size=0;uint8_t build_id[16];
    if(!load_nki_elf32(st->BootServices,file,size,&entry,&image_size,build_id)){nova_debug_string("UEFI:KERNEL-VALIDATION-ERROR\n");return 1;}
    st->BootServices->FreePool(file);nova_debug_string("UEFI:NKI-VALIDATED\n");
    efi_allocate_pool_fn alloc=(efi_allocate_pool_fn)st->BootServices->AllocatePool;efi_get_memory_map_fn getmap=(efi_get_memory_map_fn)st->BootServices->GetMemoryMap;
    UINTN map_capacity=32768,map_size=map_capacity,map_key=0,descriptor_size=0;uint32_t descriptor_version=0;VOID *map=0;
    status=alloc(EFI_LOADER_DATA,map_capacity,&map);if(EFI_ERROR(status))return status;
    map_size=map_capacity;status=getmap(&map_size,map,&map_key,&descriptor_size,&descriptor_version);
    if(EFI_ERROR(status)||!descriptor_size){nova_debug_string("UEFI:MEMORY-MAP-ERROR\n");return status;}
    if(!build_bib(map,map_size,descriptor_size,entry,image_size,build_id)){nova_debug_string("UEFI:BIB-ERROR\n");return 1;}
    nova_debug_string("UEFI:NBHP-BIB-READY\n");
    efi_exit_boot_services_fn exit_bs=(efi_exit_boot_services_fn)st->BootServices->ExitBootServices;
    status=exit_bs(image_handle,map_key);if(EFI_ERROR(status)){map_size=map_capacity;status=getmap(&map_size,map,&map_key,&descriptor_size,&descriptor_version);if(!EFI_ERROR(status))status=exit_bs(image_handle,map_key);}
    if(EFI_ERROR(status)){nova_debug_string("UEFI:EXIT-BOOT-SERVICES-ERROR\n");return status;}
    nova_debug_string("UEFI:EXIT-BOOT-SERVICES-READY\n");nova_debug_string("UEFI:KERNEL-HANDOFF-READY\n");
    uefi_enter_kernel32(entry,(uint32_t)BIB_ADDRESS);for(;;)__asm__ volatile("hlt");
}
