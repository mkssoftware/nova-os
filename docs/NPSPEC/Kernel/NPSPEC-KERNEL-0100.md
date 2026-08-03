# NPSPEC-KERNEL-0100 – Kernel Object Graph

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-KERNEL-0100 |
| Titel | Kernel Object Graph |
| Status | Spezifikation |
| Version | 1.0 |
| Datum | 2026-08-03 |
| Bereich | Kernel / Objektarchitektur |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | NPSPEC-KERNEL-0001, NPSPEC-KERNEL-0012, NPSPEC-KERNEL-0013, NPSPEC-KERNEL-0020, NPSPEC-KERNEL-0030 |
| Zugehörige ADRs | ADR-KERNEL-0100, ADR-KERNEL-0101, ADR-KERNEL-0102, ADR-KERNEL-0103, ADR-KERNEL-0104 |

---

## 1. Zweck

Diese Spezifikation definiert den Kernel Object Graph von NovaOS, im Folgenden KOG genannt.

Der KOG bildet Kernelressourcen und ihre Beziehungen als einheitlichen, gerichteten und typisierten Objektgraphen ab.

Er stellt eine gemeinsame Grundlage bereit für:

- Besitzverhältnisse,
- Abhängigkeiten,
- Sicherheitsdomains,
- Lebenszyklussteuerung,
- Ereignisweitergabe,
- Diagnose,
- Ressourcenabrechnung,
- Recovery und Fehlerisolation.

## 2. Ziele

Der Kernel Object Graph muss:

- isolierte Subsystem-Silos vermeiden,
- Beziehungen zwischen Kernelressourcen explizit darstellen,
- Objektlebenszyklen nachvollziehbar machen,
- Capability-basierte Navigation ermöglichen,
- sichere Abhängigkeitsauflösung unterstützen,
- parallele Änderungen auf SMP-Systemen erlauben,
- Diagnose ohne Offenlegung sensibler Daten ermöglichen,
- Teilgraphen für Sandboxes und Recovery isolieren,
- keine einzelne globale Graphsperre im normalen Betrieb benötigen.

## 3. Grundprinzip

Jede dauerhaft verwaltete Kernelressource ist ein Objekt.

Beziehungen zwischen Ressourcen werden als typisierte Kanten dargestellt.

```text
Objekt
  -> besitzt
  -> enthält
  -> hängt ab von
  -> ist gebunden an
  -> stellt bereit
  -> beobachtet
```

Subsysteme dürfen öffentlich referenzierbare Beziehungen nicht ausschließlich in privaten, für den Kernel unbekannten Nebentabellen verwalten.

## 4. Abgrenzung

| Komponente | Verantwortung |
|---|---|
| Object Manager | Objekttypen, Referenzen und Lebenszyklus |
| Kernel Object Graph | Beziehungen und Graphoperationen |
| Handle Manager | capability-geschützte Objektzugriffe |
| Security Manager | Sichtbarkeit und Zugriffsentscheidungen |
| Event Core | Zustands- und Beziehungsereignisse |
| Namespace Manager | namensbasierte Objektauffindbarkeit |
| Diagnostic Framework | autorisierte Graphabfragen |
| Resource Manager | Limits und Abrechnung |

## 5. Graphmodell

Der KOG ist ein gerichteter typisierter Multigraph.

Das bedeutet:

- eine Kante besitzt eine Richtung,
- jede Kante besitzt einen Typ,
- zwischen zwei Objekten können mehrere unterschiedliche Beziehungen bestehen,
- nicht jede Beziehung besitzt automatisch eine Gegenkante,
- Zyklen sind abhängig vom Beziehungstyp erlaubt oder verboten.

## 6. Kernelobjekt

```c
typedef struct np_kernel_object {
    np_object_header_t header;
    void* type_data;
} np_kernel_object_t;
```

Jedes Graphobjekt besitzt mindestens:

- eindeutige Objekt-ID,
- Objekttyp,
- Zustand,
- Referenzzähler,
- Sicherheitsmetadaten,
- Graphbeziehungen,
- optionalen Namespace-Eintrag.

## 7. Objektkennung

```c
typedef uint64_t np_object_id_t;
```

Eine Objekt-ID:

- ist während der relevanten Objektlebenszeit eindeutig,
- ist keine Speicheradresse,
- ist kein Handle,
- verleiht keine Zugriffsrechte,
- darf nicht als Autorisierungsnachweis verwendet werden.

Objekt-IDs können für Diagnose und Ereigniskorrelation verwendet werden.

## 8. Objekttypkennung

```c
typedef uint32_t np_object_type_id_t;
```

