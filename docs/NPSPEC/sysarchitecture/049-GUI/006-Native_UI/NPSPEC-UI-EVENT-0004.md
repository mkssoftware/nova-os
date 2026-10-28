# NPSPEC-UI-EVENT-0004 – Nova Event-to-Command Binding

**Status:** Angenommen  
**Bereich:** UI / Events / Command Binding  
**System:** NovaOS

## 1. Zweck

Nova Event-to-Command Binding definiert die deklarative und programmatische Verbindung zwischen UI-Ereignissen und Commands.

Ziel ist es, beliebige UI-Ereignisse direkt mit ausführbaren Commands zu verbinden, ohne dafür eigene Event Handler implementieren zu müssen.

Das System verbindet Nova UI Event System mit Nova Commands and Actions und unterstützt NovaLang sowie Logic Graph.

## 2. Architektur

Das Subsystem umfasst folgende Komponenten:

| Komponente | Aufgabe |
|---|---|
| Event Command Binding | Verbindung zwischen Ereignis und Command |
| Event Command Resolver | Auflösung des auszuführenden Commands |
| Event Parameter Mapper | Übertragung und Konvertierung von Ereignisdaten |
| Event Command Dispatcher | Weiterleitung der Ausführungsanforderung |
| Command Availability Observer | Überwachung der Command-Verfügbarkeit |
| Event Command Lifecycle Manager | Verwaltung und Freigabe der Bindungen |

Die eigentliche Command-Ausführung verbleibt bei Nova Commands and Actions.

## 3. Binding-Modell

Ein Event-to-Command Binding besteht aus:

- Ereignisquelle.
- Ereignistyp.
- Command-Referenz.
- Optionalem Command-Parameter.
- Optionaler Parameterkonvertierung.
- Optionaler Ausführungsbedingung.
- Optionaler Routing-Konfiguration.

Ein Ereignis kann mehrere Commands auslösen.

Ein Command kann mit mehreren Ereignissen verbunden werden.

Die Ausführungsreihenfolge MUSS deterministisch sein.

## 4. Deklarative Ereignisbindung

Commands können direkt mit UI-Ereignissen verbunden werden.

Konzeptionelles Beispiel:

```xml
<TextBox Name="SearchBox">

    <TextBox.EventBindings>

        <EventBinding
            Event="TextChanged"
            Command="{Binding SearchCommand}"
            CommandParameter="{Binding Text, ElementName=SearchBox}" />

    </TextBox.EventBindings>

</TextBox>
```

Beim Auftreten des Ereignisses wird der zugeordnete Command ausgeführt, sofern seine Ausführung zulässig ist.

Die konkrete XML-Syntax wird durch das Nova Declarative UI Model festgelegt.

## 5. Mehrere Ereignisbindungen

Ein Control kann mehrere Event-to-Command Bindings besitzen.

Beispiel:

```xml
<TextBox>

    <TextBox.EventBindings>

        <EventBinding
            Event="TextChanged"
            Command="{Binding ValidateCommand}" />

        <EventBinding
            Event="LostFocus"
            Command="{Binding SaveCommand}" />

    </TextBox.EventBindings>

</TextBox>
```

Mehrere Bindungen desselben Ereignisses MÜSSEN in einer definierten Reihenfolge verarbeitet werden.

## 6. Ereignisparameter

Event-to-Command Bindings können Ereignisdaten an Commands übergeben.

Unterstützt werden:

- Explizite Parameterwerte.
- Eigenschaften der Ereignisquelle.
- Eigenschaften des DataContext.
- Ereignisargumente.
- Einzelne Eigenschaften der Ereignisargumente.
- Konvertierte Parameter.

Beispiel:

```xml
<ListView>

    <ListView.EventBindings>

        <EventBinding
            Event="SelectionChanged"
            Command="{Binding SelectionCommand}"
            EventParameterPath="SelectedItem" />

    </ListView.EventBindings>

</ListView>
```

Die Parameterübertragung MUSS typsicher erfolgen.

Ungültige Parameter MÜSSEN kontrolliert behandelt werden.

## 7. Command-Verfügbarkeit

Vor der Ausführung wird die Verfügbarkeit des Commands geprüft.

Dabei gilt:

- Nicht verfügbare Commands werden nicht ausgeführt.
- Command-Parameter werden bei der Verfügbarkeitsprüfung berücksichtigt.
- Änderungen der Command-Verfügbarkeit können beobachtet werden.
- Die Verfügbarkeit DARF nicht automatisch den funktionalen Zustand der Ereignisquelle verändern.

Ein Event-to-Command Binding DARF keine Sicherheitsprüfung ersetzen.

## 8. Routed Events

Event-to-Command Binding unterstützt Nova Routed Events.

Dabei gilt:

