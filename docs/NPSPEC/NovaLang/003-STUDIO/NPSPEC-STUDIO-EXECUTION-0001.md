
# NPSPEC-STUDIO-EXECUTION-0001 – NovaLang Studio Execution

## Status

Angenommen

## Kategorie

NovaLang Studio / Ausführung / Entwicklungswerkzeuge

## Zweck

Definiert die Ausführung von NovaLang-Programmen, Projekten und Solutions direkt innerhalb von NovaLang Studio.

Ziel ist eine schnelle, kontrollierte und einheitliche Ausführungsumgebung für Entwicklung, Tests und Debugging.

Die Ausführung muss unabhängig vom verwendeten Backend funktionieren und die Sicherheitsarchitektur von NovaOS vollständig berücksichtigen.

## Architektur

| Komponente | Aufgabe |
|---|---|
| Execution Manager | Zentrale Verwaltung von Ausführungen |
| Execution Launcher | Vorbereitung und Start |
| Execution Configuration | Verwaltung von Startkonfigurationen |
| Runtime Connector | Verbindung zur NovaLang Runtime |
| Process Controller | Steuerung gestarteter Prozesse |
| Solution Executor | Ausführung von Solutions |
| Graph Execution Bridge | Verbindung zur Logic-Graph-Runtime |
| Output Manager | Verwaltung von Ausgaben |
| Execution Monitor | Überwachung von Zustand und Ressourcen |

Die Komponenten verwenden die Ausführungsmechanismen aus `NPSPEC-NOVALANG-EXECUTION-0001`.

## Ausführungsziele

NovaLang Studio unterstützt:

- Einzelne ausführbare NovaLang-Projekte
- Klassische NovaLang-Programme
- NovaOS-Solutions
- Logic Graphs innerhalb von Solutions
- Ausführbare Testziele
- Definierte Einstiegspunkte

Nicht ausführbare Bibliotheken müssen über ein geeignetes Startprojekt oder Testziel gestartet werden.

## Ausführungsmodi

| Modus | Beschreibung |
|---|---|
| Run | Normale Ausführung ohne aktiven Debugger |
| Debug | Ausführung mit Debugger |
| Test | Ausführung definierter Tests |
| Preview | Kontrollierte Vorschau von UI oder Solution |
| Deterministic | Reproduzierbare Ausführung bei unterstütztem Ziel |

Die Modi verwenden ein gemeinsames Ausführungsmodell.

## Execution Configuration

Jede Startkonfiguration enthält:

- Eindeutige Konfigurationsidentität
- Zielprojekt oder Solution
- Einstiegspunkt
- Ausführungsmodus
- Runtime-Backend
- Startargumente
- Arbeitsverzeichnis
- Zulässige Umgebungsparameter
- Ressourcenlimits
- Optionale Debug-Einstellungen

Konfigurationen können projekt- oder workspacebezogen gespeichert werden.

## Ausführungszustände

Eine Ausführung verwendet folgende Zustände:

```text
Created
   |
   v
Preparing
   |
   v
Starting
   |
   v
Running <----> Paused
   |
   v
Stopping
   |
   v
Completed / Failed / Cancelled
```

Ungültige Zustandsübergänge müssen verhindert werden.

Jede Ausführung besitzt eine eindeutige Sitzungsidentität.

## Startvorgang

Der Execution Launcher führt folgende Schritte aus:

1. Ausführungsziel ermitteln.
2. Startkonfiguration validieren.
3. Erforderliche Build-Artefakte prüfen.
4. Bei Bedarf Build ausführen.
5. Solution-Identität und Integrität prüfen.
6. Berechtigungen und Ressourcenlimits prüfen.
7. Runtime und Ausführungskontext vorbereiten.
8. Programm oder Solution starten.
9. Ausführung überwachen.

Fehler müssen über das zentrale Diagnosesystem gemeldet werden.

## Build-Integration

Der Execution Manager verwendet den Build Manager.

- Veraltete Artefakte müssen erkannt werden.
- Erforderliche Builds müssen vor dem Start abgeschlossen sein.
- Fehlgeschlagene Builds verhindern den Start ungültiger Artefakte.
- Bereits gültige Artefakte dürfen wiederverwendet werden.
- Ein Start mit älteren Artefakten darf nur als ausdrücklich gekennzeichnete Aktion erfolgen.

Build und Execution bleiben getrennte Komponenten.

## Runtime-Backends

