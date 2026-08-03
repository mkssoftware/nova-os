# NPSPEC-KERNEL-0005 – Thread Manager

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-KERNEL-0005 |
| Titel | Thread Manager |
| Status | Spezifikation |
| Version | 1.0 |
| Datum | 2026-08-03 |
| Bereich | Kernel / Thread-Verwaltung |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | NPSPEC-KERNEL-0001, NPSPEC-KERNEL-0003, NPSPEC-KERNEL-0004 |
| Zugehörige ADRs | ADR-PROC-0002, ADR-PROC-0004, ADR-PROC-0006, ADR-PROC-0007, ADR-PROC-0008 |

---

## 1. Zweck

Diese Spezifikation definiert den Thread Manager des NovaOS-Kernels.

Der Thread Manager verwaltet:

- Erzeugung und Beendigung von Threads,
- Thread-Zustände und Zustandsübergänge,
- Kernel- und Userspace-Stacks,
- Ausführungskontexte,
- Thread-lokale Daten,
- Suspendierung und Fortsetzung,
- Warten und Schlafen,
- Thread-Handles und Capabilities,
- Zusammenarbeit mit Process Manager und Scheduler.

## 2. Grundmodell

Ein Thread ist die kleinste vom Scheduler ausführbare Einheit.

Jeder Thread gehört während seiner gesamten Lebensdauer genau einem Prozess. Kernelthreads gehören zum Kernelprozess mit der Prozesskennung `0`.

Ein Thread besitzt mindestens:

- eine eindeutige Threadkennung,
- einen Besitzerprozess,
- einen Ausführungskontext,
- einen Kernel-Stack,
- optional einen Userspace-Stack,
- Scheduler-Parameter,
- einen Capability-Kontext,
- einen definierten Lebenszyklus.

## 3. Threadkennung

```c
typedef uint64_t np_thread_id_t;
```

Die Threadkennung dient der Identifikation und Diagnose. Sie stellt keine Zugriffsberechtigung dar.

Eine Kennung darf nicht erneut verwendet werden, solange noch Handles oder Referenzen auf den alten Thread existieren.

## 4. Thread-Handle

Userspace-Komponenten greifen über capability-geschützte Handles auf Threads zu:

```c
typedef uint64_t np_thread_handle_t;
```

Ein Thread-Handle enthält oder referenziert:

- das Threadobjekt,
- erlaubte Operationen,
- den besitzenden Handle-Namespace,
- eine Generationskennung,
- Vererbungs- und Übertragungsregeln.

## 5. Threadarten

NovaOS unterscheidet folgende Threadarten:

```c
typedef enum np_thread_type {
    NP_THREAD_USER,
    NP_THREAD_KERNEL,
    NP_THREAD_IDLE,
    NP_THREAD_INTERRUPT,
    NP_THREAD_RECOVERY
} np_thread_type_t;
```

| Art | Bedeutung |
|---|---|
| `USER` | regulärer Thread eines Userspace-Prozesses |
| `KERNEL` | Kernelthread ohne normalen Userspace-Kontext |
| `IDLE` | CPU-lokaler Idle-Thread |
| `INTERRUPT` | Threaded Interrupt Handler |
| `RECOVERY` | besonders geschützter Recovery-Thread |

Interruptkontexte ohne eigenen Thread werden nicht als reguläre Threadobjekte eingeplant.

## 6. Thread-Zustände

```c
typedef enum np_thread_state {
    NP_THREAD_CREATED,
    NP_THREAD_READY,
    NP_THREAD_RUNNING,
    NP_THREAD_BLOCKED,
    NP_THREAD_SLEEPING,
    NP_THREAD_SUSPENDED,
    NP_THREAD_TERMINATING,
    NP_THREAD_TERMINATED
} np_thread_state_t;
```

| Zustand | Bedeutung |
|---|---|
| `CREATED` | Threadobjekt wurde erzeugt |
| `READY` | Thread kann vom Scheduler eingeplant werden |
| `RUNNING` | Thread wird auf einer CPU ausgeführt |
| `BLOCKED` | Thread wartet auf ein Objekt oder Ereignis |
| `SLEEPING` | Thread wartet auf einen Zeitpunkt |
| `SUSPENDED` | Thread wurde ausdrücklich angehalten |
| `TERMINATING` | Beendigung wurde eingeleitet |
| `TERMINATED` | Threadausführung ist vollständig beendet |

## 7. Zustandsübergänge

