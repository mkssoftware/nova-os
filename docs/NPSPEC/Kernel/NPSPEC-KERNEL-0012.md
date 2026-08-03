# NPSPEC-KERNEL-0012 – Object Manager

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-KERNEL-0012 |
| Titel | Object Manager |
| Status | Spezifikation |
| Version | 1.0 |
| Datum | 2026-08-03 |
| Bereich | Kernel / Objektverwaltung |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | NPSPEC-KERNEL-0001, NPSPEC-KERNEL-0004, NPSPEC-KERNEL-0005, NPSPEC-KERNEL-0008, NPSPEC-KERNEL-0011 |
| Zugehörige ADRs | ADR-KERNEL-0100, ADR-KERNEL-0102, ADR-KERNEL-0103, ADR-IPC-0005, ADR-VFS-0007 |

---

## 1. Zweck

Diese Spezifikation definiert den Object Manager des NovaOS-Kernels.

Der Object Manager stellt das einheitliche Objektmodell für Kernel und Userspace bereit und verwaltet:

- Kernelobjekte,
- Objekttypen,
- Objektidentitäten,
- Referenzzählung,
- Handles,
- Capability-Rechte,
- Namespaces,
- benannte Objekte,
- Objektbeziehungen,
- Lebenszyklus und Zerstörung,
- Events und Diagnoseinformationen.

## 2. Ziele

Der Object Manager muss:

- eine gemeinsame Grundlage für Kernelressourcen schaffen,
- isolierte Subsystem-Sondermodelle vermeiden,
- typsichere Objektzugriffe ermöglichen,
- Capability-Prüfungen zentral unterstützen,
- Objektlebenszyklen nachvollziehbar verwalten,
- sichere Handle-Übertragung ermöglichen,
- benannte und anonyme Objekte unterstützen,
- Kernel Object Graph und Namespaces bereitstellen,
- parallele Zugriffe auf Mehrprozessorsystemen erlauben.

## 3. Grundprinzip

Jede dauerhaft verwaltete Kernelressource wird als Kernelobjekt dargestellt.

Beispiele sind:

- Prozesse,
- Threads,
- Jobs,
- Prozessgruppen,
- Dateien,
- Verzeichnisse,
- Geräte,
- Treiber,
- IPC-Endpunkte,
- Events,
- Shared-Memory-Objekte,
- Sockets,
- Timer,
- Namespaces,
- Capability-Domains.

Ein Subsystem darf keine öffentlich referenzierbare Ressource außerhalb des einheitlichen Objektmodells verwalten.

## 4. Kernelobjekt

```c
typedef struct np_kernel_object {
    np_object_header_t header;
    void* type_data;
} np_kernel_object_t;
```

Der öffentliche Userspace erhält niemals direkte Zeiger auf Kernelobjekte.

Objekte werden ausschließlich über Handles, Capabilities oder interne referenzgezählte Kernelzeiger angesprochen.

## 5. Objekt-Header

```c
typedef struct np_object_header {
    uint64_t object_id;
    uint32_t type_id;
    uint32_t flags;

    np_atomic_uint64_t strong_references;
    np_atomic_uint64_t weak_references;
    np_atomic_uint32_t handle_count;

    np_object_type_t* type;
    np_namespace_entry_t* namespace_entry;

    np_spinlock_t lock;
} np_object_header_t;
```

Die interne Struktur kann erweitert werden und ist nicht Bestandteil der stabilen Userspace-ABI.

## 6. Objektidentität

```c
typedef uint64_t np_object_id_t;
typedef uint32_t np_object_type_id_t;
```

Jedes Objekt erhält eine innerhalb des laufenden Systems eindeutige Objektkennung.

Eine Objektkennung:

- dient Diagnose und interner Zuordnung,
- ist kein Handle,
- verleiht keine Rechte,
- darf nicht als Kerneladresse interpretiert werden,
- wird nicht erneut verwendet, solange Referenzen existieren.

## 7. Objekttypen

Jedes Objekt gehört genau einem registrierten Objekttyp an.

```c
typedef struct np_object_type {
    np_object_type_id_t type_id;
    const char* name;
    size_t object_size;

    np_object_type_operations_t operations;
    np_capability_rights_t valid_rights;

    uint32_t flags;
} np_object_type_t;
```

Der Objekttyp bestimmt:

- gültige Operationen,
- mögliche Capability-Rechte,
- Zerstörungsverhalten,
- Wartefähigkeit,
- Namespace-Fähigkeit,
- Diagnoseinformationen.

## 8. Kernobjekttypen

Mindestens folgende Typbereiche werden reserviert:

