# ADR-KERNEL-0105: ABI-Stabilität über versionierte Kernel-Services

* **ADR-ID:** ADR-KERNEL-0105
* **Titel:** ABI-Stabilität über versionierte Kernel-Services
* **Status:** Akzeptiert
* **Version:** 1.0.0
* **Datum:** 2026-07-29
* **Autor:** NovaOS Architekturgruppe
* **Kategorie:** Kernel Architecture
* **Gültigkeitsbereich:** Kernel Service Registry, System Call Interface, Kernel Object Manager, IPC, Treiberframework, Userspace Runtime, Systemdienste, Kompatibilitätsschichten
* **Ersetzt:** Direkte Bindung von Anwendungen und Treibern an interne Kernelstrukturen
* **Ersetzt durch:** –
* **Abhängigkeiten:** ADR-KERNEL-0100, ADR-KERNEL-0101, ADR-KERNEL-0102, ADR-KERNEL-0103, ADR-KERNEL-0104
* **Verwandte Dokumente:** ADR-KERNEL-0011, ADR-KERNEL-0012, ADR-KERNEL-0013, ADR-KERNEL-0015
* **Entscheidungstyp:** Architekturentscheidung
* **Kompatibilitätsrelevanz:** Kritisch
* **Sicherheitsrelevanz:** Hoch

---

# 1. Zusammenfassung

NovaOS gewährleistet langfristige ABI-Stabilität durch **versionierte Kernel-Services**.

Userspace-Anwendungen, Systemdienste, Treiber, Laufzeitumgebungen und Kompatibilitätsschichten greifen nicht direkt auf interne Kernelstrukturen oder unversionierte Kernel-Funktionsadressen zu.

Stattdessen stellt der Kernel logisch benannte, versionierte Services mit klar definierten:

* Service-IDs
* Interface-Versionen
* Operations-IDs
* Datenschemata
* Feature-Flags
* Capability-Anforderungen
* Fehlersemantiken
* Lebenszyklen
* Kompatibilitätsregeln

bereit.

Interne Kernelimplementierungen dürfen verändert, ersetzt oder reorganisiert werden, solange die zugesicherte Service-ABI eingehalten wird.

---

# 2. Kontext

Betriebssysteme müssen Anwendungen und Treiber über lange Zeiträume hinweg unterstützen.

Gleichzeitig entwickelt sich der Kernel kontinuierlich weiter.

Typische Änderungen sind:

* neue Scheduler
* neue Speicherverwaltung
* neue Dateisysteme
* neue Treibermodelle
* neue Sicherheitsmechanismen
* neue CPU-Architekturen
* neue Objektmodelle
* neue IPC-Mechanismen
* neue Optimierungen

Werden Programme direkt an interne Kernelstrukturen oder konkrete Funktionslayouts gebunden, kann bereits eine kleine interne Änderung bestehende Software inkompatibel machen.

---

# 3. Problemstellung

Es musste entschieden werden,

* wie NovaOS eine langfristig stabile ABI bereitstellt,
* wie Kernelinterfaces versioniert werden,
* wie alte Anwendungen mit neuen Kernelversionen funktionieren,
* wie inkompatible Änderungen eingeführt werden,
* wie mehrere Interface-Versionen parallel bestehen,
* wie Treiber und Systemdienste kompatibel bleiben,
* und wie interne Kernelarchitektur unabhängig weiterentwickelt werden kann.

---

# 4. Entscheidung

NovaOS stellt Kernel-Funktionalität ausschließlich über versionierte Kernel-Services bereit.

Dabei gelten folgende Grundsätze:

* Services werden über stabile Service-IDs identifiziert.
* Jeder Service besitzt mindestens eine Interface-Version.
* Operationen besitzen stabile Operations-IDs.
* Exportierte Datenstrukturen sind vollständig versioniert.
* Interne Kernelstrukturen sind niemals Bestandteil der öffentlichen ABI.
* Neue Kernelversionen unterstützen zugesicherte ältere Service-Versionen.
* Inkompatible Änderungen erzeugen eine neue Hauptversion.
* Feature-Aushandlung erfolgt explizit.
* Services werden über Handles und Capabilities verwendet.
* Service-Aufrufe können als System Call, IPC oder optimierter Fast Path umgesetzt werden.

---

# 5. Motivation

Die Entscheidung ermöglicht:

* langfristige Binärkompatibilität
* unabhängige Kernelentwicklung
* parallele Interface-Versionen
* kontrollierte Migration
* einfachere Treiberpflege
* stabile Sprachbindungen
* sichere Kompatibilitätsschichten
* modulare Kernelentwicklung
* geringeres Risiko unbeabsichtigter ABI-Brüche
* bessere Testbarkeit

---

# 6. Nicht-Ziele

Diese ADR verlangt nicht:

* unveränderliche interne Kernelstrukturen
* unbegrenzte Unterstützung jeder historischen Version
* identische ABI auf allen CPU-Architekturen
* vollständige Quellcodekompatibilität
* vollständige semantische Gleichheit aller Service-Versionen
* direkte Exportierung interner Kernelfunktionen
* dauerhaftes Beibehalten fehlerhafter Sicherheitssemantik
* Verzicht auf optimierte Fast Paths
* automatische Binärübersetzung beliebiger Fremdformate

---

# 7. Designprinzipien

Die Service-ABI basiert auf:

* expliziter Versionierung
* stabilen IDs
* opaken Handles
* strukturierter Feature-Aushandlung
* klaren Kompatibilitätsverträgen
* minimalen öffentlichen Schnittstellen
* Trennung von Semantik und Implementierung
* Capability-basierter Autorisierung
* kontrollierter Deprecation
* automatisierten ABI-Tests
* architekturunabhängiger Semantik

---

# 8. Architekturübersicht

```text
┌───────────────────────────────────────────────────────────────┐
│                         Userspace                             │
│                                                               │
│  Anwendung / Runtime / Systemdienst / Treiber                 │
│                         │                                     │
│                         ▼                                     │
│                Service Client Library                         │
│                         │                                     │
│                         ▼                                     │
│              Service-ID + Interface-Version                   │
└─────────────────────────┬─────────────────────────────────────┘
                          │
                          │ System Call / IPC / Fast Path
                          ▼
┌───────────────────────────────────────────────────────────────┐
│                          Kernel                               │
│                                                               │
│                 Kernel Service Gateway                        │
│                          │                                    │
│                          ▼                                    │
│                 Kernel Service Registry                       │
│                          │                                    │
│             ┌────────────┼────────────┐                       │
│             ▼            ▼            ▼                       │
│         Service v1   Service v2   Service v3                  │
│             │            │            │                       │
│             └────────────┼────────────┘                       │
│                          ▼                                    │
│                Interne Kernelimplementierung                  │
└───────────────────────────────────────────────────────────────┘
```

---

# 9. Kernel-Service

