# ADR-DIAG-0006 – Kernel Profiler

| Feld | Wert |
|---|---|
| Dokument-ID | ADR-DIAG-0006 |
| Titel | Kernel Profiler |
| Status | Angenommen |
| Version | 1.0 |
| Datum | 2026-07-31 |
| Bereich | Kernel-Diagnose und Performanceanalyse |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | ADR-DIAG-0001, ADR-DIAG-0002, ADR-DIAG-0004, ADR-DIAG-0005, ADR-SEC-0010 |
| Nachfolgende Spezifikation | NPSPEC-DIAG-KERNEL-PROFILER |

---

## 1. Kontext

NovaOS benötigt einen Kernel Profiler, um Performanceprobleme im Kernel und in kernelnahen Subsystemen statistisch zu untersuchen.

Typische Anwendungsfälle sind:

- hohe CPU-Auslastung
- langsame Systemaufrufe
- Scheduler-Overhead
- Interruptlast
- Lock Contention
- Speicher- und Cacheprobleme
- langsame Treiber
- Netzwerk- und Datei-I/O-Latenzen
- Energieverbrauch
- Mehrkernskalierung

Der Profiler soll Engpässe sichtbar machen, ohne den gemessenen Systemzustand unverhältnismäßig zu verändern.

---

## 2. Problemstellung

Kernelprofiling benötigt Einblick in privilegierte Ausführung und kann dadurch sensible Informationen offenlegen.

Ein ungeeigneter Profiler kann außerdem:

- hohe Interruptlast erzeugen,
- zeitkritische Kernelpfade stören,
- Kerneladressen offenlegen,
- große Datenmengen erzeugen,
- Locking und Cacheverhalten verändern,
- falsche Messergebnisse produzieren,
- Seitenkanalangriffe erleichtern.

NovaOS benötigt deshalb ein begrenztes, statistisches und capability-geschütztes Profiling-System.

---

## 3. Entscheidung

NovaOS implementiert einen statistischen Kernel Profiler auf Grundlage von:

- Performance Countern
- zeitbasiertem Sampling
- statischen Tracepoints
- CPU-lokalen Sample-Buffern
- Stack- und Callchain-Erfassung
- aggregierten Laufzeitmetriken

Der Profiler verwendet bestehende Diagnoseinfrastrukturen, führt aber eigene Sessions, Profile und Auswertungsmetadaten.

Kernelprofiling ist im Produktionsmodus standardmäßig deaktiviert oder auf grobe, intern benötigte Metriken begrenzt.

---

## 4. Abgrenzung

| Subsystem | Aufgabe |
|---|---|
| Kernel Logging | Fehler und Zustandsmeldungen |
| Tracing | genaue Ereignisabläufe |
| Performance Counter | Zählung einzelner Hardware- und Softwareereignisse |
| Kernel Profiler | statistische Zuordnung von Kosten zu Codepfaden |
| Debug API | aktive Kontrolle und Untersuchung |
| Crash Dump | Analyse eines abgestürzten Zustands |

Der Profiler verändert keinen Kernelcode und ersetzt weder Tracing noch Debugging.

---

## 5. Architektur

```text
Timer oder PMU-Overflow
          |
          v
     Sample erfassen
          |
          v
CPU-lokaler Sample-Buffer
          |
          v
   Profiler-Aggregation
          |
          v
  Userspace-Auswertung
          |
          v
Symbole, Berichte und Flame Graphs
```

Symbolauflösung und aufwendige Berichtserzeugung erfolgen außerhalb des Kernels.

---

## 6. Profiling-Sessions

Jede Messung wird durch eine Profiler-Session repräsentiert.

```c
typedef struct profiler_session_config {
    uint32_t structure_size;
    uint32_t version;
    uint32_t mode;
    uint32_t scope;
    uint64_t sample_period;
    uint64_t duration_ns;
    uint32_t flags;
    uint32_t max_stack_depth;
} profiler_session_config_t;
```

Eine Session definiert:

- Profilingmodus
- Zielscope
- Ereignisquelle
- Samplingrate
- Laufzeit
- Stacktiefe
- Filter
- Buffergröße
- Sicherheits- und Datenschutzstufe

---

## 7. Profiling-Modi

NovaOS unterstützt mindestens:

| Modus | Bedeutung |
|---|---|
| CPU Time | periodische Stichproben aktiver CPU-Ausführung |
| Event Sampling | Sample nach PMU-Ereigniszahl |
| Scheduler Profile | Laufzeit, Wake-ups und Context Switches |
| Lock Profile | Wartezeit und Contention |
| I/O Profile | Latenz von Datei-, Block- und Netzwerk-I/O |
| Allocation Profile | Kernelallokationen nach Codepfad |
| Power Profile | CPU-, Idle- und Energiezustände |
| Mixed Profile | kontrollierte Kombination mehrerer Quellen |

Nicht jede Plattform unterstützt alle Modi mit derselben Genauigkeit.

---

## 8. Scopes

Eine Session kann begrenzt werden auf:

- einzelne CPU
- CPU Set
- Kernelmodul
- Subsystem
- Treiber
- Prozess oder Thread
- Job Object
- Netzwerk-Namespace
- gesamtes System

Systemweite Kernelprofile benötigen besondere Capabilities.

Ein Prozessprofil darf ohne zusätzliche Rechte keine Ausführung anderer Sicherheitsdomänen erfassen.

---

## 9. Sample-Format

```c
typedef struct profiler_sample {
    uint32_t record_size;
    uint16_t version;
    uint16_t flags;

    uint64_t timestamp_ns;
    uint64_t event_id;
    uint64_t value;

    uint32_t cpu_id;
    uint32_t process_id;
    uint32_t thread_id;
    uint32_t execution_mode;

    uint64_t module_id;
    uint64_t instruction_offset;
    uint32_t callchain_id;
    uint32_t weight;
} profiler_sample_t;
```

Im Produktionsmodus enthält ein Sample keine rohe Kerneladresse.

---

## 10. Zeitbasiertes Sampling

Beim zeitbasierten Sampling löst ein CPU-Timer periodisch ein Sample aus.

Vorteile:

- einfache Interpretation als CPU-Zeitverteilung,
- geringe Abhängigkeit von PMU-Funktionen,
- Unterstützung älterer und virtueller Hardware.

Die Samplingfrequenz ist begrenzt.

Sehr hohe Frequenzen werden nur in einem ausdrücklich aktivierten Entwicklungs- oder Diagnosemodus erlaubt.

---

## 11. Ereignisbasiertes Sampling

Performance Counter können ein Sample nach einer definierten Anzahl von Ereignissen auslösen.

Beispiele:

- CPU-Zyklen
- Instruktionen
- Cache Misses
- Branch Misses
- TLB Misses
- Pipeline Stalls

Die PMU-Overflow-Behandlung schreibt nicht blockierend in einen CPU-lokalen Buffer.

Nicht unterstützte Ereignisse werden nicht durch semantisch abweichende Counter ersetzt.

---

## 12. Callchains

Ein Sample kann optional eine begrenzte Callchain enthalten.

Die Erfassung verwendet:

- Frame Pointer
- kontrollierten Stack Unwinder
- architekturspezifische Unwindinformationen
- vorberechnete Stack-IDs

Die maximale Tiefe wird pro Session begrenzt.

Kann ein Stack nicht sicher gelesen werden, wird die Callchain abgeschnitten und als unvollständig markiert.

---

## 13. Symbolauflösung

Der Kernel speichert für Codepositionen:

```text
Modul-ID + Instruction-Offset + Build-ID
```

Ein autorisiertes Userspace-Werkzeug ordnet diese Werte Symbolen und Quellinformationen zu.

Vorteile:

- kein großer Symbolresolver im Kernel,
- Schutz von Kernel-ASLR,
- genaue Zuordnung zur verwendeten Binärversion,
- kleinere Sample-Datensätze.

Fehlende Symbole verhindern nicht die Erfassung des Profils.

---

## 14. CPU-lokale Buffer

Samples werden in CPU-lokalen, begrenzten Ringpuffern gespeichert.

Der Samplingpfad darf:

- nicht blockieren,
- keine gewöhnliche Heap-Allokation ausführen,
- keine komplexen Sperren verwenden,
- keine Dateisystemoperationen ausführen,
- keine rekursiven Samples erzeugen.

Bei Überlauf werden Samples verworfen oder ältere Samples überschrieben, abhängig vom Sessionmodus.

Verluste werden durch Zähler sichtbar gemacht.

---

## 15. Aggregierter Modus

Zusätzlich zu Rohsamples kann NovaOS Werte direkt aggregieren.

Ein Aggregationsschlüssel kann enthalten:

```text
Modul-ID
Instruction-Offset
Callchain-ID
CPU
Ereignis-ID
```

Der aggregierte Modus reduziert Speicherbedarf und Datenschutzrisiken.

