# NPSPEC-KERNEL-0011 – System Call Interface

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-KERNEL-0011 |
| Titel | System Call Interface |
| Status | Spezifikation |
| Version | 1.0 |
| Datum | 2026-08-03 |
| Bereich | Kernel / ABI / Systemaufrufe |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | NPSPEC-KERNEL-0001, NPSPEC-KERNEL-0003, NPSPEC-KERNEL-0004, NPSPEC-KERNEL-0005, NPSPEC-KERNEL-0010 |
| Zugehörige ADRs | ADR-KERNEL-0102, ADR-KERNEL-0103, ADR-KERNEL-0105, ADR-SEC-0006, ADR-SEC-0008 |

---

## 1. Zweck

Diese Spezifikation definiert das System Call Interface von NovaOS.

Die Systemaufrufschnittstelle bildet die kontrollierte Grenze zwischen Userspace und Kernel. Sie ermöglicht Programmen den Zugriff auf Kernel-Services, ohne Kernelstrukturen oder privilegierte Hardware direkt zugänglich zu machen.

## 2. Ziele

Die Schnittstelle muss:

- eine stabile versionierte ABI bereitstellen,
- mehrere Prozessorarchitekturen unterstützen,
- Capability-Prüfungen erzwingen,
- Userspace-Argumente vollständig validieren,
- Kernel- und Userspace-Adressräume trennen,
- schnelle Systemaufrufe ermöglichen,
- Erweiterungen ohne ABI-Bruch unterstützen,
- Diagnose und Audit kontrolliert integrieren,
- sichere Rückkehr in den Userspace gewährleisten.

## 3. Grundprinzip

Userspace-Programme greifen nicht direkt auf interne Kernelfunktionen zu.

Der Zugriff erfolgt über:

```text
Anwendung
  -> Nova Runtime Library
  -> versionierter Kernel-Service
  -> System Call Dispatcher
  -> Capability-Prüfung
  -> Kernel-Subsystem
  -> definierter Statuscode
```

Interne Kernel-APIs und öffentliche System-Call-ABIs sind voneinander getrennt.

## 4. ABI-Modell

NovaOS verwendet eine versionierte Kernel-Service-ABI.

Ein Systemaufruf wird identifiziert durch:

- Service-Namespace,
- Service- oder Operationskennung,
- ABI-Version,
- Argumentstruktur,
- Capability-Kontext.

```c
typedef uint32_t np_service_id_t;
typedef uint32_t np_operation_id_t;
typedef uint32_t np_abi_version_t;
```

## 5. Service-Namensräume

Systemaufrufe werden nach Subsystemen gruppiert.

| Namespace | Inhalt |
|---|---|
| Core | ABI-, Handle- und Systeminformationen |
| Process | Prozesse und Job Objects |
| Thread | Threads, Scheduling und Warten |
| Memory | virtuelle Speicherverwaltung |
| IPC | Nachrichten, Events und Shared Memory |
| VFS | Dateien, Verzeichnisse und Mounts |
| Device | Geräte- und Treiberdienste |
| Network | Sockets und Netzwerkobjekte |
| Security | Capabilities, Identität und Audit |
| Diagnostic | Logging, Tracing und Debugging |
| Power | Energie- und Systemzustände |

Neue Services dürfen ergänzt werden, ohne bestehende Kennungen neu zu verwenden.

## 6. Servicekennung

Eine Servicekennung kann logisch aufgebaut sein aus:

```text
[ Namespace | Service | Operation ]
```

Eine mögliche Darstellung ist:

```c
#define NP_SERVICE_ID(namespace_id, service_id) \
    ((((uint32_t)(namespace_id)) << 16) | ((service_id) & 0xFFFF))
```

Die genaue Bitaufteilung wird in der ABI-Registry festgelegt.

Veröffentlichte Kennungen dürfen nicht nachträglich eine andere Bedeutung erhalten.

## 7. ABI-Versionierung

Jeder versionierte Service besitzt:

- Major-Version,
- Minor-Version,
- Strukturgrößen,
- optionale Feature-Flags.

```c
typedef struct np_abi_header {
    uint16_t major_version;
    uint16_t minor_version;
    uint32_t structure_size;
    uint64_t feature_flags;
} np_abi_header_t;
```

Eine inkompatible Major-Version wird abgelehnt.

