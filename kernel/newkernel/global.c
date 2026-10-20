/* NovaOS C-Kernel - Globalization/Internationalization (Port aus global32.inc). */
#include "global.h"

typedef struct {
    uint32_t lang;
    uint32_t region;
    uint32_t direction;
    uint32_t calendar;
    uint32_t numfmt_flags;
    uint32_t date_order; /* 0=DMY, 1=MDY, 2=YMD */
    uint32_t time_24h;
    uint32_t decimal_sep;
    uint32_t group_sep;
    uint32_t currency_code;
    uint32_t reserved;
} glob_locale_t;

static glob_locale_t glob_locales[LOCALE_CAPACITY];
static uint32_t glob_prio_list[LANG_PRIO_CAPACITY];
static uint32_t glob_locale_count = 0;
static uint32_t glob_prio_count = 0;
static uint32_t glob_current_lang = LANG_DE_DE;
static uint32_t glob_current_dir = GLOB_DIR_LTR;
static int glob_mgr_ready = 0;

static void glob_setup_default_locale(void) {
    glob_locale_t *loc = &glob_locales[0];
    loc->lang = LANG_DE_DE;
    loc->region = LANG_DE_DE;
    loc->direction = GLOB_DIR_LTR;
    loc->calendar = CAL_GREGORIAN;
    loc->numfmt_flags = NUMFMT_DECIMAL_COMMA | NUMFMT_GROUP_DOT;
    loc->date_order = 0; /* D.M.Y */
    loc->time_24h = 1;
    loc->decimal_sep = 0x2Cu; /* ',' */
    loc->group_sep = 0x2Eu;   /* '.' */
    loc->currency_code = 0x00525545u; /* "EUR" */
    ++glob_locale_count;
}

void globalization_initialize(void) {
    for (uint32_t i = 0; i < LOCALE_CAPACITY; ++i) {
        glob_locales[i] = (glob_locale_t){0};
    }
    glob_locale_count = 0;

    glob_setup_default_locale();

    glob_prio_list[0] = LANG_DE_DE;
    glob_prio_count = 1;
    glob_current_lang = LANG_DE_DE;
    glob_current_dir = GLOB_DIR_LTR;

    glob_mgr_ready = 1;
}

int glob_set_language(uint32_t lang_tag) {
    if (lang_tag == 0) {
        return -1;
    }
    glob_current_lang = lang_tag;

    /* RTL-Sprachen (grober Bereich, z.B. Arabisch=0x41720000, Hebraeisch=0x65480000). */
    if (lang_tag >= 0x41720000u && lang_tag < 0x42000000u) {
        glob_current_dir = GLOB_DIR_RTL;
    } else {
        glob_current_dir = GLOB_DIR_LTR;
    }
    return 0;
}

uint32_t glob_get_plural_category(uint32_t n) {
    return (n == 1) ? PLURAL_ONE : PLURAL_OTHER;
}

int glob_is_supported(uint32_t lang_tag) {
    for (uint32_t i = 0; i < glob_locale_count; ++i) {
        if (glob_locales[i].lang == lang_tag) {
            return 0;
        }
    }
    return -1;
}

uint32_t glob_negotiate_language(const uint32_t *candidates, uint32_t count) {
    for (uint32_t i = 0; i < count; ++i) {
        if (glob_is_supported(candidates[i]) == 0) {
            return candidates[i];
        }
    }
    return LANG_NONE;
}

int globalization_self_test(void) {
    if (glob_mgr_ready != 1) return -1;

    if (glob_get_plural_category(1) != PLURAL_ONE) return -1;
    if (glob_get_plural_category(5) != PLURAL_OTHER) return -1;

    if (glob_set_language(0x41720000u) != 0) return -1;
    if (glob_current_dir != GLOB_DIR_RTL) return -1;

    glob_set_language(LANG_DE_DE); /* zuruecksetzen */

    return 0;
}
