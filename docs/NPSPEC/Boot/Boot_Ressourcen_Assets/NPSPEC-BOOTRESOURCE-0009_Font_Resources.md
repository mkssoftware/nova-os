# NPSPEC-BOOTRESOURCE-0009
# Font Resources

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTRESOURCE-0009 |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot UI – Resource System |
| Priorität | Kritisch |
| Gültigkeitsbereich | NovaOS Boot Manager |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTRESOURCE-0001, NPSPEC-BOOTRESOURCE-0004, NPSPEC-BOOTTEXT-0004, NPSPEC-BOOTTEXT-0005, NPSPEC-BOOTTEXT-0006, NPSPEC-BOOTTEXT-0010 |

---

# 1. Ziel

Diese Spezifikation definiert die Verwaltung von **Font Resources** innerhalb der NovaOS-Bootoberfläche.

Das System beschreibt, wie Schriftarten als Ressourcen gespeichert, geladen, registriert, versioniert und dem Textsystem zur Verfügung gestellt werden. Font Resources bilden die Grundlage für die gesamte Textdarstellung innerhalb der Bootumgebung.

Alle Schriftarten werden ausschließlich über den Boot Resource Manager verwaltet.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für:

- UI-Schriftarten
- Monospace-Schriften
- Symbolschriften
- Theme-Schriften
- Fallback-Schriften
- Sprachspezifische Schriften

---

# 3. Ziele

Das Font-Resource-System muss:

- Schriftarten zentral verwalten
- Mehrfachnutzung ermöglichen
- Font-Fallback unterstützen
- deterministisch arbeiten
- Themeintegration besitzen
- zukünftige Fontformate unterstützen

---

# 4. Architektur

```text
Boot Asset Package

↓

Resource Loader

↓

Font Resource

↓

Font Registry

↓

Text Engine

↓

Glyph Rasterizer

↓

Renderer
```

---

# 5. Komponenten

Das System besteht aus:

- Font Loader
- Font Registry
- Font Cache
- Font Validator
- Font Resolver
- Font Metadata Manager

---

# 6. Grundprinzip

Der Ablauf erfolgt wie folgt:

```text
Fontdatei

↓

Validierung

↓

Registrierung

↓

Font Registry

↓

Textsystem
```

Die eigentliche Glyphenrasterisierung erfolgt erst bei Bedarf.

---

# 7. Unterstützte Fontformate

Pflicht:

- Nova Font Package (NFP)

Optional:

- TrueType (TTF)
- OpenType (OTF)
- Bitmap Fonts

Die Unterstützung weiterer Formate ist möglich.

---

# 8. Font Resource Descriptor

Jede Font Resource enthält mindestens:

- Font-ID
- Name
- Version
- Fontfamilie
- Schriftschnitt
- Unicode-Abdeckung
- Ressourcenversion

---

# 9. Fontfamilien

Unterstützt werden:

- System
- Display
- Monospace
- Symbols
- UI
- Theme Fonts

Weitere Familien können ergänzt werden.

---

# 10. Schriftschnitte

Unterstützt werden:

- Light
- Regular
- Medium
- SemiBold
- Bold
- Italic (optional)

Jeder Schriftschnitt wird als eigene Font Resource verwaltet.

---

# 11. Unicode-Abdeckung

Jede Schrift beschreibt:

- Unicode-Blöcke
- unterstützte Scripts
- Anzahl Glyphen
- Font-Fallback-Informationen

Diese Informationen werden für die Fontauswahl verwendet.

---

# 12. Font Registry

Alle geladenen Fonts werden zentral registriert.

Die Registry verwaltet:

- Font-ID
- Fontfamilie
- Ressourcenzustand
- Referenzzähler
- Version

Direkte Dateizugriffe durch das Textsystem sind nicht zulässig.

---

# 13. Font Cache

Bereits geladene Fonts verbleiben im Cache.

Mehrfaches Laden derselben Schrift ist nicht zulässig.

Der Cache arbeitet gemeinsam mit dem Glyph Atlas.

---

# 14. Themeintegration

Ein Theme verweist ausschließlich auf Font Resources.

