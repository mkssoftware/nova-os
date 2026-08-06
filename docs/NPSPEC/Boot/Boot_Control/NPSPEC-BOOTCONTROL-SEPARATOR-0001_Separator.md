# NPSPEC-BOOTCONTROL-SEPARATOR-0001
# Separator

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTCONTROL-SEPARATOR-0001 |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot UI Controls |
| Priorität | Mittel |
| Gültigkeitsbereich | NovaOS Boot Manager |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTCONTROL-0001, NPSPEC-BOOTCONTROL-0003, NPSPEC-BOOTCONTROL-0004, NPSPEC-BOOTLAYOUT-0001, NPSPEC-BOOTACCESSIBILITY-0001 |

---

# 1. Ziel

Diese Spezifikation definiert das **Separator-Control** der NovaOS Boot UI.

Ein Separator dient der visuellen Trennung logisch zusammengehöriger Bereiche innerhalb der Benutzeroberfläche. Er verbessert die Lesbarkeit und Strukturierung komplexer Ansichten, besitzt jedoch keine eigene Funktionalität.

Typische Einsatzbereiche:

- Dialoge
- Menüs
- Listen
- Einstellungsseiten
- Informationsbereiche
- Statusanzeigen
- Werkzeugleisten

---

# 2. Geltungsbereich

Diese Spezifikation gilt für sämtliche Separatoren.

Sie umfasst:

- horizontale Separatoren
- vertikale Separatoren
- beschriftete Separatoren
- dekorative Separatoren

Nicht Bestandteil sind:

- Rahmen
- Layout-Container
- Liniengrafiken

---

# 3. Architektur

```text
Boot Control Framework

↓

Separator

↓

Renderer

↓

Framebuffer
```

---

# 4. Grundprinzip

Ein Separator besitzt keine Benutzerinteraktion.

Er dient ausschließlich der visuellen Gliederung der Benutzeroberfläche.

Ein Separator darf keine semantische Funktion übernehmen.

---

# 5. Aufgaben

Der Separator übernimmt:

- visuelle Trennung
- Strukturierung
- Themeintegration
- Layoutunterstützung
- Accessibility

---

# 6. Aufbau

```text
Separator

├── Line

├── Label (optional)

└── Decoration (optional)
```

---

# 7. Eigenschaften

Jeder Separator besitzt:

- ID
- Orientierung
- Sichtbarkeit
- Style
- Template

Optional:

- Titel
- Symbol
- Abstand

---

# 8. Typen

Unterstützt werden:

- Horizontal
- Vertikal
- Mit Titel
- Mit Symbol

Die Auswahl erfolgt über Style und Template.

---

# 9. Zustände

```text
Visible

Hidden

Disabled
```

Separatoren besitzen keine Fokus-, Hover- oder Pressed-Zustände.

---

# 10. Orientierung

Unterstützt werden:

```text
Horizontal
```

und

```text
Vertikal
```

Die Orientierung beeinflusst ausschließlich die Darstellung.

---

# 11. Positionierung

Separatoren werden vollständig durch das Layout-System positioniert.

Sie dürfen:

- Container trennen
- Listen gliedern
- Gruppen strukturieren

Sie beeinflussen nicht die Position anderer Controls.

---

# 12. Darstellung

Der Separator verwendet:

- Themefarbe
- Transparenz
- feine Linien
- optionale Beschriftung
- optionales Symbol

Die Darstellung erfolgt ausschließlich über Theme und Template.

---

# 13. Animationen

Standardmäßig besitzt der Separator keine Animation.

Optional unterstützt:

- Fade
- Theme Transition

Animationen dürfen die Lesbarkeit nicht beeinträchtigen.

---

# 14. Accessibility

Dekorative Separatoren werden von Screenreadern ignoriert.

Beschriftete Separatoren werden als Gruppenüberschrift ausgegeben.

---

# 15. Themeintegration

Das Theme definiert:

- Linienfarbe
- Transparenz
- Schriftarten
- Symbolfarben
- Abstände

Es existieren keine fest codierten Farben.

---

# 16. Speicherverwaltung

Separatoren verwenden:

- gemeinsame Templates
- gemeinsame Styles
- Objektpools

Heap-Allokationen während der Laufzeit sind verboten.

---

# 17. Performance

Anforderungen:

- O(1)-Darstellung
- O(1)-Themewechsel
- deterministische Laufzeit

Separatoren verursachen keine zusätzlichen Layoutberechnungen.

---

# 18. Fehlerbehandlung

Folgende Fehler müssen erkannt werden:

- ungültige Orientierung
- beschädigtes Template
- fehlender Style
- Nullpointer

Kann ein Separator nicht dargestellt werden, bleibt das Layout funktionsfähig.

---

# 19. Sicherheit

Das System verhindert:

- ungültige Größen
- negative Abstände
- rekursive Templates
- beschädigte Styleverweise

Separatoren dürfen niemals Eingaben entgegennehmen.

---

# 20. API

```c
typedef struct nova_separator nova_separator_t;

typedef enum
{
    NOVA_SEPARATOR_HORIZONTAL,
    NOVA_SEPARATOR_VERTICAL
}
nova_separator_orientation_t;

nova_result_t
nova_separator_create(
    nova_separator_orientation_t orientation,
    nova_separator_t** separator);

nova_result_t
nova_separator_set_title(
    nova_separator_t* separator,
    const char* title);

nova_result_t
nova_separator_set_visible(
    nova_separator_t* separator,
    bool visible);
```

---

# 21. Diagnosedaten

Zu erfassen sind:

- Separator-ID
- Orientierung
- Sichtbarkeit
- Renderzeit
- Speicherverbrauch
- Fehleranzahl

---

# 22. Testfälle

Zu testen sind:

- Horizontal
- Vertikal
- Mit Titel
- Themewechsel
- High-DPI
- Accessibility
- Sichtbarkeit
- Fehlerfälle

---

# 23. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- horizontale und vertikale Separatoren korrekt dargestellt werden.
- beschriftete Separatoren unterstützt werden.
- dekorative Separatoren keine Accessibility-Ausgabe erzeugen.
- Theme und Templates vollständig verwendet werden.
- Separatoren keine Benutzerinteraktion besitzen.
- keine Heap-Allokationen während der Laufzeit stattfinden.
- BIOS- und UEFI-Systeme identisches Verhalten erzeugen.

---

# 24. Referenzen

## ADRs

- ADR-BOOTCONTROL-SEPARATOR-0001 – Separator-Control
- ADR-BOOTLAYOUT-0001 – Layout Engine
- ADR-BOOTTHEME-0001 – Theme-System
- ADR-BOOTACCESS-0001 – Accessibility
- ADR-BOOTPERF-0002 – Deterministische UI-Verarbeitung

## NPSPECs

- NPSPEC-BOOTCONTROL-0001 – Boot Control Framework
- NPSPEC-BOOTCONTROL-0003 – Control Styling
- NPSPEC-BOOTCONTROL-0004 – Control Templates
- NPSPEC-BOOTLAYOUT-0001 – Boot Layout Engine
- NPSPEC-BOOTACCESSIBILITY-0001 – Accessibility Framework

---

# 25. Zusammenfassung

Das **Separator-Control** definiert das standardisierte Element zur visuellen Trennung von Bereichen innerhalb der NovaOS-Bootoberfläche. Es unterstützt horizontale und vertikale Ausrichtungen sowie optional beschriftete Trennlinien und integriert sich vollständig in das Theme-, Layout- und Accessibility-System. Durch seine rein dekorative Funktion, deterministische Darstellung und statische Speicherverwaltung sorgt es für eine klare und konsistente Strukturierung der Benutzeroberfläche auf BIOS- und UEFI-Systemen.