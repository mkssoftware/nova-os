# NPSPEC-KERNEL-0006 – Physical Memory Manager

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-KERNEL-0006 |
| Titel | Physical Memory Manager |
| Status | Spezifikation |
| Version | 1.0 |
| Datum | 2026-08-03 |
| Bereich | Kernel / Speicherverwaltung |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | NPSPEC-KERNEL-0001, NPSPEC-KERNEL-0002 |
| Zugehörige ADRs | ADR-MM-0001, ADR-MM-0011, ADR-MM-0012, ADR-MM-0014, ADR-MM-0015 |

---

## 1. Zweck

Diese Spezifikation definiert den Physical Memory Manager, im Folgenden PMM genannt.

Der PMM verwaltet die physischen Speicherrahmen des Systems und stellt sie anderen Kernelkomponenten kontrolliert zur Verfügung.

Zu seinen Aufgaben gehören:

- Übernahme und Normalisierung der Firmware-Speicherkarte,
- Reservierung belegter Speicherbereiche,
- Allokation und Freigabe physischer Seiten,
- Verwaltung zusammenhängender Speicherbereiche,
- Unterstützung von DMA und NUMA,
- Speicher-Hotplug,
- Erkennung und Isolierung fehlerhafter Seiten,
- Speicherstatistik und Diagnose.

## 2. Geltungsbereich

Der PMM verwaltet physischen Arbeitsspeicher und besondere physische Speicherbereiche.

Nicht direkt durch den PMM verwaltet werden:

- virtuelle Adressräume,
- Dateicaches,
- Prozess-Heaps,
- Kernelobjekte,
- Gerätedateien,
- persistente Dateisystembelegung.

Die virtuelle Speicherverwaltung verwendet den PMM als Quelle physischer Seitenrahmen.

## 3. Grundbegriffe

| Begriff | Bedeutung |
|---|---|
| Physische Adresse | Adresse innerhalb des physischen Adressraums |
| Seite | kleinste reguläre Verwaltungseinheit |
| Seitenrahmen | physische Speichereinheit einer Seite |
| PFN | Page Frame Number |
| Zone | Speicherbereich mit gemeinsamen Eigenschaften |
| Node | NUMA-Speicherknoten |
| Order | Zweierpotenz der Anzahl zusammenhängender Seiten |
| Reserved | Speicher, der nicht frei allokiert werden darf |

## 4. Adresstypen

Physische und virtuelle Adressen müssen durch unterschiedliche Typen dargestellt werden.

```c
typedef uint64_t np_physical_address_t;
typedef uintptr_t np_virtual_address_t;
typedef uint64_t np_page_frame_number_t;
```

Physische Adressen dürfen nicht ohne ausdrückliche Abbildung als virtuelle Zeiger verwendet werden.

## 5. Seitengröße

Die reguläre Basisseitengröße wird durch die Zielarchitektur festgelegt.

Für die initiale x86-Implementierung gilt:

```text
Base Page Size = 4096 Byte
```

```c
#define NP_PAGE_SHIFT 12
#define NP_PAGE_SIZE  (1UL << NP_PAGE_SHIFT)
#define NP_PAGE_MASK  (~(NP_PAGE_SIZE - 1))
```

Architekturen dürfen zusätzlich große Seiten unterstützen. Die PMM-Basiseinheit bleibt jedoch die reguläre Basisseite.

## 6. Seitenrahmennummer

Die Umrechnung zwischen physischer Adresse und PFN erfolgt durch:

```text
PFN = physical_address / page_size
physical_address = PFN × page_size
```

Hilfsfunktionen müssen Überläufe und ungültige Ausrichtungen berücksichtigen.

```c
np_page_frame_number_t np_address_to_pfn(
    np_physical_address_t address
);

np_physical_address_t np_pfn_to_address(
    np_page_frame_number_t pfn
);
```

## 7. Speicherkarte

Der PMM erhält die physische Speicherkarte über das NovaOS Boot-Handoff Protocol.

Ein Eintrag besitzt mindestens:

```c
typedef struct np_memory_map_entry {
    np_physical_address_t base;
    uint64_t length;
    uint32_t type;
    uint32_t attributes;
    uint32_t numa_node;
    uint32_t reserved;
} np_memory_map_entry_t;
```

Alle Einträge müssen vor ihrer Verwendung validiert und auf Seitengrenzen normalisiert werden.

## 8. Speichertypen

