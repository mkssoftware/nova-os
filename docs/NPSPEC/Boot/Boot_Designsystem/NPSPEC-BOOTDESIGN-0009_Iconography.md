# NPSPEC-BOOTDESIGN-0009
# Iconography

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTDESIGN-0009 |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot UI – Design System |
| Priorität | Hoch |
| Gültigkeitsbereich | NovaOS Boot Manager |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTDESIGN-0001, NPSPEC-BOOTDESIGN-0002, NPSPEC-BOOTRESOURCE-0008, NPSPEC-BOOTTHEME-0001, NPSPEC-BOOTCONTROL-ICON-0001 |

---

# 1. Ziel

Diese Spezifikation definiert das **Iconography System** der NovaOS-Bootoberfläche.

Das System beschreibt sämtliche Regeln für die Gestaltung, Verwendung und Bereitstellung von Symbolen innerhalb der Bootumgebung. Ziel ist eine konsistente, skalierbare und leicht verständliche Symbolsprache, die unabhängig von Bildschirmauflösung und Theme funktioniert.

Alle Icons werden ausschließlich über semantische Icon Tokens referenziert.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für:

- Menü-Icons
- Toolbar-Icons
- Statussymbole
- Warnsymbole
- Dialogsymbole
- Navigationssymbole
- Aktionssymbole
- Bootlogo-Symbole
- Accessibility-Symbole

---

# 3. Ziele

Das Icon-System muss:

- konsistente Symbolsprache gewährleisten
- SVG bevorzugen
- Themewechsel unterstützen
- deterministisch arbeiten
- hohe Lesbarkeit bieten
- leicht erweiterbar sein

---

# 4. Architektur

```text
Theme

↓

Icon Tokens

↓

SVG Resource

↓

SVG Renderer

↓

Renderer

↓

UI-Komponenten
```

---

# 5. Komponenten

Das System besteht aus:

- Icon Registry
- Icon Resolver
- SVG Renderer
- Theme Integration
- Validation Engine
- Icon Cache

---

# 6. Grundprinzip

Alle Komponenten verwenden ausschließlich Icon Tokens.

Beispiel:

```text
Button.Save

↓

Icon.Save

↓

Theme

↓

SVG Resource

↓

Renderer
```

Direkte Dateipfade oder Dateinamen dürfen nicht verwendet werden.

---

# 7. Icon Tokens

Beispiele:

```text
Icon.Back

Icon.Forward

Icon.Home

Icon.Settings

Icon.Power

Icon.Restart

Icon.Shutdown

Icon.Warning

Icon.Error

Icon.Information

Icon.Success

Icon.User

Icon.Lock

Icon.Search
```

Neue Tokens können ergänzt werden.

---

# 8. Icon-Stil

Alle Icons folgen denselben Gestaltungsregeln:

- klare Geometrie
- einfache Formen
- konsistente Strichstärke
- harmonische Proportionen
- keine unnötigen Details

Die Symbolsprache soll zeitlos wirken.

---

# 9. Größen

Empfohlene Zielgrößen:

```text
16 DLU

20 DLU

24 DLU

32 DLU

48 DLU

64 DLU
```

Die Zielgrößen sind logische Größen in DLU. Die Umrechnung in physische Pixel
erfolgt ausschließlich durch das zentrale Resolution-and-Scaling-System vor
der Rasterisierung.

---

# 10. Farben

Icons verwenden ausschließlich semantische Farbtokens.

Unterstützt werden:

- Primär
- Sekundär
- Akzent
- Fehler
- Warnung
- Erfolg

Direkte Farbwerte im SVG sind möglichst zu vermeiden.

---

# 11. Zustände

Icons unterstützen:

- Normal
- Hover
- Fokus
- Aktiv
- Deaktiviert
- Ausgewählt

Die Darstellung erfolgt über Theme Tokens.

---

# 12. Animation

Icons können animiert werden.

Beispiele:

- Rotation
- Einblenden
- Skalierung
- Farbwechsel
- Statuswechsel

Animationen werden ausschließlich durch die Motion Engine gesteuert.

---

# 13. Accessibility

Das Icon-System unterstützt:

- ausreichende Größen
- hohe Kontraste
- alternative Beschriftungen
- eindeutige Symbolik

