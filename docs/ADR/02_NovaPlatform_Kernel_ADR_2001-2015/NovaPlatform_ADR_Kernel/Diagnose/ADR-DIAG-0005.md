# ADR-DIAG-0005 – Debug API

| Feld | Wert |
|---|---|
| Dokument-ID | ADR-DIAG-0005 |
| Titel | Debug API |
| Status | Angenommen |
| Version | 1.0 |
| Datum | 2026-07-31 |
| Bereich | Diagnose, Prozesskontrolle und Debugging |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | ADR-DIAG-0001, ADR-DIAG-0002, ADR-DIAG-0003, ADR-DIAG-0004, ADR-SEC-0010, ADR-KERNEL-0105 |
| Nachfolgende Spezifikation | NPSPEC-DIAG-DEBUG-API |

---

## 1. Kontext

NovaOS benötigt eine stabile Schnittstelle für Debugger, Entwicklungswerkzeuge und autorisierte Diagnosekomponenten.

Die Debug API soll unter anderem ermöglichen:

- Prozesse und Threads untersuchen
- Ausführung anhalten und fortsetzen
- Register lesen und verändern
- Speicher lesen und kontrolliert schreiben
- Breakpoints und Watchpoints setzen
- Exceptions und Signals beobachten
- Module und Symbole zuordnen
- Debug-Ereignisse empfangen
- Crash Dumps und Traces korrelieren

Debugging besitzt weitreichende Rechte und muss strikt vom normalen Anwendungsbetrieb getrennt bleiben.

---

## 2. Problemstellung

Ein Debugger kann den vollständigen Zustand eines Prozesses einsehen und verändern. Dazu können Passwörter, Schlüssel, Dokumente und Sicherheitstoken gehören.

Eine unkontrollierte Debug API könnte:

- Prozessisolation umgehen
- Berechtigungen erweitern
- Code in fremde Prozesse einschleusen
- Sicherheitsdienste manipulieren
- Kernel-ASLR offenlegen
- DRM- oder Schlüsselmaterial auslesen
- Produktionssysteme destabilisieren

NovaOS benötigt deshalb ein capability-basiertes und sichtbar autorisiertes Debugmodell.

---

## 3. Entscheidung

NovaOS implementiert eine versionierte Debug API auf Grundlage von Debug Sessions.

Eine Debug Session ist ein capability-geschütztes Kernelobjekt, das genau festlegt:

- welcher Debugger beteiligt ist,
- welches Ziel untersucht wird,
- welche Rechte gewährt wurden,
- welche Sicherheitsdomäne gilt,
- welche Ereignisse abonniert sind,
- wie lange die Session gültig bleibt.

Userspace- und Kernel-Debugging werden getrennt behandelt. Eine Userspace-Debug-Capability erteilt niemals Zugriff auf den Kernel.

---

## 4. Architektur

```text
Debugger
   |
   v
Debug Session
   |
   +--> Prozesskontrolle
   +--> Threadzustände
   +--> Speicherzugriff
   +--> Registerzugriff
   +--> Breakpoints
   +--> Debug Events
   |
   v
Zielprozess
```

Der Kernel vermittelt alle Operationen und prüft die Rechte der Debug Session.

Direkte unkontrollierte Zugriffe zwischen Debugger und Zielprozess sind nicht zulässig.

---

## 5. Debug-Ziele

Die API unterstützt mindestens:

| Ziel | Bedeutung |
|---|---|
| Process | vollständiger autorisierter Prozess |
| Thread | einzelner Thread |
| Job Object | mehrere zusammengehörige Prozesse |
| Child Process Tree | Ziel und neu erzeugte Kindprozesse |
| Crash Dump | offline gespeicherter Systemzustand |
| Kernel | getrennte Secure-Debugging-Schnittstelle |

Systemprozesse, Sicherheitsdienste und der Kernel benötigen besondere Schutzklassen.

---

## 6. Debug Session

```c
typedef struct debug_session_info {
    uint64_t session_id;
    process_id_t debugger_process;
    process_id_t target_process;
    uint64_t granted_rights;
    uint32_t state;
    uint32_t flags;
    uint64_t created_at;
    uint64_t expires_at;
} debug_session_info_t;
```

Eine Session besitzt zusätzlich:

- Zielgeneration
- Debug-Event-Queue
- Breakpointobjekte
- angehaltene Threads
- Sicherheitsrichtlinie
- Auditkontext
- Referenzanzahl

