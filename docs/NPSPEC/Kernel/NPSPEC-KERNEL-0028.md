# NPSPEC-KERNEL-0028 – NUMA Support

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-KERNEL-0028 |
| Titel | NUMA Support |
| Status | Spezifikation |
| Version | 1.0 |
| Datum | 2026-08-03 |
| Bereich | Kernel / Speicher- und Prozessorarchitektur |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | NPSPEC-KERNEL-0003, NPSPEC-KERNEL-0006, NPSPEC-KERNEL-0007, NPSPEC-KERNEL-0017, NPSPEC-KERNEL-0021, NPSPEC-KERNEL-0026, NPSPEC-KERNEL-0027 |
| Zugehörige ADRs | ADR-MM-0011, ADR-MM-0012, ADR-MM-0014, ADR-PROC-0007, ADR-DEV-0010 |

---

## 1. Zweck

Diese Spezifikation definiert die Non-Uniform Memory Access-Unterstützung von NovaOS.

Die NUMA-Architektur verwaltet:

- NUMA-Nodes,
- CPU- und Speicherzuordnung,
- Entfernungsmatrizen,
- Speicherallokationsrichtlinien,
- Thread- und Prozessplatzierung,
- automatische Seitenmigration,
- Device Locality,
- Speicher- und CPU-Hotplug,
- Diagnose und Ressourcenabrechnung.

## 2. Ziele

NUMA Support muss:

- Systeme ohne NUMA weiterhin effizient unterstützen,
- lokalen Speicher bevorzugen,
- Remote-Zugriffe kontrolliert begrenzen,
- explizite Anwendungsrichtlinien ermöglichen,
- Scheduler und Speicherverwaltung koordinieren,
- Geräte- und DMA-Lokalität berücksichtigen,
- heterogene Speicherarten darstellen,
- NUMA-Hotplug unterstützen,
- unnötige Seiten- und Threadmigrationen vermeiden.

## 3. Grundmodell

NovaOS behandelt UMA-Systeme als NUMA-System mit genau einem Node.

Dadurch verwenden Kernelkomponenten dieselben Schnittstellen unabhängig von der tatsächlichen Hardwaretopologie.

```text
UMA:
  Node 0
    -> alle CPUs
    -> gesamter Arbeitsspeicher
    -> alle lokalen Geräte
```

## 4. NUMA-Begriffe

| Begriff | Bedeutung |
|---|---|
| Node | Gruppe von CPUs, Speicher und Geräten mit ähnlicher Lokalität |
| Local Memory | Speicher des aktuellen oder bevorzugten Nodes |
| Remote Memory | Speicher eines anderen Nodes |
| Distance | relative Zugriffskosten zwischen Nodes |
| Home Node | bevorzugter Node eines Prozesses oder Speicherobjekts |
| Interleave | Verteilung von Seiten über mehrere Nodes |
| Memoryless Node | Node mit CPUs, aber ohne eigenen Speicher |
| CPU-less Node | Node mit Speicher, aber ohne aktive CPU |

## 5. Node-Kennung

```c
typedef uint32_t np_numa_node_id_t;
```

Jeder Node erhält eine innerhalb des Bootvorgangs eindeutige Node ID.

Die Node ID:

- ist nicht zwingend identisch mit der Firmwarekennung,
- ist keine Capability,
- bleibt während der Lebenszeit des Nodes stabil,
- darf nach physischer Entfernung nicht sofort unkontrolliert wiederverwendet werden.

## 6. NUMA-Node-Objekt

```c
typedef struct np_numa_node {
    np_kernel_object_t object;

    np_numa_node_id_t node_id;
    np_numa_node_hardware_id_t hardware_id;
    np_numa_node_state_t state;

    np_cpu_set_t cpus;
    np_memory_range_list_t memory_ranges;
    np_device_set_t devices;

    np_numa_statistics_t statistics;
    uint32_t flags;

    np_rwlock_t lock;
} np_numa_node_t;
```

NUMA-Nodes werden in den Kernel Object Graph aufgenommen.

## 7. Node-Zustände

```c
typedef enum np_numa_node_state {
    NP_NUMA_NODE_DISCOVERED,
    NP_NUMA_NODE_INITIALIZING,
    NP_NUMA_NODE_ONLINE,
    NP_NUMA_NODE_DEGRADED,
    NP_NUMA_NODE_OFFLINING,
    NP_NUMA_NODE_OFFLINE,
    NP_NUMA_NODE_FAILED,
    NP_NUMA_NODE_REMOVED
} np_numa_node_state_t;
```

