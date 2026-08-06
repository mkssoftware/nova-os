# NPSPEC-BOOTINPUT-0003
# Keyboard Input

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTINPUT-0003 |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot Input System |
| Priorität | Kritisch |
| Gültigkeitsbereich | NovaOS Boot Manager |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTINPUT-0001, NPSPEC-BOOTINPUT-0002, NPSPEC-BOOTNAV-0001, NPSPEC-BOOTCONTROL-0001, NPSPEC-BOOTACCESSIBILITY-0001 |

---

# 1. Ziel

Diese Spezifikation definiert die Tastatureingabeverarbeitung der NovaOS-Bootoberfläche.

Das Keyboard-Subsystem abstrahiert sämtliche Tastaturhardware und Firmware-Schnittstellen zu einem einheitlichen Eingabemodell. Alle Tastaturereignisse werden unabhängig von BIOS oder UEFI identisch verarbeitet.

Die Tastatur ist das primäre Eingabegerät der Bootumgebung und muss jederzeit vollständig funktionsfähig sein.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für:

- BIOS-Tastaturen
- UEFI Simple Text Input
- UEFI Simple Text Input Ex
- USB HID Keyboards
- PS/2-Tastaturen
- Bluetooth-Keyboards (UEFI, optional)

---

# 3. Ziele

Das Keyboard-System muss:

- hardwareunabhängig arbeiten
- Unicode unterstützen
- deterministisch sein
- geringe Latenz besitzen
- Mehrsprachigkeit unterstützen
- Accessibility integrieren

---

# 4. Architektur

```text
Keyboard

↓

Firmware Driver

↓

Keyboard HAL

↓

Keyboard Manager

↓

Unified Input Event

↓

Dispatcher

↓

Focused Control
```

---

# 5. Komponenten

Das Keyboard-Subsystem besteht aus:

- Keyboard HAL
- Key Mapper
- Layout Manager
- Modifier Manager
- Repeat Manager
- Character Generator
- Event Generator

---

# 6. Unterstützte Tastaturtypen

Unterstützt werden:

- PS/2
- USB HID
- UEFI Keyboard
- Bluetooth HID (optional)

Alle Geräte erzeugen identische Keyboard Events.

---

# 7. Ereignistypen

Unterstützt werden:

```text
Key Down

Key Up

Character Input

Modifier Changed

Layout Changed

Repeat
```

Alle Ereignisse werden über das Unified Input Event Model verarbeitet.

---

# 8. Tastencodes

Intern werden ausschließlich logische NovaOS-Keycodes verwendet.

Firmware-spezifische Scan Codes werden bereits im HAL übersetzt.

Beispiele:

```text
NOVA_KEY_ENTER

NOVA_KEY_ESCAPE

NOVA_KEY_TAB

NOVA_KEY_UP

NOVA_KEY_DOWN

NOVA_KEY_LEFT

NOVA_KEY_RIGHT
```

---

# 9. Zeicheneingabe

Die Zeicheneingabe erfolgt ausschließlich über Unicode.

Unterstützt werden:

- ASCII
- UTF-8
- UTF-16 (intern optional)
- Unicode Code Points

Controls erhalten niemals Scan Codes.

---

# 10. Modifier

Unterstützt werden:

- Shift
- Ctrl
- Alt
- AltGr
- Caps Lock
- Num Lock
- Scroll Lock
- Meta (optional)

Modifier werden getrennt vom eigentlichen Tastendruck verwaltet.

---

# 11. Auto Repeat

Unterstützt werden:

- Initial Delay
- Repeat Rate
- Modifier Repeat

Die Parameter werden zentral konfiguriert.

Auto Repeat erzeugt reguläre Keyboard Events mit dem Flag `Repeat`.

---

# 12. Tastaturlayout

Das Layoutsystem unterstützt:

- QWERTZ
- QWERTY
- AZERTY
- Dvorak
- Colemak
- benutzerdefinierte Layouts

Ein Layoutwechsel erzeugt ein eigenes Ereignis.

---

# 13. Dead Keys

Dead Keys werden unterstützt.

Beispiel:

```text
´

+

e

↓

é
```

Die Verarbeitung erfolgt vollständig im Character Generator.

---

# 14. Navigationstasten

Die Navigation unterstützt:

- Pfeiltasten
- Tab
- Shift + Tab
- Enter
- Escape
- Home
- End
- Page Up
- Page Down

Die Zuordnung erfolgt über das Navigation Framework.

---

# 15. Systemtasten

Unterstützt werden:

- F1–F12
- Print Screen (optional)
- Pause (optional)

Beispiele:

```text
F1

↓

Hilfe
```

```text
F5

↓

Recovery
```

Die tatsächliche Belegung wird zentral konfiguriert.

---

# 16. Fokusintegration

Alle Tastatureingaben werden ausschließlich an das aktuell fokussierte Control gesendet.

Existiert kein Fokus, wird der Standardfokus verwendet.

---

# 17. Accessibility

Unterstützt werden:

- Sticky Keys
- Slow Keys
- Filter Keys
- Repeat Keys
- Screen Reader Events

Accessibility verändert niemals die eigentliche Eventstruktur.

---

# 18. Sicherheit

Das System verhindert:

- ungültige Keycodes
- doppelte Repeat-Events
- beschädigte Unicode-Sequenzen
- rekursive Tastaturereignisse
- manipulierte Modifier-Zustände

Alle Eingaben werden vor der Verarbeitung validiert.

---

# 19. Speicherverwaltung

Das Keyboard-System verwendet:

- feste Eventstrukturen
- statische Lookup-Tabellen
- Objektpools

Heap-Allokationen während der Laufzeit sind verboten.

---

# 20. Performance

Anforderungen:

- O(1)-Key Mapping
- O(1)-Modifier-Verarbeitung
- O(1)-Eventerzeugung
- deterministische Laufzeit

Die Eingabelatenz muss unabhängig vom aktiven Tastaturlayout konstant bleiben.

---

# 21. API

```c
typedef struct
{
    nova_key_t key;
    uint32_t modifiers;
    uint32_t unicode;
    bool repeat;
}
nova_keyboard_event_t;

nova_result_t
nova_keyboard_initialize(void);

nova_result_t
nova_keyboard_poll(void);

nova_result_t
nova_keyboard_set_layout(
    nova_keyboard_layout_t layout);

nova_keyboard_layout_t
nova_keyboard_get_layout(void);

bool
nova_keyboard_modifier_pressed(
    nova_modifier_t modifier);
```

---

# 22. Diagnosedaten

Zu erfassen sind:

- Tastaturtyp
- Layout
- Modifier
- Unicode-Zeichen
- Repeat-Events
- Eventanzahl
- Fehler
- Latenz

Passwortinhalte dürfen niemals protokolliert werden.

---

# 23. Testfälle

Zu testen sind:

- PS/2
- USB HID
- UEFI Keyboard
- Unicode
- Dead Keys
- Modifier
- Auto Repeat
- Layoutwechsel
- Accessibility
- Fehlerfälle

---

# 24. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- alle unterstützten Tastaturen identische Keyboard Events erzeugen.
- ausschließlich logische NovaOS-Keycodes verwendet werden.
- Unicode vollständig unterstützt wird.
- Modifier und Dead Keys korrekt verarbeitet werden.
- Auto Repeat deterministisch arbeitet.
- Accessibility vollständig integriert ist.
- keine dynamischen Speicherallokationen während der Laufzeit stattfinden.
- BIOS- und UEFI-Systeme identisches Verhalten erzeugen.

---

# 25. Referenzen

## ADRs

- ADR-BOOTINPUT-0001 – Einheitliche Eingabearchitektur
- ADR-BOOTINPUT-0003 – Keyboard Input
- ADR-BOOTNAV-0001 – Navigation Model
- ADR-BOOTACCESS-0001 – Accessibility

## NPSPECs

- NPSPEC-BOOTINPUT-0001 – Boot Input Architecture
- NPSPEC-BOOTINPUT-0002 – Unified Input Event Model
- NPSPEC-BOOTNAV-0001 – Boot Navigation Model
- NPSPEC-BOOTCONTROL-0001 – Boot Control Framework
- NPSPEC-BOOTACCESSIBILITY-0001 – Accessibility Framework

---

# 26. Zusammenfassung

Das **Keyboard Input**-Subsystem definiert die standardisierte Verarbeitung sämtlicher Tastatureingaben innerhalb der NovaOS-Bootoberfläche. Es abstrahiert unterschiedliche Firmware- und Hardwareimplementierungen zu einem gemeinsamen Ereignismodell, unterstützt Unicode, internationale Tastaturlayouts, Modifier, Dead Keys und Accessibility-Funktionen. Durch deterministische Verarbeitung, statische Datenstrukturen und eine vollständige Trennung von Hardware und UI bildet es die Grundlage für eine konsistente Tastaturbedienung auf BIOS- und UEFI-Systemen.