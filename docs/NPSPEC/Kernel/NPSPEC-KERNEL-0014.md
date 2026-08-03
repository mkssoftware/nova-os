# NPSPEC-KERNEL-0014 – Timer Subsystem

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-KERNEL-0014 |
| Titel | Timer Subsystem |
| Status | Spezifikation |
| Version | 1.0 |
| Datum | 2026-08-03 |
| Bereich | Kernel / Zeit- und Timerverwaltung |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | NPSPEC-KERNEL-0002, NPSPEC-KERNEL-0003, NPSPEC-KERNEL-0005, NPSPEC-KERNEL-0009, NPSPEC-KERNEL-0012 |
| Zugehörige ADRs | ADR-PROC-0003, ADR-PROC-0005, ADR-PROC-0006, ADR-POWER-0001, ADR-DIAG-0004 |

---

## 1. Zweck

Diese Spezifikation definiert das Timer Subsystem des NovaOS-Kernels.

Das Subsystem verwaltet:

- monotone Kernelzeit,
- System- und Echtzeit,
- Hardwarezeitquellen,
- Clockevent-Geräte,
- einmalige und periodische Timer,
- hochauflösende Timer,
- Thread-Schlafoperationen,
- Scheduler-Zeitscheiben,
- Timeouts,
- Timerobjekte für Userspace,
- Zeitkorrektur und Diagnose.

## 2. Ziele

Das Timer Subsystem muss:

- monotone und verlässliche Zeitmessung bereitstellen,
- mehrere Hardwarezeitquellen unterstützen,
- hochauflösende Timer ermöglichen,
- im Tickless-Modus arbeiten können,
- auf Mehrprozessorsystemen skalieren,
- Zeitüberläufe sicher behandeln,
- Suspend und Hibernate berücksichtigen,
- Zeitmanipulationen capability-basiert begrenzen,
- Realtime- und Deadline-Anforderungen unterstützen.

## 3. Abgrenzung

| Komponente | Verantwortung |
|---|---|
| Timer Subsystem | Zeitbasis und Timerverwaltung |
| Scheduler | Zeitscheiben und Threadplanung |
| Interrupt Manager | Timerinterrupts |
| Power Management | Suspend, Resume und CPU Idle |
| RTC-Treiber | batteriegepufferte Hardwareuhr |
| Network Time Service | externe Zeitsynchronisation |
| VFS | Datei-Zeitstempel |
| Userspace Runtime | benutzerfreundliche Zeit-APIs |

## 4. Grundbegriffe

| Begriff | Bedeutung |
|---|---|
| Clocksource | Hardwarequelle zum Messen verstrichener Zeit |
| Clockevent | Hardwaregerät zum Auslösen eines zukünftigen Interrupts |
| Monotone Zeit | niemals rückwärts laufende Zeit seit Systemstart |
| Realtime | zivile Systemzeit bezogen auf eine Epoche |
| Bootzeit | monotone Zeit einschließlich Suspend-Zeiten |
| Deadline | absoluter Zeitpunkt für ein Ereignis |
| Timer Slack | erlaubte Verzögerung zur Ereignisbündelung |
| Resolution | kleinster unterscheidbarer Zeitabstand |
| Precision | tatsächliche Mess- oder Auslösegenauigkeit |

## 5. Zeittypen

```c
typedef uint64_t np_time_ns_t;
typedef int64_t np_time_delta_ns_t;
```

NovaOS verwendet Nanosekunden als allgemeine Kernel- und ABI-Einheit.

Die interne Hardwareumrechnung darf mit einer anderen Auflösung erfolgen.

## 6. Uhrenklassen

```c
typedef enum np_clock_id {
    NP_CLOCK_MONOTONIC,
    NP_CLOCK_BOOTTIME,
    NP_CLOCK_REALTIME,
    NP_CLOCK_MONOTONIC_RAW,
    NP_CLOCK_PROCESS_CPU,
    NP_CLOCK_THREAD_CPU
} np_clock_id_t;
```

