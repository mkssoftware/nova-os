# NPSPEC-UI-CONTROL-0005 – Nova Control States

**Status:** Angenommen  
**Bereich:** UI / Controls / States  
**System:** NovaOS

## 1. Zweck

Nova Control States definiert die Verwaltung funktionaler und visueller Zustände nativer UI-Steuerelemente.

Ziel ist ein einheitliches, deterministisches und ressourcenschonendes Zustandsmodell, das Änderungen des Control-Verhaltens und der Darstellung zuverlässig verarbeitet.

Die funktionale Zustandsverwaltung bleibt unabhängig von Styles, Templates und Renderbackend.

## 2. Architektur

Das Subsystem umfasst folgende Komponenten:

| Komponente | Aufgabe |
|---|---|
| Control State Manager | Verwaltung aktueller Control-Zustände |
| State Definition Registry | Registrierung verfügbarer Zustände |
| State Group Manager | Verwaltung zusammengehöriger Zustände |
| State Transition Controller | Verarbeitung von Zustandswechseln |
| State Trigger Resolver | Auswertung zustandsauslösender Bedingungen |
| State Notification Dispatcher | Benachrichtigung abhängiger UI-Komponenten |

Nova Control States verwendet das Nova UI Property System und das Nova UI Event System.

## 3. Zustandsmodell

Ein Control kann mehrere Zustände gleichzeitig besitzen.

Zustände werden in logische Gruppen unterteilt.

| Zustandsgruppe | Mögliche Zustände |
|---|---|
| CommonStates | Normal, PointerOver, Pressed, Disabled |
| FocusStates | Focused, Unfocused |
| SelectionStates | Selected, Unselected |
| CheckStates | Checked, Unchecked, Indeterminate |
| ValidationStates | Valid, Invalid |
| ExpansionStates | Expanded, Collapsed |
| LoadingStates | Idle, Loading, Completed, Failed |

Innerhalb einer exklusiven Zustandsgruppe DARF jeweils nur ein Zustand aktiv sein.

Unterschiedliche Zustandsgruppen DÜRFEN gleichzeitig aktiv sein.

Nicht jeder Control-Typ muss sämtliche Zustandsgruppen unterstützen.

## 4. Funktionale und visuelle Zustände

Nova UI unterscheidet zwischen funktionalem Zustand und visueller Zustandsdarstellung.

**Funktionaler Zustand:**

Beschreibt den tatsächlichen Zustand eines Controls, beispielsweise `IsEnabled`, `IsChecked` oder `IsSelected`.

**Visueller Zustand:**

Beschreibt die Darstellung eines funktionalen oder interaktionsbedingten Zustands.

Dabei gilt:

- Funktionale Zustände werden durch das Control verwaltet.
- Visuelle Zustände werden über Styles und Templates dargestellt.
- Ein Template DARF funktionale Zustände nicht eigenständig verändern.
- Fehlende visuelle Zustandsdefinitionen DÜRFEN die Funktion eines Controls nicht beeinträchtigen.

## 5. Zustandsauslöser

Zustandswechsel können durch unterschiedliche Ereignisse ausgelöst werden.

Dazu gehören:

- Maus- und Touchinteraktionen.
- Tastatureingaben.
- Fokusänderungen.
- Eigenschaftsänderungen.
- Datenbindungen.
- Validierungsergebnisse.
- Programmatische Zustandsänderungen.
- Änderungen des Control-Lebenszyklus.

Alle Zustandsauslöser MÜSSEN über definierte Schnittstellen verarbeitet werden.

## 6. Zustandsübergänge

Ein Zustandsübergang beschreibt den Wechsel von einem Zustand in einen anderen.

Dabei gilt:

1. Der Auslöser wird verarbeitet.
2. Die Gültigkeit des Zustandswechsels wird geprüft.
3. Der funktionale Zustand wird aktualisiert, sofern erforderlich.
4. Abhängige Zustandsgruppen werden ausgewertet.
5. Änderungsbenachrichtigungen werden ausgelöst.
6. Die visuelle Darstellung wird gezielt aktualisiert.

Zustandswechsel MÜSSEN deterministisch erfolgen.

Ungültige Übergänge MÜSSEN kontrolliert behandelt werden.

## 7. Zustandsprioritäten

Mehrere Bedingungen können gleichzeitig zutreffen.

Beispielsweise kann ein Button gleichzeitig fokussiert, deaktiviert und vom Mauszeiger überlagert sein.

Dabei gilt:

- Zustandsgruppen werden unabhängig verwaltet.
- Innerhalb einer exklusiven Gruppe MUSS ein eindeutiger Zustand bestimmt werden.
- Konflikte zwischen visuellen Eigenschaften MÜSSEN durch definierte Prioritätsregeln aufgelöst werden.
- Sicherheitsrelevante und funktionale Einschränkungen besitzen Vorrang vor rein visuellen Zuständen.

