/*
 * main.c – NovaLang Compiler CLI
 *
 * Usage:
 *   novalang.exe <source.nova> [options]
 *
 * Options:
 *   --check        Syntax + type-check only, no output file
 *   --ir           Dump Nova IR to stdout
 *   --ast          Dump AST to stdout
 *   --rebuild      Ignored (reserved for Studio build system)
 *   --version      Print version and exit
 *
 * Exit codes:
 *   0  success
 *   1  compile error
 *   2  usage / file error
 */

#include "compiler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NL_VERSION "0.1.0"

static void print_usage(const char *argv0)
{
    fprintf(stderr,
        "NovaLang Compiler " NL_VERSION "\n"
        "Verwendung: %s <quelldatei.nova> [Optionen]\n"
        "\n"
        "Optionen:\n"
        "  --check     Nur Syntax- und Typprüfung, kein Ausgabeartefakt\n"
        "  --ir        Nova IR auf stdout ausgeben\n"
        "  --ast       AST auf stdout ausgeben\n"
        "  --rebuild   Vollständige Neukompilierung (ignoriert Cache)\n"
        "  --version   Version ausgeben\n"
        "\n"
        "Ausgabecodes:\n"
        "  0  Erfolg\n"
        "  1  Compilerfehler\n"
        "  2  Aufruffehler / Datei nicht gefunden\n",
        argv0);
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        print_usage(argv[0]);
        return 2;
    }

    const char *input_path  = NULL;
    const char *output_path = NULL;
    NlOutputKind out_kind   = NL_OUT_BYTECODE;
    int rebuild = 0;

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--version") == 0) {
            printf("novalang " NL_VERSION "\n");
            return 0;
        } else if (strcmp(argv[i], "--check") == 0) {
            out_kind = NL_OUT_CHECK_ONLY;
        } else if (strcmp(argv[i], "--ir") == 0) {
            out_kind = NL_OUT_IR_DUMP;
        } else if (strcmp(argv[i], "--ast") == 0) {
            out_kind = NL_OUT_AST_DUMP;
        } else if (strcmp(argv[i], "--rebuild") == 0) {
            rebuild = 1;
            (void)rebuild;
        } else if (strcmp(argv[i], "--output") == 0 && i + 1 < argc) {
            output_path = argv[++i];
        } else if (argv[i][0] != '-') {
            if (input_path) {
                fprintf(stderr, "Fehler: Mehrere Eingabedateien angegeben.\n");
                return 2;
            }
            input_path = argv[i];
        } else {
            fprintf(stderr, "Unbekannte Option: %s\n", argv[i]);
            print_usage(argv[0]);
            return 2;
        }
    }

    if (!input_path) {
        fprintf(stderr, "Fehler: Keine Quelldatei angegeben.\n");
        print_usage(argv[0]);
        return 2;
    }

    /* Derive default output path: replace .nova → .nlb */
    char auto_out[4096];
    if (!output_path && out_kind == NL_OUT_BYTECODE) {
        strncpy(auto_out, input_path, sizeof(auto_out) - 5);
        auto_out[sizeof(auto_out) - 5] = '\0';
        char *dot = strrchr(auto_out, '.');
        if (dot) *dot = '\0';
        strcat(auto_out, ".nlb");
        output_path = auto_out;
    }

    NlCompilerOptions opts;
    opts.input_path  = input_path;
    opts.output_path = output_path;
    opts.output_kind = out_kind;
    opts.verbose     = 0;

    NlCompileResult result = nl_compile(&opts);

    if (result.warning_count > 0)
        fprintf(stderr, "%u Warnung(en)\n", result.warning_count);

    if (!result.success) {
        fprintf(stderr, "%u Fehler – Kompilierung fehlgeschlagen.\n",
                result.error_count);
        return 1;
    }

    return 0;
}
