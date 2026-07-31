# ADR-DIAG-0004 – Performance Counter

| Feld | Wert |
|---|---|
| Dokument-ID | ADR-DIAG-0004 |
| Titel | Performance Counter |
| Status | Angenommen |
| Version | 1.0 |
| Datum | 2026-07-31 |
| Bereich | Performanceanalyse und Hardwarediagnose |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | ADR-DIAG-0001, ADR-DIAG-0002, ADR-PROC-0007, ADR-SEC-0006, ADR-SEC-0010 |
| Nachfolgende Spezifikation | NPSPEC-DIAG-PERFORMANCE-COUNTER |

---

## 1. Kontext

Moderne Prozessoren und ausgewählte Geräte besitzen Hardwarezähler zur Messung interner Ereignisse.

Damit können unter anderem erfasst werden:

- CPU-Zyklen
- ausgeführte Instruktionen
- Cachetreffer und Cachefehler
- Sprungvorhersagefehler
- TLB-Ereignisse
- Speicherzugriffe
- Pipeline-Stalls
- Kontextwechsel
- Interrupts
- Energie- und Leistungsdaten

NovaOS benötigt eine einheitliche Schnittstelle, um diese Zähler sicher für Performanceanalyse, Tracing und Systemoptimierung zu verwenden.

---

## 2. Problemstellung

Performance Monitoring Units unterscheiden sich zwischen CPU-Architekturen, Herstellern und Prozessorgenerationen.

Zusätzlich können Performance Counter:

- Informationen über andere Prozesse offenlegen
- Seitenkanalangriffe erleichtern
- bei hoher Samplingrate das System überlasten
- CPU-lokal unterschiedlich sein
- durch Multiplexing ungenaue Ergebnisse liefern
- unter Virtualisierung nur eingeschränkt verfügbar sein

NovaOS muss deshalb Zugriff, Genauigkeit und Ressourcenverbrauch klar kontrollieren.

---

## 3. Entscheidung

NovaOS implementiert ein capability-basiertes Performance-Counter-Framework mit architekturspezifischen PMU-Backends.

Das Framework unterstützt:

- Hardware- und Softwarezähler
- pro Thread, Prozess, CPU oder System
- zählenden Betrieb
- ereignisbasiertes Sampling
- Zählergruppen
- Multiplexing
- Integration in das Tracing-System
- Virtualisierung
- kontrollierte Userspace-Nutzung

Systemweite und sicherheitskritische Messungen benötigen besondere Capabilities.

---

## 4. Architektur

```text
Analysewerkzeug
      |
      v
Performance-Counter-API
      |
      v
Kernel-PMU-Manager
      |
      +--> x86-PMU
      +--> ARM-PMU
      +--> virtuelle PMU
      +--> Softwarezähler
```

Anwendungen greifen nicht direkt auf globale PMU-Register zu.

Das Architektur-Backend übersetzt abstrakte Ereignisse in konkrete Hardwarekonfigurationen.

---

## 5. Ereignistypen

NovaOS unterscheidet:

| Ereignistyp | Bedeutung |
|---|---|
| Hardware Event | durch CPU oder Gerät gezählt |
| Software Event | durch Kernel erzeugter Zähler |
| Trace Event | Zählung eines definierten Tracepoints |
| Derived Metric | aus mehreren Zählern berechneter Wert |
| Vendor Event | hersteller- oder modellspezifisches Ereignis |

Portabler Anwendungscode soll bevorzugt abstrakte Hardware- und Softwareereignisse verwenden.

Vendor Events werden über separate, eindeutig identifizierte Namensräume bereitgestellt.

---

## 6. Standardereignisse

Mindestens folgende abstrakte Ereignisse werden vorgesehen:

```text
CPU_CYCLES
INSTRUCTIONS_RETIRED
CACHE_REFERENCES
CACHE_MISSES
BRANCH_INSTRUCTIONS
BRANCH_MISSES
CONTEXT_SWITCHES
CPU_MIGRATIONS
PAGE_FAULTS
INTERRUPTS
SCHEDULER_RUNTIME
TASK_CLOCK
```

Nicht jede Plattform unterstützt jedes Hardwareereignis.

Ein fehlendes Ereignis liefert `STATUS_COUNTER_NOT_SUPPORTED` und wird nicht stillschweigend durch eine andere Metrik ersetzt.

---

## 7. Counter-Objekt

Jeder konfigurierte Zähler ist ein Kernelobjekt.

