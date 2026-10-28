# NPSPEC-UI-CONTROL-0001 – Nova Control Framework

**Status:** Angenommen  
**Bereich:** UI / Controls / Architecture  
**System:** NovaOS

## 1. Zweck

Nova Control Framework definiert die Architektur, Eigenschaften und grundlegenden Verhaltensweisen nativer UI-Steuerelemente innerhalb von NovaOS.

Ziel ist ein einheitliches, erweiterbares und ressourcenschonendes Steuerelementsystem, das Darstellung, Verhalten und Anwendungslogik voneinander trennt.

Das Framework bildet die Grundlage für sämtliche nativen Nova UI Controls und ermöglicht deren Verwendung in NovaLang, NovaStudio und deklarativen Benutzeroberflächen.

## 2. Architektur

Das Nova Control Framework umfasst folgende Komponenten:

| Komponente | Aufgabe |
|---|---|
| Control Base | Gemeinsame Grundlage aller Steuerelemente |
| Control Registry | Registrierung und Auflösung verfügbarer Control-Typen |
| Control Property System | Verwaltung von Eigenschaften |
| Control Event System | Verarbeitung von Ereignissen |
| Control Template System | Definition der visuellen Struktur |
| Control State Manager | Verwaltung von Zuständen |
| Control Behavior System | Wiederverwendbare Interaktionslogik |
| Control Factory | Erstellung und Initialisierung von Controls |

Das Framework verwendet die vorhandenen Nova UI Subsysteme und implementiert keine eigene Rendering- oder Layout Engine.

## 3. Control-Modell

Jedes Control besitzt eine eindeutige Typdefinition und eine Instanzidentität innerhalb seines UI-Kontexts.

Die grundlegende Control-Hierarchie lautet:

```text
UIObject
 └── VisualElement
      └── FrameworkElement
           └── Control
                ├── Button
                ├── TextBox
                ├── CheckBox
                ├── ComboBox
                ├── Slider
                ├── ProgressBar
                ├── ListView
                ├── TreeView
                └── CustomControl
```

Die Hierarchie beschreibt die grundlegende Vererbung.

Layoutcontainer, dekorative Elemente und andere UI-Komponenten MÜSSEN nicht zwingend von Control erben.

## 4. Control-Eigenschaften

Controls verwenden das Nova UI Property System.

Grundlegende Eigenschaften umfassen:

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

Eigenschaften können Standardwerte, Validierung, Änderungsbenachrichtigungen und Datenbindungen besitzen.

Änderungen MÜSSEN nur die tatsächlich betroffenen UI-Subsysteme invalidieren.

## 5. Control-Zustände

Controls unterstützen definierte visuelle und funktionale Zustände.

Grundlegende Zustände:

- Normal.
- PointerOver.
- Pressed.
- Focused.
- Disabled.
- Selected.
- Checked.
- Indeterminate.
- ValidationError.

Nicht jeder Zustand ist für jeden Control-Typ erforderlich.

Zustände DÜRFEN kombiniert werden, sofern ihre Kombination gültig ist.

Die visuelle Darstellung eines Zustands wird durch Styles und Templates bestimmt.

## 6. Darstellung und Verhalten

Die Architektur trennt drei Verantwortungsbereiche:

**Control:** Definiert Eigenschaften, Ereignisse und funktionales Verhalten.

**Template:** Definiert die visuelle Struktur des Controls.

**Style:** Definiert dessen Erscheinungsbild und zustandsabhängige Eigenschaften.

Ein Control MUSS ohne Änderung seiner funktionalen Implementierung unterschiedlich dargestellt werden können.

Templates DÜRFEN das funktionale Verhalten eines Controls nicht unzulässig verändern.

## 7. Control Templates

Control Templates beschreiben die interne visuelle Struktur eines Steuerelements.

Beispiel:

```xml
<ControlTemplate TargetType="Button">
    <Border Background="{TemplateBinding Background}"
            CornerRadius="6"
            Padding="{TemplateBinding Padding}">

        <ContentPresenter />

    </Border>
</ControlTemplate>
```

Dabei gilt:

- Templates werden deklarativ definiert.
- TemplateBinding verbindet Control-Eigenschaften mit Template-Elementen.
- Templates können vollständig ausgetauscht werden.
- Die funktionale Control-Instanz bleibt beim Templatewechsel erhalten.
- Template-Ressourcen werden über den UI-Lebenszyklus verwaltet.

