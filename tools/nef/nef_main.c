/*
 * NovaOS NEF 1.0 Host-Werkzeug (Phase 3 des NEF-Masterauftrags).
 *
 *   nef build    <output.nef> <code.bin> [--entry N] [--vaddr N]
 *                [--arch x86_32|x86_64|arm64] [--flags exec,lib,sysmod,...]
 *       Bequemer Spezialfall von "link" fuer den haeufigsten Fall: GENAU
 *       eine CODE-Section (READ|EXEC) aus einer rohen Binaerdatei (z.B.
 *       per objcopy -O binary erzeugt, identisches Muster wie die
 *       bestehenden ushell-Blobs im Kernel-Baum).
 *
 *   nef link     <output.nef> --section TYP:FLAGS:VADDR:ALIGN:DATEI[:VSIZE]
 *                [--section ...] [--entry N] [--arch ...] [--abi nova]
 *                [--flags ...] [--min-os N]
 *       Allgemeiner Fall: beliebig viele Sections aus rohen Datei-
 *       Fragmenten zusammensetzen (Masterauftrag Abschnitt 15 - Code/
 *       Daten/Imports/Exports/Relokationen verknuepfen; die Erzeugung der
 *       einzelnen Fragmente selbst bleibt Aufgabe eines echten Compiler-
 *       Backends, siehe Dateikopf-Hinweis zur Uebergangsphase).
 *
 *   nef validate <input.nef>
 *       Volle strukturelle Validierung (nef_validate_header) - Exit-Code
 *       0 = gueltig, 1 = ungueltig (mit Fehlercode auf stderr).
 *
 *   nef inspect  <input.nef>
 *       Menschlich lesbare Zusammenfassung: Header, Section-Directory,
 *       Extension-Directory.
 *
 *   nef dump     <input.nef>
 *       Wie inspect, zusaetzlich ein Hex-Dump der ersten 96 Header-Byte -
 *       fuer Low-Level-Diagnose (Masterauftrag Abschnitt 15, nef-dump).
 *
 *   nef sign/verify <input.nef> ...
 *       NOCH NICHT IMPLEMENTIERT (Phase 5 des Masterauftrags - echte
 *       kryptografische Signaturen brauchen zuerst die Signature-Block-
 *       Spezifikation + eine Pruefung der im Kernel verfuegbaren crypto.h-
 *       Primitive). Meldet das ehrlich (Exit-Code 2), TAEUSCHT KEINEN
 *       Erfolg vor (siehe Masterauftrag Abschnitt 19: "keine
 *       vorgetaeuschten Implementierungen").
 *
 *   nef test
 *       Fuehrt nef_self_test() (identische Pruefung wie im Kernel-
 *       Selbsttest, siehe nef.c) PLUS einen eigenstaendigen Build->
 *       Validate->Inspect-Rundweg ueber dieses Werkzeug selbst aus.
 *
 * Baubar auf dem Windows-Entwicklungssystem mit einem gewoehnlichen
 * hosted C-Compiler (z.B. MinGW gcc) - KEIN freistehender Kernel-Build,
 * dieses Programm laeuft auf dem Host, nicht in NovaOS selbst. Es
 * verlinkt direkt gegen kernel/newkernel/nef.c (siehe Build-Hinweis am
 * Dateiende) - nef.c ist bewusst reines, portables C89/C99 ohne Kernel-
 * Abhaengigkeiten (nur <stdint.h>), lief deshalb unveraendert auch hier.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "nef.h"

static void die(const char *msg) {
    fprintf(stderr, "nef: Fehler: %s\n", msg);
    exit(1);
}

static uint8_t *read_file(const char *path, long *out_len) {
    FILE *f = fopen(path, "rb");
    if (!f) { fprintf(stderr, "nef: Fehler: kann '%s' nicht lesen\n", path); exit(1); }
    if (fseek(f, 0, SEEK_END) != 0) die("fseek fehlgeschlagen");
    long len = ftell(f);
    if (len < 0) die("ftell fehlgeschlagen");
    if (fseek(f, 0, SEEK_SET) != 0) die("fseek fehlgeschlagen");
    uint8_t *buf = (uint8_t *)malloc((size_t)len > 0 ? (size_t)len : 1);
    if (!buf) die("Speicher reicht nicht");
    if (len > 0 && fread(buf, 1, (size_t)len, f) != (size_t)len) die("Lesen unvollstaendig");
    fclose(f);
    *out_len = len;
    return buf;
}

static void write_file(const char *path, const uint8_t *buf, long len) {
    FILE *f = fopen(path, "wb");
    if (!f) { fprintf(stderr, "nef: Fehler: kann '%s' nicht schreiben\n", path); exit(1); }
    if (len > 0 && fwrite(buf, 1, (size_t)len, f) != (size_t)len) die("Schreiben unvollstaendig");
    fclose(f);
}

static uint64_t parse_u64(const char *s) {
    return (uint64_t)strtoull(s, NULL, 0); /* "0x..." -> hex, sonst dezimal (Basis 0) */
}

