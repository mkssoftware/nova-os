# Nova Studio – Cross-Compilation (Windows-Host → NovaOS-Ziel)

## Grundprinzip

**Host** = Windows x64 (wo Nova Studio und `novac.exe`/`nef.exe` als
native Win32/PE-Programme laufen).
**Ziel** = NovaOS, x86-32, 32-Bit Protected Mode, NEF-1.0-Binärformat.

Host- und Zielarchitektur sind UNABHÄNGIG: `novac.exe` läuft selbst als
x64-PE-Programm unter Windows, erzeugt aber reinen **32-Bit-x86-
Maschinencode** für NovaOS - es ist ein echter Cross-Compiler, kein
Interpreter und kein Transpiler nach C/C++.

## Pipeline (verifiziert, 2026-11-24)

```
hello_if.nova  (NovaLang-Quelltext)
      │
      ▼
novac.exe hello_if.nova -o hello_if
      │  Lexer → Parser → x86-32-Codegen
      ▼
hello_if.code.bin    (reiner x86-32-Maschinencode, Ziel-Vaddr 0x00500000)
hello_if.rodata.bin  (String-Literale,             Ziel-Vaddr 0x00400000)
hello_if.data.bin    (Integer-Variablen,           Ziel-Vaddr 0x00480000)
      │
      ▼
nef.exe link hello_if.nef --entry 0x00500000
    --section CODE:rx:0x00500000:4096:hello_if.code.bin
    --section RODATA:r:0x00400000:4096:hello_if.rodata.bin
    --section DATA:rw:0x00480000:4096:hello_if.data.bin
      │
      ▼
hello_if.nef   (NEF-1.0-Datei, 520 Byte, 3 Sections)
```

## Tatsächlich durchgeführte Verifikation

```
$ novac.exe hello_if.nova -o hello_if
novac: 175 Bytes Code -> hello_if.code.bin (Ziel-Vaddr 0x00500000)
novac: 94 Bytes Rodata -> hello_if.rodata.bin (Ziel-Vaddr 0x00400000)
novac: 8 Bytes Data -> hello_if.data.bin (Ziel-Vaddr 0x00480000)

$ nef.exe link hello_if.nef --entry 0x00500000 --section ...
nef: 'hello_if.nef' geschrieben (520 Byte, 3 Section(s))

$ nef.exe validate hello_if.nef
GUELTIG

$ nef.exe inspect hello_if.nef
Version:          1.0
Format-Flags:     0x00000001 EXECUTABLE
Zielarchitektur:  x86_32
Entry-Point-Ref:  0x500000
-- Sections (3) --
  [0] CODE    flags=RX  vaddr=0x500000 vsize=175
  [1] RODATA  flags=R   vaddr=0x400000 vsize=94
  [2] DATA    flags=RW  vaddr=0x480000 vsize=8
```

Das beweist konkret:
- **Test #7** (Compiler läuft nativ unter Windows) ✅
- **Test #8** (Programm für NovaOS unter Windows kompiliert) ✅
- **Test #9** (Linker erzeugt gültige NEF-Datei) ✅
- **Test #10** (NEF enthält nativen x86-32-Code) ✅ (Zielarchitektur-Feld
  `x86_32`, reiner Maschinencode in der CODE-Section)
- **Test #11** (keine Windows-Laufzeitabhängigkeiten) ✅ (die drei
  Sections enthalten ausschließlich Code/Daten, keine Import-Tabelle,
  kein PE-Header, kein Verweis auf irgendeine `.dll`)

**NICHT verifiziert** (siehe `IMPLEMENTATION_STATUS.md`): **Test #12**
(das Programm tatsächlich unter NovaOS/QEMU ausführen) - dafür fehlt die
NEF→NovaFS-Transfer- und QEMU-Start-Integration in der IDE.

## Wichtige Einschränkung: eine Quelldatei pro Aufruf

`novac.exe <quelle.nova> -o <praefix>` akzeptiert **genau eine**
Quelldatei (per CLI-Test verifiziert: ein zweiter Dateiparameter führt
zur Nutzungsmeldung, nicht zu einem Mehrdatei-Compile). Es gibt noch
KEINE Cross-File-Modulauflösung (`Imports` über Dateigrenzen hinweg) und
keinen Mehrobjekt-Linkschritt im Compiler-Frontend selbst.

Praktische Folge für Nova Studio: ein Projekt kann mehrere `.nova`-
Dateien NACHVERFOLGEN (`project.xml` `<Sources>`), aber "Erstellen"
kompiliert immer nur die als Einstiegspunkt markierte Datei (siehe
`PROJECT_FORMAT.md`). Ein echtes Mehrdatei-Projekt mit internen
Modulreferenzen ist eine Erweiterung des NovaLang-Compilers selbst,
nicht der IDE, und wurde nicht begonnen.

## Zielarchitektur-Festlegung

Feste Adressen (vom Compiler selbst gewählt, nicht konfigurierbar):

| Section | Ziel-Vaddr | Zweck |
|---|---|---|
| CODE | `0x00500000` | x86-32-Maschinencode |
| RODATA | `0x00400000` | String-Literale |
| DATA | `0x00480000` | Integer-Variablen |
| BUFFER | `0x00403000` | Syscall-Argument-Puffer (falls verwendet) |

Diese Adressen entsprechen der dokumentierten NovaOS-Speicherlayout-
Konvention für Ring-3-Prozesse (siehe `kernel/newkernel`-Dokumentation);
Nova Studio erfindet hier nichts selbst, sondern übernimmt unverändert,
was `novac.exe` als nächsten Schritt vorschlägt (siehe `ARCHITECTURE.md`).

## 64-Bit-Ziel (ARM64/x86-64)

Im Entwicklungsauftrag als spätere Option erwähnt. `nef.exe build`
erwähnt `--arch x86_32|x86_64|arm64` in seiner eigenen Hilfe, `novac.exe`
selbst erzeugt aber ausschließlich x86-32-Code - ein 64-Bit-Backend
existiert im NovaLang-Compiler nicht. NovaOS ist zudem laut
Projektvorgabe bewusst bei 32-Bit UEFI belassen (kein BIOS/Long-Mode,
siehe Kernel-Projektdokumentation) - 64-Bit-Zielunterstützung ist daher
weder vorhanden noch für dieses Projekt aktuell vorgesehen.
