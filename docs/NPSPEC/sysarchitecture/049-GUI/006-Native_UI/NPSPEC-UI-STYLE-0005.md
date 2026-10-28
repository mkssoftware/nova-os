# NPSPEC-UI-STYLE-0005 – Nova Visual State Styling

**Status:** Angenommen  
**Bereich:** UI / Styling / Visual States  
**System:** NovaOS

## 1. Zweck

Nova Visual State Styling definiert die zustandsabhängige Gestaltung nativer UI-Elemente.

Ziel ist die einheitliche Darstellung von Interaktions-, Fokus-, Auswahl- und Funktionszuständen, ohne das Verhalten eines Controls zu verändern.

Das System verbindet Nova Control States mit Styles, Control Templates und Animationen.

## 2. Architektur

Nova Visual State Styling umfasst:

| Komponente | Aufgabe |
|---|---|
| Visual State Style Resolver | Ermittlung zustandsabhängiger Style-Werte |
| State Style Registry | Verwaltung von Zustandsdefinitionen |
| State Priority Resolver | Auflösung konkurrierender Style-Regeln |
| State Transition Styler | Koordination visueller Zustandsübergänge |
| State Property Tracker | Ermittlung betroffener Eigenschaften |

Die Verwaltung funktionaler Zustände verbleibt bei Nova Control States.

## 3. Zustandsabhängige Darstellung

Ein Control kann unterschiedliche visuelle Darstellungen besitzen.

Typische Zustände:

- Normal.
- PointerOver.
- Pressed.
- Focused.
- Disabled.
- Selected.
- Checked.
- Indeterminate.
- ValidationError.
- Loading.

Mehrere unabhängige Zustände können gleichzeitig aktiv sein.

Die Darstellung MUSS anhand definierter Prioritätsregeln berechnet werden.

## 4. Deklarative Definition

Zustandsabhängige Styles werden über das Nova Declarative UI Model definiert.

Beispiel:

```xml
<Style TargetType="Button">

    <Setter Property="Opacity"
            Value="1.0" />

    <Style.VisualStates>

        <VisualState Name="PointerOver">
            <Setter Property="Opacity"
                    Value="1.0" />
        </VisualState>

        <VisualState Name="Pressed">
            <Setter Property="Opacity"
                    Value="0.85" />
        </VisualState>

        <VisualState Name="Disabled">
            <Setter Property="Opacity"
                    Value="0.4" />
        </VisualState>

    </Style.VisualStates>

</Style>
```

Die Definition verändert ausschließlich die visuelle Darstellung.

## 5. Zustandsprioritäten

Treffen mehrere visuelle Zustände auf dieselbe Eigenschaft zu, MUSS der Konflikt deterministisch aufgelöst werden.

Dabei gilt:

- Explizite Prioritäten können definiert werden.
- Zustandsgruppen bleiben logisch unabhängig.
- Innerhalb exklusiver Gruppen ist nur ein Zustand aktiv.
- Sicherheits- und Barrierefreiheitsanforderungen besitzen Vorrang vor dekorativen Effekten.
- Gleichrangige Konflikte werden anhand einer festgelegten Reihenfolge aufgelöst.

Ein visueller Zustand DARF funktionale Einschränkungen nicht außer Kraft setzen.

## 6. Zustandsübergänge

Zustandswechsel können unmittelbar oder animiert dargestellt werden.

Unterstützte Übergänge können sein:

- Farbänderungen.
- Transparenzänderungen.
- Skalierung.
- Positionsänderungen.
- Schatten.
- Blur-Effekte.

Animationen sind optional.

Der funktionale Zustandswechsel MUSS unabhängig von der Dauer einer Animation erfolgen.

## 7. Dynamische Style-Aktualisierung

Ändert sich ein Control-Zustand, werden ausschließlich betroffene Style-Eigenschaften neu berechnet.

Dabei gilt:

- Unveränderte Eigenschaften bleiben erhalten.
- Reine Farbänderungen lösen keine unnötigen Layoutberechnungen aus.
- Geometrische Änderungen invalidieren das erforderliche Layout.
- Visuelle Änderungen erzeugen gezielte Renderinvalidierungen.
- Mehrere Änderungen SOLLEN zusammengefasst werden.

Eine permanente Neuberechnung ist nicht erforderlich.

## 8. Theme-Integration

Visual State Styling verwendet Ressourcen des aktiven Themes.

Dabei gilt:

- Zustandsfarben können über semantische Ressourcen definiert werden.
- Themewechsel aktualisieren betroffene Zustandsdarstellungen.
- Funktionale Control-Zustände bleiben beim Themewechsel erhalten.
- Fehlende Zustandsressourcen werden über definierte Fallbacks behandelt.

Die Darstellung MUSS mit Light-, Dark- und High-Contrast-Themes zusammenarbeiten.

## 9. Benutzerdefinierte Zustände

Custom Controls können eigene visuelle Zustände definieren.

Beispiele:

- Connected.
- Disconnected.
- Recording.
- Paused.
- Warning.
- Error.

Benutzerdefinierte Zustände MÜSSEN über Nova Control States registriert werden können.

