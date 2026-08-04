# NPSPEC-BOOTUI-0006
# Resolution and Scaling System

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTUI-0006 |
| Titel | Resolution and Scaling System |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Bootmanager |
| Priorität | Kritisch |
| Abhängigkeiten | NPSPEC-BOOTUI-0001 bis NPSPEC-BOOTUI-0005 |
| ADRs | ADR-BOOTUI-0008, ADR-BOOTUI-0009, ADR-BOOTDESIGN-0002 |

---

# 1. Ziel

Diese Spezifikation definiert das vollständige **Resolution and Scaling System** des NovaOS Bootmanagers.

Das System stellt sicher, dass die Benutzeroberfläche unabhängig von:

- Bildschirmauflösung
- Pixeldichte (DPI)
- Seitenverhältnis
- Firmware (BIOS/UEFI)
- Monitorgröße
- Grafikhardware

immer dieselbe optische Wirkung besitzt.

Der Benutzer soll niemals das Gefühl haben, dass die Oberfläche auf einem 4K-Monitor kleiner oder auf einem VGA-Monitor überdimensioniert wirkt.

---

# 2. Architekturziele

Das Skalierungssystem verfolgt folgende Ziele:

- Auflösungsunabhängigkeit
- DPI-Unabhängigkeit
- Responsive Layouts
- Pixelgenaue Darstellung
- Konsistente Bedienelementgrößen
- Einheitliche Schriftgrößen
- Einheitliche Animationen
- Einheitliche Abstände
- Einheitliche Rundungen
- Einheitliche Schatten

---

# 3. Grundprinzip

Intern arbeitet die gesamte Boot-UI **niemals direkt mit Pixeln**.

Alle Größen werden zunächst in **Logical Units (LU)** beschrieben.

```
Logical Units

↓

Scaling Engine

↓

Physical Pixels

↓

Framebuffer
```

Dadurch bleiben Layouts unabhängig von der tatsächlichen Bildschirmauflösung. Die Trennung zwischen logischen Einheiten und physischen Pixeln ist ein etabliertes Prinzip moderner UI-Systeme, um eine konsistente physische Größe auf Displays mit unterschiedlichen Pixeldichten zu erreichen. :contentReference[oaicite:0]{index=0}

---

# 4. Logical Units

NovaOS definiert eine eigene Maßeinheit:

```
1 LU
```

(Logical Unit)

Diese entspricht einer virtuellen Maßeinheit.

Alle Werte werden ausschließlich in LU gespeichert.

Beispiele

```
Button

Breite

320 LU

Höhe

48 LU

Radius

12 LU
```

---

# 5. Skalierungsmodell

Die Runtime berechnet:

```
Logical Unit

↓

Scale Factor

↓

Physical Pixel
```

Formel

```
Pixel = LU × ScaleFactor
```

Der ScaleFactor ist systemweit identisch.

---

# 6. Referenzauflösung

NovaOS verwendet folgende Referenz:

```
1920 × 1080

100 %

Scale = 1.0
```

Alle weiteren Auflösungen werden daraus berechnet.

---

# 7. Referenz-DPI

Standard

```
96 DPI
```

Falls keine zuverlässigen DPI-Daten verfügbar sind, wird eine konservative Skalierung anhand der Auflösung verwendet. Moderne UI-Systeme verwenden ebenfalls logische Einheiten statt fester Pixelwerte, um unterschiedliche Pixeldichten auszugleichen. :contentReference[oaicite:1]{index=1}

---

# 8. Skalierungsfaktoren

| Faktor | Beschreibung |
|---------|--------------|
| 100 % | 1.00 |
| 125 % | 1.25 |
| 150 % | 1.50 |
| 175 % | 1.75 |
| 200 % | 2.00 |
| 225 % | 2.25 |
| 250 % | 2.50 |
| 300 % | 3.00 |

Andere Faktoren sind zulässig.

---

# 9. Automatische Skalierung

Die Runtime berechnet automatisch

```
Auflösung

↓

DPI

↓

Monitorgröße

↓

Scale Factor
```

Falls DPI unbekannt:

```
Auflösung

↓

Heuristik

↓

Scale
```

---

# 10. Auflösungsklassen

## XS

```
640×480
```

---

## S

```
800×600
```

---

## M

```
1024×768
```

---

## L

```
1280×720
```

---

## XL

```
1920×1080
```

---

## XXL

```
2560×1440
```

---

## Ultra

```
3840×2160
```

Neue Klassen können jederzeit ergänzt werden.

---

# 11. Seitenverhältnisse

Unterstützt werden

```
4:3

5:4

16:9

16:10

21:9

32:9
```

Alle Layouts müssen adaptiv reagieren.

---

# 12. Viewport

Jeder Bildschirm besitzt

```
Logical Width

Logical Height

Scale

Safe Area
```

Beispiel

```
1920x1080

↓

Logical

1920x1080

↓

Scale 1.0
```

---

# 13. Safe Area

