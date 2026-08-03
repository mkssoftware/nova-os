# NPSPEC-KERNEL-0023 – Logging Framework

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-KERNEL-0023 |
| Titel | Logging Framework |
| Status | Spezifikation |
| Version | 1.0 |
| Datum | 2026-08-03 |
| Bereich | Kernel / Diagnose |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | NPSPEC-KERNEL-0002, NPSPEC-KERNEL-0012, NPSPEC-KERNEL-0014, NPSPEC-KERNEL-0015, NPSPEC-KERNEL-0020 |
| Zugehörige ADRs | ADR-DIAG-0001, ADR-DIAG-0002, ADR-DIAG-0003, ADR-SEC-0008, ADR-SEC-0010, ADR-KERNEL-0104 |

---

## 1. Zweck

Diese Spezifikation definiert das Logging Framework des NovaOS-Kernels.

Das Framework stellt strukturierte und lokal kontrollierte Protokollierung bereit für:

- frühe Bootphasen,
- Kernelkomponenten,
- Treiber,
- Systemdienste,
- Sicherheitsereignisse,
- Fehler und Kernel Panic,
- Performance- und Diagnoseinformationen.

## 2. Ziele

Das Logging Framework muss:

- bereits während des frühen Bootvorgangs verfügbar sein,
- strukturierte Logeinträge unterstützen,
- auf Mehrprozessorsystemen skalieren,
- im Interrupt- und Panic-Kontext eingeschränkt funktionieren,
- sensible Daten schützen,
- Rate Limiting und Duplikatunterdrückung bereitstellen,
- Logs standardmäßig lokal halten,
- mehrere Ausgabekanäle unterstützen,
- Logverlust und Pufferüberlauf eindeutig anzeigen.

## 3. Abgrenzung

| Komponente | Verantwortung |
|---|---|
| Logging Framework | strukturierte Logeinträge und Puffer |
| Tracing Framework | hochfrequente Ablauf- und Ereignisverfolgung |
| Audit Subsystem | manipulationsgeschützte Sicherheitsereignisse |
| Crash Dump | persistente Absturzdiagnose |
| Metrics Framework | numerische Messwerte und Zeitreihen |
| Userspace Log Service | Speicherung, Rotation und Anzeige |
| Secure Debugging | Schutz detaillierter Debuginformationen |

Logging darf nicht als Ersatz für Audit oder Tracing verwendet werden.

## 4. Logging-Phasen

NovaOS unterscheidet:

| Phase | Verfügbare Funktionen |
|---|---|
| Early Boot | statische Puffer und direkte Konsole |
| Kernel Initialization | globaler Boot-Log-Puffer |
| SMP Operational | CPU-lokale Ringpuffer |
| Userspace Operational | Übergabe an lokalen Log Service |
| Panic | reservierter nicht blockierender Notfallpfad |
| Recovery | eingeschränkte verifizierte Logausgabe |

Einträge aus früheren Phasen müssen soweit möglich in das reguläre System übernommen werden.

## 5. Schweregrade

```c
typedef enum np_log_level {
    NP_LOG_TRACE,
    NP_LOG_DEBUG,
    NP_LOG_INFO,
    NP_LOG_NOTICE,
    NP_LOG_WARNING,
    NP_LOG_ERROR,
    NP_LOG_CRITICAL,
    NP_LOG_FATAL
} np_log_level_t;
```

| Stufe | Bedeutung |
|---|---|
| `TRACE` | sehr detaillierter Ablauf |
| `DEBUG` | Entwicklerdiagnose |
| `INFO` | normaler Betriebszustand |
| `NOTICE` | relevantes, aber erwartetes Ereignis |
| `WARNING` | ungewöhnlicher oder reduzierter Zustand |
| `ERROR` | fehlgeschlagene Operation |
| `CRITICAL` | kritisches Subsystemproblem |
| `FATAL` | nicht fortsetzbarer Fehler |

## 6. Standardfilter

Empfohlene Standardfilter sind:

| Build-Profil | Mindeststufe |
|---|---|
| Development | `DEBUG` |
| Test | `DEBUG` |
| Release | `INFO` |
| Hardened | `NOTICE` |
| Recovery | `INFO` |
| Panic | alle verfügbaren kritischen Einträge |

`TRACE` wird standardmäßig nur gezielt für ausgewählte Komponenten aktiviert.

## 7. Logkategorien

```c
typedef enum np_log_category {
    NP_LOG_CATEGORY_BOOT,
    NP_LOG_CATEGORY_KERNEL,
    NP_LOG_CATEGORY_MEMORY,
    NP_LOG_CATEGORY_PROCESS,
    NP_LOG_CATEGORY_SCHEDULER,
    NP_LOG_CATEGORY_IPC,
    NP_LOG_CATEGORY_DEVICE,
    NP_LOG_CATEGORY_DRIVER,
    NP_LOG_CATEGORY_VFS,
    NP_LOG_CATEGORY_NETWORK,
    NP_LOG_CATEGORY_POWER,
    NP_LOG_CATEGORY_SECURITY,
    NP_LOG_CATEGORY_DIAGNOSTIC,
    NP_LOG_CATEGORY_RECOVERY
} np_log_category_t;
```

Komponenten können zusätzlich registrierte Unterkategorien verwenden.

## 8. Komponentenkennung

```c
typedef uint32_t np_log_component_id_t;
```

Jede Kernelkomponente und jeder registrierte Treiber erhält eine stabile Komponentenkennung.

Die Kennung wird für:

- Filterung,
- Rate Limiting,
- Diagnose,
- Statistiken,
- Darstellung

verwendet.

Sie ist keine Sicherheitsidentität.

## 9. Event-Code

Jeder strukturierte Logeintrag besitzt einen Event-Code.

```c
typedef uint32_t np_log_event_code_t;
```

Ein Event-Code beschreibt eine konkrete Ereignisklasse, beispielsweise:

```text
MEMORY_ALLOCATION_FAILED
DRIVER_BINDING_FAILED
FILESYSTEM_READ_ONLY
SECURITY_ACCESS_DENIED
CLOCKSOURCE_UNSTABLE
```

Veröffentlichte Event-Codes dürfen nicht mit einer anderen Bedeutung wiederverwendet werden.

## 10. Logeintrag

```c
typedef struct np_log_record_header {
    uint16_t version;
    uint16_t header_size;
    uint32_t total_size;

    np_log_level_t level;
    np_log_category_t category;
    np_log_component_id_t component_id;
    np_log_event_code_t event_code;

    np_time_ns_t timestamp;
    uint64_t sequence_number;

    np_cpu_id_t cpu_id;
    np_process_id_t process_id;
    np_thread_id_t thread_id;

    uint32_t flags;
    uint32_t field_count;
} np_log_record_header_t;
```

Auf die Kopfstruktur folgen typisierte Felder.

## 11. Typisierte Felder

```c
typedef enum np_log_field_type {
    NP_LOG_FIELD_BOOLEAN,
    NP_LOG_FIELD_SIGNED,
    NP_LOG_FIELD_UNSIGNED,
    NP_LOG_FIELD_STRING,
    NP_LOG_FIELD_BINARY,
    NP_LOG_FIELD_UUID,
    NP_LOG_FIELD_STATUS,
    NP_LOG_FIELD_ADDRESS,
    NP_LOG_FIELD_DURATION
} np_log_field_type_t;
```

Jedes Feld besitzt:

- Feldkennung,
- Datentyp,
- Länge,
- Flags,
- Wert.

Freitext darf strukturierte Felder ergänzen, aber nicht vollständig ersetzen.

## 12. Feldflags

```c
typedef enum np_log_field_flags {
    NP_LOG_FIELD_SENSITIVE   = 1U << 0,
    NP_LOG_FIELD_PERSONAL    = 1U << 1,
    NP_LOG_FIELD_SECRET      = 1U << 2,
    NP_LOG_FIELD_ADDRESS_HASHED = 1U << 3,
    NP_LOG_FIELD_TRUNCATED   = 1U << 4
} np_log_field_flags_t;
```

`SECRET`-Felder dürfen nicht in reguläre Logs geschrieben werden.

