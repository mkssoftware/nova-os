# NPSPEC-CONTROL-LIST-0001: Nova List Control

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** UI Controls
- **Erstellt:** 2026-07-23
- **Gültig für:** Nova UI Framework, Nova SDK, Nova Desktop Shell und alle Nova-Anwendungen
- **Ersetzt:** Keine

---

# 1. Einleitung

Diese Spezifikation definiert den **Nova List Control (NLC)**.

Listen gehören zu den am häufigsten verwendeten UI-Komponenten in Nova OS und dienen zur Darstellung strukturierter Sammlungen gleichartiger Objekte.

Der Nova List Control unterstützt sowohl einfache Textlisten als auch komplexe Einträge mit Bildern, Symbolen, Statusinformationen und Aktionen.

---

# 2. Zielsetzung

Der Nova List Control verfolgt folgende Ziele:

- schnelle Übersicht großer Datenmengen
- konsistente Darstellung
- hohe Performance
- GPU-beschleunigtes Rendering
- vollständige Tastaturbedienung
- Touch-Unterstützung
- Virtualisierung
- Barrierefreiheit

---

# 3. Designphilosophie

Listen sollen möglichst leicht lesbar sein.

Grundprinzipien:

- klare Struktur
- ruhige Gestaltung
- konsistente Abstände
- eindeutige Auswahl
- hohe Informationsdichte
- einfache Navigation

Eine Liste dient der Darstellung gleichartiger Elemente. Für hierarchische Daten soll stattdessen ein Tree-Control und für tabellarische Daten ein DataGrid verwendet werden. :contentReference[oaicite:0]{index=0}

---

# 4. Architektur

```text
Nova List

├── List Container
├── Virtualizer
├── Item Template
├── Selection Manager
├── Focus Manager
├── Scroll Manager
├── Drag & Drop
├── Context Menu
├── Search Filter
└── Accessibility Layer
```

---

# 5. Listenarten

Nova definiert folgende Standardlisten:

- Simple List
- Detailed List
- Icon List
- Thumbnail List
- Check List
- Multi Selection List
- Action List
- Settings List
- Navigation List

Neue Listenarten benötigen eine eigene NPSPEC.

---

# 6. Aufbau

```text
┌────────────────────────────────────────────┐
│ Icon  Titel                Status          │
│       Beschreibung                          │
└────────────────────────────────────────────┘
```

Ein Eintrag kann enthalten:

- Icon
- Bild
- Titel
- Beschreibung
- Status
- Badge
- Aktion
- Checkbox

---

# 7. Eintragshöhen

| Typ | Höhe |
|------|------:|
| Compact | 36 px |
| Standard | 48 px |
| Comfortable | 60 px |
| Detailed | 72 px |
| Thumbnail | 96 px |

Alle Höhen basieren auf dem Nova-8-Pixel-Raster.

---

# 8. Eintragsbestandteile

Jeder Eintrag besitzt mindestens:

- Primärtext

Optional:

- Sekundärtext
- Icon
- Thumbnail
- Badge
- Statussymbol
- Zeitstempel
- Fortschritt
- Aktionen

---

# 9. Auswahl

Unterstützte Auswahlarten:

```text
Keine Auswahl

↓

Einzelauswahl

↓

Mehrfachauswahl

↓

Checkbox-Auswahl
```

Die Auswahlart wird vom Entwickler festgelegt.

---

# 10. Mehrfachauswahl

Mehrfachauswahl unterstützt:

- STRG + Klick
- SHIFT + Klick
- Tastatur
- Touch
- Checkboxen

Checkboxen sollen verwendet werden, wenn Mehrfachauswahl eine häufige oder zentrale Benutzeraktion ist. :contentReference[oaicite:1]{index=1}

---

# 11. Zustände

Jeder Eintrag besitzt:

```text
Normal

↓

Hover

↓

Selected

↓

Focused

↓

Disabled

↓

Dragging
```

---

# 12. Hover

Beim Hover:

- leichte Hintergrundaufhellung
- Glow
- Mauszeiger = Hand (bei anklickbaren Einträgen)

---

# 13. Auswahl

Ausgewählte Elemente erhalten:

- Nova Accent Hintergrund
- Focus Ring
- Akzentfarbe
- invertierten Text bei Bedarf

Die Auswahl bleibt sichtbar, auch wenn die Liste den Fokus verliert. :contentReference[oaicite:2]{index=2}

---

# 14. Fokus

Der Tastaturfokus besitzt:

- Nova Focus Ring
- 2 px Breite
- vollständig sichtbaren Rahmen

---

# 15. Gruppierung

Listen unterstützen Gruppen.

Beispiel:

```text
Heute

Projekt.md

Rechnung.pdf

--------------------------------

Gestern

Urlaub.jpg

Notizen.txt
```

Gruppen können:

- eingeklappt
- ausgeklappt
- automatisch sortiert

werden.

---

# 16. Sortierung

Standard:

- alphabetisch
- numerisch
- Datum
- Größe
- Typ
- Benutzerdefiniert

Benutzer dürfen die Sortierung ändern.

---

# 17. Filter

Listen unterstützen:

- Live-Suche
- Kategorien
- Tags
- Datum
- Benutzerdefinierte Filter

Filter wirken sofort.

---

# 18. Scrollen

Unterstützt:

- Mausrad
- Touch
- Touchpad
- Tastatur
- Controller

Die Scrollanimation folgt dem Nova Motion System.

---

# 19. Virtualisierung

Große Listen verwenden:

- Item Virtualization
- Lazy Loading
- Recycling
- GPU Rendering

Auch Listen mit mehreren hunderttausend Einträgen sollen flüssig scrollen.

---

# 20. Drag & Drop

Unterstützt:

- Umsortieren
- Kopieren
- Verschieben
- Externe Dateien
- Interne Objekte

---

# 21. Kontextmenü

Standardaktionen:

- Öffnen
- Bearbeiten
- Umbenennen
- Kopieren
- Ausschneiden
- Löschen
- Eigenschaften

Anwendungen können zusätzliche Einträge registrieren.

---

# 22. Animationen

Listen verwenden:

- Fade
- Slide
- Ripple
- Glow

Keine hektischen Animationen.

---

# 23. Icons

Verwendet werden ausschließlich:

- Fluent SVG Icons
- Themefähig
- Vektorformat

Standardgröße:

```text
20 × 20 px
```

---

# 24. Badges

Einträge können besitzen:

- Anzahl
- Warnung
- Fehler
- Synchronisation
- Neu
- Ungelesen

---

# 25. Touchmodus

Touchmodus verwendet:

- größere Abstände
- größere Trefferflächen
- größere Icons
- weichere Animationen

Mindestgröße:

```text
44 × 44 px
```

---

# 26. Accessibility

Unterstützt:

- Screenreader
- Tastatursteuerung
- Sprachsteuerung
- hohe Kontraste
- große Schrift
- reduzierte Animationen

Listen verwenden geeignete ARIA-Rollen wie `list`, `listbox` oder `option`, abhängig davon, ob eine Auswahl möglich ist. :contentReference[oaicite:3]{index=3}

---

# 27. Performance

Der List Control soll:

- GPU-beschleunigt rendern
- Virtualisierung verwenden
- unter 16 ms pro Frame bleiben
- flüssig bis 240 Hz arbeiten
- minimale Speicherbelegung besitzen

---

# 28. Entwickler-API

Das Nova SDK stellt bereit:

```text
List

↓

ListItem

↓

GroupedList

↓

CheckList

↓

VirtualList

↓

ThumbnailList

↓

NavigationList
```

---

# 29. Erweiterbarkeit

Neue Listentypen müssen:

- Design Tokens verwenden
- Motion Tokens verwenden
- Accessibility unterstützen
- dieselben Zustände verwenden

---

# 30. Positive Konsequenzen

- konsistente Darstellung
- hohe Performance
- gute Skalierbarkeit
- einfache Erweiterbarkeit
- hervorragende Lesbarkeit

---

# 31. Negative Konsequenzen

- komplexe Virtualisierung
- höherer Entwicklungsaufwand
- umfangreiche UI-Tests erforderlich

---

# 32. Auswirkungen auf andere Module

Diese Spezifikation betrifft:

- NPSPEC-DESIGN-0001 – Nova Design Language
- NPSPEC-MOTION-0001 – Nova Motion System
- NPSPEC-INTERACTION-0001 – Nova Interaction Model
- NPSPEC-WINDOW-0001 – Nova Window System
- Theme Manager
- Accessibility Manager
- Scroll Manager
- Virtualization Engine

Alle Listen innerhalb von Nova OS MÜSSEN dieser Spezifikation entsprechen.

---

# 33. Umsetzungsregeln

- Alle Listen MÜSSEN Virtualisierung unterstützen.
- Große Listen MÜSSEN Lazy Loading verwenden.
- Listen MÜSSEN vollständig per Tastatur bedienbar sein.
- Alle Animationen MÜSSEN dem Nova Motion System folgen.
- Fluent SVG Icons MÜSSEN verwendet werden.
- Alle Farben MÜSSEN aus den Nova Design Tokens stammen.
- Listen MÜSSEN GPU-beschleunigt gerendert werden.
- Accessibility MUSS vollständig unterstützt werden.

---

# 34. Referenzen

- NPSPEC-DESIGN-0001 – Nova Design Language
- NPSPEC-MOTION-0001 – Nova Motion System
- NPSPEC-INTERACTION-0001 – Nova Interaction Model
- NPSPEC-WINDOW-0001 – Nova Window System
- NPSPEC-SCROLLVIEW-0001 *(zukünftig)*
- Fluent 2 – List Usage Guide :contentReference[oaicite:4]{index=4}
- Microsoft Learn – Collections and Lists :contentReference[oaicite:5]{index=5}
- Microsoft Learn – List View Guidelines :contentReference[oaicite:6]{index=6}

---

# 35. Statushistorie

- **2026-07-23:** Version 1.0 erstellt.
- **2026-07-23:** Nova List Control als verbindlicher Standard für alle Listen in Nova OS definiert.
- **Zukünftige Änderungen:** Nur durch eine neue Version dieser Spezifikation oder eine ersetzende NPSPEC zulässig.