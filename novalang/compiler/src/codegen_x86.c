#include "codegen_x86.h"
#include <stdlib.h>
#include <string.h>

/* Siehe specifications/NOVALANG_SPEC_v0.1.md Abschnitt 4: diese Werte
   sind NICHT erfunden, sondern aus syscall.c/syscall.h und
   tools/nef/nef_main.c cmd_build()/cmd_link() uebernommen. */
#define NC_SYSCALL_SERVICE_CORE  1u
#define NC_SYSCALL_ABI_VERSION   1u
#define NC_SYSCALL_CORE_YIELD    13u
#define NC_SYSCALL_CORE_DEBUG_WRITE 17u

typedef struct NcBuf {
    uint8_t *data;
    uint32_t len;
    uint32_t cap;
} NcBuf;

static void buf_init(NcBuf *b) { b->data = NULL; b->len = 0; b->cap = 0; }

static void buf_reserve(NcBuf *b, uint32_t extra) {
    if (b->len + extra <= b->cap) return;
    uint32_t new_cap = b->cap ? b->cap * 2 : 64;
    while (new_cap < b->len + extra) new_cap *= 2;
    b->data = (uint8_t *)realloc(b->data, new_cap);
    b->cap = new_cap;
}

static void buf_push_u8(NcBuf *b, uint8_t v) {
    buf_reserve(b, 1);
    b->data[b->len++] = v;
}

static void buf_push_bytes(NcBuf *b, const void *src, uint32_t n) {
    buf_reserve(b, n);
    memcpy(b->data + b->len, src, n);
    b->len += n;
}

static void push_u32_le(NcBuf *b, uint32_t v) {
    uint8_t le[4];
    le[0] = (uint8_t)(v & 0xFFu);
    le[1] = (uint8_t)((v >> 8) & 0xFFu);
    le[2] = (uint8_t)((v >> 16) & 0xFFu);
    le[3] = (uint8_t)((v >> 24) & 0xFFu);
    buf_push_bytes(b, le, 4);
}

/* mov <reg>, imm32 - Opcode B8+reg (reg: eax=0,ecx=1,edx=2,ebx=3,esi=6) */
static void emit_mov_imm32(NcBuf *b, uint8_t reg, uint32_t imm) {
    buf_push_u8(b, (uint8_t)(0xB8u + reg));
    push_u32_le(b, imm);
}

#define REG_EAX 0u
#define REG_ECX 1u
#define REG_EDX 2u
#define REG_EBX 3u
#define REG_ESI 6u

static void emit_int80(NcBuf *b) {
    buf_push_u8(b, 0xCDu);
    buf_push_u8(b, 0x80u);
}

/* <opcode> eax, [addr] bzw. [addr], eax - direkte 32-Bit-Speicher-
   Adressierung ohne Basisregister: ModRM = mod=00,reg=000(eax),rm=101
   = 0x05, gefolgt von disp32 = addr. Deckt mov/add/sub/cmp mit EAX und
   einem absoluten Adressoperanden ab (die einzige Speicherzugriffsform,
   die dieser Codegenerator braucht). */
static void emit_eax_memop32(NcBuf *b, uint8_t opcode, uint32_t addr) {
    buf_push_u8(b, opcode);
    buf_push_u8(b, 0x05u);
    push_u32_le(b, addr);
}

/* mov <reg>, [addr] fuer ein BELIEBIGES Zielregister (nicht nur EAX wie
   emit_eax_memop32) - ModRM = mod=00, reg=reg_field, rm=101 (disp32,
   kein Basisregister). Gebraucht, um SysCall(...)-Argumente direkt in
   ihr Zielregister zu laden, ohne einen Zwischenschritt ueber EAX. */
static void emit_mov_reg_from_mem32(NcBuf *b, uint8_t reg_field, uint32_t addr) {
    buf_push_u8(b, 0x8Bu);
    buf_push_u8(b, (uint8_t)((reg_field << 3) | 0x05u));
    push_u32_le(b, addr);
}
#define OP_MOV_EAX_FROM_MEM 0x8Bu /* mov eax, [addr] */
#define OP_MOV_MEM_FROM_EAX 0x89u /* mov [addr], eax */
#define OP_ADD_EAX_MEM      0x03u /* add eax, [addr] */
#define OP_SUB_EAX_MEM      0x2Bu /* sub eax, [addr] */
#define OP_CMP_EAX_MEM      0x3Bu /* cmp eax, [addr] */