## 8. Ereignisse

Controls verwenden das Nova UI Event System.

Grundlegende Ereignisse umfassen:

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

Ereignisse können deklarativ oder programmatisch gebunden werden.

NovaLang unterstützt beispielsweise:

```vb
Private Sub Button1_Click() As Handle Button_1.Click
    ' Ereignisbehandlung
End Sub
```

Mehrfachbindungen:

```vb
Private Sub Button_Click() As Handle Button_1.Click, Button_2.Click
    ' Gemeinsame Ereignisbehandlung
End Sub
```

Dynamische Bindungen:

```vb
addHandle Button_1.Click To ButtonClick()
```

Die Ereignisverarbeitung MUSS mit dem Nova UI Threading Model übereinstimmen.

## 9. Datenbindung

Controls unterstützen die Datenbindung über das Nova UI Data Binding System.

Dabei können Eigenschaften mit Datenquellen verbunden werden.

Unterstützte Bindungsarten:

- OneWay.
- TwoWay.
- OneTime.
- OneWayToSource.

Beispiel:

```xml
<TextBox Text="{Binding UserName, Mode=TwoWay}" />

<TextBlock Text="{Binding UserName}" />
```

Datenänderungen MÜSSEN gezielte Aktualisierungen auslösen.

Datenbindungen DÜRFEN keine bestehenden Capability- oder Berechtigungsgrenzen umgehen.

## 10. Control-Verhalten

Wiederverwendbare Verhaltensweisen können unabhängig von konkreten Controls definiert werden.

Beispiele:

- Eingabevalidierung.
- Tastaturnavigation.
- Auswahlverhalten.
- Drag-and-Drop.
- Kontextmenüs.
- Tooltips.
- Befehlsausführung.

Behaviors DÜRFEN deklarativ oder programmatisch zugewiesen werden.

Sie MÜSSEN beim Entfernen eines Controls ordnungsgemäß freigegeben werden.

## 11. Standard Controls

Nova Control Framework stellt grundlegende Steuerelemente bereit.

| Kategorie | Controls |
|---|---|
| Aktionen | Button, ToggleButton, RepeatButton |
| Texteingabe | TextBox, PasswordBox |
| Textdarstellung | TextBlock, Label |
| Auswahl | CheckBox, RadioButton, ComboBox |
| Werte | Slider, ProgressBar, ScrollBar |
| Listen | ListBox, ListView, TreeView |
| Navigation | TabControl, Menu, ContextMenu |
| Container | ContentControl, ItemsControl, GroupBox |
| Erweiterte Controls | Ribbon, ToolBar, StatusBar |

Die konkreten Control-Typen werden in eigenen NPSPECs spezifiziert.

## 12. Custom Controls

NovaOS unterstützt benutzerdefinierte Steuerelemente.

Custom Controls können:

- Von vorhandenen Controls erben.
- Neue Eigenschaften definieren.
- Eigene Ereignisse bereitstellen.
- Control Templates verwenden.
- Wiederverwendbare Behaviors integrieren.
- Eigene Layout- und Darstellungsanforderungen besitzen.

Custom Controls MÜSSEN dieselben grundlegenden Verträge wie Standard Controls erfüllen.

Eine Erweiterung DARF keine direkten Zugriffe auf geschützte Grafik- oder Eingaberessourcen erzwingen.

## 13. NovaStudio-Integration

NovaStudio verwendet das Control Framework für den visuellen UI-Designer.

Dabei gilt:

- Controls können über eine Toolbox eingefügt werden.
- Eigenschaften können über einen Property Editor bearbeitet werden.
- Ereignisse können mit NovaLang oder Logic Graph verbunden werden.
- Styles und Templates können visuell bearbeitet werden.
- Änderungen werden in das deklarative UI-Modell übernommen.

Designer und Runtime MÜSSEN dieselben Control-Definitionen verwenden können.

Die Vorschau DARF keine unkontrollierten Seiteneffekte ausführen.

## 14. Performance und Ressourcen

Das Control Framework verwendet die Ressourcenverwaltung der Nova UI Runtime.

Dabei gilt:

- Controls werden nur bei Bedarf erstellt.
- Nicht benötigte Ressourcen werden freigegeben.
- Änderungen lösen gezielte Invalidierungen aus.
- Unveränderte Controls verursachen keine unnötigen Renderoperationen.
- Große Datenmengen SOLLEN durch Virtualisierung unterstützt werden.
- Styles und Templates SOLLEN gemeinsam nutzbare Ressourcen wiederverwenden.