`SENSITIVE`- und `PERSONAL`-Felder unterliegen zusätzlichen Richtlinien.

## 13. Logging-API

```c
np_status_t np_log_write(
    const np_log_event_t* event
);
```

Komfortfunktionen können bereitgestellt werden:

```c
void np_log_info(
    np_log_component_id_t component,
    np_log_event_code_t event_code,
    const char* format,
    ...
);

void np_log_error(
    np_log_component_id_t component,
    np_log_event_code_t event_code,
    np_status_t status
);
```

Strukturierte APIs werden gegenüber frei formatierten Zeichenketten bevorzugt.

## 14. Formatstrings

Formatierte Logging-Funktionen dürfen nur eine begrenzte und kontrollierte Formatierung unterstützen.

Zu verhindern sind:

- Formatstring-Injektion,
- unkontrollierte Speicherzugriffe,
- unbegrenzte Ausgabegrößen,
- Fließkommaabhängigkeiten in frühen Bootphasen,
- Heapallokationen im Interrupt- oder Panic-Kontext.

Dynamische Benutzereingaben dürfen nicht als Formatstring verwendet werden.

## 15. Early Logging

Die Early-Logging-API benötigt:

- keine reguläre Heapallokation,
- keinen Scheduler,
- keine IPC,
- keine Dateisysteme,
- keine schlaffähigen Sperren.

```c
void np_early_log(
    np_log_level_t level,
    const char* message
);
```

Einträge werden in einen statischen Bootpuffer und optional auf eine frühe Konsole geschrieben.

## 16. Early-Log-Puffer

Der frühe Logpuffer ist:

- statisch oder durch den Early Allocator reserviert,
- größenbegrenzt,
- vor regulärer Speicherfreigabe geschützt,
- nach Initialisierung in das normale Logging überführbar.

Bei Überlauf wird ein Zähler verworfener Einträge geführt.

## 17. CPU-lokale Ringpuffer

Im normalen SMP-Betrieb besitzt jede CPU einen eigenen Ringpuffer.

```c
typedef struct np_log_ring_buffer {
    uint8_t* data;
    size_t capacity;

    np_atomic_uint64_t write_position;
    np_atomic_uint64_t read_position;
    np_atomic_uint64_t dropped_records;

    uint32_t flags;
} np_log_ring_buffer_t;
```

CPU-lokale Puffer reduzieren globale Sperrkonflikte.

## 18. Ringpuffer-Semantik

Ein Logeintrag wird atomar sichtbar gemacht.

Der Leser darf niemals einen teilweise geschriebenen Eintrag als vollständig interpretieren.

Mögliche Strategien sind:

- Reserve- und Commit-Position,
- Eintragsstatus,
- Sequenzzähler,
- Längenfeld mit atomarer Veröffentlichung.

Ein beschädigter Eintrag muss übersprungen und diagnostiziert werden können.

## 19. Pufferüberlauf

Bei vollem Ringpuffer kann abhängig von Kontext und Richtlinie:

- der älteste nicht kritische Eintrag überschrieben,
- der neue Eintrag verworfen,
- ein reservierter Kritisch-Puffer verwendet,
- der Userspace Log Service geweckt werden.

Logging darf im normalen Kernelpfad nicht unbegrenzt blockieren.

Die Zahl verlorener Einträge wird ausdrücklich gemeldet.

## 20. Kritischer Reservepuffer

Für `CRITICAL`- und `FATAL`-Einträge wird ein kleiner reservierter Puffer geführt.

Dieser Puffer darf nicht durch `TRACE`, `DEBUG` oder normale `INFO`-Einträge verdrängt werden.

Er muss im Panic-Pfad ohne Heapallokation zugänglich sein.

## 21. Zeitstempel

Logeinträge verwenden monotone Kernelzeit.

Optional kann der Userspace Log Service eine Echtzeitzuordnung ergänzen.

Die Verwendung monotoner Zeit verhindert eine falsche Reihenfolge durch Änderungen der Systemzeit.

