# NPSPEC-UI-ADAPTIVE-0001 – Adaptive UI

## Status

Angenommen

## Kategorie

UI / Adaptive / Context / User Experience

## Zweck

NovaOS darf seine Benutzeroberfläche dynamisch an Nutzung, Kontext, Gerät und verfügbare Ressourcen anpassen.

Dabei gilt:

```text
Adaptation unterstützt den Nutzer.
Adaptation kontrolliert den Nutzer nicht.
```

Explizite Nutzerentscheidungen haben Vorrang vor automatisch erlerntem Verhalten.

## Architektur

```text
User Interaction
      ↓
Context + UI State
      ↓
Adaptive UI Engine
      ↓
Constraints / User Preferences
      ↓
Adaptive Decision
      ↓
Declarative UI State
```

Die Adaptive UI Engine verändert nicht direkt den Rendering-Pfad, sondern erzeugt kontrollierte Änderungen am gewünschten UI-Zustand.

## Anpassbare Eigenschaften

Adaptive Entscheidungen dürfen unter anderem betreffen:

```text
Layout
Information Density
Control Placement
Suggested Actions
Panel Visibility
Window Arrangement
Animation Level
Rendering Quality
Scaling
Preloading
```

Kritische Bedienelemente dürfen nicht unvorhersehbar verschwinden oder ihre Bedeutung ändern.

## Kontext

Mögliche Kontextsignale sind:

```text
Display Size
Input Method
Device Type
Current Activity
Window Size
Resource Pressure
Power State
Accessibility Requirements
Repeated User Actions
```

Nur zulässige und erforderliche Kontextinformationen dürfen verwendet werden.

## Prediction Error

NovaOS verwendet Abweichungen zwischen Vorhersage und tatsächlicher Nutzerentscheidung als Lernsignal.

```text
Prediction
    ↓
User Decision
    ↓
Deviation
    ↓
Prediction Error
    ↓
Model Correction
```

Wiederholte Abweichungen sollen die Wahrscheinlichkeit reduzieren, dass dieselbe ungeeignete Anpassung erneut vorgenommen wird.

## Nutzerentscheidung

Die Priorität lautet:

```text
Explicit User Decision
        ↓
Stored User Preference
        ↓
Adaptive Prediction
        ↓
Default Behaviour
```

Eine explizite Nutzerentscheidung darf nicht unmittelbar durch die Adaptive UI rückgängig gemacht werden.

## Stabilität

Adaptive UI darf nicht zu ständig wechselnden Oberflächen führen.

```text
Prediction Change
      ↓
Stability Check
      ↓
Meaningful Benefit?
   ├── Yes → Adapt
   └── No  → Keep Current UI
```

Für Anpassungen können Mindestvertrauen, Hysterese oder zeitliche Stabilitätsgrenzen verwendet werden.

## Vorhersagbarkeit

Wesentliche Navigation und grundlegende Systemfunktionen sollen stabile Positionen und Bedeutungen behalten.

Adaptive Änderungen sollen bevorzugt dort stattfinden, wo sie den Arbeitsablauf verbessern, ohne erlernte Bedienmuster zu zerstören.

## Rückgängig machen

Relevante adaptive Änderungen sollen rückgängig gemacht werden können.

```text
Adaptive Change
      ↓
User Rejects
      ↓
Restore Previous State
      ↓
Record Prediction Error
```

Die Ablehnung kann als Lernsignal verwendet werden.

## Ressourcenanpassung

Die UI darf sich an verfügbare Ressourcen anpassen.

Beispiel:

```text
Normal Resources
      ↓
Full Effects

Resource Pressure
      ↓
Reduced Effects
      ↓
Simplified Animation
```

Funktionalität und Bedienbarkeit haben Vorrang vor dekorativen Effekten.

## Geräteanpassung

Dieselbe semantische UI kann abhängig vom Gerät unterschiedlich dargestellt werden.

