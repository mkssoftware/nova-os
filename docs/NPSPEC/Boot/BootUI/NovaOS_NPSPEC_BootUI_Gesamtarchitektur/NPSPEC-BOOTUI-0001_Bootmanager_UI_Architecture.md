# NPSPEC-BOOTUI-0001
# Bootmanager UI Architecture

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTUI-0001 |
| Titel | Bootmanager UI Architecture |
| Version | 1.0 |
| Status | Draft |
| Autor | NovaOS Architecture Team |
| Kategorie | Bootmanager |
| Abhängigkeiten | ADR-BOOTUI-0001 bis ADR-BOOTUI-0012 |
| Nachfolger | NPSPEC-BOOTRENDER-0001, NPSPEC-BOOTCOMPOSITOR-0001, NPSPEC-BOOTCONTROL-0001 |

---

# 1. Ziel

Diese Spezifikation definiert die vollständige Architektur der grafischen Benutzeroberfläche des NovaOS Bootmanagers.

Die Architektur muss:

- BIOS und UEFI unterstützen
- vollständig deterministisch arbeiten
- hochperformant sein
- hochwertiges Glass-Design unterstützen
- Animationen unterstützen
- später optisch nahezu identisch mit NovaOS sein
- vollständig modular aufgebaut sein
- ohne Kernel funktionieren
- ohne Windowmanager funktionieren
- ohne GPU-Treiber funktionieren
- vollständig testbar sein

Diese Spezifikation bildet die Grundlage sämtlicher Boot-UI-Komponenten.

---

# 2. Designziele

## 2.1 Hauptziele

Der Bootmanager soll nicht wie ein klassisches Bootmenü aussehen.

Er soll bereits den ersten Eindruck von NovaOS vermitteln.

Der Benutzer soll bereits während des Bootens erkennen:

- modernes Betriebssystem
- hochwertige Benutzeroberfläche
- flüssige Animationen
- klare Bedienung
- elegantes Glass Design
- konsistente Bedienung

---

## 2.2 Qualitätsziele

| Ziel | Priorität |
|--------|-----------|
| Stabilität | Kritisch |
| Geschwindigkeit | Kritisch |
| Bedienbarkeit | Kritisch |
| Lesbarkeit | Hoch |
| Animationen | Hoch |
| Design | Hoch |
| Speicherverbrauch | Hoch |
| Modularität | Hoch |

---

# 3. Architekturübersicht

```
                   Boot Manager

                         │
         ┌───────────────┴────────────────┐
         │                                │
 Platform Layer                  Resource Layer
         │                                │
         └───────────────┬────────────────┘
                         │
                  Graphics Backend
                         │
                  Rendering Engine
                         │
                  Scene Graph Engine
                         │
                Layout Engine
                         │
               Motion Engine
                         │
               Control Framework
                         │
                 Navigation System
                         │
                  Dialog Framework
                         │
                  Boot Applications
```

---

# 4. Hauptkomponenten

Die Architektur besteht aus folgenden Subsystemen.

## Platform Layer

Verantwortlich für:

- BIOS
- UEFI
- GOP
- VBE
- Timer
- Tastatur
- Maus
- Speicher

---

## Graphics Backend

Abstrahiert sämtliche Grafikfunktionen.

Stellt bereit:

- Framebuffer
- Pixel
- Blending
- Clipping
- Present

---

## Rendering Engine

Renderpipeline.

Aufgaben:

- Flächen
- Linien
- Kreise
- Texte
- Bilder
- SVG
- Blur
- Schatten

---

## Scene Graph

Verwaltet sämtliche Objekte.

Eigenschaften:

- Parent
- Child
- Z-Order
- Dirty Flag
- Visibility
- Alpha
- Animation

---

## Layout Engine

Berechnet:

- Größen
- Positionen
- Abstände
- DPI
- Skalierung

---

## Motion Engine

Animationen.

Verantwortlich für:

- Fade
- Slide
- Scale
- Blur
- Opacity
- Rotation

---

## Control Framework

Alle Bedienelemente.

Beispielsweise:

- Button
- List
- Dialog
- Checkbox
- TextField
- Progress
- Spinner

---

## Dialog Framework

Verantwortlich für:

- Overlay
- Modal
- Confirmation
- Progress
- Error

---

## Navigation

Verantwortlich für:

- Seiten
- History
- Back
- Forward
- Bootaktionen

---

## Boot Applications

Beispiele:

- Boot Auswahl
- Recovery
- Snapshot
- Passwort
- RAM Test
- Festplattenwerkzeuge
- Secure Boot

---

# 5. Layer Architektur

```
+------------------------------------+
| Dialog Layer                       |
+------------------------------------+
| Overlay Layer                      |
+------------------------------------+
| Popup Layer                        |
+------------------------------------+
| Notification Layer                 |
+------------------------------------+
| Control Layer                      |
+------------------------------------+
| Content Layer                      |
+------------------------------------+
| Background Layer                   |
+------------------------------------+
| Framebuffer                        |
+------------------------------------+
```

---

# 6. Architekturprinzipien

## Trennung der Verantwortlichkeiten

Jede Komponente besitzt genau eine Aufgabe.

Beispiel:

Rendering kennt keine Controls.

Controls kennen kein Rendering.

Layout kennt keine Ressourcen.

