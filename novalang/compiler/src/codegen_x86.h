#ifndef NOVAC_CODEGEN_X86_H
#define NOVAC_CODEGEN_X86_H

#include "ast.h"
#include <stdint.h>

/* Erzeugt rohe x86-32-Maschinencode-Bytes fuer mod, AUFGETEILT in DREI
   Sections (siehe specifications/NOVALANG_SPEC_v0.1.md Abschnitt 4 fuer
   die urspruengliche CODE/RODATA-Begruendung, v0.3.md fuer DATA und
   v0.5.md fuer die Puffer-Unterregion):
   - CODE   (R|X) bei NC_CODE_VADDR   - Instruktionen, uebliche NEF-/
     ushell-Konvention.
   - RODATA (R)   bei NC_RODATA_VADDR - String-Literale UND (v0.5) die
     Syscall-Argument-Puffer ("Dim buf As Buffer(N)") - beide MUESSEN im
     validierten Syscall-Pointer-Fenster [USER_ADDRESS_MIN,_MAX) liegen
     (syscall.h) - NICHT aus Bequemlichkeit verschieben (siehe
     feedback_syscall_pointer_window.md: ein Pointer AUSSERHALB dieses
     Fensters wird von syscall_validate_user_range() ohne Fehlermeldung
     abgelehnt, das Programm bleibt einfach stumm). String-Literale
     duerfen hoechstens NC_RODATA_MAX_SIZE Byte belegen, DAHINTER
     beginnt bei NC_BUFFER_VADDR die feste Pufferregion (siehe unten) -
     beide zusammen muessen in die 16-KiB-Fenstergroesse passen.
   - DATA   (R|W) bei NC_DATA_VADDR   - echte Laufzeit-Integer-Variablen
     (v0.3). Braucht KEINE Pointer-Validierung (wird nie an einen
     Syscall uebergeben, nur vom eigenen Code per direkter
     Speicheradressierung gelesen/geschrieben) - Adresse daher frei
     waehlbar, solange sie nicht mit CODE/RODATA/Stack ueberlappt.
   out_*_buf werden malloc't (vom Aufrufer freizugeben). Gibt 0 bei
   Erfolg zurueck, -1 bei einem Codegen-Fehler (z.B. ein If-Koerper zu
   gross fuer einen kurzen Sprung, RODATA-Limit ueberschritten, oder
   Puffer-Region voll - siehe codegen_x86.c). */
#define NC_CODE_VADDR   0x00500000u
#define NC_RODATA_VADDR 0x00400000u /* = USER_ADDRESS_MIN, syscall.h */
#define NC_RODATA_MAX_SIZE   0x3000u /* 12 KiB fuer String-Literale */
#define NC_BUFFER_VADDR      (NC_RODATA_VADDR + NC_RODATA_MAX_SIZE) /* 0x403000, noch im Fenster */
#define NC_BUFFER_REGION_SIZE 0x1000u /* 4 KiB fuer alle "Dim ... As Buffer(N)" zusammen */
#define NC_DATA_VADDR   0x00480000u /* frei gewaehlt, siehe Kommentar oben */

int nc_codegen_x86(const NcModule *mod,
                    uint8_t **out_code_buf, uint32_t *out_code_len,
                    uint8_t **out_rodata_buf, uint32_t *out_rodata_len,
                    uint8_t **out_data_buf, uint32_t *out_data_len);

#endif
