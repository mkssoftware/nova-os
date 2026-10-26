/* NovaOS C-Kernel - USB-Stack: xHCI-Controller, USB-Core (Port aus
   usb32.inc). Siehe usb.h fuer den Hinweis zu den permanent gestubbten
   Control-Transfer-Funktionen (bereits im alten ASM so). */
#include "usb.h"
#include "driver.h"
#include "paging.h"
#include "pmm.h"
#include "block.h"
#include "interrupt.h"
#include "serial.h"

#define XHCI_CAPLENGTH  0x00u
#define XHCI_HCSPARAMS1 0x04u
#define XHCI_DBOFF      0x14u
#define XHCI_RTSOFF     0x18u

#define XHCI_USBCMD    0x00u
#define XHCI_USBSTS    0x04u
#define XHCI_CRCR_LO   0x18u
#define XHCI_DCBAAP_LO 0x30u
#define XHCI_CONFIG    0x38u

#define XHCI_CMD_RS    0x00000001u
#define XHCI_CMD_HCRST 0x00000002u

#define XHCI_STS_HCH 0x00000001u
#define XHCI_STS_CNR 0x00000800u

#define XHCI_PORTSC_OFF  0x400u
#define XHCI_PORTSC_SIZE 0x10u
#define XHCI_PS_CCS      0x00000001u
#define XHCI_PS_PED      0x00000002u
#define XHCI_PS_PR       0x00000010u
#define XHCI_PS_PP       0x00000200u
#define XHCI_PS_CSC      0x00020000u
#define XHCI_PS_PRC      0x00200000u
#define XHCI_PS_SPEED    0x00003C00u
#define XHCI_PS_SPEED_SH 10u

#define XHCI_TRB_LINK 6u
#define XHCI_TRB_SIZE 16u

#define XHCI_CMD_RING_DEPTH 16u
#define XHCI_EVT_RING_DEPTH 16u

#define USB_DESC_INTERFACE 4u
#define USB_DESC_ENDPOINT  5u

#define USB_EP_BULK 2u
#define USB_EP_INTR 3u

#define USB_CLASS_HID     0x03u
#define USB_CLASS_STORAGE 0x08u

#define USB_CTRL_BUF_SIZE 512u

#define BOT_CBW_SIGNATURE 0x43425355u
#define BOT_CBW_SIZE      31u

#define SCSI_READ10  0x28u
#define SCSI_WRITE10 0x2Au

#define USB_PORT_EMPTY 0u

#define USB_DEV_POWERED    0u
#define USB_DEV_DEFAULT    1u
#define USB_DEV_ADDRESSED  2u
#define USB_DEV_CONFIGURED 3u

#define USB_DRV_NONE    0u
#define USB_DRV_HID_KB    1u
#define USB_DRV_HID_MOUSE 2u
#define USB_DRV_MSC_BOT   3u

typedef struct {
    uint32_t state;
    uint32_t base;
    uint32_t op_base;
    uint32_t pci_bus_addr;
    uint32_t max_ports;
    uint32_t max_slots;
    uint32_t dcbaa;
    uint32_t cmd_ring;
    uint32_t evt_ring;
    uint32_t erst;
    uint32_t cmd_tail;
    uint32_t evt_head;
    uint32_t evt_phase;
    uint32_t db_base;
    uint32_t next_slot;
    uint32_t scratchpad;
} xhci_ctrl_t;

typedef struct {
    uint32_t slot;
    uint32_t addr;
    uint32_t port;
    uint32_t state;
    uint32_t speed;
    uint32_t if_class;
    uint32_t if_subclass;
    uint32_t if_proto;
    uint32_t ctrl_ep_max;
    uint32_t in_ep;
    uint32_t out_ep;
    uint32_t in_bulk_ep;
    uint32_t in_maxpkt;
    uint32_t out_maxpkt;
    uint32_t config_val;
    uint32_t driver;
    void *blkdev;
    uint32_t bot_tag;
} usb_device_t;

