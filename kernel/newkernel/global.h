/* NovaOS C-Kernel - Globalization/Internationalization (Port aus global32.inc,
   NPSPEC-GLOBALIZATION-*). */
#ifndef NOVA_GLOBAL_H
#define NOVA_GLOBAL_H

#include <stdint.h>

#define LANG_NONE  0x00000000u
#define LANG_DE_DE 0x65440044u
#define LANG_DE_AT 0x65440054u
#define LANG_DE_CH 0x65440048u
#define LANG_EN_US 0x6E450053u
#define LANG_EN_GB 0x6E450042u
#define LANG_FR_FR 0x72460046u
#define LANG_JA_JP 0x614A0050u
#define LANG_ZH_CN 0x685A004Eu

#define GLOB_DIR_LTR  0u
#define GLOB_DIR_RTL  1u
#define GLOB_DIR_TTB  2u
#define GLOB_DIR_BIDI 3u

#define CAL_GREGORIAN 0u
#define CAL_ISLAMIC   1u
#define CAL_HEBREW    2u
#define CAL_JAPANESE  3u
#define CAL_CHINESE   4u

#define NUMFMT_DECIMAL_COMMA 0x01u
#define NUMFMT_GROUP_DOT     0x02u
#define NUMFMT_GROUP_SPACE   0x04u
#define NUMFMT_CURRENCY_PRE  0x08u

#define PLURAL_ZERO  0u
#define PLURAL_ONE   1u
#define PLURAL_TWO   2u
#define PLURAL_FEW   3u
#define PLURAL_MANY  4u
#define PLURAL_OTHER 5u

#define LOCALE_CAPACITY    4u
#define LANG_PRIO_CAPACITY 8u

/* Setzt Default-Locale auf de-DE. */
void globalization_initialize(void);
int globalization_self_test(void); /* 0 = ok */

/* 0 = Erfolg, -1 bei lang_tag==0. Setzt bei RTL-Sprachen (grober Bereich
   0x41720000..0x41FFFFFF) automatisch die Schreibrichtung auf RTL. */
int glob_set_language(uint32_t lang_tag);

/* Germanische Pluralregel: n==1 -> PLURAL_ONE, sonst PLURAL_OTHER. */
uint32_t glob_get_plural_category(uint32_t n);

/* 0 = unterstuetzt, -1 = nicht in den geladenen Locales vorhanden. */
int glob_is_supported(uint32_t lang_tag);

/* Gibt den ersten unterstuetzten Tag aus candidates zurueck, oder LANG_NONE. */
uint32_t glob_negotiate_language(const uint32_t *candidates, uint32_t count);

#endif
