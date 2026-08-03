# NPSPEC-KERNEL-0018 – Driver Framework

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-KERNEL-0018 |
| Titel | Driver Framework |
| Status | Spezifikation |
| Version | 1.0 |
| Datum | 2026-08-03 |
| Bereich | Kernel / Treiberarchitektur |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | NPSPEC-KERNEL-0009, NPSPEC-KERNEL-0012, NPSPEC-KERNEL-0013, NPSPEC-KERNEL-0015, NPSPEC-KERNEL-0016, NPSPEC-KERNEL-0017 |
| Zugehörige ADRs | ADR-DEV-0001, ADR-DEV-0002, ADR-DEV-0003, ADR-DEV-0004, ADR-DEV-0005, ADR-DEV-0009, ADR-DEV-0010 |

---

## 1. Zweck

Diese Spezifikation definiert das Driver Framework von NovaOS.

Das Framework stellt einheitliche Schnittstellen für:

- Kernel-Treiber,
- Userspace-Treiber,
- Bustreiber,
- Funktionstreiber,
- Filtertreiber,
- virtuelle Treiber,
- Geräteinitialisierung,
- I/O-Anfragen,
- Interrupts und DMA,
- Power Management,
- Plug-and-Play,
- Fehlerbehandlung und Treiber-Recovery

bereit.

## 2. Ziele

Das Driver Framework muss:

- Userspace-Treiber bevorzugen,
- Kernel-Treiber auf technisch notwendige Fälle begrenzen,
- Treiber durch Capabilities isolieren,
- ein stabiles versioniertes Treiber-ABI bereitstellen,
- wiederverwendbare Basiskomponenten anbieten,
- Plug-and-Play und Power Management integrieren,
- asynchrone I/O-Verarbeitung unterstützen,
- Treiberabstürze möglichst vom Kernel isolieren,
- Diagnose und kontrollierten Neustart ermöglichen.

## 3. Grundprinzipien

Für NovaOS-Treiber gelten:

1. Ein Treiber erhält nur die Ressourcen seines Geräts.
2. Userspace-Treiber sind die bevorzugte Ausführungsform.
3. Kernel-Treiber benötigen eine technische oder sicherheitsbezogene Begründung.
4. Treiber greifen nicht direkt auf fremde Kernel- oder Geräteressourcen zu.
5. Öffentliche Treiberschnittstellen sind versioniert.
6. Fehler werden möglichst auf Treiber und Gerät begrenzt.
7. Lange Operationen erfolgen asynchron oder in planbaren Threads.

## 4. Treiberarten

```c
typedef enum np_driver_type {
    NP_DRIVER_KERNEL,
    NP_DRIVER_USERSPACE,
    NP_DRIVER_BUS,
    NP_DRIVER_FUNCTION,
    NP_DRIVER_FILTER,
    NP_DRIVER_VIRTUAL,
    NP_DRIVER_FILESYSTEM,
    NP_DRIVER_PLATFORM
} np_driver_type_t;
```

| Typ | Aufgabe |
|---|---|
| `KERNEL` | privilegierter Kernel-Treiber |
| `USERSPACE` | isolierter Treiberprozess |
| `BUS` | Enumeration und Verwaltung eines Busses |
| `FUNCTION` | Steuerung einer konkreten Gerätefunktion |
| `FILTER` | kontrollierte Erweiterung eines I/O-Pfads |
| `VIRTUAL` | software- oder hypervisorbasiertes Gerät |
| `FILESYSTEM` | Dateisystemintegration |
| `PLATFORM` | architektur- oder firmwareabhängige Plattformfunktion |

Die Typen können kombiniert werden, soweit die Treiber-ABI dies ausdrücklich erlaubt.

## 5. Ausführungsmodelle

### 5.1 Kernelmodus

Ein Kernel-Treiber läuft im privilegierten Kerneladressraum.

Er ist vorgesehen für:

- Interrupt-Controller,
- grundlegende Timer,
- frühe Bootgeräte,
- Speicher- und IOMMU-Funktionen,
- besonders latenzkritische Komponenten,
- nicht ausreichend isolierbare Plattformfunktionen.

### 5.2 Userspace

Ein Userspace-Treiber läuft in einem eigenen Prozess und kommuniziert über IPC mit Kernel und Clients.

Er ist bevorzugt für:

- USB-Geräte,
- Audio,
- Eingabegeräte,
- Sensoren,
- Drucker,
- viele Netzwerkgeräte,
- nicht bootkritische Speichergeräte,
- virtuelle Geräte.

## 6. Treiberidentität

```c
typedef uint64_t np_driver_id_t;
typedef uint64_t np_driver_instance_id_t;
```