Jedes Objekt gehört genau einem registrierten Haupttyp an.

Zusätzliche Interfaces oder Rollen werden über:

- Interface-IDs,
- Feature-Flags,
- Graphbeziehungen,
- versionierte Serviceobjekte

dargestellt.

## 9. Kernobjekttypen

Mindestens folgende Typgruppen werden unterstützt:

| Gruppe | Beispiele |
|---|---|
| System | Kernel, Machine, Namespace |
| CPU | Package, Core, Logical CPU, NUMA Node |
| Process | Process, Thread, Job, Process Group |
| Memory | Address Space, Memory Object, DMA Object |
| IPC | Channel, Endpoint, Event, Pipe |
| Device | Device, Driver, Bus, Interrupt |
| VFS | Filesystem, Mount, Node, Open File |
| Network | Interface, Socket, Route, Tunnel |
| Security | Identity, Capability Domain, Policy |
| Diagnostic | Log Stream, Trace Session, Crash Dump |
| Power | Power Domain, Battery, Thermal Zone |

## 10. Wurzelobjekte

Der Graph besitzt mehrere ausdrücklich definierte Wurzelobjekte.

```text
Kernel Root
Machine Root
Namespace Root
Security Root
Device Root
Service Root
Recovery Root
```

Eine einzige allmächtige öffentliche Rootreferenz wird nicht an Userspace vergeben.

Jede Wurzel besitzt eigene Sicherheits- und Sichtbarkeitsregeln.

## 11. Kernel Root

Das Kernel-Root-Objekt repräsentiert die laufende Kernelinstanz.

Es kann Beziehungen besitzen zu:

- Machine Root,
- CPU Manager,
- Object Manager,
- Security Manager,
- Device Manager,
- VFS,
- Network Stack,
- Diagnostic Framework,
- Power Manager.

Der Zugriff auf das Kernel Root ist ausschließlich internen oder administrativ autorisierten Domains erlaubt.

## 12. Machine Root

Das Machine Root repräsentiert die aktuelle physische oder virtuelle Maschine.

Darunter können angeordnet sein:

- CPU-Packages,
- NUMA-Nodes,
- Memory Regions,
- Plattformbusse,
- Firmwareobjekte,
- Power Domains,
- Thermal Zones.

Virtuelle Maschinen werden ebenfalls durch ein Machine Root dargestellt.

## 13. Objektzustände

```c
typedef enum np_graph_object_state {
    NP_GRAPH_OBJECT_INITIALIZING,
    NP_GRAPH_OBJECT_ACTIVE,
    NP_GRAPH_OBJECT_QUIESCING,
    NP_GRAPH_OBJECT_FAILED,
    NP_GRAPH_OBJECT_REMOVING,
    NP_GRAPH_OBJECT_DESTROYED
} np_graph_object_state_t;
```

Ein Objekt im Zustand `INITIALIZING` wird nur für berechtigte Initialisierungspfade sichtbar.

`DESTROYED`-Objekte besitzen keine neuen Graphkanten.

## 14. Beziehungstypen

```c
typedef enum np_object_relation_type {
    NP_RELATION_OWNS,
    NP_RELATION_CONTAINS,
    NP_RELATION_PARENT_OF,
    NP_RELATION_CHILD_OF,
    NP_RELATION_DEPENDS_ON,
    NP_RELATION_BOUND_TO,
    NP_RELATION_PROVIDES,
    NP_RELATION_CONSUMES,
    NP_RELATION_MEMBER_OF,
    NP_RELATION_MAPPED_TO,
    NP_RELATION_OBSERVES,
    NP_RELATION_SECURED_BY
} np_object_relation_type_t;
```

Jeder Typ besitzt definierte Lebenszyklus- und Sicherheitsregeln.

## 15. Beziehungsklassen

Beziehungen werden zusätzlich klassifiziert:

```c
typedef enum np_relation_strength {
    NP_RELATION_WEAK,
    NP_RELATION_REFERENCE,
    NP_RELATION_STRONG,
    NP_RELATION_OWNERSHIP
} np_relation_strength_t;
```

| Klasse | Lebenszeitwirkung |
|---|---|
| `WEAK` | hält das Ziel nicht am Leben |
| `REFERENCE` | temporäre oder kontrollierte Referenz |
| `STRONG` | hält das Zielobjekt am Leben |
| `OWNERSHIP` | Besitzer steuert grundsätzlich den Lebenszyklus |

## 16. Graphkante