Controls DÜRFEN keine permanente Aktualisierungsschleife benötigen.

## 15. Sicherheit und Isolation

Controls unterliegen Nova.Trust.

Dabei gilt:

- Controls erhalten keine impliziten Systemberechtigungen.
- Geschützte Funktionen werden ausschließlich über autorisierte Capabilities aufgerufen.
- Benutzerdefinierte Controls MÜSSEN innerhalb ihres Sicherheitskontexts ausgeführt werden.
- Eingaben und Datenbindungen MÜSSEN validiert werden können.
- Fehler einzelner Controls DÜRFEN andere UI-Instanzen nicht beeinträchtigen.

Die visuelle Darstellung eines Controls DARF keine Berechtigungsprüfung ersetzen.

## 16. Normative Anforderungen

- **UI-001:** NovaOS MUSS ein natives Control Framework bereitstellen.
- **UI-002:** Controls MÜSSEN auf einem einheitlichen Objektmodell basieren.
- **UI-003:** Darstellung, Verhalten und Anwendungslogik MÜSSEN getrennt sein.
- **UI-004:** Controls MÜSSEN das Nova UI Property System verwenden.
- **UI-005:** Controls MÜSSEN Ereignisse bereitstellen können.
- **UI-006:** Datenbindungen MÜSSEN unterstützt werden.
- **UI-007:** Styles und Control Templates MÜSSEN unterstützt werden.
- **UI-008:** Visuelle Zustände MÜSSEN definiert und verarbeitet werden können.
- **UI-009:** Benutzerdefinierte Controls MÜSSEN unterstützt werden.
- **UI-010:** Controls MÜSSEN mit der Nova Layout Engine zusammenarbeiten.
- **UI-011:** Controls MÜSSEN unabhängig vom Renderbackend funktionieren.
- **UI-012:** Änderungen MÜSSEN gezielte Layout- oder Renderinvalidierungen auslösen.
- **UI-013:** Controls MÜSSEN den Nova UI Component Lifecycle einhalten.
- **UI-014:** Tastaturbedienung und Barrierefreiheit MÜSSEN unterstützt werden können.
- **UI-015:** NovaStudio MUSS Control-Eigenschaften und Ereignisse aus den Control-Definitionen ermitteln können.
- **UI-016:** Fehlerhafte Controls MÜSSEN kontrolliert behandelt werden.
- **UI-017:** Controls MÜSSEN die Sicherheitsregeln von Nova.Trust einhalten.
- **UI-018:** Das Nova Control Framework MUSS vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 17. Integration

- **Nova UI Architecture:** Übergeordnete UI-Architektur.
- **Nova UI Runtime:** Verwaltung der Control-Instanzen.
- **Nova UI Property System:** Eigenschaften und Änderungsbenachrichtigungen.
- **Nova UI Event System:** Ereignisverarbeitung.
- **Nova UI Data Binding:** Verbindung zwischen Controls und Daten.
- **Nova UI Layout Engine:** Größen- und Positionsberechnung.
- **Nova UI Style Engine:** Erscheinungsbild.
- **Nova UI Template Engine:** Visuelle Control-Struktur.
- **Nova UI Renderer Abstraction:** Grafische Darstellung.
- **Nova UI Component Lifecycle:** Erstellung und Freigabe.
- **Nova Input Architecture:** Eingabeverarbeitung.
- **Nova UI Accessibility:** Barrierefreiheit.
- **NovaLang:** Programmatische Control-Verwendung.
- **NovaStudio:** Visueller Designer und Ereignisbindung.
- **Nova Logic Graph:** Visuelle Programmierung.
- **Nova.Trust:** Berechtigungen und Isolation.

## 18. Ergebnis

NovaOS erhält ein natives, erweiterbares und einheitliches Control Framework.

Standard Controls und benutzerdefinierte Steuerelemente verwenden dieselben grundlegenden Eigenschaften, Ereignisse und Lebenszyklusregeln.

Durch die Trennung von Darstellung, Verhalten und Anwendungslogik können Controls unabhängig gestaltet, erweitert und wiederverwendet werden.

**Grundsatz:** Ein Nova Control definiert, was es kann und wie es sich verhält. Styles und Templates bestimmen, wie es aussieht. Die Anwendung bestimmt, was damit geschieht.