# NPSPEC-BOOTCONTROL-TEXTFIELD-0001
# Text Field

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTCONTROL-TEXTFIELD-0001 |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot UI Controls |
| Priorität | Kritisch |
| Gültigkeitsbereich | NovaOS Boot Manager |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTCONTROL-0001, NPSPEC-BOOTCONTROL-0002, NPSPEC-BOOTCONTROL-0003, NPSPEC-BOOTCONTROL-0004, NPSPEC-BOOTINPUT-0001, NPSPEC-BOOTINPUT-0006, NPSPEC-BOOTACCESSIBILITY-0001 |

---

# 1. Ziel

Diese Spezifikation definiert das **Text Field Control** der NovaOS Boot UI.

Ein Text Field dient der Eingabe, Bearbeitung und Anzeige kurzer Textinformationen innerhalb der Bootoberfläche.

Typische Einsatzbereiche:

- Passwortabfragen
- Gerätenamen
- Suchfelder
- Bootparameter
- Netzwerkkonfiguration
- Benutzereingaben
- Wiederherstellungsschlüssel
- Diagnoseparameter

---

# 2. Geltungsbereich

Diese Spezifikation gilt für sämtliche Textfelder.

Sie umfasst:

- Standard Text Field
- Passwortfeld
- Numerisches Feld
- Suchfeld
- Read-Only Feld
- Mehrzeiliges Feld (optional)

Nicht Bestandteil sind:

- Terminal
- Editor
- Konsolenfenster

---

# 3. Architektur

```text
Boot Control Framework

↓

Text Field

↓

Input Engine

↓

Caret Manager

↓

Renderer
```

---

# 4. Grundprinzip

Ein Textfeld verwaltet genau einen Textinhalt.

Die Eingabe erfolgt ausschließlich über das Input-System.

Der aktuelle Inhalt wird kontinuierlich validiert und dargestellt.

---

# 5. Aufgaben

Das Textfeld übernimmt:

- Texteingabe
- Textbearbeitung
- Cursorverwaltung
- Textauswahl
- Validierung
- Zwischenablage (optional)
- Accessibility
- Themeintegration

---

# 6. Aufbau

```text
Text Field

├── Background

├── Border

├── Placeholder

├── Text

├── Caret

├── Selection Highlight

└── Focus Indicator
```

---

# 7. Eigenschaften

Jedes Textfeld besitzt:

- ID
- Text
- Placeholder
- Maximale Länge
- ReadOnly
- Enabled
- Style
- Template
- Sichtbarkeit

Optional:

- Eingabefilter
- Validierungsregel
- Passwortmodus

---

# 8. Eingabemodi

Unterstützt werden:

- Standardtext
- Zahlen
- Hexadezimal
- Passwort
- Dateiname
- Suchtext

Der Eingabemodus bestimmt die zulässigen Zeichen.

---

# 9. Zustände

```text
Normal

Hovered

Focused

Pressed

Disabled

ReadOnly

Busy

Error
```

---

# 10. Cursor

Das Textfeld besitzt genau einen Caret.

Eigenschaften:

- blinkend
- positionsgenau
- tastatursteuerbar
- DPI-unabhängig

---

# 11. Textauswahl

Unterstützt werden:

- keine Auswahl
- Teilbereich
- gesamter Text

Die Auswahl wird farblich hervorgehoben.

---

# 12. Eingaben

Unterstützt:

- Tastatur
- Maus
- Touch
- Controller (virtuelle Tastatur)

Tastatur:

- Zeichen
- Backspace
- Delete
- Home
- End
- Pfeiltasten

---

# 13. Validierung

Die Validierung erfolgt:

```text
Eingabe

↓

Filter

↓

Validator

↓

Text übernehmen
```

Ungültige Eingaben werden abgelehnt.

---

# 14. Passwortmodus

Im Passwortmodus:

- werden Zeichen maskiert
- bleibt der Originaltext intern erhalten
- wird der Inhalt nicht protokolliert

Die Maskierung erfolgt unmittelbar nach der Eingabe.

---

# 15. Darstellung

Das Textfeld verwendet:

- Glass Material
- Focus Glow
- Placeholder
- animierten Caret
- Selection Highlight

Die Darstellung wird vollständig über Theme und Template definiert.

---

# 16. Animationen

Unterstützt werden:

- Focus Fade
- Caret Blink
- Border Transition
- Placeholder Fade
- Error Animation

Animationen beeinflussen ausschließlich die Darstellung.

---

# 17. Accessibility

Jedes Textfeld besitzt:

- Name
- Rolle
- Beschreibung
- aktueller Inhalt (außer Passwort)
- ReadOnly-Status
- Fehlerstatus