Vor vollständiger Timerinitialisierung können Einträge einen frühen Hardwarezähler und das Flag `EARLY_TIMESTAMP` verwenden.

## 22. Sequenznummern

Jeder Eintrag erhält eine Sequenznummer.

Eine streng globale Nummer ist nicht zwingend, wenn sie den Fast Path zu stark serialisieren würde.

Alternativ können verwendet werden:

- CPU-lokale Sequenznummern,
- Bootgeneration,
- Zeitstempel,
- CPU-Kennung.

Der Log Service führt daraus eine bestmögliche Gesamtordnung zusammen.

## 23. Kontextinformationen

Soweit sicher verfügbar, enthält ein Logeintrag:

- CPU-Kennung,
- Prozesskennung,
- Threadkennung,
- Sicherheitsdomain,
- Treiber- oder Gerätekennung,
- Bootphase,
- Interrupt- oder Panic-Kontext.

Im frühen Boot- oder NMI-Kontext können einzelne Felder fehlen.

Fehlende Werte müssen eindeutig markiert werden.

## 24. Logging im Interruptkontext

Direkte Interrupt Handler dürfen nur die nicht blockierende Logging-API verwenden.

Diese darf:

- keine schlaffähige Sperre erwerben,
- keine reguläre Heapallokation durchführen,
- keinen Userspace-Dienst direkt aufrufen,
- keine umfangreiche Formatierung ausführen.

Häufige Interruptmeldungen müssen rate-limitiert oder als Zähler erfasst werden.

## 25. Logging im NMI-Kontext

NMI- und Machine-Check-Kontexte verwenden einen besonders eingeschränkten CPU-lokalen Notfallpuffer.

Zulässig sind nur:

- feste Event-Codes,
- begrenzte numerische Felder,
- kurze statische Texte,
- keine normalen Sperren,
- keine dynamischen Speicherzugriffe.

NMI-Logging darf reguläres Logging nicht rekursiv betreten.

## 26. Panic Logging

Im Panic-Zustand wechselt das Framework in einen minimalen synchronen Modus.

Es verwendet:

- reservierte Puffer,
- frühe Konsole,
- serielle Ausgabe, falls verfügbar,
- Crash-Dump-Integration,
- keine reguläre Heapallokation,
- keine Abhängigkeit von normalen Kernelthreads.

Andere CPUs können angehalten werden, bevor ihre letzten Puffer gesichert werden.

## 27. Rekursionserkennung

Logging kann selbst während eines Fehlers ausgelöst werden.

Daher verwaltet jede CPU einen Rekursionszähler.

Bei erkannter Rekursion:

- wird Formatierung reduziert,
- werden optionale Ausgabekanäle übersprungen,
- wird ein Notfallpuffer verwendet,
- wird die Tiefe begrenzt.

Unbegrenzte rekursive Logs sind unzulässig.

## 28. Log Sinks

Ein Log Sink verarbeitet oder überträgt Einträge an ein Ziel.

```c
typedef enum np_log_sink_type {
    NP_LOG_SINK_RING_BUFFER,
    NP_LOG_SINK_SERIAL,
    NP_LOG_SINK_FRAMEBUFFER,
    NP_LOG_SINK_DEBUG_PORT,
    NP_LOG_SINK_USERSPACE,
    NP_LOG_SINK_CRASH_DUMP,
    NP_LOG_SINK_RECOVERY
} np_log_sink_type_t;
```

Nicht jeder Sink ist in jeder Bootphase verfügbar.

## 29. Sink-Schnittstelle

```c
typedef struct np_log_sink_operations {
    np_status_t (*write)(
        void* context,
        const np_log_record_view_t* record
    );

    np_status_t (*flush)(
        void* context
    );

    void (*shutdown)(
        void* context
    );
} np_log_sink_operations_t;
```

Ein langsamer Sink darf den normalen Kernel-Logging-Pfad nicht dauerhaft blockieren.

## 30. Userspace Log Service

Nach dem Userspace-Start übernimmt ein lokaler Log Service:

- Auslesen der Kernelringpuffer,
- Zusammenführen der CPU-Ströme,
- zusätzliche Filterung,
- persistente Speicherung,
- Rotation,
- Kompression,
- Suchindex,
- Zugriffssteuerung,
- Anzeige für Diagnosetools.

Der Kernel bleibt auch bei Ausfall des Dienstes funktionsfähig.

## 31. Log-Stream-Objekt

Der Kernel stellt Logs über ein wartbares Streamobjekt bereit.

```c
np_status_t np_log_stream_open(
    const np_log_filter_t* filter,
    np_handle_t* stream
);

np_status_t np_log_stream_read(
    np_handle_t stream,
    void* output_buffer,
    size_t output_size,
    size_t* bytes_read
);
```

Der Zugriff erfordert passende Diagnose-Capabilities.

## 32. Logfilter

```c
typedef struct np_log_filter {
    np_log_level_t minimum_level;

    np_log_category_mask_t categories;
    np_component_filter_t components;
    np_cpu_set_t cpus;

    np_process_id_t process_id;
    np_driver_id_t driver_id;

    uint32_t flags;
} np_log_filter_t;
```

Filter werden sowohl beim Schreiben als auch beim Lesen eingesetzt, abhängig von Performance- und Sicherheitsanforderungen.

## 33. Dynamische Filterung

Autorisierte Diagnosedienste können Logstufen für einzelne Komponenten zur Laufzeit verändern.

```c
np_status_t np_log_set_component_level(
    np_log_component_id_t component,
    np_log_level_t minimum_level,
    np_time_ns_t duration
);
```

Zeitlich begrenzte Debugaktivierung wird gegenüber unbegrenzter Aktivierung bevorzugt.

Änderungen geschützter Komponenten werden auditiert.

## 34. Rate Limiting

Das Framework begrenzt wiederholte Logs nach:

- Event-Code,
- Komponente,
- CPU,
- Prozess,
- Treiber,
- Zeitfenster.

Eine mögliche Richtlinie ist Token Bucket.

Unterdrückte Wiederholungen werden später zusammengefasst:

```text
Ereignis X wurde 12.481-mal unterdrückt
```

## 35. Duplikatunterdrückung

Identische oder semantisch gleiche Einträge können innerhalb eines kurzen Fensters zusammengefasst werden.

Die Zusammenfassung muss erhalten:

- erstes Auftreten,
- letztes Auftreten,
- Wiederholungszahl,
- höchste beobachtete Schwere,
- relevante Kontextänderungen.

`FATAL`-Einträge werden nicht vollständig unterdrückt.

## 36. Größenlimits

Jeder Logeintrag besitzt eine maximale Größe.

Empfohlene Grenzen sind:

| Bereich | Standardgrenze |
|---|---:|
| Logeintrag gesamt | 16 KiB |
| Freitext | 4 KiB |
| Einzelnes Binärfeld | 4 KiB |
| Feldanzahl | 64 |
| Stacktrace | 64 Frames |

Größere Daten werden gekürzt oder über eine separate Diagnose-API bereitgestellt.

## 37. Binärdaten

Binärdaten werden nur protokolliert, wenn sie für die Diagnose erforderlich und nicht geheim sind.

Nicht regulär in Logs gehören:

- vollständige Netzwerkpakete,
- Datei- oder IPC-Payloads,
- Speicherabbilder,
- Schlüsselmaterial,
- Kennwörter,
- Authentifizierungstokens.

Für Packet Capture oder Memory Dumps existieren separate capability-geschützte Systeme.

## 38. Adressschutz

Kerneladressen können ASLR und andere Schutzmechanismen schwächen.

Daher werden Adressfelder:

- in normalen Logs entfernt,
- relativ zu Modul-Basen dargestellt,
- gehasht oder tokenisiert,
- nur im autorisierten Secure-Debugging-Modus vollständig ausgegeben.

Unprivilegierte Leser erhalten keine vollständigen Kerneladressen.

## 39. Stacktraces

Stacktraces können bei Fehlern optional erfasst werden.

Voraussetzungen sind:

- sicherer Stackzustand,
- begrenzte maximale Tiefe,
- geeigneter Ausführungskontext,
- Secure-Debugging- und Datenschutzrichtlinie.

