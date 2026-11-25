/* NovaOS C-Kernel - USB-Stack: xHCI-Controller, USB-Core (Port aus
   usb32.inc). Siehe usb.h fuer den aktuellen Implementierungsstand (echte
   EP0-Control-Transfers, Bulk/BOT weiterhin offen). */
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
#define XHCI_EP_RING_DEPTH  16u

/* TRB-Typen (xHCI-Spec 6.4.6), nur die hier tatsaechlich verwendeten. */
#define XHCI_TRB_TYPE_SHIFT       10u
#define XHCI_TRB_TYPE_SETUP       2u
#define XHCI_TRB_TYPE_DATA        3u
#define XHCI_TRB_TYPE_STATUS      4u
#define XHCI_TRB_TYPE_ENABLE_SLOT 9u
#define XHCI_TRB_TYPE_ADDR_DEV    11u
#define XHCI_TRB_TYPE_TRANSFER_EV 32u
#define XHCI_TRB_TYPE_CMD_COMPL   33u

#define XHCI_TRB_CYCLE   0x00000001u
#define XHCI_TRB_TC      0x00000002u /* Toggle Cycle (nur Link-TRB) */
#define XHCI_TRB_IOC     0x00000020u
#define XHCI_TRB_IDT     0x00000040u /* Immediate Data (nur Setup-TRB) */
#define XHCI_TRB_DIR_IN  0x00010000u /* Bit16: Data-/Status-Stage-Richtung */

#define XHCI_CMD_DB 0u /* Doorbell-Array-Index 0 = Host Controller Commands */

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
    uint32_t cmd_cycle;
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
    uint32_t dev_ctx;    /* Device-Context-Seite (von DCBAA[slot] referenziert) */
    uint32_t in_ctx;     /* Input-Context-Seite (fuer Address-Device-Kommando) */
    uint32_t ep0_ring;   /* EP0-Transfer-Ring (eigene Seite pro Geraet) */
    uint32_t ep0_enqueue;
    uint32_t ep0_cycle;
} usb_device_t;

static xhci_ctrl_t usb_controllers[XHCI_CAPACITY];
static usb_device_t usb_devices[USB_DEV_CAPACITY];
static uint32_t usb_ctrl_count = 0;
static uint32_t usb_dev_count = 0;
static uint32_t usb_msc_block_size = 512;
static uint32_t usb_msc_block_cnt_lo = 0;
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

/* --------------------------------------------------------------------- */
/* Echte xHCI-Command-Ring- und EP0-Transfer-Ring-Engine (ersetzt die      */
/* bisherigen permanenten ASM-Stubs, siehe usb.h).                        */
/* --------------------------------------------------------------------- */

/* Reicht genau 1 Command-TRB ein (Link-TRB-Wrap inkl. Cycle-Bit-Toggle
   identisch zum bereits bestehenden Ring-Aufbau in xhci_init_controller).
   Ruft danach das Host-Controller-Doorbell. *out_trb_addr ist die
   physische Adresse des eingereihten TRB - dient als Korrelations-
   schluessel fuer das zugehoerige Command-Completion-Event. */
static void xhci_cmd_ring_submit(xhci_ctrl_t *ctrl, uint32_t dw0, uint32_t dw1, uint32_t dw2,
                                  uint32_t dw3_no_cycle, uint32_t *out_trb_addr) {
    uint32_t trb = ctrl->cmd_ring + ctrl->cmd_tail * XHCI_TRB_SIZE;
    *(volatile uint32_t *)(trb + 0) = dw0;
    *(volatile uint32_t *)(trb + 4) = dw1;
    *(volatile uint32_t *)(trb + 8) = dw2;
    *(volatile uint32_t *)(trb + 12) = dw3_no_cycle | ctrl->cmd_cycle;
    if (out_trb_addr) *out_trb_addr = trb;

    ++ctrl->cmd_tail;
    if (ctrl->cmd_tail >= XHCI_CMD_RING_DEPTH) {
        uint8_t *link_trb = (uint8_t *)(ctrl->cmd_ring + XHCI_CMD_RING_DEPTH * XHCI_TRB_SIZE);
        uint32_t link_dw3 = (XHCI_TRB_LINK << XHCI_TRB_TYPE_SHIFT) | XHCI_TRB_TC;
        *(volatile uint32_t *)(link_trb + 12) = link_dw3 | ctrl->cmd_cycle;
        ctrl->cmd_tail = 0;
        ctrl->cmd_cycle ^= 1u;
    }

    io_fence();
    mmio_write32(ctrl->db_base, XHCI_CMD_DB * 4u, 0);
}