---

## 7. Session-Erzeugung

```c
status_t debug_session_attach(
    cap_handle_t target_process,
    uint64_t requested_rights,
    uint32_t flags,
    cap_handle_t *session_handle
);
```

Vor der Erzeugung prüft NovaOS:

1. Zielprozess-Capability,
2. Debugberechtigung,
3. Benutzer- und Sicherheitsdomäne,
4. Zielschutzklasse,
5. Secure-Debugging-Modus,
6. angeforderte Rechte,
7. administrative Richtlinien,
8. Auditpflicht.

Die Prozess-ID allein erlaubt kein Debugging.

---

## 8. Start unter Debuggerkontrolle

Ein Prozess kann direkt als Debug-Ziel gestartet werden:

```c
status_t debug_process_create(
    const process_create_info_t *process_info,
    uint64_t debug_rights,
    cap_handle_t *process_handle,
    cap_handle_t *session_handle
);
```

Dadurch kann der Debugger Ereignisse bereits vor der ersten Userspace-Instruktion empfangen.

Dieser Weg wird für Entwicklungswerkzeuge gegenüber einem nachträglichen Attach bevorzugt.

---

## 9. Sessionzustände

```text
CREATED
   |
   v
ATTACHED
   |
   +--> RUNNING
   |
   +--> STOPPED
   |
   +--> DETACHING
   |
   v
CLOSED
```

Ein Zielprozess kann zusätzlich enden oder die Debugberechtigung verlieren.

Alle Zustandsübergänge werden atomar koordiniert.

---

## 10. Debug-Ereignisse

Der Kernel erzeugt mindestens folgende Ereignisse:

| Ereignis | Bedeutung |
|---|---|
| `PROCESS_CREATED` | Zielprozess wurde erzeugt |
| `PROCESS_EXITED` | Prozess wurde beendet |
| `THREAD_CREATED` | Thread wurde erzeugt |
| `THREAD_EXITED` | Thread wurde beendet |
| `MODULE_LOADED` | Modul wurde geladen |
| `MODULE_UNLOADED` | Modul wurde entfernt |
| `BREAKPOINT_HIT` | Breakpoint wurde erreicht |
| `WATCHPOINT_HIT` | Watchpoint wurde ausgelöst |
| `EXCEPTION_RAISED` | Exception ist aufgetreten |
| `SIGNAL_RECEIVED` | Signal soll zugestellt werden |
| `PROCESS_STOPPED` | Prozess wurde angehalten |
| `DEBUG_OUTPUT` | kontrollierte Debugausgabe liegt vor |

Ereignisse enthalten keine ungeschützten Kernelzeiger.

---

## 11. Ereigniswarteschlange

Debug-Ereignisse werden in einer begrenzten Sessionwarteschlange gespeichert.

```c
status_t debug_event_wait(
    cap_handle_t session_handle,
    debug_event_t *event,
    uint64_t timeout_ns
);
```

Der Debugger bestätigt die Behandlung über:

```c
status_t debug_event_continue(
    cap_handle_t session_handle,
    uint64_t event_id,
    uint32_t disposition
);
```

Mögliche Dispositionen:

- Exception behandelt
- Exception nicht behandelt
- Thread fortsetzen
- Prozess fortsetzen
- Prozess beenden

Eine volle Warteschlange darf den Kernel nicht unbegrenzt blockieren.

---

## 12. Prozess und Threads anhalten

```c
status_t debug_process_stop(
    cap_handle_t session_handle
);

status_t debug_process_continue(
    cap_handle_t session_handle
);

status_t debug_thread_stop(
    cap_handle_t session_handle,
    thread_id_t thread_id
);

status_t debug_thread_continue(
    cap_handle_t session_handle,
    thread_id_t thread_id
);
```

Threads werden an sicheren Unterbrechungspunkten angehalten.

Nicht unterbrechbare Kernelabschnitte werden zunächst kontrolliert abgeschlossen.

Ein Thread darf nicht mit halb veränderter Kerneloperation sichtbar angehalten werden.

---

## 13. Konsistenter Stop

Für bestimmte Diagnoseoperationen kann ein konsistenter Prozessstopp angefordert werden.

Dabei:

1. werden neue Threads vorübergehend blockiert,
2. werden alle Zielthreads angehalten,
3. werden laufende Debugereignisse stabilisiert,
4. wird der virtuelle Adressraum gegen strukturelle Änderungen geschützt,
5. erhält der Debugger eine konsistente Zielgeneration.

Ein konsistenter Stop besitzt einen Timeout.

Nicht reagierende Threads führen zu einem unvollständigen oder fehlgeschlagenen Stopstatus.

---

## 14. Registerzugriff

```c
status_t debug_thread_get_context(
    cap_handle_t session_handle,
    thread_id_t thread_id,
    debug_thread_context_t *context
);

status_t debug_thread_set_context(
    cap_handle_t session_handle,
    thread_id_t thread_id,
    const debug_thread_context_t *context
);
```

Der Kontext enthält architekturspezifisch versionierte Registergruppen.

Der Kernel validiert beim Schreiben insbesondere:

- Instruction Pointer
- Stack Pointer
- Statusregister
- Segment- oder Privilegienwerte
- Debugregister
- SIMD- und Extended State
- kanonische Adressen

Ein Debugger darf über Registeränderungen nicht in Kernelprivilegien wechseln.

---

## 15. Speicher lesen

```c
status_t debug_memory_read(
    cap_handle_t session_handle,
    uintptr_t target_address,
    void *local_buffer,
    uint64_t size,
    uint64_t *bytes_read
);
```

Der Kernel prüft:

- Zieladressraum
- zulässigen Bereich
- Seitentabellen
- Speicherrechte
- Zielgeneration
- Größenüberläufe
- Debugberechtigung

Teilweise Leseoperationen sind möglich, wenn ein Bereich nur teilweise zugänglich ist.

Kerneladressen bleiben für Userspace-Debug-Sessions unzugänglich.

---

## 16. Speicher schreiben

```c
status_t debug_memory_write(
    cap_handle_t session_handle,
    uintptr_t target_address,
    const void *local_buffer,
    uint64_t size,
    uint64_t *bytes_written
);
```

Schreibzugriff benötigt ein getrenntes Recht.

Für ausführbare Seiten gelten zusätzlich:

- Zielprozess muss angehalten sein,
- Codeänderung muss autorisiert sein,
- Instruction Caches werden synchronisiert,
- W^X bleibt grundsätzlich erhalten,
- temporäre Schreibfreigaben werden unmittelbar zurückgenommen,
- Integritätsrichtlinien können die Änderung vollständig verbieten.

Geschützte Systemmodule dürfen im Produktionsmodus nicht gepatcht werden.

---

## 17. Software-Breakpoints

Software-Breakpoints ersetzen eine Instruktion kontrolliert durch eine Trap-Instruktion.

Der Debug Manager speichert:

- ursprüngliche Bytes
- Zieladresse
- Modul- und Prozessgeneration
- Eigentümer-Session
- Aktivierungszustand

Beim Entfernen wird die ursprüngliche Instruktion nur wiederhergestellt, wenn Ziel und Generation weiterhin übereinstimmen.

Mehrere Sessions dürfen nicht unkoordiniert denselben Codebereich verändern.

---

## 18. Hardware-Breakpoints und Watchpoints

Wenn die CPU Debugregister unterstützt, können Hardware-Breakpoints verwendet werden für:

- Instruktionsausführung
- Speicherlesen
- Speicherschreiben
- Speicherzugriff
- definierte Adresslängen

Die Anzahl ist hardwareseitig begrenzt.

Der Kernel verwaltet Debugregister bei Context Switches und verhindert, dass ein Prozess die Watchpoints eines anderen Prozesses ausliest oder verändert.

---

## 19. Single Step

Ein angehaltener Thread kann genau einen architekturspezifischen Ausführungsschritt ausführen:

```c
status_t debug_thread_step(
    cap_handle_t session_handle,
    thread_id_t thread_id,
    uint32_t flags
);
```

Der nächste Stopp wird als Debug-Ereignis gemeldet.

Signals, Exceptions, Systemaufrufe und Threadwechsel können das beobachtbare Single-Step-Verhalten beeinflussen und müssen eindeutig gemeldet werden.

---

## 20. Exceptions und Signals

Der Debugger kann eine Exception beobachten, bevor die normale Prozessbehandlung erfolgt.

Er entscheidet innerhalb seiner Rechte:

- als behandelt markieren,
- unverändert an den Prozess weitergeben,
- Signal verändern, sofern erlaubt,
- Prozess oder Thread beenden.

