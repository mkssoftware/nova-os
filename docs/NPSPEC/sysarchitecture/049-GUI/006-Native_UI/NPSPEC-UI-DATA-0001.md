# NPSPEC-UI-DATA-0001 – Nova Data Binding

**Status:** Angenommen  
**Bereich:** UI / Data Binding  
**System:** NovaOS

## 1. Zweck

Nova Data Binding definiert die typsichere, deklarative und dynamische Verbindung zwischen UI-Eigenschaften und Datenquellen.

Ziel ist die Trennung von Benutzeroberfläche und Anwendungslogik, ohne manuelle Synchronisation sämtlicher Eigenschaften.

Das Binding-System unterstützt NovaLang, NovaStudio und Logic Graph und arbeitet unabhängig vom Renderbackend.

## 2. Architektur

Das Subsystem umfasst folgende Komponenten:

| Komponente | Aufgabe |
|---|---|
| Binding Engine | Verwaltung und Ausführung von Datenbindungen |
| Binding Resolver | Auflösung von Datenquellen und Eigenschaftspfaden |
| Binding Expression | Beschreibung einer Datenbindung |
| Binding Context Manager | Verwaltung von DataContext und Bindungskontexten |
| Change Notification Manager | Verarbeitung von Änderungsbenachrichtigungen |
| Binding Converter | Typkonvertierung zwischen Quelle und Ziel |
| Binding Validation Manager | Prüfung und Fehlerbehandlung |
| Binding Dependency Tracker | Verwaltung aktiver Bindungsabhängigkeiten |

Die Binding Engine verwendet das Nova UI Property System und das Nova UI Threading Model.

## 3. Binding-Modell

Eine Datenbindung verbindet eine Quelle mit einer Zieleigenschaft.

Eine Binding-Definition kann enthalten:

- Datenquelle.
- Eigenschaftspfad.
- Bindungsmodus.
- Aktualisierungsregel.
- Konverter.
- Validierungsregeln.
- Fallback-Wert.
- Null-Ersatzwert.

Datenbindungen MÜSSEN typsicher verarbeitet werden.

Ungültige Bindungen MÜSSEN kontrolliert behandelt werden.

## 4. Bindungsmodi

NovaOS unterstützt folgende Bindungsmodi:

| Modus | Beschreibung |
|---|---|
| OneWay | Quelle aktualisiert Ziel |
| TwoWay | Quelle und Ziel aktualisieren sich gegenseitig |
| OneTime | Einmalige Übertragung des Quellwerts |
| OneWayToSource | Ziel aktualisiert Quelle |

Der Standardmodus DARF vom Ziel-Property abhängen.

Der effektive Bindungsmodus MUSS eindeutig bestimmbar sein.

## 5. Deklarative Datenbindung

Datenbindungen werden im Nova Declarative UI Model definiert.

Beispiel:

```xml
<StackPanel Spacing="8">

    <TextBox
        Name="UserNameInput"
        Text="{Binding UserName, Mode=TwoWay}" />

    <TextBlock
        Text="{Binding UserName}" />

    <Button
        Content="Speichern"
        IsEnabled="{Binding CanSave}" />

</StackPanel>
```

Änderungen der Datenquelle werden automatisch an gebundene UI-Eigenschaften übertragen.

Bei `TwoWay` werden Änderungen des Zielelements entsprechend der Aktualisierungsregel an die Datenquelle zurückgegeben.

## 6. DataContext

UI-Elemente können einen DataContext besitzen.

Dabei gilt:

- Der DataContext definiert die standardmäßige Datenquelle.
- Untergeordnete Elemente können den DataContext erben.
- Ein lokaler DataContext kann den geerbten Kontext überschreiben.
- Änderungen des DataContext MÜSSEN betroffene Bindungen aktualisieren.
- Explizit definierte Datenquellen bleiben unabhängig vom geerbten DataContext.

Die Vererbung erfolgt entlang der dafür definierten logischen UI-Hierarchie.

## 7. Eigenschaftspfade

Binding-Ausdrücke können auf verschachtelte Eigenschaften zugreifen.

Beispiel:

```xml
<TextBlock Text="{Binding User.Profile.DisplayName}" />
```