| Uhr | Verhalten |
|---|---|
| `MONOTONIC` | läuft nicht rückwärts; kann sanft korrigiert werden |
| `BOOTTIME` | monotone Zeit einschließlich Suspend |
| `REALTIME` | zivile Systemzeit; darf administrativ korrigiert werden |
| `MONOTONIC_RAW` | möglichst direkte Hardwarezeit ohne Frequenzkorrektur |
| `PROCESS_CPU` | CPU-Zeit eines Prozesses |
| `THREAD_CPU` | CPU-Zeit eines Threads |

## 7. Monotone Zeit

`NP_CLOCK_MONOTONIC` muss während des laufenden Systems monoton steigen.

Es gilt:

```text
time(n + 1) >= time(n)
```

Änderungen der Echtzeit oder der RTC dürfen die monotone Uhr nicht rückwärts setzen.

Timeouts und interne Deadlines verwenden standardmäßig monotone Zeit.

## 8. Bootzeit

`NP_CLOCK_BOOTTIME` umfasst auch Zeiträume, in denen das System suspendiert war.

Nach Resume wird die Dauer des Schlafzustands anhand einer geeigneten persistenten Zeitquelle oder Plattforminformation ergänzt.

Fehlt eine ausreichend verlässliche Quelle, muss die Genauigkeit des Ergebnisses entsprechend gekennzeichnet werden.

## 9. Echtzeit

`NP_CLOCK_REALTIME` stellt die zivile Systemzeit seit einer definierten Epoche dar.

NovaOS verwendet intern:

```text
Unix Epoch: 1970-01-01 00:00:00 UTC
```

Zeitzonen, Kalenderdarstellung und Sommerzeit werden im Userspace verarbeitet.

Der Kernel arbeitet intern grundsätzlich mit UTC.

## 10. Clocksource-Abstraktion

```c
typedef struct np_clocksource_ops {
    uint64_t (*read_cycles)(void* context);
    np_status_t (*enable)(void* context);
    void (*disable)(void* context);
} np_clocksource_ops_t;

typedef struct np_clocksource {
    const char* name;
    np_clocksource_ops_t operations;
    void* context;

    uint64_t mask;
    uint64_t frequency_hz;
    uint32_t rating;
    uint32_t flags;
} np_clocksource_t;
```

Eine Clocksource muss frei laufende oder sicher erweiterbare Zähler bereitstellen.

## 11. Clocksource-Eigenschaften

```c
typedef enum np_clocksource_flags {
    NP_CLOCKSOURCE_STABLE       = 1U << 0,
    NP_CLOCKSOURCE_PER_CPU      = 1U << 1,
    NP_CLOCKSOURCE_CONTINUOUS   = 1U << 2,
    NP_CLOCKSOURCE_SUSPEND_SAFE = 1U << 3,
    NP_CLOCKSOURCE_WATCHDOG     = 1U << 4
} np_clocksource_flags_t;
```

Nicht synchronisierte per-CPU-Zähler dürfen nicht ohne Kompensation als globale Clocksource verwendet werden.

## 12. Mögliche x86-Zeitquellen

Die x86-Implementierung kann unter anderem verwenden:

- invariant TSC,
- HPET,
- ACPI Power Management Timer,
- PIT als Rückfalllösung,
- paravirtualisierte Hypervisor-Clocksource.

Die Auswahl erfolgt nach Stabilität, Frequenz, Zugriffskosten und Plattformverhalten.

## 13. Clocksource-Auswahl

Jede Clocksource erhält ein Rating.

Die höchste geeignete stabile Quelle wird als primäre Clocksource gewählt.

Zu prüfen sind:

- Monotonie,
- Frequenzstabilität,
- Synchronisation zwischen CPUs,
- Verhalten bei Energiesparzuständen,
- Virtualisierungsumgebung,
- bekannte Plattformfehler.

Eine instabile Quelle muss deaktiviert oder herabgestuft werden.

## 14. Clocksource-Kalibrierung

Clocksource-Frequenzen werden über Firmwareangaben, CPU-Informationen oder eine Referenzquelle bestimmt.

Die Kalibrierung muss:

- mehrere Messungen verwenden,
- Ausreißer begrenzen,
- Überläufe berücksichtigen,
- das Ergebnis mit einer unabhängigen Quelle vergleichen können.

Kalibrierungsdaten werden lokal diagnostisch erfasst.

## 15. Zyklen-zu-Zeit-Umrechnung

