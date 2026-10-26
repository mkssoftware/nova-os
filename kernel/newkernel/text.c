/* NovaOS C-Kernel - Text-Subsystem (Port aus text32.inc). */
#include "text.h"

typedef struct {
    uint32_t id;
    uint32_t bidi_default;
    uint32_t flags;
} text_enc_desc_t;

static text_enc_desc_t text_enc_table[TEXT_ENC_CAPACITY];
static uint32_t text_enc_count = 0;
static uint32_t text_security_flags = 0;
static int text_subsystem_ready = 0;

int text_register_encoding(uint32_t enc_id, uint32_t bidi_default) {
    if (text_enc_count >= TEXT_ENC_CAPACITY) {
        return -1;
    }
    text_enc_desc_t *desc = &text_enc_table[text_enc_count];
    desc->id = enc_id;
    desc->bidi_default = bidi_default;
    desc->flags = 0;
    ++text_enc_count;
    return 0;
}

void text_initialize(void) {
    text_enc_count = 0;
    for (uint32_t i = 0; i < TEXT_ENC_CAPACITY; ++i) {
        text_enc_table[i] = (text_enc_desc_t){0};
    }

    text_register_encoding(TEXT_ENC_ASCII, BIDI_LTR);
    text_register_encoding(TEXT_ENC_UTF8, BIDI_AUTO);
    text_register_encoding(TEXT_ENC_UTF16LE, BIDI_AUTO);

    text_security_flags = TEXTSEC_NO_BIDI_OVERRIDE | TEXTSEC_NFC_NORMALIZE;
    text_subsystem_ready = 1;
}

int text_utf8_decode(const uint8_t *src, uint32_t src_len, uint32_t *out_codepoint,
                      uint32_t *out_consumed) {
    uint32_t pos = 0;
    if (src_len == 0) {
        return -1;
    }
    uint8_t lead = src[pos++];

    if (lead < 0x80u) {
        *out_codepoint = lead;
        *out_consumed = pos;
        return 0;
    }
    if (lead < 0xC0u) {
        return -1; /* ungueltiges Lead-Byte */
    }

    uint32_t codepoint;
    uint32_t extra_bytes;
    if (lead < 0xE0u) {
        codepoint = lead & 0x1Fu;
        extra_bytes = 1;
    } else if (lead < 0xF0u) {
        codepoint = lead & 0x0Fu;
        extra_bytes = 2;
    } else if (lead < 0xF8u) {
        codepoint = lead & 0x07u;
        extra_bytes = 3;
    } else {
        return -1;
    }

    for (uint32_t i = 0; i < extra_bytes; ++i) {
        if (pos >= src_len) {
            return -1;
        }
        uint8_t cont = src[pos++];
        codepoint = (codepoint << 6) | (cont & 0x3Fu);
    }

    *out_codepoint = codepoint;
    *out_consumed = pos;
    return 0;
}

int text_utf8_encode(uint32_t codepoint, uint8_t *dst, uint32_t remaining,
                      uint32_t *out_written) {
    if (codepoint <= 0x7Fu) {
        if (remaining < 1) return -1;
        dst[0] = (uint8_t)codepoint;
        *out_written = 1;
        return 0;
    }
    if (codepoint <= 0x7FFu) {
        if (remaining < 2) return -1;
        dst[0] = (uint8_t)(0xC0u | (codepoint >> 6));
        dst[1] = (uint8_t)(0x80u | (codepoint & 0x3Fu));
        *out_written = 2;
        return 0;
    }
    if (codepoint <= 0xFFFFu) {
        if (remaining < 3) return -1;
        dst[0] = (uint8_t)(0xE0u | (codepoint >> 12));
        dst[1] = (uint8_t)(0x80u | ((codepoint >> 6) & 0x3Fu));
        dst[2] = (uint8_t)(0x80u | (codepoint & 0x3Fu));
        *out_written = 3;
        return 0;
    }
    if (remaining < 4) return -1;
    dst[0] = (uint8_t)(0xF0u | (codepoint >> 18));
    dst[1] = (uint8_t)(0x80u | ((codepoint >> 12) & 0x3Fu));
    dst[2] = (uint8_t)(0x80u | ((codepoint >> 6) & 0x3Fu));
    dst[3] = (uint8_t)(0x80u | (codepoint & 0x3Fu));
    *out_written = 4;
    return 0;
}

