# NPSPEC-BOOTCONTROL-PASSWORD-0001
# Password Field

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTCONTROL-PASSWORD-0001 |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot UI Controls |
| Priorität | Kritisch |
| Gültigkeitsbereich | NovaOS Boot Manager |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTCONTROL-TEXTFIELD-0001, NPSPEC-BOOTCONTROL-0001, NPSPEC-BOOTINPUT-0001, NPSPEC-BOOTINPUT-0006, NPSPEC-BOOTACCESSIBILITY-0001 |

---

# 1. Ziel

Diese Spezifikation definiert das **Password Field** der NovaOS Boot UI.

Das Password Field ist eine spezialisierte Variante des Text Field Controls zur sicheren Eingabe vertraulicher Informationen. Es verhindert die Offenlegung sensibler Daten während der Eingabe, Verarbeitung und Speicherung.

Typische Einsatzbereiche:

- Boot-Passwort
- Festplattenverschlüsselung
- TPM-Freigabe
- Wiederherstellungsschlüssel
- Administratorpasswort
- Netzwerkanmeldung
- Sicherheitsabfragen

---

# 2. Geltungsbereich

Diese Spezifikation gilt für sämtliche Passwortfelder.

Sie umfasst:

- Boot-Passwort
- Administratorpasswort
- Verschlüsselungsschlüssel
- PIN-Eingabe
- Wiederherstellungsschlüssel

Nicht Bestandteil sind:

- normale Textfelder
- Terminaleingaben
- Konsolenpasswörter

---

# 3. Architektur

```text
Boot Control Framework

↓

Password Field

↓

Secure Input Buffer

↓

Validation

↓

Authentication
```

---

# 4. Grundprinzip

Ein Password Field speichert den tatsächlichen Eingabetext ausschließlich intern.

Die Darstellung erfolgt grundsätzlich maskiert.

Der Klartext darf niemals gezeichnet, protokolliert oder unverschlüsselt gespeichert werden.

---

# 5. Aufgaben

Das Password Field übernimmt:

- sichere Eingabe
- Maskierung
- Cursorverwaltung
- Validierung
- Speicherbereinigung
- Accessibility
- Themeintegration
- Ereignisverwaltung

---

# 6. Aufbau

```text
Password Field

├── Background

├── Border

├── Placeholder

├── Masked Text

├── Caret

├── Reveal Button (optional)

└── Focus Indicator
```

---

# 7. Eigenschaften

Jedes Password Field besitzt:

- ID
- Placeholder
- Maximale Länge
- Passwortpuffer
- Sichtbarkeit
- Style
- Template
- Enabled

Optional:

- Reveal Button
- Passwortregeln
- Stärkeanzeige

---

# 8. Maskierung

Standardmäßig wird jedes eingegebene Zeichen unmittelbar maskiert.

Beispiel:

```text
********
```

Optional darf das zuletzt eingegebene Zeichen für eine kurze, konfigurierbare Zeit sichtbar bleiben und wird anschließend automatisch maskiert.

---

# 9. Zustände

```text
Normal

Focused

Hovered

Pressed

Disabled

Busy

Error
```

Zusätzlich:

```text
Revealed
```

Der Zustand **Revealed** ist optional und zeitlich begrenzt.

---

# 10. Eingaben

Unterstützt:

- Tastatur
- Maus
- Touch
- Controller (virtuelle Tastatur)

Unterstützte Tasten:

- Zeichen
- Backspace
- Delete
- Home
- End
- Pfeiltasten

---

# 11. Cursor

Das Password Field besitzt genau einen Caret.

Eigenschaften:

- blinkend
- positionsgenau
- unabhängig von der Maskierung

---

# 12. Passwortspeicher

Das Passwort wird ausschließlich in einem geschützten Speicherbereich gehalten.

Anforderungen:

- keine Speicherkopien
- keine Debug-Ausgabe
- keine Auslagerung
- sichere Speicherlöschung

---

# 13. Reveal-Funktion

Optional kann ein Reveal Button angeboten werden.

Eigenschaften:

- nur temporäre Anzeige
- automatische Rückkehr zur Maskierung
- deaktivierbar
- konfigurierbar

Bei sicherheitskritischen Dialogen darf die Reveal-Funktion deaktiviert werden.

---

# 14. Validierung

Die Validierung erfolgt nach der Eingabe.

Unterstützt werden:

- Mindestlänge
- Maximallänge
- erlaubte Zeichen
- Passwortregeln
- externe Authentifizierung

---

# 15. Darstellung

Das Password Field verwendet:

- Glass Material
- Focus Glow
- Placeholder
- Caret
- Maskierungszeichen
- Error Highlight

Die Darstellung erfolgt vollständig über Theme und Template.

---

# 16. Animationen

Unterstützt werden:

- Focus Fade
- Caret Blink
- Border Transition
- Error Animation
- Reveal Transition

