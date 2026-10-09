
# NPSPEC-STUDIO-GRAPH-CONNECTIONS-0001 – NovaLang Studio Graph Connections

## Status

Angenommen

## Kategorie

NovaOS / NovaLang Studio / Logic Graph Editor / Connections

## Zweck

Definiert die visuelle Erstellung, Darstellung und Bearbeitung von Verbindungen zwischen Knoten eines Logic Graphs.

Ziel ist eine intuitive, typsichere und übersichtliche Verbindung von Daten-, Steuerungs-, Ereignis- und Ressourcenflüssen.

## Architektur

Das Connection-System besteht aus:

- **Connection Manager:** Verwaltung visueller Verbindungen.
- **Port Resolver:** Ermittlung kompatibler Anschlusspunkte.
- **Connection Renderer:** Darstellung von Verbindungslinien.
- **Connection Validator:** Prüfung der Verbindungsregeln.
- **Routing Engine:** Berechnung übersichtlicher Linienverläufe.
- **Connection Inspector:** Anzeige von Eigenschaften und Typinformationen.

Das System verwendet die bestehenden Logic-Graph-Port-, Edge- und Typechecking-Verträge.

## Verbindungstypen

Unterstützt werden:

- **Data Connections:** Übertragung typisierter Daten.
- **Control Connections:** Steuerung der Ausführungsreihenfolge.
- **Event Connections:** Weiterleitung von Ereignissen.
- **Resource Connections:** Übergabe autorisierter Ressourcenreferenzen.

Jeder Verbindungstyp erhält eine eindeutig unterscheidbare visuelle Darstellung.

## Verbindungserstellung

Verbindungen werden durch Ziehen von einem Ausgangsport zu einem kompatiblen Eingangsport erstellt.

Währenddessen zeigt der Editor:

- Mögliche Zielports
- Typkompatibilität
- Vorschau der Verbindung
- Ungültige Ziele
- Erforderliche Konvertierungen

Ungültige Verbindungen dürfen nicht als gültige Graphverbindungen übernommen werden.

## Bearbeitung

Der Editor unterstützt:

- Verbindungen auswählen und löschen
- Verbindungen auf andere Ports umlegen
- Mehrere Verbindungen gemeinsam bearbeiten
- Verbindungseigenschaften untersuchen
- Verbindungsverläufe automatisch anpassen
- Optional manuelle Routingpunkte setzen
- Verbindungen bei Knotenbewegungen aktualisieren

Änderungen müssen über Undo und Redo rückgängig gemacht werden können.

## Typprüfung

Der Connection Validator verwendet den zentralen Graph Type Checker.

Geprüft werden:

- Porttyp und Datenkompatibilität
- Ein- und Ausgangsrichtung
- Zulässige Verbindungskardinalität
- Steuerungs- und Ereignisregeln
- Zyklus- und Abhängigkeitsregeln
- Capability- und Ressourcenverträge

Typkonvertierungen dürfen nur über ausdrücklich definierte und zulässige Konvertierungen erfolgen.

## Darstellung und Routing

Verbindungen werden als klar erkennbare Linien dargestellt.

Die Routing Engine unterstützt:

- Direkte und gekrümmte Linien
- Automatische Anpassung an Knotenpositionen
- Hervorhebung ausgewählter Verbindungen
- Reduzierung unnötiger Linienüberschneidungen
- Optionale Verbindungspunkte
- Darstellung umfangreicher Graphen ohne unnötige Renderlast

Verbindungslinien dürfen keine Ausführungssemantik besitzen, die über die zugrunde liegenden Graph-Edges hinausgeht.

## Debugging

Während einer Debug-Sitzung können Verbindungen ihren Ausführungszustand anzeigen.

Unterstützt werden:

- Hervorhebung aktiver Datenflüsse
- Anzeige übertragener Werte
- Darstellung von Ereignissen
- Kennzeichnung fehlerhafter Übertragungen
- Nachverfolgung von Ausführungswegen

Vertrauliche Werte dürfen nur bei entsprechender Debug-Berechtigung angezeigt werden.

## Sicherheit

Resource Connections dürfen ausschließlich autorisierte Ressourcenreferenzen übertragen.

Das Umverbinden von Ports darf keine zusätzlichen Capability-Berechtigungen erzeugen.

Die Runtime muss sämtliche Verbindungsverträge unabhängig vom Editor erneut durchsetzen.

## Normative Anforderungen

1. Der Graph Editor MUSS typisierte Verbindungen zwischen Knoten unterstützen.
2. Daten-, Steuerungs-, Ereignis- und Ressourcenverbindungen MÜSSEN unterscheidbar sein.
3. Verbindungen MÜSSEN per Drag-and-drop erstellt werden können.
4. Kompatible Zielports MÜSSEN visuell erkennbar sein.
5. Jede Verbindung MUSS vor der Übernahme validiert werden.
6. Porttypen und Verbindungskardinalitäten MÜSSEN berücksichtigt werden.
7. Verbindungen MÜSSEN verschoben, geändert und gelöscht werden können.
8. Verbindungslinien MÜSSEN bei Knotenbewegungen automatisch aktualisiert werden.
9. Undo und Redo MÜSSEN unterstützt werden.
10. Debug-Informationen MÜSSEN als separate Overlays darstellbar sein.
11. Visuelle Routinginformationen DÜRFEN die Ausführungssemantik nicht verändern.
12. Resource Connections DÜRFEN keine Capability-Grenzen umgehen.
13. Die Runtime MUSS Verbindungsregeln unabhängig vom Editor prüfen.
14. Alle grundlegenden Connection-Funktionen MÜSSEN ohne KI funktionieren.

## Ergebnis

NovaLang Studio erhält ein typsicheres, übersichtliches und interaktives Verbindungssystem zur visuellen Modellierung von Datenflüssen, Steuerungsabläufen und Capability-Verknüpfungen innerhalb von Logic Graphs.
