# NPSPEC-KERNEL-0007 – Virtual Memory Manager

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-KERNEL-0007 |
| Titel | Virtual Memory Manager |
| Status | Spezifikation |
| Version | 1.0 |
| Datum | 2026-08-03 |
| Bereich | Kernel / Speicherverwaltung |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | NPSPEC-KERNEL-0001, NPSPEC-KERNEL-0004, NPSPEC-KERNEL-0006 |
| Zugehörige ADRs | ADR-MM-0002, ADR-MM-0003, ADR-MM-0004, ADR-MM-0005, ADR-MM-0006, ADR-MM-0012, ADR-MM-0013, ADR-MM-0015 |

---

## 1. Zweck

Diese Spezifikation definiert den Virtual Memory Manager, im Folgenden VMM genannt.

Der VMM verwaltet virtuelle Adressräume, Seitentabellen und Speicherabbildungen für Kernel und Userspace-Prozesse.

Zu seinen Aufgaben gehören:

- Erzeugung isolierter Prozessadressräume,
- Abbildung physischer Seiten,
- Verwaltung von Speicherregionen,
- Demand Paging,
- Copy-on-Write,
- Shared Memory,
- Memory-Mapped Files,
- Guard Pages,
- Page-Fault-Behandlung,
- Schutz- und Cacheattribute,
- Unterstützung großer Seiten,
- TLB-Verwaltung.

## 2. Abgrenzung

Der VMM verwaltet virtuelle Abbildungen.

Der Physical Memory Manager stellt die dafür erforderlichen physischen Seitenrahmen bereit.

| Komponente | Verantwortung |
|---|---|
| PMM | physische Seitenrahmen |
| VMM | virtuelle Adressen und Seitentabellen |
| Process Manager | Besitz und Lebenszyklus von Adressräumen |
| VFS | dateibasierte Speicherobjekte |
| DMA Framework | Geräteadressräume und IOMMU-Mappings |

## 3. Adresstypen

Physische und virtuelle Adressen müssen durch unterschiedliche Typen dargestellt werden.

```c
typedef uint64_t np_physical_address_t;
typedef uintptr_t np_virtual_address_t;
typedef uint64_t np_address_space_id_t;
```

Eine physische Adresse darf nicht direkt als virtueller Zeiger verwendet werden.

## 4. Seitengrößen

Die Basisseitengröße ist architekturabhängig.

Für die initiale x86-Implementierung gilt:

```text
Base Page Size = 4096 Byte
```

Zusätzlich können große Seiten unterstützt werden.

```c
typedef enum np_page_size {
    NP_PAGE_SIZE_BASE,
    NP_PAGE_SIZE_LARGE,
    NP_PAGE_SIZE_HUGE
} np_page_size_t;
```

Die tatsächlich unterstützten Größen werden über die Architekturabstraktion abgefragt.

## 5. Adressraum

Jeder reguläre Userspace-Prozess besitzt einen eigenen virtuellen Adressraum.

```c
typedef struct np_address_space {
    np_kernel_object_t object;
    np_address_space_id_t id;

    np_arch_page_table_t* page_table;
    np_virtual_region_tree_t regions;

    np_capability_domain_t* owner_domain;
    np_spinlock_t lock;

    uint64_t resident_pages;
    uint64_t committed_pages;
    uint32_t flags;
} np_address_space_t;
```

Die interne Struktur ist nicht Teil der öffentlichen ABI.

## 6. Kerneladressraum

Der Kernel besitzt einen privilegierten Adressraum mit mindestens folgenden Bereichen:

- Kernel-Code,
- schreibgeschützte Kernel-Daten,
- Kernel-Daten und BSS,
- Kernel-Heap,
- Kernel-Stacks,
- Seitentabellen,
- physische Direktabbildung oder Mapping-Fenster,
- MMIO-Abbildungen,
- Kernelmodule,
- Diagnose- und Crash-Dump-Bereiche.

Kernelbereiche dürfen aus dem Userspace nicht zugänglich sein.

## 7. Userspace-Adressraum

Ein Prozessadressraum enthält typischerweise:

- Programmcode,
- schreibgeschützte Programmdaten,
- veränderliche Daten,
- Heap,
- Thread-Stacks,
- Thread-Local Storage,
- Shared-Memory-Regionen,
- Memory-Mapped Files,
- Systembibliotheken,
- kontrollierte Kernel-Service-Seiten.

Konkrete Adressbereiche sind architektur- und ABI-abhängig.

## 8. Virtuelle Speicherregion

Eine virtuelle Speicherregion beschreibt einen zusammenhängenden Adressbereich mit einheitlichen Eigenschaften.

```c
typedef struct np_virtual_region {
    np_virtual_address_t base;
    uint64_t length;

    uint32_t protection;
    uint32_t flags;

    np_memory_object_t* memory_object;
    uint64_t object_offset;
} np_virtual_region_t;
```

Regionen eines Adressraums dürfen sich nicht überlappen.

## 9. Regionsverwaltung

Virtuelle Regionen werden in einer geordneten Struktur verwaltet, beispielsweise:

- balancierter Suchbaum,
- Intervallbaum,
- geeignete erweiterbare Baumstruktur.

Die Struktur muss effizient unterstützen:

- Suche nach einer Adresse,
- Suche nach einem freien Bereich,
- Einfügen und Entfernen,
- Aufteilen,
- Zusammenführen kompatibler Nachbarregionen,
- Erkennen von Überlappungen.

## 10. Speicherschutzrechte

```c
typedef enum np_memory_protection {
    NP_PROT_NONE    = 0,
    NP_PROT_READ    = 1U << 0,
    NP_PROT_WRITE   = 1U << 1,
    NP_PROT_EXECUTE = 1U << 2,
    NP_PROT_USER    = 1U << 3,
    NP_PROT_GLOBAL  = 1U << 4
} np_memory_protection_t;
```

Typische Rechte sind:

| Bereich | Lesen | Schreiben | Ausführen |
|---|---:|---:|---:|
| Programmcode | Ja | Nein | Ja |
| schreibgeschützte Daten | Ja | Nein | Nein |
| Prozessdaten | Ja | Ja | Nein |
| Heap | Ja | Ja | Nein |
| Stack | Ja | Ja | Nein |
| Kernel-Code | Kernel | Nein | Kernel |
| MMIO | abhängig vom Gerät | abhängig vom Gerät | Nein |

## 11. W^X-Richtlinie

NovaOS verwendet grundsätzlich die Richtlinie Write XOR Execute.

Eine Seite darf im normalen Betrieb nicht gleichzeitig beschreibbar und ausführbar sein.

Ausnahmen benötigen:

- eine spezielle Capability,
- einen kontrollierten Übergang zwischen Schreib- und Ausführungszustand,
- eine Sicherheitsprüfung,
- eine lokale Auditaufzeichnung.

JIT-Laufzeitumgebungen müssen getrennte Schreib- und Ausführungsphasen oder getrennte Abbildungen verwenden.

## 12. Regionsflags

```c
typedef enum np_region_flags {
    NP_REGION_PRIVATE       = 1U << 0,
    NP_REGION_SHARED        = 1U << 1,
    NP_REGION_ANONYMOUS     = 1U << 2,
    NP_REGION_FILE_BACKED   = 1U << 3,
    NP_REGION_FIXED         = 1U << 4,
    NP_REGION_GUARD         = 1U << 5,
    NP_REGION_LOCKED        = 1U << 6,
    NP_REGION_NO_DUMP       = 1U << 7,
    NP_REGION_LAZY          = 1U << 8,
    NP_REGION_COPY_ON_WRITE = 1U << 9
} np_region_flags_t;
```

Widersprüchliche Flagkombinationen müssen abgelehnt werden.

## 13. Memory Objects

Virtuelle Regionen referenzieren Memory Objects.

```c
typedef enum np_memory_object_type {
    NP_MEMORY_OBJECT_ANONYMOUS,
    NP_MEMORY_OBJECT_FILE,
    NP_MEMORY_OBJECT_SHARED,
    NP_MEMORY_OBJECT_DEVICE,
    NP_MEMORY_OBJECT_PHYSICAL,
    NP_MEMORY_OBJECT_KERNEL
} np_memory_object_type_t;
```

