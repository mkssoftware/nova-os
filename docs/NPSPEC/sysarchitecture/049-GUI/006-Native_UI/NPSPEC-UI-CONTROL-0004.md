# NPSPEC-UI-CONTROL-0004 – Nova Control Templates

**Status:** Angenommen  
**Bereich:** UI / Controls / Templates  
**System:** NovaOS

## 1. Zweck

Nova Control Templates definiert die deklarative Beschreibung und Verwaltung der visuellen Struktur nativer UI-Steuerelemente.

Ziel ist die vollständige Trennung zwischen funktionalem Verhalten und visueller Darstellung eines Controls.

Ein Control kann dadurch unterschiedlich gestaltet werden, ohne seine Eigenschaften, Ereignisse oder Anwendungslogik verändern zu müssen.

## 2. Architektur

Das Subsystem umfasst folgende Komponenten:

| Komponente | Aufgabe |
|---|---|
| Template Registry | Registrierung und Auflösung von Templates |
| Template Resolver | Auswahl des anzuwendenden Templates |
| Template Parser | Verarbeitung deklarativer Template-Definitionen |
| Template Instance Manager | Erstellung und Verwaltung visueller Template-Instanzen |
| Template Binding Resolver | Verbindung zwischen Control-Eigenschaften und Template-Elementen |
| Template State Manager | Verarbeitung zustandsabhängiger Darstellungen |

Nova Control Templates verwendet die bestehenden Nova UI Subsysteme und besitzt keine eigene Layout- oder Rendering-Engine.

## 3. Template-Modell

Ein Control Template beschreibt den visuellen Aufbau eines Controls.

Es kann enthalten:

- Visuelle Elemente.
- Layoutcontainer.
- Dekorative Elemente.
- Content Presenter.
- Template Bindings.
- Visuelle Zustände.
- Animationen.
- Ressourcenreferenzen.

Ein Template MUSS einem kompatiblen Control-Typ zugeordnet werden können.

Die funktionale Control-Instanz bleibt unabhängig von ihrer visuellen Struktur bestehen.

## 4. Deklarative Definition

Control Templates werden über das Nova Declarative UI Model definiert.

Beispiel:

```xml
<ControlTemplate TargetType="Button">

    <Border
        Background="{TemplateBinding Background}"
        BorderBrush="{TemplateBinding BorderBrush}"
        BorderThickness="{TemplateBinding BorderThickness}"
        CornerRadius="6"
        Padding="{TemplateBinding Padding}">

        <ContentPresenter
            HorizontalAlignment="Center"
            VerticalAlignment="Center" />

    </Border>

</ControlTemplate>
```

Das Template definiert ausschließlich die visuelle Struktur.

Das Verhalten des Buttons bleibt Bestandteil seiner Control-Implementierung.

## 5. Template Binding

Template Binding verbindet Eigenschaften eines Controls mit Elementen innerhalb seines Templates.

Beispiele:

- Background.
- Foreground.
- BorderBrush.
- Padding.
- Content.
- FontSize.
- HorizontalAlignment.

Dabei gilt:

- Bindungen MÜSSEN typsicher validiert werden.
- Änderungen MÜSSEN gezielt weitergegeben werden.
- Ungültige Bindungen MÜSSEN kontrolliert behandelt werden.
- Template Bindings DÜRFEN keine Berechtigungsgrenzen umgehen.

Template Binding SOLL für einfache Eigenschaftsübertragungen einen ressourcenschonenden Zugriff bereitstellen.

## 6. Content Presenter

Der Content Presenter stellt den vom Control bereitgestellten Inhalt innerhalb des Templates dar.

Dabei gilt:

- Die Position des Inhalts wird durch das Template bestimmt.
- Der Inhalt bleibt Bestandteil des zugehörigen Controls.
- Änderungen des Inhalts MÜSSEN automatisch berücksichtigt werden.
- Mehrfache Darstellung desselben Inhalts MUSS ausdrücklich unterstützt und eindeutig definiert sein.

Ein Template DARF mehrere Presenter enthalten, sofern deren Verwendung mit dem Control-Vertrag vereinbar ist.

