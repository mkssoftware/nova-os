; Nova Kernel - x86-32 Entry und früher Kernel Context (NPSPEC-KERNEL-0001)
; Validiert NBHP/BIB v1, übernimmt ausschließlich TLV-Daten und ruft danach
; den minimalen Kernel Main auf.
;
; Architektur-Specs (Annotation):
; NPSPEC-ARCH-INTROSPECTION-0001 / NPSPEC-ARCH-STRUCTUREDCONCURRENCY-0001
; NPSPEC-ARCH-ZEROCOPY-0001 / NPSPEC-ARCH-RESOURCEECONOMY-0001
; NPSPEC-ARCH-DETERMINISM-0001 / NPSPEC-ARCH-LOCATIONTRANSPARENCY-0001

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

    call slab_initialize
    jc panic_slab
    call slab_self_test
    jc panic_slab
    mov esi, message_slab_ok
    call serial_write_string

    call aslr_initialize
    jc panic_aslr
    call aslr_self_test
    jc panic_aslr
    mov esi, message_aslr_ok
    call serial_write_string

    call cow_initialize
    jc panic_cow
    call cow_self_test
    jc panic_cow
    mov esi, message_cow_ok
    call serial_write_string

    call oom_initialize
    jc panic_oom
    call oom_self_test
    jc panic_oom
    mov esi, message_oom_ok
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

    call deferred_initialize
    jc panic_deferred
    call deferred_self_test
    jc panic_deferred
    mov esi, message_deferred_ok
    call serial_write_string

    call time_monotonic_initialize
    jc panic_time
    call time_monotonic_self_test
    jc panic_time
    call rtc_initialize
    jc panic_time
    call rtc_self_test
    jc panic_time
    mov esi, message_time_ok
    call serial_write_string

    call clocksource_initialize
    jc panic_clocksource
    call clocksource_self_test
    jc panic_clocksource
    mov esi, message_clocksource_ok
    call serial_write_string

    call hrt_initialize
    jc panic_hrt
    call hrt_self_test
    jc panic_hrt
    mov esi, message_hrt_ok
    call serial_write_string

    call ipc_initialize
    jc panic_ipc
    call ipc_self_test
    jc panic_ipc
    mov esi, message_ipc_ok
    call serial_write_string
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

    call locality_initialize
    jc panic_locality
    call locality_self_test
    jc panic_locality
    mov esi, message_locality_ok
    call serial_write_string

    call kernelbypass_initialize
    jc panic_kernelbypass
    call kernelbypass_self_test
    jc panic_kernelbypass
    mov esi, message_kernelbypass_ok
    call serial_write_string

    call watchdog_initialize
    jc panic_watchdog
    call watchdog_self_test
    jc panic_watchdog
    mov esi, message_watchdog_ok
    call serial_write_string

    call circuitbreaker_initialize
    jc panic_circuitbreaker
    call circuitbreaker_self_test
    jc panic_circuitbreaker
    mov esi, message_circuitbreaker_ok
    call serial_write_string

    call detection_initialize
    jc panic_detection
    call detection_self_test
    jc panic_detection
    mov esi, message_detection_ok
    call serial_write_string

    call restart_initialize
    jc panic_restart
    call restart_self_test
    jc panic_restart
    mov esi, message_restart_ok
    call serial_write_string

    call resource_accounting_initialize
    jc panic_resource_accounting
    call resource_accounting_self_test
    jc panic_resource_accounting
    mov esi, message_resource_accounting_ok
    call serial_write_string

    call failover_initialize
    jc panic_failover
    call failover_self_test
    jc panic_failover
    mov esi, message_failover_ok
    call serial_write_string

    call containment_initialize
    jc panic_containment
    call containment_self_test
    jc panic_containment
    mov esi, message_containment_ok
    call serial_write_string

    call checkpoint_initialize
    jc panic_checkpoint
    call checkpoint_self_test
    jc panic_checkpoint
    mov esi, message_checkpoint_ok
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

    call security_audit_initialize
    jc panic_security_audit
    call security_audit_self_test
    jc panic_security_audit
    mov esi, message_security_audit_ok
    call serial_write_string

    call mac_initialize
    jc panic_mac
    call mac_self_test
    jc panic_mac
    mov esi, message_mac_ok
    call serial_write_string

    call dac_initialize
    jc panic_dac
    call dac_self_test
    jc panic_dac
    mov esi, message_dac_ok
    call serial_write_string

    call rbac_initialize
    jc panic_rbac
    call rbac_self_test
    jc panic_rbac
    mov esi, message_rbac_ok
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
    jc panic_kog_init
    call kog_self_test
    jc panic_kog_test
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

    call trace_initialize
    jc panic_trace
    call trace_self_test
    jc panic_trace
    mov esi, message_trace_ok
    call serial_write_string

    call metrics_initialize
    jc panic_metrics
    call metrics_self_test
    jc panic_metrics
    mov esi, message_metrics_ok
    call serial_write_string

    call obslog_initialize
    jc panic_log_ring
    call obslog_self_test
    jc panic_log_ring
    mov esi, message_log_ring_ok
    call serial_write_string

    call intro_initialize
    jc panic_introspection
    call intro_self_test
    jc panic_introspection
    mov esi, message_introspection_ok
    call serial_write_string

    call prof_initialize
    jc panic_profiling
    call prof_self_test
    jc panic_profiling
    mov esi, message_profiling_ok
    call serial_write_string

    call csprng_initialize
    jc panic_csprng
    call csprng_self_test
    jc panic_csprng
    mov esi, message_csprng_ok
    call serial_write_string

    call cpuidle_initialize
    jc panic_cpuidle
    call cpuidle_self_test
    jc panic_cpuidle
    mov esi, message_cpuidle_ok
    call serial_write_string

    call thermal_initialize
    jc panic_thermal
    call thermal_self_test
    jc panic_thermal
    mov esi, message_thermal_ok
    call serial_write_string

    call identity_initialize
    jc panic_identity
    call identity_self_test
    jc panic_identity
    mov esi, message_identity_ok
    call serial_write_string

    call trust_initialize
    jc panic_trust
    call trust_self_test
    jc panic_trust
    mov esi, message_trust_ok
    call serial_write_string

    call adapt_initialize
    jc panic_adaptive
    call adapt_self_test
    jc panic_adaptive
    mov esi, message_adaptive_ok
    call serial_write_string

    call radmit_initialize
    jc panic_radmit
    call radmit_self_test
    jc panic_radmit
    mov esi, message_radmit_ok
    call serial_write_string

    call excon_initialize
    jc panic_excon
    call excon_self_test
    jc panic_excon
    mov esi, message_excon_ok
    call serial_write_string

    call tmpiso_initialize
    jc panic_tmpiso
    call tmpiso_self_test
    jc panic_tmpiso
    mov esi, message_tmpiso_ok
    call serial_write_string

    call selfheal_initialize
    jc panic_selfheal
    call selfheal_self_test
    jc panic_selfheal
    mov esi, message_selfheal_ok
    call serial_write_string

    call privlabel_initialize
    jc panic_privlabel
    call privlabel_self_test
    jc panic_privlabel
    mov esi, message_privlabel_ok
    call serial_write_string

    call retry_initialize
    jc panic_retry
    call retry_self_test
    jc panic_retry
    mov esi, message_retry_ok
    call serial_write_string

    call rmode_initialize
    jc panic_rmode
    call rmode_self_test
    jc panic_rmode
    mov esi, message_rmode_ok
    call serial_write_string

    call rtv_initialize
    jc panic_rtv
    call rtv_self_test
    jc panic_rtv
    mov esi, message_rtv_ok
    call serial_write_string

    call sgraph_initialize
    jc panic_sgraph
    call sgraph_self_test
    jc panic_sgraph
    mov esi, message_sgraph_ok
    call serial_write_string

    call fsm_initialize
    jc panic_fsm
    call fsm_self_test
    jc panic_fsm
    mov esi, message_fsm_ok
    call serial_write_string

    call txroll_initialize
    jc panic_txroll
    call txroll_self_test
    jc panic_txroll
    mov esi, message_txroll_ok
    call serial_write_string

    call fi_initialize
    jc panic_fi
    call fi_self_test
    jc panic_fi
    mov esi, message_fi_ok
    call serial_write_string

    call fclass_initialize
    jc panic_fclass
    call fclass_self_test
    jc panic_fclass
    mov esi, message_fclass_ok
    call serial_write_string

    call rpol_initialize
    jc panic_rpol
    call rpol_self_test
    jc panic_rpol
    mov esi, message_rpol_ok
    call serial_write_string

    call rverif_initialize
    jc panic_rverif
    call rverif_self_test
    jc panic_rverif
    mov esi, message_rverif_ok
    call serial_write_string

    call ssnap_initialize
    jc panic_ssnap
    call ssnap_self_test
    jc panic_ssnap
    mov esi, message_ssnap_ok
    call serial_write_string

    call shist_initialize
    jc panic_shist
    call shist_self_test
    jc panic_shist
    mov esi, message_shist_ok
    call serial_write_string

    call srec_initialize
    jc panic_srec
    call srec_self_test
    jc panic_srec
    mov esi, message_srec_ok
    call serial_write_string

    call txlog_initialize
    jc panic_txlog
    call txlog_self_test
    jc panic_txlog
    mov esi, message_txlog_ok
    call serial_write_string

    call tbarrier_initialize
    jc panic_tbarrier
    call tbarrier_self_test
    jc panic_tbarrier
    mov esi, message_tbarrier_ok
    call serial_write_string

    call tcomp_initialize
    jc panic_tcomp
    call tcomp_self_test
    jc panic_tcomp
    mov esi, message_tcomp_ok
    call serial_write_string

    call redundancy_initialize
    jc panic_redundancy
    call redundancy_self_test
    jc panic_redundancy
    mov esi, message_redundancy_ok
    call serial_write_string

    call chaos_initialize
    jc panic_chaos
    call chaos_self_test
    jc panic_chaos
    mov esi, message_chaos_ok
    call serial_write_string

    call resilisolation_initialize
    jc panic_resilisolation
    call resilisolation_self_test
    jc panic_resilisolation
    mov esi, message_resilisolation_ok
    call serial_write_string

    call raccount_initialize
    jc panic_raccount
    call raccount_self_test
    jc panic_raccount
    mov esi, message_raccount_ok
    call serial_write_string

    call rguarant_initialize
    jc panic_rguarant
    call rguarant_self_test
    jc panic_rguarant
    mov esi, message_rguarant_ok
    call serial_write_string

    call rreclaim_initialize
    jc panic_rreclaim
    call rreclaim_self_test
    jc panic_rreclaim
    mov esi, message_rreclaim_ok
    call serial_write_string

    call sdiag_initialize
    jc panic_sdiag
    call sdiag_self_test
    jc panic_sdiag
    mov esi, message_sdiag_ok
    call serial_write_string

    call sconf_initialize
    jc panic_sconf
    call sconf_self_test
    jc panic_sconf
    mov esi, message_sconf_ok
    call serial_write_string

    call apred_initialize
    jc panic_apred
    call apred_self_test
    jc panic_apred
    mov esi, message_apred_ok
    call serial_write_string

    call selfopt_initialize
    jc panic_selfopt
    call selfopt_self_test
    jc panic_selfopt
    mov esi, message_selfopt_ok
    call serial_write_string

    call selfprot_initialize
    jc panic_selfprot
    call selfprot_self_test
    jc panic_selfprot
    mov esi, message_selfprot_ok
    call serial_write_string

    call rresv_initialize
    jc panic_rresv
    call rresv_self_test
    jc panic_rresv
    mov esi, message_rresv_ok
    call serial_write_string

    call aconstraint_initialize
    jc panic_aconstraint
    call aconstraint_self_test
    jc panic_aconstraint
    mov esi, message_aconstraint_ok
    call serial_write_string

    call autopol_initialize
    jc panic_autopol
    call autopol_self_test
    jc panic_autopol
    mov esi, message_autopol_ok
    call serial_write_string

    call aschedsw_initialize
    jc panic_aschedsw
    call aschedsw_self_test
    jc panic_aschedsw
    mov esi, message_aschedsw_ok
    call serial_write_string

    call sver_initialize
    jc panic_sver
    call sver_self_test
    jc panic_sver
    mov esi, message_sver_ok
    call serial_write_string

    call sactual_initialize
    jc panic_sactual
    call sactual_self_test
    jc panic_sactual
    mov esi, message_sactual_ok
    call serial_write_string

    call sdesired_initialize
    jc panic_sdesired
    call sdesired_self_test
    jc panic_sdesired
    mov esi, message_sdesired_ok
    call serial_write_string

    call txres_initialize
    jc panic_txres
    call txres_self_test
    jc panic_txres
    mov esi, message_txres_ok
    call serial_write_string

    call txsys_initialize
    jc panic_txsys
    call txsys_self_test
    jc panic_txsys
    mov esi, message_txsys_ok
    call serial_write_string

    call rarbit_initialize
    jc panic_rarbit
    call rarbit_self_test
    jc panic_rarbit
    mov esi, message_rarbit_ok
    call serial_write_string

    call acache_initialize
    jc panic_acache
    call acache_self_test
    jc panic_acache
    mov esi, message_acache_ok
    call serial_write_string

    call apower_initialize
    jc panic_apower
    call apower_self_test
    jc panic_apower
    mov esi, message_apower_ok
    call serial_write_string

    call pollearn_initialize
    jc panic_pollearn
    call pollearn_self_test
    jc panic_pollearn
    mov esi, message_pollearn_ok
    call serial_write_string

    call prederr_initialize
    jc panic_prederr
    call prederr_self_test
    jc panic_prederr
    mov esi, message_prederr_ok
    call serial_write_string

    call aprefetch_initialize
    jc panic_aprefetch
    call aprefetch_self_test
    jc panic_aprefetch
    mov esi, message_aprefetch_ok
    call serial_write_string

    call apreload_initialize
    jc panic_apreload
    call apreload_self_test
    jc panic_apreload
    mov esi, message_apreload_ok
    call serial_write_string

    call astorage_initialize
    jc panic_astorage
    call astorage_self_test
    jc panic_astorage
    mov esi, message_astorage_ok
    call serial_write_string

    call anetwork_initialize
    jc panic_anetwork
    call anetwork_self_test
    jc panic_anetwork
    mov esi, message_anetwork_ok
    call serial_write_string

    call uoverride_initialize
    jc panic_uoverride
    call uoverride_self_test
    jc panic_uoverride
    mov esi, message_uoverride_ok
    call serial_write_string

    call rdeadline_initialize
    jc panic_rdeadline
    call rdeadline_self_test
    jc panic_rdeadline
    mov esi, message_rdeadline_ok
    call serial_write_string

    call reconomy_initialize
    jc panic_reconomy
    call reconomy_self_test
    jc panic_reconomy
    mov esi, message_reconomy_ok
    call serial_write_string

    call renergy_initialize
    jc panic_renergy
    call renergy_self_test
    jc panic_renergy
    mov esi, message_renergy_ok
    call serial_write_string

    call rlatency_initialize
    jc panic_rlatency
    call rlatency_self_test
    jc panic_rlatency
    mov esi, message_rlatency_ok
    call serial_write_string

    call sglobal_initialize
    jc panic_sglobal
    call sglobal_self_test
    jc panic_sglobal
    mov esi, message_sglobal_ok
    call serial_write_string

    call txconfig_initialize
    jc panic_txconfig
    call txconfig_self_test
    jc panic_txconfig
    mov esi, message_txconfig_ok
    call serial_write_string

    call txmulti_initialize
    jc panic_txmulti
    call txmulti_self_test
    jc panic_txmulti
    mov esi, message_txmulti_ok
    call serial_write_string

    call vtemporal_initialize
    jc panic_vtemporal
    call vtemporal_self_test
    jc panic_vtemporal
    mov esi, message_vtemporal_ok
    call serial_write_string

    call vinfoflow_initialize
    jc panic_vinfoflow
    call vinfoflow_self_test
    jc panic_vinfoflow
    mov esi, message_vinfoflow_ok
    call serial_write_string

    call vcapsafe_initialize
    jc panic_vcapsafe
    call vcapsafe_self_test
    jc panic_vcapsafe
    mov esi, message_vcapsafe_ok
    call serial_write_string

    call vmemsafe_initialize
    jc panic_vmemsafe
    call vmemsafe_self_test
    jc panic_vmemsafe
    mov esi, message_vmemsafe_ok
    call serial_write_string

    call vfs_initialize
    jc panic_vfs
    call vfs_self_test
    jc panic_vfs
    mov esi, message_vfs_ok
    call serial_write_string

    call cap_integration_initialize
    jc panic_cap_integ
    call cap_integration_self_test
    jc panic_cap_integ
    mov esi, message_cap_integ_ok
    call serial_write_string

    call capdis_initialize
    jc panic_capdis
    call capdis_self_test
    jc panic_capdis
    mov esi, message_capdis_ok
    call serial_write_string

    call capneg_initialize
    jc panic_capneg
    call capneg_self_test
    jc panic_capneg
    mov esi, message_capneg_ok
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
    call futex_initialize
    jc panic_futex
    call futex_self_test
    jc panic_futex
    mov esi, message_sync_ok
    call serial_write_string

    call deadlock_initialize
    jc panic_deadlock
    call deadlock_self_test
    jc panic_deadlock
    mov esi, message_deadlock_ok
    call serial_write_string

    call pi_initialize
    jc panic_pi_mutex
    call pi_self_test
    jc panic_pi_mutex
    mov esi, message_pi_ok
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

    call rt_scheduler_initialize
    jc panic_rt_scheduler
    call rt_scheduler_self_test
    jc panic_rt_scheduler
    mov esi, message_rt_scheduler_ok
    call serial_write_string

    call cfs_initialize
    jc panic_cfs
    call cfs_self_test
    jc panic_cfs
    mov esi, message_cfs_ok
    call serial_write_string

    call qos_initialize
    jc panic_qos
    call qos_self_test
    jc panic_qos
    mov esi, message_qos_ok
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
    ; VFS already initialized early (before cap_integration_self_test)
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

    ; Integration-Test: alle Subsysteme gemeinsam (nach vollständigem Bootstrap)
    call integration_self_test
    jc panic_integration
    mov esi, message_integration_ok
    call serial_write_string

    call kernel_main

