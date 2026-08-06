# NPSPEC-BOOTINPUT-0004
# Mouse Input

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTINPUT-0004 |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot Input System |
| Priorität | Hoch |
| Gültigkeitsbereich | NovaOS Boot Manager |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTINPUT-0001, NPSPEC-BOOTINPUT-0002, NPSPEC-BOOTCONTROL-0001, NPSPEC-BOOTLAYOUT-0001, NPSPEC-BOOTACCESSIBILITY-0001 |

---

# 1. Ziel

Diese Spezifikation definiert die Verarbeitung von Mauseingaben innerhalb der NovaOS-Bootoberfläche.

Das Mouse-Subsystem abstrahiert sämtliche Mausgeräte und Firmware-Schnittstellen zu einem einheitlichen Eingabemodell. Unabhängig davon, ob die Eingaben von einer PS/2-Maus, einer USB-HID-Maus oder einem UEFI-Pointer-Protokoll stammen, werden identische Pointer-Ereignisse erzeugt.

Die Maus dient als primäres Zeigegerät zur Interaktion mit grafischen Oberflächenelementen.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für:

- PS/2-Maus
- USB HID Mouse
- UEFI Simple Pointer Protocol
- UEFI Absolute Pointer Protocol
- Bluetooth HID Mouse (optional)

---

# 3. Ziele

Das Mouse-System muss:

- hardwareunabhängig arbeiten
- deterministisch reagieren
- geringe Latenz besitzen
- präzise Zeigerbewegungen liefern
- mehrere Maustasten unterstützen
- zukünftige Zeigegeräte integrieren können

---

# 4. Architektur

```text
Mouse

↓

Firmware Driver

↓

Pointer HAL

↓

Pointer Manager

↓

Unified Pointer Event

↓

Dispatcher

↓

Focused Control
```

---

# 5. Komponenten

Das Mouse-Subsystem besteht aus:

- Pointer HAL
- Cursor Manager
- Pointer Event Generator
- Button Manager
- Wheel Manager
- Capture Manager

---

# 6. Unterstützte Geräte

Unterstützt werden:

- PS/2
- USB HID
- UEFI Pointer
- Bluetooth HID (optional)

Alle Geräte erzeugen dieselben Pointer Events.

---

# 7. Ereignistypen

Unterstützt werden:

```text
Pointer Move

Pointer Down

Pointer Up

Pointer Enter

Pointer Leave

Wheel

Capture Begin

Capture End
```

Alle Ereignisse werden über das Unified Input Event Model verarbeitet.

---

# 8. Koordinatensystem

Alle Positionen verwenden ausschließlich logische UI-Koordinaten.

```text
Firmware

↓

HAL

↓

Logical Coordinates

↓

Controls
```

Hardwareauflösungen werden vollständig abstrahiert.

---

# 9. Maustasten

Unterstützt werden:

- Linke Taste
- Rechte Taste
- Mittlere Taste
- Zusatztaste 1 (optional)
- Zusatztaste 2 (optional)

Nicht unterstützte Tasten werden ignoriert.

---

# 10. Zeigerbewegung

Die Bewegung erfolgt:

- pixelgenau
- DPI-unabhängig
- kontinuierlich
- deterministisch

Pointerbeschleunigung ist standardmäßig deaktiviert.

---

# 11. Scrollrad

Unterstützt werden:

- Vertikales Scrollen
- Horizontales Scrollen (optional)
- Hochauflösende Scrollwerte (optional)

Scrollereignisse werden unabhängig von der Cursorposition verarbeitet.

---

# 12. Cursor

Der Cursor wird zentral verwaltet.

Eigenschaften:

- Position
- Sichtbarkeit
- Typ
- Hotspot
- Status

Alle Controls verwenden denselben Cursor.

---

# 13. Pointer Capture

Controls können Pointer Capture übernehmen.

Beispiele:

- Slider
- Scrollbar
- Drag & Drop
- Fensterbewegung (zukünftig)

Während Capture erhält ausschließlich das aktive Control Pointer-Ereignisse.

---

# 14. Hit Testing

Vor jeder Zustellung erfolgt ein Hit Test.

```text
Cursor

↓

Layout

↓

Control

↓

Event
```

Nur das oberste sichtbare Control erhält das Ereignis.

---