Hardwarezyklen werden ohne unnötige Fließkommaoperationen in Nanosekunden umgerechnet.

```text
nanoseconds = (cycles × multiplier) >> shift
```

```c
typedef struct np_clock_conversion {
    uint64_t multiplier;
    uint32_t shift;
    uint64_t cycle_mask;
} np_clock_conversion_t;
```

Multiplikation und Addition müssen gegen Überlauf abgesichert werden.

## 16. Clocksource-Watchdog

Eine primäre Clocksource kann regelmäßig mit einer unabhängigen Referenzquelle verglichen werden.

Erkannt werden sollen:

- rückwärts laufende Zähler,
- ungewöhnliche Frequenzabweichungen,
- CPU-übergreifende Sprünge,
- Stillstand,
- fehlerhaftes Verhalten nach Resume.

Bei einem Fehler wechselt NovaOS kontrolliert auf eine geeignete Ersatzquelle.

## 17. Clockevent-Abstraktion

```c
typedef struct np_clockevent_ops {
    np_status_t (*set_next_event)(
        void* context,
        uint64_t delta_cycles
    );

    np_status_t (*set_periodic)(
        void* context,
        uint64_t period_cycles
    );

    void (*shutdown)(void* context);
} np_clockevent_ops_t;
```

Clockevent-Geräte erzeugen Interrupts für zukünftige Zeitereignisse.

## 18. Clockevent-Modi

```c
typedef enum np_clockevent_mode {
    NP_CLOCKEVENT_ONESHOT,
    NP_CLOCKEVENT_PERIODIC,
    NP_CLOCKEVENT_SHUTDOWN
} np_clockevent_mode_t;
```

Der One-Shot-Modus wird für Tickless-Betrieb bevorzugt.

Der periodische Modus dient als Rückfalllösung oder für frühe Plattformen.

## 19. Mögliche Clockevent-Geräte

Auf x86 können verwendet werden:

- Local APIC Timer,
- TSC Deadline Timer,
- HPET Comparator,
- PIT als Rückfalllösung,
- paravirtualisierte Timer.

Auf SMP-Systemen wird ein CPU-lokales Clockevent-Gerät bevorzugt.

## 20. Timerobjekt

Timer werden als wartbare Kernelobjekte verwaltet.

```c
typedef struct np_timer {
    np_kernel_object_t object;

    np_clock_id_t clock_id;
    np_time_ns_t deadline;
    np_time_ns_t interval;
    np_time_ns_t slack;

    uint32_t flags;
    uint32_t state;

    np_timer_callback_t callback;
    void* callback_context;
} np_timer_t;
```

Userspace-Timer enthalten keine direkt aufrufbaren Userspace-Funktionszeiger im Kernel.

## 21. Timerzustände

```c
typedef enum np_timer_state {
    NP_TIMER_INACTIVE,
    NP_TIMER_ARMED,
    NP_TIMER_EXPIRING,
    NP_TIMER_SIGNALED,
    NP_TIMER_CANCELLED
} np_timer_state_t;
```

Ein Timer darf jeweils nur in genau einer aktiven Timerqueue enthalten sein.

## 22. Timerarten

```c
typedef enum np_timer_flags {
    NP_TIMER_RELATIVE      = 1U << 0,
    NP_TIMER_ABSOLUTE      = 1U << 1,
    NP_TIMER_PERIODIC      = 1U << 2,
    NP_TIMER_HIGH_RES      = 1U << 3,
    NP_TIMER_COALESCABLE   = 1U << 4,
    NP_TIMER_WAKE_SYSTEM   = 1U << 5,
    NP_TIMER_PINNED_CPU    = 1U << 6,
    NP_TIMER_KERNEL        = 1U << 7
} np_timer_flags_t;
```

Relative und absolute Timerflags dürfen nicht gleichzeitig gesetzt sein.

## 23. Timer-API

```c
np_status_t np_timer_create(
    np_clock_id_t clock_id,
    uint32_t flags,
    np_timer_handle_t* timer
);

np_status_t np_timer_arm(
    np_timer_handle_t timer,
    np_time_ns_t deadline_or_delay,
    np_time_ns_t interval,
    np_time_ns_t slack
);

np_status_t np_timer_cancel(
    np_timer_handle_t timer,
    np_time_ns_t* remaining_time
);

np_status_t np_timer_query(
    np_timer_handle_t timer,
    np_timer_information_t* information
);
```

