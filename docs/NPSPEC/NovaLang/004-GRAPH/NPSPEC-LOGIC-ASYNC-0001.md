
# NPSPEC-LOGIC-ASYNC-0001 – NovaOS Logic Graph Async Execution

## Status

Angenommen

## Kategorie

NovaOS / Logic Graph / Asynchrone Ausführung

## Zweck

Definiert die asynchrone Ausführung von Knoten und Teilgraphen innerhalb eines NovaOS Logic Graphs.

Ziel ist die effiziente Verarbeitung lang laufender Operationen, ohne andere Graph-Ausführungen oder die Benutzeroberfläche zu blockieren.

## Architektur

Das Async-System besteht aus:

- **Async Execution Manager:** Verwaltung asynchroner Operationen.
- **Task Manager:** Erstellung und Überwachung von Aufgaben.
- **Await Controller:** Kontrolliertes Warten auf Ergebnisse.
- **Continuation Scheduler:** Fortsetzung abgeschlossener Aufgaben.
- **Cancellation Manager:** Weitergabe von Abbruchsignalen.
- **Async Diagnostics:** Überwachung von Zuständen und Fehlern.

Die Ausführung verwendet den Graph Scheduler und die reguläre NovaLang Runtime.

## Asynchrones Modell

Unterstützt werden:

- Asynchrone Capability Nodes
- NovaLang `Async` und `Await`
- Asynchrone Subgraphs
- Parallele Aufgaben
- Ereignisgesteuerte Fortsetzungen
- Zeitüberschreitungen und Abbruch

Wartende Aufgaben dürfen keine CPU-Zeit durch aktives Warten verbrauchen.

## Structured Concurrency

Asynchrone Aufgaben gehören zu einem definierten übergeordneten Ausführungskontext.

Untergeordnete Aufgaben müssen vor dem Abschluss ihres Kontexts beendet, abgebrochen oder kontrolliert abgeschlossen werden.

Unkontrollierte Hintergrundaufgaben sind nicht zulässig.

## Daten und Zustände

Asynchrone Ergebnisse werden über typisierte Ausgangsports bereitgestellt.

Gemeinsam genutzte Zustände unterliegen den Synchronisationsregeln der Graph Runtime.

Fortsetzungen dürfen erst aktiviert werden, wenn ihre erforderlichen Ergebnisse verfügbar sind.

## Fehler und Abbruch

Fehler asynchroner Operationen werden über definierte Fehlerpfade oder den übergeordneten Ausführungskontext weitergeleitet.

Abbruchsignale müssen an untergeordnete Aufgaben propagiert werden.

Ressourcen müssen auch bei Fehlern, Zeitüberschreitungen und Abbruch freigegeben werden.

## Capability-Integration

Asynchrone Systemoperationen erfolgen ausschließlich über autorisierte Capability Nodes.

Custom Scripts dürfen keine zusätzlichen Capabilities anfordern.

Berechtigungen und Ressourcenlimits bleiben während der gesamten asynchronen Operation gültig und überprüfbar.

## Normative Anforderungen

1. Die Graph Runtime MUSS asynchrone Knotenausführung unterstützen.
2. NovaLang `Async` und `Await` MÜSSEN integriert sein.
3. Wartende Aufgaben DÜRFEN keine aktive CPU-Warteschleife benötigen.
4. Asynchrone Aufgaben MÜSSEN Structured Concurrency verwenden.
5. Jede Aufgabe MUSS einem kontrollierten Ausführungskontext zugeordnet sein.
6. Ergebnisse MÜSSEN typisiert über Ports bereitgestellt werden.
7. Fortsetzungen MÜSSEN Daten- und Steuerungsabhängigkeiten einhalten.
8. Abbruchsignale MÜSSEN an untergeordnete Aufgaben weitergegeben werden.
9. Zeitüberschreitungen und Ressourcenlimits MÜSSEN durchgesetzt werden.
10. Fehler und Abbrüche MÜSSEN diagnostizierbar sein.
11. Asynchrone Operationen DÜRFEN keine Capability-Grenzen umgehen.
12. Das Async-System MUSS unabhängig vom grafischen Editor und ohne KI funktionieren.

## Ergebnis

NovaOS erhält eine nicht blockierende, typsichere und kontrolliert abbrechbare asynchrone Ausführung für Logic Graphs mit vollständiger Integration in NovaLang und Structured Concurrency.