Eine Driver ID identifiziert ein Treiberpaket oder eine Treiberdefinition.

Eine Instance ID identifiziert eine konkrete laufende Treiberinstanz.

Beide Kennungen sind keine Handles und verleihen keine Zugriffsrechte.

## 7. Treiber-Manifest

Jeder Treiber besitzt ein signiertes oder anderweitig vertrauenswürdig verifiziertes Manifest.

```c
typedef struct np_driver_manifest {
    np_abi_header_t header;

    np_uuid_t driver_uuid;
    np_string_view_t name;
    np_driver_version_t version;
    np_driver_type_t type;

    np_driver_abi_range_t abi_range;
    np_driver_match_table_t match_table;
    np_driver_resource_requirements_t resources;

    uint64_t feature_flags;
    uint64_t security_flags;
} np_driver_manifest_t;
```

Das Manifest wird vor dem Laden vollständig validiert.

## 8. Manifest-Inhalte

Das Manifest beschreibt mindestens:

- Treibername und Hersteller,
- Treiberversion,
- unterstützte Architekturen,
- benötigte NovaOS-Treiber-ABI,
- unterstützte Gerätekennungen,
- Ausführungsmodell,
- erforderliche Capabilities,
- Ressourcenanforderungen,
- Power- und Hotplug-Unterstützung,
- Signatur- und Vertrauensinformationen,
- Update- und Recovery-Fähigkeiten.

Ein Treiber darf zur Laufzeit keine nicht deklarierten privilegierten Ressourcen anfordern.

## 9. Treibersignatur

Vor dem Laden wird geprüft:

- Signaturintegrität,
- Zertifikats- oder Trust-Backend-Status,
- Paket- und Image-Hash,
- Sperrlisten,
- zulässiger Herausgeber,
- Kompatibilität mit dem aktiven Sicherheitsprofil.

NovaOS unterstützt Trust-Backends mit und ohne TPM.

Unsichere oder unsignierte Treiber dürfen nur in ausdrücklich aktivierten Entwicklungs- oder Recovery-Profilen geladen werden.

## 10. Treiber-ABI

```c
typedef struct np_driver_abi_version {
    uint16_t major;
    uint16_t minor;
} np_driver_abi_version_t;
```

Eine inkompatible Major-Version wird abgelehnt.

Eine neuere kompatible Minor-Version darf akzeptiert werden, wenn alle verwendeten Strukturen ausreichend groß sind.

Veröffentlichte ABI-Felder dürfen innerhalb einer Major-Version ihre Bedeutung nicht ändern.

## 11. Kernel-Treibereinstieg

Ein Kernel-Treiber stellt einen definierten Einstiegspunkt bereit:

```c
np_status_t np_driver_entry(
    const np_driver_environment_t* environment,
    np_driver_registration_t* registration
);
```

Der Einstiegspunkt:

- validiert die bereitgestellte ABI-Version,
- registriert Treiberoperationen,
- führt keine lang laufende Geräteinitialisierung aus,
- aktiviert noch keine unvorbereiteten Interruptquellen,
- verändert keine globalen Kernelstrukturen direkt.

## 12. Treiberregistrierung

```c
typedef struct np_driver_registration {
    np_abi_header_t header;

    const np_driver_operations_t* operations;
    const np_driver_match_entry_t* match_entries;
    size_t match_entry_count;

    uint64_t driver_features;
} np_driver_registration_t;
```

Der Kernel kopiert oder referenziert Registrierungsdaten nur entsprechend der definierten Lebenszeitregeln.

Nicht versionierte interne Strukturzeiger dürfen nicht dauerhaft gespeichert werden.

## 13. Treiberoperationen

```c
typedef struct np_driver_operations {
    np_status_t (*probe)(np_device_handle_t device);
    np_status_t (*bind)(np_device_handle_t device);
    np_status_t (*start)(np_device_handle_t device);

    np_status_t (*stop)(
        np_device_handle_t device,
        uint32_t reason
    );

    np_status_t (*reset)(
        np_device_handle_t device,
        uint32_t reset_type
    );

    np_status_t (*remove)(np_device_handle_t device);
} np_driver_operations_t;
```

Optionale Operationen können fehlen und liefern dann `NP_ERR_NOT_SUPPORTED`.

## 14. Matching

Treiber deklarieren unterstützte Geräte über Match-Einträge.

```c
typedef struct np_driver_match_entry {
    uint32_t bus_type;
    uint32_t match_flags;

    uint64_t vendor_id;
    uint64_t device_id;
    uint64_t class_id;
    uint64_t revision;

    np_string_view_t compatible;
    uint32_t priority;
} np_driver_match_entry_t;
```

Ein Match-Eintrag muss eindeutig angeben, welche Felder relevant sind.