```c
typedef struct performance_counter_info {
    uint64_t counter_id;
    uint32_t event_type;
    uint32_t scope;
    uint64_t target_id;
    uint64_t sample_period;
    uint32_t state;
    uint32_t flags;
} performance_counter_info_t;
```

Das Objekt enthält zusätzlich:

- PMU-Konfiguration
- aktueller Zählerstand
- Aktiv- und Laufzeit
- Multiplexingfaktor
- Zielprozess, Thread oder CPU
- Overflow-Status
- Sampling- und Bufferreferenzen

---

## 8. Messbereiche

Ein Counter kann an folgende Scopes gebunden werden:

| Scope | Bedeutung |
|---|---|
| Thread | zählt nur während der Ausführung eines Threads |
| Process | zählt über alle Threads eines Prozesses |
| Job | zählt Prozesse eines Job Objects |
| CPU | zählt Ereignisse auf einer bestimmten CPU |
| CPU Set | zählt über eine CPU-Menge |
| System | systemweite Messung |
| Device | gerätespezifischer Zähler |

Thread- und Prozesszähler werden bei Context Switches gespeichert und wiederhergestellt oder entsprechend hardwareseitig gefiltert.

---

## 9. Erzeugung

```c
status_t performance_counter_create(
    const performance_counter_config_t *config,
    cap_handle_t *counter_handle
);
```

Die Konfiguration enthält:

- Ereignis-ID
- Scope
- Ziel
- Benutzer- oder Kernelmodus
- Zähl- oder Samplingmodus
- Sampleperiode
- optionale Filter
- Gruppenzuordnung
- Flags

Der Kernel prüft die Konfiguration vollständig, bevor Hardwarezähler programmiert werden.

---

## 10. Counter-Lebenszyklus

```c
status_t performance_counter_start(
    cap_handle_t counter_handle
);

status_t performance_counter_stop(
    cap_handle_t counter_handle
);

status_t performance_counter_reset(
    cap_handle_t counter_handle
);

status_t performance_counter_read(
    cap_handle_t counter_handle,
    performance_counter_value_t *value
);
```

Ein gelesener Wert enthält:

- Rohwert
- tatsächlich aktive Zeit
- gesamte Messzeit
- Multiplexingstatus
- Overflowstatus
- Genauigkeitskennzeichnung

---

## 11. Benutzer- und Kernelmodus

Counter können abhängig von den Rechten Ereignisse erfassen aus:

- Userspace
- Kernelspace
- Hypervisor
- ausgewählten Privilegstufen
- Kombinationen dieser Bereiche

Gewöhnliche Anwendungen dürfen standardmäßig nur eigene Userspace-Ausführung messen.

Kernel-, Hypervisor- und fremde Prozessereignisse benötigen erhöhte Berechtigungen.

Nicht jede Hardware unterstützt eine vollständige Trennung aller Privilegstufen.

---

## 12. Zählergruppen

Mehrere Counter können zu einer Gruppe verbunden werden.

```c
status_t performance_group_create(
    const cap_handle_t *counters,
    uint32_t counter_count,
    cap_handle_t *group_handle
);
```

Eine Gruppe ermöglicht:

- möglichst gleichzeitigen Start
- möglichst gleichzeitiges Stoppen
- konsistente gemeinsame Ablesung
- Berechnung abgeleiteter Metriken

Kann die Hardware nicht alle Gruppencounter gleichzeitig ausführen, wird die Gruppe abgelehnt oder ausdrücklich als gemultiplext gekennzeichnet.

---

## 13. Multiplexing

Die Anzahl gewünschter Ereignisse kann die verfügbaren Hardwarecounter überschreiten.

NovaOS darf Counter zeitlich multiplexen.

Dabei speichert das Framework:

```text
time_enabled
time_running
raw_count
```

Ein skalierter Wert kann berechnet werden als:

```text
scaled_count = raw_count × time_enabled / time_running
```

Skalierte Werte sind Schätzungen und müssen als solche gekennzeichnet werden.

Sehr kurze Messungen mit starkem Multiplexing können abgelehnt oder als unzuverlässig markiert werden.

---

## 14. Sampling

Im Samplingmodus löst ein Counter nach einer definierten Anzahl von Ereignissen einen Overflow aus.

Ein Sample kann enthalten:

- Zeitstempel
- CPU
- Prozess und Thread
- Event-ID
- Zählerwert
- Modul-ID und Instruction-Offset
- optional geschützten Stacktrace
- Korrelations-ID

Samples werden in CPU-lokale Tracing-Buffer geschrieben.

Der Overflowpfad darf nicht blockieren.

---

## 15. Samplingrate

