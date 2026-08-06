# NPSPEC-BOOTINPUT-0006
# Pointer Model

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTINPUT-0006 |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot Input System |
| Priorität | Kritisch |
| Gültigkeitsbereich | NovaOS Boot Manager |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTINPUT-0001, NPSPEC-BOOTINPUT-0002, NPSPEC-BOOTINPUT-0004, NPSPEC-BOOTINPUT-0005, NPSPEC-BOOTLAYOUT-0001, NPSPEC-BOOTCONTROL-0001 |

---

# 1. Ziel

Diese Spezifikation definiert das **Pointer Model** der NovaOS Boot UI.

Das Pointer Model beschreibt die einheitliche Verarbeitung sämtlicher zeigebasierten Eingaben innerhalb der Bootoberfläche. Es abstrahiert Maus-, Touch-, Stift- und zukünftige Zeigegeräte zu einem gemeinsamen Pointer-Modell und stellt sicher, dass sämtliche UI-Komponenten unabhängig vom eigentlichen Eingabegerät identisch arbeiten.

Das Pointer Model bildet die Grundlage für sämtliche Hit-Tests, Zeigerinteraktionen und Pointer-basierten Ereignisse.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für:

- USB-Maus
- PS/2-Maus
- UEFI Pointer
- Touchscreens
- Touchpads
- Grafiktabletts (zukünftig)
- Eingabestifte (zukünftig)

---

# 3. Ziele

Das Pointer Model muss:

- hardwareunabhängig arbeiten
- deterministisch reagieren
- mehrere Pointer unterstützen
- Capture unterstützen
- Hit Testing unterstützen
- zukünftige Geräte integrieren können

---

# 4. Architektur

```text
Pointer Device

↓

Pointer HAL

↓

Pointer Manager

↓

Pointer Model

↓

Hit Test

↓

Dispatcher

↓

Focused Control
```

---

# 5. Komponenten

Das Pointer Model besteht aus:

- Pointer Manager
- Cursor Manager
- Hit Test Engine
- Capture Manager
- Hover Manager
- Pointer Dispatcher
- Coordinate Mapper

---

# 6. Pointerarten

Unterstützt werden:

- Mouse Pointer
- Touch Pointer
- Pen Pointer
- Virtual Pointer
- Accessibility Pointer

Alle Pointer besitzen dieselbe interne Datenstruktur.

---

# 7. Pointerstruktur

Jeder Pointer besitzt:

- Pointer-ID
- Gerätetyp
- Position
- Status
- Buttons
- Modifier
- Zeitstempel
- Capture-Zustand

Optional:

- Druck
- Neigung
- Rotation

---

# 8. Pointer-Zustände

Unterstützt werden:

```text
Idle

Move

Hover

Pressed

Released

Captured

Hidden
```

Ein Pointer kann sich gleichzeitig nur in einem aktiven Primärzustand befinden.

---

# 9. Koordinatensystem

Alle Pointer verwenden ausschließlich logische UI-Koordinaten.

```text
Hardware

↓

HAL

↓

Logical UI Coordinates

↓

Layout

↓

Controls
```

Hardwarekoordinaten werden niemals direkt an Controls weitergegeben.

---

# 10. Pointerbewegung

Pointerbewegungen werden kontinuierlich verarbeitet.

Eigenschaften:

- deterministisch
- DPI-unabhängig
- kontinuierlich
- verlustfrei

Alle Bewegungen erzeugen PointerMove-Ereignisse.

---

# 11. Hit Testing

Vor jeder Zustellung erfolgt ein Hit Test.

```text
Pointer

↓

Scene Graph

↓

Visible Controls

↓

Topmost Control
```

Berücksichtigt werden:

- Sichtbarkeit
- Transparenz
- Clipping
- Z-Reihenfolge
- Eingabeberechtigung

---

# 12. Pointer Capture

Ein Control kann einen Pointer übernehmen.

Während Capture:

- erfolgen alle Pointer-Ereignisse an dasselbe Control
- bleibt Hit Testing deaktiviert
- wird Capture beim Loslassen automatisch beendet

Capture ist pointerbezogen und nicht global.

---

# 13. Hover

Unterstützt werden:

```text
Pointer Enter

↓

Pointer Move

↓

Pointer Leave
```

Hover verändert niemals den Fokus.

---

# 14. Mehrere Pointer

Das Modell unterstützt mehrere gleichzeitige Pointer.

Beispiele:

- Maus + Touch
- Zwei Touchkontakte
- Stift + Maus

Jeder Pointer besitzt eine eigene Pointer-ID.

---

# 15. Cursorverwaltung

Für Cursor-basierte Pointer werden unterstützt:

- Position
- Sichtbarkeit
- Cursorform
- Hotspot
- Theme