## 7. Template Parts

Controls können benannte Template Parts definieren.

Beispiel:

```xml
<ControlTemplate TargetType="ScrollBar">

    <Grid>
        <Track Name="PART_Track" />
        <Thumb Name="PART_Thumb" />
    </Grid>

</ControlTemplate>
```

Dabei gilt:

- Erforderliche Template Parts MÜSSEN dokumentiert werden.
- Parts MÜSSEN über definierte Namen und Typen identifizierbar sein.
- Fehlende erforderliche Parts MÜSSEN kontrolliert behandelt werden.
- Optionale Parts DÜRFEN fehlen.

Die Control-Implementierung DARF nicht von undokumentierten internen Template-Elementen abhängig sein.

## 8. Visuelle Zustände

Templates unterstützen zustandsabhängige Darstellungen.

Beispiele:

- Normal.
- PointerOver.
- Pressed.
- Focused.
- Disabled.
- Selected.
- Checked.
- ValidationError.

Ein Zustandswechsel kann Eigenschaften, visuelle Elemente oder Animationen beeinflussen.

Dabei gilt:

- Funktionale Zustände werden vom Control bereitgestellt.
- Das Template bestimmt deren visuelle Darstellung.
- Zustandswechsel MÜSSEN gezielte Aktualisierungen auslösen.
- Animationen sind optional.

Die Darstellung eines Zustands DARF dessen funktionale Bedeutung nicht verändern.

## 9. Template-Auswahl

Templates können auf unterschiedlichen Ebenen definiert werden:

1. Direkt am Control.
2. Innerhalb lokaler Ressourcen.
3. Innerhalb anwendungsweiter Ressourcen.
4. Innerhalb des aktiven Designs.
5. Als Standard-Template des Control-Typs.

Die Auflösung MUSS einer eindeutig definierten Prioritätsreihenfolge folgen.

Ein fehlendes Template MUSS kontrolliert behandelt werden.

Für Standard Controls SOLL ein gültiges Standard-Template verfügbar sein.

## 10. Dynamischer Templatewechsel

Control Templates können während der Laufzeit ausgetauscht werden.

Dabei gilt:

- Die funktionale Control-Instanz bleibt erhalten.
- Die bisherige visuelle Template-Instanz wird kontrolliert entfernt.
- Ressourcen und Ereignisbindungen werden freigegeben.
- Das neue Template wird erstellt und verbunden.
- Layout und Darstellung werden gezielt invalidiert.

Öffentliche Eigenschaften, Datenbindungen und Ereignisbindungen des Controls MÜSSEN erhalten bleiben.

Ein Templatewechsel DARF keine vollständige Neuerstellung des Controls erzwingen.

## 11. Styles und Designsystem

Control Templates arbeiten mit der Nova UI Style Engine zusammen.

Dabei gilt:

- Styles können Templates zuweisen.
- Templates können Designressourcen verwenden.
- Globale und lokale Designs werden unterstützt.
- Benutzerdefinierte Designs können Standard-Templates ersetzen.
- Funktionales Verhalten bleibt unabhängig vom aktiven Design.

Ein Designwechsel SOLL ohne Neustart der Anwendung möglich sein.

## 12. Performance und Ressourcen

Nova Control Templates verwendet die Ressourcenverwaltung der Nova UI Runtime.

Dabei gilt:

- Template-Definitionen SOLLEN gemeinsam genutzt werden.
- Visuelle Template-Instanzen werden pro Control verwaltet.
- Unveränderte Template-Bereiche werden nicht unnötig neu berechnet.
- Eigenschaftsänderungen erzeugen gezielte Invalidierungen.
- Nicht benötigte Ressourcen werden freigegeben.

Templates DÜRFEN keine permanente Aktualisierungsschleife benötigen.

## 13. NovaStudio-Integration

NovaStudio unterstützt die visuelle Bearbeitung von Control Templates.

Dazu gehören:

- Darstellung der Template-Hierarchie.
- Bearbeitung visueller Elemente.
- Bearbeitung von Template Bindings.
- Definition visueller Zustände.
- Bearbeitung von Animationen.
- Vorschau unterschiedlicher Control-Zustände.
- Validierung erforderlicher Template Parts.

