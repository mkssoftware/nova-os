# NPSPEC-BOOTCONTROL-0004
# Control Templates

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTCONTROL-0004 |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot UI Framework |
| Priorität | Hoch |
| Gültigkeitsbereich | Sämtliche UI-Controls des NovaOS Boot Managers |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTCONTROL-0001, NPSPEC-BOOTCONTROL-0002, NPSPEC-BOOTCONTROL-0003, NPSPEC-BOOTLAYOUT-0001, NPSPEC-BOOTRENDER-0001 |

---

# 1. Ziel

Diese Spezifikation definiert das **Control Template System** der NovaOS Boot UI.

Control Templates beschreiben den strukturellen Aufbau eines Controls unabhängig von dessen Daten oder Verhalten. Ein Template legt fest, aus welchen visuellen Elementen ein Control besteht und wie diese angeordnet werden.

Dadurch können Controls vollständig umgestaltet werden, ohne ihre Funktionalität zu verändern.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für sämtliche UI-Controls.

Sie umfasst:

- Button
- Label
- TextBox
- CheckBox
- RadioButton
- ProgressBar
- List
- ScrollView
- Image
- Container
- zukünftige Controls

---

# 3. Architektur

```text
Control

↓

Template Manager

↓

Control Template

↓

Visual Tree

↓

Renderer
```

---

# 4. Grundprinzip

Ein Template beschreibt ausschließlich den visuellen Aufbau eines Controls.

Es definiert:

- Visual Tree
- Platzhalter
- Dekorationen
- Container
- Zustandsflächen

Das Template enthält keine Geschäftslogik.

---

# 5. Aufgaben

Das Template-System übernimmt:

- Aufbau des Visual Tree
- Definition visueller Elemente
- Zustandsdarstellung
- Template-Vererbung
- Austausch von Templates
- Wiederverwendung
- Theme-Integration
- Rendering-Vorbereitung

---

# 6. Template-Hierarchie

```text
Base Template

├── Button Template

├── Label Template

├── TextBox Template

├── ProgressBar Template

└── List Template
```

Templates können von anderen Templates erben.

---

# 7. Visual Tree

Ein Template besteht aus einem Baum visueller Elemente.

Beispiel:

```text
Button

├── Background

├── Border

├── ContentPresenter

└── FocusIndicator
```

Der Visual Tree ist statisch definiert.

---

# 8. Template Parts

Ein Template kann benannte Bestandteile enthalten.

Beispiele:

- Background
- Border
- Icon
- Text
- ContentPresenter
- ProgressFill
- Thumb
- Track

Diese Bestandteile können vom Control angesprochen werden.

---

# 9. Content Presenter

Controls mit variablem Inhalt verwenden einen **Content Presenter**.

Dieser stellt dar:

- Text
- Icons
- kombinierte Inhalte
- zukünftige Inhaltstypen

Der Presenter besitzt keine eigene Logik.

---

# 10. Zustandsdarstellung

Templates definieren die Darstellung für:

```text
Normal

Hovered

Focused

Pressed

Disabled

Selected

Busy

Error
```

Die Zustandslogik bleibt Bestandteil des Control Frameworks.

---

# 11. Template-Vererbung

Templates dürfen erweitert werden.

Beispiel:

```text
Button

↓

Primary Button

↓

Destructive Button
```

Nur geänderte Bestandteile werden überschrieben.

---

# 12. Austauschbarkeit

Ein Control kann ein anderes Template verwenden, sofern:

- alle erforderlichen Template Parts vorhanden sind
- die Schnittstellen kompatibel bleiben

Die Funktionalität bleibt unverändert.

---

# 13. Theme-Integration

Templates verwenden ausschließlich Theme-Ressourcen.

Beispiele:

- Farben
- Materialien
- Schriftarten
- Schatten
- Rundungen
- Animationen

Templates enthalten keine festen Designwerte.

---

# 14. Rendering

Der Renderer verarbeitet ausschließlich den Visual Tree des Templates.

Das eigentliche Control erzeugt keine Renderbefehle.

---

# 15. Animationen

Templates definieren:

- animierbare Bereiche
- Übergänge
- Zustandsanimationen
- Materialwechsel

Die Ausführung übernimmt das Motion-System.

---

# 16. Accessibility

Jeder Template Part kann Accessibility-Eigenschaften besitzen.

