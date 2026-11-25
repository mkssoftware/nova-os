# NovaLang 0.15 – PeekByte/PeekInt mit Laufzeit-Offset + echte Begruessungs-Platzhalter

Baut auf v0.1-v0.14 auf. Reine Compiler-Erweiterung (symmetrisch zu
v0.12s `PokeInt`/`PokeByte`-Erweiterung) + Anwendung in `NovaShell.nova`.

## 1. `PeekInt`/`PeekByte` mit Laufzeit-Offset-Variable

Bisher (v0.5/v0.7) war der Byteoffset von `PeekInt`/`PeekByte` IMMER
ein Compile-Zeit-Literal. Neue Grammatik (identisch zu `PokeInt`/
`PokeByte` aus v0.12):

```
PeekExpr := ("PeekInt" | "PeekByte") "(" Ident "," (IntLiteral | Ident) ")"
```

Der Offset ist jetzt ENTWEDER ein Ganzzahlliteral (unveraendert, inkl.
Bereichspruefung) ODER eine bereits deklarierte Integer-Variable (NEU -
keine Bereichspruefung moeglich, der Offset ist erst zur Laufzeit
bekannt).

**Codegen:** neue Basisregister+disp32-Lese-Adressierung
(`emit_reg_from_mem_base_disp32()`, das lesende Gegenstueck zu v0.12s
`emit_mem_base_disp32_from_reg()`): `mov <reg>, [ecx+disp32]` (Opcode
`0x8B`) bzw. `movzx <reg>, byte [ecx+disp32]` (Opcode `0F B6`) -
`<reg>` ist dabei (anders als bei der Poke-Schreibvariante, die nur
AL/EAX kennt) ein BELIEBIGES Zielregister, da Peek in ganz
unterschiedlichen Kontexten vorkommt (SysCall-Argumentregister EAX/
EBX/EDX/ESI, WriteBuffer-Laenge ESI, usw.).

**Bewusste Grenze:** ein Peek mit Laufzeit-Offset-Variable ist NUR als
EINZIGER Term einer IntExpr erlaubt (nicht als Fortsetzungsterm einer
`+`/`-`-Kette) - der Parser lehnt das explizit ab (`parse_int_expr()`).
Grund: der Codegen-Scratch fuer Fortsetzungsterme (`ECX`-Akkumulator
bei `PeekByte`-Ketten, siehe `emit_int_expr()`) wuerde mit dem fuer den
Laufzeit-Offset gebrauchten `ECX` kollidieren. Fuer alle bisherigen
Anwendungsfaelle (einzelne `x = PeekByte(buf, pos)`-Zuweisungen,
SysCall-/Poke-/Call-Argumente) reicht das "einziger Term"-Limit aus.

## 2. Echte Begruessungs-Platzhalter-Ersetzung in NovaShell.nova

`${OS_VERSION}` (13 Byte) wird jetzt per linearer Suche
(`CheckPlaceholderAt(startPos)`, Byte-fuer-Byte-Vergleich ueber die
NEUE Laufzeit-Offset-Peek-Faehigkeit) im gelesenen `greeting.txt`-
Inhalt gesucht. Bei einem Treffer wird eine neue Ausgabe
zusammengesetzt: [Text vor dem Platzhalter] + [Versionszeichenkette]
+ [Text nach dem Platzhalter] - per byteweiser Kopierschleife
(`PeekByte(greetbuf, srcPos)` -> `PokeByte(outbuf, dstPos, ...)`,
BEIDE mit Laufzeit-Offset-Variablen - der erste echte Anwendungsfall,
der Lese- UND Schreib-Laufzeit-Offset in EINER Operation kombiniert).

## 3. Wichtiger, in dieser Version gefundener und behobener Bug (kein Compiler-Bug!)

`CheckPlaceholderAt()` wird in der Suchschleife bis zu 52-mal
aufgerufen. Der erste Versuch scheiterte bei JEDER Position, auch der
tatsaechlich passenden - isolierte Einzelaufruf-Diagnosen (Literal-
Argument, Variablen-Argument, Argument aus einer vorgeschalteten
Schleife) bestanden dagegen alle. Ursache (siehe
`feedback_novalang_dim_initial_is_static.md`): `Dim ok As Integer = 1`
ist ein EINMALIGER `.data`-Startwert, KEIN bei jedem Aufruf neu
ausgefuehrter Reset - nach dem ersten Nicht-Treffer blieb `ok` fuer
immer `0`. Fix: expliziter Laufzeit-Reset `ok = 1` als eigene Zeile
direkt nach der `Dim`-Deklaration. Eine wichtige, bereits ins
Projektgedaechtnis uebernommene Lehre fuer jede kuenftige Function mit
bedingt veraendertem "lokalem" Akkumulator.

## 4. Boot-verifiziert

`"Welcome to NovaOS ${OS_VERSION} - NovaShell (work in progress)"` aus
`greeting.txt` wird korrekt zu `"Welcome to NovaOS 1.0 - NovaShell
(work in progress)"` umgewandelt und ausgegeben (`"greeting
placeholder substituted"`). Voller Boot bis `NOVA_KERNEL_READY`, 0
`"FEHLGESCHLAGEN"`-Zeilen im gesamten Boot-Log (vollstaendiger
Regressionstest aller v0.1-v0.14-Programme bestaetigt).

## 5. Explizit weiterhin NICHT Teil dieser Version

Mehrere verschiedene Platzhalter in einer Datei, echte Function-lokale
Variablen (automatischer Reset bei jedem Aufruf), Peek mit Laufzeit-
Offset als Fortsetzungsterm einer `+`/`-`-Kette, Befehls-Dispatch fuer
weitere Befehle ausser `HELP`.
