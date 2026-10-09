
# NPSPEC-STUDIO-VARIABLE-INSPECTOR-0001 – NovaLang Studio Variable Inspector

## Status

Angenommen

## Kategorie

NovaLang Studio / Debugging / Variableninspektion

## Zweck

Definiert die Untersuchung von Variablen, Objekten und Datenstrukturen während einer Debug-Sitzung in NovaLang Studio.

Ziel ist eine schnelle, übersichtliche und sichere Darstellung des aktuellen Programmzustands mit direkter Integration in Code Editor, Debugger und Logic Graph.

## Architektur

| Komponente | Aufgabe |
|---|---|
| Variable Inspector | Zentrale Anzeige und Steuerung |
| Scope Resolver | Ermittlung sichtbarer Variablen |
| Value Provider | Abruf aktueller Laufzeitwerte |
| Type Inspector | Darstellung von Datentypen |
| Object Explorer | Untersuchung verschachtelter Objekte |
| Value Formatter | Formatierung von Werten |
| Change Tracker | Erkennung von Wertänderungen |
| Watch Integration | Einbindung benutzerdefinierter Ausdrücke |

Die Implementierung verwendet das gemeinsame Debug-Protokoll aus `NPSPEC-NOVALANG-DEBUGGING-0001`.

## Unterstützte Daten

Der Variable Inspector unterstützt:

- Lokale Variablen
- Funktionsparameter
- Objektfelder
- Eigenschaften
- Konstanten
- Arrays und Collections
- Strukturen und Klassen
- Generische Typen
- Nullable-Werte
- Task-lokale Zustände
- Logic-Graph-Eingaben und -Ausgaben

## Variablenmodell

Jeder angezeigte Eintrag besitzt mindestens:

- Name
- Datentyp
- Aktuellen Wert oder Verfügbarkeitsstatus
- Zugehörigen Gültigkeitsbereich
- Referenz auf den aktuellen Debug-Kontext

Optional werden bereitgestellt:

- Objektidentität
- Änderungsstatus
- Speicherinformationen
- Zugriffsbeschränkungen
- Erweiterbare Unterelemente

Variablenreferenzen dürfen nur innerhalb ihrer gültigen Debug-Sitzung und Ausführungspause verwendet werden.

## Variablenansicht

Die Darstellung erfolgt hierarchisch.

Beispiel:

```text
Locals
├── person As Benutzer
│   ├── Name As String = "Max"
│   ├── Alter As Integer = 28
│   └── Aktiv As Boolean = True
├── ergebnis As Integer = 150
└── werte As List(Of Integer)
    ├── Count = 3
    ├── [0] = 10
    ├── [1] = 20
    └── [2] = 30
```

Verschachtelte Daten werden erst beim Aufklappen geladen.

## Gültigkeitsbereiche

Der Scope Resolver unterscheidet:

- Locals
- Arguments
- Instance Members
- Shared Members
- Captured Variables
- Async State
- Graph Context

Die angezeigten Variablen beziehen sich auf den ausgewählten Stack Frame beziehungsweise Graph-Ausführungskontext.

## Wertdarstellung

Der Value Formatter unterstützt:

- Dezimal-, Hexadezimal- und Binärdarstellung
- Zeichenketten und Unicode
- Boolesche Werte
- Datums- und Zeitwerte
- Arrays und Collections
- Objektreferenzen
- `Nothing`
- Nullable-Typen

Große Werte müssen gekürzt dargestellt und bei Bedarf vollständig abrufbar sein.

## Objektinspektion

Objekte können rekursiv untersucht werden.

Dabei gilt:

- Zyklische Referenzen müssen erkannt werden.
- Wiederholte Objekte sollen über ihre Identität erkennbar bleiben.
- Große Collections werden seitenweise geladen.
- Maximale Inspektionstiefe und Ergebnisanzahl sind begrenzbar.
- Die Garbage Collection darf inspizierte Objekte nicht unbeabsichtigt ungültig machen.

Objekte dürfen durch die reine Inspektion nicht verändert werden.

## Eigenschaften und Seiteneffekte

Das Auslesen einer Eigenschaft kann Programmcode ausführen.

Deshalb gilt:

- Felder und bereits verfügbare Werte dürfen direkt angezeigt werden.
- Eigenschaften mit Getter werden standardmäßig nicht automatisch ausgeführt.
- Potenziell nebenwirkungsbehaftete Auswertungen benötigen eine ausdrückliche Benutzeraktion.
- Nicht auswertbare Eigenschaften werden entsprechend gekennzeichnet.

## Änderungsverfolgung

Der Change Tracker kann Werte zwischen zwei gültigen Ausführungspausen vergleichen.

Geänderte Werte werden dezent hervorgehoben.

Unterschieden werden:

- Wert geändert
- Variable hinzugekommen
- Variable nicht mehr verfügbar
- Objektzustand geändert
- Vergleich nicht möglich

Die Änderungsverfolgung darf keine vollständigen Speicherabbilder voraussetzen.

## Werte bearbeiten

