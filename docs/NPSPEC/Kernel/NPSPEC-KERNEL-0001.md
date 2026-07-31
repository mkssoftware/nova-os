# NPSPEC-KERNEL-0001 – Kernel Architecture

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-KERNEL-0001 |
| Titel | Kernel Architecture |
| Status | Spezifikation |
| Version | 1.0 |
| Datum | 2026-07-31 |
| Bereich | Kernelarchitektur |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | ADR-KERNEL-0100, ADR-KERNEL-0101, ADR-KERNEL-0102, ADR-KERNEL-0103, ADR-KERNEL-0104, ADR-KERNEL-0105 |
| Zielplattform | Initial x86 32-Bit, später x86-64 und weitere Architekturen |

---

## 1. Zweck

Diese Spezifikation definiert die grundlegende Architektur des NovaOS-Kernels.

Sie beschreibt:

- Kernelaufbau
- Subsystemgrenzen
- Kernelobjekte
- Capability-Modell
- Prozess- und Threadverwaltung
- Speicherverwaltung
- Ereignisverarbeitung
- Systemaufrufe
- Treiberintegration
- Sicherheit
- Diagnose
- ABI-Versionierung
- Mehrkernfähigkeit

Subsystemspezifische Details werden in nachfolgenden NPSPEC-Dokumenten definiert.

---

## 2. Normative Begriffe

Die Begriffe werden wie folgt verwendet:

| Begriff | Bedeutung |
|---|---|
| MUSS | verbindliche Anforderung |
| DARF NICHT | ausdrücklich verboten |
| SOLL | grundsätzlich einzuhalten |
| SOLLTE | empfohlen, Abweichung benötigt Begründung |
| KANN | optionale Funktion |

---

## 3. Architekturziele

Der NovaOS-Kernel MUSS folgende Ziele erfüllen:

- klare Subsystemgrenzen
- capability-basierte Ressourcenkontrolle
- einheitliches Kernelobjektmodell
- ereignisgesteuerte Verarbeitung
- Mehrkernfähigkeit
- versionierte Kernelservices
- lokale Diagnosefähigkeit
- Schutz vor fehlerhaften Userspace-Komponenten
- Unterstützung isolierter Userspace-Treiber
- Erweiterbarkeit auf weitere CPU-Architekturen
- vorhersehbare Fehler- und Ressourcenmodelle

---

## 4. Nichtziele

Diese Spezifikation definiert nicht:

- vollständige POSIX-Kompatibilität
- konkrete grafische Benutzeroberfläche
- konkrete NovaFS-On-Disk-Strukturen
- vollständige Netzwerkprotokolle
- konkrete Treiber einzelner Geräte
- Programmiersprachen-Laufzeiten
- Windows-Kompatibilitätsschichten
- Benutzer- und Anwendungsoberflächen

Diese Komponenten bauen auf den hier definierten Kernelmechanismen auf.

---

## 5. Kernelmodell

NovaOS verwendet einen modularen Hybridkernel.

Folgende Funktionen bleiben kernelnah:

- CPU- und Interruptverwaltung
- virtuelle Speicherverwaltung
- Scheduler
- Systemaufrufvermittlung
- Kernelobjekte und Capabilities
- grundlegende IPC
- Timer
- minimale Gerätedatenpfade
- Sicherheitsgrenzen
- Panic- und frühe Diagnosepfade

Komplexe Dienste SOLLEN bevorzugt im Userspace laufen:

- Dateisystemdienste, sofern praktikabel
- Netzwerkverwaltung
- DNS
- VPN Provider
- Gerätetreiber mit isolierbarem Datenpfad
- grafische Dienste
- Audio- und Mediendienste
- Update- und Paketverwaltung

---

## 6. Architekturübersicht