| Typbereich | Beispiele |
|---|---|
| Core | Namespace, Capability Domain, Handle Table |
| Process | Process, Thread, Job, Process Group |
| Memory | Address Space, Memory Object, Shared Memory |
| IPC | Channel, Message Queue, Event, Pipe, RPC Endpoint |
| VFS | File, Directory, Mount, File Handle |
| Device | Device, Driver, Interrupt Endpoint, DMA Object |
| Network | Socket, Route, Network Interface |
| Diagnostic | Log Stream, Trace Session, Crash Dump |
| Security | Identity, Audit Stream, Security Policy |

Neue Typen werden über eine zentrale Typregistry registriert.

## 9. Typoperationen

```c
typedef struct np_object_type_operations {
    np_status_t (*initialize)(np_kernel_object_t* object);
    void (*destroy)(np_kernel_object_t* object);

    np_status_t (*open)(np_kernel_object_t* object, uint32_t flags);
    void (*close)(np_kernel_object_t* object);

    np_status_t (*query)(
        np_kernel_object_t* object,
        uint32_t information_class,
        void* buffer,
        size_t buffer_size
    );

    bool (*is_signaled)(np_kernel_object_t* object);
} np_object_type_operations_t;
```

Nicht benötigte Operationen können `NULL` sein und werden mit `NP_ERR_NOT_SUPPORTED` beantwortet.

## 10. Typregistrierung

```c
np_status_t np_object_type_register(
    const np_object_type_descriptor_t* descriptor,
    np_object_type_t** object_type
);
```

Vor der Registrierung werden geprüft:

- eindeutige Typkennung,
- eindeutiger Typname,
- gültige Objektgröße,
- gültige Rechte,
- kompatible Operationen,
- Initialisierungsreihenfolge,
- Modul- und Lebenszeitabhängigkeiten.

Veröffentlichte Typkennungen dürfen nicht mit neuer Bedeutung wiederverwendet werden.

## 11. Typentfernung

Ein Objekttyp darf nur entfernt werden, wenn:

- keine lebenden Objekte dieses Typs existieren,
- keine offenen Handles existieren,
- keine abhängigen Typen aktiv sind,
- kein laufender Callback auf den Typ verweist.

Statische Kernobjekttypen werden während des normalen Betriebs nicht entfernt.

## 12. Objekterzeugung

```c
np_status_t np_object_create(
    np_object_type_t* type,
    const np_object_create_info_t* create_info,
    np_kernel_object_t** object
);
```

Die Erzeugung erfolgt in folgenden Schritten:

1. Typ und Parameter prüfen,
2. Speicher aus dem typisierten Object Cache allokieren,
3. Objekt-Header initialisieren,
4. Objektkennung vergeben,
5. typspezifische Initialisierung ausführen,
6. Sicherheitskontext anwenden,
7. optional Namespace-Eintrag erzeugen,
8. Objekt in den Kernel Object Graph aufnehmen,
9. Objekt an den Aufrufer zurückgeben.

Bei einem Fehler müssen alle Teilschritte zurückgerollt werden.

## 13. Anonyme Objekte

Anonyme Objekte besitzen keinen Namespace-Namen.

Sie sind erreichbar über:

- Handles,
- interne Kernelreferenzen,
- Capability-Beziehungen,
- Objektbeziehungen im Kernel Object Graph.

Beispiele sind Threads, private Events und temporäre Speicherobjekte.

Ein anonymes Objekt kann später nur dann benannt werden, wenn sein Typ und seine Sicherheitsrichtlinie dies zulassen.

## 14. Benannte Objekte

Benannte Objekte werden in einem Namespace registriert.

Beispiele sind:

- Systemdienste,
- IPC-Endpunkte,
- Geräte,
- Mounts,
- benannte Events,
- benannte Shared-Memory-Objekte.

Ein Name stellt keine Zugriffsberechtigung dar. Nach der Namensauflösung folgt immer eine Capability- oder Sicherheitsprüfung.

## 15. Objektnamen

NovaOS verwendet UTF-8 für Objektnamen.

Namen müssen:

- längenbegrenzt sein,
- gültig kodiert sein,
- kanonisch normalisiert werden,
- keine leeren Pfadsegmente enthalten,
- reservierte Zeichenregeln einhalten,
- innerhalb ihres Namespace-Kontexts eindeutig sein.

Groß-/Kleinschreibung ist standardmäßig relevant. Ein Namespace kann eine andere Richtlinie ausdrücklich definieren.

## 16. Namespaces

Ein Namespace ist selbst ein Kernelobjekt.

```c
typedef struct np_object_namespace {
    np_kernel_object_t object;

    np_namespace_node_t* root;
    np_object_namespace_t* parent;

    uint64_t generation;
    uint32_t flags;

    np_rwlock_t lock;
} np_object_namespace_t;
```

