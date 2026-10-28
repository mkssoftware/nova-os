# NPSPEC-UI-CONTROL-0002 – Nova Standard Controls

**Status:** Angenommen  
**Bereich:** UI / Controls / Standard Controls  
**System:** NovaOS

## 1. Zweck

Nova Standard Controls definiert die grundlegenden nativen Steuerelemente für Benutzeroberflächen innerhalb von NovaOS.

Ziel ist eine einheitliche, erweiterbare und ressourcenschonende Sammlung von Controls, die Anwendungen ohne eigene Implementierung grundlegender Bedienelemente verwenden können.

Alle Standard Controls basieren auf dem Nova Control Framework und unterstützen deklarative Definitionen, NovaLang, NovaStudio und Logic Graph.

## 2. Architektur

Die Standard Controls verwenden gemeinsame Basisklassen und Schnittstellen.

| Basisklasse | Aufgabe |
|---|---|
| Control | Grundlage interaktiver Steuerelemente |
| ContentControl | Darstellung eines einzelnen Inhalts |
| HeaderedContentControl | Inhalt mit Überschrift |
| ItemsControl | Darstellung mehrerer Datenelemente |
| Selector | Auswahl eines oder mehrerer Elemente |
| RangeBase | Verarbeitung numerischer Wertebereiche |
| TextControl | Gemeinsame Textfunktionen |
| ToggleControl | Verarbeitung umschaltbarer Zustände |

Die Basisklassen DÜRFEN erweitert werden.

Gemeinsame Funktionalität SOLL nicht mehrfach implementiert werden.

## 3. Standard-Control-Kategorien

NovaOS stellt mindestens folgende Control-Kategorien bereit.

### 3.1 Aktionen

| Control | Funktion |
|---|---|
| Button | Auslösen einer Aktion |
| ToggleButton | Umschaltbare Aktion |
| RepeatButton | Wiederholte Aktion bei gedrücktem Zustand |
| Hyperlink | Aktivierung eines Verweises oder Navigationsziels |

### 3.2 Text und Eingabe

| Control | Funktion |
|---|---|
| TextBlock | Darstellung von Text |
| Label | Beschriftung eines Elements |
| TextBox | Ein- und mehrzeilige Texteingabe |
| PasswordBox | Geschützte Passworteingabe |
| RichTextBox | Formatierte Texteingabe |
| SearchBox | Sucheingabe mit optionalen Suchfunktionen |

### 3.3 Auswahl

| Control | Funktion |
|---|---|
| CheckBox | Auswahl mit zwei oder drei Zuständen |
| RadioButton | Exklusive Auswahl innerhalb einer Gruppe |
| ComboBox | Auswahl aus einer aufklappbaren Liste |
| ListBox | Auswahl aus einer Elementliste |
| ListView | Strukturierte Darstellung und Auswahl |
| TreeView | Hierarchische Darstellung und Auswahl |
| TabControl | Wechsel zwischen Inhaltsbereichen |

### 3.4 Werte und Fortschritt

| Control | Funktion |
|---|---|
| Slider | Interaktive Werteinstellung |
| ProgressBar | Darstellung eines Fortschritts |
| ScrollBar | Steuerung eines Bildlaufbereichs |
| NumericUpDown | Numerische Eingabe |
| RangeSlider | Auswahl eines Werteintervalls |

### 3.5 Container und Navigation

| Control | Funktion |
|---|---|
| ContentControl | Aufnahme eines einzelnen Inhalts |
| ItemsControl | Darstellung einer Elementmenge |
| GroupBox | Gruppierung zusammengehöriger Inhalte |
| Expander | Ein- und ausklappbarer Inhaltsbereich |
| ScrollViewer | Darstellung scrollbarer Inhalte |
| Menu | Menüstruktur |
| ContextMenu | Kontextabhängiges Menü |
| ToolBar | Werkzeugleiste |
| StatusBar | Statusinformationen |
| Ribbon | Kontextabhängige Befehlsoberfläche |