Ein beschädigter Stack darf nicht durch unbegrenzte Stackanalyse weiter gefährdet werden.

## 40. Sicherheitslogs

Sicherheitsrelevante Logeinträge können an das Audit Subsystem weitergeleitet werden.

Logging und Audit bleiben getrennt:

- Logging ist für Diagnose optimiert,
- Audit ist manipulationsgeschützt und richtliniengebunden,
- Logverlust darf nicht automatisch unbemerkt Auditverlust bedeuten.

Ein Auditereignis darf nicht ausschließlich von einem normalen Logeintrag abhängen.

## 41. Treiberlogging

Treiber verwenden eine registrierte Komponenten- und Instanzkennung.

Ein Treiberlog enthält soweit möglich:

- Driver ID,
- Instance ID,
- Device ID,
- Event-Code,
- Status,
- Request ID.

Userspace-Treiber senden strukturierte Logs über den Driver Host oder einen capability-geschützten Logendpunkt.

## 42. Prozess- und Anwendungslogs

Userspace-Anwendungen schreiben nicht direkt in Kernelringpuffer.

Sie verwenden den lokalen Log Service oder einen kontrollierten IPC-Endpunkt.

Systemdienste können strukturierte Komponentenkennungen erhalten.

Der Kernel darf Userspace-Logdaten nicht als vertrauenswürdige Kernelereignisse kennzeichnen.

## 43. Persistente Speicherung

Der Userspace Log Service speichert Logs in einem dafür vorgesehenen Datenbereich.

Die Speicherung muss unterstützen:

- Rotation,
- Größenlimits,
- Aufbewahrungsfristen,
- Kompression,
- Integritätsmetadaten,
- getrennte Sicherheitsklassen,
- sichere Löschung nach Richtlinie.

Ein volles Logvolume darf nicht das Root-Dateisystem unkontrolliert füllen.

## 44. Logrotation

Rotation kann ausgelöst werden durch:

- Dateigröße,
- Zeitintervall,
- Gesamtvolumen,
- Systemgeneration,
- Bootvorgang,
- Sicherheitsereignis.

Alte Logs werden entsprechend der Aufbewahrungsrichtlinie komprimiert oder gelöscht.

Auditdaten besitzen eine getrennte Rotation und Aufbewahrung.

## 45. Bootübergreifende Zuordnung

Jeder Bootvorgang erhält eine Boot ID.

```c
typedef np_uuid_t np_boot_id_t;
```

Persistierte Einträge enthalten:

- Boot ID,
- Systemgeneration,
- Kernel Build ID,
- Logformatversion.

Dadurch können Logs verschiedener Starts eindeutig getrennt werden.

## 46. Logformat

Das interne Logformat ist binär und versioniert.

Für Export und Darstellung können verwendet werden:

- menschenlesbarer Text,
- JSON,
- CBOR,
- ein NovaOS-Diagnoseformat.

Der Kernel schreibt keine formatabhängigen externen Exportdateien direkt.

## 47. Logformat-Versionierung

Jeder Record enthält:

- Formatversion,
- Headergröße,
- Gesamtgröße,
- Feature- oder Record-Flags.

Ein Leser muss unbekannte kompatible Felder überspringen können.

Beschädigte oder inkompatible Records werden nicht ungeprüft interpretiert.

## 48. Capability-Rechte

Mindestens folgende Rechte werden definiert:

| Capability | Bedeutung |
|---|---|
| `CAP_LOG_WRITE` | Logs über autorisierten Endpunkt schreiben |
| `CAP_LOG_READ_SELF` | eigene Prozess- oder Komponentendaten lesen |
| `CAP_LOG_READ_SYSTEM` | allgemeine Systemlogs lesen |
| `CAP_LOG_READ_SECURITY` | geschützte Sicherheitslogs lesen |
| `CAP_LOG_CONFIGURE` | Filter und Logstufen verändern |
| `CAP_LOG_CLEAR` | persistente Logs nach Richtlinie löschen |
| `CAP_LOG_EXPORT` | Logs kontrolliert exportieren |
| `CAP_LOG_ADMIN` | globale Loggingrichtlinien verwalten |