static uint32_t parse_arch(const char *s) {
    if (strcmp(s, "x86_32") == 0) return NEF_ARCH_X86_32;
    if (strcmp(s, "x86_64") == 0) return NEF_ARCH_X86_64;
    if (strcmp(s, "arm64") == 0) return NEF_ARCH_ARM64;
    fprintf(stderr, "nef: Fehler: unbekannte Architektur '%s' (erwartet x86_32/x86_64/arm64)\n", s);
    exit(1);
}

static uint32_t parse_flags(const char *s) {
    uint32_t flags = 0;
    char buf[256];
    size_t n = strlen(s);
    if (n >= sizeof(buf)) die("--flags zu lang");
    memcpy(buf, s, n + 1);
    char *tok = strtok(buf, ",");
    while (tok) {
        if (strcmp(tok, "exec") == 0) flags |= NEF_FLAG_EXECUTABLE;
        else if (strcmp(tok, "lib") == 0) flags |= NEF_FLAG_DYNAMIC_LIBRARY;
        else if (strcmp(tok, "sysmod") == 0) flags |= NEF_FLAG_SYSTEM_MODULE;
        else if (strcmp(tok, "signed") == 0) flags |= NEF_FLAG_SIGNED;
        else if (strcmp(tok, "ir") == 0) flags |= NEF_FLAG_HAS_NOVA_IR;
        else if (strcmp(tok, "aot") == 0) flags |= NEF_FLAG_AOT_COMPILED;
        else { fprintf(stderr, "nef: Fehler: unbekanntes Flag '%s'\n", tok); exit(1); }
        tok = strtok(NULL, ",");
    }
    return flags;
}

static const char *section_type_name(uint32_t type) {
    switch (type) {
        case NEF_SECTION_NULL: return "NULL";
        case NEF_SECTION_CODE: return "CODE";
        case NEF_SECTION_RODATA: return "RODATA";
        case NEF_SECTION_DATA: return "DATA";
        case NEF_SECTION_BSS: return "BSS";
        case NEF_SECTION_IMPORTS: return "IMPORTS";
        case NEF_SECTION_EXPORTS: return "EXPORTS";
        case NEF_SECTION_RELOCATIONS: return "RELOCATIONS";
        case NEF_SECTION_TLS: return "TLS";
        case NEF_SECTION_RESOURCES: return "RESOURCES";
        case NEF_SECTION_MANIFEST: return "MANIFEST";
        case NEF_SECTION_CAPABILITIES: return "CAPABILITIES";
        case NEF_SECTION_SIGNATURES: return "SIGNATURES";
        case NEF_SECTION_DEBUG: return "DEBUG";
        case NEF_SECTION_OPTIMIZATION: return "OPTIMIZATION";
        case NEF_SECTION_NOVA_IR: return "NOVA_IR";
        case NEF_SECTION_ARCH_VARIANTS: return "ARCH_VARIANTS";
        case NEF_SECTION_EXTENSIONS: return "EXTENSIONS";
        default: return "?";
    }
}

static const char *arch_name(uint32_t a) {
    switch (a) {
        case NEF_ARCH_X86_32: return "x86_32";
        case NEF_ARCH_X86_64: return "x86_64 (NICHT ausfuehrbar in diesem NovaOS-Kernel)";
        case NEF_ARCH_ARM64: return "arm64 (NICHT ausfuehrbar in diesem NovaOS-Kernel)";
        default: return "unbekannt";
    }
}

static int validate_err_name_print(int err) {
    const char *name;
    switch (err) {
        case NEF_VALIDATE_ERR_PARSE: name = "PARSE (Header unlesbar/Magic falsch/Groesse unplausibel)"; break;
        case NEF_VALIDATE_ERR_VERSION: name = "VERSION (unbekannte/inkompatible Major-Version)"; break;
        case NEF_VALIDATE_ERR_FILESIZE_MISMATCH: name = "FILESIZE_MISMATCH (file_size != echte Dateigroesse)"; break;
        case NEF_VALIDATE_ERR_SECTION_DIR_RANGE: name = "SECTION_DIR_RANGE"; break;
        case NEF_VALIDATE_ERR_EXTENSION_DIR_RANGE: name = "EXTENSION_DIR_RANGE"; break;
        case NEF_VALIDATE_ERR_SECTION_RANGE: name = "SECTION_RANGE (Section zeigt ausserhalb der Datei)"; break;
        case NEF_VALIDATE_ERR_SECTION_POLICY: name = "SECTION_POLICY (z.B. W^X-Verstoss)"; break;
        case NEF_VALIDATE_ERR_SECTION_OVERLAP: name = "SECTION_OVERLAP"; break;
        case NEF_VALIDATE_ERR_OVERFLOW: name = "OVERFLOW (Integer-Ueberlauf in Offset/Groessen-Rechnung)"; break;
        default: name = "unbekannt"; break;
    }
    printf("%s\n", name);
    return err;
}

