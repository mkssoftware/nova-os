#ifndef NOVA_COMPILER_H
#define NOVA_COMPILER_H

/*
 * Nova OS – Compiler-Abstraktion
 *
 * Compiler-spezifische Attribute dürfen nicht überall im Code verteilt werden.
 * Alle Attribute werden zentral über diese Datei bereitgestellt.
 */

#if defined(__GNUC__) || defined(__clang__)

#define NOVA_PACKED        __attribute__((packed))
#define NOVA_ALIGN(x)      __attribute__((aligned(x)))
#define NOVA_NORETURN      __attribute__((noreturn))
#define NOVA_UNUSED        __attribute__((unused))
#define NOVA_INLINE        static inline
#define NOVA_WEAK          __attribute__((weak))

#else

#define NOVA_PACKED
#define NOVA_ALIGN(x)
#define NOVA_NORETURN
#define NOVA_UNUSED
#define NOVA_INLINE        static inline
#define NOVA_WEAK

#endif

#define NOVA_ARRAY_COUNT(array) (sizeof(array) / sizeof((array)[0]))

#endif