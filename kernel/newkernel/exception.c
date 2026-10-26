/* NovaOS C-Kernel - §010 Exception Manager 1.0 (Port aus integration32.inc).
   Siehe exception.h fuer den Hinweis zum nicht portierten Dispatch. */
#include "exception.h"
#include "serial.h"

static int exception_mgr_initialized = 0;
static uint32_t exception_stats_total = 0;
static uint32_t exception_depth_table[4];
static exception_record_t exception_emergency_record;
static exception_record_t exception_last_record;

/* Sentinel-only: keine Fixup-Eintraege registriert (wie im alten ASM;
   exc_fixup_register existiert dort nur als Kommentar-Hinweis, nicht als
   implementierte Funktion). */
static const struct {
    uint32_t fault_start;
    uint32_t fault_end;
    uint32_t recovery;
    uint32_t allowed_vecs;
} exception_fixup_table[] = {
    {0xFFFFFFFFu, 0, 0, 0}, /* Sentinel */
};

void exception_manager_initialize(void) {
    exception_stats_total = 0;
    for (uint32_t i = 0; i < 4; ++i) exception_depth_table[i] = 0;
    exception_emergency_record = (exception_record_t){0};
    exception_last_record = (exception_record_t){0};

    exception_mgr_initialized = 1;
    serial_puts("NOVA: Exception Manager ABI 1.0 bereit\r\n");
}

uint32_t exc_vector_to_code(uint32_t vector) {
    switch (vector) {
        case 0: return NP_EXC_DIVIDE_BY_ZERO;
        case 1: return NP_EXC_DEBUG;
        case 3: return NP_EXC_BREAKPOINT;
        case 4: return NP_EXC_OVERFLOW;
        case 5: return NP_EXC_BOUND_VIOLATION;
        case 6: return NP_EXC_INVALID_OPCODE;
        case 7: return NP_EXC_DEVICE_UNAVAIL;
        case 8: return NP_EXC_DOUBLE_FAULT;
        case 10: return NP_EXC_INVALID_STATE;
        case 11: return NP_EXC_SEGMENT_VIOL;
        case 12: return NP_EXC_STACK_FAULT;
        case 13: return NP_EXC_GPF;
        case 14: return NP_EXC_PAGE_FAULT;
        case 16: return NP_EXC_FLOAT_POINT;
        case 17: return NP_EXC_ALIGNMENT;
        case 18: return NP_EXC_MACHINE_CHECK;
        case 19: return NP_EXC_SIMD_FP;
        case 21: return NP_EXC_CTRL_PROTECT;
        default: return NP_EXC_INVALID_STATE;
    }
}

uint32_t exc_code_to_category(uint32_t code) {
    switch (code) {
        case NP_EXC_PAGE_FAULT: return EXC_CAT_MEMORY;
        case NP_EXC_STACK_FAULT:
        case NP_EXC_ALIGNMENT:
        case NP_EXC_RESOURCE_LIMIT: return EXC_CAT_RESOURCE;
        case NP_EXC_DIVIDE_BY_ZERO:
        case NP_EXC_OVERFLOW:
        case NP_EXC_FLOAT_POINT:
        case NP_EXC_SIMD_FP: return EXC_CAT_ARITHMETIC;
        case NP_EXC_DEBUG:
        case NP_EXC_BREAKPOINT: return EXC_CAT_DEBUG;
        case NP_EXC_INVALID_OPCODE:
        case NP_EXC_DEVICE_UNAVAIL:
        case NP_EXC_CTRL_PROTECT: return EXC_CAT_INSTRUCTION;
        case NP_EXC_DOUBLE_FAULT:
        case NP_EXC_MACHINE_CHECK: return EXC_CAT_KERNEL_FATAL;
        case NP_EXC_GPF:
        case NP_EXC_SEGMENT_VIOL:
        case NP_EXC_SECURITY_VIOL: return EXC_CAT_SECURITY;
        case NP_EXC_SOFTWARE_RAISED: return EXC_CAT_SOFTWARE;
        default: return EXC_CAT_CPU;
    }
}

