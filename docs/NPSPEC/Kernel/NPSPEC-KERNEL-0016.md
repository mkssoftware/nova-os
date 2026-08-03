# NPSPEC-KERNEL-0016 – Synchronisation

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-KERNEL-0016 |
| Titel | Synchronisation |
| Status | Spezifikation |
| Version | 1.0 |
| Datum | 2026-08-03 |
| Bereich | Kernel / Nebenläufigkeit |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | NPSPEC-KERNEL-0003, NPSPEC-KERNEL-0005, NPSPEC-KERNEL-0009, NPSPEC-KERNEL-0012, NPSPEC-KERNEL-0014, NPSPEC-KERNEL-0015 |
| Zugehörige ADRs | ADR-PROC-0004, ADR-PROC-0005, ADR-PROC-0008, ADR-IPC-0003, ADR-KERNEL-0101 |

---

## 1. Zweck

Diese Spezifikation definiert die Synchronisationsmechanismen des NovaOS-Kernels.

Sie beschreibt:

- atomare Operationen,
- Speicherbarrieren,
- Spinlocks,
- Mutexes,
- Reader-Writer Locks,
- Semaphoren,
- Condition Variables,
- Wait Queues,
- Completion Objects,
- Sequence Locks,
- Read-Copy-Update,
- Userspace-Synchronisation,
- Deadlock- und Prioritätsinversionsschutz.

## 2. Ziele

Die Synchronisationsarchitektur muss:

- korrekte Nebenläufigkeit auf SMP-Systemen ermöglichen,
- Interrupt-, Kernelthread- und Userspace-Kontexte unterscheiden,
- verlorene Wakeups verhindern,
- Prioritätsinversion begrenzen,
- kurze kritische Abschnitte fördern,
- skalierbare CPU-lokale Verfahren unterstützen,
- falsche Sperrverwendung diagnostizieren,
- definierte Speicherordnungsregeln bereitstellen,
- Energieverbrauch durch unnötiges aktives Warten begrenzen.

## 3. Grundprinzipien

Für Synchronisation gelten folgende Grundregeln:

1. Gemeinsame veränderliche Daten benötigen eine definierte Synchronisation.
2. Die leichteste korrekte Synchronisationsform soll verwendet werden.
3. Spinlocks sind ausschließlich für kurze nicht blockierende Abschnitte vorgesehen.
4. Blockierende Sperren dürfen nicht im direkten Interruptkontext verwendet werden.
5. Sperrreihenfolgen müssen dokumentiert und eingehalten werden.
6. Objektlebenszeit und Datenzugriff müssen gemeinsam abgesichert werden.
7. Atomare Operationen ersetzen nicht automatisch eine vollständige Sperrstrategie.

## 4. Ausführungskontexte

NovaOS unterscheidet:

| Kontext | Darf blockieren | Darf Spinlocks verwenden |
|---|---:|---:|
| Userspace-Thread | über Systemdienst | nicht direkt |
| Kernelthread | Ja | Ja |
| Systemaufruf | Ja, sofern erlaubt | Ja |
| Threaded Interrupt | Ja, eingeschränkt | Ja |
| Direkter Interrupt | Nein | Ja |
| NMI/Machine Check | Nein | nur speziell geeignete Mechanismen |
| Idle-Thread | grundsätzlich Nein | nur CPU-lokal |

Eine Funktion muss dokumentieren, in welchen Kontexten sie aufgerufen werden darf.

## 5. Präemptionskontext

Kernel-Präemption kann für sehr kurze CPU-lokale Abschnitte deaktiviert werden.

```c
void np_preempt_disable(void);
void np_preempt_enable(void);
bool np_preempt_is_disabled(void);
```

Jede Deaktivierung muss auf derselben CPU ausgeglichen werden.

Präemptionsdeaktivierung schützt nicht gegen Interrupts oder Zugriffe anderer CPUs.

## 6. Interruptzustand

Architekturspezifische Funktionen verwalten den lokalen Interruptzustand.

```c
np_irq_state_t np_irq_save_disable(void);
void np_irq_restore(np_irq_state_t state);
```

Der vorherige Zustand muss gespeichert und exakt wiederhergestellt werden.

Globale Interruptdeaktivierung über mehrere CPUs ist kein regulärer Synchronisationsmechanismus.

## 7. Atomare Datentypen

