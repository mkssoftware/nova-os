# NPSPEC-BOOTDESIGN-0008
# Glass Material System

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTDESIGN-0008 |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot UI – Design System |
| Priorität | Hoch |
| Gültigkeitsbereich | NovaOS Boot Manager |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTDESIGN-0001, NPSPEC-BOOTDESIGN-0002, NPSPEC-BOOTRENDER-0015, NPSPEC-BOOTTHEME-0001, NPSPEC-BOOTCONTROL-0003 |

---

# 1. Ziel

Diese Spezifikation definiert das **Glass Material System** der NovaOS-Bootoberfläche.

Das Glass Material System beschreibt die Darstellung transparenter Oberflächen innerhalb der Bootumgebung. Ziel ist eine moderne, hochwertige und ruhige Benutzeroberfläche, die Tiefenwirkung durch Glasmaterialien erzeugt, ohne Lesbarkeit oder Performance zu beeinträchtigen.

Alle Materialeigenschaften werden ausschließlich über semantische Material Tokens definiert.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für:

- Dialoge
- Karten
- Menüs
- Kontextmenüs
- Tooltips
- Seitencontainer
- Overlays
- Schnellzugriffe
- sämtliche Glass-Komponenten

---

# 3. Ziele

Das Glass Material System muss:

- konsistente Glasoberflächen bereitstellen
- Themewechsel unterstützen
- deterministisch arbeiten
- hohe Lesbarkeit gewährleisten
- Performance optimieren
- zukünftige Materialtypen unterstützen

---

# 4. Architektur

```text
Theme

↓

Material Tokens

↓

Glass Renderer

↓

Shadow System

↓

UI-Komponenten
```

---

# 5. Komponenten

Das System besteht aus:

- Material Registry
- Material Resolver
- Blur Engine
- Transparency Manager
- Border Renderer
- Validation Engine

---

# 6. Grundprinzip

Alle Glasoberflächen verwenden Material Tokens.

```text
Dialog

↓

Material.Glass.Dialog

↓

Theme

↓

Renderer
```

Komponenten enthalten keine festen Materialparameter.

---

# 7. Material Tokens

Pflichttokens:

```text
Material.None

Material.Surface

Material.Glass

Material.Glass.Light

Material.Glass.Medium

Material.Glass.Strong

Material.Overlay

Material.Dialog
```

Die tatsächlichen Werte werden ausschließlich durch das Theme definiert.

---

# 8. Materialeigenschaften

Ein Material kann definieren:

- Transparenz
- Hintergrundunschärfe
- Oberflächenfarbe
- Rahmenfarbe
- Reflexion
- Schatten
- Helligkeit

Alle Eigenschaften sind optional.

---

# 9. Hintergrundunschärfe

Glass-Materialien können Hintergrundunschärfe verwenden.

Unterstützt werden:

- keine Unschärfe
- leicht
- mittel
- stark

Die Stärke wird über Material Tokens bestimmt.

---

# 10. Transparenz

Unterstützte Transparenzstufen:

- deckend
- leicht transparent
- mittel transparent
- stark transparent

Die tatsächlichen Alpha-Werte werden vom Theme festgelegt.

---

# 11. Reflexion

Optional können Materialien definieren:

- Lichtreflexe
- Glanz
- Oberflächenakzente

Diese Effekte müssen dezent bleiben.

---

# 12. Rahmen

Glass-Komponenten können:

- feine Konturen
- halbtransparente Rahmen
- Akzentlinien

verwenden.

Rahmen werden über Border Tokens definiert.

---

# 13. Zusammenspiel mit Schatten

Glass-Materialien verwenden das Elevation and Shadow System.

Schatten und Glasmaterial werden gemeinsam gerendert.

---

# 14. Animation

Materialparameter können animiert werden.

Beispiele:

- Transparenz
- Unschärfe
- Reflexion
- Rahmen

Die Ausführung erfolgt ausschließlich über die Motion Engine.

---

# 15. Accessibility

Accessibility kann:

- Transparenzen reduzieren
- Unschärfen deaktivieren
- Kontraste erhöhen
- Glasmaterial vollständig ersetzen

Die semantischen Material Tokens bleiben erhalten.

---