Eine neuere kompatible Minor-Version darf akzeptiert werden, wenn alle benötigten Felder vorhanden sind.

## 8. Architekturabhängiger Eintritt

Die konkrete Eintrittsmethode wird durch die Zielarchitektur festgelegt.

Für x86 können verwendet werden:

- `SYSENTER/SYSEXIT`,
- `SYSCALL/SYSRET` im 64-Bit-Modus,
- ein kontrolliertes Interrupt-Gate als Kompatibilitäts- oder Fallback-Pfad.

Die generische Service-ABI darf nicht von einer bestimmten CPU-Instruktion abhängen.

## 9. Initialer x86-32-Pfad

Für die initiale 32-Bit-x86-Implementierung wird bevorzugt `SYSENTER` verwendet, sofern CPU und Plattform dies unterstützen.

Ein Interrupt-Gate kann als kompatibler Rückfallpfad bestehen.

Der Kernel muss pro CPU die erforderlichen System-Call-Register und den Eintritts-Stack konfigurieren.

Der Eintritt darf niemals direkt auf einem nicht geprüften Userspace-Stack weiterarbeiten.

## 10. System-Call-Frame

Der Low-Level-Einstieg erzeugt einen normalisierten System-Call-Frame.

```c
typedef struct np_syscall_frame {
    np_service_id_t service;
    np_operation_id_t operation;
    np_abi_version_t abi_version;
    uint32_t flags;

    uintptr_t argument_pointer;
    size_t argument_size;

    uintptr_t user_instruction_pointer;
    uintptr_t user_stack_pointer;
    uintptr_t user_flags;

    np_status_t result;
} np_syscall_frame_t;
```

Die tatsächliche Registerbelegung ist architekturspezifisch.

## 11. Eintrittsablauf

Der System-Call-Eintritt muss:

1. Interrupt- und Prozessorzustand sichern,
2. auf einen gültigen Kernel-Stack wechseln,
3. CPU- und Thread-lokale Daten aktivieren,
4. Userspace-Kontext erfassen,
5. Servicekennung und ABI-Version validieren,
6. Argumente sicher kopieren,
7. Capability-Prüfungen durchführen,
8. den Service ausführen,
9. Rückgabedaten sicher in den Userspace kopieren,
10. den Rückkehrkontext validieren,
11. sicher in den Userspace zurückkehren.

## 12. Registerkonvention

Die Architektur-ABI definiert:

- Register für Servicekennung,
- Register für Operationskennung,
- Zeiger auf Argumentstruktur,
- Größe der Argumentstruktur,
- Rückgaberegister,
- zu erhaltende Register.

Komplexe Parameter werden nicht über eine große Anzahl einzelner Register übertragen, sondern über versionierte Argumentstrukturen.

## 13. Argumentstrukturen

Jede nicht triviale Operation verwendet eine definierte Struktur.

```c
typedef struct np_thread_create_args {
    np_abi_header_t header;

    np_process_handle_t process;
    uintptr_t entry_point;
    uintptr_t argument;
    uintptr_t stack_address;
    uint64_t stack_size;

    uint32_t scheduler_class;
    uint32_t priority;

    np_thread_handle_t result_thread;
} np_thread_create_args_t;
```

Eingabe- und Ausgabefelder müssen eindeutig dokumentiert sein.

## 14. Strukturgrößen

Der Kernel darf nur Felder lesen oder schreiben, die innerhalb der übergebenen `structure_size` liegen.

Bei einer größeren Userspace-Struktur werden unbekannte Erweiterungsfelder ignoriert, sofern die Version kompatibel ist.

Reservierte Felder müssen beim Aufruf null sein, sofern die jeweilige ABI nichts anderes festlegt.

## 15. Userspace-Zeiger

Userspace-Zeiger werden niemals direkt ungeprüft dereferenziert.

Jeder Zeiger muss geprüft werden auf:

- gültigen Userspace-Adressbereich,
- erforderliche Zugriffsrechte,
- korrekte Länge,
- Größenüberlauf,
- Ausrichtung,
- vollständige Abbildung,
- mögliche Überschneidung mit anderen Puffern.

Die Prüfung ersetzt nicht den geschützten Kopiervorgang, da sich Mappings parallel ändern können.

## 16. Sicheres Kopieren

