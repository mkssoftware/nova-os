# ADR-DEV-0010 – DMA Framework

| Feld | Wert |
|---|---|
| Dokument-ID | ADR-DEV-0010 |
| Titel | DMA Framework |
| Status | Angenommen |
| Version | 1.0 |
| Datum | 2026-07-31 |
| Bereich | Geräteverwaltung und Speicherzugriff |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | ADR-DEV-0001, ADR-DEV-0002, ADR-DEV-0007, ADR-DEV-0009, ADR-MM-0014 |
| Nachfolgende Spezifikation | NPSPEC-DEV-DMA-FRAMEWORK |

---

## 1. Kontext

Direct Memory Access ermöglicht Geräten, Daten ohne fortlaufende CPU-Kopieroperationen zwischen Geräteschnittstelle und Arbeitsspeicher zu übertragen.

DMA wird unter anderem benötigt für:

- Massenspeicher
- Netzwerkadapter
- Grafikadapter
- USB-Controller
- Audiohardware
- Kameras
- Beschleuniger
- Hochgeschwindigkeitsgeräte

Da DMA die gewöhnliche virtuelle Speicherisolation umgehen kann, benötigt NovaOS ein zentrales und capability-geschütztes DMA Framework.

---

## 2. Problemstellung

Geräte dürfen nicht auf beliebige physische Speicherbereiche zugreifen.

Das Framework muss sicherstellen, dass:

- nur autorisierte Speicherbereiche für DMA freigegeben werden,
- Geräte keine Kernel- oder fremden Prozessdaten lesen können,
- DMA-Adressgrenzen des Geräts eingehalten werden,
- Cache-Kohärenz korrekt behandelt wird,
- Scatter/Gather unterstützt wird,
- DMA beim Geräte- oder Treiberende widerrufen wird,
- Systeme ohne IOMMU einen sicheren Ersatzpfad besitzen.

---

## 3. Entscheidung

NovaOS implementiert ein zentrales DMA Framework zwischen Treibern, Memory Manager, Device Manager und IOMMU.

Treiber erhalten DMA-Adressen ausschließlich über das Framework. Physische Speicheradressen dürfen nicht direkt als Geräteadressen verwendet werden.

Das Framework unterstützt drei Backends:

| Backend | Verwendung |
|---|---|
| IOMMU Mapping | Bevorzugter sicherer Pfad |
| Direct DMA | Nur für kontrollierte, sichere Konfigurationen |
| Bounce Buffer | Ersatz für eingeschränkte oder ältere Hardware |

---

## 4. Architektur

```text
Treiber
   |
   v
DMA Framework
   |
   +--> IOMMU Backend
   +--> Direct-DMA Backend
   +--> Bounce-Buffer Backend
   |
   v
Speicherverwaltung
```

Das gewählte Backend bleibt für den Treiber weitgehend transparent.

Der Treiber verwendet ausschließlich die vom Framework zurückgegebenen DMA-Adressen und Mapping-Handles.

---

## 5. DMA-Kontext

Jedes DMA-fähige Gerät erhält einen eigenen DMA-Kontext.

```c
typedef struct dma_context_info {
    uint64_t context_id;
    uint64_t device_id;
    uint64_t address_mask;
    uint32_t coherency;
    uint32_t backend;
    uint32_t max_segments;
    uint32_t flags;
} dma_context_info_t;
```

Der Kontext beschreibt:

- zugehöriges Gerät
- erreichbaren DMA-Adressraum
- IOMMU-Domain
- Kohärenzeigenschaften
- Segmentgrenzen
- Ausrichtungsanforderungen
- aktives Sicherheitsbackend

DMA-Kontexte dürfen nicht unkontrolliert zwischen Geräten geteilt werden.

---

## 6. Capability-Modell

Ein Treiber erhält eine Capability für den DMA-Kontext seines Geräts.