Ein Kernel-Service ist eine logisch abgegrenzte, versionierte Kernel-Schnittstelle.

Ein Service stellt eine zusammengehörige Gruppe von Operationen bereit.

Beispiele:

* Process Service
* Thread Service
* Memory Service
* Object Service
* Event Service
* Filesystem Service
* Device Service
* Security Service
* Scheduler Service
* Diagnostic Service
* Power Service
* Network Service

---

# 10. Service-Identität

Jeder Kernel-Service besitzt eine stabile Service-ID.

Die Service-ID muss:

* global innerhalb der NovaOS-ABI eindeutig sein,
* unabhängig von Funktionsadressen sein,
* unabhängig von der internen Implementierung sein,
* über Kernelversionen hinweg stabil bleiben,
* maschinenlesbar dokumentiert sein,
* in Diagnoseinformationen verwendbar sein.

---

# 11. Service-ID

Eine Service-ID kann logisch dargestellt werden als:

```text
Kernel Service
├── Service Namespace
├── Service Number
├── Service Name
└── Service Class
```

Das konkrete numerische Format wird durch eine separate ABI-Spezifikation festgelegt.

---

# 12. Service Registry

Alle verfügbaren Kernel-Services werden in einer Kernel Service Registry registriert.

Die Registry verwaltet:

* Service-ID
* Servicename
* unterstützte Versionen
* Operationskatalog
* Feature-Flags
* Capability-Anforderungen
* Zuständiges Kernelmodul
* Lifecycle State
* Kompatibilitätsstatus
* Deprecation-Status

---

# 13. Service Discovery

Clients können Services explizit abfragen.

Konzeptioneller Ablauf:

```text
Client
  │
  ▼
Service Query
  │
  ├── Service-ID
  ├── minimale Version
  ├── maximale Version
  └── gewünschte Features
  │
  ▼
Kernel Service Registry
  │
  ▼
Kompatible Version
  │
  ▼
Service Handle
```

---

# 14. Service Handle

Der erfolgreiche Service-Lookup liefert ein Service Handle.

Das Handle enthält oder referenziert:

* gewählte Service-Version
* verfügbare Features
* Capability-Rechte
* Service-Instanz
* Namensraumkontext
* Sicherheitskontext
* Generationskennung

Ein Service Handle ist keine direkte Funktionsadresse.

---

# 15. Versionierungsmodell

NovaOS verwendet für Kernel-Services eine dreiteilige Versionierung:

```text
MAJOR.MINOR.PATCH
```

Beispiel:

```text
3.4.2
```

Die Komponenten besitzen klar definierte Bedeutungen.

---

# 16. Major-Version

Die Major-Version wird erhöht, wenn:

* bestehende Semantik inkompatibel verändert wird,
* Operationsparameter inkompatibel geändert werden,
* Rückgabewerte inkompatibel geändert werden,
* Sicherheitsannahmen grundlegend verändert werden,
* Operationen entfernt werden,
* Datenlayouts inkompatibel geändert werden.

Mehrere Major-Versionen dürfen parallel existieren.

---

# 17. Minor-Version

Die Minor-Version wird erhöht, wenn:

* neue optionale Operationen ergänzt werden,
* neue optionale Felder ergänzt werden,
* neue Feature-Flags eingeführt werden,
* bestehende Semantik kompatibel erweitert wird,
* zusätzliche Diagnoseinformationen bereitgestellt werden.

Ein Client einer älteren Minor-Version muss weiterhin funktionieren.

---

# 18. Patch-Version

Die Patch-Version kennzeichnet:

* Fehlerkorrekturen
* Sicherheitskorrekturen
* Performanceverbesserungen
* interne Implementierungsänderungen
* Dokumentationskorrekturen

Die öffentliche Semantik darf sich dabei nicht inkompatibel ändern.

---

# 19. Interface-Version

Ein Service kann mehrere Interfaces besitzen.

Beispiel:

```text
Memory Service 4
├── Allocation Interface 3
├── Mapping Interface 5
├── Protection Interface 2
├── Shared Memory Interface 4
└── Statistics Interface 1
```

Einzelne Interfaces können unabhängig versioniert werden.

---

# 20. Operations-IDs

Jede Serviceoperation besitzt eine stabile Operations-ID.

Die ID bleibt innerhalb einer Major-Version unverändert.

Beispiele:

```text
PROCESS_CREATE       = 0x0001
PROCESS_TERMINATE    = 0x0002
PROCESS_QUERY        = 0x0003
PROCESS_SUBSCRIBE    = 0x0004
```

Operations-IDs dürfen innerhalb derselben Major-Version nicht wiederverwendet werden.

---

# 21. Entfernte Operationen

Wird eine Operation innerhalb einer Servicefamilie entfernt, bleibt ihre ID reserviert.

Die ID darf nicht für eine andere Semantik wiederverwendet werden.

Ein Aufruf einer nicht mehr unterstützten Operation liefert einen definierten Fehlerstatus.

---

# 22. Exportierte Datenstrukturen

Alle ABI-relevanten Datenstrukturen müssen versioniert sein.

Eine exportierte Struktur enthält mindestens:

* Strukturgröße
* Strukturversion
* Flags
* reservierte Felder
* definierte Ausrichtung
* definierte Integergrößen

Beispiel:

```c
typedef struct nova_process_create_info
{
    uint32_t size;
    uint16_t version_major;
    uint16_t version_minor;
    uint64_t flags;
    nova_handle_t executable;
    nova_handle_t namespace_handle;
    uint64_t reserved[4];
} nova_process_create_info_t;
```

---

# 23. Strukturgröße

Das Feld `size` ermöglicht kompatible Erweiterungen.

Der Kernel darf nur Felder lesen, die innerhalb der vom Client angegebenen Strukturgröße liegen.

Neue Clients können größere Strukturen übergeben.

Alte Kernel dürfen unbekannte Erweiterungsfelder ignorieren, sofern dies semantisch zulässig ist.

---

# 24. Reservierte Felder

Exportierte Strukturen enthalten reservierte Felder für spätere Erweiterungen.

Reservierte Felder müssen:

* beim Erzeugen auf null gesetzt werden,
* vom Empfänger ignoriert werden,
* bei Rückgabe definiert behandelt werden,
* dokumentiert bleiben.

Sie dürfen später nur durch eine kompatible Spezifikation verwendet werden.

---

# 25. Primitive Datentypen

Die öffentliche ABI verwendet ausschließlich eindeutig definierte primitive Typen.

Zulässig sind beispielsweise:

* `uint8_t`
* `uint16_t`
* `uint32_t`
* `uint64_t`
* `int32_t`
* `int64_t`
* definierte Handle-Typen
* explizite Offset-Typen
* explizite Größen-Typen

Nicht zulässig sind ABI-abhängige C-Typen ohne definierte Breite.

---

# 26. Zeiger in der ABI

Rohe Kernelpointer sind niemals Bestandteil der ABI.

