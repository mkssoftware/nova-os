# NovaLang 0.12 – Erweiterung: Eingabezeilen-Logik + Laufzeit-Offset-Poke + rel32-Spruenge

Baut auf v0.1-v0.11 auf. Schliesst den letzten offenen Punkt aus
`project_novashell.md`s urspruenglicher Prioritaetenliste: die
eigentliche REPL-Eingabezeilen-Verarbeitung (Zeichen anhaengen,
Backspace, Enter). Enthaelt dabei ZWEI echte, notwendig gewordene
Compiler-Erweiterungen (nicht nur Anwendungscode).

## 1. Neu: `PokeInt`/`PokeByte` mit Laufzeit-Offset-Variable

Bisher (v0.5/v0.7) war der Byteoffset von `PokeInt`/`PokeByte` IMMER
ein Compile-Zeit-Literal - die fertige absolute Adresse wurde komplett
zur Compile-Zeit berechnet. Eine Eingabezeile, die Zeichen an eine
LAUFZEIT-abhaengige Position anhaengt (die aktuelle Zeilenlaenge), kann
das nicht mehr leisten. Neue Grammatik:

```
PokeStmt := ("PokeInt" | "PokeByte") "(" Ident "," (IntLiteral | Ident) "," SysArg ")" NEWLINE
```

Der Offset ist jetzt ENTWEDER ein Ganzzahlliteral (unveraendert, inkl.
Bereichspruefung) ODER eine bereits deklarierte Integer-Variable (NEU -
keine Bereichspruefung moeglich, der Offset ist erst zur Laufzeit
bekannt - dieselbe bewusste Guard-Verantwortung wie bei SysCall-
Argumenten).

**Codegen:** der zu schreibende Wert wird wie bisher nach EAX geladen,
dann (NUR im Variable-Offset-Fall) der Offset nach ECX, dann per neuer
Basisregister+disp32-Adressierung geschrieben: `mov [ecx+disp32], al`
(PokeByte, Opcode `0x88`) bzw. `mov [ecx+disp32], eax` (PokeInt, Opcode
`0x89`), `disp32` = die Puffer-BASISADRESSE. `PeekInt`/`PeekByte`
bleiben in dieser Version bewusst literal-Offset-only (fuer den
REPL-Test unten nicht benoetigt - die Lesezugriffe liessen sich
vollstaendig auf feste Offsets reduzieren).

## 2. Neu: `call`/`jmp`/`jcc`-Spruenge von rel8 auf rel32 aufgewertet

