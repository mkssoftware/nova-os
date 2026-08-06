# NPSPEC-BOOTCONTROL-LIST-0001
# List

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTCONTROL-LIST-0001 |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot UI Controls |
| Priorität | Kritisch |
| Gültigkeitsbereich | NovaOS Boot Manager |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTCONTROL-0001, NPSPEC-BOOTCONTROL-0002, NPSPEC-BOOTLAYOUT-0003, NPSPEC-BOOTINPUT-0001, NPSPEC-BOOTNAV-0003 |

---

# 1. Ziel

Diese Spezifikation definiert das **List-Control** der NovaOS Boot UI.

Das List-Control dient zur Darstellung und Auswahl geordneter Datensätze innerhalb der Bootoberfläche.

Es bildet die Grundlage für:

- Bootmenüs
- Laufwerkslisten
- Recovery-Einträge
- Backup-Listen
- Protokolle
- Einstellungslisten
- Diagnoselisten

---

# 2. Geltungsbereich

Diese Spezifikation gilt für sämtliche Listen.

Sie umfasst:

- einfache Listen
- Listen mit Symbolen
- gruppierte Listen
- auswählbare Listen
- virtuelle Listen
- Listen mit Detailinformationen

Nicht Bestandteil sind:

- Tabellen
- Bäume
- Menüs

---

# 3. Architektur

```text
Boot Control Framework

↓

List Control

↓

Item Collection

↓

Layout Engine

↓

Renderer
```

---

# 4. Grundprinzip

Eine Liste besteht aus einer geordneten Folge von Listeneinträgen.

Jeder Eintrag besitzt:

- eindeutige ID
- Inhalt
- Zustand
- Position

Die Darstellung erfolgt unabhängig von der zugrunde liegenden Datenquelle.

---

# 5. Aufgaben

Das List-Control übernimmt:

- Datendarstellung
- Auswahl
- Fokusverwaltung
- Navigation
- Scrollintegration
- Virtualisierung
- Accessibility
- Ereignisverarbeitung

---

# 6. Aufbau

```text
List

├── Header (optional)

├── Items

├── Scrollbar (optional)

└── Footer (optional)
```

---

# 7. Listeneintrag

Ein Eintrag kann enthalten:

- Symbol
- Titel
- Beschreibung
- Status
- Zusatzinformationen
- Aktion

Die konkrete Darstellung erfolgt über Item-Templates.

---

# 8. Auswahl

Unterstützt werden:

```text
Keine Auswahl

Einzelauswahl

Mehrfachauswahl
```

Standardmäßig wird Einzelauswahl verwendet.

---

# 9. Zustände

Die Liste besitzt:

```text
Normal

Focused

Disabled

Busy
```

Jeder Listeneintrag besitzt zusätzlich:

```text
Hovered

Selected

Pressed

Hidden
```

---

# 10. Navigation

Unterstützt werden:

- ↑
- ↓
- Page Up
- Page Down
- Home
- End

Navigation verändert ausschließlich die aktuelle Auswahl.

---

# 11. Fokus

Zu jedem Zeitpunkt besitzt genau ein sichtbarer Eintrag den Fokus.

Beim Öffnen erhält:

- der ausgewählte Eintrag

oder

- der erste sichtbare Eintrag

den Fokus.

---

# 12. Scrollen

Die Liste unterstützt:

- vertikales Scrollen
- Controller-Scrollen
- Mausrad
- Touch-Scrollen

Horizontales Scrollen ist optional.

---

# 13. Virtualisierung

Große Listen dürfen virtualisiert werden.

Dabei werden ausschließlich sichtbare Einträge erzeugt und gerendert.

Die Reihenfolge der Daten bleibt unverändert.

---

# 14. Layout

Alle Einträge besitzen:

- identische Breite
- definierte Mindesthöhe
- automatischen Innenabstand

Variable Höhen sind optional zulässig.

---

# 15. Darstellung

Einträge verwenden:

- Glass Material
- Hover-Hintergrund
- Focus Glow
- Selection Highlight
- Statusindikatoren

Die Darstellung erfolgt ausschließlich über Styles und Templates.

---

# 16. Animationen

Unterstützt werden:

- Hover Fade
- Focus Transition
- Selection Animation
- Scroll Animation
- Insert Animation
- Remove Animation

Animationen verändern keine Daten.

---

# 17. Accessibility

Jeder Eintrag besitzt:

- Name
- Rolle
- Position
- Auswahlstatus
- Beschreibung

Screenreader geben zusätzlich die Anzahl der Einträge aus.

---

# 18. Themeintegration

Das Theme definiert:

- Hintergrund
- Auswahlfarbe
- Fokusfarbe
- Symbolfarben
- Schriftarten
- Schatten
- Transparenz

Alle Darstellungsparameter stammen aus dem Theme.

---

# 19. Speicherverwaltung

Listen verwenden:

- statische Item-Pools
- gemeinsame Templates
- zentrale Styles
- optionale Virtualisierung

Heap-Allokationen während der Laufzeit sind verboten.

---

# 20. Performance

Anforderungen:

- O(1)-Auswahlwechsel
- O(1)-Fokuswechsel
- O(1)-Scrollen
- O(n)-Initialisierung
- deterministische Laufzeit

Virtualisierung ist für große Listen vorgesehen.

---

# 21. Fehlerbehandlung

Folgende Fehler müssen erkannt werden:

- ungültiger Index
- doppelte IDs
- beschädigte Datenquelle
- fehlendes Template
- Nullpointer

Ungültige Einträge werden ignoriert.

---

# 22. Sicherheit

Das System verhindert:

- ungültige Auswahl
- Fokus außerhalb sichtbarer Einträge
- beschädigte Reihenfolge
- rekursive Einträge
- inkonsistente Itemzustände

Die Reihenfolge der Daten bleibt jederzeit stabil.

---

# 23. API

```c
typedef struct nova_list nova_list_t;
typedef struct nova_list_item nova_list_item_t;

nova_result_t
nova_list_create(
    nova_list_t** list);

nova_result_t
nova_list_add_item(
    nova_list_t* list,
    nova_list_item_t* item);

nova_result_t
nova_list_remove_item(
    nova_list_t* list,
    uint32_t index);

nova_result_t
nova_list_select(
    nova_list_t* list,
    uint32_t index);

uint32_t
nova_list_selected_index(
    const nova_list_t* list);
```

---

# 24. Diagnosedaten

Zu erfassen sind:

- Listen-ID
- Eintragsanzahl
- Auswahlwechsel
- Fokuswechsel
- Scrollposition
- Virtualisierung
- Speicherverbrauch
- Fehleranzahl

---

# 25. Testfälle

Zu testen sind:

- Einfügen
- Entfernen
- Einzelauswahl
- Mehrfachauswahl
- Scrollen
- Virtualisierung
- Themewechsel
- Accessibility
- Controller
- Fehlerfälle

---

# 26. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- Listen beliebig viele Einträge verwalten können.
- Auswahl und Fokus deterministisch funktionieren.
- Virtualisierung für große Datenmengen unterstützt wird.
- Accessibility vollständig integriert ist.
- Themewechsel automatisch übernommen werden.
- keine Heap-Allokationen während der Laufzeit stattfinden.
- BIOS- und UEFI-Systeme identisches Verhalten erzeugen.

---

# 27. Referenzen

## ADRs

- ADR-BOOTCONTROL-LIST-0001 – Listensteuerung
- ADR-BOOTLAYOUT-0003 – Stack Layout
- ADR-BOOTINPUT-0001 – Eingabearchitektur
- ADR-BOOTACCESS-0001 – Accessibility
- ADR-BOOTPERF-0002 – Deterministische UI-Verarbeitung

## NPSPECs

- NPSPEC-BOOTCONTROL-0001 – Boot Control Framework
- NPSPEC-BOOTCONTROL-0002 – Control State Model
- NPSPEC-BOOTLAYOUT-0003 – Stack Layout
- NPSPEC-BOOTNAV-0003 – Page and View Model
- NPSPEC-BOOTINPUT-0001 – Input System
- NPSPEC-BOOTACCESSIBILITY-0001 – Accessibility Framework

---

# 28. Zusammenfassung

Das **List-Control** definiert die standardisierte Darstellung und Verwaltung geordneter Datensätze innerhalb der NovaOS-Bootoberfläche. Es unterstützt Auswahlmechanismen, Fokusverwaltung, Virtualisierung, Scrollen sowie vollständige Theme- und Accessibility-Integration. Durch deterministische Navigation, stabile Datenreihenfolgen und statische Speicherverwaltung bildet es die Grundlage für sämtliche Listenansichten im NovaOS Boot Manager auf BIOS- und UEFI-Systemen.