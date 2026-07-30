# ADR-KERNEL-0101: Ereignisgesteuerter Kernel-Core

* **ADR-ID:** ADR-KERNEL-0101
* **Titel:** Ereignisgesteuerter Kernel-Core
* **Status:** Akzeptiert
* **Version:** 1.0.0
* **Datum:** 2026-07-29
* **Autor:** NovaOS Architekturgruppe
* **Kategorie:** Kernel Architecture
* **Gültigkeitsbereich:** Kernel Core, Kernel Object Graph, Scheduler, Interrupt Manager, Exception Manager, Device Manager, IPC, Memory Manager, Filesystem, Security Manager
* **Ersetzt:** Direkte, synchrone und subsystemgebundene Kernelkopplung
* **Ersetzt durch:** –
* **Abhängigkeiten:** ADR-KERNEL-0014, ADR-KERNEL-0015, ADR-KERNEL-0016, ADR-KERNEL-0017, ADR-KERNEL-0100
* **Verwandte Dokumente:** ADR-KERNEL-0011, ADR-KERNEL-0012, ADR-KERNEL-0013

---

# 1. Zusammenfassung

NovaOS implementiert einen **ereignisgesteuerten Kernel-Core**.

Kernelkomponenten kommunizieren nicht primär über direkte, starre Funktionsaufrufe zwischen Subsystemen, sondern über typisierte Ereignisse, Ereigniskanäle und definierte Zustandsänderungen innerhalb des Kernel Object Graph.

Direkte Funktionsaufrufe bleiben für lokale, deterministische und zeitkritische Operationen zulässig. Subsystemübergreifende Zustandsänderungen werden jedoch grundsätzlich als Kernelereignisse modelliert.

Dadurch entsteht ein Kernel, der lose gekoppelt, beobachtbar, erweiterbar und für asynchrone Hardware- und Systemabläufe geeignet ist.

---

# 2. Kontext

Ein Betriebssystem verarbeitet fortlaufend Ereignisse.

Beispiele:

* Interrupts
* Exceptions
* Timerabläufe
* Threadzustandsänderungen
* Prozessbeendigungen
* Speicherknappheit
* Geräte-Hotplug
* Netzwerkpakete
* Dateisystemänderungen
* Energiezustandswechsel
* Sicherheitsverletzungen

In klassischen Kernelarchitekturen werden solche Zustandsänderungen häufig über direkte Aufrufketten zwischen mehreren Subsystemen weitergegeben.

Dies führt mit wachsender Komplexität zu:

* starker Kopplung
* schwer nachvollziehbaren Kontrollflüssen
* rekursiven Aufrufketten
* Abhängigkeiten zwischen Subsystemen
* eingeschränkter Erweiterbarkeit
* schwer reproduzierbaren Race Conditions

---

# 3. Problemstellung

Es musste entschieden werden,

* wie Kernelkomponenten Zustandsänderungen kommunizieren,
* wie synchrone und asynchrone Abläufe getrennt werden,
* wie Ereignisse priorisiert werden,
* wie Echtzeitanforderungen berücksichtigt werden,
* wie Ereignisverluste verhindert werden,
* wie Ereignisse beobachtet und protokolliert werden,
* und wie der Kernel Object Graph mit dem Ereignismodell zusammenarbeitet.

---

# 4. Entscheidung

NovaOS verwendet einen zentralen, ereignisgesteuerten Kernel-Core.

Dabei gelten folgende Grundsätze:

* Jede relevante Zustandsänderung wird als typisiertes Kernelereignis modelliert.
* Ereignisse besitzen eine definierte Quelle, ein Ziel, einen Typ und einen Kontext.
* Ereignisse werden über registrierte Ereigniskanäle verteilt.
* Ereignisverarbeitung kann synchron, verzögert oder asynchron erfolgen.
* Echtzeitkritische Pfade bleiben begrenzt und deterministisch.
* Der Kernel Object Graph bildet Quelle, Ziel und Beziehungen eines Ereignisses ab.
* Direkte subsystemübergreifende Seiteneffekte ohne Ereignismodell sind nicht zulässig.

---

# 5. Motivation

Diese Architektur bietet:

* geringere Kopplung
* klare Zustandsübergänge
* bessere Erweiterbarkeit
* vollständige Beobachtbarkeit
* einfachere Fehlersuche
* bessere Parallelisierbarkeit
* einheitliche Ereignisverarbeitung
* bessere Unterstützung dynamischer Hardware
* robuste Grundlage für Self-Healing

---

# 6. Designprinzipien

Der ereignisgesteuerte Kernel-Core basiert auf:

* expliziten Ereignissen
* typisierten Nachrichten
* deterministischen Zustandsübergängen
* loser Kopplung
* begrenzten Seiteneffekten
* priorisierter Verarbeitung
* nachvollziehbarer Ereigniskausalität
* kontrollierter Asynchronität
* Capability-basierter Autorisierung
* Rückstaukontrolle

---

# 7. Architekturübersicht

```text
                    Hardware
                       │
                       ▼
             Interrupt / Exception
                       │
                       ▼
            Low-Level Event Adapter
                       │
                       ▼
          ┌───────────────────────────┐
          │   Kernel Event Core       │
          │                           │
          │  - Event Router           │
          │  - Priority Queues        │
          │  - Event Registry         │
          │  - Delivery Engine        │
          │  - Trace Context          │
          └─────────────┬─────────────┘
                        │
          ┌─────────────┼─────────────┐
          ▼             ▼             ▼
      Scheduler      Memory       Device Manager
          │             │             │
          ▼             ▼             ▼
       Threads       VMR/Pages      Driver/Device
          └─────────────┼─────────────┘
                        ▼
              Kernel Object Graph
```