## 24. Kernel-Timer

Kernelkomponenten können Callback-basierte Timer verwenden.

```c
typedef void (*np_timer_callback_t)(
    np_timer_t* timer,
    void* context
);
```

Der direkte Timerinterrupt führt keine beliebigen langen Callbacks aus.

Callbacks werden abhängig vom Typ über:

- einen kurzen Deferred-Work-Pfad,
- einen Timer-Thread,
- einen zugeordneten Kernelthread

ausgeführt.

## 25. Userspace-Timer

Userspace-Timer signalisieren ein wartbares Timerobjekt oder einen Event-Endpunkt.

Ein Userspace-Timer kann:

- durch `wait()` abgewartet,
- mit einem Event-Port verbunden,
- für asynchrone IPC-Benachrichtigung verwendet,
- kontrolliert abgebrochen werden.

Der Kernel ruft keine Userspace-Adresse direkt aus dem Timerinterrupt auf.

## 26. Einmalige Timer

Ein einmaliger Timer wird nach seinem Ablauf signalisiert und anschließend inaktiv.

```text
INACTIVE
  -> ARMED
  -> EXPIRING
  -> SIGNALED
  -> INACTIVE oder erneut ARMED
```

Die Signalisierung bleibt entsprechend dem Objekttyp sichtbar, bis sie konsumiert oder zurückgesetzt wurde.

## 27. Periodische Timer

Periodische Timer besitzen ein Intervall größer als null.

Die nächste Deadline wird grundsätzlich aus der vorherigen geplanten Deadline berechnet:

```text
next_deadline = previous_deadline + interval
```

Dadurch wird eine fortlaufende Drift durch verspätete Ausführung reduziert.

## 28. Verpasste Perioden

Wenn ein periodischer Timer mehrfach hätte auslösen müssen, kann die API:

- eine einzelne zusammengefasste Signalisierung liefern,
- die Anzahl verpasster Perioden melden,
- begrenzt mehrere Ereignisse zustellen.

Unbegrenztes Nachholen aller verpassten Perioden ist unzulässig.

```c
typedef struct np_timer_expiration {
    uint64_t expiration_count;
    uint64_t overrun_count;
} np_timer_expiration_t;
```

## 29. Hochauflösende Timer

High-Resolution Timer verwenden die beste verfügbare Clocksource und ein geeignetes One-Shot-Clockevent-Gerät.

Die angeforderte Nanosekundenauflösung stellt keine Garantie für nanosekundengenaue Ausführung dar.

Die tatsächliche Genauigkeit hängt ab von:

- Hardwareauflösung,
- Interruptlatenz,
- Schedulerlast,
- Energiezustand,
- Virtualisierung,
- Timerpriorität.

## 30. Timerqueues

Jede CPU besitzt bevorzugt eine eigene Timerqueue.

Die Queue muss effizient unterstützen:

- Einfügen nach Deadline,
- Entfernen,
- Ermitteln der nächsten Deadline,
- Ablauf mehrerer Timer,
- Migration bei CPU-Hotplug.

Geeignete Strukturen sind beispielsweise:

- Min-Heap,
- balancierter Baum,
- hierarchisches Timing Wheel,
- Kombination aus High-Resolution Tree und Timing Wheel.

## 31. CPU-lokale Timer

Ein Timer kann einer bestimmten CPU zugeordnet werden, wenn:

- seine Callbackdaten CPU-lokal sind,
- ein Per-CPU-Subsystem ihn benötigt,
- Realtime-Lokalität erforderlich ist,
- Migration ausdrücklich deaktiviert wurde.

Ein normaler Timer darf zur Lastverteilung oder für CPU-Idle migriert werden.

## 32. Tickless-Betrieb

Im Tickless-Modus programmiert der Kernel das Clockevent-Gerät auf das nächste tatsächlich erforderliche Ereignis.

Mögliche Ereignisse sind:

- Timerdeadline,
- Scheduler-Zeitscheibenende,
- Thread-Wakeup,
- Load-Balancing-Zeitpunkt,
- Realtime-Budgetgrenze,
- Watchdog-Prüfung.

