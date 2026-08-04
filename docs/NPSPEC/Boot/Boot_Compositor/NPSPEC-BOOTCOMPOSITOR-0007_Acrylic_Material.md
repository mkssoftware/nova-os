# NPSPEC-BOOTCOMPOSITOR-0007
# Acrylic Material

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-BOOTCOMPOSITOR-0007 |
| Titel | Acrylic Material |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Bootmanager / Compositor |
| Priorität | Hoch |
| Abhängigkeiten | NPSPEC-BOOTCOMPOSITOR-0005, NPSPEC-BOOTCOMPOSITOR-0006, NPSPEC-BOOTRENDER-0013 bis NPSPEC-BOOTRENDER-0018 |
| Zugehörige ADRs | ADR-BOOTDESIGN-0003, ADR-BOOTGFX-0010, ADR-BOOTGFX-0011, ADR-BOOTPERF-0004 |

---

# 1. Ziel

Diese Spezifikation definiert das **Acrylic Material** der NovaOS Boot UI.

Acrylic ist ein halbtransparentes Material, das den Eindruck von satiniertem Acrylglas erzeugt. Im Gegensatz zum allgemeinen Glass Material besitzt Acrylic eine stärkere Materialstruktur und eignet sich besonders für temporäre Oberflächen.

Das Material kombiniert:

- Background Blur
- Farbfilter
- Tint Layer
- Noise Layer
- Surface Color
- Border
- Highlight
- Shadow
- Glow

Das Material passt sich automatisch an Theme, Hintergrund und Qualitätsprofil an. Moderne Acrylic-Materialien kombinieren diese Effekte, um Tiefe und Hierarchie zu erzeugen und fallen bei eingeschränkter Hardware kontrolliert auf Volltonfarben zurück. :contentReference[oaicite:0]{index=0}

---

# 2. Geltungsbereich

Acrylic wird verwendet für:

- Popup-Menüs
- Kontextmenüs
- Flyouts
- Dialoge
- Untermenüs
- Tooltips
- Benachrichtigungen
- temporäre Panels

Nicht verwendet wird Acrylic für:

- Desktop-Hintergrund
- Vollbildoberflächen
- Splash Screen

---

# 3. Architektur

```text
Backdrop

↓

Blur

↓

Tint

↓

Noise

↓

Gradient

↓

Border

↓

Shadow

↓

Glow

↓

Compositor
```

---

# 4. Grundprinzip

Acrylic besteht aus mehreren Materialschichten.

Keine einzelne Schicht darf alleine das endgültige Erscheinungsbild erzeugen.

Das Material wird vollständig während des Compositings erzeugt.

---

# 5. Materialaufbau

Ein Acrylic Material besteht aus:

- Background Capture
- Gaussian Blur
- Tint Layer
- Noise Layer
- Gradient Layer
- Border
- Highlight
- Shadow
- Glow

Alle Ebenen können unabhängig aktiviert werden.

---

# 6. Materialstruktur

```c
typedef struct
{
    float opacity;

    float blur_radius;

    float tint_opacity;

    float noise_opacity;

    float shadow_opacity;

    float glow_opacity;

    nova_color_t tint;

} nova_acrylic_material_t;
```

---

# 7. Background Blur

Der Hintergrund wird zunächst erfasst und weichgezeichnet.

Die Blur-Parameter stammen aus:

- Rendering Quality Profile
- Theme
- Materialtyp

---

# 8. Tint Layer

Nach dem Blur wird eine halbtransparente Farbschicht aufgetragen.

Die Tintfarbe stammt ausschließlich aus den Design Tokens.

Direkte RGB-Werte sind unzulässig.

---

# 9. Noise Layer

Acrylic besitzt eine feine Materialstruktur.

Der Noise Layer:

- reduziert Banding
- simuliert Acrylglas
- verbessert die Tiefenwirkung

Der Noise Layer verwendet ein deterministisches Rauschmuster.

---

# 10. Gradient

Zusätzlich wird ein leichter Verlauf aufgetragen.

Standard:

```text
Hell

↓

Neutral

↓

Etwas dunkler
```

Dadurch wirkt das Material plastischer.

---

# 11. Transparenz

Empfohlene Werte:

| Element | Deckkraft |
|---|---:|
| Popup | 74 % |
| Menü | 76 % |
| Dialog | 80 % |
| Tooltip | 70 % |

Die Werte werden zentral über Design Tokens definiert.

---

# 12. Border

Jede Acrylic-Fläche besitzt einen feinen Rahmen.

Eigenschaften:

- 1 px
- leicht transparent
- heller als die Oberfläche

---

# 13. Highlight

Optional wird entlang der Oberkante ein Lichtreflex dargestellt.

Dieser verstärkt den Materialcharakter.

---

# 14. Schatten

Standardmäßig verwendet Acrylic:

- Ambient Shadow
- Drop Shadow

Die Parameter stammen aus dem Shadow-System.

---

# 15. Glow

Hover- und Fokuszustände dürfen einen dezenten Glow besitzen.

Im Normalzustand ist Glow deaktiviert.

---

# 16. Rounded Geometry

Acrylic verwendet ausschließlich Rounded Geometry.

Die Eckradien stammen aus dem Nova Design Token System.

---

# 17. Background Capture

Der Hintergrund wird unmittelbar vor der Materialerzeugung aufgenommen.

