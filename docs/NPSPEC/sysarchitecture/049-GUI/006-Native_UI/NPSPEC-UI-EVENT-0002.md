# NPSPEC-UI-EVENT-0002 – Nova Routed Events

**Status:** Angenommen  
**Bereich:** UI / Events / Event Routing  
**System:** NovaOS

## 1. Zweck

Nova Routed Events definiert die Weiterleitung von UI-Ereignissen innerhalb der logischen Benutzeroberflächenhierarchie.

Ziel ist es, Ereignisse nicht ausschließlich am auslösenden Element zu behandeln, sondern auch übergeordneten UI-Elementen eine kontrollierte Ereignisverarbeitung zu ermöglichen.

Das System erweitert Nova UI Event System um deterministische Ereignisrouten und unterstützt die Trennung zwischen UI-Struktur und Ereignislogik.

## 2. Architektur

Das Subsystem umfasst folgende Komponenten:

| Komponente | Aufgabe |
|---|---|
| Routed Event Registry | Registrierung weiterleitbarer Ereignisse |
| Event Route Builder | Ermittlung der Ereignisroute |
| Event Route Dispatcher | Zustellung entlang der Route |
| Routed Event Context | Verwaltung der Routinginformationen |
| Event Propagation Controller | Steuerung der Ereignisweiterleitung |
| Routed Handler Manager | Verwaltung der Ereignisbehandlungen |

Die Ausführung erfolgt über das Nova UI Event System.

## 3. Routing-Strategien

NovaOS unterstützt drei Routing-Strategien:

| Strategie | Beschreibung |
|---|---|
| Direct | Ereignis wird ausschließlich am Zielelement verarbeitet |
| Tunnel | Ereignis wird von der Wurzel zum Zielelement weitergeleitet |
| Bubble | Ereignis wird vom Zielelement zur Wurzel weitergeleitet |

Die Routing-Strategie wird bei der Ereignisdefinition festgelegt.

Eine Ereignisroute MUSS innerhalb ihres UI-Kontexts eindeutig bestimmbar sein.

## 4. Ereignisroute

Die Ereignisroute beschreibt die Reihenfolge der beteiligten UI-Elemente.

Beispiel einer Hierarchie:

```text
Window
 └── Grid
      └── StackPanel
           └── Button
```

Bei einem Klick auf den Button ergeben sich folgende Routen:

**Tunnel:**

```text
Window → Grid → StackPanel → Button
```

**Bubble:**

```text
Button → StackPanel → Grid → Window
```

**Direct:**

```text
Button
```

Die Ereignisroute wird vor Beginn der jeweiligen Zustellung festgelegt.

Änderungen der UI-Hierarchie während der Verarbeitung DÜRFEN die aktive Route nicht unkontrolliert verändern.

## 5. Routed Event Context

Ein Routed Event stellt folgende Informationen bereit:

| Eigenschaft | Bedeutung |
|---|---|
| EventType | Ereignistyp |
| OriginalSource | Ursprüngliches auslösendes Element |
| Source | Aktuelle logische Ereignisquelle |
| CurrentTarget | Aktuell verarbeitendes UI-Element |
| RoutingStrategy | Verwendete Routing-Strategie |
| Handled | Kennzeichnung als verarbeitet |
| EventData | Ereignisspezifische Daten |

`OriginalSource` bleibt während der gesamten Ereignisverarbeitung unverändert.

`CurrentTarget` wird bei jedem Routing-Schritt aktualisiert.

## 6. Ereignisbehandlung

UI-Elemente können Handler für Routed Events registrieren.

Konzeptionelles NovaLang-Beispiel:

```vb
Private Sub Window_PointerPressed(
    sender As Object,
    e As RoutedEventArgs
) As Handle MainWindow.PointerPressed

    If e.OriginalSource Is Button_1 Then
        ' Ereignis wurde durch Button_1 ausgelöst.
    End If

End Sub
```

Dabei gilt:

- Mehrere Handler können dasselbe Ereignis behandeln.
- Die Handler-Reihenfolge MUSS deterministisch sein.
- Handler können das ursprüngliche Zielelement ermitteln.
- Ereignisse können als verarbeitet markiert werden.
- Die Ereignisbehandlung MUSS mit den NovaLang-Ereignisbindungen kompatibel sein.

## 7. Verarbeitete Ereignisse

Ein Handler kann ein Ereignis als verarbeitet kennzeichnen.

Beispiel:

```vb
Private Sub Button_PointerPressed(
    sender As Object,
    e As RoutedEventArgs
) As Handle Button_1.PointerPressed

    e.Handled = True

End Sub
```

Dabei gilt:

- `Handled` kennzeichnet ein Ereignis als verarbeitet.
- Gewöhnliche nachfolgende Handler werden bei `Handled = True` übersprungen.
- Speziell registrierte Handler DÜRFEN bereits verarbeitete Ereignisse empfangen.
- Das Setzen von `Handled` DARF sicherheitsrelevante Systemoperationen nicht unterdrücken.

Die Ereignisroute selbst bleibt erhalten, auch wenn einzelne Handler übersprungen werden.

## 8. Preview Events

NovaOS unterstützt optionale Preview Events.

Preview Events verwenden normalerweise die Tunnel-Strategie.

Beispiele:

- PreviewPointerPressed.
- PreviewKeyDown.
- PreviewTextInput.

Sie ermöglichen übergeordneten UI-Elementen, Eingaben vor der eigentlichen Zielverarbeitung zu behandeln.

Preview Events und zugehörige Bubble Events können als zusammengehörige Ereignisphasen definiert werden.

Ob beide Phasen denselben Verarbeitungsstatus verwenden, MUSS durch die Ereignisdefinition eindeutig festgelegt sein.

## 9. Logische und visuelle Hierarchie

NovaOS unterscheidet zwischen:

- Logischer UI-Hierarchie.
- Visueller UI-Hierarchie.

Routed Events verwenden grundsätzlich die definierte logische Ereignishierarchie.

Control Templates können zusätzliche visuelle Elemente erzeugen, ohne automatisch die öffentliche Ereignisroute zu verändern.

Bei speziellen Controls DÜRFEN abweichende Routing-Regeln definiert werden.

Diese Regeln MÜSSEN eindeutig dokumentiert und deterministisch sein.

## 10. Benutzerdefinierte Routed Events

Custom Controls können eigene Routed Events registrieren.

Konzeptionelles Beispiel:

```vb
Public Class CustomButton
    Inherits Control

    Public Event Activated As RoutedEvent
        RoutingStrategy = RoutingStrategy.Bubble

End Class
```

Dabei gilt:

- Ereignistyp und Routing-Strategie MÜSSEN definiert sein.
- Ereignisdaten MÜSSEN typsicher sein.
- Registrierte Ereignisse MÜSSEN eindeutig identifizierbar sein.
- Benutzerdefinierte Ereignisse verwenden dieselbe Routing Engine wie Standardereignisse.

## 11. Event Routing und Commands

Routed Events können Commands auslösen.

Beispiel:

```xml
<Button
    Content="Speichern"
    Command="{Binding SaveCommand}" />
```

Dabei gilt:

- Routed Events erfassen die Benutzerinteraktion.
- Commands beschreiben die auszuführende Benutzerabsicht.
- Command Routing bleibt ein eigenständiger Mechanismus.
- Ein Routed Event DARF mehrere unabhängige Reaktionen auslösen, sofern dies ausdrücklich vorgesehen ist.

Die Ereignisweiterleitung DARF keine Command-Berechtigungen ersetzen.

## 12. Threading und Reentrancy

Routed Events werden innerhalb des zuständigen UI-Ausführungskontexts verarbeitet.

Dabei gilt:

- Die Zustellung einer Ereignisroute erfolgt geordnet.
- Handler werden gemäß dem Nova UI Threading Model ausgeführt.
- Verschachtelte Ereignisauslösungen MÜSSEN kontrolliert verarbeitet werden.
- Rekursive Ereignisschleifen MÜSSEN erkannt oder begrenzt werden können.
- Asynchrone Folgeoperationen DÜRFEN die UI Event Loop nicht blockieren.

Die ursprüngliche Ereignisroute MUSS während ihrer Verarbeitung konsistent bleiben.

## 13. Performance

Nova Routed Events arbeitet ausschließlich ereignisgesteuert.

Dabei gilt:

- Ereignisrouten werden nur bei Bedarf erstellt.
- Unveränderte Routinginformationen SOLLEN wiederverwendet werden können.
- Handler-Registrierungen werden effizient verwaltet.
- Bereits verarbeitete Ereignisse SOLLEN unnötige Handler-Aufrufe vermeiden.
- Ereignisrouting DARF keine automatische vollständige UI-Neuzeichnung auslösen.

Layout- und Renderinvalidierungen erfolgen ausschließlich bei tatsächlichen Zustandsänderungen.

## 14. NovaStudio und Logic Graph

NovaStudio unterstützt:

- Auswahl der Routing-Strategie.
- Registrierung benutzerdefinierter Routed Events.
- Visuelle Darstellung von Ereignisrouten.
- Bearbeitung von Event Handlern.
- Anzeige von `OriginalSource` und `CurrentTarget`.
- Debugging der Ereignisweiterleitung.
- Darstellung von Routed Events im Logic Graph.

Logic Graph verwendet Event Nodes als Einstiegspunkte.

