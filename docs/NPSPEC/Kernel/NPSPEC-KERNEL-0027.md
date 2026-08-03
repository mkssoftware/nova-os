# NPSPEC-KERNEL-0027 – SMP Architecture

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-KERNEL-0027 |
| Titel | SMP Architecture |
| Status | Spezifikation |
| Version | 1.0 |
| Datum | 2026-08-03 |
| Bereich | Kernel / Multiprozessorarchitektur |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | NPSPEC-KERNEL-0001, NPSPEC-KERNEL-0003, NPSPEC-KERNEL-0006, NPSPEC-KERNEL-0007, NPSPEC-KERNEL-0009, NPSPEC-KERNEL-0014, NPSPEC-KERNEL-0016, NPSPEC-KERNEL-0026 |
| Zugehörige ADRs | ADR-PROC-0006, ADR-PROC-0007, ADR-PROC-0008, ADR-MM-0012, ADR-POWER-0001, ADR-SEC-0006 |

---

## 1. Zweck

Diese Spezifikation definiert die Symmetric Multiprocessing Architecture von NovaOS.

Die SMP-Architektur beschreibt:

- Betrieb mehrerer logischer CPUs,
- Start und Synchronisation zusätzlicher Prozessoren,
- CPU-lokale und gemeinsam genutzte Daten,
- Speicherordnung und Cache-Kohärenz,
- Inter-Processor Interrupts,
- TLB-Shootdowns,
- Scheduler- und Interruptverteilung,
- CPU Hotplug,
- Fehlerisolation,
- Skalierbarkeit des Kernels.

## 2. Ziele

Die SMP-Architektur muss:

- Ein- und Mehrprozessorsysteme mit derselben Kernelbasis unterstützen,
- parallele Ausführung von Kernel- und Userspace-Threads ermöglichen,
- globale Sperren im Fast Path begrenzen,
- Cache- und NUMA-Lokalität berücksichtigen,
- CPU-lokale Interrupts und Timer bereitstellen,
- sichere Cross-CPU-Operationen ermöglichen,
- CPU Hotplug und Fehlerisolation unterstützen,
- heterogene CPUs kontrolliert integrieren,
- keine implizite sequenzielle Ausführung voraussetzen.

## 3. Grundmodell

NovaOS verwendet ein SMP-Modell, in dem grundsätzlich jede aktive CPU:

- Kernelcode ausführen,
- Userspace-Threads einplanen,
- Interrupts verarbeiten,
- Systemaufrufe bearbeiten,
- Timerereignisse verwalten,
- auf gemeinsam genutzte Kernelobjekte zugreifen

kann.

Ausnahmen werden durch CPU-Affinitäten, CPU-Isolation oder architekturspezifische Einschränkungen definiert.

## 4. SMP-Begriffe

| Begriff | Bedeutung |
|---|---|
| BSP | CPU, die den Kernel initial startet |
| AP | zusätzliche Application Processor CPU |
| Per-CPU | ausschließlich einer CPU zugeordnete Daten |
| Shared State | von mehreren CPUs erreichbarer Zustand |
| IPI | Inter-Processor Interrupt |
| TLB Shootdown | Invalidierung von TLB-Einträgen auf anderen CPUs |
| Memory Ordering | Reihenfolge sichtbarer Speicheroperationen |
| False Sharing | unnötige Cachekonflikte durch gemeinsame Cache Lines |
| Quiescent State | Zustand ohne kritischen Zugriff auf bestimmte Daten |

## 5. CPU-Mengen

Der Kernel verwaltet mindestens:

```c
typedef struct np_smp_cpu_sets {
    np_cpu_set_t possible;
    np_cpu_set_t discovered;
    np_cpu_set_t present;
    np_cpu_set_t online;
    np_cpu_set_t active;
    np_cpu_set_t isolated;
    np_cpu_set_t failed;
} np_smp_cpu_sets_t;
```

| Menge | Bedeutung |
|---|---|
| `possible` | durch Kernel und Plattform maximal unterstützte CPUs |
| `discovered` | durch Firmware oder Hardware erkannte CPUs |
| `present` | aktuell physisch oder virtuell vorhandene CPUs |
| `online` | vollständig initialisierte CPUs |
| `active` | für Scheduling freigegebene CPUs |
| `isolated` | nicht für allgemeines Load Balancing verwendete CPUs |
| `failed` | wegen Fehlern gesperrte CPUs |

## 6. CPU ID und Hardware ID

NovaOS trennt:

- kompakte kernelinterne CPU ID,
- architekturspezifische Hardware ID.

```c
typedef uint32_t np_cpu_id_t;
typedef uint64_t np_cpu_hardware_id_t;
```

Kernelstrukturen verwenden bevorzugt die CPU ID.