---

# 8. Kernel Event Core

Der Kernel Event Core ist die zentrale Infrastruktur für Kernelereignisse.

Er besteht mindestens aus:

* Event Registry
* Event Router
* Event Dispatcher
* Priority Queue Manager
* Subscription Manager
* Deferred Work Manager
* Trace Context Manager
* Backpressure Manager
* Event Statistics Manager

---

# 9. Ereignisdefinition

Jedes Kernelereignis besitzt mindestens:

* Ereignis-ID
* Ereignistyp
* Version
* Priorität
* Quelle
* optionales Ziel
* Zeitstempel
* CPU-ID
* Thread-ID
* Prozess-ID
* Sicherheitskontext
* Payload
* Korrelations-ID
* Kausalitäts-ID
* Zustellungsmodus
* Fehlerstatus

---

# 10. Ereignistypen

NovaOS definiert mehrere Ereignisklassen:

* Hardware Event
* Interrupt Event
* Exception Event
* Scheduler Event
* Process Event
* Thread Event
* Memory Event
* Device Event
* Filesystem Event
* Network Event
* Security Event
* Power Event
* Diagnostic Event
* Recovery Event
* Lifecycle Event

---

# 11. Ereignisquellen

Ereignisse können erzeugt werden durch:

* CPU
* Interrupt Controller
* Timer
* Kernelobjekte
* Gerätetreiber
* Scheduler
* Memory Manager
* Security Manager
* IPC Manager
* Filesystem
* Netzwerkstack
* Systemdienste
* Benutzerprozesse über kontrollierte Kernel-APIs

---

# 12. Ereignisziele

Ein Ereignis kann gerichtet sein an:

* ein einzelnes Kernelobjekt
* mehrere Kernelobjekte
* einen Objekttyp
* einen Ereigniskanal
* eine CPU
* einen Thread
* einen Prozess
* eine Kernelkomponente
* alle registrierten Beobachter

Broadcast-Ereignisse sind nur zulässig, wenn eine gezielte Zustellung nicht sinnvoll möglich ist.

---

# 13. Zustellungsmodi

NovaOS unterstützt folgende Zustellungsmodi:

* Immediate
* Synchronous
* Deferred
* Asynchronous
* Batched
* Coalesced
* Broadcast
* Reliable
* Best Effort

Der Zustellungsmodus ist Bestandteil der Ereignisdefinition.

---

# 14. Immediate Events

Immediate Events werden im aktuellen Ausführungskontext verarbeitet.

Sie sind ausschließlich zulässig für:

* sehr kurze Operationen
* nicht blockierende Verarbeitung
* deterministische Laufzeit
* lokal begrenzte Zustandsänderungen
* architekturspezifische Low-Level-Pfade

Immediate Events dürfen:

* keine langen Sperren halten,
* keine blockierende I/O ausführen,
* keine Speicherallokation mit unbestimmter Laufzeit benötigen,
* keine rekursive Ereigniskaskade verursachen.

---

# 15. Synchrone Ereignisse

Synchrone Ereignisse werden vollständig verarbeitet, bevor der Erzeuger fortgesetzt wird.

Sie eignen sich für:

* Validierungen
* Sicherheitsprüfungen
* Lebenszyklusübergänge
* Ressourcenzuweisungen
* deterministische Kerneltransaktionen

Synchrone Ereignisse müssen eine klar definierte maximale Verarbeitungstiefe besitzen.

---

# 16. Verzögerte Ereignisse

Deferred Events werden außerhalb des zeitkritischen Ursprungskontexts verarbeitet.

Typische Beispiele:

* Interrupt-Nachbearbeitung
* Gerätebenachrichtigungen
* Page-Cache-Bereinigung
* Logausgabe
* Statistikerfassung
* Treiber-Recovery
* Dateisystemarbeit

---

# 17. Asynchrone Ereignisse

Asynchrone Ereignisse werden über Kernelwarteschlangen verarbeitet.

Der Erzeuger wartet nicht auf den Abschluss.

Asynchrone Ereignisse benötigen:

* definierte Besitzverhältnisse
* sichere Payload-Lebensdauer
* Fehlerkanal
* Abschlussereignis oder Completion Object
* Abbruchmechanismus
* Timeout-Unterstützung

---

# 18. Ereignisprioritäten

NovaOS definiert mindestens folgende Prioritätsklassen:

* Critical
* Realtime
* High
* Normal
* Low
* Background
* Maintenance

Priorität beeinflusst:

* Warteschlangenposition
* CPU-Zuordnung
* Ausführungsbudget
* Preemption
* Coalescing
* Verwerfungsrichtlinie

---

# 19. Ereigniskanäle

Ereigniskanäle bündeln logisch verwandte Ereignisse.

Beispiele:

* CPU Channel
* Scheduler Channel
* Memory Channel
* Device Channel
* Filesystem Channel
* Security Channel
* Power Channel
* Diagnostic Channel

Ein Kanal besitzt:

* eindeutige Kanal-ID
* Ereignistypfilter
* Prioritätsrichtlinie
* Kapazität
* Backpressure-Verhalten
* Sicherheitsrichtlinie
* Beobachterliste

---

# 20. Ereignisabonnements

Kernelobjekte können Ereignisse abonnieren.

Ein Abonnement definiert:

* abonnierende Objekt-ID
* Ereignistyp
* Quellfilter
* Zielfilter
* Prioritätsschwelle
* Zustellungsmodus
* Callback oder Message Endpoint
* Capability-Anforderung
* Lebensdauer

