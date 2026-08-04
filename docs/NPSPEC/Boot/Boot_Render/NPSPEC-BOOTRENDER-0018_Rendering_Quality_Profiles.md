# NPSPEC-BOOTRENDER-0018
# Rendering Quality Profiles

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-BOOTRENDER-0018 |
| Titel | Rendering Quality Profiles |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Bootmanager / Rendering |
| Priorität | Hoch |
| Abhängigkeiten | NPSPEC-BOOTRENDER-0001 bis NPSPEC-BOOTRENDER-0017 |
| Zugehörige ADRs | ADR-BOOTGFX-0011, ADR-BOOTPERF-0003, ADR-BOOTPERF-0004, ADR-BOOTSAFE-0002 |

---

# 1. Ziel

Diese Spezifikation definiert die **Rendering Quality Profiles** der NovaOS Boot Rendering Engine.

Das System ermöglicht die automatische oder manuelle Anpassung der grafischen Qualität an die verfügbare Hardwareleistung, ohne das Erscheinungsbild der Benutzeroberfläche grundlegend zu verändern.

Die Qualitätsprofile steuern unter anderem:

- Anti-Aliasing
- Blur
- Schatten
- Glow
- Gradienten
- Animationen
- Bildskalierung
- Transparenz
- Renderauflösung
- Effektdichte

---

# 2. Geltungsbereich

Diese Spezifikation gilt für sämtliche Rendering-Komponenten des Bootmanagers.

Sie beeinflusst:

- Rendering Engine
- Compositor
- Blur Engine
- Shadow Renderer
- Image Renderer
- Animation System
- Present Scheduler

Nicht Bestandteil sind:

- Layout
- Eingabeverarbeitung
- Bootlogik

---

# 3. Architektur

```text
Hardware Analyse
        │
        ▼
Quality Manager
        │
        ▼
Quality Profile
        │
        ▼
Rendering Engine
        │
        ▼
Framebuffer
```

---

# 4. Qualitätsprofile

Die Engine unterstützt folgende Profile:

| Profil | Beschreibung |
|---|---|
| Ultra | Maximale Qualität |
| High | Hohe Qualität |
| Standard | Standardprofil |
| Performance | Reduzierte Effekte |
| Safe | Minimale Grafikfunktionen |

Standardmäßig wird **Standard** verwendet.

---

# 5. Profilbeschreibung

```c
typedef enum
{
    NOVA_RENDER_QUALITY_ULTRA,
    NOVA_RENDER_QUALITY_HIGH,
    NOVA_RENDER_QUALITY_STANDARD,
    NOVA_RENDER_QUALITY_PERFORMANCE,
    NOVA_RENDER_QUALITY_SAFE
} nova_render_quality_t;
```

---

# 6. Auswahl

Die Profilauswahl erfolgt:

1. Benutzerkonfiguration
2. Hardwareerkennung
3. Firmwareeinschränkungen
4. Safe Mode

Kann kein geeignetes Profil bestimmt werden, wird **Standard** verwendet.

---

# 7. Ultra

Eigenschaften:

- maximale Blur-Radien
- höchste Schattenqualität
- vollständiges Anti-Aliasing
- hochwertige Bildskalierung
- alle Animationen
- maximale Effektanzahl

---

# 8. High

Eigenschaften:

- hochwertige Blur-Effekte
- vollständige Schatten
- bilineare Skalierung
- vollständige Transparenz
- alle UI-Effekte

---

# 9. Standard

Eigenschaften:

- mittlere Blur-Radien
- Standard-Schatten
- bilineare Skalierung
- reduzierte Effektqualität
- vollständige Glass-Oberflächen

Dieses Profil ist für die meisten Systeme vorgesehen.

---

# 10. Performance

Eigenschaften:

- kleinere Blur-Radien
- vereinfachte Schatten
- reduzierte Glow-Effekte
- weniger Animationen
- vereinfachte Bildfilter

Die Benutzeroberfläche bleibt funktional identisch.

---

# 11. Safe

Eigenschaften:

- kein Blur
- keine Schatten
- keine Glows
- keine Transparenz
- keine Animationen
- minimale Bildverarbeitung

Dieses Profil dient ausschließlich der maximalen Stabilität.

---

# 12. Qualitätsparameter

Ein Profil steuert mindestens:

- Blur-Radius
- Shadow-Qualität
- Glow-Qualität
- Anti-Aliasing
- Skalierungsfilter
- Animationen
- Dithering
- Effektdichte

---

# 13. Blur

| Profil | Blur |
|---|---|
| Ultra | Maximal |
| High | Hoch |
| Standard | Mittel |
| Performance | Niedrig |
| Safe | Deaktiviert |

---

# 14. Schatten

| Profil | Schatten |
|---|---|
| Ultra | Mehrere Ebenen |
| High | Weich |
| Standard | Standard |
| Performance | Vereinfacht |
| Safe | Deaktiviert |

---

# 15. Glows

Glows werden abhängig vom Profil reduziert.