| Recht | Bedeutung |
|---|---|
| `DMA_RIGHT_MAP` | Speicher für DMA abbilden |
| `DMA_RIGHT_UNMAP` | Mapping entfernen |
| `DMA_RIGHT_ALLOCATE` | DMA-Puffer erzeugen |
| `DMA_RIGHT_SYNC` | Cache-Synchronisation ausführen |
| `DMA_RIGHT_INSPECT` | Eigenschaften des Kontexts lesen |
| `DMA_RIGHT_ADMIN` | DMA-Domain verwalten |

Normale Treiber erhalten kein `DMA_RIGHT_ADMIN`.

Der DMA-Kontext darf nur Speicherobjekte abbilden, für die der Treiber eine passende Capability besitzt.

---

## 7. DMA-Richtungen

NovaOS unterscheidet:

```c
#define DMA_DIRECTION_TO_DEVICE       1
#define DMA_DIRECTION_FROM_DEVICE     2
#define DMA_DIRECTION_BIDIRECTIONAL   3
```

| Richtung | Bedeutung |
|---|---|
| To Device | Gerät liest aus dem Speicher |
| From Device | Gerät schreibt in den Speicher |
| Bidirectional | Beide Richtungen sind zulässig |

Die Zugriffsrechte des IOMMU-Mappings werden entsprechend eingeschränkt.

`BIDIRECTIONAL` soll nur verwendet werden, wenn beide Richtungen tatsächlich erforderlich sind.

---

## 8. Streaming-Mappings

Streaming-Mappings werden für vorübergehende Übertragungen verwendet.

```c
status_t dma_map(
    cap_handle_t dma_context,
    cap_handle_t memory_object,
    uint64_t offset,
    uint64_t length,
    uint32_t direction,
    dma_mapping_t *mapping
);
```

Das Ergebnis enthält:

- Mapping-ID
- DMA-Adresse oder Segmentliste
- abgebildete Länge
- verwendetes Backend
- Synchronisationsanforderungen

Nach Abschluss der Übertragung muss das Mapping wieder entfernt werden.

---

## 9. Kohärente DMA-Puffer

Für Descriptor-Ringe und dauerhaft gemeinsam verwendete Kontrollstrukturen können kohärente DMA-Puffer erzeugt werden:

```c
status_t dma_allocate_coherent(
    cap_handle_t dma_context,
    uint64_t size,
    uint64_t alignment,
    cap_handle_t *memory_object,
    dma_address_t *device_address
);
```

Das Framework stellt sicher, dass CPU und Gerät die erforderliche Speicherkonsistenz erhalten.

„Kohärent“ bedeutet nicht, dass auf Memory Barriers verzichtet werden darf. Sichtbarkeits- und Reihenfolgeregeln bleiben architekturabhängig.

---

## 10. Scatter/Gather

Nicht zusammenhängende Speicherseiten können als Segmentliste abgebildet werden:

```c
typedef struct dma_segment {
    dma_address_t address;
    uint64_t length;
} dma_segment_t;
```

Das Framework berücksichtigt:

- maximale Segmentanzahl
- maximale Segmentgröße
- Segmentgrenzen
- Ausrichtung
- Geräteadressbreite
- IOMMU-Seitengröße

Wenn ein Gerät die resultierende Liste nicht unterstützt, verwendet NovaOS ein Bounce Buffer oder lehnt die Operation ab.

---

## 11. Adressgrenzen

Geräte können auf unterschiedliche Adressbreiten begrenzt sein, beispielsweise:

- 24 Bit
- 32 Bit
- 40 Bit
- 64 Bit

Der Treiber teilt diese Grenze nicht frei mit, sondern sie wird aus Gerätebeschreibung, Businformationen und validiertem Treibermanifest abgeleitet.

Das Framework garantiert, dass alle ausgegebenen DMA-Adressen innerhalb der zulässigen Adressmaske liegen.

---

## 12. IOMMU

Wenn eine kompatible IOMMU verfügbar ist, erhält jedes Gerät oder jede sichere Gerätegruppe eine eigene IOMMU-Domain.

Die IOMMU wird verwendet für:

- Begrenzung erreichbarer Speicherbereiche
- getrennte Geräteadressräume
- kontrollierte DMA-Rechte
- Interrupt Remapping, sofern unterstützt
- Erkennung unzulässiger Gerätezugriffe

