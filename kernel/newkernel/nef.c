/* NovaOS C-Kernel - Nova Executable Format (NEF) 1.0 (siehe nef.h fuer
   die vollstaendige Format-/Scope-Beschreibung).

   WICHTIG: alle Mehrbyte-Felder sind little-endian (native x86-
   Byte-Reihenfolge) - die Lese-/Schreibhilfen unten sind trotzdem
   explizit Byte-fuer-Byte geschrieben (nicht per uint32_t*-Cast auf
   den Puffer), damit das Format auch auf einer big-endian-Architektur
   (ARM64 kann beides) korrekt bleibt, falls NEF dort jemals geparst
   wird - siehe nef.h's Architektur-Hinweis. KEINE 64-Bit-Division/
   -Modulo irgendwo in dieser Datei (siehe project_c_kernel_migration_
   status.md: __udivdi3 etc. sind im freistehenden Build nicht
   verlinkbar) - alle Pruefungen hier kommen mit Vergleichen, Additionen
   und Bit-Operationen aus. */
#include "nef.h"

const uint8_t NEF_MAGIC[NEF_MAGIC_SIZE] = { 'N','O','V','A','N','E','F','1' };

static void put_u16(uint8_t *buf, uint32_t off, uint16_t v) {
    buf[off] = (uint8_t)(v & 0xFFu);
    buf[off + 1] = (uint8_t)((v >> 8) & 0xFFu);
}
static void put_u32(uint8_t *buf, uint32_t off, uint32_t v) {
    buf[off] = (uint8_t)(v & 0xFFu);
    buf[off + 1] = (uint8_t)((v >> 8) & 0xFFu);
    buf[off + 2] = (uint8_t)((v >> 16) & 0xFFu);
    buf[off + 3] = (uint8_t)((v >> 24) & 0xFFu);
}
static void put_u64(uint8_t *buf, uint32_t off, uint64_t v) {
    put_u32(buf, off, (uint32_t)(v & 0xFFFFFFFFu));
    put_u32(buf, off + 4, (uint32_t)((v >> 32) & 0xFFFFFFFFu));
}
static uint16_t get_u16(const uint8_t *buf, uint32_t off) {
    return (uint16_t)(buf[off] | ((uint16_t)buf[off + 1] << 8));
}
static uint32_t get_u32(const uint8_t *buf, uint32_t off) {
    return (uint32_t)buf[off] | ((uint32_t)buf[off + 1] << 8) |
           ((uint32_t)buf[off + 2] << 16) | ((uint32_t)buf[off + 3] << 24);
}
static uint64_t get_u64(const uint8_t *buf, uint32_t off) {
    uint64_t lo = get_u32(buf, off);
    uint64_t hi = get_u32(buf, off + 4);
    return lo | (hi << 32);
}

int nef_header_serialize(const nef_header_t *in, uint8_t *out_buf, uint32_t out_cap) {
    if (out_cap < NEF_HEADER_SIZE) return -1;
    for (uint32_t i = 0; i < NEF_MAGIC_SIZE; ++i) out_buf[NEF_HDR_OFF_MAGIC + i] = NEF_MAGIC[i];
    put_u16(out_buf, NEF_HDR_OFF_VERSION_MAJOR, in->version_major);
    put_u16(out_buf, NEF_HDR_OFF_VERSION_MINOR, in->version_minor);
    put_u32(out_buf, NEF_HDR_OFF_HEADER_SIZE, in->header_size);
    put_u64(out_buf, NEF_HDR_OFF_FILE_SIZE, in->file_size);
    put_u32(out_buf, NEF_HDR_OFF_FORMAT_FLAGS, in->format_flags);
    put_u32(out_buf, NEF_HDR_OFF_TARGET_ARCH, in->target_arch);
    put_u32(out_buf, NEF_HDR_OFF_ABI_ID, in->abi_id);
    put_u32(out_buf, NEF_HDR_OFF_MIN_OS_VERSION, in->min_os_version);
    put_u64(out_buf, NEF_HDR_OFF_ENTRY_POINT_REF, in->entry_point_ref);
    put_u64(out_buf, NEF_HDR_OFF_SECTION_DIR_OFFSET, in->section_dir_offset);
    put_u32(out_buf, NEF_HDR_OFF_SECTION_COUNT, in->section_count);
    put_u64(out_buf, NEF_HDR_OFF_EXTENSION_DIR_OFFSET, in->extension_dir_offset);
    put_u32(out_buf, NEF_HDR_OFF_EXTENSION_COUNT, in->extension_count);
    put_u64(out_buf, NEF_HDR_OFF_MANIFEST_REF, in->manifest_ref);
    put_u64(out_buf, NEF_HDR_OFF_SIGNATURE_DIR_REF, in->signature_dir_ref);
    put_u64(out_buf, NEF_HDR_OFF_INTEGRITY_REF, in->integrity_ref);
    return 0;
}