Rechte können nach Kategorie, Komponente, Prozess und Zeitraum begrenzt werden.

## 49. Sicherheit

Das Logging Framework muss sicherstellen, dass:

- Userspace Kernelringpuffer nicht verändern kann,
- Loggrößen und Feldanzahlen begrenzt sind,
- Formatstrings nicht aus ungeprüften Daten stammen,
- Kerneladressen standardmäßig verborgen werden,
- Geheimnisse nicht in regulären Logs erscheinen,
- Zugriff auf Logs Capability-Prüfungen durchläuft,
- Logfilter Sicherheitsgrenzen nicht umgehen,
- ein fehlerhafter Sink den Kernel nicht dauerhaft blockiert,
- Panic Logging ohne unsichere Abhängigkeiten funktioniert,
- manipulierte persistente Logs nicht als vertrauenswürdige Auditdaten gelten.

## 50. Datenschutz

Logging folgt dem Prinzip der Datenminimierung.

Es gelten:

- keine vollständigen Inhalte von Dateien, Nachrichten oder Netzwerkpaketen,
- keine Kennwörter, Schlüssel oder Tokens,
- personenbezogene Felder ausdrücklich kennzeichnen,
- standardmäßig lokale Verarbeitung,
- konfigurierbare Aufbewahrungsfristen,
- kontrollierter Export,
- rollen- und capability-basierter Zugriff,
- Möglichkeit zur Pseudonymisierung oder Kürzung sensibler Werte.

## 51. Diagnose des Logging Frameworks

Das Framework protokolliert eigene Zustände nur über einen rekursionssicheren internen Pfad.

Diagnostizierbar sind:

- Pufferbelegung,
- verworfene Records,
- Rekursionen,
- Sink-Fehler,
- Rate-Limit-Ereignisse,
- beschädigte Records,
- Userspace-Consumer-Lag,
- Persistenzfehler.

Eigendiagnose darf keine unendliche Logschleife erzeugen.

## 52. Statistik

```c
typedef struct np_logging_statistics {
    uint64_t written_records;
    uint64_t written_bytes;

    uint64_t dropped_records;
    uint64_t overwritten_records;
    uint64_t rate_limited_records;
    uint64_t duplicate_records;

    uint64_t malformed_records;
    uint64_t sink_failures;
    uint64_t recursion_events;

    uint64_t critical_records;
    uint64_t fatal_records;
} np_logging_statistics_t;
```

Statistiken sollen pro CPU, Komponente, Schweregrad und System verfügbar sein.

## 53. Fehlercodes

| Fehlercode | Bedeutung |
|---|---|
| `NP_OK` | Operation erfolgreich |
| `NP_ERR_INVALID_ARGUMENT` | ungültiger Parameter |
| `NP_ERR_LOG_TOO_LARGE` | Eintrag überschreitet die Größenbegrenzung |
| `NP_ERR_LOG_MALFORMED` | Record oder Feldstruktur ist ungültig |
| `NP_ERR_LOG_BUFFER_FULL` | Puffer kann den Eintrag nicht aufnehmen |
| `NP_ERR_LOG_RATE_LIMITED` | Eintrag wurde begrenzt |
| `NP_ERR_LOG_SINK_FAILED` | Ausgabekanal ist fehlgeschlagen |
| `NP_ERR_ACCESS_DENIED` | erforderliche Capability fehlt |
| `NP_ERR_RECURSION` | unzulässige Logging-Rekursion erkannt |
| `NP_ERR_NOT_SUPPORTED` | Funktion wird nicht unterstützt |

Kernelinterne Komfortfunktionen dürfen Fehler still zählen, wenn ihr Aufrufer keinen Status behandeln kann.

## 54. Fehlerbehandlung

Der Ausfall eines normalen Log Sinks darf den Kernelbetrieb nicht stoppen.

Mögliche Reaktionen sind:

- Sink deaktivieren,
- auf Ringpuffer begrenzen,
- alternativen Sink verwenden,
- verworfene Einträge zählen,
- lokale Warnung über den Reservepuffer ausgeben.