Abonnements werden beim Zerstören des zugehörigen Objekts automatisch entfernt.

---

# 21. Integration mit dem Kernel Object Graph

Der Kernel Object Graph bildet die strukturelle Grundlage der Ereigniszustellung.

Ein Ereignis kann entlang definierter Beziehungen propagiert werden.

Beispiele:

```text
Device
  │
  ├── owned-by ──> Driver
  │
  ├── attached-to ──> Bus
  │
  └── used-by ──> Process
```

Ein `DEVICE_REMOVED`-Ereignis kann dadurch kontrolliert an:

* den Treiber,
* den Bus,
* betroffene Prozesse,
* offene Handles,
* abhängige DMA-Objekte

weitergegeben werden.

---

# 22. Ereignispropagation

Ereignisse dürfen nicht unkontrolliert den gesamten Objektgraphen durchlaufen.

Jede Propagation benötigt:

* zugelassene Beziehungstypen
* maximale Tiefe
* Zyklenschutz
* Capability-Prüfung
* Prioritätsvererbung
* Abbruchbedingung
* Kausalitätskontext

---

# 23. Ereigniskausalität

Jedes abgeleitete Ereignis enthält eine Kausalitätsreferenz.

Beispiel:

```text
DEVICE_INTERRUPT
        │
        ▼
DMA_TRANSFER_COMPLETE
        │
        ▼
IO_REQUEST_COMPLETE
        │
        ▼
THREAD_WAKE
```

Dadurch kann NovaOS vollständige Ursache-Wirkungs-Ketten rekonstruieren.

---

# 24. Korrelations-IDs

Zusammengehörige Ereignisse erhalten eine gemeinsame Korrelations-ID.

Dies ermöglicht:

* Transaktionsverfolgung
* I/O-Nachverfolgung
* Fehleranalyse
* Performanceanalyse
* verteilte Kernelabläufe
* Recovery-Zuordnung

---

# 25. Interruptintegration

Hardwareinterrupts werden zunächst durch einen minimalen Interrupt-Handler verarbeitet.

Der Ablauf lautet:

```text
Hardware Interrupt
        │
        ▼
Interrupt Entry
        │
        ▼
Minimal Acknowledge
        │
        ▼
Kernel Event erzeugen
        │
        ▼
Deferred Event Processing
```

Der unmittelbare Interruptkontext führt nur zeitkritische und hardwareabhängige Minimaloperationen aus.

---

# 26. Exceptionintegration

CPU-Exceptions werden in typisierte Exception Events übersetzt.

Beispiele:

* Page Fault
* General Protection Fault
* Invalid Opcode
* Divide Error
* Alignment Fault
* Machine Check

Ein Exception Event enthält:

* CPU-Kontext
* Fault-Adresse
* Fehlercode
* Threadreferenz
* Prozessreferenz
* Recovery-Klassifikation

---

# 27. Schedulerintegration

Der Scheduler erzeugt und verarbeitet Ereignisse wie:

* Thread Ready
* Thread Blocked
* Thread Wake
* Thread Yield
* Priority Changed
* CPU Offline
* CPU Online
* Deadline Missed
* Load Balance Required

Der Scheduler darf zeitkritische lokale Entscheidungen direkt ausführen. Systemübergreifende Zustandsänderungen werden jedoch als Ereignisse veröffentlicht.

---

# 28. Prozess- und Threadereignisse

Definierte Ereignisse umfassen:

* Process Created
* Process Started
* Process Suspended
* Process Resumed
* Process Exiting
* Process Exited
* Thread Created
* Thread Started
* Thread Blocked
* Thread Woken
* Thread Terminated

Diese Ereignisse ermöglichen eine konsistente Integration von:

* Scheduler
* Security
* Debugger
* Ressourcenverwaltung
* Telemetrie
* Recovery

---

# 29. Speicherereignisse

Der Memory Manager erzeugt unter anderem:

* Memory Pressure Changed
* Page Allocated
* Page Released
* Page Fault
* Mapping Created
* Mapping Removed
* NUMA Migration Started
* NUMA Migration Completed
* Memory Hotplug Added
* Memory Hotplug Removed
* Reclaim Started
* Reclaim Completed
* OOM Imminent

Nicht jedes interne Seitendetail muss global veröffentlicht werden. Ereignisse werden nach Relevanz und Beobachtungsbedarf klassifiziert.

---

# 30. Geräteereignisse

Der Device Manager erzeugt:

* Device Detected
* Device Initialized
* Device Ready
* Device Suspended
* Device Resumed
* Device Failed
* Device Removed
* Driver Bound
* Driver Unbound
* Firmware Requested
* Power State Changed

---

# 31. Dateisystemereignisse

Das Dateisystem erzeugt:

* Volume Mounted
* Volume Unmounted
* File Created
* File Modified
* File Deleted
* Metadata Changed
* Snapshot Created
* Integrity Failure
* Storage Degraded
* Recovery Required

Ereignisse auf Dateiebene können durch Filter, Aggregation und Coalescing begrenzt werden.

---

# 32. Sicherheitsereignisse

Der Security Manager erzeugt:

* Capability Granted
* Capability Revoked
* Access Denied
* Policy Violation
* Integrity Failure
* Authentication State Changed
* Protected Object Modified
* Suspicious Device Access
* DMA Violation

Sicherheitsereignisse besitzen grundsätzlich mindestens die Priorität `High`.

---

# 33. Power-Management-Ereignisse

Das Power Management verwendet Ereignisse für:

* System Suspend
* System Resume
* Device Suspend
* Device Resume
* Thermal Limit
* Battery State
* CPU Frequency Change
* Power Domain Transition
* Emergency Shutdown