Er ist der bevorzugte Modus für längere Profile im Produktivsystem.

Rohsamples werden nur für zeitlich begrenzte Detailanalysen verwendet.

---

## 16. Scheduler-Profiling

Schedulerprofile können erfassen:

- CPU-Laufzeit
- Wartezeit
- Blockierungsdauer
- Wake-up-Latenz
- Context Switches
- CPU-Migrationen
- Run-Queue-Länge
- Prioritätsänderungen
- Echtzeitüberschreitungen

Die Messung basiert auf statischen Scheduler-Tracepoints und Laufzeitzählern.

Aktiviertes Profiling darf die Schedulerentscheidungen nicht verändern.

---

## 17. Lock-Profiling

Das Lock-Profiling untersucht:

- Anzahl der Sperrversuche
- Wartezeit
- Haltezeit
- Contention
- Eigentümerwechsel
- Callchains
- betroffene CPUs

Lockobjekte werden durch Diagnose-IDs statt Kerneladressen identifiziert.

Die zusätzliche Zeitmessung kann teuer sein und ist standardmäßig deaktiviert.

Zeitkritische Interruptsperren verwenden nur besonders leichte Messpunkte.

---

## 18. I/O-Profiling

I/O-Profile können Abläufe korrelieren zwischen:

- VFS
- File Cache
- Buffer Cache
- Block-I/O
- Gerätetreibern
- Netzwerk-Stack
- DMA
- Interruptabschluss

Erfasst werden:

- Queue-Zeit
- Bearbeitungszeit
- Datenmenge
- Ergebnis
- Cachetreffer
- Gerät oder Interface
- Callchain-ID

Dateiinhalte, Paketnutzdaten und vollständige Pfade werden nicht standardmäßig aufgezeichnet.

---

## 19. Allocation-Profiling

Der Kernel Profiler kann Speicherallokationen nach Ursprung untersuchen.

Mögliche Metriken:

- Anzahl
- Größe
- Lebenszeit
- fehlgeschlagene Allokationen
- NUMA-Knoten
- Allokator oder Pool
- Callchain

Allocation-Profiling darf nicht selbst für jedes Ereignis neue dynamische Speicherobjekte erzeugen.

Sampling oder begrenzte Aggregation wird gegenüber vollständiger Einzelaufzeichnung bevorzugt.

---

## 20. Power-Profiling

Power-Profile korrelieren:

- CPU-Auslastung
- CPU-Idle-Zustände
- Frequenz- und Leistungszustände
- Device Sleep
- Interruptaktivität
- Wake-ups
- Thermal Throttling
- Energieprofile

Gemessene Energieverbräuche werden als Schätzung gekennzeichnet, sofern keine kalibrierten Hardwarezähler vorhanden sind.

Der Profiler darf keine exakte Energiegenauigkeit behaupten, wenn nur Modellwerte verfügbar sind.

---

## 21. Filter

Eine Session kann filtern nach:

- CPU oder CPU Set
- Prozess und Thread
- Job Object
- Kernelmodul
- Treiber
- Subsystem
- Ereignistyp
- Mindestlatenz
- Sicherheitsdomäne
- Zeitfenster

Filter werden vor Aktivierung vollständig validiert.

Komplexe oder unbegrenzte Filterprogramme sind nicht zulässig.

---

## 22. Adaptive Samplingrate

NovaOS kann die Samplingrate automatisch reduzieren, wenn:

- Samplebuffer überlaufen,
- Interruptlast zu hoch ist,
- Echtzeitfristen gefährdet werden,
- CPU-Auslastung einen Grenzwert überschreitet,
- das System thermisch drosselt,
- die Profiler-Session ihr Ressourcenbudget überschreitet.

Eine automatische Anpassung wird im Profilbericht ausgewiesen.

Messergebnisse dürfen nicht so dargestellt werden, als sei die ursprüngliche Rate durchgehend verwendet worden.

---

## 23. Capability-Rechte

| Recht | Bedeutung |
|---|---|
| `PROFILER_RIGHT_SELF` | eigenen Prozess profilieren |
| `PROFILER_RIGHT_PROCESS` | autorisierten fremden Prozess profilieren |
| `PROFILER_RIGHT_KERNEL` | Kernelpfade erfassen |
| `PROFILER_RIGHT_SYSTEM` | systemweites Profiling durchführen |
| `PROFILER_RIGHT_STACK` | Callchains erfassen |
| `PROFILER_RIGHT_PMU` | Hardwarecounter verwenden |
| `PROFILER_RIGHT_SENSITIVE` | geschützte Metadaten lesen |
| `PROFILER_RIGHT_EXPORT` | Profildaten exportieren |
| `PROFILER_RIGHT_ADMIN` | Profiler verwalten |