```c
typedef struct np_object_relation {
    np_relation_id_t relation_id;

    np_object_id_t source_id;
    np_object_id_t target_id;

    np_object_relation_type_t type;
    np_relation_strength_t strength;

    uint32_t flags;
    uint64_t generation;

    np_security_label_t security_label;
} np_object_relation_t;
```

Eine Kante ist kein direkt zugänglicher Kernelzeiger.

## 17. Beziehungseigenschaften

Eine Beziehung kann zusätzliche Eigenschaften besitzen:

- bidirektionale Darstellung,
- vererbte Sichtbarkeit,
- automatische Entfernung,
- Ereignisweitergabe,
- Sicherheitsgrenze,
- Lebenszyklusabhängigkeit,
- Reihenfolgengewicht,
- optionaler Zustand.

Eigenschaften müssen im Schema des Beziehungstyps registriert sein.

## 18. Beziehungsschema

```c
typedef struct np_relation_schema {
    np_object_relation_type_t relation_type;

    np_object_type_set_t allowed_sources;
    np_object_type_set_t allowed_targets;

    np_relation_strength_t maximum_strength;
    uint32_t flags;

    np_relation_validator_t validator;
    np_relation_cleanup_t cleanup;
} np_relation_schema_t;
```

Ein Prozess darf beispielsweise nur `Thread`-Objekte besitzen, wenn das Schema diese Kombination erlaubt.

## 19. Beziehungserzeugung

```c
np_status_t np_object_graph_link(
    np_kernel_object_t* source,
    np_kernel_object_t* target,
    np_object_relation_type_t type,
    np_relation_strength_t strength,
    uint32_t flags,
    np_relation_handle_t* relation
);
```

Vor der Erzeugung werden geprüft:

- Objektzustände,
- Typkompatibilität,
- Capability-Rechte,
- Beziehungsschema,
- Zyklusregeln,
- Ressourcenlimits,
- Sicherheitsdomains.

## 20. Atomare Veröffentlichung

Eine neue Beziehung wird erst sichtbar, wenn:

- Source- und Target-Referenzen gesichert,
- alle Metadaten vollständig initialisiert,
- Graphindizes aktualisiert,
- Sicherheitsinformationen angewendet,
- Generationswerte gesetzt

wurden.

Teilweise initialisierte Kanten dürfen nicht in Abfragen erscheinen.

## 21. Beziehungsentfernung

```c
np_status_t np_object_graph_unlink(
    np_relation_handle_t relation
);
```

Die Entfernung umfasst:

1. Kante für neue Leser als entfernt markieren,
2. Graphindizes aktualisieren,
3. Lifecycle-Callbacks ausführen,
4. gehaltene Referenzen reduzieren,
5. Beobachter benachrichtigen,
6. Kante nach einer Grace Period freigeben.

## 22. Besitzbeziehungen

`OWNS` beschreibt eine starke Lebenszyklusbeziehung.

Beispiele:

```text
Process OWNS Thread
Driver Instance OWNS I/O Queue
Network Namespace OWNS Route Table
Mount Namespace OWNS Mount
```

Das Ende des Besitzers löst die definierte Behandlung der besessenen Objekte aus.

Dies muss nicht immer eine sofortige Zerstörung bedeuten.

## 23. Containment

`CONTAINS` beschreibt eine logische oder strukturelle Einordnung.

Beispiele:

```text
Directory CONTAINS Node
NUMA Node CONTAINS Memory Range
Job CONTAINS Process
Power Domain CONTAINS Device
```

Containment kann stark oder schwach sein, abhängig vom konkreten Objekttyp.

## 24. Parent-Child-Beziehungen

Parent-Child wird verwendet für hierarchische Ressourcen.

Beispiele:

```text
Bus PARENT_OF Device
Process PARENT_OF Child Process
Device PARENT_OF Child Device
```

Die Beziehung ist nicht automatisch eine Sicherheitsberechtigung.

Ein Elternobjekt erhält nur die im Beziehungsschema definierten Verwaltungsrechte.

## 25. Abhängigkeiten

`DEPENDS_ON` beschreibt eine funktionale Abhängigkeit.

Beispiele:

```text
Filesystem DEPENDS_ON Block Device
Userspace Driver DEPENDS_ON Device Manager
VPN Interface DEPENDS_ON Network Interface
Module DEPENDS_ON Kernel Service
```

Abhängigkeiten werden für Start-, Stop-, Update- und Recovery-Reihenfolgen verwendet.

## 26. Bindungen

`BOUND_TO` beschreibt eine aktive Zuordnung.

Beispiele:

```text
Driver Instance BOUND_TO Device
Thread BOUND_TO CPU
Interrupt BOUND_TO Handler
Socket BOUND_TO Address
```

