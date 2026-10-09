
# NPSPEC-NOVALANG-DETERMINISM-0001 – NovaLang Deterministic Execution

## Status

Angenommen

## Kategorie

NovaLang / Runtime / Determinismus

## Zweck

Definiert die deterministische Ausführung von NovaLang-Programmen, Tasks und Solutions.

Ziel ist, dass identische Eingaben unter identischen definierten Ausführungsbedingungen reproduzierbare Ergebnisse liefern.

## Ausführungsmodi

| Modus | Beschreibung |
|---|---|
| Standard | Normale Ausführung mit parallelem Scheduling |
| Deterministic | Kontrollierte Reihenfolge und externe Einflüsse |
| Replay | Wiederholung anhand aufgezeichneter Ereignisse |

Der deterministische Modus ist unabhängig von AOT, JIT und Interpreter verfügbar.

## Determinismusmodell

Eine Ausführung gilt als deterministisch, wenn bei identischen Eingaben, Programmversionen und definierten Umgebungsbedingungen dieselben beobachtbaren Ergebnisse entstehen.

Kontrolliert werden insbesondere:

- Task-Reihenfolge und Synchronisation
- Zeitquellen und Timer
- Zufallszahlen und Initialisierungswerte
- Externe Eingaben und Ereignisse
- Reihenfolge nebenwirkungsbehafteter Operationen
- Definierte numerische Berechnungen

Nichtdeterministische Einflüsse müssen ausgeschlossen, kontrolliert oder aufgezeichnet werden.

## Scheduling

Der deterministische Scheduler verwendet eine reproduzierbare Reihenfolge ausführbarer Tasks.

- Task-Fortsetzungen erfolgen an definierten Scheduling-Punkten.
- Nebenläufige Zugriffe auf gemeinsame Daten müssen synchronisiert sein.
- Nicht deterministisch auflösbare Datenrennen sind unzulässig.
- Parallele Ausführung darf eingeschränkt werden.

## Zeit und Zufall

Zeit und Zufallswerte werden über kontrollierte Runtime-Schnittstellen bereitgestellt.

```vb
Public Function Berechnen(zufall As IRandomSource) As Integer
    Return zufall.NextInteger(1, 100)
End Function
```

Bei identischem Generatorzustand und identischer Aufrufreihenfolge muss dasselbe Ergebnis entstehen.

Echte Systemzeit und Hardware-Zufall benötigen explizite Freigabe als externe Eingaben.

## Externe Capabilities

Capability-Aufrufe können nichtdeterministische Ergebnisse liefern.

Im deterministischen Modus müssen sie:

- Reproduzierbare Ergebnisse garantieren,
- über aufgezeichnete Ereignisse wiedergegeben werden oder
- ausdrücklich als nichtdeterministisch zurückgewiesen werden.

Replay darf externe Seiteneffekte nicht unkontrolliert erneut ausführen.

## Numerische Semantik

- Ganzzahloperationen müssen ihre definierten Überlaufregeln einhalten.
- Gleitkommaoperationen benötigen festgelegte Präzisions- und Rundungsregeln.
- Plattformabhängige Optimierungen dürfen zugesicherte deterministische Ergebnisse nicht verändern.
- Nicht reproduzierbare numerische Operationen müssen entsprechend gekennzeichnet werden.

## Replay und Diagnostik

Ein Replay-Datensatz enthält mindestens:

- Programm- und Runtime-Version
- Relevante Konfiguration
- Initialzustand beziehungsweise dessen Referenz
- Externe Eingaben und Ereignisreihenfolge
- Erforderliche Scheduling-Entscheidungen

Replay-Daten unterliegen den NovaOS-Datenschutz- und Capability-Regeln.

## NovaOS-Integration

- NovaOS stellt kontrollierte Zeit-, Ereignis- und Scheduling-Schnittstellen bereit.
- Logic Graph und Solutions können deterministisch ausgeführt werden.
- Ressourcenlimits bleiben wirksam.
- Der deterministische Modus erteilt keine zusätzlichen Berechtigungen.
- Hardware- und Systemereignisse müssen über kontrollierte Schnittstellen eingebunden werden.

## Normative Anforderungen

1. NovaLang MUSS einen deterministischen Ausführungsmodus unterstützen.
2. Identische definierte Eingaben und Bedingungen MÜSSEN reproduzierbare Ergebnisse liefern.
3. Scheduling, Zeit und Zufall MÜSSEN kontrollierbar sein.
4. Nichtdeterministische Capability-Aufrufe MÜSSEN erkannt und kontrolliert behandelt werden.
5. Replay MUSS externe Ereignisse reproduzierbar wiedergeben können.
6. AOT, JIT und Interpreter MÜSSEN dieselben deterministischen Verträge einhalten.
7. Datenrennen DÜRFEN keine zugesicherte deterministische Ausführung verletzen.
8. Replay DARF keine unautorisierten externen Seiteneffekte auslösen.
9. Sicherheits- und Ressourcenlimits MÜSSEN unverändert gelten.

## Ergebnis

NovaLang erhält ein einheitliches Determinismusmodell mit kontrolliertem Scheduling, reproduzierbaren Eingaben und Replay-Unterstützung für Tests, Simulationen und Logic-Graph-Ausführungen.
