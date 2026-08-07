#include "firmware.h"

#define EFI_VARIABLE_NON_VOLATILE 1u
#define EFI_VARIABLE_BOOTSERVICE_ACCESS 2u
#define EFI_VARIABLE_RUNTIME_ACCESS 4u
#define EFI_OS_INDICATIONS_BOOT_TO_FW_UI 1ull

static EFI_SYSTEM_TABLE *system_table;
static EFI_RUNTIME_SERVICES *runtime;
static nova_uefi_firmware_status_t status;
static EFI_GUID global_variable_guid={0x8be4df61u,0x93cau,0x11d2u,
    {0xaau,0x0du,0x00u,0xe0u,0x98u,0x03u,0x2bu,0x8cu}};
static CHAR16 secure_boot_name[]={ 'S','e','c','u','r','e','B','o','o','t',0 };
static CHAR16 setup_mode_name[]={ 'S','e','t','u','p','M','o','d','e',0 };
static CHAR16 indications_supported_name[]={ 'O','s','I','n','d','i','c','a','t','i','o','n','s','S','u','p','p','o','r','t','e','d',0 };
static CHAR16 indications_name[]={ 'O','s','I','n','d','i','c','a','t','i','o','n','s',0 };

static bool read_variable(CHAR16 *name,VOID *value,UINTN expected)
{
    if(!runtime||!runtime->GetVariable)return false;
    UINTN size=expected;uint32_t attributes=0;
    return !EFI_ERROR(runtime->GetVariable(name,&global_variable_guid,&attributes,&size,value))&&
           size==expected;
}

static void read_vendor(void)
{
    uint32_t out=0;
    if(system_table&&system_table->FirmwareVendor)for(uint32_t i=0;i<39u;++i){
        uint16_t character=system_table->FirmwareVendor[i];
        if(!character)break;
        status.vendor[out++]=character>=32u&&character<=126u?(char)character:'?';
    }
    if(!out){static const char fallback[]="Unbekannt";
        for(uint32_t i=0;fallback[i]&&i<39u;++i)status.vendor[out++]=fallback[i];}
    status.vendor[out]=0;
}

bool uefi_firmware_refresh(void)
{
    if(!system_table||!runtime)return false;
    uint8_t value=0;uint64_t supported=0;
    status.secure_boot_known=read_variable(secure_boot_name,&value,sizeof(value));
    status.secure_boot=status.secure_boot_known&&value==1u;
    value=0;
    status.setup_mode_known=read_variable(setup_mode_name,&value,sizeof(value));
    status.setup_mode=status.setup_mode_known&&value==1u;
    status.firmware_setup_supported=read_variable(indications_supported_name,&supported,
                                                   sizeof(supported))&&
                                    (supported&EFI_OS_INDICATIONS_BOOT_TO_FW_UI)!=0;
    nova_debug_string(status.secure_boot?"UEFI:FIRMWARE-SECURE-BOOT-ON\n":
                                         "UEFI:FIRMWARE-SECURE-BOOT-OFF\n");
    nova_debug_string(status.firmware_setup_supported?"UEFI:FIRMWARE-SETUP-SUPPORTED\n":
                                                       "UEFI:FIRMWARE-SETUP-UNAVAILABLE\n");
    return true;
}

bool uefi_firmware_initialize(EFI_SYSTEM_TABLE *table)
{
    system_table=table;runtime=table?table->RuntimeServices:0;
    status=(nova_uefi_firmware_status_t){0};
    if(!runtime){nova_debug_string("UEFI:FIRMWARE-UNAVAILABLE\n");return false;}
    status.uefi=true;status.revision=table->FirmwareRevision;read_vendor();
    uefi_firmware_refresh();nova_debug_string("UEFI:FIRMWARE-READY\n");return true;
}

const nova_uefi_firmware_status_t *uefi_firmware_status(void){return &status;}

bool uefi_firmware_request_setup(void)
{
    if(!status.firmware_setup_supported||!runtime||!runtime->SetVariable||
       !runtime->ResetSystem)return false;
    uint64_t indications=0;
    (void)read_variable(indications_name,&indications,sizeof(indications));
    indications|=EFI_OS_INDICATIONS_BOOT_TO_FW_UI;
    EFI_STATUS result=runtime->SetVariable(indications_name,&global_variable_guid,
        EFI_VARIABLE_NON_VOLATILE|EFI_VARIABLE_BOOTSERVICE_ACCESS|
        EFI_VARIABLE_RUNTIME_ACCESS,sizeof(indications),&indications);
    if(EFI_ERROR(result)){nova_debug_string("UEFI:FIRMWARE-SETUP-SET-FAILED\n");return false;}
    nova_debug_string("UEFI:FIRMWARE-SETUP-REQUESTED\n");
    runtime->ResetSystem(EfiResetCold,EFI_SUCCESS,0,0);
    nova_debug_string("UEFI:FIRMWARE-SETUP-RESET-FAILED\n");return false;
}