Sind keine zeitnahen Ereignisse vorhanden, kann die CPU länger im Idle-Zustand bleiben.

## 33. Scheduler-Tick

Der Scheduler darf weiterhin einen periodischen Tick anfordern, wenn:

- die Plattform keinen zuverlässigen One-Shot-Modus unterstützt,
- Diagnose oder Profiling dies verlangt,
- ein Kompatibilitätsmodus aktiv ist.

Der Tick ist keine zwingende Voraussetzung für jede Schedulerentscheidung.

## 34. Timer Coalescing

Coalescable Timer dürfen innerhalb ihres Slack-Fensters verschoben werden.

```text
deadline <= actual_expiration <= deadline + slack
```

Dadurch können mehrere Timer gemeinsam ausgelöst und längere CPU-Idle-Zeiten ermöglicht werden.

Realtime- und ausdrücklich exakte Timer verwenden standardmäßig keinen Slack.

## 35. Thread-Schlafoperationen

```c
np_status_t np_thread_sleep_for(
    np_time_ns_t duration
);

np_status_t np_thread_sleep_until(
    np_clock_id_t clock_id,
    np_time_ns_t deadline
);
```

Der Thread wird bis zum Timerablauf in den Zustand `SLEEPING` versetzt.

Das Aufwachen garantiert die erneute Einplanung, nicht die sofortige CPU-Ausführung.

## 36. Timeouts

Kerneloperationen verwenden bevorzugt absolute monotone Deadlines.

Dies verhindert eine ungewollte Verlängerung durch:

- wiederholte Warteoperationen,
- Interrupts,
- Signale,
- Schedulerverzögerungen,
- Änderungen der Echtzeit.

Ein unendlicher Timeout muss durch einen ausdrücklich definierten Wert dargestellt werden.

## 37. Zeitabfrage

```c
np_status_t np_clock_get_time(
    np_clock_id_t clock_id,
    np_time_ns_t* time
);

np_status_t np_clock_get_resolution(
    np_clock_id_t clock_id,
    np_time_ns_t* resolution
);
```

Häufige Zeitabfragen können über eine schreibgeschützte Shared Service Page beschleunigt werden.

Der schnelle Userspace-Pfad muss dieselbe monotone Semantik wie der Kernelpfad liefern.

## 38. Einstellen der Echtzeit

```c
np_status_t np_clock_set_realtime(
    np_time_ns_t realtime
);
```

Das Setzen der Echtzeit erfordert:

```text
CAP_TIME_SET
```

Die monotone Uhr wird dadurch nicht verändert.

Zeitänderungen müssen lokal auditiert werden.

## 39. Sanfte Zeitkorrektur

Kleine Abweichungen der Echtzeit sollen bevorzugt über eine begrenzte Frequenzkorrektur ausgeglichen werden.

```c
np_status_t np_clock_adjust(
    const np_clock_adjustment_t* adjustment
);
```

Die Korrektur darf die monotone Zeit nicht rückwärts laufen lassen.

Große Zeitsprünge benötigen eine ausdrücklich autorisierte Richtlinie.

## 40. RTC-Integration

Während des Bootvorgangs kann die Realtime Clock als Ausgangswert für die Systemzeit verwendet werden.

Beim Herunterfahren oder nach autorisierter Zeitsynchronisation kann die RTC aktualisiert werden.

Die RTC:

- ist keine bevorzugte hochauflösende Clocksource,
- kann ungenau sein,
- kann auf alten Systemen lokale Zeit statt UTC enthalten,
- muss durch Plattformrichtlinien interpretiert werden.

NovaOS bevorzugt eine in UTC geführte RTC.

## 41. Suspend und Resume

Vor Suspend muss das Timer Subsystem:

- Wake-Timer bestimmen,
- geeignete Hardwarequelle programmieren,
- nicht Wake-fähige Clockevents stoppen,
- aktuelle Zeitbasis sichern.

Nach Resume muss es:

1. Schlafdauer bestimmen,
2. Bootzeit aktualisieren,
3. Clocksource erneut validieren,
4. abgelaufene Timer verarbeiten,
5. periodische Overruns bestimmen,
6. CPU-lokale Clockevents wieder aktivieren.

## 42. Wake-Timer

