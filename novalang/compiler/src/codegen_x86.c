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

/* add/sub eax, <reg> (Register-Register, mod=11) - NUR fuer EAX als
   Ziel gebraucht (Scratch-Register-Operand bei PeekByte-Verkettung,
   siehe emit_int_expr()). ModRM = 0xC0 | reg_field (reg=eax=000 im
   reg-Feld ist implizit durch den Opcode selbst festgelegt). */
static void emit_add_eax_reg(NcBuf *b, uint8_t reg_field) { buf_push_u8(b, 0x03u); buf_push_u8(b, (uint8_t)(0xC0u | reg_field)); }
static void emit_sub_eax_reg(NcBuf *b, uint8_t reg_field) { buf_push_u8(b, 0x2Bu); buf_push_u8(b, (uint8_t)(0xC0u | reg_field)); }

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

/* movzx <reg>, byte [addr] (v0.7, PeekByte) - Opcode 0F B6 /r, ModRM
   wie emit_mov_reg_from_mem32 (mod=00,reg=reg_field,rm=101,disp32).
   ANDERS als ein blosses "mov al,[addr]": movzx setzt die oberen 24
   Bit des Zielregisters explizit auf 0 - ein reiner 8-Bit-Load wuerde
   sie undefiniert lassen, was fuer einen Ausdruckswert (z.B. in einem
   Vergleich) unsicher waere. */
static void emit_movzx_reg_byte_mem(NcBuf *b, uint8_t reg_field, uint32_t addr) {
    buf_push_u8(b, 0x0Fu);
    buf_push_u8(b, 0xB6u);
    buf_push_u8(b, (uint8_t)((reg_field << 3) | 0x05u));
    push_u32_le(b, addr);
}

/* mov [addr], al (v0.7, PokeByte) - Opcode 0x88 /r, gleiches ModRM-
   Schema. Schreibt NUR das niedrigste Byte von EAX - der Aufrufer muss
   den vollen Wert vorher nach EAX geladen haben (ueblicher Weg ueber
   emit_mov_imm32/emit_eax_memop32), nur das Low-Byte zaehlt. */
static void emit_mov_mem_from_al(NcBuf *b, uint32_t addr) {
    buf_push_u8(b, 0x88u);
    buf_push_u8(b, 0x05u);
    push_u32_le(b, addr);
}

/* "mov [reg+disp32], al" (opcode 0x88) bzw. "mov [reg+disp32], eax"
   (opcode 0x89) - Basisregister+disp32-Adressierung (v0.12, siehe
   NOVALANG_SPEC_v0.12.md PokeInt/PokeByte mit Laufzeit-Offset-Variable).
   ModRM = mod=10 (disp32 folgt), reg=000 (Quelle AL/EAX ist durch den
   Opcode selbst festgelegt, nicht durch das reg-Feld), rm=reg_field
   (Basisregister). Reine Schreiboperation - der Aufrufer laedt den zu
   schreibenden Wert VORHER nach EAX und die Basisadresse+Offset-Summe
   wird hier erst zur Laufzeit gebildet (disp32 = Puffer-BASISADRESSE,
   reg_field haelt den bereits geladenen Laufzeit-Offset). */
static void emit_mem_base_disp32_from_reg(NcBuf *b, uint8_t opcode, uint8_t base_reg_field, uint32_t disp32) {
    buf_push_u8(b, opcode);
    buf_push_u8(b, (uint8_t)(0x80u | base_reg_field));
    push_u32_le(b, disp32);
}

/* "mov <reg>, [baseReg+disp32]" (Opcode 0x8B) bzw. "movzx <reg>, byte
   [baseReg+disp32]" (Opcode 0F B6) - das LESENDE Gegenstueck zu
   emit_mem_base_disp32_from_reg() oben (v0.15, siehe
   NOVALANG_SPEC_v0.15.md PeekInt/PeekByte mit Laufzeit-Offset-Variable).
   ModRM = mod=10 (disp32 folgt), reg=dest_reg_field (BELIEBIGES
   Zielregister - anders als die Schreib-Variante, die nur AL/EAX
   kennt, da das Quellregister dort implizit durch den Opcode
   festgelegt ist), rm=base_reg_field (haelt den bereits geladenen
   Laufzeit-Offset, ueblicherweise ECX). disp32 = Puffer-BASISADRESSE. */