---

# 34. Ereigniswarteschlangen

NovaOS verwendet mehrere spezialisierte Warteschlangen.

Mindestens vorgesehen sind:

* Per-CPU Critical Queue
* Per-CPU Realtime Queue
* Per-CPU Normal Queue
* Global Deferred Queue
* Background Queue
* Maintenance Queue
* Recovery Queue

Eine einzige globale Warteschlange wird vermieden.

---

# 35. Per-CPU-Verarbeitung

Zur Reduktion von Lock-Contention werden Ereignisse möglichst CPU-lokal verarbeitet.

Per-CPU-Queues bieten:

* bessere Cache-Lokalität
* geringere Synchronisationskosten
* kürzere Latenzen
* bessere SMP-Skalierung

CPU-übergreifende Zustellung erfolgt über:

* Inter-Processor Interrupts
* lock-freie Übergabestrukturen
* Work Stealing
* explizite CPU-Migration

---

# 36. Work Stealing

Nicht zeitkritische Ereigniswarteschlangen dürfen Work Stealing verwenden.

Dabei gilt:

* Critical- und Realtime-Ereignisse werden nicht unkontrolliert gestohlen.
* CPU-Affinitäten werden berücksichtigt.
* NUMA-Lokalität bleibt erhalten.
* Sicherheitskontexte dürfen nicht verändert werden.
* Reihenfolgegarantien müssen eingehalten werden.

---

# 37. Reihenfolgegarantien

NovaOS unterstützt mehrere Ordnungsmodelle:

* keine garantierte Reihenfolge
* FIFO pro Quelle
* FIFO pro Ziel
* FIFO pro Kanal
* kausale Reihenfolge
* vollständige Reihenfolge innerhalb einer Transaktion

Die erforderliche Garantie wird pro Ereignistyp definiert.

---

# 38. Ereignis-Coalescing

Häufig auftretende Ereignisse dürfen zusammengeführt werden.

Beispiele:

* Mausbewegungen
* Netzwerkstatusänderungen
* Speicherdruckaktualisierungen
* Temperaturänderungen
* Dateisystemänderungen
* wiederholte Timerereignisse

Coalescing darf keine semantisch relevanten Zustandsänderungen verlieren.

---

# 39. Ereignis-Batching

Mehrere gleichartige Ereignisse können gemeinsam verarbeitet werden.

Vorteile:

* geringere Scheduling-Kosten
* bessere Cache-Nutzung
* reduzierte Lock-Operationen
* höhere I/O-Effizienz

Batchgrößen müssen begrenzt werden, um Latenzspitzen zu vermeiden.

---

# 40. Backpressure

Jeder Ereigniskanal definiert eine Backpressure-Richtlinie.

Mögliche Strategien:

* Erzeuger blockieren
* Erzeuger verlangsamen
* Ereignisse bündeln
* niedrige Prioritäten verwerfen
* älteste Ereignisse ersetzen
* Notfallpuffer verwenden
* Recovery auslösen

Kritische Ereignisse dürfen nicht stillschweigend verworfen werden.

---

# 41. Überlastschutz

Der Kernel Event Core schützt sich vor Ereignisüberlastung durch:

* Queue-Limits
* Rate Limits
* Prioritätsbudgets
* Quellenbegrenzung
* Event Coalescing
* dynamische Worker-Skalierung
* Missbrauchserkennung
* Notfallkanäle

---

# 42. Rekursionsschutz

Ereignishandler dürfen nicht unbegrenzt neue synchrone Ereignisse erzeugen.

Der Kernel verwaltet:

* Ereignistiefe
* maximale Rekursionstiefe
* Kausalitätskette
* Zyklenerkennung
* Handlerbudget

Bei Überschreitung wird das Ereignis verzögert oder kontrolliert abgebrochen.

---

# 43. Deadlock-Vermeidung

Ereignishandler müssen definierte Sperrregeln einhalten.

Insbesondere gilt:

* keine unbestimmte Blockierung in Immediate Handlers
* keine synchrone Rückzustellung an denselben gesperrten Kontext
* keine zyklische Abhängigkeit zwischen Event Channels
* klare Lock-Reihenfolge
* bevorzugte Übergabe an Deferred Work

---

# 44. Ereignis-Payload

Payloads können enthalten:

* Inline-Daten
* Referenzen auf Kernelobjekte
* Handle
* Capability
* Shared Memory Object
* Completion Object
* Fehlerobjekt

Große Payloads werden nicht in Ereignisstrukturen kopiert, sondern referenziert.

---

# 45. Besitzverhältnisse

Für jede Payload muss definiert sein:

* wer sie erzeugt,
* wer sie besitzt,
* wie lange sie gültig ist,
* wer sie freigibt,
* ob sie veränderbar ist,
* ob mehrere Empfänger darauf zugreifen dürfen.

Nicht definierte Besitzverhältnisse sind unzulässig.

---

# 46. Ereignisversionierung

Jeder Ereignistyp besitzt eine Version.

Neue Versionen dürfen:

* Felder ergänzen
* optionale Semantik erweitern
* alte Empfänger weiter unterstützen

Inkompatible Änderungen benötigen einen neuen Ereignistyp oder eine neue Hauptversion.

---

# 47. Event Registry

Alle Ereignistypen werden zentral registriert.

Ein Registry-Eintrag enthält:

* Ereigniskennung
* Name
* Version
* Payload-Schema
* Prioritätsklasse
* Zustellungsmodi
* Reihenfolgegarantie
* Sicherheitsanforderungen
* Debugbeschreibung
* Eigentümerkomponente