static xhci_ctrl_t usb_controllers[XHCI_CAPACITY];
static usb_device_t usb_devices[USB_DEV_CAPACITY];
static uint32_t usb_ctrl_count = 0;
static uint32_t usb_dev_count = 0;
static uint32_t usb_next_addr = 1;
static uint32_t usb_msc_block_size = 512;
static uint32_t usb_msc_block_cnt_lo = 0;
static uint32_t usb_bot_next_tag = 1;
static int usb_core_ready = 0;
static uint8_t usb_ctrl_buf[USB_CTRL_BUF_SIZE];

static inline uint32_t mmio_read32(uint32_t base, uint32_t offset) {
    return *(volatile uint32_t *)(base + offset);
}
static inline void mmio_write32(uint32_t base, uint32_t offset, uint32_t value) {
    *(volatile uint32_t *)(base + offset) = value;
}
static inline void io_fence(void) {
    __asm__ volatile ("outb %0, %1" : : "a"((uint8_t)0), "Nd"((uint16_t)0x80));
}

/* Im Boot-Polling-Kontext: echte EP0-Interaktion via xHCI-Transfer-Rings
   ist noch nicht implementiert (wie im alten ASM). Gibt immer -1 zurueck,
   bis der Transfer-Ring-Mechanismus existiert. */
static int usb_control_read_descriptor(uint32_t usb_addr, uint32_t length, uint32_t desc_type) {
    (void)usb_addr;
    (void)length;
    (void)desc_type;
    return -1;
}

static int usb_set_address(usb_device_t *dev, uint32_t addr) {
    (void)dev;
    (void)addr;
    return 0;
}

static int usb_set_configuration(usb_device_t *dev, uint32_t config_value) {
    (void)dev;
    (void)config_value;
    return 0;
}

static int usb_parse_config_descriptor(usb_device_t *dev) {
    uint16_t total_length = *(const uint16_t *)(usb_ctrl_buf + 2);
    if (total_length < 9 || total_length > USB_CTRL_BUF_SIZE) return -1;

    uint32_t offset = 9;
    uint32_t remaining = total_length - 9u;

    while (remaining >= 2) {
        uint8_t b_length = usb_ctrl_buf[offset];
        if (b_length == 0) return -1;
        if (b_length > remaining) return -1;
        uint8_t b_type = usb_ctrl_buf[offset + 1];

        if (b_type == USB_DESC_INTERFACE) {
            if (b_length < 9) return -1;
            dev->if_class = usb_ctrl_buf[offset + 5];
            dev->if_subclass = usb_ctrl_buf[offset + 6];
            dev->if_proto = usb_ctrl_buf[offset + 7];
        } else if (b_type == USB_DESC_ENDPOINT) {
            if (b_length < 7) return -1;
            uint8_t ep_addr = usb_ctrl_buf[offset + 2];
            uint8_t ep_type = usb_ctrl_buf[offset + 3] & 3u;
            uint16_t max_pkt = *(const uint16_t *)(usb_ctrl_buf + offset + 4);

            if (ep_addr & 0x80u) {
                if (ep_type == USB_EP_INTR) {
                    dev->in_ep = ep_addr & 0x7Fu;
                    dev->in_maxpkt = max_pkt;
                } else if (ep_type == USB_EP_BULK) {
                    dev->in_bulk_ep = ep_addr & 0x7Fu;
                    dev->in_maxpkt = max_pkt;
                }
            } else {
                if (ep_type == USB_EP_BULK) {
                    dev->out_ep = ep_addr & 0x7Fu;
                    dev->out_maxpkt = max_pkt;
                }
            }
        }

        offset += b_length;
        remaining -= b_length;
    }
    return 0;
}

/* Permanenter Polling-Stub (siehe usb.h). */
static int usb_bot_reset(void) { return 0; }
static int usb_scsi_inquiry(void) { return 0; }
static int usb_scsi_test_unit_ready(void) { return 0; }
static int usb_scsi_read_capacity(void) {
    usb_msc_block_size = 512;
    usb_msc_block_cnt_lo = 1;
    return 0;
}