Nur online oder entsprechend unterstützte degraded Nodes dürfen reguläre Allokationen bedienen.

## 8. Topologieerkennung

NUMA-Informationen können stammen aus:

- ACPI SRAT,
- ACPI SLIT,
- Device Tree,
- Firmwaretabellen,
- Hypervisorinformationen,
- architekturspezifischen Registern,
- statischen Plattformdefinitionen.

Alle Angaben werden auf Konsistenz und gültige Bereiche geprüft.

## 9. Topologienormalisierung

Der Kernel normalisiert Firmwareinformationen in ein einheitliches Modell.

Dabei werden:

- Firmware-IDs auf Kernel-Node-IDs abgebildet,
- CPUs Nodes zugeordnet,
- Speicherbereiche Nodes zugeordnet,
- Geräte- und Buslokalität ermittelt,
- Distanzwerte normalisiert,
- fehlende oder widersprüchliche Angaben markiert.

Unbekannte Lokalität wird nicht automatisch als lokal behandelt.

## 10. Entfernungsmatrix

```c
typedef uint16_t np_numa_distance_t;
```

Der Abstand eines Nodes zu sich selbst besitzt den normalisierten Basiswert:

```text
local distance = 10
```

Remote-Werte sind größer als der lokale Wert.

```c
np_numa_distance_t np_numa_distance(
    np_numa_node_id_t source,
    np_numa_node_id_t target
);
```

Die konkreten Werte stellen relative Kosten und keine direkte Zeitmessung dar.

## 11. Symmetrische und asymmetrische Distanzen

NovaOS darf nicht voraussetzen, dass:

```text
distance(A, B) == distance(B, A)
```

Firmware oder Plattformen können asymmetrische Kosten melden.

Fehlen genaue Daten, darf der Kernel eine konservative symmetrische Standardmatrix erzeugen und diesen Zustand diagnostisch kennzeichnen.

## 12. Node-Mengen

```c
typedef struct np_numa_node_set {
    uint64_t bits[NP_NUMA_NODE_SET_WORDS];
} np_numa_node_set_t;
```

Der Kernel verwaltet mindestens:

- mögliche Nodes,
- erkannte Nodes,
- Online-Nodes,
- Nodes mit Speicher,
- Nodes mit CPUs,
- Nodes mit Geräten,
- fehlgeschlagene Nodes.

Die Struktur muss für zukünftige Systeme mit vielen Nodes erweiterbar sein.

## 13. CPU-Zuordnung

Jede aktive CPU ist genau einem primären NUMA-Node zugeordnet.

```c
np_numa_node_id_t np_numa_node_for_cpu(
    np_cpu_id_t cpu_id
);
```

CPU Hotplug aktualisiert die CPU-Mengen des Nodes.

Ein CPU-loser Node kann weiterhin Speicher bereitstellen.

## 14. Speicherzuordnung

Jeder verwaltete physische Speicherbereich wird einem NUMA-Node zugeordnet.

```c
np_numa_node_id_t np_numa_node_for_physical_address(
    np_physical_address_t address
);
```

Ein Bereich darf nicht gleichzeitig mehreren primären Nodes zugeordnet sein.

Interleaved Hardwarebereiche benötigen eine ausdrücklich unterstützte Darstellung.

## 15. Gerätezordnung

Geräte können einem bevorzugten NUMA-Node zugeordnet werden anhand von:

- Bus-Root,
- PCI-Segment,
- Firmwareangaben,
- IOMMU-Gruppe,
- Interrupt-Topologie,
- Plattformbeschreibung.

```c
np_numa_node_id_t np_numa_node_for_device(
    np_device_handle_t device
);
```

Unbekannte Gerätelokalität wird mit `NP_NUMA_NODE_UNKNOWN` dargestellt.

## 16. NUMA-Richtlinien

```c
typedef enum np_numa_policy_type {
    NP_NUMA_POLICY_DEFAULT,
    NP_NUMA_POLICY_LOCAL,
    NP_NUMA_POLICY_PREFERRED,
    NP_NUMA_POLICY_BIND,
    NP_NUMA_POLICY_INTERLEAVE,
    NP_NUMA_POLICY_REPLICATE
} np_numa_policy_type_t;
```

