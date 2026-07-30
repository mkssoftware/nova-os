# ADR-KERNEL-0102: Einheitliches Objektmodell für Kernel und Userspace

* **ADR-ID:** ADR-KERNEL-0102
* **Titel:** Einheitliches Objektmodell für Kernel und Userspace
* **Status:** Akzeptiert
* **Version:** 1.0.0
* **Datum:** 2026-07-29
* **Autor:** NovaOS Architekturgruppe
* **Kategorie:** Kernel Architecture
* **Gültigkeitsbereich:** Kernel Object Manager, Kernel Object Graph, System Call Interface, IPC, Userspace Runtime, Process Manager, Security Manager, Treiberframework, Systemdienste
* **Ersetzt:** Getrennte, inkompatible Objektmodelle für Kernel und Userspace
* **Ersetzt durch:** –
* **Abhängigkeiten:** ADR-KERNEL-0011, ADR-KERNEL-0012, ADR-KERNEL-0013, ADR-KERNEL-0015, ADR-KERNEL-0100, ADR-KERNEL-0101
* **Verwandte Dokumente:** ADR-MM-0004, ADR-MM-0005, ADR-MM-0008, ADR-KERNEL-0007
* **Entscheidungstyp:** Architekturentscheidung
* **Sicherheitsrelevanz:** Kritisch

---

# 1. Zusammenfassung

NovaOS verwendet ein **einheitliches Objektmodell für Kernel und Userspace**.

Kernelressourcen, Prozesse, Threads, Dateien, Geräte, Speicherbereiche, Kommunikationsendpunkte, Fenster, Dienste und weitere Systemressourcen werden durch dasselbe grundlegende Objektmodell beschrieben.

Kernel und Userspace teilen dabei nicht dieselben internen Speicherstrukturen. Sie verwenden jedoch dieselben semantischen Konzepte:

* Objektidentität
* Objekttyp
* Lebenszyklus
* Eigenschaften
* Beziehungen
* Ereignisse
* Operationen
* Handles
* Capabilities
* Namensräume
* Versionierung

Der Kernel bleibt alleiniger Eigentümer sicherheitskritischer Objektzustände. Der Userspace erhält kontrollierte Repräsentationen, Handles und Proxys auf diese Objekte.

Dadurch wird vermieden, dass jede Kernelkomponente, jedes System-API und jede Userspace-Laufzeit ein eigenes Ressourcenmodell implementiert.

---

# 2. Kontext

Traditionelle Betriebssysteme verwenden häufig mehrere voneinander getrennte Objekt- und Ressourcenmodelle.

Beispiele:

* Kernelobjekte
* Dateideskriptoren
* Prozess-Handles
* Gerätepfade
* IPC-Endpunkte
* Dateisystemobjekte
* Fenstersystemobjekte
* Dienstobjekte
* Benutzerraum-Proxys

Diese Modelle besitzen oft unterschiedliche:

* Identifikatoren
* Lebenszyklen
* Sicherheitsmechanismen
* Ereignismodelle
* Namensräume
* Fehlerkonzepte
* APIs

Dies erhöht die Gesamtkomplexität des Systems.

---

# 3. Problemstellung

Es musste entschieden werden,

* ob Kernel und Userspace getrennte Objektmodelle verwenden,
* wie Kernelobjekte im Userspace repräsentiert werden,
* wie Objekte sicher referenziert werden,
* wie Lebenszyklen über Prozessgrenzen hinweg verwaltet werden,
* wie Ereignisse zwischen Kernel und Userspace übertragen werden,
* wie Typinformationen verfügbar gemacht werden,
* und wie die ABI langfristig stabil gehalten wird.

---

# 4. Entscheidung

NovaOS definiert ein gemeinsames semantisches Objektmodell für Kernel und Userspace.

Dabei gelten folgende Grundsätze:

* Jede systemweit adressierbare Ressource ist ein Objekt.
* Jedes Objekt besitzt eine stabile Identität.
* Jedes Objekt besitzt genau einen registrierten Objekttyp.
* Kernelobjekte werden im Userspace über Handles oder Objektproxies dargestellt.
* Interne Kerneladressen werden niemals an den Userspace weitergegeben.
* Zugriffe erfolgen ausschließlich über definierte Operationen und Capabilities.
* Ereignisse verwenden dieselben Objektidentitäten wie synchrone Operationen.
* Typen und Schnittstellen sind versioniert.
* Kernel und Userspace verwenden dieselbe Objektsemantik, aber getrennte Speicherrepräsentationen.

---

# 5. Motivation

Die Entscheidung bietet:

* konsistente System-APIs
* geringere konzeptionelle Komplexität
* einheitliche Sicherheitsprüfungen
* bessere Introspektion
* klarere Lebenszyklen
* weniger Spezialfälle
* vereinfachte IPC
* bessere Erweiterbarkeit
* stabilere ABI-Grenzen
* leichtere Werkzeugintegration

---

# 6. Nicht-Ziele

Diese ADR fordert nicht:

* identische Speicherlayouts in Kernel und Userspace
* direkten Zugriff des Userspace auf Kernelstrukturen
* eine objektorientierte Programmiersprache
* dynamische Methodendisposition in jedem Kernelpfad
* automatische Netzwerktransparenz für jedes Objekt
* vollständige Serialisierbarkeit aller Kernelzustände
* die Abschaffung effizienter Fast Paths
* die Abschaffung interner, nicht exportierter Kernelstrukturen

---

# 7. Designprinzipien

Das einheitliche Objektmodell basiert auf:

* Trennung von Identität und Speicheradresse
* Trennung von Objektsemantik und Implementierung
* expliziten Lebenszyklen
* Capability-basierter Autorisierung
* versionierten Schnittstellen
* stabilen Objekt-IDs
* kontrollierter Introspektion
* typisierten Operationen
* typisierten Ereignissen
* minimaler Vertrauensbasis
* sicherer Prozessisolation

---

# 8. Architekturübersicht

```text
┌──────────────────────────────────────────────────────────────┐
│                         Userspace                            │
│                                                              │
│  Anwendung                                                   │
│     │                                                        │
│     ▼                                                        │
│  Userspace Object Proxy                                      │
│     │                                                        │
│     ▼                                                        │
│  Handle + Capability + Interface Version                     │
└──────────────────────────┬───────────────────────────────────┘
                           │
                           │ System Call / IPC
                           ▼
┌──────────────────────────────────────────────────────────────┐
│                          Kernel                              │
│                                                              │
│  Object Gateway                                              │
│     │                                                        │
│     ▼                                                        │
│  Handle Resolver                                             │
│     │                                                        │
│     ▼                                                        │
│  Capability Check                                            │
│     │                                                        │
│     ▼                                                        │
│  Kernel Object Manager                                       │
│     │                                                        │
│     ▼                                                        │
│  Kernel Object Graph                                         │
│                                                              │
└──────────────────────────────────────────────────────────────┘
```