Eine Bindung muss aufgelöst werden, bevor eine beteiligte Ressource endgültig entfernt wird.

## 27. Provider und Consumer

`PROVIDES` und `CONSUMES` beschreiben Servicebeziehungen.

Beispiele:

```text
Service Process PROVIDES RPC Endpoint
Application CONSUMES Service
Driver PROVIDES Device Interface
Filesystem PROVIDES Mount
```

Diese Beziehungen können für Diagnose, Abhängigkeitsprüfung und Service-Recovery verwendet werden.

## 28. Sicherheitsbeziehungen

`SECURED_BY` verbindet ein Objekt mit:

- Capability-Domain,
- Sicherheitsprofil,
- ACL,
- Policy-Objekt,
- Trust Backend,
- Identität.

Eine Sicherheitsbeziehung darf nicht ohne Autorisierung entfernt oder umgebogen werden.

## 29. Starke Zyklen

Zyklen aus starken oder Ownership-Beziehungen können Objektfreigaben verhindern.

Der KOG muss:

- unzulässige Zyklen bei Link-Erzeugung erkennen,
- erlaubte Zyklen ausdrücklich im Schema markieren,
- für erlaubte Zyklen eine Auflösungsstrategie verlangen,
- Diagnoseinformationen bereitstellen.

Zyklen aus rein schwachen Beziehungen sind grundsätzlich zulässig.

## 30. Zyklenerkennung

Für kritische Beziehungstypen wird vor dem Einfügen geprüft, ob das Ziel bereits einen verbotenen Pfad zum Ausgangsobjekt besitzt.

Die Suche muss:

- eine maximale Tiefe besitzen,
- gegen parallele Graphänderungen abgesichert sein,
- den Beziehungstyp berücksichtigen,
- bei Unsicherheit restriktiv reagieren.

Große globale Vollgraphprüfungen dürfen nicht den normalen Fast Path dominieren.

## 31. Teilgraphen

Ein Teilgraph kann definiert werden durch:

- Root-Objekt,
- zulässige Beziehungstypen,
- maximale Tiefe,
- Sicherheitsdomain,
- Objekttypfilter,
- Namespace.

Teilgraphen werden verwendet für:

- Prozesssicht,
- Gerätesubtree,
- Job Object,
- Sandbox,
- Recovery-Domain,
- Diagnose.

## 32. Graphsicht

Eine Graphsicht ist ein capability-geschütztes Objekt.

```c
typedef struct np_graph_view {
    np_kernel_object_t object;

    np_object_id_t root_id;
    np_relation_type_mask_t allowed_relations;
    np_object_type_mask_t allowed_types;

    uint32_t maximum_depth;
    np_security_filter_t security_filter;
} np_graph_view_t;
```

Die Sicht kann restriktiver als der zugrunde liegende Graph sein.

## 33. Graphabfrage

```c
np_status_t np_object_graph_query(
    np_graph_view_handle_t view,
    const np_graph_query_t* query,
    np_graph_result_t* result
);
```

Abfragen können filtern nach:

- Objekt-ID,
- Objekttyp,
- Beziehungstyp,
- Zustand,
- Eigentümer,
- Namespace,
- Sicherheitsdomain,
- Generation.

Unbegrenzte Vollgraphabfragen sind für normale Prozesse unzulässig.

## 34. Traversierung

Traversierungen unterstützen:

- ausgehende Kanten,
- eingehende Kanten,
- Breadth-First,
- Depth-First,
- maximale Tiefe,
- maximale Ergebnismenge,
- Abbruchtoken,
- Generationserkennung.

Eine Traversierung garantiert ohne Snapshot keine unveränderte globale Sicht während paralleler Änderungen.

## 35. Graphcursor

```c
typedef struct np_graph_cursor {
    uint64_t generation;
    uint64_t position;
    np_object_id_t root_id;
    uint32_t flags;
} np_graph_cursor_t;
```

Ein Cursor kann ungültig werden, wenn der betrachtete Teilgraph strukturell verändert wurde.

In diesem Fall wird `NP_ERR_STALE_CURSOR` zurückgegeben.

## 36. Graph-Snapshot

Für konsistente Diagnose kann ein begrenzter Snapshot erzeugt werden.

```c
np_status_t np_object_graph_snapshot(
    np_graph_view_handle_t view,
    uint32_t flags,
    np_graph_snapshot_handle_t* snapshot
);
```

Ein Snapshot:

- ist schreibgeschützt,
- besitzt Ressourcenlimits,
- hält nicht zwingend alle Zielobjekte am Leben,
- verwendet stabile IDs und Zustandskopien,
- kann sensible Felder filtern.

## 37. Graphgeneration

Jede strukturelle Graphänderung aktualisiert eine geeignete Generation.

Der Kernel kann verwenden:

- globale Diagnosegeneration,
- Generation pro Teilgraph,
- Generation pro Objekttyp,
- Generation pro Namespace.

Eine einzige global serialisierte Generation im Fast Path ist nicht zwingend.

## 38. Ereignisse

Der KOG veröffentlicht strukturierte Ereignisse:

```text
OBJECT_ADDED
OBJECT_STATE_CHANGED
RELATION_ADDED
RELATION_REMOVING
RELATION_REMOVED
OBJECT_QUIESCING
OBJECT_FAILED
OBJECT_REMOVED
SUBGRAPH_ISOLATED
```

Ereignisse werden erst nach einem konsistenten Zustandswechsel veröffentlicht.

## 39. Ereignisreihenfolge

Für dieselbe Objekt- oder Beziehungsgeneration muss eine nachvollziehbare Reihenfolge bestehen.

Beispiel:

```text
OBJECT_ADDED
  -> RELATION_ADDED
  -> OBJECT_STATE_CHANGED: ACTIVE
```

Eine systemweit vollständige Reihenfolge aller unabhängigen CPU-Ereignisse ist nicht zwingend.

## 40. Event-Filterung

Empfänger sehen ausschließlich Ereignisse:

- innerhalb ihrer Graphsicht,
- für erlaubte Objekttypen,
- innerhalb ihrer Sicherheitsdomain,
- entsprechend ihrer Beobachtungscapability.

Sensible Objekt- und Beziehungseigenschaften werden vor der Zustellung gefiltert.

## 41. Lebenszyklussteuerung

Der KOG unterstützt Lifecycle-Entscheidungen anhand von Beziehungen.

Beispiele:

- Prozessende beendet oder übergibt Threads,
- Geräteentfernung stoppt Kindgeräte,
- Treiberende trennt Gerätebindungen,
- Mountende löst VFS-Beziehungen,
- Modulupdate behandelt abhängige Services.

Die konkrete Aktion wird durch Objekttyp- und Beziehungsschema definiert.

## 42. Quiesce

Vor der Entfernung komplexer Objekte kann ein Teilgraph in einen Quiesce-Zustand versetzt werden.

```c
np_status_t np_object_graph_quiesce(
    np_graph_view_handle_t view,
    uint32_t reason,
    np_time_ns_t deadline
);
```

Dabei werden:

- neue Operationen begrenzt,
- Abhängigkeiten informiert,
- laufende Arbeit abgeschlossen,
- nicht beendbare Beziehungen erkannt.

## 43. Entfernungsplanung

Für einen Teilgraphen kann eine sichere Entfernungsreihenfolge berechnet werden.

Grundsätzlich gilt:

```text
Consumer vor Provider
Child vor Parent
Dependent vor Dependency
Binding vor Zielobjekt
```

Zyklen oder nicht entladbare Objekte führen zu einem definierten Blockiergrund.

## 44. Fehlerisolation

Ein fehlgeschlagenes Objekt kann zusammen mit seinem abhängigen Teilgraphen isoliert werden.

Mögliche Maßnahmen sind:

- neue Zugriffe verweigern,
- Capabilities widerrufen,
- IPC-Endpunkte schließen,
- Geräteinterrupts maskieren,
- DMA-Mappings entfernen,
- abhängige Dienste neu starten,
- Teilgraph als `FAILED` markieren.

Isolation darf keine unbeteiligten Graphbereiche unkontrolliert beeinflussen.

## 45. Recovery

Der Recovery Manager kann den KOG verwenden, um:

- betroffene Komponenten zu bestimmen,
- Abhängigkeiten zu analysieren,
- Neustartreihenfolgen zu berechnen,
- Ersatzobjekte einzubinden,
- alte Beziehungen atomar umzuschalten,
- nicht wiederherstellbare Teilgraphen zu isolieren.

Eine Recovery-Aktion benötigt passende Capabilities und Policies.

## 46. Ressourcenabrechnung

Objekte und Beziehungen können einem Ressourcenbesitzer zugeordnet werden.

Beispiele:

- Prozess,
- Job Object,
- Sicherheitsdomain,
- Treiber,
- Benutzer,
- Systemdienst.

Abgerechnet werden können:

- Objektanzahl,
- Speicherverbrauch,
- Handles,
- IPC-Queues,
- Gerätebindungen,
- Graphkanten,
- Diagnose-Snapshots.