| Richtlinie | Verhalten |
|---|---|
| `DEFAULT` | Systemstandard verwenden |
| `LOCAL` | Node der ausführenden CPU bevorzugen |
| `PREFERRED` | angegebenen Node bevorzugen, Fallback erlaubt |
| `BIND` | ausschließlich angegebene Nodes verwenden |
| `INTERLEAVE` | Seiten über Node-Menge verteilen |
| `REPLICATE` | schreibgeschützte Daten auf mehreren Nodes replizieren |

## 17. Richtlinienobjekt

```c
typedef struct np_numa_policy {
    np_numa_policy_type_t type;
    np_numa_node_set_t nodes;

    np_numa_node_id_t preferred_node;
    uint32_t flags;
} np_numa_policy_t;
```

Eine Richtlinie muss mindestens einen gültigen Node enthalten, wenn ihr Typ dies erfordert.

## 18. Richtlinienebenen

NUMA-Richtlinien können gelten für:

- System,
- Job Object,
- Prozess,
- Thread,
- virtuellen Speicherbereich,
- Memory Object,
- Kernelallokation,
- Gerät oder DMA-Objekt.

Die spezifischste gültige Richtlinie hat Vorrang, darf aber zwingende System- oder Jobgrenzen nicht überschreiten.

## 19. Standardrichtlinie

Die Standardrichtlinie für regulären Speicher ist `LOCAL`.

Dabei wird Speicher bevorzugt vom Node der CPU allokiert, die den Page Fault oder die Allokation ausführt.

Ist lokaler Speicher nicht verfügbar, wird anhand der Entfernungsmatrix auf einen geeigneten anderen Node ausgewichen.

## 20. Preferred Policy

`PREFERRED` versucht zuerst den angegebenen Node.

Ist dort keine geeignete Seite verfügbar, werden alternative Nodes nach folgenden Kriterien gewählt:

1. zulässige Node-Menge,
2. Distanz,
3. verfügbare Speicherzone,
4. Speicherknappheit,
5. Thermal- und Powerstatus.

Der bevorzugte Node bleibt auch nach Fallback gespeichert.

## 21. Bind Policy

`BIND` beschränkt Allokationen auf eine festgelegte Node-Menge.

Ist innerhalb dieser Menge kein Speicher verfügbar, schlägt die Allokation fehl, auch wenn andere Nodes freien Speicher besitzen.

Diese Richtlinie ist für Realtime, Datenlokalität und isolierte Workloads vorgesehen.

Sie muss wegen des erhöhten OOM-Risikos bewusst eingesetzt werden.

## 22. Interleave Policy

`INTERLEAVE` verteilt aufeinanderfolgende Seiten über die erlaubten Nodes.

```text
Page 0 -> Node 0
Page 1 -> Node 1
Page 2 -> Node 2
Page 3 -> Node 0
```

Die Verteilung berücksichtigt nur online verfügbare Nodes mit passendem Speicher.

Sie eignet sich für große Datenmengen mit parallelem Zugriff mehrerer Nodes.

## 23. Replicate Policy

Schreibgeschützte Daten können auf mehreren Nodes repliziert werden.

Geeignete Beispiele sind:

- häufig gelesene unveränderliche Bibliotheksseiten,
- Kernel-Lookup-Tabellen,
- schreibgeschützte Konfigurationsdaten.

Schreibzugriffe erfordern:

- Auflösung der Replikation,
- Copy-on-Write,
- oder Rückkehr zu einer einzelnen kanonischen Seite.

## 24. Prozessrichtlinie

```c
np_status_t np_process_set_numa_policy(
    np_process_handle_t process,
    const np_numa_policy_t* policy
);
```

Die Prozessrichtlinie gilt als Standard für neue:

- anonyme Speicherregionen,
- Thread-Stacks,
- Heap-Seiten,
- private Memory Objects.

Bestehende Seiten werden nicht automatisch migriert, sofern dies nicht ausdrücklich angefordert wird.

## 25. Threadrichtlinie

Ein Thread kann eine abweichende Richtlinie besitzen.

```c
np_status_t np_thread_set_numa_policy(
    np_thread_handle_t thread,
    const np_numa_policy_t* policy
);
```

Sie beeinflusst Allokationen, die im Kontext dieses Threads ausgelöst werden.

Die Richtlinie verändert nicht automatisch seine CPU-Affinität.

## 26. Speicherbereichsrichtlinie

```c
np_status_t np_memory_set_numa_policy(
    np_virtual_address_t address,
    uint64_t length,
    const np_numa_policy_t* policy,
    uint32_t flags
);
```

