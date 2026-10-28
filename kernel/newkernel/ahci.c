/* NovaOS C-Kernel - AHCI SATA Storage Driver (Port aus storage32.inc, nur
   AHCI-/ATA-Teil - siehe ahci.h). */
#include "ahci.h"
#include "driver.h"
#include "paging.h"
#include "pmm.h"
#include "block.h"
#include "interrupt.h"

/* AHCI HBA-Register (ABAR-relativ). */
#define AHCI_CAP   0x00u
#define AHCI_GHC   0x04u
#define AHCI_IS    0x08u
#define AHCI_PI    0x0Cu
#define AHCI_CAP2  0x24u
#define AHCI_BOHC  0x28u

#define AHCI_GHC_HR 0x00000001u
#define AHCI_GHC_IE 0x00000002u
#define AHCI_GHC_AE 0x80000000u

/* Port-Register (ABAR + AHCI_PORT_BASE + port*AHCI_PORT_SIZE + Offset). */
#define AHCI_PORT_BASE 0x100u
#define AHCI_PORT_SIZE 0x80u
#define AHCI_PX_CLB  0x00u
#define AHCI_PX_CLBU 0x04u
#define AHCI_PX_FB   0x08u
#define AHCI_PX_FBU  0x0Cu
#define AHCI_PX_IS   0x10u
#define AHCI_PX_IE   0x14u
#define AHCI_PX_CMD  0x18u
#define AHCI_PX_TFD  0x20u
#define AHCI_PX_SIG  0x24u
#define AHCI_PX_SSTS 0x28u
#define AHCI_PX_SERR 0x30u
#define AHCI_PX_CI   0x38u

#define AHCI_PX_CMD_ST  0x00000001u
#define AHCI_PX_CMD_SUD 0x00000002u
#define AHCI_PX_CMD_POD 0x00000004u
#define AHCI_PX_CMD_FRE 0x00000010u
#define AHCI_PX_CMD_FR  0x00004000u
#define AHCI_PX_CMD_CR  0x00008000u
#define AHCI_PX_IS_TFES 0x40000000u

#define AHCI_TFD_ERR 0x01u
#define AHCI_TFD_DRQ 0x08u
#define AHCI_TFD_BSY 0x80u
#define AHCI_SIG_ATA 0x00000101u

#define ATA_CMD_IDENTIFY        0xECu
#define ATA_CMD_READ_DMA_EXT    0x25u
#define ATA_CMD_WRITE_DMA_EXT   0x35u
#define ATA_CMD_FLUSH_CACHE_EXT 0xEAu

#define AHCI_STATE_UNKNOWN 0u
#define AHCI_STATE_READY   1u
#define AHCI_STATE_FAILED  2u

#define AHCI_FLAG_LBA48 0x00000001u

#define AHCI_TIMEOUT_TICKS 300u /* 3s bei 100Hz, siehe interrupt.c */
#define AHCI_LINK_TICKS    10u
#define AHCI_SPIN_LIMIT    200000000u

typedef struct {
    uint32_t port;
    uint32_t port_mmio;
    uint32_t command_page;
    uint32_t sectors_lo, sectors_hi;
    uint32_t sector_size;
    uint32_t reads, writes, errors;
    uint32_t last_tfd;
    uint32_t flags;
    uint32_t state;
    void *blkdev;
} ahci_disk_t;

static ahci_disk_t ahci_disks[AHCI_DISK_CAPACITY];
static uint32_t ahci_disk_count = 0;
static uint32_t ahci_abar = 0;
static uint32_t ahci_present = 0;
static int ahci_mgr_ready = 0;

static inline uint32_t mmio_read32(uint32_t base, uint32_t offset) {
    return *(volatile uint32_t *)(base + offset);
}
static inline void mmio_write32(uint32_t base, uint32_t offset, uint32_t value) {
    *(volatile uint32_t *)(base + offset) = value;
}

/* Wartet, bis (*addr & mask) == expect, oder meldet -1 bei Timeout (sowohl
   Tick-basiert als auch durch ein Spin-Limit abgesichert, falls der
   Timer-Tick in diesem Kontext einmal stillstehen sollte). */
