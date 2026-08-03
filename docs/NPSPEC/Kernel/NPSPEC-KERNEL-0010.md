# NPSPEC-KERNEL-0010 – Exception Manager

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-KERNEL-0010 |
| Titel | Exception Manager |
| Status | Spezifikation |
| Version | 1.0 |
| Datum | 2026-08-03 |
| Bereich | Kernel / Exception Handling |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | NPSPEC-KERNEL-0001, NPSPEC-KERNEL-0004, NPSPEC-KERNEL-0005, NPSPEC-KERNEL-0007, NPSPEC-KERNEL-0009 |
| Zugehörige ADRs | ADR-SEC-0008, ADR-SEC-0009, ADR-SEC-0010, ADR-DIAG-0003, ADR-DIAG-0005 |

---

## 1. Zweck

Diese Spezifikation definiert den Exception Manager des NovaOS-Kernels.

Der Exception Manager verarbeitet:

- CPU-Exceptions,
- Userspace-Ausnahmeereignisse,
- Speicherzugriffsverletzungen,
- ungültige Instruktionen,
- arithmetische Fehler,
- Debug-Ausnahmen,
- Kernel-Exceptions,
- Exception-Zustellung an Prozesse,
- lokale Wiederherstellung,
- Crash-Dump- und Panic-Übergänge.

## 2. Ziele

Der Exception Manager muss:

- Architekturfehler in ein einheitliches Modell übersetzen,
- Userspace-Fehler auf den betroffenen Thread oder Prozess begrenzen,
- kontrollierte Exception Handler ermöglichen,
- Debugger sicher anbinden,
- Kernel-Recovery nur an definierten Stellen erlauben,
- sensible Daten vor unberechtigter Offenlegung schützen,
- rekursive und verschachtelte Exceptions erkennen,
- nachvollziehbare Diagnoseinformationen erzeugen.

## 3. Abgrenzung

| Komponente | Verantwortung |
|---|---|
| Interrupt Manager | technischer Eintritt über CPU-Vektoren |
| Exception Manager | Klassifizierung und Behandlung von Exceptions |
| Virtual Memory Manager | behandelbare Page Faults und Speicherabbildungen |
| Thread Manager | Threadkontext und Threadbeendigung |
| Process Manager | Prozessbeendigung und Prozess-Exception-Endpunkte |
| Debug API | autorisierte Debuggerkommunikation |
| Crash-Dump-System | persistente Fehleraufzeichnung |
| Panic Manager | nicht wiederherstellbare Kernelfehler |

## 4. Exception-Kategorien

```c
typedef enum np_exception_category {
    NP_EXCEPTION_CPU,
    NP_EXCEPTION_MEMORY,
    NP_EXCEPTION_ARITHMETIC,
    NP_EXCEPTION_INSTRUCTION,
    NP_EXCEPTION_DEBUG,
    NP_EXCEPTION_SECURITY,
    NP_EXCEPTION_RESOURCE,
    NP_EXCEPTION_SOFTWARE,
    NP_EXCEPTION_KERNEL_FATAL
} np_exception_category_t;
```

| Kategorie | Bedeutung |
|---|---|
| `CPU` | allgemeiner Prozessorfehler |
| `MEMORY` | Page Fault oder Speicherzugriffsverletzung |
| `ARITHMETIC` | Division, Überlauf oder Rechenfehler |
| `INSTRUCTION` | ungültige oder nicht erlaubte Instruktion |
| `DEBUG` | Breakpoint, Single Step oder Watchpoint |
| `SECURITY` | Schutz- oder Integritätsverletzung |
| `RESOURCE` | Stack-, Speicher- oder Ressourcenfehler |
| `SOFTWARE` | ausdrücklich ausgelöste Ausnahme |
| `KERNEL_FATAL` | nicht sicher wiederherstellbarer Kernelfehler |

## 5. Exception-Codes

