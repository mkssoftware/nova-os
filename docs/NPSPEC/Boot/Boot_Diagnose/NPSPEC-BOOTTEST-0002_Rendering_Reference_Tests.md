# NPSPEC-BOOTTEST-0002
# Rendering Reference Tests

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTTEST-0002 |
| Titel | Rendering Reference Tests |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot Testing |
| Priorität | Kritisch |
| Gültigkeitsbereich | Rendering Engine, Compositor, Boot UI |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTTEST-0001, NPSPEC-BOOTRENDER-0001, NPSPEC-BOOTCOMPOSITOR-0001, NPSPEC-BOOTRESOURCE-0001, NPSPEC-BOOTDESIGN-0001 |

---

# 1. Ziel

Diese Spezifikation definiert die **Rendering Reference Tests** der NovaOS Bootoberfläche.

Rendering Reference Tests überprüfen, ob sämtliche grafischen Elemente der Bootoberfläche auf unterschiedlichen Hardwareplattformen pixelgenau und deterministisch dargestellt werden. Dazu werden erzeugte Renderbilder mit validierten Referenzbildern verglichen.

Die Tests stellen sicher, dass Änderungen an der Rendering Engine keine unbeabsichtigten visuellen Regressionen verursachen.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für:

- Rendering Engine
- Software Renderer
- GPU Renderer
- Boot Compositor
- Text Engine
- Theme Engine
- Motion Engine
- Resource Manager
- sämtliche Boot Controls

---

# 3. Ziele

Die Rendering Reference Tests müssen:

- pixelgenaue Vergleiche ermöglichen
- Rendering-Regressionen erkennen
- deterministische Ergebnisse liefern
- hardwareübergreifend arbeiten
- automatisierbar sein
- reproduzierbar sein

---

# 4. Architektur

Das Testsystem besteht aus:

- Reference Image Manager
- Screenshot Generator
- Image Comparator
- Difference Analyzer
- Tolerance Calculator
- Report Generator

Alle Komponenten arbeiten unabhängig vom eigentlichen Renderer.

---

# 5. Referenzbilder

Für jeden Test existiert mindestens ein Referenzbild.

Ein Referenzbild besitzt:

- Referenz-ID
- Version
- Auflösung
- Theme
- DPI
- Hardwareprofil
- Erstellungsdatum

Referenzbilder werden versioniert verwaltet.

---

# 6. Testablauf

Der Standardablauf:

1. Testumgebung initialisieren
2. Ressourcen laden
3. Oberfläche rendern
4. Screenshot erzeugen
5. Referenzbild laden
6. Vergleich durchführen
7. Bericht erzeugen

---

# 7. Vergleichsverfahren

Unterstützt werden:

- Pixelvergleich
- Alphavergleich
- Farbdifferenz
- Transparenzvergleich
- Ebenenvergleich
- Maskierter Vergleich

---

# 8. Vergleichsbereiche

Verglichen werden:

- Hintergrund
- Controls
- Texte
- Icons
- SVG
- Bilder
- Schatten
- Blur
- Glasmaterialien

---

# 9. Toleranzen

Standardtoleranzen:

| Merkmal | Maximum |
|----------|----------|
| Pixelabweichung | 1 Pixel |
| Farbabweichung | ±1 RGB-Wert |
| Alpha | ±1 |
| Position | ±1 Pixel |

Animationen werden ausschließlich in definierten Referenzzuständen verglichen.

---

# 10. Themen

Alle Tests werden durchgeführt für:

- Dark Theme
- Light Theme
- High Contrast Theme

---

# 11. Auflösungen

Mindestens folgende Auflösungen:

- 800×600
- 1024×768
- 1280×720
- 1920×1080
- 3840×2160

---

# 12. Hardwareprofile

Zu testen sind:

- Software Rendering
- Low-End Profile
- Standardprofil
- High-Quality Profile
- GPU Rendering

---

# 13. Textvergleich

Überprüft werden:

- Glyphposition
- Fontauswahl
- Hinting
- Zeilenumbrüche
- Typografie
- Unicode-Darstellung