Sampling kann zeit- oder ereignisbasiert konfiguriert werden.

NovaOS begrenzt:

- maximale Samples pro Sekunde
- minimale Sampleperiode
- Stacktrace-Häufigkeit
- Bufferverbrauch
- aktive Samplingcounter
- systemweite Interruptlast

Bei Überlastung kann das Framework:

- Sampleperiode erhöhen
- Stacktraces deaktivieren
- Ereignisse verwerfen
- Session stoppen

Eine automatische Anpassung wird im Ergebnis sichtbar gekennzeichnet.

---

## 16. Context Switches

Thread- und Prozesscounter werden mit dem Scheduler koordiniert.

Bei einem Context Switch:

1. werden aktive Counter des alten Threads gestoppt oder gespeichert,
2. werden Zählerstände akkumuliert,
3. wird die PMU für den neuen Thread konfiguriert,
4. werden dessen Counter gestartet.

Der zusätzliche Scheduleraufwand wird minimiert.

Bei sehr vielen aktiven Countersets dürfen diese multiplexiert oder durch Ressourcenlimits abgelehnt werden.

---

## 17. CPU-Migration

Migriert ein Thread auf eine andere CPU:

- werden bisherige Zählerstände gespeichert,
- wird die neue PMU-Konfiguration geprüft,
- wird die Messung auf der Ziel-CPU fortgesetzt,
- werden Unterschiede der PMU-Fähigkeiten erfasst.

Auf heterogenen CPUs kann ein Ereignis auf einzelnen CPU-Typen eine andere Bedeutung oder Verfügbarkeit besitzen.

NovaOS darf solche Werte nicht ungekennzeichnet zusammenfassen.

---

## 18. CPU-Hotplug

Vor dem Offline-Schalten einer CPU:

- werden CPU-lokale Counter gestoppt,
- werden Werte gesichert,
- werden Samplingoverflows deaktiviert,
- werden Sessions informiert,
- werden migrierbare Counter verschoben.

CPU-gebundene Counter wechseln in einen pausierten oder beendeten Zustand.

Eine offline geschaltete CPU darf keine aktiven PMU-Interrupts hinterlassen.

---

## 19. Softwarecounter

Zusätzlich zur Hardware-PMU stellt der Kernel Softwarecounter bereit.

Beispiele:

- Context Switches
- Page Faults
- System Calls
- Interrupts
- Thread Wake-ups
- IPC-Nachrichten
- Dateioperationen
- Netzwerkpakete
- Schedulerlaufzeit

Softwarecounter verwenden möglichst dieselben Sessions, Scopes und Readerstrukturen wie Hardwarecounter.

Sie dürfen jedoch nicht als Hardwaremessungen dargestellt werden.

---

## 20. Abgeleitete Metriken

Werkzeuge können aus mehreren Countern Kennzahlen berechnen.

Beispiele:

```text
IPC = Instructions Retired / CPU Cycles

Cache-Miss-Rate = Cache Misses / Cache References

Branch-Miss-Rate = Branch Misses / Branch Instructions
```

Das Framework liefert die Rohwerte und Laufzeiten.

Abgeleitete Metriken werden bevorzugt im Userspace berechnet, damit der Kernel keine instabilen Interpretationen festschreibt.

---

## 21. Virtualisierung

Unter einem Hypervisor kann NovaOS folgende Backends verwenden:

- virtualisierte PMU
- paravirtualisierte Counter
- eingeschränkte Softwarecounter
- keine Hardwarecounter

Der Hypervisor kann Werte verfälschen, multiplexen oder einschränken.

NovaOS kennzeichnet daher:

- virtualisierten Ursprung
- bekannte Messunsicherheit
- nicht verfügbare Privilegstufen
- Steal Time, sofern verfügbar

Virtuelle Counter dürfen nicht als exakte physische Hardwaremessung dargestellt werden.

---

## 22. Capability-Rechte

| Recht | Bedeutung |
|---|---|
| `PERF_RIGHT_SELF` | eigenen Prozess messen |
| `PERF_RIGHT_PROCESS` | autorisierten fremden Prozess messen |
| `PERF_RIGHT_JOB` | Job Object messen |
| `PERF_RIGHT_CPU` | einzelne CPUs messen |
| `PERF_RIGHT_SYSTEM` | systemweite Messung durchführen |
| `PERF_RIGHT_KERNEL` | Kernelereignisse erfassen |
| `PERF_RIGHT_STACK` | Stacktraces aufnehmen |
| `PERF_RIGHT_RAW` | herstellerspezifische Raw Events verwenden |
| `PERF_RIGHT_ADMIN` | PMU-Framework verwalten |