static void emit_reg_from_mem_base_disp32(NcBuf *b, int is_byte, uint8_t dest_reg_field,
                                           uint8_t base_reg_field, uint32_t disp32) {
    if (is_byte) {
        buf_push_u8(b, 0x0Fu);
        buf_push_u8(b, 0xB6u);
    } else {
        buf_push_u8(b, 0x8Bu);
    }
    buf_push_u8(b, (uint8_t)(0x80u | (dest_reg_field << 3) | base_reg_field));
    push_u32_le(b, disp32);
}

#define JCC_JE  0x74u
#define JCC_JNE 0x75u
#define JCC_JL  0x7Cu
#define JCC_JGE 0x7Du
#define JCC_JLE 0x7Eu
#define JCC_JG  0x7Fu
#define JMP_SHORT 0xEBu

/* jmp/jcc rel32 mit Backpatching (v0.12-Aufwertung von rel8, siehe
   NOVALANG_SPEC_v0.12.md - der v0.3-Kommentar hier sagte bereits "eine
   spaetere Version koennte auf rel32 wechseln": ein tief verschachtelter
   If-Koerper mit mehreren Function-Aufrufen (v0.12s ProcessOneVkey)
   ueberschritt real den 8-Bit-Bereich, boot-verifiziert als Codegen-
   Fehler gefunden). emit_jump_placeholder() schreibt den Opcode + ein
   4-Byte-Platzhalter und liefert dessen Offset; patch_jump() berechnet
   NACH dem Erzeugen des Sprungziels das tatsaechliche Relativ-Offset
   und schreibt es ein. rel32 deckt jede realistische Programmgroesse
   ab, daher kein Bereichsfehler mehr moeglich (anders als zuvor bei
   rel8). Kodierung: Jcc rel32 = "0F" (Jcc_short_opcode+0x10) rel32 (der
   Short-Opcode-Bereich 0x70-0x7F mappt linear auf den passenden
   0F-praefigierten Long-Opcode 0x80-0x8F); JMP rel32 = "E9" rel32
   (einzelnes Opcode-Byte, kein 0F-Praefix - JMP_SHORT=0xEB wird hier
   als Sentinel fuer "es ist ein unbedingter Sprung" wiederverwendet). */
static uint32_t emit_jump_placeholder(NcBuf *b, uint8_t opcode) {
    if (opcode == JMP_SHORT) {
        buf_push_u8(b, 0xE9u);
    } else {
        buf_push_u8(b, 0x0Fu);
        buf_push_u8(b, (uint8_t)(opcode + 0x10u));
    }
    uint32_t patch_at = b->len;
    push_u32_le(b, 0u);
    return patch_at;
}
static int patch_jump(NcBuf *b, uint32_t patch_at) {
    int32_t rel = (int32_t)b->len - (int32_t)(patch_at + 4u);
    uint32_t rel_u = (uint32_t)rel;
    b->data[patch_at + 0] = (uint8_t)(rel_u & 0xFFu);
    b->data[patch_at + 1] = (uint8_t)((rel_u >> 8) & 0xFFu);
    b->data[patch_at + 2] = (uint8_t)((rel_u >> 16) & 0xFFu);
    b->data[patch_at + 3] = (uint8_t)((rel_u >> 24) & 0xFFu);
    return 0;
}

/* jmp/jcc rel32 zu einem BEREITS BEKANNTEN (typischerweise rueckwaerts
   liegenden) Ziel - fuer Schleifen (siehe emit_while()/die STMT_RETURN-
   YIELD-Schleife unten), wo das Ziel schon vor dem Sprung erzeugt wurde
   und daher kein Backpatching braucht. Gleiche rel32-Kodierung wie
   emit_jump_placeholder()/patch_jump(). */