/* ---- link/build: gemeinsamer Assembler ------------------------------ */

#define MAX_SECTIONS 32

typedef struct {
    nef_section_entry_t entry;
    uint8_t *data;
    long data_len;
} pending_section_t;

static void assemble_and_write(const char *out_path, pending_section_t *secs, int sec_count,
                                uint64_t entry_point_ref, uint32_t arch, uint32_t abi,
                                uint32_t format_flags, uint32_t min_os_version) {
    if (sec_count <= 0 || sec_count > MAX_SECTIONS) die("ungueltige Anzahl Sections");

    /* Layout: Header | Section-Directory | Extension-Directory (leer,
       Phase 3 erzeugt noch keine Erweiterungen) | Section-Daten
       nacheinander, jede 16-Byte-ausgerichtet (willkuerlich gewaehlte,
       aber feste Dateiausrichtung - NICHT zu verwechseln mit der
       Section-eigenen virtuellen alignment). */
    uint64_t section_dir_offset = NEF_HEADER_SIZE;
    uint64_t extension_dir_offset = section_dir_offset + (uint64_t)sec_count * NEF_SECTION_ENTRY_SIZE;
    uint64_t cursor = extension_dir_offset; /* extension_count=0, belegt daher 0 Byte */
    for (int i = 0; i < sec_count; ++i) {
        cursor = (cursor + 15u) & ~(uint64_t)15u;
        secs[i].entry.file_offset = (secs[i].data_len > 0) ? cursor : 0u;
        if (secs[i].data_len > 0) cursor += (uint64_t)secs[i].data_len;
    }
    uint64_t file_size = cursor;

    uint8_t *img = (uint8_t *)calloc(1, (size_t)file_size);
    if (!img) die("Speicher reicht nicht fuer das Ausgabeabbild");

    nef_header_t hdr;
    hdr.version_major = NEF_VERSION_MAJOR_CURRENT;
    hdr.version_minor = NEF_VERSION_MINOR_CURRENT;
    hdr.header_size = NEF_HEADER_SIZE;
    hdr.file_size = file_size;
    hdr.format_flags = format_flags;
    hdr.target_arch = arch;
    hdr.abi_id = abi;
    hdr.min_os_version = min_os_version;
    hdr.entry_point_ref = entry_point_ref;
    hdr.section_dir_offset = section_dir_offset;
    hdr.section_count = (uint32_t)sec_count;
    hdr.extension_dir_offset = extension_dir_offset;
    hdr.extension_count = 0u;
    hdr.manifest_ref = 0u;
    hdr.signature_dir_ref = 0u;
    hdr.integrity_ref = 0u;
    if (nef_header_serialize(&hdr, img, (uint32_t)file_size) != 0) die("Header-Serialisierung fehlgeschlagen");

    for (int i = 0; i < sec_count; ++i) {
        secs[i].entry.file_size = (uint64_t)secs[i].data_len;
        if (secs[i].entry.vsize < secs[i].entry.file_size) secs[i].entry.vsize = secs[i].entry.file_size;
        if (nef_validate_section(&secs[i].entry) != 0) {
            fprintf(stderr, "nef: Fehler: Section %d (%s) verstoesst gegen die Formatrichtlinie"
                            " (z.B. WRITE+EXEC gleichzeitig - siehe Masterauftrag Abschnitt 5/13)\n",
                    i, section_type_name(secs[i].entry.type));
            exit(1);
        }
        nef_section_entry_serialize(&secs[i].entry, img + section_dir_offset + (uint64_t)i * NEF_SECTION_ENTRY_SIZE);
        if (secs[i].data_len > 0) {
            memcpy(img + secs[i].entry.file_offset, secs[i].data, (size_t)secs[i].data_len);
        }
    }

    nef_header_t check_hdr;
    int err = NEF_VALIDATE_OK;
    if (nef_validate_header(img, (uint32_t)file_size, &check_hdr, &err) != 0) {
        fprintf(stderr, "nef: Fehler: selbst erzeugtes Abbild ist nicht gueltig (Fehlercode %d) -"
                        " dies ist ein Bug im Werkzeug, bitte melden\n", err);
        free(img);
        exit(1);
    }

    write_file(out_path, img, (long)file_size);
    free(img);
    printf("nef: '%s' geschrieben (%lu Byte, %d Section(s))\n", out_path, (unsigned long)file_size, sec_count);
}

/* "TYP:FLAGS:VADDR:ALIGN:DATEI[:VSIZE]" - TYP/FLAGS als Name-Listen wie
   section_type_name()/parse_flags() liest (aber eigene kleine Flag-Namen
   fuer Speicherzugriffsrechte, nicht die format_flags-Namen). */
