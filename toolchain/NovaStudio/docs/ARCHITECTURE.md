# Nova Studio – Architektur

> **Hinweis (2026-11-24):** Diese Entscheidung für Win32/C++ wurde vom
> Nutzer inzwischen ausdrücklich zurückgenommen - der aktiv
> weiterentwickelte Code-Pfad ist jetzt die C#/WPF-Anwendung unter
> `src/NovaStudio.UI/` (`NovaStudio.sln`). Siehe den Abschnitt
> "DRITTE Entscheidung" in `IMPLEMENTATION_STATUS.md` für den aktuellen
> Stand. Dieses Dokument (`ARCHITECTURE.md`) beschreibt weiterhin
> korrekt die native Win32/C++-IDE (`src/*.cpp`, `nova-studio.exe`) -
> dieser Code bleibt unverändert im Repository erhalten, wird aber
> nicht mehr weiterentwickelt.

Siehe `IMPLEMENTATION_STATUS.md` für die Vorgeschichte dieser
Architekturentscheidung (vorhandene Win32/C++-IDE statt eines neuen
C#/WPF-Stacks) und für den genauen Implementierungsstand.

## Technologie

- Natives Win32 C++ (C++17), kein .NET/WPF.
- MSYS2 ucrt64 `g++`, statisch gelinkt (`-static-libgcc -static-libstdc++`,
  `pthread_win32.cpp` als eigene Win32-native pthread-Implementierung statt
  `libwinpthread-1.dll`) – lauffähig ohne MSYS2-Laufzeit-DLLs.
- UI: natives Win32 (RichEdit-Editor, TreeView-Explorer, TabControl,
  ListView für die Probleme-Liste, eigene Titelleiste via
  `WM_NCCALCSIZE`/`DrawTitleBar`).

## Module (`src/`)

| Datei | Zweck |
|---|---|
| `main.cpp` | WndProc, Menüs/Ribbon-Kommandos, **Build-Pipeline (`DoBuild`/`DoRun`/`DoClean`)**, Projekt öffnen |
| `editor.cpp` | RichEdit-Host, Tabs, Zoom, Suchen/Ersetzen, **Output- und Probleme-Panel** |
| `explorer.cpp` | Datei-/Ordner-TreeView |
| `highlighter.cpp` | Tokenizer für Syntax-Hervorhebung (NICHT die Compiler-Grammatik – rein lexikalisch, keine semantische Analyse) |
| `ribbon.cpp` | Ribbonbar-Zeichnung/Interaktion |
| `dialogs.cpp` | Suchen/Ersetzen/Gehe-zu/Über/Neues-Projekt-Dialoge |
| `pthread_win32.cpp` | Minimale Win32-native pthread-Ersatzimplementierung |
| `studio.h` | Gemeinsame Typen, globale Handles, Funktionsdeklarationen |

## Build-Pipeline (NPSPEC-STUDIO-BUILD-0001)

```
NovaStudio DoBuild()
    │
    ├─► novalang/compiler/novac.exe <quelle.nova> -o <praefix>
    │       (Lexer → Parser → x86-32-Codegen, siehe
    │        novalang/specifications/NOVALANG_SPEC_v0.*.md)
    │       erzeugt <praefix>.code.bin / .rodata.bin / .data.bin
    │       und gibt auf stdout die fertige Linker-Kommandozeile aus
    │       ("novac: naechster Schritt: nef link <ausgabe.nef> --entry ...")
    │
    ├─► tools/nef/nef.exe link ...
    │       (EXAKT die von novac vorgeschlagene Kommandozeile, nur der
    │        Platzhalter "<ausgabe.nef>" wird durch den echten Zielpfad
    │        ersetzt - NovaStudio rät keine Sektionslayouts selbst)
    │       erzeugt <praefix>.nef
    │
    └─► tools/nef/nef.exe validate <praefix>.nef
            (beweist eine strukturell gültige NEF-1.0-Datei, nicht nur
             "Linker meldete Erfolg")
```

Jede Stufe läuft als echter Windows-Prozess (`CreateProcessW`, siehe
`RunCommand()` in `main.cpp`); stdout/stderr werden zeilenweise ins
Ausgabe-Panel geschrieben, Fehlerzeilen zusätzlich geparst und in die
Probleme-Liste (Datei + Zeile + Meldung) eingetragen.

**Pfadauflösung:** `nova-studio.exe` liegt unter
`toolchain/NovaStudio/`; die Repo-Wurzel wird als zwei Verzeichnisebenen
darüber bestimmt (`RepoRoot()` in `main.cpp`). `novac.exe`/`nef.exe`
werden relativ dazu gesucht (`novalang/compiler/novac.exe`,
`tools/nef/nef.exe`) – NICHT mehr neben `nova-studio.exe` selbst (dort lag
vorher das inzwischen nicht mehr verwendete `novalang.exe`).

## Bewusst NICHT verwendete Parallel-Implementierung

`toolchain/novalang/` ist ein **separater, älterer** NovaLang-Compiler
mit C-Transpiler-Backend (erzeugt über `gcc` direkt Windows-`.exe`-
Dateien). Er wird von Nova Studio **nicht mehr** aufgerufen, da er keine
NovaOS-NEF-Dateien erzeugt und damit für das eigentliche Ziel (Programme
für NovaOS) ungeeignet ist. Er wurde nicht gelöscht (kein Teil dieses
Auftrags), sollte aber nicht mit `novalang/compiler/` verwechselt werden.
