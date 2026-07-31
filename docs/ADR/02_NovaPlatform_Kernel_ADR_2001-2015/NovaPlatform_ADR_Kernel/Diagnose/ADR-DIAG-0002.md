# ADR-DIAG-0002 – Tracing

| Feld | Wert |
|---|---|
| Dokument-ID | ADR-DIAG-0002 |
| Titel | Tracing |
| Status | Angenommen |
| Version | 1.0 |
| Datum | 2026-07-31 |
| Bereich | Kernel-Diagnose und Performanceanalyse |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | ADR-DIAG-0001, ADR-KERNEL-0101, ADR-KERNEL-0104, ADR-SEC-0008, ADR-SEC-0010 |
| Nachfolgende Spezifikation | NPSPEC-DIAG-TRACING |

---

## 1. Kontext

Kernel Logging dokumentiert einzelne technische Ereignisse. Für die Analyse von zeitlichen Abläufen, Latenzen und Abhängigkeiten benötigt NovaOS zusätzlich ein Tracing-System.

Typische Anwendungsfälle sind:

- Scheduleranalyse
- Interruptlatenzen
- Systemaufrufe
- IPC- und RPC-Abläufe
- Datei- und Netzwerk-I/O
- Treiberoperationen
- Energiezustandswechsel
- Boot- und Resume-Analyse
- Performanceoptimierung
- Deadlock- und Stallerkennung

Tracing muss hohe Ereignisraten mit geringem Laufzeitaufwand verarbeiten können.

---

## 2. Problemstellung

Tracing kann sehr große Datenmengen erzeugen und sensible Informationen über System- und Benutzeraktivitäten offenlegen.

Ein ungeeignetes Tracing-System kann außerdem:

- zeitkritische Abläufe verändern,
- Kernelpfade blockieren,
- Speicher unbegrenzt belegen,
- Kerneladressen offenlegen,
- rekursive Trace-Ereignisse erzeugen,
- falsche Performanceergebnisse produzieren,
- den Produktionsbetrieb destabilisieren.

NovaOS benötigt deshalb eine begrenzte, capability-geschützte und standardmäßig weitgehend deaktivierte Tracing-Infrastruktur.

---

## 3. Entscheidung

NovaOS implementiert ein strukturiertes Tracing-System mit statischen Tracepoints, CPU-lokalen Ringpuffern und optionalem Sampling.

Es gilt:

- Tracepoints sind standardmäßig deaktiviert oder minimal aktiv.
- Aktivierte Tracepoints schreiben nicht blockierend in CPU-lokale Buffer.
- Tracing wird über zeitlich begrenzte Sessions verwaltet.
- Jeder Tracepoint besitzt eine stabile ID und ein versioniertes Schema.
- Sensible Felder werden abhängig von den Rechten maskiert oder ausgelassen.
- Externer Trace-Export ist standardmäßig deaktiviert.

---

## 4. Abgrenzung zu Kernel Logging und Audit

| Subsystem | Hauptzweck |
|---|---|
| Kernel Logging | technische Zustände und Fehler |
| Tracing | zeitliche Abläufe und Performance |
| Audit | sicherheitsrelevante Nachvollziehbarkeit |
| Crash Dump | Zustand nach schwerem Fehler |

Tracing darf nicht als zuverlässiges Auditprotokoll verwendet werden.

Trace-Ereignisse können bei Überlastung verloren gehen und werden normalerweise nur für begrenzte Zeit aufgezeichnet.

---

## 5. Architektur

```text
Kernel-Tracepoint
       |
       v
Sessionfilter
       |
       v
CPU-lokaler Ringpuffer
       |
       v
Trace Reader
       |
       +--> lokale Analyse
       +--> Performancewerkzeug
       +--> begrenzte Datei
```

Ein langsamer Reader darf den aufgezeichneten Kernelpfad nicht blockieren.

---

## 6. Trace Sessions

Tracing wird über Sessions gesteuert.

```c
typedef struct trace_session_config {
    uint32_t structure_size;
    uint32_t version;
    uint64_t buffer_size_per_cpu;
    uint64_t duration_ns;
    uint32_t flags;
    uint32_t event_count;
} trace_session_config_t;
```

Eine Session definiert:

- aktivierte Tracepoints
- Filter
- Buffergröße
- Laufzeit
- Samplingrate
- Sichtbarkeitsrechte
- Verluststrategie
- Ausgabeziel
- Besitzer

Sessions enden bei Timeout, Prozessende oder ausdrücklichem Stop.

---