Der Debugger kann das kontrollierte Ändern von Variablen unterstützen.

Voraussetzungen:

- Ausführung ist angehalten.
- Zielwert ist beschreibbar.
- Neuer Wert ist typkompatibel.
- Debug-Sitzung besitzt entsprechende Berechtigungen.
- Runtime unterstützt die Änderung.

Änderungen müssen ausdrücklich bestätigt und diagnostizierbar sein.

Die Bearbeitung darf keine Speicher- oder Capability-Grenzen umgehen.

## Watch-Integration

Variablen können direkt in die Watch-Ansicht übernommen werden.

Watch-Ausdrücke verwenden denselben Auswertungskontext und dieselben Sicherheitsregeln.

Standardmäßig dürfen sie keine Seiteneffekte auslösen.

## Logic-Graph-Integration

Bei Solutions zeigt der Variable Inspector:

- Eingabewerte eines Graph-Knotens
- Ausgabewerte eines Graph-Knotens
- Lokale Variablen von Custom Scripts
- Übergebene Capability-Handles
- Aktuelle Datenstrukturen

Bei parallelen Ausführungspfaden muss der jeweilige Graph-Kontext eindeutig erkennbar sein.

Capability-Handles dürfen nur über ihre autorisierten Diagnoseinformationen untersucht werden.

## AOT, JIT und Interpreter

| Modus | Variableninspektion |
|---|---|
| Interpreter | Direkter Zugriff auf Runtime-Werte |
| JIT | Zugriff über Debug-Metadaten und Runtime |
| AOT | Zugriff über native Debug-Informationen |

Optimierungsbedingt entfernte oder nicht rekonstruierbare Variablen müssen als nicht verfügbar gekennzeichnet werden.

## Benutzeroberfläche

Der Variable Inspector verwendet die NovaOS-Designsprache.

- Hierarchische Baumansicht
- Spalten für Name, Wert und Typ
- Such- und Filterfunktion
- Dezente Hervorhebung geänderter Werte
- Kontextmenü für Watch und Kopieren
- Anpassbare Zahlenformate
- Frei anordenbares Debug-Panel

Die Darstellung muss auch bei umfangreichen Objekten übersichtlich bleiben.

## Performance

- Werte werden bedarfsgerecht geladen.
- Große Collections werden virtualisiert.
- Nicht sichtbare Objekte werden nicht unnötig ausgewertet.
- Inspektionsanfragen sind abbrechbar.
- Datenmengen, Rekursionstiefe und Antwortzeiten sind begrenzbar.
- Veraltete Anfragen werden verworfen.
- Die Benutzeroberfläche darf nicht durch langsame Inspektionen blockieren.

## Sicherheit

- Variableninspektion benötigt eine autorisierte Debug-Sitzung.
- Geschützte Werte müssen maskiert oder ausgeblendet werden.
- Speicherisolation und Capability-Grenzen bleiben wirksam.
- Getter dürfen nicht unkontrolliert ausgeführt werden.
- Objektinspektion darf keine zusätzlichen Berechtigungen erzeugen.
- Änderungen an Laufzeitwerten benötigen eine gesonderte Autorisierung.
- Sensible Daten dürfen nicht unautorisiert in Logs oder Diagnoseberichte gelangen.

## Normative Anforderungen

1. NovaLang Studio MUSS einen integrierten Variable Inspector bereitstellen.
2. Lokale Variablen, Parameter, Felder und Objekte MÜSSEN inspizierbar sein.
3. Datentypen, Werte und Gültigkeitsbereiche MÜSSEN angezeigt werden.
4. Verschachtelte Objekte und Collections MÜSSEN bedarfsgerecht geladen werden.
5. Zyklische Objektreferenzen MÜSSEN erkannt werden.
6. Die Inspektion DARF standardmäßig keine Seiteneffekte auslösen.
7. Optimierungsbedingt nicht verfügbare Werte MÜSSEN eindeutig gekennzeichnet werden.
8. Variablen MÜSSEN dem ausgewählten Stack Frame oder Graph-Kontext zugeordnet werden.
9. Wertänderungen SOLLEN zwischen Ausführungspausen hervorgehoben werden.
10. Kontrollierte Wertänderungen DÜRFEN nur bei entsprechender Autorisierung erfolgen.
11. AOT, JIT und Interpreter MÜSSEN ein gemeinsames Inspektionsmodell verwenden.
12. Logic-Graph-Eingaben und -Ausgaben MÜSSEN untersucht werden können.
13. Ressourcenverbrauch und Inspektionstiefe MÜSSEN begrenzbar sein.
14. Capability-, Datenschutz- und Speichergrenzen MÜSSEN eingehalten werden.
15. Der Variable Inspector MUSS ohne KI vollständig funktionieren.

## Ergebnis

NovaLang Studio erhält einen sicheren, leistungsfähigen und ressourcenschonenden Variable Inspector zur Untersuchung von Variablen, Objekten und Datenflüssen.

Entwickler können den aktuellen Programmzustand unmittelbar nachvollziehen, Veränderungen erkennen und Fehler gezielt analysieren.
