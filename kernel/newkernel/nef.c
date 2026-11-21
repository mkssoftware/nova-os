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

/* ---- Signaturen (Phase 5) - echtes SHA-256/HMAC-SHA-256 -------------- */

/* Fester Dev-Schluessel, siehe nef.h fuer die ehrliche Einordnung
   (symmetrisch, oeffentlich im Binary sichtbar, KEINE PKI). 32 Byte
   reiner Zufallswert aus dieser Sitzung - jeder gebaute Kernel/
   `nef.exe` MUSS denselben Wert verwenden, sonst scheitert jede
   Signaturpruefung bereits am naechsten, unveraenderten Build. */
const uint8_t NEF_DEV_SIGNING_KEY[32] = {
    0x4e,0x45,0x46,0x31,0x2d,0x44,0x45,0x56,0x2d,0x53,0x49,0x47,0x4e,0x2d,0x4b,0x45,
    0x59,0x2d,0x32,0x30,0x32,0x36,0x2d,0x31,0x31,0x2d,0x32,0x30,0x2d,0x01,0x02,0x03
};

static inline uint32_t sha256_rotr(uint32_t x, uint32_t n) {
    return (x >> n) | (x << (32u - n));
}

static const uint32_t sha256_k[64] = {
    0x428a2f98u,0x71374491u,0xb5c0fbcfu,0xe9b5dba5u,0x3956c25bu,0x59f111f1u,0x923f82a4u,0xab1c5ed5u,
    0xd807aa98u,0x12835b01u,0x243185beu,0x550c7dc3u,0x72be5d74u,0x80deb1feu,0x9bdc06a7u,0xc19bf174u,
    0xe49b69c1u,0xefbe4786u,0x0fc19dc6u,0x240ca1ccu,0x2de92c6fu,0x4a7484aau,0x5cb0a9dcu,0x76f988dau,
    0x983e5152u,0xa831c66du,0xb00327c8u,0xbf597fc7u,0xc6e00bf3u,0xd5a79147u,0x06ca6351u,0x14292967u,
    0x27b70a85u,0x2e1b2138u,0x4d2c6dfcu,0x53380d13u,0x650a7354u,0x766a0abbu,0x81c2c92eu,0x92722c85u,
    0xa2bfe8a1u,0xa81a664bu,0xc24b8b70u,0xc76c51a3u,0xd192e819u,0xd6990624u,0xf40e3585u,0x106aa070u,
    0x19a4c116u,0x1e376c08u,0x2748774cu,0x34b0bcb5u,0x391c0cb3u,0x4ed8aa4au,0x5b9cca4fu,0x682e6ff3u,
    0x748f82eeu,0x78a5636fu,0x84c87814u,0x8cc70208u,0x90befffau,0xa4506cebu,0xbef9a3f7u,0xc67178f2u
};

/* Eine 64-Byte-Blockkompression (FIPS 180-4 Abschnitt 6.2.2). */
static void sha256_compress(uint32_t h[8], const uint8_t block[64]) {
    uint32_t w[64];
    for (uint32_t i = 0; i < 16u; ++i) {
        w[i] = ((uint32_t)block[i * 4u] << 24) | ((uint32_t)block[i * 4u + 1u] << 16) |
               ((uint32_t)block[i * 4u + 2u] << 8) | (uint32_t)block[i * 4u + 3u];
    }
    for (uint32_t i = 16; i < 64u; ++i) {
        uint32_t s0 = sha256_rotr(w[i - 15u], 7u) ^ sha256_rotr(w[i - 15u], 18u) ^ (w[i - 15u] >> 3u);
        uint32_t s1 = sha256_rotr(w[i - 2u], 17u) ^ sha256_rotr(w[i - 2u], 19u) ^ (w[i - 2u] >> 10u);
        w[i] = w[i - 16u] + s0 + w[i - 7u] + s1;
    }

    uint32_t a = h[0], b = h[1], c = h[2], d = h[3];
    uint32_t e = h[4], f = h[5], g = h[6], hh = h[7];
    for (uint32_t i = 0; i < 64u; ++i) {
        uint32_t s1 = sha256_rotr(e, 6u) ^ sha256_rotr(e, 11u) ^ sha256_rotr(e, 25u);
        uint32_t ch = (e & f) ^ (~e & g);
        uint32_t temp1 = hh + s1 + ch + sha256_k[i] + w[i];
        uint32_t s0 = sha256_rotr(a, 2u) ^ sha256_rotr(a, 13u) ^ sha256_rotr(a, 22u);
        uint32_t maj = (a & b) ^ (a & c) ^ (b & c);
        uint32_t temp2 = s0 + maj;
        hh = g; g = f; f = e; e = d + temp1;
        d = c; c = b; b = a; a = temp1 + temp2;
    }

    h[0] += a; h[1] += b; h[2] += c; h[3] += d;
    h[4] += e; h[5] += f; h[6] += g; h[7] += hh;
}