Raw Events und systemweite Kernelmessungen sind im Produktivmodus besonders geschützt.

---

## 23. Seitenkanalschutz

Performance Counter können Mikroarchitekturinformationen über andere Prozesse offenlegen.

NovaOS begrenzt deshalb:

- systemweite Cache- und Branch-Events
- Kernel- und Hypervisorereignisse
- Messung fremder Sicherheitsdomänen
- hochauflösende Raw Events
- Kombinationen mit präziser Adressabtastung
- Nutzung durch nicht vertrauenswürdige Anwendungen

Bei aktivierten Hochsicherheitsprofilen können bestimmte PMU-Funktionen vollständig deaktiviert werden.

Sandboxen erhalten standardmäßig nur grobe eigene Prozessmetriken.

---

## 24. Kerneladressen

Samples enthalten im Produktionsmodus keine rohen Kerneladressen.

Stattdessen werden verwendet:

- Modul-ID
- geschützter Offset
- Stack-ID
- Build-ID

Vollständige Symbolauflösung benötigt Secure-Debugging- und Tracing-Rechte.

Userspace-Adressen fremder Prozesse werden ebenfalls nicht ohne entsprechende Berechtigung offengelegt.

---

## 25. Ressourcenbegrenzung

Begrenzt werden mindestens:

- Counter pro Prozess
- Countergruppen
- systemweite Sessions
- Samplingrate
- Tracebuffer
- Stacktiefe
- Raw Events
- PMU-Reprogrammierungen
- Context-Switch-Overhead
- Multiplexingfaktor

Bei Ressourcenknappheit haben Kernelstabilität und Schedulerbetrieb Vorrang vor Performanceanalyse.

---

## 26. Sicherheit und Datenschutz

Für Performance Counter gelten folgende Regeln:

- PMU-Zugriffe erfolgen über Capabilities.
- Anwendungen erhalten standardmäßig nur eigene Userspace-Messungen.
- systemweite und Kernelmessungen sind privilegiert.
- rohe Kerneladressen werden geschützt.
- Raw Events werden nach CPU-Modell validiert.
- Samplingpfade dürfen nicht blockieren.
- Counterdaten werden nicht automatisch extern übertragen.
- Messungen fremder Prozesse benötigen eine ausdrückliche Berechtigung.
- Hochsicherheitsprofile dürfen riskante Counter deaktivieren.

---

## 27. Audit und Diagnose

Auditiert werden insbesondere:

- Aktivierung systemweiter PMU-Sessions
- Messung fremder Prozesse
- Zugriff auf Kernel- oder Hypervisorereignisse
- Verwendung von Raw Events
- hohe Samplingraten
- Änderungen globaler PMU-Richtlinien
- deaktivierte Seitenkanalschutzmaßnahmen

Gewöhnliche eigene Prozessmessungen werden nicht einzeln auditiert.

Counterwerte enthalten keine direkten Anwendungsinhalte.

---

## 28. Fehlercodes

| Fehlercode | Bedeutung |
|---|---|
| `STATUS_OK` | Operation erfolgreich |
| `STATUS_COUNTER_NOT_SUPPORTED` | Ereignis ist nicht verfügbar |
| `STATUS_COUNTER_UNAVAILABLE` | kein Hardwarecounter verfügbar |
| `STATUS_COUNTER_CONFLICT` | Gruppe kann nicht gemeinsam ausgeführt werden |
| `STATUS_COUNTER_MULTIPLEXED` | Wert wurde zeitlich multiplexiert |
| `STATUS_SAMPLE_RATE_TOO_HIGH` | Samplingrate überschreitet das Limit |
| `STATUS_CPU_OFFLINE` | Ziel-CPU ist offline |
| `STATUS_ACCESS_DENIED` | erforderliche Capability fehlt |
| `STATUS_LIMIT_EXCEEDED` | Ressourcenlimit überschritten |
| `STATUS_VIRTUALIZED` | Messung stammt aus einem virtualisierten Backend |
| `STATUS_NOT_COMPARABLE` | Werte heterogener CPUs sind nicht direkt vergleichbar |

---

## 29. Alternative Ansätze

### 29.1 Direkter PMU-Zugriff aus dem Userspace

**Vorteile:**

- geringer Systemaufrufaufwand
- flexible Werkzeuge

**Nachteile:**

- unzureichende Isolation
- Konflikte zwischen Anwendungen
- hohes Seitenkanalrisiko

**Entscheidung:** Abgelehnt.