static uint32_t parse_sec_type(const char *s) {
    if (strcmp(s, "CODE") == 0) return NEF_SECTION_CODE;
    if (strcmp(s, "RODATA") == 0) return NEF_SECTION_RODATA;
    if (strcmp(s, "DATA") == 0) return NEF_SECTION_DATA;
    if (strcmp(s, "BSS") == 0) return NEF_SECTION_BSS;
    if (strcmp(s, "IMPORTS") == 0) return NEF_SECTION_IMPORTS;
    if (strcmp(s, "EXPORTS") == 0) return NEF_SECTION_EXPORTS;
    if (strcmp(s, "RELOCATIONS") == 0) return NEF_SECTION_RELOCATIONS;
    if (strcmp(s, "TLS") == 0) return NEF_SECTION_TLS;
    if (strcmp(s, "RESOURCES") == 0) return NEF_SECTION_RESOURCES;
    if (strcmp(s, "MANIFEST") == 0) return NEF_SECTION_MANIFEST;
    if (strcmp(s, "CAPABILITIES") == 0) return NEF_SECTION_CAPABILITIES;
    if (strcmp(s, "SIGNATURES") == 0) return NEF_SECTION_SIGNATURES;
    if (strcmp(s, "DEBUG") == 0) return NEF_SECTION_DEBUG;
    if (strcmp(s, "OPTIMIZATION") == 0) return NEF_SECTION_OPTIMIZATION;
    if (strcmp(s, "NOVA_IR") == 0) return NEF_SECTION_NOVA_IR;
    if (strcmp(s, "ARCH_VARIANTS") == 0) return NEF_SECTION_ARCH_VARIANTS;
    if (strcmp(s, "EXTENSIONS") == 0) return NEF_SECTION_EXTENSIONS;
    fprintf(stderr, "nef: Fehler: unbekannter Section-Typ '%s'\n", s);
    exit(1);
}

static uint32_t parse_sec_flags(const char *s) {
    uint32_t f = 0;
    for (const char *p = s; *p; ++p) {
        switch (*p) {
            case 'r': f |= NEF_SEC_FLAG_READ; break;
            case 'w': f |= NEF_SEC_FLAG_WRITE; break;
            case 'x': f |= NEF_SEC_FLAG_EXEC; break;
            case 'c': f |= NEF_SEC_FLAG_COMPRESSED; break;
            case 's': f |= NEF_SEC_FLAG_SHARED; break;
            default:
                fprintf(stderr, "nef: Fehler: unbekanntes Section-Flag-Zeichen '%c'"
                                " (erwartet r/w/x/c/s)\n", *p);
                exit(1);
        }
    }
    return f;
}

static void parse_section_spec(const char *spec, pending_section_t *out) {
    char buf[1024];
    size_t n = strlen(spec);
    if (n >= sizeof(buf)) die("--section-Angabe zu lang");
    memcpy(buf, spec, n + 1);

    char *parts[6] = {0};
    int count = 0;
    char *tok = buf;
    char *p = buf;
    while (count < 6) {
        char *colon = strchr(p, ':');
        parts[count++] = tok = p;
        if (!colon) break;
        *colon = '\0';
        p = colon + 1;
        tok = p;
    }
    if (count < 5) die("--section braucht TYP:FLAGS:VADDR:ALIGN:DATEI[:VSIZE]");

    memset(&out->entry, 0, sizeof(out->entry));
    out->entry.type = parse_sec_type(parts[0]);
    out->entry.flags = parse_sec_flags(parts[1]);
    out->entry.vaddr = parse_u64(parts[2]);
    out->entry.alignment = (uint32_t)parse_u64(parts[3]);
    out->data = read_file(parts[4], &out->data_len);
    out->entry.vsize = (count >= 6) ? parse_u64(parts[5]) : (uint64_t)out->data_len;
}

/* ---- Subcommands ------------------------------------------------------ */

static int cmd_build(int argc, char **argv) {
    if (argc < 2) die("Verwendung: nef build <output.nef> <code.bin> [--entry N] [--vaddr N]"
                       " [--arch x86_32|x86_64|arm64] [--flags exec,...]");
    const char *out_path = argv[0];
    const char *code_path = argv[1];
    uint64_t entry_point_ref = 0;
    uint64_t vaddr = 0x00500000ull; /* Konvention dieses Kernels, siehe nef_self_test()/ushell-Kommentare */
    uint32_t arch = NEF_ARCH_X86_32;
    uint32_t format_flags = NEF_FLAG_EXECUTABLE;

    for (int i = 2; i < argc; ++i) {
        if (strcmp(argv[i], "--entry") == 0 && i + 1 < argc) entry_point_ref = parse_u64(argv[++i]);
        else if (strcmp(argv[i], "--vaddr") == 0 && i + 1 < argc) vaddr = parse_u64(argv[++i]);
        else if (strcmp(argv[i], "--arch") == 0 && i + 1 < argc) arch = parse_arch(argv[++i]);
        else if (strcmp(argv[i], "--flags") == 0 && i + 1 < argc) format_flags = parse_flags(argv[++i]);
        else { fprintf(stderr, "nef: Fehler: unbekannte Option '%s'\n", argv[i]); return 1; }
    }

    pending_section_t sec;
    memset(&sec, 0, sizeof(sec));
    sec.entry.type = NEF_SECTION_CODE;
    sec.entry.flags = NEF_SEC_FLAG_READ | NEF_SEC_FLAG_EXEC;
    sec.entry.vaddr = vaddr;
    sec.entry.alignment = 4096u;
    sec.data = read_file(code_path, &sec.data_len);
    sec.entry.vsize = (uint64_t)sec.data_len;

    assemble_and_write(out_path, &sec, 1, entry_point_ref + vaddr, arch, NEF_ABI_NOVA_NATIVE,
                        format_flags, 0u);
    free(sec.data);
    return 0;
}