Reguläre Übergänge sind:

```text
CREATED -> READY
READY -> RUNNING
RUNNING -> READY
RUNNING -> BLOCKED
RUNNING -> SLEEPING
BLOCKED -> READY
SLEEPING -> READY
READY -> SUSPENDED
RUNNING -> SUSPENDED
BLOCKED -> SUSPENDED
SUSPENDED -> READY
* -> TERMINATING
TERMINATING -> TERMINATED
```

Ungültige Zustandsübergänge müssen mit einem Fehler abgelehnt werden.

Ein Thread im Zustand `TERMINATED` darf nicht reaktiviert werden.

## 8. Threadobjekt

```c
typedef struct np_thread {
    np_kernel_object_t object;

    np_thread_id_t thread_id;
    np_thread_type_t type;
    np_thread_state_t state;
    uint32_t flags;

    np_process_t* process;
    np_sched_entity_t scheduler;
    np_arch_thread_context_t* arch_context;

    np_kernel_stack_t kernel_stack;
    np_user_stack_t user_stack;

    np_wait_context_t wait_context;
    np_thread_security_t security;
    np_thread_local_state_t local_state;

    np_thread_exit_status_t exit_status;
    np_spinlock_t lock;
} np_thread_t;
```

Die interne Struktur ist nicht Bestandteil der öffentlichen ABI.

## 9. Threaderzeugung

Ein Thread wird in folgenden Schritten erzeugt:

1. Capability des Erzeugers prüfen,
2. Threadkennung reservieren,
3. Threadobjekt anlegen,
4. Besitzerprozess referenzieren,
5. Kernel-Stack erzeugen,
6. optional Userspace-Stack erzeugen,
7. Sicherheitskontext initialisieren,
8. Architekturkontext aufbauen,
9. Scheduler-Parameter anwenden,
10. Thread in Prozess und Kernel Object Graph registrieren,
11. Thread abhängig von den Erzeugungsflags freigeben.

Teilweise erzeugte Threads müssen bei Fehlern vollständig zurückgerollt werden.

## 10. Erzeugungsparameter

```c
typedef struct np_thread_create_info {
    uint32_t structure_size;
    uint32_t flags;

    uintptr_t entry_point;
    uintptr_t argument;
    uintptr_t stack_address;
    size_t stack_size;

    np_scheduler_class_t scheduler_class;
    np_priority_t priority;
    np_cpu_set_t affinity;
} np_thread_create_info_t;
```

Ein Userspace-Eintrittspunkt muss innerhalb eines ausführbaren Bereichs des Prozessadressraums liegen.

## 11. Thread-API

```c
np_status_t np_thread_create(
    np_process_handle_t process,
    const np_thread_create_info_t* create_info,
    np_thread_handle_t* thread
);

np_status_t np_thread_start(
    np_thread_handle_t thread
);

np_status_t np_thread_suspend(
    np_thread_handle_t thread
);

np_status_t np_thread_resume(
    np_thread_handle_t thread
);

np_status_t np_thread_terminate(
    np_thread_handle_t thread,
    np_thread_exit_status_t exit_status
);

np_status_t np_thread_wait(
    np_thread_handle_t thread,
    uint64_t timeout_ns,
    np_thread_exit_status_t* exit_status
);
```

Erzeugung und Start sind getrennte Operationen.

## 12. Initialer Prozess-Thread

Der erste Thread eines Prozesses wird durch den Process Manager erstellt.

Er erhält:

- den Programmeintrittspunkt,
- den initialen Userspace-Stack,
- den Prozessstartblock,
- Standardhandles,
- Thread-lokale Laufzeitdaten,
- das Sicherheitsprofil des Prozesses.

Der Thread darf erst gestartet werden, wenn das ausführbare Image vollständig geladen und validiert wurde.

## 13. Kernelthreads

Kernelthreads werden innerhalb des Kernelprozesses erzeugt.

```c
typedef void (*np_kernel_thread_entry_t)(void* context);
```

Kernelthreads:

- besitzen keinen normalen Userspace-Stack,
- verwenden den Kerneladressraum,
- besitzen einen eingeschränkten Kernel-Capability-Kontext,
- dürfen blockieren und vom Scheduler eingeplant werden,
- müssen ihre Ressourcen kontrolliert freigeben.

Die Einstufung als Kernelthread verleiht nicht automatisch uneingeschränkte Rechte auf alle Kernelobjekte.

## 14. Idle-Threads