Userspace-Zeiger dürfen nur als explizit validierte Pufferreferenzen verwendet werden.

Bevorzugt werden:

* Handles
* Shared Memory Objects
* Buffer Descriptors
* I/O Vectors
* Offset-und-Längen-Paare
* immutable Descriptors

---

# 27. Endianness

Die native lokale ABI darf die Endianness der Zielarchitektur verwenden.

Persistente, verteilte oder architekturübergreifende Serviceformate müssen eine kanonische Endianness definieren.

Endianness darf niemals implizit bleiben, wenn Daten:

* persistent gespeichert,
* zwischen Architekturen übertragen,
* über Netzwerke versendet,
* in Crash Dumps eingebettet

werden.

---

# 28. Alignment und Padding

Alle ABI-Strukturen definieren:

* Mindest-Alignment
* Feldreihenfolge
* Padding-Verhalten
* Strukturgröße
* Pack-Regeln

Compilerabhängiges Padding darf nicht unkontrolliert Teil der ABI werden.

---

# 29. Aufrufkonvention

Die NovaOS-System-ABI definiert pro Architektur:

* Registerbelegung
* Stack-Layout
* Rückgaberegister
* Fehlerübertragung
* Registererhaltung
* Alignment
* Übergabe großer Strukturen
* Übergabe variabler Daten

Die logische Service-ABI bleibt architekturübergreifend konsistent.

---

# 30. Service-Aufrufmodell

Ein generischer Serviceaufruf besitzt konzeptionell folgende Form:

```text
Service Handle
      ├── Operation ID
      ├── Input Descriptor
      ├── Output Descriptor
      ├── Call Flags
      └── Timeout
      │
      ▼
Kernel Service Gateway
      │
      ▼
Version Dispatch
      │
      ▼
Capability Check
      │
      ▼
Service Operation
```

---

# 31. System Calls

Häufig verwendete Kernel-Services dürfen spezialisierte System Calls bereitstellen.

Beispiele:

* Thread wechseln
* Speicher abbilden
* auf Objekt warten
* Datei lesen
* IPC-Nachricht senden

Diese spezialisierten Aufrufe bleiben semantisch an einen versionierten Kernel-Service gebunden.

---

# 32. Generischer Service Call

Zusätzlich zu spezialisierten System Calls stellt NovaOS einen generischen Serviceaufruf bereit.

Dieser eignet sich für:

* selten verwendete Operationen
* optionale Services
* Erweiterungen
* Diagnosefunktionen
* experimentelle Features
* dynamisch geladene Kernelmodule

---

# 33. Fast Paths

Fast Paths dürfen den generischen Dispatch umgehen, wenn:

* die Semantik vollständig identisch bleibt,
* die Versionsprüfung erhalten bleibt,
* die Capability-Prüfung erhalten bleibt,
* Fehlercodes identisch bleiben,
* kein interner Kernelzustand offengelegt wird.

---

# 34. Service-Aushandlung

Beim Öffnen eines Services gibt der Client an:

* minimale akzeptierte Version
* maximale akzeptierte Version
* verpflichtende Features
* optionale Features
* gewünschte Sicherheitsklasse
* gewünschte Aufrufmodi

Der Kernel wählt die höchste kompatible Version.

---

# 35. Versionsbereich

Ein Client darf einen Versionsbereich definieren.

Beispiel:

```text
Minimal: 2.1
Maximal: 3.x
```

Der Kernel darf nur eine Version innerhalb dieses Bereichs auswählen.

---

# 36. Feature-Flags

Optionale Fähigkeiten werden über Feature-Flags ausgehandelt.

Beispiele:

* Async Calls
* Batch Operations
* Shared Memory Transport
* Extended Diagnostics
* Transaction Support
* Cancellation
* Event Subscription
* Zero-Copy I/O

Ein nicht gesetztes Feature darf vom Client nicht verwendet werden.

---

# 37. Pflichtfeatures

Clients können Features als verpflichtend markieren.

Fehlt ein Pflichtfeature, schlägt die Service-Aushandlung kontrolliert fehl.

Dadurch werden spätere Laufzeitfehler vermieden.

---

# 38. Service Capabilities

Der Zugriff auf einen Service ist capability-basiert.

Ein Service Handle kann Rechte enthalten wie:

* Query
* Invoke
* Configure
* Observe
* Administer
* Delegate
* Debug
* Recover

Die Verfügbarkeit eines Services bedeutet nicht automatisch Zugriffsberechtigung.

---

# 39. Capability-Prüfung

Vor jeder Operation prüft das Service Gateway:

1. Handle-Gültigkeit
2. Service-Generation
3. Interface-Version
4. Operations-ID
5. Capability-Rechte
6. Parametergröße
7. Sicherheitskontext
8. Namensraum
9. Lifecycle State
10. Rate Limit

---

# 40. Namensräume

Kernel-Services können namenspacelokal sein.

Beispiele:

* Process Service
* Device Service
* Network Service
* Filesystem Service
* Session Service

Ein Container oder eine Sandbox kann dadurch eine gefilterte Servicesicht erhalten.

---

# 41. Service-Instanzen

Ein Service kann:

* global,
* pro Namensraum,
* pro Session,
* pro Prozess,
* pro Gerät,
* pro virtueller Maschine

instanziiert werden.

Service-ID und Service-Instanz sind getrennte Konzepte.

---

# 42. Lebenszyklus eines Services

Ein Kernel-Service besitzt mindestens folgende Zustände:

```text
Registered
    │
    ▼
Initializing
    │
    ▼
Available
    ├── Degraded
    ├── Updating
    └── Quiescing
    │
    ▼
Unavailable
    │
    ▼
Unregistered
```

---

# 43. Service-Registrierung

Ein Kernelmodul registriert einen Service über die Service Registry.

Die Registrierung enthält:

* Service Descriptor
* unterstützte Versionen
* Operations Dispatch Tables
* Feature Flags
* Capability Policy
* Lifecycle Hooks
* Diagnoseinformationen
* Modulreferenz

---

# 44. Dynamische Kernelmodule

Dynamisch geladene Kernelmodule dürfen versionierte Services bereitstellen.

Beim Entladen müssen:

* neue Serviceaufrufe blockiert werden,
* laufende Aufrufe abgeschlossen oder abgebrochen werden,
* Service Handles invalidiert oder migriert werden,
* Ereignisse veröffentlicht werden,
* Referenzen freigegeben werden.

---

# 45. Service-Rebinding

Services dürfen intern neu gebunden werden.

Beispiele:

* Austausch einer Treiberimplementierung
* Wechsel eines Dateisystembackends
* Neustart eines Userspace-Systemdienstes
* Live-Update
* Recovery

Der Client behält nach Möglichkeit sein Service Handle, solange die zugesicherte Semantik erhalten bleibt.

---

# 46. Kernel- und Userspace-Services