Animation kennt keine Eingaben.

---

## Lose Kopplung

Subsysteme kommunizieren ausschließlich über definierte APIs.

Keine direkte gegenseitige Manipulation.

---

## Austauschbarkeit

Jedes Backend kann ersetzt werden.

Beispiel:

```
BIOS Backend

↓

UEFI Backend

↓

Virtuelles Backend

↓

Test Backend
```

ohne Änderungen an der UI.

---

## Determinismus

Bei identischen Eingaben muss immer dieselbe Ausgabe entstehen.

Keine zufälligen Effekte.

Keine unkontrollierten Timer.

Keine Race Conditions.

---

# 7. UI Lebenszyklus

```
Initialisierung
↓
Assets laden
↓
Theme laden
↓
Scene erstellen
↓
Layout berechnen
↓
Animation starten
↓
Rendering
↓
Present
↓
Eingaben
↓
Scene Update
↓
Layout
↓
Rendern
```

---

# 8. Datenfluss

```
Input
↓
Input Manager
↓
Focus Manager
↓
Scene Graph
↓
Control
↓
Application
↓
State
↓
Layout
↓
Renderer
↓
Framebuffer
```

---

# 9. Zustandsmodell

Jedes UI Objekt besitzt mindestens:

```
Created
↓
Initialized
↓
Visible
↓
Focused
↓
Pressed
↓
Disabled
↓
Hidden
↓
Destroyed
```

---

# 10. Speicherarchitektur

Es existieren folgende Speicherbereiche.

## Permanent

- Theme
- Icons
- Fonts

---

## Scene

- UI Objekte

---

## Frame

Temporäre Daten.

Nach jedem Frame gelöscht.

---

## Cache

- Glyph Cache
- SVG Cache
- Blur Cache

---

# 11. Rendering Pipeline

```
Scene Update
↓
Layout
↓
Dirty Detection
↓
Animations
↓
Render Queue
↓
Compositor
↓
Framebuffer
↓
Present
```

---

# 12. Fehlerbehandlung

Jeder Fehler besitzt:

- Fehlercode
- Ursache
- Recovery
- Logging
- Fallback

Beispiel:

```
SVG Fehler
↓
PNG Ersatz
↓
Falls nicht vorhanden
↓
Icon Platzhalter
```

---

# 13. Erweiterbarkeit

Neue Komponenten dürfen:

- neue Controls hinzufügen
- neue Dialoge hinzufügen
- neue Animationen hinzufügen
- neue Themes hinzufügen
- neue Layouts hinzufügen

ohne bestehende Komponenten zu verändern.

---

# 14. Sicherheitsanforderungen

Die UI darf niemals:

- Bootvorgang blockieren
- Speicher außerhalb reservierter Bereiche schreiben
- unvalidierte Ressourcen laden
- unendliche Animationen erzeugen
- Stack Overflow verursachen

---

# 15. Performanceziele

## Speicher

Boot UI:

≤ 64 MB

---

## Startzeit

Initialisierung:

< 200 ms

---

## Bildrate

Ziel:

60 FPS

Minimum:

30 FPS

---

## Renderzeit

≤ 8 ms

---

## Eingabelatenz

≤ 16 ms

---

# 16. Designrichtlinien

Die Oberfläche orientiert sich vollständig an der Nova Design Language.

Eigenschaften:

- Dark Mode
- Light Mode
- Glass Material
- Blur
- Transparenz
- Schatten
- große Rundungen
- Fluent Animationen
- hohe Kontraste

---

# 17. Kompatibilität

Unterstützte Plattformen:

- BIOS
- UEFI
- QEMU
- VMware
- Hyper-V
- VirtualBox
- Bare Metal

---

# 18. Testanforderungen

Zu testen sind mindestens:

- alle Auflösungen
- alle Skalierungen
- alle Themes
- sämtliche Dialoge
- sämtliche Controls
- BIOS
- UEFI
- beschädigte Assets
- fehlende Fonts
- beschädigte SVGs
- Speichergrenzen
- Eingabegeräte
- Performance
- Recovery

---

# 19. Referenzen

- ADR-BOOTUI-0001 – Eigene Boot-UI-Engine
- ADR-BOOTUI-0002 – Einheitliche UI-Architektur für BIOS und UEFI
- ADR-BOOTGFX-0004 – Ebenenbasiertes Compositing
- ADR-BOOTCONTROL-0001 – Gemeinsames Basismodell für Bedienelemente
- ADR-BOOTINPUT-0001 – Vereinheitlichte Eingabeereignisse
- ADR-BOOTRESOURCE-0009 – Design-Token-Paket für Bootmanager und NovaOS
- ADR-BOOTDESIGN-0001 – Ableitung vom Nova Design Language

---

# 20. Zusammenfassung

Die Bootmanager-UI von NovaOS ist als vollständig eigenständige, modulare und deterministische Architektur konzipiert. Sie trennt Rendering, Layout, Eingabe, Navigation, Animationen und Ressourcen strikt voneinander und schafft damit die Grundlage für eine hochwertige Benutzeroberfläche, die bereits vor dem Start des Kernels den visuellen und funktionalen Anspruch von NovaOS vermittelt. Sie bildet das Fundament für sämtliche weiteren Boot-UI-Spezifikationen wie Rendering, Controls, Dialoge, Navigation und Motion.