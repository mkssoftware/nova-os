# NPSPEC-BOOTINPUT-0009
# Directional Focus Navigation

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTINPUT-0009 |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot Input System |
| Priorität | Kritisch |
| Gültigkeitsbereich | NovaOS Boot Manager |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTINPUT-0008, NPSPEC-BOOTNAV-0001, NPSPEC-BOOTLAYOUT-0001, NPSPEC-BOOTCONTROL-0001, NPSPEC-BOOTACCESSIBILITY-0001 |

---

# 1. Ziel

Diese Spezifikation definiert die **Directional Focus Navigation** der NovaOS Boot UI.

Die Directional Focus Navigation ermöglicht die räumliche Navigation zwischen fokussierbaren Controls anhand ihrer Position innerhalb des Layouts. Sie stellt sicher, dass Eingaben über Tastatur, Controller oder andere richtungsbasierte Geräte intuitiv und deterministisch verarbeitet werden.

Das System bildet die Grundlage für die Navigation mit Pfeiltasten, D-Pads und Analogsticks.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für:

- Boot Manager
- Recovery
- Installer
- Dialoge
- Overlays
- sämtliche fokussierbaren Controls

---

# 3. Ziele

Das System muss:

- richtungsbasiert arbeiten
- deterministisch navigieren
- layoutunabhängig funktionieren
- Controller unterstützen
- Tastatur unterstützen
- Accessibility integrieren

---

# 4. Architektur

```text
Keyboard / Controller

↓

Input System

↓

Focus Manager

↓

Directional Navigation Engine

↓

Focus Resolver

↓

Focused Control
```

---

# 5. Komponenten

Das Navigationssystem besteht aus:

- Navigation Engine
- Focus Resolver
- Direction Calculator
- Candidate Filter
- Navigation Graph
- Accessibility Bridge

---

# 6. Grundprinzip

Ausgehend vom aktuell fokussierten Control wird anhand einer gewünschten Richtung das am besten geeignete Ziel-Control ermittelt.

Die Navigation basiert auf:

- Position
- Richtung
- Sichtbarkeit
- Fokusfähigkeit
- Priorität

---

# 7. Richtungen

Unterstützt werden:

```text
Up

Down

Left

Right
```

Optional:

```text
Forward

Backward
```

---

# 8. Navigationsquellen

Unterstützt werden:

- Pfeiltasten
- D-Pad
- Analogstick
- Accessibility Navigation

Alle Quellen erzeugen identische Navigationsereignisse.

---

# 9. Zielauswahl

Ein Ziel-Control muss:

- sichtbar sein
- aktiviert sein
- fokussierbar sein
- innerhalb desselben Focus Scope liegen

Nicht zulässige Controls werden ignoriert.

---

# 10. Kandidatenermittlung

Die Navigation berücksichtigt:

- geometrische Richtung
- Abstand
- Überlappung
- Navigationspriorität
- explizite Nachbarn

Explizite Navigationsbeziehungen haben Vorrang vor der geometrischen Berechnung.

---

# 11. Navigationsgraph

Optional kann ein statischer Navigationsgraph definiert werden.

Beispiel:

```text
Button A

↓

Button B

↓

Button C
```

Der Graph überschreibt die automatische Berechnung.

---

# 12. Layoutintegration

Die Navigation arbeitet unabhängig vom Layouttyp.

Unterstützt werden:

- Stack Layout
- Grid Layout
- Overlay Layout
- freie Positionierung

Die Navigation verwendet ausschließlich logische Koordinaten.

---

# 13. Randverhalten

Beim Erreichen des Layoutendes sind folgende Modi möglich:

- Navigation endet
- Wrap Around
- Benutzerdefinierte Weiterleitung

Das Verhalten wird pro Focus Scope konfiguriert.

---

# 14. Fokuswechsel

Ein erfolgreicher Richtungswechsel erzeugt:

```text
Focus Lost

↓

Focus Changed

↓

Focus Gained
```

Der Wechsel erfolgt atomar.

---

# 15. Controller

Controller unterstützen:

- D-Pad
- Analogstick
- digitale Navigation

Analogbewegungen werden ab einem konfigurierbaren Schwellwert in Richtungsereignisse umgewandelt.

---