Nicht jeder versionierte Service muss vollständig im Kernel implementiert sein.

Ein Service kann umgesetzt werden als:

* Kernel-intern
* Userspace-Systemdienst
* isolierter Treiberprozess
* Hypervisor-Service
* Kompatibilitätsdienst

Für Clients bleibt das Service-Modell semantisch konsistent.

---

# 47. Service Broker

Für Userspace-Services kann ein Service Broker eingesetzt werden.

```text
Client
  │
  ▼
Service Handle
  │
  ▼
Kernel Service Gateway
  │
  ▼
Service Broker
  │
  ▼
Userspace Service
```

Der Broker kontrolliert:

* Identität
* Capabilities
* Nachrichtenformat
* Lebenszyklus
* Fehlergrenzen
* Rebinding

---

# 48. Fehlersemantik

Alle Kernel-Services verwenden ein einheitliches Fehlergrundmodell.

Ein Fehler enthält mindestens:

* Fehlerdomäne
* Fehlercode
* Schweregrad
* Retry-Klassifikation
* betroffene Operation
* optionale Detailreferenz

Einfache Fast Paths dürfen kompakte Fehlercodes verwenden.

---

# 49. Stabile Fehlercodes

Fehlercodes dürfen innerhalb einer Major-Version nicht mit anderer Bedeutung wiederverwendet werden.

Neue Fehlercodes dürfen ergänzt werden.

Clients müssen unbekannte Fehlercodes sicher behandeln können.

---

# 50. Fehlerkategorien

Mindestens vorgesehen sind:

* Success
* Invalid Argument
* Unsupported Version
* Unsupported Operation
* Missing Feature
* Access Denied
* Invalid Handle
* Object Gone
* Busy
* Timeout
* Cancelled
* Resource Exhausted
* Temporary Failure
* Permanent Failure
* Internal Error

---

# 51. Asynchrone Serviceoperationen

Services können asynchrone Operationen bereitstellen.

Eine asynchrone Operation liefert:

* Operation Handle
* Completion Object
* Korrelations-ID
* optionalen Event Channel
* Abbruchmöglichkeit
* Timeoutsemantik

---

# 52. Completion Interface

Das Completion Interface wird separat versioniert.

Es unterstützt:

* Statusabfrage
* Warten
* Ergebnisabholung
* Abbruch
* Fehlerabfrage
* Ereignisabonnement

---

# 53. Batch-Operationen

Services dürfen Batch-Aufrufe unterstützen.

Ein Batch muss definieren:

* maximale Elementanzahl
* atomare oder nicht atomare Semantik
* Fehlerverhalten
* Teilresultate
* Reihenfolge
* Abbruchverhalten
* Ressourcenlimits

---

# 54. Transaktionen

Bestimmte Services können transaktionale Operationen anbieten.

Beispiele:

* Dateisystemänderungen
* Namespace-Konfiguration
* Sicherheitsrichtlinien
* Prozessstart
* Geräte-Rekonfiguration

Transaktionsschnittstellen müssen separat versioniert sein.

---

# 55. Ereignisintegration

Serviceänderungen werden über den ereignisgesteuerten Kernel-Core veröffentlicht.

Beispiele:

* Service Registered
* Service Available
* Service Updated
* Service Deprecated
* Service Degraded
* Service Rebound
* Service Unavailable
* Service Removed

---

# 56. Objektmodellintegration

Ein Kernel-Service ist selbst ein Objekt im einheitlichen Objektmodell.

Er besitzt:

* Objekt-ID
* Servicetyp
* Lifecycle State
* Interfaces
* Capabilities
* Beziehungen
* Ereignisse
* Diagnoseinformationen

---

# 57. Kernel Object Graph

Kernel-Services werden in den Kernel Object Graph integriert.

Beispiele für Beziehungen:

```text
Process
  └── uses ─────────> Process Service

Device Service
  └── provided-by ──> Device Manager

Filesystem Service
  └── depends-on ───> Block Device Service

Diagnostic Service
  └── observes ─────> Kernel Event Core
```

---

# 58. Telemetrieintegration

Jeder Service stellt lokale Diagnosedaten bereit.

Mindestens erfasst werden:

* Aufrufanzahl
* Fehleranzahl
* Latenz
* aktive Handles
* unterstützte Versionen
* Feature-Nutzung
* abgelehnte Capability-Prüfungen
* veraltete Interface-Nutzung
* Rebinding-Vorgänge

---

# 59. Datenschutz

Service-Telemetrie folgt dem Local-First-Prinzip.

Es gelten:

* lokale Speicherung
* Datenminimierung
* Capability-basierter Zugriff
* keine automatische externe Übertragung
* begrenzte Aufbewahrung
* Filterung sensibler Parameter
* Pseudonymisierung von Prozessbezügen

---

# 60. ABI-Kompatibilitätsvertrag

Jeder Service veröffentlicht einen formalen ABI-Kompatibilitätsvertrag.

Dieser definiert:

* unterstützte Versionen
* Mindestunterstützungsdauer
* Deprecation-Regeln
* Datenstrukturen
* Operations-IDs
* Fehlercodes
* Feature-Flags
* Sicherheitssemantik
* Migrationspfade

---

# 61. Kompatibilitätsklassen

NovaOS unterscheidet:

* Experimental
* Preview
* Stable
* Long-Term Stable
* Deprecated
* Removed

Nur `Stable` und `Long-Term Stable` erhalten verbindliche ABI-Garantien.

---

# 62. Experimentelle Services

Experimentelle Services dürfen häufiger geändert werden.

Sie müssen:

* ausdrücklich als experimentell markiert sein,
* eine separate Service-ID oder Versionsdomäne verwenden,
* außerhalb kritischer Systempfade liegen,
* nicht stillschweigend zu stabilen Services werden.

---

# 63. Preview-Services

Preview-Services sind weitgehend spezifiziert, können aber noch inkompatible Änderungen erhalten.

Clients müssen die Preview-Nutzung explizit aktivieren.

---

# 64. Stable Services

Stable Services garantieren:

* stabile Major-Version
* dokumentierte Semantik
* automatisierte ABI-Tests
* kontrollierte Deprecation
* definierte Migrationspfade

---

# 65. Long-Term-Stable Services

Long-Term-Stable Services sind für langfristig unterstützte Kernfunktionen vorgesehen.

Beispiele:

* Object Service
* Process Service
* Thread Service
* Memory Service
* Event Service
* Basic Filesystem Service

Sie erhalten besonders strenge Änderungsregeln.

---

# 66. Deprecation

Ein Service oder eine Operation darf nicht ohne Übergangsphase entfernt werden.

Die Deprecation umfasst:

1. Markierung als veraltet
2. Diagnosewarnungen
3. Dokumentation der Alternative
4. parallele Unterstützung
5. Migrationswerkzeuge
6. definierte Entfernungsversion

---

# 67. Deprecation-Warnungen