## 15. Probe-Phase

`probe()` prüft, ob der Treiber ein Gerät tatsächlich unterstützt.

Die Operation darf:

- sichere Geräteinformationen lesen,
- unveränderliche Identitätsregister prüfen,
- minimale kontrollierte Hardwarezugriffe ausführen.

Sie darf nicht:

- das Gerät dauerhaft aktivieren,
- globale Ressourcen exklusiv belegen,
- unkontrollierte DMA-Operationen starten,
- Interrupts vorzeitig freigeben.

## 16. Bind-Phase

Während `bind()` werden Treiber und Gerät logisch verbunden.

Dazu gehören:

- Instanzzustand erzeugen,
- Ressourcenhandles übernehmen,
- Capability-Domain anwenden,
- Kindobjekte vorbereiten,
- I/O-Endpunkte registrieren,
- Power- und PnP-Callbacks einrichten.

Fehler müssen vollständig zurückgerollt werden.

## 17. Start-Phase

`start()` aktiviert das Gerät für den normalen Betrieb.

Vor dem erfolgreichen Abschluss müssen:

- MMIO- oder Portzugriffe eingerichtet,
- DMA-Domain vorbereitet,
- Interrupt Handler oder Endpunkte registriert,
- Gerät in einen bekannten Zustand versetzt,
- I/O-Queues initialisiert sein.

Interrupts werden erst freigegeben, wenn der Treiber sie sicher verarbeiten kann.

## 18. Stop-Phase

`stop()` beendet die aktive Gerätenutzung.

Der Treiber muss:

1. neue I/O-Anfragen ablehnen,
2. laufende Anfragen abschließen oder abbrechen,
3. DMA stoppen,
4. Interruptquellen maskieren,
5. Hardware in einen sicheren Zustand versetzen,
6. temporäre Ressourcen freigeben.

Ein Stop muss idempotent oder zumindest sicher wiederholbar sein.

## 19. Remove-Phase

`remove()` löst die endgültige Bindung zwischen Treiber und Gerät.

Nach erfolgreichem Remove darf der Treiber:

- keine Geräteregister mehr lesen,
- keine DMA-Operationen starten,
- keine Geräteinterrupts erwarten,
- keine neuen Clientoperationen annehmen.

Alle Ressourcen und Objektbeziehungen der Instanz müssen freigegeben sein.

## 20. Treiberinstanz

```c
typedef struct np_driver_instance {
    np_kernel_object_t object;

    np_driver_id_t driver_id;
    np_driver_instance_id_t instance_id;

    np_device_t* device;
    np_driver_state_t state;

    np_capability_domain_t* capability_domain;
    np_resource_set_t resources;

    void* private_context;
} np_driver_instance_t;
```

Jede Bindung zwischen Treiber und Gerät erzeugt eine eigene Instanz.

## 21. Treiberzustände

```c
typedef enum np_driver_state {
    NP_DRIVER_LOADED,
    NP_DRIVER_PROBING,
    NP_DRIVER_BOUND,
    NP_DRIVER_STARTING,
    NP_DRIVER_RUNNING,
    NP_DRIVER_SUSPENDING,
    NP_DRIVER_SUSPENDED,
    NP_DRIVER_STOPPING,
    NP_DRIVER_FAILED,
    NP_DRIVER_UNLOADING,
    NP_DRIVER_UNLOADED
} np_driver_state_t;
```

Zustandsänderungen erfolgen ausschließlich über das Driver Framework.

## 22. Userspace-Treiberprozess

Ein Userspace-Treiber wird durch einen vertrauenswürdigen Driver Host gestartet.

Der Prozess erhält:

- Treiberpaket und Manifest,
- Gerätehandle,
- eingeschränkte MMIO-Objekte,
- Interrupt-Endpunkte,
- DMA-Domain,
- IPC-Kanal zum Device Manager,
- Diagnose- und Logging-Endpunkt,
- definierte Ressourcenlimits.

Der Prozess erhält keine allgemeine physische Speicher- oder Geräteadministrator-Capability.

## 23. Driver Host

Der Driver Host stellt die standardisierte Laufzeitumgebung für Userspace-Treiber bereit.

Er übernimmt:

- ABI-Aushandlung,
- IPC-Dispatch,
- Lifecycle-Callbacks,
- Exception- und Crash-Meldung,
- Ressourcenüberwachung,
- Update- und Neustartkoordination.

Ein Driver Host kann abhängig vom Sicherheitsprofil einen oder mehrere Treiber ausführen. Stärkere Isolation verwendet einen Prozess pro Treiberinstanz.

## 24. Kernel-Proxy

Für Userspace-Treiber stellt ein kleiner Kernel-Proxy notwendige privilegierte Funktionen bereit.