Die Region muss vollständig zum Adressraum des Aufrufers gehören.

Eine Änderung kann nur zukünftige Seiten betreffen oder zusätzlich eine kontrollierte Migration auslösen.

## 27. First-Touch Allocation

Bei anonymer Speicherreservierung werden physische Seiten standardmäßig beim ersten Zugriff zugewiesen.

Der Node wird anhand folgender Daten gewählt:

- Richtlinie der Region,
- Threadrichtlinie,
- Prozessrichtlinie,
- ausführende CPU,
- zulässige Node-Menge.

Dadurch kann eine parallele Initialisierung Daten automatisch auf mehrere Nodes verteilen.

## 28. Kernelallokationen

Kernelallokationen können einen bevorzugten Node angeben.

```c
void* np_kalloc_on_node(
    size_t size,
    np_numa_node_id_t node_id,
    uint32_t flags
);
```

Per-CPU- und gerätenahe Daten sollen bevorzugt auf dem zugehörigen Node liegen.

Globale selten veränderte Daten können interleaved oder repliziert werden.

## 29. Physical Memory Manager

Der PMM verwaltet getrennte Speicherzonen pro Node.

```text
Node
  -> DMA Zone
  -> DMA32 Zone
  -> Normal Zone
  -> High Zone
  -> Persistent Zone
```

Buddy-Blöcke dürfen nicht über Nodegrenzen zusammengeführt werden.

Statistiken und Wassermarken werden pro Node und Zone geführt.

## 30. Virtual Memory Manager

Der VMM speichert die NUMA-Richtlinie an:

- Adressräumen,
- virtuellen Regionen,
- Memory Objects,
- Shared-Memory-Objekten.

Page-Fault-Allokationen übergeben die effektive Richtlinie an den PMM.

Copy-on-Write-Seiten werden standardmäßig nahe beim schreibenden Thread erzeugt, sofern die Richtlinie nichts anderes vorgibt.

## 31. Shared Memory

Shared Memory kann eine eigene NUMA-Richtlinie besitzen.

Mögliche Strategien sind:

- bevorzugter Owner-Node,
- Interleave,
- Bind an eine Node-Menge,
- Read-only-Replikation,
- adaptive Migration anhand der Nutzer.

Die Richtlinie muss Konflikte zwischen mehreren Prozessen eindeutig behandeln.

## 32. File Cache

Dateibasierte Cache-Seiten können platziert werden anhand von:

- Node des ersten Lesers,
- Node des Storage-Geräts,
- Interleave-Richtlinie,
- Zugriffsmuster,
- Replikationsrichtlinie.

Einzelne File-Cache-Seiten dürfen nicht ohne geeignete Kohärenz mehrfach beschreibbar repliziert werden.

## 33. Scheduler-Integration

Der Scheduler berücksichtigt:

- Home Node des Prozesses,
- Speicherverteilung des Threads,
- CPU-Auslastung,
- CPU-Kapazität,
- Cache-Topologie,
- Geräte- und Interruptlokalität,
- Migrationskosten.

Thread- und Speichermigration werden gemeinsam bewertet, aber getrennt ausgeführt.

## 34. Wakeup-Platzierung

Beim Aufwecken eines Threads wird bevorzugt eine CPU gewählt, die:

- durch seine Affinität erlaubt ist,
- nahe an seinen aktiven Speicherseiten liegt,
- ausreichende Kapazität besitzt,
- keine unvertretbar hohe Last hat,
- passende CPU-Features unterstützt.

Eine kleine Lastdifferenz rechtfertigt keine entfernte NUMA-Migration.

## 35. NUMA Load Balancing

NUMA Load Balancing kann Threads zwischen Nodes verschieben, wenn:

- der aktuelle Node stark überlastet ist,
- ein anderer Node deutlich mehr Zugriffslokalität bietet,
- CPU-Affinität dies erlaubt,
- Migrationskosten vertretbar sind.

Kurzfristige Lastspitzen dürfen keine dauerhaften Nodewechsel verursachen.

## 36. Zugriffsstatistik

NovaOS kann NUMA-Zugriffsmuster erfassen durch:

- Hardwarezähler,
- Page-Fault-Sampling,
- Access-Bit-Sampling,
- Scheduler- und VMM-Statistiken,
- anwendungsseitige Hinweise.

Sampling muss begrenzt sein und darf den normalen Speicherzugriff nicht unverhältnismäßig belasten.

