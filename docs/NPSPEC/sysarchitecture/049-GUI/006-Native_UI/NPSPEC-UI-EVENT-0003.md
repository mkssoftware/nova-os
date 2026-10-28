# NPSPEC-UI-EVENT-0003 – Nova Event Handlers

**Status:** Angenommen  
**Bereich:** UI / Events / Event Handlers  
**System:** NovaOS

## 1. Zweck

Nova Event Handlers definiert die Registrierung, Ausführung und Verwaltung von Ereignisbehandlungen innerhalb der nativen NovaOS-Benutzeroberfläche.

Ziel ist eine einheitliche, typsichere und deterministische Ereignisbehandlung für NovaLang, deklarative UI-Definitionen und Logic Graph.

Event Handler sind unabhängig von der Ereignisquelle und dem verwendeten Renderbackend.

## 2. Architektur

Das Subsystem umfasst folgende Komponenten:

| Komponente | Aufgabe |
|---|---|
| Event Handler Registry | Verwaltung registrierter Handler |
| Event Handler Resolver | Auflösung der zuständigen Ereignisbehandlung |
| Event Handler Dispatcher | Geordnete Ausführung von Handlern |
| Event Subscription Manager | Verwaltung dynamischer Ereignisbindungen |
| Event Handler Context | Bereitstellung von Ereignisparametern |
| Event Handler Lifecycle Manager | Aktivierung und Freigabe von Handlern |

Die Ereigniszustellung erfolgt über Nova UI Event System.

## 3. Handler-Modell

Ein Event Handler besteht aus:

- Eindeutiger Handler-Identität.
- Zugeordnetem Ereignistyp.
- Ausführbarer Methode oder Logic-Graph-Einstiegspunkt.
- Definierter Parametersignatur.
- Optionalem Ausführungskontext.
- Registrierungsinformationen.

Ein Handler kann mit mehreren Ereignisquellen verbunden werden.

Eine Ereignisquelle kann mehrere Handler besitzen.

Die Ausführung MUSS die Signatur des jeweiligen Ereignistyps berücksichtigen.

## 4. Statische Ereignisbindung

NovaLang unterstützt die direkte Zuordnung von Ereignissen über `As Handle`.

Beispiel:

```vb
Private Sub Button1_Click() As Handle Button_1.Click

    SaveDocument()

End Sub
```

Mehrere Ereignisse können denselben Handler verwenden:

```vb
Private Sub Button_Click() As Handle Button_1.Click, Button_2.Click, Button_3.Click

    SaveDocument()

End Sub
```

Dabei gilt:

- Ereignisquellen MÜSSEN eindeutig auflösbar sein.
- Handler-Signaturen MÜSSEN geprüft werden.
- Die Registrierung erfolgt automatisch während der Initialisierung.
- Mehrfachbindungen MÜSSEN deterministisch verarbeitet werden.

## 5. Dynamische Ereignisbindung

Event Handler können zur Laufzeit registriert werden.

Beispiel:

```vb
addHandle Button_1.Click To buttonClick()
```

Dynamische Bindungen MÜSSEN auch wieder entfernt werden können.

Konzeptionelles Beispiel:

```vb
removeHandle Button_1.Click From buttonClick()
```

Die endgültige Syntax wird durch NovaLang festgelegt.

Dabei gilt:

- Registrierungen MÜSSEN eindeutig identifizierbar sein.
- Mehrfachregistrierungen MÜSSEN kontrolliert behandelt werden.
- Entfernte Handler DÜRFEN keine weiteren Ereignisse empfangen.
- Dynamische Bindungen unterliegen dem Lebenszyklus ihrer Komponenten.

## 6. Ereignisparameter

Event Handler können ereignisspezifische Parameter empfangen.

Beispiel:

```vb
Private Sub Button_Click(
    sender As Object,
    e As RoutedEventArgs
) As Handle Button_1.Click

    If e.Handled Then
        Return
    End If

    SaveDocument()

End Sub
```

Dabei gilt:

- Ereignisparameter MÜSSEN typsicher sein.
- Die Ereignisquelle MUSS ermittelbar sein.
- Ereignisspezifische Daten werden über definierte Argumenttypen bereitgestellt.
- Handler ohne explizite Parameter sind zulässig, sofern die Ereignisbindung dies unterstützt.

## 7. Handler-Ausführung

Event Handler werden über den zuständigen Event Dispatcher ausgeführt.

Dabei gilt:

- Die Ausführungsreihenfolge MUSS deterministisch sein.
- Handler werden innerhalb ihres vorgesehenen Ausführungskontexts aufgerufen.
- Ein Handler DARF die UI Event Loop nicht dauerhaft blockieren.
- Fehler MÜSSEN kontrolliert behandelt werden.
- Ein fehlerhafter Handler DARF nicht automatisch den gesamten UI-Kontext zum Absturz bringen.

