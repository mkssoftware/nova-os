# NPSPEC-KERNEL-0003 – Scheduler

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-KERNEL-0003 |
| Titel | Scheduler |
| Status | Spezifikation |
| Version | 1.0 |
| Datum | 2026-08-03 |
| Bereich | Kernel / Prozessverwaltung |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | NPSPEC-KERNEL-0001, NPSPEC-KERNEL-0002 |
| Zugehörige ADRs | ADR-PROC-0002, ADR-PROC-0003, ADR-PROC-0004, ADR-PROC-0005, ADR-PROC-0006, ADR-PROC-0007, ADR-PROC-0008 |

---

## 1. Zweck

Diese Spezifikation definiert den Scheduler des NovaOS-Kernels.

Der Scheduler entscheidet, welcher ausführbare Thread auf welcher CPU und für welchen Zeitraum ausgeführt wird. Er muss Desktop-Interaktivität, Hintergrundaufgaben, Systemdienste und kontrollierte Echtzeitanforderungen gemeinsam unterstützen.

## 2. Ziele

Der NovaOS-Scheduler muss:

- interaktive Anwendungen bevorzugt reaktionsfähig halten,
- faire CPU-Zeitverteilung ermöglichen,
- mehrere Scheduler-Klassen unterstützen,
- kontrollierte Echtzeitverarbeitung bereitstellen,
- Mehrprozessorsysteme effizient nutzen,
- CPU-Affinitäten berücksichtigen,
- Energieverbrauch und Cache-Lokalität einbeziehen,
- Prioritätsinversion begrenzen,
- reproduzierbare Diagnoseinformationen liefern.

## 3. Nichtziele

Der Scheduler garantiert keine harte Echtzeitfähigkeit für das gesamte System.

Harte Echtzeitgarantien sind nur innerhalb ausdrücklich unterstützter Plattform-, Treiber- und Sicherheitsprofile zulässig.

Der Scheduler ersetzt außerdem keine Ressourcenbegrenzung für Speicher, Gerätezugriffe oder I/O-Bandbreite.

## 4. Grundmodell

NovaOS plant Threads und nicht Prozesse ein.

Ein Prozess stellt Ressourcen, Adressraum und Sicherheitskontext bereit. Jeder ausführbare Prozess besitzt mindestens einen Thread.

```c
typedef uint64_t np_thread_id_t;
typedef uint64_t np_process_id_t;
typedef uint32_t np_cpu_id_t;
```

Jeder Thread befindet sich zu jedem Zeitpunkt in genau einem definierten Scheduler-Zustand.

## 5. Thread-Zustände