Jede aktive CPU besitzt genau einen Idle-Thread.

Idle-Threads werden während der CPU-Initialisierung erstellt und:

- fest an ihre CPU gebunden,
- niemals beendet,
- niemals regulär blockiert,
- nur bei leerer Runqueue ausgeführt,
- für CPU-Idle- und Energiesparzustände verwendet.

Idle-Threads besitzen keine normale Userspace-Schnittstelle.

## 15. Kernel-Stack

Jeder Thread benötigt einen eigenen Kernel-Stack.

Der Kernel-Stack wird verwendet für:

- Systemaufrufe,
- Exceptions,
- Threaded Interrupts,
- Kerneloperationen,
- Context Switching.

Jeder Kernel-Stack muss mindestens eine Guard Page besitzen.

```c
typedef struct np_kernel_stack {
    uintptr_t base;
    uintptr_t top;
    size_t size;
    uint32_t flags;
} np_kernel_stack_t;
```

Kernel-Stacks dürfen nicht in Userspace-Adressräume abgebildet werden.

## 16. Userspace-Stack

Userspace-Threads besitzen einen separaten Userspace-Stack.

Der Stack muss:

- im Prozessadressraum liegen,
- schreibbar und nicht ausführbar sein,
- korrekt ausgerichtet sein,
- durch mindestens eine Guard Page geschützt werden,
- eine definierte maximale Größe besitzen.

Automatisches Stackwachstum darf nur innerhalb vorher reservierter Grenzen erfolgen.

## 17. Stackgrößen

Empfohlene Standardgrößen sind:

| Stack | Standardgröße |
|---|---:|
| früher Boot-Stack | architekturabhängig |
| Kernelthread-Stack | 32 KiB |
| Userspace-Kernel-Stack | 32 KiB |
| initialer Userspace-Stack | 1 MiB reserviert |
| zusätzlicher Userspace-Stack | konfigurierbar |

Die konkreten Werte dürfen je nach Architektur und Sicherheitsprofil abweichen.

## 18. Stack Protector

Jeder reguläre Kernel- und Userspace-Thread erhält einen eigenen Stack Canary.

Der Canary wird bei der Threaderzeugung aus einer geeigneten Zufallsquelle erzeugt und im geschützten Threadkontext gespeichert.

Stack-Canary-Werte dürfen:

- nicht über öffentliche APIs ausgegeben,
- nicht in normale Logs geschrieben,
- nicht ungeschützt an Userspace übergeben werden.

Eine erkannte Canary-Beschädigung wird nach der gültigen Sicherheits- und Panic-Richtlinie behandelt.

## 19. Architekturkontext

Der architekturspezifische Threadkontext enthält mindestens:

- allgemeine Register,
- Instruction Pointer,
- Stack Pointer,
- Statusregister,
- Segment- oder Thread-Pointer-Register,
- Adressraumkontext,
- FPU- und SIMD-Zustand,
- gegebenenfalls Debugregister.

```c
np_status_t np_arch_thread_initialize(
    np_thread_t* thread,
    uintptr_t entry_point,
    uintptr_t stack_pointer,
    uintptr_t argument
);
```

Der initiale Kontext muss so aufgebaut sein, als wäre der Thread über den vorgesehenen Thread-Start-Stub aufgerufen worden.

## 20. Thread-Start-Stub

Neue Threads beginnen nicht direkt am übergebenen Eintrittspunkt.

Sie durchlaufen zunächst einen kontrollierten Start-Stub:

```text
Scheduler startet Thread
  -> Architekturkontext wiederherstellen
  -> Thread-lokale Daten aktivieren
  -> Sicherheitskontext prüfen
  -> Startargument vorbereiten
  -> Eintrittspunkt aufrufen
  -> Rückgabewert erfassen
  -> Threadbeendigung einleiten
```

Kehrt der Eintrittspunkt zurück, wird der Thread ordnungsgemäß beendet.

## 21. FPU- und SIMD-Zustand

Der Thread Manager verwaltet den FPU- und SIMD-Zustand jedes Threads.

Die Implementierung kann:

- den Zustand bei jedem Context Switch sichern,
- Lazy State Switching verwenden,
- erweiterte CPU-Zustände nur bei Bedarf anlegen.

Lazy State Switching darf nur verwendet werden, wenn es auf der jeweiligen Architektur sicher implementierbar ist.

Sicherheitsrelevante Registerinhalte müssen vor der Wiederverwendung gelöscht oder überschrieben werden.

