# Nova Studio – Windows-Build

> **Hinweis (2026-11-24):** Dieser Build bezieht sich auf die native
> Win32/C++-IDE (`nova-studio.exe`), die der Nutzer nicht mehr
> weiterentwickelt. Für den AKTIV weiterentwickelten C#/WPF-Build siehe
> den Abschnitt "WPF-Build (aktueller Code-Pfad)" am Ende dieser Datei.
> Beide Programme bleiben unabhängig voneinander im Repository
> erhalten.

## Voraussetzungen

- Windows 10/11 x64
- MSYS2 mit `ucrt64`-Toolchain (`g++` ≥ 13, C++17), z. B. installiert unter
  `C:\msys64\ucrt64\bin`
- `make` (Teil von MSYS2, z. B. `pacman -S make` im MSYS2-Shell)
- Für die vollständige Toolchain-Kette zusätzlich (bereits im Repository
  vorhanden, nicht Teil dieses Builds):
  - `novalang/compiler/novac.exe` (NovaLang-Compiler, eigener Build über
    `novalang/compiler/Makefile`, `CC=gcc` reicht - MinGW/MSYS2-gcc)
  - `tools/nef/nef.exe` (NEF-Linker/Validator/Inspector)

Kein .NET, keine Visual-Studio-Installation, kein WSL und keine Linux-VM
nötig - Nova Studio selbst UND die gesamte Toolchain sind reine native
Win32/MinGW-Programme.

## Build

```powershell
cd toolchain\NovaStudio
$env:PATH = "C:\msys64\ucrt64\bin;$env:PATH"
make
```

Ergebnis: `nova-studio.exe` im selben Verzeichnis, statisch gelinkt
(`-static-libgcc -static-libstdc++`, eigene `pthread_win32.cpp` statt
`libwinpthread-1.dll`) - lauffähig ohne MSYS2-Laufzeit-DLLs im
`PATH`/Zielsystem.

`make clean` entfernt `nova-studio.exe` wieder.

## Verifizierter Build-Zustand (2026-11-24)

Letzter Build: **0 Compiler-Warnungen, 0 Fehler** (vorher 2 harmlose
Warnungen durch toten Code, siehe `IMPLEMENTATION_STATUS.md`).

```
g++ -std=c++17 -O2 -Wall -Wextra -Wno-missing-field-initializers
    -Wno-unused-parameter -DUNICODE -D_UNICODE -DWIN32_LEAN_AND_MEAN
    -DNOMINMAX -D_WIN32_WINNT=0x0601 -municode
    -o nova-studio.exe src/main.cpp src/editor.cpp src/explorer.cpp
    src/highlighter.cpp src/dialogs.cpp src/ribbon.cpp src/pthread_win32.cpp
    -mwindows -static-libgcc -static-libstdc++
    -lcomctl32 -lcomdlg32 -lshlwapi -lole32 -ldwmapi -lshell32 -luser32
    -lgdi32 -luxtheme
Gebaut: nova-studio.exe
```

## Starten

```powershell
.\nova-studio.exe
```

Startet als natives Win32-Fenster mit eigener Titelleiste und Ribbon.
Kein Installationsschritt nötig (siehe `IMPLEMENTATION_STATUS.md` zum
fehlenden Installer/zur fehlenden portablen Paketierung - aktuell IST
die einzelne `.exe` bereits "portabel" im Sinne von "keine Installation
nötig", es fehlt nur ein offizielles Verteilungspaket/Installer).

## Die Toolchain selbst bauen (für "Erstellen"/"Ausführen" in der IDE)

```powershell
cd novalang\compiler
make        # erzeugt novac.exe (Makefile: CC=gcc, -std=c11)
```

Verifiziert (2026-11-24): `make clean && make` lief fehlerfrei durch,
der neu gebaute `novac.exe` wurde erneut gegen ein Testprogramm geprüft
und funktioniert identisch zum vorher ausgelieferten Binary.

`tools\nef\nef.exe` liegt bereits vorgebaut im Repository (kein Makefile
vorhanden). Ein Rebuild-Versuch aus `nef_main.c` (inkludiert
`kernel/newkernel/nef.h`) schlug beim Testen dieser Anleitung am
Link-Schritt fehl (ld-Exitcode 5, ohne lesbare Undefined-Reference-
Meldung) - die genaue Build-Konfiguration für dieses Werkzeug ist NICHT
vollständig rekonstruiert und wird hier bewusst NICHT als funktionierend
behauptet. Die vorhandene `nef.exe` selbst wurde in dieser Session
mehrfach per CLI erfolgreich verwendet (`link`/`validate`/`inspect`) und
funktioniert einwandfrei - nur das Nachbauen aus dem Quelltext ist
ungeklärt.

Nova Studio sucht beide Werkzeuge relativ zur Repo-Wurzel (zwei Ebenen
über `toolchain/NovaStudio/nova-studio.exe`), siehe `RepoRoot()` in
`src/main.cpp` und `ARCHITECTURE.md`.

## WPF-Build (aktueller Code-Pfad, 2026-11-24)

Der Nutzer hat sich für den neuen C#/WPF-Stack entschieden (siehe
`IMPLEMENTATION_STATUS.md`, Abschnitt "DRITTE Entscheidung"). Dieser
Build ist unabhängig vom Win32-Build oben und benötigt keine MSYS2-
Toolchain.

### Voraussetzungen

- .NET 8 SDK (getestet mit 8.0.101)
- Windows 10/11 x64 (WPF, kein Cross-Plattform-Build)

### Build

```powershell
cd toolchain\NovaStudio
dotnet build NovaStudio.sln
```

Verifiziert (2026-11-24): 0 Warnungen, 0 Fehler.

### Starten

```powershell
.\src\NovaStudio.UI\bin\Debug\net8.0-windows\NovaStudio.UI.exe
```

Nutzt dieselbe Toolchain (`novalang/compiler/novac.exe` +
`tools/nef/nef.exe`) wie der Win32-Build für den "Erstellen"-Befehl im
Ribbon - siehe oben für deren Aufbau. Ein globaler
`DispatcherUnhandledException`-Handler (`App.xaml.cs`) schreibt
unerwartete Abstürze nach `crash.log` im Arbeitsverzeichnis, statt dass
die Anwendung kommentarlos verschwindet.