### 3.6 Erweiterte Eingaben

| Control | Funktion |
|---|---|
| DatePicker | Datumsauswahl |
| TimePicker | Uhrzeitauswahl |
| ColorPicker | Farbauswahl |
| FilePicker | Dateiauswahl |
| FolderPicker | Ordnerauswahl |

Systembezogene Auswahlfunktionen MÜSSEN über die zuständigen NovaOS-Capabilities erfolgen.

## 4. Gemeinsame Eigenschaften

Alle geeigneten Standard Controls unterstützen gemeinsame Eigenschaften.

Dazu gehören:

- Name.
- Width und Height.
- MinWidth und MinHeight.
- MaxWidth und MaxHeight.
- Margin und Padding.
- HorizontalAlignment und VerticalAlignment.
- Visibility.
- IsEnabled.
- IsFocused.
- Style.
- Template.
- DataContext.
- ToolTip.
- TabIndex.

Controls DÜRFEN zusätzliche typspezifische Eigenschaften definieren.

Eigenschaften MÜSSEN über das Nova UI Property System verwaltet werden.

## 5. Gemeinsame Ereignisse

Standard Controls unterstützen die für ihren Typ relevanten Ereignisse.

Grundlegende Ereignisse:

- Click.
- PointerEntered.
- PointerExited.
- PointerPressed.
- PointerReleased.
- KeyDown.
- KeyUp.
- GotFocus.
- LostFocus.
- ValueChanged.
- SelectionChanged.
- TextChanged.
- Checked.
- Unchecked.

Ereignisse MÜSSEN über das Nova UI Event System verarbeitet werden.

Die Ereignisbindung MUSS sowohl deklarativ als auch programmatisch möglich sein.

## 6. Deklarative Verwendung

Standard Controls werden über das Nova Declarative UI Model definiert.

Beispiel:

```xml
<StackPanel Spacing="12"
            Padding="16">

    <TextBlock Text="Benutzereinstellungen" />

    <TextBox Name="UserName"
             Text="{Binding UserName, Mode=TwoWay}" />

    <CheckBox Name="AutoSave"
              Content="Automatisch speichern"
              IsChecked="{Binding AutoSave, Mode=TwoWay}" />

    <Slider Name="Volume"
            Minimum="0"
            Maximum="100"
            Value="{Binding Volume, Mode=TwoWay}" />

    <Button Name="SaveButton"
            Content="Speichern"
            Click="SaveButton_Click" />

</StackPanel>
```

Die Definition beschreibt Struktur, Eigenschaften und Bindungen.

Das funktionale Verhalten wird durch NovaLang oder Logic Graph bereitgestellt.

## 7. NovaLang-Integration

Standard Controls können direkt aus NovaLang angesprochen werden.

Beispiel:

```vb
Private Sub SaveButton_Click() As Handle SaveButton.Click
    SaveSettings()
End Sub
```

Mehrere Controls können denselben Ereignishandler verwenden.

```vb
Private Sub Navigation_Click() As Handle Button_1.Click, Button_2.Click
    UpdateNavigation()
End Sub
```

Dynamische Ereignisbindungen werden unterstützt.

```vb
addHandle SaveButton.Click To SaveSettings()
```

Die Ereignisbindung MUSS typsicher validiert werden.

## 8. Zustandsmodell

Standard Controls besitzen definierte funktionale und visuelle Zustände.

Beispiele:

- Normal.
- PointerOver.
- Pressed.
- Focused.
- Disabled.
- Selected.
- Checked.
- Indeterminate.
- ValidationError.

Die Zustandsverwaltung erfolgt über das Nova Control Framework.

Styles und Templates bestimmen die visuelle Darstellung.

Ein Zustandswechsel DARF keine unnötige vollständige Neuerstellung des Controls verursachen.

## 9. Styles und Templates

Alle geeigneten Standard Controls MÜSSEN individuell gestaltet werden können.