## 7. Tracepoints

Ein Tracepoint ist ein stabil definierter Messpunkt im Kernel oder in einem Treiber.

Beispiele:

```text
scheduler.thread_switch
scheduler.thread_wakeup
interrupt.enter
interrupt.exit
ipc.message_send
ipc.message_receive
vfs.open
vfs.read
network.packet_drop
device.power_transition
power.suspend_phase
```

Tracepoints dürfen ihre ID und Feldbedeutung innerhalb derselben Schemaversion nicht verändern.

---

## 8. Ereignisformat

```c
typedef struct trace_event_header {
    uint32_t record_size;
    uint16_t schema_version;
    uint16_t flags;

    uint64_t event_id;
    uint64_t timestamp_ns;
    uint64_t sequence_number;

    uint32_t cpu_id;
    uint32_t process_id;
    uint32_t thread_id;
    uint32_t session_mask;
} trace_event_header_t;
```

Auf den Header folgen typisierte Felder entsprechend dem Tracepoint-Schema.

Unbekannte neue Felder müssen anhand der Recordgröße übersprungen werden können.

---

## 9. Statische Tracepoints

Statische Tracepoints werden an vorab definierten Stellen in den Kernelcode eingebaut.

Vorteile:

- kontrollierte Feldtypen
- stabile Semantik
- geringer deaktivierter Overhead
- keine willkürliche Codeveränderung
- sichere Produktionsnutzung

Ein deaktivierter Tracepoint soll nur eine sehr kurze Aktivierungsprüfung ausführen.

Die erste NovaOS-Version verwendet primär statische Tracepoints.

---

## 10. Dynamisches Tracing

Dynamisches Tracing kann später für autorisierte Entwicklungs- und Diagnosesysteme ergänzt werden.

Es darf nur:

- definierte sichere Funktionen instrumentieren,
- verifizierte Programme verwenden,
- feste Ausführungslimits besitzen,
- keine beliebigen Kernelzeiger dereferenzieren,
- keine Kernelzustände verändern,
- Secure-Debugging-Richtlinien einhalten.

Willkürliches Patchen von Kernelcode ist im normalen Produktionsmodus nicht zulässig.

---

## 11. CPU-lokale Ringpuffer

Jede CPU besitzt pro Session oder Sessiongruppe einen begrenzten Ringpuffer.

Dadurch werden:

- globale Sperren vermieden,
- Ereignisse im Scheduler- und Interruptpfad möglich,
- Mehrkernkonkurrenz reduziert,
- Speichergrenzen eingehalten.

Bei einem Überlauf können alte oder neue Ereignisse verworfen werden, abhängig vom Sessionmodus.

Die Verluststrategie darf den Kernelpfad nicht unbegrenzt blockieren.

---

## 12. Aufzeichnungsmodi

NovaOS unterstützt mindestens:

| Modus | Verhalten |
|---|---|
| `OVERWRITE` | älteste Ereignisse werden überschrieben |
| `STOP_ON_FULL` | Session stoppt bei vollem Buffer |
| `SNAPSHOT` | Ringpuffer wird bei Trigger eingefroren |
| `STREAM` | Reader liest fortlaufend, Verlust bleibt möglich |

Ein blockierender Modus für Kernel- oder Interruptpfade wird nicht unterstützt.

---

## 13. Zeitstempel und Reihenfolge

Trace-Ereignisse verwenden eine monotone, möglichst schnelle Zeitquelle.

Zur Rekonstruktion dienen:

- Zeitstempel
- globale oder vergleichbare Sequenznummer
- CPU-ID
- CPU-lokale Reihenfolge
- Korrelations-ID

Eine perfekte globale Ereignisreihenfolge wird auf Mehrkernsystemen nicht ohne zusätzlichen Aufwand garantiert.

Werkzeuge müssen Zeitquellenauflösung und mögliche Abweichungen berücksichtigen.

---

## 14. Korrelation und Spans

Zusammengehörige Vorgänge können über Korrelations- und Span-IDs verbunden werden.

Beispiel:

```text
RPC Request
├── IPC Send
├── Server Wake-up
├── Dateizugriff
└── RPC Response
```

Eine Trace-Korrelation enthält:

- Trace-ID
- Span-ID
- Parent-Span-ID
- Start- und Endereignis
- Status

Trace-IDs erteilen keine Rechte und dürfen nicht als Sicherheitsidentität verwendet werden.

---

## 15. Scheduler-Tracing

Der Scheduler stellt Tracepoints bereit für:

- Threaderzeugung und -ende
- Wake-up
- Blockierung
- Context Switch
- Prioritätsänderung
- CPU-Migration
- Deadline- oder Echtzeitereignisse
- Idle-Ein- und Austritt

Der deaktivierte Scheduler-Tracepoint muss besonders geringen Overhead besitzen.

Detailliertes Scheduler-Tracing ist zeitlich und speicherseitig zu begrenzen.

---

## 16. Interrupt- und Latenz-Tracing

Der Interrupt Manager kann erfassen:

- Interrupt-Eintritt
- Interrupt-Ende
- Vektor oder Diagnose-ID
- Handlerlaufzeit
- Deferred Work
- CPU
- verlorene oder maskierte Interrupts

Rohe Hardwareadressen und Kernelzeiger werden nicht aufgezeichnet.

Latenz-Tracing kann Trigger auslösen, wenn ein definierter Grenzwert überschritten wird.

---

## 17. I/O- und Netzwerk-Tracing

VFS-, Block- und Netzwerk-Tracepoints können enthalten:

- Operationstyp
- Objekt- oder Interface-ID
- Datenmenge
- Start- und Abschlusszeit
- Ergebnis
- Queue-Latenz
- Cachetreffer
- Protokolltyp

Standardmäßig nicht enthalten sind:

- Dateiinhalte
- vollständige Dateipfade
- Paketnutzdaten
- DNS-Namen
- TLS-Klartext
- private Netzwerkdaten

Detailliertere Felder benötigen eine besondere Trace-Capability.

---

## 18. Sampling

Neben Ereignis-Tracepoints unterstützt NovaOS zeit- oder ereignisbasiertes Sampling.

Mögliche Quellen:

- CPU-Timer
- Performance Counter
- Scheduler-Tick
- Hardwareprofiling-Ereignis

Ein Sample kann enthalten:

- CPU
- Prozess und Thread
- Modul-ID
- geschützten Instruction-Offset
- Stack-ID
- Zählerwert

Samplingraten sind begrenzt und werden bei Systemüberlastung automatisch reduziert.

---

## 19. Stacktraces

Optional können Tracepoints einen Stacktrace erfassen.

Im Produktionsmodus gilt:

- keine rohen Kerneladressen,
- Darstellung als Modul-ID und Offset,
- begrenzte Stacktiefe,
- keine Userspace-Speicherinhalte,
- Abbruch bei ungültigem Stack,
- erhöhte Rechte erforderlich.

Stacktracing ist erheblich teurer als ein gewöhnlicher Tracepoint und standardmäßig deaktiviert.

---

## 20. Filter

Eine Session kann Ereignisse filtern nach:

- Tracepoint-ID
- Kategorie
- CPU
- Prozess oder Thread
- Job Object
- Sicherheitsdomäne
- Gerät
- Interface
- Ergebnisstatus
- Dauer oder Latenz
- Samplingrate

Filter werden vor der Sessionaktivierung validiert und in eine schnelle interne Darstellung übersetzt.

Unbegrenzte Filterprogramme sind nicht zulässig.

---

## 21. Trigger

Eine Snapshot-Session kann durch Ereignisse ausgelöst werden:

- Kernelwarnung
- Latenzüberschreitung
- Treiber-Timeout
- Paketverlustgrenze
- Dateisystemfehler
- Thermal Trip Point
- Suspend- oder Resume-Fehler
- manueller Diagnosebefehl

Der Trigger friert den relevanten Buffer ein oder verlängert eine begrenzte Nachlaufphase.

Trigger dürfen keine rekursiven Trace-Ereignisschleifen erzeugen.

---

## 22. Capability-Rechte

| Recht | Bedeutung |
|---|---|
| `TRACE_RIGHT_CREATE` | begrenzte Trace Session erzeugen |
| `TRACE_RIGHT_KERNEL` | Kernel-Tracepoints aktivieren |
| `TRACE_RIGHT_PROCESS` | eigene oder autorisierte Prozesse verfolgen |
| `TRACE_RIGHT_SYSTEM` | systemweite Trace Session starten |
| `TRACE_RIGHT_STACK` | Stacktraces erfassen |
| `TRACE_RIGHT_SENSITIVE` | geschützte Tracefelder lesen |
| `TRACE_RIGHT_EXPORT` | Trace ausdrücklich exportieren |
| `TRACE_RIGHT_ADMIN` | Tracing-Infrastruktur verwalten |

Gewöhnliche Anwendungen dürfen nur eigene, ausdrücklich freigegebene Ereignisse verfolgen.