Timer mit `NP_TIMER_WAKE_SYSTEM` dürfen das System aus einem unterstützten Schlafzustand wecken.

Sie benötigen:

- ein Wake-fähiges Timergerät,
- eine entsprechende Capability,
- eine zulässige Power-Richtlinie.

Unprivilegierte Anwendungen dürfen nicht unbegrenzt Wake-Timer erzeugen.

## 43. Hibernate

Während Hibernate läuft die normale Kernelzeit nicht weiter.

Beim Wiederherstellen wird die vergangene Zeit über RTC, Firmware oder eine andere persistente Quelle bestimmt.

Realtime und Bootzeit werden angepasst. Monotone Semantik innerhalb der wiederhergestellten Kernelinstanz muss erhalten bleiben.

Nicht persistierbare Hardwaretimer werden neu programmiert.

## 44. SMP-Synchronisation

Globale Zeitabfragen müssen auf allen CPUs konsistente Ergebnisse liefern.

Bei per-CPU-Zeitquellen muss NovaOS:

- Offsets kalibrieren,
- CPU-Migration berücksichtigen,
- rückwärts laufende Ergebnisse verhindern,
- Drift überwachen.

Ein Thread darf nach einer CPU-Migration keinen kleineren monotonen Zeitwert beobachten.

## 45. CPU-Hotplug

Beim Online-Schalten einer CPU werden:

- lokale Zeitquellen geprüft,
- Clockevent-Geräte initialisiert,
- per-CPU-Timerqueues angelegt,
- Zeitoffsets kalibriert.

Beim Offline-Schalten werden:

- neue Timerzuweisungen verhindert,
- migrierbare Timer verschoben,
- gebundene Timer behandelt oder der Vorgang abgelehnt,
- lokale Clockevents abgeschaltet.

## 46. Realtime-Timer

Realtime- und Deadline-Threads können Timer mit reduzierter Latenz verwenden.

Voraussetzungen sind:

- passende Scheduler-Capability,
- reservierte Laufzeitbudgets,
- unterstützte Clockevent-Hardware,
- begrenzte Interrupt- und Schedulerlatenz.

NovaOS garantiert keine harte Echtzeit allein durch die Verwendung eines High-Resolution Timers.

## 47. Callback-Regeln

Ein direkter Kernel-Timer-Callback darf:

- nicht unbegrenzt laufen,
- keine nicht erlaubten blockierenden Operationen ausführen,
- den Timerzustand nur über definierte APIs ändern,
- keine freigegebenen Kontextdaten verwenden.

Für komplexe Arbeit muss der Callback einen Kernelthread oder eine Work Queue aktivieren.

## 48. Timer-Abbruch

Ein Timerabbruch muss mit gleichzeitigem Ablauf synchronisiert werden.

Mögliche Ergebnisse sind:

- Timer wurde vor Ablauf erfolgreich entfernt,
- Timer läuft bereits ab,
- Timer war bereits inaktiv.

Eine synchrone Abbruchoperation kann warten, bis ein laufender Callback abgeschlossen ist:

```c
np_status_t np_timer_cancel_sync(
    np_timer_handle_t timer
);
```

Sie darf nicht aus demselben Timer-Callback aufgerufen werden, wenn dadurch ein Deadlock entstünde.

## 49. Objektlebenszyklus

Ein Timerobjekt darf erst zerstört werden, wenn:

- es nicht mehr in einer Timerqueue liegt,
- kein Callback läuft,
- keine Eventzustellung aussteht,
- keine Handles oder internen Referenzen bestehen.

Das Schließen des letzten Handles kann den Timer automatisch abbrechen, sofern keine andere starke Referenz existiert.

## 50. Capability-Rechte

Mindestens folgende Rechte werden definiert:

| Capability | Bedeutung |
|---|---|
| `CAP_TIMER_CREATE` | Timerobjekte erzeugen |
| `CAP_TIMER_ARM` | Timer starten oder ändern |
| `CAP_TIMER_CANCEL` | Timer abbrechen |
| `CAP_TIMER_QUERY` | Timerzustand abfragen |
| `CAP_TIMER_HIGH_RES` | hochauflösende Timer verwenden |
| `CAP_TIMER_WAKE` | Wake-Timer erzeugen |
| `CAP_TIME_SET` | Realtime setzen |
| `CAP_TIME_ADJUST` | Zeitfrequenz korrigieren |
| `CAP_TIME_ADMIN` | globale Zeitrichtlinien verwalten |