IOMMU-Fehler werden dem Device Manager und dem Audit-System gemeldet.

Eine vorhandene IOMMU wird standardmäßig aktiviert, sofern sie zuverlässig unterstützt wird.

---

## 13. Systeme ohne IOMMU

NovaOS muss auch auf älteren Systemen ohne IOMMU funktionieren.

In diesem Fall gelten strengere Regeln:

- bevorzugte Verwendung reservierter DMA-Pools
- Bounce Buffer für nicht direkt erreichbaren Speicher
- keine Freigabe beliebiger Userspace-Seiten
- zusätzliche Begrenzung von Userspace-Treibern
- sichtbarer reduzierter Isolationsstatus
- sichere Bereinigung aller temporären Puffer

Fehlende IOMMU-Unterstützung darf nicht fälschlich als vollständige DMA-Isolation dargestellt werden.

---

## 14. Bounce Buffer

Ein Bounce Buffer wird eingesetzt, wenn:

- das Gerät den Zielbereich nicht adressieren kann,
- kein sicheres direktes Mapping möglich ist,
- Ausrichtungs- oder Segmentbedingungen verletzt werden,
- das Sicherheitsprofil direkte DMA-Zugriffe verbietet.

Bei `TO_DEVICE` kopiert das Framework Daten vor der Übertragung in den Bounce Buffer.

Bei `FROM_DEVICE` werden die Daten nach Abschluss in den Zielpuffer kopiert.

Bounce Buffer werden vor Wiederverwendung vollständig bereinigt.

---

## 15. Cache-Synchronisation

Auf nicht vollständig kohärenten Plattformen benötigt DMA explizite Synchronisationsoperationen:

```c
status_t dma_sync_for_device(
    dma_mapping_t *mapping,
    uint64_t offset,
    uint64_t length
);

status_t dma_sync_for_cpu(
    dma_mapping_t *mapping,
    uint64_t offset,
    uint64_t length
);
```

Der Treiber muss die Eigentumsübergänge zwischen CPU und Gerät einhalten.

Das Framework führt erforderliche Cache-Operationen und Memory Barriers für die jeweilige Architektur aus.

---

## 16. Pinning und Speicherlebenszeit

Für DMA abgebildete Speicherseiten dürfen während des aktiven Mappings nicht:

- freigegeben,
- ausgelagert,
- physisch verschoben,
- durch Copy-on-Write ersetzt,
- mit inkompatiblen Rechten neu verwendet werden.

Das DMA Framework hält deshalb eine Referenz auf das Speicherobjekt und pinnt die betroffenen Seiten.

Pinning unterliegt festen Prozess-, Treiber- und Systemlimits.

---

## 17. Mapping beenden

Ein Mapping wird kontrolliert entfernt:

```c
status_t dma_unmap(
    cap_handle_t dma_context,
    dma_mapping_t *mapping
);
```

Vor der Freigabe:

1. muss der Gerätetransfer beendet sein,
2. werden notwendige Cache-Synchronisationen ausgeführt,
3. wird das IOMMU-Mapping entfernt,
4. werden IOTLB-Einträge invalidiert,
5. werden Bounce-Buffer-Daten kopiert,
6. werden Seiten entpinnt,
7. wird das Mapping ungültig.

Ein Mapping-Handle darf nach erfolgreichem Unmap nicht erneut verwendet werden.

---

## 18. Geräteentfernung und Treiberabsturz

Beim Entfernen eines Geräts oder Absturz seines Treibers:

1. wird Bus Mastering deaktiviert,
2. werden neue DMA-Mappings abgelehnt,
3. werden aktive Hardwarewarteschlangen gestoppt,
4. werden IOMMU-Mappings widerrufen,
5. werden ausstehende Transfers beendet,
6. werden Seiten und Bounce Buffer freigegeben,
7. wird der DMA-Kontext zerstört.

Kann Bus Mastering nicht zuverlässig gestoppt werden, muss das Gerät über IOMMU, Bus-Reset oder Plattformmechanismen isoliert werden.

---

## 19. DMA-Pools

