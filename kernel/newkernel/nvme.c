/* NovaOS C-Kernel - NVMe PCIe Controller Driver (Port aus nvme32.inc). */
#include "nvme.h"
#include "driver.h"
#include "paging.h"
#include "pmm.h"
#include "block.h"

/* NVMe MMIO Register-Offsets (BAR0-relativ). */
#define NVME_CAP_LO 0x00u
#define NVME_CAP_HI 0x04u
#define NVME_CC     0x14u
#define NVME_CSTS   0x1Cu
#define NVME_AQA    0x24u
#define NVME_ASQ_LO 0x28u
#define NVME_ASQ_HI 0x2Cu
#define NVME_ACQ_LO 0x30u
#define NVME_ACQ_HI 0x34u

#define NVME_CC_CSS_NVM  0x00000000u
#define NVME_CC_MPS_4K   0x00000000u
#define NVME_CC_AMS_RR   0x00000000u
#define NVME_CC_IOSQES   0x00060000u
#define NVME_CC_IOCQES   0x00400000u
#define NVME_CC_DEFAULT  (NVME_CC_CSS_NVM | NVME_CC_MPS_4K | NVME_CC_AMS_RR | NVME_CC_IOSQES | NVME_CC_IOCQES)
#define NVME_CC_EN       0x00000001u

#define NVME_CSTS_RDY 0x00000001u
#define NVME_CSTS_CFS 0x00000002u

#define NVME_ADM_CREATE_SQ 0x01u
#define NVME_ADM_CREATE_CQ 0x05u
#define NVME_ADM_IDENTIFY  0x06u

#define NVME_IO_FLUSH 0x00u
#define NVME_IO_WRITE 0x01u
#define NVME_IO_READ  0x02u

#define NVME_SQ_ENTRY 64u
#define NVME_CQ_ENTRY 16u

#define NVME_SQE_OPCODE  0u
#define NVME_SQE_CID     2u
#define NVME_SQE_NSID    4u
#define NVME_SQE_PRP1_LO 24u
#define NVME_SQE_PRP1_HI 28u
#define NVME_SQE_CDW10   40u
#define NVME_SQE_CDW11   44u
#define NVME_SQE_CDW12   48u

#define NVME_CQE_STATUS 14u

#define NVME_ID_CTL_NN   516u
#define NVME_ID_NS_NSZE  0u
#define NVME_ID_NS_LBAF0 128u

#define NVME_DB_BASE 0x1000u

typedef struct {
    uint32_t state;
    uint32_t base;
    uint32_t pci_bus_addr;
    uint32_t asq;
    uint32_t acq;
    uint32_t iosq;
    uint32_t iocq;
    uint32_t sqtail_adm;
    uint32_t cqhead_adm;
    uint32_t phase_adm;
    uint32_t sqtail_io;
    uint32_t cqhead_io;
    uint32_t phase_io;
    uint32_t next_cid;
    uint32_t ns_count;
    uint32_t dstride;
    uint32_t ident_page;
    uint32_t ns1_size_lo;
    uint32_t ns1_size_hi;
    uint32_t ns1_bsize;
    void *blkdev;
} nvme_ctrl_t;

static nvme_ctrl_t nvme_controllers[NVME_CAPACITY];
static uint32_t nvme_count = 0;
static int nvme_mgr_ready = 0;

static inline uint32_t mmio_read32(uint32_t base, uint32_t offset) {
    return *(volatile uint32_t *)(base + offset);
}
static inline void mmio_write32(uint32_t base, uint32_t offset, uint32_t value) {
    *(volatile uint32_t *)(base + offset) = value;
}
static inline void io_fence(void) {
    __asm__ volatile ("outb %0, %1" : : "a"((uint8_t)0), "Nd"((uint16_t)0x80));
}