Namespaces können hierarchisch aufgebaut und Sicherheitsdomains zugeordnet werden.

## 17. Namespace-Pfade

Ein Objektpfad besteht aus einzelnen Namenssegmenten.

Beispiel:

```text
/system/services/device-manager
/session/42/events/shutdown
/device/pci/0000:00:1f.2
```

Pfadauflösung darf keine implizite Umgehung von Namespace- oder Capability-Grenzen ermöglichen.

Die maximal zulässige Pfadlänge und Segmentanzahl werden durch die ABI begrenzt.

## 18. Namespace-Sicht

Ein Prozess sieht nicht automatisch den globalen Objekt-Namespace.

Seine Sicht kann eingeschränkt sein durch:

- Prozess-Namespace,
- Sitzung,
- Sandbox,
- Container,
- Capability-Domain,
- Job Object,
- Sicherheitsprofil.

Dasselbe Objekt kann über unterschiedliche kontrollierte Namespace-Bindungen sichtbar sein.

## 19. Namespace-Operationen

```c
np_status_t np_namespace_insert(
    np_object_namespace_t* object_namespace,
    np_string_view_t path,
    np_kernel_object_t* object,
    uint32_t flags
);

np_status_t np_namespace_lookup(
    np_object_namespace_t* object_namespace,
    np_string_view_t path,
    np_capability_rights_t requested_rights,
    np_kernel_object_t** object
);

np_status_t np_namespace_remove(
    np_object_namespace_t* object_namespace,
    np_string_view_t path
);
```

Ein Lookup liefert immer eine neue geprüfte Referenz oder ein Handle.

## 20. Atomare Namensregistrierung

Das Prüfen auf Namensfreiheit und das Einfügen eines Objekts müssen atomar erfolgen.

Zwei parallele Erzeugungsversuche für denselben exklusiven Namen dürfen nicht beide erfolgreich sein.

Optional kann eine Operation zum atomaren Ersetzen eines bestehenden Eintrags bereitgestellt werden, wenn der Objekttyp und die Sicherheitsrichtlinie dies erlauben.

## 21. Objekt-Aliase

Ein Namespace kann mehrere Namen auf dasselbe Objekt verweisen lassen.

Jeder Alias:

- hält eine eigene Referenz,
- besitzt eigene Sichtbarkeitsregeln,
- kann getrennt entfernt werden,
- verändert nicht die Objektidentität.

Aliase dürfen keine Namespace-Grenzen ohne ausdrückliche Berechtigung überbrücken.

## 22. Symbolische Namespace-Verweise

Namespaces können kontrollierte symbolische Verweise unterstützen.

Die Auflösung muss:

- eine maximale Tiefe besitzen,
- Schleifen erkennen,
- Namespace-Grenzen beachten,
- Sicherheitsprüfungen nach der endgültigen Auflösung durchführen.

Symbolische Verweise erteilen selbst keine zusätzlichen Rechte.

## 23. Starke Referenzen

Eine starke Referenz hält ein Objekt am Leben.

```c
void np_object_retain(
    np_kernel_object_t* object
);

void np_object_release(
    np_kernel_object_t* object
);
```

Erreicht die Zahl starker Referenzen null, wird die Objektzerstörung eingeleitet.

Referenzzähler müssen gegen Über- und Unterlauf geschützt sein.

## 24. Schwache Referenzen

Eine schwache Referenz hält das Objekt nicht am Leben.

```c
typedef struct np_weak_object_reference {
    np_object_id_t object_id;
    uint64_t generation;
    np_weak_reference_control_t* control;
} np_weak_object_reference_t;
```

Das Aufwerten einer schwachen Referenz muss atomar prüfen, ob das Objekt noch lebt.

```c
np_status_t np_object_weak_lock(
    np_weak_object_reference_t* weak_reference,
    np_kernel_object_t** object
);
```

## 25. Handle-Referenzen

Jedes offene Handle hält eine starke Referenz auf das Zielobjekt, sofern der konkrete Handletyp nichts anderes definiert.

Der `handle_count` dient Diagnose und Lebenszyklussteuerung.

Die Zahl offener Handles und die Zahl starker Referenzen dürfen unterschiedlich sein, da auch interne Kernelreferenzen existieren.

## 26. Handles

```c
typedef uint64_t np_handle_t;
```

Ein Handle identifiziert einen Eintrag in der Handle-Tabelle eines Prozesses oder einer Kernel-Domain.

Ein Handlewert enthält mindestens logisch:

- Tabellenindex,
- Generationskennung,
- optionale Typ- oder Schutzbits.

Das konkrete Format ist ABI-definiert, enthält aber keinen dereferenzierbaren Kernelzeiger.