int nef_header_parse(const uint8_t *buf, uint32_t buf_len, nef_header_t *out, int *out_err) {
    if (buf_len < NEF_HEADER_SIZE) {
        if (out_err) *out_err = NEF_PARSE_ERR_TOO_SHORT;
        return -1;
    }
    for (uint32_t i = 0; i < NEF_MAGIC_SIZE; ++i) {
        if (buf[NEF_HDR_OFF_MAGIC + i] != NEF_MAGIC[i]) {
            if (out_err) *out_err = NEF_PARSE_ERR_BAD_MAGIC;
            return -1;
        }
    }
    out->version_major = get_u16(buf, NEF_HDR_OFF_VERSION_MAJOR);
    out->version_minor = get_u16(buf, NEF_HDR_OFF_VERSION_MINOR);
    out->header_size = get_u32(buf, NEF_HDR_OFF_HEADER_SIZE);
    if (out->header_size != NEF_HEADER_SIZE) {
        /* Kein abweichendes Layout wird akzeptiert (siehe nef.h) - ein
           kuenftiges Major-Format-Redesign bekaeme eine eigene Magic,
           nicht nur eine andere header_size. */
        if (out_err) *out_err = NEF_PARSE_ERR_BAD_HDRSIZE;
        return -1;
    }
    out->file_size = get_u64(buf, NEF_HDR_OFF_FILE_SIZE);
    if (out->file_size < out->header_size) {
        if (out_err) *out_err = NEF_PARSE_ERR_BAD_FILESIZE;
        return -1;
    }
    out->format_flags = get_u32(buf, NEF_HDR_OFF_FORMAT_FLAGS);
    out->target_arch = get_u32(buf, NEF_HDR_OFF_TARGET_ARCH);
    out->abi_id = get_u32(buf, NEF_HDR_OFF_ABI_ID);
    out->min_os_version = get_u32(buf, NEF_HDR_OFF_MIN_OS_VERSION);
    out->entry_point_ref = get_u64(buf, NEF_HDR_OFF_ENTRY_POINT_REF);
    out->section_dir_offset = get_u64(buf, NEF_HDR_OFF_SECTION_DIR_OFFSET);
    out->section_count = get_u32(buf, NEF_HDR_OFF_SECTION_COUNT);
    out->extension_dir_offset = get_u64(buf, NEF_HDR_OFF_EXTENSION_DIR_OFFSET);
    out->extension_count = get_u32(buf, NEF_HDR_OFF_EXTENSION_COUNT);
    out->manifest_ref = get_u64(buf, NEF_HDR_OFF_MANIFEST_REF);
    out->signature_dir_ref = get_u64(buf, NEF_HDR_OFF_SIGNATURE_DIR_REF);
    out->integrity_ref = get_u64(buf, NEF_HDR_OFF_INTEGRITY_REF);
    if (out_err) *out_err = NEF_PARSE_OK;
    return 0;
}

int nef_header_version_supported(const nef_header_t *hdr) {
    /* Unbekannte Major-Version ist NIE kompatibel (Masterauftrag
       Abschnitt 4) - aktuell existiert nur Major 1, daher eine exakte
       Gleichheitspruefung statt einer "<="-Bereichspruefung (die wuerde
       stillschweigend eine kuenftige, strukturell andere Major-Version
       akzeptieren). */
    return hdr->version_major == NEF_VERSION_MAJOR_CURRENT;
}

