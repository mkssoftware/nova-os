# NPSPEC-BOOTCONTROL-ICON-0001
# Icon

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTCONTROL-ICON-0001 |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot UI Controls |
| Priorität | Hoch |
| Gültigkeitsbereich | NovaOS Boot Manager |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTCONTROL-0001, NPSPEC-BOOTCONTROL-0003, NPSPEC-BOOTCONTROL-0004, NPSPEC-BOOTRENDER-0017, NPSPEC-BOOTACCESSIBILITY-0001 |

---

# 1. Ziel

Diese Spezifikation definiert das **Icon-Control** der NovaOS Boot UI.

Das Icon-Control dient der Darstellung skalierbarer grafischer Symbole innerhalb der Bootoberfläche. Es stellt eine eigenständige UI-Komponente dar und kann sowohl dekorativ als auch semantisch eingesetzt werden.

Typische Einsatzbereiche:

- Buttons
- Listen
- Tiles
- Dialoge
- Statusanzeigen
- Warnungen
- Navigation
- Bootoptionen

---

# 2. Geltungsbereich

Diese Spezifikation gilt für sämtliche Icons.

Sie umfasst:

- Systemicons
- Statusicons
- Funktionsicons
- Dateisymbole
- Laufwerkssymbole
- Warnsymbole
- Animierte Icons (optional)

Nicht Bestandteil sind:

- Rasterbilder
- Fotos
- Hintergrundgrafiken

---

# 3. Architektur

```text
Boot Control Framework

↓

Icon Control

↓

Icon Manager

↓

Vector Renderer

↓

Framebuffer
```

---

# 4. Grundprinzip

Ein Icon repräsentiert genau eine semantische Bedeutung.

Icons sollen Informationen ergänzen und die Benutzeroberfläche verständlicher machen.

Ein Icon darf niemals die einzige Informationsquelle für sicherheitskritische Funktionen sein.

---

# 5. Aufgaben

Das Icon-Control übernimmt:

- Darstellung
- Skalierung
- Themeintegration
- Farbverwaltung
- Animationen
- Accessibility
- Statusdarstellung

---

# 6. Aufbau

```text
Icon

├── Vector Data

├── Style

├── Theme Colors

└── Animation Layer (optional)
```

---

# 7. Eigenschaften

Jedes Icon besitzt:

- ID
- Symbol
- Größe
- Farbe
- Sichtbarkeit
- Style
- Template

Optional:

- Status
- Animation
- Tooltip

---

# 8. Iconquellen

Unterstützt werden:

- SVG
- Vektorpfade
- Symbolschrift
- eingebettete Ressourcen

Rastergrafiken sind ausschließlich als Kompatibilitätsmodus zulässig.

---

# 9. Größen

Empfohlene Standardgrößen:

| Typ | Größe |
|------|-------|
| XS | 16 × 16 DLU |
| Small | 20 × 20 DLU |
| Medium | 24 × 24 DLU |
| Large | 32 × 32 DLU |
| XL | 48 × 48 DLU |
| XXL | 64 × 64 DLU |

Alle Größen sind DPI-unabhängig.

---

# 10. Skalierung

Icons werden:

- proportional
- verlustfrei
- DPI-unabhängig

skaliert.

Seitenverhältnisse dürfen nicht verändert werden.

---

# 11. Zustände

```text
Normal

Disabled

Hidden
```

Optional:

```text
Hovered

Focused

Busy

Error

Warning

Success
```

Die Zustände beeinflussen ausschließlich die Darstellung.

---

# 12. Darstellung

Icons unterstützen:

- monochrom
- mehrfarbig
- Themefarben
- Transparenz
- Glow
- Schatten

Farben stammen ausschließlich aus dem Theme.

---

# 13. Animationen

Optional unterstützt:

- Fade
- Rotation
- Pulsieren
- Scale
- Statuswechsel

Animationen erfolgen über das Motion-System.

---

# 14. Accessibility

Jedes Icon besitzt:

- Name
- Beschreibung
- semantische Rolle

Dekorative Icons werden von Screenreadern ignoriert.

Semantische Icons müssen einen Accessibility-Namen besitzen.

---

# 15. Themeintegration

Das Theme definiert:

- Primärfarbe
- Sekundärfarbe
- Akzentfarbe
- Fehlerfarbe
- Warnfarbe
- Transparenz

