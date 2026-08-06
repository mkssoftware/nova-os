# NPSPEC-BOOTRESOURCE-0011
# Theme Resources

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTRESOURCE-0011 |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot UI – Resource System |
| Priorität | Kritisch |
| Gültigkeitsbereich | NovaOS Boot Manager |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTRESOURCE-0001, NPSPEC-BOOTRESOURCE-0002, NPSPEC-BOOTRESOURCE-0004, NPSPEC-BOOTTHEME-0001, NPSPEC-BOOTTHEME-0002, NPSPEC-BOOTTHEME-0003 |

---

# 1. Ziel

Diese Spezifikation definiert die Verwaltung von **Theme Resources** innerhalb der NovaOS-Bootoberfläche.

Theme Resources stellen sämtliche visuellen Eigenschaften der Bootoberfläche bereit. Dazu gehören Farben, Typografie, Icons, Animationen, Effekte, Abstände und weitere Designparameter. UI-Komponenten greifen ausschließlich über semantische Theme-Definitionen auf diese Ressourcen zu.

Dadurch können unterschiedliche Designs bereitgestellt werden, ohne Änderungen an der Programmlogik vorzunehmen.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für:

- Farbschemata
- Typography Tokens
- Icons
- Animationen
- Hintergründe
- Effekte
- Control Styles
- Layoutparameter

---

# 3. Ziele

Das Theme-System muss:

- zentrale Themeverwaltung ermöglichen
- semantische Tokens verwenden
- deterministisch arbeiten
- Themewechsel unterstützen
- Erweiterbarkeit gewährleisten
- Konsistenz der Benutzeroberfläche sicherstellen

---

# 4. Architektur

```text
Boot Asset Package

↓

Theme Resource

↓

Theme Manager

↓

Theme Resolver

↓

UI Controls

↓

Renderer
```

---

# 5. Komponenten

Das System besteht aus:

- Theme Loader
- Theme Registry
- Theme Resolver
- Token Resolver
- Validation Engine
- Theme Cache

---

# 6. Grundprinzip

Der Ablauf erfolgt wie folgt:

```text
Theme Resource

↓

Theme Manager

↓

Semantische Tokens

↓

UI-Komponenten

↓

Renderer
```

Die Benutzeroberfläche kennt ausschließlich semantische Token und keine festen Farben oder Werte.

---

# 7. Theme Resource

Ein Theme enthält mindestens:

- Theme-ID
- Name
- Version
- Autor
- Farbschema
- Typography Tokens
- Icon-Referenzen
- Animations-Referenzen
- Layoutparameter

---

# 8. Farbschema

Ein Theme definiert unter anderem:

- Hintergrundfarben
- Vordergrundfarben
- Primärfarbe
- Sekundärfarbe
- Akzentfarbe
- Warnfarbe
- Fehlerfarbe
- Erfolgsfarbe

Alle Farben werden als semantische Rollen definiert.

---

# 9. Typografie

Ein Theme verweist auf:

- Font Resources
- Typography Tokens
- Zeilenhöhen
- Letter Spacing
- Schriftgewichte

Die tatsächlichen Schriftdateien werden über den Resource Manager bereitgestellt.

---

# 10. Icons

Icons werden ausschließlich über Resource-IDs referenziert.

Das Theme kennt keine Dateipfade.

Alle Icongrößen werden durch den SVG Renderer oder Bitmap-Ressourcen bereitgestellt.

---

# 11. Animationen

Ein Theme verweist auf:

- Seitenanimationen
- Fokusanimationen
- Dialoganimationen
- Statusanimationen
- Übergänge

Animationen werden durch Animation Resources beschrieben.

---

# 12. Layoutparameter

Ein Theme kann definieren:

- Abstände
- Innenabstände
- Außenabstände
- Eckradien
- Schattenparameter
- Unschärfen
- Transparenzwerte

Diese Werte werden ausschließlich als semantische Design-Tokens gespeichert.

---

# 13. Themewechsel

Ein Themewechsel erfolgt nach folgendem Ablauf:

```text
Theme laden

↓

Validieren

↓

Registry aktualisieren

↓

UI aktualisieren

↓

Rendering
```

Ein Neustart des Boot Managers ist nicht erforderlich.

---

# 14. Theme Registry

Alle geladenen Themes werden registriert.

Die Registry verwaltet:

- Theme-ID
- Version
- Status
- Referenzzähler

Es kann jeweils genau ein aktives Theme geben.

---

# 15. Sicherheit

Das System verhindert:

- doppelte Theme-IDs
- beschädigte Theme-Dateien
- fehlende Ressourcen
- rekursive Theme-Abhängigkeiten
- ungültige Tokenreferenzen

Alle Theme Resources werden vor der Aktivierung vollständig validiert.

---

# 16. Speicherverwaltung

Das Theme-System verwendet:

- statische Theme-Puffer
- Token-Pools
- Objektpools

Heap-Allokationen während der Laufzeit sind verboten.

---

# 17. Performance

Anforderungen:

- O(1)-Theme Lookup
- O(1)-Token Lookup
- O(1)-Resource Lookup
- O(n)-Themeinitialisierung

Themewechsel sollen ausschließlich betroffene Ressourcen neu auflösen.

---

# 18. API

```c
typedef struct
{
    uint32_t theme_id;
    uint32_t version;
    const char* name;
}
nova_theme_resource_t;

nova_result_t
nova_theme_resource_initialize(void);

const nova_theme_resource_t*
nova_theme_resource_load(
    uint32_t resource_id);

nova_result_t
nova_theme_activate(
    uint32_t theme_id);

const nova_theme_resource_t*
nova_theme_current(void);
```

---

# 19. Diagnosedaten

Zu erfassen sind:

- Theme-ID
- Themeversion
- Anzahl Tokens
- Anzahl Ressourcen
- Ladezeit
- Aktivierungszeit
- Validierungsfehler
- Speicherverbrauch

---

# 20. Testfälle

Zu testen sind:

- Theme laden
- Themewechsel
- Farben
- Typography Tokens
- Icons
- Animationen
- beschädigte Themes
- fehlende Ressourcen
- doppelte IDs
- Fehlerfälle

---

# 21. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- sämtliche visuellen Eigenschaften ausschließlich über Theme Resources bereitgestellt werden.
- Themewechsel ohne Neustart möglich sind.
- semantische Tokens korrekt aufgelöst werden.
- fehlende Ressourcen erkannt werden.
- beschädigte Themes nicht aktiviert werden.
- doppelte Theme-IDs verhindert werden.
- keine Heap-Allokationen während der Laufzeit stattfinden.
- BIOS- und UEFI-Systeme identisches Verhalten erzeugen.

---

# 22. Referenzen

## ADRs

- ADR-BOOTRESOURCE-0011 – Theme Resources
- ADR-BOOTTHEME-0001 – Theme System
- ADR-BOOTTHEME-0002 – Theme Tokens
- ADR-BOOTTHEME-0003 – Theme Loading

## NPSPECs

- NPSPEC-BOOTRESOURCE-0001 – Boot Resource Manager
- NPSPEC-BOOTRESOURCE-0002 – Boot Asset Package
- NPSPEC-BOOTRESOURCE-0004 – Resource Loading
- NPSPEC-BOOTTHEME-0001 – Theme System
- NPSPEC-BOOTTHEME-0002 – Theme Tokens
- NPSPEC-BOOTTHEME-0003 – Theme Loading

---

# 23. Zusammenfassung

Die **Theme Resources** definieren die standardisierte Bereitstellung sämtlicher visueller Eigenschaften der NovaOS-Bootoberfläche. Sie bündeln Farben, Typografie, Icons, Animationen und Layoutparameter in einer zentralen, semantischen Ressource, die vollständig über den Theme Manager verwaltet wird. Durch Themewechsel ohne Neustart, Token-basierte Konfiguration, zentrale Validierung und statische Speicherverwaltung ermöglichen Theme Resources eine konsistente, flexible und erweiterbare Gestaltung der Bootoberfläche auf BIOS- und UEFI-basierten Systemen.