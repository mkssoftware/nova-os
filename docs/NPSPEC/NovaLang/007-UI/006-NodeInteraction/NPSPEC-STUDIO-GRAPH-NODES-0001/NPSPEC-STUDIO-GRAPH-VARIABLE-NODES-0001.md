# NPSPEC-STUDIO-GRAPH-VARIABLE-NODES-0001

**Status:** Angenommen

## Zweck
Variablen innerhalb definierter Gültigkeitsbereiche lesen und schreiben.

## Festlegungen
- Get und Set sind getrennte Knoten mit typisierten Pins.
- Scope, Mutierbarkeit und Initialisierung ergeben sich aus NovaLang-Definitionen.
- Schreibzugriffe sind kontrollflussgebunden; konkurrierende Zugriffe folgen expliziten Synchronisationsregeln.
- Variablen repräsentieren keine versteckten globalen Systemrechte.

## Ergebnis
Zustandszugriffe bleiben transparent und typisiert.