void nef_section_entry_serialize(const nef_section_entry_t *in, uint8_t *out_buf) {
    put_u32(out_buf, NEF_SEC_OFF_TYPE, in->type);
    put_u32(out_buf, NEF_SEC_OFF_FLAGS, in->flags);
    put_u64(out_buf, NEF_SEC_OFF_FILE_OFFSET, in->file_offset);
    put_u64(out_buf, NEF_SEC_OFF_FILE_SIZE, in->file_size);
    put_u64(out_buf, NEF_SEC_OFF_VADDR, in->vaddr);
    put_u64(out_buf, NEF_SEC_OFF_VSIZE, in->vsize);
    put_u32(out_buf, NEF_SEC_OFF_ALIGNMENT, in->alignment);
    put_u32(out_buf, NEF_SEC_OFF_RESERVED, 0u);
}

void nef_section_entry_parse(const uint8_t *buf, nef_section_entry_t *out) {
    out->type = get_u32(buf, NEF_SEC_OFF_TYPE);
    out->flags = get_u32(buf, NEF_SEC_OFF_FLAGS);
    out->file_offset = get_u64(buf, NEF_SEC_OFF_FILE_OFFSET);
    out->file_size = get_u64(buf, NEF_SEC_OFF_FILE_SIZE);
    out->vaddr = get_u64(buf, NEF_SEC_OFF_VADDR);
    out->vsize = get_u64(buf, NEF_SEC_OFF_VSIZE);
    out->alignment = get_u32(buf, NEF_SEC_OFF_ALIGNMENT);
}

int nef_validate_section(const nef_section_entry_t *sec) {
    if (sec->type >= NEF_SECTION_TYPE_COUNT) return -1;
    /* W^X (Masterauftrag Abschnitt 5/13): WRITE und EXEC gleichzeitig
       ist IMMER ein harter Fehler, unabhaengig vom Section-Typ. */
    if ((sec->flags & NEF_SEC_FLAG_WRITE) != 0u && (sec->flags & NEF_SEC_FLAG_EXEC) != 0u) {
        return -1;
    }
    /* vsize deckt file_size ab (die Differenz ist Null-initialisiert,
       wie ELF .bss) - vsize < file_size wuerde Dateidaten abschneiden. */
    if (sec->vsize < sec->file_size) return -1;
    /* alignment muss 0 (keine Anforderung) oder eine Zweierpotenz sein. */
    if (sec->alignment != 0u && (sec->alignment & (sec->alignment - 1u)) != 0u) return -1;
    return 0;
}

/* uint64_t a+b, mit Ueberlauf-Erkennung (siehe nef.h: jede Offset/
   Groessen-Rechnung muss auf Ueberlauf geprueft werden). 0 = kein
   Ueberlauf (*out gesetzt), -1 = Ueberlauf. */
static int add_u64_checked(uint64_t a, uint64_t b, uint64_t *out) {
    uint64_t sum = a + b;
    if (sum < a) return -1; /* umgelaufen */
    *out = sum;
    return 0;
}