```c
typedef enum np_memory_type {
    NP_MEMORY_AVAILABLE,
    NP_MEMORY_KERNEL,
    NP_MEMORY_BOOT_DATA,
    NP_MEMORY_FIRMWARE,
    NP_MEMORY_ACPI_RECLAIMABLE,
    NP_MEMORY_ACPI_NVS,
    NP_MEMORY_DEVICE,
    NP_MEMORY_PERSISTENT,
    NP_MEMORY_BAD,
    NP_MEMORY_RESERVED
} np_memory_type_t;
```

| Typ | Verwendung |
|---|---|
| `AVAILABLE` | frei nutzbarer Arbeitsspeicher |
| `KERNEL` | Kernel-Image und Kernel-Daten |
| `BOOT_DATA` | Boot-Handoff, Module und Initial-RAM-Dateisystem |
| `FIRMWARE` | Firmwarecode und Firmwaredaten |
| `ACPI_RECLAIMABLE` | nach ACPI-Übernahme freigebbar |
| `ACPI_NVS` | für Firmwarezustände reserviert |
| `DEVICE` | MMIO- oder Gerätespeicher |
| `PERSISTENT` | persistenter physischer Speicher |
| `BAD` | als fehlerhaft erkannter Speicher |
| `RESERVED` | nicht verwendbarer oder unbekannter Bereich |

## 9. Normalisierung der Speicherkarte

Vor der PMM-Initialisierung muss die Speicherkarte:

1. auf gültige Adressgrenzen geprüft,
2. nach physischer Startadresse sortiert,
3. auf Seitengrenzen ausgerichtet,
4. von Überläufen bereinigt,
5. in nicht überlappende Bereiche zerlegt,
6. nach Speichertyp priorisiert,
7. mit Kernel- und Bootreservierungen ergänzt werden.

Bei Überlappungen gilt grundsätzlich:

```text
BAD
  > RESERVED
  > DEVICE
  > FIRMWARE
  > KERNEL
  > BOOT_DATA
  > AVAILABLE
```

Ein Bereich darf nur dann als frei gelten, wenn er vollständig als verfügbar bestätigt wurde.

## 10. Frühe Reservierungen

Vor Freigabe des allgemeinen Speichers müssen mindestens folgende Bereiche reserviert werden:

- Kernel-Image,
- frühe Kernel-Stacks,
- Boot-Handoff-Struktur,
- Initial-RAM-Dateisystem,
- Bootmodule,
- frühe Seitentabellen,
- Firmwaretabellen,
- Framebuffer,
- AP-Startcode,
- Early-Allocator-Belegungen,
- Crash-Dump-Reservierungen,
- bekannte MMIO-Bereiche.

Reservierungen müssen in der PMM-Diagnose nachvollziehbar bleiben.

## 11. Early Allocator

Während der frühen Bootphase wird ein einfacher Early Allocator verwendet.

```c
np_physical_address_t np_early_alloc_pages(
    size_t page_count,
    size_t alignment_pages
);
```

Der Early Allocator:

- verwendet ausschließlich geprüften freien Speicher,
- unterstützt keine reguläre Freigabe,
- protokolliert jede Zuweisung,
- wird nach Initialisierung des PMM deaktiviert.

Alle durch ihn vergebenen Seiten müssen vom PMM als belegt übernommen werden.

## 12. Seitenmetadaten

Jeder verwaltete Seitenrahmen besitzt Metadaten.

```c
typedef struct np_page {
    uint32_t flags;
    uint16_t order;
    uint16_t node_id;

    np_atomic_uint32_t reference_count;
    np_atomic_uint32_t map_count;

    uint32_t owner_tag;
    uint32_t generation;
} np_page_t;
```

Die Metadaten können für nicht vorhandene oder nicht verwaltbare Adressbereiche kompakt oder mehrstufig gespeichert werden.

## 13. Seitenzustände

```c
typedef enum np_page_flags {
    NP_PAGE_FREE        = 1U << 0,
    NP_PAGE_RESERVED    = 1U << 1,
    NP_PAGE_ALLOCATED   = 1U << 2,
    NP_PAGE_DIRTY       = 1U << 3,
    NP_PAGE_PINNED      = 1U << 4,
    NP_PAGE_DMA         = 1U << 5,
    NP_PAGE_ZEROED      = 1U << 6,
    NP_PAGE_BAD         = 1U << 7,
    NP_PAGE_OFFLINE     = 1U << 8,
    NP_PAGE_PERSISTENT  = 1U << 9
} np_page_flags_t;
```