/* Streaming-Zustand bewusst NICHT oeffentlich (nef.h braucht nur den
   Einzelaufruf-Wrapper nef_sha256() plus HMAC) - haelt den vollen
   Nachrichteninhalt NIE auf einmal im Speicher (kein kalloc()/variable
   Stack-Allokation fuer beliebig grosse Eingaben noetig). */
typedef struct {
    uint32_t h[8];
    uint8_t buffer[64];
    uint32_t buffer_len;
    uint64_t total_len;
} sha256_ctx_t;

static void sha256_init(sha256_ctx_t *ctx) {
    ctx->h[0] = 0x6a09e667u; ctx->h[1] = 0xbb67ae85u; ctx->h[2] = 0x3c6ef372u; ctx->h[3] = 0xa54ff53au;
    ctx->h[4] = 0x510e527fu; ctx->h[5] = 0x9b05688cu; ctx->h[6] = 0x1f83d9abu; ctx->h[7] = 0x5be0cd19u;
    ctx->buffer_len = 0u;
    ctx->total_len = 0u;
}

static void sha256_update(sha256_ctx_t *ctx, const uint8_t *data, uint32_t len) {
    ctx->total_len += (uint64_t)len;
    uint32_t offset = 0u;

    if (ctx->buffer_len > 0u) {
        uint32_t need = 64u - ctx->buffer_len;
        uint32_t take = (len < need) ? len : need;
        for (uint32_t i = 0; i < take; ++i) ctx->buffer[ctx->buffer_len + i] = data[i];
        ctx->buffer_len += take;
        offset += take;
        if (ctx->buffer_len == 64u) {
            sha256_compress(ctx->h, ctx->buffer);
            ctx->buffer_len = 0u;
        }
    }

    while (offset + 64u <= len) {
        sha256_compress(ctx->h, data + offset);
        offset += 64u;
    }

    uint32_t remaining = len - offset;
    for (uint32_t i = 0; i < remaining; ++i) ctx->buffer[ctx->buffer_len + i] = data[offset + i];
    ctx->buffer_len += remaining;
}

static void sha256_final(sha256_ctx_t *ctx, uint8_t out[NEF_SIGNATURE_DIGEST_SIZE]) {
    uint8_t pad[128];
    for (uint32_t i = 0; i < 128u; ++i) pad[i] = 0u;
    uint32_t pad_len = ctx->buffer_len;
    for (uint32_t i = 0; i < pad_len; ++i) pad[i] = ctx->buffer[i];
    pad[pad_len] = 0x80u;

    uint32_t blocks = (pad_len < 56u) ? 1u : 2u;
    uint64_t bit_len = ctx->total_len * 8u; /* Multiplikation, KEINE Division */
    uint32_t len_off = blocks * 64u - 8u;
    for (uint32_t i = 0; i < 8u; ++i) {
        pad[len_off + i] = (uint8_t)(bit_len >> (56u - 8u * i));
    }
    for (uint32_t i = 0; i < blocks; ++i) sha256_compress(ctx->h, pad + i * 64u);

    for (uint32_t i = 0; i < 8u; ++i) {
        out[i * 4u] = (uint8_t)(ctx->h[i] >> 24);
        out[i * 4u + 1u] = (uint8_t)(ctx->h[i] >> 16);
        out[i * 4u + 2u] = (uint8_t)(ctx->h[i] >> 8);
        out[i * 4u + 3u] = (uint8_t)(ctx->h[i]);
    }
}