static void emit_add_eax_imm32(NcBuf *b, uint32_t imm) { buf_push_u8(b, 0x05u); push_u32_le(b, imm); }
static void emit_sub_eax_imm32(NcBuf *b, uint32_t imm) { buf_push_u8(b, 0x2Du); push_u32_le(b, imm); }
static void emit_cmp_eax_imm32(NcBuf *b, uint32_t imm) { buf_push_u8(b, 0x3Du); push_u32_le(b, imm); }

/* jmp/jcc rel8 mit Backpatching: emit_jump_placeholder() schreibt den
   Opcode + ein Platzhalter-Byte und liefert dessen Offset; patch_jump()
   berechnet NACH dem Erzeugen des Sprungziels das tatsaechliche
   Relativ-Offset und schreibt es ein. 0 = Erfolg, -1 = Ziel zu weit fuer
   ein 8-Bit-Offset (ehrlicher Codegen-Fehler statt stillschweigend
   falscher Code - v0.3-If-Koerper sind klein genug, dass das in der
   Praxis nicht stoert; eine spaetere Version koennte auf rel32 wechseln). */
static uint32_t emit_jump_placeholder(NcBuf *b, uint8_t opcode) {
    buf_push_u8(b, opcode);
    uint32_t patch_at = b->len;
    buf_push_u8(b, 0u);
    return patch_at;
}
static int patch_jump(NcBuf *b, uint32_t patch_at) {
    int32_t rel = (int32_t)b->len - (int32_t)(patch_at + 1u);
    if (rel < -128 || rel > 127) return -1;
    b->data[patch_at] = (uint8_t)(int8_t)rel;
    return 0;
}

#define JCC_JE  0x74u
#define JCC_JNE 0x75u
#define JCC_JL  0x7Cu
#define JCC_JGE 0x7Du
#define JCC_JLE 0x7Eu
#define JCC_JG  0x7Fu
#define JMP_SHORT 0xEBu

/* Invertierter Sprung: "wenn die Bedingung NICHT zutrifft, ueberspringe
   den Then-Koerper" - siehe emit_if() unten. */
static uint8_t inverse_jcc(NcCmpOp op) {
    switch (op) {
        case CMP_EQ: return JCC_JNE;
        case CMP_NE: return JCC_JE;
        case CMP_LT: return JCC_JGE;
        case CMP_GT: return JCC_JLE;
        case CMP_LE: return JCC_JG;
        default: return JCC_JL; /* CMP_GE */
    }
}

static uint32_t int_var_addr(uint32_t var_index) {
    return NC_DATA_VADDR + var_index * 4u;
}

/* Wertet expr nach EAX aus - echte Laufzeit-Addition/Subtraktion, kein
   Constant Folding: jeder Term erzeugt tatsaechlich ausgefuehrten Code.
   is_peek (v0.5) liest von einer compile-time-bekannten Pufferadresse
   statt einer Variablenadresse - fuer EAX-Operationen identisch zu
   is_var (beide sind nur "lies/addiere/subtrahiere von [addr]"), daher
   hier einheitlich behandelt. */
static void emit_int_expr(NcBuf *code, NcIntTerm *expr) {
    if (expr->is_peek) emit_eax_memop32(code, OP_MOV_EAX_FROM_MEM, expr->peek_addr);
    else if (!expr->is_var) emit_mov_imm32(code, REG_EAX, (uint32_t)expr->literal);
    else emit_eax_memop32(code, OP_MOV_EAX_FROM_MEM, int_var_addr(expr->var_index));

    for (NcIntTerm *t = expr->next; t; t = t->next) {
        uint32_t addr = t->is_peek ? t->peek_addr : int_var_addr(t->var_index);
        if (t->op == '+') {
            if (t->is_peek || t->is_var) emit_eax_memop32(code, OP_ADD_EAX_MEM, addr);
            else emit_add_eax_imm32(code, (uint32_t)t->literal);
        } else {
            if (t->is_peek || t->is_var) emit_eax_memop32(code, OP_SUB_EAX_MEM, addr);
            else emit_sub_eax_imm32(code, (uint32_t)t->literal);
        }
    }
}