## 27. Handle-Eintrag

```c
typedef struct np_handle_entry {
    np_kernel_object_t* object;
    np_capability_rights_t rights;

    uint32_t flags;
    uint32_t generation;
} np_handle_entry_t;
```

Der Handle-Eintrag wird ausschließlich im Kernel gespeichert.

Userspace kann seine Rechte nicht durch Veränderung des numerischen Handlewerts erweitern.

## 28. Handle-Tabelle

Jeder Prozess besitzt eine eigene Handle-Tabelle.

```c
typedef struct np_handle_table {
    np_handle_entry_t* entries;
    size_t capacity;

    uint32_t next_generation;
    np_rwlock_t lock;
} np_handle_table_t;
```

Die Implementierung darf eine mehrstufige oder segmentierte Tabelle verwenden, um große Tabellen effizient zu verwalten.

## 29. Handle-Erzeugung

```c
np_status_t np_handle_create(
    np_handle_table_t* table,
    np_kernel_object_t* object,
    np_capability_rights_t rights,
    uint32_t flags,
    np_handle_t* handle
);
```

Die angeforderten Rechte müssen eine Teilmenge der Rechte sein, die der Erzeuger delegieren darf.

Ein Handle wird erst sichtbar, wenn sein Eintrag vollständig initialisiert wurde.

## 30. Handle-Auflösung

```c
np_status_t np_handle_resolve(
    np_handle_table_t* table,
    np_handle_t handle,
    np_object_type_id_t expected_type,
    np_capability_rights_t required_rights,
    np_kernel_object_t** object
);
```

Geprüft werden:

- Index,
- Generation,
- Belegungszustand,
- Objekttyp,
- erforderliche Rechte,
- Sicherheitsdomain,
- Objektlebenszyklus.

Bei Erfolg wird eine temporäre starke Referenz zurückgegeben.

## 31. Handle-Schließung

```c
np_status_t np_handle_close(
    np_handle_table_t* table,
    np_handle_t handle
);
```

Beim Schließen werden:

1. Handle-Eintrag atomar ungültig gemacht,
2. Generationskennung erhöht,
3. typspezifische Close-Operation ausgeführt,
4. Handle-Zähler reduziert,
5. starke Objektreferenz freigegeben.

Ein bereits geschlossenes oder veraltetes Handle wird abgelehnt.

## 32. Handle-Duplizierung

```c
np_status_t np_handle_duplicate(
    np_handle_t source,
    np_process_handle_t target_process,
    np_capability_rights_t requested_rights,
    uint32_t flags,
    np_handle_t* target_handle
);
```

Die Rechte des Zielhandles dürfen die delegierbaren Rechte des Quellhandles nicht überschreiten.

Die Übertragung in einen fremden Prozess erfordert eine passende Capability.

## 33. Handle-Vererbung

Handles werden standardmäßig nicht an Kindprozesse vererbt.

Vererbung ist nur möglich, wenn:

- das Handle als vererbbar markiert wurde,
- der Objekttyp Vererbung erlaubt,
- das Sicherheitsprofil dies zulässt,
- die Rechte nicht durch das Job Object begrenzt werden.

Administrative, Debug- und Kernelhandles sind standardmäßig nicht vererbbar.

## 34. Handle-Flags

```c
typedef enum np_handle_flags {
    NP_HANDLE_INHERITABLE    = 1U << 0,
    NP_HANDLE_TRANSFERABLE   = 1U << 1,
    NP_HANDLE_CLOSE_ON_EXEC  = 1U << 2,
    NP_HANDLE_AUDITED        = 1U << 3,
    NP_HANDLE_PROTECTED      = 1U << 4
} np_handle_flags_t;
```

Ein geschütztes Handle kann nur mit einer erweiterten Berechtigung geschlossen oder übertragen werden.

## 35. Capability-Rechte

Rechte sind objekttypabhängige Bitmasken.

Allgemeine Rechte sind:

```c
typedef enum np_object_rights {
    NP_RIGHT_QUERY       = 1ULL << 0,
    NP_RIGHT_MODIFY      = 1ULL << 1,
    NP_RIGHT_WAIT        = 1ULL << 2,
    NP_RIGHT_SIGNAL      = 1ULL << 3,
    NP_RIGHT_DUPLICATE   = 1ULL << 4,
    NP_RIGHT_TRANSFER    = 1ULL << 5,
    NP_RIGHT_DELETE      = 1ULL << 6,
    NP_RIGHT_ADMIN       = 1ULL << 7
} np_object_rights_t;
```

Objekttypen ergänzen spezialisierte Rechte.

## 36. Capability-Reduktion

