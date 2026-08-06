# NPSPEC-BOOTPERF-0003
# Memory Budget

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTPERF-0003 |
| Titel | Memory Budget |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot Performance |
| Priorität | Kritisch |
| Gültigkeitsbereich | Gesamte NovaOS Boot UI |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTPERF-0001, NPSPEC-BOOTRESOURCE-0001, NPSPEC-BOOTTEXT-0006, NPSPEC-BOOTRENDER-0001, NPSPEC-BOOTCOMPOSITOR-0001 |

---

# 1. Ziel

Diese Spezifikation definiert das **Memory Budget** der NovaOS Bootoberfläche.

Ziel ist die kontrollierte und deterministische Nutzung des verfügbaren Arbeitsspeichers während der gesamten Pre-Boot-Phase. Jeder Speicherbereich besitzt ein fest definiertes Budget, wodurch Speicherengpässe, Fragmentierung und unkontrolliertes Wachstum verhindert werden.

Die Boot UI muss auch auf Systemen mit geringer Speicherausstattung zuverlässig arbeiten.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für:

- Boot Manager
- Recovery
- Installer
- Self-Healing
- Diagnosemodule
- Rendering Engine
- Text Engine
- Resource Manager
- Animation Engine
- Compositor

---

# 3. Ziele

Das Memory Budget muss:

- Speicherverbrauch begrenzen
- Fragmentierung vermeiden
- deterministische Speicherbelegung ermöglichen
- Objektpools verwenden
- Heap-Allokationen minimieren
- Diagnosen unterstützen

---

# 4. Speicherprinzipien

Grundregeln:

- Speicherbudgets sind fest definiert.
- Alle Pools werden beim Start reserviert.
- Während des Renderings erfolgen keine Heap-Allokationen.
- Ressourcen werden wiederverwendet.
- Freier Speicher wird zentral verwaltet.

---

# 5. Gesamtbudget

Empfohlenes Gesamtbudget für die Boot UI:

| Plattform | Budget |
|-----------|--------:|
| Minimal | 32 MiB |
| Standard | 64 MiB |
| Komfort | 128 MiB |

Das Budget ist konfigurierbar und richtet sich nach der verfügbaren Hardware.

---

# 6. Speicheraufteilung

| Bereich | Budget |
|----------|--------:|
| Rendering | 8 MiB |
| Framebuffer (zusätzlich) | plattformabhängig |
| Resource Cache | 12 MiB |
| Glyph Cache | 4 MiB |
| SVG Cache | 4 MiB |
| Theme Cache | 1 MiB |
| Animation Pool | 2 MiB |
| Control Pool | 2 MiB |
| Layout Pool | 2 MiB |
| Diagnose | 2 MiB |
| Reserve | 27 MiB |

---

# 7. Objektpools

Folgende Objektpools sind verpflichtend:

- Controls
- Fenster
- Dialoge
- Animationen
- Events
- Listen
- Tree Nodes
- Layoutobjekte
- Draw Commands

Pools besitzen feste Größen und wachsen während der Laufzeit nicht dynamisch.

---

# 8. Resource Cache

Der Resource Cache enthält:

- PNG
- SVG
- Icons
- Themes
- Shaderdaten
- Lokalisierungen

Nicht verwendete Ressourcen können nach einer definierten Inaktivitätszeit freigegeben werden.

---

# 9. Glyph Cache

Der Glyph Cache enthält:

- häufig verwendete Glyphen
- Unicode-Blöcke
- Fontvarianten
- Skalierungsstufen

Glyphen werden nur einmal rasterisiert.

---

# 10. SVG Cache

Der SVG Cache speichert:

- gerenderte Icons
- Vektorgrafiken
- Rasterisierungsstufen
- Transformationsvarianten

Mehrfaches Rasterisieren identischer Ressourcen ist zu vermeiden.

---

# 11. Theme Cache

Der Theme Cache enthält:

- Farben
- Typografie
- Abstände
- Radien
- Schatten
- Materialien

Themewechsel dürfen keine vollständige Neuerzeugung der Datenstrukturen erfordern.

---

# 12. Layoutspeicher

Layoutinformationen umfassen:

- Positionen
- Größen
- Constraints
- Transformationsdaten

Layoutdaten werden nur bei Änderungen aktualisiert.

---

# 13. Draw Command Buffer

Alle Renderbefehle werden in einem vorallokierten Draw Command Buffer gespeichert.

Anforderungen:

- feste Kapazität
- sequentieller Zugriff
- keine Reallokation
- Zurücksetzen pro Frame

---

# 14. Speicherfreigabe

Freigabe erfolgt ausschließlich:

- beim Schließen eines Moduls
- beim Themewechsel
- beim Beenden der Bootoberfläche
- durch Cache-Eviction

Eine Fragmentierung des Speichers ist zu vermeiden.

---

# 15. Heap-Nutzung

