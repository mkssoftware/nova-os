/* NovaOS C-Kernel - Privacy-Subsystem (Port aus privacy32.inc, NPSPEC-PRIVACY-*). */
#ifndef NOVA_PRIVACY_H
#define NOVA_PRIVACY_H

#include <stdint.h>

#define PRIV2_LABEL_PUBLIC    0u
#define PRIV2_LABEL_INTERNAL  1u
#define PRIV2_LABEL_SENSITIVE 2u
#define PRIV2_LABEL_PERSONAL  3u
#define PRIV2_LABEL_SECRET    4u

#define PRIV_RETAIN_SESSION   0u
#define PRIV_RETAIN_DAY       1u
#define PRIV_RETAIN_WEEK      2u
#define PRIV_RETAIN_MONTH     3u
#define PRIV_RETAIN_PERMANENT 4u

#define PRIV_SOV_LOCAL_ONLY 0x01u
#define PRIV_SOV_NO_EXPORT  0x02u
#define PRIV_SOV_ENCRYPTED  0x04u
#define PRIV_SOV_AUDITED    0x08u

#define PRIV_DLP_ALLOW  0u
#define PRIV_DLP_WARN   1u
#define PRIV_DLP_BLOCK  2u
#define PRIV_DLP_REDACT 3u

#define PRIV_CAPACITY 16u

void privacy_ext_initialize(void);
int privacy_ext_self_test(void); /* 0 = ok */

/* 0 = Erfolg, -1 wenn Tabelle voll. */
int privacy_ext_label_data(uint32_t data_id, uint32_t label, uint32_t retention,
                            uint32_t sovereignty_flags);

/* *out_dlp_action erhaelt PRIV_DLP_*. Unlabeled Daten: PRIV_DLP_ALLOW.
   Abgelaufene Daten werden automatisch als PRIV_DLP_BLOCK gemeldet. */
void privacy_ext_check_access(uint32_t data_id, uint32_t *out_dlp_action);

#endif