Es werden ausschließlich darunterliegende Layer berücksichtigt.

---

# 18. Clipping

Blur, Noise und Tint werden vollständig auf die Materialgeometrie begrenzt.

Pixel außerhalb der Fläche dürfen nicht verarbeitet werden.

---

# 19. Animationen

Animierbar sind:

- Deckkraft
- Blur Radius
- Tint
- Shadow
- Glow

Noise bleibt während der Laufzeit konstant.

---

# 20. Materialvarianten

Unterstützt werden:

- Thin Acrylic
- Standard Acrylic
- Thick Acrylic
- Accent Acrylic

Die Varianten unterscheiden sich ausschließlich durch Materialparameter.

---

# 21. Qualitätsprofile

Je nach Profil dürfen reduziert werden:

- Blur Radius
- Noise
- Glow
- Shadow
- Gradientqualität

Das Material muss weiterhin eindeutig als Acrylic erkennbar bleiben.

---

# 22. Safe Mode

Im Safe-Profil besteht Acrylic ausschließlich aus:

- Volltonfarbe
- Border

Blur, Noise, Glow und Schatten werden deaktiviert.

---

# 23. Speicherverwaltung

Acrylic verwendet:

- Backdrop Surface
- Blur Surface
- Material Cache

Alle Ressourcen werden beim Boot reserviert.

Heap-Allokationen im Render-Hotpath sind verboten.

---

# 24. Performance

Optimierungen:

- Blur Cache
- Noise Cache
- Dirty Regions
- SIMD
- Surface Cache

Nur tatsächlich sichtbare Bereiche werden verarbeitet.

---

# 25. Fehlerbehandlung

Folgende Fehler müssen erkannt werden:

- fehlende Backdrop Surface
- ungültiger Blur Radius
- beschädigter Noise Cache
- ungültige Tintfarbe
- Speicherfehler

Bei Fehlern erfolgt automatisch der Rückfall auf eine halbtransparente Volltonfläche.

---

# 26. Sicherheit

Das System verhindert:

- rekursive Blur-Berechnungen
- doppelte Materialinstanzen
- ungültige Materialparameter
- Out-of-Bounds-Zugriffe
- Nullpointer

Alle Materialparameter werden vor der Verwendung validiert.

---

# 27. API

```c
nova_result_t
nova_acrylic_initialize(void);

nova_result_t
nova_acrylic_create(
    nova_acrylic_material_t* material);

nova_result_t
nova_acrylic_render(
    const nova_acrylic_material_t* material,
    nova_surface_t* destination);

nova_result_t
nova_acrylic_destroy(
    nova_acrylic_material_t* material);
```

---

# 28. Diagnosedaten

Zu erfassen sind:

- Anzahl Acrylic-Flächen
- Blur Radius
- Noise aktiviert
- Tintfarbe
- Cache Hits
- Cache Misses
- Renderzeit
- Speicherverbrauch

---

# 29. Testfälle

Zu testen sind:

- Thin Acrylic
- Standard Acrylic
- Thick Acrylic
- Accent Acrylic
- Blur deaktiviert
- Noise deaktiviert
- Qualitätswechsel
- Themewechsel
- Animationen
- Safe Mode

---

# 30. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- Acrylic aus Blur, Tint und Noise aufgebaut wird.
- ausschließlich Hintergrundlayer verarbeitet werden.
- Noise deterministisch erzeugt wird.
- Materialparameter vollständig über Design Tokens gesteuert werden.
- Qualitätsprofile korrekt unterstützt werden.
- Safe Mode ohne Blur und Noise funktionsfähig bleibt.
- keine Heap-Allokationen im Render-Hotpath stattfinden.
- BIOS- und UEFI-Systeme identische Ergebnisse liefern.

---

# 31. Referenzen

## ADRs

- ADR-BOOTDESIGN-0003 – Adaptives Glass-Design
- ADR-BOOTGFX-0010 – Hintergrundunschärfe für Glass-Dialoge
- ADR-BOOTGFX-0011 – Qualitätsstufen für grafische Effekte
- ADR-BOOTPERF-0004 – Automatische Effektdegradation

## NPSPECs

- NPSPEC-BOOTCOMPOSITOR-0005 – Backdrop Processing
- NPSPEC-BOOTCOMPOSITOR-0006 – Glass Material
- NPSPEC-BOOTRENDER-0013 – Gradients
- NPSPEC-BOOTRENDER-0014 – Rounded Geometry
- NPSPEC-BOOTRENDER-0015 – Shadows and Glows
- NPSPEC-BOOTRENDER-0016 – Background Blur
- NPSPEC-BOOTRENDER-0018 – Rendering Quality Profiles

---

# 32. Zusammenfassung

Das **Acrylic Material** definiert ein hochwertiges, halbtransparentes Oberflächenmaterial für temporäre Benutzeroberflächen des NovaOS Bootmanagers. Es kombiniert Background Blur, Tint, Materialrauschen (Noise), Gradienten, Schatten und dezente Glanzeffekte zu einer realistisch wirkenden Acrylglasoptik. Durch adaptive Qualitätsprofile, deterministische Noise-Erzeugung und vollständige Integration in das Compositing-System entsteht eine performante und konsistente Darstellung auf BIOS- und UEFI-Systemen. :contentReference[oaicite:1]{index=1}