---

# 9. Objektdefinition

Ein NovaOS-Objekt ist eine systemweit verwaltete Ressource mit:

* eindeutiger Identität
* registriertem Typ
* definiertem Lebenszyklus
* Eigenschaften
* Operationen
* Beziehungen
* Ereignissen
* Sicherheitskontext
* Version
* Eigentümer
* Referenzen

Ein Objekt kann physisch, logisch oder virtuell sein.

---

# 10. Objektkategorien

NovaOS unterscheidet mindestens:

* Kernel Core Objects
* Execution Objects
* Memory Objects
* Storage Objects
* Device Objects
* Communication Objects
* Security Objects
* Service Objects
* User Interface Objects
* Diagnostic Objects
* Virtualization Objects
* Distributed Objects

---

# 11. Beispiele für Objekttypen

Typische Objekttypen sind:

* Process
* Thread
* Address Space
* Virtual Memory Region
* Shared Memory
* File
* Directory
* Volume
* Device
* Driver
* Bus
* Interrupt
* Timer
* Event
* Event Channel
* Socket
* IPC Endpoint
* Service
* Window
* Surface
* Session
* User
* Security Token
* Capability
* Namespace
* Snapshot

---

# 12. Gemeinsame Objektbasis

Alle exportierbaren Objekte besitzen eine gemeinsame logische Basisstruktur.

```text
Nova Object
├── Object ID
├── Object Type ID
├── Object Version
├── Lifecycle State
├── Owner
├── Security Descriptor
├── Capability Requirements
├── Property Set
├── Operation Set
├── Event Set
└── Relationship Set
```

Diese Struktur beschreibt die Semantik des Objekts, nicht zwingend das physische Speicherlayout.

---

# 13. Objektidentität

Jedes Objekt erhält eine eindeutige Objekt-ID.

Die Objekt-ID muss:

* innerhalb des aktiven Systems eindeutig sein,
* unabhängig von virtuellen Adressen sein,
* nicht wiederverwendet werden, solange alte Referenzen existieren können,
* für Tracing und Diagnose geeignet sein,
* keine sensiblen Kerneladressen offenlegen,
* effizient vergleichbar sein.

---

# 14. Objekt-ID-Format

Das interne Format kann architekturabhängig optimiert werden.

Logisch enthält eine Objekt-ID mindestens:

* Instanzkennung
* Generationskennung
* Typinformation oder Typreferenz
* Gültigkeitsinformationen

Beispiel:

```text
┌──────────────┬──────────────┬──────────────┬──────────────┐
│ Namespace ID │ Generation   │ Object Index │ Integrity Tag│
└──────────────┴──────────────┴──────────────┴──────────────┘
```

Das konkrete Bitlayout wird in einer separaten Spezifikation festgelegt.

---

# 15. Objektadressen

Eine Objekt-ID ist keine Speicheradresse.

Der Userspace erhält niemals:

* Kernel Pointer
* physische Speicheradressen
* interne Slab-Adressen
* Objekt-Header-Adressen
* Seitentabellenadressen

Interne Kernelobjekte dürfen während ihrer Lebensdauer verschoben werden, solange ihre logische Identität erhalten bleibt.

---

# 16. Objekttypen

Jeder Objekttyp wird zentral registriert.

Ein Objekttyp definiert:

* Typ-ID
* Name
* Version
* Basistyp
* erlaubte Eigenschaften
* erlaubte Operationen
* erlaubte Ereignisse
* erlaubte Beziehungen
* Lifecycle Hooks
* Sicherheitsanforderungen
* Serialisierungsregeln
* Userspace-Sichtbarkeit

---

# 17. Typvererbung

Das Objektmodell unterstützt kontrollierte Typvererbung.

Beispiel:

```text
NovaObject
    │
    ├── ExecutionObject
    │      ├── Process
    │      └── Thread
    │
    ├── StorageObject
    │      ├── File
    │      ├── Directory
    │      └── Volume
    │
    └── DeviceObject
           ├── Bus
           ├── Device
           └── Driver
```

Vererbung beschreibt gemeinsame Semantik, nicht zwingend ein C++-ähnliches Speicherlayout.

---

# 18. Interfaces statt tiefer Vererbung

NovaOS bevorzugt Interfaces und Capabilities gegenüber tiefen Vererbungshierarchien.

Beispiele für Interfaces:

* Readable
* Writable
* Executable
* Mappable
* Waitable
* Enumerable
* Observable
* Configurable
* Transferable
* Persistable
* Recoverable

Ein Objekt kann mehrere Interfaces implementieren.

---

# 19. Objektoperationen

Objekte stellen typisierte Operationen bereit.

Eine Operation besitzt:

* Operations-ID
* Name
* Version
* Eingabeschema
* Ausgabeschema
* Capability-Anforderung
* Synchronitätsmodell
* Fehlerdefinition
* Timeoutverhalten
* Abbruchsemantik

---

# 20. Operationsaufruf

Der allgemeine Aufrufpfad lautet:

```text
Userspace Proxy
      │
      ▼
Handle
      │
      ▼
System Call
      │
      ▼
Handle Validation
      │
      ▼
Capability Validation
      │
      ▼
Interface Resolution
      │
      ▼
Object Operation
      │
      ▼
Result / Completion Event
```

---

# 21. Fast Paths

Häufig verwendete Operationen dürfen optimierte Systemaufrufe besitzen.

Beispiele:

* Lesen
* Schreiben
* Warten
* Signalisieren
* Speicherabbildung
* Thread Yield
* Timerabfrage

Diese Fast Paths bleiben semantisch Teil des Objektmodells.

---

# 22. Generische Objektoperationen

Zusätzlich zu spezialisierten Operationen unterstützt NovaOS generische Operationen:

* Query Type
* Query Properties
* Set Property
* Enumerate Interfaces
* Enumerate Relationships
* Duplicate Handle
* Transfer Handle
* Subscribe Events
* Close
* Query State

Generische Operationen dürfen keine sicherheitskritischen Typprüfungen umgehen.

---

# 23. Objekteigenschaften

Objekte können typisierte Eigenschaften besitzen.

Eine Eigenschaft enthält:

* Property ID
* Name
* Datentyp
* Version
* Leserechte
* Schreibrechte
* Änderbarkeit
* Sichtbarkeit
* Validierungsregeln
* Benachrichtigungsverhalten

---

# 24. Statische und dynamische Eigenschaften