Für den Datentransfer werden kontrollierte Funktionen verwendet:

```c
np_status_t np_copy_from_user(
    void* kernel_destination,
    const void* user_source,
    size_t size
);

np_status_t np_copy_to_user(
    void* user_destination,
    const void* kernel_source,
    size_t size
);
```

Ein Page Fault während dieser Funktionen wird in einen definierten Fehler umgewandelt.

Er darf nicht automatisch eine Kernel Panic auslösen.

## 17. Time-of-Check-to-Time-of-Use

Der Kernel darf nicht davon ausgehen, dass ein Userspace-Puffer nach einer Vorprüfung unverändert bleibt.

Sicherheitsrelevante Eingabedaten werden daher:

1. einmalig in Kernelspeicher kopiert,
2. dort vollständig validiert,
3. ausschließlich aus dieser Kopie verarbeitet.

Zeigerketten aus dem Userspace sind möglichst zu vermeiden.

## 18. Rückgabewert

Jeder Systemaufruf liefert einen standardisierten Statuscode.

```c
typedef int32_t np_status_t;
```

Grundregel:

```text
0        = Erfolg
< 0      = Fehler
> 0      = operationenspezifischer erfolgreicher Status
```

Daten werden über explizite Ausgabefelder oder Output-Puffer zurückgegeben.

## 19. Fehlercodes

Mindestens folgende allgemeine Fehlercodes werden definiert:

| Fehlercode | Bedeutung |
|---|---|
| `NP_OK` | Operation erfolgreich |
| `NP_ERR_INVALID_ARGUMENT` | ungültiger Parameter |
| `NP_ERR_INVALID_POINTER` | ungültiger Userspace-Zeiger |
| `NP_ERR_INVALID_SIZE` | Struktur- oder Puffergröße ungültig |
| `NP_ERR_ABI_INCOMPATIBLE` | ABI-Version nicht kompatibel |
| `NP_ERR_SERVICE_UNKNOWN` | Service nicht vorhanden |
| `NP_ERR_OPERATION_UNKNOWN` | Operation nicht vorhanden |
| `NP_ERR_ACCESS_DENIED` | erforderliche Capability fehlt |
| `NP_ERR_INVALID_HANDLE` | Handle ist ungültig |
| `NP_ERR_INVALID_STATE` | Operation im aktuellen Zustand unzulässig |
| `NP_ERR_OUT_OF_MEMORY` | Speicher nicht verfügbar |
| `NP_ERR_TIMEOUT` | Zeitüberschreitung |
| `NP_ERR_INTERRUPTED` | Operation wurde unterbrochen |
| `NP_ERR_WOULD_BLOCK` | Operation würde blockieren |
| `NP_ERR_NOT_SUPPORTED` | Funktion wird nicht unterstützt |

## 20. Handles

Kernelobjekte werden über Handles referenziert.

```c
typedef uint64_t np_handle_t;
```

Ein Handle enthält keine direkt nutzbare Kerneladresse.

Bei jeder Verwendung wird geprüft:

- Gültigkeit,
- Generationskennung,
- Objekttyp,
- Capability-Rechte,
- Besitzer-Namespace,
- Objektlebensdauer.

Die Kenntnis eines numerischen Handlewerts verleiht keine zusätzlichen Rechte.

## 21. Capability-Prüfung

Jede privilegierte Operation benötigt eine passende Capability.

Die Prüfung erfolgt:

- vor der Zustandsänderung,
- für das konkrete Zielobjekt,
- mit der konkreten Operation,
- unter Berücksichtigung von Job- und Sandbox-Richtlinien.

```c
np_status_t np_capability_check(
    np_thread_t* caller,
    np_handle_t object,
    np_capability_rights_t required_rights
);
```

Eine fehlgeschlagene Prüfung liefert `NP_ERR_ACCESS_DENIED`.

## 22. Aktueller Aufrufer

Der Dispatcher ermittelt den Aufrufer ausschließlich aus dem aktuellen Kernel-Threadkontext.

Userspace darf Prozess-, Thread- oder Identitätsinformationen nicht als vertrauenswürdige Aufruferkennung übergeben.

Der aktuelle Kontext umfasst mindestens:

- Thread,
- Prozess,
- Capability-Domain,
- Handle-Tabelle,
- Job Object,
- Sicherheitsprofil,
- aktiven Adressraum.