panic_integration:
    mov eax, 0x00003051
    mov edx, 78
    mov esi, message_integration_error
    jmp kernel_panic

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

panic_locality:
    mov eax, 0x00002026
    mov edx, 38
    mov esi, message_locality_error
    jmp kernel_panic

panic_kernelbypass:
    mov eax, 0x00002027
    mov edx, 39
    mov esi, message_kernelbypass_error
    jmp kernel_panic

panic_watchdog:
    mov eax, 0x00003033
    mov edx, 48
    mov esi, message_watchdog_error
    jmp kernel_panic

panic_circuitbreaker:
    mov eax, 0x00003037
    mov edx, 52
    mov esi, message_circuitbreaker_error
    jmp kernel_panic

panic_detection:
    mov eax, 0x00003038
    mov edx, 53
    mov esi, message_detection_error
    jmp kernel_panic

panic_restart:
    mov eax, 0x0000303C
    mov edx, 57
    mov esi, message_restart_error
    jmp kernel_panic

panic_resource_accounting:
    mov eax, 0x0000303D
    mov edx, 58
    mov esi, message_resource_accounting_error
    jmp kernel_panic

panic_failover:
    mov eax, 0x00003044
    mov edx, 65
    mov esi, message_failover_error
    jmp kernel_panic

panic_containment:
    mov eax, 0x00003045
    mov edx, 66
    mov esi, message_containment_error
    jmp kernel_panic