Warnungen dürfen ausgegeben werden über:

* lokale Diagnoseschnittstelle
* Entwicklerwerkzeuge
* Build-System
* Runtime Loader
* Service Query
* Ereigniskanäle

Produktivanwendungen dürfen nicht durch unnötige Warnungsfluten beeinträchtigt werden.

---

# 68. Sicherheitsbedingte ABI-Änderungen

Sicherheitskritische Fehler können eine beschleunigte ABI-Änderung erforderlich machen.

In diesem Fall muss NovaOS:

* die gefährdete Operation deaktivieren oder einschränken,
* eine sichere Ersatzversion bereitstellen,
* lokale Diagnoseinformationen erzeugen,
* betroffene Software identifizieren,
* einen Migrationspfad anbieten.

ABI-Stabilität darf keine bekannte Sicherheitslücke konservieren.

---

# 69. Parallelbetrieb mehrerer Versionen

Der Kernel darf mehrere Major-Versionen eines Services parallel bereitstellen.

Beispiel:

```text
Filesystem Service
├── Version 2
├── Version 3
└── Version 4
```

Alle Versionen können intern auf dieselbe aktuelle Implementierung abgebildet werden.

---

# 70. Compatibility Adapter

Ältere Service-Versionen können durch Adapter umgesetzt werden.

```text
Client v1
   │
   ▼
Service ABI v1
   │
   ▼
Compatibility Adapter
   │
   ▼
Current Internal Service
```

Adapter müssen vollständig getestet und diagnostizierbar sein.

---

# 71. Semantische Übersetzung

Ein Compatibility Adapter kann:

* Parameter konvertieren
* Standardwerte ergänzen
* alte Fehlercodes abbilden
* alte Ereignisse erzeugen
* Capability-Rechte reduzieren
* veraltete Verhalten emulieren

Er darf keine Sicherheitsgarantien der aktuellen Implementierung abschwächen.

---

# 72. Kompatibilitätsmodus

Ein Prozess kann optional in einem definierten ABI-Kompatibilitätsmodus gestartet werden.

Dieser kann festlegen:

* Ziel-ABI-Version
* erlaubte Services
* alte Fehlersemantik
* alte Namensraumregeln
* alte Zeitdarstellung
* alte Strukturversionen

---

# 73. Prozessmanifest

Anwendungen können in einem Manifest angeben:

* benötigte Services
* minimale Versionen
* maximale getestete Versionen
* Pflichtfeatures
* optionale Features
* Kompatibilitätsprofil
* benötigte Capabilities

Dadurch können Inkompatibilitäten bereits beim Start erkannt werden.

---

# 74. Loader-Integration

Der Program Loader prüft vor dem Start:

* erforderliche Services
* Versionen
* Features
* Architektur
* ABI-Profil
* Capability-Berechtigungen
* Kompatibilitätsschichten

Fehlende Anforderungen führen zu einem klar definierten Startfehler.

---

# 75. Laufzeitbibliotheken

NovaOS stellt stabile Userspace-Laufzeitbibliotheken bereit.

Diese kapseln:

* Service Discovery
* Versionsaushandlung
* Strukturinitialisierung
* Fehlerkonvertierung
* Handleverwaltung
* Event Subscription
* asynchrone Operationen

Anwendungen dürfen Services dennoch direkt über die dokumentierte ABI verwenden.

---

# 76. Sprachbindungen

Versionierte Kernel-Services bilden die Grundlage für Bindings in:

* NovaLang
* C
* C++
* Rust
* C#
* VB.NET-kompatiblen Laufzeiten
* Python
* JavaScript
* weiteren Sprachen

Die Bindings werden aus maschinenlesbaren Servicebeschreibungen erzeugt.

---

# 77. Service Description Language

NovaOS definiert langfristig eine Service Description Language.

Diese beschreibt:

* Service-IDs
* Versionen
* Operationen
* Parameter
* Rückgabewerte
* Capabilities
* Ereignisse
* Fehler
* Feature-Flags
* Lifecycle-Regeln

---

# 78. Automatische Codegenerierung

Aus der Servicebeschreibung können generiert werden:

* Kernel Dispatch Tables
* Userspace Header
* NovaLang Bindings
* Rust Bindings
* C# Bindings
* Dokumentation
* Mock-Services
* ABI-Tests
* Fuzzing-Schemas

---

# 79. Treiber-ABI

Treiber verwenden ebenfalls versionierte Services.

Ein Treiber darf nicht direkt von:

* internen Schedulerstrukturen
* internen Speicherstrukturen
* internen Geräteobjekten
* privaten Kernel-Symbolen
* konkreten Lock-Implementierungen

abhängen.

---

# 80. Treiber-Servicegruppen

Treiber erhalten Services für:

* Geräteerkennung
* Interruptverwaltung
* DMA
* Speicher
* Logging
* Energieverwaltung
* Firmware
* Synchronisation
* Ereignisse
* Objektverwaltung

---

# 81. Userspace-Treiber

Userspace-Treiber verwenden dieselben versionierten Serviceprinzipien.

Der Transport erfolgt über:

* IPC
* Shared Memory
* Event Channels
* Capability Handles
* optimierte Brokerpfade

---

# 82. Kernelmodule

Interne Kernelmodule sollen ebenfalls versionierte interne Services verwenden.

Interne Services können geringere Stabilitätsgarantien besitzen, müssen aber eindeutig klassifiziert sein.

Unterschieden werden:

* Private Kernel Interface
* Internal Stable Interface
* Public Kernel Service
* Userspace ABI Service

---

# 83. Private Kernel Interfaces

Private Interfaces:

* dürfen nur innerhalb eines Moduls verwendet werden,
* besitzen keine externe Stabilitätsgarantie,
* dürfen nicht von Treibern oder Userspace genutzt werden,
* werden nicht in der öffentlichen Service Registry veröffentlicht.

---

# 84. Internal Stable Interfaces

Internal Stable Interfaces dienen der Kommunikation zwischen Kernelmodulen.

Sie verwenden:

* Versionen
* Service-IDs
* definierte Operationen
* Lebenszyklusregeln
* Kompatibilitätstests

Ihre Stabilitätsdauer kann kürzer als die der öffentlichen ABI sein.

---

# 85. Öffentliche Kernel-Services

Öffentliche Kernel-Services sind die einzige native ABI für Anwendungen, Systemdienste und externe Treiber.

Sie unterliegen den strengsten Kompatibilitätsregeln.

---

# 86. POSIX-Kompatibilität

POSIX-Systemaufrufe werden als Kompatibilitätsschicht über native Kernel-Services umgesetzt.

```text
POSIX API
   │
   ▼
POSIX Runtime
   │
   ▼
NovaOS Service ABI
   │
   ▼
Kernel Service
```

Die native interne Architektur bleibt dadurch unabhängig von POSIX.

---

# 87. Windows-Kompatibilität

