# NPSPEC-UI-EVENT-0001 – Nova UI Event System

**Status:** Angenommen  
**Bereich:** UI / Events  
**System:** NovaOS

## 1. Zweck

Nova UI Event System definiert die einheitliche Verarbeitung von Ereignissen innerhalb der nativen NovaOS-Benutzeroberfläche.

Ziel ist eine typsichere, deterministische und ereignisgesteuerte Kommunikation zwischen UI-Elementen, Anwendungslogik und Systemkomponenten.

Das Event System unterstützt direkte Ereignisse, Ereignisweiterleitung innerhalb der UI-Hierarchie sowie die Ereignisbindung über NovaLang und Logic Graph.

## 2. Architektur

Das Subsystem umfasst folgende Komponenten:

| Komponente | Aufgabe |
|---|---|
| Event Manager | Zentrale Verwaltung von UI-Ereignissen |
| Event Dispatcher | Zustellung und Verarbeitung von Ereignissen |
| Event Registry | Registrierung von Ereignistypen |
| Event Routing Engine | Weiterleitung innerhalb der UI-Hierarchie |
| Event Handler Manager | Verwaltung registrierter Ereignisbehandlungen |
| Event Context | Bereitstellung ereignisbezogener Informationen |
| Event Queue | Geordnete Verarbeitung anstehender Ereignisse |

Die Verarbeitung erfolgt innerhalb des Nova UI Threading Model.

## 3. Ereignismodell

Ein UI-Ereignis besitzt mindestens:

- Eindeutigen Ereignistyp.
- Ursprüngliche Ereignisquelle.
- Aktuelles Zielelement.
- Zeitliche oder sequenzielle Einordnung.
- Ereignisspezifische Daten.
- Definierte Weiterleitungsstrategie.
- Optionalen Verarbeitungsstatus.

Ereignisdaten MÜSSEN typsicher bereitgestellt werden.

Ein Ereignis DARF keine zusätzlichen Berechtigungen verleihen.

## 4. Ereignisarten

NovaOS unterscheidet folgende Ereigniskategorien:

| Kategorie | Beispiele |
|---|---|
| Pointer Events | PointerMoved, PointerPressed, PointerReleased |
| Keyboard Events | KeyDown, KeyUp |
| Focus Events | GotFocus, LostFocus |
| Control Events | Click, Checked, SelectionChanged |
| Property Events | PropertyChanged |
| Lifecycle Events | Loaded, Unloaded |
| Layout Events | SizeChanged, LayoutUpdated |
| Drag & Drop Events | DragEnter, DragOver, Drop |
| Command Events | CommandExecuting, CommandCompleted |
| Custom Events | Benutzerdefinierte Ereignisse |

Die konkreten Ereignistypen werden durch die jeweiligen Subsysteme definiert.

## 5. Event Routing

Nova UI Event System unterstützt drei Weiterleitungsarten.

| Routing | Verhalten |
|---|---|
| Direct | Zustellung ausschließlich an das Zielelement |
| Bubble | Weiterleitung vom Zielelement zu übergeordneten Elementen |
| Tunnel | Weiterleitung von übergeordneten Elementen zum Zielelement |

Die Weiterleitung erfolgt entlang einer definierten Ereignisroute innerhalb der UI-Hierarchie.

Dabei gilt:

- Die Route MUSS vor der Zustellung eindeutig bestimmt werden.
- Die Verarbeitungsreihenfolge MUSS deterministisch sein.
- Änderungen der UI-Hierarchie während einer Zustellung DÜRFEN die aktive Route nicht unkontrolliert verändern.
- Ereignisse DÜRFEN nicht unbeabsichtigt zwischen isolierten UI-Kontexten weitergeleitet werden.

## 6. Ereignisbehandlung

UI-Elemente können Ereignisbehandlungen registrieren.

Dabei gilt:

- Ein Ereignis kann mehrere Handler besitzen.
- Handler werden in einer definierten Reihenfolge aufgerufen.
- Handler können Ereignisdaten auswerten.
- Ereignisse können als verarbeitet markiert werden.
- Die Wirkung des Verarbeitungsstatus auf weitere Handler MUSS eindeutig definiert sein.

Ein als verarbeitet markiertes Ereignis wird standardmäßig nicht an gewöhnliche nachfolgende Handler zugestellt.

Handler können ausdrücklich für bereits verarbeitete Ereignisse registriert werden, sofern der Ereignistyp dies unterstützt.

## 7. NovaLang-Integration

NovaLang unterstützt die direkte Bindung von Ereignissen an Methoden.

Beispiel:

```vb
Private Sub Button1_Click() As Handle Button_1.Click

    SaveDocument()

End Sub
```

Mehrere Ereignisquellen können denselben Handler verwenden:

```vb
Private Sub Save_Click() As Handle Button_1.Click, Button_2.Click

    SaveDocument()

End Sub
```

Dynamische Ereignisbindungen werden unterstützt:

```vb
addHandle Button_1.Click To buttonClick()
```

Ereignisbindungen MÜSSEN typsicher geprüft werden.