Passwortfelder dürfen den Inhalt niemals ausgeben.

---

# 18. Themeintegration

Das Theme definiert:

- Hintergrund
- Rahmen
- Caretfarbe
- Auswahlfarbe
- Schriftarten
- Schatten
- Transparenz

Es existieren keine fest codierten Farben.

---

# 19. Speicherverwaltung

Textfelder verwenden:

- Objektpools
- statische Templates
- gemeinsame Styles
- feste Textpuffer

Heap-Allokationen während der Laufzeit sind verboten.

---

# 20. Performance

Anforderungen:

- O(1)-Zeicheneingabe
- O(1)-Cursorbewegung
- O(1)-Löschen
- deterministische Laufzeit

Nur geänderte Zeichenbereiche werden neu gerendert.

---

# 21. Fehlerbehandlung

Folgende Fehler müssen erkannt werden:

- ungültige Zeichen
- Pufferüberlauf
- beschädigtes Template
- ungültiger Caret
- Nullpointer

Ungültige Eingaben werden verworfen.

---

# 22. Sicherheit

Das System verhindert:

- Pufferüberläufe
- ungültige Unicode-Sequenzen
- unautorisierte Änderungen im ReadOnly-Modus
- Protokollierung von Passwortinhalten
- inkonsistente Cursorpositionen

Sensible Eingaben werden nach der Verwendung sicher aus dem Speicher entfernt.

---

# 23. API

```c
typedef struct nova_text_field nova_text_field_t;

nova_result_t
nova_text_field_create(
    nova_text_field_t** field);

nova_result_t
nova_text_field_set_text(
    nova_text_field_t* field,
    const char* text);

const char*
nova_text_field_get_text(
    const nova_text_field_t* field);

nova_result_t
nova_text_field_set_placeholder(
    nova_text_field_t* field,
    const char* placeholder);

nova_result_t
nova_text_field_set_password_mode(
    nova_text_field_t* field,
    bool enabled);

nova_result_t
nova_text_field_set_readonly(
    nova_text_field_t* field,
    bool readonly);
```

---

# 24. Diagnosedaten

Zu erfassen sind:

- Textfeld-ID
- Zeichenanzahl
- Cursorposition
- Auswahlbereich
- Fokuswechsel
- Validierungsfehler
- Speicherverbrauch
- Fehleranzahl

Passwortinhalte dürfen niemals protokolliert werden.

---

# 25. Testfälle

Zu testen sind:

- Texteingabe
- Zeichen löschen
- Cursorbewegung
- Textauswahl
- Passwortmodus
- ReadOnly
- Validierung
- Tastatur
- Maus
- Themewechsel
- Accessibility
- Fehlerfälle

---

# 26. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- Texte korrekt eingegeben und bearbeitet werden können.
- Cursor und Auswahl jederzeit konsistent sind.
- Passwortfelder Eingaben zuverlässig maskieren.
- ReadOnly-Felder keine Änderungen zulassen.
- Accessibility vollständig unterstützt wird.
- keine Heap-Allokationen während der Laufzeit stattfinden.
- BIOS- und UEFI-Systeme identisches Verhalten erzeugen.

---

# 27. Referenzen

## ADRs

- ADR-BOOTCONTROL-TEXTFIELD-0001 – Text Field
- ADR-BOOTINPUT-0001 – Eingabearchitektur
- ADR-BOOTINPUT-0006 – Focus Management
- ADR-BOOTACCESS-0001 – Accessibility
- ADR-BOOTPERF-0002 – Deterministische UI-Verarbeitung

## NPSPECs

- NPSPEC-BOOTCONTROL-0001 – Boot Control Framework
- NPSPEC-BOOTCONTROL-0002 – Control State Model
- NPSPEC-BOOTCONTROL-0003 – Control Styling
- NPSPEC-BOOTCONTROL-0004 – Control Templates
- NPSPEC-BOOTINPUT-0001 – Input System
- NPSPEC-BOOTINPUT-0006 – Focus Management
- NPSPEC-BOOTACCESSIBILITY-0001 – Accessibility Framework

---

# 28. Zusammenfassung

Das **Text Field** definiert das standardisierte Eingabefeld der NovaOS-Bootoberfläche. Es unterstützt verschiedene Eingabemodi, Cursorverwaltung, Textauswahl, Passwortmaskierung, Validierung sowie vollständige Theme- und Accessibility-Integration. Durch deterministische Verarbeitung, feste Textpuffer und statische Speicherverwaltung entsteht ein sicheres, performantes und konsistentes Eingabesteuerelement für BIOS- und UEFI-basierte Bootumgebungen.