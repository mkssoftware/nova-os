#include "pointer.h"
#include "../bootmenu/input.h"

#define NOVA_UEFI_POINTER_CAPACITY 8u
static EFI_SIMPLE_POINTER_PROTOCOL *pointers[NOVA_UEFI_POINTER_CAPACITY];
static EFI_ABSOLUTE_POINTER_PROTOCOL *absolute_pointers[NOVA_UEFI_POINTER_CAPACITY];
static uint8_t pointer_count,absolute_pointer_count;
typedef EFI_STATUS (EFIAPI *check_event_fn)(EFI_EVENT event);
static check_event_fn check_event;
static EFI_GUID pointer_guid = {
    0x31878c87, 0x0b75, 0x11d5,
    {0x9a,0x4f,0x00,0x90,0x27,0x3f,0xc1,0x4d}
};
static EFI_GUID absolute_pointer_guid = {
    0x8d59d32b,0xc655,0x4ae9,{0x9b,0x15,0xf2,0x59,0x04,0x99,0x2a,0x43}
};
static bool pointer_scan(void *context)
{(void)uefi_pointer_refresh((EFI_SYSTEM_TABLE *)context);return true;}

bool uefi_pointer_initialize(EFI_SYSTEM_TABLE *system_table)
{
    pointer_count=absolute_pointer_count=0;
    check_event=(system_table&&system_table->BootServices)?
        (check_event_fn)system_table->BootServices->CheckEvent:0;
    if(!nova_input_device_scanner_set(pointer_scan,system_table))return false;
    if(!uefi_pointer_refresh(system_table)){
        pointer_count=absolute_pointer_count=0;
        nova_debug_string("UEFI:POINTER-UNAVAILABLE\n");
        return false;
    }
    nova_debug_string("UEFI:POINTER-READY\n");
    return true;
}

bool uefi_pointer_refresh(EFI_SYSTEM_TABLE *system_table)
{
    if(!system_table||!system_table->BootServices||
       !system_table->BootServices->LocateProtocol)return false;
    EFI_SIMPLE_POINTER_PROTOCOL *next_relative[NOVA_UEFI_POINTER_CAPACITY]={0};
    EFI_ABSOLUTE_POINTER_PROTOCOL *next_absolute[NOVA_UEFI_POINTER_CAPACITY]={0};
    uint8_t next_relative_count=0,next_absolute_count=0;
    typedef EFI_STATUS (EFIAPI *locate_handles_fn)(uint32_t,EFI_GUID *,VOID *,UINTN *,EFI_HANDLE **);
    typedef EFI_STATUS (EFIAPI *handle_protocol_fn)(EFI_HANDLE,EFI_GUID *,VOID **);
    locate_handles_fn locate_handles=(locate_handles_fn)system_table->BootServices->LocateHandleBuffer;
    handle_protocol_fn handle_protocol=(handle_protocol_fn)system_table->BootServices->HandleProtocol;
    if(locate_handles&&handle_protocol){
        EFI_GUID *guids[2]={&pointer_guid,&absolute_pointer_guid};
        for(uint8_t kind=0;kind<2;++kind){UINTN count=0;EFI_HANDLE *handles=0;
            EFI_STATUS found=locate_handles(2,guids[kind],0,&count,&handles);
            if(!EFI_ERROR(found)&&handles){
                for(UINTN i=0;i<count&&i<NOVA_UEFI_POINTER_CAPACITY;++i){VOID *protocol=0;
                    if(EFI_ERROR(handle_protocol(handles[i],guids[kind],&protocol))||!protocol)continue;
                    if(!kind){EFI_SIMPLE_POINTER_PROTOCOL *candidate=(EFI_SIMPLE_POINTER_PROTOCOL *)protocol;
                        if(candidate->GetState)next_relative[next_relative_count++]=candidate;}
                    else{EFI_ABSOLUTE_POINTER_PROTOCOL *candidate=(EFI_ABSOLUTE_POINTER_PROTOCOL *)protocol;
                        if(candidate->GetState&&candidate->Mode&&
                           candidate->Mode->AbsoluteMaxX>candidate->Mode->AbsoluteMinX&&
                           candidate->Mode->AbsoluteMaxY>candidate->Mode->AbsoluteMinY)
                            next_absolute[next_absolute_count++]=candidate;}
                }
                if(system_table->BootServices->FreePool)
                    (void)system_table->BootServices->FreePool(handles);
            }
        }
    }else{
        EFI_SIMPLE_POINTER_PROTOCOL *relative=0;
        if(!EFI_ERROR(system_table->BootServices->LocateProtocol(&pointer_guid,0,(VOID **)&relative))&&
           relative&&relative->GetState)next_relative[next_relative_count++]=relative;
        EFI_ABSOLUTE_POINTER_PROTOCOL *absolute=0;
        if(!EFI_ERROR(system_table->BootServices->LocateProtocol(&absolute_pointer_guid,0,(VOID **)&absolute))&&
           absolute&&absolute->GetState&&absolute->Mode&&
           absolute->Mode->AbsoluteMaxX>absolute->Mode->AbsoluteMinX&&
           absolute->Mode->AbsoluteMaxY>absolute->Mode->AbsoluteMinY)
            next_absolute[next_absolute_count++]=absolute;
    }
    bool ready=next_relative_count||next_absolute_count;
    if(!ready){
        if(pointer_count||absolute_pointer_count){pointer_count=absolute_pointer_count=0;
            (void)nova_input_device_set(2,NOVA_DEVICE_MOUSE,false);
            nova_debug_string("UEFI:POINTER-REMOVED\n");}
        return false;
    }
    bool changed=pointer_count!=next_relative_count||absolute_pointer_count!=next_absolute_count;
    for(uint8_t i=0;!changed&&i<pointer_count;++i)changed=pointers[i]!=next_relative[i];
    for(uint8_t i=0;!changed&&i<absolute_pointer_count;++i)
        changed=absolute_pointers[i]!=next_absolute[i];
    if(changed){pointer_count=next_relative_count;absolute_pointer_count=next_absolute_count;
        for(uint8_t i=0;i<pointer_count;++i){pointers[i]=next_relative[i];
            if(pointers[i]->Reset)(void)pointers[i]->Reset(pointers[i],false);}
        for(uint8_t i=0;i<absolute_pointer_count;++i){absolute_pointers[i]=next_absolute[i];
            if(absolute_pointers[i]->Reset)
                (void)absolute_pointers[i]->Reset(absolute_pointers[i],false);}
        (void)nova_input_device_set(2,NOVA_DEVICE_MOUSE,true);
        nova_debug_string("UEFI:POINTER-ADDED\n");}
    return true;
}