Unterstützt werden sollen:

- Einfache Eigenschaften.
- Verschachtelte Eigenschaften.
- Indizierte Elemente.
- Auflistungselemente.
- Nullable-Werte.

Die Binding Engine MUSS Änderungen innerhalb verschachtelter Eigenschaftspfade erkennen können, sofern die Datenquelle entsprechende Benachrichtigungen bereitstellt.

Ungültige oder nicht verfügbare Pfadsegmente MÜSSEN kontrolliert behandelt werden.

## 8. Änderungsbenachrichtigungen

Datenquellen können Änderungen ihrer Eigenschaften melden.

Nova Data Binding definiert dafür eine einheitliche Schnittstelle, die auch durch NovaLang-Klassen implementiert werden kann.

Dabei gilt:

- Änderungen werden ereignisgesteuert übertragen.
- Nur betroffene Bindungen werden aktualisiert.
- Mehrere Änderungen SOLLEN zusammengefasst werden können.
- Unveränderte Werte SOLLEN keine erneute Aktualisierung auslösen.
- Ereignisbindungen MÜSSEN beim Entfernen einer Bindung freigegeben werden.

Eine permanente Abfrage der Datenquelle ist nicht erforderlich.

## 9. Aktualisierungsregeln

Die Übertragung vom Ziel zur Quelle kann nach unterschiedlichen Regeln erfolgen.

| Regel | Verhalten |
|---|---|
| PropertyChanged | Aktualisierung bei Eigenschaftsänderung |
| LostFocus | Aktualisierung beim Fokusverlust |
| Explicit | Aktualisierung durch ausdrücklichen Aufruf |

Die Aktualisierungsregel MUSS deklarativ oder programmatisch festgelegt werden können.

Die Binding Engine DARF Änderungen nicht entgegen der definierten Aktualisierungsregel übertragen.

## 10. Wertkonvertierung

Binding Converter ermöglichen die Umwandlung zwischen unterschiedlichen Datentypen.

Beispiel:

```xml
<TextBlock
    Text="{Binding Temperature,
                   Converter={Resource TemperatureConverter}}" />
```

Dabei gilt:

- Konverter MÜSSEN definierte Eingabe- und Ausgabetypen besitzen.
- Konvertierungsfehler MÜSSEN kontrolliert behandelt werden.
- Rückkonvertierungen sind für geeignete TwoWay-Bindungen erforderlich.
- Konverter DÜRFEN keine Sicherheitsgrenzen umgehen.

Konverter SOLLEN möglichst deterministisch und ohne Seiteneffekte arbeiten.

## 11. Validierung

Datenbindungen unterstützen die Validierung übertragener Werte.

Validierungsfehler können entstehen durch:

- Ungültige Datentypen.
- Fehlgeschlagene Konvertierungen.
- Werte außerhalb erlaubter Grenzen.
- Benutzerdefinierte Validierungsregeln.
- Fehler der Datenquelle.

Dabei gilt:

- Fehler MÜSSEN über definierte Schnittstellen verfügbar sein.
- Controls können Fehler über Nova Control States darstellen.
- Ungültige Werte DÜRFEN nicht unkontrolliert in die Datenquelle übernommen werden.
- Die Darstellung eines Fehlers bleibt vom Validierungsmechanismus getrennt.

## 12. Fallback- und Null-Werte

Bindungen können Ersatzwerte definieren.

Beispiel:

```xml
<TextBlock
    Text="{Binding User.DisplayName,
                   FallbackValue='Unbekannt',
                   TargetNullValue='Kein Name'}" />
```

Dabei gilt:

- `FallbackValue` wird bei nicht auflösbaren Bindungen verwendet.
- `TargetNullValue` behandelt gültige Null-Werte.
- Ersatzwerte MÜSSEN mit dem Zieltyp kompatibel sein.

Fehlende Daten DÜRFEN keine unkontrollierten UI-Fehler verursachen.

## 13. Bindung an Auflistungen

Nova Data Binding unterstützt Datenquellen mit mehreren Elementen.

Dabei gilt:

- Auflistungen können an ItemsControls gebunden werden.
- Änderungen der Auflistung können automatisch übertragen werden.
- Hinzufügen, Entfernen, Verschieben und Aktualisieren von Elementen MÜSSEN unterstützt werden können.
- Stabile Elementidentitäten SOLLEN erhalten bleiben.
- Virtualisierte Controls MÜSSEN mit gebundenen Auflistungen zusammenarbeiten.

Die Binding Engine DARF keine vollständige Neuerstellung einer Elementliste bei jeder Änderung voraussetzen.

## 14. Programmatische Datenbindung

Datenbindungen können neben der deklarativen Definition auch über NovaLang erstellt und verwaltet werden.

Die programmatische API MUSS dieselben Bindungsmodi, Validierungsregeln und Sicherheitsanforderungen wie die deklarative Variante verwenden.

NovaStudio und Logic Graph DÜRFEN Bindungen über dieselben zugrunde liegenden Schnittstellen erzeugen.

## 15. Threading und asynchrone Daten

Datenquellen können außerhalb des UI-Ausführungskontexts aktualisiert werden.

Dabei gilt:

- UI-Eigenschaften werden ausschließlich über den zuständigen UI-Kontext verändert.
- Änderungsbenachrichtigungen werden bei Bedarf über den UI Dispatcher übertragen.
- Asynchrone Datenquellen MÜSSEN unterstützt werden können.
- Veraltete Ergebnisse DÜRFEN neuere Werte nicht überschreiben.
- Laufende Anforderungen MÜSSEN beim Entfernen einer Bindung kontrolliert beendet oder entkoppelt werden können.

Die Binding Engine MUSS mit dem Nova UI Threading Model zusammenarbeiten.

## 16. Zyklische Bindungen

Datenbindungen können indirekte Abhängigkeiten erzeugen.

Dabei gilt:

- Unbeabsichtigte Aktualisierungsschleifen MÜSSEN erkannt oder zuverlässig verhindert werden.
- Gleichbleibende Werte SOLLEN keine erneuten Änderungen auslösen.
- Zusammengehörige Aktualisierungen MÜSSEN konsistent verarbeitet werden.
- Fehlerhafte Bindungszyklen DÜRFEN die UI Event Loop nicht dauerhaft blockieren.

Die Verarbeitung MUSS deterministisch bleiben.

## 17. Lebenszyklus und Ressourcen

Datenbindungen unterliegen dem Nova UI Component Lifecycle.

Dabei gilt:

- Bindungen werden bei der Initialisierung oder bei Bedarf aktiviert.
- Änderungen des DataContext werden berücksichtigt.
- Nicht mehr benötigte Bindungen werden deaktiviert.
- Ereignisabonnements und Ressourcen werden kontrolliert freigegeben.
- Bindungen DÜRFEN entfernte UI-Elemente nicht unbeabsichtigt im Speicher halten.

## 18. NovaStudio-Integration

NovaStudio unterstützt:

- Visuelle Erstellung von Bindungen.
- Auswahl von Datenquellen.
- Auswahl von Eigenschaftspfaden.
- Einstellung des Bindungsmodus.
- Auswahl von Konvertern.
- Definition von Validierungsregeln.
- Prüfung von Datentypen.
- Diagnose ungültiger Bindungen.
- Darstellung von Bindungen im Logic Graph.

Designer und Runtime MÜSSEN dieselben Binding-Definitionen verwenden können.

## 19. Sicherheit

Nova Data Binding unterliegt Nova.Trust.

Dabei gilt:

- Bindungen DÜRFEN ausschließlich auf autorisierte Daten zugreifen.
- Schreibzugriffe MÜSSEN die Berechtigungen der Datenquelle einhalten.
- Binding-Ausdrücke DÜRFEN keine unkontrollierte Codeausführung ermöglichen.
- Konverter MÜSSEN innerhalb ihres autorisierten Ausführungskontexts arbeiten.
- Datenbindungen DÜRFEN keine Capability-Grenzen umgehen.
- Fehlerhafte Bindungen DÜRFEN andere UI-Instanzen nicht beeinträchtigen.

Eine erfolgreiche Eigenschaftsauflösung begründet keine zusätzliche Zugriffsberechtigung.

