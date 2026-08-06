# NPSPEC-BOOTRESOURCE-0012
# Resource Memory Cache

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTRESOURCE-0012 |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot UI – Resource System |
| Priorität | Kritisch |
| Gültigkeitsbereich | NovaOS Boot Manager |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTRESOURCE-0001, NPSPEC-BOOTRESOURCE-0003, NPSPEC-BOOTRESOURCE-0004, NPSPEC-BOOTMEMORY-0001, NPSPEC-BOOTRESOURCE-0009, NPSPEC-BOOTRESOURCE-0010, NPSPEC-BOOTRESOURCE-0011 |

---

# 1. Ziel

Diese Spezifikation definiert den **Resource Memory Cache** der NovaOS-Bootoberfläche.

Der Resource Memory Cache verwaltet alle bereits geladenen Ressourcen im Arbeitsspeicher und stellt sicher, dass identische Ressourcen nur einmal vorhanden sind. Dadurch werden Speicherverbrauch reduziert, Ladezeiten verkürzt und wiederholte Zugriffe beschleunigt.

Der Cache arbeitet transparent für alle Komponenten oberhalb des Resource Managers.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für:

- Bilder
- Icons
- Schriftarten
- Glyph Atlases
- Themes
- Animationen
- Sprachpakete
- Cursor
- Binärressourcen

---

# 3. Ziele

Der Resource Memory Cache muss:

- Mehrfachkopien verhindern
- deterministisch arbeiten
- Referenzzählung unterstützen
- Speicherverbrauch minimieren
- schnellen Zugriff ermöglichen
- Cache-Richtlinien unterstützen

---

# 4. Architektur

```text
Resource Request

↓

Resource Manager

↓

Memory Cache

├── Lookup
├── Reference Manager
├── Cache Policy
└── Memory Pool

↓

Resource
```

---

# 5. Komponenten

Der Resource Memory Cache besteht aus:

- Cache Manager
- Cache Registry
- Reference Manager
- Cache Policy Engine
- Memory Pool
- Statistics Manager

---

# 6. Grundprinzip

Der Ablauf erfolgt wie folgt:

```text
Resource Request

↓

Cache Lookup

↓

Treffer?

↓

Ja

↓

Referenz erhöhen

↓

Nein

↓

Laden

↓

Registrieren

↓

Cache
```

Jede Ressource darf höchstens einmal im Cache vorhanden sein.

---

# 7. Cache-Eintrag

Ein Cache-Eintrag enthält mindestens:

- Resource-ID
- Ressourcentyp
- Speicheradresse
- Größe
- Referenzzähler
- Zeitstempel
- Cache-Status

---

# 8. Referenzverwaltung

Jede Ressource besitzt einen Referenzzähler.

Ablauf:

```text
Load

↓

Reference +1

↓

Release

↓

Reference -1

↓

0

↓

Freigabe
```

Ressourcen mit Referenzzähler `0` können entsprechend der Cache-Richtlinie entfernt werden.

---

# 9. Cache-Richtlinien

Unterstützt werden:

- Permanent
- LRU (Least Recently Used)
- LFU (Least Frequently Used)

Standard:

```text
LRU
```

Kritische Ressourcen können als permanent markiert werden.

---

# 10. Permanenter Cache

Folgende Ressourcen sollen dauerhaft im Speicher verbleiben:

- aktives Theme
- Standardschriftarten
- Glyph Atlas
- Systemicons
- Standarddialoge

Diese Ressourcen werden nicht automatisch freigegeben.

---

# 11. Lazy Loading

Nicht vorhandene Ressourcen werden erst beim ersten Zugriff geladen.

Nach erfolgreichem Laden werden sie unmittelbar im Cache registriert.

---

# 12. Cache-Konsistenz

Eine Ressource besitzt:

- genau eine Speicherinstanz
- genau eine Resource-ID
- genau einen Registry-Eintrag

Mehrfachregistrierungen sind unzulässig.

---

# 13. Themewechsel

Bei einem Themewechsel:

```text
Altes Theme

↓

Referenzen reduzieren

↓

Nicht verwendete Ressourcen freigeben

↓

Neue Ressourcen laden
```

Gemeinsam genutzte Ressourcen bleiben erhalten.

---

# 14. Speichergrenzen

