# NPSPEC-BOOTRENDER-0011
# 2D Transformations

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-BOOTRENDER-0011 |
| Titel | 2D Transformations |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Bootmanager / Rendering |
| Priorität | Hoch |
| Abhängigkeiten | NPSPEC-BOOTRENDER-0001 bis NPSPEC-BOOTRENDER-0010 |
| Zugehörige ADRs | ADR-BOOTGFX-0007, ADR-BOOTUI-0008, ADR-BOOTPERF-0002 |

---

# 1. Ziel

Diese Spezifikation definiert das vollständige **2D-Transformationssystem** der NovaOS Boot Rendering Engine.

Das System ermöglicht die geometrische Transformation sämtlicher grafischer Objekte vor der Rasterisierung.

Unterstützt werden:

- Translation
- Rotation
- Skalierung
- Spiegelung
- Scherung (Shearing)
- Matrixtransformationen
- Pivot-Transformationen
- kombinierte Transformationen

Alle Transformationen erfolgen vollständig unabhängig vom verwendeten Firmware-Backend. Affine 2D-Transformationen bilden die Grundlage für Verschieben, Rotieren und Skalieren von Objekten. :contentReference[oaicite:0]{index=0}

---

# 2. Geltungsbereich

Das Transformationssystem wird verwendet von:

- Layern
- Controls
- Dialogen
- Bildern
- SVG-Grafiken
- Text
- Animationen
- Mauszeiger
- Blur-Flächen
- Schatten

Nicht Bestandteil sind:

- 3D-Transformationen
- Perspektivprojektionen
- Physiksimulationen

---

# 3. Architektur

```text
Scene Graph
      │
      ▼
Transform Component
      │
      ▼
Transformation Stack
      │
      ▼
Clipping
      │
      ▼
Rasterisierung
```

---

# 4. Grundprinzip

Jedes Renderobjekt besitzt genau eine lokale Transformation.

Vor der Rasterisierung werden alle Transformationen entlang des Scene Graphs miteinander multipliziert.

```text
Local

↓

Parent

↓

World

↓

Screen
```

---

# 5. Koordinatensystem

Das Rendering verwendet ein kartesisches 2D-System.

```text
0,0 ─────────────────► X

 │
 │
 │
 ▼

Y
```

Der Ursprung befindet sich links oben.

---

# 6. Transformationstypen

Folgende Transformationen sind Bestandteil der Engine:

- Identity
- Translation
- Rotation
- Scale
- Mirror Horizontal
- Mirror Vertical
- Shear X
- Shear Y
- Affine Matrix

---

# 7. Reihenfolge

Transformationen werden in folgender Reihenfolge angewendet:

```text
Scale

↓

Rotation

↓

Translation
```

Eine Änderung dieser Reihenfolge führt zu einem anderen Ergebnis und ist deshalb nicht zulässig.

---

# 8. Matrixdarstellung

Alle Transformationen werden intern als affine 3×3-Matrix dargestellt.

```text
| m11 m12 m13 |
| m21 m22 m23 |
|  0   0   1  |
```

Zur Optimierung darf die letzte Zeile implizit gespeichert werden. Affine 2D-Transformationen lassen sich kompakt als 3×2-Matrix repräsentieren. :contentReference[oaicite:1]{index=1}

---

# 9. Datentyp

```c
typedef struct
{
    float m11;
    float m12;

    float m21;
    float m22;

    float tx;
    float ty;
} nova_transform2d_t;
```

---

# 10. Identity

Die Identitätstransformation verändert keine Koordinaten.

```text
|1 0 0|
|0 1 0|
|0 0 1|
```

---

# 11. Translation

Translation verschiebt Objekte.

```text
X' = X + tx

Y' = Y + ty
```

---

# 12. Rotation

Rotation erfolgt um einen definierten Pivot.

```text
0°

45°

90°

180°

270°
```

Der Winkel wird intern im Bogenmaß gespeichert.

---

# 13. Skalierung

Unterstützt werden:

```text
Uniform

Nicht Uniform
```

Beispiele:

```text
1.0

0.5

2.0

1.5
```

Negative Skalierungswerte erzeugen Spiegelungen.

---

# 14. Spiegelung

Unterstützt werden:

- Horizontal
- Vertikal
- Beide Achsen

---

# 15. Shearing

Unterstützt werden:

- X Shear
- Y Shear

Shearing wird hauptsächlich für Animationen verwendet.

---

# 16. Pivot

Jede Transformation besitzt optional einen Pivot.

```text
Object

↓

Pivot

↓

Transformation
```

Standardmäßig liegt der Pivot im Mittelpunkt.

---

# 17. Welttransformation

Die Welttransformation ergibt sich aus:

```text
Parent

×

Local
```

---

# 18. Transformationsstack

Die Runtime verwaltet einen Stack.

```text
Push

↓

Push

↓

Pop

↓

Pop
```

---

# 19. Kombination

