# NovaLang 0.7 – Erweiterung: Byte-Level-Puffer-Zugriff und Laufzeit-"Strings"

Baut auf v0.1-v0.6 auf. Schliesst die in `project_novashell.md` priorisierte
Luecke "echte Laufzeit-Strings" - NICHT durch einen neuen `String`-
Laufzeittyp mit Heap/Verkettung, sondern durch die kleinste Erweiterung,
die dieselbe Faehigkeit tatsaechlich liefert: Byte-genauer Lese-/
Schreibzugriff auf `Buffer`-Puffer (`PeekByte`/`PokeByte`, zusaetzlich zu
den 4-Byte-Varianten aus v0.5) plus Textausgabe mit einer LAUFZEIT-Laenge
(`Console.WriteBuffer`, statt der bisherigen `Console.WriteLine`, deren
Laenge immer aus einem Compile-Zeit-Literal stammt).

## 1. Neue Grammatik

```
Statement   := ... | PokeByteStmt | WriteBufferStmt
PokeByteStmt := "PokeByte" "(" Ident "," IntLiteral "," SysArg ")" NEWLINE
IntTerm     := ... | "PeekByte(" Ident "," IntLiteral ")"
SysArg      := ... | "PeekByte(" Ident "," IntLiteral ")"
WriteBufferStmt := "Console" "." "WriteBuffer" "(" Ident "," SysArg ")" NEWLINE
```

Testprogramm (`novalang/examples/hello_bytes.nova`), boot-verifiziert:
baut `"Hello Bytes!\r\n"` BYTEWEISE per `PokeByte` auf (14 einzelne
Aufrufe, kein String-Literal), gibt den Puffer per `Console.WriteBuffer`
mit einer `Integer`-Variable als Laenge aus, liest per `PeekByte` das
erste Byte zurueck und prueft es.

## 2. Semantik

- **`PeekByte(buf, offset)`/`PokeByte(buf, offset, value)`**: wie die
  v0.5-`PeekInt`/`PokeInt`-Varianten, aber genau 1 Byte statt 4. `PokeByte`
  schreibt NUR das niedrigste Byte von `value` (ein Wert > 255 wird
  stillschweigend abgeschnitten - Standard-x86-`mov [addr],al`-Verhalten,
  keine zusaetzliche Pruefung). `PeekByte` liefert IMMER einen Wert im
  Bereich 0..255 (Zero-Extend, keine Vorzeichenerweiterung).
- **`Console.WriteBuffer(buf, laenge)`**: gibt `laenge` Byte aus `buf`
  1:1 auf die serielle Konsole aus. **KEIN automatisches `"\r\n"`**
  (anders als `WriteLine`) - der Aufrufer poked ein etwaiges Zeilenende
  selbst in den Puffer, falls gewuenscht (wie im Testprogramm). `laenge`
  ist typischerweise eine `Integer`-Variable - das ist die eigentliche
  Neuerung: `WriteLine`s Laenge steht IMMER zur Compile-Zeit fest (Laenge
  des String-Literals), `WriteBuffer`s Laenge ist eine echte Laufzeit-
  Groesse.

## 3. Codegenerierung

Neue x86-Bausteine:
- `movzx <reg>, byte [addr]` (Opcode `0F B6 /r`) fuer `PeekByte` - BEWUSST
  nicht `mov al,[addr]`: ein reiner 8-Bit-Load liesse die oberen 24 Bit
  des Zielregisters undefiniert, was fuer einen Ausdruckswert (z.B. in
  einem Vergleich) unsicher waere.
- `mov [addr], al` (Opcode `0x88 /r`) fuer `PokeByte`.
- `add`/`sub eax, <reg>` (Register-Register, `0x03`/`0x2B` mit `mod=11`)
  als Scratch-Operation fuer `PeekByte` INNERHALB einer `+`/`-`-Kette
  (`x = PeekByte(a,0) + PeekByte(a,1)`) - es gibt keinen einzelnen x86-
  Opcode fuer "addiere ein zero-extended Byte aus dem Speicher direkt zu
  EAX", daher wird ECX als Scratch-Register benutzt (ERST `movzx ecx,
  byte[addr]`, DANN `add/sub eax,ecx`).

`Console.WriteBuffer` ist strukturell identisch zu `WriteLine`s Syscall-
Aufruf-Codegen (EDX=Pufferadresse, ESI=Laenge, `SYSCALL_CORE_DEBUG_WRITE`),
nur dass EDX ein fester Puffer-Adress-Literal ist und ESI (die Laenge)
jetzt ueber die volle `SysArg`-Flexibilitaet (Literal/Variable/
`PeekInt`/`PeekByte`) geladen wird statt immer ein Compile-Zeit-Literal
zu sein.

## 4. Keine neuen Kernel-Aenderungen

Reine Codegen-/Compiler-Erweiterung auf Basis bereits vorhandener
Bausteine (derselbe `SYSCALL_CORE_DEBUG_WRITE` wie seit v0.1).

## 5. Explizit weiterhin NICHT Teil dieser Version

Siehe v0.1-v0.6 - zusaetzlich: ein echter `String`-Laufzeittyp mit
dynamischer Laenge/Heap-Verwaltung, Laufzeit-Stringverkettung,
Zeichenkette-zu-Zahl/Zahl-zu-Zeichenkette-Konvertierung (`CStr`/`CInt`),
Lesen von Benutzereingaben (Tastatur/`SYSCALL_SERVICE_INPUT`) in einen
solchen Puffer - das naechste sinnvolle Testprogramm dafuer, aber noch
nicht in dieser Version.
