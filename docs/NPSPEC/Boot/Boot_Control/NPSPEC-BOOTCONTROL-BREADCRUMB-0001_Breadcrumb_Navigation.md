# NPSPEC-BOOTCONTROL-BREADCRUMB-0001
# Breadcrumb Navigation

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTCONTROL-BREADCRUMB-0001 |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot UI Controls |
| Priorität | Mittel |
| Gültigkeitsbereich | NovaOS Boot Manager |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTCONTROL-0001, NPSPEC-BOOTNAV-0001, NPSPEC-BOOTNAV-0002, NPSPEC-BOOTNAV-0003, NPSPEC-BOOTCONTROL-BUTTON-0001, NPSPEC-BOOTACCESSIBILITY-0001 |

---

# 1. Ziel

Diese Spezifikation definiert die **Breadcrumb Navigation** der NovaOS Boot UI.

Eine Breadcrumb Navigation zeigt dem Benutzer jederzeit seine aktuelle Position innerhalb der hierarchischen Navigationsstruktur des Boot Managers. Sie ermöglicht eine schnelle Navigation zu übergeordneten Ebenen und verbessert die Orientierung innerhalb komplexer Menüs.

Typische Einsatzbereiche:

- Einstellungen
- Recovery
- Diagnose
- Boot-Konfiguration
- Dateibrowser
- Erweiterte Optionen
- Wartungswerkzeuge

---

# 2. Geltungsbereich

Diese Spezifikation gilt für sämtliche Breadcrumb-Komponenten.

Sie umfasst:

- Hierarchische Navigation
- Klickbare Navigationselemente
- Dynamische Aktualisierung
- Icons innerhalb der Navigation

Nicht Bestandteil sind:

- Tabs
- Menüs
- Listen
- Tree Views

---

# 3. Architektur

```text
Navigation Model

↓

Breadcrumb Control

├── Breadcrumb Items

├── Separator

└── Renderer
```

---

# 4. Grundprinzip

Ein Breadcrumb stellt den vollständigen Navigationspfad von der Wurzel bis zur aktuellen Ansicht dar.

Jeder Eintrag repräsentiert genau eine Navigationsebene.

Der letzte Eintrag beschreibt immer die aktuell aktive Ansicht.

---

# 5. Aufgaben

Die Breadcrumb Navigation übernimmt:

- Darstellung des Navigationspfades
- Navigation zu übergeordneten Ebenen
- Synchronisierung mit dem Navigation Stack
- Themeintegration
- Accessibility

---

# 6. Aufbau

```text
Breadcrumb

├── Root Item

├── Separator

├── Parent Items

├── Separator

└── Current Item
```

Beispiel:

```text
Start

>

Recovery

>

Dateisystem

>

Snapshots
```

---

# 7. Eigenschaften

Jede Breadcrumb Navigation besitzt:

- ID
- Navigationspfad
- Sichtbarkeit
- Style
- Template

Jeder Eintrag besitzt:

- Titel
- Zielseite
- Icon (optional)
- Aktivstatus

---

# 8. Navigation

Unterstützt werden:

- Klick auf Elternknoten
- Tastatur
- Controller
- Touch

Der aktuelle Eintrag ist standardmäßig nicht anklickbar.

---

# 9. Zustände

```text
Normal

Focused

Hovered

Pressed

Disabled
```

Der aktuelle Eintrag besitzt zusätzlich:

```text
Current
```

---

# 10. Separator

Standardseparator:

```text
>
```

Alternativ:

```text
/

→

Chevron Icon
```

Der Separator wird vollständig durch das Theme definiert.

---

# 11. Aktualisierung

Der Breadcrumb wird automatisch aktualisiert bei:

- Seitenwechsel
- Navigation zurück
- Navigation vorwärts
- Navigation per Direktaufruf

Die Reihenfolge entspricht immer dem Navigation Stack.

---

# 12. Darstellung

Die Breadcrumb Navigation verwendet:

- Theme-Schriftarten
- Themefarben
- optionale Icons
- Focus Glow
- Glass Material

Alle visuellen Eigenschaften stammen aus Theme und Template.

---

# 13. Animationen

Optional unterstützt:

- Fade
- Seitenwechsel
- Focus Transition
- Separator Animation

Animationen dürfen die Navigation nicht verzögern.

---

# 14. Accessibility

Jeder Breadcrumb besitzt:

- Name
- Rolle
- Beschreibung

Jeder Eintrag besitzt:

- Position
- Titel
- Status

Screenreader geben beispielsweise aus:

```text
Navigation:

Recovery >

Snapshots

Aktuelle Seite
```

---

# 15. Themeintegration

Das Theme definiert:

- Schriftarten
- Farben
- Separator
- Transparenz
- Abstände
- Icons

Keine Designparameter sind fest codiert.

---

# 16. Speicherverwaltung

Breadcrumbs verwenden:

- gemeinsame Templates
- gemeinsame Styles
- Objektpools

Einträge werden dynamisch anhand des Navigation Stacks referenziert.

Heap-Allokationen während der Laufzeit sind verboten.

---

# 17. Performance

Anforderungen:

- O(1)-Aktualisierung pro Navigation
- O(1)-Auswahl
- O(1)-Rendering
- deterministische Laufzeit

Es werden ausschließlich geänderte Einträge neu gerendert.

---

# 18. Fehlerbehandlung

Folgende Fehler müssen erkannt werden:

- ungültiger Navigationspfad
- fehlende Zielseite
- beschädigtes Template
- fehlender Separator
- Nullpointer

Kann der Pfad nicht aufgebaut werden, wird ausschließlich die aktuelle Seite angezeigt.

---

# 19. Sicherheit

Das System verhindert:

- rekursive Navigationspfade
- ungültige Seitenreferenzen
- doppelte Navigationseinträge
- Navigation außerhalb des gültigen Bereichs

Die Breadcrumb Navigation darf niemals den Navigation Stack verändern.

---

# 20. API

```c
typedef struct nova_breadcrumb nova_breadcrumb_t;

nova_result_t
nova_breadcrumb_create(
    nova_breadcrumb_t** breadcrumb);

nova_result_t
nova_breadcrumb_attach_navigation(
    nova_breadcrumb_t* breadcrumb,
    nova_navigation_stack_t* stack);

nova_result_t
nova_breadcrumb_refresh(
    nova_breadcrumb_t* breadcrumb);

uint32_t
nova_breadcrumb_count(
    const nova_breadcrumb_t* breadcrumb);
```

---

# 21. Diagnosedaten

Zu erfassen sind:

- Breadcrumb-ID
- Anzahl der Ebenen
- aktueller Pfad
- Aktualisierungen
- Renderzeit
- Speicherverbrauch
- Fehleranzahl

---

# 22. Testfälle

Zu testen sind:

- Navigation
- Rücksprung
- Direktnavigation
- Dynamische Aktualisierung
- Themewechsel
- Accessibility
- Touch
- Controller
- Fehlerfälle

---

# 23. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- der vollständige Navigationspfad korrekt dargestellt wird.
- Navigation zu übergeordneten Ebenen möglich ist.
- der aktuelle Eintrag eindeutig hervorgehoben wird.
- Theme und Templates vollständig verwendet werden.
- Accessibility vollständig integriert ist.
- ausschließlich Navigationseinträge dargestellt werden.
- keine Heap-Allokationen während der Laufzeit stattfinden.
- BIOS- und UEFI-Systeme identisches Verhalten erzeugen.

---

# 24. Referenzen

## ADRs

- ADR-BOOTCONTROL-BREADCRUMB-0001 – Breadcrumb Navigation
- ADR-BOOTNAV-0001 – Navigation Model
- ADR-BOOTNAV-0002 – Navigation Stack
- ADR-BOOTTHEME-0001 – Theme-System
- ADR-BOOTACCESS-0001 – Accessibility

## NPSPECs

- NPSPEC-BOOTCONTROL-0001 – Boot Control Framework
- NPSPEC-BOOTNAV-0001 – Boot Navigation Model
- NPSPEC-BOOTNAV-0002 – Navigation Stack
- NPSPEC-BOOTNAV-0003 – Page and View Model
- NPSPEC-BOOTCONTROL-BUTTON-0001 – Button
- NPSPEC-BOOTACCESSIBILITY-0001 – Accessibility Framework

---

# 25. Zusammenfassung

Die **Breadcrumb Navigation** definiert das standardisierte Navigationselement zur Darstellung des aktuellen Pfades innerhalb der NovaOS-Bootoberfläche. Sie synchronisiert sich automatisch mit dem Navigation Stack, ermöglicht die direkte Navigation zu übergeordneten Ebenen und unterstützt vollständige Theme-Integration, Animationen sowie Accessibility. Durch deterministische Aktualisierung, zentrale Verwaltung der Navigationsdaten und statische Speicherverwaltung entsteht ein konsistentes und benutzerfreundliches Orientierungssystem für BIOS- und UEFI-basierte Bootumgebungen.