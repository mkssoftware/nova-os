#include <stdio.h>
#include <stdbool.h>
#include "../boot/bootloader/uefi/pointer.h"
#include "../boot/bootloader/bootmenu/input.h"

#define EFI_NOT_FOUND 0x800000000000000eull

static bool locate_available,absolute_available,device_connected;
static uint32_t locate_calls,reset_calls,device_updates;
static nova_input_device_scanner_t scanner;
static void *scanner_context;

bool nova_input_device_scanner_set(nova_input_device_scanner_t value,void *context)
{scanner=value;scanner_context=context;return value!=0;}
bool nova_input_device_set(uint32_t id,nova_device_type_t type,bool connected)
{if(id!=2||type!=NOVA_DEVICE_MOUSE)return false;
 device_connected=connected;++device_updates;return true;}

static EFI_STATUS EFIAPI mock_reset(EFI_SIMPLE_POINTER_PROTOCOL *self,BOOLEAN verify)
{(void)self;(void)verify;++reset_calls;return EFI_SUCCESS;}
static EFI_STATUS EFIAPI mock_get_state(EFI_SIMPLE_POINTER_PROTOCOL *self,
 EFI_SIMPLE_POINTER_STATE *state)
{(void)self;*state=(EFI_SIMPLE_POINTER_STATE){4,-3,1,true,false};return EFI_SUCCESS;}
static EFI_SIMPLE_POINTER_PROTOCOL mock_pointer={mock_reset,mock_get_state,0,0};
static EFI_STATUS EFIAPI mock_absolute_reset(EFI_ABSOLUTE_POINTER_PROTOCOL *self,BOOLEAN verify)
{(void)self;(void)verify;++reset_calls;return EFI_SUCCESS;}
static EFI_STATUS EFIAPI mock_absolute_get_state(EFI_ABSOLUTE_POINTER_PROTOCOL *self,
 EFI_ABSOLUTE_POINTER_STATE *state)
{(void)self;*state=(EFI_ABSOLUTE_POINTER_STATE){750,250,0,3};return EFI_SUCCESS;}
static EFI_ABSOLUTE_POINTER_MODE absolute_mode={0,0,0,1000,1000,0,0};
static EFI_ABSOLUTE_POINTER_PROTOCOL mock_absolute={mock_absolute_reset,
 mock_absolute_get_state,0,&absolute_mode};
static EFI_STATUS EFIAPI mock_locate(EFI_GUID *guid,VOID *registration,VOID **result)
{(void)registration;++locate_calls;
 if(guid->Data1==0x8d59d32b){if(absolute_available){*result=&mock_absolute;return EFI_SUCCESS;}
    *result=0;return EFI_NOT_FOUND;}
 if(!locate_available){*result=0;return EFI_NOT_FOUND;}
 *result=&mock_pointer;return EFI_SUCCESS;}

int main(void)
{
 EFI_BOOT_SERVICES boot={0};boot.LocateProtocol=mock_locate;
 EFI_SYSTEM_TABLE system={0};system.BootServices=&boot;
 locate_available=true;
 if(!uefi_pointer_initialize(&system)||!uefi_pointer_available()||
    !scanner||scanner_context!=&system||!device_connected||device_updates!=1||
    reset_calls!=1)return 1;
 if(!scanner(scanner_context)||device_updates!=1||locate_calls!=4)return 1;
 uefi_pointer_event_t event;
 if(!uefi_pointer_poll(1280,800,&event)||event.absolute||event.dx!=4||event.dy!=-3||
    event.wheel!=1||!event.left||event.right)return 1;
 absolute_available=true;
 if(!scanner(scanner_context)||device_updates!=2||reset_calls!=3||
    !uefi_pointer_poll(1280,800,&event)||!event.absolute||event.x!=959||event.y!=199||
    !event.left||!event.right)return 1;
 absolute_available=false;
 locate_available=false;
 if(!scanner(scanner_context)||uefi_pointer_available()||device_connected||
    device_updates!=3)return 1;
 if(!scanner(scanner_context)||device_updates!=3)return 1;
 locate_available=true;
 if(!scanner(scanner_context)||!uefi_pointer_available()||!device_connected||
    device_updates!=4||reset_calls!=4)return 1;
 puts("UEFI_POINTER_RUNTIME_OK");return 0;
}
