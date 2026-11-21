# NovaLang 0.2 – Erweiterung: Dim-Variablen + String-Verkettung

Baut auf `NOVALANG_SPEC_v0.1.md` auf (bitte zuerst lesen - diese Version
aendert NICHTS an v0.1s Grundlagen: Status als vertikaler Meilenstein,
Syscall-Konventionen, Zwei-Sections-Layout, `Return`-Semantik bleiben
unveraendert). Hier steht nur das NEU Hinzugekommene.

## 1. Zweck

Beweis, dass der Compiler echte Sprachkonstrukte ueber reine Literale
hinaus verarbeiten kann, ohne die Sicherheits- und Lade-Eigenschaften aus
v0.1 zu verletzen. Testprogramm (`novalang/examples/hello_variables.nova`):

```novalang
Module HelloVariables
    Function Main() As Int32
        Dim greeting As String = "Hello"
        Dim name As String = "NovaLang"
        Dim message As String = greeting & " " & name & "!"
        Console.WriteLine(message)
        Console.WriteLine("Second line works too")
        Return 0
    End Function
End Module
```

Boot-verifiziert: beide `WriteLine`-Zeilen erscheinen korrekt im seriellen
Boot-Log ("Hello NovaLang!" gefolgt von "Second line works too").

## 2. Neue Grammatik

```
Statement       := ConsoleWriteLineStmt | ReturnStmt | DimStmt
DimStmt         := "Dim" Ident "As" "String" "=" StringExpr NEWLINE
StringExpr      := StringTerm ("&" StringTerm)*
StringTerm      := StringLiteral | Ident
ConsoleWriteLineStmt := "Console" "." "WriteLine" "(" StringExpr ")" NEWLINE
```

(`ReturnStmt` unveraendert aus v0.1.) `Function Main()` darf jetzt beliebig
viele `Console.WriteLine`/`Dim`-Anweisungen in beliebiger Reihenfolge vor
dem abschliessenden `Return` enthalten (v0.1 erlaubte implizit auch schon
mehrere `WriteLine`, war aber nie damit getestet - jetzt boot-verifiziert).

## 3. Semantik - WICHTIGE EINSCHRAENKUNG

**`Dim` und `&` werden VOLLSTAENDIG zur Compile-Zeit in `novac` aufgeloest,
NICHT zur Laufzeit im generierten Maschinencode.** Es gibt in v0.2 KEINEN
Laufzeit-Speicher fuer Variablen, keine Heap-/Stack-Allokation fuer Strings,
keinen echten String-Verkettungs-Code. `novac` fuehrt intern eine einfache
Compile-Zeit-Symboltabelle (Name -> bereits vollstaendig verketteter
String-Wert), wertet jede `&`-Kette sofort aus und behandelt eine
Variablen-Referenz in `WriteLine(x)` oder in einer weiteren `&`-Kette exakt
wie ein Literal mit demselben Inhalt. Der erzeugte Maschinencode fuer
`WriteLine(message)` ist dadurch BYTE-IDENTISCH zu dem, den `WriteLine
("Hello NovaLang!")` erzeugt haette - es gibt keinen Unterschied im
Codegen zwischen einer Variable und einem Literal.

Das ist eine bewusste, dokumentierte Vereinfachung (Masterauftrag Regel 19),
keine vorgetaeuschte Variablenunterstuetzung: `Dim` verhaelt sich hier wie
eine benannte Compile-Zeit-Konstante (vergleichbar `Const`, nicht wie eine
echte veraenderliche Laufzeitvariable). Folgen dieser Einschraenkung:
- Keine Neuzuweisung (`Dim x ...` gefolgt von `x = ...`) - der Parser lehnt
  eine zweite `Dim`-Deklaration desselben Namens explizit ab.
- Kein `Dim` mit Laufzeitwert (z.B. aus einer Funktion oder Benutzereingabe
  berechnet) - nur `StringLiteral`/`Ident`-Ketten.
- Keine anderen Typen als `String` (`Dim x As Integer` wird abgelehnt).

Eine ECHTE Laufzeit-Variable (mit Speicher, der sich zur Laufzeit aendern
kann) braucht eine Laufzeit-Speicherverwaltung (mindestens einen
beschreibbaren DATA/BSS-Abschnitt plus Stack-Frame-Layout fuer lokale
Variablen) - das ist ein eigener, spaeterer Schritt (v0.3+), kein Teil
dieser Version.

## 4. Keine neuen Kernel-/NEF-Aenderungen

Diese Version brauchte KEINE weitere Kernel- oder Toolchain-Aenderung -
dasselbe Zwei-Sections-Layout (`CODE` bei `0x500000`, `RODATA` bei
`0x400000`) und derselbe `SYSCALL_CORE_DEBUG_WRITE`-Syscall aus v0.1 reichen
unveraendert aus, weil `Dim`/`&` ausschliesslich beim Befuellen der RODATA-
Section wirken (mehr/andere String-Bytes an denselben Adressen), nicht beim
Codegen der Syscall-Aufrufe selbst.

## 5. Explizit weiterhin NICHT Teil dieser Version

Siehe `NOVALANG_SPEC_v0.1.md` Abschnitt 7 - zusaetzlich: echte Laufzeit-
Variablen/-Zuweisung, Integer-Typen fuer `Dim`, Ausdruecke mit Operatoren
ausser `&`, Kontrollfluss (`If`/`For`/`While`), Funktionsparameter,
mehrere Funktionen/Module.
