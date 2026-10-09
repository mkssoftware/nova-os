
# NPSPEC-SOLUTION-LOGIC-GRAPH-0001 – NovaOS Solution Logic Graph

## Status

Angenommen

## Kategorie

NovaOS / Solutions / Logic Graph

## Zweck

Definiert die Integration des Logic Graphs als zentrale Ausführungslogik einer NovaOS-Solution.

Ziel ist, Capabilities, Custom Scripts, Datenflüsse und Benutzeroberflächen zu einer ausführbaren Solution zu verbinden, ohne ein klassisches monolithisches Programm erstellen zu müssen.

## Architektur

Die Integration besteht aus:

- **Solution Manager:** Verwaltung der Solution und ihrer Identität.
- **Logic Graph Runtime:** Ausführung der definierten Graphlogik.
- **Capability Bridge:** Verbindung zu autorisierten NovaOS-Fähigkeiten.
- **NovaLang Runtime:** Ausführung eingebundener Custom Scripts.
- **UI Binding Bridge:** Verbindung zwischen Graph und deklarativer Benutzeroberfläche.
- **State Manager:** Verwaltung des Solution-Zustands.

Der Logic Graph verwendet die bestehenden NovaOS-Ausführungs-, Sicherheits- und Ressourcenmechanismen.

## Solution-Struktur

Eine Solution kann enthalten:

- `solution.xml` mit GUID, Version und Metadaten
- Einen oder mehrere Logic Graphs
- Custom Scripts als `.nlf`
- Deklarative Benutzeroberflächen als `.nui`
- Ressourcen und Konfigurationen
- Deklarierte Capability-Anforderungen

Die `solution.xml` bildet den zentralen Einstiegspunkt für Identität und Konfiguration.

## Ausführungsmodell

Der Logic Graph verbindet Capabilities, Scripts und UI-Ereignisse über typisierte Ports.

Unterstützt werden:

- Ereignisgesteuerte Ausführung
- Daten- und Steuerungsflüsse
- Asynchrone Aufgaben
- Subgraphs und Funktionen
- Zustandsverwaltung
- Fehlerbehandlung und Transaktionen

Die Graph Runtime steuert die Ausführung unabhängig von NovaLang Studio.

## Capability-Integration

Systemzugriffe erfolgen ausschließlich über autorisierte Capability Nodes.

Custom Scripts dürfen keine Capabilities selbstständig anfordern oder direkt auf geschützte Systemressourcen zugreifen.

Benötigte Daten und Ressourcen werden über die Verbindungen des Logic Graphs bereitgestellt.

## Benutzeroberfläche

Deklarative `.nui`-Oberflächen können mit Graph-Ereignissen, Zuständen und Daten verbunden werden.

Die UI enthält keine eigenständige privilegierte Ausführungslogik.

Eine Solution darf auch ohne grafische Benutzeroberfläche ausgeführt werden.

## Identität und Berechtigungen

Jede Solution besitzt eine dauerhaft eindeutige GUID in `solution.xml`.

Berechtigungen werden an die verifizierte Solution-Identität gebunden.

Die GUID allein gilt nicht als Vertrauensnachweis.

Sicherheitsrelevante Änderungen am Graphen oder an Capability-Anforderungen müssen eine erneute Integritäts- und gegebenenfalls Berechtigungsprüfung auslösen.

## Validierung und Lebenszyklus

Vor der Ausführung werden Graphstruktur, Typen, Abhängigkeiten und Capability-Verträge geprüft.

Der Solution Manager steuert Start, Ausführung, Abbruch und Beendigung.

Fehler und Ressourcenüberschreitungen müssen kontrolliert behandelt werden.

## Normative Anforderungen

1. Eine Solution MUSS mindestens einen definierten Einstiegspunkt für ihre Ausführungslogik besitzen.
2. Logic Graphs MÜSSEN als Bestandteil einer Solution registrierbar sein.
3. Die Graph Runtime MUSS unabhängig von NovaLang Studio funktionieren.
4. Capabilities MÜSSEN über autorisierte Capability Nodes eingebunden werden.
5. Custom Scripts DÜRFEN keine Systemfähigkeiten eigenständig anfordern.
6. `.nlf` MUSS die reguläre NovaLang-Semantik verwenden.
7. `.nui`-Oberflächen MÜSSEN mit Graph-Ereignissen und Zuständen verbindbar sein.
8. Graphen MÜSSEN vor ihrer Ausführung validiert werden.
9. Die Solution-GUID MUSS dauerhaft eindeutig sein.
10. Berechtigungen MÜSSEN an eine verifizierte Solution-Identität gebunden werden.
11. Sicherheitsrelevante Änderungen MÜSSEN erneut überprüft werden.
12. Ausführungen MÜSSEN isoliert und ressourcenbegrenzt sein.
13. Die Solution-Ausführung MUSS ohne KI möglich sein.

## Ergebnis

NovaOS-Solutions erhalten mit dem Logic Graph eine modulare, typsichere und kontrollierte Ausführungslogik, die Capabilities, NovaLang-Scripts und deklarative Benutzeroberflächen zu einer eigenständigen Anwendungseinheit verbindet.