```text
Anwendungen und Systemdienste
              |
              v
      Versionierte User ABI
              |
              v
       System Call Layer
              |
              v
+----------------------------------+
| Kernel Object and Capability Core|
+----------------------------------+
| Scheduler | Memory | IPC | VFS   |
| Device    | Network| Power| Diag |
+----------------------------------+
| Architecture Abstraction Layer   |
+----------------------------------+
| CPU | MMU | Interrupts | Timer   |
+----------------------------------+
              |
              v
           Hardware
```

Subsysteme kommunizieren primär über Kernelobjekte, Events und versionierte interne Schnittstellen.

---

## 7. Kernel-Schichten

Der Kernel wird logisch in folgende Schichten gegliedert:

| Schicht | Verantwortung |
|---|---|
| Architecture Layer | CPU-, MMU-, Interrupt- und Timerdetails |
| Kernel Core | Objekte, Capabilities, Events und Synchronisation |
| Resource Layer | Speicher, Prozesse, Threads und Handles |
| Service Layer | IPC, VFS, Geräte, Netzwerk und Power |
| Security Layer | Richtlinien, Isolation und Audit |
| Diagnostic Layer | Logging, Tracing, Dumps und Profiling |
| User ABI Layer | Systemaufrufe und versionierte Services |

Zyklische Abhängigkeiten zwischen Subsystemen SOLLEN vermieden werden.

---

## 8. Kernelstart

Der Kernel MUSS durch das NovaOS Boot-Handoff-Protokoll gestartet werden.

Der Handoff MUSS mindestens bereitstellen:

- Speicherkarte
- Kernelposition
- Framebufferinformationen
- Bootmodus
- Plattforminformationen
- ACPI- oder Device-Tree-Zeiger
- geladene Module
- Systemgeneration
- Secure-Startup-Status
- Bootargumente
- Entropiedaten

Alle Handoff-Daten gelten bis zur Validierung als nicht vertrauenswürdig.

---

## 9. Initialisierungsphasen

Der Kernelstart erfolgt in definierten Phasen:

1. CPU-Grundzustand validieren.
2. temporären Stack einrichten.
3. Boot-Handoff validieren.
4. frühes Kernel Logging aktivieren.
5. Speicherkarte normalisieren.
6. Seitentabellen und Kerneladressraum initialisieren.
7. Descriptor-, Exception- und Interrupttabellen aufbauen.
8. Kernelobjekt- und Capability-Core starten.
9. Memory Manager aktivieren.
10. Scheduler und Boot-Thread initialisieren.
11. Plattform- und Device-Manager starten.
12. Userspace-Grunddienste laden.
13. Root- oder Systemnamespace aktivieren.
14. normalen Mehrkernbetrieb freigeben.

Jede Phase MUSS einen eindeutigen Fehlerpfad besitzen.

---

## 10. Kerneladressraum

Der Kernel besitzt einen von Userspace-Prozessen getrennten virtuellen Adressraum.

Der Kerneladressraum enthält mindestens:

- Kernelcode
- schreibgeschützte Konstanten
- Kernelheap
- physische Speichermappings
- MMIO-Bereiche
- Kernelstacks
- CPU-lokale Daten
- Modulbereiche
- Diagnose- und Crash-Dump-Reserve

Kernelcode SOLL nur lesbar und ausführbar sein.

Schreibbare Kernelseiten DÜRFEN standardmäßig NICHT ausführbar sein.

---

## 11. Architecture Abstraction Layer

Architekturspezifischer Code MUSS hinter klaren Schnittstellen verborgen werden.

Die Architecture Abstraction Layer umfasst mindestens:

```text
arch_cpu
arch_mmu
arch_interrupt
arch_timer
arch_context
arch_atomic
arch_cache
arch_power
arch_debug
```

Generischer Kernelcode DARF keine direkten x86-Register oder plattformspezifischen Firmwarestrukturen voraussetzen.

---

## 12. Kernelobjektmodell

Alle dauerhaft referenzierbaren Kernelressourcen SOLLEN als Kernelobjekte dargestellt werden.

