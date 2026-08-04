# NPSPEC-BOOTUI-0010
# NovaOS Design Compatibility

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTUI-0010 |
| Titel | NovaOS Design Compatibility |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Bootmanager |
| Priorität | Kritisch |
| Abhängigkeiten | NPSPEC-BOOTUI-0001 bis NPSPEC-BOOTUI-0009 |
| ADRs | ADR-BOOTUI-0010, ADR-BOOTUI-0011, ADR-BOOTRESOURCE-0009, ADR-BOOTRESOURCE-0010, ADR-BOOTDESIGN-0001 bis ADR-BOOTDESIGN-0005 |

---

# 1. Ziel

Diese Spezifikation definiert die vollständige Design-Kompatibilität zwischen der Bootoberfläche und der späteren NovaOS Desktop-Oberfläche.

Das Ziel besteht darin, dass der Benutzer **keinen visuellen Bruch** zwischen Bootmanager und Betriebssystem wahrnimmt.

Der Übergang soll wirken, als würde dieselbe Benutzeroberfläche lediglich weitere Funktionen freischalten.

Der Bootmanager ist deshalb **kein separates Produkt**, sondern die erste Laufzeitstufe der NovaOS Design Language.

---

# 2. Architekturziele

Die Design-Kompatibilität verfolgt folgende Ziele.

- Einheitliches Erscheinungsbild
- Gemeinsame Design Language
- Gemeinsame Komponenten
- Gemeinsame Animationen
- Gemeinsame Farben
- Gemeinsame Typografie
- Gemeinsame Icons
- Gemeinsame Design Tokens
- Gemeinsame Bedienphilosophie
- Zukunftssichere Erweiterbarkeit

---

# 3. Architektur

```
                 Nova Design Language
                          │
          Design Tokens / Komponenten
                          │
        ┌─────────────────┴─────────────────┐
     Boot UI                          NovaOS Desktop
        │                                   │
    Boot Renderer                    Desktop Renderer
        │                                   │
     BIOS / UEFI                      Kernel + GPU
```

Die Bootoberfläche verwendet dieselbe Designbasis wie der Desktop.

Lediglich die Implementierung der Renderpipeline unterscheidet sich.

---

# 4. Design-Ebenen

Die Designarchitektur besteht aus vier Ebenen.

```
Nova Design Language
↓
Design Tokens
↓
UI Components
↓
Renderer
```

Nur der Renderer unterscheidet sich.

Alle übrigen Ebenen bleiben identisch.

---

# 5. Design Tokens

Folgende Tokens müssen identisch sein.

## Farben

- Hintergrund
- Oberfläche
- Primärfarbe
- Sekundärfarbe
- Akzentfarbe
- Warnung
- Fehler
- Erfolg

---

## Typografie

- Schriftfamilie
- Schriftgrößen
- Schriftgewichte
- Zeilenhöhe
- Buchstabenabstände

---

## Abstände

- Margin
- Padding
- Grid
- Safe Area

---

## Formen

- Rundungen
- Rahmen
- Schatten
- Blur
- Transparenz

---

## Animation

- Dauer
- Geschwindigkeit
- Easing
- Spring
- Fade
- Slide
- Scale

---

# 6. Komponentenkompatibilität

Folgende Komponenten müssen sich identisch verhalten.

- Button
- List
- Tree
- Dialog
- Progress
- Checkbox
- Switch
- TextField
- Spinner
- Notification
- Navigation
- Menü
- Overlay

Der Benutzer soll keinen Unterschied erkennen.

---

# 7. Designrichtlinien

Alle Komponenten müssen dieselben Regeln verwenden.

Beispiel

Button

```
Radius
12 LU

Padding
16 LU

Height
48 LU
```

Desktop

↓

identisch

Bootmanager

---

# 8. Glass Design

Das Glass Design besitzt dieselben Eigenschaften.

- Transparenz
- Blur
- Lichtkante
- Noise
- Schatten
- Abrundungen

Falls Hardware dies nicht zulässt

↓

automatische Degradation

↓

optisch möglichst identisch.

---

# 9. Animationen

Animationen besitzen

- dieselben Geschwindigkeiten
- dieselben Kurven
- dieselben Übergänge

Beispiel

Dialog

```
Opacity
0 %
↓
100 %

+

Scale
95 %
↓
100 %
```

Desktop und Bootmanager verwenden dieselben Bewegungsmuster.

---

# 10. Typografie

Bootmanager und Desktop verwenden dieselben Regeln.

Beispiel

```
Heading
32 LU

Body
14 LU

Caption
12 LU
```

Schriftarten dürfen sich intern unterscheiden.

Die optische Wirkung muss identisch bleiben.

---

# 11. Icons

Primärformat

```
SVG
```

Fallback

```
PNG
```

Alle Icons stammen aus derselben Designbibliothek.

---

# 12. Layoutsystem

Beide Oberflächen verwenden

- Grid
- Stack
- Constraints
- Safe Area
- Responsive Regeln

Pixelbasierte Layouts sind verboten.

---

# 13. Farben

Alle Farben stammen ausschließlich aus den Design Tokens.

Beispiel

```
Surface
↓
Primary
↓
Accent
↓
Success
↓
Error
```

Eigene Farben einzelner Komponenten sind nicht erlaubt.

---

# 14. Zustände

Alle Komponenten besitzen dieselben Zustände.

```
Normal
↓
Hover
↓
Pressed
↓
Focused
↓
Disabled
```

Unter BIOS-Systemen ohne Maus kann der Hover-Zustand entfallen.

Alle übrigen Zustände bleiben erhalten.

---

# 15. Navigation

Navigation muss identisch wirken.