panic_checkpoint:
    mov eax, 0x00003046
    mov edx, 67
    mov esi, message_checkpoint_error
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

panic_security_audit:
    mov eax, 0x0000303E
    mov edx, 59
    mov esi, message_security_audit_error
    jmp kernel_panic

panic_mac:
    mov eax, 0x0000304C
    mov edx, 73
    mov esi, message_mac_error
    jmp kernel_panic

panic_dac:
    mov eax, 0x0000304D
    mov edx, 74
    mov esi, message_dac_error
    jmp kernel_panic

panic_rbac:
    mov eax, 0x0000304E
    mov edx, 75
    mov esi, message_rbac_error
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

panic_kog_init:
    mov eax, 0x00000014
    mov edx, 0x4B4F4700             ; "KOG\0"
    mov esi, message_kog_init_error
    jmp kernel_panic

panic_kog_test:
    mov eax, 0x00000019
    mov edx, 0x4B4F4700             ; "KOG\0"
    mov esi, message_kog_test_error
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

panic_trace:
    mov eax, 0x00003035
    mov edx, 50
    mov esi, message_trace_error
    jmp kernel_panic

panic_metrics:
    mov eax, 0x00003039
    mov edx, 54
    mov esi, message_metrics_error
    jmp kernel_panic

panic_log_ring:
    mov eax, 0x00003052
    mov edx, 79
    mov esi, message_log_ring_error
    jmp kernel_panic