static int emit_jmp_to(NcBuf *b, uint8_t opcode, uint32_t target_offset) {
    if (opcode == JMP_SHORT) {
        buf_push_u8(b, 0xE9u);
    } else {
        buf_push_u8(b, 0x0Fu);
        buf_push_u8(b, (uint8_t)(opcode + 0x10u));
    }
    uint32_t next_instr_offset = b->len + 4u;
    int32_t rel = (int32_t)target_offset - (int32_t)next_instr_offset;
    push_u32_le(b, (uint32_t)rel);
    return 0;
}

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
    /* v0.15: ein Peek mit Laufzeit-Offset-Variable ist (siehe parser.c
       parse_int_expr()) NUR als dieser allererste Term moeglich - die
       Reihenfolge ECX-Offset-Laden -> EAX-Ergebnis-Laden ist daher
       immer sicher (keine andere EAX/ECX-Nutzung lief vorher). */
    if (expr->is_peek && expr->peek_offset_is_var) {
        emit_mov_reg_from_mem32(code, REG_ECX, int_var_addr(expr->peek_offset_var_index));
        emit_reg_from_mem_base_disp32(code, expr->peek_width == 1, REG_EAX, REG_ECX, expr->peek_addr);
    }
    else if (expr->is_peek && expr->peek_width == 1) emit_movzx_reg_byte_mem(code, REG_EAX, expr->peek_addr);
    else if (expr->is_peek) emit_eax_memop32(code, OP_MOV_EAX_FROM_MEM, expr->peek_addr);
    else if (!expr->is_var) emit_mov_imm32(code, REG_EAX, (uint32_t)expr->literal);
    else emit_eax_memop32(code, OP_MOV_EAX_FROM_MEM, int_var_addr(expr->var_index));

    for (NcIntTerm *t = expr->next; t; t = t->next) {
        if (t->is_peek && t->peek_width == 1) {
            /* Kein einzelner "add r32, zx byte [mem]"-Opcode auf x86 -
               erst per ECX-Scratch zero-extend laden, dann Register-
               Register addieren/subtrahieren. */
            emit_movzx_reg_byte_mem(code, REG_ECX, t->peek_addr);
            if (t->op == '+') emit_add_eax_reg(code, REG_ECX);
            else emit_sub_eax_reg(code, REG_ECX);
            continue;
        }
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

/* v0.9 (siehe NOVALANG_SPEC_v0.9.md): Aufruf-Kontext fuer mehrere
   Functions. func_offset[i]/func_offset_known[i] verfolgt den bereits
   bekannten Code-Offset jeder Function - Main (immer Index 0) ist per
   Konvention Offset 0 (siehe NEF-Link-entry_offset), jede weitere
   Function bekommt ihren Offset ERST, wenn ihre Emission tatsaechlich
   beginnt (siehe nc_codegen_x86()). Ein "call"-Vorwaertsverweis (die
   aufgerufene Function ist zum Zeitpunkt des "call" noch nicht emittiert
   - der haeufige Fall, da Main immer ALS ERSTES emittiert wird) wird als
   Patch vermerkt und NACH der Emission ALLER Functions aufgeloest (alle
   Offsets sind dann bekannt) - analog zum bestehenden If/While-rel8-
   Backpatching, nur mit rel32 (Functions koennen weiter als 127 Byte
   voneinander entfernt liegen). */
#define NC_MAX_CALL_PATCHES 64u
typedef struct NcCodegenCtx {
    const NcModule *mod;
    uint32_t func_offset[16];
    int      func_offset_known[16];
    uint32_t patch_at[NC_MAX_CALL_PATCHES];
    uint32_t patch_func[NC_MAX_CALL_PATCHES];
    uint32_t patch_count;
} NcCodegenCtx;

/* call rel32 (Opcode 0xE8 /rel32) - rel ist relativ zur Adresse DIREKT
   NACH der 5-Byte-Instruktion (x86-Konvention). Ist der Ziel-Offset
   schon bekannt, wird rel sofort berechnet; sonst wird ein Patch
   vermerkt (siehe NcCodegenCtx-Kommentar oben). 0 = Erfolg, -1 =
   Patch-Tabelle voll (ehrlicher Codegen-Fehler statt stillschweigend
   falscher Code). */
static int emit_call_rel32(NcBuf *code, NcCodegenCtx *ctx, uint32_t func_index) {
    buf_push_u8(code, 0xE8u);
    uint32_t patch_at = code->len;
    push_u32_le(code, 0u); /* Platzhalter */
    if (ctx->func_offset_known[func_index]) {
        int32_t rel = (int32_t)ctx->func_offset[func_index] - (int32_t)(patch_at + 4u);
        uint32_t rel_u = (uint32_t)rel;
        code->data[patch_at + 0] = (uint8_t)(rel_u & 0xFFu);
        code->data[patch_at + 1] = (uint8_t)((rel_u >> 8) & 0xFFu);
        code->data[patch_at + 2] = (uint8_t)((rel_u >> 16) & 0xFFu);
        code->data[patch_at + 3] = (uint8_t)((rel_u >> 24) & 0xFFu);
        return 0;
    }
    if (ctx->patch_count >= NC_MAX_CALL_PATCHES) return -1;
    ctx->patch_at[ctx->patch_count] = patch_at;
    ctx->patch_func[ctx->patch_count] = func_index;
    ctx->patch_count++;
    return 0;
}

static int emit_stmt_list(NcBuf *code, NcBuf *rodata, NcStmt *stmts, NcCodegenCtx *ctx);

/* Gemeinsame Bedingungsauswertung von If UND While (siehe ast.h-
   Kommentar zu den geteilten if_*-Feldern): mov eax,[lhs] / cmp eax,rhs.
   Liefert den Backpatch-Offset des INVERTIERTEN Sprungs ("wenn die
   Bedingung NICHT zutrifft, ueberspringe/verlasse") - der Aufrufer
   patcht ihn auf das passende Ziel (Then-Ende bzw. Schleifenende). */
static uint32_t emit_cond_inverse_jump(NcBuf *code, NcStmt *st) {
    emit_eax_memop32(code, OP_MOV_EAX_FROM_MEM, int_var_addr(st->if_lhs_var_index));
    if (st->if_rhs_is_var) emit_eax_memop32(code, OP_CMP_EAX_MEM, int_var_addr(st->if_rhs_var_index));
    else emit_cmp_eax_imm32(code, (uint32_t)st->if_rhs_literal);
    return emit_jump_placeholder(code, inverse_jcc(st->if_cmp_op));
}

static int emit_if(NcBuf *code, NcBuf *rodata, NcStmt *st, NcCodegenCtx *ctx) {
    uint32_t skip_then_patch = emit_cond_inverse_jump(code, st);
    if (emit_stmt_list(code, rodata, st->if_then_body, ctx) != 0) return -1;

    if (st->if_else_body) {
        uint32_t skip_else_patch = emit_jump_placeholder(code, JMP_SHORT);
        if (patch_jump(code, skip_then_patch) != 0) return -1; /* Then-Ende -> hierher bei FALSE */
        if (emit_stmt_list(code, rodata, st->if_else_body, ctx) != 0) return -1;
        if (patch_jump(code, skip_else_patch) != 0) return -1;
    } else {
        if (patch_jump(code, skip_then_patch) != 0) return -1;
    }
    return 0;
}

/* While lhs <op> rhs ... End While (v0.6, siehe NOVALANG_SPEC_v0.6.md):
   Bedingung pruefen, bei FALSE aus der Schleife springen (Backpatch,
   Ziel erst nach dem Koerper bekannt), Koerper erzeugen, unbedingt
   zurueck zur Bedingungspruefung springen (Ziel schon bekannt, siehe
   emit_jmp_to - KEIN Backpatching noetig), dann das Verlassen-Ziel
   patchen. */
static int emit_while(NcBuf *code, NcBuf *rodata, NcStmt *st, NcCodegenCtx *ctx) {
    uint32_t loop_start = code->len;
    uint32_t exit_patch = emit_cond_inverse_jump(code, st);
    if (emit_stmt_list(code, rodata, st->if_then_body, ctx) != 0) return -1;
    if (emit_jmp_to(code, JMP_SHORT, loop_start) != 0) return -1;
    if (patch_jump(code, exit_patch) != 0) return -1;
    return 0;
}

static int emit_stmt_list(NcBuf *code, NcBuf *rodata, NcStmt *stmts, NcCodegenCtx *ctx) {
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
                /* v0.15: arg_reg ist nie ECX (nur EAX/EBX/EDX/ESI), ECX
                   bleibt also immer frei fuer den Laufzeit-Offset. */
                if (a[i].is_peek && a[i].peek_offset_is_var) {
                    emit_mov_reg_from_mem32(code, REG_ECX, int_var_addr(a[i].peek_offset_var_index));
                    emit_reg_from_mem_base_disp32(code, a[i].peek_width == 1, arg_reg[i], REG_ECX, a[i].peek_addr);
                }
                else if (a[i].is_peek && a[i].peek_width == 1) emit_movzx_reg_byte_mem(code, arg_reg[i], a[i].peek_addr);
                else if (a[i].is_peek) emit_mov_reg_from_mem32(code, arg_reg[i], a[i].peek_addr);
                else if (a[i].is_var) emit_mov_reg_from_mem32(code, arg_reg[i], int_var_addr(a[i].var_index));
                else emit_mov_imm32(code, arg_reg[i], (uint32_t)a[i].literal);
            }
            emit_mov_imm32(code, REG_ECX, NC_SYSCALL_ABI_VERSION);
            emit_int80(code);
            emit_eax_memop32(code, OP_MOV_MEM_FROM_EAX, int_var_addr(st->assign_var_index));
        } else if (st->kind == STMT_POKE) {
            /* PokeInt(buffer, byteOffset, value) bzw. PokeByte(...) -
               schreibt 4 bzw. 1 Byte. Der Wert wird immer erst
               vollstaendig nach EAX geladen (auch bei PokeByte - nur
               das anschliessende "mov [...],al" beschraenkt sich dann
               auf das niedrigste Byte).
               poke_offset_is_var==0 (v0.5/v0.7, unveraendert):
               poke_addr ist bereits die fertige absolute Compile-Zeit-
               Adresse.
               poke_offset_is_var==1 (v0.12, siehe NOVALANG_SPEC_v0.12.md):
               poke_addr ist nur die Puffer-BASISADRESSE - der Laufzeit-
               Offset wird NACH dem Wert-Load (das nur EAX anfasst) in
               ECX geladen, dann per Basisregister+disp32-Adressierung
               geschrieben ("mov [ecx+poke_addr], al/eax"). */
            NcSysArg *v = &st->poke_value;
            /* v0.15: der Wert-Load laeuft IMMER zuerst komplett durch
               (auch wenn er selbst transient ECX braucht) - das
               anschliessende Laden des SCHREIB-Offsets nach ECX (siehe
               unten) folgt erst danach, kein Konflikt. */
            if (v->is_peek && v->peek_offset_is_var) {
                emit_mov_reg_from_mem32(code, REG_ECX, int_var_addr(v->peek_offset_var_index));
                emit_reg_from_mem_base_disp32(code, v->peek_width == 1, REG_EAX, REG_ECX, v->peek_addr);
            }
            else if (v->is_peek && v->peek_width == 1) emit_movzx_reg_byte_mem(code, REG_EAX, v->peek_addr);
            else if (v->is_peek) emit_eax_memop32(code, OP_MOV_EAX_FROM_MEM, v->peek_addr);
            else if (v->is_var) emit_eax_memop32(code, OP_MOV_EAX_FROM_MEM, int_var_addr(v->var_index));
            else emit_mov_imm32(code, REG_EAX, (uint32_t)v->literal);
            if (st->poke_offset_is_var) {
                emit_mov_reg_from_mem32(code, REG_ECX, int_var_addr(st->poke_offset_var_index));
                if (st->poke_width == 1) emit_mem_base_disp32_from_reg(code, 0x88u, REG_ECX, st->poke_addr);
                else emit_mem_base_disp32_from_reg(code, 0x89u, REG_ECX, st->poke_addr);
            } else {
                if (st->poke_width == 1) emit_mov_mem_from_al(code, st->poke_addr);
                else emit_eax_memop32(code, OP_MOV_MEM_FROM_EAX, st->poke_addr);
            }
        } else if (st->kind == STMT_WRITEBUFFER) {
            /* Console.WriteBuffer(buffer, laenge) - gibt `laenge` Byte
               aus `buffer` 1:1 auf die serielle Konsole aus (SYSCALL_
               CORE_DEBUG_WRITE), OHNE automatisches "\r\n" (anders als
               WriteLine) - die Laenge ist haeufig eine Laufzeit-Groesse
               (Variable), nicht aus einem Compile-Zeit-Literal bekannt. */
            emit_mov_imm32(code, REG_EDX, st->writebuf_addr);
            NcSysArg *len = &st->writebuf_len;
            if (len->is_peek && len->peek_offset_is_var) {
                emit_mov_reg_from_mem32(code, REG_ECX, int_var_addr(len->peek_offset_var_index));
                emit_reg_from_mem_base_disp32(code, len->peek_width == 1, REG_ESI, REG_ECX, len->peek_addr);
            }
            else if (len->is_peek && len->peek_width == 1) emit_movzx_reg_byte_mem(code, REG_ESI, len->peek_addr);
            else if (len->is_peek) emit_mov_reg_from_mem32(code, REG_ESI, len->peek_addr);
            else if (len->is_var) emit_mov_reg_from_mem32(code, REG_ESI, int_var_addr(len->var_index));
            else emit_mov_imm32(code, REG_ESI, (uint32_t)len->literal);
            emit_mov_imm32(code, REG_EAX, NC_SYSCALL_SERVICE_CORE);
            emit_mov_imm32(code, REG_EBX, NC_SYSCALL_CORE_DEBUG_WRITE);
            emit_mov_imm32(code, REG_ECX, NC_SYSCALL_ABI_VERSION);
            emit_int80(code);
        } else if (st->kind == STMT_IF) {
            if (emit_if(code, rodata, st, ctx) != 0) return -1;
        } else if (st->kind == STMT_WHILE) {
            if (emit_while(code, rodata, st, ctx) != 0) return -1;
        } else if (st->kind == STMT_ASSIGN_CALL) {
            /* "x = FuncName(arg1, ...)" (v0.9, siehe NOVALANG_SPEC_v0.9.md):
               jedes Argument wird VOR dem "call" in den Parameter-Slot
               der aufgerufenen Function geschrieben (siehe ast.h/
               parser.c - Parameter sind ganz normale Integer-Variablen
               mit gemangeltem Namen). KEINE echte Stackframe - ein
               rekursiver/wiedereintretender Aufruf wuerde sich seine
               eigenen Parameter-Slots ueberschreiben (dokumentierte
               Grenze dieser Version). Nach dem "call" steht der
               Rueckgabewert in EAX (siehe STMT_FUNC_RETURN unten). */
            const NcFunction *callee = ctx->mod->functions[st->call_func_index];
            for (uint32_t i = 0; i < st->call_arg_count && i < callee->param_count; ++i) {
                NcSysArg *a = &st->call_args[i];
                if (a->is_peek && a->peek_offset_is_var) {
                    emit_mov_reg_from_mem32(code, REG_ECX, int_var_addr(a->peek_offset_var_index));
                    emit_reg_from_mem_base_disp32(code, a->peek_width == 1, REG_EAX, REG_ECX, a->peek_addr);
                }
                else if (a->is_peek && a->peek_width == 1) emit_movzx_reg_byte_mem(code, REG_EAX, a->peek_addr);
                else if (a->is_peek) emit_eax_memop32(code, OP_MOV_EAX_FROM_MEM, a->peek_addr);
                else if (a->is_var) emit_eax_memop32(code, OP_MOV_EAX_FROM_MEM, int_var_addr(a->var_index));
                else emit_mov_imm32(code, REG_EAX, (uint32_t)a->literal);
                emit_eax_memop32(code, OP_MOV_MEM_FROM_EAX, int_var_addr(callee->param_var_index[i]));
            }
            if (emit_call_rel32(code, ctx, st->call_func_index) != 0) return -1;
            emit_eax_memop32(code, OP_MOV_MEM_FROM_EAX, int_var_addr(st->assign_var_index));
        } else if (st->kind == STMT_FUNC_RETURN) {
            /* Echter Funktionsrueckgabewert (v0.9) - ANDERS als Mains
               STMT_RETURN (siehe unten): beliebiger IntExpr nach EAX,
               dann ein echtes "ret" statt der YIELD-Endlosschleife. */
            emit_int_expr(code, st->return_expr);
            buf_push_u8(code, 0xC3u); /* ret */
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
            if (emit_jmp_to(code, JMP_SHORT, loop_start) != 0) return -1; /* rel32, siehe emit_jmp_to()-Kommentar */
        }
    }
    return 0;
}

