#include "kernel_loader.h"
#include "../bootmenu/graphics.h"
#include "firmware.h"
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

static void bytes_zero(void *target,UINTN length){uint8_t *p=target;while(length--)*p++=0;}
static void bytes_copy(void *target,const void *source,UINTN length){uint8_t *d=target;const uint8_t *s=source;while(length--)*d++=*s++;}
static bool bytes_equal(const void *left,const void *right,UINTN length)
{const uint8_t *a=left,*b=right;while(length--)if(*a++!=*b++)return false;return true;}
static uint32_t crc32_with_zero(const uint8_t *data,UINTN size,UINTN zero_offset,UINTN zero_size)
{uint32_t crc=0xffffffffu;for(UINTN i=0;i<size;++i){uint8_t value=(i>=zero_offset&&i<zero_offset+zero_size)?0:data[i];crc^=value;for(uint32_t bit=0;bit<8;++bit)crc=(crc>>1)^((crc&1)?0xedb88320u:0);}return ~crc;}
static bool range_valid(UINTN offset,UINTN length,UINTN total){return offset<=total&&length<=total-offset;}

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
    if(EFI_ERROR(status)||!end||end>1024u*1024u){file->Close(file);return 1;}
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

static bool load_nki_elf32(EFI_BOOT_SERVICES *bs,const uint8_t *file,UINTN size,
                           uint32_t *entry,uint32_t *image_size,uint8_t build_id[20])
{
    if(size<sizeof(nova_nki_header_t))return false;
    const nova_nki_header_t *nki=(const nova_nki_header_t *)file;
    static const uint8_t magic[8]={'N','O','V','A','N','K','I',0};
    for(UINTN i=0;i<8;++i)if(nki->magic[i]!=magic[i])return false;
    if(nki->version!=1||nki->header_size!=64||nki->architecture!=NOVA_BOOT_ARCH_X86_32||
       (nki->flags&3u)!=3u||nki->compression||nki->reserved||!range_valid(64,nki->image_size,size))return false;
    const uint8_t *payload=file+64;
    if(crc32_with_zero(payload,nki->image_size,nki->image_size,0)!=nki->payload_crc32)return false;
    uint8_t elf_build_id[20];
    if(!load_elf32(bs,payload,nki->image_size,entry,image_size,elf_build_id))return false;
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
    security->flags=NOVA_BOOT_SECURITY_ELF_BUILD_ID_VALID|(nki_container?NOVA_BOOT_SECURITY_NKI_CRC32_VALID:0u);
    security->secure_boot_state=current_secure_boot_state(&security->flags);
    security->entropy_quality=1;at=value+16;
    value=append_tlv(at,NOVA_BIB_TLV_BOOT_OPTIONS,NOVA_BIB_TLV_FLAG_REQUIRED,16);
    nova_bib_boot_options_t *options=(nova_bib_boot_options_t *)value;
    options->boot_mode=boot_mode;options->flags=boot_options_flags;
    options->selected_generation=selected_generation;options->fallback_level=fallback_level;at=value+16;
    value=append_tlv(at,NOVA_BIB_TLV_CPU,NOVA_BIB_TLV_FLAG_REQUIRED,32);uint32_t a,b,c,d;__asm__ volatile("cpuid":"=a"(a),"=b"(b),"=c"(c),"=d"(d):"a"(0),"c"(0));
    bytes_copy(value,&b,4);bytes_copy(value+4,&d,4);bytes_copy(value+8,&c,4);((uint32_t *)value)[3]=a;__asm__ volatile("cpuid":"=a"(a),"=b"(b),"=c"(c),"=d"(d):"a"(1),"c"(0));((uint32_t *)value)[4]=d;((uint32_t *)value)[5]=c;at=value+32;
    value=append_tlv(at,NOVA_BIB_TLV_ENTROPY,NOVA_BIB_TLV_FLAG_REQUIRED,32);uint64_t tsc;uint32_t lo,hi;__asm__ volatile("rdtsc":"=a"(lo),"=d"(hi));tsc=((uint64_t)hi<<32)|lo;bytes_copy(value,&tsc,8);tsc^=0x4e6f76614f535545ull;bytes_copy(value+8,&tsc,8);((uint32_t *)value)[4]=1;((uint32_t *)value)[5]=1;((uint32_t *)value)[6]=16;at=value+32;
    value=append_tlv(at,NOVA_BIB_TLV_SYSTEM,NOVA_BIB_TLV_FLAG_REQUIRED,16);((uint32_t *)value)[2]=1;at=value+16;
    uint32_t rsdp=find_acpi_rsdp(st);
    if(rsdp){value=append_tlv(at,NOVA_BIB_TLV_ACPI,0,16);((nova_bib_pointer_info_t *)value)->address=rsdp;at=value+16;}
    value=append_tlv(at,NOVA_BIB_TLV_KERNEL_IDENTITY,0,32);bytes_copy(value,build_id,20);((uint32_t *)value)[5]=kernel_format;((uint32_t *)value)[6]=nki_container?1u:0u;at=value+32;
    header->total_size=(uint32_t)(at-base);header->checksum=crc32_with_zero(base,header->total_size,20,4);return count;
}