Der Cache besitzt:

- minimale Größe
- maximale Größe
- reservierten Bereich
- kritischen Bereich

Die Grenzwerte werden systemweit konfiguriert.

---

# 15. Speicherbereinigung

Bei Speicherdruck:

```text
LRU

↓

LFU

↓

Freie Ressourcen

↓

Speicher freigeben
```

Permanente Ressourcen bleiben erhalten.

---

# 16. Sicherheit

Das System verhindert:

- doppelte Cache-Einträge
- ungültige Referenzen
- doppelte Resource-IDs
- Speicherkorruption
- Integerüberläufe
- Buffer Overflows

Alle Cacheeinträge werden validiert.

---

# 17. Speicherverwaltung

Der Resource Memory Cache verwendet:

- statische Cachetabellen
- feste Objektpools
- Memory Pools

Heap-Allokationen während des laufenden Betriebs sind verboten.

---

# 18. Performance

Anforderungen:

- O(1)-Cache Lookup
- O(1)-Referenzverwaltung
- O(1)-Einfügen
- O(1)-Entfernen

Der Cache muss vollständig deterministisch arbeiten.

---

# 19. API

```c
typedef struct
{
    uint64_t resource_id;
    void* memory;
    uint64_t size;
    uint32_t reference_count;
}
nova_cache_entry_t;

nova_result_t
nova_resource_cache_initialize(void);

nova_cache_entry_t*
nova_resource_cache_lookup(
    uint64_t resource_id);

nova_result_t
nova_resource_cache_insert(
    nova_cache_entry_t* entry);

nova_result_t
nova_resource_cache_release(
    uint64_t resource_id);

void
nova_resource_cache_collect(void);
```

---

# 20. Diagnosedaten

Zu erfassen sind:

- Cachegröße
- belegter Speicher
- freie Speicherblöcke
- Cache-Hits
- Cache-Misses
- Referenzanzahl
- Freigaben
- Speicherbereinigungen

---

# 21. Testfälle

Zu testen sind:

- Cache Lookup
- Referenzzählung
- LRU
- LFU
- permanente Ressourcen
- Themewechsel
- Lazy Loading
- Speichergrenzen
- Cachebereinigung
- Fehlerfälle

---

# 22. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- jede Ressource höchstens einmal im Cache vorhanden ist.
- Cache-Hits korrekt erkannt werden.
- Referenzzählung zuverlässig funktioniert.
- LRU- und LFU-Richtlinien korrekt arbeiten.
- permanente Ressourcen nicht entfernt werden.
- Themewechsel korrekt verarbeitet werden.
- keine Heap-Allokationen während der Laufzeit stattfinden.
- BIOS- und UEFI-Systeme identisches Verhalten erzeugen.

---

# 23. Referenzen

## ADRs

- ADR-BOOTRESOURCE-0012 – Resource Memory Cache
- ADR-BOOTRESOURCE-0001 – Resource Manager
- ADR-BOOTMEMORY-0001 – Memory Management
- ADR-BOOTRESOURCE-0004 – Resource Loading

## NPSPECs

- NPSPEC-BOOTRESOURCE-0001 – Boot Resource Manager
- NPSPEC-BOOTRESOURCE-0003 – Resource Index
- NPSPEC-BOOTRESOURCE-0004 – Resource Loading
- NPSPEC-BOOTMEMORY-0001 – Boot Memory Management
- NPSPEC-BOOTRESOURCE-0009 – Font Resources
- NPSPEC-BOOTRESOURCE-0010 – Animation Resources
- NPSPEC-BOOTRESOURCE-0011 – Theme Resources

---

# 24. Zusammenfassung

Der **Resource Memory Cache** definiert die zentrale Zwischenspeicherung sämtlicher geladener Ressourcen innerhalb der NovaOS-Bootoberfläche. Er verhindert Mehrfachkopien, verwaltet Referenzzählungen, unterstützt verschiedene Cache-Richtlinien und optimiert Speicherverbrauch sowie Zugriffszeiten. Durch deterministische Algorithmen, zentrale Cacheverwaltung, statische Speicherpools und Integration in den Resource Manager bildet der Resource Memory Cache die Grundlage für eine performante und konsistente Ressourcenverwaltung auf BIOS- und UEFI-basierten Systemen.