Heap-Allokationen sind zulässig:

- während der Initialisierung
- beim Laden neuer Module
- während der Diagnoseroutinen

Während der regulären UI-Laufzeit sind Heap-Allokationen und Heap-Freigaben untersagt.

---

# 16. Cache-Eviction

Der Resource Manager unterstützt:

- LRU (Least Recently Used)
- Prioritätsklassen
- feste Reservierungen
- geschützte Ressourcen

Kritische Ressourcen dürfen nicht automatisch entfernt werden.

---

# 17. Speicherüberwachung

Permanent überwacht werden:

- Gesamtauslastung
- Pool-Auslastung
- Cache-Auslastung
- Fragmentierung
- Peak-Verbrauch
- Reserven

---

# 18. Überlastverhalten

Bei Überschreiten des Budgets werden schrittweise Maßnahmen aktiviert:

1. Freigabe ungenutzter Ressourcen
2. Reduzierung der Cache-Größe
3. Vereinfachung von Animationen
4. Verringerung der SVG-Rasterstufen
5. Deaktivierung optionaler Effekte

Der Bootvorgang darf dadurch nicht unterbrochen werden.

---

# 19. Speicherklassen

| Klasse | Bedeutung |
|---------|-----------|
| Permanent | gesamte Laufzeit |
| Session | bis Modulende |
| Frame | ein Frame |
| Cache | wiederverwendbar |
| Temporär | kurzfristig |

Jeder Speicherblock besitzt genau eine Klasse.

---

# 20. Sicherheit

Speicherbereiche mit sensiblen Informationen müssen:

- überschrieben werden
- gegen unbefugten Zugriff geschützt sein
- nach Freigabe nicht wiederverwendbare Daten enthalten

Dies betrifft insbesondere:

- Passphrasen
- kryptographische Schlüssel
- TPM-Daten
- Recovery-Schlüssel

---

# 21. API

```c
typedef struct
{
    uint64_t total_budget;
    uint64_t used_memory;
    uint64_t free_memory;
    uint64_t cache_memory;
    uint64_t pool_memory;
    uint64_t peak_memory;
}
nova_memory_budget_t;

nova_result_t
nova_memory_budget_initialize(void);

const nova_memory_budget_t*
nova_memory_budget_status(void);

bool
nova_memory_budget_available(
    uint64_t bytes);

nova_result_t
nova_memory_budget_reset(void);
```

---

# 22. Diagnosedaten

Zu protokollieren sind:

- Gesamtspeicher
- belegter Speicher
- Spitzenverbrauch
- Pool-Auslastung
- Cache-Auslastung
- Fragmentierung
- Cache-Evictions
- Budgetüberschreitungen

---

# 23. Testfälle

Zu testen sind:

- Minimalbudget (32 MiB)
- Standardbudget (64 MiB)
- Komfortbudget (128 MiB)
- Themewechsel
- große Glyph Caches
- viele SVG-Ressourcen
- Recovery-Modus
- Installer
- Speicherknappheit
- Cache-Evictions

---

# 24. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- sämtliche Speicherbereiche feste Budgets besitzen.
- während der regulären UI-Laufzeit keine Heap-Allokationen erfolgen.
- alle Objektpools vorallokiert sind.
- Cache-Evictions deterministisch erfolgen.
- sensible Speicherbereiche nach der Nutzung sicher gelöscht werden.
- Budgetüberschreitungen erkannt und protokolliert werden.
- die Bootoberfläche innerhalb des konfigurierten Gesamtbudgets arbeitet.
- sämtliche Speicherkennzahlen über die Diagnose-API verfügbar sind.

---

# 25. Referenzen

## ADRs

- ADR-BOOTPERF-0003 – Memory Budget
- ADR-BOOTRESOURCE-0001 – Resource Manager
- ADR-BOOTRENDER-0001 – Rendering Engine

## NPSPECs

- NPSPEC-BOOTPERF-0001 – Boot UI Performance Requirements
- NPSPEC-BOOTRESOURCE-0001 – Boot Resource Manager
- NPSPEC-BOOTTEXT-0006 – Glyph Atlas
- NPSPEC-BOOTRENDER-0001 – Rendering Engine
- NPSPEC-BOOTCOMPOSITOR-0001 – Boot Compositor

---

# 26. Zusammenfassung

Das **Memory Budget** definiert die vollständige Speicherstrategie der NovaOS-Bootoberfläche. Durch feste Speicherbudgets, vorallokierte Objektpools, spezialisierte Caches und den Verzicht auf Heap-Allokationen während der regulären Laufzeit wird eine deterministische, ressourcenschonende und fragmentierungsfreie Speicherverwaltung erreicht. Das System überwacht kontinuierlich den Speicherverbrauch, reagiert kontrolliert auf Budgetüberschreitungen und gewährleistet gleichzeitig die sichere Behandlung sensibler Daten.