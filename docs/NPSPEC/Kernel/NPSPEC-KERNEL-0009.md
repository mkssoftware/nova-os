# NPSPEC-KERNEL-0009 – Interrupt Manager

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-KERNEL-0009 |
| Titel | Interrupt Manager |
| Status | Spezifikation |
| Version | 1.0 |
| Datum | 2026-08-03 |
| Bereich | Kernel / Interruptverwaltung |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | NPSPEC-KERNEL-0001, NPSPEC-KERNEL-0002, NPSPEC-KERNEL-0003, NPSPEC-KERNEL-0005 |
| Zugehörige ADRs | ADR-DEV-0004, ADR-DEV-0007, ADR-DEV-0008, ADR-DEV-0009, ADR-KERNEL-0101 |

---

## 1. Zweck

Diese Spezifikation definiert den Interrupt Manager des NovaOS-Kernels.

Der Interrupt Manager verwaltet:

- CPU-Exceptions,
- Hardware-Interrupts,
- Interruptvektoren,
- Interrupt-Controller,
- Interrupt-Routing,
- gemeinsam genutzte Interruptleitungen,
- MSI und MSI-X,
- Threaded Interrupts,
- Userspace-Treiberbenachrichtigungen,
- Interrupt-Affinitäten,
- Diagnose und Fehlerbehandlung.

## 2. Ziele

Der Interrupt Manager muss:

- architekturunabhängige Kernel-APIs bereitstellen,
- kurze und deterministische Hardware-Handler fördern,
- mehrere Interrupt-Controller unterstützen,
- SMP-fähiges Routing ermöglichen,
- Interrupt-Stürme begrenzen,
- Userspace-Treiber sicher anbinden,
- unzulässige Interruptzugriffe verhindern,
- lokale Diagnoseinformationen bereitstellen.

## 3. Abgrenzung

| Komponente | Verantwortung |
|---|---|
| Interrupt Manager | Vektoren, Handler, Routing und Zustellung |
| HAL | architekturspezifische Interruptoperationen |
| Device Manager | Geräte- und Treiberzuordnung |
| PCI Manager | MSI-/MSI-X-Fähigkeiten von PCI-Geräten |
| Scheduler | Einplanung von Interrupt-Threads |
| Event Core | asynchrone Weitergabe von Kernelereignissen |
| Power Management | Wake-Interrupts und Energiezustände |

## 4. Interruptklassen

NovaOS unterscheidet:

```c
typedef enum np_interrupt_class {
    NP_INTERRUPT_EXCEPTION,
    NP_INTERRUPT_HARDWARE,
    NP_INTERRUPT_MSI,
    NP_INTERRUPT_IPI,
    NP_INTERRUPT_TIMER,
    NP_INTERRUPT_SOFTWARE,
    NP_INTERRUPT_SPURIOUS
} np_interrupt_class_t;
```

| Klasse | Bedeutung |
|---|---|
| `EXCEPTION` | CPU-Exception oder Trap |
| `HARDWARE` | leitungsbasierter Geräteinterrupt |
| `MSI` | Message Signaled Interrupt |
| `IPI` | Inter-Processor Interrupt |
| `TIMER` | lokale oder globale Timerquelle |
| `SOFTWARE` | kontrolliert ausgelöster Softwareinterrupt |
| `SPURIOUS` | Interrupt ohne gültige Quelle |

## 5. Interruptnummern und Vektoren

Eine logische Interruptnummer identifiziert eine Quelle innerhalb des Interrupt Managers.

Ein Hardwarevektor ist eine architekturspezifische CPU-Eingangsnummer.

```c
typedef uint32_t np_interrupt_id_t;
typedef uint16_t np_interrupt_vector_t;
typedef uint64_t np_interrupt_source_id_t;
```

Logische Interruptnummer und CPU-Vektor dürfen nicht als identisch vorausgesetzt werden.

Der Interrupt Manager verwaltet die Zuordnung zwischen beiden.

## 6. Vektorbereiche

Auf x86 werden Vektoren grundsätzlich in Bereiche unterteilt:

| Bereich | Verwendung |
|---|---|
| `0–31` | CPU-Exceptions |
| `32+` | Hardware- und Softwareinterrupts |
| architekturspezifisch reserviert | IPI, Timer und Systemvektoren |
| letzter verfügbarer Bereich | Spurious- und Fehlervektoren |

Die konkrete Aufteilung wird durch die Architekturimplementierung definiert.