## 47. Ressourcenlimits

Limits gelten mindestens für:

- Objekte pro Prozess oder Job,
- Graphkanten,
- maximale Beziehungstiefe,
- Beobachter,
- Snapshots,
- Abfrageergebnisse,
- Ereignisqueues.

Ein unprivilegierter Prozess darf durch Graphoperationen keinen unbegrenzten Kernelressourcenverbrauch auslösen.

## 48. Kernel-Object-Graph-API

```c
np_status_t np_object_graph_link(
    np_handle_t source,
    np_handle_t target,
    np_object_relation_type_t type,
    uint32_t flags,
    np_relation_handle_t* relation
);

np_status_t np_object_graph_unlink(
    np_relation_handle_t relation
);

np_status_t np_object_graph_create_view(
    const np_graph_view_create_info_t* create_info,
    np_graph_view_handle_t* view
);

np_status_t np_object_graph_query(
    np_graph_view_handle_t view,
    const np_graph_query_t* query,
    np_graph_result_t* result
);
```

Nicht alle internen Beziehungstypen sind über Userspace-APIs veränderbar.

## 49. Capability-Rechte

Mindestens folgende Rechte werden definiert:

| Capability | Bedeutung |
|---|---|
| `CAP_GRAPH_QUERY` | erlaubten Teilgraph abfragen |
| `CAP_GRAPH_OBSERVE` | Graphereignisse beobachten |
| `CAP_GRAPH_LINK` | zulässige Beziehungen erzeugen |
| `CAP_GRAPH_UNLINK` | zulässige Beziehungen entfernen |
| `CAP_GRAPH_SNAPSHOT` | begrenzten Snapshot erzeugen |
| `CAP_GRAPH_QUIESCE` | Teilgraph kontrolliert anhalten |
| `CAP_GRAPH_ISOLATE` | fehlerhaften Teilgraph isolieren |
| `CAP_GRAPH_DIAGNOSTIC` | erweiterte Graphinformationen lesen |
| `CAP_GRAPH_ADMIN` | globale Graphrichtlinien verwalten |

Rechte werden auf Root-Objekte, Beziehungstypen und maximale Tiefe begrenzt.

## 50. Synchronisation

Der KOG verwendet:

- objektlokale Beziehungssperren,
- sharded Graphindizes,
- atomare Referenzen,
- RCU- oder Epoch-basierte Leser,
- verzögerte Kantenfreigabe,
- transaktionale Mehrkantenoperationen.

Eine einzelne globale Graphsperre im regulären Lookup- oder Ereignispfad ist unzulässig.

## 51. Sperrreihenfolge

Wenn mehrere Objekte gesperrt werden müssen, gilt eine deterministische Reihenfolge, beispielsweise:

```text
niedrigere Objekt-ID
  -> höhere Objekt-ID
```

Zusätzlich werden Index- und Namespace-Sperren nach einer dokumentierten globalen Reihenfolge erworben.

Callbacks werden nicht unter Graphkern-Sperren ausgeführt, wenn sie blockieren oder neue Graphoperationen auslösen können.

## 52. Parallelität

Der KOG muss sicher behandeln:

- gleichzeitiges Linken und Unlinken,
- Objektzerstörung während einer Abfrage,
- Snapshot während Zustandsänderungen,
- Ereigniszustellung während Entfernung,
- Recovery während Treiberneustart,
- parallele Teilgraphoperationen auf verschiedenen CPUs.

Temporäre Referenzen und Generationen verhindern Zugriffe auf freigegebene Objekte.

## 53. Transaktionen

Mehrere zusammengehörige Graphänderungen können transaktional ausgeführt werden.

```c
np_status_t np_graph_transaction_begin(
    np_graph_transaction_handle_t* transaction
);

np_status_t np_graph_transaction_link(...);
np_status_t np_graph_transaction_unlink(...);

np_status_t np_graph_transaction_commit(
    np_graph_transaction_handle_t transaction
);

void np_graph_transaction_abort(
    np_graph_transaction_handle_t transaction
);
```

Eine Transaktion wird vollständig veröffentlicht oder vollständig verworfen.

## 54. Sicherheitsbeziehungen

Graphoperationen erweitern niemals automatisch Capability-Rechte.

Eine neue Beziehung wie:

```text
Process MEMBER_OF Job
```

führt nur zu den im Schema und der Security Policy definierten Auswirkungen.

Graphnavigation erfordert bei jedem Objekt weiterhin die passende Sicht- oder Query-Berechtigung.

## 55. Namespace-Integration

