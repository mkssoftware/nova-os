
# NPSPEC-LOGIC-CONTROLFLOW-0001 – NovaOS Logic Graph Control Flow

## Status

Angenommen

## Kategorie

NovaOS / Logic Graph / Ablaufsteuerung

## Zweck

Definiert die Steuerung von Ausführungsabläufen innerhalb eines NovaOS Logic Graphs.

Ziel ist die deterministisch kontrollierbare Ausführung von Bedingungen, Verzweigungen, Schleifen und parallelen Abläufen.

## Architektur

Das Controlflow-System besteht aus:

- **Controlflow Engine:** Steuerung des Ausführungsablaufs.
- **Branch Evaluator:** Auswertung von Bedingungen.
- **Loop Controller:** Verwaltung wiederholter Ausführungen.
- **Execution Coordinator:** Koordination abhängiger Abläufe.
- **Synchronization Manager:** Zusammenführung paralleler Ausführungen.
- **Cancellation Handler:** Kontrollierter Abbruch.

Die Controlflow Engine arbeitet mit der Dataflow Engine und dem Execution Scheduler zusammen.

## Steuerungsknoten

Unterstützt werden:

- **Sequence:** Sequenzielle Ausführung.
- **Branch:** Bedingte Verzweigung.
- **Switch:** Auswahl mehrerer Ausführungspfade.
- **Loop:** Kontrollierte Wiederholung.
- **Parallel:** Parallele Ausführung unabhängiger Zweige.
- **Join:** Synchronisation mehrerer Zweige.
- **Wait:** Warten auf Ereignisse oder Bedingungen.
- **Return:** Beenden eines Ausführungspfads.

Weitere Steuerungsknoten können über die Node Registry ergänzt werden.

## Ausführungsmodell

Steuerungsverbindungen definieren explizite Ausführungsabhängigkeiten.

Bedingungen werden anhand typisierter NovaLang-Ausdrücke ausgewertet.

Parallele Abläufe werden über Structured Concurrency verwaltet.

Schleifen müssen kontrollierbar und abbrechbar sein.

## Zustände und Synchronisation

Jeder aktive Ausführungspfad besitzt einen nachvollziehbaren Ausführungszustand.

Gemeinsam verwendete Daten müssen synchronisiert oder durch geeignete Isolation geschützt werden.

Abhängige Zweige dürfen erst fortgesetzt werden, wenn ihre definierten Voraussetzungen erfüllt sind.

## Fehler und Abbruch

Fehler können über definierte Fehlerpfade behandelt werden.

Nicht behandelte Fehler werden an den übergeordneten Ausführungskontext weitergeleitet.

Abbruchsignale müssen an untergeordnete Ausführungen propagiert werden.

Zeitüberschreitungen und Ressourcenlimits bleiben verbindlich.

## Normative Anforderungen

1. Die Controlflow Engine MUSS explizite Ausführungsabhängigkeiten unterstützen.
2. Bedingungen MÜSSEN gemäß NovaLang-Semantik ausgewertet werden.
3. Sequenzen, Verzweigungen und Schleifen MÜSSEN unterstützt werden.
4. Parallele Abläufe MÜSSEN über Structured Concurrency verwaltet werden.
5. Schleifen MÜSSEN kontrolliert abbrechbar sein.
6. Synchronisationspunkte MÜSSEN eindeutig definiert sein.
7. Gemeinsam verwendete Zustände MÜSSEN vor konkurrierenden Zugriffen geschützt werden.
8. Fehler MÜSSEN definiert behandelt oder weitergeleitet werden.
9. Abbruchsignale MÜSSEN untergeordnete Ausführungen erreichen.
10. Ressourcenlimits und Zeitüberschreitungen MÜSSEN eingehalten werden.
11. Ablaufsteuerung DARF keine Capability-Berechtigungen erzeugen oder erweitern.
12. Die Controlflow Engine MUSS unabhängig vom grafischen Editor und ohne KI funktionieren.

## Ergebnis

NovaOS erhält eine strukturierte, sichere und kontrollierbare Ablaufsteuerung für bedingte, wiederholte und parallele Ausführungen innerhalb von Logic Graphs.