Reservierte Vektoren dürfen nicht dynamisch an Geräte vergeben werden.

## 7. Exception-Tabelle

CPU-Exceptions besitzen feste oder architekturspezifisch definierte Vektoren.

Für x86 gehören dazu unter anderem:

- Division Error,
- Debug Exception,
- Breakpoint,
- Invalid Opcode,
- General Protection Fault,
- Page Fault,
- Double Fault,
- Machine Check.

Jede Exception muss einen gültigen Handler besitzen, bevor Interrupts aktiviert werden.

## 8. Interrupt Descriptor Table

Die x86-Implementierung verwendet eine Interrupt Descriptor Table, kurz IDT.

```c
typedef struct np_x86_idt_entry {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t zero;
    uint8_t attributes;
    uint16_t offset_high;
} __attribute__((packed)) np_x86_idt_entry_t;
```

In einem späteren 64-Bit-Modus wird die Struktur entsprechend erweitert.

Die IDT muss:

- korrekt ausgerichtet sein,
- im Kernelspeicher liegen,
- gegen Userspace-Schreibzugriffe geschützt sein,
- vor Aktivierung der Interrupts vollständig geladen werden.

## 9. Interrupt-Stack

Interrupts werden auf einem gültigen Kernel-Stack verarbeitet.

Für kritische Exceptions sollen getrennte Notfall-Stacks verfügbar sein, insbesondere für:

- Double Fault,
- Machine Check,
- schwerwiegende Stackfehler,
- ausgewählte Non-Maskable Interrupts.

Jeder Stack muss durch Guard Pages geschützt werden, soweit die Architektur dies zulässt.

## 10. Interrupt Frame

Architekturspezifischer Einstiegscode erzeugt einen normalisierten Interrupt Frame.

```c
typedef struct np_interrupt_frame {
    uintptr_t instruction_pointer;
    uintptr_t stack_pointer;
    uintptr_t flags;

    uintptr_t error_code;
    uintptr_t fault_address;

    np_interrupt_vector_t vector;
    uint16_t privilege_level;
    uint32_t cpu_id;
} np_interrupt_frame_t;
```

Zusätzliche Register werden in einem architekturspezifischen Kontext gespeichert.

## 11. Interrupt-Einstieg

Der Low-Level-Einstieg muss:

1. erforderliche Register sichern,
2. Kernel-Segmente oder Kernelkontext aktivieren,
3. Thread- und CPU-lokale Daten verfügbar machen,
4. einen normalisierten Interrupt Frame erzeugen,
5. die generische Interruptverwaltung aufrufen,
6. gegebenenfalls eine Neuplanung auslösen,
7. Register wiederherstellen,
8. sicher zum unterbrochenen Kontext zurückkehren.

Die Rückkehr in den Userspace muss Privilegien und Kontext vollständig validieren.

## 12. Verschachtelte Interrupts

Verschachtelte Interrupts können unterstützt werden, wenn:

- die Architekturimplementierung dies sicher erlaubt,
- Interruptprioritäten korrekt definiert sind,
- pro CPU eine maximale Verschachtelungstiefe gilt,
- Stackgrenzen überwacht werden,
- derselbe Interrupt nicht unkontrolliert rekursiv ausgelöst wird.

In frühen Implementierungen dürfen Interrupts während des direkten Handlers grundsätzlich deaktiviert bleiben.

## 13. Interrupt-Handler

```c
typedef enum np_interrupt_result {
    NP_IRQ_NOT_HANDLED,
    NP_IRQ_HANDLED,
    NP_IRQ_WAKE_THREAD,
    NP_IRQ_DISABLE_SOURCE,
    NP_IRQ_FATAL
} np_interrupt_result_t;

typedef np_interrupt_result_t (*np_interrupt_handler_t)(
    np_interrupt_id_t interrupt,
    np_interrupt_frame_t* frame,
    void* context
);
```

Ein direkter Hardware-Handler muss möglichst kurz bleiben.

## 14. Handler-Registrierung

```c
typedef struct np_interrupt_handler_config {
    np_interrupt_handler_t handler;
    void* context;

    uint32_t flags;
    uint8_t priority;
    np_cpu_set_t affinity;

    const char* name;
} np_interrupt_handler_config_t;

np_status_t np_interrupt_register(
    np_interrupt_source_t* source,
    const np_interrupt_handler_config_t* config,
    np_interrupt_handle_t* handle
);
```

Eine Registrierung darf die Interruptquelle erst aktivieren, wenn Handler, Routing und Controllerzustand vollständig eingerichtet sind.

