# NPSPEC-KERNEL-0026 – CPU Manager

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-KERNEL-0026 |
| Titel | CPU Manager |
| Status | Spezifikation |
| Version | 1.0 |
| Datum | 2026-08-03 |
| Bereich | Kernel / Prozessorverwaltung |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | NPSPEC-KERNEL-0001, NPSPEC-KERNEL-0002, NPSPEC-KERNEL-0003, NPSPEC-KERNEL-0009, NPSPEC-KERNEL-0014, NPSPEC-KERNEL-0016, NPSPEC-KERNEL-0020, NPSPEC-KERNEL-0021 |
| Zugehörige ADRs | ADR-PROC-0006, ADR-PROC-0007, ADR-PROC-0008, ADR-POWER-0001, ADR-POWER-0005, ADR-SEC-0005, ADR-SEC-0006 |

---

## 1. Zweck

Diese Spezifikation definiert den CPU Manager des NovaOS-Kernels.

Der CPU Manager verwaltet:

- Prozessorerkennung,
- CPU-Topologie,
- Boot Processor und Application Processors,
- CPU-Funktionen,
- per-CPU-Daten,
- SMP-Initialisierung,
- CPU Hotplug,
- Microcode-Updates,
- CPU-Zustände,
- Fehlerisolation,
- Zusammenarbeit mit Scheduler, Interrupt- und Power Manager.

## 2. Ziele

Der CPU Manager muss:

- Einzel- und Mehrprozessorsysteme unterstützen,
- architekturspezifische Details über die HAL abstrahieren,
- CPU-Fähigkeiten zuverlässig erkennen,
- heterogene Prozessoren darstellen,
- zusätzliche CPUs kontrolliert starten und stoppen,
- CPU-lokale Ressourcen verwalten,
- Sicherheitsfunktionen pro CPU aktivieren,
- defekte oder inkompatible CPUs isolieren,
- alte Hardware mit ehrlichem Funktionsstatus unterstützen.

## 3. Abgrenzung

| Komponente | Verantwortung |
|---|---|
| CPU Manager | CPU-Erkennung, Zustand und Topologie |
| Scheduler | Verteilung ausführbarer Threads |
| Interrupt Manager | Local Interrupt Controller und IPIs |
| Timer Subsystem | CPU-lokale Timer und Zeitquellen |
| Power Manager | Idle-, Performance- und Power-Zustände |
| Security Manager | CPU-Schutzfunktionen und Sicherheitsstatus |
| HAL | architekturspezifische Register und Startmechanismen |
| Physical Memory Manager | NUMA-Speicherknoten |
| Thermal Manager | Temperatur- und Kapazitätsbegrenzung |

## 4. CPU-Begriffe

| Begriff | Bedeutung |
|---|---|
| BSP | Boot Processor, der den Kernel startet |
| AP | zusätzlicher Application Processor |
| Logical CPU | durch den Kernel planbare CPU-Einheit |
| Physical Core | physischer Prozessorkern |
| Package | physisches CPU-Gehäuse oder Socket |
| SMT Thread | logischer Hardwarethread eines Kerns |
| NUMA Node | CPU- und Speicherdomäne |
| CPU Capacity | relative Rechenleistung einer CPU |

## 5. CPU-Kennung

```c
typedef uint32_t np_cpu_id_t;
```

Jede logische CPU erhält eine kernelinterne CPU ID.

Die CPU ID:

- ist innerhalb eines Bootvorgangs eindeutig,
- ist nicht zwingend identisch mit APIC ID oder Hardware ID,
- wird als Index für CPU-lokale Daten verwendet,
- verleiht keine administrativen Rechte.

## 6. Hardwarekennung

```c
typedef uint64_t np_cpu_hardware_id_t;
```

Die Hardwarekennung kann abhängig von der Architektur sein:

- APIC ID,
- x2APIC ID,
- MPIDR-Wert,
- Hart ID,
- Firmware-Prozessor-ID.

Die Zuordnung zwischen CPU ID und Hardwarekennung wird durch den CPU Manager verwaltet.

## 7. CPU-Objekt

Jede erkannte CPU wird als Kernelobjekt dargestellt.

```c
typedef struct np_cpu {
    np_kernel_object_t object;

    np_cpu_id_t cpu_id;
    np_cpu_hardware_id_t hardware_id;

    np_cpu_state_t state;
    np_cpu_topology_t topology;
    np_cpu_features_t features;

    np_cpu_capacity_t capacity;
    np_cpu_local_data_t* local_data;

    np_thread_t* idle_thread;
    np_interrupt_controller_t* local_interrupt_controller;
    np_clockevent_t* local_timer;

    uint32_t flags;
    np_spinlock_t lock;
} np_cpu_t;
```