## 23. Dispatcher

```c
typedef np_status_t (*np_system_service_handler_t)(
    np_syscall_context_t* context,
    void* arguments,
    size_t argument_size
);

typedef struct np_system_service_entry {
    np_service_id_t service;
    np_operation_id_t operation;

    uint16_t minimum_major;
    uint16_t maximum_major;

    size_t minimum_argument_size;
    uint32_t flags;

    np_system_service_handler_t handler;
} np_system_service_entry_t;
```

Der Dispatcher sucht den passenden Eintrag und führt gemeinsame Sicherheitsprüfungen durch.

## 24. Registrierung von Services

Kernel-Services werden während der Kernelinitialisierung oder beim Laden eines autorisierten Kernelmoduls registriert.

```c
np_status_t np_system_service_register(
    const np_system_service_entry_t* entry
);
```

Eine bestehende Kombination aus Service-, Operations- und ABI-Major-Version darf nicht stillschweigend ersetzt werden.

Dynamisch registrierte Services müssen dieselben Sicherheits- und ABI-Regeln erfüllen wie Kernel-Core-Services.

## 25. Entfernen von Services

Ein Service darf nur entfernt werden, wenn:

- keine neuen Aufrufe mehr angenommen werden,
- alle laufenden Aufrufe beendet sind,
- keine ABI-Verpflichtung seine dauerhafte Verfügbarkeit verlangt,
- abhängige Dienste behandelt wurden.

Stabile Kernel-Core-Services dürfen im normalen Betrieb nicht entfernt werden.

## 26. Service-Feature-Abfrage

Userspace kann verfügbare Services und Versionen abfragen.

```c
np_status_t np_system_query_service(
    np_service_id_t service,
    np_service_information_t* information
);
```

Die Antwort kann enthalten:

- unterstützte Major- und Minor-Versionen,
- Feature-Flags,
- maximale Argumentgrößen,
- erforderliche Rechte,
- Verfügbarkeitsstatus.

Interne, nicht öffentliche Services werden nicht offengelegt.

## 27. Blocking System Calls

Ein Systemaufruf darf blockieren, wenn seine Definition dies erlaubt.

Vor dem Blockieren muss der Kernel:

- keine nicht schlaffähige Sperre halten,
- den Threadzustand korrekt setzen,
- Warteobjekt und Timeout registrieren,
- Abbruch- und Signalzustände berücksichtigen.

Nach dem Aufwachen wird der Systemaufruf im ursprünglichen Kernelkontext fortgesetzt oder mit einem definierten Status beendet.

## 28. Unterbrechbare Systemaufrufe

Bestimmte blockierende Operationen können durch Signale, Prozessbeendigung oder Abbruchanforderungen unterbrochen werden.

Das Ergebnis ist:

```text
NP_ERR_INTERRUPTED
```

Die API-Dokumentation muss angeben, ob:

- der Aufruf automatisch neu gestartet werden kann,
- bereits Teildaten übertragen wurden,
- der Aufrufer selbst erneut aufrufen muss.

## 29. Partielle Ergebnisse

I/O- und Übertragungsoperationen können teilweise erfolgreich sein.

Die Argumentstruktur muss dann sowohl enthalten:

- Statuscode,
- tatsächlich verarbeitete Datenmenge.

Ein partieller Erfolg darf nicht als vollständiger Erfolg ausgegeben werden.

## 30. Asynchrone Operationen

Länger laufende Services können asynchrone Operationen anbieten.

Der Aufruf liefert dabei ein wartbares Objekt oder Completion-Handle:

```c
typedef np_handle_t np_async_operation_handle_t;
```

Der Aufrufer kann:

- auf Abschluss warten,
- Status abfragen,
- die Operation abbrechen,
- einen Event-Endpunkt binden.

Asynchrone Operationen behalten nur ausdrücklich kopierte oder gepinnte Ressourcen.

## 31. Batch-Aufrufe

Für häufige kleine Operationen kann eine Batch-Schnittstelle bereitgestellt werden.

```c
np_status_t np_system_batch(
    const np_batch_operation_t* operations,
    size_t operation_count,
    np_batch_result_t* results
);
```

Jede Operation wird separat validiert.

Ein Fehler in einem Eintrag darf keine ungeprüfte Ausführung nachfolgender Einträge verursachen.