panic_introspection:
    mov eax, 0x00003053
    mov edx, 80
    mov esi, message_introspection_error
    jmp kernel_panic

panic_profiling:
    mov eax, 0x00003054
    mov edx, 81
    mov esi, message_profiling_error
    jmp kernel_panic

panic_csprng:
    mov eax, 0x00003055
    mov edx, 82
    mov esi, message_csprng_error
    jmp kernel_panic

panic_cpuidle:
    mov eax, 0x00003056
    mov edx, 83
    mov esi, message_cpuidle_error
    jmp kernel_panic

panic_thermal:
    mov eax, 0x00003057
    mov edx, 84
    mov esi, message_thermal_error
    jmp kernel_panic

panic_identity:
    mov eax, 0x00003058
    mov edx, 85
    mov esi, message_identity_error
    jmp kernel_panic

panic_trust:
    mov eax, 0x00003059
    mov edx, 86
    mov esi, message_trust_error
    jmp kernel_panic

panic_adaptive:
    mov eax, 0x0000305A
    mov edx, 87
    mov esi, message_adaptive_error
    jmp kernel_panic

panic_radmit:
    mov eax, 0x0000305B
    mov edx, 88
    mov esi, message_radmit_error
    jmp kernel_panic

panic_excon:
    mov eax, 0x0000305C
    mov edx, 89
    mov esi, message_excon_error
    jmp kernel_panic