## 22. Thread-lokaler Speicher

Jeder Userspace-Thread kann Thread-Local Storage verwenden.

```c
np_status_t np_thread_set_tls(
    np_thread_handle_t thread,
    uintptr_t tls_base
);
```

Die TLS-Adresse muss:

- im Adressraum des Besitzerprozesses liegen,
- korrekt ausgerichtet sein,
- für die Zielarchitektur gültig sein.

Die TLS-Basis wird beim Context Switch über den architekturspezifischen Threadkontext aktiviert.

## 23. Kernel-lokale Thread-Daten

Kernelinterne Thread-Daten enthalten beispielsweise:

- aktuellen Prozess,
- aktuelle Capability-Domain,
- Systemaufrufstatus,
- Fehlerstatus,
- Präemptionszähler,
- gehaltene Sperren,
- Wait-Kontext,
- Diagnosekennung.

Diese Daten dürfen nicht direkt in den Userspace eingeblendet werden.

## 24. Scheduler-Integration

Der Thread Manager stellt dem Scheduler ausschließlich vollständig initialisierte Threads bereit.

Ein Thread darf nur dann in eine Runqueue aufgenommen werden, wenn:

- sein Zustand `READY` ist,
- ein gültiger Kernel-Stack besteht,
- der Architekturkontext gültig ist,
- eine zulässige CPU-Affinität besteht,
- der Besitzerprozess ausführbar ist.

Scheduler-Klasse und Priorität werden entsprechend NPSPEC-KERNEL-0003 verwaltet.

## 25. CPU-Affinität

Jeder Thread besitzt eine Menge zulässiger CPUs.

```c
np_status_t np_thread_set_affinity(
    np_thread_handle_t thread,
    const np_cpu_set_t* cpu_set
);
```

Eine leere Affinitätsmenge ist unzulässig.

Idle-Threads und bestimmte hardwarenahe Kernelthreads dürfen fest an eine einzelne CPU gebunden sein.

Änderungen der Affinität fremder Threads erfordern eine entsprechende Capability.

## 26. Freiwillige CPU-Freigabe

Der aktuell laufende Thread kann seine verbleibende Zeitscheibe freiwillig abgeben:

```c
void np_thread_yield(void);
```

`yield()` garantiert nicht, dass ein anderer Thread ausgeführt wird. Ist kein anderer Thread bereit, kann der aktuelle Thread unmittelbar erneut ausgewählt werden.

## 27. Blockieren

Ein Thread kann auf ein wartbares Kernelobjekt blockieren.

```c
np_status_t np_thread_block(
    np_wait_object_t* object,
    uint64_t timeout_ns
);
```

Der Übergang von der Objektprüfung in den blockierten Zustand muss atomar erfolgen, damit keine Wakeups verloren gehen.

Ein Thread darf nicht blockieren, während:

- Präemption unzulässig deaktiviert ist,
- eine nicht schlaffähige Spinlock gehalten wird,
- er sich in einem nicht threadbasierten Interruptkontext befindet.

## 28. Warten auf mehrere Objekte

Threads können optional auf mehrere Kernelobjekte warten.

```c
typedef enum np_wait_mode {
    NP_WAIT_ANY,
    NP_WAIT_ALL
} np_wait_mode_t;

np_status_t np_thread_wait_many(
    const np_handle_t* handles,
    size_t handle_count,
    np_wait_mode_t mode,
    uint64_t timeout_ns,
    size_t* signaled_index
);
```

Die maximale Anzahl gleichzeitig wartbarer Objekte ist begrenzt und wird durch die ABI festgelegt.

## 29. Aufwecken

Ein blockierter oder schlafender Thread kann durch folgende Ereignisse aufgeweckt werden:

- Signalisierung eines Kernelobjekts,
- Ablauf eines Timeouts,
- Abbruch der Warteoperation,
- Prozessbeendigung,
- Zustellung eines Signals oder einer Exception,
- Systemabschaltung.

Ein Wakeup überführt den Thread zunächst in den Zustand `READY`. Die sofortige Ausführung ist nicht garantiert.

## 30. Schlafen

```c
np_status_t np_thread_sleep_for(uint64_t duration_ns);
np_status_t np_thread_sleep_until(uint64_t deadline_ns);
```

Schlafzeiten basieren auf der monotonen Kernelzeit.

