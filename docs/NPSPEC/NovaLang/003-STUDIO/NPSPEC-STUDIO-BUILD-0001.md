
# NPSPEC-STUDIO-BUILD-0001 – NovaLang Studio Build System

## Status

Angenommen

## Kategorie

NovaLang Studio / Build / Kompilierung

## Zweck

Definiert das integrierte Build-System von NovaLang Studio zur Übersetzung, Validierung und Bereitstellung von NovaLang-Projekten und NovaOS-Solutions.

Ziel ist ein schnelles, inkrementelles, reproduzierbares und ressourcenschonendes Build-Verfahren.

Das Build-System muss klassische Programme, Bibliotheken und Solutions unterstützen und unabhängig von der grafischen Entwicklungsumgebung ausführbar sein.

## Architektur

| Komponente | Aufgabe |
|---|---|
| Build Manager | Zentrale Steuerung aller Build-Vorgänge |
| Build Planner | Ermittlung erforderlicher Build-Schritte |
| Dependency Resolver | Auflösung von Projekt- und Modulabhängigkeiten |
| Incremental Build Engine | Wiederverwendung unveränderter Ergebnisse |
| Compiler Bridge | Anbindung des NovaLang-Compilers |
| Artifact Manager | Verwaltung erzeugter Artefakte |
| Build Cache | Zwischenspeicherung gültiger Ergebnisse |
| Build Validator | Prüfung von Artefakten und Verträgen |
| Build Diagnostics | Weiterleitung von Fehlern und Warnungen |

Der Build Manager verwendet die Compilerarchitektur aus `NPSPEC-NOVALANG-COMPILER-0001`.

## Build-Ziele

NovaLang Studio unterstützt:

- Einzelne NovaLang-Quelldateien, sofern eigenständig übersetzbar
- Ausführbare Projekte
- Bibliotheken und Module
- Vollständige Solutions
- Logic-Graph-Definitionen
- Deklarative `.nui`-Oberflächen
- Testprojekte
- Native AOT-Artefakte
- NovaLang-Bytecode

Nicht ausführbare Projekte erzeugen Bibliotheks- oder Zwischenartefakte.

## Build-Modi

| Modus | Beschreibung |
|---|---|
| Debug | Build mit Debug-Informationen |
| Release | Optimierter Build |
| Incremental | Übersetzung geänderter Bestandteile |
| Rebuild | Vollständige Neuerstellung |
| Clean | Entfernen generierter Build-Artefakte |
| Validate | Prüfung ohne erforderliche vollständige Artefakterzeugung |

Debug und Release sind Build-Konfigurationen. Incremental, Rebuild, Clean und Validate beschreiben Build-Operationen.

## Build-Konfiguration

Eine Build-Konfiguration enthält:

- Eindeutige Konfigurationsidentität
- Zielprojekt oder Solution
- Zielarchitektur
- Ausführungsbackend
- Optimierungsstufe
- Debug-Informationen
- Compileroptionen
- Abhängigkeiten
- Ausgabeverzeichnis
- Ressourcenlimits
- Optionale deterministische Build-Einstellungen

Konfigurationen werden versioniert und projektbezogen gespeichert.

## Build-Ablauf

```text
Build Request
     |
     v
Configuration Validation
     |
     v
Dependency Resolution
     |
     v
Build Planning
     |
     v
Incremental Analysis
     |
     v
NovaLang Compiler
     |
     v
Artifact Generation
     |
     v
Validation
     |
     v
Build Result
```

Jeder Schritt muss einen eindeutig identifizierbaren Status besitzen.

## Compiler-Integration

Der Build Manager verwendet die gemeinsame NovaLang-Compilerpipeline:

1. Lexikalische Analyse
2. Syntaxanalyse
3. Semantische Analyse
4. Typprüfung
5. Erzeugung der Nova IR
6. Optimierung
7. Backend-Codegenerierung
8. Artefaktvalidierung

Der Compiler darf keine abweichende Sprachsemantik für Studio verwenden.

## Abhängigkeitsverwaltung

Der Dependency Resolver berücksichtigt:

- Projektabhängigkeiten
- Modulimporte
- Bibliotheksreferenzen
- Solution-Komponenten
- Logic-Graph-Skripte
- UI-Deklarationen
- Versionierte Schnittstellen

Abhängigkeiten werden als gerichteter Graph verwaltet.

Zyklische Abhängigkeiten müssen entsprechend den NovaLang-Modulregeln erkannt und behandelt werden.

Unvereinbare Versionen müssen als Diagnose gemeldet werden.

## Inkrementeller Build

Die Incremental Build Engine ermittelt betroffene Bestandteile anhand von:

- Quelltextänderungen
- Öffentlichen Schnittstellen
- Compileroptionen
- Abhängigkeitsversionen
- Zielarchitektur
- Backend-Version
- Relevanten Build-Metadaten

Unveränderte und gültige Zwischenergebnisse werden wiederverwendet.

Änderungen an privaten Implementierungsdetails sollen keine unnötigen Neubuilds unabhängiger Module verursachen.

## Build Cache

