
# NPSPEC-LOGIC-NODES-0001 – NovaOS Logic Graph Nodes

## Status

Angenommen

## Kategorie

NovaOS / Logic Graph / Knotenmodell

## Zweck

Definiert Aufbau, Identität und Verhalten der Knoten innerhalb eines NovaOS Logic Graphs.

Ziel ist ein einheitliches, typisiertes und erweiterbares Knotenmodell für Capabilities, NovaLang-Skripte und Datenverarbeitung.

## Architektur

Ein Knoten besteht aus:

- **Node ID:** Eindeutige Identität innerhalb des Graphen.
- **Node Type:** Versionierter Knotentyp.
- **Input Ports:** Typisierte Eingänge.
- **Output Ports:** Typisierte Ausgänge.
- **Configuration:** Deklarative Konfiguration.
- **Execution Contract:** Definiertes Ausführungsverhalten.
- **State:** Optionaler interner Zustand.

Die Node Registry verwaltet verfügbare Knotentypen und deren Verträge.

## Knotentypen

- **Capability Node:** Stellt eine autorisierte NovaOS-Fähigkeit bereit.
- **Script Node:** Führt benutzerdefinierte NovaLang-Logik aus.
- **Data Node:** Erzeugt, speichert oder transformiert Daten.
- **Control Node:** Steuert Bedingungen, Verzweigungen und Abläufe.
- **Event Node:** Empfängt oder erzeugt Ereignisse.
- **UI Node:** Verbindet Logik mit der Benutzeroberfläche.
- **Subgraph Node:** Kapselt einen wiederverwendbaren Teilgraphen.

Weitere Knotentypen können über die Node Registry registriert werden.

## Ausführungsverhalten

Jeder ausführbare Knoten besitzt einen definierten Ausführungsvertrag.

Dieser legt Eingaben, Ausgaben, Zustandsänderungen, Fehlerverhalten und gegebenenfalls asynchrone Ausführung fest.

Knoten dürfen nur über deklarierte Verbindungen und ausdrücklich autorisierte Ressourcen miteinander interagieren.

## Capability Nodes

Capability Nodes referenzieren eine vollständige NovaOS-Capability-ID.

Sie bilden die kontrollierte Grenze zwischen Graph und Systemdiensten.

Script Nodes dürfen Systemzugriffe ausschließlich über bereitgestellte Eingaben und autorisierte Ressourcenreferenzen nutzen.

## NovaLang-Integration

Script Nodes verwenden reguläres NovaLang innerhalb von `.nlf`.

Porttypen und Script-Signaturen müssen durch den NovaLang Type Checker überprüfbar sein.

Ein separater Skriptdialekt ist nicht vorgesehen.

## Normative Anforderungen

1. Jeder Knoten MUSS eine eindeutige Node-ID besitzen.
2. Jeder Knotentyp MUSS versioniert registriert sein.
3. Ein- und Ausgänge MÜSSEN typisiert sein.
4. Knoten MÜSSEN einen definierten Ausführungsvertrag besitzen.
5. Knotenparameter MÜSSEN vor der Ausführung validiert werden.
6. Capability Nodes MÜSSEN NovaOS-Berechtigungsprüfungen einhalten.
7. Script Nodes DÜRFEN keine Capabilities eigenständig anfordern.
8. Zustandsbehaftete Knoten MÜSSEN ihren Zustand kontrolliert verwalten.
9. Fehler MÜSSEN über definierte Diagnose- oder Fehlerausgänge behandelbar sein.
10. Erweiterte Knotentypen DÜRFEN Sicherheitsgrenzen nicht umgehen.
11. Knoten MÜSSEN unabhängig von ihrer grafischen Darstellung ausführbar sein.
12. Das Knotenmodell MUSS ohne KI vollständig funktionieren.

## Ergebnis

NovaOS erhält ein einheitliches, sicheres und erweiterbares Knotenmodell als Grundlage für die visuelle und deterministisch kontrollierte Ausführung von Logic Graphs.