static int ahci_wait_mmio(uint32_t addr, uint32_t mask, uint32_t expect, uint32_t timeout_ticks) {
    uint32_t start = timer_get_ticks();
    uint32_t spins = AHCI_SPIN_LIMIT;
    for (;;) {
        if ((mmio_read32(addr, 0) & mask) == expect) return 0;
        if (spins-- == 0) return -1;
        if (timer_get_ticks() - start >= timeout_ticks) return -1;
        __asm__ volatile ("pause");
    }
}

static int ahci_port_stop_at(uint32_t port_mmio) {
    mmio_write32(port_mmio, AHCI_PX_CMD, mmio_read32(port_mmio, AHCI_PX_CMD) & ~AHCI_PX_CMD_ST);
    if (ahci_wait_mmio(port_mmio + AHCI_PX_CMD, AHCI_PX_CMD_CR, 0, 50u) != 0) return -1;
    mmio_write32(port_mmio, AHCI_PX_CMD, mmio_read32(port_mmio, AHCI_PX_CMD) & ~AHCI_PX_CMD_FRE);
    if (ahci_wait_mmio(port_mmio + AHCI_PX_CMD, AHCI_PX_CMD_FR, 0, 50u) != 0) return -1;
    return 0;
}

static int ahci_controller_reset(uint32_t abar) {
    if (mmio_read32(abar, AHCI_CAP2) & 1u) {
        mmio_write32(abar, AHCI_BOHC, mmio_read32(abar, AHCI_BOHC) | 0x02u);
        ahci_wait_mmio(abar + AHCI_BOHC, 0x01u, 0x01u, 200u);
    }
    mmio_write32(abar, AHCI_GHC, mmio_read32(abar, AHCI_GHC) | AHCI_GHC_AE);
    mmio_write32(abar, AHCI_GHC, mmio_read32(abar, AHCI_GHC) | AHCI_GHC_HR);
    if (ahci_wait_mmio(abar + AHCI_GHC, AHCI_GHC_HR, 0, 100u) != 0) return -1;
    mmio_write32(abar, AHCI_GHC, mmio_read32(abar, AHCI_GHC) | AHCI_GHC_AE);
    mmio_write32(abar, AHCI_GHC, mmio_read32(abar, AHCI_GHC) & ~AHCI_GHC_IE);
    mmio_write32(abar, AHCI_IS, 0xFFFFFFFFu);
    return 0;
}

/* Fuehrt genau ein synchrones Kommando in Slot 0 aus (Register H2D FIS,
   identisches Layout zum alten ASM). 0 = Erfolg. */