bool uefi_pointer_available(void){return pointer_count||absolute_pointer_count;}

static int32_t normalize_absolute(uint64_t value,uint64_t minimum,uint64_t maximum,
                                  uint32_t extent)
{
    if(maximum<=minimum||!extent)return 0;
    if(value<minimum)value=minimum;
    if(value>maximum)value=maximum;
    return (int32_t)(((value-minimum)*(extent-1u))/(maximum-minimum));
}

static bool pointer_input_ready(EFI_EVENT wait_event)
{
    /* UEFI pointer drivers signal WaitForInput once a state change is ready.
       Checking it first keeps polling non-blocking and, importantly, lets
       drivers that advance their state through the event contract expose the
       next packet.  Minimal firmware mocks without CheckEvent retain the
       direct GetState fallback. */
    return !check_event||!wait_event||!EFI_ERROR(check_event(wait_event));
}

bool uefi_pointer_poll(uint32_t viewport_width,uint32_t viewport_height,
                       uefi_pointer_event_t *event)
{
    if(!event||!viewport_width||!viewport_height)return false;
    *event=(uefi_pointer_event_t){0};
    for(uint8_t i=0;i<absolute_pointer_count;++i){
        EFI_ABSOLUTE_POINTER_PROTOCOL *absolute_pointer=absolute_pointers[i];
        if(!pointer_input_ready(absolute_pointer->WaitForInput))continue;
        EFI_ABSOLUTE_POINTER_STATE state;
        EFI_STATUS status=absolute_pointer->GetState(absolute_pointer,&state);
        if(!EFI_ERROR(status)){
            EFI_ABSOLUTE_POINTER_MODE *mode=absolute_pointer->Mode;
            event->x=normalize_absolute(state.CurrentX,mode->AbsoluteMinX,
                                         mode->AbsoluteMaxX,viewport_width);
            event->y=normalize_absolute(state.CurrentY,mode->AbsoluteMinY,
                                         mode->AbsoluteMaxY,viewport_height);
            event->left=(state.ActiveButtons&1u)!=0;
            event->right=(state.ActiveButtons&2u)!=0;event->absolute=true;
            nova_debug_string("UEFI:ABSOLUTE-POINTER-EVENT\n");return true;
        }
    }
    for(uint8_t i=0;i<pointer_count;++i){EFI_SIMPLE_POINTER_PROTOCOL *pointer=pointers[i];
        if(!pointer_input_ready(pointer->WaitForInput))continue;
        EFI_SIMPLE_POINTER_STATE state;
        EFI_STATUS status = pointer->GetState(pointer, &state);
        if (EFI_ERROR(status)) continue;
        event->dx=state.RelativeMovementX;event->dy=state.RelativeMovementY;
        event->wheel=state.RelativeMovementZ;event->left=state.LeftButton;
        event->right=state.RightButton;
        nova_debug_string("UEFI:RELATIVE-POINTER-EVENT\n");return true;
    }
    /* EFI_NOT_READY kennzeichnet Stillstand; EFI_SUCCESS ist auch beim Loslassen
       einer Taste ein echtes Ereignis mit komplett leerem Zustand. */
    return false;
}