## 8. CPU-Zustände

```c
typedef enum np_cpu_state {
    NP_CPU_DISCOVERED,
    NP_CPU_OFFLINE,
    NP_CPU_STARTING,
    NP_CPU_ONLINE,
    NP_CPU_ACTIVE,
    NP_CPU_IDLE,
    NP_CPU_QUIESCING,
    NP_CPU_STOPPING,
    NP_CPU_FAILED,
    NP_CPU_REMOVED
} np_cpu_state_t;
```

Nur CPUs im Zustand `ONLINE`, `ACTIVE` oder `IDLE` dürfen regulär Schedulerarbeit ausführen.

## 9. Zustandsübergänge

```text
DISCOVERED
  -> OFFLINE
  -> STARTING
  -> ONLINE
  -> ACTIVE
  -> IDLE
  -> ACTIVE
  -> QUIESCING
  -> STOPPING
  -> OFFLINE
```

Ein Initialisierungs- oder Hardwarefehler kann nach `FAILED` führen.

Eine als `REMOVED` markierte CPU kann nicht erneut online geschaltet werden.

## 10. Boot Processor

Der Boot Processor übernimmt während der frühen Bootphase:

- Kernel-Einstieg,
- frühe Architekturinitialisierung,
- Speicherverwaltung,
- Interruptbasis,
- Erkennung weiterer CPUs,
- Vorbereitung der AP-Startsequenz,
- initiale Scheduler- und Timerkonfiguration.

Der BSP bleibt nicht automatisch dauerhaft bevorzugt. Nach SMP-Initialisierung wird er wie eine reguläre aktive CPU eingeplant.

## 11. CPU-Erkennung

CPU-Informationen können stammen aus:

- CPUID oder architekturspezifischen Registern,
- ACPI,
- Device Tree,
- Firmwaretabellen,
- Hypervisorinformationen,
- Plattformkonfiguration.

Alle Quellen werden abgeglichen.

Widersprüchliche Einträge werden diagnostiziert und restriktiv behandelt.

## 12. CPU-Topologie

```c
typedef struct np_cpu_topology {
    uint32_t package_id;
    uint32_t die_id;
    uint32_t cluster_id;
    uint32_t core_id;
    uint32_t thread_id;
    uint32_t numa_node_id;

    uint32_t llc_id;
    uint32_t capacity_class;
} np_cpu_topology_t;
```

Unbekannte Topologiefelder erhalten definierte `UNKNOWN`-Werte.

## 13. Topologiegraph

Der CPU Manager bildet Beziehungen ab zwischen:

- Packages,
- Dies,
- Clustern,
- Cores,
- SMT Threads,
- Caches,
- NUMA Nodes,
- Power Domains,
- Thermal Zones.

Scheduler und Power Manager können diese Informationen für Platzierungs- und Energieentscheidungen verwenden.

## 14. Cache-Topologie

```c
typedef struct np_cpu_cache_info {
    uint32_t level;
    np_cache_type_t type;

    uint64_t size;
    uint32_t line_size;
    uint32_t associativity;

    np_cpu_set_t shared_cpu_set;
} np_cpu_cache_info_t;
```

Gemeinsam genutzte Caches werden als gemeinsame Topologieobjekte dargestellt und nicht pro CPU unabhängig dupliziert.

## 15. CPU-Funktionen

CPU-Funktionen werden in einer architekturunabhängigen Featurestruktur dargestellt.

```c
typedef enum np_cpu_feature {
    NP_CPU_FEATURE_FPU,
    NP_CPU_FEATURE_SIMD,
    NP_CPU_FEATURE_NX,
    NP_CPU_FEATURE_SMEP,
    NP_CPU_FEATURE_SMAP,
    NP_CPU_FEATURE_ASID,
    NP_CPU_FEATURE_INVARIANT_COUNTER,
    NP_CPU_FEATURE_LOCAL_APIC,
    NP_CPU_FEATURE_IOMMU_SUPPORT,
    NP_CPU_FEATURE_VIRTUALIZATION,
    NP_CPU_FEATURE_HARDWARE_RANDOM,
    NP_CPU_FEATURE_CONTROL_FLOW_PROTECTION
} np_cpu_feature_t;
```

Architekturspezifische Features können zusätzlich abgefragt werden.

## 16. Systemweite Featuremenge

Die systemweit sicher nutzbare Featuremenge ist grundsätzlich die Schnittmenge der Features aller aktiven CPUs:

```text
system_features =
    cpu0_features
    ∩ cpu1_features
    ∩ ...
    ∩ cpuN_features
```