## 37. Automatische Seitenmigration

Eine Seite kann migriert werden, wenn:

- sie häufig von einem entfernten Node verwendet wird,
- der Ziel-Node ausreichend Speicher besitzt,
- sie nicht dauerhaft gepinnt ist,
- keine inkompatible DMA-Verwendung besteht,
- die Richtlinie Migration erlaubt.

Migrationsentscheidungen verwenden Hysterese, um Ping-Pong-Effekte zu vermeiden.

## 38. Seitenmigrationsablauf

```text
Kandidatenseite auswählen
  -> Referenzen und Pinning prüfen
  -> Zielseite auf gewünschtem Node allokieren
  -> alte Seite vor neuen Schreibzugriffen schützen
  -> Inhalt kopieren
  -> Seitentabellen atomar aktualisieren
  -> TLB-Shootdown durchführen
  -> Mapping- und Referenzdaten aktualisieren
  -> alte Seite freigeben
```

Bei Fehlern bleibt die ursprüngliche Seite gültig.

## 39. Nicht migrierbare Seiten

Nicht oder nur eingeschränkt migrierbar sind:

- aktive Kernel-Stacks,
- bestimmte Seitentabellen,
- gepinnte DMA-Seiten,
- MMIO-Bereiche,
- Firmware- und ACPI-NVS-Speicher,
- Crash-Dump-Reservierungen,
- architekturspezifische Trampoline,
- als fest gebunden markierte Seiten.

Diese Seiten können Speicher-Hotplug blockieren.

## 40. Page Pinning

Pinnbare Speicheroperationen müssen Node und Dauer berücksichtigen.

Ein Prozess darf nicht unbegrenzt Seiten auf einem Node pinnen.

Limits gelten pro:

- Prozess,
- Job,
- Treiber,
- Gerät,
- Node,
- System.

Lange Pinning-Zeiten werden diagnostisch sichtbar gemacht.

## 41. Device Locality

Treiber- und Gerätepuffer sollen nahe am zugehörigen Gerät allokiert werden.

Zu berücksichtigen sind:

- NUMA-Node des Geräts,
- Interruptziel-CPU,
- Hardwarequeue,
- Verarbeitungsthread,
- IOMMU-Domain,
- DMA-Adressgrenzen.

Bei unbekannter Gerätelokalität wird die normale Systemrichtlinie verwendet.

## 42. DMA und NUMA

DMA-Buffer werden bevorzugt auf dem Node des Geräts allokiert.

Wenn das nicht möglich ist, kann das DMA Framework:

- einen entfernten Node verwenden,
- IOMMU-Mappings anlegen,
- Bounce Buffer nutzen,
- die Anfrage ablehnen.

Treiber erhalten den tatsächlichen Node und dürfen lokale Platzierung nicht ungeprüft voraussetzen.

## 43. Interrupt-Lokalität

Interrupts sollen auf CPUs nahe am Gerät und seinen Datenqueues geroutet werden.

Der Interrupt Manager berücksichtigt:

- Device Node,
- Queue Node,
- Schedulerlast,
- CPU-Isolation,
- Realtime-Richtlinie.

Interrupt- und Threadplatzierung sollen gemeinsam optimiert werden.

## 44. Netzwerk und Storage

Multi-Queue-Geräte können Queues auf mehrere Nodes verteilen.

Beispiele:

- RX-/TX-Queues einer Netzwerkkarte,
- NVMe Submission und Completion Queues,
- Storage-Controller mit mehreren Hardwarequeues.

Paket- und I/O-Puffer werden bevorzugt auf dem Node der jeweiligen Queue erzeugt.

## 45. Speicherknappheit pro Node

Jeder Node besitzt eigene Wassermarken.

Bei lokaler Knappheit kann NovaOS:

1. lokale Caches bereinigen,
2. Seiten innerhalb des Nodes reclaimen,
3. auf nahe Nodes ausweichen,
4. Threads auf andere Nodes verlagern,
5. Node-gebundene Allokationen ablehnen,
6. systemweite OOM-Policy aktivieren.

Freier Remote-Speicher verhindert nicht zwingend das Scheitern einer `BIND`-Allokation.

## 46. Reclaim

Reclaim wird bevorzugt lokal ausgeführt.

Zu berücksichtigen sind:

- File Cache,
- anonyme Seiten,
- Dirty Pages,
- gepinnte Seiten,
- Reclaimkosten,
- Remote-Ausweichmöglichkeiten,
- Prozess- und Joblimits.

