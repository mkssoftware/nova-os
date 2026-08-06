# NPSPEC-BOOTRESOURCE-0001
# Boot Resource Manager

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTRESOURCE-0001 |
| Titel | Boot Resource Manager |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot Resource System |
| Priorität | Kritisch |
| Gültigkeitsbereich | Gesamte NovaOS Bootplattform |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTFS-0001, NPSPEC-BOOTRENDER-0001, NPSPEC-BOOTRESOURCE-0002, NPSPEC-BOOTRESOURCE-0003, NPSPEC-BOOTRESOURCE-0004 |

---

# 1. Ziel

Diese Spezifikation definiert den **Boot Resource Manager** der NovaOS-Bootplattform.

Der Boot Resource Manager ist die zentrale Verwaltungsinstanz für sämtliche Ressourcen, die während der Initialisierung und Laufzeit der Bootoberfläche benötigt werden. Er abstrahiert sämtliche Zugriffe auf Grafiken, Schriftarten, Themes, Animationen, Konfigurationen und weitere Assets und stellt diese über eine einheitliche API bereit.

Der Resource Manager sorgt für deterministisches Laden, Caching, Integritätsprüfung und kontrollierte Freigabe aller Bootressourcen.

---

# 2. Geltungsbereich

Der Resource Manager verwaltet unter anderem:

- PNG-Grafiken
- SVG-Dateien
- Icons
- Schriftarten
- Glyph-Atlanten
- Themes
- Animationen
- Design Tokens
- Lokalisierungen
- UI-Layouts
- Konfigurationsdateien
- Cursor
- Audioressourcen (zukünftig)
- Videosequenzen (zukünftig)

---

# 3. Ziele

Der Resource Manager muss:

- eine zentrale Ressourcenverwaltung bereitstellen
- Ressourcen eindeutig identifizieren
- Lazy Loading unterstützen
- Ressourcen cachen
- Integritätsprüfungen durchführen
- deterministisch arbeiten
- zukünftige Ressourcentypen unterstützen

---

# 4. Architektur

Der Resource Manager besteht aus:

- Resource Registry
- Package Loader
- Resource Loader
- Cache Manager
- Integrity Validator
- Dependency Resolver
- Memory Manager
- Diagnostics Interface

Alle Komponenten arbeiten unabhängig voneinander.

---

# 5. Ressourcenmodell

Jede Ressource besitzt:

- Resource-ID
- Typ
- Name
- Version
- Größe
- Prüfsumme
- Herkunft
- Ladezustand
- Speicheradresse
- Referenzzähler

---

# 6. Ressourcenhierarchie

```text
Boot Resource Manager

├── Package Manager
│
├── Image Resources
│
├── Font Resources
│
├── Theme Resources
│
├── Animation Resources
│
├── Localization Resources
│
├── Layout Resources
│
└── Configuration Resources
```

---

# 7. Lebenszyklus

Der Lebenszyklus einer Ressource:

```text
Registriert

↓

Nicht geladen

↓

Geladen

↓

Verifiziert

↓

Im Cache

↓

In Benutzung

↓

Freigegeben

↓

Entladen
```

---

# 8. Ressourcenidentifikation

Jede Ressource besitzt eine eindeutige ID.

Beispiel:

```text
boot://themes/default/button/background
```

Oder:

```text
boot://fonts/inter/regular/16
```

IDs sind unveränderlich.

---

# 9. Resource Registry

Die Registry verwaltet:

- verfügbare Ressourcen
- Versionen
- Pakete
- Abhängigkeiten
- Referenzen
- Prüfsummen

---

# 10. Laden

Unterstützte Modi:

- sofort laden
- Lazy Loading
- Vorladen
- Hintergrundladen (optional)

---

# 11. Cache

Der Cache unterstützt:

- Referenzzählung
- LRU
- Prioritäten
- Speicherbudget
- automatische Freigabe

---

# 12. Abhängigkeiten

Beispiele:

```text
Theme

↓

Font

↓

Glyph Atlas

↓

Icons
```

Abhängigkeiten werden automatisch aufgelöst.

---

# 13. Integritätsprüfung

Vor jeder Verwendung wird geprüft:

- Hash
- Signatur
- Version
- Paketstruktur
- Ressourcentyp

Ungültige Ressourcen werden niemals geladen.

---

# 14. Speicherverwaltung

Der Manager verwendet:

- Objektpools
- Cachepools
- Paketpuffer
- temporäre Ladepuffer

Heap-Allokationen während der normalen Laufzeit sollen vermieden werden.

