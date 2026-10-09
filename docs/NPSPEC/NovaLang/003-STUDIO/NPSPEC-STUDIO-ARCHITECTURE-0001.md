
# NPSPEC-STUDIO-ARCHITECTURE-0001 – NovaLang Studio Architektur

## Status

Angenommen

## Kategorie

NovaLang Studio / Architektur

## Zweck

Definiert die modulare Architektur von NovaLang Studio als integrierte Entwicklungsumgebung für NovaLang, Solutions, Logic Graph und deklarative Benutzeroberflächen.

Ziel sind hohe Geschwindigkeit, geringer Ressourcenverbrauch und eine einheitliche Entwicklungsumgebung ohne unnötige Werkzeugwechsel.

## Architekturprinzipien

- Modularer Aufbau mit klar getrennten Komponenten
- Gemeinsames Projekt- und Dokumentmodell
- Einheitliche Compiler- und Analyseinfrastruktur
- Ereignisbasierte Kommunikation zwischen Komponenten
- Inkrementelle Verarbeitung statt vollständiger Neuberechnung
- Bedarfsgerechtes Laden von Funktionen
- Erweiterbarkeit über definierte Schnittstellen

## Kernkomponenten

| Komponente | Aufgabe |
|---|---|
| Studio Shell | Fenster, Navigation und Arbeitsbereiche |
| Workspace Manager | Projekte, Solutions und Dateien |
| Code Editor | Bearbeitung von NovaLang |
| Logic Graph Editor | Visuelle Verknüpfung von Capabilities und Skripten |
| UI Designer | Gestaltung deklarativer `.nui`-Oberflächen |
| Language Service | Syntaxanalyse, Typprüfung und IntelliSense |
| Build Manager | Compiler, AOT, Bytecode und Builds |
| Debug Manager | Debugging, Diagnostik und Profiling |
| Preview Engine | Vorschau von Oberflächen und Solutions |
| Extension Manager | Verwaltung zusätzlicher Studio-Funktionen |

## Gemeinsames Entwicklungsmodell

Alle Editoren arbeiten auf einem gemeinsamen Workspace-Modell.

- `.nova` enthält allgemeinen NovaLang-Quellcode.
- `.nlf` enthält NovaLang-Code für Logic-Graph-Komponenten.
- `.nui` beschreibt deklarative Benutzeroberflächen mit derselben NovaLang-Sprachsemantik.
- `solution.xml` definiert Identität und Konfiguration einer Solution.
- Änderungen werden zwischen Code, Logic Graph und UI Designer synchronisiert.

Die Editoren dürfen keine voneinander abweichenden Sprachdefinitionen verwenden.

## Logic-Graph-Integration

Der Logic Graph verbindet Capabilities, Datenverarbeitung und Custom Scripts.

Beispiel:

`Netzwerk-Capability → Custom Script → UI-Capability`

Custom Scripts dürfen ausschließlich bereitgestellte Daten und Capability-Handles verwenden.

Der Editor muss Verbindungen anhand ihrer Typen und Capability-Verträge validieren.

## Komponentenkommunikation

Studio-Komponenten kommunizieren über versionierte interne Schnittstellen und Ereignisse.

Gemeinsame Dienste verwalten:

- Dokumentzustand und Änderungen
- Symbolinformationen und Diagnosen
- Build- und Debug-Sitzungen
- Projektabhängigkeiten
- Berechtigungen und Capability-Verträge

Unabhängige Komponenten dürfen bei Fehlern nicht den gesamten Workspace beschädigen.

## Benutzeroberfläche

NovaLang Studio verwendet die NovaOS-Designsprache.

- Einheitliche Commandbar für Suche, Befehle und Aufgaben
- Direkt erreichbare häufig verwendete Funktionen
- Frei anordenbare Editorbereiche
- Kontextabhängige Werkzeuge ohne störende Layoutsprünge
- Gemeinsame Navigation zwischen Code, Logic Graph und UI Designer

Die Oberfläche muss auch ohne KI vollständig bedienbar bleiben.

## Performance

- Schneller Start durch Lazy Loading
- Inkrementelle Syntax- und Typanalyse
- Asynchrone Hintergrundverarbeitung
- Wiederverwendung von Compiler- und Analyseergebnissen
- Begrenzte CPU- und Speicherbudgets
- Keine Blockierung der Benutzeroberfläche durch Builds oder Analysen

## Sicherheit

- Studio-Komponenten unterliegen dem NovaOS-Capability-Modell.
- Erweiterungen erhalten nur ausdrücklich autorisierte Berechtigungen.
- Vorschau und Ausführung nicht vertrauenswürdigen Codes erfolgen isoliert.
- Projekt- und Solution-Identitäten müssen auf Integrität geprüft werden.
- KI-Funktionen dürfen keine Sicherheitsentscheidungen eigenständig umgehen.

## Normative Anforderungen

1. NovaLang Studio MUSS modular und erweiterbar aufgebaut sein.
2. Code Editor, Logic Graph und UI Designer MÜSSEN ein gemeinsames Projektmodell verwenden.
3. Alle Editoren MÜSSEN dieselbe NovaLang-Sprachdefinition nutzen.
4. Compiler, Debugger und Language Service MÜSSEN über definierte Schnittstellen integriert sein.
5. Änderungen MÜSSEN inkrementell verarbeitet werden können.
6. Lang laufende Operationen DÜRFEN die Benutzeroberfläche nicht blockieren.
7. Erweiterungen und Vorschauprozesse MÜSSEN den NovaOS-Sicherheitsregeln unterliegen.
8. Das Studio MUSS ohne KI vollständig funktionsfähig sein.
9. Nicht benötigte Komponenten SOLLEN erst bei Bedarf geladen werden.

## Ergebnis

NovaLang Studio erhält eine modulare, leistungsfähige und einheitliche Architektur, die Codeentwicklung, Logic Graph, UI-Design, Build und Debugging in einer integrierten NovaOS-Entwicklungsumgebung vereint.