```c
typedef enum np_thread_state {
    NP_THREAD_NEW,
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
| `NEW` | Thread wurde erzeugt, aber noch nicht freigegeben |
| `READY` | Thread kann eingeplant werden |
| `RUNNING` | Thread wird auf einer CPU ausgeführt |
| `BLOCKED` | Thread wartet auf ein Kernelobjekt oder Ereignis |
| `SLEEPING` | Thread wartet bis zu einem Zeitwert |
| `SUSPENDED` | Thread wurde ausdrücklich angehalten |
| `TERMINATING` | Thread wird beendet |
| `TERMINATED` | Thread ist vollständig beendet |

Ein Thread darf nur auf einer CPU gleichzeitig den Zustand `RUNNING` besitzen.

## 6. Scheduler-Klassen

NovaOS definiert folgende Scheduler-Klassen:

```c
typedef enum np_scheduler_class {
    NP_SCHED_IDLE,
    NP_SCHED_BACKGROUND,
    NP_SCHED_NORMAL,
    NP_SCHED_INTERACTIVE,
    NP_SCHED_REALTIME,
    NP_SCHED_DEADLINE
} np_scheduler_class_t;
```

| Klasse | Verwendung |
|---|---|
| `IDLE` | Aufgaben ausschließlich bei sonst ungenutzter CPU |
| `BACKGROUND` | Wartung, Indexierung und nicht dringende Aufgaben |
| `NORMAL` | reguläre Anwendungen und Systemdienste |
| `INTERACTIVE` | Benutzeroberfläche, Eingabe und latenzkritische Dienste |
| `REALTIME` | privilegierte Echtzeit-Threads |
| `DEADLINE` | zeitgebundene Aufgaben mit Laufzeit- und Periodenangabe |

Die Klasse `DEADLINE` kann in frühen Kernelversionen deaktiviert sein. Die ABI muss sie dennoch reservieren.

## 7. Prioritätsmodell

Jede Scheduler-Klasse besitzt einen eigenen Prioritätsbereich.

| Klasse | Prioritätsbereich |
|---|---:|
| Idle | 0 |
| Background | 1–15 |
| Normal | 16–47 |
| Interactive | 48–63 |
| Realtime | 64–95 |
| Deadline | intern berechnet |

Eine höhere Zahl entspricht einer höheren statischen Priorität.

```c
typedef uint8_t np_priority_t;
```

Die effektive Priorität kann zeitweise von der statischen Basispriorität abweichen.

## 8. Thread-Scheduler-Daten

```c
typedef struct np_sched_entity {
    np_scheduler_class_t sched_class;
    np_priority_t base_priority;
    np_priority_t effective_priority;

    np_thread_state_t state;
    np_cpu_id_t current_cpu;
    uint64_t affinity_mask;

    uint64_t runtime_ns;
    uint64_t virtual_runtime;
    uint64_t time_slice_ns;
    uint64_t wakeup_timestamp;

    uint32_t flags;
} np_sched_entity_t;
```

Architekturabhängige Kontextinformationen sind nicht Bestandteil dieser generischen Struktur.

## 9. Runqueues

Jede CPU besitzt eine eigene Runqueue.

```c
typedef struct np_runqueue {
    np_cpu_id_t cpu_id;
    uint32_t runnable_count;
    uint64_t total_weight;
    np_thread_t* current;
    np_thread_t* idle;
    np_spinlock_t lock;
} np_runqueue_t;
```

Per-CPU-Runqueues reduzieren Sperrkonflikte und verbessern die Cache-Lokalität.

Globale Runqueues sind für den normalen Betrieb nicht vorgesehen.

## 10. Auswahl des nächsten Threads

Die Auswahl erfolgt in dieser Reihenfolge:

1. zulässige Deadline-Threads,
2. ausführbare Realtime-Threads,
3. Interactive-Threads,
4. Normal-Threads,
5. Background-Threads,
6. Idle-Thread.

Innerhalb einer Klasse wird das für die jeweilige Klasse definierte Auswahlverfahren verwendet.

Ein Thread darf nur gewählt werden, wenn:

- sein Zustand `READY` ist,
- die CPU durch seine Affinitätsmaske erlaubt ist,
- seine Capability-Domain ausführbar ist,
- keine aktive Suspendierung vorliegt,
- erforderliche Laufzeitbudgets verfügbar sind.

## 11. Fair Scheduling

Die Klassen `NORMAL` und `BACKGROUND` verwenden gewichtete faire Planung.

Jeder Thread besitzt eine virtuelle Laufzeit:

```text
virtual_runtime += executed_time × normalization_factor / thread_weight
```

Threads mit kleinerer virtueller Laufzeit werden bevorzugt ausgewählt.

Das Gewicht wird aus der Priorität innerhalb der Klasse abgeleitet. Höhere Priorität erhöht den CPU-Anteil, darf andere Threads derselben Klasse jedoch nicht unbegrenzt verdrängen.

## 12. Interaktive Threads

Interaktive Threads sollen kurze Reaktionszeiten erhalten.

Als interaktiv können Threads gelten, die:

- regelmäßig auf Benutzereingaben warten,
- kurze CPU-Phasen besitzen,
- häufig auf Ereignisse oder IPC antworten,
- Bestandteil der aktiven Benutzeroberfläche sind.

Die Einstufung darf nicht allein durch einen Prozess selbst erfolgen. Sie benötigt eine passende Capability oder eine vertrauenswürdige Systemrichtlinie.

Dauerhafte CPU-Auslastung kann zur Rückstufung eines interaktiven Threads führen.

## 13. Zeitscheiben

Zeitscheiben werden abhängig von Klasse, Priorität und Systemlast bestimmt.

Empfohlene Ausgangswerte:

| Klasse | Typische Zeitscheibe |
|---|---:|
| Background | 8–20 ms |
| Normal | 4–12 ms |
| Interactive | 2–6 ms |
| Realtime Round Robin | 1–10 ms |
| Idle | bis zur Verdrängung |

Diese Werte sind Richtwerte und keine ABI-Konstanten.

Threads dürfen ihre verbleibende Zeitscheibe freiwillig abgeben.

```c
void np_thread_yield(void);
```

## 14. Präemption

Der NovaOS-Kernel unterstützt Kernel-Präemption.

Ein laufender Thread kann verdrängt werden, wenn:

- ein höher priorisierter Thread ausführbar wird,
- seine Zeitscheibe endet,
- sein Laufzeitbudget erschöpft ist,
- eine CPU-Ausgleichsentscheidung getroffen wird,
- eine Kernelrichtlinie eine Neuplanung verlangt.

Präemption darf innerhalb explizit markierter kritischer Abschnitte vorübergehend deaktiviert werden.

```c
void np_preempt_disable(void);
void np_preempt_enable(void);
```

Präemptionssperren müssen kurz bleiben und dürfen nicht als allgemeiner Synchronisationsmechanismus verwendet werden.

## 15. Scheduler-Tick

Der Scheduler kann mit periodischem Tick oder im Tickless-Modus betrieben werden.

Der Tick dient unter anderem:

- der Laufzeiterfassung,
- dem Ablauf von Zeitscheiben,
- der Budgetkontrolle,
- dem Aufwecken schlafender Threads,
- der Auslösung von Load Balancing.

Bei nur einem laufenden Thread soll die nächste Unterbrechung möglichst auf das tatsächlich notwendige Ereignis programmiert werden.

## 16. Context Switching

Ein Context Switch speichert den Zustand des aktuellen Threads und stellt den Zustand des nächsten Threads wieder her.

Zu sichern sind abhängig von der Architektur:

- allgemeine Register,
- Stack Pointer,
- Instruction Pointer,
- Statusregister,
- SIMD- und FPU-Zustand,
- Thread-lokale Register,
- Debugregister, sofern autorisiert,
- Adressraumreferenz,
- Sicherheits- und Capability-Kontext.

```c
void np_arch_context_switch(
    np_thread_t* previous,
    np_thread_t* next
);
```

Der Low-Level-Wechsel muss architekturspezifisch implementiert werden.

## 17. Adressraumwechsel

Beim Wechsel zwischen Threads unterschiedlicher Prozesse wird der aktive virtuelle Adressraum gewechselt.

Der Scheduler soll unnötige Adressraumwechsel vermeiden, darf dafür aber keine Fairness- oder Sicherheitsgarantien verletzen.

Wenn die Architektur Address Space Identifiers unterstützt, sollen diese zur Reduzierung von TLB-Invalidierungen verwendet werden.

## 18. Blockieren und Aufwecken

Threads dürfen nicht aktiv warten, wenn ein Ereignis über ein Kernelobjekt abgewartet werden kann.

```c
np_status_t np_thread_block(
    np_wait_object_t* object,
    uint64_t timeout_ns
);