Features können CPU-lokal verwendet werden, wenn Kernel, Scheduler und Context Switching dies ausdrücklich unterstützen.

## 17. Mindestanforderungen

Jede Zielarchitektur definiert zwingende CPU-Mindestfunktionen.

Fehlt ein erforderliches Feature, kann NovaOS:

- die CPU nicht online schalten,
- eine kompatible Kernelvariante verwenden,
- im klar gekennzeichneten Degraded Mode starten,
- den Bootvorgang kontrolliert abbrechen.

Optionale Features dürfen nicht als zwingend vorausgesetzt werden.

## 18. Heterogene CPUs

NovaOS unterstützt CPUs mit unterschiedlichen:

- Leistungsfähigkeiten,
- Energieeigenschaften,
- Befehlssatzerweiterungen,
- Cachegrößen,
- Frequenzbereichen,
- Thermalgrenzen.

Der Scheduler darf einen Thread nur auf eine CPU verschieben, die alle für den Thread aktiv verwendeten CPU-Funktionen unterstützt.

## 19. CPU-Capacity

```c
typedef uint32_t np_cpu_capacity_t;

#define NP_CPU_CAPACITY_SCALE 1024
```

Eine Referenz-CPU kann eine Kapazität von `1024` besitzen.

Kleinere oder größere Werte beschreiben relative Rechenleistung.

Die Kapazität kann durch Frequenz, Thermal Throttling oder Power Policy dynamisch reduziert werden.

## 20. CPU-Sets

```c
typedef struct np_cpu_set {
    uint64_t bits[NP_CPU_SET_WORDS];
} np_cpu_set_t;
```

Der CPU Manager stellt mindestens folgende Sets bereit:

- mögliche CPUs,
- erkannte CPUs,
- online CPUs,
- aktive CPUs,
- isolierte CPUs,
- fehlgeschlagene CPUs,
- CPUs eines NUMA-Nodes,
- CPUs einer Kapazitätsklasse.

CPU-Sets müssen für zukünftige Systeme mit mehr als 64 logischen CPUs erweiterbar sein.

## 21. Per-CPU-Daten

Jede online geschaltete CPU besitzt einen eigenen per-CPU-Datenbereich.

```c
typedef struct np_cpu_local_data {
    np_cpu_t* cpu;
    np_thread_t* current_thread;

    np_runqueue_t* runqueue;
    np_log_ring_buffer_t* log_buffer;

    uint32_t preemption_count;
    uint32_t interrupt_depth;
    uint32_t exception_depth;

    uintptr_t kernel_stack_top;
    uint64_t flags;
} np_cpu_local_data_t;
```

Zugriff erfolgt über architekturspezifische CPU-lokale Register oder sichere Indirektion.

## 22. Per-CPU-Initialisierung

Vor dem Zustand `ONLINE` müssen mindestens initialisiert sein:

- CPU-lokaler Datenbereich,
- Kernel- und Notfall-Stacks,
- Descriptor- und Exceptiontabellen,
- lokaler Interrupt-Controller,
- lokaler Timer,
- Scheduler-Runqueue,
- Idle-Thread,
- Stack Canary,
- Sicherheitsregister,
- Logging-Puffer.

Eine teilweise initialisierte CPU darf nicht für reguläre Threads freigegeben werden.

## 23. AP-Startcode

Der Application-Processor-Start erfolgt über einen kleinen architekturspezifischen Trampoline-Code.

Dieser muss:

- an einer zulässigen physischen Adresse liegen,
- vor parallelem Zugriff geschützt sein,
- die richtige CPU identifizieren,
- einen temporären Stack verwenden,
- Seitentabellen und Kerneladressraum aktivieren,
- in den generischen CPU-Startpfad wechseln.

Nach Abschluss darf der temporäre Trampoline-Bereich freigegeben werden, sofern er nicht für Hotplug benötigt wird.

## 24. AP-Startablauf

```text
BSP wählt offline CPU
  -> CPU-lokale Ressourcen anlegen
  -> Startparameter vorbereiten
  -> Trampoline bereitstellen
  -> architekturspezifisches Startsignal senden
  -> AP aktiviert minimalen CPU-Modus
  -> AP aktiviert Kerneladressraum
  -> AP initialisiert lokale Tabellen und Register
  -> AP initialisiert Interrupts, Timer und Security
  -> AP meldet ONLINE
  -> BSP gibt CPU für den Scheduler frei
```

Jede Phase besitzt einen Timeout.

## 25. CPU-Startbarrieren

SMP-Initialisierung verwendet definierte Startbarrieren.

Eine CPU darf den Zustand `ACTIVE` erst erreichen, wenn:

- globale Kernelinitialisierung ausreichend fortgeschritten ist,
- gemeinsame Datenstrukturen sichtbar sind,
- alle erforderlichen Speicherbarrieren erfolgt sind,
- Scheduler und Interrupt Manager bereit sind.

Spin-Wait-Schleifen benötigen Timeouts oder einen kontrollierten Fehlerpfad.

## 26. CPU-lokale Stacks

Jede CPU erhält mindestens:

- frühen Startstack,
- Idle-Thread-Kernelstack,
- Interrupt- oder Exception-Notfallstack,
- optional NMI- und Double-Fault-Stack.

Stacks besitzen Guard Pages, sobald der VMM verfügbar ist.

Temporäre Startstacks werden nach erfolgreicher Initialisierung nicht weiterverwendet.

## 27. Local Interrupt Controller

Vor dem Online-Schalten wird der lokale Interrupt-Controller initialisiert.

Dazu gehören:

- eindeutige Hardwarekennung,
- Spurious- und Error-Vektor,
- IPI-Unterstützung,
- Timervektor,
- Prioritätszustand,
- Maskierung unvorbereiteter Quellen.

Ein fehlerhafter lokaler Controller verhindert normalerweise das Online-Schalten der CPU.

## 28. CPU-lokaler Timer

Jede Scheduler-CPU benötigt ein geeignetes Clockevent-Gerät.

Falls kein CPU-lokaler Timer vorhanden ist, kann eine zentrale Zeitquelle verwendet werden, sofern:

- SMP-Routing korrekt funktioniert,
- Scheduler-Deadlines eingehalten werden,
- Hotplug unterstützt wird,
- keine unvertretbare Latenz entsteht.

Der Timer wird vor Freigabe der Runqueue getestet.

## 29. Scheduler-Integration

Der CPU Manager informiert den Scheduler über:

- online und aktive CPUs,
- Kapazität,
- Topologie,
- NUMA-Zugehörigkeit,
- SMT-Beziehungen,
- CPU-Features,
- Thermal- und Powerbegrenzungen.

Der Scheduler meldet zurück:

- laufende Threads,
- nicht migrierbare Threads,
- Affinitätskonflikte,
- Auslastung,
- Voraussetzungen für CPU-Offlining.

## 30. CPU-Affinität

Thread- und Interrupt-Affinitäten verwenden CPU-Sets des CPU Managers.

Eine Affinität darf nur CPUs enthalten, die:

- möglich,
- nicht entfernt,
- für den jeweiligen Kontext kompatibel

sind.

Offline CPUs können in einer gespeicherten Wunschaffinität verbleiben, dürfen aber nicht als aktuelles Ausführungsziel verwendet werden.

## 31. SMT

SMT Threads teilen Teile eines physischen Kerns.

Scheduler und Security Manager können SMT berücksichtigen für:

- Lastverteilung,
- Cachekonflikte,
- Energieeffizienz,
- Seitenkanalrisiken,
- Isolierung vertraulicher Arbeitslasten.

Ein gehärtetes Sicherheitsprofil kann SMT begrenzen oder deaktivieren.

## 32. CPU-Isolation

CPUs können für bestimmte Zwecke isoliert werden:

- Realtime-Workloads,
- sicherheitskritische Domains,
- Diagnose,
- dedizierte Gerätequeues,
- Hypervisor- oder Kompatibilitätsaufgaben.

Isolierte CPUs werden nicht regulär für allgemeines Load Balancing verwendet.

Eine Isolation benötigt administrative Rechte und eine gültige Schedulerkonfiguration.

## 33. Inter-Processor Interrupts

Der CPU Manager arbeitet mit dem Interrupt Manager für IPIs zusammen.

Mindestens unterstützt werden:

```c
typedef enum np_cpu_ipi_type {
    NP_CPU_IPI_RESCHEDULE,
    NP_CPU_IPI_TLB_SHOOTDOWN,
    NP_CPU_IPI_CALL_FUNCTION,
    NP_CPU_IPI_STOP,
    NP_CPU_IPI_WAKE,
    NP_CPU_IPI_DEBUG
} np_cpu_ipi_type_t;
```

Userspace darf keine IPIs direkt auslösen.

## 34. Cross-CPU Calls

```c
np_status_t np_cpu_call(
    const np_cpu_set_t* target_cpus,
    np_cpu_call_function_t function,
    void* context,
    uint32_t flags,
    np_time_ns_t deadline
);
```

Callbacks müssen für den vorgesehenen IPI- oder Threadkontext geeignet sein.

Synchrone Cross-CPU Calls benötigen Timeouts und dürfen keine zyklischen Warteabhängigkeiten erzeugen.