## 15. Handler-Entfernung

```c
np_status_t np_interrupt_unregister(
    np_interrupt_handle_t handle
);
```

Vor dem Entfernen müssen:

- die Quelle maskiert oder umgeleitet,
- laufende Handler abgeschlossen,
- Threaded-Handler gestoppt,
- ausstehende Zustellungen behandelt,
- alle CPU-Referenzen entfernt werden.

Der Handlerkontext darf erst danach freigegeben werden.

## 16. Interruptflags

```c
typedef enum np_interrupt_flags {
    NP_IRQ_SHARED        = 1U << 0,
    NP_IRQ_EDGE          = 1U << 1,
    NP_IRQ_LEVEL         = 1U << 2,
    NP_IRQ_THREADED      = 1U << 3,
    NP_IRQ_ONESHOT       = 1U << 4,
    NP_IRQ_WAKE_CAPABLE  = 1U << 5,
    NP_IRQ_PER_CPU       = 1U << 6,
    NP_IRQ_NO_BALANCE    = 1U << 7
} np_interrupt_flags_t;
```

Edge- und Level-Trigger dürfen nicht gleichzeitig gesetzt sein.

## 17. Trigger-Modi

### 17.1 Edge-triggered

Ein Edge-triggered Interrupt signalisiert einen Zustandswechsel.

Der Handler muss die Geräteursache rechtzeitig erfassen, da ein einzelner Impuls nicht dauerhaft anliegt.

### 17.2 Level-triggered

Ein Level-triggered Interrupt bleibt aktiv, solange die Geräteursache besteht.

Die Quelle darf erst wieder freigegeben werden, nachdem:

- die Ursache im Gerät bestätigt oder beseitigt,
- der Controller korrekt quittiert,
- der Handlerzustand konsistent ist.

## 18. Interrupt-Polarität

Leitungsbasierte Interrupts können unterschiedliche Polaritäten verwenden:

```c
typedef enum np_interrupt_polarity {
    NP_IRQ_POLARITY_DEFAULT,
    NP_IRQ_ACTIVE_HIGH,
    NP_IRQ_ACTIVE_LOW
} np_interrupt_polarity_t;
```

Trigger-Modus und Polarität werden aus Firmwaretabellen, Businformationen oder einer geprüften Gerätedefinition übernommen.

## 19. Interrupt-Controller-Abstraktion

```c
typedef struct np_interrupt_controller_ops {
    np_status_t (*initialize)(void* controller);
    np_status_t (*mask)(void* controller, np_interrupt_id_t interrupt);
    np_status_t (*unmask)(void* controller, np_interrupt_id_t interrupt);
    np_status_t (*acknowledge)(void* controller, np_interrupt_id_t interrupt);
    np_status_t (*end_of_interrupt)(void* controller, np_interrupt_id_t interrupt);
    np_status_t (*route)(void* controller, np_interrupt_id_t interrupt, np_cpu_id_t cpu);
    np_status_t (*set_trigger)(void* controller, np_interrupt_id_t interrupt, uint32_t mode);
} np_interrupt_controller_ops_t;
```

Der generische Interrupt Manager darf keine direkten PIC-, APIC- oder GIC-Register voraussetzen.

## 20. x86 Legacy PIC

Der Legacy PIC wird unterstützt für:

- ältere x86-Hardware,
- frühe Bootphasen,
- Systeme ohne funktionsfähigen APIC,
- Diagnose- und Kompatibilitätsmodi.

Bei Verwendung muss der PIC:

- auf nicht reservierte Vektoren umprogrammiert,
- mit sicheren Masken initialisiert,
- korrekt quittiert werden.

Ist APIC-Unterstützung aktiv, soll der Legacy PIC deaktiviert oder vollständig maskiert werden.

## 21. Local APIC

Auf unterstützten x86-Systemen wird der Local APIC für folgende Aufgaben verwendet:

- CPU-lokale Interruptzustellung,
- lokale Timerinterrupts,
- Inter-Processor Interrupts,
- Fehler- und Spurious-Vektoren,
- Prioritätssteuerung.

Jede aktive CPU besitzt einen initialisierten lokalen APIC-Kontext.

## 22. I/O APIC

Der I/O APIC routet leitungsbasierte Geräteinterrupts an CPUs.

Der Interrupt Manager verwaltet:

- Global System Interrupts,
- Vektorzuordnung,
- Trigger-Modus,
- Polarität,
- Ziel-CPU,
- Maskierungszustand.

