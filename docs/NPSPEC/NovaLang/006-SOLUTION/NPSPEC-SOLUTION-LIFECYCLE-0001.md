
# NPSPEC-SOLUTION-LIFECYCLE-0001 – NovaOS Solution Lifecycle

## Status

Angenommen

## Kategorie

NovaOS / Solutions / Lebenszyklus

## Zweck

Definiert den vollständigen Lebenszyklus einer NovaOS-Solution von der Registrierung bis zur Beendigung und Entfernung.

Ziel ist eine kontrollierte, sichere und ressourcenschonende Verwaltung von Solutions einschließlich ihrer Logic Graphs, Benutzeroberflächen und Capabilities.

## Architektur

Das Lifecycle-System besteht aus:

- **Solution Manager:** Zentrale Verwaltung der Solutions.
- **Lifecycle Controller:** Steuerung der Zustandsübergänge.
- **Identity Validator:** Prüfung der Solution-Identität.
- **Integrity Validator:** Prüfung der Solution-Integrität.
- **Permission Resolver:** Ermittlung autorisierter Capabilities.
- **Runtime Coordinator:** Verwaltung von Logic Graph und UI Runtime.
- **Resource Controller:** Zuweisung und Freigabe von Ressourcen.

## Lebenszykluszustände

Eine Solution unterstützt folgende Zustände:

- **Registered:** Solution ist registriert.
- **Validated:** Identität, Integrität und Struktur sind geprüft.
- **Ready:** Erforderliche Voraussetzungen sind erfüllt.
- **Starting:** Runtime und Ressourcen werden initialisiert.
- **Running:** Solution wird ausgeführt.
- **Suspended:** Ausführung ist kontrolliert angehalten.
- **Stopping:** Ausführung wird beendet.
- **Stopped:** Solution ist vollständig beendet.
- **Failed:** Start oder Ausführung ist fehlgeschlagen.

Die Entfernung einer Solution erfolgt über einen gesonderten Verwaltungsprozess.

## Startvorgang

Beim Start werden folgende Schritte durchgeführt:

1. `solution.xml` laden und validieren.
2. Solution-GUID und Integrität prüfen.
3. Logic Graphs, Scripts und UI-Beschreibungen validieren.
4. Benötigte Capability-Berechtigungen ermitteln.
5. Fehlende Berechtigungen über die zentrale Berechtigungsverwaltung behandeln.
6. Isolierten Ausführungskontext erstellen.
7. Ressourcenbudgets zuweisen.
8. Logic Graph Runtime und UI Binding initialisieren.
9. Solution in den Zustand `Running` überführen.

Ein fehlgeschlagener Start muss bereits reservierte Ressourcen freigeben.

## Ausführung

Während `Running` verwaltet die Runtime:

- Logic-Graph-Ausführungen
- Ereignisse und asynchrone Aufgaben
- UI- und Datenbindungen
- Capability-Aufrufe
- Zustandsänderungen
- Ressourcenlimits und Fehler

Die Solution darf nur autorisierte Capabilities verwenden.

## Suspend und Resume

Eine Solution kann kontrolliert angehalten und fortgesetzt werden.

Beim Suspend werden ausführbare Aufgaben an sicheren Punkten pausiert und Ressourcen gemäß ihren Verträgen behandelt.

Beim Resume müssen Identität, Integrität, Berechtigungen und erforderliche Ressourcen weiterhin gültig sein.

Nicht wiederherstellbare Aufgaben werden kontrolliert beendet oder neu initialisiert.

## Beendigung

Beim Stoppen werden:

- Neue Ausführungen unterbunden.
- Laufende Aufgaben kontrolliert abgebrochen.
- Transaktionen abgeschlossen oder zurückgerollt.
- Persistente Zustände über autorisierte Capabilities gespeichert.
- Ressourcenreferenzen geschlossen.
- UI- und Runtime-Kontexte freigegeben.

Anschließend wechselt die Solution zu `Stopped`.

## Fehlerbehandlung

Nicht behebbare Fehler führen zu `Failed`.

Die Runtime muss betroffene Ausführungen isolieren und Ressourcen kontrolliert freigeben.

Ein Neustart darf erst nach erneuter Prüfung der erforderlichen Voraussetzungen erfolgen.

## Normative Anforderungen

1. Jede Solution MUSS einen definierten Lebenszyklus besitzen.
2. Zustandsübergänge MÜSSEN zentral kontrolliert werden.
3. Vor dem Start MÜSSEN Identität, Integrität und Graphstruktur geprüft werden.
4. Capability-Berechtigungen MÜSSEN vor geschützten Operationen autorisiert sein.
5. Jede laufende Solution MUSS einen isolierten Ausführungskontext besitzen.
6. Ressourcenlimits MÜSSEN während des gesamten Lebenszyklus gelten.
7. Suspend und Resume MÜSSEN kontrollierte Zustandsübergänge verwenden.
8. Beim Beenden MÜSSEN Aufgaben und Ressourcen ordnungsgemäß behandelt werden.
9. Fehlerhafte Solutions DÜRFEN andere Solutions nicht kompromittieren.
10. Sicherheitsrelevante Änderungen MÜSSEN vor erneuter Ausführung überprüft werden.
11. Lebenszyklusereignisse MÜSSEN diagnostizierbar sein.
12. Das Lifecycle-System MUSS unabhängig von NovaLang Studio und ohne KI funktionieren.

## Ergebnis

NovaOS erhält eine zentrale, sichere und deterministisch steuerbare Lebenszyklusverwaltung für Solutions mit kontrolliertem Start, Ausführung, Suspendierung, Wiederaufnahme und Beendigung.