# 16. Accessibility

Unterstützt werden:

- vereinfachte Navigation
- alternative Fokusreihenfolgen
- Screen Reader
- Focus Announcement

Accessibility nutzt dieselbe Navigation Engine.

---

# 17. Sicherheit

Das System verhindert:

- rekursive Navigation
- Fokus außerhalb des Focus Scope
- Navigation auf deaktivierte Controls
- ungültige Nachbarn
- Endlosschleifen im Navigationsgraphen

Alle Navigationsziele werden validiert.

---

# 18. Speicherverwaltung

Das System verwendet:

- statische Navigationsgraphen
- Objektpools
- feste Focus-Tabellen

Heap-Allokationen während der Laufzeit sind verboten.

---

# 19. Performance

Anforderungen:

- O(1)-Navigation bei expliziten Nachbarn
- O(log n) oder besser bei geometrischer Kandidatensuche
- deterministische Laufzeit
- konstante Eingabelatenz

Vorberechnete Navigationsbeziehungen sollen bevorzugt verwendet werden.

---

# 20. API

```c
typedef enum
{
    NOVA_DIRECTION_UP,
    NOVA_DIRECTION_DOWN,
    NOVA_DIRECTION_LEFT,
    NOVA_DIRECTION_RIGHT,
    NOVA_DIRECTION_FORWARD,
    NOVA_DIRECTION_BACKWARD
}
nova_navigation_direction_t;

nova_result_t
nova_focus_navigate(
    nova_navigation_direction_t direction);

nova_result_t
nova_focus_set_neighbor(
    nova_control_t* source,
    nova_navigation_direction_t direction,
    nova_control_t* target);

nova_control_t*
nova_focus_find_neighbor(
    nova_control_t* source,
    nova_navigation_direction_t direction);
```

---

# 21. Diagnosedaten

Zu erfassen sind:

- aktuelles Focus-Control
- Navigationsrichtung
- Ziel-Control
- Navigationsdauer
- fehlgeschlagene Navigationen
- Focus Scope
- Fehler

---

# 22. Testfälle

Zu testen sind:

- Pfeiltasten
- D-Pad
- Analogstick
- Grid Layout
- Stack Layout
- explizite Nachbarn
- Wrap Around
- Accessibility
- Focus Scope
- Fehlerfälle

---

# 23. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- richtungsbasierte Navigation deterministisch arbeitet.
- explizite Nachbarschaften Vorrang besitzen.
- geometrische Zielberechnung zuverlässig funktioniert.
- Navigation niemals den aktiven Focus Scope verlässt.
- Controller- und Tastatureingaben identisch behandelt werden.
- Accessibility vollständig integriert ist.
- keine dynamischen Speicherallokationen während der Laufzeit stattfinden.
- BIOS- und UEFI-Systeme identisches Verhalten erzeugen.

---

# 24. Referenzen

## ADRs

- ADR-BOOTINPUT-0009 – Directional Focus Navigation
- ADR-BOOTNAV-0001 – Navigation Model
- ADR-BOOTLAYOUT-0001 – Layout Engine
- ADR-BOOTACCESS-0001 – Accessibility

## NPSPECs

- NPSPEC-BOOTINPUT-0008 – Focus Management
- NPSPEC-BOOTNAV-0001 – Boot Navigation Model
- NPSPEC-BOOTLAYOUT-0001 – Boot Layout Engine
- NPSPEC-BOOTCONTROL-0001 – Boot Control Framework
- NPSPEC-BOOTACCESSIBILITY-0001 – Accessibility Framework

---

# 25. Zusammenfassung

Die **Directional Focus Navigation** definiert das standardisierte System zur richtungsbasierten Navigation zwischen fokussierbaren Controls innerhalb der NovaOS-Bootoberfläche. Sie kombiniert explizite Navigationsbeziehungen mit einer geometrischen Zielermittlung, unterstützt Tastatur, Controller und Accessibility gleichermaßen und arbeitet vollständig unabhängig vom zugrunde liegenden Layouttyp. Durch deterministische Fokuswechsel, klare Prioritätsregeln und statische Datenstrukturen entsteht ein konsistentes und performantes Navigationssystem für BIOS- und UEFI-basierte Bootumgebungen.