Controller- und Firmwareoperationen verwenden die zugehörige Hardware ID.

## 7. Boot Processor

Der BSP initialisiert vor dem SMP-Start:

- frühen Kerneladressraum,
- Physical und Virtual Memory Manager,
- Interruptbasis,
- CPU Manager,
- gemeinsamen Kernelzustand,
- AP-Trampoline,
- grundlegenden Scheduler,
- SMP-Synchronisationsstrukturen.

Der BSP darf zusätzliche CPUs erst starten, wenn alle für APs sichtbaren gemeinsamen Daten vollständig veröffentlicht wurden.

## 8. AP-Trampoline

Der AP-Trampoline-Code ist architekturspezifisch und möglichst klein.

Er muss:

- die eigene CPU sicher identifizieren,
- einen gültigen temporären Stack verwenden,
- den Kerneladressraum aktivieren,
- CPU-lokale Daten laden,
- in den generischen AP-Startpfad wechseln.

Der Trampoline-Code darf keine uninitialisierten globalen Kernelservices verwenden.

## 9. AP-Startparameter

```c
typedef struct np_ap_start_parameters {
    np_cpu_id_t cpu_id;
    np_cpu_hardware_id_t hardware_id;

    np_physical_address_t page_table_root;
    np_virtual_address_t temporary_stack_top;
    np_virtual_address_t cpu_local_base;

    np_virtual_address_t entry_point;
    np_atomic_uint32_t state;
} np_ap_start_parameters_t;
```

Jede AP-Startanforderung besitzt einen getrennten oder sicher serialisierten Parametersatz.

## 10. AP-Startzustände

```c
typedef enum np_ap_start_state {
    NP_AP_NOT_STARTED,
    NP_AP_SIGNALLED,
    NP_AP_TRAMPOLINE,
    NP_AP_ARCH_READY,
    NP_AP_KERNEL_READY,
    NP_AP_ONLINE,
    NP_AP_FAILED
} np_ap_start_state_t;
```

Statusänderungen verwenden atomare Operationen und geeignete Speicherbarrieren.

## 11. AP-Startablauf

```text
BSP reserviert CPU-lokale Ressourcen
  -> AP-Startparameter initialisieren
  -> Startdaten mit Release-Semantik veröffentlichen
  -> architekturspezifisches Startsignal senden
  -> AP liest Parameter mit Acquire-Semantik
  -> AP aktiviert Kerneladressraum
  -> AP initialisiert lokale CPU-Funktionen
  -> AP initialisiert Interrupt-Controller und Timer
  -> AP erstellt Idle-Kontext
  -> AP meldet ONLINE
  -> BSP nimmt AP in die aktive CPU-Menge auf
```

Jede Phase besitzt einen definierten Timeout.

## 12. SMP-Bootbarrieren

Der Kernel verwendet Bootbarrieren, um Phasen zwischen CPUs zu koordinieren.

```c
typedef enum np_smp_boot_phase {
    NP_SMP_PHASE_ARCH_READY,
    NP_SMP_PHASE_MEMORY_READY,
    NP_SMP_PHASE_INTERRUPTS_READY,
    NP_SMP_PHASE_SCHEDULER_READY,
    NP_SMP_PHASE_OPERATIONAL
} np_smp_boot_phase_t;
```

Eine CPU darf keine Services einer späteren Phase verwenden, bevor die globale Phase freigegeben wurde.

## 13. Fehler beim AP-Start

Schlägt der Start einer zusätzlichen CPU fehl:

- wird die CPU nicht aktiv gesetzt,
- werden lokale Ressourcen zurückgerollt,
- wird ihre Hardware ID gesperrt,
- werden Scheduler- und Interruptmengen korrigiert,
- wird ein Diagnoseereignis erzeugt.

Das System darf mit weniger CPUs weiterarbeiten, sofern mindestens eine kompatible CPU aktiv bleibt.

## 14. Per-CPU-Daten

Häufig verwendete CPU-lokale Zustände werden in per-CPU-Bereichen gespeichert.

Dazu gehören:

- aktueller Thread,
- Runqueue,
- Idle-Thread,
- Interrupt- und Exceptiontiefe,
- Präemptionszähler,
- Timerqueue,
- Logging-Ringpuffer,
- Speicher- und Objektcaches,
- TLB-Shootdown-Status,
- CPU-Statistiken.

Per-CPU-Daten sollen Cache-Line-ausgerichtet sein.

## 15. Zugriff auf Per-CPU-Daten

```c
void* np_this_cpu_data(
    np_per_cpu_key_t key
);

void* np_cpu_data(
    np_cpu_id_t cpu_id,
    np_per_cpu_key_t key
);
```