static int usb_blk_read(void *record, uint32_t lba_lo, uint32_t lba_hi, uint32_t count, void *buffer) {
    (void)lba_hi;
    (void)buffer;
    uint8_t cbw[BOT_CBW_SIZE] = {0};
    *(uint32_t *)(cbw + 0) = BOT_CBW_SIGNATURE;
    *(uint32_t *)(cbw + 4) = usb_bot_next_tag++;
    *(uint32_t *)(cbw + 8) = count * usb_msc_block_size;
    cbw[12] = 0x80u; /* Data-In */
    cbw[13] = 0;
    cbw[14] = 10;
    cbw[15] = SCSI_READ10;
    uint32_t lba_be = __builtin_bswap32(lba_lo);
    *(uint32_t *)(cbw + 17) = lba_be;

    /* Bulk OUT (CBW), Bulk IN (Daten), Bulk IN (CSW): Polling-Stub, Erfolg
       simulieren (siehe usb.h). */
    (void)record;
    return 0;
}

static int usb_blk_write(void *record, uint32_t lba_lo, uint32_t lba_hi, uint32_t count, void *buffer) {
    (void)lba_hi;
    (void)buffer;
    uint8_t cbw[BOT_CBW_SIZE] = {0};
    *(uint32_t *)(cbw + 0) = BOT_CBW_SIGNATURE;
    *(uint32_t *)(cbw + 4) = usb_bot_next_tag++;
    *(uint32_t *)(cbw + 8) = count * usb_msc_block_size;
    cbw[12] = 0x00u; /* Data-Out */
    cbw[13] = 0;
    cbw[14] = 10;
    cbw[15] = SCSI_WRITE10;
    uint32_t lba_be = __builtin_bswap32(lba_lo);
    *(uint32_t *)(cbw + 17) = lba_be;

    (void)record;
    return 0;
}

static void usb_msc_init(usb_device_t *dev) {
    if (usb_bot_reset() != 0) return;
    if (usb_scsi_inquiry() != 0) return;
    usb_scsi_test_unit_ready(); /* Fehler ignorieren (Unit Attention erwartet) */
    if (usb_scsi_read_capacity() != 0) return;

    block_register_params_t params = {
        .id = 0x55534220u, /* "USB " */
        .flags = BLOCK_FLAG_FLUSH_CAP | BLOCK_FLAG_REMOVABLE,
        .block_size = usb_msc_block_size,
        .block_count_lo = usb_msc_block_cnt_lo,
        .block_count_hi = 0,
        .transport = BLOCK_TRANSPORT_USB,
        .driver_handle = (uint32_t)dev,
    };
    void *record = 0;
    if (block_device_register(&params, usb_blk_read, usb_blk_write, 0, 0, &record) != 0) {
        return;
    }
    dev->blkdev = record;
    serial_puts("NOVA: USB Mass Storage als Block-Device registriert\r\n");
}

static void usb_bind_class_driver(usb_device_t *dev) {
    if (dev->if_class == USB_CLASS_HID) {
        if (dev->if_proto == 1) {
            dev->driver = USB_DRV_HID_KB;
            serial_puts("NOVA: USB HID Tastatur erkannt\r\n");
        } else if (dev->if_proto == 2) {
            dev->driver = USB_DRV_HID_MOUSE;
            serial_puts("NOVA: USB HID Maus erkannt\r\n");
        }
    } else if (dev->if_class == USB_CLASS_STORAGE) {
        /* Subclass=6 (SCSI), Protocol=0x50 (BOT) oder 0x62 (UASP-Fallback auf BOT). */
        if (dev->if_proto == 0x50u || dev->if_proto == 0x62u) {
            dev->driver = USB_DRV_MSC_BOT;
            usb_msc_init(dev);
        }
    }
}