---

## 23. Ressourcenbegrenzung

Begrenzt werden mindestens:

- Sessions pro Benutzer und System
- Buffergröße pro CPU
- Ereignisgröße
- Samplingrate
- Stacktiefe
- Filterkomplexität
- Sessiondauer
- Anzahl aktivierter Tracepoints
- persistente Tracegröße
- Readeranzahl

Systemkritische Komponenten dürfen Tracing bei Ressourcenknappheit reduzieren oder beenden.

Tracing darf keine Memory-Reclaim- oder I/O-Deadlocks verursachen.

---

## 24. Datenschutz

Tracing kann detaillierte Verhaltensmuster sichtbar machen.

Deshalb gilt:

- systemweites Tracing benötigt besondere Rechte,
- Dateipfade werden standardmäßig gekürzt oder durch Objekt-IDs ersetzt,
- Netzwerkziele werden minimiert oder maskiert,
- Inhalte werden nicht aufgezeichnet,
- Trace Sessions besitzen eine begrenzte Lebenszeit,
- persistente Traces werden verschlüsselt oder zugriffsgeschützt gespeichert,
- externer Export ist standardmäßig deaktiviert,
- Benutzer müssen über dauerhaft aktiviertes Diagnosetracing informiert werden.

---

## 25. Sicherheit

Für Tracing gelten folgende Regeln:

- Tracepoints dürfen Kernelzustände nicht verändern.
- Schreiben in Tracebuffer ist nicht blockierend.
- Kernelzeiger werden im Produktionsmodus geschützt.
- Trace Reader benötigen Capabilities.
- Userspace kann keine Kernelereignisse fälschen.
- dynamisches Tracing bleibt verifiziert und beschränkt.
- Stacktracing benötigt zusätzliche Rechte.
- Trace-Daten dürfen keine Schlüssel oder Klartextinhalte enthalten.
- Tracing darf Audit und Secure Debugging nicht umgehen.
- Überlastung führt zu Traceverlust oder Sessionende, nicht zu Kernelblockierung.

---

## 26. Verlust und Genauigkeit

Trace-Ereignisse können verloren gehen durch:

- Bufferüberlauf
- deaktivierte Tracepoints
- Sampling
- CPU-Hotplug
- Sessionstart oder -ende
- Systemüberlastung
- Panic

Verlust wird sichtbar gemacht durch:

- Sequenzlücken
- Drop-Zähler
- Buffer-Overrun-Marker
- Sessionstatus

Tracing darf keine vollständige Aufzeichnung behaupten, wenn Ereignisse verloren wurden.

---

## 27. Export und Speicherung

Trace-Daten werden standardmäßig nur lokal verarbeitet.

Mögliche Ausgabeformate:

- binäres NovaTrace-Format
- menschenlesbarer Diagnosebericht
- standardisiertes Austauschformat über ein Userspace-Werkzeug

Persistente Speicherung erfolgt nicht direkt durch den Kernel.

Ein privilegierter Diagnosedienst liest, validiert, redigiert und speichert die Daten.

Netzwerkexport benötigt ausdrückliche Autorisierung und verschlüsselte Übertragung.

---

## 28. Fehlercodes

| Fehlercode | Bedeutung |
|---|---|
| `STATUS_OK` | Operation erfolgreich |
| `STATUS_TRACEPOINT_UNKNOWN` | Tracepoint existiert nicht |
| `STATUS_TRACE_SESSION_FULL` | Sessionbuffer ist voll |
| `STATUS_TRACE_OVERRUN` | Ereignisse wurden überschrieben |
| `STATUS_TRACE_FILTER_INVALID` | Filter ist ungültig |
| `STATUS_TRACE_RATE_TOO_HIGH` | Samplingrate ist nicht zulässig |
| `STATUS_TRACE_SESSION_EXPIRED` | Sessiondauer ist abgelaufen |
| `STATUS_ACCESS_DENIED` | erforderliche Capability fehlt |
| `STATUS_LIMIT_EXCEEDED` | Ressourcenlimit wurde überschritten |
| `STATUS_NOT_SUPPORTED` | Tracefunktion wird nicht unterstützt |

---

## 29. Alternative Ansätze

### 29.1 Ausschließlich Kernel Logging

**Vorteile:**

- nur ein Diagnosesystem erforderlich

**Nachteile:**

- ungeeignet für hochfrequente zeitliche Abläufe
- schwer korrelierbar

**Entscheidung:** Kernel Logging und Tracing bleiben getrennt.