Firmwareinformationen wie ACPI-MADT-Einträge müssen vor ihrer Verwendung validiert werden.

## 23. MSI

Message Signaled Interrupts werden bevorzugt verwendet, wenn Gerät und Plattform sie sicher unterstützen.

Vorteile sind:

- keine gemeinsam genutzte physische Leitung,
- flexibleres CPU-Routing,
- weniger Konflikte,
- bessere Mehrprozessor-Skalierung.

MSI-Konfiguration erfolgt über den zuständigen Busmanager und den Interrupt Manager.

## 24. MSI-X

MSI-X ermöglicht mehrere getrennte Interruptvektoren pro Gerät.

Diese können beispielsweise verwendet werden für:

- mehrere Empfangs- und Sendewarteschlangen,
- getrennte Fehlerinterrupts,
- CPU-lokale Gerätequeues,
- leistungsfähige Netzwerk- und Speichergeräte.

Jeder Vektor besitzt eine eigene Affinität und Handlerregistrierung.

Ein Userspace-Treiber darf MSI-X-Tabellen nicht unkontrolliert programmieren.

## 25. Vektorallokation

```c
np_status_t np_interrupt_vector_allocate(
    uint32_t count,
    uint32_t flags,
    np_interrupt_vector_t* first_vector
);

np_status_t np_interrupt_vector_free(
    np_interrupt_vector_t first_vector,
    uint32_t count
);
```

Die Vektorverwaltung muss:

- reservierte Vektoren ausschließen,
- zusammenhängende Bereiche bei Bedarf unterstützen,
- doppelte Vergaben verhindern,
- CPU-lokale und globale Vektoren unterscheiden,
- Freigaben erst nach abgeschlossener Synchronisation erlauben.

## 26. Gemeinsam genutzte Interrupts

Mehrere Geräte dürfen nur dann denselben leitungsbasierten Interrupt verwenden, wenn alle Handler mit `NP_IRQ_SHARED` registriert sind.

Jeder Handler prüft, ob sein Gerät die Interruptquelle ausgelöst hat.

Ein Shared Handler gibt zurück:

- `NP_IRQ_NOT_HANDLED`, wenn sein Gerät nicht verantwortlich war,
- `NP_IRQ_HANDLED`, wenn die Ursache verarbeitet wurde,
- `NP_IRQ_WAKE_THREAD`, wenn weitere Verarbeitung notwendig ist.

MSI- und MSI-X-Vektoren werden grundsätzlich nicht gemeinsam genutzt.

## 27. Top Half und Bottom Half

Die Interruptverarbeitung wird in zwei Teile getrennt.

### Top Half

Der direkte Handler:

- bestätigt die unmittelbare Hardwareursache,
- liest minimale Statusinformationen,
- schützt erforderliche Gerätedaten,
- plant nachgelagerte Arbeit ein.

### Bottom Half

Die nachgelagerte Verarbeitung erfolgt über:

- Threaded Interrupts,
- Deferred Work,
- Kernelthreads,
- Eventzustellung an Userspace-Treiber.

Zeitaufwendige Verarbeitung gehört nicht in den direkten Interruptkontext.

## 28. Threaded Interrupts

Ein Threaded Interrupt besitzt einen kurzen direkten Handler und einen planbaren Interrupt-Thread.

```c
typedef np_status_t (*np_threaded_interrupt_handler_t)(
    np_interrupt_id_t interrupt,
    void* context
);
```

Der direkte Handler gibt `NP_IRQ_WAKE_THREAD` zurück.

Der Scheduler kann dem Interrupt-Thread eine geeignete Priorität zuweisen. Realtime-Prioritäten erfordern eine ausdrücklich autorisierte Richtlinie.

## 29. Oneshot-Modus

Im Oneshot-Modus bleibt die Interruptquelle maskiert, bis der Threaded Handler seine Verarbeitung abgeschlossen hat.

Dies ist besonders für Level-triggered Interrupts geeignet, deren Ursache erst im Threadkontext vollständig beseitigt werden kann.

Ein blockierter oder abgestürzter Interrupt-Thread muss durch Timeout- und Recovery-Mechanismen erkannt werden.

## 30. Deferred Work

Nicht gerätespezifische kurze Nacharbeiten können über CPU-lokale Deferred-Work-Queues ausgeführt werden.

Deferred Work darf:

- andere Interrupts nicht unnötig blockieren,
- bei hoher Last begrenzt werden,
- bei Bedarf in einen Kernelthread verschoben werden.

Unbegrenzte Arbeitsmengen dürfen nicht vollständig in einem einzelnen Deferred-Work-Durchlauf abgearbeitet werden.

## 31. Userspace-Treiber

Userspace-Treiber erhalten keinen direkten CPU-Interrupt-Handler.

Stattdessen wird ein Interrupt-Endpunkt erzeugt:

```c
np_status_t np_interrupt_bind_endpoint(
    np_interrupt_handle_t interrupt,
    np_event_endpoint_handle_t endpoint
);
```

Der Kernel:

1. nimmt den Hardware-Interrupt entgegen,
2. maskiert bei Bedarf die Quelle,
3. signalisiert den autorisierten Endpunkt,
4. wartet auf die Treiberbestätigung,
5. gibt die Quelle anschließend wieder frei.

## 32. Userspace-Bestätigung

```c
np_status_t np_interrupt_acknowledge_user(
    np_interrupt_endpoint_handle_t endpoint,
    uint64_t sequence_number
);
```

Die Sequenznummer verhindert:

- doppelte Bestätigungen,
- Bestätigung alter Interrupts,
- Freigabe eines nicht verarbeiteten Ereignisses.

Ein Userspace-Treiber darf ausschließlich die ihm zugeordneten Interruptquellen bestätigen.

## 33. Treiberabsturz

Stürzt ein Userspace-Treiber ab, muss der Interrupt Manager:

- seine Interruptquellen maskieren,
- ausstehende Zustellungen abbrechen,
- Handlerbindungen entfernen oder pausieren,
- den Device Manager benachrichtigen,
- einen kontrollierten Treiberneustart ermöglichen.

Die Quelle darf erst nach erfolgreicher Wiederherstellung erneut aktiviert werden.

## 34. Interrupt-Affinität

Jeder routbare Interrupt besitzt eine Menge zulässiger Ziel-CPUs.

```c
np_status_t np_interrupt_set_affinity(
    np_interrupt_handle_t interrupt,
    const np_cpu_set_t* cpu_set
);
```

Die Affinität wird beeinflusst durch:

- Geräte- und Queue-Lokalität,
- NUMA-Topologie,
- CPU-Auslastung,
- Cache-Lokalität,
- Energiestatus,
- CPU-Hotplug,
- Realtime-Anforderungen.

Eine leere Affinitätsmenge ist unzulässig.

## 35. Interrupt Load Balancing

Der Interrupt Manager kann routbare Interrupts automatisch verteilen.

Dabei werden berücksichtigt:

- Interruptfrequenz,
- aktuelle CPU-Last,
- Scheduler-Auslastung,
- Gerätetopologie,
- NUMA-Nähe,
- Thread-Affinität der zugehörigen Verarbeitung,
- Energie- und Thermalrichtlinien.

Interrupts mit `NP_IRQ_NO_BALANCE` werden nicht automatisch verschoben.

## 36. CPU-Hotplug

Vor dem Offline-Schalten einer CPU müssen:

1. neue Interruptzuweisungen verhindert,
2. routbare Interrupts auf andere CPUs verschoben,
3. CPU-lokale Timer behandelt,
4. IPI-Zustände abgeschlossen,
5. ausstehende Handler beendet,
6. lokale Interrupt-Controller deaktiviert werden.

Per-CPU-Interrupts müssen ausdrücklich entfernt oder auf der CPU beendet werden.

## 37. Inter-Processor Interrupts

IPIs werden für Kernelkoordination verwendet.

Reservierte IPI-Typen können umfassen:

```c
typedef enum np_ipi_type {
    NP_IPI_RESCHEDULE,
    NP_IPI_TLB_SHOOTDOWN,
    NP_IPI_CALL_FUNCTION,
    NP_IPI_CPU_STOP,
    NP_IPI_CPU_WAKE,
    NP_IPI_DEBUG
} np_ipi_type_t;
```

Userspace-Prozesse dürfen IPIs nicht direkt auslösen.

Debug-IPIs müssen durch Secure-Debugging-Richtlinien geschützt werden.

## 38. Interruptprioritäten

Interruptprioritäten sind architektur- und controllerabhängig.

NovaOS verwendet logische Prioritätsklassen:

| Priorität | Typische Verwendung |
|---|---|
| Kritisch | Machine Check, CPU-Stop |
| Hoch | Timer, TLB-Shootdown, Realtime-Geräte |
| Normal | reguläre Geräteinterrupts |
| Niedrig | Hintergrund- und Diagnosegeräte |
| Minimal | optionale oder stark begrenzbare Quellen |