```c
typedef enum np_exception_code {
    NP_EXC_DIVIDE_BY_ZERO,
    NP_EXC_DEBUG,
    NP_EXC_BREAKPOINT,
    NP_EXC_OVERFLOW,
    NP_EXC_BOUND_VIOLATION,
    NP_EXC_INVALID_OPCODE,
    NP_EXC_DEVICE_UNAVAILABLE,
    NP_EXC_DOUBLE_FAULT,
    NP_EXC_INVALID_STATE,
    NP_EXC_SEGMENT_VIOLATION,
    NP_EXC_STACK_FAULT,
    NP_EXC_GENERAL_PROTECTION,
    NP_EXC_PAGE_FAULT,
    NP_EXC_FLOATING_POINT,
    NP_EXC_ALIGNMENT,
    NP_EXC_MACHINE_CHECK,
    NP_EXC_SIMD_FLOATING_POINT,
    NP_EXC_CONTROL_PROTECTION,
    NP_EXC_SECURITY_VIOLATION,
    NP_EXC_RESOURCE_LIMIT,
    NP_EXC_SOFTWARE_RAISED
} np_exception_code_t;
```

Architekturspezifische Fehler werden auf den passendsten generischen Code abgebildet.

## 6. Exception-Quelle

Eine Exception kann entstehen durch:

- eine CPU-Instruktion,
- einen Speicherzugriff,
- eine Schutzprüfung,
- einen Systemaufruf,
- eine Laufzeitbibliothek,
- den Kernel,
- einen Debugger,
- eine explizite Userspace-Anforderung,
- einen Hardware- oder Plattformfehler.

Die Quelle muss im Exception Record nachvollziehbar sein.

## 7. Exception Record

```c
typedef struct np_exception_record {
    uint32_t structure_size;
    uint32_t version;

    np_exception_code_t code;
    np_exception_category_t category;
    uint32_t flags;

    np_process_id_t process_id;
    np_thread_id_t thread_id;
    np_cpu_id_t cpu_id;

    uintptr_t instruction_pointer;
    np_virtual_address_t fault_address;
    uint64_t architecture_error;

    uint64_t parameter_count;
    uint64_t parameters[4];
} np_exception_record_t;
```

Architekturinterne oder sensible Werte dürfen nur an entsprechend berechtigte Empfänger weitergegeben werden.

## 8. Exception Context

Der Exception Context enthält den zur Wiederaufnahme erforderlichen Threadzustand.

```c
typedef struct np_exception_context {
    uint32_t structure_size;
    uint32_t architecture;
    uint32_t flags;
    uint32_t reserved;

    np_user_register_context_t registers;
} np_exception_context_t;
```

Der Userspace erhält niemals ungefilterte Kernelregister oder Kerneladressen.

## 9. Exception-Einstieg

Der architekturspezifische Exception-Einstieg muss:

1. alle erforderlichen Register sichern,
2. einen gültigen Kernel-Stack aktivieren,
3. Fehlercode und Vektor normalisieren,
4. Privilegienstufe des unterbrochenen Kontexts bestimmen,
5. Exception Record erzeugen,
6. den generischen Exception Manager aufrufen,
7. Ergebnis sicher anwenden,
8. Kontext wiederherstellen oder Thread beenden.

Der Low-Level-Einstieg darf nicht von dynamischer Heapallokation abhängig sein.

## 10. Ursprungskontext

Der Exception Manager unterscheidet:

```c
typedef enum np_exception_origin {
    NP_EXCEPTION_ORIGIN_USER,
    NP_EXCEPTION_ORIGIN_KERNEL,
    NP_EXCEPTION_ORIGIN_HYPERVISOR,
    NP_EXCEPTION_ORIGIN_FIRMWARE
} np_exception_origin_t;
```

Eine Userspace-Exception darf nicht allein wegen ihres CPU-Vektors wie ein Kernelfehler behandelt werden.

Der tatsächliche Privilegienkontext ist maßgeblich.

## 11. Userspace-Exceptions

Eine Userspace-Exception wird grundsätzlich auf den verursachenden Thread oder Prozess begrenzt.

Mögliche Reaktionen sind:

- lokal durch den VMM behandeln,
- an einen registrierten Exception-Endpunkt zustellen,
- an einen autorisierten Debugger zustellen,
- den Thread beenden,
- den Prozess beenden,
- einen begrenzten Userspace-Crash-Dump erzeugen.