Designer und Runtime MÜSSEN dieselben Template-Definitionen verwenden können.

## 14. Sicherheit

Control Templates unterliegen Nova.Trust.

Dabei gilt:

- Templates erhalten keine zusätzlichen Systemberechtigungen.
- Template Bindings MÜSSEN bestehende Zugriffsregeln einhalten.
- Deklarative Template-Definitionen MÜSSEN validiert werden.
- Nicht vertrauenswürdige Templates DÜRFEN keine unkontrollierte Codeausführung ermöglichen.
- Fehlerhafte Templates DÜRFEN andere UI-Instanzen nicht beeinträchtigen.

Ein Template DARF keine sicherheitsrelevanten Funktionen eines Controls umgehen.

## 15. Normative Anforderungen

- **UI-001:** NovaOS MUSS deklarative Control Templates unterstützen.
- **UI-002:** Funktionales Control-Verhalten und visuelle Darstellung MÜSSEN getrennt sein.
- **UI-003:** Templates MÜSSEN einem kompatiblen Control-Typ zugeordnet werden können.
- **UI-004:** Template Binding MUSS unterstützt werden.
- **UI-005:** Content Presenter MÜSSEN unterstützt werden.
- **UI-006:** Benannte Template Parts MÜSSEN unterstützt werden.
- **UI-007:** Visuelle Zustände MÜSSEN innerhalb von Templates dargestellt werden können.
- **UI-008:** Templates MÜSSEN dynamisch ausgetauscht werden können.
- **UI-009:** Ein Templatewechsel MUSS die funktionale Control-Instanz erhalten.
- **UI-010:** Template-Ressourcen MÜSSEN kontrolliert verwaltet werden.
- **UI-011:** Templates MÜSSEN mit der Nova UI Style Engine zusammenarbeiten.
- **UI-012:** Änderungen MÜSSEN gezielte Layout- und Renderinvalidierungen auslösen.
- **UI-013:** Templates MÜSSEN unabhängig vom Renderbackend funktionieren.
- **UI-014:** NovaStudio MUSS die Bearbeitung von Control Templates unterstützen können.
- **UI-015:** Ungültige Templates MÜSSEN kontrolliert behandelt werden.
- **UI-016:** Templates MÜSSEN die Sicherheitsregeln von Nova.Trust einhalten.
- **UI-017:** Nova Control Templates MUSS vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 16. Integration

- **Nova Control Framework:** Control-Architektur und funktionales Verhalten.
- **Nova Standard Controls:** Standard-Templates.
- **Nova Custom Controls:** Benutzerdefinierte Templates.
- **Nova UI Runtime:** Verwaltung der Template-Instanzen.
- **Nova Declarative UI Model:** Deklarative Template-Definitionen.
- **Nova UI Property System:** Eigenschaften und Template Bindings.
- **Nova UI Style Engine:** Design und Erscheinungsbild.
- **Nova UI Event System:** Ereignisverarbeitung.
- **Nova Layout Engine:** Größen- und Positionsberechnung.
- **Nova UI Renderer Abstraction:** Grafische Darstellung.
- **Nova UI Component Lifecycle:** Erstellung und Freigabe.
- **Nova UI Animation Engine:** Zustandsübergänge und Animationen.
- **NovaStudio:** Visueller Template-Designer.
- **Nova.Trust:** Sicherheit und Isolation.

## 17. Ergebnis

NovaOS erhält ein natives, deklaratives und austauschbares Template-System für sämtliche geeigneten UI-Steuerelemente.

Controls können vollständig unterschiedlich gestaltet werden, ohne ihre funktionale Implementierung verändern zu müssen.

Durch wiederverwendbare Definitionen, dynamische Templatewechsel und gezielte Invalidierungen bleibt die Darstellung flexibel und ressourcenschonend.

**Grundsatz:** Das Control bestimmt seine Funktion. Das Template bestimmt seine visuelle Struktur. Das Design bestimmt sein Erscheinungsbild.