/* Liest genau EIN neues Event aus dem Event-Ring (Cycle-Bit-Vergleich mit
   der aktuellen Producer-Cycle-State-Erwartung), aktualisiert ERDP. 0 =
   ein Event wurde geliefert, -1 = Ring ist leer (kein neues Event). */
static int xhci_poll_event(xhci_ctrl_t *ctrl, uint32_t *dw0, uint32_t *dw1, uint32_t *dw2, uint32_t *dw3) {
    uint8_t *trb = (uint8_t *)(ctrl->evt_ring + ctrl->evt_head * XHCI_TRB_SIZE);
    uint32_t d3 = *(volatile uint32_t *)(trb + 12);
    if ((d3 & XHCI_TRB_CYCLE) != ctrl->evt_phase) return -1;

    *dw0 = *(volatile uint32_t *)(trb + 0);
    *dw1 = *(volatile uint32_t *)(trb + 4);
    *dw2 = *(volatile uint32_t *)(trb + 8);
    *dw3 = d3;

    ++ctrl->evt_head;
    if (ctrl->evt_head >= XHCI_EVT_RING_DEPTH) {
        ctrl->evt_head = 0;
        ctrl->evt_phase ^= 1u;
    }

    uint32_t ir0 = ctrl->base + mmio_read32(ctrl->base, XHCI_RTSOFF) + 0x20u;
    uint32_t erdp = ctrl->evt_ring + ctrl->evt_head * XHCI_TRB_SIZE;
    mmio_write32(ir0, 0x18u, erdp | 1u);
    mmio_write32(ir0, 0x1Cu, 0);
    return 0;
}

/* Wartet (Tick- UND Spin-begrenzt, Muster identisch zu ahci_wait_mmio) auf
   ein Event vom gegebenen Typ, dessen TRB-Pointer-Feld (DWORD0) exakt
   match_ptr entspricht - so lassen sich Befehle/Transfers eindeutig ihrem
   eigenen Completion-Event zuordnen, auch wenn dazwischen andere Events
   (z.B. Port-Status-Change) eintreffen. 0 = Treffer (*out_dw2/_dw3
   gefuellt), -1 = Timeout. */
static int xhci_wait_event(xhci_ctrl_t *ctrl, uint32_t trb_type, uint32_t match_ptr,
                            uint32_t timeout_ticks, uint32_t *out_dw2, uint32_t *out_dw3) {
    uint32_t start = timer_get_ticks();
    uint32_t spins = 2000000u;
    for (;;) {
        uint32_t dw0, dw1, dw2, dw3;
        if (xhci_poll_event(ctrl, &dw0, &dw1, &dw2, &dw3) == 0) {
            uint32_t type = (dw3 >> XHCI_TRB_TYPE_SHIFT) & 0x3Fu;
            if (type == trb_type && dw0 == match_ptr) {
                if (out_dw2) *out_dw2 = dw2;
                if (out_dw3) *out_dw3 = dw3;
                return 0;
            }
            /* Fremdes Event (z.B. Port-Status-Change) - ignorieren, aber
               NICHT per continue die Timeout-Pruefung unten uebergehen:
               ein Strom fremder Events wuerde sonst das Warten unbegrenzt
               verlaengern (gefunden+behoben waehrend des Boot-Tests - ein
               echter Haenger, kein Short-Packet-Fall). */
        } else if (spins-- == 0) {
            return -1;
        }
        if (timer_get_ticks() - start >= timeout_ticks) return -1;
        __asm__ volatile ("pause");
    }
}