static int nvme_submit_admin_cmd(nvme_ctrl_t *ctrl, const uint8_t *sqe, uint32_t *out_dw0) {
    uint8_t *sq_slot = (uint8_t *)(ctrl->asq + ctrl->sqtail_adm * NVME_SQ_ENTRY);
    for (uint32_t i = 0; i < NVME_SQ_ENTRY; ++i) sq_slot[i] = sqe[i];

    uint16_t cid = (uint16_t)ctrl->next_cid;
    *(uint16_t *)(sq_slot + NVME_SQE_CID) = cid;
    ++ctrl->next_cid;

    io_fence();

    ctrl->sqtail_adm = (ctrl->sqtail_adm + 1 >= NVME_ADMIN_DEPTH) ? 0 : ctrl->sqtail_adm + 1;
    mmio_write32(ctrl->base + NVME_DB_BASE, 0, ctrl->sqtail_adm);

    const uint8_t *cqe = (const uint8_t *)(ctrl->acq + ctrl->cqhead_adm * NVME_CQ_ENTRY);
    uint32_t spins = 2000000u;
    for (;;) {
        if (spins-- == 0) return -1;
        uint16_t status = *(const uint16_t *)(cqe + NVME_CQE_STATUS);
        if ((status & 1u) == ctrl->phase_adm) break;
    }

    uint16_t status = *(const uint16_t *)(cqe + NVME_CQE_STATUS);
    uint32_t sc_sct = (status >> 1) & 0x7FFu;

    ctrl->cqhead_adm = ctrl->cqhead_adm + 1;
    if (ctrl->cqhead_adm >= NVME_ADMIN_DEPTH) {
        ctrl->cqhead_adm = 0;
        ctrl->phase_adm ^= 1u;
    }
    mmio_write32(ctrl->base + NVME_DB_BASE, ctrl->dstride, ctrl->cqhead_adm);

    if (sc_sct != 0) return -1;
    if (out_dw0) *out_dw0 = *(const uint32_t *)cqe;
    return 0;
}

static int nvme_submit_io_cmd(nvme_ctrl_t *ctrl, const uint8_t *sqe) {
    uint8_t *sq_slot = (uint8_t *)(ctrl->iosq + ctrl->sqtail_io * NVME_SQ_ENTRY);
    for (uint32_t i = 0; i < NVME_SQ_ENTRY; ++i) sq_slot[i] = sqe[i];

    uint16_t cid = (uint16_t)ctrl->next_cid;
    *(uint16_t *)(sq_slot + NVME_SQE_CID) = cid;
    ++ctrl->next_cid;

    io_fence();

    ctrl->sqtail_io = (ctrl->sqtail_io + 1 >= NVME_IO_DEPTH) ? 0 : ctrl->sqtail_io + 1;
    mmio_write32(ctrl->base + NVME_DB_BASE, ctrl->dstride * 2u, ctrl->sqtail_io);

    const uint8_t *cqe = (const uint8_t *)(ctrl->iocq + ctrl->cqhead_io * NVME_CQ_ENTRY);
    uint32_t spins = 5000000u;
    for (;;) {
        if (spins-- == 0) return -1;
        uint16_t status = *(const uint16_t *)(cqe + NVME_CQE_STATUS);
        if ((status & 1u) == ctrl->phase_io) break;
    }

    uint16_t status = *(const uint16_t *)(cqe + NVME_CQE_STATUS);
    uint32_t sc_sct = (status >> 1) & 0x7FFu;

    ctrl->cqhead_io = ctrl->cqhead_io + 1;
    if (ctrl->cqhead_io >= NVME_IO_DEPTH) {
        ctrl->cqhead_io = 0;
        ctrl->phase_io ^= 1u;
    }
    mmio_write32(ctrl->base + NVME_DB_BASE, ctrl->dstride * 2u + ctrl->dstride, ctrl->cqhead_io);

    return (sc_sct == 0) ? 0 : -1;
}

static int nvme_identify_controller(nvme_ctrl_t *ctrl) {
    uint32_t numa;
    uint32_t page = pmm_alloc_page(&numa);
    if (page == 0) return -1;
    ctrl->ident_page = page;

    uint8_t sqe[NVME_SQ_ENTRY] = {0};
    sqe[NVME_SQE_OPCODE] = NVME_ADM_IDENTIFY;
    *(uint32_t *)(sqe + NVME_SQE_NSID) = 0;
    *(uint32_t *)(sqe + NVME_SQE_PRP1_LO) = page;
    *(uint32_t *)(sqe + NVME_SQE_PRP1_HI) = 0;
    *(uint32_t *)(sqe + NVME_SQE_CDW10) = 1; /* CNS=1 Identify Controller */

    if (nvme_submit_admin_cmd(ctrl, sqe, 0) != 0) return -1;

    uint32_t ns_count = *(const uint32_t *)((const uint8_t *)page + NVME_ID_CTL_NN);
    if (ns_count == 0) return -1;
    ctrl->ns_count = ns_count;
    return 0;
}