Eine Schlafoperation kann später als angefordert enden, darf aber nicht regulär vor dem Zielzeitpunkt abgeschlossen werden, sofern sie nicht ausdrücklich unterbrochen wurde.

## 31. Suspendierung

Eine Suspendierung hält einen Thread unabhängig von seinem aktuellen normalen Ausführungszustand an.

Der Thread Manager verwaltet dafür einen Suspendierungszähler:

```c
typedef uint32_t np_suspend_count_t;
```

Jede autorisierte Suspendierungsoperation erhöht den Zähler. Eine Fortsetzung reduziert ihn.

Der Thread wird erst wieder ausführbar, wenn der Zähler null erreicht.

Überläufe und unberechtigte Änderungen müssen verhindert werden.

## 32. Sichere Suspendierung

Ein laufender Thread darf nicht an einem beliebigen unsicheren Punkt gewaltsam eingefroren werden, wenn dadurch Kernelstrukturen dauerhaft gesperrt würden.

Abhängig vom Anwendungsfall erfolgt die Suspendierung:

- sofort im Userspace,
- am nächsten sicheren Kernel-Suspendierungspunkt,
- nach Abschluss eines nicht unterbrechbaren Abschnitts,
- über ein CPU-übergreifendes Reschedule-Protokoll.

Administrative Debugoperationen können gesonderte Regeln verwenden.

## 33. Thread-Abbruch

Lang laufende oder blockierte Operationen können über ein Abbruchereignis unterbrochen werden.

```c
np_status_t np_thread_request_cancel(
    np_thread_handle_t thread,
    uint32_t reason
);
```

Der Abbruch ist standardmäßig kooperativ. Er setzt einen Zustand und weckt abbrechbare Warteoperationen auf.

Asynchrones Beenden an beliebigen Userspace-Instruktionen ist nur für erzwungene Prozessbeendigungen zulässig.

## 34. Thread-Beendigung

Ein Thread kann beendet werden durch:

- Rückkehr aus seinem Eintrittspunkt,
- expliziten Exit-Aufruf,
- unbehandelte Exception,
- Prozessbeendigung,
- autorisierte Fremdbeendigung,
- kritische Sicherheitsverletzung.

```c
_Noreturn void np_thread_exit(
    np_thread_exit_status_t exit_status
);
```

`np_thread_exit()` darf nicht zum aufrufenden Code zurückkehren.

## 35. Beendigungsablauf

Die Threadbeendigung erfolgt in dieser Reihenfolge:

1. Zustand auf `TERMINATING` setzen,
2. Thread aus Warteobjekten entfernen,
3. Scheduler-Ausführung verhindern,
4. robuste Synchronisationsobjekte behandeln,
5. Thread-lokale Ressourcen freigeben,
6. Exit-Status speichern,
7. Besitzerprozess benachrichtigen,
8. wartende Threads aufwecken,
9. Zustand auf `TERMINATED` setzen,
10. Stack und Threadobjekt nach dem Context Switch freigeben.

Ein Thread darf seinen aktuell verwendeten Kernel-Stack nicht selbst freigeben.

## 36. Exit-Status

```c
typedef struct np_thread_exit_status {
    uint32_t reason;
    int32_t code;
    uint64_t detail;
} np_thread_exit_status_t;
```

Mögliche Gründe sind:

```c
typedef enum np_thread_exit_reason {
    NP_THREAD_EXIT_NORMAL,
    NP_THREAD_EXIT_CANCELLED,
    NP_THREAD_EXIT_TERMINATED,
    NP_THREAD_EXIT_EXCEPTION,
    NP_THREAD_EXIT_SECURITY,
    NP_THREAD_EXIT_PROCESS_END
} np_thread_exit_reason_t;
```

## 37. Warten auf Threadende

Threadobjekte sind wartbare Kernelobjekte.

Ein Thread-Handle wird signalisiert, sobald der Zielthread `TERMINATED` erreicht.

Mehrere autorisierte Threads dürfen auf denselben Zielthread warten.

Ein Thread darf nicht unbegrenzt auf sein eigenes Ende warten. Ein solcher Versuch muss als Deadlock oder ungültige Operation abgelehnt werden.

## 38. Letzter Thread eines Prozesses

Endet der letzte aktive Thread eines Userspace-Prozesses, wird die Prozessbeendigung eingeleitet.

Der Process Manager bestimmt dabei den Prozess-Exit-Status aus:

- dem Status des Hauptthreads,
- einer vorherigen Prozessbeendigungsanforderung,
- einer Sicherheitsverletzung,
- einem Job-Object-Ereignis.

Kernel-Idle-Threads sind von dieser Regel ausgenommen.

## 39. Robuste Synchronisationsobjekte

Ein Thread kann während seiner Beendigung Sperren oder andere Synchronisationsobjekte besitzen.

Robuste Objekte müssen ihren Zustand entsprechend kennzeichnen:

```text
OWNER_TERMINATED
```

Wartende Threads werden aufgeweckt und müssen entscheiden, ob der geschützte Zustand wiederhergestellt werden kann.

Nicht robuste Spinlocks dürfen beim Threadende nicht gehalten werden. Ein Verstoß ist ein schwerer Kernelkonsistenzfehler.

## 40. Prioritätsvererbung

Der Thread Manager verwaltet geerbte Prioritäten in Zusammenarbeit mit Scheduler und Synchronisationssystem.

Ein Thread kann mehrere geerbte Prioritäten gleichzeitig besitzen.

Die effektive Priorität ergibt sich aus:

```text
effective_priority =
    max(base_priority, all_inherited_priorities)
```

Nach Freigabe oder Aufgabe eines Synchronisationsobjekts wird die effektive Priorität neu berechnet.

## 41. Thread-Namen

Threads können einen Diagnose- und Anzeigenamen besitzen.

```c
np_status_t np_thread_set_name(
    np_thread_handle_t thread,
    np_string_view_t name
);
```

Der Name:

- ist nicht sicherheitsrelevant,
- muss längenbegrenzt sein,
- darf die Threadkennung nicht ersetzen,
- muss für Logs sicher escaped werden,
- kann durch Datenschutzrichtlinien ausgeblendet werden.

## 42. Thread-Rechte

Mindestens folgende Capability-Rechte werden definiert:

| Capability | Bedeutung |
|---|---|
| `CAP_THREAD_QUERY` | grundlegende Threadinformationen lesen |
| `CAP_THREAD_WAIT` | auf Threadende warten |
| `CAP_THREAD_SUSPEND` | Thread suspendieren oder fortsetzen |
| `CAP_THREAD_TERMINATE` | Thread beenden |
| `CAP_THREAD_CONTEXT_READ` | Registerkontext lesen |
| `CAP_THREAD_CONTEXT_WRITE` | Registerkontext verändern |
| `CAP_THREAD_SCHEDULING` | Priorität und Scheduler-Klasse verändern |
| `CAP_THREAD_AFFINITY` | CPU-Affinität verändern |
| `CAP_THREAD_DEBUG` | erweiterte Debugoperationen ausführen |

Capabilities müssen auf konkrete Threads, Prozesse oder Sicherheitsdomains begrenzbar sein.

## 43. Kontextzugriff

Register eines Threads dürfen nur gelesen oder verändert werden, wenn:

- der Aufrufer die notwendige Capability besitzt,
- der Zielthread sicher angehalten wurde,
- der neue Kontext architekturgültig ist,
- keine privilegierten Userspace-Zustände erzeugt werden.

```c
np_status_t np_thread_get_context(
    np_thread_handle_t thread,
    np_thread_context_t* context
);

np_status_t np_thread_set_context(
    np_thread_handle_t thread,
    const np_thread_context_t* context
);
```

Ein Userspace-Debugger darf keine Kernelregister oder Kerneladressen in einen Userspace-Kontext einsetzen.

## 44. Exception-Integration

Tritt eine Userspace-Exception auf, wird der Thread angehalten und ein strukturiertes Exception-Ereignis erzeugt.

Abhängig von Prozessrichtlinie und Berechtigungen kann:

- ein Exception Handler den Kontext korrigieren,
- ein Debugger benachrichtigt werden,
- der Thread beendet werden,
- der gesamte Prozess beendet werden.

Kernel-Exceptions innerhalb eines Kernelthreads werden gemäß Kernel-Panic-Policy behandelt, wenn keine sichere lokale Wiederherstellung möglich ist.

## 45. Signale und Thread-Ereignisse

Asynchrone Signale werden nicht direkt in beliebigen Kernelabschnitten ausgeführt.

Sie werden:

- als ausstehend markiert,
- an einem sicheren Übergangspunkt geprüft,
- vor der Rückkehr in den Userspace zugestellt,
- bei blockierenden Operationen gegebenenfalls zur Unterbrechung verwendet.