Mehrere Transformationen werden durch Matrixmultiplikation kombiniert.

```text
Parent

×

Child

=

World
```

---

# 20. Reihenfolge im Renderer

```text
Layout

↓

Transformation

↓

Clipping

↓

Masking

↓

Rasterisierung
```

---

# 21. Bounding Box

Nach jeder Transformation wird eine neue Bounding Box berechnet.

Sie dient:

- Clipping
- Dirty Regions
- Hit Testing

---

# 22. Dirty Regions

Transformationen erzeugen automatisch Dirty Regions.

Dabei werden berücksichtigt:

- alte Position
- neue Position

---

# 23. Clipping

Das Clipping erfolgt immer nach der Transformation.

Dadurch werden ausschließlich tatsächlich sichtbare Pixel verarbeitet.

---

# 24. Layer

Layer besitzen eigene Welttransformationen.

Kindobjekte übernehmen diese automatisch.

---

# 25. Dialoge

Dialoge können:

- einblenden
- ausblenden
- skalieren
- rotieren

Alle Effekte basieren auf dem Transformationssystem.

---

# 26. Animationen

Animationen verändern ausschließlich Transformationsparameter.

Beispiele:

- Position
- Rotation
- Skalierung

Dadurch bleiben Layout und Rendering getrennt.

---

# 27. SVG

SVG-Grafiken übernehmen dieselben Transformationen wie andere Renderobjekte.

Eine Sonderbehandlung findet nicht statt.

---

# 28. Bilder

Rasterbilder werden transformiert, bevor sie gerastert werden.

---

# 29. Text

Text darf:

- verschoben
- skaliert
- rotiert

werden.

Nicht zulässig sind perspektivische Verzerrungen.

---

# 30. Mauszeiger

Der Mauszeiger verwendet ausschließlich Translation.

Rotationen des Mauszeigers sind nicht vorgesehen.

---

# 31. Performance

Anforderungen:

- Matrixmultiplikation < 1 µs
- Transformation eines Controls < 5 µs
- keine Heap-Allokationen
- SIMD-Optimierung zulässig

---

# 32. Speicher

Transformationen sind Bestandteil jedes Renderobjekts.

Sie werden zusammen mit dem Objekt gespeichert.

---

# 33. Sicherheit

Das System verhindert:

- NaN
- unendliche Werte
- Matrixoverflow
- rekursive Transformationen
- ungültige Matrizen

---

# 34. API

```c
void
nova_transform_identity(
    nova_transform2d_t* transform);

void
nova_transform_translate(
    nova_transform2d_t* transform,
    float x,
    float y);

void
nova_transform_rotate(
    nova_transform2d_t* transform,
    float angle);

void
nova_transform_scale(
    nova_transform2d_t* transform,
    float sx,
    float sy);

void
nova_transform_multiply(
    nova_transform2d_t* result,
    const nova_transform2d_t* left,
    const nova_transform2d_t* right);
```

---

# 35. Diagnosedaten

Zu erfassen sind:

- Anzahl Transformationen
- Matrixmultiplikationen
- Rotationen
- Translationen
- Skalierungen
- Dirty Regions durch Transformationen
- durchschnittliche Transformationszeit

---

# 36. Testfälle

Zu testen sind:

- Identity
- Translation
- Rotation
- Skalierung
- Spiegelung
- Shearing
- Pivot
- Matrixkombination
- Clipping
- Dirty Regions

---

# 37. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- sämtliche Renderobjekte transformiert werden können.
- Matrixkombinationen korrekt funktionieren.
- Clipping nach der Transformation erfolgt.
- Transformationen Dirty Regions korrekt erzeugen.
- keine Heap-Allokationen im Render-Hotpath stattfinden.
- BIOS- und UEFI-Renderer identische Ergebnisse liefern.

---

# 38. Referenzen

## ADRs

- ADR-BOOTGFX-0007 – Transformationen über 2D-Matrizen
- ADR-BOOTUI-0008 – Auflösungsunabhängiges Koordinatensystem
- ADR-BOOTPERF-0002 – Keine Speicherallokation im Render-Hotpath

## NPSPECs

- NPSPEC-BOOTRENDER-0001 – 2D Rendering Engine
- NPSPEC-BOOTRENDER-0004 – Layer and Surface Management
- NPSPEC-BOOTRENDER-0009 – Dirty Region Rendering
- NPSPEC-BOOTRENDER-0010 – Clipping and Masks

---

# 39. Zusammenfassung

Das **2D Transformationssystem** bildet die geometrische Grundlage der NovaOS Boot Rendering Engine. Sämtliche grafischen Objekte werden über affine 2D-Matrizen transformiert und anschließend geclippt sowie gerastert. Durch einen hierarchischen Transformationsstack, Pivot-Unterstützung und deterministische Matrixoperationen entsteht ein leistungsfähiges, plattformunabhängiges System für moderne Benutzeroberflächen im Bootmanager.