Das Framework kann wiederverwendbare DMA-Pools für häufig benötigte Objekte anbieten:

```c
status_t dma_pool_create(
    cap_handle_t dma_context,
    uint64_t object_size,
    uint64_t alignment,
    uint32_t object_count,
    cap_handle_t *pool_handle
);
```

Typische Anwendungen sind:

- Transferdeskriptoren
- Netzwerkpaketpuffer
- Command-Ringe
- Audiosegmente

Pools bleiben begrenzt und werden einem konkreten DMA-Kontext zugeordnet.

---

## 20. Sicherheit

Für das DMA Framework gelten folgende Regeln:

- Treiber erhalten niemals beliebige physische Adressen.
- Jedes Mapping benötigt eine gültige DMA- und Speicher-Capability.
- Zugriffsrichtung wird in Hardware oder IOMMU eingeschränkt, wenn möglich.
- Neue und wiederverwendete Puffer werden bereinigt.
- DMA-Mappings sind zeitlich und räumlich begrenzt.
- Bus Mastering bleibt bis zur sicheren Initialisierung deaktiviert.
- Userspace-Speicher wird nicht ohne kontrolliertes Pinning freigegeben.
- IOMMU-Verstöße führen zur Isolation des betroffenen Geräts.

---

## 21. Fehlercodes

| Fehlercode | Bedeutung |
|---|---|
| `STATUS_OK` | Operation erfolgreich |
| `STATUS_INVALID_HANDLE` | Ungültige DMA-Capability |
| `STATUS_ACCESS_DENIED` | Erforderliches Recht fehlt |
| `STATUS_DMA_ADDRESS_UNREACHABLE` | Gerät kann den Bereich nicht adressieren |
| `STATUS_DMA_TOO_MANY_SEGMENTS` | Segmentlimit überschritten |
| `STATUS_DMA_ALIGNMENT_ERROR` | Ausrichtung ist nicht zulässig |
| `STATUS_DMA_MAPPING_FAILED` | Mapping konnte nicht erstellt werden |
| `STATUS_DMA_NOT_COHERENT` | Synchronisation ist erforderlich |
| `STATUS_DEVICE_REMOVED` | Gerät wurde entfernt |
| `STATUS_LIMIT_EXCEEDED` | Pinning- oder Ressourcenlimit überschritten |
| `STATUS_BUSY` | Mapping wird noch vom Gerät verwendet |

---

## 22. Audit und Diagnose

Das Framework erfasst lokal:

- aktive DMA-Mappings
- gepinnte Speichergröße
- verwendetes Backend
- IOMMU-Domain
- Bounce-Buffer-Nutzung
- Mapping-Fehler
- IOMMU-Verstöße
- fehlgeschlagene Widerrufe

Auditiert werden insbesondere:

- unzulässige DMA-Zugriffe
- Überschreitung von Pinning-Limits
- Verwendung unsicherer Direct-DMA-Pfade
- DMA nach Geräteentfernung
- administrative Änderungen an IOMMU-Domains

Speicherinhalte werden nicht protokolliert.

---

## 23. Alternative Ansätze

### 23.1 Direkte physische Adressen für Treiber

**Vorteile:**

- einfache und schnelle Implementierung

**Nachteile:**

- keine Isolation
- Zugriff auf fremden oder Kernel-Speicher möglich
- ungeeignet für Userspace-Treiber

**Entscheidung:** Abgelehnt.

### 23.2 IOMMU zwingend voraussetzen

**Vorteile:**

- starke DMA-Isolation

**Nachteile:**

- ältere Hardware wäre nicht nutzbar
- widerspricht dem NovaOS-Ziel breiter Hardwareunterstützung

**Entscheidung:** Abgelehnt. Systeme ohne IOMMU verwenden eingeschränkte Ersatzpfade.

### 23.3 Ausschließlich Bounce Buffer

**Vorteile:**

- kontrollierter Speicherbereich
- funktioniert auf alter Hardware

**Nachteile:**

- zusätzliche Kopieroperationen
- hoher Speicher- und Leistungsaufwand

**Entscheidung:** Nur als Fallback oder für besondere Sicherheitsanforderungen.

