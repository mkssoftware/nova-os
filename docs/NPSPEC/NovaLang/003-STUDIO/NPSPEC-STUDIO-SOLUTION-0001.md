
# NPSPEC-STUDIO-SOLUTION-0001 – NovaLang Studio Solution Management

## Status

Angenommen

## Kategorie

NovaLang Studio / Solution-Verwaltung

## Zweck

Definiert die Erstellung, Bearbeitung, Validierung und Verwaltung von NovaOS-Solutions innerhalb von NovaLang Studio.

Eine Solution kombiniert vorhandene Systemfähigkeiten (Capabilities), einen Logic Graph, NovaLang-Skripte und eine deklarative Benutzeroberfläche zu einer ausführbaren Funktionseinheit.

Ziel ist die Entwicklung vollständiger Anwendungen durch die Zusammenstellung wiederverwendbarer Fähigkeiten, ohne klassische monolithische Programme erstellen zu müssen.

## Solution-Modell

| Element | Beschreibung |
|---|---|
| Solution | Ausführbare Zusammenstellung von Fähigkeiten und UI |
| Solution Manifest | Identität, Konfiguration und Anforderungen |
| Capability References | Verwendete Systemfähigkeiten |
| Logic Graph | Datenfluss und Ausführungslogik |
| Custom Scripts | Individuelle Verarbeitung mit NovaLang |
| UI Definition | Deklarative Benutzeroberfläche |
| Resources | Bilder, Daten und weitere Ressourcen |
| Solution State | Persistenter Anwendungszustand |

Eine Solution ist kein klassisches monolithisches Programm.

## Solution-Struktur

Beispiel:

```text
MeineSolution/
├── solution.xml
├── Logic/
│   ├── main.nlf
│   └── processing.nlf
├── UI/
│   └── main.nui
├── Scripts/
│   └── helper.nova
└── Resources/
```

Die Struktur ist eine Konvention und darf erweitert werden.

## Solution-Manifest

`solution.xml` beschreibt die Solution und enthält mindestens:

- Dauerhaft eindeutige Solution-GUID
- Name und Version
- Manifest-Schemaversion
- Einstiegspunkt des Logic Graph
- Referenzen auf UI und Ressourcen
- Verwendete Capabilities und Vertragsversionen
- Erforderliche NovaLang- und NovaOS-Versionen
- Integritäts- und Signaturinformationen, soweit vorgesehen

Die GUID identifiziert die Solution dauerhaft, ist allein jedoch kein Nachweis ihrer Vertrauenswürdigkeit.

## Solution Manager

Der Solution Manager übernimmt:

- Erstellen und Öffnen von Solutions
- Verwaltung des Solution-Manifests
- Hinzufügen und Entfernen von Capabilities
- Verwaltung von Logic Graph und UI
- Einbindung von NovaLang-Projekten
- Validierung von Abhängigkeiten
- Build, Vorschau und Ausführung
- Export und Paketierung

Änderungen müssen konsistent zwischen allen Studio-Editoren synchronisiert werden.

## Capability-Verwaltung

Capabilities werden über ihre eindeutige Identität referenziert.

Format:

`domain.authority.namespace.name`

Beispiel:

`de.nova.network.http.request`

Der Solution Manager stellt einen Capability-Katalog bereit, über den Fähigkeiten gesucht, ausgewählt und in den Logic Graph eingefügt werden können.

Die Aufnahme einer Capability in die Solution bedeutet nicht, dass ihre Ausführung bereits autorisiert ist.

## Logic-Graph-Integration

Der Logic Graph definiert die tatsächliche Verknüpfung der Fähigkeiten.

Beispiel:

`Netzwerk-Capability → Custom Script → Datenverarbeitung → UI-Capability`

- Verbindungen müssen typkompatibel sein.
- Capability-Eingaben und -Ausgaben besitzen definierte Verträge.
- Custom Scripts verarbeiten ausschließlich bereitgestellte Daten und Handles.
- Skripte dürfen keine zusätzlichen Systemberechtigungen selbst anfordern.
- Der Graph muss vor der Ausführung validiert werden.

## UI-Integration

