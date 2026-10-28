# NPSPEC-UI-STYLE-0002 – Nova Theme Management

**Status:** Angenommen  
**Bereich:** UI / Styling / Theme Management  
**System:** NovaOS

## 1. Zweck

Nova Theme Management definiert die Verwaltung, Auswahl und Anwendung systemweiter und anwendungsspezifischer Designs innerhalb von NovaOS.

Ziel ist ein konsistentes, dynamisch anpassbares Erscheinungsbild, das persönliche Einstellungen berücksichtigt und ohne Neustart geändert werden kann.

Themes beschreiben ausschließlich die visuelle Gestaltung. Funktionalität, Control-Verhalten und Anwendungslogik bleiben davon unabhängig.

## 2. Architektur

Das Subsystem umfasst folgende Komponenten:

| Komponente | Aufgabe |
|---|---|
| Theme Manager | Verwaltung und Aktivierung von Themes |
| Theme Registry | Registrierung verfügbarer Themes |
| Theme Resolver | Auflösung des gültigen Themes |
| Theme Resource Manager | Verwaltung themespezifischer Ressourcen |
| Theme Inheritance Manager | Verarbeitung von Theme-Vererbungen |
| Theme Transition Controller | Koordination dynamischer Themewechsel |
| Theme Preference Manager | Verwaltung benutzerdefinierter Einstellungen |

Nova Theme Management verwendet das Nova Styling System und das Nova UI Resource System.

## 3. Theme-Modell

Ein Theme besteht aus einer versionierten Definition und den zugehörigen Designressourcen.

Es kann enthalten:

- Eindeutige Theme-ID.
- Name und Version.
- Basis-Theme.
- Farbpalette.
- Akzentfarben.
- Schriftdefinitionen.
- Abstände und Rundungen.
- Brushes und Effekte.
- Styles.
- Control Templates.
- Animationseinstellungen.
- Varianten für Barrierefreiheit.

Themes DÜRFEN keine eigenständige Anwendungslogik ausführen.

## 4. Theme-Varianten

NovaOS unterstützt mindestens folgende Theme-Varianten:

| Variante | Beschreibung |
|---|---|
| Light | Helles Erscheinungsbild |
| Dark | Dunkles Erscheinungsbild |
| High Contrast | Kontrastoptimierte Darstellung |
| Custom | Benutzerdefiniertes Design |

Weitere Varianten DÜRFEN ergänzt werden.

Ein Theme kann mehrere Varianten bereitstellen, ohne sämtliche Ressourcen mehrfach definieren zu müssen.

NovaOS stellt ein vollständiges Standard-Theme bereit.

## 5. Deklarative Definition

Themes werden über das Nova Declarative UI Model und das Nova UI Resource System beschrieben.

Beispiel:

```xml
<Theme Id="Nova.Default"
       Name="NovaOS"
       Version="1.0">

    <Theme.Variants>

        <ThemeVariant Name="Dark">

            <Color Key="BackgroundColor"
                   Value="#10141C" />

            <Color Key="SurfaceColor"
                   Value="#1B2230" />

            <Color Key="TextColor"
                   Value="#F0F3F8" />

            <Color Key="AccentColor"
                   Value="#408CFF" />

        </ThemeVariant>

        <ThemeVariant Name="Light">

            <Color Key="BackgroundColor"
                   Value="#F5F7FA" />

            <Color Key="SurfaceColor"
                   Value="#FFFFFF" />

            <Color Key="TextColor"
                   Value="#202530" />

            <Color Key="AccentColor"
                   Value="#286DE0" />

        </ThemeVariant>

    </Theme.Variants>

</Theme>
```

Die Definition beschreibt Designwerte und Ressourcen.

Die tatsächliche Darstellung erfolgt über das Nova Styling System und die Renderer Abstraction.

## 6. Theme-Ressourcen

Themes stellen semantisch benannte Ressourcen bereit.

Beispiele:

- BackgroundBrush.
- SurfaceBrush.
- PrimaryTextBrush.
- SecondaryTextBrush.
- AccentBrush.
- BorderBrush.
- ErrorBrush.
- WarningBrush.
- SuccessBrush.
- ControlCornerRadius.
- DefaultSpacing.

Controls SOLLEN semantische Ressourcen anstelle fest codierter Farben verwenden.

Dadurch können Themes gewechselt werden, ohne Control-Definitionen verändern zu müssen.

## 7. Theme-Auflösung

Themes können auf unterschiedlichen Ebenen angewendet werden:

1. Benutzerdefinierte Einstellungen.
2. Anwendungsspezifische Theme-Einstellungen.
3. Systemweites Theme.
4. NovaOS-Standard-Theme.

Die Auflösung MUSS einer eindeutig definierten Prioritätsreihenfolge folgen.

Systemweite Barrierefreiheits- und Sicherheitsvorgaben besitzen Vorrang vor rein visuellen Theme-Einstellungen.

Fehlende Ressourcen MÜSSEN über definierte Fallback-Regeln aufgelöst werden.

## 8. Theme-Vererbung

