/* NovaOS C-Kernel - Text-Subsystem (Port aus text32.inc, NPSPEC-TEXT-*). */
#ifndef NOVA_TEXT_H
#define NOVA_TEXT_H

#include <stdint.h>

#define TEXT_ENC_UNKNOWN   0u
#define TEXT_ENC_ASCII     1u
#define TEXT_ENC_UTF8      2u
#define TEXT_ENC_UTF16LE   3u
#define TEXT_ENC_UTF16BE   4u
#define TEXT_ENC_UTF32LE   5u
#define TEXT_ENC_ISO8859_1 6u
#define TEXT_ENC_CP1252    7u

#define BIDI_LTR  0u
#define BIDI_RTL  1u
#define BIDI_AUTO 2u

#define NORM_NONE 0u
#define NORM_NFC  1u
#define NORM_NFD  2u
#define NORM_NFKC 3u
#define NORM_NFKD 4u

#define SCRIPT_UNKNOWN    0u
#define SCRIPT_LATIN      1u
#define SCRIPT_ARABIC     2u
#define SCRIPT_HEBREW     3u
#define SCRIPT_CYRILLIC   4u
#define SCRIPT_CJK        5u
#define SCRIPT_DEVANAGARI 6u
#define SCRIPT_GREEK      7u
#define SCRIPT_HANGUL     8u

#define SEG_CHAR     0u
#define SEG_WORD     1u
#define SEG_SENTENCE 2u
#define SEG_LINE     3u

#define SHAPE_LIGATURES   0x01u
#define SHAPE_KERNING     0x02u
#define SHAPE_DIACRITICS  0x04u
#define SHAPE_GLYPH_SUBST 0x08u

#define TEXTSEC_NO_BIDI_OVERRIDE 0x01u
#define TEXTSEC_NO_HOMOGRAPH     0x02u
#define TEXTSEC_NO_INVISIBLE     0x04u
#define TEXTSEC_NFC_NORMALIZE    0x08u

#define TEXT_ENC_CAPACITY 8u

/* Registriert ASCII/UTF-8/UTF-16LE als Standard-Encodings. */
void text_initialize(void);
int text_self_test(void); /* 0 = ok */

/* 0 = Erfolg, -1 wenn Tabelle voll. */
int text_register_encoding(uint32_t enc_id, uint32_t bidi_default);

/* 0 = Erfolg (*out_codepoint, *out_consumed gesetzt), -1 = ungueltig/Ende. */
int text_utf8_decode(const uint8_t *src, uint32_t src_len, uint32_t *out_codepoint,
                      uint32_t *out_consumed);

/* 0 = Erfolg (*out_written gesetzt), -1 = kein Platz im Puffer. */
int text_utf8_encode(uint32_t codepoint, uint8_t *dst, uint32_t remaining,
                      uint32_t *out_written);

/* Erkennt das Unicode-Script eines Codepoints (vereinfachte Bereichspruefung). */
uint32_t text_detect_script(uint32_t codepoint);

/* Zaehlt RTL- vs. LTR-Zeichen im UTF-8-String; Mehrheit entscheidet. */
uint32_t text_bidi_classify(const uint8_t *utf8_str, uint32_t len);

/* 0 = unauffaellig, -1 = verdaechtig (Bidi-Override/unsichtbare Zeichen je
   nach text_security_flags, standardmaessig NO_BIDI_OVERRIDE+NFC_NORMALIZE). */
int text_security_check(const uint8_t *utf8_str, uint32_t len);

#endif
