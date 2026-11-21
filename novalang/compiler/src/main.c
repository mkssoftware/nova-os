/* novac - NovaLang v0.1 Compiler CLI.
 *
 * Verwendung:
 *   novac.exe <quelle.nova> -o <ausgabe_praefix>
 *
 * Erzeugt ZWEI rohe Maschinencode-/Daten-Dateien (siehe codegen_x86.h fuer
 * die Begruendung der Zwei-Sections-Aufteilung):
 *   <ausgabe_praefix>.code.bin   - x86-32-Code, Ziel-Vaddr 0x00500000
 *   <ausgabe_praefix>.rodata.bin - String-Literale, Ziel-Vaddr 0x00400000
 *
 * WEITERVERARBEITUNG per bereits vorhandenem `tools/nef/nef_main.c`:
 *
 *   nef link ausgabe.nef --entry 0x00500000 \
 *     --section CODE:5:0x00500000:4096:ausgabe.code.bin \
 *     --section RODATA:1:0x00400000:4096:ausgabe.rodata.bin
 *
 * (Flags: 5 = READ|EXEC, 1 = READ; siehe nef.h NEF_SEC_FLAG_*)
 *
 * Siehe specifications/NOVALANG_SPEC_v0.1.md fuer den vollstaendigen Scope
 * dieser Version (bewusst NUR das Hello-NovaLang-Beispiel, keine fertige
 * NovaLang-1.0-Sprache).
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

int main(int argc, char **argv) {
    if (argc < 4 || strcmp(argv[2], "-o") != 0) {
        fprintf(stderr, "novac 0.1.0 (NovaLang Vertical Slice, siehe specifications/NOVALANG_SPEC_v0.1.md)\n");
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

    uint8_t *code_buf = NULL, *rodata_buf = NULL, *data_buf = NULL;
    uint32_t code_len = 0, rodata_len = 0, data_len = 0;
    if (nc_codegen_x86(mod, &code_buf, &code_len, &rodata_buf, &rodata_len, &data_buf, &data_len) != 0) {
        fprintf(stderr, "novac: Fehler: Codegenerierung fehlgeschlagen "
                         "(RODATA-Fenster-Limit ueberschritten oder ein If-Koerper zu gross "
                         "fuer einen kurzen Sprung, siehe codegen_x86.h/.c).\n");
        free(src);
        return 1;
    }

    char code_path[4096], rodata_path[4096], data_path[4096];
    snprintf(code_path, sizeof(code_path), "%s.code.bin", output_path);
    snprintf(rodata_path, sizeof(rodata_path), "%s.rodata.bin", output_path);
    snprintf(data_path, sizeof(data_path), "%s.data.bin", output_path);

    FILE *out_code = fopen(code_path, "wb");
    if (!out_code) {
        fprintf(stderr, "novac: Fehler: kann Ausgabedatei nicht schreiben: %s\n", code_path);
        free(code_buf); free(rodata_buf); free(data_buf); free(src);
        return 2;
    }
    fwrite(code_buf, 1, code_len, out_code);
    fclose(out_code);

    FILE *out_rodata = fopen(rodata_path, "wb");
    if (!out_rodata) {
        fprintf(stderr, "novac: Fehler: kann Ausgabedatei nicht schreiben: %s\n", rodata_path);
        free(code_buf); free(rodata_buf); free(data_buf); free(src);
        return 2;
    }
    fwrite(rodata_buf, 1, rodata_len, out_rodata);
    fclose(out_rodata);

    if (data_len > 0) {
        FILE *out_data = fopen(data_path, "wb");
        if (!out_data) {
            fprintf(stderr, "novac: Fehler: kann Ausgabedatei nicht schreiben: %s\n", data_path);
            free(code_buf); free(rodata_buf); free(data_buf); free(src);
            return 2;
        }
        fwrite(data_buf, 1, data_len, out_data);
        fclose(out_data);
    }

    printf("novac: %u Bytes Code -> %s (Ziel-Vaddr 0x00500000)\n", code_len, code_path);
    printf("novac: %u Bytes Rodata -> %s (Ziel-Vaddr 0x00400000)\n", rodata_len, rodata_path);
    if (data_len > 0) {
        printf("novac: %u Bytes Data -> %s (Ziel-Vaddr 0x00480000)\n", data_len, data_path);
        printf("novac: naechster Schritt: nef link <ausgabe.nef> --entry 0x00500000 "
               "--section CODE:rx:0x00500000:4096:%s --section RODATA:r:0x00400000:4096:%s "
               "--section DATA:rw:0x00480000:4096:%s\n",
               code_path, rodata_path, data_path);
    } else {
        printf("novac: naechster Schritt: nef link <ausgabe.nef> --entry 0x00500000 "
               "--section CODE:rx:0x00500000:4096:%s --section RODATA:r:0x00400000:4096:%s\n",
               code_path, rodata_path);
    }

    free(code_buf); free(rodata_buf); free(data_buf); free(src);
    return 0;
}