Atomare Batch-Semantik muss ausdrücklich angefordert und vom jeweiligen Service unterstützt werden.

## 32. Fast Paths

Häufige Operationen können optimierte Fast Paths besitzen.

Beispiele sind:

- Thread Yield,
- Event-Abfrage,
- monotone Zeitabfrage,
- einfache Handleoperationen,
- uncontended Synchronisation.

Fast Paths müssen dieselben Sicherheits- und ABI-Ergebnisse liefern wie der reguläre Pfad.

Sicherheitsprüfungen dürfen nicht aus Performancegründen entfallen.

## 33. Shared Service Page

Der Kernel kann eine schreibgeschützte Service-Seite in Userspace-Prozesse abbilden.

Sie kann bereitstellen:

- ABI-Versionen,
- monotone Zeitdaten,
- CPU- und Featureinformationen,
- optimierte Userspace-Stubs,
- unveränderliche Systemparameter.

Userspace darf die Seite nicht verändern.

Sicherheitsrelevante Entscheidungen dürfen nicht ausschließlich auf veränderbaren Userspace-Daten beruhen.

## 34. Nova Runtime Library

Anwendungen sollen Systemaufrufe über die Nova Runtime Library ausführen.

Die Runtime Library übernimmt:

- architekturspezifische Eintrittsinstruktionen,
- Aufbau von Argumentstrukturen,
- ABI-Aushandlung,
- Statuscodeübersetzung,
- optionale Kompatibilitätslogik.

Direkte Systemaufrufe bleiben technisch möglich, sofern die öffentliche ABI eingehalten wird.

## 35. ABI-Stabilität

Veröffentlichte System-Call-ABIs gelten als stabil innerhalb ihrer Major-Version.

Zulässige Änderungen sind:

- neue Services,
- neue Operationen,
- neue kompatible Minor-Versionen,
- Erweiterung von Strukturen am Ende,
- neue optionale Feature-Flags,
- neue Fehlercodes.

Unzulässig sind:

- Wiederverwendung alter Kennungen,
- Änderung bestehender Feldbedeutungen,
- Änderung von Größe oder Position bestehender Felder,
- stillschweigende Verschärfung ohne Versionierung,
- Ausgabe interner Kernelstrukturen.

## 36. Veraltete Services

Ein Service kann als veraltet markiert werden.

Der Ablauf ist:

1. Service als deprecated kennzeichnen,
2. Ersatzservice dokumentieren,
3. Diagnosehinweis für Entwickler bereitstellen,
4. alte ABI für den definierten Kompatibilitätszeitraum erhalten,
5. Entfernung nur in einer neuen inkompatiblen Major-Version.

Sicherheitskritische Services können früher deaktiviert werden, benötigen dann aber eine explizite Richtlinie und klare Fehlerantwort.

## 37. Kompatibilitätsschicht

NovaOS kann zusätzliche ABI-Personas bereitstellen, etwa für Kompatibilitätsumgebungen.

Diese werden außerhalb des nativen Dispatchers übersetzt:

```text
Kompatibilitätsaufruf
  -> ABI-Persona
  -> Argumentübersetzung
  -> nativer NovaOS-Service
  -> Ergebnisübersetzung
```

Kompatibilitäts-ABIs dürfen keine internen nativen Services ohne Capability-Prüfung umgehen.

## 38. Systemaufrufe aus Kernelcode

Kernelkomponenten rufen keine Userspace-System-Call-Stubs auf.

Sie verwenden interne typisierte Kernel-APIs.

Dadurch werden vermieden:

- unnötige Kontextwechsel,
- Userspace-Zeigersemantik im Kernel,
- doppelte Validierung,
- Abhängigkeit interner Komponenten von der öffentlichen ABI.

Interne APIs müssen dennoch die vorgesehenen Sicherheitsgrenzen berücksichtigen.

## 39. Rekursion

Systemaufrufe dürfen nicht unkontrolliert rekursiv in den Dispatcher eintreten.

Pro Thread wird ein System-Call-Zustand geführt:

```c
typedef struct np_syscall_state {
    uint32_t depth;
    uint32_t flags;
    np_service_id_t current_service;
    np_operation_id_t current_operation;
} np_syscall_state_t;
```

Normale Userspace-Aufrufe besitzen eine Tiefe von eins.

