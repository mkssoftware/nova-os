# NPSPEC-BOOTDESIGN-0005
# Spacing System

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTDESIGN-0005 |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot UI – Design System |
| Priorität | Kritisch |
| Gültigkeitsbereich | NovaOS Boot Manager |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTDESIGN-0001, NPSPEC-BOOTDESIGN-0002, NPSPEC-BOOTLAYOUT-0001, NPSPEC-BOOTCONTROL-0003, NPSPEC-BOOTTHEME-0001 |

---

# 1. Ziel

Diese Spezifikation definiert das **Spacing System** der NovaOS-Bootoberfläche.

Das Spacing System beschreibt sämtliche Abstände innerhalb der Benutzeroberfläche. Dazu gehören Außenabstände, Innenabstände, Abstände zwischen Komponenten sowie Layoutraster. Ziel ist eine konsistente, ausgewogene und leicht erfassbare Oberfläche.

Alle Abstände werden ausschließlich über semantische Spacing Tokens definiert.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für:

- Seitenlayouts
- Dialoge
- Karten
- Listen
- Buttons
- Menüs
- Formulare
- Container
- sämtliche UI-Komponenten

---

# 3. Ziele

Das Spacing System muss:

- konsistente Abstände gewährleisten
- Layouts vereinheitlichen
- Themewechsel unterstützen
- deterministisch arbeiten
- Accessibility unterstützen
- zukünftige Komponenten integrieren

---

# 4. Architektur

```text
Theme

↓

Spacing Tokens

↓

Layout Engine

↓

Control Styles

↓

UI-Komponenten
```

---

# 5. Komponenten

Das System besteht aus:

- Spacing Registry
- Token Resolver
- Layout Engine
- Theme Integration
- Validation Engine
- Spacing Cache

---

# 6. Grundprinzip

Alle Komponenten verwenden ausschließlich semantische Spacing Tokens.

Beispiel:

```text
Dialog

↓

Spacing.Large

↓

Theme

↓

Abstand
```

Direkte Pixelwerte innerhalb von Komponenten sind nicht zulässig.

---

# 7. Spacing Tokens

Pflichttokens:

```text
Spacing.XXS

Spacing.XS

Spacing.S

Spacing.M

Spacing.L

Spacing.XL

Spacing.XXL

Spacing.XXXL
```

Die tatsächlichen Werte werden ausschließlich durch das Theme festgelegt.

---

# 8. Innenabstände

Innenabstände (Padding) definieren den Abstand zwischen Inhalt und Begrenzung einer Komponente.

Beispiele:

- Dialoge
- Buttons
- Karten
- Eingabefelder
- Listen

Padding wird ausschließlich über Tokens definiert.

---

# 9. Außenabstände

Außenabstände (Margin) definieren den Abstand zwischen benachbarten Komponenten.

Margin-Werte werden semantisch beschrieben und nicht direkt angegeben.

---

# 10. Komponentenabstände

Zwischen zusammengehörenden Komponenten gelten definierte Mindestabstände.

Beispiele:

- Label → Textfeld
- Button → Button
- Titel → Inhalt
- Liste → Liste

Diese Abstände werden durch Layoutregeln bestimmt.

---

# 11. Seitenraster

Das Layout basiert auf einem konsistenten Raster.

Das Raster definiert:

- Standardabstände
- Ausrichtung
- Komponentenpositionierung
- Inhaltsgruppen

Alle Layouts orientieren sich an diesem Raster.

---

# 12. Hierarchische Abstände

Größere Informationsgruppen erhalten größere Abstände als zusammengehörige Elemente.

Beispiel:

```text
Seite

↓

Abschnitt

↓

Gruppe

↓

Steuerelement
```

Die Abstände nehmen entsprechend der Hierarchie ab.

---

# 13. Responsive Skalierung

Das Spacing System unterstützt unterschiedliche Auflösungen.

Die semantischen Tokens bleiben unverändert.

Das Theme kann abhängig von der Bildschirmgröße andere Werte bereitstellen.

---

# 14. Accessibility

Das System unterstützt:

- größere Zielbereiche
- größere Innenabstände
- verbesserte Lesbarkeit
- Touchoptimierung

Accessibility kann Spacing Tokens überschreiben.

---

# 15. Themeintegration

Themes definieren ausschließlich:

- Tokenwerte
- Skalierungsregeln
- Mindestabstände

Komponenten bleiben unverändert.

---

# 16. Validierung

Vor der Aktivierung eines Themes werden geprüft:

- vollständige Spacing Tokens
- ungültige Werte
- doppelte Tokens
- fehlende Kategorien

Ungültige Definitionen werden abgelehnt.

---

# 17. Sicherheit

Das System verhindert:

- negative Abstände
- ungültige Tokenreferenzen
- doppelte Token-IDs
- inkonsistente Layoutwerte

Alle Token werden validiert.

---

# 18. Speicherverwaltung

Das Spacing System verwendet:

- statische Tokentabellen
- Theme-Puffer
- Layout-Pools

Heap-Allokationen während der Laufzeit sind verboten.

---

# 19. Performance

Anforderungen:

- O(1)-Token Lookup
- O(1)-Layout Lookup
- O(1)-Theme Lookup
- deterministische Laufzeit

Spacing Tokens werden beim Theme-Laden vorbereitet.

---

# 20. API

```c
typedef enum
{
    NOVA_SPACING_XXS,
    NOVA_SPACING_XS,
    NOVA_SPACING_S,
    NOVA_SPACING_M,
    NOVA_SPACING_L,
    NOVA_SPACING_XL,
    NOVA_SPACING_XXL,
    NOVA_SPACING_XXXL
}
nova_spacing_token_t;

uint16_t
nova_spacing_get(
    nova_spacing_token_t token);

bool
nova_spacing_exists(
    nova_spacing_token_t token);

nova_result_t
nova_spacing_reload(void);
```

---

# 21. Diagnosedaten

Zu erfassen sind:

- Theme-ID
- Anzahl Spacing Tokens
- Layoutfehler
- Validierungsfehler
- Ladezeit

---

# 22. Testfälle

Zu testen sind:

- Padding
- Margin
- Layoutgruppen
- Dialoge
- Listen
- Buttons
- Themewechsel
- Accessibility
- Responsive Skalierung
- Fehlerfälle

---

# 23. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- sämtliche Abstände ausschließlich über Spacing Tokens definiert werden.
- Layouts konsistente Abstände verwenden.
- Themewechsel ohne Änderungen an Komponenten möglich sind.
- Accessibility Spacing Tokens überschreiben kann.
- Responsive Skalierung unterstützt wird.
- ungültige Abstände erkannt werden.
- keine Heap-Allokationen während der Laufzeit stattfinden.
- BIOS- und UEFI-Systeme identisches Layout erzeugen.

---

# 24. Referenzen

## ADRs

- ADR-BOOTDESIGN-0005 – Spacing System
- ADR-BOOTDESIGN-0002 – Design Tokens
- ADR-BOOTLAYOUT-0001 – Layout Engine
- ADR-BOOTTHEME-0001 – Theme System

## NPSPECs

- NPSPEC-BOOTDESIGN-0001 – Boot Design Language
- NPSPEC-BOOTDESIGN-0002 – Boot Design Tokens
- NPSPEC-BOOTLAYOUT-0001 – Boot Layout Engine
- NPSPEC-BOOTCONTROL-0003 – Control Styling
- NPSPEC-BOOTTHEME-0001 – Theme System

---

# 25. Zusammenfassung

Das **Spacing System** definiert die semantische Abstandsarchitektur der NovaOS-Bootoberfläche. Es beschreibt Innen- und Außenabstände, Layoutabstände sowie Rasterstrukturen ausschließlich über Spacing Tokens und trennt dadurch Layoutdefinitionen konsequent von ihrer Implementierung. Durch zentrale Tokenverwaltung, Themeintegration, Responsive Skalierung, Accessibility-Unterstützung und statische Speicherverwaltung gewährleistet das System eine konsistente, übersichtliche und langfristig wartbare Gestaltung aller Bootoberflächen auf BIOS- und UEFI-basierten Systemen.