Nicht alle Pixel dürfen verwendet werden.

Es existieren

```
Top

Bottom

Left

Right
```

Margins.

Diese berücksichtigen

- Firmwareeinblendungen
- Overscan
- Hardwarebesonderheiten

---

# 14. Pixel Alignment

Alle Koordinaten werden vor dem Rendering ausgerichtet.

```
23.7 Pixel

↓

24 Pixel
```

Dadurch werden

- unscharfe Linien
- verschwommene Icons
- Textartefakte

vermieden.

---

# 15. Schriftgrößen

Intern

```
14 LU
```

↓

Scale

↓

Pixel

Die Schrift wird unabhängig von der Bildschirmauflösung immer mit derselben optischen Größe dargestellt.

---

# 16. Rundungen

Alle Radien werden ebenfalls skaliert.

Beispiel

```
12 LU

↓

18 Pixel
```

bei

150 %

---

# 17. Schatten

Skalierte Eigenschaften

- Radius
- Offset
- Blur
- Opacity

Somit behalten Schatten ihre visuelle Wirkung auf allen Auflösungen.

---

# 18. Glass Material

Das Glass Material skaliert

- Blur Radius
- Noise
- Transparenz
- Kanten

automatisch.

---

# 19. Icons

SVG

↓

skalieren verlustfrei

PNG

↓

mehrere Größen

```
16

24

32

48

64

96

128

256
```

Vector-Grafiken eignen sich besonders für skalierbare Benutzeroberflächen, da sie unabhängig von der Auflösung scharf bleiben. Rastergrafiken sollten dagegen in mehreren Auflösungen bereitgestellt werden. :contentReference[oaicite:2]{index=2}

---

# 20. Layoutregeln

Layout verwendet

- Grid
- Stack
- Constraints

Absolute Pixelgrößen sind verboten.

---

# 21. Responsive Verhalten

Falls Bildschirm kleiner

↓

Controls umbrechen

↓

Dialoge verkleinern

↓

Schrift minimal reduzieren

↓

Scrollen aktivieren

---

# 22. Animationen

Animationen verwenden

```
Logical Distance
```

nicht

Pixel.

Dadurch bleiben Bewegungen auf allen Geräten gleich.

---

# 23. DPI-Wechsel

Falls

```
Resolution

oder

Scale

geändert
```

wird

↓

Neuberechnung

- Layout
- Controls
- Fonts
- Icons
- Blur
- Shadow

---

# 24. Benutzerkonfiguration

Der Benutzer kann

```
100 %

125 %

150 %

175 %

200 %
```

erzwingen.

Automatik bleibt Standard.

---

# 25. API

```cpp
SetScale()

GetScale()

SetResolution()

GetResolution()

LogicalToPixel()

PixelToLogical()

GetViewport()

GetSafeArea()
```

---

# 26. Performance

Die Skalierungsberechnung erfolgt

- beim Start
- nach Moduswechsel
- nach DPI-Wechsel

Nicht

bei jedem Frame.

---

# 27. Fehlerbehandlung

Ungültige Auflösung

↓

Standardwert

Ungültiger Scale

↓

100 %

Ungültige DPI

↓

Heuristik

---

# 28. Testanforderungen

Zu testen

- VGA
- HD
- FullHD
- QHD
- UHD
- UltraWide
- Dual Screen
- BIOS
- UEFI

Weiterhin

- Themewechsel
- Dialoge
- Animationen
- Schrift
- Icons
- Glass
- Blur

---

# 29. Referenzen

## ADR

- ADR-BOOTUI-0008 – Auflösungsunabhängiges Koordinatensystem
- ADR-BOOTUI-0009 – Skalierung anhand Auflösung und Pixeldichte
- ADR-BOOTDESIGN-0002 – Gemeinsame Design-Tokens

## NPSPEC

- NPSPEC-BOOTUI-0001 – Bootmanager UI Architecture
- NPSPEC-BOOTUI-0005 – BIOS/UEFI Graphics Abstraction
- NPSPEC-BOOTLAYOUT-0001 – Boot Layout Engine
- NPSPEC-BOOTRENDER-0001 – 2D Rendering Engine

---

# 30. Zusammenfassung

Das **Resolution and Scaling System** bildet die Grundlage für eine vollständig auflösungsunabhängige Bootoberfläche. Sämtliche UI-Elemente werden in **Logical Units** definiert und erst durch die Skalierungs-Engine in physische Pixel umgerechnet. Dadurch bleiben Bedienelemente, Typografie, Abstände, Animationen und visuelle Effekte auf Displays mit unterschiedlichen Auflösungen und Pixeldichten konsistent. Das System unterstützt automatische und benutzerdefinierte Skalierungsfaktoren, responsive Layouts sowie unterschiedliche Seitenverhältnisse und stellt sicher, dass die NovaOS-Bootoberfläche auf BIOS- und UEFI-Systemen stets ein einheitliches Erscheinungsbild bietet.