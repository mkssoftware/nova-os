# NovaLang 0.5 – Erweiterung: Syscall-Argument-Puffer (Pointer-Support)

Baut auf v0.1-v0.4 auf. Behebt die in `NOVALANG_SPEC_v0.4.md` Abschnitt 2
dokumentierte Luecke: `SysCall(...)` konnte bisher nur skalare Argumente
uebergeben - VFS-/Input-Syscalls brauchen aber einen POINTER auf eine
Argument-Struct. Testprogramm (`novalang/examples/hello_vfs.nova`),
boot-verifiziert mit korrektem Ergebnis ("vfs lookup via syscall
succeeded"): ein ECHTER `VFS_OP_LOOKUP`-Syscall findet `/hello_nova.nef`
auf NovaFS.

## 1. Neue Grammatik

```
DimStmt    := ... | "Dim" Ident "As" "Buffer" "(" IntLiteral ")" NEWLINE
PokeStmt   := "PokeInt" "(" Ident "," IntLiteral "," SysArg ")" NEWLINE
SysArg     := IntLiteral | Ident | "AddressOf(" Ident ")" | "PeekInt(" Ident "," IntLiteral ")"
IntTerm    := ... | "AddressOf(" Ident ")" | "PeekInt(" Ident "," IntLiteral ")"
```

`PokeString(buf, byteOffset, StringExpr)` wird wie `Dim` SOFORT beim
Parsen aufgeloest (reine Compile-Zeit-Dateninitialisierung, siehe
Abschnitt 3) und erzeugt deshalb keinen eigenen Grammatik-Eintrag als
Statement im klassischen Sinn - syntaktisch sieht es wie ein Aufruf aus.

Testprogramm-Ausschnitt:

```novalang
Dim pathbuf As Buffer(16)
Dim argsbuf As Buffer(20)
Dim result As Integer = 999

PokeString(pathbuf, 0, "/hello_nova.nef")

PokeInt(argsbuf, 0, 0)                       ' dir_handle
PokeInt(argsbuf, 4, AddressOf(pathbuf))       ' path_addr
PokeInt(argsbuf, 8, 15)                       ' path_len
PokeInt(argsbuf, 12, 0)                       ' flags
PokeInt(argsbuf, 16, 0)                       ' result_handle (out)

result = SysCall(3, 1, AddressOf(argsbuf), 0) ' SYSCALL_SERVICE_VFS, VFS_OP_LOOKUP
```

## 2. Semantik

- **`Dim buf As Buffer(N)`**: reserviert `N` zero-initialisierte Byte in
  einer EIGENEN, SCHREIBBAREN NEF-Section (siehe Abschnitt 4) - NICHT in
  der normalen `DATA`-Section der Integer-Variablen (die liegt AUSSERHALB
  des validierten Syscall-Pointer-Fensters, siehe
  `feedback_syscall_pointer_window.md`).
- **`AddressOf(buf)`**: liefert die absolute Adresse des Puffers als
  `Integer`-Wert - VOLLSTAENDIG zur COMPILE-ZEIT aufgeloest (Puffer-
  Adressen stehen fest, es gibt keine Laufzeit-Zeigerarithmetik). Verhaelt
  sich danach exakt wie ein normales Integer-Literal.
- **`PeekInt(buf, byteOffset)`**: liest 4 Byte aus `buf` an `byteOffset`
  (LAUFZEIT-Lesezugriff, z.B. um `result_handle` nach einem Syscall
  auszuwerten) - Adresse compile-time bekannt, Lesevorgang selbst
  tatsaechlich zur Laufzeit ausgefuehrt. Ueberall einsetzbar, wo auch ein
  Integer-Term stehen darf (inklusive `+`/`-`-Ketten - anders als
  `SysCall`, siehe Abschnitt 3, gibt es hier kein Registerkonflikt-
  Problem: `PeekInt` nutzt nur EAX).
- **`PokeInt(buf, byteOffset, value)`**: schreibt 4 Byte (`value` =
  Literal/Variable/`AddressOf`/`PeekInt`) an eine compile-time-bekannte
  absolute Adresse - LAUFZEIT-Schreibzugriff.
- **`PokeString(buf, byteOffset, StringExpr)`**: schreibt compile-time-
  bekannte String-Bytes (z.B. einen Dateipfad) DIREKT in das Programm-
  Image - KEIN Laufzeit-Code, identisch zur Natur von `Dim`-Initialwerten.

## 3. Wichtiger Bug, boot-verifiziert gefunden+behoben

Ein erster Entwurf legte die Puffer-Region einfach HINTER die String-
Literale in DERSELBEN `RODATA`-Section (read-only). Das compilierte und
validierte einwandfrei, fuehrte aber beim echten Boot-Test zu
`EXCEPTION (kein Exception-Manager aktiv) - HALT` - ein echter
Seitenfehler, weil `PokeInt` (und der VFS-Syscall selbst, der
`result_handle` in die Argument-Struct zurueckschreibt) in eine
READ-ONLY gemappte Seite schreiben wollte. Fix: eine EIGENE,
SCHREIBBARE NEF-Section fuer die Puffer-Region, getrennt von der
weiterhin echten read-only `RODATA`-Section. Lehrstueck (vgl.
`feedback_scheduler_security_capacity_drift.md`): "kompiliert und
validiert erfolgreich" ist KEIN Nachweis korrekter Speicherschutz-
Eigenschaften - nur der echte Boot-Test deckte das auf.

Zweiter, kleinerer Bug (ebenfalls boot-naher Pfad, aber vor dem
eigentlichen Boot-Test im Bytecode-Vergleich gefunden): `NcSysArg
args[4];` in `parse_assign_syscall()` war ein nicht-nullinitialisiertes
Stack-Array - das `is_peek`-Feld eines rein literalen Arguments enthielt
dadurch zufaelligen Stack-Muell, den der Codegenerator als "das ist ein
PeekInt mit Muell-Adresse" fehlinterpretierte. Fix: `memset(args, 0,
sizeof(args));` vor dem Befuellen.

## 4. Codegenerierung / NEF-Layout

Jetzt bis zu VIER Sections (vorher drei):

- CODE (R|X) bei `0x00500000`.
- RODATA (R) bei `0x00400000`, bis zu `NC_RODATA_MAX_SIZE` = 12 KiB -
  NUR String-Literale, bleibt echt read-only.
- **BUFFER (R|W, NEU) bei `0x00403000`** (= `NC_RODATA_VADDR +
  NC_RODATA_MAX_SIZE`, direkt hinter dem String-Bereich, noch innerhalb
  des validierten 16-KiB-Fensters) - NUR wenn `Dim ... As Buffer`
  verwendet wurde. `tools/nef/nef_main.c` kennt keinen eigenen
  "BUFFER"-Sections-Typ - wird als zweite Section vom NEF-Typ `DATA`
  gelinkt (der Typ ist nur eine Klassifizierung, keine
  Eindeutigkeitsbedingung).
- DATA (R|W) bei `0x00480000` - unveraendert, Integer-Variablen.

## 5. Explizit weiterhin NICHT Teil dieser Version

Siehe v0.1-v0.4 - zusaetzlich: `For`/`While`, Funktionsparameter, echte
Laufzeit-Strings (variable Laenge, Verkettung zur Laufzeit), ein neuer
Prozess-Spawn-Syscall, mehr als 16 Puffer/4 KiB Puffer-Region insgesamt,
`SysArg`/`PeekInt`-Verschachtelung ueber eine Ebene hinaus.