Der Proxy darf nur:

- zugewiesene Interrupts zustellen,
- kontrollierte MMIO-Objekte bereitstellen,
- DMA-Mappings verwalten,
- I/O-Anfragen weiterleiten,
- Zustands- und Power-Ereignisse übermitteln.

Gerätespezifische komplexe Logik soll im Userspace verbleiben.

## 25. Treiber-Capabilities

Ein Treiber kann abhängig vom Manifest erhalten:

| Capability | Bedeutung |
|---|---|
| `CAP_DRIVER_DEVICE` | zugewiesenes Gerät bedienen |
| `CAP_DRIVER_MMIO` | zugewiesene MMIO-Bereiche abbilden |
| `CAP_DRIVER_IO_PORT` | zugewiesene I/O-Ports verwenden |
| `CAP_DRIVER_INTERRUPT` | zugewiesene Interruptquelle verarbeiten |
| `CAP_DRIVER_DMA` | DMA-Objekte für das Gerät erzeugen |
| `CAP_DRIVER_CHILD_DEVICE` | Kindgeräte registrieren |
| `CAP_DRIVER_POWER` | Geräte-Power-State verwalten |
| `CAP_DRIVER_QUERY` | Treiber- und Geräteinformationen lesen |
| `CAP_DRIVER_ADMIN` | globale Treiberverwaltung |

Capabilities werden auf konkrete Geräte und Ressourcen begrenzt.

## 26. MMIO-Zugriff

MMIO wird über ein kontrolliertes Memory-Object bereitgestellt.

Der Treiber darf ausschließlich den zugewiesenen Bereich abbilden.

Die Abbildung muss:

- nicht ausführbar sein,
- den korrekten Cachemodus verwenden,
- beim Stop oder Remove widerrufbar sein,
- auf Plattform- und Gerätegrenzen geprüft werden.

Userspace-Treiber erhalten keine ungeprüften physischen Adressen.

## 27. I/O-Port-Zugriff

Auf Architekturen mit I/O-Port-Raum werden Portbereiche explizit zugewiesen.

Direkte Userspace-Portinstruktionen sind nur erlaubt, wenn:

- die Architektur sichere Begrenzung unterstützt,
- das Sicherheitsprofil es erlaubt,
- der Bereich exakt auf das Gerät beschränkt ist.

Bevorzugt erfolgt der Zugriff über einen kontrollierten Kernel-Service.

## 28. Interrupt-Verarbeitung

Kernel-Treiber können direkte oder Threaded Interrupt Handler registrieren.

Userspace-Treiber erhalten einen Interrupt-Endpunkt.

Der direkte Kernelpfad:

1. bestätigt minimale Controllerinformationen,
2. maskiert die Quelle bei Bedarf,
3. signalisiert den Treiberendpunkt,
4. wartet auf eine gültige Bestätigung,
5. gibt die Quelle wieder frei.

Nicht bestätigte Interrupts unterliegen einem Timeout und einer Recovery-Richtlinie.

## 29. DMA

DMA wird ausschließlich über das DMA Framework ausgeführt.

Ein Treiber beschreibt:

- Transfer-Richtung,
- Puffer,
- Länge,
- Segmentanforderungen,
- Gerätegrenzen,
- Synchronisationsmodus.

Der Kernel oder IOMMU-Dienst erzeugt daraus ein gerätespezifisches Mapping.

Ein Treiber darf keine beliebigen physischen Seiten als DMA-Ziel programmieren.

## 30. I/O-Anfragemodell

```c
typedef struct np_io_request {
    np_kernel_object_t object;

    uint64_t request_id;
    uint32_t operation;
    uint32_t flags;

    np_handle_t device_session;
    np_io_buffer_list_t buffers;

    np_time_ns_t deadline;
    np_io_status_t status;
} np_io_request_t;
```

I/O-Anfragen sind asynchron abschließbare Kernelobjekte.

## 31. I/O-Operationen

Generische Operationen umfassen:

```c
typedef enum np_io_operation {
    NP_IO_READ,
    NP_IO_WRITE,
    NP_IO_CONTROL,
    NP_IO_FLUSH,
    NP_IO_CANCEL,
    NP_IO_QUERY,
    NP_IO_RESET
} np_io_operation_t;
```

Gerätespezifische Operationen verwenden versionierte Control-Codes und Strukturen.

## 32. I/O-Übermittlung

```c
np_status_t np_io_submit(
    np_handle_t device_session,
    const np_io_submit_args_t* arguments,
    np_handle_t* request
);
```

Die Übermittlung umfasst:

1. Sitzung und Rechte prüfen,
2. Puffer validieren,
3. Ressourcenlimits prüfen,
4. Anfrageobjekt erzeugen,
5. Treiberqueue auswählen,
6. Anfrage an Treiber zustellen,
7. Completion-Handle zurückgeben.

## 33. I/O-Abschluss

```c
np_status_t np_io_complete(
    np_io_request_handle_t request,
    const np_io_completion_t* completion
);
```

Ein Abschluss enthält:

- Statuscode,
- übertragene Datenmenge,
- gerätespezifische Zusatzinformationen,
- optionalen Fehlerkontext.

Eine Anfrage darf genau einmal endgültig abgeschlossen werden.

## 34. I/O-Abbruch

```c
np_status_t np_io_cancel(
    np_io_request_handle_t request
);
```

Der Abbruch kann:

- vor der Treiberzustellung sofort erfolgen,
- während der Queueverarbeitung markiert werden,
- während eines Hardwaretransfers kooperativ behandelt werden.

Ein erfolgreicher Abbruch darf nicht gleichzeitig als regulärer Erfolg abgeschlossen werden.

## 35. Pufferverwaltung

I/O-Puffer können übertragen werden als:

- sichere Kernelkopie,
- Shared-Memory-Objekt,
- gepinnte Userspace-Seiten,
- DMA-Buffer-Objekt,
- Scatter-Gather-Liste.

Der Treiber darf Puffer nur während der ausdrücklich definierten Lebenszeit verwenden.

Nach Abschluss oder Abbruch werden Mappings und Pinning kontrolliert aufgehoben.

## 36. I/O-Queues

Treiber können eine oder mehrere I/O-Queues besitzen.

```c
typedef enum np_io_queue_policy {
    NP_IO_QUEUE_SERIAL,
    NP_IO_QUEUE_PARALLEL,
    NP_IO_QUEUE_MANUAL,
    NP_IO_QUEUE_PRIORITY
} np_io_queue_policy_t;
```

Mehrere Hardwarequeues können CPU-, NUMA- oder Gerätequeue-lokal zugeordnet werden.

Queuegrößen müssen begrenzt sein.

## 37. Backpressure

Ist eine Treiberqueue ausgelastet, kann das Framework:

- die Anfrage blockieren,
- `NP_ERR_WOULD_BLOCK` liefern,
- asynchron auf Queuekapazität warten,
- Anfragen nach Richtlinie priorisieren.

Unbegrenztes Puffern im Kernel oder Driver Host ist unzulässig.

## 38. Plug-and-Play-Callbacks

```c
typedef struct np_driver_pnp_operations {
    np_status_t (*prepare_start)(np_device_handle_t device);
    np_status_t (*start)(np_device_handle_t device);
    np_status_t (*query_remove)(np_device_handle_t device);
    np_status_t (*cancel_remove)(np_device_handle_t device);
    np_status_t (*remove)(np_device_handle_t device);
    void (*surprise_remove)(np_device_handle_t device);
} np_driver_pnp_operations_t;
```

`surprise_remove()` darf nicht voraussetzen, dass Gerätezugriffe noch funktionieren.

## 39. Power-Management-Callbacks

```c
typedef struct np_driver_power_operations {
    np_status_t (*prepare_suspend)(
        np_device_handle_t device,
        np_device_power_state_t target_state
    );

    np_status_t (*suspend)(
        np_device_handle_t device,
        np_device_power_state_t target_state
    );

    np_status_t (*resume)(
        np_device_handle_t device,
        np_device_power_state_t previous_state
    );
} np_driver_power_operations_t;
```

Power-Callbacks müssen Timeouts besitzen und dürfen Suspend nicht unbegrenzt blockieren.

## 40. Wake-Unterstützung

Ein Treiber kann Wake-Fähigkeiten deklarieren.

Er muss dabei angeben:

- unterstützte Schlafzustände,
- Wake-Interruptquelle,
- erforderlichen Gerätezustand,
- erwarteten Energieverbrauch,
- mögliche Wake-Gründe.

Die Aktivierung erfolgt ausschließlich nach Power-Richtlinie und Capability-Prüfung.

## 41. Bustreiber

Bustreiber dürfen:

- Kindgeräte enumerieren,
- Busressourcen verwalten,
- Gerätekonfiguration lesen und schreiben,
- Bus-Reset ausführen,
- Hotplug-Ereignisse melden,
- busspezifische Power-Übergänge koordinieren.

Ein Bustreiber erhält keine automatische Kontrolle über Geräte außerhalb seines Bus-Subtrees.

## 42. Filtertreiber

Filtertreiber können einen I/O-Pfad kontrolliert erweitern.

Zulässige Zwecke sind beispielsweise:

- Verschlüsselung,
- Integritätsprüfung,
- Protokollübersetzung,
- lokale Diagnose,
- Policy Enforcement.