static int cmd_link(int argc, char **argv) {
    if (argc < 1) die("Verwendung: nef link <output.nef> --section TYP:FLAGS:VADDR:ALIGN:DATEI[:VSIZE] ...");
    const char *out_path = argv[0];
    uint64_t entry_point_ref = 0;
    uint32_t arch = NEF_ARCH_X86_32;
    uint32_t abi = NEF_ABI_NOVA_NATIVE;
    uint32_t format_flags = NEF_FLAG_EXECUTABLE;
    uint32_t min_os_version = 0;
    pending_section_t secs[MAX_SECTIONS];
    int sec_count = 0;

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--section") == 0 && i + 1 < argc) {
            if (sec_count >= MAX_SECTIONS) die("zu viele --section (Maximum erreicht)");
            parse_section_spec(argv[++i], &secs[sec_count++]);
        } else if (strcmp(argv[i], "--entry") == 0 && i + 1 < argc) entry_point_ref = parse_u64(argv[++i]);
        else if (strcmp(argv[i], "--arch") == 0 && i + 1 < argc) arch = parse_arch(argv[++i]);
        else if (strcmp(argv[i], "--abi") == 0 && i + 1 < argc) {
            if (strcmp(argv[++i], "nova") != 0) die("nur --abi nova wird derzeit unterstuetzt");
            abi = NEF_ABI_NOVA_NATIVE;
        } else if (strcmp(argv[i], "--flags") == 0 && i + 1 < argc) format_flags = parse_flags(argv[++i]);
        else if (strcmp(argv[i], "--min-os") == 0 && i + 1 < argc) min_os_version = (uint32_t)parse_u64(argv[++i]);
        else { fprintf(stderr, "nef: Fehler: unbekannte Option '%s'\n", argv[i]); return 1; }
    }
    if (sec_count == 0) die("mindestens eine --section wird benoetigt");

    assemble_and_write(out_path, secs, sec_count, entry_point_ref, arch, abi, format_flags, min_os_version);
    for (int i = 0; i < sec_count; ++i) free(secs[i].data);
    return 0;
}

static int cmd_validate(int argc, char **argv) {
    if (argc < 1) die("Verwendung: nef validate <input.nef>");
    long len;
    uint8_t *buf = read_file(argv[0], &len);
    nef_header_t hdr;
    int err = NEF_VALIDATE_OK;
    int ok = (nef_validate_header(buf, (uint32_t)len, &hdr, &err) == 0);
    free(buf);
    if (ok) {
        printf("GUELTIG\n");
        return 0;
    }
    printf("UNGUELTIG: ");
    validate_err_name_print(err);
    return 1;
}