Treiber dürfen ihre Priorität nicht ohne Richtlinienprüfung selbst erhöhen.

## 39. Maskierung

```c
np_status_t np_interrupt_mask(
    np_interrupt_handle_t interrupt
);

np_status_t np_interrupt_unmask(
    np_interrupt_handle_t interrupt
);
```

Maskierungsoperationen können verschachtelt sein. Die Quelle wird erst wieder aktiviert, wenn alle gültigen Maskierungsgründe aufgehoben wurden.

Kritische Controller- und Kernelvektoren dürfen nicht über die normale Treiber-API maskiert werden.

## 40. Interrupt Storm Protection

Ein Interrupt Storm liegt vor, wenn eine Quelle in einem kurzen Zeitraum ungewöhnlich viele Interrupts erzeugt, ohne dass ausreichender Fortschritt erkennbar ist.

Mögliche Reaktionen sind:

1. Quelle zeitweise maskieren,
2. Zustellrate begrenzen,
3. Threaded Handler priorisieren,
4. Treiber diagnostizieren,
5. Gerät zurücksetzen,
6. Treiber oder Gerät isolieren.

Kritische Systeminterrupts erfordern eine eigene Fehlerstrategie und dürfen nicht unkontrolliert deaktiviert werden.

## 41. Spurious Interrupts

Spurious Interrupts werden gezählt und möglichst ihrer Quelle zugeordnet.

Vereinzelte Spurious Interrupts können toleriert werden.

Eine dauerhaft hohe Rate führt zu:

- Diagnoseereignis,
- Prüfung des Interrupt-Controllers,
- Maskierung einer fehlerhaften Quelle,
- möglicher Geräteisolierung.

Der Spurious-Vektor muss immer einen sicheren Handler besitzen.

## 42. Watchdog

Für ausgewählte Interruptquellen kann ein Watchdog überwachen:

- zu lange direkte Handler,
- nicht bestätigte Userspace-Interrupts,
- blockierte Threaded Handler,
- anhaltend maskierte Quellen,
- fehlende erwartete Interrupts.

Die Reaktion richtet sich nach der Kritikalität des betroffenen Geräts.

## 43. Power Management

Interruptquellen können als Wake-fähig markiert werden.

```c
np_status_t np_interrupt_set_wake(
    np_interrupt_handle_t interrupt,
    bool enabled
);
```

Vor einem Schlafzustand muss der Interrupt Manager:

- Wake-Quellen konfigurieren,
- nicht benötigte Quellen maskieren,
- Controllerzustände sichern,
- ausstehende Interrupts behandeln.

Nach dem Aufwachen werden Routing und Controllerzustände wiederhergestellt und geprüft.

## 44. Synchronisation

Handlerlisten, Vektorzuordnungen und Routingtabellen müssen gegen parallele Änderungen geschützt werden.

Der direkte Interruptpfad soll weitgehend ohne globale Sperren arbeiten.

Geeignete Verfahren sind:

- CPU-lokale Daten,
- atomare Zustände,
- kurze Spinlocks,
- Read-Copy-Update-ähnliche Verfahren,
- Generationszähler.

Schlaffähige Sperren dürfen im direkten Interruptkontext nicht verwendet werden.

## 45. Präemption und Scheduling

Der direkte Interruptkontext darf nicht regulär blockieren.

Nach der Interruptverarbeitung kann eine Neuplanung ausgelöst werden, wenn:

- ein höher priorisierter Thread aufgeweckt wurde,
- die aktuelle Zeitscheibe abgelaufen ist,
- ein Realtime- oder Deadline-Thread bereit wurde,
- ein CPU-lokales Reschedule-IPI vorliegt.

Der Context Switch erfolgt erst an einem sicheren Rückkehrpunkt.

## 46. Capability-Rechte

Mindestens folgende Rechte werden definiert:

| Capability | Bedeutung |
|---|---|
| `CAP_INTERRUPT_QUERY` | allgemeine Interruptinformationen lesen |
| `CAP_INTERRUPT_BIND` | Handler oder Endpunkt binden |
| `CAP_INTERRUPT_MASK` | zugewiesene Quelle maskieren |
| `CAP_INTERRUPT_ACK` | Userspace-Interrupt bestätigen |
| `CAP_INTERRUPT_AFFINITY` | Interrupt-Affinität verändern |
| `CAP_INTERRUPT_WAKE` | Quelle als Wake-Interrupt konfigurieren |
| `CAP_INTERRUPT_DIAGNOSTIC` | detaillierte Zähler und Traces lesen |
| `CAP_INTERRUPT_ADMIN` | Controller und globale Richtlinien verwalten |