Interne Kernel-Callbacks dürfen nicht als neue Userspace-Systemaufrufe simuliert werden.

## 40. Scheduler-Integration

Der System-Call-Pfad muss mit Kernel-Präemption und Scheduling zusammenarbeiten.

Während kurzer Dispatcherabschnitte kann Präemption kontrolliert deaktiviert werden.

Längere Services müssen präemptierbar sein, sofern keine konkrete Synchronisationsanforderung entgegensteht.

Vor der Rückkehr in den Userspace wird geprüft, ob:

- eine Neuplanung erforderlich ist,
- ein Signal oder eine Exception aussteht,
- der Thread suspendiert wurde,
- der Prozess beendet wird.

## 41. Exception-Integration

Fehlerhafte Userspace-Zeiger führen zu einem definierten System-Call-Fehler und nicht automatisch zu einer Prozess-Exception.

Eine Exception wird ausgelöst, wenn:

- der Userspace selbst eine unzulässige Instruktion ausführt,
- der Rückkehrkontext beschädigt ist,
- eine Sicherheitsrichtlinie dies ausdrücklich verlangt,
- ein nicht lokal behandelbarer Speicherfehler außerhalb des kontrollierten Kopierpfads auftritt.

Kernel-Exceptions im Service-Handler werden nach NPSPEC-KERNEL-0010 behandelt.

## 42. Restart-Semantik

Ein Systemaufruf kann als restartfähig markiert sein.

Vor einem automatischen Neustart muss sichergestellt sein, dass:

- keine sichtbare Teilwirkung erfolgt ist,
- Eingabedaten weiterhin gültig oder intern kopiert sind,
- kein Timeout unzulässig verlängert wird,
- die Operation idempotent oder transaktional fortsetzbar ist.

Andernfalls wird `NP_ERR_INTERRUPTED` mit einem dokumentierten Teilstatus zurückgegeben.

## 43. Zeitangaben

Timeouts werden grundsätzlich in Nanosekunden und bevorzugt als absolute monotone Deadlines dargestellt.

```c
typedef uint64_t np_time_ns_t;
```

Besondere Werte können definieren:

```text
0                 = nicht warten
NP_TIMEOUT_INFINITE = unbegrenzt warten
```

Überläufe bei der Umrechnung relativer Zeitwerte müssen erkannt werden.

## 44. Zeichenketten

Zeichenketten werden über Länge und Zeiger übertragen.

```c
typedef struct np_user_string {
    uintptr_t data;
    uint64_t length;
    uint32_t encoding;
    uint32_t flags;
} np_user_string_t;
```

Zeichenketten müssen nicht nullterminiert sein.

Für öffentliche Systemdienste wird UTF-8 als Standardkodierung verwendet, sofern die jeweilige API nichts anderes definiert.

## 45. Variable Datenmengen

Für Listen und variable Daten verwendet die ABI:

- Elementgröße,
- Elementanzahl,
- Gesamtpuffergröße,
- tatsächlich benötigte Größe.

Bei zu kleinem Ausgabepuffer liefert der Service:

```text
NP_ERR_BUFFER_TOO_SMALL
```

Zusätzlich wird die erforderliche Mindestgröße ausgegeben, sofern dies sicher möglich ist.

## 46. System-Call-Filter

Pro Prozess oder Sandbox kann ein System-Call-Filter aktiviert werden.

Er kann:

- Services vollständig sperren,
- einzelne Operationen sperren,
- Argumentbereiche begrenzen,
- zusätzliche Capability-Prüfungen verlangen,
- Auditereignisse auslösen.

Filter dürfen durch den eingeschränkten Prozess nicht selbst erweitert oder deaktiviert werden.

## 47. Audit

Sicherheitsrelevante Systemaufrufe können Auditereignisse erzeugen.

Dazu gehören beispielsweise:

- Capability-Delegation,
- Debugzugriff,
- Treiber- und Geräteoperationen,
- Änderungen globaler Sicherheitsrichtlinien,
- Prozesszugriffe über Sicherheitsdomains hinweg,
- wiederholte Zugriffsverletzungen.

Audit erfolgt standardmäßig lokal und darf keine geheimen Eingabedaten vollständig protokollieren.

## 48. Tracing

System-Call-Tracing kann erfassen:

- Service und Operation,
- Prozess- und Threadkennung,
- Start- und Endzeit,
- Statuscode,
- blockierte Zeit,
- kopierte Datenmengen,
- Capability-Fehler.

Pufferinhalte, Schlüssel, Kennwörter und sicherheitskritische Tokens werden nicht standardmäßig aufgezeichnet.

Tracing fremder Prozesse erfordert `CAP_SYSCALL_TRACE`.

## 49. Rate Limiting

Bestimmte administrative oder ressourcenintensive Services können begrenzt werden.

Rate Limits können gelten pro:

- Prozess,
- Thread,
- Capability-Domain,
- Job Object,
- Service,
- System.

Eine Begrenzung wird mit einem definierten Status wie `NP_ERR_RATE_LIMITED` gemeldet.

Kritische Kernelpfade dürfen nicht durch unprivilegierte Aufrufraten verdrängt werden.

## 50. Capability-Rechte

Mindestens folgende systemaufrufspezifische Rechte werden definiert:

| Capability | Bedeutung |
|---|---|
| `CAP_SYSCALL_QUERY` | Service- und ABI-Informationen abfragen |
| `CAP_SYSCALL_BATCH` | erweiterte Batch-Schnittstellen verwenden |
| `CAP_SYSCALL_FILTER` | Filter einer autorisierten Domain verwalten |
| `CAP_SYSCALL_TRACE` | System-Call-Traces lesen |
| `CAP_SYSCALL_AUDIT` | geschützte Auditinformationen lesen |
| `CAP_SYSCALL_ADMIN` | globale Service- und ABI-Richtlinien verwalten |

Die eigentlichen Operationen benötigen zusätzlich ihre subsystemspezifischen Capabilities.

## 51. Sicherheitsanforderungen

Das System Call Interface muss sicherstellen, dass:

- jeder Eintritt auf einen kontrollierten Kernel-Stack wechselt,
- Userspace-Zeiger nie ungeprüft dereferenziert werden,
- Eingabedaten gegen nachträgliche Userspace-Änderungen geschützt werden,
- Handles auf Typ, Generation und Rechte geprüft werden,
- unbekannte Strukturfelder nicht als vertrauenswürdig behandelt werden,
- Rückkehrkontexte keine privilegierten CPU-Zustände enthalten,
- Kerneladressen nicht über Statuswerte oder Ausgabestrukturen offengelegt werden,
- System-Call-Filter und Capabilities nicht umgangen werden,
- W^X- und Kernelisolationsrichtlinien erhalten bleiben,
- alle Größenberechnungen auf Integerüberläufe geprüft werden.

## 52. Datenschutz

System-Call-Diagnosen können Informationen über Benutzeraktivitäten enthalten.

Daher gelten:

- Tracing ist standardmäßig deaktiviert oder minimal,
- Inhalte von Puffern werden nicht regulär protokolliert,
- Diagnosedaten bleiben standardmäßig lokal,
- Zugriff wird durch Capabilities begrenzt,
- Aufbewahrung und Löschung müssen konfigurierbar sein,
- externe Übertragung erfordert eine ausdrückliche Konfiguration.

## 53. Statistik

```c
typedef struct np_syscall_statistics {
    uint64_t total_calls;
    uint64_t successful_calls;
    uint64_t failed_calls;

    uint64_t blocked_calls;
    uint64_t interrupted_calls;
    uint64_t capability_denials;
    uint64_t invalid_pointer_errors;

    uint64_t total_time_ns;
    uint64_t maximum_time_ns;
} np_syscall_statistics_t;
```

Statistiken sollen pro Service, Prozess, CPU und System abrufbar sein.

## 54. Fehlerbehandlung

Ungültige Userspace-Eingaben führen zu einem definierten Fehler, nicht zu einem Kernelabsturz.

Folgende Situationen gelten als schwere Kernelkonsistenzfehler:

- Rückkehr in den Userspace mit privilegiertem Kontext,
- System-Call-Eintritt ohne gültigen Kernel-Stack,
- beschädigte Dispatcher-Tabelle,
- Ausführung eines nicht registrierten Funktionszeigers,
- Umgehung einer erforderlichen Capability-Prüfung,
- Freigabe eines Service-Handlers während eines aktiven Aufrufs,
- Offenlegung interner Kerneladressen durch eine öffentliche ABI.