static int xhci_enable_slot(xhci_ctrl_t *ctrl, uint32_t *out_slot_id) {
    uint32_t trb_addr = 0;
    uint32_t dw3 = (XHCI_TRB_TYPE_ENABLE_SLOT << XHCI_TRB_TYPE_SHIFT);
    xhci_cmd_ring_submit(ctrl, 0, 0, 0, dw3, &trb_addr);

    uint32_t evt_dw2, evt_dw3;
    if (xhci_wait_event(ctrl, XHCI_TRB_TYPE_CMD_COMPL, trb_addr, 500u, &evt_dw2, &evt_dw3) != 0) return -1;
    if ((evt_dw2 >> 24) != 1u) return -1; /* Completion Code 1 = Success */
    *out_slot_id = evt_dw3 >> 24;
    return 0;
}

static int xhci_address_device(xhci_ctrl_t *ctrl, uint32_t slot_id, uint32_t input_ctx) {
    uint32_t trb_addr = 0;
    uint32_t dw3 = (XHCI_TRB_TYPE_ADDR_DEV << XHCI_TRB_TYPE_SHIFT) | (slot_id << 24);
    xhci_cmd_ring_submit(ctrl, input_ctx, 0, 0, dw3, &trb_addr);

    uint32_t evt_dw2, evt_dw3;
    if (xhci_wait_event(ctrl, XHCI_TRB_TYPE_CMD_COMPL, trb_addr, 500u, &evt_dw2, &evt_dw3) != 0) return -1;
    return ((evt_dw2 >> 24) == 1u) ? 0 : -1;
}

/* Liefert den sicheren EP0-Max-Packet-Size-Default je Port-Speed (PORTSC-
   Speed-ID == Slot-Context-Speed-ID, beide nach derselben xHCI-Tabelle
   kodiert). Fuer Full-Speed bewusst der garantiert gueltige Minimalwert 8
   (reale Geraete erlauben 8/16/32/64, aber ein zu KLEIN gewaehlter Wert
   ist immer korrekt, nur weniger effizient - ein zu GROSSER waere ein
   Protokollfehler). Low-/High-/SuperSpeed haben jeweils nur einen
   gueltigen Wert, keine Mehrdeutigkeit. Keine Evaluate-Context-
   Verfeinerung in diesem Umfang (siehe KERNEL_GAP_ANALYSIS.md). */
static uint32_t xhci_default_ep0_max_packet(uint32_t speed) {
    switch (speed) {
        case 2u: return 8u;   /* Low-Speed */
        case 1u: return 8u;   /* Full-Speed: sicherer Minimalwert */
        case 3u: return 64u;  /* High-Speed */
        case 4u: return 512u; /* SuperSpeed */
        default: return 8u;
    }
}

/* Reiht genau 1 TRB in den EP0-Transfer-Ring des Geraets ein (gleiches
   Link-TRB-Wrap-Muster wie beim Command-Ring, aber pro Geraet statt
   pro Controller). */
static void xhci_ep0_ring_submit(usb_device_t *dev, uint32_t dw0, uint32_t dw1, uint32_t dw2,
                                  uint32_t dw3_no_cycle, uint32_t *out_trb_addr) {
    uint32_t trb = dev->ep0_ring + dev->ep0_enqueue * XHCI_TRB_SIZE;
    *(volatile uint32_t *)(trb + 0) = dw0;
    *(volatile uint32_t *)(trb + 4) = dw1;
    *(volatile uint32_t *)(trb + 8) = dw2;
    *(volatile uint32_t *)(trb + 12) = dw3_no_cycle | dev->ep0_cycle;
    if (out_trb_addr) *out_trb_addr = trb;

    ++dev->ep0_enqueue;
    if (dev->ep0_enqueue >= XHCI_EP_RING_DEPTH) {
        uint8_t *link_trb = (uint8_t *)(dev->ep0_ring + XHCI_EP_RING_DEPTH * XHCI_TRB_SIZE);
        uint32_t link_dw3 = (XHCI_TRB_LINK << XHCI_TRB_TYPE_SHIFT) | XHCI_TRB_TC;
        *(volatile uint32_t *)(link_trb + 12) = link_dw3 | dev->ep0_cycle;
        dev->ep0_enqueue = 0;
        dev->ep0_cycle ^= 1u;
    }
}

