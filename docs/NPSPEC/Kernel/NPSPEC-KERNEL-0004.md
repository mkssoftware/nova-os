# NPSPEC-KERNEL-0004 – Process Manager

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-KERNEL-0004 |
| Titel | Process Manager |
| Status | Spezifikation |
| Version | 1.0 |
| Datum | 2026-08-03 |
| Bereich | Kernel / Prozessverwaltung |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | NPSPEC-KERNEL-0001, NPSPEC-KERNEL-0002, NPSPEC-KERNEL-0003 |
| Zugehörige ADRs | ADR-PROC-0001, ADR-PROC-0002, ADR-PROC-0009, ADR-PROC-0010, ADR-KERNEL-0102, ADR-KERNEL-0103 |

---

## 1. Zweck

Diese Spezifikation definiert den Process Manager des NovaOS-Kernels.

Der Process Manager verwaltet:

- Prozesse und deren Lebenszyklus,
- Threads eines Prozesses,
- virtuelle Adressräume,
- Capability-Domains,
- Prozessgruppen und Job Objects,
- Prozessbeziehungen,
- Start und Beendigung von Programmen,
- Ressourcenabrechnung,
- Prozessereignisse.

## 2. Grundmodell

Ein Prozess ist ein isolierter Ausführungskontext und ein Objekt innerhalb des Kernel Object Graph.

Ein Prozess besitzt mindestens:

- eine eindeutige Prozesskennung,
- einen virtuellen Adressraum,
- eine Capability-Domain,
- einen Handle-Namespace,
- mindestens einen Thread während der aktiven Ausführung,
- Sicherheits- und Identitätsinformationen,
- Ressourcenlimits,
- eine Referenz auf Prozessgruppe und Job Object.

Der Scheduler plant ausschließlich Threads ein. Prozesse selbst werden nicht direkt ausgeführt.

## 3. Prozesskennung

```c
typedef uint64_t np_process_id_t;
```

Folgende Werte sind reserviert:

| Prozesskennung | Bedeutung |
|---:|---|
| `0` | Kernelkontext |
| `1` | erster Userspace-Prozess |
| `2–255` | reserviert für Kerndienste |
| `256+` | reguläre Prozesse |

Eine Prozesskennung darf während der Lebensdauer des Systems nicht erneut verwendet werden, solange noch Referenzen auf den alten Prozess existieren.

## 4. Prozess-Handle

Userspace-Komponenten greifen nicht direkt über Prozesskennungen auf Prozesse zu.

Stattdessen werden capability-geschützte Handles verwendet:

```c
typedef uint64_t np_process_handle_t;
```

Ein Handle referenziert:

- das Prozessobjekt,
- erlaubte Operationen,
- den besitzenden Handle-Namespace,
- optionale Vererbungsregeln.

Die Kenntnis einer Prozesskennung allein verleiht keine Zugriffsrechte.

## 5. Prozesszustände

```c
typedef enum np_process_state {
    NP_PROCESS_CREATED,
    NP_PROCESS_INITIALIZING,
    NP_PROCESS_READY,
    NP_PROCESS_RUNNING,
    NP_PROCESS_SUSPENDED,
    NP_PROCESS_TERMINATING,
    NP_PROCESS_ZOMBIE,
    NP_PROCESS_TERMINATED
} np_process_state_t;
```

| Zustand | Bedeutung |
|---|---|
| `CREATED` | Prozessobjekt wurde angelegt |
| `INITIALIZING` | Adressraum und Startkontext werden vorbereitet |
| `READY` | Prozess ist startbereit |
| `RUNNING` | mindestens ein Thread ist aktiv oder ausführbar |
| `SUSPENDED` | alle Threads wurden angehalten |
| `TERMINATING` | Beendigung wurde eingeleitet |
| `ZOMBIE` | Ausführung beendet, Status noch abrufbar |
| `TERMINATED` | Prozessressourcen wurden freigegeben |

## 6. Erlaubte Zustandsübergänge

```text
CREATED
  -> INITIALIZING
  -> READY
  -> RUNNING
  -> SUSPENDED
  -> RUNNING
  -> TERMINATING
  -> ZOMBIE
  -> TERMINATED
```