static void usb_core_enumerate_device(xhci_ctrl_t *ctrl, uint32_t speed, uint32_t port) {
    (void)ctrl;
    usb_device_t *dev = 0;
    for (uint32_t i = 0; i < USB_DEV_CAPACITY; ++i) {
        if (usb_devices[i].state == USB_DEV_POWERED && usb_devices[i].addr == 0) {
            dev = &usb_devices[i];
            break;
        }
    }
    if (!dev) return;

    *dev = (usb_device_t){0};
    dev->port = port;
    dev->speed = speed;
    dev->state = USB_DEV_DEFAULT;

    if (usb_control_read_descriptor(0, 8, 1) != 0) goto failed;

    uint8_t ep0_max = usb_ctrl_buf[7];
    if (ep0_max == 0) goto failed;
    dev->ctrl_ep_max = ep0_max;

    uint32_t new_addr = usb_next_addr++;
    dev->addr = new_addr;
    if (usb_set_address(dev, new_addr) != 0) goto failed;
    dev->state = USB_DEV_ADDRESSED;

    if (usb_control_read_descriptor(new_addr, 18, 1) != 0) goto failed;
    if (usb_control_read_descriptor(new_addr, 9, 2) != 0) goto failed;

    uint16_t total_length = *(const uint16_t *)(usb_ctrl_buf + 2);
    if (total_length > USB_CTRL_BUF_SIZE) total_length = USB_CTRL_BUF_SIZE;
    if (usb_control_read_descriptor(new_addr, total_length, 2) != 0) goto failed;

    if (usb_parse_config_descriptor(dev) != 0) goto failed;

    uint8_t config_value = usb_ctrl_buf[5];
    if (usb_set_configuration(dev, config_value) != 0) goto failed;
    dev->config_val = config_value;
    dev->state = USB_DEV_CONFIGURED;

    usb_bind_class_driver(dev);

    ++usb_dev_count;
    return;

failed:
    dev->state = USB_DEV_POWERED;
}

static void xhci_enumerate_ports(xhci_ctrl_t *ctrl) {
    for (uint32_t port = 0; port < ctrl->max_ports; ++port) {
        uint32_t portsc_addr = ctrl->op_base + XHCI_PORTSC_OFF + port * XHCI_PORTSC_SIZE;

        if ((mmio_read32(portsc_addr, 0) & XHCI_PS_PP) == 0) {
            mmio_write32(portsc_addr, 0, mmio_read32(portsc_addr, 0) | XHCI_PS_PP);
            uint32_t target = timer_get_ticks() + 5; /* ~50ms Debounce */
            while (timer_get_ticks() < target) {
                __asm__ volatile ("pause");
            }
        }

        if ((mmio_read32(portsc_addr, 0) & XHCI_PS_CCS) == 0) {
            continue; /* nicht verbunden */
        }

        mmio_write32(portsc_addr, 0, mmio_read32(portsc_addr, 0) | XHCI_PS_CSC);

        mmio_write32(portsc_addr, 0, mmio_read32(portsc_addr, 0) | XHCI_PS_PR);
        uint32_t timeout = timer_get_ticks() + 10; /* 100ms */
        int reset_ok = 0;
        while (timer_get_ticks() < timeout) {
            if (mmio_read32(portsc_addr, 0) & XHCI_PS_PRC) {
                reset_ok = 1;
                break;
            }
        }
        if (!reset_ok) continue;
        mmio_write32(portsc_addr, 0, mmio_read32(portsc_addr, 0) | XHCI_PS_PRC);

        if ((mmio_read32(portsc_addr, 0) & XHCI_PS_PED) == 0) continue;

        uint32_t speed = (mmio_read32(portsc_addr, 0) & XHCI_PS_SPEED) >> XHCI_PS_SPEED_SH;

        usb_core_enumerate_device(ctrl, speed, port + 1);
    }
}