/* Fuehrt einen vollstaendigen Standard-Control-Transfer auf EP0 aus
   (Setup-Stage + optionale Data-Stage + Status-Stage, xHCI-Spec 4.11.2.2).
   buffer muss bei wLength!=0 ein physisch adressierbarer, fuer die
   Transferrichtung gueltiger Puffer sein (hier immer usb_ctrl_buf). 0 =
   Erfolg (Completion Code Success ODER Short Packet - bei GET_DESCRIPTOR
   mit groesserem wLength als die tatsaechliche Deskriptorlaenge normal
   und kein Fehler), -1 sonst. */
static int xhci_control_transfer(xhci_ctrl_t *ctrl, usb_device_t *dev, uint8_t bmRequestType,
                                  uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLength,
                                  void *buffer, int data_in) {
    uint32_t setup_dw0 = bmRequestType | ((uint32_t)bRequest << 8) | ((uint32_t)wValue << 16);
    uint32_t setup_dw1 = wIndex | ((uint32_t)wLength << 16);
    uint32_t trt = (wLength == 0u) ? 0u : (data_in ? 3u : 2u); /* TRT: 2=OUT-Data,3=IN-Data */
    uint32_t setup_dw3 = (XHCI_TRB_TYPE_SETUP << XHCI_TRB_TYPE_SHIFT) | XHCI_TRB_IDT | (trt << 16);
    uint32_t setup_trb = 0;
    xhci_ep0_ring_submit(dev, setup_dw0, setup_dw1, 8u, setup_dw3, &setup_trb);

    uint32_t last_trb = setup_trb;
    if (wLength != 0u) {
        uint32_t data_dw3 = (XHCI_TRB_TYPE_DATA << XHCI_TRB_TYPE_SHIFT) | (data_in ? XHCI_TRB_DIR_IN : 0u);
        uint32_t data_trb = 0;
        xhci_ep0_ring_submit(dev, (uint32_t)(uintptr_t)buffer, 0, wLength, data_dw3, &data_trb);
        last_trb = data_trb;
    }

    /* Status-Stage-Richtung ist immer entgegengesetzt zur Datenphase; ohne
       Datenphase (wLength==0) immer IN (xHCI-Spec 4.11.2.2). */
    uint32_t status_dir = (wLength == 0u) ? XHCI_TRB_DIR_IN : (data_in ? 0u : XHCI_TRB_DIR_IN);
    uint32_t status_dw3 = (XHCI_TRB_TYPE_STATUS << XHCI_TRB_TYPE_SHIFT) | status_dir | XHCI_TRB_IOC;
    uint32_t status_trb = 0;
    xhci_ep0_ring_submit(dev, 0, 0, 0, status_dw3, &status_trb);
    last_trb = status_trb;

    io_fence();
    mmio_write32(ctrl->db_base, dev->slot * 4u, 1u); /* DCI=1 fuer EP0 */

    uint32_t evt_dw2 = 0, evt_dw3 = 0;
    if (xhci_wait_event(ctrl, XHCI_TRB_TYPE_TRANSFER_EV, last_trb, 500u, &evt_dw2, &evt_dw3) != 0) return -1;
    uint32_t completion_code = evt_dw2 >> 24;
    if (completion_code != 1u && completion_code != 13u) return -1; /* 13 = Short Packet */
    return 0;
}

static int usb_control_read_descriptor(xhci_ctrl_t *ctrl, usb_device_t *dev, uint32_t length, uint32_t desc_type) {
    if (length > USB_CTRL_BUF_SIZE) length = USB_CTRL_BUF_SIZE;
    return xhci_control_transfer(ctrl, dev, 0x80u, 6u /* GET_DESCRIPTOR */,
                                  (uint16_t)(desc_type << 8), 0, (uint16_t)length, usb_ctrl_buf, 1);
}