Kritische Informationen dürfen niemals ausschließlich durch Icons vermittelt werden.

---

# 14. Themeintegration

Themes dürfen verändern:

- Iconfarben
- Symbolvarianten
- Akzentfarben
- Animationen

Die semantischen Icon Tokens bleiben unverändert.

---

# 15. Validierung

Vor der Aktivierung eines Themes werden geprüft:

- fehlende Icons
- doppelte Icon Tokens
- beschädigte SVG-Dateien
- ungültige Ressourcenreferenzen

Ungültige Icons werden durch Standardicons ersetzt.

---

# 16. Sicherheit

Das System verhindert:

- fehlende Ressourcen
- ungültige SVG-Dateien
- doppelte Icondefinitionen
- rekursive Referenzen

Alle Iconressourcen werden validiert.

---

# 17. Speicherverwaltung

Das Icon-System verwendet:

- Icon Cache
- SVG Cache
- Bitmap Cache
- statische Tokentabellen

Heap-Allokationen während der Laufzeit sind verboten.

---

# 18. Performance

Anforderungen:

- O(1)-Icon Lookup
- O(1)-Token Lookup
- O(1)-Cache Lookup
- deterministische Laufzeit

Gerenderte Icons werden zwischengespeichert.

---

# 19. API

```c
typedef enum
{
    NOVA_ICON_BACK,
    NOVA_ICON_FORWARD,
    NOVA_ICON_HOME,
    NOVA_ICON_SETTINGS,
    NOVA_ICON_POWER,
    NOVA_ICON_WARNING,
    NOVA_ICON_ERROR,
    NOVA_ICON_INFORMATION
}
nova_icon_token_t;

const nova_icon_resource_t*
nova_icon_get(
    nova_icon_token_t token);

bool
nova_icon_exists(
    nova_icon_token_t token);

nova_result_t
nova_icon_reload(void);
```

---

# 20. Diagnosedaten

Zu erfassen sind:

- Theme-ID
- Anzahl Icons
- SVG-Validierungsfehler
- Cache-Hits
- Cache-Misses
- Ladezeit

---

# 21. Testfälle

Zu testen sind:

- SVG-Rendering
- Themewechsel
- Iconfarben
- Animationen
- verschiedene Größen
- Cache
- Accessibility
- beschädigte SVG-Dateien
- fehlende Icons
- Fehlerfälle

---

# 22. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- sämtliche Icons ausschließlich über Icon Tokens referenziert werden.
- SVG als Standardformat verwendet wird.
- Themewechsel ohne Änderungen an Komponenten möglich sind.
- gerenderte Icons im Cache wiederverwendet werden.
- beschädigte Icons erkannt und ersetzt werden.
- Accessibility-Anforderungen erfüllt werden.
- keine Heap-Allokationen während der Laufzeit stattfinden.
- BIOS- und UEFI-Systeme identisches Erscheinungsbild erzeugen.

---

# 23. Referenzen

## ADRs

- ADR-BOOTDESIGN-0009 – Iconography
- ADR-BOOTRESOURCE-0008 – SVG Icon Renderer
- ADR-BOOTTHEME-0001 – Theme System
- ADR-BOOTDESIGN-0002 – Design Tokens

## NPSPECs

- NPSPEC-BOOTDESIGN-0001 – Boot Design Language
- NPSPEC-BOOTDESIGN-0002 – Boot Design Tokens
- NPSPEC-BOOTRESOURCE-0008 – SVG Icon Renderer
- NPSPEC-BOOTTHEME-0001 – Theme System
- NPSPEC-BOOTCONTROL-ICON-0001 – Icon Control

---

# 24. Zusammenfassung

Das **Iconography System** definiert die standardisierte Symbolsprache der NovaOS-Bootoberfläche. Es beschreibt Gestaltung, Referenzierung, Skalierung und Darstellung sämtlicher Icons auf semantischer Basis und nutzt SVG als bevorzugtes Ressourcenformat. Durch Icon Tokens, Themeintegration, SVG-Rendering, Caching und statische Speicherverwaltung gewährleistet das System eine konsistente, hochwertige und langfristig wartbare Symbolik für BIOS- und UEFI-basierte Bootoberflächen.
