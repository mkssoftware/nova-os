#ifndef NOVA_TYPES_H
#define NOVA_TYPES_H

/*
 * Nova OS – Basistypen
 *
 * Diese Datei definiert feste Datentypen für die gesamte Nova-Plattform.
 * Öffentliche APIs sollen keine unsicheren Typen wie int, long oder char
 * verwenden, wenn die Größe relevant ist.
 */

typedef unsigned char       nova_u8;
typedef unsigned short      nova_u16;
typedef unsigned int        nova_u32;
typedef unsigned long long  nova_u64;

typedef signed char         nova_i8;
typedef signed short        nova_i16;
typedef signed int          nova_i32;
typedef signed long long    nova_i64;

typedef nova_u64            nova_size;
typedef nova_u64            nova_address;
typedef nova_u64            nova_flags;

typedef enum
{
    NOVA_FALSE = 0,
    NOVA_TRUE  = 1
} nova_bool;

#define NOVA_NULL ((void*)0)

#endif