# 16. Themeintegration

Themes dürfen verändern:

- Transparenz
- Blur
- Reflexion
- Rahmen
- Materialfarben

Die Namen der Material Tokens bleiben unverändert.

---

# 17. Validierung

Vor der Aktivierung eines Themes werden geprüft:

- vollständige Material Tokens
- gültige Transparenzwerte
- Blurparameter
- fehlende Materialdefinitionen

Ungültige Themes werden abgelehnt.

---

# 18. Sicherheit

Das System verhindert:

- ungültige Transparenzwerte
- negative Blurparameter
- doppelte Material Tokens
- rekursive Materialdefinitionen

Alle Materialparameter werden validiert.

---

# 19. Speicherverwaltung

Das Materialsystem verwendet:

- statische Materialtabellen
- Theme-Puffer
- Blur-Pools

Heap-Allokationen während der Laufzeit sind verboten.

---

# 20. Performance

Anforderungen:

- O(1)-Material Lookup
- O(1)-Theme Lookup
- O(1)-Blur Lookup
- deterministische Laufzeit

Blurparameter werden beim Theme-Laden vorbereitet.

Software-Rendering muss ohne GPU-Unterstützung möglich sein.

---

# 21. API

```c
typedef enum
{
    NOVA_MATERIAL_NONE,
    NOVA_MATERIAL_SURFACE,
    NOVA_MATERIAL_GLASS,
    NOVA_MATERIAL_GLASS_LIGHT,
    NOVA_MATERIAL_GLASS_MEDIUM,
    NOVA_MATERIAL_GLASS_STRONG,
    NOVA_MATERIAL_OVERLAY,
    NOVA_MATERIAL_DIALOG
}
nova_material_token_t;

const nova_material_style_t*
nova_material_get(
    nova_material_token_t token);

bool
nova_material_exists(
    nova_material_token_t token);

nova_result_t
nova_material_reload(void);
```

---

# 22. Diagnosedaten

Zu erfassen sind:

- Theme-ID
- Material Tokens
- Blurstufen
- Transparenzwerte
- Validierungsfehler
- Ladezeit

---

# 23. Testfälle

Zu testen sind:

- Dialoge
- Karten
- Menüs
- Overlays
- Transparenzen
- Blur
- Themewechsel
- Accessibility
- ungültige Materialdefinitionen
- Fehlerfälle

---

# 24. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- sämtliche Glasoberflächen ausschließlich Material Tokens verwenden.
- Transparenz und Blur zentral definiert werden.
- Themewechsel ohne Änderungen an Komponenten möglich sind.
- Accessibility Glasmaterial ersetzen oder reduzieren kann.
- ungültige Materialparameter erkannt werden.
- keine festen Materialwerte in Komponenten gespeichert werden.
- keine Heap-Allokationen während der Laufzeit stattfinden.
- BIOS- und UEFI-Systeme identisches Erscheinungsbild erzeugen.

---

# 25. Referenzen

## ADRs

- ADR-BOOTDESIGN-0008 – Glass Material System
- ADR-BOOTDESIGN-0002 – Design Tokens
- ADR-BOOTRENDER-0015 – Background Blur
- ADR-BOOTTHEME-0001 – Theme System

## NPSPECs

- NPSPEC-BOOTDESIGN-0001 – Boot Design Language
- NPSPEC-BOOTDESIGN-0002 – Boot Design Tokens
- NPSPEC-BOOTRENDER-0015 – Background Blur
- NPSPEC-BOOTTHEME-0001 – Theme System
- NPSPEC-BOOTCONTROL-0003 – Control Styling

---

# 26. Zusammenfassung

Das **Glass Material System** definiert die semantische Darstellung transparenter Materialien innerhalb der NovaOS-Bootoberfläche. Es beschreibt Transparenz, Hintergrundunschärfe, Reflexionen, Rahmen und Oberflächeneigenschaften vollständig über Material Tokens und trennt deren Definition von der Implementierung. Durch Themeintegration, Unterstützung für Accessibility, deterministische Verarbeitung und statische Speicherverwaltung ermöglicht das System eine hochwertige, moderne und performante Glass-UI für BIOS- und UEFI-basierte Bootoberflächen.