static EFI_STATUS boot_kernel(EFI_HANDLE image_handle,EFI_SYSTEM_TABLE *st,bool recovery_only)
{
    (void)uefi_firmware_refresh();
    uint8_t *file=0;UINTN size=0;bool nki_container=true;
    bool recovery_mode=recovery_only,automatic_recovery=false,automatic_rollback=false;
    uint32_t selected_generation=recovery_only?NOVA_BOOT_GENERATION_RECOVERY:NOVA_BOOT_GENERATION_PRIMARY;
    uint32_t fallback_level=0;
    uint32_t architecture=NOVA_BOOT_ARCH_X86_32,kernel_format=NOVA_KERNEL_FORMAT_ELF32;
    EFI_STATUS status=read_kernel_file(image_handle,st,recovery_only?recovery_nki_path:nki_path,&file,&size);
    if(!recovery_only&&EFI_ERROR(status)){
        nki_container=false;file=0;size=0;
        status=read_kernel_file(image_handle,st,elf_path,&file,&size);
        if(EFI_ERROR(status)){
            file=0;size=0;architecture=NOVA_BOOT_ARCH_X86_64;kernel_format=NOVA_KERNEL_FORMAT_ELF64;
            status=read_kernel_file(image_handle,st,elf64_path,&file,&size);
        }
    }
    uint32_t entry=0,load_address=KERNEL_ADDRESS,image_size=0;uint8_t build_id[20];bytes_zero(build_id,sizeof(build_id));
    bool loaded=false;
    if(!EFI_ERROR(status)){
        if(nki_container)loaded=load_nki_elf32(st->BootServices,file,size,&entry,&image_size,build_id);
        else if(kernel_format==NOVA_KERNEL_FORMAT_ELF32)loaded=load_elf32(st->BootServices,file,size,&entry,&image_size,build_id);
        else loaded=load_elf64(st->BootServices,file,size,&entry,&load_address,&image_size,build_id);
    }
    if(!loaded&&recovery_only){
        if(file)st->BootServices->FreePool(file);
        nova_debug_string(EFI_ERROR(status)?"UEFI:RECOVERY-KERNEL-FILE-ERROR\n":"UEFI:KERNEL-VALIDATION-ERROR\n");return 1;
    }
    if(!loaded){
        if(file)st->BootServices->FreePool(file);
        file=0;size=0;
        nova_debug_string(EFI_ERROR(status)?"UEFI:PRIMARY-KERNEL-FILE-ERROR\n":"UEFI:PRIMARY-KERNEL-VALIDATION-ERROR\n");
        nki_container=true;architecture=NOVA_BOOT_ARCH_X86_32;kernel_format=NOVA_KERNEL_FORMAT_ELF32;
        entry=0;load_address=KERNEL_ADDRESS;image_size=0;bytes_zero(build_id,sizeof(build_id));
        status=read_kernel_file(image_handle,st,backup_nki_path,&file,&size);
        if(!EFI_ERROR(status))loaded=load_nki_elf32(st->BootServices,file,size,&entry,&image_size,build_id);
        if(loaded){automatic_rollback=true;selected_generation=NOVA_BOOT_GENERATION_BACKUP;fallback_level=1;}
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
        if(!EFI_ERROR(status))loaded=load_nki_elf32(st->BootServices,file,size,&entry,&image_size,build_id);
    }
    if(!loaded){if(file)st->BootServices->FreePool(file);nova_debug_string("UEFI:KERNEL-VALIDATION-ERROR\n");return 1;}
    if(EFI_ERROR(allocate_fixed(st->BootServices,BIB_ADDRESS,2))){nova_debug_string("UEFI:BIB-MEMORY-ERROR\n");return 1;}
    uint32_t verification_state=nki_container?NOVA_BOOT_VERIFICATION_INTEGRITY_VERIFIED:
                                              NOVA_BOOT_VERIFICATION_STRUCTURE_VALIDATED;
    uint32_t stack_top=0;
    if(!allocate_kernel_stack(st->BootServices,&stack_top)){nova_debug_string("UEFI:KERNEL-STACK-MEMORY-ERROR\n");return 1;}
    st->BootServices->FreePool(file);
    if(automatic_rollback){
        nova_debug_string("UEFI:AUTOMATIC-BACKUP-SELECTED\n");
        nova_debug_string("UEFI:BACKUP-NKI-VALIDATED\n");
    }else if(recovery_mode){
        nova_debug_string(automatic_recovery?"UEFI:AUTOMATIC-RECOVERY-SELECTED\n":"UEFI:MANUAL-RECOVERY-SELECTED\n");
        nova_debug_string("UEFI:RECOVERY-NKI-VALIDATED\n");
    }else if(nki_container)nova_debug_string("UEFI:NKI-VALIDATED\n");
    else nova_debug_string(kernel_format==NOVA_KERNEL_FORMAT_ELF32?"UEFI:ELF32-DIRECT-VALIDATED\n":"UEFI:ELF64-DIRECT-VALIDATED\n");
    nova_debug_string(nki_container?"UEFI:KERNEL-INTEGRITY-VERIFIED\n":"UEFI:KERNEL-STRUCTURE-VALIDATED\n");
    nova_debug_string("UEFI:KERNEL-SIGNATURE-NOT-PRESENT\n");
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
        status=exit_bs(image_handle,map_key);
        if(!EFI_ERROR(status))break;
        nova_debug_string("UEFI:EXIT-BOOT-SERVICES-RETRY\n");
    }
    if(EFI_ERROR(status)){nova_debug_string("UEFI:EXIT-BOOT-SERVICES-ERROR\n");return status;}
    nova_debug_string("UEFI:EXIT-BOOT-SERVICES-READY\n");nova_debug_string("UEFI:KERNEL-HANDOFF-READY\n");
    if(kernel_format==NOVA_KERNEL_FORMAT_ELF64)uefi_enter_kernel64(entry,(uint32_t)BIB_ADDRESS,stack_top);
    else uefi_enter_kernel32(entry,(uint32_t)BIB_ADDRESS,stack_top);
    for(;;)__asm__ volatile("hlt");
}

EFI_STATUS uefi_boot_kernel(EFI_HANDLE image_handle,EFI_SYSTEM_TABLE *st)
{return boot_kernel(image_handle,st,false);}

EFI_STATUS uefi_boot_recovery_kernel(EFI_HANDLE image_handle,EFI_SYSTEM_TABLE *st)
{return boot_kernel(image_handle,st,true);}