static int emit_stmt_list(NcBuf *code, NcBuf *rodata, NcStmt *stmts);

static int emit_if(NcBuf *code, NcBuf *rodata, NcStmt *st) {
    emit_eax_memop32(code, OP_MOV_EAX_FROM_MEM, int_var_addr(st->if_lhs_var_index));
    if (st->if_rhs_is_var) emit_eax_memop32(code, OP_CMP_EAX_MEM, int_var_addr(st->if_rhs_var_index));
    else emit_cmp_eax_imm32(code, (uint32_t)st->if_rhs_literal);

    uint32_t skip_then_patch = emit_jump_placeholder(code, inverse_jcc(st->if_cmp_op));
    if (emit_stmt_list(code, rodata, st->if_then_body) != 0) return -1;

    if (st->if_else_body) {
        uint32_t skip_else_patch = emit_jump_placeholder(code, JMP_SHORT);
        if (patch_jump(code, skip_then_patch) != 0) return -1; /* Then-Ende -> hierher bei FALSE */
        if (emit_stmt_list(code, rodata, st->if_else_body) != 0) return -1;
        if (patch_jump(code, skip_else_patch) != 0) return -1;
    } else {
        if (patch_jump(code, skip_then_patch) != 0) return -1;
    }
    return 0;
}

static int emit_stmt_list(NcBuf *code, NcBuf *rodata, NcStmt *stmts) {
    for (NcStmt *st = stmts; st; st = st->next) {
        if (st->kind == STMT_WRITELINE) {
            uint32_t str_total_len = st->string_len + 2u; /* + "\r\n" */
            if (rodata->len + str_total_len > NC_RODATA_MAX_SIZE) return -1; /* siehe codegen_x86.h */
            uint32_t str_vaddr = NC_RODATA_VADDR + rodata->len;
            buf_push_bytes(rodata, st->string_arg, st->string_len);
            buf_push_u8(rodata, '\r');
            buf_push_u8(rodata, '\n');

            emit_mov_imm32(code, REG_EDX, str_vaddr);
            emit_mov_imm32(code, REG_ESI, str_total_len);
            emit_mov_imm32(code, REG_EAX, NC_SYSCALL_SERVICE_CORE);
            emit_mov_imm32(code, REG_EBX, NC_SYSCALL_CORE_DEBUG_WRITE);
            emit_mov_imm32(code, REG_ECX, NC_SYSCALL_ABI_VERSION);
            emit_int80(code);
        } else if (st->kind == STMT_ASSIGN_INT) {
            emit_int_expr(code, st->assign_expr);
            emit_eax_memop32(code, OP_MOV_MEM_FROM_EAX, int_var_addr(st->assign_var_index));
        } else if (st->kind == STMT_ASSIGN_SYSCALL) {
            /* service/operation/arg1/arg2 -> EAX/EBX/ECX/EDX/ESI, int
               0x80, Ergebnis in EAX - siehe syscall.c syscall_dispatch().
               Jedes Argument wird DIREKT in sein Zielregister geladen
               (emit_mov_reg_from_mem32/emit_mov_imm32 nehmen beide ein
               beliebiges Zielregister) - kein Umweg ueber EAX, also
               keine Reihenfolge-/Ueberschreibungsgefahr zwischen den
               vier Argumenten. ECX (ABI-Version) ist IMMER fest
               NC_SYSCALL_ABI_VERSION, kein NovaLang-Operand. */
            NcSysArg *a = st->syscall_args; /* [0]=service [1]=operation [2]=arg1 [3]=arg2 */
            static const uint8_t arg_reg[4] = { REG_EAX, REG_EBX, REG_EDX, REG_ESI };
            for (int i = 0; i < 4; ++i) {
                if (a[i].is_peek) emit_mov_reg_from_mem32(code, arg_reg[i], a[i].peek_addr);
                else if (a[i].is_var) emit_mov_reg_from_mem32(code, arg_reg[i], int_var_addr(a[i].var_index));
                else emit_mov_imm32(code, arg_reg[i], (uint32_t)a[i].literal);
            }
            emit_mov_imm32(code, REG_ECX, NC_SYSCALL_ABI_VERSION);
            emit_int80(code);
            emit_eax_memop32(code, OP_MOV_MEM_FROM_EAX, int_var_addr(st->assign_var_index));
        } else if (st->kind == STMT_POKE) {
            /* PokeInt(buffer, byteOffset, value) - schreibt 4 Byte an
               eine compile-time-bekannte absolute Adresse (poke_addr,
               siehe parser.c). */
            if (st->poke_value.is_peek) emit_eax_memop32(code, OP_MOV_EAX_FROM_MEM, st->poke_value.peek_addr);
            else if (st->poke_value.is_var) emit_eax_memop32(code, OP_MOV_EAX_FROM_MEM, int_var_addr(st->poke_value.var_index));
            else emit_mov_imm32(code, REG_EAX, (uint32_t)st->poke_value.literal);
            emit_eax_memop32(code, OP_MOV_MEM_FROM_EAX, st->poke_addr);
        } else if (st->kind == STMT_IF) {
            if (emit_if(code, rodata, st) != 0) return -1;
        } else { /* STMT_RETURN */
            /* Der Rueckgabewert selbst (st->int_arg) wird bewusst NICHT
               kodiert - siehe NOVALANG_SPEC_v0.1.md Abschnitt 3 (kein
               Prozess-Exitcode-Mechanismus in NovaOS, SYSCALL_CORE_EXIT
               ist ein Einweg-Mechanismus fuer genau einen bestimmten
               Ring-3-Demoprozess). Return geht wie "Hello Nova" in eine
               dauerhafte SYSCALL_CORE_YIELD-Schleife. */
            uint32_t loop_start = code->len;
            emit_mov_imm32(code, REG_EAX, NC_SYSCALL_SERVICE_CORE);
            emit_mov_imm32(code, REG_EBX, NC_SYSCALL_CORE_YIELD);
            emit_mov_imm32(code, REG_ECX, NC_SYSCALL_ABI_VERSION);
            emit_int80(code);
            buf_push_u8(code, JMP_SHORT);
            int32_t rel = (int32_t)loop_start - (int32_t)(code->len + 1u);
            buf_push_u8(code, (uint8_t)(int8_t)rel); /* 19 Byte Schleife - passt immer in rel8 */
        }
    }
    return 0;
}

