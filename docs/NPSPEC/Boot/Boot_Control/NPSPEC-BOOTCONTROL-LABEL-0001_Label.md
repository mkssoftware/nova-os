# NPSPEC-BOOTCONTROL-LABEL-0001
# Label

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTCONTROL-LABEL-0001 |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot UI Controls |
| Priorität | Hoch |
| Gültigkeitsbereich | NovaOS Boot Manager |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTCONTROL-0001, NPSPEC-BOOTCONTROL-0002, NPSPEC-BOOTCONTROL-0003, NPSPEC-BOOTCONTROL-0004, NPSPEC-BOOTRENDER-0001, NPSPEC-BOOTACCESSIBILITY-0001 |

---

# 1. Ziel

Diese Spezifikation definiert das **Label-Control** der NovaOS Boot UI.

Ein Label dient der Anzeige statischer oder dynamischer Textinformationen ohne direkte Benutzerinteraktion. Es wird verwendet, um andere Controls zu beschriften, Informationen darzustellen oder Statusmeldungen auszugeben.

Typische Einsatzbereiche:

- Überschriften
- Beschriftungen
- Statusmeldungen
- Hinweise
- Fehlermeldungen
- Beschreibungen
- Versionsinformationen
- Copyright-Hinweise

---

# 2. Geltungsbereich

Diese Spezifikation gilt für sämtliche Label.

Sie umfasst:

- Standard Label
- Überschrift
- Untertitel
- Status Label
- Hinweistext
- Mehrzeiliges Label
- Dynamisches Label

Nicht Bestandteil sind:

- Textfelder
- Buttons
- Hyperlinks

---

# 3. Architektur

```text
Boot Control Framework

↓

Label

↓

Text Renderer

↓

Framebuffer
```

---

# 4. Grundprinzip

Ein Label stellt ausschließlich Informationen dar.

Es besitzt keine direkte Benutzerinteraktion und kann standardmäßig weder fokussiert noch bearbeitet werden.

---

# 5. Aufgaben

Das Label übernimmt:

- Textdarstellung
- automatische Zeilenumbrüche
- Textausrichtung
- Themeintegration
- Accessibility
- Zustandsdarstellung
- Aktualisierung dynamischer Inhalte

---

# 6. Aufbau

```text
Label

├── Background (optional)

├── Text

└── Decoration (optional)
```

Dekorationen umfassen beispielsweise Trennlinien oder Symbole.

---

# 7. Eigenschaften

Jedes Label besitzt:

- ID
- Text
- Schriftstil
- Ausrichtung
- Sichtbarkeit
- Style
- Template

Optional:

- Icon
- Tooltip
- Beschreibung

---

# 8. Label-Typen

Unterstützt werden:

- Standard
- Header
- Subheader
- Caption
- Status
- Error
- Warning
- Success
- Information

Der Typ beeinflusst ausschließlich die Darstellung.

---

# 9. Zustände

```text
Normal

Disabled

Hidden
```

Optional:

```text
Highlighted

Warning

Error
```

Label besitzen standardmäßig keinen Hover- oder Pressed-Zustand.

---

# 10. Textdarstellung

Unterstützt werden:

- einzeilig
- mehrzeilig
- automatischer Zeilenumbruch
- Textabschneidung
- Ellipsis

Die Darstellung erfolgt DPI-unabhängig.

---

# 11. Ausrichtung

Unterstützt werden:

Horizontal:

- Links
- Zentriert
- Rechts

Vertikal:

- Oben
- Mitte
- Unten

---

# 12. Dynamische Inhalte

Labels können zur Laufzeit aktualisiert werden.

Beispiele:

- Fortschritt
- Status
- Zeit
- Versionsnummer
- Diagnosedaten

Eine Aktualisierung verändert nicht das Layout, sofern die maximale Größe eingehalten wird.

---

# 13. Darstellung

Das Label verwendet:

- Theme-Schriftarten
- Theme-Farben
- optionalen Hintergrund
- optionale Symbole
- High-DPI-Rendering

Die Darstellung erfolgt ausschließlich über Styles und Templates.

---

# 14. Animationen

Optional unterstützt:

- Fade
- Color Transition
- Status Transition

Animationen dürfen die Lesbarkeit nicht beeinträchtigen.

---

# 15. Accessibility

Jedes Label besitzt:

- Name
- Rolle
- Beschreibung

Labels können als Beschriftung für andere Controls verwendet werden.

Screenreader lesen den vollständigen Text vor.

---

# 16. Themeintegration

