# NPSPEC-BOOTNAV-0003
# Page and View Model

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-BOOTNAV-0003 |
| Titel | Page and View Model |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Bootmanager / Navigation |
| Priorität | Kritisch |
| Abhängigkeiten | NPSPEC-BOOTNAV-0001, NPSPEC-BOOTNAV-0002, NPSPEC-BOOTLAYOUT-0001 bis NPSPEC-BOOTLAYOUT-0010 |
| Zugehörige ADRs | ADR-BOOTNAV-0003, ADR-BOOTSTATE-0001, ADR-BOOTUI-0002, ADR-BOOTCONTROL-0001, ADR-BOOTPERF-0002 |

---

# 1. Ziel

Diese Spezifikation definiert das **Page and View Model** der NovaOS Boot UI.

Das Modell beschreibt den strukturellen Aufbau sämtlicher Ansichten der Bootoberfläche.

Es definiert:

- Pages
- Views
- View-Hierarchien
- Lebenszyklen
- Zustände
- Sichtbarkeit
- Aktivierung
- Navigationseinbindung

Jede sichtbare Oberfläche des Bootmanagers basiert auf diesem Modell.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für sämtliche grafischen Bootoberflächen.

Sie umfasst:

- Hauptmenü
- Einstellungen
- Recovery
- Diagnose
- Passwortdialoge
- Backup
- Wiederherstellung
- Fehlerseiten

Nicht Bestandteil sind:

- Rendering
- Animationen
- Eingabesystem

---

# 3. Architektur

```text
Navigation Controller

↓

Page

↓

View Tree

↓

Controls

↓

Layout Engine

↓

Rendering
```

---

# 4. Grundprinzip

Das Modell unterscheidet zwischen:

- **Page**
- **View**

Eine **Page** stellt eine vollständige navigierbare Oberfläche dar.

Eine **View** ist ein Bestandteil einer Page.

Eine Page besitzt mindestens eine Root View.

---

# 5. Aufgaben

Das Modell übernimmt:

- Strukturierung
- Lebenszyklus
- Aktivierung
- Sichtbarkeit
- Zustandsverwaltung
- View-Hierarchie
- Ressourcenverwaltung
- Ereignisweiterleitung

---

# 6. Page

Eine Page besitzt:

- eindeutige ID
- Titel
- Root View
- Layout
- Navigation Context

Beispiele:

- Boot Menu
- Recovery
- Diagnostics
- Settings

---

# 7. View

Eine View beschreibt einen logischen UI-Bereich.

Beispiele:

- Header
- Navigation
- Content
- Footer
- Sidebar
- Dialog Content

Views dürfen weitere Unterviews enthalten.

---

# 8. View Tree

Alle Views bilden einen Baum.

```text
Page

├── Header

├── Navigation

├── Content

│   ├── List

│   ├── Buttons

│   └── Status

└── Footer
```

Jede View besitzt genau einen Elternknoten.

---

# 9. Root View

Jede Page besitzt genau eine Root View.

Sie bildet den Einstiegspunkt für:

- Layout
- Rendering
- Fokus
- Ereignisse

---

# 10. View Lifecycle

Jede View besitzt folgende Zustände:

```text
Created

↓

Initialized

↓

Visible

↓

Active

↓

Inactive

↓

Destroyed
```

Übersprungene Zustände sind unzulässig.

---

# 11. Page Lifecycle

Eine Page besitzt:

```text
Created

↓

Loaded

↓

Active

↓

Suspended

↓

Closed
```

Beim Wechsel zwischen Pages erfolgt ein vollständiger Zustandswechsel.

---

# 12. Sichtbarkeit

Jede View besitzt einen Sichtbarkeitsstatus:

- Visible
- Hidden
- Collapsed

Hidden bleibt Bestandteil des Layouts.

Collapsed wird vollständig aus Layout und Rendering entfernt.

---

# 13. Aktivierung

Nur eine Page kann gleichzeitig aktiv sein.

Innerhalb einer aktiven Page dürfen mehrere Views gleichzeitig aktiv sein.

---

# 14. Navigation Context

Jede Page besitzt einen eigenen Navigationskontext.

Dieser umfasst:

- History
- Fokus
- Dialoge
- Parameter

Beim erneuten Öffnen wird der Kontext wiederhergestellt.

---

# 15. Ressourcen

Jede Page verwaltet:

- Layoutdaten
- Styles
- Views
- Ressourcen
- Animationen

Beim Schließen werden nicht mehr benötigte Ressourcen freigegeben.

---

# 16. Fokus

Der Fokus gehört immer einer View.

Beim Seitenwechsel wird:

- der letzte Fokus gespeichert
- beim Zurückkehren automatisch wiederhergestellt

---

# 17. Dialoge