Eine reguläre Userspace-Exception löst keine unmittelbare Kernel Panic aus.

## 12. Kernel-Exceptions

Eine Kernel-Exception wird nur dann fortgesetzt, wenn ein ausdrücklich registrierter und sicherer Recovery-Pfad existiert.

Beispiele sind:

- kontrollierter Zugriff auf Userspace-Speicher,
- erwarteter Page Fault bei Demand Paging,
- geprüfte Gerätezugriffe,
- begrenzte Probe-Operationen,
- spezielle Debug- oder Recovery-Funktionen.

Ohne gültigen Recovery-Pfad wird die Kernel-Panic-Policy aktiviert.

## 13. Exception Domains

Kernelkomponenten können in Exception Domains organisiert werden.

Eine Domain beschreibt:

- zulässige Exceptionklassen,
- mögliche Recovery-Aktionen,
- maximale Fehleranzahl,
- Isolationsebene,
- zugehörige Diagnoseinformationen.

Userspace-Treiber bilden eigene Prozess- und Capability-Domains. Ihr Absturz darf normalerweise nicht den Kernel beenden.

## 14. Page Faults

Page Faults werden zuerst an den Virtual Memory Manager übergeben.

Der VMM prüft:

- ob die Adresse zu einer Region gehört,
- ob die Zugriffsart erlaubt ist,
- ob Demand Paging möglich ist,
- ob Copy-on-Write vorliegt,
- ob Stackwachstum zulässig ist,
- ob eine Guard Page betroffen ist.

Kann der VMM den Fehler beheben, wird die Instruktion erneut ausgeführt. Andernfalls wird eine Speicher-Exception erzeugt.

## 15. Speicherzugriffsverletzungen

Nicht behandelbare Speicherzugriffe werden klassifiziert als:

- Lesen einer nicht abgebildeten Adresse,
- Schreiben in schreibgeschützten Speicher,
- Ausführen nicht ausführbaren Speichers,
- Userspace-Zugriff auf Kernelspeicher,
- Zugriff auf eine Guard Page,
- ungültige oder nicht kanonische Adresse,
- Alignment-Verletzung.

Die konkrete Ursache wird im Exception Record dokumentiert.

## 16. Stack-Fehler

Ein Stack-Fehler kann entstehen durch:

- Zugriff auf eine Stack Guard Page,
- ungültigen Stack Pointer,
- Überschreiten der maximalen Stackgröße,
- beschädigten Kernel-Stack,
- Stack-Canary-Verletzung.

Ein Userspace-Stacküberlauf führt normalerweise zur Beendigung des betroffenen Threads oder Prozesses.

Ein beschädigter aktiver Kernel-Stack gilt als kritischer Kernelzustand und muss einen Notfall-Stack verwenden.

## 17. Ungültige Instruktionen

Eine Invalid-Opcode-Exception kann bedeuten:

- beschädigter Code,
- nicht unterstützte CPU-Instruktion,
- falsche Architektur,
- absichtliche Trap-Instruktion,
- Angriff auf den Kontrollfluss.

Eine optionale Instruktionsemulation ist nur für ausdrücklich unterstützte Fälle zulässig.

Unbekannte Instruktionen werden nicht allgemein im Kernel emuliert.

## 18. Arithmetische Exceptions

Arithmetische Exceptions umfassen:

- Division durch null,
- Integer-Überlauf, sofern von der Architektur signalisiert,
- x87-Floating-Point-Fehler,
- SIMD-Floating-Point-Fehler,
- benutzerdefinierte numerische Laufzeitfehler.

Der Exception Record enthält die Fehlerklasse, aber keine automatische Interpretation anwendungsspezifischer Daten.

## 19. Device-Not-Available-Exception

Eine Device-Not-Available-Exception kann für verzögerte FPU- oder SIMD-Zustandsverwaltung verwendet werden.

Der Kernel kann:

1. den Zustand des vorherigen Besitzers sichern,
2. den Zustand des aktuellen Threads laden,
3. die Instruktion erneut ausführen.

Ist die Funktion auf der Plattform nicht sicher implementierbar, wird der FPU-Zustand bei jedem Context Switch verwaltet.

## 20. General Protection Fault