void exc_build_record(exception_record_t *out_record, const uint32_t *frame_ptr, uint32_t vector) {
    out_record->struct_size = EXC_REC_SIZE;
    out_record->version = 1;

    uint32_t code = exc_vector_to_code(vector);
    out_record->code = code;
    out_record->category = exc_code_to_category(code);

    /* CS liegt bei frame_ptr[15] (Byte-Offset 60 / 4). RPL-Bits: 0=Ring0. */
    uint32_t cs = frame_ptr[15];
    out_record->flags = (cs & 0x3u) ? EXC_FLAG_ORIGIN_USER : EXC_FLAG_ORIGIN_KERNEL;

    /* TODO: per_cpu_current_thread/thread_table aus sync32.inc nicht
       portiert - Thread-/Prozess-ID bleiben 0 (siehe exception.h). */
    out_record->thread_id = 0;
    out_record->process_id = 0;
    out_record->cpu_id = 0; /* BSP */

    out_record->ip = frame_ptr[14]; /* EIP, Byte-Offset 56 / 4 */

    out_record->fault_addr = 0;
    if (vector == 14) {
        uint32_t cr2;
        __asm__ volatile ("mov %%cr2, %0" : "=r"(cr2));
        out_record->fault_addr = cr2;
    }

    out_record->arch_err = frame_ptr[13]; /* Fehlercode, Byte-Offset 52 / 4 */
    out_record->arch_err_hi = 0;

    out_record->param_count = 1;
    out_record->param0 = vector;
}

uint32_t exc_search_fixup(uint32_t fault_eip, uint32_t vector) {
    for (uint32_t i = 0; exception_fixup_table[i].fault_start != 0xFFFFFFFFu; ++i) {
        if (fault_eip < exception_fixup_table[i].fault_start) continue;
        if (fault_eip >= exception_fixup_table[i].fault_end) continue;

        if (vector <= 31) {
            uint32_t mask = 1u << vector;
            if ((exception_fixup_table[i].allowed_vecs & mask) == 0) continue;
        }
        return exception_fixup_table[i].recovery;
    }
    return 0;
}

int vmm_handle_page_fault(uint32_t error_code, uint32_t fault_addr) {
    (void)error_code;
    /* Fruehe Phase: keine Demand-Paging-Unterstuetzung. Kernel-Adressen
       (>=0xC0000000) und alle Userspace-Adressen (kein UserVM-Mechanismus)
       gelten als unbehandelbar. */
    (void)fault_addr;
    return 1;
}

int exception_manager_self_test(void) {
    if (exception_mgr_initialized != 1) return -1;

    if (exc_vector_to_code(0) != NP_EXC_DIVIDE_BY_ZERO) return -1;
    if (exc_vector_to_code(14) != NP_EXC_PAGE_FAULT) return -1;
    if (exc_vector_to_code(8) != NP_EXC_DOUBLE_FAULT) return -1;

    if (exc_code_to_category(NP_EXC_PAGE_FAULT) != EXC_CAT_MEMORY) return -1;
    if (exc_code_to_category(NP_EXC_DIVIDE_BY_ZERO) != EXC_CAT_ARITHMETIC) return -1;
    if (exc_code_to_category(NP_EXC_DOUBLE_FAULT) != EXC_CAT_KERNEL_FATAL) return -1;
    if (exc_code_to_category(NP_EXC_DEBUG) != EXC_CAT_DEBUG) return -1;

    if (exc_search_fixup(0xDEAD0001u, 0) != 0) return -1;

    if (exception_stats_total == 0xFFFFFFFFu) return -1;
    if (exception_depth_table[0] != 0) return -1;

    /* Synthetischer Frame: 17 DWORDs (68 Bytes), CS bei Index 15, EIP bei 14. */
    uint32_t frame[17] = {0};
    frame[15] = 0x23; /* CS = Ring-3 (User) */
    frame[14] = 0x1234;
    frame[13] = 0;
    exception_record_t rec;
    exc_build_record(&rec, frame, 14); /* Vektor = Page Fault */
    if (rec.struct_size != EXC_REC_SIZE) return -1;

    frame[15] = 0; /* CS = Ring-0 */
    frame[14] = 0x5678;
    exc_build_record(&rec, frame, 0); /* Vektor 0 = #DE */
    if (rec.category != EXC_CAT_ARITHMETIC) return -1;

    if (vmm_handle_page_fault(0, 0xC0001000u) == 0) return -1; /* Kernel-Adresse: nicht behandelt */

    serial_puts("NOVA: Exception Manager Selftest OK (14/14)\r\n");
    return 0;
}
