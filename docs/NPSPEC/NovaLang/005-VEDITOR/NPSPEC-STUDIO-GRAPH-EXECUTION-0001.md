
# NPSPEC-STUDIO-GRAPH-EXECUTION-0001 – NovaLang Studio Graph Execution

## Status

Angenommen

## Kategorie

NovaOS / NovaLang Studio / Logic Graph Editor / Ausführung

## Zweck

Definiert das Starten, Steuern und Überwachen von Logic-Graph-Ausführungen innerhalb von NovaLang Studio.

Ziel ist eine direkte, sichere und nachvollziehbare Ausführung von Solutions während der Entwicklung und beim Testen.

## Architektur

Das Execution-System besteht aus:

- **Execution Controller:** Steuerung von Start, Pause, Fortsetzung und Abbruch.
- **Runtime Bridge:** Verbindung zur Logic Graph Runtime.
- **Execution Session Manager:** Verwaltung isolierter Ausführungssitzungen.
- **Input Provider:** Bereitstellung von Eingabedaten und Startparametern.
- **Execution Monitor:** Überwachung von Zuständen und Ressourcen.
- **Result Collector:** Erfassung von Ergebnissen und Fehlern.

Die eigentliche Ausführung erfolgt ausschließlich über die bestehende Logic Graph Runtime.

## Ausführungsmodi

Unterstützt werden:

- **Run:** Reguläre Ausführung.
- **Debug:** Ausführung mit Haltepunkten und Inspektion.
- **Preview:** Kontrollierte Vorschau der Solution.
- **Deterministic:** Reproduzierbare Ausführung.
- **Replay:** Wiedergabe aufgezeichneter Ausführungen.

Alle Modi verwenden dieselben Graph- und NovaLang-Ausführungsverträge.

## Ausführungsablauf

1. Graphänderungen übernehmen.
2. Graph und Abhängigkeiten validieren.
3. Capability-Anforderungen und Berechtigungen prüfen.
4. Isolierten Ausführungskontext erstellen.
5. Graph Runtime starten.
6. Ausführungszustände und Ergebnisse überwachen.
7. Ressourcen nach Abschluss freigeben.

Ungültige Graphen dürfen nicht gestartet werden.

## Bedienung

Eine integrierte Ausführungsleiste stellt bereit:

- Starten
- Pausieren
- Fortsetzen
- Stoppen
- Neustarten
- Ausführungsmodus auswählen
- Aktuellen Status anzeigen

Häufig verwendete Aktionen müssen unmittelbar erreichbar sein.

## Laufzeitdarstellung

Der Editor kann folgende Informationen anzeigen:

- Aktive und abgeschlossene Knoten
- Wartende und asynchrone Aufgaben
- Aktuelle Datenflüsse
- Fehler und Warnungen
- Ausführungsdauer
- Ressourcenverbrauch

Die Darstellung erfolgt über separate Canvas-Overlays und Diagnoseansichten.

## Capability-Integration

Capability Nodes werden ausschließlich über die autorisierte NovaOS Capability Runtime ausgeführt.

Custom Scripts dürfen keine eigenständigen Systemzugriffe anfordern.

Preview und Debugging dürfen keine zusätzlichen Berechtigungen erzeugen.

Externe Seiteneffekte müssen auch während der Entwicklung den jeweiligen Capability-Verträgen entsprechen.

## Ausführungsisolation

Jede Ausführung besitzt einen eigenen Execution Context mit eindeutiger ID.

Zustände, Aufgaben und Ressourcen werden entsprechend den Isolationseinstellungen getrennt verwaltet.

Mehrere Ausführungssitzungen dürfen sich nicht unbeabsichtigt gegenseitig beeinflussen.

## Fehler und Abbruch

Fehler werden über das zentrale Logic Graph Error Handling verarbeitet.

Abbruchsignale werden gemäß Structured Concurrency weitergegeben.

Nach Beendigung müssen zugeordnete Ressourcen kontrolliert freigegeben werden.

## Normative Anforderungen

1. NovaLang Studio MUSS Logic Graphs direkt ausführen können.
2. Die Ausführung MUSS über die reguläre Logic Graph Runtime erfolgen.
3. Vor dem Start MÜSSEN Graphvalidierung und erforderliche Berechtigungsprüfungen erfolgen.
4. Run, Debug und Preview MÜSSEN unterstützt werden.
5. Deterministic und Replay MÜSSEN über die bestehende Runtime integrierbar sein.
6. Start, Pause, Fortsetzen und Stoppen MÜSSEN verfügbar sein.
7. Jede Ausführung MUSS eine eindeutige Execution-ID besitzen.
8. Ausführungszustände und Fehler MÜSSEN sichtbar sein.
9. Parallele und asynchrone Aufgaben MÜSSEN korrekt überwacht werden.
10. Ressourcenlimits und Isolation MÜSSEN durchgesetzt werden.
11. Abgebrochene Ausführungen MÜSSEN ihre Ressourcen kontrolliert freigeben.
12. Editor-Overlays DÜRFEN die Graph-Ausführungssemantik nicht verändern.
13. Die Ausführung DARF keine Capability- oder Berechtigungsgrenzen umgehen.
14. Alle grundlegenden Ausführungsfunktionen MÜSSEN ohne KI funktionieren.

## Ergebnis

NovaLang Studio erhält eine integrierte, sichere und kontrollierbare Ausführungsumgebung für Logic Graphs mit direkter Runtime-Anbindung, Live-Statusanzeige und vollständiger Unterstützung der bestehenden NovaOS-Ausführungsregeln.