Safe deaktiviert sämtliche Glow-Effekte.

---

# 16. Anti-Aliasing

Unterstützte Stufen:

- Aus
- Standard
- Hoch

Die Zuordnung erfolgt automatisch anhand des Profils.

---

# 17. Bildskalierung

Unterstützte Filter:

- Nearest Neighbor
- Bilinear

Ultra, High und Standard verwenden standardmäßig Bilinear.

---

# 18. Animationen

Folgende Eigenschaften können reduziert werden:

- Dauer
- Anzahl
- Blur-Animationen
- Schattenanimationen

Im Safe-Profil sind Animationen deaktiviert.

---

# 19. Glass Design

Glass-Effekte werden abhängig vom Profil angepasst.

Bei niedrigen Profilen werden Radius und Transparenz reduziert.

---

# 20. Speicherbudget

Jedes Profil besitzt ein eigenes Speicherbudget.

Dieses bestimmt:

- Anzahl Offscreen-Surfaces
- Blur-Puffer
- Cachegröße
- Effektpuffer

---

# 21. Dynamische Anpassung

Die Engine darf das Profil während der Laufzeit anpassen, wenn:

- Speichermangel erkannt wird
- Renderzeiten dauerhaft überschritten werden
- Fehler im Grafiksystem auftreten

Die Anpassung muss ohne sichtbare Darstellungsfehler erfolgen.

---

# 22. Qualitätswechsel

Ein Wechsel des Profils erfordert:

- Aktualisierung der Rendering-Parameter
- Leeren temporärer Effekt-Caches
- vollständiges Re-Rendering des Bildschirms

---

# 23. Benutzerkonfiguration

Das gewünschte Profil kann in der Bootkonfiguration gespeichert werden.

Mögliche Werte:

- Auto
- Ultra
- High
- Standard
- Performance
- Safe

---

# 24. Performance

Der Wechsel eines Profils darf den Bootprozess nur minimal verzögern.

Qualitätsentscheidungen erfolgen ausschließlich außerhalb des Render-Hotpaths.

---

# 25. Fehlerbehandlung

Kann ein Profil nicht vollständig verwendet werden:

- werden einzelne Effekte deaktiviert,
- das Profil bleibt jedoch aktiv.

Nur bei kritischen Fehlern erfolgt der Wechsel auf **Safe**.

---

# 26. Sicherheit

Das System verhindert:

- ungültige Profilwerte
- inkonsistente Renderingparameter
- negative Qualitätsstufen
- ungültige Speicherbudgets

Alle Profilparameter müssen validiert werden.

---

# 27. API

```c
void
nova_render_quality_initialize(void);

void
nova_render_quality_set(
    nova_render_quality_t profile);

nova_render_quality_t
nova_render_quality_get(void);

bool
nova_render_quality_is_supported(
    nova_render_quality_t profile);

void
nova_render_quality_apply(void);
```

---

# 28. Diagnosedaten

Zu erfassen sind:

- aktives Profil
- Profilwechsel
- Renderzeit
- Speicherverbrauch
- deaktivierte Effekte
- Blur-Radius
- Schattenqualität
- Qualitätsanpassungen

---

# 29. Testfälle

Zu testen sind:

- Ultra
- High
- Standard
- Performance
- Safe
- Profilwechsel
- Speichermangel
- Hardwareeinschränkungen
- vollständiges Re-Rendering
- Fehlerfall

---

# 30. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- alle Qualitätsprofile verfügbar sind.
- die Benutzeroberfläche in jedem Profil vollständig bedienbar bleibt.
- Effekte kontrolliert reduziert werden.
- dynamische Profilwechsel korrekt funktionieren.
- keine Heap-Allokationen im Render-Hotpath stattfinden.
- BIOS- und UEFI-Systeme identisches Verhalten zeigen.

---

# 31. Referenzen

## ADRs

- ADR-BOOTGFX-0011 – Qualitätsstufen für grafische Effekte
- ADR-BOOTPERF-0003 – Adaptive Bildwiederholrate
- ADR-BOOTPERF-0004 – Automatische Effektdegradation
- ADR-BOOTSAFE-0002 – UI-Fehler dürfen den Bootprozess nicht blockieren

## NPSPECs

- NPSPEC-BOOTRENDER-0015 – Shadows and Glows
- NPSPEC-BOOTRENDER-0016 – Background Blur
- NPSPEC-BOOTRENDER-0017 – Image Rendering
- NPSPEC-BOOTUI-0009 – Boot UI Configuration

---

# 32. Zusammenfassung

Die **Rendering Quality Profiles** ermöglichen eine adaptive Steuerung der grafischen Qualität des NovaOS Bootmanagers. Durch klar definierte Qualitätsstufen, dynamische Anpassung an Hardware- und Speicherbedingungen sowie kontrollierte Deaktivierung einzelner Effekte bleibt die Benutzeroberfläche auf allen unterstützten Systemen konsistent, performant und stabil.