Ein Node darf nicht dauerhaft durch Remote-Allokationen vollständig geleert werden.

## 47. NUMA Balancing Limits

Automatisches Balancing besitzt Limits für:

- gescannte Seiten pro Intervall,
- Migrationen pro Sekunde,
- CPU-Zeit,
- Speicherbandbreite,
- maximale Distanz,
- minimale beobachtete Verbesserung.

Realtime- und latenzkritische Threads können von automatischem Balancing ausgenommen werden.

## 48. Große Seiten

Große Seiten müssen vollständig innerhalb eines NUMA-Nodes liegen.

Ihre Migration kann erfordern:

- gleich großen zusammenhängenden Zielbereich,
- Aufteilung in Basisseiten,
- TLB-Shootdown,
- Aktualisierung der NUMA-Statistik.

Eine große Seite darf nicht über mehrere Nodes hinweg als physisch zusammenhängend dargestellt werden.

## 49. Memory Hotplug

Beim Hinzufügen von Speicher:

1. Speicherbereich validieren,
2. Node bestimmen oder neu erzeugen,
3. PMM-Metadaten anlegen,
4. Seiten testen,
5. Zonen und Wassermarken aktualisieren,
6. Node online schalten,
7. Ereignis veröffentlichen.

Neue Seiten dürfen erst nach vollständiger Initialisierung allokiert werden.

## 50. Memory Removal

Vor dem Entfernen eines Node-Speicherbereichs müssen:

- neue Allokationen verhindert,
- bewegliche Seiten migriert,
- Caches geleert,
- Dirty Pages geschrieben,
- gepinnte und nicht migrierbare Seiten geprüft,
- Seitentabellen und TLBs aktualisiert werden.

Verbleiben nicht migrierbare Seiten, wird die Entfernung abgelehnt.

## 51. CPU-Hotplug

Beim CPU-Hotplug aktualisiert NUMA Support:

- CPU-Menge des Nodes,
- lokale Schedulerkapazität,
- bevorzugte Reclaim-CPU,
- Interruptaffinitäten,
- per-Node Worker,
- Distanz- und Fallbackentscheidungen.

Ein Node kann nach Entfernung seiner letzten CPU als memory-only Node bestehen bleiben.

## 52. Node Hotplug

Ein vollständiger Node kann hinzugefügt oder entfernt werden, wenn Plattform und Kernel dies unterstützen.

Ein Node darf erst entfernt werden, wenn:

- keine CPUs aktiv sind,
- kein verwalteter Speicher mehr benutzt wird,
- keine Geräte von ihm abhängen,
- keine aktiven Richtlinien ausschließlich auf ihn gebunden sind,
- alle Kernelreferenzen freigegeben wurden.

## 53. Fallback-Reihenfolge

Bei einer lokalen Allokation bestimmt der Kernel eine Fallback-Liste.

```text
lokaler Node
  -> nächster zulässiger Node
  -> weitere Nodes nach Distanz
  -> systemweite Notfallstrategie
```

Die Liste berücksichtigt:

- Policy,
- Entfernung,
- Zone,
- Speicherstatus,
- Nodezustand,
- Hardwareeinschränkungen.

## 54. Capability-Rechte

Mindestens folgende Rechte werden definiert:

| Capability | Bedeutung |
|---|---|
| `CAP_NUMA_QUERY` | allgemeine NUMA-Informationen lesen |
| `CAP_NUMA_TOPOLOGY` | detaillierte Topologie und Distanzen lesen |
| `CAP_NUMA_POLICY_SELF` | eigene NUMA-Richtlinie verändern |
| `CAP_NUMA_POLICY_OTHER` | Richtlinie fremder Prozesse verändern |
| `CAP_NUMA_BIND` | strikte Node-Bindung verwenden |
| `CAP_NUMA_MIGRATE` | Seitenmigration anfordern |
| `CAP_NUMA_HOTPLUG` | Nodes oder Speicher hinzufügen und entfernen |
| `CAP_NUMA_ADMIN` | globale NUMA-Richtlinien verwalten |

Rechte können zusätzlich durch Jobs und Sicherheitsprofile begrenzt werden.

## 55. NUMA-API

