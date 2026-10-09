
# NPSPEC-LOGIC-LOOPS-0001 – NovaOS Logic Graph Loops

## Status

Angenommen

## Kategorie

NovaOS / Logic Graph / Schleifensteuerung

## Zweck

Definiert die kontrollierte Wiederholung von Ausführungsabläufen innerhalb eines NovaOS Logic Graphs.

Ziel ist die Unterstützung klassischer Schleifenkonstrukte mit klaren Abbruchbedingungen, Ressourcenlimits und nachvollziehbarem Ausführungsverhalten.

## Architektur

Das Loop-System besteht aus:

- **Loop Controller:** Verwaltung der Schleifenausführung.
- **Iteration Manager:** Steuerung einzelner Durchläufe.
- **Condition Evaluator:** Prüfung von Schleifenbedingungen.
- **Loop State:** Verwaltung von Zählern und Zwischenzuständen.
- **Cancellation Handler:** Kontrollierter Abbruch.

Die Ausführung erfolgt über die reguläre Graph Runtime.

## Schleifentypen

Unterstützt werden:

- **For:** Wiederholung über einen definierten Wertebereich.
- **For Each:** Iteration über Collections.
- **While:** Wiederholung bei erfüllter Bedingung.
- **Do While:** Bedingungsprüfung vor oder nach dem Durchlauf.
- **Repeat:** Wiederholung bis zu einer definierten Abbruchbedingung.

Die Semantik muss mit den entsprechenden NovaLang-Konstrukten übereinstimmen.

## Ausführungsmodell

Jede Schleife besitzt einen definierten Schleifenkörper und einen Ausführungskontext.

- Iterationen werden kontrolliert gestartet.
- Schleifenvariablen besitzen einen eindeutigen Gültigkeitsbereich.
- `Break` beendet die aktuelle Schleife.
- `Continue` startet den nächsten zulässigen Durchlauf.
- Verschachtelte Schleifen werden unterstützt.

Ausführungsabhängigkeiten innerhalb des Schleifenkörpers müssen eingehalten werden.

## Ressourcen und Abbruch

Schleifen unterliegen den Ressourcenlimits der Solution und ihres Ausführungskontexts.

Lang laufende Schleifen müssen Abbruchsignale berücksichtigen und den Scheduler nicht dauerhaft blockieren.

Unbegrenzte Schleifen sind nur zulässig, wenn sie kontrolliert unterbrechbar bleiben.

## Parallelität

Unabhängige Iterationen dürfen parallel ausgeführt werden, sofern ihre Daten- und Zustandsabhängigkeiten dies erlauben.

Parallele Iterationen müssen Structured Concurrency und definierte Synchronisationsregeln verwenden.

## Normative Anforderungen

1. Die Graph Runtime MUSS klassische Schleifenkonstrukte unterstützen.
2. Schleifenbedingungen MÜSSEN gemäß NovaLang-Semantik ausgewertet werden.
3. Schleifenvariablen MÜSSEN typisiert und eindeutig zugeordnet sein.
4. Verschachtelte Schleifen MÜSSEN unterstützt werden.
5. `Break` und `Continue` MÜSSEN definierte Auswirkungen besitzen.
6. Jede Schleife MUSS kontrolliert abbrechbar sein.
7. Ressourcenlimits und Zeitüberschreitungen MÜSSEN eingehalten werden.
8. Lang laufende Schleifen DÜRFEN den Scheduler nicht dauerhaft blockieren.
9. Parallele Iterationen MÜSSEN Datenabhängigkeiten und Synchronisationsregeln beachten.
10. Schleifenfehler MÜSSEN diagnostizierbar sein.
11. Schleifen DÜRFEN keine Capability-Berechtigungen erzeugen oder erweitern.
12. Das Loop-System MUSS unabhängig vom grafischen Editor und ohne KI funktionieren.

## Ergebnis

NovaOS erhält eine typsichere, ressourcenkontrollierte und abbrechbare Schleifensteuerung für wiederholte und parallele Abläufe innerhalb von Logic Graphs.
