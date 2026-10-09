
# NPSPEC-LOGIC-DATAFLOW-0001 – NovaOS Logic Graph Dataflow

## Status

Angenommen

## Kategorie

NovaOS / Logic Graph / Datenfluss

## Zweck

Definiert die Verarbeitung und Weitergabe von Daten zwischen Knoten eines NovaOS Logic Graphs.

Ziel ist ein typsicherer, effizienter und nachvollziehbarer Datenfluss mit kontrollierter Ausführung und minimalen Kopieroperationen.

## Architektur

Das Dataflow-System besteht aus:

- **Dataflow Engine:** Steuerung des Datenflusses.
- **Dependency Resolver:** Ermittlung von Datenabhängigkeiten.
- **Value Transport:** Übertragung typisierter Werte.
- **Execution Scheduler:** Aktivierung ausführungsbereiter Knoten.
- **Buffer Manager:** Verwaltung zwischengespeicherter Daten.
- **Flow Monitor:** Überwachung und Diagnose.

Die Ausführung basiert auf den validierten Knoten, Ports und Verbindungen des Logic Graphs.

## Datenflussmodell

Ein Knoten verarbeitet Daten, sobald seine definierten Ausführungsbedingungen erfüllt sind.

- Eingänge empfangen typisierte Werte.
- Knoten verarbeiten ihre Eingaben.
- Ergebnisse werden über Ausgangsports bereitgestellt.
- Verbundene Knoten erhalten die entsprechenden Daten.

Die Ausführungsreihenfolge ergibt sich aus den Daten- und Steuerungsabhängigkeiten.

## Datenübertragung

Unterstützt werden:

- Wertübergabe
- Referenzübergabe
- Zero-Copy-Übertragung, sofern zulässig
- Asynchrone Datenströme
- Ereignisgebundene Datenübertragung

Veränderbare Daten müssen eindeutigen Zugriffs- und Synchronisationsregeln unterliegen.

## Ausführung

Unabhängige Knoten dürfen parallel ausgeführt werden.

Datenabhängigkeiten müssen vor der Ausführung erfüllt sein.

Rückkopplungen benötigen explizite Zustands- oder Verzögerungsmechanismen.

Unkontrollierte Endlosschleifen und unbegrenztes Datenwachstum müssen durch Ressourcenlimits begrenzbar sein.

## Capability-Integration

Capability Nodes stellen Daten oder autorisierte Ressourcenreferenzen über ihre Ausgangsports bereit.

Custom Scripts verarbeiten ausschließlich die bereitgestellten Eingaben.

Der Datenfluss darf keine zusätzlichen Systemberechtigungen erzeugen oder bestehende Zugriffsrechte erweitern.

## Fehlerbehandlung

Fehler müssen dem verursachenden Knoten und Datenfluss zugeordnet werden können.

Abhängige Ausführungen dürfen bei ungültigen Eingaben nicht unkontrolliert fortgesetzt werden.

Abbruch, Zeitüberschreitung und Ressourcenerschöpfung müssen definiert behandelt werden.

## Normative Anforderungen

1. Die Dataflow Engine MUSS typisierte Daten zwischen Knoten übertragen.
2. Datenabhängigkeiten MÜSSEN vor der Ausführung berücksichtigt werden.
3. Die Typregeln aus `NPSPEC-LOGIC-TYPES-0001` MÜSSEN eingehalten werden.
4. Unabhängige Knoten DÜRFEN parallel ausgeführt werden.
5. Veränderbare Daten MÜSSEN kontrollierte Zugriffsregeln besitzen.
6. Zero-Copy SOLL bei geeigneten Datentypen unterstützt werden.
7. Asynchrone Datenströme MÜSSEN kontrollierbare Puffer und Rückstau-Behandlung unterstützen.
8. Rückkopplungen MÜSSEN explizit definiert sein.
9. Ressourcenlimits und Abbruchsignale MÜSSEN berücksichtigt werden.
10. Datenübertragungen DÜRFEN keine Capability-Berechtigungen erweitern.
11. Datenflussfehler MÜSSEN diagnostizierbar sein.
12. Die Dataflow Engine MUSS unabhängig vom grafischen Editor und ohne KI funktionieren.

## Ergebnis

NovaOS erhält eine effiziente, typsichere und kontrollierte Dataflow Engine zur Verarbeitung von Daten zwischen Capabilities, NovaLang-Skripten und weiteren Logic-Graph-Knoten.