panic_tmpiso:
    mov eax, 0x0000305D
    mov edx, 90
    mov esi, message_tmpiso_error
    jmp kernel_panic

panic_selfheal:
    mov eax, 0x0000305E
    mov edx, 91
    mov esi, message_selfheal_error
    jmp kernel_panic

panic_privlabel:
    mov eax, 0x0000305F
    mov edx, 92
    mov esi, message_privlabel_error
    jmp kernel_panic

panic_retry:
    mov eax, 0x00003060
    mov edx, 93
    mov esi, message_retry_error
    jmp kernel_panic

panic_rmode:
    mov eax, 0x00003061
    mov edx, 94
    mov esi, message_rmode_error
    jmp kernel_panic

panic_rtv:
    mov eax, 0x00003062
    mov edx, 95
    mov esi, message_rtv_error
    jmp kernel_panic

panic_sgraph:
    mov eax, 0x00003063
    mov edx, 96
    mov esi, message_sgraph_error
    jmp kernel_panic

panic_fsm:
    mov eax, 0x00003064
    mov edx, 97
    mov esi, message_fsm_error
    jmp kernel_panic

panic_txroll:
    mov eax, 0x00003065
    mov edx, 98
    mov esi, message_txroll_error
    jmp kernel_panic

panic_fi:
    mov eax, 0x00003066
    mov edx, 99
    mov esi, message_fi_error
    jmp kernel_panic