# 15. Hover

Unterstützt werden:

```text
Enter

Move

Leave
```

Hover erzeugt keine Fokusänderung.

---

# 16. Doppelklick

Optional unterstützt:

- Double Click
- Triple Click

Zeitfenster und maximale Distanz werden zentral konfiguriert.

---

# 17. Accessibility

Unterstützt werden:

- große Cursor
- hohe Kontraste
- alternative Cursorformen
- reduzierte Bewegung

Accessibility verändert nicht das Pointer-Ereignismodell.

---

# 18. Sicherheit

Das System verhindert:

- ungültige Koordinaten
- rekursive Pointer-Ereignisse
- mehrfaches Capture
- beschädigte Gerätezustände
- ungültige Tastenkombinationen

Alle Eingaben werden validiert.

---

# 19. Speicherverwaltung

Das Mouse-System verwendet:

- feste Eventstrukturen
- statische Cursorobjekte
- Objektpools

Heap-Allokationen während der Laufzeit sind verboten.

---

# 20. Performance

Anforderungen:

- O(1)-Pointer Update
- O(1)-Hit Test
- O(1)-Dispatch
- deterministische Laufzeit

Die Cursorbewegung muss unabhängig von der Anzahl der Controls flüssig bleiben.

---

# 21. API

```c
typedef struct
{
    nova_point_t position;
    uint32_t buttons;
    int32_t wheel_x;
    int32_t wheel_y;
}
nova_pointer_event_t;

nova_result_t
nova_mouse_initialize(void);

nova_result_t
nova_mouse_poll(void);

nova_point_t
nova_mouse_position(void);

bool
nova_mouse_button_pressed(
    nova_mouse_button_t button);

nova_result_t
nova_mouse_set_capture(
    nova_control_t* control);

nova_result_t
nova_mouse_release_capture(void);
```

---

# 22. Diagnosedaten

Zu erfassen sind:

- Gerätetyp
- Cursorposition
- Tastenstatus
- Scrollereignisse
- Capture-Zustand
- Eventanzahl
- Fehler
- Eingabelatenz

---

# 23. Testfälle

Zu testen sind:

- PS/2-Maus
- USB HID
- UEFI Pointer
- Cursorbewegung
- Hit Testing
- Pointer Capture
- Scrollrad
- Hover
- Accessibility
- Fehlerfälle

---

# 24. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- alle unterstützten Mausgeräte identische Pointer-Ereignisse erzeugen.
- ausschließlich logische UI-Koordinaten verwendet werden.
- Pointer Capture korrekt funktioniert.
- Hit Testing deterministisch arbeitet.
- Cursordarstellung zentral verwaltet wird.
- Accessibility vollständig integriert ist.
- keine dynamischen Speicherallokationen während der Laufzeit stattfinden.
- BIOS- und UEFI-Systeme identisches Verhalten erzeugen.

---

# 25. Referenzen

## ADRs

- ADR-BOOTINPUT-0001 – Einheitliche Eingabearchitektur
- ADR-BOOTINPUT-0004 – Mouse Input
- ADR-BOOTLAYOUT-0001 – Layout Engine
- ADR-BOOTACCESS-0001 – Accessibility

## NPSPECs

- NPSPEC-BOOTINPUT-0001 – Boot Input Architecture
- NPSPEC-BOOTINPUT-0002 – Unified Input Event Model
- NPSPEC-BOOTCONTROL-0001 – Boot Control Framework
- NPSPEC-BOOTLAYOUT-0001 – Boot Layout Engine
- NPSPEC-BOOTACCESSIBILITY-0001 – Accessibility Framework

---

# 26. Zusammenfassung

Das **Mouse Input**-Subsystem definiert die standardisierte Verarbeitung sämtlicher Mauseingaben innerhalb der NovaOS-Bootoberfläche. Es abstrahiert verschiedene Maus- und Pointer-Geräte zu einem gemeinsamen Ereignismodell, unterstützt präzise Zeigerbewegungen, Hit Testing, Pointer Capture und Scrollereignisse sowie vollständige Accessibility. Durch deterministische Verarbeitung, zentrale Cursorverwaltung und statische Speicherstrukturen entsteht eine konsistente und performante Eingabeschicht für BIOS- und UEFI-basierte Bootumgebungen.