```c
typedef struct { volatile uint32_t value; } np_atomic_uint32_t;
typedef struct { volatile uint64_t value; } np_atomic_uint64_t;
typedef struct { volatile uintptr_t value; } np_atomic_uintptr_t;
typedef struct { volatile bool value; } np_atomic_bool_t;
```

Die Implementierung verwendet compiler- und architekturspezifische atomare Primitive.

Ein bloßes `volatile` stellt keine atomare Operation oder vollständige Speicherordnung sicher.

## 8. Atomare Operationen

Mindestens folgende Operationen werden bereitgestellt:

```c
uint32_t np_atomic_load_u32(
    const np_atomic_uint32_t* value,
    np_memory_order_t order
);

void np_atomic_store_u32(
    np_atomic_uint32_t* value,
    uint32_t new_value,
    np_memory_order_t order
);

uint32_t np_atomic_exchange_u32(
    np_atomic_uint32_t* value,
    uint32_t new_value,
    np_memory_order_t order
);

bool np_atomic_compare_exchange_u32(
    np_atomic_uint32_t* value,
    uint32_t* expected,
    uint32_t desired,
    np_memory_order_t success_order,
    np_memory_order_t failure_order
);

uint32_t np_atomic_fetch_add_u32(
    np_atomic_uint32_t* value,
    uint32_t increment,
    np_memory_order_t order
);
```

Entsprechende Varianten werden für unterstützte Datentypen bereitgestellt.

## 9. Speicherordnung

```c
typedef enum np_memory_order {
    NP_MEMORY_RELAXED,
    NP_MEMORY_ACQUIRE,
    NP_MEMORY_RELEASE,
    NP_MEMORY_ACQ_REL,
    NP_MEMORY_SEQ_CST
} np_memory_order_t;
```

| Ordnung | Verwendung |
|---|---|
| `RELAXED` | reine atomare Aktualisierung ohne zusätzliche Ordnung |
| `ACQUIRE` | nachfolgende Zugriffe dürfen nicht davor wandern |
| `RELEASE` | vorherige Zugriffe dürfen nicht danach wandern |
| `ACQ_REL` | kombinierte Acquire-/Release-Semantik |
| `SEQ_CST` | global strengste angebotene Ordnung |

Die schwächste nachweislich korrekte Ordnung sollte verwendet werden.

## 10. Speicherbarrieren

```c
void np_memory_barrier(void);
void np_read_barrier(void);
void np_write_barrier(void);
void np_compiler_barrier(void);
```

Zusätzlich können gerätespezifische I/O-Barrieren erforderlich sein.

Eine Compilerbarriere ersetzt keine CPU-Speicherbarriere.

## 11. Spinlocks

Spinlocks schützen kurze kritische Abschnitte, in denen Blockieren nicht zulässig oder nicht sinnvoll ist.

```c
typedef struct np_spinlock {
    np_atomic_uint32_t state;
#if NP_LOCK_DEBUG
    np_cpu_id_t owner_cpu;
    uintptr_t acquisition_site;
#endif
} np_spinlock_t;
```

```c
void np_spin_lock(np_spinlock_t* lock);
bool np_spin_try_lock(np_spinlock_t* lock);
void np_spin_unlock(np_spinlock_t* lock);
```

## 12. Spinlock-Regeln

Während eine Spinlock gehalten wird:

- darf der Thread nicht blockieren,
- darf keine schlaffähige Allokation erfolgen,
- darf kein regulärer Userspace-Zugriff mit Page-Fault-Risiko erfolgen,
- muss der kritische Abschnitt kurz bleiben,
- darf dieselbe nicht rekursive Sperre nicht erneut erworben werden.

Spinlocks sind standardmäßig nicht rekursiv.

## 13. IRQ-sichere Spinlocks

Daten, die sowohl im Thread- als auch im Interruptkontext derselben CPU verwendet werden, benötigen eine IRQ-sichere Sperroperation.

```c
np_irq_state_t np_spin_lock_irqsave(
    np_spinlock_t* lock
);

void np_spin_unlock_irqrestore(
    np_spinlock_t* lock,
    np_irq_state_t irq_state
);
```

Die Wiederherstellung muss den ursprünglichen lokalen Interruptzustand verwenden.

## 14. Ticket- und Queue-Spinlocks

Auf SMP-Systemen kann NovaOS faire oder skalierbare Spinlocks verwenden, beispielsweise:

- Ticket Locks,
- MCS Locks,
- architekturoptimierte Queue Locks.

Die interne Implementierung darf sich ändern, solange die definierte Semantik erhalten bleibt.

Bei hoher Konkurrenz soll ein unfairer einfacher Test-and-Set-Spinlock vermieden werden.

## 15. Adaptive Spinlocks

Ein adaptiver Lock darf kurz aktiv warten, wenn der Besitzer aktuell auf einer anderen CPU läuft.

Wird der Besitzer nicht ausgeführt oder dauert das Warten zu lange, muss auf einen blockierenden Pfad gewechselt werden.

Adaptive Spinlocks sind nur in Threadkontexten zulässig, in denen Blockieren grundsätzlich erlaubt ist.

## 16. Mutexes

Mutexes sind schlaffähige exklusive Sperren.

```c
typedef struct np_mutex {
    np_atomic_uint32_t state;
    np_thread_t* owner;
    np_wait_queue_t waiters;
    uint32_t flags;
} np_mutex_t;
```

```c
np_status_t np_mutex_lock(
    np_mutex_t* mutex
);

np_status_t np_mutex_lock_until(
    np_mutex_t* mutex,
    np_time_ns_t deadline
);

bool np_mutex_try_lock(
    np_mutex_t* mutex
);

void np_mutex_unlock(
    np_mutex_t* mutex
);
```

## 17. Mutex-Eigentum

Ein Mutex besitzt genau einen Thread als Eigentümer.

Nur der Eigentümer darf ihn regulär freigeben.

Mutexes sind standardmäßig nicht rekursiv.

Ein Freigabeversuch durch einen anderen Thread wird als Synchronisationsfehler behandelt.

## 18. Rekursive Mutexes

Rekursive Mutexes können ausdrücklich bereitgestellt werden:

```c
NP_MUTEX_RECURSIVE
```

Sie verwalten einen Erwerbszähler und werden erst bei dessen Rückkehr auf null freigegeben.

Rekursive Mutexes sollen nicht verwendet werden, um unklare Sperrarchitekturen zu verdecken.

## 19. Priority-Inheritance-Mutex

Ein Mutex kann Prioritätsvererbung aktivieren:

```c
NP_MUTEX_PRIORITY_INHERIT
```

Blockiert ein höher priorisierter Thread, erhält der Besitzer vorübergehend eine entsprechend erhöhte effektive Priorität.

Die Vererbung muss:

- über verschachtelte Mutexketten propagiert werden,
- Scheduler- und Job-Obergrenzen berücksichtigen,
- nach Freigabe neu berechnet werden,
- Zyklen und übermäßige Tiefe erkennen.

## 20. Priority Ceiling

Für kontrollierte Echtzeitumgebungen kann ein Mutex eine Prioritätsobergrenze definieren.

Beim Erwerb wird die effektive Priorität des Besitzers auf die definierte Ceiling angehoben.

Die Verwendung benötigt eine Realtime-Richtlinie und ist nicht Standard für normale Kernelmutexes.

## 21. Robuste Mutexes

Robuste Mutexes können den Tod ihres Besitzers erkennen.

Wartende Threads erhalten:

```text
NP_ERR_OWNER_TERMINATED
```

Der neue Besitzer muss den geschützten Zustand als konsistent markieren:

```c
np_status_t np_mutex_make_consistent(
    np_mutex_t* mutex
);
```

Kann der Zustand nicht repariert werden, wird der Mutex als nicht wiederherstellbar markiert.

## 22. Reader-Writer Locks

Reader-Writer Locks ermöglichen mehrere gleichzeitige Leser oder genau einen Schreiber.

```c
typedef struct np_rwlock {
    np_atomic_uint32_t state;
    np_wait_queue_t readers;
    np_wait_queue_t writers;
} np_rwlock_t;
```

```c
np_status_t np_rwlock_read_lock(np_rwlock_t* lock);
np_status_t np_rwlock_write_lock(np_rwlock_t* lock);
void np_rwlock_read_unlock(np_rwlock_t* lock);
void np_rwlock_write_unlock(np_rwlock_t* lock);
```

## 23. RW-Lock-Fairness

NovaOS verwendet standardmäßig eine begrenzte Writer-Präferenz.

Sobald ein Schreiber wartet, dürfen neue Leser nicht unbegrenzt an ihm vorbeiziehen.

