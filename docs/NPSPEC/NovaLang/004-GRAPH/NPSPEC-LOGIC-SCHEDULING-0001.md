
# NPSPEC-LOGIC-SCHEDULING-0001 – NovaOS Logic Graph Scheduling

## Status

Angenommen

## Kategorie

NovaOS / Logic Graph / Scheduling

## Zweck

Definiert die Planung und Koordination ausführungsbereiter Knoten innerhalb eines NovaOS Logic Graphs.

Ziel ist eine effiziente, faire und ressourcenkontrollierte Ausführung unter Berücksichtigung von Abhängigkeiten, Prioritäten und Parallelität.

## Architektur

Das Scheduling-System besteht aus:

- **Graph Scheduler:** Koordination ausführungsbereiter Knoten.
- **Dependency Tracker:** Überwachung von Ausführungsabhängigkeiten.
- **Ready Queue:** Verwaltung ausführungsbereiter Aufgaben.
- **Priority Manager:** Berücksichtigung definierter Prioritäten.
- **Concurrency Controller:** Begrenzung paralleler Ausführungen.
- **Runtime Scheduler Bridge:** Anbindung an den NovaOS-Scheduler.

Der Graph Scheduler ergänzt den System-Scheduler, ersetzt ihn jedoch nicht.

## Scheduling-Modell

Knoten werden ausführungsbereit, sobald ihre Daten- und Steuerungsabhängigkeiten erfüllt sind.

Unterstützt werden:

- Sequenzielle Ausführung
- Parallele Ausführung unabhängiger Knoten
- Ereignisgesteuerte Aktivierung
- Asynchrone Ausführung
- Prioritätsbasierte Planung
- Kontrolliertes Warten und Fortsetzen

## Prioritäten

Ausführungen können deklarative Prioritäten besitzen.

Prioritäten dürfen die Sicherheits- und Ressourcenrichtlinien von NovaOS nicht umgehen.

Starvation muss durch geeignete Fairness-Mechanismen begrenzt werden.

## Parallelität

Unabhängige Knoten dürfen gleichzeitig ausgeführt werden.

Abhängige Knoten müssen ihre Voraussetzungen abwarten.

Untergeordnete Aufgaben werden über Structured Concurrency verwaltet.

Gemeinsame Zustände unterliegen den Synchronisationsregeln der Graph Runtime.

## Ressourcensteuerung

Der Scheduler berücksichtigt:

- CPU-Budget
- Maximale Parallelität
- Speichergrenzen
- Zeitlimits
- Warteschlangenlimits
- Abbruchsignale

Bei Ressourcenknappheit müssen Aufgaben kontrolliert verzögert, gedrosselt oder abgebrochen werden.

## Deterministische Ausführung

Ein optionaler deterministischer Modus verwendet eine reproduzierbare Scheduling-Reihenfolge bei gleichen Eingaben und gleichem Anfangszustand.

Externe Ereignisse und nicht deterministische Capability-Ergebnisse müssen dafür kontrolliert oder aufgezeichnet werden.

## Normative Anforderungen

1. Der Graph Scheduler MUSS Daten- und Steuerungsabhängigkeiten einhalten.
2. Ausführungsbereite Knoten MÜSSEN über kontrollierte Warteschlangen verwaltet werden.
3. Unabhängige Knoten DÜRFEN parallel ausgeführt werden.
4. Asynchrone und ereignisgesteuerte Ausführungen MÜSSEN unterstützt werden.
5. Prioritäten MÜSSEN innerhalb der NovaOS-Richtlinien berücksichtigt werden.
6. Starvation SOLL durch Fairness-Mechanismen verhindert werden.
7. Parallele Ausführungen MÜSSEN begrenzbar sein.
8. Untergeordnete Aufgaben MÜSSEN Structured Concurrency verwenden.
9. Ressourcenlimits und Abbruchsignale MÜSSEN eingehalten werden.
10. Ein deterministischer Scheduling-Modus MUSS unterstützt werden.
11. Scheduling-Entscheidungen MÜSSEN diagnostizierbar sein.
12. Der Graph Scheduler MUSS unabhängig vom grafischen Editor und ohne KI funktionieren.

## Ergebnis

NovaOS erhält einen effizienten, fairen und ressourcenkontrollierten Graph Scheduler, der abhängige, parallele und ereignisgesteuerte Ausführungen zuverlässig koordiniert.