Animationen beeinflussen ausschließlich die Darstellung.

---

# 17. Accessibility

Das Password Field besitzt:

- Name
- Rolle
- Beschreibung
- Fehlerstatus

Der Passwortinhalt darf niemals an Screenreader ausgegeben werden.

Es wird ausschließlich gemeldet:

```text
Passwortfeld
```

oder

```text
Passwort eingegeben
```

---

# 18. Themeintegration

Das Theme definiert:

- Hintergrund
- Rahmen
- Caret
- Schrift
- Fokusfarbe
- Transparenz
- Schatten

Maskierungszeichen verwenden dieselbe Typografie wie regulärer Text.

---

# 19. Speicherverwaltung

Das Password Field verwendet:

- geschützte Speicherbereiche
- feste Puffer
- sichere Speicherlöschung

Heap-Allokationen während der Eingabe sind verboten.

Nach Abschluss werden sämtliche Passwortdaten überschrieben.

---

# 20. Performance

Anforderungen:

- O(1)-Zeicheneingabe
- O(1)-Maskierung
- O(1)-Cursorbewegung
- deterministische Laufzeit

Die Maskierung darf keine wahrnehmbare Verzögerung verursachen.

---

# 21. Fehlerbehandlung

Folgende Fehler müssen erkannt werden:

- Pufferüberlauf
- ungültige Zeichen
- beschädigter Passwortpuffer
- Nullpointer
- ungültige Reveal-Konfiguration

Bei Fehlern wird der Puffer sofort gelöscht.

---

# 22. Sicherheit

Das System verhindert:

- Klartextdarstellung
- Protokollierung
- Speicherkopien
- Zwischenablage
- Drag & Drop
- Debug-Ausgabe
- Auslagerung sensibler Daten

Alle Passwortdaten werden nach der Verwendung sicher überschrieben.

---

# 23. API

```c
typedef struct nova_password_field nova_password_field_t;

nova_result_t
nova_password_field_create(
    nova_password_field_t** field);

nova_result_t
nova_password_field_set_placeholder(
    nova_password_field_t* field,
    const char* placeholder);

nova_result_t
nova_password_field_enable_reveal(
    nova_password_field_t* field,
    bool enabled);

nova_result_t
nova_password_field_clear(
    nova_password_field_t* field);

bool
nova_password_field_empty(
    const nova_password_field_t* field);
```

---

# 24. Diagnosedaten

Zu erfassen sind:

- Feld-ID
- Eingabelänge
- Fokuswechsel
- Fehlversuche
- Validierungsfehler
- Speicherverbrauch
- Fehleranzahl

Es dürfen niemals Passwortinhalte oder Maskierungsdaten protokolliert werden.

---

# 25. Testfälle

Zu testen sind:

- Eingabe
- Maskierung
- Reveal-Funktion
- Speicherlöschung
- Passwortregeln
- Fokus
- Tastatur
- Touch
- Themewechsel
- Accessibility
- Fehlerfälle

---

# 26. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- Passwortinhalte niemals im Klartext dargestellt werden.
- sämtliche Passwortdaten nach der Nutzung sicher gelöscht werden.
- Reveal nur kontrolliert und optional verfügbar ist.
- Passwortinhalte niemals protokolliert oder kopiert werden.
- Accessibility keine sensiblen Informationen preisgibt.
- keine Heap-Allokationen während der Laufzeit stattfinden.
- BIOS- und UEFI-Systeme identisches Verhalten erzeugen.

---

# 27. Referenzen

## ADRs

- ADR-BOOTCONTROL-PASSWORD-0001 – Password Field
- ADR-BOOTSEC-0003 – Secure Credential Handling
- ADR-BOOTINPUT-0001 – Eingabearchitektur
- ADR-BOOTACCESS-0001 – Accessibility
- ADR-BOOTPERF-0002 – Deterministische UI-Verarbeitung

## NPSPECs

- NPSPEC-BOOTCONTROL-TEXTFIELD-0001 – Text Field
- NPSPEC-BOOTCONTROL-0001 – Boot Control Framework
- NPSPEC-BOOTINPUT-0001 – Input System
- NPSPEC-BOOTINPUT-0006 – Focus Management
- NPSPEC-BOOTACCESSIBILITY-0001 – Accessibility Framework

---

# 28. Zusammenfassung

Das **Password Field** definiert das standardisierte Eingabefeld für vertrauliche Informationen innerhalb der NovaOS-Bootoberfläche. Es gewährleistet durch sofortige Maskierung, geschützte Speicherbereiche, sichere Speicherlöschung und den Verzicht auf Protokollierung einen hohen Schutz sensibler Daten. Ergänzt durch vollständige Theme-Integration, Accessibility-Unterstützung und deterministische Verarbeitung bildet es die Grundlage für alle sicherheitsrelevanten Eingabedialoge auf BIOS- und UEFI-Systemen.