np_status_t np_thread_wake(
    np_thread_t* thread
);
```

Beim Aufwecken wird eine geeignete CPU unter Berücksichtigung von Affinität, Last und Cache-Lokalität gewählt.

Verlorene Wakeups müssen durch atomare Zustandsübergänge verhindert werden.

## 19. Schlafende Threads

Ein Thread kann bis zu einem absoluten oder relativen Zeitpunkt schlafen.

```c
np_status_t np_thread_sleep_until(uint64_t deadline_ns);
np_status_t np_thread_sleep_for(uint64_t duration_ns);
```

Schlafende Threads werden in einer zeitgeordneten Struktur verwaltet.

Das Aufwachen garantiert die erneute Einplanung, nicht die sofortige Ausführung zum exakten Zeitpunkt.

## 20. CPU-Affinität

Jeder Thread besitzt eine Menge zulässiger CPUs.

```c
np_status_t np_thread_set_affinity(
    np_thread_handle_t thread,
    const np_cpu_set_t* cpu_set
);
```

Die Affinitätsmenge darf nicht leer sein.

Der Scheduler kann einen Thread innerhalb der erlaubten Menge migrieren. Eine feste Bindung an genau eine CPU ist nur für Treiber, Echtzeitanwendungen oder Diagnosezwecke vorgesehen.

Änderungen fremder Thread-Affinitäten erfordern eine passende Capability.

## 21. CPU-Migration

Ein Thread kann zwischen CPUs migriert werden, wenn:

- seine Affinität dies erlaubt,
- eine deutliche Lastdifferenz besteht,
- eine CPU offline geschaltet wird,
- Energie- oder Thermalrichtlinien dies verlangen,
- die Ziel-CPU eine bessere Platzierung ermöglicht.

Laufende Threads werden nur über ein kontrolliertes Reschedule- oder Stop-Protokoll migriert.

## 22. Load Balancing

Load Balancing erfolgt:

- beim Aufwecken eines Threads,
- periodisch bei hoher Last,
- beim Leerlauf einer CPU,
- beim Online- oder Offline-Schalten einer CPU,
- bei Änderungen von Affinitäten,
- bei Thermal- oder Energieereignissen.

Die Entscheidung berücksichtigt:

- Anzahl ausführbarer Threads,
- gewichtete Last,
- Scheduler-Klassen,
- CPU-Kapazität,
- NUMA-Lokalität,
- Cache-Topologie,
- Energiezustand,
- Thread-Affinität.

Kleine Lastunterschiede sollen keine unnötigen Migrationen auslösen.

## 23. Heterogene Prozessoren

Auf Systemen mit unterschiedlichen CPU-Leistungsklassen muss der Scheduler die Kapazität jeder CPU berücksichtigen.

Leistungsstarke Kerne werden bevorzugt für:

- interaktive Threads,
- rechenintensive Vordergrundaufgaben,
- zeitkritische Systemdienste

verwendet.

Energieeffiziente Kerne werden bevorzugt für:

- Hintergrundaufgaben,
- Wartungsdienste,
- langfristige niedrige Lasten

verwendet.

Eine feste Zuordnung ist nicht vorgeschrieben. Thermal- und Energiezustände können die Entscheidung verändern.

## 24. NUMA-Unterstützung

Auf NUMA-Systemen soll ein Thread bevorzugt auf einer CPU ausgeführt werden, die nahe an seinem hauptsächlich verwendeten Speicher liegt.

Der Scheduler berücksichtigt:

- Speicherzuordnung des Prozesses,
- bisherige CPU-Nutzung,
- Gerätezuständigkeit,
- Kosten einer Migration,
- aktuelle Knotenauslastung.

CPU- und Speichermigration müssen über getrennte Richtlinien steuerbar bleiben.

## 25. Realtime Scheduling

Die Klasse `REALTIME` unterstützt mindestens:

- Fixed Priority FIFO,
- Fixed Priority Round Robin.

```c
typedef enum np_realtime_policy {
    NP_RT_FIFO,
    NP_RT_ROUND_ROBIN
} np_realtime_policy_t;
```

Realtime-Threads verdrängen normale und interaktive Threads.

Ihre Erzeugung oder Hochstufung erfordert eine spezielle Capability:

```text
CAP_SCHED_REALTIME
```

## 26. Realtime-Begrenzung

Realtime-Threads dürfen das System nicht unbegrenzt blockieren.

NovaOS verwendet dafür:

- Laufzeitbudgets,
- maximale ununterbrochene Ausführungszeiten,
- Watchdog-Überwachung,
- reservierte Kernel- und Recovery-Zeit,
- Einschränkungen für unprivilegierte Prozesse.

Bei Budgetüberschreitung kann ein Realtime-Thread:

- vorübergehend blockiert,
- in eine niedrigere Klasse verschoben,
- diagnostiziert,
- bei wiederholtem Verstoß beendet werden.

## 27. Deadline Scheduling

Deadline-Threads deklarieren:

- Laufzeitbudget,
- relative Deadline,
- Periode.

```c
typedef struct np_deadline_parameters {
    uint64_t runtime_ns;
    uint64_t deadline_ns;
    uint64_t period_ns;
} np_deadline_parameters_t;
```

Es muss gelten:

```text
runtime <= deadline <= period
```

Neue Deadline-Anforderungen müssen eine Zulassungsprüfung durchlaufen. Eine Zusage darf nur erfolgen, wenn die verfügbare CPU-Kapazität ausreicht.

## 28. Prioritätsinversion

NovaOS unterstützt Priority Inheritance für geeignete Synchronisationsobjekte.

Blockiert ein hoch priorisierter Thread an einer Sperre, kann der Besitzer vorübergehend seine effektive Priorität erben.

```text
effective_priority =
    max(base_priority, inherited_priorities)