```c
np_status_t np_numa_query_topology(
    np_numa_topology_information_t* information
);

np_status_t np_numa_query_node(
    np_numa_node_id_t node_id,
    np_numa_node_information_t* information
);

np_status_t np_numa_set_process_policy(
    np_process_handle_t process,
    const np_numa_policy_t* policy
);

np_status_t np_numa_migrate_memory(
    np_process_handle_t process,
    np_virtual_address_t address,
    uint64_t length,
    const np_numa_node_set_t* target_nodes,
    uint32_t flags
);
```

Alle öffentlichen Strukturen enthalten Größe und ABI-Version.

## 56. Sicherheit

NUMA Support muss sicherstellen, dass:

- Prozesse nur eigene oder autorisierte Speicherbereiche migrieren,
- Node-Richtlinien keine Capability- oder Speicherlimits umgehen,
- physische Adressen nicht über normale Topologie-APIs offengelegt werden,
- Migrationen keine Daten zwischen Sicherheitsdomains offenlegen,
- TLB-Shootdowns vor Freigabe alter Seiten abgeschlossen werden,
- DMA- und gepinnte Seiten nicht unkontrolliert verschoben werden,
- entfernte Nodes keine aktiven Kernelreferenzen behalten,
- strikte Bindung nicht automatisch Zugriff auf fremde Speicherobjekte verleiht.

## 57. Datenschutz

Detaillierte NUMA-Topologie kann zur Hardwareidentifikation beitragen.

Daher gelten:

- Anwendungen erhalten standardmäßig nur notwendige Nodeinformationen,
- physische Adressbereiche werden nicht unprivilegiert ausgegeben,
- Geräte- und Distanzdetails können capability-geschützt sein,
- Diagnose bleibt standardmäßig lokal,
- externe Telemetrie ist standardmäßig deaktiviert.

## 58. Diagnose

NUMA-Diagnosen können enthalten:

- erkannte und aktive Nodes,
- CPUs und Geräte pro Node,
- Speicher pro Node und Zone,
- Entfernungsmatrix,
- lokale und Remote-Allokationen,
- Seitenmigrationen,
- Migrationsfehler,
- Node-Reclaim,
- gepinnte Seiten,
- Hotplug-Status,
- NUMA-Balancing-Aktivität.

Fremde Prozessdetails erfordern zusätzliche Diagnoseberechtigungen.

## 59. Statistik

```c
typedef struct np_numa_statistics {
    uint64_t total_pages;
    uint64_t free_pages;

    uint64_t local_allocations;
    uint64_t remote_allocations;
    uint64_t interleaved_allocations;

    uint64_t migrated_pages;
    uint64_t failed_migrations;
    uint64_t pinned_pages;

    uint64_t local_faults;
    uint64_t remote_faults;
    uint64_t reclaim_operations;
} np_numa_statistics_t;
```

Statistiken sollen pro Node, Prozess, Gerät und System verfügbar sein.

## 60. Fehlercodes

| Fehlercode | Bedeutung |
|---|---|
| `NP_OK` | Operation erfolgreich |
| `NP_ERR_INVALID_NODE` | Node ID ist ungültig |
| `NP_ERR_NODE_OFFLINE` | Node ist nicht online |
| `NP_ERR_NODE_UNAVAILABLE` | Node kann die Anfrage nicht erfüllen |
| `NP_ERR_POLICY_INVALID` | NUMA-Richtlinie ist ungültig |
| `NP_ERR_POLICY_EMPTY` | erforderliche Node-Menge ist leer |
| `NP_ERR_BINDING_FAILED` | strikte Bindung konnte nicht erfüllt werden |
| `NP_ERR_MIGRATION_FAILED` | Seite konnte nicht migriert werden |
| `NP_ERR_PAGE_PINNED` | Seite ist gepinnt |
| `NP_ERR_MEMORY_BUSY` | Speicherbereich wird noch verwendet |
| `NP_ERR_HOTPLUG_BLOCKED` | Node oder Speicher kann nicht entfernt werden |
| `NP_ERR_ACCESS_DENIED` | erforderliche Capability fehlt |
| `NP_ERR_NOT_SUPPORTED` | Funktion wird nicht unterstützt |

## 61. Fehlerbehandlung

Kann eine bevorzugte lokale Allokation nicht erfüllt werden, wird entsprechend der Policy auf andere Nodes ausgewichen.

Bei strikter Bindung wird stattdessen ein Fehler zurückgegeben.

Fehlerhafte Topologiedaten können zu einem konservativen UMA- oder vereinfachten NUMA-Modell führen.

Eine mögliche Beschädigung physischer Speicherzuordnungen oder Seitentabellen gilt als kritischer Kernelzustand.

