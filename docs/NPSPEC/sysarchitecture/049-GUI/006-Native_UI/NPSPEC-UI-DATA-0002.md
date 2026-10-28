# NPSPEC-UI-DATA-0002 – Nova Observable Properties

**Status:** Angenommen  
**Bereich:** UI / Data / Observable Properties  
**System:** NovaOS

## 1. Zweck

Nova Observable Properties definiert ein einheitliches System zur Erkennung und Weitergabe von Eigenschaftsänderungen.

Ziel ist die automatische Aktualisierung abhängiger UI-Elemente, Datenbindungen und Logikkomponenten, ohne Eigenschaften permanent abfragen zu müssen.

Das System ist ereignisgesteuert, typsicher und unabhängig vom Renderbackend.

## 2. Architektur

Das Subsystem umfasst folgende Komponenten:

| Komponente | Aufgabe |
|---|---|
| Observable Property | Beobachtbare Eigenschaft |
| Property Change Tracker | Erkennung von Wertänderungen |
| Property Notification Dispatcher | Weitergabe von Änderungsbenachrichtigungen |
| Property Dependency Manager | Verwaltung abhängiger Eigenschaften |
| Property Subscription Manager | Verwaltung von Beobachtern |
| Property Update Coordinator | Zusammenfassung und Reihenfolge von Änderungen |

Nova Observable Properties verwendet das Nova UI Property System und stellt Änderungsinformationen für Nova Data Binding bereit.

## 3. Eigenschaftsmodell

Eine beobachtbare Eigenschaft besitzt:

- Eindeutigen Namen innerhalb ihres Objekttyps.
- Definierten Datentyp.
- Aktuellen Wert.
- Optionalen Standardwert.
- Änderungsbenachrichtigung.
- Optionalen Validierungsregeln.
- Optionalen Abhängigkeiten.

Eine Eigenschaft MUSS nicht automatisch beobachtbar sein.

Beobachtbare Eigenschaften MÜSSEN Änderungen über eine einheitliche Schnittstelle bereitstellen.

## 4. NovaLang-Integration

NovaLang unterstützt beobachtbare Eigenschaften über eine deklarative Kennzeichnung.

Beispiel:

```vb
Public Class UserModel

    Public Observable Property UserName As String = ""

    Public Observable Property IsActive As Boolean = False

End Class
```

Die erforderliche Änderungsverfolgung wird durch NovaLang beziehungsweise die zuständige Runtime bereitgestellt.

Entwickler müssen für einfache beobachtbare Eigenschaften keine manuellen Änderungsereignisse implementieren.

## 5. Änderungsbenachrichtigungen

Ändert sich der Wert einer beobachtbaren Eigenschaft, werden registrierte Beobachter benachrichtigt.

Dabei gilt:

- Änderungen werden ereignisgesteuert erkannt.
- Unveränderte Werte SOLLEN keine Benachrichtigung auslösen.
- Die Gleichheitsprüfung MUSS definiert sein.
- Benachrichtigungen MÜSSEN den betroffenen Eigenschaftsnamen identifizieren.
- Alte und neue Werte DÜRFEN bereitgestellt werden, sofern dies zulässig ist.

Eine permanente Überprüfung der Eigenschaftswerte ist nicht erforderlich.

## 6. Änderungsereignisse

Observable Properties stellen ein standardisiertes Änderungsereignis bereit.

Konzeptionelles Beispiel:

```vb
Private Sub UserName_Changed(
    oldValue As String,
    newValue As String
) As Handle UserModel.UserName.Changed

    UpdateUserDisplay()

End Sub
```

Änderungsereignisse MÜSSEN mit dem Nova UI Event System zusammenarbeiten.

Die konkrete Ereignisbindung folgt den allgemeinen NovaLang-Ereignisregeln.

## 7. Abhängige Eigenschaften

Eine beobachtbare Eigenschaft kann von anderen Eigenschaften abhängen.

Beispiel:

```vb
Public Class UserModel

    Public Observable Property FirstName As String = ""

    Public Observable Property LastName As String = ""

    Public ReadOnly Computed Property FullName As String
        DependsOn FirstName, LastName

        Get
            Return FirstName & " " & LastName
        End Get
    End Property

End Class
```

Dabei gilt:

- Abhängigkeiten MÜSSEN eindeutig definiert werden können.
- Änderungen der Quellwerte MÜSSEN abhängige Eigenschaften invalidieren.
- Berechnete Werte SOLLEN nur bei Bedarf neu ermittelt werden.
- Zyklische Abhängigkeiten MÜSSEN erkannt und kontrolliert behandelt werden.

## 8. Änderungsreihenfolge

Mehrere zusammenhängende Eigenschaftsänderungen müssen konsistent verarbeitet werden.

Dabei gilt:

- Änderungen werden innerhalb eines definierten Ausführungskontexts geordnet.
- Abhängige Eigenschaften werden nach ihren Abhängigkeiten aktualisiert.
- Mehrfache Invalidierungen derselben Eigenschaft SOLLEN zusammengefasst werden.
- Beobachter DÜRFEN keine dauerhaft inkonsistenten Zwischenzustände erhalten, wenn eine atomare Aktualisierung vereinbart wurde.

Die Verarbeitung MUSS deterministisch erfolgen.

## 9. Batch Updates

Nova Observable Properties unterstützt die Zusammenfassung mehrerer Änderungen.

Konzeptionelles Beispiel:

```vb
Using UpdateScope = BeginPropertyUpdate()

    User.FirstName = "Max"
    User.LastName = "Mustermann"
    User.IsActive = True

End Using
```

Innerhalb eines Update-Bereichs können Benachrichtigungen gesammelt werden.

Beim Abschluss werden die relevanten Änderungen konsistent veröffentlicht.

Batch Updates DÜRFEN keine tatsächlich erforderlichen Änderungsinformationen verlieren.

## 10. Datenbindung

Observable Properties bilden eine Grundlage für Nova Data Binding.

Dabei gilt:

- Bindungen können Eigenschaften direkt beobachten.
- Änderungen werden automatisch an abhängige UI-Eigenschaften weitergegeben.
- TwoWay-Bindungen können beobachtbare Eigenschaften aktualisieren.
- Ungültige Werte werden gemäß den Binding- und Validierungsregeln behandelt.
- Entfernte Bindungen MÜSSEN ihre Beobachtungsregistrierungen freigeben.

Die Datenquelle MUSS keine Kenntnis über die gebundenen UI-Elemente besitzen.

## 11. Auflistungen und Objekte

Observable Properties können Referenzen auf komplexe Objekte und Auflistungen enthalten.

Dabei gilt:

- Änderungen der Eigenschaftsreferenz werden erkannt.
- Änderungen innerhalb referenzierter Objekte benötigen deren eigene Änderungsbenachrichtigungen.
- Änderungen innerhalb von Auflistungen werden über das zuständige Observable-Collection-Modell bereitgestellt.
- Verschachtelte Bindungen MÜSSEN ihre Beobachtungsregistrierungen bei Referenzänderungen aktualisieren.

Eine beobachtbare Objektreferenz bedeutet nicht automatisch, dass sämtliche internen Objektänderungen beobachtbar sind.

## 12. Threading

Eigenschaften können außerhalb des UI-Ausführungskontexts verändert werden.

Dabei gilt:

- Änderungsbenachrichtigungen MÜSSEN eine definierte Threading-Semantik besitzen.
- UI-Aktualisierungen erfolgen über den zuständigen UI Dispatcher.
- Gleichzeitige Änderungen MÜSSEN kontrolliert synchronisiert werden.
- Veraltete Benachrichtigungen DÜRFEN neuere Zustände nicht überschreiben.
- Beobachter DÜRFEN die UI Event Loop nicht dauerhaft blockieren.

Das System MUSS mit dem Nova UI Threading Model zusammenarbeiten.

## 13. Lebenszyklus

Beobachtungsregistrierungen unterliegen einer definierten Lebensdauer.

Dabei gilt:

- Beobachter können registriert und entfernt werden.
- Registrierungen MÜSSEN eindeutig verwaltet werden.
- Nicht mehr benötigte Registrierungen werden freigegeben.
- Entfernte UI-Elemente DÜRFEN nicht unbeabsichtigt durch Beobachtungsregistrierungen im Speicher verbleiben.
- Objektentsorgung MUSS aktive Beobachtungen kontrolliert beenden.

Schwache Referenzen DÜRFEN zur Vermeidung unbeabsichtigter Objektbindung verwendet werden.

## 14. Performance

Nova Observable Properties arbeitet vollständig ereignisgesteuert.

Dabei gilt:

- Unveränderte Werte erzeugen keine unnötigen Aktualisierungen.
- Abhängigkeiten SOLLEN effizient zwischengespeichert werden.
- Benachrichtigungen SOLLEN bei Bedarf zusammengefasst werden.
- Nur tatsächlich betroffene Beobachter werden informiert.
- Berechnete Eigenschaften SOLLEN verzögert ausgewertet werden können.
- Nicht benötigte Registrierungen werden freigegeben.

Das System DARF keine permanente Polling-Schleife benötigen.

## 15. NovaStudio und Logic Graph

NovaStudio unterstützt:

- Kennzeichnung beobachtbarer Eigenschaften.
- Anzeige von Eigenschaftsabhängigkeiten.
- Visuelle Verbindung mit Data Binding.
- Diagnose von Änderungsbenachrichtigungen.
- Erkennung zyklischer Abhängigkeiten.
- Darstellung von Änderungsereignissen im Logic Graph.