## 35. CPU Hotplug

CPU Hotplug unterstützt das logische Online- und Offline-Schalten von CPUs.

```c
np_status_t np_cpu_online(
    np_cpu_id_t cpu_id
);

np_status_t np_cpu_offline(
    np_cpu_id_t cpu_id,
    uint32_t flags
);
```

Hotplug benötigt eine administrative Capability.

## 36. CPU-Offlining

Vor dem Offline-Schalten müssen:

1. neue Threadzuweisungen verhindert,
2. reguläre Threads migriert,
3. gebundene Threads behandelt,
4. Interrupts umgeroutet,
5. Timer migriert,
6. per-CPU Work Queues geleert,
7. RCU- und TLB-Aktivitäten abgeschlossen,
8. CPU-lokale Caches zurückgegeben,
9. Idle-Thread in Stop-Pfad versetzt werden.

Die letzte aktive CPU darf nicht offline geschaltet werden.

## 37. Nicht migrierbare Zustände

CPU-Offlining wird abgelehnt, wenn die CPU noch besitzt:

- nicht migrierbaren Realtime-Thread,
- fest gebundenen kritischen Interrupt,
- laufenden Panic- oder Recovery-Kontext,
- nicht beendbare CPU-lokale Hardwarefunktion,
- kritischen Sperr- oder RCU-Zustand,
- unverschiebbare Timer- oder Treiberarbeit.

Der genaue Blockiergrund muss diagnostizierbar sein.

## 38. CPU-Onlining

Beim erneuten Online-Schalten werden alle CPU-lokalen Zustände neu validiert.

Insbesondere werden geprüft:

- Microcodeversion,
- Featuremenge,
- Security Register,
- Interrupt-Controller,
- Timer,
- per-CPU-Speicher,
- Topologie,
- Power- und Thermalstatus.

Eine zuvor gültige CPU kann nach einem Plattform- oder Firmwareereignis abgelehnt werden.

## 39. Physischer CPU-Hotplug

Unterstützt die Plattform physisches CPU-Hotplug, kann eine CPU vollständig hinzugefügt oder entfernt werden.

Vor physischer Entfernung müssen:

- CPU offline sein,
- alle Referenzen auf ihre lokalen Daten entfernt,
- Topologiegraph aktualisiert,
- NUMA- und Power-Domains angepasst,
- Firmwarefreigabe bestätigt sein.

Das Entfernen des BSP ist nur möglich, wenn Architektur und Plattform einen sicheren Wechsel der Bootzuständigkeit unterstützen.

## 40. Microcode

Der CPU Manager kann Microcode-Updates über eine architekturspezifische Schnittstelle anwenden.

```c
np_status_t np_cpu_microcode_update(
    const np_microcode_package_t* package,
    np_microcode_result_t* result
);
```

Microcodepakete müssen signiert oder durch die CPU-Hardware authentifiziert sein.

## 41. Microcode-Update-Ablauf

```text
Microcodepaket laden
  -> Hersteller und CPU-Signatur prüfen
  -> Paketintegrität prüfen
  -> betroffene CPUs bestimmen
  -> CPUs in sicheren Updatezustand bringen
  -> Update pro CPU anwenden
  -> Revision und Featurestatus neu lesen
  -> Sicherheits- und Schedulerstatus aktualisieren
  -> Ergebnis auditieren
```

Ein Update darf nicht als erfolgreich gelten, wenn nur ein Teil der erforderlichen CPUs aktualisiert wurde und kein kompatibler Mischbetrieb möglich ist.

## 42. Microcode-Fehler

Bei einem fehlgeschlagenen Update kann NovaOS:

- alte Revision weiterverwenden,
- betroffene CPU offline schalten,
- System-Sicherheitsstufe reduzieren,
- Boot oder Update abbrechen,
- Recovery aktivieren.

Microcode-Downgrades sind standardmäßig nicht erlaubt.

## 43. CPU-Sicherheitsfunktionen

Der CPU Manager aktiviert pro CPU verfügbare Schutzfunktionen, darunter abhängig von der Architektur:

- NX,
- SMEP,
- SMAP,
- Supervisor Write Protection,
- Control-Flow-Schutz,
- sichere System-Call-Funktionen,
- Kernel-Userspace-Isolation,
- relevante Speculation Mitigations.

Alle aktiven CPUs müssen mit der systemweiten Sicherheitsrichtlinie kompatibel sein.

## 44. Speculation Mitigations

Der CPU Manager erfasst bekannte CPU-Verwundbarkeitsklassen und verfügbare Mitigations.

Die Policy kann abhängig vom Profil festlegen:

- immer aktiv,
- automatisch,
- nur für Domainwechsel,
- durch sichere Hardwarefunktion ersetzt,
- nicht verfügbar.

Performancekosten und fehlende Absicherungen werden diagnostisch ausgewiesen.

## 45. CPU-Zufallsquelle

Hardware-Zufallsinstruktionen können als Entropiequelle verwendet werden, jedoch nicht blind als einzige Vertrauensquelle.

Der Security Manager kombiniert sie mit weiteren Entropiequellen.

Erkannte Hardwarefehler oder wiederholte Werte führen zur Deaktivierung der Quelle.

## 46. Virtualisierung

Der CPU Manager erkennt, ob NovaOS:

- auf realer Hardware,
- unter einem unterstützten Hypervisor,
- in einer eingeschränkten virtuellen Umgebung

ausgeführt wird.

Paravirtualisierte CPU-, Timer- und IPI-Funktionen können verwendet werden, wenn deren Schnittstellen validiert und unterstützt sind.

Hypervisorangaben gelten nicht automatisch als vertrauenswürdig.

## 47. NUMA-Integration

CPUs werden NUMA-Nodes zugeordnet.

Der CPU Manager stellt bereit:

- CPU-Menge pro Node,
- Entfernungsmatrix,
- lokale Speicherzuordnung,
- Node-Kapazität,
- Online- und Hotplugstatus.

Scheduler und Speicherverwaltung nutzen diese Daten für lokale Platzierung.

## 48. Power-Integration

Der Power Manager erhält:

- unterstützte Idle-Zustände,
- Performance-Zustände,
- Kapazitätsklassen,
- Package- und Core-Beziehungen,
- aktive CPU-Menge,
- Thermal- und Frequenzlimits.

Der CPU Manager führt architekturspezifische Zustandswechsel nur nach einer gültigen Power-Entscheidung aus.

## 49. Thermal-Integration

Thermalereignisse können:

- CPU-Kapazität reduzieren,
- Frequenzen begrenzen,
- SMT Threads deaktivieren,
- einzelne Cores offline schalten,
- Package-Abschaltung anfordern.

Kritische Hardwaregrenzen haben Vorrang vor Scheduler- und Benutzerprofilen.

## 50. CPU-Fehler

CPU-Fehler können erkannt werden durch:

- Machine Check,
- Watchdog,
- fehlgeschlagene IPI-Antwort,
- Timerstillstand,
- ungültigen Microcodezustand,
- wiederholte Hardwarefehler,
- Firmwaremeldung.

Der CPU Manager klassifiziert Fehler als korrigierbar, isolierbar oder systemkritisch.

## 51. CPU-Isolation bei Fehlern

Kann ein Fehler lokal begrenzt werden, erfolgt:

1. CPU für neue Arbeit sperren,
2. Threads soweit sicher migrieren,
3. Interrupts umleiten,
4. Timer und Work Queues behandeln,
5. CPU anhalten,
6. Zustand auf `FAILED` setzen,
7. Diagnose- und Security-Ereignis erzeugen.

Ist der Kernelzustand möglicherweise beschädigt, wird die Kernel-Panic-Policy angewendet.

## 52. Watchdog

Ein CPU-Watchdog kann erkennen:

- Schedulerstillstand,
- nicht reagierende CPU,
- dauerhaft deaktivierte Interrupts,
- überlange kritische Abschnitte,
- IPI-Timeouts,
- Timerstillstand.

Ein Watchdog-Ereignis kann Stack- und CPU-Kontexte erfassen, bevor eine Isolation oder Panic erfolgt.

## 53. CPU Manager API

```c
np_status_t np_cpu_query(
    np_cpu_id_t cpu_id,
    np_cpu_information_t* information
);

np_status_t np_cpu_enumerate(
    np_cpu_information_t* cpus,
    size_t* cpu_count
);

np_status_t np_cpu_online(
    np_cpu_id_t cpu_id
);

np_status_t np_cpu_offline(
    np_cpu_id_t cpu_id,
    uint32_t flags
);
```

Öffentliche Strukturen enthalten Größen- und Versionsfelder.

## 54. Capability-Rechte

Mindestens folgende Rechte werden definiert:

| Capability | Bedeutung |
|---|---|
| `CAP_CPU_QUERY` | allgemeine CPU-Informationen lesen |
| `CAP_CPU_TOPOLOGY` | detaillierte Topologiedaten lesen |
| `CAP_CPU_AFFINITY` | zulässige CPU-Zuordnungen verwalten |
| `CAP_CPU_HOTPLUG` | CPUs online oder offline schalten |
| `CAP_CPU_ISOLATE` | CPU für besondere Aufgaben isolieren |
| `CAP_CPU_MICROCODE` | Microcode-Update ausführen |
| `CAP_CPU_DEBUG` | geschützte CPU-Zustände lesen |
| `CAP_CPU_POLICY` | CPU-Feature- und Mitigation-Policy verändern |
| `CAP_CPU_ADMIN` | globale CPU-Verwaltung |