Fehler während der Initialisierung führen von `CREATED`, `INITIALIZING` oder `READY` direkt nach `TERMINATING`.

Ein Prozess darf den Zustand `TERMINATED` nicht verlassen.

## 7. Prozessobjekt

```c
typedef struct np_process {
    np_kernel_object_t object;

    np_process_id_t process_id;
    np_process_state_t state;
    uint32_t flags;

    np_address_space_t* address_space;
    np_capability_domain_t* capability_domain;
    np_handle_table_t* handle_table;

    np_thread_list_t threads;
    np_process_group_t* process_group;
    np_job_object_t* job;

    np_process_id_t parent_id;
    np_exit_status_t exit_status;
    np_resource_accounting_t accounting;

    np_spinlock_t lock;
} np_process_t;
```

Die konkrete interne Struktur darf erweitert werden. Öffentlich sichtbar ist ausschließlich die versionierte Prozess-API.

## 8. Prozesserzeugung

Ein Prozess wird in mehreren kontrollierten Schritten erzeugt:

1. Prozessobjekt anlegen,
2. Prozesskennung reservieren,
3. virtuellen Adressraum erzeugen,
4. Capability-Domain anlegen,
5. Handle-Tabelle initialisieren,
6. Sicherheitsrichtlinie anwenden,
7. ausführbares Image laden,
8. initialen Thread erzeugen,
9. Prozess in den Kernel Object Graph eintragen,
10. Prozess zur Ausführung freigeben.

Der Prozess darf erst sichtbar ausführbar werden, wenn alle zwingenden Initialisierungsschritte erfolgreich abgeschlossen wurden.

## 9. Prozesserzeugungsparameter

```c
typedef struct np_process_create_info {
    uint32_t structure_size;
    uint32_t flags;

    np_string_view_t image_path;
    np_string_list_t arguments;
    np_string_list_t environment;

    np_process_handle_t parent;
    np_job_handle_t job;
    np_process_group_handle_t process_group;

    np_security_profile_t security_profile;
    np_resource_limits_t resource_limits;
} np_process_create_info_t;
```

Alle Strukturen müssen eine Größen- oder Versionsangabe enthalten, damit spätere Erweiterungen ABI-kompatibel umgesetzt werden können.

## 10. Prozess-API

```c
np_status_t np_process_create(
    const np_process_create_info_t* create_info,
    np_process_handle_t* process
);

np_status_t np_process_start(
    np_process_handle_t process
);

np_status_t np_process_suspend(
    np_process_handle_t process
);

np_status_t np_process_resume(
    np_process_handle_t process
);

np_status_t np_process_terminate(
    np_process_handle_t process,
    np_exit_status_t exit_status
);

np_status_t np_process_wait(
    np_process_handle_t process,
    uint64_t timeout_ns,
    np_exit_status_t* exit_status
);
```

Die Trennung zwischen `create` und `start` ermöglicht die vollständige Konfiguration eines Prozesses vor seiner ersten Ausführung.

## 11. Initialer Thread

Jeder ausführbare Prozess benötigt einen initialen Thread.

Der Process Manager erstellt dafür:

- einen Userspace-Stack,
- einen Kernel-Stack,
- einen Registerkontext,
- Thread-lokale Daten,
- den Eintrittspunkt,
- die initialen Argumente,
- die Scheduler-Konfiguration.

Der initiale Thread darf erst in den Zustand `READY` wechseln, nachdem das Prozess-Image vollständig validiert und abgebildet wurde.

## 12. Ausführbare Images

Der Image Loader ist für das Laden ausführbarer Dateien verantwortlich.

Der Process Manager fordert dabei mindestens folgende Prüfungen an:

- gültiges Dateiformat,
- unterstützte Architektur,
- kompatible ABI-Version,
- gültige Segmentgrenzen,
- zulässige Speicherrechte,
- Integritäts- und Signaturstatus,
- Sicherheitsprofil,
- erforderliche Kernel-Services.

Image-Segmente dürfen nicht gleichzeitig beschreibbar und ausführbar sein.

## 13. Adressraum

Jeder reguläre Userspace-Prozess besitzt einen eigenen virtuellen Adressraum.

Der Adressraum enthält typischerweise:

- ausführbaren Programmcode,
- schreibgeschützte Daten,
- veränderliche Daten,
- Heap,
- Thread-Stacks,
- Shared-Memory-Abbildungen,
- Systembibliotheken,
- Userspace-Kernel-Service-Seiten,
- Guard Pages.

Kerneladressbereiche dürfen aus dem Userspace nicht zugänglich sein.

## 14. Prozessvererbung

NovaOS verwendet standardmäßig kein uneingeschränktes `fork()`-Modell.

Neue Prozesse werden aus einem definierten Erzeugungsprofil erstellt. Ressourcen werden nur vererbt, wenn dies ausdrücklich angefordert und erlaubt wurde.

Vererbbar können sein:

- ausgewählte Handles,
- definierte Capabilities,
- Umgebungsvariablen,
- Arbeitsverzeichnis,
- Prozessgruppe,
- Job-Zugehörigkeit,
- Standard-I/O-Verbindungen.

Nicht vererbbar sind standardmäßig:

- administrative Capabilities,
- Debugrechte,
- Treiberrechte,
- Kernelhandles,
- geheime Schlüsselobjekte,
- nicht markierte IPC-Endpunkte.

## 15. Eltern-Kind-Beziehung

Ein Prozess kann einen logischen Elternprozess besitzen.

Die Eltern-Kind-Beziehung dient:

- dem Warten auf Prozessbeendigung,
- der Übergabe ausgewählter Startinformationen,
- der Diagnose,
- der Zuordnung verwaister Prozesse,
- der Anwendung von Job-Richtlinien.

Sie stellt keine automatische Sicherheitsberechtigung dar.

Ein Elternprozess darf sein Kind nur kontrollieren, wenn er passende Capabilities besitzt.

## 16. Verwaiste Prozesse

Endet ein Elternprozess vor seinen Kindern, werden die Kinder nicht automatisch beendet.

Sie werden abhängig von ihrer Richtlinie:

- dem zuständigen Job Object übergeben,
- dem System Service Manager zugeordnet,
- kontrolliert beendet,
- als unabhängige Prozesse fortgeführt.

Das Verhalten muss bei der Prozesserzeugung oder durch das Job Object definiert werden.

## 17. Prozessgruppen

Prozessgruppen fassen logisch zusammengehörige Prozesse zusammen.

Sie unterstützen:

- gemeinsame Ereigniszustellung,
- kontrollierte Gruppenbeendigung,
- Terminal- und Sitzungsverwaltung,
- Diagnoseabfragen,
- Gruppenrichtlinien.

```c
np_status_t np_process_group_create(
    np_process_group_handle_t* group
);

np_status_t np_process_group_add(
    np_process_group_handle_t group,
    np_process_handle_t process
);
```

Die Mitgliedschaft in einer Prozessgruppe ersetzt kein Job Object.

## 18. Job Objects

Job Objects erzwingen gemeinsame Ressourcen- und Lebenszyklusrichtlinien.

Ein Job Object kann festlegen:

- maximale Prozessanzahl,
- CPU-Budget,
- Speicherlimit,
- zulässige Scheduler-Klassen,
- erlaubte CPU-Affinitäten,
- Capability-Obergrenzen,
- I/O-Begrenzungen,
- Verhalten beim Schließen des Job Objects,
- Behandlung untergeordneter Prozesse.

Ein Prozess kann zur gleichen Zeit genau einem primären Job Object angehören.

## 19. Capability-Domain

Jeder Prozess besitzt eine Capability-Domain.

Sie enthält ausschließlich die Rechte, die der Prozess für seine Aufgabe benötigt.

Bei der Prozesserzeugung werden Capabilities aus folgenden Quellen abgeleitet:

- Sicherheitsprofil des Programms,
- Richtlinie des Service Managers,
- Job Object,
- explizite Übergabe durch den Erzeuger,
- systemweite Sicherheitsrichtlinien.

Ein Prozess darf keine Capability erzeugen, die ihm nicht bereits selbst oder über eine autorisierte Delegation zur Verfügung steht.

## 20. Handle-Tabelle

Die Handle-Tabelle eines Prozesses verwaltet Referenzen auf Kernelobjekte.

```c
typedef struct np_handle_entry {
    np_kernel_object_t* object;
    np_capability_rights_t rights;
    uint32_t flags;
    uint32_t generation;
} np_handle_entry_t;
```

