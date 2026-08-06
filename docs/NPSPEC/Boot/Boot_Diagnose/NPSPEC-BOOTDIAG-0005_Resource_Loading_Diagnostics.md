# NPSPEC-BOOTDIAG-0005
# Resource Loading Diagnostics

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTDIAG-0005 |
| Titel | Resource Loading Diagnostics |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot Diagnostics |
| Priorität | Hoch |
| Gültigkeitsbereich | Resource Manager, Rendering Engine, Theme Engine, Text Engine, Boot UI |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTDIAG-0001, NPSPEC-BOOTRESOURCE-0001, NPSPEC-BOOTRESOURCE-0004, NPSPEC-BOOTRESOURCE-0012, NPSPEC-BOOTPERF-0004 |

---

# 1. Ziel

Diese Spezifikation definiert das **Resource Loading Diagnostics Framework** der NovaOS Bootumgebung.

Das Framework überwacht sämtliche Ressourcenladevorgänge der Bootoberfläche und ermöglicht eine vollständige Analyse von Ladezeiten, Cache-Nutzung, Integritätsprüfungen sowie Fehlern beim Laden von Assets.

Die Diagnose arbeitet ausschließlich passiv und beeinflusst den eigentlichen Ladevorgang nicht.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für:

- Resource Manager
- PNG Decoder
- SVG Renderer
- Font Loader
- Theme Loader
- Animation Loader
- Localization Loader
- Boot Manager
- Recovery
- Installer

---

# 3. Ziele

Das Framework muss:

- alle Ressourcenladevorgänge erfassen
- Ladezeiten messen
- Cache-Nutzung überwachen
- Integritätsprüfungen dokumentieren
- deterministisch arbeiten
- modular erweiterbar sein

---

# 4. Architektur

Das Framework besteht aus:

- Resource Trace Collector
- Load Time Recorder
- Cache Monitor
- Integrity Monitor
- Resource Statistics
- Export Service
- Diagnostics API

Alle Komponenten arbeiten unabhängig vom eigentlichen Resource Manager.

---

# 5. Überwachte Ressourcen

Folgende Ressourcentypen werden unterstützt:

- PNG
- SVG
- Fonts
- Themes
- Animationen
- Lokalisierungen
- Konfigurationsdateien
- Audio (zukünftig)
- Videos (zukünftig)

---

# 6. Ladeereignisse

Für jede Ressource werden erfasst:

- Ladeanforderung
- Ladevorgang gestartet
- Integritätsprüfung
- Dekomprimierung
- Cache-Zugriff
- Bereitstellung
- Freigabe
- Fehler

---

# 7. Ressourcenstruktur

Jeder Ressourceneintrag enthält:

- Resource-ID
- Name
- Ressourcentyp
- Version
- Größe
- Komprimierungsverfahren
- Integritätsstatus
- Cache-Status

---

# 8. Zeitmessung

Für jede Ressource werden gemessen:

- Anforderungszeit
- Ladebeginn
- Ladeende
- Dekomprimierungszeit
- Validierungszeit
- Cache-Zeit
- Gesamtladezeit

---

# 9. Cache-Diagnose

Für jeden Cache werden erfasst:

- Cache-Hits
- Cache-Misses
- Evictions
- Speicherverbrauch
- Auslastung
- durchschnittliche Zugriffszeit

Überwachte Caches:

- Resource Cache
- Theme Cache
- Glyph Cache
- SVG Cache
- Animation Cache

---

# 10. Integritätsprüfung

Zu protokollieren sind:

- Hashprüfung
- Signaturprüfung
- Versionsprüfung
- Prüfsummenprüfung
- Fehlerursachen

Beschädigte Ressourcen werden eindeutig markiert.

---

# 11. Komprimierungsdiagnose

Für komprimierte Ressourcen werden erfasst:

- Komprimierungsverfahren
- Originalgröße
- Komprimierte Größe
- Dekomprimierungsdauer
- Kompressionsrate

---

# 12. Ressourcenabhängigkeiten

Das Framework verfolgt:

- Parent Resource
- Child Resources
- Theme-Abhängigkeiten
- Font-Abhängigkeiten
- Animation-Abhängigkeiten

Abhängigkeiten können rekonstruiert werden.

---

# 13. Fehlererkennung

Das Framework erkennt:

- fehlende Ressourcen
- beschädigte Dateien
- Integritätsfehler
- Versionskonflikte
- Cache-Korruption
- Lade-Timeouts
- Dekomprimierungsfehler

---

# 14. Speicherstatistiken

Erfasst werden:

- Ressourcengröße
- Cachegröße
- belegter Speicher
- Peak-Verbrauch
- Freigaben
- temporärer Speicher

---

# 15. Performancekennzahlen

Zu messen sind:

- Ressourcen pro Sekunde
- durchschnittliche Ladezeit
- langsamste Ressource
- schnellste Ressource
- Cache-Trefferquote
- Gesamtladezeit

---

# 16. Timeline

Alle Ressourcen werden chronologisch protokolliert.

Jeder Eintrag enthält:

- Zeitstempel
- Ressource
- Aktion
- Ergebnis
- Dauer

Dadurch kann der komplette Ressourcenladeprozess rekonstruiert werden.

---

# 17. Filter

Unterstützte Filter:

- Ressourcentyp
- Ladeergebnis
- Cachestatus
- Zeitbereich
- Bootphase
- Modul
- Integritätsstatus

---

# 18. Export

Unterstützte Formate:

- Nova Diagnostic Format (NDF)
- JSON
- CSV
- Binärformat

Der Export erfolgt ausschließlich nach Benutzeranforderung.

---

# 19. Datenschutz

Diagnosedaten enthalten ausschließlich technische Informationen.

Nicht gespeichert werden:

- Benutzerdaten
- Kennwörter
- Recovery Keys
- kryptographische Schlüssel

---

# 20. Sicherheit

Das Framework darf:

- Ressourcen niemals verändern
- Integritätsprüfungen nicht beeinflussen
- Bootentscheidungen nicht verändern
- Caches nicht manipulieren

Die Diagnose arbeitet ausschließlich lesend.

Zeitintensive Diagnosefunktionen sollen vom normalen Bootpfad getrennt bleiben oder so implementiert werden, dass sie die Bootzeit nur minimal beeinflussen. UEFI sieht für umfangreiche Diagnosen separate Diagnosemechanismen vor. :contentReference[oaicite:0]{index=0}

---

# 21. Speicherverwaltung

Das Framework verwendet:

- Resource Trace Pool
- Statistics Cache
- Ring Buffer
- Export Buffer

Alle Speicherbereiche werden vor dem ersten Ressourcenzugriff reserviert.

Heap-Allokationen während der Laufzeit sind verboten.

---

# 22. API

```c
typedef enum
{
    NOVA_RESOURCE_EVENT_REQUEST,
    NOVA_RESOURCE_EVENT_LOAD_BEGIN,
    NOVA_RESOURCE_EVENT_VALIDATE,
    NOVA_RESOURCE_EVENT_CACHE_HIT,
    NOVA_RESOURCE_EVENT_CACHE_MISS,
    NOVA_RESOURCE_EVENT_LOAD_COMPLETE,
    NOVA_RESOURCE_EVENT_RELEASE,
    NOVA_RESOURCE_EVENT_ERROR
}
nova_resource_event_type_t;

typedef struct
{
    uint64_t timestamp_us;
    uint32_t resource_id;
    nova_resource_event_type_t event;
    uint32_t resource_type;
    uint64_t load_time_us;
    bool cache_hit;
    bool integrity_ok;
}
nova_resource_trace_t;

nova_result_t
nova_resource_diag_initialize(void);

nova_result_t
nova_resource_diag_record(
    const nova_resource_trace_t* trace);

const nova_resource_trace_t*
nova_resource_diag_get(
    uint32_t index);

nova_result_t
nova_resource_diag_export(void);

void
nova_resource_diag_reset(void);
```

---

# 23. Diagnosedaten

Zu protokollieren sind:

- Resource-ID
- Ressourcentyp
- Ladezeit
- Dekomprimierungszeit
- Cache-Hit
- Cache-Miss
- Integritätsstatus
- Speicherverbrauch
- Fehlercode
- Bootphase
- Ladeergebnis

---

# 24. Testfälle

Zu testen sind:

- PNG laden
- SVG laden
- Font laden
- Theme laden
- Animation laden
- beschädigte Ressource
- Integritätsfehler
- Cache-Hit
- Cache-Miss
- Exportfunktion

---

# 25. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- sämtliche Ressourcenladevorgänge vollständig protokolliert werden.
- Ladezeiten und Cache-Zugriffe exakt gemessen werden.
- Integritätsprüfungen nachvollziehbar dokumentiert werden.
- beschädigte oder fehlende Ressourcen eindeutig erkannt werden.
- Export in alle definierten Formate möglich ist.
- das Diagnosesystem den Bootvorgang nicht messbar verlangsamt.
- keine Heap-Allokationen während der Laufzeit stattfinden.
- sämtliche Diagnosedaten über die Resource Diagnostics API verfügbar sind.

---

# 26. Referenzen

## ADRs

- ADR-BOOTDIAG-0005 – Resource Loading Diagnostics
- ADR-BOOTRESOURCE-0001 – Resource Manager
- ADR-BOOTPERF-0004 – Startup-Time Budget

## NPSPECs

- NPSPEC-BOOTDIAG-0001 – Boot UI Diagnostics
- NPSPEC-BOOTRESOURCE-0001 – Boot Resource Manager
- NPSPEC-BOOTRESOURCE-0004 – Resource Loading
- NPSPEC-BOOTRESOURCE-0012 – Resource Memory Cache
- NPSPEC-BOOTPERF-0004 – Startup-Time Budget

---

# 27. Zusammenfassung

Das **Resource Loading Diagnostics Framework** stellt die vollständige Diagnoseinfrastruktur für sämtliche Ressourcenladevorgänge der NovaOS-Bootoberfläche bereit. Es überwacht den gesamten Lebenszyklus einer Ressource – von der Ladeanforderung über Integritätsprüfung, Dekomprimierung und Cache-Nutzung bis zur Freigabe – und erfasst dabei detaillierte Zeit-, Speicher- und Performancekennzahlen. Durch deterministische Speicherverwaltung, Ring-Buffer-Protokollierung und standardisierte Exportfunktionen ermöglicht das Framework eine umfassende Analyse der Ressourcenpipeline, ohne die Stabilität oder Performance des Bootvorgangs zu beeinträchtigen.