Rechte können zusätzlich durch Prozess-, Job- und Power-Richtlinien begrenzt werden.

## 51. Sicherheit

Das Timer Subsystem muss sicherstellen, dass:

- unprivilegierte Prozesse die Systemzeit nicht verändern,
- monotone Zeit nicht rückwärts läuft,
- Timer keine direkten Userspace-Funktionszeiger ausführen,
- Handle- und Capability-Prüfungen vor jeder Timeroperation erfolgen,
- Zeitberechnungen auf Integerüberläufe geprüft werden,
- ein Prozess das System nicht durch unbegrenzte Timer überlastet,
- Wake-Timer capability- und richtliniengeschützt sind,
- Kernel-Timerobjekte nicht vor Abschluss ihrer Callbacks freigegeben werden,
- sensible Zeitkorrekturen auditiert werden.

## 52. Datenschutz

Timerdaten können Rückschlüsse auf Benutzeraktivität und Prozessverhalten ermöglichen.

Daher gelten:

- detaillierte Fremdprozess-Timerdaten erfordern Diagnoseberechtigungen,
- Systemstatistiken werden standardmäßig aggregiert,
- Timer-Tracing bleibt lokal und ist standardmäßig begrenzt,
- externe Übertragung erfolgt nur nach ausdrücklicher Konfiguration.

## 53. Diagnose

Das Timer Subsystem stellt mindestens bereit:

- aktive Clocksource,
- verfügbare Clocksources,
- aktive Clockevent-Geräte,
- gemessene Auflösung,
- Anzahl aktiver Timer,
- verspätete Timer,
- maximale Auslöselatenz,
- Clocksource-Wechsel,
- Drift- und Watchdog-Ereignisse,
- Wake-Timer.

## 54. Statistik

```c
typedef struct np_timer_statistics {
    uint64_t created_timers;
    uint64_t active_timers;
    uint64_t expired_timers;
    uint64_t cancelled_timers;

    uint64_t periodic_overruns;
    uint64_t coalesced_timers;
    uint64_t late_expirations;

    uint64_t clocksource_switches;
    uint64_t watchdog_failures;
    uint64_t maximum_lateness_ns;
} np_timer_statistics_t;
```

Statistiken sollen pro CPU, Prozess und System verfügbar sein.

## 55. Fehlercodes

| Fehlercode | Bedeutung |
|---|---|
| `NP_OK` | Operation erfolgreich |
| `NP_ERR_INVALID_ARGUMENT` | ungültiger Parameter |
| `NP_ERR_INVALID_CLOCK` | Uhr wird nicht unterstützt |
| `NP_ERR_INVALID_TIMER` | Timerhandle ist ungültig |
| `NP_ERR_INVALID_STATE` | Operation im aktuellen Zustand unzulässig |
| `NP_ERR_INVALID_DEADLINE` | Deadline ist ungültig |
| `NP_ERR_OVERFLOW` | Zeitberechnung ist übergelaufen |
| `NP_ERR_ACCESS_DENIED` | erforderliche Capability fehlt |
| `NP_ERR_TIMER_RUNNING` | Callback wird bereits ausgeführt |
| `NP_ERR_CPU_UNAVAILABLE` | gebundene CPU ist nicht verfügbar |
| `NP_ERR_RESOURCE_LIMIT` | Timerlimit wurde erreicht |
| `NP_ERR_NOT_SUPPORTED` | angeforderte Funktion wird nicht unterstützt |

## 56. Fehlerbehandlung

Der Ausfall einer optionalen Zeitquelle führt zum Wechsel auf die beste verfügbare Ersatzquelle.

Folgende Situationen gelten als schwere Kernelkonsistenzfehler:

- monotone Zeit läuft rückwärts,
- keine funktionsfähige Kernelzeitquelle ist verfügbar,
- beschädigte Timerqueue,
- derselbe Timer befindet sich mehrfach in Queues,
- Timerobjekt wird während eines laufenden Callbacks freigegeben,
- kritische Scheduler- oder Watchdog-Timer können nicht mehr programmiert werden.