static int nvme_create_io_queues(nvme_ctrl_t *ctrl) {
    uint32_t numa;
    uint32_t iocq_page = pmm_alloc_page(&numa);
    if (iocq_page == 0) return -1;
    ctrl->iocq = iocq_page;
    for (uint32_t i = 0; i < PMM_PAGE_SIZE; ++i) ((uint8_t *)iocq_page)[i] = 0;

    uint8_t sqe[NVME_SQ_ENTRY] = {0};
    sqe[NVME_SQE_OPCODE] = NVME_ADM_CREATE_CQ;
    *(uint32_t *)(sqe + NVME_SQE_PRP1_LO) = iocq_page;
    *(uint32_t *)(sqe + NVME_SQE_CDW10) = ((NVME_IO_DEPTH - 1u) << 16) | 1u;
    *(uint32_t *)(sqe + NVME_SQE_CDW11) = 0x00000001u; /* PC=1 */
    if (nvme_submit_admin_cmd(ctrl, sqe, 0) != 0) return -1;

    uint32_t iosq_page = pmm_alloc_page(&numa);
    if (iosq_page == 0) return -1;
    ctrl->iosq = iosq_page;
    for (uint32_t i = 0; i < PMM_PAGE_SIZE; ++i) ((uint8_t *)iosq_page)[i] = 0;

    for (uint32_t i = 0; i < NVME_SQ_ENTRY; ++i) sqe[i] = 0;
    sqe[NVME_SQE_OPCODE] = NVME_ADM_CREATE_SQ;
    *(uint32_t *)(sqe + NVME_SQE_PRP1_LO) = iosq_page;
    *(uint32_t *)(sqe + NVME_SQE_CDW10) = ((NVME_IO_DEPTH - 1u) << 16) | 1u;
    *(uint32_t *)(sqe + NVME_SQE_CDW11) = 0x00010001u; /* CQID=1, PC=1 */
    if (nvme_submit_admin_cmd(ctrl, sqe, 0) != 0) return -1;

    ctrl->sqtail_io = 0;
    ctrl->cqhead_io = 0;
    ctrl->phase_io = 1;
    return 0;
}

static int nvme_blk_read(void *record, uint32_t lba_lo, uint32_t lba_hi, uint32_t count, void *buffer) {
    nvme_ctrl_t *ctrl = (nvme_ctrl_t *)block_record_driver_handle(record);
    if (!ctrl) return -1;

    uint8_t sqe[NVME_SQ_ENTRY] = {0};
    sqe[NVME_SQE_OPCODE] = NVME_IO_READ;
    *(uint32_t *)(sqe + NVME_SQE_NSID) = 1;
    *(uint32_t *)(sqe + NVME_SQE_PRP1_LO) = (uint32_t)buffer;
    *(uint32_t *)(sqe + NVME_SQE_CDW10) = lba_lo;
    *(uint32_t *)(sqe + NVME_SQE_CDW11) = lba_hi;
    *(uint32_t *)(sqe + NVME_SQE_CDW12) = count - 1u; /* NLB = count - 1 */

    return nvme_submit_io_cmd(ctrl, sqe);
}

static int nvme_blk_write(void *record, uint32_t lba_lo, uint32_t lba_hi, uint32_t count, void *buffer) {
    nvme_ctrl_t *ctrl = (nvme_ctrl_t *)block_record_driver_handle(record);
    if (!ctrl) return -1;

    uint8_t sqe[NVME_SQ_ENTRY] = {0};
    sqe[NVME_SQE_OPCODE] = NVME_IO_WRITE;
    *(uint32_t *)(sqe + NVME_SQE_NSID) = 1;
    *(uint32_t *)(sqe + NVME_SQE_PRP1_LO) = (uint32_t)buffer;
    *(uint32_t *)(sqe + NVME_SQE_CDW10) = lba_lo;
    *(uint32_t *)(sqe + NVME_SQE_CDW11) = lba_hi;
    *(uint32_t *)(sqe + NVME_SQE_CDW12) = count - 1u;

    return nvme_submit_io_cmd(ctrl, sqe);
}