`np_this_cpu_data()` darf nur verwendet werden, wenn CPU-Migration während des Zugriffs ausgeschlossen oder unschädlich ist.

Dazu kann Präemption vorübergehend deaktiviert werden.

## 16. Dynamische Per-CPU-Bereiche

Kernelmodule können begrenzte per-CPU-Datenbereiche registrieren.

```c
np_status_t np_per_cpu_allocate(
    size_t size,
    size_t alignment,
    np_per_cpu_handle_t* allocation
);
```

Die Allokation muss:

- für alle aktiven CPUs verfügbar sein,
- CPU Hotplug unterstützen,
- vor Modulentladung vollständig freigegeben werden,
- Größen- und Ausrichtungslimits einhalten.

## 17. Gemeinsame Daten

Gemeinsam genutzte veränderliche Daten benötigen eine dokumentierte Strategie:

- atomare Operationen,
- Spinlocks,
- Mutexes,
- Reader-Writer Locks,
- Sequence Locks,
- RCU,
- unveränderliche Kopien,
- Sharding nach CPU oder NUMA-Node.

Die bloße Annahme, Zugriffe würden selten gleichzeitig auftreten, ist unzulässig.

## 18. Speicherordnung

NovaOS verwendet das Speichermodell aus NPSPEC-KERNEL-0016.

SMP-relevante Veröffentlichungen verwenden mindestens:

```text
Writer:
  Daten vollständig schreiben
  -> Release Store auf Status oder Zeiger

Reader:
  Acquire Load auf Status oder Zeiger
  -> veröffentlichte Daten lesen
```

Architekturcode muss die notwendigen CPU- und Compilerbarrieren bereitstellen.

## 19. Cache-Kohärenz

NovaOS setzt für reguläre SMP-Systeme hardwaregestützte Cache-Kohärenz voraus oder verwendet eine architekturspezifische Kohärenzschicht.

Für nicht kohärente Bereiche müssen explizite Operationen bereitstehen:

```c
void np_cache_clean_range(
    np_virtual_address_t address,
    size_t size
);

void np_cache_invalidate_range(
    np_virtual_address_t address,
    size_t size
);
```

Geräte-DMA unterliegt zusätzlich dem DMA Framework.

## 20. Cache-Line-Ausrichtung

Häufig veränderte CPU-lokale Felder sollen nicht dieselbe Cache Line mit Feldern anderer CPUs teilen.

```c
#define NP_CACHELINE_ALIGNED \
    __attribute__((aligned(NP_CACHE_LINE_SIZE)))
```

Ausrichtung wird insbesondere verwendet für:

- Runqueue Locks,
- CPU-Statistiken,
- per-CPU-Zähler,
- Paketqueues,
- Logpufferpositionen,
- Allocator-Caches.

## 21. False Sharing

False Sharing wird durch folgende Maßnahmen reduziert:

- per-CPU-Sharding,
- Cache-Line-Padding,
- getrennte Lese- und Schreibfelder,
- Batch-Aggregation von Statistiken,
- NUMA-lokale Datenstrukturen.

Padding darf nicht ungeprüft als Teil einer stabilen öffentlichen ABI verwendet werden.

## 22. SMP-Synchronisation

Synchronisationsprimitive müssen auf allen unterstützten CPU-Architekturen korrekte atomare Semantik bereitstellen.

Spinlocks dürfen nur kurze kritische Abschnitte schützen.

Lange oder potenziell blockierende Operationen verwenden:

- Mutexes,
- Wait Queues,
- Completion Objects,
- asynchrone Work Queues.

Globale Sperren im häufigen Fast Path sollen vermieden werden.

## 23. Lock Ordering

Subsysteme definieren eine feste Reihenfolge beim Sperren mehrerer CPU- oder Runqueue-Objekte.

Für mehrere CPUs kann gelten:

```text
niedrigere CPU ID
  -> höhere CPU ID
```

Alternativ wird ein transaktionales oder lockfreies Verfahren verwendet.

Gegenläufige Cross-CPU-Sperrfolgen ohne feste Ordnung sind unzulässig.

## 24. Inter-Processor Interrupts

NovaOS unterstützt mindestens:

```c
typedef enum np_ipi_type {
    NP_IPI_RESCHEDULE,
    NP_IPI_TLB_SHOOTDOWN,
    NP_IPI_CALL_FUNCTION,
    NP_IPI_CPU_STOP,
    NP_IPI_CPU_WAKE,
    NP_IPI_DEBUG,
    NP_IPI_PANIC_STOP
} np_ipi_type_t;
```

IPI-Vektoren sind reserviert und dürfen nicht dynamisch an Geräte vergeben werden.

## 25. IPI-Zustellung