NovaOS unterscheidet:

## Statische Eigenschaften

* Objekttyp
* Erstellungszeit
* Creator
* grundlegende Sicherheitsklasse
* ursprünglicher Namensraum

## Dynamische Eigenschaften

* Zustand
* Priorität
* Größe
* Besitzer
* Energiezustand
* Verbindungsstatus
* aktuelle Berechtigungen

---

# 25. Objektbeziehungen

Objekte können typisierte Beziehungen besitzen.

Beispiele:

* Owns
* Contains
* Uses
* Depends On
* Maps
* Connected To
* Backed By
* Created By
* Observes
* Attached To
* Scheduled On
* Protected By

Diese Beziehungen werden im Kernel Object Graph gespeichert.

---

# 26. Userspace-Repräsentation

Der Userspace arbeitet nicht mit vollständigen Kernelobjekten.

Stattdessen verwendet er:

* Handles
* Object Proxies
* Interface Proxies
* immutable Object Descriptors
* Shared Metadata Views
* Event Subscriptions

Ein Object Proxy enthält nur die für seine Funktion notwendigen Informationen.

---

# 27. Object Proxy

Ein Object Proxy ist eine Userspace-Repräsentation eines Kernelobjekts.

Er enthält typischerweise:

```text
Object Proxy
├── Handle
├── Object Type ID
├── Interface Version
├── Cached Properties
├── Runtime State
└── Userspace Methods
```

Der Proxy besitzt keine Autorität außerhalb der mit dem Handle verbundenen Capability.

---

# 28. Handles

Handles sind pro Prozess oder Namensraum gültige Referenzen auf Objekte.

Ein Handle muss:

* fälschungssicher sein,
* Generationsprüfungen unterstützen,
* effizient auflösbar sein,
* Capabilities tragen oder referenzieren,
* widerrufbar sein,
* übertragbar oder nicht übertragbar markiert werden können,
* beim Prozessende automatisch geschlossen werden.

---

# 29. Handle-Tabellen

Jeder Prozess besitzt mindestens eine Handle-Tabelle.

```text
Process
   │
   ▼
Handle Table
   │
   ├── Handle 0x21 ──> File Object
   ├── Handle 0x22 ──> Event Channel
   ├── Handle 0x23 ──> Shared Memory
   └── Handle 0x24 ──> Device Interface
```

Handle-Tabellen sind Kernelobjekte und Bestandteil des Kernel Object Graph.

---

# 30. Capabilities

Ein Handle allein reicht nicht zur Autorisierung.

Jeder Zugriff wird durch Capabilities begrenzt.

Beispiele:

* Read
* Write
* Execute
* Map
* Configure
* Observe
* Signal
* Wait
* Transfer
* Duplicate
* Destroy
* Administer

Capabilities gelten für ein konkretes Objekt oder eine definierte Objektgruppe.

---

# 31. Capability-Abschwächung

Beim Duplizieren oder Übertragen eines Handles dürfen Rechte reduziert werden.

Beispiel:

```text
Original Handle
    Read + Write + Configure
              │
              ▼
Abgeschwächtes Handle
    Read
```

Eine Rechteerweiterung ohne explizite Autorisierung ist unzulässig.

---

# 32. Objektlebenszyklus

Alle Objekte folgen einem einheitlichen Lebenszyklus.

```text
Allocated
    │
    ▼
Constructed
    │
    ▼
Initialized
    │
    ▼
Active
    │
    ├── Suspended
    │
    ├── Failed
    │
    └── Quiescing
    │
    ▼
Destroy Pending
    │
    ▼
Destroyed
    │
    ▼
Reclaimed
```

Nicht jeder Objekttyp muss jeden Zustand verwenden.

---

# 33. Objekterstellung

Objekte können erstellt werden durch:

* Kernelkomponenten
* Treiber
* Systemdienste
* Benutzerprozesse mit entsprechender Capability
* Hardwareerkennung
* Wiederherstellung
* Deserialisierung
* Klonen bestehender Objekte

Die Erstellung erfolgt über den Kernel Object Manager.

---

# 34. Objektinitialisierung

Die Initialisierung ist vom Sichtbarwerden getrennt.

Dadurch kann NovaOS:

* Eigenschaften validieren,
* Ressourcen reservieren,
* Abhängigkeiten auflösen,
* Sicherheitsrichtlinien anwenden,
* Beziehungen erstellen,
* Fehler ohne sichtbares Teilobjekt behandeln.

Ein Objekt wird erst nach erfolgreicher Initialisierung veröffentlicht.

---

# 35. Objektzerstörung

Objektzerstörung erfolgt mehrstufig.

```text
Destroy Request
      │
      ▼
Capability Check
      │
      ▼
New Operations Blocked
      │
      ▼
Dependencies Quiesced
      │
      ▼
Handles Invalidated
      │
      ▼
References Released
      │
      ▼
Object Reclaimed
```

---

# 36. Referenzzählung

Kernelobjekte verwenden Referenzzählung für aktive Verweise.

Referenzarten:

* starke Referenz
* schwache Referenz
* Handle-Referenz
* Graph-Referenz
* Ereignisreferenz
* temporäre Operationsreferenz
* Userspace-Proxy-Referenz

Eine starke Referenz verhindert die Freigabe des Objekts.

---

# 37. Zyklische Referenzen

Da der Kernel Object Graph Zyklen enthalten kann, reicht reine Referenzzählung nicht immer aus.

NovaOS verwendet ergänzend:

* schwache Beziehungen
* explizite Ownership-Kanten
* Lifecycle Domains
* Zyklenerkennung
* Quiescing-Protokolle
* Graphbereinigung
* diagnostische Leak Detection

---

# 38. Eigentümerschaft

Jedes Objekt besitzt ein definiertes Ownership-Modell.

Mögliche Eigentümer:

* Kernel
* Prozess
* Systemdienst
* Benutzer
* Session
* Container
* Gerät
* Namespace
* anderes Kernelobjekt

Eigentümerschaft ist nicht automatisch identisch mit Zugriffsrecht.

---

# 39. Lebenszyklusdomänen

Zusammengehörige Objekte können einer Lifecycle Domain angehören.

Beispiele:

* Prozess und Threads
* Treiber und Geräteinstanzen
* Volume und offene Dateisystemobjekte
* Session und UI-Objekte
* virtueller Computer und virtuelle Geräte

Eine Domain definiert geordnete Start-, Stop- und Recovery-Abläufe.

---

# 40. Namensräume

Objekte können in Namensräumen sichtbar sein.

Namensräume dienen:

* Isolation
* Organisation
* Containerisierung
* Sitzungsverwaltung
* Kompatibilität
* Ressourcenvirtualisierung

