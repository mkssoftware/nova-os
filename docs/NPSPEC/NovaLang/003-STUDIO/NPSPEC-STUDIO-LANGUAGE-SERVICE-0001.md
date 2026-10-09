
# NPSPEC-STUDIO-LANGUAGE-SERVICE-0001 – NovaLang Studio Language Service

## Status

Angenommen

## Kategorie

NovaLang Studio / Sprachdienste / Architektur

## Zweck

Definiert den zentralen Language Service von NovaLang Studio.

Der Language Service stellt allen Entwicklungswerkzeugen eine gemeinsame Schnittstelle für Syntaxanalyse, Typprüfung, Symbolauflösung, Diagnostik und semantische Codeoperationen bereit.

Ziel sind konsistente Sprachinformationen, schnelle Reaktionszeiten und die Vermeidung mehrfacher Analyseimplementierungen.

## Architektur

| Komponente | Aufgabe |
|---|---|
| Language Service Host | Verwaltung des Sprachdienstes |
| Document Manager | Verwaltung versionierter Dokumentzustände |
| Syntax Service | Lexikalische und syntaktische Analyse |
| Semantic Service | Typprüfung und Symbolauflösung |
| Symbol Index | Projektübergreifende Symbolverwaltung |
| Dependency Tracker | Verfolgung von Analyseabhängigkeiten |
| Diagnostic Provider | Bereitstellung strukturierter Diagnosen |
| Language Protocol | Kommunikation mit Studio-Komponenten |

Der Language Service verwendet die gemeinsame NovaLang-Compilerinfrastruktur.

## Sprachmodell

Folgende Dateitypen werden unterstützt:

| Dateityp | Sprachmodell |
|---|---|
| `.nova` | Allgemeiner NovaLang-Quellcode |
| `.nlf` | NovaLang innerhalb von Logic Graph |
| `.nui` | Deklarative NovaLang-UI-Beschreibung |

Alle drei Dateitypen verwenden dieselbe grundlegende Syntax, Typsemantik und Symbolauflösung.

Kontextspezifische Einschränkungen werden durch zusätzliche Validierungsregeln umgesetzt, nicht durch getrennte Sprachimplementierungen.

## Dienste

Der Language Service stellt mindestens bereit:

- Syntaxanalyse
- Semantische Analyse
- Typprüfung
- Symbolauflösung
- Autovervollständigung
- Signature Help
- Hover-Informationen
- Definitionen und Referenzen
- Dokument- und Workspace-Symbole
- Semantische Token
- Code Actions
- Refactoring
- Formatierungsunterstützung
- Diagnostik

IntelliSense und Autocompletion verwenden diese Dienste, statt eigene Compilerlogik zu implementieren.

## Dokumentmodell

Jedes geöffnete Dokument besitzt:

- Eindeutige Dokumentidentität
- Zugehörigen Workspace
- Sprach- und Dateityp
- Dokumentversion
- Aktuellen Textzustand
- Zugeordnete Analyseergebnisse

Analyseergebnisse werden an die jeweilige Dokumentversion gebunden.

Ungespeicherte Änderungen müssen analysiert werden können, ohne die Datei vorher auf den Datenträger zu schreiben.

## Inkrementelle Analyse

Bei Änderungen werden nur betroffene Analysebereiche und deren Abhängigkeiten aktualisiert.

- Syntaxbäume werden soweit möglich wiederverwendet.
- Symboltabellen werden gezielt aktualisiert.
- Projektabhängigkeiten werden berücksichtigt.
- Unveränderte Ergebnisse bleiben zwischengespeichert.
- Veraltete Analyseanfragen werden abgebrochen oder verworfen.

Eine vollständige Projektanalyse bei jeder Tastatureingabe ist zu vermeiden.

## Symbolindex

Der Symbolindex verwaltet:

- Module und Namespaces
- Klassen, Strukturen und Interfaces
- Funktionen und Methoden
- Eigenschaften und Felder
- Variablen und Parameter
- Generische Typen
- Deklarationen und Referenzen

Symbole erhalten stabile Identitäten innerhalb des jeweiligen Analysekontexts.

Projektübergreifende Referenzen werden anhand der tatsächlichen Abhängigkeiten aufgelöst.

## Language Protocol

Studio-Komponenten kommunizieren über ein versioniertes, asynchrones Protokoll mit dem Language Service.

Eine Anfrage enthält mindestens:

- Operation
- Dokument- oder Workspace-Identität
- Dokumentversion, soweit erforderlich
- Position oder Textbereich
- Anfrageparameter

Eine Antwort enthält den Status und die angeforderten Daten beziehungsweise strukturierte Diagnosen.

Das Protokoll muss Abbruch, Fehlerbehandlung und die Erkennung veralteter Ergebnisse unterstützen.