---

# 48. Capability-Sicherheit

Das Erzeugen, Abonnieren oder Weiterleiten bestimmter Ereignisse kann Capabilities erfordern.

Beispiele:

* Security Events
* Process Control Events
* Device Power Events
* Memory Hotplug Events
* Kernel Recovery Events

Ein Ereignis darf keine Berechtigungen übertragen, die der Empfänger nicht bereits besitzt.

---

# 49. Vertrauensgrenzen

Ereignisse aus nicht vertrauenswürdigen Quellen werden validiert.

Dazu gehören:

* Benutzerprozesse
* Gerätetreiber
* virtuelle Geräte
* Netzwerkquellen
* externe Firmware
* Gastbetriebssysteme

Validierung umfasst:

* Größe
* Typ
* Version
* Wertebereiche
* Objektberechtigungen
* Referenzgültigkeit
* Rate Limit

---

# 50. Treiberintegration

Treiber kommunizieren mit dem Kernel-Core über:

* Hardware Events
* Device Events
* Completion Events
* Error Events
* Power Events
* Hotplug Events

Treiber dürfen keine internen Kernelkomponenten direkt manipulieren, wenn dafür ein definierter Ereignispfad existiert.

---

# 51. IPC-Integration

Bestimmte Kernelereignisse können kontrolliert in Benutzerraumereignisse übersetzt werden.

Beispiele:

* Prozessbeendigung
* Geräteänderung
* Dateisystembenachrichtigung
* Energiezustand
* Netzwerkstatus

Die Übersetzung erfolgt über dedizierte IPC-Endpunkte und Sicherheitsfilter.

---

# 52. Completion Objects

Asynchrone Kerneloperationen verwenden Completion Objects.

Ein Completion Object enthält:

* Operations-ID
* Status
* Ergebnis
* Fehler
* wartende Threads
* Timeout
* Abbruchstatus

Nach Abschluss wird ein Completion Event erzeugt.

---

# 53. Timerintegration

Timer erzeugen Ereignisse wie:

* Timer Expired
* Periodic Timer Tick
* Deadline Reached
* Timeout
* Watchdog Triggered

Der Tickless Kernel verwendet Ereignisse ausschließlich zu tatsächlich programmierten Zeitpunkten.

---

# 54. Fehlerbehandlung

Jeder Ereignishandler liefert einen definierten Status.

Mögliche Ergebnisse:

* Handled
* Ignored
* Deferred
* Retry
* Rejected
* Failed
* Escalated
* Cancelled

Fehler können eigene Ereignisse erzeugen, müssen jedoch Rekursionsschutz beachten.

---

# 55. Wiederholungsstrategie

Für temporär fehlgeschlagene Ereignisse kann eine Retry Policy definiert werden.

Sie enthält:

* maximale Versuche
* Verzögerung
* Backoff
* Prioritätsanpassung
* Abbruchbedingung
* Eskalationsziel

Endlose Wiederholungen sind unzulässig.

---

# 56. Recovery-Integration

Der Kernel Event Core unterstützt Self-Healing und Recovery.

Beispiele:

* Treiber neu initialisieren
* Gerät isolieren
* Speicherbereich sperren
* Worker neu starten
* Dateisystemprüfung auslösen
* sicheren Systemzustand aktivieren

Recovery-Aktionen werden als eigene, nachvollziehbare Ereignisketten modelliert.

---

# 57. Determinismus

Für Echtzeit- und sicherheitskritische Ereignisse müssen definiert werden:

* maximale Queue-Zeit
* maximale Handlerlaufzeit
* maximale Kettenlänge
* erlaubte Speicheroperationen
* erlaubte Sperren
* CPU-Affinität
* Eskalationsverhalten

---

# 58. Echtzeitunterstützung

Realtime Events erhalten:

* reservierte Queue-Kapazität
* reservierte Worker
* begrenzte Handlerlaufzeit
* Prioritätsvererbung
* CPU-Affinität
* keine unkontrollierte Speicherallokation
* keine Best-Effort-Zustellung

---

# 59. SMP-Unterstützung

Der Event Core ist vollständig SMP-fähig.

Er unterstützt:

* parallele Handler
* Per-CPU-Queues
* atomare Zustandsänderungen
* CPU-übergreifende Zustellung
* lock-freie Datenstrukturen
* Work Stealing
* CPU Hotplug

---

# 60. CPU Hotplug

Beim Offlining einer CPU werden:

* neue Ereignisse umgeleitet
* bestehende Queues geleert
* kritische Ereignisse migriert
* CPU-lokale Abonnements aktualisiert
* laufende Handler abgeschlossen oder abgebrochen

---

# 61. NUMA-Unterstützung

Ereignisverarbeitung berücksichtigt:

* Quellknoten
* Zielknoten
* CPU-Lokalität
* Speicherlokalität
* Gerätezugehörigkeit
* Migrationskosten

NUMA-ferne Zustellung wird vermieden, sofern keine andere Priorität überwiegt.

---

# 62. Speicherverwaltung

Der Event Core verwendet:

* vorreservierte Ereignispools
* Per-CPU-Allocator
* Slab Caches
* Notfallreserven
* begrenzte Payloadgrößen

Zeitkritische Ereignisse dürfen nicht von einer möglicherweise blockierenden Speicherallokation abhängen.

---

# 63. Ereignispersistenz

Die meisten Kernelereignisse sind flüchtig.

Bestimmte Ereignisse können persistent protokolliert werden:

* Kernel Panic
* Machine Check
* Sicherheitsverletzung
* Dateisystemkorruption
* Recovery-Fehler
* wiederholter Treiberabsturz

