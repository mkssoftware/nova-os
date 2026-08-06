# NPSPEC-BOOTCONTROL-IMAGE-0001
# Image

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTCONTROL-IMAGE-0001 |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot UI Controls |
| Priorität | Hoch |
| Gültigkeitsbereich | NovaOS Boot Manager |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTCONTROL-0001, NPSPEC-BOOTCONTROL-0003, NPSPEC-BOOTCONTROL-0004, NPSPEC-BOOTRENDER-0017, NPSPEC-BOOTRENDER-0018, NPSPEC-BOOTACCESSIBILITY-0001 |

---

# 1. Ziel

Diese Spezifikation definiert das **Image-Control** der NovaOS Boot UI.

Das Image-Control dient der Darstellung statischer oder animierter Bilder innerhalb der Bootoberfläche. Es stellt Logos, Illustrationen, Hintergrundelemente und andere grafische Inhalte unabhängig vom eigentlichen Rendering-System bereit.

Typische Einsatzbereiche:

- NovaOS-Logo
- Bootlogos
- Herstellerlogos
- Recovery-Illustrationen
- Hintergrundgrafiken
- Statusbilder
- Diagnosedarstellungen
- QR-Codes

---

# 2. Geltungsbereich

Diese Spezifikation gilt für sämtliche Bilddarstellungen.

Sie umfasst:

- Bitmap-Bilder
- Vektorbilder
- Logos
- Transparente Bilder
- Mehrschichtige Bilder
- Animierte Bilder (optional)

Nicht Bestandteil sind:

- Video
- 3D-Modelle
- Schriftarten
- Icons (siehe NPSPEC-BOOTCONTROL-ICON-0001)

---

# 3. Architektur

```text
Boot Control Framework

↓

Image Control

↓

Image Manager

↓

Image Renderer

↓

Framebuffer
```

---

# 4. Grundprinzip

Ein Image-Control stellt genau ein Bildobjekt dar.

Das Bild besitzt keine eigene Logik und dient ausschließlich der visuellen Darstellung.

Interaktive Funktionen werden ausschließlich durch übergeordnete Controls bereitgestellt.

---

# 5. Aufgaben

Das Image-Control übernimmt:

- Bilddarstellung
- Skalierung
- Zuschneiden
- Transparenz
- Themeintegration
- Animation (optional)
- Accessibility
- Ressourcenverwaltung

---

# 6. Aufbau

```text
Image

├── Image Resource

├── Background (optional)

├── Overlay (optional)

└── Focus Indicator (optional)
```

---

# 7. Eigenschaften

Jedes Image besitzt:

- ID
- Bildquelle
- Größe
- Position
- Sichtbarkeit
- Style
- Template

Optional:

- Beschreibung
- Transparenz
- Tint-Farbe
- Overlay

---

# 8. Unterstützte Bildformate

Unterstützt werden:

### Raster

- PNG
- BMP
- QOI

### Vektor

- SVG

Optional:

- JPEG (nur für Fotos)
- APNG (Animation)

Nicht unterstützt:

- GIF
- TIFF
- WebP
- AVIF

---

# 9. Skalierungsmodi

Unterstützt werden:

```text
Original

↓

Stretch

↓

Fit

↓

Fill

↓

Center
```

Die Skalierung erfolgt DPI-unabhängig.

---

# 10. Zuschneiden

Unterstützt werden:

- kein Zuschnitt
- automatischer Zuschnitt
- definierter Ausschnitt

Der Zuschnitt verändert niemals die Originaldaten.

---

# 11. Transparenz

Unterstützt werden:

- vollständig deckend
- Alpha-Kanal
- Theme-Transparenz

Premultiplied Alpha wird bevorzugt.

---

# 12. Darstellung

Das Image-Control unterstützt:

- Transparenz
- Schatten
- Glow
- abgerundete Ecken
- Farbüberlagerungen

Alle Darstellungseigenschaften stammen aus Styles und Templates.

---

# 13. Animationen

Optional unterstützt:

- Fade
- Crossfade
- Zoom
- Rotation
- Alpha-Animation

Animationen werden durch das Motion-System gesteuert.

---

# 14. Themeintegration

Das Theme definiert:

- Hintergrund
- Transparenz
- Tint-Farben
- Schatten
- Glows
- Materialeffekte

Logos dürfen optional automatisch an Hell-/Dunkelmodus angepasst werden.

---

# 15. Accessibility

Jedes Image besitzt:

- Name
- Beschreibung
- semantische Rolle

Dekorative Bilder werden von Screenreadern ignoriert.