Handles enthalten eine Generationskomponente, damit geschlossene und später wiederverwendete Einträge erkannt werden können.

Beim Prozessende werden alle verbleibenden Handles kontrolliert geschlossen.

## 21. Prozessrechte

Mindestens folgende Capability-Rechte werden definiert:

| Capability | Bedeutung |
|---|---|
| `CAP_PROCESS_QUERY` | grundlegende Prozessinformationen lesen |
| `CAP_PROCESS_WAIT` | auf Prozessende warten |
| `CAP_PROCESS_SUSPEND` | Prozess anhalten und fortsetzen |
| `CAP_PROCESS_TERMINATE` | Prozess beenden |
| `CAP_PROCESS_CREATE_THREAD` | zusätzliche Threads erstellen |
| `CAP_PROCESS_VM_READ` | fremden Prozessspeicher lesen |
| `CAP_PROCESS_VM_WRITE` | fremden Prozessspeicher verändern |
| `CAP_PROCESS_DUP_HANDLE` | Handles übertragen oder duplizieren |
| `CAP_PROCESS_DEBUG` | Prozess debuggen |
| `CAP_PROCESS_ADMIN` | administrative Prozessoperationen |

Rechte auf fremden Speicher und Debugoperationen müssen standardmäßig verweigert werden.

## 22. Identität und Sicherheitskontext

Ein Prozess besitzt einen unveränderlichen Basis-Sicherheitskontext.

Dieser kann enthalten:

- Benutzeridentität,
- Dienstidentität,
- Anwendungspaket,
- Sicherheitsprofil,
- Integritätsstufe,
- Sandbox-Kennung,
- Sitzung,
- Vertrauensstatus des Images.

Laufzeitänderungen dürfen nur kontrollierte Teilaspekte betreffen. Eine eigenständige Erhöhung der Vertrauens- oder Integritätsstufe ist unzulässig.

## 23. Umgebungsdaten

Argumente und Umgebungsvariablen werden beim Prozessstart in einen definierten Userspace-Startblock kopiert.

```c
typedef struct np_user_process_start {
    uint32_t structure_size;
    uint32_t abi_version;

    uint32_t argument_count;
    uintptr_t arguments;

    uint32_t environment_count;
    uintptr_t environment;

    np_handle_t standard_input;
    np_handle_t standard_output;
    np_handle_t standard_error;
} np_user_process_start_t;
```

Kernelinterne Zeiger dürfen darin nicht enthalten sein.

## 24. Standard-I/O

Ein Prozess kann folgende Standardkanäle erhalten:

- Standardeingabe,
- Standardausgabe,
- Standardfehlerausgabe.

Diese Kanäle können auf folgende Objekte verweisen:

- Pipes,
- Terminalobjekte,
- Dateien,
- Logging-Endpunkte,
- Nullgerät,
- IPC-Dienste.

Die Kanäle sind reguläre capability-geschützte Handles.

## 25. Thread-Verwaltung

Der Process Manager verwaltet die Zugehörigkeit von Threads zu Prozessen.

Ein Thread kann seinen Prozess nicht wechseln.

Der Prozessstatus wird unter anderem aus seinen Thread-Zuständen abgeleitet:

- besitzt der Prozess ausführbare Threads, gilt er als aktiv,
- sind alle Threads suspendiert, kann der Prozess als suspendiert gelten,
- endet der letzte Thread, wird die Prozessbeendigung eingeleitet.

Das Ende eines einzelnen Threads beendet nicht automatisch den gesamten Prozess, sofern es nicht der definierte Hauptthread oder ein fataler Fehler ist.

## 26. Suspendieren und Fortsetzen

Beim Suspendieren eines Prozesses werden alle zugehörigen Threads angehalten.

Neue Threads dürfen während einer vollständigen Suspendierung nicht ausführbar werden.

Die Operation muss mit gleichzeitigen Thread-Erzeugungen und Thread-Beendigungen synchronisiert werden.

Systemkritische Prozesse dürfen nur mit administrativer Capability suspendiert werden.

## 27. Prozessbeendigung

Eine kontrollierte Prozessbeendigung erfolgt in dieser Reihenfolge:

1. Zustand auf `TERMINATING` setzen,
2. neue Threads und Handles verhindern,
3. alle Threads zur Beendigung auffordern,
4. ausstehende IPC-Operationen abbrechen,
5. Prozess-Exit-Ereignis erzeugen,
6. Handles schließen,
7. Adressraum freigeben,
8. Ressourcenabrechnung abschließen,
9. Exit-Status speichern,
10. Prozess in den Zustand `ZOMBIE` versetzen.

Nach Abholung des Exit-Status und Freigabe aller Referenzen wechselt der Prozess nach `TERMINATED`.

## 28. Unfreiwillige Beendigung

Ein Prozess kann unfreiwillig beendet werden durch:

- unbehandelte Userspace-Exception,
- Sicherheitsverletzung,
- ungültigen Systemaufruf,
- Überschreitung eines harten Ressourcenlimits,
- Job-Object-Richtlinie,
- explizite autorisierte Beendigung,
- kritischen Fehler eines Userspace-Treibers,
- Systemabschaltung.

Der Grund muss im Exit-Status unterscheidbar sein.

## 29. Exit-Status

```c
typedef struct np_exit_status {
    uint32_t reason;
    int32_t code;
    uint64_t detail;
} np_exit_status_t;
```

Mögliche Gründe sind:

```c
typedef enum np_exit_reason {
    NP_EXIT_NORMAL,
    NP_EXIT_TERMINATED,
    NP_EXIT_EXCEPTION,
    NP_EXIT_SECURITY_VIOLATION,
    NP_EXIT_RESOURCE_LIMIT,
    NP_EXIT_JOB_POLICY,
    NP_EXIT_SYSTEM_SHUTDOWN
} np_exit_reason_t;
```

Sensible Kernelinformationen dürfen nicht über `detail` an unberechtigte Prozesse weitergegeben werden.

## 30. Warten auf Prozesse

Prozessobjekte sind wartbare Kernelobjekte.

Ein wartender Thread wird aufgeweckt, sobald der Prozess den Zustand `ZOMBIE` oder `TERMINATED` erreicht.

Mehrere berechtigte Threads dürfen auf denselben Prozess warten.

Ein Timeout verändert den Zustand des Zielprozesses nicht.

## 31. Zombie-Prozesse

Ein Zombie-Prozess enthält nur noch die für Statusabfrage und Diagnose erforderlichen Daten.

Dazu gehören:

- Prozesskennung,
- Exit-Status,
- grundlegende Ressourcenabrechnung,
- Beendigungszeitpunkt,
- minimale Eltern- und Job-Zuordnung.

Adressraum, Threads und reguläre Handles müssen zu diesem Zeitpunkt bereits freigegeben sein.

Zombie-Prozesse dürfen nicht unbegrenzt bestehen bleiben. Der Service Manager muss verwaiste Statusobjekte einsammeln können.

## 32. Ressourcenabrechnung

Der Process Manager erfasst mindestens:

- CPU-Zeit,
- maximale und aktuelle Speichernutzung,
- Anzahl erzeugter Threads,
- Anzahl offener Handles,
- Systemaufrufe,
- IPC-Nutzung,
- I/O-Mengen,
- Start- und Endzeitpunkt.

```c
typedef struct np_resource_accounting {
    uint64_t user_time_ns;
    uint64_t kernel_time_ns;
    uint64_t memory_current;
    uint64_t memory_peak;
    uint64_t io_read_bytes;
    uint64_t io_write_bytes;
    uint64_t system_calls;
} np_resource_accounting_t;
```

Die Erfassung darf nicht zu einer globalen Sperre bei jedem Systemaufruf führen.

## 33. Ressourcenlimits

```c
typedef struct np_resource_limits {
    uint64_t memory_bytes;
    uint64_t cpu_time_ns;
    uint32_t maximum_threads;
    uint32_t maximum_handles;
    uint32_t maximum_children;
    uint32_t flags;
} np_resource_limits_t;
```

Limits können durch Prozessprofil, Job Object oder Systemrichtlinie gesetzt werden.

Ein Prozess darf seine Limits freiwillig reduzieren, aber nicht ohne passende Capability erhöhen.

## 34. Speicherlimit

Speicherlimits umfassen mindestens:

- privaten Prozessspeicher,
- Heap und Stacks,
- private Copy-on-Write-Seiten,
- einen definierten Anteil gemeinsam genutzter Seiten,
- Kernelressourcen im Namen des Prozesses.

Bei Überschreitung kann der Kernel:

- weitere Allokationen ablehnen,
- ein Warnereignis senden,
- Speicherbereinigung anfordern,
- den Prozess bei einem harten Limit beenden.

## 35. Prozess-Namespace

Prozesse können innerhalb eines eingeschränkten Namespace ausgeführt werden.

Ein Namespace kann die Sichtbarkeit begrenzen auf:

- Prozesse,
- Dateisystem-Mounts,
- benannte IPC-Objekte,
- Geräte,
- Netzwerkressourcen,
- Systemdienste.

Prozesskennungen innerhalb eines Namespace dürfen von globalen Kernelkennungen abstrahiert werden.

Capabilities bleiben auch innerhalb eines Namespace verbindlich.

## 36. Systemprozesse

Systemprozesse werden durch den Service Manager anhand signierter oder anderweitig verifizierter Dienstdefinitionen gestartet.

Sie können zusätzliche Rechte erhalten, müssen aber weiterhin:

- ein definiertes Sicherheitsprofil besitzen,
- einem Job Object zugeordnet sein,
- Ressourcenlimits einhalten,
- lokal diagnostizierbar bleiben,
- ausschließlich erforderliche Capabilities erhalten.

Die Kennzeichnung als Systemprozess allein verleiht keine uneingeschränkten Kernelrechte.

## 37. Userspace-Treiberprozesse

Ein Userspace-Treiber wird als speziell isolierter Prozess gestartet.

Er erhält nur Capabilities für:

- zugewiesene Geräte,
- erforderliche MMIO-Bereiche,
- autorisierte Interrupt-Endpunkte,
- kontrollierte DMA-Objekte,
- benötigte IPC-Dienste.

Bei einem Absturz kann der Device Manager den Treiber neu starten, ohne den Kernel oder andere Prozesse neu starten zu müssen.

## 38. Prozessereignisse

Der Process Manager veröffentlicht strukturierte Ereignisse:

```text
PROCESS_CREATED
PROCESS_STARTED
PROCESS_SUSPENDED
PROCESS_RESUMED
PROCESS_TERMINATING
PROCESS_EXITED
PROCESS_RESOURCE_WARNING
PROCESS_SECURITY_VIOLATION
```

Ereignisse enthalten ausschließlich die für den Empfänger zulässigen Informationen.

Die Überwachung fremder Prozesse erfordert passende Capabilities.

## 39. Exception-Behandlung

Userspace-Exceptions werden zunächst an den registrierten Exception-Endpunkt des Prozesses zugestellt.

Mögliche Reaktionen sind:

- Exception behandeln und Thread fortsetzen,
- Thread beenden,
- Prozess beenden,
- autorisierten Debugger benachrichtigen,
- Crash Dump erzeugen.

Kann die Exception nicht sicher zugestellt oder behandelt werden, wird der betroffene Prozess beendet.

Eine Userspace-Exception darf nicht unmittelbar eine Kernel Panic auslösen.

## 40. Debugging

Debugzugriff auf einen Prozess erfordert `CAP_PROCESS_DEBUG` für das konkrete Prozessobjekt.

Ein Debugger kann abhängig von seinen Rechten:

- Threads anhalten,
- Register lesen,
- Speicher lesen,
- Breakpoints setzen,
- Ereignisse empfangen.

Secure-Debugging-Richtlinien können Debugging zusätzlich einschränken oder vollständig deaktivieren.

## 41. Prozess-Image-Austausch

Ein laufender Prozess kann optional sein ausführbares Image ersetzen.

```c
np_status_t np_process_replace_image(
    const np_process_replace_info_t* replace_info
);
```

Dabei werden:

- alle anderen Threads beendet,
- der alte Userspace-Adressraum ersetzt,
- nicht vererbbare Handles geschlossen,
- ein neuer Startkontext erzeugt,
- Sicherheitsrichtlinien erneut geprüft.

Ein Image-Austausch darf keine Privilegienerhöhung ohne erneute Autorisierung bewirken.

## 42. IPC-Integration