Nicht behandelbare Kernel- oder Sicherheitssignale dürfen nicht durch einen gewöhnlichen Userspace-Debugger unterdrückt werden.

Debugging darf die verbindlichen Signalregeln nicht umgehen.

---

## 21. Module und Symbole

Die Debug API stellt Modulmetadaten bereit:

- Modul-ID
- Build-ID
- Ladeadresse in geschützter Form
- Größe
- Architektur
- Symbolpaketkennung
- Debug-Informationskennung

Symboltabellen werden bevorzugt im Debugger und nicht im Kernel verarbeitet.

Der Kernel stellt nur die zur sicheren Zuordnung notwendigen Metadaten bereit.

---

## 22. Kindprozesse

Eine Session kann optional Kindprozesse verfolgen.

Mögliche Modi:

- nur aktueller Prozess
- direkte Kindprozesse
- vollständiger Prozessbaum
- Prozesse innerhalb eines Job Objects

Jeder neue Prozess wird vor der ersten Ausführung mit einem Ereignis gemeldet, sofern der gewählte Modus dies verlangt.

Capability- und Sicherheitsgrenzen werden für jeden neuen Prozess erneut geprüft.

---

## 23. Detach

```c
status_t debug_session_detach(
    cap_handle_t session_handle,
    uint32_t flags
);
```

Beim Detach:

1. werden Software-Breakpoints entfernt,
2. werden Hardware-Debugregister bereinigt,
3. werden Debugereignisse abgeschlossen,
4. werden angehaltene Threads entsprechend der Richtlinie fortgesetzt,
5. werden Sessionreferenzen freigegeben,
6. wird ein Audit-Eintrag erzeugt, falls erforderlich.

Schlägt das Wiederherstellen eines Breakpoints fehl, wird der Zielprozess nicht unkontrolliert fortgesetzt.

---

## 24. Debugger-Ausfall

Beendet sich der Debugger unerwartet, wendet NovaOS eine bei Sessionstart festgelegte Richtlinie an:

| Richtlinie | Verhalten |
|---|---|
| Continue | Zielprozess fortsetzen |
| Terminate | Zielprozess beenden |
| Hold | Ziel für begrenzte Zeit angehalten lassen |
| Delegate | Session an autorisierten Debugdienst übergeben |

`Hold` besitzt immer einen Timeout.

Geschützte Prozesse verwenden eine vom System festgelegte Fail-safe-Richtlinie.

---

## 25. Protected Processes

Sicherheitskritische Prozesse können als `PROTECTED_DEBUG` markiert werden.

Dazu gehören möglicherweise:

- Key Store
- Authentifizierungsdienst
- Auditdienst
- Secure-Startup-Komponenten
- Kernelnahe Sicherheitsdienste
- DRM- oder Geheimnisspeicher, sofern vorhanden

Debugging benötigt dafür eine besondere Capability und einen aktivierten Secure-Debugging-Modus.

Ein gewöhnliches Administratorrecht allein genügt nicht automatisch.

---

## 26. Kernel-Debugging

Kernel-Debugging ist nicht Bestandteil einer gewöhnlichen Userspace-Debug-Session.

Es verwendet:

- getrennte Kernel-Debug-Capabilities,
- Secure-Debugging-Richtlinien,
- geschützte lokale oder physische Aktivierung,
- eingeschränkte Transportwege,
- sichtbaren Systemzustand,
- zusätzliche Auditierung.

Im Produktionsmodus ist schreibendes Kernel-Debugging standardmäßig deaktiviert.

Eine Userspace-Debug API darf keine Kerneladressräume lesen oder verändern.

---

## 27. Capability-Rechte

| Recht | Bedeutung |
|---|---|
| `DEBUG_RIGHT_ATTACH` | Session an Ziel anhängen |
| `DEBUG_RIGHT_CONTROL` | Ziel anhalten und fortsetzen |
| `DEBUG_RIGHT_READ_REGISTERS` | Register lesen |
| `DEBUG_RIGHT_WRITE_REGISTERS` | Register verändern |
| `DEBUG_RIGHT_READ_MEMORY` | Zielspeicher lesen |
| `DEBUG_RIGHT_WRITE_MEMORY` | Zielspeicher verändern |
| `DEBUG_RIGHT_BREAKPOINT` | Breakpoints verwalten |
| `DEBUG_RIGHT_SIGNAL` | Exception- und Signalbehandlung beeinflussen |
| `DEBUG_RIGHT_CHILDREN` | Kindprozesse verfolgen |
| `DEBUG_RIGHT_PROTECTED` | besonders geschützte Prozesse debuggen |
| `DEBUG_RIGHT_KERNEL` | getrennte Kernel-Debug-Funktionen verwenden |

