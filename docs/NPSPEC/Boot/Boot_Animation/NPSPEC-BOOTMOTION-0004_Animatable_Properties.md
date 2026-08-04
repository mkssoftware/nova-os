# NPSPEC-BOOTMOTION-0004
# Animatable Properties

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-BOOTMOTION-0004 |
| Titel | Animatable Properties |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Bootmanager / Motion |
| Priorität | Kritisch |
| Abhängigkeiten | NPSPEC-BOOTMOTION-0001 bis NPSPEC-BOOTMOTION-0003, NPSPEC-BOOTCOMPOSITOR-0001 |
| Zugehörige ADRs | ADR-BOOTMOTION-0003, ADR-BOOTMOTION-0005, ADR-BOOTGFX-0007, ADR-BOOTGFX-0011 |

---

# 1. Ziel

Diese Spezifikation definiert sämtliche **animierbaren Eigenschaften (Animatable Properties)** der NovaOS Boot UI.

Sie legt fest:

- welche Eigenschaften animiert werden dürfen,
- welche Datentypen verwendet werden,
- welche Interpolationsverfahren zulässig sind,
- welche Eigenschaften gleichzeitig animiert werden können,
- welche Einschränkungen gelten.

Das Ziel ist eine einheitliche, deterministische und hardwareunabhängige Animationsplattform.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für alle UI-Elemente des Bootmanagers.

Sie umfasst:

- Controls
- Panels
- Dialoge
- Menüs
- Overlays
- Glass Materials
- Acrylic Materials
- Layer
- Cursor
- Icons

Nicht Bestandteil sind:

- Audio
- Physiksimulationen
- Partikelsysteme

---

# 3. Architektur

```text
Animation

↓

Property Animator

↓

Animatable Property

↓

Render Command

↓

Compositor
```

---

# 4. Grundprinzip

Animationen verändern ausschließlich Properties.

Sie verändern niemals direkt:

- Pixel
- Render Commands
- Framebuffer
- Speicherstrukturen

Alle Änderungen erfolgen über das Property-System.

---

# 5. Propertyarten

Unterstützt werden:

- numerische Werte
- Vektoren
- Farben
- Matrizen
- Winkel
- Rechtecke
- Transformationen
- Materialparameter

---

# 6. Propertybeschreibung

```c
typedef struct
{
    uint32_t id;

    uint32_t type;

    void* target;

} nova_animatable_property_t;
```

---

# 7. Positionswerte

Animierbar:

- X
- Y
- Z (logisch)

Interpolation:

```text
Linear
```

---

# 8. Größen

Animierbar:

- Breite
- Höhe
- Radius
- Rahmenstärke

Interpolation:

```text
Linear
```

---

# 9. Rotation

Animierbar:

- 2D-Rotation

Einheit:

```text
Grad
```

Interpolation erfolgt immer über den kürzesten Winkel.

---

# 10. Skalierung

Animierbar:

- Scale X
- Scale Y
- Uniform Scale

Negative Skalierungswerte sind unzulässig.

---

# 11. Deckkraft

Animierbar:

```text
0.0

↓

1.0
```

Interpolation erfolgt linear.

---

# 12. Farben

Animierbar:

- Hintergrundfarbe
- Rahmenfarbe
- Tintfarbe
- Glowfarbe

Interpolation erfolgt komponentenweise im RGBA-Farbraum.

---

# 13. Blur

Animierbar:

- Blur Radius

Negative Werte sind unzulässig.

---

# 14. Schatten

Animierbar:

- Deckkraft
- Offset X
- Offset Y
- Blur
- Farbe

---

# 15. Glow

Animierbar:

- Intensität
- Radius
- Deckkraft
- Farbe

---

# 16. Border

Animierbar:

- Breite
- Transparenz
- Farbe

---

# 17. Corner Radius

Animierbar:

- globaler Radius
- einzelne Ecken

Alle Werte müssen größer oder gleich Null sein.

---

# 18. Transformationen

Animierbar:

- Translation
- Rotation
- Skalierung

Alle Transformationen werden über Matrizen kombiniert.

---

# 19. Materialparameter

Animierbar:

- Tint
- Glass Strength
- Acrylic Strength
- Noise Deckkraft
- Highlight
- Reflection

---

# 20. Layerparameter

Animierbar:

- Sichtbarkeit
- Deckkraft
- Z-Index (diskret)
- Clippingbereich

Der Z-Index darf nur am Ende einer Animation geändert werden.

---

# 21. Cursor

Animierbar:

- Position
- Größe
- Deckkraft

Die Aktualisierung erfolgt synchron mit der globalen Timeline.

---

# 22. Interpolation

Unterstützt werden:

- Linear
- Cubic
- Bezier
- Spring
- Step

Die konkrete Easing-Funktion wird separat definiert.

---

# 23. Gleichzeitige Animationen

Mehrere Properties eines Objekts dürfen gleichzeitig animiert werden.

Beispiel:

- Position
- Größe
- Deckkraft
- Blur

innerhalb derselben Timeline.

---

# 24. Einschränkungen

Nicht animierbar sind:

- Objekt-ID
- Speicheradresse
- Ressourcenzuordnung
- Parent-Beziehung
- Objektklasse

Diese Eigenschaften dürfen sich ausschließlich außerhalb laufender Animationen ändern.

---

# 25. Speicherverwaltung

Alle Property-Animationen verwenden feste Property-Deskriptoren.

Heap-Allokationen während des Animations-Hotpaths sind verboten.

---

# 26. Performance

Anforderungen:

- O(1)-Propertyzugriff
- SIMD-fähige Interpolation
- keine rekursiven Propertyupdates
- deterministische Laufzeit

---

# 27. Fehlerbehandlung

Folgende Fehler müssen erkannt werden:

- ungültige Property-ID
- falscher Datentyp
- Nullpointer
- ungültiger Wertebereich
- doppelte Property-Animation

Ungültige Animationen werden sofort beendet.

---

# 28. Sicherheit

Das System verhindert:

- gleichzeitige widersprüchliche Propertyupdates
- ungültige Typkonvertierungen
- Speicherbeschädigungen
- Out-of-Bounds-Zugriffe
- Nullpointer

Alle Propertyänderungen werden vor ihrer Anwendung validiert.

---

# 29. API

```c
nova_result_t
nova_property_register(
    nova_animatable_property_t* property);

nova_result_t
nova_property_set_value(
    nova_animatable_property_t* property,
    const void* value);

nova_result_t
nova_property_interpolate(
    nova_animatable_property_t* property,
    float progress);

bool
nova_property_is_animatable(
    uint32_t property_id);
```

---

# 30. Diagnosedaten

Zu erfassen sind:

- animierte Properties
- aktive Propertyanimationen
- Interpolationen
- Materialanimationen
- Fehleranzahl
- Propertyzugriffe
- Schedulerlaufzeit
- Speicherverbrauch

---

# 31. Testfälle

Zu testen sind:

- Positionsanimation
- Größenanimation
- Rotationsanimation
- Deckkraft
- Blur
- Schatten
- Glow
- Materialanimation
- gleichzeitige Properties
- ungültige Property

---

# 32. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- sämtliche unterstützten Properties korrekt animiert werden.
- mehrere Properties gleichzeitig animiert werden können.
- Interpolationen deterministisch arbeiten.
- ungültige Propertyänderungen erkannt werden.
- Materialparameter vollständig unterstützt werden.
- keine Heap-Allokationen im Animations-Hotpath stattfinden.
- BIOS- und UEFI-Systeme identische Ergebnisse liefern.

---

# 33. Referenzen

## ADRs

- ADR-BOOTMOTION-0003 – Property-basierte Animationen
- ADR-BOOTMOTION-0005 – Unterbrechbare und reversible Animationen
- ADR-BOOTGFX-0007 – Transformationen über 2D-Matrizen
- ADR-BOOTGFX-0011 – Qualitätsstufen für grafische Effekte

## NPSPECs

- NPSPEC-BOOTMOTION-0001 – Boot Motion System
- NPSPEC-BOOTMOTION-0002 – Animation Timeline
- NPSPEC-BOOTMOTION-0003 – Animation Scheduler
- NPSPEC-BOOTCOMPOSITOR-0006 – Glass Material
- NPSPEC-BOOTCOMPOSITOR-0007 – Acrylic Material
- NPSPEC-BOOTRENDER-0011 – 2D Transformations

---

# 34. Zusammenfassung

Die **Animatable Properties** definieren sämtliche Eigenschaften, die innerhalb der NovaOS Bootoberfläche animiert werden dürfen. Durch ein einheitliches Property-Modell, deterministische Interpolation und eine klare Trennung zwischen Objektzustand und Rendering entsteht eine flexible und leistungsfähige Animationsplattform. Das System unterstützt sowohl klassische UI-Animationen als auch Material- und Transformationseffekte und gewährleistet konsistentes Verhalten auf BIOS- und UEFI-Systemen.