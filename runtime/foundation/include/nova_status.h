#ifndef NOVA_STATUS_H
#define NOVA_STATUS_H

/*
 * Nova OS – Status- und Fehlercodes
 *
 * Sichtbare Fehlermeldungen werden später auf Deutsch ausgegeben.
 * Diese Konstanten bleiben bewusst englisch, damit sie im Quellcode kurz,
 * eindeutig und international verständlich sind.
 */

typedef enum
{
    NOVA_OK = 0,

    NOVA_ERROR = 1,
    NOVA_INVALID_ARGUMENT = 2,
    NOVA_NOT_FOUND = 3,
    NOVA_NO_MEMORY = 4,
    NOVA_ACCESS_DENIED = 5,
    NOVA_TIMEOUT = 6,
    NOVA_NOT_SUPPORTED = 7,
    NOVA_ALREADY_EXISTS = 8,
    NOVA_BUSY = 9,
    NOVA_CORRUPTED = 10,
    NOVA_SECURITY_VIOLATION = 11,
    NOVA_INVALID_STATE = 12,
    NOVA_OVERFLOW = 13,
    NOVA_UNDERFLOW = 14,
    NOVA_ALIGNMENT_ERROR = 15,
    NOVA_VERSION_MISMATCH = 16

} nova_status;

#endif