Ein General Protection Fault kann unter anderem entstehen durch:

- ungültige Segmentzustände,
- privilegierte Instruktionen im Userspace,
- ungültige Deskriptoren,
- fehlerhafte Kontrollübergänge,
- bestimmte nicht kanonische Zugriffe.

Im Userspace führt dies zu einer Schutzverletzungs-Exception.

Im Kernel ist eine Fortsetzung nur mit einem passenden Recovery-Eintrag zulässig.

## 21. Double Fault

Ein Double Fault bedeutet, dass während der Behandlung einer vorherigen Exception ein weiterer kritischer Fehler auftrat.

Der Handler muss:

- einen separaten Notfall-Stack verwenden,
- weitere komplexe Operationen vermeiden,
- minimale Diagnoseinformationen erfassen,
- Rekursion verhindern,
- die Kernel-Panic-Policy aktivieren.

Eine normale Fortsetzung ist unzulässig.

## 22. Machine Check

Machine-Check-Exceptions signalisieren mögliche Hardwarefehler.

Die Behandlung hängt von der Plattforminformation ab:

- korrigierter Fehler protokollieren,
- betroffene Speicherseite isolieren,
- fehlerhafte CPU oder Gerätedomäne deaktivieren,
- Prozess mit beschädigten Daten beenden,
- System kontrolliert anhalten.

Bei unklarem oder nicht korrigierbarem Kernelzustand wird eine Kernel Panic ausgelöst.

## 23. Debug-Exceptions

Debug-Exceptions umfassen:

- Breakpoints,
- Single Step,
- Hardware-Watchpoints,
- Debugregister-Ereignisse.

Sie werden nur an einen Debugger zugestellt, wenn:

- ein autorisierter Debugger gebunden ist,
- Secure Debugging aktiv ist,
- der Zielprozess Debugging erlaubt,
- die Debug-Capability gültig ist.

Andernfalls werden sie entsprechend ihrer Art behandelt oder der Prozess wird beendet.

## 24. Software-Exceptions

Ein Thread kann eine definierte Software-Exception auslösen:

```c
np_status_t np_exception_raise(
    np_exception_code_t code,
    const uint64_t* parameters,
    size_t parameter_count
);
```

Unprivilegierte Prozesse dürfen ausschließlich für Userspace zugelassene Codes auslösen.

Kernel-Fatal-Codes können nicht durch normale Userspace-Prozesse erzeugt werden.

## 25. Exception-Endpunkt

Ein Prozess kann einen Exception-Endpunkt registrieren.

```c
np_status_t np_exception_endpoint_create(
    uint32_t flags,
    np_exception_endpoint_handle_t* endpoint
);

np_status_t np_process_set_exception_endpoint(
    np_process_handle_t process,
    np_exception_endpoint_handle_t endpoint
);
```

Der Endpunkt empfängt strukturierte Exception-Ereignisse über den IPC-Mechanismus.

## 26. Zustellung an den Exception Handler

Bei der Zustellung wird der betroffene Thread angehalten.

Der Empfänger erhält:

- Exception Record,
- gefilterten Threadkontext,
- eingeschränktes Thread-Handle,
- eindeutige Sequenznummer,
- erlaubte Antwortoperationen.

Der Handler darf nur die im Handle enthaltenen Rechte verwenden.

## 27. Exception-Antwort

```c
typedef enum np_exception_action {
    NP_EXCEPTION_CONTINUE,
    NP_EXCEPTION_RETRY,
    NP_EXCEPTION_TERMINATE_THREAD,
    NP_EXCEPTION_TERMINATE_PROCESS,
    NP_EXCEPTION_FORWARD_TO_DEBUGGER
} np_exception_action_t;

np_status_t np_exception_reply(
    np_exception_token_t token,
    np_exception_action_t action,
    const np_exception_context_t* context
);
```

Eine Fortsetzung ist nur zulässig, wenn der resultierende Kontext vollständig validiert wurde.

## 28. Kontextänderung

Ein Exception Handler darf den Userspace-Kontext nur verändern, wenn er die erforderlichen Rechte besitzt.

Zu prüfen sind:

- gültiger Instruction Pointer,
- gültiger Stack Pointer,
- erlaubte Statusbits,
- korrekte Privilegienstufe,
- zulässige Segment- und TLS-Werte,
- gültige SIMD- und Debugzustände.

Kerneladressen oder privilegierte CPU-Flags müssen abgelehnt werden.

## 29. Exception-Kette

Optional können Exceptions in dieser Reihenfolge verarbeitet werden:

1. interner VMM- oder Kernel-Recovery-Pfad,
2. Prozess-Exception-Endpunkt,
3. autorisierter Debugger,
4. Standard-Exception-Policy,
5. Thread- oder Prozessbeendigung.

Ein Empfänger darf eine Exception nur weiterreichen, wenn die zugehörige Richtlinie dies erlaubt.

Unbegrenzte Weiterleitungsschleifen müssen verhindert werden.

## 30. Timeouts

Ein angehaltener Thread darf nicht unbegrenzt auf einen fehlerhaften Exception Handler warten.

Für Exception-Endpunkte gelten konfigurierbare Timeouts.

Nach einem Timeout kann der Kernel:

- an einen Debugger weiterleiten,
- den Exception Handler als fehlerhaft markieren,
- den betroffenen Thread beenden,
- den gesamten Prozess beenden.

Systemkritische Prozesse verwenden gesonderte Recovery-Richtlinien.

## 31. Fehler im Exception Handler

Erzeugt der Exception Handler selbst eine Exception, darf keine unkontrollierte Rekursion entstehen.

Mögliche Reaktionen sind:

- sekundären Handler verwenden,
- Debugger benachrichtigen,
- Handlerprozess beenden,
- ursprünglichen Zielprozess nach Standardrichtlinie behandeln,
- bei kritischen Systemdiensten Recovery aktivieren.

Ein Prozess darf nicht sein eigener einziger Exception Handler sein, wenn dadurch unauflösbare Rekursion entstehen kann.

## 32. Exception-Tiefe

Jeder Thread und jede CPU besitzt einen Exception-Tiefenzähler.

```c
typedef uint32_t np_exception_depth_t;
```

Eine begrenzte Verschachtelung kann erforderlich sein, beispielsweise für einen Page Fault während kontrollierter Kernelverarbeitung.

Wird die zulässige Tiefe überschritten, gilt der Zustand als nicht sicher wiederherstellbar.

## 33. Kernel-Recovery-Tabellen

Für bestimmte fehleranfällige Kerneloperationen können statische Recovery-Einträge definiert werden.

```c
typedef struct np_exception_fixup {
    uintptr_t fault_start;
    uintptr_t fault_end;
    uintptr_t recovery_address;
    uint32_t allowed_exceptions;
} np_exception_fixup_t;
```

Typische Verwendung ist das sichere Kopieren zwischen Kernel und Userspace.

Recovery-Tabellen müssen schreibgeschützt und während des Builds oder Modul-Ladens validiert werden.

## 34. Sicherer Userspace-Zugriff

Direkter ungeschützter Zugriff auf Userspace-Zeiger ist im Kernel unzulässig.

Es werden kontrollierte Funktionen verwendet:

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

Tritt dabei ein erwarteter Page Fault auf, wird die Operation mit einem definierten Fehler beendet, ohne eine Kernel Panic auszulösen.

## 35. Threadbeendigung

Muss ein Thread beendet werden, übergibt der Exception Manager den Vorgang an den Thread Manager.

Der Thread-Exit-Status enthält mindestens:

- Exception-Code,
- Fehleradresse, soweit freigegeben,
- Instruction Pointer, soweit freigegeben,
- Beendigungsgrund,
- optionale Crash-Dump-Referenz.

Der Exception Manager gibt den aktuell verwendeten Kernel-Stack nicht selbst frei.

## 36. Prozessbeendigung

Eine Exception beendet den gesamten Prozess, wenn:

- die Prozessrichtlinie dies verlangt,
- der Hauptthread fatal fehlschlägt,
- der Prozesszustand nicht mehr vertrauenswürdig ist,
- eine Sicherheitsverletzung mehrere Threads betrifft,
- der Exception Handler dies autorisiert anfordert,
- eine Job- oder Service-Richtlinie dies festlegt.