Unvereinbare Zustandskombinationen müssen verhindert werden. Eine Seite darf beispielsweise nicht gleichzeitig `FREE` und `ALLOCATED` sein.

## 14. Allokationsverfahren

NovaOS verwendet einen zonen- und NUMA-fähigen Buddy Allocator für reguläre physische Seiten.

Der Buddy Allocator verwaltet Blöcke mit Größen von:

```text
2^order Seiten
```

Beispiele bei 4-KiB-Seiten:

| Order | Seiten | Größe |
|---:|---:|---:|
| 0 | 1 | 4 KiB |
| 1 | 2 | 8 KiB |
| 2 | 4 | 16 KiB |
| 4 | 16 | 64 KiB |
| 8 | 256 | 1 MiB |
| 10 | 1024 | 4 MiB |

Die maximal unterstützte Order wird pro Architektur und Systemprofil festgelegt.

## 15. Freilisten

Jede Speicherzone besitzt pro Order eine Freiliste.

```c
typedef struct np_free_area {
    np_list_t blocks;
    uint64_t free_blocks;
} np_free_area_t;

typedef struct np_memory_zone {
    np_spinlock_t lock;
    np_free_area_t free_area[NP_MAX_ORDER + 1];
    uint64_t free_pages;
    uint64_t managed_pages;
} np_memory_zone_t;
```

Freilisten dürfen nicht aus dem Userspace zugänglich sein.

## 16. Speicherzonen

Physischer Speicher wird nach Adressierbarkeit und Verwendung in Zonen aufgeteilt.

```c
typedef enum np_memory_zone_type {
    NP_ZONE_DMA,
    NP_ZONE_DMA32,
    NP_ZONE_NORMAL,
    NP_ZONE_HIGH,
    NP_ZONE_PERSISTENT
} np_memory_zone_type_t;
```

| Zone | Zweck |
|---|---|
| `DMA` | Speicher für Geräte mit sehr kleinem Adressraum |
| `DMA32` | Speicher unterhalb der 4-GiB-Grenze |
| `NORMAL` | regulär durch den Kernel adressierbarer Speicher |
| `HIGH` | nicht dauerhaft direkt abgebildeter Speicher |
| `PERSISTENT` | persistenter physischer Speicher |

Nicht jede Architektur muss alle Zonen verwenden.

## 17. NUMA-Nodes

Jeder physische Speicherbereich kann einem NUMA-Node zugeordnet sein.

```c
typedef struct np_memory_node {
    uint32_t node_id;
    uint32_t flags;

    np_memory_zone_t zones[NP_ZONE_COUNT];

    uint64_t total_pages;
    uint64_t free_pages;
} np_memory_node_t;
```

Allokationen sollen zunächst lokalen Speicher verwenden und nur bei Bedarf auf entfernte Nodes ausweichen.

## 18. Allokationsflags

```c
typedef enum np_alloc_flags {
    NP_ALLOC_KERNEL       = 1U << 0,
    NP_ALLOC_USER         = 1U << 1,
    NP_ALLOC_ZERO         = 1U << 2,
    NP_ALLOC_DMA          = 1U << 3,
    NP_ALLOC_DMA32        = 1U << 4,
    NP_ALLOC_CONTIGUOUS   = 1U << 5,
    NP_ALLOC_NO_WAIT      = 1U << 6,
    NP_ALLOC_CRITICAL     = 1U << 7,
    NP_ALLOC_LOCAL_NODE   = 1U << 8,
    NP_ALLOC_CAN_RECLAIM  = 1U << 9
} np_alloc_flags_t;
```

Unbekannte oder widersprüchliche Flags müssen abgelehnt werden.

## 19. Allokations-API

```c
np_status_t np_pmm_allocate_page(
    np_alloc_flags_t flags,
    np_page_t** page
);

np_status_t np_pmm_allocate_pages(
    uint32_t order,
    np_alloc_flags_t flags,
    np_page_t** first_page
);

np_status_t np_pmm_allocate_on_node(
    uint32_t node_id,
    uint32_t order,
    np_alloc_flags_t flags,
    np_page_t** first_page
);
```

Die API gibt Seitenobjekte zurück. Physische Adressen werden über eine separate geprüfte Funktion ermittelt.

## 20. Freigabe-API

```c
np_status_t np_pmm_free_page(
    np_page_t* page
);

np_status_t np_pmm_free_pages(
    np_page_t* first_page,
    uint32_t order
);
```

Bei der Freigabe muss geprüft werden:

- ob die Seite vom PMM verwaltet wird,
- ob sie aktuell allokiert ist,
- ob die Order korrekt ist,
- ob keine Referenz oder Pinning-Sperre besteht,
- ob sie nicht reserviert oder fehlerhaft ist.

Doppelte Freigaben sind als schwerer Speicherverwaltungsfehler zu behandeln.

## 21. Buddy-Zusammenführung

Bei der Freigabe versucht der PMM, einen Block mit seinem freien Buddy zusammenzuführen.

```text
buddy_pfn = block_pfn XOR (1 << order)
```

Die Zusammenführung erfolgt nur, wenn:

- beide Blöcke dieselbe Order besitzen,
- beide frei sind,
- beide derselben Zone und demselben NUMA-Node angehören,
- keine Reservierungsgrenze überschritten wird.

## 22. Seitennullung

Seiten für Userspace oder sicherheitsgetrennte Domains müssen vor ihrer Freigabe an den neuen Besitzer genullt werden.

NovaOS kann dafür folgende Strategien kombinieren:

- Nullung während der Allokation,
- Hintergrundnullung freier Seiten,
- verzögerte Nullung vor der Abbildung,
- hardwarebeschleunigte Nullung.

Das Flag `NP_PAGE_ZEROED` darf nur gesetzt werden, wenn die gesamte Seite nachweislich genullt wurde.

## 23. Referenzzählung

Physische Seiten können von mehreren Kernelobjekten referenziert werden.

```c
void np_page_retain(np_page_t* page);
void np_page_release(np_page_t* page);
```

Eine Seite darf erst in den freien Pool zurückkehren, wenn:

```text
reference_count == 0
and map_count == 0
and pinned == false
```

Über- und Unterläufe der Referenzzähler müssen erkannt werden.

## 24. Abbildungszähler

Der Abbildungszähler dokumentiert, wie oft eine Seite in virtuellen Adressräumen abgebildet ist.

```c
void np_page_mapping_add(np_page_t* page);
void np_page_mapping_remove(np_page_t* page);
```

Der Abbildungszähler ersetzt keine vollständige Reverse-Mapping-Struktur, kann aber für Konsistenzprüfungen und Freigabeentscheidungen verwendet werden.

## 25. Gepinnte Seiten

Seiten können zeitweise gegen Verschiebung, Austausch oder Freigabe gesperrt werden.

Typische Gründe sind:

- aktive DMA-Operation,
- Gerätetransfer,
- Kernel-Stack,
- Seitentabelle,
- Crash-Dump-Puffer,
- hardwarenahe Datenstruktur.

```c
np_status_t np_page_pin(np_page_t* page);
np_status_t np_page_unpin(np_page_t* page);
```

Unprivilegierte Prozesse dürfen physische Seiten nicht unbegrenzt pinnen.

## 26. Zusammenhängender Speicher

Zusammenhängender physischer Speicher wird über eine Order-Allokation oder eine besondere Contiguous-Memory-Schnittstelle angefordert.

```c
np_status_t np_pmm_allocate_contiguous(
    size_t page_count,
    size_t alignment_pages,
    np_physical_address_t maximum_address,
    np_alloc_flags_t flags,
    np_page_t** first_page
);
```

Große zusammenhängende Allokationen können wegen Fragmentierung fehlschlagen, obwohl insgesamt genügend freie Seiten vorhanden sind.

Treiber sollen bevorzugt Scatter-Gather-DMA verwenden.

## 27. DMA-Speicher

DMA-Allokationen müssen zusätzlich berücksichtigen:

- Adressgrenze des Geräts,
- Ausrichtung,
- Segmentgrenzen,
- physische Zusammenhängigkeit,
- Cache-Kohärenz,
- IOMMU-Verfügbarkeit,
- Sicherheitsdomain des Geräts.

Der PMM stellt physische Seiten bereit. Die DMA-Schicht übernimmt IOMMU-Mappings und gerätespezifische Synchronisation.

Direkte physische Adressen dürfen einem Userspace-Treiber nur über kontrollierte DMA-Objekte zugänglich gemacht werden.

## 28. Große Seiten

Der PMM kann zusammenhängende Blöcke für große virtuelle Seiten bereitstellen.

Mögliche Seitengrößen auf x86 sind:

- 4 KiB,
- 2 MiB,
- abhängig vom Betriebsmodus weitere Größen.

Eine große Seite muss:

- korrekt ausgerichtet,
- physisch zusammenhängend,
- vollständig verfügbar,
- innerhalb eines kompatiblen Speicherbereichs liegen.