Microcode-, Debug- und Policyoperationen werden auditiert.

## 55. Sicherheit

Der CPU Manager muss sicherstellen, dass:

- nicht initialisierte CPUs keine Kernelthreads ausführen,
- alle aktiven CPUs die notwendigen Sicherheitsfunktionen aktivieren,
- CPU-Funktionsunterschiede beim Threadwechsel berücksichtigt werden,
- Microcodepakete vor Anwendung validiert werden,
- Userspace keine privilegierten CPU-Register verändern kann,
- IPIs nur über autorisierte Kernelpfade ausgelöst werden,
- vollständige Kerneladressen und Registerzustände geschützt bleiben,
- fehlerhafte CPUs keine neue Arbeit erhalten,
- Degraded Security bei fehlenden Schutzfunktionen sichtbar bleibt.

## 56. Datenschutz

CPU-Topologie und genaue Hardwarekennungen können zur Gerätefingerprinting beitragen.

Daher gelten:

- normale Anwendungen erhalten nur erforderliche allgemeine Angaben,
- Hardware-IDs und Microcodeinformationen können eingeschränkt werden,
- detaillierte Topologie benötigt eine passende Capability,
- Diagnose bleibt standardmäßig lokal,
- externe Telemetrie ist standardmäßig deaktiviert.

## 57. Diagnose

Der CPU Manager stellt autorisierten Diagnosediensten bereit:

- CPU ID und Hardware ID,
- Hersteller, Familie und Modell,
- Microcoderevision,
- Zustand,
- Topologie,
- Featuremenge,
- Kapazität,
- NUMA-Node,
- Idle- und Performancezustand,
- Interrupt- und Timerstatus,
- Fehler- und Watchdogereignisse,
- fehlende Sicherheitsfunktionen.

## 58. Statistik

```c
typedef struct np_cpu_manager_statistics {
    uint64_t discovered_cpus;
    uint64_t online_cpus;
    uint64_t failed_cpus;

    uint64_t startup_attempts;
    uint64_t startup_failures;
    uint64_t offline_operations;
    uint64_t online_operations;

    uint64_t ipi_sent;
    uint64_t ipi_timeouts;

    uint64_t microcode_updates;
    uint64_t microcode_failures;
    uint64_t watchdog_events;
} np_cpu_manager_statistics_t;
```

Statistiken sollen pro CPU und systemweit verfügbar sein.

## 59. Fehlercodes

| Fehlercode | Bedeutung |
|---|---|
| `NP_OK` | Operation erfolgreich |
| `NP_ERR_INVALID_CPU` | CPU ID ist ungültig |
| `NP_ERR_CPU_OFFLINE` | CPU ist nicht online |
| `NP_ERR_CPU_ONLINE` | CPU ist bereits online |
| `NP_ERR_CPU_FAILED` | CPU wurde als fehlerhaft markiert |
| `NP_ERR_CPU_INCOMPATIBLE` | CPU ist nicht mit dem aktiven System kompatibel |
| `NP_ERR_CPU_BUSY` | CPU kann derzeit nicht offline geschaltet werden |
| `NP_ERR_CPU_LAST_ACTIVE` | letzte aktive CPU darf nicht entfernt werden |
| `NP_ERR_CPU_START_TIMEOUT` | CPU-Start lief in einen Timeout |
| `NP_ERR_IPI_TIMEOUT` | CPU antwortete nicht auf IPI |
| `NP_ERR_MICROCODE_INVALID` | Microcodepaket ist ungültig |
| `NP_ERR_MICROCODE_REJECTED` | CPU lehnte das Update ab |
| `NP_ERR_ACCESS_DENIED` | erforderliche Capability fehlt |
| `NP_ERR_NOT_SUPPORTED` | Funktion wird nicht unterstützt |

## 60. Fehlerbehandlung

Schlägt der Start einer zusätzlichen CPU fehl, kann NovaOS mit weniger CPUs weiterarbeiten.

Dabei werden:

- CPU als `FAILED` markiert,
- Ressourcen zurückgerollt,
- Scheduler- und Topologiedaten korrigiert,
- Sicherheits- und Diagnoseereignisse erzeugt.

Der Ausfall des BSP während der frühen Bootphase ist normalerweise fatal.