Kernel- und systemweites Profiling sind im Produktionsmodus besonders geschützt.

---

## 24. Ressourcenbegrenzung

Begrenzt werden mindestens:

- Sessions pro Benutzer und System
- Samplingfrequenz
- Buffergröße pro CPU
- maximale Callchain-Tiefe
- Profilingdauer
- Aggregationseinträge
- aktive PMU-Ereignisse
- Lock- und Allocation-Tracepoints
- persistente Profilgröße
- CPU-Zeit für Auswertung

Kernelstabilität, Echtzeit und thermischer Schutz haben Vorrang vor laufenden Profiling-Sessions.

---

## 25. Sicherheit

Für den Kernel Profiler gelten folgende Regeln:

- Profiling-Sessions benötigen Capabilities.
- Systemweite Kernelprofile sind standardmäßig deaktiviert.
- Samples enthalten keine rohen Kerneladressen.
- Samplingpfade dürfen nicht blockieren.
- Profiler und Debug API bleiben getrennt.
- Profiling darf keinen Kernelzustand verändern.
- fremde Prozesse und Sicherheitsdomänen benötigen besondere Rechte.
- Hochsicherheitsprofile dürfen PMU- und Stackprofiling einschränken.
- Profildaten werden nicht automatisch extern übertragen.
- Schlüssel und Speicherinhalte werden nicht aufgezeichnet.

---

## 26. Datenschutz

Profilergebnisse können Rückschlüsse auf Anwendungen und Benutzeraktivitäten erlauben.

Deshalb gilt:

- langfristige Profile verwenden bevorzugt Aggregation,
- Prozessnamen und Pfade werden minimiert,
- Anwendungsinhalte werden nicht erfasst,
- Netzwerkziele werden standardmäßig nicht gespeichert,
- Sessiondauer bleibt begrenzt,
- persistente Profile werden zugriffsgeschützt gespeichert,
- systemweites Profiling wird sichtbar gekennzeichnet,
- externer Export benötigt ausdrückliche Freigabe.

---

## 27. Audit und Diagnose

Auditiert werden insbesondere:

- Aktivierung systemweiter Kernelprofile
- Profiling geschützter Prozesse
- hohe Samplingraten
- PMU-Raw-Events
- Stack- und Callchain-Erfassung
- Export von Profildaten
- Änderungen globaler Profiler-Richtlinien

Gewöhnliches Profiling des eigenen Prozesses wird nicht einzeln auditiert.

Der Profiler führt eigene Verlust-, Overhead- und Genauigkeitsstatistiken.

---

## 28. Fehlercodes

| Fehlercode | Bedeutung |
|---|---|
| `STATUS_OK` | Session erfolgreich gestartet oder beendet |
| `STATUS_PROFILER_UNAVAILABLE` | benötigtes Backend ist nicht verfügbar |
| `STATUS_EVENT_NOT_SUPPORTED` | Ereignis wird nicht unterstützt |
| `STATUS_SAMPLE_RATE_TOO_HIGH` | Samplingrate überschreitet das Limit |
| `STATUS_PROFILE_INCOMPLETE` | Samples oder CPUs fehlen |
| `STATUS_PROFILE_BUFFER_OVERRUN` | Samplebuffer wurde überholt |
| `STATUS_PROFILE_DEGRADED` | Samplingrate oder Umfang wurde reduziert |
| `STATUS_ACCESS_DENIED` | erforderliche Capability fehlt |
| `STATUS_LIMIT_EXCEEDED` | Ressourcenbudget wurde überschritten |
| `STATUS_CPU_OFFLINE` | Ziel-CPU ist nicht aktiv |
| `STATUS_NOT_COMPARABLE` | Messwerte sind nicht direkt vergleichbar |

---

## 29. Alternative Ansätze

### 29.1 Ausschließlich Instrumentierungs-Profiling

**Vorteile:**

- genaue Funktionsaufrufe und Laufzeiten

**Nachteile:**

- hoher Laufzeitaufwand
- verändert das gemessene System stark

**Entscheidung:** Statische Instrumentierung ergänzt das statistische Sampling nur gezielt.

### 29.2 Ausschließlich PMU-Sampling

**Vorteile:**

- detaillierte Hardwareereignisse