Persistenz ist Aufgabe des Diagnose- und Recovery-Subsystems, nicht der normalen Ereigniswarteschlange.

---

# 64. Beobachtbarkeit

Der Kernel Event Core stellt Metriken bereit:

* Ereignisse pro Typ
* Ereignisse pro CPU
* Queue-Längen
* Zustellungszeiten
* Handlerlaufzeiten
* Verwerfungen
* Wiederholungen
* Fehler
* Backpressure-Aktivierungen
* maximale Kausalitätstiefe

---

# 65. Tracing

Optional kann für Ereignisse ein Trace Context aktiviert werden.

Dieser enthält:

* Trace-ID
* Span-ID
* Parent-Span-ID
* Zeitstempel
* Quelle
* Ziel
* Queue-Dauer
* Handlerdauer
* Ergebnis

Tracing muss im Produktivbetrieb begrenzbar sein.

---

# 66. Debug-Unterstützung

NovaOS unterstützt:

* Event Inspector
* Live Event Stream
* Event Graph
* Kausalitätsanalyse
* Queue Inspector
* Handler Profiler
* verlorene Ereignisanalyse
* Deadlock-Korrelation
* Replay-Vorbereitung

---

# 67. Ereignisaufzeichnung

Für ausgewählte Ereignistypen kann eine begrenzte Ringpufferaufzeichnung aktiviert werden.

Gespeichert werden:

* Header
* Typ
* Quelle
* Ziel
* Kausalität
* Zeit
* Ergebnis

Sensible Payloads werden nicht ungefiltert aufgezeichnet.

---

# 68. Deterministisches Replay

Die Architektur soll zukünftig partielles Ereignis-Replay ermöglichen.

Dafür werden benötigt:

* stabile Ereignis-IDs
* definierte Reihenfolge
* reproduzierbare Payloads
* kontrollierte Zeitquellen
* externe Eingabeprotokolle
* deterministische Handler

Vollständiges Kernel-Replay ist kein unmittelbares Implementierungsziel.

---

# 69. Datenschutz

Ereignisprotokolle dürfen keine unnötigen personenbezogenen Daten enthalten.

Es gelten:

* Datenminimierung
* Zugriffskontrolle
* begrenzte Aufbewahrung
* lokale Verarbeitung
* Pseudonymisierung
* redaktionelle Filterung sensibler Payloads

---

# 70. Performanceziele

Der Event Core soll erreichen:

* konstante oder amortisiert konstante Queue-Operationen
* minimale Interrupt-Nachbearbeitungslatenz
* geringe Cache-Invalidierung
* lineare Skalierung über mehrere CPUs
* begrenzten Speicherverbrauch
* niedrigen Overhead bei deaktiviertem Tracing

---

# 71. Fast Paths

Nicht jeder Kernelvorgang wird vollständig über generische Ereigniswarteschlangen geleitet.

Zulässige Fast Paths sind:

* lokale Schedulerentscheidung
* einfache Referenzzählung
* atomare Statusabfrage
* TLB-Verwaltung
* Interrupt-Acknowledge
* kurze Page-Table-Operationen

Fast Paths müssen weiterhin definierte Ereignisse für relevante externe Zustandsänderungen veröffentlichen.

---

# 72. Slow Paths

Komplexe, blockierende oder subsystemübergreifende Operationen werden als Slow Paths modelliert.

Beispiele:

* Gerät initialisieren
* Speicher migrieren
* Volume mounten
* Treiber wiederherstellen
* Prozess vollständig beenden
* Sicherheitsrichtlinie neu laden

---

# 73. API-Modell

Die Kern-API umfasst konzeptionell:

```text
event_type_register()
event_type_unregister()

event_channel_create()
event_channel_destroy()

event_subscribe()
event_unsubscribe()

event_emit()
event_emit_sync()
event_emit_deferred()

event_wait()
event_cancel()

event_complete()
event_query()
```

Die konkreten Funktionsnamen werden durch die Kernel-API-Spezifikation festgelegt.

---

# 74. Beispiel: Geräte-Hotplug

```text
Hardware meldet neues Gerät
            │
            ▼
DEVICE_DETECTED
            │
            ▼
Bus Manager untersucht Gerät
            │
            ▼
DRIVER_MATCH_REQUESTED
            │
            ▼
Treiber wird gebunden
            │
            ▼
DEVICE_INITIALIZED
            │
            ▼
DEVICE_READY
```

Jeder Übergang ist einzeln nachvollziehbar.

---

# 75. Beispiel: Page Fault

```text
CPU Page Fault
      │
      ▼
PAGE_FAULT_EVENT
      │
      ├── Guard Page ──> STACK_OVERFLOW_EVENT
      │
      ├── COW Page ────> COW_RESOLUTION_EVENT
      │
      ├── File Mapping ─> PAGE_IN_REQUEST
      │
      └── Invalid ──────> ACCESS_VIOLATION_EVENT
```

---

# 76. Beispiel: Prozessbeendigung

```text
PROCESS_EXIT_REQUEST
        │
        ▼
Threads stoppen
        │
        ▼
Handles schließen
        │
        ▼
Mappings entfernen
        │
        ▼
Ressourcen freigeben
        │
        ▼
PROCESS_EXITED
```

---

# 77. Beispiel: Memory Pressure

```text
Freier Speicher sinkt
        │
        ▼
MEMORY_PRESSURE_CHANGED
        │
        ▼
Reclaim Worker
        │
        ├── Page Cache bereinigen
        ├── Seiten komprimieren
        ├── Seiten auslagern
        └── OOM vermeiden
```

---

# 78. Erweiterbarkeit