- Direct-, Tunnel- und Bubble-Ereignisse können gebunden werden.
- Die Ereignisquelle bleibt nachvollziehbar.
- Der Verarbeitungsstatus `Handled` wird berücksichtigt.
- Bereits verarbeitete Ereignisse können bei ausdrücklicher Konfiguration berücksichtigt werden.
- Das Binding DARF die bestehende Ereignisroute nicht verändern.

Ob ein Ereignis nach der Command-Auslösung als verarbeitet markiert wird, MUSS ausdrücklich konfigurierbar sein.

## 9. Ausführungsbedingungen

Event-to-Command Bindings können zusätzliche Bedingungen besitzen.

Beispiele:

- Command nur bei gedrückter Enter-Taste ausführen.
- Command nur bei bestimmtem Auswahlzustand ausführen.
- Command nur bei gültigen Eingabedaten ausführen.

Dabei gilt:

- Bedingungen MÜSSEN deterministisch ausgewertet werden.
- Bedingungen DÜRFEN keine Berechtigungen erweitern.
- Die Command-Verfügbarkeitsprüfung bleibt verpflichtend.

## 10. Asynchrone Commands

Event-to-Command Bindings unterstützen asynchrone Commands.

Dabei gilt:

- Die Ereigniszustellung DARF nicht durch lang laufende Commands blockiert werden.
- Mehrfachausführungen werden gemäß der Command-Konfiguration behandelt.
- Fehler werden über die Command-Fehlerbehandlung bereitgestellt.
- Abbruch und Fortschritt werden durch den Command verwaltet.
- Veraltete Ausführungsergebnisse DÜRFEN aktuelle Zustände nicht unkontrolliert überschreiben.

Die Lebensdauer asynchroner Operationen unterliegt Nova Commands and Actions und der Structured Concurrency.

## 11. Ereignisfrequenz

Häufig auftretende Ereignisse können besondere Ausführungsregeln benötigen.

Unterstützt werden können:

- **Immediate:** Sofortige Ausführungsanforderung.
- **Debounce:** Ausführung nach einer definierten Ruhezeit.
- **Throttle:** Begrenzung der Ausführungshäufigkeit.
- **Latest:** Bevorzugung der neuesten Anforderung.

Diese Mechanismen MÜSSEN optional und konfigurierbar sein.

Ereignisse mit erforderlicher Einzelverarbeitung DÜRFEN nicht unbeabsichtigt verworfen werden.

## 12. NovaLang-Integration

Event-to-Command Bindings können programmatisch erstellt werden.

Konzeptionelles Beispiel:

```vb
BindEventToCommand(
    SearchBox.TextChanged,
    ViewModel.SearchCommand
)
```

NovaLang MUSS dieselben Binding-Regeln wie das deklarative UI-Modell verwenden.

Die konkrete API-Syntax wird durch NovaLang und das Nova Framework festgelegt.

## 13. Logic-Graph-Integration

Nova Logic Graph unterstützt Event-to-Command Bindings.

Dabei gilt:

- Event Nodes können Commands direkt auslösen.
- Ereignisparameter können an Command-Eingänge gebunden werden.
- Verfügbarkeitsbedingungen können visuell dargestellt werden.
- Die Command-Ausführung verwendet die reguläre Command-Infrastruktur.

Ein Event-to-Command Binding DARF keine zusätzliche, vom Command Framework unabhängige Ausführungslogik erzeugen.

## 14. Lebenszyklus

Event-to-Command Bindings unterliegen dem Nova UI Component Lifecycle.

Dabei gilt:

- Bindungen werden bei der Initialisierung aktiviert.
- Änderungen des DataContext werden berücksichtigt.
- Command-Referenzen können dynamisch aktualisiert werden.
- Nicht mehr benötigte Ereignisregistrierungen werden freigegeben.
- Entfernte UI-Elemente DÜRFEN nicht unbeabsichtigt im Speicher verbleiben.

## 15. Performance

Nova Event-to-Command Binding arbeitet ereignisgesteuert.

Dabei gilt:

- Commands werden ausschließlich bei relevanten Ereignissen angefordert.
- Unveränderte Bindungen SOLLEN wiederverwendet werden.
- Parameterauflösungen SOLLEN effizient verarbeitet werden.
- Häufige Ereignisse können kontrolliert zusammengefasst werden.
- Nicht benötigte Bindungen werden freigegeben.

Event-to-Command Bindings DÜRFEN keine permanente UI-Neuzeichnung verursachen.

## 16. NovaStudio-Integration

NovaStudio unterstützt:

- Visuelle Zuordnung von Ereignissen zu Commands.
- Auswahl verfügbarer Ereignisse.
- Auswahl registrierter Commands.
- Konfiguration von Command-Parametern.
- Definition von Ausführungsbedingungen.
- Einstellung von Debounce und Throttle.
- Darstellung von Routed-Event-Einstellungen.
- Debugging von Event-to-Command Bindings.
- Verbindung mit Logic Graph.