Themes können auf vorhandenen Themes aufbauen.

Dabei gilt:

- Ein abgeleitetes Theme übernimmt nicht überschriebene Ressourcen.
- Einzelne Farben, Styles und Templates können ersetzt werden.
- Theme-Abhängigkeiten MÜSSEN eindeutig aufgelöst werden.
- Zyklische Vererbungen MÜSSEN erkannt werden.
- Inkompatible Theme-Versionen MÜSSEN kontrolliert behandelt werden.

Theme-Vererbung SOLL die Wiederverwendung gemeinsamer Ressourcen ermöglichen.

## 9. Dynamischer Themewechsel

NovaOS unterstützt Themewechsel während der Laufzeit.

Dabei gilt:

1. Das neue Theme wird validiert.
2. Erforderliche Ressourcen werden aufgelöst.
3. Die Änderung wird vorbereitet.
4. Das Theme wird konsistent aktiviert.
5. Betroffene UI-Eigenschaften werden aktualisiert.
6. Nicht mehr benötigte Ressourcen werden freigegeben.

Ein Themewechsel MUSS ohne Neustart der Benutzeroberfläche möglich sein.

Die funktionalen Control-Instanzen und ihre Zustände MÜSSEN erhalten bleiben.

Fehler beim Wechsel MÜSSEN eine Rückkehr zum vorherigen gültigen Theme ermöglichen.

## 10. Benutzerpräferenzen

Nova Theme Management unterstützt persönliche Einstellungen.

Dazu gehören:

- Bevorzugtes Theme.
- Hell- oder Dunkeldesign.
- Akzentfarbe.
- Kontrast.
- Transparenz.
- Animationen.
- Schriftgrößen.
- Individuelle Designanpassungen.

Benutzerpräferenzen MÜSSEN dauerhaft gespeichert und wiederhergestellt werden können.

Automatische Anpassungen DÜRFEN ausdrücklich festgelegte Benutzerentscheidungen nicht unbemerkt überschreiben.

## 11. Automatische Theme-Anpassung

NovaOS DARF Themes anhand definierter Bedingungen automatisch wechseln.

Mögliche Bedingungen:

- Tageszeit.
- Benutzerdefinierter Zeitplan.
- Gerätekontext.
- Barrierefreiheitseinstellungen.
- Energieeinstellungen.

Automatische Anpassungen MÜSSEN optional und abschaltbar sein.

Die Auswertung erfolgt deterministisch und benötigt keine KI.

Der Benutzer MUSS jederzeit ein Theme manuell auswählen können.

## 12. Anwendungsspezifische Themes

Anwendungen können eigene Themes bereitstellen.

Dabei gilt:

- Anwendungsthemes bleiben grundsätzlich auf ihren UI-Kontext beschränkt.
- Systemweite Einstellungen werden nicht unzulässig verändert.
- Gemeinsame Systemdialoge behalten ihre definierten Sicherheits- und Bedienbarkeitsanforderungen.
- Anwendungen können systemweite Designressourcen übernehmen.

Anwendungsspezifische Themes DÜRFEN die Barrierefreiheit nicht unzulässig einschränken.

## 13. Transparenz und Effekte

Themes können visuelle Effekte definieren.

Dazu gehören:

- Transparenz.
- Acrylic- und Blur-Effekte.
- Schatten.
- Rahmen.
- Farbverläufe.
- Zustandsanimationen.

Effekte MÜSSEN über die zuständigen Grafik- und Compositor-Schnittstellen umgesetzt werden.

Bei fehlender Hardwareunterstützung oder eingeschränkten Ressourcen MÜSSEN geeignete Fallback-Darstellungen möglich sein.

Die Bedienbarkeit MUSS unabhängig von aktivierten Effekten erhalten bleiben.

## 14. Barrierefreiheit

Nova Theme Management berücksichtigt die Nova UI Accessibility Architecture.

Dabei gilt:

- Kontrastanforderungen MÜSSEN unterstützt werden.
- Reduzierte Animationen MÜSSEN berücksichtigt werden.
- Transparenzeffekte MÜSSEN deaktivierbar sein.
- Schriftgrößenanpassungen MÜSSEN unterstützt werden.
- Zustände DÜRFEN nicht ausschließlich durch Farbe unterscheidbar sein, sofern dies die Bedienbarkeit beeinträchtigt.

Barrierefreiheitseinstellungen besitzen Vorrang vor dekorativen Theme-Eigenschaften.

## 15. Performance und Ressourcen

Nova Theme Management verwendet die Ressourcenverwaltung der Nova UI Runtime.

Dabei gilt:

- Gemeinsam nutzbare Theme-Ressourcen SOLLEN wiederverwendet werden.
- Ressourcen SOLLEN bedarfsgerecht geladen werden.
- Unveränderte Eigenschaften werden nicht erneut ausgewertet.
- Themewechsel erzeugen gezielte Layout- und Renderinvalidierungen.
- Reine Farbänderungen DÜRFEN keine unnötigen Layoutberechnungen verursachen.
- Nicht benötigte Ressourcen werden kontrolliert freigegeben.

