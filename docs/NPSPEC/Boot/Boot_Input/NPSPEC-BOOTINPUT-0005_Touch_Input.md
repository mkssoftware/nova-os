# NPSPEC-BOOTINPUT-0005
# Touch Input

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTINPUT-0005 |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot Input System |
| Priorität | Hoch |
| Gültigkeitsbereich | NovaOS Boot Manager |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTINPUT-0001, NPSPEC-BOOTINPUT-0002, NPSPEC-BOOTCONTROL-0001, NPSPEC-BOOTLAYOUT-0001, NPSPEC-BOOTACCESSIBILITY-0001 |

---

# 1. Ziel

Diese Spezifikation definiert die Verarbeitung von Touch-Eingaben innerhalb der NovaOS-Bootoberfläche.

Das Touch-Subsystem abstrahiert sämtliche Touch-Hardware zu einem einheitlichen Eingabemodell. Alle Touch-Geräte erzeugen standardisierte Touch-Ereignisse, unabhängig davon, ob sie über BIOS-, UEFI- oder zukünftige Firmware-Schnittstellen bereitgestellt werden.

Touch-Eingaben ergänzen Tastatur und Maus und ermöglichen die vollständige Bedienung der grafischen Bootoberfläche auf Touch-fähigen Geräten.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für:

- UEFI Touch Devices
- USB HID Touch
- HID Multitouch
- integrierte Touchscreens
- externe Touch-Monitore

---

# 3. Ziele

Das Touch-System muss:

- hardwareunabhängig arbeiten
- deterministisch reagieren
- geringe Latenz besitzen
- Gesten unterstützen
- Mehrfingereingaben verwalten
- Accessibility integrieren

---

# 4. Architektur

```text
Touch Device

↓

Firmware Driver

↓

Touch HAL

↓

Touch Manager

↓

Unified Touch Event

↓

Gesture Engine

↓

Dispatcher

↓

Focused Control
```

---

# 5. Komponenten

Das Touch-Subsystem besteht aus:

- Touch HAL
- Touch Manager
- Contact Manager
- Gesture Engine
- Capture Manager
- Event Generator

---

# 6. Unterstützte Geräte

Unterstützt werden:

- Single Touch
- Multi Touch
- HID Touch
- UEFI Touch

Alle Geräte erzeugen identische Touch-Ereignisse.

---

# 7. Ereignistypen

Unterstützt werden:

```text
Touch Begin

Touch Update

Touch End

Touch Cancel

Gesture Begin

Gesture Update

Gesture End
```

Alle Ereignisse werden über das Unified Input Event Model verarbeitet.

---

# 8. Koordinatensystem

Touch-Koordinaten werden ausschließlich in logische UI-Koordinaten umgerechnet.

```text
Touch Hardware

↓

HAL

↓

Logical Coordinates

↓

Controls
```

Die Hardwareauflösung bleibt vollständig verborgen.

---

# 9. Touch-Kontakte

Jeder Kontakt besitzt:

- Contact ID
- Position
- Druck (optional)
- Kontaktfläche (optional)
- Zeitstempel

Jeder aktive Finger wird eindeutig identifiziert.

---

# 10. Mehrfingereingaben

Unterstützt werden:

- Single Touch
- Zwei Finger
- Mehrere Finger (optional)

Die maximale Anzahl gleichzeitiger Kontakte wird zentral konfiguriert.

---

# 11. Gesten

Unterstützt werden:

- Tap
- Double Tap
- Long Press
- Drag
- Swipe

Optional:

- Pinch
- Rotate

Gesten werden ausschließlich durch die Gesture Engine erkannt.

---

# 12. Touch Capture

Controls können Touch Capture übernehmen.

Beispiele:

- Slider
- Scroll View
- Listen
- Drag-Operationen

Während Capture erhält ausschließlich das aktive Control Touch-Ereignisse des entsprechenden Kontakts.

---

# 13. Hit Testing

Vor jeder Zustellung erfolgt ein Hit Test.

```text
Touch Position

↓

Layout

↓

Control

↓

Touch Event
```

Das oberste sichtbare Control erhält den Touch-Kontakt.

---

# 14. Touch-Feedback

Optional unterstützt:

- visuelle Berührungspunkte
- Ripple-Effekt
- Focus Glow

Feedback wird vollständig über das Theme gesteuert.