Designer und Runtime MÜSSEN dieselben Binding-Definitionen verwenden.

## 17. Sicherheit

Nova Event-to-Command Binding unterliegt Nova.Trust.

Dabei gilt:

- Ereignisse DÜRFEN keine zusätzlichen Capabilities verleihen.
- Command-Parameter MÜSSEN validiert werden.
- Commands MÜSSEN innerhalb ihres autorisierten Kontexts ausgeführt werden.
- Ereignisbindungen DÜRFEN keine Sicherheitsgrenzen umgehen.
- Nicht vertrauenswürdige Ereignisquellen DÜRFEN keine privilegierten Operationen autorisieren.
- Sicherheitsrelevante Commands MÜSSEN ihre Berechtigungen vor der Ausführung prüfen.

## 18. Normative Anforderungen

- **UI-001:** NovaOS MUSS Event-to-Command Bindings unterstützen.
- **UI-002:** Beliebige registrierte UI-Ereignisse MÜSSEN mit kompatiblen Commands verbunden werden können.
- **UI-003:** Bindungen MÜSSEN deklarativ und programmatisch definiert werden können.
- **UI-004:** Ein Ereignis MUSS mehrere Commands auslösen können.
- **UI-005:** Ein Command MUSS mit mehreren Ereignissen verbunden werden können.
- **UI-006:** Ereignisparameter MÜSSEN an Commands übergeben werden können.
- **UI-007:** Parameterübertragungen MÜSSEN typsicher erfolgen.
- **UI-008:** Die Command-Verfügbarkeit MUSS vor der Ausführung berücksichtigt werden.
- **UI-009:** Routed Events MÜSSEN unterstützt werden.
- **UI-010:** Der Ereignisstatus `Handled` MUSS berücksichtigt werden.
- **UI-011:** Ausführungsbedingungen MÜSSEN unterstützt werden können.
- **UI-012:** Asynchrone Commands MÜSSEN unterstützt werden.
- **UI-013:** Ereignisfrequenzbegrenzungen MÜSSEN optional konfigurierbar sein.
- **UI-014:** Die Ausführungsreihenfolge MUSS deterministisch sein.
- **UI-015:** Änderungen des DataContext MÜSSEN berücksichtigt werden.
- **UI-016:** Ereignisregistrierungen MÜSSEN kontrolliert freigegeben werden.
- **UI-017:** NovaLang MUSS Event-to-Command Bindings verwenden können.
- **UI-018:** Logic Graph MUSS Event-to-Command Bindings unterstützen.
- **UI-019:** NovaStudio MUSS Event-to-Command Bindings visuell bearbeiten können.
- **UI-020:** Event-to-Command Bindings MÜSSEN unabhängig vom Renderbackend funktionieren.
- **UI-021:** Event-to-Command Bindings MÜSSEN die Sicherheitsregeln von Nova.Trust einhalten.
- **UI-022:** Nova Event-to-Command Binding MUSS vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 19. Integration

- **Nova UI Event System:** Ereigniserkennung und Zustellung.
- **Nova Routed Events:** Ereignisweiterleitung.
- **Nova Event Handlers:** Ereignisregistrierung.
- **Nova Commands and Actions:** Command-Verfügbarkeit und Ausführung.
- **Nova Data Binding:** Auflösung von Commands und Parametern.
- **Nova Observable Properties:** Dynamische Zustandsänderungen.
- **Nova UI Runtime:** Verwaltung aktiver Bindungen.
- **Nova UI Threading Model:** Synchronisation und asynchrone Verarbeitung.
- **Nova UI Component Lifecycle:** Aktivierung und Freigabe.
- **Nova Control Framework:** Ereignisquellen.
- **Nova Declarative UI Model:** Deklarative Bindungsdefinitionen.
- **NovaLang:** Programmatische Ereignisbindungen.
- **NovaStudio:** Visueller Binding-Editor.
- **Nova Logic Graph:** Ereignisgesteuerte Command-Ausführung.
- **Nova.Trust:** Berechtigungen und Isolation.

## 20. Ergebnis

NovaOS erhält einen einheitlichen Mechanismus zur direkten Verbindung von UI-Ereignissen mit Commands.

Benutzerinteraktionen können ohne zusätzliche Event-Handler-Methoden an die Anwendungslogik weitergegeben werden.

Durch typsichere Parameter, dynamische Command-Verfügbarkeit und kontrollierte asynchrone Ausführung bleibt die Benutzeroberfläche flexibel, wartbar und ressourcenschonend.

**Grundsatz:** Ein Ereignis beschreibt, was geschehen ist. Ein Command beschreibt, was daraufhin ausgeführt werden soll. Event-to-Command Binding verbindet beide, ohne zusätzliche Ereignislogik zu erzwingen.