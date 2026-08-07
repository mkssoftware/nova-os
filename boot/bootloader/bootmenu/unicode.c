#include "unicode.h"

static nova_unicode_diagnostics_t diagnostics;
static uint32_t cached_codepoint;
static uint8_t cached_glyph;

void nova_unicode_initialize(void)
{
    diagnostics = (nova_unicode_diagnostics_t){0};
    cached_codepoint = 0xffffffffu;
    cached_glyph = 0;
}

bool nova_unicode_is_valid(uint32_t cp)
{
    return cp <= 0x10ffffu && !(cp >= 0xd800u && cp <= 0xdfffu) &&
           cp != 0xfffeu && cp != 0xffffu;
}

bool nova_unicode_next(const char **cursor, uint32_t *codepoint)
{
    if (!cursor || !*cursor || !codepoint || !**cursor) return false;
    const uint8_t *p = (const uint8_t *)*cursor;
    uint32_t cp = NOVA_UNICODE_REPLACEMENT, length = 1;
    bool valid = true;
    if (p[0] < 0x80u) cp = p[0];
    else if ((p[0] & 0xe0u) == 0xc0u && p[1] && (p[1] & 0xc0u) == 0x80u) {
        cp = ((p[0] & 0x1fu) << 6) | (p[1] & 0x3fu); length = 2;
        valid = cp >= 0x80u;
    } else if ((p[0] & 0xf0u) == 0xe0u && p[1] && p[2] &&
               (p[1] & 0xc0u) == 0x80u && (p[2] & 0xc0u) == 0x80u) {
        cp = ((p[0] & 0x0fu) << 12) | ((p[1] & 0x3fu) << 6) | (p[2] & 0x3fu);
        length = 3; valid = cp >= 0x800u && nova_unicode_is_valid(cp);
    } else if ((p[0] & 0xf8u) == 0xf0u && p[1] && p[2] && p[3] &&
               (p[1] & 0xc0u) == 0x80u && (p[2] & 0xc0u) == 0x80u &&
               (p[3] & 0xc0u) == 0x80u) {
        cp = ((p[0] & 7u) << 18) | ((p[1] & 0x3fu) << 12) |
             ((p[2] & 0x3fu) << 6) | (p[3] & 0x3fu);
        length = 4; valid = cp >= 0x10000u && nova_unicode_is_valid(cp);
    } else valid = false;
    if (!valid) { cp = NOVA_UNICODE_REPLACEMENT; length = 1; ++diagnostics.invalid; }
    if (cp == NOVA_UNICODE_REPLACEMENT) ++diagnostics.replacements;
    *cursor += length; *codepoint = cp; ++diagnostics.decoded;
    return valid;
}

uint8_t nova_unicode_latin_glyph_code(uint32_t cp, bool *fallback)
{
    if (cp == cached_codepoint) {
        ++diagnostics.cache_hits; if (fallback) *fallback = cached_glyph == 0; return cached_glyph;
    }
    ++diagnostics.cache_misses;
    uint8_t glyph = 0;
    if (cp < 0x80u) glyph = (uint8_t)cp;
    else switch (cp) {
        case 0x00e4: glyph = 0x84; break; case 0x00f6: glyph = 0x94; break;
        case 0x00fc: glyph = 0x81; break; case 0x00c4: glyph = 0x8e; break;
        case 0x00d6: glyph = 0x99; break; case 0x00dc: glyph = 0x9a; break;
        case 0x00df: glyph = 0xe1; break; default: break;
    }
    cached_codepoint = cp; cached_glyph = glyph;
    if (!glyph) ++diagnostics.fallback_hits;
    if (fallback) *fallback = glyph == 0;
    return glyph;
}

const nova_unicode_diagnostics_t *nova_unicode_diagnostics(void) { return &diagnostics; }