```c
np_status_t np_smp_send_ipi(
    const np_cpu_set_t* targets,
    np_ipi_type_t type,
    const void* payload
);
```

Die Zielmenge muss:

- online oder für den konkreten Startpfad zulässig,
- mit dem IPI-Typ kompatibel,
- frei von entfernten CPUs

sein.

Userspace darf keine IPIs direkt senden.

## 26. IPI-Handler

IPI-Handler müssen:

- kurz bleiben,
- nicht regulär blockieren,
- CPU-lokale Zustände verwenden,
- notwendige Speicherbarrieren ausführen,
- den Abschluss eindeutig bestätigen,
- Rekursion begrenzen.

Komplexe Arbeit wird in einen planbaren Kernelkontext verschoben.

## 27. Cross-CPU Calls

Der Kernel kann eine Funktion auf anderen CPUs ausführen lassen.

```c
np_status_t np_smp_call_function(
    const np_cpu_set_t* targets,
    np_smp_function_t function,
    void* context,
    uint32_t flags,
    np_time_ns_t deadline
);
```

Unterstützt werden:

- asynchrone Ausführung,
- synchrones Warten,
- Aufruf auf einer CPU,
- Aufruf auf allen aktiven CPUs.

## 28. Cross-CPU-Call-Regeln

Eine Cross-CPU-Funktion darf:

- nicht auf dieselbe synchrone Call-Kette zurückwarten,
- keine unzulässigen Sperren halten,
- nur im dokumentierten IPI- oder Threadkontext arbeiten,
- den übergebenen Kontext nicht nach dessen Lebensende verwenden.

Synchrone Aufrufe besitzen Timeouts und Fehlerstatus pro Ziel-CPU.

## 29. Reschedule IPI

Der Scheduler verwendet ein Reschedule IPI, wenn eine andere CPU ihre Threadauswahl neu bewerten soll.

Mögliche Gründe sind:

- höher priorisierter Thread wurde aufgeweckt,
- Affinität wurde geändert,
- CPU soll offline geschaltet werden,
- Realtime-Budget wurde verändert,
- Threadmigration steht an.

Mehrere ausstehende Reschedule-Anforderungen dürfen zusammengefasst werden.

## 30. TLB-Shootdown

Änderungen aktiver Adressräume können eine Invalidierung auf mehreren CPUs erfordern.

Der Ablauf ist:

```text
Mapping ändern und alte Seite sperren
  -> aktive CPU-Menge des Adressraums bestimmen
  -> lokale TLB invalidieren
  -> Shootdown-Anfrage veröffentlichen
  -> IPI an Ziel-CPUs senden
  -> Ziel-CPUs invalidieren betroffenen Bereich
  -> Bestätigungen abwarten
  -> alte physische Seite freigeben
```

Die Seite darf nicht vor Abschluss aller erforderlichen Invalidierungen wiederverwendet werden.

## 31. TLB-Shootdown-Anfrage

```c
typedef struct np_tlb_shootdown_request {
    np_address_space_id_t address_space_id;

    np_virtual_address_t start;
    uint64_t length;

    uint32_t flags;
    uint64_t generation;

    np_atomic_uint32_t remaining_cpus;
} np_tlb_shootdown_request_t;
```

Große Bereiche können als vollständige Adressrauminvalidierung behandelt werden.

## 32. TLB-Shootdown-Optimierung

Zur Reduzierung der Kosten können verwendet werden:

- ASIDs oder PCIDs,
- Generationen pro Adressraum,
- Batch-Invalidierungen,
- Lazy TLB,
- CPU-Masken aktiver Adressräume,
- Zusammenfassen benachbarter Bereiche.

Optimierungen dürfen keine veralteten Zugriffsrechte sichtbar lassen.

## 33. Scheduler-Architektur

Jede aktive CPU besitzt eine eigene Runqueue.

Der Scheduler vermeidet eine einzelne globale Runqueue.

SMP-Scheduling berücksichtigt:

- gewichtete Last,
- CPU-Kapazität,
- Cache-Topologie,
- NUMA-Nähe,
- SMT-Beziehungen,
- Energiezustand,
- Thermalgrenzen,
- Affinität.

## 34. Threadmigration

Ein Thread kann migriert werden, wenn:

- Ziel-CPU online und aktiv ist,
- Affinität die Ziel-CPU erlaubt,
- CPU-Features kompatibel sind,
- kein nicht migrierbarer CPU-lokaler Zustand aktiv ist,
- Scheduler und Thread Manager den Wechsel synchronisieren.

Ein Thread darf nie gleichzeitig auf Quell- und Ziel-CPU laufen.

## 35. Wakeup-Platzierung

Beim Aufwecken wird die Ziel-CPU bestimmt anhand von:

- letzter CPU,
- Cache-Lokalität,
- aktueller Last,
- NUMA-Speicherort,
- CPU-Kapazität,
- Energieprofil,
- Threadklasse,
- Affinität.

Kleine Lastunterschiede sollen keine unnötige Migration verursachen.

## 36. Load Balancing

Load Balancing erfolgt:

- bei Thread-Wakeup,
- periodisch,
- wenn eine CPU idle wird,
- bei CPU Hotplug,
- bei Thermal- oder Poweränderungen.

Es kann Aufgaben zwischen:

- SMT Threads,
- Cores,
- Clustern,
- Packages,
- NUMA-Nodes

verschieben.

Die Kosten steigen mit größerer Topologiedistanz.

## 37. Idle Balancing

Eine CPU ohne ausführbare Threads kann Arbeit von anderen CPUs übernehmen.

Dabei werden bevorzugt Threads gewählt, die:

- migrierbar sind,
- geringe Cache-Migrationskosten besitzen,
- nicht streng NUMA-gebunden sind,
- keine Realtime-Lokalität verlieren.

Ist keine geeignete Arbeit vorhanden, wird der Idle-Thread fortgesetzt.

## 38. Interruptverteilung

Routbare Geräteinterrupts werden über CPUs verteilt.

Die Platzierung berücksichtigt:

- zugehörige Gerätequeue,
- Verarbeitungsthread,
- NUMA-Node des Geräts,
- CPU-Last,
- Cache-Lokalität,
- Realtime-Richtlinie,
- CPU-Isolation.

Per-CPU-Interrupts werden nicht migriert.

## 39. Timerarchitektur

Jede CPU besitzt bevorzugt:

- lokale Timerqueue,
- lokales Clockevent-Gerät,
- Schedulerdeadline,
- Wakeup-Verarbeitung.

Globale Timer können einer CPU zugeordnet und bei Hotplug migriert werden.

Zeitmessung muss auf allen CPUs monoton konsistent bleiben.

## 40. Per-CPU-Allokatoren

Häufige Allokationen dürfen CPU-lokale Caches verwenden.

Beispiele sind:

- physische Order-0-Seiten,
- Kernel-Heap-Objekte,
- Paketpuffer,
- Timerobjekte,
- IPC-Nachrichten.

Bei CPU-Offlining werden diese Caches geleert oder an den zuständigen NUMA-Node zurückgegeben.

## 41. Per-CPU-Logging

Jede CPU verwendet einen eigenen Logging-Ringpuffer.

Dadurch benötigen normale Logeinträge keine globale Sperre.

Der Userspace Log Service führt die Ströme anhand von:

- Zeitstempel,
- CPU ID,
- Sequenznummer,
- Boot ID

zusammen.

## 42. RCU und Grace Periods

RCU-Grace-Perioden müssen alle relevanten CPUs berücksichtigen.

Eine CPU meldet einen Quiescent State beispielsweise bei:

- Context Switch,
- Userspace-Rückkehr,
- Idle-Eintritt,
- ausdrücklich registriertem RCU-Punkt.

Offline CPUs werden aus zukünftigen Grace Periods entfernt, nachdem ihre laufenden RCU-Abschnitte beendet wurden.

## 43. SMP und NUMA

SMP und NUMA werden gemeinsam betrachtet.

Gemeinsame Datenstrukturen sollen nach Möglichkeit:

- node-lokal allokiert,
- node-lokal gesperrt,
- per-CPU oder per-Node geshardet,
- nur bei Bedarf global aggregiert

werden.

Globale Konsistenz darf nicht durch falsche NUMA-Optimierung verloren gehen.

## 44. Heterogene SMP-Systeme

Auf heterogenen Systemen können CPUs unterschiedliche Befehlssatz- oder Kapazitätsklassen besitzen.

Der Kernel verwaltet:

- systemweit sichere Features,
- CPU-lokale Zusatzfeatures,
- Thread-Anforderungen,
- kompatible CPU-Sets,
- Kapazitätswerte.

Ein Thread mit CPU-spezifischem erweitertem Zustand darf nur auf kompatiblen CPUs laufen.

## 45. CPU Hotplug

CPU-Offlining erfolgt in Phasen:

```text
CPU aus aktiver Menge entfernen
  -> neue Thread- und Interruptzuweisungen verhindern
  -> Threads migrieren
  -> Interrupts und Timer umleiten
  -> Work Queues und Caches leeren
  -> RCU-, TLB- und Cross-CPU-Arbeit abschließen
  -> CPU in Stop-Pfad versetzen
  -> CPU aus Online-Menge entfernen
```

Die letzte aktive CPU darf nicht regulär offline geschaltet werden.

## 46. Hotplug-Rollback