Neue Kernelkomponenten integrieren sich durch:

* Registrierung neuer Ereignistypen
* Abonnement bestehender Ereignisse
* Veröffentlichung eigener Zustandsänderungen
* Definition neuer Objektbeziehungen
* Bereitstellung eigener Handler

Bestehende Komponenten müssen dafür nicht direkt verändert werden.

---

# 79. Modularität

Der Event Core unterstützt optionale Kernelmodule.

Module können:

* Ereignistypen registrieren
* Handler bereitstellen
* Kanäle abonnieren
* beim Entladen alle Abonnements entfernen
* offene Ereignisse kontrolliert abschließen

---

# 80. Auswirkungen

Positive Auswirkungen:

* geringere direkte Kopplung
* klarere Kontrollflüsse
* bessere Beobachtbarkeit
* bessere Erweiterbarkeit
* höhere Parallelisierbarkeit
* konsistente Fehlerbehandlung
* robuste Hotplug-Unterstützung
* Grundlage für Self-Healing

Negative Auswirkungen:

* zusätzlicher Verwaltungsaufwand
* komplexere Ereignisinfrastruktur
* möglicher Latenzaufwand
* Risiko von Ereignisstürmen
* anspruchsvollere Reihenfolgegarantien
* zusätzlicher Speicherbedarf

---

# 81. Verworfene Alternative: ausschließlich direkte Funktionsaufrufe

Vorteile:

* geringer unmittelbarer Overhead
* einfache lokale Kontrollflüsse
* geringe Anfangskomplexität

Nachteile:

* starke Kopplung
* schwer beobachtbare Seiteneffekte
* schlechte Erweiterbarkeit
* lange Aufrufketten
* schwierige asynchrone Verarbeitung
* geringe Fehlertoleranz

---

# 82. Verworfene Alternative: globaler Event Bus

Vorteile:

* einfache konzeptionelle Architektur
* zentrale Beobachtbarkeit

Nachteile:

* globaler Engpass
* hohe Lock-Contention
* schlechte Skalierbarkeit
* unklare Sicherheitsgrenzen
* Ereignisüberflutung

---

# 83. Verworfene Alternative: vollständig asynchroner Kernel

Vorteile:

* maximale Entkopplung
* hohe Parallelisierbarkeit

Nachteile:

* unnötiger Overhead für lokale Operationen
* komplizierte Fehlerbehandlung
* schwerer Determinismus
* höhere Latenzen
* aufwendige Zustandsautomaten

NovaOS verwendet deshalb ein hybrides Modell aus Fast Paths, synchronen Ereignissen und asynchroner Verarbeitung.

---

# 84. Verworfene Alternative: Ereignisse nur für Benutzerraum

Vorteile:

* geringere Kernelkomplexität

Nachteile:

* interne Kernelkopplung bleibt bestehen
* unvollständige Beobachtbarkeit
* keine einheitliche Kausalitätsanalyse
* begrenzte Recovery-Fähigkeit

---

# 85. Risiken

Wesentliche Risiken sind:

* Ereignisstürme
* Queue-Überläufe
* Prioritätsinversion
* rekursive Ereignisketten
* Deadlocks
* Payload-Lebensdauerfehler
* schwer verständliche asynchrone Abläufe
* unkontrollierte Broadcasts

Diese Risiken werden durch strikte Registry-, Queue-, Sicherheits- und Handlerregeln begrenzt.

---

# 86. Implementierungsrichtlinien

Alle Kernelkomponenten müssen:

* relevante Zustandsänderungen als Ereignisse definieren,
* Ereignistypen zentral registrieren,
* Ereignisquellen und Ziele explizit angeben,
* Payload-Besitz eindeutig definieren,
* Capability-Prüfungen durchführen,
* zeitkritische Handler begrenzen,
* blockierende Arbeit verzögert ausführen,
* Ereignisrekursion kontrollieren,
* Backpressure-Richtlinien einhalten,
* Ereignisse nach Möglichkeit CPU- und NUMA-lokal verarbeiten.

---

# 87. Verbotene Muster

Nicht zulässig sind:

* unregistrierte Ereignistypen
* globale Broadcasts ohne Begründung
* unbegrenzte Ereigniswarteschlangen
* blockierende Immediate Handlers
* implizite Payload-Besitzwechsel
* unkontrollierte synchrone Rekursion
* Capability-Umgehung über Ereignisse
* direkte subsystemübergreifende Seiteneffekte ohne Ereignismodell
* stilles Verwerfen kritischer Ereignisse

---

# 88. Implementierungsphasen

## Phase 1

* Event Registry
* statische Ereignistypen
* synchrone Zustellung
* einfache Deferred Queue
* Per-CPU-Queues
* Basis-Tracing

## Phase 2

* Prioritätsklassen
* Completion Objects
* Backpressure
* Event Coalescing
* Capability-Prüfung
* Kernel Object Graph Integration

## Phase 3

* NUMA-aware Routing
* Recovery Events
* erweiterte Kausalitätsanalyse
* Event Inspector
* deterministische Replay-Grundlagen

---

# 89. Teststrategie

Der ereignisgesteuerte Kernel-Core wird geprüft durch:

* Unit-Tests
* Registry-Tests
* Zustellungstests
* Prioritätstests
* Reihenfolgetests
* Rekursionstests
* Backpressure-Tests
* Queue-Overflow-Tests
* Capability-Tests
* SMP-Tests
* NUMA-Tests
* CPU-Hotplug-Tests
* Langzeittests
* Fehler-Injektion
* Performancebenchmarks
* Ereignissturm-Simulationen

---

# 90. Unit-Tests

