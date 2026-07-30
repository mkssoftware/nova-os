#ifndef NOVA_PANIC_H
#define NOVA_PANIC_H
#include <stdint.h>
#define NOVA_PANIC_ABI_MAJOR 1u
#define NOVA_PANIC_ABI_MINOR 0u
typedef struct nova_panic_report {
    uint32_t StructSize;
    uint32_t AbiVersion;
    uint32_t ErrorCode;
    uint32_t Subsystem;
    uint32_t InstructionPointer;
    uint32_t StackPointer;
    uint32_t FaultAddress;
    uint32_t TimerTicks;
} nova_panic_report_t;
typedef struct nova_panic_api {
    uint32_t StructSize;
    uint16_t AbiMajor;
    uint16_t AbiMinor;
    uint32_t ReportSize;
    uint32_t PanicEntry;
    uint32_t LastReportAddress;
    uint32_t SelfTestEntry;
    uint32_t Reserved[2];
} nova_panic_api_t;
_Static_assert(sizeof(nova_panic_report_t) == 32,
               "nova_panic_report_t ABI size changed");
_Static_assert(sizeof(nova_panic_api_t) == 32,
               "nova_panic_api_t ABI size changed");
#endif