---

# 15. Touch-Navigation

Touch unterstützt:

- direkte Auswahl
- Scrollen
- Wischen
- Kontextmenü per Long Press

Die Navigation erfolgt über dieselben Controls wie bei Maus und Tastatur.

---

# 16. Accessibility

Unterstützt werden:

- große Touch-Ziele
- verzögerte Berührung
- alternative Gesten
- vereinfachte Navigation

Accessibility verändert nicht die Touch-Ereignisstruktur.

---

# 17. Sicherheit

Das System verhindert:

- ungültige Kontakt-IDs
- rekursive Touch-Ereignisse
- doppelte Kontakte
- beschädigte Gestenerkennung
- ungültige Koordinaten

Alle Touch-Ereignisse werden validiert.

---

# 18. Speicherverwaltung

Das Touch-System verwendet:

- feste Kontaktstrukturen
- statische Eventobjekte
- Objektpools

Heap-Allokationen während der Laufzeit sind verboten.

---

# 19. Performance

Anforderungen:

- O(1)-Kontaktverwaltung
- O(1)-Hit Test
- O(1)-Dispatch
- deterministische Laufzeit

Die Verarbeitung muss unabhängig von der Anzahl aktiver Kontakte konstant bleiben.

---

# 20. API

```c
typedef struct
{
    uint32_t contact_id;
    nova_point_t position;
    float pressure;
    float area;
}
nova_touch_contact_t;

typedef struct
{
    uint32_t contact_count;
    nova_touch_contact_t contacts[NOVA_MAX_TOUCH_CONTACTS];
}
nova_touch_event_t;

nova_result_t
nova_touch_initialize(void);

nova_result_t
nova_touch_poll(void);

uint32_t
nova_touch_contact_count(void);

bool
nova_touch_capture(
    nova_control_t* control,
    uint32_t contact_id);

nova_result_t
nova_touch_release_capture(
    uint32_t contact_id);
```

---

# 21. Diagnosedaten

Zu erfassen sind:

- Gerätetyp
- Anzahl Kontakte
- Kontakt-IDs
- Gesten
- Capture-Zustände
- Eventanzahl
- Fehler
- Eingabelatenz

---

# 22. Testfälle

Zu testen sind:

- Single Touch
- Multi Touch
- Tap
- Double Tap
- Long Press
- Drag
- Swipe
- Capture
- Accessibility
- Fehlerfälle

---

# 23. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- alle unterstützten Touch-Geräte identische Touch-Ereignisse erzeugen.
- ausschließlich logische UI-Koordinaten verwendet werden.
- Mehrfingereingaben korrekt verarbeitet werden.
- Gestenerkennung zuverlässig arbeitet.
- Touch Capture deterministisch funktioniert.
- Accessibility vollständig integriert ist.
- keine dynamischen Speicherallokationen während der Laufzeit stattfinden.
- BIOS- und UEFI-Systeme identisches Verhalten erzeugen.

---

# 24. Referenzen

## ADRs

- ADR-BOOTINPUT-0001 – Einheitliche Eingabearchitektur
- ADR-BOOTINPUT-0005 – Touch Input
- ADR-BOOTLAYOUT-0001 – Layout Engine
- ADR-BOOTACCESS-0001 – Accessibility

## NPSPECs

- NPSPEC-BOOTINPUT-0001 – Boot Input Architecture
- NPSPEC-BOOTINPUT-0002 – Unified Input Event Model
- NPSPEC-BOOTCONTROL-0001 – Boot Control Framework
- NPSPEC-BOOTLAYOUT-0001 – Boot Layout Engine
- NPSPEC-BOOTACCESSIBILITY-0001 – Accessibility Framework

---

# 25. Zusammenfassung

Das **Touch Input**-Subsystem definiert die standardisierte Verarbeitung sämtlicher Touch-Eingaben innerhalb der NovaOS-Bootoberfläche. Es abstrahiert unterschiedliche Touch-Hardware zu einem gemeinsamen Ereignismodell, unterstützt Mehrfingereingaben, Gestenerkennung, Touch Capture und vollständige Accessibility. Durch deterministische Ereignisverarbeitung, zentrale Kontaktverwaltung und statische Speicherstrukturen bildet es die Grundlage für eine konsistente Touch-Bedienung auf BIOS- und UEFI-basierten Geräten.