Die Signalzustellung muss den ursprünglichen Threadkontext sicher wiederherstellen können.

## 46. Threaded Interrupts

Zeitaufwendige Interruptverarbeitung kann in einem speziellen Kernelthread erfolgen.

Der direkte Interrupt Handler:

1. bestätigt die Hardwarequelle,
2. erfasst minimale Statusdaten,
3. weckt den zugehörigen Interrupt-Thread,
4. beendet den Hardware-Interrupt.

Der Interrupt-Thread führt anschließend die komplexere Verarbeitung in einem planbaren Kontext aus.

## 47. CPU Hotplug

Vor dem Offline-Schalten einer CPU müssen deren Threads behandelt werden.

Der Thread Manager muss:

- migrierbare Threads auf andere CPUs verschieben,
- gebundene Threads erkennen,
- Affinitätsmasken aktualisieren oder den Vorgang ablehnen,
- den CPU-lokalen Idle-Thread kontrolliert anhalten,
- CPU-lokale Threaddaten sichern oder freigeben.

Ein Thread darf nach dem Offline-Schalten nicht auf der entfernten CPU verbleiben.

## 48. Ressourcenabrechnung

Pro Thread werden mindestens folgende Werte erfasst:

```c
typedef struct np_thread_accounting {
    uint64_t user_time_ns;
    uint64_t kernel_time_ns;
    uint64_t ready_time_ns;
    uint64_t blocked_time_ns;
    uint64_t context_switches;
    uint64_t voluntary_switches;
    uint64_t involuntary_switches;
} np_thread_accounting_t;
```

Die Werte werden auch für die Prozess- und Job-Abrechnung aggregiert.

## 49. Diagnose

Der Thread Manager stellt autorisierten Diagnosediensten bereit:

- Threadkennung,
- Besitzerprozess,
- Zustand,
- aktuelle oder letzte CPU,
- Scheduler-Klasse,
- Basis- und effektive Priorität,
- CPU-Affinität,
- Laufzeitstatistik,
- Wartegrund,
- Stackauslastung,
- Context-Switch-Zähler.

Stackinhalte, Register und Thread-lokale Daten erfordern zusätzliche Debugrechte.

## 50. Fehlercodes

| Fehlercode | Bedeutung |
|---|---|
| `NP_OK` | Operation erfolgreich |
| `NP_ERR_INVALID_ARGUMENT` | ungültiger Parameter |
| `NP_ERR_INVALID_HANDLE` | ungültiges Thread-Handle |
| `NP_ERR_INVALID_STATE` | Operation im aktuellen Zustand unzulässig |
| `NP_ERR_ACCESS_DENIED` | erforderliche Capability fehlt |
| `NP_ERR_THREAD_LIMIT` | Threadlimit erreicht |
| `NP_ERR_STACK_INVALID` | Stack ist ungültig |
| `NP_ERR_ENTRY_INVALID` | Eintrittspunkt ist ungültig |
| `NP_ERR_CPU_UNAVAILABLE` | keine erlaubte CPU verfügbar |
| `NP_ERR_TIMEOUT` | Wartezeit abgelaufen |
| `NP_ERR_INTERRUPTED` | Operation wurde unterbrochen |
| `NP_ERR_THREAD_EXITED` | Zielthread wurde bereits beendet |
| `NP_ERR_DEADLOCK` | Operation würde einen Deadlock verursachen |

## 51. Sicherheitsanforderungen

Der Thread Manager muss sicherstellen, dass:

- Userspace-Threads niemals Kernel-Stacks lesen oder verändern können,
- Eintrittspunkte und Stackadressen validiert werden,
- Threadkontexte keine privilegierten CPU-Zustände erzeugen,
- fremde Threads nur mit passender Capability kontrolliert werden,
- Stack Guard Pages aktiv sind,
- Stack Canaries nicht offengelegt werden,
- Thread-Handles keine direkten Kernelzeiger enthalten,
- beendete Threadkontexte keine sensiblen Daten zurücklassen,
- unprivilegierte Threads keine Realtime-Rechte erschleichen,
- Diagnoseinformationen entsprechend ihrer Sensibilität geschützt sind.

## 52. Fehlerbehandlung

Fehler bei der Threaderzeugung müssen vollständig zurückgerollt werden.

Kann ein Userspace-Thread nicht gestartet werden, wird nur der betroffene Thread oder dessen Prozess beendet.

Folgende Situationen gelten als schwerer Kernelzustand:

- beschädigte Scheduler-Verknüpfung,
- fehlender Kernel-Stack eines laufenden Threads,
- gleichzeitige Ausführung desselben Threads auf mehreren CPUs,
- Freigabe eines noch aktiven Threadkontexts,
- Beendigung eines Threads mit gehaltener nicht robuster Kernel-Spinlock.

## 53. Testanforderungen

Mindestens folgende Tests sind erforderlich:

1. Erzeugung eines Userspace-Threads,
2. Erzeugung eines Kernelthreads,
3. Trennung von Erzeugung und Start,
4. Ablehnung eines ungültigen Eintrittspunkts,
5. Ablehnung einer ungültigen Stackadresse,
6. Schutz durch Stack Guard Pages,
7. Initialisierung eines eigenen Stack Canarys,
8. korrekter erster Threadstart,
9. Rückkehr aus dem Eintrittspunkt,
10. freiwillige CPU-Freigabe,
11. Blockieren und Aufwecken,
12. Warten auf mehrere Objekte,
13. Timeout einer Warteoperation,
14. Schlafen bis zu einem Zeitpunkt,
15. Suspendierung und Fortsetzung,
16. verschachtelte Suspendierungen,
17. kooperativer Thread-Abbruch,
18. erzwungene Prozessbeendigung,
19. Warten auf Threadende,
20. Ablehnung des Wartens auf sich selbst,
21. vollständige Registerwiederherstellung,
22. Erhaltung des FPU- und SIMD-Zustands,
23. TLS-Wechsel zwischen Threads,
24. Einhaltung der CPU-Affinität,
25. Migration auf eine andere CPU,
26. Priority Inheritance,
27. Behandlung eines beendeten Lock-Besitzers,
28. CPU-Hotplug mit aktiven Threads,
29. parallele Threaderzeugung und Prozessbeendigung,
30. Freigabe von Stack und Threadobjekt nach Beendigung.

## 54. Verbindliche Invarianten

1. Jeder Thread gehört während seiner Lebensdauer genau einem Prozess.
2. Ein Thread läuft niemals gleichzeitig auf mehreren CPUs.
3. Jeder aktive Thread besitzt einen gültigen Kernel-Stack.
4. Nur Threads im Zustand `READY` dürfen in eine Runqueue aufgenommen werden.
5. Ein beendeter Thread kann nicht erneut gestartet werden.
6. Ein Thread gibt seinen aktuell verwendeten Kernel-Stack niemals selbst frei.
7. Userspace-Stacks sind nicht ausführbar.
8. Kernel-Stacks sind für Userspace nicht zugänglich.
9. CPU-Affinitäten werden bei Ausführung und Migration eingehalten.
10. Ein Threadkontext darf keine unzulässige Privilegienstufe enthalten.
11. Der letzte Thread eines Userspace-Prozesses leitet dessen Beendigung ein.
12. Threadzustandsänderungen erfolgen atomar und nachvollziehbar.

## 55. Referenzablauf einer Threaderzeugung

```text
Thread-Anforderung
  -> Capability prüfen
  -> Besitzerprozess prüfen
  -> Threadkennung reservieren
  -> Threadobjekt anlegen
  -> Kernel-Stack mit Guard Page erzeugen
  -> Userspace-Stack prüfen oder erzeugen
  -> Stack Canary initialisieren
  -> TLS und Sicherheitskontext vorbereiten
  -> Architekturkontext erzeugen
  -> Scheduler-Parameter anwenden
  -> Thread im Prozess registrieren
  -> Thread im Kernel Object Graph registrieren
  -> Zustand CREATED setzen
  -> Thread starten
  -> Zustand READY setzen
  -> Scheduler benachrichtigen
```

## 56. Zusammenfassung

Der NovaOS Thread Manager verwaltet Threads als capability-geschützte Kernelobjekte und stellt ihren vollständigen Lebenszyklus bereit.

Jeder Thread besitzt einen getrennten Ausführungskontext und Kernel-Stack. Userspace-Threads erhalten zusätzlich einen geschützten Userspace-Stack und Thread-Local Storage. Zustandswechsel, Suspendierungen, Warteoperationen und Beendigungen werden atomar mit Scheduler und Process Manager abgestimmt.

Threadfehler bleiben möglichst auf den betroffenen Thread oder Prozess begrenzt. Verletzungen zentraler Kernelinvarianten werden als schwerwiegende Kernelkonsistenzfehler behandelt.