**Boot-verifiziert gefundener, echter Codegen-Fehler:** `novac` scheiterte
mit "RODATA-/Puffer-Limit ueberschritten oder ein If-Koerper zu gross
fuer einen kurzen Sprung" beim Kompilieren des REPL-Tests - ein tief
verschachtelter If-Koerper mit mehreren Function-Aufrufen
(`ProcessOneVkey`, siehe Abschnitt 3) ueberschritt den 8-Bit-Bereich
(±127 Byte) der bisherigen `Jcc`/`JMP`-Kodierung. Der v0.3-Code-
Kommentar hatte das bereits vorausgesehen ("eine spaetere Version
koennte auf rel32 wechseln"). Fix: `emit_jump_placeholder()`/
`patch_jump()`/`emit_jmp_to()` (`codegen_x86.c`) kodieren jetzt
IMMER als `rel32`: `Jcc rel32` = `0F` (Short-Opcode+0x10) `rel32`
(0x70-0x7F mappt linear auf 0x80-0x8F), `JMP rel32` = `E9 rel32`. Deckt
jede realistische Programmgroesse ab - kein Bereichsfehler mehr
moeglich. Betrifft ALLE `If`/`While`/Mains YIELD-Schleife
(rueckwirkend, nicht nur v0.12-Code) - Regressionstest aller v0.1-v0.11-
Beispielprogramme bestaetigt: alle kompilieren weiterhin korrekt (nur
die reine Code-Groesse wuchs leicht, da `rel32`-Spruenge 5-6 statt 2
Byte brauchen).

## 3. `ProcessOneVkey`: die eigentliche Zeilenlogik

An `hello_syscall.nova` angehaengt (siehe `feedback_novafs_leaf_
capacity.md` - keine neue NovaFS-Datei). Verarbeitet EINEN `vkey`-Wert:

```novalang
Function ProcessOneVkey(vk As Integer) As Int32
    If done = 0 Then
        If vk = 259 Then          ' Enter
            done = 1
        Else
            If vk = 257 Then      ' Backspace
                If lineLen > 0 Then
                    lineLen = lineLen - 1   ' "lazy delete" - altes Byte bleibt im Speicher, wird aber nie wieder gelesen
                End If
            Else
                ch = VkeyToAscii(vk)
                If ch <> 0 Then
                    PokeByte(line, lineLen, ch)   ' NEU: Laufzeit-Offset
                    lineLen = lineLen + 1
                End If
            End If
        End If
    End If
    Return 0
End Function
```

`done`/`line`/`lineLen`/`ch` sind `Main`s `Dim`-Variablen/-Puffer - in
dieser Sprachversion sind `Dim`s bewusst modulglobal (keine Function-
lokalen Namensraeume, siehe `NOVALANG_SPEC_v0.9.md` Abschnitt 6), daher
kann `ProcessOneVkey` direkt darauf zugreifen, OHNE dass sie als
Parameter durchgereicht werden muessen. Kein `Exit While`/`Break` in
dieser Sprachversion - das fruehe Verlassen bei Enter wird per
Waechter-Flag (`done`) nachgebildet (derselbe Stil wie die
Zero-Iteration-While-Pruefung in `hello_while.nova`).

## 4. Wichtige Einschraenkung (unveraendert von v0.10/v0.11, ehrlich benannt)

Diese QEMU-Testumgebung kann weiterhin KEINE echten Tastendruecke per
QMP injizieren. Der Test verarbeitet deshalb eine Folge SYNTHETISCHER
(per `PokeInt` buchstaeblich "getippter") `vkey`-Werte durch GENAU den
Code-Pfad, den ein echter Tastendruck durchlaufen wuerde (`PeekInt` ->
`ProcessOneVkey` -> `VkeyToAscii` -> `PokeByte`) - eine echte Boot-
Verifikation des tatsaechlich im Kernel laufenden Codes mit konkreten
Eingaben, keine Simulation von Hardware-Verhalten.

## 5. Boot-verifiziert

Synthetische Eingabe: `H`(72), `i`(105), Backspace(257), `!`(33),
Enter(259) - erwartetes Ergebnis: Zeile `"H!"` (H bleibt, i wird durch
Backspace entfernt, `!` wird angehaengt), Laenge 2. Serial-Output
bestaetigt: `"repl line loop finished"`, `"repl line length correct"`,
`"repl line first char correct"`, `"repl line second char correct
after backspace+retype"`, und die tatsaechlich per `Console.
WriteBuffer` ausgegebene Zeile `"H!"`. Voller Boot bis
`NOVA_KERNEL_READY`, alle bisherigen NEF-/NovaLang-Testprogramme
weiterhin korrekt (inkl. Regressionstest aller v0.1-v0.11-Beispiele
gegen den neuen rel32-Codegen).

## 6. Explizit weiterhin NICHT Teil dieser Version

Echte Boot-Verifikation mit einem tatsaechlichen Tastendruck, eine
ECHTE, dauerhaft laufende REPL-Schleife (diese Version verarbeitet eine
FESTE, bereits im Speicher stehende Sequenz, keine Live-Polling-
Schleife bis Enter), Function-lokale `Dim`-Namensraeume, `PeekInt`/
`PeekByte` mit Laufzeit-Offset (nur `Poke*` bekam das in dieser
Version), Zeilen-Editierung ueber Backspace-Position hinaus
(Pfeiltasten/Cursor-Bewegung), History/Mehrzeilen-Eingabe.