Dialoge sind eigenständige Pages mit modalem Verhalten.

Sie besitzen:

- eigene Root View
- eigenen Layoutbaum
- eigenen Fokuskontext

---

# 18. Ereignisse

Ereignisse durchlaufen:

```text
Input

↓

Page

↓

Root View

↓

Child Views

↓

Control
```

Nicht behandelte Ereignisse werden entlang der Hierarchie zurückgegeben.

---

# 19. Layoutintegration

Jede View besitzt:

- Layout Box
- Render Node
- Focus Node
- Hit-Test Node

Die Layout Engine verarbeitet ausschließlich Views.

---

# 20. Accessibility

Jede View besitzt Accessibility-Informationen:

- Name
- Rolle
- Beschreibung
- Fokusreihenfolge

Diese Informationen bleiben unabhängig vom Layout erhalten.

---

# 21. Speicherverwaltung

Pages und Views werden aus festen Objektpools erzeugt.

Heap-Allokationen während der Laufzeit sind verboten.

Beim Schließen einer Page werden sämtliche zugehörigen Views kontrolliert freigegeben.

---

# 22. Performance

Anforderungen:

- O(n)-Traversal
- deterministische Aktivierung
- keine rekursiven Endlosschleifen
- keine Heap-Allokationen

Nur betroffene Views werden aktualisiert.

---

# 23. Fehlerbehandlung

Folgende Fehler müssen erkannt werden:

- fehlende Root View
- zyklischer View Tree
- doppelte View-ID
- ungültiger Lifecycle
- Nullpointer

Im Fehlerfall wird die betroffene Page nicht aktiviert.

---

# 24. Sicherheit

Das System verhindert:

- mehrere aktive Root Pages
- rekursive View-Hierarchien
- doppelte View-IDs
- ungültige Lebenszyklen
- inkonsistente Fokuszustände

---

# 25. API

```c
typedef struct nova_page nova_page_t;
typedef struct nova_view nova_view_t;

nova_result_t
nova_page_create(
    nova_page_t** page);

nova_result_t
nova_page_activate(
    nova_page_t* page);

nova_result_t
nova_page_destroy(
    nova_page_t* page);

nova_result_t
nova_view_add_child(
    nova_view_t* parent,
    nova_view_t* child);

nova_view_t*
nova_page_root_view(
    const nova_page_t* page);
```

---

# 26. Diagnosedaten

Zu erfassen sind:

- aktive Page
- Anzahl Views
- View-Hierarchie
- Lifecycle-Zustände
- Fokuswechsel
- Layoutinvalidierungen
- Speicherverbrauch
- Fehleranzahl

---

# 27. Testfälle

Zu testen sind:

- Page erstellen
- Page aktivieren
- Page schließen
- View Tree
- Root View
- Sichtbarkeitswechsel
- Fokuswiederherstellung
- Dialog Pages
- Accessibility
- Recovery Navigation

---

# 28. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- jede Oberfläche aus genau einer Page mit einer Root View besteht.
- View-Hierarchien korrekt aufgebaut werden.
- sämtliche Lifecycle-Zustände deterministisch durchlaufen werden.
- Navigation und Fokus korrekt integriert sind.
- Dialoge als eigenständige Pages funktionieren.
- keine Heap-Allokationen während der Laufzeit stattfinden.
- BIOS- und UEFI-Systeme identisches Verhalten erzeugen.

---

# 29. Referenzen

## ADRs

- ADR-BOOTNAV-0003 – Page- und View-Modell
- ADR-BOOTSTATE-0001 – Globales Zustandsmodell
- ADR-BOOTUI-0002 – Deklaratives UI-Modell
- ADR-BOOTCONTROL-0001 – Einheitliches Control-Modell
- ADR-BOOTPERF-0002 – Deterministische Laufzeiten

## NPSPECs

- NPSPEC-BOOTNAV-0001 – Boot Navigation Model
- NPSPEC-BOOTNAV-0002 – Navigation Stack
- NPSPEC-BOOTLAYOUT-0001 – Boot Layout Engine
- NPSPEC-BOOTCONTROL-0001 – Base Control Model
- NPSPEC-BOOTDIALOG-0001 – Dialog System

---

# 30. Zusammenfassung

Das **Page and View Model** definiert die strukturelle Grundlage sämtlicher Benutzeroberflächen der NovaOS Boot UI. Jede navigierbare Oberfläche besteht aus genau einer Page mit einer Root View, die wiederum einen hierarchischen View Tree enthält. Durch klar definierte Lebenszyklen, deterministische Aktivierung, feste Navigationskontexte und statische Speicherverwaltung entsteht ein robustes, konsistentes und leicht erweiterbares UI-Modell für BIOS- und UEFI-basierte Bootumgebungen.