Ein Memory Object beschreibt die logische Quelle der Seiten und kann von mehreren Regionen oder Adressräumen verwendet werden.

## 14. Anonymer Speicher

Anonymer Speicher besitzt keine direkte Dateiquelle.

Er wird verwendet für:

- Heap,
- Thread-Stacks,
- private Programmdaten,
- temporäre Speicherbereiche,
- interne Laufzeitstrukturen.

Physische Seiten können erst beim ersten Zugriff bereitgestellt werden.

Neue anonyme Seiten müssen vor der Abbildung in einen Userspace-Prozess genullt sein.

## 15. Adressraum-API

```c
np_status_t np_vmm_address_space_create(
    uint32_t flags,
    np_address_space_t** address_space
);

np_status_t np_vmm_address_space_destroy(
    np_address_space_t* address_space
);

np_status_t np_vmm_address_space_activate(
    np_address_space_t* address_space
);
```

Ein Adressraum darf erst zerstört werden, wenn er auf keiner CPU mehr aktiv ist und keine unzulässigen Referenzen bestehen.

## 16. Mapping-API

```c
np_status_t np_vmm_map(
    np_address_space_t* address_space,
    np_virtual_address_t requested_address,
    uint64_t length,
    uint32_t protection,
    uint32_t flags,
    np_memory_object_t* memory_object,
    uint64_t object_offset,
    np_virtual_address_t* mapped_address
);

np_status_t np_vmm_unmap(
    np_address_space_t* address_space,
    np_virtual_address_t address,
    uint64_t length
);

np_status_t np_vmm_protect(
    np_address_space_t* address_space,
    np_virtual_address_t address,
    uint64_t length,
    uint32_t protection
);
```

Alle Adressen, Längen und Offsets müssen auf Überläufe und gültige Grenzen geprüft werden.

## 17. Mapping-Platzierung

Wird keine feste Adresse verlangt, wählt der VMM einen geeigneten freien Bereich.

Die Auswahl berücksichtigt:

- erforderliche Ausrichtung,
- Adressraumlayout,
- ASLR,
- Guard Pages,
- große Seiten,
- Architekturgrenzen,
- Konflikte mit bestehenden Regionen.

`NP_REGION_FIXED` darf bestehende Regionen nicht stillschweigend überschreiben.

## 18. Address Space Layout Randomization

Die Position wichtiger Userspace-Bereiche wird zufällig variiert.

Dazu gehören:

- ausführbares Image,
- Systembibliotheken,
- Heap,
- Thread-Stacks,
- Shared-Memory-Regionen,
- Memory-Mapped Files.

Die Zufallsquelle muss während der Kernel-Bootsequenz initialisiert worden sein.

ASLR ist keine Ersatzmaßnahme für korrekte Speicherrechte oder sichere Speicherverwaltung.

## 19. Demand Paging

Bei Demand Paging werden virtuelle Regionen zunächst ohne sofortige Bereitstellung aller physischen Seiten erzeugt.

Beim ersten Zugriff:

1. CPU erzeugt einen Page Fault,
2. VMM ermittelt die betroffene Region,
3. Zugriff wird gegen die Regionsrechte geprüft,
4. eine physische Seite wird beschafft,
5. der Seiteninhalt wird erzeugt oder geladen,
6. die Seite wird abgebildet,
7. die Instruktion wird fortgesetzt.

Demand Paging darf nur in einem Kontext erfolgen, in dem die erforderliche Verarbeitung sicher möglich ist.

## 20. Page Faults

```c
typedef struct np_page_fault_info {
    np_virtual_address_t address;
    uintptr_t instruction_pointer;

    bool present;
    bool write;
    bool execute;
    bool user;
    bool reserved_bit;
} np_page_fault_info_t;
```

Der architekturspezifische Exception Handler übersetzt Hardwareinformationen in diese generische Struktur.

## 21. Page-Fault-Behandlung

Die Page-Fault-Behandlung erfolgt in folgender Reihenfolge:

```text
Page Fault
  -> Fehleradresse und Zugriffsart erfassen
  -> Adressraum bestimmen
  -> passende Region suchen
  -> Zugriffsrechte prüfen
  -> Guard- oder Schutzverletzung erkennen
  -> Memory Object befragen
  -> physische Seite bereitstellen
  -> Seitentabelle aktualisieren
  -> TLB-Zustand aktualisieren
  -> Ausführung fortsetzen
```