Rechte werden pro Session festgelegt und können nachträglich nur reduziert werden.

---

## 28. Ressourcenbegrenzung

Begrenzt werden mindestens:

- Debug Sessions
- Debugger pro Ziel
- Breakpoints und Watchpoints
- Eventwarteschlange
- Speicherübertragungsgröße
- Stopdauer
- Single-Step-Ereignisrate
- verfolgte Kindprozesse
- Debugausgabe
- Sessions pro Benutzer und Sicherheitsdomäne

Ein Debugger darf durch angehaltene Prozesse keine unbegrenzten Kernelressourcen binden.

---

## 29. Sicherheit und Datenschutz

Für die Debug API gelten folgende Regeln:

- Prozess-IDs stellen keine Debugberechtigung dar.
- Jede Session benötigt eine Capability.
- Userspace- und Kernel-Debugging bleiben getrennt.
- Registerkontexte werden vor dem Schreiben vollständig validiert.
- W^X darf durch Speicherpatches nicht dauerhaft aufgehoben werden.
- geschützte Prozesse benötigen besondere Rechte.
- Debuggerzugriff kann Secrets des Zielprozesses offenlegen und wird entsprechend geschützt.
- Sessions besitzen Eigentümer, Lebenszeit und Auditkontext.
- Netzwerk-Debugging ist standardmäßig deaktiviert.
- Debugdaten werden nicht automatisch extern übertragen.

---

## 30. Audit

Auditiert werden insbesondere:

- Attach an fremde Prozesse,
- Debugging geschützter Prozesse,
- schreibender Speicher- oder Registerzugriff,
- Codepatches und Software-Breakpoints,
- Zugriff auf Kernel-Debugging,
- Aktivierung entfernter Debugtransporte,
- Sessiondelegation,
- fehlgeschlagene Autorisierungsversuche.

Speicherinhalte, Registergeheimnisse und private Schlüssel werden nicht in den Auditdatensatz kopiert.

---

## 31. Fehlercodes

| Fehlercode | Bedeutung |
|---|---|
| `STATUS_OK` | Operation erfolgreich |
| `STATUS_DEBUG_DENIED` | Debugging ist nicht autorisiert |
| `STATUS_DEBUG_TARGET_PROTECTED` | Ziel besitzt besonderen Schutz |
| `STATUS_DEBUG_SESSION_CLOSED` | Session ist nicht mehr aktiv |
| `STATUS_DEBUG_TARGET_EXITED` | Zielprozess wurde beendet |
| `STATUS_DEBUG_THREAD_RUNNING` | Operation erfordert angehaltenen Thread |
| `STATUS_DEBUG_CONTEXT_INVALID` | Registerkontext ist ungültig |
| `STATUS_DEBUG_MEMORY_INVALID` | Zielbereich ist nicht zugänglich |
| `STATUS_BREAKPOINT_CONFLICT` | Breakpoint kollidiert mit anderer Session |
| `STATUS_EVENT_QUEUE_FULL` | Debug-Ereigniswarteschlange ist voll |
| `STATUS_GENERATION_CHANGED` | Zielzustand hat sich verändert |
| `STATUS_LIMIT_EXCEEDED` | Debugressourcenlimit überschritten |

---

## 32. Alternative Ansätze

### 32.1 Direkter Speicherzugriff über Prozess-IDs

**Vorteile:**

- einfache Debuggerimplementierung

**Nachteile:**

- Prozess-ID wird faktisch zur Berechtigung,
- keine feingranulare Kontrolle.

**Entscheidung:** Abgelehnt.

### 32.2 Debugging vollständig über Signals

**Vorteile:**

- Nutzung eines bestehenden Mechanismus

**Nachteile:**

- unzureichend für Speicher, Register und Ereignisse,
- schwache Sessionsemantik.

**Entscheidung:** Signals können Debugereignisse ergänzen, ersetzen die Debug API aber nicht.

### 32.3 Immer aktiviertes Kernel-Debugging