Die konkrete Syntax und ihre Semantik werden durch NovaLang festgelegt.

## 8. Deklarative Ereignisbindung

Ereignisse können innerhalb des Nova Declarative UI Model mit Handlern verbunden werden.

Konzeptionelles Beispiel:

```xml
<Button
    Name="SaveButton"
    Content="Speichern"
    Click="SaveButton_Click" />
```

Die deklarative Bindung MUSS dieselben Ereignisregeln wie die programmatische Bindung verwenden.

Die Ausführung erfolgt über das Nova UI Event System.

## 9. Benutzerdefinierte Ereignisse

Custom Controls und Anwendungskomponenten können eigene Ereignisse definieren.

Dabei gilt:

- Ereignisse MÜSSEN registrierbar sein.
- Ereignisdaten MÜSSEN einen definierten Typ besitzen.
- Die Routing-Strategie MUSS festgelegt werden können.
- Handler MÜSSEN dynamisch registriert und entfernt werden können.
- Ereignisnamen MÜSSEN innerhalb ihres Gültigkeitsbereichs eindeutig auflösbar sein.

Benutzerdefinierte Ereignisse verwenden dieselbe Infrastruktur wie Standardereignisse.

## 10. Ereignisreihenfolge und Event Queue

Ereignisse werden innerhalb eines UI-Ausführungskontexts geordnet verarbeitet.

Dabei gilt:

- Die Reihenfolge eingereihter Ereignisse MUSS nachvollziehbar und deterministisch sein.
- Ereignisse werden über den zuständigen UI Dispatcher verarbeitet.
- Priorisierte Ereignisse DÜRFEN unterstützt werden.
- Prioritätsregeln MÜSSEN eindeutig definiert sein.
- Ereignisverarbeitung DARF die UI Event Loop nicht dauerhaft blockieren.

Unabhängige Ereignisse können zusammengefasst werden, sofern dadurch keine erforderliche Semantik verloren geht.

## 11. Synchrones und asynchrones Verhalten

Ereigniszustellung und asynchrone Folgeoperationen werden getrennt behandelt.

Dabei gilt:

- Die eigentliche Ereigniszustellung besitzt eine definierte Ausführungsreihenfolge.
- Handler können asynchrone Operationen starten.
- Asynchrone Operationen DÜRFEN die UI Event Loop nicht blockieren.
- Fehler asynchroner Operationen MÜSSEN kontrolliert verarbeitet werden.
- Der Abschluss einer asynchronen Operation DARF nicht unbeabsichtigt die ursprüngliche Ereignisroute verändern.

Asynchrone Aufgaben unterliegen den Regeln der NovaOS Structured Concurrency.

## 12. Event Propagation und Kontrolle

Ereignisse können kontrolliert weitergeleitet oder als verarbeitet markiert werden.

Dabei gilt:

- Die ursprüngliche Ereignisquelle bleibt nachvollziehbar.
- Das aktuell verarbeitende Element wird eindeutig bestimmt.
- Handler können den Verarbeitungsstatus verändern, sofern dies zulässig ist.
- Sicherheitsrelevante Systemereignisse DÜRFEN nicht allein durch gewöhnliche UI-Handler unterdrückt werden.
- Das Routing MUSS die Grenzen des jeweiligen UI-Kontexts einhalten.

## 13. Lebenszyklus

Ereignisbindungen unterliegen dem Nova UI Component Lifecycle.

Dabei gilt:

- Handler können während der Initialisierung registriert werden.
- Dynamische Handler können zur Laufzeit hinzugefügt oder entfernt werden.
- Nicht mehr benötigte Registrierungen MÜSSEN freigegeben werden.
- Entfernte UI-Elemente DÜRFEN nicht unbeabsichtigt durch Ereignisbindungen im Speicher verbleiben.
- Ereignisse für bereits entsorgte Elemente MÜSSEN kontrolliert verworfen werden.

## 14. Performance

Nova UI Event System arbeitet ereignisgesteuert.

Dabei gilt:

- Ereignisse werden ausschließlich bei tatsächlichem Bedarf erzeugt.
- Häufige Ereignisse wie PointerMoved DÜRFEN zusammengefasst werden.
- Ereignisrouten SOLLEN effizient ermittelt werden.
- Handler-Registrierungen SOLLEN ressourcenschonend verwaltet werden.
- Unveränderte UI-Zustände verursachen keine zusätzlichen Ereignisse.
- Ereignisse lösen nur erforderliche Layout- und Renderinvalidierungen aus.

Eine permanente Neuzeichnung aufgrund der Ereignisverarbeitung ist nicht zulässig.

## 15. NovaStudio und Logic Graph

NovaStudio unterstützt:

- Visuelle Auswahl verfügbarer Ereignisse.
- Erstellung von Event Handlern.
- Zuordnung mehrerer Ereignisse zu einem Handler.
- Dynamische Ereignisbindungen.
- Anzeige von Ereignisparametern.
- Debugging der Ereignisreihenfolge.
- Darstellung von Event Routing.
- Verbindung mit Logic Graph.

Im Logic Graph werden Ereignisse durch Event Nodes repräsentiert.