Observable Properties MÜSSEN über dieselben Metadaten für NovaLang und Logic Graph verfügbar sein.

## 16. Sicherheit

Nova Observable Properties unterliegt Nova.Trust.

Dabei gilt:

- Beobachter DÜRFEN ausschließlich autorisierte Eigenschaften lesen.
- Schreibzugriffe MÜSSEN bestehende Berechtigungen einhalten.
- Änderungsbenachrichtigungen DÜRFEN keine geschützten Werte offenlegen.
- Beobachtungen DÜRFEN keine Capability-Grenzen umgehen.
- Fehlerhafte Beobachter DÜRFEN andere UI-Instanzen nicht beeinträchtigen.

Eine Beobachtungsregistrierung begründet keine zusätzlichen Zugriffsrechte.

## 17. Normative Anforderungen

- **UI-001:** NovaOS MUSS beobachtbare Eigenschaften unterstützen.
- **UI-002:** Observable Properties MÜSSEN typsicher definiert werden können.
- **UI-003:** Eigenschaftsänderungen MÜSSEN ereignisgesteuert erkannt werden.
- **UI-004:** Änderungsbenachrichtigungen MÜSSEN standardisierte Schnittstellen verwenden.
- **UI-005:** Unveränderte Werte SOLLEN keine Benachrichtigung auslösen.
- **UI-006:** Beobachter MÜSSEN registriert und entfernt werden können.
- **UI-007:** Abhängige Eigenschaften MÜSSEN unterstützt werden.
- **UI-008:** Zyklische Eigenschaftsabhängigkeiten MÜSSEN erkannt werden.
- **UI-009:** Berechnete Eigenschaften MÜSSEN unterstützt werden können.
- **UI-010:** Mehrere Eigenschaftsänderungen MÜSSEN zusammengefasst werden können.
- **UI-011:** Änderungsbenachrichtigungen MÜSSEN deterministisch verarbeitet werden.
- **UI-012:** Observable Properties MÜSSEN mit Nova Data Binding zusammenarbeiten.
- **UI-013:** Verschachtelte Eigenschaftsbindungen MÜSSEN unterstützt werden können.
- **UI-014:** UI-Aktualisierungen MÜSSEN das Nova UI Threading Model einhalten.
- **UI-015:** Beobachtungsregistrierungen MÜSSEN kontrolliert freigegeben werden.
- **UI-016:** NovaLang MUSS eine deklarative Definition beobachtbarer Eigenschaften ermöglichen.
- **UI-017:** NovaStudio MUSS Observable Properties bearbeiten und diagnostizieren können.
- **UI-018:** Logic Graph MUSS Eigenschaftsänderungen über definierte Schnittstellen verarbeiten können.
- **UI-019:** Observable Properties MÜSSEN unabhängig vom Renderbackend funktionieren.
- **UI-020:** Observable Properties MÜSSEN die Sicherheitsregeln von Nova.Trust einhalten.
- **UI-021:** Nova Observable Properties MUSS vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 18. Integration

- **Nova UI Property System:** Einheitliches Eigenschaftsmodell.
- **Nova Data Binding:** Automatische Synchronisation von Daten und UI.
- **Nova UI Event System:** Änderungsereignisse.
- **Nova UI Runtime:** Verwaltung aktiver Beobachtungen.
- **Nova UI Threading Model:** Synchronisation und Dispatcher.
- **Nova UI Component Lifecycle:** Lebensdauer von Beobachtern.
- **Nova Control Framework:** Beobachtbare Control-Eigenschaften.
- **Nova Control States:** Zustandsabhängige Eigenschaften.
- **Nova Virtualized Controls:** Aktualisierung datenbasierter Elemente.
- **NovaLang:** Sprachunterstützung für Observable Properties.
- **NovaStudio:** Visueller Eigenschafts- und Binding-Editor.
- **Nova Logic Graph:** Ereignisgesteuerte Eigenschaftsverarbeitung.
- **Nova.Trust:** Berechtigungen und Isolation.

## 19. Ergebnis

NovaOS erhält ein natives, typsicheres und ereignisgesteuertes System zur Beobachtung von Eigenschaftsänderungen.

Datenmodelle, UI-Elemente und Logikkomponenten können automatisch auf relevante Änderungen reagieren, ohne Werte permanent abfragen zu müssen.

Durch Abhängigkeitsverwaltung, Batch Updates und gezielte Benachrichtigungen bleibt die Verarbeitung konsistent und ressourcenschonend.

**Grundsatz:** Eine Eigenschaft meldet ihre Änderung nur dann, wenn sich ihr Wert tatsächlich verändert – und ausschließlich an die Komponenten, die davon betroffen sind.