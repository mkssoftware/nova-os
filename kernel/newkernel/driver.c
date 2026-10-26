/* NovaOS C-Kernel - Driver Discovery und Driver Lifecycle (Port aus driver32.inc). */
#include "driver.h"
#include "pci.h"

typedef struct {
    uint32_t source;
    uint32_t pci_bus_addr;
    uint32_t vendor_id;
    uint32_t device_id;
    uint32_t class24;
    uint32_t state;
    uint32_t flags;
    uint32_t handle;
    uint32_t bar0;
    uint32_t irq;
    uint32_t reserved;
} disc_record_t;

static disc_record_t disc_records[DISC_CAPACITY];
static uint32_t disc_count_value = 0;
static int disc_mgr_ready = 0;

void driver_discovery_initialize(void) {
    for (uint32_t i = 0; i < DISC_CAPACITY; ++i) {
        disc_records[i] = (disc_record_t){0};
    }
    disc_count_value = 0;
    disc_mgr_ready = 1;
}

uint32_t disc_count(void) {
    return disc_count_value;
}

int disc_register_device(uint32_t source, uint32_t pci_bus_addr, uint32_t vendor_id,
                          uint32_t device_id, uint32_t class24, uint32_t *out_record_index) {
    if (disc_count_value >= DISC_CAPACITY) {
        return -1;
    }
    uint32_t slot;
    for (slot = 0; slot < DISC_CAPACITY; ++slot) {
        disc_record_t *rec = &disc_records[slot];
        if (rec->state == DRV_LC_UNBOUND || rec->vendor_id == 0) {
            break;
        }
    }
    if (slot >= DISC_CAPACITY) {
        return -1;
    }

    disc_record_t *rec = &disc_records[slot];
    rec->source = source;
    rec->pci_bus_addr = pci_bus_addr;
    rec->vendor_id = vendor_id;
    rec->device_id = device_id;
    rec->class24 = class24;
    rec->state = DRV_LC_DISCOVERED;
    rec->flags = 0;
    rec->handle = 0;
    rec->bar0 = 0;
    rec->irq = 0;
    ++disc_count_value;

    if (out_record_index) {
        *out_record_index = slot;
    }
    return 0;
}

int driver_pci_scan(void) {
    for (uint32_t bus_addr = 0; bus_addr < 0x01000000u; ) {
        uint32_t vendor_device = pci_config_read32(bus_addr);
        int absent = ((vendor_device & 0xFFFFu) == 0xFFFFu);

        if (!absent) {
            uint32_t class24 = pci_config_read32(bus_addr | 0x08u) >> 8;
            uint32_t vendor_id = vendor_device & 0xFFFFu;
            uint32_t device_id = vendor_device >> 16;

            if (class24 == PCI_CLASS_NVME || class24 == PCI_CLASS_XHCI) {
                uint32_t slot = 0;
                if (disc_register_device(DISC_SRC_PCI, bus_addr, vendor_id, device_id,
                                          class24, &slot) == 0) {
                    disc_record_t *rec = &disc_records[slot];
                    uint32_t bar0 = pci_config_read32(rec->pci_bus_addr | 0x10u) & 0xFFFFF000u;
                    rec->bar0 = bar0;
                    rec->flags |= DISC_FLAG_MMIO;
                    rec->state = DRV_LC_MATCHED;
                    rec->flags |= DISC_FLAG_MATCHED;
                }
            }
        }

        /* Multifunction-/Bounds-Logik identisch zum alten ASM. */
        uint32_t function = (bus_addr >> 8) & 0x7u;
        if (function != 0) {
            bus_addr += 0x100u; /* naechste Funktion */
            continue;
        }

        if (!absent) {
            uint32_t header = pci_config_read32(bus_addr | 0x0Cu);
            if (header & 0x00800000u) {
                bus_addr += 0x100u; /* multifunction: naechste Funktion pruefen */
                continue;
            }
        }

        /* Einzel-Funktion oder abwesend: naechstes Geraet. */
        bus_addr = (bus_addr & 0xFFFFF800u) + 0x800u;
    }

    return 0;
}

void driver_enable_bus_master(uint32_t pci_bus_addr) {
    uint32_t reg = pci_bus_addr | 0x04u;
    uint32_t value = pci_config_read32(reg);
    value |= 0x00000406u; /* MemSpace + BusMaster + INTx-Disable */
    pci_config_write32(reg, value);
}

int driver_discovery_self_test(void) {
    if (disc_mgr_ready != 1) {
        return -1;
    }

    uint32_t saved_count = disc_count_value;
    uint32_t slot = 0;
    if (disc_register_device(DISC_SRC_VIRTUAL, 0, 0, 0, 0, &slot) != 0) {
        return -1;
    }
    if (disc_count_value != saved_count + 1) {
        return -1;
    }

    /* Aufraeumen: nur den Testeintrag zuruecksetzen. */
    disc_records[slot] = (disc_record_t){0};
    disc_count_value = saved_count;

    return 0;
}