**Vorteile:**

- einfache Fehleranalyse.

**Nachteile:**

- erhebliche Angriffsfläche,
- Schwächung von Kernelisolation und ASLR.

**Entscheidung:** Im Produktionsmodus abgelehnt.

---

## 33. Konsequenzen

### Positive Konsequenzen

- stabile native Schnittstelle für Debugger,
- capability-basierte Autorisierung,
- kontrollierter Speicher- und Registerzugriff,
- Software- und Hardware-Breakpoints,
- strukturierte Debug-Ereignisse,
- Unterstützung für Kindprozesse und Job Objects,
- klare Trennung von Userspace- und Kernel-Debugging.

### Negative Konsequenzen

- Debug Sessions und konsistente Stops sind komplex,
- Breakpoints verändern vorübergehend Zielcode,
- Debugging kann das Zeitverhalten stark beeinflussen,
- geschützte Prozesse benötigen zusätzliche Richtlinien.

### Risiken

- Offenlegung sensibler Zieldaten,
- Privilegienerweiterung durch fehlerhafte Registervalidierung,
- beschädigter Code nach Breakpointfehlern,
- dauerhaft angehaltene Prozesse,
- Sessionübernahme,
- Umgehung von W^X oder Kernelisolation.

---

## 34. Implementierungsrichtlinien

- Jede Debugbeziehung wird durch eine Debug Session repräsentiert.
- Prozess- und Thread-IDs dienen nur zur Identifikation.
- Debugrechte werden bei Sessionstart festgelegt und nicht erweitert.
- Threads werden nur an sicheren Punkten angehalten.
- Registeränderungen werden vollständig validiert.
- Schreibzugriffe auf Code verwenden temporäre, kontrollierte Berechtigungswechsel.
- Software-Breakpoints speichern Zielgeneration und Originalbytes.
- Kernel-Debugging bleibt von Userspace-Debugging getrennt.
- Geschützte Prozesse benötigen Secure-Debugging-Autorisierung.
- Debuggerausfall verwendet eine vorab definierte Fail-safe-Richtlinie.
- Debugdaten werden nicht automatisch exportiert.

---

## 35. Testanforderungen

Die Implementierung benötigt mindestens Tests für:

- Attach und Start unter Debuggerkontrolle,
- Prozess- und Threadstopp,
- konsistenten Gesamtstopp,
- Lesen und Schreiben von Registern,
- Lesen und Schreiben von Speicher,
- Software-Breakpoints,
- Hardware-Breakpoints und Watchpoints,
- Single Step,
- Exceptions und Signals,
- Modulereignisse,
- Kindprozesse,
- Debuggerabsturz,
- Detach,
- geschützte Prozesse,
- Capability- und Ressourcenlimits,
- 32-Bit-/64-Bit-Debugging.

---

## 36. Verbindliche Invarianten

1. Jede Debugbeziehung besitzt eine capability-geschützte Debug Session.
2. Prozess- und Thread-IDs stellen keine Debugberechtigung dar.
3. Userspace-Debugging erteilt keinen Zugriff auf den Kernel.
4. Debugrechte können nach Sessionstart nur reduziert werden.
5. Threads werden nicht in inkonsistenten Kerneloperationen sichtbar angehalten.
6. Registerkontexte werden vor dem Schreiben vollständig validiert.
7. Codepatches dürfen W^X nicht dauerhaft aufheben.
8. Geschützte Prozesse benötigen eine besondere Secure-Debugging-Freigabe.
9. Debuggerausfall folgt einer vorab festgelegten Fail-safe-Richtlinie.
10. Debugdaten und Zielprozessinhalte werden nicht automatisch extern übertragen.

---

## 37. Zusammenfassung

NovaOS implementiert eine versionierte, capability-basierte Debug API auf Grundlage expliziter Debug Sessions.

Autorisierte Debugger können Prozesse und Threads anhalten, Register und Speicher untersuchen, Breakpoints setzen sowie Exceptions und andere Debug-Ereignisse behandeln. Alle Rechte sind an ein konkretes Ziel und eine begrenzte Session gebunden.

Userspace- und Kernel-Debugging bleiben strikt getrennt. Geschützte Prozesse, schreibende Codeänderungen und Kernelzugriffe benötigen zusätzliche Secure-Debugging-Autorisierung und werden sicherheitsrelevant auditiert.