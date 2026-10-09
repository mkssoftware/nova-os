
# NPSPEC-LOGIC-EXECUTION-0001 – NovaOS Logic Graph Execution

## Status

Angenommen

## Kategorie

NovaOS / Logic Graph / Ausführung

## Zweck

Definiert die kontrollierte Ausführung validierter Logic Graphs innerhalb von NovaOS-Solutions.

Ziel ist eine effiziente, typsichere und nachvollziehbare Verarbeitung von Knoten, Datenflüssen und Ereignissen.

## Architektur

Das Execution-System besteht aus:

- **Execution Manager:** Verwaltung der Graph-Ausführungen.
- **Execution Planner:** Erstellung ausführbarer Ablaufpläne.
- **Graph Runtime:** Ausführung der Knoten.
- **Execution Scheduler:** Koordination abhängiger und paralleler Aufgaben.
- **Execution Context:** Verwaltung von Zustand und Ressourcen.
- **Fault Handler:** Behandlung von Ausführungsfehlern.

Die Graph Runtime verwendet die reguläre NovaLang Runtime für Custom Scripts.

## Ausführungsmodell

Vor dem Start wird der Graph vollständig validiert.

Der Execution Planner ermittelt Daten- und Steuerungsabhängigkeiten.

Knoten werden ausgeführt, sobald ihre definierten Voraussetzungen erfüllt sind.

Unabhängige Knoten dürfen parallel ausgeführt werden.

## Ausführungszustände

Jede Graph-Ausführung besitzt einen definierten Zustand:

- **Created:** Ausführung angelegt.
- **Ready:** Validiert und startbereit.
- **Running:** Aktive Verarbeitung.
- **Waiting:** Warten auf Daten oder Ereignisse.
- **Suspended:** Kontrolliert angehalten.
- **Completed:** Erfolgreich abgeschlossen.
- **Failed:** Fehlerhaft beendet.
- **Cancelled:** Kontrolliert abgebrochen.

Zustandsübergänge müssen nachvollziehbar sein.

## Ausführungskontext

Jede Ausführung besitzt:

- Eindeutige Execution-ID
- Referenz auf die verifizierte Solution
- Lokale Zustände und Variablen
- Ressourcenbudget
- Abbruchkontext
- Diagnoseinformationen

Untergeordnete Ausführungen verwenden Structured Concurrency.

## Capability-Integration

Systemoperationen erfolgen ausschließlich über explizite Capability Nodes.

Custom Scripts erhalten nur die über ihre Ports bereitgestellten Daten und autorisierten Ressourcen.

Berechtigungen werden bei geschützten Operationen durch NovaOS geprüft.

## Fehler und Abbruch

Fehler werden über definierte Fehlerpfade behandelt oder an den übergeordneten Ausführungskontext weitergeleitet.

Abbruchsignale müssen untergeordnete Aufgaben erreichen.

Ressourcen und laufende Operationen müssen kontrolliert freigegeben werden.

## Normative Anforderungen

1. Nur vollständig validierte Graphen DÜRFEN produktiv ausgeführt werden.
2. Die Runtime MUSS Daten- und Steuerungsabhängigkeiten einhalten.
3. Unabhängige Knoten DÜRFEN parallel ausgeführt werden.
4. Jede Ausführung MUSS eine eindeutige Execution-ID besitzen.
5. Ausführungszustände MÜSSEN eindeutig definiert sein.
6. Custom Scripts MÜSSEN über die reguläre NovaLang Runtime ausgeführt werden.
7. Systemzugriffe MÜSSEN über autorisierte Capability Nodes erfolgen.
8. Ausführungen MÜSSEN Ressourcenlimits einhalten.
9. Untergeordnete Aufgaben MÜSSEN Structured Concurrency verwenden.
10. Ausführungen MÜSSEN kontrolliert abbrechbar sein.
11. Fehler und Zustandsübergänge MÜSSEN diagnostizierbar sein.
12. Die Graph Runtime MUSS unabhängig vom grafischen Editor und ohne KI funktionieren.

## Ergebnis

NovaOS erhält eine effiziente, sichere und kontrollierbare Ausführungsumgebung für Logic Graphs mit integrierter NovaLang Runtime, Capability-Sicherheit und nachvollziehbarer Ablaufsteuerung.