Gleichzeitig darf eine dauerhafte Folge von Schreibern Leser nicht unbegrenzt verhungern lassen.

Die genaue Fairnessstrategie kann implementierungsabhängig bleiben.

## 24. Upgrade und Downgrade

Ein Write Lock kann kontrolliert zu einem Read Lock herabgestuft werden.

```c
void np_rwlock_downgrade(
    np_rwlock_t* lock
);
```

Ein direktes Upgrade von Read zu Write kann einen Deadlock erzeugen und ist nur über eine ausdrücklich definierte Try-Upgrade-Operation zulässig.

Bei fehlgeschlagenem Upgrade bleibt die dokumentierte Sperrsemantik erhalten.

## 25. Semaphoren

Semaphoren verwalten eine begrenzte Anzahl verfügbarer Einheiten.

```c
typedef struct np_semaphore {
    np_atomic_uint64_t count;
    uint64_t maximum;
    np_wait_queue_t waiters;
} np_semaphore_t;
```

```c
np_status_t np_semaphore_wait(
    np_semaphore_t* semaphore,
    np_time_ns_t deadline
);

np_status_t np_semaphore_release(
    np_semaphore_t* semaphore,
    uint64_t release_count
);
```

Der Zähler darf das definierte Maximum nicht überschreiten.

## 26. Condition Variables

Condition Variables ermöglichen das Warten auf eine durch eine externe Sperre geschützte Bedingung.

```c
np_status_t np_condition_wait(
    np_condition_t* condition,
    np_mutex_t* mutex,
    np_time_ns_t deadline
);

void np_condition_signal(
    np_condition_t* condition
);

void np_condition_broadcast(
    np_condition_t* condition
);
```

Die Mutexfreigabe und das Einreihen in die Wait Queue müssen atomar erscheinen.

## 27. Bedingungsschleifen

Nach dem Aufwachen muss die Bedingung erneut geprüft werden.

```c
np_mutex_lock(&mutex);

while (!condition_is_true()) {
    np_condition_wait(
        &condition,
        &mutex,
        NP_TIMEOUT_INFINITE
    );
}

use_protected_state();
np_mutex_unlock(&mutex);
```

Spurious Wakeups sind zulässig. Der Aufrufer darf nicht allein aus dem erfolgreichen Aufwachen auf einen bestimmten Zustand schließen.

## 28. Wait Queues

Wait Queues verbinden Synchronisationsobjekte mit blockierten Threads.

```c
typedef struct np_wait_queue {
    np_list_t waiters;
    np_spinlock_t lock;
    uint64_t generation;
} np_wait_queue_t;
```

Das Prüfen einer Bedingung und das Blockieren müssen so synchronisiert werden, dass keine Wakeups verloren gehen.

## 29. Wait-Einträge

```c
typedef struct np_wait_entry {
    np_thread_t* thread;
    uint32_t flags;
    uint32_t priority;
    uint64_t sequence;
} np_wait_entry_t;
```

Ein Thread kann je nach Wait-API auf ein oder mehrere Objekte warten.

Beim Timeout oder Abbruch muss er atomar aus allen betreffenden Queues entfernt werden.

## 30. Wakeup-Semantik

Ein Wakeup überführt einen blockierten Thread in den Zustand `READY`.

Mögliche Varianten sind:

- einen Thread wecken,
- alle Threads wecken,
- eine bestimmte Anzahl wecken,
- den höchstpriorisierten zulässigen Thread wecken.

Das Aufwachen garantiert keine sofortige Ausführung.

## 31. Completion Objects

Completion Objects signalisieren den Abschluss einer Operation.

```c
void np_completion_initialize(
    np_completion_t* completion
);

np_status_t np_completion_wait(
    np_completion_t* completion,
    np_time_ns_t deadline
);

void np_completion_signal(
    np_completion_t* completion
);
```

Sie eignen sich für:

- Treiberinitialisierung,
- asynchrone I/O-Abschlüsse,
- CPU-Startsequenzen,
- Kernelthread-Koordination.

## 32. Sequence Locks

Sequence Locks eignen sich für Daten mit häufigen Lese- und seltenen Schreibzugriffen.

```c
uint32_t np_seqlock_read_begin(
    const np_seqlock_t* lock
);

bool np_seqlock_read_retry(
    const np_seqlock_t* lock,
    uint32_t sequence
);

void np_seqlock_write_lock(
    np_seqlock_t* lock
);

void np_seqlock_write_unlock(
    np_seqlock_t* lock
);
```