uint32_t text_detect_script(uint32_t codepoint) {
    if (codepoint >= 0x0041u && codepoint <= 0x024Fu) return SCRIPT_LATIN;
    if (codepoint >= 0x0370u && codepoint <= 0x03FFu) return SCRIPT_GREEK;
    if (codepoint >= 0x0400u && codepoint <= 0x04FFu) return SCRIPT_CYRILLIC;
    if (codepoint >= 0x0590u && codepoint <= 0x05FFu) return SCRIPT_HEBREW;
    if (codepoint >= 0x0600u && codepoint <= 0x06FFu) return SCRIPT_ARABIC;
    if (codepoint >= 0x0900u && codepoint <= 0x097Fu) return SCRIPT_DEVANAGARI;
    if (codepoint >= 0x4E00u && codepoint <= 0x9FFFu) return SCRIPT_CJK;
    return SCRIPT_UNKNOWN;
}

uint32_t text_bidi_classify(const uint8_t *utf8_str, uint32_t len) {
    uint32_t ltr_count = 0;
    uint32_t rtl_count = 0;
    uint32_t pos = 0;

    while (pos < len) {
        uint32_t codepoint, consumed;
        if (text_utf8_decode(utf8_str + pos, len - pos, &codepoint, &consumed) != 0) {
            break;
        }
        pos += consumed;

        uint32_t script = text_detect_script(codepoint);
        if (script == SCRIPT_ARABIC || script == SCRIPT_HEBREW) {
            ++rtl_count;
        } else {
            ++ltr_count;
        }
    }

    return (rtl_count > ltr_count) ? BIDI_RTL : BIDI_LTR;
}

int text_security_check(const uint8_t *utf8_str, uint32_t len) {
    uint32_t pos = 0;

    while (pos < len) {
        uint32_t codepoint, consumed;
        if (text_utf8_decode(utf8_str + pos, len - pos, &codepoint, &consumed) != 0) {
            break;
        }
        pos += consumed;

        if (text_security_flags & TEXTSEC_NO_BIDI_OVERRIDE) {
            /* Bidi-Override-Zeichen: U+202A..U+202E, U+2066..U+2069. */
            if ((codepoint >= 0x202Au && codepoint <= 0x202Eu) ||
                (codepoint >= 0x2066u && codepoint <= 0x2069u)) {
                return -1;
            }
        }
        if (text_security_flags & TEXTSEC_NO_INVISIBLE) {
            /* Unsichtbare Zeichen: U+200B..U+200F (Zero-Width), U+FEFF (BOM). */
            if ((codepoint >= 0x200Bu && codepoint <= 0x200Fu) || codepoint == 0xFEFFu) {
                return -1;
            }
        }
    }

    return 0;
}

int text_self_test(void) {
    int errors = 0;

    if (text_subsystem_ready != 1) ++errors;

    uint8_t buf[16];
    uint32_t written = 0;
    if (text_utf8_encode(0x41u, buf, sizeof(buf), &written) != 0) ++errors;

    uint32_t codepoint = 0, consumed = 0;
    if (text_utf8_decode(buf, 1, &codepoint, &consumed) != 0) {
        ++errors;
    } else if (codepoint != 0x41u) {
        ++errors;
    }

    if (text_detect_script(0x41u) != SCRIPT_LATIN) ++errors;
    if (text_detect_script(0x0630u) != SCRIPT_ARABIC) ++errors;

    return (errors == 0) ? 0 : -1;
}
