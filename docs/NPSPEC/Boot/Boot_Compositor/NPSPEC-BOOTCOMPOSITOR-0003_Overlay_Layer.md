# NPSPEC-BOOTCOMPOSITOR-0003
# Overlay Layer

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-BOOTCOMPOSITOR-0003 |
| Titel | Overlay Layer |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Bootmanager / Compositor |
| Priorität | Hoch |
| Abhängigkeiten | NPSPEC-BOOTCOMPOSITOR-0001, NPSPEC-BOOTCOMPOSITOR-0002, NPSPEC-BOOTRENDER-0012, NPSPEC-BOOTRENDER-0016 |
| Zugehörige ADRs | ADR-BOOTGFX-0004, ADR-BOOTDIALOG-0002, ADR-BOOTDIALOG-0003, ADR-BOOTPERF-0002 |

---

# 1. Ziel

Diese Spezifikation definiert den **Overlay Layer** des NovaOS Boot Compositors.

Der Overlay Layer stellt grafische Inhalte dar, die temporär über der normalen Benutzeroberfläche angezeigt werden, ohne diese dauerhaft zu verändern.

Typische Anwendungsfälle sind:

- Modale Dialoge
- Sicherheitsabfragen
- Recovery-Fenster
- Benachrichtigungen
- Ladeanzeigen
- Sperrbildschirme
- Debug-Overlays
- Bildschirmeffekte

Der Overlay Layer bildet die Grundlage für das Glass-Design des Bootmanagers.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für sämtliche temporären grafischen Ebenen oberhalb der Standardoberfläche.

Nicht Bestandteil sind:

- Desktop Layer
- Wallpaper Layer
- Cursor Layer
- Rendering Engine

---

# 3. Architektur

```text
Desktop Layer
        │
        ▼
Dialog Layer
        │
        ▼
Overlay Layer
        │
        ▼
Cursor Layer
        │
        ▼
Framebuffer
```

---

# 4. Grundprinzip

Overlay Layer besitzen immer eine höhere Priorität als normale UI-Layer.

Sie bleiben vollständig unabhängig von der darunterliegenden Oberfläche.

Beim Entfernen wird der ursprüngliche Bildinhalt automatisch wieder sichtbar.

---

# 5. Aufgaben

Der Overlay Layer übernimmt:

- Darstellung modaler Inhalte
- Verdunklung des Hintergrunds
- Glass-Effekte
- Blur
- Fokusbindung
- Animationen
- Priorisierung
- Eingabeabschirmung

---

# 6. Layerstruktur

```c
typedef struct
{
    uint32_t id;

    bool visible;

    float opacity;

    nova_surface_t* surface;

    bool modal;

    bool blocks_input;
} nova_overlay_layer_t;
```

---

# 7. Z-Reihenfolge

Standard:

| Ebene | Z |
|---|---:|
| Desktop | 100 |
| Dialog | 300 |
| Overlay | 500 |
| Cursor | 900 |
| Debug | 1000 |

Overlay Layer liegen grundsätzlich oberhalb aller normalen Dialoge.

---

# 8. Sichtbarkeit

Ein Overlay wird dargestellt wenn:

- sichtbar
- gültige Surface vorhanden
- Deckkraft größer als Null

Unsichtbare Overlays werden vollständig übersprungen.

---

# 9. Hintergrundverdunklung

Ein Overlay darf den Hintergrund abdunkeln.

Eigenschaften:

- einstellbare Deckkraft
- vollständige Bildschirmgröße
- Alpha-Compositing
- optional animiert

Die Verdunklung gehört zum Overlay Layer und nicht zum Dialog.

---

# 10. Background Blur

Glass-Overlays dürfen einen Hintergrund-Blur verwenden.

Der Blur wird ausschließlich auf darunterliegenden Layern berechnet.

Das Overlay selbst wird niemals verwischt.

---

# 11. Transparenz

Overlay Layer unterstützen:

- globale Deckkraft
- Alpha-Kanal
- Premultiplied Alpha

Die Deckkraft wirkt auf den gesamten Layer.

---

# 12. Glass Design

Glass-Overlays bestehen typischerweise aus:

- Background Blur
- halbtransparenter Fläche
- Gradient
- Schatten
- Rahmen
- Glow

Alle Effekte werden vor dem finalen Compositing berechnet.

---

# 13. Modale Overlays

Ein modales Overlay blockiert sämtliche Eingaben an darunterliegende Ebenen.

Nur Elemente innerhalb des aktiven Overlays dürfen Eingaben empfangen.

---

# 14. Nicht-modale Overlays

Nicht-modale Overlays dürfen Eingaben transparent weiterleiten.

Beispiele:

- FPS-Anzeige
- Debug-Overlay
- Statusinformationen

---

# 15. Mehrere Overlays

Es dürfen mehrere Overlays gleichzeitig existieren.

Die Darstellung erfolgt nach Z-Reihenfolge.