int nef_validate_header(const uint8_t *buf, uint32_t buf_len, nef_header_t *out_hdr,
                         int *out_err) {
    nef_header_t hdr;
    int perr = NEF_PARSE_OK;
    if (nef_header_parse(buf, buf_len, &hdr, &perr) != 0) {
        if (out_err) *out_err = NEF_VALIDATE_ERR_PARSE;
        return -1;
    }
    if (!nef_header_version_supported(&hdr)) {
        if (out_err) *out_err = NEF_VALIDATE_ERR_VERSION;
        return -1;
    }
    if (hdr.file_size != (uint64_t)buf_len) {
        if (out_err) *out_err = NEF_VALIDATE_ERR_FILESIZE_MISMATCH;
        return -1;
    }

    /* Section-Directory-Lage. */
    uint64_t sec_dir_end;
    uint64_t sec_dir_bytes = (uint64_t)hdr.section_count * (uint64_t)NEF_SECTION_ENTRY_SIZE;
    /* section_count ist u32, NEF_SECTION_ENTRY_SIZE=48 - das Produkt
       passt immer in uint64_t ohne Ueberlauf (max 0xFFFFFFFF*48 <
       2^64), trotzdem ueber add_u64_checked zusammenfuehren statt
       direkt zu vergleichen, damit ein kuenftig groesseres Eintrags-
       format diese Pruefung nicht unbemerkt unsicher macht. */
    if (add_u64_checked(hdr.section_dir_offset, sec_dir_bytes, &sec_dir_end) != 0) {
        if (out_err) *out_err = NEF_VALIDATE_ERR_OVERFLOW;
        return -1;
    }
    if (sec_dir_end > (uint64_t)buf_len) {
        if (out_err) *out_err = NEF_VALIDATE_ERR_SECTION_DIR_RANGE;
        return -1;
    }

    /* Extension-Directory-Lage (gleiches Muster). */
    uint64_t ext_dir_end;
    uint64_t ext_dir_bytes = (uint64_t)hdr.extension_count * (uint64_t)NEF_EXTENSION_ENTRY_SIZE;
    if (add_u64_checked(hdr.extension_dir_offset, ext_dir_bytes, &ext_dir_end) != 0) {
        if (out_err) *out_err = NEF_VALIDATE_ERR_OVERFLOW;
        return -1;
    }
    if (ext_dir_end > (uint64_t)buf_len) {
        if (out_err) *out_err = NEF_VALIDATE_ERR_EXTENSION_DIR_RANGE;
        return -1;
    }

    /* Jede Section: Range+Policy, UND paarweise Ueberlappung im
       Dateibereich (O(n^2), aber section_count ist klein - siehe
       pmm_alloc_contiguous() im selben Kernel fuer ein etabliertes,
       akzeptiertes Beispiel desselben Musters bei kleinen n). Zwei
       Sections mit file_size==0 ueberlappen nie (z.B. zwei BSS-
       Deklarationen ohne Dateiinhalt) - das ist beabsichtigt. */
    for (uint32_t i = 0; i < hdr.section_count; ++i) {
        nef_section_entry_t si;
        nef_section_entry_parse(buf + hdr.section_dir_offset + (uint64_t)i * NEF_SECTION_ENTRY_SIZE, &si);
        if (nef_validate_section(&si) != 0) {
            if (out_err) *out_err = NEF_VALIDATE_ERR_SECTION_POLICY;
            return -1;
        }
        uint64_t si_end;
        if (add_u64_checked(si.file_offset, si.file_size, &si_end) != 0) {
            if (out_err) *out_err = NEF_VALIDATE_ERR_OVERFLOW;
            return -1;
        }
        if (si_end > (uint64_t)buf_len) {
            if (out_err) *out_err = NEF_VALIDATE_ERR_SECTION_RANGE;
            return -1;
        }
        if (si.file_size == 0u) continue;
        for (uint32_t j = i + 1; j < hdr.section_count; ++j) {
            nef_section_entry_t sj;
            nef_section_entry_parse(buf + hdr.section_dir_offset + (uint64_t)j * NEF_SECTION_ENTRY_SIZE, &sj);
            if (sj.file_size == 0u) continue;
            uint64_t sj_end = sj.file_offset + sj.file_size; /* bereits oben als ueberlauffrei fuer i bewiesen; fuer j identische Form, erneut unten geprueft */
            if (add_u64_checked(sj.file_offset, sj.file_size, &sj_end) != 0) {
                if (out_err) *out_err = NEF_VALIDATE_ERR_OVERFLOW;
                return -1;
            }
            int disjoint = (si_end <= sj.file_offset) || (sj_end <= si.file_offset);
            if (!disjoint) {
                if (out_err) *out_err = NEF_VALIDATE_ERR_SECTION_OVERLAP;
                return -1;
            }
        }
    }

    if (out_hdr) *out_hdr = hdr;
    if (out_err) *out_err = NEF_VALIDATE_OK;
    return 0;
}

void nef_extension_entry_serialize(const nef_extension_entry_t *in, uint8_t *out_buf) {
    put_u32(out_buf, NEF_EXT_OFF_TYPE_ID, in->type_id);
    put_u16(out_buf, NEF_EXT_OFF_VERSION, in->version);
    out_buf[NEF_EXT_OFF_REQUIRED] = in->required;
    out_buf[NEF_EXT_OFF_RESERVED] = 0u;
    put_u64(out_buf, NEF_EXT_OFF_DATA_OFFSET, in->data_offset);
    put_u64(out_buf, NEF_EXT_OFF_DATA_SIZE, in->data_size);
}