Leser können wiederholen müssen und dürfen während des Lesens keine nicht wiederholbaren Nebenwirkungen ausführen.

## 33. RCU

Read-Copy-Update kann für sehr leselastige Kernelstrukturen verwendet werden.

Grundablauf:

```text
Leser betritt Read-Side Critical Section
  -> liest unveränderlichen aktuellen Zeiger
  -> verlässt Read-Side Critical Section

Schreiber erzeugt neue Version
  -> veröffentlicht neuen Zeiger atomar
  -> wartet Grace Period
  -> gibt alte Version frei
```

RCU wird nur für Strukturen eingesetzt, deren Lebenszyklus und Updatesemantik dafür geeignet sind.

## 34. RCU-API

```c
void np_rcu_read_lock(void);
void np_rcu_read_unlock(void);

void np_rcu_assign_pointer(
    void* volatile* destination,
    void* value
);

void* np_rcu_dereference(
    void* volatile const* source
);

void np_rcu_call(
    np_rcu_callback_t callback,
    void* context
);

void np_rcu_synchronize(void);
```

`np_rcu_synchronize()` darf nur in einem blockierbaren Kontext verwendet werden.

## 35. Per-CPU-Daten

CPU-lokale Daten können ohne globale Sperre verwendet werden, wenn:

- Präemption während des Zugriffs passend kontrolliert wird,
- Interruptzugriffe berücksichtigt werden,
- kein anderer Prozessor dieselben Daten verändert,
- Aggregation und CPU-Hotplug korrekt synchronisiert werden.

Per-CPU-Daten ersetzen keine Synchronisation bei gemeinsamem Besitz.

## 36. Reference Counting

Referenzzählung schützt die Lebenszeit von Objekten.

```c
bool np_refcount_retain(
    np_refcount_t* reference_count
);

bool np_refcount_release(
    np_refcount_t* reference_count
);
```

Die Implementierung muss:

- Überlauf verhindern,
- Unterlauf erkennen,
- Wiederbelebung eines bereits freizugebenden Objekts verhindern,
- geeignete Acquire-/Release-Semantik verwenden.

Referenzzählung schützt nicht automatisch die veränderlichen Inhalte des Objekts.

## 37. Userspace-Synchronisation

Userspace-Laufzeitbibliotheken verwenden bevorzugt atomare Fast Paths.

Nur bei Konkurrenz oder Wartebedarf wird der Kernel aufgerufen.

```text
Uncontended Mutex
  -> atomare Userspace-Operation

Contended Mutex
  -> Kernel-Wait-Service
  -> Thread blockieren
  -> Kernel-Wake-Service
```

Dieses Modell reduziert unnötige Systemaufrufe.

## 38. Wait-on-Address

Der Kernel stellt eine kontrollierte Wait-on-Address-Schnittstelle bereit.

```c
np_status_t np_wait_on_address(
    const void* user_address,
    const void* expected_value,
    size_t value_size,
    np_time_ns_t deadline
);

np_status_t np_wake_address(
    void* user_address,
    uint32_t wake_count
);
```

Unterstützte Wertgrößen und Ausrichtungen werden durch die ABI festgelegt.

## 39. Wait-on-Address-Sicherheit

Der Kernel darf dem Userspace-Wert nicht als dauerhaft stabil vertrauen.

Er muss:

- Adresse und Größe validieren,
- Wert sicher kopieren,
- Mappingänderungen berücksichtigen,
- Wait Queue und erneute Prüfung atomar koordinieren,
- fremde Adressräume ohne Capability ablehnen.

Der Userspace bleibt für die semantische Konsistenz seiner Sperrstruktur verantwortlich.

## 40. Ereignisobjekte

Für prozessübergreifende oder handlebasierte Synchronisation werden Eventobjekte verwendet.

Unterstützt werden:

- Auto Reset,
- Manual Reset,
- Counting Event.

Events sind wartbare Kernelobjekte und können über das IPC Framework übertragen werden.

## 41. Timeout-Semantik

Blockierende Synchronisationsoperationen verwenden absolute monotone Deadlines.

Ein Timeout bedeutet, dass die Ressource nicht rechtzeitig erworben wurde.

Es muss eindeutig sein, ob der Aufrufer die Sperre bei Rückkehr besitzt.

