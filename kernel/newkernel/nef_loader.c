/* NovaOS C-Kernel - NEF 1.0 Nativer Loader (Phase 4, siehe nef_loader.h
   fuer Scope/Architektur-Begruendung). */
#include "nef_loader.h"
#include "nef.h"
#include "vfsapi.h"
#include "pmm.h"
#include "paging.h"
#include "process.h"
#include "scheduler.h"
#include "ring3.h"
#include "security.h"
#include "kalloc.h"
#include "novafs.h"

static int set_err(int *out_err, int code) {
    if (out_err) *out_err = code;
    return code;
}

static uint32_t cstr_len(const char *s) {
    uint32_t n = 0;
    while (s[n] != '\0') ++n;
    return n;
}

int nef_load_and_spawn(uint32_t loader_pid, const char *path, uint32_t path_len,
                        nef_process_t *out_proc, int *out_err, int *out_validate_err) {
    if (out_proc) {
        out_proc->pid = 0u;
        out_proc->directory_phys = 0u;
        out_proc->scheduler_slot = 0xFFFFFFFFu;
        out_proc->page_count = 0u;
    }
    if (out_proc == 0) return set_err(out_err, NEF_LOAD_ERR_OPEN);

    uint32_t handle;
    if (vfsapi_lookup(loader_pid, 0u, path, path_len, 0u, &handle) != 0) {
        return set_err(out_err, NEF_LOAD_ERR_OPEN);
    }

    vfsapi_info_t info;
    if (vfsapi_query(loader_pid, handle, &info) != 0) {
        vfsapi_close(loader_pid, handle);
        return set_err(out_err, NEF_LOAD_ERR_QUERY);
    }
    if (info.size_hi != 0u || info.size_lo == 0u || info.size_lo > NEF_LOADER_MAX_FILE_SIZE) {
        vfsapi_close(loader_pid, handle);
        return set_err(out_err, NEF_LOAD_ERR_TOO_LARGE);
    }

    uint32_t file_size = info.size_lo;
    uint8_t *buf = (uint8_t *)kalloc(file_size);
    if (buf == 0) {
        vfsapi_close(loader_pid, handle);
        return set_err(out_err, NEF_LOAD_ERR_OOM);
    }

    uint32_t total_read = 0u;
    while (total_read < file_size) {
        uint32_t chunk = file_size - total_read;
        if (chunk > VFSAPI_IO_MAX) chunk = VFSAPI_IO_MAX;
        uint32_t transferred = 0u;
        if (vfsapi_read(loader_pid, handle, total_read, buf + total_read, chunk, &transferred) != 0 ||
            transferred == 0u) {
            kfree(buf);
            vfsapi_close(loader_pid, handle);
            return set_err(out_err, NEF_LOAD_ERR_READ);
        }
        total_read += transferred;
    }
    vfsapi_close(loader_pid, handle);

    nef_header_t hdr;
    int verr = NEF_VALIDATE_OK;
    if (nef_validate_header(buf, file_size, &hdr, &verr) != 0) {
        kfree(buf);
        if (out_validate_err) *out_validate_err = verr;
        return set_err(out_err, NEF_LOAD_ERR_VALIDATE);
    }
    if (nef_header_version_supported(&hdr) == 0) {
        kfree(buf);
        return set_err(out_err, NEF_LOAD_ERR_VERSION);
    }
    if ((hdr.format_flags & NEF_FLAG_EXECUTABLE) == 0u) {
        kfree(buf);
        return set_err(out_err, NEF_LOAD_ERR_NOT_EXECUTABLE);
    }
    /* Nur X86_32/NOVA_NATIVE hat in diesem Kernel einen echten
       Ausfuehrungspfad - siehe nef.h Dateikopf. Eine Datei, deren
       einzige Architekturvariante etwas anderes ist, wird kontrolliert
       abgelehnt, NIEMALS so behandelt, als liefe sie. */
    if (hdr.target_arch != NEF_ARCH_X86_32) {
        kfree(buf);
        return set_err(out_err, NEF_LOAD_ERR_ARCH);
    }
    if (hdr.abi_id != NEF_ABI_NOVA_NATIVE) {
        kfree(buf);
        return set_err(out_err, NEF_LOAD_ERR_ABI);
    }
    if (hdr.section_count > NEF_LOADER_MAX_SECTIONS) {
        kfree(buf);
        return set_err(out_err, NEF_LOAD_ERR_TOO_MANY_SECTIONS);
    }

    nef_section_entry_t secs[NEF_LOADER_MAX_SECTIONS];
    for (uint32_t i = 0; i < hdr.section_count; ++i) {
        uint64_t off = hdr.section_dir_offset + (uint64_t)i * NEF_SECTION_ENTRY_SIZE;
        nef_section_entry_parse(buf + (uint32_t)off, &secs[i]);
        uint32_t type = secs[i].type;
        /* Ehrliche Ablehnung statt Vortaeuschung: diese vier Section-
           Typen brauchen ein Eintragsformat (Symbole/Relokationen/TLS-
           Template) plus eine Modul-Registry fuer Importaufloesung -
           beides existiert in dieser Phase noch nicht (siehe
           nef_loader.h Dateikopf). Eine LEERE Section dieses Typs (z.B.
           ein Werkzeug, das vorsorglich einen Eintrag anlegt) ist
           harmlos und wird NICHT abgelehnt. */
        if ((type == NEF_SECTION_IMPORTS || type == NEF_SECTION_EXPORTS ||
             type == NEF_SECTION_RELOCATIONS || type == NEF_SECTION_TLS) &&
            (secs[i].file_size > 0u || secs[i].vsize > 0u)) {
            kfree(buf);
            return set_err(out_err, NEF_LOAD_ERR_UNSUPPORTED_DYNAMIC);
        }
    }

    for (uint32_t i = 0; i < hdr.extension_count; ++i) {
        uint64_t off = hdr.extension_dir_offset + (uint64_t)i * NEF_EXTENSION_ENTRY_SIZE;
        nef_extension_entry_t ext;
        nef_extension_entry_parse(buf + (uint32_t)off, &ext);
        /* Dieser Loader kennt (noch) KEINE Extension-Type-IDs (siehe
           nef.h: nur der RESERVED_NONE-Platzhalter existiert bisher) -
           unbekannt+required MUSS abgelehnt werden (siehe nef.h
           Kommentar zu nef_compat_resolve). */
        if (ext.required != 0u) {
            kfree(buf);
            return set_err(out_err, NEF_LOAD_ERR_EXTENSION);
        }
    }

    /* Gesamten Seitenbedarf (Sections + Stack) VOR jeder Allokation
       abschaetzen, damit ein zu grosses Image sauber abgelehnt wird,
       statt erst halb zu allozieren und dann wieder zurueckzurollen. */
    uint64_t needed_pages = 1u; /* Stack */
    uint64_t max_end = 0u;
    for (uint32_t i = 0; i < hdr.section_count; ++i) {
        if (secs[i].vaddr == 0u) continue;
        uint64_t pages = (secs[i].vsize + (uint64_t)PMM_PAGE_SIZE - 1u) / (uint64_t)PMM_PAGE_SIZE;
        needed_pages += pages;
        uint64_t end = secs[i].vaddr + pages * (uint64_t)PMM_PAGE_SIZE;
        if (end > max_end) max_end = end;
    }
    if (needed_pages > NEF_LOADER_MAX_PAGES) {
        kfree(buf);
        return set_err(out_err, NEF_LOAD_ERR_IMAGE_TOO_LARGE);
    }

    uint32_t dir_phys;
    if (paging_create_address_space(&dir_phys) != 0) {
        kfree(buf);
        return set_err(out_err, NEF_LOAD_ERR_ADDRESS_SPACE);
    }

    uint32_t pid = 0u;
    uint32_t slot = 0xFFFFFFFFu;
    int slot_claimed = 0;
    int err = NEF_LOAD_OK;
    uint32_t stack_vaddr = 0u;
    uint32_t stack_top = 0u;

    for (uint32_t i = 0; i < hdr.section_count && err == NEF_LOAD_OK; ++i) {
        if (secs[i].vaddr == 0u) continue;
        uint64_t page_count = (secs[i].vsize + (uint64_t)PMM_PAGE_SIZE - 1u) / (uint64_t)PMM_PAGE_SIZE;
        uint32_t page_flags = PAGING_PAGE_PRESENT | PAGING_PAGE_USER;
        /* NEF_SEC_FLAG_WRITE steuert die einzige hier tatsaechlich
           durchsetzbare Grenze (PAGING_PAGE_WRITE) - EXEC ist auf dieser
           32-Bit-Non-PAE-Paging-Implementierung mangels NX-Bit NICHT
           hardwaredurchsetzbar, siehe nef_loader.h Dateikopf. */
        if ((secs[i].flags & NEF_SEC_FLAG_WRITE) != 0u) page_flags |= PAGING_PAGE_WRITE;

        for (uint64_t p = 0; p < page_count; ++p) {
            if (out_proc->page_count >= NEF_LOADER_MAX_TRACKED_PAGES) {
                err = NEF_LOAD_ERR_IMAGE_TOO_LARGE;
                break;
            }
            uint32_t numa;
            uint32_t frame = pmm_alloc_page(&numa);
            if (frame == 0u) { err = NEF_LOAD_ERR_NO_MEMORY; break; }
            out_proc->pages[out_proc->page_count++] = frame;

            uint8_t *dst = (uint8_t *)(uintptr_t)frame;
            uint64_t page_off = p * (uint64_t)PMM_PAGE_SIZE;
            for (uint32_t b = 0; b < PMM_PAGE_SIZE; ++b) {
                uint64_t file_local = page_off + (uint64_t)b;
                dst[b] = (file_local < secs[i].file_size) ?
                         buf[(uint32_t)(secs[i].file_offset + file_local)] : 0u;
            }
            uint32_t vpage = (uint32_t)(secs[i].vaddr + page_off);
            if (paging_map_page_into(dir_phys, vpage, frame, page_flags) != 0) {
                err = NEF_LOAD_ERR_MAP;
                break;
            }
        }
    }

    if (err == NEF_LOAD_OK) {
        uint64_t aligned_end = (max_end + (uint64_t)PMM_PAGE_SIZE - 1u) &
                                ~((uint64_t)PMM_PAGE_SIZE - 1u);
        /* Eine Seite Luecke zwischen hoechster Section und Stack - rein
           kosmetisch (kein Guard-Page-Schutz, dieser Kernel hat kein
           Demand-Paging fuer einen Stack-Ueberlauf), aber verhindert,
           dass ein Image mit knapp passender vsize den Stack ueberlappt,
           falls eine kuenftige Phase vsize nicht mehr strikt
           seitenaufgerundet behandelt. */
        stack_vaddr = (uint32_t)(aligned_end + (uint64_t)PMM_PAGE_SIZE);
        uint32_t numa;
        uint32_t stack_page = pmm_alloc_page(&numa);
        if (stack_page == 0u) {
            err = NEF_LOAD_ERR_NO_MEMORY;
        } else {
            out_proc->pages[out_proc->page_count++] = stack_page;
            uint8_t *sdst = (uint8_t *)(uintptr_t)stack_page;
            for (uint32_t b = 0; b < PMM_PAGE_SIZE; ++b) sdst[b] = 0u;
            if (paging_map_page_into(dir_phys, stack_vaddr, stack_page,
                                      PAGING_PAGE_PRESENT | PAGING_PAGE_WRITE | PAGING_PAGE_USER) != 0) {
                err = NEF_LOAD_ERR_MAP;
            } else {
                stack_top = stack_vaddr + PMM_PAGE_SIZE - 16u;
            }
        }
    }

    if (err == NEF_LOAD_OK) {
        pid = process_create(0x4 /* PROCESS_FLAG_USERSPACE, siehe ring3.c/ushell_process.c */, dir_phys);
        if (pid == 0u) err = NEF_LOAD_ERR_PROCESS;
    }

    if (err == NEF_LOAD_OK) {
        uint32_t kstack_numa;
        uint32_t kstack_page = pmm_alloc_page(&kstack_numa);
        if (kstack_page == 0u) {
            err = NEF_LOAD_ERR_NO_MEMORY;
        } else {
            out_proc->pages[out_proc->page_count++] = kstack_page;
            uint8_t *kdst = (uint8_t *)(uintptr_t)kstack_page;
            for (uint32_t b = 0; b < PMM_PAGE_SIZE; ++b) kdst[b] = 0u;
            uint32_t kernel_stack_top = kstack_page + PMM_PAGE_SIZE - 16u;
            /* entry_point_ref ist per Konvention (siehe tools/nef/
               nef_main.c cmd_build) eine ABSOLUTE virtuelle Adresse
               innerhalb des neuen Adressraums, kein Dateioffset. */
            if (scheduler_claim_dynamic_user_slot((uint32_t)hdr.entry_point_ref, stack_top,
                                                   RING3_CODE_SELECTOR, RING3_DATA_SELECTOR,
                                                   -1, dir_phys, kernel_stack_top, &slot) != 0) {
                err = NEF_LOAD_ERR_SCHEDULER;
            } else {
                slot_claimed = 1;
            }
        }
    }

    kfree(buf);

    if (err != NEF_LOAD_OK) {
        if (slot_claimed) scheduler_release_dynamic_slot(slot);
        if (pid != 0u) { process_terminate(pid, 0xFFFFFFFFu); process_reap(pid); }
        for (uint32_t i = 0; i < out_proc->page_count; ++i) pmm_free_page(out_proc->pages[i]);
        out_proc->page_count = 0u;
        return set_err(out_err, err);
    }

    out_proc->pid = pid;
    out_proc->directory_phys = dir_phys;
    out_proc->scheduler_slot = slot;
    return set_err(out_err, NEF_LOAD_OK);
}