Kann der Fehler nicht sicher behandelt werden, wird eine strukturierte Speicher-Exception an den betroffenen Prozess zugestellt.

## 22. Kernel Page Faults

Ein Page Fault im Kernel ist nur dann wiederherstellbar, wenn er in einem ausdrücklich dafür vorgesehenen Bereich auftritt.

Beispiele sind:

- kontrollierter Zugriff auf Userspace-Speicher,
- Demand Paging eines Kernelobjekts,
- definierte Mapping-Fenster.

Ein unerwarteter Kernel Page Fault gilt als schwerer Kernelkonsistenzfehler und wird entsprechend der Kernel-Panic-Policy behandelt.

## 23. Copy-on-Write

Copy-on-Write ermöglicht die gemeinsame Nutzung identischer Seiten bis zum ersten Schreibzugriff.

Eine COW-Seite wird:

- schreibgeschützt abgebildet,
- von mindestens zwei Abbildungen referenziert,
- bei einem Schreibzugriff kopiert,
- anschließend für den schreibenden Adressraum beschreibbar abgebildet.

```text
Write Fault
  -> COW-Status prüfen
  -> neue physische Seite allokieren
  -> ursprünglichen Inhalt kopieren
  -> Referenzzählung aktualisieren
  -> neue Seite beschreibbar abbilden
```

COW darf nicht auf MMIO-Seiten angewendet werden.

## 24. Shared Memory

Ein Shared-Memory-Objekt kann in mehrere Adressräume abgebildet werden.

Die Berechtigungen werden für jedes Mapping getrennt geprüft.

Ein Prozess kann beispielsweise nur lesenden Zugriff erhalten, während ein anderer Prozess Schreibrechte besitzt.

Die gemeinsame physische Seite muss so lange bestehen bleiben, wie mindestens ein Mapping oder Objekt-Handle existiert.

## 25. Memory-Mapped Files

Dateien können über das VFS in virtuelle Regionen abgebildet werden.

Unterstützte Modi sind:

- private Abbildung,
- gemeinsame Abbildung,
- schreibgeschützte Abbildung,
- ausführbare Abbildung nach Sicherheitsprüfung.

Bei privaten Abbildungen werden Änderungen über Copy-on-Write behandelt.

Bei gemeinsamen Abbildungen können Änderungen über den File Cache in die Datei zurückgeschrieben werden.

## 26. Guard Pages

Guard Pages besitzen keine Zugriffsrechte und keine reguläre physische Abbildung.

Sie werden mindestens verwendet für:

- Kernel-Stacks,
- Userspace-Stacks,
- kritische Kernelstrukturen,
- ausgewählte Heapbereiche,
- Diagnose- und Sicherheitszonen.

Ein Zugriff auf eine Guard Page erzeugt eine definierte Speicherverletzung.

## 27. Stackwachstum

Automatisches Stackwachstum ist nur innerhalb eines vorher reservierten virtuellen Bereichs zulässig.

Der VMM prüft dabei:

- maximal zulässige Stackgröße,
- Nähe zum aktuellen Stack Pointer,
- gültige Wachstumsrichtung,
- Guard-Page-Grenzen,
- Ressourcenlimit des Prozesses.

Beliebige Zugriffe weit unterhalb oder oberhalb des aktuellen Stacks dürfen kein Stackwachstum auslösen.

## 28. Seitentabellen

Seitentabellen werden durch architekturspezifischen Code verwaltet.

```c
np_status_t np_arch_map_page(
    np_arch_page_table_t* page_table,
    np_virtual_address_t virtual_address,
    np_physical_address_t physical_address,
    uint32_t protection,
    np_page_size_t page_size
);

np_status_t np_arch_unmap_page(
    np_arch_page_table_t* page_table,
    np_virtual_address_t virtual_address
);
```

Seitentabellenseiten werden über den PMM bereitgestellt und als nicht regulär freigebbare Kernelressourcen markiert.

## 29. TLB-Verwaltung