| Backend | Ausführung |
|---|---|
| Interpreter | Direkte Ausführung von NovaLang-Bytecode |
| JIT | Laufzeitkompilierung unterstützter Codebereiche |
| AOT | Ausführung vorkompilierter nativer Artefakte |

Alle Backends müssen dieselbe definierte NovaLang-Semantik einhalten.

Backend-spezifische Einschränkungen müssen erkennbar sein.

## Programmausführung

Klassische Programme werden über ihren definierten Einstiegspunkt gestartet.

Beispiel:

```vb
Module Program

    Sub Main()
        Console.WriteLine("Hallo NovaOS")
    End Sub

End Module
```

Der Einstiegspunkt wird durch die Projektkonfiguration beziehungsweise die NovaLang-Sprachregeln bestimmt.

## Solution-Ausführung

Solutions bestehen aus:

- Capabilities
- Logic Graph
- Custom-NovaLang-Skripten
- Deklarativer UI

Der Solution Executor lädt und validiert die `solution.xml`.

Dabei werden geprüft:

- Solution-GUID
- Version und Integrität
- Capability-Anforderungen
- Berechtigungsstatus
- Logic-Graph-Struktur
- Verwendete Ressourcen

Die GUID allein stellt keinen Vertrauensnachweis dar.

## Logic-Graph-Ausführung

Die Graph Execution Bridge verbindet NovaLang Studio mit der Logic-Graph-Runtime.

Sie unterstützt:

- Starten definierter Graph-Einstiegspunkte
- Überwachung aktiver Knoten
- Anzeige von Ausführungszuständen
- Übergabe typisierter Daten
- Fehlerweiterleitung
- Kontrolliertes Beenden

Custom Scripts dürfen keine eigenständigen Systemberechtigungen anfordern.

Systemzugriffe erfolgen ausschließlich über explizit verbundene und autorisierte Capability-Knoten.

## Capability-Integration

Vor der Ausführung werden benötigte Capabilities geprüft.

Dabei gilt:

- Capability-Referenzen gewähren keine Berechtigungen.
- Bereits autorisierte Berechtigungen werden anhand der verifizierten Solution-Identität geprüft.
- Sicherheitsrelevante Änderungen können eine erneute Autorisierung erfordern.
- Fehlende Berechtigungen werden über den NovaOS-Berechtigungsmechanismus behandelt.
- Studio darf keine Berechtigungen selbstständig erteilen.

Ein verweigerter Zugriff muss als strukturierte Diagnose verfügbar sein.

## Ausführungssteuerung

NovaLang Studio unterstützt:

- Start
- Stop
- Restart
- Pause bei unterstützter Runtime
- Continue bei angehaltener Ausführung
- Auswahl der Startkonfiguration
- Wechsel zwischen aktiven Ausführungen

Pause und Continue werden über die Runtime beziehungsweise den Debugger ausgeführt.

Ein erzwungener Abbruch muss vom regulären Beenden unterscheidbar sein.

## Parallele Ausführungen

Mehrere Ausführungssitzungen können gleichzeitig existieren.

Jede Sitzung besitzt:

- Eigenen Ausführungskontext
- Eigene Ressourcenlimits
- Eigenen Status
- Eigene Ausgaben
- Eigene Diagnosezuordnung

Gemeinsame Ressourcen dürfen nur gemäß den NovaOS-Zugriffsregeln verwendet werden.

## Output Management

Der Output Manager verarbeitet:

- Standardausgabe
- Fehlerausgabe
- Runtime-Diagnosen
- Build-Meldungen
- Solution-Ereignisse
- Beendigungsstatus

Ausgaben werden nach Sitzung und Quelle getrennt.

Große Ausgabemengen müssen begrenzt und bei Bedarf virtualisiert dargestellt werden.

## Execution Monitor

Der Execution Monitor zeigt verfügbare Laufzeitinformationen:

- Ausführungsstatus
- Laufzeitdauer
- CPU-Auslastung
- Speicherverbrauch
- Task-Anzahl
- Ressourcenlimits
- Exit-Code
- Fehlerzustand

Nicht verfügbare Messwerte müssen entsprechend gekennzeichnet werden.

## Debugger-Integration

Der Execution Manager arbeitet mit `NPSPEC-STUDIO-DEBUGGER-0001` zusammen.

Im Debug-Modus werden:

- Debug-Sitzungen initialisiert
- Breakpoints registriert
- Ausführungskontexte verbunden
- Runtime-Ereignisse weitergeleitet
- Pausen und Fortsetzungen koordiniert