Schlägt CPU-Offlining vor dem endgültigen Stop fehl, wird die CPU kontrolliert reaktiviert.

Dabei werden:

- Runqueue freigegeben,
- Interrupt-Routing wiederhergestellt,
- Timer aktiviert,
- CPU-Sets korrigiert,
- Diagnoseinformationen erzeugt.

Nach einem bereits bestätigten Hardware-Stop ist ein erneuter vollständiger Online-Vorgang erforderlich.

## 47. CPU-Isolation

Isolierte CPUs können vorgesehen sein für:

- Realtime,
- sicherheitskritische Workloads,
- dedizierte Netzwerkqueues,
- Diagnose,
- virtuelle Maschinen.

Sie bleiben Teil des SMP-Systems, werden jedoch aus bestimmten automatischen Verteilungsmechanismen ausgeschlossen.

Notfall-, Panic- und kritische Kernel-IPIs dürfen sie weiterhin erreichen.

## 48. Power Management

Der Power Manager kann Last auf weniger CPUs konsolidieren und nicht benötigte CPUs in tiefe Idle-Zustände versetzen.

Vor einem Package-Schlafzustand müssen alle betroffenen CPUs kompatible Zustände erreicht haben.

Energieoptimierungen dürfen keine zugesagten Realtime- oder Deadline-Grenzen verletzen.

## 49. Thermal Management

Bei thermischer Begrenzung kann die verfügbare Kapazität einzelner CPUs, Cores oder Packages sinken.

SMP-Subsysteme müssen:

- Kapazitätswerte aktualisieren,
- Last neu verteilen,
- Hintergrundarbeit drosseln,
- überhitzte CPUs gegebenenfalls offline schalten,
- kritische Systemthreads funktionsfähig halten.

Kritische Thermalabschaltungen haben Vorrang.

## 50. Panic-Koordination

Bei Kernel Panic bestimmt eine CPU atomar die Panic-Führung.

Die Panic-CPU:

- sendet Panic-Stop-IPIs,
- sammelt CPU-Kontexte,
- stoppt reguläre Schedulerarbeit,
- koordiniert Logging und Crash Dump,
- führt die Panic Policy aus.

Andere CPUs beginnen keinen konkurrierenden primären Panic-Pfad.

## 51. CPU-Fehlerisolation

Bei einem isolierbaren CPU-Fehler versucht NovaOS:

1. neue Arbeit von der CPU fernzuhalten,
2. Interrupts umzuleiten,
3. migrierbare Threads zu verschieben,
4. CPU-Kontext zu sichern,
5. CPU anzuhalten,
6. sie als `FAILED` zu markieren.

Ist der gemeinsame Kernelzustand möglicherweise beschädigt, wird nicht lokal fortgesetzt.

## 52. SMP-Watchdog

Der SMP-Watchdog erkennt:

- nicht reagierende CPUs,
- IPI-Timeouts,
- Schedulerstillstand,
- dauerhaft deaktivierte Interrupts,
- überlange kritische Abschnitte,
- steckengebliebene TLB-Shootdowns,
- unvollständige Hotplug-Operationen.

Eine Warnung kann vor Isolation oder Panic einen CPU-Stacktrace erfassen.

## 53. Skalierungsanforderungen

SMP-kritische Subsysteme sollen:

- keine einzelne globale Sperre im normalen Fast Path besitzen,
- per-CPU- oder per-Node-Daten verwenden,
- globale Statistiken verzögert aggregieren,
- Cross-CPU-IPIs bündeln,
- TLB-Shootdowns begrenzen,
- Cache-Line-Sharing reduzieren.

Ausnahmen müssen durch Messungen und Einfachheits- oder Sicherheitsgründe begründet sein.

## 54. SMP-API

```c
np_cpu_id_t np_smp_current_cpu(void);

const np_cpu_set_t* np_smp_online_cpus(void);
const np_cpu_set_t* np_smp_active_cpus(void);

np_status_t np_smp_send_ipi(
    const np_cpu_set_t* targets,
    np_ipi_type_t type,
    const void* payload
);

np_status_t np_smp_call_function(
    const np_cpu_set_t* targets,
    np_smp_function_t function,
    void* context,
    uint32_t flags,
    np_time_ns_t deadline
);
```

Userspace erhält keinen direkten Zugriff auf diese internen Kernel-APIs.

## 55. Capability-Rechte

Administrative SMP-Operationen verwenden mindestens:

| Capability | Bedeutung |
|---|---|
| `CAP_SMP_QUERY` | allgemeine CPU- und SMP-Informationen lesen |
| `CAP_SMP_TOPOLOGY` | detaillierte Topologie lesen |
| `CAP_SMP_AFFINITY` | CPU-Affinitäten verwalten |
| `CAP_SMP_HOTPLUG` | CPUs online oder offline schalten |
| `CAP_SMP_ISOLATE` | CPUs isolieren |
| `CAP_SMP_DIAGNOSTIC` | IPI-, TLB- und Watchdogdaten lesen |
| `CAP_SMP_ADMIN` | globale SMP-Richtlinien verwalten |

IPIs und Cross-CPU-Kernelcallbacks sind nicht als allgemeine Userspace-Capability verfügbar.

## 56. Sicherheit

Die SMP-Architektur muss sicherstellen, dass:

- nur vollständig initialisierte CPUs Kernelarbeit ausführen,
- Userspace keine IPIs oder CPU-lokalen Kernelzustände manipuliert,
- TLB-Shootdowns vor Wiederverwendung geschützter Seiten abgeschlossen werden,
- Threads nicht gleichzeitig auf mehreren CPUs laufen,
- CPU-Featureunterschiede bei Migration berücksichtigt werden,
- Panic- und Stop-IPIs nicht durch normale Workloads blockiert werden,
- per-CPU-Sicherheitsregister auf jeder CPU aktiviert werden,
- Debug- und CPU-Kontexte capability-geschützt bleiben,
- CPU-Isolation keine Sicherheitsprüfungen umgeht.

## 57. Datenschutz

Detaillierte CPU- und Topologiedaten können zur Geräteidentifikation beitragen.

Daher gelten:

- normale Anwendungen erhalten nur allgemeine CPU-Informationen,
- Hardware IDs und genaue Cachetopologie können eingeschränkt werden,
- vollständige SMP-Diagnosen bleiben lokal und capability-geschützt,
- externe Telemetrie ist standardmäßig deaktiviert.

## 58. Diagnose

Autorisierte Diagnosedienste können abrufen:

- CPU-Mengen und Zustände,
- Topologie,
- Runqueue-Last,
- IPI-Zähler und Timeouts,
- TLB-Shootdown-Latenzen,
- CPU-Startzeiten,
- Hotplug-Fehler,
- Cache- und NUMA-Informationen,
- Watchdogereignisse,
- Cross-CPU-Call-Status,
- fehlende CPU-Sicherheitsfunktionen.

## 59. Statistik

```c
typedef struct np_smp_statistics {
    uint64_t online_cpus;
    uint64_t active_cpus;

    uint64_t ap_start_attempts;
    uint64_t ap_start_failures;

    uint64_t ipi_sent;
    uint64_t ipi_received;
    uint64_t ipi_timeouts;

    uint64_t tlb_shootdowns;
    uint64_t tlb_shootdown_timeouts;

    uint64_t thread_migrations;
    uint64_t cpu_hotplug_operations;
    uint64_t watchdog_events;
} np_smp_statistics_t;
```

Statistiken sollen pro CPU und systemweit verfügbar sein.

## 60. Fehlercodes

| Fehlercode | Bedeutung |
|---|---|
| `NP_OK` | Operation erfolgreich |
| `NP_ERR_INVALID_CPU` | CPU ID ist ungültig |
| `NP_ERR_CPU_OFFLINE` | Ziel-CPU ist offline |
| `NP_ERR_CPU_FAILED` | Ziel-CPU wurde als fehlerhaft markiert |
| `NP_ERR_CPU_INCOMPATIBLE` | CPU ist nicht kompatibel |
| `NP_ERR_CPU_BUSY` | CPU kann nicht gestoppt oder migriert werden |
| `NP_ERR_LAST_CPU` | letzte aktive CPU darf nicht entfernt werden |
| `NP_ERR_AP_START_TIMEOUT` | AP-Start lief in einen Timeout |
| `NP_ERR_IPI_TIMEOUT` | IPI wurde nicht rechtzeitig bestätigt |
| `NP_ERR_TLB_TIMEOUT` | TLB-Shootdown lief in einen Timeout |
| `NP_ERR_AFFINITY_CONFLICT` | keine zulässige CPU verfügbar |
| `NP_ERR_CROSS_CPU_DEADLOCK` | zyklische Cross-CPU-Abhängigkeit erkannt |
| `NP_ERR_ACCESS_DENIED` | erforderliche Capability fehlt |
| `NP_ERR_NOT_SUPPORTED` | Funktion wird nicht unterstützt |

## 61. Fehlerbehandlung

Nicht kritische SMP-Fehler werden soweit möglich auf die betroffene CPU begrenzt.

Mögliche Reaktionen sind:

- IPI erneut senden,
- CPU aus der aktiven Menge entfernen,
- Threads und Interrupts migrieren,
- CPU isolieren,
- Hotplug zurückrollen,
- System mit weniger CPUs fortsetzen,
- Degraded Mode aktivieren.

