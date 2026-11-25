/* novac - NovaLang Compiler CLI (v0.1-v0.5, siehe
 * specifications/NOVALANG_SPEC_v0.*.md fuer den jeweiligen Scope).
 *
 * Verwendung:
 *   novac.exe <quelle.nova> -o <ausgabe_praefix>
 *
 * Erzeugt bis zu VIER rohe Maschinencode-/Daten-Dateien (siehe
 * codegen_x86.h fuer die Section-Begruendung):
 *   <ausgabe_praefix>.code.bin    - x86-32-Code, Ziel-Vaddr 0x00500000 (immer)
 *   <ausgabe_praefix>.rodata.bin  - String-Literale, Ziel-Vaddr 0x00400000 (immer)
 *   <ausgabe_praefix>.data.bin    - Integer-Variablen, Ziel-Vaddr 0x00480000 (nur falls welche deklariert)
 *   <ausgabe_praefix>.buffer.bin  - Syscall-Argument-Puffer, Ziel-Vaddr 0x00403000 (nur falls welche deklariert)
 *
 * WEITERVERARBEITUNG per bereits vorhandenem `tools/nef/nef_main.c`
 * (siehe die am Ende ausgegebene vollstaendige `nef link`-Befehlszeile).
 */
#include "lexer.h"
#include "parser.h"
#include "codegen_x86.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *read_file(const char *path, uint32_t *out_len) {
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    rewind(f);
    char *buf = (char *)malloc((size_t)sz + 1);
    size_t rd = fread(buf, 1, (size_t)sz, f);
    buf[rd] = '\0';
    fclose(f);
    *out_len = (uint32_t)rd;
    return buf;
}

static int write_file(const char *path, const uint8_t *data, uint32_t len) {
    FILE *f = fopen(path, "wb");
    if (!f) return -1;
    fwrite(data, 1, len, f);
    fclose(f);
    return 0;
}

int main(int argc, char **argv) {
    if (argc < 4 || strcmp(argv[2], "-o") != 0) {
        fprintf(stderr, "novac (NovaLang Compiler, siehe specifications/NOVALANG_SPEC_v0.*.md)\n");
        fprintf(stderr, "Verwendung: %s <quelle.nova> -o <ausgabe_praefix>\n", argv[0]);
        return 2;
    }
    const char *input_path = argv[1];
    const char *output_path = argv[3];

    uint32_t src_len = 0;
    char *src = read_file(input_path, &src_len);
    if (!src) {
        fprintf(stderr, "novac: Fehler: Datei nicht gefunden: %s\n", input_path);
        return 2;
    }

    NcLexer lx;
    nc_lexer_init(&lx, src, src_len);

    char error[256];
    NcModule *mod = nc_parse_module(&lx, error, sizeof(error));
    if (!mod) {
        fprintf(stderr, "novac: Syntaxfehler: %s\n", error);
        free(src);
        return 1;
    }

    uint8_t *code_buf = NULL, *rodata_buf = NULL, *data_buf = NULL, *buffer_buf = NULL;
    uint32_t code_len = 0, rodata_len = 0, data_len = 0, buffer_len = 0;
    if (nc_codegen_x86(mod, &code_buf, &code_len, &rodata_buf, &rodata_len,
                        &data_buf, &data_len, &buffer_buf, &buffer_len) != 0) {
        fprintf(stderr, "novac: Fehler: Codegenerierung fehlgeschlagen "
                         "(RODATA-/Puffer-Limit ueberschritten oder ein If-Koerper zu gross "
                         "fuer einen kurzen Sprung, siehe codegen_x86.h/.c).\n");
        free(src);
        return 1;
    }

    char code_path[4096], rodata_path[4096], data_path[4096], buffer_path[4096];
    snprintf(code_path, sizeof(code_path), "%s.code.bin", output_path);
    snprintf(rodata_path, sizeof(rodata_path), "%s.rodata.bin", output_path);
    snprintf(data_path, sizeof(data_path), "%s.data.bin", output_path);
    snprintf(buffer_path, sizeof(buffer_path), "%s.buffer.bin", output_path);

    if (write_file(code_path, code_buf, code_len) != 0 ||
        write_file(rodata_path, rodata_buf, rodata_len) != 0 ||
        (data_len > 0 && write_file(data_path, data_buf, data_len) != 0) ||
        (buffer_len > 0 && write_file(buffer_path, buffer_buf, buffer_len) != 0)) {
        fprintf(stderr, "novac: Fehler: kann eine Ausgabedatei nicht schreiben.\n");
        free(code_buf); free(rodata_buf); free(data_buf); free(buffer_buf); free(src);
        return 2;
    }

    printf("novac: %u Bytes Code -> %s (Ziel-Vaddr 0x00500000)\n", code_len, code_path);
    printf("novac: %u Bytes Rodata -> %s (Ziel-Vaddr 0x00400000)\n", rodata_len, rodata_path);
    if (data_len > 0) printf("novac: %u Bytes Data -> %s (Ziel-Vaddr 0x00480000)\n", data_len, data_path);
    if (buffer_len > 0) printf("novac: %u Bytes Buffer -> %s (Ziel-Vaddr 0x00403000)\n", buffer_len, buffer_path);

    char cmd[8192];
    int n = snprintf(cmd, sizeof(cmd),
        "nef link <ausgabe.nef> --entry 0x00500000 "
        "--section CODE:rx:0x00500000:4096:%s --section RODATA:r:0x00400000:4096:%s",
        code_path, rodata_path);
    if (data_len > 0) {
        n += snprintf(cmd + n, sizeof(cmd) - (size_t)n,
            " --section DATA:rw:0x00480000:4096:%s", data_path);
    }
    if (buffer_len > 0) {
        n += snprintf(cmd + n, sizeof(cmd) - (size_t)n,
            " --section BUFFER:rw:0x00403000:4096:%s", buffer_path);
    }
    printf("novac: naechster Schritt: %s\n", cmd);

    free(code_buf); free(rodata_buf); free(data_buf); free(buffer_buf); free(src);
    return 0;
}