Direkte Fontdateien dürfen nicht referenziert werden.

Themewechsel führen lediglich zu neuen Fontreferenzen.

---

# 15. Font-Fallback

Kann eine Glyphe nicht dargestellt werden:

```text
Primärfont

↓

Fallback Font

↓

Systemfont

↓

Missing Glyph
```

Die Fallback-Kette wird separat definiert.

---

# 16. Sicherheit

Das System verhindert:

- beschädigte Fontdateien
- doppelte Font-IDs
- ungültige Unicode-Tabellen
- rekursive Fontreferenzen
- Buffer Overflows

Alle Fontdateien werden vor der Registrierung validiert.

---

# 17. Speicherverwaltung

Das Font-Resource-System verwendet:

- statische Fonttabellen
- Font-Pools
- Objektpools

Glyphdaten werden erst bei Bedarf geladen.

Heap-Allokationen während der Laufzeit sind verboten.

---

# 18. Performance

Anforderungen:

- O(1)-Font Lookup
- O(1)-Registry Lookup
- O(1)-Cache Lookup
- O(n)-Fontinitialisierung

Bereits registrierte Fonts werden nicht erneut geladen.

---

# 19. API

```c
typedef struct
{
    uint32_t font_id;
    uint32_t version;
    const char* family;
    const char* style;
}
nova_font_resource_t;

nova_result_t
nova_font_resource_initialize(void);

const nova_font_resource_t*
nova_font_resource_load(
    uint32_t resource_id);

const nova_font_resource_t*
nova_font_resource_find(
    uint32_t font_id);

bool
nova_font_resource_validate(
    const nova_font_resource_t* font);
```

---

# 20. Diagnosedaten

Zu erfassen sind:

- Font-ID
- Fontfamilie
- Schriftschnitt
- Unicode-Abdeckung
- Ladezeit
- Cache-Hits
- Referenzanzahl
- Speicherverbrauch

---

# 21. Testfälle

Zu testen sind:

- Nova Font Package
- TrueType (optional)
- OpenType (optional)
- Font-Fallback
- Themewechsel
- Unicode-Abdeckung
- doppelte Font-IDs
- beschädigte Fonts
- Cache-Wiederverwendung
- Fehlerfälle

---

# 22. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- sämtliche Schriftarten ausschließlich über Font Resources verwaltet werden.
- die Font Registry korrekt arbeitet.
- Font-Fallback unterstützt wird.
- Themewechsel ohne Neuladen der UI möglich sind.
- beschädigte Fonts erkannt werden.
- mehrfaches Laden derselben Schrift verhindert wird.
- keine Heap-Allokationen während der Laufzeit stattfinden.
- BIOS- und UEFI-Systeme identisches Verhalten erzeugen.

---

# 23. Referenzen

## ADRs

- ADR-BOOTRESOURCE-0009 – Font Resources
- ADR-BOOTTEXT-0004 – Font Loading
- ADR-BOOTTEXT-0010 – Font Fallback
- ADR-BOOTRESOURCE-0001 – Resource Manager

## NPSPECs

- NPSPEC-BOOTRESOURCE-0001 – Boot Resource Manager
- NPSPEC-BOOTRESOURCE-0004 – Resource Loading
- NPSPEC-BOOTTEXT-0004 – Font Loading
- NPSPEC-BOOTTEXT-0005 – Glyph Rasterization
- NPSPEC-BOOTTEXT-0006 – Glyph Atlas
- NPSPEC-BOOTTEXT-0010 – Font Fallback

---

# 24. Zusammenfassung

Die **Font Resources** definieren die standardisierte Verwaltung sämtlicher Schriftarten innerhalb der NovaOS-Bootoberfläche. Sie ermöglichen die zentrale Registrierung, Versionierung und Bereitstellung von Schriftarten für das Textsystem und unterstützen Font-Fallback, Themeintegration sowie Unicode-basierte Schriftwahl. Durch Resource-Management, Font-Caching, deterministische Verarbeitung und statische Speicherverwaltung bilden Font Resources die Grundlage für eine konsistente und performante Textdarstellung auf BIOS- und UEFI-basierten Systemen.