panic_fclass:
    mov eax, 0x00003067
    mov edx, 100
    mov esi, message_fclass_error
    jmp kernel_panic

panic_rpol:
    mov eax, 0x00003068
    mov edx, 101
    mov esi, message_rpol_error
    jmp kernel_panic

panic_rverif:
    mov eax, 0x00003069
    mov edx, 102
    mov esi, message_rverif_error
    jmp kernel_panic

panic_ssnap:
    mov eax, 0x0000306A
    mov edx, 103
    mov esi, message_ssnap_error
    jmp kernel_panic

panic_shist:
    mov eax, 0x0000306B
    mov edx, 104
    mov esi, message_shist_error
    jmp kernel_panic

panic_srec:
    mov eax, 0x0000306C
    mov edx, 105
    mov esi, message_srec_error
    jmp kernel_panic

panic_txlog:
    mov eax, 0x0000306D
    mov edx, 106
    mov esi, message_txlog_error
    jmp kernel_panic

panic_tbarrier:
    mov eax, 0x0000306E
    mov edx, 107
    mov esi, message_tbarrier_error
    jmp kernel_panic

panic_tcomp:
    mov eax, 0x0000306F
    mov edx, 108
    mov esi, message_tcomp_error
    jmp kernel_panic

panic_redundancy:
    mov eax, 0x00003070
    mov edx, 109
    mov esi, message_redundancy_error
    jmp kernel_panic

panic_chaos:
    mov eax, 0x00003071
    mov edx, 110
    mov esi, message_chaos_error
    jmp kernel_panic

panic_resilisolation:
    mov eax, 0x00003072
    mov edx, 111
    mov esi, message_resilisolation_error
    jmp kernel_panic

panic_raccount:
    mov eax, 0x00003073
    mov edx, 112
    mov esi, message_raccount_error
    jmp kernel_panic

panic_rguarant:
    mov eax, 0x00003074
    mov edx, 113
    mov esi, message_rguarant_error
    jmp kernel_panic

panic_rreclaim:
    mov eax, 0x00003075
    mov edx, 114
    mov esi, message_rreclaim_error
    jmp kernel_panic

panic_sdiag:
    mov eax, 0x00003076
    mov edx, 115
    mov esi, message_sdiag_error
    jmp kernel_panic

panic_sconf:
    mov eax, 0x00003077
    mov edx, 116
    mov esi, message_sconf_error
    jmp kernel_panic

panic_apred:
    mov eax, 0x00003078
    mov edx, 117
    mov esi, message_apred_error
    jmp kernel_panic

panic_selfopt:
    mov eax, 0x00003079
    mov edx, 118
    mov esi, message_selfopt_error
    jmp kernel_panic

panic_selfprot:
    mov eax, 0x0000307A
    mov edx, 119
    mov esi, message_selfprot_error
    jmp kernel_panic

panic_rresv:
    mov eax, 0x0000307B
    mov edx, 120
    mov esi, message_rresv_error
    jmp kernel_panic

panic_aconstraint:
    mov eax, 0x0000307C
    mov edx, 121
    mov esi, message_aconstraint_error
    jmp kernel_panic

panic_autopol:
    mov eax, 0x0000307D
    mov edx, 122
    mov esi, message_autopol_error
    jmp kernel_panic