Windows-kompatible APIs werden auf versionierte NovaOS-Services abgebildet.

Die Kompatibilitätsschicht übersetzt:

* Handles
* Prozesse
* Threads
* Dateien
* Events
* Speicher
* Sicherheitsmodelle
* Fehlercodes

---

# 88. Linux-Kompatibilität

Linux-kompatible System Calls können in ein bestimmtes NovaOS-Serviceprofil übersetzt werden.

Die Kompatibilitätsschicht darf nicht auf private Kernelstrukturen zugreifen.

---

# 89. Android-Kompatibilität

Android-Dienste und Binder-Semantik werden auf:

* IPC Service
* Shared Memory Service
* Process Service
* Security Service
* Event Service

abgebildet.

---

# 90. Architekturportabilität

Die semantische Service-ABI bleibt über Architekturen hinweg konsistent.

Architekturspezifisch sind nur:

* System-Call-Instruktion
* Registerbelegung
* Pointerbreite
* Alignment
* Endianness
* optionale Hardwarefeatures

---

# 91. 32-Bit- und 64-Bit-Kompatibilität

NovaOS unterstützt getrennte ABI-Profile für:

* 32-Bit-Userspace auf 32-Bit-Kernel
* 64-Bit-Userspace auf 64-Bit-Kernel
* optional 32-Bit-Userspace auf 64-Bit-Kernel

Pointerabhängige Strukturen werden vermieden oder explizit übersetzt.

---

# 92. Cross-ABI Gateway

Ein Cross-ABI Gateway übernimmt bei Bedarf:

* Strukturkonvertierung
* Pointerbreitenkonvertierung
* Alignment-Anpassung
* System-Call-Übersetzung
* Fehlercodeübersetzung
* Handlevalidierung

---

# 93. Live-Update

Versionierte Services ermöglichen Kernel-Live-Updates.

Ablauf:

```text
Service Implementation A
          │
          ▼
      Quiescing
          │
          ▼
     State Transfer
          │
          ▼
Service Implementation B
          │
          ▼
    Handle Rebinding
          │
          ▼
   Service Available
```

Clients müssen nicht zwingend neu gestartet werden.

---

# 94. Zustandsmigration

Bei einem Service-Update kann interner Zustand migriert werden.

Die Migration muss definieren:

* Quellversion
* Zielversion
* Zustandsformat
* Rollback
* Fehlerbehandlung
* Handlebeziehungen
* offene Operationen
* Ereignisreihenfolge

---

# 95. Recovery

Fällt ein Service aus, kann NovaOS:

* den Service neu initialisieren,
* einen Ersatzservice aktivieren,
* Handles neu binden,
* laufende Operationen abbrechen,
* Clients benachrichtigen,
* einen degradierten Modus aktivieren.

---

# 96. Fehlerisolation

Versionierte Servicegrenzen dienen als Fehlergrenzen.

Insbesondere bei Userspace-Services und Treibern können:

* Zeitüberschreitungen erkannt,
* ungültige Antworten verworfen,
* Dienste neu gestartet,
* Handles widerrufen,
* Ersatzimplementierungen aktiviert

werden.

---

# 97. Service-Health

Services veröffentlichen einen Gesundheitszustand:

* Healthy
* Degraded
* Recovering
* Updating
* Failed
* Unavailable

Der Zustand ist über Diagnose- und Ereignisschnittstellen verfügbar.

---

# 98. Sicherheitsmodell

Versionierte Services dürfen Sicherheitsprüfungen nicht an Clients delegieren.

Der Kernel oder autoritative Service prüft immer:

* Capability
* Objektzugriff
* Namensraum
* Parameter
* Ressourcenlimits
* Sicherheitskontext
* Serviceversion

---

# 99. Parameterprüfung

Alle Eingaben aus weniger privilegierten Kontexten werden validiert.

Prüfungen umfassen:

* Strukturgröße
* Strukturversion
* Flags
* reservierte Felder
* Handles
* Längen
* Offsets
* Wertebereiche
* Überläufe
* Speicherzugriffe

---

# 100. ABI-Fuzzing

Alle öffentlichen Services müssen automatisiert gefuzzt werden.

Fuzzing umfasst:

* ungültige Strukturen
* unbekannte Versionen
* übergroße Puffer
* ungültige Handles
* zufällige Flags
* Race Conditions
* Abbrüche
* Zeitüberschreitungen
* parallele Aufrufe

---

# 101. Formale Spezifikation

Kritische Services sollen formal oder maschinenlesbar spezifiziert werden.

Priorität besitzen:

* Object Service
* Capability Service
* Process Service
* Memory Service
* Event Service
* IPC Service
* Security Service

---

# 102. ABI-Snapshot

Jeder stabile Kernel-Build erzeugt einen ABI-Snapshot.

Der Snapshot enthält:

* Service-IDs
* Versionen
* Operations-IDs
* Strukturgrößen
* Feldpositionen
* Fehlercodes
* Feature-Flags
* Capability-Rechte
* Ereignistypen

---

# 103. ABI-Diff

Build- und CI-Systeme vergleichen den aktuellen ABI-Snapshot mit früheren Versionen.

Erkannt werden:

* entfernte Operationen
* geänderte IDs
* geänderte Feldpositionen
* geänderte Strukturgrößen
* inkompatible Typänderungen
* entfernte Fehlercodes
* geänderte Capability-Anforderungen

---

# 104. Automatische Blockierung

Ein unbeabsichtigter ABI-Bruch in einem stabilen Service blockiert den Build oder die Freigabe.

Eine bewusste inkompatible Änderung benötigt:

* neue Major-Version
* Architekturfreigabe
* Migrationsplan
* Dokumentationsänderung
* Kompatibilitätstests

---

# 105. Testmatrix

Die ABI-Testmatrix umfasst:

* alte Anwendung gegen neuen Kernel
* neue Anwendung gegen unterstützten alten Kernel
* alter Treiber gegen neuen Kernel
* neue Laufzeit gegen alte Service-Version
* 32-Bit gegen 64-Bit-Gateway
* verschiedene Compiler
* verschiedene Optimierungsstufen
* verschiedene CPU-Architekturen

---

# 106. Golden Tests

Für stabile Services werden Golden Tests gepflegt.

Diese enthalten bekannte:

* Eingabestrukturen
* Binärlayouts
* Rückgabewerte
* Fehlercodes
* Ereignisse
* Feature-Aushandlungen
* Capability-Prüfungen

---

# 107. Kompatibilitätsdatenbank

NovaOS kann lokal eine Kompatibilitätsdatenbank führen.

Diese enthält:

* installierte Anwendungen
* verwendete Service-Versionen
* veraltete Interfaces
* bekannte Inkompatibilitäten
* verfügbare Adapter
* erforderliche Migrationen

---

# 108. Entwicklerwerkzeuge

Entwicklerwerkzeuge umfassen:

* Service Registry Explorer
* ABI Inspector
* Interface Diff
* Service Call Tracer
* Capability Inspector
* Compatibility Analyzer
* Deprecation Scanner
* Mock Service Generator

---

# 109. Dokumentationsanforderungen

Jeder öffentliche Service muss dokumentieren:

* Service-ID
* Name
* Zweck
* Stabilitätsklasse
* unterstützte Versionen
* Interfaces
* Operationen
* Datenstrukturen
* Fehlercodes
* Capability-Anforderungen
* Ereignisse
* Feature-Flags
* Deprecation-Status
* Threading-Modell
* Datenschutzklasse

---

# 110. Performance

Die Servicearchitektur darf keine unnötigen Kosten verursachen.

Optimierungen umfassen:

* gecachte Service Handles
* direkte Version Dispatch Tables
* Per-CPU-Service-Caches
* spezialisierte System Calls
* Shared-Memory-Transport
* Batch-Operationen
* statische Capability-Masken
* vordefinierte Fast Paths

---

# 111. Speicherbedarf

Der Speicheraufwand wird begrenzt durch:

* gemeinsame immutable Service Descriptors
* kompakte Dispatch Tables
* geteilte Versionstabellen
* optionale Diagnosemetadaten
* bedarfsgesteuerte Compatibility Adapter
* Referenzzählung

---

# 112. SMP-Unterstützung

Die Service Registry und alle öffentlichen Servicepfade müssen SMP-fähig sein.

Unterstützt werden:

* parallele Serviceaufrufe
* lock-freie Lookups
* Read-Copy-Update
* atomare Rebinding-Vorgänge
* Per-CPU-Caches
* versionierte Registry-Snapshots

---

# 113. NUMA-Unterstützung

Service-Instanzen können NUMA-lokal bereitgestellt werden.

Beispiele:

* Memory Service pro NUMA-Knoten
* Device Service nahe am Gerät
* Scheduler Service pro CPU-Domain
* lokale Event Queues

Die Service-ABI bleibt unabhängig von der Platzierung.

---

# 114. Verworfene Alternative: stabile interne Kernelstrukturen

Vorteile:

* einfache direkte Zugriffe
* geringer Dispatch-Overhead

Nachteile:

* interne Weiterentwicklung blockiert
* hohe Sicherheitsrisiken
* starke Kopplung
* keine kontrollierte Versionierung
* schwerer Live-Update

---

# 115. Verworfene Alternative: unversionierte System Calls

Vorteile:

* einfache Implementierung
* kleine Schnittstelle

Nachteile:

* unklare Kompatibilität
* schwer erweiterbar
* hohe Gefahr stiller Semantikänderungen
* keine Feature-Aushandlung

---

# 116. Verworfene Alternative: nur symbolbasierte Kernel-Exports

Vorteile:

* bekannte Modultechnik
* einfache dynamische Verknüpfung

Nachteile:

* Bindung an Symbolnamen
* Compilerabhängigkeit
* instabile Datentypen
* keine Capability-Semantik
* keine sichere Userspace-Nutzung

---

# 117. Verworfene Alternative: vollständig statische ABI

Vorteile:

* sehr einfache Kompatibilitätsprüfung
* geringe Laufzeitkomplexität

Nachteile:

* kaum erweiterbar
* keine parallelen Versionen
* kein flexibles Feature-Modell
* keine Live-Migration

---

# 118. Verworfene Alternative: automatische Laufzeitübersetzung aller Versionen

Vorteile:

* theoretisch hohe Kompatibilität

Nachteile:

* unkontrollierbare Komplexität
* schwer testbar
* hohe Laufzeitkosten
* unklare Sicherheitssemantik
* fehleranfällige Übersetzungen

NovaOS verwendet nur explizit implementierte und getestete Adapter.

---

# 119. Risiken

Wesentliche Risiken sind:

* zu viele parallel unterstützte Versionen
* wachsender Adapteraufwand
* veraltete Services
* inkonsistente Feature-Flags
* zu frühe Stabilisierung ungeeigneter APIs
* erhöhte Testkomplexität
* Laufzeitkosten durch Dispatch
* Sicherheitsprobleme in alten Interfaces

---

# 120. Gegenmaßnahmen

Die Risiken werden begrenzt durch:

* klare Stabilitätsklassen
* begrenzte Supportzeiträume
* automatische ABI-Diffs
* strukturierte Deprecation
* Compatibility Adapter nur bei Bedarf
* zentrale Service Registry
* maschinenlesbare Spezifikationen
* Sicherheitsreviews
* Performancebenchmarks
* kontrollierte Major-Versionen

---

# 121. Implementierungsrichtlinien

Alle öffentlichen Kernel-Schnittstellen müssen:

* als versionierter Service registriert sein,
* stabile IDs besitzen,
* opake Handles verwenden,
* exportierte Strukturen versionieren,
* Capability-Prüfungen durchführen,
* Fehlercodes dokumentieren,
* Feature-Aushandlung unterstützen,
* Diagnoseinformationen bereitstellen,
* automatisierte ABI-Tests besitzen.

---

# 122. Verbotene Muster

Nicht zulässig sind:

* öffentliche Kernelpointer
* direkte Zugriffe auf private Kernelstrukturen
* wiederverwendete Operations-IDs
* unversionierte exportierte Strukturen
* stille inkompatible Semantikänderungen
* nicht dokumentierte Feature-Flags
* Capability-Umgehung durch Fast Paths
* compilerabhängige öffentliche Klassenlayouts
* unkontrollierte symbolbasierte Treiberbindung
* Entfernung stabiler Operationen ohne Deprecation

---

# 123. Implementierungsphasen

## Phase 1

* Kernel Service Registry
* Service-IDs
* Major- und Minor-Versionierung
* generischer Service Call
* Service Handles
* Capability-Prüfung
* Object-, Process- und Thread-Service

## Phase 2

* Memory Service
* Event Service
* IPC Service
* versionierte Datenstrukturen
* Feature-Aushandlung
* ABI-Snapshot
* automatische ABI-Diffs

## Phase 3

* Filesystem Service
* Device Service
* Security Service
* Treiber-Service-ABI
* Compatibility Adapter
* Deprecation-Werkzeuge

## Phase 4

* Userspace-Service-Broker
* Live-Rebinding
* Live-Update
* Service Description Language
* automatische Sprachbindungen
* Cross-ABI Gateway

---

# 124. Teststrategie

Die Service-ABI wird geprüft durch:

* Unit-Tests
* ABI-Layout-Tests
* Versionsaushandlungstests
* Capability-Tests
* Fehlercode-Tests
* Compatibility-Tests
* Fuzzing
* SMP-Tests
* NUMA-Tests
* Live-Update-Tests
* Recovery-Tests
* Langzeittests
* Performancebenchmarks

---

# 125. Unit-Tests

Unit-Tests prüfen:

* Service-Registrierung
* Service-Lookup
* Versionsauswahl
* Operations-Dispatch
* ungültige Operations-IDs
* Feature-Flags
* Handle-Lebenszyklus
* Capability-Rechte
* Strukturgrößen
* Fehlerabbildung

---

# 126. Integrationstests

Integrationstests umfassen:

* Anwendung gegen mehrere Kernelversionen
* Treiber gegen mehrere Service-Versionen
* Service-Rebinding
* Userspace-Service-Neustart
* Capability-Widerruf
* Kompatibilitätsmodus
* 32-Bit-zu-64-Bit-Übersetzung
* Live-Update

---

# 127. Sicherheitstests

Geprüft werden:

* gefälschte Service Handles
* ungültige Versionen
* manipulierte Strukturgrößen
* Integerüberläufe
* unbekannte Flags
* Capability-Eskalation
* Service-Spoofing
* Race Conditions beim Rebinding
* Use-after-free
* veraltete Compatibility Adapter

---

# 128. Performanceprüfungen

Gemessen werden:

* Service-Lookup-Zeit
* Dispatch-Latenz
* Fast-Path-Latenz
* Capability-Check-Kosten
* IPC-Kosten
* Batch-Durchsatz
* Cache-Miss-Rate
* parallele Skalierung
* Speicheraufwand pro Service Handle

---

# 129. Akzeptanzkriterien

Die ADR gilt als umgesetzt, wenn:

* alle öffentlichen Kerneloperationen versionierten Services zugeordnet sind,
* Service-IDs und Operations-IDs stabil verwaltet werden,
* Clients Versionen und Features aushandeln können,
* exportierte Strukturen Größen- und Versionsfelder besitzen,
* Kernelpointer nicht Teil der ABI sind,
* mehrere Service-Versionen parallel unterstützt werden können,
* ABI-Snapshots automatisch erzeugt und verglichen werden,
* Capability-Prüfungen vor jedem Serviceaufruf stattfinden,
* alte Clients gegen neuere Kernel getestet werden,
* interne Kernelimplementierungen ohne ABI-Bruch austauschbar sind.

---

# 130. Konsequenzen für bestehende Kernelkomponenten

Bestehende Kernelkomponenten werden in folgender Reihenfolge auf Services umgestellt:

1. Kernel Object Manager
2. Handle und Capability Manager
3. Process Manager
4. Thread Manager
5. Memory Manager
6. Event Core
7. IPC
8. Device Manager
9. Filesystem
10. Security Manager
11. Diagnostic Manager
12. Power Manager

---

# 131. Konsequenzen für Userspace

Userspace-Anwendungen erhalten:

* stabile Service Handles
* explizite Versionsaushandlung
* klare Feature-Erkennung
* einheitliche Fehlersemantik
* stabile Sprachbindungen
* kontrollierte Deprecation
* frühzeitige Kompatibilitätsprüfung

---

# 132. Konsequenzen für Treiber

Treiber dürfen ausschließlich dokumentierte Kernel-Services verwenden.

Dadurch können:

* interne Kernelstrukturen geändert,
* Treiber isoliert,
* Userspace-Treiber unterstützt,
* Treiberservices neu gebunden,
* Treiber über Kernelversionen hinweg kompatibel gehalten

werden.

---

# 133. Konsequenzen für NovaLang

NovaLang kann Service-Versionen und Capabilities in das Typsystem integrieren.

Mögliche Sprachunterstützung:

```text
service MemoryService requires version >= 3.0

let memory = system.service<MemoryService>()

let region = memory.allocate(
    size: 4096,
    rights: MemoryRights.Read | MemoryRights.Write
)
```

Der Compiler kann fehlende Versionen oder Capabilities bereits vor dem Aufruf erkennen.

---

# 134. Konsequenzen für Kompatibilitätsschichten

POSIX-, Linux-, Windows- und Android-Kompatibilität werden vollständig oberhalb der nativen Service-ABI umgesetzt.

Dadurch bleiben:

* native Kernelarchitektur
* Kompatibilitätsschichten
* Fremd-ABIs
* interne Implementierung

klar voneinander getrennt.

---

# 135. Dokumentationsstruktur

Für jeden Kernel-Service wird mindestens folgendes Dokument erstellt:

```text
NPSPEC-KERNEL-SERVICE-<NAME>-0001
```

Es beschreibt:

* Service-ID
* Version
* Operations-IDs
* Datentypen
* Fehler
* Capabilities
* Ereignisse
* Threading
* Kompatibilität
* Sicherheitsmodell
* Testanforderungen

---

# 136. Zukunftsperspektive

Langfristig soll die Service-ABI erweitert werden um:

* formal verifizierte Serviceverträge
* automatisch generierte Bindings
* deklarative Capability-Definitionen
* verteilte Service Proxies
* clusterweite Services
* sichere Remote-Service-Handles
* automatische Kompatibilitätsadapter
* deterministisches Service-Replay
* KI-gestützte ABI-Migrationsanalyse
* hardwaregestützten Service Dispatch
* transaktionale Live-Service-Migration

---

# 137. Konsequenz

Versionierte Kernel-Services werden zur einzigen stabilen nativen ABI zwischen dem NovaOS-Kernel und externen Komponenten.

Interne Kernelstrukturen, Funktionsadressen und Implementierungsdetails sind ausdrücklich nicht Teil der Kompatibilitätsgarantie.

Die Stabilitätsgarantie bezieht sich auf:

* dokumentierte Services
* Service-Versionen
* Operations-IDs
* Datenstrukturen
* Fehlersemantik
* Feature-Flags
* Capability-Anforderungen
* Ereignisse

---

# 138. Zusammenfassung

NovaOS gewährleistet **ABI-Stabilität über versionierte Kernel-Services**.

Anwendungen, Systemdienste, Treiber, Laufzeitumgebungen und Kompatibilitätsschichten kommunizieren mit dem Kernel über stabile Service-IDs, versionierte Interfaces, feste Operations-IDs, opake Handles und explizite Capability-Prüfungen.

Die Architektur unterstützt:

* Major-, Minor- und Patch-Versionierung
* parallele Service-Versionen
* Feature-Aushandlung
* versionierte Datenstrukturen
* kontrollierte Deprecation
* Compatibility Adapter
* ABI-Snapshots
* automatische ABI-Diffs
* Treiber- und Userspace-Services
* Live-Update und Service-Rebinding
* architekturübergreifende Semantik

Damit kann NovaOS seine interne Kernelarchitektur langfristig weiterentwickeln, ohne bestehende Anwendungen und Treiber unnötig an konkrete Implementierungsdetails zu binden.

Die versionierten Kernel-Services bilden gemeinsam mit dem Kernel Object Graph, dem einheitlichen Objektmodell, dem ereignisgesteuerten Kernel-Core und der Capability-basierten Ressourcenverwaltung die stabile Systemgrenze von NovaOS.
