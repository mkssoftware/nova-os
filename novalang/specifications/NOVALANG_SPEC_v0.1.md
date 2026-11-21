# NovaLang 0.1 – Vertical-Slice-Spezifikation

Status: **erster End-to-End-Meilenstein** (Masterauftrag Abschnitt 28), NICHT
NovaLang 1.0. Dieses Dokument beschreibt bewusst nur, was tatsaechlich
implementiert ist. Alles, was im NovaLang-1.0-Masterauftrag zusaetzlich
verlangt wird (Klassen, Generics, async, GC, IR/Optimizer, eigener
Assembler/Linker, Debugger, Language Server, Paketverwaltung, UI-Sprache,
usw.) ist **nicht** Teil von v0.1 und wird hier nicht als vorhanden behauptet.

## 1. Zweck dieser Version

Beweis der kompletten Kette:

```
NovaLang-Quelltext (.nova)
  -> Lexer -> Parser -> AST
  -> x86-32-Codegenerator (direkt, keine separate IR in v0.1)
  -> rohe Maschinencode-Bytes (.bin)
  -> tools/nef/nef_main.c "nef build" -> echte .nef-Datei
  -> NovaOS-Kernel (nef_load_and_spawn) -> Ring-3-Ausfuehrung unter QEMU
  -> beobachtbare Ausgabe (serielle Konsole, SYSCALL_CORE_DEBUG_WRITE)
```

Erstes Testprogramm (identisch zum Masterauftrag-Beispiel):

```novalang
Module HelloNova
    Function Main() As Int32
        Console.WriteLine("Hello NovaLang!")
        Return 0
    End Function
End Module
```

## 2. Unterstuetzte Grammatik (v0.1, vollstaendig)

```
CompilationUnit := ModuleDecl
ModuleDecl      := "Module" Ident NEWLINE FunctionDecl* "End" "Module" NEWLINE
FunctionDecl    := "Function" Ident "(" ")" "As" TypeName NEWLINE
                    Statement* "End" "Function" NEWLINE
Statement       := ConsoleWriteLineStmt | ReturnStmt
ConsoleWriteLineStmt := "Console" "." "WriteLine" "(" StringLiteral ")" NEWLINE
ReturnStmt      := "Return" IntLiteral NEWLINE
TypeName        := "Int32"
```

Kommentare: `'` bis Zeilenende (wie in den bestehenden `.nova`-Beispielen unter
`toolchain/nova-studio/examples/`). Leerzeilen werden ignoriert.

Das ist **bewusst** eine sehr kleine Teilmenge der im Masterauftrag
beschriebenen Sprache (Abschnitt 3). Sie wurde nicht erfunden, sondern exakt
auf das Hello-World-Beispiel aus Abschnitt 28 zugeschnitten. Jede
Erweiterung (Parameter, weitere Typen, `Dim`, `If`, Ausdruecke, Module mit
mehreren Funktionen, Klassen, ...) ist ein eigener, spaeterer Schritt mit
eigenem Test.

## 3. Semantik

- Genau ein `Module` mit genau einer `Function Main() As Int32` pro Datei.
- `Console.WriteLine(S)` schreibt die Bytes von `S` plus `\r\n` ueber den
  neuen Syscall `SYSCALL_CORE_DEBUG_WRITE` (siehe Abschnitt 5) auf die
  serielle Konsole. Es gibt noch KEINE allgemeine `Console`-Klasse, keine
  String-Verkettung, keine Formatierung - nur dieser eine woertliche Aufruf.
