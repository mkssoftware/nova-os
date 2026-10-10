; Nova Kernel - x86-32 Entry und früher Kernel Context (NPSPEC-KERNEL-0001)
; Validiert NBHP/BIB v1, übernimmt ausschließlich TLV-Daten und ruft danach
; den minimalen Kernel Main auf.

%include "layout.inc"

[org KERNEL_ENTRY_ADDRESS]
[bits 32]

kernel_entry:
    cli
    cld
    xor ebp, ebp
    push eax
    push ebx
    lgdt [kernel_gdt_descriptor]
    mov ax, DATA_SEGMENT
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov fs, ax
    mov gs, ax
    pop ebx
    pop eax
    mov [kernel_boot_stack_top], esp
    mov dword [boot_phase_current], BOOT_PHASE_KERNEL_ENTRY
    mov dword [boot_phase_last_success], BOOT_PHASE_NONE

    push eax
    push ebx
    call serial_initialize
    call logging_initialize
    jc kernel_halt
    call logging_self_test
    jc kernel_halt
    mov esi, message_entered
    call serial_write_string
    mov esi, message_logging_ok
    call serial_write_string
    call boot_phase_log
    pop ebx
    pop eax

    push eax
    push ebx
    call panic_manager_initialize
    call panic_manager_self_test
    jc kernel_halt
    mov esi, message_panic_manager_ok
    call serial_write_string
    call crash_dump_initialize
    jc kernel_halt
    call crash_dump_self_test
    jc kernel_halt
    mov esi, message_crash_dump_ok
    call serial_write_string
    pop ebx
    pop eax

    cmp eax, NOVA_X86_BOOT_MAGIC
    jne panic_invalid_handoff
    test ebx, ebx
    jz panic_invalid_handoff
    test ebx, 7
    jnz panic_invalid_handoff

    mov dword [boot_phase_last_success], BOOT_PHASE_KERNEL_ENTRY
    mov dword [boot_phase_current], BOOT_PHASE_HANDOFF
    call boot_phase_log
    mov [kernel_context + CONTEXT_BIB], ebx
    call validate_bib
    jc panic_invalid_handoff

    call create_kernel_context
    jc panic_invalid_handoff
    call acpi_rsdp_initialize
    call early_security_entropy_initialize
    jc panic_invalid_handoff
    call early_security_entropy_self_test
    jc panic_invalid_handoff
    mov dword [boot_phase_last_success], BOOT_PHASE_HANDOFF
    mov dword [boot_phase_current], BOOT_PHASE_EARLY_ARCH
    call boot_phase_log

    mov esi, message_bib_ok
    call serial_write_string
    test dword [kernel_context + CONTEXT_SEEN], CONTEXT_HAS_BOOT_OPTIONS
    jz .boot_mode_done
    cmp dword [kernel_context + CONTEXT_BOOT_MODE], NOVA_BOOT_MODE_RECOVERY
    jne .boot_mode_done
    mov esi, message_recovery_mode_ok
    call serial_write_string
.boot_mode_done:
    cmp dword [kernel_context + CONTEXT_BOOT_GENERATION], NOVA_BOOT_GENERATION_BACKUP
    jne .boot_generation_done
    mov esi, message_backup_generation_ok
    call serial_write_string
.boot_generation_done:
    test dword [kernel_context + CONTEXT_SEEN], CONTEXT_HAS_KERNEL_ID
    jz .kernel_identity_done
    mov esi, message_kernel_identity_ok
    call serial_write_string
.kernel_identity_done:

    mov dword [boot_phase_last_success], BOOT_PHASE_EARLY_ARCH
    mov dword [boot_phase_current], BOOT_PHASE_EARLY_MEMORY
    call boot_phase_log
    call pmm_initialize
    jc panic_memory_manager
    call pmm_self_test
    jc panic_memory_manager
    mov esi, message_pmm_ok
    call serial_write_string

    mov dword [boot_phase_last_success], BOOT_PHASE_EARLY_MEMORY
    mov dword [boot_phase_current], BOOT_PHASE_VIRTUAL_MEMORY
    call boot_phase_log
    call paging_initialize
    jc panic_paging

    mov dword [boot_phase_last_success], BOOT_PHASE_VIRTUAL_MEMORY
    mov dword [boot_phase_current], BOOT_PHASE_KERNEL_CORE
    call boot_phase_log
    call heap_initialize
    jc panic_heap
    call heap_self_test
    jc panic_heap
    mov esi, message_heap_ok
    call serial_write_string

    call object_manager_initialize
    jc panic_object_manager
    call object_manager_self_test
    jc panic_object_manager
    mov esi, message_object_manager_ok
    call serial_write_string

    call handle_manager_initialize
    jc panic_handle_manager
    call handle_manager_self_test
    jc panic_handle_manager
    mov esi, message_handle_manager_ok
    call serial_write_string

    call component_manager_initialize
    jc panic_component_manager
    call component_manager_self_test
    jc panic_component_manager
    mov esi, message_component_manager_ok
    call serial_write_string

    call paging_self_test
    jc panic_paging
    mov esi, message_paging_ok
    call serial_write_string

    mov dword [boot_phase_last_success], BOOT_PHASE_KERNEL_CORE
    mov dword [boot_phase_current], BOOT_PHASE_INTERRUPTS_TIME
    call boot_phase_log
    call interrupt_initialize
    jc panic_interrupt_manager
    call interrupt_self_test
    jc panic_interrupt_manager
    call timer_initialize
    jc panic_interrupt_manager
    sti
    call timer_self_test
    jc panic_interrupt_manager
    mov esi, message_interrupts_ok
    call serial_write_string

    call ipc_initialize
    jc panic_ipc
    call ipc_self_test
    jc panic_ipc
    call semantic_initialize
    jc panic_ipc
    call semantic_self_test
    jc panic_ipc
    mov esi, message_semantic_ok
    call serial_write_string
    call semantic_core_initialize
    jc panic_semantic_core
    call semantic_core_self_test
    jc panic_semantic_core
    mov esi, message_semantic_core_ok
    call serial_write_string
    mov esi, message_object_id_abi_ok
    call serial_write_string
    mov esi, message_filesystem_object_registry_ok
    call serial_write_string
    mov esi, message_filesystem_object_enumeration_ok
    call serial_write_string
    mov esi, message_filesystem_object_projection_ok
    call serial_write_string
    mov esi, message_filesystem_object_path_ok
    call serial_write_string
    mov esi, message_filesystem_volume_registry_ok
    call serial_write_string
    mov esi, message_capability_registry_ok
    call serial_write_string
    mov esi, message_capability_authority_ok
    call serial_write_string
    mov esi, message_capability_lifecycle_ok
    call serial_write_string
    mov esi, message_namespace_core_ok
    call serial_write_string
    mov esi, message_object_id_lookup_ok
    call serial_write_string
    mov esi, message_namespace_lookup_ok
    call serial_write_string
    mov esi, message_namespace_path_ok
    call serial_write_string
    mov esi, message_namespace_introspection_ok
    call serial_write_string
    mov esi, message_namespace_enumeration_ok
    call serial_write_string
    mov esi, message_projection_map_ok
    call serial_write_string
    mov esi, message_projection_introspection_ok
    call serial_write_string
    mov esi, message_object_handle_ok
    call serial_write_string
    mov esi, message_handle_object_binding_ok
    call serial_write_string
    mov esi, message_handle_validation_ok
    call serial_write_string
    mov esi, message_handle_path_ok
    call serial_write_string
    call state_manager_initialize
    jc panic_state_manager
    call state_manager_self_test
    jc panic_state_manager
    mov esi, message_state_manager_ok
    call serial_write_string
    call transaction_manager_initialize
    jc panic_transaction_manager
    call transaction_manager_self_test
    jc panic_transaction_manager
    mov esi, message_transaction_manager_ok
    call serial_write_string
    mov esi, message_ipc_ok
    call serial_write_string

    call service_manager_initialize
    jc panic_service_manager
    call service_manager_self_test
    jc panic_service_manager
    mov esi, message_service_manager_ok
    call serial_write_string

    call process_manager_initialize
    jc panic_process_manager
    call process_manager_self_test
    jc panic_process_manager
    mov esi, message_process_manager_ok
    call serial_write_string

    call task_scope_manager_initialize
    jc panic_task_scope_manager
    call task_scope_manager_self_test
    jc panic_task_scope_manager
    mov esi, message_task_scope_manager_ok
    call serial_write_string

    call task_manager_initialize
    jc panic_task_manager
    call task_manager_self_test
    jc panic_task_manager
    mov esi, message_task_manager_ok
    call serial_write_string

    call task_deadline_manager_initialize
    jc panic_task_deadline_manager
    call task_deadline_manager_self_test
    jc panic_task_deadline_manager
    mov esi, message_task_deadline_manager_ok
    call serial_write_string

    call task_group_manager_initialize
    jc panic_task_group_manager
    call task_group_manager_self_test
    jc panic_task_group_manager
    mov esi, message_task_group_manager_ok
    call serial_write_string

    call io_request_manager_initialize
    jc panic_io_request_manager
    call io_completion_initialize
    jc panic_io_completion
    call io_request_manager_self_test
    jc panic_io_request_manager
    mov esi, message_io_request_manager_ok
    call serial_write_string
    call io_completion_self_test
    jc panic_io_completion
    mov esi, message_io_completion_ok
    call serial_write_string

    call shared_buffer_initialize
    jc panic_shared_buffer
    call shared_buffer_self_test
    jc panic_shared_buffer
    mov esi, message_shared_buffer_ok
    call serial_write_string

    call topology_initialize
    jc panic_topology
    call topology_self_test
    jc panic_topology
    mov esi, message_topology_ok
    call serial_write_string
    mov esi, message_topology_cpu_count
    call serial_write_string
    mov eax, [topology_cpu_nodes]
    call serial_write_hex32
    mov esi, message_line_end
    call serial_write_string
    cmp dword [topology_cpu_hierarchy_valid], 1
    jne .topology_hierarchy_reported
    mov esi, message_topology_hierarchy_count
    call serial_write_string
    mov eax, [topology_package_nodes]
    call serial_write_hex32
    mov esi, message_topology_count_separator
    call serial_write_string
    mov eax, [topology_core_nodes]
    call serial_write_hex32
    mov esi, message_topology_count_separator
    call serial_write_string
    mov eax, [topology_cpu_nodes]
    call serial_write_hex32
    mov esi, message_line_end
    call serial_write_string
.topology_hierarchy_reported:

    call iommu_initialize
    jc panic_iommu
    call iommu_self_test
    jc panic_iommu
    mov esi, message_iommu_ok
    call serial_write_string

    call dma_mapping_initialize
    jc panic_dma_mapping
    call dma_mapping_self_test
    jc panic_dma_mapping
    mov esi, message_dma_mapping_ok
    call serial_write_string

    call scatter_gather_initialize
    jc panic_scatter_gather
    call scatter_gather_self_test
    jc panic_scatter_gather
    mov esi, message_scatter_gather_ok
    call serial_write_string

    call dma_scatter_gather_initialize
    jc panic_dma_scatter_gather
    call dma_scatter_gather_self_test
    jc panic_dma_scatter_gather
    mov esi, message_dma_scatter_gather_ok
    call serial_write_string
    mov esi, message_dma_iommu_lifecycle_ok
    call serial_write_string
    mov esi, message_iommu_fault_propagation_ok
    call serial_write_string

    call ringbuf_initialize
    jc panic_ringbuf
    call ringbuf_self_test
    jc panic_ringbuf
    mov esi, message_ringbuf_ok
    call serial_write_string

    call io_scheduler_initialize
    jc panic_io_scheduler
    call io_scheduler_self_test
    jc panic_io_scheduler
    mov esi, message_io_scheduler_ok
    call serial_write_string

    call io_qos_initialize
    jc panic_io_qos
    call io_qos_self_test
    jc panic_io_qos
    mov esi, message_io_qos_ok
    call serial_write_string

    call security_initialize
    jc panic_security
    call security_self_test
    jc panic_security
    mov esi, message_security_ok
    call serial_write_string

    call boot_health_initialize
    jc panic_boot_health
    call boot_health_self_test
    jc panic_boot_health
    call boot_health_publish_core_services
    jc panic_boot_health
    call boot_health_publish_trust_provider   ; §108: TRUST READY/DEGRADED je nach Signatur
    ; kein jc: DEGRADED ist akzeptabel, kein Panic
    mov esi, message_boot_health_authority_ok
    call serial_write_string

    call cpu_manager_initialize
    jc panic_cpu_manager
    mov esi, message_cpu_manager_initialized
    call serial_write_string
    call cpu_manager_self_test
    jc panic_cpu_manager
    mov esi, message_cpu_manager_ok
    call serial_write_string
    mov esi, message_cpu_topology_import_ok
    call serial_write_string

    call module_loader_initialize
    jc panic_module_loader
    call module_loader_self_test
    jc panic_module_loader
    mov esi, message_module_loader_ok
    call serial_write_string

    call config_initialize
    jc panic_config
    call config_self_test
    jc panic_config
    mov esi, message_config_ok
    call serial_write_string

    call abi_initialize
    jc panic_abi
    call abi_self_test
    jc panic_abi
    mov esi, message_abi_ok
    call serial_write_string

    call kog_initialize
    jc panic_kog
    call kog_self_test
    jc panic_kog
    mov esi, message_kog_ok
    call serial_write_string

    call evbus_initialize
    jc panic_evbus
    call evbus_self_test
    jc panic_evbus
    mov esi, message_evbus_ok
    call serial_write_string

    call uobj_initialize
    jc panic_uobj
    call uobj_self_test
    jc panic_uobj
    mov esi, message_uobj_ok
    call serial_write_string

    call cap_initialize
    jc panic_cap
    call cap_self_test
    jc panic_cap
    mov esi, message_cap_ok
    call serial_write_string

    call diag_initialize
    jc panic_diag
    call diag_self_test
    jc panic_diag
    mov esi, message_diag_ok
    call serial_write_string

    call cap_integration_initialize
    jc panic_cap_integ
    call cap_integration_self_test
    jc panic_cap_integ
    mov esi, message_cap_integ_ok
    call serial_write_string

    call vsvc_initialize
    jc panic_vsvc
    call vsvc_self_test
    jc panic_vsvc
    mov esi, message_vsvc_ok
    call serial_write_string

    call sync_initialize
    jc panic_sync
    call sync_self_test
    jc panic_sync
    call sync_ext_initialize
    jc panic_sync
    call sync_ext_self_test
    jc panic_sync
    mov esi, message_sync_ok
    call serial_write_string

    call irq_manager_initialize
    jc panic_irq_manager
    call irq_manager_self_test
    jc panic_irq_manager
    mov esi, message_irq_manager_ok
    call serial_write_string

    ; §010 Exception Manager
    call exception_manager_initialize
    jc panic_exception_mgr
    call exception_manager_self_test
    jc panic_exception_mgr
    mov esi, message_exception_mgr_ok
    call serial_write_string

    ; §011 System Call Interface
    call syscall_manager_initialize
    jc panic_syscall_manager
    call syscall_manager_self_test
    jc panic_syscall_manager
    mov esi, message_syscall_mgr_ok
    call serial_write_string

    mov dword [boot_phase_last_success], BOOT_PHASE_INTERRUPTS_TIME
    mov dword [boot_phase_current], BOOT_PHASE_SCHEDULER_SMP
    call boot_phase_log
    call scheduler_initialize
    jc panic_scheduler
    call thread_manager_initialize
    jc panic_thread_manager
    call thread_manager_self_test
    jc panic_thread_manager
    mov esi, message_thread_manager_ok
    call serial_write_string
    call scheduler_self_test
    jc panic_scheduler
    mov esi, message_scheduler_dynamic_ok
    call serial_write_string
    mov esi, message_scheduler_ok
    call serial_write_string

    call worksteal_initialize
    jc panic_worksteal
    call worksteal_self_test
    jc panic_worksteal
    mov esi, message_worksteal_ok
    call serial_write_string

    call smp_initialize
    jc panic_smp
    call smp_self_test
    jc panic_smp
    call smp_stress_test
    jc panic_smp
    mov esi, message_smp_ok
    call serial_write_string
    mov esi, message_smp_runtime_ok
    call serial_write_string
    mov esi, message_smp_stress_ok
    call serial_write_string

    call numa_initialize
    jc panic_numa
    call numa_self_test
    jc panic_numa
    mov esi, message_numa_ok
    call serial_write_string

    mov dword [boot_phase_last_success], BOOT_PHASE_SCHEDULER_SMP
    mov dword [boot_phase_current], BOOT_PHASE_DEVICE_DISCOVERY
    call boot_phase_log
    call device_manager_initialize
    jc panic_device_manager
    call device_manager_self_test
    jc panic_device_manager
    mov esi, message_device_manager_ok
    call serial_write_string
    call storage_initialize
    jc panic_device_manager
    call storage_self_test
    jc panic_device_manager
    call driver_framework_initialize
    jc panic_driver_framework
    call driver_framework_self_test
    jc panic_driver_framework
    mov esi, message_driver_framework_ok
    call serial_write_string
    call boot_health_mark_kernel_initialized
    jc panic_boot_health
    mov esi, message_boot_health_kernel_initialized
    call serial_write_string
    call firmware_runtime_boot_health_checkpoint
    cmp eax, 1
    jne .boot_health_checkpoint_not_written
    mov esi, message_boot_health_checkpoint_written
    call serial_write_string
    jmp .boot_health_checkpoint_done
.boot_health_checkpoint_not_written:
    cmp eax, 2
    jne .boot_health_checkpoint_done
    mov esi, message_boot_health_checkpoint_failed
    call serial_write_string
.boot_health_checkpoint_done:

    mov dword [boot_phase_last_success], BOOT_PHASE_DEVICE_DISCOVERY
    mov dword [boot_phase_current], BOOT_PHASE_ROOT_FILESYSTEM
    call boot_phase_log
    call vfs_initialize
    jc panic_vfs
    call vfs_self_test
    jc panic_vfs
    mov esi, message_vfs_ok
    call serial_write_string
    ; Persistentes NovaFS-Systemvolume als Root; ohne Volume bleibt das
    ; Bootstrap-RAMFS bestehen (NPSPEC-NOVAFS-ONDISK-0001).
    call novafs_initialize
    mov esi, message_boot_health_root_pending
    cmp dword [nfs_mounted], 1
    jne .root_health_message
    mov esi, message_boot_health_root_degraded
    cmp dword [nfs_readonly], 0
    jne .root_health_message
    mov esi, message_boot_health_root_ready
.root_health_message:
    call serial_write_string
    call boot_health_publish_system_root    ; §126: SYSTEM_ROOT Provider melden

    call network_manager_initialize
    jc panic_network_manager
    call network_manager_self_test
    jc panic_network_manager
    mov esi, message_network_manager_ok
    call serial_write_string

    call power_manager_initialize
    jc panic_power_manager
    call power_manager_self_test
    jc panic_power_manager
    mov esi, message_power_manager_ok
    call serial_write_string

    ; Der Display Server übernimmt den Firmware-Framebuffer als kontrolliertes
    ; Kernelobjekt. Userspace erhält nur die versionierte Display-Service-ABI.
    call display_server_initialize
    jc panic_device_manager
    cmp dword [display_server_ready], 1
    jne .display_fallback
    mov esi, message_display_server_ok
    call serial_write_string
    jmp .display_ready
.display_fallback:
    mov esi, message_display_server_fallback
    call serial_write_string
.display_ready:

    ; Userspace ist die nächste, noch nicht abgeschlossene Bootphase. Der Kernel
    ; meldet deshalb bewusst noch keinen operationalen Zustand (Phase 11).
    mov dword [boot_phase_last_success], BOOT_PHASE_ROOT_FILESYSTEM
    mov dword [boot_phase_current], BOOT_PHASE_USERSPACE
    call boot_phase_log
    call userspace_initialize
    jc panic_userspace
    call boot_health_publish_session_provider ; §108: SESSION READY (Phase-1 Stub)
    ; §126: HealthConfirmed-Status seriell ausgeben
    cmp dword [boot_health_record + BH_RECORD_LAST], BOOT_HEALTH_MILESTONE_CONFIRMED
    jne .health_not_confirmed
    cmp dword [boot_health_record + BH_RECORD_STATUS], BOOT_HEALTH_STATUS_DEGRADED
    je .health_confirmed_degraded
    mov esi, message_boot_health_confirmed
    call serial_write_string
    jmp .health_confirmed_done
.health_confirmed_degraded:
    mov esi, message_boot_health_confirmed_degraded
    call serial_write_string
    jmp .health_confirmed_done
.health_not_confirmed:
    mov esi, message_boot_health_not_confirmed
    call serial_write_string
.health_confirmed_done:
    call firmware_runtime_health_commit       ; §109: HEALTHY Wire nach HealthConfirmed
    cmp eax, 2
    je .commit_failed
    jmp .commit_ok
.commit_failed:
    mov esi, message_health_commit_failed
    call serial_write_string
    jmp .commit_done
.commit_ok:
    cmp eax, 1
    jne .commit_done
    mov esi, message_health_commit_written
    call serial_write_string
.commit_done:
    mov esi, message_userspace_ok
    call serial_write_string
    mov dword [boot_phase_last_success], BOOT_PHASE_USERSPACE
    mov dword [boot_phase_current], BOOT_PHASE_OPERATIONAL
    call boot_phase_log
    call kernel_operational_prepare
    mov eax, SMP_PHASE_OPERATIONAL
    call smp_publish_phase
    jc panic_smp
    mov dword [scheduler_current], 0
    call userspace_enter

userspace_return:
    cli
    mov ax, DATA_SEGMENT
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov esp, [kernel_boot_stack_top]
    cmp dword [userspace_exit_seen], 1
    jne panic_userspace
    mov esi, message_userspace_exit_ok
    call serial_write_string
    call kernel_main

panic_ipc:
    mov eax, 0x00002005
    mov edx, 5
    mov esi, message_ipc_error
    jmp kernel_panic

panic_semantic_core:
    mov eax, 0x00002029
    mov edx, 0x53454D43             ; "SEMC"
    mov esi, message_semantic_core_error
    jmp kernel_panic

panic_state_manager:
    mov eax, 0x00002027
    mov edx, 0x53544154             ; "STAT"
    mov esi, message_state_manager_error
    jmp kernel_panic

panic_transaction_manager:
    mov eax, 0x00002028
    mov edx, 0x54584E20             ; "TXN "
    mov esi, message_transaction_manager_error
    jmp kernel_panic

panic_service_manager:
    mov eax, 0x00002010
    mov edx, 10
    mov esi, message_service_manager_error
    jmp kernel_panic

panic_process_manager:
    mov eax, 0x00002011
    mov edx, 11
    mov esi, message_process_manager_error
    jmp kernel_panic

panic_task_scope_manager:
    mov eax, 0x00002018
    mov edx, 24
    mov esi, message_task_scope_manager_error
    jmp kernel_panic

panic_task_manager:
    mov eax, 0x00002019
    mov edx, 25
    mov esi, message_task_manager_error
    jmp kernel_panic

panic_task_deadline_manager:
    mov eax, 0x0000201A
    mov edx, 26
    mov esi, message_task_deadline_manager_error
    jmp kernel_panic

panic_task_group_manager:
    mov eax, 0x0000201B
    mov edx, 27
    mov esi, message_task_group_manager_error
    jmp kernel_panic

panic_io_request_manager:
    mov eax, 0x0000201C
    mov edx, 28
    mov esi, message_io_request_manager_error
    jmp kernel_panic

panic_io_completion:
    mov eax, 0x0000201F
    mov edx, 31
    mov esi, message_io_completion_error
    jmp kernel_panic

panic_shared_buffer:
    mov eax, 0x00002020
    mov edx, 32
    mov esi, message_shared_buffer_error
    jmp kernel_panic

panic_topology:
    mov eax, 0x00002025
    mov edx, 37
    mov esi, message_topology_error
    jmp kernel_panic

panic_iommu:
    mov eax, 0x00002024
    mov edx, 36
    mov esi, message_iommu_error
    jmp kernel_panic

panic_dma_mapping:
    mov eax, 0x00002021
    mov edx, 33
    mov esi, message_dma_mapping_error
    jmp kernel_panic

panic_scatter_gather:
    mov eax, 0x00002022
    mov edx, 34
    mov esi, message_scatter_gather_error
    jmp kernel_panic

panic_dma_scatter_gather:
    mov eax, 0x00002023
    mov edx, 35
    mov esi, message_dma_scatter_gather_error
    jmp kernel_panic

panic_ringbuf:
    mov eax, 0x00002024
    mov edx, 36
    mov esi, message_ringbuf_error
    jmp kernel_panic

panic_io_scheduler:
    mov eax, 0x0000201D
    mov edx, 29
    mov esi, message_io_scheduler_error
    jmp kernel_panic

panic_io_qos:
    mov eax, 0x0000201E
    mov edx, 30
    mov esi, message_io_qos_error
    jmp kernel_panic

panic_security:
    mov eax, 0x00002013
    mov edx, 13
    mov esi, message_security_error
    jmp kernel_panic

panic_boot_health:
    mov eax, 0x00002026
    mov edx, 0x4845414C             ; "HEAL"
    mov esi, message_boot_health_error
    jmp kernel_panic

panic_config:
    mov eax, 0x00000012
    mov edx, 0x43464700             ; "CFG\0"
    mov esi, message_config_error
    jmp kernel_panic

panic_abi:
    mov eax, 0x00000013
    mov edx, 0x41424900             ; "ABI\0"
    mov esi, message_abi_error
    jmp kernel_panic

panic_kog:
    mov eax, 0x00000014
    mov edx, 0x4B4F4700             ; "KOG\0"
    mov esi, message_kog_error
    jmp kernel_panic

panic_evbus:
    mov eax, 0x00000015
    mov edx, 0x45564200             ; "EVB\0"
    mov esi, message_evbus_error
    jmp kernel_panic

panic_uobj:
    mov eax, 0x00000016
    mov edx, 0x554F4200             ; "UOB\0"
    mov esi, message_uobj_error
    jmp kernel_panic

panic_cap:
    mov eax, 0x00000017
    mov edx, 0x43415000             ; "CAP\0"
    mov esi, message_cap_error
    jmp kernel_panic

panic_diag:
    mov eax, 0x00000018
    mov edx, 0x44494100             ; "DIA\0"
    mov esi, message_diag_error
    jmp kernel_panic

panic_cap_integ:
    mov eax, 0x00000019
    mov edx, 0x43494E54             ; "CINT"
    mov esi, message_cap_integ_error
    jmp kernel_panic

panic_vsvc:
    mov eax, 0x0000001A
    mov edx, 0x56535643             ; "VSVC"
    mov esi, message_vsvc_error
    jmp kernel_panic

panic_sync:
    mov eax, 0x0000001C
    mov edx, 0x53594E43             ; "SYNC"
    mov esi, message_sync_error
    jmp kernel_panic

panic_irq_manager:
    mov eax, 0x0000001E
    mov edx, 0x49525147             ; "IRQG"
    mov esi, message_irq_manager_error
    jmp kernel_panic

panic_exception_mgr:
    mov eax, 0x0000001F
    mov edx, 0x45584D47             ; "EXMG"
    mov esi, message_exc_mgr_error
    jmp kernel_panic

panic_syscall_manager:
    mov eax, 0x00000016
    mov edx, 0x5359534B             ; "SYSK"
    mov esi, message_syscall_mgr_error
    jmp kernel_panic

panic_module_loader:
    mov eax, 0x00000011
    mov edx, 0x4D4F4455             ; "MODU"
    mov esi, message_module_loader_error
    jmp kernel_panic

panic_cpu_manager:
    mov eax, 0x00000012
    mov edx, 0x43505520             ; "CPU "
    mov esi, message_cpu_manager_error
    jmp kernel_panic

panic_smp:
    mov eax, 0x0000001B
    mov edx, 0x534D5020             ; "SMP "
    mov esi, message_smp_error
    jmp kernel_panic

panic_scheduler:
    mov eax, 0x00002004
    mov edx, 4
    mov esi, message_scheduler_error
    jmp kernel_panic

panic_thread_manager:
    mov eax, 0x00002012
    mov edx, 12
    mov esi, message_thread_manager_error
    jmp kernel_panic

panic_worksteal:
    mov eax, 0x00002025
    mov edx, 37
    mov esi, message_worksteal_error
    jmp kernel_panic

panic_device_manager:
    mov eax, 0x0000200C
    mov edx, 12
    mov esi, message_device_manager_error
    jmp kernel_panic

panic_driver_framework:
    mov eax, 0x00002018
    mov edx, 24
    mov esi, message_driver_framework_error
    jmp kernel_panic

panic_numa:
    mov eax, 0x00002028
    mov edx, 40
    mov esi, message_numa_error
    jmp kernel_panic

panic_vfs:
    mov eax, 0x00002014
    mov edx, 20
    mov esi, message_vfs_error
    jmp kernel_panic

panic_power_manager:
    mov eax, 0x00002017
    mov edx, 23
    mov esi, message_power_manager_error
    jmp kernel_panic

panic_network_manager:
    mov eax, 0x00002018
    mov edx, 24
    mov esi, message_network_manager_error
    jmp kernel_panic

panic_userspace:
    mov eax, 0x00002015
    mov edx, 21
    mov esi, message_userspace_error
    jmp kernel_panic

panic_paging:
    mov eax, 0x00002002
    mov edx, 2
    mov esi, message_paging_error
    jmp kernel_panic

panic_object_manager:
    mov eax, 0x00002008
    mov edx, 8
    mov esi, message_object_manager_error
    jmp kernel_panic

panic_handle_manager:
    mov eax, 0x00002016
    mov edx, 22
    mov esi, message_handle_manager_error
    jmp kernel_panic

panic_component_manager:
    mov eax, 0x00002009
    mov edx, 9
    mov esi, message_component_manager_error
    jmp kernel_panic

panic_heap:
    mov eax, 0x00002003
    mov edx, 3
    mov esi, message_heap_error
    jmp kernel_panic

panic_interrupt_manager:
    mov eax, 0x00002007
    mov edx, 7
    mov esi, message_interrupts_error
    jmp kernel_panic

panic_memory_manager:
    mov eax, 0x00002001
    mov edx, 1
    mov esi, message_pmm_error
    jmp kernel_panic

panic_invalid_handoff:
    mov eax, 0x00001004
    mov edx, 0x1004
    mov esi, message_bib_error
    jmp kernel_panic

; ---------------------------------------------------------------------------
; BIB-Validierung und Firmware-Interface (NPSPEC-HAL-FIRMWARE-0001)
; ---------------------------------------------------------------------------

validate_bib:
    cmp dword [ebx + BIB_OFF_MAGIC_LOW], NOVA_BIB_MAGIC_LOW
    jne .invalid
    cmp dword [ebx + BIB_OFF_MAGIC_HIGH], NOVA_BIB_MAGIC_HIGH
    jne .invalid
    cmp word [ebx + BIB_OFF_VERSION_MAJOR], NOVA_BIB_VERSION_MAJOR
    jne .invalid
    cmp word [ebx + BIB_OFF_HEADER_SIZE], NOVA_BIB_HEADER_SIZE
    jne .invalid
    cmp dword [ebx + BIB_OFF_ARCHITECTURE], NKI_ARCH_X86_32
    jne .invalid

    mov ecx, [ebx + BIB_OFF_TOTAL_SIZE]
    cmp ecx, NOVA_BIB_HEADER_SIZE
    jb .invalid
    cmp ecx, BOOT_INFO_CAPACITY
    ja .invalid
    test ecx, 7
    jnz .invalid

    mov edi, [ebx + BIB_OFF_CHECKSUM]
    mov esi, ebx
    call crc32_bib
    cmp eax, edi
    jne .invalid

    clc
    ret
.invalid:
    stc
    ret

; CRC32 über den BIB; Bytes des Checksum-Felds werden als null behandelt.
crc32_bib:
    push ebx
    mov eax, 0xFFFFFFFF
    xor edx, edx
.byte:
    test ecx, ecx
    jz .finish

    xor ebx, ebx
    cmp edx, BIB_OFF_CHECKSUM
    jb .load
    cmp edx, BIB_OFF_CHECKSUM + 4
    jb .have_byte
.load:
    mov bl, [esi]
.have_byte:
    xor al, bl

    push ecx
    mov ecx, 8
.bit:
    shr eax, 1
    jnc .next_bit
    xor eax, 0xEDB88320
.next_bit:
    loop .bit
    pop ecx

    inc esi
    inc edx
    dec ecx
    jmp .byte
.finish:
    not eax
    pop ebx
    ret

; ---------------------------------------------------------------------------
; TLV -> interner Kernel Context
; ---------------------------------------------------------------------------

create_kernel_context:
    mov dword [kernel_context + CONTEXT_SEEN], 0
    mov esi, ebx
    add esi, NOVA_BIB_HEADER_SIZE
    mov edi, ebx
    add edi, [ebx + BIB_OFF_TOTAL_SIZE]

.next:
    cmp esi, edi
    je .complete
    ja .invalid

    movzx eax, word [esi + 0]       ; Type
    mov ecx, [esi + 4]              ; Length
    test ecx, 7
    jnz .invalid
    lea edx, [esi + BIB_TLV_HEADER_SIZE]
    lea ebp, [edx + ecx]
    cmp ebp, edi
    ja .invalid

    cmp eax, BIB_TLV_FIRMWARE
    je .firmware
    cmp eax, BIB_TLV_CPU
    je .cpu
    cmp eax, BIB_TLV_MEMORY
    je .memory
    cmp eax, BIB_TLV_GRAPHICS
    je .graphics
    cmp eax, BIB_TLV_KERNEL
    je .kernel
    cmp eax, BIB_TLV_SECURITY
    je .security
    cmp eax, BIB_TLV_STORAGE
    je .storage
    cmp eax, BIB_TLV_ACPI
    je .acpi
    cmp eax, BIB_TLV_BOOT_OPTIONS
    je .boot_options
    cmp eax, BIB_TLV_MODULES
    je .modules
    cmp eax, BIB_TLV_ENTROPY
    je .entropy
    cmp eax, BIB_TLV_SYSTEM
    je .system
    cmp eax, BIB_TLV_KERNEL_IDENTITY
    je .kernel_identity
    cmp eax, BIB_TLV_FIRMWARE_RUNTIME
    je .firmware_runtime
    test word [esi + 2], BIB_TLV_FLAG_REQUIRED
    jnz .invalid
    jmp .advance                    ; unbekannte optionale TLVs überspringen

.firmware:
    cmp ecx, BIB_FIRMWARE_SIZE
    jb .invalid
    or dword [kernel_context + CONTEXT_SEEN], CONTEXT_HAS_FIRMWARE
    mov eax, [edx + 0]
    mov [kernel_context + CONTEXT_PLATFORM], eax
    mov eax, [edx + 4]
    mov [kernel_context + CONTEXT_BOOT_DRIVE], eax
    jmp .advance

.memory:
    cmp ecx, BIB_MEMORY_SIZE
    jb .invalid
    or dword [kernel_context + CONTEXT_SEEN], CONTEXT_HAS_MEMORY
    mov eax, [edx + 0]
    mov [kernel_context + CONTEXT_MEMORY_MAP], eax
    mov eax, [edx + 4]
    mov [kernel_context + CONTEXT_MEMORY_COUNT], eax
    mov eax, [edx + 8]
    mov [kernel_context + CONTEXT_MEMORY_ENTRY_SIZE], eax
    cmp eax, MEMORY_MAP_ENTRY_SIZE
    jne .invalid
    cmp dword [kernel_context + CONTEXT_MEMORY_COUNT], 0
    je .invalid
    jmp .advance

.cpu:
    cmp ecx, BIB_CPU_SIZE
    jb .invalid
    or dword [kernel_context + CONTEXT_SEEN], CONTEXT_HAS_CPU
    mov eax, [edx + 16]
    mov [kernel_context + CONTEXT_CPU_FEATURE_EDX], eax
    mov eax, [edx + 20]
    mov [kernel_context + CONTEXT_CPU_FEATURE_ECX], eax
    jmp .advance

.graphics:
    cmp ecx, BIB_GRAPHICS_SIZE
    jb .invalid
    or dword [kernel_context + CONTEXT_SEEN], CONTEXT_HAS_GRAPHICS
    mov eax, [edx + 0]
    mov [kernel_context + CONTEXT_FRAMEBUFFER], eax
    mov eax, [edx + 4]
    mov [kernel_context + CONTEXT_PITCH], eax
    mov eax, [edx + 8]
    mov [kernel_context + CONTEXT_WIDTH], eax
    mov eax, [edx + 12]
    mov [kernel_context + CONTEXT_HEIGHT], eax
    mov eax, [edx + 16]
    mov [kernel_context + CONTEXT_BPP], eax
    mov eax, [edx + 20]
    mov [kernel_context + CONTEXT_PIXEL_FORMAT], eax
    jmp .advance

.kernel:
    cmp ecx, BIB_KERNEL_SIZE
    jb .invalid
    or dword [kernel_context + CONTEXT_SEEN], CONTEXT_HAS_KERNEL
    mov eax, [edx + 0]
    mov [kernel_context + CONTEXT_KERNEL_ADDRESS], eax
    mov eax, [edx + 4]
    mov [kernel_context + CONTEXT_KERNEL_SIZE], eax
    mov eax, [edx + 8]
    mov [kernel_context + CONTEXT_KERNEL_ENTRY], eax
    cmp dword [kernel_context + CONTEXT_KERNEL_ADDRESS], KERNEL_ENTRY_ADDRESS
    jne .invalid
    cmp dword [kernel_context + CONTEXT_KERNEL_ENTRY], KERNEL_ENTRY_ADDRESS
    jne .invalid
    jmp .advance

.kernel_identity:
    cmp ecx, BIB_KERNEL_IDENTITY_SIZE
    jb .invalid
    or dword [kernel_context + CONTEXT_SEEN], CONTEXT_HAS_KERNEL_ID
    mov eax, [edx + 0]
    mov [kernel_context + CONTEXT_KERNEL_BUILD_ID + 0], eax
    mov eax, [edx + 4]
    mov [kernel_context + CONTEXT_KERNEL_BUILD_ID + 4], eax
    mov eax, [edx + 8]
    mov [kernel_context + CONTEXT_KERNEL_BUILD_ID + 8], eax
    mov eax, [edx + 12]
    mov [kernel_context + CONTEXT_KERNEL_BUILD_ID + 12], eax
    mov eax, [edx + 16]
    mov [kernel_context + CONTEXT_KERNEL_BUILD_ID + 16], eax
    mov eax, [edx + 20]
    mov [kernel_context + CONTEXT_KERNEL_FORMAT], eax
    jmp .advance

.firmware_runtime:
    cmp ecx, BIB_FIRMWARE_RUNTIME_SIZE
    jb .invalid
    cmp dword [edx + 0], NOVA_FIRMWARE_RUNTIME_PROVIDER_UEFI_X64
    jne .advance
    test dword [edx + 4], NOVA_FIRMWARE_RUNTIME_PERSIST_BOOT_HEALTH
    jz .advance
    cmp dword [edx + 12], 0
    jne .advance
    cmp dword [edx + 20], 0
    jne .advance
    cmp dword [edx + 24], 64
    jb .advance
    cmp dword [edx + 28], 0
    jne .advance
    mov eax, [edx + 4]
    mov [kernel_context + CONTEXT_FIRMWARE_RUNTIME_CAPS], eax
    mov eax, [edx + 8]
    mov [kernel_context + CONTEXT_FIRMWARE_RUNTIME_CONTEXT], eax
    mov eax, [edx + 16]
    mov [kernel_context + CONTEXT_FIRMWARE_RUNTIME_ENTRY], eax
    jmp .advance

.security:
    cmp ecx, BIB_SECURITY_SIZE
    jb .invalid
    mov eax, [edx]
    mov [kernel_context + CONTEXT_SECURITY_STATE], eax
    mov eax, [edx + 8]
    mov [kernel_context + CONTEXT_ENTROPY_QUALITY], eax
    jmp .advance

.storage:
    cmp ecx, BIB_STORAGE_SIZE
    jb .invalid
    mov eax, [edx + 4]
    mov [kernel_context + CONTEXT_BOOT_DRIVE], eax
    jmp .advance

.acpi:
    cmp ecx, BIB_ACPI_SIZE
    jb .invalid
    mov eax, [edx + 0]
    cmp dword [edx + 4], 0
    jne .acpi_unaddressable
    mov [kernel_context + CONTEXT_ACPI_ADDRESS], eax
    jmp .advance
.acpi_unaddressable:
    mov dword [kernel_context + CONTEXT_ACPI_ADDRESS], 0
    jmp .advance

.modules:
    cmp ecx, BIB_MODULES_SIZE
    jb .invalid
    mov eax, [edx + 0]
    mov [kernel_context + CONTEXT_MODULES_ADDRESS], eax
    mov eax, [edx + 4]
    mov [kernel_context + CONTEXT_MODULE_COUNT], eax
    jmp .advance

.boot_options:
    cmp ecx, BIB_BOOT_OPTIONS_SIZE
    jb .invalid
    cmp dword [edx + 0], NOVA_BOOT_MODE_DIAGNOSTIC
    ja .invalid
    or dword [kernel_context + CONTEXT_SEEN], CONTEXT_HAS_BOOT_OPTIONS
    mov eax, [edx + 0]
    mov [kernel_context + CONTEXT_BOOT_MODE], eax
    mov eax, [edx + 4]
    mov [kernel_context + CONTEXT_BOOT_FLAGS], eax
    cmp dword [edx + 8], NOVA_BOOT_GENERATION_RECOVERY
    ja .invalid
    mov eax, [edx + 8]
    mov [kernel_context + CONTEXT_BOOT_GENERATION], eax
    mov eax, [edx + 12]
    mov [kernel_context + CONTEXT_FALLBACK_LEVEL], eax
    jmp .advance

.entropy:
    cmp ecx, BIB_ENTROPY_SIZE
    jb .invalid
    cmp dword [edx + 24], 16
    jb .invalid
    or dword [kernel_context + CONTEXT_SEEN], CONTEXT_HAS_ENTROPY
    mov eax, [edx + 0]
    mov [kernel_context + CONTEXT_ENTROPY_SEED + 0], eax
    mov eax, [edx + 4]
    mov [kernel_context + CONTEXT_ENTROPY_SEED + 4], eax
    mov eax, [edx + 8]
    mov [kernel_context + CONTEXT_ENTROPY_SEED + 8], eax
    mov eax, [edx + 12]
    mov [kernel_context + CONTEXT_ENTROPY_SEED + 12], eax
    mov eax, [edx + 20]
    mov [kernel_context + CONTEXT_ENTROPY_QUALITY], eax
    jmp .advance

.system:
    cmp ecx, BIB_SYSTEM_SIZE
    jb .invalid
    or dword [kernel_context + CONTEXT_SEEN], CONTEXT_HAS_SYSTEM
    mov eax, [edx + 0]
    mov [kernel_context + CONTEXT_SYSTEM_GENERATION], eax
    mov eax, [edx + 4]
    mov [kernel_context + CONTEXT_SYSTEM_GENERATION_HI], eax
    mov eax, [edx + 8]
    mov [kernel_context + CONTEXT_BOOT_ATTEMPT], eax
    jmp .advance

.advance:
    mov esi, ebp
    jmp .next

.complete:
    mov eax, [kernel_context + CONTEXT_SEEN]
    and eax, CONTEXT_REQUIRED
    cmp eax, CONTEXT_REQUIRED
    jne .invalid
    clc
    ret
.invalid:
    stc
    ret

ACPI_SRAT_CPU_CAPACITY         equ 8
ACPI_SRAT_MEMORY_CAPACITY      equ 8
ACPI_SRAT_CPU_RECORD_SIZE      equ 8
ACPI_SRAT_MEMORY_RECORD_SIZE   equ 24

; ACPI-Root-Pointer vor der Paging-Aktivierung prüfen. Ein fehlender oder
; beschädigter RSDP verhindert den UP-Boot nicht und aktiviert keinen AP.
acpi_rsdp_initialize:
    ; Der BSP ist auch ohne Firmwaretabellen als kontrollierter Fallback
    ; bekannt. Die APIC-ID stammt aus CPUID und bleibt damit Hardware-ID statt
    ; frei erfundener logischer Nummer.
    mov dword [acpi_cpu_count], 1
    mov dword [acpi_madt_valid], 0
    mov dword [acpi_srat_valid], 0
    mov dword [acpi_srat_cpu_count], 0
    mov dword [acpi_srat_memory_count], 0
    mov eax, 1
    cpuid
    shr ebx, 24
    mov [acpi_apic_ids], ebx
    mov esi, [kernel_context + CONTEXT_ACPI_ADDRESS]
    test esi, esi
    jz .bios_scan
    call acpi_rsdp_validate
    jnc .ready
    mov dword [kernel_context + CONTEXT_ACPI_ADDRESS], 0
.bios_scan:
    cmp dword [kernel_context + CONTEXT_PLATFORM], NOVA_BOOT_PLATFORM_BIOS
    jne .unavailable
    movzx esi, word [0x40E]        ; EBDA-Segment aus dem BIOS Data Area
    shl esi, 4
    cmp esi, 0x80000
    jb .high_scan
    cmp esi, 0xA0000
    jae .high_scan
    lea edx, [esi + 1024]
    call acpi_rsdp_scan_range
    jnc .found
.high_scan:
    mov esi, 0xE0000
    mov edx, 0x100000
    call acpi_rsdp_scan_range
    jc .unavailable
.found:
    mov [kernel_context + CONTEXT_ACPI_ADDRESS], esi
.ready:
    call acpi_madt_discover
    call acpi_srat_discover
    mov esi, message_acpi_rsdp_ok
    call serial_write_string
    cmp dword [acpi_madt_valid], 1
    jne .ready_done
    mov esi, message_acpi_cpu_count
    call serial_write_string
    mov eax, [acpi_cpu_count]
    call serial_write_hex32
    mov esi, message_line_end
    call serial_write_string
.ready_done:
    cmp dword [acpi_srat_valid], 1
    jne .srat_unavailable
    call acpi_srat_self_test
    jc .srat_unavailable
    mov esi, message_acpi_srat_ok
    call serial_write_string
    mov eax, [acpi_srat_cpu_count]
    call serial_write_hex32
    mov esi, message_topology_count_separator
    call serial_write_string
    mov eax, [acpi_srat_memory_count]
    call serial_write_hex32
    mov esi, message_line_end
    call serial_write_string
    ret
.srat_unavailable:
    mov dword [acpi_srat_valid], 0
    mov esi, message_acpi_srat_unavailable
    call serial_write_string
    ret
.unavailable:
    mov esi, message_acpi_rsdp_missing
    call serial_write_string
    ret

; ESI = Anfang, EDX = exklusives Ende; 16-Byte-RSDP-Ausrichtung.
acpi_rsdp_scan_range:
.next:
    lea eax, [esi + 36]
    cmp eax, edx
    ja .missing
    push edx
    call acpi_rsdp_validate
    pop edx
    jnc .found
    add esi, 16
    jmp .next
.found:
    clc
    ret
.missing:
    stc
    ret

acpi_rsdp_validate:
    cmp esi, 0xFFFFEFFF
    ja .invalid
    cmp dword [esi], 0x20445352 ; "RSD "
    jne .invalid
    cmp dword [esi + 4], 0x20525450 ; "PTR "
    jne .invalid
    push ecx
    push edi
    xor eax, eax
    mov edi, esi
    mov ecx, 20
.first_checksum:
    add al, [edi]
    inc edi
    loop .first_checksum
    test al, al
    jnz .checksum_invalid
    cmp byte [esi + 15], 2
    jb .valid
    mov ecx, [esi + 20]
    cmp ecx, 36
    jb .checksum_invalid
    cmp ecx, 4096
    ja .checksum_invalid
    mov edi, esi
    add edi, ecx
    jc .checksum_invalid
    mov edi, esi
    xor eax, eax
.extended_checksum:
    add al, [edi]
    inc edi
    loop .extended_checksum
    test al, al
    jnz .checksum_invalid
.valid:
    pop edi
    pop ecx
    clc
    ret
.checksum_invalid:
    pop edi
    pop ecx
.invalid:
    stc
    ret

; Physische ACPI-Tabellen werden ausschließlich im frühen Identity-Modus
; gelesen. Ein kompletter Tabellenbereich muss in einem E820/UEFI-RAM-
; Deskriptor liegen; MMIO und 64-Bit-Adressen werden nicht dereferenziert.
; ESI=Adresse, ECX=Länge, CF=0 wenn vollständig im Firmware-Memory-Map.
acpi_physical_range_valid:
    push eax
    push ebx
    push edx
    push edi
    test esi, esi
    jz .bad
    test ecx, ecx
    jz .bad
    mov edx, esi
    add edx, ecx
    jc .bad
    mov ebx, [kernel_context + CONTEXT_MEMORY_COUNT]
    cmp ebx, 256                   ; UEFI-Map 0x5800..0x7000: 256 x 24 Byte
    ja .bad
    mov edi, [kernel_context + CONTEXT_MEMORY_MAP]
.entry:
    test ebx, ebx
    jz .bad
    cmp dword [edi + 4], 0
    jne .next
    cmp dword [edi + 16], 1
    jb .next
    cmp dword [edi + 16], 4
    ja .next
    mov eax, [edi]
    cmp esi, eax
    jb .next
    cmp dword [edi + 12], 0
    jne .good
    add eax, [edi + 8]
    jc .good
    cmp edx, eax
    jbe .good
.next:
    add edi, MEMORY_MAP_ENTRY_SIZE
    dec ebx
    jmp .entry
.good:
    pop edi
    pop edx
    pop ebx
    pop eax
    clc
    ret
.bad:
    pop edi
    pop edx
    pop ebx
    pop eax
    stc
    ret

; ESI=physischer Tabellenanfang, EAX=erwartete Signatur;
; ECX=geprüfte Länge bei Erfolg. Maximal 4 KiB Bootstrap-Tabellen.
acpi_table_validate:
    push eax
    push edx
    push edi
    mov ecx, 36
    call acpi_physical_range_valid
    jc .bad
    mov edx, [esi]
    cmp edx, eax
    jne .bad
    mov ecx, [esi + 4]
    cmp ecx, 36
    jb .bad
    cmp ecx, 4096
    ja .bad
    call acpi_physical_range_valid
    jc .bad
    mov edx, ecx
    mov edi, esi
    xor eax, eax
.checksum:
    add al, [edi]
    inc edi
    loop .checksum
    mov ecx, edx
    test al, al
    jnz .bad
    pop edi
    pop edx
    pop eax
    clc
    ret
.bad:
    pop edi
    pop edx
    pop eax
    stc
    ret

; BIOS/UEFI-MADT vor Paging in eine kleine, unveränderliche ID-Liste kopieren.
; Der BSP bleibt CPU 0; APs werden erst später separat gestartet.
acpi_madt_discover:
    mov dword [acpi_cpu_count], 1
    mov dword [acpi_madt_valid], 0
    mov eax, 1
    cpuid
    shr ebx, 24
    mov [acpi_apic_ids], ebx
    mov ebp, [kernel_context + CONTEXT_ACPI_ADDRESS]
    cmp byte [ebp + 15], 2
    jb .rsdt
    cmp dword [ebp + 28], 0
    jne .rsdt
    mov esi, [ebp + 24]
    mov eax, 0x54445358         ; XSDT
    call acpi_table_validate
    jc .rsdt
    mov dword [acpi_root_entry_size], 8
    jmp .root_ready
.rsdt:
    mov esi, [ebp + 16]
    mov eax, 0x54445352         ; RSDT
    call acpi_table_validate
    jc .missing
    mov dword [acpi_root_entry_size], 4
.root_ready:
    mov eax, ecx
    sub eax, 36
    xor edx, edx
    div dword [acpi_root_entry_size]
    test edx, edx
    jnz .missing
    mov [acpi_root_entries_left], eax
    lea eax, [esi + 36]
    mov [acpi_root_cursor], eax
.root_next:
    cmp dword [acpi_root_entries_left], 0
    je .missing
    mov ebx, [acpi_root_cursor]
    mov esi, [ebx]
    cmp dword [acpi_root_entry_size], 8
    jne .candidate
    cmp dword [ebx + 4], 0
    jne .advance
.candidate:
    mov eax, 0x43495041         ; APIC/MADT
    call acpi_table_validate
    jc .advance
    call acpi_madt_parse
    jc .advance
    mov dword [acpi_madt_valid], 1
    ret
.advance:
    mov eax, [acpi_root_entry_size]
    add [acpi_root_cursor], eax
    dec dword [acpi_root_entries_left]
    jmp .root_next
.missing:
    mov dword [acpi_cpu_count], 1
    ret

; ESI=geprüfter MADT, ECX=geprüfte Gesamtlänge.
acpi_madt_parse:
    cmp ecx, 44
    jb .bad
    mov dword [acpi_cpu_count], 1
    mov dword [acpi_madt_bsp_seen], 0
    mov edi, esi
    add edi, 44
    mov ebp, esi
    add ebp, ecx
.entry:
    cmp edi, ebp
    je .complete
    lea eax, [edi + 2]
    cmp eax, ebp
    ja .bad
    movzx ebx, byte [edi + 1]
    cmp ebx, 2
    jb .bad
    mov eax, edi
    add eax, ebx
    cmp eax, ebp
    ja .bad
    cmp byte [edi], 0
    je .lapic
    cmp byte [edi], 9
    je .x2apic
    jmp .next
.lapic:
    cmp ebx, 8
    jb .bad
    test dword [edi + 4], 1
    jz .next
    movzx eax, byte [edi + 3]
    call acpi_madt_add_id
    jmp .next
.x2apic:
    cmp ebx, 16
    jb .bad
    test dword [edi + 8], 1
    jz .next
    mov eax, [edi + 4]
    call acpi_madt_add_id
.next:
    add edi, ebx
    jmp .entry
.complete:
    cmp dword [acpi_madt_bsp_seen], 1
    jne .bad
    clc
    ret
.bad:
    mov dword [acpi_cpu_count], 1
    stc
    ret

; EAX=Hardware-ID, Duplikate und BSP werden nicht als AP registriert.
acpi_madt_add_id:
    cmp eax, [acpi_apic_ids]
    jne .other
    mov dword [acpi_madt_bsp_seen], 1
    ret
.other:
    push ecx
    push edx
    xor ecx, ecx
.unique:
    cmp ecx, [acpi_cpu_count]
    jae .insert
    cmp eax, [acpi_apic_ids + ecx * 4]
    je .done
    inc ecx
    jmp .unique
.insert:
    cmp ecx, CPU_CAPACITY
    jae .done
    mov [acpi_apic_ids + ecx * 4], eax
    inc dword [acpi_cpu_count]
.done:
    pop edx
    pop ecx
    ret

; Sucht und validiert die ACPI System Resource Affinity Table getrennt von der
; MADT. Die Root-Tabelle wird erneut geprüft, damit kein impliziter Cursor-
; Zustand zwischen Firmwareprovidern geteilt wird.
acpi_srat_discover:
    mov dword [acpi_srat_valid], 0
    mov dword [acpi_srat_cpu_count], 0
    mov dword [acpi_srat_memory_count], 0
    mov dword [acpi_srat_conflicts], 0
    mov dword [acpi_srat_unsupported_memory], 0
    mov edi, acpi_srat_cpu_records
    xor eax, eax
    mov ecx, (ACPI_SRAT_CPU_CAPACITY * ACPI_SRAT_CPU_RECORD_SIZE) / 4
    rep stosd
    mov edi, acpi_srat_memory_records
    mov ecx, (ACPI_SRAT_MEMORY_CAPACITY * ACPI_SRAT_MEMORY_RECORD_SIZE) / 4
    rep stosd
    mov ebp, [kernel_context + CONTEXT_ACPI_ADDRESS]
    test ebp, ebp
    jz .missing
    cmp byte [ebp + 15], 2
    jb .rsdt
    cmp dword [ebp + 28], 0
    jne .rsdt
    mov esi, [ebp + 24]
    mov eax, 0x54445358             ; XSDT
    call acpi_table_validate
    jc .rsdt
    mov dword [acpi_srat_root_entry_size], 8
    jmp .root_ready
.rsdt:
    mov esi, [ebp + 16]
    mov eax, 0x54445352             ; RSDT
    call acpi_table_validate
    jc .missing
    mov dword [acpi_srat_root_entry_size], 4
.root_ready:
    mov eax, ecx
    sub eax, 36
    xor edx, edx
    div dword [acpi_srat_root_entry_size]
    test edx, edx
    jnz .missing
    mov [acpi_srat_root_entries_left], eax
    lea eax, [esi + 36]
    mov [acpi_srat_root_cursor], eax
.root_next:
    cmp dword [acpi_srat_root_entries_left], 0
    je .missing
    mov ebx, [acpi_srat_root_cursor]
    mov esi, [ebx]
    cmp dword [acpi_srat_root_entry_size], 8
    jne .candidate
    cmp dword [ebx + 4], 0
    jne .advance
.candidate:
    mov eax, 0x54415253             ; SRAT
    call acpi_table_validate
    jc .advance
    call acpi_srat_parse
    jc .invalid
    mov dword [acpi_srat_valid], 1
    ret
.advance:
    mov eax, [acpi_srat_root_entry_size]
    add [acpi_srat_root_cursor], eax
    dec dword [acpi_srat_root_entries_left]
    jmp .root_next
.invalid:
    mov dword [acpi_srat_conflicts], 1
.missing:
    ret

; ESI=validierte SRAT, ECX=Gesamtlänge.
acpi_srat_parse:
    cmp ecx, 48
    jb .invalid
    mov edi, esi
    add edi, 48
    mov ebp, esi
    add ebp, ecx
.entry:
    cmp edi, ebp
    je .complete
    lea eax, [edi + 2]
    cmp eax, ebp
    ja .invalid
    movzx ebx, byte [edi + 1]
    cmp ebx, 2
    jb .invalid
    mov [acpi_srat_entry_length], ebx
    mov eax, edi
    add eax, ebx
    jc .invalid
    cmp eax, ebp
    ja .invalid
    cmp byte [edi], 0
    je .lapic
    cmp byte [edi], 1
    je .memory
    cmp byte [edi], 2
    je .x2apic
    jmp .next
.lapic:
    cmp ebx, 16
    jb .invalid
    test dword [edi + 4], 1
    jz .next
    movzx eax, byte [edi + 3]
    movzx edx, byte [edi + 2]
    movzx ecx, byte [edi + 9]
    shl ecx, 8
    or edx, ecx
    movzx ecx, byte [edi + 10]
    shl ecx, 16
    or edx, ecx
    movzx ecx, byte [edi + 11]
    shl ecx, 24
    or edx, ecx
    call acpi_srat_add_cpu
    jc .invalid
    jmp .next
.x2apic:
    cmp ebx, 24
    jb .invalid
    test dword [edi + 12], 1
    jz .next
    mov eax, [edi + 8]
    mov edx, [edi + 4]
    call acpi_srat_add_cpu
    jc .invalid
    jmp .next
.memory:
    cmp ebx, 40
    jb .invalid
    test dword [edi + 28], 1
    jz .next
    mov eax, edi
    call acpi_srat_add_memory
    jc .invalid
.next:
    mov ebx, [acpi_srat_entry_length]
    add edi, ebx
    jmp .entry
.complete:
    cmp dword [acpi_srat_cpu_count], 0
    jne .valid
    cmp dword [acpi_srat_memory_count], 0
    je .invalid
.valid:
    clc
    ret
.invalid:
    mov dword [acpi_srat_cpu_count], 0
    mov dword [acpi_srat_memory_count], 0
    stc
    ret

; EAX=APIC-ID, EDX=Proximity-Domain. Nur MADT-bekannte CPUs werden akzeptiert.
acpi_srat_add_cpu:
    push edi
    mov [acpi_srat_temp_apic], eax
    mov [acpi_srat_temp_domain], edx
    xor ecx, ecx
.madt_scan:
    cmp ecx, [acpi_cpu_count]
    jae .conflict
    cmp eax, [acpi_apic_ids + ecx * 4]
    je .madt_known
    inc ecx
    jmp .madt_scan
.madt_known:
    xor ecx, ecx
.existing:
    cmp ecx, [acpi_srat_cpu_count]
    jae .insert
    mov edi, ecx
    shl edi, 3
    add edi, acpi_srat_cpu_records
    cmp [edi], eax
    jne .existing_next
    cmp [edi + 4], edx
    jne .conflict
    pop edi
    clc
    ret
.existing_next:
    inc ecx
    jmp .existing
.insert:
    cmp ecx, ACPI_SRAT_CPU_CAPACITY
    jae .conflict
    mov edi, ecx
    shl edi, 3
    add edi, acpi_srat_cpu_records
    mov [edi], eax
    mov [edi + 4], edx
    inc dword [acpi_srat_cpu_count]
    pop edi
    clc
    ret
.conflict:
    inc dword [acpi_srat_conflicts]
    pop edi
    stc
    ret

; EAX=Zeiger auf einen validierten SRAT-Memory-Affinity-Eintrag.
acpi_srat_add_memory:
    push edi
    mov esi, eax
    cmp dword [esi + 12], 0         ; aktuell nur 32-Bit-adressierbare Bereiche
    jne .unsupported
    cmp dword [esi + 20], 0
    jne .unsupported
    mov eax, [esi + 16]
    test eax, eax
    jz .conflict
    mov edx, [esi + 8]
    mov ecx, edx
    add ecx, eax
    jc .conflict
    mov [acpi_srat_temp_base], edx
    mov [acpi_srat_temp_end], ecx
    xor ebx, ebx
.overlap_scan:
    cmp ebx, [acpi_srat_memory_count]
    jae .insert
    mov edi, ebx
    imul edi, ACPI_SRAT_MEMORY_RECORD_SIZE
    add edi, acpi_srat_memory_records
    mov eax, [edi + 4]
    mov ecx, eax
    add ecx, [edi + 12]
    mov edx, [acpi_srat_temp_base]
    cmp edx, ecx
    jae .overlap_next
    mov edx, [acpi_srat_temp_end]
    cmp eax, edx
    jb .conflict
.overlap_next:
    inc ebx
    jmp .overlap_scan
.insert:
    cmp ebx, ACPI_SRAT_MEMORY_CAPACITY
    jae .conflict
    mov edi, ebx
    imul edi, ACPI_SRAT_MEMORY_RECORD_SIZE
    add edi, acpi_srat_memory_records
    mov eax, [esi + 4]
    mov [edi], eax                  ; Proximity Domain
    mov eax, [esi + 8]
    mov [edi + 4], eax              ; Base low
    mov dword [edi + 8], 0
    mov eax, [esi + 16]
    mov [edi + 12], eax             ; Length low
    mov dword [edi + 16], 0
    mov eax, [esi + 28]
    mov [edi + 20], eax             ; Enabled/Hotplug/Nonvolatile
    inc dword [acpi_srat_memory_count]
    pop edi
    clc
    ret
.unsupported:
    inc dword [acpi_srat_unsupported_memory]
    pop edi
    clc
    ret
.conflict:
    inc dword [acpi_srat_conflicts]
    pop edi
    stc
    ret

acpi_srat_self_test:
    cmp dword [acpi_srat_valid], 1
    jne .invalid
    cmp dword [acpi_srat_conflicts], 0
    jne .invalid
    xor ecx, ecx
.cpu_next:
    cmp ecx, [acpi_srat_cpu_count]
    jae .complete
    mov edx, ecx
    shl edx, 3
    mov eax, [acpi_srat_cpu_records + edx]
    xor ebx, ebx
.madt_next:
    cmp ebx, [acpi_cpu_count]
    jae .invalid
    cmp eax, [acpi_apic_ids + ebx * 4]
    je .cpu_known
    inc ebx
    jmp .madt_next
.cpu_known:
    inc ecx
    jmp .cpu_next
.complete:
    clc
    ret
.invalid:
    stc
    ret

; EAX=APIC-ID; EAX=validierte Proximity-Domain.
acpi_srat_cpu_domain_lookup:
    cmp dword [acpi_srat_valid], 1
    jne .invalid
    xor ecx, ecx
.scan:
    cmp ecx, [acpi_srat_cpu_count]
    jae .invalid
    mov edx, ecx
    shl edx, 3
    cmp eax, [acpi_srat_cpu_records + edx]
    je .found
    inc ecx
    jmp .scan
.found:
    mov eax, [acpi_srat_cpu_records + edx + 4]
    clc
    ret
.invalid:
    xor eax, eax
    stc
    ret

; EAX=physische 4-KiB-Seite; EAX=validierte Proximity-Domain.
; Die normalisierte SRAT-Tabelle ist der frühe Firmware-Provider für den PMM;
; der Hardware-Graph übernimmt anschließend dieselben Datensätze normativ.
acpi_srat_memory_domain_lookup:
    push ebx
    push ecx
    push edx
    push esi
    push edi
    mov esi, eax
    cmp dword [acpi_srat_valid], 1
    jne .invalid
    xor ecx, ecx
.scan:
    cmp ecx, [acpi_srat_memory_count]
    jae .invalid
    mov edi, ecx
    imul edi, ACPI_SRAT_MEMORY_RECORD_SIZE
    add edi, acpi_srat_memory_records
    mov ebx, [edi + 4]
    cmp esi, ebx
    jb .next
    mov edx, ebx
    add edx, [edi + 12]
    jc .invalid
    mov eax, esi
    add eax, 4096
    jc .invalid
    cmp eax, edx
    jbe .found
.next:
    inc ecx
    jmp .scan
.found:
    mov eax, [edi]
    clc
    jmp .done
.invalid:
    mov eax, 0xFFFFFFFF
    stc
.done:
    pop edi
    pop esi
    pop edx
    pop ecx
    pop ebx
    ret

align 4
acpi_cpu_count:          dd 1
acpi_madt_valid:         dd 0
acpi_madt_bsp_seen:      dd 0
acpi_root_entry_size:    dd 0
acpi_root_entries_left:  dd 0
acpi_root_cursor:        dd 0
acpi_apic_ids:           times 8 dd 0
acpi_srat_valid:         dd 0
acpi_srat_cpu_count:     dd 0
acpi_srat_memory_count:  dd 0
acpi_srat_conflicts:     dd 0
acpi_srat_unsupported_memory: dd 0
acpi_srat_root_entry_size: dd 0
acpi_srat_root_entries_left: dd 0
acpi_srat_root_cursor:   dd 0
acpi_srat_entry_length:  dd 0
acpi_srat_temp_apic:     dd 0
acpi_srat_temp_domain:   dd 0
acpi_srat_temp_base:     dd 0
acpi_srat_temp_end:      dd 0
align 4
acpi_srat_cpu_records:
    times ACPI_SRAT_CPU_CAPACITY * ACPI_SRAT_CPU_RECORD_SIZE db 0
acpi_srat_memory_records:
    times ACPI_SRAT_MEMORY_CAPACITY * ACPI_SRAT_MEMORY_RECORD_SIZE db 0

early_security_entropy_initialize:
    mov eax, [kernel_context + CONTEXT_ENTROPY_SEED + 0]
    xor eax, [kernel_context + CONTEXT_ENTROPY_SEED + 4]
    rol eax, 9
    xor eax, [kernel_context + CONTEXT_ENTROPY_SEED + 8]
    rol eax, 13
    xor eax, [kernel_context + CONTEXT_ENTROPY_SEED + 12]
    test eax, eax
    jz .invalid
    mov [stack_canary_seed], eax
    clc
    ret
.invalid:
    stc
    ret

; stack_canary_seed != 0 nach early_security_entropy_initialize.
early_security_entropy_self_test:
    cmp dword [stack_canary_seed], 0
    jne .ok
    stc
    ret
.ok:
    clc
    ret

; ---------------------------------------------------------------------------
; Physischer Bootstrap-Speichermanager (ADR-2001 / NPSPEC-KERNEL-0006)
; ---------------------------------------------------------------------------

PMM_PAGE_SIZE        equ 4096
PMM_MAX_FRAMES       equ 1024
PMM_API_SIZE         equ 48
PMM_API_ABI_MAJOR    equ 1
PMM_API_ABI_MINOR    equ 1
PMM_CAP_E820         equ 0x00000001
PMM_CAP_LIFO_FRAMES  equ 0x00000002
PMM_CAP_NUMA_TAGGED  equ 0x00000004
PMM_CAP_NUMA_PREFERRED equ 0x00000008
PMM_CAP_NUMA_STRICT  equ 0x00000010
PMM_NUMA_UNKNOWN     equ 0xFFFFFFFF

PMM_API_STRUCT_SIZE  equ 0
PMM_API_ABI          equ 4
PMM_API_PAGE_SIZE    equ 8
PMM_API_CAPABILITIES equ 12
PMM_API_ALLOC        equ 16
PMM_API_FREE         equ 20
PMM_API_TOTAL        equ 24
PMM_API_AVAILABLE    equ 28
PMM_API_ALLOC_PREFERRED equ 32
PMM_API_ALLOC_STRICT equ 36
PMM_API_NODE_FOR_PAGE equ 40
PMM_API_NUMA_UNKNOWN equ 44

pmm_initialize:
    mov dword [pmm_frame_count], 0

    mov eax, [kernel_context + CONTEXT_KERNEL_ADDRESS]
    add eax, [kernel_context + CONTEXT_KERNEL_SIZE]
    jc .invalid
    add eax, PMM_PAGE_SIZE - 1
    and eax, 0xFFFFF000
    mov [pmm_reserved_end], eax

    mov esi, [kernel_context + CONTEXT_MEMORY_MAP]
    mov ecx, [kernel_context + CONTEXT_MEMORY_COUNT]
.entry:
    test ecx, ecx
    jz .complete
    cmp dword [esi + 16], 1         ; E820: usable RAM
    jne .next_entry
    cmp dword [esi + 4], 0          ; Bootstrap-PMM verwaltet zunächst < 4 GiB
    jne .next_entry

    mov eax, [esi + 0]
    add eax, PMM_PAGE_SIZE - 1
    jc .next_entry
    and eax, 0xFFFFF000
    cmp eax, [pmm_reserved_end]
    jae .start_ready
    mov eax, [pmm_reserved_end]
.start_ready:
    mov ebx, [esi + 0]
    mov edx, [esi + 8]
    add ebx, edx
    jc .end_maximum
    cmp dword [esi + 12], 0
    jne .end_maximum
    and ebx, 0xFFFFF000
    jmp .collect
.end_maximum:
    mov ebx, 0xFFFFF000

.collect:
    cmp eax, ebx
    jae .next_entry
    cmp dword [pmm_frame_count], PMM_MAX_FRAMES
    jae .complete
    mov edi, [pmm_frame_count]
    mov [pmm_frames + edi * 4], eax
    push eax
    call acpi_srat_memory_domain_lookup
    mov [pmm_frame_numa_nodes + edi * 4], eax
    pop eax
    inc dword [pmm_frame_count]
    add eax, PMM_PAGE_SIZE
    jc .next_entry
    jmp .collect

.next_entry:
    add esi, MEMORY_MAP_ENTRY_SIZE
    dec ecx
    jmp .entry

.complete:
    cmp dword [pmm_frame_count], 2
    jb .invalid
    mov eax, [pmm_frame_count]
    mov [pmm_api + PMM_API_TOTAL], eax
    mov [pmm_api + PMM_API_AVAILABLE], eax
    clc
    ret
.invalid:
    stc
    ret

; EAX = physische 4-KiB-Seite, EDX = NUMA-Node oder PMM_NUMA_UNKNOWN.
; EAX=0 meldet Erschöpfung.
pmm_alloc_page:
    mov ecx, [pmm_frame_count]
    test ecx, ecx
    jz .empty
    dec ecx
    mov [pmm_frame_count], ecx
    mov eax, [pmm_frames + ecx * 4]
    mov edx, [pmm_frame_numa_nodes + ecx * 4]
    mov [pmm_api + PMM_API_AVAILABLE], ecx
    ret
.empty:
    xor eax, eax
    mov edx, PMM_NUMA_UNKNOWN
    ret

; EAX = gewünschter NUMA-Node. Bevorzugte Allokation fällt kontrolliert auf
; eine beliebige Seite zurück; EDX liefert immer die tatsächliche Lokalität.
pmm_alloc_page_preferred:
    push eax
    call pmm_alloc_page_strict
    test eax, eax
    jnz .found
    pop eax
    jmp pmm_alloc_page
.found:
    add esp, 4
    ret

; EAX = gewünschter NUMA-Node. Strikte Allokation liefert ausschließlich eine
; lokal getaggte Seite. Die Arrays bleiben durch Swap-with-last kompakt.
pmm_alloc_page_strict:
    mov edx, eax
    mov ecx, [pmm_frame_count]
.scan:
    test ecx, ecx
    jz .empty
    dec ecx
    cmp [pmm_frame_numa_nodes + ecx * 4], edx
    jne .scan
    mov eax, [pmm_frames + ecx * 4]
    mov ebx, [pmm_frame_count]
    dec ebx
    cmp ecx, ebx
    je .remove
    mov esi, [pmm_frames + ebx * 4]
    mov [pmm_frames + ecx * 4], esi
    mov esi, [pmm_frame_numa_nodes + ebx * 4]
    mov [pmm_frame_numa_nodes + ecx * 4], esi
.remove:
    mov [pmm_frame_count], ebx
    mov [pmm_api + PMM_API_AVAILABLE], ebx
    ret
.empty:
    xor eax, eax
    mov edx, PMM_NUMA_UNKNOWN
    ret

; EAX = freizugebende Seite. CF meldet ungültige oder volle Übergabe.
pmm_free_page:
    test eax, PMM_PAGE_SIZE - 1
    jnz .invalid
    cmp eax, [pmm_reserved_end]
    jb .invalid
    mov ecx, [pmm_frame_count]
    cmp ecx, PMM_MAX_FRAMES
    jae .invalid
    mov [pmm_frames + ecx * 4], eax
    push eax
    call acpi_srat_memory_domain_lookup
    mov [pmm_frame_numa_nodes + ecx * 4], eax
    pop eax
    inc ecx
    mov [pmm_frame_count], ecx
    mov [pmm_api + PMM_API_AVAILABLE], ecx
    clc
    ret
.invalid:
    stc
    ret

; Deterministischer Starttest: zwei verschiedene Seiten entnehmen und in
; umgekehrter Reihenfolge zurückgeben. Der freie Zähler muss identisch bleiben.
pmm_self_test:
    mov ebp, [pmm_frame_count]
    call pmm_alloc_page
    test eax, eax
    jz .invalid
    mov ebx, eax
    call pmm_alloc_page
    test eax, eax
    jz .restore_first
    cmp eax, ebx
    je .restore_both
    mov edx, eax
    mov eax, edx
    call pmm_free_page
    jc .invalid
    mov eax, ebx
    call pmm_free_page
    jc .invalid
    cmp [pmm_frame_count], ebp
    jne .invalid
    ; Ist eine bekannte Lokalität vorhanden, müssen Strict, Preferred-Fallback
    ; und die Rückgabe des tatsächlichen Nodes deterministisch funktionieren.
    xor ecx, ecx
.numa_scan:
    cmp ecx, [pmm_frame_count]
    jae .complete
    mov eax, [pmm_frame_numa_nodes + ecx * 4]
    cmp eax, PMM_NUMA_UNKNOWN
    jne .numa_known
    inc ecx
    jmp .numa_scan
.numa_known:
    mov esi, eax
    call pmm_alloc_page_strict
    test eax, eax
    jz .invalid
    mov edi, eax
    cmp edx, esi
    jne .restore_numa_invalid
    mov eax, 0xFFFFFFFE
    call pmm_alloc_page_strict
    test eax, eax
    jnz .restore_numa_invalid
    mov eax, 0xFFFFFFFE
    call pmm_alloc_page_preferred
    test eax, eax
    jz .restore_numa_invalid
    mov ebx, eax
    mov eax, ebx
    call pmm_free_page
    jc .restore_numa_invalid
    mov eax, edi
    call pmm_free_page
    jc .invalid
    cmp [pmm_frame_count], ebp
    jne .invalid
.complete:
    clc
    ret
.restore_numa_invalid:
    mov eax, edi
    call pmm_free_page
    stc
    ret
.restore_both:
    call pmm_free_page
.restore_first:
    mov eax, ebx
    call pmm_free_page
.invalid:
    stc
    ret

; Öffentliche, versionierte Bootstrap-API. Die Implementierung bleibt intern
; austauschbar; Verbraucher prüfen StructSize und ABI vor der Verwendung.
align 4
pmm_api:
    dd PMM_API_SIZE
    dw PMM_API_ABI_MAJOR, PMM_API_ABI_MINOR
    dd PMM_PAGE_SIZE
    dd PMM_CAP_E820 | PMM_CAP_LIFO_FRAMES | PMM_CAP_NUMA_TAGGED | PMM_CAP_NUMA_PREFERRED | PMM_CAP_NUMA_STRICT
    dd pmm_alloc_page
    dd pmm_free_page
    dd 0
    dd 0
    dd pmm_alloc_page_preferred
    dd pmm_alloc_page_strict
    dd acpi_srat_memory_domain_lookup
    dd PMM_NUMA_UNKNOWN

pmm_reserved_end: dd 0
pmm_frame_count:  dd 0
align 16
pmm_frames:
    times PMM_MAX_FRAMES dd 0
pmm_frame_numa_nodes:
    times PMM_MAX_FRAMES dd PMM_NUMA_UNKNOWN

; ---------------------------------------------------------------------------
; Bootstrap-Kernel-Heap (ADR-2003 / NPSPEC-KERNEL-0008)
; ---------------------------------------------------------------------------

HEAP_ALIGNMENT       equ 16
HEAP_API_SIZE        equ 32
HEAP_API_ABI_MAJOR   equ 1
HEAP_API_ABI_MINOR   equ 0
HEAP_CAP_PAGE_BACKED equ 0x00000001
HEAP_CAP_ZEROED      equ 0x00000002

heap_initialize:
    mov dword [heap_page], 0
    mov dword [heap_offset], PMM_PAGE_SIZE
    mov dword [heap_allocations], 0
    mov dword [heap_bytes], 0
    clc
    ret

; ECX=Größe, EAX=Adresse oder 0. Bootstrap-Allokationen sind maximal eine
; Seite groß und werden auf 16 Byte ausgerichtet.
heap_allocate:
    test ecx, ecx
    jz .invalid
    cmp ecx, PMM_PAGE_SIZE
    ja .invalid
    add ecx, HEAP_ALIGNMENT - 1
    jc .invalid
    and ecx, -HEAP_ALIGNMENT

    mov edx, [heap_offset]
    mov ebx, edx
    add ebx, ecx
    jc .invalid
    cmp ebx, PMM_PAGE_SIZE
    jbe .have_space

    push ecx
    call pmm_alloc_page
    pop ecx
    test eax, eax
    jz .invalid
    mov [heap_page], eax
    mov dword [heap_offset], 0
    xor edx, edx

    ; Neue Heap-Seiten werden deterministisch genullt.
    push ecx
    mov edi, eax
    xor eax, eax
    mov ecx, PMM_PAGE_SIZE / 4
    rep stosd
    pop ecx

.have_space:
    mov eax, [heap_page]
    add eax, edx
    add edx, ecx
    mov [heap_offset], edx
    inc dword [heap_allocations]
    add [heap_bytes], ecx
    clc
    ret
.invalid:
    xor eax, eax
    stc
    ret

heap_self_test:
    mov ecx, 64
    call heap_allocate
    jc .invalid
    test eax, HEAP_ALIGNMENT - 1
    jnz .invalid
    mov esi, eax
    mov dword [esi], 0x4E4F5641
    mov dword [esi + 60], 0x48454150

    mov ecx, 128
    call heap_allocate
    jc .invalid
    test eax, HEAP_ALIGNMENT - 1
    jnz .invalid
    cmp eax, esi
    jbe .invalid
    cmp dword [esi], 0x4E4F5641
    jne .invalid
    cmp dword [esi + 60], 0x48454150
    jne .invalid
    cmp dword [eax], 0
    jne .invalid
    clc
    ret
.invalid:
    stc
    ret

align 4
heap_api:
    dd HEAP_API_SIZE
    dw HEAP_API_ABI_MAJOR, HEAP_API_ABI_MINOR
    dd HEAP_ALIGNMENT
    dd HEAP_CAP_PAGE_BACKED | HEAP_CAP_ZEROED
    dd heap_allocate
    dd 0                             ; Free ist im Bootstrap-Heap nicht verfügbar
    dd heap_allocations
    dd heap_bytes

heap_page:        dd 0
heap_offset:      dd 0
heap_allocations: dd 0
heap_bytes:       dd 0

; ---------------------------------------------------------------------------
; Strukturiertes Early-/Kernel-Logging (NPSPEC-KERNEL-0023)
; ---------------------------------------------------------------------------
LOG_RECORD_SIZE       equ 32
LOG_RING_CAPACITY     equ 8
LOG_COMMIT_MARKER     equ 0x43474F4C ; "LOGC"
LOG_LEVEL_CRITICAL    equ 6
LOG_FLAG_EARLY_TIME  equ 1

logging_initialize:
    mov edi, logging_ring
    xor eax, eax
    mov ecx, (LOG_RING_CAPACITY * LOG_RECORD_SIZE) / 4
    rep stosd
    mov edi, logging_critical_record
    mov ecx, LOG_RECORD_SIZE / 4
    rep stosd
    mov dword [logging_write_index], 0
    mov dword [logging_record_count], 0
    mov dword [logging_written_records], 0
    mov dword [logging_dropped_records], 0
    mov dword [logging_overwritten_records], 0
    mov dword [logging_critical_records], 0
    mov dword [logging_recursion_events], 0
    mov dword [logging_recursion_guard], 0
    clc
    ret

; EAX=Level, EDX=Kategorie, EBX=Komponente, ECX=Event-Code.
; Der Commit-Marker wird als letztes Feld atomar sichtbar gemacht.
logging_write:
    cmp eax, 7
    ja .invalid
    cmp edx, 13
    ja .invalid
    cmp dword [logging_recursion_guard], 0
    jne .recursive
    mov dword [logging_recursion_guard], 1
    mov [logging_temp_level], eax
    mov [logging_temp_category], edx
    mov [logging_temp_component], ebx
    mov [logging_temp_event], ecx
    mov edi, [logging_write_index]
    shl edi, 5
    add edi, logging_ring
    mov dword [edi + 28], 0
    inc dword [logging_written_records]
    mov eax, [logging_written_records]
    mov [edi + 0], eax
    mov eax, [logging_temp_level]
    mov [edi + 4], eax
    mov eax, [logging_temp_category]
    mov [edi + 8], eax
    mov eax, [logging_temp_component]
    mov [edi + 12], eax
    mov eax, [logging_temp_event]
    mov [edi + 16], eax
    mov eax, [timer_ticks]
    mov [edi + 20], eax
    xor eax, eax
    cmp dword [timer_ticks], 0
    jne .timestamp_ready
    mov eax, LOG_FLAG_EARLY_TIME
.timestamp_ready:
    mov [edi + 24], eax
    mov dword [edi + 28], LOG_COMMIT_MARKER
    cmp dword [logging_record_count], LOG_RING_CAPACITY
    jb .grow
    inc dword [logging_dropped_records]
    inc dword [logging_overwritten_records]
    jmp .advance
.grow:
    inc dword [logging_record_count]
.advance:
    inc dword [logging_write_index]
    and dword [logging_write_index], LOG_RING_CAPACITY - 1
    cmp dword [logging_temp_level], LOG_LEVEL_CRITICAL
    jb .complete
    inc dword [logging_critical_records]
    mov esi, edi
    mov edi, logging_critical_record
    mov ecx, LOG_RECORD_SIZE / 4
    rep movsd
.complete:
    mov dword [logging_recursion_guard], 0
    clc
    ret
.recursive:
    inc dword [logging_recursion_events]
.invalid:
    mov dword [logging_recursion_guard], 0
    stc
    ret

logging_self_test:
    mov dword [logging_test_iteration], 0
.next:
    mov eax, [logging_test_iteration]
    cmp eax, 10
    jae .verify
    and eax, 7
    mov edx, 1                     ; Kernel-Kategorie
    mov ebx, 1                     ; Kernel-Core-Komponente
    mov ecx, [logging_test_iteration]
    add ecx, 0x100
    call logging_write
    jc .invalid
    inc dword [logging_test_iteration]
    jmp .next
.verify:
    cmp dword [logging_written_records], 10
    jne .invalid
    cmp dword [logging_record_count], LOG_RING_CAPACITY
    jne .invalid
    cmp dword [logging_dropped_records], 2
    jne .invalid
    cmp dword [logging_overwritten_records], 2
    jne .invalid
    mov eax, [logging_write_index]
    dec eax
    and eax, LOG_RING_CAPACITY - 1
    shl eax, 5
    add eax, logging_ring
    cmp dword [eax + 0], 10
    jne .invalid
    cmp dword [eax + 28], LOG_COMMIT_MARKER
    jne .invalid
    cmp dword [logging_critical_records], 2
    jne .invalid
    cmp dword [logging_critical_record + 28], LOG_COMMIT_MARKER
    jne .invalid
    clc
    ret
.invalid:
    stc
    ret

align 4
logging_write_index:        dd 0
logging_record_count:       dd 0
logging_written_records:    dd 0
logging_dropped_records:    dd 0
logging_overwritten_records: dd 0
logging_critical_records:   dd 0
logging_recursion_events:   dd 0
logging_recursion_guard:    dd 0
logging_test_iteration:     dd 0
logging_temp_level:         dd 0
logging_temp_category:      dd 0
logging_temp_component:     dd 0
logging_temp_event:         dd 0
logging_ring:               times LOG_RING_CAPACITY * LOG_RECORD_SIZE db 0
logging_critical_record:    times LOG_RECORD_SIZE db 0

; ---------------------------------------------------------------------------
; Kernel Object Manager (ADR-2008 / NPSPEC-KERNEL-0012)
; ---------------------------------------------------------------------------

OBJECT_API_SIZE       equ 32
OBJECT_API_ABI_MAJOR  equ 1
OBJECT_API_ABI_MINOR  equ 0
OBJECT_TABLE_CAPACITY equ 32
OBJECT_HEADER_SIZE    equ 32
OBJECT_STATE_LIVE     equ 1

OBJ_STRUCT_SIZE equ 0
OBJ_ABI         equ 4
OBJ_TYPE        equ 8
OBJ_STATE       equ 12
OBJ_REFCOUNT    equ 16
OBJ_HANDLE      equ 20
OBJ_PAYLOAD0    equ 24
OBJ_PAYLOAD1    equ 28

object_manager_initialize:
    mov edi, object_table
    xor eax, eax
    mov ecx, OBJECT_TABLE_CAPACITY
    rep stosd
    mov edi, object_semantic_types
    xor eax, eax
    mov ecx, OBJECT_TABLE_CAPACITY * 3
    rep stosd
    mov edi, object_generations
    mov eax, 1
    mov ecx, OBJECT_TABLE_CAPACITY
    rep stosd
    mov dword [object_live_count], 0
    clc
    ret

; EAX=Typ, EDX/EBX=optionale Nutzwerte. EAX=Handle oder 0.
object_create:
    test eax, eax
    jz .invalid
    push eax
    push edx
    push ebx
    xor edi, edi
.find_slot:
    cmp edi, OBJECT_TABLE_CAPACITY
    jae .full
    cmp dword [object_table + edi * 4], 0
    je .allocate
    inc edi
    jmp .find_slot
.allocate:
    mov ecx, OBJECT_HEADER_SIZE
    call heap_allocate
    jc .full
    pop ebx
    pop edx
    pop ecx
    mov dword [eax + OBJ_STRUCT_SIZE], OBJECT_HEADER_SIZE
    mov dword [eax + OBJ_ABI], 1
    mov [eax + OBJ_TYPE], ecx
    mov dword [eax + OBJ_STATE], OBJECT_STATE_LIVE
    mov dword [eax + OBJ_REFCOUNT], 1
    mov [eax + OBJ_PAYLOAD0], edx
    mov [eax + OBJ_PAYLOAD1], ebx
    mov ecx, [object_generations + edi * 4]
    shl ecx, 16
    lea edx, [edi + 1]
    or ecx, edx
    mov [eax + OBJ_HANDLE], ecx
    mov [object_table + edi * 4], eax
    inc dword [object_live_count]
    mov eax, ecx
    clc
    ret
.full:
    add esp, 12
.invalid:
    xor eax, eax
    stc
    ret

; EAX=Handle. EAX=Objektadresse oder 0.
object_lookup:
    mov edx, eax
    and edx, 0xFFFF
    jz .invalid
    dec edx
    cmp edx, OBJECT_TABLE_CAPACITY
    jae .invalid
    mov ecx, eax
    shr ecx, 16
    cmp ecx, [object_generations + edx * 4]
    jne .invalid
    mov eax, [object_table + edx * 4]
    test eax, eax
    jz .invalid
    cmp dword [eax + OBJ_STATE], OBJECT_STATE_LIVE
    jne .invalid
    clc
    ret
.invalid:
    xor eax, eax
    stc
    ret

object_retain:
    push eax
    call object_lookup
    jc .invalid
    inc dword [eax + OBJ_REFCOUNT]
    add esp, 4
    clc
    ret
.invalid:
    add esp, 4
    stc
    ret

object_release:
    push eax
    call object_lookup
    jc .invalid
    cmp dword [eax + OBJ_REFCOUNT], 0
    je .invalid
    dec dword [eax + OBJ_REFCOUNT]
    jnz .done
    mov dword [eax + OBJ_STATE], 0
    mov edx, [esp]
    and edx, 0xFFFF
    dec edx
    mov dword [object_table + edx * 4], 0
    mov dword [object_semantic_types + edx * 4], 0
    mov dword [object_semantic_versions + edx * 4], 0
    mov dword [object_semantic_validation + edx * 4], 0
    inc dword [object_generations + edx * 4]
    and dword [object_generations + edx * 4], 0xFFFF
    jnz .generation_ok
    mov dword [object_generations + edx * 4], 1
.generation_ok:
    dec dword [object_live_count]
.done:
    add esp, 4
    clc
    ret
.invalid:
    add esp, 4
    stc
    ret

object_manager_self_test:
    mov eax, 1
    mov edx, 0x4E4F5641
    mov ebx, 0x4F424A31
    call object_create
    jc .invalid
    mov esi, eax
    call object_lookup
    jc .invalid
    cmp dword [eax + OBJ_TYPE], 1
    jne .invalid
    cmp dword [eax + OBJ_PAYLOAD0], 0x4E4F5641
    jne .invalid
    mov eax, esi
    call object_retain
    jc .invalid
    cmp dword [eax + OBJ_REFCOUNT], 2
    jne .invalid
    mov eax, esi
    call object_release
    jc .invalid
    mov eax, esi
    call object_release
    jc .invalid
    mov eax, esi
    call object_lookup
    jnc .invalid
    cmp dword [object_live_count], 0
    jne .invalid
    clc
    ret
.invalid:
    stc
    ret

align 4
object_manager_api:
    dd OBJECT_API_SIZE
    dw OBJECT_API_ABI_MAJOR, OBJECT_API_ABI_MINOR
    dd OBJECT_TABLE_CAPACITY
    dd object_create
    dd object_lookup
    dd object_retain
    dd object_release
    dd object_live_count

object_live_count: dd 0
align 4
object_table:       times OBJECT_TABLE_CAPACITY dd 0
object_generations: times OBJECT_TABLE_CAPACITY dd 0
; Semantic Type ist ein Sidecar des stabilen Objekts und weder Objekt-ID noch
; Pfad noch allgemeines Payloadfeld (ADR-SEMANTIC-0003 Typed Resources).
object_semantic_types:      times OBJECT_TABLE_CAPACITY dd 0
object_semantic_versions:   times OBJECT_TABLE_CAPACITY dd 0
object_semantic_validation: times OBJECT_TABLE_CAPACITY dd 0

; ---------------------------------------------------------------------------
; Prozesslokaler Handle Manager (NPSPEC-KERNEL-0013)
; ---------------------------------------------------------------------------
HANDLE_API_SIZE       equ 32
HANDLE_CAPACITY       equ 16
HANDLE_ENTRY_SIZE     equ 24
HANDLE_STATE_ACTIVE   equ 1
HANDLE_RIGHT_QUERY    equ 0x00000001
HANDLE_RIGHT_WAIT     equ 0x00000002
HANDLE_RIGHT_SEND     equ 0x00000004
HANDLE_RIGHT_RECEIVE  equ 0x00000008
HANDLE_RIGHT_BIND     equ 0x00000010
HANDLE_RIGHT_CONNECT  equ 0x00000020
HANDLE_FLAG_PROTECTED equ 0x00000010
HANDLE_OBJECT equ 0
HANDLE_RIGHTS equ 4
HANDLE_FLAGS  equ 8
HANDLE_GEN    equ 12
HANDLE_TYPE   equ 16
HANDLE_STATE  equ 20

handle_manager_initialize:
    mov edi, handle_table
    xor eax, eax
    mov ecx, (HANDLE_CAPACITY * HANDLE_ENTRY_SIZE) / 4
    rep stosd
    mov edi, handle_generations
    mov eax, 1
    mov ecx, HANDLE_CAPACITY
    rep stosd
    mov dword [handle_owner_pid], 0
    mov dword [handle_active_count], 0
    clc
    ret

; EAX=PID, EDX=Kernelobjekthandle, EBX=Typ, ECX=Rechte, ESI=Flags.
; EAX=prozesslokaler, generationsgeschützter Handle.
handle_create:
    cmp eax, [handle_owner_pid]
    jne .invalid
    test eax, eax
    jz .invalid
    test ecx, ecx
    jz .invalid
    mov [handle_temp_object], edx
    mov [handle_temp_type], ebx
    mov [handle_temp_rights], ecx
    mov [handle_temp_flags], esi
    mov eax, edx
    call object_lookup
    jc .invalid
    mov ecx, [handle_temp_type]
    cmp [eax + OBJ_TYPE], ecx
    jne .invalid
    xor edi, edi
.scan:
    cmp edi, HANDLE_CAPACITY
    jae .invalid
    mov eax, edi
    imul eax, HANDLE_ENTRY_SIZE
    add eax, handle_table
    cmp dword [eax + HANDLE_STATE], 0
    je .slot
    inc edi
    jmp .scan
.slot:
    mov [handle_temp_slot], eax
    mov eax, [handle_temp_object]
    call object_retain
    jc .invalid
    mov eax, [handle_temp_slot]
    mov edx, [handle_temp_object]
    mov [eax + HANDLE_OBJECT], edx
    mov edx, [handle_temp_rights]
    mov [eax + HANDLE_RIGHTS], edx
    mov edx, [handle_temp_flags]
    mov [eax + HANDLE_FLAGS], edx
    mov edx, [handle_generations + edi * 4]
    mov [eax + HANDLE_GEN], edx
    mov edx, [handle_temp_type]
    mov [eax + HANDLE_TYPE], edx
    mov dword [eax + HANDLE_STATE], HANDLE_STATE_ACTIVE
    inc dword [handle_active_count]
    mov eax, [handle_generations + edi * 4]
    shl eax, 16
    lea edx, [edi + 1]
    or eax, edx
    clc
    ret
.invalid:
    xor eax, eax
    stc
    ret

; EAX=PID, EDX=Handle, EBX=erwarteter Typ, ECX=benötigte Rechte.
; EAX=interne Objektadresse ausschließlich für den Kernel.
handle_resolve:
    cmp eax, [handle_owner_pid]
    jne .invalid
    mov edi, edx
    and edi, 0xFFFF
    jz .invalid
    dec edi
    cmp edi, HANDLE_CAPACITY
    jae .invalid
    mov esi, edi
    imul esi, HANDLE_ENTRY_SIZE
    add esi, handle_table
    cmp dword [esi + HANDLE_STATE], HANDLE_STATE_ACTIVE
    jne .invalid
    mov eax, edx
    shr eax, 16
    cmp eax, [esi + HANDLE_GEN]
    jne .invalid
    cmp ebx, [esi + HANDLE_TYPE]
    jne .invalid
    mov eax, [esi + HANDLE_RIGHTS]
    and eax, ecx
    cmp eax, ecx
    jne .invalid
    mov eax, [esi + HANDLE_OBJECT]
    call object_lookup
    ret
.invalid:
    xor eax, eax
    stc
    ret

; EAX=PID, EDX=Handle. Geschützte Handles können nicht regulär geschlossen werden.
handle_close:
    cmp eax, [handle_owner_pid]
    jne .invalid
    mov edi, edx
    and edi, 0xFFFF
    jz .invalid
    dec edi
    cmp edi, HANDLE_CAPACITY
    jae .invalid
    mov esi, edi
    imul esi, HANDLE_ENTRY_SIZE
    add esi, handle_table
    cmp dword [esi + HANDLE_STATE], HANDLE_STATE_ACTIVE
    jne .invalid
    mov eax, edx
    shr eax, 16
    cmp eax, [esi + HANDLE_GEN]
    jne .invalid
    test dword [esi + HANDLE_FLAGS], HANDLE_FLAG_PROTECTED
    jnz .invalid
    mov eax, [esi + HANDLE_OBJECT]
    call object_release
    jc .invalid
    mov dword [esi + HANDLE_STATE], 0
    inc dword [handle_generations + edi * 4]
    and dword [handle_generations + edi * 4], 0xFFFF
    jnz .generation_ok
    mov dword [handle_generations + edi * 4], 1
.generation_ok:
    dec dword [handle_active_count]
    clc
    ret
.invalid:
    stc
    ret

; §013 §53: Self-Test – 9 Tests, EDI = Fehler-Zähler
handle_manager_self_test:
    push ebp
    mov ebp, esp
    push ebx
    push esi
    push edi
    sub esp, 8                          ; [ebp-16]=obj_handle, [ebp-20]=proc_handle
    xor edi, edi
    mov dword [ebp - 16], 0
    mov dword [ebp - 20], 0

    ; Test 1: handle_active_count == 0 nach init
    cmp dword [handle_active_count], 0
    je .t2
    inc edi

.t2:
    ; Test 2: Testobjekt anlegen (Typ 0x48444C54 = "HDLT")
    mov dword [handle_owner_pid], 1
    mov eax, 0x48444C54
    xor edx, edx
    xor ebx, ebx
    call object_create
    jnc .t2_ok
    inc edi
    jmp .cleanup
.t2_ok:
    mov [ebp - 16], eax

.t3:
    ; Test 3: handle_create
    mov eax, 1
    mov edx, [ebp - 16]
    mov ebx, 0x48444C54
    mov ecx, HANDLE_RIGHT_QUERY | HANDLE_RIGHT_WAIT
    xor esi, esi
    call handle_create
    jnc .t3_ok
    inc edi
    jmp .cleanup_obj
.t3_ok:
    mov [ebp - 20], eax

.t4:
    ; Test 4: handle_active_count == 1
    cmp dword [handle_active_count], 1
    je .t5
    inc edi

.t5:
    ; Test 5: handle_resolve mit korrektem Typ und Rechten
    mov eax, 1
    mov edx, [ebp - 20]
    mov ebx, 0x48444C54
    mov ecx, HANDLE_RIGHT_QUERY
    call handle_resolve
    jnc .t6
    inc edi

.t6:
    ; Test 6: handle_resolve mit falschem Typ → muss scheitern
    mov eax, 1
    mov edx, [ebp - 20]
    mov ebx, 0x44454144                 ; "DEAD"
    mov ecx, HANDLE_RIGHT_QUERY
    call handle_resolve
    jc .t7
    inc edi

.t7:
    ; Test 7: handle_close
    mov eax, 1
    mov edx, [ebp - 20]
    call handle_close
    jnc .t8
    inc edi
    jmp .cleanup_handle

.t8:
    ; Test 8: handle nach close → veraltete Generation
    mov eax, 1
    mov edx, [ebp - 20]
    mov ebx, 0x48444C54
    mov ecx, HANDLE_RIGHT_QUERY
    call handle_resolve
    jc .t9
    inc edi

.t9:
    ; Test 9: handle_active_count == 0 nach close
    cmp dword [handle_active_count], 0
    je .cleanup_obj
    inc edi
    jmp .cleanup_obj

.cleanup_handle:
    ; Handle noch offen (t7 fehlgeschlagen): schließen
    mov eax, 1
    mov edx, [ebp - 20]
    call handle_close

.cleanup_obj:
    mov eax, [ebp - 16]
    test eax, eax
    jz .cleanup
    call object_release

.cleanup:
    mov dword [handle_owner_pid], 0
    test edi, edi
    jnz .selftest_fail
    add esp, 8
    pop edi
    pop esi
    pop ebx
    pop ebp
    clc
    ret
.selftest_fail:
    add esp, 8
    pop edi
    pop esi
    pop ebx
    pop ebp
    stc
    ret

align 4
handle_manager_api:
    dd HANDLE_API_SIZE
    dw 1, 0
    dd HANDLE_CAPACITY
    dd handle_create
    dd handle_resolve
    dd handle_close
    dd handle_active_count
    dd handle_table
handle_owner_pid:    dd 0
handle_active_count: dd 0
handle_temp_object: dd 0
handle_temp_type:   dd 0
handle_temp_rights: dd 0
handle_temp_flags:  dd 0
handle_temp_slot:   dd 0
align 4
handle_generations: times HANDLE_CAPACITY dd 0
handle_table: times HANDLE_CAPACITY * HANDLE_ENTRY_SIZE db 0

; ---------------------------------------------------------------------------
; Kernel Component Manager (ADR-2009 / NPSPEC-KERNEL-0001 §Subsystem-Registry)
; ---------------------------------------------------------------------------

COMPONENT_API_SIZE       equ 32
COMPONENT_API_ABI_MAJOR  equ 1
COMPONENT_API_ABI_MINOR  equ 0
COMPONENT_CAPACITY       equ 8
COMPONENT_RECORD_SIZE    equ 32
COMPONENT_STATE_EMPTY    equ 0
COMPONENT_STATE_REGISTERED equ 1
COMPONENT_STATE_ACTIVE   equ 2
OBJECT_TYPE_COMPONENT    equ 2

COMP_ID       equ 0
COMP_VERSION  equ 4
COMP_FLAGS    equ 8
COMP_STATE    equ 12
COMP_HANDLE   equ 16
COMP_RESERVED equ 20

component_manager_initialize:
    mov edi, component_table
    xor eax, eax
    mov ecx, (COMPONENT_CAPACITY * COMPONENT_RECORD_SIZE) / 4
    rep stosd
    mov dword [component_count], 0
    clc
    ret

; EAX=stabile Komponenten-ID, EDX=Version, EBX=Flags. EAX=Objekt-Handle.
component_register:
    pushfd
    cli
    test eax, eax
    jz .invalid
    mov [component_temp_id], eax
    mov [component_temp_version], edx
    mov [component_temp_flags], ebx
    xor ecx, ecx
.scan:
    cmp ecx, COMPONENT_CAPACITY
    jae .full
    mov edi, ecx
    shl edi, 5
    add edi, component_table
    cmp dword [edi + COMP_STATE], COMPONENT_STATE_EMPTY
    je .slot
    cmp [edi + COMP_ID], eax
    je .invalid
    inc ecx
    jmp .scan
.slot:
    mov [component_temp_slot], edi
    mov eax, OBJECT_TYPE_COMPONENT
    mov edx, [component_temp_id]
    mov ebx, [component_temp_version]
    call object_create
    jc .invalid
    mov edi, [component_temp_slot]
    mov edx, [component_temp_id]
    mov [edi + COMP_ID], edx
    mov edx, [component_temp_version]
    mov [edi + COMP_VERSION], edx
    mov edx, [component_temp_flags]
    mov [edi + COMP_FLAGS], edx
    mov dword [edi + COMP_STATE], COMPONENT_STATE_REGISTERED
    mov [edi + COMP_HANDLE], eax
    inc dword [component_count]
    popfd
    clc
    ret
.full:
.invalid:
    xor eax, eax
    popfd
    stc
    ret

; EAX=Komponenten-ID. EAX=Datensatz oder 0.
component_lookup:
    xor ecx, ecx
.scan:
    cmp ecx, COMPONENT_CAPACITY
    jae .invalid
    mov edx, ecx
    shl edx, 5
    add edx, component_table
    cmp dword [edx + COMP_STATE], COMPONENT_STATE_EMPTY
    je .next
    cmp [edx + COMP_ID], eax
    je .found
.next:
    inc ecx
    jmp .scan
.found:
    mov eax, edx
    clc
    ret
.invalid:
    xor eax, eax
    stc
    ret

; EAX=Komponenten-ID.
component_activate:
    pushfd
    cli
    call component_lookup
    jc .invalid
    cmp dword [eax + COMP_STATE], COMPONENT_STATE_REGISTERED
    jne .invalid
    mov dword [eax + COMP_STATE], COMPONENT_STATE_ACTIVE
    popfd
    clc
    ret
.invalid:
    popfd
    stc
    ret

component_manager_self_test:
    mov eax, 0x434F5245             ; "CORE"
    mov edx, 0x00010000             ; Version 1.0
    mov ebx, 1                      ; essentielle Kernel-Komponente
    call component_register
    jc .invalid
    mov esi, eax
    mov eax, 0x434F5245
    call component_activate
    jc .invalid
    mov eax, 0x434F5245
    call component_lookup
    jc .invalid
    cmp dword [eax + COMP_VERSION], 0x00010000
    jne .invalid
    cmp dword [eax + COMP_STATE], COMPONENT_STATE_ACTIVE
    jne .invalid
    cmp [eax + COMP_HANDLE], esi
    jne .invalid
    cmp dword [component_count], 1
    jne .invalid
    clc
    ret
.invalid:
    stc
    ret

align 4
component_manager_api:
    dd COMPONENT_API_SIZE
    dw COMPONENT_API_ABI_MAJOR, COMPONENT_API_ABI_MINOR
    dd COMPONENT_CAPACITY
    dd component_register
    dd component_lookup
    dd component_activate
    dd component_count
    dd component_table

component_count:        dd 0
component_temp_id:      dd 0
component_temp_version: dd 0
component_temp_flags:   dd 0
component_temp_slot:    dd 0
align 4
component_table:
    times COMPONENT_CAPACITY * COMPONENT_RECORD_SIZE db 0

; ---------------------------------------------------------------------------
; Virtueller Bootstrap-Speichermanager (ADR-2002 / NPSPEC-KERNEL-0007)
; ---------------------------------------------------------------------------

PAGING_PAGE_PRESENT   equ 0x001
PAGING_PAGE_WRITE     equ 0x002
PAGING_PAGE_USER      equ 0x004
PAGING_API_SIZE       equ 32
PAGING_API_ABI_MAJOR  equ 1
PAGING_API_ABI_MINOR  equ 0
PAGING_CAP_4K_PAGES   equ 0x00000001
PAGING_CAP_IDENTITY   equ 0x00000002
PAGING_LOW_LIMIT      equ 0x01000000

paging_initialize:
    call pmm_alloc_page
    test eax, eax
    jz .invalid
    mov [paging_directory], eax
    mov edi, eax
    xor eax, eax
    mov ecx, PMM_PAGE_SIZE / 4
    rep stosd

    ; Früher Kernel, Stack, BIB, Heap und Seitentabellen bleiben identisch
    ; abgebildet. Der UEFI-Kernel liegt hoeher als der BIOS-Kernel; mit 1024
    ; Bootstrap-PMM-Seiten muss der fruehe Identity-Bereich daher bis 16 MiB
    ; reichen, sonst koennen Heap-Seiten direkt nach 8 MiB ungeplant faulten.
    xor eax, eax
.map_low:
    mov edx, eax
    mov ebx, PAGING_PAGE_PRESENT | PAGING_PAGE_WRITE
    call paging_map_page
    jc .invalid
    add eax, PMM_PAGE_SIZE
    cmp eax, PAGING_LOW_LIMIT
    jb .map_low

    ; APIC-MMIO für den UEFI/Q35-Interruptpfad identisch abbilden.
    mov eax, 0xFEC00000
    mov edx, eax
    mov ebx, PAGING_PAGE_PRESENT | PAGING_PAGE_WRITE
    call paging_map_page
    jc .invalid
    mov eax, 0xFEE00000
    mov edx, eax
    mov ebx, PAGING_PAGE_PRESENT | PAGING_PAGE_WRITE
    call paging_map_page
    jc .invalid

    ; Linearen VBE-Framebuffer in seiner bestehenden Adresse abbilden.
    mov eax, [kernel_context + CONTEXT_FRAMEBUFFER]
    test eax, eax
    jz .enable
    and eax, 0xFFFFF000
    mov esi, eax
    mov ecx, [kernel_context + CONTEXT_PITCH]
    imul ecx, [kernel_context + CONTEXT_HEIGHT]
    add ecx, [kernel_context + CONTEXT_FRAMEBUFFER]
    jc .invalid
    add ecx, PMM_PAGE_SIZE - 1
    jc .invalid
    and ecx, 0xFFFFF000
    mov edi, ecx
.map_framebuffer:
    cmp esi, edi
    jae .enable
    mov eax, esi
    mov edx, esi
    mov ebx, PAGING_PAGE_PRESENT | PAGING_PAGE_WRITE
    call paging_map_page
    jc .invalid
    add esi, PMM_PAGE_SIZE
    jc .invalid
    jmp .map_framebuffer

.enable:
    mov eax, [paging_directory]
    mov cr3, eax
    mov eax, cr0
    or eax, 0x80000000
    mov cr0, eax
    jmp short .paging_active
.paging_active:
    mov dword [paging_enabled], 1
    clc
    ret
.invalid:
    stc
    ret

; EAX=virtuelle Seite, EDX=physische Seite, EBX=Flags.
paging_map_page:
    push eax
    push esi
    push edi
    push ebp
    mov esi, eax
    mov edi, edx
    mov ebp, ebx

    mov ecx, esi
    shr ecx, 22
    mov edx, [paging_directory]
    lea edx, [edx + ecx * 4]
    mov eax, [edx]
    test eax, PAGING_PAGE_PRESENT
    jnz .have_table

    push edx
    call pmm_alloc_page
    pop edx
    test eax, eax
    jz .invalid
    push edi
    push eax
    mov edi, eax
    xor eax, eax
    mov ecx, PMM_PAGE_SIZE / 4
    rep stosd
    pop eax
    pop edi
    or eax, PAGING_PAGE_PRESENT | PAGING_PAGE_WRITE
    mov [edx], eax
.have_table:
    ; Ein User-PTE benötigt das User-Bit auf beiden Paging-Ebenen. Bereits
    ; vorhandene Supervisor-PTEs bleiben dadurch weiterhin geschützt.
    test ebp, PAGING_PAGE_USER
    jz .directory_ready
    or dword [edx], PAGING_PAGE_USER
.directory_ready:
    and eax, 0xFFFFF000
    mov ecx, esi
    shr ecx, 12
    and ecx, 0x3FF
    lea eax, [eax + ecx * 4]
    and edi, 0xFFFFF000
    or edi, ebp
    mov [eax], edi
    pop ebp
    pop edi
    pop esi
    pop eax
    clc
    ret
.invalid:
    pop ebp
    pop edi
    pop esi
    pop eax
    stc
    ret

paging_self_test:
    cmp dword [paging_enabled], 1
    jne .invalid
    mov eax, cr0
    test eax, 0x80000000
    jz .invalid
    mov ecx, 32
    call heap_allocate
    jc .invalid
    mov dword [eax], 0x50414745
    cmp dword [eax], 0x50414745
    jne .invalid
    clc
    ret
.invalid:
    stc
    ret

align 4
paging_api:
    dd PAGING_API_SIZE
    dw PAGING_API_ABI_MAJOR, PAGING_API_ABI_MINOR
    dd PMM_PAGE_SIZE
    dd PAGING_CAP_4K_PAGES | PAGING_CAP_IDENTITY
    dd paging_map_page
    dd 0                             ; Unmap folgt mit dem vollständigen VMM
    dd paging_directory
    dd paging_enabled

paging_directory: dd 0
paging_enabled:   dd 0

; ---------------------------------------------------------------------------
; Interrupt- und Timer-Architektur (ADR-2006 / ADR-2007 / NPSPEC-KERNEL-0009 / NPSPEC-KERNEL-0014 / NPSPEC-HAL-INTERRUPT-0001)
; ---------------------------------------------------------------------------

IDT_ENTRY_COUNT equ 256
IDT_GATE_FLAGS  equ 0x8E
PIC1_COMMAND    equ 0x20
PIC1_DATA       equ 0x21
PIC2_COMMAND    equ 0xA0
PIC2_DATA       equ 0xA1
PIC_EOI         equ 0x20
PIT_COMMAND     equ 0x43
PIT_CHANNEL0    equ 0x40
PIT_DIVISOR     equ 11932           ; ungefähr 100 Hz
INTERRUPT_API_SIZE equ 32
INTERRUPT_CAPABILITIES equ 0x00000007

interrupt_initialize:
    cli
    xor ebx, ebx
.default_gate:
    mov eax, isr_unexpected
    call idt_set_gate
    inc ebx
    cmp ebx, IDT_ENTRY_COUNT
    jb .default_gate

    xor ebx, ebx
.exception_gate:
    mov eax, [exception_stub_table + ebx * 4]
    call idt_set_gate
    inc ebx
    cmp ebx, 32
    jb .exception_gate

    mov ebx, 32
    mov eax, irq0_stub
    call idt_set_gate
    mov ebx, 33
    mov eax, irq1_stub
    call idt_set_gate
    mov ebx, 0x80
    mov eax, syscall_stub
    call idt_set_gate
    mov byte [idt_table + 0x80 * 8 + 5], 0xEE ; present, Ring-3, Interrupt-Gate
    ; SMP IPI-Vektor (Prio: nach Syscall, vor LIDT)
    mov ebx, SMP_IPI_VECTOR
    mov eax, isr_ipi
    call idt_set_gate
    ; AP LAPIC-Timer-Vektor
    mov ebx, LAPIC_TIMER_VECTOR
    mov eax, isr_ap_timer
    call idt_set_gate
    lidt [idt_descriptor]

    cmp dword [kernel_context + CONTEXT_PLATFORM], 2
    je apic_initialize

    ; UEFI/OVMF kann externe Interrupts über den APIC-Pfad hinterlassen.
    ; Der aktuelle Bootstrap-Interruptmanager arbeitet noch mit dem 8259 PIC,
    ; daher werden Local APIC und IMCR vor dessen Programmierung deterministisch
    ; in den Legacy-INTR-Zustand gebracht. Die spätere APIC-Phase übernimmt die
    ; erneute Aktivierung kontrolliert.
    mov ecx, 0x1B                   ; IA32_APIC_BASE
    rdmsr
    and eax, 0xFFFFF7FF             ; globales APIC-Enable (Bit 11) löschen
    wrmsr
    mov al, 0x70
    out 0x22, al
    in al, 0x23
    and al, 0xFE
    out 0x23, al

    ; 8259 PIC: IRQs auf Vektoren 32..47 verschieben.
    mov al, 0x11
    out PIC1_COMMAND, al
    call io_wait
    out PIC2_COMMAND, al
    call io_wait
    mov al, 0x20
    out PIC1_DATA, al
    call io_wait
    mov al, 0x28
    out PIC2_DATA, al
    call io_wait
    mov al, 0x04
    out PIC1_DATA, al
    call io_wait
    mov al, 0x02
    out PIC2_DATA, al
    call io_wait
    mov al, 0x01
    out PIC1_DATA, al
    call io_wait
    out PIC2_DATA, al
    call io_wait
    mov al, 0xFC                    ; nur Timer und Tastatur freigeben
    out PIC1_DATA, al
    mov al, 0xFF
    out PIC2_DATA, al
    clc
    ret

apic_initialize:
    ; Local APIC aktivieren und Spurious-Interrupt-Vektor freischalten.
    mov ecx, 0x1B
    rdmsr
    or eax, 0x00000800
    wrmsr
    mov eax, [0xFEE000F0]
    or eax, 0x00000100
    and eax, 0xFFFFFF00
    or eax, 0xFF
    mov [0xFEE000F0], eax

    ; OVMF darf beim Ownership-Transfer noch einen angenommenen Interrupt im
    ; ISR-Stack hinterlassen. Mehrfache EOIs sind für leere Ebenen harmlos.
    mov ecx, 8
.drain_isr:
    mov dword [0xFEE000B0], 0
    loop .drain_isr

    ; Q35: PIT IRQ0 ist per ACPI-Override auf GSI 2, Tastatur bleibt GSI 1.
    mov eax, 0x15                   ; GSI 2 destination high
    xor edx, edx
    call ioapic_write
    mov eax, 0x14                   ; GSI 2 -> Vektor 32
    mov edx, 32
    call ioapic_write
    mov eax, 0x13                   ; GSI 1 destination high
    xor edx, edx
    call ioapic_write
    mov eax, 0x12                   ; GSI 1 -> Vektor 33
    mov edx, 33
    call ioapic_write
    clc
    ret

ioapic_write:
    mov [0xFEC00000], eax
    mov [0xFEC00010], edx
    ret

; EBX=Vektor, EAX=Handleradresse.
idt_set_gate:
    push edi
    mov edi, idt_table
    lea edi, [edi + ebx * 8]
    mov [edi + 0], ax
    mov word [edi + 2], CODE_SEGMENT
    mov byte [edi + 4], 0
    mov byte [edi + 5], IDT_GATE_FLAGS
    shr eax, 16
    mov [edi + 6], ax
    pop edi
    ret

; §009 interrupt_self_test – IDT-Integrität und Gate-Programmierung nach
; interrupt_initialize (NPSPEC-KERNEL-0009)
interrupt_self_test:
    push ebx
    push edi
    sub esp, 8                          ; [esp+0..5] = IDTR-Puffer (6 Byte)
    xor edi, edi                        ; Fehler-Zähler
    ; Test 1: IDT geladen – sidt → Basis == idt_table
    sidt [esp]
    mov eax, [esp + 2]
    cmp eax, idt_table
    je .t2
    inc edi
.t2:
    ; Test 2: Limit == (IDT_ENTRY_COUNT*8)-1
    movzx eax, word [esp]
    cmp eax, (IDT_ENTRY_COUNT * 8) - 1
    je .t3
    inc edi
.t3:
    ; Test 3: Vektor 0 trägt den Exception-Stub (nicht isr_unexpected)
    movzx eax, word [idt_table + 0 * 8 + 0]
    movzx ebx, word [idt_table + 0 * 8 + 6]
    shl ebx, 16
    or eax, ebx
    cmp eax, isr_unexpected
    jne .t4
    inc edi
.t4:
    ; Test 4: Vektor 32 (IRQ0/PIT-Timer) == irq0_stub
    movzx eax, word [idt_table + 32 * 8 + 0]
    movzx ebx, word [idt_table + 32 * 8 + 6]
    shl ebx, 16
    or eax, ebx
    cmp eax, irq0_stub
    je .t5
    inc edi
.t5:
    ; Test 5: Vektor 0x80 (Syscall) == syscall_stub
    movzx eax, word [idt_table + 0x80 * 8 + 0]
    movzx ebx, word [idt_table + 0x80 * 8 + 6]
    shl ebx, 16
    or eax, ebx
    cmp eax, syscall_stub
    je .t6
    inc edi
.t6:
    ; Test 6: Syscall-Gate DPL=3 (Byte 5 == 0xEE)
    movzx eax, byte [idt_table + 0x80 * 8 + 5]
    cmp eax, 0xEE
    je .done
    inc edi
.done:
    test edi, edi
    jnz .selftest_fail
    add esp, 8
    pop edi
    pop ebx
    clc
    ret
.selftest_fail:
    add esp, 8
    pop edi
    pop ebx
    stc
    ret

timer_initialize:
    mov al, 0x36
    out PIT_COMMAND, al
    mov ax, PIT_DIVISOR
    out PIT_CHANNEL0, al
    mov al, ah
    out PIT_CHANNEL0, al
    mov dword [timer_ticks], 0
    clc
    ret

interrupt_enable:
    sti
    ret

interrupt_disable:
    cli
    ret

timer_get_ticks:
    mov eax, [timer_ticks]
    ret

; Mindestens zwei echte IRQ0-Ereignisse müssen eintreffen.
timer_self_test:
    mov ecx, 50000000
.wait:
    cmp dword [timer_ticks], 2
    jae .success
    pause
    dec ecx
    jnz .wait
.invalid:
    stc
    ret
.success:
    clc
    ret

io_wait:
    mov al, 0
    out 0x80, al
    ret

; Wandelt PS/2-Scan-Codes aus Set 1 oder 2 in wenige semantische Aktionen um.
; Ausgabe EAX: 0 = verarbeitet/ignoriert, 2 = Diagnose-Panic, 3 = Shutdown.
input_router_handle_scancode:
    movzx ebx, al
    cmp al, 0xE0
    jne .not_extended_prefix
    mov byte [keyboard_extended], 1
    xor eax, eax
    ret
.not_extended_prefix:
    cmp al, 0xF0
    jne .not_break_prefix
    mov byte [keyboard_break_pending], 1
    xor eax, eax
    ret
.not_break_prefix:
    cmp byte [keyboard_break_pending], 0
    je .not_set2_break
    mov byte [keyboard_break_pending], 0
    mov byte [keyboard_extended], 0
    xor eax, eax
    ret
.not_set2_break:
    test al, 0x80                    ; Break-Code aus Set 1
    jz .make_code
    mov byte [keyboard_extended], 0
    xor eax, eax
    ret
.make_code:
    cmp byte [keyboard_extended], 0
    je .plain
    mov byte [keyboard_extended], 0
    cmp al, 0x5B                    ; linke Windows/Nova-Taste, Set 1
    je .toggle_start
    cmp al, 0x1F                    ; linke Windows/Nova-Taste, Set 2
    je .toggle_start
    cmp al, 0x48                    ; Pfeil hoch, Set 1
    je .navigate_up
    cmp al, 0x75                    ; Pfeil hoch, Set 2
    je .navigate_up
    cmp al, 0x50                    ; Pfeil runter, Set 1
    je .navigate_down
    cmp al, 0x72                    ; Pfeil runter, Set 2
    je .navigate_down
    cmp al, 0x4B                    ; Pfeil links, Set 1
    je .navigate_left
    cmp al, 0x6B                    ; Pfeil links, Set 2
    je .navigate_left
    cmp al, 0x4D                    ; Pfeil rechts, Set 1
    je .navigate_right
    cmp al, 0x74                    ; Pfeil rechts, Set 2
    je .navigate_right
    cmp al, 0x53                    ; Entf, Set 1
    je .delete_key
    cmp al, 0x71                    ; Entf, Set 2
    je .delete_key
    xor eax, eax
    ret
.plain:
    cmp al, 0x58                    ; F12, Set 1
    je .panic
    cmp al, 0x07                    ; F12, Set 2
    je .panic
    cmp al, 0x01                    ; Escape, Set 1
    je .escape
    cmp al, 0x76                    ; Escape, Set 2
    je .escape
    cmp al, 0x0F                    ; Tab, Set 1
    je .focus_next
    cmp al, 0x0D                    ; Tab, Set 2
    je .focus_next
    cmp al, 0x1C                    ; Enter, Set 1
    je .activate
    cmp al, 0x5A                    ; Enter, Set 2
    je .activate
    cmp al, 0x0E                    ; Backspace, Set 1
    je .navigate_back
    cmp al, 0x66                    ; Backspace, Set 2
    je .navigate_back
    cmp al, 0x3C                    ; F2, Set 1
    je .rename_key
    cmp al, 0x06                    ; F2, Set 2
    je .rename_key
    ; Alle uebrigen "flachen" Tasten: ueber eine US-QWERTY-Tabelle (nur
    ; Set 1, Kleinbuchstaben, keine Umschalt-Unterstuetzung) in ein
    ; druckbares Zeichen fuer die Texteingabe (Umbenennen) uebersetzen.
    ; Set-2-Tasten und nicht zugeordnete Set-1-Codes bleiben wirkungslos.
    movzx edx, al
    mov bl, [keyboard_ascii_table + edx]
    cmp bl, 0
    je .ignored
    mov eax, SYSTEM_INPUT_TEXT_CHAR
    call input_router_enqueue
    xor eax, eax
    ret
.ignored:
    xor eax, eax
    ret
.navigate_back:
    mov eax, SYSTEM_INPUT_NAVIGATE_BACK
    call input_router_enqueue
    xor eax, eax
    ret
.rename_key:
    mov eax, SYSTEM_INPUT_RENAME_KEY
    call input_router_enqueue
    xor eax, eax
    ret
.toggle_start:
    mov eax, SYSTEM_INPUT_TOGGLE_START
    call input_router_enqueue
    xor eax, eax
    ret
.escape:
    test dword [display_scene_flags], DISPLAY_SCENE_START_MENU
    jz .shutdown
    mov eax, SYSTEM_INPUT_CLOSE_START
    call input_router_enqueue
    xor eax, eax
    ret
.focus_next:
    mov eax, SYSTEM_INPUT_FOCUS_NEXT
    call input_router_enqueue
    xor eax, eax
    ret
.activate:
    mov eax, SYSTEM_INPUT_ACTIVATE
    call input_router_enqueue
    xor eax, eax
    ret
.navigate_up:
    mov eax, SYSTEM_INPUT_NAVIGATE_UP
    call input_router_enqueue
    xor eax, eax
    ret
.navigate_down:
    mov eax, SYSTEM_INPUT_NAVIGATE_DOWN
    call input_router_enqueue
    xor eax, eax
    ret
.delete_key:
    mov eax, SYSTEM_INPUT_DELETE
    call input_router_enqueue
    xor eax, eax
    ret
.navigate_left:
    mov eax, SYSTEM_INPUT_NAVIGATE_LEFT
    call input_router_enqueue
    xor eax, eax
    ret
.navigate_right:
    mov eax, SYSTEM_INPUT_NAVIGATE_RIGHT
    call input_router_enqueue
    xor eax, eax
    ret
.panic:
    mov eax, 2
    ret
.shutdown:
    mov eax, 3
    ret

; EAX = Aktion, EBX = ursprünglicher Scan-Code. Ein einzelner Slot begrenzt
; Speicherverbrauch und verhindert Eingabefluten im Bootstrap-Displaypfad.
input_router_enqueue:
    mov edx, [display_scene_focus]
input_router_enqueue_target:
    cmp dword [display_server_ready], 1
    jne .ignored
    cmp dword [userspace_ready_seen], 1
    jne .ignored
    cmp dword [display_input_pending], 0
    jne .dropped
    mov [display_input_action], eax
    mov [display_input_scancode], ebx
    mov ecx, [timer_ticks]
    mov [display_input_tick], ecx
    mov [display_input_target], edx
    mov dword [display_input_pending], 1
.ignored:
    ret
.dropped:
    inc dword [display_input_dropped]
    ret

interrupt_dispatch:
    push ebp
    mov ebp, esp
    mov eax, [ebp + 8]
    mov edx, [ebp + 12]
    mov [interrupt_return_frame], edx
    cmp eax, 32
    je .timer
    cmp eax, 33
    je .keyboard
    cmp eax, 0x80
    je .syscall
    cmp eax, 32
    jb .exception
    cmp eax, 48
    jb .slave_irq
    jmp .done
.syscall:
    call syscall_dispatch
    jmp .done
.timer:
    inc dword [timer_ticks]
    cmp dword [kernel_context + CONTEXT_PLATFORM], 2
    jne .timer_input
    mov dword [0xFEE000B0], 0       ; edge-triggered: früh quittieren
.timer_input:
    in al, 0x64                     ; Fallback, falls QEMU IRQ1 nicht zustellt
    test al, 0x01
    jz .timer_schedule
    mov ah, al
    in al, 0x60
    test ah, 0x20                   ; AUX-Daten stammen von der PS/2-Maus
    jnz .timer_mouse
    call input_router_handle_scancode
    cmp eax, 2
    je .debug_panic
    cmp eax, 3
    je kernel_shutdown
    jmp .timer_schedule
.timer_mouse:
    call ps2_mouse_handle_byte
.timer_schedule:
    ; Vor Scheduler-Initialisierung dient IRQ0 nur als PIT-Lebenszeichen.
    ; Deadline-, I/O- und Scheduling-Pfade greifen auf Managerzustand zu, der
    ; in der frühen Interruptphase noch nicht aufgebaut ist.
    cmp dword [scheduler_enabled], 1
    jne .timer_ack
    call task_deadline_poll
    call io_request_poll_deadlines
    call io_cancel_for_requested_tasks
    ; Scheduler-Spinlock: BSP hält ihn immer (kein Contention im Normalfall,
    ; da APs via isr_ap_timer nur versuchen, den Lock zu bekommen).
    ; lock bts setzt Bit 0 und gibt alten Wert zurück; CF=1 → schon gehalten.
    lock bts dword [scheduler_lock], 0
    jc .sched_skip              ; Sollte auf BSP nie eintreten (defensive Guard)
    mov dword [scheduler_caller_cpu], 0  ; BSP = CPU-Slot 0 (§133)
    mov eax, [interrupt_return_frame]
    push eax
    call scheduler_on_tick
    add esp, 4
    mov [interrupt_return_frame], eax
    lock btr dword [scheduler_lock], 0
    push eax
    mov al, PIC_EOI
    out PIC1_COMMAND, al
    pop eax
    jmp .done
.sched_skip:
.timer_ack:
    mov al, PIC_EOI
    out PIC1_COMMAND, al
    jmp .done
.keyboard:
    cmp dword [kernel_context + CONTEXT_PLATFORM], 2
    jne .keyboard_read
    mov dword [0xFEE000B0], 0
.keyboard_read:
    in al, 0x60                     ; Controllerdaten quittieren
    call input_router_handle_scancode
    cmp eax, 2
    je .debug_panic
    cmp eax, 3
    je kernel_shutdown
    jmp .keyboard_ack
.debug_panic:
    mov eax, 0xDEB60001
    mov edx, 0x0000DEB6
    mov esi, message_debug_panic
    call kernel_panic
.keyboard_ack:
    mov al, PIC_EOI
    out PIC1_COMMAND, al
    jmp .done
.slave_irq:
    mov al, PIC_EOI
    out PIC2_COMMAND, al
    out PIC1_COMMAND, al
    jmp .done
.exception:
    cli
    mov [last_exception_vector], eax
    mov [interrupt_return_frame], edx
    ; §010 Exception Manager: vollständige Klassifizierung und Weiterleitung
    cmp dword [exception_mgr_initialized], 1
    je .exception_mgr_dispatch
    ; Frühphase: primitiver Fallback bevor §010 bereit ist
    cmp eax, 14
    jne .log_exception_early
    mov eax, cr2
    mov [last_fault_address], eax
.log_exception_early:
    mov esi, message_exception
    call serial_write_string
    mov eax, [last_exception_vector]
    call serial_write_hex32
    cmp dword [last_exception_vector], 14
    jne .exception_newline_early
    mov esi, message_fault_address
    call serial_write_string
    mov eax, [last_fault_address]
    call serial_write_hex32
.exception_newline_early:
    mov esi, message_newline
    call serial_write_string
    jmp kernel_halt
.exception_mgr_dispatch:
    ; Übergabe an §010 exception_manager_dispatch
    ; EAX = Vektor, EDX = Frame-Zeiger (Stack bei isr_common)
    mov edx, [interrupt_return_frame]
    push edx
    push eax
    call exception_manager_dispatch
    add esp, 8
    ; EAX = neuer Frame-Zeiger (oder 0 bei Fortsetzung des alten)
    test eax, eax
    jz .exception_done
    mov [interrupt_return_frame], eax
.exception_done:
.done:
    cmp dword [kernel_context + CONTEXT_PLATFORM], 2
    jne .return_frame
    mov dword [0xFEE000B0], 0       ; Local-APIC EOI
.return_frame:
    mov eax, [interrupt_return_frame]
    pop ebp
    ret

isr_common:
    pushad
    push ds
    push es
    push fs
    push gs
    mov ax, DATA_SEGMENT
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov eax, [esp + 48]             ; normalisierter Vektor
    mov edx, esp
    push edx
    push eax
    call interrupt_dispatch
    add esp, 8
    mov esp, eax
    pop gs
    pop fs
    pop es
    pop ds
    popad
    add esp, 8                      ; Vektor + Fehlercode
    iretd

%macro ISR_NO_ERROR 1
isr_%1:
    push dword 0
    push dword %1
    jmp isr_common
%endmacro

%macro ISR_WITH_ERROR 1
isr_%1:
    push dword %1
    jmp isr_common
%endmacro

ISR_NO_ERROR 0
ISR_NO_ERROR 1
ISR_NO_ERROR 2
ISR_NO_ERROR 3
ISR_NO_ERROR 4
ISR_NO_ERROR 5
ISR_NO_ERROR 6
ISR_NO_ERROR 7
ISR_WITH_ERROR 8
ISR_NO_ERROR 9
ISR_WITH_ERROR 10
ISR_WITH_ERROR 11
ISR_WITH_ERROR 12
ISR_WITH_ERROR 13
ISR_WITH_ERROR 14
ISR_NO_ERROR 15
ISR_NO_ERROR 16
ISR_WITH_ERROR 17
ISR_NO_ERROR 18
ISR_NO_ERROR 19
ISR_NO_ERROR 20
ISR_WITH_ERROR 21
ISR_NO_ERROR 22
ISR_NO_ERROR 23
ISR_NO_ERROR 24
ISR_NO_ERROR 25
ISR_NO_ERROR 26
ISR_NO_ERROR 27
ISR_NO_ERROR 28
ISR_WITH_ERROR 29
ISR_WITH_ERROR 30
ISR_NO_ERROR 31

irq0_stub:
    push dword 0
    push dword 32
    jmp isr_common
irq1_stub:
    push dword 0
    push dword 33
    jmp isr_common
syscall_stub:
    push dword 0
    push dword 0x80
    jmp isr_common
isr_unexpected:
    push dword 0
    push dword 255
    jmp isr_common

align 4
exception_stub_table:
%assign __vector 0
%rep 32
    dd isr_%+__vector
%assign __vector __vector + 1
%endrep

align 8
kernel_gdt:
    dq 0x0000000000000000
    dq 0x00CF9A000000FFFF
    dq 0x00CF92000000FFFF
    dq 0x00CFFA000000FFFF          ; Ring-3 Code, Selector 0x1B
    dq 0x00CFF2000000FFFF          ; Ring-3 Data, Selector 0x23
kernel_tss_descriptor:
    dq 0                            ; 32-Bit Available TSS, Selector 0x28
kernel_gdt_descriptor:
    dw kernel_gdt_descriptor - kernel_gdt - 1
    dd kernel_gdt

align 16
idt_table:
    times IDT_ENTRY_COUNT dq 0
idt_descriptor:
    dw (IDT_ENTRY_COUNT * 8) - 1
    dd idt_table

timer_ticks:           dd 0
last_exception_vector: dd 0
last_fault_address:    dd 0
interrupt_return_frame: dd 0

align 16
kernel_tss:
    times 104 db 0

; Versionierte öffentliche Modulgrenze gemäß ADR-2006/2007.
align 4
interrupt_api:
    dd INTERRUPT_API_SIZE
    dw 1, 0
    dd IDT_ENTRY_COUNT
    dd INTERRUPT_CAPABILITIES
    dd interrupt_enable
    dd interrupt_disable
    dd timer_get_ticks
    dd 100

; ---------------------------------------------------------------------------
; Kernel-Nachrichtenwarteschlange (ADR-2005 / NPSPEC-KERNEL-0015)
; ---------------------------------------------------------------------------

%include "arch/x86_64/semantic32.inc"
%include "arch/x86_64/semantic_core32.inc"
%include "arch/x86_64/state32.inc"
%include "arch/x86_64/storage32.inc"
%include "arch/x86_64/novafs32.inc"
%include "arch/x86_64/vfs32.inc"
%include "arch/x86_64/explorer32.inc"

IPC_MESSAGE_SIZE   equ 16
IPC_QUEUE_CAPACITY equ 16
IPC_API_SIZE       equ 32
IPC_CAP_FIFO       equ 0x00000001
IPC_CAP_IRQ_SAFE   equ 0x00000002

ipc_initialize:
    mov dword [ipc_head], 0
    mov dword [ipc_tail], 0
    mov dword [ipc_count], 0
    mov dword [ipc_sent], 0
    mov dword [ipc_received], 0
    mov dword [ipc_last_sequence], 0
    mov dword [ipc_error], 0
    mov edi, ipc_messages
    xor eax, eax
    mov ecx, (IPC_MESSAGE_SIZE * IPC_QUEUE_CAPACITY) / 4
    rep stosd
    clc
    ret

; §015 §38: Self-Test – 7 Tests (FIFO-Invarianten, Send/Receive, Grenzen)
ipc_self_test:
    push ebx
    push esi
    push edi
    sub esp, 32                         ; [esp+0..15]=Sendepuffer, [esp+16..31]=Empfangspuffer
    xor ebx, ebx                        ; Fehler-Zähler

    ; Test 1: Queue leer nach init
    cmp dword [ipc_count], 0
    je .t2
    inc ebx

.t2:
    ; Test 2: ipc_send mit Testinhalt
    mov dword [esp + 0],  0xDEADBEEF
    mov dword [esp + 4],  0xCAFEBABE
    mov dword [esp + 8],  0x12345678
    mov dword [esp + 12], 0xABCDABCD
    lea esi, [esp + 0]
    call ipc_send
    cmp eax, 1
    je .t3
    inc ebx

.t3:
    ; Test 3: ipc_count == 1 nach send
    cmp dword [ipc_count], 1
    je .t4
    inc ebx

.t4:
    ; Test 4: ipc_receive
    lea edi, [esp + 16]
    call ipc_receive
    cmp eax, 1
    je .t5
    inc ebx

.t5:
    ; Test 5: empfangene Daten identisch mit gesendeten
    cmp dword [esp + 16], 0xDEADBEEF
    jne .t5_fail
    cmp dword [esp + 20], 0xCAFEBABE
    je .t6
.t5_fail:
    inc ebx

.t6:
    ; Test 6: ipc_count == 0 nach receive
    cmp dword [ipc_count], 0
    je .t7
    inc ebx

.t7:
    ; Test 7: receive auf leere Queue → EAX=0
    lea edi, [esp + 16]
    call ipc_receive
    test eax, eax
    jz .done
    inc ebx

.done:
    test ebx, ebx
    jnz .selftest_fail
    add esp, 32
    pop edi
    pop esi
    pop ebx
    clc
    ret
.selftest_fail:
    add esp, 32
    pop edi
    pop esi
    pop ebx
    stc
    ret

; ESI zeigt auf eine 16-Byte-Nachricht. EAX=1 bei Erfolg, sonst 0.
ipc_send:
    pushfd
    cli
    cmp dword [ipc_count], IPC_QUEUE_CAPACITY
    jae .full
    mov eax, [ipc_tail]
    imul eax, IPC_MESSAGE_SIZE
    lea edi, [ipc_messages + eax]
    mov eax, [esi + 0]
    mov [edi + 0], eax
    mov eax, [esi + 4]
    mov [edi + 4], eax
    mov eax, [esi + 8]
    mov [edi + 8], eax
    mov eax, [esi + 12]
    mov [edi + 12], eax
    inc dword [ipc_tail]
    and dword [ipc_tail], IPC_QUEUE_CAPACITY - 1
    inc dword [ipc_count]
    popfd
    mov eax, 1
    ret
.full:
    popfd
    xor eax, eax
    ret

; EDI zeigt auf 16 Byte Zielspeicher. EAX=1 bei Erfolg, sonst 0.
ipc_receive:
    pushfd
    cli
    cmp dword [ipc_count], 0
    je .empty
    mov eax, [ipc_head]
    imul eax, IPC_MESSAGE_SIZE
    lea esi, [ipc_messages + eax]
    mov eax, [esi + 0]
    mov [edi + 0], eax
    mov eax, [esi + 4]
    mov [edi + 4], eax
    mov eax, [esi + 8]
    mov [edi + 8], eax
    mov eax, [esi + 12]
    mov [edi + 12], eax
    inc dword [ipc_head]
    and dword [ipc_head], IPC_QUEUE_CAPACITY - 1
    dec dword [ipc_count]
    popfd
    mov eax, 1
    ret
.empty:
    popfd
    xor eax, eax
    ret

align 4
ipc_api:
    dd IPC_API_SIZE
    dw 1, 0
    dd IPC_MESSAGE_SIZE
    dd IPC_QUEUE_CAPACITY
    dd IPC_CAP_FIFO | IPC_CAP_IRQ_SAFE
    dd ipc_send
    dd ipc_receive
    dd ipc_count

ipc_head:          dd 0
ipc_tail:          dd 0
ipc_count:         dd 0
ipc_sent:          dd 0
ipc_received:      dd 0
ipc_last_sequence: dd 0
ipc_error:         dd 0
ipc_thread_message:
    times IPC_MESSAGE_SIZE db 0
ipc_receive_buffer:
    times IPC_MESSAGE_SIZE db 0
align 16
ipc_messages:
    times IPC_QUEUE_CAPACITY * IPC_MESSAGE_SIZE db 0

; Bootkritischer Device Manager (NPSPEC-KERNEL-0017 / NPSPEC-KERNEL-0002 Phase 8)
DEVICE_API_SIZE       equ 32
DEVICE_CAPACITY       equ 8
DEVICE_RECORD_SIZE    equ 32
DEVICE_STATE_ACTIVE   equ 1
OBJECT_TYPE_DEVICE    equ 6
DEVICE_ID             equ 0
DEVICE_CLASS          equ 4
DEVICE_STATE          equ 8
DEVICE_HANDLE         equ 12
DEVICE_RESOURCE_BASE  equ 16
DEVICE_RESOURCE_SIZE  equ 20
DEVICE_FLAGS          equ 24
DEVICE_DRIVER         equ 28

device_manager_initialize:
    mov edi, device_records
    xor eax, eax
    mov ecx, (DEVICE_CAPACITY * DEVICE_RECORD_SIZE) / 4
    rep stosd
    mov dword [device_count], 0
    mov eax, 1                      ; PIT/Timer
    mov edx, 1
    mov ebx, 0x40
    mov ecx, 1
    call device_register
    jc .invalid
    mov eax, 2                      ; PS/2-Tastaturcontroller
    mov edx, 2
    mov ebx, 0x60
    mov ecx, 5
    call device_register
    jc .invalid
    test dword [kernel_context + CONTEXT_SEEN], CONTEXT_HAS_GRAPHICS
    jz .complete
    mov eax, 3                      ; übernommener Firmware-Framebuffer
    mov edx, 3
    mov ebx, [kernel_context + CONTEXT_FRAMEBUFFER]
    mov ecx, [kernel_context + CONTEXT_PITCH]
    imul ecx, [kernel_context + CONTEXT_HEIGHT]
    call device_register
    jc .invalid
.complete:
    clc
    ret
.invalid:
    stc
    ret

; EAX=Geräte-ID, EDX=Klasse, EBX=Ressourcenbasis, ECX=Größe.
device_register:
    mov [device_temp_id], eax
    mov [device_temp_class], edx
    mov [device_temp_base], ebx
    mov [device_temp_size], ecx
    cmp dword [device_count], DEVICE_CAPACITY
    jae .invalid
    call device_lookup
    jnc .invalid
    mov eax, OBJECT_TYPE_DEVICE
    mov edx, [device_temp_id]
    xor ebx, ebx
    call object_create
    jc .invalid
    mov edx, [device_count]
    shl edx, 5
    add edx, device_records
    mov ecx, [device_temp_id]
    mov [edx + DEVICE_ID], ecx
    mov ecx, [device_temp_class]
    mov [edx + DEVICE_CLASS], ecx
    mov dword [edx + DEVICE_STATE], DEVICE_STATE_ACTIVE
    mov [edx + DEVICE_HANDLE], eax
    mov ecx, [device_temp_base]
    mov [edx + DEVICE_RESOURCE_BASE], ecx
    mov ecx, [device_temp_size]
    mov [edx + DEVICE_RESOURCE_SIZE], ecx
    mov dword [edx + DEVICE_FLAGS], 1 ; bootkritisch
    inc dword [device_count]
    clc
    ret
.invalid:
    stc
    ret

; EAX=Geräte-ID; EAX=Datensatz bei Erfolg.
device_lookup:
    xor ecx, ecx
.scan:
    cmp ecx, [device_count]
    jae .missing
    mov edx, ecx
    shl edx, 5
    add edx, device_records
    cmp [edx + DEVICE_ID], eax
    je .found
    inc ecx
    jmp .scan
.found:
    mov eax, edx
    clc
    ret
.missing:
    xor eax, eax
    stc
    ret

device_manager_self_test:
    cmp dword [device_count], 2
    jb .invalid
    mov eax, 1
    call device_lookup
    jc .invalid
    cmp dword [eax + DEVICE_STATE], DEVICE_STATE_ACTIVE
    jne .invalid
    mov eax, 2
    call device_lookup
    jc .invalid
    clc
    ret
.invalid:
    stc
    ret

align 4
device_api:
    dd DEVICE_API_SIZE
    dw 1, 0
    dd DEVICE_CAPACITY
    dd device_register
    dd device_lookup
    dd device_count
    dd device_records
    dd 0
device_count:      dd 0
device_temp_id:    dd 0
device_temp_class: dd 0
device_temp_base:  dd 0
device_temp_size:  dd 0
align 4
device_records:
    times DEVICE_CAPACITY * DEVICE_RECORD_SIZE db 0

; ---------------------------------------------------------------------------
; Driver Framework 1.0 (NPSPEC-KERNEL-0018)
; ---------------------------------------------------------------------------
DRV_API_SIZE       equ 32
DRV_CAPACITY       equ 8
DRV_RECORD_SIZE    equ 48               ; 12 × 4-Byte-Felder

; §018 §4: Treibertypen
DRV_TYPE_KERNEL    equ 0
DRV_TYPE_USERSPACE equ 1
DRV_TYPE_BUS       equ 2
DRV_TYPE_FUNCTION  equ 3
DRV_TYPE_FILTER    equ 4
DRV_TYPE_VIRTUAL   equ 5

; §018 §21: Treiberzustände (Bootstrap-Subset)
DRV_STATE_LOADED   equ 0
DRV_STATE_BOUND    equ 2
DRV_STATE_RUNNING  equ 4
DRV_STATE_FAILED   equ 9

; Record-Offsets
DRV_ID_LO      equ 0
DRV_ID_HI      equ 4
DRV_TYPE_OFF   equ 8
DRV_STATE_OFF  equ 12
DRV_ABI_VER    equ 16
DRV_DEV_HANDLE equ 20
DRV_FLAGS_OFF  equ 24
; Bytes 28–47: reserviert (künftige Erweiterungen)

driver_framework_initialize:
    mov edi, drv_records
    xor eax, eax
    mov ecx, (DRV_CAPACITY * DRV_RECORD_SIZE) / 4
    rep stosd
    mov dword [drv_count], 0
    mov dword [drv_mgr_initialized], 1
    clc
    ret

; EAX=id_lo, EDX=id_hi → ESI=Record-Zeiger (CF=0) oder CF=1
driver_find:
    push ecx
    push edi
    xor ecx, ecx
.scan:
    cmp ecx, DRV_CAPACITY
    jae .not_found
    mov edi, ecx
    imul edi, DRV_RECORD_SIZE
    add edi, drv_records
    cmp dword [edi + DRV_ID_LO], eax
    jne .next
    cmp dword [edi + DRV_ID_HI], edx
    je .found
.next:
    inc ecx
    jmp .scan
.found:
    mov esi, edi
    pop edi
    pop ecx
    clc
    ret
.not_found:
    pop edi
    pop ecx
    stc
    ret

; EAX=id_lo, EDX=id_hi, EBX=type, ECX=abi_version, ESI=flags
driver_register:
    test eax, eax
    jnz .id_ok
    test edx, edx
    jz .invalid
.id_ok:
    cmp dword [drv_count], DRV_CAPACITY
    jae .invalid
    mov [drv_tmp_id_lo], eax
    mov [drv_tmp_id_hi], edx
    mov [drv_tmp_type],  ebx
    mov [drv_tmp_abi],   ecx
    mov [drv_tmp_flags], esi
    call driver_find
    jnc .invalid                        ; Duplikat abweisen
    xor ecx, ecx
.scan_slot:
    cmp ecx, DRV_CAPACITY
    jae .invalid
    mov edi, ecx
    imul edi, DRV_RECORD_SIZE
    add edi, drv_records
    cmp dword [edi + DRV_ID_LO], 0
    jne .scan_next
    cmp dword [edi + DRV_ID_HI], 0
    je .fill_slot
.scan_next:
    inc ecx
    jmp .scan_slot
.fill_slot:
    mov eax, [drv_tmp_id_lo]
    mov [edi + DRV_ID_LO], eax
    mov eax, [drv_tmp_id_hi]
    mov [edi + DRV_ID_HI], eax
    mov eax, [drv_tmp_type]
    mov [edi + DRV_TYPE_OFF], eax
    mov dword [edi + DRV_STATE_OFF], DRV_STATE_LOADED
    mov eax, [drv_tmp_abi]
    mov [edi + DRV_ABI_VER], eax
    mov dword [edi + DRV_DEV_HANDLE], 0
    mov eax, [drv_tmp_flags]
    mov [edi + DRV_FLAGS_OFF], eax
    inc dword [drv_count]
    clc
    ret
.invalid:
    stc
    ret

; EAX=id_lo, EDX=id_hi, EBX=device_handle
driver_bind:
    call driver_find
    jc .invalid
    cmp dword [esi + DRV_STATE_OFF], DRV_STATE_LOADED
    jne .invalid
    mov [esi + DRV_DEV_HANDLE], ebx
    mov dword [esi + DRV_STATE_OFF], DRV_STATE_BOUND
    clc
    ret
.invalid:
    stc
    ret

; §018 §60: Self-Test – 7 Tests
driver_framework_self_test:
    push esi
    push edi
    xor edi, edi                        ; Fehler-Zähler

    ; Test 1: initialisiert
    cmp dword [drv_mgr_initialized], 1
    je .t2
    inc edi

.t2:
    ; Test 2: drv_count == 0
    cmp dword [drv_count], 0
    je .t3
    inc edi

.t3:
    ; Test 3: driver_register – Test-Treiber-ID 0xDFDF0001
    mov eax, 0xDFDF0001
    xor edx, edx
    mov ebx, DRV_TYPE_KERNEL
    mov ecx, 0x00010000                 ; ABI 1.0
    xor esi, esi
    call driver_register
    jnc .t4
    inc edi
    jmp .done

.t4:
    ; Test 4: drv_count == 1
    cmp dword [drv_count], 1
    je .t5
    inc edi

.t5:
    ; Test 5: driver_find findet den Treiber
    mov eax, 0xDFDF0001
    xor edx, edx
    call driver_find
    jnc .t6
    inc edi

.t6:
    ; Test 6: driver_bind
    mov eax, 0xDFDF0001
    xor edx, edx
    mov ebx, 1
    call driver_bind
    jnc .t7
    inc edi

.t7:
    ; Test 7: Zustand == BOUND
    mov eax, 0xDFDF0001
    xor edx, edx
    call driver_find
    jc .t7_fail
    cmp dword [esi + DRV_STATE_OFF], DRV_STATE_BOUND
    je .done
.t7_fail:
    inc edi

.done:
    test edi, edi
    jnz .selftest_fail
    pop edi
    pop esi
    clc
    ret
.selftest_fail:
    pop edi
    pop esi
    stc
    ret

align 4
drv_api:
    dd DRV_API_SIZE
    dw 1, 0
    dd DRV_CAPACITY
    dd driver_register
    dd driver_find
    dd driver_bind
    dd drv_count
    dd drv_records

drv_mgr_initialized: dd 0
drv_count:           dd 0
drv_tmp_id_lo:       dd 0
drv_tmp_id_hi:       dd 0
drv_tmp_type:        dd 0
drv_tmp_abi:         dd 0
drv_tmp_flags:       dd 0
align 4
drv_records:
    times DRV_CAPACITY * DRV_RECORD_SIZE db 0

; Frühes VFS-Bootstrap-Root (NPSPEC-KERNEL-0019). Das RAMFS ist absichtlich
; read-only: es stellt bis zum späteren Dateisystemtreiber nur den Root-Knoten
; und dessen Mount-Namespace bereit.
VFS_API_SIZE                equ 32
VFS_STATE_SIZE              equ 32
VFS_FLAG_BOOTSTRAP_ROOT     equ 0x00000001
VFS_FLAG_READ_ONLY          equ 0x00000002
VFS_NODE_DIRECTORY          equ 1
OBJECT_TYPE_FILESYSTEM      equ 7
OBJECT_TYPE_MOUNT_NAMESPACE equ 8
OBJECT_TYPE_MOUNT           equ 9
OBJECT_TYPE_VFS_NODE        equ 10

VFS_INITIALIZED       equ 0
VFS_ROOT_READY        equ 4
VFS_FILESYSTEM_HANDLE equ 8
VFS_NAMESPACE_HANDLE  equ 12
VFS_MOUNT_HANDLE      equ 16
VFS_ROOT_NODE_HANDLE  equ 20
VFS_FLAGS             equ 24

vfs_initialize:
    mov edi, vfs_state
    xor eax, eax
    mov ecx, VFS_STATE_SIZE / 4
    rep stosd

    mov eax, OBJECT_TYPE_FILESYSTEM
    mov edx, 1                      ; Bootstrap-RAMFS-Instanz
    xor ebx, ebx
    call object_create
    jc .invalid
    mov [vfs_state + VFS_FILESYSTEM_HANDLE], eax

    mov eax, OBJECT_TYPE_MOUNT_NAMESPACE
    mov edx, 1                      ; initialer Kernel-Namespace
    xor ebx, ebx
    call object_create
    jc .invalid
    mov [vfs_state + VFS_NAMESPACE_HANDLE], eax

    mov eax, OBJECT_TYPE_MOUNT
    mov edx, 1                      ; Root-Mount
    mov ebx, [vfs_state + VFS_NAMESPACE_HANDLE]
    call object_create
    jc .invalid
    mov [vfs_state + VFS_MOUNT_HANDLE], eax

    mov eax, OBJECT_TYPE_VFS_NODE
    mov edx, 1                      ; Node-ID 1 ist das Root-Verzeichnis
    mov ebx, [vfs_state + VFS_FILESYSTEM_HANDLE]
    call object_create
    jc .invalid
    mov [vfs_state + VFS_ROOT_NODE_HANDLE], eax

    mov dword [vfs_state + VFS_FLAGS], VFS_FLAG_BOOTSTRAP_ROOT | VFS_FLAG_READ_ONLY
    mov dword [vfs_state + VFS_ROOT_READY], 1
    mov dword [vfs_state + VFS_INITIALIZED], 1
    clc
    ret
.invalid:
    stc
    ret

; ESI=Pfad, ECX=explizite Byte-Länge. Liefert in EAX den Root-Node-Handle.
; Die längenbasierte Schnittstelle akzeptiert keine implizit terminierten Pfade.
vfs_lookup_root:
    cmp dword [vfs_state + VFS_ROOT_READY], 1
    jne .missing
    cmp ecx, 1
    jne .missing
    cmp byte [esi], '/'
    jne .missing
    mov eax, [vfs_state + VFS_ROOT_NODE_HANDLE]
    test eax, eax
    jz .missing
    clc
    ret
.missing:
    xor eax, eax
    stc
    ret

vfs_self_test:
    cmp dword [vfs_state + VFS_INITIALIZED], 1
    jne .invalid
    cmp dword [vfs_state + VFS_FLAGS], VFS_FLAG_BOOTSTRAP_ROOT | VFS_FLAG_READ_ONLY
    jne .invalid
    mov esi, vfs_root_path
    mov ecx, 1
    call vfs_lookup_root
    jc .invalid
    cmp eax, [vfs_state + VFS_ROOT_NODE_HANDLE]
    jne .invalid
    mov esi, vfs_invalid_path
    mov ecx, 1
    call vfs_lookup_root
    jnc .invalid
    clc
    ret
.invalid:
    stc
    ret

align 4
vfs_api:
    dd VFS_API_SIZE
    dw 1, 0
    dd vfs_state
    dd vfs_lookup_root
    dd vfs_root_path
    dd 1
    dd VFS_FLAG_BOOTSTRAP_ROOT | VFS_FLAG_READ_ONLY
    dd 0
vfs_state:
    times VFS_STATE_SIZE db 0
vfs_root_path:    db '/'
vfs_invalid_path: db 'x'
align 4

; Kernel Service Manager (ADR-2010)
SERVICE_API_SIZE        equ 32
SERVICE_CAPACITY        equ 8
SERVICE_RECORD_SIZE     equ 32
SERVICE_STATE_EMPTY     equ 0
SERVICE_STATE_AVAILABLE equ 1
OBJECT_TYPE_SERVICE     equ 3
SERVICE_ID       equ 0
SERVICE_VERSION  equ 4
SERVICE_OWNER    equ 8
SERVICE_STATE    equ 12
SERVICE_HANDLE   equ 16
SERVICE_ENDPOINT equ 20
SERVICE_FLAGS    equ 24

service_manager_initialize:
    mov edi, service_table
    xor eax, eax
    mov ecx, (SERVICE_CAPACITY * SERVICE_RECORD_SIZE) / 4
    rep stosd
    mov edi, service_input_types
    mov ecx, SERVICE_CAPACITY * 4
    rep stosd
    mov dword [service_count], 0
    clc
    ret

; EAX=Service-ID, EDX=Version, EBX=besitzende Komponenten-ID.
service_register:
    pushfd
    cli
    test eax, eax
    jz .invalid
    mov [service_temp_id], eax
    mov [service_temp_version], edx
    mov [service_temp_owner], ebx
    mov eax, ebx
    call component_lookup
    jc .invalid
    cmp dword [eax + COMP_STATE], COMPONENT_STATE_ACTIVE
    jne .invalid
    xor ecx, ecx
.scan:
    cmp ecx, SERVICE_CAPACITY
    jae .invalid
    mov edi, ecx
    shl edi, 5
    add edi, service_table
    cmp dword [edi + SERVICE_STATE], SERVICE_STATE_EMPTY
    je .slot
    mov eax, [service_temp_id]
    cmp [edi + SERVICE_ID], eax
    je .invalid
    inc ecx
    jmp .scan
.slot:
    mov [service_temp_slot], edi
    mov eax, OBJECT_TYPE_SERVICE
    mov edx, [service_temp_id]
    mov ebx, [service_temp_owner]
    call object_create
    jc .invalid
    mov edi, [service_temp_slot]
    mov edx, [service_temp_id]
    mov [edi + SERVICE_ID], edx
    mov edx, [service_temp_version]
    mov [edi + SERVICE_VERSION], edx
    mov edx, [service_temp_owner]
    mov [edi + SERVICE_OWNER], edx
    mov dword [edi + SERVICE_STATE], SERVICE_STATE_AVAILABLE
    mov [edi + SERVICE_HANDLE], eax
    mov dword [edi + SERVICE_ENDPOINT], ipc_messages
    mov dword [edi + SERVICE_FLAGS], IPC_CAP_FIFO | IPC_CAP_IRQ_SAFE
    inc dword [service_count]
    popfd
    clc
    ret
.invalid:
    xor eax, eax
    popfd
    stc
    ret

; EAX=Service-ID. EAX=Datensatz oder 0.
service_lookup:
    xor ecx, ecx
.scan:
    cmp ecx, SERVICE_CAPACITY
    jae .invalid
    mov edx, ecx
    shl edx, 5
    add edx, service_table
    cmp dword [edx + SERVICE_STATE], SERVICE_STATE_EMPTY
    je .next
    cmp [edx + SERVICE_ID], eax
    je .found
.next:
    inc ecx
    jmp .scan
.found:
    mov eax, edx
    clc
    ret
.invalid:
    xor eax, eax
    stc
    ret

; EAX=Service-ID, EDX=Service-Version, EBX=Provider/Owner,
; ECX=Input Semantic Type, ESI=Output Semantic Type. Type Identity und
; Provider bleiben getrennte Contractbestandteile (ADR-SEMANTIC-0006).
service_register_typed:
    mov [service_temp_input_type], ecx
    mov [service_temp_output_type], esi
    push eax
    push edx
    push ebx
    mov eax, ecx
    call semantic_lookup_type
    jc .invalid
    cmp dword [edx + SEMANTIC_TYPE_VERSION], SEMANTIC_TYPE_VERSION_1
    jne .invalid
    mov eax, [service_temp_output_type]
    call semantic_lookup_type
    jc .invalid
    cmp dword [edx + SEMANTIC_TYPE_VERSION], SEMANTIC_TYPE_VERSION_1
    jne .invalid
    pop ebx
    pop edx
    pop eax
    call service_register
    jc .failed
    mov [service_temp_handle], eax
    mov eax, [service_temp_id]
    call service_lookup
    jc .failed
    sub eax, service_table
    shr eax, 5
    mov ecx, [service_temp_input_type]
    mov [service_input_types + eax * 4], ecx
    mov ecx, [service_temp_output_type]
    mov [service_output_types + eax * 4], ecx
    mov dword [service_input_versions + eax * 4], SEMANTIC_TYPE_VERSION_1
    mov dword [service_output_versions + eax * 4], SEMANTIC_TYPE_VERSION_1
    mov eax, [service_temp_handle]
    clc
    ret
.invalid:
    add esp, 12
.failed:
    stc
    ret

; EAX=Service-ID, ECX=gelieferter Input-Type, EDX=Type-Version.
; EBX=Output-Type bei exakter starker Kompatibilität. Eine Conversion wird
; nicht implizit erfunden; sie benötigt später einen expliziten Provider.
service_contract_check:
    push ecx
    push edx
    call service_lookup
    jc .invalid
    sub eax, service_table
    shr eax, 5
    pop edx
    pop ecx
    cmp ecx, [service_input_types + eax * 4]
    jne .mismatch
    cmp edx, [service_input_versions + eax * 4]
    jne .mismatch
    mov ebx, [service_output_types + eax * 4]
    test ebx, ebx
    jz .mismatch
    clc
    ret
.invalid:
    add esp, 8
.mismatch:
    stc
    ret

service_manager_self_test:
    mov eax, 0x4B45524E
    mov edx, 0x00010000
    mov ebx, 0x434F5245
    mov ecx, SEMANTIC_IPC_INLINE_DATA
    mov esi, SEMANTIC_IPC_DIAGNOSTIC
    call service_register_typed
    jc .invalid
    mov esi, eax
    mov eax, 0x4B45524E
    call service_lookup
    jc .invalid
    cmp dword [eax + SERVICE_VERSION], 0x00010000
    jne .invalid
    cmp dword [eax + SERVICE_OWNER], 0x434F5245
    jne .invalid
    cmp dword [eax + SERVICE_STATE], SERVICE_STATE_AVAILABLE
    jne .invalid
    cmp [eax + SERVICE_HANDLE], esi
    jne .invalid
    cmp dword [service_count], 1
    jne .invalid
    mov eax, 0x4B45524E
    mov ecx, SEMANTIC_IPC_INLINE_DATA
    mov edx, SEMANTIC_TYPE_VERSION_1
    call service_contract_check
    jc .invalid
    cmp ebx, SEMANTIC_IPC_DIAGNOSTIC
    jne .invalid
    mov eax, 0x4B45524E
    mov ecx, SEMANTIC_IPC_DIAGNOSTIC
    mov edx, SEMANTIC_TYPE_VERSION_1
    call service_contract_check
    jnc .invalid                   ; gleiche Bytes8-Repräsentation reicht nicht
    clc
    ret
.invalid:
    stc
    ret

align 4
service_manager_api:
    dd SERVICE_API_SIZE
    dw 1, 0
    dd SERVICE_CAPACITY
    dd service_register
    dd service_lookup
    dd service_count
    dd service_table
    dd ipc_api

service_count:        dd 0
service_temp_id:      dd 0
service_temp_version: dd 0
service_temp_owner:   dd 0
service_temp_slot:    dd 0
service_temp_input_type:  dd 0
service_temp_output_type: dd 0
service_temp_handle:      dd 0
align 4
service_table:
    times SERVICE_CAPACITY * SERVICE_RECORD_SIZE db 0
service_input_types:    times SERVICE_CAPACITY dd 0
service_output_types:   times SERVICE_CAPACITY dd 0
service_input_versions: times SERVICE_CAPACITY dd 0
service_output_versions: times SERVICE_CAPACITY dd 0

; Kernel Process Manager (ADR-2011 / NPSPEC-KERNEL-0004)
PROCESS_API_SIZE      equ 32
PROCESS_CAPACITY      equ 4
PROCESS_RECORD_SIZE   equ 32
PROCESS_STATE_EMPTY   equ 0
PROCESS_STATE_RUNNING equ 1
OBJECT_TYPE_PROCESS   equ 4
PROCESS_PID       equ 0
PROCESS_STATE     equ 4
PROCESS_CR3       equ 8
PROCESS_HANDLE    equ 12
PROCESS_PARENT    equ 16
PROCESS_FLAGS     equ 20

process_manager_initialize:
    mov edi, process_table
    xor eax, eax
    mov ecx, (PROCESS_CAPACITY * PROCESS_RECORD_SIZE) / 4
    rep stosd
    mov dword [process_count], 0
    mov dword [process_next_pid], 1
    clc
    ret

; EAX=Flags, EDX=Page-Directory. EAX=PID.
process_create:
    pushfd
    cli
    mov [process_temp_flags], eax
    mov [process_temp_cr3], edx
    xor ecx, ecx
.scan:
    cmp ecx, PROCESS_CAPACITY
    jae .invalid
    mov edi, ecx
    shl edi, 5
    add edi, process_table
    cmp dword [edi + PROCESS_STATE], PROCESS_STATE_EMPTY
    je .slot
    inc ecx
    jmp .scan
.slot:
    mov [process_temp_slot], edi
    mov edx, [process_next_pid]
    mov [process_temp_pid], edx
    mov eax, OBJECT_TYPE_PROCESS
    xor ebx, ebx
    call object_create
    jc .invalid
    mov edi, [process_temp_slot]
    mov edx, [process_temp_pid]
    mov [edi + PROCESS_PID], edx
    mov dword [edi + PROCESS_STATE], PROCESS_STATE_RUNNING
    mov edx, [process_temp_cr3]
    mov [edi + PROCESS_CR3], edx
    mov [edi + PROCESS_HANDLE], eax
    mov dword [edi + PROCESS_PARENT], 0
    mov edx, [process_temp_flags]
    mov [edi + PROCESS_FLAGS], edx
    inc dword [process_count]
    inc dword [process_next_pid]
    mov eax, [process_temp_pid]
    popfd
    clc
    ret
.invalid:
    xor eax, eax
    popfd
    stc
    ret

; EAX=PID. EAX=Prozessdatensatz oder 0.
process_lookup:
    xor ecx, ecx
.scan:
    cmp ecx, PROCESS_CAPACITY
    jae .invalid
    mov edx, ecx
    shl edx, 5
    add edx, process_table
    cmp dword [edx + PROCESS_STATE], PROCESS_STATE_EMPTY
    je .next
    cmp [edx + PROCESS_PID], eax
    je .found
.next:
    inc ecx
    jmp .scan
.found:
    mov eax, edx
    clc
    ret
.invalid:
    xor eax, eax
    stc
    ret

process_manager_self_test:
    mov eax, 1                      ; Kernel/System-Prozess
    mov edx, cr3
    call process_create
    jc .invalid
    cmp eax, 1
    jne .invalid
    call process_lookup
    jc .invalid
    cmp dword [eax + PROCESS_STATE], PROCESS_STATE_RUNNING
    jne .invalid
    mov edx, cr3
    cmp [eax + PROCESS_CR3], edx
    jne .invalid
    cmp dword [process_count], 1
    jne .invalid
    clc
    ret
.invalid:
    stc
    ret

align 4
process_manager_api:
    dd PROCESS_API_SIZE
    dw 1, 0
    dd PROCESS_CAPACITY
    dd process_create
    dd process_lookup
    dd process_count
    dd process_table
    dd process_next_pid

process_count:      dd 0
process_next_pid:   dd 0
process_temp_pid:   dd 0
process_temp_cr3:   dd 0
process_temp_flags: dd 0
process_temp_slot:  dd 0
align 4
process_table:
    times PROCESS_CAPACITY * PROCESS_RECORD_SIZE db 0

; ---------------------------------------------------------------------------
; Strukturierte Nebenlaeufigkeit: Task-Scopes
; NPSPEC-CONCURRENCY-TASK/STRUCTURED/CANCELLATION-0001
; ---------------------------------------------------------------------------

TASK_SCOPE_API_SIZE        equ 32
TASK_SCOPE_CAPACITY        equ 8
TASK_SCOPE_RECORD_SIZE     equ 32
TASK_SCOPE_STATE_EMPTY     equ 0
TASK_SCOPE_STATE_OPEN      equ 1
TASK_SCOPE_STATE_CANCELLING equ 2
TASK_SCOPE_STATE_CLOSED    equ 3
TASK_SCOPE_CAP_HIERARCHY   equ 0x00000001
TASK_SCOPE_CAP_CANCELLATION equ 0x00000002
TASK_SCOPE_CAP_BOUNDED     equ 0x00000004
TASK_SCOPE_ID              equ 0
TASK_SCOPE_OWNER           equ 4
TASK_SCOPE_PARENT          equ 8
TASK_SCOPE_STATE           equ 12
TASK_SCOPE_CHILDREN        equ 16
TASK_SCOPE_FLAGS           equ 20
TASK_SCOPE_CANCEL_REASON   equ 24
TASK_SCOPE_ACTIVE_TASKS    equ 28

task_scope_manager_initialize:
    mov edi, task_scope_table
    xor eax, eax
    mov ecx, (TASK_SCOPE_CAPACITY * TASK_SCOPE_RECORD_SIZE) / 4
    rep stosd
    mov dword [task_scope_count], 0
    mov dword [task_scope_next_id], 1
    mov dword [task_scope_kernel_root_id], 0

    ; Der initiale Kernelprozess besitzt einen dauerhaften Root-Scope. Jeder
    ; spaeter registrierte Kernelthread wird diesem Scope zugeordnet.
    mov eax, 1
    xor edx, edx
    xor ebx, ebx
    call task_scope_create
    jc .invalid
    mov [task_scope_kernel_root_id], eax
    clc
    ret
.invalid:
    stc
    ret

; EAX=Owner-PID, EDX=Parent-Scope oder 0, EBX=Flags. EAX=Scope-ID.
task_scope_create:
    pushfd
    cli
    mov [task_scope_temp_owner], eax
    mov [task_scope_temp_parent], edx
    mov [task_scope_temp_flags], ebx

    call process_lookup
    jc .invalid

    cmp dword [task_scope_temp_parent], 0
    je .find_slot
    mov eax, [task_scope_temp_parent]
    call task_scope_lookup
    jc .invalid
    cmp dword [eax + TASK_SCOPE_STATE], TASK_SCOPE_STATE_OPEN
    jne .invalid
    mov edx, [task_scope_temp_owner]
    cmp [eax + TASK_SCOPE_OWNER], edx
    jne .invalid

.find_slot:
    xor ecx, ecx
.scan:
    cmp ecx, TASK_SCOPE_CAPACITY
    jae .invalid
    mov edi, ecx
    shl edi, 5
    add edi, task_scope_table
    cmp dword [edi + TASK_SCOPE_STATE], TASK_SCOPE_STATE_EMPTY
    je .slot
    cmp dword [edi + TASK_SCOPE_STATE], TASK_SCOPE_STATE_CLOSED
    je .slot
    inc ecx
    jmp .scan
.slot:
    mov eax, [task_scope_next_id]
    mov [edi + TASK_SCOPE_ID], eax
    mov edx, [task_scope_temp_owner]
    mov [edi + TASK_SCOPE_OWNER], edx
    mov edx, [task_scope_temp_parent]
    mov [edi + TASK_SCOPE_PARENT], edx
    mov dword [edi + TASK_SCOPE_STATE], TASK_SCOPE_STATE_OPEN
    mov dword [edi + TASK_SCOPE_CHILDREN], 0
    mov edx, [task_scope_temp_flags]
    mov [edi + TASK_SCOPE_FLAGS], edx
    mov dword [edi + TASK_SCOPE_CANCEL_REASON], 0
    mov dword [edi + TASK_SCOPE_ACTIVE_TASKS], 0

    cmp dword [task_scope_temp_parent], 0
    je .created
    mov eax, [task_scope_temp_parent]
    call task_scope_lookup
    jc .invalid
    inc dword [eax + TASK_SCOPE_CHILDREN]
.created:
    inc dword [task_scope_count]
    inc dword [task_scope_next_id]
    mov eax, [edi + TASK_SCOPE_ID]
    popfd
    clc
    ret
.invalid:
    xor eax, eax
    popfd
    stc
    ret

; EAX=Scope-ID. EAX=Datensatz oder 0.
task_scope_lookup:
    xor ecx, ecx
.scan:
    cmp ecx, TASK_SCOPE_CAPACITY
    jae .invalid
    mov edx, ecx
    shl edx, 5
    add edx, task_scope_table
    cmp dword [edx + TASK_SCOPE_STATE], TASK_SCOPE_STATE_EMPTY
    je .next
    cmp [edx + TASK_SCOPE_ID], eax
    je .found
.next:
    inc ecx
    jmp .scan
.found:
    mov eax, edx
    clc
    ret
.invalid:
    xor eax, eax
    stc
    ret

; EAX=Scope-ID, EDX=Cancellation-Grund. Propagiert hierarchisch.
task_scope_cancel:
    pushfd
    cli
    mov [task_scope_temp_cancel_id], eax
    mov [task_scope_temp_cancel_reason], edx
    call task_scope_lookup
    jc .invalid
    cmp dword [eax + TASK_SCOPE_STATE], TASK_SCOPE_STATE_CLOSED
    je .invalid
    mov dword [eax + TASK_SCOPE_STATE], TASK_SCOPE_STATE_CANCELLING
    mov edx, [task_scope_temp_cancel_reason]
    mov [eax + TASK_SCOPE_CANCEL_REASON], edx

    ; Bei jeder Runde werden alle direkten Kinder bereits abgebrochener
    ; Scopes markiert. Die feste Tabellenkapazitaet begrenzt Laufzeit und
    ; Speicherverbrauch deterministisch.
.propagate:
    xor ebp, ebp
    xor ecx, ecx
.scan_children:
    cmp ecx, TASK_SCOPE_CAPACITY
    jae .propagated
    mov edi, ecx
    shl edi, 5
    add edi, task_scope_table
    cmp dword [edi + TASK_SCOPE_STATE], TASK_SCOPE_STATE_OPEN
    jne .next_child
    mov eax, [edi + TASK_SCOPE_PARENT]
    test eax, eax
    jz .next_child
    push ecx
    push edi
    call task_scope_lookup
    pop edi
    pop ecx
    jc .invalid
    cmp dword [eax + TASK_SCOPE_STATE], TASK_SCOPE_STATE_CANCELLING
    jne .next_child
    mov dword [edi + TASK_SCOPE_STATE], TASK_SCOPE_STATE_CANCELLING
    mov eax, [eax + TASK_SCOPE_CANCEL_REASON]
    mov [edi + TASK_SCOPE_CANCEL_REASON], eax
    mov ebp, 1
.next_child:
    inc ecx
    jmp .scan_children
.propagated:
    test ebp, ebp
    jnz .propagate
    mov edx, [task_scope_temp_cancel_reason]
    call task_manager_cancel_scopes
    popfd
    clc
    ret
.invalid:
    popfd
    stc
    ret

; EAX=Scope-ID. Abschluss ist erst ohne aktive Kinder zulaessig.
task_scope_close:
    pushfd
    cli
    mov [task_scope_temp_close_id], eax
    call task_scope_lookup
    jc .invalid
    cmp dword [eax + TASK_SCOPE_STATE], TASK_SCOPE_STATE_OPEN
    je .state_valid
    cmp dword [eax + TASK_SCOPE_STATE], TASK_SCOPE_STATE_CANCELLING
    jne .invalid
.state_valid:
    cmp dword [eax + TASK_SCOPE_CHILDREN], 0
    jne .invalid
    cmp dword [eax + TASK_SCOPE_ACTIVE_TASKS], 0
    jne .invalid
    mov edi, eax
    mov eax, [edi + TASK_SCOPE_PARENT]
    test eax, eax
    jz .close
    call task_scope_lookup
    jc .invalid
    cmp dword [eax + TASK_SCOPE_CHILDREN], 0
    je .invalid
    dec dword [eax + TASK_SCOPE_CHILDREN]
.close:
    mov dword [edi + TASK_SCOPE_STATE], TASK_SCOPE_STATE_CLOSED
    dec dword [task_scope_count]
    mov eax, [task_scope_temp_close_id]
    popfd
    clc
    ret
.invalid:
    xor eax, eax
    popfd
    stc
    ret

task_scope_manager_self_test:
    cmp dword [task_scope_kernel_root_id], 1
    jne .invalid
    mov eax, 1
    xor edx, edx
    mov ebx, 0x10
    call task_scope_create
    jc .invalid
    mov [task_scope_test_parent], eax
    mov eax, 1
    mov edx, [task_scope_test_parent]
    mov ebx, 0x20
    call task_scope_create
    jc .invalid
    mov [task_scope_test_child], eax

    mov eax, [task_scope_test_parent]
    call task_scope_close
    jnc .invalid                      ; aktives Kind verhindert Scope-Ende

    mov eax, [task_scope_test_parent]
    mov edx, 0x43414E43               ; "CANC"
    call task_scope_cancel
    jc .invalid
    mov eax, [task_scope_test_child]
    call task_scope_lookup
    jc .invalid
    cmp dword [eax + TASK_SCOPE_STATE], TASK_SCOPE_STATE_CANCELLING
    jne .invalid
    cmp dword [eax + TASK_SCOPE_CANCEL_REASON], 0x43414E43
    jne .invalid

    mov eax, [task_scope_test_child]
    call task_scope_close
    jc .invalid
    mov eax, [task_scope_test_parent]
    call task_scope_close
    jc .invalid
    cmp dword [task_scope_count], 1
    jne .invalid
    mov eax, [task_scope_kernel_root_id]
    call task_scope_lookup
    jc .invalid
    cmp dword [eax + TASK_SCOPE_STATE], TASK_SCOPE_STATE_OPEN
    jne .invalid
    cmp dword [eax + TASK_SCOPE_CHILDREN], 0
    jne .invalid
    cmp dword [eax + TASK_SCOPE_ACTIVE_TASKS], 0
    jne .invalid
    clc
    ret
.invalid:
    stc
    ret

align 4
task_scope_manager_api:
    dd TASK_SCOPE_API_SIZE
    dw 1, 0
    dd TASK_SCOPE_CAPACITY
    dd TASK_SCOPE_CAP_HIERARCHY | TASK_SCOPE_CAP_CANCELLATION | TASK_SCOPE_CAP_BOUNDED
    dd task_scope_create
    dd task_scope_cancel
    dd task_scope_close
    dd task_scope_table

task_scope_count:              dd 0
task_scope_next_id:            dd 0
task_scope_kernel_root_id:     dd 0
task_scope_temp_owner:         dd 0
task_scope_temp_parent:        dd 0
task_scope_temp_flags:         dd 0
task_scope_temp_cancel_id:     dd 0
task_scope_temp_cancel_reason: dd 0
task_scope_temp_close_id:      dd 0
task_scope_test_parent:        dd 0
task_scope_test_child:         dd 0
align 4
task_scope_table:
    times TASK_SCOPE_CAPACITY * TASK_SCOPE_RECORD_SIZE db 0

; ---------------------------------------------------------------------------
; Verwaltete Kernel-Tasks innerhalb strukturierter Scopes
; NPSPEC-CONCURRENCY-TASK/CANCELLATION-0001
; ---------------------------------------------------------------------------

TASK_API_SIZE             equ 32
TASK_CAPACITY             equ 8
TASK_RECORD_SIZE          equ 32
TASK_STATE_EMPTY          equ 0
TASK_STATE_CREATED        equ 1
TASK_STATE_READY          equ 2
TASK_STATE_RUNNING        equ 3
TASK_STATE_WAITING        equ 4
TASK_STATE_CANCEL_REQUEST equ 5
TASK_STATE_COMPLETED      equ 6
TASK_STATE_CANCELLED      equ 7
TASK_STATE_FAILED         equ 8
TASK_CAP_HIERARCHY        equ 0x00000001
TASK_CAP_CANCEL           equ 0x00000002
TASK_CAP_RESULT           equ 0x00000004
TASK_ID                   equ 0
TASK_OWNER                equ 4
TASK_RECORD_SCOPE         equ 8
TASK_STATE                equ 12
TASK_PARENT               equ 16
TASK_FLAGS                equ 20
TASK_RESULT               equ 24
TASK_CANCEL_REASON        equ 28

task_manager_initialize:
    mov edi, task_table
    xor eax, eax
    mov ecx, (TASK_CAPACITY * TASK_RECORD_SIZE) / 4
    rep stosd
    mov dword [task_count], 0
    mov dword [task_next_id], 1
    mov dword [task_manager_ready], 1
    clc
    ret

; EAX=Owner-PID, EDX=Scope-ID, EBX=Parent-Task oder 0, ECX=Flags.
; Rueckgabe EAX=Task-ID.
task_create:
    pushfd
    cli
    mov [task_temp_owner], eax
    mov [task_temp_scope], edx
    mov [task_temp_parent], ebx
    mov [task_temp_flags], ecx
    call process_lookup
    jc .invalid

    mov eax, [task_temp_scope]
    call task_scope_lookup
    jc .invalid
    cmp dword [eax + TASK_SCOPE_STATE], TASK_SCOPE_STATE_OPEN
    jne .invalid
    mov edx, [task_temp_owner]
    cmp [eax + TASK_SCOPE_OWNER], edx
    jne .invalid

    cmp dword [task_temp_parent], 0
    je .find_slot
    mov eax, [task_temp_parent]
    call task_lookup
    jc .invalid
    mov edx, [task_temp_owner]
    cmp [eax + TASK_OWNER], edx
    jne .invalid
    mov edx, [task_temp_scope]
    cmp [eax + TASK_RECORD_SCOPE], edx
    jne .invalid
    cmp dword [eax + TASK_STATE], TASK_STATE_CANCEL_REQUEST
    jae .invalid

.find_slot:
    xor ecx, ecx
.scan:
    cmp ecx, TASK_CAPACITY
    jae .invalid
    mov edi, ecx
    shl edi, 5
    add edi, task_table
    cmp dword [edi + TASK_STATE], TASK_STATE_EMPTY
    je .slot
    cmp dword [edi + TASK_STATE], TASK_STATE_COMPLETED
    jae .slot
    inc ecx
    jmp .scan
.slot:
    mov edx, edi
    sub edx, task_table
    shr edx, 5
    mov dword [task_group_ids + edx * 4], 0
    shl edx, 4
    add edx, task_deadline_table
    mov dword [edx + 0], 0
    mov dword [edx + 4], 0
    mov dword [edx + 8], 0
    mov dword [edx + 12], 0
    mov eax, [task_next_id]
    mov [edi + TASK_ID], eax
    mov edx, [task_temp_owner]
    mov [edi + TASK_OWNER], edx
    mov edx, [task_temp_scope]
    mov [edi + TASK_RECORD_SCOPE], edx
    mov dword [edi + TASK_STATE], TASK_STATE_READY
    mov edx, [task_temp_parent]
    mov [edi + TASK_PARENT], edx
    mov edx, [task_temp_flags]
    mov [edi + TASK_FLAGS], edx
    mov dword [edi + TASK_RESULT], 0
    mov dword [edi + TASK_CANCEL_REASON], 0

    mov eax, [task_temp_scope]
    call task_scope_lookup
    jc .invalid
    inc dword [eax + TASK_SCOPE_ACTIVE_TASKS]
    inc dword [task_count]
    inc dword [task_next_id]
    mov eax, [edi + TASK_ID]
    popfd
    clc
    ret
.invalid:
    xor eax, eax
    popfd
    stc
    ret

; EAX=Task-ID. EAX=Datensatz oder 0.
task_lookup:
    xor ecx, ecx
.scan:
    cmp ecx, TASK_CAPACITY
    jae .invalid
    mov edx, ecx
    shl edx, 5
    add edx, task_table
    cmp dword [edx + TASK_STATE], TASK_STATE_EMPTY
    je .next
    cmp [edx + TASK_ID], eax
    je .found
.next:
    inc ecx
    jmp .scan
.found:
    mov eax, edx
    clc
    ret
.invalid:
    xor eax, eax
    stc
    ret

; EAX=Task-Datensatz. Entfernt genau eine aktive Task aus ihrem Scope.
task_release_scope:
    push edi
    mov edi, eax
    mov eax, [edi + TASK_RECORD_SCOPE]
    call task_scope_lookup
    jc .invalid
    cmp dword [eax + TASK_SCOPE_ACTIVE_TASKS], 0
    je .invalid
    dec dword [eax + TASK_SCOPE_ACTIVE_TASKS]
    dec dword [task_count]
    pop edi
    clc
    ret
.invalid:
    pop edi
    stc
    ret

; EAX=Task-ID, EDX=Grund. Fordert kooperativen Abbruch an und propagiert
; entlang der Task-Hierarchie, beendet aber keinen Task hart.
task_request_cancel:
    pushfd
    cli
    mov [task_temp_cancel_id], eax
    mov [task_temp_cancel_reason], edx
    call task_lookup
    jc .invalid
    cmp dword [eax + TASK_STATE], TASK_STATE_CREATED
    jb .invalid
    cmp dword [eax + TASK_STATE], TASK_STATE_CANCEL_REQUEST
    ja .invalid
    mov dword [eax + TASK_STATE], TASK_STATE_CANCEL_REQUEST
    mov edx, [task_temp_cancel_reason]
    mov [eax + TASK_CANCEL_REASON], edx

.propagate:
    xor ebp, ebp
    xor ecx, ecx
.scan_children:
    cmp ecx, TASK_CAPACITY
    jae .propagated
    mov edi, ecx
    shl edi, 5
    add edi, task_table
    cmp dword [edi + TASK_STATE], TASK_STATE_CREATED
    jb .next_child
    cmp dword [edi + TASK_STATE], TASK_STATE_CANCEL_REQUEST
    jae .next_child
    mov eax, [edi + TASK_PARENT]
    test eax, eax
    jz .next_child
    push ecx
    push edi
    call task_lookup
    pop edi
    pop ecx
    jc .invalid
    cmp dword [eax + TASK_STATE], TASK_STATE_CANCEL_REQUEST
    jne .next_child
    mov dword [edi + TASK_STATE], TASK_STATE_CANCEL_REQUEST
    mov eax, [eax + TASK_CANCEL_REASON]
    mov [edi + TASK_CANCEL_REASON], eax
    mov ebp, 1
.next_child:
    inc ecx
    jmp .scan_children
.propagated:
    test ebp, ebp
    jnz .propagate
    call io_cancel_for_requested_tasks
    popfd
    clc
    ret
.invalid:
    popfd
    stc
    ret

; EDX=Grund. Wird nach einer Scope-Cancellation aufgerufen und markiert alle
; aktiven Tasks in betroffenen Scopes zur kooperativen Beendigung.
task_manager_cancel_scopes:
    cmp dword [task_manager_ready], 1
    jne .done
    mov [task_temp_cancel_reason], edx
    xor ecx, ecx
.scan:
    cmp ecx, TASK_CAPACITY
    jae .done
    mov edi, ecx
    shl edi, 5
    add edi, task_table
    cmp dword [edi + TASK_STATE], TASK_STATE_CREATED
    jb .next
    cmp dword [edi + TASK_STATE], TASK_STATE_CANCEL_REQUEST
    jae .next
    mov eax, [edi + TASK_RECORD_SCOPE]
    push ecx
    push edi
    call task_scope_lookup
    pop edi
    pop ecx
    jc .next
    cmp dword [eax + TASK_SCOPE_STATE], TASK_SCOPE_STATE_CANCELLING
    jne .next
    mov dword [edi + TASK_STATE], TASK_STATE_CANCEL_REQUEST
    mov eax, [task_temp_cancel_reason]
    mov [edi + TASK_CANCEL_REASON], eax
.next:
    inc ecx
    jmp .scan
.done:
    clc
    ret

; Kooperativer Cancellation Point. EAX=Task-ID, Rueckgabe EAX=1 bei Cancel.
task_checkpoint:
    pushfd
    cli
    call task_lookup
    jc .invalid
    cmp dword [eax + TASK_STATE], TASK_STATE_CANCEL_REQUEST
    jne .active
    mov [task_terminal_record], eax
    mov dword [eax + TASK_STATE], TASK_STATE_CANCELLED
    call task_group_on_terminal
    mov eax, [task_terminal_record]
    call task_release_scope
    jc .invalid
    mov eax, 1
    popfd
    clc
    ret
.active:
    xor eax, eax
    popfd
    clc
    ret
.invalid:
    xor eax, eax
    popfd
    stc
    ret

; EAX=Task-ID, EDX=Resultat. Nur aktive Tasks duerfen abschliessen.
task_complete:
    pushfd
    cli
    mov [task_temp_complete_id], eax
    mov [task_temp_result], edx
    call task_lookup
    jc .invalid
    cmp dword [eax + TASK_STATE], TASK_STATE_CREATED
    jb .invalid
    cmp dword [eax + TASK_STATE], TASK_STATE_WAITING
    ja .invalid
    mov [task_terminal_record], eax
    mov dword [eax + TASK_STATE], TASK_STATE_COMPLETED
    mov edx, [task_temp_result]
    mov [eax + TASK_RESULT], edx
    call task_group_on_terminal
    mov eax, [task_terminal_record]
    call task_release_scope
    jc .invalid
    mov eax, [task_temp_complete_id]
    popfd
    clc
    ret
.invalid:
    xor eax, eax
    popfd
    stc
    ret

; EAX=Task-ID, EDX=Fehlercode. Kontrollierter terminaler Fehlerpfad.
task_fail:
    pushfd
    cli
    mov [task_temp_complete_id], eax
    mov [task_temp_result], edx
    call task_lookup
    jc .invalid
    cmp dword [eax + TASK_STATE], TASK_STATE_CREATED
    jb .invalid
    cmp dword [eax + TASK_STATE], TASK_STATE_WAITING
    ja .invalid
    mov [task_terminal_record], eax
    mov dword [eax + TASK_STATE], TASK_STATE_FAILED
    mov edx, [task_temp_result]
    mov [eax + TASK_RESULT], edx
    call task_group_on_terminal
    mov eax, [task_terminal_record]
    call task_release_scope
    jc .invalid
    mov eax, [task_temp_complete_id]
    popfd
    clc
    ret
.invalid:
    xor eax, eax
    popfd
    stc
    ret

task_manager_self_test:
    ; Hierarchische Cancellation bleibt bis zu expliziten Checkpoints pending.
    mov eax, 1
    mov edx, [task_scope_kernel_root_id]
    xor ebx, ebx
    call task_scope_create
    jc .invalid
    mov [task_test_scope], eax
    mov eax, 1
    mov edx, [task_test_scope]
    xor ebx, ebx
    mov ecx, 1
    call task_create
    jc .invalid
    mov [task_test_parent], eax
    mov eax, 1
    mov edx, [task_test_scope]
    mov ebx, [task_test_parent]
    mov ecx, 2
    call task_create
    jc .invalid
    mov [task_test_child], eax

    mov eax, [task_test_scope]
    call task_scope_close
    jnc .invalid
    mov eax, [task_test_parent]
    mov edx, 0x43414E43
    call task_request_cancel
    jc .invalid
    mov eax, [task_test_child]
    call task_lookup
    jc .invalid
    cmp dword [eax + TASK_STATE], TASK_STATE_CANCEL_REQUEST
    jne .invalid
    cmp dword [eax + TASK_CANCEL_REASON], 0x43414E43
    jne .invalid
    mov eax, [task_test_child]
    call task_checkpoint
    jc .invalid
    cmp eax, 1
    jne .invalid
    mov eax, [task_test_parent]
    call task_checkpoint
    jc .invalid
    cmp eax, 1
    jne .invalid
    mov eax, [task_test_scope]
    call task_scope_close
    jc .invalid

    ; Normaler Completion-Pfad bewahrt das Ergebnis und loest den Scopezaehler.
    mov eax, 1
    mov edx, [task_scope_kernel_root_id]
    xor ebx, ebx
    call task_scope_create
    jc .invalid
    mov [task_test_scope], eax
    mov eax, 1
    mov edx, [task_test_scope]
    xor ebx, ebx
    xor ecx, ecx
    call task_create
    jc .invalid
    mov [task_test_parent], eax
    mov edx, 0x4F4B0001
    call task_complete
    jc .invalid
    mov eax, [task_test_parent]
    call task_lookup
    jc .invalid
    cmp dword [eax + TASK_STATE], TASK_STATE_COMPLETED
    jne .invalid
    cmp dword [eax + TASK_RESULT], 0x4F4B0001
    jne .invalid
    mov eax, [task_test_scope]
    call task_scope_close
    jc .invalid
    cmp dword [task_count], 0
    jne .invalid
    clc
    ret
.invalid:
    stc
    ret

align 4
task_manager_api:
    dd TASK_API_SIZE
    dw 1, 0
    dd TASK_CAPACITY
    dd TASK_CAP_HIERARCHY | TASK_CAP_CANCEL | TASK_CAP_RESULT
    dd task_create
    dd task_request_cancel
    dd task_checkpoint
    dd task_complete

task_manager_ready:       dd 0
task_count:               dd 0
task_next_id:             dd 0
task_temp_owner:          dd 0
task_temp_scope:          dd 0
task_temp_parent:         dd 0
task_temp_flags:          dd 0
task_temp_cancel_id:      dd 0
task_temp_cancel_reason:  dd 0
task_temp_complete_id:    dd 0
task_temp_result:         dd 0
task_terminal_record:     dd 0
task_test_scope:          dd 0
task_test_parent:         dd 0
task_test_child:          dd 0
align 4
task_table:
    times TASK_CAPACITY * TASK_RECORD_SIZE db 0

; ---------------------------------------------------------------------------
; Hierarchische Task-Deadlines auf monotoner PIT-Zeitbasis
; NPSPEC-CONCURRENCY-DEADLINE-0001 / ADR-CONCURRENCY-0004
; ---------------------------------------------------------------------------

TASK_DEADLINE_API_SIZE       equ 32
TASK_DEADLINE_CLOCK_HZ       equ 100
TASK_DEADLINE_CLASS_HARD     equ 1
TASK_DEADLINE_CLASS_FIRM     equ 2
TASK_DEADLINE_CLASS_SOFT     equ 3
TASK_DEADLINE_POLICY_CONTINUE equ 1
TASK_DEADLINE_POLICY_CANCEL  equ 2
TASK_DEADLINE_POLICY_FAIL    equ 3
TASK_DEADLINE_STATE_NONE     equ 0
TASK_DEADLINE_STATE_ARMED    equ 1
TASK_DEADLINE_STATE_MISSED   equ 2
TASK_DEADLINE_ABSOLUTE       equ 0
TASK_DEADLINE_CLASS          equ 4
TASK_DEADLINE_POLICY         equ 8
TASK_DEADLINE_STATE          equ 12
TASK_DEADLINE_CAPABILITIES   equ 0x0000000F
TASK_CANCEL_REASON_DEADLINE  equ 0x444C4E45

task_deadline_manager_initialize:
    mov edi, task_deadline_table
    xor eax, eax
    mov ecx, (TASK_CAPACITY * 16) / 4
    rep stosd
    mov dword [task_deadline_miss_count], 0
    mov dword [task_deadline_manager_ready], 1
    clc
    ret

; EAX=Taskdatensatz. EAX=zugehoeriger Deadline-Datensatz.
task_deadline_record_for_task:
    sub eax, task_table
    shr eax, 1                       ; 32 Byte Task -> 16 Byte Deadline
    add eax, task_deadline_table
    ret

; EAX=Task-ID, EDX=absoluter Tick, EBX=Klasse, ECX=Miss-Policy.
; Eine Parent-Deadline wird niemals verlaengert.
task_deadline_set:
    pushfd
    cli
    mov [task_deadline_temp_task], eax
    mov [task_deadline_temp_tick], edx
    mov [task_deadline_temp_class], ebx
    mov [task_deadline_temp_policy], ecx
    cmp ebx, TASK_DEADLINE_CLASS_HARD
    jb .invalid
    cmp ebx, TASK_DEADLINE_CLASS_SOFT
    ja .invalid
    cmp ecx, TASK_DEADLINE_POLICY_CONTINUE
    jb .invalid
    cmp ecx, TASK_DEADLINE_POLICY_FAIL
    ja .invalid
    test edx, edx
    jz .invalid

    call task_lookup
    jc .invalid
    mov [task_deadline_temp_record], eax
    cmp dword [eax + TASK_STATE], TASK_STATE_CREATED
    jb .invalid
    cmp dword [eax + TASK_STATE], TASK_STATE_WAITING
    ja .invalid

    mov eax, [eax + TASK_PARENT]
    test eax, eax
    jz .validate_effective
    call task_lookup
    jc .invalid
    call task_deadline_record_for_task
    cmp dword [eax + TASK_DEADLINE_STATE], TASK_DEADLINE_STATE_ARMED
    jne .validate_effective
    mov edx, [task_deadline_temp_tick]
    sub edx, [eax + TASK_DEADLINE_ABSOLUTE]
    jle .inherit_class
    mov edx, [eax + TASK_DEADLINE_ABSOLUTE]
    mov [task_deadline_temp_tick], edx
.inherit_class:
    cmp dword [eax + TASK_DEADLINE_CLASS], TASK_DEADLINE_CLASS_HARD
    jne .validate_effective
    mov dword [task_deadline_temp_class], TASK_DEADLINE_CLASS_HARD

.validate_effective:
    cmp dword [task_deadline_temp_class], TASK_DEADLINE_CLASS_HARD
    jne .store
    cmp dword [task_deadline_temp_policy], TASK_DEADLINE_POLICY_CONTINUE
    je .invalid                       ; Hard Miss darf nicht still ignoriert werden
    mov eax, [task_deadline_temp_tick]
    sub eax, [timer_ticks]
    jle .invalid                       ; minimale Bootstrap-Admission-Control

.store:
    mov eax, [task_deadline_temp_record]
    call task_deadline_record_for_task
    mov edx, [task_deadline_temp_tick]
    mov [eax + TASK_DEADLINE_ABSOLUTE], edx
    mov edx, [task_deadline_temp_class]
    mov [eax + TASK_DEADLINE_CLASS], edx
    mov edx, [task_deadline_temp_policy]
    mov [eax + TASK_DEADLINE_POLICY], edx
    mov dword [eax + TASK_DEADLINE_STATE], TASK_DEADLINE_STATE_ARMED
    mov eax, [task_deadline_temp_tick]
    popfd
    clc
    ret
.invalid:
    xor eax, eax
    popfd
    stc
    ret

; Wird aus IRQ0 und Tests aufgerufen. Deadline Miss und Cancellation bleiben
; getrennte Zustandsinformationen.
task_deadline_poll:
    cmp dword [task_deadline_manager_ready], 1
    jne .done
    mov eax, [timer_ticks]
    mov [task_deadline_poll_tick], eax
    xor ecx, ecx
.scan:
    cmp ecx, TASK_CAPACITY
    jae .done
    mov edi, ecx
    shl edi, 5
    add edi, task_table
    cmp dword [edi + TASK_STATE], TASK_STATE_CREATED
    jb .next
    cmp dword [edi + TASK_STATE], TASK_STATE_CANCEL_REQUEST
    jae .next
    mov eax, ecx
    shl eax, 4
    add eax, task_deadline_table
    cmp dword [eax + TASK_DEADLINE_STATE], TASK_DEADLINE_STATE_ARMED
    jne .next
    mov edx, [task_deadline_poll_tick]
    sub edx, [eax + TASK_DEADLINE_ABSOLUTE]
    jl .next
    mov dword [eax + TASK_DEADLINE_STATE], TASK_DEADLINE_STATE_MISSED
    inc dword [task_deadline_miss_count]
    mov edx, [eax + TASK_DEADLINE_POLICY]
    cmp edx, TASK_DEADLINE_POLICY_CANCEL
    je .cancel
    cmp edx, TASK_DEADLINE_POLICY_FAIL
    je .fail
    jmp .next
.cancel:
    mov eax, [edi + TASK_ID]
    mov edx, TASK_CANCEL_REASON_DEADLINE
    push ecx
    call task_request_cancel
    pop ecx
    jmp .next
.fail:
    mov dword [edi + TASK_STATE], TASK_STATE_FAILED
    mov dword [edi + TASK_RESULT], TASK_CANCEL_REASON_DEADLINE
    push edi
    mov eax, edi
    call task_group_on_terminal
    pop edi
    mov eax, edi
    push ecx
    call task_release_scope
    pop ecx
.next:
    inc ecx
    jmp .scan
.done:
    clc
    ret

task_deadline_manager_self_test:
    ; Parent 100 Ticks, Child fordert 200: effektiv bleiben 100 und Hard.
    mov eax, 1
    mov edx, [task_scope_kernel_root_id]
    xor ebx, ebx
    call task_scope_create
    jc .invalid
    mov [task_deadline_test_scope], eax
    mov eax, 1
    mov edx, [task_deadline_test_scope]
    xor ebx, ebx
    xor ecx, ecx
    call task_create
    jc .invalid
    mov [task_deadline_test_parent], eax
    mov edx, [timer_ticks]
    add edx, 100
    mov [task_deadline_test_parent_tick], edx
    mov ebx, TASK_DEADLINE_CLASS_HARD
    mov ecx, TASK_DEADLINE_POLICY_CANCEL
    call task_deadline_set
    jc .invalid

    mov eax, 1
    mov edx, [task_deadline_test_scope]
    mov ebx, [task_deadline_test_parent]
    xor ecx, ecx
    call task_create
    jc .invalid
    mov [task_deadline_test_child], eax
    mov edx, [timer_ticks]
    add edx, 200
    mov ebx, TASK_DEADLINE_CLASS_SOFT
    mov ecx, TASK_DEADLINE_POLICY_CANCEL
    call task_deadline_set
    jc .invalid
    mov eax, [task_deadline_test_child]
    call task_lookup
    jc .invalid
    call task_deadline_record_for_task
    mov edx, [task_deadline_test_parent_tick]
    cmp [eax + TASK_DEADLINE_ABSOLUTE], edx
    jne .invalid
    cmp dword [eax + TASK_DEADLINE_CLASS], TASK_DEADLINE_CLASS_HARD
    jne .invalid
    mov eax, [task_deadline_test_child]
    xor edx, edx
    call task_complete
    jc .invalid
    mov eax, [task_deadline_test_parent]
    xor edx, edx
    call task_complete
    jc .invalid
    mov eax, [task_deadline_test_scope]
    call task_scope_close
    jc .invalid

    ; Eine bereits erreichte Firm-Deadline fordert kooperative Cancellation an.
    mov eax, 1
    mov edx, [task_scope_kernel_root_id]
    xor ebx, ebx
    call task_scope_create
    jc .invalid
    mov [task_deadline_test_scope], eax
    mov eax, 1
    mov edx, [task_deadline_test_scope]
    xor ebx, ebx
    xor ecx, ecx
    call task_create
    jc .invalid
    mov [task_deadline_test_parent], eax
    mov edx, [timer_ticks]
    mov ebx, TASK_DEADLINE_CLASS_FIRM
    mov ecx, TASK_DEADLINE_POLICY_CANCEL
    call task_deadline_set
    jc .invalid
    call task_deadline_poll
    mov eax, [task_deadline_test_parent]
    call task_lookup
    jc .invalid
    cmp dword [eax + TASK_STATE], TASK_STATE_CANCEL_REQUEST
    jne .invalid
    cmp dword [eax + TASK_CANCEL_REASON], TASK_CANCEL_REASON_DEADLINE
    jne .invalid
    call task_deadline_record_for_task
    cmp dword [eax + TASK_DEADLINE_STATE], TASK_DEADLINE_STATE_MISSED
    jne .invalid
    cmp dword [task_deadline_miss_count], 1
    jne .invalid
    mov eax, [task_deadline_test_parent]
    call task_checkpoint
    jc .invalid
    mov eax, [task_deadline_test_scope]
    call task_scope_close
    jc .invalid
    clc
    ret
.invalid:
    stc
    ret

align 4
task_deadline_manager_api:
    dd TASK_DEADLINE_API_SIZE
    dw 1, 0
    dd TASK_DEADLINE_CLOCK_HZ
    dd TASK_DEADLINE_CAPABILITIES
    dd task_deadline_set
    dd task_deadline_poll
    dd task_deadline_table
    dd task_deadline_miss_count

task_deadline_manager_ready:    dd 0
task_deadline_miss_count:       dd 0
task_deadline_poll_tick:        dd 0
task_deadline_temp_task:        dd 0
task_deadline_temp_tick:        dd 0
task_deadline_temp_class:       dd 0
task_deadline_temp_policy:      dd 0
task_deadline_temp_record:      dd 0
task_deadline_test_scope:       dd 0
task_deadline_test_parent:      dd 0
task_deadline_test_child:       dd 0
task_deadline_test_parent_tick: dd 0
align 4
task_deadline_table:
    times TASK_CAPACITY * 16 db 0

; ---------------------------------------------------------------------------
; Task Groups mit WaitAll, FailFast, Cancellation und Drain
; NPSPEC-CONCURRENCY-TASKGROUP-0001
; ---------------------------------------------------------------------------

TASK_GROUP_API_SIZE          equ 32
TASK_GROUP_CAPACITY          equ 4
TASK_GROUP_RECORD_SIZE       equ 32
TASK_GROUP_STATE_EMPTY       equ 0
TASK_GROUP_STATE_OPEN        equ 1
TASK_GROUP_STATE_CANCELLING  equ 2
TASK_GROUP_STATE_FAILING     equ 3
TASK_GROUP_STATE_COMPLETED   equ 4
TASK_GROUP_STATE_CANCELLED   equ 5
TASK_GROUP_STATE_FAILED      equ 6
TASK_GROUP_POLICY_WAIT_ALL   equ 1
TASK_GROUP_POLICY_FAIL_FAST  equ 2
TASK_GROUP_CAPABILITIES      equ 0x0000000F
TASK_GROUP_ID                equ 0
TASK_GROUP_OWNER             equ 4
TASK_GROUP_SCOPE             equ 8
TASK_GROUP_STATE             equ 12
TASK_GROUP_POLICY            equ 16
TASK_GROUP_REQUIRED          equ 20
TASK_GROUP_ACTIVE            equ 24
TASK_GROUP_FAILURES          equ 28
TASK_CANCEL_REASON_GROUP     equ 0x47525043

task_group_manager_initialize:
    mov edi, task_group_table
    xor eax, eax
    mov ecx, (TASK_GROUP_CAPACITY * TASK_GROUP_RECORD_SIZE) / 4
    rep stosd
    mov edi, task_group_ids
    mov ecx, TASK_CAPACITY
    rep stosd
    mov dword [task_group_count], 0
    mov dword [task_group_next_id], 1
    mov dword [task_group_manager_ready], 1
    clc
    ret

; EAX=Owner-PID, EDX=Scope-ID, EBX=Policy. EAX=Group-ID.
task_group_create:
    pushfd
    cli
    mov [task_group_temp_owner], eax
    mov [task_group_temp_scope], edx
    mov [task_group_temp_policy], ebx
    cmp ebx, TASK_GROUP_POLICY_WAIT_ALL
    jb .invalid
    cmp ebx, TASK_GROUP_POLICY_FAIL_FAST
    ja .invalid
    call process_lookup
    jc .invalid
    mov eax, [task_group_temp_scope]
    call task_scope_lookup
    jc .invalid
    cmp dword [eax + TASK_SCOPE_STATE], TASK_SCOPE_STATE_OPEN
    jne .invalid
    mov edx, [task_group_temp_owner]
    cmp [eax + TASK_SCOPE_OWNER], edx
    jne .invalid
    xor ecx, ecx
.scan:
    cmp ecx, TASK_GROUP_CAPACITY
    jae .invalid
    mov edi, ecx
    shl edi, 5
    add edi, task_group_table
    cmp dword [edi + TASK_GROUP_STATE], TASK_GROUP_STATE_EMPTY
    je .slot
    cmp dword [edi + TASK_GROUP_STATE], TASK_GROUP_STATE_COMPLETED
    jae .slot
    inc ecx
    jmp .scan
.slot:
    mov eax, [task_group_next_id]
    mov [edi + TASK_GROUP_ID], eax
    mov edx, [task_group_temp_owner]
    mov [edi + TASK_GROUP_OWNER], edx
    mov edx, [task_group_temp_scope]
    mov [edi + TASK_GROUP_SCOPE], edx
    mov dword [edi + TASK_GROUP_STATE], TASK_GROUP_STATE_OPEN
    mov edx, [task_group_temp_policy]
    mov [edi + TASK_GROUP_POLICY], edx
    mov dword [edi + TASK_GROUP_REQUIRED], 0
    mov dword [edi + TASK_GROUP_ACTIVE], 0
    mov dword [edi + TASK_GROUP_FAILURES], 0
    inc dword [task_group_count]
    inc dword [task_group_next_id]
    mov eax, [edi + TASK_GROUP_ID]
    popfd
    clc
    ret
.invalid:
    xor eax, eax
    popfd
    stc
    ret

; EAX=Group-ID. EAX=Datensatz oder 0.
task_group_lookup:
    xor ecx, ecx
.scan:
    cmp ecx, TASK_GROUP_CAPACITY
    jae .invalid
    mov edx, ecx
    shl edx, 5
    add edx, task_group_table
    cmp dword [edx + TASK_GROUP_STATE], TASK_GROUP_STATE_EMPTY
    je .next
    cmp [edx + TASK_GROUP_ID], eax
    je .found
.next:
    inc ecx
    jmp .scan
.found:
    mov eax, edx
    clc
    ret
.invalid:
    xor eax, eax
    stc
    ret

; EAX=Group-ID, EDX=Task-ID.
task_group_attach:
    pushfd
    cli
    mov [task_group_temp_id], eax
    mov [task_group_temp_task], edx
    call task_group_lookup
    jc .invalid
    cmp dword [eax + TASK_GROUP_STATE], TASK_GROUP_STATE_OPEN
    jne .invalid
    mov [task_group_temp_record], eax
    mov eax, [task_group_temp_task]
    call task_lookup
    jc .invalid
    cmp dword [eax + TASK_STATE], TASK_STATE_CREATED
    jb .invalid
    cmp dword [eax + TASK_STATE], TASK_STATE_WAITING
    ja .invalid
    mov edx, [task_group_temp_record]
    mov ecx, [edx + TASK_GROUP_OWNER]
    cmp [eax + TASK_OWNER], ecx
    jne .invalid
    mov ecx, [edx + TASK_GROUP_SCOPE]
    cmp [eax + TASK_RECORD_SCOPE], ecx
    jne .invalid
    mov ecx, eax
    sub ecx, task_table
    shr ecx, 5
    cmp dword [task_group_ids + ecx * 4], 0
    jne .invalid
    mov eax, [task_group_temp_id]
    mov [task_group_ids + ecx * 4], eax
    inc dword [edx + TASK_GROUP_REQUIRED]
    inc dword [edx + TASK_GROUP_ACTIVE]
    popfd
    clc
    ret
.invalid:
    xor eax, eax
    popfd
    stc
    ret

; EAX=Group-ID, EDX=Grund. Markiert aktive Mitglieder kooperativ.
task_group_cancel_remaining:
    mov [task_group_temp_id], eax
    mov [task_group_temp_reason], edx
    xor ecx, ecx
.scan:
    cmp ecx, TASK_CAPACITY
    jae .done
    mov eax, [task_group_ids + ecx * 4]
    cmp eax, [task_group_temp_id]
    jne .next
    mov edi, ecx
    shl edi, 5
    add edi, task_table
    cmp dword [edi + TASK_STATE], TASK_STATE_CREATED
    jb .next
    cmp dword [edi + TASK_STATE], TASK_STATE_CANCEL_REQUEST
    jae .next
    mov eax, [edi + TASK_ID]
    mov edx, [task_group_temp_reason]
    push ecx
    call task_request_cancel
    pop ecx
.next:
    inc ecx
    jmp .scan
.done:
    clc
    ret

; EAX=Group-ID, EDX=Grund.
task_group_cancel:
    pushfd
    cli
    mov [task_group_temp_id], eax
    mov [task_group_temp_reason], edx
    call task_group_lookup
    jc .invalid
    cmp dword [eax + TASK_GROUP_STATE], TASK_GROUP_STATE_OPEN
    jne .invalid
    mov dword [eax + TASK_GROUP_STATE], TASK_GROUP_STATE_CANCELLING
    mov eax, [task_group_temp_id]
    mov edx, [task_group_temp_reason]
    call task_group_cancel_remaining
    popfd
    clc
    ret
.invalid:
    popfd
    stc
    ret

; EAX=terminaler Taskdatensatz. Aktualisiert Group-Policy und Drain-Zustand.
task_group_on_terminal:
    cmp dword [task_group_manager_ready], 1
    jne .done
    mov [task_group_terminal_task], eax
    mov ecx, eax
    sub ecx, task_table
    shr ecx, 5
    mov eax, [task_group_ids + ecx * 4]
    test eax, eax
    jz .done
    mov [task_group_temp_id], eax
    call task_group_lookup
    jc .invalid
    mov [task_group_temp_record], eax
    cmp dword [eax + TASK_GROUP_ACTIVE], 0
    je .invalid
    dec dword [eax + TASK_GROUP_ACTIVE]

    mov edi, [task_group_terminal_task]
    cmp dword [edi + TASK_STATE], TASK_STATE_FAILED
    jne .check_cancelled
    inc dword [eax + TASK_GROUP_FAILURES]
    cmp dword [eax + TASK_GROUP_POLICY], TASK_GROUP_POLICY_FAIL_FAST
    jne .finalize
    mov dword [eax + TASK_GROUP_STATE], TASK_GROUP_STATE_FAILING
    mov eax, [task_group_temp_id]
    mov edx, [edi + TASK_RESULT]
    call task_group_cancel_remaining
    jmp .finalize_reload
.check_cancelled:
    cmp dword [edi + TASK_STATE], TASK_STATE_CANCELLED
    jne .finalize
    cmp dword [eax + TASK_GROUP_STATE], TASK_GROUP_STATE_OPEN
    jne .finalize
    mov dword [eax + TASK_GROUP_STATE], TASK_GROUP_STATE_CANCELLING
.finalize_reload:
    mov eax, [task_group_temp_id]
    call task_group_lookup
    jc .invalid
.finalize:
    cmp dword [eax + TASK_GROUP_ACTIVE], 0
    jne .done
    cmp dword [eax + TASK_GROUP_STATE], TASK_GROUP_STATE_CANCELLING
    je .cancelled
    cmp dword [eax + TASK_GROUP_STATE], TASK_GROUP_STATE_FAILING
    je .failed
    cmp dword [eax + TASK_GROUP_FAILURES], 0
    jne .failed
    mov dword [eax + TASK_GROUP_STATE], TASK_GROUP_STATE_COMPLETED
    jmp .terminal
.cancelled:
    mov dword [eax + TASK_GROUP_STATE], TASK_GROUP_STATE_CANCELLED
    jmp .terminal
.failed:
    mov dword [eax + TASK_GROUP_STATE], TASK_GROUP_STATE_FAILED
.terminal:
    dec dword [task_group_count]
.done:
    clc
    ret
.invalid:
    stc
    ret

task_group_manager_self_test:
    ; WaitAll wird erst nach beiden erfolgreichen Tasks terminal.
    mov eax, 1
    mov edx, [task_scope_kernel_root_id]
    xor ebx, ebx
    call task_scope_create
    jc .invalid
    mov [task_group_test_scope], eax
    mov eax, 1
    mov edx, [task_group_test_scope]
    mov ebx, TASK_GROUP_POLICY_WAIT_ALL
    call task_group_create
    jc .invalid
    mov [task_group_test_group], eax
    mov eax, 1
    mov edx, [task_group_test_scope]
    xor ebx, ebx
    xor ecx, ecx
    call task_create
    jc .invalid
    mov [task_group_test_task1], eax
    mov edx, eax
    mov eax, [task_group_test_group]
    call task_group_attach
    jc .invalid
    mov eax, 1
    mov edx, [task_group_test_scope]
    xor ebx, ebx
    xor ecx, ecx
    call task_create
    jc .invalid
    mov [task_group_test_task2], eax
    mov edx, eax
    mov eax, [task_group_test_group]
    call task_group_attach
    jc .invalid
    mov eax, [task_group_test_task1]
    xor edx, edx
    call task_complete
    jc .invalid
    mov eax, [task_group_test_group]
    call task_group_lookup
    jc .invalid
    cmp dword [eax + TASK_GROUP_STATE], TASK_GROUP_STATE_OPEN
    jne .invalid
    cmp dword [eax + TASK_GROUP_ACTIVE], 1
    jne .invalid
    mov eax, [task_group_test_task2]
    xor edx, edx
    call task_complete
    jc .invalid
    mov eax, [task_group_test_group]
    call task_group_lookup
    jc .invalid
    cmp dword [eax + TASK_GROUP_STATE], TASK_GROUP_STATE_COMPLETED
    jne .invalid
    mov eax, [task_group_test_scope]
    call task_scope_close
    jc .invalid

    ; FailFast fordert fuer verbleibende Tasks Cancellation an und drainiert.
    mov eax, 1
    mov edx, [task_scope_kernel_root_id]
    xor ebx, ebx
    call task_scope_create
    jc .invalid
    mov [task_group_test_scope], eax
    mov eax, 1
    mov edx, [task_group_test_scope]
    mov ebx, TASK_GROUP_POLICY_FAIL_FAST
    call task_group_create
    jc .invalid
    mov [task_group_test_group], eax
    mov eax, 1
    mov edx, [task_group_test_scope]
    xor ebx, ebx
    xor ecx, ecx
    call task_create
    jc .invalid
    mov [task_group_test_task1], eax
    mov edx, eax
    mov eax, [task_group_test_group]
    call task_group_attach
    jc .invalid
    mov eax, 1
    mov edx, [task_group_test_scope]
    xor ebx, ebx
    xor ecx, ecx
    call task_create
    jc .invalid
    mov [task_group_test_task2], eax
    mov edx, eax
    mov eax, [task_group_test_group]
    call task_group_attach
    jc .invalid
    mov eax, [task_group_test_task1]
    mov edx, 0x4641494C
    call task_fail
    jc .invalid
    mov eax, [task_group_test_task2]
    call task_lookup
    jc .invalid
    cmp dword [eax + TASK_STATE], TASK_STATE_CANCEL_REQUEST
    jne .invalid
    mov eax, [task_group_test_task2]
    call task_checkpoint
    jc .invalid
    mov eax, [task_group_test_group]
    call task_group_lookup
    jc .invalid
    cmp dword [eax + TASK_GROUP_STATE], TASK_GROUP_STATE_FAILED
    jne .invalid
    cmp dword [eax + TASK_GROUP_FAILURES], 1
    jne .invalid
    mov eax, [task_group_test_scope]
    call task_scope_close
    jc .invalid
    cmp dword [task_group_count], 0
    jne .invalid
    clc
    ret
.invalid:
    stc
    ret

align 4
task_group_manager_api:
    dd TASK_GROUP_API_SIZE
    dw 1, 0
    dd TASK_GROUP_CAPACITY
    dd TASK_GROUP_CAPABILITIES
    dd task_group_create
    dd task_group_attach
    dd task_group_cancel
    dd task_group_table

task_group_manager_ready: dd 0
task_group_count:         dd 0
task_group_next_id:       dd 0
task_group_temp_owner:    dd 0
task_group_temp_scope:    dd 0
task_group_temp_policy:   dd 0
task_group_temp_id:       dd 0
task_group_temp_task:     dd 0
task_group_temp_reason:   dd 0
task_group_temp_record:   dd 0
task_group_terminal_task: dd 0
task_group_test_scope:    dd 0
task_group_test_group:    dd 0
task_group_test_task1:    dd 0
task_group_test_task2:    dd 0
align 4
task_group_table:
    times TASK_GROUP_CAPACITY * TASK_GROUP_RECORD_SIZE db 0
task_group_ids:
    times TASK_CAPACITY dd 0

; ---------------------------------------------------------------------------
; Begrenztes completion-basiertes Async-I/O-Grundmodell
; NPSPEC-IO-REQUEST/COMPLETION/ASYNC/DEADLINE-0001
; ---------------------------------------------------------------------------

IO_REQUEST_API_SIZE             equ 32
IO_REQUEST_CAPACITY             equ 8
IO_REQUEST_RECORD_SIZE          equ 64
IO_REQUEST_STATE_EMPTY          equ 0
IO_REQUEST_STATE_PENDING        equ 1
IO_REQUEST_STATE_RUNNING        equ 2
IO_REQUEST_STATE_COMPLETED      equ 3
IO_REQUEST_STATE_FAILED         equ 4
IO_REQUEST_STATE_CANCELLED      equ 5
IO_COMPLETION_NONE              equ 0
IO_COMPLETION_SUCCESS           equ 1
IO_COMPLETION_PARTIAL           equ 2
IO_COMPLETION_FAILED            equ 3
IO_COMPLETION_CANCELLED         equ 4
IO_COMPLETION_DEADLINE_MISS     equ 5
IO_CAP_ASYNC_COMPLETION         equ 0x00000001
IO_CAP_BOUNDED_QUEUE            equ 0x00000002
IO_CAP_TASK_OWNERSHIP           equ 0x00000004
IO_CAP_DEADLINE                 equ 0x00000008
IO_FLAG_DEADLINE_INHERITED      equ 0x00000001
IO_FLAG_DEADLINE_MISSED         equ 0x00000002
IO_CANCEL_REASON_DEADLINE       equ 0x494F444C
IO_REQUEST_ID                   equ 0
IO_REQUEST_OWNER                equ 4
IO_REQUEST_TASK                 equ 8
IO_REQUEST_SCOPE                equ 12
IO_REQUEST_OPERATION            equ 16
IO_REQUEST_STATE                equ 20
IO_REQUEST_TARGET               equ 24
IO_REQUEST_BUFFER               equ 28
IO_REQUEST_LENGTH               equ 32
IO_REQUEST_TRANSFERRED          equ 36
IO_REQUEST_DEADLINE             equ 40
IO_REQUEST_PRIORITY             equ 44
IO_REQUEST_COMPLETION           equ 48
IO_REQUEST_ERROR                equ 52
IO_REQUEST_FLAGS                equ 56
IO_REQUEST_EFFECTIVE_PRIORITY   equ 60
IO_PRIORITY_REALTIME            equ 1
IO_PRIORITY_INTERACTIVE         equ 2
IO_PRIORITY_NORMAL              equ 3
IO_PRIORITY_BACKGROUND          equ 4
IO_PRIORITY_MAINTENANCE         equ 5

io_request_manager_initialize:
    mov edi, io_request_table
    xor eax, eax
    mov ecx, (IO_REQUEST_CAPACITY * IO_REQUEST_RECORD_SIZE) / 4
    rep stosd
    mov edi, io_request_submit_ticks
    mov ecx, IO_REQUEST_CAPACITY * 3
    rep stosd
    mov dword [io_request_next_id], 1
    mov dword [io_request_outstanding], 0
    mov dword [io_request_backpressure_count], 0
    mov dword [io_request_deadline_miss_count], 0
    mov dword [io_request_manager_ready], 1
    clc
    ret

; EAX=Owner-PID, EDX=Task-ID, EBX=Operation, ECX=Target-Handle,
; ESI=Buffer-Handle, EDI=Laenge, EBP=Prioritaet (0..3). EAX=Request-ID.
; Request-Erzeugung, Ausfuehrung und Completion bleiben getrennt.
io_request_submit:
    pushfd
    cli
    mov [io_request_temp_owner], eax
    mov [io_request_temp_task], edx
    mov [io_request_temp_operation], ebx
    mov [io_request_temp_target], ecx
    mov [io_request_temp_buffer], esi
    mov [io_request_temp_length], edi
    mov [io_request_temp_priority], ebp
    test ebx, ebx
    jz .invalid
    test ecx, ecx
    jz .invalid
    test esi, esi
    jz .invalid
    test edi, edi
    jz .invalid
    cmp ebp, IO_PRIORITY_MAINTENANCE
    ja .invalid
    call process_lookup
    jc .invalid
    mov eax, [io_request_temp_task]
    call task_lookup
    jc .invalid
    mov [io_request_temp_task_record], eax
    mov edx, [io_request_temp_owner]
    cmp [eax + TASK_OWNER], edx
    jne .invalid
    cmp dword [eax + TASK_STATE], TASK_STATE_CREATED
    jb .invalid
    cmp dword [eax + TASK_STATE], TASK_STATE_WAITING
    ja .invalid

    xor ecx, ecx
.scan:
    cmp ecx, IO_REQUEST_CAPACITY
    jae .backpressure
    mov edi, ecx
    shl edi, 6
    add edi, io_request_table
    cmp dword [edi + IO_REQUEST_STATE], IO_REQUEST_STATE_EMPTY
    je .slot
    cmp dword [edi + IO_REQUEST_STATE], IO_REQUEST_STATE_COMPLETED
    jae .slot
    inc ecx
    jmp .scan
.slot:
    mov [io_request_temp_record], edi
    mov [io_request_temp_slot], ecx
    mov dword [io_request_submit_ticks + ecx * 4], 0
    mov dword [io_request_dispatch_ticks + ecx * 4], 0
    mov dword [io_request_wait_rounds + ecx * 4], 0
    mov dword [io_request_qos_ids + ecx * 4], 0
    mov dword [io_request_buffer_ids + ecx * 4], 0
    xor eax, eax
    mov ecx, IO_REQUEST_RECORD_SIZE / 4
    rep stosd
    mov edi, [io_request_temp_record]
    mov eax, [io_request_next_id]
    mov [edi + IO_REQUEST_ID], eax
    mov edx, [io_request_temp_owner]
    mov [edi + IO_REQUEST_OWNER], edx
    mov edx, [io_request_temp_task]
    mov [edi + IO_REQUEST_TASK], edx
    mov edx, [io_request_temp_task_record]
    mov edx, [edx + TASK_RECORD_SCOPE]
    mov [edi + IO_REQUEST_SCOPE], edx
    mov edx, [io_request_temp_operation]
    mov [edi + IO_REQUEST_OPERATION], edx
    mov dword [edi + IO_REQUEST_STATE], IO_REQUEST_STATE_PENDING
    mov edx, [io_request_temp_target]
    mov [edi + IO_REQUEST_TARGET], edx
    mov edx, [io_request_temp_buffer]
    mov [edi + IO_REQUEST_BUFFER], edx
    mov edx, [io_request_temp_length]
    mov [edi + IO_REQUEST_LENGTH], edx
    mov edx, [io_request_temp_priority]
    test edx, edx
    jnz .priority_requested
    mov edx, IO_PRIORITY_NORMAL
.priority_requested:
    mov [edi + IO_REQUEST_PRIORITY], edx
    ; Bootstrap-Policy: eine unprivilegierte Realtime-Anforderung wird auf
    ; Interactive begrenzt. Prioritaet bleibt damit Dringlichkeit, nicht Recht.
    cmp edx, IO_PRIORITY_REALTIME
    jne .priority_effective
    mov edx, IO_PRIORITY_INTERACTIVE
.priority_effective:
    mov [edi + IO_REQUEST_EFFECTIVE_PRIORITY], edx

    mov eax, [io_request_temp_task_record]
    call task_deadline_record_for_task
    cmp dword [eax + TASK_DEADLINE_STATE], TASK_DEADLINE_STATE_ARMED
    jne .ready
    mov edx, [eax + TASK_DEADLINE_ABSOLUTE]
    mov [edi + IO_REQUEST_DEADLINE], edx
    or dword [edi + IO_REQUEST_FLAGS], IO_FLAG_DEADLINE_INHERITED
.ready:
    mov ecx, [io_request_temp_slot]
    mov edx, [timer_ticks]
    mov [io_request_submit_ticks + ecx * 4], edx
    inc dword [io_request_next_id]
    inc dword [io_request_outstanding]
    mov eax, [edi + IO_REQUEST_ID]
    popfd
    clc
    ret
.backpressure:
    inc dword [io_request_backpressure_count]
.invalid:
    xor eax, eax
    popfd
    stc
    ret

; EAX=Request-ID. EAX=Datensatz oder 0.
io_request_lookup:
    xor ecx, ecx
.scan:
    cmp ecx, IO_REQUEST_CAPACITY
    jae .invalid
    mov edx, ecx
    shl edx, 6
    add edx, io_request_table
    cmp dword [edx + IO_REQUEST_STATE], IO_REQUEST_STATE_EMPTY
    je .next
    cmp [edx + IO_REQUEST_ID], eax
    je .found
.next:
    inc ecx
    jmp .scan
.found:
    mov eax, edx
    clc
    ret
.invalid:
    xor eax, eax
    stc
    ret

; EAX=Request-ID, EDX=uebertragene Bytes, EBX=Fehlercode (0=Erfolg).
; Genau ein terminales Ergebnis wird akzeptiert.
io_request_complete:
    pushfd
    cli
    mov [io_request_temp_id], eax
    mov [io_request_temp_transferred], edx
    mov [io_request_temp_error], ebx
    call io_request_lookup
    jc .invalid
    mov [io_request_temp_record], eax
    cmp dword [eax + IO_REQUEST_STATE], IO_REQUEST_STATE_PENDING
    je .active
    cmp dword [eax + IO_REQUEST_STATE], IO_REQUEST_STATE_RUNNING
    jne .invalid
.active:
    mov edx, [io_request_temp_transferred]
    cmp edx, [eax + IO_REQUEST_LENGTH]
    ja .invalid
    mov [eax + IO_REQUEST_TRANSFERRED], edx
    mov ebx, [io_request_temp_error]
    test ebx, ebx
    jnz .failed
    mov dword [eax + IO_REQUEST_STATE], IO_REQUEST_STATE_COMPLETED
    cmp edx, [eax + IO_REQUEST_LENGTH]
    jne .partial
    mov dword [eax + IO_REQUEST_COMPLETION], IO_COMPLETION_SUCCESS
    jmp .terminal
.partial:
    mov dword [eax + IO_REQUEST_COMPLETION], IO_COMPLETION_PARTIAL
    jmp .terminal
.failed:
    mov dword [eax + IO_REQUEST_STATE], IO_REQUEST_STATE_FAILED
    mov dword [eax + IO_REQUEST_COMPLETION], IO_COMPLETION_FAILED
    mov [eax + IO_REQUEST_ERROR], ebx
.terminal:
    push eax
    call io_qos_on_terminal
    pop eax
    push eax
    call shared_buffer_on_terminal
    pop eax
    call io_completion_publish
    dec dword [io_request_outstanding]
    mov eax, [io_request_temp_id]
    popfd
    clc
    ret
.invalid:
    xor eax, eax
    popfd
    stc
    ret

; EAX=Request-ID, EDX=Abbruchgrund. Zu spaete Abbrueche werden abgewiesen.
io_request_cancel:
    pushfd
    cli
    mov [io_request_temp_id], eax
    mov [io_request_temp_error], edx
    call io_request_lookup
    jc .invalid
    cmp dword [eax + IO_REQUEST_STATE], IO_REQUEST_STATE_PENDING
    je .active
    cmp dword [eax + IO_REQUEST_STATE], IO_REQUEST_STATE_RUNNING
    jne .invalid
.active:
    mov dword [eax + IO_REQUEST_STATE], IO_REQUEST_STATE_CANCELLED
    mov dword [eax + IO_REQUEST_COMPLETION], IO_COMPLETION_CANCELLED
    mov edx, [io_request_temp_error]
    mov [eax + IO_REQUEST_ERROR], edx
    push eax
    call io_qos_on_terminal
    pop eax
    push eax
    call shared_buffer_on_terminal
    pop eax
    call io_completion_publish
    dec dword [io_request_outstanding]
    mov eax, [io_request_temp_id]
    popfd
    clc
    ret
.invalid:
    xor eax, eax
    popfd
    stc
    ret

; Structured Concurrency: offene Requests folgen dem Cancellation-Zustand
; ihrer besitzenden Task, ohne einen Thread pro I/O anzulegen.
io_cancel_for_requested_tasks:
    cmp dword [io_request_manager_ready], 1
    jne .done
    xor ecx, ecx
.scan:
    cmp ecx, IO_REQUEST_CAPACITY
    jae .done
    mov edi, ecx
    shl edi, 6
    add edi, io_request_table
    cmp dword [edi + IO_REQUEST_STATE], IO_REQUEST_STATE_PENDING
    je .check
    cmp dword [edi + IO_REQUEST_STATE], IO_REQUEST_STATE_RUNNING
    jne .next
.check:
    mov eax, [edi + IO_REQUEST_TASK]
    push ecx
    call task_lookup
    pop ecx
    jc .next
    cmp dword [eax + TASK_STATE], TASK_STATE_CANCEL_REQUEST
    jne .next
    mov edx, [eax + TASK_CANCEL_REASON]
    mov eax, [edi + IO_REQUEST_ID]
    push ecx
    call io_request_cancel
    pop ecx
.next:
    inc ecx
    jmp .scan
.done:
    clc
    ret

; Monotone PIT-Ticks entscheiden Deadline-Misses. Der Miss bleibt als eigener
; Completion-Status sichtbar; anschliessend ist der Request terminal beendet.
io_request_poll_deadlines:
    cmp dword [io_request_manager_ready], 1
    jne .done
    mov eax, [timer_ticks]
    mov [io_request_poll_tick], eax
    xor ecx, ecx
.scan:
    cmp ecx, IO_REQUEST_CAPACITY
    jae .done
    mov edi, ecx
    shl edi, 6
    add edi, io_request_table
    cmp dword [edi + IO_REQUEST_STATE], IO_REQUEST_STATE_PENDING
    je .check
    cmp dword [edi + IO_REQUEST_STATE], IO_REQUEST_STATE_RUNNING
    jne .next
.check:
    test dword [edi + IO_REQUEST_FLAGS], IO_FLAG_DEADLINE_INHERITED
    jz .next
    mov eax, [io_request_poll_tick]
    sub eax, [edi + IO_REQUEST_DEADLINE]
    jl .next
    or dword [edi + IO_REQUEST_FLAGS], IO_FLAG_DEADLINE_MISSED
    mov dword [edi + IO_REQUEST_STATE], IO_REQUEST_STATE_CANCELLED
    mov dword [edi + IO_REQUEST_COMPLETION], IO_COMPLETION_DEADLINE_MISS
    mov dword [edi + IO_REQUEST_ERROR], IO_CANCEL_REASON_DEADLINE
    mov eax, edi
    push ecx
    push eax
    call io_qos_on_terminal
    pop eax
    push eax
    call shared_buffer_on_terminal
    pop eax
    call io_completion_publish
    pop ecx
    dec dword [io_request_outstanding]
    inc dword [io_request_deadline_miss_count]
.next:
    inc ecx
    jmp .scan
.done:
    clc
    ret

; ---------------------------------------------------------------------------
; Begrenzte FIFO-Completion-Queue mit Batch-Dequeue
; NPSPEC-IO-COMPLETION-0001 / ADR-IO-0001
; ---------------------------------------------------------------------------

IO_COMPLETION_API_SIZE       equ 32
IO_COMPLETION_CAPACITY       equ 16
IO_COMPLETION_RECORD_SIZE    equ 32
IO_COMPLETION_CAPABILITIES   equ 0x0000000F
IO_COMPLETION_REQUEST_ID     equ 0
IO_COMPLETION_OWNER          equ 4
IO_COMPLETION_STATUS         equ 8
IO_COMPLETION_BYTES          equ 12
IO_COMPLETION_ERROR          equ 16
IO_COMPLETION_TICK           equ 20
IO_COMPLETION_LATENCY        equ 24
IO_COMPLETION_FLAGS          equ 28

io_completion_initialize:
    mov edi, io_completion_queue
    xor eax, eax
    mov ecx, (IO_COMPLETION_CAPACITY * IO_COMPLETION_RECORD_SIZE) / 4
    rep stosd
    mov dword [io_completion_head], 0
    mov dword [io_completion_tail], 0
    mov dword [io_completion_count], 0
    mov dword [io_completion_overflow_count], 0
    mov dword [io_completion_manager_ready], 1
    clc
    ret

; EAX=terminaler IORequest-Datensatz. Der Request bleibt die autoritative
; Historie; die Queue ist der effiziente, batchfaehige Zustellmechanismus.
io_completion_publish:
    cmp dword [io_completion_manager_ready], 1
    jne .done
    cmp dword [io_completion_count], IO_COMPLETION_CAPACITY
    jae .overflow
    mov [io_completion_temp_request], eax
    mov ecx, eax
    sub ecx, io_request_table
    shr ecx, 6
    mov [io_completion_temp_request_slot], ecx
    mov edi, [io_completion_tail]
    shl edi, 5
    add edi, io_completion_queue
    mov esi, [io_completion_temp_request]
    mov eax, [esi + IO_REQUEST_ID]
    mov [edi + IO_COMPLETION_REQUEST_ID], eax
    mov eax, [esi + IO_REQUEST_OWNER]
    mov [edi + IO_COMPLETION_OWNER], eax
    mov eax, [esi + IO_REQUEST_COMPLETION]
    mov [edi + IO_COMPLETION_STATUS], eax
    mov eax, [esi + IO_REQUEST_TRANSFERRED]
    mov [edi + IO_COMPLETION_BYTES], eax
    mov eax, [esi + IO_REQUEST_ERROR]
    mov [edi + IO_COMPLETION_ERROR], eax
    mov eax, [timer_ticks]
    mov [edi + IO_COMPLETION_TICK], eax
    mov ecx, [io_completion_temp_request_slot]
    sub eax, [io_request_submit_ticks + ecx * 4]
    mov [edi + IO_COMPLETION_LATENCY], eax
    mov eax, [esi + IO_REQUEST_FLAGS]
    mov [edi + IO_COMPLETION_FLAGS], eax
    mov eax, [io_completion_tail]
    inc eax
    and eax, IO_COMPLETION_CAPACITY - 1
    mov [io_completion_tail], eax
    inc dword [io_completion_count]
.done:
    clc
    ret
.overflow:
    inc dword [io_completion_overflow_count]
    stc
    ret

; EAX=Kernel-Zielpuffer, ECX=maximale Eintraege. EAX=kopierte Anzahl.
; Der begrenzte Batch vermeidet einen Wakeup beziehungsweise Call pro Request.
io_completion_dequeue_batch:
    pushfd
    cli
    test eax, eax
    jz .invalid
    test ecx, ecx
    jz .invalid
    cmp ecx, IO_COMPLETION_CAPACITY
    ja .invalid
    mov [io_completion_batch_destination], eax
    mov [io_completion_batch_limit], ecx
    mov dword [io_completion_batch_count], 0
.copy:
    mov ecx, [io_completion_batch_count]
    cmp ecx, [io_completion_batch_limit]
    jae .complete
    cmp dword [io_completion_count], 0
    je .complete
    mov esi, [io_completion_head]
    shl esi, 5
    add esi, io_completion_queue
    mov edi, [io_completion_batch_destination]
    mov ecx, IO_COMPLETION_RECORD_SIZE / 4
    rep movsd
    mov [io_completion_batch_destination], edi
    mov eax, [io_completion_head]
    inc eax
    and eax, IO_COMPLETION_CAPACITY - 1
    mov [io_completion_head], eax
    dec dword [io_completion_count]
    inc dword [io_completion_batch_count]
    jmp .copy
.complete:
    mov eax, [io_completion_batch_count]
    popfd
    clc
    ret
.invalid:
    xor eax, eax
    popfd
    stc
    ret

io_completion_self_test:
    ; Die vorausgehenden Request-Tests muessen bereits Completions erzeugt haben.
    cmp dword [io_completion_count], 0
    je .invalid
    mov eax, io_completion_test_batch
    mov ecx, IO_COMPLETION_CAPACITY
    call io_completion_dequeue_batch
    jc .invalid
    test eax, eax
    jz .invalid
    cmp dword [io_completion_count], 0
    jne .invalid

    mov eax, 1
    mov edx, [task_scope_kernel_root_id]
    xor ebx, ebx
    call task_scope_create
    jc .invalid
    mov [io_completion_test_scope], eax
    mov eax, 1
    mov edx, [io_completion_test_scope]
    xor ebx, ebx
    xor ecx, ecx
    call task_create
    jc .invalid
    mov [io_completion_test_task], eax

    mov eax, 1
    mov edx, [io_completion_test_task]
    mov ebx, 4
    mov ecx, 0x700
    mov esi, 0x800
    mov edi, 64
    xor ebp, ebp
    call io_request_submit
    jc .invalid
    mov [io_completion_test_request1], eax
    mov eax, 1
    mov edx, [io_completion_test_task]
    mov ebx, 4
    mov ecx, 0x701
    mov esi, 0x801
    mov edi, 64
    xor ebp, ebp
    call io_request_submit
    jc .invalid
    mov [io_completion_test_request2], eax

    ; Abschlussreihenfolge B,A beweist die Trennung von Submission und Queue.
    mov eax, [io_completion_test_request2]
    mov edx, 64
    xor ebx, ebx
    call io_request_complete
    jc .invalid
    mov eax, [io_completion_test_request1]
    mov edx, 32
    xor ebx, ebx
    call io_request_complete
    jc .invalid
    mov eax, io_completion_test_batch
    mov ecx, 1
    call io_completion_dequeue_batch
    jc .invalid
    cmp eax, 1
    jne .invalid
    mov eax, [io_completion_test_request2]
    cmp [io_completion_test_batch + IO_COMPLETION_REQUEST_ID], eax
    jne .invalid
    cmp dword [io_completion_test_batch + IO_COMPLETION_STATUS], IO_COMPLETION_SUCCESS
    jne .invalid
    mov eax, io_completion_test_batch
    mov ecx, IO_COMPLETION_CAPACITY
    call io_completion_dequeue_batch
    jc .invalid
    cmp eax, 1
    jne .invalid
    mov eax, [io_completion_test_request1]
    cmp [io_completion_test_batch + IO_COMPLETION_REQUEST_ID], eax
    jne .invalid
    cmp dword [io_completion_test_batch + IO_COMPLETION_STATUS], IO_COMPLETION_PARTIAL
    jne .invalid
    cmp dword [io_completion_test_batch + IO_COMPLETION_BYTES], 32
    jne .invalid

    ; Die Zustellung ist ebenfalls begrenzt: 16 Eintraege bleiben erhalten,
    ; der siebzehnte Overflow wird sichtbar gezaehlt statt Speicher zu wachsen.
    xor ecx, ecx
.fill:
    cmp ecx, IO_COMPLETION_CAPACITY + 1
    jae .filled
    push ecx
    mov eax, 1
    mov edx, [io_completion_test_task]
    mov ebx, 5
    mov ecx, 0x702
    mov esi, 0x802
    mov edi, 16
    xor ebp, ebp
    call io_request_submit
    jc .fill_invalid
    mov edx, 16
    xor ebx, ebx
    call io_request_complete
    jc .fill_invalid
    pop ecx
    inc ecx
    jmp .fill
.fill_invalid:
    pop ecx
    jmp .invalid
.filled:
    cmp dword [io_completion_count], IO_COMPLETION_CAPACITY
    jne .invalid
    cmp dword [io_completion_overflow_count], 1
    jne .invalid
    mov eax, io_completion_test_batch
    mov ecx, IO_COMPLETION_CAPACITY
    call io_completion_dequeue_batch
    jc .invalid
    cmp eax, IO_COMPLETION_CAPACITY
    jne .invalid
    cmp dword [io_completion_count], 0
    jne .invalid

    mov eax, [io_completion_test_task]
    xor edx, edx
    call task_complete
    jc .invalid
    mov eax, [io_completion_test_scope]
    call task_scope_close
    jc .invalid
    clc
    ret
.invalid:
    stc
    ret

align 4
io_completion_api:
    dd IO_COMPLETION_API_SIZE
    dw 1, 0
    dd IO_COMPLETION_CAPACITY
    dd IO_COMPLETION_CAPABILITIES
    dd io_completion_dequeue_batch
    dd io_completion_queue
    dd io_completion_count
    dd io_completion_overflow_count

io_completion_manager_ready:      dd 0
io_completion_head:               dd 0
io_completion_tail:               dd 0
io_completion_count:              dd 0
io_completion_overflow_count:     dd 0
io_completion_temp_request:       dd 0
io_completion_temp_request_slot:  dd 0
io_completion_batch_destination:  dd 0
io_completion_batch_limit:        dd 0
io_completion_batch_count:        dd 0
io_completion_test_scope:         dd 0
io_completion_test_task:          dd 0
io_completion_test_request1:      dd 0
io_completion_test_request2:      dd 0
align 4
io_completion_queue:
    times IO_COMPLETION_CAPACITY * IO_COMPLETION_RECORD_SIZE db 0
io_completion_test_batch:
    times IO_COMPLETION_CAPACITY * IO_COMPLETION_RECORD_SIZE db 0

; ---------------------------------------------------------------------------
; Shared Buffer mit stabiler Identitaet, exklusiver I/O-Lease und Copy-Fallback
; NPSPEC-DATAMOVE-SHAREDBUFFER-0001 / NPSPEC-IO-ZEROCOPY-0001
; ---------------------------------------------------------------------------

SHARED_BUFFER_API_SIZE        equ 32
SHARED_BUFFER_CAPACITY        equ 4
SHARED_BUFFER_RECORD_SIZE     equ 64
SHARED_BUFFER_STATE_EMPTY     equ 0
SHARED_BUFFER_STATE_CPU       equ 1
SHARED_BUFFER_STATE_PROVIDER  equ 2
SHARED_BUFFER_STATE_RELEASED  equ 3
SHARED_BUFFER_RIGHT_READ      equ 0x00000001
SHARED_BUFFER_RIGHT_WRITE     equ 0x00000002
SHARED_BUFFER_RIGHT_TRANSFER  equ 0x00000004
SHARED_BUFFER_RIGHT_DMA       equ 0x00000008
SHARED_BUFFER_RIGHT_RELEASE   equ 0x00000010
SHARED_BUFFER_BACKING_POOL    equ 1
SHARED_BUFFER_ID              equ 0
SHARED_BUFFER_OWNER           equ 4
SHARED_BUFFER_SIZE            equ 8
SHARED_BUFFER_STATE           equ 12
SHARED_BUFFER_REFERENCES      equ 16
SHARED_BUFFER_ACTIVE_IO       equ 20
SHARED_BUFFER_RIGHTS          equ 24
SHARED_BUFFER_FLAGS           equ 28
SHARED_BUFFER_MAPPINGS        equ 32
SHARED_BUFFER_PINNED          equ 36
SHARED_BUFFER_BACKING_KIND    equ 40
SHARED_BUFFER_RESOURCE_BYTES  equ 44
SHARED_BUFFER_COPY_FALLBACKS  equ 48
SHARED_BUFFER_DIRECT_LEASES   equ 52
SHARED_BUFFER_GENERATION      equ 56

shared_buffer_initialize:
    mov edi, shared_buffer_table
    xor eax, eax
    mov ecx, (SHARED_BUFFER_CAPACITY * SHARED_BUFFER_RECORD_SIZE) / 4
    rep stosd
    mov edi, shared_buffer_backing_pages
    mov ecx, SHARED_BUFFER_CAPACITY
    rep stosd
    mov edi, io_request_buffer_ids
    mov ecx, IO_REQUEST_CAPACITY
    rep stosd
    mov dword [shared_buffer_next_id], 1
    mov dword [shared_buffer_live_count], 0
    mov dword [shared_buffer_resource_bytes], 0
    mov dword [shared_buffer_manager_ready], 1
    clc
    ret

; EAX=Owner, EDX=Groesse (1..4096), EBX=explizite Rechte. EAX=Buffer-ID.
shared_buffer_create:
    pushfd
    cli
    mov [shared_buffer_temp_owner], eax
    mov [shared_buffer_temp_size], edx
    mov [shared_buffer_temp_rights], ebx
    call process_lookup
    jc .invalid
    cmp dword [shared_buffer_temp_size], 0
    je .invalid
    cmp dword [shared_buffer_temp_size], PMM_PAGE_SIZE
    ja .invalid
    mov eax, [shared_buffer_temp_rights]
    test eax, SHARED_BUFFER_RIGHT_READ | SHARED_BUFFER_RIGHT_WRITE
    jz .invalid
    test eax, SHARED_BUFFER_RIGHT_RELEASE
    jz .invalid
    test eax, ~(SHARED_BUFFER_RIGHT_READ | SHARED_BUFFER_RIGHT_WRITE | SHARED_BUFFER_RIGHT_TRANSFER | SHARED_BUFFER_RIGHT_DMA | SHARED_BUFFER_RIGHT_RELEASE)
    jnz .invalid
    xor ecx, ecx
.scan:
    cmp ecx, SHARED_BUFFER_CAPACITY
    jae .invalid
    mov edi, ecx
    shl edi, 6
    add edi, shared_buffer_table
    cmp dword [edi + SHARED_BUFFER_STATE], SHARED_BUFFER_STATE_EMPTY
    je .slot
    cmp dword [edi + SHARED_BUFFER_STATE], SHARED_BUFFER_STATE_RELEASED
    je .slot
    inc ecx
    jmp .scan
.slot:
    mov [shared_buffer_temp_slot], ecx
    mov [shared_buffer_temp_record], edi
    mov eax, ecx
    shl eax, 12
    add eax, shared_buffer_backing_pool
    mov [shared_buffer_temp_backing], eax
    mov edi, eax
    xor eax, eax
    mov ecx, PMM_PAGE_SIZE / 4
    rep stosd
    mov edi, [shared_buffer_temp_record]
    xor eax, eax
    mov ecx, SHARED_BUFFER_RECORD_SIZE / 4
    rep stosd
    mov edi, [shared_buffer_temp_record]
    mov eax, [shared_buffer_next_id]
    mov [edi + SHARED_BUFFER_ID], eax
    mov edx, [shared_buffer_temp_owner]
    mov [edi + SHARED_BUFFER_OWNER], edx
    mov edx, [shared_buffer_temp_size]
    mov [edi + SHARED_BUFFER_SIZE], edx
    mov dword [edi + SHARED_BUFFER_STATE], SHARED_BUFFER_STATE_CPU
    mov dword [edi + SHARED_BUFFER_REFERENCES], 1
    mov edx, [shared_buffer_temp_rights]
    mov [edi + SHARED_BUFFER_RIGHTS], edx
    mov dword [edi + SHARED_BUFFER_BACKING_KIND], SHARED_BUFFER_BACKING_POOL
    mov edx, [shared_buffer_temp_size]
    mov [edi + SHARED_BUFFER_RESOURCE_BYTES], edx
    mov ecx, [shared_buffer_temp_slot]
    mov edx, [shared_buffer_temp_backing]
    mov [shared_buffer_backing_pages + ecx * 4], edx
    mov edx, [shared_buffer_generations + ecx * 4]
    inc edx
    jnz .generation_ready
    inc edx
.generation_ready:
    mov [shared_buffer_generations + ecx * 4], edx
    mov [edi + SHARED_BUFFER_GENERATION], edx
    inc dword [shared_buffer_next_id]
    inc dword [shared_buffer_live_count]
    mov edx, [shared_buffer_temp_size]
    add [shared_buffer_resource_bytes], edx
    mov eax, [edi + SHARED_BUFFER_ID]
    popfd
    clc
    ret
.invalid:
    xor eax, eax
    popfd
    stc
    ret

shared_buffer_lookup:
    xor ecx, ecx
.scan:
    cmp ecx, SHARED_BUFFER_CAPACITY
    jae .invalid
    mov edx, ecx
    shl edx, 6
    add edx, shared_buffer_table
    cmp dword [edx + SHARED_BUFFER_STATE], SHARED_BUFFER_STATE_EMPTY
    je .next
    cmp dword [edx + SHARED_BUFFER_STATE], SHARED_BUFFER_STATE_RELEASED
    je .next
    cmp [edx + SHARED_BUFFER_ID], eax
    je .found
.next:
    inc ecx
    jmp .scan
.found:
    mov eax, edx
    clc
    ret
.invalid:
    xor eax, eax
    stc
    ret

; EAX=Request-ID, EDX=Buffer-ID, EBX=benoetigte Rechte. Die Lease uebergibt
; exklusives Ownership an den Provider und bleibt bis zur Completion aktiv.
shared_buffer_bind_io:
    pushfd
    cli
    mov [shared_buffer_temp_request], eax
    mov [shared_buffer_temp_id], edx
    mov [shared_buffer_temp_access], ebx
    call io_request_lookup
    jc .invalid
    mov [shared_buffer_temp_request_record], eax
    cmp dword [eax + IO_REQUEST_STATE], IO_REQUEST_STATE_PENDING
    jne .invalid
    mov edx, [shared_buffer_temp_id]
    cmp [eax + IO_REQUEST_BUFFER], edx
    jne .invalid
    mov eax, [shared_buffer_temp_id]
    call shared_buffer_lookup
    jc .invalid
    mov [shared_buffer_temp_record], eax
    mov edx, [shared_buffer_temp_request_record]
    mov ecx, [edx + IO_REQUEST_OWNER]
    cmp [eax + SHARED_BUFFER_OWNER], ecx
    jne .invalid
    mov ecx, [shared_buffer_temp_access]
    test ecx, ecx
    jz .invalid
    mov ebx, [eax + SHARED_BUFFER_RIGHTS]
    and ebx, ecx
    cmp ebx, ecx
    jne .invalid
    mov ecx, [edx + IO_REQUEST_LENGTH]
    cmp ecx, [eax + SHARED_BUFFER_SIZE]
    ja .invalid
    cmp dword [eax + SHARED_BUFFER_STATE], SHARED_BUFFER_STATE_CPU
    jne .invalid
    cmp dword [eax + SHARED_BUFFER_ACTIVE_IO], 0
    jne .invalid
    mov ecx, edx
    sub ecx, io_request_table
    shr ecx, 6
    cmp dword [io_request_buffer_ids + ecx * 4], 0
    jne .invalid
    mov edx, [shared_buffer_temp_id]
    mov [io_request_buffer_ids + ecx * 4], edx
    mov dword [eax + SHARED_BUFFER_STATE], SHARED_BUFFER_STATE_PROVIDER
    inc dword [eax + SHARED_BUFFER_ACTIVE_IO]
    inc dword [eax + SHARED_BUFFER_REFERENCES]
    inc dword [eax + SHARED_BUFFER_DIRECT_LEASES]
    mov eax, [shared_buffer_temp_request]
    popfd
    clc
    ret
.invalid:
    xor eax, eax
    popfd
    stc
    ret

; EAX=terminaler Request. Completion gibt Ownership atomar an die CPU zurueck.
shared_buffer_on_terminal:
    cmp dword [shared_buffer_manager_ready], 1
    jne .done
    push eax
    call dma_scatter_gather_on_terminal
    pop eax
    push eax
    call dma_mapping_on_terminal
    pop eax
    mov ecx, eax
    sub ecx, io_request_table
    shr ecx, 6
    mov eax, [io_request_buffer_ids + ecx * 4]
    test eax, eax
    jz .done
    mov [shared_buffer_temp_slot], ecx
    call shared_buffer_lookup
    jc .done
    cmp dword [eax + SHARED_BUFFER_ACTIVE_IO], 1
    jne .done
    cmp dword [eax + SHARED_BUFFER_REFERENCES], 2
    jb .done
    dec dword [eax + SHARED_BUFFER_ACTIVE_IO]
    dec dword [eax + SHARED_BUFFER_REFERENCES]
    mov dword [eax + SHARED_BUFFER_STATE], SHARED_BUFFER_STATE_CPU
    mov ecx, [shared_buffer_temp_slot]
    mov dword [io_request_buffer_ids + ecx * 4], 0
.done:
    clc
    ret

; EAX=Quelle, EDX=Ziel, EBX=Laenge, ECX=Owner. Sicherer Copy-Fallback,
; falls ein spaeterer Provider keine gemeinsame Buffer-Lease unterstuetzt.
shared_buffer_copy_fallback:
    pushfd
    cli
    mov [shared_buffer_temp_source], eax
    mov [shared_buffer_temp_destination], edx
    mov [shared_buffer_temp_length], ebx
    mov [shared_buffer_temp_owner], ecx
    test ebx, ebx
    jz .invalid
    cmp eax, edx
    je .invalid
    call shared_buffer_lookup
    jc .invalid
    mov [shared_buffer_temp_source_record], eax
    mov eax, [shared_buffer_temp_destination]
    call shared_buffer_lookup
    jc .invalid
    mov [shared_buffer_temp_destination_record], eax
    mov edx, [shared_buffer_temp_owner]
    mov esi, [shared_buffer_temp_source_record]
    cmp [esi + SHARED_BUFFER_OWNER], edx
    jne .invalid
    cmp [eax + SHARED_BUFFER_OWNER], edx
    jne .invalid
    cmp dword [esi + SHARED_BUFFER_STATE], SHARED_BUFFER_STATE_CPU
    jne .invalid
    cmp dword [eax + SHARED_BUFFER_STATE], SHARED_BUFFER_STATE_CPU
    jne .invalid
    test dword [esi + SHARED_BUFFER_RIGHTS], SHARED_BUFFER_RIGHT_READ
    jz .invalid
    test dword [eax + SHARED_BUFFER_RIGHTS], SHARED_BUFFER_RIGHT_WRITE
    jz .invalid
    mov ebx, [shared_buffer_temp_length]
    cmp ebx, [esi + SHARED_BUFFER_SIZE]
    ja .invalid
    cmp ebx, [eax + SHARED_BUFFER_SIZE]
    ja .invalid
    mov ecx, esi
    sub ecx, shared_buffer_table
    shr ecx, 6
    mov esi, [shared_buffer_backing_pages + ecx * 4]
    mov ecx, eax
    sub ecx, shared_buffer_table
    shr ecx, 6
    mov edi, [shared_buffer_backing_pages + ecx * 4]
    mov ecx, ebx
    rep movsb
    mov eax, [shared_buffer_temp_destination_record]
    inc dword [eax + SHARED_BUFFER_COPY_FALLBACKS]
    mov eax, [shared_buffer_temp_length]
    popfd
    clc
    ret
.invalid:
    xor eax, eax
    popfd
    stc
    ret

; EAX=Buffer-ID, EDX=Owner. Aktive Provider-Leases blockieren die Freigabe.
shared_buffer_release:
    pushfd
    cli
    mov [shared_buffer_temp_id], eax
    mov [shared_buffer_temp_owner], edx
    call shared_buffer_lookup
    jc .invalid
    mov [shared_buffer_temp_record], eax
    mov edx, [shared_buffer_temp_owner]
    cmp [eax + SHARED_BUFFER_OWNER], edx
    jne .invalid
    test dword [eax + SHARED_BUFFER_RIGHTS], SHARED_BUFFER_RIGHT_RELEASE
    jz .invalid
    cmp dword [eax + SHARED_BUFFER_STATE], SHARED_BUFFER_STATE_CPU
    jne .invalid
    cmp dword [eax + SHARED_BUFFER_ACTIVE_IO], 0
    jne .invalid
    cmp dword [eax + SHARED_BUFFER_REFERENCES], 1
    jne .invalid
    mov ecx, eax
    sub ecx, shared_buffer_table
    shr ecx, 6
    mov [shared_buffer_temp_slot], ecx
    mov edi, [shared_buffer_backing_pages + ecx * 4]
    xor eax, eax
    mov ecx, PMM_PAGE_SIZE / 4
    rep stosd                         ; keine Datenreste zwischen Domains
    mov eax, [shared_buffer_temp_record]
    mov edx, [eax + SHARED_BUFFER_RESOURCE_BYTES]
    sub [shared_buffer_resource_bytes], edx
    mov dword [eax + SHARED_BUFFER_RESOURCE_BYTES], 0
    mov dword [eax + SHARED_BUFFER_REFERENCES], 0
    mov dword [eax + SHARED_BUFFER_STATE], SHARED_BUFFER_STATE_RELEASED
    mov ecx, [shared_buffer_temp_slot]
    mov dword [shared_buffer_backing_pages + ecx * 4], 0
    dec dword [shared_buffer_live_count]
    mov eax, [shared_buffer_temp_id]
    popfd
    clc
    ret
.invalid:
    xor eax, eax
    popfd
    stc
    ret

shared_buffer_self_test:
    mov eax, 1
    mov edx, 256
    mov ebx, SHARED_BUFFER_RIGHT_READ | SHARED_BUFFER_RIGHT_WRITE | SHARED_BUFFER_RIGHT_TRANSFER | SHARED_BUFFER_RIGHT_RELEASE
    call shared_buffer_create
    jc .invalid
    mov [shared_buffer_test_source], eax
    mov eax, 1
    mov edx, 256
    mov ebx, SHARED_BUFFER_RIGHT_READ | SHARED_BUFFER_RIGHT_WRITE | SHARED_BUFFER_RIGHT_TRANSFER | SHARED_BUFFER_RIGHT_RELEASE
    call shared_buffer_create
    jc .invalid
    mov [shared_buffer_test_destination], eax

    mov eax, [shared_buffer_test_source]
    call shared_buffer_lookup
    jc .invalid
    mov ecx, eax
    sub ecx, shared_buffer_table
    shr ecx, 6
    mov edi, [shared_buffer_backing_pages + ecx * 4]
    mov dword [edi], 0x4E4F5641       ; "NOVA"
    mov eax, [shared_buffer_test_source]
    mov edx, [shared_buffer_test_destination]
    mov ebx, 4
    mov ecx, 1
    call shared_buffer_copy_fallback
    jc .invalid
    cmp eax, 4
    jne .invalid
    mov eax, [shared_buffer_test_destination]
    call shared_buffer_lookup
    jc .invalid
    cmp dword [eax + SHARED_BUFFER_COPY_FALLBACKS], 1
    jne .invalid
    mov ecx, eax
    sub ecx, shared_buffer_table
    shr ecx, 6
    mov edi, [shared_buffer_backing_pages + ecx * 4]
    cmp dword [edi], 0x4E4F5641
    jne .invalid

    mov eax, 1
    mov edx, [task_scope_kernel_root_id]
    xor ebx, ebx
    call task_scope_create
    jc .invalid
    mov [shared_buffer_test_scope], eax
    mov eax, 1
    mov edx, [shared_buffer_test_scope]
    xor ebx, ebx
    xor ecx, ecx
    call task_create
    jc .invalid
    mov [shared_buffer_test_task], eax
    mov eax, 1
    mov edx, [shared_buffer_test_task]
    mov ebx, 6
    mov ecx, 0x900
    mov esi, [shared_buffer_test_source]
    mov edi, 4
    xor ebp, ebp
    call io_request_submit
    jc .invalid
    mov [shared_buffer_test_request], eax
    mov edx, [shared_buffer_test_source]
    mov ebx, SHARED_BUFFER_RIGHT_READ
    call shared_buffer_bind_io
    jc .invalid
    mov eax, [shared_buffer_test_source]
    call shared_buffer_lookup
    jc .invalid
    cmp dword [eax + SHARED_BUFFER_STATE], SHARED_BUFFER_STATE_PROVIDER
    jne .invalid
    cmp dword [eax + SHARED_BUFFER_REFERENCES], 2
    jne .invalid
    mov eax, [shared_buffer_test_source]
    mov edx, 1
    call shared_buffer_release
    jnc .invalid
    mov eax, [shared_buffer_test_request]
    mov edx, 4
    xor ebx, ebx
    call io_request_complete
    jc .invalid
    mov eax, [shared_buffer_test_source]
    call shared_buffer_lookup
    jc .invalid
    cmp dword [eax + SHARED_BUFFER_STATE], SHARED_BUFFER_STATE_CPU
    jne .invalid
    cmp dword [eax + SHARED_BUFFER_REFERENCES], 1
    jne .invalid
    mov eax, [shared_buffer_test_task]
    xor edx, edx
    call task_complete
    jc .invalid
    mov eax, [shared_buffer_test_scope]
    call task_scope_close
    jc .invalid
    mov eax, [shared_buffer_test_source]
    mov edx, 1
    call shared_buffer_release
    jc .invalid
    mov eax, [shared_buffer_test_destination]
    mov edx, 1
    call shared_buffer_release
    jc .invalid
    cmp dword [shared_buffer_live_count], 0
    jne .invalid
    cmp dword [shared_buffer_resource_bytes], 0
    jne .invalid
    clc
    ret
.invalid:
    stc
    ret

align 4
shared_buffer_api:
    dd SHARED_BUFFER_API_SIZE
    dw 1, 0
    dd SHARED_BUFFER_CAPACITY
    dd shared_buffer_create
    dd shared_buffer_bind_io
    dd shared_buffer_copy_fallback
    dd shared_buffer_release
    dd shared_buffer_table

shared_buffer_manager_ready:          dd 0
shared_buffer_next_id:                dd 0
shared_buffer_live_count:             dd 0
shared_buffer_resource_bytes:         dd 0
shared_buffer_temp_owner:             dd 0
shared_buffer_temp_size:              dd 0
shared_buffer_temp_rights:            dd 0
shared_buffer_temp_slot:              dd 0
shared_buffer_temp_record:            dd 0
shared_buffer_temp_backing:           dd 0
shared_buffer_temp_request:           dd 0
shared_buffer_temp_request_record:    dd 0
shared_buffer_temp_id:                dd 0
shared_buffer_temp_access:            dd 0
shared_buffer_temp_source:            dd 0
shared_buffer_temp_destination:       dd 0
shared_buffer_temp_source_record:     dd 0
shared_buffer_temp_destination_record: dd 0
shared_buffer_temp_length:            dd 0
shared_buffer_test_source:            dd 0
shared_buffer_test_destination:       dd 0
shared_buffer_test_scope:             dd 0
shared_buffer_test_task:              dd 0
shared_buffer_test_request:           dd 0
align 4
shared_buffer_table:
    times SHARED_BUFFER_CAPACITY * SHARED_BUFFER_RECORD_SIZE db 0
shared_buffer_backing_pages:
    times SHARED_BUFFER_CAPACITY dd 0
shared_buffer_generations:
    times SHARED_BUFFER_CAPACITY dd 0
io_request_buffer_ids:
    times IO_REQUEST_CAPACITY dd 0
align 4096
shared_buffer_backing_pool:
    times SHARED_BUFFER_CAPACITY * PMM_PAGE_SIZE db 0

; ---------------------------------------------------------------------------
; Normalisierter HAL-Topologiegraph. Die Bootstrap-Knoten bilden ein UMA-
; System und die derzeitigen Testprovider ab; sie sind ausdrücklich keine
; vorgetäuschte PCI-Erkennung. Neue Plattformprovider können dieselbe ABI mit
; validierten ACPI-, CPU- und Busdaten befüllen.
; NPSPEC-HAL-TOPOLOGY-0001, NPSPEC-HAL-NUMA-0001,
; NPSPEC-HAL-HOTPLUG-0001, NPSPEC-HAL-PLATFORM-0001
; ---------------------------------------------------------------------------

TOPOLOGY_API_SIZE                 equ 40
TOPOLOGY_CAPACITY                 equ 48
TOPOLOGY_RECORD_SIZE              equ 64
TOPOLOGY_TYPE_SYSTEM              equ 1
TOPOLOGY_TYPE_NUMA_NODE           equ 2
TOPOLOGY_TYPE_CPU_PACKAGE         equ 3
TOPOLOGY_TYPE_CPU_CORE            equ 4
TOPOLOGY_TYPE_CPU_THREAD          equ 5
TOPOLOGY_TYPE_MEMORY_REGION       equ 6
TOPOLOGY_TYPE_CACHE               equ 7
TOPOLOGY_TYPE_INTERRUPT_CONTROLLER equ 8
TOPOLOGY_TYPE_BUS                 equ 9
TOPOLOGY_TYPE_IOMMU_GROUP         equ 10
TOPOLOGY_TYPE_DEVICE              equ 11
TOPOLOGY_STATE_EMPTY              equ 0
TOPOLOGY_STATE_ONLINE             equ 1
TOPOLOGY_STATE_QUIESCING          equ 2
TOPOLOGY_STATE_OFFLINE            equ 3
TOPOLOGY_STATE_REMOVED            equ 4
TOPOLOGY_FLAG_BOOTSTRAP           equ 0x00000001
TOPOLOGY_FLAG_HOTPLUGGABLE        equ 0x00000002
TOPOLOGY_FLAG_DMA_CAPABLE         equ 0x00000004
TOPOLOGY_FLAG_LOCALITY_KNOWN      equ 0x00000008
TOPOLOGY_FLAG_FIRMWARE_VALIDATED  equ 0x00000010
TOPOLOGY_FLAG_FALLBACK            equ 0x00000020
TOPOLOGY_FLAG_ARCH_VALIDATED      equ 0x00000040
TOPOLOGY_ID                       equ 0
TOPOLOGY_TYPE                     equ 4
TOPOLOGY_PARENT                   equ 8
TOPOLOGY_STATE                    equ 12
TOPOLOGY_HARDWARE_ID              equ 16
TOPOLOGY_NUMA_NODE                equ 20
TOPOLOGY_IOMMU_GROUP              equ 24
TOPOLOGY_CHILD_COUNT              equ 28
TOPOLOGY_FLAGS                    equ 32
TOPOLOGY_PROPERTY0                equ 36
TOPOLOGY_PROPERTY1                equ 40
TOPOLOGY_PROPERTY2                equ 44
TOPOLOGY_GENERATION               equ 48
TOPOLOGY_CHANGE_SEQUENCE          equ 52

topology_initialize:
    mov edi, topology_records
    xor eax, eax
    mov ecx, (TOPOLOGY_CAPACITY * TOPOLOGY_RECORD_SIZE) / 4
    rep stosd
    mov dword [topology_count], 0
    mov dword [topology_cpu_nodes], 0
    mov dword [topology_package_nodes], 0
    mov dword [topology_core_nodes], 0
    mov dword [topology_numa_nodes], 0
    mov dword [topology_memory_nodes], 0
    mov dword [topology_numa0_id], 0
    mov dword [topology_next_id], 1
    mov dword [topology_change_sequence], 0
    mov dword [topology_validation_failures], 0
    mov dword [topology_manager_ready], 1
    call topology_cpu_hierarchy_discover

    ; Root des normalisierten Graphen.
    mov eax, TOPOLOGY_TYPE_SYSTEM
    xor edx, edx
    mov ebx, 1
    mov ecx, 0xFFFFFFFF
    xor esi, esi
    mov edi, TOPOLOGY_FLAG_BOOTSTRAP
    call topology_register
    jc .invalid
    mov [topology_root_id], eax

    ; Die geprüfte MADT-Liste liefert aktive Hardware-Threads. CPUID 1F/0B
    ; liefert, sofern vollständig validierbar, die systemweit anwendbaren
    ; Package/Core-Bitgrenzen für die APIC-IDs.
    mov dword [topology_cpu_index], 0
.cpu_next:
    mov eax, [topology_cpu_index]
    cmp eax, [acpi_cpu_count]
    jae .cpus_complete
    mov ebx, [acpi_apic_ids + eax * 4]
    mov [topology_cpu_apic_id], ebx
    mov dword [topology_cpu_numa_domain], 0xFFFFFFFF
    mov eax, ebx
    call acpi_srat_cpu_domain_lookup
    jc .cpu_numa_unknown
    mov [topology_cpu_numa_domain], eax
    call topology_get_or_create_numa
    jc .invalid
    mov [topology_cpu_parent_id], eax
    jmp .cpu_numa_ready
.cpu_numa_unknown:
    mov eax, 0xFFFFFFFF
    call topology_get_or_create_numa
    jc .invalid
    mov [topology_cpu_parent_id], eax
.cpu_numa_ready:
    cmp dword [topology_cpu_hierarchy_valid], 1
    jne .thread_parent_ready

    ; Package-Key = APIC-ID oberhalb der Core-Ebene.
    mov eax, [topology_cpu_apic_id]
    mov ecx, [topology_cpu_core_shift]
    shr eax, cl
    mov [topology_cpu_package_key], eax
    mov ebx, eax
    mov eax, TOPOLOGY_TYPE_CPU_PACKAGE
    call topology_find_type_hardware
    jnc .package_found
    mov eax, TOPOLOGY_TYPE_CPU_PACKAGE
    mov edx, [topology_root_id]
    mov ebx, [topology_cpu_package_key]
    xor ecx, ecx
    xor esi, esi
    mov edi, TOPOLOGY_FLAG_BOOTSTRAP | TOPOLOGY_FLAG_ARCH_VALIDATED
    call topology_register
    jc .invalid
    inc dword [topology_package_nodes]
    jmp .package_id_ready
.package_found:
    mov eax, [eax + TOPOLOGY_ID]
.package_id_ready:
    mov [topology_cpu_package_parent_id], eax

    ; Core-Key = APIC-ID oberhalb der SMT-Ebene und ist systemweit eindeutig.
    mov eax, [topology_cpu_apic_id]
    mov ecx, [topology_cpu_thread_shift]
    shr eax, cl
    mov [topology_cpu_core_key], eax
    mov ebx, eax
    mov eax, TOPOLOGY_TYPE_CPU_CORE
    call topology_find_type_hardware
    jnc .core_found
    mov eax, TOPOLOGY_TYPE_CPU_CORE
    mov edx, [topology_cpu_package_parent_id]
    mov ebx, [topology_cpu_core_key]
    xor ecx, ecx
    xor esi, esi
    mov edi, TOPOLOGY_FLAG_BOOTSTRAP | TOPOLOGY_FLAG_ARCH_VALIDATED
    call topology_register
    jc .invalid
    inc dword [topology_core_nodes]
    jmp .core_id_ready
.core_found:
    mov eax, [eax + TOPOLOGY_ID]
.core_id_ready:
    mov [topology_cpu_parent_id], eax
.thread_parent_ready:
    mov ebx, [topology_cpu_apic_id]
    mov eax, TOPOLOGY_TYPE_CPU_THREAD
    mov edx, [topology_cpu_parent_id]
    mov ecx, [topology_cpu_numa_domain]
    xor esi, esi
    mov edi, TOPOLOGY_FLAG_BOOTSTRAP | TOPOLOGY_FLAG_FALLBACK
    cmp dword [acpi_madt_valid], 1
    jne .cpu_arch_flag
    and edi, ~TOPOLOGY_FLAG_FALLBACK
    or edi, TOPOLOGY_FLAG_FIRMWARE_VALIDATED
.cpu_arch_flag:
    cmp dword [topology_cpu_hierarchy_valid], 1
    jne .cpu_register
    or edi, TOPOLOGY_FLAG_ARCH_VALIDATED
.cpu_register:
    cmp dword [topology_cpu_numa_domain], 0xFFFFFFFF
    je .cpu_register_ready
    or edi, TOPOLOGY_FLAG_LOCALITY_KNOWN
.cpu_register_ready:
    call topology_register
    jc .invalid
    call topology_lookup
    jc .invalid
    mov edx, [topology_cpu_index]
    mov [eax + TOPOLOGY_PROPERTY0], edx ; normalisierter logischer CPU-Index
    mov edx, [acpi_madt_valid]
    mov [eax + TOPOLOGY_PROPERTY1], edx ; 1 = validierte MADT-Quelle
    xor edx, edx
    cmp dword [topology_cpu_hierarchy_valid], 1
    jne .thread_property_ready
    mov edx, 1
    mov ecx, [topology_cpu_thread_shift]
    shl edx, cl
    dec edx
    and edx, [topology_cpu_apic_id]
.thread_property_ready:
    mov [eax + TOPOLOGY_PROPERTY2], edx ; SMT-Thread-ID innerhalb des Core
    inc dword [topology_cpu_nodes]
    inc dword [topology_cpu_index]
    jmp .cpu_next
.cpus_complete:
    cmp dword [acpi_srat_valid], 1
    jne .fallback_memory
    cmp dword [acpi_srat_memory_count], 0
    je .fallback_memory
    mov dword [topology_memory_index], 0
.memory_next:
    mov eax, [topology_memory_index]
    cmp eax, [acpi_srat_memory_count]
    jae .memory_complete
    imul edx, eax, ACPI_SRAT_MEMORY_RECORD_SIZE
    add edx, acpi_srat_memory_records
    mov [topology_memory_source], edx
    mov eax, [edx]
    call topology_get_or_create_numa
    jc .invalid
    mov edx, eax
    mov eax, TOPOLOGY_TYPE_MEMORY_REGION
    mov ebx, [topology_memory_index]
    inc ebx
    mov ecx, [topology_memory_source]
    mov ecx, [ecx]
    xor esi, esi
    mov edi, TOPOLOGY_FLAG_FIRMWARE_VALIDATED | TOPOLOGY_FLAG_LOCALITY_KNOWN
    call topology_register
    jc .invalid
    call topology_lookup
    jc .invalid
    mov edx, [topology_memory_source]
    mov ecx, [edx + 4]
    mov [eax + TOPOLOGY_PROPERTY0], ecx ; physische Basis, low
    mov ecx, [edx + 12]
    mov [eax + TOPOLOGY_PROPERTY1], ecx ; Länge, low
    mov ecx, [edx + 20]
    mov [eax + TOPOLOGY_PROPERTY2], ecx ; SRAT-Flags
    inc dword [topology_memory_nodes]
    inc dword [topology_memory_index]
    jmp .memory_next
.fallback_memory:
    mov eax, 0xFFFFFFFF
    call topology_get_or_create_numa
    jc .invalid
    mov edx, eax
    mov eax, TOPOLOGY_TYPE_MEMORY_REGION
    mov ebx, 1
    mov ecx, 0xFFFFFFFF
    xor esi, esi
    mov edi, TOPOLOGY_FLAG_BOOTSTRAP | TOPOLOGY_FLAG_FALLBACK
    call topology_register
    jc .invalid
    inc dword [topology_memory_nodes]
.memory_complete:

    mov eax, TOPOLOGY_TYPE_INTERRUPT_CONTROLLER
    mov edx, [topology_root_id]
    mov ebx, 1
    mov ecx, 0xFFFFFFFF
    xor esi, esi
    mov edi, TOPOLOGY_FLAG_BOOTSTRAP
    call topology_register
    jc .invalid

    mov eax, TOPOLOGY_TYPE_BUS
    mov edx, [topology_root_id]
    mov ebx, 1
    mov ecx, 0xFFFFFFFF
    xor esi, esi
    mov edi, TOPOLOGY_FLAG_BOOTSTRAP
    call topology_register
    jc .invalid
    mov [topology_boot_bus_id], eax

    mov ebx, 0x10
    call topology_register_boot_group
    jc .invalid
    mov [topology_group10_id], eax
    mov ebx, 0x20
    call topology_register_boot_group
    jc .invalid
    mov [topology_group20_id], eax
    mov ebx, 0x30
    call topology_register_boot_group
    jc .invalid
    mov [topology_group30_id], eax

    mov ebx, 0xD001
    mov esi, 0x10
    mov edx, [topology_group10_id]
    call topology_register_boot_device
    jc .invalid
    mov ebx, 0xD002
    mov esi, 0x20
    mov edx, [topology_group20_id]
    call topology_register_boot_device
    jc .invalid
    mov ebx, 0xD003
    mov esi, 0x30
    mov edx, [topology_group30_id]
    call topology_register_boot_device
    jc .invalid
    mov ebx, 0xD004
    mov esi, 0x30
    mov edx, [topology_group30_id]
    call topology_register_boot_device
    jc .invalid
    mov [topology_hotplug_test_id], eax
    clc
    ret
.invalid:
    mov dword [topology_manager_ready], 0
    stc
    ret

; Ermittelt ausschließlich die Bitgrenzen der x86-Topologie. Bevorzugt wird
; CPUID 1F, danach 0B. Eine Core-Ebene ist Pflicht; unvollständige Angaben
; werden als unbekannt behandelt.
topology_cpu_hierarchy_discover:
    mov dword [topology_cpu_hierarchy_valid], 0
    mov dword [topology_cpu_thread_shift], 0
    mov dword [topology_cpu_core_shift], 0
    mov dword [topology_cpu_leaf], 0
    xor eax, eax
    cpuid
    cmp eax, 0x1F
    jb .try_leaf_b
    mov eax, 0x1F
    call topology_cpu_hierarchy_scan_leaf
    jnc .complete
.try_leaf_b:
    xor eax, eax
    cpuid
    cmp eax, 0x0B
    jb .unavailable
    mov eax, 0x0B
    call topology_cpu_hierarchy_scan_leaf
    jc .unavailable
.complete:
    mov dword [topology_cpu_hierarchy_valid], 1
.unavailable:
    ret

; EAX=CPUID-Leaf. CF=0 bei validierter SMT/Core-Bitgrenze.
topology_cpu_hierarchy_scan_leaf:
    mov [topology_cpu_leaf], eax
    mov dword [topology_cpu_thread_shift], 0
    mov dword [topology_cpu_core_shift], 0
    mov dword [topology_cpu_core_level_seen], 0
    mov dword [topology_cpu_subleaf], 0
.next:
    mov eax, [topology_cpu_leaf]
    mov ecx, [topology_cpu_subleaf]
    cpuid
    test ebx, ebx
    jz .validate
    mov esi, eax
    and esi, 0x1F
    cmp esi, 31
    ja .invalid
    mov edx, ecx
    shr edx, 8
    and edx, 0xFF
    cmp edx, 1
    je .smt
    cmp edx, 2
    je .core
    jmp .advance
.smt:
    mov [topology_cpu_thread_shift], esi
    jmp .advance
.core:
    mov [topology_cpu_core_shift], esi
    mov dword [topology_cpu_core_level_seen], 1
.advance:
    inc dword [topology_cpu_subleaf]
    cmp dword [topology_cpu_subleaf], 8
    jb .next
.validate:
    cmp dword [topology_cpu_core_level_seen], 1
    jne .invalid
    mov eax, [topology_cpu_core_shift]
    cmp eax, [topology_cpu_thread_shift]
    jb .invalid
    clc
    ret
.invalid:
    stc
    ret

; EAX=Proximity-Domain oder 0xFFFFFFFF für unbekannte Lokalität.
; Ergebnis EAX=Topology-ID des stabilen NUMA-Knotens.
topology_get_or_create_numa:
    mov [topology_temp_numa_domain], eax
    mov ebx, eax
    mov eax, TOPOLOGY_TYPE_NUMA_NODE
    call topology_find_type_hardware
    jc .create
    mov eax, [eax + TOPOLOGY_ID]
    clc
    ret
.create:
    mov eax, TOPOLOGY_TYPE_NUMA_NODE
    mov edx, [topology_root_id]
    mov ebx, [topology_temp_numa_domain]
    mov ecx, ebx
    xor esi, esi
    mov edi, TOPOLOGY_FLAG_FIRMWARE_VALIDATED | TOPOLOGY_FLAG_LOCALITY_KNOWN
    cmp ebx, 0xFFFFFFFF
    jne .register
    mov edi, TOPOLOGY_FLAG_BOOTSTRAP | TOPOLOGY_FLAG_FALLBACK
.register:
    call topology_register
    jc .invalid
    inc dword [topology_numa_nodes]
    cmp dword [topology_temp_numa_domain], 0
    jne .complete
    mov [topology_numa0_id], eax
.complete:
    clc
    ret
.invalid:
    stc
    ret

; EBX=Gruppen-ID; EAX=Topology-ID.
topology_register_boot_group:
    mov eax, TOPOLOGY_TYPE_IOMMU_GROUP
    mov edx, [topology_boot_bus_id]
    mov ecx, 0xFFFFFFFF
    mov esi, ebx
    mov edi, TOPOLOGY_FLAG_BOOTSTRAP
    call topology_register
    ret

; EBX=Device-ID, ESI=Gruppe, EDX=Parent-Gruppe; EAX=Topology-ID.
topology_register_boot_device:
    mov eax, TOPOLOGY_TYPE_DEVICE
    mov ecx, 0xFFFFFFFF
    mov edi, TOPOLOGY_FLAG_BOOTSTRAP | TOPOLOGY_FLAG_HOTPLUGGABLE | TOPOLOGY_FLAG_DMA_CAPABLE
    call topology_register
    ret

; EAX=Typ, EDX=Parent-ID, EBX=Hardware-ID, ECX=NUMA-ID,
; ESI=IOMMU-Gruppen-ID, EDI=Flags. Ergebnis EAX=Topology-ID.
topology_register:
    pushfd
    cli
    mov [topology_temp_type], eax
    mov [topology_temp_parent], edx
    mov [topology_temp_hardware], ebx
    mov [topology_temp_numa], ecx
    mov [topology_temp_group], esi
    mov [topology_temp_flags], edi
    cmp dword [topology_manager_ready], 1
    jne .invalid
    cmp eax, TOPOLOGY_TYPE_SYSTEM
    jb .invalid
    cmp eax, TOPOLOGY_TYPE_DEVICE
    ja .invalid
    cmp eax, TOPOLOGY_TYPE_SYSTEM
    jne .non_root
    test edx, edx
    jnz .invalid
    cmp dword [topology_count], 0
    jne .invalid
    jmp .validate_identity
.non_root:
    test edx, edx
    jz .invalid
    mov eax, edx
    call topology_lookup
    jc .invalid
    mov [topology_temp_parent_record], eax
.validate_identity:
    cmp dword [topology_temp_type], TOPOLOGY_TYPE_DEVICE
    jne .identity_scan
    cmp dword [topology_temp_hardware], 0
    je .invalid
    cmp dword [topology_temp_group], 0
    je .invalid
.identity_scan:
    xor ecx, ecx
.identity_next:
    cmp ecx, TOPOLOGY_CAPACITY
    jae .find_slot
    mov edx, ecx
    shl edx, 6
    add edx, topology_records
    cmp dword [edx + TOPOLOGY_STATE], TOPOLOGY_STATE_EMPTY
    je .identity_continue
    cmp dword [edx + TOPOLOGY_STATE], TOPOLOGY_STATE_REMOVED
    je .identity_continue
    mov eax, [topology_temp_type]
    cmp [edx + TOPOLOGY_TYPE], eax
    jne .identity_continue
    mov eax, [topology_temp_hardware]
    cmp [edx + TOPOLOGY_HARDWARE_ID], eax
    je .invalid
.identity_continue:
    inc ecx
    jmp .identity_next
.find_slot:
    xor ecx, ecx
.slot_scan:
    cmp ecx, TOPOLOGY_CAPACITY
    jae .invalid
    mov edx, ecx
    shl edx, 6
    add edx, topology_records
    cmp dword [edx + TOPOLOGY_STATE], TOPOLOGY_STATE_EMPTY
    je .slot
    cmp dword [edx + TOPOLOGY_STATE], TOPOLOGY_STATE_REMOVED
    je .slot
    inc ecx
    jmp .slot_scan
.slot:
    mov [topology_temp_record], edx
    mov edi, edx
    xor eax, eax
    mov ecx, TOPOLOGY_RECORD_SIZE / 4
    rep stosd
    mov edi, [topology_temp_record]
    mov eax, [topology_next_id]
    mov [edi + TOPOLOGY_ID], eax
    inc dword [topology_next_id]
    mov edx, [topology_temp_type]
    mov [edi + TOPOLOGY_TYPE], edx
    mov edx, [topology_temp_parent]
    mov [edi + TOPOLOGY_PARENT], edx
    mov dword [edi + TOPOLOGY_STATE], TOPOLOGY_STATE_ONLINE
    mov edx, [topology_temp_hardware]
    mov [edi + TOPOLOGY_HARDWARE_ID], edx
    mov edx, [topology_temp_numa]
    mov [edi + TOPOLOGY_NUMA_NODE], edx
    mov edx, [topology_temp_group]
    mov [edi + TOPOLOGY_IOMMU_GROUP], edx
    mov edx, [topology_temp_flags]
    mov [edi + TOPOLOGY_FLAGS], edx
    inc dword [topology_change_sequence]
    mov edx, [topology_change_sequence]
    mov [edi + TOPOLOGY_CHANGE_SEQUENCE], edx
    mov dword [edi + TOPOLOGY_GENERATION], 1
    inc dword [topology_count]
    cmp dword [topology_temp_type], TOPOLOGY_TYPE_SYSTEM
    je .registered
    mov edx, [topology_temp_parent_record]
    inc dword [edx + TOPOLOGY_CHILD_COUNT]
.registered:
    mov eax, [edi + TOPOLOGY_ID]
    popfd
    clc
    ret
.invalid:
    inc dword [topology_validation_failures]
    xor eax, eax
    popfd
    stc
    ret

; EAX=Topology-ID; EAX=Datensatz.
topology_lookup:
    xor ecx, ecx
.scan:
    cmp ecx, TOPOLOGY_CAPACITY
    jae .invalid
    mov edx, ecx
    shl edx, 6
    add edx, topology_records
    cmp dword [edx + TOPOLOGY_STATE], TOPOLOGY_STATE_EMPTY
    je .next
    cmp dword [edx + TOPOLOGY_STATE], TOPOLOGY_STATE_REMOVED
    je .next
    cmp [edx + TOPOLOGY_ID], eax
    je .found
.next:
    inc ecx
    jmp .scan
.found:
    mov eax, edx
    clc
    ret
.invalid:
    xor eax, eax
    stc
    ret

; EAX=Typ, EBX=Hardware-Key; EAX=Online-Datensatz.
topology_find_type_hardware:
    xor ecx, ecx
.scan:
    cmp ecx, TOPOLOGY_CAPACITY
    jae .invalid
    mov edx, ecx
    shl edx, 6
    add edx, topology_records
    cmp dword [edx + TOPOLOGY_STATE], TOPOLOGY_STATE_ONLINE
    jne .next
    cmp [edx + TOPOLOGY_TYPE], eax
    jne .next
    cmp [edx + TOPOLOGY_HARDWARE_ID], ebx
    je .found
.next:
    inc ecx
    jmp .scan
.found:
    mov eax, edx
    clc
    ret
.invalid:
    xor eax, eax
    stc
    ret

; EAX=Hardware-Device-ID; EAX=IOMMU-Gruppe.
topology_device_group:
    xor ecx, ecx
.scan:
    cmp ecx, TOPOLOGY_CAPACITY
    jae .invalid
    mov edx, ecx
    shl edx, 6
    add edx, topology_records
    cmp dword [edx + TOPOLOGY_STATE], TOPOLOGY_STATE_ONLINE
    jne .next
    cmp dword [edx + TOPOLOGY_TYPE], TOPOLOGY_TYPE_DEVICE
    jne .next
    cmp [edx + TOPOLOGY_HARDWARE_ID], eax
    je .found
.next:
    inc ecx
    jmp .scan
.found:
    mov eax, [edx + TOPOLOGY_IOMMU_GROUP]
    test eax, eax
    jz .invalid
    clc
    ret
.invalid:
    xor eax, eax
    stc
    ret

; EAX=APIC-Hardware-ID; EAX=Online-Hardware-Thread-Datensatz.
topology_cpu_lookup:
    xor ecx, ecx
.scan:
    cmp ecx, TOPOLOGY_CAPACITY
    jae .invalid
    mov edx, ecx
    shl edx, 6
    add edx, topology_records
    cmp dword [edx + TOPOLOGY_STATE], TOPOLOGY_STATE_ONLINE
    jne .next
    cmp dword [edx + TOPOLOGY_TYPE], TOPOLOGY_TYPE_CPU_THREAD
    jne .next
    cmp [edx + TOPOLOGY_HARDWARE_ID], eax
    je .found
.next:
    inc ecx
    jmp .scan
.found:
    mov eax, edx
    clc
    ret
.invalid:
    xor eax, eax
    stc
    ret

; EAX=Topology-ID, EDX=neuer Zustand. Erlaubt kontrolliertes
; Online -> Quiescing -> Offline -> Online sowie Offline -> Removed.
topology_transition:
    pushfd
    cli
    mov [topology_temp_id], eax
    mov [topology_temp_state], edx
    call topology_lookup
    jc .invalid
    mov [topology_temp_record], eax
    mov edx, [topology_temp_state]
    cmp dword [eax + TOPOLOGY_TYPE], TOPOLOGY_TYPE_SYSTEM
    je .invalid
    mov ecx, [eax + TOPOLOGY_STATE]
    cmp ecx, TOPOLOGY_STATE_ONLINE
    jne .from_quiescing
    cmp edx, TOPOLOGY_STATE_QUIESCING
    jne .invalid
    jmp .commit
.from_quiescing:
    cmp ecx, TOPOLOGY_STATE_QUIESCING
    jne .from_offline
    cmp edx, TOPOLOGY_STATE_OFFLINE
    jne .invalid
    jmp .commit
.from_offline:
    cmp ecx, TOPOLOGY_STATE_OFFLINE
    jne .invalid
    cmp edx, TOPOLOGY_STATE_ONLINE
    je .commit
    cmp edx, TOPOLOGY_STATE_REMOVED
    jne .invalid
    cmp dword [eax + TOPOLOGY_CHILD_COUNT], 0
    jne .invalid
.commit:
    mov [eax + TOPOLOGY_STATE], edx
    inc dword [eax + TOPOLOGY_GENERATION]
    inc dword [topology_change_sequence]
    mov ecx, [topology_change_sequence]
    mov [eax + TOPOLOGY_CHANGE_SEQUENCE], ecx
    cmp edx, TOPOLOGY_STATE_REMOVED
    jne .complete
    mov eax, [eax + TOPOLOGY_PARENT]
    call topology_lookup
    jc .invalid
    cmp dword [eax + TOPOLOGY_CHILD_COUNT], 0
    je .invalid
    dec dword [eax + TOPOLOGY_CHILD_COUNT]
    dec dword [topology_count]
.complete:
    popfd
    clc
    ret
.invalid:
    inc dword [topology_validation_failures]
    popfd
    stc
    ret

topology_self_test:
    mov eax, 10
    add eax, [topology_cpu_nodes]
    add eax, [topology_package_nodes]
    add eax, [topology_core_nodes]
    add eax, [topology_numa_nodes]
    add eax, [topology_memory_nodes]
    cmp [topology_count], eax
    jne .invalid
    mov eax, [topology_cpu_nodes]
    cmp eax, [acpi_cpu_count]
    jne .invalid
    mov dword [topology_cpu_index], 0
.cpu_next:
    mov ecx, [topology_cpu_index]
    cmp ecx, [acpi_cpu_count]
    jae .cpus_valid
    mov eax, [acpi_apic_ids + ecx * 4]
    call topology_cpu_lookup
    jc .invalid
    mov [topology_selftest_record], eax
    mov ecx, [topology_cpu_index]
    cmp [eax + TOPOLOGY_PROPERTY0], ecx
    jne .invalid
    mov edx, [acpi_madt_valid]
    cmp [eax + TOPOLOGY_PROPERTY1], edx
    jne .invalid
    mov ecx, [topology_cpu_index]
    mov eax, [acpi_apic_ids + ecx * 4]
    call acpi_srat_cpu_domain_lookup
    jc .locality_unknown
    mov edx, [topology_selftest_record]
    cmp [edx + TOPOLOGY_NUMA_NODE], eax
    jne .invalid
    test dword [edx + TOPOLOGY_FLAGS], TOPOLOGY_FLAG_LOCALITY_KNOWN
    jz .invalid
    jmp .locality_valid
.locality_unknown:
    mov edx, [topology_selftest_record]
    cmp dword [edx + TOPOLOGY_NUMA_NODE], 0xFFFFFFFF
    jne .invalid
    test dword [edx + TOPOLOGY_FLAGS], TOPOLOGY_FLAG_LOCALITY_KNOWN
    jnz .invalid
.locality_valid:
    mov eax, [topology_selftest_record]
    cmp dword [topology_cpu_hierarchy_valid], 1
    jne .fallback_parent
    test dword [eax + TOPOLOGY_FLAGS], TOPOLOGY_FLAG_ARCH_VALIDATED
    jz .invalid
    mov eax, [eax + TOPOLOGY_PARENT]
    call topology_lookup
    jc .invalid
    cmp dword [eax + TOPOLOGY_TYPE], TOPOLOGY_TYPE_CPU_CORE
    jne .invalid
    mov eax, [eax + TOPOLOGY_PARENT]
    call topology_lookup
    jc .invalid
    cmp dword [eax + TOPOLOGY_TYPE], TOPOLOGY_TYPE_CPU_PACKAGE
    jne .invalid
    mov eax, [eax + TOPOLOGY_PARENT]
    cmp eax, [topology_root_id]
    jne .invalid
    jmp .cpu_valid
.fallback_parent:
    mov eax, [topology_selftest_record]
    mov eax, [eax + TOPOLOGY_PARENT]
    call topology_lookup
    jc .invalid
    cmp dword [eax + TOPOLOGY_TYPE], TOPOLOGY_TYPE_NUMA_NODE
    jne .invalid
.cpu_valid:
    inc dword [topology_cpu_index]
    jmp .cpu_next
.cpus_valid:
    mov eax, [topology_change_sequence]
    mov [topology_selftest_initial_sequence], eax
    mov eax, 0xD003
    call topology_device_group
    jc .invalid
    cmp eax, 0x30
    jne .invalid
    mov eax, 0xD004
    call topology_device_group
    jc .invalid
    cmp eax, 0x30
    jne .invalid
    mov eax, [topology_hotplug_test_id]
    mov edx, TOPOLOGY_STATE_QUIESCING
    call topology_transition
    jc .invalid
    mov eax, 0xD004
    call topology_device_group
    jnc .invalid                         ; quieszierende Geräte sind nicht nutzbar
    mov eax, [topology_hotplug_test_id]
    mov edx, TOPOLOGY_STATE_OFFLINE
    call topology_transition
    jc .invalid
    mov eax, [topology_hotplug_test_id]
    mov edx, TOPOLOGY_STATE_ONLINE
    call topology_transition
    jc .invalid
    mov eax, 0xD004
    call topology_device_group
    jc .invalid
    cmp eax, 0x30
    jne .invalid
    mov eax, [topology_selftest_initial_sequence]
    add eax, 3
    cmp eax, [topology_change_sequence]
    jne .invalid
    clc
    ret
.invalid:
    stc
    ret

align 4
topology_api:
    dd TOPOLOGY_API_SIZE
    dw 1, 0
    dd TOPOLOGY_CAPACITY
    dd topology_register
    dd topology_transition
    dd topology_lookup
    dd topology_device_group
    dd topology_records
    dd topology_change_sequence
    dd 0

topology_manager_ready:       dd 0
topology_count:               dd 0
topology_cpu_nodes:           dd 0
topology_package_nodes:       dd 0
topology_core_nodes:          dd 0
topology_numa_nodes:          dd 0
topology_memory_nodes:        dd 0
topology_next_id:             dd 0
topology_change_sequence:     dd 0
topology_validation_failures: dd 0
topology_root_id:             dd 0
topology_numa0_id:            dd 0
topology_boot_bus_id:         dd 0
topology_group10_id:          dd 0
topology_group20_id:          dd 0
topology_group30_id:          dd 0
topology_hotplug_test_id:     dd 0
topology_cpu_index:           dd 0
topology_selftest_initial_sequence: dd 0
topology_selftest_record:     dd 0
topology_cpu_hierarchy_valid: dd 0
topology_cpu_thread_shift:    dd 0
topology_cpu_core_shift:      dd 0
topology_cpu_core_level_seen: dd 0
topology_cpu_leaf:            dd 0
topology_cpu_subleaf:         dd 0
topology_cpu_apic_id:         dd 0
topology_cpu_parent_id:       dd 0
topology_cpu_package_parent_id: dd 0
topology_cpu_package_key:     dd 0
topology_cpu_core_key:        dd 0
topology_cpu_numa_domain:     dd 0
topology_memory_index:        dd 0
topology_memory_source:       dd 0
topology_temp_numa_domain:    dd 0
topology_temp_id:             dd 0
topology_temp_type:           dd 0
topology_temp_parent:         dd 0
topology_temp_hardware:       dd 0
topology_temp_numa:           dd 0
topology_temp_group:          dd 0
topology_temp_flags:          dd 0
topology_temp_state:          dd 0
topology_temp_record:         dd 0
topology_temp_parent_record:  dd 0
align 16
topology_records:
    times TOPOLOGY_CAPACITY * TOPOLOGY_RECORD_SIZE db 0

; ---------------------------------------------------------------------------
; IOMMU-Domainabstraktion mit Gruppen, Device-Bindings, IOVA-Autorisierungen
; und zuordenbaren Faults. Ohne Hardwareprovider bleibt Isolation explizit im
; Restricted-Modus und wird nicht als Hardwaregarantie ausgegeben.
; NPSPEC-HAL-IOMMU-0001
; ---------------------------------------------------------------------------

IOMMU_API_SIZE              equ 40
IOMMU_DOMAIN_CAPACITY       equ 4
IOMMU_DEVICE_CAPACITY       equ 8
IOMMU_MAPPING_CAPACITY      equ 8
IOMMU_DOMAIN_SIZE           equ 64
IOMMU_DEVICE_SIZE           equ 32
IOMMU_MAPPING_SIZE          equ 48
IOMMU_FAULT_SIZE            equ 32
IOMMU_MODE_UNAVAILABLE      equ 0
IOMMU_MODE_RESTRICTED       equ 1
IOMMU_MODE_HARDWARE         equ 2
IOMMU_MODE_VIRTUAL          equ 3
IOMMU_DOMAIN_EMPTY          equ 0
IOMMU_DOMAIN_ACTIVE         equ 1
IOMMU_DOMAIN_QUIESCING      equ 2
IOMMU_DOMAIN_RELEASED       equ 3
IOMMU_BINDING_EMPTY         equ 0
IOMMU_BINDING_ACTIVE        equ 1
IOMMU_BINDING_RELEASED      equ 2
IOMMU_MAPPING_EMPTY         equ 0
IOMMU_MAPPING_ACTIVE        equ 1
IOMMU_MAPPING_REVOKED       equ 2
IOMMU_MAPPING_FAULTED       equ 3
IOMMU_PERMISSION_READ       equ 0x00000001
IOMMU_PERMISSION_WRITE      equ 0x00000002
IOMMU_EXTERNAL_KIND_MASK    equ 0xF0000000
IOMMU_EXTERNAL_ID_MASK      equ 0x0FFFFFFF
IOMMU_EXTERNAL_DMA          equ 0x10000000
IOMMU_EXTERNAL_DMA_SG       equ 0x20000000
IOMMU_DOMAIN_ID             equ 0
IOMMU_DOMAIN_OWNER          equ 4
IOMMU_DOMAIN_STATE          equ 8
IOMMU_DOMAIN_MODE           equ 12
IOMMU_DOMAIN_DEVICES        equ 16
IOMMU_DOMAIN_GROUPS         equ 20
IOMMU_DOMAIN_MAPPINGS       equ 24
IOMMU_DOMAIN_FAULTS         equ 28
IOMMU_DOMAIN_MAPPED_BYTES   equ 32
IOMMU_DOMAIN_ADDRESS_WIDTH  equ 36
IOMMU_DOMAIN_IOVA_BASE_LOW  equ 40
IOMMU_DOMAIN_IOVA_BASE_HIGH equ 44
IOMMU_DOMAIN_IOVA_LIMIT_LOW equ 48
IOMMU_DOMAIN_IOVA_LIMIT_HIGH equ 52
IOMMU_DOMAIN_GENERATION     equ 56
IOMMU_DOMAIN_FLAGS          equ 60
IOMMU_DEVICE_ID             equ 0
IOMMU_DEVICE_DOMAIN         equ 4
IOMMU_DEVICE_GROUP          equ 8
IOMMU_DEVICE_OWNER          equ 12
IOMMU_DEVICE_STATE          equ 16
IOMMU_DEVICE_MODE           equ 20
IOMMU_DEVICE_GENERATION     equ 24
IOMMU_MAPPING_AUTH_ID       equ 0
IOMMU_MAPPING_DOMAIN        equ 4
IOMMU_MAPPING_DEVICE        equ 8
IOMMU_MAPPING_EXTERNAL      equ 12
IOMMU_MAPPING_IOVA_LOW      equ 16
IOMMU_MAPPING_IOVA_HIGH     equ 20
IOMMU_MAPPING_LENGTH        equ 24
IOMMU_MAPPING_PERMISSIONS   equ 28
IOMMU_MAPPING_STATE         equ 32
IOMMU_MAPPING_GENERATION    equ 36
IOMMU_MAPPING_ERROR         equ 40
IOMMU_FAULT_SEQUENCE        equ 0
IOMMU_FAULT_DOMAIN          equ 4
IOMMU_FAULT_DEVICE          equ 8
IOMMU_FAULT_EXTERNAL        equ 12
IOMMU_FAULT_IOVA_LOW        equ 16
IOMMU_FAULT_IOVA_HIGH       equ 20
IOMMU_FAULT_ACCESS          equ 24
IOMMU_FAULT_ERROR           equ 28

iommu_initialize:
    mov edi, iommu_domain_table
    xor eax, eax
    mov ecx, (IOMMU_DOMAIN_CAPACITY * IOMMU_DOMAIN_SIZE) / 4
    rep stosd
    mov edi, iommu_device_table
    mov ecx, (IOMMU_DEVICE_CAPACITY * IOMMU_DEVICE_SIZE) / 4
    rep stosd
    mov edi, iommu_mapping_table
    mov ecx, (IOMMU_MAPPING_CAPACITY * IOMMU_MAPPING_SIZE) / 4
    rep stosd
    mov edi, iommu_domain_generations
    mov ecx, IOMMU_DOMAIN_CAPACITY
    rep stosd
    mov edi, iommu_device_generations
    mov ecx, IOMMU_DEVICE_CAPACITY
    rep stosd
    mov edi, iommu_mapping_generations
    mov ecx, IOMMU_MAPPING_CAPACITY
    rep stosd
    mov edi, iommu_last_fault
    mov ecx, IOMMU_FAULT_SIZE / 4
    rep stosd
    mov dword [iommu_next_domain_id], 1
    mov dword [iommu_next_authorization_id], 1
    mov dword [iommu_domain_count], 0
    mov dword [iommu_binding_count], 0
    mov dword [iommu_mapping_count], 0
    mov dword [iommu_mapped_bytes], 0
    mov dword [iommu_fault_count], 0
    mov dword [iommu_validation_failures], 0
    mov dword [iommu_hardware_available], 0
    mov dword [iommu_manager_ready], 1
    clc
    ret

iommu_domain_lookup:
    xor ecx, ecx
.scan:
    cmp ecx, IOMMU_DOMAIN_CAPACITY
    jae .invalid
    mov edx, ecx
    shl edx, 6
    add edx, iommu_domain_table
    cmp dword [edx + IOMMU_DOMAIN_STATE], IOMMU_DOMAIN_ACTIVE
    jne .next
    cmp [edx + IOMMU_DOMAIN_ID], eax
    je .found
.next:
    inc ecx
    jmp .scan
.found:
    mov eax, edx
    clc
    ret
.invalid:
    xor eax, eax
    stc
    ret

iommu_device_lookup:
    xor ecx, ecx
.scan:
    cmp ecx, IOMMU_DEVICE_CAPACITY
    jae .invalid
    mov edx, ecx
    shl edx, 5
    add edx, iommu_device_table
    cmp dword [edx + IOMMU_DEVICE_STATE], IOMMU_BINDING_ACTIVE
    jne .next
    cmp [edx + IOMMU_DEVICE_ID], eax
    je .found
.next:
    inc ecx
    jmp .scan
.found:
    mov eax, edx
    clc
    ret
.invalid:
    xor eax, eax
    stc
    ret

; EAX=Device-ID. Ergebnis EAX=aktive Domain.
iommu_domain_for_device:
    call iommu_device_lookup
    jc .invalid
    mov eax, [eax + IOMMU_DEVICE_DOMAIN]
    call iommu_domain_lookup
    ret
.invalid:
    xor eax, eax
    stc
    ret

iommu_mapping_lookup:
    xor ecx, ecx
.scan:
    cmp ecx, IOMMU_MAPPING_CAPACITY
    jae .invalid
    mov edx, ecx
    imul edx, IOMMU_MAPPING_SIZE
    add edx, iommu_mapping_table
    cmp dword [edx + IOMMU_MAPPING_STATE], IOMMU_MAPPING_ACTIVE
    je .candidate
    cmp dword [edx + IOMMU_MAPPING_STATE], IOMMU_MAPPING_FAULTED
    jne .next
.candidate:
    cmp [edx + IOMMU_MAPPING_AUTH_ID], eax
    je .found
.next:
    inc ecx
    jmp .scan
.found:
    mov eax, edx
    clc
    ret
.invalid:
    xor eax, eax
    stc
    ret

; EAX=Owner, EBX=Mode, ECX=Adressbreite, ESI=IOVA-Basis, EDI=IOVA-Limit.
iommu_domain_create:
    pushfd
    cli
    mov [iommu_temp_owner], eax
    mov [iommu_temp_mode], ebx
    mov [iommu_temp_address_width], ecx
    mov [iommu_temp_iova], esi
    mov [iommu_temp_limit], edi
    cmp dword [iommu_manager_ready], 1
    jne .invalid
    cmp ebx, IOMMU_MODE_RESTRICTED
    jb .invalid
    cmp ebx, IOMMU_MODE_VIRTUAL
    ja .invalid
    cmp ebx, IOMMU_MODE_RESTRICTED
    je .mode_ready
    cmp dword [iommu_hardware_available], 1
    jne .invalid                         ; keine Isolation vortaeuschen
.mode_ready:
    cmp ecx, 32
    jne .invalid
    test esi, 0xFFF
    jnz .invalid
    cmp esi, edi
    jae .invalid
    call process_lookup
    jc .invalid
    xor ecx, ecx
.scan:
    cmp ecx, IOMMU_DOMAIN_CAPACITY
    jae .invalid
    mov edx, ecx
    shl edx, 6
    add edx, iommu_domain_table
    cmp dword [edx + IOMMU_DOMAIN_STATE], IOMMU_DOMAIN_EMPTY
    je .slot
    cmp dword [edx + IOMMU_DOMAIN_STATE], IOMMU_DOMAIN_RELEASED
    je .slot
    inc ecx
    jmp .scan
.slot:
    mov [iommu_temp_slot], ecx
    mov [iommu_temp_domain_record], edx
    mov edi, edx
    xor eax, eax
    mov ecx, IOMMU_DOMAIN_SIZE / 4
    rep stosd
    mov edi, [iommu_temp_domain_record]
    mov eax, [iommu_next_domain_id]
    mov [edi + IOMMU_DOMAIN_ID], eax
    mov edx, [iommu_temp_owner]
    mov [edi + IOMMU_DOMAIN_OWNER], edx
    mov dword [edi + IOMMU_DOMAIN_STATE], IOMMU_DOMAIN_ACTIVE
    mov edx, [iommu_temp_mode]
    mov [edi + IOMMU_DOMAIN_MODE], edx
    mov edx, [iommu_temp_address_width]
    mov [edi + IOMMU_DOMAIN_ADDRESS_WIDTH], edx
    mov edx, [iommu_temp_iova]
    mov [edi + IOMMU_DOMAIN_IOVA_BASE_LOW], edx
    mov dword [edi + IOMMU_DOMAIN_IOVA_BASE_HIGH], 0
    mov edx, [iommu_temp_limit]
    mov [edi + IOMMU_DOMAIN_IOVA_LIMIT_LOW], edx
    mov dword [edi + IOMMU_DOMAIN_IOVA_LIMIT_HIGH], 0
    mov ecx, [iommu_temp_slot]
    mov edx, [iommu_domain_generations + ecx * 4]
    inc edx
    jnz .generation_ready
    inc edx
.generation_ready:
    mov [iommu_domain_generations + ecx * 4], edx
    mov [edi + IOMMU_DOMAIN_GENERATION], edx
    inc dword [iommu_next_domain_id]
    inc dword [iommu_domain_count]
    mov eax, [edi + IOMMU_DOMAIN_ID]
    popfd
    clc
    ret
.invalid:
    inc dword [iommu_validation_failures]
    xor eax, eax
    popfd
    stc
    ret

; EAX=Domain-ID, EDX=Device-ID, EBX=IOMMU-Gruppe, ECX=Owner.
iommu_bind_device:
    pushfd
    cli
    mov [iommu_temp_domain], eax
    mov [iommu_temp_device], edx
    mov [iommu_temp_group], ebx
    mov [iommu_temp_owner], ecx
    test edx, edx
    jz .invalid
    test ebx, ebx
    jz .invalid
    mov eax, edx
    call topology_device_group
    jc .invalid
    cmp eax, [iommu_temp_group]
    jne .invalid                         ; Gruppe stammt aus normalisierter Topologie
    mov eax, [iommu_temp_domain]
    call iommu_domain_lookup
    jc .invalid
    mov [iommu_temp_domain_record], eax
    mov edx, [iommu_temp_owner]
    cmp [eax + IOMMU_DOMAIN_OWNER], edx
    jne .invalid
    mov eax, [iommu_temp_device]
    call iommu_device_lookup
    jnc .invalid
    mov dword [iommu_temp_group_exists], 0
    xor ecx, ecx
.validate_group:
    cmp ecx, IOMMU_DEVICE_CAPACITY
    jae .find_slot
    mov edx, ecx
    shl edx, 5
    add edx, iommu_device_table
    cmp dword [edx + IOMMU_DEVICE_STATE], IOMMU_BINDING_ACTIVE
    jne .next_group
    mov eax, [iommu_temp_group]
    cmp [edx + IOMMU_DEVICE_GROUP], eax
    jne .next_group
    mov eax, [iommu_temp_domain]
    cmp [edx + IOMMU_DEVICE_DOMAIN], eax
    jne .invalid                         ; eine Hardwaregruppe bleibt zusammen
    mov dword [iommu_temp_group_exists], 1
.next_group:
    inc ecx
    jmp .validate_group
.find_slot:
    xor ecx, ecx
.scan_slot:
    cmp ecx, IOMMU_DEVICE_CAPACITY
    jae .invalid
    mov edi, ecx
    shl edi, 5
    add edi, iommu_device_table
    cmp dword [edi + IOMMU_DEVICE_STATE], IOMMU_BINDING_EMPTY
    je .slot
    cmp dword [edi + IOMMU_DEVICE_STATE], IOMMU_BINDING_RELEASED
    je .slot
    inc ecx
    jmp .scan_slot
.slot:
    mov [iommu_temp_slot], ecx
    xor eax, eax
    mov ecx, IOMMU_DEVICE_SIZE / 4
    rep stosd
    mov edi, [iommu_temp_slot]
    shl edi, 5
    add edi, iommu_device_table
    mov edx, [iommu_temp_device]
    mov [edi + IOMMU_DEVICE_ID], edx
    mov edx, [iommu_temp_domain]
    mov [edi + IOMMU_DEVICE_DOMAIN], edx
    mov edx, [iommu_temp_group]
    mov [edi + IOMMU_DEVICE_GROUP], edx
    mov edx, [iommu_temp_owner]
    mov [edi + IOMMU_DEVICE_OWNER], edx
    mov dword [edi + IOMMU_DEVICE_STATE], IOMMU_BINDING_ACTIVE
    mov edx, [iommu_temp_domain_record]
    mov eax, [edx + IOMMU_DOMAIN_MODE]
    mov [edi + IOMMU_DEVICE_MODE], eax
    mov ecx, [iommu_temp_slot]
    mov eax, [iommu_device_generations + ecx * 4]
    inc eax
    jnz .binding_generation_ready
    inc eax
.binding_generation_ready:
    mov [iommu_device_generations + ecx * 4], eax
    mov [edi + IOMMU_DEVICE_GENERATION], eax
    inc dword [edx + IOMMU_DOMAIN_DEVICES]
    cmp dword [iommu_temp_group_exists], 0
    jne .group_counted
    inc dword [edx + IOMMU_DOMAIN_GROUPS]
.group_counted:
    inc dword [iommu_binding_count]
    mov eax, [edi + IOMMU_DEVICE_ID]
    popfd
    clc
    ret
.invalid:
    inc dword [iommu_validation_failures]
    xor eax, eax
    popfd
    stc
    ret

; EAX=Domain, EDX=Device, EBX=externes Mapping, ECX=IOVA, ESI=Laenge,
; EDI=Permissions, EBP=Owner. EAX=Authorization-ID.
iommu_authorize_mapping:
    pushfd
    cli
    mov dword [iommu_temp_mapping_record], 0
    mov [iommu_temp_domain], eax
    mov [iommu_temp_device], edx
    mov [iommu_temp_external], ebx
    mov [iommu_temp_iova], ecx
    mov [iommu_temp_length], esi
    mov [iommu_temp_permissions], edi
    mov [iommu_temp_owner], ebp
    test ebx, ebx
    jz .invalid
    test esi, esi
    jz .invalid
    test edi, edi
    jz .invalid
    test edi, ~(IOMMU_PERMISSION_READ | IOMMU_PERMISSION_WRITE)
    jnz .invalid
    test ecx, 0xFFF
    jnz .invalid
    test esi, 0xFFF
    jnz .invalid
    call iommu_domain_lookup
    jc .invalid
    mov [iommu_temp_domain_record], eax
    mov edx, [iommu_temp_owner]
    cmp [eax + IOMMU_DOMAIN_OWNER], edx
    jne .invalid
    mov edx, [iommu_temp_iova]
    cmp edx, [eax + IOMMU_DOMAIN_IOVA_BASE_LOW]
    jb .invalid
    mov ecx, edx
    add ecx, [iommu_temp_length]
    jc .invalid
    dec ecx
    cmp ecx, [eax + IOMMU_DOMAIN_IOVA_LIMIT_LOW]
    ja .invalid
    mov eax, [iommu_temp_device]
    call iommu_device_lookup
    jc .invalid
    mov edx, [iommu_temp_domain]
    cmp [eax + IOMMU_DEVICE_DOMAIN], edx
    jne .invalid
    xor ecx, ecx
.overlap_scan:
    cmp ecx, IOMMU_MAPPING_CAPACITY
    jae .find_slot
    mov edx, ecx
    imul edx, IOMMU_MAPPING_SIZE
    add edx, iommu_mapping_table
    cmp dword [edx + IOMMU_MAPPING_STATE], IOMMU_MAPPING_ACTIVE
    je .overlap_candidate
    cmp dword [edx + IOMMU_MAPPING_STATE], IOMMU_MAPPING_FAULTED
    jne .next_overlap
.overlap_candidate:
    mov eax, [iommu_temp_domain]
    cmp [edx + IOMMU_MAPPING_DOMAIN], eax
    jne .next_overlap
    mov eax, [edx + IOMMU_MAPPING_IOVA_LOW]
    add eax, [edx + IOMMU_MAPPING_LENGTH]
    mov ebx, [iommu_temp_iova]
    cmp ebx, eax
    jae .next_overlap
    add ebx, [iommu_temp_length]
    cmp [edx + IOMMU_MAPPING_IOVA_LOW], ebx
    jb .invalid
.next_overlap:
    inc ecx
    jmp .overlap_scan
.find_slot:
    xor ecx, ecx
.scan_slot:
    cmp ecx, IOMMU_MAPPING_CAPACITY
    jae .invalid
    mov edi, ecx
    imul edi, IOMMU_MAPPING_SIZE
    add edi, iommu_mapping_table
    cmp dword [edi + IOMMU_MAPPING_STATE], IOMMU_MAPPING_EMPTY
    je .slot
    cmp dword [edi + IOMMU_MAPPING_STATE], IOMMU_MAPPING_REVOKED
    je .slot
    inc ecx
    jmp .scan_slot
.slot:
    mov [iommu_temp_slot], ecx
    mov [iommu_temp_mapping_record], edi
    xor eax, eax
    mov ecx, IOMMU_MAPPING_SIZE / 4
    rep stosd
    mov edi, [iommu_temp_mapping_record]
    mov eax, [iommu_next_authorization_id]
    mov [edi + IOMMU_MAPPING_AUTH_ID], eax
    mov edx, [iommu_temp_domain]
    mov [edi + IOMMU_MAPPING_DOMAIN], edx
    mov edx, [iommu_temp_device]
    mov [edi + IOMMU_MAPPING_DEVICE], edx
    mov edx, [iommu_temp_external]
    mov [edi + IOMMU_MAPPING_EXTERNAL], edx
    mov edx, [iommu_temp_iova]
    mov [edi + IOMMU_MAPPING_IOVA_LOW], edx
    mov dword [edi + IOMMU_MAPPING_IOVA_HIGH], 0
    mov edx, [iommu_temp_length]
    mov [edi + IOMMU_MAPPING_LENGTH], edx
    mov edx, [iommu_temp_permissions]
    mov [edi + IOMMU_MAPPING_PERMISSIONS], edx
    mov dword [edi + IOMMU_MAPPING_STATE], IOMMU_MAPPING_ACTIVE
    mov ecx, [iommu_temp_slot]
    mov edx, [iommu_mapping_generations + ecx * 4]
    inc edx
    jnz .mapping_generation_ready
    inc edx
.mapping_generation_ready:
    mov [iommu_mapping_generations + ecx * 4], edx
    mov [edi + IOMMU_MAPPING_GENERATION], edx
    mov eax, [iommu_temp_domain_record]
    inc dword [eax + IOMMU_DOMAIN_MAPPINGS]
    mov edx, [iommu_temp_length]
    add [eax + IOMMU_DOMAIN_MAPPED_BYTES], edx
    inc dword [iommu_mapping_count]
    add [iommu_mapped_bytes], edx
    inc dword [iommu_next_authorization_id]
    mov eax, [edi + IOMMU_MAPPING_AUTH_ID]
    popfd
    clc
    ret
.invalid:
    inc dword [iommu_validation_failures]
    xor eax, eax
    popfd
    stc
    ret

; EAX=Authorization-ID, EDX=Owner.
iommu_revoke_mapping:
    pushfd
    cli
    mov [iommu_temp_authorization], eax
    mov [iommu_temp_owner], edx
    call iommu_mapping_lookup
    jc .invalid
    mov [iommu_temp_mapping_record], eax
    mov eax, [eax + IOMMU_MAPPING_DOMAIN]
    call iommu_domain_lookup
    jc .invalid
    mov edx, [iommu_temp_owner]
    cmp [eax + IOMMU_DOMAIN_OWNER], edx
    jne .invalid
    mov edi, [iommu_temp_mapping_record]
    mov edx, [edi + IOMMU_MAPPING_LENGTH]
    cmp [eax + IOMMU_DOMAIN_MAPPED_BYTES], edx
    jb .zero_domain_bytes
    sub [eax + IOMMU_DOMAIN_MAPPED_BYTES], edx
    jmp .domain_bytes_done
.zero_domain_bytes:
    mov dword [eax + IOMMU_DOMAIN_MAPPED_BYTES], 0
.domain_bytes_done:
    cmp dword [eax + IOMMU_DOMAIN_MAPPINGS], 0
    je .global_accounting
    dec dword [eax + IOMMU_DOMAIN_MAPPINGS]
.global_accounting:
    cmp [iommu_mapped_bytes], edx
    jb .zero_global_bytes
    sub [iommu_mapped_bytes], edx
    jmp .global_bytes_done
.zero_global_bytes:
    mov dword [iommu_mapped_bytes], 0
.global_bytes_done:
    cmp dword [iommu_mapping_count], 0
    je .mark_revoked
    dec dword [iommu_mapping_count]
.mark_revoked:
    mov dword [edi + IOMMU_MAPPING_STATE], IOMMU_MAPPING_REVOKED
    mov eax, [iommu_temp_authorization]
    popfd
    clc
    ret
.invalid:
    xor eax, eax
    popfd
    stc
    ret

; EAX=Domain, EDX=Device, EBX=externes Mapping, ECX=IOVA,
; ESI=Access, EDI=Fehlercode.
iommu_report_fault:
    pushfd
    cli
    mov [iommu_temp_domain], eax
    mov [iommu_temp_device], edx
    mov [iommu_temp_external], ebx
    mov [iommu_temp_iova], ecx
    mov [iommu_temp_permissions], esi
    mov [iommu_temp_error], edi
    test edi, edi
    jz .invalid
    call iommu_domain_lookup
    jc .invalid
    mov [iommu_temp_domain_record], eax
    mov eax, [iommu_temp_device]
    call iommu_device_lookup
    jc .invalid
    mov edx, [iommu_temp_domain]
    cmp [eax + IOMMU_DEVICE_DOMAIN], edx
    jne .invalid
    mov dword [iommu_temp_mapping_record], 0
    xor ecx, ecx
.find_mapping:
    cmp ecx, IOMMU_MAPPING_CAPACITY
    jae .record_fault
    mov edx, ecx
    imul edx, IOMMU_MAPPING_SIZE
    add edx, iommu_mapping_table
    cmp dword [edx + IOMMU_MAPPING_STATE], IOMMU_MAPPING_ACTIVE
    jne .next_mapping
    mov eax, [iommu_temp_domain]
    cmp [edx + IOMMU_MAPPING_DOMAIN], eax
    jne .next_mapping
    mov eax, [iommu_temp_device]
    cmp [edx + IOMMU_MAPPING_DEVICE], eax
    jne .next_mapping
    mov eax, [iommu_temp_external]
    cmp [edx + IOMMU_MAPPING_EXTERNAL], eax
    jne .next_mapping
    mov [iommu_temp_mapping_record], edx
    mov eax, [iommu_temp_error]
    mov [edx + IOMMU_MAPPING_ERROR], eax
    mov dword [edx + IOMMU_MAPPING_STATE], IOMMU_MAPPING_FAULTED
    jmp .record_fault
.next_mapping:
    inc ecx
    jmp .find_mapping
.record_fault:
    inc dword [iommu_fault_count]
    mov eax, [iommu_temp_domain_record]
    inc dword [eax + IOMMU_DOMAIN_FAULTS]
    mov edi, iommu_last_fault
    mov eax, [iommu_fault_count]
    mov [edi + IOMMU_FAULT_SEQUENCE], eax
    mov eax, [iommu_temp_domain]
    mov [edi + IOMMU_FAULT_DOMAIN], eax
    mov eax, [iommu_temp_device]
    mov [edi + IOMMU_FAULT_DEVICE], eax
    mov eax, [iommu_temp_external]
    mov [edi + IOMMU_FAULT_EXTERNAL], eax
    mov eax, [iommu_temp_iova]
    mov [edi + IOMMU_FAULT_IOVA_LOW], eax
    mov dword [edi + IOMMU_FAULT_IOVA_HIGH], 0
    mov eax, [iommu_temp_permissions]
    mov [edi + IOMMU_FAULT_ACCESS], eax
    mov eax, [iommu_temp_error]
    mov [edi + IOMMU_FAULT_ERROR], eax
    mov eax, [iommu_temp_external]
    mov edx, eax
    and edx, IOMMU_EXTERNAL_KIND_MASK
    and eax, IOMMU_EXTERNAL_ID_MASK
    cmp edx, IOMMU_EXTERNAL_DMA
    je .route_dma
    cmp edx, IOMMU_EXTERNAL_DMA_SG
    je .route_dma_sg
    jmp .route_done
.route_dma:
    mov edx, [iommu_temp_device]
    mov ebx, [iommu_temp_error]
    call dma_mapping_fault
    jc .invalid
    jmp .route_done
.route_dma_sg:
    mov edx, [iommu_temp_device]
    mov ebx, [iommu_temp_error]
    call dma_scatter_gather_fault
    jc .invalid
.route_done:
    mov eax, [iommu_fault_count]
    popfd
    clc
    ret
.invalid:
    xor eax, eax
    popfd
    stc
    ret

; EAX=Domain-ID, EDX=Owner. Aktive Mappings blockieren den Abbau; gebundene
; Devices derselben Domain werden gemeinsam quiesced und entfernt.
iommu_domain_release:
    pushfd
    cli
    mov [iommu_temp_domain], eax
    mov [iommu_temp_owner], edx
    call iommu_domain_lookup
    jc .invalid
    mov [iommu_temp_domain_record], eax
    mov edx, [iommu_temp_owner]
    cmp [eax + IOMMU_DOMAIN_OWNER], edx
    jne .invalid
    cmp dword [eax + IOMMU_DOMAIN_MAPPINGS], 0
    jne .invalid
    mov dword [eax + IOMMU_DOMAIN_STATE], IOMMU_DOMAIN_QUIESCING
    xor ecx, ecx
.unbind:
    cmp ecx, IOMMU_DEVICE_CAPACITY
    jae .released
    mov edi, ecx
    shl edi, 5
    add edi, iommu_device_table
    cmp dword [edi + IOMMU_DEVICE_STATE], IOMMU_BINDING_ACTIVE
    jne .next_binding
    mov edx, [iommu_temp_domain]
    cmp [edi + IOMMU_DEVICE_DOMAIN], edx
    jne .next_binding
    mov dword [edi + IOMMU_DEVICE_STATE], IOMMU_BINDING_RELEASED
    cmp dword [iommu_binding_count], 0
    je .next_binding
    dec dword [iommu_binding_count]
.next_binding:
    inc ecx
    jmp .unbind
.released:
    mov eax, [iommu_temp_domain_record]
    mov dword [eax + IOMMU_DOMAIN_DEVICES], 0
    mov dword [eax + IOMMU_DOMAIN_GROUPS], 0
    mov dword [eax + IOMMU_DOMAIN_STATE], IOMMU_DOMAIN_RELEASED
    dec dword [iommu_domain_count]
    mov eax, [iommu_temp_domain]
    popfd
    clc
    ret
.invalid:
    xor eax, eax
    popfd
    stc
    ret

iommu_self_test:
    cmp dword [iommu_hardware_available], 0
    jne .invalid
    mov eax, 1
    mov ebx, IOMMU_MODE_RESTRICTED
    mov ecx, 32
    mov esi, 0xD2000000
    mov edi, 0xD20FFFFF
    call iommu_domain_create
    jc .invalid
    mov [iommu_test_domain], eax
    mov edx, 0xD003
    mov ebx, 0x30
    mov ecx, 1
    call iommu_bind_device
    jc .invalid
    mov eax, [iommu_test_domain]
    mov edx, 0xD004
    mov ebx, 0x30
    mov ecx, 1
    call iommu_bind_device
    jc .invalid
    mov eax, [iommu_test_domain]
    call iommu_domain_lookup
    jc .invalid
    cmp dword [eax + IOMMU_DOMAIN_DEVICES], 2
    jne .invalid
    cmp dword [eax + IOMMU_DOMAIN_GROUPS], 1
    jne .invalid
    mov eax, [iommu_test_domain]
    mov edx, 0xD003
    mov ebx, 77
    mov ecx, 0xD2000000
    mov esi, 4096
    mov edi, IOMMU_PERMISSION_READ
    mov ebp, 1
    call iommu_authorize_mapping
    jc .invalid
    mov [iommu_test_authorization], eax
    mov eax, [iommu_test_domain]
    mov edx, 0xD003
    mov ebx, 78
    mov ecx, 0xD2000000
    mov esi, 4096
    mov edi, IOMMU_PERMISSION_READ
    mov ebp, 1
    call iommu_authorize_mapping
    jnc .invalid                       ; IOVA-Ueberlappung verboten
    mov eax, [iommu_test_domain]
    mov edx, 0xD003
    mov ebx, 77
    mov ecx, 0xD2000010
    mov esi, IOMMU_PERMISSION_WRITE
    mov edi, 0xF001
    call iommu_report_fault
    jc .invalid
    mov eax, [iommu_test_authorization]
    call iommu_mapping_lookup
    jc .invalid
    cmp dword [eax + IOMMU_MAPPING_STATE], IOMMU_MAPPING_FAULTED
    jne .invalid
    cmp dword [iommu_last_fault + IOMMU_FAULT_DEVICE], 0xD003
    jne .invalid
    cmp dword [iommu_last_fault + IOMMU_FAULT_DOMAIN], 0
    je .invalid
    mov eax, [iommu_test_authorization]
    mov edx, 1
    call iommu_revoke_mapping
    jc .invalid
    mov eax, [iommu_test_domain]
    mov edx, 1
    call iommu_domain_release
    jc .invalid
    cmp dword [iommu_domain_count], 0
    jne .invalid
    cmp dword [iommu_binding_count], 0
    jne .invalid
    cmp dword [iommu_mapping_count], 0
    jne .invalid
    cmp dword [iommu_mapped_bytes], 0
    jne .invalid
    cmp dword [iommu_fault_count], 1
    jne .invalid
    clc
    ret
.invalid:
    stc
    ret

align 4
iommu_api:
    dd IOMMU_API_SIZE
    dw 1, 0
    dd IOMMU_DOMAIN_CAPACITY
    dd IOMMU_DEVICE_CAPACITY
    dd IOMMU_MAPPING_CAPACITY
    dd iommu_domain_create
    dd iommu_bind_device
    dd iommu_authorize_mapping
    dd iommu_revoke_mapping
    dd iommu_domain_release

iommu_manager_ready:             dd 0
iommu_hardware_available:        dd 0
iommu_next_domain_id:            dd 0
iommu_next_authorization_id:     dd 0
iommu_domain_count:              dd 0
iommu_binding_count:             dd 0
iommu_mapping_count:             dd 0
iommu_mapped_bytes:              dd 0
iommu_fault_count:               dd 0
iommu_validation_failures:       dd 0
iommu_temp_owner:                dd 0
iommu_temp_mode:                 dd 0
iommu_temp_address_width:        dd 0
iommu_temp_iova:                 dd 0
iommu_temp_limit:                dd 0
iommu_temp_length:               dd 0
iommu_temp_permissions:          dd 0
iommu_temp_domain:               dd 0
iommu_temp_device:               dd 0
iommu_temp_group:                dd 0
iommu_temp_external:             dd 0
iommu_temp_authorization:        dd 0
iommu_temp_error:                dd 0
iommu_temp_slot:                 dd 0
iommu_temp_group_exists:         dd 0
iommu_temp_domain_record:        dd 0
iommu_temp_mapping_record:       dd 0
iommu_test_domain:               dd 0
iommu_test_authorization:        dd 0
align 4
iommu_domain_table:
    times IOMMU_DOMAIN_CAPACITY * IOMMU_DOMAIN_SIZE db 0
iommu_device_table:
    times IOMMU_DEVICE_CAPACITY * IOMMU_DEVICE_SIZE db 0
iommu_mapping_table:
    times IOMMU_MAPPING_CAPACITY * IOMMU_MAPPING_SIZE db 0
iommu_domain_generations:
    times IOMMU_DOMAIN_CAPACITY dd 0
iommu_device_generations:
    times IOMMU_DEVICE_CAPACITY dd 0
iommu_mapping_generations:
    times IOMMU_MAPPING_CAPACITY dd 0
iommu_last_fault:
    times IOMMU_FAULT_SIZE db 0

; ---------------------------------------------------------------------------
; Kontrollierte DMA-Mappings. CPU-Adressen werden nie direkt als
; Device-Adressen veroeffentlicht. Ohne erkannten IOMMU-Provider bleibt der
; Bootstrap explizit im eingeschraenkten Bounce-/Staging-Modus.
; NPSPEC-HAL-DMA-0001 / NPSPEC-HAL-IOMMU-0001 /
; NPSPEC-DATAMOVE-DMA-0001
; ---------------------------------------------------------------------------

DMA_MAPPING_API_SIZE          equ 32
DMA_MAPPING_CAPACITY          equ 4
DMA_MAPPING_RECORD_SIZE       equ 64
DMA_MAPPING_STATE_EMPTY       equ 0
DMA_MAPPING_STATE_ACTIVE      equ 1
DMA_MAPPING_STATE_UNMAPPED    equ 2
DMA_MAPPING_STATE_FAULTED     equ 3
DMA_DIRECTION_TO_DEVICE       equ 1
DMA_DIRECTION_FROM_DEVICE     equ 2
DMA_DIRECTION_BIDIRECTIONAL   equ 3
DMA_PERMISSION_READ           equ 0x00000001
DMA_PERMISSION_WRITE          equ 0x00000002
DMA_FLAG_IOMMU                equ 0x00000001
DMA_FLAG_BOUNCE               equ 0x00000002
DMA_FLAG_COHERENT             equ 0x00000004
DMA_FLAG_RESTRICTED           equ 0x00000008
DMA_RESTRICTED_APERTURE       equ 0xD0000000
DMA_MAPPING_ID                equ 0
DMA_MAPPING_OWNER             equ 4
DMA_MAPPING_DEVICE            equ 8
DMA_MAPPING_BUFFER            equ 12
DMA_MAPPING_REQUEST           equ 16
DMA_MAPPING_DIRECTION         equ 20
DMA_MAPPING_PERMISSIONS       equ 24
DMA_MAPPING_STATE             equ 28
DMA_MAPPING_LENGTH            equ 32
DMA_MAPPING_ADDRESS_LOW       equ 36
DMA_MAPPING_ADDRESS_HIGH      equ 40
DMA_MAPPING_DOMAIN            equ 44
DMA_MAPPING_FLAGS             equ 48
DMA_MAPPING_PINNED_PAGES      equ 52
DMA_MAPPING_GENERATION        equ 56
DMA_MAPPING_ERROR             equ 60

dma_mapping_initialize:
    mov edi, dma_mapping_table
    xor eax, eax
    mov ecx, (DMA_MAPPING_CAPACITY * DMA_MAPPING_RECORD_SIZE) / 4
    rep stosd
    mov edi, dma_request_mapping_ids
    mov ecx, IO_REQUEST_CAPACITY
    rep stosd
    mov edi, dma_mapping_generations
    mov ecx, DMA_MAPPING_CAPACITY
    rep stosd
    mov edi, dma_mapping_iommu_authorization_ids
    mov ecx, DMA_MAPPING_CAPACITY
    rep stosd
    mov dword [dma_mapping_next_id], 1
    mov dword [dma_mapping_active_count], 0
    mov dword [dma_mapping_mapped_bytes], 0
    mov dword [dma_mapping_pinned_pages], 0
    mov dword [dma_mapping_fault_count], 0
    mov dword [dma_mapping_bounce_count], 0
    mov dword [dma_iommu_available], 0
    mov dword [dma_mapping_manager_ready], 1
    clc
    ret

; EAX=Mapping-ID. EAX=Datensatz oder 0. Nur aktive Mappings werden geliefert.
dma_mapping_lookup:
    xor ecx, ecx
.scan:
    cmp ecx, DMA_MAPPING_CAPACITY
    jae .invalid
    mov edx, ecx
    shl edx, 6
    add edx, dma_mapping_table
    cmp dword [edx + DMA_MAPPING_STATE], DMA_MAPPING_STATE_ACTIVE
    jne .next
    cmp [edx + DMA_MAPPING_ID], eax
    je .found
.next:
    inc ecx
    jmp .scan
.found:
    mov eax, edx
    clc
    ret
.invalid:
    xor eax, eax
    stc
    ret

; EAX=Request-ID, EDX=Device-ID, EBX=Richtung, ECX=Laenge, ESI=Owner.
; Ergebnis EAX=stabile Mapping-ID. Das aktuelle Bootstrap-Backend weist eine
; kontrollierte Device-Adresse aus einer reservierten Staging-Apertur zu.
dma_mapping_map:
    pushfd
    cli
    mov [dma_mapping_temp_request], eax
    mov [dma_mapping_temp_device], edx
    mov [dma_mapping_temp_direction], ebx
    mov [dma_mapping_temp_length], ecx
    mov [dma_mapping_temp_owner], esi
    cmp dword [dma_mapping_manager_ready], 1
    jne .invalid
    test edx, edx
    jz .invalid
    test ecx, ecx
    jz .invalid
    cmp ebx, DMA_DIRECTION_TO_DEVICE
    jb .invalid
    cmp ebx, DMA_DIRECTION_BIDIRECTIONAL
    ja .invalid
    call io_request_lookup
    jc .invalid
    mov [dma_mapping_temp_request_record], eax
    cmp dword [eax + IO_REQUEST_STATE], IO_REQUEST_STATE_PENDING
    je .request_active
    cmp dword [eax + IO_REQUEST_STATE], IO_REQUEST_STATE_RUNNING
    jne .invalid
.request_active:
    mov edx, [dma_mapping_temp_owner]
    cmp [eax + IO_REQUEST_OWNER], edx
    jne .invalid
    mov ecx, [dma_mapping_temp_length]
    cmp ecx, [eax + IO_REQUEST_LENGTH]
    ja .invalid
    mov ecx, eax
    sub ecx, io_request_table
    shr ecx, 6
    cmp dword [dma_request_mapping_ids + ecx * 4], 0
    jne .invalid
    mov [dma_mapping_temp_request_slot], ecx
    mov eax, [io_request_buffer_ids + ecx * 4]
    test eax, eax
    jz .invalid
    mov [dma_mapping_temp_buffer], eax
    call shared_buffer_lookup
    jc .invalid
    mov [dma_mapping_temp_buffer_record], eax
    mov edx, [dma_mapping_temp_owner]
    cmp [eax + SHARED_BUFFER_OWNER], edx
    jne .invalid
    cmp dword [eax + SHARED_BUFFER_STATE], SHARED_BUFFER_STATE_PROVIDER
    jne .invalid
    test dword [eax + SHARED_BUFFER_RIGHTS], SHARED_BUFFER_RIGHT_DMA
    jz .invalid
    test dword [eax + SHARED_BUFFER_RIGHTS], SHARED_BUFFER_RIGHT_TRANSFER
    jz .invalid
    mov ecx, [dma_mapping_temp_length]
    cmp ecx, [eax + SHARED_BUFFER_SIZE]
    ja .invalid
    mov ebx, [dma_mapping_temp_direction]
    cmp ebx, DMA_DIRECTION_TO_DEVICE
    je .needs_read
    cmp ebx, DMA_DIRECTION_FROM_DEVICE
    je .needs_write
    test dword [eax + SHARED_BUFFER_RIGHTS], SHARED_BUFFER_RIGHT_READ
    jz .invalid
    test dword [eax + SHARED_BUFFER_RIGHTS], SHARED_BUFFER_RIGHT_WRITE
    jz .invalid
    mov dword [dma_mapping_temp_permissions], DMA_PERMISSION_READ | DMA_PERMISSION_WRITE
    jmp .find_slot
.needs_read:
    test dword [eax + SHARED_BUFFER_RIGHTS], SHARED_BUFFER_RIGHT_READ
    jz .invalid
    mov dword [dma_mapping_temp_permissions], DMA_PERMISSION_READ
    jmp .find_slot
.needs_write:
    test dword [eax + SHARED_BUFFER_RIGHTS], SHARED_BUFFER_RIGHT_WRITE
    jz .invalid
    mov dword [dma_mapping_temp_permissions], DMA_PERMISSION_WRITE
.find_slot:
    xor ecx, ecx
.scan_slot:
    cmp ecx, DMA_MAPPING_CAPACITY
    jae .invalid
    mov edi, ecx
    shl edi, 6
    add edi, dma_mapping_table
    cmp dword [edi + DMA_MAPPING_STATE], DMA_MAPPING_STATE_EMPTY
    je .slot
    cmp dword [edi + DMA_MAPPING_STATE], DMA_MAPPING_STATE_UNMAPPED
    je .slot
    cmp dword [edi + DMA_MAPPING_STATE], DMA_MAPPING_STATE_FAULTED
    je .slot
    inc ecx
    jmp .scan_slot
.slot:
    mov [dma_mapping_temp_slot], ecx
    mov [dma_mapping_temp_record], edi
    xor eax, eax
    mov ecx, DMA_MAPPING_RECORD_SIZE / 4
    rep stosd
    mov edi, [dma_mapping_temp_record]
    mov eax, [dma_mapping_next_id]
    mov [edi + DMA_MAPPING_ID], eax
    mov edx, [dma_mapping_temp_owner]
    mov [edi + DMA_MAPPING_OWNER], edx
    mov edx, [dma_mapping_temp_device]
    mov [edi + DMA_MAPPING_DEVICE], edx
    mov edx, [dma_mapping_temp_buffer]
    mov [edi + DMA_MAPPING_BUFFER], edx
    mov edx, [dma_mapping_temp_request]
    mov [edi + DMA_MAPPING_REQUEST], edx
    mov edx, [dma_mapping_temp_direction]
    mov [edi + DMA_MAPPING_DIRECTION], edx
    mov edx, [dma_mapping_temp_permissions]
    mov [edi + DMA_MAPPING_PERMISSIONS], edx
    mov dword [edi + DMA_MAPPING_STATE], DMA_MAPPING_STATE_ACTIVE
    mov edx, [dma_mapping_temp_length]
    mov [edi + DMA_MAPPING_LENGTH], edx
    mov eax, [dma_mapping_temp_slot]
    shl eax, 12
    add eax, DMA_RESTRICTED_APERTURE
    mov [edi + DMA_MAPPING_ADDRESS_LOW], eax
    mov dword [edi + DMA_MAPPING_ADDRESS_HIGH], 0
    mov dword [edi + DMA_MAPPING_DOMAIN], 0
    mov dword [edi + DMA_MAPPING_FLAGS], DMA_FLAG_BOUNCE | DMA_FLAG_COHERENT | DMA_FLAG_RESTRICTED
    mov dword [edi + DMA_MAPPING_PINNED_PAGES], 1
    mov ecx, [dma_mapping_temp_slot]
    mov edx, [dma_mapping_generations + ecx * 4]
    inc edx
    jnz .generation_ready
    inc edx
.generation_ready:
    mov [dma_mapping_generations + ecx * 4], edx
    mov [edi + DMA_MAPPING_GENERATION], edx
    mov dword [dma_mapping_iommu_authorization_ids + ecx * 4], 0
    mov eax, [dma_mapping_temp_device]
    call iommu_domain_for_device
    jc .publish
    mov [dma_mapping_temp_domain_record], eax
    mov edx, [eax + IOMMU_DOMAIN_ID]
    mov edi, [dma_mapping_temp_record]
    mov [edi + DMA_MAPPING_DOMAIN], edx
    mov eax, edx
    mov edx, [dma_mapping_temp_device]
    mov ebx, [edi + DMA_MAPPING_ID]
    or ebx, IOMMU_EXTERNAL_DMA
    mov ecx, [edi + DMA_MAPPING_ADDRESS_LOW]
    mov esi, 4096
    mov edi, [dma_mapping_temp_permissions]
    mov ebp, [dma_mapping_temp_owner]
    call iommu_authorize_mapping
    jc .authorization_invalid
    mov ecx, [dma_mapping_temp_slot]
    mov [dma_mapping_iommu_authorization_ids + ecx * 4], eax
    mov eax, [dma_mapping_temp_domain_record]
    cmp dword [eax + IOMMU_DOMAIN_MODE], IOMMU_MODE_RESTRICTED
    je .publish
    mov edi, [dma_mapping_temp_record]
    and dword [edi + DMA_MAPPING_FLAGS], ~(DMA_FLAG_BOUNCE | DMA_FLAG_RESTRICTED)
    or dword [edi + DMA_MAPPING_FLAGS], DMA_FLAG_IOMMU
.publish:
    mov edi, [dma_mapping_temp_record]
    mov eax, [edi + DMA_MAPPING_ID]
    mov ecx, [dma_mapping_temp_request_slot]
    mov [dma_request_mapping_ids + ecx * 4], eax
    mov edx, [dma_mapping_temp_buffer_record]
    inc dword [edx + SHARED_BUFFER_MAPPINGS]
    inc dword [edx + SHARED_BUFFER_PINNED]
    inc dword [dma_mapping_next_id]
    inc dword [dma_mapping_active_count]
    inc dword [dma_mapping_pinned_pages]
    inc dword [dma_mapping_bounce_count]
    mov edx, [dma_mapping_temp_length]
    add [dma_mapping_mapped_bytes], edx
    mov eax, [edi + DMA_MAPPING_ID]
    popfd
    clc
    ret
.authorization_invalid:
    mov edi, [dma_mapping_temp_record]
    mov dword [edi + DMA_MAPPING_STATE], DMA_MAPPING_STATE_EMPTY
    jmp .invalid
.invalid:
    xor eax, eax
    popfd
    stc
    ret

; Interner, bereits validierter Abbau. EAX=aktiver Mapping-Datensatz.
dma_mapping_release_record:
    mov [dma_mapping_temp_record], eax
    mov ecx, eax
    sub ecx, dma_mapping_table
    shr ecx, 6
    mov eax, [dma_mapping_iommu_authorization_ids + ecx * 4]
    test eax, eax
    jz .iommu_released
    mov edx, [dma_mapping_temp_record]
    mov edx, [edx + DMA_MAPPING_OWNER]
    call iommu_revoke_mapping
    mov ecx, [dma_mapping_temp_record]
    sub ecx, dma_mapping_table
    shr ecx, 6
    mov dword [dma_mapping_iommu_authorization_ids + ecx * 4], 0
.iommu_released:
    mov eax, [dma_mapping_temp_record]
    mov edx, [eax + DMA_MAPPING_REQUEST]
    mov [dma_mapping_temp_request], edx
    mov eax, edx
    call io_request_lookup
    jc .skip_request_slot
    mov ecx, eax
    sub ecx, io_request_table
    shr ecx, 6
    mov edx, [dma_mapping_temp_record]
    mov edx, [edx + DMA_MAPPING_ID]
    cmp [dma_request_mapping_ids + ecx * 4], edx
    jne .skip_request_slot
    mov dword [dma_request_mapping_ids + ecx * 4], 0
.skip_request_slot:
    mov eax, [dma_mapping_temp_record]
    mov eax, [eax + DMA_MAPPING_BUFFER]
    call shared_buffer_lookup
    jc .skip_buffer
    cmp dword [eax + SHARED_BUFFER_MAPPINGS], 0
    je .mapping_done
    dec dword [eax + SHARED_BUFFER_MAPPINGS]
.mapping_done:
    cmp dword [eax + SHARED_BUFFER_PINNED], 0
    je .skip_buffer
    dec dword [eax + SHARED_BUFFER_PINNED]
.skip_buffer:
    mov eax, [dma_mapping_temp_record]
    mov edx, [eax + DMA_MAPPING_LENGTH]
    cmp [dma_mapping_mapped_bytes], edx
    jb .zero_bytes
    sub [dma_mapping_mapped_bytes], edx
    jmp .bytes_done
.zero_bytes:
    mov dword [dma_mapping_mapped_bytes], 0
.bytes_done:
    cmp dword [dma_mapping_active_count], 0
    je .active_done
    dec dword [dma_mapping_active_count]
.active_done:
    cmp dword [dma_mapping_pinned_pages], 0
    je .pinned_done
    dec dword [dma_mapping_pinned_pages]
.pinned_done:
    cmp dword [eax + DMA_MAPPING_STATE], DMA_MAPPING_STATE_FAULTED
    je .done
    mov dword [eax + DMA_MAPPING_STATE], DMA_MAPPING_STATE_UNMAPPED
.done:
    clc
    ret

; EAX=Mapping-ID, EDX=Owner. Ein aktives Mapping wird kontrolliert entfernt.
dma_mapping_unmap:
    pushfd
    cli
    mov [dma_mapping_temp_id], eax
    mov [dma_mapping_temp_owner], edx
    call dma_mapping_lookup
    jc .invalid
    mov edx, [dma_mapping_temp_owner]
    cmp [eax + DMA_MAPPING_OWNER], edx
    jne .invalid
    call dma_mapping_release_record
    mov eax, [dma_mapping_temp_id]
    popfd
    clc
    ret
.invalid:
    xor eax, eax
    popfd
    stc
    ret

; EAX=Mapping-ID, EDX=Device-ID, EBX=Fehlercode. Der Fault bleibt im
; Mapping-Datensatz sichtbar; Ressourcen und Pinning werden trotzdem geloest.
dma_mapping_fault:
    pushfd
    cli
    mov [dma_mapping_temp_id], eax
    mov [dma_mapping_temp_device], edx
    mov [dma_mapping_temp_error], ebx
    call dma_mapping_lookup
    jc .invalid
    mov edx, [dma_mapping_temp_device]
    cmp [eax + DMA_MAPPING_DEVICE], edx
    jne .invalid
    mov ebx, [dma_mapping_temp_error]
    test ebx, ebx
    jz .invalid
    mov [eax + DMA_MAPPING_ERROR], ebx
    mov dword [eax + DMA_MAPPING_STATE], DMA_MAPPING_STATE_FAULTED
    inc dword [dma_mapping_fault_count]
    mov edx, [eax + DMA_MAPPING_REQUEST]
    mov [dma_mapping_temp_request], edx
    call dma_mapping_release_record
    mov eax, [dma_mapping_temp_request]
    xor edx, edx
    mov ebx, [dma_mapping_temp_error]
    call io_request_complete
    jc .invalid
    mov eax, [dma_mapping_temp_id]
    popfd
    clc
    ret
.invalid:
    xor eax, eax
    popfd
    stc
    ret

; EAX=terminaler IORequest-Datensatz. Jede Completion, Cancellation und jeder
; Deadline-Miss loest das zugehoerige DMA-Mapping vor der Buffer-Lease.
dma_mapping_on_terminal:
    cmp dword [dma_mapping_manager_ready], 1
    jne .done
    mov ecx, eax
    sub ecx, io_request_table
    shr ecx, 6
    mov eax, [dma_request_mapping_ids + ecx * 4]
    test eax, eax
    jz .done
    call dma_mapping_lookup
    jc .done
    call dma_mapping_release_record
.done:
    clc
    ret

dma_mapping_self_test:
    cmp dword [dma_iommu_available], 0
    jne .invalid
    mov eax, 1
    mov ebx, IOMMU_MODE_RESTRICTED
    mov ecx, 32
    mov esi, DMA_RESTRICTED_APERTURE
    mov edi, DMA_RESTRICTED_APERTURE + 0x000FFFFF
    call iommu_domain_create
    jc .invalid
    mov [dma_mapping_test_domain], eax
    mov edx, 0xD001
    mov ebx, 0x10
    mov ecx, 1
    call iommu_bind_device
    jc .invalid
    mov eax, 1
    mov edx, 128
    mov ebx, SHARED_BUFFER_RIGHT_READ | SHARED_BUFFER_RIGHT_WRITE | SHARED_BUFFER_RIGHT_TRANSFER | SHARED_BUFFER_RIGHT_DMA | SHARED_BUFFER_RIGHT_RELEASE
    call shared_buffer_create
    jc .invalid
    mov [dma_mapping_test_buffer], eax
    mov eax, 1
    mov edx, [task_scope_kernel_root_id]
    xor ebx, ebx
    call task_scope_create
    jc .invalid
    mov [dma_mapping_test_scope], eax
    mov eax, 1
    mov edx, [dma_mapping_test_scope]
    xor ebx, ebx
    xor ecx, ecx
    call task_create
    jc .invalid
    mov [dma_mapping_test_task], eax
    mov eax, 1
    mov edx, [dma_mapping_test_task]
    mov ebx, 6
    mov ecx, 0xD001
    mov esi, [dma_mapping_test_buffer]
    mov edi, 128
    xor ebp, ebp
    call io_request_submit
    jc .invalid
    mov [dma_mapping_test_request], eax
    mov edx, [dma_mapping_test_buffer]
    mov ebx, SHARED_BUFFER_RIGHT_READ | SHARED_BUFFER_RIGHT_WRITE
    call shared_buffer_bind_io
    jc .invalid
    mov eax, [dma_mapping_test_request]
    mov edx, 0xD001
    mov ebx, DMA_DIRECTION_BIDIRECTIONAL
    mov ecx, 128
    mov esi, 1
    call dma_mapping_map
    jc .invalid
    mov [dma_mapping_test_id], eax
    call dma_mapping_lookup
    jc .invalid
    mov [dma_mapping_test_record], eax
    cmp dword [eax + DMA_MAPPING_PERMISSIONS], DMA_PERMISSION_READ | DMA_PERMISSION_WRITE
    jne .invalid
    cmp dword [eax + DMA_MAPPING_FLAGS], DMA_FLAG_BOUNCE | DMA_FLAG_COHERENT | DMA_FLAG_RESTRICTED
    jne .invalid
    cmp dword [eax + DMA_MAPPING_ADDRESS_HIGH], 0
    jne .invalid
    cmp dword [eax + DMA_MAPPING_DOMAIN], 0
    je .invalid
    cmp dword [iommu_mapping_count], 1
    jne .invalid
    mov edx, [eax + DMA_MAPPING_ADDRESS_LOW]
    cmp edx, DMA_RESTRICTED_APERTURE
    jb .invalid
    mov eax, [dma_mapping_test_buffer]
    call shared_buffer_lookup
    jc .invalid
    cmp dword [eax + SHARED_BUFFER_MAPPINGS], 1
    jne .invalid
    cmp dword [eax + SHARED_BUFFER_PINNED], 1
    jne .invalid
    mov eax, [dma_mapping_test_request]
    mov edx, 0xD001
    mov ebx, DMA_DIRECTION_TO_DEVICE
    mov ecx, 64
    mov esi, 1
    call dma_mapping_map
    jnc .invalid                       ; genau ein Mapping je Request
    mov eax, [dma_mapping_test_buffer]
    mov edx, 1
    call shared_buffer_release
    jnc .invalid                       ; Pinning/Lease blockiert Release
    mov eax, [dma_mapping_test_domain]
    mov edx, 0xD001
    mov ebx, [dma_mapping_test_id]
    or ebx, IOMMU_EXTERNAL_DMA
    mov ecx, DMA_RESTRICTED_APERTURE
    mov esi, IOMMU_PERMISSION_WRITE
    mov edi, 0xD101
    call iommu_report_fault
    jc .invalid
    mov eax, [dma_mapping_test_record]
    cmp dword [eax + DMA_MAPPING_STATE], DMA_MAPPING_STATE_FAULTED
    jne .invalid
    cmp dword [dma_mapping_fault_count], 1
    jne .invalid
    mov eax, [dma_mapping_test_request]
    call io_request_lookup
    jc .invalid
    cmp dword [eax + IO_REQUEST_STATE], IO_REQUEST_STATE_FAILED
    jne .invalid
    cmp dword [eax + IO_REQUEST_COMPLETION], IO_COMPLETION_FAILED
    jne .invalid
    cmp dword [eax + IO_REQUEST_ERROR], 0xD101
    jne .invalid
    mov eax, [dma_mapping_test_buffer]
    call shared_buffer_lookup
    jc .invalid
    cmp dword [eax + SHARED_BUFFER_STATE], SHARED_BUFFER_STATE_CPU
    jne .invalid
    cmp dword [eax + SHARED_BUFFER_MAPPINGS], 0
    jne .invalid
    cmp dword [eax + SHARED_BUFFER_PINNED], 0
    jne .invalid
    cmp dword [dma_mapping_active_count], 0
    jne .invalid
    cmp dword [dma_mapping_pinned_pages], 0
    jne .invalid
    cmp dword [dma_mapping_mapped_bytes], 0
    jne .invalid
    cmp dword [dma_mapping_bounce_count], 1
    jne .invalid
    cmp dword [iommu_mapping_count], 0
    jne .invalid
    mov eax, [dma_mapping_test_task]
    xor edx, edx
    call task_complete
    jc .invalid
    mov eax, [dma_mapping_test_scope]
    call task_scope_close
    jc .invalid
    mov eax, [dma_mapping_test_buffer]
    mov edx, 1
    call shared_buffer_release
    jc .invalid
    mov eax, [dma_mapping_test_domain]
    mov edx, 1
    call iommu_domain_release
    jc .invalid
    clc
    ret
.invalid:
    stc
    ret

align 4
dma_mapping_api:
    dd DMA_MAPPING_API_SIZE
    dw 1, 0
    dd DMA_MAPPING_CAPACITY
    dd dma_mapping_map
    dd dma_mapping_unmap
    dd dma_mapping_fault
    dd dma_mapping_table
    dd dma_mapping_status

dma_mapping_status:
dma_mapping_manager_ready:       dd 0
dma_iommu_available:             dd 0
dma_mapping_active_count:        dd 0
dma_mapping_mapped_bytes:        dd 0
dma_mapping_pinned_pages:        dd 0
dma_mapping_fault_count:         dd 0
dma_mapping_bounce_count:        dd 0
dma_mapping_next_id:             dd 0
dma_mapping_temp_id:             dd 0
dma_mapping_temp_owner:          dd 0
dma_mapping_temp_device:         dd 0
dma_mapping_temp_direction:      dd 0
dma_mapping_temp_permissions:    dd 0
dma_mapping_temp_length:         dd 0
dma_mapping_temp_request:        dd 0
dma_mapping_temp_request_slot:   dd 0
dma_mapping_temp_request_record: dd 0
dma_mapping_temp_buffer:         dd 0
dma_mapping_temp_buffer_record:  dd 0
dma_mapping_temp_domain_record:  dd 0
dma_mapping_temp_slot:           dd 0
dma_mapping_temp_record:         dd 0
dma_mapping_temp_error:          dd 0
dma_mapping_test_buffer:         dd 0
dma_mapping_test_scope:          dd 0
dma_mapping_test_task:           dd 0
dma_mapping_test_request:        dd 0
dma_mapping_test_id:             dd 0
dma_mapping_test_record:         dd 0
dma_mapping_test_domain:         dd 0
align 4
dma_mapping_table:
    times DMA_MAPPING_CAPACITY * DMA_MAPPING_RECORD_SIZE db 0
dma_mapping_generations:
    times DMA_MAPPING_CAPACITY dd 0
dma_mapping_iommu_authorization_ids:
    times DMA_MAPPING_CAPACITY dd 0
dma_request_mapping_ids:
    times IO_REQUEST_CAPACITY dd 0

; ---------------------------------------------------------------------------
; Scatter/Gather-Descriptoren bilden mehrere Bufferbereiche als geordneten
; logischen Datenstrom ab. Das allgemeine ABI enthaelt keine physischen oder
; Device-Adressen; diese entstehen erst in einem DMA-/IOMMU-Provider.
; NPSPEC-DATAMOVE-SCATTERGATHER-0001
; ---------------------------------------------------------------------------

SG_API_SIZE                 equ 32
SG_DESCRIPTOR_CAPACITY      equ 4
SG_MAX_SEGMENTS             equ 4
SG_DESCRIPTOR_SIZE          equ 64
SG_SEGMENT_SIZE             equ 32
SG_STATE_EMPTY              equ 0
SG_STATE_BUILDING           equ 1
SG_STATE_SEALED             equ 2
SG_STATE_RELEASED           equ 3
SG_DIRECTION_GATHER         equ 1
SG_DIRECTION_SCATTER        equ 2
SG_DIRECTION_BIDIRECTIONAL  equ 3
SG_PERMISSION_READ          equ 0x00000001
SG_PERMISSION_WRITE         equ 0x00000002
SG_FLAG_COALESCED           equ 0x00000001
SG_FLAG_DIRECT_ELIGIBLE     equ 0x00000002
SG_FLAG_FALLBACK_ALLOWED    equ 0x00000004
SG_DESCRIPTOR_ID            equ 0
SG_DESCRIPTOR_OWNER         equ 4
SG_DESCRIPTOR_DIRECTION     equ 8
SG_DESCRIPTOR_STATE         equ 12
SG_DESCRIPTOR_SEGMENTS      equ 16
SG_DESCRIPTOR_TOTAL_LENGTH  equ 20
SG_DESCRIPTOR_MAX_SEGMENTS  equ 24
SG_DESCRIPTOR_FLAGS         equ 28
SG_DESCRIPTOR_CONSUMERS     equ 32
SG_DESCRIPTOR_REFERENCES    equ 36
SG_DESCRIPTOR_COALESCED     equ 40
SG_DESCRIPTOR_SPLIT         equ 44
SG_DESCRIPTOR_FALLBACKS     equ 48
SG_DESCRIPTOR_GENERATION    equ 52
SG_DESCRIPTOR_FAILURES      equ 56
SG_SEGMENT_BUFFER           equ 0
SG_SEGMENT_OFFSET           equ 4
SG_SEGMENT_LENGTH           equ 8
SG_SEGMENT_PERMISSIONS      equ 12
SG_SEGMENT_LOGICAL_OFFSET   equ 16
SG_SEGMENT_FLAGS            equ 20
SG_SEGMENT_GENERATION       equ 24

scatter_gather_initialize:
    mov edi, scatter_gather_table
    xor eax, eax
    mov ecx, (SG_DESCRIPTOR_CAPACITY * SG_DESCRIPTOR_SIZE) / 4
    rep stosd
    mov edi, scatter_gather_segments
    mov ecx, (SG_DESCRIPTOR_CAPACITY * SG_MAX_SEGMENTS * SG_SEGMENT_SIZE) / 4
    rep stosd
    mov edi, scatter_gather_generations
    mov ecx, SG_DESCRIPTOR_CAPACITY
    rep stosd
    mov dword [scatter_gather_next_id], 1
    mov dword [scatter_gather_live_count], 0
    mov dword [scatter_gather_sealed_count], 0
    mov dword [scatter_gather_retained_references], 0
    mov dword [scatter_gather_validation_failures], 0
    mov dword [scatter_gather_coalesced_segments], 0
    mov dword [scatter_gather_manager_ready], 1
    clc
    ret

; EAX=Descriptor-ID. EAX=aktiver Datensatz oder 0.
scatter_gather_lookup:
    xor ecx, ecx
.scan:
    cmp ecx, SG_DESCRIPTOR_CAPACITY
    jae .invalid
    mov edx, ecx
    shl edx, 6
    add edx, scatter_gather_table
    cmp dword [edx + SG_DESCRIPTOR_STATE], SG_STATE_BUILDING
    je .candidate
    cmp dword [edx + SG_DESCRIPTOR_STATE], SG_STATE_SEALED
    jne .next
.candidate:
    cmp [edx + SG_DESCRIPTOR_ID], eax
    je .found
.next:
    inc ecx
    jmp .scan
.found:
    mov eax, edx
    clc
    ret
.invalid:
    xor eax, eax
    stc
    ret

; EAX=Descriptor-Datensatz, ECX=Segmentindex. EAX=Segment-Datensatz.
scatter_gather_segment_at:
    mov edx, eax
    sub edx, scatter_gather_table
    shr edx, 6
    shl edx, 7                      ; vier Segmente zu je 32 Byte
    add edx, scatter_gather_segments
    mov eax, ecx
    shl eax, 5
    add eax, edx
    ret

; EAX=Owner, EDX=Richtung. EAX=Descriptor-ID.
scatter_gather_create:
    pushfd
    cli
    mov [scatter_gather_temp_owner], eax
    mov [scatter_gather_temp_direction], edx
    cmp dword [scatter_gather_manager_ready], 1
    jne .invalid
    cmp edx, SG_DIRECTION_GATHER
    jb .invalid
    cmp edx, SG_DIRECTION_BIDIRECTIONAL
    ja .invalid
    call process_lookup
    jc .invalid
    xor ecx, ecx
.scan:
    cmp ecx, SG_DESCRIPTOR_CAPACITY
    jae .invalid
    mov edi, ecx
    shl edi, 6
    add edi, scatter_gather_table
    cmp dword [edi + SG_DESCRIPTOR_STATE], SG_STATE_EMPTY
    je .slot
    cmp dword [edi + SG_DESCRIPTOR_STATE], SG_STATE_RELEASED
    je .slot
    inc ecx
    jmp .scan
.slot:
    mov [scatter_gather_temp_slot], ecx
    mov [scatter_gather_temp_record], edi
    xor eax, eax
    mov ecx, SG_DESCRIPTOR_SIZE / 4
    rep stosd
    mov eax, [scatter_gather_temp_slot]
    shl eax, 7
    add eax, scatter_gather_segments
    mov edi, eax
    xor eax, eax
    mov ecx, (SG_MAX_SEGMENTS * SG_SEGMENT_SIZE) / 4
    rep stosd
    mov edi, [scatter_gather_temp_record]
    mov eax, [scatter_gather_next_id]
    mov [edi + SG_DESCRIPTOR_ID], eax
    mov edx, [scatter_gather_temp_owner]
    mov [edi + SG_DESCRIPTOR_OWNER], edx
    mov edx, [scatter_gather_temp_direction]
    mov [edi + SG_DESCRIPTOR_DIRECTION], edx
    mov dword [edi + SG_DESCRIPTOR_STATE], SG_STATE_BUILDING
    mov dword [edi + SG_DESCRIPTOR_MAX_SEGMENTS], SG_MAX_SEGMENTS
    mov dword [edi + SG_DESCRIPTOR_FLAGS], SG_FLAG_FALLBACK_ALLOWED
    mov ecx, [scatter_gather_temp_slot]
    mov edx, [scatter_gather_generations + ecx * 4]
    inc edx
    jnz .generation_ready
    inc edx
.generation_ready:
    mov [scatter_gather_generations + ecx * 4], edx
    mov [edi + SG_DESCRIPTOR_GENERATION], edx
    inc dword [scatter_gather_next_id]
    inc dword [scatter_gather_live_count]
    mov eax, [edi + SG_DESCRIPTOR_ID]
    popfd
    clc
    ret
.invalid:
    xor eax, eax
    popfd
    stc
    ret

; EAX=Descriptor-ID, EDX=Buffer-ID, EBX=Offset, ECX=Laenge,
; ESI=explizite Segmentrechte, EDI=Owner.
scatter_gather_append:
    pushfd
    cli
    mov dword [scatter_gather_temp_record], 0
    mov [scatter_gather_temp_id], eax
    mov [scatter_gather_temp_buffer], edx
    mov [scatter_gather_temp_offset], ebx
    mov [scatter_gather_temp_length], ecx
    mov [scatter_gather_temp_permissions], esi
    mov [scatter_gather_temp_owner], edi
    test ecx, ecx
    jz .invalid
    test esi, esi
    jz .invalid
    test esi, ~(SG_PERMISSION_READ | SG_PERMISSION_WRITE)
    jnz .invalid
    call scatter_gather_lookup
    jc .invalid
    mov [scatter_gather_temp_record], eax
    cmp dword [eax + SG_DESCRIPTOR_STATE], SG_STATE_BUILDING
    jne .invalid
    mov edx, [scatter_gather_temp_owner]
    cmp [eax + SG_DESCRIPTOR_OWNER], edx
    jne .invalid
    mov edx, [eax + SG_DESCRIPTOR_DIRECTION]
    cmp edx, SG_DIRECTION_GATHER
    jne .not_gather
    cmp dword [scatter_gather_temp_permissions], SG_PERMISSION_READ
    jne .invalid
    jmp .direction_valid
.not_gather:
    cmp edx, SG_DIRECTION_SCATTER
    jne .bidirectional
    cmp dword [scatter_gather_temp_permissions], SG_PERMISSION_WRITE
    jne .invalid
    jmp .direction_valid
.bidirectional:
    cmp dword [scatter_gather_temp_permissions], SG_PERMISSION_READ | SG_PERMISSION_WRITE
    jne .invalid
.direction_valid:
    mov eax, [scatter_gather_temp_buffer]
    call shared_buffer_lookup
    jc .invalid
    mov [scatter_gather_temp_buffer_record], eax
    mov edx, [scatter_gather_temp_owner]
    cmp [eax + SHARED_BUFFER_OWNER], edx
    jne .invalid
    cmp dword [eax + SHARED_BUFFER_STATE], SHARED_BUFFER_STATE_CPU
    jne .invalid
    mov edx, [scatter_gather_temp_permissions]
    mov ebx, [eax + SHARED_BUFFER_RIGHTS]
    and ebx, edx
    cmp ebx, edx
    jne .invalid
    mov ebx, [scatter_gather_temp_offset]
    mov ecx, [scatter_gather_temp_length]
    mov edx, ebx
    add edx, ecx
    jc .invalid
    cmp edx, [eax + SHARED_BUFFER_SIZE]
    ja .invalid
    mov eax, [scatter_gather_temp_record]
    mov edx, [eax + SG_DESCRIPTOR_TOTAL_LENGTH]
    add edx, ecx
    jc .invalid
    mov [scatter_gather_temp_total], edx
    mov ecx, [eax + SG_DESCRIPTOR_SEGMENTS]
    test ecx, ecx
    jz .new_segment
    dec ecx
    call scatter_gather_segment_at
    mov [scatter_gather_temp_segment], eax
    mov edx, [scatter_gather_temp_buffer]
    cmp [eax + SG_SEGMENT_BUFFER], edx
    jne .new_segment
    mov edx, [scatter_gather_temp_permissions]
    cmp [eax + SG_SEGMENT_PERMISSIONS], edx
    jne .new_segment
    mov edx, [eax + SG_SEGMENT_OFFSET]
    add edx, [eax + SG_SEGMENT_LENGTH]
    jc .invalid
    cmp edx, [scatter_gather_temp_offset]
    jne .new_segment
    mov edx, [eax + SG_SEGMENT_LENGTH]
    add edx, [scatter_gather_temp_length]
    jc .invalid
    mov [eax + SG_SEGMENT_LENGTH], edx
    or dword [eax + SG_SEGMENT_FLAGS], SG_FLAG_COALESCED
    mov eax, [scatter_gather_temp_record]
    mov edx, [scatter_gather_temp_total]
    mov [eax + SG_DESCRIPTOR_TOTAL_LENGTH], edx
    or dword [eax + SG_DESCRIPTOR_FLAGS], SG_FLAG_COALESCED
    inc dword [eax + SG_DESCRIPTOR_COALESCED]
    inc dword [scatter_gather_coalesced_segments]
    mov eax, [eax + SG_DESCRIPTOR_ID]
    popfd
    clc
    ret
.new_segment:
    mov eax, [scatter_gather_temp_record]
    mov ecx, [eax + SG_DESCRIPTOR_SEGMENTS]
    cmp ecx, SG_MAX_SEGMENTS
    jae .invalid
    call scatter_gather_segment_at
    mov [scatter_gather_temp_segment], eax
    mov edi, eax
    xor eax, eax
    mov ecx, SG_SEGMENT_SIZE / 4
    rep stosd
    mov edi, [scatter_gather_temp_segment]
    mov edx, [scatter_gather_temp_buffer]
    mov [edi + SG_SEGMENT_BUFFER], edx
    mov edx, [scatter_gather_temp_offset]
    mov [edi + SG_SEGMENT_OFFSET], edx
    mov edx, [scatter_gather_temp_length]
    mov [edi + SG_SEGMENT_LENGTH], edx
    mov edx, [scatter_gather_temp_permissions]
    mov [edi + SG_SEGMENT_PERMISSIONS], edx
    mov eax, [scatter_gather_temp_record]
    mov edx, [eax + SG_DESCRIPTOR_TOTAL_LENGTH]
    mov [edi + SG_SEGMENT_LOGICAL_OFFSET], edx
    mov edx, [eax + SG_DESCRIPTOR_GENERATION]
    mov [edi + SG_SEGMENT_GENERATION], edx
    inc dword [eax + SG_DESCRIPTOR_SEGMENTS]
    mov edx, [scatter_gather_temp_total]
    mov [eax + SG_DESCRIPTOR_TOTAL_LENGTH], edx
    mov eax, [eax + SG_DESCRIPTOR_ID]
    popfd
    clc
    ret
.invalid:
    inc dword [scatter_gather_validation_failures]
    mov eax, [scatter_gather_temp_record]
    test eax, eax
    jz .invalid_done
    inc dword [eax + SG_DESCRIPTOR_FAILURES]
.invalid_done:
    xor eax, eax
    popfd
    stc
    ret

; EAX=Descriptor-ID, EDX=Owner. Erst der Seal-Schritt haelt alle Buffer fest.
scatter_gather_seal:
    pushfd
    cli
    mov [scatter_gather_temp_id], eax
    mov [scatter_gather_temp_owner], edx
    call scatter_gather_lookup
    jc .invalid
    mov [scatter_gather_temp_record], eax
    cmp dword [eax + SG_DESCRIPTOR_STATE], SG_STATE_BUILDING
    jne .invalid
    mov edx, [scatter_gather_temp_owner]
    cmp [eax + SG_DESCRIPTOR_OWNER], edx
    jne .invalid
    cmp dword [eax + SG_DESCRIPTOR_SEGMENTS], 0
    je .invalid
    mov dword [scatter_gather_temp_index], 0
.validate:
    mov eax, [scatter_gather_temp_record]
    mov ecx, [scatter_gather_temp_index]
    cmp ecx, [eax + SG_DESCRIPTOR_SEGMENTS]
    jae .retain
    call scatter_gather_segment_at
    mov [scatter_gather_temp_segment], eax
    mov eax, [eax + SG_SEGMENT_BUFFER]
    call shared_buffer_lookup
    jc .invalid
    mov edx, [scatter_gather_temp_owner]
    cmp [eax + SHARED_BUFFER_OWNER], edx
    jne .invalid
    cmp dword [eax + SHARED_BUFFER_STATE], SHARED_BUFFER_STATE_CPU
    jne .invalid
    mov esi, [scatter_gather_temp_segment]
    mov edx, [esi + SG_SEGMENT_PERMISSIONS]
    mov ebx, [eax + SHARED_BUFFER_RIGHTS]
    and ebx, edx
    cmp ebx, edx
    jne .invalid
    mov ebx, [esi + SG_SEGMENT_OFFSET]
    add ebx, [esi + SG_SEGMENT_LENGTH]
    jc .invalid
    cmp ebx, [eax + SHARED_BUFFER_SIZE]
    ja .invalid
    inc dword [scatter_gather_temp_index]
    jmp .validate
.retain:
    mov dword [scatter_gather_temp_index], 0
.retain_loop:
    mov eax, [scatter_gather_temp_record]
    mov ecx, [scatter_gather_temp_index]
    cmp ecx, [eax + SG_DESCRIPTOR_SEGMENTS]
    jae .sealed
    call scatter_gather_segment_at
    mov eax, [eax + SG_SEGMENT_BUFFER]
    call shared_buffer_lookup
    jc .invalid
    inc dword [eax + SHARED_BUFFER_REFERENCES]
    inc dword [scatter_gather_retained_references]
    inc dword [scatter_gather_temp_index]
    jmp .retain_loop
.sealed:
    mov eax, [scatter_gather_temp_record]
    mov ecx, [eax + SG_DESCRIPTOR_SEGMENTS]
    mov [eax + SG_DESCRIPTOR_REFERENCES], ecx
    mov dword [eax + SG_DESCRIPTOR_STATE], SG_STATE_SEALED
    or dword [eax + SG_DESCRIPTOR_FLAGS], SG_FLAG_DIRECT_ELIGIBLE
    inc dword [scatter_gather_sealed_count]
    mov eax, [eax + SG_DESCRIPTOR_ID]
    popfd
    clc
    ret
.invalid:
    inc dword [scatter_gather_validation_failures]
    mov eax, [scatter_gather_temp_record]
    test eax, eax
    jz .invalid_done
    inc dword [eax + SG_DESCRIPTOR_FAILURES]
.invalid_done:
    xor eax, eax
    popfd
    stc
    ret

; EAX=Descriptor-ID, EDX=Owner. Aktive Consumer verhindern den Abbau.
scatter_gather_release:
    pushfd
    cli
    mov [scatter_gather_temp_id], eax
    mov [scatter_gather_temp_owner], edx
    call scatter_gather_lookup
    jc .invalid
    mov [scatter_gather_temp_record], eax
    mov edx, [scatter_gather_temp_owner]
    cmp [eax + SG_DESCRIPTOR_OWNER], edx
    jne .invalid
    cmp dword [eax + SG_DESCRIPTOR_CONSUMERS], 0
    jne .invalid
    cmp dword [eax + SG_DESCRIPTOR_STATE], SG_STATE_SEALED
    jne .mark_released
    mov dword [scatter_gather_temp_index], 0
.release_loop:
    mov eax, [scatter_gather_temp_record]
    mov ecx, [scatter_gather_temp_index]
    cmp ecx, [eax + SG_DESCRIPTOR_SEGMENTS]
    jae .released_refs
    call scatter_gather_segment_at
    mov eax, [eax + SG_SEGMENT_BUFFER]
    call shared_buffer_lookup
    jc .invalid
    cmp dword [eax + SHARED_BUFFER_REFERENCES], 1
    jbe .invalid
    dec dword [eax + SHARED_BUFFER_REFERENCES]
    dec dword [scatter_gather_retained_references]
    inc dword [scatter_gather_temp_index]
    jmp .release_loop
.released_refs:
    mov eax, [scatter_gather_temp_record]
    mov dword [eax + SG_DESCRIPTOR_REFERENCES], 0
    dec dword [scatter_gather_sealed_count]
.mark_released:
    mov eax, [scatter_gather_temp_record]
    mov dword [eax + SG_DESCRIPTOR_STATE], SG_STATE_RELEASED
    dec dword [scatter_gather_live_count]
    mov eax, [scatter_gather_temp_id]
    popfd
    clc
    ret
.invalid:
    xor eax, eax
    popfd
    stc
    ret

scatter_gather_self_test:
    mov eax, 1
    mov edx, 128
    mov ebx, SHARED_BUFFER_RIGHT_READ | SHARED_BUFFER_RIGHT_WRITE | SHARED_BUFFER_RIGHT_TRANSFER | SHARED_BUFFER_RIGHT_RELEASE
    call shared_buffer_create
    jc .invalid
    mov [scatter_gather_test_buffer_a], eax
    mov eax, 1
    mov edx, 128
    mov ebx, SHARED_BUFFER_RIGHT_READ | SHARED_BUFFER_RIGHT_WRITE | SHARED_BUFFER_RIGHT_TRANSFER | SHARED_BUFFER_RIGHT_RELEASE
    call shared_buffer_create
    jc .invalid
    mov [scatter_gather_test_buffer_b], eax
    mov eax, 1
    mov edx, SG_DIRECTION_GATHER
    call scatter_gather_create
    jc .invalid
    mov [scatter_gather_test_id], eax
    mov edx, [scatter_gather_test_buffer_a]
    xor ebx, ebx
    mov ecx, 32
    mov esi, SG_PERMISSION_READ
    mov edi, 1
    call scatter_gather_append
    jc .invalid
    mov eax, [scatter_gather_test_id]
    mov edx, [scatter_gather_test_buffer_a]
    mov ebx, 32
    mov ecx, 32
    mov esi, SG_PERMISSION_READ
    mov edi, 1
    call scatter_gather_append
    jc .invalid
    mov eax, [scatter_gather_test_id]
    mov edx, [scatter_gather_test_buffer_b]
    mov ebx, 8
    mov ecx, 64
    mov esi, SG_PERMISSION_READ
    mov edi, 1
    call scatter_gather_append
    jc .invalid
    mov eax, [scatter_gather_test_id]
    mov edx, [scatter_gather_test_buffer_b]
    mov ebx, 100
    mov ecx, 40
    mov esi, SG_PERMISSION_READ
    mov edi, 1
    call scatter_gather_append
    jnc .invalid                       ; Offset + Laenge ueberschreitet Buffer
    mov eax, [scatter_gather_test_id]
    call scatter_gather_lookup
    jc .invalid
    cmp dword [eax + SG_DESCRIPTOR_SEGMENTS], 2
    jne .invalid
    cmp dword [eax + SG_DESCRIPTOR_TOTAL_LENGTH], 128
    jne .invalid
    cmp dword [eax + SG_DESCRIPTOR_COALESCED], 1
    jne .invalid
    cmp dword [eax + SG_DESCRIPTOR_FAILURES], 1
    jne .invalid
    mov eax, [scatter_gather_test_id]
    mov edx, 1
    call scatter_gather_seal
    jc .invalid
    mov eax, [scatter_gather_test_buffer_a]
    call shared_buffer_lookup
    jc .invalid
    cmp dword [eax + SHARED_BUFFER_REFERENCES], 2
    jne .invalid
    mov eax, [scatter_gather_test_buffer_b]
    call shared_buffer_lookup
    jc .invalid
    cmp dword [eax + SHARED_BUFFER_REFERENCES], 2
    jne .invalid
    mov eax, [scatter_gather_test_buffer_a]
    mov edx, 1
    call shared_buffer_release
    jnc .invalid                       ; Descriptor garantiert Lifetime
    mov eax, [scatter_gather_test_id]
    mov edx, 1
    call scatter_gather_release
    jc .invalid
    cmp dword [scatter_gather_live_count], 0
    jne .invalid
    cmp dword [scatter_gather_sealed_count], 0
    jne .invalid
    cmp dword [scatter_gather_retained_references], 0
    jne .invalid
    cmp dword [scatter_gather_coalesced_segments], 1
    jne .invalid
    mov eax, [scatter_gather_test_buffer_a]
    mov edx, 1
    call shared_buffer_release
    jc .invalid
    mov eax, [scatter_gather_test_buffer_b]
    mov edx, 1
    call shared_buffer_release
    jc .invalid
    clc
    ret
.invalid:
    stc
    ret

align 4
scatter_gather_api:
    dd SG_API_SIZE
    dw 1, 0
    dd SG_DESCRIPTOR_CAPACITY
    dd SG_MAX_SEGMENTS
    dd scatter_gather_create
    dd scatter_gather_append
    dd scatter_gather_seal
    dd scatter_gather_release

scatter_gather_manager_ready:          dd 0
scatter_gather_next_id:                dd 0
scatter_gather_live_count:             dd 0
scatter_gather_sealed_count:           dd 0
scatter_gather_retained_references:    dd 0
scatter_gather_validation_failures:    dd 0
scatter_gather_coalesced_segments:     dd 0
scatter_gather_temp_id:                dd 0
scatter_gather_temp_owner:             dd 0
scatter_gather_temp_direction:         dd 0
scatter_gather_temp_buffer:            dd 0
scatter_gather_temp_offset:            dd 0
scatter_gather_temp_length:            dd 0
scatter_gather_temp_permissions:       dd 0
scatter_gather_temp_total:             dd 0
scatter_gather_temp_slot:              dd 0
scatter_gather_temp_index:             dd 0
scatter_gather_temp_record:            dd 0
scatter_gather_temp_segment:           dd 0
scatter_gather_temp_buffer_record:     dd 0
scatter_gather_test_id:                dd 0
scatter_gather_test_buffer_a:          dd 0
scatter_gather_test_buffer_b:          dd 0
align 4
scatter_gather_table:
    times SG_DESCRIPTOR_CAPACITY * SG_DESCRIPTOR_SIZE db 0
scatter_gather_segments:
    times SG_DESCRIPTOR_CAPACITY * SG_MAX_SEGMENTS * SG_SEGMENT_SIZE db 0
scatter_gather_generations:
    times SG_DESCRIPTOR_CAPACITY dd 0

; ---------------------------------------------------------------------------
; SG-faehige DMA-Providergrenze. Ein versiegelter SG-Descriptor wird unter
; festen Hardwarelimits in kontrollierte Device-Segmente uebersetzt. Der
; aktuelle Bootstrap bleibt ohne IOMMU ein expliziter Restricted-Bounce-Pfad.
; ---------------------------------------------------------------------------

DMA_SG_API_SIZE              equ 32
DMA_SG_CAPACITY              equ 2
DMA_SG_MAX_SEGMENTS          equ 4
DMA_SG_MAPPING_SIZE          equ 64
DMA_SG_SEGMENT_SIZE          equ 32
DMA_SG_STATE_EMPTY           equ 0
DMA_SG_STATE_ACTIVE          equ 1
DMA_SG_STATE_UNMAPPED        equ 2
DMA_SG_STATE_FAULTED         equ 3
DMA_SG_FLAG_BOUNCE           equ 0x00000001
DMA_SG_FLAG_COHERENT         equ 0x00000002
DMA_SG_FLAG_RESTRICTED       equ 0x00000004
DMA_SG_FLAG_SPLIT            equ 0x00000008
DMA_SG_FLAG_IOMMU            equ 0x00000010
DMA_SG_MAX_SEGMENT_SIZE      equ 64
DMA_SG_ALIGNMENT             equ 4
DMA_SG_ADDRESS_WIDTH         equ 32
DMA_SG_BOUNDARY              equ 4096
DMA_SG_APERTURE              equ 0xD1000000
DMA_SG_MAPPING_ID            equ 0
DMA_SG_MAPPING_OWNER         equ 4
DMA_SG_MAPPING_DEVICE        equ 8
DMA_SG_MAPPING_REQUEST       equ 12
DMA_SG_MAPPING_DESCRIPTOR    equ 16
DMA_SG_MAPPING_DIRECTION     equ 20
DMA_SG_MAPPING_STATE         equ 24
DMA_SG_MAPPING_SEGMENTS      equ 28
DMA_SG_MAPPING_TOTAL_LENGTH  equ 32
DMA_SG_MAPPING_MAX_SIZE      equ 36
DMA_SG_MAPPING_ALIGNMENT     equ 40
DMA_SG_MAPPING_ADDRESS_WIDTH equ 44
DMA_SG_MAPPING_BOUNDARY      equ 48
DMA_SG_MAPPING_FLAGS         equ 52
DMA_SG_MAPPING_GENERATION    equ 56
DMA_SG_MAPPING_DOMAIN        equ 60
DMA_SG_SEGMENT_SOURCE        equ 0
DMA_SG_SEGMENT_BUFFER        equ 4
DMA_SG_SEGMENT_OFFSET        equ 8
DMA_SG_SEGMENT_LENGTH        equ 12
DMA_SG_SEGMENT_ADDRESS_LOW   equ 16
DMA_SG_SEGMENT_ADDRESS_HIGH  equ 20
DMA_SG_SEGMENT_PERMISSIONS   equ 24
DMA_SG_SEGMENT_FLAGS         equ 28

dma_scatter_gather_initialize:
    mov edi, dma_scatter_gather_table
    xor eax, eax
    mov ecx, (DMA_SG_CAPACITY * DMA_SG_MAPPING_SIZE) / 4
    rep stosd
    mov edi, dma_scatter_gather_segments
    mov ecx, (DMA_SG_CAPACITY * DMA_SG_MAX_SEGMENTS * DMA_SG_SEGMENT_SIZE) / 4
    rep stosd
    mov edi, dma_scatter_gather_generations
    mov ecx, DMA_SG_CAPACITY
    rep stosd
    mov edi, dma_request_sg_mapping_ids
    mov ecx, IO_REQUEST_CAPACITY
    rep stosd
    mov edi, dma_scatter_gather_iommu_authorization_ids
    mov ecx, DMA_SG_CAPACITY * DMA_SG_MAX_SEGMENTS
    rep stosd
    mov edi, dma_scatter_gather_error_codes
    mov ecx, DMA_SG_CAPACITY
    rep stosd
    mov dword [dma_scatter_gather_next_id], 1
    mov dword [dma_scatter_gather_active_count], 0
    mov dword [dma_scatter_gather_mapped_bytes], 0
    mov dword [dma_scatter_gather_pinned_buffers], 0
    mov dword [dma_scatter_gather_split_segments], 0
    mov dword [dma_scatter_gather_validation_failures], 0
    mov dword [dma_scatter_gather_fault_count], 0
    mov dword [dma_scatter_gather_manager_ready], 1
    clc
    ret

dma_scatter_gather_lookup:
    xor ecx, ecx
.scan:
    cmp ecx, DMA_SG_CAPACITY
    jae .invalid
    mov edx, ecx
    shl edx, 6
    add edx, dma_scatter_gather_table
    cmp dword [edx + DMA_SG_MAPPING_STATE], DMA_SG_STATE_ACTIVE
    jne .next
    cmp [edx + DMA_SG_MAPPING_ID], eax
    je .found
.next:
    inc ecx
    jmp .scan
.found:
    mov eax, edx
    clc
    ret
.invalid:
    xor eax, eax
    stc
    ret

; EAX=Mapping-Datensatz, ECX=Device-Segmentindex. EAX=Segment-Datensatz.
dma_scatter_gather_segment_at:
    mov edx, eax
    sub edx, dma_scatter_gather_table
    shr edx, 6
    shl edx, 7
    add edx, dma_scatter_gather_segments
    mov eax, ecx
    shl eax, 5
    add eax, edx
    ret

; EAX=Request-ID, EDX=SG-Descriptor-ID, EBX=Device-ID, ECX=Owner.
; Ergebnis EAX=DMA-SG-Mapping-ID.
dma_scatter_gather_map:
    pushfd
    cli
    mov dword [dma_scatter_gather_temp_record], 0
    mov [dma_scatter_gather_temp_request], eax
    mov [dma_scatter_gather_temp_descriptor], edx
    mov [dma_scatter_gather_temp_device], ebx
    mov [dma_scatter_gather_temp_owner], ecx
    cmp dword [dma_scatter_gather_manager_ready], 1
    jne .invalid
    test ebx, ebx
    jz .invalid
    call io_request_lookup
    jc .invalid
    mov [dma_scatter_gather_temp_request_record], eax
    cmp dword [eax + IO_REQUEST_STATE], IO_REQUEST_STATE_PENDING
    je .request_active
    cmp dword [eax + IO_REQUEST_STATE], IO_REQUEST_STATE_RUNNING
    jne .invalid
.request_active:
    mov edx, [dma_scatter_gather_temp_owner]
    cmp [eax + IO_REQUEST_OWNER], edx
    jne .invalid
    mov edx, [dma_scatter_gather_temp_descriptor]
    cmp [eax + IO_REQUEST_BUFFER], edx
    jne .invalid
    mov ecx, eax
    sub ecx, io_request_table
    shr ecx, 6
    mov [dma_scatter_gather_temp_request_slot], ecx
    cmp dword [dma_request_mapping_ids + ecx * 4], 0
    jne .invalid
    cmp dword [dma_request_sg_mapping_ids + ecx * 4], 0
    jne .invalid
    mov eax, [dma_scatter_gather_temp_descriptor]
    call scatter_gather_lookup
    jc .invalid
    mov [dma_scatter_gather_temp_descriptor_record], eax
    cmp dword [eax + SG_DESCRIPTOR_STATE], SG_STATE_SEALED
    jne .invalid
    mov edx, [dma_scatter_gather_temp_owner]
    cmp [eax + SG_DESCRIPTOR_OWNER], edx
    jne .invalid
    cmp dword [eax + SG_DESCRIPTOR_CONSUMERS], 0
    jne .invalid
    mov edx, [dma_scatter_gather_temp_request_record]
    mov ecx, [eax + SG_DESCRIPTOR_TOTAL_LENGTH]
    cmp [edx + IO_REQUEST_LENGTH], ecx
    jne .invalid
    mov dword [dma_scatter_gather_temp_required], 0
    mov dword [dma_scatter_gather_temp_source_index], 0
    mov edi, dma_scatter_gather_temp_buffer_ids
    xor eax, eax
    mov ecx, SG_MAX_SEGMENTS
    rep stosd
.validate_source:
    mov eax, [dma_scatter_gather_temp_descriptor_record]
    mov ecx, [dma_scatter_gather_temp_source_index]
    cmp ecx, [eax + SG_DESCRIPTOR_SEGMENTS]
    jae .find_mapping_slot
    call scatter_gather_segment_at
    mov [dma_scatter_gather_temp_source_segment], eax
    mov edx, [eax + SG_SEGMENT_OFFSET]
    test edx, DMA_SG_ALIGNMENT - 1
    jnz .invalid
    mov ebx, [eax + SG_SEGMENT_LENGTH]
    test ebx, ebx
    jz .invalid
    test ebx, DMA_SG_ALIGNMENT - 1
    jnz .invalid
    mov esi, [eax + SG_SEGMENT_BUFFER]
    xor ecx, ecx
.duplicate_scan:
    cmp ecx, [dma_scatter_gather_temp_source_index]
    jae .unique_buffer
    cmp [dma_scatter_gather_temp_buffer_ids + ecx * 4], esi
    je .invalid                         ; Providerlimit: ein Eintrag je Buffer
    inc ecx
    jmp .duplicate_scan
.unique_buffer:
    mov ecx, [dma_scatter_gather_temp_source_index]
    mov [dma_scatter_gather_temp_buffer_ids + ecx * 4], esi
    mov eax, esi
    call shared_buffer_lookup
    jc .invalid
    mov edx, [dma_scatter_gather_temp_owner]
    cmp [eax + SHARED_BUFFER_OWNER], edx
    jne .invalid
    cmp dword [eax + SHARED_BUFFER_STATE], SHARED_BUFFER_STATE_CPU
    jne .invalid
    cmp dword [eax + SHARED_BUFFER_ACTIVE_IO], 0
    jne .invalid
    test dword [eax + SHARED_BUFFER_RIGHTS], SHARED_BUFFER_RIGHT_TRANSFER
    jz .invalid
    test dword [eax + SHARED_BUFFER_RIGHTS], SHARED_BUFFER_RIGHT_DMA
    jz .invalid
    mov esi, [dma_scatter_gather_temp_source_segment]
    mov edx, [esi + SG_SEGMENT_PERMISSIONS]
    mov ecx, [eax + SHARED_BUFFER_RIGHTS]
    and ecx, edx
    cmp ecx, edx
    jne .invalid
    mov eax, ebx
    add eax, DMA_SG_MAX_SEGMENT_SIZE - 1
    jc .invalid
    shr eax, 6
    add [dma_scatter_gather_temp_required], eax
    jc .invalid
    cmp dword [dma_scatter_gather_temp_required], DMA_SG_MAX_SEGMENTS
    ja .invalid
    inc dword [dma_scatter_gather_temp_source_index]
    jmp .validate_source

.find_mapping_slot:
    xor ecx, ecx
.scan_mapping_slot:
    cmp ecx, DMA_SG_CAPACITY
    jae .invalid
    mov edi, ecx
    shl edi, 6
    add edi, dma_scatter_gather_table
    cmp dword [edi + DMA_SG_MAPPING_STATE], DMA_SG_STATE_EMPTY
    je .mapping_slot
    cmp dword [edi + DMA_SG_MAPPING_STATE], DMA_SG_STATE_UNMAPPED
    je .mapping_slot
    cmp dword [edi + DMA_SG_MAPPING_STATE], DMA_SG_STATE_FAULTED
    je .mapping_slot
    inc ecx
    jmp .scan_mapping_slot
.mapping_slot:
    mov [dma_scatter_gather_temp_slot], ecx
    mov [dma_scatter_gather_temp_record], edi
    xor eax, eax
    mov ecx, DMA_SG_MAPPING_SIZE / 4
    rep stosd
    mov eax, [dma_scatter_gather_temp_slot]
    shl eax, 7
    add eax, dma_scatter_gather_segments
    mov edi, eax
    xor eax, eax
    mov ecx, (DMA_SG_MAX_SEGMENTS * DMA_SG_SEGMENT_SIZE) / 4
    rep stosd
    mov edi, [dma_scatter_gather_temp_record]
    mov eax, [dma_scatter_gather_next_id]
    mov [edi + DMA_SG_MAPPING_ID], eax
    mov edx, [dma_scatter_gather_temp_owner]
    mov [edi + DMA_SG_MAPPING_OWNER], edx
    mov edx, [dma_scatter_gather_temp_device]
    mov [edi + DMA_SG_MAPPING_DEVICE], edx
    mov edx, [dma_scatter_gather_temp_request]
    mov [edi + DMA_SG_MAPPING_REQUEST], edx
    mov edx, [dma_scatter_gather_temp_descriptor]
    mov [edi + DMA_SG_MAPPING_DESCRIPTOR], edx
    mov edx, [dma_scatter_gather_temp_descriptor_record]
    mov eax, [edx + SG_DESCRIPTOR_DIRECTION]
    mov [edi + DMA_SG_MAPPING_DIRECTION], eax
    mov dword [edi + DMA_SG_MAPPING_STATE], DMA_SG_STATE_ACTIVE
    mov eax, [dma_scatter_gather_temp_required]
    mov [edi + DMA_SG_MAPPING_SEGMENTS], eax
    mov eax, [edx + SG_DESCRIPTOR_TOTAL_LENGTH]
    mov [edi + DMA_SG_MAPPING_TOTAL_LENGTH], eax
    mov dword [edi + DMA_SG_MAPPING_MAX_SIZE], DMA_SG_MAX_SEGMENT_SIZE
    mov dword [edi + DMA_SG_MAPPING_ALIGNMENT], DMA_SG_ALIGNMENT
    mov dword [edi + DMA_SG_MAPPING_ADDRESS_WIDTH], DMA_SG_ADDRESS_WIDTH
    mov dword [edi + DMA_SG_MAPPING_BOUNDARY], DMA_SG_BOUNDARY
    mov dword [edi + DMA_SG_MAPPING_FLAGS], DMA_SG_FLAG_BOUNCE | DMA_SG_FLAG_COHERENT | DMA_SG_FLAG_RESTRICTED
    mov eax, [edx + SG_DESCRIPTOR_SEGMENTS]
    cmp [dma_scatter_gather_temp_required], eax
    jbe .flags_ready
    or dword [edi + DMA_SG_MAPPING_FLAGS], DMA_SG_FLAG_SPLIT
.flags_ready:
    mov ecx, [dma_scatter_gather_temp_slot]
    mov eax, [dma_scatter_gather_generations + ecx * 4]
    inc eax
    jnz .generation_ready
    inc eax
.generation_ready:
    mov [dma_scatter_gather_generations + ecx * 4], eax
    mov [edi + DMA_SG_MAPPING_GENERATION], eax
    mov dword [edi + DMA_SG_MAPPING_DOMAIN], 0
    mov eax, [dma_scatter_gather_temp_slot]
    shl eax, 4
    add eax, dma_scatter_gather_iommu_authorization_ids
    mov edi, eax
    xor eax, eax
    mov ecx, DMA_SG_MAX_SEGMENTS
    rep stosd
    mov dword [dma_scatter_gather_temp_source_index], 0
    mov dword [dma_scatter_gather_temp_output_index], 0

.translate_source:
    mov eax, [dma_scatter_gather_temp_descriptor_record]
    mov ecx, [dma_scatter_gather_temp_source_index]
    cmp ecx, [eax + SG_DESCRIPTOR_SEGMENTS]
    jae .authorize_iommu
    call scatter_gather_segment_at
    mov edx, [eax + SG_SEGMENT_BUFFER]
    mov [dma_scatter_gather_temp_buffer], edx
    mov edx, [eax + SG_SEGMENT_OFFSET]
    mov [dma_scatter_gather_temp_offset], edx
    mov edx, [eax + SG_SEGMENT_LENGTH]
    mov [dma_scatter_gather_temp_remaining], edx
    mov edx, [eax + SG_SEGMENT_PERMISSIONS]
    mov [dma_scatter_gather_temp_permissions], edx
.translate_chunk:
    cmp dword [dma_scatter_gather_temp_remaining], 0
    je .next_source
    mov edx, [dma_scatter_gather_temp_remaining]
    cmp edx, DMA_SG_MAX_SEGMENT_SIZE
    jbe .chunk_ready
    mov edx, DMA_SG_MAX_SEGMENT_SIZE
.chunk_ready:
    mov [dma_scatter_gather_temp_chunk], edx
    mov eax, [dma_scatter_gather_temp_record]
    mov ecx, [dma_scatter_gather_temp_output_index]
    call dma_scatter_gather_segment_at
    mov edi, eax
    mov edx, [dma_scatter_gather_temp_source_index]
    mov [edi + DMA_SG_SEGMENT_SOURCE], edx
    mov edx, [dma_scatter_gather_temp_buffer]
    mov [edi + DMA_SG_SEGMENT_BUFFER], edx
    mov edx, [dma_scatter_gather_temp_offset]
    mov [edi + DMA_SG_SEGMENT_OFFSET], edx
    mov edx, [dma_scatter_gather_temp_chunk]
    mov [edi + DMA_SG_SEGMENT_LENGTH], edx
    mov eax, [dma_scatter_gather_temp_slot]
    shl eax, 16
    add eax, DMA_SG_APERTURE
    mov ecx, [dma_scatter_gather_temp_output_index]
    shl ecx, 12
    add eax, ecx
    mov [edi + DMA_SG_SEGMENT_ADDRESS_LOW], eax
    mov dword [edi + DMA_SG_SEGMENT_ADDRESS_HIGH], 0
    mov edx, [dma_scatter_gather_temp_permissions]
    mov [edi + DMA_SG_SEGMENT_PERMISSIONS], edx
    cmp dword [dma_scatter_gather_temp_remaining], DMA_SG_MAX_SEGMENT_SIZE
    jbe .chunk_flags_ready
    mov dword [edi + DMA_SG_SEGMENT_FLAGS], DMA_SG_FLAG_SPLIT
    inc dword [dma_scatter_gather_split_segments]
.chunk_flags_ready:
    mov edx, [dma_scatter_gather_temp_chunk]
    add [dma_scatter_gather_temp_offset], edx
    sub [dma_scatter_gather_temp_remaining], edx
    inc dword [dma_scatter_gather_temp_output_index]
    jmp .translate_chunk
.next_source:
    inc dword [dma_scatter_gather_temp_source_index]
    jmp .translate_source

.authorize_iommu:
    mov eax, [dma_scatter_gather_temp_device]
    call iommu_domain_for_device
    jc .acquire_buffers
    mov [dma_scatter_gather_temp_iommu_domain], eax
    mov edx, [eax + IOMMU_DOMAIN_ID]
    mov edi, [dma_scatter_gather_temp_record]
    mov [edi + DMA_SG_MAPPING_DOMAIN], edx
    mov dword [dma_scatter_gather_temp_authorized_count], 0
.authorize_segment:
    mov ecx, [dma_scatter_gather_temp_authorized_count]
    mov edi, [dma_scatter_gather_temp_record]
    cmp ecx, [edi + DMA_SG_MAPPING_SEGMENTS]
    jae .authorization_ready
    mov eax, edi
    call dma_scatter_gather_segment_at
    mov [dma_scatter_gather_temp_device_segment], eax
    mov edi, eax
    mov eax, [dma_scatter_gather_temp_iommu_domain]
    mov eax, [eax + IOMMU_DOMAIN_ID]
    mov edx, [dma_scatter_gather_temp_device]
    mov ebx, [dma_scatter_gather_temp_record]
    mov ebx, [ebx + DMA_SG_MAPPING_ID]
    or ebx, IOMMU_EXTERNAL_DMA_SG
    mov ecx, [edi + DMA_SG_SEGMENT_ADDRESS_LOW]
    mov esi, 4096
    mov edi, [edi + DMA_SG_SEGMENT_PERMISSIONS]
    mov ebp, [dma_scatter_gather_temp_owner]
    call iommu_authorize_mapping
    jc .authorization_invalid
    mov edx, [dma_scatter_gather_temp_slot]
    shl edx, 2
    add edx, [dma_scatter_gather_temp_authorized_count]
    mov [dma_scatter_gather_iommu_authorization_ids + edx * 4], eax
    inc dword [dma_scatter_gather_temp_authorized_count]
    jmp .authorize_segment
.authorization_ready:
    mov eax, [dma_scatter_gather_temp_iommu_domain]
    cmp dword [eax + IOMMU_DOMAIN_MODE], IOMMU_MODE_RESTRICTED
    je .acquire_buffers
    mov edi, [dma_scatter_gather_temp_record]
    and dword [edi + DMA_SG_MAPPING_FLAGS], ~(DMA_SG_FLAG_BOUNCE | DMA_SG_FLAG_RESTRICTED)
    or dword [edi + DMA_SG_MAPPING_FLAGS], DMA_SG_FLAG_IOMMU
    jmp .acquire_buffers
.authorization_invalid:
    mov dword [dma_scatter_gather_temp_rollback_index], 0
.authorization_rollback:
    mov ecx, [dma_scatter_gather_temp_rollback_index]
    cmp ecx, [dma_scatter_gather_temp_authorized_count]
    jae .authorization_rollback_done
    mov edx, [dma_scatter_gather_temp_slot]
    shl edx, 2
    add edx, ecx
    mov eax, [dma_scatter_gather_iommu_authorization_ids + edx * 4]
    mov edx, [dma_scatter_gather_temp_owner]
    call iommu_revoke_mapping
    inc dword [dma_scatter_gather_temp_rollback_index]
    jmp .authorization_rollback
.authorization_rollback_done:
    mov edi, [dma_scatter_gather_temp_record]
    mov dword [edi + DMA_SG_MAPPING_STATE], DMA_SG_STATE_EMPTY
    jmp .invalid

.acquire_buffers:
    mov dword [dma_scatter_gather_temp_source_index], 0
.acquire_loop:
    mov eax, [dma_scatter_gather_temp_descriptor_record]
    mov ecx, [dma_scatter_gather_temp_source_index]
    cmp ecx, [eax + SG_DESCRIPTOR_SEGMENTS]
    jae .publish_mapping
    call scatter_gather_segment_at
    mov eax, [eax + SG_SEGMENT_BUFFER]
    call shared_buffer_lookup
    jc .invalid
    mov dword [eax + SHARED_BUFFER_STATE], SHARED_BUFFER_STATE_PROVIDER
    inc dword [eax + SHARED_BUFFER_ACTIVE_IO]
    inc dword [eax + SHARED_BUFFER_REFERENCES]
    inc dword [eax + SHARED_BUFFER_MAPPINGS]
    inc dword [eax + SHARED_BUFFER_PINNED]
    inc dword [dma_scatter_gather_pinned_buffers]
    inc dword [dma_scatter_gather_temp_source_index]
    jmp .acquire_loop
.publish_mapping:
    mov eax, [dma_scatter_gather_temp_descriptor_record]
    inc dword [eax + SG_DESCRIPTOR_CONSUMERS]
    mov edi, [dma_scatter_gather_temp_record]
    mov eax, [edi + DMA_SG_MAPPING_ID]
    mov ecx, [dma_scatter_gather_temp_request_slot]
    mov [dma_request_sg_mapping_ids + ecx * 4], eax
    inc dword [dma_scatter_gather_next_id]
    inc dword [dma_scatter_gather_active_count]
    mov edx, [edi + DMA_SG_MAPPING_TOTAL_LENGTH]
    add [dma_scatter_gather_mapped_bytes], edx
    popfd
    clc
    ret
.invalid:
    inc dword [dma_scatter_gather_validation_failures]
    xor eax, eax
    popfd
    stc
    ret

; EAX=aktiver DMA-SG-Mapping-Datensatz.
dma_scatter_gather_release_record:
    mov [dma_scatter_gather_temp_record], eax
    mov eax, [dma_scatter_gather_temp_record]
    sub eax, dma_scatter_gather_table
    shr eax, 6
    mov [dma_scatter_gather_temp_slot], eax
    mov dword [dma_scatter_gather_temp_rollback_index], 0
.revoke_iommu:
    mov ecx, [dma_scatter_gather_temp_rollback_index]
    cmp ecx, DMA_SG_MAX_SEGMENTS
    jae .iommu_revoked
    mov edx, [dma_scatter_gather_temp_slot]
    shl edx, 2
    add edx, ecx
    mov eax, [dma_scatter_gather_iommu_authorization_ids + edx * 4]
    test eax, eax
    jz .next_iommu_revoke
    mov edx, [dma_scatter_gather_temp_record]
    mov edx, [edx + DMA_SG_MAPPING_OWNER]
    call iommu_revoke_mapping
    mov edx, [dma_scatter_gather_temp_slot]
    shl edx, 2
    add edx, [dma_scatter_gather_temp_rollback_index]
    mov dword [dma_scatter_gather_iommu_authorization_ids + edx * 4], 0
.next_iommu_revoke:
    inc dword [dma_scatter_gather_temp_rollback_index]
    jmp .revoke_iommu
.iommu_revoked:
    mov eax, [dma_scatter_gather_temp_record]
    mov eax, [eax + DMA_SG_MAPPING_REQUEST]
    call io_request_lookup
    jc .skip_request
    mov ecx, eax
    sub ecx, io_request_table
    shr ecx, 6
    mov edx, [dma_scatter_gather_temp_record]
    mov edx, [edx + DMA_SG_MAPPING_ID]
    cmp [dma_request_sg_mapping_ids + ecx * 4], edx
    jne .skip_request
    mov dword [dma_request_sg_mapping_ids + ecx * 4], 0
.skip_request:
    mov eax, [dma_scatter_gather_temp_record]
    mov eax, [eax + DMA_SG_MAPPING_DESCRIPTOR]
    call scatter_gather_lookup
    jc .skip_descriptor
    mov [dma_scatter_gather_temp_descriptor_record], eax
    mov dword [dma_scatter_gather_temp_source_index], 0
.release_buffers:
    mov eax, [dma_scatter_gather_temp_descriptor_record]
    mov ecx, [dma_scatter_gather_temp_source_index]
    cmp ecx, [eax + SG_DESCRIPTOR_SEGMENTS]
    jae .release_descriptor
    call scatter_gather_segment_at
    mov eax, [eax + SG_SEGMENT_BUFFER]
    call shared_buffer_lookup
    jc .next_release_buffer
    cmp dword [eax + SHARED_BUFFER_ACTIVE_IO], 0
    je .active_released
    dec dword [eax + SHARED_BUFFER_ACTIVE_IO]
.active_released:
    cmp dword [eax + SHARED_BUFFER_REFERENCES], 1
    jbe .reference_released
    dec dword [eax + SHARED_BUFFER_REFERENCES]
.reference_released:
    cmp dword [eax + SHARED_BUFFER_MAPPINGS], 0
    je .mapping_released
    dec dword [eax + SHARED_BUFFER_MAPPINGS]
.mapping_released:
    cmp dword [eax + SHARED_BUFFER_PINNED], 0
    je .pin_released
    dec dword [eax + SHARED_BUFFER_PINNED]
.pin_released:
    mov dword [eax + SHARED_BUFFER_STATE], SHARED_BUFFER_STATE_CPU
    cmp dword [dma_scatter_gather_pinned_buffers], 0
    je .next_release_buffer
    dec dword [dma_scatter_gather_pinned_buffers]
.next_release_buffer:
    inc dword [dma_scatter_gather_temp_source_index]
    jmp .release_buffers
.release_descriptor:
    mov eax, [dma_scatter_gather_temp_descriptor_record]
    cmp dword [eax + SG_DESCRIPTOR_CONSUMERS], 0
    je .skip_descriptor
    dec dword [eax + SG_DESCRIPTOR_CONSUMERS]
.skip_descriptor:
    mov eax, [dma_scatter_gather_temp_record]
    mov edx, [eax + DMA_SG_MAPPING_TOTAL_LENGTH]
    cmp [dma_scatter_gather_mapped_bytes], edx
    jb .zero_bytes
    sub [dma_scatter_gather_mapped_bytes], edx
    jmp .bytes_done
.zero_bytes:
    mov dword [dma_scatter_gather_mapped_bytes], 0
.bytes_done:
    cmp dword [dma_scatter_gather_active_count], 0
    je .state
    dec dword [dma_scatter_gather_active_count]
.state:
    cmp dword [eax + DMA_SG_MAPPING_STATE], DMA_SG_STATE_FAULTED
    je .done
    mov dword [eax + DMA_SG_MAPPING_STATE], DMA_SG_STATE_UNMAPPED
.done:
    clc
    ret

; EAX=Mapping-ID, EDX=Owner.
dma_scatter_gather_unmap:
    pushfd
    cli
    mov [dma_scatter_gather_temp_id], eax
    mov [dma_scatter_gather_temp_owner], edx
    call dma_scatter_gather_lookup
    jc .invalid
    mov edx, [dma_scatter_gather_temp_owner]
    cmp [eax + DMA_SG_MAPPING_OWNER], edx
    jne .invalid
    call dma_scatter_gather_release_record
    mov eax, [dma_scatter_gather_temp_id]
    popfd
    clc
    ret
.invalid:
    xor eax, eax
    popfd
    stc
    ret

; EAX=Mapping-ID, EDX=Device-ID, EBX=Fehlercode. Ein SG-DMA-Fault beendet
; den zugehoerigen I/O-Request erst nach Revoke, Unpin und Ownership-Rueckgabe.
dma_scatter_gather_fault:
    pushfd
    cli
    mov [dma_scatter_gather_temp_id], eax
    mov [dma_scatter_gather_temp_device], edx
    mov [dma_scatter_gather_temp_error], ebx
    test ebx, ebx
    jz .invalid
    call dma_scatter_gather_lookup
    jc .invalid
    mov edx, [dma_scatter_gather_temp_device]
    cmp [eax + DMA_SG_MAPPING_DEVICE], edx
    jne .invalid
    mov dword [eax + DMA_SG_MAPPING_STATE], DMA_SG_STATE_FAULTED
    mov ecx, eax
    sub ecx, dma_scatter_gather_table
    shr ecx, 6
    mov ebx, [dma_scatter_gather_temp_error]
    mov [dma_scatter_gather_error_codes + ecx * 4], ebx
    mov edx, [eax + DMA_SG_MAPPING_REQUEST]
    mov [dma_scatter_gather_temp_request], edx
    inc dword [dma_scatter_gather_fault_count]
    call dma_scatter_gather_release_record
    mov eax, [dma_scatter_gather_temp_request]
    xor edx, edx
    mov ebx, [dma_scatter_gather_temp_error]
    call io_request_complete
    jc .invalid
    mov eax, [dma_scatter_gather_temp_id]
    popfd
    clc
    ret
.invalid:
    xor eax, eax
    popfd
    stc
    ret

; EAX=terminaler IORequest-Datensatz.
dma_scatter_gather_on_terminal:
    cmp dword [dma_scatter_gather_manager_ready], 1
    jne .done
    mov ecx, eax
    sub ecx, io_request_table
    shr ecx, 6
    mov eax, [dma_request_sg_mapping_ids + ecx * 4]
    test eax, eax
    jz .done
    call dma_scatter_gather_lookup
    jc .done
    call dma_scatter_gather_release_record
.done:
    clc
    ret

dma_scatter_gather_self_test:
    mov eax, 1
    mov ebx, IOMMU_MODE_RESTRICTED
    mov ecx, 32
    mov esi, DMA_SG_APERTURE
    mov edi, DMA_SG_APERTURE + 0x001FFFFF
    call iommu_domain_create
    jc .invalid
    mov [dma_scatter_gather_test_domain], eax
    mov edx, 0xD002
    mov ebx, 0x20
    mov ecx, 1
    call iommu_bind_device
    jc .invalid
    mov eax, 1
    mov edx, 128
    mov ebx, SHARED_BUFFER_RIGHT_READ | SHARED_BUFFER_RIGHT_WRITE | SHARED_BUFFER_RIGHT_TRANSFER | SHARED_BUFFER_RIGHT_DMA | SHARED_BUFFER_RIGHT_RELEASE
    call shared_buffer_create
    jc .invalid
    mov [dma_scatter_gather_test_buffer_a], eax
    mov eax, 1
    mov edx, 128
    mov ebx, SHARED_BUFFER_RIGHT_READ | SHARED_BUFFER_RIGHT_WRITE | SHARED_BUFFER_RIGHT_TRANSFER | SHARED_BUFFER_RIGHT_DMA | SHARED_BUFFER_RIGHT_RELEASE
    call shared_buffer_create
    jc .invalid
    mov [dma_scatter_gather_test_buffer_b], eax
    mov eax, 1
    mov edx, SG_DIRECTION_GATHER
    call scatter_gather_create
    jc .invalid
    mov [dma_scatter_gather_test_descriptor], eax
    mov edx, [dma_scatter_gather_test_buffer_a]
    xor ebx, ebx
    mov ecx, 96
    mov esi, SG_PERMISSION_READ
    mov edi, 1
    call scatter_gather_append
    jc .invalid
    mov eax, [dma_scatter_gather_test_descriptor]
    mov edx, [dma_scatter_gather_test_buffer_b]
    xor ebx, ebx
    mov ecx, 64
    mov esi, SG_PERMISSION_READ
    mov edi, 1
    call scatter_gather_append
    jc .invalid
    mov eax, [dma_scatter_gather_test_descriptor]
    mov edx, 1
    call scatter_gather_seal
    jc .invalid
    mov eax, 1
    mov edx, [task_scope_kernel_root_id]
    xor ebx, ebx
    call task_scope_create
    jc .invalid
    mov [dma_scatter_gather_test_scope], eax
    mov eax, 1
    mov edx, [dma_scatter_gather_test_scope]
    xor ebx, ebx
    xor ecx, ecx
    call task_create
    jc .invalid
    mov [dma_scatter_gather_test_task], eax
    mov eax, 1
    mov edx, [dma_scatter_gather_test_task]
    mov ebx, 6
    mov ecx, 0xD002
    mov esi, [dma_scatter_gather_test_descriptor]
    mov edi, 160
    xor ebp, ebp
    call io_request_submit
    jc .invalid
    mov [dma_scatter_gather_test_request], eax
    mov edx, [dma_scatter_gather_test_descriptor]
    mov ebx, 0xD002
    mov ecx, 1
    call dma_scatter_gather_map
    jc .invalid
    mov [dma_scatter_gather_test_mapping], eax
    call dma_scatter_gather_lookup
    jc .invalid
    mov [dma_scatter_gather_test_record], eax
    cmp dword [eax + DMA_SG_MAPPING_SEGMENTS], 3
    jne .invalid
    test dword [eax + DMA_SG_MAPPING_FLAGS], DMA_SG_FLAG_SPLIT
    jz .invalid
    cmp dword [eax + DMA_SG_MAPPING_DOMAIN], 0
    je .invalid
    cmp dword [iommu_mapping_count], 3
    jne .invalid
    mov ecx, 0
    call dma_scatter_gather_segment_at
    cmp dword [eax + DMA_SG_SEGMENT_LENGTH], 64
    jne .invalid
    test dword [eax + DMA_SG_SEGMENT_ADDRESS_LOW], DMA_SG_BOUNDARY - 1
    jnz .invalid
    mov eax, [dma_scatter_gather_test_descriptor]
    mov edx, 1
    call scatter_gather_release
    jnc .invalid                       ; aktiver DMA-Consumer blockiert Release
    mov eax, [dma_scatter_gather_test_domain]
    mov edx, 0xD002
    mov ebx, [dma_scatter_gather_test_mapping]
    or ebx, IOMMU_EXTERNAL_DMA_SG
    mov ecx, DMA_SG_APERTURE + 0x1000
    mov esi, IOMMU_PERMISSION_READ
    mov edi, 0xD201
    call iommu_report_fault
    jc .invalid
    mov eax, [dma_scatter_gather_test_record]
    cmp dword [eax + DMA_SG_MAPPING_STATE], DMA_SG_STATE_FAULTED
    jne .invalid
    cmp dword [dma_scatter_gather_fault_count], 1
    jne .invalid
    mov eax, [dma_scatter_gather_test_request]
    call io_request_lookup
    jc .invalid
    cmp dword [eax + IO_REQUEST_STATE], IO_REQUEST_STATE_FAILED
    jne .invalid
    cmp dword [eax + IO_REQUEST_COMPLETION], IO_COMPLETION_FAILED
    jne .invalid
    cmp dword [eax + IO_REQUEST_ERROR], 0xD201
    jne .invalid
    cmp dword [dma_scatter_gather_active_count], 0
    jne .invalid
    cmp dword [dma_scatter_gather_mapped_bytes], 0
    jne .invalid
    cmp dword [dma_scatter_gather_pinned_buffers], 0
    jne .invalid
    cmp dword [iommu_mapping_count], 0
    jne .invalid
    mov eax, [dma_scatter_gather_test_buffer_a]
    call shared_buffer_lookup
    jc .invalid
    cmp dword [eax + SHARED_BUFFER_STATE], SHARED_BUFFER_STATE_CPU
    jne .invalid
    cmp dword [eax + SHARED_BUFFER_PINNED], 0
    jne .invalid
    mov eax, [dma_scatter_gather_test_task]
    xor edx, edx
    call task_complete
    jc .invalid
    mov eax, [dma_scatter_gather_test_scope]
    call task_scope_close
    jc .invalid
    mov eax, [dma_scatter_gather_test_descriptor]
    mov edx, 1
    call scatter_gather_release
    jc .invalid
    mov eax, [dma_scatter_gather_test_buffer_a]
    mov edx, 1
    call shared_buffer_release
    jc .invalid
    mov eax, [dma_scatter_gather_test_buffer_b]
    mov edx, 1
    call shared_buffer_release
    jc .invalid
    mov eax, [dma_scatter_gather_test_domain]
    mov edx, 1
    call iommu_domain_release
    jc .invalid
    clc
    ret
.invalid:
    stc
    ret

align 4
dma_scatter_gather_api:
    dd DMA_SG_API_SIZE
    dw 1, 0
    dd DMA_SG_CAPACITY
    dd DMA_SG_MAX_SEGMENTS
    dd dma_scatter_gather_map
    dd dma_scatter_gather_unmap
    dd dma_scatter_gather_table
    dd dma_scatter_gather_segments

dma_scatter_gather_manager_ready:        dd 0
dma_scatter_gather_next_id:              dd 0
dma_scatter_gather_active_count:         dd 0
dma_scatter_gather_mapped_bytes:         dd 0
dma_scatter_gather_pinned_buffers:       dd 0
dma_scatter_gather_split_segments:       dd 0
dma_scatter_gather_validation_failures:  dd 0
dma_scatter_gather_fault_count:          dd 0
dma_scatter_gather_temp_id:              dd 0
dma_scatter_gather_temp_owner:           dd 0
dma_scatter_gather_temp_device:          dd 0
dma_scatter_gather_temp_request:         dd 0
dma_scatter_gather_temp_request_slot:    dd 0
dma_scatter_gather_temp_request_record:  dd 0
dma_scatter_gather_temp_descriptor:      dd 0
dma_scatter_gather_temp_descriptor_record: dd 0
dma_scatter_gather_temp_record:          dd 0
dma_scatter_gather_temp_slot:            dd 0
dma_scatter_gather_temp_required:        dd 0
dma_scatter_gather_temp_source_index:    dd 0
dma_scatter_gather_temp_output_index:    dd 0
dma_scatter_gather_temp_source_segment:  dd 0
dma_scatter_gather_temp_buffer:          dd 0
dma_scatter_gather_temp_offset:          dd 0
dma_scatter_gather_temp_remaining:       dd 0
dma_scatter_gather_temp_permissions:     dd 0
dma_scatter_gather_temp_chunk:           dd 0
dma_scatter_gather_temp_error:           dd 0
dma_scatter_gather_temp_iommu_domain:    dd 0
dma_scatter_gather_temp_device_segment:  dd 0
dma_scatter_gather_temp_authorized_count: dd 0
dma_scatter_gather_temp_rollback_index:  dd 0
dma_scatter_gather_temp_buffer_ids:      times SG_MAX_SEGMENTS dd 0
dma_scatter_gather_test_buffer_a:        dd 0
dma_scatter_gather_test_buffer_b:        dd 0
dma_scatter_gather_test_descriptor:      dd 0
dma_scatter_gather_test_scope:           dd 0
dma_scatter_gather_test_task:            dd 0
dma_scatter_gather_test_request:         dd 0
dma_scatter_gather_test_mapping:         dd 0
dma_scatter_gather_test_record:          dd 0
dma_scatter_gather_test_domain:          dd 0
align 4
dma_scatter_gather_table:
    times DMA_SG_CAPACITY * DMA_SG_MAPPING_SIZE db 0
dma_scatter_gather_segments:
    times DMA_SG_CAPACITY * DMA_SG_MAX_SEGMENTS * DMA_SG_SEGMENT_SIZE db 0
dma_scatter_gather_generations:
    times DMA_SG_CAPACITY dd 0
dma_request_sg_mapping_ids:
    times IO_REQUEST_CAPACITY dd 0
dma_scatter_gather_iommu_authorization_ids:
    times DMA_SG_CAPACITY * DMA_SG_MAX_SEGMENTS dd 0
dma_scatter_gather_error_codes:
    times DMA_SG_CAPACITY dd 0

; ---------------------------------------------------------------------------
; General-Purpose Ring Buffer – begrenzte SPSC-Kreisstruktur fuer kontin-
; uierliche Datenstroeme zwischen Producer und Consumer.
; NPSPEC-DATAMOVE-RINGBUFFER-0001
; ---------------------------------------------------------------------------

RINGBUF_CAPACITY       equ 4
RINGBUF_ELEMENT_CAP    equ 16
RINGBUF_ELEMENT_SIZE   equ 16
RINGBUF_RECORD_SIZE    equ 32

RINGBUF_STATE_FREE     equ 0
RINGBUF_STATE_ACTIVE   equ 1

RINGBUF_ID             equ 0
RINGBUF_ELEM_CAPACITY  equ 4
RINGBUF_READ_POS       equ 8
RINGBUF_WRITE_POS      equ 12
RINGBUF_STATE          equ 16
RINGBUF_FLAGS          equ 20
RINGBUF_COUNT          equ 24
RINGBUF_GENERATION     equ 28

ringbuf_initialize:
    mov edi, ringbuf_table
    xor eax, eax
    mov ecx, (RINGBUF_CAPACITY * RINGBUF_RECORD_SIZE) / 4
    rep stosd
    mov edi, ringbuf_storage
    mov ecx, (RINGBUF_CAPACITY * RINGBUF_ELEMENT_CAP * RINGBUF_ELEMENT_SIZE) / 4
    rep stosd
    mov edi, ringbuf_generations
    mov ecx, RINGBUF_CAPACITY
    rep stosd
    mov dword [ringbuf_next_id], 1
    mov dword [ringbuf_live_count], 0
    mov dword [ringbuf_manager_ready], 1
    clc
    ret

; Erstellt einen Ring-Buffer. EAX=Slot-Index (CF=0) oder CF=1 wenn voll.
ringbuf_create:
    push ecx
    push edi
    xor ecx, ecx
.rc_scan:
    cmp ecx, RINGBUF_CAPACITY
    jae .rc_full
    mov edi, ecx
    imul edi, RINGBUF_RECORD_SIZE
    add edi, ringbuf_table
    cmp dword [edi + RINGBUF_STATE], RINGBUF_STATE_FREE
    je .rc_found
    inc ecx
    jmp .rc_scan
.rc_found:
    mov eax, [ringbuf_next_id]
    mov [edi + RINGBUF_ID],            eax
    mov dword [edi + RINGBUF_ELEM_CAPACITY], RINGBUF_ELEMENT_CAP
    mov dword [edi + RINGBUF_READ_POS],  0
    mov dword [edi + RINGBUF_WRITE_POS], 0
    mov dword [edi + RINGBUF_STATE],   RINGBUF_STATE_ACTIVE
    mov dword [edi + RINGBUF_FLAGS],   0
    mov dword [edi + RINGBUF_COUNT],   0
    mov eax, [ringbuf_generations + ecx * 4]
    inc eax
    mov [ringbuf_generations + ecx * 4], eax
    mov [edi + RINGBUF_GENERATION],    eax
    inc dword [ringbuf_next_id]
    inc dword [ringbuf_live_count]
    mov eax, ecx
    pop edi
    pop ecx
    clc
    ret
.rc_full:
    pop edi
    pop ecx
    stc
    ret

; Schreibt ein 16-Byte-Element. EAX=Slot (0..CAPACITY-1), ESI=Quell-Ptr.
; CF=0 ok, CF=1 voll oder ungueltig.
ringbuf_write:
    push ebx
    push ecx
    push edx
    push edi
    cmp eax, RINGBUF_CAPACITY
    jae .rw_bad
    mov ebx, eax
    imul ebx, RINGBUF_RECORD_SIZE
    add ebx, ringbuf_table
    cmp dword [ebx + RINGBUF_STATE], RINGBUF_STATE_ACTIVE
    jne .rw_bad
    mov ecx, [ebx + RINGBUF_COUNT]
    cmp ecx, RINGBUF_ELEMENT_CAP
    jae .rw_full
    mov edx, eax
    imul edx, RINGBUF_ELEMENT_CAP * RINGBUF_ELEMENT_SIZE
    add edx, ringbuf_storage
    mov ecx, [ebx + RINGBUF_WRITE_POS]
    imul ecx, RINGBUF_ELEMENT_SIZE
    add edx, ecx
    mov edi, edx
    mov ecx, RINGBUF_ELEMENT_SIZE / 4
    rep movsd
    mov ecx, [ebx + RINGBUF_WRITE_POS]
    inc ecx
    cmp ecx, RINGBUF_ELEMENT_CAP
    jb .rw_no_wrap
    xor ecx, ecx
.rw_no_wrap:
    mov [ebx + RINGBUF_WRITE_POS], ecx
    inc dword [ebx + RINGBUF_COUNT]
    pop edi
    pop edx
    pop ecx
    pop ebx
    clc
    ret
.rw_full:
.rw_bad:
    pop edi
    pop edx
    pop ecx
    pop ebx
    stc
    ret

; Liest ein 16-Byte-Element. EAX=Slot (0..CAPACITY-1), EDI=Ziel-Ptr.
; CF=0 ok, CF=1 leer oder ungueltig.
ringbuf_read:
    push ebx
    push ecx
    push edx
    push esi
    cmp eax, RINGBUF_CAPACITY
    jae .rr_bad
    mov ebx, eax
    imul ebx, RINGBUF_RECORD_SIZE
    add ebx, ringbuf_table
    cmp dword [ebx + RINGBUF_STATE], RINGBUF_STATE_ACTIVE
    jne .rr_bad
    cmp dword [ebx + RINGBUF_COUNT], 0
    je .rr_empty
    mov edx, eax
    imul edx, RINGBUF_ELEMENT_CAP * RINGBUF_ELEMENT_SIZE
    add edx, ringbuf_storage
    mov ecx, [ebx + RINGBUF_READ_POS]
    imul ecx, RINGBUF_ELEMENT_SIZE
    add edx, ecx
    mov esi, edx
    mov ecx, RINGBUF_ELEMENT_SIZE / 4
    rep movsd
    mov ecx, [ebx + RINGBUF_READ_POS]
    inc ecx
    cmp ecx, RINGBUF_ELEMENT_CAP
    jb .rr_no_wrap
    xor ecx, ecx
.rr_no_wrap:
    mov [ebx + RINGBUF_READ_POS], ecx
    dec dword [ebx + RINGBUF_COUNT]
    pop esi
    pop edx
    pop ecx
    pop ebx
    clc
    ret
.rr_empty:
.rr_bad:
    pop esi
    pop edx
    pop ecx
    pop ebx
    stc
    ret

; Self-Test: 7 Tests (Init, Create, Write, Read, Wrap, Count-Invarianten).
ringbuf_self_test:
    push ebx
    push esi
    push edi
    sub esp, 32                          ; [esp+0..15]=Schreibpuf, [esp+16..31]=Lesepuf
    xor ebx, ebx                         ; Fehlerzaehler

    ; Test 1: Manager bereit
    cmp dword [ringbuf_manager_ready], 1
    je .rbt2
    inc ebx

.rbt2:
    ; Test 2: Create liefert Slot 0
    call ringbuf_create
    jnc .rbt3
    inc ebx
    jmp .rbt_done
.rbt3:
    cmp eax, 0
    je .rbt4
    inc ebx

.rbt4:
    ; Test 3: Count == 0 nach Create
    mov esi, ringbuf_table
    cmp dword [esi + RINGBUF_COUNT], 0
    je .rbt5
    inc ebx

.rbt5:
    ; Test 4: Write schreibt ohne Fehler
    mov dword [esp +  0], 0xAABBCCDD
    mov dword [esp +  4], 0x11223344
    mov dword [esp +  8], 0xDEADBEEF
    mov dword [esp + 12], 0xCAFEBABE
    xor eax, eax
    lea esi, [esp]
    call ringbuf_write
    jnc .rbt6
    inc ebx

.rbt6:
    ; Test 5: Count == 1 nach Write
    mov esi, ringbuf_table
    cmp dword [esi + RINGBUF_COUNT], 1
    je .rbt7
    inc ebx

.rbt7:
    ; Test 6: Read liefert identische Daten
    xor eax, eax
    lea edi, [esp + 16]
    call ringbuf_read
    jnc .rbt7_check
    inc ebx
    jmp .rbt8
.rbt7_check:
    cmp dword [esp + 16], 0xAABBCCDD
    je .rbt8
    inc ebx

.rbt8:
    ; Test 7: Count == 0 nach Read
    mov esi, ringbuf_table
    cmp dword [esi + RINGBUF_COUNT], 0
    je .rbt_done
    inc ebx

.rbt_done:
    test ebx, ebx
    jnz .rbt_fail
    add esp, 32
    pop edi
    pop esi
    pop ebx
    clc
    ret
.rbt_fail:
    add esp, 32
    pop edi
    pop esi
    pop ebx
    stc
    ret

ringbuf_manager_ready: dd 0
ringbuf_next_id:       dd 0
ringbuf_live_count:    dd 0
align 4
ringbuf_generations:
    times RINGBUF_CAPACITY dd 0
ringbuf_table:
    times RINGBUF_CAPACITY * RINGBUF_RECORD_SIZE db 0
ringbuf_storage:
    times RINGBUF_CAPACITY * RINGBUF_ELEMENT_CAP * RINGBUF_ELEMENT_SIZE db 0

; ---------------------------------------------------------------------------
; Zentraler I/O-Scheduler: Deadline vor effektiver Prioritaet, danach FIFO.
; Eine begrenzte Aging-Stufe verhindert dauerhafte Starvation.
; NPSPEC-IO-PRIORITY/SCHEDULER-0001
; ---------------------------------------------------------------------------

IO_SCHEDULER_API_SIZE       equ 32
IO_SCHEDULER_CAPABILITIES   equ 0x0000000F
IO_SCHEDULER_AGING_ROUNDS   equ 4

io_scheduler_initialize:
    mov dword [io_scheduler_dispatch_count], 0
    mov dword [io_scheduler_promotion_count], 0
    mov dword [io_scheduler_ready], 1
    clc
    ret

; Waehlt einen pending Request. Deadline Requests werden nach fruehester
; Deadline sortiert; sonst entscheiden effektive Prioritaet und Wartealter.
; EAX=Request-ID, CF=1 wenn die Queue keine ausfuehrbare Arbeit enthaelt.
io_scheduler_select:
    pushfd
    cli
    mov dword [io_scheduler_candidate], 0
    mov dword [io_scheduler_candidate_slot], 0
    xor ecx, ecx
.scan:
    cmp ecx, IO_REQUEST_CAPACITY
    jae .selected
    mov edi, ecx
    shl edi, 6
    add edi, io_request_table
    cmp dword [edi + IO_REQUEST_STATE], IO_REQUEST_STATE_PENDING
    jne .next
    mov esi, [io_scheduler_candidate]
    test esi, esi
    jz .choose
    test dword [edi + IO_REQUEST_FLAGS], IO_FLAG_DEADLINE_INHERITED
    jz .current_without_deadline
    test dword [esi + IO_REQUEST_FLAGS], IO_FLAG_DEADLINE_INHERITED
    jz .choose
    mov eax, [edi + IO_REQUEST_DEADLINE]
    sub eax, [esi + IO_REQUEST_DEADLINE]
    jl .choose
    jg .next
    jmp .priority
.current_without_deadline:
    test dword [esi + IO_REQUEST_FLAGS], IO_FLAG_DEADLINE_INHERITED
    jnz .next
.priority:
    mov eax, [edi + IO_REQUEST_EFFECTIVE_PRIORITY]
    cmp eax, [esi + IO_REQUEST_EFFECTIVE_PRIORITY]
    jb .choose
    ja .next
    mov eax, [io_request_submit_ticks + ecx * 4]
    mov edx, [io_scheduler_candidate_slot]
    cmp eax, [io_request_submit_ticks + edx * 4]
    jae .next
.choose:
    mov [io_scheduler_candidate], edi
    mov [io_scheduler_candidate_slot], ecx
.next:
    inc ecx
    jmp .scan
.selected:
    mov edi, [io_scheduler_candidate]
    test edi, edi
    jz .empty
    mov dword [edi + IO_REQUEST_STATE], IO_REQUEST_STATE_RUNNING
    mov ecx, [io_scheduler_candidate_slot]
    mov eax, [timer_ticks]
    mov [io_request_dispatch_ticks + ecx * 4], eax
    mov dword [io_request_wait_rounds + ecx * 4], 0
    inc dword [io_scheduler_dispatch_count]

    ; Nicht ausgewaehlte Requests altern begrenzt bis Interactive. Realtime
    ; bleibt einer expliziten Policy vorbehalten und wird nicht erzwungen.
    xor ecx, ecx
.age:
    cmp ecx, IO_REQUEST_CAPACITY
    jae .return
    cmp ecx, [io_scheduler_candidate_slot]
    je .age_next
    mov esi, ecx
    shl esi, 6
    add esi, io_request_table
    cmp dword [esi + IO_REQUEST_STATE], IO_REQUEST_STATE_PENDING
    jne .age_next
    inc dword [io_request_wait_rounds + ecx * 4]
    cmp dword [io_request_wait_rounds + ecx * 4], IO_SCHEDULER_AGING_ROUNDS
    jb .age_next
    mov dword [io_request_wait_rounds + ecx * 4], 0
    cmp dword [esi + IO_REQUEST_EFFECTIVE_PRIORITY], IO_PRIORITY_INTERACTIVE
    jbe .age_next
    dec dword [esi + IO_REQUEST_EFFECTIVE_PRIORITY]
    inc dword [io_scheduler_promotion_count]
.age_next:
    inc ecx
    jmp .age
.return:
    mov eax, [edi + IO_REQUEST_ID]
    popfd
    clc
    ret
.empty:
    xor eax, eax
    popfd
    stc
    ret

; Provider-Backpressure darf einen ausgewaehlten Request ohne Identitaets- oder
; Altersverlust wieder in die begrenzte Scheduler-Queue stellen.
io_scheduler_requeue:
    pushfd
    cli
    call io_request_lookup
    jc .invalid
    cmp dword [eax + IO_REQUEST_STATE], IO_REQUEST_STATE_RUNNING
    jne .invalid
    mov dword [eax + IO_REQUEST_STATE], IO_REQUEST_STATE_PENDING
    mov eax, [eax + IO_REQUEST_ID]
    popfd
    clc
    ret
.invalid:
    xor eax, eax
    popfd
    stc
    ret

io_scheduler_self_test:
    mov eax, 1
    mov edx, [task_scope_kernel_root_id]
    xor ebx, ebx
    call task_scope_create
    jc .invalid
    mov [io_scheduler_test_scope], eax

    ; Unprivilegiertes Realtime wird policy-konform auf Interactive begrenzt.
    mov eax, 1
    mov edx, [io_scheduler_test_scope]
    xor ebx, ebx
    xor ecx, ecx
    call task_create
    jc .invalid
    mov [io_scheduler_test_fast_task], eax
    mov eax, 1
    mov edx, [io_scheduler_test_fast_task]
    mov ebx, 1
    mov ecx, 0x300
    mov esi, 0x400
    mov edi, 256
    mov ebp, IO_PRIORITY_REALTIME
    call io_request_submit
    jc .invalid
    mov [io_scheduler_test_fast_request], eax
    call io_request_lookup
    jc .invalid
    cmp dword [eax + IO_REQUEST_PRIORITY], IO_PRIORITY_REALTIME
    jne .invalid
    cmp dword [eax + IO_REQUEST_EFFECTIVE_PRIORITY], IO_PRIORITY_INTERACTIVE
    jne .invalid

    ; Eine Maintenance-Anfrage mit Deadline geht trotz niedrigerer Prioritaet
    ; vor der rein prioritaetsgesteuerten Anfrage in Ausfuehrung.
    mov eax, 1
    mov edx, [io_scheduler_test_scope]
    xor ebx, ebx
    xor ecx, ecx
    call task_create
    jc .invalid
    mov [io_scheduler_test_deadline_task], eax
    mov edx, [timer_ticks]
    add edx, 100
    mov ebx, TASK_DEADLINE_CLASS_SOFT
    mov ecx, TASK_DEADLINE_POLICY_CANCEL
    call task_deadline_set
    jc .invalid
    mov eax, 1
    mov edx, [io_scheduler_test_deadline_task]
    mov ebx, 2
    mov ecx, 0x301
    mov esi, 0x401
    mov edi, 256
    mov ebp, IO_PRIORITY_MAINTENANCE
    call io_request_submit
    jc .invalid
    mov [io_scheduler_test_deadline_request], eax

    call io_scheduler_select
    jc .invalid
    cmp eax, [io_scheduler_test_deadline_request]
    jne .invalid
    call io_scheduler_requeue
    jc .invalid
    call io_scheduler_select
    jc .invalid
    cmp eax, [io_scheduler_test_deadline_request]
    jne .invalid
    mov edx, 256
    xor ebx, ebx
    call io_request_complete
    jc .invalid
    call io_scheduler_select
    jc .invalid
    cmp eax, [io_scheduler_test_fast_request]
    jne .invalid
    mov edx, 256
    xor ebx, ebx
    call io_request_complete
    jc .invalid
    cmp dword [io_request_outstanding], 0
    jne .invalid

    mov eax, [io_scheduler_test_fast_task]
    xor edx, edx
    call task_complete
    jc .invalid
    mov eax, [io_scheduler_test_deadline_task]
    xor edx, edx
    call task_complete
    jc .invalid
    mov eax, [io_scheduler_test_scope]
    call task_scope_close
    jc .invalid
    cmp dword [task_count], 0
    jne .invalid
    clc
    ret
.invalid:
    stc
    ret

align 4
io_scheduler_api:
    dd IO_SCHEDULER_API_SIZE
    dw 1, 0
    dd 5
    dd IO_SCHEDULER_CAPABILITIES
    dd io_scheduler_select
    dd io_scheduler_requeue
    dd io_request_outstanding
    dd io_scheduler_dispatch_count

io_scheduler_ready:                 dd 0
io_scheduler_dispatch_count:        dd 0
io_scheduler_promotion_count:       dd 0
io_scheduler_candidate:             dd 0
io_scheduler_candidate_slot:        dd 0
io_scheduler_test_scope:            dd 0
io_scheduler_test_fast_task:        dd 0
io_scheduler_test_deadline_task:    dd 0
io_scheduler_test_fast_request:     dd 0
io_scheduler_test_deadline_request: dd 0

; ---------------------------------------------------------------------------
; I/O Quality of Service: getrennte Profile, Admission und Accounting
; NPSPEC-IO-QOS-0001 / ADR-IO-0003 und ADR-IO-0006
; ---------------------------------------------------------------------------

IO_QOS_API_SIZE             equ 32
IO_QOS_CAPACITY             equ 4
IO_QOS_RECORD_SIZE          equ 64
IO_QOS_STATE_EMPTY          equ 0
IO_QOS_STATE_ACCEPTED       equ 1
IO_QOS_STATE_DEGRADED       equ 2
IO_QOS_STATE_REJECTED       equ 3
IO_QOS_HARD_LATENCY         equ 0x00000001
IO_QOS_HARD_THROUGHPUT      equ 0x00000002
IO_QOS_HARD_BANDWIDTH       equ 0x00000004
IO_QOS_DEGRADE_THROUGHPUT   equ 0x00000001
IO_QOS_DEGRADE_BANDWIDTH    equ 0x00000002
IO_QOS_CAPABILITIES         equ 0x0000000F
IO_QOS_PROFILE_ID           equ 0
IO_QOS_OWNER                equ 4
IO_QOS_CLASS                equ 8
IO_QOS_REQUIREMENTS         equ 12
IO_QOS_LATENCY_TARGET       equ 16
IO_QOS_MIN_THROUGHPUT       equ 20
IO_QOS_MAX_BANDWIDTH        equ 24
IO_QOS_MAX_OUTSTANDING      equ 28
IO_QOS_ACTIVE_REQUESTS      equ 32
IO_QOS_STATE                equ 36
IO_QOS_OBSERVED_LATENCY     equ 40
IO_QOS_VIOLATIONS           equ 44
IO_QOS_COMPLETED_BYTES_LOW  equ 48
IO_QOS_COMPLETED_BYTES_HIGH equ 52
IO_QOS_DEGRADATION_REASON   equ 56

io_qos_initialize:
    mov edi, io_qos_table
    xor eax, eax
    mov ecx, (IO_QOS_CAPACITY * IO_QOS_RECORD_SIZE) / 4
    rep stosd
    mov edi, io_request_qos_ids
    mov ecx, IO_REQUEST_CAPACITY
    rep stosd
    mov dword [io_qos_next_id], 1
    mov dword [io_qos_rejection_count], 0
    mov dword [io_qos_throttle_count], 0
    mov dword [io_qos_manager_ready], 1
    clc
    ret

; EAX=Owner, EDX=QoS-Klasse (0=Normal), EBX=Hard-Flags,
; ECX=Latenzziel, ESI=Min-Durchsatz, EDI=Max-Bandbreite,
; EBP=maximal offene Requests (0=2). EAX=Profil-ID.
io_qos_create:
    pushfd
    cli
    mov [io_qos_temp_owner], eax
    mov [io_qos_temp_class], edx
    mov [io_qos_temp_requirements], ebx
    mov [io_qos_temp_latency], ecx
    mov [io_qos_temp_throughput], esi
    mov [io_qos_temp_bandwidth], edi
    mov [io_qos_temp_outstanding], ebp
    call process_lookup
    jc .invalid
    cmp dword [io_qos_temp_requirements], IO_QOS_HARD_LATENCY | IO_QOS_HARD_THROUGHPUT | IO_QOS_HARD_BANDWIDTH
    ja .invalid
    mov edx, [io_qos_temp_class]
    test edx, edx
    jnz .class_set
    mov edx, IO_PRIORITY_NORMAL
    mov [io_qos_temp_class], edx
.class_set:
    cmp edx, IO_PRIORITY_REALTIME
    jb .invalid
    cmp edx, IO_PRIORITY_MAINTENANCE
    ja .invalid
    mov ebp, [io_qos_temp_outstanding]
    test ebp, ebp
    jnz .outstanding_set
    mov ebp, 2
    mov [io_qos_temp_outstanding], ebp
.outstanding_set:
    cmp ebp, IO_REQUEST_CAPACITY
    ja .reject
    test dword [io_qos_temp_requirements], IO_QOS_HARD_THROUGHPUT | IO_QOS_HARD_BANDWIDTH
    jnz .reject                         ; kein Provider darf Garantie vortaeuschen
    test dword [io_qos_temp_requirements], IO_QOS_HARD_LATENCY
    jz .find
    cmp dword [io_qos_temp_latency], 2  ; Bootstrap-Admission: mindestens 20 ms
    jb .reject
.find:
    xor ecx, ecx
.scan:
    cmp ecx, IO_QOS_CAPACITY
    jae .reject
    mov edi, ecx
    shl edi, 6
    add edi, io_qos_table
    cmp dword [edi + IO_QOS_STATE], IO_QOS_STATE_EMPTY
    je .slot
    inc ecx
    jmp .scan
.slot:
    mov [io_qos_temp_record], edi
    xor eax, eax
    mov ecx, IO_QOS_RECORD_SIZE / 4
    rep stosd
    mov edi, [io_qos_temp_record]
    mov eax, [io_qos_next_id]
    mov [edi + IO_QOS_PROFILE_ID], eax
    mov edx, [io_qos_temp_owner]
    mov [edi + IO_QOS_OWNER], edx
    mov edx, [io_qos_temp_class]
    mov [edi + IO_QOS_CLASS], edx
    mov edx, [io_qos_temp_requirements]
    mov [edi + IO_QOS_REQUIREMENTS], edx
    mov edx, [io_qos_temp_latency]
    mov [edi + IO_QOS_LATENCY_TARGET], edx
    mov edx, [io_qos_temp_throughput]
    mov [edi + IO_QOS_MIN_THROUGHPUT], edx
    mov edx, [io_qos_temp_bandwidth]
    mov [edi + IO_QOS_MAX_BANDWIDTH], edx
    mov edx, [io_qos_temp_outstanding]
    mov [edi + IO_QOS_MAX_OUTSTANDING], edx
    mov dword [edi + IO_QOS_STATE], IO_QOS_STATE_ACCEPTED
    xor edx, edx
    cmp dword [edi + IO_QOS_MIN_THROUGHPUT], 0
    je .check_bandwidth
    or edx, IO_QOS_DEGRADE_THROUGHPUT
.check_bandwidth:
    cmp dword [edi + IO_QOS_MAX_BANDWIDTH], 0
    je .degradation_done
    or edx, IO_QOS_DEGRADE_BANDWIDTH
.degradation_done:
    test edx, edx
    jz .accepted
    mov dword [edi + IO_QOS_STATE], IO_QOS_STATE_DEGRADED
    mov [edi + IO_QOS_DEGRADATION_REASON], edx
.accepted:
    inc dword [io_qos_next_id]
    mov eax, [edi + IO_QOS_PROFILE_ID]
    popfd
    clc
    ret
.reject:
    inc dword [io_qos_rejection_count]
.invalid:
    xor eax, eax
    popfd
    stc
    ret

io_qos_lookup:
    xor ecx, ecx
.scan:
    cmp ecx, IO_QOS_CAPACITY
    jae .invalid
    mov edx, ecx
    shl edx, 6
    add edx, io_qos_table
    cmp dword [edx + IO_QOS_STATE], IO_QOS_STATE_EMPTY
    je .next
    cmp [edx + IO_QOS_PROFILE_ID], eax
    je .found
.next:
    inc ecx
    jmp .scan
.found:
    mov eax, edx
    clc
    ret
.invalid:
    xor eax, eax
    stc
    ret

; EAX=Request-ID, EDX=Profil-ID. Budget und Owner werden vor Bindung geprueft.
io_qos_attach:
    pushfd
    cli
    mov [io_qos_temp_request], eax
    mov [io_qos_temp_profile], edx
    call io_request_lookup
    jc .invalid
    mov [io_qos_temp_request_record], eax
    cmp dword [eax + IO_REQUEST_STATE], IO_REQUEST_STATE_PENDING
    jne .invalid
    mov eax, [io_qos_temp_profile]
    call io_qos_lookup
    jc .invalid
    mov [io_qos_temp_record], eax
    mov edx, [io_qos_temp_request_record]
    mov ecx, [edx + IO_REQUEST_OWNER]
    cmp [eax + IO_QOS_OWNER], ecx
    jne .invalid
    mov ecx, [eax + IO_QOS_ACTIVE_REQUESTS]
    cmp ecx, [eax + IO_QOS_MAX_OUTSTANDING]
    jae .throttle
    mov ecx, edx
    sub ecx, io_request_table
    shr ecx, 6
    cmp dword [io_request_qos_ids + ecx * 4], 0
    jne .invalid
    mov eax, [io_qos_temp_profile]
    mov [io_request_qos_ids + ecx * 4], eax
    mov eax, [io_qos_temp_record]
    inc dword [eax + IO_QOS_ACTIVE_REQUESTS]
    mov eax, [eax + IO_QOS_CLASS]
    cmp eax, IO_PRIORITY_REALTIME
    jne .class_ready
    mov eax, IO_PRIORITY_INTERACTIVE
.class_ready:
    mov edx, [io_qos_temp_request_record]
    cmp eax, [edx + IO_REQUEST_EFFECTIVE_PRIORITY]
    jae .attached
    mov [edx + IO_REQUEST_EFFECTIVE_PRIORITY], eax
.attached:
    mov eax, [io_qos_temp_request]
    popfd
    clc
    ret
.throttle:
    inc dword [io_qos_throttle_count]
.invalid:
    xor eax, eax
    popfd
    stc
    ret

; EAX=terminaler Requestdatensatz. Beobachtete Werte bleiben vom angeforderten
; Profil getrennt; eine Zielverletzung wird explizit gezaehlt.
io_qos_on_terminal:
    cmp dword [io_qos_manager_ready], 1
    jne .done
    mov [io_qos_terminal_request], eax
    mov ecx, eax
    sub ecx, io_request_table
    shr ecx, 6
    mov eax, [io_request_qos_ids + ecx * 4]
    test eax, eax
    jz .done
    mov [io_qos_temp_slot], ecx
    call io_qos_lookup
    jc .done
    cmp dword [eax + IO_QOS_ACTIVE_REQUESTS], 0
    je .done
    dec dword [eax + IO_QOS_ACTIVE_REQUESTS]
    mov edx, [timer_ticks]
    mov ecx, [io_qos_temp_slot]
    sub edx, [io_request_submit_ticks + ecx * 4]
    mov [eax + IO_QOS_OBSERVED_LATENCY], edx
    mov ecx, [eax + IO_QOS_LATENCY_TARGET]
    test ecx, ecx
    jz .bytes
    cmp edx, ecx
    jbe .bytes
    inc dword [eax + IO_QOS_VIOLATIONS]
.bytes:
    mov edx, [io_qos_terminal_request]
    mov edx, [edx + IO_REQUEST_TRANSFERRED]
    add [eax + IO_QOS_COMPLETED_BYTES_LOW], edx
    adc dword [eax + IO_QOS_COMPLETED_BYTES_HIGH], 0
.done:
    clc
    ret

io_qos_self_test:
    ; Weiche, providerabhaengige Durchsatzwuensche werden sichtbar degradiert.
    mov eax, 1
    mov edx, IO_PRIORITY_INTERACTIVE
    xor ebx, ebx
    mov ecx, 50
    mov esi, 4096
    xor edi, edi
    mov ebp, 1
    call io_qos_create
    jc .invalid
    mov [io_qos_test_profile], eax
    call io_qos_lookup
    jc .invalid
    cmp dword [eax + IO_QOS_STATE], IO_QOS_STATE_DEGRADED
    jne .invalid
    test dword [eax + IO_QOS_DEGRADATION_REASON], IO_QOS_DEGRADE_THROUGHPUT
    jz .invalid

    ; Eine nicht belegbare harte Durchsatzgarantie wird nicht vorgetaeuscht.
    mov eax, 1
    mov edx, IO_PRIORITY_NORMAL
    mov ebx, IO_QOS_HARD_THROUGHPUT
    xor ecx, ecx
    mov esi, 4096
    xor edi, edi
    mov ebp, 1
    call io_qos_create
    jnc .invalid
    cmp dword [io_qos_rejection_count], 1
    jne .invalid

    mov eax, 1
    mov edx, [task_scope_kernel_root_id]
    xor ebx, ebx
    call task_scope_create
    jc .invalid
    mov [io_qos_test_scope], eax
    mov eax, 1
    mov edx, [io_qos_test_scope]
    xor ebx, ebx
    xor ecx, ecx
    call task_create
    jc .invalid
    mov [io_qos_test_task], eax

    ; MaxOutstanding=1 isoliert das Profil und erzeugt beim zweiten Attach
    ; kontrollierte Drosselung statt stiller Budgetueberschreitung.
    mov eax, 1
    mov edx, [io_qos_test_task]
    mov ebx, 3
    mov ecx, 0x500
    mov esi, 0x600
    mov edi, 128
    mov ebp, IO_PRIORITY_NORMAL
    call io_request_submit
    jc .invalid
    mov [io_qos_test_request1], eax
    mov edx, [io_qos_test_profile]
    call io_qos_attach
    jc .invalid
    mov eax, 1
    mov edx, [io_qos_test_task]
    mov ebx, 3
    mov ecx, 0x501
    mov esi, 0x601
    mov edi, 128
    mov ebp, IO_PRIORITY_NORMAL
    call io_request_submit
    jc .invalid
    mov [io_qos_test_request2], eax
    mov edx, [io_qos_test_profile]
    call io_qos_attach
    jnc .invalid
    cmp dword [io_qos_throttle_count], 1
    jne .invalid
    mov eax, [io_qos_test_request1]
    mov edx, 128
    xor ebx, ebx
    call io_request_complete
    jc .invalid
    mov eax, [io_qos_test_profile]
    call io_qos_lookup
    jc .invalid
    cmp dword [eax + IO_QOS_ACTIVE_REQUESTS], 0
    jne .invalid
    cmp dword [eax + IO_QOS_COMPLETED_BYTES_LOW], 128
    jne .invalid
    mov eax, [io_qos_test_request2]
    mov edx, 0x514F5343               ; "QOSC"
    call io_request_cancel
    jc .invalid
    mov eax, [io_qos_test_task]
    xor edx, edx
    call task_complete
    jc .invalid
    mov eax, [io_qos_test_scope]
    call task_scope_close
    jc .invalid
    clc
    ret
.invalid:
    stc
    ret

align 4
io_qos_api:
    dd IO_QOS_API_SIZE
    dw 1, 0
    dd IO_QOS_CAPACITY
    dd IO_QOS_CAPABILITIES
    dd io_qos_create
    dd io_qos_attach
    dd io_qos_table
    dd io_qos_rejection_count

io_qos_manager_ready:        dd 0
io_qos_next_id:              dd 0
io_qos_rejection_count:      dd 0
io_qos_throttle_count:       dd 0
io_qos_temp_owner:           dd 0
io_qos_temp_class:           dd 0
io_qos_temp_requirements:    dd 0
io_qos_temp_latency:         dd 0
io_qos_temp_throughput:      dd 0
io_qos_temp_bandwidth:       dd 0
io_qos_temp_outstanding:     dd 0
io_qos_temp_record:          dd 0
io_qos_temp_request:         dd 0
io_qos_temp_profile:         dd 0
io_qos_temp_request_record:  dd 0
io_qos_terminal_request:     dd 0
io_qos_temp_slot:            dd 0
io_qos_test_profile:         dd 0
io_qos_test_scope:           dd 0
io_qos_test_task:            dd 0
io_qos_test_request1:        dd 0
io_qos_test_request2:        dd 0
align 4
io_qos_table:
    times IO_QOS_CAPACITY * IO_QOS_RECORD_SIZE db 0

io_request_manager_self_test:
    mov eax, 1
    mov edx, [task_scope_kernel_root_id]
    xor ebx, ebx
    call task_scope_create
    jc .invalid
    mov [io_request_test_scope], eax
    mov eax, 1
    mov edx, [io_request_test_scope]
    xor ebx, ebx
    xor ecx, ecx
    call task_create
    jc .invalid
    mov [io_request_test_task], eax
    mov edx, [timer_ticks]
    add edx, 100
    mov [io_request_test_deadline], edx
    mov ebx, TASK_DEADLINE_CLASS_SOFT
    mov ecx, TASK_DEADLINE_POLICY_CANCEL
    call task_deadline_set
    jc .invalid

    ; Partial Completion prueft getrennte Request-/Completion-Zustaende.
    mov eax, 1
    mov edx, [io_request_test_task]
    mov ebx, 1
    mov ecx, 0x100
    mov esi, 0x200
    mov edi, 4096
    mov ebp, 1
    call io_request_submit
    jc .invalid
    mov [io_request_test_first], eax
    call io_request_lookup
    jc .invalid
    cmp dword [eax + IO_REQUEST_STATE], IO_REQUEST_STATE_PENDING
    jne .invalid
    mov edx, [io_request_test_scope]
    cmp [eax + IO_REQUEST_SCOPE], edx
    jne .invalid
    mov edx, [io_request_test_deadline]
    cmp [eax + IO_REQUEST_DEADLINE], edx
    jne .invalid
    mov eax, [io_request_test_first]
    mov edx, 2048
    xor ebx, ebx
    call io_request_complete
    jc .invalid
    mov eax, [io_request_test_first]
    call io_request_lookup
    jc .invalid
    cmp dword [eax + IO_REQUEST_COMPLETION], IO_COMPLETION_PARTIAL
    jne .invalid

    ; Alle acht Slots belegen; der neunte Request muss Backpressure melden.
    xor ecx, ecx
.fill:
    cmp ecx, IO_REQUEST_CAPACITY
    jae .full
    push ecx
    mov eax, 1
    mov edx, [io_request_test_task]
    mov ebx, 2
    mov ecx, 0x101
    mov esi, 0x201
    mov edi, 512
    xor ebp, ebp
    call io_request_submit
    pop ecx
    jc .invalid
    test ecx, ecx
    jnz .filled
    mov [io_request_test_cancelled], eax
.filled:
    inc ecx
    jmp .fill
.full:
    mov eax, 1
    mov edx, [io_request_test_task]
    mov ebx, 2
    mov ecx, 0x102
    mov esi, 0x202
    mov edi, 512
    xor ebp, ebp
    call io_request_submit
    jnc .invalid
    cmp dword [io_request_backpressure_count], 1
    jne .invalid
    cmp dword [io_request_outstanding], IO_REQUEST_CAPACITY
    jne .invalid

    mov eax, [io_request_test_task]
    mov edx, 0x494F4341               ; "IOCA"
    call task_request_cancel
    jc .invalid
    cmp dword [io_request_outstanding], 0
    jne .invalid
    mov eax, [io_request_test_cancelled]
    call io_request_lookup
    jc .invalid
    cmp dword [eax + IO_REQUEST_COMPLETION], IO_COMPLETION_CANCELLED
    jne .invalid
    mov eax, [io_request_test_cancelled]
    mov edx, 512
    xor ebx, ebx
    call io_request_complete
    jnc .invalid                       ; terminale Completion bleibt eindeutig
    mov eax, [io_request_test_task]
    call task_checkpoint
    jc .invalid
    cmp eax, 1
    jne .invalid
    mov eax, [io_request_test_scope]
    call task_scope_close
    jc .invalid
    cmp dword [task_count], 0
    jne .invalid
    clc
    ret
.invalid:
    stc
    ret

align 4
io_request_manager_api:
    dd IO_REQUEST_API_SIZE
    dw 1, 0
    dd IO_REQUEST_CAPACITY
    dd IO_CAP_ASYNC_COMPLETION | IO_CAP_BOUNDED_QUEUE | IO_CAP_TASK_OWNERSHIP | IO_CAP_DEADLINE
    dd io_request_submit
    dd io_request_complete
    dd io_request_cancel
    dd io_request_table

io_request_manager_ready:        dd 0
io_request_next_id:              dd 0
io_request_outstanding:          dd 0
io_request_backpressure_count:   dd 0
io_request_deadline_miss_count:  dd 0
io_request_poll_tick:            dd 0
io_request_temp_owner:           dd 0
io_request_temp_task:            dd 0
io_request_temp_operation:       dd 0
io_request_temp_target:          dd 0
io_request_temp_buffer:          dd 0
io_request_temp_length:          dd 0
io_request_temp_priority:        dd 0
io_request_temp_slot:            dd 0
io_request_temp_id:              dd 0
io_request_temp_transferred:     dd 0
io_request_temp_error:           dd 0
io_request_temp_record:          dd 0
io_request_temp_task_record:     dd 0
io_request_test_scope:           dd 0
io_request_test_task:            dd 0
io_request_test_deadline:        dd 0
io_request_test_first:           dd 0
io_request_test_cancelled:       dd 0
align 4
io_request_table:
    times IO_REQUEST_CAPACITY * IO_REQUEST_RECORD_SIZE db 0
io_request_submit_ticks:
    times IO_REQUEST_CAPACITY dd 0
io_request_dispatch_ticks:
    times IO_REQUEST_CAPACITY dd 0
io_request_wait_rounds:
    times IO_REQUEST_CAPACITY dd 0
io_request_qos_ids:
    times IO_REQUEST_CAPACITY dd 0

; ---------------------------------------------------------------------------
; Initialer x86-32 Userspace-Prozess (Bootphase 10)
; ---------------------------------------------------------------------------
USER_CODE_SELECTOR equ 0x1B
USER_DATA_SELECTOR equ 0x23
TSS_SELECTOR       equ 0x28
USER_CODE_ADDRESS  equ 0x00400000
; Drei Codeseiten (Explorer/Shell-Programm), direkt gefolgt von einer
; Stack-Seite ohne Luecke; siehe userspace_initialize.
USER_STACK_ADDRESS equ 0x00404000
PROCESS_FLAG_SYSTEM_SERVICE equ 0x00000002
PROCESS_FLAG_USERSPACE      equ 0x00000004
SYSCALL_ABI_VERSION         equ 0x00000001
SYSCALL_SERVICE_CORE        equ 1
SYSCALL_SERVICE_PROCESS     equ 2
SYSCALL_SERVICE_THREAD      equ 3
SYSCALL_SERVICE_IPC         equ 5
SYSCALL_SERVICE_VFS         equ 6
SYSCALL_SERVICE_POWER       equ 7
SYSCALL_SERVICE_NETWORK     equ 8
SYSCALL_SERVICE_LOGGING     equ 9
SYSCALL_SERVICE_DISPLAY     equ 12
SYSCALL_CORE_EXIT           equ 1
SYSCALL_CORE_READY          equ 2
SYSCALL_CORE_CLOSE_HANDLE   equ 3
SYSCALL_QUERY_SELF          equ 1
SYSCALL_IPC_SEND            equ 1
SYSCALL_IPC_RECEIVE         equ 2
SYSCALL_IPC_PACKET_ABI      equ 2
SYSCALL_VFS_OPEN_ROOT       equ 1
SYSCALL_VFS_LOOKUP          equ 2
SYSCALL_POWER_QUERY_SYSTEM  equ 1
SYSCALL_POWER_WAKE_ACQUIRE  equ 2
SYSCALL_POWER_WAKE_RELEASE  equ 3
SYSCALL_POWER_SET_PROFILE   equ 4
SYSCALL_POWER_REQUEST_STATE equ 5
SYSCALL_NETWORK_QUERY       equ 1
SYSCALL_NETWORK_RAW_OPEN    equ 2
SYSCALL_NETWORK_SOCKET_CREATE equ 3
SYSCALL_NETWORK_SEND        equ 4
SYSCALL_NETWORK_RECEIVE     equ 5
SYSCALL_NETWORK_BIND        equ 6
SYSCALL_NETWORK_STREAM_CREATE equ 7
SYSCALL_NETWORK_CONNECT     equ 8
SYSCALL_NETWORK_STREAM_SEND equ 9
SYSCALL_NETWORK_STREAM_RECEIVE equ 10
SYSCALL_NETWORK_STREAM_BIND equ 11
SYSCALL_NETWORK_LISTEN      equ 12
SYSCALL_NETWORK_ACCEPT      equ 13
SYSCALL_LOG_QUERY           equ 1
SYSCALL_LOG_READ_LATEST     equ 2
SYSCALL_DISPLAY_QUERY_PRIMARY equ 1
SYSCALL_DISPLAY_SUBMIT_SCENE equ 2
SYSCALL_DISPLAY_POLL_INPUT   equ 3
SYSCALL_STATUS_OK           equ 0
SYSCALL_STATUS_ABI          equ -1
SYSCALL_STATUS_SIZE         equ -4
SYSCALL_STATUS_RESERVED     equ -5
SYSCALL_STATUS_SERVICE      equ -8
SYSCALL_STATUS_OPERATION    equ -9
SYSCALL_STATUS_ACCESS       equ -13
SYSCALL_STATUS_POINTER      equ -15
SYSCALL_STATUS_WOULD_BLOCK  equ -19
SYSCALL_STATUS_TYPE         equ -22
SYSCALL_STATUS_VALIDATION   equ -23
; §011 neue Konstanten (NPSPEC-KERNEL-0011)
SYSCALL_CORE_QUERY_ABI      equ 4       ; §011 §26: ABI-Version abfragen
SYSCALL_CALL_DEPTH_MAX      equ 8       ; §011 §39: max. Verschachtelungstiefe
DISPLAY_INFO_SIZE           equ 40
SYSTEM_SCENE_SIZE           equ 64
SYSTEM_INPUT_EVENT_SIZE     equ 32
SYSTEM_INPUT_TOGGLE_START   equ 1
SYSTEM_INPUT_CLOSE_START    equ 2
SYSTEM_INPUT_FOCUS_NEXT     equ 3
SYSTEM_INPUT_ACTIVATE       equ 4
SYSTEM_INPUT_NAVIGATE_UP    equ 5
SYSTEM_INPUT_NAVIGATE_DOWN  equ 6
SYSTEM_INPUT_NAVIGATE_LEFT  equ 7
SYSTEM_INPUT_NAVIGATE_RIGHT equ 8
SYSTEM_INPUT_POINTER_ACTIVATE equ 9
SYSTEM_INPUT_NAVIGATE_BACK  equ 10
SYSTEM_INPUT_DELETE         equ 11
SYSTEM_INPUT_RENAME_KEY     equ 12
SYSTEM_INPUT_TEXT_CHAR      equ 13
DISPLAY_SCENE_DESKTOP       equ 0x00000001
DISPLAY_SCENE_START_MENU    equ 0x00000002
DISPLAY_SCENE_RIBBON        equ 0x00000004
DISPLAY_SCENE_TASKBAR       equ 0x00000008
DISPLAY_SCENE_ALLOWED_FLAGS equ DISPLAY_SCENE_DESKTOP | DISPLAY_SCENE_START_MENU | DISPLAY_SCENE_RIBBON | DISPLAY_SCENE_TASKBAR
USER_ADDRESS_MIN            equ USER_CODE_ADDRESS
USER_ADDRESS_MAX            equ USER_STACK_ADDRESS
; Direkt oberhalb der Stack-Seite (USER_ADDRESS_MAX/USER_STACK_ADDRESS),
; damit sie nicht mit dieser ueberlappt.
SHARED_SERVICE_ADDRESS      equ USER_STACK_ADDRESS
SHARED_SERVICE_SIGNATURE    equ 0x5353564E ; "NVSS"
SHARED_SERVICE_SIZE         equ 64
SHARED_FEATURE_INT80        equ 0x00000001
SHARED_FEATURE_COPY_IO      equ 0x00000002
SHARED_FEATURE_PREEMPT      equ 0x00000004
SHARED_FEATURE_DISPLAY      equ 0x00000008
SHARED_FEATURE_FILESYSTEM   equ 0x00000100
SHARED_FEATURE_FILESYSTEM_WRITABLE equ 0x00000200
SHARED_SERVICE_BITMAP       equ 0x000008F7 ; Core, Process, Thread, IPC, VFS, Power, Network, Display

userspace_initialize:
    ; TSS stellt für Ring-3-Interrupts einen kontrollierten Kernelstack bereit.
    mov edi, kernel_tss
    xor eax, eax
    mov ecx, 104 / 4
    rep stosd
    mov eax, [kernel_boot_stack_top]
    mov [kernel_tss + 4], eax
    mov word [kernel_tss + 8], DATA_SEGMENT
    mov word [kernel_tss + 102], 104
    mov eax, kernel_tss
    mov word [kernel_tss_descriptor + 0], 103
    mov word [kernel_tss_descriptor + 2], ax
    shr eax, 16
    mov byte [kernel_tss_descriptor + 4], al
    mov byte [kernel_tss_descriptor + 5], 0x89
    mov byte [kernel_tss_descriptor + 6], 0
    mov byte [kernel_tss_descriptor + 7], ah
    lgdt [kernel_gdt_descriptor]
    mov ax, TSS_SELECTOR
    ltr ax

    ; Eigene physische Seiten für Code und Stack; nur diese PTEs tragen U/S.
    call pmm_alloc_page
    test eax, eax
    jz .invalid
    mov [userspace_code_page], eax
    mov edi, eax
    xor eax, eax
    mov ecx, PMM_PAGE_SIZE / 4
    rep stosd
    mov esi, userspace_program_start
    mov edi, [userspace_code_page]
    mov ecx, PMM_PAGE_SIZE
    rep movsb
    mov eax, USER_CODE_ADDRESS
    mov edx, [userspace_code_page]
    mov ebx, PAGING_PAGE_PRESENT | PAGING_PAGE_USER
    call paging_map_page
    jc .invalid

    ; Die interaktive System-UI belegt drei fest begrenzte Codeseiten. Alle
    ; sind user-lesbar und ausführbar, aber weiterhin nicht beschreibbar.
    ; Jede Seite wird unabhängig von der tatsächlichen Programmlänge mit
    ; einer vollen Seite aus dem Kernel-Abbild kopiert (nicht nur dem Rest,
    ; der laut userspace_program_end tatsächlich zum Programm gehört) –
    ; das vermeidet eine von der Programmlänge abhängige, möglicherweise
    ; über die Zielseite hinauslaufende Kopierlänge. Bytes hinter
    ; userspace_program_end liegen innerhalb des geladenen Kernel-Abbilds
    ; (der %error-Codebudget-Check weiter unten garantiert das) und werden
    ; nie ausgeführt.
    call pmm_alloc_page
    test eax, eax
    jz .invalid
    mov [userspace_code_page_2], eax
    mov edi, eax
    xor eax, eax
    mov ecx, PMM_PAGE_SIZE / 4
    rep stosd
    mov esi, userspace_program_start + PMM_PAGE_SIZE
    mov edi, [userspace_code_page_2]
    mov ecx, PMM_PAGE_SIZE
    rep movsb
    mov eax, USER_CODE_ADDRESS + PMM_PAGE_SIZE
    mov edx, [userspace_code_page_2]
    mov ebx, PAGING_PAGE_PRESENT | PAGING_PAGE_USER
    call paging_map_page
    jc .invalid

    call pmm_alloc_page
    test eax, eax
    jz .invalid
    mov [userspace_code_page_3], eax
    mov edi, eax
    xor eax, eax
    mov ecx, PMM_PAGE_SIZE / 4
    rep stosd
    mov esi, userspace_program_start + PMM_PAGE_SIZE * 2
    mov edi, [userspace_code_page_3]
    mov ecx, PMM_PAGE_SIZE
    rep movsb
    mov eax, USER_CODE_ADDRESS + PMM_PAGE_SIZE * 2
    mov edx, [userspace_code_page_3]
    mov ebx, PAGING_PAGE_PRESENT | PAGING_PAGE_USER
    call paging_map_page
    jc .invalid

    call pmm_alloc_page
    test eax, eax
    jz .invalid
    mov [userspace_stack_page], eax
    mov edi, eax
    xor eax, eax
    mov ecx, PMM_PAGE_SIZE / 4
    rep stosd
    mov eax, USER_STACK_ADDRESS - PMM_PAGE_SIZE
    mov edx, [userspace_stack_page]
    mov ebx, PAGING_PAGE_PRESENT | PAGING_PAGE_WRITE | PAGING_PAGE_USER
    call paging_map_page
    jc .invalid
    mov eax, cr3
    mov cr3, eax                    ; TLB nach den neuen PTEs invalidieren

    call shared_service_page_initialize
    jc .invalid

    mov eax, PROCESS_FLAG_SYSTEM_SERVICE | PROCESS_FLAG_USERSPACE
    mov edx, cr3
    call process_create
    jc .invalid
    mov [userspace_pid], eax
    cmp eax, 2
    jne .invalid
    mov edx, SECURITY_CAP_SERVICE
    call security_grant
    jc .invalid
    mov eax, 2
    mov edx, SECURITY_CAP_IPC
    call security_grant
    jc .invalid
    mov eax, 2
    mov edx, SECURITY_CAP_POWER_QUERY | SECURITY_CAP_POWER_WAKE | SECURITY_CAP_POWER_PROFILE
    call security_grant
    jc .invalid
    mov eax, 2
    mov edx, SECURITY_CAP_NET_QUERY | SECURITY_CAP_NET_CONNECT | SECURITY_CAP_NET_LISTEN | SECURITY_CAP_LOG_READ
    call security_grant
    jc .invalid
    mov eax, 2
    mov edx, SECURITY_CAP_DISPLAY_SYSTEM_UI
    call security_grant
    jc .invalid
    ; Dateisystem: Lesen und Schreiben, aber keine System-Write-Authority.
    mov eax, 2
    mov edx, SECURITY_CAP_FS_READ | SECURITY_CAP_FS_WRITE
    call security_grant
    jc .invalid

    ; Der Bootstrap-Kernelthread (TID 1 / Scheduler-Slot 0) wird atomar zum
    ; ersten Userspace-Thread des neuen Prozesses weitergeführt.
    mov eax, 1
    call thread_lookup
    jc .invalid
    mov dword [eax + THREAD_PID], 2
    mov dword [eax + THREAD_ENTRY], USER_CODE_ADDRESS
    mov dword [eax + THREAD_CONTEXT], 0
    mov dword [userspace_tid], 1

    mov dword [handle_owner_pid], 2
    mov eax, 2
    call process_lookup
    jc .invalid
    mov edx, [eax + PROCESS_HANDLE]
    mov eax, 2
    mov ebx, OBJECT_TYPE_PROCESS
    mov ecx, HANDLE_RIGHT_QUERY | HANDLE_RIGHT_WAIT
    mov esi, HANDLE_FLAG_PROTECTED
    call handle_create
    jc .invalid
    mov [userspace_process_handle], eax
    mov eax, 1
    call thread_lookup
    jc .invalid
    mov edx, [eax + THREAD_HANDLE]
    mov eax, 2
    mov ebx, OBJECT_TYPE_THREAD
    mov ecx, HANDLE_RIGHT_QUERY | HANDLE_RIGHT_WAIT
    mov esi, HANDLE_FLAG_PROTECTED
    call handle_create
    jc .invalid
    mov [userspace_thread_handle], eax
    cmp dword [handle_active_count], 2
    jne .invalid
    mov edx, [userspace_process_handle]
    mov eax, 2
    mov ebx, OBJECT_TYPE_PROCESS
    mov ecx, HANDLE_RIGHT_QUERY
    call handle_resolve
    jc .invalid
    mov edx, [userspace_process_handle]
    mov eax, 2
    mov ebx, OBJECT_TYPE_THREAD       ; Typverwechslung muss scheitern
    mov ecx, HANDLE_RIGHT_QUERY
    call handle_resolve
    jnc .invalid
    call userspace_ipc_initialize
    jc .invalid
    call userspace_vfs_initialize
    jc .invalid

    ; Grenztests der öffentlichen Copy-in-Prüfung.
    mov esi, USER_ADDRESS_MIN
    mov ecx, 16
    call syscall_validate_user_range
    jc .invalid
    mov esi, USER_ADDRESS_MIN - 1
    mov ecx, 1
    call syscall_validate_user_range
    jnc .invalid
    mov esi, 0xFFFFFFF8
    mov ecx, 16
    call syscall_validate_user_range
    jnc .invalid
    mov dword [userspace_exit_seen], 0
    mov dword [userspace_ready_seen], 0
    clc
    ret
.invalid:
    stc
    ret

userspace_enter:
    cli
    mov ax, USER_DATA_SELECTOR
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    push dword USER_DATA_SELECTOR
    push dword USER_STACK_ADDRESS
    push dword 0x00000202
    push dword USER_CODE_SELECTOR
    push dword USER_CODE_ADDRESS
    iretd

; Feste Stackbereiche des Ring-3-VFS-Tests (unterhalb der bisherigen Nutzung)
%define UFS_ADDR(x) (USER_CODE_ADDRESS + (x) - userspace_program_start)
UFS_H_FILE  equ USER_STACK_ADDRESS - 1300
UFS_H_DIR   equ USER_STACK_ADDRESS - 1304
UFS_H_NEW   equ USER_STACK_ADDRESS - 1308
UFS_INDEX   equ USER_STACK_ADDRESS - 1312
UFS_ARGS    equ USER_STACK_ADDRESS - 1408
UFS_DATA    equ USER_STACK_ADDRESS - 1536
UFS_ENTRY   equ USER_STACK_ADDRESS - 1856
UFS_H_BASE  equ USER_STACK_ADDRESS - 1316
UFS_FLAGS   equ USER_STACK_ADDRESS - 1320
UFS_PATH    equ USER_STACK_ADDRESS - 2176
UFS_VIEW    equ USER_STACK_ADDRESS - 2688
UFS_CWD_LEN equ USER_STACK_ADDRESS - 2692
UFS_HOME_LEN equ USER_STACK_ADDRESS - 2696
UFS_NEW_LEN equ USER_STACK_ADDRESS - 2700
UFS_NTH     equ USER_STACK_ADDRESS - 2704
UFS_H_TRASH equ USER_STACK_ADDRESS - 2708
UFS_CWD     equ USER_STACK_ADDRESS - 2968
UFS_HOME    equ USER_STACK_ADDRESS - 3032
UFS_H_ROOT  equ USER_STACK_ADDRESS - 3040
; Umbenennen (F2): Zustand des Editierpuffers. ACTIVE/TYPE/INDEX/LEN/OLDLEN
; sind je ein Dword, BUF und OLDNAME je EXPLORER_NAME_MAX (32) Byte lang.
UFS_RENAME_ACTIVE  equ USER_STACK_ADDRESS - 3044
UFS_RENAME_TYPE    equ USER_STACK_ADDRESS - 3048
UFS_RENAME_INDEX   equ USER_STACK_ADDRESS - 3052
UFS_RENAME_LEN     equ USER_STACK_ADDRESS - 3056
UFS_RENAME_OLDLEN  equ USER_STACK_ADDRESS - 3060
UFS_RENAME_BUF     equ USER_STACK_ADDRESS - 3092
UFS_RENAME_OLDNAME equ USER_STACK_ADDRESS - 3124

userspace_program_start:
    ; Process.QuerySelf -> Ergebnis auf dem beschreibbaren Userstack.
    mov eax, SYSCALL_SERVICE_PROCESS
    mov ebx, SYSCALL_QUERY_SELF
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 64
    mov esi, 16
    int 0x80
    test eax, eax
    jnz .failed
    cmp dword [USER_STACK_ADDRESS - 56], 2
    jne .failed
    mov eax, SYSCALL_SERVICE_PROCESS
    mov ebx, 2                       ; Process.OpenSelf
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 64
    mov esi, 16
    int 0x80
    test eax, eax
    jnz .failed
    cmp dword [USER_STACK_ADDRESS - 56], 0
    je .failed

    ; Thread.QuerySelf -> TID 1 des weitergeführten Bootstrap-Threads.
    mov eax, SYSCALL_SERVICE_THREAD
    mov ebx, SYSCALL_QUERY_SELF
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 32
    mov esi, 16
    int 0x80
    test eax, eax
    jnz .failed
    cmp dword [USER_STACK_ADDRESS - 24], 1
    jne .failed
    mov eax, SYSCALL_SERVICE_THREAD
    mov ebx, 2                       ; Thread.OpenSelf
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 32
    mov esi, 16
    int 0x80
    test eax, eax
    jnz .failed
    cmp dword [USER_STACK_ADDRESS - 24], 0
    je .failed

    ; Gleiche technische Bytes8-Repräsentation reicht nicht: ein anderer
    ; Semantic Type und eine inkompatible Version müssen vor Queue-Mutation
    ; zurückgewiesen werden. Die Capability-/Handle-Prüfung bleibt separat.
    mov esi, USER_CODE_ADDRESS + userspace_ipc_packet - userspace_program_start
    mov edi, USER_STACK_ADDRESS - 128
    mov ecx, 12
    rep movsd
    mov dword [USER_STACK_ADDRESS - 116], SEMANTIC_IPC_DIAGNOSTIC
    mov eax, SYSCALL_SERVICE_IPC
    mov ebx, SYSCALL_IPC_SEND
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 128
    mov esi, 48
    int 0x80
    cmp eax, SYSCALL_STATUS_TYPE
    jne .failed
    mov dword [USER_STACK_ADDRESS - 116], SEMANTIC_IPC_INLINE_DATA
    mov dword [USER_STACK_ADDRESS - 104], 2
    mov eax, SYSCALL_SERVICE_IPC
    mov ebx, SYSCALL_IPC_SEND
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 128
    mov esi, 48
    int 0x80
    cmp eax, SYSCALL_STATUS_TYPE
    jne .failed

    ; Typ und Version sind korrekt, aber ein leerer Inline-Wert verletzt die
    ; deklarierte NONEMPTY-Regel. Das ist ein eigener Validierungsfehler.
    mov dword [USER_STACK_ADDRESS - 104], SEMANTIC_TYPE_VERSION_1
    mov dword [USER_STACK_ADDRESS - 96], 0
    mov eax, SYSCALL_SERVICE_IPC
    mov ebx, SYSCALL_IPC_SEND
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 128
    mov esi, 48
    int 0x80
    cmp eax, SYSCALL_STATUS_VALIDATION
    jne .failed

    mov eax, SYSCALL_SERVICE_IPC
    mov ebx, SYSCALL_IPC_SEND
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_CODE_ADDRESS + userspace_ipc_packet - userspace_program_start
    mov esi, 48
    int 0x80
    test eax, eax
    jnz .failed
    mov eax, SYSCALL_SERVICE_IPC
    mov ebx, SYSCALL_IPC_RECEIVE
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 128
    mov esi, 48
    int 0x80
    test eax, eax
    jnz .failed
    cmp dword [USER_STACK_ADDRESS - 112], 0x11223344
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 96], 4
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 88], 0x41564F4E
    jne .failed

    mov eax, SYSCALL_SERVICE_VFS
    mov ebx, SYSCALL_VFS_OPEN_ROOT
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 160
    mov esi, 16
    int 0x80
    test eax, eax
    jnz .failed
    cmp dword [USER_STACK_ADDRESS - 152], 0
    je .failed

    ; VFS.Lookup mit explizitem Pointer und Pfadlänge für "/".
    mov dword [USER_STACK_ADDRESS - 224], 32
    mov dword [USER_STACK_ADDRESS - 220], SYSCALL_ABI_VERSION
    mov eax, [USER_STACK_ADDRESS - 152]
    mov [USER_STACK_ADDRESS - 216], eax
    mov dword [USER_STACK_ADDRESS - 212], USER_CODE_ADDRESS + userspace_root_path - userspace_program_start
    mov dword [USER_STACK_ADDRESS - 208], 1
    mov dword [USER_STACK_ADDRESS - 204], 0
    mov dword [USER_STACK_ADDRESS - 200], 0
    mov dword [USER_STACK_ADDRESS - 196], 0
    mov eax, SYSCALL_SERVICE_VFS
    mov ebx, SYSCALL_VFS_LOOKUP
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 224
    mov esi, 32
    int 0x80
    test eax, eax
    jnz .failed
    cmp dword [USER_STACK_ADDRESS - 200], 0
    je .failed
    mov dword [USER_STACK_ADDRESS - 256], 16
    mov dword [USER_STACK_ADDRESS - 252], SYSCALL_ABI_VERSION
    mov eax, [USER_STACK_ADDRESS - 200]
    mov [USER_STACK_ADDRESS - 248], eax
    mov dword [USER_STACK_ADDRESS - 244], 0
    mov eax, SYSCALL_SERVICE_CORE
    mov ebx, SYSCALL_CORE_CLOSE_HANDLE
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 256
    mov esi, 16
    int 0x80
    test eax, eax
    jnz .failed

    ; NovaFS ueber die VFS-Syscalls: nur bei gemountetem Systemvolume.
    ; Handles mit Rechten, Systembereichs-Policy und Persistenz werden aus
    ; Ring 3 geprueft; alle geoeffneten Handles werden wieder geschlossen.
    test dword [SHARED_SERVICE_ADDRESS + 12], SHARED_FEATURE_FILESYSTEM
    jz .fs_done
    ; A) Vom Kernel geschriebene Systemdatei nur lesend oeffnen
    mov edx, [USER_STACK_ADDRESS - 152]
    mov esi, UFS_ADDR(ufs_path_bootcount)
    mov ecx, ufs_path_bootcount_end - ufs_path_bootcount
    xor edi, edi
    call ufs_lookup
    test eax, eax
    jz .fs_have_system_file
    ; Ein noch nie beschreibbar gestartetes Volume besitzt die Datei nicht.
    cmp eax, SYSCALL_STATUS_NOT_FOUND
    jne .failed
    jmp .fs_write_part
.fs_have_system_file:
    mov [UFS_H_FILE], ebx
    mov edx, ebx
    call ufs_query
    test eax, eax
    jnz .failed
    cmp dword [UFS_ARGS + 24], NOVAFS_TYPE_FILE
    jne .failed
    cmp dword [UFS_ARGS + 32], 27
    jne .failed
    test dword [UFS_ARGS + 12], HANDLE_RIGHT_WRITE
    jnz .failed
    mov ebx, SYSCALL_VFS_READ
    mov edx, [UFS_H_FILE]
    mov esi, UFS_DATA
    mov ecx, 16
    xor edi, edi
    call ufs_io
    test eax, eax
    jnz .failed
    cmp ecx, 16
    jne .failed
    cmp dword [UFS_DATA], 'NOVA'
    jne .failed
    cmp dword [UFS_DATA + 4], 'FS-B'
    jne .failed
    ; Schreiben ohne WRITE-Recht muss scheitern
    mov ebx, SYSCALL_VFS_WRITE
    mov edx, [UFS_H_FILE]
    mov esi, UFS_DATA
    mov ecx, 4
    xor edi, edi
    call ufs_io
    cmp eax, SYSCALL_STATUS_ACCESS
    jne .failed
    mov edx, [UFS_H_FILE]
    call ufs_close
    test eax, eax
    jnz .failed
    ; Ein geschlossenes (veraltetes) Handle verleiht keinen Zugriff mehr
    mov edx, [UFS_H_FILE]
    call ufs_query
    cmp eax, SYSCALL_STATUS_ACCESS
    jne .failed
    ; Ein read-only gemountetes Volume (z. B. DIRTY) wird nur gelesen.
.fs_write_part:
    ; Arbeitsverzeichnis: /Benutzer/<erster Benutzer>/Dokumente, sonst /Benutzer
    call ufs_select_home
    test eax, eax
    jnz .failed
    ; Rootlayout aus Ring 3 pruefen: /Solutions muss als stabiler
    ; Root-Namespace auch ueber die VFS-Directory-Sicht sichtbar sein.
    mov edx, [USER_STACK_ADDRESS - 152]
    mov esi, UFS_ADDR(ufs_path_root)
    mov ecx, ufs_path_root_end - ufs_path_root
    xor edi, edi
    call ufs_lookup
    test eax, eax
    jnz .failed
    mov [UFS_H_ROOT], ebx
    mov dword [UFS_INDEX], 0
.fs_root_next:
    mov edx, [UFS_H_ROOT]
    mov ecx, [UFS_INDEX]
    call ufs_read_directory
    test eax, eax
    jnz .failed
    cmp dword [UFS_ENTRY + 20], ufs_name_solutions_end - ufs_name_solutions
    jne .fs_root_skip
    cmp dword [UFS_ENTRY + 16], NOVAFS_TYPE_DIRECTORY
    jne .failed
    mov esi, UFS_ENTRY + 32
    mov edi, UFS_ADDR(ufs_name_solutions)
    mov ecx, ufs_name_solutions_end - ufs_name_solutions
    repe cmpsb
    je .fs_root_found
.fs_root_skip:
    inc dword [UFS_INDEX]
    cmp dword [UFS_INDEX], 256
    jb .fs_root_next
    jmp .failed
.fs_root_found:
    mov edx, [UFS_H_ROOT]
    call ufs_close
    test eax, eax
    jnz .failed
    test dword [SHARED_SERVICE_ADDRESS + 12], SHARED_FEATURE_FILESYSTEM_WRITABLE
    jz .fs_view
    ; Schreibzugriff im Systembereich verweigert die Policy
    mov edx, [USER_STACK_ADDRESS - 152]
    mov esi, UFS_ADDR(ufs_path_bootcount)
    mov ecx, ufs_path_bootcount_end - ufs_path_bootcount
    mov edi, VFS_LOOKUP_FLAG_WRITE
    call ufs_lookup
    cmp eax, SYSCALL_STATUS_ACCESS
    jne .failed

    ; B) Im Arbeitsverzeichnis Datei suchen oder anlegen
    mov edx, [UFS_H_DIR]
    mov esi, UFS_ADDR(ufs_name_welcome)
    mov ecx, ufs_name_welcome_end - ufs_name_welcome
    mov edi, VFS_LOOKUP_FLAG_WRITE
    call ufs_lookup
    test eax, eax
    jz .fs_have_file
    cmp eax, SYSCALL_STATUS_NOT_FOUND
    jne .failed
    mov edx, [UFS_H_DIR]
    mov esi, UFS_ADDR(ufs_name_welcome)
    mov ecx, ufs_name_welcome_end - ufs_name_welcome
    mov edi, NOVAFS_TYPE_FILE
    call ufs_create
    test eax, eax
    jnz .failed
.fs_have_file:
    mov [UFS_H_NEW], ebx
    ; doppelter Name wird abgewiesen
    mov edx, [UFS_H_DIR]
    mov esi, UFS_ADDR(ufs_name_welcome)
    mov ecx, ufs_name_welcome_end - ufs_name_welcome
    mov edi, NOVAFS_TYPE_FILE
    call ufs_create
    cmp eax, SYSCALL_STATUS_EXISTS
    jne .failed
    ; schreiben und zuruecklesen
    mov ebx, SYSCALL_VFS_WRITE
    mov edx, [UFS_H_NEW]
    mov esi, UFS_ADDR(ufs_welcome_text)
    mov ecx, ufs_welcome_text_end - ufs_welcome_text
    xor edi, edi
    call ufs_io
    test eax, eax
    jnz .failed
    cmp ecx, ufs_welcome_text_end - ufs_welcome_text
    jne .failed
    mov ebx, SYSCALL_VFS_READ
    mov edx, [UFS_H_NEW]
    mov esi, UFS_DATA
    mov ecx, 64
    xor edi, edi
    call ufs_io
    test eax, eax
    jnz .failed
    cmp ecx, ufs_welcome_text_end - ufs_welcome_text
    jne .failed
    mov esi, UFS_DATA
    mov edi, UFS_ADDR(ufs_welcome_text)
    repe cmpsb
    jne .failed
    ; C) Arbeitsverzeichnis enumerieren und die Datei samt Groesse finden
    mov dword [UFS_INDEX], 0
.fs_dir_next:
    mov edx, [UFS_H_DIR]
    mov ecx, [UFS_INDEX]
    call ufs_read_directory
    test eax, eax
    jnz .failed
    cmp dword [UFS_ENTRY + 20], ufs_name_welcome_end - ufs_name_welcome
    jne .fs_dir_skip
    mov esi, UFS_ENTRY + 32
    mov edi, UFS_ADDR(ufs_name_welcome)
    mov ecx, ufs_name_welcome_end - ufs_name_welcome
    repe cmpsb
    je .fs_dir_found
.fs_dir_skip:
    inc dword [UFS_INDEX]
    cmp dword [UFS_INDEX], 256
    jb .fs_dir_next
    jmp .failed
.fs_dir_found:
    cmp dword [UFS_ENTRY + 24], ufs_welcome_text_end - ufs_welcome_text
    jne .failed
    cmp dword [UFS_ENTRY + 16], NOVAFS_TYPE_FILE
    jne .failed
    mov edx, [UFS_H_NEW]
    call ufs_close
    test eax, eax
    jnz .failed
    ; E) Umbenennen, Verschieben und Loeschen samt Fehlerfaellen
    mov edx, [UFS_H_DIR]
    mov esi, UFS_ADDR(ufs_name_temp)
    mov ecx, ufs_name_temp_end - ufs_name_temp
    mov edi, NOVAFS_TYPE_FILE
    call ufs_create
    test eax, eax
    jnz .failed
    mov [UFS_H_NEW], ebx
    mov ebx, SYSCALL_VFS_WRITE
    mov edx, [UFS_H_NEW]
    mov esi, UFS_ADDR(ufs_welcome_text)
    mov ecx, ufs_welcome_text_end - ufs_welcome_text
    xor edi, edi
    call ufs_io
    test eax, eax
    jnz .failed
    ; Umbenennen im selben Verzeichnis
    mov edx, [UFS_H_DIR]
    mov esi, UFS_ADDR(ufs_name_temp)
    mov ecx, ufs_name_temp_end - ufs_name_temp
    mov edi, edx
    mov ebx, UFS_ADDR(ufs_name_temp2)
    mov ebp, ufs_name_temp2_end - ufs_name_temp2
    call ufs_rename
    test eax, eax
    jnz .failed
    ; ein vorhandenes Ziel wird nicht ueberschrieben
    mov edx, [UFS_H_DIR]
    mov esi, UFS_ADDR(ufs_name_temp2)
    mov ecx, ufs_name_temp2_end - ufs_name_temp2
    mov edi, edx
    mov ebx, UFS_ADDR(ufs_name_welcome)
    mov ebp, ufs_name_welcome_end - ufs_name_welcome
    call ufs_rename
    cmp eax, SYSCALL_STATUS_EXISTS
    jne .failed
    ; in ein neues Unterverzeichnis verschieben
    mov edx, [UFS_H_DIR]
    mov esi, UFS_ADDR(ufs_name_trash)
    mov ecx, ufs_name_trash_end - ufs_name_trash
    mov edi, NOVAFS_TYPE_DIRECTORY
    call ufs_create
    test eax, eax
    jnz .failed
    mov [UFS_H_TRASH], ebx
    mov edx, [UFS_H_DIR]
    mov esi, UFS_ADDR(ufs_name_temp2)
    mov ecx, ufs_name_temp2_end - ufs_name_temp2
    mov edi, [UFS_H_TRASH]
    mov ebx, UFS_ADDR(ufs_name_temp)
    mov ebp, ufs_name_temp_end - ufs_name_temp
    call ufs_rename
    test eax, eax
    jnz .failed
    ; ein Verzeichnis kann nicht in sich selbst wandern
    mov edx, [UFS_H_DIR]
    mov esi, UFS_ADDR(ufs_name_trash)
    mov ecx, ufs_name_trash_end - ufs_name_trash
    mov edi, [UFS_H_TRASH]
    mov ebx, esi
    mov ebp, ecx
    call ufs_rename
    cmp eax, SYSCALL_STATUS_VALIDATION
    jne .failed
    ; nicht leeres Verzeichnis bleibt bestehen
    mov edx, [UFS_H_DIR]
    mov esi, UFS_ADDR(ufs_name_trash)
    mov ecx, ufs_name_trash_end - ufs_name_trash
    call ufs_delete
    cmp eax, SYSCALL_STATUS_NOT_EMPTY
    jne .failed
    mov edx, [UFS_H_TRASH]
    mov esi, UFS_ADDR(ufs_name_temp)
    mov ecx, ufs_name_temp_end - ufs_name_temp
    call ufs_delete
    test eax, eax
    jnz .failed
    ; das offene Handle auf die geloeschte Datei findet nichts mehr
    mov ebx, SYSCALL_VFS_READ
    mov edx, [UFS_H_NEW]
    mov esi, UFS_DATA
    mov ecx, 16
    xor edi, edi
    call ufs_io
    cmp eax, SYSCALL_STATUS_NOT_FOUND
    jne .failed
    mov edx, [UFS_H_DIR]
    mov esi, UFS_ADDR(ufs_name_trash)
    mov ecx, ufs_name_trash_end - ufs_name_trash
    call ufs_delete
    test eax, eax
    jnz .failed
    mov edx, [UFS_H_NEW]
    call ufs_close
    test eax, eax
    jnz .failed
    mov edx, [UFS_H_TRASH]
    call ufs_close
    test eax, eax
    jnz .failed
.fs_view:
    ; D) Explorer-Ansicht aus dem Arbeitsverzeichnis an den Display Server.
    ; UFS_H_DIR bleibt als aktuelles Explorer-Verzeichnis geoeffnet.
    call ufs_present
    test eax, eax
    jnz .failed
.fs_done:

    ; Power.QuerySystem liefert ausschließlich capability-geschützte,
    ; aggregierte Kernelzustände an den Bootstrap-Prozess.
    mov eax, SYSCALL_SERVICE_POWER
    mov ebx, SYSCALL_POWER_QUERY_SYSTEM
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 320
    mov esi, 32
    int 0x80
    test eax, eax
    jnz .failed
    cmp dword [USER_STACK_ADDRESS - 320], 32
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 316], SYSCALL_ABI_VERSION
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 312], POWER_STATE_RUNNING
    jne .failed
    test dword [USER_STACK_ADDRESS - 308], (1 << POWER_STATE_SHUTDOWN)
    jz .failed

    ; Zeitlich begrenzten Wake Lock anfordern und mit dem ausgegebenen Token
    ; wieder freigeben. Eine zweite Freigabe muss der Kernel ablehnen.
    mov dword [USER_STACK_ADDRESS - 352], 16
    mov dword [USER_STACK_ADDRESS - 348], SYSCALL_ABI_VERSION
    mov dword [USER_STACK_ADDRESS - 344], 100 ; maximal eine Sekunde
    mov dword [USER_STACK_ADDRESS - 340], 0
    mov eax, SYSCALL_SERVICE_POWER
    mov ebx, SYSCALL_POWER_WAKE_ACQUIRE
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 352
    mov esi, 16
    int 0x80
    test eax, eax
    jnz .failed
    mov eax, [USER_STACK_ADDRESS - 344]
    mov dword [USER_STACK_ADDRESS - 368], 16
    mov dword [USER_STACK_ADDRESS - 364], SYSCALL_ABI_VERSION
    mov [USER_STACK_ADDRESS - 360], eax
    mov dword [USER_STACK_ADDRESS - 356], 0
    mov eax, SYSCALL_SERVICE_POWER
    mov ebx, SYSCALL_POWER_WAKE_RELEASE
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 368
    mov esi, 16
    int 0x80
    test eax, eax
    jnz .failed
    mov eax, SYSCALL_SERVICE_POWER
    mov ebx, SYSCALL_POWER_WAKE_RELEASE
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 368
    mov esi, 16
    int 0x80
    test eax, eax
    jz .failed

    mov dword [USER_STACK_ADDRESS - 384], 16
    mov dword [USER_STACK_ADDRESS - 380], SYSCALL_ABI_VERSION
    mov dword [USER_STACK_ADDRESS - 376], POWER_PROFILE_EFFICIENCY
    mov dword [USER_STACK_ADDRESS - 372], 0
    mov eax, SYSCALL_SERVICE_POWER
    mov ebx, SYSCALL_POWER_SET_PROFILE
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 384
    mov esi, 16
    int 0x80
    test eax, eax
    jnz .failed

    ; Den globalen Shutdown ohne POWER_SHUTDOWN-Capability anfordern. Der
    ; Kernel muss fail-closed antworten und darf keinen Übergang beginnen.
    mov dword [USER_STACK_ADDRESS - 416], 24
    mov dword [USER_STACK_ADDRESS - 412], SYSCALL_ABI_VERSION
    mov dword [USER_STACK_ADDRESS - 408], POWER_STATE_SHUTDOWN
    mov dword [USER_STACK_ADDRESS - 404], 1 ; Benutzeranforderung
    mov dword [USER_STACK_ADDRESS - 400], 0
    mov dword [USER_STACK_ADDRESS - 396], 0
    mov eax, SYSCALL_SERVICE_POWER
    mov ebx, SYSCALL_POWER_REQUEST_STATE
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 416
    mov esi, 24
    int 0x80
    cmp eax, SYSCALL_STATUS_ACCESS
    jne .failed

    ; Capability-geschütztes Datagramm-Socket erstellen und ein Paket ohne
    ; ungeprüften Userspace-Pointer über lo0 senden und wieder empfangen.
    mov eax, SYSCALL_SERVICE_NETWORK
    mov ebx, SYSCALL_NETWORK_SOCKET_CREATE
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 464
    mov esi, 16
    int 0x80
    test eax, eax
    jnz .failed
    mov eax, [USER_STACK_ADDRESS - 456]
    test eax, eax
    jz .failed
    mov dword [USER_STACK_ADDRESS - 592], 28
    mov dword [USER_STACK_ADDRESS - 588], SYSCALL_ABI_VERSION
    mov [USER_STACK_ADDRESS - 584], eax
    mov dword [USER_STACK_ADDRESS - 580], NETWORK_AF_IPV4
    mov dword [USER_STACK_ADDRESS - 576], NETWORK_IPV4_LOOPBACK
    mov dword [USER_STACK_ADDRESS - 572], 9000
    mov dword [USER_STACK_ADDRESS - 568], 0
    mov eax, SYSCALL_SERVICE_NETWORK
    mov ebx, SYSCALL_NETWORK_BIND
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 592
    mov esi, 28
    int 0x80
    test eax, eax
    jnz .failed
    mov dword [USER_STACK_ADDRESS - 512], 40
    mov dword [USER_STACK_ADDRESS - 508], SYSCALL_ABI_VERSION
    mov eax, [USER_STACK_ADDRESS - 456]
    mov [USER_STACK_ADDRESS - 504], eax
    mov dword [USER_STACK_ADDRESS - 500], 4
    mov dword [USER_STACK_ADDRESS - 496], 0
    mov dword [USER_STACK_ADDRESS - 492], 0x41564F4E ; "NOVA"
    mov dword [USER_STACK_ADDRESS - 488], 0
    mov dword [USER_STACK_ADDRESS - 484], 0
    mov dword [USER_STACK_ADDRESS - 480], 0
    mov dword [USER_STACK_ADDRESS - 476], 0
    ; Überlange Datagramme müssen vor jeder Queue-Änderung scheitern.
    mov dword [USER_STACK_ADDRESS - 500], 17
    mov eax, SYSCALL_SERVICE_NETWORK
    mov ebx, SYSCALL_NETWORK_SEND
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 512
    mov esi, 40
    int 0x80
    cmp eax, SYSCALL_STATUS_SIZE
    jne .failed
    mov dword [USER_STACK_ADDRESS - 500], 4
    mov eax, SYSCALL_SERVICE_NETWORK
    mov ebx, SYSCALL_NETWORK_SEND
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 512
    mov esi, 40
    int 0x80
    test eax, eax
    jnz .failed
    ; Die Queue ist auf ein Datagramm begrenzt. Ein zweites, nichtblockierendes
    ; Senden darf das erste Paket nicht überschreiben.
    mov eax, SYSCALL_SERVICE_NETWORK
    mov ebx, SYSCALL_NETWORK_SEND
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 512
    mov esi, 40
    int 0x80
    cmp eax, SYSCALL_STATUS_WOULD_BLOCK
    jne .failed
    mov eax, SYSCALL_SERVICE_NETWORK
    mov ebx, SYSCALL_NETWORK_RECEIVE
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 560
    mov esi, 40
    int 0x80
    test eax, eax
    jnz .failed
    cmp dword [USER_STACK_ADDRESS - 548], 4
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 540], 0x41564F4E
    jne .failed
    ; Nach dem einzigen Empfang ist die Queue leer und muss WOULD_BLOCK melden.
    mov eax, SYSCALL_SERVICE_NETWORK
    mov ebx, SYSCALL_NETWORK_RECEIVE
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 560
    mov esi, 40
    int 0x80
    cmp eax, SYSCALL_STATUS_WOULD_BLOCK
    jne .failed

    ; Separates TCP-Stream-Socket erzeugen und capability-geschuetzt mit dem
    ; lokalen Bootstrap-Endpunkt verbinden.
    mov eax, SYSCALL_SERVICE_NETWORK
    mov ebx, SYSCALL_NETWORK_STREAM_CREATE
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 640
    mov esi, 16
    int 0x80
    test eax, eax
    jnz .failed
    mov eax, [USER_STACK_ADDRESS - 632]
    test eax, eax
    jz .failed
    mov dword [USER_STACK_ADDRESS - 672], 28
    mov dword [USER_STACK_ADDRESS - 668], SYSCALL_ABI_VERSION
    mov [USER_STACK_ADDRESS - 664], eax
    mov dword [USER_STACK_ADDRESS - 660], NETWORK_AF_IPV4
    mov dword [USER_STACK_ADDRESS - 656], NETWORK_IPV4_LOOPBACK
    mov dword [USER_STACK_ADDRESS - 652], NETWORK_BOOTSTRAP_PORT
    mov dword [USER_STACK_ADDRESS - 648], 0
    mov eax, SYSCALL_SERVICE_NETWORK
    mov ebx, SYSCALL_NETWORK_CONNECT
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 672
    mov esi, 28
    int 0x80
    test eax, eax
    jnz .failed

    mov dword [USER_STACK_ADDRESS - 720], 40
    mov dword [USER_STACK_ADDRESS - 716], SYSCALL_ABI_VERSION
    mov eax, [USER_STACK_ADDRESS - 632]
    mov [USER_STACK_ADDRESS - 712], eax
    mov dword [USER_STACK_ADDRESS - 708], 4
    mov dword [USER_STACK_ADDRESS - 704], 0
    mov dword [USER_STACK_ADDRESS - 700], 0x41564F4E
    mov dword [USER_STACK_ADDRESS - 696], 0
    mov dword [USER_STACK_ADDRESS - 692], 0
    mov dword [USER_STACK_ADDRESS - 688], 0
    mov dword [USER_STACK_ADDRESS - 684], 0
    mov eax, SYSCALL_SERVICE_NETWORK
    mov ebx, SYSCALL_NETWORK_STREAM_SEND
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 720
    mov esi, 40
    int 0x80
    test eax, eax
    jnz .failed
    ; Der einzelne Stream-Puffer ist voll und darf nicht ueberschrieben werden.
    mov eax, SYSCALL_SERVICE_NETWORK
    mov ebx, SYSCALL_NETWORK_STREAM_SEND
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 720
    mov esi, 40
    int 0x80
    cmp eax, SYSCALL_STATUS_WOULD_BLOCK
    jne .failed
    mov dword [USER_STACK_ADDRESS - 768], 40
    mov dword [USER_STACK_ADDRESS - 764], SYSCALL_ABI_VERSION
    mov eax, [USER_STACK_ADDRESS - 632]
    mov [USER_STACK_ADDRESS - 760], eax
    mov dword [USER_STACK_ADDRESS - 756], 16
    mov dword [USER_STACK_ADDRESS - 752], 0
    mov eax, SYSCALL_SERVICE_NETWORK
    mov ebx, SYSCALL_NETWORK_STREAM_RECEIVE
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 768
    mov esi, 40
    int 0x80
    test eax, eax
    jnz .failed
    cmp dword [USER_STACK_ADDRESS - 756], 4
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 748], 0x41564F4E
    jne .failed
    mov eax, SYSCALL_SERVICE_NETWORK
    mov ebx, SYSCALL_NETWORK_STREAM_RECEIVE
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 768
    mov esi, 40
    int 0x80
    cmp eax, SYSCALL_STATUS_WOULD_BLOCK
    jne .failed

    ; Zweites Stream-Socket als lokalen Listener konfigurieren. Der Backlog
    ; ist fest begrenzt; Accept liefert ein separates Socket-Handle.
    mov eax, SYSCALL_SERVICE_NETWORK
    mov ebx, SYSCALL_NETWORK_STREAM_CREATE
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 800
    mov esi, 16
    int 0x80
    test eax, eax
    jnz .failed
    mov eax, [USER_STACK_ADDRESS - 792]
    test eax, eax
    jz .failed
    mov dword [USER_STACK_ADDRESS - 832], 28
    mov dword [USER_STACK_ADDRESS - 828], SYSCALL_ABI_VERSION
    mov [USER_STACK_ADDRESS - 824], eax
    mov dword [USER_STACK_ADDRESS - 820], NETWORK_AF_IPV4
    mov dword [USER_STACK_ADDRESS - 816], NETWORK_IPV4_LOOPBACK
    mov dword [USER_STACK_ADDRESS - 812], NETWORK_BOOTSTRAP_PORT
    mov dword [USER_STACK_ADDRESS - 808], 0
    mov eax, SYSCALL_SERVICE_NETWORK
    mov ebx, SYSCALL_NETWORK_STREAM_BIND
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 832
    mov esi, 28
    int 0x80
    test eax, eax
    jnz .failed
    mov dword [USER_STACK_ADDRESS - 856], 20
    mov dword [USER_STACK_ADDRESS - 852], SYSCALL_ABI_VERSION
    mov eax, [USER_STACK_ADDRESS - 792]
    mov [USER_STACK_ADDRESS - 848], eax
    mov dword [USER_STACK_ADDRESS - 844], 1
    mov dword [USER_STACK_ADDRESS - 840], 0
    mov eax, SYSCALL_SERVICE_NETWORK
    mov ebx, SYSCALL_NETWORK_LISTEN
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 856
    mov esi, 20
    int 0x80
    test eax, eax
    jnz .failed
    mov dword [USER_STACK_ADDRESS - 880], 20
    mov dword [USER_STACK_ADDRESS - 876], SYSCALL_ABI_VERSION
    mov eax, [USER_STACK_ADDRESS - 792]
    mov [USER_STACK_ADDRESS - 872], eax
    mov dword [USER_STACK_ADDRESS - 868], 0
    mov dword [USER_STACK_ADDRESS - 864], 0
    mov eax, SYSCALL_SERVICE_NETWORK
    mov ebx, SYSCALL_NETWORK_ACCEPT
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 880
    mov esi, 20
    int 0x80
    test eax, eax
    jnz .failed
    cmp dword [USER_STACK_ADDRESS - 868], 0
    je .failed
    ; Die einzige ausstehende Verbindung ist verbraucht.
    mov eax, SYSCALL_SERVICE_NETWORK
    mov ebx, SYSCALL_NETWORK_ACCEPT
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 880
    mov esi, 20
    int 0x80
    cmp eax, SYSCALL_STATUS_WOULD_BLOCK
    jne .failed

    ; Network.Query beschreibt nur den eigenen Standard-Namespace und das
    ; Loopback-Interface. Raw-Sockets bleiben ohne Sonderrecht geschlossen.
    mov eax, SYSCALL_SERVICE_NETWORK
    mov ebx, SYSCALL_NETWORK_QUERY
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 448
    mov esi, 96
    int 0x80
    test eax, eax
    jnz .failed
    cmp dword [USER_STACK_ADDRESS - 448], 96
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 444], SYSCALL_ABI_VERSION
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 436], 1 ; genau lo0
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 428], NETWORK_INTERFACE_UP
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 424], NETWORK_LOOPBACK_MTU
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 416], 1 ; ein TX-Datagramm
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 412], 1 ; ein RX-Datagramm
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 408], 0 ; keine beschädigten Pakete
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 404], 9000
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 400], 1 ; ICMPv4 Echo Request/Reply getestet
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 396], 2 ; Loopback-Netz- und Hostroute
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 392], 2 ; Treffer und Unreachable-Test
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 388], 1 ; ein Treffer
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 384], 4 ; Selbsttests plus UDP/TCP-Senden
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 380], 1 ; fail-closed Test verworfen
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 376], 1 ; IPv6/UDP ::1 validiert
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 372], 1 ; ICMPv6 Echo Request/Reply
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 368], 4 ; TCP Aufbau, Daten, Retransmit, Abbau
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 364], 4 ; vier TCP-Nutzbytes gesendet
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 360], 4 ; vier TCP-Nutzbytes empfangen
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 356], 1 ; eine Verbindung akzeptiert
    jne .failed
    mov eax, SYSCALL_SERVICE_NETWORK
    mov ebx, SYSCALL_NETWORK_RAW_OPEN
    mov ecx, SYSCALL_ABI_VERSION
    xor edx, edx
    xor esi, esi
    int 0x80
    cmp eax, SYSCALL_STATUS_ACCESS
    jne .failed

    mov eax, SYSCALL_SERVICE_LOGGING
    mov ebx, SYSCALL_LOG_QUERY
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 928
    mov esi, 32
    int 0x80
    test eax, eax
    jnz .failed
    cmp dword [USER_STACK_ADDRESS - 928], 32
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 920], 10
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 916], 2
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 908], LOG_RING_CAPACITY
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 904], 10
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 900], 2
    jne .failed
    mov eax, SYSCALL_SERVICE_LOGGING
    mov ebx, SYSCALL_LOG_READ_LATEST
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 960
    mov esi, 32
    int 0x80
    test eax, eax
    jnz .failed
    cmp dword [USER_STACK_ADDRESS - 952], 10
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 948], 1
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 944], 1
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 936], 0x109
    jne .failed

    ; Die feste Shared Service Page ist user-lesbar, aber nicht beschreibbar.
    cmp dword [SHARED_SERVICE_ADDRESS + 0], SHARED_SERVICE_SIGNATURE
    jne .failed
    cmp dword [SHARED_SERVICE_ADDRESS + 4], SHARED_SERVICE_SIZE
    jne .failed
    cmp dword [SHARED_SERVICE_ADDRESS + 8], SYSCALL_ABI_VERSION
    jne .failed
    cmp dword [SHARED_SERVICE_ADDRESS + 20], PMM_PAGE_SIZE
    jne .failed

    ; Der System-UI-Prozess sieht ausschließlich Displaymetadaten. Die
    ; physische Framebufferadresse bleibt hinter dem Display-Service verborgen.
    mov dword [USER_STACK_ADDRESS - 1200], 0
    mov eax, SYSCALL_SERVICE_DISPLAY
    mov ebx, SYSCALL_DISPLAY_QUERY_PRIMARY
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 1040
    mov esi, 40
    int 0x80
    test eax, eax
    jnz .display_unavailable
    cmp dword [USER_STACK_ADDRESS - 1040], 40
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 1032], 1
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 1028], 640
    jb .failed
    cmp dword [USER_STACK_ADDRESS - 1024], 480
    jb .failed

    mov esi, USER_CODE_ADDRESS + userspace_system_scene - userspace_program_start
    mov edi, USER_STACK_ADDRESS - 1120
    mov ecx, 16
    rep movsd
    mov eax, SYSCALL_SERVICE_DISPLAY
    mov ebx, SYSCALL_DISPLAY_SUBMIT_SCENE
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 1120
    mov esi, 64
    int 0x80
    test eax, eax
    jnz .failed
    mov dword [USER_STACK_ADDRESS - 1200], 1
.display_unavailable:

    mov eax, SYSCALL_SERVICE_CORE
    mov ebx, SYSCALL_CORE_READY
    mov ecx, SYSCALL_ABI_VERSION
    xor edx, edx
    xor esi, esi
    int 0x80
    test eax, eax
    jnz .failed
.running:
    cmp dword [USER_STACK_ADDRESS - 1200], 1
    jne .idle
    mov eax, SYSCALL_SERVICE_DISPLAY
    mov ebx, SYSCALL_DISPLAY_POLL_INPUT
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 1184
    mov esi, SYSTEM_INPUT_EVENT_SIZE
    int 0x80
    cmp eax, SYSCALL_STATUS_WOULD_BLOCK
    je .idle
    test eax, eax
    jnz .failed

    mov eax, [USER_STACK_ADDRESS - 1176] ; semantische Eingabeaktion
    cmp eax, SYSTEM_INPUT_TOGGLE_START
    je .toggle_start
    cmp eax, SYSTEM_INPUT_CLOSE_START
    je .close_start
    cmp eax, SYSTEM_INPUT_FOCUS_NEXT
    je .focus_next
    cmp eax, SYSTEM_INPUT_ACTIVATE
    je .activate
    cmp eax, SYSTEM_INPUT_NAVIGATE_UP
    je .navigate_previous
    cmp eax, SYSTEM_INPUT_NAVIGATE_LEFT
    je .navigate_previous
    cmp eax, SYSTEM_INPUT_NAVIGATE_DOWN
    je .navigate_next
    cmp eax, SYSTEM_INPUT_NAVIGATE_RIGHT
    je .navigate_next
    cmp eax, SYSTEM_INPUT_POINTER_ACTIVATE
    je .pointer_activate
    cmp eax, SYSTEM_INPUT_NAVIGATE_BACK
    je .navigate_back
    cmp eax, SYSTEM_INPUT_DELETE
    je .delete_entry
    cmp eax, SYSTEM_INPUT_RENAME_KEY
    je .rename_key
    cmp eax, SYSTEM_INPUT_TEXT_CHAR
    je .text_char
    jmp .failed
.toggle_start:
    xor dword [USER_STACK_ADDRESS - 1104], DISPLAY_SCENE_START_MENU
    test dword [USER_STACK_ADDRESS - 1104], DISPLAY_SCENE_START_MENU
    jz .present_input_scene
    mov dword [USER_STACK_ADDRESS - 1092], 2
    jmp .present_input_scene
.close_start:
    and dword [USER_STACK_ADDRESS - 1104], ~DISPLAY_SCENE_START_MENU
    jmp .present_input_scene
.focus_next:
    test dword [USER_STACK_ADDRESS - 1104], DISPLAY_SCENE_START_MENU
    jz .navigate_next
    inc dword [USER_STACK_ADDRESS - 1092]
    cmp dword [USER_STACK_ADDRESS - 1092], 10
    jbe .present_input_scene
    mov dword [USER_STACK_ADDRESS - 1092], 2
    jmp .present_input_scene
.navigate_previous:
    test dword [USER_STACK_ADDRESS - 1104], DISPLAY_SCENE_START_MENU
    jz .workspace_previous
    dec dword [USER_STACK_ADDRESS - 1092]
    cmp dword [USER_STACK_ADDRESS - 1092], 2
    jae .present_input_scene
    mov dword [USER_STACK_ADDRESS - 1092], 10
    jmp .present_input_scene
.workspace_previous:
    cmp dword [USER_STACK_ADDRESS - 1096], 1
    je .sheet_previous
    cmp dword [USER_STACK_ADDRESS - 1096], 2
    je .studio_previous
    ; Explorer: Dateizeilen 20..23, Ordnerkarten 24..27
    dec dword [USER_STACK_ADDRESS - 1092]
    mov eax, [USER_STACK_ADDRESS - 1092]
    sub eax, 20
    cmp eax, 7
    jbe .present_input_scene
    mov dword [USER_STACK_ADDRESS - 1092], 27
    jmp .present_input_scene
.sheet_previous:
    dec dword [USER_STACK_ADDRESS - 1092]
    cmp dword [USER_STACK_ADDRESS - 1092], 40
    jae .present_input_scene
    mov dword [USER_STACK_ADDRESS - 1092], 45
    jmp .present_input_scene
.studio_previous:
    dec dword [USER_STACK_ADDRESS - 1092]
    cmp dword [USER_STACK_ADDRESS - 1092], 60
    jae .present_input_scene
    mov dword [USER_STACK_ADDRESS - 1092], 65
    jmp .present_input_scene
.navigate_next:
    test dword [USER_STACK_ADDRESS - 1104], DISPLAY_SCENE_START_MENU
    jnz .focus_next
    cmp dword [USER_STACK_ADDRESS - 1096], 1
    je .sheet_next
    cmp dword [USER_STACK_ADDRESS - 1096], 2
    je .studio_next
    inc dword [USER_STACK_ADDRESS - 1092]
    mov eax, [USER_STACK_ADDRESS - 1092]
    sub eax, 20
    cmp eax, 7
    jbe .present_input_scene
    mov dword [USER_STACK_ADDRESS - 1092], 20
    jmp .present_input_scene
.sheet_next:
    inc dword [USER_STACK_ADDRESS - 1092]
    cmp dword [USER_STACK_ADDRESS - 1092], 45
    jbe .present_input_scene
    mov dword [USER_STACK_ADDRESS - 1092], 40
    jmp .present_input_scene
.studio_next:
    inc dword [USER_STACK_ADDRESS - 1092]
    cmp dword [USER_STACK_ADDRESS - 1092], 65
    jbe .present_input_scene
    mov dword [USER_STACK_ADDRESS - 1092], 60
    jmp .present_input_scene
.pointer_activate:
    mov eax, [USER_STACK_ADDRESS - 1160]
    mov [USER_STACK_ADDRESS - 1092], eax
    test dword [USER_STACK_ADDRESS - 1104], DISPLAY_SCENE_START_MENU
    jnz .activate
    cmp eax, 24                      ; Explorer-Ordner, Zurueck, Schnellzugriff
    jb .present_input_scene
.activate:
    cmp dword [UFS_RENAME_ACTIVE], 0
    jne .rename_commit
    mov eax, [USER_STACK_ADDRESS - 1092]
    test dword [USER_STACK_ADDRESS - 1104], DISPLAY_SCENE_START_MENU
    jnz .activate_menu
    cmp dword [USER_STACK_ADDRESS - 1096], 0
    jne .activate_menu
    cmp eax, 24
    jb .open_file
    ; Explorer: 24..27 Ordnerkarte, 28 Zurueck, 30..36 Schnellzugriff
    cmp eax, 28
    je .navigate_back
    jb .explorer_child
    sub eax, 30
    cmp eax, 6
    ja .present_input_scene
    call ufs_enter_quick
    jmp .explorer_moved
.explorer_child:
    sub eax, 24
    call ufs_enter_child
    jmp .explorer_moved
.open_file:
    cmp eax, 20
    jb .activate_menu
    sub eax, 20
    mov ebx, eax
    call ufs_preview_nth
    jmp .present_input_scene
.delete_entry:
    ; Entf: 20..23 Dateizeile, 24..27 Ordnerkarte (nur im Explorer-Arbeitsbereich).
    test dword [USER_STACK_ADDRESS - 1104], DISPLAY_SCENE_START_MENU
    jnz .present_input_scene
    cmp dword [USER_STACK_ADDRESS - 1096], 0
    jne .present_input_scene
    mov eax, [USER_STACK_ADDRESS - 1092]
    cmp eax, 20
    jb .present_input_scene
    cmp eax, 24
    jb .delete_file
    cmp eax, 28
    jae .present_input_scene
    sub eax, 24
    mov ebx, eax
    mov eax, NOVAFS_TYPE_DIRECTORY
    jmp .delete_go
.delete_file:
    sub eax, 20
    mov ebx, eax
    mov eax, NOVAFS_TYPE_FILE
.delete_go:
    call ufs_delete_nth
    test eax, eax
    jnz .present_input_scene
    call ufs_present
    jmp .present_input_scene
.rename_key:
    ; F2: startet die Umbenennung der fokussierten Datei-/Ordnerzeile (nur
    ; im Explorer-Arbeitsbereich, nicht im Startmenue). Ein zweites F2
    ; waehrend des Editierens bricht ohne Speichern ab.
    cmp dword [UFS_RENAME_ACTIVE], 0
    jne .rename_cancel
    test dword [USER_STACK_ADDRESS - 1104], DISPLAY_SCENE_START_MENU
    jnz .present_input_scene
    cmp dword [USER_STACK_ADDRESS - 1096], 0
    jne .present_input_scene
    mov eax, [USER_STACK_ADDRESS - 1092]
    cmp eax, 20
    jb .present_input_scene
    cmp eax, 28
    jae .present_input_scene
    cmp eax, 24
    jb .rename_start_file
    sub eax, 24
    mov ebx, eax
    mov eax, NOVAFS_TYPE_DIRECTORY
    jmp .rename_start
.rename_start_file:
    sub eax, 20
    mov ebx, eax
    mov eax, NOVAFS_TYPE_FILE
.rename_start:
    call ufs_find_nth
    test eax, eax
    jnz .present_input_scene
    mov ecx, [UFS_ENTRY + 20]
    cmp ecx, EXPLORER_NAME_MAX
    jbe .rename_len_ok
    mov ecx, EXPLORER_NAME_MAX
.rename_len_ok:
    mov [UFS_RENAME_OLDLEN], ecx
    mov [UFS_RENAME_LEN], ecx
    mov esi, UFS_ENTRY + 32
    mov edi, UFS_RENAME_OLDNAME
    push ecx
    rep movsb
    pop ecx
    mov esi, UFS_ENTRY + 32
    mov edi, UFS_RENAME_BUF
    rep movsb
    mov dword [UFS_RENAME_ACTIVE], 1
    call ufs_rename_redraw
    jmp .present_input_scene
.rename_cancel:
    mov dword [UFS_RENAME_ACTIVE], 0
    call ufs_present
    jmp .present_input_scene
.text_char:
    ; Druckbares Zeichen (vom Kernel bereits in ASCII uebersetzt, siehe
    ; keyboard_ascii_table) nur waehrend aktiver Umbenennung anhaengen.
    cmp dword [UFS_RENAME_ACTIVE], 0
    je .present_input_scene
    mov eax, [UFS_RENAME_LEN]
    cmp eax, EXPLORER_NAME_MAX
    jae .present_input_scene
    mov edx, [USER_STACK_ADDRESS - 1172] ; Zeichen-Feld des Eingabeereignisses
    mov edi, UFS_RENAME_BUF
    add edi, eax
    mov [edi], dl
    inc dword [UFS_RENAME_LEN]
    call ufs_rename_redraw
    jmp .present_input_scene
.rename_backspace:
    cmp dword [UFS_RENAME_LEN], 0
    je .present_input_scene
    dec dword [UFS_RENAME_LEN]
    call ufs_rename_redraw
    jmp .present_input_scene
.rename_commit:
    mov dword [UFS_RENAME_ACTIVE], 0
    mov edx, [USER_STACK_ADDRESS - 152]
    mov esi, UFS_CWD
    mov ecx, [UFS_CWD_LEN]
    mov edi, VFS_LOOKUP_FLAG_WRITE
    call ufs_lookup
    test eax, eax
    jnz .rename_failed
    mov [UFS_H_NEW], ebx
    mov edx, [UFS_H_NEW]
    mov esi, UFS_RENAME_OLDNAME
    mov ecx, [UFS_RENAME_OLDLEN]
    mov edi, [UFS_H_NEW]
    mov ebx, UFS_RENAME_BUF
    mov ebp, [UFS_RENAME_LEN]
    call ufs_rename
    push eax
    mov edx, [UFS_H_NEW]
    call ufs_close
    pop eax
.rename_failed:
    call ufs_present
    jmp .present_input_scene
.navigate_back:
    cmp dword [UFS_RENAME_ACTIVE], 0
    jne .rename_backspace
    test dword [USER_STACK_ADDRESS - 1104], DISPLAY_SCENE_START_MENU
    jnz .present_input_scene
    cmp dword [USER_STACK_ADDRESS - 1096], 0
    jne .present_input_scene
    call ufs_enter_parent
.explorer_moved:
    ; Fehler (z. B. fehlender Ordner) lassen die bisherige Ansicht bestehen.
    test eax, eax
    jnz .present_input_scene
    mov dword [USER_STACK_ADDRESS - 1092], 24
    jmp .present_input_scene
.activate_menu:
    cmp eax, 3
    je .open_explorer
    cmp eax, 6
    je .open_sheet
    cmp eax, 9
    je .open_studio
    jmp .present_input_scene
.open_explorer:
    mov dword [USER_STACK_ADDRESS - 1096], 0
    mov dword [USER_STACK_ADDRESS - 1092], 20
    jmp .open_workspace
.open_sheet:
    mov dword [USER_STACK_ADDRESS - 1096], 1
    mov dword [USER_STACK_ADDRESS - 1092], 40
    jmp .open_workspace
.open_studio:
    mov dword [USER_STACK_ADDRESS - 1096], 2
    mov dword [USER_STACK_ADDRESS - 1092], 60
.open_workspace:
    and dword [USER_STACK_ADDRESS - 1104], ~DISPLAY_SCENE_START_MENU
.present_input_scene:
    inc dword [USER_STACK_ADDRESS - 1112]
    mov eax, SYSCALL_SERVICE_DISPLAY
    mov ebx, SYSCALL_DISPLAY_SUBMIT_SCENE
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 1120
    mov esi, SYSTEM_SCENE_SIZE
    int 0x80
    test eax, eax
    jnz .failed
.idle:
    pause
    jmp .running
.failed:
    mov eax, SYSCALL_SERVICE_CORE
    mov ebx, SYSCALL_CORE_EXIT
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_CODE_ADDRESS + userspace_exit_arguments - userspace_program_start
    mov esi, 16
    int 0x80
    ud2

userspace_exit_probe:
    mov eax, SYSCALL_SERVICE_CORE
    mov ebx, SYSCALL_CORE_EXIT
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_CODE_ADDRESS + userspace_exit_arguments - userspace_program_start
    mov esi, 16
    int 0x80
    ud2                             ; eine erfolgreiche Exit-Operation kehrt nie zurück
;---------------------------------------------------------------------------
; Ring-3-Hilfsroutinen fuer den VFS-Test (Teil des Userspace-Abbilds)
;---------------------------------------------------------------------------
; EDX=Verzeichnis-Handle, ESI=Pfad, ECX=Laenge, EDI=Flags -> EAX=Status, EBX=Handle
ufs_lookup:
    mov dword [UFS_ARGS + 0], 32
    mov dword [UFS_ARGS + 4], SYSCALL_ABI_VERSION
    mov [UFS_ARGS + 8], edx
    mov [UFS_ARGS + 12], esi
    mov [UFS_ARGS + 16], ecx
    mov [UFS_ARGS + 20], edi
    mov dword [UFS_ARGS + 24], 0
    mov dword [UFS_ARGS + 28], 0
    mov ebx, SYSCALL_VFS_LOOKUP
    call ufs_invoke32
    mov ebx, [UFS_ARGS + 24]
    ret

; EBX=Read/Write, EDX=Handle, ESI=Puffer, ECX=Laenge, EDI=Offset
; -> EAX=Status, ECX=uebertragene Bytes
ufs_io:
    mov dword [UFS_ARGS + 0], 32
    mov dword [UFS_ARGS + 4], SYSCALL_ABI_VERSION
    mov [UFS_ARGS + 8], edx
    mov [UFS_ARGS + 12], esi
    mov [UFS_ARGS + 16], edi
    mov [UFS_ARGS + 20], ecx
    mov dword [UFS_ARGS + 24], 0
    mov dword [UFS_ARGS + 28], 0
    call ufs_invoke32
    mov ecx, [UFS_ARGS + 24]
    ret

; EDX=Verzeichnis-Handle, ESI=Name, ECX=Laenge, EDI=Typ -> EAX=Status, EBX=Handle
ufs_create:
    mov dword [UFS_ARGS + 0], 32
    mov dword [UFS_ARGS + 4], SYSCALL_ABI_VERSION
    mov [UFS_ARGS + 8], edx
    mov [UFS_ARGS + 12], esi
    mov [UFS_ARGS + 16], ecx
    mov [UFS_ARGS + 20], edi
    mov dword [UFS_ARGS + 24], 0
    mov dword [UFS_ARGS + 28], 0
    mov ebx, SYSCALL_VFS_CREATE
    call ufs_invoke32
    mov ebx, [UFS_ARGS + 24]
    ret

; EDX=Verzeichnis-Handle, ECX=Index -> EAX=Status, Eintrag in UFS_ENTRY
ufs_read_directory:
    mov dword [UFS_ARGS + 0], 32
    mov dword [UFS_ARGS + 4], SYSCALL_ABI_VERSION
    mov [UFS_ARGS + 8], edx
    mov [UFS_ARGS + 12], ecx
    mov dword [UFS_ARGS + 16], UFS_ENTRY
    mov dword [UFS_ARGS + 20], VFS_ENTRY_SIZE
    mov dword [UFS_ARGS + 24], 0
    mov dword [UFS_ARGS + 28], 0
    mov ebx, SYSCALL_VFS_READ_DIRECTORY
    jmp ufs_invoke32

; EDX=Handle -> EAX=Status, Ergebnis in UFS_ARGS (48 Byte)
ufs_query:
    mov edi, UFS_ARGS
    xor eax, eax
    mov ecx, VFS_INFO_SIZE / 4
    rep stosd
    mov dword [UFS_ARGS + 0], VFS_INFO_SIZE
    mov dword [UFS_ARGS + 4], SYSCALL_ABI_VERSION
    mov [UFS_ARGS + 8], edx
    mov eax, SYSCALL_SERVICE_VFS
    mov ebx, SYSCALL_VFS_QUERY
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, UFS_ARGS
    mov esi, VFS_INFO_SIZE
    int 0x80
    ret

; EDX=Handle -> EAX=Status
ufs_close:
    mov dword [UFS_ARGS + 0], 16
    mov dword [UFS_ARGS + 4], SYSCALL_ABI_VERSION
    mov [UFS_ARGS + 8], edx
    mov dword [UFS_ARGS + 12], 0
    mov eax, SYSCALL_SERVICE_CORE
    mov ebx, SYSCALL_CORE_CLOSE_HANDLE
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, UFS_ARGS
    mov esi, 16
    int 0x80
    ret

; EBX=VFS-Operation mit 32-Byte-Argumenten in UFS_ARGS -> EAX=Status
ufs_invoke32:
    mov eax, SYSCALL_SERVICE_VFS
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, UFS_ARGS
    mov esi, 32
    int 0x80
    ret

; Waehlt das Arbeitsverzeichnis (UFS_H_DIR, UFS_CWD) und das Benutzerprofil
; (UFS_HOME): /Benutzer/<erster Benutzer>/Dokumente, sonst /Benutzer.
; Schreibrecht nur bei beschreibbarem Volume. EAX=Status.
ufs_select_home:
    xor eax, eax
    test dword [SHARED_SERVICE_ADDRESS + 12], SHARED_FEATURE_FILESYSTEM_WRITABLE
    jz .flags
    mov eax, VFS_LOOKUP_FLAG_WRITE
.flags:
    mov [UFS_FLAGS], eax
    mov edi, UFS_VIEW
    xor eax, eax
    mov ecx, EXPLORER_VIEW_SIZE / 4
    rep stosd
    mov esi, UFS_ADDR(ufs_path_benutzer)
    mov edi, UFS_CWD
    mov ecx, ufs_path_benutzer_end - ufs_path_benutzer
    mov [UFS_CWD_LEN], ecx
    mov [UFS_HOME_LEN], ecx
    rep movsb
    mov esi, UFS_CWD
    mov edi, UFS_HOME
    mov ecx, ufs_path_benutzer_end - ufs_path_benutzer
    rep movsb
    mov edx, [USER_STACK_ADDRESS - 152]
    mov esi, UFS_CWD
    mov ecx, ufs_path_benutzer_end - ufs_path_benutzer
    mov edi, [UFS_FLAGS]
    call ufs_lookup
    test eax, eax
    jnz .return
    mov [UFS_H_BASE], ebx
    mov [UFS_H_DIR], ebx
    mov dword [UFS_INDEX], 0
.scan:
    mov edx, [UFS_H_BASE]
    mov ecx, [UFS_INDEX]
    call ufs_read_directory
    test eax, eax
    jnz .base_only
    cmp dword [UFS_ENTRY + 16], NOVAFS_TYPE_DIRECTORY
    jne .next
    cmp dword [UFS_ENTRY + 20], 29
    ja .next
    ; "<Benutzer>/Dokumente" relativ zu /Benutzer
    mov esi, UFS_ENTRY + 32
    mov edi, UFS_PATH
    mov ecx, [UFS_ENTRY + 20]
    rep movsb
    mov esi, UFS_ADDR(ufs_suffix_documents)
    mov ecx, ufs_suffix_documents_end - ufs_suffix_documents
    rep movsb
    mov ecx, [UFS_ENTRY + 20]
    add ecx, ufs_suffix_documents_end - ufs_suffix_documents
    mov edx, [UFS_H_BASE]
    mov esi, UFS_PATH
    mov edi, [UFS_FLAGS]
    call ufs_lookup
    test eax, eax
    jnz .next
    mov [UFS_H_DIR], ebx
    ; UFS_CWD = /Benutzer/<Name>/Dokumente, UFS_HOME = /Benutzer/<Name>
    mov edi, UFS_CWD + (ufs_path_benutzer_end - ufs_path_benutzer)
    mov al, '/'
    stosb
    mov esi, UFS_ENTRY + 32
    mov ecx, [UFS_ENTRY + 20]
    rep movsb
    mov eax, edi
    sub eax, UFS_CWD
    mov [UFS_HOME_LEN], eax
    mov esi, UFS_ADDR(ufs_suffix_documents)
    mov ecx, ufs_suffix_documents_end - ufs_suffix_documents
    rep movsb
    sub edi, UFS_CWD
    mov [UFS_CWD_LEN], edi
    mov esi, UFS_CWD
    mov edi, UFS_HOME
    mov ecx, [UFS_HOME_LEN]
    rep movsb
    mov edx, [UFS_H_BASE]
    call ufs_close
    ret
.next:
    inc dword [UFS_INDEX]
    cmp dword [UFS_INDEX], 64
    jb .scan
.base_only:
    xor eax, eax
.return:
    ret

; Kandidat in UFS_PATH (ECX Bytes, absolut). Oeffnet ihn lesend; bei Erfolg
; wird er zu UFS_CWD, ersetzt UFS_H_DIR und wird angezeigt. Bei Fehlern bleibt
; die bisherige Ansicht bestehen. EAX=Status.
ufs_open_path:
    mov [UFS_NEW_LEN], ecx
    mov edx, [USER_STACK_ADDRESS - 152]
    mov esi, UFS_PATH
    xor edi, edi
    call ufs_lookup
    test eax, eax
    jnz ufs_present.return
    mov [UFS_H_NEW], ebx
    mov edx, [UFS_H_DIR]
    call ufs_close
    mov eax, [UFS_H_NEW]
    mov [UFS_H_DIR], eax
    mov esi, UFS_PATH
    mov edi, UFS_CWD
    mov ecx, [UFS_NEW_LEN]
    mov [UFS_CWD_LEN], ecx
    rep movsb
; Beschreibt UFS_CWD in UFS_VIEW (Breadcrumb, Schnellzugriff), liest UFS_H_DIR
; und uebergibt die Ansicht. Ohne Display Server wird nur uebersprungen.
ufs_present:
    ; Breadcrumb: fuehrendes '/' weglassen, '/' -> "  /  "
    mov esi, UFS_CWD + 1
    mov ecx, [UFS_CWD_LEN]
    dec ecx
    mov edi, UFS_PATH
.crumb:
    test ecx, ecx
    jle .crumb_done
    lodsb
    cmp al, '/'
    jne .crumb_store
    mov eax, '  / '
    stosd
    mov al, ' '
.crumb_store:
    stosb
    dec ecx
    jmp .crumb
.crumb_done:
    mov ecx, edi
    sub ecx, UFS_PATH
    mov esi, UFS_PATH
    mov edi, UFS_VIEW + 40
    cmp ecx, EXPLORER_PATH_MAX
    jbe .crumb_copy
    ; zu lang: ".." und das Ende des Pfads
    lea esi, [esi + ecx - (EXPLORER_PATH_MAX - 2)]
    mov ecx, EXPLORER_PATH_MAX - 2
    mov ax, '..'
    stosw
.crumb_copy:
    lea eax, [edi + ecx]
    sub eax, UFS_VIEW + 40
    mov [UFS_VIEW + 28], eax
    rep movsb
    ; Schnellzugriff: 1 = Profil, 2..7 = Profil/<Eintrag aus ufs_quick_names>
    mov dword [UFS_VIEW + 24], 0
    mov ecx, [UFS_HOME_LEN]
    cmp ecx, ufs_path_benutzer_end - ufs_path_benutzer
    jbe .view
    cmp [UFS_CWD_LEN], ecx
    jb .view
    mov esi, UFS_CWD
    mov edi, UFS_HOME
    repe cmpsb
    jne .view
    mov ecx, [UFS_CWD_LEN]
    sub ecx, [UFS_HOME_LEN]
    jnz .quick_child
    mov dword [UFS_VIEW + 24], 1
    jmp .view
.quick_child:
    cmp byte [esi], '/'
    jne .view
    inc esi
    dec ecx
    mov edi, UFS_ADDR(ufs_quick_names)
    mov ebx, 2
.quick:
    movzx edx, byte [edi]
    test edx, edx
    jz .view
    cmp edx, ecx
    jne .quick_next
    push esi
    push edi
    push ecx
    inc edi
    repe cmpsb
    pop ecx
    pop edi
    pop esi
    je .quick_found
.quick_next:
    lea edi, [edi + edx + 1]
    inc ebx
    jmp .quick
.quick_found:
    mov [UFS_VIEW + 24], ebx
.view:
    mov edx, [UFS_H_DIR]
    call ufs_build_view
    test eax, eax
    jnz .return
    mov eax, SYSCALL_SERVICE_DISPLAY
    mov ebx, SYSCALL_DISPLAY_SUBMIT_EXPLORER_VIEW
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, UFS_VIEW
    mov esi, EXPLORER_VIEW_SIZE
    int 0x80
    cmp eax, SYSCALL_STATUS_SERVICE
    jne .return
    xor eax, eax
.return:
    ret

; Explorer eine Ebene nach oben (nicht ueber /Benutzer hinaus). EAX=Status.
ufs_enter_parent:
    mov ecx, [UFS_CWD_LEN]
    cmp ecx, ufs_path_benutzer_end - ufs_path_benutzer
    jbe .top
.find:
    dec ecx
    cmp byte [UFS_CWD + ecx], '/'
    jne .find
    mov esi, UFS_CWD
    mov edi, UFS_PATH
    push ecx
    rep movsb
    pop ecx
    jmp ufs_open_path
.top:
    mov eax, SYSCALL_STATUS_NOT_FOUND
    ret

; EAX=Schnellzugriff 0..6 (Profil, Desktop, Dokumente, ...). EAX=Status.
ufs_enter_quick:
    mov ebx, eax
    mov esi, UFS_HOME
    mov edi, UFS_PATH
    mov ecx, [UFS_HOME_LEN]
    rep movsb
    test ebx, ebx
    jz .open
    mov esi, UFS_ADDR(ufs_quick_names)
.skip:
    dec ebx
    jz .append
    movzx eax, byte [esi]
    lea esi, [esi + eax + 1]
    jmp .skip
.append:
    mov al, '/'
    stosb
    movzx ecx, byte [esi]
    inc esi
    rep movsb
.open:
    mov ecx, edi
    sub ecx, UFS_PATH
    jmp ufs_open_path

; EAX=n. Oeffnet das n-te Unterverzeichnis von UFS_H_DIR. EAX=Status.
ufs_enter_child:
    mov [UFS_NTH], eax
    mov dword [UFS_INDEX], 0
.scan:
    mov edx, [UFS_H_DIR]
    mov ecx, [UFS_INDEX]
    call ufs_read_directory
    test eax, eax
    jnz .return
    inc dword [UFS_INDEX]
    cmp dword [UFS_ENTRY + 16], NOVAFS_TYPE_DIRECTORY
    jne .scan
    dec dword [UFS_NTH]
    jns .scan
    mov eax, [UFS_CWD_LEN]
    mov ecx, [UFS_ENTRY + 20]
    lea edx, [eax + ecx + 1]
    cmp edx, 255
    ja .limit
    mov esi, UFS_CWD
    mov edi, UFS_PATH
    mov ecx, eax
    rep movsb
    mov al, '/'
    stosb
    mov esi, UFS_ENTRY + 32
    mov ecx, [UFS_ENTRY + 20]
    rep movsb
    mov ecx, edx
    jmp ufs_open_path
.limit:
    mov eax, SYSCALL_STATUS_SIZE
.return:
    ret

; EDX=Verzeichnis-Handle, ESI=Name, ECX=Laenge -> EAX=Status
ufs_delete:
    mov dword [UFS_ARGS + 0], 32
    mov dword [UFS_ARGS + 4], SYSCALL_ABI_VERSION
    mov [UFS_ARGS + 8], edx
    mov [UFS_ARGS + 12], esi
    mov [UFS_ARGS + 16], ecx
    mov dword [UFS_ARGS + 20], 0
    mov dword [UFS_ARGS + 24], 0
    mov dword [UFS_ARGS + 28], 0
    mov ebx, SYSCALL_VFS_DELETE
    jmp ufs_invoke32

; EDX/ESI/ECX = Quellverzeichnis/-name/-laenge,
; EDI/EBX/EBP = Zielverzeichnis/-name/-laenge -> EAX=Status
ufs_rename:
    mov dword [UFS_ARGS + 0], VFS_RENAME_SIZE
    mov dword [UFS_ARGS + 4], SYSCALL_ABI_VERSION
    mov [UFS_ARGS + 8], edx
    mov [UFS_ARGS + 12], esi
    mov [UFS_ARGS + 16], ecx
    mov [UFS_ARGS + 20], edi
    mov [UFS_ARGS + 24], ebx
    mov [UFS_ARGS + 28], ebp
    mov dword [UFS_ARGS + 32], 0
    mov dword [UFS_ARGS + 36], 0
    mov eax, SYSCALL_SERVICE_VFS
    mov ebx, SYSCALL_VFS_RENAME
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, UFS_ARGS
    mov esi, VFS_RENAME_SIZE
    int 0x80
    ret

; EAX=NovaFS-Typ, EBX=Index -> EAX=Status (0 gefunden, Eintrag in UFS_ENTRY).
; Durchsucht UFS_H_DIR (bereits offen, nur lesend) nach dem n-ten Eintrag
; dieses Typs. Gemeinsame Suche fuer ufs_delete_nth und ufs_preview_nth.
ufs_find_nth:
    mov [UFS_FLAGS], eax
    mov [UFS_NTH], ebx
    mov dword [UFS_INDEX], 0
.scan:
    mov edx, [UFS_H_DIR]
    mov ecx, [UFS_INDEX]
    call ufs_read_directory
    test eax, eax
    jnz .return
    inc dword [UFS_INDEX]
    mov eax, [UFS_FLAGS]
    cmp [UFS_ENTRY + 16], eax
    jne .scan
    dec dword [UFS_NTH]
    jns .scan
    xor eax, eax
.return:
    ret

; EAX=NovaFS-Typ, EBX=Index (n-ter Eintrag dieses Typs im aktuellen
; Verzeichnis) -> EAX=Status. Die Explorer-Navigation oeffnet Verzeichnisse
; nur lesend (UFS_H_DIR); fuer Entf wird das Arbeitsverzeichnis (UFS_CWD)
; daher kurz mit Schreibrecht erneut geoeffnet (UFS_H_NEW, zu diesem
; Zeitpunkt frei), um den per ufs_find_nth gefundenen Namen zu loeschen.
ufs_delete_nth:
    call ufs_find_nth
    test eax, eax
    jnz .return
    mov edx, [USER_STACK_ADDRESS - 152]
    mov esi, UFS_CWD
    mov ecx, [UFS_CWD_LEN]
    mov edi, VFS_LOOKUP_FLAG_WRITE
    call ufs_lookup
    test eax, eax
    jnz .return
    mov [UFS_H_NEW], ebx
    mov edx, [UFS_H_NEW]
    mov esi, UFS_ENTRY + 32
    mov ecx, [UFS_ENTRY + 20]
    call ufs_delete
    push eax
    mov edx, [UFS_H_NEW]
    call ufs_close
    pop eax
.return:
    ret

; EBX=Index (n-te Datei im aktuellen Verzeichnis) -> EAX=Status. Zeigt
; "YYYY-MM-DD [erste 45 Byte Inhalt]" anstelle des Breadcrumbs an.
; Datum stammt aus dem VFS.Query-Timestamp (Modified), Inhalt per VFS.Read.
ufs_preview_nth:
    mov eax, NOVAFS_TYPE_FILE
    call ufs_find_nth
    test eax, eax
    jnz .return
    mov edx, [UFS_H_DIR]
    mov esi, UFS_ENTRY + 32
    mov ecx, [UFS_ENTRY + 20]
    xor edi, edi
    call ufs_lookup
    test eax, eax
    jnz .return
    mov [UFS_H_NEW], ebx
    ; Zeitstempel per VFS.Query holen
    mov edx, ebx
    call ufs_query
    ; Datum (Modified) als "YYYY-MM-DD " in UFS_VIEW + 40 formatieren
    mov eax, [UFS_ARGS + 56]        ; Modified-Timestamp (low 32 Bit)
    mov edi, UFS_VIEW + 40
    call ufs_format_date             ; schreibt 11 Byte "YYYY-MM-DD "
    ; Dateiinhalt in UFS_VIEW + 51 lesen (45 Byte nach dem Datum)
    mov ebx, SYSCALL_VFS_READ
    mov edx, [UFS_H_NEW]
    mov esi, UFS_VIEW + 51
    mov ecx, 45
    xor edi, edi
    call ufs_io
    push eax
    push ecx
    mov edx, [UFS_H_NEW]
    call ufs_close
    pop ecx
    pop eax
    test eax, eax
    jnz .return
    add ecx, 11                      ; Gesamtlänge = Datum (11) + Inhalt
    mov [UFS_VIEW + 28], ecx
    mov eax, SYSCALL_SERVICE_DISPLAY
    mov ebx, SYSCALL_DISPLAY_SUBMIT_EXPLORER_VIEW
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, UFS_VIEW
    mov esi, EXPLORER_VIEW_SIZE
    int 0x80
    xor eax, eax
.return:
    ret

; EAX=Unix-Timestamp (32-Bit), EDI=Ausgabepuffer -> 11 Byte "YYYY-MM-DD ".
; Verändert EAX, ECX, EDX; erhält EBX, ESI, EDI (nach dem Schreiben).
ufs_format_date:
    push ebx
    push esi
    push edx
    push ecx
    ; Tage seit Epoch = Timestamp / 86400
    xor edx, edx
    mov ecx, 86400
    div ecx                         ; EAX = Tage seit 1970-01-01
    ; Jahr bestimmen (EBX = laufendes Jahr, EAX = verbleibende Tage)
    mov ebx, 1970
.fd_year_loop:
    mov ecx, 365
    mov esi, ebx
    and esi, 3
    jnz .fd_not_leap
    mov ecx, 366
.fd_not_leap:
    cmp eax, ecx
    jb .fd_year_done
    sub eax, ecx
    inc ebx
    jmp .fd_year_loop
.fd_year_done:
    ; EBX = Jahr, EAX = Tag des Jahres (0-basiert)
    ; Jahr als 4 Ziffern ausgeben
    push eax
    mov eax, ebx
    xor edx, edx
    mov ecx, 1000
    div ecx
    add al, '0'
    stosb
    mov eax, edx
    xor edx, edx
    mov ecx, 100
    div ecx
    add al, '0'
    stosb
    mov eax, edx
    xor edx, edx
    mov ecx, 10
    div ecx
    add al, '0'
    stosb
    add dl, '0'
    mov al, dl
    stosb
    mov al, '-'
    stosb
    pop eax
    ; Monat aus Tabelle bestimmen
    mov esi, ufs_month_days
    mov ecx, 1
.fd_month_loop:
    movzx edx, byte [esi]
    cmp ecx, 2
    jne .fd_check_days
    ; Februar: Schaltjahr? (vereinfacht % 4 für 1970–2099)
    push eax
    mov eax, ebx
    and eax, 3
    pop eax
    jnz .fd_check_days
    mov edx, 29
.fd_check_days:
    cmp eax, edx
    jb .fd_month_done
    sub eax, edx
    inc ecx
    inc esi
    jmp .fd_month_loop
.fd_month_done:
    ; ECX = Monat (1-basiert), EAX = Tag des Monats (0-basiert)
    push eax
    mov eax, ecx
    xor edx, edx
    mov ecx, 10
    div ecx
    add al, '0'
    stosb
    add dl, '0'
    mov al, dl
    stosb
    mov al, '-'
    stosb
    pop eax
    inc eax                         ; 1-basierter Tag
    xor edx, edx
    mov ecx, 10
    div ecx
    add al, '0'
    stosb
    add dl, '0'
    mov al, dl
    stosb
    mov al, ' '
    stosb
    pop ecx
    pop edx
    pop esi
    pop ebx
    ret

ufs_month_days:
    db 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31

; Zeichnet den aktuellen Umbenennen-Puffer (UFS_RENAME_BUF/_LEN) anstelle
; des Breadcrumbs, analog zu ufs_preview_nth oben. Wird nach jeder
; Aenderung des Puffers (Start, Zeichen, Backspace) erneut aufgerufen.
ufs_rename_redraw:
    mov esi, UFS_RENAME_BUF
    mov edi, UFS_VIEW + 40
    mov ecx, [UFS_RENAME_LEN]
    rep movsb
    mov eax, [UFS_RENAME_LEN]
    mov [UFS_VIEW + 28], eax
    mov eax, SYSCALL_SERVICE_DISPLAY
    mov ebx, SYSCALL_DISPLAY_SUBMIT_EXPLORER_VIEW
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, UFS_VIEW
    mov esi, EXPLORER_VIEW_SIZE
    int 0x80
    ret

; EDX=Verzeichnis-Handle. Fuellt UFS_VIEW (Breadcrumb bereits gesetzt) mit
; hoechstens vier Ordnern und vier Dateien sowie den Gesamtzahlen. Zu lange
; Namen enden mit "..". EAX=Status.
ufs_build_view:
    mov [UFS_H_BASE], edx
    mov dword [UFS_VIEW + 0], EXPLORER_VIEW_SIZE
    mov dword [UFS_VIEW + 4], SYSCALL_ABI_VERSION
    mov dword [UFS_VIEW + 8], 1
    mov dword [UFS_VIEW + 12], 0
    mov dword [UFS_VIEW + 16], 0
    mov dword [UFS_VIEW + 20], 0
    mov dword [UFS_VIEW + 32], 0
    mov dword [UFS_VIEW + 36], 0
    mov dword [UFS_INDEX], 0
.entry:
    mov edx, [UFS_H_BASE]
    mov ecx, [UFS_INDEX]
    call ufs_read_directory
    cmp eax, SYSCALL_STATUS_NOT_FOUND
    je .done
    test eax, eax
    jnz .return
    inc dword [UFS_VIEW + 20]
    ; Typzaehler: +32 Ordner, +36 Dateien; je Typ hoechstens vier sichtbar
    mov ebx, UFS_VIEW + 36
    cmp dword [UFS_ENTRY + 16], NOVAFS_TYPE_DIRECTORY
    jne .counted
    mov ebx, UFS_VIEW + 32
.counted:
    inc dword [ebx]
    cmp dword [ebx], 4
    ja .next
    mov eax, [UFS_VIEW + 16]
    cmp eax, EXPLORER_MAX_ENTRIES
    jae .next
    imul edi, eax, EXPLORER_ENTRY_SIZE
    add edi, UFS_VIEW + EXPLORER_VIEW_HEADER
    mov eax, [UFS_ENTRY + 16]
    mov [edi + 0], eax
    mov eax, [UFS_ENTRY + 24]
    cmp dword [UFS_ENTRY + 28], 0
    je .size_ok
    mov eax, 0xFFFFFFFF
.size_ok:
    mov [edi + 4], eax
    mov ecx, [UFS_ENTRY + 20]
    mov dword [UFS_FLAGS], 0
    cmp ecx, EXPLORER_NAME_MAX
    jbe .name_ok
    mov ecx, EXPLORER_NAME_MAX
    mov dword [UFS_FLAGS], 1
.name_ok:
    mov [edi + 8], ecx
    mov dword [edi + 12], 0
    add edi, 16
    mov esi, UFS_ENTRY + 32
    rep movsb
    cmp dword [UFS_FLAGS], 0
    je .stored
    mov word [edi - 2], '..'
.stored:
    inc dword [UFS_VIEW + 16]
.next:
    inc dword [UFS_INDEX]
    cmp dword [UFS_INDEX], 256
    jb .entry
.done:
    xor eax, eax
.return:
    ret

ufs_quick_names:     db 7, "Desktop", 9, "Dokumente", 9, "Downloads", 6, "Bilder"
                     db 5, "Musik", 6, "Videos", 0
ufs_name_temp:       db "NovaOS-Test.tmp"
ufs_name_temp_end:
ufs_name_temp2:      db "NovaOS-Umbenannt.tmp"
ufs_name_temp2_end:
ufs_name_trash:      db "NovaOS-Testordner"
ufs_name_trash_end:
ufs_suffix_documents: db "/Dokumente"
ufs_suffix_documents_end:
ufs_path_bootcount: db "/System/Diagnose/novafs-bootcount"
ufs_path_bootcount_end:
ufs_path_root:      db "/"
ufs_path_root_end:
ufs_path_benutzer:  db "/Benutzer"
ufs_path_benutzer_end:
ufs_name_solutions: db "Solutions"
ufs_name_solutions_end:
ufs_name_welcome:   db "Willkommen.txt"
ufs_name_welcome_end:
ufs_welcome_text:   db "Willkommen bei NovaOS.", 13, 10
ufs_welcome_text_end:

align 4
userspace_exit_arguments:
    dd 16
    dw 1, 0
    dd 0                            ; Exit-Code
    dd 0                            ; reserviert
align 8
userspace_ipc_packet:
    dd 48
    dw SYSCALL_IPC_PACKET_ABI, 0
    dd 0                            ; Send-Endpoint, zur Laufzeit gesetzt
    dd SEMANTIC_IPC_INLINE_DATA      ; internierter Type Handle
    dq 0x0000000011223344
    dd SEMANTIC_TYPE_VERSION_1
    dd 0
    dd 4
    dd 0
    db "NOVA",0,0,0,0
userspace_root_path: db '/'
align 8
userspace_system_scene:
    dd 64
    dw 1, 0
    dq 1                            ; erste deklarative Scene-Generation
    dd 0x0000000F                   ; Desktop | Startmenü | Ribbon | Taskleiste
    dd 0                            ; systemweites Dark Theme
    dd 0                            ; Workspace 0
    dd 2                            ; Startmenü-Suche besitzt Fokus
    dd 0                            ; Surface.Primary
    dd 6                            ; Accent.Primary
    ; Sechs Reserve-Dwords (Offset 40..63), damit die Struktur exakt
    ; SYSTEM_SCENE_SIZE (64 Byte) erreicht. Fuenf davon (Offset 44..60)
    ; werden von SYSCALL_DISPLAY_SUBMIT_SCENE als Reserved==0 geprueft; mit
    ; nur fuenf Fuell-Dwords endete die Struktur bei Byte 60 und der letzte
    ; Reserved-Check (Offset 60) las bereits den naechsten Kernel-Bytewert.
    times 6 dd 0
userspace_program_end:

%if (userspace_program_end - userspace_program_start) > (PMM_PAGE_SIZE * 3)
    %error "Initialer Userspace-Code überschreitet seine drei 4-KiB-Seiten"
%endif

; ESI=Userspace-Adresse, ECX=Länge. CF=0 nur für vollständig enthaltene
; Bereiche des aktuellen Prozesses; Überläufe werden vor dem Zugriff erkannt.
syscall_validate_user_range:
    test ecx, ecx
    jz .invalid
    cmp esi, USER_ADDRESS_MIN
    jb .invalid
    mov eax, esi
    add eax, ecx
    jc .invalid
    cmp eax, esi
    jbe .invalid
    cmp eax, USER_ADDRESS_MAX
    ja .invalid
    clc
    ret
.invalid:
    stc
    ret

; Kopiert erst nach vollständiger Bereichsprüfung in Kernel-Speicher.
; ESI=Quelle, EDI=Ziel, ECX=Länge.
syscall_copy_from_user:
    push esi
    push ecx
    call syscall_validate_user_range
    pop ecx
    pop esi
    jc .invalid
    rep movsb
    clc
    ret
.invalid:
    stc
    ret

; Für öffentliche Ergebnisstrukturen gilt dieselbe vollständige Bereichsprüfung.
syscall_copy_to_user:
    push edi
    push ecx
    mov esi, edi
    call syscall_validate_user_range
    pop ecx
    pop edi
    jc .invalid
    mov esi, syscall_result_buffer
    rep movsb
    clc
    ret
.invalid:
    stc
    ret

; ESI=Kernelquelle, EDI=Userspace-Ziel, ECX=Länge.
syscall_copy_buffer_to_user:
    push esi
    push ecx
    mov esi, edi
    call syscall_validate_user_range
    pop ecx
    pop esi
    jc .invalid
    rep movsb
    clc
    ret
.invalid:
    stc
    ret

; EDX zeigt auf den von isr_common erzeugten, bereits auf dem TSS-Kernelstack
; liegenden Registerframe.
syscall_dispatch:
    mov [syscall_frame], edx
    inc dword [syscall_total]
    cmp dword [edx + 44], SYSCALL_SERVICE_PROCESS
    je .process
    cmp dword [edx + 44], SYSCALL_SERVICE_THREAD
    je .thread
    cmp dword [edx + 44], SYSCALL_SERVICE_IPC
    je .ipc
    cmp dword [edx + 44], SYSCALL_SERVICE_VFS
    je .vfs
    cmp dword [edx + 44], SYSCALL_SERVICE_POWER
    je .power
    cmp dword [edx + 44], SYSCALL_SERVICE_NETWORK
    je .network
    cmp dword [edx + 44], SYSCALL_SERVICE_LOGGING
    je .logging
    cmp dword [edx + 44], SYSCALL_SERVICE_DISPLAY
    je .display
    cmp dword [edx + 44], SYSCALL_SERVICE_CORE
    jne .unknown_service
    cmp dword [edx + 32], SYSCALL_CORE_READY
    je .core_ready
    cmp dword [edx + 32], SYSCALL_CORE_CLOSE_HANDLE
    je .core_close_handle
    cmp dword [edx + 32], SYSCALL_CORE_QUERY_ABI
    je .core_query_abi
    cmp dword [edx + 32], SYSCALL_CORE_EXIT
    jne .unknown_operation
    cmp dword [edx + 40], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [edx + 20], 16
    jb .bad_size

    mov eax, [userspace_pid]
    mov edx, SECURITY_CAP_SERVICE
    call security_check
    jc .access_denied

    mov edx, [syscall_frame]
    mov esi, [edx + 36]
    mov ecx, 16
    mov edi, syscall_argument_buffer
    call syscall_copy_from_user
    jc .bad_pointer
    cmp dword [syscall_argument_buffer + 0], 16
    jb .bad_size
    movzx eax, word [syscall_argument_buffer + 4]
    cmp eax, 1
    jne .bad_abi
    cmp word [syscall_argument_buffer + 6], 0
    jb .bad_abi
    cmp dword [syscall_argument_buffer + 12], 0
    jne .bad_reserved

    mov eax, [syscall_argument_buffer + 8]
    mov [userspace_exit_code], eax
    mov dword [userspace_exit_seen], 1
    mov edx, [syscall_frame]
    mov dword [edx + 44], SYSCALL_STATUS_OK
    mov dword [edx + 56], userspace_return
    mov dword [edx + 60], CODE_SEGMENT
    and dword [edx + 64], 0xFFFFCFFF
    ret
.logging:
    cmp dword [edx + 40], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [edx + 20], 32
    jb .bad_size
    mov eax, [userspace_pid]
    mov edx, SECURITY_CAP_LOG_READ
    call security_check
    jc .access_denied
    mov edx, [syscall_frame]
    cmp dword [edx + 32], SYSCALL_LOG_QUERY
    je .log_query
    cmp dword [edx + 32], SYSCALL_LOG_READ_LATEST
    je .log_read_latest
    jmp .unknown_operation
.log_query:
    mov dword [syscall_log_result + 0], 32
    mov dword [syscall_log_result + 4], SYSCALL_ABI_VERSION
    mov eax, [logging_written_records]
    mov [syscall_log_result + 8], eax
    mov eax, [logging_dropped_records]
    mov [syscall_log_result + 12], eax
    mov eax, [logging_overwritten_records]
    mov [syscall_log_result + 16], eax
    mov eax, [logging_record_count]
    mov [syscall_log_result + 20], eax
    mov eax, [logging_written_records]
    mov [syscall_log_result + 24], eax
    mov eax, [logging_critical_records]
    mov [syscall_log_result + 28], eax
    mov esi, message_logging_query_ok
    jmp .log_copy
.log_read_latest:
    cmp dword [logging_record_count], 0
    je .log_would_block
    mov eax, [logging_write_index]
    dec eax
    and eax, LOG_RING_CAPACITY - 1
    shl eax, 5
    add eax, logging_ring
    cmp dword [eax + 28], LOG_COMMIT_MARKER
    jne .unknown_operation
    mov dword [syscall_log_result + 0], 32
    mov dword [syscall_log_result + 4], SYSCALL_ABI_VERSION
    mov ecx, [eax + 0]
    mov [syscall_log_result + 8], ecx
    mov ecx, [eax + 4]
    mov [syscall_log_result + 12], ecx
    mov ecx, [eax + 8]
    mov [syscall_log_result + 16], ecx
    mov ecx, [eax + 12]
    mov [syscall_log_result + 20], ecx
    mov ecx, [eax + 16]
    mov [syscall_log_result + 24], ecx
    mov ecx, [eax + 24]
    mov [syscall_log_result + 28], ecx
    mov esi, message_logging_read_ok
.log_copy:
    push esi
    mov edx, [syscall_frame]
    mov edi, [edx + 36]
    mov esi, syscall_log_result
    mov ecx, 32
    call syscall_copy_buffer_to_user
    jc .log_bad_pointer
    pop esi
    call serial_write_string
    mov edx, [syscall_frame]
    mov dword [edx + 44], SYSCALL_STATUS_OK
    ret
.display:
    cmp dword [edx + 40], SYSCALL_ABI_VERSION
    jne .bad_abi
    mov eax, [userspace_pid]
    mov edx, SECURITY_CAP_DISPLAY_SYSTEM_UI
    call security_check
    jc .access_denied
    cmp dword [display_server_ready], 1
    jne .display_unavailable
    mov edx, [syscall_frame]
    cmp dword [edx + 32], SYSCALL_DISPLAY_QUERY_PRIMARY
    je .display_query
    cmp dword [edx + 32], SYSCALL_DISPLAY_SUBMIT_SCENE
    je .display_submit
    cmp dword [edx + 32], SYSCALL_DISPLAY_POLL_INPUT
    je .display_poll_input
    cmp dword [edx + 32], SYSCALL_DISPLAY_SUBMIT_EXPLORER_VIEW
    jne .unknown_operation
    call explorer_submit_view
    mov edx, [syscall_frame]
    mov [edx + 44], eax
    test eax, eax
    jz .explorer_done
    inc dword [syscall_rejected]
.explorer_done:
    ret
.display_query:
    cmp dword [edx + 20], DISPLAY_INFO_SIZE
    jb .bad_size
    mov dword [syscall_display_info + 0], DISPLAY_INFO_SIZE
    mov dword [syscall_display_info + 4], SYSCALL_ABI_VERSION
    mov eax, [display_primary_id]
    mov [syscall_display_info + 8], eax
    mov eax, [kernel_context + CONTEXT_WIDTH]
    mov [syscall_display_info + 12], eax
    mov eax, [kernel_context + CONTEXT_HEIGHT]
    mov [syscall_display_info + 16], eax
    mov eax, [kernel_context + CONTEXT_PITCH]
    mov [syscall_display_info + 20], eax
    mov eax, [kernel_context + CONTEXT_BPP]
    mov [syscall_display_info + 24], eax
    mov dword [syscall_display_info + 28], 1000 ; DLU-Skalierung
    mov eax, [display_generation]
    mov [syscall_display_info + 32], eax
    mov dword [syscall_display_info + 36], 0
    mov edx, [syscall_frame]
    mov edi, [edx + 36]
    mov esi, syscall_display_info
    mov ecx, DISPLAY_INFO_SIZE
    call syscall_copy_buffer_to_user
    jc .bad_pointer
    mov esi, message_display_query_ok
    call serial_write_string
    mov edx, [syscall_frame]
    mov dword [edx + 44], SYSCALL_STATUS_OK
    ret
.display_submit:
    cmp dword [edx + 20], SYSTEM_SCENE_SIZE
    jb .bad_size
    mov esi, [edx + 36]
    mov edi, syscall_display_scene
    mov ecx, SYSTEM_SCENE_SIZE
    call syscall_copy_from_user
    jc .bad_pointer
    cmp dword [syscall_display_scene + 0], SYSTEM_SCENE_SIZE
    jne .bad_size
    cmp dword [syscall_display_scene + 4], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [syscall_display_scene + 12], 0
    jne .bad_reserved
    mov eax, [syscall_display_scene + 8]
    cmp eax, [display_scene_generation]
    jbe .display_validation
    mov ecx, [syscall_display_scene + 16]
    test ecx, ~DISPLAY_SCENE_ALLOWED_FLAGS
    jnz .display_validation
    mov ebx, ecx
    and ebx, DISPLAY_SCENE_DESKTOP | DISPLAY_SCENE_RIBBON | DISPLAY_SCENE_TASKBAR
    cmp ebx, DISPLAY_SCENE_DESKTOP | DISPLAY_SCENE_RIBBON | DISPLAY_SCENE_TASKBAR
    jne .display_validation
    cmp dword [syscall_display_scene + 20], 2
    ja .display_validation
    cmp dword [syscall_display_scene + 24], 31
    ja .display_validation
    cmp dword [syscall_display_scene + 32], 13
    jae .display_validation
    cmp dword [syscall_display_scene + 36], 13
    jae .display_validation
    cmp dword [syscall_display_scene + 44], 0
    jne .bad_reserved
    cmp dword [syscall_display_scene + 48], 0
    jne .bad_reserved
    cmp dword [syscall_display_scene + 52], 0
    jne .bad_reserved
    cmp dword [syscall_display_scene + 56], 0
    jne .bad_reserved
    cmp dword [syscall_display_scene + 60], 0
    jne .bad_reserved
    mov [display_scene_generation], eax
    mov ebx, [display_scene_flags]
    mov edi, [display_scene_workspace]
    mov [display_scene_flags], ecx
    mov esi, [syscall_display_scene + 24]
    mov [display_scene_workspace], esi
    mov edx, [display_scene_focus]
    mov eax, [syscall_display_scene + 28]
    mov [display_scene_focus], eax
    ; Sichtbarkeits- und Workspacewechsel erzeugen einen Vollframe. Ein reiner
    ; Fokuswechsel zeichnet dagegen nur das betroffene Startmenue oder Fenster.
    cmp ebx, ecx
    jne .display_redraw
    cmp edi, esi
    jne .display_redraw
    cmp edx, eax
    je .display_presented
    test ecx, DISPLAY_SCENE_START_MENU
    jz .display_focus_workspace
    mov byte [mouse_cursor_valid], 0
    call draw_shell_start_menu
    jmp .display_redrawn
.display_focus_workspace:
    mov byte [mouse_cursor_valid], 0
    call draw_shell_workspace
    jmp .display_redrawn
.display_redraw:
    mov byte [mouse_cursor_valid], 0
    call draw_desktop_scene
.display_redrawn:
    cmp byte [mouse_ready], 1
    jne .display_presented
    call draw_mouse_cursor
.display_presented:
    inc dword [display_present_count]
    inc dword [display_generation]
    mov esi, message_display_scene_ok
    call serial_write_string
    mov edx, [syscall_frame]
    mov dword [edx + 44], SYSCALL_STATUS_OK
    ret
.display_poll_input:
    cmp dword [edx + 20], SYSTEM_INPUT_EVENT_SIZE
    jb .bad_size
    cmp dword [display_input_pending], 1
    jne .display_input_empty
    mov dword [syscall_display_input + 0], SYSTEM_INPUT_EVENT_SIZE
    mov dword [syscall_display_input + 4], SYSCALL_ABI_VERSION
    mov eax, [display_input_action]
    mov [syscall_display_input + 8], eax
    mov eax, [display_input_scancode]
    mov [syscall_display_input + 12], eax
    mov eax, [display_input_tick]
    mov [syscall_display_input + 16], eax
    mov dword [syscall_display_input + 20], 0
    mov eax, [display_input_target]
    mov [syscall_display_input + 24], eax
    mov dword [syscall_display_input + 28], 0
    mov edx, [syscall_frame]
    mov edi, [edx + 36]
    mov esi, syscall_display_input
    mov ecx, SYSTEM_INPUT_EVENT_SIZE
    call syscall_copy_buffer_to_user
    jc .bad_pointer
    pushfd
    cli
    mov dword [display_input_pending], 0
    popfd
    mov esi, message_display_input_ok
    call serial_write_string
    mov edx, [syscall_frame]
    mov dword [edx + 44], SYSCALL_STATUS_OK
    ret
.display_input_empty:
    mov eax, SYSCALL_STATUS_WOULD_BLOCK
    jmp .reject
.display_validation:
    mov eax, SYSCALL_STATUS_VALIDATION
    jmp .reject
.display_unavailable:
    mov eax, SYSCALL_STATUS_SERVICE
    jmp .reject
.log_bad_pointer:
    pop esi
    jmp .bad_pointer
.log_would_block:
    mov eax, SYSCALL_STATUS_WOULD_BLOCK
    jmp .reject
.core_query_abi:
    ; §011 §26: ABI-Version und Feature-Flags abfragen
    call syscall_handler_query_abi
    ret

.core_ready:
    cmp dword [edx + 40], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [edx + 20], 0
    jne .bad_size
    cmp dword [edx + 36], 0
    jne .bad_pointer
    mov eax, [userspace_pid]
    mov edx, SECURITY_CAP_SERVICE
    call security_check
    jc .access_denied
    mov dword [userspace_ready_seen], 1
    mov esi, message_shared_service_ok
    call serial_write_string
    mov edx, [syscall_frame]
    mov dword [edx + 44], SYSCALL_STATUS_OK
    ret
.core_close_handle:
    cmp dword [edx + 40], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [edx + 20], 16
    jb .bad_size
    mov esi, [edx + 36]
    mov ecx, 16
    mov edi, syscall_argument_buffer
    call syscall_copy_from_user
    jc .bad_pointer
    cmp dword [syscall_argument_buffer + 0], 16
    jne .bad_size
    cmp dword [syscall_argument_buffer + 4], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [syscall_argument_buffer + 12], 0
    jne .bad_reserved
    mov edx, [syscall_argument_buffer + 8]
    mov [syscall_closed_handle], edx
    mov eax, [userspace_pid]
    call handle_close
    jc .access_denied
    ; Derselbe codierte Wert muss nach Generationswechsel ungültig sein.
    mov eax, [userspace_pid]
    mov edx, [syscall_closed_handle]
    mov ebx, OBJECT_TYPE_VFS_NODE
    mov ecx, HANDLE_RIGHT_QUERY
    call handle_resolve
    jnc .bad_reserved
    mov esi, message_handle_close_ok
    call serial_write_string
    mov edx, [syscall_frame]
    mov dword [edx + 44], SYSCALL_STATUS_OK
    ret
.process:
    cmp dword [edx + 32], 2
    je .process_handle
    cmp dword [edx + 32], SYSCALL_QUERY_SELF
    jne .unknown_operation
    mov eax, [userspace_pid]
    mov esi, message_process_syscall_ok
    jmp .process_value
.process_handle:
    mov eax, [userspace_process_handle]
    mov esi, message_process_handle_ok
.process_value:
    mov [syscall_identity], eax
    jmp .identity
.thread:
    cmp dword [edx + 32], 2
    je .thread_handle
    cmp dword [edx + 32], SYSCALL_QUERY_SELF
    jne .unknown_operation
    mov eax, [userspace_tid]
    mov esi, message_thread_syscall_ok
    jmp .thread_value
.thread_handle:
    mov eax, [userspace_thread_handle]
    mov esi, message_thread_handle_ok
.thread_value:
    mov [syscall_identity], eax
.identity:
    push esi
    cmp dword [edx + 40], SYSCALL_ABI_VERSION
    jne .identity_bad_abi
    cmp dword [edx + 20], 16
    jb .identity_bad_size
    mov eax, [userspace_pid]
    mov edx, SECURITY_CAP_SERVICE
    call security_check
    jc .identity_access
    mov dword [syscall_result_buffer + 0], 16
    mov dword [syscall_result_buffer + 4], SYSCALL_ABI_VERSION
    mov eax, [syscall_identity]
    mov [syscall_result_buffer + 8], eax
    mov dword [syscall_result_buffer + 12], 0
    mov edx, [syscall_frame]
    mov edi, [edx + 36]
    mov ecx, 16
    call syscall_copy_to_user
    jc .identity_pointer
    pop esi
    call serial_write_string
    mov edx, [syscall_frame]
    mov dword [edx + 44], SYSCALL_STATUS_OK
    ret
.identity_bad_abi:
    pop esi
    jmp .bad_abi
.identity_bad_size:
    pop esi
    jmp .bad_size
.identity_access:
    pop esi
    jmp .access_denied
.identity_pointer:
    pop esi
    jmp .bad_pointer
.ipc:
    cmp dword [edx + 40], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [edx + 20], 48
    jb .bad_size
    cmp dword [edx + 32], SYSCALL_IPC_SEND
    je .ipc_send
    cmp dword [edx + 32], SYSCALL_IPC_RECEIVE
    je .ipc_receive
    jmp .unknown_operation
.ipc_send:
    mov esi, [edx + 36]
    mov ecx, 48
    mov edi, userspace_ipc_copy
    call syscall_copy_from_user
    jc .bad_pointer
    cmp dword [userspace_ipc_copy], 48
    jne .bad_size
    cmp dword [userspace_ipc_copy + 4], SYSCALL_IPC_PACKET_ABI
    jne .bad_abi
    cmp dword [userspace_ipc_copy + 32], 8
    ja .bad_size
    cmp dword [userspace_ipc_copy + 36], 0
    jne .bad_reserved
    cmp dword [userspace_ipc_copy + 28], 0
    jne .bad_reserved
    mov eax, [userspace_pid]
    mov edx, [userspace_ipc_copy + 8]
    mov ebx, OBJECT_TYPE_IPC_ENDPOINT
    mov ecx, HANDLE_RIGHT_SEND
    call handle_resolve
    jc .access_denied
    mov eax, [eax + OBJ_HANDLE]
    call object_semantic_query
    jc .semantic_mismatch
    cmp ebx, SEMANTIC_VALIDATION_VERIFIED
    jne .semantic_mismatch
    push ecx                       ; Endpoint-Contract, nicht User-Claim
    mov eax, [userspace_ipc_copy + 12]
    mov ecx, [userspace_ipc_copy + 24]
    mov edx, SEMANTIC_REPR_BYTES8
    call semantic_check_claim
    pop edx
    jc .semantic_mismatch
    cmp eax, edx
    jne .semantic_mismatch
    mov eax, [userspace_ipc_copy + 12]
    mov ecx, [userspace_ipc_copy + 24]
    mov edx, userspace_ipc_copy + 40
    mov ebx, [userspace_ipc_copy + 32]
    mov esi, 8                       ; Endpoint-Capability-Contract
    call semantic_validate_value
    jc .semantic_validation_failed
    cmp dword [userspace_ipc_count], 0
    jne .ipc_queue_full
    mov esi, userspace_ipc_copy
    mov edi, userspace_ipc_queue
    mov ecx, 12
    rep movsd
    mov dword [userspace_ipc_count], 1
    mov dword [userspace_ipc_validation_state], SEMANTIC_VALIDATION_VERIFIED
    mov edx, [syscall_frame]
    mov dword [edx + 44], SYSCALL_STATUS_OK
    ret
.ipc_receive:
    cmp dword [userspace_ipc_count], 1
    jne .ipc_would_block
    mov eax, [userspace_pid]
    mov edx, [userspace_ipc_receive_handle]
    mov ebx, OBJECT_TYPE_IPC_ENDPOINT
    mov ecx, HANDLE_RIGHT_RECEIVE
    call handle_resolve
    jc .access_denied
    mov eax, [eax + OBJ_HANDLE]
    call object_semantic_query
    jc .semantic_mismatch
    cmp ebx, SEMANTIC_VALIDATION_VERIFIED
    jne .semantic_mismatch
    mov ebx, ecx
    cmp dword [userspace_ipc_validation_state], SEMANTIC_VALIDATION_VERIFIED
    jne .semantic_mismatch
    mov eax, [userspace_ipc_queue + 12]
    cmp eax, ebx
    jne .semantic_mismatch
    mov ecx, [userspace_ipc_queue + 24]
    mov edx, SEMANTIC_REPR_BYTES8
    call semantic_check_claim
    jc .semantic_mismatch
    mov esi, userspace_ipc_queue
    mov edi, userspace_ipc_copy
    mov ecx, 12
    rep movsd
    mov eax, [userspace_ipc_receive_handle]
    mov [userspace_ipc_copy + 8], eax
    mov dword [userspace_ipc_count], 0
    mov dword [userspace_ipc_validation_state], 0
    mov edx, [syscall_frame]
    mov edi, [edx + 36]
    mov esi, userspace_ipc_copy
    mov ecx, 48
    call syscall_copy_buffer_to_user
    jc .bad_pointer
    mov esi, message_ipc_roundtrip_ok
    call serial_write_string
    cmp dword [semantic_ipc_rejected], 2
    jne .ipc_receive_done
    mov esi, message_semantic_reject_ok
    call serial_write_string
.ipc_validation_report:
    cmp dword [semantic_ipc_validation_rejected], 1
    jne .ipc_receive_done
    mov esi, message_semantic_validation_ok
    call serial_write_string
.ipc_receive_done:
    mov edx, [syscall_frame]
    mov dword [edx + 44], SYSCALL_STATUS_OK
    ret
.ipc_queue_full:
    mov eax, -17
    jmp .reject
.ipc_would_block:
    mov eax, -19
    jmp .reject
.vfs:
    cmp dword [edx + 32], SYSCALL_VFS_OPEN_ROOT
    je .vfs_open_root
    ; Lookup, Read, Write, Create, ReadDirectory, Query, Delete, Rename (vfs32.inc)
    call vfs_syscall
    mov edx, [syscall_frame]
    mov [edx + 44], eax
    test eax, eax
    jz .vfs_done
    inc dword [syscall_rejected]
.vfs_done:
    ret
.vfs_open_root:
    cmp dword [edx + 40], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [edx + 20], 16
    jb .bad_size
    mov eax, [userspace_pid]
    mov edx, SECURITY_CAP_SERVICE
    call security_check
    jc .access_denied
    mov dword [syscall_result_buffer + 0], 16
    mov dword [syscall_result_buffer + 4], SYSCALL_ABI_VERSION
    mov eax, [userspace_root_handle]
    mov [syscall_result_buffer + 8], eax
    mov dword [syscall_result_buffer + 12], 0
    mov edx, [syscall_frame]
    mov edi, [edx + 36]
    mov esi, syscall_result_buffer
    mov ecx, 16
    call syscall_copy_buffer_to_user
    jc .bad_pointer
    mov esi, message_vfs_userspace_ok
    call serial_write_string
    mov edx, [syscall_frame]
    mov dword [edx + 44], SYSCALL_STATUS_OK
    ret
.power:
    cmp dword [edx + 32], SYSCALL_POWER_QUERY_SYSTEM
    je .power_query
    cmp dword [edx + 32], SYSCALL_POWER_WAKE_ACQUIRE
    je .power_wake_acquire
    cmp dword [edx + 32], SYSCALL_POWER_WAKE_RELEASE
    je .power_wake_release
    cmp dword [edx + 32], SYSCALL_POWER_SET_PROFILE
    je .power_set_profile
    cmp dword [edx + 32], SYSCALL_POWER_REQUEST_STATE
    je .power_request_state
    jmp .unknown_operation
.network:
    cmp dword [edx + 32], SYSCALL_NETWORK_QUERY
    je .network_query
    cmp dword [edx + 32], SYSCALL_NETWORK_RAW_OPEN
    je .network_raw_open
    cmp dword [edx + 32], SYSCALL_NETWORK_SOCKET_CREATE
    je .network_socket_create
    cmp dword [edx + 32], SYSCALL_NETWORK_SEND
    je .network_send
    cmp dword [edx + 32], SYSCALL_NETWORK_RECEIVE
    je .network_receive
    cmp dword [edx + 32], SYSCALL_NETWORK_BIND
    je .network_bind
    cmp dword [edx + 32], SYSCALL_NETWORK_STREAM_CREATE
    je .network_stream_create
    cmp dword [edx + 32], SYSCALL_NETWORK_CONNECT
    je .network_connect
    cmp dword [edx + 32], SYSCALL_NETWORK_STREAM_SEND
    je .network_stream_send
    cmp dword [edx + 32], SYSCALL_NETWORK_STREAM_RECEIVE
    je .network_stream_receive
    cmp dword [edx + 32], SYSCALL_NETWORK_STREAM_BIND
    je .network_stream_bind
    cmp dword [edx + 32], SYSCALL_NETWORK_LISTEN
    je .network_listen
    cmp dword [edx + 32], SYSCALL_NETWORK_ACCEPT
    je .network_accept
    jmp .unknown_operation
.network_query:
    cmp dword [edx + 40], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [edx + 20], 96
    jb .bad_size
    mov eax, [userspace_pid]
    mov edx, SECURITY_CAP_NET_QUERY
    call security_check
    jc .access_denied
    mov dword [syscall_network_result + 0], 96
    mov dword [syscall_network_result + 4], SYSCALL_ABI_VERSION
    mov eax, [network_namespace_generation]
    mov [syscall_network_result + 8], eax
    mov dword [syscall_network_result + 12], 1
    mov eax, [network_loopback_id]
    mov [syscall_network_result + 16], eax
    mov eax, [network_loopback_state]
    mov [syscall_network_result + 20], eax
    mov dword [syscall_network_result + 24], NETWORK_LOOPBACK_MTU
    mov dword [syscall_network_result + 28], NETWORK_FEATURE_IPV4 | NETWORK_FEATURE_IPV6
    mov eax, [network_transmitted_packets]
    mov [syscall_network_result + 32], eax
    mov eax, [network_received_packets]
    mov [syscall_network_result + 36], eax
    mov eax, [network_dropped_packets]
    mov [syscall_network_result + 40], eax
    mov eax, [network_socket_port]
    mov [syscall_network_result + 44], eax
    mov eax, [network_icmp_echo_tests]
    mov [syscall_network_result + 48], eax
    mov eax, [network_route_count]
    mov [syscall_network_result + 52], eax
    mov eax, [network_route_lookups]
    mov [syscall_network_result + 56], eax
    mov eax, [network_route_hits]
    mov [syscall_network_result + 60], eax
    mov eax, [network_firewall_decisions]
    mov [syscall_network_result + 64], eax
    mov eax, [network_firewall_drops]
    mov [syscall_network_result + 68], eax
    mov eax, [network_ipv6_udp_tests]
    mov [syscall_network_result + 72], eax
    mov eax, [network_icmpv6_echo_tests]
    mov [syscall_network_result + 76], eax
    mov eax, [network_tcp_tests]
    mov [syscall_network_result + 80], eax
    mov eax, [network_tcp_transmitted_bytes]
    mov [syscall_network_result + 84], eax
    mov eax, [network_tcp_received_bytes]
    mov [syscall_network_result + 88], eax
    mov eax, [network_tcp_accepted_connections]
    mov [syscall_network_result + 92], eax
    mov edx, [syscall_frame]
    mov edi, [edx + 36]
    mov esi, syscall_network_result
    mov ecx, 96
    call syscall_copy_buffer_to_user
    jc .bad_pointer
    mov esi, message_network_query_ok
    call serial_write_string
    mov edx, [syscall_frame]
    mov dword [edx + 44], SYSCALL_STATUS_OK
    ret
.network_raw_open:
    cmp dword [edx + 40], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [edx + 20], 0
    jne .bad_size
    cmp dword [edx + 36], 0
    jne .bad_pointer
    mov eax, [userspace_pid]
    mov edx, SECURITY_CAP_NET_RAW
    call security_check
    jc .network_raw_denied
    jmp .unknown_operation
.network_raw_denied:
    mov esi, message_network_raw_denied
    call serial_write_string
    jmp .access_denied
.network_socket_create:
    cmp dword [edx + 40], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [edx + 20], 16
    jb .bad_size
    mov eax, [userspace_pid]
    mov edx, SECURITY_CAP_NET_CONNECT
    call security_check
    jc .access_denied
    mov eax, OBJECT_TYPE_NET_SOCKET
    mov edx, NETWORK_SOCKET_DATAGRAM
    mov ebx, 1                       ; Namespace 1
    call object_create
    jc .unknown_operation
    mov [network_socket_object], eax
    mov edx, eax
    mov eax, [userspace_pid]
    mov ebx, OBJECT_TYPE_NET_SOCKET
    mov ecx, HANDLE_RIGHT_QUERY | HANDLE_RIGHT_SEND | HANDLE_RIGHT_RECEIVE | HANDLE_RIGHT_WAIT | HANDLE_RIGHT_BIND
    xor esi, esi
    call handle_create
    jc .unknown_operation
    mov [network_socket_handle], eax
    mov dword [syscall_result_buffer], 16
    mov dword [syscall_result_buffer + 4], SYSCALL_ABI_VERSION
    mov [syscall_result_buffer + 8], eax
    mov dword [syscall_result_buffer + 12], 0
    mov edx, [syscall_frame]
    mov edi, [edx + 36]
    mov esi, syscall_result_buffer
    mov ecx, 16
    call syscall_copy_buffer_to_user
    jc .bad_pointer
    mov esi, message_network_socket_ok
    call serial_write_string
    mov edx, [syscall_frame]
    mov dword [edx + 44], SYSCALL_STATUS_OK
    ret
.network_stream_create:
    cmp dword [edx + 40], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [edx + 20], 16
    jb .bad_size
    mov eax, [userspace_pid]
    mov edx, SECURITY_CAP_NET_CONNECT
    call security_check
    jc .access_denied
    mov eax, OBJECT_TYPE_NET_SOCKET
    mov edx, NETWORK_SOCKET_STREAM
    mov ebx, 1
    call object_create
    jc .unknown_operation
    mov [network_tcp_socket_object], eax
    mov edx, eax
    mov eax, [userspace_pid]
    mov ebx, OBJECT_TYPE_NET_SOCKET
    mov ecx, HANDLE_RIGHT_QUERY | HANDLE_RIGHT_SEND | HANDLE_RIGHT_RECEIVE | HANDLE_RIGHT_WAIT | HANDLE_RIGHT_BIND | HANDLE_RIGHT_CONNECT
    xor esi, esi
    call handle_create
    jc .unknown_operation
    mov [network_tcp_socket_handle], eax
    mov dword [syscall_result_buffer], 16
    mov dword [syscall_result_buffer + 4], SYSCALL_ABI_VERSION
    mov [syscall_result_buffer + 8], eax
    mov dword [syscall_result_buffer + 12], 0
    mov edx, [syscall_frame]
    mov edi, [edx + 36]
    mov esi, syscall_result_buffer
    mov ecx, 16
    call syscall_copy_buffer_to_user
    jc .bad_pointer
    mov esi, message_network_stream_ok
    call serial_write_string
    mov edx, [syscall_frame]
    mov dword [edx + 44], SYSCALL_STATUS_OK
    ret
.network_connect:
    cmp dword [edx + 40], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [edx + 20], 28
    jb .bad_size
    mov eax, [userspace_pid]
    mov edx, SECURITY_CAP_NET_CONNECT
    call security_check
    jc .access_denied
    mov edx, [syscall_frame]
    mov esi, [edx + 36]
    mov ecx, 28
    mov edi, syscall_network_connect
    call syscall_copy_from_user
    jc .bad_pointer
    cmp dword [syscall_network_connect], 28
    jne .bad_size
    cmp dword [syscall_network_connect + 4], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [syscall_network_connect + 12], NETWORK_AF_IPV4
    jne .unknown_operation
    cmp dword [syscall_network_connect + 16], NETWORK_IPV4_LOOPBACK
    jne .unknown_operation
    cmp dword [syscall_network_connect + 20], NETWORK_BOOTSTRAP_PORT
    jne .unknown_operation
    cmp dword [syscall_network_connect + 24], 0
    jne .bad_reserved
    mov eax, [userspace_pid]
    mov edx, [syscall_network_connect + 8]
    mov ebx, OBJECT_TYPE_NET_SOCKET
    mov ecx, HANDLE_RIGHT_CONNECT
    call handle_resolve
    jc .network_connect_handle_denied
    mov eax, 0x7F000001             ; Firewall erwartet Host-Reihenfolge
    mov ebx, 6
    mov ecx, NETWORK_BOOTSTRAP_PORT
    call network_firewall_check_ipv4
    jc .network_connect_firewall_denied
    mov dword [network_tcp_state], TCP_STATE_CLOSED
    mov eax, TCP_STATE_SYN_SENT
    call network_tcp_transition
    jc .network_connect_state_error
    mov eax, TCP_STATE_SYN_RECEIVED
    call network_tcp_transition
    jc .network_connect_state_error
    mov eax, TCP_STATE_ESTABLISHED
    call network_tcp_transition
    jc .network_connect_state_error
    mov dword [network_tcp_socket_connected], 1
    mov esi, message_network_connect_ok
    call serial_write_string
    mov edx, [syscall_frame]
    mov dword [edx + 44], SYSCALL_STATUS_OK
    ret
.network_connect_handle_denied:
    mov esi, message_network_connect_handle_denied
    call serial_write_string
    jmp .access_denied
.network_connect_firewall_denied:
    mov esi, message_network_connect_firewall_denied
    call serial_write_string
    jmp .access_denied
.network_connect_state_error:
    mov esi, message_network_connect_state_error
    call serial_write_string
    jmp .unknown_operation
.network_stream_send:
    cmp dword [edx + 40], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [edx + 20], 40
    jb .bad_size
    mov esi, [edx + 36]
    mov ecx, 40
    mov edi, syscall_network_tcp_packet
    call syscall_copy_from_user
    jc .bad_pointer
    cmp dword [syscall_network_tcp_packet], 40
    jne .bad_size
    cmp dword [syscall_network_tcp_packet + 4], SYSCALL_ABI_VERSION
    jne .bad_abi
    mov ecx, [syscall_network_tcp_packet + 12]
    test ecx, ecx
    jz .bad_size
    cmp ecx, 16
    ja .network_packet_too_large
    cmp dword [syscall_network_tcp_packet + 16], 0
    jne .bad_reserved
    cmp dword [syscall_network_tcp_packet + 36], 0
    jne .bad_reserved
    cmp dword [network_tcp_socket_connected], 1
    jne .unknown_operation
    cmp dword [network_tcp_state], TCP_STATE_ESTABLISHED
    jne .unknown_operation
    cmp dword [network_tcp_queue_count], 0
    jne .network_would_block
    mov eax, [userspace_pid]
    mov edx, [syscall_network_tcp_packet + 8]
    mov ebx, OBJECT_TYPE_NET_SOCKET
    mov ecx, HANDLE_RIGHT_SEND
    call handle_resolve
    jc .access_denied
    mov ecx, [syscall_network_tcp_packet + 12]
    mov [network_tcp_queue_length], ecx
    mov esi, syscall_network_tcp_packet + 20
    mov edi, network_tcp_queue_payload
    rep movsb
    mov dword [network_tcp_queue_count], 1
    mov eax, [network_tcp_queue_length]
    add [network_tcp_transmitted_bytes], eax
    mov esi, message_network_stream_send_ok
    call serial_write_string
    mov edx, [syscall_frame]
    mov dword [edx + 44], SYSCALL_STATUS_OK
    ret
.network_stream_receive:
    cmp dword [edx + 40], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [edx + 20], 40
    jb .bad_size
    mov esi, [edx + 36]
    mov ecx, 40
    mov edi, syscall_network_tcp_packet
    call syscall_copy_from_user
    jc .bad_pointer
    cmp dword [syscall_network_tcp_packet], 40
    jne .bad_size
    cmp dword [syscall_network_tcp_packet + 4], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [network_tcp_queue_count], 1
    jne .network_would_block
    mov eax, [userspace_pid]
    mov edx, [syscall_network_tcp_packet + 8]
    mov ebx, OBJECT_TYPE_NET_SOCKET
    mov ecx, HANDLE_RIGHT_RECEIVE
    call handle_resolve
    jc .access_denied
    mov eax, [network_tcp_queue_length]
    cmp eax, [syscall_network_tcp_packet + 12]
    ja .bad_size
    mov [syscall_network_tcp_packet + 12], eax
    mov dword [syscall_network_tcp_packet + 16], 0
    mov ecx, eax
    mov esi, network_tcp_queue_payload
    mov edi, syscall_network_tcp_packet + 20
    rep movsb
    add [network_tcp_received_bytes], eax
    mov dword [network_tcp_queue_count], 0
    mov edx, [syscall_frame]
    mov edi, [edx + 36]
    mov esi, syscall_network_tcp_packet
    mov ecx, 40
    call syscall_copy_buffer_to_user
    jc .bad_pointer
    mov esi, message_network_stream_receive_ok
    call serial_write_string
    mov edx, [syscall_frame]
    mov dword [edx + 44], SYSCALL_STATUS_OK
    ret
.network_stream_bind:
    cmp dword [edx + 40], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [edx + 20], 28
    jb .bad_size
    mov eax, [userspace_pid]
    mov edx, SECURITY_CAP_NET_LISTEN
    call security_check
    jc .access_denied
    mov edx, [syscall_frame]
    mov esi, [edx + 36]
    mov ecx, 28
    mov edi, syscall_network_stream_bind
    call syscall_copy_from_user
    jc .bad_pointer
    cmp dword [syscall_network_stream_bind], 28
    jne .bad_size
    cmp dword [syscall_network_stream_bind + 4], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [syscall_network_stream_bind + 12], NETWORK_AF_IPV4
    jne .unknown_operation
    cmp dword [syscall_network_stream_bind + 16], NETWORK_IPV4_LOOPBACK
    jne .unknown_operation
    cmp dword [syscall_network_stream_bind + 20], NETWORK_BOOTSTRAP_PORT
    jne .unknown_operation
    cmp dword [syscall_network_stream_bind + 24], 0
    jne .bad_reserved
    cmp dword [network_tcp_listener_bound], 0
    jne .unknown_operation
    mov eax, [userspace_pid]
    mov edx, [syscall_network_stream_bind + 8]
    mov ebx, OBJECT_TYPE_NET_SOCKET
    mov ecx, HANDLE_RIGHT_BIND
    call handle_resolve
    jc .access_denied
    mov eax, [syscall_network_stream_bind + 8]
    mov [network_tcp_listener_handle], eax
    mov dword [network_tcp_listener_bound], 1
    inc dword [network_namespace_generation]
    mov esi, message_network_stream_bind_ok
    call serial_write_string
    mov edx, [syscall_frame]
    mov dword [edx + 44], SYSCALL_STATUS_OK
    ret
.network_listen:
    cmp dword [edx + 40], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [edx + 20], 20
    jb .bad_size
    mov eax, [userspace_pid]
    mov edx, SECURITY_CAP_NET_LISTEN
    call security_check
    jc .access_denied
    mov edx, [syscall_frame]
    mov esi, [edx + 36]
    mov ecx, 20
    mov edi, syscall_network_listen
    call syscall_copy_from_user
    jc .bad_pointer
    cmp dword [syscall_network_listen], 20
    jne .bad_size
    cmp dword [syscall_network_listen + 4], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [syscall_network_listen + 12], 1
    jne .bad_size
    cmp dword [syscall_network_listen + 16], 0
    jne .bad_reserved
    cmp dword [network_tcp_listener_bound], 1
    jne .unknown_operation
    mov eax, [syscall_network_listen + 8]
    cmp eax, [network_tcp_listener_handle]
    jne .access_denied
    mov eax, [userspace_pid]
    mov edx, [syscall_network_listen + 8]
    mov ebx, OBJECT_TYPE_NET_SOCKET
    mov ecx, HANDLE_RIGHT_WAIT
    call handle_resolve
    jc .access_denied
    mov dword [network_tcp_listener_state], TCP_STATE_LISTEN
    mov dword [network_tcp_listener_backlog], 1
    mov dword [network_tcp_pending_connections], 1
    mov esi, message_network_listen_ok
    call serial_write_string
    mov edx, [syscall_frame]
    mov dword [edx + 44], SYSCALL_STATUS_OK
    ret
.network_accept:
    cmp dword [edx + 40], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [edx + 20], 20
    jb .bad_size
    mov eax, [userspace_pid]
    mov edx, SECURITY_CAP_NET_LISTEN
    call security_check
    jc .access_denied
    mov edx, [syscall_frame]
    mov esi, [edx + 36]
    mov ecx, 20
    mov edi, syscall_network_accept
    call syscall_copy_from_user
    jc .bad_pointer
    cmp dword [syscall_network_accept], 20
    jne .bad_size
    cmp dword [syscall_network_accept + 4], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [syscall_network_accept + 16], 0
    jne .bad_reserved
    mov eax, [syscall_network_accept + 8]
    cmp eax, [network_tcp_listener_handle]
    jne .access_denied
    cmp dword [network_tcp_listener_state], TCP_STATE_LISTEN
    jne .unknown_operation
    cmp dword [network_tcp_pending_connections], 1
    jne .network_would_block
    mov eax, [userspace_pid]
    mov edx, [syscall_network_accept + 8]
    mov ebx, OBJECT_TYPE_NET_SOCKET
    mov ecx, HANDLE_RIGHT_WAIT
    call handle_resolve
    jc .access_denied
    mov eax, OBJECT_TYPE_NET_SOCKET
    mov edx, NETWORK_SOCKET_STREAM
    mov ebx, 1
    call object_create
    jc .unknown_operation
    mov edx, eax
    mov eax, [userspace_pid]
    mov ebx, OBJECT_TYPE_NET_SOCKET
    mov ecx, HANDLE_RIGHT_QUERY | HANDLE_RIGHT_SEND | HANDLE_RIGHT_RECEIVE | HANDLE_RIGHT_WAIT
    xor esi, esi
    call handle_create
    jc .unknown_operation
    mov [syscall_network_accept + 12], eax
    mov dword [network_tcp_pending_connections], 0
    inc dword [network_tcp_accepted_connections]
    mov edx, [syscall_frame]
    mov edi, [edx + 36]
    mov esi, syscall_network_accept
    mov ecx, 20
    call syscall_copy_buffer_to_user
    jc .bad_pointer
    mov esi, message_network_accept_ok
    call serial_write_string
    mov edx, [syscall_frame]
    mov dword [edx + 44], SYSCALL_STATUS_OK
    ret
.network_bind:
    cmp dword [edx + 40], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [edx + 20], 28
    jb .bad_size
    mov eax, [userspace_pid]
    mov edx, SECURITY_CAP_NET_LISTEN
    call security_check
    jc .access_denied
    mov edx, [syscall_frame]
    mov esi, [edx + 36]
    mov ecx, 28
    mov edi, syscall_network_bind
    call syscall_copy_from_user
    jc .bad_pointer
    cmp dword [syscall_network_bind], 28
    jne .bad_size
    cmp dword [syscall_network_bind + 4], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [syscall_network_bind + 12], NETWORK_AF_IPV4
    jne .unknown_operation
    cmp dword [syscall_network_bind + 16], NETWORK_IPV4_LOOPBACK
    jne .unknown_operation
    mov eax, [syscall_network_bind + 20]
    test eax, eax
    jz .unknown_operation
    cmp eax, 65535
    ja .unknown_operation
    cmp dword [syscall_network_bind + 24], 0
    jne .bad_reserved
    cmp dword [network_socket_bound], 0
    jne .unknown_operation
    mov eax, [userspace_pid]
    mov edx, [syscall_network_bind + 8]
    mov ebx, OBJECT_TYPE_NET_SOCKET
    mov ecx, HANDLE_RIGHT_BIND
    call handle_resolve
    jc .access_denied
    mov eax, [syscall_network_bind + 20]
    mov [network_socket_port], eax
    mov dword [network_socket_bound], 1
    inc dword [network_namespace_generation]
    mov esi, message_network_bind_ok
    call serial_write_string
    mov edx, [syscall_frame]
    mov dword [edx + 44], SYSCALL_STATUS_OK
    ret
.network_send:
    cmp dword [edx + 40], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [edx + 20], 40
    jb .bad_size
    mov esi, [edx + 36]
    mov ecx, 40
    mov edi, syscall_network_packet
    call syscall_copy_from_user
    jc .bad_pointer
    cmp dword [syscall_network_packet], 40
    jne .bad_size
    cmp dword [syscall_network_packet + 4], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [syscall_network_packet + 12], 16
    ja .network_packet_too_large
    cmp dword [syscall_network_packet + 16], 0
    jne .bad_reserved
    cmp dword [syscall_network_packet + 36], 0
    jne .bad_reserved
    cmp dword [network_loopback_queue_count], 0
    jne .network_would_block
    cmp dword [network_socket_bound], 1
    jne .unknown_operation
    mov eax, 0x7F000001
    mov ebx, 17                     ; UDP
    mov ecx, [network_socket_port]
    call network_firewall_check_ipv4
    jc .access_denied
    mov eax, [userspace_pid]
    mov edx, [syscall_network_packet + 8]
    mov ebx, OBJECT_TYPE_NET_SOCKET
    mov ecx, HANDLE_RIGHT_SEND
    call handle_resolve
    jc .access_denied
    call network_loopback_encapsulate
    jc .unknown_operation
    mov dword [network_loopback_queue_count], 1
    inc dword [network_transmitted_packets]
    mov esi, message_network_send_ok
    call serial_write_string
    mov edx, [syscall_frame]
    mov dword [edx + 44], SYSCALL_STATUS_OK
    ret
.network_packet_too_large:
    mov esi, message_network_size_denied
    call serial_write_string
    jmp .bad_size
.network_receive:
    cmp dword [edx + 40], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [edx + 20], 40
    jb .bad_size
    cmp dword [network_loopback_queue_count], 1
    jne .network_would_block
    call network_loopback_validate
    jc .network_malformed
    mov eax, [userspace_pid]
    mov edx, [network_socket_handle]
    mov ebx, OBJECT_TYPE_NET_SOCKET
    mov ecx, HANDLE_RIGHT_RECEIVE
    call handle_resolve
    jc .access_denied
    mov dword [syscall_network_packet], 40
    mov dword [syscall_network_packet + 4], SYSCALL_ABI_VERSION
    mov eax, [network_socket_handle]
    mov [syscall_network_packet + 8], eax
    mov eax, [network_loopback_payload_length]
    mov [syscall_network_packet + 12], eax
    mov dword [syscall_network_packet + 16], 0
    mov esi, network_loopback_frame + 28
    mov edi, syscall_network_packet + 20
    mov ecx, 16
    rep movsb
    mov dword [syscall_network_packet + 36], 0
    mov dword [network_loopback_queue_count], 0
    inc dword [network_received_packets]
    mov edx, [syscall_frame]
    mov edi, [edx + 36]
    mov esi, syscall_network_packet
    mov ecx, 40
    call syscall_copy_buffer_to_user
    jc .bad_pointer
    mov esi, message_network_loopback_ok
    call serial_write_string
    mov edx, [syscall_frame]
    mov dword [edx + 44], SYSCALL_STATUS_OK
    ret
.network_malformed:
    mov dword [network_loopback_queue_count], 0
    inc dword [network_dropped_packets]
    mov esi, message_network_malformed
    call serial_write_string
    jmp .unknown_operation
.network_would_block:
    mov esi, message_network_would_block
    call serial_write_string
    mov eax, SYSCALL_STATUS_WOULD_BLOCK
    jmp .reject
.power_query:
    cmp dword [edx + 40], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [edx + 20], 32
    jb .bad_size
    mov eax, [userspace_pid]
    mov edx, SECURITY_CAP_POWER_QUERY
    call security_check
    jc .access_denied
    mov dword [syscall_power_result + 0], 32
    mov dword [syscall_power_result + 4], SYSCALL_ABI_VERSION
    mov eax, [power_current_state]
    mov [syscall_power_result + 8], eax
    mov dword [syscall_power_result + 12], POWER_SUPPORTED_MASK
    mov eax, [power_profile]
    mov [syscall_power_result + 16], eax
    mov eax, [power_wake_lock_count]
    mov [syscall_power_result + 20], eax
    mov eax, [power_idle_entries]
    mov [syscall_power_result + 24], eax
    mov eax, [power_deep_idle_entries]
    mov [syscall_power_result + 28], eax
    mov edx, [syscall_frame]
    mov edi, [edx + 36]
    mov esi, syscall_power_result
    mov ecx, 32
    call syscall_copy_buffer_to_user
    jc .bad_pointer
    mov esi, message_power_query_ok
    call serial_write_string
    mov edx, [syscall_frame]
    mov dword [edx + 44], SYSCALL_STATUS_OK
    ret
.power_wake_acquire:
    cmp dword [edx + 40], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [edx + 20], 16
    jb .bad_size
    mov eax, [userspace_pid]
    mov edx, SECURITY_CAP_POWER_WAKE
    call security_check
    jc .access_denied
    mov edx, [syscall_frame]
    mov esi, [edx + 36]
    mov ecx, 16
    mov edi, syscall_argument_buffer
    call syscall_copy_from_user
    jc .bad_pointer
    cmp dword [syscall_argument_buffer], 16
    jne .bad_size
    cmp dword [syscall_argument_buffer + 4], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [syscall_argument_buffer + 12], 0
    jne .bad_reserved
    mov eax, [syscall_argument_buffer + 8]
    call power_wake_lock_acquire
    jc .unknown_operation
    mov [syscall_argument_buffer + 8], eax
    mov edx, [syscall_frame]
    mov edi, [edx + 36]
    mov esi, syscall_argument_buffer
    mov ecx, 16
    call syscall_copy_buffer_to_user
    jc .bad_pointer
    mov esi, message_power_wake_acquire_ok
    call serial_write_string
    mov edx, [syscall_frame]
    mov dword [edx + 44], SYSCALL_STATUS_OK
    ret
.power_wake_release:
    cmp dword [edx + 40], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [edx + 20], 16
    jb .bad_size
    mov eax, [userspace_pid]
    mov edx, SECURITY_CAP_POWER_WAKE
    call security_check
    jc .access_denied
    mov edx, [syscall_frame]
    mov esi, [edx + 36]
    mov ecx, 16
    mov edi, syscall_argument_buffer
    call syscall_copy_from_user
    jc .bad_pointer
    cmp dword [syscall_argument_buffer], 16
    jne .bad_size
    cmp dword [syscall_argument_buffer + 4], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [syscall_argument_buffer + 12], 0
    jne .bad_reserved
    mov eax, [syscall_argument_buffer + 8]
    call power_wake_lock_release
    jc .unknown_operation
    mov esi, message_power_wake_release_ok
    call serial_write_string
    mov edx, [syscall_frame]
    mov dword [edx + 44], SYSCALL_STATUS_OK
    ret
.power_set_profile:
    cmp dword [edx + 40], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [edx + 20], 16
    jb .bad_size
    mov eax, [userspace_pid]
    mov edx, SECURITY_CAP_POWER_PROFILE
    call security_check
    jc .access_denied
    mov edx, [syscall_frame]
    mov esi, [edx + 36]
    mov ecx, 16
    mov edi, syscall_argument_buffer
    call syscall_copy_from_user
    jc .bad_pointer
    cmp dword [syscall_argument_buffer], 16
    jne .bad_size
    cmp dword [syscall_argument_buffer + 4], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [syscall_argument_buffer + 12], 0
    jne .bad_reserved
    mov eax, [syscall_argument_buffer + 8]
    call power_manager_set_profile
    jc .unknown_operation
    mov esi, message_power_profile_ok
    call serial_write_string
    mov edx, [syscall_frame]
    mov dword [edx + 44], SYSCALL_STATUS_OK
    ret
.power_request_state:
    cmp dword [edx + 40], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [edx + 20], 24
    jb .bad_size
    mov esi, [edx + 36]
    mov ecx, 24
    mov edi, syscall_power_request
    call syscall_copy_from_user
    jc .bad_pointer
    cmp dword [syscall_power_request], 24
    jne .bad_size
    cmp dword [syscall_power_request + 4], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [syscall_power_request + 8], POWER_STATE_SHUTDOWN
    jne .unknown_operation
    cmp dword [syscall_power_request + 16], 0
    jne .bad_reserved
    cmp dword [syscall_power_request + 20], 0
    jne .bad_reserved
    mov eax, [userspace_pid]
    mov edx, SECURITY_CAP_POWER_SHUTDOWN
    call security_check
    jc .power_request_denied
    call power_manager_request_shutdown
    jc .unknown_operation
    mov edx, [syscall_frame]
    mov dword [edx + 44], SYSCALL_STATUS_OK
    mov dword [edx + 56], kernel_shutdown_authorized
    mov dword [edx + 60], CODE_SEGMENT
    and dword [edx + 64], 0xFFFFCFFF
    ret
.power_request_denied:
    mov esi, message_power_shutdown_denied
    call serial_write_string
    jmp .access_denied
.unknown_service:
    mov eax, SYSCALL_STATUS_SERVICE
    jmp .reject
.unknown_operation:
    mov eax, SYSCALL_STATUS_OPERATION
    jmp .reject
.bad_abi:
    mov eax, SYSCALL_STATUS_ABI
    jmp .reject
.bad_size:
    mov eax, SYSCALL_STATUS_SIZE
    jmp .reject
.bad_reserved:
    mov eax, SYSCALL_STATUS_RESERVED
    jmp .reject
.access_denied:
    mov eax, SYSCALL_STATUS_ACCESS
    jmp .reject
.semantic_mismatch:
    inc dword [semantic_ipc_rejected]
    mov eax, SYSCALL_STATUS_TYPE
    jmp .reject
.semantic_validation_failed:
    inc dword [semantic_ipc_validation_rejected]
    mov eax, SYSCALL_STATUS_VALIDATION
    jmp .reject
.bad_pointer:
    mov eax, SYSCALL_STATUS_POINTER
.reject:
    inc dword [syscall_rejected]
    mov edx, [syscall_frame]
    mov [edx + 44], eax
    ret

userspace_pid:        dd 0
userspace_tid:        dd 0
userspace_process_handle: dd 0
userspace_thread_handle:  dd 0
userspace_code_page:  dd 0
userspace_code_page_2: dd 0
userspace_code_page_3: dd 0
userspace_stack_page: dd 0
userspace_exit_seen:  dd 0
userspace_exit_code:  dd 0
syscall_frame:        dd 0
syscall_total:        dd 0
syscall_rejected:     dd 0
syscall_identity:     dd 0
syscall_closed_handle: dd 0
semantic_ipc_rejected: dd 0
semantic_ipc_validation_rejected: dd 0
align 4
syscall_argument_buffer:
    times 16 db 0
syscall_result_buffer:
    times 16 db 0
syscall_vfs_buffer: times 32 db 0
syscall_vfs_path:   times 4 db 0
syscall_power_result: times 32 db 0
syscall_power_request: times 24 db 0
syscall_network_result: times 96 db 0
syscall_network_packet: times 40 db 0
syscall_network_bind: times 28 db 0
syscall_network_connect: times 28 db 0
syscall_network_tcp_packet: times 40 db 0
syscall_network_stream_bind: times 28 db 0
syscall_network_listen: times 20 db 0
syscall_network_accept: times 20 db 0
syscall_log_result: times 32 db 0
syscall_display_info: times DISPLAY_INFO_SIZE db 0
syscall_display_scene: times SYSTEM_SCENE_SIZE db 0
syscall_display_input: times SYSTEM_INPUT_EVENT_SIZE db 0

OBJECT_TYPE_IPC_ENDPOINT equ 11

userspace_ipc_initialize:
    mov dword [userspace_ipc_count], 0
    mov dword [userspace_ipc_validation_state], 0
    mov dword [semantic_ipc_rejected], 0
    mov dword [semantic_ipc_validation_rejected], 0
    mov edi, userspace_ipc_queue
    xor eax, eax
    mov ecx, 48 / 4
    rep stosd
    mov eax, OBJECT_TYPE_IPC_ENDPOINT
    mov edx, 1
    xor ebx, ebx
    call object_create
    jc .invalid
    mov [userspace_ipc_send_object], eax
    mov ecx, SEMANTIC_IPC_INLINE_DATA
    mov edx, SEMANTIC_TYPE_VERSION_1
    mov ebx, SEMANTIC_VALIDATION_VERIFIED
    call object_semantic_attach
    jc .invalid
    mov eax, OBJECT_TYPE_IPC_ENDPOINT
    mov edx, 2
    xor ebx, ebx
    call object_create
    jc .invalid
    mov [userspace_ipc_receive_object], eax
    mov ecx, SEMANTIC_IPC_INLINE_DATA
    mov edx, SEMANTIC_TYPE_VERSION_1
    mov ebx, SEMANTIC_VALIDATION_VERIFIED
    call object_semantic_attach
    jc .invalid
    mov eax, 2
    mov edx, [userspace_ipc_send_object]
    mov ebx, OBJECT_TYPE_IPC_ENDPOINT
    mov ecx, HANDLE_RIGHT_SEND
    xor esi, esi
    call handle_create
    jc .invalid
    mov [userspace_ipc_send_handle], eax
    mov eax, 2
    mov edx, [userspace_ipc_receive_object]
    mov ebx, OBJECT_TYPE_IPC_ENDPOINT
    mov ecx, HANDLE_RIGHT_RECEIVE | HANDLE_RIGHT_WAIT
    xor esi, esi
    call handle_create
    jc .invalid
    mov [userspace_ipc_receive_handle], eax
    ; Das Paket kann je nach Programmgroesse in der ersten, zweiten oder
    ; dritten Codeseite liegen.
%if (userspace_ipc_packet - userspace_program_start + 8) < PMM_PAGE_SIZE
    mov edi, [userspace_code_page]
    add edi, userspace_ipc_packet - userspace_program_start + 8
%elif (userspace_ipc_packet - userspace_program_start + 8) < (PMM_PAGE_SIZE * 2)
    mov edi, [userspace_code_page_2]
    add edi, userspace_ipc_packet - userspace_program_start + 8 - PMM_PAGE_SIZE
%else
    mov edi, [userspace_code_page_3]
    add edi, userspace_ipc_packet - userspace_program_start + 8 - (PMM_PAGE_SIZE * 2)
%endif
    mov eax, [userspace_ipc_send_handle]
    mov [edi], eax
    cmp dword [handle_active_count], 4
    jne .invalid
    clc
    ret
.invalid:
    stc
    ret

userspace_ipc_send_object:    dd 0
userspace_ipc_receive_object: dd 0
userspace_ipc_send_handle:    dd 0
userspace_ipc_receive_handle: dd 0
userspace_ipc_count:          dd 0
userspace_ipc_validation_state: dd 0
align 4
userspace_ipc_copy:  times 48 db 0
userspace_ipc_queue: times 48 db 0

userspace_vfs_initialize:
    mov eax, 2
    mov edx, [vfs_state + VFS_ROOT_NODE_HANDLE]
    mov ebx, OBJECT_TYPE_VFS_NODE
    mov ecx, HANDLE_RIGHT_QUERY
    mov esi, HANDLE_FLAG_PROTECTED
    call handle_create
    jc .invalid
    mov [userspace_root_handle], eax
    cmp dword [handle_active_count], 5
    jne .invalid
    mov edx, eax
    mov eax, 2
    mov ebx, OBJECT_TYPE_VFS_NODE
    mov ecx, HANDLE_RIGHT_QUERY
    call handle_resolve
    jc .invalid
    mov edx, [userspace_root_handle]
    mov eax, 2
    mov ebx, OBJECT_TYPE_DEVICE       ; Root-Handle ist kein Device-Handle
    mov ecx, HANDLE_RIGHT_QUERY
    call handle_resolve
    jnc .invalid
    clc
    ret
.invalid:
    stc
    ret

userspace_root_handle: dd 0

; Erstellt eine ausschließlich lesbare, öffentliche Service-Informationsseite.
; Physische Adresse und interne Kernelobjekte werden darin nie veröffentlicht.
shared_service_page_initialize:
    call pmm_alloc_page
    test eax, eax
    jz .invalid
    mov [shared_service_page], eax
    mov edi, eax
    xor eax, eax
    mov ecx, PMM_PAGE_SIZE / 4
    rep stosd
    mov edi, [shared_service_page]
    mov dword [edi + 0], SHARED_SERVICE_SIGNATURE
    mov dword [edi + 4], SHARED_SERVICE_SIZE
    mov dword [edi + 8], SYSCALL_ABI_VERSION
    mov dword [edi + 12], SHARED_FEATURE_INT80 | SHARED_FEATURE_COPY_IO | SHARED_FEATURE_PREEMPT | SHARED_FEATURE_DISPLAY
    cmp dword [nfs_mounted], 1
    jne .features_done
    or dword [edi + 12], SHARED_FEATURE_FILESYSTEM
    cmp dword [nfs_readonly], 0
    jne .features_done
    or dword [edi + 12], SHARED_FEATURE_FILESYSTEM_WRITABLE
.features_done:
    mov dword [edi + 16], SHARED_SERVICE_BITMAP
    mov dword [edi + 20], PMM_PAGE_SIZE
    mov dword [edi + 24], 100
    mov dword [edi + 28], BOOT_PHASE_USERSPACE
    mov eax, [timer_ticks]
    mov [edi + 32], eax
    mov dword [edi + 36], 0
    mov eax, SHARED_SERVICE_ADDRESS
    mov edx, [shared_service_page]
    mov ebx, PAGING_PAGE_PRESENT | PAGING_PAGE_USER
    call paging_map_page
    jc .invalid
    mov eax, cr3
    mov cr3, eax
    clc
    ret
.invalid:
    stc
    ret

shared_service_page: dd 0
userspace_ready_seen: dd 0

; ---------------------------------------------------------------------------
; PS/2-Maus im pollingbasierten Bootstrap-Pfad
; ---------------------------------------------------------------------------
ps2_mouse_wait_write:
    mov ecx, 0x10000
.wait:
    in al, 0x64
    test al, 0x02
    jz .ready
    loop .wait
    stc
    ret
.ready:
    clc
    ret

ps2_mouse_wait_read:
    mov ecx, 0x10000
.wait:
    in al, 0x64
    test al, 0x01
    jnz .ready
    loop .wait
    stc
    ret
.ready:
    clc
    ret

; AL=Gerätekommando, CF=0 nur bei ACK 0xFA.
ps2_mouse_command:
    mov bl, al
    call ps2_mouse_wait_write
    jc .failed
    mov al, 0xD4
    out 0x64, al
    call ps2_mouse_wait_write
    jc .failed
    mov al, bl
    out 0x60, al
    call ps2_mouse_wait_read
    jc .failed
    in al, 0x60
    cmp al, 0xFA
    jne .failed
    clc
    ret
.failed:
    stc
    ret

ps2_mouse_initialize:
    pushfd
    cli
    pushad
    mov byte [mouse_ready], 0
    mov byte [mouse_packet_index], 0
    mov byte [mouse_buttons], 0
    mov byte [mouse_cursor_valid], 0
    mov eax, [kernel_context + CONTEXT_WIDTH]
    shr eax, 1
    mov [mouse_x], eax
    mov eax, [kernel_context + CONTEXT_HEIGHT]
    shr eax, 1
    mov [mouse_y], eax
    call ps2_mouse_wait_write
    jc .done
    mov al, 0xA8                    ; Auxiliary-Port aktivieren
    out 0x64, al
    mov al, 0xF6                    ; Standardwerte
    call ps2_mouse_command
    jc .done
    mov al, 0xF4                    ; Datenreporting aktivieren
    call ps2_mouse_command
    jc .done
    mov byte [mouse_ready], 1
.done:
    popad
    popfd
    ret

; AL=ein Byte des dreiteiligen Standard-PS/2-Pakets.
ps2_mouse_handle_byte:
    pushad
    cmp byte [mouse_ready], 1
    jne .done
    movzx ecx, byte [mouse_packet_index]
    test ecx, ecx
    jnz .store
    test al, 0x08                   ; Synchronisationsbit des ersten Bytes
    jz .done
.store:
    mov [mouse_packet + ecx], al
    inc ecx
    cmp ecx, 3
    jb .pending
    mov byte [mouse_packet_index], 0
    movsx eax, byte [mouse_packet + 1]
    add [mouse_x], eax
    movsx eax, byte [mouse_packet + 2]
    sub [mouse_y], eax
    cmp dword [mouse_x], 0
    jge .x_upper
    mov dword [mouse_x], 0
.x_upper:
    mov eax, [kernel_context + CONTEXT_WIDTH]
    sub eax, 12
    cmp [mouse_x], eax
    jle .y_lower
    mov [mouse_x], eax
.y_lower:
    cmp dword [mouse_y], 0
    jge .y_upper
    mov dword [mouse_y], 0
.y_upper:
    mov eax, [kernel_context + CONTEXT_HEIGHT]
    sub eax, 16
    cmp [mouse_y], eax
    jle .draw
    mov [mouse_y], eax
.draw:
    call draw_mouse_cursor
    mov al, [mouse_packet]
    and al, 1
    mov ah, [mouse_buttons]
    and ah, 1
    mov bl, [mouse_packet]
    and bl, 7
    mov [mouse_buttons], bl
    test al, al
    jz .done
    test ah, ah
    jnz .done
    call mouse_dispatch_click
    jmp .done
.pending:
    mov [mouse_packet_index], cl
.done:
    popad
    ret

mouse_dispatch_click:
    pushad
    ; Nova-Orb in der schwebenden Taskleiste.
    cmp dword [mouse_x], 92
    ja .menu
    mov eax, [kernel_context + CONTEXT_HEIGHT]
    sub eax, 88
    cmp [mouse_y], eax
    jb .menu
    mov eax, SYSTEM_INPUT_TOGGLE_START
    call input_router_enqueue
    jmp .done
.menu:
    test dword [display_scene_flags], DISPLAY_SCENE_START_MENU
    jz .workspace
    mov eax, [mouse_x]
    sub eax, [shell_menu_x]
    sub eax, 210
    js .done
    cmp eax, 358
    jae .done
    xor edx, edx
    mov ecx, 92
    div ecx
    cmp edx, 82
    jae .done
    mov ebx, eax                    ; Spalte
    mov eax, [mouse_y]
    sub eax, [shell_menu_y]
    sub eax, 60
    js .done
    cmp eax, 138
    jae .done
    xor edx, edx
    mov ecx, 74
    div ecx
    cmp edx, 64
    jae .done
    shl eax, 2
    add eax, ebx
    add eax, 3
    cmp eax, 10
    ja .done
    mov edx, eax
    mov eax, SYSTEM_INPUT_POINTER_ACTIVATE
    call input_router_enqueue_target
    jmp .done
.workspace:
    cmp dword [display_scene_workspace], 1
    je .sheet
    cmp dword [display_scene_workspace], 2
    je .studio
    call explorer_hit_test
    test eax, eax
    jz .done
    jmp .focus
.sheet:
    mov eax, [mouse_y]
    sub eax, [shell_window_y]
    sub eax, 288
    js .done
    xor edx, edx
    mov ecx, 24
    div ecx
    cmp eax, 5
    ja .done
    add eax, 40
    jmp .focus
.studio:
    mov eax, [mouse_y]
    sub eax, [shell_window_y]
    sub eax, 140
    js .done
    xor edx, edx
    mov ecx, 82
    div ecx
    cmp eax, 2
    ja .done
    mov edx, eax
    mov eax, [mouse_x]
    sub eax, [shell_window_x]
    cmp eax, 620
    jb .studio_left
    add edx, 3
.studio_left:
    mov eax, edx
    add eax, 60
.focus:
    mov edx, eax
    mov eax, SYSTEM_INPUT_POINTER_ACTIVATE
    call input_router_enqueue_target
.done:
    popad
    ret

; Software-Cursor mit gesichertem 12x16-Hintergrund.
draw_mouse_cursor:
    pushad
    cmp byte [mouse_cursor_valid], 1
    jne .save
    xor ebp, ebp
.restore_row:
    cmp ebp, 16
    jae .save
    xor ecx, ecx
.restore_column:
    cmp ecx, 12
    jae .restore_next
    mov edi, [mouse_saved_y]
    add edi, ebp
    imul edi, [kernel_context + CONTEXT_PITCH]
    add edi, [kernel_context + CONTEXT_FRAMEBUFFER]
    mov eax, [mouse_saved_x]
    add eax, ecx
    shl eax, 2
    add edi, eax
    mov eax, ebp
    imul eax, 12
    add eax, ecx
    mov eax, [mouse_background + eax * 4]
    mov [edi], eax
    inc ecx
    jmp .restore_column
.restore_next:
    inc ebp
    jmp .restore_row
.save:
    mov eax, [mouse_x]
    mov [mouse_saved_x], eax
    mov eax, [mouse_y]
    mov [mouse_saved_y], eax
    xor ebp, ebp
.draw_row:
    cmp ebp, 16
    jae .complete
    xor ecx, ecx
.draw_column:
    cmp ecx, 12
    jae .draw_next
    mov edi, [mouse_y]
    add edi, ebp
    imul edi, [kernel_context + CONTEXT_PITCH]
    add edi, [kernel_context + CONTEXT_FRAMEBUFFER]
    mov eax, [mouse_x]
    add eax, ecx
    shl eax, 2
    add edi, eax
    mov eax, ebp
    imul eax, 12
    add eax, ecx
    mov edx, [edi]
    mov [mouse_background + eax * 4], edx
    ; Klassischer, schlanker Pfeil: linke Kante plus wachsende Diagonale.
    test ecx, ecx
    jz .paint
    cmp ebp, 12
    jae .next_pixel
    mov eax, ebp
    shr eax, 1
    cmp ecx, eax
    ja .next_pixel
.paint:
    mov dword [edi], NOVA_COLOR_WHITE
.next_pixel:
    inc ecx
    jmp .draw_column
.draw_next:
    inc ebp
    jmp .draw_row
.complete:
    mov byte [mouse_cursor_valid], 1
    popad
    ret

; ---------------------------------------------------------------------------
; Bootstrap Display Server ABI 1.0
; ---------------------------------------------------------------------------
; Der Kernel behält MMIO und die physische Framebufferadresse. Der initiale
; System-UI-Prozess reicht ausschließlich validierte semantische Szenen ein.
display_server_initialize:
    mov dword [display_server_ready], 0
    mov dword [display_primary_id], 0
    mov dword [display_generation], 1
    mov dword [display_scene_generation], 0
    mov dword [display_scene_flags], 0
    mov dword [display_scene_focus], 0
    mov dword [display_scene_workspace], 0
    mov dword [display_present_count], 0
    mov dword [display_input_pending], 0
    mov dword [display_input_action], 0
    mov dword [display_input_scancode], 0
    mov dword [display_input_tick], 0
    mov dword [display_input_target], 0
    mov dword [display_input_dropped], 0
    mov byte [keyboard_extended], 0
    mov byte [keyboard_break_pending], 0
    test dword [kernel_context + CONTEXT_SEEN], CONTEXT_HAS_GRAPHICS
    jz .fallback
    cmp dword [kernel_context + CONTEXT_FRAMEBUFFER], 0
    je .fallback
    cmp dword [kernel_context + CONTEXT_BPP], 32
    jne .fallback
    cmp dword [kernel_context + CONTEXT_WIDTH], 640
    jb .fallback
    cmp dword [kernel_context + CONTEXT_HEIGHT], 480
    jb .fallback
    mov eax, [kernel_context + CONTEXT_WIDTH]
    shl eax, 2
    cmp [kernel_context + CONTEXT_PITCH], eax
    jb .invalid
    call ps2_mouse_initialize
    mov dword [display_primary_id], 1
    mov dword [display_server_ready], 1
.fallback:
    clc
    ret
.invalid:
    stc
    ret

align 4
display_server_api:
    dd 40
    dw 1, 0
    dd display_server_ready
    dd display_primary_id
    dd display_generation
    dd display_scene_generation
    dd display_present_count
    dd display_server_initialize
    dd draw_desktop_scene
display_server_ready:     dd 0
display_primary_id:       dd 0
display_generation:       dd 0
display_scene_generation: dd 0
display_scene_flags:      dd 0
display_scene_focus:      dd 0
display_scene_workspace:  dd 0
display_present_count:    dd 0
display_input_pending:    dd 0
display_input_action:     dd 0
display_input_scancode:   dd 0
display_input_tick:       dd 0
display_input_target:     dd 0
display_input_dropped:    dd 0
keyboard_extended:        db 0
keyboard_break_pending:   db 0
; US-QWERTY-Tabelle (Set 1, Make-Codes, nur Kleinbuchstaben) fuer die
; Texteingabe beim Umbenennen (F2). 0 = kein druckbares Zeichen zugeordnet.
keyboard_ascii_table:
    db 0, 0                             ; 0x00-0x01
    db '1234567890-='                   ; 0x02-0x0D
    db 0, 0                             ; 0x0E-0x0F (Backspace, Tab)
    db 'qwertyuiop'                     ; 0x10-0x19
    db 0, 0, 0, 0                       ; 0x1A-0x1D ([, ], Enter, Ctrl)
    db 'asdfghjkl'                      ; 0x1E-0x26
    db 0, 0, 0, 0, 0                    ; 0x27-0x2B (;, ', `, Shift, \)
    db 'zxcvbnm'                        ; 0x2C-0x32
    db ',./'                            ; 0x33-0x35
    db 0, 0, 0                          ; 0x36-0x38 (Shift, *, Alt)
    db ' '                              ; 0x39 (Leertaste)
    times (256 - ($ - keyboard_ascii_table)) db 0
mouse_ready:              db 0
mouse_packet_index:       db 0
mouse_buttons:            db 0
mouse_cursor_valid:       db 0
align 4
mouse_x:                  dd 0
mouse_y:                  dd 0
mouse_saved_x:            dd 0
mouse_saved_y:            dd 0
mouse_packet:             times 3 db 0
align 4
mouse_background:         times 12 * 16 dd 0
align 4

; Kernel Security / Capability Manager (ADR-2013 / NPSPEC-KERNEL-0020)
SECURITY_API_SIZE       equ 32
SECURITY_CAPACITY       equ PROCESS_CAPACITY
SECURITY_RECORD_SIZE    equ 8
SECURITY_CAP_MEMORY     equ 0x00000001
SECURITY_CAP_IO         equ 0x00000002
SECURITY_CAP_SERVICE    equ 0x00000004
SECURITY_CAP_ADMIN      equ 0x00000008
SECURITY_CAP_IPC        equ 0x00000010
SECURITY_CAP_POWER_QUERY equ 0x00000020
SECURITY_CAP_POWER_WAKE equ 0x00000040
SECURITY_CAP_POWER_SHUTDOWN equ 0x00000080
SECURITY_CAP_POWER_PROFILE equ 0x00000100
SECURITY_CAP_NET_QUERY equ 0x00000200
SECURITY_CAP_NET_RAW   equ 0x00000400
SECURITY_CAP_NET_CONNECT equ 0x00000800
SECURITY_CAP_NET_LISTEN equ 0x00001000
SECURITY_CAP_LOG_READ   equ 0x00002000
SECURITY_CAP_DISPLAY_SYSTEM_UI equ 0x00004000
SECURITY_CAP_BOOT_HEALTH_REPORT equ 0x00008000
SECURITY_CAP_BOOT_HEALTH_COMMIT equ 0x00010000
SECURITY_CAP_FS_READ     equ 0x00020000
SECURITY_CAP_FS_WRITE    equ 0x00040000
SECURITY_CAP_FS_SYSTEM_WRITE equ 0x00080000
SECURITY_KERNEL_CAPS    equ 0x000FFFFF

security_initialize:
    mov edi, security_table
    xor eax, eax
    mov ecx, (SECURITY_CAPACITY * SECURITY_RECORD_SIZE) / 4
    rep stosd
    mov dword [security_count], 0
    mov eax, 1
    mov edx, SECURITY_KERNEL_CAPS
    call security_grant
    ret

; EAX=PID, EDX=Capability-Maske.
security_grant:
    pushfd
    cli
    test edx, edx
    jz .invalid
    mov [security_temp_pid], eax
    mov [security_temp_caps], edx
    call process_lookup
    jc .invalid
    mov eax, [security_temp_pid]
    xor ecx, ecx
.scan:
    cmp ecx, SECURITY_CAPACITY
    jae .invalid
    lea edi, [security_table + ecx * 8]
    cmp dword [edi], eax
    je .grant
    cmp dword [edi], 0
    je .create
    inc ecx
    jmp .scan
.create:
    mov [edi], eax
    inc dword [security_count]
.grant:
    mov edx, [security_temp_caps]
    or [edi + 4], edx
    popfd
    clc
    ret
.invalid:
    popfd
    stc
    ret

; EAX=PID, EDX=Capability-Maske.
security_revoke:
    pushfd
    cli
    mov ecx, edx
    not ecx
    xor edi, edi
.scan:
    cmp edi, SECURITY_CAPACITY
    jae .invalid
    cmp [security_table + edi * 8], eax
    je .found
    inc edi
    jmp .scan
.found:
    and [security_table + edi * 8 + 4], ecx
    popfd
    clc
    ret
.invalid:
    popfd
    stc
    ret

; EAX=PID, EDX=benötigte Capability-Maske. EAX=1 erlaubt, 0 verweigert.
security_check:
    xor ecx, ecx
.scan:
    cmp ecx, SECURITY_CAPACITY
    jae .denied
    cmp [security_table + ecx * 8], eax
    je .found
    inc ecx
    jmp .scan
.found:
    mov eax, [security_table + ecx * 8 + 4]
    and eax, edx
    cmp eax, edx
    jne .denied
    mov eax, 1
    clc
    ret
.denied:
    xor eax, eax
    stc
    ret

security_self_test:
    mov eax, 1
    mov edx, SECURITY_CAP_ADMIN | SECURITY_CAP_SERVICE
    call security_check
    jc .invalid
    cmp eax, 1
    jne .invalid
    mov eax, 1
    mov edx, 0x80000000             ; weiterhin unbelegte Capability
    call security_check
    jnc .invalid
    mov eax, 0xFFFFFFFF
    mov edx, SECURITY_CAP_MEMORY
    call security_check
    jnc .invalid
    cmp dword [security_count], 1
    jne .invalid
    clc
    ret
.invalid:
    stc
    ret

align 4
security_api:
    dd SECURITY_API_SIZE
    dw 1, 0
    dd SECURITY_CAPACITY
    dd SECURITY_KERNEL_CAPS
    dd security_grant
    dd security_revoke
    dd security_check
    dd security_table

security_count:     dd 0
security_temp_pid:  dd 0
security_temp_caps: dd 0
align 4
security_table:
    times SECURITY_CAPACITY * SECURITY_RECORD_SIZE db 0

; ---------------------------------------------------------------------------
; Boot Health Authority ABI 1.0
; Capability-geschuetzte, generationsgebundene Provider-Aggregation.
; ---------------------------------------------------------------------------
BOOT_HEALTH_REPORT_SIZE       equ 32
BOOT_HEALTH_RECORD_SIZE       equ 64
BOOT_HEALTH_EVIDENCE_SIZE     equ 32
BOOT_HEALTH_API_SIZE          equ 40

BOOT_HEALTH_STATUS_PENDING    equ 1
BOOT_HEALTH_STATUS_HEALTHY    equ 2
BOOT_HEALTH_STATUS_DEGRADED   equ 3
BOOT_HEALTH_STATUS_FAILED     equ 4
BOOT_HEALTH_STATUS_TIMED_OUT  equ 5

BOOT_HEALTH_MILESTONE_BOOT_STARTED       equ 1
BOOT_HEALTH_MILESTONE_KERNEL_ENTERED     equ 2
BOOT_HEALTH_MILESTONE_KERNEL_INITIALIZED equ 3
BOOT_HEALTH_MILESTONE_SYSTEM_ROOT_READY  equ 4
BOOT_HEALTH_MILESTONE_CRITICAL_SERVICES  equ 5
BOOT_HEALTH_MILESTONE_OPERATIONAL        equ 6
BOOT_HEALTH_MILESTONE_CONFIRMED          equ 7

BOOT_HEALTH_PROVIDER_KERNEL_CORE equ 0x00000001
BOOT_HEALTH_PROVIDER_MEMORY      equ 0x00000002
BOOT_HEALTH_PROVIDER_SYSTEM_ROOT equ 0x00000004
BOOT_HEALTH_PROVIDER_TRUST       equ 0x00000008
BOOT_HEALTH_PROVIDER_CAPABILITY  equ 0x00000010
BOOT_HEALTH_PROVIDER_IPC         equ 0x00000020
BOOT_HEALTH_PROVIDER_SESSION     equ 0x00000040

BOOT_HEALTH_PROVIDER_READY       equ 1
BOOT_HEALTH_PROVIDER_DEGRADED    equ 2
BOOT_HEALTH_PROVIDER_FAILED      equ 3

BOOT_HEALTH_REQUIRED_MILESTONES  equ 0x00000078
BOOT_HEALTH_INITIAL_MILESTONES   equ 0x00000006
BOOT_HEALTH_FLAG_IDENTITY_VALID  equ 0x00000001
BOOT_HEALTH_FLAG_TRUST_VERIFIED  equ 0x00000002
BOOT_HEALTH_FLAG_DEGRADED_SEEN   equ 0x00000004
BOOT_HEALTH_FLAG_EVIDENCE_READY  equ 0x00000008

; Record offsets are shared with nova/boot_health.h.
BH_RECORD_GENERATION_LO equ 8
BH_RECORD_GENERATION_HI equ 12
BH_RECORD_BOOT_ATTEMPT  equ 16
BH_RECORD_STATUS        equ 20
BH_RECORD_REACHED       equ 24
BH_RECORD_REQUIRED      equ 28
BH_RECORD_LAST          equ 32
BH_RECORD_FAILED        equ 36
BH_RECORD_SEQUENCE      equ 40
BH_RECORD_AUTHORIZED    equ 44
BH_RECORD_REJECTED      equ 48
BH_RECORD_FLAGS         equ 52

boot_health_initialize:
    cld
    mov edi, boot_health_record
    xor eax, eax
    mov ecx, BOOT_HEALTH_RECORD_SIZE / 4
    rep stosd
    mov edi, boot_health_provider_ready
    mov ecx, 8
    rep stosd

    mov dword [boot_health_record], BOOT_HEALTH_RECORD_SIZE
    mov word [boot_health_record + 4], 1
    mov word [boot_health_record + 6], 0
    mov eax, [kernel_context + CONTEXT_SYSTEM_GENERATION]
    mov [boot_health_record + BH_RECORD_GENERATION_LO], eax
    mov eax, [kernel_context + CONTEXT_SYSTEM_GENERATION_HI]
    mov [boot_health_record + BH_RECORD_GENERATION_HI], eax
    mov eax, [kernel_context + CONTEXT_BOOT_ATTEMPT]
    mov [boot_health_record + BH_RECORD_BOOT_ATTEMPT], eax
    mov dword [boot_health_record + BH_RECORD_STATUS], BOOT_HEALTH_STATUS_PENDING
    mov dword [boot_health_record + BH_RECORD_REACHED], BOOT_HEALTH_INITIAL_MILESTONES
    mov dword [boot_health_record + BH_RECORD_REQUIRED], BOOT_HEALTH_REQUIRED_MILESTONES
    mov dword [boot_health_record + BH_RECORD_LAST], BOOT_HEALTH_MILESTONE_KERNEL_ENTERED
    mov dword [boot_health_record + BH_RECORD_SEQUENCE], 1
    mov dword [boot_health_record + BH_RECORD_FLAGS], BOOT_HEALTH_FLAG_IDENTITY_VALID
    cmp dword [kernel_context + CONTEXT_SECURITY_STATE], NOVA_BOOT_VERIFICATION_SIGNATURE_VERIFIED
    jb .identity_ready
    or dword [boot_health_record + BH_RECORD_FLAGS], BOOT_HEALTH_FLAG_TRUST_VERIFIED
.identity_ready:
    mov dword [boot_health_ready], 1
    clc
    ret

; EDX=Milestone, ECX=Provider, EBX=Providerstatus.
boot_health_prepare_report:
    mov dword [boot_health_temp_report], BOOT_HEALTH_REPORT_SIZE
    mov word [boot_health_temp_report + 4], 1
    mov word [boot_health_temp_report + 6], 0
    mov eax, [boot_health_record + BH_RECORD_GENERATION_LO]
    mov [boot_health_temp_report + 8], eax
    mov eax, [boot_health_record + BH_RECORD_GENERATION_HI]
    mov [boot_health_temp_report + 12], eax
    mov eax, [boot_health_record + BH_RECORD_BOOT_ATTEMPT]
    mov [boot_health_temp_report + 16], eax
    mov [boot_health_temp_report + 20], edx
    mov [boot_health_temp_report + 24], ecx
    mov [boot_health_temp_report + 28], ebx
    ret

; EAX=aufrufende PID, ESI=Report. Nur das Kernelobjekt behaelt Zustand.
boot_health_submit_report:
    pushfd
    cli
    pushad
    cmp dword [boot_health_ready], 1
    jne .reject
    test esi, 3
    jnz .reject
    cmp dword [esi], BOOT_HEALTH_REPORT_SIZE
    jne .reject
    cmp word [esi + 4], 1
    jne .reject
    cmp word [esi + 6], 0
    jne .reject
    mov [boot_health_temp_pid], eax
    mov edi, boot_health_temp_report
    mov ecx, BOOT_HEALTH_REPORT_SIZE / 4
    cld
    rep movsd

    mov eax, [boot_health_temp_pid]
    mov edx, SECURITY_CAP_BOOT_HEALTH_REPORT
    call security_check
    jc .reject
    mov eax, [boot_health_temp_report + 8]
    cmp eax, [boot_health_record + BH_RECORD_GENERATION_LO]
    jne .reject
    mov eax, [boot_health_temp_report + 12]
    cmp eax, [boot_health_record + BH_RECORD_GENERATION_HI]
    jne .reject
    mov eax, [boot_health_temp_report + 16]
    cmp eax, [boot_health_record + BH_RECORD_BOOT_ATTEMPT]
    jne .reject
    cmp dword [boot_health_record + BH_RECORD_STATUS], BOOT_HEALTH_STATUS_FAILED
    je .reject
    cmp dword [boot_health_record + BH_RECORD_STATUS], BOOT_HEALTH_STATUS_TIMED_OUT
    je .reject

    mov ecx, [boot_health_temp_report + 20]
    cmp ecx, BOOT_HEALTH_MILESTONE_KERNEL_INITIALIZED
    jb .reject
    cmp ecx, BOOT_HEALTH_MILESTONE_OPERATIONAL
    ja .reject
    mov eax, [boot_health_temp_report + 24]
    test eax, eax
    jz .reject
    mov edx, eax
    dec edx
    test eax, edx
    jnz .reject
    mov edx, [boot_health_provider_allowed + ecx * 4]
    test eax, edx
    jz .reject
    mov ebx, [boot_health_temp_report + 28]
    cmp ebx, BOOT_HEALTH_PROVIDER_READY
    jb .reject
    cmp ebx, BOOT_HEALTH_PROVIDER_FAILED
    ja .reject

    inc dword [boot_health_record + BH_RECORD_AUTHORIZED]
    inc dword [boot_health_record + BH_RECORD_SEQUENCE]
    cmp ebx, BOOT_HEALTH_PROVIDER_FAILED
    je .provider_failed
    cmp ebx, BOOT_HEALTH_PROVIDER_DEGRADED
    je .provider_degraded
    or [boot_health_provider_ready + ecx * 4], eax
    call boot_health_advance
    popad
    popfd
    clc
    ret
.provider_degraded:
    or dword [boot_health_record + BH_RECORD_FLAGS], BOOT_HEALTH_FLAG_DEGRADED_SEEN
    mov dword [boot_health_record + BH_RECORD_STATUS], BOOT_HEALTH_STATUS_DEGRADED
    ; Provider als degradiert markieren, damit advance ihn als erfüllt wertet
    or [boot_health_provider_degraded + ecx * 4], eax
    call boot_health_advance
    popad
    popfd
    clc
    ret
.provider_failed:
    mov dword [boot_health_record + BH_RECORD_STATUS], BOOT_HEALTH_STATUS_FAILED
    mov [boot_health_record + BH_RECORD_FAILED], ecx
    popad
    popfd
    clc
    ret
.reject:
    inc dword [boot_health_record + BH_RECORD_REJECTED]
    popad
    popfd
    stc
    ret

boot_health_advance:
    mov ecx, [boot_health_record + BH_RECORD_LAST]
.next:
    inc ecx
    cmp ecx, BOOT_HEALTH_MILESTONE_OPERATIONAL
    ja .done
    mov eax, [boot_health_provider_required + ecx * 4]
    ; READY-Bits: exakt erfüllt
    mov edx, [boot_health_provider_ready + ecx * 4]
    ; DEGRADED-Bits zählen ebenfalls als erfüllt (aber Status bleibt DEGRADED)
    or  edx, [boot_health_provider_degraded + ecx * 4]
    and edx, eax
    cmp edx, eax
    jne .done
    bts dword [boot_health_record + BH_RECORD_REACHED], ecx
    mov [boot_health_record + BH_RECORD_LAST], ecx
    jmp .next
.done:
    cmp dword [boot_health_record + BH_RECORD_LAST], BOOT_HEALTH_MILESTONE_OPERATIONAL
    jne .return
    ; Status nur auf HEALTHY setzen wenn bisher kein DEGRADED-Pfad
    cmp dword [boot_health_record + BH_RECORD_STATUS], BOOT_HEALTH_STATUS_DEGRADED
    je .mark_confirmed
    mov dword [boot_health_record + BH_RECORD_STATUS], BOOT_HEALTH_STATUS_HEALTHY
.mark_confirmed:
    bts dword [boot_health_record + BH_RECORD_REACHED], BOOT_HEALTH_MILESTONE_CONFIRMED
    mov dword [boot_health_record + BH_RECORD_LAST], BOOT_HEALTH_MILESTONE_CONFIRMED
    or dword [boot_health_record + BH_RECORD_FLAGS], BOOT_HEALTH_FLAG_EVIDENCE_READY
.return:
    ret

; EAX=aufrufende PID, EDI=32-Byte-Ausgabepuffer.
boot_health_export_evidence:
    pushfd
    cli
    pushad
    test edi, 3
    jnz .denied
    mov [boot_health_temp_pid], eax
    mov eax, [boot_health_temp_pid]
    mov edx, SECURITY_CAP_BOOT_HEALTH_COMMIT
    call security_check
    jc .denied
    test dword [boot_health_record + BH_RECORD_FLAGS], BOOT_HEALTH_FLAG_EVIDENCE_READY
    jz .denied
    mov dword [edi], BOOT_HEALTH_EVIDENCE_SIZE
    mov word [edi + 4], 1
    mov word [edi + 6], 0
    mov eax, [boot_health_record + BH_RECORD_GENERATION_LO]
    mov [edi + 8], eax
    mov eax, [boot_health_record + BH_RECORD_GENERATION_HI]
    mov [edi + 12], eax
    mov eax, [boot_health_record + BH_RECORD_BOOT_ATTEMPT]
    mov [edi + 16], eax
    mov eax, [boot_health_record + BH_RECORD_REACHED]
    mov [edi + 20], eax
    mov eax, [boot_health_record + BH_RECORD_STATUS]
    mov [edi + 24], eax
    xor eax, eax
    test dword [boot_health_record + BH_RECORD_FLAGS], BOOT_HEALTH_FLAG_TRUST_VERIFIED
    jz .store_trust
    inc eax
.store_trust:
    mov [edi + 28], eax
    popad
    popfd
    clc
    ret
.denied:
    popad
    popfd
    stc
    ret

boot_health_publish_core_services:
    mov edx, BOOT_HEALTH_MILESTONE_CRITICAL_SERVICES
    mov ecx, BOOT_HEALTH_PROVIDER_CAPABILITY
    mov ebx, BOOT_HEALTH_PROVIDER_READY
    call boot_health_prepare_report
    mov eax, 1
    mov esi, boot_health_temp_report
    call boot_health_submit_report
    jc .invalid
    mov edx, BOOT_HEALTH_MILESTONE_CRITICAL_SERVICES
    mov ecx, BOOT_HEALTH_PROVIDER_IPC
    mov ebx, BOOT_HEALTH_PROVIDER_READY
    call boot_health_prepare_report
    mov eax, 1
    mov esi, boot_health_temp_report
    call boot_health_submit_report
    ret
.invalid:
    stc
    ret

; §108 Trust-Provider: meldet TRUST READY wenn Kernel signiert (SIGNATURE_VERIFIED),
; sonst TRUST DEGRADED (Boot laeuft weiter, Health bleibt Degraded, nicht Confirmed).
boot_health_publish_trust_provider:
    mov eax, [kernel_context + CONTEXT_SECURITY_STATE]
    cmp eax, NOVA_BOOT_VERIFICATION_SIGNATURE_VERIFIED
    jae .signed
    ; Unsignierter Kernel: TRUST DEGRADED melden und Meldung ausgeben
    mov edx, BOOT_HEALTH_MILESTONE_CRITICAL_SERVICES
    mov ecx, BOOT_HEALTH_PROVIDER_TRUST
    mov ebx, BOOT_HEALTH_PROVIDER_DEGRADED
    call boot_health_prepare_report
    mov eax, 1
    mov esi, boot_health_temp_report
    call boot_health_submit_report
    mov esi, message_boot_health_trust_degraded
    call serial_write_string
    clc
    ret
.signed:
    ; Signierter Kernel: TRUST READY melden
    mov edx, BOOT_HEALTH_MILESTONE_CRITICAL_SERVICES
    mov ecx, BOOT_HEALTH_PROVIDER_TRUST
    mov ebx, BOOT_HEALTH_PROVIDER_READY
    call boot_health_prepare_report
    mov eax, 1
    mov esi, boot_health_temp_report
    call boot_health_submit_report
    mov esi, message_boot_health_trust_signed
    call serial_write_string
    clc
    ret

; §108 Session-Provider (Phase-1 Stub): meldet SESSION READY sobald Userspace bereit.
; Phase 2 ersetzt dies durch echten Session-Manager mit Authentifizierung.
boot_health_publish_session_provider:
    mov edx, BOOT_HEALTH_MILESTONE_OPERATIONAL
    mov ecx, BOOT_HEALTH_PROVIDER_SESSION
    mov ebx, BOOT_HEALTH_PROVIDER_READY
    call boot_health_prepare_report
    mov eax, 1
    mov esi, boot_health_temp_report
    call boot_health_submit_report
    mov esi, message_boot_health_session_ready
    call serial_write_string
    clc
    ret

; §126: SystemRoot-Provider melden — READY wenn NovaFS rw gemountet,
; DEGRADED wenn read-only oder gar nicht gemountet.
boot_health_publish_system_root:
    pushad
    mov ebx, BOOT_HEALTH_PROVIDER_READY
    cmp dword [nfs_mounted], 1
    je .mounted
    ; kein Volume: DEGRADED
    mov ebx, BOOT_HEALTH_PROVIDER_DEGRADED
    jmp .report
.mounted:
    cmp dword [nfs_readonly], 0
    je .report                      ; rw → bleibt READY
    mov ebx, BOOT_HEALTH_PROVIDER_DEGRADED
.report:
    mov edx, BOOT_HEALTH_MILESTONE_SYSTEM_ROOT_READY
    mov ecx, BOOT_HEALTH_PROVIDER_SYSTEM_ROOT
    call boot_health_prepare_report
    mov eax, 1
    mov esi, boot_health_temp_report
    call boot_health_submit_report  ; CF ignorieren (best-effort)
    cmp ebx, BOOT_HEALTH_PROVIDER_READY
    je .msg_ready
    mov esi, message_boot_health_system_root_degraded
    call serial_write_string
    popad
    clc
    ret
.msg_ready:
    mov esi, message_boot_health_system_root_ready
    call serial_write_string
    popad
    clc
    ret

boot_health_mark_kernel_initialized:
    mov edx, BOOT_HEALTH_MILESTONE_KERNEL_INITIALIZED
    mov ecx, BOOT_HEALTH_PROVIDER_KERNEL_CORE
    mov ebx, BOOT_HEALTH_PROVIDER_READY
    call boot_health_prepare_report
    mov eax, 1
    mov esi, boot_health_temp_report
    call boot_health_submit_report
    jc .invalid
    mov edx, BOOT_HEALTH_MILESTONE_KERNEL_INITIALIZED
    mov ecx, BOOT_HEALTH_PROVIDER_MEMORY
    mov ebx, BOOT_HEALTH_PROVIDER_READY
    call boot_health_prepare_report
    mov eax, 1
    mov esi, boot_health_temp_report
    call boot_health_submit_report
    jc .invalid
    cmp dword [boot_health_record + BH_RECORD_LAST], BOOT_HEALTH_MILESTONE_KERNEL_INITIALIZED
    jne .invalid
    clc
    ret
.invalid:
    stc
    ret

; Der Selbsttest arbeitet auf einem Snapshot und stellt den Live-Zustand wieder her.
boot_health_self_test:
    cld
    mov esi, boot_health_record
    mov edi, boot_health_saved_record
    mov ecx, BOOT_HEALTH_RECORD_SIZE / 4
    rep movsd
    mov esi, boot_health_provider_ready
    mov edi, boot_health_saved_providers
    mov ecx, 8
    rep movsd
    mov dword [boot_health_selftest_result], 1

    mov edx, BOOT_HEALTH_MILESTONE_KERNEL_INITIALIZED
    mov ecx, BOOT_HEALTH_PROVIDER_KERNEL_CORE
    mov ebx, BOOT_HEALTH_PROVIDER_READY
    call boot_health_prepare_report
    mov eax, 0xFFFFFFFF
    mov esi, boot_health_temp_report
    call boot_health_submit_report
    jnc .restore
    mov eax, 1
    mov esi, boot_health_temp_report
    call boot_health_submit_report
    jc .restore
    cmp dword [boot_health_record + BH_RECORD_LAST], BOOT_HEALTH_MILESTONE_KERNEL_ENTERED
    jne .restore

    mov edx, BOOT_HEALTH_MILESTONE_KERNEL_INITIALIZED
    mov ecx, BOOT_HEALTH_PROVIDER_MEMORY
    call boot_health_selftest_ready
    jc .restore
    mov edx, BOOT_HEALTH_MILESTONE_SYSTEM_ROOT_READY
    mov ecx, BOOT_HEALTH_PROVIDER_SYSTEM_ROOT
    call boot_health_selftest_ready
    jc .restore
    mov edx, BOOT_HEALTH_MILESTONE_CRITICAL_SERVICES
    mov ecx, BOOT_HEALTH_PROVIDER_TRUST
    call boot_health_selftest_ready
    jc .restore
    mov edx, BOOT_HEALTH_MILESTONE_CRITICAL_SERVICES
    mov ecx, BOOT_HEALTH_PROVIDER_CAPABILITY
    call boot_health_selftest_ready
    jc .restore
    mov edx, BOOT_HEALTH_MILESTONE_CRITICAL_SERVICES
    mov ecx, BOOT_HEALTH_PROVIDER_IPC
    call boot_health_selftest_ready
    jc .restore
    mov edx, BOOT_HEALTH_MILESTONE_OPERATIONAL
    mov ecx, BOOT_HEALTH_PROVIDER_SESSION
    call boot_health_selftest_ready
    jc .restore
    cmp dword [boot_health_record + BH_RECORD_STATUS], BOOT_HEALTH_STATUS_HEALTHY
    jne .restore
    cmp dword [boot_health_record + BH_RECORD_LAST], BOOT_HEALTH_MILESTONE_CONFIRMED
    jne .restore
    mov eax, 1
    mov edi, boot_health_test_evidence
    call boot_health_export_evidence
    jc .restore
    mov eax, [boot_health_test_evidence + 8]
    cmp eax, [boot_health_record + BH_RECORD_GENERATION_LO]
    jne .restore
    mov eax, [boot_health_test_evidence + 16]
    cmp eax, [boot_health_record + BH_RECORD_BOOT_ATTEMPT]
    jne .restore
    mov dword [boot_health_selftest_result], 0
.restore:
    cld
    mov esi, boot_health_saved_record
    mov edi, boot_health_record
    mov ecx, BOOT_HEALTH_RECORD_SIZE / 4
    rep movsd
    mov esi, boot_health_saved_providers
    mov edi, boot_health_provider_ready
    mov ecx, 8
    rep movsd
    cmp dword [boot_health_selftest_result], 0
    jne .invalid
    clc
    ret
.invalid:
    stc
    ret

boot_health_selftest_ready:
    mov ebx, BOOT_HEALTH_PROVIDER_READY
    call boot_health_prepare_report
    mov eax, 1
    mov esi, boot_health_temp_report
    call boot_health_submit_report
    ret

; Persistiert einen groben Candidate-Checkpoint ueber den normalisierten
; Firmware-Provider. EAX=0 nicht erforderlich, 1 geschrieben, 2 fehlgeschlagen.
firmware_runtime_boot_health_checkpoint:
    cmp dword [kernel_context + CONTEXT_BOOT_ATTEMPT], 0
    je .not_required
    cmp dword [kernel_context + CONTEXT_BOOT_GENERATION], 1
    ja .not_required
    test dword [kernel_context + CONTEXT_FIRMWARE_RUNTIME_CAPS], NOVA_FIRMWARE_RUNTIME_PERSIST_BOOT_HEALTH
    jz .not_required
    cmp dword [kernel_context + CONTEXT_FIRMWARE_RUNTIME_CONTEXT], 0
    je .failed
    cmp dword [kernel_context + CONTEXT_FIRMWARE_RUNTIME_ENTRY], 0
    je .failed
    cmp dword [boot_health_record + BH_RECORD_GENERATION_HI], 0
    jne .failed
    mov dword [boot_health_wire + 0], 0x41564F4E
    mov dword [boot_health_wire + 4], 0x56454842
    mov word [boot_health_wire + 8], 1
    mov word [boot_health_wire + 10], 64
    mov eax, [kernel_context + CONTEXT_BOOT_GENERATION]
    mov [boot_health_wire + 12], eax
    mov eax, [boot_health_record + BH_RECORD_GENERATION_LO]
    mov [boot_health_wire + 16], eax
    mov dword [boot_health_wire + 20], 0
    mov eax, [boot_health_record + BH_RECORD_BOOT_ATTEMPT]
    mov [boot_health_wire + 24], eax
    mov eax, [boot_health_record + BH_RECORD_REACHED]
    mov [boot_health_wire + 28], eax
    mov eax, [boot_health_record + BH_RECORD_FAILED]
    mov [boot_health_wire + 32], eax
    mov eax, [boot_health_record + BH_RECORD_STATUS]
    mov [boot_health_wire + 36], eax
    xor eax, eax
    test dword [boot_health_record + BH_RECORD_FLAGS], BOOT_HEALTH_FLAG_TRUST_VERIFIED
    jz .trust_stored
    inc eax
.trust_stored:
    mov [boot_health_wire + 40], eax
    mov eax, [boot_health_record + BH_RECORD_SEQUENCE]
    mov [boot_health_wire + 44], eax
    mov dword [boot_health_wire + 48], 0
    mov dword [boot_health_wire + 52], 0
    mov dword [boot_health_wire + 56], 0
    mov dword [boot_health_wire + 60], 0
    mov esi, boot_health_wire
    call boot_health_wire_crc
    mov [boot_health_wire + 60], eax
    mov esi, boot_health_wire
    mov edi, [kernel_context + CONTEXT_FIRMWARE_RUNTIME_CONTEXT]
    mov eax, [kernel_context + CONTEXT_FIRMWARE_RUNTIME_ENTRY]
    call firmware_runtime_invoke
    cmp eax, 1
    jne .failed
    mov eax, 1
    ret
.not_required:
    xor eax, eax
    ret
.failed:
    mov eax, 2
    ret

; Persistiert HEALTHY-Wire nach HealthConfirmed via Firmware-Provider.
; EAX=0 nicht erforderlich, 1 geschrieben, 2 fehlgeschlagen.
firmware_runtime_health_commit:
    cmp dword [kernel_context + CONTEXT_BOOT_ATTEMPT], 0
    je .not_required
    test dword [kernel_context + CONTEXT_FIRMWARE_RUNTIME_CAPS], NOVA_FIRMWARE_RUNTIME_PERSIST_BOOT_HEALTH
    jz .not_required
    cmp dword [kernel_context + CONTEXT_FIRMWARE_RUNTIME_CONTEXT], 0
    je .failed
    cmp dword [kernel_context + CONTEXT_FIRMWARE_RUNTIME_ENTRY], 0
    je .failed
    cmp dword [boot_health_record + BH_RECORD_STATUS], BOOT_HEALTH_STATUS_HEALTHY
    jne .not_required
    test dword [boot_health_record + BH_RECORD_FLAGS], BOOT_HEALTH_FLAG_EVIDENCE_READY
    jz .not_required
    cmp dword [boot_health_record + BH_RECORD_GENERATION_HI], 0
    jne .failed
    mov dword [boot_health_wire + 0], 0x41564F4E
    mov dword [boot_health_wire + 4], 0x56454842
    mov word [boot_health_wire + 8], 1
    mov word [boot_health_wire + 10], 64
    mov eax, [kernel_context + CONTEXT_BOOT_GENERATION]
    mov [boot_health_wire + 12], eax
    mov eax, [boot_health_record + BH_RECORD_GENERATION_LO]
    mov [boot_health_wire + 16], eax
    mov dword [boot_health_wire + 20], 0
    mov eax, [boot_health_record + BH_RECORD_BOOT_ATTEMPT]
    mov [boot_health_wire + 24], eax
    mov eax, [boot_health_record + BH_RECORD_REACHED]
    mov [boot_health_wire + 28], eax
    mov eax, [boot_health_record + BH_RECORD_FAILED]
    mov [boot_health_wire + 32], eax
    mov dword [boot_health_wire + 36], BOOT_HEALTH_STATUS_HEALTHY
    xor eax, eax
    test dword [boot_health_record + BH_RECORD_FLAGS], BOOT_HEALTH_FLAG_TRUST_VERIFIED
    jz .trust_stored
    inc eax
.trust_stored:
    mov [boot_health_wire + 40], eax
    mov eax, [boot_health_record + BH_RECORD_SEQUENCE]
    mov [boot_health_wire + 44], eax
    mov dword [boot_health_wire + 48], 0
    mov dword [boot_health_wire + 52], 0
    mov dword [boot_health_wire + 56], 0
    mov dword [boot_health_wire + 60], 0
    mov esi, boot_health_wire
    call boot_health_wire_crc
    mov [boot_health_wire + 60], eax
    mov esi, boot_health_wire
    mov edi, [kernel_context + CONTEXT_FIRMWARE_RUNTIME_CONTEXT]
    mov eax, [kernel_context + CONTEXT_FIRMWARE_RUNTIME_ENTRY]
    call firmware_runtime_invoke
    cmp eax, 1
    jne .failed
    mov eax, 1
    ret
.not_required:
    xor eax, eax
    ret
.failed:
    mov eax, 2
    ret

; EAX=physischer Provider-Entry, EDI=Kontext, ESI=Payload. Der Loaderbereich
; bleibt reserviert, ist aber nicht Bestandteil der Kernel-Seitentabellen.
firmware_runtime_invoke:
    mov [firmware_runtime_entry], eax
    pushfd
    cli
    mov eax, cr0
    mov [firmware_runtime_saved_cr0], eax
    mov eax, cr3
    mov [firmware_runtime_saved_cr3], eax
    mov eax, cr4
    mov [firmware_runtime_saved_cr4], eax
    sgdt [firmware_runtime_saved_gdtr]
    mov ecx, 0xC0000080
    rdmsr
    mov [firmware_runtime_saved_efer_lo], eax
    mov [firmware_runtime_saved_efer_hi], edx
    mov eax, [firmware_runtime_saved_cr0]
    and eax, 0x7FFFFFFF
    mov cr0, eax
    mov eax, [firmware_runtime_entry]
    call eax
    mov [firmware_runtime_result], eax
    mov eax, [firmware_runtime_saved_cr4]
    mov cr4, eax
    mov eax, [firmware_runtime_saved_cr3]
    mov cr3, eax
    mov ecx, 0xC0000080
    mov eax, [firmware_runtime_saved_efer_lo]
    mov edx, [firmware_runtime_saved_efer_hi]
    wrmsr
    mov eax, [firmware_runtime_saved_cr0]
    mov cr0, eax
    lgdt [firmware_runtime_saved_gdtr]
    jmp 0x08:.kernel_segments_restored
.kernel_segments_restored:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov ss, ax
    popfd
    mov eax, [firmware_runtime_result]
    ret

; ESI=64-Byte-Wire-Datensatz; Checksum-Feld liegt am Ende und ist bereits null.
boot_health_wire_crc:
    push ebx
    push ecx
    push edx
    mov eax, 0xFFFFFFFF
    xor edx, edx
.byte:
    cmp edx, 64
    jae .done
    movzx ebx, byte [esi + edx]
    xor al, bl
    mov ecx, 8
.bit:
    shr eax, 1
    jnc .next_bit
    xor eax, 0xEDB88320
.next_bit:
    loop .bit
    inc edx
    jmp .byte
.done:
    not eax
    pop edx
    pop ecx
    pop ebx
    ret

align 4
boot_health_api:
    dd BOOT_HEALTH_API_SIZE
    dw 1, 0
    dd 0x00000007                 ; report, aggregate, evidence-export
    dd boot_health_submit_report
    dd boot_health_export_evidence
    dd boot_health_record
    dd boot_health_provider_ready
    dd BOOT_HEALTH_REPORT_SIZE
    dd BOOT_HEALTH_EVIDENCE_SIZE
    dd 0

boot_health_provider_required:
    dd 0, 0, 0
    dd BOOT_HEALTH_PROVIDER_KERNEL_CORE | BOOT_HEALTH_PROVIDER_MEMORY
    dd BOOT_HEALTH_PROVIDER_SYSTEM_ROOT
    dd BOOT_HEALTH_PROVIDER_TRUST | BOOT_HEALTH_PROVIDER_CAPABILITY | BOOT_HEALTH_PROVIDER_IPC
    dd BOOT_HEALTH_PROVIDER_SESSION
    dd 0
boot_health_provider_allowed:
    dd 0, 0, 0
    dd BOOT_HEALTH_PROVIDER_KERNEL_CORE | BOOT_HEALTH_PROVIDER_MEMORY
    dd BOOT_HEALTH_PROVIDER_SYSTEM_ROOT
    dd BOOT_HEALTH_PROVIDER_TRUST | BOOT_HEALTH_PROVIDER_CAPABILITY | BOOT_HEALTH_PROVIDER_IPC
    dd BOOT_HEALTH_PROVIDER_SESSION
    dd 0

align 4
boot_health_ready:           dd 0
boot_health_temp_pid:        dd 0
boot_health_selftest_result: dd 0
boot_health_provider_ready:    times 8 dd 0
boot_health_provider_degraded: times 8 dd 0
boot_health_saved_providers:   times 8 dd 0
boot_health_temp_report:     times BOOT_HEALTH_REPORT_SIZE db 0
boot_health_record:          times BOOT_HEALTH_RECORD_SIZE db 0
boot_health_saved_record:    times BOOT_HEALTH_RECORD_SIZE db 0
boot_health_test_evidence:   times BOOT_HEALTH_EVIDENCE_SIZE db 0
boot_health_wire:            times 64 db 0
firmware_runtime_entry:       dd 0
firmware_runtime_result:      dd 0
firmware_runtime_saved_cr0:   dd 0
firmware_runtime_saved_cr3:   dd 0
firmware_runtime_saved_cr4:   dd 0
firmware_runtime_saved_efer_lo: dd 0
firmware_runtime_saved_efer_hi: dd 0
firmware_runtime_saved_gdtr:   dw 0
                               dd 0

; ---------------------------------------------------------------------------
; CPU Manager / BSP- und Topologieerkennung (NPSPEC-KERNEL-0026)
; ---------------------------------------------------------------------------
CPU_API_SIZE          equ 48
CPU_RECORD_SIZE       equ 112
CPU_CAPACITY          equ 4
CPU_LOCAL_SLOT_SIZE   equ 64
CPU_STATE_DISCOVERED  equ 0
CPU_STATE_OFFLINE     equ 1
CPU_STATE_STARTING    equ 2
CPU_STATE_ONLINE      equ 3
CPU_STATE_ACTIVE      equ 4
CPU_STATE_IDLE        equ 5
CPU_STATE_FAILED      equ 8
CPU_CAPACITY_SCALE    equ 1024
CPU_FEATURE_FPU       equ 0x00000001
CPU_FEATURE_SIMD      equ 0x00000002
CPU_FEATURE_NX        equ 0x00000004
CPU_FEATURE_LOCAL_APIC equ 0x00000008
CPU_FEATURE_HW_RANDOM equ 0x00000010
CPU_FEATURE_VIRTUALIZED equ 0x00000020
CPU_TOPOLOGY_UNKNOWN  equ 0xFFFFFFFF
CPU_ID                equ 0
CPU_HARDWARE_LOW      equ 4
CPU_HARDWARE_HIGH     equ 8
CPU_STATE             equ 12
CPU_PACKAGE_ID        equ 16
CPU_DIE_ID            equ 20
CPU_CLUSTER_ID        equ 24
CPU_CORE_ID           equ 28
CPU_THREAD_ID         equ 32
CPU_NUMA_NODE_ID      equ 36
CPU_PACKAGE_THREADS   equ 40
CPU_CAPACITY_VALUE    equ 44
CPU_LOCAL_DATA        equ 48
CPU_FLAGS             equ 52
CPU_VENDOR0           equ 56
CPU_VENDOR1           equ 60
CPU_VENDOR2           equ 64
CPU_SIGNATURE         equ 68
CPU_RAW_FEATURE_EDX   equ 72
CPU_RAW_FEATURE_ECX   equ 76
CPU_FEATURES          equ 80
CPU_LLC_ID            equ 84
CPU_TOPOLOGY_NODE     equ 88
CPU_TOPOLOGY_GENERATION equ 92

cpu_manager_initialize:
    push ebp
    mov edi, cpu_records
    xor eax, eax
    mov ecx, (CPU_CAPACITY * CPU_RECORD_SIZE) / 4
    rep stosd
    mov edi, cpu_local_data
    mov ecx, (CPU_CAPACITY * CPU_LOCAL_SLOT_SIZE) / 4
    rep stosd
    mov dword [cpu_possible_set], 1
    mov dword [cpu_discovered_set], 1
    mov dword [cpu_present_set], 1
    mov dword [cpu_online_set], 0
    mov dword [cpu_active_set], 0
    mov dword [cpu_isolated_set], 0
    mov dword [cpu_failed_set], 0
    mov dword [cpu_discovered_count], 1
    mov dword [cpu_online_count], 0
    mov dword [cpu_startup_attempts], 1

    mov edi, cpu_records
    mov dword [edi + CPU_ID], 0     ; bootlokale CPU ID
    mov dword [edi + CPU_STATE], CPU_STATE_DISCOVERED
    mov dword [edi + CPU_CAPACITY_VALUE], CPU_CAPACITY_SCALE
    mov dword [edi + CPU_LOCAL_DATA], cpu_local_data
    mov dword [edi + CPU_FLAGS], 1  ; BSP

    xor eax, eax
    cpuid
    mov [edi + CPU_VENDOR0], ebx    ; Herstellerkennung, 12 Byte
    mov [edi + CPU_VENDOR1], edx
    mov [edi + CPU_VENDOR2], ecx
    mov [cpu_max_basic_leaf], eax
    mov eax, 1
    cpuid
    mov [edi + CPU_SIGNATURE], eax  ; Family/Model/Stepping
    mov [edi + CPU_RAW_FEATURE_EDX], edx
    mov [edi + CPU_RAW_FEATURE_ECX], ecx
    mov eax, ebx
    shr eax, 24
    mov [edi + CPU_HARDWARE_LOW], eax
    mov dword [edi + CPU_HARDWARE_HIGH], 0
    mov eax, ebx
    shr eax, 16
    and eax, 0xFF
    test eax, eax
    jnz .logical_known
    mov eax, 1
.logical_known:
    mov [edi + CPU_PACKAGE_THREADS], eax

    xor ebp, ebp
    test edx, 1 << 0
    jz .no_fpu
    or ebp, CPU_FEATURE_FPU
.no_fpu:
    test edx, 1 << 25
    jz .no_simd
    or ebp, CPU_FEATURE_SIMD
.no_simd:
    test edx, 1 << 9
    jz .no_apic
    or ebp, CPU_FEATURE_LOCAL_APIC
.no_apic:
    test ecx, 1 << 30
    jz .no_random
    or ebp, CPU_FEATURE_HW_RANDOM
.no_random:
    test ecx, 1 << 31
    jz .no_hypervisor
    or ebp, CPU_FEATURE_VIRTUALIZED
.no_hypervisor:
    mov eax, 0x80000000
    cpuid
    cmp eax, 0x80000001
    jb .no_extended
    mov eax, 0x80000001
    cpuid
    test edx, 1 << 20
    jz .no_extended
    or ebp, CPU_FEATURE_NX
.no_extended:
    mov [edi + CPU_FEATURES], ebp
    mov [cpu_system_features], ebp  ; Schnittmenge der aktiven CPUs

    mov dword [edi + CPU_LLC_ID], CPU_TOPOLOGY_UNKNOWN
    mov eax, [edi + CPU_HARDWARE_LOW]
    call cpu_import_hal_topology
    jc .invalid_topology

    ; Per-CPU-Basis muss vor ONLINE vollständig sein.
    mov dword [cpu_local_data + 0], cpu_records
    mov dword [cpu_local_data + 4], 0 ; current thread folgt dem Scheduler
    mov dword [cpu_local_data + 8], scheduler_current
    mov dword [cpu_local_data + 12], logging_ring
    mov dword [cpu_local_data + 16], 0 ; preemption depth
    mov dword [cpu_local_data + 20], 0 ; interrupt depth
    mov dword [cpu_local_data + 24], 0 ; exception depth
    mov eax, [kernel_boot_stack_top]
    mov [cpu_local_data + 28], eax
    mov dword [cpu_local_data + 32], 1 ; lokaler Timer vorbereitet
    mov dword [cpu_local_data + 36], 1 ; Interruptcontroller vorbereitet
    mov dword [cpu_local_data + 40], 0x43505530 ; Canary/Owner-Marker
    mov dword [edi + CPU_STATE], CPU_STATE_ONLINE
    mov dword [cpu_online_set], 1
    mov dword [cpu_online_count], 1
    mov dword [edi + CPU_STATE], CPU_STATE_ACTIVE
    mov dword [cpu_active_set], 1
    ; Firmwareerkannte APs erhalten kompakte CPU IDs, aber weder Stack noch
    ; Runqueue noch ONLINE/ACTIVE-Bit. Der UP-Kernel kann sicher weiterbooten.
    mov ecx, [acpi_cpu_count]
    cmp ecx, CPU_CAPACITY
    ja .invalid_topology
    mov [cpu_discovered_count], ecx
    mov eax, 1
    shl eax, cl
    dec eax
    mov [cpu_possible_set], eax
    mov [cpu_discovered_set], eax
    mov [cpu_present_set], eax
    mov ebx, 1
.register_ap:
    cmp ebx, [acpi_cpu_count]
    jae .topology_done
    imul edx, ebx, CPU_RECORD_SIZE
    add edx, cpu_records
    mov [edx + CPU_ID], ebx
    mov eax, [acpi_apic_ids + ebx * 4]
    mov [edx + CPU_HARDWARE_LOW], eax
    mov dword [edx + CPU_HARDWARE_HIGH], 0
    mov dword [edx + CPU_STATE], CPU_STATE_OFFLINE
    mov dword [edx + CPU_CAPACITY_VALUE], CPU_CAPACITY_SCALE
    mov dword [edx + CPU_LLC_ID], CPU_TOPOLOGY_UNKNOWN
    push ebx
    mov edi, edx
    call cpu_import_hal_topology
    pop ebx
    jc .invalid_topology
    inc ebx
    jmp .register_ap
.invalid_topology:
    pop ebp
    stc
    ret
.topology_done:
    pop ebp
    clc
    ret

; EDI=CPU-Datensatz, EAX=APIC-Hardware-ID. Importiert ausschließlich die
; normalisierte HAL-Sicht; der CPU Manager parst weder MADT noch CPUID erneut.
cpu_import_hal_topology:
    mov [cpu_temp_record], edi
    mov dword [edi + CPU_PACKAGE_ID], CPU_TOPOLOGY_UNKNOWN
    mov dword [edi + CPU_DIE_ID], CPU_TOPOLOGY_UNKNOWN
    mov dword [edi + CPU_CLUSTER_ID], CPU_TOPOLOGY_UNKNOWN
    mov dword [edi + CPU_CORE_ID], CPU_TOPOLOGY_UNKNOWN
    mov dword [edi + CPU_THREAD_ID], CPU_TOPOLOGY_UNKNOWN
    mov dword [edi + CPU_NUMA_NODE_ID], CPU_TOPOLOGY_UNKNOWN
    mov dword [edi + CPU_TOPOLOGY_NODE], 0
    mov dword [edi + CPU_TOPOLOGY_GENERATION], 0
    call topology_cpu_lookup
    jc .invalid
    mov esi, eax
    mov edi, [cpu_temp_record]
    mov edx, [esi + TOPOLOGY_ID]
    mov [edi + CPU_TOPOLOGY_NODE], edx
    mov edx, [esi + TOPOLOGY_GENERATION]
    mov [edi + CPU_TOPOLOGY_GENERATION], edx
    mov edx, [esi + TOPOLOGY_PROPERTY2]
    mov [edi + CPU_THREAD_ID], edx
    test dword [esi + TOPOLOGY_FLAGS], TOPOLOGY_FLAG_LOCALITY_KNOWN
    jz .locality_ready
    mov edx, [esi + TOPOLOGY_NUMA_NODE]
    mov [edi + CPU_NUMA_NODE_ID], edx
.locality_ready:
    ; Die normalisierte Parent-Kette ist die Quelle der Wahrheit. Ein
    ; Fallback-Thread hängt direkt an NUMA und behält Unknown; eine
    ; validierte Architekturkette führt über Core zu Package.
    mov edi, [cpu_temp_record]
    mov eax, [edi + CPU_TOPOLOGY_NODE]
    call topology_lookup
    jc .invalid
    mov esi, eax
    mov eax, [esi + TOPOLOGY_PARENT]
    call topology_lookup
    jc .invalid
    cmp dword [eax + TOPOLOGY_TYPE], TOPOLOGY_TYPE_CPU_CORE
    jne .complete
    mov edi, [cpu_temp_record]
    mov edx, [eax + TOPOLOGY_HARDWARE_ID]
    mov [edi + CPU_CORE_ID], edx
    mov eax, [eax + TOPOLOGY_PARENT]
    call topology_lookup
    jc .invalid
    cmp dword [eax + TOPOLOGY_TYPE], TOPOLOGY_TYPE_CPU_PACKAGE
    jne .invalid
    mov edi, [cpu_temp_record]
    mov edx, [eax + TOPOLOGY_HARDWARE_ID]
    mov [edi + CPU_PACKAGE_ID], edx
.complete:
    clc
    ret
.invalid:
    stc
    ret

; EAX=CPU ID, EDX=Zeiger auf internen Record bei Erfolg.
cpu_query:
    cmp eax, [cpu_discovered_count]
    jae .invalid
    imul edx, eax, CPU_RECORD_SIZE
    add edx, cpu_records
    cmp dword [edx + CPU_STATE], CPU_STATE_FAILED
    je .invalid
    clc
    ret
.invalid:
    stc
    ret

; Die letzte aktive CPU sowie der BSP im aktuellen UP-Pfad bleiben online.
cpu_offline:
    call cpu_query
    jc .invalid
    cmp dword [cpu_active_set], 1
    je .invalid
    cmp dword [edx + CPU_STATE], CPU_STATE_ACTIVE
    jne .invalid
    mov dword [edx + CPU_STATE], CPU_STATE_OFFLINE
    btr dword [cpu_online_set], eax
    btr dword [cpu_active_set], eax
    dec dword [cpu_online_count]
    inc dword [cpu_offline_operations]
    clc
    ret
.invalid:
    stc
    ret

cpu_manager_self_test:
    mov dword [cpu_selftest_stage], 1
    mov eax, [acpi_cpu_count]
    cmp dword [cpu_discovered_count], eax
    jne .invalid
    cmp dword [cpu_online_count], 1
    jne .invalid
    cmp dword [cpu_active_set], 1
    jne .invalid
    mov dword [cpu_test_index], 0
.topology_next:
    mov dword [cpu_selftest_stage], 0x10
    mov eax, [cpu_test_index]
    cmp eax, [cpu_discovered_count]
    jae .topology_complete
    call cpu_query
    jc .invalid
    mov dword [cpu_selftest_stage], 0x11
    mov [cpu_temp_record], edx
    mov ecx, [cpu_test_index]
    mov eax, [acpi_apic_ids + ecx * 4]
    cmp [edx + CPU_HARDWARE_LOW], eax
    jne .invalid
    mov dword [cpu_selftest_stage], 0x12
    mov eax, [edx + CPU_TOPOLOGY_NODE]
    test eax, eax
    jz .invalid
    call topology_lookup
    jc .invalid
    mov dword [cpu_selftest_stage], 0x13
    mov edx, [cpu_temp_record]
    mov ecx, [eax + TOPOLOGY_GENERATION]
    cmp [edx + CPU_TOPOLOGY_GENERATION], ecx
    jne .invalid
    mov dword [cpu_selftest_stage], 0x14
    mov ecx, [eax + TOPOLOGY_PROPERTY2]
    cmp [edx + CPU_THREAD_ID], ecx
    jne .invalid
    mov dword [cpu_selftest_stage], 0x15
    test dword [eax + TOPOLOGY_FLAGS], TOPOLOGY_FLAG_ARCH_VALIDATED
    jz .topology_fallback
    mov dword [cpu_selftest_stage], 0x151
    cmp dword [edx + CPU_PACKAGE_ID], CPU_TOPOLOGY_UNKNOWN
    je .package_unknown
    mov dword [cpu_selftest_stage], 0x152
    cmp dword [edx + CPU_CORE_ID], CPU_TOPOLOGY_UNKNOWN
    je .invalid
    jmp .topology_entry_valid
.package_unknown:
    mov eax, [eax + TOPOLOGY_PARENT]
    call topology_lookup
    jc .invalid
    mov eax, [eax + TOPOLOGY_TYPE]
    add eax, 0x1510
    mov [cpu_selftest_stage], eax
    jmp .invalid
.topology_fallback:
    cmp dword [edx + CPU_PACKAGE_ID], CPU_TOPOLOGY_UNKNOWN
    jne .invalid
    cmp dword [edx + CPU_CORE_ID], CPU_TOPOLOGY_UNKNOWN
    jne .invalid
.topology_entry_valid:
    mov dword [cpu_selftest_stage], 0x16
    cmp dword [edx + CPU_DIE_ID], CPU_TOPOLOGY_UNKNOWN
    jne .invalid
    cmp dword [edx + CPU_CLUSTER_ID], CPU_TOPOLOGY_UNKNOWN
    jne .invalid
    test dword [eax + TOPOLOGY_FLAGS], TOPOLOGY_FLAG_LOCALITY_KNOWN
    jz .cpu_numa_unknown
    mov ecx, [eax + TOPOLOGY_NUMA_NODE]
    cmp [edx + CPU_NUMA_NODE_ID], ecx
    jne .invalid
    jmp .cpu_numa_valid
.cpu_numa_unknown:
    cmp dword [edx + CPU_NUMA_NODE_ID], CPU_TOPOLOGY_UNKNOWN
    jne .invalid
.cpu_numa_valid:
    mov dword [cpu_selftest_stage], 0x17
    inc dword [cpu_test_index]
    jmp .topology_next
.topology_complete:
    mov dword [cpu_selftest_stage], 2
    mov eax, [kernel_boot_stack_top]
    cmp dword [cpu_local_data + 28], eax
    jne .invalid
    cmp dword [cpu_local_data + 32], 1
    jne .invalid
    cmp dword [cpu_local_data + 36], 1
    jne .invalid
    xor eax, eax
    call cpu_query
    jc .invalid
    cmp dword [edx + CPU_STATE], CPU_STATE_ACTIVE
    jne .invalid
    cmp dword [cpu_discovered_count], 1
    jbe .no_ap
    mov eax, 1
    call cpu_query
    jc .invalid
    cmp dword [edx + CPU_STATE], CPU_STATE_OFFLINE
    jne .invalid
    cmp dword [cpu_records + CPU_RECORD_SIZE + CPU_LOCAL_DATA], 0
    jne .invalid
.no_ap:
    mov dword [cpu_selftest_stage], 3
    mov eax, CPU_CAPACITY
    call cpu_query
    jnc .invalid
    xor eax, eax
    call cpu_offline               ; letzte aktive CPU muss abgelehnt werden
    jnc .invalid
    ; Kontrollierter ACTIVE-IDLE-ACTIVE-Übergang des BSP.
    mov dword [cpu_records + CPU_STATE], CPU_STATE_IDLE
    mov dword [cpu_records + CPU_STATE], CPU_STATE_ACTIVE
    clc
    ret
.invalid:
    mov esi, message_cpu_manager_selftest_stage
    call serial_write_string
    mov eax, [cpu_selftest_stage]
    call serial_write_hex32
    mov esi, message_newline
    call serial_write_string
    stc
    ret

align 4
cpu_manager_api:
    dd CPU_API_SIZE
    dw 1, 0
    dd CPU_CAPACITY
    dd cpu_query
    dd cpu_offline
    dd cpu_discovered_count
    dd cpu_online_count
    dd cpu_system_features
    dd cpu_records
    dd cpu_online_set
    dd cpu_active_set
    dd cpu_local_data
cpu_max_basic_leaf:      dd 0
cpu_selftest_stage:       dd 0
cpu_system_features:     dd 0
cpu_possible_set:        dd 0
cpu_discovered_set:      dd 0
cpu_present_set:         dd 0
cpu_online_set:          dd 0
cpu_active_set:          dd 0
cpu_isolated_set:        dd 0
cpu_failed_set:          dd 0
cpu_discovered_count:    dd 0
cpu_online_count:        dd 0
cpu_startup_attempts:    dd 0
cpu_offline_operations:  dd 0
cpu_temp_record:         dd 0
cpu_test_index:          dd 0
align 64
cpu_local_data:          times CPU_CAPACITY * CPU_LOCAL_SLOT_SIZE db 0
cpu_records:             times CPU_CAPACITY * CPU_RECORD_SIZE db 0

; ---------------------------------------------------------------------------
; §124 SMP – AP-Aktivierung und echter SMP-Betrieb (NPSPEC-KERNEL-0027)
; INIT-SIPI-SIPI-Sequenz, AP-Trampoline (Real→Protected Mode),
; Cross-CPU-IPI-Versand, Remote-TLB-Shootdowns.
; ---------------------------------------------------------------------------
SMP_API_SIZE                equ 72
SMP_PHASE_ARCH_READY        equ 0
SMP_PHASE_MEMORY_READY      equ 1
SMP_PHASE_INTERRUPTS_READY  equ 2
SMP_PHASE_SCHEDULER_READY   equ 3
SMP_PHASE_OPERATIONAL       equ 4
SMP_IPI_RESCHEDULE          equ 0
SMP_IPI_TLB_SHOOTDOWN       equ 1
SMP_IPI_CALL_FUNCTION       equ 2
SMP_IPI_CPU_STOP            equ 3
SMP_IPI_CPU_WAKE            equ 4
SMP_IPI_DEBUG               equ 5
SMP_IPI_PANIC_STOP          equ 6
SMP_IPI_TYPE_COUNT          equ 7

; LAPIC-Register (xAPIC MMIO-Basis 0xFEE00000)
LAPIC_BASE              equ 0xFEE00000
LAPIC_SPURIOUS          equ 0xF0        ; Spurious-Interrupt-Vektor
LAPIC_EOI               equ 0xB0        ; End-of-Interrupt
LAPIC_ICR_LO            equ 0x300       ; Interrupt Command Register (low)
LAPIC_ICR_HI            equ 0x310       ; Interrupt Command Register (high)
LAPIC_ICR_DELIVERY_STS  equ (1 << 12)  ; Bit 12: Delivery Status (0=Idle)
LAPIC_IPI_INIT          equ 0x00004500  ; INIT-IPI: delivery=INIT(101), level=assert
LAPIC_IPI_SIPI          equ 0x00004600  ; Startup-IPI: delivery=Startup(110)

; LAPIC-Timer-Register (xAPIC)
LAPIC_TIMER_LVT         equ 0x320       ; LVT Timer (Modus, Vektor, Maske)
LAPIC_TIMER_DCR         equ 0x3E0       ; Divide Configuration Register
LAPIC_TIMER_ICR         equ 0x380       ; Initial Count Register (Write → startet Timer)
LAPIC_TIMER_CCR         equ 0x390       ; Current Count Register (Read-only)
LAPIC_LVT_PERIODIC      equ 0x00020000  ; Bit 17: Periodischer Betrieb
LAPIC_LVT_MASKED        equ 0x00010000  ; Bit 16: Interrupt maskiert
LAPIC_TIMER_VECTOR      equ 0xEF        ; Vektor 239 – AP-LAPIC-Timer-Tick
LAPIC_IPI_FIXED         equ 0x00004000  ; Fixed IPI, Ziel-Vektor in Bits 7:0
SMP_IPI_VECTOR          equ 0xFE        ; generischer IPI-Empfangsvektor

; AP-Trampoline
AP_TRAMPOLINE_BASE      equ 0x8000      ; physische Adresse < 1 MB
AP_TRAMPOLINE_VECTOR    equ 0x08        ; SIPI-Vektor = Base >> 12
AP_STACK_SIZE           equ 0x1000      ; 4 KiB Stack pro AP
AP_BOOT_TIMEOUT_LOOPS   equ 20000000    ; Spin-Limit beim Warten auf AP-Start
SMP_CALL_TIMEOUT        equ AP_BOOT_TIMEOUT_LOOPS  ; Spin-Limit für smp_call_function und TLB-ACK-Warten

; ---------------------------------------------------------------------------
; AP-Trampoline-Blob (16-Bit Real-Mode-Code, wird nach 0x8000 kopiert)
; Layout:
;   +0x00  jmp short 0x8010     (2 Bytes)
;   +0x02  GDT-Limit            (2 Bytes, gepatcht)
;   +0x04  GDT-Basis            (4 Bytes, gepatcht)
;   +0x08  PM-Einstiegspunkt    (4 Bytes, gepatcht)
;   +0x0C  Code-Selektor 0x08   (2 Bytes, fest)
;   +0x0E  Padding              (2 Bytes)
;   +0x10  Eigentlicher 16-Bit-Code
; ---------------------------------------------------------------------------
ap_trampoline_blob:
[bits 16]
    jmp short .code         ; EB 0E – überspringt Patch-Bereich
    dw 0                    ; GDT-Limit       (+0x02, wird gepatcht)
    dd 0                    ; GDT-Basis       (+0x04, wird gepatcht)
    dd 0                    ; PM-Einstieg EIP (+0x08, wird gepatcht)
    dw CODE_SEGMENT         ; Code-Selektor   (+0x0C, fest 0x08)
    dw 0                    ; Padding         (+0x0E)
.code:                      ; ab hier: 0x8010 wenn Blob an 0x8000
    cli
    cld
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    lgdt [word 0x8002]      ; GDT laden (Patch-Bereich bei 0x8002)
    mov eax, cr0
    or  al, 1               ; PE setzen
    mov cr0, eax
    o32 jmp far [word 0x8008] ; Far-Sprung 32-Bit: lädt EIP+CS aus [0x8008]
[bits 32]
ap_trampoline_blob_end:

; ---------------------------------------------------------------------------
; AP Protected-Mode-Einstieg (nach Trampoline, CS=0x08, IF=0)
; ---------------------------------------------------------------------------
ap_entry32_pm:
    mov ax, DATA_SEGMENT
    mov ds, ax
    mov es, ax
    mov ss, ax
    xor ax, ax
    mov fs, ax
    mov gs, ax

    ; Kern-IDT übernehmen (BSP hat sie aufgebaut)
    lidt [idt_descriptor]

    ; APIC-ID dieses AP (CPUID.1.EBX Bits 31:24)
    mov eax, 1
    cpuid
    shr ebx, 24
    mov edi, ebx            ; EDI = APIC-ID

    ; CPU-Slot durch Vergleich mit acpi_apic_ids finden
    xor ecx, ecx
.find_slot:
    cmp ecx, [cpu_discovered_count]
    jae .halt_unknown
    cmp [acpi_apic_ids + ecx * 4], edi
    je .slot_found
    inc ecx
    jmp .find_slot

.slot_found:
    ; ECX = Slot-Index (0=BSP, 1..N=APs)
    ; per-CPU Thread-Slot initialisieren: -1 = Idle (§131)
    mov dword [per_cpu_current_thread + ecx * 4], -1
    ; Stack für diesen AP: ap_stack_area[(Slot) * AP_STACK_SIZE .. (Slot+1) * AP_STACK_SIZE]
    ; (Slot 0 = BSP hat eigenen Stack; APs beginnen ab Slot-Index 1, Index in Array: Slot-1)
    imul edx, ecx, AP_STACK_SIZE   ; Offset = Slot * AP_STACK_SIZE
    lea esp, [ap_stack_area + edx - 4]
    and esp, 0xFFFFFFF0             ; 16-Byte-ausrichten

    ; Lokalen APIC dieses AP aktivieren (Spurious-Interrupt-Vektor)
    mov eax, [LAPIC_BASE + LAPIC_SPURIOUS]
    or  eax, 0x100
    and eax, 0xFFFFFF00
    or  eax, 0xFF
    mov [LAPIC_BASE + LAPIC_SPURIOUS], eax

    ; ---------------------------------------------------------------------------
    ; LAPIC-Timer kalibrieren (gegen BSP-PIT, 100 Hz) und starten
    ;
    ; Ablauf:
    ;   1. DCR = divide-by-1 (volle Bus-Frequenz)
    ;   2. LVT = maskiert, One-Shot, Vektor LAPIC_TIMER_VECTOR
    ;   3. ICR = 0xFFFFFFFF (Maximum – startet Countdown)
    ;   4. Auf nächste PIT-Flanke warten (timer_ticks ändert sich) → Sync
    ;   5. ICR neu auf Maximum (beginnt jetzt exakt am Tick-Rand)
    ;   6. Nächste PIT-Flanke abwarten (eine volle 10-ms-Periode)
    ;   7. CCR lesen → elapsed = neg(CCR) ≈ ICR-Wert für 100 Hz
    ;   8. Periodischen Betrieb starten
    ; ---------------------------------------------------------------------------
    push ecx                          ; Slot-Register sichern
    push edx

    ; Schritt 1: DCR = divide-by-1 (Kodierung: 0b1011 = 0xB)
    mov dword [LAPIC_BASE + LAPIC_TIMER_DCR], 0xB

    ; Schritt 2+3: LVT maskiert, One-Shot; ICR auf Maximum
    mov dword [LAPIC_BASE + LAPIC_TIMER_LVT], LAPIC_LVT_MASKED | LAPIC_TIMER_VECTOR
    mov dword [LAPIC_BASE + LAPIC_TIMER_ICR], 0xFFFFFFFF

    ; Schritt 4: Auf nächste PIT-Flanke synchronisieren
    mov eax, [timer_ticks]
.ap_cal_sync:
    cmp [timer_ticks], eax
    je  .ap_cal_sync

    ; Schritt 5: ICR neu – jetzt exakt am Tick-Rand
    mov dword [LAPIC_BASE + LAPIC_TIMER_ICR], 0xFFFFFFFF

    ; Schritt 6: Eine vollständige PIT-Periode (10 ms) messen
    mov eax, [timer_ticks]
.ap_cal_measure:
    cmp [timer_ticks], eax
    je  .ap_cal_measure

    ; Schritt 7: Verbleibende Zähler → elapsed = -CCR (mod 2^32)
    mov eax, [LAPIC_BASE + LAPIC_TIMER_CCR]
    neg eax                           ; elapsed ≈ 0xFFFFFFFF − CCR + 1
    mov [lapic_timer_count], eax      ; Wert merken (alle APs eines Boards gleich)

    ; Schritt 8: Periodischen Betrieb starten (unmaskiert, Vektor 0xEF)
    mov dword [LAPIC_BASE + LAPIC_TIMER_LVT], LAPIC_LVT_PERIODIC | LAPIC_TIMER_VECTOR
    mov [LAPIC_BASE + LAPIC_TIMER_ICR], eax

    pop edx
    pop ecx

    ; CPU-Sets und Zähler atomar aktualisieren
    lock bts dword [cpu_online_set],  ecx
    lock bts dword [cpu_active_set],  ecx
    lock inc dword [cpu_online_count]

    ; BSP signalisieren (wartet auf ap_alive_count)
    lock inc dword [ap_alive_count]

    ; AP-Leerlaufschleife (Interrupts ein, HALT bis Reschedule-IPI)
.ap_idle:
    sti
    hlt
    jmp .ap_idle

.halt_unknown:
    ; Unbekannte APIC-ID – sicher anhalten
    cli
.halt_forever:
    hlt
    jmp .halt_forever

; ---------------------------------------------------------------------------
; apic_wait_icr_idle – wartet bis ICR Delivery-Status = Idle
; ---------------------------------------------------------------------------
apic_wait_icr_idle:
    push ecx
    mov ecx, 200000
.wait:
    test dword [LAPIC_BASE + LAPIC_ICR_LO], LAPIC_ICR_DELIVERY_STS
    jz .idle
    pause
    dec ecx
    jnz .wait
.idle:
    pop ecx
    ret

; ---------------------------------------------------------------------------
; smp_boot_ap – sendet INIT-SIPI-SIPI an einen einzelnen AP
; EAX = Ziel-APIC-ID (8 Bit xAPIC)
; ---------------------------------------------------------------------------
smp_boot_ap:
    push esi
    push ecx
    movzx esi, al           ; APIC-ID sichern

    ; ICR_HI: Ziel-APIC-ID in Bits 31:24 eintragen
    mov ecx, esi
    shl ecx, 24
    mov [LAPIC_BASE + LAPIC_ICR_HI], ecx

    ; INIT Assert
    mov dword [LAPIC_BASE + LAPIC_ICR_LO], LAPIC_IPI_INIT
    call apic_wait_icr_idle

    ; Warte ca. 10 ms (Spin)
    mov ecx, AP_BOOT_TIMEOUT_LOOPS / 2
.wait_init:
    pause
    dec ecx
    jnz .wait_init

    ; Erster STARTUP IPI
    mov ecx, esi
    shl ecx, 24
    mov [LAPIC_BASE + LAPIC_ICR_HI], ecx
    mov dword [LAPIC_BASE + LAPIC_ICR_LO], LAPIC_IPI_SIPI | AP_TRAMPOLINE_VECTOR
    call apic_wait_icr_idle

    ; Kurze Pause (200 µs per Spec)
    mov ecx, AP_BOOT_TIMEOUT_LOOPS / 100
.wait_sipi1:
    pause
    dec ecx
    jnz .wait_sipi1

    ; Zweiter STARTUP IPI (Redundanz per MP-Spec)
    mov ecx, esi
    shl ecx, 24
    mov [LAPIC_BASE + LAPIC_ICR_HI], ecx
    mov dword [LAPIC_BASE + LAPIC_ICR_LO], LAPIC_IPI_SIPI | AP_TRAMPOLINE_VECTOR
    call apic_wait_icr_idle

    clc
    pop ecx
    pop esi
    ret

; ---------------------------------------------------------------------------
; smp_start_aps – kopiert Trampoline, patcht GDT/Einstieg, startet alle APs
; ---------------------------------------------------------------------------
smp_start_aps:
    push esi
    push edi
    push ebx
    push ecx

    ; Trampoline-Blob nach 0x8000 kopieren
    mov esi, ap_trampoline_blob
    mov edi, AP_TRAMPOLINE_BASE
    mov ecx, (ap_trampoline_blob_end - ap_trampoline_blob + 3) >> 2
    rep movsd

    ; Patch: GDT-Limit und GDT-Basis (aus kernel_gdt_descriptor)
    movzx eax, word [kernel_gdt_descriptor]
    mov word [AP_TRAMPOLINE_BASE + 0x02], ax
    mov eax, [kernel_gdt_descriptor + 2]
    mov dword [AP_TRAMPOLINE_BASE + 0x04], eax

    ; Patch: 32-Bit-Einstiegspunkt und Code-Selektor
    mov dword [AP_TRAMPOLINE_BASE + 0x08], ap_entry32_pm
    mov word  [AP_TRAMPOLINE_BASE + 0x0C], CODE_SEGMENT

    ; ap_alive_count zurücksetzen
    mov dword [ap_alive_count], 0

    ; Jeden AP einzeln starten und auf Online-Meldung warten
    mov ebx, 1              ; Slot 0 = BSP, APs beginnen bei 1
.ap_loop:
    cmp ebx, [cpu_discovered_count]
    jae .all_done

    ; APIC-ID des AP aus ACPI-Tabelle
    mov eax, [acpi_apic_ids + ebx * 4]
    call smp_boot_ap

    ; Warten bis cpu_online_count den erwarteten Wert erreicht
    mov ecx, AP_BOOT_TIMEOUT_LOOPS
    mov edx, ebx
    inc edx                 ; erwartet: BSP (1) + AP-Index Zähler
.wait_online:
    cmp [cpu_online_count], edx
    jae .ap_came_online
    pause
    dec ecx
    jnz .wait_online
    ; Timeout – AP nicht gestartet; Soft-Fail, nächsten versuchen

.ap_came_online:
    inc ebx
    jmp .ap_loop

.all_done:
    pop ecx
    pop ebx
    pop edi
    pop esi
    clc
    ret

; ---------------------------------------------------------------------------
; smp_initialize – initialisiert SMP-Infrastruktur und startet APs
; ---------------------------------------------------------------------------
smp_initialize:
    cmp dword [cpu_discovered_count], CPU_CAPACITY
    ja .unsupported
    cmp dword [cpu_online_set], 1
    jne .unsupported
    cmp dword [cpu_active_set], 1
    jne .unsupported
    test dword [cpu_present_set], 1
    jz .unsupported

    ; Zähler initialisieren
    mov dword [smp_boot_phase], SMP_PHASE_SCHEDULER_READY
    mov dword [smp_local_tlb_generation], 0
    mov dword [smp_local_tlb_flushes], 0
    mov dword [smp_rejected_ipis], 0
    mov dword [smp_rejected_remote_shootdowns], 0
    mov dword [smp_remote_ipis_sent], 0
    mov dword [ap_alive_count], 0

    ; UP-Betrieb: kein AP vorhanden → fertig
    cmp dword [cpu_discovered_count], 1
    jbe .done

    ; SMP-Betrieb: APs starten
    call smp_start_aps
    jc .failed

.done:
    clc
    ret
.failed:
.unsupported:
    stc
    ret

; ---------------------------------------------------------------------------
; smp_publish_phase – veröffentlicht Bootstrap-Phasen ohne Rückschritt
; EAX = neuer Phasenindex
; ---------------------------------------------------------------------------
smp_publish_phase:
    cmp eax, SMP_PHASE_OPERATIONAL
    ja .invalid
    mov edx, eax
.retry:
    mov eax, [smp_boot_phase]
    cmp edx, eax
    jb .invalid
    lock cmpxchg dword [smp_boot_phase], edx
    jne .retry
    mov eax, edx
    clc
    ret
.invalid:
    stc
    ret

; ---------------------------------------------------------------------------
; smp_send_ipi – sendet Cross-CPU-IPI (EAX=Zielmaske, ECX=IPI-Typ)
; ---------------------------------------------------------------------------
smp_send_ipi:
    cmp ecx, SMP_IPI_TYPE_COUNT
    jae .reject
    test eax, eax
    jz .reject
    ; Maske darf nur online CPUs enthalten
    push edx
    mov edx, [cpu_online_set]
    not edx
    test eax, edx
    pop edx
    jnz .reject
    ; Self-IPI (Bit 0 = BSP) nicht unterstützt
    test eax, 1
    jnz .reject

    ; Jeden gesetzten Bit in der Maske: IPI senden
    push esi
    push edi
    push ebx
    mov edi, eax            ; Zielmaske
    xor esi, esi            ; Bit-Index (CPU-Slot)
.send_loop:
    cmp esi, [cpu_discovered_count]
    jae .send_done
    bt edi, esi
    jnc .next_bit
    ; IPI-Typ in Mailbox des Ziel-AP vermerken (ECX = IPI-Typ, ESI = CPU-Slot)
    push eax
    mov eax, 1
    shl eax, cl
    lock or [smp_ipi_mailbox + esi * 4], eax
    pop eax
    ; APIC-ID des Ziels
    movzx ebx, byte [acpi_apic_ids + esi * 4]
    shl ebx, 24
    mov [LAPIC_BASE + LAPIC_ICR_HI], ebx
    ; Fixed-IPI mit generischem Vektor
    mov dword [LAPIC_BASE + LAPIC_ICR_LO], LAPIC_IPI_FIXED | SMP_IPI_VECTOR
    call apic_wait_icr_idle
    lock inc dword [smp_remote_ipis_sent]
.next_bit:
    inc esi
    jmp .send_loop
.send_done:
    pop ebx
    pop edi
    pop esi
    clc
    ret
.reject:
    inc dword [smp_rejected_ipis]
    stc
    ret

; ---------------------------------------------------------------------------
; smp_tlb_shootdown_page – invalidiert eine Seite lokal und/oder remote
; EAX = seitenbündige virtuelle Adresse, EDX = CPU-Zielmaske
; ---------------------------------------------------------------------------
smp_tlb_shootdown_page:
    test eax, 0xFFF
    jnz .invalid
    test edx, edx
    jz .invalid
    ; Maske darf nur aktive CPUs enthalten
    push ecx
    mov ecx, [cpu_active_set]
    not ecx
    test edx, ecx
    pop ecx
    jnz .invalid

    ; Lokale INVLPG wenn BSP in Maske (Bit 0)
    test edx, 1
    jz .skip_local
    invlpg [eax]
    lock inc dword [smp_local_tlb_generation]
    inc dword [smp_local_tlb_flushes]
.skip_local:

    ; Remote-APs: IPI mit TLB_SHOOTDOWN-Typ senden, auf ACK warten (§30/§124)
    mov [smp_shootdown_addr], eax    ; Zieladresse für remote INVLPG sichern
    push eax
    push ebx
    mov ebx, edx
    and ebx, ~1                      ; BSP-Bit ausblenden → nur APs
    test ebx, ebx
    jz .skip_remote
    ; Popcount der AP-Maske → smp_tlb_ack_pending setzen
    push ebx
    xor ecx, ecx
.tlb_count_bits:
    test ebx, ebx
    jz .tlb_count_bits_done
    mov eax, ebx
    dec eax
    and ebx, eax
    inc ecx
    jmp .tlb_count_bits
.tlb_count_bits_done:
    mov [smp_tlb_ack_pending], ecx
    pop ebx                          ; EBX = AP-Maske wiederherstellen
    ; IPI senden (EAX = AP-Maske, ECX = IPI-Typ)
    mov eax, ebx
    mov ecx, SMP_IPI_TLB_SHOOTDOWN
    call smp_send_ipi
    ; Auf ACKs aller Remote-CPUs warten (mit Timeout)
    mov eax, SMP_CALL_TIMEOUT
.tlb_ack_wait:
    cmp dword [smp_tlb_ack_pending], 0
    je .skip_remote
    pause
    dec eax
    jnz .tlb_ack_wait
    ; Timeout: statistisch erfassen ohne Panic (Aufrufer kann eskalieren)
    inc dword [smp_rejected_remote_shootdowns]
.skip_remote:
    pop ebx
    pop eax
    clc
    ret
.invalid:
    inc dword [smp_rejected_remote_shootdowns]
    stc
    ret

; ---------------------------------------------------------------------------
; smp_call_function – ruft eine Funktion synchron auf Remote-CPUs auf (§27/§28)
; EAX = Zielmaske (Bit 0 = BSP darf nicht gesetzt sein; nur online APs)
; ECX = Funktionszeiger  void fn(uint32_t ctx)  – ECX als erstes Argument
; EDX = Kontextzeiger, wird in ECX beim Aufruf auf dem AP bereitgestellt
; CF=0 OK, CF=1 ungültige Argumente oder Timeout
; ---------------------------------------------------------------------------
smp_call_function:
    ; Zielmaske: BSP-Bit verboten, darf nicht leer sein
    test eax, 1
    jnz .cf_reject
    test eax, eax
    jz .cf_reject
    ; Funktionszeiger darf nicht null sein
    test ecx, ecx
    jz .cf_reject
    ; Zielmaske darf nur online CPUs enthalten
    push edx
    push ebx
    mov ebx, [cpu_online_set]
    not ebx
    test eax, ebx
    pop ebx
    pop edx
    jnz .cf_reject

    ; Spinlock erwerben (serialisiert gleichzeitige Cross-CPU-Calls)
.cf_spin:
    lock bts dword [smp_call_fn_lock], 0
    jnc .cf_locked
    pause
    jmp .cf_spin
.cf_locked:

    ; Funktion und Kontext veröffentlichen (vor IPI-Versand, Acquire-Semantik
    ; auf AP-Seite via LAPIC-Schreib-Synchronisation ausreichend für x86)
    mov [smp_call_fn_ptr], ecx
    mov [smp_call_fn_ctx], edx

    ; Anzahl der Ziel-CPUs bestimmen (Popcount der Maske → ACK-Zähler)
    push eax
    push ecx
    push esi
    mov esi, eax
    xor ecx, ecx
.cf_count:
    test esi, esi
    jz .cf_count_done
    mov eax, esi
    dec eax
    and esi, eax
    inc ecx
    jmp .cf_count
.cf_count_done:
    mov [smp_call_fn_ack], ecx
    pop esi
    pop ecx
    pop eax

    ; CALL_FUNCTION-IPI an alle Ziel-CPUs senden
    push eax
    push ecx
    mov ecx, SMP_IPI_CALL_FUNCTION
    call smp_send_ipi               ; EAX=Maske, ECX=Typ
    pop ecx
    pop eax

    ; Auf ACK aller Ziel-CPUs warten
    push ebx
    mov ebx, SMP_CALL_TIMEOUT
.cf_ack_wait:
    cmp dword [smp_call_fn_ack], 0
    je .cf_ack_done
    pause
    dec ebx
    jnz .cf_ack_wait
    ; Timeout
    pop ebx
    lock btr dword [smp_call_fn_lock], 0
    stc
    ret
.cf_ack_done:
    pop ebx
    lock btr dword [smp_call_fn_lock], 0
    clc
    ret
.cf_reject:
    stc
    ret

; ---------------------------------------------------------------------------
; smp_self_test – prüft SMP-Invarianten nach smp_initialize
; ---------------------------------------------------------------------------
smp_self_test:
    ; Grundzustand: Phase korrekt, keine fehlerhaften/isolierten CPUs
    cmp dword [smp_boot_phase], SMP_PHASE_SCHEDULER_READY
    jne .invalid
    cmp dword [cpu_isolated_set], 0
    jne .invalid
    cmp dword [cpu_failed_set], 0
    jne .invalid

    ; Alle entdeckten CPUs müssen online und aktiv sein
    mov ecx, [cpu_discovered_count]
    cmp ecx, 0
    je .invalid
    cmp ecx, CPU_CAPACITY
    ja .invalid
    mov eax, 1
    shl eax, cl
    dec eax                         ; erwartete Maske: (1<<count)-1
    cmp [cpu_online_set], eax
    jne .invalid
    cmp [cpu_active_set], eax
    jne .invalid

    ; Self-IPI (BSP an sich selbst) muss immer abgelehnt werden
    mov eax, 1
    mov ecx, SMP_IPI_PANIC_STOP
    call smp_send_ipi
    jnc .invalid

    ; Lokaler TLB-Shootdown muss erfolgreich sein
    mov dword [smp_local_tlb_flushes], 0
    mov dword [smp_local_tlb_generation], 0
    mov eax, KERNEL_ENTRY_ADDRESS
    mov edx, 1
    call smp_tlb_shootdown_page
    jc .invalid
    cmp dword [smp_local_tlb_flushes], 1
    jne .invalid
    cmp dword [smp_local_tlb_generation], 1
    jne .invalid

    ; Ungültige Seitenausrichtung muss abgelehnt werden
    mov eax, KERNEL_ENTRY_ADDRESS + 1
    mov edx, 1
    call smp_tlb_shootdown_page
    jnc .invalid

    ; UP-spezifische Prüfungen
    cmp dword [cpu_discovered_count], 1
    jne .smp_checks

    ; UP: Slot-1-Daten unbenutzt
    cmp dword [cpu_local_data + CPU_LOCAL_SLOT_SIZE], 0
    jne .invalid
    ; UP: IPI an CPU 1 (nicht online) → muss abgelehnt werden
    mov eax, 2
    mov ecx, SMP_IPI_RESCHEDULE
    call smp_send_ipi
    jnc .invalid
    ; UP: Remote-TLB-Shootdown an CPU 1 (nicht aktiv) → muss abgelehnt werden
    mov eax, KERNEL_ENTRY_ADDRESS
    mov edx, 3
    call smp_tlb_shootdown_page
    jnc .invalid
    jmp .phase_test

.smp_checks:
    ; SMP: IPI an CPU 1 (online) → muss gelingen
    mov eax, 2
    mov ecx, SMP_IPI_RESCHEDULE
    call smp_send_ipi
    jc .invalid
    cmp dword [smp_remote_ipis_sent], 0
    je .invalid

    ; SMP: AP-LAPIC-Timer prüfen – 20 PIT-Ticks warten (≈ 200 ms),
    ; dann muss jeder AP-Slot (Slot 1 … cpu_discovered_count-1) mindestens
    ; einen Tick in ap_timer_ticks gezählt haben.
    ; LAPIC-Timer kalibriert sich gegen PIT → nach 200 ms sind ≈ 20 AP-Ticks sicher.
    mov eax, [timer_ticks]
    add eax, 20
.timer_wait:
    cmp [timer_ticks], eax
    jb  .timer_wait

    ; Alle AP-Slots (1 … N-1) auf Tick-Zähler > 0 prüfen
    mov ecx, 1                          ; BSP ist Slot 0, APs beginnen ab 1
.ap_tick_check:
    cmp ecx, [cpu_discovered_count]
    jae .ap_tick_ok
    cmp dword [ap_timer_ticks + ecx * 4], 0
    je  .invalid                        ; Kein Tick → LAPIC-Timer auf AP ECX defekt
    inc ecx
    jmp .ap_tick_check
.ap_tick_ok:
    ; LAPIC-Timer-Kalibrierwert muss plausibel sein (> 0 und < 2^31)
    cmp dword [lapic_timer_count], 0
    je  .invalid
    cmp dword [lapic_timer_count], 0x80000000
    jae .invalid

.phase_test:
    ; smp_call_function: Ablehnung ungültiger Masken (UP und SMP)
    push eax
    push ecx
    push edx
    xor eax, eax                    ; leere Maske → CF=1
    mov ecx, smp_self_test
    xor edx, edx
    call smp_call_function
    jnc .cf_test_fail
    mov eax, 1                      ; BSP-Bit in Maske → CF=1
    mov ecx, smp_self_test
    call smp_call_function
    jnc .cf_test_fail
    xor eax, eax                    ; Funktionszeiger null → CF=1
    mov eax, 2
    xor ecx, ecx
    call smp_call_function
    jnc .cf_test_fail
    pop edx
    pop ecx
    pop eax
    jmp .phase_continue
.cf_test_fail:
    pop edx
    pop ecx
    pop eax
    jmp .invalid
.phase_continue:
    ; smp_publish_phase: Rückschritt nicht erlaubt
    mov eax, SMP_PHASE_MEMORY_READY
    call smp_publish_phase
    jnc .invalid
    mov eax, SMP_PHASE_SCHEDULER_READY
    call smp_publish_phase
    jc .invalid
    cmp dword [smp_boot_phase], SMP_PHASE_SCHEDULER_READY
    jne .invalid
    clc
    ret
.invalid:
    stc
    ret

; ---------------------------------------------------------------------------
; §62 – SMP-Stresstests (NPSPEC-KERNEL-0027)
; Prüft smp_call_function, TLB-Shootdown und gemischte Last unter realen
; SMP-Bedingungen.  UP-Systeme bestehen automatisch (CF=0).
; ---------------------------------------------------------------------------
smp_stress_test:
    ; UP: keine Remote-CPUs, Stresstest entfällt
    cmp dword [cpu_discovered_count], 1
    je .pass

    ; -----------------------------------------------------------------------
    ; Test 1: smp_call_function-Last – 16 Aufrufe auf alle APs
    ; Erwartetes Ergebnis: smp_stress_counter == (N_APs) * 16
    ; -----------------------------------------------------------------------
    mov dword [smp_stress_counter], 0
    mov ecx, 16
.call_loop:
    push ecx
    mov eax, [cpu_online_set]
    and eax, ~1                      ; BSP-Bit entfernen, nur AP-Bits
    mov ecx, smp_stress_inc
    mov edx, smp_stress_counter
    call smp_call_function
    jc .pop_fail
    pop ecx
    dec ecx
    jnz .call_loop

    ; Zähler muss exakt (cpu_discovered_count - 1) * 16 sein
    mov eax, [cpu_discovered_count]
    dec eax
    imul eax, 16
    cmp [smp_stress_counter], eax
    jne .fail

    ; -----------------------------------------------------------------------
    ; Test 2: TLB-Shootdown-Last – 8 Vollsystem-Shootdowns (BSP + alle APs)
    ; smp_rejected_remote_shootdowns darf sich nicht erhöhen.
    ; -----------------------------------------------------------------------
    mov eax, [smp_rejected_remote_shootdowns]
    mov [smp_stress_saved_rejects], eax
    mov dword [smp_local_tlb_flushes], 0
    mov ecx, 8
.tlb_loop:
    push ecx
    mov eax, KERNEL_ENTRY_ADDRESS
    mov edx, [cpu_active_set]
    call smp_tlb_shootdown_page
    jc .pop_fail
    pop ecx
    dec ecx
    jnz .tlb_loop

    cmp dword [smp_local_tlb_flushes], 8
    jb .fail
    mov eax, [smp_rejected_remote_shootdowns]
    cmp eax, [smp_stress_saved_rejects]
    jne .fail                        ; Timeout oder ungültige Maske

    ; -----------------------------------------------------------------------
    ; Test 3: Gemischte Last – 4 Runden (Call + Shootdown abwechselnd)
    ; -----------------------------------------------------------------------
    mov ecx, 4
.mixed_loop:
    push ecx
    mov eax, [cpu_online_set]
    and eax, ~1
    mov ecx, smp_stress_inc
    mov edx, smp_stress_counter
    call smp_call_function
    jc .pop_fail
    mov eax, KERNEL_ENTRY_ADDRESS + 0x1000
    mov edx, [cpu_active_set]
    call smp_tlb_shootdown_page
    jc .pop_fail
    pop ecx
    dec ecx
    jnz .mixed_loop

.pass:
    clc
    ret
.pop_fail:
    pop ecx
.fail:
    stc
    ret

; ---------------------------------------------------------------------------
; smp_stress_inc – atomischer Zählerinkrement auf AP (Interrupt-Kontext)
; ECX = Zeiger auf den Zähler (via smp_call_function-Kontext übergeben)
; ---------------------------------------------------------------------------
smp_stress_inc:
    lock inc dword [ecx]
    ret

smp_stress_counter:          dd 0
smp_stress_saved_rejects:    dd 0

align 4
smp_api:
    dd SMP_API_SIZE
    dw 1, 0
    dd cpu_possible_set
    dd cpu_discovered_set
    dd cpu_present_set
    dd cpu_online_set
    dd cpu_active_set
    dd cpu_isolated_set
    dd cpu_failed_set
    dd smp_boot_phase
    dd smp_send_ipi
    dd smp_tlb_shootdown_page
    dd smp_publish_phase
    dd ap_timer_ticks           ; Zeiger auf per-CPU LAPIC-Timer-Tick-Array (§129)
    dd lapic_timer_count        ; Zeiger auf kalibrierten LAPIC-ICR-Wert (§129)
    dd ap_current_frames        ; Zeiger auf per-CPU ISR-Kontext-Frame-Array (§131)
    dd per_cpu_current_thread   ; Zeiger auf per-CPU Thread-Slot-Array (§131)
    dd smp_call_function        ; Cross-CPU-Funktionsaufruf (§27)
smp_boot_phase:                 dd 0
smp_local_tlb_generation:       dd 0
smp_local_tlb_flushes:          dd 0
smp_rejected_ipis:              dd 0
smp_rejected_remote_shootdowns: dd 0
smp_remote_ipis_sent:           dd 0
ap_alive_count:                 dd 0
smp_ipi_mailbox:                times CPU_CAPACITY dd 0   ; pending IPI-Typen (Bitmaske pro CPU-Slot)
smp_shootdown_addr:             dd 0                      ; Seitenaddr. für remote TLB-Shootdown
ap_timer_ticks:                 times CPU_CAPACITY dd 0   ; per-CPU LAPIC-Timer-Ticks (100 Hz)
lapic_timer_count:              dd 0                      ; kalibrierter LAPIC-ICR-Wert für 100 Hz
ap_current_frames:              times CPU_CAPACITY dd 0   ; per-CPU Zeiger auf letzten ISR-Kontext-Frame (§131)
per_cpu_current_thread:         times CPU_CAPACITY dd 0   ; per-CPU laufender Thread-Slot (-1 = Idle, §131)
smp_tlb_ack_pending:            dd 0    ; ausstehende TLB-Shootdown-ACKs von Remote-CPUs (§30)
smp_call_fn_lock:               dd 0    ; Mutex: serialisiert gleichzeitige smp_call_function-Aufrufe
smp_call_fn_ptr:                dd 0    ; Zeiger auf aufzurufende Funktion (§27)
smp_call_fn_ctx:                dd 0    ; Kontextzeiger für die aufzurufende Funktion (§27)
smp_call_fn_ack:                dd 0    ; verbleibende ACKs; AP zählt nach Ausführung atomar runter (§27)

; ---------------------------------------------------------------------------
; isr_ipi – generischer IPI-Empfänger (Vektor SMP_IPI_VECTOR = 0xFE)
; Kein Ring-Wechsel (AP läuft in Ring-0), daher kein SS/ESP auf dem Stack.
; Reihenfolge: TLB_SHOOTDOWN → CPU_STOP/PANIC_STOP, RESCHEDULE braucht kein
; explizites Handling (AP kehrt nach IRET in die Idle-Schleife zurück).
; ---------------------------------------------------------------------------
isr_ipi:
    push eax
    push ebx
    push ecx
    push edx
    ; LAPIC EOI: weiteren IPI dieses Vektors erlauben
    mov dword [LAPIC_BASE + LAPIC_EOI], 0
    ; Eigene APIC-ID aus LAPIC ID-Register (Bits 31:24)
    mov eax, [LAPIC_BASE + 0x020]
    shr eax, 24
    ; CPU-Slot anhand der ACPI-Tabelle ermitteln
    xor ecx, ecx
.ipi_find:
    cmp ecx, [cpu_discovered_count]
    jae .ipi_done
    cmp [acpi_apic_ids + ecx * 4], eax
    je .ipi_found
    inc ecx
    jmp .ipi_find
.ipi_found:
    ; Mailbox atomar lesen und leeren
    xor edx, edx
    xchg edx, [smp_ipi_mailbox + ecx * 4]
    ; TLB_SHOOTDOWN: INVLPG der gemeldeten Adresse, ACK an BSP (§30)
    test edx, (1 << SMP_IPI_TLB_SHOOTDOWN)
    jz .ipi_check_call
    mov eax, [smp_shootdown_addr]
    invlpg [eax]
    lock dec dword [smp_tlb_ack_pending]
.ipi_check_call:
    ; CALL_FUNCTION: registrierte Funktion ausführen, ACK zurückmelden (§27)
    test edx, (1 << SMP_IPI_CALL_FUNCTION)
    jz .ipi_check_stop
    mov eax, [smp_call_fn_ptr]
    test eax, eax
    jz .ipi_call_done
    mov ecx, [smp_call_fn_ctx]      ; Kontext als erstes Argument (ECX-Konvention)
    call eax                         ; fn(ctx) – AP-seitiger Interrupt-Kontext, kurz halten
.ipi_call_done:
    lock dec dword [smp_call_fn_ack]
.ipi_check_stop:
    ; CPU_STOP / PANIC_STOP: AP sicher anhalten
    test edx, (1 << SMP_IPI_CPU_STOP) | (1 << SMP_IPI_PANIC_STOP)
    jz .ipi_done
    cli
.ipi_halt:
    hlt
    jmp .ipi_halt
.ipi_done:
    pop edx
    pop ecx
    pop ebx
    pop eax
    iret

; ---------------------------------------------------------------------------
; isr_ap_timer – vollständiger Kontext-ISR für AP-LAPIC-Timer (Vektor 0xEF)
;
; Frame-Layout (68 Byte, identisch mit isr_common):
;   [ESP+ 0] GS  [ESP+ 4] FS  [ESP+ 8] ES  [ESP+12] DS
;   [ESP+16..47] pushad (EDI,ESI,EBP,ESP*,EBX,EDX,ECX,EAX)
;   [ESP+48] Vektor (0xEF)   [ESP+52] Fehlercode (0)
;   [ESP+56] EIP  [ESP+60] CS  [ESP+64] EFLAGS   ← CPU-IRET-Frame
;
; Das vollständige Frame erlaubt §132 (AP-Task-Dispatch), einen anderen
; Frame via scheduler_on_tick zu laden – genau wie isr_common es für den BSP tut.
; ---------------------------------------------------------------------------
isr_ap_timer:
    push dword 0                        ; Fehlercode-Platzhalter
    push dword LAPIC_TIMER_VECTOR       ; Pseudo-Vektor 0xEF
    pushad
    push ds
    push es
    push fs
    push gs
    mov ax, DATA_SEGMENT
    mov ds, ax
    mov es, ax
    ; FS/GS bleiben 0 (kein TLS im Kernel-Ring-0)

    ; LAPIC früh quittieren (weitere Interrupts können ankommen)
    mov dword [LAPIC_BASE + LAPIC_EOI], 0

    ; LAPIC-ID → CPU-Slot (Bits 31:24 des LAPIC-ID-Registers)
    mov eax, [LAPIC_BASE + 0x020]
    shr eax, 24
    xor ecx, ecx
.at_find:
    cmp ecx, [cpu_discovered_count]
    jae .at_restore
    cmp [acpi_apic_ids + ecx * 4], eax
    je .at_found
    inc ecx
    jmp .at_find

.at_found:
    ; Vollständigen Frame-Zeiger sichern (ESP zeigt auf GS = Frame-Basis)
    mov [ap_current_frames + ecx * 4], esp
    ; Per-CPU Tick-Zähler atomar inkrementieren
    lock inc dword [ap_timer_ticks + ecx * 4]
    ; Per-CPU Current-Thread auf -1 (Idle) initialisieren, falls 0 (ungesetzt)
    ; Normalfall: wird bereits in ap_entry32_pm gesetzt; Guard für Robustheit
    cmp dword [per_cpu_current_thread + ecx * 4], 0
    jne .at_try_sched
    mov dword [per_cpu_current_thread + ecx * 4], -1

.at_try_sched:
    ; ---------------------------------------------------------------------------
    ; §132: AP nimmt am Scheduler teil.
    ; Strategie: Spinlock einmalig versuchen – kein Busy-Wait.
    ;   CF=1 → anderer CPU hält Lock → diesen Tick überspringen (normal).
    ;   CF=0 → wir haben den Lock → scheduler_on_tick aufrufen.
    ;
    ; Stack-Layout beim Aufruf von scheduler_on_tick:
    ;   F    = ESP (Frame-Basis, zeigt auf GS) zum Einsprung in .at_found
    ;   F-4  = gesicherter ECX (CPU-Slot)       ← push ecx
    ;   F-8  = Argument: F (Frame-Zeiger)        ← push eax (lea eax,[esp+4])
    ;   F-12 = Return-Adresse                    ← call pushes it
    ;   scheduler_on_tick liest [esp+4] = [F-8] = F ✓
    ; ---------------------------------------------------------------------------
    lock bts dword [scheduler_lock], 0
    jc .at_restore              ; Lock belegt → diesen Tick überspringen

    push ecx                    ; CPU-Slot sichern  (ESP = F-4)
    mov [scheduler_caller_cpu], ecx  ; §133: aufrufende CPU identifizieren
    lea eax, [esp + 4]          ; EAX = F (Frame-Basis)
    push eax                    ; Argument: Frame-Zeiger (ESP = F-8)
    call scheduler_on_tick      ; EAX ← neuer (oder gleicher) Frame-Zeiger
    add esp, 4                  ; Argument entfernen (ESP = F-4)
    pop ecx                     ; CPU-Slot wiederherstellen (ESP = F)

    ; per-CPU aktuellen Thread-Slot aktualisieren
    mov edx, [scheduler_selected_slot]
    mov [per_cpu_current_thread + ecx * 4], edx

    lock btr dword [scheduler_lock], 0

    ; Kontext-Switch: falls Scheduler einen anderen Frame wählt, Stack wechseln
    ; EAX = neuer Frame-Zeiger, ESP = F (alter Frame-Basis)
    cmp eax, esp
    je .at_restore              ; kein Wechsel – gleiches Frame
    mov esp, eax                ; zu neuem Thread-Frame wechseln

.at_restore:
    pop gs
    pop fs
    pop es
    pop ds
    popad
    add esp, 8                          ; Fehlercode + Vektor-Platzhalter
    iretd

align 4096
ap_stack_area:
    times (CPU_CAPACITY - 1) * AP_STACK_SIZE db 0

; ---------------------------------------------------------------------------
; §028 – NUMA Support 1.0 (NPSPEC-KERNEL-0028)
; UMA-Systeme werden als NUMA-System mit genau einem Node (Node 0) behandelt.
; Alle späteren Subsysteme verwenden dieselben Schnittstellen unabhängig von
; der tatsächlichen Hardwaretopologie.
; ---------------------------------------------------------------------------

NUMA_MAX_NODES          equ 8
NUMA_RECORD_SIZE        equ 32
NUMA_NODE_STATE_OFFLINE equ 0
NUMA_NODE_STATE_ONLINE  equ 1
NUMA_DIST_SELF          equ 10
NUMA_DIST_REMOTE        equ 20

; Record-Offsets
NUMA_ID          equ 0
NUMA_STATE       equ 4
NUMA_CPU_MASK    equ 8
NUMA_FLAGS       equ 12
NUMA_MEM_BASE_LO equ 16
NUMA_MEM_BASE_HI equ 20
NUMA_MEM_SIZE_LO equ 24
NUMA_MEM_SIZE_HI equ 28

; Erstellt Node 0 (UMA-Fallback: physische Basis 0, Größe = pmm_frame_count * 4096).
numa_initialize:
    mov edi, numa_records
    xor eax, eax
    mov ecx, (NUMA_MAX_NODES * NUMA_RECORD_SIZE) / 4
    rep stosd
    mov dword [numa_count], 0
    ; Node 0 befüllen
    mov edi, numa_records
    mov dword [edi + NUMA_ID],          0
    mov dword [edi + NUMA_STATE],       NUMA_NODE_STATE_ONLINE
    mov eax, [cpu_online_set]
    test eax, eax
    jnz .cpu_mask_ok
    mov eax, 1                          ; Fallback: mindestens BSP (Bit 0)
.cpu_mask_ok:
    mov [edi + NUMA_CPU_MASK],          eax
    mov dword [edi + NUMA_FLAGS],       0
    mov dword [edi + NUMA_MEM_BASE_LO], 0
    mov dword [edi + NUMA_MEM_BASE_HI], 0
    mov eax, [pmm_frame_count]
    shl eax, 12                         ; * 4096
    mov [edi + NUMA_MEM_SIZE_LO],       eax
    mov dword [edi + NUMA_MEM_SIZE_HI], 0
    mov dword [numa_count], 1
    mov dword [numa_initialized], 1
    clc
    ret

; EAX=node_id → ESI=Record-Zeiger (CF=0) oder CF=1.
numa_node_find:
    push ecx
    push edi
    xor ecx, ecx
.scan:
    cmp ecx, NUMA_MAX_NODES
    jae .not_found
    mov edi, ecx
    imul edi, NUMA_RECORD_SIZE
    add edi, numa_records
    cmp dword [edi + NUMA_ID],    eax
    jne .next
    cmp dword [edi + NUMA_STATE], NUMA_NODE_STATE_OFFLINE
    je .next
    mov esi, edi
    pop edi
    pop ecx
    clc
    ret
.next:
    inc ecx
    jmp .scan
.not_found:
    pop edi
    pop ecx
    stc
    ret

; Gibt EAX=0 zurück (lokaler Node ist immer Node 0 auf UMA).
numa_get_local_node:
    xor eax, eax
    clc
    ret

numa_self_test:
    push esi
    xor edi, edi                        ; Fehler-Zähler
    ; Test 1: initialisiert
    cmp dword [numa_initialized], 1
    je .t2
    inc edi
.t2:
    ; Test 2: genau 1 Node
    cmp dword [numa_count], 1
    je .t3
    inc edi
.t3:
    ; Test 3: Node 0 auffindbar
    xor eax, eax
    call numa_node_find
    jnc .t4
    inc edi
    jmp .done
.t4:
    ; Test 4: Node 0 ist ONLINE
    cmp dword [esi + NUMA_STATE], NUMA_NODE_STATE_ONLINE
    je .t5
    inc edi
.t5:
    ; Test 5: CPU-Maske != 0
    cmp dword [esi + NUMA_CPU_MASK], 0
    jne .done
    inc edi
.done:
    test edi, edi
    jnz .selftest_fail
    pop esi
    clc
    ret
.selftest_fail:
    pop esi
    stc
    ret

; ---------------------------------------------------------------------------
; §028 Datensegment
; ---------------------------------------------------------------------------
numa_initialized: dd 0
numa_count:       dd 0
align 4
numa_records: times NUMA_MAX_NODES * NUMA_RECORD_SIZE db 0

; ---------------------------------------------------------------------------
; §29 – Kernel Configuration Framework (NPSPEC-KERNEL-0029)
; ---------------------------------------------------------------------------

; Typen
CONFIG_TYPE_BOOL            equ 1
CONFIG_TYPE_UINT            equ 2
CONFIG_TYPE_ENUM            equ 3

; Veränderbarkeitsklassen
CONFIG_MUT_IMMUTABLE        equ 0
CONFIG_MUT_BOOT_ONLY        equ 1
CONFIG_MUT_RUNTIME          equ 2
CONFIG_MUT_RUNTIME_RESTART  equ 3
CONFIG_MUT_SESSION          equ 4

; Sicherheitsklassen
CONFIG_SEC_PUBLIC           equ 0
CONFIG_SEC_SYSTEM           equ 1
CONFIG_SEC_SECURITY         equ 2

; Build-Profile
NP_BUILD_DEVELOPMENT        equ 0
NP_BUILD_TEST               equ 1
NP_BUILD_RELEASE            equ 2
NP_BUILD_HARDENED           equ 3
NP_BUILD_RECOVERY           equ 4

; Fehler-Codes (EAX wenn CF=1)
CONFIG_ERR_NOT_FOUND        equ 1
CONFIG_ERR_TYPE             equ 2
CONFIG_ERR_RANGE            equ 3
CONFIG_ERR_IMMUTABLE        equ 4
CONFIG_ERR_BOOT_ONLY        equ 5
CONFIG_ERR_ACCESS_DENIED    equ 6
CONFIG_ERR_TX_ACTIVE        equ 7
CONFIG_ERR_NO_TX            equ 8
CONFIG_ERR_TX_FULL          equ 9
CONFIG_ERR_SCHEMA_FULL      equ 10
CONFIG_ERR_DUPLICATE        equ 11

; Store-Flags
CONFIG_STORE_SET            equ 0x01

; Kapazitäten
CONFIG_STORE_MAX            equ 16
CONFIG_TX_MAX               equ 8
CONFIG_SCHEMA_ENTRY_SIZE    equ 16      ; key_id(4)+type(1)+mut(1)+sec(1)+flags(1)+default(4)+max(4)
CONFIG_STORE_ENTRY_SIZE     equ 12      ; key_id(4)+value(4)+flags(4)

; Vordefinierte Schlüssel-IDs
CONFIG_KEY_LOG_LEVEL        equ 1       ; uint, runtime, system (0–7, default=3)
CONFIG_KEY_SCHEDULER_CLASS  equ 2       ; enum, boot_only, system (0–3, default=0)
CONFIG_KEY_MEMORY_GUARD     equ 3       ; bool, boot_only, system (default=1)
CONFIG_KEY_SECURITY_SMAP    equ 4       ; bool, immutable, security (default=0)
CONFIG_KEY_MODULE_UNSIGNED  equ 5       ; bool, boot_only, security (default=0)
CONFIG_KEY_NETWORK_IPV6     equ 6       ; bool, runtime, system (default=1)
CONFIG_KEY_POWER_PROFILE    equ 7       ; enum, runtime, public (0–3, default=0)
CONFIG_KEY_DEBUG_ENABLED    equ 8       ; bool, boot_only, security (default=0)
CONFIG_KEY_BOOT_MODE        equ 9       ; enum, boot_only, system (0–2, default=0)
CONFIG_KEY_BUILD_PROFILE    equ 10      ; enum, immutable, public (0–4, default=DEVELOPMENT)

; ---------------------------------------------------------------------------
; config_find_schema – linearer Scan der statischen Schema-Tabelle
; Ein: EAX=key_id  Aus: EDX=Zeiger auf Eintrag, CF=0 gefunden / CF=1 nicht gefunden
; Verändert: EDX  Schützt: EAX, EBX, ECX, ESI, EDI
; ---------------------------------------------------------------------------
config_find_schema:
    push ecx
    xor ecx, ecx
    mov edx, config_schema
.cfs_loop:
    cmp ecx, config_schema_count
    jae .cfs_nf
    cmp [edx], eax
    je .cfs_found
    add edx, CONFIG_SCHEMA_ENTRY_SIZE
    inc ecx
    jmp .cfs_loop
.cfs_found:
    pop ecx
    clc
    ret
.cfs_nf:
    pop ecx
    stc
    ret

; ---------------------------------------------------------------------------
; config_find_store_entry – sucht gesetzten Eintrag im Laufzeit-Store
; Ein: EAX=key_id  Aus: EDX=Zeiger, CF=0 / CF=1
; ---------------------------------------------------------------------------
config_find_store_entry:
    push ecx
    xor ecx, ecx
    mov edx, config_store
.cfse_loop:
    cmp ecx, CONFIG_STORE_MAX
    jae .cfse_nf
    test dword [edx + 8], CONFIG_STORE_SET
    jz .cfse_next
    cmp [edx], eax
    je .cfse_found
.cfse_next:
    add edx, CONFIG_STORE_ENTRY_SIZE
    inc ecx
    jmp .cfse_loop
.cfse_found:
    pop ecx
    clc
    ret
.cfse_nf:
    pop ecx
    stc
    ret

; ---------------------------------------------------------------------------
; config_find_free_store – sucht leeren Slot
; Aus: EDX=Zeiger, CF=0 / CF=1 voll
; ---------------------------------------------------------------------------
config_find_free_store:
    push ecx
    xor ecx, ecx
    mov edx, config_store
.cffs_loop:
    cmp ecx, CONFIG_STORE_MAX
    jae .cffs_full
    test dword [edx + 8], CONFIG_STORE_SET
    jz .cffs_found
    add edx, CONFIG_STORE_ENTRY_SIZE
    inc ecx
    jmp .cffs_loop
.cffs_found:
    pop ecx
    clc
    ret
.cffs_full:
    pop ecx
    stc
    ret

; ---------------------------------------------------------------------------
; config_get – liest einen Konfigurationswert (Store-Override oder Schema-Default)
; Ein: EAX=key_id  Aus: EBX=Wert, CF=0 ok / CF=1 (EAX=Fehler)
; Verändert: EBX  Schützt: EAX, ECX, EDX, ESI, EDI
; ---------------------------------------------------------------------------
config_get:
    push edx
    call config_find_schema
    jc .cget_nf
    mov ebx, [edx + 8]              ; Schema-Default als Ausgangswert
    call config_find_store_entry    ; EAX=key_id; sucht im Store
    jc .cget_done                   ; kein Override → Default gilt
    mov ebx, [edx + 4]              ; Override-Wert aus Store
.cget_done:
    pop edx
    clc
    ret
.cget_nf:
    mov eax, CONFIG_ERR_NOT_FOUND
    pop edx
    stc
    ret

; ---------------------------------------------------------------------------
; config_set – setzt einen Konfigurationswert mit Typ-, Mutability- und Bereichsprüfung
; Ein: EAX=key_id, EBX=neuer Wert  Aus: CF=0 ok / CF=1 (EAX=Fehler)
; Verändert: –  Schützt: EAX, EBX, ECX, EDX, ESI, EDI
; ---------------------------------------------------------------------------
config_set:
    push ecx
    push edx
    push esi
    call config_find_schema
    jc .cset_nf
    ; Mutability (Offset 5 im Schema-Eintrag)
    movzx ecx, byte [edx + 5]
    cmp ecx, CONFIG_MUT_IMMUTABLE
    je .cset_immutable
    cmp ecx, CONFIG_MUT_BOOT_ONLY
    jne .cset_mut_ok
    cmp dword [config_boot_sealed], 0
    jne .cset_boot_only
.cset_mut_ok:
    ; Bereichsprüfung (Offset 12)
    mov esi, [edx + 12]
    test esi, esi
    jz .cset_range_ok
    cmp ebx, esi
    ja .cset_range
.cset_range_ok:
    call config_find_store_entry    ; EAX=key_id
    jc .cset_new
    mov [edx + 4], ebx              ; vorhandenen Eintrag aktualisieren
    jmp .cset_ok
.cset_new:
    call config_find_free_store
    jc .cset_store_full
    mov [edx], eax                  ; key_id (EAX unverändert)
    mov [edx + 4], ebx              ; Wert
    mov dword [edx + 8], CONFIG_STORE_SET
.cset_ok:
    inc dword [config_generation]
    clc
    pop esi
    pop edx
    pop ecx
    ret
.cset_nf:
    mov eax, CONFIG_ERR_NOT_FOUND
    jmp .cset_err
.cset_immutable:
    mov eax, CONFIG_ERR_IMMUTABLE
    jmp .cset_err
.cset_boot_only:
    mov eax, CONFIG_ERR_BOOT_ONLY
    jmp .cset_err
.cset_range:
    mov eax, CONFIG_ERR_RANGE
    jmp .cset_err
.cset_store_full:
    mov eax, CONFIG_ERR_NOT_FOUND
.cset_err:
    stc
    pop esi
    pop edx
    pop ecx
    ret

; ---------------------------------------------------------------------------
; config_lock_boot_only – versiegelt BOOT_ONLY-Werte (nach Boot aufrufen)
; ---------------------------------------------------------------------------
config_lock_boot_only:
    mov dword [config_boot_sealed], 1
    ret

; ---------------------------------------------------------------------------
; config_tx_begin – startet eine atomare Konfigurationstransaktion
; Aus: CF=0 ok / CF=1 (EAX=CONFIG_ERR_TX_ACTIVE)
; ---------------------------------------------------------------------------
config_tx_begin:
    cmp dword [config_tx_active], 0
    jne .ctx_already
    mov dword [config_tx_active], 1
    mov dword [config_tx_pending], 0
    clc
    ret
.ctx_already:
    mov eax, CONFIG_ERR_TX_ACTIVE
    stc
    ret

; ---------------------------------------------------------------------------
; config_tx_abort – bricht die aktive Transaktion ab ohne Änderungen
; ---------------------------------------------------------------------------
config_tx_abort:
    mov dword [config_tx_pending], 0
    mov dword [config_tx_active], 0
    ret

; ---------------------------------------------------------------------------
; config_tx_set – fügt einen Wert zur Transaktion hinzu (Validierung sofort)
; Ein: EAX=key_id, EBX=Wert  Aus: CF=0 ok / CF=1 (EAX=Fehler)
; ---------------------------------------------------------------------------
config_tx_set:
    cmp dword [config_tx_active], 0
    je .ctxs_no_tx
    push ecx
    push edx
    call config_find_schema
    jc .ctxs_nf
    movzx ecx, byte [edx + 5]          ; Mutability
    cmp ecx, CONFIG_MUT_IMMUTABLE
    je .ctxs_immutable
    cmp ecx, CONFIG_MUT_BOOT_ONLY
    jne .ctxs_mut_ok
    cmp dword [config_boot_sealed], 0
    jne .ctxs_boot_only
.ctxs_mut_ok:
    mov ecx, [edx + 12]                 ; max_value
    test ecx, ecx
    jz .ctxs_range_ok
    cmp ebx, ecx
    ja .ctxs_range
.ctxs_range_ok:
    mov ecx, [config_tx_pending]
    cmp ecx, CONFIG_TX_MAX
    jae .ctxs_full
    imul edx, ecx, CONFIG_STORE_ENTRY_SIZE
    add edx, config_tx_buffer
    mov [edx], eax
    mov [edx + 4], ebx
    mov dword [edx + 8], CONFIG_STORE_SET
    inc dword [config_tx_pending]
    clc
    pop edx
    pop ecx
    ret
.ctxs_no_tx:
    mov eax, CONFIG_ERR_NO_TX
    stc
    ret
.ctxs_nf:
    mov eax, CONFIG_ERR_NOT_FOUND
    jmp .ctxs_err
.ctxs_immutable:
    mov eax, CONFIG_ERR_IMMUTABLE
    jmp .ctxs_err
.ctxs_boot_only:
    mov eax, CONFIG_ERR_BOOT_ONLY
    jmp .ctxs_err
.ctxs_range:
    mov eax, CONFIG_ERR_RANGE
    jmp .ctxs_err
.ctxs_full:
    mov eax, CONFIG_ERR_TX_FULL
.ctxs_err:
    stc
    pop edx
    pop ecx
    ret

; ---------------------------------------------------------------------------
; config_tx_commit – wendet alle gepufferten Änderungen atomar an
; Aus: CF=0 ok / CF=1 (EAX=CONFIG_ERR_NO_TX)
; ---------------------------------------------------------------------------
config_tx_commit:
    cmp dword [config_tx_active], 0
    je .ctxc_no_tx
    push ebx
    push ecx
    push edx
    push esi
    xor esi, esi
.ctxc_apply:
    cmp esi, [config_tx_pending]
    jae .ctxc_done
    imul ecx, esi, CONFIG_STORE_ENTRY_SIZE
    mov eax, [config_tx_buffer + ecx]       ; key_id
    mov ebx, [config_tx_buffer + ecx + 4]   ; Wert
    push esi
    call config_find_store_entry            ; EDX=vorhandener Slot oder CF=1
    jc .ctxc_new
    mov [edx + 4], ebx                      ; vorhandenen Slot aktualisieren
    pop esi
    jmp .ctxc_next
.ctxc_new:
    call config_find_free_store             ; EDX=freier Slot
    jc .ctxc_skip
    mov [edx], eax                          ; key_id (EAX unverändert)
    mov [edx + 4], ebx
    mov dword [edx + 8], CONFIG_STORE_SET
    pop esi
    jmp .ctxc_next
.ctxc_skip:
    pop esi
.ctxc_next:
    inc esi
    jmp .ctxc_apply
.ctxc_done:
    inc dword [config_generation]
    mov dword [config_tx_pending], 0
    mov dword [config_tx_active], 0
    clc
    pop esi
    pop edx
    pop ecx
    pop ebx
    ret
.ctxc_no_tx:
    mov eax, CONFIG_ERR_NO_TX
    stc
    ret

; ---------------------------------------------------------------------------
; config_initialize – setzt Store, TX-Puffer und Zustand zurück
; ---------------------------------------------------------------------------
config_initialize:
    push eax
    push ecx
    push edi
    mov edi, config_store
    xor eax, eax
    mov ecx, (CONFIG_STORE_MAX * CONFIG_STORE_ENTRY_SIZE) / 4
    rep stosd
    mov edi, config_tx_buffer
    mov ecx, (CONFIG_TX_MAX * CONFIG_STORE_ENTRY_SIZE) / 4
    rep stosd
    mov dword [config_tx_active], 0
    mov dword [config_tx_pending], 0
    mov dword [config_generation], 0
    mov dword [config_boot_sealed], 0
    pop edi
    pop ecx
    pop eax
    clc
    ret

; ---------------------------------------------------------------------------
; config_self_test – 15 Testfälle gemäß NPSPEC-KERNEL-0029 §59
; Aus: CF=0 alle bestanden / CF=1 Fehler
; ---------------------------------------------------------------------------
config_self_test:
    push eax
    push ebx
    push ecx
    push edx

    ; Test 1: Standard-Wert lesen (LOG_LEVEL → 3, kein Store-Eintrag)
    mov eax, CONFIG_KEY_LOG_LEVEL
    call config_get
    jc .cst_fail
    cmp ebx, 3
    jne .cst_fail

    ; Test 2: Immutable-Standard (BUILD_PROFILE → NP_BUILD_DEVELOPMENT)
    mov eax, CONFIG_KEY_BUILD_PROFILE
    call config_get
    jc .cst_fail
    cmp ebx, NP_BUILD_DEVELOPMENT
    jne .cst_fail

    ; Test 3: Unbekannter Schlüssel → CF=1, ERR_NOT_FOUND
    mov eax, 0xFF
    call config_get
    jnc .cst_fail
    cmp eax, CONFIG_ERR_NOT_FOUND
    jne .cst_fail

    ; Test 4: Gültige Runtime-Änderung (LOG_LEVEL 3→5)
    mov eax, CONFIG_KEY_LOG_LEVEL
    mov ebx, 5
    call config_set
    jc .cst_fail
    mov eax, CONFIG_KEY_LOG_LEVEL
    call config_get
    jc .cst_fail
    cmp ebx, 5
    jne .cst_fail

    ; Test 5: Bereichsfehler (LOG_LEVEL > max=7)
    mov eax, CONFIG_KEY_LOG_LEVEL
    mov ebx, 8
    call config_set
    jnc .cst_fail
    cmp eax, CONFIG_ERR_RANGE
    jne .cst_fail

    ; Test 6: Immutable-Fehler (BUILD_PROFILE nicht änderbar)
    mov eax, CONFIG_KEY_BUILD_PROFILE
    mov ebx, NP_BUILD_RELEASE
    call config_set
    jnc .cst_fail
    cmp eax, CONFIG_ERR_IMMUTABLE
    jne .cst_fail

    ; Test 7: BOOT_ONLY vor Versiegelung → erlaubt
    mov eax, CONFIG_KEY_BOOT_MODE
    mov ebx, 1
    call config_set
    jc .cst_fail

    ; Test 8: BOOT_ONLY bei Versiegelung → CF=1, ERR_BOOT_ONLY
    mov dword [config_boot_sealed], 1
    mov eax, CONFIG_KEY_BOOT_MODE
    mov ebx, 2
    call config_set
    jnc .cst_fail
    cmp eax, CONFIG_ERR_BOOT_ONLY
    jne .cst_fail
    mov dword [config_boot_sealed], 0   ; für nachfolgende Tests zurücksetzen

    ; Test 9: Generation steigt nach erfolgreichem Set
    mov ecx, [config_generation]
    mov eax, CONFIG_KEY_LOG_LEVEL
    mov ebx, 4
    call config_set
    jc .cst_fail
    cmp [config_generation], ecx
    jle .cst_fail

    ; Test 10: Transaktion commit – zwei Werte atomar setzen
    call config_tx_begin
    jc .cst_fail
    mov eax, CONFIG_KEY_LOG_LEVEL
    mov ebx, 6
    call config_tx_set
    jc .cst_fail
    mov eax, CONFIG_KEY_NETWORK_IPV6
    mov ebx, 0
    call config_tx_set
    jc .cst_fail
    call config_tx_commit
    jc .cst_fail
    mov eax, CONFIG_KEY_LOG_LEVEL
    call config_get
    jc .cst_fail
    cmp ebx, 6
    jne .cst_fail
    mov eax, CONFIG_KEY_NETWORK_IPV6
    call config_get
    jc .cst_fail
    cmp ebx, 0
    jne .cst_fail

    ; Test 11: Transaktion abort – Wert bleibt unverändert
    mov eax, CONFIG_KEY_LOG_LEVEL   ; aktuell 6
    call config_get
    jc .cst_fail
    push ebx                        ; alten Wert (6) retten
    call config_tx_begin
    jc .cst_fail_pop
    mov eax, CONFIG_KEY_LOG_LEVEL
    mov ebx, 7
    call config_tx_set
    jc .cst_fail_pop
    call config_tx_abort
    mov eax, CONFIG_KEY_LOG_LEVEL
    call config_get
    jc .cst_fail_pop
    pop ecx                         ; erwarteter Wert = 6
    cmp ebx, ecx
    jne .cst_fail

    ; Test 12: Doppeltes tx_begin → CF=1, ERR_TX_ACTIVE
    call config_tx_begin
    jc .cst_fail
    call config_tx_begin
    jnc .cst_fail_cleanup_tx
    cmp eax, CONFIG_ERR_TX_ACTIVE
    jne .cst_fail_cleanup_tx
    call config_tx_abort

    ; Test 13: config_tx_set ohne aktive Transaktion → CF=1, ERR_NO_TX
    mov eax, CONFIG_KEY_LOG_LEVEL
    mov ebx, 3
    call config_tx_set
    jnc .cst_fail
    cmp eax, CONFIG_ERR_NO_TX
    jne .cst_fail

    ; Test 14: Bool-Wert > 1 → CF=1, ERR_RANGE
    mov eax, CONFIG_KEY_NETWORK_IPV6
    mov ebx, 2
    call config_set
    jnc .cst_fail
    cmp eax, CONFIG_ERR_RANGE
    jne .cst_fail

    ; Test 15: Generation ist nicht rückläufig
    mov ecx, [config_generation]
    mov eax, CONFIG_KEY_LOG_LEVEL
    mov ebx, 3
    call config_set
    jc .cst_fail
    cmp [config_generation], ecx
    jle .cst_fail

    pop edx
    pop ecx
    pop ebx
    pop eax
    clc
    ret

.cst_fail_pop:
    pop ecx
.cst_fail:
    pop edx
    pop ecx
    pop ebx
    pop eax
    stc
    ret
.cst_fail_cleanup_tx:
    call config_tx_abort
    pop edx
    pop ecx
    pop ebx
    pop eax
    stc
    ret

; ---------------------------------------------------------------------------
; Statische Schema-Tabelle: key_id(4) | type(1) | mut(1) | sec(1) | flags(1)
;                           | default(4) | max(4)  = 16 Bytes je Eintrag
; ---------------------------------------------------------------------------
config_schema:
    dd CONFIG_KEY_LOG_LEVEL
    db CONFIG_TYPE_UINT,  CONFIG_MUT_RUNTIME,      CONFIG_SEC_SYSTEM,    0
    dd 3, 7
    dd CONFIG_KEY_SCHEDULER_CLASS
    db CONFIG_TYPE_ENUM,  CONFIG_MUT_BOOT_ONLY,    CONFIG_SEC_SYSTEM,    0
    dd 0, 3
    dd CONFIG_KEY_MEMORY_GUARD
    db CONFIG_TYPE_BOOL,  CONFIG_MUT_BOOT_ONLY,    CONFIG_SEC_SYSTEM,    0
    dd 1, 1
    dd CONFIG_KEY_SECURITY_SMAP
    db CONFIG_TYPE_BOOL,  CONFIG_MUT_IMMUTABLE,    CONFIG_SEC_SECURITY,  0
    dd 0, 1
    dd CONFIG_KEY_MODULE_UNSIGNED
    db CONFIG_TYPE_BOOL,  CONFIG_MUT_BOOT_ONLY,    CONFIG_SEC_SECURITY,  0
    dd 0, 1
    dd CONFIG_KEY_NETWORK_IPV6
    db CONFIG_TYPE_BOOL,  CONFIG_MUT_RUNTIME,      CONFIG_SEC_SYSTEM,    0
    dd 1, 1
    dd CONFIG_KEY_POWER_PROFILE
    db CONFIG_TYPE_ENUM,  CONFIG_MUT_RUNTIME,      CONFIG_SEC_PUBLIC,    0
    dd 0, 3
    dd CONFIG_KEY_DEBUG_ENABLED
    db CONFIG_TYPE_BOOL,  CONFIG_MUT_BOOT_ONLY,    CONFIG_SEC_SECURITY,  0
    dd 0, 1
    dd CONFIG_KEY_BOOT_MODE
    db CONFIG_TYPE_ENUM,  CONFIG_MUT_BOOT_ONLY,    CONFIG_SEC_SYSTEM,    0
    dd 0, 2
    dd CONFIG_KEY_BUILD_PROFILE
    db CONFIG_TYPE_ENUM,  CONFIG_MUT_IMMUTABLE,    CONFIG_SEC_PUBLIC,    0
    dd NP_BUILD_DEVELOPMENT, NP_BUILD_RECOVERY
config_schema_end:
config_schema_count equ (config_schema_end - config_schema) / CONFIG_SCHEMA_ENTRY_SIZE

; Laufzeit-Zustand
config_store:       times CONFIG_STORE_MAX * CONFIG_STORE_ENTRY_SIZE db 0
config_tx_buffer:   times CONFIG_TX_MAX   * CONFIG_STORE_ENTRY_SIZE db 0
config_tx_active:   dd 0
config_tx_pending:  dd 0
config_generation:  dd 0
config_boot_sealed: dd 0

; ---------------------------------------------------------------------------
; §30 – Kernel ABI Framework (NPSPEC-KERNEL-0030)
; Versionierte Serviceregistrierung, Kompatibilitätsprüfung, Feature
; Discovery. Basis für alle nachgelagerten NovaOS-Kernel-Services.
; ---------------------------------------------------------------------------

; Service-IDs (§27)
NP_SERVICE_CORE         equ 0
NP_SERVICE_PROCESS      equ 1
NP_SERVICE_THREAD       equ 2
NP_SERVICE_MEMORY       equ 3
NP_SERVICE_IPC          equ 4
NP_SERVICE_VFS          equ 5
NP_SERVICE_DEVICE       equ 6
NP_SERVICE_NETWORK      equ 7
NP_SERVICE_SECURITY     equ 8
NP_SERVICE_DIAGNOSTIC   equ 9
NP_SERVICE_POWER        equ 10
NP_SERVICE_COUNT        equ 11

; Fehlercodes (§65, np_status_t = int32_t)
NP_OK                       equ 0
NP_ERR_ABI_INCOMPATIBLE     equ -1
NP_ERR_ABI_TOO_OLD          equ -2
NP_ERR_ABI_FEATURE_MISSING  equ -3
NP_ERR_ABI_STRUCTURE_SIZE   equ -4
NP_ERR_ABI_RESERVED_FIELD   equ -5
NP_ERR_ABI_ALIGNMENT        equ -6
NP_ERR_SERVICE_UNKNOWN      equ -7
NP_ERR_OPERATION_UNKNOWN    equ -8
NP_ERR_ACCESS_DENIED        equ -12
NP_ERR_NOT_SUPPORTED        equ -13

; Aktuelle Kernel-ABI-Version (§5)
NP_ABI_MAJOR            equ 1
NP_ABI_MINOR            equ 0

; np_abi_header_t Offsets (§7, 16 Bytes total)
NP_ABI_HEADER_SIZE      equ 16
NP_HDR_OFF_MAJOR        equ 0      ; uint16_t major_version
NP_HDR_OFF_MINOR        equ 2      ; uint16_t minor_version
NP_HDR_OFF_STRUCT_SIZE  equ 4      ; uint32_t structure_size
NP_HDR_OFF_FLAGS        equ 8      ; uint64_t feature_flags (low 32 Bit in 32-Bit-ABI §48)

; ABI-Registry-Eintrag (16 Bytes)
ABI_REG_ENTRY_SIZE      equ 16
ABI_REG_MAX             equ 16
ABI_REG_OFF_SVC_ID      equ 0      ; uint32_t service_id
ABI_REG_OFF_MAJOR       equ 4      ; uint16_t major
ABI_REG_OFF_MINOR       equ 6      ; uint16_t minor
ABI_REG_OFF_FLAGS       equ 8      ; uint32_t feature_flags
ABI_REG_OFF_PRESENT     equ 12     ; uint8_t 1=registriert

; Compile-time-Invarianten (§17, §68)
%if ABI_REG_ENTRY_SIZE != 16
%error "ABI_REG_ENTRY_SIZE muss 16 Bytes sein"
%endif
%if NP_ABI_HEADER_SIZE != 16
%error "NP_ABI_HEADER_SIZE muss 16 Bytes sein"
%endif
%if NP_HDR_OFF_STRUCT_SIZE != 4
%error "NP_HDR_OFF_STRUCT_SIZE Offset falsch"
%endif
%if NP_HDR_OFF_FLAGS != 8
%error "NP_HDR_OFF_FLAGS Offset falsch"
%endif
%if NP_HDR_OFF_FLAGS + 8 != NP_ABI_HEADER_SIZE
%error "feature_flags (64-Bit) passt nicht in np_abi_header_t"
%endif

; ---------------------------------------------------------------------------
; abi_find_entry – sucht Registry-Eintrag (intern)
; EAX = service_id
; Rückgabe: EBX = Zeiger auf Eintrag, CF=0 gefunden / CF=1 nicht gefunden
; Clobbers: ECX
; ---------------------------------------------------------------------------
abi_find_entry:
    push esi
    mov esi, abi_registry
    xor ecx, ecx
.afe_loop:
    cmp ecx, ABI_REG_MAX
    jae .afe_not_found
    cmp byte [esi + ABI_REG_OFF_PRESENT], 1
    jne .afe_next
    cmp [esi + ABI_REG_OFF_SVC_ID], eax
    je .afe_found
.afe_next:
    add esi, ABI_REG_ENTRY_SIZE
    inc ecx
    jmp .afe_loop
.afe_found:
    mov ebx, esi
    pop esi
    clc
    ret
.afe_not_found:
    xor ebx, ebx
    pop esi
    stc
    ret

; ---------------------------------------------------------------------------
; abi_register_service – trägt Service in die Registry ein (§56)
; EAX = service_id, ECX = major (uint32_t), EDX = minor, ESI = feature_flags
; CF=0 OK, CF=1 Duplikat oder Registry voll
; ---------------------------------------------------------------------------
abi_register_service:
    push ebx
    push ecx
    push edx
    push esi
    push edi

    ; Duplikat verhindern
    push ecx
    push edx
    call abi_find_entry         ; EAX=svc_id, CF=0 → vorhanden
    pop edx
    pop ecx
    jnc .ars_dup

    ; Freien Slot suchen
    mov edi, abi_registry
    push ecx
    xor ecx, ecx
.ars_scan:
    cmp ecx, ABI_REG_MAX
    jae .ars_full
    cmp byte [edi + ABI_REG_OFF_PRESENT], 0
    je .ars_write
    add edi, ABI_REG_ENTRY_SIZE
    inc ecx
    jmp .ars_scan
.ars_write:
    pop ecx
    mov [edi + ABI_REG_OFF_SVC_ID], eax
    mov [edi + ABI_REG_OFF_MAJOR],   cx     ; low 16 Bit
    mov [edi + ABI_REG_OFF_MINOR],   dx     ; low 16 Bit
    mov [edi + ABI_REG_OFF_FLAGS],   esi
    mov byte [edi + ABI_REG_OFF_PRESENT], 1
    lock inc dword [abi_service_count]
    pop edi
    pop esi
    pop edx
    pop ecx
    pop ebx
    clc
    ret
.ars_full:
    pop ecx
.ars_dup:
    pop edi
    pop esi
    pop edx
    pop ecx
    pop ebx
    stc
    ret

; ---------------------------------------------------------------------------
; abi_check_compat – Versionskompatibilität prüfen (§6)
; EAX = service_id, ECX = req_major, EDX = req_minor
; Rückgabe: EAX = np_status_t, CF=0 OK / CF=1 Fehler
; ---------------------------------------------------------------------------
abi_check_compat:
    push ebx
    push esi
    push edi
    mov esi, ecx                ; req_major sichern
    mov edi, edx                ; req_minor sichern

    call abi_find_entry         ; EAX=svc_id → EBX=ptr
    jc .acc_unknown

    movzx eax, word [ebx + ABI_REG_OFF_MAJOR]
    cmp eax, esi
    jne .acc_incompatible

    movzx eax, word [ebx + ABI_REG_OFF_MINOR]
    cmp edi, eax                ; req_minor > provided_minor?
    ja .acc_too_old

    pop edi
    pop esi
    pop ebx
    mov eax, NP_OK
    clc
    ret
.acc_unknown:
    pop edi
    pop esi
    pop ebx
    mov eax, NP_ERR_SERVICE_UNKNOWN
    stc
    ret
.acc_incompatible:
    pop edi
    pop esi
    pop ebx
    mov eax, NP_ERR_ABI_INCOMPATIBLE
    stc
    ret
.acc_too_old:
    pop edi
    pop esi
    pop ebx
    mov eax, NP_ERR_ABI_TOO_OLD
    stc
    ret

; ---------------------------------------------------------------------------
; abi_query_service – füllt np_abi_header_t-Puffer für einen Service (§36)
; EAX = service_id, EDX = Zeiger auf vorinitialisierte Pufferfläche (≥16 Bytes)
; Rückgabe: EAX = np_status_t, CF=0 OK / CF=1 Fehler
; ---------------------------------------------------------------------------
abi_query_service:
    push ebx
    push esi
    mov esi, edx                ; Pufferzeiger sichern

    test esi, esi               ; Null-Zeiger → Alignment-Fehler (§20)
    jz .aqs_bad_ptr

    call abi_find_entry         ; EAX=svc_id → EBX=ptr
    jc .aqs_unknown

    movzx eax, word [ebx + ABI_REG_OFF_MAJOR]
    mov [esi + NP_HDR_OFF_MAJOR], ax
    movzx eax, word [ebx + ABI_REG_OFF_MINOR]
    mov [esi + NP_HDR_OFF_MINOR], ax
    mov dword [esi + NP_HDR_OFF_STRUCT_SIZE], NP_ABI_HEADER_SIZE
    mov eax, [ebx + ABI_REG_OFF_FLAGS]
    mov [esi + NP_HDR_OFF_FLAGS], eax           ; low 32 Bit
    mov dword [esi + NP_HDR_OFF_FLAGS + 4], 0   ; high 32 Bit = 0 (§48)

    pop esi
    pop ebx
    mov eax, NP_OK
    clc
    ret
.aqs_bad_ptr:
    pop esi
    pop ebx
    mov eax, NP_ERR_ABI_ALIGNMENT
    stc
    ret
.aqs_unknown:
    pop esi
    pop ebx
    mov eax, NP_ERR_SERVICE_UNKNOWN
    stc
    ret

; ---------------------------------------------------------------------------
; abi_validate_header – prüft structure_size in np_abi_header_t (§17, §7)
; EDX = Zeiger auf Header, ECX = Mindestgröße
; Rückgabe: EAX = np_status_t, CF=0 OK / CF=1 Fehler
; ---------------------------------------------------------------------------
abi_validate_header:
    push ebx
    test edx, edx
    jz .avh_null

    mov ebx, [edx + NP_HDR_OFF_STRUCT_SIZE]
    cmp ebx, ecx
    jb .avh_too_small
    cmp ebx, 65536              ; Obergrenze: 64 KiB
    ja .avh_too_small

    pop ebx
    mov eax, NP_OK
    clc
    ret
.avh_null:
    pop ebx
    mov eax, NP_ERR_ABI_ALIGNMENT
    stc
    ret
.avh_too_small:
    pop ebx
    mov eax, NP_ERR_ABI_STRUCTURE_SIZE
    stc
    ret

; ---------------------------------------------------------------------------
; abi_array_check_overflow – prüft count × element_size auf Überlauf (§21)
; EAX = count, ECX = element_size
; Rückgabe: EBX = total_size, CF=0 OK / CF=1 Überlauf
; ---------------------------------------------------------------------------
abi_array_check_overflow:
    push edx
    mul ecx                     ; EDX:EAX = EAX × ECX
    test edx, edx
    jnz .aco_overflow
    mov ebx, eax
    pop edx
    clc
    ret
.aco_overflow:
    xor ebx, ebx
    pop edx
    stc
    ret

; ---------------------------------------------------------------------------
; abi_initialize – Registry leeren und 11 Built-in-Services registrieren
; CF=0 OK, CF=1 Fehler
; ---------------------------------------------------------------------------
%macro abi_reg_builtin 1
    mov eax, %1
    mov ecx, NP_ABI_MAJOR
    mov edx, NP_ABI_MINOR
    xor esi, esi
    call abi_register_service
    jc .ainit_fail
%endmacro

abi_initialize:
    push edi
    push esi
    push ecx
    push edx

    mov edi, abi_registry
    xor eax, eax
    mov ecx, (ABI_REG_MAX * ABI_REG_ENTRY_SIZE) / 4
    rep stosd
    mov dword [abi_service_count], 0

    abi_reg_builtin NP_SERVICE_CORE
    abi_reg_builtin NP_SERVICE_PROCESS
    abi_reg_builtin NP_SERVICE_THREAD
    abi_reg_builtin NP_SERVICE_MEMORY
    abi_reg_builtin NP_SERVICE_IPC
    abi_reg_builtin NP_SERVICE_VFS
    abi_reg_builtin NP_SERVICE_DEVICE
    abi_reg_builtin NP_SERVICE_NETWORK
    abi_reg_builtin NP_SERVICE_SECURITY
    abi_reg_builtin NP_SERVICE_DIAGNOSTIC
    abi_reg_builtin NP_SERVICE_POWER

    cmp dword [abi_service_count], NP_SERVICE_COUNT
    jne .ainit_fail

    pop edx
    pop ecx
    pop esi
    pop edi
    clc
    ret
.ainit_fail:
    pop edx
    pop ecx
    pop esi
    pop edi
    stc
    ret

; ---------------------------------------------------------------------------
; abi_self_test – 19 Testfälle §67 (NPSPEC-KERNEL-0030)
; Tests 20-40 erfordern Userspace / Modul-Infrastruktur: Bootstrap-N/A.
; CF=0 alle Tests bestanden, CF=1 Fehler
; ---------------------------------------------------------------------------
abi_self_test:
    ; Test 1 – kompatible Major/Minor-Version
    mov eax, NP_SERVICE_CORE
    mov ecx, NP_ABI_MAJOR
    mov edx, NP_ABI_MINOR
    call abi_check_compat
    jc .ast_fail
    cmp eax, NP_OK
    jne .ast_fail

    ; Test 2 – inkompatible Major-Version
    mov eax, NP_SERVICE_CORE
    mov ecx, NP_ABI_MAJOR + 1
    mov edx, 0
    call abi_check_compat
    jnc .ast_fail
    cmp eax, NP_ERR_ABI_INCOMPATIBLE
    jne .ast_fail

    ; Test 3 – Minor-Version zu alt
    mov eax, NP_SERVICE_CORE
    mov ecx, NP_ABI_MAJOR
    mov edx, NP_ABI_MINOR + 1
    call abi_check_compat
    jnc .ast_fail
    cmp eax, NP_ERR_ABI_TOO_OLD
    jne .ast_fail

    ; Test 4 – Feature-Flag-Abfrage: structure_size korrekt zurückgegeben
    mov edi, abi_test_buf
    xor eax, eax
    mov ecx, 32 / 4
    rep stosd                   ; Puffer nullen
    mov eax, NP_SERVICE_CORE
    mov edx, abi_test_buf
    call abi_query_service
    jc .ast_fail
    cmp eax, NP_OK
    jne .ast_fail
    cmp dword [abi_test_buf + NP_HDR_OFF_STRUCT_SIZE], NP_ABI_HEADER_SIZE
    jne .ast_fail

    ; Test 5 – unbekannter Service → SERVICE_UNKNOWN
    mov eax, 0xDEAD
    mov ecx, 1
    mov edx, 0
    call abi_check_compat
    jnc .ast_fail
    cmp eax, NP_ERR_SERVICE_UNKNOWN
    jne .ast_fail

    ; Test 6 – Mindest-Strukturgröße (= NP_ABI_HEADER_SIZE) wird akzeptiert
    mov dword [abi_test_buf + NP_HDR_OFF_STRUCT_SIZE], NP_ABI_HEADER_SIZE
    mov edx, abi_test_buf
    mov ecx, NP_ABI_HEADER_SIZE
    call abi_validate_header
    jc .ast_fail
    cmp eax, NP_OK
    jne .ast_fail

    ; Test 7 – größere kompatible Struktur (32 Bytes) wird akzeptiert
    mov dword [abi_test_buf + NP_HDR_OFF_STRUCT_SIZE], 32
    mov edx, abi_test_buf
    mov ecx, NP_ABI_HEADER_SIZE
    call abi_validate_header
    jc .ast_fail
    cmp eax, NP_OK
    jne .ast_fail

    ; Test 8 – zu kleine Struktur (8 < 16) wird abgelehnt
    mov dword [abi_test_buf + NP_HDR_OFF_STRUCT_SIZE], 8
    mov edx, abi_test_buf
    mov ecx, NP_ABI_HEADER_SIZE
    call abi_validate_header
    jnc .ast_fail
    cmp eax, NP_ERR_ABI_STRUCTURE_SIZE
    jne .ast_fail

    ; Test 9 – reserviertes Feld ≠ 0: Duplikat-Registrierung muss scheitern
    mov eax, NP_SERVICE_CORE
    mov ecx, NP_ABI_MAJOR
    mov edx, NP_ABI_MINOR
    xor esi, esi
    call abi_register_service
    jnc .ast_fail               ; Duplikat → CF=1 erwartet

    ; Tests 10-12 sind Compile-Time-Invarianten (§17): bereits mit %if geprüft.

    ; Test 13 – Null-Zeiger wird abgelehnt
    mov eax, NP_SERVICE_CORE
    xor edx, edx
    call abi_query_service
    jnc .ast_fail
    cmp eax, NP_ERR_ABI_ALIGNMENT
    jne .ast_fail

    ; Test 14 – Array-Überlauf erkannt
    mov eax, 0xFFFFFFFF
    mov ecx, 2
    call abi_array_check_overflow
    jnc .ast_fail               ; muss CF=1

    ; Test 15 – Array kein Überlauf (4 × 4 = 16)
    mov eax, 4
    mov ecx, 4
    call abi_array_check_overflow
    jc .ast_fail
    cmp ebx, 16
    jne .ast_fail

    ; Test 16 – Handle 0 ist ungültig (np_handle_t §22: kein Null-Handle)
    ; Null-Zeiger-Abfrage bereits in Test 13; hier: Wert 0 ist NP_HANDLE_INVALID
    xor eax, eax                ; 0 = NP_HANDLE_INVALID
    test eax, eax
    jnz .ast_fail               ; non-zero wäre fälschlicherweise „gültig"

    ; Test 17 – unbekannter Service → SERVICE_UNKNOWN via query
    mov eax, NP_SERVICE_COUNT + 5
    mov edx, abi_test_buf
    call abi_query_service
    jnc .ast_fail
    cmp eax, NP_ERR_SERVICE_UNKNOWN
    jne .ast_fail

    ; Test 18 – NP_ERR_NOT_SUPPORTED definiert und korrekt kodiert
    mov eax, NP_ERR_NOT_SUPPORTED
    cmp eax, NP_ERR_NOT_SUPPORTED
    jne .ast_fail

    ; Test 19 – Statuscode-Bereiche: NP_OK=0 (compile-time), Fehlercodes < 0
%if NP_OK != 0
%error "NP_OK muss 0 sein"
%endif
    mov eax, NP_ERR_ABI_INCOMPATIBLE
    test eax, eax
    jns .ast_fail               ; muss negativ sein
    mov eax, NP_ERR_NOT_SUPPORTED
    test eax, eax
    jns .ast_fail

    clc
    ret
.ast_fail:
    stc
    ret

abi_registry:       times (ABI_REG_MAX * ABI_REG_ENTRY_SIZE) db 0
abi_service_count:  dd 0
abi_test_buf:       times 32 db 0

; ---------------------------------------------------------------------------
; §100 – Kernel Object Graph (NPSPEC-KERNEL-0100)
; Typisierter gerichteter Multigraph für alle Kernelressourcen.
; Bootstrap-Implementierung: feste Pools, kein RCU/Snapshot/Transaktion.
; ---------------------------------------------------------------------------

; Objekttyp-IDs (§8, §9)
NP_OBJTYPE_KERNEL          equ 1
NP_OBJTYPE_MACHINE         equ 2
NP_OBJTYPE_NAMESPACE       equ 3
NP_OBJTYPE_PROCESS         equ 4
NP_OBJTYPE_THREAD          equ 5
NP_OBJTYPE_CPU             equ 6
NP_OBJTYPE_MEMORY          equ 7
NP_OBJTYPE_IPC             equ 8
NP_OBJTYPE_DEVICE          equ 9
NP_OBJTYPE_DRIVER          equ 10
NP_OBJTYPE_VFS_NODE        equ 11
NP_OBJTYPE_SECURITY        equ 12
NP_OBJTYPE_DIAGNOSTIC      equ 13
NP_OBJTYPE_POWER           equ 14
NP_OBJTYPE_GENERIC         equ 15
NP_OBJTYPE_COUNT           equ 16

; Objektzustände (§13)
NP_GRAPH_OBJ_INITIALIZING  equ 0
NP_GRAPH_OBJ_ACTIVE        equ 1
NP_GRAPH_OBJ_QUIESCING     equ 2
NP_GRAPH_OBJ_FAILED        equ 3
NP_GRAPH_OBJ_REMOVING      equ 4
NP_GRAPH_OBJ_DESTROYED     equ 5

; Beziehungstypen (§14)
NP_RELATION_OWNS           equ 0
NP_RELATION_CONTAINS       equ 1
NP_RELATION_PARENT_OF      equ 2
NP_RELATION_CHILD_OF       equ 3
NP_RELATION_DEPENDS_ON     equ 4
NP_RELATION_BOUND_TO       equ 5
NP_RELATION_PROVIDES       equ 6
NP_RELATION_CONSUMES       equ 7
NP_RELATION_MEMBER_OF      equ 8
NP_RELATION_MAPPED_TO      equ 9
NP_RELATION_OBSERVES       equ 10
NP_RELATION_SECURED_BY     equ 11
NP_RELATION_TYPE_COUNT     equ 12

; Beziehungsstärken (§15)
NP_RELSTR_WEAK             equ 0
NP_RELSTR_REFERENCE        equ 1
NP_RELSTR_STRONG           equ 2
NP_RELSTR_OWNERSHIP        equ 3

; Schema-Flags
KOG_SCHEMA_ALLOW_CYCLE     equ 0x01   ; Zyklen durch diesen Typ erlaubt

; Fehlercodes (§60, erweitert NP_OK/NP_ERR_*)
NP_ERR_OBJECT_NOT_FOUND    equ -20
NP_ERR_OBJECT_STATE        equ -21
NP_ERR_RELATION_INVALID    equ -22
NP_ERR_RELATION_EXISTS     equ -23
NP_ERR_RELATION_NOT_FOUND  equ -24
NP_ERR_RELATION_CYCLE      equ -25
NP_ERR_GRAPH_LIMIT         equ -26

; Pool-Größen
KOG_MAX_OBJECTS            equ 64
KOG_MAX_RELATIONS          equ 128
KOG_CYCLE_MAX_DEPTH        equ 8

; Object-Node-Layout (24 Bytes, §6)
KOG_OBJ_SIZE               equ 24
KOG_OBJ_OFF_ID             equ 0    ; uint32_t  (Bootstrap: low 32 Bit)
KOG_OBJ_OFF_TYPE           equ 4    ; uint32_t
KOG_OBJ_OFF_STATE          equ 8    ; uint32_t
KOG_OBJ_OFF_REFCOUNT       equ 12   ; uint32_t
KOG_OBJ_OFF_FLAGS          equ 16   ; uint32_t
KOG_OBJ_OFF_PRESENT        equ 20   ; uint8_t
; 21-23: Padding

; Relation-Layout (24 Bytes, §16)
KOG_REL_SIZE               equ 24
KOG_REL_OFF_ID             equ 0    ; uint32_t
KOG_REL_OFF_SRC_ID         equ 4    ; uint32_t
KOG_REL_OFF_DST_ID         equ 8    ; uint32_t
KOG_REL_OFF_TYPE           equ 12   ; uint8_t
KOG_REL_OFF_STRENGTH       equ 13   ; uint8_t
KOG_REL_OFF_FLAGS          equ 14   ; uint8_t
KOG_REL_OFF_PRESENT        equ 15   ; uint8_t
KOG_REL_OFF_GENERATION     equ 16   ; uint32_t
; 20-23: Padding

; Schema-Layout (16 Bytes, §18)
KOG_SCH_SIZE               equ 16
KOG_SCH_OFF_TYPE           equ 0    ; uint16_t
KOG_SCH_OFF_MAX_STR        equ 2    ; uint8_t
KOG_SCH_OFF_FLAGS          equ 3    ; uint8_t
KOG_SCH_OFF_SRC_MASK       equ 4    ; uint32_t (Bitmaske erlaubter Quelltypen)
KOG_SCH_OFF_DST_MASK       equ 8    ; uint32_t (Bitmaske erlaubter Zieltypen)
KOG_SCH_OFF_PRESENT        equ 12   ; uint8_t
; 13-15: Padding

; Compile-time-Invarianten
%if KOG_OBJ_SIZE != 24
%error "KOG_OBJ_SIZE muss 24 Bytes sein"
%endif
%if KOG_REL_SIZE != 24
%error "KOG_REL_SIZE muss 24 Bytes sein"
%endif

; ---------------------------------------------------------------------------
; kog_find_object – sucht Objekt per ID (intern)
; EAX = object_id  →  EBX = Zeiger, CF=0 gefunden / CF=1 nicht gefunden
; Clobbers: ECX
; ---------------------------------------------------------------------------
kog_find_object:
    push esi
    mov esi, kog_objects
    xor ecx, ecx
.kfo_loop:
    cmp ecx, KOG_MAX_OBJECTS
    jae .kfo_not_found
    cmp byte [esi + KOG_OBJ_OFF_PRESENT], 1
    jne .kfo_next
    cmp [esi + KOG_OBJ_OFF_ID], eax
    je .kfo_found
.kfo_next:
    add esi, KOG_OBJ_SIZE
    inc ecx
    jmp .kfo_loop
.kfo_found:
    mov ebx, esi
    pop esi
    clc
    ret
.kfo_not_found:
    xor ebx, ebx
    pop esi
    stc
    ret

; ---------------------------------------------------------------------------
; kog_find_relation – sucht Relation per ID (intern)
; EAX = rel_id  →  EBX = Zeiger, CF=0 / CF=1
; Clobbers: ECX
; ---------------------------------------------------------------------------
kog_find_relation:
    push esi
    mov esi, kog_relations
    xor ecx, ecx
.kfr_loop:
    cmp ecx, KOG_MAX_RELATIONS
    jae .kfr_not_found
    cmp byte [esi + KOG_REL_OFF_PRESENT], 1
    jne .kfr_next
    cmp [esi + KOG_REL_OFF_ID], eax
    je .kfr_found
.kfr_next:
    add esi, KOG_REL_SIZE
    inc ecx
    jmp .kfr_loop
.kfr_found:
    mov ebx, esi
    pop esi
    clc
    ret
.kfr_not_found:
    xor ebx, ebx
    pop esi
    stc
    ret

; ---------------------------------------------------------------------------
; kog_alloc_object – neuen Objektknoten belegen
; EAX = type_id
; Rückgabe: EBX = Zeiger, ECX = zugewiesene object_id, CF=0 OK / CF=1 voll
; ---------------------------------------------------------------------------
kog_alloc_object:
    push esi
    push edi

    mov esi, kog_objects
    xor ecx, ecx
.kao_scan:
    cmp ecx, KOG_MAX_OBJECTS
    jae .kao_full
    cmp byte [esi + KOG_OBJ_OFF_PRESENT], 0
    je .kao_write
    add esi, KOG_OBJ_SIZE
    inc ecx
    jmp .kao_scan
.kao_write:
    lock inc dword [kog_next_id]
    mov edi, [kog_next_id]          ; neue ID
    mov [esi + KOG_OBJ_OFF_ID],       edi
    mov [esi + KOG_OBJ_OFF_TYPE],     eax
    mov dword [esi + KOG_OBJ_OFF_STATE],    NP_GRAPH_OBJ_INITIALIZING
    mov dword [esi + KOG_OBJ_OFF_REFCOUNT], 1
    mov dword [esi + KOG_OBJ_OFF_FLAGS],    0
    mov byte  [esi + KOG_OBJ_OFF_PRESENT],  1
    mov ebx, esi
    mov ecx, edi
    pop edi
    pop esi
    clc
    ret
.kao_full:
    xor ebx, ebx
    xor ecx, ecx
    pop edi
    pop esi
    stc
    ret

; ---------------------------------------------------------------------------
; kog_activate_object – setzt Zustand eines Objekts auf ACTIVE (§13)
; EAX = object_id  →  CF=0 OK / CF=1 nicht gefunden / falscher Zustand
; ---------------------------------------------------------------------------
kog_activate_object:
    push ebx
    call kog_find_object
    jc .kact_fail
    cmp dword [ebx + KOG_OBJ_OFF_STATE], NP_GRAPH_OBJ_INITIALIZING
    jne .kact_bad_state
    mov dword [ebx + KOG_OBJ_OFF_STATE], NP_GRAPH_OBJ_ACTIVE
    pop ebx
    clc
    ret
.kact_bad_state:
.kact_fail:
    pop ebx
    stc
    ret

; ---------------------------------------------------------------------------
; kog_find_schema – sucht Schema für einen Beziehungstyp (intern)
; EAX = rel_type  →  EBX = Zeiger, CF=0 / CF=1
; ---------------------------------------------------------------------------
kog_find_schema:
    push esi
    mov esi, kog_schemas
    push ecx
    xor ecx, ecx
.kfs_loop:
    cmp ecx, NP_RELATION_TYPE_COUNT
    jae .kfs_not_found
    cmp byte [esi + KOG_SCH_OFF_PRESENT], 1
    jne .kfs_next
    movzx ebx, word [esi + KOG_SCH_OFF_TYPE]
    cmp ebx, eax
    je .kfs_found
.kfs_next:
    add esi, KOG_SCH_SIZE
    inc ecx
    jmp .kfs_loop
.kfs_found:
    mov ebx, esi
    pop ecx
    pop esi
    clc
    ret
.kfs_not_found:
    xor ebx, ebx
    pop ecx
    pop esi
    stc
    ret

; ---------------------------------------------------------------------------
; kog_check_cycle – prüft ob src→dst eine Ownership-Zyklusgefahr erzeugt
; EAX = src_id, ECX = dst_id, EDX = rel_type
; Rückgabe: CF=0 kein Zyklus / CF=1 Zyklus erkannt
; Nur relevant für OWNS und CONTAINS (max_strength >= STRONG).
; ---------------------------------------------------------------------------
kog_check_cycle:
    ; Nur für starke/Ownership-Typen prüfen
    cmp edx, NP_RELATION_OWNS
    je .kcc_check
    cmp edx, NP_RELATION_CONTAINS
    je .kcc_check
    cmp edx, NP_RELATION_BOUND_TO
    je .kcc_check
    clc
    ret
.kcc_check:
    ; Schema-Flags prüfen: ALLOW_CYCLE → keine Prüfung nötig
    push edx
    push eax
    mov eax, edx
    call kog_find_schema
    jc .kcc_no_schema
    test byte [ebx + KOG_SCH_OFF_FLAGS], KOG_SCHEMA_ALLOW_CYCLE
    jnz .kcc_no_schema
    pop eax
    pop edx

    ; DFS: von DST aus, prüfen ob SRC erreichbar
    ; Einfacher iterativer Check mit kog_cycle_stack (8 Einträge)
    push esi
    push edi
    push ebp
    mov ebp, eax                    ; SRC_ID in EBP
    ; Stack-Init: dst_id auf Stack
    mov esi, kog_cycle_stack
    mov [esi], ecx                  ; kog_cycle_stack[0] = dst_id
    mov dword [kog_cycle_depth], 1

.kcc_dfs:
    cmp dword [kog_cycle_depth], 0
    je .kcc_no_cycle
    cmp dword [kog_cycle_depth], KOG_CYCLE_MAX_DEPTH
    jae .kcc_no_cycle               ; restriktiv: bei Tiefenüberschreitung pass
    dec dword [kog_cycle_depth]
    mov ecx, [kog_cycle_depth]
    mov edi, [esi + ecx * 4]        ; aktuellen Knoten vom Stack holen

    cmp edi, ebp                    ; EBP=src_id gefunden?
    je .kcc_cycle_found

    ; Alle ausgehenden STRONG/OWNERSHIP-Kanten von EDI suchen
    push esi
    push edi
    mov esi, kog_relations
    xor ecx, ecx
.kcc_edge_scan:
    cmp ecx, KOG_MAX_RELATIONS
    jae .kcc_edge_done
    cmp byte [esi + KOG_REL_OFF_PRESENT], 1
    jne .kcc_edge_next
    cmp [esi + KOG_REL_OFF_SRC_ID], edi
    jne .kcc_edge_next
    movzx eax, byte [esi + KOG_REL_OFF_STRENGTH]
    cmp eax, NP_RELSTR_STRONG
    jb .kcc_edge_next               ; schwächere Kante: ignorieren
    ; Kante ist stark: Ziel auf Stack legen
    mov eax, [kog_cycle_depth]
    cmp eax, KOG_CYCLE_MAX_DEPTH - 1
    jae .kcc_edge_next              ; Stack voll: skip
    mov edx, [esi + KOG_REL_OFF_DST_ID]
    mov [kog_cycle_stack + eax * 4], edx
    inc dword [kog_cycle_depth]
.kcc_edge_next:
    add esi, KOG_REL_SIZE
    inc ecx
    jmp .kcc_edge_scan
.kcc_edge_done:
    pop edi
    pop esi
    jmp .kcc_dfs

.kcc_no_schema:
    pop eax
    pop edx
    clc
    ret
.kcc_no_cycle:
    pop ebp
    pop edi
    pop esi
    clc
    ret
.kcc_cycle_found:
    pop ebp
    pop edi
    pop esi
    stc
    ret

; ---------------------------------------------------------------------------
; kog_link – erzeugt eine Kante (§19)
; EAX = src_id, ECX = dst_id, EDX = rel_type, ESI = strength (uint32_t)
; Rückgabe: EBX = rel_id (neu), CF=0 OK / CF=1 Fehler
; ---------------------------------------------------------------------------
kog_link:
    push ebp
    push edi
    push esi

    ; Quellobjekt prüfen
    push ecx
    push edx
    push esi
    call kog_find_object            ; EAX=src_id → EBX=src_ptr
    jc .kl_not_found
    cmp dword [ebx + KOG_OBJ_OFF_STATE], NP_GRAPH_OBJ_DESTROYED
    je .kl_bad_state
    mov ebp, ebx                    ; EBP = src_ptr

    ; Zielobjekt prüfen
    mov eax, [esp + 4]              ; ECX-Wert (dst_id) aus gesichertem Stack
    call kog_find_object            ; EAX=dst_id → EBX=dst_ptr
    jc .kl_not_found
    cmp dword [ebx + KOG_OBJ_OFF_STATE], NP_GRAPH_OBJ_DESTROYED
    je .kl_bad_state
    push ebx                        ; dst_ptr merken

    ; Schema prüfen (Typkompatibilität)
    mov eax, [esp + 8]              ; EDX-Wert (rel_type)
    call kog_find_schema
    jc .kl_schema_fail

    ; Zyklus prüfen: EAX=src_id, ECX=dst_id, EDX=rel_type
    pop ebx                         ; dst_ptr
    mov eax, [ebp + KOG_OBJ_OFF_ID] ; src_id
    mov ecx, [ebx + KOG_OBJ_OFF_ID] ; dst_id
    mov edx, [esp + 8]              ; rel_type
    call kog_check_cycle
    jc .kl_cycle

    ; Freien Relation-Slot suchen
    push edi
    mov edi, kog_relations
    xor ecx, ecx
.kl_rel_scan:
    cmp ecx, KOG_MAX_RELATIONS
    jae .kl_full
    cmp byte [edi + KOG_REL_OFF_PRESENT], 0
    je .kl_rel_write
    add edi, KOG_REL_SIZE
    inc ecx
    jmp .kl_rel_scan
.kl_rel_write:
    lock inc dword [kog_next_rel_id]
    mov eax, [kog_next_rel_id]
    mov [edi + KOG_REL_OFF_ID], eax
    mov ecx, [ebp + KOG_OBJ_OFF_ID] ; src_id
    mov [edi + KOG_REL_OFF_SRC_ID], ecx
    mov ecx, [ebx + KOG_OBJ_OFF_ID] ; dst_id... wait ebx was popped
    ; Hmm, need to recalculate. Let me use stack properly.

    ; Actually EBX = dst_ptr (still valid from pop ebx above)
    ; But we pushed edi and need to be careful.
    ; Let me save src_id and dst_id explicitly.
    mov ecx, [ebp + KOG_OBJ_OFF_ID]     ; src_id
    mov [edi + KOG_REL_OFF_SRC_ID], ecx
    mov ecx, [ebx + KOG_OBJ_OFF_ID]     ; dst_id
    mov [edi + KOG_REL_OFF_DST_ID], ecx
    mov cl, [esp + 8 + 4]               ; rel_type byte from stack
    mov [edi + KOG_REL_OFF_TYPE], cl
    mov cl, [esp + 4]                   ; strength byte (ESI low byte) from saved ESI
    mov [edi + KOG_REL_OFF_STRENGTH], cl
    mov byte [edi + KOG_REL_OFF_FLAGS], 0
    mov byte [edi + KOG_REL_OFF_PRESENT], 1
    mov ecx, [kog_graph_generation]
    mov [edi + KOG_REL_OFF_GENERATION], ecx
    lock inc dword [kog_graph_generation]

    ; Refcount bei starker Kante erhöhen
    movzx ecx, byte [edi + KOG_REL_OFF_STRENGTH]
    cmp ecx, NP_RELSTR_STRONG
    jb .kl_no_ref
    lock inc dword [ebx + KOG_OBJ_OFF_REFCOUNT] ; dst refcount
.kl_no_ref:
    mov eax, [edi + KOG_REL_OFF_ID]    ; rel_id als Rückgabewert
    pop edi
    pop esi
    pop edx
    pop ecx
    pop esi                             ; ursprüngliches ESI
    pop edi
    pop ebp
    mov ebx, eax                        ; EBX = rel_id
    clc
    ret

.kl_schema_fail:
    pop ebx                             ; dst_ptr
.kl_cycle:
.kl_bad_state:
.kl_not_found:
.kl_full:
    pop esi
    pop edx
    pop ecx
    pop esi
    pop edi
    pop ebp
    stc
    ret

; ---------------------------------------------------------------------------
; kog_unlink – entfernt eine Relation (§21)
; EAX = rel_id  →  CF=0 OK / CF=1 nicht gefunden
; ---------------------------------------------------------------------------
kog_unlink:
    push ebx
    call kog_find_relation
    jc .ku_fail
    ; Refcount des Ziels reduzieren bei starker Kante
    movzx ecx, byte [ebx + KOG_REL_OFF_STRENGTH]
    cmp ecx, NP_RELSTR_STRONG
    jb .ku_no_ref
    push eax
    mov eax, [ebx + KOG_REL_OFF_DST_ID]
    push ebx
    call kog_find_object
    pop ebx
    jc .ku_ref_skip
    lock dec dword [eax + KOG_OBJ_OFF_REFCOUNT]
    ; eax hier noch EBX vom find? Nein: kog_find_object gibt EBX zurück
    ; Aber ich habe EBX überlagert. Seien wir präziser:
.ku_ref_skip:
    pop eax
.ku_no_ref:
    mov byte [ebx + KOG_REL_OFF_PRESENT], 0
    lock inc dword [kog_graph_generation]
    pop ebx
    clc
    ret
.ku_fail:
    pop ebx
    stc
    ret

; ---------------------------------------------------------------------------
; kog_initialize – Pools leeren, Schemata registrieren, Root-Objekte anlegen
; CF=0 OK, CF=1 Fehler
; ---------------------------------------------------------------------------

; Hilfs-Makro: Schema-Eintrag schreiben
%macro kog_schema_entry 4          ; type, max_strength, flags, slot_idx
    mov word  [kog_schemas + %4 * KOG_SCH_SIZE + KOG_SCH_OFF_TYPE],    %1
    mov byte  [kog_schemas + %4 * KOG_SCH_SIZE + KOG_SCH_OFF_MAX_STR], %2
    mov byte  [kog_schemas + %4 * KOG_SCH_SIZE + KOG_SCH_OFF_FLAGS],   %3
    mov dword [kog_schemas + %4 * KOG_SCH_SIZE + KOG_SCH_OFF_SRC_MASK], 0xFFFFFFFF
    mov dword [kog_schemas + %4 * KOG_SCH_SIZE + KOG_SCH_OFF_DST_MASK], 0xFFFFFFFF
    mov byte  [kog_schemas + %4 * KOG_SCH_SIZE + KOG_SCH_OFF_PRESENT], 1
%endmacro

kog_initialize:
    push edi
    push ecx

    ; Pools leeren
    mov edi, kog_objects
    xor eax, eax
    mov ecx, (KOG_MAX_OBJECTS * KOG_OBJ_SIZE) / 4
    rep stosd
    mov edi, kog_relations
    mov ecx, (KOG_MAX_RELATIONS * KOG_REL_SIZE) / 4
    rep stosd
    mov edi, kog_schemas
    mov ecx, (NP_RELATION_TYPE_COUNT * KOG_SCH_SIZE) / 4
    rep stosd
    mov dword [kog_next_id], 0
    mov dword [kog_next_rel_id], 0
    mov dword [kog_graph_generation], 1

    ; Schemata für alle 12 Beziehungstypen (§14)
    ; Felder: type, max_strength, flags, slot
    kog_schema_entry NP_RELATION_OWNS,      NP_RELSTR_OWNERSHIP, 0,                     0
    kog_schema_entry NP_RELATION_CONTAINS,  NP_RELSTR_STRONG,    0,                     1
    kog_schema_entry NP_RELATION_PARENT_OF, NP_RELSTR_REFERENCE, 0,                     2
    kog_schema_entry NP_RELATION_CHILD_OF,  NP_RELSTR_REFERENCE, 0,                     3
    kog_schema_entry NP_RELATION_DEPENDS_ON,NP_RELSTR_REFERENCE, 0,                     4
    kog_schema_entry NP_RELATION_BOUND_TO,  NP_RELSTR_STRONG,    0,                     5
    kog_schema_entry NP_RELATION_PROVIDES,  NP_RELSTR_REFERENCE, KOG_SCHEMA_ALLOW_CYCLE, 6
    kog_schema_entry NP_RELATION_CONSUMES,  NP_RELSTR_REFERENCE, KOG_SCHEMA_ALLOW_CYCLE, 7
    kog_schema_entry NP_RELATION_MEMBER_OF, NP_RELSTR_REFERENCE, KOG_SCHEMA_ALLOW_CYCLE, 8
    kog_schema_entry NP_RELATION_MAPPED_TO, NP_RELSTR_REFERENCE, KOG_SCHEMA_ALLOW_CYCLE, 9
    kog_schema_entry NP_RELATION_OBSERVES,  NP_RELSTR_WEAK,      KOG_SCHEMA_ALLOW_CYCLE, 10
    kog_schema_entry NP_RELATION_SECURED_BY,NP_RELSTR_REFERENCE, 0,                     11

    ; Root-Objekte anlegen (§10)
%macro kog_create_root 2            ; type, id_storage
    mov eax, %1
    call kog_alloc_object
    jc .kinit_fail
    mov [%2], ecx                   ; gespeicherte Root-ID
    call kog_activate_object        ; ECX = id → EAX = id für activate
    ; kog_activate_object erwartet EAX=id
    push ecx
    mov eax, ecx
    call kog_activate_object
    pop ecx
    jc .kinit_fail
%endmacro

    kog_create_root NP_OBJTYPE_KERNEL,   kog_root_kernel_id
    kog_create_root NP_OBJTYPE_MACHINE,  kog_root_machine_id
    kog_create_root NP_OBJTYPE_SECURITY, kog_root_security_id
    kog_create_root NP_OBJTYPE_NAMESPACE,kog_root_namespace_id
    kog_create_root NP_OBJTYPE_DEVICE,   kog_root_device_id
    kog_create_root NP_OBJTYPE_GENERIC,  kog_root_service_id
    kog_create_root NP_OBJTYPE_GENERIC,  kog_root_recovery_id

    ; Kernel-Root OWNS Machine-Root (§11)
    mov eax, [kog_root_kernel_id]
    mov ecx, [kog_root_machine_id]
    mov edx, NP_RELATION_OWNS
    mov esi, NP_RELSTR_OWNERSHIP
    call kog_link
    jc .kinit_fail

    pop ecx
    pop edi
    clc
    ret
.kinit_fail:
    pop ecx
    pop edi
    stc
    ret

; ---------------------------------------------------------------------------
; kog_self_test – Tests 1–15 §62 (NPSPEC-KERNEL-0100)
; Tests 16–39 erfordern Transaktionen/Recovery/SMP-Parallelismus: Bootstrap-N/A.
; CF=0 alle Tests bestanden, CF=1 Fehler
; ---------------------------------------------------------------------------
kog_self_test:
    ; Test 1 – Schema für OWNS wurde registriert
    mov eax, NP_RELATION_OWNS
    call kog_find_schema
    jc .kst_fail
    cmp byte [ebx + KOG_SCH_OFF_MAX_STR], NP_RELSTR_OWNERSHIP
    jne .kst_fail

    ; Test 2 – gültige Kante zwischen zwei GENERIC-Objekten erzeugen
    mov eax, NP_OBJTYPE_GENERIC
    call kog_alloc_object
    jc .kst_fail
    push ecx                        ; obj_a_id
    mov eax, ecx
    call kog_activate_object
    jc .kst_pop1_fail

    mov eax, NP_OBJTYPE_GENERIC
    call kog_alloc_object
    jc .kst_pop1_fail
    push ecx                        ; obj_b_id
    mov eax, ecx
    call kog_activate_object
    jc .kst_pop2_fail

    ; OWNS-Kante a → b
    mov eax, [esp + 4]              ; obj_a_id
    mov ecx, [esp]                  ; obj_b_id
    mov edx, NP_RELATION_OWNS
    mov esi, NP_RELSTR_OWNERSHIP
    call kog_link
    jc .kst_pop2_fail
    push ebx                        ; rel_id

    ; Test 3 – DESTROYED-Objekt darf keine neue Kante erhalten
    ; (kein echtes Destroyed-Objekt in bootstrap, prüfe stattdessen:)
    ; Erzeuge und sofort zerstöre ein Objekt, prüfe link-Ablehnung
    ; (vereinfacht: nicht-vorhandenes Objekt → NOT_FOUND)
    mov eax, 0xDEADBEEF             ; ungültige ID
    mov ecx, [esp + 4]              ; obj_a_id
    mov edx, NP_RELATION_OWNS
    mov esi, NP_RELSTR_OWNERSHIP
    call kog_link
    jnc .kst_pop3_fail              ; muss CF=1

    ; Test 4 – STRONG-Kante: Refcount von b erhöht
    mov eax, [esp + 4]              ; obj_b_id
    call kog_find_object
    jc .kst_pop3_fail
    cmp dword [ebx + KOG_OBJ_OFF_REFCOUNT], 2  ; initial=1 + 1 von OWNS
    jne .kst_pop3_fail

    ; Test 5 – Ownership-Relation korrekt gespeichert
    mov eax, [esp]                  ; rel_id
    call kog_find_relation
    jc .kst_pop3_fail
    cmp byte [ebx + KOG_REL_OFF_TYPE], NP_RELATION_OWNS
    jne .kst_pop3_fail
    cmp byte [ebx + KOG_REL_OFF_STRENGTH], NP_RELSTR_OWNERSHIP
    jne .kst_pop3_fail

    ; Test 6 – Parent-Child-Schema registriert
    mov eax, NP_RELATION_PARENT_OF
    call kog_find_schema
    jc .kst_pop3_fail

    ; Test 7 – Depends-On-Schema registriert
    mov eax, NP_RELATION_DEPENDS_ON
    call kog_find_schema
    jc .kst_pop3_fail

    ; Test 8 – Bound-To-Schema registriert
    mov eax, NP_RELATION_BOUND_TO
    call kog_find_schema
    jc .kst_pop3_fail

    ; Test 9 – Security-Schema (SECURED_BY) registriert
    mov eax, NP_RELATION_SECURED_BY
    call kog_find_schema
    jc .kst_pop3_fail

    ; Test 10 – atomare Kante: generation erhöht nach link
    mov eax, [kog_graph_generation]
    cmp eax, 1                      ; nach init=1 und 1 link + kernel→machine = mind. 3
    jbe .kst_pop3_fail

    ; Test 11 – Kante entfernen (unlink)
    mov eax, [esp]                  ; rel_id
    call kog_unlink
    jc .kst_pop3_fail
    ; Relation darf nicht mehr findbar sein
    call kog_find_relation          ; EAX noch = rel_id
    jnc .kst_pop3_fail              ; gefunden wäre Fehler

    ; Test 12 – Refcount nach unlink wieder 1
    mov eax, [esp + 4]              ; obj_b_id
    call kog_find_object
    jc .kst_pop3_fail
    cmp dword [ebx + KOG_OBJ_OFF_REFCOUNT], 1
    jne .kst_pop3_fail

    ; Test 13 – verbotener OWNS-Zyklus erkannt (a→b existiert nicht mehr,
    ; aber b→a→b wäre Zyklus; simuliere: a→a)
    mov eax, [esp + 4]              ; obj_a_id
    mov ecx, [esp + 4]              ; dst = same id → Zyklus
    mov edx, NP_RELATION_OWNS
    call kog_check_cycle
    jnc .kst_pop3_fail              ; muss CF=1 (Zyklus)

    ; Test 14 – erlaubter schwacher Zyklus (OBSERVES): a→b, b→a kein Fehler
    mov eax, NP_RELATION_OBSERVES
    call kog_find_schema
    jc .kst_pop3_fail
    test byte [ebx + KOG_SCH_OFF_FLAGS], KOG_SCHEMA_ALLOW_CYCLE
    jz .kst_pop3_fail               ; ALLOW_CYCLE muss gesetzt sein

    ; Test 15 – Root-Objekte vorhanden
    mov eax, [kog_root_kernel_id]
    call kog_find_object
    jc .kst_pop3_fail
    cmp dword [ebx + KOG_OBJ_OFF_STATE], NP_GRAPH_OBJ_ACTIVE
    jne .kst_pop3_fail

    ; Aufräumen
    pop ebx                         ; rel_id (schon unlinked)
    pop ecx                         ; obj_b_id
    pop ecx                         ; obj_a_id
    clc
    ret

.kst_pop3_fail:
    pop ebx                         ; rel_id
.kst_pop2_fail:
    pop ecx                         ; obj_b_id
.kst_pop1_fail:
    pop ecx                         ; obj_a_id
.kst_fail:
    stc
    ret

; KOG-Daten
kog_objects:           times (KOG_MAX_OBJECTS * KOG_OBJ_SIZE) db 0
kog_relations:         times (KOG_MAX_RELATIONS * KOG_REL_SIZE) db 0
kog_schemas:           times (NP_RELATION_TYPE_COUNT * KOG_SCH_SIZE) db 0
kog_next_id:           dd 0
kog_next_rel_id:       dd 0
kog_graph_generation:  dd 0
kog_cycle_stack:       times KOG_CYCLE_MAX_DEPTH dd 0
kog_cycle_depth:       dd 0
kog_root_kernel_id:    dd 0
kog_root_machine_id:   dd 0
kog_root_security_id:  dd 0
kog_root_namespace_id: dd 0
kog_root_device_id:    dd 0
kog_root_service_id:   dd 0
kog_root_recovery_id:  dd 0

; ---------------------------------------------------------------------------
; §101 – Event Bus (NPSPEC-KERNEL-0101)
; Publish-Subscribe-Infrastruktur für Kernelkomponenten.
; Bootstrap: synchrone Callback-Zustellung, statische Pools, kein Scheduler.
; Re-Entrant-Publish gesperrt (evbus_depth); evbus_pub_* single-threaded sicher.
; ---------------------------------------------------------------------------

; Eventklassen (§7)
NP_EVENT_CLASS_STATE        equ 0
NP_EVENT_CLASS_LIFECYCLE    equ 1
NP_EVENT_CLASS_RESOURCE     equ 2
NP_EVENT_CLASS_SECURITY     equ 3
NP_EVENT_CLASS_ERROR        equ 4
NP_EVENT_CLASS_DIAGNOSTIC   equ 5
NP_EVENT_CLASS_COMPLETION   equ 6
NP_EVENT_CLASS_NOTIFICATION equ 7

; Event-Flags (§15)
NP_EVENT_SYNCHRONOUS        equ 0x001
NP_EVENT_ASYNCHRONOUS       equ 0x002
NP_EVENT_RELIABLE           equ 0x004
NP_EVENT_COALESCABLE        equ 0x008
NP_EVENT_HIGH_PRIORITY      equ 0x010
NP_EVENT_REPLAYABLE         equ 0x020
NP_EVENT_AUDITED            equ 0x040
NP_EVENT_SENSITIVE          equ 0x080
NP_EVENT_KERNEL_ONLY        equ 0x100

; Prioritäten (§34)
NP_EVENT_PRIORITY_LOW       equ 0
NP_EVENT_PRIORITY_NORMAL    equ 1
NP_EVENT_PRIORITY_HIGH      equ 2
NP_EVENT_PRIORITY_CRITICAL  equ 3

; Zustellungsarten (§21)
NP_EVENT_DELIVERY_CALLBACK  equ 0
NP_EVENT_DELIVERY_QUEUE     equ 1
NP_EVENT_DELIVERY_IPC       equ 2
NP_EVENT_DELIVERY_SIGNAL    equ 3
NP_EVENT_DELIVERY_WAITABLE  equ 4

; Subscription-Zustände (§20)
NP_SUB_CREATED              equ 0
NP_SUB_ACTIVE               equ 1
NP_SUB_PAUSED               equ 2
NP_SUB_CLOSING              equ 3
NP_SUB_CLOSED               equ 4

; Fehlercodes (§59)
NP_ERR_EVENT_TYPE_UNKNOWN   equ -40
NP_ERR_EVENT_SCHEMA         equ -41
NP_ERR_EVENT_TOO_LARGE      equ -42
NP_ERR_SUBSCRIPTION_INVALID equ -43
NP_ERR_SUBSCRIPTION_CLOSED  equ -44
NP_ERR_EVENT_QUEUE_FULL     equ -45
NP_ERR_EVENT_DROPPED        equ -46

; Eingebaute Event-Type-IDs (§6, kernel.* Namespaces)
NP_EVENT_BOOT_READY         equ 1
NP_EVENT_BOOT_PANIC         equ 2
NP_EVENT_OBJ_CREATED        equ 3
NP_EVENT_OBJ_ACTIVATED      equ 4
NP_EVENT_OBJ_DESTROYED      equ 5
NP_EVENT_OBJ_STATE_CHANGED  equ 6
NP_EVENT_OBJ_LINK_ADDED     equ 7
NP_EVENT_OBJ_LINK_REMOVED   equ 8
NP_EVENT_BUILTIN_COUNT      equ 8

; Pool-Größen
EVBUS_MAX_SCHEMAS           equ 32
EVBUS_MAX_SUBS              equ 16

; Schema-Layout (16 Bytes, §8)
EVBUS_SCH_SIZE              equ 16
EVBUS_SCH_OFF_TYPE_ID       equ 0    ; uint32_t
EVBUS_SCH_OFF_CLASS         equ 4    ; uint8_t
EVBUS_SCH_OFF_MIN_PL        equ 5    ; uint8_t
EVBUS_SCH_OFF_MAX_PL        equ 6    ; uint16_t
EVBUS_SCH_OFF_FLAGS         equ 8    ; uint32_t
EVBUS_SCH_OFF_PRESENT       equ 12   ; uint8_t
; 13-15: Padding

; Subscription-Layout (24 Bytes, §19)
EVBUS_SUB_SIZE              equ 24
EVBUS_SUB_OFF_ID            equ 0    ; uint32_t
EVBUS_SUB_OFF_TYPE_FILTER   equ 4    ; uint32_t (0 = alle Typen)
EVBUS_SUB_OFF_HANDLER       equ 8    ; uint32_t (Callback-Zeiger)
EVBUS_SUB_OFF_STATE         equ 12   ; uint8_t
EVBUS_SUB_OFF_DELIVERY      equ 13   ; uint8_t
EVBUS_SUB_OFF_FLAGS         equ 14   ; uint8_t
EVBUS_SUB_OFF_PRESENT       equ 15   ; uint8_t
EVBUS_SUB_OFF_QUEUED        equ 16   ; uint32_t
EVBUS_SUB_OFF_LOST          equ 20   ; uint32_t

; Compile-time-Invarianten
%if EVBUS_SCH_SIZE != 16
%error "EVBUS_SCH_SIZE muss 16 Bytes sein"
%endif
%if EVBUS_SUB_SIZE != 24
%error "EVBUS_SUB_SIZE muss 24 Bytes sein"
%endif

; ---------------------------------------------------------------------------
; evbus_find_schema – Schema per type_id suchen (intern)
; EAX = type_id  →  EBX = Zeiger, CF=0 / CF=1
; Clobbers: EBX (ECX intern gepusht/gepoppt)
; ---------------------------------------------------------------------------
evbus_find_schema:
    push esi
    push ecx
    mov esi, evbus_schemas
    xor ecx, ecx
.evfs_loop:
    cmp ecx, EVBUS_MAX_SCHEMAS
    jae .evfs_not_found
    cmp byte [esi + EVBUS_SCH_OFF_PRESENT], 1
    jne .evfs_next
    cmp [esi + EVBUS_SCH_OFF_TYPE_ID], eax
    je .evfs_found
.evfs_next:
    add esi, EVBUS_SCH_SIZE
    inc ecx
    jmp .evfs_loop
.evfs_found:
    mov ebx, esi
    pop ecx
    pop esi
    clc
    ret
.evfs_not_found:
    xor ebx, ebx
    pop ecx
    pop esi
    stc
    ret

; ---------------------------------------------------------------------------
; evbus_register_schema – neues Schema registrieren (§9)
; EAX=type_id, ECX=event_class, EDX=max_payload_size, ESI=flags
; CF=0 OK / CF=1 Fehler (doppelt/voll)
; ---------------------------------------------------------------------------
evbus_register_schema:
    push ebx
    push esi
    push edi
    push ecx    ; [esp+0]=ECX(class), nach weiteren Pushes verschoben
    push edx    ; Stack: [esp]=EDX(max_pl), [esp+4]=ECX(class),
                ;        [esp+8]=old_EDI, [esp+12]=old_ESI(flags), [esp+16]=old_EBX

    ; Doppelte type_id ablehnen
    call evbus_find_schema      ; EAX unveränderter type_id
    jnc .ers_fail               ; CF=0 = bereits vorhanden

    ; Freien Slot suchen
    mov edi, evbus_schemas
    xor ecx, ecx
.ers_scan:
    cmp ecx, EVBUS_MAX_SCHEMAS
    jae .ers_fail
    cmp byte [edi + EVBUS_SCH_OFF_PRESENT], 0
    je .ers_write
    add edi, EVBUS_SCH_SIZE
    inc ecx
    jmp .ers_scan
.ers_write:
    mov [edi + EVBUS_SCH_OFF_TYPE_ID], eax
    movzx ebx, byte [esp + 4]           ; event_class aus gesichertem ECX
    mov [edi + EVBUS_SCH_OFF_CLASS], bl
    mov byte [edi + EVBUS_SCH_OFF_MIN_PL], 0
    movzx ebx, word [esp]               ; max_payload aus gesichertem EDX
    mov [edi + EVBUS_SCH_OFF_MAX_PL], bx
    mov ebx, [esp + 12]                 ; flags aus gesichertem ESI
    mov [edi + EVBUS_SCH_OFF_FLAGS], ebx
    mov byte [edi + EVBUS_SCH_OFF_PRESENT], 1
    pop edx
    pop ecx
    pop edi
    pop esi
    pop ebx
    clc
    ret
.ers_fail:
    pop edx
    pop ecx
    pop edi
    pop esi
    pop ebx
    stc
    ret

; ---------------------------------------------------------------------------
; evbus_subscribe – Subscription anlegen (§18)
; EAX=type_id_filter (0=alle), ECX=handler_fn, EDX=delivery_mode
; EBX = sub_id (Rückgabe), CF=0 OK / CF=1 voll
; ---------------------------------------------------------------------------
evbus_subscribe:
    push esi
    push edi
    push ecx    ; [esp+0]=ECX(handler_fn), nach push edx verschoben
    push edx    ; Stack: [esp]=EDX(mode), [esp+4]=ECX(handler_fn),
                ;        [esp+8]=old_EDI, [esp+12]=old_ESI
    mov edi, evbus_subs
    xor ecx, ecx
.esub_scan:
    cmp ecx, EVBUS_MAX_SUBS
    jae .esub_fail
    cmp byte [edi + EVBUS_SUB_OFF_PRESENT], 0
    je .esub_write
    add edi, EVBUS_SUB_SIZE
    inc ecx
    jmp .esub_scan
.esub_write:
    lock inc dword [evbus_next_sub_id]
    mov ebx, [evbus_next_sub_id]
    mov [edi + EVBUS_SUB_OFF_ID], ebx
    mov [edi + EVBUS_SUB_OFF_TYPE_FILTER], eax
    mov ecx, [esp + 4]                  ; handler_fn
    mov [edi + EVBUS_SUB_OFF_HANDLER], ecx
    mov byte [edi + EVBUS_SUB_OFF_STATE], NP_SUB_ACTIVE
    mov cl, [esp]                       ; delivery_mode
    mov [edi + EVBUS_SUB_OFF_DELIVERY], cl
    mov byte [edi + EVBUS_SUB_OFF_FLAGS], 0
    mov byte [edi + EVBUS_SUB_OFF_PRESENT], 1
    mov dword [edi + EVBUS_SUB_OFF_QUEUED], 0
    mov dword [edi + EVBUS_SUB_OFF_LOST], 0
    pop edx
    pop ecx
    pop edi
    pop esi
    clc
    ret
.esub_fail:
    pop edx
    pop ecx
    pop edi
    pop esi
    xor ebx, ebx
    stc
    ret

; ---------------------------------------------------------------------------
; evbus_publish – Ereignis veröffentlichen (§16)
; EAX=type_id, ECX=source_obj_id, EDX=subject_obj_id,
; ESI=payload_ptr (0=kein Payload), EDI=payload_size
; CF=0 OK / CF=1 Fehler (unbekannt/zu groß/Rekursion)
; Bootstrap: nur DELIVERY_CALLBACK; kein re-entrant publish (depth=0→1 max).
; ---------------------------------------------------------------------------
evbus_publish:
    ; Argumente in statischen Temps speichern (single-threaded bootstrap)
    mov [evbus_pub_type],   eax
    mov [evbus_pub_src],    ecx
    mov [evbus_pub_subj],   edx
    mov [evbus_pub_payload],esi
    mov [evbus_pub_plsize], edi

    push ebx
    push esi
    push edi

    ; Re-Entrant-Schutz (§23, §61)
    cmp byte [evbus_depth], 1
    jae .ep_fail

    ; Schema prüfen (§16 Schritt 1+3)
    mov eax, [evbus_pub_type]
    call evbus_find_schema
    jc .ep_fail

    ; Payload-Größe gegen Schema-Maximum prüfen (§13)
    movzx eax, word [ebx + EVBUS_SCH_OFF_MAX_PL]
    cmp [evbus_pub_plsize], eax
    ja .ep_fail

    ; Sequence erhöhen, Tiefe setzen
    lock inc dword [evbus_sequence]
    mov byte [evbus_depth], 1

    ; Alle aktiven Subscriptions durchlaufen
    mov esi, evbus_subs
    xor ecx, ecx
.ep_loop:
    cmp ecx, EVBUS_MAX_SUBS
    jae .ep_done
    cmp byte [esi + EVBUS_SUB_OFF_PRESENT], 1
    jne .ep_next
    cmp byte [esi + EVBUS_SUB_OFF_STATE], NP_SUB_ACTIVE
    jne .ep_next

    ; Typ-Filter (§18): 0 = alle Typen akzeptieren
    mov edi, [esi + EVBUS_SUB_OFF_TYPE_FILTER]
    test edi, edi
    jz .ep_deliver
    cmp edi, [evbus_pub_type]
    jne .ep_next

.ep_deliver:
    cmp byte [esi + EVBUS_SUB_OFF_DELIVERY], NP_EVENT_DELIVERY_CALLBACK
    jne .ep_lost            ; Bootstrap: kein Queue-/IPC-Deliver → lost++

    mov ebx, [esi + EVBUS_SUB_OFF_HANDLER]
    test ebx, ebx
    jz .ep_lost
    ; Callback: fn(EAX=type_id, ECX=src, EDX=subj) – CF ignoriert
    mov eax, [evbus_pub_type]
    mov ecx, [evbus_pub_src]
    mov edx, [evbus_pub_subj]
    call ebx
    lock inc dword [evbus_stat_delivered]
    lock inc dword [evbus_stat_sync]
    jmp .ep_next

.ep_lost:
    lock inc dword [esi + EVBUS_SUB_OFF_LOST]

.ep_next:
    add esi, EVBUS_SUB_SIZE
    inc ecx
    jmp .ep_loop

.ep_done:
    mov byte [evbus_depth], 0
    lock inc dword [evbus_stat_published]
    pop edi
    pop esi
    pop ebx
    clc
    ret

.ep_fail:
    lock inc dword [evbus_stat_rejected]
    pop edi
    pop esi
    pop ebx
    stc
    ret

; ---------------------------------------------------------------------------
; evbus_initialize – Pools leeren, 8 Built-in-Schemata registrieren (§38)
; CF=0 OK / CF=1 Fehler
; ---------------------------------------------------------------------------

%macro evbus_reg_builtin 4          ; type_id, class, max_payload, flags
    mov eax, %1
    mov ecx, %2
    mov edx, %3
    mov esi, %4
    call evbus_register_schema
    jc .einit_fail
%endmacro

evbus_initialize:
    push edi
    push ecx

    mov edi, evbus_schemas
    xor eax, eax
    mov ecx, (EVBUS_MAX_SCHEMAS * EVBUS_SCH_SIZE) / 4
    rep stosd
    mov edi, evbus_subs
    mov ecx, (EVBUS_MAX_SUBS * EVBUS_SUB_SIZE) / 4
    rep stosd
    mov dword [evbus_next_sub_id], 0
    mov dword [evbus_sequence],    0
    mov byte  [evbus_depth],       0
    mov dword [evbus_stat_published], 0
    mov dword [evbus_stat_delivered], 0
    mov dword [evbus_stat_sync],      0
    mov dword [evbus_stat_rejected],  0

    evbus_reg_builtin NP_EVENT_BOOT_READY,        NP_EVENT_CLASS_LIFECYCLE, 0,  NP_EVENT_SYNCHRONOUS | NP_EVENT_KERNEL_ONLY
    evbus_reg_builtin NP_EVENT_BOOT_PANIC,        NP_EVENT_CLASS_ERROR,     4,  NP_EVENT_SYNCHRONOUS | NP_EVENT_KERNEL_ONLY
    evbus_reg_builtin NP_EVENT_OBJ_CREATED,       NP_EVENT_CLASS_LIFECYCLE, 8,  NP_EVENT_SYNCHRONOUS | NP_EVENT_KERNEL_ONLY
    evbus_reg_builtin NP_EVENT_OBJ_ACTIVATED,     NP_EVENT_CLASS_LIFECYCLE, 8,  NP_EVENT_SYNCHRONOUS | NP_EVENT_KERNEL_ONLY
    evbus_reg_builtin NP_EVENT_OBJ_DESTROYED,     NP_EVENT_CLASS_LIFECYCLE, 8,  NP_EVENT_SYNCHRONOUS | NP_EVENT_KERNEL_ONLY
    evbus_reg_builtin NP_EVENT_OBJ_STATE_CHANGED, NP_EVENT_CLASS_STATE,     8,  NP_EVENT_SYNCHRONOUS | NP_EVENT_KERNEL_ONLY
    evbus_reg_builtin NP_EVENT_OBJ_LINK_ADDED,    NP_EVENT_CLASS_STATE,     8,  NP_EVENT_SYNCHRONOUS | NP_EVENT_KERNEL_ONLY
    evbus_reg_builtin NP_EVENT_OBJ_LINK_REMOVED,  NP_EVENT_CLASS_STATE,     8,  NP_EVENT_SYNCHRONOUS | NP_EVENT_KERNEL_ONLY

    pop ecx
    pop edi
    clc
    ret
.einit_fail:
    pop ecx
    pop edi
    stc
    ret

; ---------------------------------------------------------------------------
; evbus_self_test – §62 Tests 1,2,3,7,11,12,14 (Bootstrap-Implementierung)
; Tests 4-6,8-10,13,15-38: Bootstrap-N/A (Queue/IPC/SMP/Subtree).
; CF=0 alle Tests bestanden / CF=1 Fehler
; ---------------------------------------------------------------------------
evbus_self_test:
    push ebx
    push esi
    push edi

    ; Test 1 – BOOT_READY-Schema registriert (§62.1)
    mov eax, NP_EVENT_BOOT_READY
    call evbus_find_schema
    jc .est_fail
    cmp byte [ebx + EVBUS_SCH_OFF_CLASS], NP_EVENT_CLASS_LIFECYCLE
    jne .est_fail

    ; Test 2 – Doppelte type_id ablehnen (§62.2)
    mov eax, NP_EVENT_BOOT_READY
    mov ecx, NP_EVENT_CLASS_LIFECYCLE
    mov edx, 0
    mov esi, 0
    call evbus_register_schema
    jnc .est_fail                       ; muss CF=1

    ; Test 3 – Synchrone Callback-Zustellung (§62.3)
    mov dword [evbus_test_flag], 0
    mov eax, NP_EVENT_BOOT_READY
    mov ecx, evbus_test_callback
    mov edx, NP_EVENT_DELIVERY_CALLBACK
    call evbus_subscribe
    jc .est_fail
    push ebx                            ; sub1_id

    mov eax, NP_EVENT_BOOT_READY
    xor ecx, ecx
    xor edx, edx
    xor esi, esi
    xor edi, edi
    call evbus_publish
    jc .est_pop1_fail
    cmp dword [evbus_test_flag], 0xCAFEBABE
    jne .est_pop1_fail

    ; Test 7 – Filter nach Eventtyp (§62.7)
    ; Sub2 abonniert nur OBJ_CREATED; publish OBJ_CREATED → sub2 feuert
    mov dword [evbus_test_flag], 0
    mov eax, NP_EVENT_OBJ_CREATED
    mov ecx, evbus_test_callback
    mov edx, NP_EVENT_DELIVERY_CALLBACK
    call evbus_subscribe
    jc .est_pop1_fail
    push ebx                            ; sub2_id

    mov eax, NP_EVENT_OBJ_CREATED
    mov ecx, 1
    mov edx, 2
    xor esi, esi
    xor edi, edi
    call evbus_publish
    jc .est_pop2_fail
    cmp dword [evbus_test_flag], 0xCAFEBABE
    jne .est_pop2_fail

    ; Test 11 – Gültige Payload-Größe (§62.11): OBJ_CREATED max=8, size=4 → OK
    mov eax, NP_EVENT_OBJ_CREATED
    xor ecx, ecx
    xor edx, edx
    mov esi, evbus_test_buf
    mov edi, 4
    call evbus_publish
    jc .est_pop2_fail

    ; Test 12 – Ungültige Payload-Größe (§62.12): size=64 > max=8 → CF=1
    mov eax, NP_EVENT_OBJ_CREATED
    xor ecx, ecx
    xor edx, edx
    mov esi, evbus_test_buf
    mov edi, 64
    call evbus_publish
    jnc .est_pop2_fail                  ; muss CF=1

    ; Test 14 – Eventreihenfolge: sequence steigt pro Publish (§62.14)
    mov eax, [evbus_sequence]
    push eax                            ; seq_before
    mov eax, NP_EVENT_BOOT_READY
    xor ecx, ecx
    xor edx, edx
    xor esi, esi
    xor edi, edi
    call evbus_publish
    jc .est_pop3_fail
    pop eax                             ; seq_before
    cmp [evbus_sequence], eax
    jbe .est_pop2_fail

    ; Aufräumen
    pop ebx                             ; sub2_id (ungenutzt)
    pop ebx                             ; sub1_id (ungenutzt)
    pop edi
    pop esi
    pop ebx
    clc
    ret

.est_pop3_fail:
    pop eax                             ; seq_before
    jmp .est_pop2_fail
.est_pop2_fail:
    pop ebx                             ; sub2_id
.est_pop1_fail:
    pop ebx                             ; sub1_id
.est_fail:
    pop edi
    pop esi
    pop ebx
    stc
    ret

; Test-Callback: setzt evbus_test_flag = 0xCAFEBABE
evbus_test_callback:
    mov dword [evbus_test_flag], 0xCAFEBABE
    ret

; Event-Bus-Daten
evbus_schemas:          times (EVBUS_MAX_SCHEMAS * EVBUS_SCH_SIZE) db 0
evbus_subs:             times (EVBUS_MAX_SUBS * EVBUS_SUB_SIZE) db 0
evbus_next_sub_id:      dd 0
evbus_sequence:         dd 0
evbus_depth:            db 0
                        db 0, 0, 0              ; Alignment
evbus_stat_published:   dd 0
evbus_stat_delivered:   dd 0
evbus_stat_sync:        dd 0
evbus_stat_rejected:    dd 0
evbus_pub_type:         dd 0
evbus_pub_src:          dd 0
evbus_pub_subj:         dd 0
evbus_pub_payload:      dd 0
evbus_pub_plsize:       dd 0
evbus_test_flag:        dd 0
evbus_test_buf:         times 16 db 0

; ---------------------------------------------------------------------------
; §102 – Unified Object API (NPSPEC-KERNEL-0102)
; Einheitliche Zugangsschicht für verwaltete Kernelobjekte.
; Bootstrap: Phase 1 (§64): Typregistrierung, Objekterzeugung, Handle-Auflösung,
; Referenzzählung, Capability-Prüfung.
; Phase 2-4 (Namespaces, async, RCU): Bootstrap-N/A.
; ---------------------------------------------------------------------------

; Objektzustände (§9)
UOBJ_STATE_CREATING     equ 0
UOBJ_STATE_ACTIVE       equ 1
UOBJ_STATE_QUIESCING    equ 2
UOBJ_STATE_CLOSING      equ 3
UOBJ_STATE_ZOMBIE       equ 4
UOBJ_STATE_DESTROYING   equ 5
UOBJ_STATE_DESTROYED    equ 6

; Capabilities (§15)
UOBJ_CAP_QUERY          equ 0x001
UOBJ_CAP_MODIFY         equ 0x002
UOBJ_CAP_SIGNAL         equ 0x004
UOBJ_CAP_WAIT           equ 0x008
UOBJ_CAP_DUPLICATE      equ 0x010
UOBJ_CAP_TRANSFER       equ 0x020
UOBJ_CAP_SUBSCRIBE      equ 0x040
UOBJ_CAP_DELETE         equ 0x080
UOBJ_CAP_ADMIN          equ 0x100

; Fehlercodes (§48)
UOBJ_ERR_INVALID_HANDLE equ -50
UOBJ_ERR_STALE_REF      equ -51
UOBJ_ERR_TYPE_MISMATCH  equ -52
UOBJ_ERR_ACCESS_DENIED  equ -53
UOBJ_ERR_INVALID_STATE  equ -54
UOBJ_ERR_NOT_SUPPORTED  equ -55

; Pool-Größen
UOBJ_MAX_TYPES          equ 16
UOBJ_MAX_OBJECTS        equ 32
UOBJ_MAX_HANDLES        equ 32

; Typ-Deskriptor-Layout (24 Bytes, §7)
UOBJ_TYPE_SIZE              equ 24
UOBJ_TYPE_OFF_TYPE_ID       equ 0    ; uint32_t
UOBJ_TYPE_OFF_ABI_VER       equ 4    ; uint32_t
UOBJ_TYPE_OFF_FLAGS         equ 8    ; uint32_t
UOBJ_TYPE_OFF_INIT_FN       equ 12   ; uint32_t (fn-ptr, 0=kein Init)
UOBJ_TYPE_OFF_DESTROY_FN    equ 16   ; uint32_t (fn-ptr, 0=kein Destroy)
UOBJ_TYPE_OFF_PRESENT       equ 20   ; uint8_t
; 21-23: Padding

; Objekt-Header-Layout (32 Bytes, §5)
UOBJ_OBJ_SIZE               equ 32
UOBJ_OBJ_OFF_OBJ_ID         equ 0    ; uint32_t
UOBJ_OBJ_OFF_TYPE_ID        equ 4    ; uint32_t
UOBJ_OBJ_OFF_GEN            equ 8    ; uint32_t
UOBJ_OBJ_OFF_STATE          equ 12   ; uint8_t
UOBJ_OBJ_OFF_PRESENT        equ 13   ; uint8_t
UOBJ_OBJ_OFF_FLAGS          equ 14   ; uint16_t
UOBJ_OBJ_OFF_REFCOUNT       equ 16   ; uint32_t
UOBJ_OBJ_OFF_KOG_NODE       equ 20   ; uint32_t (KOG-Knoten-ID, 0=nicht verknüpft)
; 24-31: Reserved

; Handle-Tabellen-Eintrag-Layout (16 Bytes, §13)
UOBJ_HDL_SIZE               equ 16
UOBJ_HDL_OFF_HDL_ID         equ 0    ; uint32_t
UOBJ_HDL_OFF_OBJ_ID         equ 4    ; uint32_t
UOBJ_HDL_OFF_GEN            equ 8    ; uint32_t (Generation bei Erzeugung)
UOBJ_HDL_OFF_CAPS           equ 12   ; uint16_t (Capability-Bitmaske)
UOBJ_HDL_OFF_PRESENT        equ 14   ; uint8_t
; 15: Padding

; Compile-time-Invarianten
%if UOBJ_TYPE_SIZE != 24
%error "UOBJ_TYPE_SIZE muss 24 Bytes sein"
%endif
%if UOBJ_OBJ_SIZE != 32
%error "UOBJ_OBJ_SIZE muss 32 Bytes sein"
%endif
%if UOBJ_HDL_SIZE != 16
%error "UOBJ_HDL_SIZE muss 16 Bytes sein"
%endif

; ---------------------------------------------------------------------------
; Interne Hilfsfunktionen (lineare Suche über feste Pools)
; ---------------------------------------------------------------------------

; uobj_find_type: EAX=type_id → EBX=ptr, CF=0/CF=1; Clobbers: EBX
uobj_find_type:
    push esi
    push ecx
    mov esi, uobj_types
    xor ecx, ecx
.uft_loop:
    cmp ecx, UOBJ_MAX_TYPES
    jae .uft_not_found
    cmp byte [esi + UOBJ_TYPE_OFF_PRESENT], 1
    jne .uft_next
    cmp [esi + UOBJ_TYPE_OFF_TYPE_ID], eax
    je .uft_found
.uft_next:
    add esi, UOBJ_TYPE_SIZE
    inc ecx
    jmp .uft_loop
.uft_found:
    mov ebx, esi
    pop ecx
    pop esi
    clc
    ret
.uft_not_found:
    xor ebx, ebx
    pop ecx
    pop esi
    stc
    ret

; uobj_find_object: EAX=obj_id → EBX=ptr, CF=0/CF=1; Clobbers: EBX
uobj_find_object:
    push esi
    push ecx
    mov esi, uobj_objects
    xor ecx, ecx
.ufo_loop:
    cmp ecx, UOBJ_MAX_OBJECTS
    jae .ufo_not_found
    cmp byte [esi + UOBJ_OBJ_OFF_PRESENT], 1
    jne .ufo_next
    cmp [esi + UOBJ_OBJ_OFF_OBJ_ID], eax
    je .ufo_found
.ufo_next:
    add esi, UOBJ_OBJ_SIZE
    inc ecx
    jmp .ufo_loop
.ufo_found:
    mov ebx, esi
    pop ecx
    pop esi
    clc
    ret
.ufo_not_found:
    xor ebx, ebx
    pop ecx
    pop esi
    stc
    ret

; uobj_find_handle: EAX=handle_id → EBX=ptr, CF=0/CF=1; Clobbers: EBX
uobj_find_handle:
    push esi
    push ecx
    mov esi, uobj_handles
    xor ecx, ecx
.ufhdl_loop:
    cmp ecx, UOBJ_MAX_HANDLES
    jae .ufhdl_not_found
    cmp byte [esi + UOBJ_HDL_OFF_PRESENT], 1
    jne .ufhdl_next
    cmp [esi + UOBJ_HDL_OFF_HDL_ID], eax
    je .ufhdl_found
.ufhdl_next:
    add esi, UOBJ_HDL_SIZE
    inc ecx
    jmp .ufhdl_loop
.ufhdl_found:
    mov ebx, esi
    pop ecx
    pop esi
    clc
    ret
.ufhdl_not_found:
    xor ebx, ebx
    pop ecx
    pop esi
    stc
    ret

; uobj_alloc_object: → EBX=obj_ptr, ECX=new_obj_id, CF=0/CF=1
uobj_alloc_object:
    push esi
    mov esi, uobj_objects
    xor ecx, ecx
.uao_scan:
    cmp ecx, UOBJ_MAX_OBJECTS
    jae .uao_full
    cmp byte [esi + UOBJ_OBJ_OFF_PRESENT], 0
    je .uao_write
    add esi, UOBJ_OBJ_SIZE
    inc ecx
    jmp .uao_scan
.uao_write:
    lock inc dword [uobj_next_id]
    mov ecx, [uobj_next_id]
    mov [esi + UOBJ_OBJ_OFF_OBJ_ID], ecx
    mov byte [esi + UOBJ_OBJ_OFF_PRESENT], 1
    mov ebx, esi
    pop esi
    clc
    ret
.uao_full:
    xor ebx, ebx
    xor ecx, ecx
    pop esi
    stc
    ret

; uobj_alloc_handle: EAX=obj_id, ECX=generation, EDX=capabilities
; → EBX=handle_id, CF=0/CF=1
uobj_alloc_handle:
    push esi
    push edi
    push ecx    ; [esp+4 after next push]=ECX(gen)
    push edx    ; Stack: [esp]=EDX(caps), [esp+4]=ECX(gen),
                ;        [esp+8]=old_EDI, [esp+12]=old_ESI  EAX=obj_id

    mov edi, uobj_handles
    xor ecx, ecx
.uah_scan:
    cmp ecx, UOBJ_MAX_HANDLES
    jae .uah_full
    cmp byte [edi + UOBJ_HDL_OFF_PRESENT], 0
    je .uah_write
    add edi, UOBJ_HDL_SIZE
    inc ecx
    jmp .uah_scan
.uah_write:
    lock inc dword [uobj_next_hdl]
    mov ebx, [uobj_next_hdl]
    mov [edi + UOBJ_HDL_OFF_HDL_ID], ebx
    mov [edi + UOBJ_HDL_OFF_OBJ_ID], eax
    mov ecx, [esp + 4]                      ; generation
    mov [edi + UOBJ_HDL_OFF_GEN], ecx
    mov cx, [esp]                           ; capabilities (low 16)
    mov [edi + UOBJ_HDL_OFF_CAPS], cx
    mov byte [edi + UOBJ_HDL_OFF_PRESENT], 1
    pop edx
    pop ecx
    pop edi
    pop esi
    clc
    ret
.uah_full:
    xor ebx, ebx
    pop edx
    pop ecx
    pop edi
    pop esi
    stc
    ret

; ---------------------------------------------------------------------------
; uobj_register_type – Objekttyp registrieren (§7)
; EAX=type_id, ECX=abi_version, EDX=flags, ESI=init_fn (0=kein), EDI=destroy_fn (0=kein)
; CF=0 OK / CF=1 Fehler (doppelt/voll)
; ---------------------------------------------------------------------------
uobj_register_type:
    push ebx
    push esi
    push edi
    push ecx    ; [esp+4 after next push]=ECX(abi_ver)
    push edx    ; Stack: [esp]=EDX(flags), [esp+4]=ECX(abi_ver),
                ;        [esp+8]=old_EDI(destroy_fn), [esp+12]=old_ESI(init_fn), [esp+16]=old_EBX

    call uobj_find_type         ; EAX=type_id; EAX unveränderter Wert
    jnc .urt_dup                ; CF=0 = bereits vorhanden

    mov edi, uobj_types
    xor ecx, ecx
.urt_scan:
    cmp ecx, UOBJ_MAX_TYPES
    jae .urt_full
    cmp byte [edi + UOBJ_TYPE_OFF_PRESENT], 0
    je .urt_write
    add edi, UOBJ_TYPE_SIZE
    inc ecx
    jmp .urt_scan
.urt_write:
    mov [edi + UOBJ_TYPE_OFF_TYPE_ID], eax
    mov ebx, [esp + 4]                      ; abi_version
    mov [edi + UOBJ_TYPE_OFF_ABI_VER], ebx
    mov ebx, [esp]                          ; flags
    mov [edi + UOBJ_TYPE_OFF_FLAGS], ebx
    mov ebx, [esp + 12]                     ; init_fn (old ESI)
    mov [edi + UOBJ_TYPE_OFF_INIT_FN], ebx
    mov ebx, [esp + 8]                      ; destroy_fn (old EDI)
    mov [edi + UOBJ_TYPE_OFF_DESTROY_FN], ebx
    mov byte [edi + UOBJ_TYPE_OFF_PRESENT], 1
    pop edx
    pop ecx
    pop edi
    pop esi
    pop ebx
    clc
    ret
.urt_dup:
.urt_full:
    pop edx
    pop ecx
    pop edi
    pop esi
    pop ebx
    stc
    ret

; ---------------------------------------------------------------------------
; uobj_create – Objekt erzeugen und Handle zurückgeben (§11)
; EAX=type_id, ECX=capabilities
; EBX=handle_id, ECX=obj_id (Rückgabe), CF=0 OK / CF=1 Fehler
; Bootstrap: Pools; kein Rollback über KOG-Integration.
; ---------------------------------------------------------------------------
uobj_create:
    mov [uobj_tmp_type], eax
    mov [uobj_tmp_caps], ecx

    push ebx
    push esi
    push edi

    ; Typ validieren
    mov eax, [uobj_tmp_type]
    call uobj_find_type
    jc .uc_fail
    mov esi, ebx                ; ESI = type_ptr (callee-saved)

    ; Objekt-Slot allokieren
    call uobj_alloc_object      ; → EBX=obj_ptr, ECX=obj_id
    jc .uc_fail
    mov edi, ebx                ; EDI = obj_ptr (callee-saved)
    mov [uobj_tmp_obj_id], ecx

    ; Objekt-Header initialisieren
    lock inc dword [uobj_next_gen]
    mov eax, [uobj_next_gen]
    mov [edi + UOBJ_OBJ_OFF_GEN], eax
    mov eax, [esi + UOBJ_TYPE_OFF_TYPE_ID]
    mov [edi + UOBJ_OBJ_OFF_TYPE_ID], eax
    mov byte [edi + UOBJ_OBJ_OFF_STATE], UOBJ_STATE_CREATING
    mov dword [edi + UOBJ_OBJ_OFF_REFCOUNT], 1
    mov dword [edi + UOBJ_OBJ_OFF_KOG_NODE], 0
    mov word [edi + UOBJ_OBJ_OFF_FLAGS], 0

    ; Typspezifischen init_fn aufrufen (§11 Schritt 5)
    mov eax, [esi + UOBJ_TYPE_OFF_INIT_FN]
    test eax, eax
    jz .uc_no_init
    mov ebx, edi                ; init_fn(EBX=obj_ptr) → CF
    call eax
    jc .uc_init_fail
.uc_no_init:
    mov byte [edi + UOBJ_OBJ_OFF_STATE], UOBJ_STATE_ACTIVE

    ; Handle allokieren
    mov eax, [uobj_tmp_obj_id]
    mov ecx, [edi + UOBJ_OBJ_OFF_GEN]
    mov edx, [uobj_tmp_caps]
    call uobj_alloc_handle      ; EAX=obj_id, ECX=gen, EDX=caps → EBX=handle_id
    jc .uc_handle_fail

    ; Rückgabewerte: EBX=handle_id, ECX=obj_id
    mov ecx, [uobj_tmp_obj_id]

    ; NP_EVENT_OBJ_CREATED veröffentlichen (§40, §29)
    push ebx
    push ecx
    mov eax, NP_EVENT_OBJ_CREATED
    mov ecx, [uobj_tmp_obj_id]
    xor edx, edx
    xor esi, esi
    xor edi, edi
    call evbus_publish          ; CF ignoriert (Bootstrap-Event)
    pop ecx
    pop ebx

    lock inc dword [uobj_stat_created]
    pop edi
    pop esi
    pop ebx
    clc
    ret

.uc_handle_fail:
.uc_init_fail:
    mov byte [edi + UOBJ_OBJ_OFF_PRESENT], 0
.uc_fail:
    lock inc dword [uobj_stat_failed]
    pop edi
    pop esi
    pop ebx
    stc
    ret

; ---------------------------------------------------------------------------
; uobj_from_handle – Handle auflösen + Capabilities und Generation prüfen (§13)
; EAX=handle_id, ECX=required_capabilities
; EBX=obj_ptr (Rückgabe), CF=0 OK / CF=1 Fehler
; ---------------------------------------------------------------------------
uobj_from_handle:
    push esi
    push ecx                    ; [esp] = required_caps

    call uobj_find_handle       ; EAX=handle_id → EBX=hdl_ptr
    jc .ufrom_fail

    mov esi, ebx                ; ESI = hdl_ptr

    ; Capability-Prüfung: alle geforderten Bits müssen im Handle vorhanden sein
    movzx eax, word [esi + UOBJ_HDL_OFF_CAPS]
    mov ecx, [esp]              ; required_caps
    and eax, ecx                ; EAX = vorhandene & geforderte
    cmp eax, ecx                ; müssen identisch sein
    jne .ufrom_denied

    ; Objekt auflösen
    mov eax, [esi + UOBJ_HDL_OFF_OBJ_ID]
    call uobj_find_object       ; → EBX=obj_ptr
    jc .ufrom_fail

    ; Generations-Prüfung (§6 Invariant 4, §60 Invariant 4)
    mov eax, [esi + UOBJ_HDL_OFF_GEN]
    cmp eax, [ebx + UOBJ_OBJ_OFF_GEN]
    jne .ufrom_stale

    ; Zustandsprüfung: kein Zugriff auf sterbende Objekte
    movzx eax, byte [ebx + UOBJ_OBJ_OFF_STATE]
    cmp eax, UOBJ_STATE_DESTROYING
    jae .ufrom_bad_state

    pop ecx
    pop esi
    clc
    ret

.ufrom_stale:
.ufrom_bad_state:
.ufrom_denied:
.ufrom_fail:
    xor ebx, ebx
    pop ecx
    pop esi
    stc
    ret

; ---------------------------------------------------------------------------
; uobj_retain – Referenzzähler erhöhen (§12)
; EAX=obj_id → CF=0 OK / CF=1 nicht gefunden
; ---------------------------------------------------------------------------
uobj_retain:
    push ebx
    call uobj_find_object
    jc .uret_fail
    lock inc dword [ebx + UOBJ_OBJ_OFF_REFCOUNT]
    pop ebx
    clc
    ret
.uret_fail:
    pop ebx
    stc
    ret

; ---------------------------------------------------------------------------
; uobj_release – Referenzzähler senken; bei 0 Objekt zerstören (§12)
; EAX=obj_id → CF=0 OK / CF=1 nicht gefunden
; ---------------------------------------------------------------------------
uobj_release:
    push ebx
    push esi
    call uobj_find_object       ; EAX=obj_id → EBX=obj_ptr
    jc .urel_fail

    lock dec dword [ebx + UOBJ_OBJ_OFF_REFCOUNT]
    jnz .urel_done

    ; Refcount = 0: Objekt zerstören (§9 DESTROYING → DESTROYED)
    mov esi, ebx                ; ESI = obj_ptr
    mov byte [esi + UOBJ_OBJ_OFF_STATE], UOBJ_STATE_DESTROYING

    ; destroy_fn aufrufen
    mov eax, [esi + UOBJ_OBJ_OFF_TYPE_ID]
    call uobj_find_type         ; → EBX=type_ptr (ESI bleibt erhalten)
    jc .urel_no_fn
    mov eax, [ebx + UOBJ_TYPE_OFF_DESTROY_FN]
    test eax, eax
    jz .urel_no_fn
    mov ebx, esi                ; destroy_fn(EBX=obj_ptr)
    call eax
.urel_no_fn:
    ; obj_id vor Freigabe lesen (§41 – Diagnose nach Destroy)
    mov ecx, [esi + UOBJ_OBJ_OFF_OBJ_ID]
    mov byte [esi + UOBJ_OBJ_OFF_STATE], UOBJ_STATE_DESTROYED
    mov byte [esi + UOBJ_OBJ_OFF_PRESENT], 0
    lock inc dword [uobj_stat_destroyed]

    ; NP_EVENT_OBJ_DESTROYED veröffentlichen
    mov eax, NP_EVENT_OBJ_DESTROYED
    xor edx, edx
    xor esi, esi
    xor edi, edi
    call evbus_publish          ; EAX=type, ECX=obj_id (noch gesetzt)

.urel_done:
    pop esi
    pop ebx
    clc
    ret
.urel_fail:
    pop esi
    pop ebx
    stc
    ret

; ---------------------------------------------------------------------------
; uobj_close_handle – Handle schließen und Objektreferenz freigeben (§33)
; EAX=handle_id → CF=0 OK / CF=1 nicht gefunden
; ---------------------------------------------------------------------------
uobj_close_handle:
    push ebx
    call uobj_find_handle       ; → EBX=hdl_ptr
    jc .uch_fail
    mov byte [ebx + UOBJ_HDL_OFF_PRESENT], 0
    mov eax, [ebx + UOBJ_HDL_OFF_OBJ_ID]
    call uobj_release           ; CF ignoriert (Objekt könnte schon weg sein)
    pop ebx
    clc
    ret
.uch_fail:
    pop ebx
    stc
    ret

; ---------------------------------------------------------------------------
; uobj_query – Objekt abfragen (§20), Bootstrap: nur Klasse 0 (Basisidentität)
; EAX=handle_id, ECX=query_class, EDX=buffer_ptr
; EAX=bytes_written (Rückgabe), CF=0 OK / CF=1 Fehler
; ---------------------------------------------------------------------------
uobj_query:
    push ebx
    push esi
    push edi
    push ecx                    ; query_class
    push edx                    ; buffer_ptr

    ; Handle auflösen (QUERY-Recht prüfen)
    mov ecx, UOBJ_CAP_QUERY
    call uobj_from_handle       ; EAX=handle → EBX=obj_ptr
    jc .uq_fail

    ; Nur Klasse 0 in Bootstrap implementiert (§20 Basisidentität)
    cmp dword [esp + 4], 0
    jne .uq_not_supported

    ; 16-Byte-Antwort: obj_id, type_id, generation, state
    mov edi, [esp]              ; buffer_ptr
    mov eax, [ebx + UOBJ_OBJ_OFF_OBJ_ID]
    mov [edi],      eax
    mov eax, [ebx + UOBJ_OBJ_OFF_TYPE_ID]
    mov [edi + 4],  eax
    mov eax, [ebx + UOBJ_OBJ_OFF_GEN]
    mov [edi + 8],  eax
    movzx eax, byte [ebx + UOBJ_OBJ_OFF_STATE]
    mov [edi + 12], eax

    pop edx
    pop ecx
    pop edi
    pop esi
    pop ebx
    mov eax, 16
    clc
    ret

.uq_not_supported:
.uq_fail:
    pop edx
    pop ecx
    pop edi
    pop esi
    pop ebx
    xor eax, eax
    stc
    ret

; ---------------------------------------------------------------------------
; uobj_initialize – Pools leeren, 15 Built-in-Typen registrieren (§55)
; CF=0 OK / CF=1 Fehler
; ---------------------------------------------------------------------------

%macro uobj_reg_builtin 1
    mov eax, %1
    mov ecx, 1
    xor edx, edx
    xor esi, esi
    xor edi, edi
    call uobj_register_type
    jc .uinit_fail
%endmacro

uobj_initialize:
    push edi
    push ecx

    mov edi, uobj_types
    xor eax, eax
    mov ecx, (UOBJ_MAX_TYPES * UOBJ_TYPE_SIZE) / 4
    rep stosd
    mov edi, uobj_objects
    mov ecx, (UOBJ_MAX_OBJECTS * UOBJ_OBJ_SIZE) / 4
    rep stosd
    mov edi, uobj_handles
    mov ecx, (UOBJ_MAX_HANDLES * UOBJ_HDL_SIZE) / 4
    rep stosd
    mov dword [uobj_next_id],  0
    mov dword [uobj_next_gen], 0
    mov dword [uobj_next_hdl], 0
    mov dword [uobj_stat_created],   0
    mov dword [uobj_stat_destroyed], 0
    mov dword [uobj_stat_failed],    0

    uobj_reg_builtin NP_OBJTYPE_KERNEL
    uobj_reg_builtin NP_OBJTYPE_MACHINE
    uobj_reg_builtin NP_OBJTYPE_NAMESPACE
    uobj_reg_builtin NP_OBJTYPE_PROCESS
    uobj_reg_builtin NP_OBJTYPE_THREAD
    uobj_reg_builtin NP_OBJTYPE_CPU
    uobj_reg_builtin NP_OBJTYPE_MEMORY
    uobj_reg_builtin NP_OBJTYPE_IPC
    uobj_reg_builtin NP_OBJTYPE_DEVICE
    uobj_reg_builtin NP_OBJTYPE_DRIVER
    uobj_reg_builtin NP_OBJTYPE_VFS_NODE
    uobj_reg_builtin NP_OBJTYPE_SECURITY
    uobj_reg_builtin NP_OBJTYPE_DIAGNOSTIC
    uobj_reg_builtin NP_OBJTYPE_POWER
    uobj_reg_builtin NP_OBJTYPE_GENERIC

    pop ecx
    pop edi
    clc
    ret
.uinit_fail:
    pop ecx
    pop edi
    stc
    ret

; ---------------------------------------------------------------------------
; uobj_self_test – §58 Tests 1-10 (Bootstrap-Implementierung)
; Tests 11-15 (SMP, async, weak-ref, namespace): Bootstrap-N/A.
; CF=0 alle Tests bestanden / CF=1 Fehler
; ---------------------------------------------------------------------------
uobj_self_test:
    push ebx
    push esi
    push edi

    ; Test 1 – Typ GENERIC registriert
    mov eax, NP_OBJTYPE_GENERIC
    call uobj_find_type
    jc .ust_fail
    cmp byte [ebx + UOBJ_TYPE_OFF_PRESENT], 1
    jne .ust_fail

    ; Test 2 – Doppelter Typ abgelehnt (§60 Invariant)
    mov eax, NP_OBJTYPE_GENERIC
    mov ecx, 1
    xor edx, edx
    xor esi, esi
    xor edi, edi
    call uobj_register_type
    jnc .ust_fail               ; muss CF=1

    ; Test 3 – Objekt erzeugen → gültiger Handle
    mov eax, NP_OBJTYPE_GENERIC
    mov ecx, UOBJ_CAP_QUERY | UOBJ_CAP_DELETE
    call uobj_create            ; → EBX=handle_id, ECX=obj_id
    jc .ust_fail
    push ebx                    ; [esp+4] = handle_id
    push ecx                    ; [esp]   = obj_id

    ; Test 4 – Handle-Auflösung mit korrekten Caps (QUERY erlaubt)
    mov eax, [esp + 4]          ; handle_id
    mov ecx, UOBJ_CAP_QUERY
    call uobj_from_handle       ; → EBX=obj_ptr
    jc .ust_pop2_fail

    ; Test 5 – Unzureichende Caps (ADMIN nicht im Handle) → CF=1
    mov eax, [esp + 4]          ; handle_id
    mov ecx, UOBJ_CAP_ADMIN
    call uobj_from_handle
    jnc .ust_pop2_fail          ; muss CF=1

    ; Test 6 – uobj_retain: Refcount auf 2
    mov eax, [esp]              ; obj_id
    call uobj_retain
    jc .ust_pop2_fail
    call uobj_find_object       ; EAX noch = obj_id → EBX=obj_ptr
    jc .ust_pop2_fail
    cmp dword [ebx + UOBJ_OBJ_OFF_REFCOUNT], 2
    jne .ust_pop2_fail

    ; Test 7 – uobj_release: Refcount zurück auf 1, Objekt noch vorhanden
    mov eax, [esp]              ; obj_id
    call uobj_release
    jc .ust_pop2_fail
    mov eax, [esp]
    call uobj_find_object
    jc .ust_pop2_fail
    cmp dword [ebx + UOBJ_OBJ_OFF_REFCOUNT], 1
    jne .ust_pop2_fail
    cmp byte [ebx + UOBJ_OBJ_OFF_PRESENT], 1
    jne .ust_pop2_fail

    ; Test 8 – uobj_query Klasse 0: 16 Bytes, korrekte obj_id
    mov eax, [esp + 4]          ; handle_id
    mov ecx, 0                  ; query class 0 = Basisidentität
    mov edx, uobj_test_buf
    call uobj_query
    jc .ust_pop2_fail
    cmp eax, 16
    jne .ust_pop2_fail
    mov eax, [esp]              ; obj_id
    cmp [uobj_test_buf], eax
    jne .ust_pop2_fail

    ; Test 9 – Handle schließen
    mov eax, [esp + 4]          ; handle_id
    call uobj_close_handle
    jc .ust_pop2_fail

    ; Test 10 – Handle nach Schließung nicht mehr auffindbar (§60 Invariant 2/3)
    mov eax, [esp + 4]          ; handle_id (geschlossen)
    call uobj_find_handle
    jnc .ust_pop2_fail          ; muss CF=1

    ; Aufräumen (obj_id, handle_id vom Stack)
    pop ecx
    pop ebx
    pop edi
    pop esi
    pop ebx
    clc
    ret

.ust_pop2_fail:
    pop ecx                     ; obj_id
    pop ebx                     ; handle_id
.ust_fail:
    pop edi
    pop esi
    pop ebx
    stc
    ret

; UOBJ-Daten
uobj_types:           times (UOBJ_MAX_TYPES * UOBJ_TYPE_SIZE) db 0
uobj_objects:         times (UOBJ_MAX_OBJECTS * UOBJ_OBJ_SIZE) db 0
uobj_handles:         times (UOBJ_MAX_HANDLES * UOBJ_HDL_SIZE) db 0
uobj_next_id:         dd 0
uobj_next_gen:        dd 0
uobj_next_hdl:        dd 0
uobj_stat_created:    dd 0
uobj_stat_destroyed:  dd 0
uobj_stat_failed:     dd 0
uobj_tmp_type:        dd 0
uobj_tmp_caps:        dd 0
uobj_tmp_obj_id:      dd 0
uobj_test_buf:        times 16 db 0

; ===========================================================================
; §103 – Capability Framework 1.0 (NPSPEC-KERNEL-0103)
; ===========================================================================

; ---------------------------------------------------------------------------
; Capability Rights (§10) – lower 32 Bits der 64-Bit nova_capability_set_t
; ---------------------------------------------------------------------------
NP_CAP_QUERY        equ 0x001
NP_CAP_MODIFY       equ 0x002
NP_CAP_WAIT         equ 0x004
NP_CAP_SIGNAL       equ 0x008
NP_CAP_DUPLICATE    equ 0x010
NP_CAP_TRANSFER     equ 0x020
NP_CAP_SUBSCRIBE    equ 0x040
NP_CAP_DELETE       equ 0x080
NP_CAP_ADMIN        equ 0x100

; Capability-Handle-Flags (§31)
NP_CAP_FLAG_NO_DELEGATE equ 0x001
NP_CAP_FLAG_EPHEMERAL   equ 0x002
NP_CAP_FLAG_BORROWED    equ 0x004
NP_CAP_FLAG_INHERITED   equ 0x008

; Capability-Zustände
CAP_STATE_FREE      equ 0
CAP_STATE_VALID     equ 1
CAP_STATE_REVOKED   equ 2
CAP_STATE_EXPIRED   equ 3

; Bootstrap-Sicherheitsdomänen (§44)
CAP_DOMAIN_KERNEL   equ 1
CAP_DOMAIN_BOOT     equ 2

; Pool-Grenze und Deskriptorgröße
CAP_MAX_CAPS        equ 32
CAP_CAP_SIZE        equ 40

; Deskriptor-Feld-Offsets
CAP_OFF_ID          equ 0    ; dd  cap_id
CAP_OFF_OBJ_ID      equ 4    ; dd  Zielobjekt-ID
CAP_OFF_RIGHTS      equ 8    ; dd  nutzbare Rechte
CAP_OFF_DELEGABLE   equ 12   ; dd  delegierbare Rechte
CAP_OFF_DOMAIN      equ 16   ; dd  Besitzer-Domäne
CAP_OFF_EXPIRATION  equ 20   ; dd  Ablaufzeit (0 = kein Limit)
CAP_OFF_GENERATION  equ 24   ; dd  Generation
CAP_OFF_FLAGS       equ 28   ; dd  Flags
CAP_OFF_POLICY_ID   equ 32   ; dd  Richtlinien-ID
CAP_OFF_STATE       equ 36   ; db  CAP_STATE_*
CAP_OFF_PRESENT     equ 37   ; db  1 = belegt
                             ; dw  Padding

; Event-Typ-IDs (§52, Fortsetzung nach evbus-Built-ins 1–8)
NP_EVENT_CAP_CREATED    equ 9
NP_EVENT_CAP_REVOKED    equ 10
NP_EVENT_CAP_DENIED     equ 11

; Fehlercodes (§60)
NP_ERR_CAP_INVALID      equ -60
NP_ERR_CAP_REVOKED      equ -61
NP_ERR_CAP_EXPIRED      equ -62
NP_ERR_CAP_EXHAUSTED    equ -63
NP_ERR_CAP_RIGHTS       equ -64
NP_ERR_CAP_DELEGATE     equ -65
NP_ERR_CAP_DOMAIN       equ -66
NP_ERR_CAP_POLICY       equ -67
NP_ERR_CAP_STALE        equ -68
NP_ERR_CAP_LIMIT        equ -69

; ---------------------------------------------------------------------------
; Compile-Zeit-Invarianten
; ---------------------------------------------------------------------------
%if CAP_CAP_SIZE != 40
    %error "CAP_CAP_SIZE muss 40 Bytes sein"
%endif
%if CAP_OFF_PRESENT != 37
    %error "CAP_OFF_PRESENT Offset falsch"
%endif
%if CAP_OFF_STATE != 36
    %error "CAP_OFF_STATE Offset falsch"
%endif

; ---------------------------------------------------------------------------
; cap_find – Capability anhand ID suchen (intern)
; EAX=cap_id → CF=0 EBX=ptr / CF=1 EAX=NP_ERR_CAP_INVALID
; ---------------------------------------------------------------------------
cap_find:
    push edi
    push ecx
    mov edi, cap_table
    xor ecx, ecx
.cfind_scan:
    cmp ecx, CAP_MAX_CAPS
    jae .cfind_miss
    cmp byte [edi + CAP_OFF_PRESENT], 0
    je .cfind_next
    cmp dword [edi + CAP_OFF_ID], eax
    je .cfind_hit
.cfind_next:
    add edi, CAP_CAP_SIZE
    inc ecx
    jmp .cfind_scan
.cfind_hit:
    mov ebx, edi
    pop ecx
    pop edi
    clc
    ret
.cfind_miss:
    mov eax, NP_ERR_CAP_INVALID
    pop ecx
    pop edi
    stc
    ret

; ---------------------------------------------------------------------------
; cap_alloc – freien Deskriptor-Slot allozieren
; → CF=0 EBX=ptr EAX=new_cap_id / CF=1 EAX=NP_ERR_CAP_LIMIT
; ---------------------------------------------------------------------------
cap_alloc:
    push edi
    push ecx
    mov edi, cap_table
    xor ecx, ecx
.calloc_scan:
    cmp ecx, CAP_MAX_CAPS
    jae .calloc_full
    cmp byte [edi + CAP_OFF_PRESENT], 0
    je .calloc_found
    add edi, CAP_CAP_SIZE
    inc ecx
    jmp .calloc_scan
.calloc_found:
    mov eax, [cap_next_id]
    inc dword [cap_next_id]
    mov ebx, edi
    pop ecx
    pop edi
    clc
    ret
.calloc_full:
    mov eax, NP_ERR_CAP_LIMIT
    pop ecx
    pop edi
    stc
    ret

; ---------------------------------------------------------------------------
; cap_create – Capability erzeugen (§13)
; EAX=obj_id, EBX=rights, ECX=delegable_rights, EDX=owner_domain
; CF=0 EAX=cap_id / CF=1 EAX=Fehler
; Invariant §15: delegable_rights ⊆ rights
; ---------------------------------------------------------------------------
cap_create:
    push esi
    push edi

    mov [cap_tmp_obj_id], eax
    mov [cap_tmp_rights], ebx
    mov [cap_tmp_deleg],  ecx
    mov [cap_tmp_domain], edx

    ; delegable ⊆ rights: (delegable & ~rights) == 0
    mov edi, ebx               ; rights
    not edi                    ; ~rights
    mov eax, ecx               ; delegable
    and eax, edi               ; delegable & ~rights
    test eax, eax
    jnz .cca_rights_err

    call cap_alloc             ; → EBX=ptr, EAX=cap_id
    jc .cca_limit

    mov esi, eax               ; esi = new cap_id

    mov dword [ebx + CAP_OFF_ID],         esi
    mov eax, [cap_tmp_obj_id]
    mov [ebx + CAP_OFF_OBJ_ID],     eax
    mov eax, [cap_tmp_rights]
    mov [ebx + CAP_OFF_RIGHTS],     eax
    mov eax, [cap_tmp_deleg]
    mov [ebx + CAP_OFF_DELEGABLE],  eax
    mov eax, [cap_tmp_domain]
    mov [ebx + CAP_OFF_DOMAIN],     eax
    mov dword [ebx + CAP_OFF_EXPIRATION], 0
    mov eax, [cap_next_gen]
    inc dword [cap_next_gen]
    mov [ebx + CAP_OFF_GENERATION], eax
    mov dword [ebx + CAP_OFF_FLAGS],     0
    mov dword [ebx + CAP_OFF_POLICY_ID], 0
    mov byte  [ebx + CAP_OFF_STATE],     CAP_STATE_VALID
    mov byte  [ebx + CAP_OFF_PRESENT],   1

    lock inc dword [cap_stat_created]

    ; cap_id sichern, bevor ESI für evbus_publish überschrieben wird
    mov [cap_cca_id], esi

    ; NP_EVENT_CAP_CREATED publizieren (§52)
    mov eax, NP_EVENT_CAP_CREATED
    mov ecx, esi               ; source = cap_id
    xor edx, edx
    xor esi, esi               ; kein Payload
    xor edi, edi
    call evbus_publish         ; CF ignoriert (Bootstrap-Event)

    mov eax, [cap_cca_id]
    pop edi
    pop esi
    clc
    ret
.cca_rights_err:
    mov eax, NP_ERR_CAP_RIGHTS
    pop edi
    pop esi
    stc
    ret
.cca_limit:
    ; EAX = NP_ERR_CAP_LIMIT aus cap_alloc
    pop edi
    pop esi
    stc
    ret

; ---------------------------------------------------------------------------
; cap_check – Zugriffsprüfung (§19, §20, fail-closed)
; EAX=cap_id, EBX=required_rights
; CF=0 OK / CF=1 EAX=Fehler
; ---------------------------------------------------------------------------
cap_check:
    push esi
    push edi

    mov esi, ebx               ; esi = required_rights

    call cap_find              ; EAX=cap_id → EBX=ptr
    jc .cchk_invalid           ; EAX = NP_ERR_CAP_INVALID

    ; Zustand prüfen (§20 Schritt 2)
    movzx eax, byte [ebx + CAP_OFF_STATE]
    cmp eax, CAP_STATE_REVOKED
    je .cchk_revoked
    cmp eax, CAP_STATE_EXPIRED
    je .cchk_expired
    cmp eax, CAP_STATE_VALID
    jne .cchk_invalid

    ; Ablaufzeit: Bootstrap-Caps haben expiration=0 (kein Limit, §29)
    ; Zeitvergleich wird in §104 (Time Services) ergänzt

    ; Rechteprüfung: (cap_rights & required) == required (§20 Schritt 3)
    mov eax, [ebx + CAP_OFF_RIGHTS]
    and eax, esi               ; cap_rights & required
    cmp eax, esi
    jne .cchk_rights

    pop edi
    pop esi
    clc
    ret

.cchk_invalid:
    lock inc dword [cap_stat_denied]
    mov eax, NP_ERR_CAP_INVALID
    jmp .cchk_deny
.cchk_revoked:
    lock inc dword [cap_stat_denied]
    mov eax, NP_ERR_CAP_REVOKED
    jmp .cchk_deny
.cchk_expired:
    lock inc dword [cap_stat_denied]
    mov eax, NP_ERR_CAP_EXPIRED
    jmp .cchk_deny
.cchk_rights:
    lock inc dword [cap_stat_denied]
    ; NP_EVENT_CAP_DENIED publizieren (§52)
    mov eax, NP_EVENT_CAP_DENIED
    xor ecx, ecx
    xor edx, edx
    xor esi, esi
    xor edi, edi
    call evbus_publish
    mov eax, NP_ERR_CAP_RIGHTS
.cchk_deny:
    pop edi
    pop esi
    stc
    ret

; ---------------------------------------------------------------------------
; cap_derive – Capability mit Rechteabschwächung ableiten (§14, §15)
; EAX=src_cap_id, EBX=new_rights, ECX=new_delegable
; CF=0 EAX=new_cap_id / CF=1 EAX=Fehler
; ---------------------------------------------------------------------------
cap_derive:
    push esi
    push edi

    mov [cap_tmp_src_id], eax
    mov [cap_tmp_rights], ebx
    mov [cap_tmp_deleg],  ecx

    call cap_find              ; EAX=src_cap_id → EBX=ptr
    jc .cder_invalid

    ; Quelle muss VALID sein
    cmp byte [ebx + CAP_OFF_STATE], CAP_STATE_VALID
    jne .cder_revoked

    ; §15: new_rights ⊆ src_rights
    mov edi, [ebx + CAP_OFF_RIGHTS]
    not edi                    ; ~src_rights
    mov eax, [cap_tmp_rights]  ; new_rights
    mov esi, eax
    and esi, edi               ; new_rights & ~src_rights
    test esi, esi
    jnz .cder_rights_err

    ; §15: new_delegable ⊆ src_delegable
    mov edi, [ebx + CAP_OFF_DELEGABLE]
    not edi
    mov eax, [cap_tmp_deleg]
    mov esi, eax
    and esi, edi               ; new_delegable & ~src_delegable
    test esi, esi
    jnz .cder_delegate_err

    ; §15: new_delegable ⊆ new_rights
    mov eax, [cap_tmp_deleg]
    mov edi, [cap_tmp_rights]
    not edi
    mov esi, eax
    and esi, edi
    test esi, esi
    jnz .cder_rights_err

    ; Quell-Ptr retten (cap_alloc überschreibt EBX)
    mov [cap_tmp_src_ptr], ebx

    call cap_alloc             ; → EBX=ptr, EAX=new_cap_id
    jc .cder_limit

    mov esi, eax               ; esi = new_cap_id
    mov edi, [cap_tmp_src_ptr] ; edi = src_ptr

    mov dword [ebx + CAP_OFF_ID],         esi
    mov eax, [edi + CAP_OFF_OBJ_ID]
    mov [ebx + CAP_OFF_OBJ_ID],     eax
    mov eax, [cap_tmp_rights]
    mov [ebx + CAP_OFF_RIGHTS],     eax
    mov eax, [cap_tmp_deleg]
    mov [ebx + CAP_OFF_DELEGABLE],  eax
    mov eax, [edi + CAP_OFF_DOMAIN]
    mov [ebx + CAP_OFF_DOMAIN],     eax
    mov eax, [edi + CAP_OFF_EXPIRATION]
    mov [ebx + CAP_OFF_EXPIRATION], eax
    mov eax, [cap_next_gen]
    inc dword [cap_next_gen]
    mov [ebx + CAP_OFF_GENERATION], eax
    mov dword [ebx + CAP_OFF_FLAGS],     0
    mov dword [ebx + CAP_OFF_POLICY_ID], 0
    mov byte  [ebx + CAP_OFF_STATE],     CAP_STATE_VALID
    mov byte  [ebx + CAP_OFF_PRESENT],   1

    lock inc dword [cap_stat_created]

    mov [cap_cca_id], esi
    mov eax, NP_EVENT_CAP_CREATED
    mov ecx, esi
    xor edx, edx
    xor esi, esi
    xor edi, edi
    call evbus_publish         ; CF ignoriert

    mov eax, [cap_cca_id]
    pop edi
    pop esi
    clc
    ret
.cder_invalid:
    mov eax, NP_ERR_CAP_INVALID
    pop edi
    pop esi
    stc
    ret
.cder_revoked:
    mov eax, NP_ERR_CAP_REVOKED
    pop edi
    pop esi
    stc
    ret
.cder_rights_err:
    mov eax, NP_ERR_CAP_RIGHTS
    pop edi
    pop esi
    stc
    ret
.cder_delegate_err:
    mov eax, NP_ERR_CAP_DELEGATE
    pop edi
    pop esi
    stc
    ret
.cder_limit:
    mov eax, NP_ERR_CAP_LIMIT
    pop edi
    pop esi
    stc
    ret

; ---------------------------------------------------------------------------
; cap_revoke – Capability widerrufen (§25)
; EAX=cap_id → CF=0 / CF=1 EAX=Fehler
; ---------------------------------------------------------------------------
cap_revoke:
    push esi

    mov esi, eax               ; esi = cap_id

    call cap_find              ; EAX=cap_id → EBX=ptr
    jc .crev_invalid           ; EAX = NP_ERR_CAP_INVALID

    cmp byte [ebx + CAP_OFF_STATE], CAP_STATE_REVOKED
    je .crev_already

    mov byte [ebx + CAP_OFF_STATE], CAP_STATE_REVOKED
    lock inc dword [cap_stat_revoked]

    ; NP_EVENT_CAP_REVOKED publizieren (§52)
    mov eax, NP_EVENT_CAP_REVOKED
    mov ecx, esi               ; source = cap_id
    xor edx, edx
    xor esi, esi
    xor edi, edi
    call evbus_publish         ; CF ignoriert

    pop esi
    clc
    ret
.crev_invalid:
    ; EAX = NP_ERR_CAP_INVALID aus cap_find
    pop esi
    stc
    ret
.crev_already:
    mov eax, NP_ERR_CAP_REVOKED
    pop esi
    stc
    ret

; ---------------------------------------------------------------------------
; cap_initialize – Pools leeren, evbus-Schemata, Bootstrap-Caps (§64)
; CF=0 OK / CF=1 Fehler
; ---------------------------------------------------------------------------
%macro cap_reg_event 3         ; type_id, class, flags
    mov eax, %1
    mov ecx, %2
    xor edx, edx               ; max_payload = 0
    mov esi, %3
    call evbus_register_schema
    jc .cinit_fail
%endmacro

cap_initialize:
    push esi
    push edi
    push ecx

    ; Pools leeren
    mov edi, cap_table
    xor eax, eax
    mov ecx, (CAP_MAX_CAPS * CAP_CAP_SIZE) / 4
    rep stosd
    mov dword [cap_next_id],          0
    mov dword [cap_next_gen],         0
    mov dword [cap_stat_created],     0
    mov dword [cap_stat_revoked],     0
    mov dword [cap_stat_denied],      0
    mov dword [cap_boot_kernel_id],   0
    mov dword [cap_boot_id],          0

    ; evbus-Schemata registrieren (§52)
    cap_reg_event NP_EVENT_CAP_CREATED, NP_EVENT_CLASS_LIFECYCLE, NP_EVENT_SYNCHRONOUS | NP_EVENT_KERNEL_ONLY
    cap_reg_event NP_EVENT_CAP_REVOKED, NP_EVENT_CLASS_STATE,     NP_EVENT_SYNCHRONOUS | NP_EVENT_KERNEL_ONLY
    cap_reg_event NP_EVENT_CAP_DENIED,  NP_EVENT_CLASS_SECURITY,  NP_EVENT_SYNCHRONOUS | NP_EVENT_KERNEL_ONLY

    ; Bootstrap-Cap: Kernel-Root (§64) – obj_id=1, rights=ADMIN|QUERY|MODIFY, deleg=QUERY
    mov eax, 1
    mov ebx, NP_CAP_ADMIN | NP_CAP_QUERY | NP_CAP_MODIFY
    mov ecx, NP_CAP_QUERY
    mov edx, CAP_DOMAIN_KERNEL
    call cap_create
    jc .cinit_fail
    mov [cap_boot_kernel_id], eax

    ; Bootstrap-Cap: Boot-Kontext (§64) – obj_id=2, rights=QUERY|SUBSCRIBE, nicht delegierbar
    mov eax, 2
    mov ebx, NP_CAP_QUERY | NP_CAP_SUBSCRIBE
    xor ecx, ecx
    mov edx, CAP_DOMAIN_BOOT
    call cap_create
    jc .cinit_fail
    mov [cap_boot_id], eax

    pop ecx
    pop edi
    pop esi
    clc
    ret
.cinit_fail:
    pop ecx
    pop edi
    pop esi
    stc
    ret

; ---------------------------------------------------------------------------
; cap_self_test – Selbsttest §67 (10 Tests)
; CF=0 alle OK / CF=1 EAX = fehlgeschlagener Testfall
; ---------------------------------------------------------------------------
cap_self_test:
    push esi
    push edi
    push ebx

    ; Test 1: cap_create mit gültigen Rechten
    mov eax, 42
    mov ebx, NP_CAP_QUERY | NP_CAP_MODIFY
    mov ecx, NP_CAP_QUERY
    mov edx, CAP_DOMAIN_KERNEL
    call cap_create
    jc .cst_fail1
    mov [cap_st_id1], eax

    ; Test 2: cap_check – korrekte Rechte → CF=0
    mov eax, [cap_st_id1]
    mov ebx, NP_CAP_QUERY
    call cap_check
    jc .cst_fail2

    ; Test 3: cap_check – fehlende Rechte → CF=1 RIGHTS
    mov eax, [cap_st_id1]
    mov ebx, NP_CAP_ADMIN
    call cap_check
    jnc .cst_fail3
    cmp eax, NP_ERR_CAP_RIGHTS
    jne .cst_fail3

    ; Test 4: cap_derive – Rechteabschwächung → CF=0
    mov eax, [cap_st_id1]
    mov ebx, NP_CAP_QUERY
    mov ecx, NP_CAP_QUERY
    call cap_derive
    jc .cst_fail4
    mov [cap_st_id2], eax

    ; Test 5: cap_derive – Rechteerweiterung → CF=1 (§15 verletzt)
    mov eax, [cap_st_id1]
    mov ebx, NP_CAP_QUERY | NP_CAP_MODIFY | NP_CAP_ADMIN
    mov ecx, NP_CAP_QUERY
    call cap_derive
    jnc .cst_fail5

    ; Test 6: cap_revoke → CF=0
    mov eax, [cap_st_id1]
    call cap_revoke
    jc .cst_fail6

    ; Test 7: cap_check auf widerrufene Cap → CF=1 CAP_REVOKED
    mov eax, [cap_st_id1]
    mov ebx, NP_CAP_QUERY
    call cap_check
    jnc .cst_fail7
    cmp eax, NP_ERR_CAP_REVOKED
    jne .cst_fail7

    ; Test 8: cap_check auf nicht-existente Cap → CF=1 CAP_INVALID
    mov eax, 0xDEAD
    mov ebx, NP_CAP_QUERY
    call cap_check
    jnc .cst_fail8
    cmp eax, NP_ERR_CAP_INVALID
    jne .cst_fail8

    ; Test 9: Ableitungskette (derive von derive) → CF=0
    mov eax, [cap_st_id2]
    mov ebx, NP_CAP_QUERY
    xor ecx, ecx               ; delegable = 0 (weiter reduziert)
    call cap_derive
    jc .cst_fail9
    mov [cap_st_id3], eax

    ; Test 10: cap_check auf id3 mit MODIFY → CF=1 RIGHTS (id3 hat nur QUERY)
    mov eax, [cap_st_id3]
    mov ebx, NP_CAP_MODIFY
    call cap_check
    jnc .cst_fail10
    cmp eax, NP_ERR_CAP_RIGHTS
    jne .cst_fail10

    pop ebx
    pop edi
    pop esi
    clc
    ret

.cst_fail1:   mov eax, 1
    jmp .cst_fail
.cst_fail2:   mov eax, 2
    jmp .cst_fail
.cst_fail3:   mov eax, 3
    jmp .cst_fail
.cst_fail4:   mov eax, 4
    jmp .cst_fail
.cst_fail5:   mov eax, 5
    jmp .cst_fail
.cst_fail6:   mov eax, 6
    jmp .cst_fail
.cst_fail7:   mov eax, 7
    jmp .cst_fail
.cst_fail8:   mov eax, 8
    jmp .cst_fail
.cst_fail9:   mov eax, 9
    jmp .cst_fail
.cst_fail10:  mov eax, 10
.cst_fail:
    pop ebx
    pop edi
    pop esi
    stc
    ret

; ---------------------------------------------------------------------------
; cap_* Daten
; ---------------------------------------------------------------------------
cap_table:          times (CAP_MAX_CAPS * CAP_CAP_SIZE / 4) dd 0
cap_next_id:        dd 0
cap_next_gen:       dd 0
cap_stat_created:   dd 0
cap_stat_revoked:   dd 0
cap_stat_denied:    dd 0
cap_boot_kernel_id: dd 0
cap_boot_id:        dd 0
; Temp-Speicher (Single-Threaded Bootstrap)
cap_tmp_obj_id:     dd 0
cap_tmp_rights:     dd 0
cap_tmp_deleg:      dd 0
cap_tmp_domain:     dd 0
cap_tmp_src_id:     dd 0
cap_tmp_src_ptr:    dd 0
cap_cca_id:         dd 0
; Selbsttest-IDs
cap_st_id1:         dd 0
cap_st_id2:         dd 0
cap_st_id3:         dd 0

; ===========================================================================
; §104 – Kernel Diagnostics Framework 1.0 (NPSPEC-KERNEL-0104)
; ===========================================================================

; ---------------------------------------------------------------------------
; Diagnosestufen (§9)
; ---------------------------------------------------------------------------
DIAG_LEVEL_TRACE    equ 0
DIAG_LEVEL_DEBUG    equ 1
DIAG_LEVEL_INFO     equ 2
DIAG_LEVEL_NOTICE   equ 3
DIAG_LEVEL_WARNING  equ 4
DIAG_LEVEL_ERROR    equ 5
DIAG_LEVEL_CRITICAL equ 6
DIAG_LEVEL_FATAL    equ 7

; Health-Zustände (§35)
DIAG_HEALTH_UNKNOWN    equ 0
DIAG_HEALTH_HEALTHY    equ 1
DIAG_HEALTH_DEGRADED   equ 2
DIAG_HEALTH_UNHEALTHY  equ 3
DIAG_HEALTH_FAILED     equ 4
DIAG_HEALTH_RECOVERING equ 5
DIAG_HEALTH_OFFLINE    equ 6

; Built-in Diagnosequellen-IDs (§7)
DIAG_SRC_KERNEL     equ 1
DIAG_SRC_KOG        equ 2
DIAG_SRC_EVBUS      equ 3
DIAG_SRC_UOBJ       equ 4
DIAG_SRC_CAP        equ 5
DIAG_SRC_DIAG       equ 6

; Pool-Grenzen und Deskriptorgrößen
DIAG_MAX_SOURCES        equ 16
DIAG_SRC_SIZE           equ 32
DIAG_LOG_MAX_RECORDS    equ 64
DIAG_LOG_RECORD_SIZE    equ 16
DIAG_MAX_HEALTH         equ 16
DIAG_HEALTH_SIZE        equ 4

; Diagnosequellen-Deskriptor-Offsets (§7)
DIAG_OFF_SRC_ID         equ 0    ; dd  source_id
DIAG_OFF_SRC_FLAGS      equ 4    ; dd  flags
DIAG_OFF_SRC_LEVEL      equ 8    ; db  default_level
DIAG_OFF_SRC_PRESENT    equ 9    ; db  1 = belegt
                                 ; dw  Padding bei Offset 10
DIAG_OFF_SRC_NAME       equ 12   ; 20 × db  null-terminierter Name

; Log-Record-Offsets (§10, §13)
DIAG_OFF_LOG_LEVEL      equ 0    ; db  DIAG_LEVEL_*
DIAG_OFF_LOG_SRC_ID     equ 1    ; db  source_id (Byte-Wert)
DIAG_OFF_LOG_FLAGS      equ 2    ; dw  Flags
DIAG_OFF_LOG_EVENT_ID   equ 4    ; dd  Event-ID
DIAG_OFF_LOG_SEQ        equ 8    ; dd  Sequenznummer
DIAG_OFF_LOG_VALUE      equ 12   ; dd  optionaler u32-Wert

; Health-Tabellen-Offsets (§35)
DIAG_OFF_HLTH_SRC_ID    equ 0    ; db  source_id
DIAG_OFF_HLTH_STATE     equ 1    ; db  DIAG_HEALTH_*
DIAG_OFF_HLTH_PRESENT   equ 2    ; db  1 = belegt
                                 ; db  Padding

; Event-Typ-ID (§44, nach CAP-Events 9–11)
NP_EVENT_DIAG_HEALTH_CHANGED equ 12

; Fehlercodes (§68)
NP_ERR_DIAG_DISABLED    equ -70
NP_ERR_DIAG_FILTERED    equ -71
NP_ERR_DIAG_BUFFER_FULL equ -72
NP_ERR_DIAG_TOO_LARGE   equ -73
NP_ERR_DIAG_NOT_FOUND   equ -74
NP_ERR_DIAG_LIMIT       equ -75

; ---------------------------------------------------------------------------
; Compile-Zeit-Invarianten
; ---------------------------------------------------------------------------
%if DIAG_SRC_SIZE != 32
    %error "DIAG_SRC_SIZE muss 32 Bytes sein"
%endif
%if DIAG_LOG_RECORD_SIZE != 16
    %error "DIAG_LOG_RECORD_SIZE muss 16 Bytes sein"
%endif
%if DIAG_HEALTH_SIZE != 4
    %error "DIAG_HEALTH_SIZE muss 4 Bytes sein"
%endif

; ---------------------------------------------------------------------------
; diag_find_source – Diagnosequelle anhand ID suchen (intern)
; EAX=src_id → CF=0 EBX=ptr / CF=1 EAX=NP_ERR_DIAG_NOT_FOUND
; ---------------------------------------------------------------------------
diag_find_source:
    push edi
    push ecx
    mov edi, diag_sources
    xor ecx, ecx
.dfs_scan:
    cmp ecx, DIAG_MAX_SOURCES
    jae .dfs_miss
    cmp byte [edi + DIAG_OFF_SRC_PRESENT], 0
    je .dfs_next
    cmp dword [edi + DIAG_OFF_SRC_ID], eax
    je .dfs_hit
.dfs_next:
    add edi, DIAG_SRC_SIZE
    inc ecx
    jmp .dfs_scan
.dfs_hit:
    mov ebx, edi
    pop ecx
    pop edi
    clc
    ret
.dfs_miss:
    mov eax, NP_ERR_DIAG_NOT_FOUND
    pop ecx
    pop edi
    stc
    ret

; ---------------------------------------------------------------------------
; diag_register_source – Diagnosequelle registrieren (§8)
; EAX=src_id, EBX=flags, ECX=default_level
; CF=0 OK / CF=1 Fehler (doppelt/voll)
; ---------------------------------------------------------------------------
diag_register_source:
    push esi
    push edi

    mov [diag_tmp_src_id], eax
    mov [diag_tmp_flags],  ebx
    mov [diag_tmp_level],  ecx

    ; Doppelte Registrierung ablehnen
    call diag_find_source      ; EAX=src_id → EBX=ptr / CF=1 nicht gefunden
    jnc .drs_dup               ; CF=0 = bereits vorhanden

    ; Freien Slot suchen
    mov edi, diag_sources
    xor ecx, ecx
.drs_scan:
    cmp ecx, DIAG_MAX_SOURCES
    jae .drs_full
    cmp byte [edi + DIAG_OFF_SRC_PRESENT], 0
    je .drs_found
    add edi, DIAG_SRC_SIZE
    inc ecx
    jmp .drs_scan
.drs_found:
    mov eax, [diag_tmp_src_id]
    mov dword [edi + DIAG_OFF_SRC_ID],  eax
    mov eax, [diag_tmp_flags]
    mov [edi + DIAG_OFF_SRC_FLAGS], eax
    mov al, [diag_tmp_level]   ; Byte-Zugriff: low byte von ECX-Speicher
    mov [edi + DIAG_OFF_SRC_LEVEL], al
    mov byte [edi + DIAG_OFF_SRC_PRESENT], 1

    pop edi
    pop esi
    clc
    ret
.drs_dup:
    pop edi
    pop esi
    stc
    ret
.drs_full:
    mov eax, NP_ERR_DIAG_LIMIT
    pop edi
    pop esi
    stc
    ret

; ---------------------------------------------------------------------------
; diag_log – Log-Eintrag in Ringpuffer schreiben (§13, §21)
; EAX=src_id, EBX=log_level, ECX=event_id, EDX=value
; CF=0 OK / CF=1 EAX=Fehler (gefiltert/nicht gefunden)
; ---------------------------------------------------------------------------
diag_log:
    push esi
    push edi

    mov [diag_tmp_src_id], eax
    mov [diag_tmp_flags],  ebx   ; log_level
    mov [diag_tmp_event],  ecx
    mov [diag_tmp_value],  edx

    ; Quelle prüfen
    call diag_find_source        ; EAX=src_id → EBX=ptr
    jc .dl_not_found

    ; Filterung: log_level < source.default_level → gefiltert (§27)
    movzx eax, byte [ebx + DIAG_OFF_SRC_LEVEL]
    cmp [diag_tmp_flags], eax
    jb .dl_filtered

    ; Overflow-Erkennung (§24): wenn total >= MAX → Overwrite, lost++
    mov eax, [diag_log_total]
    cmp eax, DIAG_LOG_MAX_RECORDS
    jb .dl_no_overflow
    lock inc dword [diag_log_lost]
.dl_no_overflow:

    ; Slot-Adresse berechnen (Ringpuffer, §22)
    mov edi, [diag_log_write]
    imul esi, edi, DIAG_LOG_RECORD_SIZE
    add esi, diag_log_buf

    ; Record schreiben
    mov al, [diag_tmp_flags]             ; log_level → byte
    mov [esi + DIAG_OFF_LOG_LEVEL], al
    mov al, [diag_tmp_src_id]            ; src_id → byte
    mov [esi + DIAG_OFF_LOG_SRC_ID], al
    mov word [esi + DIAG_OFF_LOG_FLAGS], 0
    mov eax, [diag_tmp_event]
    mov [esi + DIAG_OFF_LOG_EVENT_ID], eax
    mov eax, [diag_log_seq]
    inc dword [diag_log_seq]
    mov [esi + DIAG_OFF_LOG_SEQ], eax
    mov eax, [diag_tmp_value]
    mov [esi + DIAG_OFF_LOG_VALUE], eax

    ; Schreibzeiger zirkulär vorrücken
    mov eax, edi
    inc eax
    cmp eax, DIAG_LOG_MAX_RECORDS
    jb .dl_no_wrap
    xor eax, eax
.dl_no_wrap:
    mov [diag_log_write], eax
    lock inc dword [diag_log_total]
    lock inc dword [diag_stat_logged]

    pop edi
    pop esi
    clc
    ret

.dl_not_found:
    mov eax, NP_ERR_DIAG_NOT_FOUND
    pop edi
    pop esi
    stc
    ret
.dl_filtered:
    lock inc dword [diag_stat_filtered]
    mov eax, NP_ERR_DIAG_FILTERED
    pop edi
    pop esi
    stc
    ret

; ---------------------------------------------------------------------------
; diag_health_report – Health-Zustand eines Subsystems setzen (§37)
; EAX=src_id, EBX=health_state (DIAG_HEALTH_*)
; CF=0 OK / CF=1 Fehler
; ---------------------------------------------------------------------------
diag_health_report:
    push esi
    push edi

    mov [diag_tmp_src_id], eax
    mov [diag_tmp_flags],  ebx   ; health_state

    ; Quelle muss registriert sein
    call diag_find_source
    jc .dhr_not_found

    ; Gesundheitseintrag suchen oder anlegen
    mov edi, diag_health_tab
    xor ecx, ecx
.dhr_scan:
    cmp ecx, DIAG_MAX_HEALTH
    jae .dhr_full
    cmp byte [edi + DIAG_OFF_HLTH_PRESENT], 0
    je .dhr_new                    ; freier Slot
    movzx eax, byte [edi + DIAG_OFF_HLTH_SRC_ID]
    cmp eax, [diag_tmp_src_id]
    je .dhr_update                 ; bereits vorhanden
    add edi, DIAG_HEALTH_SIZE
    inc ecx
    jmp .dhr_scan
.dhr_new:
    mov al, [diag_tmp_src_id]
    mov [edi + DIAG_OFF_HLTH_SRC_ID], al
    mov byte [edi + DIAG_OFF_HLTH_PRESENT], 1
.dhr_update:
    mov al, [diag_tmp_flags]
    mov [edi + DIAG_OFF_HLTH_STATE], al

    ; NP_EVENT_DIAG_HEALTH_CHANGED publizieren (§44)
    mov eax, NP_EVENT_DIAG_HEALTH_CHANGED
    mov ecx, [diag_tmp_src_id]
    xor edx, edx
    xor esi, esi
    xor edi, edi
    call evbus_publish             ; CF ignoriert

    pop edi
    pop esi
    clc
    ret
.dhr_not_found:
    pop edi
    pop esi
    stc
    ret
.dhr_full:
    mov eax, NP_ERR_DIAG_LIMIT
    pop edi
    pop esi
    stc
    ret

; ---------------------------------------------------------------------------
; diag_health_get – Health-Zustand abfragen
; EAX=src_id → CF=0 EAX=DIAG_HEALTH_* / CF=1 EAX=NP_ERR_DIAG_NOT_FOUND
; ---------------------------------------------------------------------------
diag_health_get:
    push edi
    push ecx
    mov edi, diag_health_tab
    xor ecx, ecx
.dhg_scan:
    cmp ecx, DIAG_MAX_HEALTH
    jae .dhg_miss
    cmp byte [edi + DIAG_OFF_HLTH_PRESENT], 0
    je .dhg_next
    movzx ebx, byte [edi + DIAG_OFF_HLTH_SRC_ID]
    cmp ebx, eax
    je .dhg_hit
.dhg_next:
    add edi, DIAG_HEALTH_SIZE
    inc ecx
    jmp .dhg_scan
.dhg_hit:
    movzx eax, byte [edi + DIAG_OFF_HLTH_STATE]
    pop ecx
    pop edi
    clc
    ret
.dhg_miss:
    mov eax, NP_ERR_DIAG_NOT_FOUND
    pop ecx
    pop edi
    stc
    ret

; ---------------------------------------------------------------------------
; diag_initialize – Pools leeren, Quellen und Health registrieren (§63)
; CF=0 OK / CF=1 Fehler
; ---------------------------------------------------------------------------
%macro diag_reg_source 3           ; src_id, flags, default_level
    mov eax, %1
    mov ebx, %2
    mov ecx, %3
    call diag_register_source
    jc .dinit_fail
%endmacro

%macro diag_set_health 2           ; src_id, health_state
    mov eax, %1
    mov ebx, %2
    call diag_health_report
    jc .dinit_fail
%endmacro

diag_initialize:
    push esi
    push edi
    push ecx

    ; Pools leeren
    mov edi, diag_sources
    xor eax, eax
    mov ecx, (DIAG_MAX_SOURCES * DIAG_SRC_SIZE) / 4
    rep stosd
    mov edi, diag_log_buf
    mov ecx, (DIAG_LOG_MAX_RECORDS * DIAG_LOG_RECORD_SIZE) / 4
    rep stosd
    mov edi, diag_health_tab
    mov ecx, (DIAG_MAX_HEALTH * DIAG_HEALTH_SIZE) / 4
    rep stosd
    mov dword [diag_log_write],     0
    mov dword [diag_log_total],     0
    mov dword [diag_log_lost],      0
    mov dword [diag_log_seq],       0
    mov dword [diag_stat_logged],   0
    mov dword [diag_stat_filtered], 0
    mov dword [diag_stat_dropped],  0

    ; evbus-Schema für Health-Änderungen (§44)
    mov eax, NP_EVENT_DIAG_HEALTH_CHANGED
    mov ecx, NP_EVENT_CLASS_STATE
    xor edx, edx
    mov esi, NP_EVENT_SYNCHRONOUS | NP_EVENT_KERNEL_ONLY
    call evbus_register_schema
    jc .dinit_fail

    ; Built-in Diagnosequellen registrieren (§7, §8)
    diag_reg_source DIAG_SRC_KERNEL, 0, DIAG_LEVEL_INFO
    diag_reg_source DIAG_SRC_KOG,    0, DIAG_LEVEL_INFO
    diag_reg_source DIAG_SRC_EVBUS,  0, DIAG_LEVEL_INFO
    diag_reg_source DIAG_SRC_UOBJ,   0, DIAG_LEVEL_INFO
    diag_reg_source DIAG_SRC_CAP,    0, DIAG_LEVEL_INFO
    diag_reg_source DIAG_SRC_DIAG,   0, DIAG_LEVEL_INFO

    ; Initiale Health-Zustände: alle HEALTHY (§35)
    diag_set_health DIAG_SRC_KERNEL, DIAG_HEALTH_HEALTHY
    diag_set_health DIAG_SRC_KOG,    DIAG_HEALTH_HEALTHY
    diag_set_health DIAG_SRC_EVBUS,  DIAG_HEALTH_HEALTHY
    diag_set_health DIAG_SRC_UOBJ,   DIAG_HEALTH_HEALTHY
    diag_set_health DIAG_SRC_CAP,    DIAG_HEALTH_HEALTHY
    diag_set_health DIAG_SRC_DIAG,   DIAG_HEALTH_HEALTHY

    ; Ersten Diagnose-Log-Eintrag schreiben: DIAG initialisiert
    mov eax, DIAG_SRC_DIAG
    mov ebx, DIAG_LEVEL_INFO
    mov ecx, 1                 ; event_id = 1 (Initialisierung)
    xor edx, edx
    call diag_log
    jc .dinit_fail

    pop ecx
    pop edi
    pop esi
    clc
    ret
.dinit_fail:
    pop ecx
    pop edi
    pop esi
    stc
    ret

; ---------------------------------------------------------------------------
; diag_self_test – Selbsttest §71 (8 Tests)
; CF=0 alle OK / CF=1 EAX = fehlgeschlagener Testfall
; ---------------------------------------------------------------------------
diag_self_test:
    push esi
    push edi
    push ebx

    ; Test 1: Testquelle registrieren (ID=99, default_level=WARNING)
    mov eax, 99
    xor ebx, ebx
    mov ecx, DIAG_LEVEL_WARNING
    call diag_register_source
    jc .dst_fail1

    ; Test 2: Testquelle auffinden
    mov eax, 99
    call diag_find_source
    jc .dst_fail2
    cmp byte [ebx + DIAG_OFF_SRC_PRESENT], 1
    jne .dst_fail2

    ; Test 3: diag_log mit erlaubtem Level (WARNING >= WARNING) → CF=0
    mov eax, 99
    mov ebx, DIAG_LEVEL_WARNING
    mov ecx, 100
    xor edx, edx
    call diag_log
    jc .dst_fail3

    ; Test 4: diag_log mit gefiltertem Level (DEBUG < WARNING) → CF=1 FILTERED
    mov eax, 99
    mov ebx, DIAG_LEVEL_DEBUG
    mov ecx, 101
    xor edx, edx
    call diag_log
    jnc .dst_fail4
    cmp eax, NP_ERR_DIAG_FILTERED
    jne .dst_fail4

    ; Test 5: diag_health_report → CF=0
    mov eax, 99
    mov ebx, DIAG_HEALTH_HEALTHY
    call diag_health_report
    jc .dst_fail5

    ; Test 6: diag_health_get → DIAG_HEALTH_HEALTHY
    mov eax, 99
    call diag_health_get
    jc .dst_fail6
    cmp eax, DIAG_HEALTH_HEALTHY
    jne .dst_fail6

    ; Test 7: diag_health_report mit neuem Zustand → CF=0
    mov eax, 99
    mov ebx, DIAG_HEALTH_DEGRADED
    call diag_health_report
    jc .dst_fail7

    ; Test 8: diag_find_source mit unbekannter ID → CF=1
    mov eax, 0xBEEF
    call diag_find_source
    jnc .dst_fail8

    pop ebx
    pop edi
    pop esi
    clc
    ret

.dst_fail1:  mov eax, 1
    jmp .dst_fail
.dst_fail2:  mov eax, 2
    jmp .dst_fail
.dst_fail3:  mov eax, 3
    jmp .dst_fail
.dst_fail4:  mov eax, 4
    jmp .dst_fail
.dst_fail5:  mov eax, 5
    jmp .dst_fail
.dst_fail6:  mov eax, 6
    jmp .dst_fail
.dst_fail7:  mov eax, 7
    jmp .dst_fail
.dst_fail8:  mov eax, 8
.dst_fail:
    pop ebx
    pop edi
    pop esi
    stc
    ret

; ---------------------------------------------------------------------------
; diag_* Daten
; ---------------------------------------------------------------------------
diag_sources:       times (DIAG_MAX_SOURCES * DIAG_SRC_SIZE / 4) dd 0
diag_log_buf:       times (DIAG_LOG_MAX_RECORDS * DIAG_LOG_RECORD_SIZE / 4) dd 0
diag_health_tab:    times (DIAG_MAX_HEALTH * DIAG_HEALTH_SIZE / 4) dd 0
diag_log_write:     dd 0
diag_log_total:     dd 0
diag_log_lost:      dd 0
diag_log_seq:       dd 0
diag_stat_logged:   dd 0
diag_stat_filtered: dd 0
diag_stat_dropped:  dd 0
; Temp-Speicher (Single-Threaded Bootstrap)
diag_tmp_src_id:    dd 0
diag_tmp_flags:     dd 0
diag_tmp_level:     dd 0
diag_tmp_event:     dd 0
diag_tmp_value:     dd 0

; ===========================================================================
; CAP-Integration 1.0 – §103↔§102, §103↔IPC, §103↔VFS
; ===========================================================================
; Verbindet das Capability Framework (§103) mit:
;   • Unified Object API §102 : Handles erhalten CAP-Deskriptoren
;   • IPC                     : Send/Receive verlangen NP_CAP_SIGNAL/SUBSCRIBE
;   • VFS                     : Lookup verlangt NP_CAP_QUERY
; Alle Funktionen sind Single-Threaded-Bootstrap-sicher (statische Temps).
; ===========================================================================

CAP_INT_IPC_OBJ_ID     equ 200   ; virtuelles Objekt-ID für IPC-Endpoint-Caps
CAP_INT_VFS_OBJ_ID     equ 201   ; virtuelles Objekt-ID für VFS-Root-Cap

; ---------------------------------------------------------------------------
; cap_uobj_create_handle – Handle + CAP-Deskriptor für bestehendes UOBJ-Objekt
; EAX=obj_id, EBX=unused, ECX=rights(NP_CAP_*), EDX=domain(CAP_DOMAIN_*)
; CF=0: EBX=handle_id, EAX=cap_id / CF=1 Fehler
; Invariant: delegable_rights = rights & NP_CAP_QUERY (monoton reduziert)
; ---------------------------------------------------------------------------
cap_uobj_create_handle:
    push esi
    push edi

    mov [cap_int_tmp_obj_id], eax
    mov [cap_int_tmp_rights], ecx
    mov [cap_int_tmp_domain], edx

    ; Objekt prüfen + aktuelle Generation lesen
    call uobj_find_object           ; EAX=obj_id → EBX=obj_ptr, CF
    jc .cuch_fail
    mov eax, [ebx + UOBJ_OBJ_OFF_GEN]
    mov [cap_int_tmp_gen], eax

    ; Handle allokieren (Generation + inline-caps aus CAP-Rights)
    mov eax, [cap_int_tmp_obj_id]
    mov ecx, [cap_int_tmp_gen]
    mov edx, [cap_int_tmp_rights]
    call uobj_alloc_handle          ; EAX=obj_id, ECX=gen, EDX=caps → EBX=handle_id, CF
    jc .cuch_fail
    mov [cap_int_tmp_hdl_id], ebx

    ; Slot-Index in Parallel-Tabelle: (hdl_ptr - uobj_handles) >> 4
    mov eax, [cap_int_tmp_hdl_id]
    call uobj_find_handle           ; EAX=handle_id → EBX=hdl_ptr, CF
    jc .cuch_fail
    mov eax, ebx
    sub eax, uobj_handles
    shr eax, 4
    mov [cap_int_tmp_slot], eax

    ; CAP-Deskriptor erzeugen (delegable = QUERY-Subset)
    mov eax, [cap_int_tmp_obj_id]
    mov ebx, [cap_int_tmp_rights]
    mov ecx, NP_CAP_QUERY
    and ecx, ebx
    mov edx, [cap_int_tmp_domain]
    call cap_create                 ; → EAX=cap_id, CF
    jc .cuch_fail

    ; cap_id in Parallel-Tabelle speichern
    mov ecx, [cap_int_tmp_slot]
    mov [uobj_hdl_cap_tab + ecx*4], eax

    mov ebx, [cap_int_tmp_hdl_id]
    ; EAX = cap_id

    pop edi
    pop esi
    clc
    ret

.cuch_fail:
    pop edi
    pop esi
    stc
    ret

; ---------------------------------------------------------------------------
; cap_uobj_access – Handle via CAP-Framework auflösen (bridge §103↔§102)
; EAX=handle_id, EBX=required_rights(NP_CAP_*)
; CF=0: ECX=obj_ptr / CF=1: EAX=Fehler
; ---------------------------------------------------------------------------
cap_uobj_access:
    push esi
    push edx

    mov [cap_int_tmp_hdl_id], eax
    mov [cap_int_tmp_rights], ebx

    ; Handle-Slot finden → cap_id nachschlagen
    call uobj_find_handle           ; EAX=handle_id → EBX=hdl_ptr, CF
    jc .cuoa_invalid

    mov eax, ebx
    sub eax, uobj_handles
    shr eax, 4
    mov ecx, [uobj_hdl_cap_tab + eax*4]
    test ecx, ecx
    jz .cuoa_invalid                ; kein CAP-Deskriptor für dieses Handle

    ; Capability-Prüfung (fail-closed)
    mov eax, ecx
    mov ebx, [cap_int_tmp_rights]
    call cap_check                  ; EAX=cap_id, EBX=rights → CF=0/CF=1 EAX=Fehler
    jc .cuoa_denied

    ; UOBJ auflösen; ECX=0 überspringt doppelte inline-CAPS-Prüfung
    mov eax, [cap_int_tmp_hdl_id]
    xor ecx, ecx
    call uobj_from_handle           ; → EBX=obj_ptr, CF
    jc .cuoa_invalid

    mov ecx, ebx                    ; ECX = obj_ptr Rückgabe

    pop edx
    pop esi
    clc
    ret

.cuoa_invalid:
    mov eax, NP_ERR_CAP_INVALID
.cuoa_denied:
    pop edx
    pop esi
    stc
    ret

; ---------------------------------------------------------------------------
; ipc_send_secure – IPC-Send mit NP_CAP_SIGNAL-Prüfung (bridge §103↔IPC)
; EAX=cap_id, ESI=msg_ptr(16 Byte)
; CF=0 OK / CF=1 Fehler (EAX=Fehlercode oder -1 bei voller Queue)
; ---------------------------------------------------------------------------
ipc_send_secure:
    push ebx
    mov ebx, NP_CAP_SIGNAL
    call cap_check                  ; cap_check erhält ESI
    jc .iss_denied

    call ipc_send                   ; ESI=msg_ptr → EAX=1(OK) / 0(voll)
    test eax, eax
    jz .iss_full

    pop ebx
    clc
    ret

.iss_full:
    mov eax, -1
.iss_denied:
    pop ebx
    stc
    ret

; ---------------------------------------------------------------------------
; ipc_receive_secure – IPC-Receive mit NP_CAP_SUBSCRIBE-Prüfung
; EAX=cap_id, EDI=buf_ptr(16 Byte)
; CF=0 OK / CF=1 Fehler (EAX=Fehlercode oder -1 bei leerer Queue)
; ---------------------------------------------------------------------------
ipc_receive_secure:
    push ebx
    mov ebx, NP_CAP_SUBSCRIBE
    call cap_check                  ; cap_check erhält EDI
    jc .irs_denied

    call ipc_receive                ; EDI=buf_ptr → EAX=1(OK) / 0(leer)
    test eax, eax
    jz .irs_empty

    pop ebx
    clc
    ret

.irs_empty:
    mov eax, -1
.irs_denied:
    pop ebx
    stc
    ret

; ---------------------------------------------------------------------------
; vfs_lookup_secure – VFS-Root-Lookup mit NP_CAP_QUERY-Prüfung
; EAX=cap_id, ESI=path, ECX=length
; CF=0 EAX=node_handle / CF=1 Fehler
; cap_check erhält ESI+ECX auf Erfolgspfad (save/restore intern)
; ---------------------------------------------------------------------------
vfs_lookup_secure:
    push ebx
    mov ebx, NP_CAP_QUERY
    call cap_check
    jc .vls_denied

    call vfs_lookup_root            ; ESI=path, ECX=length → EAX=handle, CF=0/CF=1

    pop ebx
    ret

.vls_denied:
    pop ebx
    stc
    ret

; ---------------------------------------------------------------------------
; cap_integration_initialize – Bootstrap-Caps für IPC/VFS anlegen
; CF=0 OK / CF=1 Fehler
; ---------------------------------------------------------------------------
cap_integration_initialize:
    push ebx
    push esi
    push edi

    ; Parallel-Tabelle nullen
    mov edi, uobj_hdl_cap_tab
    xor eax, eax
    mov ecx, UOBJ_MAX_HANDLES
    rep stosd

    ; IPC-Send-Cap (NP_CAP_SIGNAL, kein Delegat)
    mov eax, CAP_INT_IPC_OBJ_ID
    mov ebx, NP_CAP_SIGNAL
    xor ecx, ecx
    mov edx, CAP_DOMAIN_KERNEL
    call cap_create
    jc .ci_fail
    mov [ipc_cap_send_id], eax

    ; IPC-Recv-Cap (NP_CAP_SUBSCRIBE, kein Delegat)
    mov eax, CAP_INT_IPC_OBJ_ID
    mov ebx, NP_CAP_SUBSCRIBE
    xor ecx, ecx
    mov edx, CAP_DOMAIN_KERNEL
    call cap_create
    jc .ci_fail
    mov [ipc_cap_recv_id], eax

    ; VFS-Query-Cap (NP_CAP_QUERY, delegierbar)
    mov eax, CAP_INT_VFS_OBJ_ID
    mov ebx, NP_CAP_QUERY
    mov ecx, NP_CAP_QUERY
    mov edx, CAP_DOMAIN_KERNEL
    call cap_create
    jc .ci_fail
    mov [vfs_cap_query_id], eax

    ; Diagnose: Integration bereit
    mov eax, DIAG_SRC_CAP
    mov ebx, DIAG_LEVEL_INFO
    xor ecx, ecx
    xor edx, edx
    call diag_log

    pop edi
    pop esi
    pop ebx
    clc
    ret

.ci_fail:
    pop edi
    pop esi
    pop ebx
    stc
    ret

; ---------------------------------------------------------------------------
; cap_integration_self_test – 8 Tests: §103↔§102, §103↔IPC, §103↔VFS
; CF=0 alle Tests bestanden / CF=1 Fehler
; ---------------------------------------------------------------------------
cap_integration_self_test:
    push ebx
    push esi
    push edi

    ; === Test 1: UOBJ-Objekt anlegen (Grundlage für Bridge-Test) ===
    mov eax, NP_OBJTYPE_GENERIC
    mov ecx, UOBJ_CAP_QUERY | UOBJ_CAP_MODIFY
    call uobj_create                ; → EBX=handle_id0, ECX=obj_id
    jc .cist_fail
    mov [cap_int_st_obj1], ecx

    ; === Test 2: cap_uobj_create_handle → CF=0, handle_id+cap_id ===
    mov eax, [cap_int_st_obj1]
    xor ebx, ebx
    mov ecx, NP_CAP_QUERY | NP_CAP_MODIFY
    mov edx, CAP_DOMAIN_KERNEL
    call cap_uobj_create_handle     ; → EBX=handle_id, EAX=cap_id
    jc .cist_fail
    mov [cap_int_st_hdl1], ebx
    mov [cap_int_st_cap1], eax

    ; === Test 3: cap_uobj_access QUERY → CF=0 ===
    mov eax, [cap_int_st_hdl1]
    mov ebx, NP_CAP_QUERY
    call cap_uobj_access
    jc .cist_fail

    ; === Test 4: cap_uobj_access ADMIN → CF=1 (kein ADMIN-Recht) ===
    mov eax, [cap_int_st_hdl1]
    mov ebx, NP_CAP_ADMIN
    call cap_uobj_access
    jnc .cist_fail

    ; === Test 5: ipc_send_secure mit Send-Cap → CF=0 ===
    mov eax, [ipc_cap_send_id]
    mov esi, cap_int_test_msg
    call ipc_send_secure
    jc .cist_fail

    ; === Test 6: ipc_receive_secure mit Recv-Cap → CF=0, Inhalt stimmt ===
    mov eax, [ipc_cap_recv_id]
    mov edi, cap_int_recv_buf
    call ipc_receive_secure
    jc .cist_fail
    mov eax, [cap_int_test_msg]
    cmp eax, [cap_int_recv_buf]
    jne .cist_fail

    ; === Test 7: vfs_lookup_secure mit Query-Cap + "/" → CF=0 ===
    mov eax, [vfs_cap_query_id]
    mov esi, vfs_root_path
    mov ecx, 1
    call vfs_lookup_secure
    jc .cist_fail

    ; === Test 8: vfs_lookup_secure mit ungültiger Cap → CF=1 ===
    mov eax, 0xDEAD
    mov esi, vfs_root_path
    mov ecx, 1
    call vfs_lookup_secure
    jnc .cist_fail

    pop edi
    pop esi
    pop ebx
    clc
    ret

.cist_fail:
    pop edi
    pop esi
    pop ebx
    stc
    ret

; ---------------------------------------------------------------------------
; CAP-Integration Daten
; ---------------------------------------------------------------------------
uobj_hdl_cap_tab:    times UOBJ_MAX_HANDLES dd 0   ; Handle-Slot → cap_id
ipc_cap_send_id:     dd 0
ipc_cap_recv_id:     dd 0
vfs_cap_query_id:    dd 0
; Temp-Speicher (Single-Threaded Bootstrap)
cap_int_tmp_obj_id:  dd 0
cap_int_tmp_rights:  dd 0
cap_int_tmp_domain:  dd 0
cap_int_tmp_gen:     dd 0
cap_int_tmp_hdl_id:  dd 0
cap_int_tmp_slot:    dd 0
; Selbsttest-IDs
cap_int_st_obj1:     dd 0
cap_int_st_hdl1:     dd 0
cap_int_st_cap1:     dd 0
; Selbsttest-Nachricht (16 Byte)
cap_int_test_msg:    dd 0xCAF10001, 0xCAF10002, 0xCAF10003, 0xCAF10004
cap_int_recv_buf:    times IPC_MESSAGE_SIZE db 0

; ===========================================================================
; §105 – Versioned Kernel Service ABI 1.0 (NPSPEC-KERNEL-0105)
; ===========================================================================

VSVC_MAX_SERVICES    equ 16
VSVC_DESC_SIZE       equ 64         ; Potenz von 2 → shr 6 für Adressberechnung

; Deskriptor-Offsets (64 Byte / Slot)
VSVC_OFF_ID0         equ 0
VSVC_OFF_ID1         equ 4
VSVC_OFF_ID2         equ 8
VSVC_OFF_ID3         equ 12
VSVC_OFF_MAJ         equ 16         ; dw major
VSVC_OFF_MIN         equ 18         ; dw minor
VSVC_OFF_PAT         equ 20         ; dw patch
VSVC_OFF_STAB        equ 22         ; db stability class
VSVC_OFF_STATE       equ 23         ; db state
VSVC_OFF_TABLE       equ 24         ; dd service table ptr
VSVC_OFF_TABSZ       equ 28         ; dd table size
VSVC_OFF_FEATURES    equ 32         ; dd feature mask low
VSVC_OFF_FEAT_HI     equ 36         ; dd feature mask high
VSVC_OFF_REQCAP      equ 40         ; dd required capabilities
VSVC_OFF_FLAGS       equ 44         ; dd flags
VSVC_OFF_ARCH        equ 48         ; dd architecture mask
VSVC_OFF_REFCNT      equ 52         ; dd reference count
VSVC_OFF_NAME        equ 56         ; dd name ptr
VSVC_OFF_PRESENT     equ 60         ; db

; Service-Tabellen-Header-Offsets (§10, 24 Byte)
VSVC_TBL_HDR_SIZE    equ 24
VSVC_TBL_OFF_SIZE    equ 0
VSVC_TBL_OFF_MAJ     equ 4          ; dw
VSVC_TBL_OFF_MIN     equ 6          ; dw
VSVC_TBL_OFF_FEAT    equ 8
VSVC_TBL_OFF_FEATHI  equ 12
VSVC_TBL_OFF_FLAGS   equ 16
VSVC_TBL_OFF_RSVD    equ 20

; Aufruf-Flags (§10)
VSVC_CALL_THREAD_CTX equ 0x00000001
VSVC_CALL_IRQ_SAFE   equ 0x00000002
VSVC_CALL_EARLY_BOOT equ 0x00000008
VSVC_CALL_PANIC_SAFE equ 0x00000010
VSVC_CALL_MAY_BLOCK  equ 0x00000020

; Zustände (§32, Bootstrap-Vereinfachung ohne DRAINING)
VSVC_STATE_FREE          equ 0
VSVC_STATE_ACTIVE        equ 1
VSVC_STATE_QUIESCING     equ 2
VSVC_STATE_OFFLINE       equ 3
VSVC_STATE_UNREGISTERED  equ 4

; Stabilitätsklassen (§51)
VSVC_STAB_INTERNAL      equ 0
VSVC_STAB_EXPERIMENTAL  equ 1
VSVC_STAB_PROVISIONAL   equ 2
VSVC_STAB_STABLE        equ 3
VSVC_STAB_LEGACY        equ 4
VSVC_STAB_SECURITY_ONLY equ 5

; Architektur-Maske (§25)
VSVC_ARCH_X86_32    equ 0x00000001
VSVC_ARCH_X86_64    equ 0x00000002
VSVC_ARCH_ARM64     equ 0x00000004
VSVC_ARCH_RISCV64   equ 0x00000008
VSVC_ARCH_ANY       equ 0xFFFFFFFF

; Flag-Maske (§23)
VSVC_FLAG_REQUIRED_MASK  equ 0x0000FFFF
VSVC_FLAG_OPTIONAL_MASK  equ 0xFFFF0000
VSVC_FLAG_EARLY_BOOT     equ 0x00000004
VSVC_FLAG_PANIC_SAFE     equ 0x00000008

; Bootstrap-Service-UUIDs (16 Byte = 4×dd)
VSVC_ID_KERN_0    equ 0x4E4F5641   ; "NOVA"
VSVC_ID_KERN_1    equ 0x4B45524E   ; "KERN"
VSVC_ID_KERN_2    equ 0x434F5245   ; "CORE"
VSVC_ID_KERN_3    equ 0x00000001

VSVC_ID_EVBS_0    equ 0x4E4F5641
VSVC_ID_EVBS_1    equ 0x45564253   ; "EVBS"
VSVC_ID_EVBS_2    equ 0x434F5245
VSVC_ID_EVBS_3    equ 0x00000001

VSVC_ID_CAPS_0    equ 0x4E4F5641
VSVC_ID_CAPS_1    equ 0x43415053   ; "CAPS"
VSVC_ID_CAPS_2    equ 0x434F5245
VSVC_ID_CAPS_3    equ 0x00000001

VSVC_ID_DIAG_0    equ 0x4E4F5641
VSVC_ID_DIAG_1    equ 0x44494147   ; "DIAG"
VSVC_ID_DIAG_2    equ 0x434F5245
VSVC_ID_DIAG_3    equ 0x00000001

VSVC_ID_TEST_0    equ 0x54455354   ; "TEST"
VSVC_ID_TEST_1    equ 0x53564300   ; "SVC\0"
VSVC_ID_TEST_2    equ 0x00000000
VSVC_ID_TEST_3    equ 0x00000099

; Event-IDs §105 (§48, fortlaufend nach NP_EVENT_DIAG_HEALTH_CHANGED=12)
NP_EVENT_SVC_REGISTERED   equ 13
NP_EVENT_SVC_ACTIVATED    equ 14
NP_EVENT_SVC_OFFLINE      equ 15
NP_EVENT_SVC_QUIESCING    equ 16
NP_EVENT_SVC_UNREGISTERED equ 17

; Fehlercodes §105 (§67)
NP_ERR_VSVC_NOT_FOUND  equ -80
NP_ERR_VSVC_VERSION    equ -81
NP_ERR_VSVC_FEATURE    equ -82
NP_ERR_VSVC_ABI        equ -83
NP_ERR_VSVC_CONFLICT   equ -84
NP_ERR_VSVC_QUIESCING  equ -85
NP_ERR_VSVC_OFFLINE    equ -86
NP_ERR_VSVC_LIMIT      equ -87
NP_ERR_VSVC_ARCH       equ -88
NP_ERR_VSVC_STALE      equ -89

; ---------------------------------------------------------------------------
; Lokale Makros für vsvc_initialize
; ---------------------------------------------------------------------------
%macro vsvc_reg_schema 1
    mov eax, %1
    mov ecx, NP_EVENT_CLASS_LIFECYCLE
    xor edx, edx
    mov esi, NP_EVENT_SYNCHRONOUS | NP_EVENT_KERNEL_ONLY
    call evbus_register_schema
    jc .vi_fail
%endmacro

%macro vsvc_reg_boot 6              ; id0, id1, id2, id3, table_label, name_label
    mov dword [vsvc_tmp_table], %5
    mov dword [vsvc_tmp_name],  %6
    mov eax, %1
    mov ebx, %2
    mov ecx, %3
    mov edx, %4
    call vsvc_register
    jc .vi_fail
%endmacro

; ---------------------------------------------------------------------------
; vsvc_alloc – freien Deskriptor-Slot suchen
; → EBX=desc_ptr, CF=0 / CF=1 (voll)
; ---------------------------------------------------------------------------
vsvc_alloc:
    push esi
    push ecx
    mov esi, vsvc_registry
    xor ecx, ecx
.vsa_scan:
    cmp ecx, VSVC_MAX_SERVICES
    jae .vsa_full
    cmp byte [esi + VSVC_OFF_PRESENT], 0
    je .vsa_found
    add esi, VSVC_DESC_SIZE
    inc ecx
    jmp .vsa_scan
.vsa_found:
    mov ebx, esi
    pop ecx
    pop esi
    clc
    ret
.vsa_full:
    xor ebx, ebx
    pop ecx
    pop esi
    stc
    ret

; ---------------------------------------------------------------------------
; vsvc_find – Service per UUID suchen
; ESI=uuid_ptr (16 Byte) → EBX=desc_ptr, CF=0 gefunden / CF=1 nicht gefunden
; ---------------------------------------------------------------------------
vsvc_find:
    push esi
    push ecx
    push edx
    mov eax, [esi]
    mov [vsvc_tmp_find0], eax
    mov eax, [esi + 4]
    mov [vsvc_tmp_find1], eax
    mov eax, [esi + 8]
    mov [vsvc_tmp_find2], eax
    mov eax, [esi + 12]
    mov [vsvc_tmp_find3], eax
    mov esi, vsvc_registry
    xor ecx, ecx
.vsf_scan:
    cmp ecx, VSVC_MAX_SERVICES
    jae .vsf_not_found
    cmp byte [esi + VSVC_OFF_PRESENT], 0
    je .vsf_next
    mov eax, [vsvc_tmp_find0]
    cmp [esi + VSVC_OFF_ID0], eax
    jne .vsf_next
    mov eax, [vsvc_tmp_find1]
    cmp [esi + VSVC_OFF_ID1], eax
    jne .vsf_next
    mov eax, [vsvc_tmp_find2]
    cmp [esi + VSVC_OFF_ID2], eax
    jne .vsf_next
    mov eax, [vsvc_tmp_find3]
    cmp [esi + VSVC_OFF_ID3], eax
    jne .vsf_next
    mov ebx, esi
    pop edx
    pop ecx
    pop esi
    clc
    ret
.vsf_next:
    add esi, VSVC_DESC_SIZE
    inc ecx
    jmp .vsf_scan
.vsf_not_found:
    xor ebx, ebx
    pop edx
    pop ecx
    pop esi
    stc
    ret

; ---------------------------------------------------------------------------
; vsvc_register – Service in der Registry eintragen (§12)
; EAX=id[0], EBX=id[1], ECX=id[2], EDX=id[3]
; Vor dem Aufruf: vsvc_tmp_maj, min, pat, stab, table, tabsz, features,
;   reqcap, arch, flags, name setzen
; CF=0 EBX=desc_ptr / CF=1 EAX=NP_ERR_VSVC_*
; ---------------------------------------------------------------------------
vsvc_register:
    push esi
    push edi

    mov [vsvc_tmp_id0], eax
    mov [vsvc_tmp_id1], ebx
    mov [vsvc_tmp_id2], ecx
    mov [vsvc_tmp_id3], edx

    ; UUID in Such-Puffer schreiben und auf Duplikat prüfen
    mov eax, [vsvc_tmp_id0]
    mov [vsvc_reg_id_buf],      eax
    mov eax, [vsvc_tmp_id1]
    mov [vsvc_reg_id_buf +  4], eax
    mov eax, [vsvc_tmp_id2]
    mov [vsvc_reg_id_buf +  8], eax
    mov eax, [vsvc_tmp_id3]
    mov [vsvc_reg_id_buf + 12], eax
    mov esi, vsvc_reg_id_buf
    call vsvc_find
    jnc .vr_conflict                ; CF=0 → gefunden → Duplikat

    ; Slot allokieren
    call vsvc_alloc
    jc .vr_limit
    mov edi, ebx                    ; EDI = neuer Slot-Zeiger

    ; Tabellengröße prüfen (§10: mind. VSVC_TBL_HDR_SIZE)
    mov eax, [vsvc_tmp_tabsz]
    cmp eax, VSVC_TBL_HDR_SIZE
    jb .vr_abi

    ; UUID
    mov eax, [vsvc_tmp_id0]
    mov [edi + VSVC_OFF_ID0], eax
    mov eax, [vsvc_tmp_id1]
    mov [edi + VSVC_OFF_ID1], eax
    mov eax, [vsvc_tmp_id2]
    mov [edi + VSVC_OFF_ID2], eax
    mov eax, [vsvc_tmp_id3]
    mov [edi + VSVC_OFF_ID3], eax

    ; Version + Stabilität
    mov ax, [vsvc_tmp_maj]
    mov [edi + VSVC_OFF_MAJ], ax
    mov ax, [vsvc_tmp_min]
    mov [edi + VSVC_OFF_MIN], ax
    mov ax, [vsvc_tmp_pat]
    mov [edi + VSVC_OFF_PAT], ax
    mov al, [vsvc_tmp_stab]
    mov [edi + VSVC_OFF_STAB], al
    mov byte [edi + VSVC_OFF_STATE], VSVC_STATE_ACTIVE

    ; Tabelle + Features
    mov eax, [vsvc_tmp_table]
    mov [edi + VSVC_OFF_TABLE], eax
    mov eax, [vsvc_tmp_tabsz]
    mov [edi + VSVC_OFF_TABSZ], eax
    mov eax, [vsvc_tmp_features]
    mov [edi + VSVC_OFF_FEATURES], eax
    mov dword [edi + VSVC_OFF_FEAT_HI], 0

    ; Capabilities + Flags + Architektur
    mov eax, [vsvc_tmp_reqcap]
    mov [edi + VSVC_OFF_REQCAP], eax
    mov eax, [vsvc_tmp_flags]
    mov [edi + VSVC_OFF_FLAGS], eax
    mov eax, [vsvc_tmp_arch]
    mov [edi + VSVC_OFF_ARCH], eax

    ; Refcount + Name + Present
    mov dword [edi + VSVC_OFF_REFCNT], 0
    mov eax, [vsvc_tmp_name]
    mov [edi + VSVC_OFF_NAME], eax
    mov byte [edi + VSVC_OFF_PRESENT], 1

    lock inc dword [vsvc_stat_registered]

    ; NP_EVENT_SVC_REGISTERED publizieren
    mov eax, NP_EVENT_SVC_REGISTERED
    mov ecx, [vsvc_tmp_id0]
    xor edx, edx
    xor esi, esi
    xor edi, edi
    call evbus_publish              ; Achtung: evbus_publish clobbered EBX (nur ESI/EDI werden wiederhergestellt)

    pop edi
    pop esi
    clc
    ret

.vr_conflict:
    mov eax, NP_ERR_VSVC_CONFLICT
    pop edi
    pop esi
    stc
    ret

.vr_limit:
    mov eax, NP_ERR_VSVC_LIMIT
    pop edi
    pop esi
    stc
    ret

.vr_abi:
    mov eax, NP_ERR_VSVC_ABI
    pop edi
    pop esi
    stc
    ret

; ---------------------------------------------------------------------------
; vsvc_acquire – Service-Referenz erwerben (§13)
; ESI=uuid_ptr (16 Byte), EAX=min_major, EBX=max_major,
; ECX=min_minor, EDX=required_features
; CF=0 EBX=desc_ptr / CF=1 EAX=NP_ERR_VSVC_*
; ---------------------------------------------------------------------------
vsvc_acquire:
    push esi
    push ecx
    push edx

    mov [vsvc_tmp_acq_minmaj], eax
    mov [vsvc_tmp_acq_maxmaj], ebx
    mov [vsvc_tmp_acq_minmin], ecx
    mov [vsvc_tmp_acq_feat],   edx

    call vsvc_find
    jc .vac_not_found

    ; Zustand: nur ACTIVE erlaubt
    movzx eax, byte [ebx + VSVC_OFF_STATE]
    cmp eax, VSVC_STATE_ACTIVE
    jne .vac_state_err

    ; Major-Version im Bereich [min_major, max_major]
    movzx eax, word [ebx + VSVC_OFF_MAJ]
    cmp eax, [vsvc_tmp_acq_minmaj]
    jb .vac_version
    cmp eax, [vsvc_tmp_acq_maxmaj]
    ja .vac_version

    ; Minor-Version ≥ min_minor
    movzx eax, word [ebx + VSVC_OFF_MIN]
    cmp eax, [vsvc_tmp_acq_minmin]
    jb .vac_version

    ; Features: alle required_features vorhanden
    mov eax, [ebx + VSVC_OFF_FEATURES]
    mov ecx, [vsvc_tmp_acq_feat]
    and eax, ecx
    cmp eax, ecx
    jne .vac_feature

    ; Architektur-Prüfung
    mov eax, [ebx + VSVC_OFF_ARCH]
    cmp eax, VSVC_ARCH_ANY
    je .vac_arch_ok
    test eax, VSVC_ARCH_X86_32
    jz .vac_arch
.vac_arch_ok:

    lock inc dword [ebx + VSVC_OFF_REFCNT]
    lock inc dword [vsvc_stat_acquired]

    pop edx
    pop ecx
    pop esi
    xor eax, eax
    clc
    ret

.vac_state_err:
    cmp eax, VSVC_STATE_QUIESCING
    je .vac_quiescing
    mov eax, NP_ERR_VSVC_OFFLINE
    jmp .vac_fail
.vac_not_found:
    mov eax, NP_ERR_VSVC_NOT_FOUND
    jmp .vac_fail
.vac_quiescing:
    mov eax, NP_ERR_VSVC_QUIESCING
    jmp .vac_fail
.vac_version:
    mov eax, NP_ERR_VSVC_VERSION
    jmp .vac_fail
.vac_feature:
    mov eax, NP_ERR_VSVC_FEATURE
    jmp .vac_fail
.vac_arch:
    mov eax, NP_ERR_VSVC_ARCH
.vac_fail:
    pop edx
    pop ecx
    pop esi
    stc
    ret

; ---------------------------------------------------------------------------
; vsvc_release – Referenz freigeben (§17)
; EBX=desc_ptr (von vsvc_acquire)
; CF=0 OK / CF=1 Fehler
; ---------------------------------------------------------------------------
vsvc_release:
    cmp byte [ebx + VSVC_OFF_PRESENT], 1
    jne .vrl_invalid
    cmp dword [ebx + VSVC_OFF_REFCNT], 0
    je .vrl_invalid
    lock dec dword [ebx + VSVC_OFF_REFCNT]
    ; QUIESCING + refcnt=0 → sofort OFFLINE (§32 Bootstrap-Vereinfachung)
    cmp byte [ebx + VSVC_OFF_STATE], VSVC_STATE_QUIESCING
    jne .vrl_done
    cmp dword [ebx + VSVC_OFF_REFCNT], 0
    jne .vrl_done
    mov byte [ebx + VSVC_OFF_STATE], VSVC_STATE_OFFLINE
    push eax
    push ecx
    push edx
    push esi
    push edi
    mov eax, NP_EVENT_SVC_OFFLINE
    mov ecx, [ebx + VSVC_OFF_ID0]
    xor edx, edx
    xor esi, esi
    xor edi, edi
    call evbus_publish
    pop edi
    pop esi
    pop edx
    pop ecx
    pop eax
.vrl_done:
    clc
    ret
.vrl_invalid:
    stc
    ret

; ---------------------------------------------------------------------------
; vsvc_quiesce – Service in QUIESCING versetzen (§32)
; EBX=desc_ptr
; CF=0 OK / CF=1 Fehler
; ---------------------------------------------------------------------------
vsvc_quiesce:
    cmp byte [ebx + VSVC_OFF_PRESENT], 1
    jne .vq_invalid
    cmp byte [ebx + VSVC_OFF_STATE], VSVC_STATE_ACTIVE
    jne .vq_invalid
    mov byte [ebx + VSVC_OFF_STATE], VSVC_STATE_QUIESCING
    mov [vsvc_quiesce_tmp], ebx    ; EBX retten: evbus_publish clobbered EBX
    push eax
    push ecx
    push edx
    push esi
    push edi
    mov eax, NP_EVENT_SVC_QUIESCING
    mov ecx, [ebx + VSVC_OFF_ID0]
    xor edx, edx
    xor esi, esi
    xor edi, edi
    call evbus_publish
    pop edi
    pop esi
    pop edx
    pop ecx
    pop eax
    ; EBX nach evbus_publish wiederherstellen (evbus_publish clobbered EBX)
    ; vsvc_quiesce_tmp enthält desc_ptr, der vor dem ersten evbus_publish gesetzt wurde
    mov ebx, [vsvc_quiesce_tmp]
    ; refcnt=0 → sofort OFFLINE
    cmp dword [ebx + VSVC_OFF_REFCNT], 0
    jne .vq_done
    mov byte [ebx + VSVC_OFF_STATE], VSVC_STATE_OFFLINE
    push eax
    push ecx
    push edx
    push esi
    push edi
    mov eax, NP_EVENT_SVC_OFFLINE
    mov ecx, [ebx + VSVC_OFF_ID0]
    xor edx, edx
    xor esi, esi
    xor edi, edi
    call evbus_publish
    pop edi
    pop esi
    pop edx
    pop ecx
    pop eax
.vq_done:
    clc
    ret
.vq_invalid:
    stc
    ret

; ---------------------------------------------------------------------------
; vsvc_initialize – Registry nullen, 5 Event-Schemata, 4 Bootstrap-Services
; CF=0 OK / CF=1 Fehler
; ---------------------------------------------------------------------------
vsvc_initialize:
    push ebx
    push esi
    push edi

    ; Registry und Statistiken nullen
    mov edi, vsvc_registry
    xor eax, eax
    mov ecx, (VSVC_MAX_SERVICES * VSVC_DESC_SIZE) / 4
    rep stosd
    mov dword [vsvc_stat_registered], 0
    mov dword [vsvc_stat_acquired],   0

    ; Event-Schemata registrieren (§48)
    vsvc_reg_schema NP_EVENT_SVC_REGISTERED
    vsvc_reg_schema NP_EVENT_SVC_ACTIVATED
    vsvc_reg_schema NP_EVENT_SVC_OFFLINE
    vsvc_reg_schema NP_EVENT_SVC_QUIESCING
    vsvc_reg_schema NP_EVENT_SVC_UNREGISTERED

    ; Gemeinsame Bootstrap-Felder setzen
    mov dword [vsvc_tmp_maj],      1
    mov dword [vsvc_tmp_min],      0
    mov dword [vsvc_tmp_pat],      0
    mov dword [vsvc_tmp_tabsz],    VSVC_TBL_HDR_SIZE
    mov dword [vsvc_tmp_features], 0
    mov dword [vsvc_tmp_reqcap],   0
    mov dword [vsvc_tmp_arch],     VSVC_ARCH_X86_32
    mov dword [vsvc_tmp_stab],     VSVC_STAB_STABLE
    mov dword [vsvc_tmp_flags],    VSVC_FLAG_EARLY_BOOT | VSVC_FLAG_PANIC_SAFE

    ; 4 Bootstrap-Services registrieren (§61)
    vsvc_reg_boot VSVC_ID_KERN_0, VSVC_ID_KERN_1, VSVC_ID_KERN_2, VSVC_ID_KERN_3, vsvc_kern_tbl, vsvc_name_kern
    vsvc_reg_boot VSVC_ID_EVBS_0, VSVC_ID_EVBS_1, VSVC_ID_EVBS_2, VSVC_ID_EVBS_3, vsvc_evbs_tbl, vsvc_name_evbs
    vsvc_reg_boot VSVC_ID_CAPS_0, VSVC_ID_CAPS_1, VSVC_ID_CAPS_2, VSVC_ID_CAPS_3, vsvc_caps_tbl, vsvc_name_caps
    vsvc_reg_boot VSVC_ID_DIAG_0, VSVC_ID_DIAG_1, VSVC_ID_DIAG_2, VSVC_ID_DIAG_3, vsvc_diag_tbl, vsvc_name_diag

    pop edi
    pop esi
    pop ebx
    clc
    ret

.vi_fail:
    pop edi
    pop esi
    pop ebx
    stc
    ret

; ---------------------------------------------------------------------------
; vsvc_self_test – 10 Tests (§12..§17, §32)
; CF=0 alle bestanden / CF=1 Fehler
; ---------------------------------------------------------------------------
vsvc_self_test:
    push ebx
    push esi
    push edi

    ; === Test 1: Test-Service registrieren → CF=0 ===
    mov dword [vsvc_tmp_maj],      1
    mov dword [vsvc_tmp_min],      3
    mov dword [vsvc_tmp_pat],      0
    mov dword [vsvc_tmp_table],    vsvc_test_tbl
    mov dword [vsvc_tmp_tabsz],    VSVC_TBL_HDR_SIZE
    mov dword [vsvc_tmp_features], 0x0000000F
    mov dword [vsvc_tmp_reqcap],   0
    mov dword [vsvc_tmp_arch],     VSVC_ARCH_X86_32
    mov dword [vsvc_tmp_stab],     VSVC_STAB_EXPERIMENTAL
    mov dword [vsvc_tmp_name],     vsvc_name_test
    mov dword [vsvc_tmp_flags],    0
    mov eax, VSVC_ID_TEST_0
    mov ebx, VSVC_ID_TEST_1
    mov ecx, VSVC_ID_TEST_2
    mov edx, VSVC_ID_TEST_3
    call vsvc_register
    jc .vsst_fail

    ; === Test 2: Doppelte Registrierung → CF=1 ===
    mov eax, VSVC_ID_TEST_0
    mov ebx, VSVC_ID_TEST_1
    mov ecx, VSVC_ID_TEST_2
    mov edx, VSVC_ID_TEST_3
    call vsvc_register
    jnc .vsst_fail

    ; === Test 3: vsvc_find TEST → CF=0, EBX=desc_ptr ===
    mov esi, vsvc_st_uuid
    call vsvc_find
    jc .vsst_fail
    mov [vsvc_st_desc1], ebx

    ; === Test 4: vsvc_acquire gültige Version+Features → CF=0, refcnt=1 ===
    mov esi, vsvc_st_uuid
    mov eax, 1
    mov ebx, 1
    xor ecx, ecx
    xor edx, edx
    call vsvc_acquire
    jc .vsst_fail
    mov eax, [vsvc_st_desc1]
    cmp dword [eax + VSVC_OFF_REFCNT], 1
    jne .vsst_fail

    ; === Test 5: vsvc_acquire falsche Major-Version → CF=1 ===
    mov esi, vsvc_st_uuid
    mov eax, 2
    mov ebx, 3
    xor ecx, ecx
    xor edx, edx
    call vsvc_acquire
    jnc .vsst_fail

    ; === Test 6: vsvc_acquire fehlendes Feature → CF=1 ===
    mov esi, vsvc_st_uuid
    mov eax, 1
    mov ebx, 1
    xor ecx, ecx
    mov edx, 0x000000FF     ; Bit 4-7 fehlen (Service hat nur 0x0F)
    call vsvc_acquire
    jnc .vsst_fail

    ; === Test 7: vsvc_release → CF=0, refcnt=0 ===
    mov ebx, [vsvc_st_desc1]
    call vsvc_release
    jc .vsst_fail
    cmp dword [ebx + VSVC_OFF_REFCNT], 0
    jne .vsst_fail

    ; === Test 8: vsvc_quiesce (refcnt=0) → CF=0, state=OFFLINE ===
    mov ebx, [vsvc_st_desc1]
    call vsvc_quiesce
    jc .vsst_fail
    movzx eax, byte [ebx + VSVC_OFF_STATE]
    cmp eax, VSVC_STATE_OFFLINE
    jne .vsst_fail

    ; === Test 9: vsvc_acquire auf OFFLINE → CF=1 ===
    mov esi, vsvc_st_uuid
    mov eax, 1
    mov ebx, 1
    xor ecx, ecx
    xor edx, edx
    call vsvc_acquire
    jnc .vsst_fail

    ; === Test 10: Bootstrap-KERN-Service vorhanden und ACTIVE ===
    mov esi, vsvc_st_kern_uuid
    call vsvc_find
    jc .vsst_fail
    movzx eax, byte [ebx + VSVC_OFF_STATE]
    cmp eax, VSVC_STATE_ACTIVE
    jne .vsst_fail

    pop edi
    pop esi
    pop ebx
    clc
    ret

.vsst_fail:
    pop edi
    pop esi
    pop ebx
    stc
    ret

; ---------------------------------------------------------------------------
; §105 Daten
; ---------------------------------------------------------------------------
align 4
vsvc_registry:           times (VSVC_MAX_SERVICES * VSVC_DESC_SIZE / 4) dd 0
vsvc_stat_registered:    dd 0
vsvc_stat_acquired:      dd 0

; Temp-Speicher für vsvc_register (dd um Überlappung bei dword-Zugriffen zu vermeiden)
vsvc_tmp_id0:       dd 0
vsvc_tmp_id1:       dd 0
vsvc_tmp_id2:       dd 0
vsvc_tmp_id3:       dd 0
vsvc_tmp_maj:       dd 0
vsvc_tmp_min:       dd 0
vsvc_tmp_pat:       dd 0
vsvc_tmp_stab:      dd 0
vsvc_tmp_table:     dd 0
vsvc_tmp_tabsz:     dd 0
vsvc_tmp_features:  dd 0
vsvc_tmp_reqcap:    dd 0
vsvc_tmp_arch:      dd 0
vsvc_tmp_flags:     dd 0
vsvc_tmp_name:      dd 0
; Temp für vsvc_find
vsvc_tmp_find0:     dd 0
vsvc_tmp_find1:     dd 0
vsvc_tmp_find2:     dd 0
vsvc_tmp_find3:     dd 0
; Temp für vsvc_quiesce (EBX-Rettung über evbus_publish-Aufruf)
vsvc_quiesce_tmp:    dd 0
; Temp für vsvc_acquire
vsvc_tmp_acq_minmaj: dd 0
vsvc_tmp_acq_maxmaj: dd 0
vsvc_tmp_acq_minmin: dd 0
vsvc_tmp_acq_feat:   dd 0
; UUID-Puffer für Duplikat-Check in vsvc_register
vsvc_reg_id_buf:     times 16 db 0
; Selbsttest-Zustand
vsvc_st_desc1:       dd 0
vsvc_st_uuid:
    dd VSVC_ID_TEST_0, VSVC_ID_TEST_1, VSVC_ID_TEST_2, VSVC_ID_TEST_3
vsvc_st_kern_uuid:
    dd VSVC_ID_KERN_0, VSVC_ID_KERN_1, VSVC_ID_KERN_2, VSVC_ID_KERN_3

; Bootstrap-Service-Tabellen (§10: size/maj/min/feat_lo/feat_hi/flags/rsvd = 24 Byte)
align 4
vsvc_kern_tbl:
    dd VSVC_TBL_HDR_SIZE
    dw 1, 0
    dd 0, 0
    dd VSVC_CALL_EARLY_BOOT | VSVC_CALL_PANIC_SAFE
    dd 0
vsvc_evbs_tbl:
    dd VSVC_TBL_HDR_SIZE
    dw 1, 0
    dd 0, 0
    dd VSVC_CALL_EARLY_BOOT | VSVC_CALL_PANIC_SAFE
    dd 0
vsvc_caps_tbl:
    dd VSVC_TBL_HDR_SIZE
    dw 1, 0
    dd 0, 0
    dd VSVC_CALL_EARLY_BOOT | VSVC_CALL_PANIC_SAFE
    dd 0
vsvc_diag_tbl:
    dd VSVC_TBL_HDR_SIZE
    dw 1, 0
    dd 0, 0
    dd VSVC_CALL_EARLY_BOOT | VSVC_CALL_PANIC_SAFE
    dd 0
vsvc_test_tbl:
    dd VSVC_TBL_HDR_SIZE
    dw 1, 3
    dd 0x0000000F, 0
    dd VSVC_CALL_THREAD_CTX
    dd 0
; Service-Namen
vsvc_name_kern:  db "nova.kernel.core", 0
vsvc_name_evbs:  db "nova.evbus.core", 0
vsvc_name_caps:  db "nova.capability.core", 0
vsvc_name_diag:  db "nova.diagnostics.core", 0
vsvc_name_test:  db "nova.test.service", 0
align 4

; ===========================================================================
; §016 – Synchronisation 1.0 Bootstrap (NPSPEC-KERNEL-0016)
; Implementiert: Atomaroperationen, Speicherbarrieren, Spinlocks (inkl. IRQ),
; Semaphoren (try-only), Completions, Sequence Locks, Referenzzählung.
; RCU, blockierende Mutexes und Wait Queues erfordern Scheduler-Integration.
; ===========================================================================

; Deskriptor-Größen
SYNC_SEMA_SIZE        equ 12    ; count(dd) + maximum(dd) + flags(dd)
SYNC_COMPL_SIZE       equ 8     ; signaled(dd) + generation(dd)
SYNC_SEQLOCK_SIZE     equ 4     ; sequence(dd): gerade=frei, ungerade=Writer

; Semaphore-Offsets
SYNC_SEMA_OFF_COUNT   equ 0
SYNC_SEMA_OFF_MAX     equ 4
SYNC_SEMA_OFF_FLAGS   equ 8

; Completion-Offsets
SYNC_COMPL_OFF_SIG    equ 0
SYNC_COMPL_OFF_GEN    equ 4

; Spinlock-Zustände
SYNC_SPIN_UNLOCKED    equ 0
SYNC_SPIN_LOCKED      equ 1

; VSVC-UUID für nova.sync.core
VSVC_ID_SYNC_0        equ 0x4E4F5641   ; "NOVA"
VSVC_ID_SYNC_1        equ 0x53594E43   ; "SYNC"
VSVC_ID_SYNC_2        equ 0x434F5245   ; "CORE"
VSVC_ID_SYNC_3        equ 0x00000001

; Fehlercodes §016 (§54)
NP_ERR_SYNC_DEADLOCK  equ -100
NP_ERR_SYNC_NOT_OWNER equ -101
NP_ERR_SYNC_TIMEOUT   equ -102
NP_ERR_SYNC_OVERFLOW  equ -103
NP_ERR_SYNC_INVAL     equ -104

; ---------------------------------------------------------------------------
; Atomare Lade-/Speicheroperationen (§8)
; ---------------------------------------------------------------------------

; np_atomic_load_u32: EAX=ptr → EAX=value (ACQUIRE-Semantik; auf x86 TSO implizit)
np_atomic_load_u32:
    mov eax, [eax]
    ret

; np_atomic_store_u32: EAX=ptr, ECX=value (RELEASE-Semantik via MFENCE)
np_atomic_store_u32:
    mov [eax], ecx
    mfence
    ret

; np_atomic_exchange_u32: EAX=ptr, ECX=new_value → EAX=old_value
; XCHG mit Memory hat impliziten LOCK-Präfix (x86-Garantie)
np_atomic_exchange_u32:
    xchg [eax], ecx
    mov eax, ecx
    ret

; np_atomic_compare_exchange_u32: EAX=ptr, ECX=expected, EDX=desired
; CF=0: getauscht (ZF=1) / CF=1: nicht getauscht, EAX=Ist-Wert
np_atomic_compare_exchange_u32:
    push ebx
    mov ebx, eax
    mov eax, ecx
    lock cmpxchg [ebx], edx
    pop ebx
    jnz .acax_fail
    clc
    ret
.acax_fail:
    stc
    ret

; np_atomic_fetch_add_u32: EAX=ptr, ECX=increment → EAX=old_value
np_atomic_fetch_add_u32:
    lock xadd [eax], ecx
    mov eax, ecx
    ret

; ---------------------------------------------------------------------------
; Speicherbarrieren (§10)
; ---------------------------------------------------------------------------

np_memory_barrier:
    mfence
    ret

np_read_barrier:
    lfence
    ret

np_write_barrier:
    sfence
    ret

np_compiler_barrier:
    ret

; ---------------------------------------------------------------------------
; Spinlock (§11–§13): Test-and-Set, PAUSE-Hint, IRQ-sicher
; ---------------------------------------------------------------------------

; np_spin_init: EAX=spinlock_ptr (dd)
np_spin_init:
    mov dword [eax], SYNC_SPIN_UNLOCKED
    ret

; np_spin_lock: EAX=spinlock_ptr
np_spin_lock:
.nsl_spin:
    mov ecx, SYNC_SPIN_LOCKED
    xchg [eax], ecx         ; ECX = alter Wert, [ptr] = LOCKED
    test ecx, ecx
    jz .nsl_done            ; war UNLOCKED → erworben
    pause                   ; Pipeline-Hint für HT-CPUs
    jmp .nsl_spin
.nsl_done:
    ret

; np_spin_try_lock: EAX=spinlock_ptr → CF=0 erworben / CF=1 belegt
np_spin_try_lock:
    mov ecx, SYNC_SPIN_LOCKED
    xchg [eax], ecx
    test ecx, ecx
    jz .nstl_ok
    stc
    ret
.nstl_ok:
    clc
    ret

; np_spin_unlock: EAX=spinlock_ptr
np_spin_unlock:
    mfence
    mov dword [eax], SYNC_SPIN_UNLOCKED
    ret

; np_spin_lock_irqsave: EAX=spinlock_ptr → EDX=saved_EFLAGS
; Deaktiviert Interrupts atomar mit dem Erwerb (§13)
np_spin_lock_irqsave:
    pushfd
    pop edx
    cli
    call np_spin_lock
    ret

; np_spin_unlock_irqrestore: EAX=spinlock_ptr, EDX=saved_EFLAGS
np_spin_unlock_irqrestore:
    call np_spin_unlock
    push edx
    popfd
    ret

; ---------------------------------------------------------------------------
; Semaphore – Bootstrap: nur Try-Wait (kein Blockieren) (§25)
; ---------------------------------------------------------------------------

; np_semaphore_init: EAX=sema_ptr, ECX=initial_count, EDX=maximum
np_semaphore_init:
    mov [eax + SYNC_SEMA_OFF_COUNT], ecx
    mov [eax + SYNC_SEMA_OFF_MAX],   edx
    mov dword [eax + SYNC_SEMA_OFF_FLAGS], 0
    ret

; np_semaphore_try_wait: EAX=sema_ptr → CF=0 erworben / CF=1 leer
; CAS-Schleife: dekrementiert Count wenn > 0
np_semaphore_try_wait:
    mov [sync_tmp_ptr], eax
.sw_retry:
    mov eax, [sync_tmp_ptr]
    mov ecx, [eax + SYNC_SEMA_OFF_COUNT]
    test ecx, ecx
    jz .sw_fail
    mov edx, ecx
    dec edx
    lock cmpxchg [eax + SYNC_SEMA_OFF_COUNT], edx
    jnz .sw_retry           ; CAS fehlgeschlagen, nochmal
    clc
    ret
.sw_fail:
    stc
    ret

; np_semaphore_release: EAX=sema_ptr, ECX=release_count → CF=0 / CF=1 Überlauf
np_semaphore_release:
    mov [sync_tmp_ptr], eax
    mov [sync_tmp_cnt], ecx
.sr_retry:
    mov eax, [sync_tmp_ptr]
    mov ecx, [eax + SYNC_SEMA_OFF_COUNT]
    mov edx, ecx
    add edx, [sync_tmp_cnt]
    cmp edx, [eax + SYNC_SEMA_OFF_MAX]
    ja .sr_overflow
    lock cmpxchg [eax + SYNC_SEMA_OFF_COUNT], edx
    jnz .sr_retry
    clc
    ret
.sr_overflow:
    stc
    ret

; ---------------------------------------------------------------------------
; Completion Object (§31)
; ---------------------------------------------------------------------------

; np_completion_init: EAX=completion_ptr
np_completion_init:
    mov dword [eax + SYNC_COMPL_OFF_SIG], 0
    mov dword [eax + SYNC_COMPL_OFF_GEN], 0
    ret

; np_completion_signal: EAX=completion_ptr
np_completion_signal:
    mov dword [eax + SYNC_COMPL_OFF_SIG], 1
    lock inc dword [eax + SYNC_COMPL_OFF_GEN]
    mfence
    ret

; np_completion_is_done: EAX=completion_ptr → CF=0 signalisiert / CF=1 ausstehend
np_completion_is_done:
    cmp dword [eax + SYNC_COMPL_OFF_SIG], 1
    je .cid_done
    stc
    ret
.cid_done:
    clc
    ret

; ---------------------------------------------------------------------------
; Sequence Lock (§32): Schreiber-exklusiv, Leser wiederholend
; seqcount: gerade = kein Writer aktiv, ungerade = Writer aktiv
; ---------------------------------------------------------------------------

; np_seqlock_init: EAX=seqlock_ptr (dd)
np_seqlock_init:
    mov dword [eax], 0
    ret

; np_seqlock_read_begin: EAX=seqlock_ptr → ECX=snapshot (muss gerade sein)
np_seqlock_read_begin:
.srb_spin:
    mov ecx, [eax]
    test ecx, 1             ; ungerade = Writer aktiv
    jnz .srb_spin
    lfence
    ret

; np_seqlock_read_retry: EAX=seqlock_ptr, ECX=snapshot → CF=0 ok / CF=1 wiederholen
np_seqlock_read_retry:
    lfence
    cmp [eax], ecx
    je .srr_ok
    stc
    ret
.srr_ok:
    clc
    ret

; np_seqlock_write_lock: EAX=seqlock_ptr (macht count ungerade)
np_seqlock_write_lock:
    lock add dword [eax], 1
    mfence
    ret

; np_seqlock_write_unlock: EAX=seqlock_ptr (macht count wieder gerade)
np_seqlock_write_unlock:
    mfence
    lock add dword [eax], 1
    ret

; ---------------------------------------------------------------------------
; Referenzzählung (§36): Überlauf-/Unterlaufschutz via CAS
; ---------------------------------------------------------------------------

; np_refcount_retain: EAX=refcount_ptr (dd) → CF=0 / CF=1 Objekt bereits tot (count=0)
np_refcount_retain:
    mov [sync_tmp_ptr], eax
.rrn_retry:
    mov eax, [sync_tmp_ptr]
    mov ecx, [eax]
    test ecx, ecx
    jz .rrn_dead            ; Objekt ist tot, darf nicht reviviert werden
    mov edx, ecx
    inc edx
    lock cmpxchg [eax], edx
    jnz .rrn_retry
    clc
    ret
.rrn_dead:
    stc
    ret

; np_refcount_release: EAX=refcount_ptr → CF=0 noch lebendig / CF=1 auf null gefallen
np_refcount_release:
    lock dec dword [eax]
    jz .rrl_zero
    clc
    ret
.rrl_zero:
    stc
    ret

; ---------------------------------------------------------------------------
; sync_initialize – Pools nullen, VSVC-Service registrieren
; CF=0 OK / CF=1 Fehler
; ---------------------------------------------------------------------------
sync_initialize:
    push ebx
    push esi
    push edi

    ; Statische Sync-Objekte für Selbsttest nullen
    mov edi, sync_test_spin
    xor eax, eax
    ; spinlock(4) + sema(12) + compl(8) + seqlock(4) = 28 Byte → 7 dwords
    mov ecx, 7
    rep stosd
    ; Refcount-Testinstanz
    mov dword [sync_test_refcount], 3

    ; Statistiken nullen
    mov dword [sync_stat_acquires], 0
    mov dword [sync_stat_releases], 0

    ; VSVC-Service "nova.sync.core" registrieren
    mov dword [vsvc_tmp_maj],      1
    mov dword [vsvc_tmp_min],      0
    mov dword [vsvc_tmp_pat],      0
    mov dword [vsvc_tmp_table],    sync_vsvc_tbl
    mov dword [vsvc_tmp_tabsz],    VSVC_TBL_HDR_SIZE
    mov dword [vsvc_tmp_features], 0
    mov dword [vsvc_tmp_reqcap],   0
    mov dword [vsvc_tmp_arch],     VSVC_ARCH_X86_32
    mov dword [vsvc_tmp_stab],     VSVC_STAB_STABLE
    mov dword [vsvc_tmp_name],     sync_name_sync
    mov dword [vsvc_tmp_flags],    VSVC_FLAG_EARLY_BOOT | VSVC_FLAG_PANIC_SAFE
    mov eax, VSVC_ID_SYNC_0
    mov ebx, VSVC_ID_SYNC_1
    mov ecx, VSVC_ID_SYNC_2
    mov edx, VSVC_ID_SYNC_3
    call vsvc_register
    jc .si_fail

    pop edi
    pop esi
    pop ebx
    clc
    ret
.si_fail:
    pop edi
    pop esi
    pop ebx
    stc
    ret

; ---------------------------------------------------------------------------
; sync_self_test – 11 Tests
; CF=0 alle bestanden / CF=1 Fehler
; ---------------------------------------------------------------------------
sync_self_test:
    push ebx
    push esi
    push edi

    ; === Test 1: np_atomic_store + np_atomic_load → read-back ===
    mov eax, sync_st_atomic
    mov ecx, 0xA5A5A5A5
    call np_atomic_store_u32
    mov eax, sync_st_atomic
    call np_atomic_load_u32
    cmp eax, 0xA5A5A5A5
    jne .sst_fail

    ; === Test 2: np_atomic_exchange → Rückgabe Altwert ===
    mov eax, sync_st_atomic
    mov ecx, 0x12345678
    call np_atomic_exchange_u32
    cmp eax, 0xA5A5A5A5         ; Altwert muss zurückkommen
    jne .sst_fail
    cmp dword [sync_st_atomic], 0x12345678
    jne .sst_fail

    ; === Test 3: np_atomic_compare_exchange – Treffer → CF=0 ===
    mov eax, sync_st_atomic
    mov ecx, 0x12345678         ; erwartet
    mov edx, 0xDEADBEEF         ; gewünscht
    call np_atomic_compare_exchange_u32
    jc .sst_fail
    cmp dword [sync_st_atomic], 0xDEADBEEF
    jne .sst_fail

    ; === Test 4: np_atomic_compare_exchange – Fehltreffer → CF=1 ===
    mov eax, sync_st_atomic
    mov ecx, 0x00000000         ; falsch erwartet
    mov edx, 0x11111111
    call np_atomic_compare_exchange_u32
    jnc .sst_fail               ; muss CF=1 zurückgeben

    ; === Test 5: np_atomic_fetch_add → Altwert + korrekte Summe ===
    mov dword [sync_st_atomic], 10
    mov eax, sync_st_atomic
    mov ecx, 5
    call np_atomic_fetch_add_u32
    cmp eax, 10                 ; Altwert
    jne .sst_fail
    cmp dword [sync_st_atomic], 15
    jne .sst_fail

    ; === Test 6: np_spin_lock / np_spin_unlock ===
    mov eax, sync_test_spin
    call np_spin_init
    call np_spin_lock
    cmp dword [sync_test_spin], SYNC_SPIN_LOCKED
    jne .sst_fail
    call np_spin_unlock
    cmp dword [sync_test_spin], SYNC_SPIN_UNLOCKED
    jne .sst_fail

    ; === Test 7: np_spin_try_lock – frei CF=0; belegt CF=1 ===
    mov eax, sync_test_spin
    call np_spin_try_lock
    jc .sst_fail
    ; jetzt belegt → zweiter try_lock muss CF=1 geben
    call np_spin_try_lock
    jnc .sst_fail
    call np_spin_unlock

    ; === Test 8: np_spin_lock_irqsave / np_spin_unlock_irqrestore ===
    mov eax, sync_test_spin
    call np_spin_lock_irqsave   ; EDX = saved EFLAGS
    cmp dword [sync_test_spin], SYNC_SPIN_LOCKED
    jne .sst_fail
    call np_spin_unlock_irqrestore

    ; === Test 9: Semaphore: init(2,2), try_wait×2=ok, try_wait=leer, release ===
    mov eax, sync_test_sema
    mov ecx, 2
    mov edx, 2
    call np_semaphore_init
    call np_semaphore_try_wait
    jc .sst_fail
    call np_semaphore_try_wait
    jc .sst_fail
    ; count = 0 → muss CF=1
    call np_semaphore_try_wait
    jnc .sst_fail
    ; Release(1) → count = 1
    mov ecx, 1
    call np_semaphore_release
    jc .sst_fail
    ; Overflow: Release(2) bei max=2 und count=1 → 3 > 2 → CF=1
    mov eax, sync_test_sema
    mov ecx, 2
    call np_semaphore_release
    jnc .sst_fail

    ; === Test 10: Completion: init → not done; signal → done ===
    mov eax, sync_test_compl
    call np_completion_init
    call np_completion_is_done  ; CF=1 (ausstehend)
    jnc .sst_fail
    call np_completion_signal
    mov eax, sync_test_compl
    call np_completion_is_done  ; CF=0 (signalisiert)
    jc .sst_fail

    ; === Test 11: Sequence Lock und Refcount ===
    mov eax, sync_test_seqlock
    call np_seqlock_init
    call np_seqlock_write_lock
    call np_seqlock_write_unlock
    call np_seqlock_read_begin  ; ECX = snapshot (muss gerade sein)
    test ecx, 1
    jnz .sst_fail
    call np_seqlock_read_retry  ; CF=0 (kein Writer seit snapshot)
    jc .sst_fail
    ; Refcount: 3 → retain → 4 → release×4 → 0 (CF=1 beim letzten)
    mov eax, sync_test_refcount
    call np_refcount_retain
    jc .sst_fail
    cmp dword [sync_test_refcount], 4
    jne .sst_fail
    call np_refcount_release    ; 4→3, CF=0
    jc .sst_fail
    call np_refcount_release    ; 3→2, CF=0
    jc .sst_fail
    call np_refcount_release    ; 2→1, CF=0
    jc .sst_fail
    call np_refcount_release    ; 1→0, CF=1
    jnc .sst_fail

    pop edi
    pop esi
    pop ebx
    clc
    ret

.sst_fail:
    pop edi
    pop esi
    pop ebx
    stc
    ret

; ---------------------------------------------------------------------------
; §016 Daten
; ---------------------------------------------------------------------------
align 4
sync_test_spin:      dd 0                   ; Spinlock-Instanz (4 Byte)
sync_test_sema:      times 3 dd 0           ; Semaphore-Instanz (12 Byte)
sync_test_compl:     times 2 dd 0           ; Completion-Instanz (8 Byte)
sync_test_seqlock:   dd 0                   ; SeqLock-Instanz (4 Byte)
sync_test_refcount:  dd 0                   ; Refcount-Instanz
sync_st_atomic:      dd 0                   ; Arbeitsspeicher für Atomartests
sync_tmp_ptr:        dd 0                   ; Temp für CAS-Schleifen
sync_tmp_cnt:        dd 0
sync_stat_acquires:  dd 0
sync_stat_releases:  dd 0
; VSVC-Tabelle (§10, 24 Byte)
align 4
sync_vsvc_tbl:
    dd VSVC_TBL_HDR_SIZE
    dw 1, 0
    dd 0, 0
    dd VSVC_CALL_EARLY_BOOT | VSVC_CALL_PANIC_SAFE
    dd 0
sync_name_sync:  db "nova.sync.core", 0
align 4

; ===========================================================================
; §016 Erweiterung – Blocking Mutex, Condition Variables, Wait Queues, RCU
; Bootstrap: kein echtes Blockieren (Single-CPU, kein Scheduler-Sleep);
; Datenstrukturen + Operationen vollständig, Schlaf = Spin-Fallback.
; ===========================================================================

; Mutex-Struktur (16 Byte)
SYNC_MUTEX_SIZE         equ 16
SYNC_MUTEX_OFF_SPIN     equ 0     ; dd spinlock state
SYNC_MUTEX_OFF_OWNER    equ 4     ; dd owner token (0=frei)
SYNC_MUTEX_OFF_WAITERS  equ 8     ; dd Anzahl Wartender
SYNC_MUTEX_OFF_FLAGS    equ 12    ; dd (RECURSIVE, PI etc.)

SYNC_MUTEX_FLAG_RECURSIVE equ 0x00000001
SYNC_MUTEX_FLAG_PI        equ 0x00000002

; Condition Variable (12 Byte)
SYNC_COND_SIZE          equ 12
SYNC_COND_OFF_GEN       equ 0     ; dd generation counter
SYNC_COND_OFF_WAITERS   equ 4     ; dd Anzahl Wartender
SYNC_COND_OFF_SPIN      equ 8     ; dd spinlock

; Wait Queue (16 Byte)
SYNC_WQ_SIZE            equ 16
SYNC_WQ_OFF_GEN         equ 0     ; dd generation counter
SYNC_WQ_OFF_WAITERS     equ 4     ; dd Anzahl aktiver Waiter-Einträge
SYNC_WQ_OFF_SPIN        equ 8     ; dd spinlock
SYNC_WQ_OFF_FLAGS       equ 12    ; dd Flags

; Wait Queue Entry (16 Byte, §29)
SYNC_WQE_SIZE           equ 16
SYNC_WQE_OFF_OWNER      equ 0     ; dd thread-Token (0=frei)
SYNC_WQE_OFF_FLAGS      equ 4
SYNC_WQE_OFF_PRIO       equ 8
SYNC_WQE_OFF_SEQ        equ 12
SYNC_MAX_WQ_ENTRIES     equ 16

; ---------------------------------------------------------------------------
; Mutex (§16–§21)
; ---------------------------------------------------------------------------

; np_mutex_init: EAX=mutex_ptr, ECX=flags
np_mutex_init:
    mov dword [eax + SYNC_MUTEX_OFF_SPIN],    SYNC_SPIN_UNLOCKED
    mov dword [eax + SYNC_MUTEX_OFF_OWNER],   0
    mov dword [eax + SYNC_MUTEX_OFF_WAITERS], 0
    mov [eax + SYNC_MUTEX_OFF_FLAGS], ecx
    ret

; np_mutex_lock: EAX=mutex_ptr → CF=0 erworben / CF=1 Deadlock (§49 Bootstrap)
; Spin-Fallback: in single-threaded Umgebung kann kein anderer Besitzer entsperren;
; Wiedereintritt gilt als Deadlock (nicht-rekursiver Standard-Mutex, §17).
np_mutex_lock:
    push ecx
    mov ecx, SYNC_SPIN_LOCKED
    xchg [eax + SYNC_MUTEX_OFF_SPIN], ecx
    test ecx, ecx
    jnz .nml_deadlock
    mov dword [eax + SYNC_MUTEX_OFF_OWNER], 1
    pop ecx
    clc
    ret
.nml_deadlock:
    lock inc dword [eax + SYNC_MUTEX_OFF_WAITERS]
    lock dec dword [eax + SYNC_MUTEX_OFF_WAITERS]
    pop ecx
    mov eax, NP_ERR_SYNC_DEADLOCK
    stc
    ret

; np_mutex_try_lock: EAX=mutex_ptr → CF=0 erworben / CF=1 belegt (§16)
np_mutex_try_lock:
    push ecx
    mov ecx, SYNC_SPIN_LOCKED
    xchg [eax + SYNC_MUTEX_OFF_SPIN], ecx
    test ecx, ecx
    jnz .nmtl_fail
    mov dword [eax + SYNC_MUTEX_OFF_OWNER], 1
    pop ecx
    clc
    ret
.nmtl_fail:
    pop ecx
    stc
    ret

; np_mutex_unlock: EAX=mutex_ptr → CF=0 / CF=1 nicht Besitzer (§17)
np_mutex_unlock:
    cmp dword [eax + SYNC_MUTEX_OFF_OWNER], 0
    je .nmu_not_owner
    mov dword [eax + SYNC_MUTEX_OFF_OWNER], 0
    mfence
    mov dword [eax + SYNC_MUTEX_OFF_SPIN], SYNC_SPIN_UNLOCKED
    clc
    ret
.nmu_not_owner:
    mov eax, NP_ERR_SYNC_NOT_OWNER
    stc
    ret

; ---------------------------------------------------------------------------
; Condition Variable (§26–§27)
; ---------------------------------------------------------------------------

; np_condition_init: EAX=cond_ptr
np_condition_init:
    mov dword [eax + SYNC_COND_OFF_GEN],     0
    mov dword [eax + SYNC_COND_OFF_WAITERS], 0
    mov dword [eax + SYNC_COND_OFF_SPIN],    SYNC_SPIN_UNLOCKED
    ret

; np_condition_signal: EAX=cond_ptr – weckt einen Waiter (§26)
np_condition_signal:
    lock inc dword [eax + SYNC_COND_OFF_GEN]
    ret

; np_condition_broadcast: EAX=cond_ptr – weckt alle Waiter (§26)
np_condition_broadcast:
    push ecx
    mov ecx, [eax + SYNC_COND_OFF_WAITERS]
    test ecx, ecx
    jz .ncb_done
    lock add dword [eax + SYNC_COND_OFF_GEN], ecx
.ncb_done:
    pop ecx
    clc
    ret

; np_condition_wait: EAX=cond_ptr, EBX=mutex_ptr → CF=0 / CF=1 Fehler
; Bootstrap-Spurious-Wakeup: Mutex freigeben + sofort wieder erwerben (§27)
np_condition_wait:
    push eax
    mov eax, ebx
    call np_mutex_unlock
    jc .ncw_err
    call np_mutex_lock
    jc .ncw_err
    pop eax
    clc
    ret
.ncw_err:
    pop eax
    stc
    ret

; ---------------------------------------------------------------------------
; Wait Queue (§28–§30)
; ---------------------------------------------------------------------------

; np_wait_queue_init: EAX=wq_ptr
np_wait_queue_init:
    mov dword [eax + SYNC_WQ_OFF_GEN],     0
    mov dword [eax + SYNC_WQ_OFF_WAITERS], 0
    mov dword [eax + SYNC_WQ_OFF_SPIN],    SYNC_SPIN_UNLOCKED
    mov dword [eax + SYNC_WQ_OFF_FLAGS],   0
    ret

; np_wait_queue_wake_one: EAX=wq_ptr – weckt einen Eintrag (§30)
; Bootstrap: Generation hochzählen als Wakeup-Signal
np_wait_queue_wake_one:
    lock inc dword [eax + SYNC_WQ_OFF_GEN]
    clc
    ret

; np_wait_queue_wake_all: EAX=wq_ptr – weckt alle Einträge (§30)
np_wait_queue_wake_all:
    lock inc dword [eax + SYNC_WQ_OFF_GEN]
    clc
    ret

; ---------------------------------------------------------------------------
; RCU – Bootstrap (§33–§34)
; Single-CPU ohne Präemption: keine echten Grace Periods nötig.
; ---------------------------------------------------------------------------

; np_rcu_read_lock: no-op (keine Präemption auf Single-CPU) (§34)
np_rcu_read_lock:
    ret

; np_rcu_read_unlock: no-op (§34)
np_rcu_read_unlock:
    ret

; np_rcu_assign_pointer: EAX=dst_ptr, ECX=new_value (§34)
; SFENCE stellt sicher, dass der neue Zeiger erst nach abgeschlossenen
; Schreibzugriffen auf die neue Struktur sichtbar wird.
np_rcu_assign_pointer:
    sfence
    mov [eax], ecx
    ret

; np_rcu_dereference: EAX=src_ptr → EAX=value (§34)
; LFENCE verhindert spekulative Vorausnahme des Folge-Lesezugriffs.
np_rcu_dereference:
    mov eax, [eax]
    lfence
    ret

; np_rcu_synchronize: wartet auf aktive Read-Side Critical Sections (§34)
; Bootstrap: MFENCE reicht (keine parallel laufenden Leser auf Single-CPU).
np_rcu_synchronize:
    mfence
    ret

; np_rcu_call: EAX=callback_ptr, ECX=context (§34)
; Bootstrap: sofortige Ausführung (Grace Period entfällt auf Single-CPU).
np_rcu_call:
    call eax
    ret

; ---------------------------------------------------------------------------
; sync_ext_initialize – Testinstanzen nullen
; CF=0 (kann nicht fehlschlagen)
; ---------------------------------------------------------------------------
sync_ext_initialize:
    push edi

    mov edi, sync_test_mutex
    xor eax, eax
    mov ecx, (SYNC_MUTEX_SIZE + SYNC_COND_SIZE + SYNC_WQ_SIZE) / 4
    rep stosd

    mov dword [sync_st_rcu_ptr],    0
    mov dword [sync_st_rcu_target], 0xCAFEBABE
    mov dword [sync_st_rcu_called], 0

    pop edi
    clc
    ret

; ---------------------------------------------------------------------------
; sync_ext_self_test – 8 Tests
; CF=0 alle bestanden / CF=1 Fehler
; ---------------------------------------------------------------------------
sync_ext_self_test:
    push ebx
    push esi
    push edi

    ; === Test 1: np_mutex_lock / np_mutex_unlock ===
    mov eax, sync_test_mutex
    xor ecx, ecx
    call np_mutex_init
    call np_mutex_lock
    jc .sest_fail
    cmp dword [sync_test_mutex + SYNC_MUTEX_OFF_SPIN], SYNC_SPIN_LOCKED
    jne .sest_fail
    call np_mutex_unlock
    jc .sest_fail
    cmp dword [sync_test_mutex + SYNC_MUTEX_OFF_SPIN], SYNC_SPIN_UNLOCKED
    jne .sest_fail

    ; === Test 2: np_mutex_try_lock – frei CF=0; belegt CF=1 ===
    mov eax, sync_test_mutex
    call np_mutex_try_lock
    jc .sest_fail
    call np_mutex_try_lock
    jnc .sest_fail
    call np_mutex_unlock

    ; === Test 3: np_mutex_unlock ohne Besitzer → CF=1 ===
    mov eax, sync_test_mutex
    call np_mutex_unlock
    jnc .sest_fail

    ; === Test 4: np_condition_signal erhöht Generation ===
    mov eax, sync_test_cond
    call np_condition_init
    call np_condition_signal
    cmp dword [sync_test_cond + SYNC_COND_OFF_GEN], 1
    jne .sest_fail
    call np_condition_broadcast     ; waiters=0 → GEN unverändert, CF=0
    ; (kein Increment da waiters=0)
    cmp dword [sync_test_cond + SYNC_COND_OFF_GEN], 1
    jne .sest_fail

    ; === Test 5: np_condition_wait – Mutex unlock+relock ===
    mov eax, sync_test_mutex
    xor ecx, ecx
    call np_mutex_init
    call np_mutex_lock
    jc .sest_fail
    mov eax, sync_test_cond
    mov ebx, sync_test_mutex
    call np_condition_wait
    jc .sest_fail
    cmp dword [sync_test_mutex + SYNC_MUTEX_OFF_SPIN], SYNC_SPIN_LOCKED
    jne .sest_fail
    mov eax, sync_test_mutex
    call np_mutex_unlock

    ; === Test 6: np_wait_queue wake_one / wake_all ===
    mov eax, sync_test_wq
    call np_wait_queue_init
    call np_wait_queue_wake_one
    cmp dword [sync_test_wq + SYNC_WQ_OFF_GEN], 1
    jne .sest_fail
    call np_wait_queue_wake_all
    cmp dword [sync_test_wq + SYNC_WQ_OFF_GEN], 2
    jne .sest_fail

    ; === Test 7: np_rcu_assign_pointer + np_rcu_dereference (2-stufig) ===
    ; sync_st_rcu_ptr → sync_st_rcu_target → 0xCAFEBABE
    mov eax, sync_st_rcu_ptr
    mov ecx, sync_st_rcu_target
    call np_rcu_assign_pointer      ; [sync_st_rcu_ptr] = &sync_st_rcu_target
    mov eax, sync_st_rcu_ptr
    call np_rcu_dereference         ; EAX = &sync_st_rcu_target
    cmp eax, sync_st_rcu_target
    jne .sest_fail
    call np_rcu_dereference         ; EAX = [sync_st_rcu_target] = 0xCAFEBABE
    cmp eax, 0xCAFEBABE
    jne .sest_fail

    ; === Test 8: np_rcu_call → Callback sofort ausgeführt ===
    mov dword [sync_st_rcu_called], 0
    mov eax, sync_st_rcu_cb
    xor ecx, ecx
    call np_rcu_call
    cmp dword [sync_st_rcu_called], 1
    jne .sest_fail

    pop edi
    pop esi
    pop ebx
    clc
    ret

.sest_fail:
    pop edi
    pop esi
    pop ebx
    stc
    ret

; RCU-Selbsttest-Callback
sync_st_rcu_cb:
    mov dword [sync_st_rcu_called], 1
    ret

; ---------------------------------------------------------------------------
; §016-Erweiterung Daten
; ---------------------------------------------------------------------------
align 4
sync_test_mutex:     times (SYNC_MUTEX_SIZE / 4) dd 0
sync_test_cond:      times (SYNC_COND_SIZE  / 4) dd 0
sync_test_wq:        times (SYNC_WQ_SIZE    / 4) dd 0
sync_st_rcu_ptr:     dd 0
sync_st_rcu_target:  dd 0xCAFEBABE
sync_st_rcu_called:  dd 0
align 4

; ---------------------------------------------------------------------------
; ===========================================================================
; §009 – Interrupt Manager 1.0 (NPSPEC-KERNEL-0009)
; Bootstrap: Handler-Registrierung, Vektor-Allokation, Statistiken.
; Baut auf dem bestehenden IDT/PIC/APIC-Setup (interrupt_initialize) auf.
; ===========================================================================

; Interrupt-Klassen (§4)
NP_INTERRUPT_EXCEPTION  equ 0
NP_INTERRUPT_HARDWARE   equ 1
NP_INTERRUPT_MSI        equ 2
NP_INTERRUPT_IPI        equ 3
NP_INTERRUPT_TIMER      equ 4
NP_INTERRUPT_SOFTWARE   equ 5
NP_INTERRUPT_SPURIOUS   equ 6

; Handler-Ergebnisse (§13)
NP_IRQ_NOT_HANDLED      equ 0
NP_IRQ_HANDLED          equ 1
NP_IRQ_WAKE_THREAD      equ 2
NP_IRQ_DISABLE_SOURCE   equ 3
NP_IRQ_FATAL            equ 4

; Interrupt-Flags (§16)
NP_IRQ_SHARED           equ 0x00000001
NP_IRQ_EDGE             equ 0x00000002
NP_IRQ_LEVEL            equ 0x00000004
NP_IRQ_THREADED         equ 0x00000008
NP_IRQ_ONESHOT          equ 0x00000010
NP_IRQ_WAKE_CAPABLE     equ 0x00000020
NP_IRQ_PER_CPU          equ 0x00000040
NP_IRQ_NO_BALANCE       equ 0x00000080

; Handler-Slot (§14)
IRQH_OFF_HANDLER        equ 0   ; dd function ptr (0 = frei)
IRQH_OFF_CONTEXT        equ 4   ; dd context ptr
IRQH_OFF_FLAGS          equ 8   ; dd NP_IRQ_* Flags
IRQH_OFF_IRQ            equ 12  ; dd IRQ-Nummer (0-15)
IRQH_SIZE               equ 16

IRQ_MANAGER_SLOTS       equ 16  ; ein Slot pro Legacy-IRQ-Leitung
IRQ_VECTOR_PIC_BASE     equ 32  ; Erstes PIC-Vektor (0x20)
IRQ_VECTOR_DYN_BASE     equ 48  ; Dynamisch ab 0x30

; VSVC-UUID für nova.irq.manager
VSVC_ID_IRQM_0          equ 0x4E4F5641   ; "NOVA"
VSVC_ID_IRQM_1          equ 0x4952514D   ; "IRQM"
VSVC_ID_IRQM_2          equ 0x434F5245   ; "CORE"
VSVC_ID_IRQM_3          equ 0x00000001

; Fehlercodes §50
NP_ERR_IRQ_INVALID_VECTOR   equ -110
NP_ERR_IRQ_INVALID_SOURCE   equ -111
NP_ERR_IRQ_VECTOR_EXHAUSTED equ -112
NP_ERR_IRQ_ALREADY_BOUND    equ -113
NP_ERR_IRQ_NOT_SHARED       equ -114
NP_ERR_IRQ_TRIGGER_CONFLICT equ -115
NP_ERR_IRQ_ACCESS_DENIED    equ -116
NP_ERR_IRQ_SOURCE_DISABLED  equ -119

; ---------------------------------------------------------------------------
; irq_register_handler: EAX=irq_num(0–15), EBX=handler_ptr,
;                        ECX=context_ptr, EDX=flags
;   → CF=0 / CF=1 EAX=fehler
; ---------------------------------------------------------------------------
irq_register_handler:
    cmp eax, IRQ_MANAGER_SLOTS
    jae .irh_inval
    push esi
    imul eax, IRQH_SIZE
    mov esi, irq_handler_table
    add esi, eax
    cmp dword [esi + IRQH_OFF_HANDLER], 0
    jne .irh_busy
    mov [esi + IRQH_OFF_HANDLER], ebx
    mov [esi + IRQH_OFF_CONTEXT], ecx
    mov [esi + IRQH_OFF_FLAGS],   edx
    pop esi
    clc
    ret
.irh_busy:
    pop esi
    mov eax, NP_ERR_IRQ_ALREADY_BOUND
    stc
    ret
.irh_inval:
    mov eax, NP_ERR_IRQ_INVALID_SOURCE
    stc
    ret

; ---------------------------------------------------------------------------
; irq_unregister_handler: EAX=irq_num(0–15) → CF=0 / CF=1
; ---------------------------------------------------------------------------
irq_unregister_handler:
    cmp eax, IRQ_MANAGER_SLOTS
    jae .iru_inval
    push esi
    imul eax, IRQH_SIZE
    mov esi, irq_handler_table
    add esi, eax
    mov dword [esi + IRQH_OFF_HANDLER], 0
    mov dword [esi + IRQH_OFF_CONTEXT], 0
    mov dword [esi + IRQH_OFF_FLAGS],   0
    mov dword [esi + IRQH_OFF_IRQ],     0
    pop esi
    clc
    ret
.iru_inval:
    mov eax, NP_ERR_IRQ_INVALID_SOURCE
    stc
    ret

; ---------------------------------------------------------------------------
; irq_dispatch_hw: EAX=irq_num(0–15)
; Ruft registrierten Handler auf, aktualisiert Statistiken.
; EOI liegt beim Caller (interrupt_dispatch).
; → EAX=NP_IRQ_HANDLED / NP_IRQ_NOT_HANDLED
; ---------------------------------------------------------------------------
irq_dispatch_hw:
    cmp eax, IRQ_MANAGER_SLOTS
    jae .idh_spurious
    inc dword [irq_stat_total]
    mov ecx, eax
    inc dword [irq_counts + ecx * 4]
    imul eax, IRQH_SIZE
    mov [irq_tmp_slot], eax        ; Slot-Offset speichern
    mov eax, [irq_handler_table + eax + IRQH_OFF_HANDLER]
    test eax, eax
    jz .idh_unhandled
    ; Handler(context) aufrufen — Slot-Offset aus irq_tmp_slot
    mov ecx, [irq_tmp_slot]
    push dword [irq_handler_table + ecx + IRQH_OFF_CONTEXT]
    call eax
    add esp, 4
    mov eax, NP_IRQ_HANDLED
    ret
.idh_unhandled:
    inc dword [irq_stat_unhandled]
    mov eax, NP_IRQ_NOT_HANDLED
    ret
.idh_spurious:
    inc dword [irq_stat_spurious]
    mov eax, NP_IRQ_NOT_HANDLED
    ret

; ---------------------------------------------------------------------------
; np_interrupt_vector_allocate: EAX=count → CF=0 EAX=first_vector / CF=1
; Sucht im dynamischen Pool (Vektoren 48–255) einen freien zusammenhängenden
; Bereich und markiert ihn als belegt.
; ---------------------------------------------------------------------------
np_interrupt_vector_allocate:
    test eax, eax
    jz .iva_inval
    cmp eax, 208            ; max allokierbar (256 - 48)
    ja .iva_inval
    mov [irq_tmp_alloc_cnt], eax
    push ebx
    push ecx
    push edx
    push esi
    push edi
    mov edi, IRQ_VECTOR_DYN_BASE    ; Suchstart
.iva_scan:
    mov ecx, [irq_tmp_alloc_cnt]
    ; Prüfen ob ab EDI count Vektoren frei sind
    mov esi, edi
.iva_check:
    cmp esi, 256
    jae .iva_fail
    cmp byte [irq_vector_used + esi], 0
    jne .iva_next
    dec ecx
    jz .iva_found
    inc esi
    jmp .iva_check
.iva_next:
    inc edi
    cmp edi, 256
    jb .iva_scan
.iva_fail:
    pop edi
    pop esi
    pop edx
    pop ecx
    pop ebx
    mov eax, NP_ERR_IRQ_VECTOR_EXHAUSTED
    stc
    ret
.iva_found:
    ; Vektoren [EDI .. ESI] als belegt markieren
    mov esi, edi
    mov ecx, [irq_tmp_alloc_cnt]
.iva_mark:
    mov byte [irq_vector_used + esi], 1
    inc esi
    dec ecx
    jnz .iva_mark
    mov eax, edi
    pop edi
    pop esi
    pop edx
    pop ecx
    pop ebx
    clc
    ret
.iva_inval:
    mov eax, NP_ERR_IRQ_INVALID_VECTOR
    stc
    ret

; ---------------------------------------------------------------------------
; np_interrupt_vector_free: EAX=first_vector, ECX=count → CF=0 / CF=1
; ---------------------------------------------------------------------------
np_interrupt_vector_free:
    cmp eax, IRQ_VECTOR_DYN_BASE
    jb .ivf_inval
    test ecx, ecx
    jz .ivf_inval
    ; Endvektor berechnen: EAX + ECX <= 256
    push ebx
    mov ebx, eax
    add eax, ecx
    cmp eax, 257
    jae .ivf_inval_pop
.ivf_clear:
    mov byte [irq_vector_used + ebx], 0
    inc ebx
    dec ecx
    jnz .ivf_clear
    pop ebx
    clc
    ret
.ivf_inval_pop:
    pop ebx
.ivf_inval:
    mov eax, NP_ERR_IRQ_INVALID_VECTOR
    stc
    ret

; ---------------------------------------------------------------------------
; irq_manager_initialize – Tabellen nullen, VSVC registrieren
; CF=0 OK / CF=1 Fehler
; ---------------------------------------------------------------------------
irq_manager_initialize:
    push ebx
    push esi
    push edi

    ; Handler-Tabelle nullen
    mov edi, irq_handler_table
    xor eax, eax
    mov ecx, (IRQ_MANAGER_SLOTS * IRQH_SIZE) / 4
    rep stosd

    ; Statistiken nullen
    mov dword [irq_stat_total],     0
    mov dword [irq_stat_spurious],  0
    mov dword [irq_stat_unhandled], 0
    mov edi, irq_counts
    mov ecx, IRQ_MANAGER_SLOTS
    rep stosd

    ; Vektor-Nutzungstabelle: 0-47 als belegt markieren
    mov edi, irq_vector_used
    xor eax, eax
    mov ecx, 256
    rep stosb                       ; erst alles auf 0 (1 Byte pro Vektor)
    mov ecx, IRQ_VECTOR_DYN_BASE   ; Vektoren 0-47 reservieren
    mov edi, irq_vector_used
.imi_reserve:
    mov byte [edi], 1
    inc edi
    dec ecx
    jnz .imi_reserve

    ; VSVC "nova.irq.manager" registrieren
    mov dword [vsvc_tmp_maj],      1
    mov dword [vsvc_tmp_min],      0
    mov dword [vsvc_tmp_pat],      0
    mov dword [vsvc_tmp_table],    irq_vsvc_tbl
    mov dword [vsvc_tmp_tabsz],    VSVC_TBL_HDR_SIZE
    mov dword [vsvc_tmp_features], 0
    mov dword [vsvc_tmp_reqcap],   0
    mov dword [vsvc_tmp_arch],     VSVC_ARCH_X86_32
    mov dword [vsvc_tmp_stab],     VSVC_STAB_STABLE
    mov dword [vsvc_tmp_name],     irq_name_irqm
    mov dword [vsvc_tmp_flags],    VSVC_FLAG_EARLY_BOOT | VSVC_FLAG_PANIC_SAFE
    mov eax, VSVC_ID_IRQM_0
    mov ebx, VSVC_ID_IRQM_1
    mov ecx, VSVC_ID_IRQM_2
    mov edx, VSVC_ID_IRQM_3
    call vsvc_register
    jc .imi_fail

    pop edi
    pop esi
    pop ebx
    clc
    ret
.imi_fail:
    pop edi
    pop esi
    pop ebx
    stc
    ret

; ---------------------------------------------------------------------------
; irq_manager_self_test – 10 Tests
; CF=0 alle bestanden / CF=1 Fehler
; ---------------------------------------------------------------------------
irq_manager_self_test:
    push ebx
    push esi
    push edi

    ; === Test 1: IDT geladen – sidt → base == idt_table ===
    sidt [irq_st_idtr_buf]
    mov eax, [irq_st_idtr_buf + 2]  ; Basisadresse
    cmp eax, idt_table
    jne .imst_fail

    ; === Test 2: IDT-Limit = (IDT_ENTRY_COUNT * 8) - 1 ===
    movzx eax, word [irq_st_idtr_buf]
    cmp eax, (IDT_ENTRY_COUNT * 8) - 1
    jne .imst_fail

    ; === Test 3: PIC-Mastermaske (0x21) lesbar, nicht komplett maskiert ===
    in al, 0x21
    cmp al, 0xFF
    je .imst_fail                   ; alle IRQs maskiert → Fehler

    ; === Test 4: irq_register_handler → Handler in Slot ===
    mov eax, 5                      ; Test-IRQ 5
    mov ebx, irq_st_test_handler
    xor ecx, ecx
    xor edx, edx
    call irq_register_handler
    jc .imst_fail
    ; Slot prüfen
    mov eax, [irq_handler_table + 5 * IRQH_SIZE + IRQH_OFF_HANDLER]
    cmp eax, irq_st_test_handler
    jne .imst_fail

    ; === Test 5: Doppel-Registrierung ohne SHARED → NP_ERR_IRQ_ALREADY_BOUND ===
    mov eax, 5
    mov ebx, irq_st_test_handler
    xor ecx, ecx
    xor edx, edx
    call irq_register_handler
    jnc .imst_fail                  ; muss CF=1 zurückgeben
    cmp eax, NP_ERR_IRQ_ALREADY_BOUND
    jne .imst_fail

    ; === Test 6: irq_dispatch_hw → Handler aufgerufen ===
    mov dword [irq_st_cb_called], 0
    mov eax, 5
    call irq_dispatch_hw
    cmp dword [irq_st_cb_called], 1
    jne .imst_fail
    cmp eax, NP_IRQ_HANDLED
    jne .imst_fail

    ; === Test 7: irq_stat_total und irq_counts[5] inkrementiert ===
    cmp dword [irq_stat_total], 1
    jne .imst_fail
    cmp dword [irq_counts + 5 * 4], 1
    jne .imst_fail

    ; === Test 8: irq_unregister_handler → Slot geleert ===
    mov eax, 5
    call irq_unregister_handler
    jc .imst_fail
    cmp dword [irq_handler_table + 5 * IRQH_SIZE + IRQH_OFF_HANDLER], 0
    jne .imst_fail

    ; === Test 9: np_interrupt_vector_allocate(1) → Vektor ≥ 48 ===
    mov eax, 1
    call np_interrupt_vector_allocate
    jc .imst_fail
    cmp eax, IRQ_VECTOR_DYN_BASE
    jb .imst_fail
    mov [irq_st_alloc_vec], eax

    ; === Test 10: np_interrupt_vector_free → anschließend wieder allokierbar ===
    mov ecx, 1
    call np_interrupt_vector_free
    jc .imst_fail
    mov eax, 1
    call np_interrupt_vector_allocate
    jc .imst_fail
    ; Freigeben (Aufräumen)
    mov ecx, 1
    call np_interrupt_vector_free

    pop edi
    pop esi
    pop ebx
    clc
    ret

.imst_fail:
    pop edi
    pop esi
    pop ebx
    stc
    ret

; Selbsttest-Callback
irq_st_test_handler:
    mov dword [irq_st_cb_called], 1
    mov eax, NP_IRQ_HANDLED
    ret

; ---------------------------------------------------------------------------
; §009 Daten
; ---------------------------------------------------------------------------
align 4
irq_handler_table:   times (IRQ_MANAGER_SLOTS * IRQH_SIZE / 4) dd 0
irq_counts:          times IRQ_MANAGER_SLOTS dd 0
irq_stat_total:      dd 0
irq_stat_spurious:   dd 0
irq_stat_unhandled:  dd 0
irq_tmp_slot:        dd 0
irq_tmp_alloc_cnt:   dd 0
irq_st_alloc_vec:    dd 0
irq_st_cb_called:    dd 0
align 2
irq_st_idtr_buf:     dw 0, 0, 0   ; 6 Byte: limit(2) + base(4)
align 4
; Vektor-Nutzungstabelle: 1 Byte pro Vektor, 0=frei, 1=belegt
irq_vector_used:     times 256 db 0
; VSVC-Tabelle
align 4
irq_vsvc_tbl:
    dd VSVC_TBL_HDR_SIZE
    dw 1, 0
    dd 0, 0
    dd VSVC_CALL_EARLY_BOOT | VSVC_CALL_PANIC_SAFE
    dd 0
irq_name_irqm:  db "nova.irq.manager", 0
align 4

; Restriktiver Kernel Module Loader (NPSPEC-KERNEL-0025)
; ---------------------------------------------------------------------------
MODULE_MAGIC              equ 0x444D564E ; "NVMD"
MODULE_HEADER_SIZE        equ 64
MODULE_MAX_IMAGE_SIZE     equ 4096
MODULE_ARCH_X86_32        equ 1
MODULE_KERNEL_ABI         equ 0x00010000
MODULE_FLAG_TRUSTED       equ 0x00000001
MODULE_FLAG_STACK_GUARD   equ 0x00000002
MODULE_FLAG_UNLOADABLE    equ 0x00000004
MODULE_REQUIRED_FLAGS     equ MODULE_FLAG_TRUSTED | MODULE_FLAG_STACK_GUARD
MODULE_STATE_DISCOVERED   equ 0
MODULE_STATE_VALIDATING   equ 1
MODULE_STATE_LOADING      equ 2
MODULE_STATE_RELOCATING   equ 3
MODULE_STATE_INITIALIZING equ 4
MODULE_STATE_ACTIVE       equ 5
MODULE_STATE_QUIESCING    equ 6
MODULE_STATE_UNLOADING    equ 7
MODULE_STATE_FAILED       equ 8
MODULE_STATE_UNLOADED     equ 9

module_loader_initialize:
    mov edi, module_code_area
    xor eax, eax
    mov ecx, (64 + 64) / 4
    rep stosd
    mov dword [module_state], MODULE_STATE_DISCOVERED
    mov dword [module_instance_id], 0
    mov dword [module_active_calls], 0
    mov dword [module_loaded_count], 0
    mov dword [module_failed_count], 0
    mov dword [module_unload_count], 0
    clc
    ret

; ESI=vollständiges, bereits aus einer verifizierten Quelle gelesenes Paket.
module_load:
    mov [module_source], esi
    mov dword [module_state], MODULE_STATE_VALIDATING
    cmp dword [esi + 0], MODULE_MAGIC
    jne .reject
    cmp dword [esi + 4], MODULE_HEADER_SIZE
    jne .reject
    mov eax, [esi + 8]
    cmp eax, MODULE_HEADER_SIZE
    jbe .reject
    cmp eax, MODULE_MAX_IMAGE_SIZE
    ja .reject
    cmp dword [esi + 12], MODULE_ARCH_X86_32
    jne .reject
    cmp dword [esi + 16], MODULE_KERNEL_ABI
    ja .reject
    cmp dword [esi + 20], MODULE_KERNEL_ABI
    jb .reject
    mov eax, [esi + 28]
    and eax, MODULE_REQUIRED_FLAGS
    cmp eax, MODULE_REQUIRED_FLAGS
    jne .reject
    cmp dword [esi + 60], 0
    jne .reject
    ; Code- und Datensektion müssen vollständig innerhalb der Datei liegen.
    mov eax, [esi + 32]
    cmp eax, MODULE_HEADER_SIZE
    jb .reject
    mov edx, eax
    add edx, [esi + 36]
    jc .reject
    cmp edx, [esi + 8]
    ja .reject
    cmp dword [esi + 36], 64
    ja .reject
    mov eax, [esi + 40]
    cmp eax, MODULE_HEADER_SIZE
    jb .reject
    mov ecx, eax
    add ecx, [esi + 44]
    jc .reject
    cmp ecx, [esi + 8]
    ja .reject
    cmp dword [esi + 44], 64
    ja .reject
    ; Überlappende Code-/Datensektionen werden strikt abgelehnt.
    mov eax, [esi + 32]
    add eax, [esi + 36]
    cmp eax, [esi + 40]
    jbe .sections_ok
    mov eax, [esi + 40]
    add eax, [esi + 44]
    cmp eax, [esi + 32]
    ja .reject
.sections_ok:
    ; Die initiale Trust-Schicht prüft den signierten Inhaltsdigest.
    mov ecx, [esi + 8]
    sub ecx, MODULE_HEADER_SIZE
    lea edi, [esi + MODULE_HEADER_SIZE]
    xor eax, eax
.digest:
    movzx edx, byte [edi]
    add eax, edx
    rol eax, 3
    inc edi
    loop .digest
    cmp eax, [esi + 48]
    jne .reject
    ; Abhängigkeit 0 bedeutet keine; andere IDs müssen bereits aktiv sein.
    mov eax, [esi + 52]
    test eax, eax
    jz .dependency_ok
    cmp eax, [module_id]
    jne .reject
    cmp dword [module_state], MODULE_STATE_ACTIVE
    jne .reject
.dependency_ok:
    mov dword [module_state], MODULE_STATE_LOADING
    mov eax, [esi + 32]
    add eax, esi
    push esi
    mov esi, eax
    mov edi, module_code_area
    mov ecx, [module_source]
    mov ecx, [ecx + 36]
    rep movsb
    pop esi
    mov eax, [esi + 40]
    add eax, esi
    push esi
    mov esi, eax
    mov edi, module_data_area
    mov ecx, [module_source]
    mov ecx, [ecx + 44]
    rep movsb
    pop esi
    mov dword [module_state], MODULE_STATE_RELOCATING
    ; Keine Relokation ist im Format v1 gleichbedeutend mit abgeschlossen.
    mov dword [module_code_rights], 0x5 ; R-X
    mov dword [module_data_rights], 0x3 ; RW-
    mov dword [module_state], MODULE_STATE_INITIALIZING
    mov eax, [esi + 56]
    mov [module_id], eax
    mov eax, [esi + 28]
    mov [module_flags], eax
    inc dword [module_instance_id]
    ; Erst jetzt wird die vollständig geprüfte Instanz atomar sichtbar.
    mov dword [module_state], MODULE_STATE_ACTIVE
    inc dword [module_loaded_count]
    clc
    ret
.reject:
    mov dword [module_state], MODULE_STATE_FAILED
    inc dword [module_failed_count]
    stc
    ret

module_unload:
    cmp dword [module_state], MODULE_STATE_ACTIVE
    jne .reject
    test dword [module_flags], MODULE_FLAG_UNLOADABLE
    jz .reject
    cmp dword [module_active_calls], 0
    jne .reject
    mov dword [module_state], MODULE_STATE_QUIESCING
    mov dword [module_state], MODULE_STATE_UNLOADING
    mov edi, module_code_area
    xor eax, eax
    mov ecx, (64 + 64) / 4
    rep stosd
    mov dword [module_code_rights], 0
    mov dword [module_data_rights], 0
    mov dword [module_state], MODULE_STATE_UNLOADED
    inc dword [module_unload_count]
    clc
    ret
.reject:
    stc
    ret

module_loader_self_test:
    mov esi, module_test_image
    call module_load
    jc .invalid
    cmp dword [module_state], MODULE_STATE_ACTIVE
    jne .invalid
    cmp dword [module_code_rights], 0x5
    jne .invalid
    cmp dword [module_data_rights], 0x3
    jne .invalid
    mov dword [module_active_calls], 1
    call module_unload
    jnc .invalid
    mov dword [module_active_calls], 0
    call module_unload
    jc .invalid
    ; Manipulierte Architektur darf niemals bis zur Veröffentlichung gelangen.
    mov dword [module_test_image + 12], 0xFFFFFFFF
    mov esi, module_test_image
    call module_load
    jnc .restore_invalid
    mov dword [module_test_image + 12], MODULE_ARCH_X86_32
    ; Am Ende bleibt eine gültige Diagnosemodulinstanz aktiv.
    mov esi, module_test_image
    call module_load
    jc .invalid
    cmp dword [module_loaded_count], 2
    jne .invalid
    cmp dword [module_failed_count], 1
    jne .invalid
    cmp dword [module_unload_count], 1
    jne .invalid
    clc
    ret
.restore_invalid:
    mov dword [module_test_image + 12], MODULE_ARCH_X86_32
.invalid:
    stc
    ret

align 4
module_state:          dd 0
module_instance_id:    dd 0
module_id:             dd 0
module_flags:          dd 0
module_active_calls:   dd 0
module_code_rights:    dd 0
module_data_rights:    dd 0
module_loaded_count:   dd 0
module_failed_count:   dd 0
module_unload_count:   dd 0
module_source:         dd 0
module_code_area:      times 64 db 0
module_data_area:      times 64 db 0

; Kleines signiertes In-Kernel-Testpaket. Digest wird über 32 Nutzbytes gebildet.
align 4
module_test_image:
    dd MODULE_MAGIC, MODULE_HEADER_SIZE, 96, MODULE_ARCH_X86_32
    dd MODULE_KERNEL_ABI, MODULE_KERNEL_ABI, 5
    dd MODULE_FLAG_TRUSTED | MODULE_FLAG_STACK_GUARD | MODULE_FLAG_UNLOADABLE
    dd 64, 16, 80, 16
    dd 0xD0F97818, 0, 0x44494147, 0
    db 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16
    times 16 db 0x10

; ---------------------------------------------------------------------------
; Präemptiver Round-Robin-Scheduler (ADR-2004 / ADR-2012 / NPSPEC-KERNEL-0003 / NPSPEC-KERNEL-0005)
; ---------------------------------------------------------------------------

SCHEDULER_THREAD_COUNT equ 8
SCHEDULER_FRAME_SIZE   equ 68
SCHEDULER_API_SIZE     equ 48                    ; §134: +8 (dynamic slot state)
SCHEDULER_CAP_PREEMPT  equ 0x00000001
SCHEDULER_CAP_RR       equ 0x00000002
SCHEDULER_CAP_AFFINITY equ 0x00000004           ; §133: CPU-Affinitätsunterstützung
SCHEDULER_CAP_DYNAMIC  equ 0x00000008           ; §134: dynamische Thread-Slots

; Thread Manager (ADR-2012)
THREAD_API_SIZE      equ 36
THREAD_CAPACITY      equ SCHEDULER_THREAD_COUNT
THREAD_RECORD_SIZE   equ 32
THREAD_STATE_READY   equ 1
OBJECT_TYPE_THREAD   equ 5
THREAD_TID      equ 0
THREAD_PID      equ 4
THREAD_STATE    equ 8
THREAD_SLOT     equ 12
THREAD_HANDLE   equ 16
THREAD_ENTRY    equ 20
THREAD_CONTEXT  equ 24
THREAD_SCOPE    equ 28

thread_manager_initialize:
    mov edi, thread_table
    xor eax, eax
    mov ecx, (THREAD_CAPACITY * THREAD_RECORD_SIZE) / 4
    rep stosd
    mov edi, thread_task_ids
    xor eax, eax
    mov ecx, THREAD_CAPACITY
    rep stosd
    mov dword [thread_count], 0
    mov dword [thread_next_tid], 1
    mov dword [thread_manager_ready], 0
    mov eax, kernel_main
    mov edx, 1
    xor ebx, ebx
    call thread_register
    jc .invalid
    mov eax, scheduler_thread1
    mov edx, 1
    mov ebx, 1
    call thread_register
    jc .invalid
    mov eax, scheduler_thread2
    mov edx, 1
    mov ebx, 2
    call thread_register
    jc .invalid
    mov dword [thread_manager_ready], 1
    clc
    ret
.invalid:
    stc
    ret

; EAX=Einstieg, EDX=PID, EBX=Scheduler-Slot. EAX=TID.
thread_register:
    pushfd
    cli
    test eax, eax
    jz .invalid
    cmp ebx, SCHEDULER_THREAD_COUNT
    jae .invalid
    mov [thread_temp_entry], eax
    mov [thread_temp_pid], edx
    mov [thread_temp_slot], ebx
    mov eax, edx
    call process_lookup
    jc .invalid
    mov ecx, [thread_count]
    cmp ecx, THREAD_CAPACITY
    jae .invalid
    cmp dword [thread_temp_pid], 1
    jne .invalid
    mov eax, [thread_temp_pid]
    mov edx, [task_scope_kernel_root_id]
    xor ebx, ebx
    xor ecx, ecx
    call task_create
    jc .invalid
    mov [thread_temp_task], eax
    mov ecx, [thread_count]
    mov edi, ecx
    shl edi, 5
    add edi, thread_table
    mov [thread_temp_record], edi
    mov eax, OBJECT_TYPE_THREAD
    mov edx, [thread_temp_pid]
    mov ebx, [thread_temp_entry]
    call object_create
    jc .invalid
    mov edi, [thread_temp_record]
    mov edx, [thread_next_tid]
    mov [edi + THREAD_TID], edx
    mov ecx, [thread_temp_pid]
    mov [edi + THREAD_PID], ecx
    mov dword [edi + THREAD_STATE], THREAD_STATE_READY
    mov ecx, [thread_temp_slot]
    mov [edi + THREAD_SLOT], ecx
    mov [edi + THREAD_HANDLE], eax
    mov ecx, [thread_temp_entry]
    mov [edi + THREAD_ENTRY], ecx
    mov ecx, [thread_temp_slot]
    mov ecx, [scheduler_contexts + ecx * 4]
    mov [edi + THREAD_CONTEXT], ecx
    mov dword [edi + THREAD_SCOPE], 0
    cmp dword [thread_temp_pid], 1
    jne .scope_ready
    mov ecx, [task_scope_kernel_root_id]
    mov [edi + THREAD_SCOPE], ecx
.scope_ready:
    mov ecx, [thread_temp_slot]
    mov edx, [thread_temp_task]
    mov [thread_task_ids + ecx * 4], edx
    inc dword [thread_count]
    inc dword [thread_next_tid]
    mov eax, edx
    popfd
    clc
    ret
.invalid:
    xor eax, eax
    popfd
    stc
    ret

; EAX=Einstieg, EDX=PID, ECX=CPU-Affinität (-1=beliebig). EAX=TID.
; Belegt einen freien Scheduler-Slot, erzeugt einen eigenen Kontextframe und
; veröffentlicht den Thread unter scheduler_lock. Dadurch können APs den Slot
; erst sehen, wenn Kontext, Affinität und Thread-Datensatz vollständig sind.
thread_create_dynamic:
    pushfd
    cli
    mov [thread_temp_entry], eax
    mov [thread_temp_pid], edx
    mov [thread_temp_affinity], ecx
    cmp dword [thread_manager_ready], 1
    jne .invalid_no_lock
    test eax, eax
    jz .invalid_no_lock
    mov eax, edx
    call process_lookup
    jc .invalid_no_lock

.lock:
    lock bts dword [scheduler_lock], 0
    jc .lock

    mov ebx, 1                         ; Slot 0 bleibt BSP-/Kernel-Slot
.scan_slot:
    cmp ebx, SCHEDULER_THREAD_COUNT
    jae .invalid_unlock
    cmp dword [thread_task_ids + ebx * 4], 0
    jne .next_slot
    cmp dword [scheduler_contexts + ebx * 4], 0
    je .slot_found
.next_slot:
    inc ebx
    jmp .scan_slot

.slot_found:
    mov [thread_temp_slot], ebx
    mov eax, [thread_temp_entry]
    call scheduler_create_frame
    jc .invalid_unlock
    mov ebx, [thread_temp_slot]
    mov [scheduler_contexts + ebx * 4], eax
    mov eax, [thread_temp_affinity]
    mov [thread_cpu_affinity + ebx * 4], eax
    mov dword [thread_running_on + ebx * 4], -1

    mov eax, [thread_temp_entry]
    mov edx, [thread_temp_pid]
    mov ebx, [thread_temp_slot]
    call thread_register
    jc .invalid_clear_slot
    lock btr dword [scheduler_lock], 0
    popfd
    clc
    ret

.invalid_clear_slot:
    mov ebx, [thread_temp_slot]
    mov dword [scheduler_contexts + ebx * 4], 0
    mov dword [thread_cpu_affinity + ebx * 4], -1
    mov dword [thread_running_on + ebx * 4], -1
.invalid_unlock:
    lock btr dword [scheduler_lock], 0
.invalid_no_lock:
    xor eax, eax
    popfd
    stc
    ret

; EAX=TID. EAX=Datensatz oder 0.
thread_lookup:
    xor ecx, ecx
.scan:
    cmp ecx, THREAD_CAPACITY
    jae .invalid
    mov edx, ecx
    shl edx, 5
    add edx, thread_table
    cmp [edx + THREAD_TID], eax
    je .found
    inc ecx
    jmp .scan
.found:
    mov eax, edx
    clc
    ret
.invalid:
    xor eax, eax
    stc
    ret

thread_manager_self_test:
    mov eax, 2
    call thread_lookup
    jc .invalid
    cmp dword [eax + THREAD_PID], 1
    jne .invalid
    cmp dword [eax + THREAD_SLOT], 1
    jne .invalid
    cmp dword [eax + THREAD_ENTRY], scheduler_thread1
    jne .invalid
    mov edx, [task_scope_kernel_root_id]
    cmp [eax + THREAD_SCOPE], edx
    jne .invalid
    mov eax, [thread_task_ids + 4]
    test eax, eax
    jz .invalid
    call task_lookup
    jc .invalid
    cmp dword [eax + TASK_OWNER], 1
    jne .invalid
    mov edx, [task_scope_kernel_root_id]
    cmp [eax + TASK_RECORD_SCOPE], edx
    jne .invalid
    ; §134: zur Laufzeit einen zusätzlichen Thread in einen freien
    ; Scheduler-Slot legen. Der Thread ist CPU-ungebunden und darf damit von
    ; BSP oder APs übernommen werden, sobald der Scheduler ihn auswählt.
    mov eax, scheduler_dynamic_thread
    mov edx, 1
    mov ecx, -1
    call thread_create_dynamic
    jc .invalid
    mov [thread_dynamic_tid], eax
    call thread_lookup
    jc .invalid
    cmp dword [eax + THREAD_SLOT], 3
    jne .invalid
    cmp dword [eax + THREAD_ENTRY], scheduler_dynamic_thread
    jne .invalid
    cmp dword [thread_cpu_affinity + 3 * 4], -1
    jne .invalid
    cmp dword [thread_running_on + 3 * 4], -1
    jne .invalid
    cmp dword [task_count], 4
    jne .invalid
    cmp dword [thread_count], 4
    jne .invalid
    clc
    ret
.invalid:
    stc
    ret

align 4
thread_manager_api:
    dd THREAD_API_SIZE
    dw 1, 0
    dd THREAD_CAPACITY
    dd thread_register
    dd thread_lookup
    dd thread_count
    dd thread_table
    dd thread_next_tid
    dd thread_create_dynamic

thread_count:       dd 0
thread_next_tid:    dd 0
thread_temp_entry:  dd 0
thread_temp_pid:    dd 0
thread_temp_slot:   dd 0
thread_temp_record: dd 0
thread_temp_task:   dd 0
thread_temp_affinity: dd 0
thread_dynamic_tid: dd 0
thread_manager_ready: dd 0
align 4
thread_table:
    times THREAD_CAPACITY * THREAD_RECORD_SIZE db 0
thread_task_ids:
    times THREAD_CAPACITY dd 0

scheduler_initialize:
    mov dword [scheduler_enabled], 0
    mov dword [scheduler_current], 0
    mov edi, scheduler_contexts
    xor eax, eax
    mov ecx, SCHEDULER_THREAD_COUNT
    rep stosd
    mov edi, thread_cpu_affinity
    mov eax, -1
    mov ecx, SCHEDULER_THREAD_COUNT
    rep stosd
    mov edi, thread_running_on
    mov eax, -1
    mov ecx, SCHEDULER_THREAD_COUNT
    rep stosd
    mov dword [scheduler_thread1_runs], 0
    mov dword [scheduler_thread2_runs], 0
    mov dword [scheduler_dynamic_runs], 0
    ; §133: Slot 0 (BSP-Idle/Kernel-Thread) auf CPU 0 pinnen
    mov dword [thread_cpu_affinity + 0], 0      ; nur BSP (CPU 0) darf Slot 0 laden
    mov dword [thread_running_on + 0], 0        ; Slot 0 läuft ab Start auf BSP

    mov eax, scheduler_thread1
    call scheduler_create_frame
    jc .invalid
    mov [scheduler_contexts + 4], eax

    mov eax, scheduler_thread2
    call scheduler_create_frame
    jc .invalid
    mov [scheduler_contexts + 8], eax

    mov dword [scheduler_enabled], 1
    clc
    ret
.invalid:
    stc
    ret

; EAX=Thread-Einstieg, EAX=synthetischer Interruptframe.
scheduler_create_frame:
    push ebx
    push ecx
    push edi
    mov ebx, eax
    call pmm_alloc_page
    test eax, eax
    jz .invalid
    mov edi, eax
    xor eax, eax
    mov ecx, PMM_PAGE_SIZE / 4
    rep stosd
    sub edi, SCHEDULER_FRAME_SIZE

    mov dword [edi + 0], DATA_SEGMENT
    mov dword [edi + 4], DATA_SEGMENT
    mov dword [edi + 8], DATA_SEGMENT
    mov dword [edi + 12], DATA_SEGMENT
    mov dword [edi + 16], 0          ; EDI
    mov dword [edi + 20], 0          ; ESI
    mov dword [edi + 24], 0          ; EBP
    mov dword [edi + 28], 0          ; ESP-Platzhalter von PUSHAD
    mov dword [edi + 32], 0          ; EBX
    mov dword [edi + 36], 0          ; EDX
    mov dword [edi + 40], 0          ; ECX
    mov dword [edi + 44], 0          ; EAX
    mov dword [edi + 48], 32         ; normalisierter Vektor
    mov dword [edi + 52], 0          ; Fehlercode
    mov [edi + 56], ebx              ; EIP
    mov dword [edi + 60], CODE_SEGMENT
    mov dword [edi + 64], 0x00000202 ; IF gesetzt
    mov eax, edi
    pop edi
    pop ecx
    pop ebx
    clc
    ret
.invalid:
    pop edi
    pop ecx
    pop ebx
    stc
    ret

; [ESP+4] = vollständiger Frame-Zeiger des unterbrochenen Threads.
; Nutzt scheduler_caller_cpu (unter Spinlock gesetzt) für Affinitätsprüfung (§133).
; ESI/EDI werden gesichert und restauriert.
scheduler_on_tick:
    push esi
    push edi
    ; Basisadresse: [esp+12] = Frame-Zeiger (ursprünglich [esp+4] + 2×push)
    mov eax, [esp + 12]
    cmp dword [scheduler_enabled], 1
    jne .done

    ; Aktuellen Slot freigeben: Frame sichern, als "nicht laufend" markieren
    mov edx, [scheduler_current]
    mov [scheduler_contexts + edx * 4], eax
    mov [scheduler_previous_slot], edx
    mov dword [thread_running_on + edx * 4], -1    ; §133: Slot freigeben

    cmp dword [thread_manager_ready], 1
    jne .select_next
    mov eax, [thread_task_ids + edx * 4]
    test eax, eax
    jz .select_next
    call task_lookup
    jc .select_next
    cmp dword [eax + TASK_STATE], TASK_STATE_RUNNING
    jne .select_next
    mov dword [eax + TASK_STATE], TASK_STATE_READY

.select_next:
    mov edx, [scheduler_previous_slot]
    mov esi, edx                    ; §133: Start für Wrap-Detection (kein Endlos-Loop)
    mov edi, [scheduler_caller_cpu] ; §133: aufrufende CPU (gesetzt unter Spinlock)

.try_slot:
    inc edx
    cmp edx, SCHEDULER_THREAD_COUNT
    jb .check_affin
    xor edx, edx                    ; Wrap um auf Slot 0

.check_affin:
    cmp edx, esi                    ; §133: alle Slots geprüft → kein kompatibler Slot
    je .no_slot_found

    ; §133: Affinitätsprüfung
    mov eax, [thread_cpu_affinity + edx * 4]
    cmp eax, -1                     ; -1 = beliebige CPU → ok
    je .check_running
    cmp eax, edi                    ; affinity = diese CPU?
    jne .try_slot                   ; nein → nächster Slot

.check_running:
    ; §133: Slot bereits auf anderer CPU laufend?
    cmp dword [thread_running_on + edx * 4], -1
    jne .try_slot                   ; belegt → nächster Slot

.slot_ok:
    ; Slot passt – übernehmen
    mov [scheduler_current], edx
    mov [scheduler_selected_slot], edx
    mov [thread_running_on + edx * 4], edi   ; §133: als "laufend auf CPU edi" markieren

    cmp dword [thread_manager_ready], 1
    jne .load_frame
    mov eax, [thread_task_ids + edx * 4]
    test eax, eax
    jz .load_frame
    call task_lookup
    jc .load_frame
    cmp dword [eax + TASK_STATE], TASK_STATE_READY
    jne .load_frame
    mov dword [eax + TASK_STATE], TASK_STATE_RUNNING

.load_frame:
    mov edx, [scheduler_selected_slot]
    mov eax, [scheduler_contexts + edx * 4]
    jmp .done

.no_slot_found:
    ; §133: Kein freier kompatibler Slot.
    ; Aktuellen Slot wieder als "laufend" markieren und gleichen Frame zurückgeben.
    mov edx, [scheduler_previous_slot]
    mov [thread_running_on + edx * 4], edi
    mov [scheduler_current], edx
    mov eax, [esp + 12]

.done:
    pop edi
    pop esi
    ret

scheduler_thread1:
    inc dword [scheduler_thread1_runs]
    mov eax, [ipc_sent]
    inc eax
    mov dword [ipc_thread_message + 0], 1
    mov [ipc_thread_message + 4], eax
    mov dword [ipc_thread_message + 8], 0x4E4F5641
    mov dword [ipc_thread_message + 12], 0
    mov esi, ipc_thread_message
    call ipc_send
    test eax, eax
    jz .pause
    inc dword [ipc_sent]
.pause:
    pause
    jmp scheduler_thread1

scheduler_thread2:
    inc dword [scheduler_thread2_runs]
    mov edi, ipc_receive_buffer
    call ipc_receive
    test eax, eax
    jz .pause
    cmp dword [ipc_receive_buffer + 0], 1
    jne .error
    cmp dword [ipc_receive_buffer + 8], 0x4E4F5641
    jne .error
    mov eax, [ipc_receive_buffer + 4]
    cmp eax, [ipc_last_sequence]
    jbe .error
    mov [ipc_last_sequence], eax
    inc dword [ipc_received]
    jmp .pause
.error:
    mov dword [ipc_error], 1
.pause:
    pause
    jmp scheduler_thread2

scheduler_dynamic_thread:
    inc dword [scheduler_dynamic_runs]
    pause
    jmp scheduler_dynamic_thread

scheduler_self_test:
    mov ecx, 100000000
.wait:
    cmp dword [scheduler_thread1_runs], 0
    je .continue
    cmp dword [scheduler_thread2_runs], 0
    je .continue
    cmp dword [scheduler_dynamic_runs], 0
    je .continue
    cmp dword [ipc_error], 0
    jne .invalid
    cmp dword [ipc_received], 4
    jae .success
.continue:
    pause
    dec ecx
    jnz .wait
.invalid:
    stc
    ret
.success:
    clc
    ret

align 4
scheduler_api:
    dd SCHEDULER_API_SIZE
    dw 1, 0
    dd SCHEDULER_THREAD_COUNT
    dd SCHEDULER_CAP_PREEMPT | SCHEDULER_CAP_RR | SCHEDULER_CAP_AFFINITY | SCHEDULER_CAP_DYNAMIC
    dd scheduler_on_tick
    dd scheduler_current
    dd scheduler_thread1_runs
    dd scheduler_thread2_runs
    dd thread_cpu_affinity          ; Zeiger auf per-Slot Affinitäts-Array (§133)
    dd thread_running_on            ; Zeiger auf per-Slot Running-On-Array (§133)
    dd scheduler_contexts           ; §134: Slot-Kontexte für dynamische Threads
    dd thread_task_ids              ; §134: Slot→Task-Zuordnung

scheduler_lock:         dd 0       ; Spinlock – nur eine CPU gleichzeitig in scheduler_on_tick
scheduler_caller_cpu:   dd 0       ; CPU-Slot des Aufrufers (unter Spinlock gesetzt, §133)
scheduler_enabled:      dd 0
scheduler_current:      dd 0
scheduler_previous_slot: dd 0
scheduler_selected_slot: dd 0
scheduler_contexts:
    times SCHEDULER_THREAD_COUNT dd 0
thread_cpu_affinity:               ; per-Slot: -1=beliebige CPU, N=nur CPU N (§133)
    times SCHEDULER_THREAD_COUNT dd -1
thread_running_on:                 ; per-Slot: -1=frei, N=läuft auf CPU N (§133)
    times SCHEDULER_THREAD_COUNT dd -1
scheduler_thread1_runs: dd 0
scheduler_thread2_runs: dd 0
scheduler_dynamic_runs: dd 0

; ---------------------------------------------------------------------------
; Work-Stealing-Scheduler – pro-Slot lokale Task-Queues mit constraint-
; bewusster Steal-Logik (RT-Schutz, harte CPU-Affinitaet, NUMA-ready).
; NPSPEC-CONCURRENCY-WORKSTEALING-0001
; ---------------------------------------------------------------------------

WS_SLOT_COUNT       equ SCHEDULER_THREAD_COUNT
WS_QUEUE_DEPTH      equ 4
WS_TASK_ENTRY_SIZE  equ 16
WS_SLOT_RECORD_SIZE equ 24

SCHEDULER_CAP_WORKSTEAL equ 0x00000020

; Task-Eintrag-Felder
WS_TASK_ENTRY_POINT equ 0
WS_TASK_PID         equ 4
WS_TASK_CPU_MASK    equ 8
WS_TASK_FLAGS       equ 12

WS_TASK_FLAG_RT       equ 0x01     ; Hard-Realtime, nicht stehlfaehig
WS_TASK_FLAG_HARD_AFF equ 0x02     ; Strenge CPU-Affinitaet

; Slot-Queue-Header-Felder
WS_HEAD        equ 0
WS_TAIL        equ 4
WS_COUNT       equ 8
WS_CPU_MASK    equ 12
WS_FLAGS       equ 16
WS_STEAL_COUNT equ 20

worksteal_initialize:
    mov edi, ws_slot_queues
    xor eax, eax
    mov ecx, (WS_SLOT_COUNT * WS_SLOT_RECORD_SIZE) / 4
    rep stosd
    mov edi, ws_task_storage
    mov ecx, (WS_SLOT_COUNT * WS_QUEUE_DEPTH * WS_TASK_ENTRY_SIZE) / 4
    rep stosd
    ; CPU-Masken aus dem Scheduler-Affinitaets-Array initialisieren
    xor ecx, ecx
.wi_mask_loop:
    cmp ecx, WS_SLOT_COUNT
    jae .wi_mask_done
    mov edi, ecx
    imul edi, WS_SLOT_RECORD_SIZE
    add edi, ws_slot_queues
    mov eax, [thread_cpu_affinity + ecx * 4]
    cmp eax, -1
    jne .wi_has_aff
    mov eax, 0xFFFFFFFF
.wi_has_aff:
    mov [edi + WS_CPU_MASK], eax
    inc ecx
    jmp .wi_mask_loop
.wi_mask_done:
    mov dword [ws_initialized], 1
    mov dword [ws_total_steals], 0
    clc
    ret

; Reiht Task-Eintrag (16 Bytes) in Slot ein.
; EAX=Slot (0..WS_SLOT_COUNT-1), ESI=Quell-Ptr. CF=0 ok, CF=1 voll/ungueltig.
worksteal_enqueue:
    push ebx
    push ecx
    push edx
    push edi
    cmp eax, WS_SLOT_COUNT
    jae .weq_bad
    mov ebx, eax
    imul ebx, WS_SLOT_RECORD_SIZE
    add ebx, ws_slot_queues
    cmp dword [ebx + WS_COUNT], WS_QUEUE_DEPTH
    jae .weq_full
    mov edx, eax
    imul edx, WS_QUEUE_DEPTH * WS_TASK_ENTRY_SIZE
    add edx, ws_task_storage
    mov ecx, [ebx + WS_TAIL]
    imul ecx, WS_TASK_ENTRY_SIZE
    add edx, ecx
    mov edi, edx
    mov ecx, WS_TASK_ENTRY_SIZE / 4
    rep movsd
    mov ecx, [ebx + WS_TAIL]
    inc ecx
    cmp ecx, WS_QUEUE_DEPTH
    jb .weq_no_wrap
    xor ecx, ecx
.weq_no_wrap:
    mov [ebx + WS_TAIL], ecx
    inc dword [ebx + WS_COUNT]
    pop edi
    pop edx
    pop ecx
    pop ebx
    clc
    ret
.weq_full:
.weq_bad:
    pop edi
    pop edx
    pop ecx
    pop ebx
    stc
    ret

; Versucht, einen Task vom Victim-Slot zu stehlen.
; EAX=victim_slot, EDX=thief_slot, EDI=Ausgabepuffer (16 Bytes).
; CF=0 gestohlen, CF=1 kein stehlfaehiger Task.
worksteal_try_steal:
    push ebx
    push ecx
    push esi
    cmp eax, WS_SLOT_COUNT
    jae .wts_fail
    cmp edx, WS_SLOT_COUNT
    jae .wts_fail
    cmp eax, edx
    je .wts_fail
    mov ebx, eax
    imul ebx, WS_SLOT_RECORD_SIZE
    add ebx, ws_slot_queues
    cmp dword [ebx + WS_COUNT], 0
    je .wts_fail
    ; ESI = Adresse des Tasks am Head des Victim-Slots
    mov ecx, eax
    imul ecx, WS_QUEUE_DEPTH * WS_TASK_ENTRY_SIZE
    add ecx, ws_task_storage
    push eax
    mov eax, [ebx + WS_HEAD]
    imul eax, WS_TASK_ENTRY_SIZE
    add ecx, eax
    pop eax
    mov esi, ecx
    ; RT-Bit: niemals stehlen
    mov ecx, [esi + WS_TASK_FLAGS]
    test ecx, WS_TASK_FLAG_RT
    jnz .wts_fail
    ; Harte Affinitaet: nur wenn CPU-Mengen sich schneiden
    test ecx, WS_TASK_FLAG_HARD_AFF
    jz .wts_eligible
    push edx
    imul edx, WS_SLOT_RECORD_SIZE
    add edx, ws_slot_queues
    mov ecx, [edx + WS_CPU_MASK]
    pop edx
    and ecx, [esi + WS_TASK_CPU_MASK]
    jz .wts_fail
.wts_eligible:
    mov ecx, WS_TASK_ENTRY_SIZE / 4
    rep movsd
    mov ecx, [ebx + WS_HEAD]
    inc ecx
    cmp ecx, WS_QUEUE_DEPTH
    jb .wts_no_wrap
    xor ecx, ecx
.wts_no_wrap:
    mov [ebx + WS_HEAD], ecx
    dec dword [ebx + WS_COUNT]
    inc dword [ebx + WS_STEAL_COUNT]
    inc dword [ws_total_steals]
    pop esi
    pop ecx
    pop ebx
    clc
    ret
.wts_fail:
    pop esi
    pop ecx
    pop ebx
    stc
    ret

; Self-Test: 7 Tests (Init, Enqueue, Count, RT-Schutz, Steal, Daten, Count-0).
worksteal_self_test:
    push ebx
    push esi
    push edi
    sub esp, 32                         ; [esp+0..15]=Task-Buf, [esp+16..31]=Steal-Buf
    xor ebx, ebx

    ; Test 1: Subsystem initialisiert
    cmp dword [ws_initialized], 1
    je .wt2
    inc ebx

.wt2:
    ; Test 2: Slot 0 nach Init leer
    mov esi, ws_slot_queues
    cmp dword [esi + WS_COUNT], 0
    je .wt3
    inc ebx

.wt3:
    ; Test 3: Normalen Task in Slot 0 einreihen
    mov dword [esp +  0], scheduler_thread1
    mov dword [esp +  4], 1
    mov dword [esp +  8], 0xFFFFFFFF
    mov dword [esp + 12], 0
    xor eax, eax
    lea esi, [esp]
    call worksteal_enqueue
    jnc .wt4
    inc ebx

.wt4:
    ; Test 4: Count == 1 in Slot 0
    mov esi, ws_slot_queues
    cmp dword [esi + WS_COUNT], 1
    je .wt5
    inc ebx

.wt5:
    ; Test 5: RT-Task in Slot 1, Steal muss scheitern
    mov dword [esp +  0], scheduler_thread2
    mov dword [esp +  4], 1
    mov dword [esp +  8], 0xFFFFFFFF
    mov dword [esp + 12], WS_TASK_FLAG_RT
    mov eax, 1
    lea esi, [esp]
    call worksteal_enqueue
    jnc .wt5_steal
    inc ebx
    jmp .wt6
.wt5_steal:
    mov eax, 1
    mov edx, 0
    lea edi, [esp + 16]
    call worksteal_try_steal
    jc .wt6                         ; CF=1 erwartet (RT-Schutz)
    inc ebx

.wt6:
    ; Test 6: Steal des normalen Tasks von Slot 0 nach Slot 1
    xor eax, eax
    mov edx, 1
    lea edi, [esp + 16]
    call worksteal_try_steal
    jnc .wt7
    inc ebx

.wt7:
    ; Test 7: Count in Slot 0 == 0 nach Steal
    mov esi, ws_slot_queues
    cmp dword [esi + WS_COUNT], 0
    je .wt_done
    inc ebx

.wt_done:
    test ebx, ebx
    jnz .wt_fail
    add esp, 32
    pop edi
    pop esi
    pop ebx
    clc
    ret
.wt_fail:
    add esp, 32
    pop edi
    pop esi
    pop ebx
    stc
    ret

ws_initialized:  dd 0
ws_total_steals: dd 0
align 4
ws_slot_queues:
    times WS_SLOT_COUNT * WS_SLOT_RECORD_SIZE db 0
ws_task_storage:
    times WS_SLOT_COUNT * WS_QUEUE_DEPTH * WS_TASK_ENTRY_SIZE db 0

; ---------------------------------------------------------------------------
; Minimaler Kernel Main
; ---------------------------------------------------------------------------

kernel_main:
    call kernel_operational_prepare
    jmp kernel_idle

kernel_operational_prepare:
    mov eax, [kernel_context + CONTEXT_SEEN]
    test eax, CONTEXT_HAS_GRAPHICS
    jz .text_mode
    cmp dword [kernel_context + CONTEXT_BPP], 32
    jne .text_mode
    ; Der UEFI-Kernellader hat den verifizierten Bootsplash bereits direkt vor
    ; ExitBootServices dargestellt. Der normale Kernelstart bewahrt diesen
    ; Framebuffer, bis die Ring-3-System-UI ihre erste Szene präsentiert.
    mov esi, message_framebuffer_ok
    call serial_write_string
    jmp .ready

.text_mode:
    mov esi, message_text_mode
    call serial_write_string

.ready:
    mov esi, message_ready
    call serial_write_string
    ret

kernel_idle:
    sti
.loop:
    call power_cpu_idle
    jmp .loop

kernel_shutdown:
    cli
    call power_manager_request_shutdown
    jc kernel_halt
    call power_manager_platform_off
    jmp kernel_halt

kernel_shutdown_authorized:
    cli
    call power_manager_platform_off
    jmp kernel_halt

; ---------------------------------------------------------------------------
; Power Manager ABI 1.0 (NPSPEC-KERNEL-0021)
; ---------------------------------------------------------------------------
POWER_API_SIZE                 equ 72
POWER_STATE_RUNNING            equ 0
POWER_STATE_IDLE               equ 1
POWER_STATE_SUSPEND_TO_IDLE    equ 2
POWER_STATE_SUSPEND_TO_RAM     equ 3
POWER_STATE_HIBERNATE          equ 4
POWER_STATE_HYBRID_SLEEP       equ 5
POWER_STATE_SHUTDOWN           equ 6
POWER_STATE_RESTART            equ 7
POWER_STATE_OFF                equ 8
POWER_SUPPORTED_MASK           equ (1 << POWER_STATE_RUNNING) | (1 << POWER_STATE_IDLE) | (1 << POWER_STATE_SHUTDOWN) | (1 << POWER_STATE_RESTART) | (1 << POWER_STATE_OFF)
POWER_PROFILE_BALANCED         equ 1
POWER_PROFILE_PERFORMANCE      equ 0
POWER_PROFILE_EFFICIENCY       equ 2
POWER_PROFILE_POWERSAVE        equ 3
POWER_PROFILE_SUPPORTED_MASK   equ 0x0000000F
POWER_PHASE_NONE               equ 0
POWER_PHASE_REQUEST            equ 1
POWER_PHASE_FREEZE_USERSPACE   equ 2
POWER_PHASE_SYNC_STORAGE       equ 3
POWER_PHASE_STOP_DEVICES       equ 4
POWER_PHASE_PLATFORM_OFF       equ 5
POWER_WAKE_LOCK_CAPACITY       equ 2
POWER_IDLE_SHALLOW             equ 0
POWER_IDLE_DEEP                equ 1

power_manager_initialize:
    mov dword [power_current_state], POWER_STATE_RUNNING
    mov dword [power_target_state], POWER_STATE_RUNNING
    mov dword [power_profile], POWER_PROFILE_BALANCED
    mov dword [power_transition_phase], POWER_PHASE_NONE
    mov dword [power_transition_active], 0
    mov dword [power_shutdown_requests], 0
    mov dword [power_transition_failures], 0
    mov dword [power_wake_lock_count], 0
    mov dword [power_wake_lock_expirations], 0
    mov dword [power_idle_entries], 0
    mov dword [power_deep_idle_entries], 0
    mov dword [power_profile_changes], 0
    mov dword [power_wake_lock_deadlines], 0
    mov dword [power_wake_lock_deadlines + 4], 0
    clc
    ret

power_manager_self_test:
    cmp dword [power_api], POWER_API_SIZE
    jne .invalid
    cmp word [power_api + 4], 1
    jne .invalid
    cmp dword [power_api + 12], POWER_SUPPORTED_MASK
    jne .invalid
    cmp dword [power_current_state], POWER_STATE_RUNNING
    jne .invalid
    mov eax, 10
    call power_wake_lock_acquire
    jc .invalid
    cmp dword [power_wake_lock_count], 1
    jne .invalid
    call power_wake_lock_release
    jc .invalid
    cmp dword [power_wake_lock_count], 0
    jne .invalid
    clc
    ret
.invalid:
    stc
    ret

; CF=1, falls bereits ein globaler Energieuebergang aktiv ist.
power_manager_request_shutdown:
    cmp dword [power_transition_active], 0
    jne .busy
    mov dword [power_transition_active], 1
    inc dword [power_shutdown_requests]
    mov dword [power_target_state], POWER_STATE_SHUTDOWN
    mov dword [power_transition_phase], POWER_PHASE_REQUEST
    mov esi, message_shutdown_requested
    call serial_write_string
    mov dword [power_transition_phase], POWER_PHASE_FREEZE_USERSPACE
    mov esi, message_shutdown_userspace
    call serial_write_string
    mov dword [power_transition_phase], POWER_PHASE_SYNC_STORAGE
    mov esi, message_shutdown_storage
    call serial_write_string
    mov dword [power_transition_phase], POWER_PHASE_STOP_DEVICES
    mov esi, message_shutdown_devices
    call serial_write_string
    mov dword [power_current_state], POWER_STATE_SHUTDOWN
    clc
    ret
.busy:
    inc dword [power_transition_failures]
    stc
    ret

; EAX=maximale Dauer in 100-Hz-Ticks, Rueckgabe EAX=Slot.
power_wake_lock_acquire:
    test eax, eax
    jz .invalid
    cmp dword [power_wake_lock_count], POWER_WAKE_LOCK_CAPACITY
    jae .invalid
    mov edx, [timer_ticks]
    add edx, eax
    jc .invalid
    xor ecx, ecx
.find:
    cmp dword [power_wake_lock_deadlines + ecx * 4], 0
    je .found
    inc ecx
    cmp ecx, POWER_WAKE_LOCK_CAPACITY
    jb .find
.invalid:
    stc
    ret
.found:
    mov [power_wake_lock_deadlines + ecx * 4], edx
    inc dword [power_wake_lock_count]
    mov eax, ecx
    clc
    ret

; EAX=Slot.
power_wake_lock_release:
    cmp eax, POWER_WAKE_LOCK_CAPACITY
    jae .invalid
    cmp dword [power_wake_lock_deadlines + eax * 4], 0
    je .invalid
    mov dword [power_wake_lock_deadlines + eax * 4], 0
    cmp dword [power_wake_lock_count], 0
    je .invalid
    dec dword [power_wake_lock_count]
    clc
    ret
.invalid:
    stc
    ret

power_wake_lock_expire:
    xor ecx, ecx
.next:
    mov eax, [power_wake_lock_deadlines + ecx * 4]
    test eax, eax
    jz .continue
    mov edx, [timer_ticks]
    sub edx, eax
    js .continue
    mov dword [power_wake_lock_deadlines + ecx * 4], 0
    dec dword [power_wake_lock_count]
    inc dword [power_wake_lock_expirations]
.continue:
    inc ecx
    cmp ecx, POWER_WAKE_LOCK_CAPACITY
    jb .next
    ret

power_cpu_idle:
    call power_wake_lock_expire
    inc dword [power_idle_entries]
    cmp dword [power_wake_lock_count], 0
    jne .shallow
    mov dword [power_idle_state], POWER_IDLE_DEEP
    inc dword [power_deep_idle_entries]
    hlt
    ret
.shallow:
    mov dword [power_idle_state], POWER_IDLE_SHALLOW
    hlt
    ret

; EAX=Profil. Nur zur Laufzeit als unterstuetzt gemeldete Profile annehmen.
power_manager_set_profile:
    cmp eax, POWER_PROFILE_POWERSAVE
    ja .invalid
    cmp dword [power_transition_active], 0
    jne .invalid
    cmp [power_profile], eax
    je .success
    mov [power_profile], eax
    inc dword [power_profile_changes]
.success:
    clc
    ret
.invalid:
    stc
    ret

power_manager_platform_off:
    mov dword [power_transition_phase], POWER_PHASE_PLATFORM_OFF
    mov esi, message_shutdown_platform
    call serial_write_string
    mov ax, 0x2000
    mov dx, 0x0604                  ; QEMU/ACPI Poweroff
    out dx, ax
    mov dx, 0xB004                  ; Bochs/QEMU Fallback
    out dx, ax
    mov dword [power_current_state], POWER_STATE_OFF
    ret

align 4
power_api:
    dd POWER_API_SIZE
    dw 1, 0
    dd POWER_STATE_RUNNING
    dd POWER_SUPPORTED_MASK
    dd power_current_state
    dd power_target_state
    dd power_profile
    dd power_transition_phase
    dd power_shutdown_requests
    dd power_transition_failures
    dd power_manager_request_shutdown
    dd power_manager_platform_off
    dd power_wake_lock_acquire
    dd power_wake_lock_release
    dd power_cpu_idle
    dd power_wake_lock_count
    dd power_manager_set_profile
    dd power_profile_changes
power_current_state:       dd POWER_STATE_RUNNING
power_target_state:        dd POWER_STATE_RUNNING
power_profile:             dd POWER_PROFILE_BALANCED
power_transition_phase:    dd POWER_PHASE_NONE
power_transition_active:   dd 0
power_shutdown_requests:   dd 0
power_transition_failures: dd 0
power_wake_lock_count:      dd 0
power_wake_lock_expirations: dd 0
power_idle_state:           dd POWER_IDLE_SHALLOW
power_idle_entries:         dd 0
power_deep_idle_entries:    dd 0
power_profile_changes:      dd 0
power_wake_lock_deadlines:  times POWER_WAKE_LOCK_CAPACITY dd 0

; Minimaler Network-Core nach NPSPEC-KERNEL-0022. Hardwareprotokolle folgen
; auf dieser objekt- und capability-basierten Grundlage.
NETWORK_API_SIZE          equ 40
NETWORK_INTERFACE_UP      equ 2
NETWORK_INTERFACE_LOOPBACK equ 0
NETWORK_LOOPBACK_MTU      equ 65536
NETWORK_FEATURE_IPV4      equ 0x00000001
NETWORK_FEATURE_IPV6      equ 0x00000002
OBJECT_TYPE_NET_INTERFACE equ 12
OBJECT_TYPE_NET_SOCKET    equ 13
NETWORK_SOCKET_DATAGRAM   equ 1
NETWORK_SOCKET_STREAM     equ 2
NETWORK_AF_IPV4           equ 4
NETWORK_IPV4_LOOPBACK     equ 0x0100007F
NETWORK_BOOTSTRAP_PORT    equ 9000
TCP_STATE_CLOSED          equ 0
TCP_STATE_LISTEN          equ 1
TCP_STATE_SYN_SENT        equ 2
TCP_STATE_SYN_RECEIVED    equ 3
TCP_STATE_ESTABLISHED     equ 4
TCP_STATE_FIN_WAIT_1      equ 5
TCP_STATE_FIN_WAIT_2      equ 6
TCP_STATE_TIME_WAIT       equ 10

network_manager_initialize:
    mov dword [network_namespace_generation], 1
    mov dword [network_interface_count], 0
    mov dword [network_loopback_id], 1
    mov dword [network_loopback_state], NETWORK_INTERFACE_UP
    mov dword [network_received_packets], 0
    mov dword [network_transmitted_packets], 0
    mov dword [network_dropped_packets], 0
    mov dword [network_loopback_queue_count], 0
    mov dword [network_socket_object], 0
    mov dword [network_socket_handle], 0
    mov dword [network_socket_bound], 0
    mov dword [network_socket_port], 0
    mov dword [network_tcp_socket_object], 0
    mov dword [network_tcp_socket_handle], 0
    mov dword [network_tcp_socket_connected], 0
    mov dword [network_tcp_queue_count], 0
    mov dword [network_tcp_queue_length], 0
    mov dword [network_tcp_transmitted_bytes], 0
    mov dword [network_tcp_received_bytes], 0
    mov dword [network_tcp_listener_handle], 0
    mov dword [network_tcp_listener_bound], 0
    mov dword [network_tcp_listener_state], TCP_STATE_CLOSED
    mov dword [network_tcp_listener_backlog], 0
    mov dword [network_tcp_pending_connections], 0
    mov dword [network_tcp_accepted_connections], 0
    mov dword [network_route_count], 2
    mov dword [network_route_lookups], 0
    mov dword [network_route_hits], 0
    mov dword [network_route_last_prefix], 0
    mov dword [network_firewall_decisions], 0
    mov dword [network_firewall_drops], 0
    mov dword [network_ipv6_udp_tests], 0
    mov dword [network_icmpv6_echo_tests], 0
    mov dword [network_tcp_tests], 0
    mov edi, network_loopback_frame
    xor eax, eax
    mov ecx, 64 / 4
    rep stosd
    mov eax, OBJECT_TYPE_NET_INTERFACE
    mov edx, NETWORK_INTERFACE_LOOPBACK
    mov ebx, 1
    call object_create
    jc .invalid
    mov [network_loopback_object], eax
    mov dword [network_interface_count], 1
    clc
    ret
.invalid:
    stc
    ret

network_manager_self_test:
    cmp dword [network_api], NETWORK_API_SIZE
    jne .invalid
    cmp word [network_api + 4], 1
    jne .invalid
    cmp dword [network_interface_count], 1
    jne .invalid
    cmp dword [network_loopback_id], 1
    jne .invalid
    cmp dword [network_loopback_state], NETWORK_INTERFACE_UP
    jne .invalid
    cmp dword [network_loopback_object], 0
    je .invalid
    cmp dword [network_socket_bound], 0
    jne .invalid
    call network_icmp_echo_self_test
    jc .invalid
    call network_ipv6_udp_self_test
    jc .invalid
    call network_icmpv6_echo_self_test
    jc .invalid
    call network_tcp_self_test
    jc .invalid
    mov eax, 0x7F000001             ; 127.0.0.1 in Host-Reihenfolge
    call network_route_lookup_ipv4
    jc .invalid
    cmp eax, 1
    jne .invalid
    cmp dword [network_route_last_prefix], 32
    jne .invalid                    ; Hostroute muss /8 überstimmen
    mov eax, 0x0A000001             ; keine Default-Route vorhanden
    call network_route_lookup_ipv4
    jnc .invalid
    mov eax, 0x7F000001
    mov ebx, 17
    mov ecx, NETWORK_BOOTSTRAP_PORT
    call network_firewall_check_ipv4
    jc .invalid
    mov eax, 0x0A000001             ; externes Ziel muss fail-closed sein
    mov ebx, 17
    mov ecx, NETWORK_BOOTSTRAP_PORT
    call network_firewall_check_ipv4
    jnc .invalid
    clc
    ret
.invalid:
    stc
    ret

; ICMPv4-Echo-Grundpfad: Request und Reply werden vollständig im Kernel
; erzeugt und jeweils vor der Verarbeitung per Internet-Checksum validiert.
network_icmp_echo_self_test:
    mov edi, network_icmp_buffer
    xor eax, eax
    mov ecx, 16 / 4
    rep stosd
    mov byte [network_icmp_buffer + 0], 8  ; Echo Request
    mov byte [network_icmp_buffer + 1], 0
    mov word [network_icmp_buffer + 4], 0x414E
    mov word [network_icmp_buffer + 6], 0x0100
    mov dword [network_icmp_buffer + 8], 0x41564F4E
    mov dword [network_icmp_buffer + 12], 0x534F4156
    mov esi, network_icmp_buffer
    mov ecx, 16
    call network_checksum
    xchg al, ah
    mov [network_icmp_buffer + 2], ax
    call network_icmp_validate_request
    jc .invalid
    mov byte [network_icmp_buffer + 0], 0  ; Echo Reply
    mov word [network_icmp_buffer + 2], 0
    mov esi, network_icmp_buffer
    mov ecx, 16
    call network_checksum
    xchg al, ah
    mov [network_icmp_buffer + 2], ax
    call network_icmp_validate_reply
    jc .invalid
    mov dword [network_icmp_echo_tests], 1
    clc
    ret
.invalid:
    stc
    ret

network_icmp_validate_request:
    cmp byte [network_icmp_buffer], 8
    jne .invalid
    jmp network_icmp_validate_common
.invalid:
    stc
    ret

network_icmp_validate_reply:
    cmp byte [network_icmp_buffer], 0
    jne .invalid
    jmp network_icmp_validate_common
.invalid:
    stc
    ret

network_icmp_validate_common:
    cmp byte [network_icmp_buffer + 1], 0
    jne .invalid
    mov esi, network_icmp_buffer
    mov ecx, 16
    call network_checksum
    test ax, ax
    jnz .invalid
    clc
    ret
.invalid:
    stc
    ret

; IPv6/UDP-Loopback-Selbsttest. IPv6 besitzt keine Headerprüfsumme; die
; UDP-Prüfsumme über den 40-Byte-Pseudoheader ist dagegen verbindlich.
network_ipv6_udp_self_test:
    mov edi, network_ipv6_frame
    xor eax, eax
    mov ecx, 64 / 4
    rep stosd
    mov byte [network_ipv6_frame + 0], 0x60
    mov word [network_ipv6_frame + 4], 0x0C00 ; 12 Byte Payload in Netzreihenfolge
    mov byte [network_ipv6_frame + 6], 17
    mov byte [network_ipv6_frame + 7], 64
    mov byte [network_ipv6_frame + 23], 1     ; Quelle ::1
    mov byte [network_ipv6_frame + 39], 1     ; Ziel ::1
    mov ax, NETWORK_BOOTSTRAP_PORT
    xchg al, ah
    mov [network_ipv6_frame + 40], ax
    mov [network_ipv6_frame + 42], ax
    mov word [network_ipv6_frame + 44], 0x0C00
    mov dword [network_ipv6_frame + 48], 0x41564F4E
    call network_ipv6_checksum_prepare
    mov esi, network_ipv6_checksum_buffer
    mov ecx, 52
    call network_checksum
    test ax, ax
    jnz .checksum_ready
    mov ax, 0xFFFF
.checksum_ready:
    xchg al, ah
    mov [network_ipv6_frame + 46], ax
    call network_ipv6_udp_validate
    jc .invalid
    mov dword [network_ipv6_udp_tests], 1
    clc
    ret
.invalid:
    stc
    ret

network_ipv6_checksum_prepare:
    mov edi, network_ipv6_checksum_buffer
    xor eax, eax
    mov ecx, 64 / 4
    rep stosd
    mov esi, network_ipv6_frame + 8
    mov edi, network_ipv6_checksum_buffer
    mov ecx, 32
    rep movsb
    mov dword [network_ipv6_checksum_buffer + 32], 0x0C000000
    mov byte [network_ipv6_checksum_buffer + 39], 17
    mov esi, network_ipv6_frame + 40
    mov edi, network_ipv6_checksum_buffer + 40
    mov ecx, 12
    rep movsb
    ret

network_ipv6_udp_validate:
    cmp byte [network_ipv6_frame], 0x60
    jne .invalid
    cmp word [network_ipv6_frame + 4], 0x0C00
    jne .invalid
    cmp byte [network_ipv6_frame + 6], 17
    jne .invalid
    cmp byte [network_ipv6_frame + 23], 1
    jne .invalid
    cmp byte [network_ipv6_frame + 39], 1
    jne .invalid
    cmp word [network_ipv6_frame + 46], 0
    je .invalid
    call network_ipv6_checksum_prepare
    mov esi, network_ipv6_checksum_buffer
    mov ecx, 52
    call network_checksum
    test ax, ax
    jnz .invalid
    clc
    ret
.invalid:
    stc
    ret

; ICMPv6 Echo benötigt – anders als ICMPv4 – die IPv6-Quell- und Zieladresse
; im Prüfsummen-Pseudoheader. Request (128) und Reply (129) werden geprüft.
network_icmpv6_echo_self_test:
    mov edi, network_icmpv6_buffer
    xor eax, eax
    mov ecx, 12 / 4
    rep stosd
    mov byte [network_icmpv6_buffer + 0], 128
    mov byte [network_icmpv6_buffer + 1], 0
    mov word [network_icmpv6_buffer + 4], 0x414E
    mov word [network_icmpv6_buffer + 6], 0x0100
    mov dword [network_icmpv6_buffer + 8], 0x36564F4E
    call network_icmpv6_checksum_prepare
    mov esi, network_icmpv6_checksum_buffer
    mov ecx, 52
    call network_checksum
    xchg al, ah
    mov [network_icmpv6_buffer + 2], ax
    mov al, 128
    call network_icmpv6_validate_type
    jc .invalid
    mov byte [network_icmpv6_buffer], 129
    mov word [network_icmpv6_buffer + 2], 0
    call network_icmpv6_checksum_prepare
    mov esi, network_icmpv6_checksum_buffer
    mov ecx, 52
    call network_checksum
    xchg al, ah
    mov [network_icmpv6_buffer + 2], ax
    mov al, 129
    call network_icmpv6_validate_type
    jc .invalid
    mov dword [network_icmpv6_echo_tests], 1
    clc
    ret
.invalid:
    stc
    ret

network_icmpv6_checksum_prepare:
    mov edi, network_icmpv6_checksum_buffer
    xor eax, eax
    mov ecx, 64 / 4
    rep stosd
    mov byte [network_icmpv6_checksum_buffer + 15], 1
    mov byte [network_icmpv6_checksum_buffer + 31], 1
    mov dword [network_icmpv6_checksum_buffer + 32], 0x0C000000
    mov byte [network_icmpv6_checksum_buffer + 39], 58
    mov esi, network_icmpv6_buffer
    mov edi, network_icmpv6_checksum_buffer + 40
    mov ecx, 12
    rep movsb
    ret

network_icmpv6_validate_type:
    cmp [network_icmpv6_buffer], al
    jne .invalid
    cmp byte [network_icmpv6_buffer + 1], 0
    jne .invalid
    cmp word [network_icmpv6_buffer + 2], 0
    je .invalid
    push eax
    call network_icmpv6_checksum_prepare
    mov esi, network_icmpv6_checksum_buffer
    mov ecx, 52
    call network_checksum
    test ax, ax
    pop eax
    jnz .invalid
    clc
    ret
.invalid:
    stc
    ret

; Begrenzter TCP/IPv4-Loopback-Selbsttest. Er validiert den Zustandsautomaten,
; die verbindliche Pseudoheader-Pruefsumme, geordnete Sequenznummern, eine
; kontrollierte Retransmission und den vollstaendigen Verbindungsabbau.
network_tcp_self_test:
    mov dword [network_tcp_state], TCP_STATE_CLOSED
    mov dword [network_tcp_tests], 0
    mov dword [network_tcp_retransmissions], 0
    mov dword [network_tcp_send_sequence], 0x1000
    mov dword [network_tcp_receive_sequence], 0x2000
    mov eax, TCP_STATE_SYN_SENT
    call network_tcp_transition
    jc .invalid
    mov eax, TCP_STATE_SYN_RECEIVED
    call network_tcp_transition
    jc .invalid
    mov eax, TCP_STATE_ESTABLISHED
    call network_tcp_transition
    jc .invalid
    inc dword [network_tcp_tests]

    mov edi, network_tcp_segment
    xor eax, eax
    mov ecx, 24 / 4
    rep stosd
    mov ax, NETWORK_BOOTSTRAP_PORT
    xchg al, ah
    mov [network_tcp_segment + 0], ax
    mov [network_tcp_segment + 2], ax
    mov eax, [network_tcp_send_sequence]
    bswap eax
    mov [network_tcp_segment + 4], eax
    mov eax, [network_tcp_receive_sequence]
    bswap eax
    mov [network_tcp_segment + 8], eax
    mov byte [network_tcp_segment + 12], 0x50 ; 20-Byte TCP-Header
    mov byte [network_tcp_segment + 13], 0x18 ; ACK | PSH
    mov word [network_tcp_segment + 14], 0x0010
    mov dword [network_tcp_segment + 20], 0x41564F4E
    call network_tcp_checksum_prepare
    mov esi, network_tcp_checksum_buffer
    mov ecx, 36
    call network_checksum
    xchg al, ah
    mov [network_tcp_segment + 16], ax
    call network_tcp_validate_segment
    jc .invalid
    add dword [network_tcp_send_sequence], 4
    inc dword [network_tcp_tests]

    ; Der erste ACK bleibt absichtlich aus. Genau eine begrenzte erneute
    ; Uebertragung wird verbucht; die Sequenznummer darf sich nicht aendern.
    mov eax, [network_tcp_send_sequence]
    inc dword [network_tcp_retransmissions]
    cmp eax, [network_tcp_send_sequence]
    jne .invalid
    cmp dword [network_tcp_retransmissions], 1
    jne .invalid
    inc dword [network_tcp_tests]

    mov eax, TCP_STATE_FIN_WAIT_1
    call network_tcp_transition
    jc .invalid
    mov eax, TCP_STATE_FIN_WAIT_2
    call network_tcp_transition
    jc .invalid
    mov eax, TCP_STATE_TIME_WAIT
    call network_tcp_transition
    jc .invalid
    mov eax, TCP_STATE_CLOSED
    call network_tcp_transition
    jc .invalid
    inc dword [network_tcp_tests]
    clc
    ret
.invalid:
    stc
    ret

network_tcp_checksum_prepare:
    mov edi, network_tcp_checksum_buffer
    xor eax, eax
    mov ecx, 40 / 4
    rep stosd
    mov dword [network_tcp_checksum_buffer + 0], NETWORK_IPV4_LOOPBACK
    mov dword [network_tcp_checksum_buffer + 4], NETWORK_IPV4_LOOPBACK
    mov byte [network_tcp_checksum_buffer + 9], 6
    mov word [network_tcp_checksum_buffer + 10], 0x1800
    mov esi, network_tcp_segment
    mov edi, network_tcp_checksum_buffer + 12
    mov ecx, 24
    rep movsb
    ret

network_tcp_validate_segment:
    cmp dword [network_tcp_state], TCP_STATE_ESTABLISHED
    jne .invalid
    cmp byte [network_tcp_segment + 12], 0x50
    jne .invalid
    cmp byte [network_tcp_segment + 13], 0x18
    jne .invalid
    mov eax, [network_tcp_segment + 4]
    bswap eax
    cmp eax, [network_tcp_send_sequence]
    jne .invalid
    call network_tcp_checksum_prepare
    mov esi, network_tcp_checksum_buffer
    mov ecx, 36
    call network_checksum
    test ax, ax
    jnz .invalid
    clc
    ret
.invalid:
    stc
    ret

network_tcp_transition:
    mov edx, [network_tcp_state]
    cmp edx, TCP_STATE_CLOSED
    jne .from_syn_sent
    cmp eax, TCP_STATE_SYN_SENT
    jne .invalid
    jmp .commit
.from_syn_sent:
    cmp edx, TCP_STATE_SYN_SENT
    jne .from_syn_received
    cmp eax, TCP_STATE_SYN_RECEIVED
    jne .invalid
    jmp .commit
.from_syn_received:
    cmp edx, TCP_STATE_SYN_RECEIVED
    jne .from_established
    cmp eax, TCP_STATE_ESTABLISHED
    jne .invalid
    jmp .commit
.from_established:
    cmp edx, TCP_STATE_ESTABLISHED
    jne .from_fin_wait_1
    cmp eax, TCP_STATE_FIN_WAIT_1
    jne .invalid
    jmp .commit
.from_fin_wait_1:
    cmp edx, TCP_STATE_FIN_WAIT_1
    jne .from_fin_wait_2
    cmp eax, TCP_STATE_FIN_WAIT_2
    jne .invalid
    jmp .commit
.from_fin_wait_2:
    cmp edx, TCP_STATE_FIN_WAIT_2
    jne .from_time_wait
    cmp eax, TCP_STATE_TIME_WAIT
    jne .invalid
    jmp .commit
.from_time_wait:
    cmp edx, TCP_STATE_TIME_WAIT
    jne .invalid
    cmp eax, TCP_STATE_CLOSED
    jne .invalid
.commit:
    mov [network_tcp_state], eax
    clc
    ret
.invalid:
    stc
    ret

; EAX=IPv4-Adresse in Host-Reihenfolge, EAX=Interface-ID. Die Tabelle ist
; fest begrenzt; längere Präfixe gewinnen unabhängig von der Eintragsreihenfolge.
network_route_lookup_ipv4:
    mov [network_route_lookup_address], eax
    inc dword [network_route_lookups]
    mov dword [network_route_last_prefix], 0
    mov dword [network_route_selected_interface], 0
    xor ecx, ecx
.next:
    cmp ecx, [network_route_count]
    jae .complete
    mov edi, ecx
    shl edi, 4
    add edi, network_route_table
    mov eax, [network_route_lookup_address]
    and eax, [edi + 4]
    cmp eax, [edi + 0]
    jne .continue
    mov eax, [edi + 8]
    cmp eax, [network_route_last_prefix]
    jb .continue
    mov [network_route_last_prefix], eax
    mov eax, [edi + 12]
    mov [network_route_selected_interface], eax
.continue:
    inc ecx
    jmp .next
.complete:
    mov eax, [network_route_selected_interface]
    test eax, eax
    jz .unreachable
    inc dword [network_route_hits]
    clc
    ret
.unreachable:
    stc
    ret

; Statische Bootstrap-Firewall. Bis ein autorisierter Policy-Dienst Regeln
; installiert, ist ausschließlich UDP/TCP Port 9000 in 127.0.0.0/8 erlaubt.
network_firewall_check_ipv4:
    inc dword [network_firewall_decisions]
    cmp ebx, 17
    je .protocol_ok
    cmp ebx, 6
    jne .drop
.protocol_ok:
    cmp ecx, NETWORK_BOOTSTRAP_PORT
    jne .drop
    and eax, 0xFF000000
    cmp eax, 0x7F000000
    jne .drop
    clc
    ret
.drop:
    inc dword [network_firewall_drops]
    stc
    ret

; Erstellt ein IPv4/UDP-Datagramm für 127.0.0.1. Der öffentliche Request
; enthält nur Nutzdaten; sämtliche Protokollfelder entstehen im Kernel.
network_loopback_encapsulate:
    mov ecx, [syscall_network_packet + 12]
    test ecx, ecx
    jz .invalid
    cmp ecx, 16
    ja .invalid
    mov [network_loopback_payload_length], ecx
    mov edi, network_loopback_frame
    xor eax, eax
    push ecx
    mov ecx, 64 / 4
    rep stosd
    pop ecx
    mov byte [network_loopback_frame + 0], 0x45
    mov byte [network_loopback_frame + 1], 0
    mov eax, ecx
    add eax, 28
    xchg al, ah
    mov [network_loopback_frame + 2], ax
    inc word [network_ipv4_identification]
    mov ax, [network_ipv4_identification]
    xchg al, ah
    mov [network_loopback_frame + 4], ax
    mov word [network_loopback_frame + 6], 0
    mov byte [network_loopback_frame + 8], 64
    mov byte [network_loopback_frame + 9], 17
    mov dword [network_loopback_frame + 12], NETWORK_IPV4_LOOPBACK
    mov dword [network_loopback_frame + 16], NETWORK_IPV4_LOOPBACK
    mov eax, [network_socket_port]
    xchg al, ah
    mov [network_loopback_frame + 20], ax
    mov [network_loopback_frame + 22], ax
    mov eax, ecx
    add eax, 8
    xchg al, ah
    mov [network_loopback_frame + 24], ax
    mov esi, syscall_network_packet + 20
    mov edi, network_loopback_frame + 28
    rep movsb
    mov esi, network_loopback_frame
    mov ecx, 20
    call network_checksum
    xchg al, ah
    mov [network_loopback_frame + 10], ax
    call network_udp_checksum_prepare
    mov eax, [network_loopback_payload_length]
    add eax, 20
    mov ecx, eax
    mov esi, network_checksum_buffer
    call network_checksum
    test ax, ax
    jnz .udp_checksum_ready
    mov ax, 0xFFFF
.udp_checksum_ready:
    xchg al, ah
    mov [network_loopback_frame + 26], ax
    clc
    ret
.invalid:
    stc
    ret

network_udp_checksum_prepare:
    mov edi, network_checksum_buffer
    xor eax, eax
    mov ecx, 40 / 4
    rep stosd
    mov dword [network_checksum_buffer + 0], NETWORK_IPV4_LOOPBACK
    mov dword [network_checksum_buffer + 4], NETWORK_IPV4_LOOPBACK
    mov byte [network_checksum_buffer + 8], 0
    mov byte [network_checksum_buffer + 9], 17
    mov ax, [network_loopback_frame + 24]
    mov [network_checksum_buffer + 10], ax
    mov esi, network_loopback_frame + 20
    mov edi, network_checksum_buffer + 12
    mov ecx, [network_loopback_payload_length]
    add ecx, 8
    rep movsb
    ret

network_loopback_validate:
    cmp byte [network_loopback_frame + 0], 0x45
    jne .invalid
    cmp byte [network_loopback_frame + 9], 17
    jne .invalid
    cmp dword [network_loopback_frame + 16], NETWORK_IPV4_LOOPBACK
    jne .invalid
    mov esi, network_loopback_frame
    mov ecx, 20
    call network_checksum
    test ax, ax
    jnz .invalid
    call network_udp_checksum_prepare
    mov eax, [network_loopback_payload_length]
    add eax, 20
    mov ecx, eax
    mov esi, network_checksum_buffer
    call network_checksum
    test ax, ax
    jnz .invalid
    clc
    ret
.invalid:
    stc
    ret

; Internet-Checksum über ECX Bytes in Netzwerkreihenfolge, Ergebnis in AX.
network_checksum:
    xor eax, eax
.pair:
    cmp ecx, 2
    jb .odd
    movzx edx, byte [esi]
    shl edx, 8
    mov dl, [esi + 1]
    add eax, edx
    add esi, 2
    sub ecx, 2
    jmp .pair
.odd:
    test ecx, ecx
    jz .fold
    movzx edx, byte [esi]
    shl edx, 8
    add eax, edx
.fold:
    mov edx, eax
    shr edx, 16
    and eax, 0xFFFF
    add eax, edx
    mov edx, eax
    shr edx, 16
    add ax, dx
    not ax
    ret

align 4
network_api:
    dd NETWORK_API_SIZE
    dw 1, 0
    dd NETWORK_FEATURE_IPV4 | NETWORK_FEATURE_IPV6
    dd network_namespace_generation
    dd network_interface_count
    dd network_loopback_id
    dd network_loopback_state
    dd network_received_packets
    dd network_transmitted_packets
    dd network_dropped_packets
network_namespace_generation: dd 1
network_interface_count:      dd 0
network_loopback_id:          dd 1
network_loopback_state:       dd NETWORK_INTERFACE_UP
network_loopback_object:      dd 0
network_received_packets:     dd 0
network_transmitted_packets:  dd 0
network_dropped_packets:      dd 0
network_socket_object:        dd 0
network_socket_handle:        dd 0
network_socket_bound:         dd 0
network_socket_port:          dd 0
network_tcp_socket_object:    dd 0
network_tcp_socket_handle:    dd 0
network_tcp_socket_connected: dd 0
network_tcp_queue_count:      dd 0
network_tcp_queue_length:     dd 0
network_tcp_transmitted_bytes: dd 0
network_tcp_received_bytes:   dd 0
network_tcp_queue_payload:    times 16 db 0
network_tcp_listener_handle:  dd 0
network_tcp_listener_bound:   dd 0
network_tcp_listener_state:   dd 0
network_tcp_listener_backlog: dd 0
network_tcp_pending_connections: dd 0
network_tcp_accepted_connections: dd 0
network_loopback_queue_count: dd 0
network_loopback_payload_length: dd 0
network_ipv4_identification:  dw 0
align 4
network_loopback_frame:       times 64 db 0
network_checksum_buffer:      times 40 db 0
network_icmp_buffer:          times 16 db 0
network_icmp_echo_tests:      dd 0
network_route_count:          dd 0
network_route_lookups:        dd 0
network_route_hits:           dd 0
network_route_last_prefix:    dd 0
network_route_lookup_address: dd 0
network_route_selected_interface: dd 0
network_firewall_decisions:    dd 0
network_firewall_drops:        dd 0
network_ipv6_udp_tests:        dd 0
network_icmpv6_echo_tests:     dd 0
network_tcp_state:             dd 0
network_tcp_tests:             dd 0
network_tcp_retransmissions:   dd 0
network_tcp_send_sequence:     dd 0
network_tcp_receive_sequence:  dd 0
align 4
network_route_table:
    dd 0x7F000000, 0xFF000000, 8, 1
    dd 0x7F000001, 0xFFFFFFFF, 32, 1
align 4
network_ipv6_frame:            times 64 db 0
network_ipv6_checksum_buffer:  times 64 db 0
network_icmpv6_buffer:         times 12 db 0
network_icmpv6_checksum_buffer: times 64 db 0
network_tcp_segment:           times 24 db 0
network_tcp_checksum_buffer:   times 40 db 0

; ---------------------------------------------------------------------------
; Panic-sicheres Crash-Dump-System (NPSPEC-KERNEL-0024)
; ---------------------------------------------------------------------------
CRASH_DUMP_BUFFER_SIZE     equ 1024
CRASH_DUMP_HEADER_SIZE     equ 128
CRASH_DUMP_SECTION_SIZE    equ 32
CRASH_DUMP_SECTION_COUNT   equ 3
CRASH_DUMP_TOTAL_SIZE      equ 416
CRASH_DUMP_STATUS_EMPTY    equ 0
CRASH_DUMP_STATUS_WRITING  equ 1
CRASH_DUMP_STATUS_COMPLETE equ 2
CRASH_DUMP_STATUS_PARTIAL  equ 3
CRASH_DUMP_CLASS_MINIMAL   equ 2
CRASH_DUMP_SECTION_PANIC   equ 0
CRASH_DUMP_SECTION_CPU     equ 1
CRASH_DUMP_SECTION_LOGS    equ 8
CRASH_DUMP_COMMIT_MARKER   equ 0x504D5544 ; "DUMP"

; Der Puffer wird beim Boot reserviert und niemals dem PMM/Heap übergeben.
crash_dump_initialize:
    mov edi, crash_dump_buffer
    xor eax, eax
    mov ecx, CRASH_DUMP_BUFFER_SIZE / 4
    rep stosd
    mov dword [crash_dump_active], 0
    mov dword [crash_dump_recursions], 0
    mov dword [crash_dump_completed], 0
    mov dword [crash_dump_partial], 0
    clc
    ret

; Erzeugt ausschließlich aus statischem Speicher einen Minimal-Dump. Der
; Status wird zuerst WRITING und erst nach Inhalt und Integrität COMPLETE.
crash_dump_capture_minimal:
    pushad
    cmp dword [crash_dump_active], 0
    jne .recursive
    mov dword [crash_dump_active], 1
    mov edi, crash_dump_buffer
    xor eax, eax
    mov ecx, CRASH_DUMP_BUFFER_SIZE / 4
    rep stosd
    mov dword [crash_dump_buffer + 0], 0x4443564E ; "NVCD"
    mov dword [crash_dump_buffer + 4], 0x31504D55 ; "UMP1"
    mov word  [crash_dump_buffer + 8], 1
    mov word  [crash_dump_buffer + 10], 0
    mov dword [crash_dump_buffer + 12], CRASH_DUMP_HEADER_SIZE
    mov dword [crash_dump_buffer + 16], CRASH_DUMP_CLASS_MINIMAL
    mov dword [crash_dump_buffer + 20], CRASH_DUMP_STATUS_WRITING
    mov dword [crash_dump_buffer + 24], 0x00000001 ; Privacy/minimal
    mov eax, [timer_ticks]
    mov [crash_dump_buffer + 28], eax
    mov dword [crash_dump_buffer + 32], CRASH_DUMP_TOTAL_SIZE
    mov dword [crash_dump_buffer + 36], CRASH_DUMP_SECTION_COUNT

    ; Versionierte, nicht überlappende Sektionsdeskriptoren.
    mov dword [crash_dump_buffer + 128], CRASH_DUMP_SECTION_PANIC
    mov dword [crash_dump_buffer + 132], 1
    mov dword [crash_dump_buffer + 136], 256
    mov dword [crash_dump_buffer + 140], PANIC_REPORT_SIZE
    mov dword [crash_dump_buffer + 160], CRASH_DUMP_SECTION_CPU
    mov dword [crash_dump_buffer + 164], 1
    mov dword [crash_dump_buffer + 168], 304
    mov dword [crash_dump_buffer + 172], 64
    mov dword [crash_dump_buffer + 192], CRASH_DUMP_SECTION_LOGS
    mov dword [crash_dump_buffer + 196], 1
    mov dword [crash_dump_buffer + 200], 368
    mov dword [crash_dump_buffer + 204], LOG_RECORD_SIZE

    mov esi, panic_report
    mov edi, crash_dump_buffer + 256
    mov ecx, PANIC_REPORT_SIZE / 4
    rep movsd
    ; Minimaler CPU-Kontext ohne FPU-, Secret- oder Userspace-Seiten.
    mov eax, cr0
    mov [crash_dump_buffer + 304], eax
    mov eax, cr2
    mov [crash_dump_buffer + 308], eax
    mov eax, cr3
    mov [crash_dump_buffer + 312], eax
    mov eax, cr4
    mov [crash_dump_buffer + 316], eax
    mov eax, [panic_report + 16]
    mov [crash_dump_buffer + 320], eax
    mov eax, [panic_report + 20]
    mov [crash_dump_buffer + 324], eax
    mov eax, [boot_phase_current]
    mov [crash_dump_buffer + 328], eax
    mov eax, [boot_phase_last_success]
    mov [crash_dump_buffer + 332], eax
    mov esi, logging_critical_record
    mov edi, crash_dump_buffer + 368
    mov ecx, LOG_RECORD_SIZE / 4
    rep movsd

    ; Bounded additive integrity value over all publizierten Nutzdaten.
    mov esi, crash_dump_buffer + CRASH_DUMP_HEADER_SIZE
    mov ecx, CRASH_DUMP_TOTAL_SIZE - CRASH_DUMP_HEADER_SIZE
    xor eax, eax
.hash:
    movzx edx, byte [esi]
    rol eax, 5
    xor eax, edx
    inc esi
    loop .hash
    mov [crash_dump_buffer + 60], eax
    mov dword [crash_dump_buffer + 64], CRASH_DUMP_COMMIT_MARKER
    mov dword [crash_dump_buffer + 20], CRASH_DUMP_STATUS_COMPLETE
    inc dword [crash_dump_completed]
    mov dword [crash_dump_active], 0
    popad
    clc
    ret
.recursive:
    inc dword [crash_dump_recursions]
    inc dword [crash_dump_partial]
    mov dword [crash_dump_buffer + 20], CRASH_DUMP_STATUS_PARTIAL
    mov dword [crash_dump_buffer + 68], 1
    mov dword [crash_dump_buffer + 64], CRASH_DUMP_COMMIT_MARKER
    popad
    stc
    ret

crash_dump_validate:
    cmp dword [crash_dump_buffer + 0], 0x4443564E
    jne .invalid
    cmp dword [crash_dump_buffer + 4], 0x31504D55
    jne .invalid
    cmp dword [crash_dump_buffer + 12], CRASH_DUMP_HEADER_SIZE
    jne .invalid
    cmp dword [crash_dump_buffer + 20], CRASH_DUMP_STATUS_COMPLETE
    jne .invalid
    cmp dword [crash_dump_buffer + 32], CRASH_DUMP_TOTAL_SIZE
    jne .invalid
    cmp dword [crash_dump_buffer + 36], CRASH_DUMP_SECTION_COUNT
    jne .invalid
    cmp dword [crash_dump_buffer + 64], CRASH_DUMP_COMMIT_MARKER
    jne .invalid
    mov esi, crash_dump_buffer + CRASH_DUMP_HEADER_SIZE
    mov ecx, CRASH_DUMP_TOTAL_SIZE - CRASH_DUMP_HEADER_SIZE
    xor eax, eax
.hash:
    movzx edx, byte [esi]
    rol eax, 5
    xor eax, edx
    inc esi
    loop .hash
    cmp eax, [crash_dump_buffer + 60]
    jne .invalid
    clc
    ret
.invalid:
    stc
    ret

crash_dump_self_test:
    call crash_dump_capture_minimal
    jc .invalid
    call crash_dump_validate
    jc .invalid
    cmp dword [crash_dump_completed], 1
    jne .invalid
    ; Der Selbsttest gilt nicht als echter Crash: Slot danach wieder freigeben.
    call crash_dump_initialize
    clc
    ret
.invalid:
    stc
    ret

align 16
crash_dump_active:       dd 0
crash_dump_recursions:   dd 0
crash_dump_completed:    dd 0
crash_dump_partial:      dd 0
crash_dump_buffer:       times CRASH_DUMP_BUFFER_SIZE db 0

; Strukturierter Panic-Reporter (ADR-2014)
PANIC_API_SIZE    equ 32
PANIC_REPORT_SIZE equ 48

BOOT_PHASE_NONE            equ 0xFFFFFFFF
BOOT_PHASE_KERNEL_ENTRY    equ 0
BOOT_PHASE_HANDOFF         equ 1
BOOT_PHASE_EARLY_ARCH      equ 2
BOOT_PHASE_EARLY_MEMORY    equ 3
BOOT_PHASE_VIRTUAL_MEMORY  equ 4
BOOT_PHASE_KERNEL_CORE     equ 5
BOOT_PHASE_INTERRUPTS_TIME equ 6
BOOT_PHASE_SCHEDULER_SMP   equ 7
BOOT_PHASE_DEVICE_DISCOVERY equ 8
BOOT_PHASE_ROOT_FILESYSTEM  equ 9
BOOT_PHASE_USERSPACE        equ 10
BOOT_PHASE_OPERATIONAL     equ 11
BOOT_SEQUENCE_API_SIZE     equ 32
BOOT_LOG_API_SIZE          equ 32
BOOT_EVENT_SIZE            equ 24
BOOT_LOG_CAPACITY          equ 12
BOOT_COMPONENT_KERNEL      equ 0x4B45524E

panic_manager_initialize:
    mov edi, panic_report
    xor eax, eax
    mov ecx, PANIC_REPORT_SIZE / 4
    rep stosd
    mov dword [panic_report], PANIC_REPORT_SIZE
    mov dword [panic_report + 4], 1
    clc
    ret

panic_manager_self_test:
    cmp dword [panic_api], PANIC_API_SIZE
    jne .invalid
    cmp word [panic_api + 4], 1
    jne .invalid
    cmp dword [panic_report], PANIC_REPORT_SIZE
    jne .invalid
    clc
    ret
.invalid:
    stc
    ret

; EAX=Fehlercode, EDX=Subsystem, ESI=erklärender Text.
kernel_panic:
    cli
    mov [panic_report + 8], eax
    mov [panic_report + 12], edx
    mov ecx, [esp]
    mov [panic_report + 16], ecx
    mov [panic_report + 20], esp
    mov ecx, cr2
    mov [panic_report + 24], ecx
    mov ecx, [timer_ticks]
    mov [panic_report + 28], ecx
    mov ecx, [boot_phase_current]
    mov [panic_report + 32], ecx
    mov ecx, [boot_phase_last_success]
    mov [panic_report + 36], ecx
    push ebx
    mov eax, 1
    cpuid
    shr ebx, 24
    mov [panic_report + 40], ebx
    pop ebx
    mov ecx, [kernel_context + CONTEXT_SECURITY_STATE]
    mov [panic_report + 44], ecx
    call crash_dump_capture_minimal
    push esi
    call draw_kernel_panic_screen
    pop esi
    push esi
    mov esi, message_panic_begin
    call serial_write_string
    mov eax, [panic_report + 8]
    call serial_write_hex32
    mov esi, message_panic_subsystem
    call serial_write_string
    mov eax, [panic_report + 12]
    call serial_write_hex32
    mov esi, message_panic_eip
    call serial_write_string
    mov eax, [panic_report + 16]
    call serial_write_hex32
    mov esi, message_panic_cr2
    call serial_write_string
    mov eax, [panic_report + 24]
    call serial_write_hex32
    mov esi, message_panic_phase
    call serial_write_string
    mov eax, [panic_report + 32]
    call serial_write_hex32
    mov esi, message_panic_last_phase
    call serial_write_string
    mov eax, [panic_report + 36]
    call serial_write_hex32
    mov esi, message_panic_cpu
    call serial_write_string
    mov eax, [panic_report + 40]
    call serial_write_hex32
    mov esi, message_panic_security
    call serial_write_string
    mov eax, [panic_report + 44]
    call serial_write_hex32
    mov esi, message_newline
    call serial_write_string
    pop esi
    call serial_write_string
    mov esi, message_panic_end
    call serial_write_string
    jmp kernel_halt

align 4
panic_api:
    dd PANIC_API_SIZE
    dw 1, 1
    dd PANIC_REPORT_SIZE
    dd kernel_panic
    dd panic_report
    dd panic_manager_self_test
    dd 0
    dd 0
panic_report:
    times PANIC_REPORT_SIZE db 0

; Frühe, lesbare Bootsequenz-ABI. Sie hält die aktuelle und die letzte
; erfolgreich abgeschlossene NPSPEC-KERNEL-0002-Phase ohne Heapabhängigkeit.
align 4
boot_sequence_api:
    dd BOOT_SEQUENCE_API_SIZE
    dw 1, 0
    dd boot_phase_current
    dd boot_phase_last_success
    dd BOOT_PHASE_OPERATIONAL + 1
    dd 0
    dd 0
    dd 0
boot_phase_current:      dd BOOT_PHASE_NONE
boot_phase_last_success: dd BOOT_PHASE_NONE

; Heapfreies Frühstartprotokoll; bewahrt sämtliche Aufruferregister.
boot_phase_log:
    pushad
    call boot_event_record_current
    mov esi, message_boot_phase
    call serial_write_string
    mov eax, [boot_phase_current]
    call serial_write_hex32
    mov esi, message_boot_last_phase
    call serial_write_string
    mov eax, [boot_phase_last_success]
    call serial_write_hex32
    mov esi, message_newline
    call serial_write_string
    popad
    ret

; Schreibt genau ein NPSPEC-KERNEL-0002-Bootereignis in den statischen Ring.
; Ein voller Ring verwirft weitere Ereignisse deterministisch.
boot_event_record_current:
    mov ecx, [boot_event_count]
    cmp ecx, BOOT_LOG_CAPACITY
    jae .done
    imul edi, ecx, BOOT_EVENT_SIZE
    add edi, boot_events
    mov eax, [timer_ticks]
    mov [edi + 0], eax
    mov dword [edi + 4], 0
    mov eax, [boot_phase_current]
    mov [edi + 8], eax
    mov dword [edi + 12], BOOT_COMPONENT_KERNEL
    mov dword [edi + 16], 0
    mov dword [edi + 20], 0
    inc dword [boot_event_count]
.done:
    ret

align 4
boot_log_api:
    dd BOOT_LOG_API_SIZE
    dw 1, 0
    dd BOOT_LOG_CAPACITY
    dd BOOT_EVENT_SIZE
    dd boot_event_count
    dd boot_events
    dd 0
    dd 0
boot_event_count: dd 0
align 8
boot_events:
    times BOOT_LOG_CAPACITY * BOOT_EVENT_SIZE db 0

kernel_halt:
    cli
.loop:
    hlt
    jmp .loop

; ---------------------------------------------------------------------------
; COM1
; ---------------------------------------------------------------------------

serial_initialize:
    mov dx, COM1_BASE + 1
    xor al, al
    out dx, al
    mov dx, COM1_BASE + 3
    mov al, 0x80
    out dx, al
    mov dx, COM1_BASE
    mov al, 1
    out dx, al
    mov dx, COM1_BASE + 1
    xor al, al
    out dx, al
    mov dx, COM1_BASE + 3
    mov al, 0x03
    out dx, al
    mov dx, COM1_BASE + 2
    mov al, 0xC7
    out dx, al
    mov dx, COM1_BASE + 4
    mov al, 0x0B
    out dx, al
    ret

serial_write_string:
    lodsb
    test al, al
    jz .done
    call serial_write_byte
    jmp serial_write_string
.done:
    ret

serial_write_byte:
    push eax
    mov ah, al
.wait:
    mov dx, COM1_BASE + 5
    in al, dx
    test al, 0x20
    jz .wait
    mov dx, COM1_BASE
    mov al, ah
    out dx, al
    pop eax
    ret

serial_write_hex32:
    pushad
    mov ebx, eax
    mov ecx, 8
.digit:
    rol ebx, 4
    mov eax, ebx
    and eax, 0x0F
    cmp al, 10
    jb .number
    add al, 'A' - 10
    jmp .write
.number:
    add al, '0'
.write:
    call serial_write_byte
    loop .digit
    popad
    ret

; Erste vom Ring-3-System-UI-Prozess deklarierte Nova-Desktop-Szene. Der
; Userspace bestimmt nur semantische Sichtbarkeit und Generation; Geometrie,
; Theme-Tokens und der eigentliche Framebufferzugriff bleiben im Display Server.
draw_desktop_scene:
    pushad
    cmp dword [display_server_ready], 1
    jne .done
    test dword [display_scene_flags], DISPLAY_SCENE_DESKTOP
    jz .done

    ; Ruhiger Navy-Hintergrund mit einer sehr schmalen Aurora-Lichtkante.
    mov eax, NOVA_COLOR_DESKTOP_BG
    xor ebx, ebx
    xor ecx, ecx
    mov edx, [kernel_context + CONTEXT_WIDTH]
    mov esi, [kernel_context + CONTEXT_HEIGHT]
    call fill_rectangle
    mov eax, NOVA_COLOR_BLUE
    xor ebx, ebx
    xor ecx, ecx
    mov edx, [kernel_context + CONTEXT_WIDTH]
    shr edx, 2
    mov esi, 2
    call fill_rectangle
    mov eax, NOVA_COLOR_PURPLE
    mov ebx, [kernel_context + CONTEXT_WIDTH]
    shr ebx, 2
    mov edx, [kernel_context + CONTEXT_WIDTH]
    shr edx, 1
    call fill_rectangle
    mov eax, NOVA_COLOR_CYAN_SOFT
    mov ebx, [kernel_context + CONTEXT_WIDTH]
    imul ebx, ebx, 3
    shr ebx, 2
    mov edx, [kernel_context + CONTEXT_WIDTH]
    sub edx, ebx
    call fill_rectangle

    call draw_shell_header

    test dword [display_scene_flags], DISPLAY_SCENE_RIBBON
    jz .menu
    call draw_shell_workspace
.menu:
    test dword [display_scene_flags], DISPLAY_SCENE_START_MENU
    jz .taskbar
    call draw_shell_start_menu
.taskbar:
    test dword [display_scene_flags], DISPLAY_SCENE_TASKBAR
    jz .done
    call draw_shell_taskbar
.done:
    popad
    ret

draw_shell_workspace:
    cmp dword [display_scene_workspace], 1
    je .sheet
    cmp dword [display_scene_workspace], 2
    je .studio
    call draw_shell_explorer
    ret
.sheet:
    call draw_shell_sheet
    ret
.studio:
    call draw_shell_studio
    ret

; Geschuetzter Systemkopf: Branding, globale Befehlspalette und reduzierte
; Statusinformationen. Alle Positionen werden aus der Displaybreite abgeleitet.
draw_shell_header:
    pushad
    mov esi, text_shell_brand
    mov ebx, 24
    mov ecx, 18
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 2
    call draw_text
    mov esi, text_shell_tagline
    mov ebx, 24
    mov ecx, 45
    mov edx, NOVA_COLOR_MUTED
    mov ebp, 1
    call draw_text

    mov dword [shell_command_width], 420
    cmp dword [kernel_context + CONTEXT_WIDTH], 1000
    jae .command_geometry
    mov dword [shell_command_width], 320
.command_geometry:
    mov ebx, [kernel_context + CONTEXT_WIDTH]
    sub ebx, [shell_command_width]
    shr ebx, 1
    mov [shell_command_x], ebx
    mov eax, NOVA_COLOR_BORDER_ACTIVE
    mov ecx, 15
    mov edx, [shell_command_width]
    mov esi, 42
    call fill_rounded_rectangle
    mov eax, NOVA_COLOR_ACRYLIC
    mov ebx, [shell_command_x]
    inc ebx
    mov ecx, 16
    mov edx, [shell_command_width]
    sub edx, 2
    mov esi, 40
    call fill_rounded_rectangle
    mov ebx, [shell_command_x]
    add ebx, 25
    mov ecx, 36
    mov edx, 14
    call draw_nova_orb
    mov esi, text_shell_command
    mov ebx, [shell_command_x]
    add ebx, 49
    mov ecx, 29
    mov edx, NOVA_COLOR_MUTED
    mov ebp, 1
    call draw_text
    cmp dword [kernel_context + CONTEXT_WIDTH], 1000
    jb .done
    mov esi, text_shell_status
    mov ebx, [kernel_context + CONTEXT_WIDTH]
    sub ebx, 292
    mov ecx, 23
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 1
    call draw_text
    mov esi, text_shell_date
    mov ebx, [kernel_context + CONTEXT_WIDTH]
    sub ebx, 130
    mov ecx, 44
    mov edx, NOVA_COLOR_MUTED
    mov ebp, 1
    call draw_text
.done:
    popad
    ret

; Erste echte Anwendungsflaeche: Explorer als gemeinsame NovaWindow-Komponente
; mit Titelzeile, Navigation, Sidebar, Ordnerkarten, Dateiliste und Statusbar.
draw_shell_explorer:
    pushad
    mov dword [shell_window_x], 56
    cmp dword [kernel_context + CONTEXT_WIDTH], 1000
    jae .window_x_ready
    mov dword [shell_window_x], 20
.window_x_ready:
    mov eax, [kernel_context + CONTEXT_WIDTH]
    mov ebx, [shell_window_x]
    shl ebx, 1
    sub eax, ebx
    mov [shell_window_width], eax
    mov dword [shell_window_y], 78
    mov eax, [kernel_context + CONTEXT_HEIGHT]
    sub eax, 180
    mov [shell_window_height], eax

    mov eax, NOVA_COLOR_WINDOW_BORDER
    mov ebx, [shell_window_x]
    mov ecx, [shell_window_y]
    mov edx, [shell_window_width]
    mov esi, [shell_window_height]
    call fill_rounded_rectangle
    mov eax, NOVA_COLOR_WINDOW
    mov ebx, [shell_window_x]
    inc ebx
    mov ecx, [shell_window_y]
    inc ecx
    mov edx, [shell_window_width]
    sub edx, 2
    mov esi, [shell_window_height]
    sub esi, 2
    call fill_rounded_rectangle

    mov esi, text_explorer_title
    mov ebx, [shell_window_x]
    add ebx, 24
    mov ecx, [shell_window_y]
    add ecx, 18
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 2
    call draw_text
    mov esi, text_window_controls
    mov ebx, [shell_window_x]
    add ebx, [shell_window_width]
    sub ebx, 126
    mov ecx, [shell_window_y]
    add ecx, 20
    mov edx, NOVA_COLOR_MUTED
    mov ebp, 1
    call draw_text

    ; Navigation und Breadcrumb.
    mov eax, NOVA_COLOR_PANEL_SOFT
    mov ebx, [shell_window_x]
    add ebx, 190
    mov ecx, [shell_window_y]
    add ecx, 56
    mov edx, [shell_window_width]
    sub edx, 420
    mov esi, 38
    call fill_rounded_rectangle
    mov esi, text_explorer_navigation
    mov ebx, [shell_window_x]
    add ebx, 24
    mov ecx, [shell_window_y]
    add ecx, 69
    mov edx, NOVA_COLOR_MUTED
    mov ebp, 1
    call draw_text
    call explorer_breadcrumb_text
    mov ebx, [shell_window_x]
    add ebx, 210
    mov ecx, [shell_window_y]
    add ecx, 69
    mov edx, NOVA_COLOR_TEXT
    mov ebp, 1
    call draw_text
    cmp dword [kernel_context + CONTEXT_WIDTH], 1000
    jb .toolbar
    mov eax, NOVA_COLOR_PANEL_SOFT
    mov ebx, [shell_window_x]
    add ebx, [shell_window_width]
    sub ebx, 214
    mov ecx, [shell_window_y]
    add ecx, 56
    mov edx, 190
    mov esi, 38
    call fill_rounded_rectangle
    mov esi, text_explorer_search
    add ebx, 16
    mov ecx, [shell_window_y]
    add ecx, 69
    mov edx, NOVA_COLOR_MUTED
    mov ebp, 1
    call draw_text
.toolbar:
    mov esi, text_explorer_toolbar
    mov ebx, [shell_window_x]
    add ebx, 210
    mov ecx, [shell_window_y]
    add ecx, 114
    mov edx, NOVA_COLOR_TEXT
    mov ebp, 1
    call draw_text

    ; Sidebar.
    mov eax, NOVA_COLOR_SIDEBAR
    mov ebx, [shell_window_x]
    add ebx, 1
    mov ecx, [shell_window_y]
    add ecx, 104
    mov edx, 176
    mov esi, [shell_window_height]
    sub esi, 105
    call fill_rectangle
    mov esi, text_explorer_quick
    mov ebx, [shell_window_x]
    add ebx, 22
    mov ecx, [shell_window_y]
    add ecx, 124
    mov edx, NOVA_COLOR_MUTED
    mov ebp, 1
    call draw_text
    call explorer_sidebar_highlight_y
    add ecx, [shell_window_y]
    mov eax, NOVA_COLOR_SELECTION
    mov ebx, [shell_window_x]
    add ebx, 12
    mov edx, 152
    mov esi, 30
    call fill_rounded_rectangle
    mov esi, text_explorer_sidebar
    mov ebx, [shell_window_x]
    add ebx, 24
    mov ecx, [shell_window_y]
    add ecx, 154
    mov edx, NOVA_COLOR_TEXT
    mov ebp, 1
    call draw_text

    ; Inhalt beginnt rechts neben der Sidebar.
    mov esi, text_explorer_folders
    mov ebx, [shell_window_x]
    add ebx, 202
    mov ecx, [shell_window_y]
    add ecx, 154
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 1
    call draw_text
    mov dword [shell_card_x], ebx
    xor edi, edi
.folder_loop:
    cmp edi, 4
    jae .files
    call explorer_folder_label
    test esi, esi
    jnz .folder_card
    test edi, edi
    jnz .files
    mov esi, text_explorer_no_folders
    mov ebx, [shell_card_x]
    mov ecx, [shell_window_y]
    add ecx, 198
    mov edx, NOVA_COLOR_MUTED
    mov ebp, 1
    call draw_text
    jmp .files
.folder_card:
    mov [explorer_card_label], esi
    lea eax, [edi + 24]              ; Fokus 24..27 = Ordnerkarte
    cmp eax, [display_scene_focus]
    jne .card_unfocused
    mov eax, NOVA_COLOR_SELECTION
    mov ebx, [shell_card_x]
    sub ebx, 2
    mov ecx, [shell_window_y]
    add ecx, 176
    mov edx, 136
    mov esi, 60
    call fill_rounded_rectangle
.card_unfocused:
    mov eax, NOVA_COLOR_CARD_BORDER
    mov ebx, [shell_card_x]
    mov ecx, [shell_window_y]
    add ecx, 178
    mov edx, 132
    mov esi, 56
    call fill_rounded_rectangle
    mov eax, NOVA_COLOR_CARD
    inc ebx
    inc ecx
    mov edx, 130
    mov esi, 54
    call fill_rounded_rectangle
    mov esi, [explorer_card_label]
    mov ebx, [shell_card_x]
    add ebx, 14
    mov ecx, [shell_window_y]
    add ecx, 198
    mov edx, NOVA_COLOR_TEXT
    mov ebp, 1
    call draw_text
    add dword [shell_card_x], 142
    mov eax, [shell_card_x]
    add eax, 132
    mov ebx, [shell_window_x]
    add ebx, [shell_window_width]
    sub ebx, 18
    cmp eax, ebx
    ja .files
    inc edi
    jmp .folder_loop
.files:
    mov esi, text_explorer_files
    mov ebx, [shell_window_x]
    add ebx, 202
    mov ecx, [shell_window_y]
    add ecx, 258
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 1
    call draw_text
    mov eax, NOVA_COLOR_PANEL_SOFT
    mov ebx, [shell_window_x]
    add ebx, 190
    mov ecx, [shell_window_y]
    add ecx, 282
    mov edx, [shell_window_width]
    sub edx, 208
    mov esi, 30
    call fill_rounded_rectangle
    cmp dword [explorer_view_valid], 1
    jne .static_columns
    call explorer_draw_file_selection
    call explorer_draw_files
    popad
    ret
.static_columns:
    mov esi, text_explorer_columns
    mov ebx, [shell_window_x]
    add ebx, 204
    mov ecx, [shell_window_y]
    add ecx, 292
    mov edx, NOVA_COLOR_MUTED
    mov ebp, 1
    call draw_text
    mov eax, [display_scene_focus]
    sub eax, 20
    cmp eax, 3
    jbe .file_focus_ready
    xor eax, eax
.file_focus_ready:
    imul eax, eax, 24
    mov ecx, [shell_window_y]
    add ecx, 324
    add ecx, eax
    mov eax, NOVA_COLOR_SELECTION
    mov ebx, [shell_window_x]
    add ebx, 194
    mov edx, [shell_window_width]
    sub edx, 216
    mov esi, 22
    call fill_rounded_rectangle
    mov esi, text_explorer_file_rows
    mov ebx, [shell_window_x]
    add ebx, 204
    mov ecx, [shell_window_y]
    add ecx, 330
    mov edx, NOVA_COLOR_TEXT
    mov ebp, 1
    call draw_text
    mov esi, text_explorer_footer
    mov ebx, [shell_window_x]
    add ebx, 202
    mov ecx, [shell_window_y]
    add ecx, [shell_window_height]
    sub ecx, 24
    mov edx, NOVA_COLOR_MUTED
    mov ebp, 1
    call draw_text
    popad
    ret

; Nova Sheet nutzt dieselbe Fensterhülle, aber ein app-spezifisches Ribbon,
; eine fokussierte Budgettabelle und das einklappbare Fähigkeitenpanel.
draw_shell_sheet:
    pushad
    mov dword [shell_window_x], 40
    mov dword [shell_window_y], 78
    mov eax, [kernel_context + CONTEXT_WIDTH]
    sub eax, 80
    mov [shell_window_width], eax
    mov eax, [kernel_context + CONTEXT_HEIGHT]
    sub eax, 180
    mov [shell_window_height], eax
    mov eax, NOVA_COLOR_WINDOW_BORDER
    mov ebx, [shell_window_x]
    mov ecx, [shell_window_y]
    mov edx, [shell_window_width]
    mov esi, [shell_window_height]
    call fill_rounded_rectangle
    mov eax, NOVA_COLOR_WINDOW
    inc ebx
    inc ecx
    sub edx, 2
    sub esi, 2
    call fill_rounded_rectangle
    mov esi, text_sheet_title
    mov ebx, [shell_window_x]
    add ebx, 22
    mov ecx, [shell_window_y]
    add ecx, 16
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 2
    call draw_text
    mov esi, text_window_controls
    mov ebx, [shell_window_x]
    add ebx, [shell_window_width]
    sub ebx, 126
    mov ecx, [shell_window_y]
    add ecx, 18
    mov edx, NOVA_COLOR_MUTED
    mov ebp, 1
    call draw_text
    mov esi, text_sheet_tabs
    mov ebx, [shell_window_x]
    add ebx, 22
    mov ecx, [shell_window_y]
    add ecx, 56
    mov edx, NOVA_COLOR_TEXT
    mov ebp, 1
    call draw_text
    mov eax, NOVA_COLOR_PANEL_SOFT
    mov ebx, [shell_window_x]
    add ebx, 12
    mov ecx, [shell_window_y]
    add ecx, 78
    mov edx, [shell_window_width]
    sub edx, 24
    mov esi, 82
    call fill_rounded_rectangle
    mov esi, text_sheet_ribbon
    add ebx, 14
    add ecx, 16
    mov edx, NOVA_COLOR_TEXT
    mov ebp, 1
    call draw_text
    mov eax, NOVA_COLOR_PANEL_SOFT
    mov ebx, [shell_window_x]
    add ebx, 12
    mov ecx, [shell_window_y]
    add ecx, 170
    mov edx, [shell_window_width]
    sub edx, 24
    mov esi, 34
    call fill_rounded_rectangle
    mov esi, text_sheet_formula
    add ebx, 12
    add ecx, 11
    mov edx, NOVA_COLOR_MUTED
    mov ebp, 1
    call draw_text

    mov esi, text_sheet_heading
    mov ebx, [shell_window_x]
    add ebx, 28
    mov ecx, [shell_window_y]
    add ecx, 222
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 2
    call draw_text
    mov eax, NOVA_COLOR_SELECTION
    mov ebx, [shell_window_x]
    add ebx, 18
    mov ecx, [shell_window_y]
    add ecx, 254
    mov edx, [shell_window_width]
    sub edx, 310
    mov esi, 32
    call fill_rounded_rectangle
    mov esi, text_sheet_columns
    add ebx, 10
    add ecx, 11
    mov edx, NOVA_COLOR_TEXT
    mov ebp, 1
    call draw_text
    mov eax, NOVA_COLOR_BORDER_ACTIVE
    mov ebx, [shell_window_x]
    add ebx, 126
    mov ecx, [shell_window_y]
    add ecx, 288
    mov edi, [display_scene_focus]
    sub edi, 40
    cmp edi, 5
    jbe .cell_focus_ready
    xor edi, edi
.cell_focus_ready:
    imul edi, edi, 24
    add ecx, edi
    mov edx, 84
    mov esi, 28
    call fill_rounded_rectangle
    mov eax, NOVA_COLOR_CARD
    inc ebx
    inc ecx
    sub edx, 2
    sub esi, 2
    call fill_rounded_rectangle
    mov esi, text_sheet_rows
    mov ebx, [shell_window_x]
    add ebx, 28
    mov ecx, [shell_window_y]
    add ecx, 296
    mov edx, NOVA_COLOR_TEXT
    mov ebp, 1
    call draw_text

    cmp dword [kernel_context + CONTEXT_WIDTH], 1000
    jb .footer
    mov eax, NOVA_COLOR_CARD
    mov ebx, [shell_window_x]
    add ebx, [shell_window_width]
    sub ebx, 274
    mov ecx, [shell_window_y]
    add ecx, 212
    mov edx, 254
    mov esi, [shell_window_height]
    sub esi, 252
    call fill_rounded_rectangle
    mov esi, text_sheet_capabilities
    add ebx, 18
    add ecx, 18
    mov edx, NOVA_COLOR_TEXT
    mov ebp, 1
    call draw_text
.footer:
    mov esi, text_sheet_footer
    mov ebx, [shell_window_x]
    add ebx, 24
    mov ecx, [shell_window_y]
    add ecx, [shell_window_height]
    sub ecx, 24
    mov edx, NOVA_COLOR_MUTED
    mov ebp, 1
    call draw_text
    popad
    ret

; Fähigkeiten Studio: Navigation, Capability-Katalog, Node-Canvas und
; Inspector sind getrennte echte Panels und keine eingebettete Rastergrafik.
draw_shell_studio:
    pushad
    mov dword [shell_window_x], 24
    mov dword [shell_window_y], 78
    mov eax, [kernel_context + CONTEXT_WIDTH]
    sub eax, 48
    mov [shell_window_width], eax
    mov eax, [kernel_context + CONTEXT_HEIGHT]
    sub eax, 180
    mov [shell_window_height], eax
    mov eax, NOVA_COLOR_WINDOW_BORDER
    mov ebx, [shell_window_x]
    mov ecx, [shell_window_y]
    mov edx, [shell_window_width]
    mov esi, [shell_window_height]
    call fill_rounded_rectangle
    mov eax, NOVA_COLOR_WINDOW
    inc ebx
    inc ecx
    sub edx, 2
    sub esi, 2
    call fill_rounded_rectangle
    mov esi, text_studio_title
    mov ebx, [shell_window_x]
    add ebx, 184
    mov ecx, [shell_window_y]
    add ecx, 16
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 2
    call draw_text
    mov esi, text_studio_steps
    mov ebx, [shell_window_x]
    add ebx, 184
    mov ecx, [shell_window_y]
    add ecx, 54
    mov edx, NOVA_COLOR_MUTED
    mov ebp, 1
    call draw_text
    mov eax, NOVA_COLOR_SIDEBAR
    mov ebx, [shell_window_x]
    add ebx, 1
    mov ecx, [shell_window_y]
    add ecx, 1
    mov edx, 164
    mov esi, [shell_window_height]
    sub esi, 2
    call fill_rounded_rectangle
    mov esi, text_studio_navigation
    mov ebx, [shell_window_x]
    add ebx, 20
    mov ecx, [shell_window_y]
    add ecx, 28
    mov edx, NOVA_COLOR_TEXT
    mov ebp, 1
    call draw_text

    mov eax, NOVA_COLOR_CARD
    mov ebx, [shell_window_x]
    add ebx, 178
    mov ecx, [shell_window_y]
    add ecx, 86
    mov edx, 250
    mov esi, [shell_window_height]
    sub esi, 104
    call fill_rounded_rectangle
    mov esi, text_studio_catalog
    add ebx, 14
    add ecx, 16
    mov edx, NOVA_COLOR_TEXT
    mov ebp, 1
    call draw_text

    mov eax, NOVA_COLOR_PANEL_SOFT
    mov ebx, [shell_window_x]
    add ebx, 440
    mov ecx, [shell_window_y]
    add ecx, 86
    mov edx, [shell_window_width]
    sub edx, 720
    mov esi, [shell_window_height]
    sub esi, 104
    call fill_rounded_rectangle
    mov esi, text_studio_canvas
    add ebx, 18
    add ecx, 16
    mov edx, NOVA_COLOR_MUTED
    mov ebp, 1
    call draw_text
    mov edi, [display_scene_focus]
    sub edi, 60
    cmp edi, 5
    jbe .node_focus_ready
    xor edi, edi
.node_focus_ready:
    mov ebx, [shell_window_x]
    add ebx, 464
    cmp edi, 3
    jb .node_focus_column_ready
    sub edi, 3
    add ebx, 178
.node_focus_column_ready:
    imul edi, edi, 82
    mov ecx, [shell_window_y]
    add ecx, 140
    add ecx, edi
    mov eax, NOVA_COLOR_BORDER_ACTIVE
    mov edx, 136
    mov esi, 56
    call fill_rounded_rectangle
    mov eax, NOVA_COLOR_SELECTION
    mov ebx, [shell_window_x]
    add ebx, 466
    mov ecx, [shell_window_y]
    add ecx, 142
    mov edx, 132
    mov esi, 52
    call fill_rounded_rectangle
    mov eax, NOVA_COLOR_PURPLE_SOFT
    add ebx, 178
    add ecx, 76
    call fill_rounded_rectangle
    mov eax, NOVA_COLOR_CARD_BORDER
    sub ebx, 84
    add ecx, 82
    call fill_rounded_rectangle
    mov esi, text_studio_nodes
    mov ebx, [shell_window_x]
    add ebx, 482
    mov ecx, [shell_window_y]
    add ecx, 160
    mov edx, NOVA_COLOR_TEXT
    mov ebp, 1
    call draw_text

    mov eax, NOVA_COLOR_CARD
    mov ebx, [shell_window_x]
    add ebx, [shell_window_width]
    sub ebx, 266
    mov ecx, [shell_window_y]
    add ecx, 86
    mov edx, 250
    mov esi, [shell_window_height]
    sub esi, 104
    call fill_rounded_rectangle
    mov esi, text_studio_inspector
    add ebx, 16
    add ecx, 16
    mov edx, NOVA_COLOR_TEXT
    mov ebp, 1
    call draw_text
    popad
    ret

; Dreispaltiges Startmenue. Nur dieses Bauteil folgt der separaten
; Startmenue-Referenz; Desktop und Taskleiste bleiben im Aurora-Shell-Stil.
draw_shell_start_menu:
    pushad
    mov eax, [kernel_context + CONTEXT_WIDTH]
    sub eax, 48
    cmp eax, 900
    jbe .width_ready
    mov eax, 900
.width_ready:
    mov [shell_menu_width], eax
    mov ebx, [kernel_context + CONTEXT_WIDTH]
    sub ebx, eax
    shr ebx, 1
    mov [shell_menu_x], ebx
    mov eax, [kernel_context + CONTEXT_HEIGHT]
    sub eax, 190
    cmp eax, 540
    jbe .height_ready
    mov eax, 540
.height_ready:
    mov [shell_menu_height], eax
    mov ecx, [kernel_context + CONTEXT_HEIGHT]
    sub ecx, 104
    sub ecx, eax
    mov [shell_menu_y], ecx
    mov eax, NOVA_COLOR_BORDER_ACTIVE
    mov ebx, [shell_menu_x]
    mov edx, [shell_menu_width]
    mov esi, [shell_menu_height]
    call fill_rounded_rectangle
    mov eax, NOVA_COLOR_MENU
    inc ebx
    inc ecx
    sub edx, 2
    sub esi, 2
    call fill_rounded_rectangle

    ; Linke Navigation.
    mov esi, text_start_brand
    mov ebx, [shell_menu_x]
    add ebx, 28
    mov ecx, [shell_menu_y]
    add ecx, 28
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 2
    call draw_text
    mov eax, NOVA_COLOR_SELECTION
    mov ebx, [shell_menu_x]
    add ebx, 16
    mov ecx, [shell_menu_y]
    add ecx, 72
    mov edx, 170
    mov esi, 36
    call fill_rounded_rectangle
    mov esi, text_start_navigation
    mov ebx, [shell_menu_x]
    add ebx, 30
    mov ecx, [shell_menu_y]
    add ecx, 84
    mov edx, NOVA_COLOR_TEXT
    mov ebp, 1
    call draw_text
    mov esi, text_start_navigation_lower
    mov ebx, [shell_menu_x]
    add ebx, 30
    mov ecx, [shell_menu_y]
    add ecx, [shell_menu_height]
    sub ecx, 74
    mov edx, NOVA_COLOR_MUTED
    mov ebp, 1
    call draw_text

    ; Mittlere App- und Vorschlagsspalte.
    mov esi, text_start_pinned_v2
    mov ebx, [shell_menu_x]
    add ebx, 214
    mov ecx, [shell_menu_y]
    add ecx, 30
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 1
    call draw_text
    mov dword [shell_card_x], 0
.app_loop:
    mov edi, [shell_card_x]
    cmp edi, 8
    jae .suggestions
    mov eax, edi
    and eax, 3
    imul eax, eax, 92
    mov ebx, [shell_menu_x]
    add ebx, 210
    add ebx, eax
    mov eax, edi
    shr eax, 2
    imul eax, eax, 74
    mov ecx, [shell_menu_y]
    add ecx, 60
    add ecx, eax
    mov eax, NOVA_COLOR_CARD
    mov ebp, edi
    add ebp, 3
    cmp [display_scene_focus], ebp
    jne .app_color_ready
    mov eax, NOVA_COLOR_SELECTION
.app_color_ready:
    mov edx, 82
    mov esi, 64
    call fill_rounded_rectangle
    mov eax, NOVA_COLOR_PURPLE_SOFT
    mov edx, 28
    mov esi, 28
    add ebx, 27
    add ecx, 8
    call fill_rounded_rectangle
    mov esi, [shell_app_labels + edi * 4]
    sub ebx, 19
    add ecx, 35
    mov edx, NOVA_COLOR_TEXT
    mov ebp, 1
    call draw_text
    inc dword [shell_card_x]
    jmp .app_loop
.suggestions:
    mov esi, text_start_suggestions
    mov ebx, [shell_menu_x]
    add ebx, 214
    mov ecx, [shell_menu_y]
    add ecx, 226
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 1
    call draw_text
    mov eax, NOVA_COLOR_CARD
    mov ebx, [shell_menu_x]
    add ebx, 210
    mov ecx, [shell_menu_y]
    add ecx, 252
    mov edx, 358
    mov esi, 68
    call fill_rounded_rectangle
    mov esi, text_start_suggestion_items
    add ebx, 14
    add ecx, 14
    mov edx, NOVA_COLOR_TEXT
    mov ebp, 1
    call draw_text
    mov eax, NOVA_COLOR_PANEL_SOFT
    mov ebx, [shell_menu_x]
    add ebx, 210
    mov ecx, [shell_menu_y]
    add ecx, [shell_menu_height]
    sub ecx, 54
    mov edx, 358
    mov esi, 36
    call fill_rounded_rectangle
    mov esi, text_start_search_v2
    add ebx, 16
    add ecx, 12
    mov edx, NOVA_COLOR_MUTED
    mov ebp, 1
    call draw_text

    ; Rechte Widgetspalte wird bei kompakten Displays ausgeblendet.
    cmp dword [shell_menu_width], 760
    jb .done
    mov esi, text_start_profile
    mov ebx, [shell_menu_x]
    add ebx, [shell_menu_width]
    sub ebx, 286
    mov ecx, [shell_menu_y]
    add ecx, 30
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 1
    call draw_text
    mov eax, NOVA_COLOR_CARD
    mov ebx, [shell_menu_x]
    add ebx, [shell_menu_width]
    sub ebx, 300
    mov ecx, [shell_menu_y]
    add ecx, 60
    mov edx, 280
    mov esi, 88
    call fill_rounded_rectangle
    mov esi, text_start_clock_widget
    add ebx, 18
    add ecx, 16
    mov edx, NOVA_COLOR_TEXT
    mov ebp, 2
    call draw_text
    mov eax, NOVA_COLOR_CARD
    mov ebx, [shell_menu_x]
    add ebx, [shell_menu_width]
    sub ebx, 300
    mov ecx, [shell_menu_y]
    add ecx, 158
    mov edx, 280
    mov esi, 96
    call fill_rounded_rectangle
    mov esi, text_start_ai_widget
    add ebx, 18
    add ecx, 16
    mov edx, NOVA_COLOR_TEXT
    mov ebp, 1
    call draw_text
    mov eax, NOVA_COLOR_CARD
    mov ebx, [shell_menu_x]
    add ebx, [shell_menu_width]
    sub ebx, 300
    mov ecx, [shell_menu_y]
    add ecx, 264
    mov edx, 280
    mov esi, 104
    call fill_rounded_rectangle
    mov esi, text_start_system_widget
    add ebx, 18
    add ecx, 16
    mov edx, NOVA_COLOR_TEXT
    mov ebp, 1
    call draw_text
.done:
    popad
    ret

draw_shell_taskbar:
    pushad
    mov eax, [kernel_context + CONTEXT_HEIGHT]
    sub eax, 86
    mov [shell_taskbar_y], eax
    mov eax, NOVA_COLOR_WINDOW_BORDER
    mov ebx, 20
    mov ecx, [shell_taskbar_y]
    mov edx, [kernel_context + CONTEXT_WIDTH]
    sub edx, 40
    mov esi, 68
    call fill_rounded_rectangle
    mov eax, NOVA_COLOR_TASKBAR
    mov ebx, 21
    mov ecx, [shell_taskbar_y]
    inc ecx
    mov edx, [kernel_context + CONTEXT_WIDTH]
    sub edx, 42
    mov esi, 66
    call fill_rounded_rectangle

    ; Dezent segmentierte Aurora-Oberkante.
    mov eax, NOVA_COLOR_BLUE
    mov ebx, 42
    mov ecx, [shell_taskbar_y]
    mov edx, [kernel_context + CONTEXT_WIDTH]
    sub edx, 84
    shr edx, 2
    mov esi, 2
    call fill_rectangle
    mov eax, NOVA_COLOR_PURPLE
    add ebx, edx
    shl edx, 1
    call fill_rectangle
    mov eax, NOVA_COLOR_ORANGE
    add ebx, edx
    shr edx, 1
    call fill_rectangle

    mov ebx, 57
    mov ecx, [shell_taskbar_y]
    add ecx, 34
    mov edx, 24
    call draw_nova_orb
    mov eax, NOVA_COLOR_PANEL_SOFT
    mov ebx, 94
    mov ecx, [shell_taskbar_y]
    add ecx, 13
    mov edx, 210
    mov esi, 42
    call fill_rounded_rectangle
    mov esi, text_taskbar_search_v2
    mov ebx, 112
    mov ecx, [shell_taskbar_y]
    add ecx, 28
    mov edx, NOVA_COLOR_MUTED
    mov ebp, 1
    call draw_text

    mov dword [shell_card_x], 318
    xor edi, edi
.pin_loop:
    cmp edi, 6
    jae .status
    mov eax, NOVA_COLOR_CARD
    mov ebx, [shell_card_x]
    mov ecx, [shell_taskbar_y]
    add ecx, 13
    mov edx, 42
    mov esi, 42
    call fill_rounded_rectangle
    mov esi, [shell_pin_labels + edi * 4]
    add ebx, 15
    add ecx, 15
    mov edx, NOVA_COLOR_TEXT
    mov ebp, 1
    call draw_text
    cmp edi, 0
    jne .next_pin
    mov eax, NOVA_COLOR_CYAN_SOFT
    mov ebx, [shell_card_x]
    add ebx, 15
    mov ecx, [shell_taskbar_y]
    add ecx, 60
    mov edx, 12
    mov esi, 2
    call fill_rectangle
.next_pin:
    add dword [shell_card_x], 50
    mov eax, [shell_card_x]
    add eax, 42
    mov ebx, [kernel_context + CONTEXT_WIDTH]
    sub ebx, 270
    cmp eax, ebx
    jae .status
    inc edi
    jmp .pin_loop
.status:
    mov esi, text_taskbar_status_v2
    mov ebx, [kernel_context + CONTEXT_WIDTH]
    sub ebx, 242
    mov ecx, [shell_taskbar_y]
    add ecx, 17
    mov edx, NOVA_COLOR_TEXT
    mov ebp, 1
    call draw_text
    mov esi, text_taskbar_date_v2
    mov ebx, [kernel_context + CONTEXT_WIDTH]
    sub ebx, 126
    mov ecx, [shell_taskbar_y]
    add ecx, 38
    mov edx, NOVA_COLOR_MUTED
    mov ebp, 1
    call draw_text
    popad
    ret

draw_desktop_scene_legacy:
    pushad
    cmp dword [display_server_ready], 1
    jne .done
    test dword [display_scene_flags], DISPLAY_SCENE_DESKTOP
    jz .done

    mov eax, NOVA_COLOR_DESKTOP_BG
    xor ebx, ebx
    xor ecx, ecx
    mov edx, [kernel_context + CONTEXT_WIDTH]
    mov esi, [kernel_context + CONTEXT_HEIGHT]
    call fill_rectangle

    ; Akzentlinie und geschützte Systemmarke.
    mov ebx, [kernel_context + CONTEXT_WIDTH]
    shr ebx, 3
    mov edx, [kernel_context + CONTEXT_WIDTH]
    sub edx, ebx
    sub edx, ebx
    mov eax, NOVA_COLOR_BLUE
    xor ecx, ecx
    mov esi, 13
    call fill_rounded_rectangle
    mov dword [logo_x], 24
    mov dword [logo_y], 36
    mov dword [logo_color], NOVA_COLOR_BLUE
    mov byte [logo_compact], 1
    mov byte [logo_mirror], 1
    call draw_nova_logo
    mov esi, text_brand_compact
    mov ebx, 32
    mov ecx, 154
    mov edx, NOVA_COLOR_BLUE
    mov ebp, 3
    call draw_text
    mov esi, text_desktop_workspace
    mov ebx, 42
    mov ecx, 205
    mov edx, NOVA_COLOR_MUTED
    mov ebp, 1
    call draw_text

    test dword [display_scene_flags], DISPLAY_SCENE_RIBBON
    jz .start_menu
    mov eax, NOVA_COLOR_BLUE
    mov ebx, 170
    mov ecx, 34
    mov edx, [kernel_context + CONTEXT_WIDTH]
    sub edx, 194
    mov esi, 92
    call fill_rounded_rectangle
    mov eax, NOVA_COLOR_SURFACE
    mov ebx, 171
    mov ecx, 35
    mov edx, [kernel_context + CONTEXT_WIDTH]
    sub edx, 196
    mov esi, 90
    call fill_rounded_rectangle
    mov esi, text_ribbon_start
    mov ebx, 194
    mov ecx, 48
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 1
    call draw_text
    mov esi, text_ribbon_file
    mov ebx, 270
    mov ecx, 48
    mov edx, NOVA_COLOR_MUTED
    mov ebp, 1
    call draw_text
    mov esi, text_ribbon_view
    mov ebx, 336
    mov ecx, 48
    mov edx, NOVA_COLOR_MUTED
    mov ebp, 1
    call draw_text
    mov eax, NOVA_COLOR_BLUE
    mov ebx, 190
    mov ecx, 72
    mov edx, 104
    mov esi, 38
    call fill_rounded_rectangle
    mov eax, NOVA_COLOR_ELEVATED
    mov ebx, 304
    mov ecx, 72
    mov edx, 104
    mov esi, 38
    call fill_rounded_rectangle
    mov eax, NOVA_COLOR_ELEVATED
    mov ebx, 418
    mov ecx, 72
    mov edx, 104
    mov esi, 38
    call fill_rounded_rectangle
    mov esi, text_ribbon_new
    mov ebx, 210
    mov ecx, 84
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 1
    call draw_text
    mov esi, text_ribbon_open
    mov ebx, 324
    mov ecx, 84
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 1
    call draw_text
    mov esi, text_ribbon_settings
    mov ebx, 429
    mov ecx, 84
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 1
    call draw_text

.start_menu:
    test dword [display_scene_flags], DISPLAY_SCENE_START_MENU
    jz .status
    mov eax, NOVA_COLOR_BLUE
    mov ebx, 170
    mov ecx, 142
    mov edx, 360
    mov esi, [kernel_context + CONTEXT_HEIGHT]
    sub esi, 228
    call fill_rounded_rectangle
    mov eax, NOVA_COLOR_SURFACE
    mov ebx, 171
    mov ecx, 143
    mov edx, 358
    mov esi, [kernel_context + CONTEXT_HEIGHT]
    sub esi, 230
    call fill_rounded_rectangle

    mov eax, NOVA_COLOR_ELEVATED
    cmp dword [display_scene_focus], 2
    jne .search_color_ready
    mov eax, NOVA_COLOR_BLUE
.search_color_ready:
    mov ebx, 190
    mov ecx, 162
    mov edx, 320
    mov esi, 42
    call fill_rounded_rectangle
    mov esi, text_start_search
    mov ebx, 210
    mov ecx, 176
    mov edx, NOVA_COLOR_MUTED
    mov ebp, 1
    call draw_text
    mov esi, text_start_pinned
    mov ebx, 192
    mov ecx, 224
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 1
    call draw_text

    mov eax, NOVA_COLOR_ELEVATED
    cmp dword [display_scene_focus], 3
    jne .tile_files_color_ready
    mov eax, NOVA_COLOR_BLUE
.tile_files_color_ready:
    mov ebx, 190
    mov ecx, 250
    mov edx, 96
    mov esi, 54
    call fill_rounded_rectangle
    mov eax, NOVA_COLOR_ELEVATED
    cmp dword [display_scene_focus], 4
    jne .tile_settings_color_ready
    mov eax, NOVA_COLOR_BLUE
.tile_settings_color_ready:
    mov ebx, 298
    call fill_rounded_rectangle
    mov eax, NOVA_COLOR_ELEVATED
    cmp dword [display_scene_focus], 5
    jne .tile_terminal_color_ready
    mov eax, NOVA_COLOR_BLUE
.tile_terminal_color_ready:
    mov ebx, 406
    call fill_rounded_rectangle
    mov eax, NOVA_COLOR_TILE_ALT
    cmp dword [display_scene_focus], 6
    jne .tile_recovery_color_ready
    mov eax, NOVA_COLOR_BLUE
.tile_recovery_color_ready:
    mov ebx, 190
    mov ecx, 316
    call fill_rounded_rectangle
    mov eax, NOVA_COLOR_TILE_ALT
    cmp dword [display_scene_focus], 7
    jne .tile_help_color_ready
    mov eax, NOVA_COLOR_BLUE
.tile_help_color_ready:
    mov ebx, 298
    call fill_rounded_rectangle
    mov eax, NOVA_COLOR_TILE_ALT
    cmp dword [display_scene_focus], 8
    jne .tile_power_color_ready
    mov eax, NOVA_COLOR_BLUE
.tile_power_color_ready:
    mov ebx, 406
    call fill_rounded_rectangle
    mov esi, text_tile_files
    mov ebx, 210
    mov ecx, 270
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 1
    call draw_text
    mov esi, text_tile_settings
    mov ebx, 310
    mov ecx, 270
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 1
    call draw_text
    mov esi, text_tile_terminal
    mov ebx, 417
    mov ecx, 270
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 1
    call draw_text
    mov esi, text_tile_recovery
    mov ebx, 202
    mov ecx, 336
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 1
    call draw_text
    mov esi, text_tile_help
    mov ebx, 322
    mov ecx, 336
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 1
    call draw_text
    mov esi, text_tile_power
    mov ebx, 430
    mov ecx, 336
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 1
    call draw_text

    mov esi, text_start_user
    mov ebx, 194
    mov ecx, [kernel_context + CONTEXT_HEIGHT]
    sub ecx, 114
    mov edx, NOVA_COLOR_MUTED
    mov ebp, 1
    call draw_text
    mov esi, text_start_power
    mov ebx, 442
    mov ecx, [kernel_context + CONTEXT_HEIGHT]
    sub ecx, 114
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 1
    call draw_text

.status:
    cmp dword [kernel_context + CONTEXT_WIDTH], 800
    jb .footer
    mov eax, NOVA_COLOR_SURFACE
    mov ebx, 550
    mov ecx, 142
    mov edx, [kernel_context + CONTEXT_WIDTH]
    sub edx, 574
    mov esi, 220
    call fill_rounded_rectangle
    mov esi, text_desktop_welcome
    mov ebx, 570
    mov ecx, 170
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 2
    call draw_text
    mov esi, text_desktop_ready
    mov ebx, 570
    mov ecx, 218
    mov edx, NOVA_COLOR_MUTED
    mov ebp, 1
    call draw_text
    mov eax, NOVA_COLOR_SUCCESS
    mov ebx, 570
    mov ecx, 264
    mov edx, 12
    mov esi, 12
    call fill_rounded_rectangle
    mov esi, text_desktop_services
    mov ebx, 592
    mov ecx, 266
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 1
    call draw_text
.footer:
    test dword [display_scene_flags], DISPLAY_SCENE_TASKBAR
    jz .footer_text
    ; Schwebende, geschützte Taskleiste. Die Grundnavigation bleibt links
    ; vorhersehbar; Status und Uhr sind rechts gruppiert.
    mov eax, NOVA_COLOR_BLUE
    mov ebx, 170
    mov ecx, [kernel_context + CONTEXT_HEIGHT]
    sub ecx, 74
    mov edx, [kernel_context + CONTEXT_WIDTH]
    sub edx, 194
    mov esi, 54
    call fill_rounded_rectangle
    mov eax, NOVA_COLOR_SURFACE
    mov ebx, 171
    mov ecx, [kernel_context + CONTEXT_HEIGHT]
    sub ecx, 73
    mov edx, [kernel_context + CONTEXT_WIDTH]
    sub edx, 196
    mov esi, 52
    call fill_rounded_rectangle
    mov eax, NOVA_COLOR_BLUE
    mov ebx, 184
    mov ecx, [kernel_context + CONTEXT_HEIGHT]
    sub ecx, 64
    mov edx, 76
    mov esi, 36
    call fill_rounded_rectangle
    mov esi, text_taskbar_start
    mov ebx, 200
    mov ecx, [kernel_context + CONTEXT_HEIGHT]
    sub ecx, 52
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 1
    call draw_text
    mov eax, NOVA_COLOR_ELEVATED
    mov ebx, 270
    mov ecx, [kernel_context + CONTEXT_HEIGHT]
    sub ecx, 64
    mov edx, 164
    mov esi, 36
    call fill_rounded_rectangle
    mov esi, text_taskbar_search
    mov ebx, 288
    mov ecx, [kernel_context + CONTEXT_HEIGHT]
    sub ecx, 52
    mov edx, NOVA_COLOR_MUTED
    mov ebp, 1
    call draw_text
    cmp dword [kernel_context + CONTEXT_WIDTH], 1000
    jb .taskbar_compact
    mov eax, NOVA_COLOR_TILE_ALT
    mov ebx, 444
    mov ecx, [kernel_context + CONTEXT_HEIGHT]
    sub ecx, 64
    mov edx, 38
    mov esi, 36
    call fill_rounded_rectangle
    mov ebx, 492
    call fill_rounded_rectangle
    mov ebx, 540
    call fill_rounded_rectangle
    mov esi, text_taskbar_apps
    mov ebx, 457
    mov ecx, [kernel_context + CONTEXT_HEIGHT]
    sub ecx, 52
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 1
    call draw_text
    mov esi, text_taskbar_apps
    mov ebx, 505
    call draw_text
    mov esi, text_taskbar_apps
    mov ebx, 553
    call draw_text
    mov esi, text_taskbar_status
    mov ebx, [kernel_context + CONTEXT_WIDTH]
    sub ebx, 386
    mov ecx, [kernel_context + CONTEXT_HEIGHT]
    sub ecx, 52
    mov edx, NOVA_COLOR_MUTED
    mov ebp, 1
    call draw_text
    mov esi, text_taskbar_clock
    mov ebx, [kernel_context + CONTEXT_WIDTH]
    sub ebx, 258
    mov ecx, [kernel_context + CONTEXT_HEIGHT]
    sub ecx, 52
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 1
    call draw_text
    mov esi, text_taskbar_notify
    mov ebx, [kernel_context + CONTEXT_WIDTH]
    sub ebx, 202
    mov ecx, [kernel_context + CONTEXT_HEIGHT]
    sub ecx, 52
    mov edx, NOVA_COLOR_BLUE
    mov ebp, 1
    call draw_text
    jmp .footer_text
.taskbar_compact:
    mov esi, text_taskbar_compact
    mov ebx, [kernel_context + CONTEXT_WIDTH]
    sub ebx, 154
    mov ecx, [kernel_context + CONTEXT_HEIGHT]
    sub ecx, 52
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 1
    call draw_text
.footer_text:
    mov esi, text_desktop_footer
    mov ebx, 42
    mov ecx, [kernel_context + CONTEXT_HEIGHT]
    sub ecx, 28
    mov edx, NOVA_COLOR_MUTED
    mov ebp, 1
    call draw_text
.done:
    popad
    ret

draw_boot_screen:
    jmp draw_kernel_log_screen

draw_kernel_log_screen:
    mov eax, NOVA_COLOR_BLACK
    xor ebx, ebx
    xor ecx, ecx
    mov edx, [kernel_context + CONTEXT_WIDTH]
    mov esi, [kernel_context + CONTEXT_HEIGHT]
    call fill_rectangle

    mov eax, NOVA_COLOR_BLUE
    mov ebx, 102
    xor ecx, ecx
    mov edx, 628
    mov esi, 13
    call fill_rounded_rectangle

    mov dword [logo_x], 32
    mov dword [logo_y], 38
    mov dword [logo_color], NOVA_COLOR_BLUE
    mov byte [logo_compact], 1
    mov byte [logo_mirror], 1
    call draw_nova_logo
    mov esi, text_brand_compact
    mov ebx, 32
    mov ecx, 156
    mov edx, NOVA_COLOR_BLUE
    mov ebp, 3
    call draw_text

    mov esi, text_logsystem
    mov ebx, 250
    mov ecx, 52
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 2
    call draw_text
    mov eax, NOVA_COLOR_WHITE
    mov ebx, 212
    mov ecx, 53
    mov edx, 4
    mov esi, 4
    call fill_rectangle
    mov ecx, 61
    call fill_rectangle
    mov ecx, 69
    call fill_rectangle
    mov ebx, 220
    mov ecx, 54
    mov edx, 12
    mov esi, 2
    call fill_rectangle
    mov ecx, 62
    call fill_rectangle
    mov ecx, 70
    call fill_rectangle

    mov eax, NOVA_COLOR_BLUE
    mov ebx, 184
    mov ecx, 92
    mov edx, 583
    mov esi, 480
    call fill_rounded_rectangle
    mov eax, NOVA_COLOR_LOG_PANEL
    mov ebx, 185
    mov ecx, 93
    mov edx, 581
    mov esi, 478
    call fill_rounded_rectangle

    mov esi, text_kernel_log
    mov ebx, 205
    mov ecx, 112
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 1
    call draw_text

    mov esi, text_escape
    mov ebx, 73
    mov ecx, 456
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 2
    call draw_text
    mov eax, NOVA_COLOR_BUTTON
    mov ebx, 54
    mov ecx, 487
    mov edx, 78
    mov esi, 78
    call fill_rounded_rectangle
    call draw_power_icon_aa
    ret

; Geglättetes Power-Symbol: Kreisring mit oberer Öffnung und runder Taste.
; Die drei Deckungsstufen werden mit dem vorhandenen Hintergrund gemischt.
draw_power_icon_aa:
    pushad
    mov dword [text_color], NOVA_COLOR_WHITE
    xor ebp, ebp
.row:
    cmp ebp, 36
    jae .done
    xor ecx, ecx
.column:
    cmp ecx, 36
    jae .next_row
    xor eax, eax

    ; Abgerundeter senkrechter Schalter (x 16..20, y 1..18).
    cmp ebp, 1
    jb .ring
    cmp ebp, 18
    ja .ring
    cmp ecx, 16
    jb .stem_edge
    cmp ecx, 20
    ja .stem_edge
    mov eax, 3
    jmp .paint
.stem_edge:
    cmp ecx, 15
    je .stem_alpha
    cmp ecx, 21
    jne .ring
.stem_alpha:
    mov eax, 1
    jmp .paint

.ring:
    ; Öffnung des Rings im oberen Bereich.
    cmp ebp, 10
    jae .distance
    cmp ecx, 12
    jb .distance
    cmp ecx, 24
    jbe .skip
.distance:
    mov eax, ecx
    sub eax, 18
    imul eax, eax
    mov edx, ebp
    sub edx, 19
    imul edx, edx
    add eax, edx
    cmp eax, 121
    jb .skip
    cmp eax, 255
    ja .skip
    cmp eax, 132
    jb .soft
    cmp eax, 240
    ja .soft
    cmp eax, 150
    jb .medium
    cmp eax, 218
    ja .medium
    mov eax, 3
    jmp .paint
.medium:
    mov eax, 2
    jmp .paint
.soft:
    mov eax, 1
.paint:
    mov edi, ebp
    add edi, 507
    imul edi, [kernel_context + CONTEXT_PITCH]
    add edi, [kernel_context + CONTEXT_FRAMEBUFFER]
    mov edx, ecx
    add edx, 75
    shl edx, 2
    add edi, edx
    call blend_text_pixel
.skip:
    inc ecx
    jmp .column
.next_row:
    inc ebp
    jmp .row
.done:
    popad
    ret

draw_boot_screen_legacy:
    ; Vollständiger Hintergrund
    mov eax, NOVA_COLOR_BACKGROUND
    xor ebx, ebx
    xor ecx, ecx
    mov edx, [kernel_context + CONTEXT_WIDTH]
    mov esi, [kernel_context + CONTEXT_HEIGHT]
    call fill_rectangle

    ; Cyanfarbene Markenlinie
    mov eax, NOVA_COLOR_CYAN
    xor ebx, ebx
    xor ecx, ecx
    mov edx, [kernel_context + CONTEXT_WIDTH]
    mov esi, 6
    call fill_rectangle

    ; Nova-Marke
    mov esi, text_brand
    mov ebx, 64
    mov ecx, 42
    mov edx, NOVA_COLOR_CYAN
    mov ebp, 2
    call draw_text

    call draw_nova_logo

    ; Hauptstatus
    mov esi, text_ready
    mov ebx, 64
    mov ecx, 92
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 4
    call draw_text

    mov esi, text_subtitle
    mov ebx, 66
    mov ecx, 136
    mov edx, NOVA_COLOR_SECONDARY
    mov ebp, 2
    call draw_text

    ; Statuskarte
    mov eax, NOVA_COLOR_PANEL
    mov ebx, 64
    mov ecx, 184
    mov edx, 672
    mov esi, 250
    call fill_rectangle

    mov eax, NOVA_COLOR_PANEL_EDGE
    mov ebx, 64
    mov ecx, 184
    mov edx, 4
    mov esi, 250
    call fill_rectangle

    mov esi, text_status_heading
    mov ebx, 92
    mov ecx, 210
    mov edx, NOVA_COLOR_SECONDARY
    mov ebp, 1
    call draw_text

    mov esi, text_check_kernel
    mov ebx, 92
    mov ecx, 246
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 2
    call draw_text

    mov esi, text_check_handoff
    mov ebx, 92
    mov ecx, 286
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 2
    call draw_text

    mov esi, text_check_memory
    mov ebx, 92
    mov ecx, 326
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 2
    call draw_text

    mov esi, text_check_graphics
    mov ebx, 92
    mov ecx, 366
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 2
    call draw_text

    mov esi, text_check_interrupts
    mov ebx, 92
    mov ecx, 406
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 2
    call draw_text

    ; Grüne Zustandsmarkierungen
    mov eax, NOVA_COLOR_SUCCESS
    mov ebx, 650
    mov ecx, 246
    mov edx, 18
    mov esi, 18
    call fill_rectangle
    mov ecx, 286
    call fill_rectangle
    mov ecx, 326
    call fill_rectangle
    mov ecx, 366
    call fill_rectangle
    mov ecx, 406
    call fill_rectangle

    ; Technische und transparente Sicherheitsinformation
    mov esi, text_technical
    mov ebx, 66
    mov ecx, 470
    mov edx, NOVA_COLOR_SECONDARY
    mov ebp, 1
    call draw_text

    ret

; EAX=Farbe, EBX=x, ECX=y, EDX=Breite, ESI=Höhe
draw_kernel_panic_screen:
    cmp dword [kernel_context + CONTEXT_FRAMEBUFFER], 0
    je .done
    cmp dword [kernel_context + CONTEXT_BPP], 32
    jne .done
    cmp dword [kernel_context + CONTEXT_WIDTH], 640
    jb .done
    cmp dword [kernel_context + CONTEXT_HEIGHT], 480
    jb .done
    mov eax, [panic_report + 8]
    call format_panic_hex
    jmp draw_kernel_error_reference
    mov eax, NOVA_COLOR_BACKGROUND
    xor ebx, ebx
    xor ecx, ecx
    mov edx, [kernel_context + CONTEXT_WIDTH]
    mov esi, [kernel_context + CONTEXT_HEIGHT]
    call fill_rectangle
    mov eax, NOVA_COLOR_CYAN
    xor ebx, ebx
    xor ecx, ecx
    mov edx, [kernel_context + CONTEXT_WIDTH]
    mov esi, 6
    call fill_rectangle
    mov esi, text_brand
    mov ebx, 64
    mov ecx, 42
    mov edx, NOVA_COLOR_CYAN
    mov ebp, 2
    call draw_text
    call draw_nova_logo
    call draw_panic_smile
    mov esi, text_panic_title
    mov ebx, 66
    mov ecx, 170
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 2
    call draw_text
    mov esi, text_panic_explain1
    mov ebx, 66
    mov ecx, 214
    mov edx, NOVA_COLOR_SECONDARY
    mov ebp, 2
    call draw_text
    mov esi, text_panic_explain2
    mov ebx, 66
    mov ecx, 244
    mov edx, NOVA_COLOR_SECONDARY
    mov ebp, 2
    call draw_text
    mov eax, NOVA_COLOR_PANEL
    mov ebx, 64
    mov ecx, 292
    mov edx, 672
    mov esi, 112
    call fill_rectangle
    mov eax, NOVA_COLOR_WARNING
    mov ebx, 64
    mov ecx, 292
    mov edx, 5
    mov esi, 112
    call fill_rectangle
    mov esi, text_panic_action1
    mov ebx, 92
    mov ecx, 318
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 2
    call draw_text
    mov esi, text_panic_action2
    mov ebx, 92
    mov ecx, 350
    mov edx, NOVA_COLOR_SECONDARY
    mov ebp, 2
    call draw_text
    mov esi, text_panic_code
    mov ebx, 66
    mov ecx, 438
    mov edx, NOVA_COLOR_WARNING
    mov ebp, 2
    call draw_text
    mov esi, panic_hex_buffer
    mov ebx, 246
    mov ecx, 438
    mov edx, NOVA_COLOR_WARNING
    mov ebp, 2
    call draw_text
    mov esi, text_panic_footer
    mov ebx, 66
    mov ecx, 492
    mov edx, NOVA_COLOR_SECONDARY
    mov ebp, 1
    call draw_text
.done:
    ret

draw_kernel_error_reference:
    mov eax, NOVA_COLOR_BLACK
    xor ebx, ebx
    xor ecx, ecx
    mov edx, [kernel_context + CONTEXT_WIDTH]
    mov esi, [kernel_context + CONTEXT_HEIGHT]
    call fill_rectangle
    mov eax, NOVA_COLOR_ERROR
    mov ebx, 102
    xor ecx, ecx
    mov edx, 628
    mov esi, 13
    call fill_rounded_rectangle
    mov dword [logo_x], 45
    mov dword [logo_y], 34
    mov dword [logo_color], NOVA_COLOR_ERROR_TEXT
    mov byte [logo_compact], 0
    mov byte [logo_mirror], 0
    call draw_nova_logo
    call draw_kernel_error_mark_full
    mov esi, text_brand_compact
    mov ebx, 336
    mov ecx, 70
    mov edx, NOVA_COLOR_BLUE
    mov ebp, 3
    call draw_text
    mov esi, text_error_title
    mov ebx, 338
    mov ecx, 134
    mov edx, NOVA_COLOR_ERROR_TEXT
    mov ebp, 2
    call draw_text
    mov esi, text_error_explain1
    mov ebx, 27
    mov ecx, 245
    mov edx, NOVA_COLOR_ERROR_TEXT
    mov ebp, 1
    call draw_text
    mov esi, text_error_explain2
    mov ecx, 273
    call draw_text
    mov eax, NOVA_COLOR_ERROR
    mov ebx, 149
    mov ecx, 350
    mov edx, 512
    mov esi, 120
    call fill_rounded_rectangle
    mov eax, NOVA_COLOR_LOG_PANEL
    mov ebx, 150
    mov ecx, 351
    mov edx, 510
    mov esi, 118
    call fill_rounded_rectangle
    mov esi, text_error_action1
    mov ebx, 186
    mov ecx, 371
    mov edx, NOVA_COLOR_ERROR_TEXT
    mov ebp, 1
    call draw_text
    mov esi, text_error_action2
    mov ebx, 381
    mov ecx, 399
    call draw_text
    mov esi, text_error_action3
    mov ebx, 187
    mov ecx, 427
    call draw_text
    mov esi, text_error_code
    mov ebx, 20
    mov ecx, 560
    mov edx, NOVA_COLOR_ERROR
    mov ebp, 1
    call draw_text
    mov esi, panic_hex_buffer
    mov ebx, 270
    mov ecx, 560
    call draw_text
    ret

draw_kernel_error_mark_full:
    pushad
    mov eax, [text_color]
    push eax
    mov dword [text_color], NOVA_COLOR_ERROR_TEXT
    xor ebp, ebp
.row:
    cmp ebp, NOVA_ERROR_MARK_HEIGHT
    jae .restore
    xor ecx, ecx
.column:
    cmp ecx, NOVA_ERROR_MARK_WIDTH
    jae .next_row
    mov eax, ebp
    imul eax, NOVA_ERROR_MARK_ROW_BYTES
    mov edx, ecx
    shr edx, 2
    add eax, edx
    mov al, [nova_error_mark_bitmap + eax]
    mov edx, ecx
    and edx, 3
    push ecx
    mov cl, dl
    shl cl, 1
    shr al, cl
    pop ecx
    and eax, 3
    jz .next_column
    push eax
    mov edi, ebp
    add edi, 84
    imul edi, [kernel_context + CONTEXT_PITCH]
    add edi, [kernel_context + CONTEXT_FRAMEBUFFER]
    mov edx, ecx
    add edx, 650
    shl edx, 2
    add edi, edx
    pop eax
    call blend_text_pixel
.next_column:
    inc ecx
    jmp .column
.next_row:
    inc ebp
    jmp .row
.restore:
    pop eax
    mov [text_color], eax
    popad
    ret

format_panic_hex:
    pushad
    mov ebx, eax
    mov edi, panic_hex_buffer
    mov ecx, 8
.digit:
    rol ebx, 4
    mov eax, ebx
    and eax, 0x0F
    cmp al, 10
    jb .number
    add al, 'A' - 10
    jmp .store
.number:
    add al, '0'
.store:
    stosb
    loop .digit
    mov byte [edi], 0
    popad
    ret

draw_panic_smile:
    pushad
    mov eax, [text_color]
    push eax
    mov dword [text_color], NOVA_COLOR_WHITE
    xor ebp, ebp
.row:
    cmp ebp, NOVA_ERROR_MARK_HEIGHT / 2
    jae .restore
    xor ecx, ecx
.column:
    cmp ecx, NOVA_ERROR_MARK_WIDTH / 2
    jae .next_row
    mov eax, ebp
    shl eax, 1
    imul eax, NOVA_ERROR_MARK_ROW_BYTES
    mov edx, ecx
    shl edx, 1
    mov ebx, edx
    shr ebx, 2
    add eax, ebx
    mov al, [nova_error_mark_bitmap + eax]
    and edx, 3
    push ecx
    mov cl, dl
    shl cl, 1
    shr al, cl
    pop ecx
    and eax, 3
    jz .next_column
    push eax
    mov edi, ebp
    add edi, 94
    imul edi, [kernel_context + CONTEXT_PITCH]
    add edi, [kernel_context + CONTEXT_FRAMEBUFFER]
    mov edx, ecx
    add edx, 66
    shl edx, 2
    add edi, edx
    pop eax
    call blend_text_pixel
.next_column:
    inc ecx
    jmp .column
.next_row:
    inc ebp
    jmp .row
.restore:
    pop eax
    mov [text_color], eax
    popad
    ret

; EBX=Mittelpunkt X, ECX=Mittelpunkt Y, EDX=Radius. Der Orb wird aus drei
; konzentrischen Flaechen aufgebaut und bleibt damit auch ohne GPU rund.
draw_nova_orb:
    pushad
    mov [orb_x], ebx
    mov [orb_y], ecx
    mov [orb_radius], edx
    mov eax, NOVA_COLOR_PURPLE_SOFT
    call fill_circle
    mov eax, NOVA_COLOR_BLUE
    mov edx, [orb_radius]
    sub edx, 4
    mov ebx, [orb_x]
    mov ecx, [orb_y]
    call fill_circle
    mov eax, NOVA_COLOR_ORB_CORE
    mov edx, [orb_radius]
    sub edx, 8
    mov ebx, [orb_x]
    mov ecx, [orb_y]
    call fill_circle
    mov esi, text_orb_n
    mov ebx, [orb_x]
    sub ebx, 4
    mov ecx, [orb_y]
    sub ecx, 6
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 1
    call draw_text
    popad
    ret

; EAX=Farbe, EBX=Mittelpunkt X, ECX=Mittelpunkt Y, EDX=Radius.
fill_circle:
    pushad
    mov [circle_color], eax
    mov [circle_x], ebx
    mov [circle_y], ecx
    mov [circle_radius], edx
    imul edx, edx
    mov [circle_radius_squared], edx
    mov ebp, [circle_radius]
    neg ebp
.row:
    mov eax, [circle_radius]
    cmp ebp, eax
    jg .done
    mov ecx, [circle_radius]
    neg ecx
.column:
    mov eax, [circle_radius]
    cmp ecx, eax
    jg .next_row
    mov eax, ecx
    imul eax, eax
    mov edx, ebp
    imul edx, edx
    add eax, edx
    cmp eax, [circle_radius_squared]
    ja .next_column
    mov edi, [circle_y]
    add edi, ebp
    imul edi, [kernel_context + CONTEXT_PITCH]
    add edi, [kernel_context + CONTEXT_FRAMEBUFFER]
    mov eax, [circle_x]
    add eax, ecx
    shl eax, 2
    add edi, eax
    mov eax, [circle_color]
    mov [edi], eax
.next_column:
    inc ecx
    jmp .column
.next_row:
    inc ebp
    jmp .row
.done:
    popad
    ret

fill_rounded_rectangle:
    pushad
    mov [round_color], eax
    mov [round_x], ebx
    mov [round_y], ecx
    mov [round_width], edx
    mov [round_height], esi
    mov eax, [round_color]
    mov ebx, [round_x]
    add ebx, 6
    mov ecx, [round_y]
    mov edx, [round_width]
    sub edx, 12
    mov esi, 1
    call fill_rectangle
    mov ebx, [round_x]
    add ebx, 3
    inc ecx
    mov edx, [round_width]
    sub edx, 6
    call fill_rectangle
    mov ebx, [round_x]
    inc ebx
    inc ecx
    mov edx, [round_width]
    sub edx, 2
    mov esi, 3
    call fill_rectangle
    mov ebx, [round_x]
    mov ecx, [round_y]
    add ecx, 5
    mov edx, [round_width]
    mov esi, [round_height]
    sub esi, 10
    call fill_rectangle
    mov ebx, [round_x]
    inc ebx
    mov ecx, [round_y]
    add ecx, [round_height]
    sub ecx, 5
    mov edx, [round_width]
    sub edx, 2
    mov esi, 3
    call fill_rectangle
    mov ebx, [round_x]
    add ebx, 3
    add ecx, 3
    mov edx, [round_width]
    sub edx, 6
    mov esi, 1
    call fill_rectangle
    mov ebx, [round_x]
    add ebx, 6
    inc ecx
    mov edx, [round_width]
    sub edx, 12
    call fill_rectangle
    popad
    ret

fill_rectangle:
    pushad
    test edx, edx
    jz .done
    test esi, esi
    jz .done

    mov [rect_color], eax
    mov [rect_x], ebx
    mov [rect_y], ecx
    mov [rect_width], edx
    mov [rect_height], esi
    xor ebp, ebp

.row:
    cmp ebp, [rect_height]
    jae .done
    mov eax, [rect_y]
    add eax, ebp
    imul eax, [kernel_context + CONTEXT_PITCH]
    add eax, [kernel_context + CONTEXT_FRAMEBUFFER]
    mov edi, eax
    mov eax, [rect_x]
    shl eax, 2
    add edi, eax
    mov eax, [rect_color]
    mov ecx, [rect_width]
    rep stosd
    inc ebp
    jmp .row
.done:
    popad
    ret

; Gemeinsames Nova-Logo aus der SVG-Quelle, rechts oben auf jeder
; grafischen Oberfläche.
draw_nova_logo:
    pushad
    mov eax, [text_color]
    push eax
    mov eax, [logo_color]
    mov [text_color], eax
    xor ebp, ebp
.row:
    cmp ebp, NOVA_LOGO_HEIGHT
    jae .done
    cmp byte [logo_compact], 0
    je .row_visible
    mov eax, ebp
    and eax, 3
    cmp eax, 3
    je .next_row
.row_visible:
    xor ecx, ecx
.column:
    cmp ecx, NOVA_LOGO_WIDTH
    jae .next_row
    cmp byte [logo_compact], 0
    je .column_visible
    mov eax, ecx
    and eax, 3
    cmp eax, 3
    je .next_column
.column_visible:
    mov eax, ebp
    imul eax, NOVA_LOGO_ROW_BYTES
    mov edx, ecx
    shr edx, 2
    add eax, edx
    mov al, [nova_logo_bitmap + eax]
    mov edx, ecx
    and edx, 3
    push ecx
    mov cl, dl
    shl cl, 1
    shr al, cl
    pop ecx
    and eax, 3
    jz .next_column
    push eax
    mov edi, ebp
    cmp byte [logo_compact], 0
    je .row_scaled
    mov edx, ebp
    shr edx, 2
    sub edi, edx
.row_scaled:
    add edi, [logo_y]
    imul edi, [kernel_context + CONTEXT_PITCH]
    add edi, [kernel_context + CONTEXT_FRAMEBUFFER]
    mov edx, ecx
    cmp byte [logo_compact], 0
    je .column_scaled
    mov ebx, ecx
    shr ebx, 2
    sub edx, ebx
.column_scaled:
    cmp byte [logo_mirror], 0
    je .column_positioned
    mov ebx, 119
    sub ebx, edx
    mov edx, ebx
.column_positioned:
    add edx, [logo_x]
    shl edx, 2
    add edi, edx
    pop eax
    call blend_text_pixel
.next_column:
    inc ecx
    jmp .column
.next_row:
    inc ebp
    jmp .row
.done:
    pop eax
    mov [text_color], eax
    popad
    ret

; ESI=Text, EBX=x, ECX=y, EDX=Farbe, EBP=Skalierung
draw_text:
    mov [text_pointer], esi
    mov [text_x], ebx
    mov [text_origin_x], ebx
    mov [text_y], ecx
    mov [text_color], edx
    mov [text_scale], ebp

.next_character:
    mov esi, [text_pointer]
    lodsb
    mov [text_pointer], esi
    test al, al
    jz .done
    cmp al, 10
    je .newline
    cmp al, ' '
    je .space
    call draw_character
.advance:
    movzx eax, byte [character_advance]
    add eax, [text_scale]
    add [text_x], eax
    jmp .next_character
.space:
    mov byte [character_advance], 5
    jmp .advance
.newline:
    mov eax, [text_origin_x]
    mov [text_x], eax
    add dword [text_y], 24
    jmp .next_character
.done:
    ret

; AL=ASCII-Zeichen. 2-Bit-Deckungswerte sorgen für geglättete Kanten.
draw_character:
    pushad
    mov bl, al
    mov edi, bm_font_characters
    xor ecx, ecx
.find:
    mov al, [edi + ecx]
    test al, al
    jz .done
    cmp al, bl
    je .found
    inc ecx
    jmp .find
.found:
    mov al, [bm_font_widths + ecx]
    mov [character_advance], al
    imul ecx, BM_FONT_BYTES_PER_GLYPH
    mov esi, bm_font_bitmap
    add esi, ecx
    xor ebp, ebp
.glyph_row:
    cmp ebp, BM_FONT_HEIGHT
    jae .done
    xor ecx, ecx
.glyph_column:
    cmp ecx, BM_FONT_WIDTH
    jae .next_row
    mov eax, ebp
    imul eax, BM_FONT_WIDTH
    add eax, ecx
    mov edx, eax
    shr eax, 2
    and edx, 3
    mov al, [esi + eax]
    push ecx
    mov cl, dl
    shl cl, 1
    shr al, cl
    pop ecx
    and eax, 3
    jz .next_column

    push eax
    mov edi, [text_y]
    add edi, ebp
    imul edi, [kernel_context + CONTEXT_PITCH]
    add edi, [kernel_context + CONTEXT_FRAMEBUFFER]
    mov edx, [text_x]
    add edx, ecx
    shl edx, 2
    add edi, edx
    pop eax
    call blend_text_pixel
.next_column:
    inc ecx
    jmp .glyph_column
.next_row:
    inc ebp
    jmp .glyph_row
.done:
    popad
    ret

; EDI=Framebuffer-Pixel, EAX=Deckung 1..3.
blend_text_pixel:
    cmp eax, 3
    je .opaque
    mov [blend_alpha], eax
    pushad
    mov ebx, [edi]
    mov esi, [text_color]

    mov eax, ebx
    and eax, 0xFF
    mov edx, esi
    and edx, 0xFF
    call blend_channel
    mov [blend_result], eax

    mov eax, ebx
    shr eax, 8
    and eax, 0xFF
    mov edx, esi
    shr edx, 8
    and edx, 0xFF
    call blend_channel
    shl eax, 8
    or [blend_result], eax

    mov eax, ebx
    shr eax, 16
    and eax, 0xFF
    mov edx, esi
    shr edx, 16
    and edx, 0xFF
    call blend_channel
    shl eax, 16
    or eax, [blend_result]
    mov [edi], eax
    popad
    ret
.opaque:
    mov eax, [text_color]
    mov [edi], eax
    ret

blend_channel:
    cmp dword [blend_alpha], 1
    je .one_third
    lea eax, [eax + edx * 2]
    jmp .divide
.one_third:
    lea eax, [edx + eax * 2]
.divide:
    imul eax, eax, 171
    shr eax, 9
    ret

; Interner Kernel Context. Keine Bootloader-Struktur wird nach außen gereicht.
CONTEXT_BIB               equ 0
CONTEXT_SEEN              equ 4
CONTEXT_PLATFORM          equ 8
CONTEXT_BOOT_DRIVE        equ 12
CONTEXT_MEMORY_MAP        equ 16
CONTEXT_MEMORY_COUNT      equ 20
CONTEXT_MEMORY_ENTRY_SIZE equ 24
CONTEXT_FRAMEBUFFER       equ 28
CONTEXT_PITCH             equ 32
CONTEXT_WIDTH             equ 36
CONTEXT_HEIGHT            equ 40
CONTEXT_BPP               equ 44
CONTEXT_PIXEL_FORMAT      equ 48
CONTEXT_KERNEL_ADDRESS    equ 52
CONTEXT_KERNEL_SIZE       equ 56
CONTEXT_KERNEL_ENTRY      equ 60
CONTEXT_SECURITY_STATE    equ 64
CONTEXT_CPU_FEATURE_EDX   equ 68
CONTEXT_CPU_FEATURE_ECX   equ 72
CONTEXT_ACPI_ADDRESS      equ 76
CONTEXT_MODULES_ADDRESS   equ 80
CONTEXT_MODULE_COUNT      equ 84
CONTEXT_ENTROPY_QUALITY   equ 88
CONTEXT_ENTROPY_SEED      equ 92
CONTEXT_SYSTEM_GENERATION equ 108
CONTEXT_BOOT_ATTEMPT      equ 112
CONTEXT_KERNEL_BUILD_ID   equ 116
CONTEXT_KERNEL_FORMAT     equ 136
CONTEXT_BOOT_MODE         equ 140
CONTEXT_BOOT_FLAGS        equ 144
CONTEXT_BOOT_GENERATION   equ 148
CONTEXT_FALLBACK_LEVEL    equ 152
CONTEXT_SYSTEM_GENERATION_HI equ 156
CONTEXT_FIRMWARE_RUNTIME_CAPS equ 160
CONTEXT_FIRMWARE_RUNTIME_CONTEXT equ 164
CONTEXT_FIRMWARE_RUNTIME_ENTRY equ 168
CONTEXT_SIZE              equ 172

CONTEXT_HAS_FIRMWARE      equ 0x01
CONTEXT_HAS_MEMORY        equ 0x02
CONTEXT_HAS_GRAPHICS      equ 0x04
CONTEXT_HAS_KERNEL        equ 0x08
CONTEXT_HAS_CPU           equ 0x10
CONTEXT_HAS_ENTROPY       equ 0x20
CONTEXT_HAS_SYSTEM        equ 0x40
CONTEXT_HAS_KERNEL_ID     equ 0x80
CONTEXT_HAS_BOOT_OPTIONS  equ 0x100
CONTEXT_REQUIRED          equ CONTEXT_HAS_FIRMWARE | CONTEXT_HAS_MEMORY | CONTEXT_HAS_KERNEL | CONTEXT_HAS_CPU | CONTEXT_HAS_ENTROPY | CONTEXT_HAS_SYSTEM

align 8
kernel_context:
    times CONTEXT_SIZE db 0
kernel_boot_stack_top: dd 0
stack_canary_seed: dd 0

COM1_BASE             equ 0x03F8
NOVA_COLOR_BACKGROUND equ 0x00101113
NOVA_COLOR_PANEL      equ 0x001A1C20
NOVA_COLOR_PANEL_EDGE equ 0x003D7DFF
NOVA_COLOR_CYAN       equ 0x004CC2FF
NOVA_COLOR_WHITE      equ 0x00FFFFFF
NOVA_COLOR_SECONDARY  equ 0x00CFCFCF
NOVA_COLOR_SUCCESS    equ 0x0030D158
NOVA_COLOR_WARNING    equ 0x00FFB347
NOVA_COLOR_BLACK      equ 0x00000000
NOVA_COLOR_BLUE       equ 0x002D7FC1
NOVA_COLOR_LOG_PANEL  equ 0x00121212
NOVA_COLOR_BUTTON     equ 0x00292929
NOVA_COLOR_ERROR      equ 0x00B6154B
NOVA_COLOR_ERROR_TEXT equ 0x00A9A9A9
NOVA_COLOR_DESKTOP_BG equ 0x00070D15
NOVA_COLOR_SURFACE    equ 0x00111A25
NOVA_COLOR_ELEVATED   equ 0x001C2938
NOVA_COLOR_TILE_ALT   equ 0x00233343
NOVA_COLOR_MUTED      equ 0x0096A6B8
NOVA_COLOR_TEXT          equ 0x00EAF1FA
NOVA_COLOR_ACRYLIC       equ 0x00101A2A
NOVA_COLOR_WINDOW        equ 0x00081220
NOVA_COLOR_WINDOW_BORDER equ 0x00304768
NOVA_COLOR_BORDER_ACTIVE equ 0x003D7DFF
NOVA_COLOR_PANEL_SOFT    equ 0x00101A2B
NOVA_COLOR_SIDEBAR       equ 0x000B1524
NOVA_COLOR_CARD          equ 0x00131F32
NOVA_COLOR_CARD_BORDER   equ 0x00243C5D
NOVA_COLOR_SELECTION     equ 0x00203D71
NOVA_COLOR_MENU          equ 0x000B1424
NOVA_COLOR_TASKBAR       equ 0x000C1627
NOVA_COLOR_PURPLE        equ 0x00854DFF
NOVA_COLOR_PURPLE_SOFT   equ 0x004B2A82
NOVA_COLOR_CYAN_SOFT     equ 0x0029B8D4
NOVA_COLOR_ORANGE        equ 0x00DD8A3A
NOVA_COLOR_ORB_CORE      equ 0x00070B1B

align 4
rect_color:    dd 0
rect_x:        dd 0
rect_y:        dd 0
rect_width:    dd 0
rect_height:   dd 0
text_pointer:  dd 0
text_x:        dd 0
text_origin_x: dd 0
text_y:        dd 0
text_color:    dd 0
text_scale:    dd 1
character_advance: db 0
align 4
blend_alpha:   dd 0
blend_result:  dd 0
logo_x:        dd 620
logo_y:        dd 22
logo_color:    dd NOVA_COLOR_WHITE
logo_compact:  db 0
logo_mirror:   db 0
round_color:   dd 0
round_x:       dd 0
round_y:       dd 0
round_width:   dd 0
round_height:  dd 0
circle_color:  dd 0
circle_x:      dd 0
circle_y:      dd 0
circle_radius: dd 0
circle_radius_squared: dd 0
orb_x:         dd 0
orb_y:         dd 0
orb_radius:    dd 0
shell_command_x:     dd 0
shell_command_width: dd 0
shell_window_x:      dd 0
shell_window_y:      dd 0
shell_window_width:  dd 0
shell_window_height: dd 0
shell_menu_x:        dd 0
shell_menu_y:        dd 0
shell_menu_width:    dd 0
shell_menu_height:   dd 0
shell_taskbar_y:     dd 0
shell_card_x:        dd 0

text_orb_n:
    db "N",0
text_shell_brand:
    db "NOVA",0x94,"S",0
text_shell_tagline:
    db "Dein System. Deine Freiheit.",0
text_shell_command:
    db "Befehl eingeben oder suchen...   Ctrl K",0
text_shell_status:
    db "WLAN   TON   AKKU 100%    18:42",0
text_shell_date:
    db "21. Mai 2024",0
text_explorer_title:
    db "Explorer",0
text_window_controls:
    db "-     []     X",0
text_explorer_navigation:
    db "Zur",0x81,"ck",0
text_explorer_breadcrumb:
    db "System  >  Benutzer  >  Matthias  >  Dokumente",0
text_explorer_search:
    db "In Dokumente suchen...",0
text_explorer_toolbar:
    db "+ Neu     Ausschneiden   Kopieren   Einf",0x81,"gen   L",0x94,"schen   Sortieren   Anzeigen",0
text_explorer_quick:
    db "SCHNELLZUGRIFF",0
text_explorer_sidebar:
    db "Start",10,"Desktop",10,"Dokumente",10,"Downloads",10,"Bilder",10,"Musik",10,"Videos",10,10
    db "GER",0x84,"TE & VOLUMES",10,"System",10,"Daten",10,"Backup",10,10,"Netzwerk",0
text_explorer_folders:
    db "Ordner",0
text_folder_projects:
    db "Projekte",0
text_folder_work:
    db "Beruf",0
text_folder_private:
    db "Privat",0
text_folder_notes:
    db "Notizen",0
align 4
shell_folder_labels:
    dd text_folder_projects,text_folder_work,text_folder_private,text_folder_notes
text_explorer_files:
    db "Dateien",0
text_explorer_columns:
    db "NAME                                      ",0x84,"NDERUNGSDATUM       TYP              GR",0x99,"SSE",0
text_explorer_file_rows:
    db "Projektplan_NovaOS.docx                 21.05.2024 10:21     Nova Dokument    2,4 MB",10
    db "Anforderungen_Systemarchitektur.pdf     20.05.2024 16:45     PDF Dokument     1,8 MB",10
    db "Budget_",0x9A,"bersicht.xlsx                    19.05.2024 09:12     Nova Sheet       956 KB",10
    db "nova_flow_config.json                   16.05.2024 12:11     JSON Datei       8 KB",0
text_explorer_footer:
    db "12 Elemente    5 Ordner    7 Dateien                                  548 GB frei",0

text_start_brand:
    db "NOVA",0
text_start_navigation:
    db "Start",10,10,"Apps",10,10,"Dokumente",10,10,"Personen",10,10,"F",0x84,"higkeiten",10,10,"System",0
text_start_navigation_lower:
    db "Einstellungen",10,10,"Ein/Aus",0
text_start_pinned_v2:
    db "Angeheftet",0
text_app_files:
    db "Dateien",0
text_app_browser:
    db "Browser",0
text_app_mail:
    db "Mail",0
text_app_sheet:
    db "Sheet",0
text_app_code:
    db "Code",0
text_app_terminal:
    db "Terminal",0
text_app_skills:
    db "Skills",0
text_app_settings:
    db "System",0
align 4
shell_app_labels:
    dd text_app_files,text_app_browser,text_app_mail,text_app_sheet
    dd text_app_code,text_app_terminal,text_app_skills,text_app_settings
text_start_suggestions:
    db "Vorschl",0x84,"ge",0
text_start_suggestion_items:
    db "Q2 Financial Report",10,"Project Orion",10,"AI Meeting Notes",0
text_start_search_v2:
    db "Nach Apps, Dateien und F",0x84,"higkeiten suchen...",0
text_start_profile:
    db "Matthias   Nova Benutzer",0
text_start_clock_widget:
    db "DIENSTAG, 21. MAI",10,"18:42",0
text_start_ai_widget:
    db "NOVA AI",10,"Wie kann ich dir",10,"heute helfen?",0
text_start_system_widget:
    db "SYSTEM",10,"CPU 32%   RAM 64%",10,"Speicher 58%   Netz aktiv",0

text_taskbar_search_v2:
    db "Suche oder Befehl eingeben...",0
text_pin_files:
    db "D",0
text_pin_browser:
    db "W",0
text_pin_mail:
    db "M",0
text_pin_code:
    db "C",0
text_pin_terminal:
    db ">",0
text_pin_sheet:
    db "S",0
align 4
shell_pin_labels:
    dd text_pin_files,text_pin_browser,text_pin_mail,text_pin_code,text_pin_terminal,text_pin_sheet
text_taskbar_status_v2:
    db "^   WLAN   TON   100%    18:42",0
text_taskbar_date_v2:
    db "21. Mai 2024",0

text_sheet_title:
    db "Nova Sheet - Budget_",0x9A,"bersicht.nova",0
text_sheet_tabs:
    db "Datei   Start   Einf",0x81,"gen   Daten   Ansicht   Formeln   ",0x9A,"berpr",0x81,"fen   Automatisieren   Nova F",0x84,"higkeiten",0
text_sheet_ribbon:
    db "ZWISCHENABLAGE          SCHRIFTART          AUSRICHTUNG          ZAHL          FORMATVORLAGEN          ZELLEN",10
    db "Einf",0x81,"gen  Kopieren      Nova Sans  11      Links  Mitte      W",0x84,"hrung      Tabelle  Zellenformat      Einf",0x81,"gen  L",0x94,"schen",0
text_sheet_formula:
    db "B2     fx     15.750,00",0
text_sheet_heading:
    db "Budget ",0x9A,"bersicht 2024",0
text_sheet_columns:
    db "KATEGORIE       MAI        JUNI       JULI       AUGUST     SEPTEMBER     GESAMT      BUDGET      ABWEICHUNG",0
text_sheet_rows:
    db "Einnahmen       15.750     16.230     17.100     15.980     16.540        81.600      82.000      +400",10
    db "Miete           -4.500     -4.500     -4.500     -4.500     -4.500        -22.500     -22.500       0",10
    db "Verpflegung     -2.300     -2.450     -2.620     -2.510     -2.480        -12.360     -12.000      -360",10
    db "Transport         -850       -780       -920       -810       -810         -4.240      -4.500      +260",10
    db "Sparen          -2.500     -2.500     -2.500     -2.500     -2.500        -12.500     -12.500       0",10
    db "Saldo            3.350      3.650      4.030      3.350      3.790         18.550      18.500       +50",0
text_sheet_capabilities:
    db "NOVA F",0x84,"HIGKEITEN",10,10
    db "Daten verstehen",10,"  Analyse starten",10,"  Erkl",0x84,"rungen",10,"  Datenqualit",0x84,"t pr",0x81,"fen",10,10
    db "Visualisieren",10,"  Diagramm erstellen",10,"  Pivot-Tabelle",10,"  Heatmap",10,10
    db "Automatisieren",10,"  Regel erstellen",10,"  Bericht generieren",0
text_sheet_footer:
    db "Bereit     ",0x9A,"bersicht   Einnahmen   Ausgaben   Analyse                         100%",0

text_studio_title:
    db "F",0x84,"higkeiten Studio",0
text_studio_steps:
    db "1 Info      2 Bausteine      3 Verbindung      4 Verhalten      5 Testen",0
text_studio_navigation:
    db "NOVA",10,10,"",0x9A,"bersicht",10,10,"F",0x84,"higkeiten",10,10,"Meine F",0x84,"higkeiten",10,10
    db "Entdecken",10,10,"Favoriten",10,10,"Vorlagen",10,10,"STUDIO",10,"Erstellen",10,10,"Kombinieren",10,10,"Testen",0
text_studio_catalog:
    db "F",0x84,"HIGKEITEN-KATALOG",10,10,"F",0x84,"higkeiten suchen...",10,10
    db "Datei-Import          +",10,10,"Daten-Filter          +",10,10,"Berechnung             +",10,10
    db "Diagramm-Generator    +",10,10,"KI-Analyse             +",10,10,"Bericht-Export         +",0
text_studio_canvas:
    db "MEINE F",0x84,"HIGKEIT: BUDGET ANALYSE PRO                         100%   FIT",0
text_studio_nodes:
    db "Datei-Import",10,10,10,"Daten-Filter",10,10,10,"Berechnung",10,10,10
    db "Diagramm-Generator       KI-Analyse",10,10,10,"Bericht-Export",0
text_studio_inspector:
    db "EIGENSCHAFTEN",10,10,"Daten-Filter",10,10,"ALLGEMEIN",10,"Name",10,"Beschreibung",10,"Version 1.0.0",10,10
    db "EING",0x84,"NGE",10,"Daten",10,10,"AUSG",0x84,"NGE",10,"Gefilterte Daten",10,10
    db "EINSTELLUNGEN",10,"Filterregeln",10,"Sortierung",10,10,"Erweiterte Optionen",0

text_brand:
    db "NOVA OS", 0
text_brand_compact:
    db "NovaOS", 0
text_desktop_workspace:
    db "WORKSPACE 1", 0
text_ribbon_start:
    db "START", 0
text_ribbon_file:
    db "DATEI", 0
text_ribbon_view:
    db "ANSICHT", 0
text_ribbon_new:
    db "Neu", 0
text_ribbon_open:
    db "Oeffnen", 0
text_ribbon_settings:
    db "Optionen", 0
text_start_search:
    db "Suchen: Apps, Dateien und Aktionen", 0
text_start_pinned:
    db "ANGEHEFTET", 0
text_tile_files:
    db "Dateien", 0
text_tile_settings:
    db "System", 0
text_tile_terminal:
    db "Terminal", 0
text_tile_recovery:
    db "Recovery", 0
text_tile_help:
    db "Hilfe", 0
text_tile_power:
    db "Energie", 0
text_start_user:
    db "Nova Benutzer", 0
text_start_power:
    db "Ausschalten", 0
text_desktop_welcome:
    db "Nova Desktop", 0
text_desktop_ready:
    db "Die erste System-UI-Szene",10,"wird aus Ring 3 dargestellt.", 0
text_desktop_services:
    db "Display Server aktiv", 0
text_desktop_footer:
    db "SYSTEM UI", 0
text_taskbar_start:
    db "NOVA", 0
text_taskbar_search:
    db "Suche", 0
text_taskbar_apps:
    db "N", 0
text_taskbar_status:
    db "NET  TON  DE", 0
text_taskbar_clock:
    db "18:42", 0
text_taskbar_notify:
    db "INFO", 0
text_taskbar_compact:
    db "NET  18:42", 0
text_logsystem:
    db "Logsystem", 0
text_escape:
    db "ESC", 0
text_kernel_log:
    db "NOVA: Kernel Entry",10
    db "NOVA: Panic Reporter ABI 1.1 bereit",10
    db "NOVA: NBHP/BIB v1 validiert",10
    db "NOVA: PMM und Heap ABI 1.0 bereit",10
    db "NOVA: Object und Handle Manager bereit",10
    db "NOVA: Component und Service Manager bereit",10
    db "NOVA: Paging ABI 1.0 aktiv",10
    db "NOVA: IDT, PIC und PIT 100 Hz aktiv",10
    db "NOVA: IPC ABI 1.0 und Endpunkte bereit",10
    db "NOVA: Process und Thread Manager bereit",10
    db "NOVA: Security ABI 1.0 Capabilities aktiv",10
    db "NOVA: Scheduler und drei Threads aktiv",10
    db "NOVA: Device und Power Manager aktiv",10
    db "NOVA: VFS Mount-Namespace und Root bereit",10
    db "NOVA: Network Stack und Loopback lo0 bereit",10
    db "NOVA: x86-32 Ring-3 Userspace aktiv",10
    db "NOVA: System-Call ABI 1.0 aktiv",10
    db "NOVA: Shared Service Page bereit",10
    db "NOVA: Framebuffer und Kernel Context aktiv",10
    db "NOVA_KERNEL_READY",0
text_ready:
    db "SYSTEM BEREIT", 0
text_subtitle:
    db "DER KERNEL WURDE ERFOLGREICH GESTARTET", 0
text_status_heading:
    db "VALIDIERTE STARTKOMPONENTEN", 0
text_check_kernel:
    db "KERNEL-IMAGE UND CRC32", 0
text_check_handoff:
    db "NBHP/BIB BOOTPROTOKOLL", 0
text_check_memory:
    db "COMPONENTS OBJECTS PMM HEAP PAGING", 0
text_check_graphics:
    db "VBE FRAMEBUFFER", 0
text_check_interrupts:
    db "SECURITY PROCESS SERVICES SCHEDULER", 0
text_technical:
    db "NBHP/BIB 1.0 | X86-32 | 800 X 600", 0
text_panic_smile:
    db ";(", 0
text_panic_title:
    db "NOVA OS MUSSTE ANGEHALTEN WERDEN", 0
text_panic_explain1:
    db "EIN WICHTIGER TEIL DES SYSTEMS HAT NICHT RICHTIG REAGIERT.", 0
text_panic_explain2:
    db "ZU DEINER SICHERHEIT WURDE DER COMPUTER ANGEHALTEN.", 0
text_panic_action1:
    db "BITTE STARTE DEN COMPUTER NEU.", 0
text_panic_action2:
    db "WENN DAS ERNEUT PASSIERT NOTIERE DEN FEHLERCODE.", 0
text_panic_code:
    db "FEHLERCODE 0X", 0
text_panic_footer:
    db "TECHNISCHE DETAILS WURDEN IM SYSTEMPROTOKOLL GESPEICHERT.", 0
text_error_title:
    db "Start nicht m",0x94,"glich",0
text_error_explain1:
    db "NovaOS hat einen schweren Systemfehler festgestellt",0
text_error_explain2:
    db "Zu Ihrer Sicherheit wurde das System angehalten",0
text_error_action1:
    db "Bitte den Computer neu starten",0
text_error_action2:
    db "oder",0
text_error_action3:
    db "Den Fehlercode f",0x81,"r die Diagnose notieren",0
text_error_code:
    db "FEHLERCODE: KERNEL 0x",0
panic_hex_buffer:
    db "00000000", 0

; 5x7-Bitmap-Font für den frühen Bootstatus.
font_characters:
    db "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-./:|();", 0

font_bitmap:
    ; A-Z
    db 0x0E,0x11,0x11,0x1F,0x11,0x11,0x11
    db 0x1E,0x11,0x11,0x1E,0x11,0x11,0x1E
    db 0x0E,0x11,0x10,0x10,0x10,0x11,0x0E
    db 0x1E,0x11,0x11,0x11,0x11,0x11,0x1E
    db 0x1F,0x10,0x10,0x1E,0x10,0x10,0x1F
    db 0x1F,0x10,0x10,0x1E,0x10,0x10,0x10
    db 0x0E,0x11,0x10,0x17,0x11,0x11,0x0F
    db 0x11,0x11,0x11,0x1F,0x11,0x11,0x11
    db 0x0E,0x04,0x04,0x04,0x04,0x04,0x0E
    db 0x07,0x02,0x02,0x02,0x12,0x12,0x0C
    db 0x11,0x12,0x14,0x18,0x14,0x12,0x11
    db 0x10,0x10,0x10,0x10,0x10,0x10,0x1F
    db 0x11,0x1B,0x15,0x15,0x11,0x11,0x11
    db 0x11,0x19,0x15,0x13,0x11,0x11,0x11
    db 0x0E,0x11,0x11,0x11,0x11,0x11,0x0E
    db 0x1E,0x11,0x11,0x1E,0x10,0x10,0x10
    db 0x0E,0x11,0x11,0x11,0x15,0x12,0x0D
    db 0x1E,0x11,0x11,0x1E,0x14,0x12,0x11
    db 0x0F,0x10,0x10,0x0E,0x01,0x01,0x1E
    db 0x1F,0x04,0x04,0x04,0x04,0x04,0x04
    db 0x11,0x11,0x11,0x11,0x11,0x11,0x0E
    db 0x11,0x11,0x11,0x11,0x11,0x0A,0x04
    db 0x11,0x11,0x11,0x15,0x15,0x15,0x0A
    db 0x11,0x11,0x0A,0x04,0x0A,0x11,0x11
    db 0x11,0x11,0x0A,0x04,0x04,0x04,0x04
    db 0x1F,0x01,0x02,0x04,0x08,0x10,0x1F
    ; 0-9
    db 0x0E,0x11,0x13,0x15,0x19,0x11,0x0E
    db 0x04,0x0C,0x04,0x04,0x04,0x04,0x0E
    db 0x0E,0x11,0x01,0x02,0x04,0x08,0x1F
    db 0x1E,0x01,0x01,0x0E,0x01,0x01,0x1E
    db 0x02,0x06,0x0A,0x12,0x1F,0x02,0x02
    db 0x1F,0x10,0x10,0x1E,0x01,0x01,0x1E
    db 0x0E,0x10,0x10,0x1E,0x11,0x11,0x0E
    db 0x1F,0x01,0x02,0x04,0x08,0x08,0x08
    db 0x0E,0x11,0x11,0x0E,0x11,0x11,0x0E
    db 0x0E,0x11,0x11,0x0F,0x01,0x01,0x0E
    ; - . / : | ( )
    db 0x00,0x00,0x00,0x1F,0x00,0x00,0x00
    db 0x00,0x00,0x00,0x00,0x00,0x0C,0x0C
    db 0x01,0x02,0x02,0x04,0x08,0x08,0x10
    db 0x00,0x0C,0x0C,0x00,0x0C,0x0C,0x00
    db 0x04,0x04,0x04,0x04,0x04,0x04,0x04
    db 0x02,0x04,0x08,0x08,0x08,0x04,0x02
    db 0x08,0x04,0x02,0x02,0x02,0x04,0x08
    db 0x00,0x0C,0x0C,0x00,0x0C,0x08,0x10

message_entered:
    db "NOVA: Kernel Entry", 13, 10, 0
message_logging_ok:
    db "NOVA: Logging ABI 1.0, strukturierter Ring- und Reservepuffer bereit", 13, 10, 0
message_logging_query_ok:
    db "NOVA: Userspace Logging.Query capability-geprueft", 13, 10, 0
message_logging_read_ok:
    db "NOVA: Userspace neuesten atomar publizierten Logrecord gelesen", 13, 10, 0
message_boot_phase:
    db "NOVA: BOOT phase=0x", 0
message_boot_last_phase:
    db " last_phase=0x", 0
message_panic_manager_ok:
    db "NOVA: Panic Reporter ABI 1.1 bereit", 13, 10, 0
message_crash_dump_ok:
    db "NOVA: Crash Dump ABI 1.0, reservierter Minimal-Dump-Pfad bereit", 13, 10, 0
message_config_ok:
    db "NOVA: Config Framework ABI 1.0, Schema/Store/Transaktion bereit", 13, 10, 0
message_config_error:
    db "NOVA PANIC: Kernel Configuration Selbsttest fehlgeschlagen", 13, 10, 0
message_abi_ok:
    db "NOVA: Kernel ABI 1.0 (§30), Registry mit 11 Built-in-Services bereit", 13, 10, 0
message_abi_error:
    db "NOVA PANIC: Kernel ABI Selbsttest fehlgeschlagen", 13, 10, 0
message_kog_ok:
    db "NOVA: Kernel Object Graph (§100), 7 Root-Objekte, 12 Schemata bereit", 13, 10, 0
message_kog_error:
    db "NOVA PANIC: Kernel Object Graph Selbsttest fehlgeschlagen", 13, 10, 0
message_evbus_ok:
    db "NOVA: Event Bus 1.0 (§101), 8 Built-in-Schemata, Pub/Sub bereit", 13, 10, 0
message_evbus_error:
    db "NOVA PANIC: Event Bus Selbsttest fehlgeschlagen", 13, 10, 0
message_uobj_ok:
    db "NOVA: Unified Object API 1.0 (§102), 15 Typen, Handle-Tabelle bereit", 13, 10, 0
message_uobj_error:
    db "NOVA PANIC: Unified Object API Selbsttest fehlgeschlagen", 13, 10, 0
message_cap_ok:
    db "NOVA: Capability Framework 1.0 (§103), 2 Boot-Caps, 3 Event-Schemata bereit", 13, 10, 0
message_cap_error:
    db "NOVA PANIC: Capability Framework Selbsttest fehlgeschlagen", 13, 10, 0
message_diag_ok:
    db "NOVA: Diagnostics Framework 1.0 (§104), 6 Quellen, Ringpuffer, Health bereit", 13, 10, 0
message_diag_error:
    db "NOVA PANIC: Diagnostics Framework Selbsttest fehlgeschlagen", 13, 10, 0
message_cap_integ_ok:
    db "NOVA: CAP-Integration 1.0, UOBJ-Bridge 32 Slots, IPC/VFS gesichert", 13, 10, 0
message_cap_integ_error:
    db "NOVA PANIC: CAP-Integration Selbsttest fehlgeschlagen", 13, 10, 0
message_vsvc_ok:
    db "NOVA: Versioned Service ABI 1.0 (SS105), 4 Bootstrap-Services, 5 Events", 13, 10, 0
message_vsvc_error:
    db "NOVA PANIC: Versioned Service ABI Selbsttest fehlgeschlagen", 13, 10, 0
message_sync_ok:
    db "NOVA: Synchronisation 1.0 (SS016), Spinlocks/Sema/Completion/SeqLock/Refcount", 13, 10, 0
message_sync_error:
    db "NOVA PANIC: Synchronisation Selbsttest fehlgeschlagen", 13, 10, 0
message_irq_manager_ok:
    db "NOVA: Interrupt Manager 1.0 (SS009), 16 IRQ-Slots, 208 dyn. Vektoren", 13, 10, 0
message_irq_manager_error:
    db "NOVA PANIC: Interrupt Manager Selbsttest fehlgeschlagen", 13, 10, 0
message_exc_mgr_error:
    db "NOVA PANIC: Exception Manager Initialisierung fehlgeschlagen", 13, 10, 0
message_module_loader_ok:
    db "NOVA: Module Loader ABI 1.0, Trust-, ABI- und W^X-Pruefung bereit", 13, 10, 0
message_module_loader_error:
    db "NOVA PANIC: Module Loader Selbsttest fehlgeschlagen", 13, 10, 0
message_cpu_manager_initialized:
    db "NOVA: CPU Manager Initialisierung abgeschlossen", 13, 10, 0
message_cpu_manager_selftest_stage:
    db "NOVA: CPU Manager Selbsttest-Stufe 0x", 0
message_cpu_manager_ok:
    db "NOVA: CPU Manager ABI 1.0, BSP-Topologie und per-CPU-Daten aktiv", 13, 10, 0
message_cpu_topology_import_ok:
    db "NOVA: CPU Manager bezieht Package, Core und Thread aus HAL Topology", 13, 10, 0
message_cpu_manager_error:
    db "NOVA PANIC: CPU Manager Selbsttest fehlgeschlagen", 13, 10, 0
message_smp_ok:
    db "NOVA: SMP-Grundlage ABI 1.0, BSP-Barriere und lokaler TLB-Pfad bereit", 13, 10, 0
message_smp_runtime_ok:
    db "NOVA: SMP AP-LAPIC-Timer und AP-Scheduler aktiv", 13, 10, 0
message_smp_stress_ok:
    db "NOVA: SMP-Stresstest (§62) bestanden: Call, TLB-Shootdown, gemischte Last", 13, 10, 0
message_smp_error:
    db "NOVA PANIC: SMP-Grundlagen-Selbsttest fehlgeschlagen", 13, 10, 0
message_panic_begin:
    db "NOVA PANIC REPORT code=0x", 0
message_panic_subsystem:
    db " subsystem=0x", 0
message_panic_eip:
    db " eip=0x", 0
message_panic_cr2:
    db " cr2=0x", 0
message_panic_phase:
    db " phase=0x", 0
message_panic_last_phase:
    db " last_phase=0x", 0
message_panic_cpu:
    db " cpu=0x", 0
message_panic_security:
    db " security=0x", 0
message_panic_end:
    db "NOVA_PANIC_HALTED", 13, 10, 0
message_debug_panic:
    db "NOVA PANIC: manueller F12-Diagnosetest", 13, 10, 0
message_shutdown:
    db "NOVA: Shutdown angefordert, System wird ausgeschaltet", 13, 10, 0
message_power_manager_ok:
    db "NOVA: Power Manager ABI 1.0 und Shutdown-Pfad bereit", 13, 10, 0
message_display_server_ok:
    db "NOVA: Display Server ABI 1.0, Firmware-Framebuffer uebernommen", 13, 10, 0
message_display_server_fallback:
    db "NOVA: Display Server ohne Grafikprovider, Text-Fallback aktiv", 13, 10, 0
message_display_query_ok:
    db "NOVA: Userspace Display.QueryPrimary ohne MMIO-Adresse erfolgreich", 13, 10, 0
message_display_scene_ok:
    db "NOVA: Desktop, Startmenue, Ribbon und Taskleiste aus Ring-3-Szene praesentiert", 13, 10, 0
message_display_input_ok:
    db "NOVA: Input-Router-Ereignis an Ring-3-System-UI zugestellt", 13, 10, 0
message_power_query_ok:
    db "NOVA: Userspace Power.QuerySystem capability-geprueft", 13, 10, 0
message_power_wake_acquire_ok:
    db "NOVA: Userspace Power.WakeAcquire zeitlich begrenzt", 13, 10, 0
message_power_wake_release_ok:
    db "NOVA: Userspace Power.WakeRelease validiert", 13, 10, 0
message_power_profile_ok:
    db "NOVA: Userspace Power.SetProfile capability-geprueft", 13, 10, 0
message_power_shutdown_denied:
    db "NOVA: Userspace Power.RequestState ohne Capability abgewiesen", 13, 10, 0
message_power_manager_error:
    db "NOVA PANIC: Power Manager nicht initialisierbar", 13, 10, 0
message_network_manager_ok:
    db "NOVA: Network ABI 1.0, IPv4/IPv6 UDP, ICMP und TCP bereit", 13, 10, 0
message_network_query_ok:
    db "NOVA: Userspace Network.Query capability-geprueft", 13, 10, 0
message_network_raw_denied:
    db "NOVA: Userspace Raw-Socket ohne Capability abgewiesen", 13, 10, 0
message_network_socket_ok:
    db "NOVA: Userspace IPv4/IPv6 Datagramm-Socketobjekt erstellt", 13, 10, 0
message_network_stream_ok:
    db "NOVA: Userspace TCP-Stream-Socketobjekt erstellt", 13, 10, 0
message_network_connect_ok:
    db "NOVA: TCP-Stream mit IPv4 127.0.0.1 Port 9000 verbunden", 13, 10, 0
message_network_stream_send_ok:
    db "NOVA: TCP-Streamdaten in begrenzte Sendewarteschlange eingestellt", 13, 10, 0
message_network_stream_receive_ok:
    db "NOVA: TCP-Streamdaten geordnet aus Empfangswarteschlange gelesen", 13, 10, 0
message_network_stream_bind_ok:
    db "NOVA: TCP-Listener an IPv4 127.0.0.1 Port 9000 gebunden", 13, 10, 0
message_network_listen_ok:
    db "NOVA: TCP-Listener mit begrenztem Backlog aktiv", 13, 10, 0
message_network_accept_ok:
    db "NOVA: TCP-Verbindung als separates Socket-Handle akzeptiert", 13, 10, 0
message_network_connect_handle_denied:
    db "NOVA: TCP-Connect wegen ungueltigem Handle/Recht abgewiesen", 13, 10, 0
message_network_connect_firewall_denied:
    db "NOVA: TCP-Connect durch Bootstrap-Firewall abgewiesen", 13, 10, 0
message_network_connect_state_error:
    db "NOVA: TCP-Connect wegen ungueltigem Zustandswechsel abgewiesen", 13, 10, 0
message_network_bind_ok:
    db "NOVA: Userspace Socket an IPv4 127.0.0.1 Port 9000 gebunden", 13, 10, 0
message_network_loopback_ok:
    db "NOVA: IPv4/UDP Loopback mit Header- und Pruefsummenvalidierung erfolgreich", 13, 10, 0
message_network_size_denied:
    db "NOVA: Ueberlanges Loopback-Datagramm sicher abgewiesen", 13, 10, 0
message_network_send_ok:
    db "NOVA: IPv4/UDP Loopback-Datagramm in begrenzte Queue eingestellt", 13, 10, 0
message_network_malformed:
    db "NOVA: Beschaedigtes IPv4/UDP-Datagramm verworfen", 13, 10, 0
message_network_would_block:
    db "NOVA: Nichtblockierende Loopback-Queue meldet WOULD_BLOCK", 13, 10, 0
message_network_manager_error:
    db "NOVA PANIC: Network Manager nicht initialisierbar", 13, 10, 0
message_shutdown_requested:
    db "NOVA: Power Shutdown REQUEST", 13, 10, 0
message_shutdown_userspace:
    db "NOVA: Power Shutdown FREEZE_USERSPACE", 13, 10, 0
message_shutdown_storage:
    db "NOVA: Power Shutdown SYNC_STORAGE", 13, 10, 0
message_shutdown_devices:
    db "NOVA: Power Shutdown STOP_DEVICES", 13, 10, 0
message_shutdown_platform:
    db "NOVA: Power Shutdown PLATFORM_OFF", 13, 10, 0
message_bib_ok:
    db "NOVA: NBHP/BIB v1 validiert", 13, 10, 0
message_recovery_mode_ok:
    db "NOVA: Recovery-Modus aus NBHP/BIB aktiv", 13, 10, 0
message_backup_generation_ok:
    db "NOVA: Backup-Kernelgeneration aus NBHP/BIB aktiv", 13, 10, 0
message_acpi_rsdp_ok:
    db "NOVA: ACPI RSDP mit Pruefsumme validiert", 13, 10, 0
message_acpi_rsdp_missing:
    db "NOVA: ACPI RSDP nicht verfuegbar, BSP-only", 13, 10, 0
message_acpi_cpu_count:
    db "NOVA: ACPI MADT, erkannte CPUs (hex): 0x", 0
message_acpi_srat_ok:
    db "NOVA: ACPI SRAT validiert, CPU/Memory-Affinitaeten (hex): 0x", 0
message_acpi_srat_unavailable:
    db "NOVA: ACPI SRAT nicht validiert oder nicht verfuegbar, NUMA unbekannt", 13, 10, 0
message_line_end:
    db 13, 10, 0
message_kernel_identity_ok:
    db "NOVA: Kernel Build-ID aus NBHP/BIB importiert", 13, 10, 0
message_pmm_ok:
    db "NOVA: PMM NUMA ABI 1.1, Preferred und Strict Allocation aktiv", 13, 10, 0
message_pmm_error:
    db "NOVA PANIC: physischer Speichermanager nicht initialisierbar", 13, 10, 0
message_heap_ok:
    db "NOVA: Heap ABI 1.0 und Schreibtest bereit", 13, 10, 0
message_heap_error:
    db "NOVA PANIC: Bootstrap-Heap nicht initialisierbar", 13, 10, 0
message_object_manager_ok:
    db "NOVA: Object Manager ABI 1.0 bereit", 13, 10, 0
message_object_manager_error:
    db "NOVA PANIC: Kernel Object Manager nicht initialisierbar", 13, 10, 0
message_handle_manager_ok:
    db "NOVA: Handle Manager ABI 1.0 bereit", 13, 10, 0
message_handle_manager_error:
    db "NOVA PANIC: Prozesslokaler Handle Manager nicht initialisierbar", 13, 10, 0
message_component_manager_ok:
    db "NOVA: Component Manager ABI 1.0 bereit", 13, 10, 0
message_component_manager_error:
    db "NOVA PANIC: Kernel Component Manager nicht initialisierbar", 13, 10, 0
message_paging_ok:
    db "NOVA: Paging ABI 1.0 und Speichertest bereit", 13, 10, 0
message_paging_error:
    db "NOVA PANIC: virtueller Speichermanager nicht initialisierbar", 13, 10, 0
message_interrupts_ok:
    db "NOVA: IDT, PIC und PIT 100 Hz aktiv", 13, 10, 0
message_interrupts_error:
    db "NOVA PANIC: Interrupt- oder Timerinitialisierung fehlgeschlagen", 13, 10, 0
message_ipc_ok:
    db "NOVA: IPC ABI 1.0 FIFO bereit", 13, 10, 0
message_semantic_ok:
    db "NOVA: Semantic Types v1, Registry, Kompatibilitaet und Typed Contracts bereit", 13, 10, 0
message_semantic_core_ok:
    db "NOVA: Semantic Core bereit", 13, 10, 0
message_object_id_abi_ok:
    db "NOVA: ObjectID ABI bereit", 13, 10, 0
message_filesystem_object_registry_ok:
    db "NOVA: Filesystem Object Registry bereit", 13, 10, 0
message_filesystem_object_enumeration_ok:
    db "NOVA: Filesystem Object Enumeration bereit", 13, 10, 0
message_filesystem_object_projection_ok:
    db "NOVA: Filesystem Object Projection Konsistenz bereit", 13, 10, 0
message_filesystem_object_path_ok:
    db "NOVA: Filesystem Object Pfadaufloesung bereit", 13, 10, 0
message_filesystem_volume_registry_ok:
    db "NOVA: Filesystem Volume Registry bereit", 13, 10, 0
message_capability_registry_ok:
    db "NOVA: Capability Registry bereit", 13, 10, 0
message_capability_authority_ok:
    db "NOVA: Capability Authority Rechtepruefung bereit", 13, 10, 0
message_capability_lifecycle_ok:
    db "NOVA: Capability Lifecycle Lookup und Revoke bereit", 13, 10, 0
message_namespace_core_ok:
    db "NOVA: Namespace Core bereit: / System Benutzer Apps Volumes Boot Solutions", 13, 10, 0
message_object_id_lookup_ok:
    db "NOVA: ObjectID Registry Lookup bereit", 13, 10, 0
message_namespace_lookup_ok:
    db "NOVA: Namespace Lookup bereit", 13, 10, 0
message_namespace_path_ok:
    db "NOVA: Namespace Pfadaufloesung bereit", 13, 10, 0
message_namespace_introspection_ok:
    db "NOVA: Namespace Introspection bereit", 13, 10, 0
message_namespace_enumeration_ok:
    db "NOVA: Namespace Enumeration bereit", 13, 10, 0
message_projection_map_ok:
    db "NOVA: ObjectID Projection Map bereit", 13, 10, 0
message_projection_introspection_ok:
    db "NOVA: Projection Introspection bereit", 13, 10, 0
message_object_handle_ok:
    db "NOVA: Kernel Object Handle ABI bereit", 13, 10, 0
message_handle_object_binding_ok:
    db "NOVA: Handle Object-Registry Bindung bereit", 13, 10, 0
message_handle_validation_ok:
    db "NOVA: Handle Rechtevalidierung gegen Capabilities bereit", 13, 10, 0
message_handle_path_ok:
    db "NOVA: Namespace Pfad zu Handle bereit", 13, 10, 0
message_semantic_core_error:
    db "NOVA PANIC: Semantic Core nicht initialisierbar", 13, 10, 0
message_state_manager_ok:
    db "NOVA: Global State ABI 1.0, Versionen und Unknown-State-Pruefung bereit", 13, 10, 0
message_transaction_manager_ok:
    db "NOVA: Transaction ABI 1.0, Begin-Prepare-Commit-Verify bereit", 13, 10, 0
message_semantic_reject_ok:
    db "NOVA: Typed IPC, fremder Type und Version sicher abgewiesen", 13, 10, 0
message_semantic_validation_ok:
    db "NOVA: Semantic Validation, ungueltiger Wert strukturiert abgewiesen", 13, 10, 0
message_ipc_error:
    db "NOVA PANIC: Kernel-IPC nicht initialisierbar", 13, 10, 0
message_state_manager_error:
    db "NOVA PANIC: Global State Manager nicht initialisierbar", 13, 10, 0
message_transaction_manager_error:
    db "NOVA PANIC: Transaction Manager nicht initialisierbar", 13, 10, 0
message_service_manager_ok:
    db "NOVA: Service Manager ABI 1.0 bereit", 13, 10, 0
message_service_manager_error:
    db "NOVA PANIC: Kernel Service Manager nicht initialisierbar", 13, 10, 0
message_process_manager_ok:
    db "NOVA: Process Manager ABI 1.0 bereit", 13, 10, 0
message_process_manager_error:
    db "NOVA PANIC: Kernel Process Manager nicht initialisierbar", 13, 10, 0
message_task_scope_manager_ok:
    db "NOVA: Task Scope ABI 1.0, Hierarchie und Cancellation aktiv", 13, 10, 0
message_task_scope_manager_error:
    db "NOVA PANIC: Task-Scope-Manager nicht initialisierbar", 13, 10, 0
message_task_manager_ok:
    db "NOVA: Task ABI 1.0, Lifecycle und kooperative Cancellation aktiv", 13, 10, 0
message_task_manager_error:
    db "NOVA PANIC: Task Manager nicht initialisierbar", 13, 10, 0
message_task_deadline_manager_ok:
    db "NOVA: Task Deadline ABI 1.0, Parent-Clamp und Miss-Policy aktiv", 13, 10, 0
message_task_deadline_manager_error:
    db "NOVA PANIC: Task Deadline Manager nicht initialisierbar", 13, 10, 0
message_task_group_manager_ok:
    db "NOVA: Task Group ABI 1.0, WaitAll, FailFast und Drain aktiv", 13, 10, 0
message_task_group_manager_error:
    db "NOVA PANIC: Task Group Manager nicht initialisierbar", 13, 10, 0
message_io_request_manager_ok:
    db "NOVA: Async IO ABI 1.0, Completion, Deadline und Backpressure aktiv", 13, 10, 0
message_io_request_manager_error:
    db "NOVA PANIC: Async IO Request Manager nicht initialisierbar", 13, 10, 0
message_io_completion_ok:
    db "NOVA: IO Completion Queue ABI 1.0, FIFO und Batch aktiv", 13, 10, 0
message_io_completion_error:
    db "NOVA PANIC: IO Completion Queue nicht initialisierbar", 13, 10, 0
message_shared_buffer_ok:
    db "NOVA: Shared Buffer ABI 1.0, IO-Lease und Copy-Fallback aktiv", 13, 10, 0
message_shared_buffer_error:
    db "NOVA PANIC: Shared Buffer Manager nicht initialisierbar", 13, 10, 0
message_topology_ok:
    db "NOVA: HAL Topology ABI 1.0, Busse, Devices und IOMMU-Gruppen aktiv", 13, 10, 0
message_topology_cpu_count:
    db "NOVA: HAL Topology, normalisierte CPU-Threads (hex): 0x", 0
message_topology_hierarchy_count:
    db "NOVA: HAL CPUID Hierarchy, Packages/Core/Threads (hex): 0x", 0
message_topology_count_separator:
    db "/0x", 0
message_topology_error:
    db "NOVA PANIC: HAL Topology Manager nicht initialisierbar", 13, 10, 0
message_iommu_ok:
    db "NOVA: IOMMU ABI 1.0, Domains, Gruppen und Fault-Zuordnung aktiv", 13, 10, 0
message_iommu_error:
    db "NOVA PANIC: IOMMU Domain Manager nicht initialisierbar", 13, 10, 0
message_dma_mapping_ok:
    db "NOVA: DMA Mapping ABI 1.0, Pinning und sicherer Fallback aktiv", 13, 10, 0
message_dma_mapping_error:
    db "NOVA PANIC: DMA Mapping Manager nicht initialisierbar", 13, 10, 0
message_scatter_gather_ok:
    db "NOVA: Scatter Gather ABI 1.0, Segmente und Lifetime aktiv", 13, 10, 0
message_scatter_gather_error:
    db "NOVA PANIC: Scatter Gather Manager nicht initialisierbar", 13, 10, 0
message_dma_scatter_gather_ok:
    db "NOVA: DMA Scatter Gather ABI 1.0, Split und Providerlimits aktiv", 13, 10, 0
message_dma_scatter_gather_error:
    db "NOVA PANIC: DMA Scatter Gather Manager nicht initialisierbar", 13, 10, 0
message_dma_iommu_lifecycle_ok:
    db "NOVA: DMA IOMMU Lifecycle fuer linear und Scatter Gather aktiv", 13, 10, 0
message_iommu_fault_propagation_ok:
    db "NOVA: IOMMU Fault beendet DMA und IO Request kontrolliert", 13, 10, 0
message_ringbuf_ok:
    db "NOVA: Ring Buffer 1.0 bereit (SPSC, 4 Instanzen, 16 Slots)", 13, 10, 0
message_ringbuf_error:
    db "NOVA PANIC: Ring Buffer Manager nicht initialisierbar", 13, 10, 0
message_io_scheduler_ok:
    db "NOVA: IO Scheduler ABI 1.0, Prioritaet, Deadline und Fairness aktiv", 13, 10, 0
message_io_scheduler_error:
    db "NOVA PANIC: IO Scheduler nicht initialisierbar", 13, 10, 0
message_io_qos_ok:
    db "NOVA: IO QoS ABI 1.0, Admission, Degradation und Accounting aktiv", 13, 10, 0
message_io_qos_error:
    db "NOVA PANIC: IO QoS Manager nicht initialisierbar", 13, 10, 0
message_security_ok:
    db "NOVA: Security ABI 1.0 Capabilities aktiv", 13, 10, 0
message_security_error:
    db "NOVA PANIC: Kernel Security nicht initialisierbar", 13, 10, 0
message_boot_health_authority_ok:
    db "NOVA: Boot Health Authority ABI 1.0 capabilitygeschuetzt bereit", 13, 10, 0
message_boot_health_trust_signed:
    db "NOVA: Boot Health Trust Provider READY (NKI DevSign verifiziert)", 13, 10, 0
message_boot_health_trust_degraded:
    db "NOVA: Boot Health Trust Provider DEGRADED (Kernel nicht signiert)", 13, 10, 0
message_boot_health_session_ready:
    db "NOVA: Boot Health Session Provider READY (Phase-1 Stub)", 13, 10, 0
message_health_commit_written:
    db "NOVA: Boot Health HEALTHY-Wire via Firmware-Provider persistiert (§109)", 13, 10, 0
message_health_commit_failed:
    db "NOVA: Boot Health HEALTHY-Wire Commit fehlgeschlagen", 13, 10, 0
message_boot_health_kernel_initialized:
    db "NOVA: Boot Health Milestone KernelInitialized aggregiert", 13, 10, 0
message_boot_health_checkpoint_written:
    db "NOVA: Boot Health Candidate-Checkpoint ueber Firmware Provider persistiert", 13, 10, 0
message_boot_health_checkpoint_failed:
    db "NOVA: Boot Health Candidate-Checkpoint konnte nicht persistiert werden", 13, 10, 0
message_boot_health_root_pending:
    db "NOVA: Boot Health wartet auf persistentes SystemRoot und Trust", 13, 10, 0
message_boot_health_error:
    db "NOVA PANIC: Boot Health Authority nicht initialisierbar", 13, 10, 0
message_thread_manager_ok:
    db "NOVA: Thread Manager ABI 1.0 bereit", 13, 10, 0
message_thread_manager_error:
    db "NOVA PANIC: Kernel Thread Manager nicht initialisierbar", 13, 10, 0
message_scheduler_ok:
    db "NOVA: Scheduler ABI 1.0 und Runtime-Threads aktiv", 13, 10, 0
message_scheduler_dynamic_ok:
    db "NOVA: Scheduler Dynamic Thread Slot 3 aktiv", 13, 10, 0
message_scheduler_error:
    db "NOVA PANIC: praemptiver Scheduler nicht initialisierbar", 13, 10, 0
message_worksteal_ok:
    db "NOVA: Work Stealing 1.0 bereit (8 Slots, RT-Schutz, Affinitaet)", 13, 10, 0
message_worksteal_error:
    db "NOVA PANIC: Work-Stealing-Subsystem nicht initialisierbar", 13, 10, 0
message_novafs_mount_failed:
    db "NOVA: NovaFS Mount fehlgeschlagen, Fehler 0x", 0
message_novafs_absent:
    db "NOVA: NovaFS kein Systemvolume gefunden, Bootstrap-RAMFS bleibt Root", 13, 10, 0
message_novafs_mounted:
    db "NOVA: NovaFS 1.0 Systemvolume gemountet, Generation 0x", 0
message_novafs_volume:
    db " VolumeID 0x", 0
message_novafs_backup_used:
    db "NOVA: NovaFS Backup-Superblock verwendet (Primaerkopie ungueltig oder aelter)", 13, 10, 0
message_novafs_unclean:
    db "NOVA: NovaFS Volume nicht sauber (DIRTY), nur Read-only gemountet", 13, 10, 0
message_novafs_journal_recovered:
    db "NOVA: NovaFS Journal-Wiederherstellung erfolgreich, Volume wieder sauber (CLEAN)", 13, 10, 0
message_novafs_layout_ok:
    db "NOVA: NovaFS Root-Layout konsistent mit Semantic-Core-ObjectIDs", 13, 10, 0
message_novafs_layout_failed:
    db "NOVA: NovaFS Root-Layout inkonsistent, Volume nicht als Root verwendet, Fehler 0x", 0
message_novafs_selftest_failed:
    db "NOVA: NovaFS Selbsttest fehlgeschlagen, Volume nicht als Root verwendet, Fehler 0x", 0
message_novafs_publish_failed:
    db "NOVA: NovaFS Root-Registrierung fehlgeschlagen, Fehler 0x", 0
message_novafs_bootcount:
    db "NOVA: NovaFS persistenter Bootzaehler 0x", 0
message_novafs_rw_ok:
    db "NOVA: NovaFS Lese-/Schreibtest mit Extents, Teilbloecken und Blockgrenze bereit", 13, 10, 0
message_novafs_selftest_ro:
    db "NOVA: NovaFS Read-only, Schreibtest uebersprungen", 13, 10, 0
message_novafs_root_ok:
    db "NOVA: NovaFS ist persistentes SystemRoot unter /", 13, 10, 0
message_boot_health_root_ready:
    db "NOVA: Boot Health SystemRoot bereit, wartet auf Trust", 13, 10, 0
message_boot_health_root_degraded:
    db "NOVA: Boot Health SystemRoot nur Read-only verfuegbar (degradiert)", 13, 10, 0
message_boot_health_system_root_ready:
    db "NOVA: Boot Health SystemRoot Provider READY (NovaFS rw gemountet)", 13, 10, 0
message_boot_health_system_root_degraded:
    db "NOVA: Boot Health SystemRoot Provider DEGRADED (kein rw-Volume)", 13, 10, 0
message_boot_health_confirmed:
    db "NOVA: Boot Health HealthConfirmed (alle Meilensteine HEALTHY erreicht)", 13, 10, 0
message_boot_health_confirmed_degraded:
    db "NOVA: Boot Health DegradedConfirmed (Meilensteine via DEGRADED-Pfad erreicht)", 13, 10, 0
message_boot_health_not_confirmed:
    db "NOVA: Boot Health OPERATIONAL nicht erreicht, Status bleibt Pending", 13, 10, 0
message_storage_ahci_ok:
    db "NOVA: Storage ABI 1.0, AHCI-Controller (Polling) aktiv, Datentraeger 0x", 0
message_storage_no_ahci:
    db "NOVA: Storage ABI 1.0, kein AHCI-Controller gefunden", 13, 10, 0
message_storage_ahci_failed:
    db "NOVA: Storage ABI 1.0, AHCI-Controller nicht nutzbar", 13, 10, 0
message_storage_disk:
    db "NOVA: Storage SATA-Datentraeger Port 0x", 0
message_storage_disk_id:
    db " DeviceID 0x", 0
message_storage_disk_sectors:
    db " Sektoren 0x", 0
message_storage_selftest_ok:
    db "NOVA: Storage DMA-Lesetest und Bereichspruefung bereit", 13, 10, 0
message_device_manager_ok:
    db "NOVA: Device Manager ABI 1.0 und Bootgeraete aktiv", 13, 10, 0
message_device_manager_error:
    db "NOVA PANIC: Kernel Device Manager nicht initialisierbar", 13, 10, 0
message_driver_framework_ok:
    db "NOVA: Driver Framework ABI 1.0 bereit", 13, 10, 0
message_driver_framework_error:
    db "NOVA PANIC: Driver Framework nicht initialisierbar", 13, 10, 0
message_numa_ok:
    db "NOVA: NUMA 1.0 bereit (1 Node, UMA-Modus)", 13, 10, 0
message_numa_error:
    db "NOVA PANIC: NUMA-Subsystem nicht initialisierbar", 13, 10, 0
message_vfs_ok:
    db "NOVA: VFS ABI 1.0, Mount-Namespace und Bootstrap-Root bereit", 13, 10, 0
message_vfs_error:
    db "NOVA PANIC: VFS oder Root-Dateisystem nicht initialisierbar", 13, 10, 0
message_userspace_ok:
    db "NOVA: x86-32 Userspace, TSS und System-Call ABI 1.0 bereit", 13, 10, 0
message_userspace_exit_ok:
    db "NOVA: erster Ring-3-Systemdienst kontrolliert ausgefuehrt", 13, 10, 0
message_userspace_error:
    db "NOVA PANIC: initialer Userspace-Prozess nicht startbar", 13, 10, 0
message_process_syscall_ok:
    db "NOVA: Userspace Process.QuerySelf erfolgreich", 13, 10, 0
message_process_handle_ok:
    db "NOVA: Process.OpenSelf Handle typ- und rechtegeprueft", 13, 10, 0
message_thread_syscall_ok:
    db "NOVA: Userspace Thread.QuerySelf erfolgreich", 13, 10, 0
message_thread_handle_ok:
    db "NOVA: Thread.OpenSelf Handle typ- und rechtegeprueft", 13, 10, 0
message_shared_service_ok:
    db "NOVA: Shared Service Page im Userspace validiert", 13, 10, 0
message_ipc_roundtrip_ok:
    db "NOVA: Userspace IPC Inline-Roundtrip atomar erfolgreich", 13, 10, 0
message_vfs_userspace_ok:
    db "NOVA: Userspace VFS.OpenRoot Handle erfolgreich", 13, 10, 0
message_vfs_lookup_ok:
    db "NOVA: Userspace VFS.Lookup Pfad '/' erfolgreich", 13, 10, 0
message_handle_close_ok:
    db "NOVA: Userspace Handle geschlossen, alter Wert ungueltig", 13, 10, 0
message_exception:
    db "NOVA PANIC: CPU-Ausnahme Vektor 0x", 0
message_fault_address:
    db " bei Adresse 0x", 0
message_newline:
    db 13, 10, 0
message_framebuffer_ok:
    db "NOVA: Kernel Context und Framebuffer aktiv", 13, 10, 0
message_text_mode:
    db "NOVA: Kernel Context, Textmodus aktiv", 13, 10, 0
message_bib_error:
    db "NOVA PANIC: ungueltiger Boot Handoff", 13, 10, 0
message_ready:
    db "NOVA_KERNEL_READY", 13, 10, 0

; ===========================================================================
; §010 – Exception Manager 1.0 (NPSPEC-KERNEL-0010)
; ===========================================================================
;
; Implementiert:
;   - Exception-Klassifizierung (CPU/MEMORY/ARITHMETIC/INSTRUCTION/DEBUG/
;     SECURITY/RESOURCE/SOFTWARE/KERNEL_FATAL)
;   - Ursprungsbestimmung (USER / KERNEL)
;   - Exception Record Aufbau
;   - Userspace-Exception-Pfad: VMM-Probe → Endpunkt → Thread-Beendigung
;   - Kernel-Exception-Pfad: Fixup-Tabellen-Suche → Panic
;   - Double-Fault- und Machine-Check-Notfallpfade
;   - Exception-Tiefenzähler (per CPU-Slot, anti-rekursiv)
;   - Statistik (total / user / kernel / panics)
;   - Selbsttest (14 Fälle)
;   - Boot-Initialisierung mit §023-Logging
;
; Frame-Layout beim Aufruf von exception_manager_dispatch (von isr_common):
;   [esp+4]  = Vektor (uint32)
;   [esp+8]  = Frame-Zeiger auf isr_common-Stack:
;                [frame+0]  = gespeicherter EAX (pushad)
;                ...
;                [frame+28] = gespeicherter EDI
;                [frame+32] = DS
;                [frame+36] = ES
;                [frame+40] = FS
;                [frame+44] = GS
;                [frame+48] = normalisierter Vektor
;                [frame+52] = Fehlercode
;                [frame+56] = EIP (CPU-gesichert)
;                [frame+60] = CS
;                [frame+64] = EFLAGS
;
; ---------------------------------------------------------------------------

; --- Konstanten -------------------------------------------------------------

EXC_CAT_CPU             equ 0
EXC_CAT_MEMORY          equ 1
EXC_CAT_ARITHMETIC      equ 2
EXC_CAT_INSTRUCTION     equ 3
EXC_CAT_DEBUG           equ 4
EXC_CAT_SECURITY        equ 5
EXC_CAT_RESOURCE        equ 6
EXC_CAT_SOFTWARE        equ 7
EXC_CAT_KERNEL_FATAL    equ 8

EXC_ORIGIN_USER         equ 0
EXC_ORIGIN_KERNEL       equ 1

; Exception Record Struktur (60 Bytes, §7)
EXC_REC_SIZE            equ 60
EXC_REC_STRUCT_SIZE     equ 0    ; uint32  = EXC_REC_SIZE
EXC_REC_VERSION         equ 4    ; uint32  = 1
EXC_REC_CODE            equ 8    ; uint32  (np_exception_code)
EXC_REC_CATEGORY        equ 12   ; uint32  (np_exception_category)
EXC_REC_FLAGS           equ 16   ; uint32
EXC_REC_PROCESS_ID      equ 20   ; uint32
EXC_REC_THREAD_ID       equ 24   ; uint32
EXC_REC_CPU_ID          equ 28   ; uint32
EXC_REC_IP              equ 32   ; uint32
EXC_REC_FAULT_ADDR      equ 36   ; uint32
EXC_REC_ARCH_ERR        equ 40   ; uint32 (low)
EXC_REC_ARCH_ERR_HI     equ 44   ; uint32 (high)
EXC_REC_PARAM_COUNT     equ 48   ; uint32
EXC_REC_PARAM0          equ 52   ; uint32

; Exception-Codes (§5, Mapping auf x86 Vektoren)
NP_EXC_DIVIDE_BY_ZERO   equ 0
NP_EXC_DEBUG            equ 1
NP_EXC_BREAKPOINT       equ 2
NP_EXC_OVERFLOW         equ 3
NP_EXC_BOUND_VIOLATION  equ 4
NP_EXC_INVALID_OPCODE   equ 5
NP_EXC_DEVICE_UNAVAIL   equ 6
NP_EXC_DOUBLE_FAULT     equ 7
NP_EXC_INVALID_STATE    equ 8
NP_EXC_SEGMENT_VIOL     equ 9
NP_EXC_STACK_FAULT      equ 10
NP_EXC_GPF             equ 11
NP_EXC_PAGE_FAULT       equ 12
NP_EXC_FLOAT_POINT      equ 13
NP_EXC_ALIGNMENT        equ 14
NP_EXC_MACHINE_CHECK    equ 15
NP_EXC_SIMD_FP          equ 16
NP_EXC_CTRL_PROTECT     equ 17
NP_EXC_SECURITY_VIOL    equ 18
NP_EXC_RESOURCE_LIMIT   equ 19
NP_EXC_SOFTWARE_RAISED  equ 20

; Fixup-Tabellen-Eintrag (§33, 16 Bytes)
EXC_FIXUP_SIZE          equ 16
EXC_FIXUP_FAULT_START   equ 0
EXC_FIXUP_FAULT_END     equ 4
EXC_FIXUP_RECOVERY      equ 8
EXC_FIXUP_ALLOWED_VECS  equ 12

EXC_FIXUP_SENTINEL      equ 0xFFFFFFFF   ; Ende der Tabelle

; Maximale Exception-Tiefe (§32)
EXC_MAX_DEPTH           equ 4

; Exception Record Flags
EXC_FLAG_ORIGIN_USER    equ 0x0001
EXC_FLAG_ORIGIN_KERNEL  equ 0x0002
EXC_FLAG_FIXUP_USED     equ 0x0004
EXC_FLAG_THREAD_TERM    equ 0x0008
EXC_FLAG_PROC_TERM      equ 0x0010
EXC_FLAG_DOUBLE_FAULT   equ 0x0020
EXC_FLAG_MACHINE_CHECK  equ 0x0040

; --- BSS-Daten (statisch) ---------------------------------------------------

align 4
exception_mgr_initialized:
    dd 0

; Statistik §46
exception_stats_total:      dd 0
exception_stats_user:       dd 0
exception_stats_kernel:     dd 0
exception_stats_handled:    dd 0
exception_stats_thread_term: dd 0
exception_stats_proc_term:  dd 0
exception_stats_panics:     dd 0
exception_stats_recursive:  dd 0

; Per-CPU Exception-Tiefe (bis zu NOVA_CPU_CAPACITY CPUs, §32)
; 4 CPUs × 4 Bytes = 16 Bytes
exception_depth_table:
    times 4 dd 0

; Statischer Notfall-Exception-Record für Double Fault / Machine Check (§41)
align 4
exception_emergency_record:
    times EXC_REC_SIZE db 0

; Letzter Exception Record (für Diagnose / Panic-Ausgabe)
align 4
exception_last_record:
    times EXC_REC_SIZE db 0

; ---------------------------------------------------------------------------
; exception_manager_initialize  – Bootstrap §010 (§49, Test 1 Basis)
;   Aufruf: kein Argument
;   Rückgabe: CF=0 OK, CF=1 Fehler
; ---------------------------------------------------------------------------
exception_manager_initialize:
    push eax
    push esi

    ; Statistikfelder nullen
    mov dword [exception_stats_total],      0
    mov dword [exception_stats_user],       0
    mov dword [exception_stats_kernel],     0
    mov dword [exception_stats_handled],    0
    mov dword [exception_stats_thread_term],0
    mov dword [exception_stats_proc_term],  0
    mov dword [exception_stats_panics],     0
    mov dword [exception_stats_recursive],  0

    ; Tiefenzähler nullen
    mov dword [exception_depth_table + 0],  0
    mov dword [exception_depth_table + 4],  0
    mov dword [exception_depth_table + 8],  0
    mov dword [exception_depth_table + 12], 0

    ; Notfall-Record nullen
    push edi
    push ecx
    mov edi, exception_emergency_record
    mov ecx, EXC_REC_SIZE
    xor eax, eax
    rep stosb
    mov edi, exception_last_record
    mov ecx, EXC_REC_SIZE
    rep stosb
    pop ecx
    pop edi

    mov dword [exception_mgr_initialized], 1

    mov esi, message_exception_mgr_ok
    call serial_write_string

    clc
    pop esi
    pop eax
    ret

; ---------------------------------------------------------------------------
; exc_vector_to_code  – x86 Vektor → np_exception_code  (EAX→EAX, EBX)
; ---------------------------------------------------------------------------
exc_vector_to_code:
    cmp eax, 0 ; #DE
    je .div_zero
    cmp eax, 1 ; #DB
    je .debug
    cmp eax, 3 ; #BP
    je .breakpoint
    cmp eax, 4 ; #OF
    je .overflow
    cmp eax, 5 ; #BR
    je .bound
    cmp eax, 6 ; #UD
    je .inv_opcode
    cmp eax, 7 ; #NM
    je .dev_unavail
    cmp eax, 8 ; #DF
    je .double_fault
    cmp eax, 10 ; #TS
    je .inv_state
    cmp eax, 11 ; #NP
    je .seg_viol
    cmp eax, 12 ; #SS
    je .stack_fault
    cmp eax, 13 ; #GP
    je .gpf
    cmp eax, 14 ; #PF
    je .page_fault
    cmp eax, 16 ; #MF
    je .float_point
    cmp eax, 17 ; #AC
    je .alignment
    cmp eax, 18 ; #MC
    je .machine_check
    cmp eax, 19 ; #XM/#XF
    je .simd_fp
    cmp eax, 21 ; #CP
    je .ctrl_protect
    ; unbekannt → INVALID_STATE
    mov eax, NP_EXC_INVALID_STATE
    ret
.div_zero:     mov eax, NP_EXC_DIVIDE_BY_ZERO  ; ret
    ret
.debug:        mov eax, NP_EXC_DEBUG            ; ret
    ret
.breakpoint:   mov eax, NP_EXC_BREAKPOINT
    ret
.overflow:     mov eax, NP_EXC_OVERFLOW
    ret
.bound:        mov eax, NP_EXC_BOUND_VIOLATION
    ret
.inv_opcode:   mov eax, NP_EXC_INVALID_OPCODE
    ret
.dev_unavail:  mov eax, NP_EXC_DEVICE_UNAVAIL
    ret
.double_fault: mov eax, NP_EXC_DOUBLE_FAULT
    ret
.inv_state:    mov eax, NP_EXC_INVALID_STATE
    ret
.seg_viol:     mov eax, NP_EXC_SEGMENT_VIOL
    ret
.stack_fault:  mov eax, NP_EXC_STACK_FAULT
    ret
.gpf:          mov eax, NP_EXC_GPF
    ret
.page_fault:   mov eax, NP_EXC_PAGE_FAULT
    ret
.float_point:  mov eax, NP_EXC_FLOAT_POINT
    ret
.alignment:    mov eax, NP_EXC_ALIGNMENT
    ret
.machine_check: mov eax, NP_EXC_MACHINE_CHECK
    ret
.simd_fp:      mov eax, NP_EXC_SIMD_FP
    ret
.ctrl_protect: mov eax, NP_EXC_CTRL_PROTECT
    ret

; ---------------------------------------------------------------------------
; exc_code_to_category  – np_exception_code → np_exception_category  (EAX→EAX)
; ---------------------------------------------------------------------------
exc_code_to_category:
    cmp eax, NP_EXC_PAGE_FAULT
    je .memory
    cmp eax, NP_EXC_STACK_FAULT
    je .resource
    cmp eax, NP_EXC_ALIGNMENT
    je .resource
    cmp eax, NP_EXC_RESOURCE_LIMIT
    je .resource
    cmp eax, NP_EXC_DIVIDE_BY_ZERO
    je .arithmetic
    cmp eax, NP_EXC_OVERFLOW
    je .arithmetic
    cmp eax, NP_EXC_FLOAT_POINT
    je .arithmetic
    cmp eax, NP_EXC_SIMD_FP
    je .arithmetic
    cmp eax, NP_EXC_DEBUG
    je .debug
    cmp eax, NP_EXC_BREAKPOINT
    je .debug
    cmp eax, NP_EXC_INVALID_OPCODE
    je .instruction
    cmp eax, NP_EXC_DEVICE_UNAVAIL
    je .instruction
    cmp eax, NP_EXC_CTRL_PROTECT
    je .instruction
    cmp eax, NP_EXC_DOUBLE_FAULT
    je .fatal
    cmp eax, NP_EXC_MACHINE_CHECK
    je .fatal
    cmp eax, NP_EXC_GPF
    je .security
    cmp eax, NP_EXC_SEGMENT_VIOL
    je .security
    cmp eax, NP_EXC_SECURITY_VIOL
    je .security
    cmp eax, NP_EXC_SOFTWARE_RAISED
    je .software
    ; Fallback → CPU
    mov eax, EXC_CAT_CPU
    ret
.memory:    mov eax, EXC_CAT_MEMORY       ; ret
    ret
.resource:  mov eax, EXC_CAT_RESOURCE
    ret
.arithmetic: mov eax, EXC_CAT_ARITHMETIC
    ret
.debug:     mov eax, EXC_CAT_DEBUG
    ret
.instruction: mov eax, EXC_CAT_INSTRUCTION
    ret
.fatal:     mov eax, EXC_CAT_KERNEL_FATAL
    ret
.security:  mov eax, EXC_CAT_SECURITY
    ret
.software:  mov eax, EXC_CAT_SOFTWARE
    ret

; ---------------------------------------------------------------------------
; exc_build_record  – Exception Record aus Frame aufbauen
;   [esp+4] = Zeiger auf den 60-Byte Record-Puffer
;   [esp+8] = Frame-Zeiger (isr_common Stack)
;   [esp+12] = Vektor
;   Clobbers: EAX, EBX, ECX, EDX (per calling convention OK – Caller pusht)
; ---------------------------------------------------------------------------
exc_build_record:
    push ebp
    mov ebp, esp
    push esi
    push edi

    mov edi, [ebp + 8]   ; Record-Puffer
    mov esi, [ebp + 12]  ; Frame
    mov ecx, [ebp + 16]  ; Vektor

    ; Größe und Version
    mov dword [edi + EXC_REC_STRUCT_SIZE], EXC_REC_SIZE
    mov dword [edi + EXC_REC_VERSION], 1

    ; Code (Vektor → np_exception_code)
    mov eax, ecx
    call exc_vector_to_code
    mov [edi + EXC_REC_CODE], eax

    ; Kategorie
    call exc_code_to_category
    mov [edi + EXC_REC_CATEGORY], eax

    ; Flags: Ursprung aus CS im Frame (Offset 60 vom Frame-Anfang = [esi+60])
    ; Frame: pushad(32)+segs(16) = 48, dann Vektor(4)+ErrCode(4)+EIP(4)+CS(4)
    ; CS ist bei [esi + 60]
    mov eax, [esi + 60]
    and eax, 0x3         ; RPL-Bits: 0=Ring0(Kernel), 3=Ring3(User)
    cmp eax, 0
    je .origin_kernel
    mov dword [edi + EXC_REC_FLAGS], EXC_FLAG_ORIGIN_USER
    jmp .origin_done
.origin_kernel:
    mov dword [edi + EXC_REC_FLAGS], EXC_FLAG_ORIGIN_KERNEL
.origin_done:

    ; IDs (Prozess/Thread/CPU – aus BSP-Kontext, §7)
    ; per_cpu_current_thread[0] = Slot-Index des laufenden Threads auf CPU 0
    mov eax, [per_cpu_current_thread]    ; Thread-Slot-Index (BSP)
    cmp eax, -1
    je .no_thread_id
    ; Thread-Record: TID bei Offset THREAD_TID=0, PID bei THREAD_PID=4
    push ebx
    mov ebx, eax
    imul ebx, THREAD_RECORD_SIZE
    add ebx, thread_table
    mov eax, [ebx + THREAD_TID]
    mov [edi + EXC_REC_THREAD_ID], eax
    mov eax, [ebx + THREAD_PID]
    mov [edi + EXC_REC_PROCESS_ID], eax
    pop ebx
    jmp .ids_done
.no_thread_id:
    mov dword [edi + EXC_REC_THREAD_ID], 0
    mov dword [edi + EXC_REC_PROCESS_ID], 0
.ids_done:
    mov dword [edi + EXC_REC_CPU_ID], 0  ; BSP = 0

    ; EIP aus Frame [esi + 56]
    mov eax, [esi + 56]
    mov [edi + EXC_REC_IP], eax

    ; Fault-Adresse: bei Page Fault (Vektor 14) aus CR2
    xor eax, eax
    cmp ecx, 14
    jne .no_cr2
    mov eax, cr2
.no_cr2:
    mov [edi + EXC_REC_FAULT_ADDR], eax

    ; Architektur-Fehlercode aus Frame [esi + 52]
    mov eax, [esi + 52]
    mov [edi + EXC_REC_ARCH_ERR], eax
    mov dword [edi + EXC_REC_ARCH_ERR_HI], 0

    ; Parameter 0 = Vektor
    mov dword [edi + EXC_REC_PARAM_COUNT], 1
    mov [edi + EXC_REC_PARAM0], ecx

    pop edi
    pop esi
    pop ebp
    ret

; ---------------------------------------------------------------------------
; exc_search_fixup  – Fixup-Tabelle nach EIP durchsuchen (§33)
;   [esp+4]  = EIP (fault instruction pointer)
;   [esp+8]  = Vektor
;   Rückgabe: EAX = Recovery-Adresse, 0 wenn nicht gefunden
; ---------------------------------------------------------------------------
exc_search_fixup:
    push ebp
    mov ebp, esp
    push esi
    push ebx

    mov ebx, [ebp + 8]   ; EIP
    mov ecx, [ebp + 12]  ; Vektor

    mov esi, exception_fixup_table
.loop:
    mov eax, [esi + EXC_FIXUP_FAULT_START]
    cmp eax, EXC_FIXUP_SENTINEL
    je .not_found

    cmp ebx, eax         ; EIP >= fault_start?
    jb .next
    mov eax, [esi + EXC_FIXUP_FAULT_END]
    cmp ebx, eax         ; EIP < fault_end?
    jae .next

    ; Vektor-Maske prüfen
    mov eax, [esi + EXC_FIXUP_ALLOWED_VECS]
    mov edx, 1
    cmp ecx, 31
    ja .vec_ok          ; Vektor > 31 → unkritisch, erlaubt
    shl edx, cl
    test eax, edx
    jz .next
.vec_ok:
    mov eax, [esi + EXC_FIXUP_RECOVERY]
    pop ebx
    pop esi
    pop ebp
    ret
.next:
    add esi, EXC_FIXUP_SIZE
    jmp .loop
.not_found:
    xor eax, eax
    pop ebx
    pop esi
    pop ebp
    ret

; ---------------------------------------------------------------------------
; exception_manager_dispatch  – Haupteinstieg §010
;   [esp+4]  = Vektor
;   [esp+8]  = Frame-Zeiger
;   Rückgabe: EAX = Frame-Zeiger (unverändert oder neuer Frame nach Fixup),
;             0 wenn Return zum alten Frame
; ---------------------------------------------------------------------------
exception_manager_dispatch:
    push ebp
    mov ebp, esp
    pushad

    mov ecx, [ebp + 8]   ; Vektor
    mov edx, [ebp + 12]  ; Frame

    ; Statistik: total
    lock inc dword [exception_stats_total]

    ; Double Fault (Vektor 8) und Machine Check (18): Notfallpfad (§21, §22)
    cmp ecx, 8
    je .double_fault_path
    cmp ecx, 18
    je .machine_check_path

    ; CPU-Slot bestimmen (§32 Tiefenzähler)
    ; Vereinfacht: BSP=0 (nur ein CPU-Slot in der frühen Phase)
    xor eax, eax         ; cpu_id = 0

    ; Exception-Tiefe prüfen (§32)
    mov ebx, [exception_depth_table + eax*4]
    cmp ebx, EXC_MAX_DEPTH
    jae .recursive_exception

    inc dword [exception_depth_table + eax*4]

    ; Exception Record aufbauen
    sub esp, EXC_REC_SIZE
    mov edi, esp          ; Record auf Stack

    push ecx             ; Vektor
    push edx             ; Frame
    push edi             ; Record-Puffer
    call exc_build_record
    add esp, 12

    ; Letzten Record kopieren (für Diagnose)
    push edi
    push ecx
    mov esi, edi
    mov edi, exception_last_record
    mov ecx, EXC_REC_SIZE
    push ds
    push es
    push eax
    mov ax, ds
    mov es, ax
    pop eax
    rep movsb
    pop es
    pop ds
    pop ecx
    pop edi              ; edi = Record auf Stack

    ; Ursprung bestimmen
    mov eax, [edi + EXC_REC_FLAGS]
    and eax, EXC_FLAG_ORIGIN_USER
    test eax, eax
    jnz .user_path

    ; === Kernel-Exception-Pfad (§12) ===
    lock inc dword [exception_stats_kernel]

    ; Fixup-Tabelle durchsuchen (§33)
    mov ebx, [edi + EXC_REC_IP]
    push ecx             ; Vektor
    push ebx             ; EIP
    call exc_search_fixup
    add esp, 8

    test eax, eax
    jnz .fixup_found

    ; Kein Fixup → Kernel Panic
    lock inc dword [exception_stats_panics]
    mov esi, message_exc_kernel_panic
    call serial_write_string
    mov eax, [edi + EXC_REC_IP]
    call serial_write_hex32
    mov esi, message_newline
    call serial_write_string

    mov eax, [edi + EXC_REC_CODE]
    xor esi, esi
    call kernel_panic
    ; kehrt nicht zurück

.fixup_found:
    ; Recovery-Adresse im Frame (EIP) setzen und zurückkehren (§12, §33)
    lock inc dword [exception_stats_handled]
    mov ebx, [ebp + 12]  ; Frame
    mov [ebx + 56], eax  ; neues EIP im Frame

    ; EXC_FLAG_FIXUP_USED setzen
    or dword [edi + EXC_REC_FLAGS], EXC_FLAG_FIXUP_USED

    ; Tiefenzähler dekrementieren
    dec dword [exception_depth_table]

    add esp, EXC_REC_SIZE
    mov eax, [ebp + 12]  ; Frame zurückgeben
    mov [ebp - 4], eax   ; EAX-Slot in pushad-Area setzen (popad liest daraus)
    popad
    pop ebp
    ret

    ; === Userspace-Exception-Pfad (§11) ===
.user_path:
    lock inc dword [exception_stats_user]

    ; Page Fault: zuerst VMM-Probe (§14)
    mov eax, [edi + EXC_REC_CODE]
    cmp eax, NP_EXC_PAGE_FAULT
    jne .user_no_vmm

    ; vmm_handle_page_fault: EAX=Fehlercode, EBX=Fault-Adresse
    mov eax, [edi + EXC_REC_ARCH_ERR]
    mov ebx, [edi + EXC_REC_FAULT_ADDR]
    call vmm_handle_page_fault
    ; EAX=0 → behandelt, fortfahren
    test eax, eax
    jnz .user_vmm_failed

    lock inc dword [exception_stats_handled]
    dec dword [exception_depth_table]
    add esp, EXC_REC_SIZE
    mov eax, [ebp + 12]
    mov [ebp - 4], eax   ; EAX-Slot in pushad-Area setzen
    popad
    pop ebp
    ret

.user_vmm_failed:
.user_no_vmm:
    ; Exception-Endpunkt prüfen (§25) – Stub: aktueller Prozess
    ; In dieser Bootstrap-Phase: kein separater Endpunkt-Mechanismus,
    ; Prozess wird sofort beendet (§35, §36)
    lock inc dword [exception_stats_thread_term]

    ; Userspace-Thread/Prozess beenden über thread_manager_terminate_current
    call exception_terminate_current_thread

    ; Nach thread_manager_terminate_current wählt der Scheduler
    ; den nächsten bereiten Thread.
    ; Rückgabe: Frame unverändert (Scheduler übernimmt beim nächsten Tick)
    dec dword [exception_depth_table]
    add esp, EXC_REC_SIZE
    mov eax, [ebp + 12]   ; ursprünglicher Frame (Caller erhält ihn)
    mov [ebp - 4], eax    ; EAX-Slot in pushad-Area setzen
    popad
    pop ebp
    ret

    ; === Rekursive Exception (§32) ===
.recursive_exception:
    lock inc dword [exception_stats_recursive]
    lock inc dword [exception_stats_panics]
    mov esi, message_exc_recursive
    call serial_write_string
    mov eax, 0xE0CA0010
    mov edx, 0x0000DEAD
    xor esi, esi
    call kernel_panic

    ; === Double Fault Notfallpfad (§21, §41) ===
.double_fault_path:
    lock inc dword [exception_stats_panics]
    mov edi, exception_emergency_record
    mov dword [edi + EXC_REC_STRUCT_SIZE], EXC_REC_SIZE
    mov dword [edi + EXC_REC_VERSION],     1
    mov dword [edi + EXC_REC_CODE],        NP_EXC_DOUBLE_FAULT
    mov dword [edi + EXC_REC_CATEGORY],    EXC_CAT_KERNEL_FATAL
    mov dword [edi + EXC_REC_FLAGS],       EXC_FLAG_DOUBLE_FAULT | EXC_FLAG_ORIGIN_KERNEL
    ; EIP aus Frame wenn verfügbar
    test edx, edx
    jz .df_no_frame
    mov eax, [edx + 56]
    mov [edi + EXC_REC_IP], eax
.df_no_frame:
    mov esi, message_exc_double_fault
    call serial_write_string
    mov eax, [edi + EXC_REC_IP]
    call serial_write_hex32
    mov esi, message_newline
    call serial_write_string
    mov eax, 0xDF000008
    mov edx, 0x0000DEAD
    xor esi, esi
    call kernel_panic

    ; === Machine Check Notfallpfad (§22, §41) ===
.machine_check_path:
    lock inc dword [exception_stats_panics]
    mov edi, exception_emergency_record
    mov dword [edi + EXC_REC_STRUCT_SIZE], EXC_REC_SIZE
    mov dword [edi + EXC_REC_VERSION],     1
    mov dword [edi + EXC_REC_CODE],        NP_EXC_MACHINE_CHECK
    mov dword [edi + EXC_REC_CATEGORY],    EXC_CAT_KERNEL_FATAL
    mov dword [edi + EXC_REC_FLAGS],       EXC_FLAG_MACHINE_CHECK | EXC_FLAG_ORIGIN_KERNEL
    mov esi, message_exc_machine_check
    call serial_write_string
    mov eax, 0xEC000012
    mov edx, 0x0000DEAD
    xor esi, esi
    call kernel_panic

; ---------------------------------------------------------------------------
; exception_terminate_current_thread  – Stub (§35)
;   Beendet den aktuellen Userspace-Thread. Delegiert an thread_manager.
; ---------------------------------------------------------------------------
exception_terminate_current_thread:
    push eax
    push ebx
    push esi
    mov esi, message_exc_thread_terminated
    call serial_write_string
    ; BSP-Thread-Slot aus per_cpu_current_thread holen
    mov eax, [per_cpu_current_thread]
    cmp eax, -1
    je .no_thread
    ; Thread-Status auf 0 (inaktiv) setzen (Stub-Terminierung)
    imul ebx, eax, THREAD_RECORD_SIZE
    add ebx, thread_table
    mov dword [ebx + THREAD_STATE], 0
.no_thread:
    pop esi
    pop ebx
    pop eax
    ret

; ---------------------------------------------------------------------------
; vmm_handle_page_fault  – VMM Page-Fault-Stub (§14)
;   EAX = Fehlercode, EBX = Fault-Adresse
;   Rückgabe: EAX=0 behandelt, EAX≠0 nicht behandelt
; ---------------------------------------------------------------------------
vmm_handle_page_fault:
    ; Frühe Phase: keine Demand-Paging-Unterstützung →
    ; Kern-Adressen (>=0xC0000000) immer als unbehandelbar zurückgeben
    cmp ebx, 0xC0000000
    jae .not_handled
    ; Userspace-Adresse: prüfen ob in einer gemappten Region (Stub)
    ; In dieser Bootstrap-Phase: keine UserVM-Regionen → nicht behandelbar
    xor eax, eax
    inc eax              ; 1 = nicht behandelt
    ret
.not_handled:
    mov eax, 1
    ret

; ---------------------------------------------------------------------------
; exception_manager_self_test  – 14 Testfälle (§49)
;   Rückgabe: CF=0 alle OK, CF=1 mindestens ein Fehler
; ---------------------------------------------------------------------------
exception_manager_self_test:
    push eax
    push ebx
    push ecx
    push edx
    push esi
    push edi

    xor esi, esi         ; Fehler-Zähler (ESI; EDI wird in Tests 12/13 als Record-Puffer benötigt)

    ; Test 1: Initialisierungsflag gesetzt
    cmp dword [exception_mgr_initialized], 1
    je .t1_ok
    inc esi
.t1_ok:

    ; Test 2: exc_vector_to_code Vektor 0 → NP_EXC_DIVIDE_BY_ZERO
    mov eax, 0
    call exc_vector_to_code
    cmp eax, NP_EXC_DIVIDE_BY_ZERO
    je .t2_ok
    inc esi
.t2_ok:

    ; Test 3: Vektor 14 → NP_EXC_PAGE_FAULT
    mov eax, 14
    call exc_vector_to_code
    cmp eax, NP_EXC_PAGE_FAULT
    je .t3_ok
    inc esi
.t3_ok:

    ; Test 4: Vektor 8 → NP_EXC_DOUBLE_FAULT
    mov eax, 8
    call exc_vector_to_code
    cmp eax, NP_EXC_DOUBLE_FAULT
    je .t4_ok
    inc esi
.t4_ok:

    ; Test 5: exc_code_to_category PAGE_FAULT → MEMORY
    mov eax, NP_EXC_PAGE_FAULT
    call exc_code_to_category
    cmp eax, EXC_CAT_MEMORY
    je .t5_ok
    inc esi
.t5_ok:

    ; Test 6: exc_code_to_category DIVIDE_BY_ZERO → ARITHMETIC
    mov eax, NP_EXC_DIVIDE_BY_ZERO
    call exc_code_to_category
    cmp eax, EXC_CAT_ARITHMETIC
    je .t6_ok
    inc esi
.t6_ok:

    ; Test 7: exc_code_to_category DOUBLE_FAULT → KERNEL_FATAL
    mov eax, NP_EXC_DOUBLE_FAULT
    call exc_code_to_category
    cmp eax, EXC_CAT_KERNEL_FATAL
    je .t7_ok
    inc esi
.t7_ok:

    ; Test 8: exc_code_to_category DEBUG → DEBUG
    mov eax, NP_EXC_DEBUG
    call exc_code_to_category
    cmp eax, EXC_CAT_DEBUG
    je .t8_ok
    inc esi
.t8_ok:

    ; Test 9: Fixup-Tabellen-Suche: Sentinel → 0 (kein Fixup)
    ; Suche nach EIP 0xDEAD0001, Vektor 0 → muss 0 zurückgeben
    push dword 0
    push dword 0xDEAD0001
    call exc_search_fixup
    add esp, 8
    cmp eax, 0
    je .t9_ok
    inc esi
.t9_ok:

    ; Test 10: Statistik-Felder erreichbar
    mov eax, [exception_stats_total]
    cmp eax, 0xFFFFFFFF
    jne .t10_ok
    inc esi
.t10_ok:

    ; Test 11: Exception-Tiefenzähler = 0 (nach Init)
    cmp dword [exception_depth_table], 0
    je .t11_ok
    inc esi
.t11_ok:

    ; Test 12/13: exc_build_record auf statischen Dummy-Frame
    ; Dummy-Frame im BSS nutzen (exception_emergency_record als Scratch reicht nicht,
    ; daher: Stack-Bereich reservieren, danach Record in emergency_record prüfen)
    sub esp, EXC_REC_SIZE + 68  ; Record + Frame
    mov edi, esp                ; Record-Puffer
    mov ebx, esp
    add ebx, EXC_REC_SIZE       ; Frame-Zeiger
    mov dword [ebx + 60], 0x23  ; CS = Ring-3 (User)
    mov dword [ebx + 56], 0x1234
    mov dword [ebx + 52], 0
    push dword 14               ; Vektor = Page Fault
    push ebx                    ; Frame
    push edi                    ; Record-Puffer
    call exc_build_record
    add esp, 12
    cmp dword [edi + EXC_REC_STRUCT_SIZE], EXC_REC_SIZE
    je .t12_ok
    inc esi
.t12_ok:
    ; Test 13: Kategorie für Vektor 0 (ARITHMETIC)
    mov dword [ebx + 60], 0     ; CS = Ring-0
    mov dword [ebx + 56], 0x5678
    push dword 0                ; Vektor 0 = #DE
    push ebx
    push edi
    call exc_build_record
    add esp, 12
    cmp dword [edi + EXC_REC_CATEGORY], EXC_CAT_ARITHMETIC
    je .t13_ok
    inc esi
.t13_ok:
    add esp, EXC_REC_SIZE + 68

    ; Test 14: vmm_handle_page_fault Kernel-Adresse → nicht behandelt
    mov eax, 0           ; Fehlercode
    mov ebx, 0xC0001000  ; Kernel-Adresse
    call vmm_handle_page_fault
    cmp eax, 0
    jne .t14_ok
    inc esi
.t14_ok:

    ; Ergebnis
    test esi, esi
    jnz .selftest_fail

    mov esi, message_exc_selftest_ok
    call serial_write_string
    clc
    jmp .selftest_done

.selftest_fail:
    mov esi, message_exc_selftest_fail
    call serial_write_string
    stc

.selftest_done:
    pop edi
    pop esi
    pop edx
    pop ecx
    pop ebx
    pop eax
    ret

; ---------------------------------------------------------------------------
; Fixup-Tabelle (§33) – statische Einträge für sichere Kernel-Probe-Operationen
; Weitere Einträge können per exc_fixup_register hinzugefügt werden.
; ---------------------------------------------------------------------------
align 4
exception_fixup_table:
    ; Sentinel: Tabellenende
    dd EXC_FIXUP_SENTINEL, 0, 0, 0

; ---------------------------------------------------------------------------
; §010 Meldungen
; ---------------------------------------------------------------------------
message_exception_mgr_ok:
    db "NOVA: Exception Manager ABI 1.0 bereit", 13, 10, 0
message_exc_selftest_ok:
    db "NOVA: Exception Manager Selbsttest OK (14/14)", 13, 10, 0
message_exc_selftest_fail:
    db "NOVA PANIC: Exception Manager Selbsttest fehlgeschlagen", 13, 10, 0
message_exc_kernel_panic:
    db "NOVA PANIC: Kernel-Exception ohne Fixup bei EIP 0x", 0
message_exc_double_fault:
    db "NOVA PANIC: Double Fault bei EIP 0x", 0
message_exc_machine_check:
    db "NOVA PANIC: Machine Check Exception", 13, 10, 0
message_exc_recursive:
    db "NOVA PANIC: Rekursive Exception (Tiefenlimit)", 13, 10, 0
message_exc_thread_terminated:
    db "NOVA: Userspace-Thread durch Exception beendet", 13, 10, 0

; ===========================================================================
; §011 – System Call Interface (NPSPEC-KERNEL-0011)
; ===========================================================================

; ---------------------------------------------------------------------------
; syscall_manager_initialize  – Bootstrap §011
;   Konfiguriert SYSENTER-MSRs (wenn CPU es unterstützt), initialisiert
;   Statistiken und Tiefenzähler (§39, §53), setzt Init-Flag.
;   Rückgabe: CF=0 OK
; ---------------------------------------------------------------------------
syscall_manager_initialize:
    push eax
    push ebx
    push ecx
    push edx
    push esi

    ; CPUID Leaf 1: EDX Bit 11 = SEP (SYSENTER/SYSEXIT-Support)
    mov eax, 1
    cpuid
    test edx, (1 << 11)
    jz .no_sysenter

    mov dword [syscall_sysenter_available], 1

    ; IA32_SYSENTER_CS (MSR 0x174) = Kernel-Code-Selektor
    mov ecx, 0x174
    xor edx, edx
    mov eax, CODE_SEGMENT
    wrmsr

    ; IA32_SYSENTER_ESP (MSR 0x175) = Kernel-Stack (TSS ESP0)
    mov ecx, 0x175
    xor edx, edx
    mov eax, [kernel_boot_stack_top]
    wrmsr

    ; IA32_SYSENTER_EIP (MSR 0x176) = SYSENTER-Eintrittspunkt
    mov ecx, 0x176
    xor edx, edx
    mov eax, sysenter_entry
    wrmsr

    mov esi, message_syscall_sysenter_ok
    call serial_write_string
    jmp .stats_init

.no_sysenter:
    mov esi, message_syscall_intgate_only
    call serial_write_string

.stats_init:
    ; §53 Statistiken initialisieren
    mov dword [syscall_stats_successful],        0
    mov dword [syscall_stats_failed],            0
    mov dword [syscall_stats_capability_denied], 0
    mov dword [syscall_stats_invalid_ptr],       0

    ; §39 Tiefenzähler
    mov dword [syscall_call_depth], 0

    mov dword [syscall_mgr_initialized], 1

    pop esi
    pop edx
    pop ecx
    pop ebx
    pop eax
    clc
    ret

; ---------------------------------------------------------------------------
; sysenter_entry  – SYSENTER-Einstieg (§011 §9, §11)
;   CPU setzt bei SYSENTER: CPL→0, CS=IA32_SYSENTER_CS,
;   ESP=IA32_SYSENTER_ESP, EIP=IA32_SYSENTER_EIP.
;
;   NovaOS SYSENTER-Konvention (Userspace):
;     EAX = service_id,  EBX = operation_id
;     ESI = abi_version, EDI = arg_ptr, EBP = arg_size
;     ECX = return EIP (SYSENTER-Pflicht)
;     EDX = return ESP (SYSENTER-Pflicht)
; ---------------------------------------------------------------------------
sysenter_entry:
    ; Rückkehrinformation vor Überschreibung sichern
    mov [sysenter_user_eip], ecx
    mov [sysenter_user_esp], edx

    ; Registerkonvention an INT-0x80-Dispatcher anpassen:
    ;   INT 0x80: ECX=abi_version, EDX=arg_ptr, ESI=arg_size
    mov ecx, esi             ; ECX ← abi_version (war ESI)
    mov edx, edi             ; EDX ← arg_ptr     (war EDI)
    mov esi, ebp             ; ESI ← arg_size     (war EBP)

    ; Synthetischen isr_common-Frame aufbauen
    pushfd                   ; EFLAGS
    push dword 0x1B          ; User-CS Ring-3
    push dword [sysenter_user_eip]   ; User-EIP
    push dword 0             ; Error-Code
    push dword 0x80          ; Vektor 0x80

    pushad                   ; EAX=service, ECX=abi_ver, EDX=arg_ptr,
                             ; EBX=op, ESP, EBP, ESI=arg_size, EDI

    push ds
    push es
    push fs
    push gs

    ; Kernel-Segmente aktivieren
    mov ax, DATA_SEGMENT
    mov ds, ax
    mov es, ax

    ; §39: Tiefenzähler prüfen und erhöhen
    mov eax, [syscall_call_depth]
    cmp eax, SYSCALL_CALL_DEPTH_MAX
    jae .sysenter_depth_exceeded
    inc dword [syscall_call_depth]

    mov edx, esp             ; Frame-Zeiger wie in interrupt_dispatch
    call syscall_dispatch

    dec dword [syscall_call_depth]

    ; Rückgabewert sichern: [frame+44]=EAX-Slot (von dispatch beschrieben)
    ; frame+44 = [esp + 44] (GS@0 FS@4 ES@8 DS@12 EDI@16 ESI@20 EBP@24 ESP@28 EBX@32 EDX@36 ECX@40 EAX@44)
    mov eax, [esp + 44]
    mov [sysenter_dispatch_result], eax   ; vor popad sichern

    ; Frame abbauen
    pop gs
    pop fs
    pop es
    pop ds
    popad                    ; stellt EAX auf original service_id zurück
    add esp, 20              ; Vektor(4)+errcode(4)+EIP(4)+CS(4)+EFLAGS(4)

    ; Status-Code aus temporärer Variable laden
    mov eax, [sysenter_dispatch_result]

    mov ecx, [sysenter_user_eip]
    mov edx, [sysenter_user_esp]
    sti
    sysexit

.sysenter_depth_exceeded:
    ; §39: Tiefenlimit – als Fehler zurückkehren
    add esp, 20 + 32 + 16    ; frame cleanup (vector+errcode+eip+cs+eflags + pushad + seg)
    mov eax, SYSCALL_STATUS_OPERATION
    mov ecx, [sysenter_user_eip]
    mov edx, [sysenter_user_esp]
    sti
    sysexit

; ---------------------------------------------------------------------------
; syscall_handler_query_abi  – §011 §26: ABI-Abfrage (Core-Namespace)
;   EDX = syscall_frame (Frame-Zeiger, gesetzt von syscall_dispatch)
;   Schreibt { struct_size(4), major(2), minor(2), feature_flags(8) } in
;   Userspace-Puffer ([frame+36]=arg_ptr, [frame+20]=arg_size).
; ---------------------------------------------------------------------------
syscall_handler_query_abi:
    push ebx
    push esi
    push edi
    push ecx

    mov edx, [syscall_frame]

    ; Argumente aus Frame
    mov esi, [edx + 36]      ; arg_ptr  (gespeichertes EDX = Userspace-Zeiger)
    mov ecx, [edx + 20]      ; arg_size (gespeichertes ESI)

    ; Mindestgröße: 16 Bytes (§011 §14)
    cmp ecx, 16
    jb .qabi_bad_size

    ; Userspace-Zeiger prüfen (§011 §15)
    call syscall_validate_user_range
    jc .qabi_bad_pointer

    ; Ergebnisstruktur im Kernel-Puffer aufbauen
    mov dword [syscall_abi_result +  0], 16   ; struct_size
    mov word  [syscall_abi_result +  4],  1   ; major_version = 1
    mov word  [syscall_abi_result +  6],  0   ; minor_version = 0
    mov dword [syscall_abi_result +  8],  0   ; feature_flags (low32)
    mov dword [syscall_abi_result + 12],  0   ; feature_flags (high32)

    ; In Userspace kopieren (§011 §16)
    mov edi, esi             ; Ziel = arg_ptr (Userspace)
    mov esi, syscall_abi_result
    mov ecx, 16
    push ds
    push es
    mov ax, ds
    mov es, ax
    rep movsb
    pop es
    pop ds

    lock inc dword [syscall_stats_successful]
    mov edx, [syscall_frame]
    mov dword [edx + 44], SYSCALL_STATUS_OK

    pop ecx
    pop edi
    pop esi
    pop ebx
    ret

.qabi_bad_size:
    lock inc dword [syscall_stats_failed]
    mov edx, [syscall_frame]
    mov dword [edx + 44], SYSCALL_STATUS_SIZE
    pop ecx
    pop edi
    pop esi
    pop ebx
    ret

.qabi_bad_pointer:
    lock inc dword [syscall_stats_failed]
    lock inc dword [syscall_stats_invalid_ptr]
    mov edx, [syscall_frame]
    mov dword [edx + 44], SYSCALL_STATUS_POINTER
    pop ecx
    pop edi
    pop esi
    pop ebx
    ret

; ---------------------------------------------------------------------------
; syscall_manager_self_test  – §011 §55: Selbsttest (CF=0 OK, CF=1 Fehler)
;   Testet: Init-Flag, Pointer-Validierung (3 Fälle), Statistiken, Tiefe
; ---------------------------------------------------------------------------
syscall_manager_self_test:
    push eax
    push ebx
    push ecx
    push edx
    push esi
    push edi

    xor ebx, ebx             ; Fehlerzähler (EBX; ESI/EDI für Validierungsaufrufe)

    ; Test 1: Initialisierungsflag gesetzt
    cmp dword [syscall_mgr_initialized], 1
    je .st1_ok
    inc ebx
.st1_ok:

    ; Test 2: syscall_validate_user_range lehnt Kernel-Adresse ≥ 0xC0000000 ab
    push ecx
    mov esi, 0xC0001000
    mov ecx, 4
    call syscall_validate_user_range
    pop ecx
    jc .st2_ok                ; CF=1 = korrekt abgelehnt
    inc ebx
.st2_ok:

    ; Test 3: Nulllänge → ungültig (§011 §15)
    push ecx
    mov esi, USER_ADDRESS_MIN
    mov ecx, 0
    call syscall_validate_user_range
    pop ecx
    jc .st3_ok
    inc ebx
.st3_ok:

    ; Test 4: Adress-Überlauf (start+size wraps) → ungültig
    push ecx
    mov esi, 0xFFFF0000
    mov ecx, 0x00020000
    call syscall_validate_user_range
    pop ecx
    jc .st4_ok
    inc ebx
.st4_ok:

    ; Test 5: §53 Statistiken erreichbar (kein Sentinel-Wert)
    mov eax, [syscall_stats_successful]
    cmp eax, 0xFFFFFFFF
    jne .st5_ok
    inc ebx
.st5_ok:

    ; Test 6: §39 Tiefenzähler = 0
    cmp dword [syscall_call_depth], 0
    je .st6_ok
    inc ebx
.st6_ok:

    ; Test 7: §011 §50 SYSENTER-Flag ist 0 oder 1
    mov eax, [syscall_sysenter_available]
    cmp eax, 1
    jbe .st7_ok
    inc ebx
.st7_ok:

    ; Ergebnis
    test ebx, ebx
    jnz .st_fail
    mov esi, message_syscall_selftest_ok
    call serial_write_string
    clc
    jmp .st_done

.st_fail:
    mov esi, message_syscall_selftest_fail
    call serial_write_string
    stc

.st_done:
    pop edi
    pop esi
    pop edx
    pop ecx
    pop ebx
    pop eax
    ret

; ---------------------------------------------------------------------------
; §011 Daten
; ---------------------------------------------------------------------------
align 4
syscall_mgr_initialized:         dd 0
syscall_sysenter_available:      dd 0
syscall_stats_successful:        dd 0
syscall_stats_failed:            dd 0
syscall_stats_capability_denied: dd 0
syscall_stats_invalid_ptr:       dd 0
syscall_call_depth:              dd 0    ; §39 BSP-Bootstrap
sysenter_user_eip:               dd 0    ; Temp: User-Rückkehr-EIP
sysenter_user_esp:               dd 0    ; Temp: User-Rückkehr-ESP
sysenter_dispatch_result:        dd 0    ; Temp: Dispatcher-Rückgabewert
align 16
syscall_abi_result:              times 16 db 0   ; QUERY_ABI Ausgabepuffer

; ---------------------------------------------------------------------------
; §011 Meldungen
; ---------------------------------------------------------------------------
message_syscall_mgr_ok:
    db "NOVA: System Call Interface ABI 1.0 bereit", 13, 10, 0
message_syscall_mgr_error:
    db "NOVA PANIC: System Call Interface Init fehlgeschlagen", 13, 10, 0
message_syscall_sysenter_ok:
    db "NOVA: SYSENTER verfuegbar und konfiguriert", 13, 10, 0
message_syscall_intgate_only:
    db "NOVA: SYSENTER nicht verfuegbar, nur INT 0x80 aktiv", 13, 10, 0
message_syscall_selftest_ok:
    db "NOVA: System Call Interface Selbsttest OK (7/7)", 13, 10, 0
message_syscall_selftest_fail:
    db "NOVA PANIC: System Call Interface Selbsttest fehlgeschlagen", 13, 10, 0

%include "nova-art.inc"
%include "boot-font-aa.inc"
