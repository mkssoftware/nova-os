# Nova Studio – NovaOS SDK

## Status: NICHT IMPLEMENTIERT

Kein SDK Manager, keine Mehrversions-Verwaltung, keine
projektbezogene SDK-Auswahl.

## Warum

Ein "SDK" im Sinn des Auftrags (gebündelte, versionierte NovaLang-/
ABI-/Syscall-/Capability-/Laufzeit-Definitionen, parallel installierbar)
setzt voraus, dass es bereits MEHRERE unterscheidbare Versionen dieser
Artefakte gibt, zwischen denen gewählt werden könnte. Aktuell existiert
genau EINE Toolchain-Version (`novac.exe` + `tools/nef/nef.exe`,
NOVALANG_SPEC v0.1-v0.16 inkrementell im selben Compiler-Codebestand,
kein versioniertes Nebeneinander mehrerer Compiler-Generationen). Ein
SDK-Manager für eine einzige, nie parallel installierte Version wäre
reine Attrappe.

## Was tatsächlich als "Quelle der Wahrheit" existiert (unverändert,
nicht dupliziert)

- NovaLang-Version/Sprachsyntax: `novalang/specifications/
  NOVALANG_SPEC_v0.*.md`
- Compiler: `novalang/compiler/` (`novac.exe`)
- NEF-Version/ABI: `kernel/newkernel/nef.h`, `tools/nef/`
- Systemaufrufe: `kernel/newkernel/syscall.h`
- Capability-Definitionen: `kernel/newkernel/security.h`

**Update (2026-11-24): `syscall.h` UND `security.h` werden jetzt
tatsächlich gelesen** - der Syscall Browser parst `kernel/newkernel/
syscall.h` echt (53 Konstanten) und der Capability Manager parst
`kernel/newkernel/security.h` echt (21 Konstanten, inkl. errechneter
Bitpositionen) - beide waren hier ursprünglich als "nächster
realistischer Schritt" vorgeschlagen, jetzt umgesetzt (siehe
`IMPLEMENTATION_STATUS.md`). Der Capability Manager deckt bewusst nur
`security.c`s Flat-Bitmask-System ab, nicht `cap_core.c`/`program.c`/
`verify.c` (drei weitere, separate Capability-Konzepte laut
`security.h`-Dateikopf).

Jede künftige SDK-Manager-Implementierung MUSS diese Dateien als
alleinige Quelle verwenden, nicht eigene, parallele Definitionen
einführen (ausdrückliches Verbot im Auftrag: "Keine erfundenen
Systemaufrufe").

## Nächster realistischer Schritt

Das gleiche Muster auf `cap_core.c`s objektbasierte Capabilities
anwenden (Delegation/Revocation sichtbar machen) - deutlich komplexer
als eine reine `#define`-Liste, da es sich um Laufzeitzustand handelt,
nicht um statische Header-Konstanten. Nicht begonnen.