```

Nach Freigabe der Ressource wird die geerbte Priorität neu berechnet.

Verschachtelte Vererbungen müssen über mehrere Sperren hinweg propagiert werden können.

## 29. Prioritätsänderungen

Ein Thread kann seine Basispriorität innerhalb der zulässigen Grenzen ändern.

```c
np_status_t np_thread_set_priority(
    np_thread_handle_t thread,
    np_priority_t priority
);
```

Erhöhungen über die normalen Benutzergrenzen hinaus erfordern eine Capability.

Eine Änderung muss die Position des Threads in seiner Runqueue unmittelbar aktualisieren.

## 30. Job Objects und Prozessgruppen

Der Scheduler berücksichtigt Richtlinien von Job Objects und Prozessgruppen.

Mögliche Gruppenrichtlinien sind:

- CPU-Zeitlimit,
- maximaler CPU-Anteil,
- zulässige CPUs,
- maximale Realtime-Priorität,
- Hintergrundmodus,
- bevorzugte NUMA-Knoten,
- Energieprofil.

Eine Thread-Priorität darf die Obergrenze seines Job Objects nicht überschreiten.

## 31. CPU-Bandbreitenkontrolle

Für Prozesse oder Jobs kann ein CPU-Budget definiert werden.

```c
typedef struct np_cpu_budget {
    uint64_t runtime_ns;
    uint64_t period_ns;
} np_cpu_budget_t;
```

Ist das Budget erschöpft, werden zugehörige Threads bis zur nächsten Periode gedrosselt.

Kritische Kernelthreads dürfen nicht versehentlich durch Userspace-Budgets blockiert werden.

## 32. Idle-Thread

Jede aktive CPU besitzt genau einen Idle-Thread.

Der Idle-Thread:

- wird nur ausgeführt, wenn kein anderer Thread bereit ist,
- besitzt die niedrigste Scheduler-Priorität,
- kann Load Balancing anfordern,
- meldet den Leerlaufzustand an das Power Management,
- aktiviert einen geeigneten CPU-Idle-Zustand.

Der Idle-Thread darf niemals blockieren oder beendet werden.

## 33. Kernelthreads

Kernelthreads werden wie andere Threads eingeplant, besitzen jedoch einen Kerneladressraum und einen definierten Capability-Kontext.

Kritische Kernelthreads können erhöhte Prioritäten erhalten. Eine pauschale Realtime-Einstufung aller Kernelthreads ist unzulässig.

Beispiele für Kernelthreads sind:

- Event-Verarbeitung,
- Deferred Work,
- Speicherbereinigung,
- Treiber-Hilfsdienste,
- Dateisystem-Flush,
- Load Balancing.

## 34. Interrupt-Kontext

Interrupt Handler sind keine regulären Threads und werden nicht direkt vom Scheduler eingeplant.

Zeitaufwendige Arbeit muss in einen der folgenden Kontexte verschoben werden:

- Deferred-Work-Queue,
- Kernelthread,
- Threaded Interrupt Handler,
- Userspace-Treiberereignis.

Nach einem Interrupt kann eine Neuplanung angefordert werden.

## 35. Scheduler-Sperren

Runqueues und Scheduler-Daten werden durch kurze CPU-lokale oder atomare Synchronisationsmechanismen geschützt.

Für mehrere Runqueues gilt eine eindeutige Sperrreihenfolge, beispielsweise nach aufsteigender CPU-Kennung.

Während eines Context Switch dürfen keine Sperren gehalten werden, die durch den nächsten Thread erneut benötigt werden könnten.

## 36. CPU Hotplug

Beim Offline-Schalten einer CPU muss der Scheduler:

1. neue Zuweisungen an diese CPU verhindern,
2. migrierbare Threads verschieben,
3. gebundene Threads behandeln oder den Vorgang ablehnen,
4. Timer und Interruptzuständigkeiten verlagern,
5. den Idle-Thread der CPU kontrolliert anhalten.

Die letzte funktionsfähige CPU darf im normalen Betrieb nicht offline geschaltet werden.

## 37. Energieverwaltung

Der Scheduler arbeitet mit dem Power Management zusammen.

Er kann:

- Arbeit auf weniger CPUs konsolidieren,
- nicht benötigte CPUs in tiefe Schlafzustände versetzen,
- Hintergrundarbeit zeitlich bündeln,
- energieeffiziente CPUs bevorzugen,
- Latenzanforderungen interaktiver Threads berücksichtigen.

Energieeinsparungen dürfen keine zugesagten Realtime- oder Deadline-Garantien verletzen.

## 38. Thermal Management

Bei thermischer Begrenzung kann die verfügbare CPU-Kapazität reduziert werden.

Der Scheduler muss darauf reagieren, indem er:

- Last neu verteilt,
- Hintergrundarbeit drosselt,
- Deadline-Zusagen neu bewertet,
- Realtime-Verstöße meldet,
- kritische Systemdienste bevorzugt funktionsfähig hält.

Thermische Notabschaltungen liegen außerhalb der normalen Schedulerentscheidung und haben Vorrang.

## 39. Capability-Rechte

Mindestens folgende Rechte werden definiert:

| Capability | Bedeutung |
|---|---|
| `CAP_SCHED_QUERY` | Schedulerinformationen abfragen |
| `CAP_SCHED_MODIFY_SELF` | eigene Parameter ändern |
| `CAP_SCHED_MODIFY_OTHER` | fremde Threads verändern |
| `CAP_SCHED_AFFINITY` | CPU-Affinitäten festlegen |
| `CAP_SCHED_REALTIME` | Realtime-Klasse verwenden |
| `CAP_SCHED_DEADLINE` | Deadline-Anforderungen stellen |
| `CAP_SCHED_ADMIN` | globale Richtlinien verwalten |
| `CAP_SCHED_TRACE` | detaillierte Scheduler-Traces lesen |

Capabilities müssen auf konkrete Objekte oder Domains beschränkbar sein.

## 40. Scheduler-API

Die öffentliche Kernel-Service-Schnittstelle stellt mindestens folgende Operationen bereit:

```c
np_status_t np_sched_query(
    np_thread_handle_t thread,
    np_sched_info_t* info
);

