# NPSPEC-BOOTTEXT-0004
# Font Loading

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTTEXT-0004 |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot UI – Text System |
| Priorität | Kritisch |
| Gültigkeitsbereich | NovaOS Boot Manager |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTTEXT-0001, NPSPEC-BOOTTEXT-0002, NPSPEC-BOOTTEXT-0003, NPSPEC-BOOTFONT-0001, NPSPEC-BOOTFS-0001, NPSPEC-BOOTRENDER-0001 |

---

# 1. Ziel

Diese Spezifikation definiert das **Font Loading System** der NovaOS-Bootoberfläche.

Das Font Loading System ist für das Laden, Validieren, Registrieren und Verwalten sämtlicher Schriftarten innerhalb der Bootumgebung verantwortlich. Es stellt sicher, dass alle Texte unabhängig von Firmware, Sprache oder Bildschirmauflösung mit den vorgesehenen Schriftarten dargestellt werden.

Das System arbeitet vollständig unabhängig vom eigentlichen Textlayout oder Rendering.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für:

- Boot Manager
- Recovery
- Installer
- Diagnosemodule
- Theme-System
- Lokalisierung
- sämtliche UI-Controls

---

# 3. Ziele

Das Font-System muss:

- Fonts sicher laden
- Fonts validieren
- mehrere Fontfamilien unterstützen
- Font Fallback ermöglichen
- deterministisch arbeiten
- ohne dynamische Speicherallokation auskommen

---

# 4. Architektur

```text
NovaFS

↓

Font Loader

↓

Font Validator

↓

Font Registry

↓

Glyph Cache

↓

Text Engine

↓

Renderer
```

---

# 5. Komponenten

Das Font Loading System besteht aus:

- Font Loader
- Font Registry
- Font Validator
- Font Cache
- Font Resolver
- Font Fallback Manager

---

# 6. Grundprinzip

Der Ladevorgang erfolgt in mehreren Schritten:

```text
Datei

↓

Prüfung

↓

Validierung

↓

Registrierung

↓

Glyph Cache

↓

Verwendung
```

Nicht validierte Fonts dürfen niemals verwendet werden.

---

# 7. Unterstützte Fontformate

Pflicht:

- Nova Font Package (NFP)

Optional:

- Bitmap Fonts
- OpenType (OTF)
- TrueType (TTF)
- WOFF2 (zukünftig)

Die tatsächlich unterstützten Formate werden durch den Font Loader bestimmt.

---

# 8. Fontpakete

Ein Fontpaket besteht aus:

```text
Header

Glyphdaten

Unicode Mapping

Metriken

Kerning

Signatur (optional)
```

Alle Bereiche werden vor dem Laden geprüft.

---

# 9. Validierung

Vor der Registrierung werden geprüft:

- Dateiformat
- Versionsnummer
- Header
- Glyphanzahl
- Unicode Mapping
- Tabellenintegrität
- optionale Signatur

Ungültige Fonts werden verworfen.

---

# 10. Font Registry

Alle geladenen Fonts werden zentral registriert.

Jeder Font besitzt:

- Font-ID
- Familienname
- Stil
- Gewicht
- Version
- Priorität

Die Registry enthält ausschließlich gültige Fonts.

---

# 11. Fontfamilien

Unterstützt werden:

- Regular
- Bold
- Italic
- Bold Italic
- Light
- Medium
- SemiBold

Weitere Varianten können ergänzt werden.

---

# 12. Font Fallback

Kann ein Font ein Zeichen nicht darstellen:

```text
Primärfont

↓

Fallback Font

↓

System Font

↓

Missing Glyph
```

Die Reihenfolge wird zentral definiert.

---

# 13. Glyph Cache

Nach erfolgreichem Laden werden Glyphen in den Glyph Cache übernommen.

Der Cache ist:

- gemeinsam
- statisch
- zentral verwaltet

