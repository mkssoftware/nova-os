
# NPSPEC-LOGIC-SUBGRAPHS-0001 – NovaOS Logic Graph Subgraphs

## Status

Angenommen

## Kategorie

NovaOS / Logic Graph / Teilgraphen

## Zweck

Definiert die Kapselung und Wiederverwendung zusammengehöriger Logic-Graph-Strukturen als Subgraphs.

Ziel ist die modulare Entwicklung komplexer Solutions ohne unnötige Duplizierung von Knoten und Verbindungen.

## Architektur

Das Subgraph-System besteht aus:

- **Subgraph Manager:** Verwaltung von Teilgraphen.
- **Subgraph Registry:** Registrierung und Identifikation.
- **Interface Manager:** Definition öffentlicher Ein- und Ausgänge.
- **Dependency Resolver:** Auflösung verschachtelter Teilgraphen.
- **Subgraph Validator:** Prüfung von Struktur und Schnittstellen.
- **Execution Bridge:** Integration in die Graph Runtime.

Ein Subgraph wird im übergeordneten Graphen als einzelner Knoten dargestellt.

## Subgraph-Modell

Jeder Subgraph besitzt:

- Eindeutige Subgraph-ID
- Versionierte Definition
- Typisierte Eingangsports
- Typisierte Ausgangsports
- Interne Knoten und Verbindungen
- Optionale Parameter und Zustandsdefinitionen

Interne Knoten bleiben gegenüber dem übergeordneten Graphen gekapselt.

## Wiederverwendung

Subgraphs können mehrfach innerhalb einer Solution verwendet werden.

Unterstützt werden:

- Lokale Teilgraphen
- Verschachtelte Teilgraphen
- Wiederverwendbare Graphmodule
- Parametrisierte Subgraphs

Mehrere Instanzen verwenden dieselbe Definition, besitzen jedoch standardmäßig getrennte Laufzeitzustände.

## Ausführung

Die Graph Runtime führt Subgraphs gemäß ihren definierten Ein- und Ausgängen aus.

Daten- und Steuerungsabhängigkeiten müssen über die Subgraph-Grenzen hinweg erhalten bleiben.

Rekursive Subgraph-Aufrufe sind nur mit definierten Abbruchbedingungen und Ressourcenlimits zulässig.

## Capability-Integration

Subgraphs dürfen Capability Nodes enthalten.

Benötigte Capabilities müssen bei der Validierung der gesamten Solution berücksichtigt werden.

Die Einbindung eines Subgraphs darf keine zusätzlichen Berechtigungen erzeugen oder bestehende Sicherheitsgrenzen umgehen.

## Versionierung

Änderungen an öffentlichen Schnittstellen müssen versioniert werden.

Abhängige Graphen müssen bei inkompatiblen Änderungen erneut validiert werden.

Interne Änderungen ohne Schnittstellenänderung sollen bestehende Verbindungen nicht beeinträchtigen.

## Normative Anforderungen

1. Logic Graphs MÜSSEN wiederverwendbare Subgraphs unterstützen.
2. Jeder Subgraph MUSS eine eindeutige ID besitzen.
3. Ein- und Ausgänge MÜSSEN typisiert sein.
4. Interne Knoten MÜSSEN gekapselt bleiben.
5. Mehrfachinstanzen MÜSSEN getrennte Laufzeitzustände unterstützen.
6. Verschachtelte Subgraphs MÜSSEN unterstützt werden.
7. Rekursion MUSS durch Ressourcenlimits kontrollierbar sein.
8. Subgraph-Abhängigkeiten MÜSSEN validiert werden.
9. Schnittstellenänderungen MÜSSEN versionierbar sein.
10. Capability-Anforderungen MÜSSEN bei der Solution-Validierung berücksichtigt werden.
11. Subgraphs MÜSSEN unabhängig von ihrer grafischen Darstellung ausführbar sein.
12. Das Subgraph-System MUSS ohne KI vollständig funktionieren.

## Ergebnis

NovaOS erhält ein modulares Subgraph-System zur gekapselten, typsicheren und wiederverwendbaren Strukturierung komplexer Logic Graphs.