Themes DÜRFEN keine permanente Aktualisierungsschleife benötigen.

## 16. NovaStudio-Integration

NovaStudio unterstützt die Erstellung und Bearbeitung von Themes.

Dazu gehören:

- Theme-Editor.
- Farbpalettenverwaltung.
- Ressourcenbearbeitung.
- Style- und Template-Zuordnung.
- Definition von Theme-Varianten.
- Vorschau unterschiedlicher Controls.
- Vorschau von Barrierefreiheitseinstellungen.
- Validierung und Paketierung.

Designer und Runtime MÜSSEN dieselben Theme-Definitionen verwenden können.

## 17. Sicherheit

Nova Theme Management unterliegt Nova.Trust.

Dabei gilt:

- Themes MÜSSEN vor ihrer Aktivierung validiert werden.
- Themes DÜRFEN keine unkontrollierte Codeausführung ermöglichen.
- Externe Ressourcen MÜSSEN bestehende Berechtigungen einhalten.
- Themes DÜRFEN keine zusätzlichen Capabilities verleihen.
- Fehlerhafte Themes DÜRFEN andere UI-Instanzen nicht beeinträchtigen.
- Ein gültiges Standard-Theme MUSS als Rückfallebene verfügbar bleiben.

Themes aus nicht vertrauenswürdigen Quellen MÜSSEN entsprechend den Nova.Trust-Richtlinien behandelt werden.

## 18. Normative Anforderungen

- **UI-001:** NovaOS MUSS ein natives Theme Management bereitstellen.
- **UI-002:** Themes MÜSSEN unabhängig von funktionalem Control-Verhalten definiert werden.
- **UI-003:** Light-, Dark- und High-Contrast-Varianten MÜSSEN unterstützt werden.
- **UI-004:** Benutzerdefinierte Themes MÜSSEN unterstützt werden.
- **UI-005:** Themes MÜSSEN deklarativ beschrieben werden können.
- **UI-006:** Semantische Theme-Ressourcen MÜSSEN unterstützt werden.
- **UI-007:** Theme-Vererbung MUSS unterstützt werden.
- **UI-008:** Theme-Abhängigkeiten MÜSSEN deterministisch aufgelöst werden.
- **UI-009:** Themewechsel MÜSSEN ohne Neustart möglich sein.
- **UI-010:** Funktionale Control-Instanzen MÜSSEN beim Themewechsel erhalten bleiben.
- **UI-011:** Fehlerhafte Themewechsel MÜSSEN kontrolliert rückgängig gemacht werden können.
- **UI-012:** Benutzerpräferenzen MÜSSEN gespeichert und wiederhergestellt werden können.
- **UI-013:** Automatische Theme-Anpassungen MÜSSEN abschaltbar sein.
- **UI-014:** Anwendungsspezifische Themes MÜSSEN unterstützt werden.
- **UI-015:** Barrierefreiheitseinstellungen MÜSSEN Vorrang vor dekorativen Theme-Eigenschaften besitzen.
- **UI-016:** Themeänderungen MÜSSEN gezielte Invalidierungen auslösen.
- **UI-017:** Themes MÜSSEN unabhängig vom Renderbackend funktionieren.
- **UI-018:** NovaStudio MUSS Themes bearbeiten und validieren können.
- **UI-019:** Themes MÜSSEN die Sicherheitsregeln von Nova.Trust einhalten.
- **UI-020:** Nova Theme Management MUSS vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 19. Integration

- **Nova Styling System:** Anwendung und Auflösung von Styles.
- **Nova UI Resource System:** Verwaltung von Designressourcen.
- **Nova Control Framework:** Gestaltung nativer Controls.
- **Nova Control Templates:** Visuelle Control-Struktur.
- **Nova Control States:** Zustandsabhängige Darstellung.
- **Nova UI Property System:** Dynamische Eigenschaftswerte.
- **Nova UI Runtime:** Verwaltung und Aktivierung.
- **Nova Layout Engine:** Geometrische Auswirkungen.
- **Nova UI Renderer Abstraction:** Grafische Darstellung.
- **Nova UI Animation Engine:** Animierte Themewechsel.
- **Nova Compositor:** Transparenz und visuelle Effekte.
- **Nova UI Accessibility:** Barrierefreiheit.
- **Nova Settings:** Benutzerpräferenzen.
- **NovaStudio:** Theme-Editor.
- **Nova.Trust:** Sicherheit und Isolation.

## 20. Ergebnis

NovaOS erhält ein natives, dynamisches und erweiterbares Theme Management.

Benutzer können das Erscheinungsbild des gesamten Systems oder einzelner Anwendungen individuell gestalten.

Themes lassen sich während der Laufzeit wechseln, ohne funktionale Control-Zustände oder Anwendungslogik zu verändern.

**Grundsatz:** Nova Theme Management passt das Erscheinungsbild an den Benutzer an – konsistent, ressourcenschonend und jederzeit unter seiner Kontrolle.