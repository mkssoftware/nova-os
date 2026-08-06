# NPSPEC-BOOTTEXT-0012
# Localized Text Rendering

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTTEXT-0012 |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot UI – Text System |
| Priorität | Kritisch |
| Gültigkeitsbereich | NovaOS Boot Manager |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTTEXT-0001, NPSPEC-BOOTTEXT-0002, NPSPEC-BOOTTEXT-0003, NPSPEC-BOOTTEXT-0008, NPSPEC-BOOTTEXT-0010, NPSPEC-BOOTTHEME-0001, NPSPEC-BOOTLOCALIZATION-0001 |

---

# 1. Ziel

Diese Spezifikation definiert das **Localized Text Rendering System** der NovaOS-Bootoberfläche.

Das System stellt sicher, dass sämtliche Benutzeroberflächentexte entsprechend der aktuell ausgewählten Sprache und Region geladen, verarbeitet, gelayoutet und gerendert werden. Dabei werden sprachabhängige Eigenschaften wie Schreibrichtung, Zeilenumbrüche, Font-Fallbacks und typografische Besonderheiten berücksichtigt.

Die Darstellung erfolgt vollständig unabhängig vom zugrunde liegenden Rendering-Backend.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für:

- Boot Manager
- Recovery
- Installer
- Diagnosemodule
- Dialoge
- Menüs
- Labels
- Buttons
- Fehlermeldungen
- Statusanzeigen

---

# 3. Ziele

Das System muss:

- Unicode vollständig unterstützen
- sprachabhängig rendern
- Regionen berücksichtigen
- Font-Fallback integrieren
- deterministisch arbeiten
- dynamische Sprachwechsel unterstützen

---

# 4. Architektur

```text
Locale

↓

Localization Manager

↓

Translation Database

↓

UTF-8 Processing

↓

Text Layout

↓

Localized Text Rendering

↓

Renderer
```

---

# 5. Komponenten

Das System besteht aus:

- Localization Manager
- Translation Resolver
- Locale Manager
- Font Resolver
- Script Resolver
- Text Layout Engine
- Renderer Adapter

---

# 6. Grundprinzip

Der Ablauf erfolgt wie folgt:

```text
Text-ID

↓

Übersetzung

↓

UTF-8

↓

Unicode

↓

Layout

↓

Rendering
```

Die UI arbeitet ausschließlich mit Text-IDs und niemals mit fest codierten Zeichenketten.

---

# 7. Locale

Eine Locale besteht mindestens aus:

- Sprache
- Region
- Standardschrift
- Zahlenformat
- Schreibrichtung

Beispiele:

```text
de-DE

en-US

fr-FR

ja-JP

zh-CN
```

---

# 8. Sprachressourcen

Alle Übersetzungen werden in separaten Sprachpaketen gespeichert.

Jeder Eintrag besteht aus:

- Text-ID
- Übersetzung
- Version
- Kontext (optional)

Nicht vorhandene Übersetzungen werden über eine definierte Fallback-Kette aufgelöst.

---

# 9. Font-Auswahl

Für jede Locale wird automatisch eine geeignete Schriftfamilie ausgewählt.

Die Auswahl erfolgt anhand:

- Schriftsystem
- Unicode-Abdeckung
- Theme
- Font-Fallback

Direkte Fontzuweisungen in UI-Komponenten sind nicht zulässig.

---

# 10. Schreibrichtung

Unterstützt werden:

### Pflicht

- Left-to-Right (LTR)

### Optional

- Right-to-Left (RTL)
- Bidirectional Text (BiDi)

Die Schreibrichtung wird durch die Locale bestimmt.

---

# 11. Textlayout

Das Layout berücksichtigt:

- Wortgrenzen
- Zeilenumbrüche
- Glyphmetriken
- Kerning
- Letter Spacing
- Ellipsis

Alle sprachabhängigen Regeln werden vor dem Rendering angewendet.

---

# 12. Zahlen und Datumsformate

Optional können sprachabhängige Formate verwendet werden für:

- Zahlen
- Prozentwerte
- Größenangaben
- Datums- und Zeitwerte

Diese Formatierung erfolgt vor dem Textlayout.

---

# 13. Sprachwechsel

Ein Wechsel der Sprache führt zu:

```text
Locale ändern

↓

Übersetzungen neu laden

↓

Layouts neu berechnen

↓

Rendering aktualisieren
```

Ein Neustart des Boot Managers ist hierfür nicht erforderlich.

---

# 14. Fallback