Die Benutzeroberfläche wird über `.nui` beschrieben.

Der UI Designer ermöglicht:

- Visuelle Bearbeitung
- Datenbindung an Logic-Graph-Ausgaben
- Vorschau
- Ereignisverknüpfungen
- Responsive Layouts
- Prüfung benötigter UI-Capabilities

`.nui` verwendet die entsprechende deklarative NovaLang-Syntax und dieselben Typregeln.

## Identität und Berechtigungen

Jede Solution besitzt eine dauerhaft eindeutige GUID.

NovaOS bindet dauerhaft erteilte Capability-Berechtigungen an die verifizierte Solution-Identität.

Dabei gilt:

- Die GUID allein begründet kein Vertrauen.
- Integrität und gegebenenfalls Herausgeberidentität müssen geprüft werden.
- Sicherheitsrelevante Änderungen können eine erneute Berechtigungsprüfung erfordern.
- Unveränderte, weiterhin autorisierte Solutions sollen nicht bei jedem Start erneut nach denselben Berechtigungen fragen.
- Berechtigungen dürfen nicht durch Kopieren oder Manipulieren einer GUID übernommen werden.

Die endgültige Autorisierung erfolgt durch NovaOS, nicht durch NovaLang Studio.

## Build und Validierung

Vor der Ausführung werden geprüft:

- Gültigkeit des Manifests
- Sprach- und Versionskompatibilität
- Verfügbarkeit benötigter Capabilities
- Typkorrektheit des Logic Graph
- Gültigkeit der NovaLang-Skripte
- Konsistenz der UI-Bindungen
- Integrität der Solution-Artefakte

Fehler werden über das gemeinsame Diagnosesystem angezeigt.

## Vorschau und Debugging

NovaLang Studio unterstützt:

- Isolierte Solution-Vorschau
- Breakpoints in Custom Scripts
- Untersuchung von Logic-Graph-Datenflüssen
- Anzeige von Capability-Aufrufen
- Fehlerdiagnostik einzelner Graph-Knoten
- Profiling von Ausführungszeiten und Ressourcenverbrauch

Vorschau und Debugging dürfen keine Sicherheitsgrenzen umgehen.

## Portabilität

Solutions sollen zwischen kompatiblen NovaOS-Systemen übertragbar sein.

- Referenzen müssen möglichst geräteunabhängig sein.
- Capability-Versionen müssen überprüft werden.
- Fehlende Fähigkeiten müssen eindeutig gemeldet werden.
- Berechtigungen dürfen nicht ungeprüft auf andere Geräte übertragen werden.
- UI und Logic Graph müssen aus den gespeicherten Artefakten rekonstruierbar sein.

## Normative Anforderungen

1. NovaLang Studio MUSS Solutions als eigenständige Entwicklungseinheiten unterstützen.
2. Jede Solution MUSS eine dauerhaft eindeutige GUID besitzen.
3. `solution.xml` MUSS Identität, Konfiguration und Capability-Anforderungen beschreiben.
4. Logic Graph, Custom Scripts und UI MÜSSEN gemeinsam verwaltet werden können.
5. Capability-Verbindungen MÜSSEN typisiert und validiert werden.
6. Custom Scripts DÜRFEN keine zusätzlichen Capabilities eigenständig anfordern.
7. Capability-Referenzen DÜRFEN nicht mit erteilten Berechtigungen gleichgesetzt werden.
8. Solution-Identität und Integrität MÜSSEN vor der Übernahme gespeicherter Berechtigungen geprüft werden.
9. Sicherheitsrelevante Änderungen MÜSSEN eine erneute Autorisierungsprüfung auslösen können.
10. Solutions MÜSSEN isoliert getestet und debuggt werden können.
11. Solution-Artefakte MÜSSEN versionierbar und portabel sein.
12. Der Solution Manager MUSS ohne KI vollständig funktionieren.

## Ergebnis

NovaLang Studio erhält eine integrierte Solution-Verwaltung, mit der Capabilities, Logic Graph, NovaLang-Skripte und Benutzeroberflächen zu sicheren, portablen und wiederverwendbaren NovaOS-Solutions zusammengestellt werden können.
