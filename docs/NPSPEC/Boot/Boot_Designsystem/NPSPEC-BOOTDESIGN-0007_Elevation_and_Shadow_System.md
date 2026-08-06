# NPSPEC-BOOTDESIGN-0007
# Elevation and Shadow System

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTDESIGN-0007 |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot UI – Design System |
| Priorität | Hoch |
| Gültigkeitsbereich | NovaOS Boot Manager |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTDESIGN-0001, NPSPEC-BOOTDESIGN-0002, NPSPEC-BOOTRENDER-0014, NPSPEC-BOOTTHEME-0001, NPSPEC-BOOTCONTROL-0003 |

---

# 1. Ziel

Diese Spezifikation definiert das **Elevation and Shadow System** der NovaOS-Bootoberfläche.

Das System beschreibt die räumliche Darstellung sämtlicher UI-Komponenten über definierte Ebenen (Elevation) und Schatten. Dadurch wird eine klare visuelle Hierarchie geschaffen, ohne die Oberfläche durch übermäßige Effekte zu überladen.

Alle Tiefeneffekte werden ausschließlich über semantische Design Tokens definiert.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für:

- Dialoge
- Karten
- Buttons
- Menüs
- Kontextmenüs
- Tooltips
- Fenster
- Overlays
- Popups
- sämtliche UI-Komponenten

---

# 3. Ziele

Das System muss:

- eine eindeutige Tiefenhierarchie schaffen
- konsistente Schatten verwenden
- Themewechsel unterstützen
- deterministisch arbeiten
- GPU- und Software-Rendering unterstützen
- leicht erweiterbar sein

---

# 4. Architektur

```text
Theme

↓

Elevation Tokens

↓

Shadow Tokens

↓

Renderer

↓

UI-Komponenten
```

---

# 5. Komponenten

Das System besteht aus:

- Elevation Registry
- Shadow Registry
- Token Resolver
- Theme Integration
- Shadow Renderer
- Validation Engine

---

# 6. Grundprinzip

Jede Komponente besitzt genau eine Elevation.

```text
Komponente

↓

Elevation Token

↓

Shadow Token

↓

Renderer
```

Die Elevation bestimmt ausschließlich die visuelle Tiefe, nicht die Z-Reihenfolge der Ereignisverarbeitung.

---

# 7. Elevation Tokens

Pflichttokens:

```text
Elevation.None

Elevation.Level1

Elevation.Level2

Elevation.Level3

Elevation.Level4

Elevation.Level5
```

Die konkreten Schattenparameter werden durch das Theme definiert.

---

# 8. Shadow Tokens

Zu jeder Elevation existiert ein Shadow Token.

Beispiele:

```text
Shadow.None

Shadow.Small

Shadow.Medium

Shadow.Large

Shadow.Dialog

Shadow.Popup
```

Shadow Tokens können intern mehrere Schattenebenen kombinieren.

---

# 9. Ebenenhierarchie

Empfohlene Zuordnung:

| Ebene | Typische Verwendung |
|--------|---------------------|
| Level0 | Hintergrund |
| Level1 | Standardoberflächen |
| Level2 | Karten |
| Level3 | Menüs |
| Level4 | Dialoge |
| Level5 | Tooltips und kritische Overlays |

Die Hierarchie bleibt unabhängig vom Theme erhalten.

---

# 10. Schattendarstellung

Ein Schatten kann definieren:

- X-Versatz
- Y-Versatz
- Weichzeichnung
- Deckkraft
- Farbe

Diese Werte werden ausschließlich über das Theme bereitgestellt.

---

# 11. Transparenz

Schatten können Transparenzen verwenden.

Die Deckkraft wird über semantische Shadow Tokens festgelegt.

Harte Schatten sollen vermieden werden.

---

# 12. Glasoberflächen

Glass-Komponenten besitzen:

- reduzierte Schatten
- Hintergrundunschärfe
- transparente Oberflächen

Die Kombination wird durch das Theme definiert.

---

# 13. Animation

Elevation kann animiert werden.

Beispiele:

- Hover
- Fokus
- Dialog öffnen
- Menü öffnen

Die Animation erfolgt ausschließlich über die Motion Engine.

---

# 14. Accessibility

Accessibility kann:

- Schatten verstärken
- Schatten reduzieren
- Tiefeneffekte deaktivieren

Die semantischen Tokens bleiben unverändert.

---

# 15. Themeintegration

Themes dürfen verändern:

- Schattenparameter
- Schattenfarbe
- Transparenz
- Weichzeichnung

Die Namen der Elevation Tokens bleiben unverändert.

---

# 16. Validierung

Vor der Aktivierung eines Themes werden geprüft:

- vollständige Shadow Tokens
- vollständige Elevation Tokens
- ungültige Parameter
- fehlende Ebenen

Ungültige Themes werden abgelehnt.

---

# 17. Sicherheit

Das System verhindert:

- negative Unschärfen
- ungültige Deckkraftwerte
- fehlende Shadow Tokens
- doppelte Tokendefinitionen

Alle Schattenparameter werden validiert.

---

# 18. Speicherverwaltung

Das Elevation-System verwendet:

- statische Tokentabellen
- Shadow-Pools
- Theme-Puffer

Heap-Allokationen während der Laufzeit sind verboten.

---

# 19. Performance

Anforderungen:

- O(1)-Elevation Lookup
- O(1)-Shadow Lookup
- O(1)-Theme Lookup
- deterministische Laufzeit

Schattenparameter werden beim Theme-Laden vorbereitet.

---

# 20. API

```c
typedef enum
{
    NOVA_ELEVATION_NONE,
    NOVA_ELEVATION_LEVEL1,
    NOVA_ELEVATION_LEVEL2,
    NOVA_ELEVATION_LEVEL3,
    NOVA_ELEVATION_LEVEL4,
    NOVA_ELEVATION_LEVEL5
}
nova_elevation_token_t;

const nova_shadow_style_t*
nova_shadow_get(
    nova_elevation_token_t token);

bool
nova_shadow_exists(
    nova_elevation_token_t token);

nova_result_t
nova_shadow_reload(void);
```

---

# 21. Diagnosedaten

Zu erfassen sind:

- Theme-ID
- Anzahl Shadow Tokens
- Elevation Tokens
- Ladezeit
- Validierungsfehler
- Shadow Overrides

---

# 22. Testfälle

Zu testen sind:

- Buttons
- Karten
- Dialoge
- Menüs
- Tooltips
- Themewechsel
- Animationen
- Accessibility
- ungültige Schattenparameter
- Fehlerfälle

---

# 23. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- sämtliche Tiefeneffekte ausschließlich über Elevation- und Shadow-Tokens definiert werden.
- Komponenten konsistente Elevationsebenen verwenden.
- Themewechsel ohne Änderungen an Komponenten möglich sind.
- Elevation animiert werden kann.
- ungültige Schattenparameter erkannt werden.
- keine festen Schattenwerte in Komponenten gespeichert werden.
- keine Heap-Allokationen während der Laufzeit stattfinden.
- BIOS- und UEFI-Systeme identisches Erscheinungsbild erzeugen.

---

# 24. Referenzen

## ADRs

- ADR-BOOTDESIGN-0007 – Elevation and Shadow System
- ADR-BOOTDESIGN-0002 – Design Tokens
- ADR-BOOTTHEME-0001 – Theme System
- ADR-BOOTRENDER-0014 – Shadows and Glow

## NPSPECs

- NPSPEC-BOOTDESIGN-0001 – Boot Design Language
- NPSPEC-BOOTDESIGN-0002 – Boot Design Tokens
- NPSPEC-BOOTRENDER-0014 – Shadows and Glow
- NPSPEC-BOOTTHEME-0001 – Theme System
- NPSPEC-BOOTCONTROL-0003 – Control Styling

---

# 25. Zusammenfassung

Das **Elevation and Shadow System** definiert die semantische Darstellung räumlicher Tiefe innerhalb der NovaOS-Bootoberfläche. Es beschreibt Elevationsebenen und Schatten vollständig über Design Tokens und trennt deren Definition von der Implementierung. Durch zentrale Tokenverwaltung, Themeintegration, Unterstützung für Animationen und Accessibility sowie statische Speicherverwaltung gewährleistet das System eine konsistente, moderne und leistungsfähige Tiefendarstellung für BIOS- und UEFI-basierte Bootoberflächen.