Ein später isolierbarer CPU-Ausfall muss nicht automatisch den gesamten Kernel stoppen.

## 61. Testanforderungen

Mindestens folgende Tests sind erforderlich:

1. Start mit einer CPU,
2. Erkennung mehrerer CPUs,
3. Zuordnung von Hardware ID und CPU ID,
4. Aufbau der Package- und Core-Topologie,
5. Erkennung von SMT Threads,
6. Cache-Topologie,
7. NUMA-Zuordnung,
8. CPU-Feature-Erkennung,
9. systemweite Feature-Schnittmenge,
10. heterogene Kapazitätsklassen,
11. per-CPU-Dateninitialisierung,
12. AP-Trampoline,
13. erfolgreicher AP-Start,
14. AP-Start-Timeout,
15. Initialisierung des lokalen Interrupt-Controllers,
16. Initialisierung des lokalen Timers,
17. Idle-Thread pro CPU,
18. Reschedule-IPI,
19. TLB-Shootdown-IPI,
20. synchroner Cross-CPU Call,
21. IPI-Timeout,
22. CPU-Offlining,
23. Migration von Threads und Interrupts,
24. Ablehnung beim letzten aktiven Prozessor,
25. Ablehnung bei nicht migrierbarem Thread,
26. erneutes CPU-Onlining,
27. physisches CPU-Hotplug, sofern unterstützt,
28. gültiges Microcode-Update,
29. ungültiges Microcodepaket,
30. Teilfehler eines Microcode-Updates,
31. Aktivierung von NX und SMEP/SMAP,
32. Speculation-Mitigation-Policy,
33. SMT-Einschränkung im Hardened Profile,
34. CPU-Isolation für Realtime,
35. CPU-Watchdog,
36. Isolation einer fehlerhaften CPU,
37. Thermal-Kapazitätsreduzierung,
38. Capability- und Auditprüfung,
39. korrekte Diagnose und Statistik.

## 62. Verbindliche Invarianten

1. Jede logische CPU besitzt eine innerhalb des Bootvorgangs eindeutige CPU ID.
2. Eine CPU führt erst nach vollständiger Initialisierung reguläre Threads aus.
3. Jede aktive CPU besitzt gültige per-CPU-Daten, einen Idle-Thread und einen Kernel-Stack.
4. Die systemweite Featuremenge enthält nur auf allen erforderlichen CPUs sicher nutzbare Funktionen.
5. Die letzte aktive CPU wird nicht regulär offline geschaltet.
6. Eine offline oder fehlerhafte CPU erhält keine neue Schedulerarbeit.
7. Threads werden nur auf CPUs mit kompatiblen CPU-Funktionen ausgeführt.
8. IPIs werden ausschließlich über kontrollierte Kernelpfade ausgelöst.
9. Microcode wird nur nach erfolgreicher Validierung angewendet.
10. CPU-lokale Ressourcen werden vor physischer Entfernung vollständig freigegeben.
11. Fehlende Sicherheitsfunktionen werden im Systemstatus korrekt ausgewiesen.
12. CPU-Zustandsänderungen erfolgen unter geeigneter Synchronisation.

## 63. Referenzablauf eines AP-Starts

```text
BSP erkennt zusätzliche CPU
  -> CPU-Objekt und CPU ID erzeugen
  -> Topologie und Features erfassen
  -> Kompatibilität prüfen
  -> per-CPU-Daten und Stacks allokieren
  -> Runqueue und Idle-Thread vorbereiten
  -> AP-Trampoline und Startparameter einrichten
  -> architekturspezifisches Startsignal senden
  -> AP aktiviert Kerneladressraum
  -> AP initialisiert lokale Tabellen und Security-Features
  -> AP initialisiert Interrupt-Controller und Timer
  -> AP meldet Zustand ONLINE
  -> BSP nimmt CPU in Scheduler- und Interrupt-Sets auf
  -> CPU wechselt nach ACTIVE
```

## 64. Zusammenfassung

Der NovaOS CPU Manager stellt ein einheitliches Objekt- und Zustandsmodell für Einzelprozessor-, SMP-, NUMA- und heterogene CPU-Systeme bereit.

Jede aktive CPU erhält eigene Stacks, per-CPU-Daten, Runqueue, Idle-Thread, Interrupt-Controller und Timer. Hardwarefunktionen und Sicherheitsmechanismen werden pro CPU validiert, bevor sie reguläre Kernelarbeit ausführen darf.

CPU Hotplug, Microcode-Updates, Topologieinformationen, Fehlerisolation und dynamische Kapazitätswerte ermöglichen die enge Zusammenarbeit mit Scheduler, Power Manager, Interrupt Manager und Security Manager.