**Nachteile:**

- nicht auf jeder Plattform verfügbar
- stärkeres Seitenkanalrisiko

**Entscheidung:** PMU, Timer und Tracepoints werden kombiniert.

### 29.3 Dauerhaftes systemweites Profiling

**Vorteile:**

- historische Performanceanalyse jederzeit möglich

**Nachteile:**

- dauerhafter Overhead
- erhebliche Datenschutz- und Sicherheitsprobleme

**Entscheidung:** Abgelehnt. Sessions sind ausdrücklich aktiviert und begrenzt.

---

## 30. Konsequenzen

### Positive Konsequenzen

- statistische Analyse von Kernelengpässen
- Integration mit PMU und Tracing
- CPU-lokale skalierbare Datenerfassung
- Profile für Scheduler, Locks, I/O, Speicher und Energie
- aggregierter Modus für längere Messungen
- geschützte ASLR- und Symbolinformationen
- kontrollierte Produktionsdiagnose

### Negative Konsequenzen

- Sampling liefert keine vollständige Ereignishistorie
- Ergebnisse besitzen statistische Unsicherheit
- Callchain-Erfassung kann teuer sein
- heterogene CPUs erschweren Vergleiche
- Profiling beeinflusst das gemessene System teilweise selbst

### Risiken

- zu hohe Samplinglast
- Offenlegung fremder Aktivitätsmuster
- unvollständige oder verzerrte Profile
- PMU-Seitenkanäle
- Bufferüberlauf
- fehlerhafte Stack-Unwinder

---

## 31. Implementierungsrichtlinien

- Der Profiler verwendet statistisches Sampling als Standardmodell.
- Samplingpfade schreiben nicht blockierend in CPU-lokale Buffer.
- Symbolauflösung erfolgt außerhalb des Kernels.
- Kernelpositionen werden als Modul-ID und Offset gespeichert.
- Langzeitprofile verwenden bevorzugt aggregierte Daten.
- Sessions besitzen feste Dauer-, Speicher- und CPU-Budgets.
- Callchains werden sicher und mit begrenzter Tiefe erfasst.
- Adaptive Samplingänderungen werden im Ergebnis ausgewiesen.
- Profiling darf Scheduler-, Lock- oder I/O-Semantik nicht verändern.
- Kernel-, System- und Fremdprozessprofile benötigen besondere Capabilities.
- Profildaten werden nicht automatisch extern übertragen.

---

## 32. Testanforderungen

Die Implementierung benötigt mindestens Tests für:

- zeit- und ereignisbasiertes Sampling
- CPU-lokale Buffer
- Aggregation
- Callchains
- Symbolzuordnung über Build-IDs
- Scheduler-Profiling
- Lock Contention
- I/O-Latenzen
- Allocation-Profiling
- Power-Profiling
- adaptive Samplingraten
- Bufferüberlauf
- CPU-Hotplug
- heterogene CPUs
- Virtualisierung
- Capability- und Datenschutzregeln

---

## 33. Verbindliche Invarianten

1. Kernelprofiling erfolgt ausschließlich über capability-geschützte Sessions.
2. Statistisches Sampling ist das Standardmodell.
3. Samplingpfade dürfen nicht blockieren.
4. Samples werden in begrenzten CPU-lokalen Buffern gespeichert.
5. Rohe Kerneladressen erscheinen nicht in Produktionsprofilen.
6. Symbolauflösung erfolgt außerhalb des Kernels.
7. Profiling darf den beobachteten Kernelzustand nicht verändern.
8. Systemweite und fremde Prozessprofile benötigen besondere Rechte.
9. Adaptive Reduzierungen und Sampleverluste werden sichtbar gekennzeichnet.
10. Profildaten und Anwendungsinhalte werden nicht automatisch extern übertragen.

---

## 34. Zusammenfassung

NovaOS implementiert einen statistischen Kernel Profiler auf Grundlage von Performance Countern, Timersampling und statischen Tracepoints.

Der Profiler kann CPU-Zeit, Scheduler, Locks, I/O, Speicherallokationen und Energiezustände analysieren. Samples werden nicht blockierend in CPU-lokale Buffer geschrieben und anschließend außerhalb des Kernels symbolisiert und ausgewertet.

Capability-Rechte, begrenzte Sessions, geschützte Kerneladressen und aggregierte Produktionsprofile ermöglichen aussagekräftige Performanceanalysen, ohne daraus eine dauerhafte oder unkontrollierte Systemüberwachung zu machen.