Ein deaktiviertes Control DARF durch einen visuellen Zustand nicht wieder bedienbar werden.

## 8. Deklarative Zustandsdefinition

Visuelle Zustände können innerhalb eines Control Templates definiert werden.

Beispiel:

```xml
<ControlTemplate TargetType="Button">

    <Border Name="ButtonBorder"
            Background="{TemplateBinding Background}"
            CornerRadius="6">

        <ContentPresenter />

    </Border>

    <ControlTemplate.VisualStates>

        <VisualStateGroup Name="CommonStates">

            <VisualState Name="Normal" />

            <VisualState Name="PointerOver">
                <Setter Target="ButtonBorder.Opacity"
                        Value="1.0" />
            </VisualState>

            <VisualState Name="Pressed">
                <Setter Target="ButtonBorder.Opacity"
                        Value="0.8" />
            </VisualState>

            <VisualState Name="Disabled">
                <Setter Target="ButtonBorder.Opacity"
                        Value="0.4" />
            </VisualState>

        </VisualStateGroup>

    </ControlTemplate.VisualStates>

</ControlTemplate>
```

Die Definition beschreibt ausschließlich die visuelle Reaktion auf Zustandsänderungen.

Die funktionale Zustandsverwaltung erfolgt weiterhin durch das Control.

## 9. Zustandsanimationen

Zustandsübergänge können animiert werden.

Dabei gilt:

- Animationen sind optional.
- Zustandswechsel MÜSSEN auch ohne Animation funktionieren.
- Animationen DÜRFEN den funktionalen Zustandswechsel nicht verzögern.
- Laufende Animationen MÜSSEN unterbrechbar sein.
- Neue Zustände MÜSSEN veraltete visuelle Übergänge ersetzen können.
- Barrierefreiheitseinstellungen MÜSSEN berücksichtigt werden.

Die Animation Engine verarbeitet ausschließlich die visuelle Übergangsdarstellung.

## 10. Benutzerdefinierte Zustände

Custom Controls können eigene Zustandsgruppen und Zustände definieren.

Beispiele:

- Recording.
- Paused.
- Connected.
- Disconnected.
- Warning.
- Error.

Dabei gilt:

- Zustände MÜSSEN eindeutig identifizierbar sein.
- Zustandsgruppen MÜSSEN registriert werden können.
- Exklusive und unabhängige Zustände MÜSSEN unterscheidbar sein.
- Benutzerdefinierte Zustände MÜSSEN mit Styles und Templates zusammenarbeiten.

Custom Controls DÜRFEN bestehende Zustandsverträge nicht unzulässig verändern.

## 11. Datenbindung und Ereignisse

Control-Zustände können mit dem Nova UI Property System und Data Binding verbunden werden.

Dabei gilt:

- Zustandsrelevante Eigenschaften können gebunden werden.
- Eigenschaftsänderungen können Zustandswechsel auslösen.
- Zustandsänderungen können Ereignisse erzeugen.
- Mehrere zusammengehörige Änderungen SOLLEN konsistent verarbeitet werden.
- Zyklische Zustandsänderungen MÜSSEN erkannt und begrenzt werden.

Ein Zustandswechsel DARF keine unbegrenzte Ereigniskette verursachen.

## 12. Lebenszyklus

Control States berücksichtigt den Nova UI Component Lifecycle.

Dabei gilt:

- Initialzustände werden während der Initialisierung bestimmt.
- Beim Anhängen eines Controls werden gültige Zustände wiederhergestellt.
- Während der Suspendierung bleiben funktionale Zustände erhalten, sofern der Control-Vertrag dies vorsieht.
- Beim Entfernen werden temporäre Zustandsbindungen kontrolliert gelöst.
- Beim Entsorgen werden Zustandsressourcen freigegeben.

Visuelle Zustände können nach einem Templatewechsel erneut angewendet werden.

## 13. Performance

Nova Control States arbeitet ereignisgesteuert.

Dabei gilt:

- Zustände werden nur bei relevanten Änderungen neu ausgewertet.
- Unveränderte Zustände verursachen keine zusätzlichen Aktualisierungen.
- Mehrere Zustandsänderungen SOLLEN zusammengefasst werden.
- Nur betroffene visuelle Eigenschaften werden invalidiert.
- Zustandsanimationen werden über die Nova UI Animation Engine verwaltet.

Eine permanente Zustandsabfrage ist nicht erforderlich.

## 14. NovaStudio-Integration

NovaStudio unterstützt die Bearbeitung und Vorschau von Control-Zuständen.

Dazu gehören:

- Anzeige verfügbarer Zustandsgruppen.
- Auswahl einzelner Zustände.
- Bearbeitung zustandsabhängiger Styles.
- Definition von Zustandsanimationen.
- Vorschau kombinierter Zustände.
- Validierung von Zustandsübergängen.

Designer und Runtime MÜSSEN dieselben Zustandsdefinitionen verwenden können.

## 15. Sicherheit

Control States unterliegt Nova.Trust.

Dabei gilt:

- Visuelle Zustände DÜRFEN keine Berechtigungen verändern.
- Sicherheitsrelevante Einschränkungen MÜSSEN unabhängig von der Darstellung durchgesetzt werden.
- Zustandsänderungen MÜSSEN innerhalb des autorisierten UI-Kontexts erfolgen.
- Benutzerdefinierte Zustände DÜRFEN keine geschützten Systemfunktionen umgehen.
- Fehlerhafte Zustandsdefinitionen DÜRFEN andere UI-Instanzen nicht beeinträchtigen.

Ein visuell aktiviertes Control besitzt nicht automatisch die Berechtigung zur Ausführung einer Aktion.

## 16. Normative Anforderungen

- **UI-001:** NovaOS MUSS ein einheitliches Control-State-System bereitstellen.
- **UI-002:** Funktionale und visuelle Zustände MÜSSEN getrennt verwaltet werden.
- **UI-003:** Zustände MÜSSEN in logische Gruppen unterteilt werden können.
- **UI-004:** Exklusive Zustandsgruppen MÜSSEN unterstützt werden.
- **UI-005:** Mehrere unabhängige Zustandsgruppen MÜSSEN gleichzeitig aktiv sein können.
- **UI-006:** Zustandswechsel MÜSSEN deterministisch erfolgen.
- **UI-007:** Zustandsauslöser MÜSSEN über definierte Schnittstellen verarbeitet werden.
- **UI-008:** Zustandskonflikte MÜSSEN eindeutig aufgelöst werden.
- **UI-009:** Visuelle Zustände MÜSSEN deklarativ definiert werden können.
- **UI-010:** Zustandsanimationen MÜSSEN optional unterstützt werden.
- **UI-011:** Benutzerdefinierte Zustände MÜSSEN unterstützt werden.
- **UI-012:** Zustände MÜSSEN mit dem Nova UI Property System zusammenarbeiten.
- **UI-013:** Zustandsänderungen MÜSSEN gezielte Renderinvalidierungen auslösen.
- **UI-014:** Unveränderte Zustände DÜRFEN keine unnötigen Aktualisierungen verursachen.
- **UI-015:** Zyklische Zustandsänderungen MÜSSEN kontrolliert behandelt werden.
- **UI-016:** Control States MUSS unabhängig vom Renderbackend funktionieren.
- **UI-017:** NovaStudio MUSS Zustandsdefinitionen bearbeiten und darstellen können.
- **UI-018:** Zustandsänderungen MÜSSEN die Sicherheitsregeln von Nova.Trust einhalten.
- **UI-019:** Nova Control States MUSS vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 17. Integration

- **Nova Control Framework:** Funktionale Control-Architektur.
- **Nova Standard Controls:** Standardzustände.
- **Nova Custom Controls:** Benutzerdefinierte Zustände.
- **Nova Control Templates:** Visuelle Zustandsdarstellung.
- **Nova UI Property System:** Zustandsrelevante Eigenschaften.
- **Nova UI Event System:** Zustandsauslöser und Benachrichtigungen.
- **Nova UI Data Binding:** Zustandsbindungen.
- **Nova UI Style Engine:** Zustandsabhängige Darstellung.
- **Nova UI Animation Engine:** Animierte Zustandsübergänge.
- **Nova UI Renderer Abstraction:** Grafische Aktualisierung.
- **Nova UI Component Lifecycle:** Initialisierung und Freigabe.
- **Nova Input Architecture:** Interaktionszustände.
- **Nova UI Accessibility:** Zugängliche Zustandsinformationen.
- **NovaStudio:** Zustandseditor und Vorschau.
- **Nova.Trust:** Sicherheit und Isolation.

## 18. Ergebnis

NovaOS erhält ein einheitliches, deterministisches und erweiterbares Zustandsmodell für native UI-Steuerelemente.

Funktionale Zustände bleiben unabhängig von ihrer visuellen Darstellung und können durch Styles, Templates und Animationen unterschiedlich präsentiert werden.

Durch ereignisgesteuerte Verarbeitung und gezielte Invalidierungen werden unnötige Aktualisierungen vermieden.

**Grundsatz:** Ein Nova Control verwaltet seinen tatsächlichen Zustand selbst. Styles und Templates bestimmen ausschließlich, wie dieser Zustand dargestellt wird.