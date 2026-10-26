/* NovaOS C-Kernel - Font-Subsystem (Port aus font32.inc,
   NPSPEC-FONT-MANAGER-0001 und verwandte NPSPEC-FONT-Spezifikationen). */
#ifndef NOVA_FONT_H
#define NOVA_FONT_H

#include <stdint.h>

#define FONT_FLAG_MONO     0x01u
#define FONT_FLAG_BITMAP   0x02u
#define FONT_FLAG_VARIABLE 0x04u
#define FONT_FLAG_COLOR    0x08u
#define FONT_FLAG_FALLBACK 0x10u
#define FONT_FLAG_SYSTEM   0x20u
#define FONT_FLAG_VERIFIED 0x40u

#define FONT_PROV_UNKNOWN   0u
#define FONT_PROV_EMBEDDED  1u
#define FONT_PROV_LOADED    2u
#define FONT_PROV_GENERATED 3u

#define FONT_AXIS_WEIGHT  0u
#define FONT_AXIS_WIDTH   1u
#define FONT_AXIS_SLANT   2u
#define FONT_AXIS_OPTICAL 3u
#define FONT_AXIS_COUNT   4u

#define FONT_CAPACITY 8u

#define FONT_SYS_GLYPH_W     8u
#define FONT_SYS_GLYPH_H     16u
#define FONT_SYS_FIRST_CHAR  0x20u
#define FONT_SYS_LAST_CHAR   0x7Eu
#define FONT_SYS_CHAR_COUNT  95u
#define FONT_SYS_BYTES_PER   16u

/* Registriert den eingebetteten 8x16 System-Font (ASCII 0x20..0x7E). */
void font_manager_initialize(void);
int font_manager_self_test(void); /* 0 = ok */

/* 0 = Erfolg, -1 wenn Tabelle voll. */
int font_register(uint32_t id, uint32_t flags, uint32_t size_px);

/* 0 = Erfolg (out_glyph zeigt auf 16 Bytes Bitmap-Daten), -1 wenn ausserhalb
   des Bereichs (out_glyph zeigt dann auf das Leerzeichen-Fallback-Glyph). */
int font_get_glyph(uint32_t codepoint, uint32_t font_id, const uint8_t **out_glyph);

/* 0 = Erfolg, -1 wenn font_id unbekannt. */
int font_set_fallback(uint32_t font_id, uint32_t fallback_id);

/* 0 = Erfolg, -1 wenn axis_id ungueltig oder font_id unbekannt. */
int font_set_axis(uint32_t font_id, uint32_t axis_id, uint8_t value);

#endif
