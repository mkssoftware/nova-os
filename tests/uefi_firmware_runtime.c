#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "../boot/bootloader/uefi/firmware.h"

#define EFI_NOT_FOUND (0x800000000000000eull)
static bool capability=true,set_called,reset_called;
static uint64_t written_indications;

static bool name_is(const CHAR16 *name,const char *ascii)
{
    while(*ascii&&*name==(uint16_t)(uint8_t)*ascii){++name;++ascii;}
    return !*ascii&&!*name;
}

static EFI_STATUS EFIAPI mock_get(CHAR16 *name,EFI_GUID *guid,uint32_t *attributes,
                                  UINTN *size,VOID *value)
{
    (void)guid;(void)attributes;
    if(name_is(name,"SecureBoot")&&*size==1){*(uint8_t *)value=0;return EFI_SUCCESS;}
    if(name_is(name,"SetupMode")&&*size==1){*(uint8_t *)value=1;return EFI_SUCCESS;}
    if(name_is(name,"OsIndicationsSupported")&&*size==8&&capability){
        *(uint64_t *)value=1;return EFI_SUCCESS;}
    if(name_is(name,"OsIndications")&&*size==8){*(uint64_t *)value=0;return EFI_SUCCESS;}
    return EFI_NOT_FOUND;
}

static EFI_STATUS EFIAPI mock_set(CHAR16 *name,EFI_GUID *guid,uint32_t attributes,
                                  UINTN size,VOID *value)
{
    (void)guid;
    if(!name_is(name,"OsIndications")||attributes!=7u||size!=8)return EFI_NOT_FOUND;
    set_called=true;written_indications=*(uint64_t *)value;return EFI_SUCCESS;
}

static void EFIAPI mock_reset(EFI_RESET_TYPE type,EFI_STATUS status,UINTN size,VOID *data)
{
    (void)status;(void)size;(void)data;if(type==EfiResetCold)reset_called=true;
}

static int check(bool condition,const char *message)
{
    if(condition)return 0;
    fprintf(stderr,"FAIL: %s\n",message);
    return 1;
}

int main(void)
{
    EFI_RUNTIME_SERVICES runtime={0};runtime.GetVariable=mock_get;
    runtime.SetVariable=mock_set;runtime.ResetSystem=mock_reset;
    CHAR16 vendor[]={'E','D','K',' ','I','I',0};
    EFI_SYSTEM_TABLE table={0};table.FirmwareVendor=vendor;table.FirmwareRevision=0x20064;
    table.RuntimeServices=&runtime;
    int failed=0;
    failed|=check(uefi_firmware_initialize(&table),"Firmware initialisieren");
    const nova_uefi_firmware_status_t *status=uefi_firmware_status();
    failed|=check(status->uefi&&!strcmp(status->vendor,"EDK II")&&
                  status->secure_boot_known&&!status->secure_boot&&
                  status->setup_mode_known&&status->setup_mode&&
                  status->firmware_setup_supported,"Firmwarestatus und Capability lesen");
    (void)uefi_firmware_request_setup();
    failed|=check(set_called&&reset_called&&written_indications==1,
                  "BootToFwUi setzen und kalt neu starten");
    capability=false;set_called=reset_called=false;written_indications=0;
    failed|=check(uefi_firmware_refresh()&&!uefi_firmware_status()->firmware_setup_supported,
                  "fehlende Setup-Capability erkennen");
    failed|=check(!uefi_firmware_request_setup()&&!set_called&&!reset_called,
                  "nicht unterstützten Setupwechsel sicher sperren");
    if(!failed)puts("UEFI_FIRMWARE_RUNTIME_OK");
    return failed?1:0;
}