```text
Desktop
Tablet
Touch Display
Small Display
Remote Session
```

Die semantische Funktion eines Elements bleibt dabei erhalten.

## Accessibility

Accessibility-Anforderungen gelten als harte Randbedingungen gegenüber rein adaptiven Optimierungen.

```text
Adaptive Decision
      ↓
Accessibility Constraints
      ↓
Allowed UI State
```

Adaptive Änderungen dürfen Accessibility-Einstellungen nicht umgehen.

## Sicherheit und Datenschutz

Adaptive UI darf keine Sicherheits-, Privacy- oder Capability-Regeln überschreiben.

```text
Adaptive Preference
       ↓
Hard Constraints
       ↓
Allowed / Rejected
```

Aus einer vorhergesagten Nutzerabsicht entsteht keine zusätzliche Berechtigung.

## Erklärbarkeit

Relevante adaptive Entscheidungen sollen nachvollziehbar sein.

NovaOS soll bei Bedarf erklären können:

```text
Was wurde angepasst?
Warum wurde es angepasst?
Welche Signale waren relevant?
Wie kann die Anpassung geändert werden?
```

## Normative Anforderungen

1. Adaptive UI MUSS auf der deklarativen UI-Architektur aufbauen.
2. Explizite Nutzerentscheidungen MÜSSEN adaptive Vorhersagen überstimmen.
3. Prediction Error SOLL als Lernsignal verwendet werden.
4. Wiederholte Nutzerkorrekturen SOLLEN zukünftige Anpassungen beeinflussen.
5. Adaptive Änderungen DÜRFEN keine harten Systemregeln überschreiben.
6. Adaptive Änderungen DÜRFEN keine zusätzlichen Capabilities erzeugen.
7. Accessibility-Anforderungen MÜSSEN gegenüber adaptiven Optimierungen Vorrang besitzen.
8. Kritische UI-Funktionen MÜSSEN vorhersehbar erreichbar bleiben.
9. Adaptive UI MUSS unnötige oder instabile Layoutänderungen vermeiden.
10. Relevante adaptive Änderungen SOLLEN rückgängig gemacht werden können.
11. Ressourcenabhängige Anpassungen DÜRFEN dekorative Qualität reduzieren, aber keine notwendige Funktionalität entfernen.
12. Adaptive Entscheidungen SOLLEN beobachtbar und nachvollziehbar sein.
13. Geräteabhängige Darstellung DARF die semantische Bedeutung einer Funktion nicht verändern.
14. Unzureichende Sicherheit einer Vorhersage SOLL zur Beibehaltung des stabilen UI-Zustands führen.

## Abhängigkeiten

- `NPSPEC-UI-DECLARATIVE-0001`
- `NPSPEC-UI-RETAINEDMODE-0001`
- `NPSPEC-UI-ACCESSIBILITYTREE-0001`
- `ADR-ADAPTIVE-0001_Prediction_Error_als_zentrales_Lernsignal`
- `ADR-ADAPTIVE-0002_Prediction_User_Decision_Deviation_Model_Correction`
- `ADR-ADAPTIVE-0003_Explizite_Nutzerentscheidung_vor_Adaptation`
- `ADR-ADAPTIVE-0011_Context_Awareness`
- `ADR-ADAPTIVE-0012_Policy_Learning_mit_harten_Grenzen`
- `ADR-ARCH-0004_Systemweite_Ressourcenökonomie`

## Ergebnis

Die NovaOS-Oberfläche kann sich an Nutzer und Kontext anpassen, ohne ihre Stabilität und Vorhersehbarkeit aufzugeben:

```text
Observe
   ↓
Predict
   ↓
Check Hard Constraints
   ↓
Adapt
   ↓
Observe User Decision
   ↓
Measure Prediction Error
   ↓
Learn
```

Damit entsteht eine lernfähige Oberfläche, bei der Anpassung ein Optimierungsmechanismus bleibt und die endgültige Kontrolle beim Nutzer liegt.