Der Debugger darf die grundlegende Ausführungsverwaltung nicht ersetzen.

## UI-Preview

Der Preview-Modus ermöglicht die kontrollierte Ausführung deklarativer Oberflächen.

Unterstützt werden:

- Start der UI-Vorschau
- Darstellung aktueller Komponenten
- Aktualisierung nach Änderungen
- Diagnose fehlerhafter Bindungen
- Kontrolliertes Beenden

Die Vorschau verwendet einen isolierten Ausführungskontext.

Externe Systemzugriffe benötigen dieselbe Autorisierung wie bei regulärer Ausführung.

## Benutzeroberfläche

NovaLang Studio verwendet die NovaOS-Designsprache.

- Direkt erreichbare Run- und Debug-Schaltflächen
- Auswahl der Startkonfiguration
- Kompakte Statusanzeige
- Integrierte Output-Ansicht
- Anzeige aktiver Ausführungssitzungen
- Schneller Zugriff auf Fehler und Diagnosen
- Tastenkombinationen für häufige Aktionen

Die Bedienung muss ohne unnötige Menüwechsel möglich sein.

## Performance

- Gültige Build-Artefakte werden wiederverwendet.
- Runtime-Komponenten werden bedarfsgerecht geladen.
- Startvorgänge erfolgen asynchron.
- Unnötige Initialisierungen werden vermieden.
- Ausgaben und Monitoring-Daten werden gepuffert und begrenzt.
- Ressourcenlimits werden durch die Runtime und NovaOS durchgesetzt.
- Die Studio-Oberfläche darf durch gestartete Programme nicht blockiert werden.

## Sicherheit

- Programme und Solutions werden in ihrem vorgesehenen Sicherheitskontext ausgeführt.
- Sandbox- und Capability-Grenzen bleiben wirksam.
- Studio darf keine privilegierten Systemzugriffe vermitteln, die dem Ausführungsziel nicht erlaubt sind.
- Startargumente und Umgebungsdaten müssen kontrolliert übergeben werden.
- Vertrauliche Informationen dürfen nicht unautorisiert protokolliert werden.
- Ausführungen müssen kontrolliert beendet und ihre Ressourcen freigegeben werden.
- Nicht vertrauenswürdige Programme dürfen Studio nicht kompromittieren.

## Normative Anforderungen

1. NovaLang Studio MUSS einen zentralen Execution Manager bereitstellen.
2. Programme, Projekte und Solutions MÜSSEN direkt aus Studio gestartet werden können.
3. Run-, Debug- und Test-Ausführung MÜSSEN unterstützt werden.
4. Ausführungen MÜSSEN eindeutige Sitzungsidentitäten besitzen.
5. Startkonfigurationen MÜSSEN gespeichert und wiederverwendet werden können.
6. Der Execution Manager MUSS mit dem Build Manager zusammenarbeiten.
7. Ungültige oder fehlgeschlagene Build-Artefakte DÜRFEN nicht unbeabsichtigt gestartet werden.
8. Interpreter, JIT und AOT MÜSSEN ein gemeinsames Ausführungsmodell verwenden.
9. Solutions MÜSSEN vor der Ausführung hinsichtlich Identität, Integrität und Berechtigungen geprüft werden.
10. Logic-Graph-Ausführungen MÜSSEN integriert sein.
11. Custom Scripts DÜRFEN keine eigenständigen Capability-Berechtigungen anfordern.
12. Parallele Ausführungen MÜSSEN voneinander unterscheidbar sein.
13. Ausgaben, Diagnosen und Ressourceninformationen MÜSSEN einer Sitzung zugeordnet werden.
14. Start, Stop und Restart MÜSSEN unterstützt werden.
15. Ausführungen DÜRFEN die Benutzeroberfläche nicht blockieren.
16. Ressourcenlimits, Sandbox- und Capability-Grenzen MÜSSEN eingehalten werden.
17. Die grundlegende Ausführung MUSS ohne KI vollständig funktionieren.

## Ergebnis

NovaLang Studio erhält eine zentrale, schnelle und sichere Ausführungsverwaltung für klassische NovaLang-Programme, Projekte und NovaOS-Solutions.

Entwickler können Anwendungen und Logic Graphs direkt starten, testen, überwachen und debuggen, während Build-System, Runtime und NovaOS-Sicherheitsarchitektur klar voneinander getrennt bleiben.
