
# NPSPEC-STUDIO-TESTING-0001 – NovaLang Studio Testing

## Status

Angenommen

## Kategorie

NovaLang Studio / Testing / Qualitätssicherung

## Zweck

Definiert das integrierte Testsystem von NovaLang Studio zur automatisierten Prüfung von NovaLang-Programmen, Bibliotheken, Solutions, Logic Graphs und deklarativen Benutzeroberflächen.

Ziel ist eine schnelle, reproduzierbare und sichere Testumgebung, die Fehler frühzeitig erkennt und Entwickler bei der Qualitätssicherung unterstützt.

Das Testsystem muss unabhängig von der grafischen Entwicklungsumgebung funktionieren und vollständig ohne KI nutzbar sein.

## Architektur

| Komponente | Aufgabe |
|---|---|
| Test Manager | Zentrale Verwaltung aller Testvorgänge |
| Test Discovery | Automatische Erkennung von Tests |
| Test Runner | Kontrollierte Ausführung |
| Test Framework | Bereitstellung von Assertions und Testfunktionen |
| Test Isolation | Isolation einzelner Tests |
| Test Fixture Manager | Verwaltung von Testvorbedingungen und Ressourcen |
| Test Result Collector | Sammlung und Auswertung der Ergebnisse |
| Test Explorer | Darstellung und Navigation |
| Coverage Collector | Ermittlung der Testabdeckung |
| Test Diagnostics | Integration in das Diagnosesystem |

Der Test Manager verwendet den Build Manager und Execution Manager von NovaLang Studio.

## Testarten

NovaLang Studio unterstützt:

- Unit Tests
- Integration Tests
- Functional Tests
- Regression Tests
- Logic-Graph-Tests
- Capability-Contract-Tests
- UI-Tests
- Async- und Concurrency-Tests
- Performance Tests

Die Testarten verwenden ein gemeinsames Test- und Ergebnismodell.

## Testmodell

Jeder Test besitzt mindestens:

- Eindeutige Testidentität
- Testname
- Zugehöriges Projekt oder Solution
- Testtyp
- Ausführungsziel
- Erwartetes Ergebnis
- Ausführungsstatus

Optional werden unterstützt:

- Testkategorie
- Tags
- Timeout
- Ressourcenlimits
- Testparameter
- Abhängigkeiten
- Fixture-Konfiguration

Testidentitäten müssen über wiederholte Testläufe möglichst stabil bleiben.

## Testdefinition

NovaLang stellt ein natives Test Framework bereit.

Beispiel:

```vb
Imports Nova.Testing

<TestClass>
Public Class MathematikTests

    <Test>
    Public Sub AdditionTest()
        Dim ergebnis As Integer = 2 + 3

        Assert.Equal(5, ergebnis)
    End Sub

    <Test>
    Public Sub DivisionTest()
        Assert.Throws(Of DivideByZeroException)(
            Sub()
                Dim ergebnis = 10 / 0
            End Sub
        )
    End Sub

End Class
```

Die gezeigte Test-API definiert die vorgesehene Schnittstelle des NovaLang-Testframeworks.

Testattribute verwenden das gemeinsame NovaLang-Metadatenmodell.

## Test Discovery

Test Discovery erkennt Tests anhand definierter Metadaten und Testverträge.

Unterstützt werden:

- Automatische Testerkennung
- Inkrementelle Aktualisierung
- Projektübergreifende Tests
- Filterung nach Kategorien
- Erkennung parametrisierter Tests
- Zuordnung zu Quellcodepositionen

Die Erkennung darf keine Testfunktionen unbeabsichtigt ausführen.

## Test Runner

Der Test Runner unterstützt:

- Alle Tests ausführen
- Tests eines Projekts ausführen
- Tests einer Solution ausführen
- Einzelnen Test ausführen
- Ausgewählte Tests ausführen
- Fehlgeschlagene Tests erneut ausführen
- Tests mit Debugger starten
- Testausführung abbrechen

Jeder Testlauf besitzt eine eindeutige Sitzungsidentität.

## Testzustände

Ein Test verwendet folgende Zustände:

| Zustand | Bedeutung |
|---|---|
| Discovered | Test wurde erkannt |
| Queued | Test wartet auf Ausführung |
| Running | Test wird ausgeführt |
| Passed | Test erfolgreich |
| Failed | Erwartetes Ergebnis nicht erreicht |
| Skipped | Test bewusst übersprungen |
| Cancelled | Test abgebrochen |
| Error | Test konnte nicht ordnungsgemäß ausgeführt werden |
| Timeout | Zulässige Ausführungszeit überschritten |