Dabei gilt:

- Styles definieren Darstellungseigenschaften.
- Templates definieren die visuelle Struktur.
- Funktionales Verhalten bleibt unabhängig vom Erscheinungsbild.
- Zustandsabhängige Darstellungen werden unterstützt.
- Globale und lokale Designressourcen können verwendet werden.

NovaOS stellt ein einheitliches Standarddesign bereit.

Anwendungen DÜRFEN eigene Styles und Templates verwenden.

## 10. Eingabeverhalten

Standard Controls unterstützen die jeweils geeigneten Eingabemethoden.

Dazu gehören:

- Maus.
- Tastatur.
- Touch.
- Stift.

Interaktive Controls MÜSSEN eine definierte Tastaturbedienung besitzen.

Fokusnavigation und Eingabeverarbeitung erfolgen über die zuständigen Nova UI Subsysteme.

Controls DÜRFEN Eingabeereignisse nicht außerhalb ihres autorisierten UI-Kontexts abfangen.

## 11. Datenbindung und Validierung

Standard Controls unterstützen die Datenbindung über Nova UI Data Binding.

Dabei gilt:

- Änderungen können automatisch zwischen Control und Datenquelle übertragen werden.
- Bindungsrichtungen werden durch das Binding System bestimmt.
- Eingaben können validiert werden.
- Validierungsfehler können über Control-Zustände dargestellt werden.
- Ungültige Eingaben DÜRFEN keine unkontrollierten Zustandsänderungen verursachen.

Die Datenbindung MUSS bestehende Berechtigungsgrenzen einhalten.

## 12. Barrierefreiheit

Standard Controls MÜSSEN die Nova UI Accessibility Architecture unterstützen.

Dazu gehören:

- Semantische Rollen.
- Zugängliche Namen.
- Beschreibungen.
- Tastaturbedienung.
- Fokusinformationen.
- Zustandsinformationen.
- Unterstützung assistiver Technologien.

Die semantische Funktion eines Controls MUSS unabhängig von seinem visuellen Template erkennbar bleiben.

Benutzerdefinierte Styles DÜRFEN die grundlegende Bedienbarkeit nicht unzulässig einschränken.

## 13. Performance und Ressourcen

Standard Controls verwenden die Ressourcenverwaltung der Nova UI Runtime.

Dabei gilt:

- Controls werden bedarfsgerecht erstellt.
- Gemeinsam nutzbare Styles und Templates SOLLEN wiederverwendet werden.
- Änderungen erzeugen gezielte Layout- und Renderinvalidierungen.
- Unveränderte Controls verursachen keine unnötigen Renderoperationen.
- Listen- und Datenelemente SOLLEN Virtualisierung unterstützen.
- Ressourcen werden beim Entfernen eines Controls kontrolliert freigegeben.

Standard Controls DÜRFEN keine permanente Aktualisierungsschleife benötigen.

## 14. NovaStudio-Integration

Alle Standard Controls MÜSSEN im NovaStudio UI Designer verfügbar gemacht werden können.

Dabei gilt:

- Controls werden über eine Toolbox angeboten.
- Eigenschaften werden im Property Editor angezeigt.
- Ereignisse können NovaLang-Methoden oder Logic-Graph-Nodes zugeordnet werden.
- Styles und Templates können bearbeitet werden.
- Die Vorschau verwendet dieselben Control-Definitionen wie die Runtime.

Control-Metadaten SOLLEN automatisch für Designer, Dokumentation und Codevervollständigung bereitgestellt werden.

## 15. Sicherheit

Standard Controls unterliegen Nova.Trust.

Dabei gilt:

- Controls erhalten keine impliziten Systemberechtigungen.
- Geschützte Systemfunktionen werden ausschließlich über autorisierte Capabilities ausgeführt.
- Eingaben und Datenbindungen werden entsprechend ihrer Anforderungen validiert.
- Sensible Daten MÜSSEN vor unberechtigtem Zugriff geschützt werden.
- Fehler einzelner Controls DÜRFEN andere UI-Instanzen nicht beeinträchtigen.