Der Build Cache speichert wiederverwendbare Zwischenergebnisse.

Cache-Einträge enthalten:

- Inhaltsbasierte Identität
- Compiler- und Backend-Version
- Relevante Konfiguration
- Abhängigkeitsinformationen
- Integritätsnachweis
- Erzeugte Artefakte

Ungültige oder beschädigte Cache-Einträge müssen verworfen werden.

Cache-Treffer dürfen keine erforderlichen Sicherheits- oder Validierungsprüfungen umgehen.

## Parallele Builds

Unabhängige Build-Schritte können parallel ausgeführt werden.

Dabei gilt:

- Abhängigkeiten bestimmen die Ausführungsreihenfolge.
- Ressourcenlimits müssen berücksichtigt werden.
- Build-Schritte müssen abbrechbar sein.
- Gemeinsame Artefakte dürfen nicht gleichzeitig unkontrolliert verändert werden.
- Fehler eines Schritts müssen abhängige Schritte kontrolliert verhindern.

Die Anzahl paralleler Aufgaben wird an die verfügbaren Ressourcen angepasst.

## Solution-Build

Beim Build einer NovaOS-Solution werden verarbeitet:

- `solution.xml`
- Projekt- und Modulabhängigkeiten
- Logic Graph
- Custom-NovaLang-Skripte
- `.nui`-Oberflächen
- Capability-Verträge
- Ressourcen und Metadaten

Der Solution-Build prüft insbesondere:

- Gültigkeit der Solution-Struktur
- Eindeutigkeit der Solution-GUID
- Typkompatibilität von Graph-Verbindungen
- Auflösbarkeit der Skripte
- UI-Bindungen
- Capability-Schnittstellen
- Versionskompatibilität

Die Solution-GUID bleibt bei regulären Builds erhalten.

Ein Build darf keine Capability-Berechtigungen erteilen.

## Logic-Graph-Build

Der Logic Graph wird in eine validierte, ausführbare Repräsentation überführt.

Dabei werden geprüft:

- Knotenidentitäten
- Ein- und Ausgangstypen
- Verbindungen
- Ausführungsabhängigkeiten
- Capability-Verträge
- Skriptreferenzen
- Fehlerbehandlung

Custom Scripts verwenden exakt dieselbe NovaLang-Syntax und Compilersemantik wie `.nova`-Dateien.

## UI-Build

`.nui`-Dateien werden durch die gemeinsame deklarative NovaLang-Semantik verarbeitet.

Geprüft werden:

- UI-Komponententypen
- Eigenschaften
- Datenbindungen
- Ereignisbindungen
- Ressourcenreferenzen
- Typkompatibilität

Die UI-Beschreibung muss als versioniertes Artefakt erzeugt und ohne KI rekonstruierbar sein.

## Artefaktverwaltung

Der Artifact Manager verwaltet:

- Native Binärdateien
- NovaLang-Bytecode
- Bibliotheken
- Debug-Symbole
- Solution-Artefakte
- UI-Beschreibungen
- Logic-Graph-Repräsentationen
- Build-Manifeste

Jedes Artefakt muss seinem Build und seiner Konfiguration eindeutig zugeordnet werden können.

## Build-Manifest

Ein Build-Manifest dokumentiert mindestens:

- Build-ID
- Projekt- oder Solution-Identität
- Build-Konfiguration
- Compiler-Version
- Zielarchitektur
- Abhängigkeitsversionen
- Artefaktliste
- Integritätswerte
- Build-Ergebnis

Zeitabhängige Metadaten dürfen reproduzierbare Artefakte nicht unnötig verändern.

## Reproduzierbare Builds

Bei aktiviertem deterministischem Build-Modus müssen identische Eingaben und definierte Werkzeugversionen identische Artefakte erzeugen.

Nicht deterministische Eingaben müssen ausgeschlossen oder ausdrücklich dokumentiert werden.

Build-Zeitstempel, Dateireihenfolgen und absolute Entwicklungspfade dürfen die Artefaktidentität nicht unbeabsichtigt beeinflussen.

## Build-Diagnostik

Build-Fehler werden über `NPSPEC-STUDIO-DIAGNOSTICS-0001` bereitgestellt.

Diagnosen enthalten:

- Fehlercode
- Schweregrad
- Build-ID
- Ursprungskomponente
- Betroffenes Projekt
- Quelltextposition, sofern verfügbar
- Beschreibung

Fehler müssen direkt zur betroffenen Stelle navigierbar sein.

## Build-Ausgabe

Das Build Output Panel zeigt:

- Aktuellen Build-Status
- Build-Schritte
- Fortschritt, soweit bestimmbar
- Fehler und Warnungen
- Build-Dauer
- Erzeugte Artefakte
- Abschlussstatus

Die Ausgabe muss kompakt, filterbar und nach Build-Sitzung unterscheidbar sein.

## Abbruch und Fehlerbehandlung

Build-Vorgänge müssen kontrolliert abgebrochen werden können.

Dabei gilt:

- Laufende Schritte erhalten ein Abbruchsignal.
- Temporäre Artefakte werden bereinigt.
- Bereits gültige Cache-Einträge bleiben erhalten.
- Unvollständige Ergebnisse dürfen nicht als gültig veröffentlicht werden.
- Vorherige gültige Artefakte bleiben nach Möglichkeit verfügbar.

## Execution-Integration

Der Build Manager arbeitet mit `NPSPEC-STUDIO-EXECUTION-0001` zusammen.

Vor einer Ausführung werden die erforderlichen Artefakte geprüft.

Ein erfolgreicher Build stellt dem Execution Manager ein validiertes Artefaktmanifest bereit.

Build und Programmausführung bleiben voneinander getrennt.

## Benutzeroberfläche

NovaLang Studio verwendet die NovaOS-Designsprache.

- Direkt erreichbare Build- und Rebuild-Aktionen
- Auswahl der Build-Konfiguration
- Kompakte Fortschrittsanzeige
- Integriertes Build Output Panel
- Direkte Fehlernavigation
- Anzeige des letzten Build-Ergebnisses
- Tastenkombinationen für häufige Aktionen

Die Benutzeroberfläche darf während eines Builds nicht blockieren.

## Headless Build

Das Build-System muss ohne grafische Studio-Oberfläche verwendbar sein.

Unterstützt werden:

- Kommandozeilenaufrufe
- Automatisierte Builds
- CI/CD-Integration
- Strukturierte Diagnoseausgabe
- Maschinenlesbare Build-Ergebnisse
- Definierte Exit-Codes

Headless Builds verwenden dieselbe Build Engine und Compilerpipeline wie Studio.

## Performance

- Inkrementelle Kompilierung wird bevorzugt.
- Unveränderte Artefakte werden wiederverwendet.
- Unabhängige Schritte werden parallelisiert.
- Build-Caches sind größenbegrenzt.
- Nicht benötigte Komponenten werden nicht geladen.
- Hintergrund-Builds dürfen die Editorreaktion nicht beeinträchtigen.
- Ressourcenlimits werden durch NovaOS durchgesetzt.

## Sicherheit

- Build-Skripte und externe Werkzeuge unterliegen den NovaOS-Sicherheitsregeln.
- Build-Vorgänge dürfen keine zusätzlichen Capability-Berechtigungen erzeugen.
- Nicht vertrauenswürdige Build-Schritte müssen isoliert werden.
- Cache- und Build-Artefakte müssen auf Integrität geprüft werden.
- Vertrauliche Daten dürfen nicht unautorisiert in Artefakte oder Logs gelangen.
- Build-Ausgaben dürfen keine geschützten Systembereiche unautorisiert verändern.
- Signierung und Berechtigungsverwaltung bleiben getrennte, autorisierte Prozesse.

## Normative Anforderungen

1. NovaLang Studio MUSS einen zentralen Build Manager bereitstellen.
2. Programme, Bibliotheken, Projekte und Solutions MÜSSEN gebaut werden können.
3. Debug- und Release-Konfigurationen MÜSSEN unterstützt werden.
4. Incremental Build, Rebuild, Clean und Validate MÜSSEN verfügbar sein.
5. Alle Builds MÜSSEN dieselbe NovaLang-Compilerpipeline verwenden.
6. Projekt- und Modulabhängigkeiten MÜSSEN automatisch aufgelöst werden.
7. Ungültige Abhängigkeiten MÜSSEN als Diagnose gemeldet werden.
8. Unveränderte gültige Build-Ergebnisse MÜSSEN wiederverwendet werden können.
9. Parallele Build-Schritte MÜSSEN Abhängigkeiten und Ressourcenlimits berücksichtigen.
10. `.nova`, `.nlf` und `.nui` MÜSSEN konsistent verarbeitet werden.
11. Logic-Graph-Verbindungen und Capability-Verträge MÜSSEN validiert werden.
12. Jeder Build MUSS eindeutig identifizierbar sein.
13. Erzeugte Artefakte MÜSSEN einem Build-Manifest zugeordnet werden.
14. Build-Vorgänge MÜSSEN kontrolliert abbrechbar sein.
15. Unvollständige oder ungültige Artefakte DÜRFEN nicht als erfolgreich veröffentlicht werden.
16. Build-Diagnosen MÜSSEN in das zentrale Diagnosesystem integriert sein.
17. Das Build-System MUSS ohne grafische Benutzeroberfläche funktionieren.
18. Build-Vorgänge DÜRFEN keine Capability- oder Sandbox-Grenzen umgehen.
19. Reproduzierbare Builds MÜSSEN bei definierter deterministischer Konfiguration unterstützt werden.
20. Das Build-System MUSS ohne KI vollständig funktionieren.

## Ergebnis

NovaLang Studio erhält ein einheitliches, inkrementelles und reproduzierbares Build-System für klassische Programme, Bibliotheken und NovaOS-Solutions.

Quellcode, Logic Graph und deklarative Oberflächen werden über eine gemeinsame Compiler- und Build-Infrastruktur verarbeitet.

Durch Caching, Parallelisierung und gezielte Neukompilierung werden Build-Zeiten reduziert, während Integrität, Sicherheit und Nachvollziehbarkeit erhalten bleiben.
