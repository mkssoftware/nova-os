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

/* Mappt jede Section mit vaddr!=0 aus file_buf (file_len Byte) in
   dir_phys, kopiert die Datei-Bytes (BSS-Wachstum nullgefuellt), haengt
   jeden allozierten Rahmen an out_proc->pages[] an (fuer nef_unload()
   bzw. den Fehlerpfad) und aktualisiert *inout_max_end mit dem
   hoechsten belegten Adressende. Wird fuer das Executable UND (Phase 5)
   fuer eine nachgemappte Bibliothek gleichermassen verwendet - EIN Satz
   Mapping-Logik statt Code-Duplikation. 0 = Erfolg, sonst ein
   NEF_LOAD_ERR_*-Code (Ressourcen bis zum Fehlerpunkt bleiben in
   out_proc->pages[] stehen - der Aufrufer rollt im Gesamtfehlerpfad
   zurueck, siehe nef_load_and_spawn()). */
static int map_sections(uint32_t dir_phys, const uint8_t *file_buf,
                         const nef_section_entry_t *secs, uint32_t count,
                         nef_process_t *out_proc, uint64_t *inout_max_end) {
    for (uint32_t i = 0; i < count; ++i) {
        if (secs[i].vaddr == 0u) continue;
        uint64_t page_count = (secs[i].vsize + (uint64_t)PMM_PAGE_SIZE - 1u) / (uint64_t)PMM_PAGE_SIZE;
        uint32_t page_flags = PAGING_PAGE_PRESENT | PAGING_PAGE_USER;
        /* NEF_SEC_FLAG_WRITE steuert die einzige hier tatsaechlich
           durchsetzbare Grenze (PAGING_PAGE_WRITE) - EXEC ist auf dieser
           32-Bit-Non-PAE-Paging-Implementierung mangels NX-Bit NICHT
           hardwaredurchsetzbar, siehe nef_loader.h Dateikopf. */
        if ((secs[i].flags & NEF_SEC_FLAG_WRITE) != 0u) page_flags |= PAGING_PAGE_WRITE;

        for (uint64_t p = 0; p < page_count; ++p) {
            if (out_proc->page_count >= NEF_LOADER_MAX_TRACKED_PAGES) return NEF_LOAD_ERR_IMAGE_TOO_LARGE;
            uint32_t numa;
            uint32_t frame = pmm_alloc_page(&numa);
            if (frame == 0u) return NEF_LOAD_ERR_NO_MEMORY;
            out_proc->pages[out_proc->page_count++] = frame;

            uint8_t *dst = (uint8_t *)(uintptr_t)frame;
            uint64_t page_off = p * (uint64_t)PMM_PAGE_SIZE;
            for (uint32_t b = 0; b < PMM_PAGE_SIZE; ++b) {
                uint64_t file_local = page_off + (uint64_t)b;
                dst[b] = (file_local < secs[i].file_size) ?
                         file_buf[(uint32_t)(secs[i].file_offset + file_local)] : 0u;
            }
            uint32_t vpage = (uint32_t)(secs[i].vaddr + page_off);
            if (paging_map_page_into(dir_phys, vpage, frame, page_flags) != 0) return NEF_LOAD_ERR_MAP;
        }

        uint64_t end = secs[i].vaddr + page_count * (uint64_t)PMM_PAGE_SIZE;
        if (end > *inout_max_end) *inout_max_end = end;
    }
    return NEF_LOAD_OK;
}

/* Schreibt value (little-endian u32) an iat_vaddr IM Adressraum
   dir_phys - findet den tatsaechlich dahinterliegenden physischen Rahmen
   per paging_query_page_in() (SMP-sicher, unabhaengig vom aktuell per
   CR3 aktiven Adressraum) statt per paging_switch_address_space() kurz
   umzuschalten. 0 = Erfolg, -1 wenn die Adresse nicht gemappt ist oder
   der 4-Byte-Slot eine Seitengrenze ueberschreiten wuerde. */
static int patch_iat_slot(uint32_t dir_phys, uint32_t iat_vaddr, uint32_t value) {
    uint32_t page = iat_vaddr & ~(PMM_PAGE_SIZE - 1u);
    uint32_t offset_in_page = iat_vaddr & (PMM_PAGE_SIZE - 1u);
    if (offset_in_page + 4u > PMM_PAGE_SIZE) return -1;

    uint32_t phys, flags;
    if (paging_query_page_in(dir_phys, page, &phys, &flags) != 0) return -1;

    uint8_t *dst = (uint8_t *)(uintptr_t)(phys + offset_in_page);
    dst[0] = (uint8_t)(value & 0xFFu);
    dst[1] = (uint8_t)((value >> 8) & 0xFFu);
    dst[2] = (uint8_t)((value >> 16) & 0xFFu);
    dst[3] = (uint8_t)((value >> 24) & 0xFFu);
    return 0;
}

/* Findet die EINE Section vom gegebenen Typ mit passender Groesse
   (file_size muss ein Vielfaches von entry_size sein, wenn entry_size
   != 0; bei entry_size == 0 ist jede file_size zulaessig - genutzt fuer
   die exakt 64 Byte grosse MANIFEST-Section). 0 = gefunden (*out
   gesetzt), -1 sonst. */
static int find_section(const nef_section_entry_t *secs, uint32_t count, uint32_t type,
                         uint32_t entry_size, nef_section_entry_t *out) {
    for (uint32_t i = 0; i < count; ++i) {
        if (secs[i].type != type) continue;
        if (entry_size != 0u && (secs[i].file_size % entry_size) != 0u) continue;
        *out = secs[i];
        return 0;
    }
    return -1;
}