Ein Event Node stellt den Einstiegspunkt für einen ereignisgesteuerten Logikablauf dar.

NovaLang und Logic Graph MÜSSEN dieselben Ereignisdefinitionen verwenden.

## 16. Sicherheit

Nova UI Event System unterliegt Nova.Trust.

Dabei gilt:

- Ereignisse DÜRFEN keine zusätzlichen Capabilities verleihen.
- Ereignisquellen MÜSSEN innerhalb ihres Sicherheitskontexts identifizierbar sein.
- Ereignisdaten DÜRFEN keine geschützten Informationen unautorisiert offenlegen.
- Ereignisweiterleitung MUSS Sicherheitsgrenzen berücksichtigen.
- Nicht vertrauenswürdige Komponenten DÜRFEN keine privilegierten Systemereignisse vortäuschen.
- Fehlerhafte Handler DÜRFEN andere Sicherheitskontexte nicht beeinträchtigen.

Sicherheitsrelevante Operationen MÜSSEN unabhängig vom auslösenden Ereignis autorisiert werden.

## 17. Normative Anforderungen

- **UI-001:** NovaOS MUSS ein einheitliches natives UI Event System bereitstellen.
- **UI-002:** Ereignisse MÜSSEN typsicher definiert werden können.
- **UI-003:** Direct-, Bubble- und Tunnel-Ereignisse MÜSSEN unterstützt werden.
- **UI-004:** Ereignisrouten MÜSSEN deterministisch ermittelt werden.
- **UI-005:** Mehrere Handler pro Ereignis MÜSSEN unterstützt werden.
- **UI-006:** Ereignisse MÜSSEN als verarbeitet markiert werden können.
- **UI-007:** Handler MÜSSEN dynamisch registriert und entfernt werden können.
- **UI-008:** Benutzerdefinierte Ereignisse MÜSSEN unterstützt werden.
- **UI-009:** Ereignisse MÜSSEN deklarativ und programmatisch gebunden werden können.
- **UI-010:** NovaLang MUSS die definierte Ereignisbindung unterstützen.
- **UI-011:** Ereignisse MÜSSEN über den zuständigen UI-Ausführungskontext verarbeitet werden.
- **UI-012:** Die Ereignisverarbeitung MUSS eine definierte Reihenfolge besitzen.
- **UI-013:** Asynchrone Folgeoperationen DÜRFEN die UI Event Loop nicht blockieren.
- **UI-014:** Ereignisbindungen MÜSSEN kontrolliert freigegeben werden.
- **UI-015:** Häufige Ereignisse DÜRFEN unter Erhaltung ihrer Semantik zusammengefasst werden.
- **UI-016:** NovaStudio MUSS Ereignisse visuell bearbeiten und diagnostizieren können.
- **UI-017:** Logic Graph MUSS Ereignisse über Event Nodes verarbeiten können.
- **UI-018:** Das Event System MUSS unabhängig vom Renderbackend funktionieren.
- **UI-019:** Ereignisse MÜSSEN die Sicherheitsregeln von Nova.Trust einhalten.
- **UI-020:** Nova UI Event System MUSS vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 18. Integration

- **Nova UI Architecture:** Übergeordnete UI-Architektur.
- **Nova UI Runtime:** Verwaltung der Ereignisverarbeitung.
- **Nova UI Threading Model:** Event Loop und Dispatcher.
- **Nova UI Component Lifecycle:** Registrierung und Freigabe von Handlern.
- **Nova Declarative UI Model:** Deklarative Ereignisbindungen.
- **Nova Control Framework:** Control-Ereignisse.
- **Nova Control States:** Zustandsänderungen.
- **Nova Data Binding:** Ereignisabhängige Datenaktualisierung.
- **Nova Observable Properties:** Eigenschaftsänderungen.
- **Nova Commands and Actions:** Ausführung von Benutzeraktionen.
- **Nova Layout Engine:** Ereignisabhängige Layoutaktualisierung.
- **Nova UI Renderer Abstraction:** Ereignisabhängige Darstellung.
- **Nova Input System:** Eingabeereignisse.
- **Nova Window Manager:** Fensterbezogene Ereignisse.
- **NovaLang:** Programmatische Ereignisbehandlung.
- **NovaStudio:** Event Editor und Debugging.
- **Nova Logic Graph:** Ereignisgesteuerte Logikabläufe.
- **Nova.Trust:** Sicherheit und Isolation.

## 19. Ergebnis

NovaOS erhält ein einheitliches, typsicheres und deterministisches System zur Verarbeitung nativer UI-Ereignisse.

Ereignisse können direkt oder entlang der UI-Hierarchie verarbeitet und sowohl mit NovaLang als auch mit Logic Graph verbunden werden.

Durch kontrolliertes Routing, definierte Ausführungsreihenfolgen und gezielte Aktualisierungen bleibt die Benutzeroberfläche reaktionsschnell und ressourcenschonend.

**Grundsatz:** Ein Ereignis beschreibt, was geschehen ist. Das Event System bestimmt, wer davon erfährt und in welcher Reihenfolge darauf reagiert werden kann.