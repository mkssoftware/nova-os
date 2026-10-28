# NPSPEC-UI-STYLE-0001 – Nova Styling System

**Status:** Angenommen  
**Bereich:** UI / Styling  
**System:** NovaOS

## 1. Zweck

Nova Styling System definiert die einheitliche Gestaltung nativer UI-Elemente innerhalb von NovaOS.

Ziel ist die Trennung von Erscheinungsbild, visueller Struktur und funktionalem Verhalten.

Styles ermöglichen ein konsistentes, anpassbares und ressourcenschonendes Design für Standard Controls, Custom Controls und vollständige Benutzeroberflächen.

Das Styling System arbeitet unabhängig vom verwendeten Renderbackend.

## 2. Architektur

Das Subsystem umfasst folgende Komponenten:

| Komponente | Aufgabe |
|---|---|
| Style Manager | Zentrale Verwaltung und Anwendung von Styles |
| Style Registry | Registrierung verfügbarer Styles |
| Style Resolver | Ermittlung des gültigen Styles |
| Style Property Resolver | Berechnung effektiver Eigenschaftswerte |
| Style Inheritance Manager | Verwaltung von Style-Vererbungen |
| Style Trigger Engine | Verarbeitung bedingter Style-Regeln |
| Style Resource Resolver | Auflösung referenzierter Designressourcen |
| Style Cache | Wiederverwendung aufgelöster Styles |

Das Styling System verwendet das Nova UI Property System und die Nova Control Templates.

## 3. Style-Modell

Ein Style beschreibt die Gestaltung eines bestimmten UI-Elementtyps.

Ein Style kann enthalten:

- Zieltyp.
- Eindeutigen Schlüssel.
- Eigenschaftswerte.
- Ressourcenreferenzen.
- Zustandsabhängige Regeln.
- Trigger.
- Template-Zuweisungen.
- Vererbungsinformationen.

Styles DÜRFEN keine eigenständige Anwendungslogik ausführen.

Die funktionale Bedeutung eines Controls bleibt unabhängig vom angewendeten Style.

## 4. Deklarative Definition

Styles werden über das Nova Declarative UI Model definiert.

Beispiel:

```xml
<Style Key="PrimaryButton"
       TargetType="Button">

    <Setter Property="Background"
            Value="{Resource AccentBrush}" />

    <Setter Property="Foreground"
            Value="{Resource PrimaryTextBrush}" />

    <Setter Property="Padding"
            Value="16,8" />

    <Setter Property="CornerRadius"
            Value="6" />

    <Setter Property="FontSize"
            Value="14" />

</Style>
```

Verwendung:

```xml
<Button Content="Speichern"
        Style="{Resource PrimaryButton}" />
```

Die Style-Definition MUSS unabhängig von der Control-Implementierung verarbeitet werden können.

## 5. Style-Auflösung

Styles können auf unterschiedlichen Ebenen definiert werden:

1. Direkt am UI-Element.
2. Innerhalb lokaler Ressourcen.
3. Innerhalb übergeordneter UI-Container.
4. Innerhalb anwendungsweiter Ressourcen.
5. Innerhalb des aktiven NovaOS-Designs.
6. Als Standard-Style des Elementtyps.

Die Auflösung MUSS einer eindeutig definierten Prioritätsreihenfolge folgen.

Explizite lokale Eigenschaftswerte besitzen grundsätzlich Vorrang vor Style-Settern.

Ausnahmen MÜSSEN durch das Nova UI Property System ausdrücklich definiert werden.

## 6. Implizite und explizite Styles

Nova Styling System unterstützt zwei grundlegende Style-Arten.

### Explizite Styles

Werden über einen eindeutigen Schlüssel referenziert.

### Implizite Styles

Werden automatisch anhand des Zieltyps angewendet.

Beispiel:

```xml
<Style TargetType="TextBox">

    <Setter Property="Padding"
            Value="8" />

    <Setter Property="FontSize"
            Value="14" />

</Style>
```

Implizite Styles MÜSSEN innerhalb ihres gültigen Ressourcenbereichs automatisch aufgelöst werden können.

## 7. Style-Vererbung

Styles können auf anderen Styles aufbauen.

Beispiel:

```xml
<Style Key="BaseButton"
       TargetType="Button">

    <Setter Property="Padding"
            Value="12,6" />

    <Setter Property="CornerRadius"
            Value="6" />

</Style>

<Style Key="PrimaryButton"
       TargetType="Button"
       BasedOn="{Resource BaseButton}">

    <Setter Property="Background"
            Value="{Resource AccentBrush}" />

</Style>
```

Dabei gilt:

- Abgeleitete Styles übernehmen gültige Eigenschaften des Basis-Styles.
- Eigene Setter können geerbte Setter überschreiben.
- Zieltypen MÜSSEN kompatibel sein.
- Zyklische Style-Vererbungen MÜSSEN erkannt werden.