Beispiele:

- Rolle
- Name
- Beschreibung
- Fokusdarstellung

Unsichtbare Dekorationen dürfen keine Accessibility-Rolle besitzen.

---

# 17. Ressourcenverwaltung

Templates werden:

- einmal geladen
- zentral gespeichert
- zwischen Controls gemeinsam genutzt

Mehrfachinstanzen desselben Templates sind zu vermeiden.

---

# 18. Speicherverwaltung

Templates verwenden:

- statische Template-Tabellen
- konstante Visual Trees
- feste Template Parts

Heap-Allokationen während der Laufzeit sind verboten.

---

# 19. Performance

Anforderungen:

- O(1)-Templatezugriff
- O(1)-Templatebindung
- deterministische Laufzeit
- keine Heap-Allokationen

Templates dürfen nicht mehrfach aufgebaut werden.

---

# 20. Fehlerbehandlung

Folgende Fehler müssen erkannt werden:

- fehlendes Template
- fehlender Template Part
- rekursive Template-Vererbung
- beschädigter Visual Tree
- Nullpointer

Kann ein Template nicht geladen werden, wird das Standardtemplate verwendet.

---

# 21. Sicherheit

Das System verhindert:

- rekursive Templates
- ungültige Template Parts
- beschädigte Visual Trees
- inkompatible Templatebindungen
- fehlende Pflichtbestandteile

Alle Templates werden vor der Verwendung validiert.

---

# 22. API

```c
typedef struct nova_control_template nova_control_template_t;

const nova_control_template_t*
nova_control_template_get(
    nova_template_id_t id);

nova_result_t
nova_control_template_apply(
    nova_control_t* control,
    const nova_control_template_t* templ);

bool
nova_control_template_has_part(
    const nova_control_template_t* templ,
    const char* part_name);

const void*
nova_control_template_get_part(
    const nova_control_template_t* templ,
    const char* part_name);
```

---

# 23. Diagnosedaten

Zu erfassen sind:

- Template-ID
- Template-Typ
- Visual-Tree-Größe
- verwendete Template Parts
- Theme-ID
- Speicherverbrauch
- Ladezeit
- Fehleranzahl

---

# 24. Testfälle

Zu testen sind:

- Template laden
- Template anwenden
- Template-Vererbung
- Content Presenter
- Template Parts
- Themewechsel
- Animationen
- Accessibility
- Standardtemplate
- Fehlerfälle

---

# 25. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- sämtliche Controls über Templates aufgebaut werden.
- Templates ausschließlich die visuelle Struktur beschreiben.
- Template-Vererbung korrekt funktioniert.
- Theme-Ressourcen vollständig verwendet werden.
- alle Pflichtbestandteile validiert werden.
- keine Heap-Allokationen während der Laufzeit stattfinden.
- BIOS- und UEFI-Systeme identische Template-Strukturen erzeugen.

---

# 26. Referenzen

## ADRs

- ADR-BOOTCONTROL-0004 – Control Templates
- ADR-BOOTTHEME-0001 – Theme-System
- ADR-BOOTRENDER-0002 – Scene Graph
- ADR-BOOTACCESS-0001 – Accessibility
- ADR-BOOTPERF-0002 – Deterministische UI-Verarbeitung

## NPSPECs

- NPSPEC-BOOTCONTROL-0001 – Boot Control Framework
- NPSPEC-BOOTCONTROL-0002 – Control State Model
- NPSPEC-BOOTCONTROL-0003 – Control Styling
- NPSPEC-BOOTLAYOUT-0001 – Boot Layout Engine
- NPSPEC-BOOTRENDER-0002 – Scene Graph
- NPSPEC-BOOTACCESSIBILITY-0001 – Accessibility Framework

---

# 27. Zusammenfassung

Das **Control Template System** definiert den strukturellen Aufbau sämtlicher UI-Controls der NovaOS Bootoberfläche. Templates beschreiben den Visual Tree, benannte Template Parts sowie die visuelle Zusammensetzung eines Controls, ohne dessen Verhalten zu beeinflussen. Durch Template-Vererbung, Theme-Integration, zentrale Ressourcenverwaltung und statische Speicherstrukturen entsteht ein flexibles, konsistentes und performantes Framework für die Darstellung aller Controls auf BIOS- und UEFI-Systemen.