IPC-Endpunkte werden als Handles im Prozess-Namespace verwaltet.

Beim Prozessende müssen:

- wartende Partner benachrichtigt,
- offene Nachrichten kontrolliert verworfen oder zurückgegeben,
- Shared-Memory-Abbildungen getrennt,
- RPC-Aufrufe mit einem definierten Fehler beendet werden.

Die Beendigung eines Prozesses darf keine unbegrenzte Blockierung anderer Prozesse verursachen.

## 43. Systemstart

Der erste Userspace-Prozess erhält die Prozesskennung `1`.

Er wird durch den Kernel nach erfolgreicher Kernel-Bootsequenz gestartet und dient als initialer Service Manager.

Der Prozess erhält eine speziell definierte Init-Capability-Domain, aber nicht automatisch die uneingeschränkte Kernel-Root-Capability.

Kann der Init-Prozess nicht gestartet werden, muss der Kernel den Recovery-Pfad aktivieren oder gemäß Kernel-Panic-Policy reagieren.

## 44. Systemabschaltung

Bei einer kontrollierten Systemabschaltung beendet der Process Manager Prozesse in festgelegten Gruppen:

1. interaktive Anwendungen,
2. reguläre Hintergrunddienste,
3. Netzwerk- und Dateidienste,
4. Userspace-Treiber,
5. kritische Systemdienste,
6. Init-Prozess.

Jede Gruppe erhält eine begrenzte Zeit für die kontrollierte Beendigung. Danach dürfen verbleibende Prozesse zwangsweise beendet werden.

## 45. Parallelität und Synchronisation

Prozesszustand, Threadliste, Handle-Tabelle und Adressraum müssen gegen parallele Änderungen geschützt werden.

Der Process Manager muss insbesondere folgende Konflikte behandeln:

- Beendigung während der Thread-Erzeugung,
- Suspendierung während eines Wakeups,
- Handle-Übertragung während der Beendigung,
- gleichzeitiges Warten und Freigeben,
- Prozessabfrage während des Zustandswechsels,
- Job-Wechsel während einer Ressourcenprüfung.

Sperren dürfen nicht über potenziell blockierende Userspace- oder IPC-Operationen gehalten werden.

## 46. Fehlercodes

| Fehlercode | Bedeutung |
|---|---|
| `NP_OK` | Operation erfolgreich |
| `NP_ERR_INVALID_ARGUMENT` | ungültiger Parameter |
| `NP_ERR_INVALID_HANDLE` | ungültiges Prozess-Handle |
| `NP_ERR_INVALID_STATE` | Operation im aktuellen Zustand unzulässig |
| `NP_ERR_ACCESS_DENIED` | erforderliche Capability fehlt |
| `NP_ERR_PROCESS_LIMIT` | Prozesslimit erreicht |
| `NP_ERR_THREAD_LIMIT` | Threadlimit erreicht |
| `NP_ERR_MEMORY_LIMIT` | Speicherlimit überschritten |
| `NP_ERR_IMAGE_INVALID` | ausführbares Image ungültig |
| `NP_ERR_ABI_INCOMPATIBLE` | ABI nicht kompatibel |
| `NP_ERR_TIMEOUT` | Wartezeit abgelaufen |
| `NP_ERR_PROCESS_EXITED` | Zielprozess wurde bereits beendet |

## 47. Sicherheitsanforderungen

Der Process Manager muss sicherstellen, dass:

- Prozesse standardmäßig voneinander isoliert sind,
- Prozesskennungen keine Zugriffsrechte darstellen,
- alle fremden Prozessoperationen Capability-Prüfungen durchlaufen,
- Kernelzeiger niemals an Userspace-Prozesse ausgegeben werden,
- ausführbare Speicherbereiche nicht gleichzeitig beschreibbar sind,
- administrative Handles nicht unbeabsichtigt vererbt werden,
- Ressourcenlimits nicht ohne Autorisierung erhöht werden,
- Prozessbeendigung alle sicherheitsrelevanten Ressourcen freigibt,
- Debugoperationen ausdrücklich autorisiert werden,
- Prozessereignisse keine unzulässigen Informationen offenlegen.

## 48. Diagnose

Der Process Manager stellt lokal abrufbare Diagnoseinformationen bereit:

- Prozesskennung und Zustand,
- Threadanzahl,
- Speicherverbrauch,
- CPU-Zeit,
- Job- und Prozessgruppenzuordnung,
- Startzeit,
- Exit-Status,
- Sicherheitsprofil,
- Anzahl offener Handles.

Sensible Daten wie Argumente, Umgebungsvariablen oder Speicherinhalte erfordern erweiterte Diagnose- oder Debugrechte.

## 49. Testanforderungen

Mindestens folgende Tests sind erforderlich:

1. erfolgreiche Prozesserzeugung,
2. Trennung von Erzeugung und Start,
3. Fehler beim Laden eines ungültigen Images,
4. Ablehnung einer inkompatiblen ABI,
5. Erstellung des initialen Threads,
6. Isolation zweier Prozessadressräume,
7. Capability-basierter Zugriff auf fremde Prozesse,
8. Handle-Vererbung entsprechend der Richtlinie,
9. Verhinderung unerlaubter Handle-Vererbung,
10. Suspendieren und Fortsetzen aller Threads,
11. kontrollierte Prozessbeendigung,
12. unfreiwillige Beendigung nach Exception,
13. Warten auf einen Prozess,
14. Timeout beim Warten,
15. Zustand eines Zombie-Prozesses,
16. Freigabe verwaister Zombie-Prozesse,
17. Einhaltung von Speicherlimits,
18. Einhaltung von Thread- und Handle-Limits,
19. Job-Object-Richtlinien,
20. Prozessgruppenoperationen,
21. Behandlung verwaister Kindprozesse,
22. parallele Beendigung und Thread-Erzeugung,
23. Neustart eines Userspace-Treibers,
24. Start des Init-Prozesses,
25. Recovery bei fehlgeschlagenem Init-Start,
26. vollständige Freigabe aller Prozessressourcen.

## 50. Verbindliche Invarianten

1. Jeder aktive Userspace-Prozess besitzt einen eigenen isolierten Adressraum.
2. Jeder Prozess besitzt genau eine primäre Capability-Domain.
3. Ein Thread gehört während seiner gesamten Lebensdauer genau einem Prozess.
4. Ein Prozess im Zustand `TERMINATED` kann nicht erneut gestartet werden.
5. Prozesskennungen allein erteilen keine Zugriffsrechte.
6. Prozesse können ihre eigenen Privilegien nicht erhöhen.
7. Der letzte aktive Thread leitet die Prozessbeendigung ein.
8. Alle Handles werden spätestens bei der endgültigen Prozessbeendigung geschlossen.
9. Ein Zombie-Prozess enthält keinen aktiven Userspace-Adressraum.
10. Ressourcenlimits eines Job Objects können durch den Prozess nicht überschrieben werden.
11. Ein Userspace-Prozessfehler führt nicht unmittelbar zu einer Kernel Panic.
12. Prozesszustandsänderungen erfolgen atomar und nachvollziehbar.

## 51. Referenzablauf einer Prozesserzeugung

```text
Erzeugungsanfrage
  -> Capability des Erzeugers prüfen
  -> Prozesskennung reservieren
  -> Prozessobjekt anlegen
  -> Job und Prozessgruppe zuordnen
  -> Capability-Domain erzeugen
  -> Adressraum erzeugen
  -> Image prüfen und laden
  -> Handle-Tabelle initialisieren
  -> initialen Thread erstellen
  -> Prozess im Kernel Object Graph registrieren
  -> Zustand READY setzen
  -> Prozess starten
  -> Zustand RUNNING setzen
```

## 52. Zusammenfassung

Der NovaOS Process Manager verwaltet Prozesse als isolierte und capability-geschützte Objekte innerhalb des Kernel Object Graph.

Prozesserzeugung und Prozessstart sind getrennte Vorgänge, sodass Adressraum, Sicherheitsprofil, Capabilities, Handles und Ressourcenlimits vollständig eingerichtet werden können, bevor der erste Thread ausgeführt wird.

Prozessgruppen organisieren logisch zusammengehörige Prozesse, während Job Objects verbindliche Ressourcen- und Lebenszyklusrichtlinien durchsetzen. Fehler eines Userspace-Prozesses bleiben auf dessen Sicherheits- und Ressourcenbereich begrenzt.