### 29.2 Nur Softwarecounter

**Vorteile:**

- plattformunabhängig
- einfache Sicherheitskontrolle

**Nachteile:**

- keine Mikroarchitektur- und Cacheanalyse
- geringerer Diagnosewert

**Entscheidung:** Softwarecounter ergänzen die Hardware-PMU.

### 29.3 Keine PMU im Produktivsystem

**Vorteile:**

- minimale Seitenkanalfläche

**Nachteile:**

- stark eingeschränkte Performanceanalyse
- schwierige Diagnose realer Lastprobleme

**Entscheidung:** PMU bleibt verfügbar, aber capability- und richtliniengeschützt.

---

## 30. Konsequenzen

### Positive Konsequenzen

- detaillierte CPU- und Systemanalyse
- gemeinsame API für Hardware- und Softwarecounter
- Unterstützung von Sampling und Tracing
- pro Thread, Prozess, CPU und System nutzbar
- kontrolliertes Multiplexing
- Unterstützung unterschiedlicher Architekturen und Hypervisoren
- Seitenkanalschutz durch Capability-Rechte

### Negative Konsequenzen

- architekturspezifische PMU-Backends erforderlich
- Multiplexing reduziert die Genauigkeit
- Context-Switch-Integration erzeugt zusätzlichen Aufwand
- heterogene CPUs erschweren Vergleiche
- Sampling kann die Systemleistung beeinflussen

### Risiken

- Informationslecks zwischen Prozessen
- zu hohe Samplinginterruptlast
- fehlerhafte Raw Events
- ungenaue skalierte Werte
- PMU-Fehler unter Virtualisierung
- Offenlegung von Kerneladressen

---

## 31. Implementierungsrichtlinien

- Anwendungen greifen nicht direkt auf globale PMU-Register zu.
- Portabler Code verwendet abstrakte Standardereignisse.
- Raw Events sind CPU-modell- und capability-gebunden.
- Counterwerte enthalten Rohwert, aktive Zeit und Messzeit.
- Multiplexte Werte werden eindeutig als Schätzungen markiert.
- Sampling schreibt nicht blockierend in CPU-lokale Tracebuffer.
- Kernel- und Fremdprozessmessungen benötigen besondere Rechte.
- CPU-Migration und heterogene CPUs werden im Ergebnis sichtbar gemacht.
- Kerneladressen werden als Modul-ID und Offset dargestellt.
- Der Scheduler darf durch Performance Counter nicht unvertretbar belastet werden.

---

## 32. Testanforderungen

Die Implementierung benötigt mindestens Tests für:

- Hardware- und Softwarecounter
- Thread-, Prozess-, CPU- und Systemscope
- Start, Stop, Reset und Read
- Countergruppen
- Multiplexing und Skalierung
- Sampling und Overflow
- Stacktraces
- Context Switches
- CPU-Migration
- CPU-Hotplug
- heterogene CPUs
- Virtualisierung
- Capability- und Seitenkanalschutz
- hohe Samplingraten
- ungültige Raw Events

---

## 33. Verbindliche Invarianten

1. Performance Counter werden durch den Kernel-PMU-Manager koordiniert.
2. Gewöhnliche Anwendungen messen standardmäßig nur ihren eigenen Userspace.
3. Kernel-, System- und Fremdprozessmessungen benötigen besondere Capabilities.
4. Nicht unterstützte Ereignisse werden nicht durch semantisch andere Werte ersetzt.
5. Multiplexte Werte werden als skalierte Schätzungen gekennzeichnet.
6. Samplingpfade dürfen nicht blockieren.
7. Raw Events werden gegen CPU-Modell und Richtlinie validiert.
8. Rohe Kerneladressen erscheinen nicht in Produktionssamples.
9. Hochsicherheitsprofile dürfen riskante PMU-Funktionen deaktivieren.
10. Performance-Daten werden nicht automatisch extern übertragen.

---

## 34. Zusammenfassung

NovaOS implementiert ein capability-basiertes Performance-Counter-Framework für Hardware-PMUs, Softwareereignisse und Tracing.

Counter können an Threads, Prozesse, Jobs, CPUs oder das Gesamtsystem gebunden werden. Zählergruppen, Multiplexing und Sampling ermöglichen detaillierte Analysen, wobei Genauigkeit und Messzeit transparent ausgewiesen werden.

Strenge Rechte für Kernel-, System- und Raw-Messungen reduzieren Seitenkanalrisiken. Anwendungen erhalten standardmäßig nur Zugriff auf ihre eigenen Userspace-Metriken.