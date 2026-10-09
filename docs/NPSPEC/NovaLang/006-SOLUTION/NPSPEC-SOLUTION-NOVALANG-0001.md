
# NPSPEC-SOLUTION-NOVALANG-0001 – NovaOS Solution NovaLang Integration

## Status

Angenommen

## Kategorie

NovaOS / Solutions / NovaLang

## Zweck

Definiert die Integration von NovaLang als Programmiersprache innerhalb von NovaOS-Solutions.

Ziel ist die Verbindung deklarativer Benutzeroberflächen, visueller Logic Graphs und typisierter Custom Scripts zu einer einheitlichen, sicheren Ausführungseinheit.

## Architektur

Die Integration besteht aus:

- **Solution Manifest:** Definition von Identität, Version und Komponenten in `solution.xml`.
- **NovaLang Compiler:** Übersetzung und Typprüfung der Scripts.
- **Logic Graph Bridge:** Einbindung von Custom Scripts als Graphknoten.
- **Capability Bridge:** Kontrollierte Bereitstellung autorisierter Daten und Ressourcen.
- **NovaLang Runtime:** Ausführung kompilierter Scripts.
- **UI Binding Bridge:** Verbindung zwischen NovaLang-Datenmodellen und deklarativer UI.

Die Solution verwendet die reguläre NovaLang-Sprachdefinition und Runtime.

## Sprachintegration

NovaLang ist die einheitliche Programmiersprache für programmierbare Solution-Komponenten.

- `.nova` enthält regulären NovaLang-Quellcode.
- `.nlf` enthält NovaLang-Code für Logic-Graph-Scripts.
- `.nui` beschreibt deklarative Benutzeroberflächen mit zur NovaLang-Deklarationssyntax konsistenter Semantik.

`.nlf` verwendet keinen eigenen Dialekt und keine abweichenden Sprachregeln.

Alle NovaLang-Quellen verwenden denselben Compiler, Type Checker und Language Service.

## Logic-Graph-Integration

Custom Scripts werden als typisierte Script Nodes in den Logic Graph eingebunden.

Ihre Ein- und Ausgänge werden durch definierte Ports beschrieben.

Der Graph bestimmt die Ausführungsreihenfolge und stellt erforderliche Daten bereit.

Ein Custom Script darf keine Systemfähigkeit eigenständig anfordern oder aufrufen.

## Capability-Zugriff

Systemzugriffe erfolgen ausschließlich über ausdrücklich eingebundene Capability Nodes.

Beispiel:

**Network Capability → Custom Script → Storage Capability**

Das Script verarbeitet die übergebenen Daten. Die Capability Nodes übernehmen die autorisierten Systemoperationen.

Die zentrale Berechtigungsverwaltung entscheidet über die tatsächliche Freigabe.

## UI-Integration

NovaLang-Daten und Graphzustände können über typisierte Bindungen mit `.nui`-Oberflächen verbunden werden.

UI-Ereignisse werden über definierte Ereignis- und Graphschnittstellen verarbeitet.

UI-Bindungen dürfen keine zusätzlichen Systemberechtigungen erzeugen.

## Build und Ausführung

NovaLang Studio prüft beim Build:

- Syntax und Typen
- Script- und Graphschnittstellen
- UI-Bindungen
- Abhängigkeiten und Versionen
- Deklarierte Capability-Anforderungen

Die Runtime führt ausschließlich validierte Artefakte innerhalb des zugewiesenen Solution-Kontexts aus.

## Identität und Sicherheit

Jede Solution besitzt eine dauerhafte GUID in `solution.xml`.

Die Berechtigungsverwaltung bindet Freigaben an die verifizierte Solution-Identität.

Änderungen an Scripts, Graphen oder Capability-Anforderungen müssen durch die Integritätsprüfung erfasst werden.

Sicherheitsrelevante Änderungen können eine erneute Autorisierung erforderlich machen.

## Normative Anforderungen

1. Solutions MÜSSEN die reguläre NovaLang-Sprachdefinition verwenden.
2. `.nova` und `.nlf` MÜSSEN dieselbe NovaLang-Semantik besitzen.
3. `.nui` MUSS mit der entsprechenden deklarativen NovaLang-Syntax konsistent sein.
4. Custom Scripts MÜSSEN über typisierte Graphschnittstellen eingebunden werden.
5. Custom Scripts DÜRFEN keine Capabilities eigenständig anfordern oder aufrufen.
6. Systemzugriffe MÜSSEN über autorisierte Capability Nodes erfolgen.
7. UI-Bindungen MÜSSEN typisiert und validierbar sein.
8. Scripts MÜSSEN die Isolation und Ressourcenlimits ihrer Solution einhalten.
9. Die Solution-Identität MUSS vor der Übernahme gespeicherter Berechtigungen verifiziert werden.
10. Sicherheitsrelevante Änderungen MÜSSEN eine erneute Berechtigungsprüfung auslösen.
11. Build und Ausführung MÜSSEN unabhängig von NovaLang Studio möglich sein.
12. Die gesamte Integration MUSS ohne KI funktionieren.

## Ergebnis

NovaOS-Solutions verwenden NovaLang als einheitliche Programmiersprache für Custom Scripts und programmierbare Komponenten, während Logic Graphs die Abläufe steuern und Capabilities sämtliche autorisierten Systemzugriffe übernehmen.