void nef_sha256(const uint8_t *data, uint32_t len, uint8_t out[NEF_SIGNATURE_DIGEST_SIZE]) {
    sha256_ctx_t ctx;
    sha256_init(&ctx);
    sha256_update(&ctx, data, len);
    sha256_final(&ctx, out);
}

void nef_hmac_sha256(const uint8_t *key, uint32_t key_len, const uint8_t *data, uint32_t len,
                      uint8_t out[NEF_SIGNATURE_DIGEST_SIZE]) {
    uint8_t key_block[64];
    for (uint32_t i = 0; i < 64u; ++i) key_block[i] = 0u;
    if (key_len > 64u) {
        nef_sha256(key, key_len, key_block); /* fuellt die ersten 32 Byte, Rest bleibt 0 */
    } else {
        for (uint32_t i = 0; i < key_len; ++i) key_block[i] = key[i];
    }

    uint8_t ipad[64], opad[64];
    for (uint32_t i = 0; i < 64u; ++i) {
        ipad[i] = (uint8_t)(key_block[i] ^ 0x36u);
        opad[i] = (uint8_t)(key_block[i] ^ 0x5cu);
    }

    uint8_t inner[NEF_SIGNATURE_DIGEST_SIZE];
    sha256_ctx_t ctx;
    sha256_init(&ctx);
    sha256_update(&ctx, ipad, 64u);
    sha256_update(&ctx, data, len);
    sha256_final(&ctx, inner);

    sha256_init(&ctx);
    sha256_update(&ctx, opad, 64u);
    sha256_update(&ctx, inner, NEF_SIGNATURE_DIGEST_SIZE);
    sha256_final(&ctx, out);
}

void nef_compute_signature(const uint8_t *buf, uint32_t signed_len,
                            uint8_t out[NEF_SIGNATURE_DIGEST_SIZE]) {
    nef_hmac_sha256(NEF_DEV_SIGNING_KEY, sizeof(NEF_DEV_SIGNING_KEY), buf, signed_len, out);
}