static int ahci_execute(ahci_disk_t *disk, uint8_t opcode, uint32_t lba_lo, uint32_t lba_hi,
                         uint32_t count, void *buffer, uint32_t bytes, int write) {
    uint32_t port_mmio = disk->port_mmio;
    if (mmio_read32(port_mmio, AHCI_PX_CI) & 1u) return -1;

    uint32_t page = disk->command_page;
    uint32_t cmd_table = page + 0x800u;

    uint32_t header0 = 5u | (1u << 16); /* CFL=5 DWORDs, PRDTL=1 */
    if (write) header0 |= 0x40u;
    *(volatile uint32_t *)(page + 0) = header0;
    *(volatile uint32_t *)(page + 4) = 0;
    *(volatile uint32_t *)(page + 8) = cmd_table;
    *(volatile uint32_t *)(page + 12) = 0;

    uint8_t *fis = (uint8_t *)cmd_table;
    for (uint32_t i = 0; i < 0x90u; ++i) fis[i] = 0;
    fis[0] = 0x27u; /* Register H2D FIS */
    fis[1] = 0x80u; /* C=1, PM port 0 */
    fis[2] = opcode;
    fis[4] = (uint8_t)(lba_lo);
    fis[5] = (uint8_t)(lba_lo >> 8);
    fis[6] = (uint8_t)(lba_lo >> 16);
    fis[7] = 0x40u; /* Device: LBA-Modus */
    fis[8] = (uint8_t)(lba_lo >> 24);
    fis[9] = (uint8_t)(lba_hi);
    fis[10] = (uint8_t)(lba_hi >> 8);
    fis[12] = (uint8_t)(count);
    fis[13] = (uint8_t)(count >> 8);

    if (bytes != 0) {
        *(volatile uint32_t *)(cmd_table + 0x80u) = (uint32_t)buffer;
        *(volatile uint32_t *)(cmd_table + 0x84u) = 0;
        *(volatile uint32_t *)(cmd_table + 0x88u) = 0;
        *(volatile uint32_t *)(cmd_table + 0x8Cu) = bytes - 1u;
    } else {
        *(volatile uint32_t *)(page + 0) = header0 & 0x0000FFFFu; /* PRDTL=0 */
    }

    mmio_write32(port_mmio, AHCI_PX_IS, 0xFFFFFFFFu);
    mmio_write32(port_mmio, AHCI_PX_CI, 1u);

    uint32_t start = timer_get_ticks();
    uint32_t spins = AHCI_SPIN_LIMIT;
    int task_error = 0;
    for (;;) {
        if (mmio_read32(port_mmio, AHCI_PX_IS) & AHCI_PX_IS_TFES) { task_error = 1; break; }
        if ((mmio_read32(port_mmio, AHCI_PX_CI) & 1u) == 0) break;
        if (spins-- == 0) { task_error = 1; break; }
        if (timer_get_ticks() - start >= AHCI_TIMEOUT_TICKS) { task_error = 1; break; }
        __asm__ volatile ("pause");
    }

    uint32_t tfd = mmio_read32(port_mmio, AHCI_PX_TFD);
    disk->last_tfd = tfd;
    if (!task_error && (tfd & (AHCI_TFD_ERR | AHCI_TFD_BSY | AHCI_TFD_DRQ))) task_error = 1;

    if (task_error) {
        ++disk->errors;
        ahci_port_stop_at(port_mmio);
        mmio_write32(port_mmio, AHCI_PX_SERR, 0xFFFFFFFFu);
        mmio_write32(port_mmio, AHCI_PX_IS, 0xFFFFFFFFu);
        mmio_write32(port_mmio, AHCI_PX_CMD, mmio_read32(port_mmio, AHCI_PX_CMD) | AHCI_PX_CMD_FRE);
        mmio_write32(port_mmio, AHCI_PX_CMD, mmio_read32(port_mmio, AHCI_PX_CMD) | AHCI_PX_CMD_ST);
        return -1;
    }
    return 0;
}

static int ahci_blk_read(void *record, uint32_t lba_lo, uint32_t lba_hi, uint32_t count, void *buffer) {
    ahci_disk_t *disk = (ahci_disk_t *)block_record_driver_handle(record);
    if (!disk) return -1;
    if (ahci_execute(disk, ATA_CMD_READ_DMA_EXT, lba_lo, lba_hi, count, buffer, count * disk->sector_size, 0) != 0) {
        return -1;
    }
    ++disk->reads;
    return 0;
}

static int ahci_blk_write(void *record, uint32_t lba_lo, uint32_t lba_hi, uint32_t count, void *buffer) {
    ahci_disk_t *disk = (ahci_disk_t *)block_record_driver_handle(record);
    if (!disk) return -1;
    if (ahci_execute(disk, ATA_CMD_WRITE_DMA_EXT, lba_lo, lba_hi, count, buffer, count * disk->sector_size, 1) != 0) {
        return -1;
    }
    ++disk->writes;
    return 0;
}

static int ahci_blk_flush(void *record) {
    ahci_disk_t *disk = (ahci_disk_t *)block_record_driver_handle(record);
    if (!disk) return -1;
    return ahci_execute(disk, ATA_CMD_FLUSH_CACHE_EXT, 0, 0, 0, 0, 0, 0);
}

static int ahci_all_links_up(uint32_t ports_implemented) {
    for (uint32_t i = 0; i < 32u; ++i) {
        if (!(ports_implemented & (1u << i))) continue;
        uint32_t port_mmio = ahci_abar + AHCI_PORT_BASE + i * AHCI_PORT_SIZE;
        if ((mmio_read32(port_mmio, AHCI_PX_SSTS) & 0x0Fu) != 3u) return -1;
    }
    return 0;
}