Fehler des Logging Frameworks dürfen nur dann eine Kernel Panic auslösen, wenn sie auf eine breitere Kernelkorruption hindeuten.

## 55. Testanforderungen

Mindestens folgende Tests sind erforderlich:

1. Early Logging vor Heapinitialisierung,
2. Übernahme früher Logs in den regulären Puffer,
3. strukturierter Logeintrag,
4. Freitext-Logeintrag,
5. alle Schweregrade,
6. Kategorie- und Komponentenfilter,
7. CPU-lokale Ringpuffer,
8. paralleles Logging auf mehreren CPUs,
9. atomare Record-Veröffentlichung,
10. Pufferüberlauf,
11. Zähler verworfener Einträge,
12. kritischer Reservepuffer,
13. Logging im Interruptkontext,
14. Logging im NMI-Kontext,
15. Panic Logging,
16. Rekursionserkennung,
17. Rate Limiting,
18. Duplikatunterdrückung,
19. Eintragskürzung,
20. Schutz von Secret-Feldern,
21. Ausblendung von Kerneladressen,
22. Stacktrace mit Tiefenbegrenzung,
23. Treiberlogging,
24. Userspace-Logendpunkt,
25. Ausfall des Userspace Log Service,
26. Sink-Fehler,
27. Log-Stream-Objekt,
28. Capability-gefiltertes Lesen,
29. dynamische Logstufe,
30. persistente Speicherung,
31. Logrotation,
32. Boot-ID-Zuordnung,
33. beschädigter persistierter Record,
34. Audit-Trennung,
35. Datenschutzfilter,
36. korrekte Statistik und Eigendiagnose.

## 56. Verbindliche Invarianten

1. Ein Logrecord wird vollständig oder gar nicht als gültig veröffentlicht.
2. Logging im direkten Interruptkontext blockiert nicht.
3. Panic Logging benötigt weder regulären Heap noch Userspace-Dienste.
4. Secret-Felder werden nicht in normale Logausgaben übernommen.
5. Kerneladressen werden unprivilegierten Lesern nicht vollständig offengelegt.
6. Pufferüberläufe werden gezählt und eindeutig sichtbar gemacht.
7. Ein langsamer oder fehlerhafter Sink blockiert den Kernel nicht dauerhaft.
8. Userspace kann Kernelrecords weder verändern noch als Kernelquelle fälschen.
9. Jeder Record besitzt eine validierbare Größe und Formatversion.
10. Rekursives Logging ist begrenzt und verwendet einen sicheren Notfallpfad.
11. Sicherheits-Audit hängt nicht ausschließlich vom normalen Logging ab.
12. Logging- und Filterzustände werden unter geeigneter Synchronisation verwaltet.

## 57. Referenzablauf eines Logeintrags

```text
Kernelkomponente erzeugt Ereignis
  -> Schweregrad und Kategorie prüfen
  -> Schreibfilter und Rate Limit anwenden
  -> erforderliche Recordgröße berechnen
  -> CPU-lokalen Ringpufferplatz reservieren
  -> Header und typisierte Felder schreiben
  -> sensible Felder filtern oder kennzeichnen
  -> Record atomar committen
  -> wartenden Log Service benachrichtigen
  -> optional kritischen Sink auslösen
  -> Statistik aktualisieren
```

## 58. Zusammenfassung

Das NovaOS Logging Framework stellt strukturierte, versionierte und rekursionssichere Protokollierung vom frühen Bootvorgang bis zur Kernel Panic bereit.

CPU-lokale Ringpuffer reduzieren Sperrkonflikte, während Rate Limiting, Duplikatunterdrückung und feste Größenlimits eine Überlastung verhindern. Kritische Ereignisse erhalten einen reservierten Notfallpuffer.

Sensible Daten, Geheimnisse und Kerneladressen werden durch Feldklassifikation und Capability-Prüfungen geschützt. Speicherung, Rotation und Darstellung übernimmt ein lokaler Userspace Log Service, ohne dass der Kernel von dessen Verfügbarkeit abhängig wird.