int nc_codegen_x86(const NcModule *mod,
                    uint8_t **out_code_buf, uint32_t *out_code_len,
                    uint8_t **out_rodata_buf, uint32_t *out_rodata_len,
                    uint8_t **out_data_buf, uint32_t *out_data_len) {
    *out_code_buf = NULL; *out_code_len = 0;
    *out_rodata_buf = NULL; *out_rodata_len = 0;
    *out_data_buf = NULL; *out_data_len = 0;

    const NcFunction *fn = mod->main_func;
    if (!fn) return -1;

    NcBuf code, rodata, data;
    buf_init(&code);
    buf_init(&rodata);
    buf_init(&data);

    for (uint32_t i = 0; i < mod->int_var_count; ++i) {
        push_u32_le(&data, (uint32_t)mod->int_var_initial[i]);
    }

    if (emit_stmt_list(&code, &rodata, fn->body) != 0) {
        free(code.data); free(rodata.data); free(data.data);
        return -1;
    }

    /* Puffer-Unterregion (v0.5, siehe codegen_x86.h): liegt FEST bei
       NC_BUFFER_VADDR = NC_RODATA_VADDR + NC_RODATA_MAX_SIZE, unabhaengig
       von der tatsaechlichen String-Laenge - die RODATA-Section wird
       deshalb bis dorthin mit Nullen aufgefuellt, dann folgen die
       zero-initialisierten Pufferbytes selbst. Nur wenn tatsaechlich
       Puffer deklariert wurden (sonst bleibt die Section so klein wie
       bisher). */
    if (mod->buf_count > 0u) {
        uint32_t buf_total = 0u;
        for (uint32_t i = 0; i < mod->buf_count; ++i) buf_total += mod->buf_size[i];
        while (rodata.len < NC_RODATA_MAX_SIZE) buf_push_u8(&rodata, 0u);
        for (uint32_t i = 0; i < buf_total; ++i) buf_push_u8(&rodata, 0u);
    }

    *out_code_buf = code.data; *out_code_len = code.len;
    *out_rodata_buf = rodata.data; *out_rodata_len = rodata.len;
    *out_data_buf = data.data; *out_data_len = data.len;
    return 0;
}