Filtertreiber müssen ausdrücklich durch Richtlinie aktiviert werden.

Unkontrollierte oder versteckte globale I/O-Hooks sind unzulässig.

## 43. Filterreihenfolge

Filter werden in einer definierten Reihenfolge angeordnet.

```text
Client
  -> Upper Filter
  -> Function Driver
  -> Lower Filter
  -> Bus Driver
  -> Hardware
```

Die Reihenfolge muss deterministisch und diagnostisch sichtbar sein.

Zyklische Filterabhängigkeiten werden abgelehnt.

## 44. Virtuelle Geräte

Ein virtueller Treiber kann Geräteobjekte ohne direkte physische Hardware erzeugen.

Beispiele sind:

- RAM-Disk,
- virtuelles Netzwerkgerät,
- Loopback-Gerät,
- Software-Audiogerät,
- Hypervisorgerät.

Virtuelle Geräte unterliegen denselben Objekt-, Capability-, Lifecycle- und Ressourcenregeln wie physische Geräte.

## 45. Treiberabhängigkeiten

Treiber können Abhängigkeiten auf andere Treiber oder Dienste deklarieren.

Beispiele:

- USB-Funktionstreiber benötigt USB Core,
- Dateisystemtreiber benötigt Blockservice,
- virtueller Treiber benötigt Backenddienst,
- Netzwerkfilter benötigt Socket Layer.

Abhängigkeiten müssen im Manifest stehen und dürfen keine unbegrenzten Zyklen bilden.

## 46. Parallelität

Das Framework muss parallele Verarbeitung unterstützen für:

- verschiedene Geräte,
- mehrere Hardwarequeues,
- mehrere Clients,
- Interrupt und I/O-Completion,
- Plug-and-Play und Power-Ereignisse.

Der Treiber muss seine Parallelitätsanforderungen deklarieren.

Das Framework kann serielle Dispatch-Queues bereitstellen, damit einfache Treiber nicht jede Synchronisation selbst implementieren müssen.

## 47. Synchronisation

Treiber verwenden die Synchronisationsprimitive aus NPSPEC-KERNEL-0016.

Regeln:

- direkte Interrupt Handler blockieren nicht,
- I/O-Callbacks dürfen nur entsprechend ihrem Kontext warten,
- Gerätezustände werden gegen PnP- und Power-Übergänge geschützt,
- MMIO-Zugriffe verwenden erforderliche Speicherbarrieren,
- Request Completion und Cancellation werden atomar aufgelöst.

## 48. Zeitlimits

Folgende Vorgänge benötigen definierte Timeouts:

- Treiberstart,
- Gerätestopp,
- Power-Übergang,
- Userspace-Interruptbestätigung,
- I/O-Abbruch,
- Geräte-Reset,
- Treiber-Shutdown.

Ein Timeout führt abhängig von der Kritikalität zu Reset, Treiberneustart, Geräteisolierung oder Recovery.

## 49. Treiberabsturz

Beim Absturz eines Userspace-Treibers muss das Framework:

1. Treiberprozess als fehlerhaft markieren,
2. Interruptquellen maskieren,
3. DMA-Mappings widerrufen,
4. MMIO-Zugriffe entziehen,
5. I/O-Anfragen mit Fehler abschließen,
6. Clients und Device Manager benachrichtigen,
7. Geräte-Reset prüfen,
8. Treiber gemäß Recovery-Richtlinie neu starten.

Der Kernelzustand darf nicht vom Treiberprozess abhängig bleiben.

## 50. Kernel-Treiberfehler

Ein Fehler in einem Kernel-Treiber ist sicherheitskritischer.

Eine lokale Wiederherstellung ist nur zulässig, wenn:

- keine Kernelstrukturen beschädigt wurden,
- keine nicht freigegebenen Spinlocks bestehen,
- DMA und Interrupts sicher gestoppt werden können,
- ein definierter Recovery-Pfad existiert.

Andernfalls wird die Kernel-Panic-Policy angewendet.

## 51. Recovery-Richtlinie

```c
typedef struct np_driver_recovery_policy {
    uint32_t maximum_restarts;
    np_time_ns_t restart_window;
    uint32_t reset_policy;
    uint32_t fallback_policy;
} np_driver_recovery_policy_t;
```

Mögliche Fallbacks sind:

- ältere Treiberversion,
- generischer Treiber,
- Geräteisolierung,
- Safe Mode,
- Recovery-Umgebung.

Neustartschleifen müssen verhindert werden.

## 52. Live-Update

Ein Userspace-Treiber kann live aktualisiert werden, wenn:

- die neue Version verifiziert ist,
- das Gerät vorübergehend gestoppt werden kann,
- aktive I/O-Anfragen behandelt werden,
- Zustand exportiert oder neu aufgebaut werden kann,
- Rollback verfügbar ist.

Kernel-Treiberupdates benötigen strengere Prüfungen und können einen Neustart erfordern.

## 53. Treiberentladung

Ein Treiber darf nur entladen werden, wenn:

- keine aktiven Instanzen verbleiben,
- keine I/O-Anfragen laufen,
- keine Interrupt Handler aktiv sind,
- keine DMA-Mappings bestehen,
- keine Callbacks mehr ausgeführt werden,
- keine Kernelobjekte den Treibercode referenzieren.

Der Code darf erst nach Abschluss einer geeigneten Grace Period freigegeben werden.

## 54. Logging

Treiber verwenden die zentrale Kernel- oder Driver-Host-Logging-Schnittstelle.

Logs enthalten mindestens:

- Treiber- und Instanzkennung,
- Gerätekennung,
- Schweregrad,
- Ereigniscode,
- Zeitstempel.

Treiber dürfen sensible Nutzdaten, Schlüssel oder vollständige DMA-Puffer nicht standardmäßig protokollieren.

## 55. Tracing

Optionales Treiber-Tracing kann erfassen:

- Lifecycle-Callbacks,
- I/O-Anfragen und Abschlüsse,
- Interruptzustellungen,
- DMA-Mappings,
- Power-Übergänge,
- Reset- und Recovery-Ereignisse.

Tracing muss filterbar, lokal und capability-geschützt sein.

## 56. Statistik

```c
typedef struct np_driver_statistics {
    uint64_t loaded_drivers;
    uint64_t active_instances;
    uint64_t userspace_instances;
    uint64_t kernel_instances;

    uint64_t submitted_requests;
    uint64_t completed_requests;
    uint64_t failed_requests;
    uint64_t cancelled_requests;

    uint64_t driver_crashes;
    uint64_t driver_restarts;
    uint64_t device_resets;
} np_driver_statistics_t;
```

Statistiken sollen pro Treiber, Gerät und System abrufbar sein.

## 57. Fehlercodes

| Fehlercode | Bedeutung |
|---|---|
| `NP_OK` | Operation erfolgreich |
| `NP_ERR_INVALID_ARGUMENT` | ungültiger Parameter |
| `NP_ERR_ABI_INCOMPATIBLE` | Treiber-ABI ist nicht kompatibel |
| `NP_ERR_DRIVER_INVALID` | Treiberpaket oder Manifest ist ungültig |
| `NP_ERR_DRIVER_REJECTED` | Sicherheitsrichtlinie lehnt den Treiber ab |
| `NP_ERR_DRIVER_NOT_BOUND` | keine gültige Bindung vorhanden |
| `NP_ERR_DRIVER_CRASHED` | Treiberinstanz ist abgestürzt |
| `NP_ERR_DEVICE_REMOVED` | Gerät wurde entfernt |
| `NP_ERR_RESOURCE_CONFLICT` | Ressource ist nicht verfügbar |
| `NP_ERR_IO_CANCELLED` | I/O-Anfrage wurde abgebrochen |
| `NP_ERR_IO_FAILED` | I/O-Anfrage ist fehlgeschlagen |
| `NP_ERR_QUEUE_FULL` | I/O-Queue ist vollständig belegt |
| `NP_ERR_TIMEOUT` | Treiberoperation lief in einen Timeout |
| `NP_ERR_ACCESS_DENIED` | erforderliche Capability fehlt |
| `NP_ERR_NOT_SUPPORTED` | Operation wird nicht unterstützt |

## 58. Sicherheitsanforderungen

Das Driver Framework muss sicherstellen, dass:

- Treiber vor dem Laden verifiziert werden,
- Userspace-Treiber nur deklarierte Ressourcen erhalten,
- MMIO-Abbildungen nicht ausführbar sind,
- DMA ausschließlich über kontrollierte Domains erfolgt,
- Interruptquellen beim Treiberstopp maskiert werden,
- entfernte Geräte keine aktiven Zugriffsmöglichkeiten behalten,
- Treiber keine Kernelzeiger über öffentliche APIs erhalten,
- I/O-Puffer nur während ihrer gültigen Lebenszeit verwendet werden,
- Filtertreiber nicht ohne ausdrückliche Richtlinie installiert werden,
- fehlgeschlagene Initialisierungen vollständig zurückgerollt werden.

## 59. Datenschutz

Treiber können Zugriff auf sensible Geräte- und Benutzerdaten besitzen.

Daher gelten:

- Zugriff nur nach Capability und Anwendungsberechtigung,
- Datenerfassung auf das notwendige Minimum begrenzen,
- Logging und Tracing enthalten standardmäßig keine Payload-Daten,
- sensible Puffer nach Gebrauch sicher behandeln,
- Telemetrie bleibt standardmäßig lokal,
- externe Übertragung benötigt ausdrückliche Zustimmung und Konfiguration.

## 60. Testanforderungen

Mindestens folgende Tests sind erforderlich:

1. Laden eines gültigen Kernel-Treibers,
2. Laden eines gültigen Userspace-Treibers,
3. Ablehnung eines ungültigen Manifests,
4. Ablehnung einer inkompatiblen ABI,
5. Ablehnung eines nicht vertrauenswürdigen Treibers,
6. Treiber-Matching,
7. Probe ohne dauerhafte Ressourcenbelegung,
8. erfolgreiche Bindung,
9. Rollback einer fehlgeschlagenen Bindung,
10. erfolgreicher Start,
11. idempotenter Stop,
12. vollständiges Remove,
13. isolierter Userspace-Treiberprozess,
14. MMIO-Capability-Begrenzung,
15. I/O-Port-Begrenzung,
16. Interruptzustellung an Userspace,
17. Timeout einer Interruptbestätigung,
18. DMA-Mapping und Freigabe,
19. synchrone und asynchrone I/O-Anfrage,
20. I/O-Abbruch,
21. atomare Auflösung von Completion und Cancellation,
22. serielle und parallele I/O-Queues,
23. Queue-Backpressure,
24. angekündigtes Entfernen,
25. überraschendes Entfernen,
26. Suspend und Resume,
27. Wake-fähiges Gerät,
28. Bus- und Kindgeräteerzeugung,
29. Filtertreiberreihenfolge,
30. virtuelles Gerät,
31. Absturz eines Userspace-Treibers,
32. Widerruf aller Treiberressourcen,
33. kontrollierter Treiberneustart,
34. Begrenzung einer Neustartschleife,
35. Live-Update und Rollback,
36. Ablehnung der Entladung bei aktiven Referenzen,
37. parallele Verarbeitung auf mehreren CPUs,
38. Capability-Prüfung administrativer Operationen,
39. korrekte Logging-, Diagnose- und Statistikdaten.

## 61. Verbindliche Invarianten

1. Jede aktive Treiberinstanz ist genau einem Geräteobjekt zugeordnet.
2. Ein Treiber erhält nur die für seine Geräteinstanz autorisierten Ressourcen.
3. Interrupts werden erst aktiviert, wenn der Treiber empfangsbereit ist.
4. DMA erfolgt ausschließlich über kontrollierte DMA-Objekte und Domains.
5. Eine I/O-Anfrage wird höchstens einmal endgültig abgeschlossen.
6. Abschluss und Abbruch einer I/O-Anfrage werden atomar gegeneinander aufgelöst.
7. Nach `remove()` greift der Treiber nicht mehr auf das Gerät zu.
8. Ein abgestürzter Userspace-Treiber behält keine MMIO-, DMA- oder Interruptrechte.
9. Treibercode wird nicht entladen, solange er noch ausgeführt oder referenziert wird.
10. Treiber-ABI-Strukturen werden über Version und Größe validiert.
11. Kernel-Treiber sind auf technisch notwendige Einsatzbereiche begrenzt.
12. Treiberzustandsänderungen erfolgen unter geeigneter Synchronisation.

## 62. Referenzablauf eines Userspace-Treibers

```text
Gerät wird erkannt
  -> passenden Treiber ermitteln
  -> Manifest und Signatur prüfen
  -> Driver Host starten
  -> isolierte Capability-Domain erzeugen
  -> Geräte-, MMIO-, Interrupt- und DMA-Handles übergeben
  -> Probe und Bind ausführen
  -> I/O-Queues einrichten
  -> Gerät starten
  -> Interrupts freigeben
  -> I/O-Anfragen über IPC verarbeiten
  -> Fehler oder Entfernung kontrolliert behandeln
```

## 63. Zusammenfassung

Das NovaOS Driver Framework stellt ein gemeinsames, versioniertes Modell für Kernel- und Userspace-Treiber bereit.

Userspace-Treiber werden bevorzugt in isolierten Driver Hosts ausgeführt und erhalten ausschließlich capability-geschützte Ressourcen für ihr konkretes Gerät. MMIO, Interrupts und DMA bleiben unter Kontrolle des Kernels und können bei einem Treiberabsturz widerrufen werden.

Ein einheitliches I/O-Anfragemodell, klare Lifecycle-Callbacks, Plug-and-Play, Power Management und begrenzte Recovery-Richtlinien ermöglichen robuste Treiber, ohne gerätespezifische Logik unnötig in den Kernel zu verlagern.