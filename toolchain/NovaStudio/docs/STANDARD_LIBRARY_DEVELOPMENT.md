# Nova Studio – Entwicklung der NovaLang-Standardbibliothek

## Status: NICHT IMPLEMENTIERT (kein Projekttyp, kein Bootstrap-Modus)

Es gibt in Nova Studio keinen "Standardbibliotheks"-Projekttyp und
keinen Bootstrap-Build-Modus.

## Warum - der eigentliche Befund

Geprüft (Quelltext-Suche in `novalang/compiler/src/`): `Console.
WriteLine(...)` ist ein **fest in den Parser eingebauter Spezialfall**
(`parser.c`, Fehlermeldung "Anweisung erwartet (Dim/Console.WriteLine/
Return/If/While/Zuweisung/PokeInt/PokeByte/PokeString)" listet es als
eigene Grammatik-Alternative neben Schlüsselwörtern wie `Dim`/`Return`).
Es ist **keine Bibliotheksfunktion**, die über ein Modul-/Import-System
aufgelöst wird - es gibt aktuell gar kein solches System. `Imports
Nova.System` (siehe `taschenrechner.nova`-Beispiel in der älteren,
nicht verwendeten `toolchain/novalang/`-Implementierung) wird vom
AKTIVEN Compiler (`novalang/compiler/`) nicht einmal als Sprachkonstrukt
anerkannt.

Eine "Standardbibliothek" im Sinn des Auftrags (Module wie `Core`,
`Collections`, `IO`, `Threading` mit echten, kompilierbaren,
importierbaren NovaLang-Funktionen) setzt voraus, dass die Sprache
selbst erst ein Modul-/Sichtbarkeits-/Importkonzept UND einen
Mehrdatei-Compiler-Linkschritt bekommt (siehe `CROSS_COMPILATION.md` -
`novac.exe` kompiliert nur eine Datei pro Aufruf). Das ist eine
Erweiterung des Compilers/der Sprache selbst, keine IDE-Funktion.

## Bootstrap-Modus - warum er aktuell nicht sinnvoll definierbar ist

Ein "Bootstrap-Modus" (Standardbibliothek kompilierbar, ohne dass sie
schon existiert) setzt voraus, dass es überhaupt eine Trennung zwischen
"Compiler-Intrinsics", "minimaler Runtime" und "Standardbibliothek"
gibt. Aktuell sind ALLE verfügbaren Operationen (`Console.WriteLine`,
`PokeInt`/`PokeByte`/`PokeString` für Syscall-Puffer, arithmetische
Operatoren) direkt im Parser/Codegenerator verdrahtete Intrinsics - es
gibt keine Zwischenschicht, die eine "Standardbibliothek" ERSETZEN
könnte, weil es noch keine Bibliotheksebene gibt.

## Nächster realistischer Schritt (Compiler-Arbeit, nicht IDE-Arbeit)

1. Ein echtes Modulsystem in der Sprache definieren (Namespace-
   Auflösung, `Imports`, Sichtbarkeit).
2. Mehrdatei-Kompilierung mit echtem Objekt-Link (nicht nur ein
   `.code.bin` pro Aufruf).
3. Erst danach: einen ersten, winzigen Satz an in NovaLang
   geschriebenen (nicht compiler-intrinsischen) Funktionen als Kern
   einer echten Standardbibliothek.

Jeder dieser Schritte ist ein eigenständiges, umfangreiches
Compiler-/Sprach-Vorhaben, keine Erweiterung von Nova Studio selbst.