## 62. Testanforderungen

Mindestens folgende Tests sind erforderlich:

1. UMA-System als einzelner Node,
2. Erkennung mehrerer NUMA-Nodes,
3. CPU-Zuordnung zu Nodes,
4. Speicherzuordnung zu Nodes,
5. Gerätezuordnung zu Nodes,
6. Entfernungsmatrix,
7. asymmetrische Distanzwerte,
8. memoryless Node,
9. CPU-less Memory Node,
10. lokale Standardallokation,
11. Preferred Policy,
12. Preferred-Fallback,
13. strikte Bind Policy,
14. Fehler bei erschöpfter Bind-Menge,
15. Interleave Policy,
16. Read-only Replicate Policy,
17. Prozessrichtlinie,
18. Threadrichtlinie,
19. Richtlinie einer virtuellen Region,
20. First-Touch Allocation,
21. Copy-on-Write-Lokalität,
22. Shared-Memory-Policy,
23. File-Cache-Platzierung,
24. Scheduler-Wakeup-Lokalität,
25. Threadmigration zwischen Nodes,
26. automatische Seitenmigration,
27. Verhinderung von Migration-Ping-Pong,
28. gepinnte Seite,
29. große Seite auf einem Node,
30. gerätenahe DMA-Allokation,
31. Multi-Queue-Netzwerkgerät,
32. lokaler Node-Reclaim,
33. Remote-Fallback bei Speicherknappheit,
34. Speicher-Hotplug,
35. Ablehnung nicht migrierbarer Speicherentfernung,
36. CPU-Hotplug innerhalb eines Nodes,
37. vollständiges Node Hotplug, sofern unterstützt,
38. Capability-Prüfung,
39. parallele NUMA-Allokationen,
40. korrekte Diagnose und Statistik.

## 63. Verbindliche Invarianten

1. Jedes verwaltete physische Speichersegment gehört genau einem primären NUMA-Node.
2. Jede aktive CPU gehört genau einem primären NUMA-Node.
3. UMA-Systeme werden als gültiges Ein-Node-NUMA-System dargestellt.
4. Buddy-Blöcke werden niemals über Nodegrenzen zusammengeführt.
5. `BIND`-Allokationen verwenden ausschließlich die angegebene Node-Menge.
6. Eine migrierte Seite wird erst nach abgeschlossenem TLB-Shootdown am alten Ort freigegeben.
7. Gepinnte DMA-Seiten werden nicht unkontrolliert migriert.
8. Große physische Seiten überschreiten keine Nodegrenze.
9. Ein Node wird erst nach vollständiger Initialisierung für Allokationen freigegeben.
10. Entfernte Nodes behalten keine aktiven CPU-, Speicher- oder Gerätereferenzen.
11. NUMA-Richtlinien erweitern keine Capability- oder Zugriffsrechte.
12. NUMA-Zustandsänderungen erfolgen unter geeigneter Synchronisation.

## 64. Referenzablauf eines NUMA Page Faults

```text
Thread verursacht Page Fault
  -> virtuelle Speicherregion bestimmen
  -> regionsspezifische NUMA-Policy prüfen
  -> alternativ Thread-, Prozess- und Jobpolicy prüfen
  -> aktuelle CPU und ihren Node bestimmen
  -> zulässige Node-Menge aufbauen
  -> bevorzugten Node auswählen
  -> physische Seite auf diesem Node anfordern
  -> bei erlaubtem Fallback nächsten Node nach Distanz wählen
  -> Seite nullen oder Inhalt laden
  -> virtuelle Abbildung erstellen
  -> NUMA- und VMM-Statistiken aktualisieren
  -> Threadausführung fortsetzen
```

## 65. Zusammenfassung

NovaOS behandelt UMA als Ein-Node-Sonderfall und stellt dadurch eine einheitliche NUMA-Architektur für alle Systeme bereit.

CPU-, Speicher- und Gerätelokalität werden in einem gemeinsamen Topologiemodell erfasst. Local, Preferred, Bind, Interleave und Replicate Policies ermöglichen sowohl automatische als auch explizit kontrollierte Platzierung.

Scheduler, PMM, VMM, Device Manager und DMA Framework arbeiten zusammen, um Threads, Seiten und I/O-Puffer möglichst lokal zu halten. Hysterese, Migrationslimits und klare Hotplug-Regeln verhindern, dass NUMA-Optimierungen unnötige Migrationen oder inkonsistente Speicherzustände erzeugen.