Namespaces und KOG erfüllen unterschiedliche Aufgaben:

- Namespace: Auffindbarkeit über Namen,
- KOG: semantische Objektbeziehungen.

Ein Namespace-Eintrag kann eine Graphbeziehung zu seinem Zielobjekt besitzen.

Das Entfernen eines Namens muss das Zielobjekt nicht zerstören, solange andere Referenzen bestehen.

## 56. Handle-Integration

Ein Handle kann Zugriff auf ein Objekt oder eine Graphsicht gewähren.

Die Übergabe eines Objekthandles über IPC überträgt nicht automatisch:

- alle Nachbarobjekte,
- vollständige Traversierrechte,
- administrative Beziehungen,
- globale Namespace-Sicht.

Graphrechte müssen ausdrücklich delegiert werden.

## 57. Diagnose

Autorisierte Diagnosedienste können darstellen:

- Objekte und Typen,
- Zustände,
- Besitzbeziehungen,
- Abhängigkeiten,
- Geräte- und Treiberbindungen,
- Prozess- und Jobbeziehungen,
- Ressourcenverbrauch,
- isolierte Teilgraphen,
- blockierende Entfernungsabhängigkeiten,
- starke Zyklen.

Sensible Daten werden abhängig von Capability und Secure-Debugging-Status gefiltert.

## 58. Datenschutz

Graphbeziehungen können Benutzeraktivität, Anwendungen, Geräte und Kommunikation sichtbar machen.

Daher gelten:

- normale Prozesse sehen nur eigene oder freigegebene Teilgraphen,
- globale Graphdiagnose benötigt besondere Capabilities,
- sensible Namen und Identitäten können pseudonymisiert werden,
- IPC-Payloads und Dateiinhalte sind keine Graphdaten,
- Diagnose bleibt standardmäßig lokal,
- externe Telemetrie ist standardmäßig deaktiviert.

## 59. Statistik

```c
typedef struct np_object_graph_statistics {
    uint64_t active_objects;
    uint64_t active_relations;

    uint64_t ownership_relations;
    uint64_t dependency_relations;
    uint64_t weak_relations;

    uint64_t link_operations;
    uint64_t unlink_operations;

    uint64_t query_operations;
    uint64_t snapshot_operations;

    uint64_t rejected_cycles;
    uint64_t isolated_subgraphs;
} np_object_graph_statistics_t;
```

Statistiken sollen pro Objekttyp, Sicherheitsdomain und System verfügbar sein.

## 60. Fehlercodes

| Fehlercode | Bedeutung |
|---|---|
| `NP_OK` | Operation erfolgreich |
| `NP_ERR_OBJECT_NOT_FOUND` | Objekt wurde nicht gefunden |
| `NP_ERR_OBJECT_STATE` | Objektzustand erlaubt die Operation nicht |
| `NP_ERR_RELATION_INVALID` | Beziehung ist für die Typen unzulässig |
| `NP_ERR_RELATION_EXISTS` | identische exklusive Beziehung besteht bereits |
| `NP_ERR_RELATION_NOT_FOUND` | Beziehung wurde nicht gefunden |
| `NP_ERR_RELATION_CYCLE` | unzulässiger Zyklus erkannt |
| `NP_ERR_GRAPH_LIMIT` | Graphressourcenlimit erreicht |
| `NP_ERR_GRAPH_BUSY` | Teilgraph wird noch verwendet |
| `NP_ERR_STALE_CURSOR` | Cursor oder Generation ist veraltet |
| `NP_ERR_SNAPSHOT_TOO_LARGE` | Snapshot überschreitet die Grenze |
| `NP_ERR_DEPENDENCY_ACTIVE` | aktive Abhängigkeit verhindert Entfernung |
| `NP_ERR_ACCESS_DENIED` | erforderliche Capability fehlt |
| `NP_ERR_TRANSACTION_ABORTED` | Graphtransaktion wurde zurückgerollt |

## 61. Fehlerbehandlung

Ungültige Graphoperationen werden abgelehnt, ohne bestehende Beziehungen zu verändern.

Folgende Situationen gelten als schwere Kernelkonsistenzfehler:

- starke Beziehung verweist auf freigegebenes Objekt,
- Ownership-Zähler und Graphkante sind inkonsistent,
- doppelte Freigabe einer Kantenreferenz,
- beschädigter Graphindex,
- teilweise veröffentlichte Graphtransaktion,
- unerkannt ausgeführte Zerstörung trotz aktiver starker Abhängigkeit,
- Sicherheitsbeziehung wird ohne Autorisierung umgangen.

## 62. Testanforderungen

