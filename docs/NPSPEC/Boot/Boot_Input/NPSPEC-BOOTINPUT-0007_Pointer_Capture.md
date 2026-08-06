# NPSPEC-BOOTINPUT-0007
# Pointer Capture

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTINPUT-0007 |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot Input System |
| Priorität | Kritisch |
| Gültigkeitsbereich | NovaOS Boot Manager |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTINPUT-0001, NPSPEC-BOOTINPUT-0002, NPSPEC-BOOTINPUT-0006, NPSPEC-BOOTCONTROL-0001, NPSPEC-BOOTLAYOUT-0001 |

---

# 1. Ziel

Diese Spezifikation definiert das **Pointer Capture System** der NovaOS Boot UI.

Pointer Capture ermöglicht es einem Control, sämtliche Pointer-Ereignisse eines bestimmten Zeigers exklusiv zu empfangen, unabhängig davon, ob sich der Pointer während einer Interaktion außerhalb der ursprünglichen Control-Grenzen bewegt.

Dies ist insbesondere für Drag-&-Drop-Operationen, Slider, Scrollbars und ähnliche kontinuierliche Interaktionen erforderlich.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für:

- Maus
- Touch
- Pen
- virtuelle Pointer
- zukünftige Pointergeräte

---

# 3. Ziele

Das Pointer-Capture-System muss:

- deterministisch arbeiten
- pointerbezogen sein
- mehrere Pointer gleichzeitig unterstützen
- automatisch freigegeben werden können
- unabhängig vom Hit Testing funktionieren
- hardwareunabhängig sein

---

# 4. Architektur

```text
Pointer Device

↓

Pointer Model

↓

Capture Manager

↓

Captured Control

↓

Event Dispatcher
```

---

# 5. Komponenten

Das Pointer-Capture-System besteht aus:

- Capture Manager
- Pointer Registry
- Capture Table
- Dispatcher
- Validation Layer

---

# 6. Grundprinzip

Pointer Capture bindet einen einzelnen Pointer an genau ein Control.

Während des Captures werden sämtliche Pointer-Ereignisse dieses Pointers ausschließlich an das gebundene Control zugestellt.

Andere Controls erhalten keine Pointer-Ereignisse dieses Pointers.

---

# 7. Capture-Lebenszyklus

```text
Pointer Down

↓

Capture Begin

↓

Pointer Move

↓

Pointer Move

↓

Pointer Up

↓

Capture End
```

Capture endet automatisch nach Abschluss der Interaktion oder explizit durch das Control.

---

# 8. Capture-Zustände

Unterstützt werden:

```text
Inactive

Pending

Captured

Released

Cancelled
```

Ein Pointer kann sich zu jedem Zeitpunkt nur in einem Capture-Zustand befinden.

---

# 9. Capture-Regeln

Es gelten folgende Regeln:

- Ein Pointer kann nur von einem Control gleichzeitig erfasst werden.
- Ein Control kann mehrere Pointer gleichzeitig erfassen.
- Capture ist immer pointerbezogen.
- Capture überschreibt das normale Hit Testing.
- Capture endet spätestens mit dem Ende des Pointer-Kontakts.

---

# 10. Pointer-Routing

Während eines aktiven Captures erfolgt das Routing wie folgt:

```text
Pointer Event

↓

Capture Manager

↓

Captured Control
```

Das Hit Testing wird in diesem Fall übersprungen.

---

# 11. Mehrere Pointer

Das System unterstützt mehrere gleichzeitige Pointer.

Beispiele:

```text
Pointer 1

↓

Slider A
```

```text
Pointer 2

↓

Scroll View
```

Jeder Pointer besitzt einen unabhängigen Capture-Zustand.

---

# 12. Hit Testing

Vor Capture:

```text
Pointer

↓

Hit Test

↓

Control
```

Während Capture:

```text
Pointer

↓

Capture Manager

↓

Captured Control
```

Nach Freigabe wird wieder das normale Hit Testing verwendet.

---

# 13. Freigabe

Capture endet durch:

- Pointer Up
- Touch End
- explizite Freigabe
- Geräteverlust
- Dialogwechsel
- Fehlerzustand

Ein freigegebener Pointer wird sofort wieder über das normale Routing verarbeitet.

---

# 14. Abbruch

Capture wird automatisch abgebrochen bei:

- Geräteentfernung
- Pointer Cancel
- Control-Zerstörung
- Szenenwechsel
- Recovery

Alle betroffenen Controls erhalten ein Capture-Cancel-Ereignis.

---

# 15. Sicherheit

Das System verhindert:

- mehrfaches Capture desselben Pointers
- Capture ungültiger Pointer
- rekursive Capture-Anforderungen
- Capture zerstörter Controls
- inkonsistente Capture-Zustände

Jede Capture-Anforderung wird vor der Aktivierung validiert.

---

# 16. Accessibility

Pointer Capture unterstützt:

- alternative Pointer
- Touch-Hilfen
- vereinfachte Gesten
- große Eingabebereiche

Accessibility verändert nicht das Capture-Modell.

---

# 17. Speicherverwaltung

Das Capture-System verwendet:

- feste Capture-Einträge
- statische Tabellen
- Objektpools

Heap-Allokationen während der Laufzeit sind verboten.

---

# 18. Performance

Anforderungen:

- O(1)-Capture
- O(1)-Freigabe
- O(1)-Lookup
- deterministische Laufzeit

Die Verarbeitung darf unabhängig von der Anzahl aktiver Controls konstant bleiben.

---

# 19. API

```c
typedef enum
{
    NOVA_CAPTURE_INACTIVE,
    NOVA_CAPTURE_PENDING,
    NOVA_CAPTURE_ACTIVE,
    NOVA_CAPTURE_RELEASED,
    NOVA_CAPTURE_CANCELLED
}
nova_capture_state_t;

nova_result_t
nova_pointer_capture(
    uint32_t pointer_id,
    nova_control_t* control);

nova_result_t
nova_pointer_release(
    uint32_t pointer_id);

bool
nova_pointer_is_captured(
    uint32_t pointer_id);

nova_control_t*
nova_pointer_capture_owner(
    uint32_t pointer_id);

nova_capture_state_t
nova_pointer_capture_state(
    uint32_t pointer_id);
```

---

# 20. Diagnosedaten

Zu erfassen sind:

- Pointer-ID
- Capture-Status
- Capture-Besitzer
- Beginn
- Ende
- Dauer
- Abbrüche
- Fehler

---

# 21. Testfälle

Zu testen sind:

- Maus-Capture
- Touch-Capture
- Pen-Capture
- Mehrere Pointer
- Capture-Freigabe
- Geräteverlust
- Szenenwechsel
- Accessibility
- Fehlerfälle

---

# 22. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- Pointer Capture deterministisch arbeitet.
- pro Pointer maximal ein Capture aktiv sein kann.
- mehrere Pointer gleichzeitig unterstützt werden.
- Capture automatisch beendet wird.
- das normale Hit Testing nach der Freigabe wieder aktiv ist.
- Accessibility vollständig unterstützt wird.
- keine Heap-Allokationen während der Laufzeit stattfinden.
- BIOS- und UEFI-Systeme identisches Verhalten erzeugen.

---

# 23. Referenzen

## ADRs

- ADR-BOOTINPUT-0001 – Einheitliche Eingabearchitektur
- ADR-BOOTINPUT-0007 – Pointer Capture
- ADR-BOOTLAYOUT-0001 – Layout Engine
- ADR-BOOTACCESS-0001 – Accessibility

## NPSPECs

- NPSPEC-BOOTINPUT-0001 – Boot Input Architecture
- NPSPEC-BOOTINPUT-0002 – Unified Input Event Model
- NPSPEC-BOOTINPUT-0006 – Pointer Model
- NPSPEC-BOOTCONTROL-0001 – Boot Control Framework
- NPSPEC-BOOTLAYOUT-0001 – Boot Layout Engine

---

# 24. Zusammenfassung

Das **Pointer Capture System** definiert den standardisierten Mechanismus zur exklusiven Zuordnung eines Pointers zu einem UI-Control innerhalb der NovaOS-Bootoberfläche. Während eines aktiven Captures werden sämtliche Pointer-Ereignisse direkt an das zugehörige Control weitergeleitet und das reguläre Hit Testing ausgesetzt. Durch pointerbezogene Verwaltung, automatische Freigabe, Unterstützung mehrerer gleichzeitiger Pointer sowie deterministische Verarbeitung bildet das System die Grundlage für zuverlässige Drag-, Scroll- und kontinuierliche Interaktionen auf BIOS- und UEFI-basierten Systemen.