Ein Control DARF durch seine Darstellung keine Sicherheitsprüfung umgehen.

## 16. Normative Anforderungen

- **UI-001:** NovaOS MUSS eine native Sammlung grundlegender Standard Controls bereitstellen.
- **UI-002:** Alle Standard Controls MÜSSEN auf dem Nova Control Framework basieren.
- **UI-003:** Gemeinsame Funktionalität SOLL über wiederverwendbare Basisklassen bereitgestellt werden.
- **UI-004:** Controls MÜSSEN deklarativ definiert werden können.
- **UI-005:** Controls MÜSSEN aus NovaLang verwendbar sein.
- **UI-006:** Controls MÜSSEN mit dem Nova Logic Graph verbunden werden können.
- **UI-007:** Eigenschaften MÜSSEN über das Nova UI Property System verwaltet werden.
- **UI-008:** Ereignisse MÜSSEN über das Nova UI Event System verarbeitet werden.
- **UI-009:** Datenbindungen MÜSSEN unterstützt werden.
- **UI-010:** Styles und Templates MÜSSEN unterstützt werden.
- **UI-011:** Interaktive Controls MÜSSEN definierte Fokus- und Tastaturregeln besitzen.
- **UI-012:** Standard Controls MÜSSEN Barrierefreiheit unterstützen.
- **UI-013:** Controls MÜSSEN mit der Nova Layout Engine zusammenarbeiten.
- **UI-014:** Controls MÜSSEN unabhängig vom Renderbackend funktionieren.
- **UI-015:** Änderungen MÜSSEN gezielte Invalidierungen auslösen.
- **UI-016:** Große Datenmengen SOLLEN durch Virtualisierung verarbeitet werden können.
- **UI-017:** Standard Controls MÜSSEN im NovaStudio Designer verfügbar gemacht werden können.
- **UI-018:** Controls MÜSSEN die Sicherheitsregeln von Nova.Trust einhalten.
- **UI-019:** Fehlerhafte Controls MÜSSEN kontrolliert behandelt werden.
- **UI-020:** Nova Standard Controls MÜSSEN vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 17. Integration

- **Nova Control Framework:** Gemeinsame Control-Architektur.
- **Nova UI Runtime:** Verwaltung der Control-Instanzen.
- **Nova UI Property System:** Eigenschaften und Zustände.
- **Nova UI Event System:** Ereignisverarbeitung.
- **Nova UI Data Binding:** Verbindung mit Datenquellen.
- **Nova Layout Engine:** Größen- und Positionsberechnung.
- **Nova UI Style Engine:** Erscheinungsbild.
- **Nova UI Template Engine:** Visuelle Control-Struktur.
- **Nova UI Renderer Abstraction:** Darstellung.
- **Nova Input Architecture:** Eingabeverarbeitung.
- **Nova UI Accessibility:** Barrierefreiheit.
- **NovaLang:** Programmatische Verwendung.
- **NovaStudio:** Visueller Designer.
- **Nova Logic Graph:** Visuelle Ereignis- und Anwendungslogik.
- **Nova.Trust:** Berechtigungen und Isolation.

## 18. Ergebnis

NovaOS erhält eine einheitliche Sammlung nativer Standard Controls für grundlegende und komplexe Benutzeroberflächen.

Alle Controls verwenden gemeinsame Eigenschaften, Ereignisse, Datenbindungen und Lebenszyklusregeln.

Durch die Trennung von Funktionalität und Darstellung können Steuerelemente individuell gestaltet und in unterschiedlichen Anwendungen wiederverwendet werden.

**Grundsatz:** Nova Standard Controls stellen einheitliche, vertraute und anpassbare Bedienelemente bereit, ohne Anwendungen auf ein bestimmtes Erscheinungsbild oder Renderbackend festzulegen.