Ein Handle darf mit reduzierten Rechten dupliziert werden.

```c
np_status_t np_handle_restrict(
    np_handle_t source,
    np_capability_rights_t reduced_rights,
    np_handle_t* restricted_handle
);
```

Ein einmal reduziertes Handle kann seine entfernten Rechte nicht selbst wiederherstellen.

## 37. Pseudohandles

NovaOS kann Pseudohandles für den aktuellen Kontext bereitstellen.

Beispiele:

```text
NP_CURRENT_PROCESS
NP_CURRENT_THREAD
NP_CURRENT_JOB
NP_CURRENT_NAMESPACE
```

Pseudohandles:

- sind keine normalen Handle-Tabelleneinträge,
- dürfen nicht direkt übertragen werden,
- werden beim Systemaufruf gegen den aktuellen Kontext aufgelöst,
- verleihen nur die ausdrücklich definierten Eigenrechte.

## 38. Wartbare Objekte

Bestimmte Objekttypen können einen signalisierten Zustand besitzen.

Beispiele sind:

- Threads,
- Prozesse,
- Events,
- Timer,
- Pipes,
- IPC-Endpunkte,
- asynchrone Operationen.

Der Object Manager stellt eine gemeinsame Wait-Schnittstelle bereit, während die konkrete Signalisierungslogik beim Objekttyp verbleibt.

## 39. Wait-API

```c
np_status_t np_object_wait(
    np_handle_t handle,
    uint64_t timeout_ns
);

np_status_t np_object_wait_many(
    const np_handle_t* handles,
    size_t handle_count,
    np_wait_mode_t mode,
    uint64_t timeout_ns,
    size_t* signaled_index
);
```

Vor dem Blockieren werden Handle, `WAIT`-Recht und Wartefähigkeit des Typs geprüft.

## 40. Objektzustandsereignisse

Objekte können strukturierte Zustandsereignisse veröffentlichen.

Beispiele sind:

```text
OBJECT_CREATED
OBJECT_NAMED
OBJECT_HANDLE_OPENED
OBJECT_HANDLE_CLOSED
OBJECT_SIGNALLED
OBJECT_UNLINKED
OBJECT_DESTROYING
OBJECT_DESTROYED
```

Nicht jedes Ereignis wird systemweit veröffentlicht. Sichtbarkeit und Detailtiefe hängen von Sicherheits- und Diagnoserichtlinien ab.

## 41. Kernel Object Graph

Der Kernel Object Graph bildet Beziehungen zwischen Kernelobjekten ab.

Mögliche Beziehungen sind:

- Prozess besitzt Thread,
- Prozess gehört zu Job,
- Treiber steuert Gerät,
- Mount referenziert Dateisystem,
- Handle-Tabelle referenziert Objekt,
- Namespace enthält Objekt,
- IPC-Kanal verbindet Endpunkte,
- DMA-Objekt gehört zu Gerät und Prozess.

Der Graph dient Architektur, Diagnose, Lebenszyklussteuerung und lokalen Abhängigkeitsanalysen.

## 42. Objektbeziehungen

```c
typedef enum np_object_relation_type {
    NP_RELATION_OWNS,
    NP_RELATION_CONTAINS,
    NP_RELATION_PARENT,
    NP_RELATION_CHILD,
    NP_RELATION_DEPENDS_ON,
    NP_RELATION_BOUND_TO,
    NP_RELATION_EXPOSES
} np_object_relation_type_t;
```

Eine Beziehung kann:

- stark oder schwach sein,
- Sicherheitsattribute besitzen,
- beim Objektende automatisch entfernt werden,
- Diagnoseinformationen tragen.

Zyklische starke Besitzbeziehungen müssen verhindert oder durch eine explizite Auflösungsstrategie behandelt werden.

## 43. Graph-Operationen

```c
np_status_t np_object_relation_add(
    np_kernel_object_t* source,
    np_kernel_object_t* target,
    np_object_relation_type_t type,
    uint32_t flags
);

np_status_t np_object_relation_remove(
    np_kernel_object_t* source,
    np_kernel_object_t* target,
    np_object_relation_type_t type
);
```

Beziehungsänderungen müssen atomar und capability-geprüft erfolgen.

## 44. Objektlebenszyklus

Ein Objekt durchläuft folgende Zustände:

```c
typedef enum np_object_state {
    NP_OBJECT_ALLOCATED,
    NP_OBJECT_INITIALIZING,
    NP_OBJECT_ACTIVE,
    NP_OBJECT_CLOSING,
    NP_OBJECT_DESTROYING,
    NP_OBJECT_DESTROYED
} np_object_state_t;
```

Nur vollständig initialisierte Objekte dürfen als `ACTIVE` veröffentlicht oder über Handles zugänglich gemacht werden.