Ein Objekt kann in mehreren Namensräumen unterschiedliche Namen besitzen, ohne seine Identität zu verlieren.

---

# 41. Objektname und Objektidentität

Objektnamen sind nicht eindeutig genug, um als Identität zu dienen.

Daher gilt:

* Name ist eine Eigenschaft.
* Objekt-ID ist die Identität.
* Handles sind autorisierte Referenzen.
* Pfade sind Namensraumauflösungen.
* Namen können geändert werden.
* Objekt-IDs bleiben während der Lebensdauer stabil.

---

# 42. Userspace-API

Die Userspace-API stellt objektorientierte Semantik unabhängig von der verwendeten Programmiersprache bereit.

Konzeptionelle Operationen:

```text
nova_object_open()
nova_object_create()
nova_object_close()

nova_object_query_type()
nova_object_query_property()
nova_object_set_property()

nova_object_invoke()
nova_object_subscribe()
nova_object_wait()

nova_handle_duplicate()
nova_handle_transfer()
nova_handle_restrict()
```

Die endgültigen Namen werden durch die ABI-Spezifikation festgelegt.

---

# 43. Sprachbindungen

Das Objektmodell muss Bindings unterstützen für:

* NovaLang
* C
* C++
* Rust
* VB.NET-kompatible Laufzeiten
* C#
* Python
* JavaScript
* weitere Sprachen

Die Sprachbindung darf die Sicherheitssemantik des Kernelobjekts nicht verändern.

---

# 44. NovaLang-Integration

NovaLang soll Kernel- und Systemobjekte als native Sprachobjekte darstellen können.

Beispielhafte Semantik:

```text
let file = File.open("/data/example.txt")

file.write(data)

file.onChanged(handler)

file.close()
```

Intern bleibt dies eine sichere Folge von Handle- und Objektoperationen.

---

# 45. C-ABI

Für maximale Portierbarkeit stellt NovaOS eine stabile C-kompatible ABI bereit.

Die C-ABI verwendet:

* opaque Handles
* versionierte Strukturen
* explizite Größenfelder
* fest definierte Integergrößen
* keine Compiler-spezifischen Klassenlayouts
* keine direkten Kernelpointer
* keine impliziten Ausnahmen

---

# 46. ABI-Versionierung

Jede exportierte Schnittstelle besitzt:

* Major Version
* Minor Version
* Strukturgröße
* Feature Flags
* optionale Erweiterungsfelder

Kompatible Erweiterungen dürfen bestehende Binärprogramme nicht brechen.

---

# 47. Typversionierung

Objekttyp und Interface-Version werden getrennt verwaltet.

Beispiel:

```text
Object Type: File
Object Type Version: 3
Readable Interface: 2
Writable Interface: 1
Observable Interface: 4
```

Dadurch können einzelne Interfaces unabhängig weiterentwickelt werden.

---

# 48. Interface-Aushandlung

Ein Prozess kann prüfen, welche Interface-Version ein Objekt unterstützt.

Der Ablauf lautet:

```text
Query Interface
      │
      ▼
Requested Version
      │
      ▼
Capability Check
      │
      ▼
Compatible Version Found
      │
      ▼
Interface Handle Returned
```

---

# 49. Ereignismodell

Kernel und Userspace verwenden dasselbe semantische Ereignismodell.

Ein Objekt kann Ereignisse veröffentlichen wie:

* Created
* Ready
* Changed
* Suspended
* Resumed
* Failed
* Closing
* Destroyed
* Capability Revoked
* Relationship Changed

Die Zustellung in den Userspace erfolgt kontrolliert über Ereigniskanäle.

---

# 50. Ereignisabonnements

Der Userspace kann Ereignisse abonnieren, wenn er die notwendige Capability besitzt.

Ein Abonnement enthält:

* Quellobjekt
* Ereignistyp
* Filter
* Zielendpunkt
* Queue-Limit
* Zustellungsrichtlinie
* Datenschutzklassifikation
* Lebensdauer

---

# 51. Ereignissicherheit

Ereignisse dürfen keine Informationen offenlegen, auf die der Empfänger keinen Zugriff besitzt.

Vor der Zustellung werden geprüft:

* Objektberechtigung
* Property-Sichtbarkeit
* Payload-Felder
* Namensraumgrenzen
* Sicherheitsstufe
* Datenklassifikation
* Rate Limit

---

# 52. IPC-Integration

IPC-Endpunkte sind selbst Objekte.

Dadurch besitzen sie:

* Identität
* Capabilities
* Lebenszyklus
* Ereignisse
* Beziehungen
* Namensraumzuordnung

Kommunikationskanäle werden nicht als Sonderfall außerhalb des Objektmodells behandelt.

---

# 53. Shared Memory

Shared-Memory-Bereiche sind Objekte mit:

* Eigentümer
* Größe
* Mapping-Rechten
* Cache-Attributen
* Synchronisationsmodell
* Lebenszyklus
* Sicherheitskontext
* Beziehungen zu Prozessen

Mappings sind eigenständige Beziehungen zwischen Address Space und Shared Memory Object.

---

# 54. Dateisystemintegration

Dateien und Verzeichnisse sind Objekte.

Sie implementieren Interfaces wie:

* Readable
* Writable
* Enumerable
* Mappable
* Observable
* Persistable

Dateipfade bleiben Namensraummechanismen und sind nicht die Objektidentität.

---

# 55. Geräteintegration

Geräte werden über Device Objects repräsentiert.

Ein Gerät kann Interfaces bereitstellen wie:

* Block Device
* Character Device
* Network Adapter
* Display Output
* Input Device
* Sensor
* Compute Accelerator
* Power Controller

Userspace-Prozesse erhalten nur explizit freigegebene Geräteinterfaces.

---

# 56. Treiberintegration

Treiber sind Kernelobjekte oder isolierte Userspace-Serviceobjekte.

Ein Treiber besitzt Beziehungen zu:

* Device
* Bus
* Interrupt
* DMA Context
* Power Domain
* Firmware
* Security Policy

Der Object Manager verwaltet den Lebenszyklus dieser Beziehungen.

---

# 57. Prozessmodell

Ein Prozess ist ein Objekt.

Er besitzt Beziehungen zu:

* Address Space
* Threads
* Handle Tables
* Security Token
* Namespace
* Executable Image
* Parent Process
* Job oder Lifecycle Domain
* IPC Endpoints

---

# 58. Threadmodell

Ein Thread ist ein Objekt mit:

* Schedulerzustand
* Kernel Stack
* User Stack
* CPU-Affinität
* Priorität
* Sicherheitskontext
* Wait Objects
* Prozessbeziehung
* Ereignissen

