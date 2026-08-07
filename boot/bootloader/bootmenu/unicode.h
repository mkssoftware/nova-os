#ifndef NOVA_BOOT_UNICODE_H
#define NOVA_BOOT_UNICODE_H

#include <stdint.h>
#include <stdbool.h>

#define NOVA_UNICODE_REPLACEMENT 0xfffdu

typedef struct {
    uint32_t decoded, invalid, replacements, fallback_hits, cache_hits, cache_misses;
} nova_unicode_diagnostics_t;

void nova_unicode_initialize(void);
bool nova_unicode_next(const char **utf8, uint32_t *codepoint);
bool nova_unicode_is_valid(uint32_t codepoint);
uint8_t nova_unicode_latin_glyph_code(uint32_t codepoint, bool *fallback);
const nova_unicode_diagnostics_t *nova_unicode_diagnostics(void);

#endif