Ihre Darstellung erfolgt über Styles und Templates.

## 10. Barrierefreiheit

Visual State Styling berücksichtigt die Nova UI Accessibility Architecture.

Dabei gilt:

- Fokuszustände MÜSSEN erkennbar dargestellt werden können.
- Wichtige Zustände DÜRFEN nicht ausschließlich durch Farbunterschiede vermittelt werden.
- Kontrasteinstellungen MÜSSEN berücksichtigt werden.
- Reduzierte Animationen MÜSSEN unterstützt werden.
- Transparenz und Blur DÜRFEN die Bedienbarkeit nicht beeinträchtigen.

Benutzerdefinierte Styles DÜRFEN verbindliche Barrierefreiheitsanforderungen nicht umgehen.

## 11. Performance

Nova Visual State Styling arbeitet ereignisgesteuert.

Dabei gilt:

- Zustandsabhängige Style-Ergebnisse SOLLEN zwischengespeichert werden.
- Änderungen invalidieren ausschließlich betroffene Eigenschaften.
- Unveränderte Controls verursachen keine zusätzlichen Renderoperationen.
- Laufende Animationen werden durch die Nova UI Animation Engine verwaltet.
- Nicht benötigte Übergangsressourcen werden freigegeben.

## 12. NovaStudio-Integration

NovaStudio unterstützt:

- Bearbeitung visueller Zustände.
- Vorschau kombinierter Zustände.
- Definition von Zustandsprioritäten.
- Bearbeitung von Übergangsanimationen.
- Prüfung von Theme-Varianten.
- Vorschau von Barrierefreiheitseinstellungen.

Designer und Runtime MÜSSEN dieselben Zustandsregeln verwenden.

## 13. Sicherheit

Visual State Styling unterliegt Nova.Trust.

Dabei gilt:

- Styles DÜRFEN keine funktionalen Berechtigungen verändern.
- Zustandsdefinitionen MÜSSEN validiert werden.
- Nicht vertrauenswürdige Styles DÜRFEN keine unkontrollierte Codeausführung ermöglichen.
- Sicherheitsrelevante Einschränkungen MÜSSEN unabhängig von der Darstellung durchgesetzt werden.

## 14. Normative Anforderungen

- **UI-001:** NovaOS MUSS zustandsabhängiges Styling unterstützen.
- **UI-002:** Funktionale Zustände und visuelle Darstellung MÜSSEN getrennt bleiben.
- **UI-003:** Zustandsabhängige Styles MÜSSEN deklarativ definiert werden können.
- **UI-004:** Mehrere unabhängige Zustände MÜSSEN gleichzeitig berücksichtigt werden können.
- **UI-005:** Style-Konflikte MÜSSEN deterministisch aufgelöst werden.
- **UI-006:** Benutzerdefinierte visuelle Zustände MÜSSEN unterstützt werden.
- **UI-007:** Zustandsübergänge MÜSSEN ohne Animation funktionieren.
- **UI-008:** Animierte Übergänge MÜSSEN optional unterstützt werden.
- **UI-009:** Zustandsänderungen MÜSSEN gezielte Layout- und Renderinvalidierungen auslösen.
- **UI-010:** Themewechsel MÜSSEN bestehende funktionale Zustände erhalten.
- **UI-011:** Barrierefreiheitseinstellungen MÜSSEN berücksichtigt werden.
- **UI-012:** Visual State Styling MUSS unabhängig vom Renderbackend funktionieren.
- **UI-013:** NovaStudio MUSS visuelle Zustände bearbeiten und darstellen können.
- **UI-014:** Zustandsstyles MÜSSEN die Sicherheitsregeln von Nova.Trust einhalten.
- **UI-015:** Nova Visual State Styling MUSS vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 15. Integration

- **Nova Control States:** Funktionale Zustandsverwaltung.
- **Nova Styling System:** Style-Auflösung und Eigenschaften.
- **Nova Control Templates:** Visuelle Control-Struktur.
- **Nova Theme Management:** Themeabhängige Zustandsressourcen.
- **Nova Resource Dictionaries:** Farben, Brushes und Effekte.
- **Nova UI Property System:** Effektive Eigenschaftswerte.
- **Nova UI Animation Engine:** Zustandsübergänge.
- **Nova Layout Engine:** Geometrische Aktualisierung.
- **Nova UI Renderer Abstraction:** Grafische Darstellung.
- **Nova UI Accessibility:** Zugängliche Zustandsdarstellung.
- **NovaStudio:** Zustandseditor und Vorschau.
- **Nova.Trust:** Sicherheit und Isolation.

## 16. Ergebnis

NovaOS erhält ein einheitliches System zur zustandsabhängigen Gestaltung nativer UI-Elemente.

Controls können auf Interaktionen und Zustandsänderungen visuell reagieren, ohne ihre funktionale Implementierung verändern zu müssen.

Durch deterministische Prioritäten, optionale Animationen und gezielte Invalidierungen bleibt die Darstellung konsistent und ressourcenschonend.

**Grundsatz:** Der Zustand bestimmt, was ein Control gerade ist. Visual State Styling bestimmt, wie dieser Zustand für den Benutzer sichtbar wird.