```c
typedef struct nova_kernel_object {
    uint64_t object_id;
    uint32_t object_type;
    uint32_t state;
    uint64_t generation;
    atomic_uint reference_count;
    const nova_object_operations_t *operations;
} nova_kernel_object_t;
```

Ein Kernelobjekt besitzt mindestens:

- Typ
- eindeutige Objekt-ID
- Generation
- Zustand
- Referenzanzahl
- Rechtebeschreibung
- Lebenszyklusoperationen
- Diagnosemetadaten

Kernelzeiger DÜRFEN NICHT als öffentliche Objekt-Handles verwendet werden.

---

## 13. Kernelobjekttypen

Mindestens folgende Objekttypen werden vorgesehen:

- Process
- Thread
- Address Space
- Memory Object
- IPC Channel
- Event
- Shared Memory
- File Handle
- Device
- Device Interface
- Socket
- Timer
- Interrupt Binding
- Job Object
- Namespace
- Debug Session
- Trace Session
- Power Request

Neue Objekttypen benötigen eine stabile Typ-ID und definierte Rechte.

---

## 14. Capability-Modell

Der Zugriff auf Kernelobjekte erfolgt über Capabilities.

Eine Capability enthält logisch:

```c
typedef struct capability_entry {
    uint64_t object_id;
    uint64_t object_generation;
    uint64_t rights;
    uint32_t flags;
    uint32_t handle_generation;
} capability_entry_t;
```

Eine Capability:

- referenziert genau ein Kernelobjekt,
- enthält ausdrücklich gewährte Rechte,
- kann übertragbar oder nicht übertragbar sein,
- kann widerrufbar sein,
- darf Rechte niemals selbst erweitern,
- wird über einen prozesslokalen Handle angesprochen.

Objekt-ID oder Handlewert allein stellen keine Berechtigung dar.

---

## 15. Capability-Operationen

Der Kernel stellt mindestens bereit:

```c
status_t capability_duplicate(
    cap_handle_t source,
    uint64_t reduced_rights,
    cap_handle_t *destination
);

status_t capability_close(
    cap_handle_t handle
);

status_t capability_query(
    cap_handle_t handle,
    capability_info_t *info
);
```

Capability-Übertragungen zwischen Prozessen erfolgen über ausdrücklich dafür vorgesehene IPC-Mechanismen.

Die übertragenen Rechte DÜRFEN die Quellrechte NICHT überschreiten.

---

## 16. Referenzierung und Lebenszeit

Kernelobjekte verwenden sichere Referenzzählung oder eine gleichwertige Lebenszeitstrategie.

Es gilt:

- Handles halten Objekt-Referenzen.
- laufende Kerneloperationen halten temporäre Referenzen.
- ein Objekt wird erst nach Freigabe aller Referenzen zerstört.
- Entfernen und Zerstören bleiben getrennte Zustände.
- veraltete Handles werden über Generationen erkannt.
- Destructor-Code DARF keine neuen externen Referenzen erzeugen.

Zyklische Objektbeziehungen MÜSSEN ausdrücklich aufgelöst oder schwach referenziert werden.

---

## 17. Ereignisgesteuerter Kernel-Core

Kernelkomponenten SOLLEN Zustandsänderungen über Events und wartbare Objekte signalisieren.

Beispiele:

- I/O-Abschluss
- Thread Wake-up
- Geräteereignis
- Timerablauf
- IPC-Nachricht
- Netzwerkzustand
- Power-Übergang
- Debug-Ereignis

Interrupt Handler führen nur minimale unmittelbare Arbeit aus und verschieben umfangreiche Verarbeitung in Deferred Work oder Worker Threads.

---

## 18. Scheduler

Der Scheduler verwaltet Threads als ausführbare Kernelobjekte.

Er MUSS unterstützen:

- mehrere Schedulingklassen
- Prioritäten
- CPU-Affinität
- Echtzeitthreads
- Fairness
- Idle Threads
- Mehrkern-Load-Balancing
- blockierende Wait-Operationen
- Priority-Inversion-Gegenmaßnahmen

Jede aktive CPU besitzt genau einen Idle Thread.

Ein Thread darf sich immer nur in genau einem gültigen Schedulerzustand befinden.

---

## 19. Prozesse und Threads

Ein Prozess besitzt mindestens:

- Prozess-ID
- Address Space
- Capability Space
- Sicherheitskontext
- Mount- und Netzwerk-Namespace
- Threadliste
- Job-Zuordnung
- Ressourcenlimits
- Signalzustand

Ein Thread besitzt mindestens:

- Thread-ID
- CPU-Kontext
- Kernelstack
- Schedulerzustand
- Priorität
- Affinität
- Signalmaske
- Wait-Zustand
- Thread-lokale Kernelmetadaten

Prozess- und Thread-IDs stellen keine Zugriffsberechtigungen dar.

---

## 20. Speicherverwaltung

Der Memory Manager umfasst mindestens:

- physische Seitenverwaltung
- virtuelle Adressräume
- Page Fault Handling
- Memory Objects
- Shared Memory
- File-backed Mappings
- Copy-on-Write
- Guard Pages
- Kernelheap
- Kernelstackverwaltung
- DMA-Speicher
- NUMA-Richtlinien

Jeder Userspace-Prozess besitzt einen isolierten virtuellen Adressraum.

Userspace-Speicher wird vor jedem Kernelzugriff validiert.

---

## 21. Userspace-Zugriffe

Der Kernel DARF Userspace-Pointer NICHT direkt und ungeprüft dereferenzieren.

Es werden zentrale Funktionen verwendet:

```c
status_t copy_from_user(
    void *kernel_destination,
    const void *user_source,
    size_t size
);

status_t copy_to_user(
    void *user_destination,
    const void *kernel_source,
    size_t size
);
```

Diese Funktionen MÜSSEN:

- Adressgrenzen prüfen,
- Überläufe erkennen,
- Page Faults kontrolliert behandeln,
- teilweise Kopien eindeutig melden,
- Kerneladressen ablehnen.

---

## 22. Systemaufrufe

Systemaufrufe bilden die kontrollierte Grenze zwischen Userspace und Kernel.

Jeder Systemaufruf MUSS:

1. Nummer und ABI-Version validieren.
2. Argumente kopieren und prüfen.
3. Handles in referenzierte Objekte auflösen.
4. Capability-Rechte prüfen.
5. Ressourcen reservieren.
6. Operation ausführen.
7. Ergebnis in stabile ABI-Strukturen übersetzen.
8. temporäre Referenzen freigeben.

Systemaufrufe DÜRFEN keine internen Kernelstrukturen direkt offenlegen.

---

## 23. Kernel-Service-ABI

NovaOS verwendet versionierte Kernelservices statt einer unkontrolliert wachsenden Systemaufrufliste.

Ein Service wird identifiziert durch:

- Service-ID
- Major-Version
- Minor-Version
- Methoden-ID
- Eingabegröße
- Ausgabegröße
- Featureflags

Inkompatible Änderungen benötigen eine neue Major-Version.

Einmal veröffentlichte Methoden-IDs DÜRFEN innerhalb derselben Major-Version NICHT neu belegt werden.

---

## 24. ABI-Datentypen

Öffentliche ABI-Strukturen verwenden ausschließlich:

- festbreite Integer
- explizite Längen
- Offsets statt eingebetteter Pointer, wenn möglich
- Größen- und Versionsfelder
- definierte Byte-Reihenfolge
- reservierte, nullinitialisierte Felder

Native Typen wie `long`, `size_t` oder compilerabhängige Enums SOLLEN nicht in dauerhaften ABI-Strukturen verwendet werden.

32-Bit- und 64-Bit-Anwendungen müssen eindeutig unterstützt werden können.