---

# 14. SVG-Vergleich

Geprüft werden:

- Geometrie
- Skalierung
- Antialiasing
- Farben
- Transparenz

---

# 15. Animationstests

Animationen werden getestet:

- Startzustand
- Zwischenzustände
- Endzustand

Referenzframes werden einzeln verglichen.

---

# 16. Fehleranalyse

Bei Abweichungen werden erzeugt:

- Differenzbild
- Heatmap
- Pixelstatistik
- Fehlerbeschreibung
- betroffene Bereiche

---

# 17. Automatisierung

Renderingtests müssen:

- vollständig automatisiert
- reproduzierbar
- skriptgesteuert

ausgeführt werden können.

---

# 18. Regression

Jede Änderung an:

- Rendering Engine
- Theme
- Compositor
- Motion Engine
- Resource Manager

löst automatisch Rendering Reference Tests aus.

---

# 19. Sicherheit

Renderingtests dürfen:

- Ressourcen nicht verändern
- Referenzbilder nicht überschreiben
- Bootkonfiguration nicht verändern

Referenzbilder sind schreibgeschützt.

---

# 20. Speicherverwaltung

Das Testsystem verwendet:

- Screenshot Pool
- Comparison Buffer
- Difference Buffer
- Report Buffer

Alle Speicherbereiche werden vor Testbeginn reserviert.

---

# 21. API

```c
typedef struct
{
    uint32_t test_id;
    uint32_t differing_pixels;
    float similarity;
    bool passed;
}
nova_render_reference_result_t;

nova_result_t
nova_render_reference_initialize(void);

nova_result_t
nova_render_reference_execute(
    uint32_t reference_id);

const nova_render_reference_result_t*
nova_render_reference_result(void);

nova_result_t
nova_render_reference_generate_report(void);
```

---

# 22. Diagnosedaten

Zu protokollieren sind:

- Referenz-ID
- Test-ID
- Auflösung
- Theme
- Hardwareprofil
- Pixelabweichungen
- Laufzeit
- Ergebnis
- Differenzbilder

---

# 23. Testfälle

Zu testen sind:

- Boot Manager
- Dialoge
- Recovery
- Installer
- Themes
- SVG Rendering
- Text Rendering
- Glasmaterialien
- Schatten
- Animationen

---

# 24. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- sämtliche Referenzbilder erfolgreich geladen werden.
- Renderbilder deterministisch erzeugt werden.
- Pixelabweichungen innerhalb der definierten Toleranzen liegen.
- Differenzbilder bei Fehlern automatisch erstellt werden.
- alle Themes und Hardwareprofile unterstützt werden.
- Regressionen automatisch erkannt werden.
- Testberichte vollständig erzeugt werden.
- sämtliche Ergebnisse über die Test-API verfügbar sind.

---

# 25. Referenzen

## ADRs

- ADR-BOOTTEST-0002 – Rendering Reference Tests
- ADR-BOOTRENDER-0001 – Rendering Engine
- ADR-BOOTCOMPOSITOR-0001 – Boot Compositor

## NPSPECs

- NPSPEC-BOOTTEST-0001 – Boot UI Test Architecture
- NPSPEC-BOOTRENDER-0001 – Rendering Engine
- NPSPEC-BOOTCOMPOSITOR-0001 – Boot Compositor
- NPSPEC-BOOTRESOURCE-0001 – Boot Resource Manager
- NPSPEC-BOOTDESIGN-0001 – Boot Design Language

---

# 26. Zusammenfassung

Die **Rendering Reference Tests** bilden das zentrale Verfahren zur visuellen Qualitätssicherung der NovaOS-Bootoberfläche. Durch den automatisierten Vergleich gerenderter Ausgaben mit versionierten Referenzbildern werden grafische Regressionen zuverlässig erkannt. Das Testsystem unterstützt verschiedene Themes, Auflösungen und Hardwareprofile, erzeugt bei Abweichungen detaillierte Differenzanalysen und gewährleistet so eine konsistente, reproduzierbare und plattformübergreifend identische Darstellung der Bootoberfläche.