Die Routing-Semantik MUSS unabhängig davon bleiben, ob ein Handler in NovaLang oder im Logic Graph implementiert wurde.

## 15. Sicherheit

Nova Routed Events unterliegt Nova.Trust.

Dabei gilt:

- Ereignisrouten DÜRFEN keine Sicherheitsgrenzen überschreiten.
- Nicht vertrauenswürdige Komponenten DÜRFEN keine privilegierten Ereignisse vortäuschen.
- Ereignisdaten MÜSSEN entsprechend ihrer Zugriffsrechte geschützt werden.
- Das Markieren eines Ereignisses als verarbeitet DARF keine Sicherheitsprüfung umgehen.
- Ereignisweiterleitung DARF keine zusätzlichen Capabilities verleihen.

Sicherheitsrelevante Systemereignisse werden unabhängig von gewöhnlichen UI-Handlern durchgesetzt.

## 16. Normative Anforderungen

- **UI-001:** NovaOS MUSS Routed Events unterstützen.
- **UI-002:** Direct-, Tunnel- und Bubble-Routing MÜSSEN unterstützt werden.
- **UI-003:** Ereignisrouten MÜSSEN deterministisch ermittelt werden.
- **UI-004:** Die ursprüngliche Ereignisquelle MUSS während der Verarbeitung erhalten bleiben.
- **UI-005:** Das aktuell verarbeitende UI-Element MUSS ermittelbar sein.
- **UI-006:** Routed Events MÜSSEN typsichere Ereignisdaten unterstützen.
- **UI-007:** Mehrere Handler pro Ereignis MÜSSEN unterstützt werden.
- **UI-008:** Ereignisse MÜSSEN als verarbeitet markiert werden können.
- **UI-009:** Bereits verarbeitete Ereignisse MÜSSEN durch ausdrücklich registrierte Handler empfangen werden können.
- **UI-010:** Preview Events MÜSSEN unterstützt werden können.
- **UI-011:** Benutzerdefinierte Routed Events MÜSSEN registrierbar sein.
- **UI-012:** Routing-Regeln MÜSSEN mit Control Templates kompatibel sein.
- **UI-013:** Änderungen der UI-Hierarchie DÜRFEN aktive Ereignisrouten nicht unkontrolliert verändern.
- **UI-014:** Routed Events MÜSSEN das Nova UI Threading Model einhalten.
- **UI-015:** Ereignisbindungen MÜSSEN kontrolliert freigegeben werden.
- **UI-016:** NovaLang MUSS Routed Events behandeln können.
- **UI-017:** NovaStudio MUSS Ereignisrouten darstellen und diagnostizieren können.
- **UI-018:** Logic Graph MUSS Routed Events verarbeiten können.
- **UI-019:** Routed Events MÜSSEN unabhängig vom Renderbackend funktionieren.
- **UI-020:** Routed Events MÜSSEN die Sicherheitsregeln von Nova.Trust einhalten.
- **UI-021:** Nova Routed Events MUSS vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 17. Integration

- **Nova UI Event System:** Ereignisregistrierung und Zustellung.
- **Nova UI Runtime:** Verwaltung der UI-Hierarchie.
- **Nova UI Threading Model:** Geordnete Ereignisverarbeitung.
- **Nova UI Component Lifecycle:** Registrierung und Freigabe von Handlern.
- **Nova Declarative UI Model:** Deklarative Ereignisbindungen.
- **Nova Control Framework:** Ereignisse nativer Controls.
- **Nova Control Templates:** Trennung visueller und logischer Hierarchie.
- **Nova Commands and Actions:** Ausführung von Benutzeraktionen.
- **Nova Input System:** Eingabeereignisse.
- **Nova Window Manager:** Fensterbezogene Ereignisgrenzen.
- **NovaLang:** Programmatische Ereignisbehandlung.
- **NovaStudio:** Routing-Editor und Debugging.
- **Nova Logic Graph:** Ereignisgesteuerte Logikabläufe.
- **Nova.Trust:** Sicherheit und Isolation.

## 18. Ergebnis

NovaOS erhält eine einheitliche und deterministische Weiterleitung von UI-Ereignissen innerhalb der logischen Benutzeroberflächenhierarchie.

Übergeordnete Controls können Ereignisse ihrer untergeordneten Elemente behandeln, ohne direkte Ereignisbindungen zu jedem einzelnen Element erstellen zu müssen.

Durch Direct-, Tunnel- und Bubble-Routing entstehen flexible und wiederverwendbare UI-Komponenten mit klar definiertem Ereignisverhalten.

**Grundsatz:** Ein Routed Event gehört nicht ausschließlich dem Element, das es ausgelöst hat. Es kann entlang einer definierten Route von allen dafür zuständigen UI-Elementen verarbeitet werden.