---

## 25. Fehlerbehandlung

Kernel-APIs verwenden einen gemeinsamen `status_t`-Typ.

```c
typedef int32_t status_t;
```

Statuswerte werden gruppiert in:

- Erfolg
- allgemeine Fehler
- Speicherfehler
- Prozessfehler
- IPC-Fehler
- VFS-Fehler
- Gerätefehler
- Netzwerkfehler
- Sicherheitsfehler
- Power- und Diagnosefehler

Fehlerpfade MÜSSEN temporäre Ressourcen vollständig freigeben.

Ein Fehler darf kein teilweise autorisiertes Objekt veröffentlichen.

---

## 26. Synchronisation

Der Kernel stellt mindestens bereit:

- atomare Operationen
- Spinlocks
- Mutexes
- Reader-Writer-Locks
- Seqlocks oder generationsbasierte Leser
- Events
- Semaphore
- Completion-Objekte
- CPU-lokale Daten
- Memory Barriers

Spinlocks DÜRFEN nicht über blockierende Operationen hinweg gehalten werden.

Eine definierte Lock-Reihenfolge MUSS dokumentiert und überprüfbar sein.

---

## 27. Interrupt- und Exceptionkontext

Code im Interruptkontext DARF NICHT:

- blockieren,
- gewöhnliche Mutexes verwenden,
- unbeschränkte Speicherallokationen ausführen,
- synchrone RPC-Aufrufe starten,
- Dateisystemoperationen durchführen,
- Userspace-Code aufrufen.

Exceptions aus dem Userspace werden in Signals oder Prozessfehler übersetzt.

Nicht behandelbare Kernelexceptions führen entsprechend der Kernel-Panic-Richtlinie zum sicheren Fehlerpfad.

---

## 28. Mehrkernarchitektur

Der Kernel MUSS symmetrischen Mehrprozessorbetrieb unterstützen.

Grundsätze:

- CPU-lokale Daten für häufige Pfade
- keine unnötigen globalen Sperren
- explizite CPU-Affinität
- skalierbare Run Queues
- TLB-Shootdown-Protokoll
- Inter-Processor Interrupts
- CPU-Hotplug-fähige Subsysteme
- NUMA-bewusste Speicherverwaltung

Datenstrukturen müssen ihre Synchronisations- und Besitzregeln dokumentieren.

---

## 29. Treiberarchitektur

Treiber werden grundsätzlich über das Driver Framework verwaltet.

Userspace-Treiber sind zu bevorzugen für:

- USB-Klassen
- Audio
- Netzwerkfunktionen
- Sensoren
- nicht kritische Geräte
- komplexe Protokollparser

Kernel-Treiber bleiben zulässig für:

- frühe Bootgeräte
- Interruptcontroller
- Timer
- Speichercontroller
- Busgrundlagen
- zwingend latenzkritische Funktionen

Treiber erhalten Hardwarezugriff ausschließlich über zugewiesene Capabilities.

---

## 30. Kernelmodule

Dynamische Kernelmodule sind besonders privilegiert.

Ein Modul MUSS:

- gültig signiert sein,
- eine kompatible Kernel-ABI angeben,
- Abhängigkeiten deklarieren,
- W^X-Regeln einhalten,
- registrierte Ressourcen beim Entladen freigeben,
- vor Veröffentlichung vollständig validiert werden.

Das Entladen eines Moduls ist nur zulässig, wenn keine aktiven Referenzen, Handler oder Gerätebindungen mehr bestehen.

Userspace-Dienste werden gegenüber Kernelmodulen bevorzugt.

---

## 31. Sicherheitsarchitektur

Der Kernel MUSS mindestens unterstützen:

- Kernel ASLR
- NX und W^X
- Stack Protector
- Guard Pages
- SMEP und SMAP, sofern verfügbar
- getrennte Kernel- und Userspaceadressräume
- Capability-Prüfungen
- signierte Treiber und Module
- sichere Debugrichtlinien
- Audit
- kontrollierte Panic- und Recovery-Pfade