static void print_inspect(const uint8_t *buf, long len, int with_hexdump) {
    nef_header_t hdr;
    int err = NEF_VALIDATE_OK;
    int ok = (nef_validate_header(buf, (uint32_t)len, &hdr, &err) == 0);

    if (with_hexdump) {
        printf("-- Header (erste %u Byte, hex) --\n", NEF_HEADER_SIZE);
        for (uint32_t i = 0; i < NEF_HEADER_SIZE && i < (uint32_t)len; ++i) {
            printf("%02X ", buf[i]);
            if ((i + 1) % 16 == 0) printf("\n");
        }
        printf("\n\n");
    }

    if (!ok) {
        /* Auch bei ungueltiger Datei so viel wie moeglich mit dem reinen
           (ungeprueften) Parser anzeigen - nuetzlich fuer Diagnose eines
           KAPUTTEN Files, nicht nur gueltiger. */
        int perr = NEF_PARSE_OK;
        if (nef_header_parse(buf, (uint32_t)len, &hdr, &perr) != 0) {
            printf("Header unlesbar (Parse-Fehlercode %d) - keine weiteren Details moeglich.\n", perr);
            return;
        }
        printf("WARNUNG: Datei besteht die volle Validierung NICHT (Fehlercode ");
        validate_err_name_print(err);
        printf("- die folgenden Werte stammen aus dem reinen, UNGEPRUEFTEN Header.\n\n");
    }

    printf("Version:          %u.%u\n", hdr.version_major, hdr.version_minor);
    printf("Header-Groesse:   %u\n", hdr.header_size);
    printf("Datei-Groesse:    %lu\n", (unsigned long)hdr.file_size);
    printf("Format-Flags:     0x%08X", hdr.format_flags);
    if (hdr.format_flags & NEF_FLAG_EXECUTABLE) printf(" EXECUTABLE");
    if (hdr.format_flags & NEF_FLAG_DYNAMIC_LIBRARY) printf(" DYNAMIC_LIBRARY");
    if (hdr.format_flags & NEF_FLAG_SYSTEM_MODULE) printf(" SYSTEM_MODULE");
    if (hdr.format_flags & NEF_FLAG_SIGNED) printf(" SIGNED");
    if (hdr.format_flags & NEF_FLAG_HAS_NOVA_IR) printf(" HAS_NOVA_IR");
    if (hdr.format_flags & NEF_FLAG_AOT_COMPILED) printf(" AOT_COMPILED");
    printf("\n");
    printf("Zielarchitektur:  %s\n", arch_name(hdr.target_arch));
    printf("ABI:              %u\n", hdr.abi_id);
    printf("Min-OS-Version:   %u\n", hdr.min_os_version);
    printf("Entry-Point-Ref:  0x%lX\n", (unsigned long)hdr.entry_point_ref);
    printf("Manifest-Ref:     0x%lX%s\n", (unsigned long)hdr.manifest_ref, hdr.manifest_ref == 0 ? " (keins)" : "");
    printf("Signatur-Ref:     0x%lX%s\n", (unsigned long)hdr.signature_dir_ref, hdr.signature_dir_ref == 0 ? " (keine)" : "");
    printf("Integritaets-Ref: 0x%lX%s\n", (unsigned long)hdr.integrity_ref, hdr.integrity_ref == 0 ? " (keine)" : "");

    printf("\n-- Sections (%u) --\n", hdr.section_count);
    for (uint32_t i = 0; i < hdr.section_count; ++i) {
        uint64_t off = hdr.section_dir_offset + (uint64_t)i * NEF_SECTION_ENTRY_SIZE;
        if (off + NEF_SECTION_ENTRY_SIZE > (uint64_t)len) {
            printf("  [%u] (ausserhalb der Datei, Abbruch der Auflistung)\n", i);
            break;
        }
        nef_section_entry_t sec;
        nef_section_entry_parse(buf + off, &sec);
        char flagbuf[8] = {0};
        int fp = 0;
        if (sec.flags & NEF_SEC_FLAG_READ) flagbuf[fp++] = 'R';
        if (sec.flags & NEF_SEC_FLAG_WRITE) flagbuf[fp++] = 'W';
        if (sec.flags & NEF_SEC_FLAG_EXEC) flagbuf[fp++] = 'X';
        if (sec.flags & NEF_SEC_FLAG_COMPRESSED) flagbuf[fp++] = 'C';
        if (sec.flags & NEF_SEC_FLAG_SHARED) flagbuf[fp++] = 'S';
        flagbuf[fp] = '\0';
        printf("  [%u] %-12s flags=%-5s file=0x%lX+%lu vaddr=0x%lX vsize=%lu align=%u\n",
               i, section_type_name(sec.type), flagbuf,
               (unsigned long)sec.file_offset, (unsigned long)sec.file_size,
               (unsigned long)sec.vaddr, (unsigned long)sec.vsize, sec.alignment);
    }

    printf("\n-- Extensions (%u) --\n", hdr.extension_count);
    for (uint32_t i = 0; i < hdr.extension_count; ++i) {
        uint64_t off = hdr.extension_dir_offset + (uint64_t)i * NEF_EXTENSION_ENTRY_SIZE;
        if (off + NEF_EXTENSION_ENTRY_SIZE > (uint64_t)len) {
            printf("  [%u] (ausserhalb der Datei, Abbruch der Auflistung)\n", i);
            break;
        }
        nef_extension_entry_t ext;
        nef_extension_entry_parse(buf + off, &ext);
        printf("  [%u] type_id=%u version=%u %s data=0x%lX+%lu\n",
               i, ext.type_id, ext.version, ext.required ? "REQUIRED" : "optional",
               (unsigned long)ext.data_offset, (unsigned long)ext.data_size);
    }
}

static int cmd_inspect(int argc, char **argv) {
    if (argc < 1) die("Verwendung: nef inspect <input.nef>");
    long len;
    uint8_t *buf = read_file(argv[0], &len);
    print_inspect(buf, len, 0);
    free(buf);
    return 0;
}

static int cmd_dump(int argc, char **argv) {
    if (argc < 1) die("Verwendung: nef dump <input.nef>");
    long len;
    uint8_t *buf = read_file(argv[0], &len);
    print_inspect(buf, len, 1);
    free(buf);
    return 0;
}

/* Sucht die SIGNATURES-Section (type/file_size bereits passend) in einer
   bereits per nef_validate_header() als strukturell gueltig bestaetigten
   Datei. 0 = gefunden (*out gesetzt), -1 sonst. */