panic_aschedsw:
    mov eax, 0x0000307E
    mov edx, 123
    mov esi, message_aschedsw_error
    jmp kernel_panic

panic_sver:
    mov eax, 0x0000307F
    mov edx, 124
    mov esi, message_sver_error
    jmp kernel_panic

panic_sactual:
    mov eax, 0x00003080
    mov edx, 125
    mov esi, message_sactual_error
    jmp kernel_panic

panic_sdesired:
    mov eax, 0x00003081
    mov edx, 126
    mov esi, message_sdesired_error
    jmp kernel_panic

panic_txres:
    mov eax, 0x00003082
    mov edx, 127
    mov esi, message_txres_error
    jmp kernel_panic

panic_txsys:
    mov eax, 0x00003083
    mov edx, 128
    mov esi, message_txsys_error
    jmp kernel_panic

panic_rarbit:
    mov eax, 0x00003084
    mov edx, 129
    mov esi, message_rarbit_error
    jmp kernel_panic

panic_acache:
    mov eax, 0x00003085
    mov edx, 130
    mov esi, message_acache_error
    jmp kernel_panic

panic_apower:
    mov eax, 0x00003086
    mov edx, 131
    mov esi, message_apower_error
    jmp kernel_panic

panic_pollearn:
    mov eax, 0x00003087
    mov edx, 132
    mov esi, message_pollearn_error
    jmp kernel_panic

panic_prederr:
    mov eax, 0x00003088
    mov edx, 133
    mov esi, message_prederr_error
    jmp kernel_panic

panic_aprefetch:
    mov eax, 0x00003089
    mov edx, 134
    mov esi, message_aprefetch_error
    jmp kernel_panic

panic_apreload:
    mov eax, 0x0000308A
    mov edx, 135
    mov esi, message_apreload_error
    jmp kernel_panic

panic_astorage:
    mov eax, 0x0000308B
    mov edx, 136
    mov esi, message_astorage_error
    jmp kernel_panic

panic_anetwork:
    mov eax, 0x0000308C
    mov edx, 137
    mov esi, message_anetwork_error
    jmp kernel_panic

panic_uoverride:
    mov eax, 0x0000308D
    mov edx, 138
    mov esi, message_uoverride_error
    jmp kernel_panic

panic_rdeadline:
    mov eax, 0x0000308E
    mov edx, 139
    mov esi, message_rdeadline_error
    jmp kernel_panic

panic_reconomy:
    mov eax, 0x0000308F
    mov edx, 140
    mov esi, message_reconomy_error
    jmp kernel_panic

panic_renergy:
    mov eax, 0x00003090
    mov edx, 141
    mov esi, message_renergy_error
    jmp kernel_panic

panic_rlatency:
    mov eax, 0x00003091
    mov edx, 142
    mov esi, message_rlatency_error
    jmp kernel_panic

panic_sglobal:
    mov eax, 0x00003092
    mov edx, 143
    mov esi, message_sglobal_error
    jmp kernel_panic

panic_txconfig:
    mov eax, 0x00003093
    mov edx, 144
    mov esi, message_txconfig_error
    jmp kernel_panic

panic_txmulti:
    mov eax, 0x00003094
    mov edx, 145
    mov esi, message_txmulti_error
    jmp kernel_panic

panic_vtemporal:
    mov eax, 0x00003095
    mov edx, 146
    mov esi, message_vtemporal_error
    jmp kernel_panic

panic_vinfoflow:
    mov eax, 0x00003096
    mov edx, 147
    mov esi, message_vinfoflow_error
    jmp kernel_panic

panic_vcapsafe:
    mov eax, 0x00003097
    mov edx, 148
    mov esi, message_vcapsafe_error
    jmp kernel_panic

panic_vmemsafe:
    mov eax, 0x00003098
    mov edx, 149
    mov esi, message_vmemsafe_error
    jmp kernel_panic

panic_cap_integ:
    mov eax, 0x00000019
    mov edx, 0x43494E54             ; "CINT"
    mov esi, message_cap_integ_error
    jmp kernel_panic

panic_capdis:
    mov eax, 0x0000304F
    mov edx, 76
    mov esi, message_capdis_error
    jmp kernel_panic

panic_capneg:
    mov eax, 0x00003050
    mov edx, 77
    mov esi, message_capneg_error
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

panic_futex:
    mov eax, 0x0000303A
    mov edx, 55
    mov esi, message_futex_error
    jmp kernel_panic

panic_deadlock:
    mov eax, 0x00003040
    mov edx, 61
    mov esi, message_deadlock_error
    jmp kernel_panic