Warteobjekte verwenden dieselbe Objektsemantik.

---

# 59. Waitable Objects

Objekte können das Interface `Waitable` implementieren.

Beispiele:

* Thread
* Process
* Event
* Semaphore
* Mutex
* Timer
* Completion Object
* IPC Endpoint

Dadurch kann ein einheitlicher Wait-Mechanismus verwendet werden.

---

# 60. Sicherheitsobjekte

Auch Sicherheitsressourcen sind Objekte.

Beispiele:

* Security Token
* Capability Set
* Policy
* Identity
* Certificate
* Trust Anchor
* Audit Channel
* Sandbox Profile

Sicherheitsobjekte besitzen besonders eingeschränkte Introspektions- und Übertragungsregeln.

---

# 61. Serviceobjekte

Systemdienste werden als Service Objects registriert.

Ein Service Object besitzt:

* Service Identity
* Endpoints
* Interfaces
* Health State
* Dependencies
* Lifecycle State
* Restart Policy
* Security Policy

Dadurch können Kernel und Userspace Systemdienste einheitlich entdecken und überwachen.

---

# 62. UI-Objekte

Fenster, Oberflächen und Sitzungen können ebenfalls Teil des Objektmodells sein.

Beispiele:

* Window
* Surface
* Display
* Input Context
* Clipboard
* Notification
* Session

UI-Objekte müssen nicht vollständig im Kernel implementiert sein. Sie verwenden jedoch dieselbe Identitäts-, Handle- und Capability-Semantik.

---

# 63. Objekt-Gateway

Der Übergang zwischen Kernel und Userspace erfolgt über ein Object Gateway.

Das Gateway übernimmt:

* Handleauflösung
* Typprüfung
* Versionsprüfung
* Capability-Prüfung
* Parameterprüfung
* Speichergrenzenprüfung
* Operation Dispatch
* Ergebnisvalidierung
* Ereignisregistrierung
* Audit

---

# 64. Parameterübertragung

Parameter werden übertragen als:

* kleine Inline-Werte
* versionierte Strukturen
* Handles
* Shared Memory
* I/O Vectors
* immutable Descriptors

Unkontrollierte Zeigerübergabe zwischen Kernel und Userspace ist unzulässig.

---

# 65. Fehlerobjekte

NovaOS kann komplexe Fehler als Error Objects darstellen.

Ein Error Object enthält:

* Error Domain
* Error Code
* Schweregrad
* Ursache
* betroffene Objekt-ID
* Kausalitätskette
* Recovery-Hinweise
* Diagnoseinformationen
* Datenschutzklassifikation

Einfache Fast Paths dürfen weiterhin kompakte Fehlercodes verwenden.

---

# 66. Introspektion

Das einheitliche Objektmodell ermöglicht kontrollierte Introspektion.

Mögliche Abfragen:

* Welchen Typ besitzt ein Objekt?
* Welche Interfaces unterstützt es?
* Welche Eigenschaften sind sichtbar?
* Welche Ereignisse stellt es bereit?
* Welche Beziehungen sind zugänglich?
* Welchen Lifecycle State besitzt es?
* Welche Capabilities besitzt das aktuelle Handle?

---

# 67. Introspektionsgrenzen

Nicht alle internen Informationen sind sichtbar.

Nicht standardmäßig exportiert werden:

* Kerneladressen
* vollständige Referenzgraphen
* Sicherheitsgeheimnisse
* fremde Prozessbeziehungen
* interne Sperrzustände
* kryptografische Schlüssel
* privilegierte Diagnoseinformationen

---

# 68. Objektbeschreibung

Für Diagnose und Werkzeuge kann der Kernel einen immutable Object Descriptor erzeugen.

Dieser enthält abhängig von den Rechten:

* Objekt-ID
* Typ
* Name
* Version
* Zustand
* sichtbare Eigenschaften
* sichtbare Interfaces
* sichtbare Beziehungen
* Ereignisunterstützung

---

# 69. Objektgraph im Userspace

Der vollständige Kernel Object Graph wird nicht direkt in den Userspace gespiegelt.

Stattdessen erhält ein Prozess eine gefilterte Sicht.

```text
Kernel Object Graph
        │
        ▼
Security Filter
        │
        ▼
Namespace Filter
        │
        ▼
Visibility Policy
        │
        ▼
Userspace Object View
```

---

# 70. Caching

Object Proxies dürfen unveränderliche oder selten veränderte Eigenschaften cachen.

Beispiele:

* Typname
* Interface-Version
* statische Flags
* Erstellungszeit

Dynamische Eigenschaften benötigen:

* Ablaufzeit
* Versionszähler
* Änderungsereignisse
* explizite Aktualisierung

---

# 71. Konsistenzmodell

NovaOS definiert mehrere Konsistenzklassen:

* Strong
* Snapshot
* Versioned
* Eventual
* Best Effort

Sicherheitskritische Eigenschaften benötigen starke oder versionierte Konsistenz.

---

# 72. Transaktionen

Mehrere Objektoperationen können optional in einer Transaktion zusammengefasst werden.

Beispiele:

* mehrere Dateisystemänderungen
* Prozessstart mit Ressourcenübergabe
* Geräte-Rekonfiguration
* Sicherheitsrichtlinienänderung
* Namespace-Umschaltung

Transaktionen müssen klar definierte Commit- und Rollback-Regeln besitzen.

---

# 73. Objektübertragung zwischen Prozessen

Handles können zwischen Prozessen übertragen werden.

Dabei werden geprüft:

* Transfer-Capability
* Zielprozess
* Namensraumgrenze
* Rechteabschwächung
* Objekttypregeln
* Sicherheitsrichtlinie
* Audit-Anforderung

---

# 74. Objektübertragung an Kindprozesse

Bei Prozesserstellung können Handles:

* vererbt,
* explizit übertragen,
* abgeschwächt,
* ausgeschlossen,
* durch neue Proxys ersetzt

werden.

Implizite Vererbung aller Handles ist nicht zulässig.

---

# 75. Handle-Widerruf

Capabilities und Handles können widerrufen werden.

Nach einem Widerruf:

* schlagen neue Operationen fehl,
* laufende Operationen werden abhängig vom Typ beendet oder abgeschlossen,
* Proxys erhalten ein Widerrufsereignis,
* gecachte Rechte werden ungültig,
* abhängige Handles können ebenfalls widerrufen werden.

---

# 76. Objektisolation

Das gemeinsame Objektmodell reduziert nicht die Isolation zwischen Kernel und Userspace.

Es gilt weiterhin:

* getrennte Adressräume
* privilegierte CPU-Modi
* validierte System Calls
* Capability-Prüfung
* Copy-in und Copy-out
* sichere Shared-Memory-Regeln
* getrennte Fehlerdomänen

---

# 77. Sandbox-Unterstützung

Sandboxes erhalten eine gefilterte Objektwelt.

Sie können beschränkt werden auf:

* erlaubte Objekttypen
* erlaubte Namensräume
* erlaubte Operationen
* maximale Objektanzahl
* maximale Ressourcenmenge
* erlaubte Ereignisse
* erlaubte Handle-Transfers

---

# 78. Container-Unterstützung

Container verwenden virtuelle Objektansichten.

Ein Container kann eigene sichtbare Objekte besitzen für:

* Prozesse
* Geräte
* Dateisysteme
* Netzwerkendpunkte
* Services
* Benutzer
* Sessions
* Ressourcenlimits

Die zugrunde liegenden Kernelobjekte behalten ihre systemweite Identität.

---

# 79. Virtualisierung

Virtuelle Maschinen und virtuelle Geräte werden ebenfalls als Objekte dargestellt.

Beispiele:

* Virtual Machine
* Virtual CPU
* Virtual Memory
* Virtual Device
* Virtual Interrupt Controller
* Virtual Bus
* Snapshot
* Migration Session

---

# 80. Netzwerkobjekte

Netzwerkressourcen sind Objekte.

Beispiele:

* Socket
* Interface
* Route
* Namespace
* Connection
* Listener
* Packet Queue
* Firewall Policy

Netzwerkobjekte verwenden dieselbe Handle-, Capability- und Ereignisarchitektur.

---

# 81. Verteilte Objekte

Das einheitliche Modell soll später verteilte Objektproxies unterstützen.

Dabei gilt:

* entfernte Objekte sind explizit als remote markiert,
* Netzwerklatenz bleibt sichtbar,
* Fehlerdomänen werden nicht verborgen,
* lokale und entfernte Handles sind unterscheidbar,
* Sicherheitsgrenzen bleiben erhalten,
* keine implizite vollständige Ortstransparenz.

---

# 82. Persistente Objektidentitäten

Nicht jedes Objekt benötigt eine Identität über Neustarts hinweg.

NovaOS unterscheidet:

* Ephemeral Object ID
* Boot-Stable Object ID
* Persistent Resource ID
* Globally Unique Resource ID

Dateien, Volumes und Geräte können persistente IDs besitzen. Threads und temporäre Ereignisse sind typischerweise flüchtig.

---

# 83. Serialisierung

Nur explizit freigegebene Objekte oder Deskriptoren dürfen serialisiert werden.

Serialisierung muss definieren:

* Schema
* Version
* Endianness
* Größenlimits
* Referenzbehandlung
* Capability-Ausschluss
* Sicherheitsfilter
* Integritätsprüfung

Ein Handle darf nicht als gültige Autorität serialisiert und später beliebig wiederhergestellt werden.

---

# 84. Neustart von Systemdiensten

Service Objects können nach einem Neustart eines Userspace-Dienstes erhalten bleiben oder neu verbunden werden.

Dazu können verwendet werden:

* stabile Service Identity
* Endpoint Rebinding
* Handle Revocation
* Proxy Reconnection
* State Restoration
* Recovery Events

---

# 85. Self-Healing

Das Objektmodell unterstützt Self-Healing durch:

* explizite Abhängigkeiten
* Gesundheitszustände
* Lifecycle Events
* Recovery Interfaces
* isolierte Fehlerdomänen
* automatische Handle-Widerrufe
* Objektneuerstellung
* Proxy-Neuverbindung

---

# 86. Diagnostik

Diagnosewerkzeuge können das Objektmodell verwenden für:

* Prozessinspektion
* Handle-Analyse
* Capability-Analyse
* Objektgraphdarstellung
* Ressourcenleckerkennung
* Lebenszyklusverfolgung
* Event-Korrelation
* Abhängigkeitsanalyse
* Deadlock-Analyse

---

# 87. Audit

Sicherheitsrelevante Objektoperationen können protokolliert werden.

Audit-Einträge enthalten abhängig von der Richtlinie:

* Subjekt
* Objekt-ID
* Objekttyp
* Operation
* Capability
* Ergebnis
* Zeit
* Prozess
* Session
* Fehler
* Korrelation

---

# 88. Datenschutz

Objektmetadaten können personenbezogene oder sensible Informationen enthalten.

Daher gelten:

* Datenminimierung
* Capability-basierte Sichtbarkeit
* Namensraumfilterung
* lokale Verarbeitung
* begrenzte Protokollierung
* kontrollierte Diagnosefreigabe
* Pseudonymisierung
* Aufbewahrungsfristen

---

# 89. Performance

Das Objektmodell darf keine unnötige dynamische Abstraktion in jeden Fast Path einführen.

Optimierungen umfassen:

* typisierte direkte Dispatch-Tabellen
* kompakte Handles
* Per-CPU-Caches
* Inline Capability Checks
* generationbasierte Validierung
* Interface-Caching
* Batch-Operationen
* Shared-Memory-Datenpfade
* spezialisierte System Calls

---

# 90. Speicherbedarf

Jedes Objekt verursacht Metadatenaufwand.

Dieser wird begrenzt durch:

* kompakte Objekt-Header
* optionale Erweiterungsblöcke
* ausgelagerte Property Sets
* typabhängige Metadaten
* Slab Caches
* gemeinsame immutable Type Descriptors
* schwache Graphkanten
* bedarfsgesteuerte Introspektionsdaten

---

# 91. SMP-Unterstützung

Das Objektmodell ist vollständig SMP-fähig.

Es unterstützt:

* atomare Referenzzählung
* lock-freie Handleauflösung
* Read-Copy-Update
* Per-CPU-Objektcaches
* fein granulare Sperren
* versionierte Properties
* parallele Ereigniszustellung
* CPU Hotplug

---

# 92. NUMA-Unterstützung

Objekte können NUMA-Metadaten besitzen.

Beispiele:

* bevorzugter NUMA-Knoten
* Speicherlokalität
* CPU-Lokalität
* Gerätelokalität
* Migrationsstatus

Object Proxies bleiben von der physischen Platzierung unabhängig.

---

# 93. Fehlergrenzen

Fehler im Userspace dürfen interne Kernelobjekte nicht beschädigen.

Dazu dienen:

* Parameterprüfung
* Handlevalidierung
* Capability-Prüfung
* Kopiergrenzen
* immutable Descriptors
* sichere Referenzzählung
* Timeout
* Abbruch
* Prozessisolation

---

# 94. Kompatibilitätsschichten