Die Vererbung MUSS deterministisch aufgelöst werden.

## 8. Style Trigger

Styles unterstützen bedingte Eigenschaftsänderungen.

Auslöser können sein:

- Control-Zustände.
- Eigenschaftswerte.
- Datenbindungen.
- Fokusänderungen.
- Eingabezustände.
- Adaptive Layoutbedingungen.

Beispiel:

```xml
<Style TargetType="Button">

    <Setter Property="Opacity"
            Value="1.0" />

    <Style.Triggers>

        <Trigger Property="IsEnabled"
                 Value="False">

            <Setter Property="Opacity"
                    Value="0.4" />

        </Trigger>

    </Style.Triggers>

</Style>
```

Trigger MÜSSEN ereignisgesteuert ausgewertet werden.

Konkurrierende Trigger MÜSSEN nach definierten Prioritätsregeln aufgelöst werden.

## 9. Styles und Control States

Das Styling System verwendet die Zustandsinformationen aus Nova Control States.

Dabei gilt:

- Funktionale Zustände werden vom Control verwaltet.
- Styles definieren zustandsabhängige Eigenschaftswerte.
- Templates definieren die visuelle Struktur.
- Animationen können Zustandsübergänge darstellen.

Ein Style DARF funktionale Einschränkungen eines Controls nicht umgehen.

Ein deaktiviertes Control bleibt unabhängig von seiner visuellen Darstellung deaktiviert.

## 10. Styles und Templates

Styles können Control Templates zuweisen.

Beispiel:

```xml
<Style TargetType="Button">

    <Setter Property="Template"
            Value="{Resource ModernButtonTemplate}" />

</Style>
```

Dabei gilt:

- Styles bestimmen die anzuwendenden Gestaltungseigenschaften.
- Templates bestimmen die visuelle Struktur.
- Templates können Style-Ressourcen verwenden.
- Ein Stylewechsel DARF keine vollständige Neuerstellung der funktionalen Control-Instanz erfordern.

Styles und Templates MÜSSEN unabhängig voneinander austauschbar bleiben.

## 11. Designressourcen

Styles können gemeinsam genutzte Ressourcen referenzieren.

Dazu gehören:

- Farben.
- Brushes.
- Schriftarten.
- Schriftgrößen.
- Abstände.
- Rahmen.
- Rundungen.
- Schatten.
- Animationseinstellungen.
- Control Templates.

Ressourcen MÜSSEN über das Nova UI Resource System aufgelöst werden.

Gemeinsam verwendete Ressourcen SOLLEN speichereffizient bereitgestellt werden.

## 12. Dynamische Styleänderungen

Styles können während der Laufzeit geändert werden.

Beispiele:

- Wechsel zwischen Hell- und Dunkeldesign.
- Änderung der Akzentfarbe.
- Anpassung der Schriftgröße.
- Änderung von Benutzerpräferenzen.
- Wechsel des Control-Designs.
- Aktivierung von Barrierefreiheitseinstellungen.

Dabei gilt:

- Änderungen werden gezielt an betroffene Elemente weitergegeben.
- Unveränderte Style-Eigenschaften bleiben erhalten.
- Layoutinvalidierungen erfolgen nur bei geometrisch relevanten Änderungen.
- Reine Darstellungsänderungen erzeugen ausschließlich notwendige Renderinvalidierungen.

Ein Stylewechsel SOLL ohne Neustart der Anwendung möglich sein.

## 13. Benutzeranpassung

Nova Styling System unterstützt benutzerdefinierte Gestaltungseinstellungen.

Dazu gehören:

- Designauswahl.
- Akzentfarben.
- Kontrast.
- Schriftgrößen.
- Transparenz.
- Animationen.
- Individuelle Control-Styles.

Benutzerpräferenzen SOLLEN gegenüber automatischen Designempfehlungen Vorrang besitzen.

Anwendungen DÜRFEN sicherheits- oder barrierefreiheitsrelevante Einstellungen nicht unzulässig überschreiben.

## 14. Performance und Caching

Nova Styling System arbeitet bedarfsorientiert.

Dabei gilt:

- Style-Definitionen SOLLEN gemeinsam verwendet werden.
- Aufgelöste Styles SOLLEN zwischengespeichert werden.
- Unveränderte Eigenschaften werden nicht erneut ausgewertet.
- Trigger werden nur bei relevanten Änderungen verarbeitet.
- Mehrere Styleänderungen SOLLEN zusammengefasst werden.
- Ungültige Cacheeinträge werden gezielt entfernt.

Styles DÜRFEN keine permanente Aktualisierungsschleife benötigen.

## 15. NovaStudio-Integration