Timeout und Wakeup müssen atomar gegeneinander aufgelöst werden, sodass eine Ressource nicht unbemerkt erworben wird.

## 42. Abbruch und Signale

Warteoperationen können je nach API:

- nicht unterbrechbar,
- durch Signale unterbrechbar,
- durch Thread-Cancellation abbrechbar,
- durch Prozessbeendigung abbrechbar

sein.

Die konkrete Semantik muss pro Synchronisationsobjekt dokumentiert werden.

Nach einem Abbruch darf der Thread nicht mehr in der Wait Queue verbleiben.

## 43. Sperrreihenfolge

Jedes Subsystem mit mehreren Sperren definiert eine eindeutige Sperrreihenfolge.

Beispiel:

```text
Namespace Lock
  -> Object Lock
  -> Handle Table Lock
  -> Wait Queue Lock
```

Die tatsächliche Reihenfolge wird pro Subsystem festgelegt.

Ein dynamisches Sperren mehrerer gleichartiger Objekte erfolgt beispielsweise nach aufsteigender Objektkennung.

## 44. Deadlock-Erkennung

Diagnose-Builds sollen einen Lock Dependency Tracker bereitstellen.

Er kann erfassen:

- gehaltene Sperren pro Thread,
- Erwerbsreihenfolge,
- Besitzer,
- Wartende,
- erkannte Zyklen,
- maximale Haltedauer,
- Erwerbsstelle.

Ein erkannter potenzieller Zyklus wird diagnostiziert, bevor er dauerhaft blockiert, soweit dies technisch möglich ist.

## 45. Lock-Rank

Sperrtypen können einen Rang besitzen.

```c
typedef uint32_t np_lock_rank_t;
```

Ein Thread darf normalerweise nur Sperren mit gleichem oder höherem zulässigem Folgerang erwerben.

Ausnahmen müssen ausdrücklich dokumentiert und durch spezielle Mechanismen abgesichert werden.

## 46. Sperrzeitüberwachung

Für kritische Sperren können maximale erwartete Halte- und Wartezeiten definiert werden.

Bei Überschreitung kann NovaOS:

- eine Warnung erzeugen,
- einen Stacktrace erfassen,
- den Besitzer priorisieren,
- einen Watchdog auslösen,
- bei nicht wiederherstellbarem Kernelstillstand die Panic-Policy anwenden.

Die Überwachung darf den normalen Fast Path nur gering belasten.

## 47. Lock Contention

Der Kernel erfasst optional:

- Erwerbsversuche,
- fehlgeschlagene Try-Locks,
- Spin-Zeit,
- Blockierzeit,
- maximale Wartezeit,
- Besitzerwechsel,
- Prioritätsvererbungen.

Diese Daten helfen, globale oder stark umkämpfte Sperren zu identifizieren.

## 48. Interrupt-Integration

Direkte Interrupt Handler dürfen ausschließlich nicht blockierende Synchronisationsmechanismen verwenden.

Wenn ein direkter Handler eine Spinlock verwendet, die auch im Threadkontext erworben wird, muss der Threadpfad den lokalen Interruptzustand entsprechend kontrollieren.

Zeitaufwendige oder blockierende Arbeit wird in Threaded Interrupts oder Deferred Work verschoben.

## 49. Scheduler-Integration

Das Blockieren auf einer Sperre übergibt die CPU an den Scheduler.

Der Scheduler muss:

- den Thread aus der Runqueue entfernen,
- den Wartegrund dokumentieren,
- Priority Inheritance anwenden,
- beim Wakeup die geeignete CPU wählen,
- Timeout- und Abbruchereignisse berücksichtigen.

Aktives Warten darf nicht als Ersatz für planbares Blockieren bei langen Wartezeiten verwendet werden.

## 50. CPU-Hotplug

Vor dem Offline-Schalten einer CPU müssen:

- CPU-lokale Wait- und RCU-Zustände abgeschlossen,
- Per-CPU-Synchronisationsdaten migriert oder zusammengeführt,
- ausstehende Callbacks verarbeitet,
- CPU-lokale Lockbesitzer ausgeschlossen werden.

Ein Prozessor darf nicht offline geschaltet werden, solange ein nicht migrierbarer kritischer Abschnitt aktiv ist.

## 51. Capability-Rechte

Kernelinterne Sperren sind keine direkt zugänglichen Userspace-Objekte.

Für öffentliche Synchronisationsobjekte gelten mindestens:

| Capability | Bedeutung |
|---|---|
| `CAP_SYNC_WAIT` | auf Objekt warten |
| `CAP_SYNC_SIGNAL` | Objekt signalisieren |
| `CAP_SYNC_RESET` | Ereignis zurücksetzen |
| `CAP_SYNC_QUERY` | Zustand abfragen |
| `CAP_SYNC_TRANSFER` | Synchronisationshandle übertragen |
| `CAP_SYNC_ADMIN` | globale Synchronisationsrichtlinien verwalten |

Rechte müssen auf konkrete Objekte begrenzbar sein.

## 52. Diagnose

Das Synchronisationssystem stellt autorisierten Diagnosediensten bereit:

- gehaltene Kernel-Sperren,
- wartende Threads,
- Sperrbesitzer,
- Wartezeiten,
- Prioritätsvererbungen,
- Deadlock-Warnungen,
- Wait-Queue-Auslastung,
- RCU-Grace-Perioden,
- robuste Owner-Termination-Ereignisse.

Sensible Adressen und Stacktraces erfordern erweiterte Debugberechtigungen.

## 53. Statistik

```c
typedef struct np_sync_statistics {
    uint64_t lock_acquisitions;
    uint64_t contended_acquisitions;
    uint64_t try_lock_failures;

    uint64_t blocked_waits;
    uint64_t timed_out_waits;
    uint64_t interrupted_waits;

    uint64_t priority_inheritances;
    uint64_t owner_terminations;
    uint64_t deadlock_warnings;

    uint64_t total_wait_time_ns;
    uint64_t maximum_wait_time_ns;
} np_sync_statistics_t;
```

Statistiken sollen pro Sperrtyp, Subsystem, CPU und System abrufbar sein.

## 54. Fehlercodes

| Fehlercode | Bedeutung |
|---|---|
| `NP_OK` | Operation erfolgreich |
| `NP_ERR_INVALID_ARGUMENT` | ungültiger Parameter |
| `NP_ERR_INVALID_STATE` | Operation im aktuellen Zustand unzulässig |
| `NP_ERR_WOULD_BLOCK` | nicht blockierender Erwerb fehlgeschlagen |
| `NP_ERR_TIMEOUT` | Deadline wurde erreicht |
| `NP_ERR_INTERRUPTED` | Warteoperation wurde unterbrochen |
| `NP_ERR_DEADLOCK` | Deadlock oder Selbstsperrung erkannt |
| `NP_ERR_NOT_OWNER` | Aufrufer besitzt die Sperre nicht |
| `NP_ERR_OWNER_TERMINATED` | vorheriger Besitzer wurde beendet |
| `NP_ERR_NOT_RECOVERABLE` | robuster Zustand kann nicht wiederhergestellt werden |
| `NP_ERR_COUNT_OVERFLOW` | Semaphore- oder Ereigniszähler würde überlaufen |
| `NP_ERR_ACCESS_DENIED` | erforderliche Capability fehlt |
| `NP_ERR_NOT_SUPPORTED` | Operation wird nicht unterstützt |

## 55. Sicherheitsanforderungen

Die Synchronisationsarchitektur muss sicherstellen, dass:

- Userspace keine internen Kernel-Sperren manipulieren kann,
- Wait-on-Address-Zeiger vollständig validiert werden,
- Referenzzähler nicht über- oder unterlaufen,
- Priority Inheritance keine unzulässige Priorität erzeugt,
- beendete Besitzer robuster Objekte erkannt werden,
- Sperrmetadaten keine Kerneladressen an Userspace offenlegen,
- verlorene Wakeups verhindert werden,
- Timeout und Wakeup atomar gegeneinander aufgelöst werden,
- freigegebene Synchronisationsobjekte keine wartenden Threads besitzen,
- direkte Interrupt Handler niemals auf blockierenden Sperren warten.

## 56. Fehlerbehandlung

Fehlerhafte Userspace-Synchronisation bleibt grundsätzlich auf den betreffenden Prozess begrenzt.

Folgende Situationen gelten als schwere Kernelkonsistenzfehler:

- Freigabe einer Kernelmutex durch einen Nichtbesitzer,
- zerstörte Sperre mit aktiven Waitern,
- beschädigte Wait Queue,
- verlorener Thread zwischen Queue und Scheduler,
- Referenzzähler-Unterlauf,
- dauerhaft gehaltene Spinlock nach Threadende,
- Blockieren im direkten Interruptkontext,
- nicht auflösbarer Deadlock kritischer Kernelthreads.

