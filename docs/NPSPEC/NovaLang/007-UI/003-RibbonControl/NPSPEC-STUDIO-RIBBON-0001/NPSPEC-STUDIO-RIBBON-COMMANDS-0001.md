# NPSPEC-STUDIO-RIBBON-COMMANDS-0001

**Status:** Angenommen

## Zweck

Definiert das Befehlsmodell des Studios.

## Festlegungen

- Jeder Command besitzt stabile ID, Anzeigename, Beschreibung und Handler.
- Sichtbarkeit, Aktivierbarkeit, Ausgewählt-Zustand und Fortschritt sind getrennte Zustandsfelder.
- Befehle prüfen vor Ausführung Kontext und erforderliche Capabilities.
- Ribbon, Tastaturkürzel, Suche und Kontextmenüs rufen denselben Command auf.

## Ergebnis

Eine einheitliche, sichere Befehlsquelle ohne doppelte Ausführungslogik.