Die endgültige Ressourcenfreigabe übernimmt der Process Manager.

## 37. Systemkritische Prozesse

Exceptions in systemkritischen Prozessen können folgende Recovery-Aktionen auslösen:

- Neustart des einzelnen Dienstes,
- Start einer Ersatzinstanz,
- Zurücksetzen einer Capability-Domain,
- Neustart eines Userspace-Treibers,
- Wechsel in einen eingeschränkten Modus,
- Aktivierung der Recovery-Umgebung.

Die Kennzeichnung als systemkritisch verhindert nicht automatisch die Beendigung eines beschädigten Prozesses.

## 38. Kernelmodule

Exceptions innerhalb eines Kernelmoduls werden grundsätzlich wie Kernel-Exceptions behandelt.

Eine lokale Modulisolation ist nur möglich, wenn:

- keine kritischen Sperren gehalten werden,
- keine Kerneldaten beschädigt wurden,
- das Modul ausdrücklich isolierbar ist,
- alle zugehörigen Geräte sicher angehalten werden können,
- ein definierter Recovery-Pfad existiert.

Andernfalls wird die Kernel-Panic-Policy angewendet.

## 39. Crash Dumps

Vor einer Prozessbeendigung kann ein Userspace-Crash-Dump erzeugt werden.

Er kann enthalten:

- Exception Record,
- Threadkontext,
- Liste geladener Module,
- ausgewählte Stackbereiche,
- Speicherregionen nach Richtlinie,
- Prozess- und Threadmetadaten.

Bereiche mit `NO_DUMP` oder sensiblen Sicherheitsdaten dürfen nicht ohne besondere Autorisierung aufgenommen werden.

## 40. Kernel Panic

Eine Kernel Panic wird ausgelöst, wenn eine sichere Fortsetzung nicht möglich ist.

Beispiele sind:

- Double Fault,
- beschädigter Kernel-Stack,
- ungültige Kernel-Seitentabellen,
- nicht korrigierbarer Machine Check,
- Exception im Panic Handler,
- kritische Kontrollflussverletzung,
- beschädigte Exception-Verwaltung.

Der Exception Manager übergibt die Kontrolle an den Panic Manager und kehrt nicht zurück.

## 41. Notfallpfad

Der Notfallpfad muss ohne reguläre Heapallokation funktionieren.

Er verwendet:

- vorab reservierten Notfall-Stack,
- statische oder CPU-lokale Diagnosepuffer,
- minimale Ausgabefunktionen,
- direkte Panic- und Crash-Dump-Schnittstellen.

Nicht erforderliche Treiber- oder Dateisystemoperationen sind im Notfallpfad zu vermeiden.

## 42. Capability-Rechte

Mindestens folgende Rechte werden definiert:

| Capability | Bedeutung |
|---|---|
| `CAP_EXCEPTION_QUERY` | eigene Exceptioninformationen lesen |
| `CAP_EXCEPTION_HANDLE` | Exceptions einer zugewiesenen Domain behandeln |
| `CAP_EXCEPTION_CONTEXT_READ` | Userspace-Kontext lesen |
| `CAP_EXCEPTION_CONTEXT_WRITE` | Userspace-Kontext verändern |
| `CAP_EXCEPTION_TERMINATE` | Zielthread oder Zielprozess beenden |
| `CAP_EXCEPTION_DEBUG_FORWARD` | Exception an Debugger weiterreichen |
| `CAP_EXCEPTION_OTHER_PROCESS` | fremde Prozess-Exceptions verwalten |
| `CAP_EXCEPTION_ADMIN` | globale Exception-Richtlinien verwalten |

Capabilities müssen auf konkrete Prozesse, Threads oder Domains beschränkbar sein.

## 43. Sicherheitsanforderungen

Der Exception Manager muss sicherstellen, dass:

- Userspace keine Kernelregister erhält,
- Exception Handler keine privilegierten Kontexte erzeugen,
- Kerneladressen nur nach Sicherheitsrichtlinie offengelegt werden,
- Debug-Exceptions ausschließlich autorisiert weitergeleitet werden,
- Recovery-Pfade nur für ausdrücklich erlaubte Instruktionsbereiche gelten,
- Exception-Tokens nicht wiederverwendet werden,
- veraltete Antworten abgelehnt werden,
- Exception-Endpunkte keine fremden Prozesse ohne Capability kontrollieren,
- sensible Speicherbereiche nicht unkontrolliert in Crash Dumps gelangen,
- rekursive Exceptions begrenzt und erkannt werden.

## 44. Datenschutz

Exception Records und Crash Dumps können personenbezogene oder vertrauliche Daten enthalten.

Daher gelten folgende Regeln:

- Speicherung erfolgt standardmäßig lokal,
- Speicherinhalte werden auf das erforderliche Minimum begrenzt,
- sensible Regionen können als `NO_DUMP` markiert werden,
- Zugriff erfordert Diagnose- oder Debug-Capabilities,
- automatische externe Übertragung ist standardmäßig deaktiviert,
- Aufbewahrungsfristen müssen konfigurierbar sein.

## 45. Diagnose

Der Exception Manager erfasst mindestens:

- Exception-Code und Kategorie,
- Prozess- und Threadkennung,
- CPU-Kennung,
- Ursprungskontext,
- Fehleradresse,
- Instruction Pointer entsprechend der Berechtigung,
- gewählte Behandlungsaktion,
- Zustellungs- und Antwortdauer,
- Ergebnis der Exception-Behandlung.

Wiederholte Exceptions können zusammengefasst oder rate-limitiert werden.

## 46. Statistik

```c
typedef struct np_exception_statistics {
    uint64_t total_exceptions;
    uint64_t user_exceptions;
    uint64_t kernel_exceptions;

    uint64_t handled_exceptions;
    uint64_t terminated_threads;
    uint64_t terminated_processes;
    uint64_t debugger_deliveries;

    uint64_t handler_timeouts;
    uint64_t recursive_exceptions;
    uint64_t kernel_panics;
} np_exception_statistics_t;
```

Statistiken sollen pro Prozess, Exception-Code, CPU und System verfügbar sein.

## 47. Rate Limiting

Fehlerhafte Prozesse können sehr viele identische Exceptions erzeugen.

Der Exception Manager darf:

- wiederholte Diagnosemeldungen zusammenfassen,
- Crash-Dump-Erzeugung begrenzen,
- einen fehlerhaften Prozess frühzeitig beenden,
- Exception-Endpunkte vor Überlastung schützen.

Die eigentliche Exception-Behandlung darf dadurch nicht übersprungen werden.

## 48. Fehlercodes der Exception-API

| Fehlercode | Bedeutung |
|---|---|
| `NP_OK` | Operation erfolgreich |
| `NP_ERR_INVALID_ARGUMENT` | ungültiger Parameter |
| `NP_ERR_INVALID_CONTEXT` | Kontext ist nicht fortsetzbar |
| `NP_ERR_INVALID_TOKEN` | Exception-Token ist ungültig |
| `NP_ERR_TOKEN_EXPIRED` | Antwort kam zu spät |
| `NP_ERR_ACCESS_DENIED` | erforderliche Capability fehlt |
| `NP_ERR_ALREADY_HANDLED` | Exception wurde bereits abgeschlossen |
| `NP_ERR_THREAD_EXITED` | Zielthread wurde beendet |
| `NP_ERR_PROCESS_EXITED` | Zielprozess wurde beendet |
| `NP_ERR_RECURSIVE_EXCEPTION` | unzulässige Exception-Rekursion |
| `NP_ERR_TIMEOUT` | Exception Handler antwortete nicht |
| `NP_ERR_NOT_SUPPORTED` | Aktion wird nicht unterstützt |

## 49. Testanforderungen

Mindestens folgende Tests sind erforderlich:

1. Division durch null im Userspace,
2. ungültige Instruktion im Userspace,
3. General Protection Fault im Userspace,
4. nicht behandelbarer Page Fault,
5. behandelbarer Demand-Page-Fault,
6. Copy-on-Write-Fault,
7. Schreibzugriff auf schreibgeschützte Seite,
8. Ausführung einer NX-Seite,
9. Zugriff auf eine Guard Page,
10. kontrolliertes Stackwachstum,
11. Userspace-Stacküberlauf,
12. Floating-Point-Exception,
13. Software-Exception,
14. Registrierung eines Exception-Endpunkts,
15. Zustellung eines Exception Records,
16. Fortsetzung mit unverändertem Kontext,
17. Fortsetzung mit gültig verändertem Kontext,
18. Ablehnung eines privilegierten Kontexts,
19. Beendigung des Zielthreads,
20. Beendigung des Zielprozesses,
21. Weiterleitung an einen Debugger,
22. Exception-Handler-Timeout,
23. Exception im Exception Handler,
24. Ablehnung eines veralteten Exception-Tokens,
25. sicherer `copy_from_user()`-Fehler,
26. sicherer `copy_to_user()`-Fehler,
27. Kernel-Recovery über Fixup-Tabelle,
28. unerwartete Kernel-Exception,
29. Double-Fault-Notfallpfad,
30. Machine-Check-Behandlung,
31. Prozess-Crash-Dump,
32. Schutz von `NO_DUMP`-Regionen,
33. Exception Rate Limiting,
34. parallele Exceptions auf mehreren CPUs,
35. korrekte Statistik und Diagnose.

## 50. Verbindliche Invarianten

1. Jede Exception wird eindeutig als Userspace- oder Kernel-Exception klassifiziert.
2. Eine reguläre Userspace-Exception löst keine unmittelbare Kernel Panic aus.
3. Ein Kernel-Recovery-Pfad wird nur für ausdrücklich registrierte Instruktionsbereiche verwendet.
4. Der Userspace erhält keine ungefilterten Kernelregister.
5. Ein veränderter Kontext wird vor der Fortsetzung vollständig validiert.
6. Jeder Exception-Token kann höchstens einmal erfolgreich verwendet werden.
7. Ein angehaltener Exception-Thread wird nicht gleichzeitig regulär eingeplant.
8. Double Faults verwenden einen unabhängigen Notfallpfad.
9. Nicht wiederherstellbare Kernel-Exceptions führen zur Kernel-Panic-Policy.
10. Exception Handler kontrollieren nur Prozesse und Threads, für die sie Capabilities besitzen.
11. Crash Dumps respektieren Sicherheits- und `NO_DUMP`-Richtlinien.
12. Exception-Rekursion wird erkannt und begrenzt.

## 51. Referenzablauf einer Userspace-Exception

```text
CPU meldet Exception
  -> Architekturcode sichert Register
  -> Exception Manager klassifiziert Ursprung
  -> internen Recovery-Pfad prüfen
  -> VMM-Behandlung prüfen
  -> Exception Record erzeugen
  -> Zielthread anhalten
  -> Prozess-Exception-Endpunkt benachrichtigen
  -> Antwort und Kontext validieren
  -> Thread fortsetzen oder beenden
  -> Diagnose und Statistik aktualisieren
```

## 52. Referenzablauf einer Kernel-Exception

```text
CPU meldet Kernel-Exception
  -> Notfallfähigkeit des Stacks prüfen
  -> Exception Record erzeugen
  -> registrierten Fixup- oder Recovery-Pfad suchen
  -> falls sicher: definierten Fehler zurückgeben
  -> andernfalls minimale Diagnose sichern
  -> Crash-Dump-System benachrichtigen
  -> Kernel-Panic-Policy aktivieren
  -> System sicher anhalten, neu starten oder Recovery beginnen
```

## 53. Zusammenfassung

Der NovaOS Exception Manager übersetzt architekturspezifische CPU-Fehler in ein einheitliches, capability-geschütztes Exception-Modell.

Userspace-Exceptions werden zunächst durch Speicherverwaltung, Prozess-Exception-Endpunkte oder autorisierte Debugger behandelt. Nicht behandelbare Fehler bleiben auf den betroffenen Thread oder Prozess begrenzt.

Kernel-Exceptions dürfen nur an ausdrücklich definierten Stellen wiederhergestellt werden. Ist die Integrität des Kernels nicht mehr gewährleistet, wechselt der Exception Manager in den reservierten Notfall-, Crash-Dump- und Kernel-Panic-Pfad.