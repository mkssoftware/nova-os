/* NovaOS C-Kernel - Driver Discovery und Driver Lifecycle (Port aus
   driver32.inc, NPSPEC-DRIVER-DISCOVERY-0001 / NPSPEC-DRIVER-LIFECYCLE-0001 /
   NPSPEC-DRIVER-DMA-0001). */
#ifndef NOVA_DRIVER_H
#define NOVA_DRIVER_H

#include <stdint.h>

#define DISC_SRC_PCI     0u
#define DISC_SRC_ACPI    1u
#define DISC_SRC_USB     2u
#define DISC_SRC_VIRTUAL 3u

#define DRV_LC_DISCOVERED 0u
#define DRV_LC_MATCHED    1u
#define DRV_LC_LOADED     2u
#define DRV_LC_BOUND      3u
#define DRV_LC_RUNNING    4u
#define DRV_LC_SUSPENDED  5u
#define DRV_LC_STOPPING   6u
#define DRV_LC_UNBOUND    7u
#define DRV_LC_FAILED     8u

#define PCI_CLASS_NVME 0x010802u
#define PCI_CLASS_XHCI 0x0C0330u
#define PCI_CLASS_EHCI 0x0C0320u
#define PCI_CLASS_OHCI 0x0C0310u

#define DISC_CAPACITY 16u

#define DISC_FLAG_MATCHED 0x00000001u
#define DISC_FLAG_MMIO    0x00000002u
#define DISC_FLAG_MSI     0x00000004u

void driver_discovery_initialize(void);
int driver_discovery_self_test(void); /* 0 = ok */

/* 0 = Erfolg (*out_record_index zeigt auf den Slot), -1 wenn Tabelle voll. */
int disc_register_device(uint32_t source, uint32_t pci_bus_addr, uint32_t vendor_id,
                          uint32_t device_id, uint32_t class24, uint32_t *out_record_index);

/* Durchsucht den PCI-Bus nach NVMe- und xHCI-Controllern (Mechanism #1 via
   I/O-Ports 0xCF8/0xCFC). Fehlende Geraete sind kein Fehler (immer 0). */
int driver_pci_scan(void);

/* Setzt Memory Space Enable + Bus Master Enable + INTx-Disable. */
void driver_enable_bus_master(uint32_t pci_bus_addr);

uint32_t disc_count(void);

/* Durchsucht die Discovery-Tabelle nach dem index-ten (0-basiert) Eintrag
   mit passender class24 UND DISC_FLAG_MATCHED. 0 = gefunden (out_bus_addr/
   out_bar0 gesetzt), -1 wenn nicht vorhanden. */
int disc_find_matched(uint32_t class24, uint32_t index, uint32_t *out_bus_addr, uint32_t *out_bar0);

#endif