Die Reihenfolge mehrerer Handler wird durch das Nova UI Event System festgelegt.

## 8. Routed Event Handlers

Event Handler können an Routed Events gebunden werden.

Dabei gilt:

- Handler können Direct-, Tunnel- und Bubble-Ereignisse behandeln.
- `OriginalSource` und `CurrentTarget` MÜSSEN verfügbar sein.
- Der Verarbeitungsstatus `Handled` MUSS berücksichtigt werden.
- Handler können ausdrücklich für bereits verarbeitete Ereignisse registriert werden.
- Die Ereignisroute bleibt unabhängig von der Handler-Implementierung.

Die Routing-Semantik wird durch Nova Routed Events definiert.

## 9. Asynchrone Event Handler

Event Handler können asynchrone Operationen auslösen.

Konzeptionelles Beispiel:

```vb
Private Async Sub LoadButton_Click() As Handle LoadButton.Click

    Await LoadDocumentAsync()

End Sub
```

Dabei gilt:

- Asynchrone Operationen DÜRFEN die UI Event Loop nicht blockieren.
- Fehler MÜSSEN an eine definierte Fehlerbehandlung weitergegeben werden.
- Die Lebensdauer asynchroner Aufgaben MUSS verwaltet werden.
- Nicht mehr benötigte Aufgaben MÜSSEN kontrolliert beendet oder entkoppelt werden können.
- Asynchrone Ergebnisse DÜRFEN keine bereits entsorgten UI-Elemente unkontrolliert verändern.

Die Ereigniszustellung wartet nicht automatisch auf den Abschluss asynchroner Folgeoperationen.

## 10. Handler und Commands

Event Handler können Commands auslösen.

Dabei gilt:

- Event Handler reagieren auf eingetretene Ereignisse.
- Commands beschreiben ausführbare Benutzerabsichten.
- Wiederverwendbare Benutzeraktionen SOLLEN bevorzugt als Commands implementiert werden.
- Ein Handler DARF mehrere Commands oder Actions koordinieren.
- Die Ausführung MUSS die Berechtigungen des jeweiligen Commands berücksichtigen.

Die Verwendung von Commands ist nicht für jeden Event Handler verpflichtend.

## 11. Logic-Graph-Integration

Event Handler können durch Logic Graph implementiert werden.

Dabei gilt:

- Event Nodes bilden den Einstiegspunkt.
- Ereignisparameter stehen als typisierte Ausgänge zur Verfügung.
- Der Ausführungsfluss beginnt beim Eintreten des Ereignisses.
- Mehrere Ereignisquellen können denselben Logikablauf verwenden.
- Die Ausführung verwendet dieselben Ereignisregeln wie NovaLang.

NovaStudio MUSS die Zuordnung zwischen Ereignis, Handler und Logic Graph darstellen können.

## 12. Lebenszyklus

Event Handler unterliegen dem Nova UI Component Lifecycle.

Dabei gilt:

- Statische Handler werden bei der Initialisierung registriert.
- Dynamische Handler können während der Laufzeit hinzugefügt werden.
- Nicht mehr benötigte Handler werden entfernt.
- Ereignisregistrierungen MÜSSEN beim Entsorgen eines UI-Elements freigegeben werden.
- Registrierungen DÜRFEN entfernte UI-Elemente nicht unbeabsichtigt im Speicher halten.

Schwache Referenzen DÜRFEN verwendet werden, sofern ihre Lebensdauersemantik eindeutig definiert ist.

## 13. Performance

Nova Event Handlers arbeitet ereignisgesteuert.

Dabei gilt:

- Handler werden ausschließlich bei relevanten Ereignissen ausgeführt.
- Handler-Auflösungen SOLLEN effizient verwaltet werden.
- Unveränderte Registrierungen SOLLEN wiederverwendet werden.
- Nicht mehr benötigte Registrierungen werden freigegeben.
- Ereignisbehandlungen lösen nur erforderliche Zustands-, Layout- oder Renderaktualisierungen aus.

Ein Event Handler DARF keine permanente UI-Neuzeichnung voraussetzen.

## 14. NovaStudio-Integration

NovaStudio unterstützt:

- Erstellung von Event Handlern.
- Auswahl verfügbarer Ereignisse.
- Statische und dynamische Ereignisbindungen.
- Zuordnung mehrerer Ereignisse zu einem Handler.
- Automatische Prüfung von Handler-Signaturen.
- Navigation zwischen UI-Designer, NovaLang und Logic Graph.
- Debugging von Ereignisbehandlungen.
- Anzeige aktiver Ereignisregistrierungen.