Fehlende Hardwarefunktionen müssen erkannt und als reduzierter Sicherheitsstatus ausgewiesen werden.

---

## 32. Kernel Logging und Tracing

Der Kernel MUSS lokale Diagnose bereitstellen durch:

- Kernel Logging
- Tracing
- Performance Counter
- Kernel Profiler
- Crash Dumps
- Debug API

Häufige Schreibpfade verwenden CPU-lokale, begrenzte Buffer.

Passwörter, private Schlüssel, Sitzungstoken und unnötige Nutzdaten DÜRFEN NICHT protokolliert werden.

Externe Telemetrie ist standardmäßig deaktiviert.

---

## 33. Panic und Recovery

Eine nicht fortsetzbare Kernelverletzung löst die definierte Panic Policy aus.

Der Panic-Pfad MUSS:

- rekursionsarm sein,
- keine gewöhnlichen Heap- oder VFS-Abhängigkeiten voraussetzen,
- andere CPUs kontrolliert stoppen,
- einen Mini Crash Dump versuchen,
- lokale Diagnose anzeigen,
- abhängig von Richtlinie neu starten, herunterfahren oder Recovery aktivieren.

Recovery MUSS über einen verifizierten, vom normalen System getrennten Startpfad erreichbar sein.

---

## 34. Ressourcenlimits

Jedes Kernelobjekt und Subsystem MUSS begrenzbar sein.

Limits gelten mindestens für:

- Prozesse
- Threads
- Capabilities
- Kernelobjekte
- IPC-Nachrichten
- offene Dateien
- Sockets
- Geräteoperationen
- gepinnte Seiten
- DMA-Mappings
- Trace- und Logbuffer
- Debug- und Profiler-Sessions

Limits werden pro Prozess, Job, Benutzer, Sicherheitsdomäne und systemweit angewendet.

---

## 35. Namens- und Identitätsregeln

IDs wie:

- Prozess-ID
- Thread-ID
- Objekt-ID
- Geräte-ID
- Interface-ID
- Namespace-ID

dienen ausschließlich der Identifikation und Diagnose.

Sie erteilen keine Rechte.

Öffentliche Zugriffe erfolgen über Capabilities oder ausdrücklich autorisierte Namensauflösungen, die eine Capability erzeugen.

---

## 36. Interne Schnittstellen

Kernelinterne Subsystem-APIs MÜSSEN:

- klare Eigentümerschaft definieren,
- erlaubte Aufrufkontexte dokumentieren,
- Sperrregeln angeben,
- Fehlercodes verwenden,
- Lebenszeit von Zeigern beschreiben,
- synchrones oder asynchrones Verhalten festlegen,
- ABI- oder API-Versionen besitzen, wenn sie von Modulen verwendet werden.

Ein interner Funktionsaufruf darf nicht stillschweigend eine Capability-Prüfung umgehen, wenn eine Sicherheitsgrenze überschritten wird.

---

## 37. Coding-Anforderungen

Kernelcode MUSS:

- freestanding-kompatibel sein,
- ohne unkontrollierte Laufzeitabhängigkeiten funktionieren,
- Integerüberläufe prüfen,
- Größen und Offsets validieren,
- Fehlerpfade vollständig behandeln,
- Initialisierung und Zerstörung symmetrisch umsetzen,
- Compilerwarnungen streng behandeln,
- undefiniertes Verhalten vermeiden,
- architekturspezifischen Code isolieren.

Sicherheitskritische Parser SOLLEN zusätzlich fuzz-getestet werden.

---

## 38. Bootstrapping-Abhängigkeiten

Die minimale Startreihenfolge lautet:

```text
Boot Handoff
    |
    v
Early Logging
    |
    v
Memory Manager
    |
    v
Kernel Objects und Capabilities
    |
    v
Interrupts und Timer
    |
    v
Scheduler
    |
    v
Device Manager
    |
    v
VFS und Systemdienste
    |
    v
Userspace
```

Ein früheres Subsystem DARF nicht zwingend von einem noch nicht initialisierten späteren Subsystem abhängen.

---

## 39. Testanforderungen

Die Kernelarchitektur benötigt mindestens:

- Unit Tests für reine Datenstrukturen
- Integrationstests für Subsystemgrenzen
- Systemaufruf-Fuzzing
- Userspace-Pointer-Validierung
- Capability-Rechte- und Widerrufstests
- Mehrkern- und Race-Tests
- Lock-Order-Prüfung
- Speicherknappheitstests
- Treiberabsturztests
- CPU-Hotplug
- Suspend und Resume
- Panic und Crash Dump
- ABI-Kompatibilitätstests
- Tests auf alter und moderner Hardware
- Virtualisierungstests

---

## 40. Verbindliche Invarianten

1. Öffentliche Kernelressourcen werden durch Kernelobjekte repräsentiert.
2. Zugriff auf Kernelobjekte erfolgt über Capabilities.
3. IDs und Kernelzeiger stellen keine Zugriffsberechtigung dar.
4. Userspace-Pointer werden niemals ungeprüft dereferenziert.
5. Schreibbare Kernelseiten sind standardmäßig nicht ausführbar.
6. Interrupt Handler führen keine blockierenden Operationen aus.
7. Systemaufrufe veröffentlichen keine internen Kernelstrukturen.
8. Kernelservices und ABI-Strukturen sind versioniert.
9. Jedes Kernelobjekt besitzt einen definierten Lebenszyklus.
10. Ressourcen sind pro Sicherheits- und Systemkontext begrenzt.
11. Userspace-Treiber werden bevorzugt, wenn technisch vertretbar.
12. Diagnose bleibt lokal und gibt keine Geheimnisse aus.
13. Mehrkern-Datenstrukturen dokumentieren Synchronisation und Eigentümerschaft.
14. Nicht fortsetzbare Kernelverletzungen verwenden den definierten Panic-Pfad.
15. Recovery bleibt vom gewöhnlichen Systemzustand getrennt und verifiziert.

---

## 41. Abnahmekriterien

NPSPEC-KERNEL-0001 gilt als initial umgesetzt, wenn:

- der Kernel über das NovaOS Boot-Handoff startet,
- Speicherverwaltung und isolierte Adressräume funktionieren,
- Kernelobjekte und Capabilities verfügbar sind,
- Prozesse und Threads erzeugt und beendet werden können,
- der Scheduler auf mindestens zwei CPUs arbeitet,
- IPC und Events funktionieren,
- versionierte Systemaufrufe verfügbar sind,
- mindestens ein Userspace-Systemdienst gestartet wird,
- Gerätetreiber über kontrollierte Ressourcen arbeiten,
- Kernel Logging und Panic-Pfad funktionieren,
- grundlegende Sicherheitsmechanismen aktiv oder ehrlich als nicht verfügbar gemeldet werden,
- automatisierte Architektur- und Isolationstests bestanden sind.

---

## 42. Zusammenfassung

Der NovaOS-Kernel wird als modularer Hybridkernel mit einem einheitlichen Kernelobjektmodell, capability-basierter Ressourcenverwaltung und ereignisgesteuertem Core implementiert.

Kernnahe Funktionen wie Speicherverwaltung, Scheduling, Interrupts und Sicherheitsgrenzen bleiben im Kernel. Komplexe und isolierbare Dienste sowie Treiber laufen bevorzugt im Userspace.

Versionierte Kernelservices, klar definierte Lebenszyklen, begrenzte Ressourcen und lokale Diagnose bilden die Grundlage für einen sicheren, skalierbaren und langfristig erweiterbaren NovaOS-Kernel.