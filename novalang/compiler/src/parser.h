#ifndef NOVAC_PARSER_H
#define NOVAC_PARSER_H

#include "ast.h"
#include "lexer.h"

/* Parst eine komplette Datei (lx muss am Anfang stehen). Gibt bei Erfolg
   ein malloc'tes NcModule* zurueck, bei einem Syntaxfehler NULL (Fehler-
   text in out_error, vom Aufrufer NICHT freizugeben - statischer Puffer). */
NcModule *nc_parse_module(NcLexer *lx, char *out_error, uint32_t out_error_cap);

#endif