static int find_signature_section(const uint8_t *buf, const nef_header_t *hdr,
                                   nef_section_entry_t *out) {
    for (uint32_t i = 0; i < hdr->section_count; ++i) {
        uint64_t off = hdr->section_dir_offset + (uint64_t)i * NEF_SECTION_ENTRY_SIZE;
        nef_section_entry_t sec;
        nef_section_entry_parse(buf + off, &sec);
        if (sec.type == NEF_SECTION_SIGNATURES && sec.file_size == NEF_SIGNATURE_DIGEST_SIZE) {
            *out = sec;
            return 0;
        }
    }
    return -1;
}

/* Echte Dev-Signatur (HMAC-SHA-256, siehe nef.h fuer die ehrliche
   Einordnung "symmetrisch, keine PKI") - erwartet, dass die Zieldatei
   bereits eine SIGNATURES-Section (32 Platzhalter-Byte, z.B. per
   `nef link --section SIGNATURES:r:0:1:zero32.bin` angelegt, ueblicherweise
   als LETZTE Section) enthaelt - die Signatur deckt per Konvention immer
   genau [0, Dateioffset-dieser-Section) ab, siehe nef.h. Schreibt den
   berechneten Digest in diese 32 Byte UND setzt NEF_FLAG_SIGNED im
   Header - beides in-place, KEINE Grossenaenderung/Section-Directory-
   Verschiebung noetig. */
static int cmd_sign(int argc, char **argv) {
    if (argc < 1) die("Verwendung: nef sign <input.nef>");
    long len;
    uint8_t *buf = read_file(argv[0], &len);

    nef_header_t hdr;
    int err = NEF_VALIDATE_OK;
    if (nef_validate_header(buf, (uint32_t)len, &hdr, &err) != 0) {
        fprintf(stderr, "nef: sign fehlgeschlagen - Datei ist strukturell UNGUELTIG: ");
        validate_err_name_print(err);
        free(buf);
        return 1;
    }

    nef_section_entry_t sig_sec;
    if (find_signature_section(buf, &hdr, &sig_sec) != 0) {
        fprintf(stderr, "nef: sign fehlgeschlagen - keine SIGNATURES-Section (%u Byte, Typ %u) "
                        "in dieser Datei. Zuerst per `nef link --section SIGNATURES:r:0:1:<32-Byte-"
                        "Platzhalterdatei>` als LETZTE Section anlegen.\n",
                NEF_SIGNATURE_DIGEST_SIZE, NEF_SECTION_SIGNATURES);
        free(buf);
        return 1;
    }

    /* REIHENFOLGE KRITISCH: NEF_FLAG_SIGNED liegt selbst INNERHALB des
       signierten Bereichs [0, sig_sec.file_offset) (Header ist Byte 0..96)
       - das Flag MUSS VOR der Digest-Berechnung gesetzt UND serialisiert
       werden, sonst signiert man Bytes, die man danach noch veraendert,
       und die eigene Verifikation schlaegt sofort fehl (so im ersten
       Versuch dieser Aenderung real passiert, per nef verify entdeckt). */
    hdr.format_flags |= NEF_FLAG_SIGNED;
    if (nef_header_serialize(&hdr, buf, (uint32_t)len) != 0) {
        fprintf(stderr, "nef: sign fehlgeschlagen - Header liess sich nicht neu serialisieren.\n");
        free(buf);
        return 1;
    }

    uint8_t digest[NEF_SIGNATURE_DIGEST_SIZE];
    nef_compute_signature(buf, (uint32_t)sig_sec.file_offset, digest);
    for (uint32_t i = 0; i < NEF_SIGNATURE_DIGEST_SIZE; ++i) {
        buf[(uint32_t)sig_sec.file_offset + i] = digest[i];
    }

    write_file(argv[0], buf, len);
    free(buf);
    printf("Signiert (HMAC-SHA-256, Dev-Schluessel, deckt [0, 0x%lX) ab).\n",
           (unsigned long)sig_sec.file_offset);
    return 0;
}