## 55. Testanforderungen

Mindestens folgende Tests sind erforderlich:

1. gültiger einfacher Systemaufruf,
2. unbekannte Servicekennung,
3. unbekannte Operationskennung,
4. kompatible ABI-Minor-Version,
5. inkompatible ABI-Major-Version,
6. zu kleine Argumentstruktur,
7. größere Struktur mit unbekannten Erweiterungsfeldern,
8. ungültiger Userspace-Zeiger,
9. teilweise nicht abgebildeter Puffer,
10. Überlauf einer Größenberechnung,
11. sicherer `copy_from_user()`-Fehler,
12. sicherer `copy_to_user()`-Fehler,
13. gültiges Handle,
14. ungültige Handle-Generation,
15. falscher Handle-Objekttyp,
16. fehlende Capability,
17. erfolgreicher blockierender Aufruf,
18. Timeout eines blockierenden Aufrufs,
19. Unterbrechung eines Systemaufrufs,
20. partielles I/O-Ergebnis,
21. asynchrone Operation,
22. Batch-Aufruf mit fehlerhaftem Eintrag,
23. System-Call-Filter,
24. System-Call-Tracing,
25. Audit eines Sicherheitsaufrufs,
26. parallele Systemaufrufe auf mehreren CPUs,
27. Systemaufruf während Prozessbeendigung,
28. Scheduler-Neuplanung vor Userspace-Rückkehr,
29. validierte Rückkehr nach Signal oder Exception,
30. Fast Path und regulärer Pfad mit identischem Ergebnis,
31. Rückfall vom schnellen x86-Eintritt auf Interrupt-Gate,
32. Rate Limiting,
33. stabile Kennungen über ABI-Versionen,
34. Kompatibilität alter Argumentstrukturen.

## 56. Verbindliche Invarianten

1. Jeder Systemaufruf beginnt auf einem gültigen Kernel-Stack.
2. Userspace-Zeiger werden niemals direkt ungeprüft dereferenziert.
3. Die Aufruferidentität stammt ausschließlich aus dem aktuellen Kernel-Threadkontext.
4. Jede privilegierte Operation führt eine Capability-Prüfung durch.
5. Handles enthalten keine direkt nutzbaren Kernelzeiger.
6. Veröffentlichte Servicekennungen werden nicht mit neuer Bedeutung wiederverwendet.
7. Bestehende Felder einer stabilen ABI ändern ihre Bedeutung und Position nicht.
8. Ein Systemaufruffehler aus dem Userspace beschädigt nicht den Kernelzustand.
9. Die Rückkehr in den Userspace erfolgt nur mit validiertem Prozessorzustand.
10. Fast Paths umgehen keine Sicherheitsprüfungen.
11. Eingabestrukturen werden vor sicherheitsrelevanter Verarbeitung in Kernelspeicher kopiert.
12. Kernelkomponenten verwenden interne APIs und keine Userspace-System-Call-Stubs.

## 57. Referenzablauf eines Systemaufrufs

```text
Userspace Runtime
  -> Architektur-Eintritt ausführen
  -> auf Kernel-Stack wechseln
  -> System-Call-Frame erzeugen
  -> Service und ABI-Version prüfen
  -> Argumentstruktur in Kernelspeicher kopieren
  -> Argumente und Handles validieren
  -> Capability- und Filterprüfung
  -> Kernel-Service ausführen
  -> Ausgabedaten in Userspace kopieren
  -> Audit, Statistik und Tracing aktualisieren
  -> Signale, Exceptions und Reschedule prüfen
  -> Rückkehrkontext validieren
  -> in den Userspace zurückkehren
```

## 58. Zusammenfassung

Das NovaOS System Call Interface stellt eine versionierte und capability-geschützte Grenze zwischen Userspace und Kernel bereit.

Systemdienste werden über stabile Service- und Operationskennungen angesprochen. Komplexe Parameter verwenden erweiterbare Argumentstrukturen, während sichere Kopierfunktionen, Handle-Prüfungen und System-Call-Filter die Kernelgrenze schützen.

Architekturspezifische Eintrittsmechanismen bleiben von der öffentlichen Service-ABI getrennt. Dadurch kann NovaOS schnelle Systemaufrufe, langfristige ABI-Stabilität und zukünftige Prozessorarchitekturen gemeinsam unterstützen.