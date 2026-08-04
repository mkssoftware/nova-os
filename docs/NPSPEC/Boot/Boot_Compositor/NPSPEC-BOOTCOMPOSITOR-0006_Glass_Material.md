# NPSPEC-BOOTCOMPOSITOR-0006
# Glass Material

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-BOOTCOMPOSITOR-0006 |
| Titel | Glass Material |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Bootmanager / Compositor |
| Priorität | Kritisch |
| Abhängigkeiten | NPSPEC-BOOTCOMPOSITOR-0001 bis NPSPEC-BOOTCOMPOSITOR-0005, NPSPEC-BOOTRENDER-0016 |
| Zugehörige ADRs | ADR-BOOTDESIGN-0003, ADR-BOOTGFX-0010, ADR-BOOTGFX-0011, ADR-BOOTGFX-0002, ADR-BOOTPERF-0004 |

---

# 1. Ziel

Diese Spezifikation definiert das **Glass Material** der NovaOS Boot UI.

Glass Material ist ein zusammengesetztes Oberflächenmaterial, das mehrere Rendering-Effekte kombiniert und eine moderne, hochwertige Benutzeroberfläche erzeugt.

Das Material besteht aus:

- Background Blur
- Transparenz
- Hintergrundtönung
- Gradienten
- Rahmen
- Schatten
- Glows
- Lichtreflexen
- Materialrauschen
- Farbkorrektur

Das Erscheinungsbild orientiert sich an modernen Materialsystemen, bleibt jedoch vollständig unabhängig von Betriebssystemen oder Frameworks. Moderne UI-Materialien kombinieren Hintergrundinformationen, Transparenz und visuelle Tiefe, wobei bei fehlender Hardwareunterstützung auf einfachere Darstellungen zurückgefallen wird. :contentReference[oaicite:0]{index=0}

---

# 2. Geltungsbereich

Glass Material wird verwendet für:

- Dialoge
- Panels
- Karten
- Menüs
- Popups
- Overlay-Fenster
- Benachrichtigungen
- Eingabefelder

Nicht Bestandteil sind:

- Hintergrundbilder
- Icons
- Text
- Mauszeiger

---

# 3. Architektur

```text
Backdrop

↓

Background Blur

↓

Tint

↓

Gradient

↓

Glass Surface

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

Glass Material besteht niemals aus einer einzelnen Farbe.

Jede Glass-Oberfläche wird dynamisch aus dem aktuellen Hintergrund erzeugt.

Das Material passt sich automatisch an:

- Hintergrund
- Design Theme
- Qualitätsprofil
- Transparenz
- Kontrast

an.

---

# 5. Materialaufbau

Ein Glass Material besteht aus:

- Backdrop
- Blur
- Tint
- Surface Color
- Border
- Highlight
- Shadow
- Glow

Jede Komponente kann unabhängig aktiviert oder deaktiviert werden.

---

# 6. Materialbeschreibung

```c
typedef struct
{
    float opacity;

    float blur_radius;

    float border_opacity;

    float shadow_opacity;

    float glow_opacity;

    nova_color_t tint;

} nova_glass_material_t;
```

---

# 7. Background Blur

Blur bildet die Grundlage jeder Glass-Oberfläche.

Der Blur verarbeitet ausschließlich Hintergrundlayer.

Die Blur-Implementierung ist in NPSPEC-BOOTRENDER-0016 definiert.

---

# 8. Transparenz

Glass besitzt grundsätzlich eine Teiltransparenz.

Empfohlene Werte:

| Element | Deckkraft |
|---|---:|
| Dialog | 82 % |
| Menü | 78 % |
| Panel | 85 % |
| Popup | 80 % |

Alle Werte stammen aus den Design Tokens.

---

# 9. Hintergrundtönung

Nach dem Blur wird eine Farbtönung angewendet.

Standardfarben:

- Neutral Hell
- Neutral Dunkel
- Akzentfarbe

Die Intensität bleibt gering, damit der Hintergrund weiterhin erkennbar bleibt.

---

# 10. Gradienten

Glass verwendet standardmäßig einen vertikalen Verlauf.

```text
Hell

↓

Neutral

↓