Der PMM entscheidet nicht selbst, ob eine virtuelle Abbildung als große Seite eingerichtet wird.

## 29. Speicherreservierungen zur Laufzeit

Kernelkomponenten dürfen physische Bereiche reservieren, wenn dies erforderlich ist.

```c
np_status_t np_pmm_reserve_range(
    np_physical_address_t base,
    uint64_t length,
    uint32_t owner_tag
);

np_status_t np_pmm_release_reserved_range(
    np_physical_address_t base,
    uint64_t length,
    uint32_t owner_tag
);
```

Bereits allokierte Seiten dürfen nicht stillschweigend als reserviert umklassifiziert werden.

## 30. Owner Tags

Allokationen und Reservierungen können einen internen Besitzer-Tag erhalten.

Beispiele:

```text
KERNEL_IMAGE
PAGE_TABLE
KERNEL_STACK
USER_MEMORY
FILE_CACHE
DMA_BUFFER
CRASH_DUMP
FIRMWARE
DRIVER
```

Owner Tags dienen Diagnose und Abrechnung. Sie ersetzen keine Capability-Prüfung.

## 31. Speicherknappheit

Der PMM verwaltet Wassermarken pro Zone:

- Minimum,
- Low,
- High.

```c
typedef struct np_zone_watermarks {
    uint64_t minimum_pages;
    uint64_t low_pages;
    uint64_t high_pages;
} np_zone_watermarks_t;
```

Bei Unterschreitung können folgende Maßnahmen ausgelöst werden:

1. Hintergrund-Reclaim starten,
2. Cache-Bereinigung anfordern,
3. optionale Allokationen verzögern,
4. Notfallreserven verwenden,
5. kontrollierte Out-of-Memory-Behandlung aktivieren.

## 32. Notfallreserve

Für kritische Kernelpfade wird eine begrenzte Notfallreserve geführt.

Sie darf nur verwendet werden für:

- Speicherbereinigung,
- Fehlerbehandlung,
- Prozessbeendigung,
- I/O-Abschluss zur Speicherfreigabe,
- Crash-Dump- oder Recovery-Pfade.

Normale Anwendungen und nicht kritische Treiber dürfen nicht auf die Notfallreserve zugreifen.

## 33. Allokationskontexte

Der PMM unterscheidet mindestens:

| Kontext | Darf warten | Darf Reclaim auslösen |
|---|---:|---:|
| Normal | Ja | Ja |
| Kernel kritisch | begrenzt | Ja |
| Interrupt | Nein | Nein |
| Realtime | nur nach Richtlinie | normalerweise Nein |
| Recovery | begrenzt | Ja |

Eine Allokation im Interruptkontext muss `NP_ALLOC_NO_WAIT` verwenden oder auf einen vorab reservierten Pool zugreifen.

## 34. Out-of-Memory-Behandlung

Kann eine kritische Allokation nicht erfüllt werden, wird die zentrale OOM-Policy aktiviert.

Mögliche Maßnahmen sind:

- Dateicaches reduzieren,
- ungenutzte Speicherobjekte freigeben,
- Hintergrunddienste drosseln,
- Prozesse anhand einer kontrollierten Richtlinie beenden,
- in einen eingeschränkten Systemzustand wechseln,
- Recovery oder Kernel Panic auslösen.

Der PMM allein wählt keinen Benutzerprozess zur Beendigung aus. Diese Entscheidung erfolgt durch den zuständigen Ressourcenmanager.

## 35. NUMA-Allokationsrichtlinien

Unterstützte Richtlinien sind:

```c
typedef enum np_numa_policy {
    NP_NUMA_LOCAL,
    NP_NUMA_PREFERRED,
    NP_NUMA_INTERLEAVE,
    NP_NUMA_BIND,
    NP_NUMA_SYSTEM_DEFAULT
} np_numa_policy_t;
```

| Richtlinie | Verhalten |
|---|---|
| `LOCAL` | lokalen Node bevorzugen |
| `PREFERRED` | angegebenen Node bevorzugen, Ausweichmöglichkeit erlaubt |
| `INTERLEAVE` | Seiten über mehrere Nodes verteilen |
| `BIND` | ausschließlich erlaubte Nodes verwenden |
| `SYSTEM_DEFAULT` | globale Systemrichtlinie verwenden |

Eine Bind-Richtlinie kann trotz freien Speichers auf anderen Nodes fehlschlagen.

## 36. Speicher-Hotplug

Der PMM unterstützt das kontrollierte Hinzufügen und Entfernen physischer Speicherbereiche.