panic_pi_mutex:
    mov eax, 0x00003041
    mov edx, 62
    mov esi, message_pi_error
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

panic_rt_scheduler:
    mov eax, 0x0000303F
    mov edx, 60
    mov esi, message_rt_scheduler_error
    jmp kernel_panic

panic_cfs:
    mov eax, 0x0000304A
    mov edx, 71
    mov esi, message_cfs_error
    jmp kernel_panic

panic_qos:
    mov eax, 0x0000304B
    mov edx, 72
    mov esi, message_qos_error
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

panic_slab:
    mov eax, 0x0000303B
    mov edx, 56
    mov esi, message_slab_error
    jmp kernel_panic

panic_aslr:
    mov eax, 0x00003047
    mov edx, 68
    mov esi, message_aslr_error
    jmp kernel_panic

panic_cow:
    mov eax, 0x00003048
    mov edx, 69
    mov esi, message_cow_error
    jmp kernel_panic

panic_oom:
    mov eax, 0x00003049
    mov edx, 70
    mov esi, message_oom_error
    jmp kernel_panic

panic_interrupt_manager:
    mov eax, 0x00002007
    mov edx, 7
    mov esi, message_interrupts_error
    jmp kernel_panic

panic_deferred:
    mov eax, 0x00003034
    mov edx, 49
    mov esi, message_deferred_error
    jmp kernel_panic

panic_time:
    mov eax, 0x00003036
    mov edx, 51
    mov esi, message_time_error
    jmp kernel_panic

panic_clocksource:
    mov eax, 0x00003042
    mov edx, 63
    mov esi, message_clocksource_error
    jmp kernel_panic

panic_hrt:
    mov eax, 0x00003043
    mov edx, 64
    mov esi, message_hrt_error
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

; ===========================================================================
; Kernel-Subsystem-Module
; Jedes Subsystem ist in einer eigenen .inc-Datei unter arch/x86_64/modules/
; Die Reihenfolge entspricht der Initialisierungsreihenfolge.
; ===========================================================================

; Hardware-Abstraktionsschicht, BIB-Validierung, Firmware-Interface
%include "arch/x86_64/modules/hal32.inc"

; Speicherverwaltung (PMM, Paging, Heap, Slab, ASLR, COW, OOM)
%include "arch/x86_64/modules/memory32.inc"

; Interrupt-Controller, IDT, Timer
%include "arch/x86_64/modules/interrupt32.inc"

; Semantik / Zustand / Speicher / Dateisystem-Subsysteme
%include "arch/x86_64/semantic32.inc"
%include "arch/x86_64/semantic_core32.inc"
%include "arch/x86_64/state32.inc"
%include "arch/x86_64/storage32.inc"
%include "arch/x86_64/novafs32.inc"
%include "arch/x86_64/vfs32.inc"
%include "arch/x86_64/explorer32.inc"

; IPC, Messaging, Service-Registry
%include "arch/x86_64/modules/ipc32.inc"

; Prozessverwaltung
%include "arch/x86_64/modules/process32.inc"

; Concurrency (Tasks, Structured Concurrency, Deadlines, TaskGroups)
%include "arch/x86_64/modules/concurrency32.inc"

; I/O-Subsystem, HAL-Topology, DataMove
%include "arch/x86_64/modules/io32.inc"

; Resilience (Watchdog, CircuitBreaker, Failover, Checkpoint)
%include "arch/x86_64/modules/resilience32.inc"

; I/O-Scheduler und QoS-Priorisierung
%include "arch/x86_64/modules/io_qos32.inc"

; Sicherheit und Capability-Framework
%include "arch/x86_64/modules/security32.inc"

; Observability (Diagnose, Tracing, Metriken, Profiling)
%include "arch/x86_64/modules/observability32.inc"

; Adaptive / Autonomie / State / Transaktionen / Verifikation
%include "arch/x86_64/modules/adaptive32.inc"

; CAP-Integration 1.0
%include "arch/x86_64/modules/cap_integration32.inc"

; Synchronisationsprimitive (Spinlock bis Priority-Inheritance)
%include "arch/x86_64/modules/sync32.inc"

; Deferred-IRQ, Zeitdienste, Clocksource, HRT
%include "arch/x86_64/modules/time32.inc"

; Haupt-Scheduler, Work-Stealing, CFS, RT, QoS-Scheduler
%include "arch/x86_64/modules/scheduler32.inc"

; Integration-Pipelines, UI, Syscall-Interface, Datensegmente
%include "arch/x86_64/modules/integration32.inc"

; Boot-Assets
%include "nova-art.inc"
%include "boot-font-aa.inc"