/* Oeffnet+liest+validiert eine NEF-Datei VOLLSTAENDIG (Struktur+Version+
   Architektur+ABI) - gemeinsame Pipeline fuer das Haupt-Executable UND
   (Phase 5) fuer eine nachgemappte Bibliothek, statt sie zweimal
   auszuschreiben. *out_buf ist bei Erfolg per kalloc() alloziert (MUSS
   vom Aufrufer per kfree() freigegeben werden) und bei JEDEM Fehler
   bereits NULL (kein halb-alloziertes Teilergebnis haengen lassen).
   Parst zusaetzlich das Section-Directory nach out_secs (Kapazitaet
   max_secs) - ueberschreitet section_count max_secs, ist das
   NEF_LOAD_ERR_TOO_MANY_SECTIONS. */
static int open_validate_nef(uint32_t loader_pid, const char *path, uint32_t path_len,
                              uint8_t **out_buf, uint32_t *out_file_size, nef_header_t *out_hdr,
                              nef_section_entry_t *out_secs, uint32_t max_secs,
                              int *out_validate_err) {
    *out_buf = 0;

    uint32_t handle;
    if (vfsapi_lookup(loader_pid, 0u, path, path_len, 0u, &handle) != 0) return NEF_LOAD_ERR_OPEN;

    vfsapi_info_t info;
    if (vfsapi_query(loader_pid, handle, &info) != 0) {
        vfsapi_close(loader_pid, handle);
        return NEF_LOAD_ERR_QUERY;
    }
    if (info.size_hi != 0u || info.size_lo == 0u || info.size_lo > NEF_LOADER_MAX_FILE_SIZE) {
        vfsapi_close(loader_pid, handle);
        return NEF_LOAD_ERR_TOO_LARGE;
    }

    uint32_t file_size = info.size_lo;
    uint8_t *buf = (uint8_t *)kalloc(file_size);
    if (buf == 0) {
        vfsapi_close(loader_pid, handle);
        return NEF_LOAD_ERR_OOM;
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
            return NEF_LOAD_ERR_READ;
        }
        total_read += transferred;
    }
    vfsapi_close(loader_pid, handle);

    int verr = NEF_VALIDATE_OK;
    if (nef_validate_header(buf, file_size, out_hdr, &verr) != 0) {
        kfree(buf);
        if (out_validate_err) *out_validate_err = verr;
        return NEF_LOAD_ERR_VALIDATE;
    }
    if (nef_header_version_supported(out_hdr) == 0) {
        kfree(buf);
        return NEF_LOAD_ERR_VERSION;
    }
    if (out_hdr->target_arch != NEF_ARCH_X86_32) {
        /* Siehe nef.h Dateikopf: NUR X86_32 hat in diesem Kernel einen
           echten Ausfuehrungspfad - eine Datei mit anderer Zielarch
           wird kontrolliert abgelehnt, NIEMALS so behandelt, als liefe
           sie. */
        kfree(buf);
        return NEF_LOAD_ERR_ARCH;
    }
    if (out_hdr->abi_id != NEF_ABI_NOVA_NATIVE) {
        kfree(buf);
        return NEF_LOAD_ERR_ABI;
    }
    if (out_hdr->section_count > max_secs) {
        kfree(buf);
        return NEF_LOAD_ERR_TOO_MANY_SECTIONS;
    }

    for (uint32_t i = 0; i < out_hdr->section_count; ++i) {
        uint64_t off = out_hdr->section_dir_offset + (uint64_t)i * NEF_SECTION_ENTRY_SIZE;
        nef_section_entry_parse(buf + (uint32_t)off, &out_secs[i]);
    }

    *out_buf = buf;
    *out_file_size = file_size;
    return NEF_LOAD_OK;
}