Icons übernehmen Themeänderungen automatisch.

---

# 16. Ressourcenverwaltung

Icons werden:

- zentral registriert
- gemeinsam genutzt
- einmal geladen
- mehrfach referenziert

Doppelte Ressourcen sind zu vermeiden.

---

# 17. Speicherverwaltung

Icons verwenden:

- zentrale Iconbibliothek
- Objektpools
- Vektordaten
- gemeinsame Farbdefinitionen

Heap-Allokationen während der Laufzeit sind verboten.

---

# 18. Performance

Anforderungen:

- O(1)-Iconzugriff
- O(1)-Farbwechsel
- O(1)-Skalierung
- deterministische Laufzeit

Bereits gerenderte Icons dürfen zwischengespeichert werden.

---

# 19. Fehlerbehandlung

Folgende Fehler müssen erkannt werden:

- fehlende Icon-ID
- beschädigte SVG-Daten
- ungültige Vektordaten
- fehlende Themefarbe
- Nullpointer

Kann ein Icon nicht geladen werden, wird ein Standard-Platzhaltersymbol verwendet.

---

# 20. Sicherheit

Das System verhindert:

- beschädigte Iconreferenzen
- rekursive Ressourcenabhängigkeiten
- ungültige Themeverweise
- fehlerhafte SVG-Daten
- unkontrollierte Speicherbelegung

Alle Icondaten werden vor der Verwendung validiert.

---

# 21. API

```c
typedef struct nova_icon_control nova_icon_control_t;

nova_result_t
nova_icon_create(
    nova_icon_t icon,
    nova_icon_control_t** control);

nova_result_t
nova_icon_set(
    nova_icon_control_t* control,
    nova_icon_t icon);

nova_result_t
nova_icon_set_size(
    nova_icon_control_t* control,
    uint32_t size);

nova_result_t
nova_icon_set_color(
    nova_icon_control_t* control,
    nova_color_t color);

nova_icon_t
nova_icon_get(
    const nova_icon_control_t* control);
```

---

# 22. Diagnosedaten

Zu erfassen sind:

- Icon-ID
- Größe
- Themefarbe
- Renderzeit
- Cache-Treffer
- Speicherverbrauch
- Fehleranzahl

---

# 23. Testfälle

Zu testen sind:

- SVG-Rendering
- Skalierung
- Themewechsel
- High-DPI
- Transparenz
- Statusfarben
- Animationen
- Accessibility
- Ressourcenfreigabe
- Fehlerfälle

---

# 24. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- ausschließlich skalierbare Vektorgrafiken unterstützt werden.
- Themefarben automatisch übernommen werden.
- High-DPI ohne Qualitätsverlust funktioniert.
- dekorative und semantische Icons korrekt unterschieden werden.
- Ressourcen gemeinsam genutzt werden.
- keine Heap-Allokationen während der Laufzeit stattfinden.
- BIOS- und UEFI-Systeme identisches Verhalten erzeugen.

---

# 25. Referenzen

## ADRs

- ADR-BOOTCONTROL-ICON-0001 – Icon-Control
- ADR-BOOTRENDER-0017 – Image Rendering
- ADR-BOOTTHEME-0001 – Theme-System
- ADR-BOOTACCESS-0001 – Accessibility
- ADR-BOOTPERF-0002 – Deterministische UI-Verarbeitung

## NPSPECs

- NPSPEC-BOOTCONTROL-0001 – Boot Control Framework
- NPSPEC-BOOTCONTROL-0003 – Control Styling
- NPSPEC-BOOTCONTROL-0004 – Control Templates
- NPSPEC-BOOTRENDER-0017 – Image Rendering
- NPSPEC-BOOTACCESSIBILITY-0001 – Accessibility Framework

---

# 26. Zusammenfassung

Das **Icon-Control** definiert die standardisierte Darstellung skalierbarer Symbole innerhalb der NovaOS-Bootoberfläche. Es basiert auf Vektorgrafiken, unterstützt automatische Themeanpassungen, High-DPI-Skalierung, Animationen und vollständige Accessibility. Durch zentrale Ressourcenverwaltung, deterministische Verarbeitung und statische Speicherverwaltung entsteht ein leistungsfähiges und konsistentes Symbolsystem für BIOS- und UEFI-basierte Bootumgebungen.