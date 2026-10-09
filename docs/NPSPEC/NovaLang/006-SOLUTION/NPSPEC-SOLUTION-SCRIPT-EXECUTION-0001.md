
# NPSPEC-SOLUTION-SCRIPT-EXECUTION-0001 – NovaOS Solution Script Execution

## Status

Angenommen

## Kategorie

NovaOS / Solution / Script Execution

## Zweck

Definiert die kontrollierte Ausführung von NovaLang-Scripts innerhalb einer NovaOS-Solution.

Scripts verarbeiten Daten und implementieren individuelle Logik. Systemzugriffe erfolgen ausschließlich über explizit verbundene Capability Nodes im Logic Graph.

## Architektur

Die Script-Ausführung besteht aus:

- **Script Execution Manager:** Verwaltung der Script-Ausführungen.
- **NovaLang Runtime Bridge:** Anbindung an die reguläre NovaLang Runtime.
- **Execution Context:** Bereitstellung von Eingaben, Zustand und Ressourcenlimits.
- **Port Binding:** Typisierte Übergabe von Ein- und Ausgangsdaten.
- **Script Isolation:** Isolation von Scripts und Ausführungskontexten.
- **Error Bridge:** Weiterleitung von Fehlern an die Logic Graph Runtime.

Die Ausführung wird ausschließlich durch die Solution Logic Runtime koordiniert.

## Script-Modell

- Custom Scripts verwenden die Dateiendung `.nlf`.
- `.nlf` verwendet exakt die reguläre NovaLang-Syntax.
- Scripts werden über Custom Script Nodes eingebunden.
- Ein- und Ausgaben erfolgen über deklarierte, typisierte Ports.
- Scripts können synchrone und asynchrone Logik enthalten.
- Scripts besitzen keine eigenständige Berechtigungsverwaltung.

## Ausführungsablauf

1. Die Logic Graph Runtime aktiviert den Script Node.
2. Eingabedaten und zulässige Ressourcen werden bereitgestellt.
3. Die NovaLang Runtime führt das Script isoliert aus.
4. Ergebnisse werden über die Ausgangsports zurückgegeben.
5. Nachfolgende Graphknoten werden entsprechend dem Controlflow aktiviert.

## Capability-Zugriff

Custom Scripts dürfen Systemfähigkeiten weder selbst anfordern noch direkt aufrufen.

Benötigte Capabilities werden ausdrücklich im Logic Graph platziert.

Beispiel:

**Network Capability → Custom Script → Storage Capability**

Das Script verarbeitet ausschließlich die vom Network Node bereitgestellten Daten. Die Speicherung erfolgt anschließend über den Storage Node.

Übergebene Ressourcenreferenzen dürfen keine zusätzlichen Berechtigungen vermitteln.

## Isolation und Ressourcen

Jede Script-Ausführung unterliegt:

- Solution- und Ausführungskontext
- Speicher- und CPU-Limits
- Zeitüberschreitungen
- Structured Concurrency
- Kontrolliertem Abbruch
- NovaLang-Typ- und Laufzeitprüfungen

Scripts dürfen keine anderen Solutions oder fremden Ausführungskontexte beeinflussen.

## Fehlerbehandlung

NovaLang-Exceptions werden über die Error Bridge an die Logic Graph Runtime weitergeleitet.

Fehler, Abbruch und Zeitüberschreitungen müssen kontrolliert behandelt werden.

Ressourcen werden beim Abschluss oder Abbruch freigegeben.

## Normative Anforderungen

1. Custom Scripts MÜSSEN über die reguläre NovaLang Runtime ausgeführt werden.
2. `.nlf` MUSS dieselbe Sprachsemantik wie NovaLang verwenden.
3. Scripts MÜSSEN über Custom Script Nodes eingebunden werden.
4. Ein- und Ausgaben MÜSSEN typisierte Ports verwenden.
5. Scripts DÜRFEN Capabilities weder selbst anfordern noch direkt aufrufen.
6. Systemzugriffe MÜSSEN über explizite Capability Nodes erfolgen.
7. Jede Ausführung MUSS einem isolierten Execution Context zugeordnet sein.
8. Ressourcenlimits und Abbruchsignale MÜSSEN durchgesetzt werden.
9. Asynchrone Scripts MÜSSEN die Structured-Concurrency-Regeln einhalten.
10. Fehler MÜSSEN an die Logic Graph Runtime weitergeleitet werden.
11. Script-Ausführungen DÜRFEN keine Berechtigungs- oder Isolationsgrenzen umgehen.
12. Die Script-Ausführung MUSS unabhängig von NovaLang Studio und ohne KI funktionieren.

## Ergebnis

NovaOS erhält eine sichere, typisierte und vollständig in die Logic Graph Runtime integrierte Script-Ausführung, bei der NovaLang die individuelle Logik verarbeitet und ausschließlich Capability Nodes autorisierte Systemoperationen übernehmen.