Informationsrelevante Bilder müssen eine Beschreibung besitzen.

---

# 16. Ressourcenverwaltung

Bilder werden:

- zentral geladen
- zwischengespeichert
- mehrfach referenziert
- gemeinsam genutzt

Mehrfachkopien derselben Ressource sind zu vermeiden.

---

# 17. Speicherverwaltung

Das Image-Control verwendet:

- Bildcache
- Objektpools
- zentrale Ressourcenverwaltung

Große Bilder dürfen bei Bedarf komprimiert im Speicher gehalten werden.

Heap-Allokationen während des Renderings sind verboten.

---

# 18. Performance

Anforderungen:

- O(1)-Bildzugriff
- O(1)-Skalierung
- O(1)-Ressourcenwechsel
- deterministische Laufzeit

Bereits dekodierte Bilder werden wiederverwendet.

---

# 19. Fehlerbehandlung

Folgende Fehler müssen erkannt werden:

- fehlende Bilddatei
- beschädigte Bilddaten
- ungültiges Format
- fehlende Ressourcen
- Nullpointer

Kann ein Bild nicht geladen werden, wird ein Standard-Platzhalterbild dargestellt.

---

# 20. Sicherheit

Das System verhindert:

- beschädigte Bilddaten
- fehlerhafte Alpha-Kanäle
- ungültige Bildgrößen
- Speicherüberläufe
- rekursive Ressourcenabhängigkeiten

Alle Bilddaten werden vor dem Rendern validiert.

---

# 21. API

```c
typedef struct nova_image_control nova_image_control_t;

nova_result_t
nova_image_create(
    nova_image_resource_t* resource,
    nova_image_control_t** image);

nova_result_t
nova_image_set_resource(
    nova_image_control_t* image,
    nova_image_resource_t* resource);

nova_result_t
nova_image_set_scaling(
    nova_image_control_t* image,
    nova_image_scaling_mode_t mode);

nova_result_t
nova_image_set_tint(
    nova_image_control_t* image,
    nova_color_t color);

nova_image_resource_t*
nova_image_get_resource(
    const nova_image_control_t* image);
```

---

# 22. Diagnosedaten

Zu erfassen sind:

- Image-ID
- Ressourcen-ID
- Bildformat
- Bildgröße
- Skalierungsmodus
- Renderzeit
- Cache-Treffer
- Speicherverbrauch
- Fehleranzahl

---

# 23. Testfälle

Zu testen sind:

- PNG
- BMP
- SVG
- QOI
- Alpha-Kanal
- Skalierung
- Zuschneiden
- Themewechsel
- High-DPI
- Accessibility
- Fehlerfälle

---

# 24. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- alle unterstützten Bildformate korrekt dargestellt werden.
- Skalierungsmodi DPI-unabhängig arbeiten.
- Transparenz vollständig unterstützt wird.
- Ressourcen zentral verwaltet und wiederverwendet werden.
- Accessibility dekorative und semantische Bilder korrekt unterscheidet.
- keine Heap-Allokationen während des Renderings stattfinden.
- BIOS- und UEFI-Systeme identisches Verhalten erzeugen.

---

# 25. Referenzen

## ADRs

- ADR-BOOTCONTROL-IMAGE-0001 – Image-Control
- ADR-BOOTRENDER-0017 – Image Rendering
- ADR-BOOTRENDER-0018 – Rendering Quality Profiles
- ADR-BOOTTHEME-0001 – Theme-System
- ADR-BOOTACCESS-0001 – Accessibility

## NPSPECs

- NPSPEC-BOOTCONTROL-0001 – Boot Control Framework
- NPSPEC-BOOTCONTROL-0003 – Control Styling
- NPSPEC-BOOTCONTROL-0004 – Control Templates
- NPSPEC-BOOTRENDER-0017 – Image Rendering
- NPSPEC-BOOTRENDER-0018 – Rendering Quality Profiles
- NPSPEC-BOOTACCESSIBILITY-0001 – Accessibility Framework

---

# 26. Zusammenfassung

Das **Image-Control** definiert die standardisierte Darstellung von Bildern innerhalb der NovaOS-Bootoberfläche. Es unterstützt Raster- und Vektorgrafiken, verschiedene Skalierungs- und Transparenzmodi, Theme-Integration sowie vollständige Accessibility. Durch zentrale Ressourcenverwaltung, Bild-Caching, deterministische Verarbeitung und statische Speicherverwaltung entsteht ein leistungsfähiges, konsistentes und erweiterbares Bildsystem für BIOS- und UEFI-basierte Bootumgebungen.