## 45. Objektzerstörung

Die Zerstörung beginnt, wenn keine starken Referenzen mehr bestehen und keine besondere Lebenszyklusregel das Objekt hält.

Der Ablauf ist:

1. Objekt für neue Zugriffe sperren,
2. Namespace-Bindungen entfernen,
3. Waiter und Beobachter benachrichtigen,
4. Objektbeziehungen auflösen,
5. typspezifischen Destruktor ausführen,
6. schwache Referenzen ungültig machen,
7. Diagnosezustand aktualisieren,
isieren,
8. Objektspeicher an den Object Cache zurückgeben.

Der Destruktor darf das bereits zerstörende Objekt nicht erneut dauerhaft referenzieren.

## 46. Verzögerte Zerstörung

Objekte dürfen nicht freigegeben werden, während eine andere CPU noch eine gültige temporäre Referenz besitzt.

Je nach Implementierung können verwendet werden:

- atomare Referenzzählung,
- Deferred Reclamation,
- Epoch-basierte Freigabe,
- RCU-ähnliche Verfahren.

Die endgültige Speicherfreigabe erfolgt erst, wenn keine CPU mehr auf den alten Objektzustand zugreifen kann.

## 47. Zerstörungsabhängigkeiten

Die Zerstörung eines Objekts kann andere Aktionen auslösen.

Beispiele:

- Prozessende schließt Handles,
- Treiberende trennt Gerätebindungen,
- IPC-Endpunktende weckt Partner,
- Mountende invalidiert VFS-Beziehungen,
- Jobende beendet zugeordnete Prozesse.

Solche Aktionen müssen explizit definiert sein. Implizite rekursive Zerstörung ohne Tiefenbegrenzung ist unzulässig.

## 48. Objektabfragen

```c
np_status_t np_object_query(
    np_handle_t handle,
    uint32_t information_class,
    void* output_buffer,
    size_t output_size,
    size_t* required_size
);
```

Allgemeine Informationsklassen können enthalten:

- Objektkennung,
- Objekttyp,
- Name,
- Handle-Rechte,
- Zustand,
- Referenz- und Handleanzahl,
- Namespace,
- Beziehungen.

Sensible Informationen erfordern zusätzliche Diagnoseberechtigungen.

## 49. Objektänderungen

Generische Objektattribute dürfen nur verändert werden, wenn:

- der Objekttyp die Änderung unterstützt,
- der Aufrufer das erforderliche Recht besitzt,
- der Objektzustand die Änderung erlaubt,
- Namespace- und Sicherheitsregeln eingehalten werden.

Typspezifische Eigenschaften werden über die jeweilige Service-API verwaltet.

## 50. Synchronisation

Der Object Manager verwendet:

- atomare Referenzzähler,
- Handle-Tabellen-Sperren,
- Namespace-Lese-/Schreibsperren,
- objektlokale Sperren,
- verzögerte Freigabeverfahren,
- eindeutige Sperrreihenfolgen.

Sperren dürfen nicht über beliebige Userspace-, IPC- oder blockierende Typcallbacks gehalten werden.

## 51. Sperrreihenfolge

Wenn mehrere Objekte gleichzeitig gesperrt werden müssen, gilt eine deterministische Reihenfolge, beispielsweise nach:

1. Namespace-Ebene,
2. Objekttypklasse,
3. Objektkennung.

Alternativ müssen spezielle atomare Transaktionsoperationen verwendet werden.

Eine unkontrollierte Sperrung in Graph-Reihenfolge ist wegen möglicher Zyklen unzulässig.

## 52. Capability-Domains

Capability-Domains begrenzen die Menge der für einen Prozess oder Dienst erreichbaren Objekte.

Eine Domain kann:

- Handles aufnehmen,
- delegierbare Rechte begrenzen,
- Objektarten sperren,
- Namespace-Sichten zuordnen,
- Auditregeln definieren,
- Kind-Domains mit reduzierten Rechten erzeugen.

Eine Kind-Domain darf keine Rechte besitzen, die nicht von ihrer Quelle delegiert wurden.

## 53. Objekt- und Handle-Limits

Limits können festgelegt werden pro:

- Prozess,
- Job Object,
- Capability-Domain,
- Objekttyp,
- System.

Beispiele sind:

- maximale offene Handles,
- maximale benannte Objekte,
- maximale Wait-Objekte,
- maximale Namespace-Einträge,
- maximale Objektbeziehungen.

Ein Überschreiten liefert einen definierten Ressourcenfehler.

## 54. Audit

Sicherheitsrelevante Objektoperationen können Auditereignisse erzeugen.

Dazu gehören:

- Öffnen eines geschützten Objekts,
- Capability-Delegation,
- Handle-Übertragung zwischen Domains,
- Namespace-Änderungen,
- administrative Objektlöschung,
- wiederholte Zugriffsverletzungen.

Numerische Handlewerte oder sensible Objektdaten sollen nicht unnötig protokolliert werden.

## 55. Diagnose

Der Object Manager stellt autorisierten Diagnosediensten bereit:

- Anzahl lebender Objekte pro Typ,
- Objektzustände,
- Referenz- und Handleanzahlen,
- Namespace-Auslastung,
- Handle-Tabellengrößen,
- Objektbeziehungen,
- fehlgeschlagene Lookups,
- abgelehnte Capability-Prüfungen,
- verzögerte Zerstörungen.

Eine vollständige Graphansicht erfordert administrative Diagnoseberechtigungen.

## 56. Statistik

```c
typedef struct np_object_manager_statistics {
    uint64_t active_objects;
    uint64_t registered_types;

    uint64_t active_handles;
    uint64_t namespace_entries;
    uint64_t graph_relations;

    uint64_t created_objects;
    uint64_t destroyed_objects;
    uint64_t failed_lookups;
    uint64_t denied_operations;
} np_object_manager_statistics_t;
```

Statistiken sollen pro System, Objekttyp und Sicherheitsdomain verfügbar sein.

## 57. Fehlercodes

| Fehlercode | Bedeutung |
|---|---|
| `NP_OK` | Operation erfolgreich |
| `NP_ERR_INVALID_ARGUMENT` | ungültiger Parameter |
| `NP_ERR_INVALID_HANDLE` | Handle ist ungültig |
| `NP_ERR_STALE_HANDLE` | Handle-Generation ist veraltet |
| `NP_ERR_OBJECT_TYPE` | unerwarteter Objekttyp |
| `NP_ERR_OBJECT_CLOSING` | Objekt wird bereits geschlossen |
| `NP_ERR_OBJECT_NOT_FOUND` | Objekt oder Name nicht gefunden |
| `NP_ERR_NAME_IN_USE` | Name ist bereits belegt |
| `NP_ERR_INVALID_NAME` | Objektname ist ungültig |
| `NP_ERR_NAMESPACE_LIMIT` | Namespace-Limit erreicht |
| `NP_ERR_HANDLE_LIMIT` | Handle-Limit erreicht |
| `NP_ERR_ACCESS_DENIED` | erforderliche Capability fehlt |
| `NP_ERR_REFERENCE_OVERFLOW` | Referenzzähler würde überlaufen |
| `NP_ERR_BUSY` | Objekt oder Typ wird noch verwendet |
| `NP_ERR_NOT_SUPPORTED` | Operation wird nicht unterstützt |

## 58. Sicherheitsanforderungen

Der Object Manager muss sicherstellen, dass:

- Userspace keine Kernelobjektzeiger erhält,
- Objektkennungen keine Zugriffsrechte verleihen,
- Handles auf Generation, Typ und Rechte geprüft werden,
- veraltete Handles nicht auf neue Objekte verweisen,
- Rechte bei Duplizierung nur reduziert oder autorisiert delegiert werden,
- Namen keine Capability-Prüfungen ersetzen,
- Namespace-Grenzen nicht durch Aliase oder symbolische Verweise umgangen werden,
- administrative Handles nicht unbeabsichtigt vererbt werden,
- Objekte nicht während aktiver Referenzen freigegeben werden,
- sensible Objektinformationen nur autorisiert diagnostiziert werden.

## 59. Datenschutz

Objektnamen und Beziehungen können Rückschlüsse auf Benutzeraktivitäten oder Anwendungen ermöglichen.

Daher gelten:

- Diagnose bleibt standardmäßig lokal,
- unprivilegierte Prozesse sehen nur eigene oder freigegebene Objekte,
- globale Graphabfragen erfordern besondere Capabilities,
- Objektinhalte werden nicht über generische Diagnose-APIs ausgegeben,
- Aufbewahrung von Audit- und Diagnosedaten muss begrenzt werden können.

## 60. Fehlerbehandlung

Ungültige Handle- oder Namespace-Operationen liefern definierte Fehler und dürfen den Kernelzustand nicht beschädigen.

Folgende Situationen gelten als schwere Kernelkonsistenzfehler:

- Referenzzähler-Unterlauf,
- Freigabe eines noch referenzierten Objekts,
- Handle auf bereits wiederverwendeten Objektspeicher trotz gültiger Generation,
- beschädigte Handle-Tabelle,
- beschädigte Namespace-Struktur,
- doppelte Objektzerstörung,
- starke nicht auflösbare Besitzzyklen in kritischen Kernelobjekten.