Etwas dunkler
```

Dadurch entsteht räumliche Tiefe.

---

# 11. Rahmen

Jede Glass-Fläche besitzt einen feinen Innen- oder Außenrahmen.

Eigenschaften:

- geringe Deckkraft
- heller als die Oberfläche
- Anti-Aliasing

Der Rahmen verbessert die Abgrenzung gegenüber dem Hintergrund.

---

# 12. Lichtreflex

Optional besitzt das Material eine obere Lichtkante.

Eigenschaften:

- geringe Deckkraft
- Verlauf
- abhängig vom Theme

Sie verstärkt den Eindruck einer Glasoberfläche.

---

# 13. Schatten

Glass verwendet standardmäßig:

- Ambient Shadow
- Drop Shadow

Die Schattenparameter stammen aus NPSPEC-BOOTRENDER-0015.

---

# 14. Glow

Interaktive Elemente dürfen einen dezenten Glow besitzen.

Der Glow wird ausschließlich bei:

- Hover
- Fokus
- Auswahl

verwendet.

---

# 15. Rounded Geometry

Glass Material verwendet ausschließlich Rounded Geometry.

Die Radien werden über die Nova Design Tokens definiert.

---

# 16. Clipping

Glass wird vollständig gegen die Objektgeometrie geclippt.

Blur, Gradient und Tint dürfen den Clippingbereich nicht verlassen.

---

# 17. Animationen

Folgende Eigenschaften dürfen animiert werden:

- Transparenz
- Blur Radius
- Tint
- Glow
- Shadow
- Deckkraft

Alle Animationen erfolgen über den Motion Scheduler.

---

# 18. Qualitätsprofile

Je nach Qualitätsprofil dürfen reduziert werden:

- Blur Radius
- Glow
- Shadow
- Gradientqualität
- Highlight

Das Material muss weiterhin eindeutig als Glass erkennbar bleiben.

---

# 19. Safe Mode

Im Safe-Profil besteht Glass ausschließlich aus:

- Volltonfarbe
- Rahmen

Blur, Glow und Schatten werden deaktiviert.

---

# 20. Materialvarianten

Unterstützt werden:

- Thin Glass
- Standard Glass
- Thick Glass
- Accent Glass

Weitere Varianten können ergänzt werden.

---

# 21. Materialstatus

Glass kann unterschiedliche Zustände besitzen:

- Normal
- Hover
- Focus
- Pressed
- Disabled

Jeder Zustand besitzt eigene Materialparameter.

---

# 22. Farbverwaltung

Alle Farben stammen aus dem Nova Design Token System.

Direkte Farbwerte im Quellcode sind unzulässig.

---

# 23. Speicherverwaltung

Glass verwendet:

- Blur Surface
- Backdrop Surface
- temporäre Effektflächen

Alle Ressourcen werden beim Start reserviert.

---

# 24. Performance

Optimierungen:

- Blur Cache
- Dirty Regions
- SIMD
- Surface Cache
- frühes Clipping

Heap-Allokationen während des Renderings sind unzulässig.

---

# 25. Fehlerbehandlung

Folgende Fehler müssen erkannt werden:

- fehlende Backdrop Surface
- ungültiger Blur Radius
- ungültige Tintfarbe
- beschädigte Effektfläche
- Speicherfehler

Bei Fehlern wird automatisch auf eine einfache, halbtransparente Fläche zurückgegriffen.

---

# 26. Sicherheit

Das System verhindert:

- rekursive Blur-Effekte
- doppelte Materialinstanzen
- ungültige Farbwerte
- Out-of-Bounds-Zugriffe
- Nullpointer

Alle Materialparameter müssen validiert werden.

---

# 27. API

```c
nova_result_t
nova_glass_initialize(void);

nova_result_t
nova_glass_create(
    nova_glass_material_t* material);

nova_result_t
nova_glass_render(
    const nova_glass_material_t* material,
    nova_surface_t* destination);

nova_result_t
nova_glass_destroy(
    nova_glass_material_t* material);
```

---

# 28. Diagnosedaten

Zu erfassen sind:

- Anzahl Glass-Flächen
- Blur Radius
- Tintfarbe
- Qualitätsprofil
- Cache Hits
- Cache Misses
- Renderzeit
- Speicherverbrauch

---

# 29. Testfälle

Zu testen sind:

- Standard Glass
- Thin Glass
- Thick Glass
- Accent Glass
- Blur deaktiviert
- Qualitätswechsel
- Themewechsel
- Animationen
- Dirty Regions
- Safe Mode

---

# 30. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- Glass ausschließlich aus definierten Materialkomponenten aufgebaut wird.
- Blur ausschließlich Hintergrundinformationen verarbeitet.
- sämtliche Materialparameter über Design Tokens gesteuert werden.
- Qualitätsprofile korrekt unterstützt werden.
- Safe Mode ohne Blur funktionsfähig bleibt.
- keine Heap-Allokationen im Render-Hotpath stattfinden.
- BIOS- und UEFI-Systeme identische Ergebnisse liefern.

---

# 31. Referenzen

## ADRs

- ADR-BOOTDESIGN-0003 – Adaptives Glass-Design
- ADR-BOOTGFX-0010 – Hintergrundunschärfe für Glass-Dialoge
- ADR-BOOTGFX-0011 – Qualitätsstufen für grafische Effekte
- ADR-BOOTGFX-0002 – Alpha-Compositing im Bootmanager
- ADR-BOOTPERF-0004 – Automatische Effektdegradation

## NPSPECs

- NPSPEC-BOOTCOMPOSITOR-0005 – Backdrop Processing
- NPSPEC-BOOTRENDER-0012 – Alpha Compositing
- NPSPEC-BOOTRENDER-0013 – Gradients
- NPSPEC-BOOTRENDER-0014 – Rounded Geometry
- NPSPEC-BOOTRENDER-0015 – Shadows and Glows
- NPSPEC-BOOTRENDER-0016 – Background Blur
- NPSPEC-BOOTRENDER-0018 – Rendering Quality Profiles

---

# 32. Zusammenfassung

Das **Glass Material** definiert das zentrale Oberflächenmaterial der NovaOS Bootoberfläche. Es kombiniert Background Blur, Transparenz, Tönung, Gradienten, Schatten, Glow und hochwertige Rahmen zu einem einheitlichen Materialsystem. Durch Design Tokens, adaptive Qualitätsprofile und deterministische Verarbeitung entsteht ein konsistentes Erscheinungsbild, das auf BIOS- und UEFI-Systemen identisch funktioniert und bei eingeschränkten Fähigkeiten kontrolliert auf einfachere Darstellungen zurückfällt. :contentReference[oaicite:1]{index=1}