## 20. Normative Anforderungen

- **UI-001:** NovaOS MUSS ein natives Data-Binding-System bereitstellen.
- **UI-002:** Datenbindungen MÜSSEN deklarativ und programmatisch definiert werden können.
- **UI-003:** OneWay-, TwoWay-, OneTime- und OneWayToSource-Bindungen MÜSSEN unterstützt werden.
- **UI-004:** DataContext und dessen Vererbung MÜSSEN unterstützt werden.
- **UI-005:** Verschachtelte Eigenschaftspfade MÜSSEN aufgelöst werden können.
- **UI-006:** Datenbindungen MÜSSEN typsicher verarbeitet werden.
- **UI-007:** Änderungsbenachrichtigungen MÜSSEN unterstützt werden.
- **UI-008:** Aktualisierungsregeln MÜSSEN konfigurierbar sein.
- **UI-009:** Wertkonverter MÜSSEN unterstützt werden.
- **UI-010:** Validierungsregeln und Fehlerzustände MÜSSEN unterstützt werden.
- **UI-011:** Fallback- und Null-Ersatzwerte MÜSSEN unterstützt werden.
- **UI-012:** Bindungen an dynamische Auflistungen MÜSSEN möglich sein.
- **UI-013:** Datenbindungen MÜSSEN mit virtualisierten Controls zusammenarbeiten.
- **UI-014:** Asynchrone Datenquellen MÜSSEN unterstützt werden können.
- **UI-015:** UI-Aktualisierungen MÜSSEN das Nova UI Threading Model einhalten.
- **UI-016:** Unbeabsichtigte Aktualisierungsschleifen MÜSSEN verhindert werden.
- **UI-017:** Änderungen MÜSSEN ausschließlich betroffene Bindungen aktualisieren.
- **UI-018:** Bindungsressourcen MÜSSEN kontrolliert freigegeben werden.
- **UI-019:** NovaStudio MUSS Datenbindungen bearbeiten und validieren können.
- **UI-020:** Logic Graph MUSS Datenbindungen über definierte Schnittstellen verwenden können.
- **UI-021:** Datenbindungen MÜSSEN unabhängig vom Renderbackend funktionieren.
- **UI-022:** Datenbindungen MÜSSEN die Sicherheitsregeln von Nova.Trust einhalten.
- **UI-023:** Nova Data Binding MUSS vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 21. Integration

- **Nova UI Architecture:** Übergeordnete UI-Architektur.
- **Nova UI Runtime:** Verwaltung aktiver Bindungen.
- **Nova Declarative UI Model:** Deklarative Binding-Ausdrücke.
- **Nova UI Property System:** Ziel- und Quelleigenschaften.
- **Nova Control Framework:** Bindbare Control-Eigenschaften.
- **Nova Control States:** Validierungs- und Ladezustände.
- **Nova Virtualized Controls:** Bindungen großer Datenmengen.
- **Nova Styling System:** Datenabhängige Gestaltung.
- **Nova Resource Dictionaries:** Ressourcenbasierte Bindungsdefinitionen.
- **Nova UI Event System:** Änderungsbenachrichtigungen.
- **Nova UI Threading Model:** Synchronisation und asynchrone Verarbeitung.
- **Nova UI Component Lifecycle:** Aktivierung und Freigabe.
- **NovaLang:** Datenmodelle und programmatische Bindungen.
- **NovaStudio:** Visueller Binding-Editor.
- **Nova Logic Graph:** Visuelle Datenverbindungen.
- **Nova.Trust:** Berechtigungen und Isolation.

## 22. Ergebnis

NovaOS erhält ein natives, typsicheres und ereignisgesteuertes Data-Binding-System.

Benutzeroberflächen können deklarativ mit Datenmodellen verbunden werden, ohne jede Änderung manuell synchronisieren zu müssen.

Durch Änderungsverfolgung, Validierung, asynchrone Verarbeitung und gezielte Aktualisierungen bleiben Datenbindungen konsistent und ressourcenschonend.

**Grundsatz:** Nova Data Binding verbindet Daten und Benutzeroberfläche, ohne beide voneinander abhängig zu machen.