Mindestens folgende Tests sind erforderlich:

1. Registrierung eines Beziehungsschemas,
2. Erzeugung einer gültigen Kante,
3. Ablehnung inkompatibler Objekttypen,
4. starke und schwache Beziehung,
5. Ownership-Beziehung,
6. Parent-Child-Beziehung,
7. Dependency-Beziehung,
8. Binding-Beziehung,
9. Security-Beziehung,
10. atomare Veröffentlichung einer Kante,
11. Beziehungsentfernung,
12. verzögerte Kantenfreigabe,
13. Erkennung eines verbotenen starken Zyklus,
14. erlaubter schwacher Zyklus,
15. Teilgraphabfrage,
16. Tiefenbegrenzung,
17. Ergebnismengenbegrenzung,
18. Graphcursor,
19. veralteter Cursor,
20. konsistenter Graph-Snapshot,
21. parallele Link- und Unlink-Operationen,
22. Objektzerstörung während einer Abfrage,
23. Graphtransaktion mit mehreren Kanten,
24. Rollback einer fehlgeschlagenen Transaktion,
25. Prozess-Thread-Besitzbeziehung,
26. Job-Prozess-Mitgliedschaft,
27. Geräte-Treiber-Bindung,
28. Mount-Dateisystem-Abhängigkeit,
29. Service-Provider-Consumer-Beziehung,
30. Quiesce eines Teilgraphen,
31. Entfernungsreihenfolge,
32. Blockierung durch aktive Abhängigkeit,
33. Isolation eines fehlerhaften Treiberteilgraphen,
34. Recovery mit Ersatzobjekt,
35. Ressourcenlimits,
36. Capability-gefilterte Graphsicht,
37. Namespace- und Handle-Integration,
38. Datenschutzfilter,
39. korrekte Diagnose und Statistik.

## 63. Verbindliche Invarianten

1. Jedes Graphobjekt ist ein gültiges Objekt des Object Managers.
2. Jede Graphkante besitzt einen registrierten Beziehungstyp.
3. Beziehungssource und -ziel bleiben während einer starken Kante referenziert.
4. Teilweise initialisierte Kanten werden niemals sichtbar.
5. Objekt-IDs und Beziehungen verleihen allein keine Zugriffsrechte.
6. Verbotene starke Besitzzyklen werden nicht in den Graph aufgenommen.
7. Ein zerstörtes Objekt erhält keine neuen Beziehungen.
8. Eine Graphsicht zeigt nur Objekte und Kanten innerhalb ihrer Sicherheitsgrenzen.
9. Graphtransaktionen werden vollständig veröffentlicht oder vollständig verworfen.
10. Consumer- und Dependency-Beziehungen werden vor Entfernung eines Providers behandelt.
11. Das Entfernen eines Namespace-Namens zerstört nicht automatisch ein anderweitig referenziertes Objekt.
12. Graphänderungen erfolgen unter geeigneter Synchronisation und Lebenszeitsicherung.

## 64. Referenzablauf einer Gerätebindung

```text
Device Manager besitzt Geräteobjekt
  -> Driver Framework erzeugt Treiberinstanz
  -> KOG validiert Device-Driver-Beziehungsschema
  -> Treiberinstanz DEPENDS_ON Driver Module
  -> Treiberinstanz BOUND_TO Device
  -> Treiberinstanz SECURED_BY Capability Domain
  -> Device PROVIDES Device Interface
  -> alle Kanten in einer Transaktion vorbereiten
  -> Ressourcen und Capabilities prüfen
  -> Graphänderungen atomar veröffentlichen
  -> Device und Driver Instance auf ACTIVE setzen
  -> Graph- und Device-Ereignisse veröffentlichen
```

## 65. Zusammenfassung

Der NovaOS Kernel Object Graph bildet Kernelressourcen nicht als isolierte Subsystemeinträge, sondern als zusammenhängenden typisierten Objektgraphen ab.

Besitz, Containment, Abhängigkeiten, Bindungen, Services und Sicherheitsbeziehungen werden als explizite Kanten mit definierten Lebenszeit- und Zugriffsregeln verwaltet. Graphsichten und Teilgraphen begrenzen, welche Beziehungen ein Prozess oder Diagnosedienst sehen und verändern darf.

Transaktionen, Generationen, verzögerte Freigabe und sharded Indizes ermöglichen parallele Graphänderungen auf SMP-Systemen. Gleichzeitig können Device Manager, Process Manager, Recovery und Security Manager Abhängigkeiten analysieren und fehlerhafte Teilgraphen gezielt isolieren oder neu aufbauen.