### 29.2 Tracing immer vollständig aktiv

**Vorteile:**

- Fehlerhistorie ist jederzeit verfügbar

**Nachteile:**

- hoher Speicher- und Performanceaufwand
- erhebliche Datenschutzprobleme

**Entscheidung:** Nur minimale Tracepoints sind standardmäßig aktiv.

### 29.3 Beliebiges dynamisches Kernel-Patching

**Vorteile:**

- maximale Flexibilität

**Nachteile:**

- hohe Sicherheits- und Stabilitätsrisiken

**Entscheidung:** Im Produktionsmodus abgelehnt.

---

## 30. Konsequenzen

### Positive Konsequenzen

- detaillierte Performance- und Latenzanalyse
- strukturierte systemübergreifende Korrelation
- geringer Overhead deaktivierter Tracepoints
- Mehrkernskalierung durch CPU-lokale Buffer
- Snapshot- und Trigger-Unterstützung
- kontrolliertes Sampling
- lokale, capability-geschützte Diagnose

### Negative Konsequenzen

- Trace-Schemata müssen langfristig gepflegt werden
- Ereignisverlust ist unter Last möglich
- Stacktracing und Sampling können teuer sein
- mehrere CPU-Buffer erschweren die globale Reihenfolge

### Risiken

- sensible Daten in Tracefeldern
- hoher Overhead durch zu viele aktive Tracepoints
- Bufferüberläufe
- falsche Zeitkorrelation
- missbräuchliches systemweites Tracing
- Verifierfehler bei späterem dynamischem Tracing

---

## 31. Implementierungsrichtlinien

- Die erste Implementierung verwendet primär statische Tracepoints.
- Deaktivierte Tracepoints besitzen minimalen Laufzeitaufwand.
- Aktive Tracepoints schreiben nicht blockierend in CPU-lokale Buffer.
- Tracepoint-IDs und Feldschemata sind versioniert.
- Sessions besitzen feste Laufzeit- und Speicherlimits.
- Filter werden vor Aktivierung vollständig validiert.
- Kernelzeiger werden als Modul-ID und Offset dargestellt.
- Dateiinhalte, Paketnutzdaten und Schlüssel dürfen nicht aufgezeichnet werden.
- Traceverlust muss durch Zähler oder Sequenzlücken erkennbar sein.
- Persistente Speicherung und Export erfolgen außerhalb des Kernels.
- Tracing ersetzt weder Logging noch Audit.

---

## 32. Testanforderungen

Die Implementierung benötigt mindestens Tests für:

- statische Tracepoints
- deaktivierten Tracepoint-Overhead
- mehrere parallele Sessions
- CPU-lokale Ringpuffer
- Bufferüberlauf
- Snapshot und Trigger
- Filter
- Sampling
- Stacktraces
- Mehrkernreihenfolge
- CPU-Hotplug
- Session-Timeout
- Capability-Rechte
- Zeiger- und Datenschutzredaktion
- Suspend, Hibernate und Resume
- persistente Userspace-Ausgabe

---

## 33. Verbindliche Invarianten

1. Tracing und Kernel Logging bleiben getrennte Subsysteme.
2. Tracing ersetzt kein sicherheitsrelevantes Audit.
3. Trace Sessions sind capability-geschützt und ressourcenbegrenzt.
4. Der aktive Schreibpfad darf nicht blockieren.
5. Jede CPU verwendet begrenzte lokale Tracebuffer.
6. Tracepoint-IDs und Schemas bleiben versioniert.
7. Tracepoints dürfen den beobachteten Kernelzustand nicht verändern.
8. Rohe Kernelzeiger erscheinen nicht in Produktions-Traces.
9. Schlüssel, Dateiinhalte und Paketnutzdaten werden nicht standardmäßig aufgezeichnet.
10. Verlorene Trace-Ereignisse werden sichtbar gekennzeichnet.

---

## 34. Zusammenfassung

NovaOS implementiert ein strukturiertes Tracing-System mit statischen Tracepoints, CPU-lokalen Ringpuffern, Sampling, Filtern und Snapshot-Triggern.

Tracing wird über zeitlich und räumlich begrenzte Sessions aktiviert. Deaktivierte Tracepoints verursachen nur minimalen Overhead, während aktive Tracepoints nicht blockierend arbeiten.

Capability-Rechte, geschützte Kerneladressen und datensparsame Felder ermöglichen detaillierte Performanceanalyse, ohne Tracing zu einem dauerhaften Überwachungs- oder Sicherheitsrisiko werden zu lassen.