static int nvme_blk_flush(void *record) {
    nvme_ctrl_t *ctrl = (nvme_ctrl_t *)block_record_driver_handle(record);
    if (!ctrl) return -1;

    uint8_t sqe[NVME_SQ_ENTRY] = {0};
    sqe[NVME_SQE_OPCODE] = NVME_IO_FLUSH;
    *(uint32_t *)(sqe + NVME_SQE_NSID) = 1;

    return nvme_submit_io_cmd(ctrl, sqe);
}

static void nvme_namespace_discover(nvme_ctrl_t *ctrl) {
    uint8_t sqe[NVME_SQ_ENTRY] = {0};
    sqe[NVME_SQE_OPCODE] = NVME_ADM_IDENTIFY;
    *(uint32_t *)(sqe + NVME_SQE_NSID) = 1;

    uint8_t *ident = (uint8_t *)ctrl->ident_page;
    for (uint32_t i = 0; i < PMM_PAGE_SIZE; ++i) ident[i] = 0;
    *(uint32_t *)(sqe + NVME_SQE_PRP1_LO) = ctrl->ident_page;
    *(uint32_t *)(sqe + NVME_SQE_CDW10) = 0; /* CNS=0 Identify Namespace */

    if (nvme_submit_admin_cmd(ctrl, sqe, 0) != 0) return;

    uint32_t nsze_lo = *(const uint32_t *)(ident + NVME_ID_NS_NSZE);
    uint32_t nsze_hi = *(const uint32_t *)(ident + NVME_ID_NS_NSZE + 4);
    if (nsze_lo == 0) return;
    ctrl->ns1_size_lo = nsze_lo;
    ctrl->ns1_size_hi = nsze_hi;

    uint32_t lbaf0 = *(const uint32_t *)(ident + NVME_ID_NS_LBAF0);
    uint32_t lbads = (lbaf0 >> 16) & 0xFFu;
    uint32_t block_size = 1u << lbads;
    if (block_size < 512u) return;
    ctrl->ns1_bsize = block_size;

    block_register_params_t params = {
        .id = 0x4E564D45u, /* "NVME" */
        .flags = BLOCK_FLAG_FLUSH_CAP,
        .block_size = block_size,
        .block_count_lo = ctrl->ns1_size_lo,
        .block_count_hi = ctrl->ns1_size_hi,
        .transport = BLOCK_TRANSPORT_NVME,
        .driver_handle = (uint32_t)ctrl,
    };
    void *record = 0;
    if (block_device_register(&params, nvme_blk_read, nvme_blk_write, nvme_blk_flush, 0, &record) != 0) {
        return;
    }
    ctrl->blkdev = record;
}