Bereits geladene Glyphen werden mehrfach verwendet.

---

# 14. Themeintegration

Das Theme definiert:

- Standardfont
- Überschriftenfont
- Monospacefont
- Fallbackfont

Das Theme lädt keine Fonts direkt.

---

# 15. Fontwechsel

Ein Themewechsel kann den aktiven Font ändern.

Der Ablauf:

```text
Themewechsel

↓

Neue Font-ID

↓

Glyph Cache Update

↓

Neues Rendering
```

Die Registry bleibt unverändert.

---

# 16. Sicherheit

Das System verhindert:

- beschädigte Fontdateien
- ungültige Header
- manipulierte Glyphtabellen
- doppelte Font-IDs
- rekursive Fontreferenzen

Alle Fontdateien werden vollständig validiert.

---

# 17. Speicherverwaltung

Das Font-System verwendet:

- statische Fonttabellen
- Glyph-Pools
- Font-Pools

Heap-Allokationen während der Laufzeit sind verboten.

---

# 18. Performance

Anforderungen:

- O(1)-Font Lookup
- O(1)-Registry Lookup
- O(1)-Glyph Lookup
- O(n)-Ladevorgang

Fontdateien werden ausschließlich einmal geladen.

---

# 19. API

```c
typedef struct nova_font nova_font_t;

nova_result_t
nova_font_initialize(void);

nova_result_t
nova_font_load(
    const char* path,
    nova_font_t** font);

nova_result_t
nova_font_register(
    nova_font_t* font);

const nova_font_t*
nova_font_find(
    const char* family,
    nova_font_style_t style);

bool
nova_font_validate(
    const char* path);
```

---

# 20. Diagnosedaten

Zu erfassen sind:

- Font-ID
- Fontfamilie
- Version
- Glyphanzahl
- Ladezeit
- Cachegröße
- Validierungsfehler
- Speicherverbrauch

---

# 21. Testfälle

Zu testen sind:

- Nova Font Package
- Bitmap Font
- TTF (optional)
- OTF (optional)
- Font Fallback
- beschädigte Fonts
- Themewechsel
- Registry
- Glyph Cache
- Fehlerfälle

---

# 22. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- alle unterstützten Fontformate korrekt geladen werden.
- ausschließlich validierte Fonts registriert werden.
- Font Fallback deterministisch arbeitet.
- Themewechsel Fonts korrekt übernehmen.
- Glyphen nur einmal geladen werden.
- keine dynamischen Speicherallokationen während der Laufzeit stattfinden.
- BIOS- und UEFI-Systeme identisches Verhalten erzeugen.

---

# 23. Referenzen

## ADRs

- ADR-BOOTTEXT-0004 – Font Loading
- ADR-BOOTFONT-0001 – Font Management
- ADR-BOOTTHEME-0001 – Theme System
- ADR-BOOTRENDER-0001 – Rendering Engine

## NPSPECs

- NPSPEC-BOOTTEXT-0001 – Boot Text Engine
- NPSPEC-BOOTTEXT-0002 – UTF-8 Text Processing
- NPSPEC-BOOTTEXT-0003 – Unicode Character Support
- NPSPEC-BOOTFONT-0001 – Font Management
- NPSPEC-BOOTFS-0001 – Boot File System
- NPSPEC-BOOTRENDER-0001 – Boot Rendering Engine

---

# 24. Zusammenfassung

Das **Font Loading System** definiert den standardisierten Ladevorgang für sämtliche Schriftarten der NovaOS-Bootoberfläche. Es übernimmt das sichere Laden, Validieren, Registrieren und Bereitstellen von Fonts für die Text Engine und den Glyph Cache. Durch zentrale Fontverwaltung, deterministische Font-Fallback-Mechanismen, statische Speicherstrukturen und vollständige Theme-Integration gewährleistet das System eine konsistente und performante Textdarstellung auf BIOS- und UEFI-basierten Systemen.