Änderungen an aktiven Seitentabellen erfordern eine passende TLB-Invalidierung.

Abhängig von der Änderung kann invalidiert werden:

- eine einzelne virtuelle Adresse,
- ein Adressbereich,
- ein vollständiger Adressraum,
- ein globales Kernelmapping.

Der VMM muss alle CPUs berücksichtigen, auf denen der betroffene Adressraum aktiv ist.

## 30. TLB Shootdown

Bei Änderungen auf Mehrprozessorsystemen verwendet der VMM ein TLB-Shootdown-Protokoll.

```text
Mapping ändern
  -> aktive CPU-Maske des Adressraums bestimmen
  -> lokale TLB invalidieren
  -> Invalidierungsanfrage an andere CPUs senden
  -> Bestätigungen abwarten
  -> alte Seite erst danach freigeben
```

Timeouts oder fehlende CPU-Bestätigungen gelten als schwerer Systemfehler.

## 31. Address Space Identifier

Unterstützt die Architektur Address Space Identifier, sollen diese verwendet werden.

Sie ermöglichen:

- weniger vollständige TLB-Leerungen,
- schnellere Adressraumwechsel,
- bessere Scheduler-Performance.

ASIDs müssen sicher wiederverwendet werden. Vor einer Wiederverwendung sind veraltete TLB-Einträge zu invalidieren.

## 32. Große Seiten

Große Seiten können verwendet werden für:

- Kernel-Direktabbildungen,
- große zusammenhängende Speicherbereiche,
- geeignete Datenbanken oder Rechenanwendungen,
- ausgewählte Shared-Memory-Objekte.

Voraussetzungen sind:

- korrekte virtuelle und physische Ausrichtung,
- physisch zusammenhängender Speicher,
- einheitliche Schutzrechte,
- Unterstützung durch die Zielarchitektur.

Der VMM darf große Seiten bei Bedarf in kleinere Seiten aufteilen.

## 33. Kernel-Mappings

Globale Kernel-Mappings müssen über eine zentrale VMM-Schnittstelle eingerichtet werden.

```c
np_status_t np_vmm_map_kernel(
    np_physical_address_t physical_address,
    uint64_t length,
    uint32_t protection,
    uint32_t cache_mode,
    np_virtual_address_t* virtual_address
);

np_status_t np_vmm_unmap_kernel(
    np_virtual_address_t virtual_address,
    uint64_t length
);
```

Kernelmodule und Treiber dürfen Seitentabellen nicht direkt verändern.

## 34. Temporäre physische Abbildungen

Nicht dauerhaft direkt abgebildeter physischer Speicher wird über kontrollierte Mapping-Fenster zugänglich gemacht.

```c
np_status_t np_vmm_map_temporary(
    np_physical_address_t physical_address,
    uint32_t protection,
    np_virtual_address_t* virtual_address
);

void np_vmm_unmap_temporary(
    np_virtual_address_t virtual_address
);
```

Temporäre Abbildungen dürfen nicht über ihren vorgesehenen Gültigkeitsbereich hinaus verwendet werden.

## 35. Gerätespeicher und MMIO

MMIO-Bereiche werden mit gerätespezifischen Cacheattributen abgebildet.

```c
typedef enum np_cache_mode {
    NP_CACHE_DEFAULT,
    NP_CACHE_WRITE_BACK,
    NP_CACHE_WRITE_THROUGH,
    NP_CACHE_UNCACHED,
    NP_CACHE_WRITE_COMBINING,
    NP_CACHE_DEVICE
} np_cache_mode_t;
```

MMIO-Abbildungen müssen:

- nicht ausführbar sein,
- auf autorisierte Gerätedomains beschränkt sein,
- mit der Plattform-Speicherkarte übereinstimmen,
- die erforderliche Cachepolitik verwenden.

## 36. Speicher-Locking

Berechtigte Prozesse können Speicherbereiche gegen Paging oder Migration sperren.

```c
np_status_t np_vmm_lock(
    np_address_space_t* address_space,
    np_virtual_address_t address,
    uint64_t length
);

np_status_t np_vmm_unlock(
    np_address_space_t* address_space,
    np_virtual_address_t address,
    uint64_t length
);
```

Die Operation unterliegt:

- Prozess- und Job-Limits,
- Capability-Prüfungen,
- systemweiten Obergrenzen,
- Realtime-Richtlinien.

## 37. Commit und Reserve

NovaOS unterscheidet:

- virtuellen Adressraum reservieren,
- Speicher zusagen,
- physische Seiten resident halten.

Eine reservierte Region verbraucht zunächst nur virtuellen Adressraum.

Committed Memory garantiert, dass der Kernel die Region entsprechend seiner Speicher- und Auslagerungsrichtlinie bereitstellen kann.

Eine Zusage darf nur erfolgen, wenn Commit-Limits eingehalten werden.

## 38. Speicherfreigabe

Beim Unmapping müssen:

1. Regionen gesucht und gegebenenfalls aufgeteilt werden,
2. Seitentabelleneinträge entfernt werden,
3. TLBs invalidiert werden,
4. Mapping-Zähler reduziert werden,
5. Dirty Pages an das zuständige Memory Object gemeldet werden,
6. nicht mehr referenzierte Seiten freigegeben werden.

Physische Seiten dürfen erst nach Abschluss aller relevanten TLB-Invalidierungen wiederverwendet werden.

## 39. Adressraumzerstörung

Beim Prozessende wird der Adressraum kontrolliert zerstört.

Dazu gehören:

- neue Mappingoperationen verhindern,
- Adressraum von allen CPUs entfernen,
- alle Regionen auflösen,
- dateibasierte Dirty Pages behandeln,
- Shared-Memory-Referenzen reduzieren,
- physische Seiten freigeben,
- Seitentabellen freigeben,
- ASID zurückgeben.

Kernelanteile des Adressraums bleiben unberührt.

## 40. Scheduler-Integration

Beim Context Switch aktiviert der Scheduler den Adressraum des nächsten Threads.

Ein Adressraumwechsel umfasst abhängig von der Architektur:

- Laden des Seitentabellen-Root,
- Aktivieren des ASID,
- Aktualisieren CPU-lokaler Adressraumdaten,
- gegebenenfalls TLB-Invalidierung.

Threads desselben Prozesses teilen denselben Adressraum.

## 41. NUMA-Unterstützung

Bei der Bereitstellung physischer Seiten berücksichtigt der VMM die NUMA-Richtlinie von:

- Prozess,
- Memory Object,
- virtueller Region,
- ausführendem Thread.

Unterstützte Richtlinien umfassen:

- lokal,
- bevorzugter Node,
- Interleave,
- feste Node-Menge,
- Systemstandard.

Die virtuelle Adresse selbst legt keinen NUMA-Node fest.

## 42. Speicher-Hotplug

Vor dem Entfernen physischen Speichers müssen betroffene virtuelle Abbildungen untersucht werden.

Bewegliche Seiten können:

- auf andere physische Seiten kopiert,
- in den Seitentabellen ersetzt,
- über TLB-Shootdown aktiviert,
- anschließend aus dem alten Bereich entfernt werden.

Nicht verschiebbare Kernel-, DMA- oder MMIO-Abbildungen können die Speicherentfernung verhindern.

## 43. Capability-Rechte

Mindestens folgende Rechte werden definiert:

| Capability | Bedeutung |
|---|---|
| `CAP_VM_QUERY` | eigene Speicherregionen abfragen |
| `CAP_VM_MAP` | Speicherregionen anlegen |
| `CAP_VM_UNMAP` | Speicherregionen entfernen |
| `CAP_VM_PROTECT` | Schutzrechte ändern |
| `CAP_VM_SHARED` | Shared-Memory-Objekte abbilden |
| `CAP_VM_EXECUTE` | ausführbare Abbildungen anlegen |
| `CAP_VM_LOCK` | Speicher resident halten |
| `CAP_VM_OTHER_PROCESS` | fremden Adressraum bearbeiten |
| `CAP_VM_PHYSICAL` | kontrollierte physische Abbildungen erzeugen |
| `CAP_VM_ADMIN` | globale VMM-Richtlinien verwalten |

Rechte müssen auf konkrete Prozesse, Memory Objects oder Gerätedomains begrenzbar sein.

## 44. Userspace-Schnittstelle