Capabilities müssen auf konkrete Geräte oder Interruptquellen begrenzt sein.

## 47. Diagnose

Der Interrupt Manager stellt mindestens folgende Informationen bereit:

- Interruptkennung,
- Quelle und Controller,
- Vektor,
- Trigger-Modus und Polarität,
- Ziel-CPU oder Affinitätsmenge,
- Maskierungszustand,
- Handler oder Treiber,
- Interruptanzahl,
- unbehandelte Interrupts,
- maximale Handlerlaufzeit,
- Storm- und Timeout-Ereignisse.

Sensible Adressen oder vollständige Registerkontexte erfordern Debugberechtigungen.

## 48. Statistik

```c
typedef struct np_interrupt_statistics {
    uint64_t total_count;
    uint64_t handled_count;
    uint64_t unhandled_count;
    uint64_t spurious_count;

    uint64_t threaded_wakeups;
    uint64_t user_deliveries;
    uint64_t timeouts;
    uint64_t storm_events;

    uint64_t total_handler_time_ns;
    uint64_t maximum_handler_time_ns;
} np_interrupt_statistics_t;
```

Statistiken sollen pro Quelle und pro CPU abrufbar sein.

## 49. Tracing

Optionales Interrupt-Tracing kann erfassen:

- Eintritt und Austritt,
- CPU und Vektor,
- Handlerergebnis,
- Laufzeit,
- Thread-Wakeup,
- Routingänderung,
- Maskierung und Freigabe.

Tracing muss filterbar sein und darf die zeitkritische Interruptverarbeitung nicht wesentlich beeinträchtigen.

## 50. Fehlercodes

| Fehlercode | Bedeutung |
|---|---|
| `NP_OK` | Operation erfolgreich |
| `NP_ERR_INVALID_ARGUMENT` | ungültiger Parameter |
| `NP_ERR_INVALID_VECTOR` | ungültiger oder reservierter Vektor |
| `NP_ERR_INVALID_SOURCE` | Interruptquelle ist ungültig |
| `NP_ERR_VECTOR_EXHAUSTED` | kein passender Vektor verfügbar |
| `NP_ERR_ALREADY_BOUND` | Quelle ist bereits exklusiv gebunden |
| `NP_ERR_NOT_SHARED` | gemeinsame Registrierung nicht zulässig |
| `NP_ERR_TRIGGER_CONFLICT` | Trigger-Konfiguration widerspricht bestehender Nutzung |
| `NP_ERR_ACCESS_DENIED` | erforderliche Capability fehlt |
| `NP_ERR_CPU_UNAVAILABLE` | Ziel-CPU ist nicht verfügbar |
| `NP_ERR_TIMEOUT` | Synchronisation oder Bestätigung fehlgeschlagen |
| `NP_ERR_SOURCE_DISABLED` | Quelle wurde deaktiviert |
| `NP_ERR_NOT_SUPPORTED` | Funktion wird nicht unterstützt |

## 51. Sicherheitsanforderungen

Der Interrupt Manager muss sicherstellen, dass:

- Userspace keine IDT- oder Controllertabellen verändern kann,
- nicht autorisierte Treiber keine Interruptquellen binden können,
- Userspace-Treiber keine beliebigen MSI-Ziele programmieren,
- Interrupt-Frames vor der Rückkehr in den Userspace validiert werden,
- Kernel-Stacks für Userspace unzugänglich bleiben,
- direkte Handler nicht auf freigegebenen Kontext zugreifen,
- Vektoren nicht mehrfach exklusiv vergeben werden,
- Interrupt-Stürme das System nicht unbegrenzt blockieren,
- Debug- und IPI-Funktionen capability-geschützt sind,
- MMIO- und DMA-Rechte nicht allein durch Interruptbesitz entstehen.

## 52. Fehlerbehandlung

Ein fehlerhafter Gerätinterrupt soll möglichst auf Quelle, Gerät oder Treiber begrenzt werden.

Folgende Situationen gelten als schwere Kernelkonsistenzfehler:

- beschädigte IDT,
- ungültiger privilegierter Rückkehrkontext,
- Interrupt ohne gültigen Kernel-Stack,
- beschädigte Controller-Routingtabellen,
- TLB-Shootdown-IPI ohne sichere Verarbeitung,
- Freigabe eines noch laufenden Handlers,
- Überschreitung der sicheren Interruptverschachtelung.

Je nach Auswirkung wird die Quelle isoliert, die CPU gestoppt oder die Kernel-Panic-Policy aktiviert.

## 53. Testanforderungen

Mindestens folgende Tests sind erforderlich:

1. Aufbau und Laden der Interrupttabelle,
2. Handler für alle CPU-Exceptions,
3. Division-by-zero-Exception,
4. Invalid-Opcode-Exception,
5. kontrollierter Page Fault,
6. Legacy-PIC-Initialisierung,
7. Local-APIC-Initialisierung,
8. I/O-APIC-Routing,
9. Vektorallokation und Freigabe,
10. leitungsbasierter Edge-Interrupt,
11. leitungsbasierter Level-Interrupt,
12. gemeinsam genutzter Interrupt,
13. MSI-Registrierung,
14. mehrere MSI-X-Vektoren,
15. direkte Handlerverarbeitung,
16. Threaded Interrupt,
17. Oneshot-Maskierung,
18. Deferred Work,
19. Zustellung an einen Userspace-Treiber,
20. Ablehnung einer falschen Userspace-Bestätigung,
21. Behandlung eines Treiberabsturzes,
22. Interrupt-Affinitätsänderung,
23. Routing auf mehreren CPUs,
24. Interrupt Load Balancing,
25. CPU-Hotplug,
26. Reschedule-IPI,
27. TLB-Shootdown-IPI,
28. Spurious Interrupt,
29. Interrupt Storm Protection,
30. Handler-Timeout,
31. Wake-Interrupt während Suspend,
32. parallele Registrierung und Zustellung,
33. Entfernung eines aktiven Handlers,
34. Capability-Prüfung aller administrativen Operationen,
35. korrekte Statistiken und Trace-Ereignisse.

## 54. Verbindliche Invarianten

1. Jeder CPU-Vektor besitzt zu jedem Zeitpunkt eine eindeutige gültige Bedeutung.
2. Reservierte Vektoren werden niemals dynamisch an Geräte vergeben.
3. Jeder aktive Interrupt besitzt einen gültigen Handler oder sicheren Standardhandler.
4. Direkte Interrupt Handler blockieren nicht.
5. Ein Handlerkontext wird erst nach Abschluss aller laufenden Handler freigegeben.
6. Userspace-Treiber erhalten keinen direkten Zugriff auf Interrupt-Controller.
7. MSI- und MSI-X-Ziele werden ausschließlich durch vertrauenswürdige Kernelkomponenten programmiert.
8. Level-triggered Quellen werden erst nach Beseitigung der Ursache wieder freigegeben.
9. Interrupt-Affinitäten enthalten nur aktive und zulässige CPUs.
10. Jede aktive CPU besitzt einen gültigen Interrupt-Stack.
11. Die Rückkehr in den Userspace erfolgt nur mit validiertem Kontext.
12. Interruptzustandsänderungen erfolgen unter geeigneter Synchronisation.

## 55. Referenzablauf eines Hardware-Interrupts

```text
Hardware signalisiert Interrupt
  -> Interrupt-Controller liefert CPU-Vektor
  -> Architekturcode sichert Register
  -> Interrupt Manager bestimmt Quelle
  -> direkter Handler prüft und bestätigt das Gerät
  -> Controller wird korrekt quittiert
  -> optional Threaded Handler oder Userspace-Endpunkt wecken
  -> Scheduler-Neuplanung prüfen
  -> Registerkontext wiederherstellen
  -> unterbrochene Ausführung fortsetzen
```

## 56. Zusammenfassung

Der NovaOS Interrupt Manager abstrahiert CPU-Exceptions, leitungsbasierte Hardwareinterrupts, MSI, MSI-X und Inter-Processor Interrupts über eine einheitliche Kernelarchitektur.

Zeitkritische Direkt-Handler bleiben kurz. Umfangreichere Verarbeitung wird in Threaded Interrupts, Deferred Work oder capability-geschützte Userspace-Treiber verlagert.

Interrupt-Affinitäten, Load Balancing, Storm Protection, sichere Vektorverwaltung und lokale Diagnosefunktionen gewährleisten einen skalierbaren und robusten Betrieb auf Einzel- und Mehrprozessorsystemen.