int nef_verify_signature(const uint8_t *buf, uint32_t buf_len, const nef_header_t *hdr,
                          const nef_section_entry_t *sections, uint32_t section_count) {
    if ((hdr->format_flags & NEF_FLAG_SIGNED) == 0u) return NEF_SIG_ERR_NOT_SIGNED;

    for (uint32_t i = 0; i < section_count; ++i) {
        if (sections[i].type != NEF_SECTION_SIGNATURES) continue;
        if (sections[i].file_size != NEF_SIGNATURE_DIGEST_SIZE) continue;
        /* Bereits durch nef_validate_header() ueberlaufsicher gegen
           buf_len geprueft - direkter Zugriff hier ist sicher. */
        uint32_t sig_offset = (uint32_t)sections[i].file_offset;
        if ((uint64_t)sig_offset + NEF_SIGNATURE_DIGEST_SIZE > buf_len) continue;

        uint8_t expected[NEF_SIGNATURE_DIGEST_SIZE];
        nef_compute_signature(buf, sig_offset, expected);

        int mismatch = 0;
        for (uint32_t b = 0; b < NEF_SIGNATURE_DIGEST_SIZE; ++b) {
            if (buf[sig_offset + b] != expected[b]) { mismatch = 1; break; }
        }
        return mismatch ? NEF_SIG_ERR_MISMATCH : NEF_SIG_OK;
    }
    return NEF_SIG_ERR_NO_SECTION;
}

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

    /* 9) Signatur-Rundweg: Image signieren, Verifikation muss gueltig
       sein; eine einzelne verfaelschte Byte VOR der Signatur-Section
       MUSS die Verifikation zum Scheitern bringen. */
    {
        static uint8_t simg[NEF_HEADER_SIZE + NEF_SECTION_ENTRY_SIZE * 2u + 16u +
                             NEF_SIGNATURE_DIGEST_SIZE];
        for (uint32_t i = 0; i < sizeof(simg); ++i) simg[i] = 0u;

        nef_header_t sh;
        sh.version_major = NEF_VERSION_MAJOR_CURRENT;
        sh.version_minor = NEF_VERSION_MINOR_CURRENT;
        sh.header_size = NEF_HEADER_SIZE;
        sh.file_size = sizeof(simg);
        sh.format_flags = NEF_FLAG_EXECUTABLE | NEF_FLAG_SIGNED;
        sh.target_arch = NEF_ARCH_X86_32;
        sh.abi_id = NEF_ABI_NOVA_NATIVE;
        sh.min_os_version = 0u;
        sh.entry_point_ref = 0x00500000u;
        sh.section_dir_offset = NEF_HEADER_SIZE;
        sh.section_count = 2u;
        sh.extension_dir_offset = NEF_HEADER_SIZE + NEF_SECTION_ENTRY_SIZE * 2u + 16u +
                                   NEF_SIGNATURE_DIGEST_SIZE;
        sh.extension_count = 0u;
        sh.manifest_ref = 0u;
        sh.signature_dir_ref = 0u;
        sh.integrity_ref = 0u;
        if (nef_header_serialize(&sh, simg, sizeof(simg)) != 0) return -1;

        nef_section_entry_t code_sec;
        code_sec.type = NEF_SECTION_CODE;
        code_sec.flags = NEF_SEC_FLAG_READ | NEF_SEC_FLAG_EXEC;
        code_sec.file_offset = NEF_HEADER_SIZE + NEF_SECTION_ENTRY_SIZE * 2u;
        code_sec.file_size = 16u;
        code_sec.vaddr = 0x00500000u;
        code_sec.vsize = 16u;
        code_sec.alignment = 4096u;
        nef_section_entry_serialize(&code_sec, simg + NEF_HEADER_SIZE);

        nef_section_entry_t sig_sec;
        sig_sec.type = NEF_SECTION_SIGNATURES;
        sig_sec.flags = NEF_SEC_FLAG_READ;
        sig_sec.file_offset = code_sec.file_offset + code_sec.file_size;
        sig_sec.file_size = NEF_SIGNATURE_DIGEST_SIZE;
        sig_sec.vaddr = 0u;
        sig_sec.vsize = NEF_SIGNATURE_DIGEST_SIZE;
        sig_sec.alignment = 1u;
        nef_section_entry_serialize(&sig_sec, simg + NEF_HEADER_SIZE + NEF_SECTION_ENTRY_SIZE);

        uint8_t digest[NEF_SIGNATURE_DIGEST_SIZE];
        nef_compute_signature(simg, (uint32_t)sig_sec.file_offset, digest);
        for (uint32_t i = 0; i < NEF_SIGNATURE_DIGEST_SIZE; ++i) {
            simg[(uint32_t)sig_sec.file_offset + i] = digest[i];
        }

        nef_header_t sparsed;
        int serr = NEF_VALIDATE_OK;
        if (nef_validate_header(simg, sizeof(simg), &sparsed, &serr) != 0) return -1;
        if (serr != NEF_VALIDATE_OK) return -1;

        nef_section_entry_t ssecs[2];
        nef_section_entry_parse(simg + (uint32_t)sparsed.section_dir_offset, &ssecs[0]);
        nef_section_entry_parse(simg + (uint32_t)sparsed.section_dir_offset + NEF_SECTION_ENTRY_SIZE,
                                 &ssecs[1]);
        if (nef_verify_signature(simg, sizeof(simg), &sparsed, ssecs, sparsed.section_count) !=
            NEF_SIG_OK) {
            return -1;
        }

        /* Negativtest: ein Byte VOR der Signatur-Section verfaelschen -
           muss die Verifikation zum Scheitern bringen. */
        simg[0] ^= 0xFFu; /* Teil des Magic, aber VOR sig_sec.file_offset */
        if (nef_verify_signature(simg, sizeof(simg), &sparsed, ssecs, sparsed.section_count) ==
            NEF_SIG_OK) {
            return -1;
        }
        simg[0] ^= 0xFFu; /* zurueck auf gueltig */
        if (nef_verify_signature(simg, sizeof(simg), &sparsed, ssecs, sparsed.section_count) !=
            NEF_SIG_OK) {
            return -1;
        }
    }

    /* 10) SHA-256/HMAC-SHA-256 gegen bekannte, oeffentliche Testvektoren
       (FIPS 180-4 Anhang B.1 bzw. RFC 4231 Testfall 1) - unabhaengig von
       der NEF-eigenen Signaturlogik oben, prueft NUR die Primitiven
       selbst. */
    {
        static const uint8_t expected_empty[NEF_SIGNATURE_DIGEST_SIZE] = {
            0xe3,0xb0,0xc4,0x42,0x98,0xfc,0x1c,0x14,0x9a,0xfb,0xf4,0xc8,0x99,0x6f,0xb9,0x24,
            0x27,0xae,0x41,0xe4,0x64,0x9b,0x93,0x4c,0xa4,0x95,0x99,0x1b,0x78,0x52,0xb8,0x55
        };
        uint8_t got[NEF_SIGNATURE_DIGEST_SIZE];
        nef_sha256((const uint8_t *)"", 0u, got);
        for (uint32_t i = 0; i < NEF_SIGNATURE_DIGEST_SIZE; ++i) {
            if (got[i] != expected_empty[i]) return -1;
        }

        static const uint8_t expected_abc[NEF_SIGNATURE_DIGEST_SIZE] = {
            0xba,0x78,0x16,0xbf,0x8f,0x01,0xcf,0xea,0x41,0x41,0x40,0xde,0x5d,0xae,0x22,0x23,
            0xb0,0x03,0x61,0xa3,0x96,0x17,0x7a,0x9c,0xb4,0x10,0xff,0x61,0xf2,0x00,0x15,0xad
        };
        nef_sha256((const uint8_t *)"abc", 3u, got);
        for (uint32_t i = 0; i < NEF_SIGNATURE_DIGEST_SIZE; ++i) {
            if (got[i] != expected_abc[i]) return -1;
        }

        /* RFC 4231 Testfall 1: Key = 20 Byte 0x0b, Data = "Hi There". */
        static const uint8_t hmac_key[20] = {
            0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,
            0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b
        };
        static const uint8_t expected_hmac[NEF_SIGNATURE_DIGEST_SIZE] = {
            0xb0,0x34,0x4c,0x61,0xd8,0xdb,0x38,0x53,0x5c,0xa8,0xaf,0xce,0xaf,0x0b,0xf1,0x2b,
            0x88,0x1d,0xc2,0x00,0xc9,0x83,0x3d,0xa7,0x26,0xe9,0x37,0x6c,0x2e,0x32,0xcf,0xf7
        };
        nef_hmac_sha256(hmac_key, sizeof(hmac_key), (const uint8_t *)"Hi There", 8u, got);
        for (uint32_t i = 0; i < NEF_SIGNATURE_DIGEST_SIZE; ++i) {
            if (got[i] != expected_hmac[i]) return -1;
        }
    }

    return 0;
}
