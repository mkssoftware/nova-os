# Nova Studio – IntelliSense

## Status: NICHT IMPLEMENTIERT

Keine Autovervollständigung, kein Hover/Quick-Info, kein Go-to-
Definition, kein Find-All-References, kein Rename-Symbol, keine
Code-Actions/Quick-Fixes, kein CodeLens, keine Inlay-Hints.

## Warum

IntelliSense in diesem Sinn setzt zwingend einen funktionierenden
Language Server (Symboltabellen, Typauflösung, projektweiter
Symbolindex) voraus - siehe `LANGUAGE_SERVER.md`. Ohne den ist
IntelliSense entweder (a) gar nicht vorhanden, oder (b) eine fingierte
Liste von Schlüsselwörtern ohne echten semantischen Bezug - genau das,
was der Auftrag ausdrücklich verbietet ("Keine simulierte
Autovervollständigung", "IntelliSense darf nicht lediglich auf einer
statischen Liste von Schlüsselwörtern beruhen").

Aus genau diesem Grund wurde in dieser Session **keine** Minimal-
Variante (z. B. reine Schlüsselwort-Liste) nachgerüstet - das wäre
bewusst gegen die eigene Vorgabe des Auftrags.

## Was tatsächlich vorhanden ist

- Syntax-Hervorhebung (lexikalisch, siehe `highlighter.cpp`) - keine
  Vervollständigung, nur Einfärbung.

## Voraussetzung für einen ersten echten Schritt

1. `LANGUAGE_SERVER.md` Punkt 1+2 (fehlerresistenter Parser,
   wiederverwendbare Analyse-Bibliothek).
2. Ein projektweiter Symbolindex (Funktionsnamen, Variablennamen,
   Typen) - im aktuellen Compiler nicht als wiederverwendbare
   Datenstruktur vorhanden (Symboltabellen leben nur lokal innerhalb
   eines einzelnen `novac`-Laufs, siehe `parser.c:NcParser`).
3. Erst danach ließe sich eine ERSTE, ehrliche Teilfunktion anbieten:
   z. B. Vervollständigung von Funktions-/Variablennamen, die der
   Compiler im aktuellen Modul tatsächlich kennt - keine erfundene
   Liste.