static int cmd_verify(int argc, char **argv) {
    if (argc < 1) die("Verwendung: nef verify <input.nef>");
    long len;
    uint8_t *buf = read_file(argv[0], &len);

    nef_header_t hdr;
    int err = NEF_VALIDATE_OK;
    if (nef_validate_header(buf, (uint32_t)len, &hdr, &err) != 0) {
        fprintf(stderr, "nef: verify fehlgeschlagen - Datei ist strukturell UNGUELTIG: ");
        validate_err_name_print(err);
        free(buf);
        return 1;
    }

    nef_section_entry_t secs[MAX_SECTIONS];
    uint32_t n = hdr.section_count;
    if (n > (uint32_t)MAX_SECTIONS) n = (uint32_t)MAX_SECTIONS;
    for (uint32_t i = 0; i < n; ++i) {
        uint64_t off = hdr.section_dir_offset + (uint64_t)i * NEF_SECTION_ENTRY_SIZE;
        nef_section_entry_parse(buf + off, &secs[i]);
    }

    int rc = nef_verify_signature(buf, (uint32_t)len, &hdr, secs, n);
    free(buf);
    switch (rc) {
        case NEF_SIG_OK:
            printf("GUELTIG signiert (HMAC-SHA-256, Dev-Schluessel).\n");
            return 0;
        case NEF_SIG_ERR_NOT_SIGNED:
            printf("UNSIGNIERT (NEF_FLAG_SIGNED nicht gesetzt).\n");
            return 1;
        case NEF_SIG_ERR_NO_SECTION:
            printf("UNGUELTIG: keine SIGNATURES-Section gefunden.\n");
            return 1;
        case NEF_SIG_ERR_MISMATCH:
            printf("UNGUELTIG: Digest stimmt nicht (Datei wurde nach dem Signieren veraendert).\n");
            return 1;
        default:
            printf("UNGUELTIG: unbekannter Fehlercode %d.\n", rc);
            return 1;
    }
}

static int cmd_test(int argc, char **argv) {
    (void)argc; (void)argv;
    int failures = 0;

    printf("[1/2] nef_self_test() (Format-Bibliothek, identisch zum Kernel-Selbsttest)... ");
    if (nef_self_test() == 0) {
        printf("OK\n");
    } else {
        printf("FEHLGESCHLAGEN\n");
        ++failures;
    }

    printf("[2/2] Werkzeug-Rundweg (build -> validate -> inspect)... ");
    {
        const char *tmp_code = "nef_test_code.bin";
        const char *tmp_nef = "nef_test_output.nef";
        static const uint8_t fake_code[8] = {0x90,0x90,0x90,0x90,0xC3,0x00,0x00,0x00}; /* NOP NOP NOP NOP RET */
        write_file(tmp_code, fake_code, sizeof(fake_code));

        char *build_argv[] = { (char *)tmp_nef, (char *)tmp_code, (char *)"--entry", (char *)"0" };
        int build_rc = cmd_build(4, build_argv);

        long len = 0;
        uint8_t *buf = build_rc == 0 ? read_file(tmp_nef, &len) : NULL;
        nef_header_t hdr;
        int err = NEF_VALIDATE_OK;
        int valid = buf && nef_validate_header(buf, (uint32_t)len, &hdr, &err) == 0;
        int section_ok = valid && hdr.section_count == 1u;

        if (build_rc == 0 && valid && section_ok) {
            printf("OK\n");
        } else {
            printf("FEHLGESCHLAGEN\n");
            ++failures;
        }
        free(buf);
        remove(tmp_code);
        remove(tmp_nef);
    }

    if (failures == 0) {
        printf("\nAlle Tests bestanden.\n");
        return 0;
    }
    printf("\n%d Test(s) fehlgeschlagen.\n", failures);
    return 1;
}

static void print_usage(void) {
    printf("nef - NovaOS NEF 1.0 Host-Werkzeug\n\n"
           "  nef build    <out.nef> <code.bin> [--entry N] [--vaddr N] [--arch A] [--flags F]\n"
           "  nef link     <out.nef> --section TYP:FLAGS:VADDR:ALIGN:DATEI[:VSIZE] ... [--entry N] [--arch A] [--abi nova] [--flags F] [--min-os N]\n"
           "  nef validate <in.nef>\n"
           "  nef inspect  <in.nef>\n"
           "  nef dump     <in.nef>\n"
           "  nef sign     <in.nef> ...   (noch nicht implementiert, Phase 5)\n"
           "  nef verify   <in.nef> ...   (noch nicht implementiert, Phase 5)\n"
           "  nef test\n");
}

int main(int argc, char **argv) {
    if (argc < 2) { print_usage(); return 1; }
    const char *cmd = argv[1];
    if (strcmp(cmd, "build") == 0) return cmd_build(argc - 2, argv + 2);
    if (strcmp(cmd, "link") == 0) return cmd_link(argc - 2, argv + 2);
    if (strcmp(cmd, "validate") == 0) return cmd_validate(argc - 2, argv + 2);
    if (strcmp(cmd, "inspect") == 0) return cmd_inspect(argc - 2, argv + 2);
    if (strcmp(cmd, "dump") == 0) return cmd_dump(argc - 2, argv + 2);
    if (strcmp(cmd, "sign") == 0) return cmd_sign(argc - 2, argv + 2);
    if (strcmp(cmd, "verify") == 0) return cmd_verify(argc - 2, argv + 2);
    if (strcmp(cmd, "test") == 0) return cmd_test(argc - 2, argv + 2);
    print_usage();
    return 1;
}

/* Build-Hinweis (Windows-Entwicklungssystem, MinGW gcc):
 *   gcc -std=c99 -Wall -Wextra -I ../../kernel/newkernel \
 *       nef_main.c ../../kernel/newkernel/nef.c -o nef.exe
 */