Beispiele

- Zurück
- Weiter
- Dialog öffnen
- Dialog schließen
- Seitenwechsel

Animationen dürfen sich technisch unterscheiden.

Die Wahrnehmung muss identisch bleiben.

---

# 16. Theme-System

Bootmanager unterstützt dieselben Themes.

```
Dark
↓
Light
↓
High Contrast
```

Später können weitere Themes ergänzt werden.

---

# 17. Skalierung

Die Skalierungsregeln stammen vollständig aus

```
NPSPEC-BOOTUI-0006
```

Desktop und Bootmanager verwenden dieselben Logical Units.

---

# 18. Ressourcen

Gemeinsam genutzt werden

- Farben
- Icons
- SVG
- Fonts
- Animationen
- Design Tokens

Der Bootmanager verwendet dabei eine vorverarbeitete Teilmenge.

---

# 19. Unterschiede

Nicht identisch sein müssen

- GPU Shader
- Hardwarebeschleunigung
- Videowiedergabe
- 3D
- Fenstermanager
- Desktop Composition

Diese Unterschiede dürfen jedoch **nicht sichtbar** sein.

---

# 20. Designmigration

Neue Version

```
Nova Design
↓
Design Tokens
↓
Boot UI
↓
Desktop
↓
Release
```

Bootmanager und Desktop werden immer gemeinsam aktualisiert.

---

# 21. Versionsverwaltung

Jede Designversion besitzt

```
Major

Minor

Patch
```

Bootmanager und Desktop müssen dieselbe Major-Version verwenden.

---

# 22. OEM-Anpassungen

OEMs dürfen anpassen

- Hintergrundbild
- Logo
- Akzentfarbe
- Bootanimation

Nicht anpassbar

- Komponenten
- Animationen
- Layout
- Navigation
- Design Tokens

Dadurch bleibt die Bedienung konsistent.

---

# 23. API

```cpp
GetTheme()

SetTheme()

GetAccentColor()

GetDesignTokens()

GetTypography()

GetAnimations()

GetIcons()

ValidateDesignVersion()
```

---

# 24. Validierung

Beim Start werden geprüft

- Theme-Version
- Token-Version
- Icon-Version
- Font-Version
- Animation-Version

Inkompatible Versionen werden nicht geladen.

---

# 25. Performance

Die Designkompatibilität darf

- keine zusätzlichen Renderdurchläufe erzeugen
- keine zusätzlichen Layoutberechnungen verursachen
- keine zusätzliche Speicherfragmentierung erzeugen

Alle Tokens werden beim Start geladen.

---

# 26. Sicherheitsanforderungen

Designressourcen dürfen niemals

- Programmcode enthalten
- Skripte ausführen
- Speicheradressen referenzieren
- dynamisch nachgeladen werden

Alle Ressourcen werden vor der Nutzung validiert.

---

# 27. Testanforderungen

Zu testen sind

## Darstellung

- Dark Theme
- Light Theme
- High Contrast

## Komponenten

- Buttons
- Dialoge
- Listen
- Eingabefelder

## Animationen

- Dialog
- Navigation
- Fade
- Scale

## Typografie

- Größen
- Abstände
- Zeilenhöhe

## Icons

- SVG
- PNG
- Skalierung

## Recovery

- fehlende Tokens
- ungültige Themes
- beschädigte Icons

---

# 28. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt wenn

- Bootmanager und Desktop visuell identisch wirken
- alle Design Tokens kompatibel sind
- dieselben Komponentenregeln gelten
- Animationen konsistent erscheinen
- OEM-Anpassungen die Design Language nicht verletzen
- sämtliche Themes kompatibel sind
- Versionskonflikte automatisch erkannt werden

---

# 29. Referenzen

## ADR

- ADR-BOOTUI-0010 – Gemeinsame Rendering-API für Bootmanager und NovaOS
- ADR-BOOTUI-0011 – Begrenzte Wiederverwendung der Nova Design Engine
- ADR-BOOTRESOURCE-0009 – Design-Token-Paket für Bootmanager und NovaOS
- ADR-BOOTRESOURCE-0010 – Versionsgebundene Theme-Ressourcen
- ADR-BOOTDESIGN-0001 – Ableitung vom Nova Design Language
- ADR-BOOTDESIGN-0002 – Gemeinsame Design-Tokens

## NPSPEC

- NPSPEC-BOOTUI-0001 – Bootmanager UI Architecture
- NPSPEC-BOOTUI-0006 – Resolution and Scaling System
- NPSPEC-BOOTRESOURCE-0001 – Boot Resource Manager
- NPSPEC-BOOTDESIGN-0001 – Nova Design Language
- NPSPEC-BOOTMOTION-0001 – Motion System
- NPSPEC-BOOTCONTROL-0001 – Control Framework

---

# 30. Zusammenfassung

Die **NovaOS Design Compatibility** stellt sicher, dass Bootmanager und Desktop dieselbe visuelle Identität, dieselben Interaktionsmuster und dieselbe Design Language verwenden. Beide Systeme greifen auf gemeinsame Design Tokens, Typografie, Farbdefinitionen, Komponentenregeln und Animationsprinzipien zurück. Unterschiede dürfen ausschließlich in der technischen Implementierung der Rendering-Engine bestehen, niemals jedoch im wahrgenommenen Erscheinungsbild. Dadurch entsteht für den Benutzer ein nahtloser Übergang vom ersten Bootbildschirm bis zur vollständig geladenen NovaOS-Desktopumgebung. Diese Architektur erleichtert zudem die Wartung, da Änderungen an der Nova Design Language zentral erfolgen und konsistent auf Bootmanager und Betriebssystem angewendet werden können.