
# NPSPEC-STUDIO-DIAGNOSTICS-0001 – NovaLang Studio Diagnostics

## Status

Angenommen

## Kategorie

NovaLang Studio / Diagnostik / Fehleranalyse

## Zweck

Definiert die zentrale Erfassung, Darstellung und Verwaltung von Diagnosen innerhalb von NovaLang Studio.

Ziel ist, Fehler, Warnungen und Hinweise aus Compiler, Language Service, Runtime, Logic Graph und UI Designer verständlich darzustellen und ihre Ursachen schnell auffindbar zu machen.

Die Implementierung basiert auf dem Diagnosemodell aus `NPSPEC-NOVALANG-DIAGNOSTICS-0001`.

## Architektur

| Komponente | Aufgabe |
|---|---|
| Diagnostic Manager | Zentrale Verwaltung aller Diagnosen |
| Diagnostic Collector | Empfang von Diagnosen verschiedener Komponenten |
| Diagnostic Store | Speicherung und Aktualisierung |
| Diagnostic Filter | Filterung und Gruppierung |
| Diagnostic Renderer | Darstellung im Editor und Studio |
| Diagnostic Navigator | Navigation zu Fehlerursprüngen |
| Quick Fix Provider | Bereitstellung möglicher Korrekturen |
| Diagnostic History | Verwaltung vergangener Diagnosesitzungen |

Alle Komponenten verwenden ein gemeinsames, versioniertes Diagnoseformat.

## Diagnosequellen

NovaLang Studio verarbeitet Diagnosen aus:

- Lexer und Parser
- Type Checker und Compiler
- Language Service
- Bytecode Verifier
- Runtime und Nova VM
- Build Manager
- Workspace und Projektverwaltung
- Logic Graph
- UI Designer
- Capability- und Sicherheitsprüfung

Diagnosequellen müssen eindeutig identifizierbar sein.

## Diagnoseklassen

| Klasse | Bedeutung |
|---|---|
| Fatal | Kritischer, nicht fortsetzbarer Fehler |
| Error | Fehler, der eine Operation verhindert |
| Warning | Mögliches Problem |
| Information | Technischer Hinweis |
| Hint | Verbesserungsvorschlag |

Die Klassifizierung folgt dem gemeinsamen NovaLang-Diagnosemodell.

## Diagnosemodell

Jede Diagnose enthält mindestens:

- Stabilen Diagnosecode
- Schweregrad
- Beschreibung
- Ursprungskomponente
- Eindeutige Diagnoseidentität innerhalb der jeweiligen Sitzung

Soweit verfügbar werden ergänzt:

- Workspace- und Projektidentität
- Solution-Identität
- Dokument und Quelltextposition
- Logic-Graph-Knoten oder UI-Komponente
- Zugehörige Symbole
- Laufzeit- und Task-Kontext
- Dokument- oder Build-Version
- Verknüpfte Diagnosen
- Mögliche Korrekturaktionen

## Fehlercodes

NovaLang Studio übernimmt die stabilen NovaLang-Fehlercodes.

Beispiele:

- `NL-SYN-0001` – Syntaxfehler
- `NL-TYP-0001` – Typkonflikt
- `NL-RUN-0001` – Laufzeitfehler
- `NL-CAP-0001` – Capability-Zugriff verweigert

Studio-spezifische Diagnosen verwenden den eigenen Namensraum:

`NS-<BEREICH>-<NUMMER>`

Ein veröffentlichter Diagnosecode darf nicht für eine andere Fehlerbedeutung wiederverwendet werden.

## Diagnosedarstellung

NovaLang Studio stellt Diagnosen an mehreren Stellen dar:

| Ansicht | Darstellung |
|---|---|
| Code Editor | Unterstreichungen und Inline-Hinweise |
| Problems Panel | Zentrale Fehler- und Warnungsliste |
| Logic Graph | Markierung betroffener Knoten und Verbindungen |
| UI Designer | Markierung fehlerhafter Komponenten |
| Build Output | Build-bezogene Diagnoseausgabe |
| Debug Console | Laufzeitfehler und Ausführungshinweise |

Die Darstellungen müssen auf denselben Diagnosedaten basieren.

## Problems Panel

Das Problems Panel unterstützt:

- Gruppierung nach Datei, Projekt, Solution und Quelle
- Filterung nach Schweregrad
- Suche nach Fehlercode und Beschreibung
- Sortierung nach Position und Zeitpunkt
- Navigation zum Fehlerursprung
- Anzeige möglicher Korrekturen
- Unterscheidung aktueller und veralteter Diagnosen

Ein Klick auf eine Diagnose öffnet nach Möglichkeit direkt die betroffene Stelle.

## Echtzeitdiagnostik

Der Language Service liefert während der Bearbeitung inkrementelle Diagnosen.

- Änderungen aktualisieren nur betroffene Analysebereiche.
- Diagnosen werden an Dokumentversionen gebunden.
- Veraltete Ergebnisse dürfen aktuelle Diagnosen nicht überschreiben.
- Laufende Analysen müssen abbrechbar sein.
- Die Texteingabe darf nicht blockiert werden.