Ein fehlgeschlagener Test muss von einem Fehler der Testinfrastruktur unterscheidbar sein.

## Assertions

Das Test Framework stellt mindestens bereit:

- `Assert.True`
- `Assert.False`
- `Assert.Equal`
- `Assert.NotEqual`
- `Assert.Null`
- `Assert.NotNull`
- `Assert.Throws`
- `Assert.Contains`
- `Assert.Fail`

Assertions erzeugen strukturierte Testergebnisse.

Bei Vergleichen sollen erwarteter und tatsächlicher Wert getrennt verfügbar sein.

## Test Fixtures

Fixtures ermöglichen die kontrollierte Vorbereitung und Bereinigung von Testumgebungen.

Unterstützt werden:

- Setup vor einem Test
- Cleanup nach einem Test
- Setup vor einer Testgruppe
- Cleanup nach einer Testgruppe
- Bereitstellung definierter Testdaten
- Verwaltung temporärer Ressourcen

Cleanup muss auch nach fehlgeschlagenen Tests soweit möglich ausgeführt werden.

Fehler bei Setup und Cleanup müssen gesondert diagnostiziert werden.

## Testisolation

Tests werden in definierten Ausführungskontexten gestartet.

Die Isolation berücksichtigt:

- Speicher
- Tasks und Threads
- Dateizugriffe
- Netzwerkzugriffe
- Capability-Berechtigungen
- Temporäre Ressourcen
- Gemeinsame Testdaten

Tests dürfen sich nicht unbeabsichtigt gegenseitig beeinflussen.

Für Tests mit gemeinsamem Zustand müssen explizite Ausführungsregeln festgelegt werden.

## Parallele Testausführung

Unabhängige Tests können parallel ausgeführt werden.

Dabei gilt:

- Ressourcenlimits müssen eingehalten werden.
- Testabhängigkeiten müssen berücksichtigt werden.
- Gemeinsame Ressourcen benötigen kontrollierte Synchronisation.
- Ergebnisse müssen eindeutig zugeordnet werden.
- Fehler eines Tests dürfen unabhängige Tests nicht unkontrolliert beeinflussen.

Die Parallelität muss konfigurierbar sein.

## Parametrisierte Tests

Das Test Framework unterstützt die mehrfache Ausführung eines Tests mit unterschiedlichen Eingabewerten.

Beispiel:

```vb
<TestCase(2, 3, 5)>
<TestCase(10, 20, 30)>
<TestCase(-5, 5, 0)>
Public Sub AdditionTest(a As Integer, b As Integer, erwartet As Integer)
    Assert.Equal(erwartet, a + b)
End Sub
```

Jeder Parametersatz muss im Testergebnis eindeutig identifizierbar sein.

## Async-Tests

Asynchrone Tests unterstützen:

- `Async` und `Await`
- Task-Abschluss
- Cancellation
- Timeouts
- Fehlerweiterleitung
- Structured Concurrency

Ein Test gilt erst als abgeschlossen, wenn alle ihm zugeordneten erforderlichen Tasks beendet oder kontrolliert abgebrochen wurden.

Nicht abgeschlossene Tasks müssen diagnostiziert werden.

## Logic-Graph-Testing

Logic Graphs können unabhängig von der vollständigen Solution getestet werden.

Unterstützt werden:

- Einzelne Graph-Knoten
- Zusammenhängende Graph-Abschnitte
- Vollständige Graph-Ausführungen
- Eingangs- und Ausgangswerte
- Fehlerpfade
- Parallele Ausführungspfade
- Capability-Verbindungen

Die Testumgebung muss definierte Eingabedaten bereitstellen können.

## Capability-Mocking

Externe Capabilities können durch kontrollierte Testimplementierungen ersetzt werden.

Beispiele:

- Simulierte Netzwerkantworten
- Virtuelle Dateisystemzugriffe
- Definierte Geräteereignisse
- Fehlerhafte Capability-Antworten
- Simulierte Zeitüberschreitungen

Mocks müssen denselben typisierten Capability-Vertrag erfüllen.

Ein Mock stellt keine echte NovaOS-Berechtigung dar und darf nicht als produktive Capability ausgegeben werden.

## Solution-Testing

Beim Testen einer Solution werden geprüft:

- Logic-Graph-Struktur
- Custom-NovaLang-Skripte
- Capability-Verträge
- Datenflüsse
- UI-Bindungen
- Ereignisverarbeitung
- Fehlerbehandlung

Die Solution-Identität und ihre Sicherheitsgrenzen bleiben erhalten.

Testberechtigungen müssen ausdrücklich vom produktiven Berechtigungskontext getrennt sein.

## UI-Testing

Deklarative `.nui`-Oberflächen können automatisiert getestet werden.

Unterstützt werden:

- Komponenteninitialisierung
- Eigenschaften
- Datenbindungen
- Ereignisse
- Zustandsänderungen
- Benutzerinteraktionen
- Fehlerzustände

Tests verwenden stabile Komponentenidentitäten statt ausschließlich visueller Bildschirmkoordinaten.

Die Ausführung erfolgt nach Möglichkeit in einer isolierten UI-Testumgebung.

## Deterministische Tests

NovaLang Studio unterstützt deterministische Testausführungen, soweit die verwendeten Komponenten dies ermöglichen.

Kontrollierbar sind insbesondere:

- Zufallswerte
- Zeitquellen
- Eingabedaten
- Ereignisreihenfolgen
- Simulierte Capability-Antworten
- Unterstützte Scheduling-Entscheidungen

Nicht deterministische Einflüsse müssen erkennbar sein.

Ein Test darf nicht als vollständig deterministisch gekennzeichnet werden, wenn relevante externe Einflüsse unkontrolliert bleiben.

## Test Coverage

Der Coverage Collector kann erfassen:

- Line Coverage
- Branch Coverage
- Function Coverage
- Logic-Graph-Node-Coverage
- Logic-Graph-Edge-Coverage

Coverage-Daten werden dem jeweiligen Build und Testlauf zugeordnet.

Nicht instrumentierbarer Code muss von nicht ausgeführtem Code unterscheidbar sein.

Eine hohe Testabdeckung darf nicht automatisch als Fehlerfreiheit bewertet werden.

## Performance Testing

Performance Tests können erfassen:

- Ausführungsdauer
- Speicherverbrauch
- CPU-Zeit
- Allokationen
- Task-Anzahl
- Definierte Ressourcenlimits

Vergleichsmessungen müssen ihre Testumgebung und relevanten Rahmenbedingungen dokumentieren.

Performance-Grenzwerte können als Testbedingungen definiert werden.

## Test Explorer

Der Test Explorer verwendet die NovaOS-Designsprache.

Unterstützt werden:

- Hierarchische Testübersicht
- Gruppierung nach Projekt und Solution
- Anzeige des Teststatus
- Suche und Filterung
- Direkter Teststart
- Debugging einzelner Tests
- Navigation zum Quellcode
- Anzeige von Fehlermeldungen
- Testdauer und Verlauf

Häufig benötigte Funktionen müssen unmittelbar erreichbar sein.

## Testergebnisse

Jeder Testlauf erzeugt ein strukturiertes Ergebnis.

Dieses enthält mindestens:

- Testlauf-ID
- Testidentität
- Build-ID
- Ausführungsstatus
- Dauer
- Fehlermeldungen
- Zugehörige Diagnosen

Optional werden gespeichert:

- Stack Trace
- Erwarteter Wert
- Tatsächlicher Wert
- Testausgaben
- Coverage-Daten
- Ressourcenmessungen

Ergebnisse müssen maschinenlesbar exportierbar sein.

## Testhistorie

NovaLang Studio kann vergangene Testläufe speichern.

Unterstützt werden:

- Vergleich von Testläufen
- Erkennung neu fehlgeschlagener Tests
- Erkennung behobener Fehler
- Anzeige wiederholt instabiler Tests
- Zuordnung zu Build-Versionen

Historische Ergebnisse dürfen nicht als aktuelle Testergebnisse dargestellt werden.

Speicherumfang und Aufbewahrungsdauer müssen begrenzbar sein.

## Build-Integration

Der Test Manager verwendet `NPSPEC-STUDIO-BUILD-0001`.

Vor einer Testausführung werden erforderliche Testartefakte erstellt oder validiert.

Fehlgeschlagene Builds verhindern die Ausführung betroffener Tests.

Unabhängige gültige Testziele dürfen weiterhin ausgeführt werden, sofern ihre Voraussetzungen erfüllt sind.

## Debugger-Integration

Tests können mit `NPSPEC-STUDIO-DEBUGGER-0001` ausgeführt werden.

Unterstützt werden:

- Breakpoints
- Step Into, Step Over und Step Out
- Variable Inspector
- Call Stack
- Exception-Diagnostik
- Logic-Graph-Debugging

Der Debugger verwendet denselben Testausführungskontext wie der Test Runner.

## Diagnostik

Testergebnisse werden in `NPSPEC-STUDIO-DIAGNOSTICS-0001` integriert.

Fehlgeschlagene Assertions müssen die Ursache möglichst präzise anzeigen.

Testfehler, Infrastrukturfehler, Timeouts und Berechtigungsfehler müssen unterscheidbar bleiben.

## Headless Testing

Das Testsystem muss ohne grafische Entwicklungsumgebung ausführbar sein.

Unterstützt werden:

- Kommandozeilenaufrufe
- CI/CD-Integration
- Automatisierte Testausführung
- Maschinenlesbare Ergebnisse
- Definierte Exit-Codes
- Konfigurierbare Testfilter
- Ressourcenlimits

Headless Testing verwendet dieselbe Test Engine wie NovaLang Studio.

## Performance

- Test Discovery erfolgt inkrementell.
- Unveränderte Testmetadaten werden wiederverwendet.
- Unabhängige Tests können parallel laufen.
- Testumgebungen werden bedarfsgerecht initialisiert.
- Ausgabe- und Ergebnisdaten werden begrenzt.
- Lang laufende Tests müssen abbrechbar sein.
- Die Studio-Oberfläche darf während der Testausführung nicht blockieren.

## Sicherheit

- Tests laufen in autorisierten und isolierten Ausführungskontexten.
- Capability-Berechtigungen dürfen nicht automatisch aus produktiven Solutions übernommen werden.
- Test-Mocks dürfen keine Sicherheitsgrenzen umgehen.
- Netzwerk- und Dateizugriffe benötigen entsprechende Berechtigungen.
- Vertrauliche Testdaten dürfen nicht unautorisiert protokolliert werden.
- Testartefakte müssen auf Integrität geprüft werden.
- Temporäre Ressourcen müssen nach Testende freigegeben werden.
- Nicht vertrauenswürdiger Testcode darf NovaLang Studio nicht kompromittieren.

## Normative Anforderungen

1. NovaLang Studio MUSS einen zentralen Test Manager bereitstellen.
2. Unit-, Integration-, Functional- und Regression-Tests MÜSSEN unterstützt werden.
3. Tests MÜSSEN automatisch erkannt und einzeln ausführbar sein.
4. Das Test Framework MUSS native Assertions bereitstellen.
5. Testläufe MÜSSEN eindeutige Identitäten und strukturierte Ergebnisse besitzen.
6. Test Fixtures MÜSSEN unterstützt werden.
7. Tests MÜSSEN in definierten Ausführungskontexten laufen.
8. Parallele Testausführung MUSS kontrollierbar sein.
9. Parametrisierte und asynchrone Tests MÜSSEN unterstützt werden.
10. Logic-Graph-Knoten und Datenflüsse MÜSSEN testbar sein.
11. Capability-Mocks MÜSSEN typisierte Verträge einhalten.
12. Solutions MÜSSEN ohne automatische Übernahme produktiver Berechtigungen testbar sein.
13. `.nui`-Oberflächen MÜSSEN automatisiert getestet werden können.
14. Deterministische Testausführung MUSS für unterstützte Komponenten verfügbar sein.
15. Test Coverage MUSS für Quellcode und Logic Graph erfassbar sein.
16. Testergebnisse MÜSSEN in das zentrale Diagnosesystem integriert werden.
17. Fehlgeschlagene Tests MÜSSEN direkt debugbar sein.
18. Testläufe MÜSSEN kontrolliert abbrechbar sein.
19. Testausführung MUSS ohne grafische Benutzeroberfläche möglich sein.
20. Ressourcenlimits und Sicherheitsgrenzen MÜSSEN eingehalten werden.
21. Das Testsystem MUSS ohne KI vollständig funktionieren.

## Ergebnis

NovaLang Studio erhält ein integriertes, automatisierbares und sicheres Testsystem für klassische NovaLang-Programme und NovaOS-Solutions.

Quellcode, Logic Graphs, Capabilities und deklarative Benutzeroberflächen können innerhalb einer gemeinsamen Testinfrastruktur geprüft werden.

Durch inkrementelle Testerkennung, parallele Ausführung, deterministische Testumgebungen und direkte Debugger-Integration wird die Qualitätssicherung beschleunigt und die Zuverlässigkeit entwickelter Software verbessert.
