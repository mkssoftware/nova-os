
# NPSPEC-LOGIC-GRAPH-0001 – NovaOS Logic Graph

## Status

Angenommen

## Kategorie

NovaOS / Solutions / Logic Graph

## Zweck

Definiert den Logic Graph als visuelles, ausführbares Logikmodell für NovaOS-Solutions.

Ziel ist die Verbindung von Systemfähigkeiten, Datenverarbeitung und benutzerdefinierter Logik ohne die Entwicklung klassischer monolithischer Programme.

## Architektur

Der Logic Graph besteht aus:

- **Graph Model:** Deklarative Beschreibung von Knoten und Verbindungen.
- **Node Registry:** Registrierung verfügbarer Knotentypen.
- **Type System:** Prüfung von Ein- und Ausgängen.
- **Graph Validator:** Validierung der Graphstruktur.
- **Execution Planner:** Erstellung eines ausführbaren Ablaufplans.
- **Graph Runtime:** Kontrollierte Ausführung.
- **State Manager:** Verwaltung von Zuständen und Ereignissen.

Die grafische Darstellung ist von der Ausführungslogik getrennt.

## Knotenmodell

Ein Graph unterstützt folgende Knotenkategorien:

- **Capability Nodes:** Zugriff auf autorisierte NovaOS-Fähigkeiten.
- **Script Nodes:** Benutzerdefinierte Logik in NovaLang.
- **Data Nodes:** Bereitstellung und Verarbeitung von Daten.
- **Control Nodes:** Bedingungen, Verzweigungen und Ablaufsteuerung.
- **Event Nodes:** Verarbeitung von Ereignissen.
- **UI Nodes:** Verbindung zur deklarativen Benutzeroberfläche.
- **Subgraph Nodes:** Wiederverwendbare Teilgraphen.

Jeder Knoten besitzt eine eindeutige ID, einen definierten Typ sowie typisierte Ein- und Ausgänge.

## Datenfluss

Verbindungen übertragen typisierte Werte, Ereignisse oder kontrollierte Ressourcenreferenzen.

Die Ausführung erfolgt anhand expliziter Daten- und Steuerungsabhängigkeiten.

Zyklische Verbindungen sind nur zulässig, wenn ihre Ausführungssemantik eindeutig definiert ist.

## Capability-Modell

Systemzugriffe erfolgen ausschließlich über explizite Capability Nodes.

Custom Scripts dürfen keine zusätzlichen Systemfähigkeiten selbstständig anfordern.

Ein Script Node verarbeitet ausschließlich die über seine Eingänge bereitgestellten Daten und autorisierten Ressourcen.

Die Berechtigungsprüfung erfolgt durch NovaOS anhand der verifizierten Solution-Identität.

## NovaLang-Integration

Custom Scripts verwenden `.nlf` und exakt dieselbe NovaLang-Sprache wie `.nova`.

Es existiert kein separater Logic-Graph-Dialekt.

Die Ausführung erfolgt über die reguläre NovaLang Runtime.

## Persistenz

Logic Graphs werden als versionierte, deklarative `.nlf`-Artefakte gespeichert, deren enthaltene Scripts der regulären NovaLang-Semantik entsprechen.

Knotenidentitäten, Verbindungen, Parameter und Zustandsdefinitionen müssen rekonstruierbar sein.

## Normative Anforderungen

1. Der Logic Graph MUSS Bestandteil des NovaOS-Solution-Modells sein.
2. Knoten und Verbindungen MÜSSEN eindeutig identifizierbar sein.
3. Ein- und Ausgänge MÜSSEN typisiert sein.
4. Graphen MÜSSEN vor der Ausführung validiert werden.
5. Systemzugriffe MÜSSEN über explizite Capability Nodes erfolgen.
6. Custom Scripts DÜRFEN keine Capabilities eigenständig anfordern.
7. `.nlf` MUSS reguläre NovaLang-Semantik verwenden.
8. Teilgraphen MÜSSEN wiederverwendbar sein.
9. Fehler und Ausführungszustände MÜSSEN diagnostizierbar sein.
10. Graph-Ausführung MUSS kontrolliert abbrechbar sein.
11. Die Runtime MUSS Ressourcenlimits und Sicherheitsgrenzen einhalten.
12. Logic Graphs MÜSSEN ohne KI vollständig ausführbar sein.

## Ergebnis

NovaOS erhält ein typisiertes, sicheres und modulares Logic-Graph-System, mit dem Solutions aus Capabilities, NovaLang-Logik und Benutzeroberflächen zusammengesetzt werden können.