void nef_extension_entry_parse(const uint8_t *buf, nef_extension_entry_t *out) {
    out->type_id = get_u32(buf, NEF_EXT_OFF_TYPE_ID);
    out->version = get_u16(buf, NEF_EXT_OFF_VERSION);
    out->required = buf[NEF_EXT_OFF_REQUIRED];
    out->data_offset = get_u64(buf, NEF_EXT_OFF_DATA_OFFSET);
    out->data_size = get_u64(buf, NEF_EXT_OFF_DATA_SIZE);
}

/* ---- Selbsttest ------------------------------------------------------ */

int nef_self_test(void) {
    /* 1) Runder Roundtrip: Header + eine CODE-Section serialisieren,
       dann wieder validieren - muss vollstaendig gueltig sein. */
    static uint8_t img[NEF_HEADER_SIZE + NEF_SECTION_ENTRY_SIZE + 16u];
    for (uint32_t i = 0; i < sizeof(img); ++i) img[i] = 0u;

    nef_header_t hdr;
    hdr.version_major = NEF_VERSION_MAJOR_CURRENT;
    hdr.version_minor = NEF_VERSION_MINOR_CURRENT;
    hdr.header_size = NEF_HEADER_SIZE;
    hdr.file_size = sizeof(img);
    hdr.format_flags = NEF_FLAG_EXECUTABLE;
    hdr.target_arch = NEF_ARCH_X86_32;
    hdr.abi_id = NEF_ABI_NOVA_NATIVE;
    hdr.min_os_version = 0u;
    hdr.entry_point_ref = 0u;
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
    sec.file_size = 16u;
    sec.vaddr = 0x00500000u;
    sec.vsize = 16u;
    sec.alignment = 4096u;
    nef_section_entry_serialize(&sec, img + NEF_HEADER_SIZE);

    nef_header_t parsed;
    int err = NEF_VALIDATE_OK;
    if (nef_validate_header(img, sizeof(img), &parsed, &err) != 0) return -1;
    if (err != NEF_VALIDATE_OK) return -1;
    if (parsed.section_count != 1u) return -1;
    if (parsed.target_arch != NEF_ARCH_X86_32) return -1;

    nef_section_entry_t reread;
    nef_section_entry_parse(img + parsed.section_dir_offset, &reread);
    if (reread.type != NEF_SECTION_CODE) return -1;
    if (reread.vaddr != 0x00500000u) return -1;
    if (reread.alignment != 4096u) return -1;

    /* 2) Negativtest: W^X-Verstoss (WRITE+EXEC) muss abgelehnt werden. */
    nef_section_entry_t bad_sec = sec;
    bad_sec.flags = NEF_SEC_FLAG_WRITE | NEF_SEC_FLAG_EXEC;
    nef_section_entry_serialize(&bad_sec, img + NEF_HEADER_SIZE);
    err = NEF_VALIDATE_OK;
    if (nef_validate_header(img, sizeof(img), &parsed, &err) == 0) return -1;
    if (err != NEF_VALIDATE_ERR_SECTION_POLICY) return -1;
    nef_section_entry_serialize(&sec, img + NEF_HEADER_SIZE); /* zurueck auf gueltig */

    /* 3) Negativtest: kaputte Magic. */
    uint8_t saved_byte = img[0];
    img[0] = (uint8_t)(img[0] ^ 0xFFu);
    int perr = NEF_PARSE_OK;
    if (nef_header_parse(img, sizeof(img), &parsed, &perr) == 0) return -1;
    if (perr != NEF_PARSE_ERR_BAD_MAGIC) return -1;
    img[0] = saved_byte;

    /* 4) Negativtest: behauptete file_size stimmt nicht mit der echten
       Puffergroesse ueberein (muss IMMER gegen die Wahrheit, nicht nur
       gegen sich selbst geprueft werden - siehe nef_validate_header). */
    hdr.file_size = sizeof(img) - 1u;
    nef_header_serialize(&hdr, img, sizeof(img));
    err = NEF_VALIDATE_OK;
    if (nef_validate_header(img, sizeof(img), &parsed, &err) == 0) return -1;
    if (err != NEF_VALIDATE_ERR_FILESIZE_MISMATCH) return -1;
    hdr.file_size = sizeof(img);
    nef_header_serialize(&hdr, img, sizeof(img));

    /* 5) Negativtest: unbekannte Major-Version darf NIE akzeptiert
       werden (Masterauftrag Abschnitt 4). */
    hdr.version_major = (uint16_t)(NEF_VERSION_MAJOR_CURRENT + 1u);
    nef_header_serialize(&hdr, img, sizeof(img));
    err = NEF_VALIDATE_OK;
    if (nef_validate_header(img, sizeof(img), &parsed, &err) == 0) return -1;
    if (err != NEF_VALIDATE_ERR_VERSION) return -1;
    hdr.version_major = NEF_VERSION_MAJOR_CURRENT;
    nef_header_serialize(&hdr, img, sizeof(img));

    /* 6) Negativtest: Section-Directory zeigt ueber das Dateiende
       hinaus (manipulierter Offset). */
    hdr.section_dir_offset = sizeof(img); /* genau am Ende - kein Platz fuer 1 Eintrag */
    nef_header_serialize(&hdr, img, sizeof(img));
    err = NEF_VALIDATE_OK;
    if (nef_validate_header(img, sizeof(img), &parsed, &err) == 0) return -1;
    if (err != NEF_VALIDATE_ERR_SECTION_DIR_RANGE) return -1;
    hdr.section_dir_offset = NEF_HEADER_SIZE;
    nef_header_serialize(&hdr, img, sizeof(img));

    /* 7) Negativtest: Integer-Ueberlauf im Section-Directory-Offset
       (sehr grosser Offset + kleine Groesse laeuft in uint64_t um,
       wenn man es NICHT ueberlaufsicher rechnet - hier absichtlich ein
       Offset nahe UINT64_MAX). */
    hdr.section_dir_offset = 0xFFFFFFFFFFFFFFFFull - 4u; /* nahe am Umlauf */
    nef_header_serialize(&hdr, img, sizeof(img));
    err = NEF_VALIDATE_OK;
    if (nef_validate_header(img, sizeof(img), &parsed, &err) == 0) return -1;
    if (err != NEF_VALIDATE_ERR_OVERFLOW && err != NEF_VALIDATE_ERR_SECTION_DIR_RANGE) return -1;
    hdr.section_dir_offset = NEF_HEADER_SIZE;
    nef_header_serialize(&hdr, img, sizeof(img));

    /* 8) Negativtest: zwei ueberlappende Sections im Dateibereich. */
    {
        static uint8_t img2[NEF_HEADER_SIZE + NEF_SECTION_ENTRY_SIZE * 2u + 32u];
        for (uint32_t i = 0; i < sizeof(img2); ++i) img2[i] = 0u;
        nef_header_t h2 = hdr;
        h2.file_size = sizeof(img2);
        h2.section_dir_offset = NEF_HEADER_SIZE;
        h2.section_count = 2u;
        h2.extension_dir_offset = sizeof(img2);
        h2.extension_count = 0u;
        nef_header_serialize(&h2, img2, sizeof(img2));

        nef_section_entry_t a = sec;
        a.flags = NEF_SEC_FLAG_READ;
        a.file_offset = NEF_HEADER_SIZE + NEF_SECTION_ENTRY_SIZE * 2u;
        a.file_size = 16u;
        a.vsize = 16u;
        nef_section_entry_t b = a;
        b.file_offset = a.file_offset + 8u; /* ueberlappt a um 8 Byte */
        nef_section_entry_serialize(&a, img2 + NEF_HEADER_SIZE);
        nef_section_entry_serialize(&b, img2 + NEF_HEADER_SIZE + NEF_SECTION_ENTRY_SIZE);

        nef_header_t p2;
        int err2 = NEF_VALIDATE_OK;
        if (nef_validate_header(img2, sizeof(img2), &p2, &err2) == 0) return -1;
        if (err2 != NEF_VALIDATE_ERR_SECTION_OVERLAP) return -1;
    }

    return 0;
}