Kann eine Übersetzung nicht gefunden werden, erfolgt die Suche in folgender Reihenfolge:

```text
Aktuelle Locale

↓

Regionale Standardsprache

↓

Systemsprache

↓

Englisch

↓

Text-ID
```

Die Fallback-Kette ist deterministisch.

---

# 15. Themeintegration

Das Theme kann definieren:

- Standardschrift
- Schriftgrößen
- Typography Tokens
- bevorzugte Fallback-Fonts

Die sprachabhängige Auswahl erfolgt weiterhin durch das Lokalisierungssystem.

---

# 16. Accessibility

Unterstützt werden:

- Screen Reader
- große Schriftgrößen
- hoher Kontrast
- alternative Fonts

Accessibility verarbeitet stets die bereits lokalisierte Zeichenkette.

---

# 17. Sicherheit

Das System verhindert:

- fehlende Text-IDs
- ungültige Locale-Codes
- rekursive Fallback-Ketten
- beschädigte Sprachpakete
- ungültige Unicode-Sequenzen

Alle Sprachressourcen werden beim Laden validiert.

---

# 18. Speicherverwaltung

Das System verwendet:

- statische Übersetzungstabellen
- Locale-Pools
- Font-Pools
- Objektpools

Heap-Allokationen während der Laufzeit sind verboten.

---

# 19. Performance

Anforderungen:

- O(1)-Text-ID-Lookup
- O(1)-Locale-Lookup
- O(1)-Fontauflösung
- O(n)-Layoutberechnung

Bereits geladene Übersetzungen werden zwischengespeichert.

---

# 20. API

```c
typedef struct
{
    const char* language;
    const char* region;
}
nova_locale_t;

nova_result_t
nova_localization_initialize(void);

nova_result_t
nova_locale_set(
    const nova_locale_t* locale);

const char*
nova_localization_translate(
    nova_text_id_t text_id);

const nova_locale_t*
nova_locale_current(void);
```

---

# 21. Diagnosedaten

Zu erfassen sind:

- aktive Locale
- Sprache
- Region
- verwendete Fallbacks
- Font-Fallbacks
- Übersetzungsfehler
- Ladezeiten

---

# 22. Testfälle

Zu testen sind:

- Deutsch
- Englisch
- Französisch
- Japanisch
- Chinesisch
- fehlende Übersetzungen
- Locale-Wechsel
- RTL (optional)
- Font-Fallback
- Accessibility
- Fehlerfälle

---

# 23. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- sämtliche Texte über Text-IDs aufgelöst werden.
- Locale-Wechsel ohne Neustart möglich sind.
- Übersetzungs-Fallbacks deterministisch funktionieren.
- sprachabhängige Fonts korrekt gewählt werden.
- Unicode vollständig unterstützt wird.
- Accessibility korrekt mit lokalisierten Texten arbeitet.
- keine Heap-Allokationen während der Laufzeit stattfinden.
- BIOS- und UEFI-Systeme identisches Verhalten erzeugen.

---

# 24. Referenzen

## ADRs

- ADR-BOOTTEXT-0012 – Localized Text Rendering
- ADR-BOOTLOCALIZATION-0001 – Localization System
- ADR-BOOTTHEME-0001 – Theme System
- ADR-BOOTTEXT-0010 – Font Fallback

## NPSPECs

- NPSPEC-BOOTTEXT-0001 – Boot Text Engine
- NPSPEC-BOOTTEXT-0002 – UTF-8 Text Processing
- NPSPEC-BOOTTEXT-0003 – Unicode Character Support
- NPSPEC-BOOTTEXT-0008 – Text Layout
- NPSPEC-BOOTTEXT-0010 – Font Fallback
- NPSPEC-BOOTTHEME-0001 – Theme System
- NPSPEC-BOOTLOCALIZATION-0001 – Localization System

---

# 25. Zusammenfassung

Das **Localized Text Rendering System** definiert die sprach- und regionsabhängige Darstellung sämtlicher Texte innerhalb der NovaOS-Bootoberfläche. Es verbindet Übersetzungen, Unicode-Verarbeitung, Font-Fallback, Textlayout und Rendering zu einer konsistenten Lokalisierungspipeline. Durch deterministische Fallback-Strategien, dynamische Locale-Wechsel, vollständige Unicode-Unterstützung und statische Speicherverwaltung gewährleistet das System eine hochwertige und international nutzbare Benutzeroberfläche auf BIOS- und UEFI-basierten Systemen.