Die versionierte Kernel-Service-Schnittstelle stellt mindestens bereit:

```c
np_status_t np_memory_map(
    np_handle_t memory_object,
    np_virtual_address_t requested_address,
    uint64_t length,
    uint64_t offset,
    uint32_t protection,
    uint32_t flags,
    np_virtual_address_t* mapped_address
);

np_status_t np_memory_unmap(
    np_virtual_address_t address,
    uint64_t length
);

np_status_t np_memory_protect(
    np_virtual_address_t address,
    uint64_t length,
    uint32_t protection
);

np_status_t np_memory_query(
    np_virtual_address_t address,
    np_memory_region_info_t* information
);
```

Userspace kann ausschließlich den eigenen Adressraum bearbeiten, sofern keine zusätzliche Capability vorliegt.

## 45. Fehlercodes

| Fehlercode | Bedeutung |
|---|---|
| `NP_OK` | Operation erfolgreich |
| `NP_ERR_INVALID_ARGUMENT` | ungültiger Parameter |
| `NP_ERR_INVALID_ADDRESS` | ungültige virtuelle Adresse |
| `NP_ERR_ADDRESS_IN_USE` | Bereich ist bereits belegt |
| `NP_ERR_ADDRESS_NOT_MAPPED` | Bereich ist nicht abgebildet |
| `NP_ERR_ACCESS_DENIED` | erforderliche Capability fehlt |
| `NP_ERR_PROTECTION` | unzulässige Schutzkombination |
| `NP_ERR_ALIGNMENT` | Ausrichtung ist ungültig |
| `NP_ERR_OUT_OF_MEMORY` | keine physischen Seiten verfügbar |
| `NP_ERR_COMMIT_LIMIT` | Commit-Limit überschritten |
| `NP_ERR_PAGE_FAULT` | Page Fault nicht behandelbar |
| `NP_ERR_OBJECT_INVALID` | ungültiges Memory Object |
| `NP_ERR_NOT_SUPPORTED` | Funktion wird nicht unterstützt |
| `NP_ERR_BUSY` | Region wird noch verwendet |

## 46. Sicherheit

Der VMM muss sicherstellen, dass:

- Prozessadressräume voneinander isoliert bleiben,
- Userspace keine Kernelbereiche abbilden kann,
- neue Userspace-Seiten genullt sind,
- W^X standardmäßig erzwungen wird,
- Stack- und Heapbereiche nicht ausführbar sind,
- Guard Pages nicht regulär abgebildet werden,
- MMIO ausschließlich autorisierten Gerätedomains zugänglich ist,
- physische Adressen nicht unkontrolliert offengelegt werden,
- Schutzänderungen atomar wirksam werden,
- alte TLB-Einträge vor Wiederverwendung physischer Seiten entfernt sind.

## 47. Diagnose

Der VMM stellt autorisierten Diagnosediensten bereit:

- belegte virtuelle Regionen,
- Schutzrechte,
- Memory-Object-Typ,
- residente und committed Seiten,
- Anzahl Page Faults,
- Copy-on-Write-Ereignisse,
- große Seiten,
- TLB-Shootdowns,
- NUMA-Verteilung.

Speicherinhalte und exakte physische Adressen erfordern zusätzliche Debugrechte.

## 48. Leistungszähler

```c
typedef struct np_vmm_statistics {
    uint64_t mapped_regions;
    uint64_t resident_pages;
    uint64_t committed_pages;

    uint64_t page_faults;
    uint64_t demand_faults;
    uint64_t copy_on_write_faults;
    uint64_t protection_faults;

    uint64_t tlb_invalidations;
    uint64_t tlb_shootdowns;
    uint64_t large_page_mappings;
} np_vmm_statistics_t;
```

Zähler sollen pro Prozess, Adressraum und System verfügbar sein.

## 49. Fehlerbehandlung

Nicht behandelbare Userspace-Speicherzugriffe führen zu einer strukturierten Prozess-Exception.

Sie dürfen den Kernel nicht unmittelbar zum Absturz bringen.

Folgende Situationen gelten als schwere Kernelkonsistenzfehler:

- beschädigte Seitentabellen,
- Userspace-Zugriff auf Kernelseiten trotz korrekter Privilegienstufe,
- Wiederverwendung einer Seite vor abgeschlossenem TLB-Shootdown,
- überlappende Regionen im selben Adressraum,
- inkonsistente Mapping- oder Referenzzähler,
- ausführbare und beschreibbare Kernelseite ohne autorisierte Ausnahme.

## 50. Testanforderungen

Mindestens folgende Tests sind erforderlich:

1. Erzeugung und Zerstörung eines Adressraums,
2. Abbildung einer einzelnen Seite,
3. Abbildung mehrerer Seiten,
4. Entfernen einer vollständigen Region,
5. teilweises Unmapping und Regionsaufteilung,
6. Änderung von Schutzrechten,
7. Verhinderung von Write-and-Execute,
8. Isolation zwischen zwei Prozessen,
9. Schutz des Kerneladressraums,
10. Demand Paging anonymer Seiten,
11. Nullung neuer Userspace-Seiten,
12. Copy-on-Write nach Schreibzugriff,
13. Shared Memory zwischen Prozessen,
14. private dateibasierte Abbildung,
15. gemeinsame dateibasierte Abbildung,
16. Guard-Page-Verletzung,
17. kontrolliertes Stackwachstum,
18. Ablehnung ungültigen Stackwachstums,
19. große Seiten und deren Aufteilung,
20. MMIO mit korrektem Cachemodus,
21. TLB-Invalidierung auf einer CPU,
22. TLB-Shootdown auf mehreren CPUs,
23. ASID-Wiederverwendung,
24. Adressraumwechsel durch den Scheduler,
25. Einhaltung von Commit-Limits,
26. Memory Locking und Limits,
27. NUMA-Richtlinien,
28. Migration bei Speicher-Hotplug,
29. parallele Mappingoperationen,
30. vollständige Freigabe beim Prozessende.

## 51. Verbindliche Invarianten

1. Virtuelle Regionen eines Adressraums überlappen sich nicht.
2. Userspace kann keine privilegierten Kernelbereiche erreichen.
3. Eine virtuelle Seite besitzt nur die ausdrücklich zugewiesenen Rechte.
4. Beschreibbare Seiten sind standardmäßig nicht ausführbar.
5. Guard Pages besitzen keine zugängliche reguläre Abbildung.
6. Physische Seiten werden erst nach Abschluss notwendiger TLB-Invalidierungen wiederverwendet.
7. Seitentabellenseiten werden nicht als normaler Nutzspeicher freigegeben.
8. Jeder aktive Adressraum besitzt gültige architekturspezifische Seitentabellen.
9. Userspace-Seiten enthalten bei Erstübergabe keine Daten früherer Besitzer.
10. Mapping-, Referenz- und Pinning-Zähler bleiben konsistent.
11. Ein Prozess kann ohne Capability nur seinen eigenen Adressraum verändern.
12. Adressraumänderungen erfolgen atomar gegenüber Page-Fault- und Schedulerpfaden.

## 52. Referenzablauf eines Demand Page Faults

```text
CPU meldet Page Fault
  -> Fehleradresse erfassen
  -> aktuellen Adressraum bestimmen
  -> virtuelle Region suchen
  -> Zugriffsrechte prüfen
  -> Memory Object bestimmen
  -> physische Seite vom PMM anfordern
  -> Seite nullen oder Inhalt laden
  -> Seitentabelleneintrag erzeugen
  -> TLB aktualisieren
  -> Statistik aktualisieren
  -> fehlerhafte Instruktion erneut ausführen
```

## 53. Zusammenfassung

Der NovaOS Virtual Memory Manager stellt isolierte virtuelle Adressräume für Kernel und Userspace-Prozesse bereit.

Virtuelle Regionen werden mit klaren Schutzrechten und Memory Objects verbunden. Demand Paging, Copy-on-Write, Shared Memory, dateibasierte Abbildungen und große Seiten ermöglichen eine flexible und effiziente Speichernutzung.

W^X, Guard Pages, ASLR, Capability-Prüfungen und sichere TLB-Verwaltung schützen das System vor unzulässigen Speicherzugriffen und der Offenlegung von Daten zwischen Sicherheitsdomains.