void nef_unload(nef_process_t *proc) {
    if (proc == 0 || proc->pid == 0u) return;

    process_terminate(proc->pid, 0u);
    process_reap(proc->pid);
    if (proc->scheduler_slot != 0xFFFFFFFFu) {
        scheduler_release_dynamic_slot(proc->scheduler_slot);
    }
    for (uint32_t i = 0; i < proc->page_count; ++i) pmm_free_page(proc->pages[i]);

    proc->pid = 0u;
    proc->directory_phys = 0u;
    proc->scheduler_slot = 0xFFFFFFFFu;
    proc->page_count = 0u;
}

/* ===================================================================== */
/* Selbsttest: echter Rundweg Datei->Laden->Spawnen->Pruefen->Abbauen.    */
/* ===================================================================== */

#define NEF_LOADER_SELFTEST_PATH "/nef_loader_selftest.bin"
#define NEF_LOADER_SELFTEST_NAME "nef_loader_selftest.bin"
#define NEF_LOADER_SELFTEST_VADDR 0x00500000u

int nef_loader_self_test(void) {
    uint8_t img[NEF_HEADER_SIZE + NEF_SECTION_ENTRY_SIZE + 16u];
    for (uint32_t i = 0; i < sizeof(img); ++i) img[i] = 0u;

    static const uint8_t payload[16] = {
        0x90u, 0x90u, 0x90u, 0x90u, 0xEBu, 0xFEu, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u
    };

    nef_header_t hdr;
    hdr.version_major = NEF_VERSION_MAJOR_CURRENT;
    hdr.version_minor = NEF_VERSION_MINOR_CURRENT;
    hdr.header_size = NEF_HEADER_SIZE;
    hdr.file_size = sizeof(img);
    hdr.format_flags = NEF_FLAG_EXECUTABLE;
    hdr.target_arch = NEF_ARCH_X86_32;
    hdr.abi_id = NEF_ABI_NOVA_NATIVE;
    hdr.min_os_version = 0u;
    hdr.entry_point_ref = NEF_LOADER_SELFTEST_VADDR;
    hdr.section_dir_offset = NEF_HEADER_SIZE;
    hdr.section_count = 1u;
    hdr.extension_dir_offset = NEF_HEADER_SIZE + NEF_SECTION_ENTRY_SIZE;
    hdr.extension_count = 0u;
    hdr.manifest_ref = 0u;
    hdr.signature_dir_ref = 0u;
    hdr.integrity_ref = 0u;
    if (nef_header_serialize(&hdr, img, sizeof(img)) != 0) return -1;

    nef_section_entry_t sec;
    sec.type = NEF_SECTION_CODE;
    sec.flags = NEF_SEC_FLAG_READ | NEF_SEC_FLAG_EXEC;
    sec.file_offset = NEF_HEADER_SIZE + NEF_SECTION_ENTRY_SIZE;
    sec.file_size = sizeof(payload);
    sec.vaddr = NEF_LOADER_SELFTEST_VADDR;
    sec.vsize = sizeof(payload);
    sec.alignment = 4096u;
    nef_section_entry_serialize(&sec, img + NEF_HEADER_SIZE);
    for (uint32_t i = 0; i < sizeof(payload); ++i) {
        img[(uint32_t)sec.file_offset + i] = payload[i];
    }

    /* Testdatei unter NovaFS-Wurzel anlegen (PID 1 = Kernel, hat
       SECURITY_KERNEL_CAPS inkl. FS_SYSTEM_WRITE, siehe security.h). */
    uint32_t root_handle;
    if (vfsapi_lookup(1u, 0u, "/", 1u, VFSAPI_LOOKUP_FLAG_WRITE, &root_handle) != 0) return -1;

    uint32_t file_handle;
    uint32_t name_len = cstr_len(NEF_LOADER_SELFTEST_NAME);
    int create_rc = vfsapi_create(1u, root_handle, NEF_LOADER_SELFTEST_NAME, name_len,
                                   NOVAFS_TYPE_FILE, &file_handle);
    if (create_rc != 0) {
        /* Von einem fruehen, abgebrochenen Lauf uebrig geblieben -
           existierende Testdatei per Lookup (mit Schreibrecht) oeffnen
           statt den Selbsttest daran scheitern zu lassen. */
        if (vfsapi_lookup(1u, root_handle, NEF_LOADER_SELFTEST_NAME, name_len,
                           VFSAPI_LOOKUP_FLAG_WRITE, &file_handle) != 0) {
            vfsapi_close(1u, root_handle);
            return -1;
        }
    }

    uint32_t written = 0u;
    int write_rc = vfsapi_write(1u, file_handle, 0u, img, sizeof(img), &written);
    vfsapi_close(1u, file_handle);
    vfsapi_close(1u, root_handle);
    if (write_rc != 0 || written != sizeof(img)) return -1;

    nef_process_t proc;
    int err = 0;
    int verr = 0;
    int rc = nef_load_and_spawn(1u, NEF_LOADER_SELFTEST_PATH,
                                 cstr_len(NEF_LOADER_SELFTEST_PATH), &proc, &err, &verr);
    if (rc != 0 || err != NEF_LOAD_OK || proc.pid == 0u) {
        vfsapi_lookup(1u, 0u, "/", 1u, VFSAPI_LOOKUP_FLAG_WRITE, &root_handle);
        vfsapi_delete(1u, root_handle, NEF_LOADER_SELFTEST_NAME, name_len);
        vfsapi_close(1u, root_handle);
        return -1;
    }

    int ok = 1;
    if (proc.directory_phys == 0u) ok = 0;
    if (process_get_cr3(proc.pid) != proc.directory_phys) ok = 0;
    if (proc.scheduler_slot == 0xFFFFFFFFu) ok = 0;
    if (ok && scheduler_slot_directory(proc.scheduler_slot) != proc.directory_phys) ok = 0;
    if (ok && scheduler_context_at(proc.scheduler_slot) == 0u) ok = 0;
    if (ok && proc.page_count == 0u) ok = 0;

    if (ok) {
        /* Kurzzeitig in den neuen Adressraum wechseln, um den
           tatsaechlich gemappten Code gegen die Quelle zu pruefen -
           dasselbe Verfahren wie ushell_process.c's
           ushell_instance_self_test(). Schaltet in JEDEM Fall zurueck. */
        uint32_t kernel_dir = paging_kernel_directory_physical();
        paging_switch_address_space(proc.directory_phys);
        const uint8_t *code = (const uint8_t *)(uintptr_t)NEF_LOADER_SELFTEST_VADDR;
        for (uint32_t i = 0; i < sizeof(payload); ++i) {
            if (code[i] != payload[i]) { ok = 0; break; }
        }
        paging_switch_address_space(kernel_dir);
    }

    nef_unload(&proc);

    if (vfsapi_lookup(1u, 0u, "/", 1u, VFSAPI_LOOKUP_FLAG_WRITE, &root_handle) == 0) {
        vfsapi_delete(1u, root_handle, NEF_LOADER_SELFTEST_NAME, name_len);
        vfsapi_close(1u, root_handle);
    }

    return ok ? 0 : -1;
}
