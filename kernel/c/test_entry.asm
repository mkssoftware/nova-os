; Minimaler Test-Entry fuer die C-Toolchain-Verifikation (win32/COFF-Format).
bits 32

extern c_hello

global _start
_start:
    call c_hello
.hang:
    jmp .hang
