# NPSPEC-STUDIO-GRAPH-TYPE-COMPATIBILITY-0001

**Status:** Angenommen

## Zweck
Verbindungen zwischen Knoten anhand gemeinsamer NovaLang-Typregeln validieren.

## Festlegungen
- Kompatibilität berücksichtigt Typ, Nullbarkeit, Generics, Richtung und mutierbare Referenzen.
- Nur nach NovaLang eindeutig verlustfreie Umwandlungen können automatisch eingefügt werden.
- Inkompatible Verbindungen werden vor Ausführung abgewiesen und verständlich markiert.
- Capability-Typen und privilegierte Handles bleiben strikt invariant und nicht frei konvertierbar.

## Ergebnis
Jede Graph-Verbindung folgt denselben Typregeln wie NovaLang.