static void ahci_port_attach(uint32_t port) {
    uint32_t port_mmio = ahci_abar + AHCI_PORT_BASE + port * AHCI_PORT_SIZE;
    if ((mmio_read32(port_mmio, AHCI_PX_SSTS) & 0x0Fu) != 3u) return;
    if (ahci_port_stop_at(port_mmio) != 0) return;

    uint32_t numa;
    uint32_t page = pmm_alloc_page(&numa);
    if (page == 0) return;
    for (uint32_t i = 0; i < PMM_PAGE_SIZE / 4u; ++i) ((uint32_t *)page)[i] = 0;

    mmio_write32(port_mmio, AHCI_PX_CLB, page);
    mmio_write32(port_mmio, AHCI_PX_CLBU, 0);
    mmio_write32(port_mmio, AHCI_PX_FB, page + 0x400u);
    mmio_write32(port_mmio, AHCI_PX_FBU, 0);
    mmio_write32(port_mmio, AHCI_PX_SERR, 0xFFFFFFFFu);
    mmio_write32(port_mmio, AHCI_PX_IS, 0xFFFFFFFFu);
    mmio_write32(port_mmio, AHCI_PX_CMD, mmio_read32(port_mmio, AHCI_PX_CMD) | AHCI_PX_CMD_FRE);

    if (ahci_wait_mmio(port_mmio + AHCI_PX_TFD, AHCI_TFD_BSY | AHCI_TFD_DRQ, 0, AHCI_TIMEOUT_TICKS) != 0) {
        goto release;
    }
    if (mmio_read32(port_mmio, AHCI_PX_SIG) != AHCI_SIG_ATA) goto release;
    mmio_write32(port_mmio, AHCI_PX_CMD, mmio_read32(port_mmio, AHCI_PX_CMD) | AHCI_PX_CMD_ST);

    if (ahci_disk_count >= AHCI_DISK_CAPACITY) goto release;
    ahci_disk_t *disk = &ahci_disks[ahci_disk_count];
    disk->port = port;
    disk->port_mmio = port_mmio;
    disk->command_page = page;
    disk->sector_size = 512;
    disk->state = AHCI_STATE_UNKNOWN;

    uint32_t ident_page = pmm_alloc_page(&numa);
    if (ident_page == 0) goto release;
    for (uint32_t i = 0; i < PMM_PAGE_SIZE / 4u; ++i) ((uint32_t *)ident_page)[i] = 0;

    if (ahci_execute(disk, ATA_CMD_IDENTIFY, 0, 0, 0, (void *)ident_page, 512, 0) != 0) {
        pmm_free_page(ident_page);
        goto release;
    }

    const uint16_t *ident = (const uint16_t *)ident_page;
    if (!(ident[83] & 0x0400u)) { pmm_free_page(ident_page); goto release; } /* kein LBA48 */
    /* Wort 106: Bit14 gesetzt heisst "Wort gueltig"; nur dann zaehlt Bit12
       (logische Sektorgroesse > 256 Woerter, d.h. != 512 Byte) - nicht
       unterstuetzt in diesem Bootstrap-Treiber. */
    uint16_t word106 = ident[106];
    if ((word106 & 0x4000u) != 0 && (word106 & 0x1000u) != 0) {
        pmm_free_page(ident_page);
        goto release;
    }

    uint32_t sectors_lo = ((const uint32_t *)ident_page)[100 / 2];
    uint32_t sectors_hi = ((const uint32_t *)ident_page)[102 / 2];
    pmm_free_page(ident_page);
    if (sectors_lo == 0 && sectors_hi == 0) goto release;

    disk->sectors_lo = sectors_lo;
    disk->sectors_hi = sectors_hi;
    disk->flags |= AHCI_FLAG_LBA48;
    disk->state = AHCI_STATE_READY;

    block_register_params_t params = {
        .id = 0x41484349u, /* "AHCI" */
        .flags = BLOCK_FLAG_FLUSH_CAP,
        .block_size = disk->sector_size,
        .block_count_lo = disk->sectors_lo,
        .block_count_hi = disk->sectors_hi,
        .transport = BLOCK_TRANSPORT_AHCI,
        .driver_handle = (uint32_t)disk,
    };
    void *record = 0;
    if (block_device_register(&params, ahci_blk_read, ahci_blk_write, ahci_blk_flush, 0, &record) != 0) {
        disk->state = AHCI_STATE_FAILED;
        return;
    }
    disk->blkdev = record;
    ++ahci_disk_count;
    return;

release:
    ahci_port_stop_at(port_mmio);
    pmm_free_page(page);
}