Eine externe Anbindung über einen LSP-kompatiblen Adapter kann vorgesehen werden.

## Editor-Integration

Der Code Editor verwendet den Language Service für:

- Syntax Highlighting
- IntelliSense
- Autocompletion
- Fehlerkennzeichnung
- Symbolnavigation
- Refactoring
- Codeformatierung

Der Editor bleibt während laufender Analysen bedienbar.

## Logic-Graph-Integration

Der Language Service berücksichtigt den Ausführungskontext von Custom Scripts.

Dazu gehören:

- Typisierte Eingangs- und Ausgangswerte
- Bereitgestellte Capability-Handles
- Verfügbare Module und Symbole
- Typverträge verbundener Graph-Knoten

Der Language Service prüft die Gültigkeit von Skripten und Schnittstellen, erteilt jedoch keine Berechtigungen.

## UI-Designer-Integration

Für `.nui`-Dateien stellt der Language Service bereit:

- UI-Typinformationen
- Eigenschaften und Ereignisse
- Typprüfung von Datenbindungen
- Validierung deklarativer Ausdrücke
- Navigation zu referenzierten Symbolen
- Diagnosen ungültiger Bindungen

Code Editor und UI Designer müssen dieselben Analyseergebnisse verwenden können.

## Compiler-Integration

Language Service und Compiler teilen sich:

- Sprachdefinition
- Lexer und Parser
- Syntaxbaum
- Typensystem
- Semantische Regeln
- Diagnosekatalog

Der Language Service arbeitet fehlertolerant mit unvollständigem Quellcode.

Die endgültige Build-Gültigkeit wird weiterhin durch den Compiler und die erforderlichen Verifikationsschritte bestimmt.

## Performance

- Analysen erfolgen asynchron.
- Sichtbare und aktive Dokumente werden priorisiert.
- Ergebnisse werden inkrementell zwischengespeichert.
- Analyseaufgaben sind abbrechbar.
- CPU- und Speicherverbrauch sind begrenzbar.
- Nicht benötigte Projekte werden bedarfsgerecht analysiert.
- Große Workspaces dürfen die Benutzeroberfläche nicht blockieren.

## Zuverlässigkeit

- Fehler in einzelnen Analyseaufgaben dürfen den gesamten Workspace nicht beschädigen.
- Veraltete Ergebnisse dürfen aktuelle Ergebnisse nicht überschreiben.
- Analyse-Caches müssen bei Inkonsistenzen neu aufgebaut werden können.
- Der Language Service muss nach einem Fehler kontrolliert neu gestartet werden können.
- Dokumentzustände bleiben beim Workspace Manager erhalten.

## Sicherheit

- Quellcode wird zur Analyse nicht ausgeführt.
- Projektdateien gelten grundsätzlich als nicht vertrauenswürdig.
- Dateizugriffe unterliegen den NovaOS-Capability-Regeln.
- Der Language Service darf keine zusätzlichen Systemberechtigungen erzeugen.
- Erweiterungen dürfen nur autorisierte Sprachinformationen abrufen.
- Externe Sprachdienste erhalten keinen automatischen Zugriff auf Workspace-Inhalte.

## Normative Anforderungen

1. NovaLang Studio MUSS einen zentralen Language Service bereitstellen.
2. Der Language Service MUSS die gemeinsame NovaLang-Compilerinfrastruktur verwenden.
3. `.nova`, `.nlf` und `.nui` MÜSSEN dieselbe grundlegende Sprachsemantik verwenden.
4. Syntaxanalyse, Typprüfung, Symbolauflösung und Diagnostik MÜSSEN verfügbar sein.
5. Analyseergebnisse MÜSSEN eindeutig an Dokumentversionen gebunden sein.
6. Ungespeicherte Dokumentänderungen MÜSSEN analysierbar sein.
7. Analysen MÜSSEN inkrementell, asynchron und abbrechbar sein.
8. Veraltete Ergebnisse DÜRFEN aktuelle Analysezustände nicht überschreiben.
9. Editor, IntelliSense, Logic Graph und UI Designer MÜSSEN dieselben Sprachdienste verwenden können.
10. Das Kommunikationsprotokoll MUSS versioniert und erweiterbar sein.
11. Der Language Service DARF analysierten Quellcode nicht eigenständig ausführen.
12. Capability- und Sicherheitsgrenzen MÜSSEN eingehalten werden.
13. Der Language Service MUSS ohne KI vollständig funktionieren.

## Ergebnis

NovaLang Studio erhält einen zentralen, modularen und ressourcenschonenden Language Service als gemeinsame Grundlage aller sprachbezogenen Entwicklungsfunktionen.

Code Editor, IntelliSense, Logic Graph und UI Designer arbeiten dadurch mit konsistenten Syntax-, Typ- und Symbolinformationen, ohne voneinander unabhängige Sprachimplementierungen zu benötigen.