Die Darstellung eines Handlers in NovaLang oder Logic Graph MUSS dieselbe funktionale Semantik besitzen.

## 15. Sicherheit

Nova Event Handlers unterliegt Nova.Trust.

Dabei gilt:

- Handler DÜRFEN keine zusätzlichen Capabilities erhalten.
- Ereignisparameter MÜSSEN entsprechend ihrer Zugriffsrechte bereitgestellt werden.
- Handler MÜSSEN innerhalb ihres autorisierten Ausführungskontexts arbeiten.
- Dynamische Registrierungen DÜRFEN keine Sicherheitsgrenzen umgehen.
- Fehlerhafte Handler MÜSSEN kontrolliert isoliert werden.
- Sicherheitsrelevante Operationen MÜSSEN unabhängig vom auslösenden Ereignis autorisiert werden.

## 16. Normative Anforderungen

- **UI-001:** NovaOS MUSS ein einheitliches Event-Handler-System bereitstellen.
- **UI-002:** Event Handler MÜSSEN typsicher definiert werden können.
- **UI-003:** Statische Ereignisbindungen über NovaLang MÜSSEN unterstützt werden.
- **UI-004:** Ein Handler MUSS mehrere Ereignisquellen behandeln können.
- **UI-005:** Eine Ereignisquelle MUSS mehrere Handler unterstützen.
- **UI-006:** Dynamische Handler-Registrierungen MÜSSEN unterstützt werden.
- **UI-007:** Dynamische Handler MÜSSEN wieder entfernt werden können.
- **UI-008:** Handler-Signaturen MÜSSEN geprüft werden.
- **UI-009:** Ereignisparameter MÜSSEN standardisiert bereitgestellt werden.
- **UI-010:** Die Handler-Ausführung MUSS deterministisch erfolgen.
- **UI-011:** Routed Event Handler MÜSSEN unterstützt werden.
- **UI-012:** Der Verarbeitungsstatus von Routed Events MUSS berücksichtigt werden.
- **UI-013:** Asynchrone Event Handler MÜSSEN unterstützt werden.
- **UI-014:** Asynchrone Handler DÜRFEN die UI Event Loop nicht blockieren.
- **UI-015:** Handler-Fehler MÜSSEN kontrolliert verarbeitet werden.
- **UI-016:** Ereignisregistrierungen MÜSSEN kontrolliert freigegeben werden.
- **UI-017:** Event Handler MÜSSEN Commands auslösen können.
- **UI-018:** Logic Graph MUSS Event Handler implementieren können.
- **UI-019:** NovaStudio MUSS Event Handler erstellen und diagnostizieren können.
- **UI-020:** Event Handler MÜSSEN unabhängig vom Renderbackend funktionieren.
- **UI-021:** Event Handler MÜSSEN die Sicherheitsregeln von Nova.Trust einhalten.
- **UI-022:** Nova Event Handlers MUSS vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 17. Integration

- **Nova UI Event System:** Ereignisregistrierung und Zustellung.
- **Nova Routed Events:** Weiterleitung und Verarbeitungsstatus.
- **Nova UI Runtime:** Verwaltung der Handler-Ausführung.
- **Nova UI Threading Model:** Dispatcher und asynchrone Verarbeitung.
- **Nova UI Component Lifecycle:** Registrierung und Freigabe.
- **Nova Declarative UI Model:** Deklarative Ereignisbindungen.
- **Nova Control Framework:** Control-spezifische Ereignisse.
- **Nova Commands and Actions:** Ausführung von Benutzeraktionen.
- **Nova Observable Properties:** Eigenschaftsänderungen.
- **NovaLang:** Programmatische Ereignisbehandlung.
- **NovaStudio:** Handler-Editor und Debugging.
- **Nova Logic Graph:** Visuelle Ereignisbehandlung.
- **Nova.Trust:** Berechtigungen und Isolation.

## 18. Ergebnis

NovaOS erhält ein einheitliches und typsicheres System zur Definition, Registrierung und Ausführung von Event Handlern.

Ereignisse können statisch oder dynamisch mit NovaLang-Methoden und Logic-Graph-Abläufen verbunden werden.

Durch deterministische Ausführung, kontrollierte Ressourcenverwaltung und asynchrone Verarbeitung bleibt die Ereignisbehandlung flexibel und ressourcenschonend.

**Grundsatz:** Ein Event Handler bestimmt, wie auf ein Ereignis reagiert wird – unabhängig davon, welches UI-Element es ausgelöst hat und ob die Logik in NovaLang oder im Logic Graph implementiert wurde.