---

# 15. Fehlerbehandlung

Erkannte Fehler:

- Ressource fehlt
- beschädigte Ressource
- ungültige Signatur
- unbekannter Typ
- Speicherfehler
- Version inkompatibel

---

# 16. Fallback

Falls Ressourcen fehlen:

- Standardtheme
- Standardfont
- Platzhaltericon
- Platzhaltergrafik
- Recovery Theme

---

# 17. Performance

Der Manager muss:

- mehrfaches Laden verhindern
- Ressourcen wiederverwenden
- Ladezeiten minimieren
- Speicher effizient nutzen

---

# 18. Sicherheit

Der Resource Manager:

- lädt ausschließlich verifizierte Ressourcen
- akzeptiert keine unbekannten Formate
- prüft Paketgrenzen
- verhindert Buffer Overflows

---

# 19. Diagnostik

Bereitgestellt werden:

- Anzahl Ressourcen
- Cachegröße
- Speicherverbrauch
- Ladezeiten
- Referenzzähler
- Integritätsstatus
- Fehlerstatistik

---

# 20. Speicherlayout

Reservierte Bereiche:

- Registry
- Cache
- Paketdaten
- Objektpools
- Diagnosedaten

Alle Bereiche werden während der Initialisierung eingerichtet.

---

# 21. API

```c
typedef enum
{
    NOVA_RESOURCE_IMAGE,
    NOVA_RESOURCE_FONT,
    NOVA_RESOURCE_THEME,
    NOVA_RESOURCE_ANIMATION,
    NOVA_RESOURCE_LAYOUT,
    NOVA_RESOURCE_LOCALIZATION,
    NOVA_RESOURCE_CONFIGURATION
}
nova_resource_type_t;

typedef struct
{
    uint64_t id;
    nova_resource_type_t type;
    uint32_t version;
    uint64_t size;
    uint32_t reference_count;
    bool loaded;
    bool verified;
    void* data;
}
nova_resource_t;

nova_result_t
nova_resource_manager_initialize(void);

nova_result_t
nova_resource_load(
    uint64_t id,
    nova_resource_t** resource);

nova_result_t
nova_resource_release(
    nova_resource_t* resource);

const nova_resource_t*
nova_resource_find(
    uint64_t id);

void
nova_resource_manager_shutdown(void);
```

---

# 22. Diagnosedaten

Zu protokollieren sind:

- Anzahl Ressourcen
- geladene Ressourcen
- Cachegröße
- Referenzzähler
- Ladezeiten
- Speicherverbrauch
- Integritätsfehler
- Fallback-Aktivierungen

---

# 23. Testfälle

Zu testen sind:

- Ressourcen laden
- Lazy Loading
- Cache-Hit
- Cache-Miss
- Referenzzählung
- Integritätsprüfung
- beschädigte Ressourcen
- Fallback-Ressourcen
- Speichergrenzen
- parallele Ladeanforderungen

---

# 24. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- sämtliche Ressourcen zentral über den Resource Manager verwaltet werden.
- jede Ressource eindeutig identifizierbar ist.
- Integritätsprüfungen vor jeder Nutzung erfolgen.
- Caching und Referenzzählung korrekt funktionieren.
- Fallback-Ressourcen automatisch verwendet werden.
- Speicherbudgets eingehalten werden.
- alle Diagnosedaten über die API verfügbar sind.
- zukünftige Ressourcentypen ohne Änderungen an bestehenden Komponenten integrierbar sind.

---

# 25. Referenzen

## ADRs

- ADR-BOOTRESOURCE-0001 – Boot Resource Manager

## NPSPECs

- NPSPEC-BOOTFS-0001 – Boot Filesystem
- NPSPEC-BOOTRENDER-0001 – Rendering Engine
- NPSPEC-BOOTRESOURCE-0002 – Boot Asset Package
- NPSPEC-BOOTRESOURCE-0003 – Resource Index
- NPSPEC-BOOTRESOURCE-0004 – Resource Loading

---

# 26. Zusammenfassung

Der **Boot Resource Manager** bildet die zentrale Ressourcenverwaltung der NovaOS-Bootplattform. Er übernimmt das Laden, Verifizieren, Cachen, Verwalten und Freigeben sämtlicher Bootressourcen über eine einheitliche API. Durch deterministische Speicherverwaltung, Integritätsprüfungen, Referenzzählung und definierte Fallback-Mechanismen stellt er sicher, dass alle Komponenten der Bootoberfläche effizient, sicher und konsistent auf benötigte Ressourcen zugreifen können.