/* NovaOS C-Kernel - PCI-Konfigurationsraum (Port aus storage32.inc, Mechanism #1). */
#ifndef NOVA_PCI_H
#define NOVA_PCI_H

#include <stdint.h>

/* address = Bus<<16 | Device<<11 | Function<<8 | Register-Offset. */
uint32_t pci_config_read32(uint32_t address);
void pci_config_write32(uint32_t address, uint32_t value);

#endif