Beim Hinzufügen werden:

1. Bereich und Plattformstatus validiert,
2. Seitenmetadaten erzeugt,
3. Seiten getestet oder geprüft,
4. Zone und NUMA-Node bestimmt,
5. Seiten in den freien Pool aufgenommen,
6. ein Speicherereignis veröffentlicht.

Neue Bereiche dürfen erst nach vollständiger Initialisierung allokiert werden.

## 37. Speicherentfernung

Vor dem Entfernen eines Speicherbereichs müssen:

- neue Allokationen daraus verhindert,
- bewegliche Seiten migriert,
- Caches geleert,
- DMA-Operationen beendet,
- gepinnte Seiten geprüft,
- Seitentabellen aktualisiert,
- verbleibende Referenzen ausgeschlossen werden.

Enthält der Bereich nicht migrierbare Seiten, muss die Entfernung abgelehnt werden.

Kernel-Code, aktive Kernel-Stacks oder nicht verschiebbare Seitentabellen dürfen nicht ohne gesonderte Migrationsunterstützung entfernt werden.

## 38. Fehlerhafte Seiten

Eine als fehlerhaft erkannte Seite wird mit `NP_PAGE_BAD` markiert.

Sie muss:

- aus allen Freilisten entfernt,
- gegen neue Allokationen gesperrt,
- diagnostisch erfasst,
- bei möglicher Datengefährdung an höhere Schichten gemeldet werden.

Enthält die Seite kritische Kernel- oder Benutzerdaten, entscheidet die Fehlerbehandlung über Migration, Prozessbeendigung oder Kernel Panic.

## 39. Persistenter Speicher

Persistenter physischer Speicher wird in einer getrennten Zone verwaltet.

Er darf nicht automatisch wie flüchtiger Arbeitsspeicher verwendet werden.

Die Verwendung benötigt eine explizite Richtlinie für:

- Datenpersistenz,
- Cache-Flush,
- Fehlerbehandlung,
- Sicherheitslöschung,
- Dateisystem- oder Objektintegration.

## 40. Firmware-Speicher

Firmwarebereiche dürfen erst freigegeben werden, wenn die zuständige Plattformkomponente bestätigt, dass sie nicht mehr benötigt werden.

ACPI-Reclaimable-Speicher kann nach dem Kopieren und Validieren der erforderlichen Tabellen freigegeben werden.

ACPI-NVS- und Firmware-Runtime-Bereiche bleiben entsprechend der Plattformanforderungen reserviert.

## 41. Synchronisation

Jede Zone besitzt eigene Synchronisationsmechanismen.

Allokationen auf verschiedenen NUMA-Nodes oder Zonen sollen weitgehend parallel möglich sein.

Per-CPU-Seitencaches dürfen für Order-0-Allokationen verwendet werden, wenn:

- globale Statistiken korrekt bleiben,
- Speicher-Hotplug berücksichtigt wird,
- Seiten bei CPU-Offlining zurückgegeben werden,
- Reservierungsgrenzen nicht umgangen werden.

Globale Sperren im normalen Allokationspfad sollen vermieden werden.

## 42. Per-CPU-Seitencache

Häufige Einzelallokationen können über kleine CPU-lokale Pools bedient werden.

```c
typedef struct np_per_cpu_page_cache {
    np_page_t* pages[NP_PCPU_PAGE_CACHE_SIZE];
    uint32_t count;
} np_per_cpu_page_cache_t;
```

Der Cache darf nur eine begrenzte Anzahl Seiten zurückhalten.

Bei Speicherknappheit oder CPU-Hotplug müssen die Seiten an die regulären Zonen zurückgegeben werden.

## 43. Statistik

Der PMM stellt mindestens folgende Statistiken bereit:

```c
typedef struct np_pmm_statistics {
    uint64_t total_pages;
    uint64_t managed_pages;
    uint64_t free_pages;
    uint64_t allocated_pages;
    uint64_t reserved_pages;
    uint64_t pinned_pages;
    uint64_t bad_pages;
    uint64_t offline_pages;
    uint64_t zeroed_free_pages;
} np_pmm_statistics_t;
```

Statistiken müssen pro System, NUMA-Node und Speicherzone abrufbar sein.

## 44. Diagnose-API

```c
np_status_t np_pmm_query_statistics(
    np_pmm_scope_t scope,
    uint32_t scope_id,
    np_pmm_statistics_t* statistics
);

np_status_t np_pmm_query_page(
    np_page_frame_number_t pfn,
    np_page_information_t* information
);
```