- `Return N` beendet den Prozess **NICHT wirklich** - `SYSCALL_CORE_EXIT` ist
  im aktuellen Kernel ein Einweg-Mechanismus fuer genau EINEN bestimmten
  Ring-3-Bootstrap-Demoprozess (siehe `kmain.c`-Kommentar zu "Hello Nova":
  "ein zweiter Aufrufer wuerde `scheduler_enable()` ein zweites Mal
  ausloesen"). Ein allgemeiner, fuer beliebige Prozesse sicherer Terminierungs-
  Syscall existiert in NovaOS noch nicht. `Return N` erzeugt deshalb
  (identisch zum bestehenden "Hello Nova"-Testprogramm) eine dauerhafte
  `SYSCALL_CORE_YIELD`-Schleife - der Prozess bleibt als echter, dauerhaft
  laufender Scheduler-Slot bestehen. **Der Rueckgabewert N wird dabei NICHT
  verwendet/propagiert** (weder an einen Exitcode noch sonstwohin). Dies ist
  eine bewusste, dokumentierte Luecke (Masterauftrag Regel 19), keine
  vorgetaeuschte Funktionalitaet. Jedes `Function ... As Int32` MUSS
  trotzdem mit `Return <IntLiteral>` enden (Parserpflicht), um sprachlich
  ehrlich zu bleiben: der Compiler verspricht nicht mehr Semantik, als er
  tatsaechlich einhaelt. Ein echter, fuer beliebige Prozesse sicherer
  Terminierungsmechanismus ist eine noetige Kernel-Erweiterung fuer eine
  spaetere NovaLang-Version (nicht Teil von v0.1).

## 4. Codegenerierung (x86-32, flache Section, keine Relokationen)

- **ZWEI Sections** (NICHT eine - siehe wichtiger Korrektur-Hinweis unten):
  - CODE (READ|EXEC) an `0x00500000` - dieselbe Lade-Konvention wie ALLE
    anderen NEF-/ushell-Prozesse in diesem Kernel (`nef_main.c cmd_build`-
    Kommentar "Konvention dieses Kernels", `ushell_process.c USHELL_CODE_
    VADDR`).
  - RODATA (READ, nicht EXEC) an `0x00400000` - **MUSS** in diesem exakten
    Fenster liegen, siehe naechster Absatz.
- **Wichtiger Korrekturhinweis (boot-verifiziert gefunden):** ein erster
  Entwurf dieser Version legte die String-Literale einfach HINTER den Code
  in DERSELBEN Section (bei `0x00500000+code_size`). Das compiliert und
  validiert einwandfrei, aber `Console.WriteLine` blieb beim echten
  Boot-Test STUMM (kein Absturz, keine Fehlermeldung, einfach keine
  Ausgabe) - `syscall_validate_user_range()` (`syscall.c`) laesst als
  Syscall-Argument-Pointer NUR das feste Fenster `[0x00400000, 0x00404000)`
  zu (`USER_ADDRESS_MIN`/`_MAX`, `syscall.h`) - GENAU das Fenster, das auch
  `ushell_process.c`s `USHELL_ARGS_VADDR` fuer denselben Zweck benutzt,
  getrennt von dessen eigenem `USHELL_CODE_VADDR`. Dieses Fenster einfach
  zu erweitern waere eine ECHTE Sicherheitsregression gewesen: jeder
  Prozess bekommt per `paging_create_address_space()` einen Klon des
  Kernel-Basis-Adressraums - ein `0x00500000`-Pointer ist nur fuer den
  EINEN Prozess sicher, der dort per `paging_map_page_into()` privat
  ueberschrieben hat (siehe `paging.c`-Kommentar zur Seitentabellen-
  Privatisierung); fuer JEDEN ANDEREN Aufrufer wuerde derselbe numerische
  Pointer auf echten, geteilten KERNEL-Speicher zeigen, den
  `SYSCALL_CORE_DEBUG_WRITE` dann unwissentlich auf die serielle Konsole
  leaken wuerde. Der korrekte, sichere Fix: String-Literale in eine EIGENE
  Section im bereits etablierten, validierten Fenster legen (wie oben).
- Aufrufkonvention fuer Syscalls (ermittelt aus `syscall.c syscall_dispatch()`,
  NICHT erfunden): `EAX`=Service (`SYSCALL_SERVICE_CORE`=1), `EBX`=Operation,
  `ECX`=ABI-Version (`SYSCALL_ABI_VERSION`=1), `EDX`/`ESI`=Argument 1/2,
  danach `INT 0x80`.
- `Console.WriteLine(S)` -> `mov edx, <rodata_vaddr_von_S>` (im `0x00400000`-
  Fenster) / `mov esi, <len+2>` / `mov eax, 1` / `mov ebx, 17
  (SYSCALL_CORE_DEBUG_WRITE)` / `mov ecx, 1` / `int 0x80`.
- `Return N` -> dauerhafte Schleife aus `mov eax, 1` / `mov ebx, 13
  (SYSCALL_CORE_YIELD)` / `mov ecx, 1` / `int 0x80` / `jmp` (kurzer
  Ruecksprung zum Schleifenanfang) - siehe Abschnitt 3 fuer die Begruendung
  (kein sicherer allgemeiner Terminierungs-Syscall in diesem Kernel).

## 5. Kernel-Erweiterung: SYSCALL_CORE_DEBUG_WRITE

Vor v0.1 gab es **keinen** Ring-3-Syscall fuer freien Text-Pointer-Output
(nur `SYSCALL_CORE_NOTIFY` mit fester Enum-Liste, siehe
`project_notify_wiring.md`). Ergaenzt in `syscall.h`/`syscall.c`
(Operation 17 unter `SYSCALL_SERVICE_CORE`): `EDX`=Pointer, `ESI`=Laenge
(hart gekappt auf `SYSCALL_DEBUG_WRITE_MAX_LEN`=1024, per
`syscall_validate_user_range()` geprueft, keine Capability-Pruefung,
analog zu YIELD/NOTIFY). Schreibt die Bytes 1:1 auf die serielle Konsole.
Boot-regressionsgetestet (kein Verhaltensunterschied bei den bestehenden
5 NEF-Testprogrammen + 5 Desktop-Prozessen).

## 6. Werkzeugkette (wiederverwendet, nicht neu erfunden)

- `novalang/compiler/novac.exe` (dieses Projekt): `.nova` -> `<praefix>.code.bin`
  + `<praefix>.rodata.bin` (rohe x86-32-Maschinencode-/Daten-Bytes, siehe
  Abschnitt 4 fuer die Zwei-Sections-Begruendung).
- `tools/nef/nef_main.c` (bereits vorhanden, Phase 3 des NEF-Masterauftrags):
  `nef link <out.nef> --entry 0x00500000 --section CODE:5:0x00500000:4096:<praefix>.code.bin
  --section RODATA:1:0x00400000:4096:<praefix>.rodata.bin` -> echte, validierte
  `.nef`-Datei. NovaLang baut **keinen eigenen NEF-Builder** - das waere unnoetige
  Doppelarbeit, siehe Masterauftrag Abschnitt 1 ("vorhandene funktionierende
  Komponenten nicht unnoetig ersetzen").
- Einspielen ins laufende Boot-Image: wie bei den 5 bestehenden Testprogrammen
  per `novafs_create()`/`novafs_write()` AUS DEM KERNEL (siehe
  `feedback_novafs_host_tool_write_bug.md` - niemals per `novafs put`).

## 7. Explizit NICHT Teil von v0.1

Parameter/Rueckgabewerte ausser dem festen `Return <Int>`-Muster, Variablen
(`Dim`), Ausdruecke/Arithmetik, Kontrollfluss (`If`/`For`/`While`/...),
mehrere Funktionen/Module, Klassen/Structs/Enums, Generics, Nova IR,
Optimizer, eigener Assembler (`nvas`)/Linker (`nvld`) statt `nef_main.c`,
Nova Runtime/Standardbibliothek, `.nui`, Build-System (`nvbuild`),
Paketverwaltung (`nvpkg`), Debugger, Language Server, Formatter/Linter,
Testframework, Self-Hosting. Alles das bleibt im Masterauftrag stehen und
wird in spaeteren, eigenen Schritten nachgezogen - siehe
`project_novalang_toolchain.md` (Fortschritts-Memo) fuer den aktuellen Stand.