## 61. Testanforderungen

Mindestens folgende Tests sind erforderlich:

1. Registrierung eines Objekttyps,
2. Ablehnung einer doppelten Typkennung,
3. Erzeugung eines anonymen Objekts,
4. Erzeugung eines benannten Objekts,
5. atomare Namensregistrierung,
6. Namespace-Lookup,
7. Ablehnung ungültiger Namen,
8. Namespace-Isolation zwischen Prozessen,
9. Objekt-Aliase,
10. Erkennung symbolischer Schleifen,
11. starke Referenzierung,
12. schwache Referenzierung,
13. Aufwertung einer gültigen schwachen Referenz,
14. Ablehnung einer abgelaufenen schwachen Referenz,
15. Handle-Erzeugung,
16. Handle-Auflösung mit korrektem Typ,
17. Ablehnung eines falschen Objekttyps,
18. Handle-Schließung,
19. Erkennung eines veralteten Handles,
20. Handle-Duplizierung mit reduzierten Rechten,
21. Ablehnung einer Rechteerhöhung,
22. Handle-Übertragung zwischen Prozessen,
23. Handle-Vererbung,
24. Schutz nicht vererbbarer Handles,
25. Warten auf ein signalisierbares Objekt,
26. Kernel-Object-Graph-Beziehung,
27. Erkennung problematischer Besitzzyklen,
28. Objektzerstörung nach letzter Referenz,
29. verzögerte Freigabe auf mehreren CPUs,
30. Typentfernung mit aktiven Objekten,
31. Prozessende mit offenen Handles,
32. Einhaltung von Handle- und Objektlimits,
33. parallele Lookups und Entfernungen,
34. Capability-Prüfung geschützter Objekte,
35. korrekte Audit- und Diagnosedaten.

## 62. Verbindliche Invarianten

1. Jedes aktive Kernelobjekt gehört genau einem registrierten Objekttyp an.
2. Jede aktive Objektkennung ist innerhalb des Systems eindeutig.
3. Objektkennungen und Namen verleihen keine Zugriffsrechte.
4. Userspace greift niemals über direkte Kernelzeiger auf Objekte zu.
5. Jedes gültige Handle verweist auf genau ein Objekt und eine definierte Rechtemenge.
6. Veraltete Handle-Generationen werden abgelehnt.
7. Handle-Rechte können ohne neue Autorisierung nur reduziert werden.
8. Ein Objekt wird erst nach Wegfall aller starken Referenzen zerstört.
9. Schwache Referenzen halten ein Objekt nicht am Leben.
10. Ein Objekt wird höchstens einmal zerstört.
11. Namespace-Einträge halten eine definierte Objektreferenz.
12. Objekt- und Handle-Zustandsänderungen erfolgen unter geeigneter Synchronisation.

## 63. Referenzablauf einer Handle-Auflösung

```text
Systemaufruf erhält Handle
  -> aktuelle Handle-Tabelle bestimmen
  -> Index und Generation dekodieren
  -> Handle-Eintrag unter Synchronisation suchen
  -> Belegungszustand prüfen
  -> Objekttyp prüfen
  -> Capability-Rechte prüfen
  -> Sicherheitsdomain prüfen
  -> starke temporäre Referenz übernehmen
  -> Tabellensperre freigeben
  -> Objektoperation ausführen
  -> temporäre Referenz freigeben
```

## 64. Referenzablauf einer Objektzerstörung

```text
Letzte starke Referenz wird freigegeben
  -> Objekt atomar auf CLOSING setzen
  -> neue Handles und Referenzen verhindern
  -> Namespace-Einträge entfernen
  -> Waiter und Beobachter benachrichtigen
  -> Objektbeziehungen auflösen
  -> typspezifischen Destruktor ausführen
  -> schwache Referenzen ungültig machen
  -> verzögerte CPU-Referenzen abwarten
  -> Objekt auf DESTROYED setzen
  -> Speicher an Object Cache zurückgeben
```

## 65. Zusammenfassung

Der NovaOS Object Manager stellt ein gemeinsames, capability-geschütztes Modell für Kernel- und Userspace-Ressourcen bereit.

Kernelobjekte besitzen stabile Typen, eindeutige Identitäten und klar definierte Lebenszyklen. Userspace greift ausschließlich über generationsgeschützte Handles mit begrenzten Rechten auf sie zu.

Namespaces ermöglichen kontrollierte Namensauflösung, während der Kernel Object Graph Besitz-, Abhängigkeits- und Bindungsbeziehungen sichtbar macht. Referenzzählung und verzögerte Freigabe verhindern, dass Objekte während paralleler Zugriffe zerstört werden.