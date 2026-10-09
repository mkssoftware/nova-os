# NPSPEC-STUDIO-GRAPH-STEP-EXECUTION-0001

**Status:** Angenommen

## Zweck
Schrittweise Graph-Ausführung.

## Festlegungen
- Unterstützt Einzelschritt, Schritt über Untergraph/Funktionsaufruf und Schritt heraus; jeder Schritt orientiert sich an definierten Ausführungsereignissen statt Renderframes.
- Bei asynchronen Verzweigungen bleibt der gewählte Kontext eindeutig; Wartezustände und nicht deterministische Fortsetzungen werden kenntlich gemacht.
- Debug-Daten sind von der produktiven Ausführung getrennt; Capability-Grenzen und Isolation bleiben wirksam.

## Ergebnis
Komplexe Abläufe sind kontrolliert schrittweise prüfbar.