Unit-Tests prüfen mindestens:

* Ereignisregistrierung
* Versionsprüfung
* Subscription-Lifecycle
* Handlerauswahl
* Payloadvalidierung
* Queueoperationen
* Completion Objects
* Retry Policy
* Kausalitätsverknüpfung
* Abbruchlogik

---

# 91. Integrationstests

Integrationstests umfassen:

* Interrupt zu Deferred Event
* Page Fault zu Memory Event
* Geräte-Hotplug
* Prozessbeendigung
* Speicherknappheit
* CPU Hotplug
* Treiberfehler und Recovery
* Sicherheitsverletzung
* Dateisystemfehler

---

# 92. Stresstests

Stresstests simulieren:

* Millionen niedrig priorisierter Ereignisse
* gleichzeitige Geräteinterrupts
* wiederholtes CPU Hotplug
* massenhaftes Thread Wake-up
* Speicherdruck
* Netzwerkpaketspitzen
* Event-Handler-Fehler
* Queue-Überlastung

---

# 93. Sicherheitsprüfungen

Geprüft werden:

* unerlaubtes Abonnieren
* gefälschte Ereignisquellen
* ungültige Objektverweise
* Capability-Eskalation
* Payload-Manipulation
* Event Flooding
* vertrauliche Daten in Traces
* Missbrauch von Broadcast-Kanälen

---

# 94. Performanceprüfungen

Gemessen werden:

* Emit-Latenz
* Dispatch-Latenz
* Queue-Durchsatz
* Handlerlaufzeit
* Cache-Miss-Rate
* Lock-Contention
* CPU-Skalierung
* NUMA-Fernzugriffe
* Speicherverbrauch pro Ereignis
* Tracing-Overhead

---

# 95. Akzeptanzkriterien

Die ADR gilt als umgesetzt, wenn:

* der Event Core initialisiert wird,
* Ereignistypen registriert werden können,
* synchrone und verzögerte Zustellung funktionieren,
* Per-CPU-Queues verfügbar sind,
* Payload-Lebenszyklen korrekt verwaltet werden,
* Capability-Prüfungen aktiv sind,
* der Kernel Object Graph als Routinggrundlage dient,
* kritische Ereignisse verlustfrei verarbeitet werden,
* Queue-Überlastung kontrolliert behandelt wird,
* Tracing und Diagnose grundlegende Kausalitätsketten darstellen können.

---

# 96. Konsequenzen für bestehende Kernelkomponenten

Bestehende Komponenten müssen schrittweise angepasst werden.

Priorität besitzen:

1. Interrupt Manager
2. Exception Manager
3. Scheduler
4. Process Manager
5. Memory Manager
6. Device Manager
7. Filesystem
8. Security Manager
9. Power Manager

Lokale Fast Paths dürfen bestehen bleiben, müssen jedoch relevante Zustandsänderungen veröffentlichen.

---

# 97. Dokumentationsanforderungen

Jeder Ereignistyp muss dokumentieren:

* Zweck
* Quelle
* zulässige Ziele
* Payload-Schema
* Version
* Priorität
* Zustellungsmodus
* Reihenfolgegarantie
* Capability-Anforderung
* Fehlerverhalten
* Wiederholungsstrategie
* Datenschutzklassifikation

---

# 98. Zukunftsperspektive

Langfristig soll der ereignisgesteuerte Kernel-Core erweitert werden um:

* automatische Ereigniskorrelationsanalyse
* KI-gestützte Anomalieerkennung
* selbstoptimierende Queue-Parameter
* adaptive Prioritäten
* präventive Recovery
* verteilte Kernelereignisse
* clusterweite Ereigniskanäle
* deterministisches System-Replay
* visuelle Live-Darstellung des Kernel Object Graph
* formale Verifikation kritischer Ereignisketten

---

# 99. Konsequenz

Der ereignisgesteuerte Kernel-Core wird zur verbindlichen Kommunikations- und Koordinationsarchitektur von NovaOS.

Subsystemübergreifende Zustandsänderungen werden nicht mehr als implizite Seiteneffekte behandelt, sondern als explizite, typisierte und nachvollziehbare Ereignisse.

Dadurch wird der Kernel zu einem zusammenhängenden System aus Objekten, Zuständen, Ereignissen und kontrollierten Reaktionen.

---

# 100. Zusammenfassung

NovaOS implementiert einen **ereignisgesteuerten Kernel-Core**, der eng mit dem Kernel Object Graph verbunden ist.

Hardwareinterrupts, Exceptions, Prozesszustände, Speicherereignisse, Geräteänderungen, Sicherheitsverletzungen und Recovery-Aktionen werden durch ein einheitliches Ereignismodell verarbeitet.

Die Architektur verwendet:

* typisierte Ereignisse
* zentrale Registrierung
* Per-CPU-Warteschlangen
* priorisierte Zustellung
* synchrone und asynchrone Verarbeitung
* Kausalitäts- und Korrelationsinformationen
* Capability-basierte Sicherheit
* Backpressure
* kontrollierte Ereignispropagation
* vollständige Diagnosefähigkeit

NovaOS entscheidet sich bewusst gegen einen ausschließlich direkten oder vollständig asynchronen Kernel. Stattdessen wird ein hybrides Modell verwendet, das schnelle lokale Operationen mit einer klaren, ereignisgesteuerten Koordination zwischen Kernelobjekten verbindet.

Damit entsteht eine skalierbare, beobachtbare und fehlertolerante Kernelarchitektur, die als Grundlage für dynamische Hardware, Self-Healing, Live-Diagnose und langfristige Erweiterbarkeit dient.
