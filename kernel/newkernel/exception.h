/* NovaOS C-Kernel - §010 Exception Manager 1.0 (Port aus integration32.inc,
   NPSPEC-KERNEL-0010).

   WICHTIG: Nur die reinen Klassifizierungs-/Record-/Fixup-Funktionen und
   der Selbsttest sind portiert. `exception_manager_dispatch` (der
   eigentliche Einsprung aus isr_common, inkl. Userspace-Thread-Beendigung
   und Scheduler-Interaktion) ist NICHT portiert und NICHT an
   interrupt_dispatch_c angebunden - das wuerde echten Taskwechsel
   voraussetzen, der in scheduler.c bewusst nicht verdrahtet wurde (siehe
   scheduler.h). interrupt.c's Minimal-Dispatch (Exception = HALT) bleibt
   unveraendert. exc_build_record stubbt Thread-/Prozess-ID als 0, da
   per_cpu_current_thread/thread_table aus sync32.inc nicht portiert sind. */
#ifndef NOVA_EXCEPTION_H
#define NOVA_EXCEPTION_H

#include <stdint.h>

#define EXC_CAT_CPU          0u
#define EXC_CAT_MEMORY       1u
#define EXC_CAT_ARITHMETIC   2u
#define EXC_CAT_INSTRUCTION  3u
#define EXC_CAT_DEBUG        4u
#define EXC_CAT_SECURITY     5u
#define EXC_CAT_RESOURCE     6u
#define EXC_CAT_SOFTWARE     7u
#define EXC_CAT_KERNEL_FATAL 8u

#define EXC_ORIGIN_USER   0u
#define EXC_ORIGIN_KERNEL 1u

#define EXC_REC_SIZE 60u

typedef struct {
    uint32_t struct_size;
    uint32_t version;
    uint32_t code;
    uint32_t category;
    uint32_t flags;
    uint32_t process_id;
    uint32_t thread_id;
    uint32_t cpu_id;
    uint32_t ip;
    uint32_t fault_addr;
    uint32_t arch_err;
    uint32_t arch_err_hi;
    uint32_t param_count;
    uint32_t param0;
} exception_record_t;

#define NP_EXC_DIVIDE_BY_ZERO  0u
#define NP_EXC_DEBUG           1u
#define NP_EXC_BREAKPOINT      2u
#define NP_EXC_OVERFLOW        3u
#define NP_EXC_BOUND_VIOLATION 4u
#define NP_EXC_INVALID_OPCODE  5u
#define NP_EXC_DEVICE_UNAVAIL  6u
#define NP_EXC_DOUBLE_FAULT    7u
#define NP_EXC_INVALID_STATE   8u
#define NP_EXC_SEGMENT_VIOL    9u
#define NP_EXC_STACK_FAULT     10u
#define NP_EXC_GPF             11u
#define NP_EXC_PAGE_FAULT      12u
#define NP_EXC_FLOAT_POINT     13u
#define NP_EXC_ALIGNMENT       14u
#define NP_EXC_MACHINE_CHECK   15u
#define NP_EXC_SIMD_FP         16u
#define NP_EXC_CTRL_PROTECT    17u
#define NP_EXC_SECURITY_VIOL   18u
#define NP_EXC_RESOURCE_LIMIT  19u
#define NP_EXC_SOFTWARE_RAISED 20u

#define EXC_FLAG_ORIGIN_USER   0x0001u
#define EXC_FLAG_ORIGIN_KERNEL 0x0002u
#define EXC_FLAG_FIXUP_USED    0x0004u
#define EXC_FLAG_THREAD_TERM   0x0008u
#define EXC_FLAG_PROC_TERM     0x0010u
#define EXC_FLAG_DOUBLE_FAULT  0x0020u
#define EXC_FLAG_MACHINE_CHECK 0x0040u

#define EXC_MAX_DEPTH 4u

void exception_manager_initialize(void);
int exception_manager_self_test(void); /* 0 = ok */

/* x86-Vektor (0..21 bekannt) -> np_exception_code. Unbekannt -> INVALID_STATE. */
uint32_t exc_vector_to_code(uint32_t vector);
/* np_exception_code -> np_exception_category. Unbekannt -> CPU. */
uint32_t exc_code_to_category(uint32_t code);

/* Baut einen Exception Record aus einem isr_common-Frame (siehe
   interrupt_isr.asm: pushad(32)+DS/ES/FS/GS(16)+Vektor(4)+Fehlercode(4)+
   EIP(4)+CS(4)+EFLAGS(4) = 68 Bytes; frame_ptr zeigt auf den Frame-Anfang,
   CS liegt bei frame_ptr+60, EIP bei frame_ptr+56, Fehlercode bei +52). */
void exc_build_record(exception_record_t *out_record, const uint32_t *frame_ptr, uint32_t vector);

/* 0 = nicht gefunden, sonst Recovery-Adresse. */
uint32_t exc_search_fixup(uint32_t fault_eip, uint32_t vector);

/* EAX=Fehlercode, EBX=Fault-Adresse -> 0 = behandelt, != 0 = nicht behandelt.
   Bootstrap-Stub: keine Demand-Paging-Unterstuetzung, Kernel-Adressen
   (>=0xC0000000) und alle Userspace-Adressen gelten als unbehandelbar. */
int vmm_handle_page_fault(uint32_t error_code, uint32_t fault_addr);

#endif
