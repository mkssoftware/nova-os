/* NovaOS C-Kernel - Nova Fault Classification (Port aus adaptive32.inc,
   NPSPEC-RESILIENCE-CLASSIFICATION-0001). Klassifiziert erkannte Fehler
   nach Schweregrad und Quell-Domaene, damit die Recovery-Policy die
   richtige Aktion waehlen kann (siehe rpol.h). class_id ist 1-basiert
   (1..FCLASS_CAPACITY), 0 ist ungueltig. */
#ifndef NOVA_FCLASS_H
#define NOVA_FCLASS_H

#include <stdint.h>

#define FCLASS_CAPACITY 8u

#define FCLASS_UNKNOWN   0u
#define FCLASS_TRANSIENT 1u
#define FCLASS_SOFT      2u
#define FCLASS_HARD      3u
#define FCLASS_CRITICAL  4u

#define FCLASS_SRC_KERNEL   1u
#define FCLASS_SRC_SCHED    2u
#define FCLASS_SRC_MEMORY   3u
#define FCLASS_SRC_SECURITY 4u
#define FCLASS_SRC_DRIVER   5u
#define FCLASS_SRC_USER     6u

void fclass_initialize(void);
int fclass_self_test(void); /* 0 = ok */

/* 0 = Erfolg, -1 wenn class_id ausserhalb 1..FCLASS_CAPACITY, severity >
   FCLASS_CRITICAL oder source == 0. */
int fclass_register(uint32_t class_id, uint32_t severity, uint32_t source);

/* 0 = Erfolg (out_severity/out_source gesetzt, Zaehler erhoeht), -1 wenn
   class_id ungueltig oder nicht registriert. */
int fclass_classify(uint32_t class_id, uint32_t *out_severity, uint32_t *out_source);

#endif