Unvollständiger Quellcode darf vorübergehend diagnostiziert werden, ohne den gesamten Workspace als fehlerhaft zu behandeln.

## Logic-Graph-Diagnostik

Der Logic Graph meldet insbesondere:

- Nicht verbundene erforderliche Eingänge
- Inkompatible Datentypen
- Ungültige Verbindungen
- Fehlende Capability-Verträge
- Nicht auflösbare Skriptreferenzen
- Ungültige Ausführungsabhängigkeiten

Die Diagnose muss den betroffenen Graph-Knoten oder die Verbindung eindeutig referenzieren.

Fehlende Capability-Berechtigungen müssen von fehlenden oder inkompatiblen Capabilities unterschieden werden.

## UI-Designer-Diagnostik

Für `.nui`-Dateien werden geprüft:

- Ungültige UI-Typen
- Nicht vorhandene Eigenschaften
- Fehlerhafte Datenbindungen
- Inkompatible Datentypen
- Ungültige Ereignisbindungen
- Fehlende referenzierte Ressourcen

Die Diagnose muss sowohl im visuellen Designer als auch im Quellcode erreichbar sein.

## Quick Fixes

Diagnosen können strukturierte Korrekturvorschläge enthalten.

Beispiele:

- Fehlenden Import ergänzen
- Typkonflikt beheben
- Nicht implementierte Member erzeugen
- Ungültige Datenbindung korrigieren
- Fehlende Graph-Verbindung ergänzen

Korrekturen verwenden die gemeinsame Refactoring-Infrastruktur.

Änderungen müssen überprüfbar und rückgängig machbar sein.

## Build- und Laufzeitdiagnostik

Build-Diagnosen werden einer konkreten Build-Sitzung zugeordnet.

Laufzeitdiagnosen können zusätzlich enthalten:

- Exception-Typ
- Call Stack
- Async Stack
- Task-Identität
- Ausführungskontext
- Ressourcenstatus

Laufzeitdiagnosen dürfen keine vertraulichen Informationen unautorisiert offenlegen.

## Diagnosehistorie

NovaLang Studio kann abgeschlossene Diagnose- und Build-Sitzungen speichern.

- Sitzungen müssen eindeutig identifizierbar sein.
- Aktuelle und historische Diagnosen bleiben unterscheidbar.
- Gespeicherte Daten müssen begrenzbar und löschbar sein.
- Historische Diagnosen dürfen nicht als aktueller Dokumentzustand dargestellt werden.

## Performance

- Diagnosen werden inkrementell aktualisiert.
- Wiederholte identische Meldungen können zusammengefasst werden.
- Diagnosepuffer besitzen konfigurierbare Grenzen.
- Große Ergebnismengen werden virtualisiert dargestellt.
- Hintergrundverarbeitung darf die Benutzeroberfläche nicht blockieren.
- Kritische Fehler müssen auch bei eingeschränkten Ressourcen gemeldet werden können.

## Sicherheit

- Diagnosen dürfen keine unautorisierten Speicherinhalte offenlegen.
- Geschützte Pfade, Zugangsdaten und Tokens müssen entsprechend den Zugriffsregeln behandelt werden.
- Diagnoseanzeigen dürfen keine zusätzlichen Capability-Berechtigungen erzeugen.
- Quick Fixes dürfen keine Sicherheitsgrenzen umgehen.
- Externe Diagnosequellen müssen validiert werden.
- Automatische Korrekturen dürfen nicht ohne kontrollierte Übernahme ausgeführt werden.

## Normative Anforderungen

1. NovaLang Studio MUSS einen zentralen Diagnostic Manager bereitstellen.
2. Alle Diagnosequellen MÜSSEN ein gemeinsames strukturiertes Diagnosemodell verwenden.
3. Jede Diagnose MUSS einen stabilen Fehlercode und Schweregrad besitzen.
4. Diagnosen MÜSSEN im Code Editor und Problems Panel dargestellt werden können.
5. Logic Graph und UI Designer MÜSSEN ihre Diagnosen in das gemeinsame System integrieren.
6. Diagnosen MÜSSEN ihrem Ursprung und Analysezustand zugeordnet werden.
7. Veraltete Diagnosen DÜRFEN aktuelle Analyseergebnisse nicht überschreiben.
8. Diagnoseaktualisierungen MÜSSEN inkrementell und asynchron erfolgen.
9. Die Navigation zum Diagnoseursprung MUSS unterstützt werden.
10. Quick Fixes MÜSSEN überprüfbar und rückgängig machbar sein.
11. Diagnosehistorien MÜSSEN von aktuellen Diagnosen unterscheidbar bleiben.
12. Diagnosepuffer und Ressourcenverbrauch MÜSSEN begrenzbar sein.
13. Diagnosen DÜRFEN keine Capability- oder Datenschutzgrenzen verletzen.
14. Das Diagnosesystem MUSS ohne KI vollständig funktionieren.

## Ergebnis

NovaLang Studio erhält ein zentrales, einheitliches und ressourcenschonendes Diagnosesystem für Quellcode, Projekte, Solutions, Logic Graph, UI Designer und Runtime.

Fehler werden unmittelbar erkennbar, eindeutig zugeordnet und mit wenigen Aktionen analysierbar oder korrigierbar.