static int nvme_probe_controller(uint32_t pci_bus_addr, uint32_t bar0) {
    if (nvme_count >= NVME_CAPACITY) return -1;
    nvme_ctrl_t *ctrl = &nvme_controllers[nvme_count];

    driver_enable_bus_master(pci_bus_addr);

    if (bar0 == 0) {
        ctrl->state = NVME_STATE_FAILED;
        return -1;
    }
    ctrl->base = bar0;
    ctrl->pci_bus_addr = pci_bus_addr;
    ctrl->state = NVME_STATE_DISABLED;

    /* 8 Seiten MMIO mappen (32 KiB: Register + Doorbells). */
    for (uint32_t i = 0; i < 8u; ++i) {
        uint32_t page = bar0 + i * 4096u;
        if (paging_map_page(page, page, PAGING_PAGE_PRESENT | PAGING_PAGE_WRITE | 0x18u) != 0) {
            ctrl->state = NVME_STATE_FAILED;
            return -1;
        }
    }

    /* Schritt 1: Controller deaktivieren (CC.EN=0), auf CSTS.RDY=0 warten. */
    mmio_write32(ctrl->base, NVME_CC, mmio_read32(ctrl->base, NVME_CC) & ~NVME_CC_EN);
    uint32_t spins = 1000000u;
    for (;;) {
        if (spins-- == 0) { ctrl->state = NVME_STATE_FAILED; return -1; }
        if ((mmio_read32(ctrl->base, NVME_CSTS) & NVME_CSTS_RDY) == 0) break;
    }

    /* Schritt 2: Admin-Queue-Seiten allozieren. */
    uint32_t numa;
    uint32_t asq_page = pmm_alloc_page(&numa);
    if (asq_page == 0) { ctrl->state = NVME_STATE_FAILED; return -1; }
    ctrl->asq = asq_page;
    for (uint32_t i = 0; i < PMM_PAGE_SIZE; ++i) ((uint8_t *)asq_page)[i] = 0;

    uint32_t acq_page = pmm_alloc_page(&numa);
    if (acq_page == 0) { ctrl->state = NVME_STATE_FAILED; return -1; }
    ctrl->acq = acq_page;
    for (uint32_t i = 0; i < PMM_PAGE_SIZE; ++i) ((uint8_t *)acq_page)[i] = 0;

    /* Schritt 3: AQA, ASQ, ACQ programmieren. */
    mmio_write32(ctrl->base, NVME_AQA, ((NVME_ADMIN_DEPTH - 1u) << 16) | (NVME_ADMIN_DEPTH - 1u));
    mmio_write32(ctrl->base, NVME_ASQ_LO, ctrl->asq);
    mmio_write32(ctrl->base, NVME_ASQ_HI, 0);
    mmio_write32(ctrl->base, NVME_ACQ_LO, ctrl->acq);
    mmio_write32(ctrl->base, NVME_ACQ_HI, 0);

    /* Schritt 4: CC setzen und aktivieren. */
    mmio_write32(ctrl->base, NVME_CC, NVME_CC_DEFAULT | NVME_CC_EN);
    io_fence();

    /* Schritt 5: CSTS.RDY abwarten. */
    spins = 5000000u;
    for (;;) {
        if (spins-- == 0) { ctrl->state = NVME_STATE_FAILED; return -1; }
        uint32_t csts = mmio_read32(ctrl->base, NVME_CSTS);
        if (csts & NVME_CSTS_CFS) { ctrl->state = NVME_STATE_FAILED; return -1; }
        if (csts & NVME_CSTS_RDY) break;
    }
    ctrl->state = NVME_STATE_ADMINREADY;

    /* Schritt 6: Doorbell-Stride aus CAP ermitteln (Bits [55:52] -> CAP_HI[23:20]). */
    uint32_t cap_hi = mmio_read32(ctrl->base, NVME_CAP_HI);
    uint32_t dstrd = (cap_hi >> 20) & 0xFu;
    ctrl->dstride = 4u << dstrd;

    /* Schritt 7: Queue-Indizes initialisieren. */
    ctrl->sqtail_adm = 0;
    ctrl->cqhead_adm = 0;
    ctrl->phase_adm = 1;
    ctrl->next_cid = 1;

    /* Schritt 8: Identify Controller. */
    if (nvme_identify_controller(ctrl) != 0) { ctrl->state = NVME_STATE_FAILED; return -1; }
    ctrl->state = NVME_STATE_ADMINREADY;

    /* Schritt 9: I/O-Queues anlegen. */
    if (nvme_create_io_queues(ctrl) != 0) { ctrl->state = NVME_STATE_FAILED; return -1; }

    /* Schritt 10: Namespace 1 enumerieren (Fehler hier sind nicht fatal). */
    nvme_namespace_discover(ctrl);

    ctrl->state = NVME_STATE_RUNNING;
    ++nvme_count;
    return 0;
}

int nvme_initialize(void) {
    nvme_count = 0;
    for (uint32_t i = 0; i < NVME_CAPACITY; ++i) {
        nvme_controllers[i] = (nvme_ctrl_t){0};
    }
    nvme_mgr_ready = 1;

    for (uint32_t index = 0; nvme_count < NVME_CAPACITY; ++index) {
        uint32_t bus_addr = 0, bar0 = 0;
        if (disc_find_matched(PCI_CLASS_NVME, index, &bus_addr, &bar0) != 0) {
            break;
        }
        nvme_probe_controller(bus_addr, bar0);
    }

    return 0;
}

int nvme_self_test(void) {
    if (nvme_mgr_ready != 1) return -1;

    if (nvme_count == 0) return 0; /* kein NVMe - kein Fehler (NPSPEC) */

    if (nvme_controllers[0].state != NVME_STATE_RUNNING) return -1;
    if (nvme_controllers[0].ns1_size_lo == 0) return -1;

    return 0;
}