---

## 24. Konsequenzen

### Positive Konsequenzen

- zentrale DMA-Sicherheitskontrolle
- Unterstützung für IOMMU und ältere Hardware
- einheitliche API für Treiber
- kontrolliertes Pinning
- Scatter/Gather-Unterstützung
- sichere Behandlung von Geräteentfernung
- geeignet für Userspace-Treiber

### Negative Konsequenzen

- komplexe Speicher- und Cacheverwaltung
- Bounce Buffer reduzieren die Leistung
- Pinning kann verfügbaren Speicher einschränken
- unterschiedliche Plattformen benötigen eigene Backends

### Risiken

- DMA nach Unmap
- fehlerhafte Cache-Synchronisation
- IOMMU-Fehlkonfiguration
- Pinning-basierte Ressourcenerschöpfung
- Datenlecks durch nicht bereinigte Bounce Buffer
- Race Conditions beim Treiberabsturz

---

## 25. Implementierungsrichtlinien

- DMA-Adressen werden ausschließlich vom Framework erzeugt.
- Jedes Gerät erhält einen getrennten DMA-Kontext oder eine begründete sichere Gruppe.
- IOMMU wird bevorzugt, wenn verfügbar.
- Fehlende IOMMU-Isolation muss sichtbar gemeldet werden.
- Gepinnte Seiten unterliegen festen Ressourcenlimits.
- DMA-Richtung wird so eng wie möglich festgelegt.
- Unmap erfolgt erst nach sicherem Abschluss des Gerätetransfers.
- Geräteentfernung widerruft alle DMA-Mappings.
- Bounce Buffer werden vor Freigabe und Wiederverwendung bereinigt.
- Cache- und Memory-Barrier-Regeln werden architekturspezifisch umgesetzt.

---

## 26. Testanforderungen

Die Implementierung benötigt mindestens Tests für:

- IOMMU-, Direct-DMA- und Bounce-Buffer-Backend
- 32-Bit- und 64-Bit-DMA-Adressen
- Scatter/Gather
- Segment- und Ausrichtungsgrenzen
- kohärente und nicht kohärente Systeme
- Cache-Synchronisation
- Pinning-Limits
- Geräteentfernung während eines Transfers
- Treiberabsturz
- IOMMU-Verstöße
- DMA-Pools
- parallele Mappings
- Speicherobjektzerstörung bei aktivem Mapping
- Systeme ohne IOMMU

---

## 27. Verbindliche Invarianten

1. Treiber erhalten DMA-Adressen ausschließlich über das DMA Framework.
2. Physische Speicheradressen stellen keine DMA-Berechtigung dar.
3. Jedes Mapping benötigt gültige Geräte- und Speicher-Capabilities.
4. Bus Mastering bleibt bis zur sicheren DMA-Initialisierung deaktiviert.
5. Abgebildete Seiten bleiben während des Mappings gepinnt.
6. DMA-Mappings werden beim Geräte- oder Treiberende widerrufen.
7. Eine vorhandene und unterstützte IOMMU wird bevorzugt verwendet.
8. Systeme ohne IOMMU verwenden eingeschränkte und sichtbar degradierte Sicherheitsmodi.
9. Bounce Buffer werden vor Wiederverwendung bereinigt.
10. DMA-Speicherinhalte werden nicht standardmäßig protokolliert.

---

## 28. Zusammenfassung

NovaOS implementiert ein zentrales, capability-basiertes DMA Framework für sichere und effiziente Gerätetransfers.

Das Framework verwaltet DMA-Kontexte, IOMMU-Domains, Scatter/Gather-Listen, Cache-Synchronisation, Pinning und Bounce Buffer. Treiber erhalten keine beliebigen physischen Adressen, sondern ausschließlich kontrollierte DMA-Mappings.

Moderne Systeme verwenden bevorzugt IOMMU-Isolation. Ältere Hardware bleibt über begrenzte Direct-DMA- oder Bounce-Buffer-Pfade nutzbar, wobei der reduzierte Sicherheitsstatus klar erkennbar bleibt.