int nef_load_and_spawn(uint32_t loader_pid, const char *path, uint32_t path_len,
                        nef_process_t *out_proc, int *out_err, int *out_validate_err) {
    if (out_proc) {
        out_proc->pid = 0u;
        out_proc->directory_phys = 0u;
        out_proc->scheduler_slot = 0xFFFFFFFFu;
        out_proc->page_count = 0u;
        out_proc->stack_vaddr = 0u;
    }
    if (out_proc == 0) return set_err(out_err, NEF_LOAD_ERR_OPEN);

    nef_header_t hdr;
    nef_section_entry_t secs[NEF_LOADER_MAX_SECTIONS];
    uint8_t *buf = 0;
    uint32_t file_size = 0u;
    int rc = open_validate_nef(loader_pid, path, path_len, &buf, &file_size, &hdr, secs,
                                NEF_LOADER_MAX_SECTIONS, out_validate_err);
    if (rc != NEF_LOAD_OK) return set_err(out_err, rc);

    if ((hdr.format_flags & NEF_FLAG_EXECUTABLE) == 0u) {
        kfree(buf);
        return set_err(out_err, NEF_LOAD_ERR_NOT_EXECUTABLE);
    }

    /* Bibliotheksdatei fuer dynamisches Linken (Phase 5, Minimalversion -
       siehe nef_loader.h/nef.h) - nur geladen, falls das Executable
       selbst eine nichtleere IMPORTS-Section hat. Puffer/Header/Sections
       bleiben hier ausserhalb der Section-Schleife unten deklariert, da
       sie erst NACH dem Pruefen der eigenen Sections (einschliesslich
       der ehrlichen RELOCATIONS/TLS-Ablehnung) bekannt werden koennen -
       das Executable koennte selbst schon vorher scheitern. */
    uint8_t *lib_buf = 0;
    nef_header_t lib_hdr;
    nef_section_entry_t lib_secs[NEF_LOADER_MAX_SECTIONS];
    uint32_t lib_file_size = 0u;
    uint32_t import_count = 0u;
    nef_section_entry_t imports_sec;
    nef_section_entry_t exports_sec = {0};
    uint32_t export_count = 0u;
    int have_library = 0;

    for (uint32_t i = 0; i < hdr.section_count; ++i) {
        uint32_t type = secs[i].type;
        /* Ehrliche Ablehnung statt Vortaeuschung: RELOCATIONS/TLS
           brauchen Mechanismen (Rebasing-Bedarf bzw. Segment-Register-
           Setup), die dieses Prozessmodell (noch) nicht hat - siehe
           nef_loader.h Dateikopf. Eine LEERE Section dieses Typs ist
           harmlos und wird NICHT abgelehnt. */
        if ((type == NEF_SECTION_RELOCATIONS || type == NEF_SECTION_TLS) &&
            (secs[i].file_size > 0u || secs[i].vsize > 0u)) {
            kfree(buf);
            return set_err(out_err, NEF_LOAD_ERR_UNSUPPORTED_DYNAMIC);
        }
        if (type == NEF_SECTION_IMPORTS && secs[i].file_size > 0u) {
            if ((secs[i].file_size % NEF_IMPORT_ENTRY_SIZE) != 0u) {
                kfree(buf);
                return set_err(out_err, NEF_LOAD_ERR_IMPORT_FORMAT);
            }
            imports_sec = secs[i];
            import_count = (uint32_t)(secs[i].file_size / NEF_IMPORT_ENTRY_SIZE);
        }
    }

    if (import_count > 0u) {
        nef_section_entry_t manifest_sec;
        if (find_section(secs, hdr.section_count, NEF_SECTION_MANIFEST, 0u, &manifest_sec) != 0 ||
            manifest_sec.file_size != NEF_LIBRARY_PATH_MAX) {
            kfree(buf);
            return set_err(out_err, NEF_LOAD_ERR_LIBRARY_MANIFEST);
        }
        char lib_path[NEF_LIBRARY_PATH_MAX + 1u];
        uint32_t mo = (uint32_t)manifest_sec.file_offset;
        uint32_t lib_path_len = 0u;
        for (uint32_t i = 0; i < NEF_LIBRARY_PATH_MAX; ++i) {
            uint8_t c = buf[mo + i];
            if (c == 0u) break;
            lib_path[i] = (char)c;
            ++lib_path_len;
        }
        lib_path[lib_path_len] = '\0';

        int lib_rc = open_validate_nef(loader_pid, lib_path, lib_path_len, &lib_buf,
                                        &lib_file_size, &lib_hdr, lib_secs,
                                        NEF_LOADER_MAX_SECTIONS, 0);
        if (lib_rc != NEF_LOAD_OK) {
            kfree(buf);
            /* Oeffnen/Lesen/Validieren/Arch/ABI der Bibliothek teilt
               sich dieselben Fehlercodes wie das Hauptfile - fuer den
               Aufrufer unterscheidbar bleibt der Kontext ueber
               out_validate_err (nur beim Hauptfile gesetzt) genug. */
            return set_err(out_err, (lib_rc == NEF_LOAD_ERR_VALIDATE) ? NEF_LOAD_ERR_LIBRARY_INVALID
                                                                       : lib_rc);
        }
        if ((lib_hdr.format_flags & NEF_FLAG_DYNAMIC_LIBRARY) == 0u) {
            kfree(buf); kfree(lib_buf);
            return set_err(out_err, NEF_LOAD_ERR_LIBRARY_NOT_DYNAMIC);
        }
        for (uint32_t i = 0; i < lib_hdr.section_count; ++i) {
            uint32_t type = lib_secs[i].type;
            if ((type == NEF_SECTION_RELOCATIONS || type == NEF_SECTION_TLS) &&
                (lib_secs[i].file_size > 0u || lib_secs[i].vsize > 0u)) {
                kfree(buf); kfree(lib_buf);
                return set_err(out_err, NEF_LOAD_ERR_UNSUPPORTED_DYNAMIC);
            }
            /* Keine transitiven Importe in dieser Minimalversion - siehe
               nef.h Dateikopf zum dynamischen Linken. */
            if (type == NEF_SECTION_IMPORTS && lib_secs[i].file_size > 0u) {
                kfree(buf); kfree(lib_buf);
                return set_err(out_err, NEF_LOAD_ERR_LIBRARY_TRANSITIVE);
            }
        }
        if (find_section(lib_secs, lib_hdr.section_count, NEF_SECTION_EXPORTS,
                          NEF_EXPORT_ENTRY_SIZE, &exports_sec) != 0) {
            kfree(buf); kfree(lib_buf);
            return set_err(out_err, NEF_LOAD_ERR_EXPORT_MISSING);
        }
        export_count = (uint32_t)(exports_sec.file_size / NEF_EXPORT_ENTRY_SIZE);
        have_library = 1;
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
            kfree(buf); if (have_library) kfree(lib_buf);
            return set_err(out_err, NEF_LOAD_ERR_EXTENSION);
        }
    }

    /* Signaturpruefung (Phase 5, siehe nef.h fuer die ehrliche Einordnung
       "symmetrische Dev-Signatur, keine PKI") - NUR erzwungen, wenn die
       Datei SELBST behauptet, signiert zu sein (NEF_FLAG_SIGNED). Dieser
       Kernel hat noch keine Mandatory-Signing-Policy (z.B. "nur
       signierte Programme duerfen aus /System laufen") - das waere eine
       eigene, hier bewusst nicht erfundene Entscheidung. */
    if ((hdr.format_flags & NEF_FLAG_SIGNED) != 0u) {
        if (nef_verify_signature(buf, file_size, &hdr, secs, hdr.section_count) != NEF_SIG_OK) {
            kfree(buf); if (have_library) kfree(lib_buf);
            return set_err(out_err, NEF_LOAD_ERR_SIGNATURE_INVALID);
        }
    }

    /* Capability-Manifest (Phase 5) - siehe nef_loader.h fuer das
       4-Byte-Format und das Confinement-Prinzip (loader_pid kann nur
       Rechte weitergeben, die es selbst hat). */
    uint32_t requested_caps = 0u;
    for (uint32_t i = 0; i < hdr.section_count; ++i) {
        if (secs[i].type != NEF_SECTION_CAPABILITIES || secs[i].file_size != 4u) continue;
        uint32_t off = (uint32_t)secs[i].file_offset;
        requested_caps = (uint32_t)buf[off] | ((uint32_t)buf[off + 1u] << 8) |
                          ((uint32_t)buf[off + 2u] << 16) | ((uint32_t)buf[off + 3u] << 24);
        break;
    }
    if (requested_caps != 0u && security_check(loader_pid, requested_caps) != 1) {
        kfree(buf); if (have_library) kfree(lib_buf);
        return set_err(out_err, NEF_LOAD_ERR_CAPABILITY_DENIED);
    }

    /* Gesamten Seitenbedarf (Executable-Sections + ggf. Bibliotheks-
       Sections + Stack) VOR jeder Allokation abschaetzen, damit ein zu
       grosses Image sauber abgelehnt wird, statt erst halb zu allozieren
       und dann wieder zurueckzurollen. */
    uint64_t needed_pages = 1u; /* Stack */
    uint64_t max_end = 0u;
    for (uint32_t i = 0; i < hdr.section_count; ++i) {
        if (secs[i].vaddr == 0u) continue;
        uint64_t pages = (secs[i].vsize + (uint64_t)PMM_PAGE_SIZE - 1u) / (uint64_t)PMM_PAGE_SIZE;
        needed_pages += pages;
        uint64_t end = secs[i].vaddr + pages * (uint64_t)PMM_PAGE_SIZE;
        if (end > max_end) max_end = end;
    }
    if (have_library) {
        for (uint32_t i = 0; i < lib_hdr.section_count; ++i) {
            if (lib_secs[i].vaddr == 0u) continue;
            uint64_t pages = (lib_secs[i].vsize + (uint64_t)PMM_PAGE_SIZE - 1u) / (uint64_t)PMM_PAGE_SIZE;
            needed_pages += pages;
            uint64_t end = lib_secs[i].vaddr + pages * (uint64_t)PMM_PAGE_SIZE;
            if (end > max_end) max_end = end;

            /* Echte Sicherheitspruefung, kein Dekorationscode: die
               Bibliothek wird in DENSELBEN Adressraum wie das
               Executable gemappt - ueberlappende vaddr-Bereiche wuerden
               sich stillschweigend gegenseitig ueberschreiben (letztes
               Mapping gewinnt, siehe paging_map_page_into). */
            uint64_t lib_start = lib_secs[i].vaddr;
            uint64_t lib_end = lib_start + pages * (uint64_t)PMM_PAGE_SIZE;
            for (uint32_t j = 0; j < hdr.section_count; ++j) {
                if (secs[j].vaddr == 0u) continue;
                uint64_t exe_pages = (secs[j].vsize + (uint64_t)PMM_PAGE_SIZE - 1u) /
                                     (uint64_t)PMM_PAGE_SIZE;
                uint64_t exe_start = secs[j].vaddr;
                uint64_t exe_end = exe_start + exe_pages * (uint64_t)PMM_PAGE_SIZE;
                if (lib_start < exe_end && exe_start < lib_end) {
                    kfree(buf); kfree(lib_buf);
                    return set_err(out_err, NEF_LOAD_ERR_LIBRARY_OVERLAP);
                }
            }
        }
    }
    if (needed_pages > NEF_LOADER_MAX_PAGES) {
        kfree(buf); if (have_library) kfree(lib_buf);
        return set_err(out_err, NEF_LOAD_ERR_IMAGE_TOO_LARGE);
    }

    uint32_t dir_phys;
    if (paging_create_address_space(&dir_phys) != 0) {
        kfree(buf); if (have_library) kfree(lib_buf);
        return set_err(out_err, NEF_LOAD_ERR_ADDRESS_SPACE);
    }

    uint32_t pid = 0u;
    uint32_t slot = 0xFFFFFFFFu;
    int slot_claimed = 0;
    uint32_t stack_vaddr = 0u;
    uint32_t stack_top = 0u;

    int err = map_sections(dir_phys, buf, secs, hdr.section_count, out_proc, &max_end);
    if (err == NEF_LOAD_OK && have_library) {
        err = map_sections(dir_phys, lib_buf, lib_secs, lib_hdr.section_count, out_proc, &max_end);
    }
    if (err == NEF_LOAD_OK && import_count > 0u) {
        /* Importe aufloesen: fuer jeden {ordinal, iat_vaddr}-Eintrag des
           Executables den passenden Export der Bibliothek lesen und als
           little-endian u32 in die bereits gemappte Executable-Seite an
           iat_vaddr schreiben (IAT-Patching). */
        for (uint32_t i = 0; i < import_count && err == NEF_LOAD_OK; ++i) {
            uint32_t eo = (uint32_t)imports_sec.file_offset + i * NEF_IMPORT_ENTRY_SIZE;
            uint32_t ordinal = (uint32_t)buf[eo] | ((uint32_t)buf[eo + 1u] << 8) |
                                ((uint32_t)buf[eo + 2u] << 16) | ((uint32_t)buf[eo + 3u] << 24);
            uint64_t iat_vaddr =
                (uint64_t)buf[eo + 4u] | ((uint64_t)buf[eo + 5u] << 8) |
                ((uint64_t)buf[eo + 6u] << 16) | ((uint64_t)buf[eo + 7u] << 24) |
                ((uint64_t)buf[eo + 8u] << 32) | ((uint64_t)buf[eo + 9u] << 40) |
                ((uint64_t)buf[eo + 10u] << 48) | ((uint64_t)buf[eo + 11u] << 56);

            if (ordinal >= export_count) { err = NEF_LOAD_ERR_IMPORT_UNRESOLVED; break; }
            uint32_t xo = (uint32_t)exports_sec.file_offset + ordinal * NEF_EXPORT_ENTRY_SIZE;
            uint64_t export_vaddr =
                (uint64_t)lib_buf[xo] | ((uint64_t)lib_buf[xo + 1u] << 8) |
                ((uint64_t)lib_buf[xo + 2u] << 16) | ((uint64_t)lib_buf[xo + 3u] << 24) |
                ((uint64_t)lib_buf[xo + 4u] << 32) | ((uint64_t)lib_buf[xo + 5u] << 40) |
                ((uint64_t)lib_buf[xo + 6u] << 48) | ((uint64_t)lib_buf[xo + 7u] << 56);

            if (patch_iat_slot(dir_phys, (uint32_t)iat_vaddr, (uint32_t)export_vaddr) != 0) {
                err = NEF_LOAD_ERR_IAT_PATCH;
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
        else if (requested_caps != 0u) {
            /* loader_pid wurde oben bereits per security_check() als
               Obergrenze bestaetigt - security_grant() selbst kann
               theoretisch nur noch an einer vollen security.h-Tabelle
               scheitern (SECURITY_CAPACITY Slots), was bei einem gerade
               frisch erzeugten Prozess praktisch nicht vorkommt, aber
               trotzdem nicht stillschweigend ignoriert wird. */
            if (security_grant(pid, requested_caps) != 0) err = NEF_LOAD_ERR_CAPABILITY_DENIED;
        }
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
    if (have_library) kfree(lib_buf);

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
    out_proc->stack_vaddr = stack_vaddr;
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

#define NEF_LOADER_SELFTEST_LIB_PATH "/nef_loader_selftest_lib.bin"
#define NEF_LOADER_SELFTEST_LIB_NAME "nef_loader_selftest_lib.bin"
#define NEF_LOADER_SELFTEST_DYN_PATH "/nef_loader_selftest_dyn.bin"
#define NEF_LOADER_SELFTEST_DYN_NAME "nef_loader_selftest_dyn.bin"
#define NEF_LOADER_SELFTEST_LIB_VADDR 0x00600000u
#define NEF_LOADER_SELFTEST_DATA_VADDR 0x00501000u

static void write_u64le(uint8_t *buf, uint64_t v) {
    for (uint32_t i = 0; i < 8u; ++i) buf[i] = (uint8_t)(v >> (8u * i));
}
static void write_u32le(uint8_t *buf, uint32_t v) {
    for (uint32_t i = 0; i < 4u; ++i) buf[i] = (uint8_t)(v >> (8u * i));
}

/* 1 = ok, 0 = Fehlschlag (Aufrufkonvention passend zu nef_loader_self_
   test()s lokaler `ok`-Variable). Baut ECHT eine Bibliothek (1 Export)
   + ein Executable (1 Import darauf), schreibt beide auf NovaFS, laedt
   das Executable per nef_load_and_spawn() und prueft, dass die per
   IAT-Patching geschriebene Adresse tatsaechlich im Speicher des neuen
   Prozesses steht - kein reiner Formatcheck, sondern der volle Rundweg. */
static int nef_loader_self_test_dynlink(void) {
    static const uint8_t lib_payload[16] = {
        0x90u, 0x90u, 0x90u, 0x90u, 0xEBu, 0xFEu, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u
    };

    /* -- Bibliothek: CODE(0) + EXPORTS(1), Ordinal 0 = CODE-vaddr. -- */
    uint8_t lib_img[NEF_HEADER_SIZE + NEF_SECTION_ENTRY_SIZE * 2u + 16u + NEF_EXPORT_ENTRY_SIZE];
    for (uint32_t i = 0; i < sizeof(lib_img); ++i) lib_img[i] = 0u;

    nef_header_t lib_hdr;
    lib_hdr.version_major = NEF_VERSION_MAJOR_CURRENT;
    lib_hdr.version_minor = NEF_VERSION_MINOR_CURRENT;
    lib_hdr.header_size = NEF_HEADER_SIZE;
    lib_hdr.file_size = sizeof(lib_img);
    lib_hdr.format_flags = NEF_FLAG_DYNAMIC_LIBRARY;
    lib_hdr.target_arch = NEF_ARCH_X86_32;
    lib_hdr.abi_id = NEF_ABI_NOVA_NATIVE;
    lib_hdr.min_os_version = 0u;
    lib_hdr.entry_point_ref = 0u; /* Bibliotheken werden nicht selbst gestartet */
    lib_hdr.section_dir_offset = NEF_HEADER_SIZE;
    lib_hdr.section_count = 2u;
    lib_hdr.extension_dir_offset = NEF_HEADER_SIZE + NEF_SECTION_ENTRY_SIZE * 2u + 16u +
                                    NEF_EXPORT_ENTRY_SIZE;
    lib_hdr.extension_count = 0u;
    lib_hdr.manifest_ref = 0u;
    lib_hdr.signature_dir_ref = 0u;
    lib_hdr.integrity_ref = 0u;
    if (nef_header_serialize(&lib_hdr, lib_img, sizeof(lib_img)) != 0) return 0;

    nef_section_entry_t lib_code;
    lib_code.type = NEF_SECTION_CODE;
    lib_code.flags = NEF_SEC_FLAG_READ | NEF_SEC_FLAG_EXEC;
    lib_code.file_offset = NEF_HEADER_SIZE + NEF_SECTION_ENTRY_SIZE * 2u;
    lib_code.file_size = sizeof(lib_payload);
    lib_code.vaddr = NEF_LOADER_SELFTEST_LIB_VADDR;
    lib_code.vsize = sizeof(lib_payload);
    lib_code.alignment = 4096u;
    nef_section_entry_serialize(&lib_code, lib_img + NEF_HEADER_SIZE);
    for (uint32_t i = 0; i < sizeof(lib_payload); ++i) {
        lib_img[(uint32_t)lib_code.file_offset + i] = lib_payload[i];
    }

    nef_section_entry_t lib_exports;
    lib_exports.type = NEF_SECTION_EXPORTS;
    lib_exports.flags = NEF_SEC_FLAG_READ;
    lib_exports.file_offset = lib_code.file_offset + lib_code.file_size;
    lib_exports.file_size = NEF_EXPORT_ENTRY_SIZE;
    lib_exports.vaddr = 0u;
    lib_exports.vsize = NEF_EXPORT_ENTRY_SIZE;
    lib_exports.alignment = 1u;
    nef_section_entry_serialize(&lib_exports, lib_img + NEF_HEADER_SIZE + NEF_SECTION_ENTRY_SIZE);
    write_u64le(lib_img + (uint32_t)lib_exports.file_offset, (uint64_t)NEF_LOADER_SELFTEST_LIB_VADDR);

    /* -- Executable: CODE(0) + DATA(1, = IAT-Slot) + IMPORTS(2) +
       MANIFEST(3, = Bibliothekspfad). -- */
    uint8_t exe_img[NEF_HEADER_SIZE + NEF_SECTION_ENTRY_SIZE * 4u + 16u + 4u +
                     NEF_IMPORT_ENTRY_SIZE + NEF_LIBRARY_PATH_MAX];
    for (uint32_t i = 0; i < sizeof(exe_img); ++i) exe_img[i] = 0u;

    nef_header_t exe_hdr;
    exe_hdr.version_major = NEF_VERSION_MAJOR_CURRENT;
    exe_hdr.version_minor = NEF_VERSION_MINOR_CURRENT;
    exe_hdr.header_size = NEF_HEADER_SIZE;
    exe_hdr.file_size = sizeof(exe_img);
    exe_hdr.format_flags = NEF_FLAG_EXECUTABLE;
    exe_hdr.target_arch = NEF_ARCH_X86_32;
    exe_hdr.abi_id = NEF_ABI_NOVA_NATIVE;
    exe_hdr.min_os_version = 0u;
    exe_hdr.entry_point_ref = NEF_LOADER_SELFTEST_VADDR;
    exe_hdr.section_dir_offset = NEF_HEADER_SIZE;
    exe_hdr.section_count = 4u;
    exe_hdr.extension_dir_offset = NEF_HEADER_SIZE + NEF_SECTION_ENTRY_SIZE * 4u + 16u + 4u +
                                    NEF_IMPORT_ENTRY_SIZE + NEF_LIBRARY_PATH_MAX;
    exe_hdr.extension_count = 0u;
    exe_hdr.manifest_ref = 0u;
    exe_hdr.signature_dir_ref = 0u;
    exe_hdr.integrity_ref = 0u;
    if (nef_header_serialize(&exe_hdr, exe_img, sizeof(exe_img)) != 0) return 0;

    static const uint8_t exe_payload[16] = {
        0x90u, 0x90u, 0x90u, 0x90u, 0xEBu, 0xFEu, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u
    };

    nef_section_entry_t exe_code;
    exe_code.type = NEF_SECTION_CODE;
    exe_code.flags = NEF_SEC_FLAG_READ | NEF_SEC_FLAG_EXEC;
    exe_code.file_offset = NEF_HEADER_SIZE + NEF_SECTION_ENTRY_SIZE * 4u;
    exe_code.file_size = sizeof(exe_payload);
    exe_code.vaddr = NEF_LOADER_SELFTEST_VADDR;
    exe_code.vsize = sizeof(exe_payload);
    exe_code.alignment = 4096u;
    nef_section_entry_serialize(&exe_code, exe_img + NEF_HEADER_SIZE);
    for (uint32_t i = 0; i < sizeof(exe_payload); ++i) {
        exe_img[(uint32_t)exe_code.file_offset + i] = exe_payload[i];
    }

    nef_section_entry_t exe_data;
    exe_data.type = NEF_SECTION_DATA;
    exe_data.flags = NEF_SEC_FLAG_READ | NEF_SEC_FLAG_WRITE;
    exe_data.file_offset = exe_code.file_offset + exe_code.file_size;
    exe_data.file_size = 4u;
    exe_data.vaddr = NEF_LOADER_SELFTEST_DATA_VADDR;
    exe_data.vsize = 4u;
    exe_data.alignment = 4096u;
    nef_section_entry_serialize(&exe_data, exe_img + NEF_HEADER_SIZE + NEF_SECTION_ENTRY_SIZE);

    nef_section_entry_t exe_imports;
    exe_imports.type = NEF_SECTION_IMPORTS;
    exe_imports.flags = NEF_SEC_FLAG_READ;
    exe_imports.file_offset = exe_data.file_offset + exe_data.file_size;
    exe_imports.file_size = NEF_IMPORT_ENTRY_SIZE;
    exe_imports.vaddr = 0u;
    exe_imports.vsize = NEF_IMPORT_ENTRY_SIZE;
    exe_imports.alignment = 1u;
    nef_section_entry_serialize(&exe_imports, exe_img + NEF_HEADER_SIZE + NEF_SECTION_ENTRY_SIZE * 2u);
    {
        uint32_t eo = (uint32_t)exe_imports.file_offset;
        write_u32le(exe_img + eo, 0u); /* Ordinal 0 */
        write_u64le(exe_img + eo + 4u, (uint64_t)NEF_LOADER_SELFTEST_DATA_VADDR);
    }

    nef_section_entry_t exe_manifest;
    exe_manifest.type = NEF_SECTION_MANIFEST;
    exe_manifest.flags = NEF_SEC_FLAG_READ;
    exe_manifest.file_offset = exe_imports.file_offset + exe_imports.file_size;
    exe_manifest.file_size = NEF_LIBRARY_PATH_MAX;
    exe_manifest.vaddr = 0u;
    exe_manifest.vsize = NEF_LIBRARY_PATH_MAX;
    exe_manifest.alignment = 1u;
    nef_section_entry_serialize(&exe_manifest, exe_img + NEF_HEADER_SIZE + NEF_SECTION_ENTRY_SIZE * 3u);
    {
        uint32_t mo = (uint32_t)exe_manifest.file_offset;
        uint32_t lib_path_len = cstr_len(NEF_LOADER_SELFTEST_LIB_PATH);
        for (uint32_t i = 0; i < lib_path_len; ++i) {
            exe_img[mo + i] = (uint8_t)NEF_LOADER_SELFTEST_LIB_PATH[i];
        }
    }

    /* Beide Testdateien auf NovaFS schreiben. */
    uint32_t root_handle;
    if (vfsapi_lookup(1u, 0u, "/", 1u, VFSAPI_LOOKUP_FLAG_WRITE, &root_handle) != 0) return 0;

    uint32_t lib_handle;
    uint32_t lib_name_len = cstr_len(NEF_LOADER_SELFTEST_LIB_NAME);
    if (vfsapi_create(1u, root_handle, NEF_LOADER_SELFTEST_LIB_NAME, lib_name_len,
                       NOVAFS_TYPE_FILE, &lib_handle) != 0 &&
        vfsapi_lookup(1u, root_handle, NEF_LOADER_SELFTEST_LIB_NAME, lib_name_len,
                      VFSAPI_LOOKUP_FLAG_WRITE, &lib_handle) != 0) {
        vfsapi_close(1u, root_handle);
        return 0;
    }
    uint32_t written = 0u;
    int lib_write_rc = vfsapi_write(1u, lib_handle, 0u, lib_img, sizeof(lib_img), &written);
    vfsapi_close(1u, lib_handle);
    if (lib_write_rc != 0 || written != sizeof(lib_img)) {
        vfsapi_close(1u, root_handle);
        return 0;
    }

    uint32_t dyn_handle;
    uint32_t dyn_name_len = cstr_len(NEF_LOADER_SELFTEST_DYN_NAME);
    if (vfsapi_create(1u, root_handle, NEF_LOADER_SELFTEST_DYN_NAME, dyn_name_len,
                       NOVAFS_TYPE_FILE, &dyn_handle) != 0 &&
        vfsapi_lookup(1u, root_handle, NEF_LOADER_SELFTEST_DYN_NAME, dyn_name_len,
                      VFSAPI_LOOKUP_FLAG_WRITE, &dyn_handle) != 0) {
        vfsapi_close(1u, root_handle);
        return 0;
    }
    written = 0u;
    int dyn_write_rc = vfsapi_write(1u, dyn_handle, 0u, exe_img, sizeof(exe_img), &written);
    vfsapi_close(1u, dyn_handle);
    vfsapi_close(1u, root_handle);
    if (dyn_write_rc != 0 || written != sizeof(exe_img)) return 0;

    nef_process_t proc;
    int err = 0;
    int verr = 0;
    int rc = nef_load_and_spawn(1u, NEF_LOADER_SELFTEST_DYN_PATH,
                                 cstr_len(NEF_LOADER_SELFTEST_DYN_PATH), &proc, &err, &verr);

    int ok = (rc == 0 && err == NEF_LOAD_OK && proc.pid != 0u);
    if (ok) {
        uint32_t kernel_dir = paging_kernel_directory_physical();
        paging_switch_address_space(proc.directory_phys);
        const uint8_t *iat = (const uint8_t *)(uintptr_t)NEF_LOADER_SELFTEST_DATA_VADDR;
        uint32_t resolved = (uint32_t)iat[0] | ((uint32_t)iat[1] << 8) |
                             ((uint32_t)iat[2] << 16) | ((uint32_t)iat[3] << 24);
        paging_switch_address_space(kernel_dir);
        if (resolved != NEF_LOADER_SELFTEST_LIB_VADDR) ok = 0;
    }
    if (proc.pid != 0u) nef_unload(&proc);

    if (vfsapi_lookup(1u, 0u, "/", 1u, VFSAPI_LOOKUP_FLAG_WRITE, &root_handle) == 0) {
        vfsapi_delete(1u, root_handle, NEF_LOADER_SELFTEST_DYN_NAME, dyn_name_len);
        vfsapi_delete(1u, root_handle, NEF_LOADER_SELFTEST_LIB_NAME, lib_name_len);
        vfsapi_close(1u, root_handle);
    }

    return ok;
}

int nef_loader_self_test(void) {
    /* 3 Sections: CODE (ausgefuehrt/geprueft), CAPABILITIES (4 Byte -
       fordert SECURITY_CAP_FS_READ, das PID 1 als Kernel besitzt),
       SIGNATURES (32 Byte, als LETZTE Section - deckt per Konvention
       alles Vorherige inkl. CAPABILITIES ab). Prueft damit Phase 4
       (Mapping/Scheduler) UND Phase 5 (Signatur+Capability-Gate) in
       einem echten Rundweg. */
    uint8_t img[NEF_HEADER_SIZE + NEF_SECTION_ENTRY_SIZE * 3u + 16u + 4u +
                NEF_SIGNATURE_DIGEST_SIZE];
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
    hdr.format_flags = NEF_FLAG_EXECUTABLE | NEF_FLAG_SIGNED;
    hdr.target_arch = NEF_ARCH_X86_32;
    hdr.abi_id = NEF_ABI_NOVA_NATIVE;
    hdr.min_os_version = 0u;
    hdr.entry_point_ref = NEF_LOADER_SELFTEST_VADDR;
    hdr.section_dir_offset = NEF_HEADER_SIZE;
    hdr.section_count = 3u;
    hdr.extension_dir_offset = NEF_HEADER_SIZE + NEF_SECTION_ENTRY_SIZE * 3u + 16u + 4u +
                                NEF_SIGNATURE_DIGEST_SIZE;
    hdr.extension_count = 0u;
    hdr.manifest_ref = 0u;
    hdr.signature_dir_ref = 0u;
    hdr.integrity_ref = 0u;
    if (nef_header_serialize(&hdr, img, sizeof(img)) != 0) return -1;

    nef_section_entry_t code_sec;
    code_sec.type = NEF_SECTION_CODE;
    code_sec.flags = NEF_SEC_FLAG_READ | NEF_SEC_FLAG_EXEC;
    code_sec.file_offset = NEF_HEADER_SIZE + NEF_SECTION_ENTRY_SIZE * 3u;
    code_sec.file_size = sizeof(payload);
    code_sec.vaddr = NEF_LOADER_SELFTEST_VADDR;
    code_sec.vsize = sizeof(payload);
    code_sec.alignment = 4096u;
    nef_section_entry_serialize(&code_sec, img + NEF_HEADER_SIZE);
    for (uint32_t i = 0; i < sizeof(payload); ++i) {
        img[(uint32_t)code_sec.file_offset + i] = payload[i];
    }

    nef_section_entry_t cap_sec;
    cap_sec.type = NEF_SECTION_CAPABILITIES;
    cap_sec.flags = NEF_SEC_FLAG_READ;
    cap_sec.file_offset = code_sec.file_offset + code_sec.file_size;
    cap_sec.file_size = 4u;
    cap_sec.vaddr = 0u;
    cap_sec.vsize = 4u;
    cap_sec.alignment = 1u;
    nef_section_entry_serialize(&cap_sec, img + NEF_HEADER_SIZE + NEF_SECTION_ENTRY_SIZE);
    {
        uint32_t off = (uint32_t)cap_sec.file_offset;
        img[off] = (uint8_t)(SECURITY_CAP_FS_READ & 0xFFu);
        img[off + 1u] = (uint8_t)((SECURITY_CAP_FS_READ >> 8) & 0xFFu);
        img[off + 2u] = (uint8_t)((SECURITY_CAP_FS_READ >> 16) & 0xFFu);
        img[off + 3u] = (uint8_t)((SECURITY_CAP_FS_READ >> 24) & 0xFFu);
    }

    nef_section_entry_t sig_sec;
    sig_sec.type = NEF_SECTION_SIGNATURES;
    sig_sec.flags = NEF_SEC_FLAG_READ;
    sig_sec.file_offset = cap_sec.file_offset + cap_sec.file_size;
    sig_sec.file_size = NEF_SIGNATURE_DIGEST_SIZE;
    sig_sec.vaddr = 0u;
    sig_sec.vsize = NEF_SIGNATURE_DIGEST_SIZE;
    sig_sec.alignment = 1u;
    nef_section_entry_serialize(&sig_sec, img + NEF_HEADER_SIZE + NEF_SECTION_ENTRY_SIZE * 2u);

    uint8_t digest[NEF_SIGNATURE_DIGEST_SIZE];
    nef_compute_signature(img, (uint32_t)sig_sec.file_offset, digest);
    for (uint32_t i = 0; i < NEF_SIGNATURE_DIGEST_SIZE; ++i) {
        img[(uint32_t)sig_sec.file_offset + i] = digest[i];
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

    /* 1) Negativtest: EIN verfaelschtes Digest-Byte MUSS den Load
       zuverlaessig mit NEF_LOAD_ERR_SIGNATURE_INVALID ablehnen - sonst
       waere die Signaturpruefung nur Dekoration. */
    {
        uint8_t bad_img[sizeof(img)];
        for (uint32_t i = 0; i < sizeof(img); ++i) bad_img[i] = img[i];
        bad_img[(uint32_t)sig_sec.file_offset] ^= 0xFFu;

        uint32_t written = 0u;
        if (vfsapi_write(1u, file_handle, 0u, bad_img, sizeof(bad_img), &written) != 0 ||
            written != sizeof(bad_img)) {
            vfsapi_close(1u, file_handle);
            vfsapi_close(1u, root_handle);
            return -1;
        }

        nef_process_t bad_proc;
        int bad_err = 0, bad_verr = 0;
        int bad_rc = nef_load_and_spawn(1u, NEF_LOADER_SELFTEST_PATH,
                                         cstr_len(NEF_LOADER_SELFTEST_PATH), &bad_proc,
                                         &bad_err, &bad_verr);
        if (bad_rc == 0 || bad_err != NEF_LOAD_ERR_SIGNATURE_INVALID || bad_proc.pid != 0u) {
            if (bad_proc.pid != 0u) nef_unload(&bad_proc);
            vfsapi_close(1u, file_handle);
            vfsapi_close(1u, root_handle);
            return -1;
        }
    }

    /* 2) Positivtest: gueltig signierte Datei mit CAPABILITIES-Section -
       muss laden UND die angeforderte Capability tatsaechlich auf den
       neuen Prozess uebertragen (nicht nur "laedt durch"). */
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
    if (ok && security_check(proc.pid, SECURITY_CAP_FS_READ) != 1) ok = 0;

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

    /* 3) Dynamisches Linken (Phase 5, Minimalversion - echter Rundweg):
       eine Bibliothek mit EINEM Export + ein Executable mit EINEM Import
       darauf bauen, laden, und pruefen, dass die aufgeloeste Adresse
       tatsaechlich in die Import-Adresstabelle des Executables
       geschrieben wurde. */
    if (ok) {
        ok = nef_loader_self_test_dynlink();
    }

    return ok ? 0 : -1;
}