static int usb_set_configuration(xhci_ctrl_t *ctrl, usb_device_t *dev, uint32_t config_value) {
    return xhci_control_transfer(ctrl, dev, 0x00u, 9u /* SET_CONFIGURATION */,
                                  (uint16_t)config_value, 0, 0, 0, 0);
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

/* Echte Bulk-Transfers (CBW/Daten/CSW ueber Bulk-Endpoint-Transfer-Ringe,
   analog zum oben neu eingefuehrten EP0-Mechanismus) sind NICHT Teil
   dieses Umfangs (nur EP0-Control-Transfers, siehe KERNEL_GAP_ANALYSIS.md
   Abschnitt 2.8). Diese Funktionen gaben VORHER Fake-Erfolg zurueck, ohne
   jemals wirklich mit dem Geraet zu sprechen (vom Master-Auftrag
   verbotener Dummy-Code) - jetzt ehrlich -1, bis ein echter Bulk-
   Transfer-Mechanismus existiert. usb_msc_init() bricht dadurch VOR der
   Block-Device-Registrierung ab: es wird nie ein fingiertes USB-
   Massenspeicher-Geraet registriert. */
static int usb_bot_reset(void) { return -1; }
static int usb_scsi_inquiry(void) { return -1; }
static int usb_scsi_test_unit_ready(void) { return -1; }
static int usb_scsi_read_capacity(void) { return -1; }

static int usb_blk_read(void *record, uint32_t lba_lo, uint32_t lba_hi, uint32_t count, void *buffer) {
    (void)record;
    (void)lba_lo;
    (void)lba_hi;
    (void)count;
    (void)buffer;
    return -1;
}

static int usb_blk_write(void *record, uint32_t lba_lo, uint32_t lba_hi, uint32_t count, void *buffer) {
    (void)record;
    (void)lba_lo;
    (void)lba_hi;
    (void)count;
    (void)buffer;
    return -1;
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

/* Echte xHCI-Geraete-Enumeration (ersetzt die bisherige, strukturell
   unerreichbare Kette aus usb.h beschriebenen Stubs): Enable Slot ->
   Device-/Input-Context+EP0-Ring anlegen -> Address Device (der xHC
   fuehrt SET_ADDRESS dabei selbst aus, Software waehlt die Adresse bei
   xHCI NICHT selbst) -> Standard-Deskriptoren lesen -> Konfigurieren. */
static void usb_core_enumerate_device(xhci_ctrl_t *ctrl, uint32_t speed, uint32_t port) {
    usb_device_t *dev = 0;
    for (uint32_t i = 0; i < USB_DEV_CAPACITY; ++i) {
        if (usb_devices[i].state == USB_DEV_POWERED) {
            dev = &usb_devices[i];
            break;
        }
    }
    if (!dev) return;

    *dev = (usb_device_t){0};
    dev->port = port;
    dev->speed = speed;
    dev->state = USB_DEV_DEFAULT;

    uint32_t slot_id = 0;
    if (xhci_enable_slot(ctrl, &slot_id) != 0) goto failed;
    dev->slot = slot_id;

    uint32_t numa;
    uint32_t dev_ctx = pmm_alloc_page(&numa);
    if (dev_ctx == 0) goto failed;
    for (uint32_t i = 0; i < PMM_PAGE_SIZE / 4u; ++i) ((uint32_t *)dev_ctx)[i] = 0;
    dev->dev_ctx = dev_ctx;
    *(volatile uint32_t *)(ctrl->dcbaa + slot_id * 8u) = dev_ctx;
    *(volatile uint32_t *)(ctrl->dcbaa + slot_id * 8u + 4u) = 0;

    uint32_t ep0_ring = pmm_alloc_page(&numa);
    if (ep0_ring == 0) goto failed;
    for (uint32_t i = 0; i < PMM_PAGE_SIZE; ++i) ((uint8_t *)ep0_ring)[i] = 0;
    dev->ep0_ring = ep0_ring;
    dev->ep0_enqueue = 0;
    dev->ep0_cycle = 1;

    uint32_t in_ctx = pmm_alloc_page(&numa);
    if (in_ctx == 0) goto failed;
    for (uint32_t i = 0; i < PMM_PAGE_SIZE / 4u; ++i) ((uint32_t *)in_ctx)[i] = 0;
    dev->in_ctx = in_ctx;

    uint32_t ep0_max_packet = xhci_default_ep0_max_packet(speed);

    /* Input Control Context (Offset 0, 32 Byte): A0|A1 (Slot+EP0 hinzufuegen). */
    *(volatile uint32_t *)(in_ctx + 4u) = 0x00000003u;

    /* Slot Context (Offset 32): Speed[23:20], Context Entries[31:27]=1,
       Root Hub Port Number[15:8]. */
    uint32_t slot_ctx = in_ctx + 32u;
    *(volatile uint32_t *)(slot_ctx + 0u) = (speed << 20) | (1u << 27);
    *(volatile uint32_t *)(slot_ctx + 4u) = (port << 8);

    /* EP0-Kontext (Offset 64 = Slot + EP-Index 1*32): CErr=3[2:1],
       EP Type=4 Control[5:3], Max Packet Size[31:16], TR Dequeue Ptr+DCS,
       Average TRB Length=8. */
    uint32_t ep0_ctx = in_ctx + 64u;
    *(volatile uint32_t *)(ep0_ctx + 4u) = (3u << 1) | (4u << 3) | (ep0_max_packet << 16);
    *(volatile uint32_t *)(ep0_ctx + 8u) = ep0_ring | 1u;
    *(volatile uint32_t *)(ep0_ctx + 12u) = 0;
    *(volatile uint32_t *)(ep0_ctx + 16u) = 8u;

    if (xhci_address_device(ctrl, slot_id, in_ctx) != 0) goto failed;
    dev->addr = slot_id; /* interner Marker, KEINE echte USB-Adresse - die
                            vergibt bei Address-Device(BSR=0) der xHC selbst. */
    dev->state = USB_DEV_ADDRESSED;

    if (usb_control_read_descriptor(ctrl, dev, 8, 1) != 0) goto failed;
    uint8_t ep0_max = usb_ctrl_buf[7];
    if (ep0_max != 0) dev->ctrl_ep_max = ep0_max;

    if (usb_control_read_descriptor(ctrl, dev, 18, 1) != 0) goto failed;
    if (usb_control_read_descriptor(ctrl, dev, 9, 2) != 0) goto failed;

    uint16_t total_length = *(const uint16_t *)(usb_ctrl_buf + 2);
    if (total_length > USB_CTRL_BUF_SIZE) total_length = USB_CTRL_BUF_SIZE;
    if (usb_control_read_descriptor(ctrl, dev, total_length, 2) != 0) goto failed;

    if (usb_parse_config_descriptor(dev) != 0) goto failed;

    uint8_t config_value = usb_ctrl_buf[5];
    if (usb_set_configuration(ctrl, dev, config_value) != 0) goto failed;
    dev->config_val = config_value;
    dev->state = USB_DEV_CONFIGURED;

    usb_bind_class_driver(dev);

    ++usb_dev_count;
    return;

failed:
    /* Slot/Speicher werden bewusst NICHT wieder freigegeben (kein Disable-
       Slot-Kommando) - ein eigenstaendiger Folgepunkt, analog zum bereits
       in ipc.c behobenen Endpoint-Reaper-Muster, siehe
       KERNEL_GAP_ANALYSIS.md. Kein Sicherheitsrisiko fuer den Boot-Pfad:
       nur bei tatsaechlich angeschlossenen, aber fehlschlagenden USB-
       Geraeten betroffen. */
    dev->state = USB_DEV_POWERED;
}

static void xhci_enumerate_ports(xhci_ctrl_t *ctrl) {
    /* Generelle Settle-Zeit VOR dem ersten CCS-Check: war Port-Power schon
       vor dem Start an (bei emulierten Controllern oft der Fall), griff
       bisher GAR KEIN Debounce - der CCS-Check kam dann moeglicherweise
       fruehER als das Backend sein "verbunden"-Signal setzt (bei QEMU
       real beobachtet: USB-Tastatur wurde dadurch nie erkannt, nicht weil
       die Enumeration fehlerhaft war, sondern weil der Port-Scan schlicht
       zu frueh lief). Einmalig pro Controller, nicht pro Port. */
    uint32_t settle_target = timer_get_ticks() + 10; /* ~100ms */
    while (timer_get_ticks() < settle_target) {
        __asm__ volatile ("pause");
    }

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
    ctrl->cmd_cycle = 1;

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
