
# NPSPEC-LOGIC-SCHEMA-0001 – NovaOS Logic Graph Schema

## Status

Angenommen

## Kategorie

NovaOS / Logic Graph / Datenmodell

## Zweck

Definiert das einheitliche, versionierte Datenmodell für Logic Graphs innerhalb von NovaOS-Solutions.

Ziel ist die zuverlässige Speicherung, Validierung und Rekonstruktion von Knoten, Verbindungen und Ausführungsinformationen.

## Architektur

Das Logic-Graph-Schema umfasst:

- **Graph Metadata:** Identität, Schemaversion und Metadaten.
- **Node Definitions:** Knotenidentitäten, Typen und Konfiguration.
- **Port Definitions:** Typisierte Ein- und Ausgänge.
- **Connections:** Verbindungen zwischen Ports.
- **State Definitions:** Persistente und temporäre Zustände.
- **Execution Metadata:** Ausführungsregeln und Abhängigkeiten.
- **Editor Metadata:** Positionen und Darstellungsinformationen.

Ausführungsrelevante Daten und reine Editorinformationen bleiben logisch getrennt.

## Graphstruktur

Jeder Graph besitzt:

- Eindeutige Graph-ID
- Schemaversion
- Zugehörige Solution-Referenz
- Knotensammlung
- Verbindungssammlung
- Optionale Zustandsdefinitionen
- Optionale Teilgraph-Referenzen

Jeder Knoten besitzt eine innerhalb des Graphen eindeutige ID und eine versionierte Typreferenz.

## Ports und Verbindungen

Ports definieren:

- Eindeutige Port-ID
- Richtung: Eingang oder Ausgang
- Datentyp
- Verbindungsregeln
- Optionale Standardwerte

Verbindungen referenzieren Quell- und Zielports über stabile Identitäten.

Typinkompatible oder ungültige Verbindungen müssen bei der Validierung erkannt werden.

## Capability-Referenzen

Capability Nodes referenzieren Capabilities über ihre vollständige Capability-ID.

Die Referenz beschreibt eine Anforderung, stellt jedoch keine Berechtigung dar.

Autorisierungen werden ausschließlich durch NovaOS verwaltet.

## NovaLang-Integration

Custom Script Nodes referenzieren NovaLang-Code innerhalb der `.nlf`-Struktur.

Die enthaltene Skriptsyntax entspricht exakt NovaLang.

Porttypen müssen mit dem NovaLang-Typsystem kompatibel sein.

## Versionierung

Das Schema unterstützt explizite Versionsnummern und kontrollierte Migrationen.

Unbekannte oder inkompatible Schemaelemente dürfen nicht stillschweigend verworfen werden.

Änderungen an Knoten- und Portidentitäten müssen bei Migrationen nachvollziehbar bleiben.

## Normative Anforderungen

1. Jeder Logic Graph MUSS eine eindeutige ID und Schemaversion besitzen.
2. Knoten und Ports MÜSSEN stabile Identitäten verwenden.
3. Verbindungen MÜSSEN gültige Portreferenzen besitzen.
4. Porttypen MÜSSEN vor der Ausführung geprüft werden.
5. Ausführungsdaten und Editor-Metadaten MÜSSEN getrennt verwaltbar sein.
6. Capability-Referenzen DÜRFEN keine Berechtigungen implizieren.
7. Custom Scripts MÜSSEN die reguläre NovaLang-Semantik verwenden.
8. Graphen MÜSSEN vollständig rekonstruierbar sein.
9. Schemamigrationen MÜSSEN kontrolliert und nachvollziehbar erfolgen.
10. Unbekannte Daten DÜRFEN nicht unbemerkt verloren gehen.
11. Das Schema MUSS unabhängig von einer grafischen Oberfläche verarbeitbar sein.
12. Das Schema MUSS ohne KI interpretierbar und validierbar sein.

## Ergebnis

NovaOS erhält ein stabiles, versioniertes Logic-Graph-Schema als gemeinsame Grundlage für Speicherung, Editor, Validierung und Runtime.