Das oberste modale Overlay besitzt stets den Eingabefokus.

---

# 16. Eingabeverarbeitung

Der Overlay Layer arbeitet mit dem Input Manager zusammen.

Bei aktivem modalem Overlay gilt:

```text
Eingabe

↓

Overlay

↓

Dialog

↓

Desktop
```

---

# 17. Dirty Regions

Verändert sich ein Overlay, wird ausschließlich dessen Begrenzungsbereich invalidiert.

Ein vollständiges Re-Rendering ist nicht erforderlich.

---

# 18. Animationen

Unterstützt werden:

- Fade-In
- Fade-Out
- Scale-In
- Scale-Out
- Slide-In
- Slide-Out

Die Animationen werden durch den Motion Scheduler gesteuert.

---

# 19. Clipping

Jedes Overlay besitzt einen eigenen Clippingbereich.

Pixel außerhalb dieses Bereichs werden verworfen.

---

# 20. Transformation

Overlay Layer unterstützen:

- Translation
- Skalierung
- Rotation

Transformationen wirken auf den gesamten Layer.

---

# 21. Speicherverwaltung

Für jeden Overlay Layer existiert genau eine Surface.

Temporäre Effektflächen werden vom Rendering-System verwaltet.

Heap-Allokationen während des Render-Hotpaths sind unzulässig.

---

# 22. Performance

Optimierungen:

- Dirty Regions
- Surface Cache
- Blur Cache
- frühes Verwerfen unsichtbarer Layer

Das Aktivieren eines Overlays darf den Bootprozess nicht merklich verzögern.

---

# 23. Fehlerbehandlung

Folgende Fehler müssen erkannt werden:

- fehlende Surface
- ungültige Deckkraft
- beschädigte Blur Surface
- ungültiger Z-Index
- doppelte Overlay-ID

Fehler dürfen den Bootvorgang nicht blockieren.

---

# 24. Sicherheit

Das System verhindert:

- doppelte modale Overlays mit identischer Priorität
- ungültige Layerreferenzen
- Nullpointer
- Out-of-Bounds-Zugriffe
- rekursive Overlay-Beziehungen

---

# 25. API

```c
nova_result_t
nova_overlay_create(
    nova_overlay_layer_t* overlay);

nova_result_t
nova_overlay_destroy(
    nova_overlay_layer_t* overlay);

nova_result_t
nova_overlay_show(
    nova_overlay_layer_t* overlay);

nova_result_t
nova_overlay_hide(
    nova_overlay_layer_t* overlay);

nova_result_t
nova_overlay_set_modal(
    nova_overlay_layer_t* overlay,
    bool modal);
```

---

# 26. Diagnosedaten

Zu erfassen sind:

- Anzahl Overlays
- modale Overlays
- nicht-modale Overlays
- Blur-Flächen
- Renderzeit
- Speicherverbrauch
- Animationen
- Dirty Regions

---

# 27. Testfälle

Zu testen sind:

- Overlay anzeigen
- Overlay ausblenden
- mehrere Overlays
- modale Overlays
- Hintergrundverdunklung
- Blur
- Transparenz
- Animationen
- Dirty Regions
- Fokusbindung

---

# 28. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- Overlays unabhängig von der Hauptoberfläche dargestellt werden.
- modale Overlays sämtliche Eingaben blockieren.
- Background Blur ausschließlich darunterliegende Layer verarbeitet.
- mehrere Overlays korrekt sortiert werden.
- Dirty Regions vollständig unterstützt werden.
- keine Heap-Allokationen im Render-Hotpath stattfinden.
- BIOS- und UEFI-Systeme identische Ergebnisse liefern.

---

# 29. Referenzen

## ADRs

- ADR-BOOTGFX-0004 – Ebenenbasiertes Compositing
- ADR-BOOTDIALOG-0002 – Dialoge als eigene Compositor-Ebene
- ADR-BOOTDIALOG-0003 – Glass-Design mit adaptivem Fallback
- ADR-BOOTPERF-0002 – Keine Speicherallokation im Render-Hotpath

## NPSPECs

- NPSPEC-BOOTCOMPOSITOR-0001 – Boot Compositor
- NPSPEC-BOOTCOMPOSITOR-0002 – Compositor Layer Model
- NPSPEC-BOOTRENDER-0012 – Alpha Compositing
- NPSPEC-BOOTRENDER-0015 – Shadows and Glows
- NPSPEC-BOOTRENDER-0016 – Background Blur

---

# 30. Zusammenfassung

Der **Overlay Layer** bildet die Grundlage für sämtliche temporären und hervorgehobenen Inhalte des NovaOS Bootmanagers. Durch die Kombination aus eigener Compositor-Ebene, Background Blur, Glass-Design, Hintergrundverdunklung und modaler Eingabesteuerung entstehen hochwertige und klar strukturierte Benutzeroberflächen mit konsistentem Verhalten auf BIOS- und UEFI-Systemen.