Ein fehlgeschlagener TLB-Shootdown oder möglicher gemeinsamer Kernelzustandsschaden kann eine Kernel Panic erfordern.

## 62. Testanforderungen

Mindestens folgende Tests sind erforderlich:

1. Betrieb mit genau einer CPU,
2. Start mehrerer APs,
3. paralleler AP-Start,
4. AP-Start-Timeout,
5. SMP-Bootbarrieren,
6. per-CPU-Datenzugriff,
7. Präemptionsschutz bei `this_cpu`,
8. dynamische per-CPU-Allokation,
9. atomare Speicherordnung,
10. Release-/Acquire-Veröffentlichung,
11. Cache-Line-Ausrichtung,
12. Spinlock unter SMP-Last,
13. Cross-CPU-Call,
14. Cross-CPU-Call-Timeout,
15. Reschedule-IPI,
16. TLB-Shootdown für einzelne Seite,
17. TLB-Shootdown für Adressraum,
18. Wiederverwendung der Seite erst nach Bestätigung,
19. per-CPU-Runqueues,
20. Threadmigration,
21. Wakeup-Platzierung,
22. periodisches Load Balancing,
23. Idle Balancing,
24. Interruptverteilung,
25. per-CPU-Timerqueues,
26. per-CPU-Allokatorcaches,
27. per-CPU-Logging,
28. RCU-Grace-Period über mehrere CPUs,
29. NUMA-lokale Datenstrukturen,
30. heterogene CPU-Features,
31. CPU Hotplug Offline,
32. Hotplug-Rollback,
33. erneutes CPU-Onlining,
34. CPU-Isolation,
35. Panic-Stop aller CPUs,
36. CPU-Watchdog,
37. Isolation einer nicht reagierenden CPU,
38. Capability- und Diagnoseprüfung,
39. Lasttest mit maximal unterstützter CPU-Anzahl.

## 63. Verbindliche Invarianten

1. Eine CPU wird erst nach vollständiger Initialisierung in die aktive Menge aufgenommen.
2. Ein Thread läuft niemals gleichzeitig auf mehreren CPUs.
3. Per-CPU-Daten werden nur unter gültiger CPU-Bindung oder Synchronisation verwendet.
4. Gemeinsame veränderliche Daten besitzen eine dokumentierte SMP-Synchronisation.
5. TLB-geschützte Seiten werden erst nach Abschluss aller erforderlichen Shootdowns wiederverwendet.
6. Die letzte aktive CPU wird nicht regulär offline geschaltet.
7. Offline und fehlgeschlagene CPUs erhalten keine neue Arbeit.
8. IPI-Handler blockieren nicht regulär.
9. Cross-CPU-Operationen besitzen eindeutige Abschluss- und Timeoutregeln.
10. CPU-Featureunterschiede werden bei jeder Migration berücksichtigt.
11. Andere CPUs beginnen während einer Panic keinen konkurrierenden primären Dump-Pfad.
12. SMP-Zustandsänderungen erfolgen mit geeigneten atomaren Operationen und Speicherbarrieren.

## 64. Referenzablauf eines TLB-Shootdowns

```text
CPU 0 verändert ein aktives Mapping
  -> betroffene Seite gegen Wiederverwendung sperren
  -> aktive CPU-Menge des Adressraums bestimmen
  -> Shootdown-Anfrage mit neuer Generation veröffentlichen
  -> lokale TLB-Einträge invalidieren
  -> IPI an CPU 1 bis CPU N senden
  -> Ziel-CPUs prüfen Adressraum und Generation
  -> Ziel-CPUs invalidieren TLB-Einträge
  -> Ziel-CPUs reduzieren Bestätigungszähler
  -> CPU 0 wartet bis Zähler null oder Timeout
  -> bei Erfolg alte Seite freigeben
  -> bei Timeout CPU isolieren oder Panic Policy anwenden
```

## 65. Zusammenfassung

Die NovaOS SMP Architecture ermöglicht parallele Kernel- und Userspace-Ausführung auf Einzel-, Multi-Core-, SMT-, NUMA- und heterogenen Prozessorsystemen.

Per-CPU-Runqueues, Timer, Logging-Puffer und Allokatorcaches reduzieren globale Sperrkonflikte. IPIs koordinieren Scheduling, TLB-Invalidierungen, Cross-CPU-Arbeit, Hotplug und Panic-Zustände.

Klare Speicherordnungsregeln, CPU-Featureprüfung, sichere TLB-Shootdowns, CPU Hotplug und Watchdog-basierte Fehlerisolation gewährleisten, dass Skalierbarkeit nicht auf Kosten von Speicher- oder Kernelintegrität erreicht wird.