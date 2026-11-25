# NovaLang 0.11 – Erweiterung: VKEY->ASCII-Uebersetzung

Baut auf v0.1-v0.10 auf. Reine NovaLang-Function (siehe v0.9 "mehrere
Functions"), KEINE neue Compiler-Grammatik und KEINE Kernel-Aenderung
noetig. Schliesst den in v0.10 offen gelassenen Punkt: eine echte
VKEY->ASCII-Uebersetzung fuer NovaShells geplante REPL-Eingabezeile.

## 1. Kernel-seitige Erkenntnis (keine Erfindung, aus `ps2.c` gelesen)

`ps2.c ps2_set1_table[]` liefert fuer Buchstaben/Ziffern/Satzzeichen
bereits den ASCII-Code SELBST als `vkey`-Wert (z.B. die A-Taste ->
`vkey == 'A' == 65`, IMMER Grossbuchstabe unabhaengig vom Shift-Status
- die Shift-Taste wird separat im `modifiers`-Bitfeld gemeldet, siehe
`syscall_input_poll_args_t`). NUR Sondertasten liegen im Bereich
`0x0100`+ (`ps2.h`): `VKEY_ESCAPE`=256, `VKEY_BACKSPACE`=257,
`VKEY_TAB`=258, `VKEY_ENTER`=259, `VKEY_LSHIFT`=260, ... bis
`VKEY_LSUPER`=275.

Die Uebersetzung ist dadurch TRIVIAL - keine 95-Eintrags-Tabelle
noetig:
- `vkey` im druckbaren ASCII-Bereich `[32,126]` -> 1:1 durchreichen
  (ist bereits der korrekte ASCII-Code).
- `vkey == 256` (Escape) -> `27`.
- `vkey == 257` (Backspace) -> `8`.
- `vkey == 258` (Tab) -> `9`.
- `vkey == 259` (Enter) -> `13` (CR).
- alles andere (Pfeiltasten, Shift/Ctrl/Alt, Capslock, Entf, Pos1/Ende,
  Super, F4) -> `0` ("nicht druckbar", vom Aufrufer zu ignorieren).

## 2. `VkeyToAscii(vkey As Integer) As Int32`

```novalang
Function VkeyToAscii(vkey As Integer) As Int32
    If vkey >= 32 Then
        If vkey <= 126 Then
            Return vkey
        End If
    End If
    If vkey = 256 Then
        Return 27
    End If
    If vkey = 257 Then
        Return 8
    End If
    If vkey = 258 Then
        Return 9
    End If
    If vkey = 259 Then
        Return 13
    End If
    Return 0
End Function
```

Eine Bereichspruefung (`lhs >= N1 AND lhs <= N2`) ist in der v0.3-
`Cond`-Grammatik NICHT als einzelner Ausdruck moeglich (kein `And`) -
gleichwertig und ohne neue Grammatik durch ZWEI VERSCHACHTELTE `If`s
gelost, die sich dank v0.9s echtem `Return` (sofortiges `ret`, keine
Fall-Through-Gefahr) wie ein kurzgeschlossenes `And` verhalten.

An `hello_syscall.nova` angehaengt (siehe Abschnitt 4, NICHT als eigene
Datei).

## 3. Wichtige Einschraenkung (ehrlich benannt, wie schon in v0.10)

Diese QEMU-Testumgebung kann weiterhin KEINE echten Tastendruecke per
QMP injizieren. `VkeyToAscii` wird deshalb mit bekannten, LITERALEN
`vkey`-Werten getestet (deterministischer Unit-Test einer reinen,
seiteneffektfreien Function) - das ist eine echte Boot-Verifikation
DES CODES (der tatsaechlich im Kernel laufende, compilierte NovaLang-
Code wird mit konkreten Eingaben durchlaufen und sein Ergebnis per
seriellem Output ueberprueft), NICHT eine Simulation von Hardware-
Verhalten. Ein Test mit einem ECHTEN Tastendruck bleibt offen, bis ein
Verifikationsweg fuer diese Umgebung existiert (analog zum
synthetischen `input_self_test()` aus `project_keyboard_shortcuts.md`).

## 4. Boot-verifiziert

Fuenf Testfaelle in `hello_syscall.nova` (Main ruft `VkeyToAscii`
fuenfmal mit Literalen auf - eine ECHTE Vorwaertsreferenz-Funktions-
aufruf-Kette, genau das Szenario, fuer das v0.9s Backpatching gebaut
wurde): `VkeyToAscii(65)=65` (Buchstabe A), `VkeyToAscii(53)=53`
(Ziffer 5), `VkeyToAscii(259)=13` (Enter), `VkeyToAscii(257)=8`
(Backspace), `VkeyToAscii(267)=0` (Pfeiltaste hoch, nicht druckbar).
Serial-Output bestaetigt alle fuenf: `"vkey letter A translated ok"`,
`"vkey digit 5 translated ok"`, `"vkey enter translated ok"`,
`"vkey backspace translated ok"`, `"vkey arrow correctly non-
printable"`. Voller Boot bis `NOVA_KERNEL_READY`, alle bisherigen NEF-/
NovaLang-Testprogramme weiterhin korrekt.

## 5. Explizit weiterhin NICHT Teil dieser Version

Echte Boot-Verifikation mit einem tatsaechlichen Tastendruck, eine
echte Eingabezeile/REPL-Schleife mit Zeichen-Puffer und Backspace/
Enter-Behandlung (naechster logischer Schritt), Shift-abhaengige
Gross-/Kleinschreibung (liefert bisher immer Grossbuchstaben),
Sondertasten-Kombinationen (Strg+C etc.).
