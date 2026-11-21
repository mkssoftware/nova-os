# NovaLang 0.3 – Erweiterung: echte Laufzeit-Integer-Variablen, Arithmetik, If/Else

Baut auf `NOVALANG_SPEC_v0.1.md`+`v0.2.md` auf. Hier steht nur das NEU
Hinzugekommene.

## 1. Zweck

Erster echter Kontrollfluss: `Dim x As Integer`, Zuweisung mit `+`/`-`,
`If`/`Else`/`End If` mit Vergleichsoperatoren - ALLES zur LAUFZEIT
ausgefuehrt (anders als v0.2s rein compile-time String-`Dim`). Testprogramm
(`novalang/examples/hello_if.nova`), boot-verifiziert mit korrektem
Ergebnis:

```novalang
Dim counter As Integer = 5
counter = counter + 10        ' echte Laufzeit-Addition
If counter = 15 Then          ' -> wahr, "counter is 15, as expected"
    ...
Else
    ...
End If
```

## 2. Neue Grammatik

```
Statement   := ... | AssignStmt | IfStmt
DimStmt     := "Dim" Ident "As" ("String" "=" StringExpr | "Integer" "=" IntLiteral) NEWLINE
AssignStmt  := Ident "=" IntExpr NEWLINE
IntExpr     := IntTerm (("+" | "-") IntTerm)*
IntTerm     := IntLiteral | Ident
IfStmt      := "If" Ident CmpOp (IntLiteral | Ident) "Then" NEWLINE
                 Statement*
               ("Else" NEWLINE Statement*)?
               "End" "If" NEWLINE
CmpOp       := "=" | "<>" | "<" | ">" | "<=" | ">="
```

Einschraenkungen (bewusst, siehe Abschnitt 5): die linke Seite einer
`If`-Bedingung MUSS eine Integer-Variable sein (kein beliebiger Ausdruck),
`Ident` in `AssignStmt`/`IfStmt` MUSS bereits per `Dim ... As Integer`
deklariert sein.

## 3. Codegenerierung (x86-32)

Dritte NEF-Section, zusaetzlich zu CODE/RODATA aus v0.1:

- **DATA** (READ|WRITE) bei `0x00480000` - je eine 4-Byte-Zelle pro
  `Dim ... As Integer`-Deklaration, Reihenfolge = Deklarationsreihenfolge,
  mit dem Initialwert vorbefuellt. Braucht KEINE Pointer-Validierung (wird
  nie an einen Syscall uebergeben) - Adresse frei waehlbar zwischen
  RODATA-Fenster-Ende (`0x404000`) und CODE (`0x500000`).
- Speicherzugriff: direkte 32-Bit-Adressierung ohne Basisregister
  (ModRM `0x05` + `disp32`) - deckt `mov eax,[addr]`/`mov [addr],eax`/
  `add eax,[addr]`/`sub eax,[addr]`/`cmp eax,[addr]` ab (einzige
  benoetigte Speicherzugriffsform).
- `counter = counter + 10` -> `mov eax,[addr_counter]` / `add eax,10` /
  `mov [addr_counter],eax`.
- `If lhs <op> rhs Then ... [Else ...] End If` -> `mov eax,[addr_lhs]` /
  `cmp eax,<rhs>` / **invertierter** bedingter Sprung (z.B. `=` -> `jne`)
  ueber den Then-Koerper, bei vorhandenem `Else` ein zusaetzlicher
  unbedingter `jmp` am Then-Ende ueber den Else-Koerper. Sprungziele
  werden per Backpatching aufgeloest (Opcode+Platzhalter-Byte emittieren,
  NACH Erzeugung des Zielblocks das tatsaechliche `rel8`-Offset
  eintragen).
- **Bewusste v0.3-Grenze:** nur KURZE Spruenge (`rel8`, -128..127 Byte).
  Ein If-Koerper, der dieses Limit sprengt, fuehrt zu einem ehrlichen
  Compilerfehler ("Codegenerierung fehlgeschlagen"), NIEMALS zu
  stillschweigend falschem Code. Fuer v0.3-Testprogramme (wenige
  WriteLine/Dim/Return pro Zweig) reicht das aus; eine spaetere Version
  kann auf `rel32` wechseln, falls If-Koerper groesser werden.

## 4. Kernel-Erweiterung: SCHEDULER_THREAD_COUNT 20 -> 40

Jedes neue dauerhafte Testprogramm (egal ob NEF-hand-assembliert oder
NovaLang-kompiliert) belegt einen eigenen Scheduler-Slot (siehe
`feedback_scheduler_security_capacity_drift.md`). Mit dem 8. Testprozess
(`hello_if.nef`) war die vorherige Grenze von 20 erreicht
(`scheduler_claim_slot` scheiterte beim Desktop-Icons-Ushell-Spawn, exakt
dasselbe Symptom wie beim fruehern Vorfall). Diesmal bewusst GROSSZUEGIG
auf 40 angehoben (nicht nur +1), da im Rahmen des NovaShell-
Entwicklungsauftrags weitere Testprogramme folgen werden - siehe
`scheduler.h`-Kommentar fuer die vollstaendige Historie. `SECURITY_
CAPACITY` war NICHT betroffen (NovaLang-Testprogramme fordern bisher
keine Capabilities an).

## 5. Explizit weiterhin NICHT Teil dieser Version

Siehe v0.1/v0.2 Abschnitt 7 - zusaetzlich: beliebige Ausdruecke in
If-Bedingungen (nur Variable-vs-Literal/Variable), `ElseIf`, verschachtelte
Vergleiche, Integer-Multiplikation/-Division, `For`/`While`-Schleifen,
Funktionsparameter, Arrays, Strings als Laufzeit-Objekte (weiterhin nur
compile-time via v0.2), Zeichenketten-zu-Zahl-Konvertierung (`CStr`/
`CInt`), echter Terminierungs-Syscall.