Das Theme definiert:

- Schriftarten
- Schriftgrößen
- Farben
- Transparenz
- Schatten
- Zeilenabstände

Das Label enthält keine fest codierten Designparameter.

---

# 17. Speicherverwaltung

Labels verwenden:

- gemeinsame Styles
- gemeinsame Templates
- zentrale Schriftobjekte
- statische Textpuffer (bei festen Texten)

Heap-Allokationen während der Laufzeit sind verboten.

---

# 18. Performance

Anforderungen:

- O(1)-Textänderung
- O(1)-Layoutaktualisierung
- O(1)-Stylewechsel
- deterministische Laufzeit

Nur geänderte Labels werden neu gerendert.

---

# 19. Fehlerbehandlung

Folgende Fehler müssen erkannt werden:

- ungültiger Text
- fehlender Font
- beschädigtes Template
- ungültige Ausrichtung
- Nullpointer

Kann ein Font nicht geladen werden, wird der Systemfont verwendet.

---

# 20. Sicherheit

Das System verhindert:

- ungültige UTF-8-Sequenzen
- Textüberläufe
- beschädigte Fontreferenzen
- inkonsistente Layoutdaten

Texte werden vor der Darstellung validiert.

---

# 21. API

```c
typedef struct nova_label nova_label_t;

typedef enum
{
    NOVA_LABEL_STANDARD,
    NOVA_LABEL_HEADER,
    NOVA_LABEL_SUBHEADER,
    NOVA_LABEL_CAPTION,
    NOVA_LABEL_STATUS,
    NOVA_LABEL_ERROR,
    NOVA_LABEL_WARNING,
    NOVA_LABEL_SUCCESS,
    NOVA_LABEL_INFORMATION
}
nova_label_type_t;

nova_result_t
nova_label_create(
    nova_label_type_t type,
    nova_label_t** label);

nova_result_t
nova_label_set_text(
    nova_label_t* label,
    const char* text);

const char*
nova_label_get_text(
    const nova_label_t* label);

nova_result_t
nova_label_set_alignment(
    nova_label_t* label,
    nova_alignment_t alignment);
```

---

# 22. Diagnosedaten

Zu erfassen sind:

- Label-ID
- Label-Typ
- Textlänge
- Aktualisierungen
- Renderzeit
- Speicherverbrauch
- Fehleranzahl

---

# 23. Testfälle

Zu testen sind:

- Einzeilig
- Mehrzeilig
- Zeilenumbruch
- Ellipsis
- Themewechsel
- Schriftgrößen
- Dynamische Inhalte
- Accessibility
- High-DPI
- Fehlerfälle

---

# 24. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- Labels Texte korrekt darstellen.
- automatische Zeilenumbrüche unterstützt werden.
- Theme und Templates vollständig verwendet werden.
- Accessibility vollständig integriert ist.
- High-DPI korrekt unterstützt wird.
- keine Heap-Allokationen während der Laufzeit stattfinden.
- BIOS- und UEFI-Systeme identisches Verhalten erzeugen.

---

# 25. Referenzen

## ADRs

- ADR-BOOTCONTROL-LABEL-0001 – Label-Control
- ADR-BOOTTHEME-0001 – Theme-System
- ADR-BOOTTEXT-0001 – Textdarstellung
- ADR-BOOTACCESS-0001 – Accessibility
- ADR-BOOTPERF-0002 – Deterministische UI-Verarbeitung

## NPSPECs

- NPSPEC-BOOTCONTROL-0001 – Boot Control Framework
- NPSPEC-BOOTCONTROL-0002 – Control State Model
- NPSPEC-BOOTCONTROL-0003 – Control Styling
- NPSPEC-BOOTCONTROL-0004 – Control Templates
- NPSPEC-BOOTRENDER-0001 – Rendering Engine
- NPSPEC-BOOTACCESSIBILITY-0001 – Accessibility Framework

---

# 26. Zusammenfassung

Das **Label-Control** definiert die standardisierte Darstellung statischer und dynamischer Textinformationen innerhalb der NovaOS-Bootoberfläche. Es unterstützt unterschiedliche Labeltypen, automatische Zeilenumbrüche, Theme-Integration, High-DPI-Rendering sowie vollständige Accessibility-Unterstützung. Durch die klare Trennung zwischen Inhalt, Darstellung und Layout sowie statische Speicherverwaltung entsteht ein konsistentes, performantes und leicht wiederverwendbares UI-Control für BIOS- und UEFI-basierte Bootumgebungen.