static int xhci_init_controller(uint32_t pci_bus_addr, uint32_t bar0) {
    if (usb_ctrl_count >= XHCI_CAPACITY) return -1;
    xhci_ctrl_t *ctrl = &usb_controllers[usb_ctrl_count];

    driver_enable_bus_master(pci_bus_addr);

    if (bar0 == 0) return -1;
    ctrl->base = bar0;
    ctrl->pci_bus_addr = pci_bus_addr;

    for (uint32_t i = 0; i < 16u; ++i) {
        uint32_t page = bar0 + i * 4096u;
        if (paging_map_page(page, page, PAGING_PAGE_PRESENT | PAGING_PAGE_WRITE | 0x18u) != 0) {
            return -1;
        }
    }

    uint8_t caplength = *(const volatile uint8_t *)(uintptr_t)(ctrl->base + XHCI_CAPLENGTH);
    ctrl->op_base = ctrl->base + caplength;

    uint32_t hcsparams1 = mmio_read32(ctrl->base, XHCI_HCSPARAMS1);
    ctrl->max_ports = (hcsparams1 >> 24) & 0xFFu;
    uint32_t max_slots = hcsparams1 & 0xFFu;
    ctrl->max_slots = (max_slots > 8u) ? 8u : max_slots;
    ctrl->db_base = ctrl->base + mmio_read32(ctrl->base, XHCI_DBOFF);

    /* Schritt 1: Controller stoppen, auf HCHalted warten. */
    mmio_write32(ctrl->op_base, XHCI_USBCMD, mmio_read32(ctrl->op_base, XHCI_USBCMD) & ~XHCI_CMD_RS);
    uint32_t spins = 1000000u;
    for (;;) {
        if (spins-- == 0) return -1;
        if (mmio_read32(ctrl->op_base, XHCI_USBSTS) & XHCI_STS_HCH) break;
    }

    /* Schritt 2: Host Controller Reset. */
    mmio_write32(ctrl->op_base, XHCI_USBCMD, mmio_read32(ctrl->op_base, XHCI_USBCMD) | XHCI_CMD_HCRST);
    io_fence();
    spins = 2000000u;
    for (;;) {
        if (spins-- == 0) return -1;
        if ((mmio_read32(ctrl->op_base, XHCI_USBCMD) & XHCI_CMD_HCRST) == 0) break;
    }
    spins = 1000000u;
    for (;;) {
        if (spins-- == 0) return -1;
        if ((mmio_read32(ctrl->op_base, XHCI_USBSTS) & XHCI_STS_CNR) == 0) break;
    }

    /* Schritt 3: DCBAA. */
    uint32_t numa;
    uint32_t dcbaa = pmm_alloc_page(&numa);
    if (dcbaa == 0) return -1;
    ctrl->dcbaa = dcbaa;
    for (uint32_t i = 0; i < PMM_PAGE_SIZE; ++i) ((uint8_t *)dcbaa)[i] = 0;

    /* Schritt 4: Scratchpad-Seite. */
    uint32_t scratchpad = pmm_alloc_page(&numa);
    if (scratchpad == 0) return -1;
    ctrl->scratchpad = scratchpad;
    *(uint32_t *)dcbaa = scratchpad;
    *(uint32_t *)(dcbaa + 4) = 0;

    /* Schritt 5: Command Ring + Link-TRB. */
    uint32_t cmd_ring = pmm_alloc_page(&numa);
    if (cmd_ring == 0) return -1;
    ctrl->cmd_ring = cmd_ring;
    for (uint32_t i = 0; i < PMM_PAGE_SIZE; ++i) ((uint8_t *)cmd_ring)[i] = 0;
    uint8_t *link_trb = (uint8_t *)(cmd_ring + XHCI_CMD_RING_DEPTH * XHCI_TRB_SIZE);
    *(uint32_t *)(link_trb + 0) = cmd_ring;
    *(uint32_t *)(link_trb + 4) = 0;
    *(uint32_t *)(link_trb + 8) = 0;
    *(uint32_t *)(link_trb + 12) = (XHCI_TRB_LINK << 10) | 0x02u | 0x01u; /* Type=Link, TC=1, Cycle=1 */
    ctrl->cmd_tail = 0;

    /* Schritt 6: Event Ring Segment Table + Event Ring. */
    uint32_t erst = pmm_alloc_page(&numa);
    if (erst == 0) return -1;
    ctrl->erst = erst;
    for (uint32_t i = 0; i < PMM_PAGE_SIZE; ++i) ((uint8_t *)erst)[i] = 0;

    uint32_t evt_ring = pmm_alloc_page(&numa);
    if (evt_ring == 0) return -1;
    ctrl->evt_ring = evt_ring;
    for (uint32_t i = 0; i < PMM_PAGE_SIZE; ++i) ((uint8_t *)evt_ring)[i] = 0;

    *(uint32_t *)(erst + 0) = evt_ring;
    *(uint32_t *)(erst + 4) = 0;
    *(uint32_t *)(erst + 8) = XHCI_EVT_RING_DEPTH;
    *(uint32_t *)(erst + 12) = 0;

    ctrl->evt_head = 0;
    ctrl->evt_phase = 1;
    ctrl->next_slot = 1;

    /* Schritt 7: Runtime Interrupter 0. */
    uint32_t ir0 = ctrl->base + mmio_read32(ctrl->base, XHCI_RTSOFF) + 0x20u;
    mmio_write32(ir0, 0x08u, 1); /* ERSTSZ */
    mmio_write32(ir0, 0x10u, erst); /* ERSTBA lo */
    mmio_write32(ir0, 0x14u, 0);
    mmio_write32(ir0, 0x18u, evt_ring | 1u); /* ERDP lo, Cycle Bit */
    mmio_write32(ir0, 0x1Cu, 0);

    /* Schritt 8: DCBAAP, CRCR, CONFIG. */
    mmio_write32(ctrl->op_base, XHCI_DCBAAP_LO, dcbaa);
    mmio_write32(ctrl->op_base, XHCI_DCBAAP_LO + 4u, 0);
    mmio_write32(ctrl->op_base, XHCI_CRCR_LO, cmd_ring | 1u);
    mmio_write32(ctrl->op_base, XHCI_CRCR_LO + 4u, 0);
    mmio_write32(ctrl->op_base, XHCI_CONFIG, ctrl->max_slots);

    /* Schritt 9: Controller starten. */
    mmio_write32(ctrl->op_base, XHCI_USBCMD, mmio_read32(ctrl->op_base, XHCI_USBCMD) | XHCI_CMD_RS);
    io_fence();
    spins = 1000000u;
    for (;;) {
        if (spins-- == 0) return -1;
        if ((mmio_read32(ctrl->op_base, XHCI_USBSTS) & XHCI_STS_HCH) == 0) break;
    }

    ctrl->state = USB_CORE_RUNNING;
    ++usb_ctrl_count;

    /* Schritt 10: Root-Hub-Ports enumerieren. */
    xhci_enumerate_ports(ctrl);

    return 0;
}

int usb_initialize(void) {
    usb_core_ready = 0;
    usb_ctrl_count = 0;
    usb_dev_count = 0;
    for (uint32_t i = 0; i < XHCI_CAPACITY; ++i) usb_controllers[i] = (xhci_ctrl_t){0};
    for (uint32_t i = 0; i < USB_DEV_CAPACITY; ++i) usb_devices[i] = (usb_device_t){0};

    for (uint32_t index = 0; usb_ctrl_count < XHCI_CAPACITY; ++index) {
        uint32_t bus_addr = 0, bar0 = 0;
        if (disc_find_matched(PCI_CLASS_XHCI, index, &bus_addr, &bar0) != 0) {
            break;
        }
        xhci_init_controller(bus_addr, bar0);
    }

    usb_core_ready = 1;
    return 0;
}

int usb_self_test(void) {
    if (usb_core_ready != 1) return -1;

    if (usb_ctrl_count == 0) return 0; /* kein Controller - kein Fehler (NPSPEC) */

    if (usb_controllers[0].state != USB_CORE_RUNNING) return -1;

    return 0;
}
