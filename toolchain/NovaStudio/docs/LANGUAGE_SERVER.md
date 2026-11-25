# Nova Studio – NovaLang Language Server

## Status: NICHT IMPLEMENTIERT

Es existiert kein eigenständiger NovaLang Language Server (kein
LSP-Prozess, keine `textDocument/*`-Handler, keine JSON-RPC-Anbindung).

## Warum

Der verbindliche Auftrag verlangt, den Language Server auf der
GLEICHEN Lexer-/Parser-/Typsystem-/Diagnostik-Implementierung
aufzubauen wie der Compiler ("Implementiere keinen zweiten
unabhängigen NovaLang-Compiler"). Die reale, aktiv gepflegte
Compiler-Implementierung (`novalang/compiler/src/`) ist dafür aktuell
NICHT geeignet:

- `novac.exe` ist ein reiner **Batch-Compiler**: eine Quelldatei pro
  Prozessaufruf, kein Daemon-/Server-Modus, kein stdin/stdout-Protokoll.
- Der Parser (`parser.c`) bricht beim ERSTEN Syntaxfehler komplett ab
  (`p->failed`-Kurzschlusslogik, siehe `IMPLEMENTATION_STATUS.md`-
  Historie) - keine Fehlerresilienz, keine Teilergebnisse für
  unvollständigen/syntaktisch fehlerhaften Code, wie ein Language
  Server sie beim Tippen ständig braucht.
- Keine inkrementelle Re-Analyse, keine persistente Symboltabelle über
  mehrere Aufrufe hinweg, kein Dokument-Versionierungskonzept.

Eine zweite, parallele Analyse-Implementierung nur für die IDE zu
bauen, würde exakt die vom Auftrag verbotene Doppelimplementierung
erzeugen (zwei Quellen der Wahrheit über NovaLang-Syntax/Semantik, die
garantiert auseinanderlaufen).

## Was tatsächlich vorhanden ist

- Syntax-Hervorhebung in Nova Studio (`highlighter.cpp`) ist ein
  eigener, einfacher Tokenizer - bewusst NICHT der Compiler-Lexer,
  rein für Farbgebung, ohne semantischen Anspruch.
- Echte Compiler-Diagnostik (Datei + Zeile) erscheint nach einem
  "Erstellen"-Lauf im Probleme-Panel (siehe `IMPLEMENTATION_STATUS.md`)
  - das ist Batch-Diagnostik nach einem vollständigen Compile-Lauf,
  kein Live-Feedback beim Tippen.

## Was dafür nötig wäre (nicht begonnen)

1. `novac.exe` um einen fehlerresistenten Parse-Modus erweitern
   (Panic-Mode-Recovery statt Abbruch beim ersten Fehler) - ein
   eigenständiges, riskantes Compiler-Refactoring (siehe
   `IMPLEMENTATION_STATUS.md`, bewusst nicht begonnen wegen
   Regressionsgefahr für den aktiv genutzten Compiler).
2. Eine wiederverwendbare Analyse-Bibliothek (Lexer/Parser/Typsystem
   als Funktionsaufrufe statt als CLI-Prozess) extrahieren, die sowohl
   `novac.exe` als auch ein neues `novals.exe` (Language Server)
   nutzen.
3. Erst danach: LSP-Protokollschicht (JSON-RPC über stdio,
   `initialize`/`textDocument/didChange`/`publishDiagnostics` usw.).

Jeder dieser drei Schritte ist für sich ein mehrtägiges bis
mehrwöchiges Vorhaben am Compiler-Frontend, nicht an der IDE.