NovaStudio unterstützt die visuelle Bearbeitung von Styles.

Dazu gehören:

- Erstellung und Bearbeitung von Styles.
- Auswahl des Zieltyps.
- Bearbeitung von Settern.
- Definition von Triggern.
- Verwaltung von Style-Vererbungen.
- Auswahl von Ressourcen.
- Vorschau unterschiedlicher Control-Zustände.
- Bearbeitung globaler und lokaler Designs.

Designer und Runtime MÜSSEN dieselben Style-Definitionen verwenden können.

## 16. Sicherheit

Nova Styling System unterliegt Nova.Trust.

Dabei gilt:

- Styles erhalten keine zusätzlichen Systemberechtigungen.
- Style-Definitionen MÜSSEN validiert werden.
- Ressourcenreferenzen MÜSSEN bestehende Zugriffsregeln einhalten.
- Styles DÜRFEN keine unkontrollierte Codeausführung ermöglichen.
- Fehlerhafte Styles DÜRFEN andere UI-Instanzen nicht beeinträchtigen.
- Sicherheitsrelevante Funktionen DÜRFEN nicht allein durch visuelle Eigenschaften durchgesetzt werden.

## 17. Normative Anforderungen

- **UI-001:** NovaOS MUSS ein natives Styling System bereitstellen.
- **UI-002:** Styles MÜSSEN unabhängig von funktionalem Control-Verhalten definiert werden können.
- **UI-003:** Styles MÜSSEN deklarativ beschrieben werden können.
- **UI-004:** Explizite und implizite Styles MÜSSEN unterstützt werden.
- **UI-005:** Styles MÜSSEN Eigenschaften über Setter definieren können.
- **UI-006:** Style-Vererbung MUSS unterstützt werden.
- **UI-007:** Zyklische Style-Vererbungen MÜSSEN erkannt werden.
- **UI-008:** Zustands- und eigenschaftsabhängige Trigger MÜSSEN unterstützt werden.
- **UI-009:** Style-Konflikte MÜSSEN deterministisch aufgelöst werden.
- **UI-010:** Styles MÜSSEN Control Templates zuweisen können.
- **UI-011:** Gemeinsam verwendete Designressourcen MÜSSEN unterstützt werden.
- **UI-012:** Dynamische Styleänderungen MÜSSEN möglich sein.
- **UI-013:** Änderungen MÜSSEN gezielte Layout- oder Renderinvalidierungen auslösen.
- **UI-014:** Gültige Style-Ergebnisse SOLLEN zwischengespeichert werden.
- **UI-015:** Benutzerdefinierte Designpräferenzen MÜSSEN unterstützt werden.
- **UI-016:** Styles MÜSSEN unabhängig vom Renderbackend funktionieren.
- **UI-017:** NovaStudio MUSS Styles visuell bearbeiten können.
- **UI-018:** Fehlerhafte Style-Definitionen MÜSSEN kontrolliert behandelt werden.
- **UI-019:** Styles MÜSSEN die Sicherheitsregeln von Nova.Trust einhalten.
- **UI-020:** Nova Styling System MUSS vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 18. Integration

- **Nova UI Architecture:** Übergeordnete UI-Architektur.
- **Nova UI Runtime:** Verwaltung und Anwendung von Styles.
- **Nova Declarative UI Model:** Deklarative Style-Definitionen.
- **Nova UI Property System:** Eigenschaften und Prioritätsauflösung.
- **Nova Control Framework:** Styling nativer Controls.
- **Nova Control Templates:** Visuelle Control-Struktur.
- **Nova Control States:** Zustandsabhängige Darstellung.
- **Nova UI Resource System:** Gemeinsam genutzte Designressourcen.
- **Nova UI Data Binding:** Datenabhängige Style-Regeln.
- **Nova Layout Engine:** Geometrische Auswirkungen.
- **Nova UI Renderer Abstraction:** Grafische Darstellung.
- **Nova UI Animation Engine:** Animierte Zustandsübergänge.
- **Nova UI Accessibility:** Barrierefreiheit und Kontrast.
- **NovaStudio:** Visueller Style-Designer.
- **Nova.Trust:** Sicherheit und Isolation.

## 19. Ergebnis

NovaOS erhält ein natives, deklaratives und erweiterbares Styling System.

Standard Controls und Custom Controls können einheitlich gestaltet und individuell angepasst werden, ohne ihre funktionale Implementierung zu verändern.

Durch Style-Vererbung, Ressourcen, Trigger und gezielte Invalidierungen bleibt das System flexibel und ressourcenschonend.

**Grundsatz:** Nova Styling System bestimmt das Erscheinungsbild einer Benutzeroberfläche, ohne deren Funktionalität oder visuelle Grundstruktur festzulegen.