np_status_t np_sched_configure(
    np_thread_handle_t thread,
    const np_sched_config_t* config
);

np_status_t np_sched_yield(void);

np_status_t np_sched_set_affinity(
    np_thread_handle_t thread,
    const np_cpu_set_t* cpu_set
);
```

Alle Userspace-Zeiger müssen vor ihrer Verwendung validiert werden.

## 41. Fehlercodes

| Fehlercode | Bedeutung |
|---|---|
| `NP_OK` | Operation erfolgreich |
| `NP_ERR_INVALID_ARGUMENT` | ungültige Parameter |
| `NP_ERR_INVALID_STATE` | unzulässiger Thread-Zustand |
| `NP_ERR_ACCESS_DENIED` | Capability fehlt |
| `NP_ERR_CPU_UNAVAILABLE` | angeforderte CPU nicht verfügbar |
| `NP_ERR_AFFINITY_EMPTY` | leere Affinitätsmenge |
| `NP_ERR_BUDGET_EXCEEDED` | Laufzeitbudget überschritten |
| `NP_ERR_ADMISSION_DENIED` | Deadline-Anforderung abgelehnt |
| `NP_ERR_NOT_SUPPORTED` | Funktion nicht unterstützt |

## 42. Diagnose und Tracing

Der Scheduler stellt lokale Diagnoseinformationen bereit:

- Context Switches,
- Wakeup-Latenzen,
- Runqueue-Längen,
- CPU-Auslastung,
- Thread-Migrationen,
- Prioritätsänderungen,
- Budgetüberschreitungen,
- Realtime- und Deadline-Verstöße,
- Idle-Zeiten.

Tracing muss filterbar und standardmäßig so begrenzt sein, dass keine unnötigen personenbezogenen oder anwendungsbezogenen Daten aufgezeichnet werden.

## 43. Performance Counter

Mindestens folgende Zähler werden pro CPU und systemweit geführt:

```c
typedef struct np_sched_counters {
    uint64_t context_switches;
    uint64_t preemptions;
    uint64_t voluntary_yields;
    uint64_t migrations;
    uint64_t wakeups;
    uint64_t idle_time_ns;
    uint64_t runnable_time_ns;
} np_sched_counters_t;
```

Das Lesen detaillierter Prozessdaten erfordert entsprechende Diagnoseberechtigungen.

## 44. Deterministischer Testmodus

Für Kerneltests kann ein deterministischer Scheduler-Modus bereitgestellt werden.

Dieser Modus darf:

- feste Zeitscheiben,
- kontrollierte Wakeup-Reihenfolgen,
- deaktiviertes automatisches Load Balancing,
- reproduzierbare Zufallsentscheidungen

verwenden.

Er ist nicht für den produktiven Normalbetrieb vorgesehen.

## 45. Fehlerbehandlung

Kann der Scheduler keinen gültigen nächsten Thread bestimmen, muss der Idle-Thread der CPU gewählt werden.

Fehlt ein gültiger Idle-Thread oder ist die Runqueue strukturell beschädigt, liegt ein fataler Kernelzustand vor.

Nicht kritische Abrechnungs- oder Diagnosefehler dürfen den Scheduler-Kern nicht blockieren.

## 46. Sicherheitsanforderungen

Der Scheduler muss verhindern, dass unprivilegierte Prozesse:

- Realtime-Prioritäten ohne Capability erhalten,
- CPU-Zeitlimits umgehen,
- Threads außerhalb ihrer Domain verändern,
- Kernelthreads manipulieren,
- andere Prozesse durch dauerhafte CPU-Belegung verhungern lassen,
- sensible Scheduler-Traces auslesen,
- Offline-CPUs als Affinitätsziel erzwingen.

Alle Parameter müssen auf zulässige Wertebereiche geprüft werden.

## 47. Testanforderungen

Mindestens folgende Tests sind erforderlich:

1. Auswahl des höchstpriorisierten ausführbaren Threads,
2. faire Verteilung zwischen gleich priorisierten Threads,
3. Verdrängung durch einen höher priorisierten Thread,
4. Ablauf einer Zeitscheibe,
5. freiwilliges Abgeben der CPU,
6. Blockieren und Aufwecken ohne verlorene Wakeups,
7. Context Switch zwischen unterschiedlichen Prozessen,
8. Erhaltung des Register- und FPU-Zustands,
9. Einhaltung von CPU-Affinitäten,
10. Migration zwischen CPUs,
11. Load Balancing unter hoher Last,
12. Verhalten auf heterogenen CPUs,
13. Priority Inheritance,
14. verschachtelte Prioritätsvererbung,
15. Realtime-Budgetüberschreitung,
16. Deadline-Zulassungsprüfung,
17. CPU-Budget eines Job Objects,
18. Aktivierung des Idle-Threads,
19. CPU Hotplug,
20. Betrieb im Tickless-Modus,
21. fehlende Scheduler-Capability,
22. gleichzeitige Scheduleroperationen auf mehreren CPUs,
23. Thermal- und Power-Management-Ereignisse,
24. Wiederherstellung nach abgebrochener Thread-Ausführung.

## 48. Leistungsanforderungen

Der Scheduler soll folgende Eigenschaften erreichen:

- konstante oder logarithmische Auswahlkosten innerhalb der Hauptklassen,
- geringe Sperrkonflikte zwischen CPUs,
- begrenzte Wakeup-Latenz,
- möglichst wenige unnötige Thread-Migrationen,
- geringe Kosten für Context Switches,
- keine global serialisierte Runqueue im Normalbetrieb.

Konkrete Grenzwerte werden pro Zielarchitektur und Hardwareprofil festgelegt.

## 49. Verbindliche Invarianten

1. Ein Thread läuft niemals gleichzeitig auf mehreren CPUs.
2. Jede aktive CPU besitzt genau einen Idle-Thread.
3. Nur Threads im Zustand `READY` dürfen eingeplant werden.
4. Jede CPU führt höchstens einen regulären Thread gleichzeitig aus.
5. CPU-Affinitäten werden bei Auswahl und Migration eingehalten.
6. Realtime- und Deadline-Klassen erfordern passende Capabilities.
7. Ein Context Switch erhält den vollständigen erforderlichen Threadzustand.
8. Runqueue-Änderungen erfolgen atomar unter der vorgesehenen Synchronisation.
9. Prioritätsvererbung endet nach Wegfall ihrer Ursache.
10. Kein unprivilegierter Thread darf kritische Kernelthreads dauerhaft verdrängen.
11. Blockierte oder suspendierte Threads erhalten keine CPU-Zeit.
12. CPU-Zeit und Budgetverbrauch werden monoton erfasst.

## 50. Referenzablauf einer Neuplanung

```text
Scheduler-Ereignis
  -> Präemptionszustand prüfen
  -> aktuellen Thread abrechnen
  -> Threadzustand aktualisieren
  -> Runqueue sperren
  -> höchste zulässige Scheduler-Klasse auswählen
  -> nächsten Thread bestimmen
  -> Runqueue aktualisieren
  -> Adressraum und Sicherheitskontext vorbereiten
  -> architekturspezifischen Context Switch ausführen
  -> neuen Thread fortsetzen
```

## 51. Zusammenfassung

Der NovaOS-Scheduler verwendet per-CPU-Runqueues und plant Threads anhand von Scheduler-Klasse, Priorität, Fairness, CPU-Affinität und Laufzeitbudgets ein.

Normale und Hintergrundaufgaben werden fair verteilt, während interaktive Threads kurze Reaktionszeiten erhalten. Realtime- und Deadline-Verarbeitung sind capability-geschützt und durch Budgets begrenzt.

Mehrprozessorsysteme, heterogene CPUs, NUMA, Energieverwaltung und Thermal Management werden berücksichtigt, ohne die grundlegenden Sicherheits- und Fairnessregeln des Systems aufzugeben.