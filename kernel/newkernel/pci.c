/* NovaOS C-Kernel - PCI-Konfigurationsraum (Port aus storage32.inc). */
#include "pci.h"

#define PCI_CONFIG_ADDRESS 0x0CF8u
#define PCI_CONFIG_DATA    0x0CFCu

static inline void outl(uint16_t port, uint32_t value) {
    __asm__ volatile ("outl %0, %1" : : "a"(value), "Nd"(port));
}

static inline uint32_t inl(uint16_t port) {
    uint32_t value;
    __asm__ volatile ("inl %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

uint32_t pci_config_read32(uint32_t address) {
    outl(PCI_CONFIG_ADDRESS, (address & 0x00FFFFFCu) | 0x80000000u);
    return inl(PCI_CONFIG_DATA);
}

void pci_config_write32(uint32_t address, uint32_t value) {
    outl(PCI_CONFIG_ADDRESS, (address & 0x00FFFFFCu) | 0x80000000u);
    outl(PCI_CONFIG_DATA, value);
}