int nc_codegen_x86(const NcModule *mod,
                    uint8_t **out_code_buf, uint32_t *out_code_len,
                    uint8_t **out_rodata_buf, uint32_t *out_rodata_len,
                    uint8_t **out_data_buf, uint32_t *out_data_len,
                    uint8_t **out_buffer_buf, uint32_t *out_buffer_len) {
    *out_code_buf = NULL; *out_code_len = 0;
    *out_rodata_buf = NULL; *out_rodata_len = 0;
    *out_data_buf = NULL; *out_data_len = 0;
    *out_buffer_buf = NULL; *out_buffer_len = 0;

    if (mod->function_count == 0) return -1;

    NcBuf code, rodata, data, bufregion;
    buf_init(&code);
    buf_init(&rodata);
    buf_init(&data);
    buf_init(&bufregion);

    for (uint32_t i = 0; i < mod->int_var_count; ++i) {
        push_u32_le(&data, (uint32_t)mod->int_var_initial[i]);
    }

    /* v0.9 (siehe NOVALANG_SPEC_v0.9.md): Main (immer functions[0]) wird
       IMMER ALS ERSTES emittiert - entry_offset=0 bei "nef link" bleibt
       dadurch trivial. Jede weitere Function bekommt ihren Code-Offset
       erst unmittelbar vor ihrer eigenen Emission; ein "call" auf eine
       noch nicht emittierte Function (der Normalfall fuer von Main
       aufgerufene Helfer-Functions) wird ueber NcCodegenCtx vorgemerkt
       und erst ganz am Ende aufgeloest (siehe emit_call_rel32()). */
    NcCodegenCtx ctx;
    memset(&ctx, 0, sizeof(ctx));
    ctx.mod = mod;
    ctx.func_offset[0] = 0u;
    ctx.func_offset_known[0] = 1;

    if (emit_stmt_list(&code, &rodata, mod->functions[0]->body, &ctx) != 0) {
        free(code.data); free(rodata.data); free(data.data); free(bufregion.data);
        return -1;
    }

    for (uint32_t i = 1; i < mod->function_count; ++i) {
        ctx.func_offset[i] = code.len;
        ctx.func_offset_known[i] = 1;
        if (emit_stmt_list(&code, &rodata, mod->functions[i]->body, &ctx) != 0) {
            free(code.data); free(rodata.data); free(data.data); free(bufregion.data);
            return -1;
        }
    }

    for (uint32_t i = 0; i < ctx.patch_count; ++i) {
        uint32_t patch_at = ctx.patch_at[i];
        uint32_t target_offset = ctx.func_offset[ctx.patch_func[i]];
        int32_t rel = (int32_t)target_offset - (int32_t)(patch_at + 4u);
        uint32_t rel_u = (uint32_t)rel;
        code.data[patch_at + 0] = (uint8_t)(rel_u & 0xFFu);
        code.data[patch_at + 1] = (uint8_t)((rel_u >> 8) & 0xFFu);
        code.data[patch_at + 2] = (uint8_t)((rel_u >> 16) & 0xFFu);
        code.data[patch_at + 3] = (uint8_t)((rel_u >> 24) & 0xFFu);
    }

    /* BUFFER-Section (v0.5, siehe codegen_x86.h) - EIGENE, SCHREIBBARE
       Section, getrennt von der read-only RODATA-Section (siehe dortigen
       Kommentar: ein frueherer Versuch legte Puffer faelschlich IN
       RODATA, was bei jedem Schreibzugriff - PokeInt() oder ein
       zurueckschreibender Syscall wie VFS_OP_LOOKUPs result_handle -
       einen echten Seitenfehler ausloeste, boot-verifiziert gefunden).
       Nur erzeugt, wenn tatsaechlich Puffer deklariert wurden. */
    if (mod->buf_count > 0u) {
        uint32_t buf_total = 0u;
        for (uint32_t i = 0; i < mod->buf_count; ++i) buf_total += mod->buf_size[i];
        for (uint32_t i = 0; i < buf_total; ++i) buf_push_u8(&bufregion, 0u);

        /* "PokeString(...)"-Patches (v0.5) anwenden - rein eine Daten-
           Initialisierung zur Compile-Zeit, siehe ast.h-Kommentar zu
           NcModule::patch_*. */
        for (uint32_t i = 0; i < mod->patch_count; ++i) {
            uint32_t rel = mod->patch_addr[i] - NC_BUFFER_VADDR;
            if ((uint64_t)rel + (uint64_t)mod->patch_len[i] > (uint64_t)bufregion.len) {
                free(code.data); free(rodata.data); free(data.data); free(bufregion.data);
                return -1; /* sollte durch die Parser-Pruefung nie vorkommen */
            }
            memcpy(bufregion.data + rel, mod->patch_bytes[i], mod->patch_len[i]);
        }
    }

    *out_code_buf = code.data; *out_code_len = code.len;
    *out_rodata_buf = rodata.data; *out_rodata_len = rodata.len;
    *out_data_buf = data.data; *out_data_len = data.len;
    *out_buffer_buf = bufregion.data; *out_buffer_len = bufregion.len;
    return 0;
}