## 57. Testanforderungen

Mindestens folgende Tests sind erforderlich:

1. atomarer Load und Store,
2. Compare-and-Exchange,
3. Fetch-and-Add auf mehreren CPUs,
4. Acquire-/Release-Semantik,
5. Spinlock ohne Konkurrenz,
6. Spinlock mit SMP-Konkurrenz,
7. IRQ-sichere Spinlock,
8. Mutex Lock und Unlock,
9. Mutex Try-Lock,
10. Mutex-Timeout,
11. Ablehnung der Freigabe durch Nichtbesitzer,
12. rekursive Mutex,
13. Priority-Inheritance-Mutex,
14. verschachtelte Prioritätsvererbung,
15. robuste Mutex nach Besitzerende,
16. Reader-Writer Lock mit mehreren Lesern,
17. Writer-Fairness,
18. RW-Lock-Downgrade,
19. Semaphore Wait und Release,
20. Semaphore-Überlauf,
21. Condition Variable Signal,
22. Condition Variable Broadcast,
23. Spurious Wakeup,
24. Wait Queue ohne verlorenen Wakeup,
25. Completion Object,
26. Sequence Lock mit parallelen Lesern,
27. RCU-Update und Grace Period,
28. Per-CPU-Datenzugriff,
29. Referenzzählung und letzte Freigabe,
30. Wait-on-Address Fast Path,
31. Wait-on-Address Kernelpfad,
32. Timeout gegen gleichzeitiges Wakeup,
33. Abbruch einer Warteoperation,
34. Deadlock-Erkennung,
35. Lock-Rank-Verletzung,
36. CPU-Hotplug mit RCU und Per-CPU-Daten,
37. direkte Interruptkontextprüfung,
38. Prozessende mit robusten Userspace-Mutexes,
39. korrekte Diagnose und Statistik.

## 58. Verbindliche Invarianten

1. Eine exklusive Sperre besitzt höchstens einen Eigentümer.
2. Nur der Eigentümer darf eine normale Mutex freigeben.
3. Ein Thread blockiert nicht, während er eine nicht schlaffähige Spinlock hält.
4. Direkte Interrupt Handler verwenden keine blockierenden Sperren.
5. Das Einreihen in eine Wait Queue und die Zustandsprüfung verhindern verlorene Wakeups.
6. Ein Timeout führt niemals zu unklarem Sperrbesitz.
7. Priority Inheritance endet nach Wegfall ihrer Ursache.
8. Referenzzähler werden nicht unter null oder über ihre sichere Grenze verändert.
9. Ein Synchronisationsobjekt wird nicht mit aktiven Waitern zerstört.
10. Präemptions- und Interruptdeaktivierungen werden korrekt ausgeglichen.
11. Userspace kann keine Kernel-Sperrmetadaten verändern.
12. Alle gemeinsamen veränderlichen Daten besitzen eine dokumentierte Synchronisationsstrategie.

## 59. Referenzablauf einer blockierenden Mutex

```text
Thread fordert Mutex an
  -> atomaren Fast Path versuchen
  -> bei Erfolg Eigentümer setzen
  -> bei Konkurrenz Wait Queue sperren
  -> Zustand erneut prüfen
  -> Thread in Wait Queue einreihen
  -> Priority Inheritance anwenden
  -> Thread blockieren
  -> durch Unlock, Timeout oder Abbruch aufwecken
  -> Ergebnis atomar bestimmen
  -> bei Erfolg Eigentümer setzen
  -> Wait Queue verlassen
  -> geschützten Abschnitt ausführen
```

## 60. Zusammenfassung

NovaOS verwendet unterschiedliche Synchronisationsmechanismen für unterschiedliche Ausführungskontexte.

Atomare Operationen und Speicherbarrieren bilden die Grundlage. Spinlocks schützen kurze nicht blockierbare Abschnitte, während Mutexes, Reader-Writer Locks, Semaphoren und Condition Variables planbares Warten ermöglichen.

Wait Queues, Priority Inheritance, robuste Sperren, Deadlock-Diagnose und ein Userspace-Fast-Path über Wait-on-Address sorgen für sichere und skalierbare Nebenläufigkeit auf Einzel- und Mehrprozessorsystemen.