Touch Pointer besitzen standardmäßig keinen sichtbaren Cursor.

---

# 16. Pointer-Routing

Der Dispatcher bestimmt das Ziel anhand:

- Capture
- Hit Test
- Overlay
- Dialoge
- Fokusregeln

Pointerereignisse werden niemals direkt an mehrere Controls gleichzeitig zugestellt.

---

# 17. Accessibility

Das Pointer Model unterstützt:

- große Cursor
- alternative Pointer
- reduzierte Bewegung
- vereinfachtes Hit Testing

Accessibility verwendet dieselben Pointerstrukturen.

---

# 18. Sicherheit

Das System verhindert:

- ungültige Pointer-IDs
- doppelte Capture-Zustände
- Pointer außerhalb der Anzeige
- rekursive Pointerereignisse
- ungültige Koordinaten

Alle Pointerdaten werden vor der Verarbeitung validiert.

---

# 19. Speicherverwaltung

Das Pointer Model verwendet:

- feste Pointerobjekte
- statische Tabellen
- Objektpools

Heap-Allokationen während der Laufzeit sind verboten.

---

# 20. Performance

Anforderungen:

- O(1)-Pointer Update
- O(1)-Capture
- O(1)-Routing
- O(1)-Hit Test (am Dispatcher-Einstieg)

Die Verarbeitung muss unabhängig von der Anzahl vorhandener Controls deterministisch bleiben.

---

# 21. API

```c
typedef enum
{
    NOVA_POINTER_MOUSE,
    NOVA_POINTER_TOUCH,
    NOVA_POINTER_PEN,
    NOVA_POINTER_VIRTUAL,
    NOVA_POINTER_ACCESSIBILITY
}
nova_pointer_type_t;

typedef struct
{
    uint32_t pointer_id;
    nova_pointer_type_t type;
    nova_point_t position;
    uint32_t buttons;
    uint32_t modifiers;
    bool captured;
}
nova_pointer_t;

nova_result_t
nova_pointer_initialize(void);

nova_result_t
nova_pointer_update(
    const nova_pointer_t* pointer);

const nova_pointer_t*
nova_pointer_get(
    uint32_t pointer_id);

nova_result_t
nova_pointer_capture(
    uint32_t pointer_id,
    nova_control_t* control);

nova_result_t
nova_pointer_release(
    uint32_t pointer_id);
```

---

# 22. Diagnosedaten

Zu erfassen sind:

- Pointer-ID
- Pointertyp
- Position
- Capture-Zustand
- Ziel-Control
- Eventanzahl
- Latenz
- Fehler

---

# 23. Testfälle

Zu testen sind:

- Maus
- Touch
- Mehrere Pointer
- Pointer Capture
- Hover
- Hit Testing
- Cursorverwaltung
- Accessibility
- Gerätewechsel
- Fehlerfälle

---

# 24. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- sämtliche Pointergeräte identische Pointerobjekte erzeugen.
- ausschließlich logische UI-Koordinaten verwendet werden.
- Pointer Capture korrekt funktioniert.
- Hit Testing deterministisch arbeitet.
- mehrere Pointer gleichzeitig unterstützt werden.
- Accessibility vollständig integriert ist.
- keine dynamischen Speicherallokationen während der Laufzeit stattfinden.
- BIOS- und UEFI-Systeme identisches Verhalten erzeugen.

---

# 25. Referenzen

## ADRs

- ADR-BOOTINPUT-0001 – Einheitliche Eingabearchitektur
- ADR-BOOTINPUT-0006 – Pointer Model
- ADR-BOOTLAYOUT-0001 – Layout Engine
- ADR-BOOTACCESS-0001 – Accessibility

## NPSPECs

- NPSPEC-BOOTINPUT-0001 – Boot Input Architecture
- NPSPEC-BOOTINPUT-0002 – Unified Input Event Model
- NPSPEC-BOOTINPUT-0004 – Mouse Input
- NPSPEC-BOOTINPUT-0005 – Touch Input
- NPSPEC-BOOTLAYOUT-0001 – Boot Layout Engine
- NPSPEC-BOOTCONTROL-0001 – Boot Control Framework

---

# 26. Zusammenfassung

Das **Pointer Model** definiert die einheitliche Verarbeitung aller zeigebasierten Eingaben innerhalb der NovaOS-Bootoberfläche. Es abstrahiert Maus-, Touch-, Stift- und zukünftige Pointergeräte zu einer gemeinsamen Datenstruktur und stellt deterministische Mechanismen für Hit Testing, Pointer Capture, Hover und Routing bereit. Durch logische UI-Koordinaten, statische Speicherverwaltung und vollständige Hardwareabstraktion bildet es die Grundlage für konsistente Pointerinteraktionen auf BIOS- und UEFI-basierten Systemen.