Je nach Auswirkung wird ein Degraded Mode aktiviert oder die Kernel-Panic-Policy angewendet.

## 57. Testanforderungen

Mindestens folgende Tests sind erforderlich:

1. Auswahl einer stabilen Clocksource,
2. Clocksource-Kalibrierung,
3. Zyklen-zu-Nanosekunden-Umrechnung,
4. monotone Zeit auf einer CPU,
5. monotone Zeit nach CPU-Migration,
6. Clocksource-Watchdog,
7. Wechsel auf eine Ersatz-Clocksource,
8. One-Shot-Clockevent,
9. periodisches Clockevent,
10. einmaliger Kernel-Timer,
11. periodischer Kernel-Timer,
12. Userspace-Timerobjekt,
13. relativer Timer,
14. absoluter Timer,
15. Timerabbruch vor Ablauf,
16. synchroner Abbruch während eines Callbacks,
17. verpasste periodische Auslösungen,
18. High-Resolution Timer,
19. Timer Coalescing,
20. Thread-Schlafoperation,
21. Timeout eines Wait-Objekts,
22. Tickless Idle,
23. mehrere Timer mit gleicher Deadline,
24. parallele Timer auf mehreren CPUs,
25. CPU-Affinität eines Timers,
26. Timer-Migration bei CPU-Hotplug,
27. Realtime-Änderung ohne Beeinflussung der monotonen Zeit,
28. sanfte Zeitkorrektur,
29. RTC-Initialisierung,
30. Suspend und Resume,
31. abgelaufene Timer nach Resume,
32. Wake-Timer,
33. Hibernate-Wiederherstellung,
34. Timerlimit eines Prozesses,
35. Capability-Prüfung administrativer Zeitoperationen,
36. Überlauferkennung großer Zeitwerte,
37. korrekte Diagnose und Statistik.

## 58. Verbindliche Invarianten

1. Die monotone Zeit läuft niemals rückwärts.
2. Jeder aktive Timer befindet sich in höchstens einer Timerqueue.
3. Ein Timer wird erst nach vollständiger Initialisierung aktiviert.
4. Timerinterrupts führen keine beliebigen Userspace-Funktionszeiger aus.
5. Ein Timerobjekt wird nicht während eines aktiven Callbacks zerstört.
6. Absolute Kernel-Timeouts verwenden standardmäßig monotone Zeit.
7. Änderungen der Echtzeit verändern nicht die monotone Uhr.
8. Periodische Timer berechnen ihre Folgetermine aus der geplanten Deadline.
9. CPU-lokale Timer werden vor dem Offline-Schalten ihrer CPU behandelt.
10. Zeit- und Größenberechnungen werden auf Überlauf geprüft.
11. Wake-Timer benötigen eine passende Capability und Power-Richtlinie.
12. Jede Clocksource wird vor ihrer produktiven Verwendung validiert.

## 59. Referenzablauf eines Timerereignisses

```text
Timer wird aktiviert
  -> Zeitparameter und Capability prüfen
  -> absolute Deadline berechnen
  -> Ziel-CPU bestimmen
  -> Timer in CPU-lokale Queue einfügen
  -> nächste Clockevent-Deadline neu programmieren
  -> Hardware löst Timerinterrupt aus
  -> abgelaufene Timer aus Queue entfernen
  -> Timerzustand auf EXPIRING setzen
  -> Event, Thread oder Callback aktivieren
  -> periodischen Timer neu einplanen
  -> nächste Hardwaredeadline programmieren
```

## 60. Zusammenfassung

Das NovaOS Timer Subsystem trennt Zeitmessung durch Clocksources von der Ereignisauslösung durch Clockevent-Geräte.

Monotone Zeit, Realtime, Bootzeit und CPU-Zeit werden über klar definierte Uhren bereitgestellt. CPU-lokale Timerqueues und One-Shot-Clockevents ermöglichen hochauflösende Timer und einen energieeffizienten Tickless-Betrieb.

Capability-geschützte Zeitänderungen, Timerlimits, sichere Callback-Regeln und Clocksource-Watchdogs sorgen dafür, dass Zeitfehler oder missbräuchliche Timer das System nicht unkontrolliert beeinträchtigen.