Die Abfrage einzelner Seiten und ihrer Besitzerinformationen erfordert erweiterte Diagnoseberechtigungen.

Physische Speicherinhalte werden über diese API nicht ausgegeben.

## 45. Capability-Rechte

Mindestens folgende Rechte werden definiert:

| Capability | Bedeutung |
|---|---|
| `CAP_PMM_QUERY` | allgemeine Speicherstatistiken lesen |
| `CAP_PMM_DIAGNOSTIC` | detaillierte Seiteninformationen lesen |
| `CAP_PMM_RESERVE` | physische Bereiche reservieren |
| `CAP_PMM_DMA` | DMA-fähige Speicherobjekte anfordern |
| `CAP_PMM_PIN` | Seiten kontrolliert pinnen |
| `CAP_PMM_HOTPLUG` | Speicher hinzufügen oder entfernen |
| `CAP_PMM_ADMIN` | globale PMM-Richtlinien verwalten |

Reguläre Userspace-Prozesse erhalten keinen direkten Zugriff auf die PMM-Allokations-API.

## 46. Fehlercodes

| Fehlercode | Bedeutung |
|---|---|
| `NP_OK` | Operation erfolgreich |
| `NP_ERR_INVALID_ARGUMENT` | ungültiger Parameter |
| `NP_ERR_INVALID_ADDRESS` | ungültige physische Adresse |
| `NP_ERR_INVALID_STATE` | unzulässiger Seitenzustand |
| `NP_ERR_OUT_OF_MEMORY` | kein geeigneter Speicher verfügbar |
| `NP_ERR_FRAGMENTED` | kein ausreichend großer zusammenhängender Block |
| `NP_ERR_ZONE_EXHAUSTED` | angeforderte Zone ist erschöpft |
| `NP_ERR_NODE_UNAVAILABLE` | NUMA-Node nicht verfügbar |
| `NP_ERR_PAGE_PINNED` | Seite ist gepinnt |
| `NP_ERR_PAGE_REFERENCED` | Seite besitzt aktive Referenzen |
| `NP_ERR_ALREADY_FREE` | Seite wurde bereits freigegeben |
| `NP_ERR_RESERVED` | Bereich ist reserviert |
| `NP_ERR_HOTPLUG_BLOCKED` | Speicher kann nicht entfernt werden |
| `NP_ERR_ACCESS_DENIED` | erforderliche Capability fehlt |

## 47. Sicherheitsanforderungen

Der PMM muss sicherstellen, dass:

- Speicher für einen neuen Sicherheitskontext zuvor genullt wird,
- freier Speicher keine Daten früherer Besitzer offenlegt,
- physische Adressen nicht unkontrolliert an Userspace ausgegeben werden,
- Kernel- und Firmwarebereiche niemals regulär allokiert werden,
- DMA-Speicher über IOMMU oder kontrollierte DMA-Objekte geschützt wird,
- doppelte Freigaben erkannt werden,
- Metadaten nicht durch verwalteten Nutzspeicher überschrieben werden können,
- fehlerhafte Seiten dauerhaft aus den Freilisten entfernt werden,
- Notfallreserven nur autorisierten Kernelpfaden zur Verfügung stehen,
- Diagnosezugriffe Capability-Prüfungen durchlaufen.

## 48. Datenschutz

Der PMM erfasst keine fachlichen Inhalte des Speichers.

Diagnoseinformationen beschränken sich standardmäßig auf:

- Seitentyp,
- Zustand,
- Node und Zone,
- Besitzerklasse,
- Referenz- und Abbildungszähler.

Speicherinhalte dürfen ausschließlich über ausdrücklich autorisierte Debug- oder Crash-Dump-Schnittstellen gelesen werden.

## 49. Fehlerbehandlung

Nicht erfüllbare normale Allokationen liefern einen definierten Fehler an den Aufrufer zurück.

Folgende Situationen gelten als schwere PMM-Konsistenzfehler:

- doppelte Freigabe,
- beschädigte Freiliste,
- Überlappung freier und reservierter Bereiche,
- Freigabe einer noch referenzierten Seite,
- Buddy-Zusammenführung über Zonengrenzen,
- Allokation einer als fehlerhaft markierten Seite,
- widersprüchliche Seitenmetadaten.

Abhängig von der Auswirkung wird die betroffene Zone isoliert oder die Kernel-Panic-Policy aktiviert.

## 50. Testanforderungen

Mindestens folgende Tests sind erforderlich:

1. Übernahme einer gültigen Speicherkarte,
2. Behandlung überlappender Speicherkarteneinträge,
3. Ausrichtung nicht seitengenauer Bereiche,
4. Reservierung des Kernel-Images,
5. Übernahme der Early-Allocator-Belegungen,
6. Einzelallokation und Freigabe,
7. Allokation verschiedener Orders,
8. korrekte Buddy-Aufteilung,
9. korrekte Buddy-Zusammenführung,
10. Ablehnung einer doppelten Freigabe,
11. Ablehnung reservierter Seiten,
12. Nullung von Userspace-Seiten,
13. Referenz- und Abbildungszählung,
14. Pinning und Unpinning,
15. zusammenhängende DMA-Allokation,
16. Einhaltung von DMA-Adressgrenzen,
17. Allokation aus verschiedenen Zonen,
18. lokale NUMA-Allokation,
19. NUMA-Ausweichstrategie,
20. strikte NUMA-Bindung,
21. Verhalten bei Zonenknappheit,
22. Nutzung und Schutz der Notfallreserve,
23. parallele Allokation auf mehreren CPUs,
24. Per-CPU-Seitencaches,
25. Speicher-Hotplug,
26. Ablehnung nicht entfernbaren Speichers,
27. Entfernung vollständig migrierbarer Bereiche,
28. Isolierung fehlerhafter Seiten,
29. CPU-Hotplug mit gefülltem Seitencache,
30. korrekte Speicherstatistiken.

## 51. Verbindliche Invarianten

1. Eine physische Seite befindet sich zu jedem Zeitpunkt in genau einem gültigen PMM-Zustand.
2. Eine Seite ist niemals gleichzeitig frei und allokiert.
3. Reservierte, fehlerhafte oder offline geschaltete Seiten werden nicht allokiert.
4. Eine referenzierte, abgebildete oder gepinnte Seite wird nicht freigegeben.
5. Userspace-Seiten werden vor der Übergabe an einen neuen Besitzer genullt.
6. Buddy-Blöcke werden nur innerhalb derselben Zone und desselben NUMA-Nodes zusammengeführt.
7. Physische Adressen werden nicht als ungeprüfte virtuelle Zeiger verwendet.
8. Early-Allocator-Belegungen werden vollständig in den PMM übernommen.
9. DMA-Adressgrenzen werden bei jeder entsprechenden Allokation eingehalten.
10. Speicher-Hotplug macht Seiten erst nach vollständiger Initialisierung verfügbar.
11. PMM-Metadaten liegen niemals in frei allokierbarem Nutzspeicher.
12. Jede erfolgreiche Allokation besitzt eine eindeutig nachvollziehbare Größe und Order.

## 52. Referenzablauf einer Allokation

```text
Allokationsanfrage
  -> Parameter und Kontext prüfen
  -> NUMA-Node auswählen
  -> Speicherzone auswählen
  -> Per-CPU-Cache prüfen
  -> passende Freiliste suchen
  -> größeren Buddy-Block bei Bedarf teilen
  -> Block aus Freiliste entfernen
  -> Seitenmetadaten aktualisieren
  -> Seite bei Bedarf nullen
  -> Referenz und Owner Tag setzen
  -> Seite an Aufrufer zurückgeben
```

## 53. Referenzablauf einer Freigabe

```text
Freigabeanforderung
  -> Seite und Order validieren
  -> Referenz-, Mapping- und Pinning-Zustand prüfen
  -> sensible Daten nach Richtlinie behandeln
  -> Seite als frei markieren
  -> freien Buddy suchen
  -> kompatible Buddy-Blöcke zusammenführen
  -> Block in passende Freiliste einfügen
  -> Statistik und Wassermarken aktualisieren
  -> bei Bedarf Speicher-Wakeup auslösen
```

## 54. Zusammenfassung

Der NovaOS Physical Memory Manager verwaltet physischen Speicher über einen zonen- und NUMA-fähigen Buddy Allocator.

Er übernimmt die validierte Firmware-Speicherkarte, schützt Kernel-, Firmware- und Gerätebereiche und stellt reguläre, zusammenhängende sowie DMA-fähige Seitenallokationen bereit. Referenzzählung, Pinning, Seitennullung und Guard-Richtlinien verhindern vorzeitige Freigaben und Datenoffenlegung.

Speicher-Hotplug, fehlerhafte Seiten, Notfallreserven und lokale Diagnosefunktionen ermöglichen einen robusten Betrieb auch unter Speicherknappheit oder bei teilweisen Hardwarefehlern.