static void ahci_probe_ports(void) {
    uint32_t ports_implemented = mmio_read32(ahci_abar, AHCI_PI);

    for (uint32_t i = 0; i < 32u; ++i) {
        if (!(ports_implemented & (1u << i))) continue;
        uint32_t port_mmio = ahci_abar + AHCI_PORT_BASE + i * AHCI_PORT_SIZE;
        mmio_write32(port_mmio, AHCI_PX_SERR, 0xFFFFFFFFu);
        mmio_write32(port_mmio, AHCI_PX_IS, 0xFFFFFFFFu);
        mmio_write32(port_mmio, AHCI_PX_IE, 0);
        mmio_write32(port_mmio, AHCI_PX_CMD,
                     mmio_read32(port_mmio, AHCI_PX_CMD) | AHCI_PX_CMD_SUD | AHCI_PX_CMD_POD);
    }

    /* Gemeinsames Link-Fenster: endet frueh, wenn alle Ports DET=3 melden. */
    uint32_t start = timer_get_ticks();
    uint32_t spins = AHCI_SPIN_LIMIT;
    for (;;) {
        if (ahci_all_links_up(ports_implemented) == 0) break;
        if (spins-- == 0) break;
        if (timer_get_ticks() - start >= AHCI_LINK_TICKS) break;
        __asm__ volatile ("pause");
    }

    for (uint32_t i = 0; i < 32u && ahci_disk_count < AHCI_DISK_CAPACITY; ++i) {
        if (!(ports_implemented & (1u << i))) continue;
        ahci_port_attach(i);
    }
}

int ahci_initialize(void) {
    ahci_disk_count = 0;
    ahci_abar = 0;
    ahci_present = 0;
    for (uint32_t i = 0; i < AHCI_DISK_CAPACITY; ++i) ahci_disks[i] = (ahci_disk_t){0};
    ahci_mgr_ready = 1;

    uint32_t bus_addr = 0, abar = 0;
    if (disc_find_matched(PCI_CLASS_AHCI, 0, &bus_addr, &abar) != 0) {
        return 0; /* kein Controller - kein Fehler (NPSPEC) */
    }
    if (abar == 0) return 0;

    driver_enable_bus_master(bus_addr);

    /* ABAR (0x1100 Byte) identitaetsabbilden, ungecacht (PCD+PWT, wie im
       alten ASM: Flags-Bits 0x18). */
    for (uint32_t i = 0; i < 2u; ++i) {
        uint32_t page = abar + i * PMM_PAGE_SIZE;
        if (paging_map_page(page, page, PAGING_PAGE_PRESENT | PAGING_PAGE_WRITE | 0x18u) != 0) {
            return 0;
        }
    }
    ahci_abar = abar;
    ahci_present = 1;

    if (ahci_controller_reset(ahci_abar) != 0) {
        ahci_present = 0;
        return 0;
    }

    ahci_probe_ports();
    return 0;
}

uint32_t ahci_disk_count_value(void) {
    return ahci_disk_count;
}

uint32_t ahci_disk_sectors_lo(uint32_t index) {
    if (index >= ahci_disk_count) return 0;
    return ahci_disks[index].sectors_lo;
}

int ahci_self_test(void) {
    if (ahci_mgr_ready != 1) return -1;
    if (ahci_disk_count == 0) return 0; /* kein Datentraeger - kein Fehler */

    uint32_t numa;
    uint32_t page = pmm_alloc_page(&numa);
    if (page == 0) return -1;

    void *record = ahci_disks[0].blkdev;
    int rc = block_io_submit(record, 0, 0, 1, (void *)page, BLK_IO_READ);
    if (rc != 0) { pmm_free_page(page); return -1; }

    /* Ein Bereich jenseits der Kapazitaet muss abgewiesen werden. */
    uint32_t beyond = ahci_disks[0].sectors_lo; /* = erste ungueltige LBA */
    if (ahci_disks[0].sectors_hi == 0) {
        rc = block_io_submit(record, beyond, 0, 1, (void *)page, BLK_IO_READ);
        if (rc == 0) { pmm_free_page(page); return -1; }
    }

    pmm_free_page(page);
    return 0;
}