Kompatibilitätsschichten dürfen fremde Ressourcenmodelle auf das NovaOS-Objektmodell abbilden.

Beispiele:

* POSIX-Dateideskriptoren
* Windows-kompatible Handles
* Linux-kompatible Prozesse
* Android Binder Objects
* Web-API-Objekte

Die interne Autorität bleibt immer ein NovaOS-Handle oder eine NovaOS-Capability.

---

# 95. POSIX-Kompatibilität

POSIX-Dateideskriptoren werden als schmale Kompatibilitätssicht auf Handle-Einträge implementiert.

```text
POSIX FD
   │
   ▼
Compatibility Table
   │
   ▼
Nova Handle
   │
   ▼
Kernel Object
```

Dadurch bleibt die interne Architektur einheitlich.

---

# 96. Windows-Kompatibilität

Windows-kompatible Objekt- und Handle-Semantik kann über eine Kompatibilitätsschicht bereitgestellt werden.

Diese Schicht übersetzt:

* Handlezugriffe
* Objektarten
* Warteoperationen
* Namensräume
* Sicherheitsprüfungen
* Ereignisse

auf das native NovaOS-Objektmodell.

---

# 97. Android-Kompatibilität

Android-ähnliche Binder-Objekte können als:

* IPC Endpoint Objects
* Service Objects
* Capability Handles
* Shared Memory Objects
* Death Notifications

abgebildet werden.

---

# 98. Web- und UI-Runtime

Webbasierte NovaOS-Komponenten erhalten sichere JavaScript-Proxys.

Beispiel:

```text
JavaScript Object
       │
       ▼
Runtime Proxy
       │
       ▼
Brokered Capability
       │
       ▼
NovaOS Object Handle
```

Direkter Zugriff auf privilegierte Systemobjekte ist nicht zulässig.

---

# 99. Verworfene Alternative: getrennte Kernel- und Userspace-Modelle

Vorteile:

* unabhängige Implementierung
* geringere Anfangskomplexität
* bekannte Architektur

Nachteile:

* doppelte Semantik
* viele Übersetzungsschichten
* inkonsistente Lebenszyklen
* unterschiedliche Sicherheitsmodelle
* erschwerte Diagnose
* hoher Wartungsaufwand

---

# 100. Verworfene Alternative: direkte Kernelobjekte im Userspace

Vorteile:

* geringer Übersetzungsaufwand
* potenziell schnelle Zugriffe

Nachteile:

* Verlust der Isolation
* Offenlegung interner Layouts
* instabile ABI
* massive Sicherheitsrisiken
* keine kontrollierte Lebensdauer
* keine sichere Verschiebbarkeit

---

# 101. Verworfene Alternative: ausschließlich Dateideskriptoren

Vorteile:

* einfaches Modell
* POSIX-Kompatibilität
* kompakte Referenzen

Nachteile:

* eingeschränkte Typsemantik
* begrenzte Capability-Darstellung
* unzureichend für komplexe Objektbeziehungen
* schwache Introspektion
* viele Sonder-IOCTLs erforderlich

---

# 102. Verworfene Alternative: globale Objekt-IDs als alleinige Autorität

Vorteile:

* einfache systemweite Referenzierung

Nachteile:

* leicht fälschbar
* keine prozessbezogene Rechtekontrolle
* schlechte Widerrufbarkeit
* Risiko von Informationslecks
* keine sichere Rechteabschwächung

Objekt-IDs identifizieren Objekte, Handles und Capabilities autorisieren den Zugriff.

---

# 103. Verworfene Alternative: sprachabhängiges Objektmodell

Vorteile:

* komfortable Integration in eine einzelne Sprache
* Nutzung nativer Klassenkonzepte

Nachteile:

* Bindung an Compiler und Laufzeit
* instabile ABI
* schlechte Mehrsprachigkeit
* ungeeignet für Kernelgrenzen
* hohe Kompatibilitätskosten

---

# 104. Risiken

Wesentliche Risiken sind:

* zu hoher Abstraktionsaufwand
* generische APIs mit schlechter Performance
* unübersichtliche Typregistrierung
* Handle-Leaks
* zyklische Referenzen
* inkonsistente Proxy-Caches
* Capability-Fehlkonfigurationen
* zu weitreichende Introspektion
* ABI-Verfestigung zu früher Designentscheidungen

---

# 105. Gegenmaßnahmen

Die Risiken werden begrenzt durch:

* spezialisierte Fast Paths
* strikte Typregistry
* generationbasierte Handles
* Lifecycle Domains
* Capability-Prüfungen
* versionierte Interfaces
* Introspektionsfilter
* automatisierte Leak Detection
* Kompatibilitätstests
* ABI-Review-Prozesse

---

# 106. Implementierungsrichtlinien

Alle exportierbaren Systemressourcen müssen:

* als Objekttyp registriert werden,
* eine stabile Identität besitzen,
* einen definierten Lebenszyklus besitzen,
* Capabilities verwenden,
* versionierte Interfaces bereitstellen,
* Ereignisse typisiert veröffentlichen,
* keine Kernelpointer exportieren,
* sichere Handleauflösung verwenden,
* Namensraumregeln einhalten,
* dokumentierte Fehlersemantik besitzen.

---

# 107. Verbotene Muster

Nicht zulässig sind:

* rohe Kernelpointer im Userspace
* nicht versionierte exportierte Strukturen
* Handles ohne Generationsprüfung
* implizite Rechteerweiterung
* Objektzugriff ohne Capability-Prüfung
* versteckte globale Objektregister für Anwendungen
* unkontrollierte Handlevererbung
* sprachabhängige Kernel-ABI
* unbeschränkte Objektintrospektion
* direkte Manipulation interner Objektzustände

---

# 108. Implementierungsphasen

## Phase 1

* gemeinsamer Object Header
* Type Registry
* Object ID
* Handle Table
* Basis-Capabilities
* generische Query-Operation
* Prozess-, Thread- und Eventobjekte

## Phase 2

* Interface Registry
* Property System
* Userspace Object Proxies
* versionierte Object Gateway ABI
* Ereignisabonnements
* Shared Memory und IPC Objects

## Phase 3

* Dateisystem- und Geräteobjekte
* Lifecycle Domains
* gefilterte Objektgraphansichten
* Service Objects
* Kompatibilitätsschichten

## Phase 4

* UI Objects
* Containeransichten
* verteilte Proxys
* Recovery und Rebinding
* erweiterte Introspektion
* formale ABI-Stabilisierung

---

# 109. Teststrategie

Das Objektmodell wird geprüft durch:

* Unit-Tests
* Handle-Tests
* Capability-Tests
* Lifecycle-Tests
* Typversionierungstests
* ABI-Tests
* IPC-Tests
* Proxy-Tests
* SMP-Tests
* NUMA-Tests
* Sicherheitstests
* Langzeittests
* Fehler-Injektion
* Performancebenchmarks

---

# 110. Handle-Tests

Geprüft werden:

* gültige Auflösung
* ungültige Handles
* veraltete Generationen
* Handle-Duplizierung
* Rechteabschwächung
* Handle-Transfer
* Widerruf
* Prozessende
* parallele Zugriffe
* Handle-Tabellenwachstum

---

# 111. Lifecycle-Tests

Geprüft werden:

* erfolgreiche Erstellung
* fehlgeschlagene Initialisierung
* Suspend und Resume
* Quiescing
* Destroy Pending
* offene Referenzen
* zyklische Beziehungen
* Prozessabbruch
* Treiberentfernung
* Recovery

---

# 112. ABI-Tests

ABI-Tests prüfen:

* Strukturgrößen
* Feldpositionen
* Endianness
* Versionierung
* unbekannte Felder
* alte Anwendungen
* neue Kernelversionen
* verschiedene Compiler
* verschiedene Sprachen
* 32-Bit- und 64-Bit-Kompatibilität

---

# 113. Sicherheitstests

Geprüft werden:

* Handle-Fälschung
* Generation-Reuse
* Capability-Eskalation
* unzulässige Introspektion
* fremde Objektzugriffe
* Namensraumdurchbruch
* Proxy-Manipulation
* ungültige Parameter
* Race Conditions
* Use-after-free
* Double Close
* Transfermissbrauch

---

# 114. Performanceprüfungen

Gemessen werden:

* Handleauflösungszeit
* Objektaufruflatenz
* Capability-Check-Kosten
* Proxy-Overhead
* Ereigniszustellung
* Referenzzählungsaufwand
* Speicherbedarf pro Objekt
* Skalierung über CPUs
* Cache-Miss-Rate
* Fast-Path-Leistung

---

# 115. Akzeptanzkriterien

Die ADR gilt als umgesetzt, wenn:

* Kernelobjekte eine stabile Objekt-ID besitzen,
* Prozesse sichere Handles verwenden,
* Capabilities pro Handle geprüft werden,
* Userspace-Proxys keine Kernelpointer enthalten,
* Typen und Interfaces versioniert sind,
* Prozess-, Thread-, Speicher- und IPC-Objekte dasselbe Grundmodell verwenden,
* Ereignisse Objekt-IDs als Quelle und Ziel verwenden,
* Handle-Widerruf funktioniert,
* ABI-Kompatibilitätstests bestehen,
* der Kernel Object Graph auf demselben Objektmodell basiert.

---

# 116. Konsequenzen für bestehende Kernelkomponenten

Bestehende Kernelkomponenten müssen schrittweise migriert werden.

Empfohlene Reihenfolge:

1. Kernel Object Manager
2. Process Manager
3. Thread Manager
4. Handle Manager
5. Security Manager
6. Event Core
7. IPC
8. Memory Objects
9. Device Manager
10. Filesystem
11. Service Manager
12. UI-System

---

# 117. Konsequenzen für Userspace

Userspace-Komponenten erhalten:

* konsistente Objektproxys
* einheitliche Handleverwaltung
* gemeinsame Fehlersemantik
* einheitliche Ereignisabonnements
* sprachübergreifende APIs
* kontrollierte Introspektion
* klar definierte Lebenszyklen

Anwendungen müssen keine subsystemabhängigen Ressourcenmodelle mehr kombinieren.

---

# 118. Konsequenzen für NovaLang

NovaLang kann Systemressourcen als sichere native Objekte darstellen.

Der Compiler oder die Runtime kann:

* Handles automatisch schließen,
* Capabilities statisch darstellen,
* Interface-Versionen prüfen,
* asynchrone Operationen kapseln,
* Ereignisse als Streams darstellen,
* Lebensdauerfehler reduzieren.

Die Kernelautorität bleibt dennoch ausschließlich beim Kernel.

---

# 119. Konsequenzen für die Dokumentation

Jeder exportierte Objekttyp muss dokumentieren:

* Typ-ID
* Name
* Version
* Basistyp
* Interfaces
* Eigenschaften
* Operationen
* Ereignisse
* Beziehungen
* Lifecycle States
* Capability-Anforderungen
* Fehler
* ABI-Regeln
* Datenschutzklasse

---

# 120. Zukunftsperspektive

Langfristig soll das einheitliche Objektmodell erweitert werden um:

* formale Schnittstellenbeschreibungen
* automatisch generierte Sprachbindungen
* statische Capability-Prüfung in NovaLang
* verteilte Object Proxies
* Live-Objektgraphvisualisierung
* deterministisches Object Replay
* automatische Leak-Erkennung
* selbstheilende Service-Proxys
* KI-gestützte Objektgraphanalyse
* Cluster-übergreifende Objektidentitäten
* sichere persistente Object References

---

# 121. Konsequenz

Das einheitliche Objektmodell wird zur verbindlichen semantischen Grundlage aller exportierbaren NovaOS-Ressourcen.

Kernel und Userspace verwenden dieselben Konzepte für:

* Identität
* Typ
* Interface
* Lebenszyklus
* Sicherheit
* Ereignisse
* Beziehungen
* Fehler
* Versionierung

Die physische Implementierung bleibt getrennt, damit Isolation, Leistung und Sicherheit gewährleistet werden.

---

# 122. Zusammenfassung

NovaOS implementiert ein **einheitliches Objektmodell für Kernel und Userspace**.

Alle systemweit adressierbaren Ressourcen werden als typisierte Objekte mit stabiler Identität, definiertem Lebenszyklus, versionierten Interfaces, Capabilities, Ereignissen und Beziehungen modelliert.

Der Kernel verwaltet die autoritative Objektinstanz. Der Userspace erhält ausschließlich sichere Handles, gefilterte Deskriptoren und sprachspezifische Object Proxies.

Damit verbindet NovaOS:

* den Kernel Object Graph,
* den ereignisgesteuerten Kernel-Core,
* das Handle-System,
* die Capability-Sicherheit,
* IPC,
* Systemdienste,
* Geräte,
* Speicher,
* Dateisysteme,
* Benutzeroberflächen

zu einem konsistenten Systemmodell.

Diese Entscheidung reduziert architektonische Sonderfälle, verbessert die Sicherheitsprüfung, stabilisiert die ABI und schafft eine gemeinsame Grundlage für NovaLang, Kompatibilitätsschichten, Container, Systemdienste und zukünftige verteilte NovaOS-Komponenten.
