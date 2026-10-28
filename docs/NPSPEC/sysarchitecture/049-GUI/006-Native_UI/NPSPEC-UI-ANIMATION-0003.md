# NPSPEC-UI-ANIMATION-0003 – Nova Transitions

**Status:** Angenommen  
**Bereich:** UI / Animation / Transitions  
**System:** NovaOS

## 1. Zweck

Nova Transitions definiert automatische, zeitgesteuerte Übergänge zwischen unterschiedlichen Eigenschaftswerten und visuellen Zuständen nativer UI-Elemente.

Ziel ist es, Zustandsänderungen flüssig darzustellen, ohne für jede Änderung eine eigene Animation starten zu müssen.

Transitions verwenden die Nova Animation Engine und das Nova UI Property System.

## 2. Architektur

Das Subsystem umfasst folgende Komponenten:

| Komponente | Aufgabe |
|---|---|
| Transition Manager | Verwaltung aktiver Übergänge |
| Transition Resolver | Ermittlung gültiger Übergangsdefinitionen |
| Transition Evaluator | Berechnung der Übergangswerte |
| Transition State Tracker | Erkennung relevanter Zustandsänderungen |
| Transition Conflict Resolver | Auflösung konkurrierender Übergänge |

Die Zeitsteuerung erfolgt ausschließlich über die Nova Animation Engine.

## 3. Transition-Modell

Eine Transition beschreibt den automatischen Übergang vom bisherigen effektiven Wert zu einem neuen Zielwert.

Eine Transition kann enthalten:

- Zieleigenschaft.
- Dauer.
- Interpolationsverfahren.
- Optionale Verzögerung.
- Auslösende Zustandsänderung.
- Unterbrechungsverhalten.

Im Gegensatz zu einer Property Animation benötigt eine Transition keine expliziten Start- und Zielwerte.

Diese werden aus der jeweiligen Eigenschaftsänderung ermittelt.

## 4. Deklarative Definition

Transitions können direkt an UI-Elementen oder innerhalb von Styles definiert werden.

Konzeptionelles Beispiel:

```xml
<Button Content="Speichern">

    <Button.Transitions>
        <Transition Property="Opacity"
                    Duration="200ms"
                    Easing="EaseOut" />

        <Transition Property="Background"
                    Duration="150ms"
                    Easing="Linear" />
    </Button.Transitions>

</Button>
```

Die konkrete Syntax wird durch das Nova Declarative UI Model festgelegt.

## 5. Automatische Auslösung

Transitions werden durch Änderungen des effektiven Zielwerts ausgelöst.

Mögliche Ursachen sind:

- Änderungen von Control States.
- Änderungen von Styles.
- Dynamische Themewechsel.
- Änderungen gebundener Eigenschaften.
- Programmatische Eigenschaftsänderungen.

Dabei gilt:

- Eine Transition startet nur bei einer tatsächlichen Wertänderung.
- Identische Werte lösen keine neue Transition aus.
- Die Transition verändert nicht den zugrunde liegenden Basiswert.
- Nicht animierbare Eigenschaften werden unmittelbar aktualisiert.

## 6. Visual State Transitions

Transitions unterstützen Übergänge zwischen visuellen Control States.

Beispiele:

- Normal → Hover.
- Hover → Pressed.
- Pressed → Normal.
- Normal → Disabled.
- Unfocused → Focused.
- Collapsed → Expanded.

Dabei gilt:

- Funktionale Zustandsänderungen erfolgen unmittelbar.
- Die visuelle Darstellung kann zeitlich übergehen.
- Zustandswechsel DÜRFEN nicht durch Animationen blockiert werden.
- Für unterschiedliche Zustandswechsel DÜRFEN unterschiedliche Transitions definiert werden.

## 7. Unterbrechung und Richtungswechsel

Eine laufende Transition kann durch eine neue Zustandsänderung unterbrochen werden.

Dabei gilt:

- Der aktuelle visuelle Wert wird als Ausgangspunkt des neuen Übergangs verwendet.
- Neue Zielwerte besitzen Vorrang vor veralteten Zielwerten.
- Richtungswechsel MÜSSEN ohne unnötige visuelle Sprünge erfolgen.
- Nicht mehr benötigte Übergänge werden beendet.
- Die Behandlung konkurrierender Animationen erfolgt nach den Regeln von Nova Property Animations.

## 8. Transitions und Property Animations

Transitions und explizite Property Animations verwenden dieselbe Animationsinfrastruktur.

Dabei gilt:

- Transitions reagieren automatisch auf Eigenschaftsänderungen.
- Property Animations werden ausdrücklich gestartet.
- Konflikte MÜSSEN deterministisch aufgelöst werden.
- Explizite Animationen besitzen standardmäßig Vorrang vor automatischen Transitions.
- Nach Abschluss einer expliziten Animation MUSS der aktuelle effektive Zielwert berücksichtigt werden.

Die konkrete Prioritätsauflösung erfolgt über das Nova UI Property System.

## 9. Layout und Rendering

Transitions verwenden gezielte Aktualisierungen.

Dabei gilt:

- Änderungen von Layout-Eigenschaften lösen erforderliche Layoutinvalidierungen aus.
- Reine Transformations- und Transparenzübergänge SOLLEN compositorseitig erfolgen.
- Unveränderte Bereiche werden nicht neu gezeichnet.
- Damage Tracking MUSS berücksichtigt werden.
- Ohne aktive Transitions werden keine zusätzlichen Animationsframes angefordert.

## 10. Barrierefreiheit und Ressourcen

Nova Transitions berücksichtigt Benutzerpräferenzen und verfügbare Systemressourcen.

Dabei gilt:

- Reduzierte Animationen MÜSSEN unterstützt werden.
- Deaktivierte Transitions führen unmittelbar zum Zielzustand.
- Nicht sichtbare Übergänge SOLLEN vermieden oder reduziert werden.
- Bei eingeschränkten Ressourcen DÜRFEN dekorative Transitions vereinfacht werden.
- Die Bedienbarkeit besitzt Vorrang vor visuellen Übergängen.

## 11. NovaStudio und Logic Graph

NovaStudio unterstützt:

- Visuelle Definition von Transitions.
- Auswahl animierbarer Eigenschaften.
- Einstellung von Dauer und Easing.
- Zuordnung zu Visual States.
- Vorschau und Debugging.
- Erkennung konkurrierender Animationen.

Logic Graph kann Transitions über Eigenschafts- und Zustandsänderungen auslösen.

Eine zusätzliche explizite Animationssteuerung ist dafür nicht erforderlich.

## 12. Sicherheit

Nova Transitions unterliegt Nova.Trust.

Dabei gilt:

- Transitions DÜRFEN keine zusätzlichen Capabilities verleihen.
- Geschützte Eigenschaften DÜRFEN nicht unautorisiert verändert werden.
- Sicherheitsrelevante Zustandsänderungen MÜSSEN unabhängig von ihrer Animation wirksam werden.
- Ungültige Transition-Definitionen MÜSSEN kontrolliert behandelt werden.

## 13. Normative Anforderungen

- **UI-001:** NovaOS MUSS automatische Property Transitions unterstützen.
- **UI-002:** Transitions MÜSSEN deklarativ definiert werden können.
- **UI-003:** Start- und Zielwerte MÜSSEN automatisch ermittelt werden.
- **UI-004:** Transitions DÜRFEN nur bei tatsächlichen Wertänderungen starten.
- **UI-005:** Dauer und Interpolation MÜSSEN konfigurierbar sein.
- **UI-006:** Visual State Transitions MÜSSEN unterstützt werden.
- **UI-007:** Laufende Transitions MÜSSEN unterbrechbar sein.
- **UI-008:** Richtungswechsel MÜSSEN den aktuellen visuellen Wert berücksichtigen.
- **UI-009:** Konflikte mit Property Animations MÜSSEN deterministisch aufgelöst werden.
- **UI-010:** Basiswerte und Datenbindungen DÜRFEN nicht unbeabsichtigt überschrieben werden.
- **UI-011:** Transitions MÜSSEN gezielte Layout- und Renderinvalidierungen verwenden.
- **UI-012:** Geeignete Transitions SOLLEN compositorseitig ausgeführt werden.
- **UI-013:** Reduzierte Animationen MÜSSEN berücksichtigt werden.
- **UI-014:** NovaStudio MUSS Transitions visuell bearbeiten können.
- **UI-015:** Transitions MÜSSEN unabhängig vom Renderbackend definiert werden können.
- **UI-016:** Transitions MÜSSEN die Sicherheitsregeln von Nova.Trust einhalten.
- **UI-017:** Nova Transitions MUSS vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 14. Ergebnis

NovaOS erhält ein einheitliches System für automatische Übergänge zwischen UI-Eigenschaften und visuellen Zuständen.

Zustandsänderungen können flüssig dargestellt werden, ohne zusätzliche Event Handler oder manuell gestartete Animationen zu benötigen.

Durch gezielte Invalidierungen, kontrollierte Unterbrechungen und die gemeinsame Nutzung der Nova Animation Engine bleiben Transitions effizient und konsistent.

**Grundsatz:** Der Zustand ändert sich sofort – die Darstellung darf sanft folgen.