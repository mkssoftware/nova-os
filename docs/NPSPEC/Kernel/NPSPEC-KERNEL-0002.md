# NPSPEC-KERNEL-0002 – Kernel Boot Sequence

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-KERNEL-0002 |
| Titel | Kernel Boot Sequence |
| Status | Spezifikation |
| Version | 1.0 |
| Datum | 2026-08-03 |
| Bereich | Kernel / Boot |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | NPSPEC-KERNEL-0001, NPSPEC-BOOTHANDOFF-0001, NPSPEC-KERNELENTRY-0001 |
| Zugehörige ADRs | ADR-BOOT-0001, ADR-BOOT-1012, ADR-KERNEL-0100, ADR-KERNEL-0101 |

---

## 1. Zweck

Diese Spezifikation definiert die Bootsequenz des NovaOS-Kernels vom Eintritt über das NovaOS Boot-Handoff Protocol bis zum Start des ersten Userspace-Prozesses.

Die Sequenz muss für BIOS- und UEFI-Systeme weitgehend identisch sein. Unterschiede zwischen Firmware und Bootloader dürfen nach dem Kernel-Einstieg nur noch über die übergebene Boot-Handoff-Struktur sichtbar sein.

## 2. Geltungsbereich

Diese Spezifikation beschreibt:

- die Kernel-Einstiegspunkte,
- die Validierung der Boot-Handoff-Daten,
- die Initialisierung der Prozessorarchitektur,
- den Aufbau der Speicherverwaltung,
- die Initialisierung der Kernel-Subsysteme,
- die Aktivierung von Interrupts und Scheduling,
- die Erkennung von Geräten,
- das Einhängen des Root-Dateisystems,
- den Start des ersten Userspace-Prozesses,
- Fehler- und Recovery-Pfade.

Nicht Bestandteil sind die interne Firmwareinitialisierung und die Implementierung des Bootloaders.

## 3. Normative Begriffe

Die Begriffe **MUSS**, **DARF NICHT**, **SOLLTE**, **SOLLTE NICHT** und **KANN** sind normativ zu verstehen.

## 4. Bootphasen

Die Kernel-Bootsequenz wird in folgende Phasen unterteilt:

| Phase | Bezeichnung | Hauptaufgabe |
|---:|---|---|
| 0 | Kernel Entry | Prozessorzustand übernehmen |
| 1 | Handoff Validation | Bootinformationen prüfen |
| 2 | Early Architecture | CPU und frühe Plattform initialisieren |
| 3 | Early Memory | physische Speicherverwaltung aufbauen |
| 4 | Virtual Memory | Seitentabellen und Kerneladressraum aktivieren |
| 5 | Kernel Core | Objektmodell und Kernsubsysteme starten |
| 6 | Interrupts and Time | Interrupts, Timer und Zeitbasis aktivieren |
| 7 | Scheduler and SMP | Scheduler und weitere CPUs starten |
| 8 | Device Discovery | Geräte und Treiber initialisieren |
| 9 | Root Filesystem | Root-Dateisystem bereitstellen |
| 10 | Userspace | ersten Userspace-Prozess starten |
| 11 | Operational | normalen Systembetrieb aufnehmen |

Die Phasen müssen in der definierten Reihenfolge durchlaufen werden. Explizit als parallelisierbar markierte Arbeit darf nach Aktivierung des Schedulers parallel erfolgen.

## 5. Kernel-Einstieg

Der Bootloader übergibt die Kontrolle an den architekturspezifischen Kernel-Einstiegspunkt.

Für die erste x86-Implementierung gilt:

```c
void kernel_entry(
    uint32_t boot_magic,
    uintptr_t handoff_address
);
```

Beim Eintritt müssen mindestens folgende Bedingungen erfüllt sein:

- die CPU befindet sich im vereinbarten Betriebsmodus,
- Interrupts sind deaktiviert,
- ein temporärer Stack ist verfügbar,
- die Boot-Handoff-Struktur ist adressierbar,
- das Kernel-Image befindet sich an der vereinbarten Ladeadresse,
- die Richtung für Stringoperationen ist zurückgesetzt,
- Paging entspricht dem vereinbarten Handoff-Zustand.

Architekturspezifischer Assemblercode muss anschließend eine definierte C-Laufzeitumgebung herstellen.

## 6. Früher Prozessorzustand

Vor dem Aufruf der generischen Kernelinitialisierung muss der Einstiegscode:

1. Interrupts deaktivieren,
2. einen bekannten Stack aktivieren,
3. erforderliche Segmentregister laden,
4. die Direction Flag löschen,
5. nicht initialisierte Kernelbereiche auf null setzen,
6. den Boot-Prozessor identifizieren,
7. die Bootparameter an die generische Kernelroutine weiterreichen.

```asm
cli
cld
mov esp, early_boot_stack_top
call np_kernel_early_start
```

Der frühe Stack darf nur bis zur Initialisierung regulärer Kernel-Stacks verwendet werden.

## 7. Boot-Handoff-Validierung

Der Kernel muss die Boot-Handoff-Struktur vollständig validieren, bevor darin enthaltene Zeiger verwendet werden.

Zu prüfen sind:

- Magic Number,
- Protokollversion,
- Gesamtgröße,
- Strukturgrenzen,
- Prüfsumme,
- Speicheradressbereiche,
- Kernel-Ladeinformationen,
- Speicherkarteneinträge,
- Framebufferinformationen,
- Module und Initial-RAM-Dateisystem,
- Firmwaretabellen,
- Sicherheitsstatus.

```c
typedef struct np_boot_handoff_header {
    uint64_t magic;
    uint16_t major_version;
    uint16_t minor_version;
    uint32_t total_size;
    uint32_t checksum;
    uint32_t flags;
} np_boot_handoff_header_t;
```

Eine unbekannte Hauptversion MUSS abgelehnt werden. Unbekannte optionale Felder einer kompatiblen Nebenversion dürfen übersprungen werden.

## 8. Frühe Diagnoseausgabe

Unmittelbar nach erfolgreicher Minimalvalidierung wird eine frühe Diagnoseausgabe eingerichtet.

Mögliche Ziele sind:

- serielle Schnittstelle,
- Firmware-Konsole,
- Textmodus,
- linearer Framebuffer,
- reservierter Boot-Log-Puffer.

Die frühe Diagnose darf keine dynamische Speicherverwaltung voraussetzen.

```c
void np_early_log(const char* message);
```

Sicherheitsrelevante Werte wie Stack Canaries, Schlüssel oder nicht freigegebene Speicherinhalte dürfen nicht ausgegeben werden.

## 9. Frühe Sicherheitsinitialisierung

Vor der Verarbeitung komplexer Daten müssen mindestens folgende Sicherheitsfunktionen initialisiert werden:

- temporärer Stack Canary,
- Prüfung des Secure-Boot-Status,
- Prüfung der Kernel-Image-Integrität,
- Initialisierung früher Zufallsquellen,
- Erfassung des aktiven Sicherheitsprofils,
- Vorbereitung von Kernel ASLR,
- Aktivierung verfügbarer CPU-Schutzfunktionen.

Kann eine Funktion auf der Hardware nicht aktiviert werden, muss dies als Sicherheitsstatus dokumentiert werden. Ältere Hardware darf in einem ausdrücklich gekennzeichneten reduzierten Sicherheitsmodus starten.

## 10. CPU- und Architekturerkennung

Der Kernel ermittelt die verfügbaren CPU-Funktionen und prüft die Mindestanforderungen der jeweiligen Architektur.

Für x86 gehören dazu unter anderem:

- CPUID-Unterstützung,
- Paging-Funktionen,
- NX-Unterstützung,
- APIC-Verfügbarkeit,
- TSC-Eigenschaften,
- SMEP und SMAP,
- unterstützte SIMD-Erweiterungen,
- Virtualisierungsmerkmale.

Nicht unterstützte optionale Funktionen werden deaktiviert. Fehlen zwingende Funktionen, wird der Bootvorgang kontrolliert beendet.

## 11. Frühe Plattforminitialisierung

Die Hardware-Abstraktionsschicht initialisiert die für den weiteren Bootvorgang notwendigen Plattformkomponenten:

- CPU-spezifische Tabellen,
- minimale Interrupt-Controller-Konfiguration,
- Firmwaretabellen,
- ACPI oder alternative Plattformbeschreibungen,
- frühe Timerquelle,
- Konsolen- und Diagnosegerät.

Firmwaredaten müssen kopiert oder reserviert werden, bevor der Speicher anderweitig verwendet wird.

## 12. Physische Speicherkarte

Die vom Bootloader übergebene Speicherkarte wird normalisiert.

Speicherbereiche werden mindestens in folgende Klassen eingeteilt:

| Klasse | Verwendung |
|---|---|
| Available | frei nutzbarer Arbeitsspeicher |
| Kernel | Kernel-Image und Kernel-Daten |
| Boot Data | Handoff-Struktur und Bootmodule |
| Firmware | Firmware- und Plattformdaten |
| Device Memory | MMIO- und Gerätespeicher |
| Reserved | nicht verwendbarer Speicher |
| Persistent | persistenter oder spezieller Speicher |
| Bad Memory | als fehlerhaft markierter Speicher |

Überlappende Einträge müssen aufgelöst werden. Reservierte Bereiche haben Vorrang vor frei nutzbaren Bereichen.

## 13. Early Allocator

Vor der regulären physischen Speicherverwaltung wird ein einfacher Early Allocator verwendet.

```c
void* np_early_alloc(
    size_t size,
    size_t alignment
);
```

Der Early Allocator muss:

- ausschließlich bekannte freie Speicherbereiche verwenden,
- ausgerichtete Allokationen unterstützen,
- alle vergebenen Bereiche protokollieren,
- ohne Freigabeoperation auskommen,
- nach Übergabe an den regulären Allocator deaktiviert werden.

Seine Speicherbelegungen müssen später als reserviert übernommen werden.

## 14. Physische Speicherverwaltung

Nach Normalisierung der Speicherkarte wird der Physical Memory Manager initialisiert.

Zu seinen Aufgaben gehören:

- Verwaltung physischer Seitenrahmen,
- Reservierung belegter Bereiche,
- Erkennung zusammenhängender Bereiche,
- Unterstützung von DMA-Anforderungen,
- Vorbereitung von NUMA-Richtlinien,
- Bereitstellung physischer Seiten für Seitentabellen.

Der Kernel darf die Bootloader-Speicherkarte nach Abschluss dieser Phase nicht mehr als primäre Speicherverwaltung verwenden.

## 15. Virtuelle Speicherverwaltung

Die virtuelle Speicherverwaltung erzeugt den endgültigen Kerneladressraum.

Dieser enthält mindestens:

- Kernel-Code,
- schreibgeschützte Kernel-Daten,
- Kernel-Daten und BSS,
- Kernel-Heap,
- Kernel-Stacks,
- direkten oder selektiven physischen Speicherzugriff,
- MMIO-Bereiche,
- modulare Kernelkomponenten,
- Guard Pages.

Die Seiteneigenschaften müssen dem tatsächlichen Verwendungszweck entsprechen.

| Bereich | Schreiben | Ausführen |
|---|---:|---:|
| Kernel-Code | Nein | Ja |
| Read-only Data | Nein | Nein |
| Kernel-Daten | Ja | Nein |
| Kernel-Stack | Ja | Nein |
| MMIO | Abhängig vom Gerät | Nein |

Write-and-Execute-Seiten sind im normalen Betrieb unzulässig.

## 16. Wechsel auf den endgültigen Kerneladressraum

Nach Aufbau der endgültigen Seitentabellen erfolgt ein kontrollierter Adressraumwechsel.

Dabei müssen:

1. alle noch benötigten Bootdaten abgebildet sein,
2. der neue Stack vorbereitet sein,
3. die Seitentabellen aktiviert werden,
4. Prozessorcaches und TLB-Zustände berücksichtigt werden,
5. die Ausführung an einer gültigen virtuellen Adresse fortgesetzt werden.

Nach erfolgreichem Wechsel darf der temporäre Bootadressraum freigegeben werden, sofern keine Abhängigkeit mehr besteht.

## 17. Kernel-Heap

Der Kernel-Heap wird erst nach funktionsfähiger physischer und virtueller Speicherverwaltung aktiviert.

```c
void* np_kalloc(size_t size, uint32_t flags);
void np_kfree(void* address);
```

Vor der Heapinitialisierung dürfen nur statische Speicherbereiche und der Early Allocator verwendet werden.

Kernsubsysteme müssen Allokationsfehler behandeln können. Eine fehlgeschlagene Allokation darf nicht automatisch zu einem undefinierten Systemzustand führen.

## 18. Kernel Object Graph

Nach Verfügbarkeit der Speicherverwaltung wird der Kernel Object Graph initialisiert.

Die ersten Objekte umfassen:

- Kernelinstanz,
- Boot-Prozessor,
- physische Maschine,
- Root-Namespace,
- Root-Capability-Domain,
- Kernel-Ereignisbus,
- Speicherverwaltungsobjekte,
- frühe Diagnoseobjekte.

```c
np_status_t np_object_graph_initialize(void);
```

Kein späteres Subsystem darf Objekte außerhalb des einheitlichen Objektmodells als dauerhaft isolierte Ressourcen verwalten.

## 19. Capability-System

Das Capability-System wird vor dem Start von Treibern und Userspace-Prozessen aktiviert.

Initial erstellt werden:

- Kernel-Root-Capability,
- Boot-Domain,
- Device-Manager-Domain,
- Filesystem-Domain,
- Init-Domain,
- Recovery-Domain.

Capabilities müssen nach dem Prinzip der minimal erforderlichen Rechte vergeben werden. Die Root-Capability darf nicht direkt an normale Userspace-Prozesse weitergegeben werden.

## 20. Ereignisgesteuerter Kernel-Core

Der Kernel-Ereignisbus wird nach Objekt- und Capability-System initialisiert.

Frühe Bootereignisse werden zunächst synchron verarbeitet. Nach Aktivierung des Schedulers dürfen geeignete Ereignisse asynchron zugestellt werden.

Beispiele:

```text
KERNEL_MEMORY_READY
KERNEL_OBJECT_GRAPH_READY
KERNEL_INTERRUPTS_READY
KERNEL_SCHEDULER_READY
DEVICE_DISCOVERY_STARTED
ROOT_FILESYSTEM_READY
USERSPACE_STARTED
```

Jedes Ereignis muss eine definierte Quelle, einen Typ und einen nachvollziehbaren Zeitstempel besitzen.

## 21. Interruptsystem

Das Interruptsystem wird in zwei Schritten initialisiert:

### 21.1 Strukturinitialisierung

- Interrupttabellen aufbauen,
- Exception Handler registrieren,
- Interrupt Controller konfigurieren,
- Interrupt-Routing vorbereiten,
- sichere Standardhandler installieren.

### 21.2 Globale Aktivierung

Interrupts dürfen erst global aktiviert werden, wenn:

- gültige Handler installiert sind,
- ein funktionsfähiger Kernel-Stack besteht,
- die Zeitquelle initialisiert wurde,
- kritische Datenstrukturen konsistent sind.

Unbehandelte Interrupts müssen maskiert und diagnostiziert werden.

## 22. Zeitverwaltung

Der Kernel wählt eine geeignete Clocksource und ein Clockevent-Gerät aus.

Mögliche Quellen sind:

- TSC,
- HPET,
- APIC Timer,
- PIT als Rückfalllösung,
- architekturspezifische Timer.

Die Zeitverwaltung stellt mindestens bereit:

- monotone Kernelzeit,
- Scheduler-Ticks oder Tickless Events,
- Timeout-Verwaltung,
- Profiling-Zeitstempel,
- Grundlage für die Systemzeit.

## 23. Scheduler-Initialisierung

Vor dem Start des Schedulers werden folgende Threads erstellt:

- Boot-Thread,
- Idle-Thread des Boot-Prozessors,
- Kernel-Event-Thread,
- Deferred-Work-Thread,
- gegebenenfalls früher Device-Manager-Thread.

Der aktuelle Bootkontext wird kontrolliert in den Boot-Thread überführt.

Der Scheduler darf erst aktiviert werden, wenn Thread-Kontexte, Timer und Interrupts funktionsfähig sind.

## 24. SMP-Initialisierung

Nach Aktivierung der grundlegenden Speicher- und Interruptverwaltung dürfen zusätzliche Prozessoren gestartet werden.

Der Boot-Prozessor übernimmt:

- Erkennung weiterer CPUs,
- Bereitstellung der AP-Startsequenz,
- Anlage CPU-lokaler Daten,
- Zuweisung eigener Kernel-Stacks,
- Einrichtung lokaler Interrupt Controller,
- Synchronisation mit dem Scheduler.

Jeder gestartete Prozessor muss einen eigenen Idle-Thread erhalten.

Das System darf mit weniger CPUs starten als erkannt wurden, wenn fehlerhafte CPUs isoliert und der reduzierte Zustand gemeldet werden.

## 25. Systemaufruf-Schnittstelle

Die Systemaufruf-Schnittstelle wird vor dem Start des ersten Userspace-Prozesses eingerichtet.

Sie umfasst:

- architekturspezifischen Eintritt,
- Wechsel auf einen Kernel-Stack,
- Validierung von Argumenten,
- Capability-Prüfung,
- sichere Rückkehr in den Userspace,
- versionierte Kernel-Service-Schnittstellen.

Noch nicht verfügbare Dienste müssen einen definierten Statuscode zurückgeben.

## 26. Device Manager

Der Device Manager wird nach dem Scheduler gestartet.

Er übernimmt:

- Import der Firmware- und Device-Tree-Informationen,
- Erkennung von Bussen,
- Erzeugung von Geräteobjekten,
- Treiberzuordnung,
- Verwaltung von Gerätezuständen,
- Ausgabe von Plug-and-Play-Ereignissen.

Geräte werden als Objekte in den Kernel Object Graph aufgenommen.

## 27. Treiberinitialisierung

Treiber werden nach Abhängigkeiten in Klassen gestartet:

1. Interrupt- und Timer-Treiber,
2. Firmware- und Bus-Treiber,
3. Speicher- und IOMMU-Treiber,
4. Konsolen- und Diagnosegeräte,
5. Blockgeräte,
6. Eingabegeräte,
7. Netzwerkgeräte,
8. optionale Geräte.

Userspace-Treiber dürfen erst gestartet werden, wenn IPC, Scheduling, Capabilities und die minimale Userspace-Laufzeit verfügbar sind.

## 28. Bootmodule und Initial-RAM-Dateisystem

Vom Bootloader geladene Module werden validiert und in Kernelobjekte überführt.

Das Initial-RAM-Dateisystem kann enthalten:

- frühe Treiber,
- Dateisystemmodule,
- Konfigurationsdaten,
- Recovery-Werkzeuge,
- den ersten Userspace-Prozess,
- Schlüsselmaterial in geschützter Form.

Jedes Modul muss hinsichtlich Größe, Position, Typ und Integrität geprüft werden.

## 29. VFS-Initialisierung

Das Virtual File System wird initialisiert, sobald Speicherverwaltung, Objektmodell und grundlegende IPC-Dienste verfügbar sind.

Dabei werden erzeugt:

- globales VFS-Objekt,
- initialer Mount-Namespace,
- Root-Mount-Platzhalter,
- Datei- und Verzeichnisobjekttypen,
- Dateihandle-Verwaltung,
- Cache-Strukturen.

Das Initial-RAM-Dateisystem wird zunächst als temporäres Root-Dateisystem eingehängt.

## 30. Root-Dateisystem

Das endgültige Root-Dateisystem wird entsprechend der Bootkonfiguration ausgewählt.

Mögliche Quellen sind:

- lokale Partition,
- NovaFS-Systemgeneration,
- verschlüsseltes Volume,
- Netzwerkdateisystem,
- Recovery-Subvolume,
- Initial-RAM-Dateisystem als Rückfalllösung.

Vor dem Root-Wechsel müssen Integrität, Kompatibilität und erforderliche Sicherheitsrichtlinien geprüft werden.

```c
np_status_t np_mount_system_root(
    const np_root_descriptor_t* descriptor
);
```

## 31. Systemgeneration und Recovery

NovaOS unterstützt unveränderliche Systemgenerationen.

Beim Bootvorgang muss geprüft werden:

- welche Generation ausgewählt wurde,
- ob ihre Metadaten gültig sind,
- ob Integritätsprüfungen erfolgreich sind,
- ob ein vorheriger Bootversuch fehlgeschlagen ist,
- ob eine Recovery-Generation verwendet werden muss.

Ein fehlgeschlagener normaler Start darf die verifizierte Recovery-Umgebung nicht beschädigen.

## 32. Start des ersten Userspace-Prozesses

Nach Bereitstellung des Root-Dateisystems startet der Kernel den ersten Userspace-Prozess.

Standardpfad:

```text
/system/core/init
```

Rückfallpfade können sein:

```text
/system/recovery/init
/boot/init
```

Der Init-Prozess erhält ausschließlich die für seine Aufgabe notwendigen Capabilities.

Dazu können gehören:

- Zugriff auf den System-Namespace,
- Erzeugung von Diensten,
- Start autorisierter Prozesse,
- Zugriff auf den Service Manager,
- eingeschränkter Zugriff auf Geräte- und Mount-Dienste.

## 33. Übergang zum normalen Betrieb

Der Bootvorgang gilt als erfolgreich abgeschlossen, wenn:

- der Init-Prozess läuft,
- das endgültige Root-Dateisystem aktiv ist,
- der Service Manager erreichbar ist,
- kritische Treiber verfügbar sind,
- die Sicherheitsrichtlinien aktiviert sind,
- Bootprotokolle dauerhaft übergeben wurden.

Anschließend wird das Ereignis `KERNEL_BOOT_COMPLETED` veröffentlicht.

Nicht mehr benötigte Bootressourcen werden freigegeben oder als schreibgeschützte Diagnosedaten archiviert.

## 34. Parallelisierung

Vor Aktivierung des Schedulers erfolgt die Initialisierung grundsätzlich seriell.

Danach dürfen folgende Vorgänge parallel ausgeführt werden:

- unabhängige Gerätetreiberinitialisierung,
- Erkennung verschiedener Busse,
- nicht kritische Integritätsprüfungen,
- Initialisierung optionaler Dateisysteme,
- Aufbau von Netzwerkkomponenten,
- Laden nicht kritischer Kernelservices.

Abhängigkeiten müssen explizit beschrieben werden. Die bloße Reihenfolge der Registrierung darf nicht als Abhängigkeitsmodell verwendet werden.

## 35. Initialisierungsstufen

Kernelkomponenten deklarieren ihre Initialisierungsstufe:

```c
typedef enum np_init_level {
    NP_INIT_EARLY_ARCH,
    NP_INIT_EARLY_MEMORY,
    NP_INIT_MEMORY,
    NP_INIT_KERNEL_CORE,
    NP_INIT_INTERRUPTS,
    NP_INIT_SCHEDULER,
    NP_INIT_BUS,
    NP_INIT_DEVICE,
    NP_INIT_FILESYSTEM,
    NP_INIT_USERSPACE,
    NP_INIT_LATE
} np_init_level_t;
```

Jede Komponente muss zusätzlich ihre Abhängigkeiten und Fehlerkritikalität angeben.

## 36. Initialisierungsstatus

Initialisierungsfunktionen liefern einen definierten Status zurück:

```c
typedef enum np_init_result {
    NP_INIT_SUCCESS,
    NP_INIT_DEGRADED,
    NP_INIT_RETRY,
    NP_INIT_RECOVERY_REQUIRED,
    NP_INIT_FATAL
} np_init_result_t;
```

| Ergebnis | Reaktion |
|---|---|
| `SUCCESS` | Bootvorgang fortsetzen |
| `DEGRADED` | protokollieren und eingeschränkt fortsetzen |
| `RETRY` | innerhalb definierter Grenzen erneut versuchen |
| `RECOVERY_REQUIRED` | Recovery-Pfad aktivieren |
| `FATAL` | sicheren Bootabbruch auslösen |

## 37. Boot-Timeouts

Initialisierungsvorgänge, die auf Hardware oder externe Zustände warten, müssen über definierte Timeouts verfügen.

Ein Timeout muss:

- das betroffene Subsystem identifizieren,
- einen Statuscode erzeugen,
- blockierende Ressourcen freigeben,
- abhängig von der Kritikalität einen Rückfallpfad auslösen.

Unbegrenztes aktives Warten ist außerhalb eng begrenzter früher Hardwaresequenzen unzulässig.

## 38. Bootfehler

Fehler werden in folgende Klassen eingeteilt:

| Klasse | Beispiel | Reaktion |
|---|---|---|
| Warnung | optionales Gerät fehlt | weiter starten |
| Degraded | Sicherheitsfunktion nicht verfügbar | eingeschränkt starten |
| Recoverable | Root-Dateisystem beschädigt | Recovery starten |
| Fatal | ungültige Seitentabellen | Boot abbrechen |
| Security Fatal | Kernelintegrität verletzt | sicher anhalten |

Der Fehlerbildschirm soll auch bei frühem Bootversagen über das bestmögliche verfügbare Ausgabegerät dargestellt werden.

## 39. Kernel Panic während des Bootvorgangs

Eine Kernel Panic vor dem Start des Userspace muss mindestens erfassen:

- aktuelle Bootphase,
- letzte erfolgreiche Initialisierungsstufe,
- Fehlercode,
- CPU-Kennung,
- Instruction Pointer,
- betroffene Komponente,
- Sicherheitsstatus.

Die Ausgabe sensibler Daten richtet sich nach der Secure-Debugging-Richtlinie.

Je nach Kernel-Panic-Policy wird anschließend:

- sicher angehalten,
- neu gestartet,
- die Recovery-Generation gestartet,
- ein Crash Dump erzeugt.

## 40. Bootprotokoll

Der Kernel führt ein strukturiertes Bootprotokoll.

```c
typedef struct np_boot_event {
    uint64_t timestamp;
    uint32_t phase;
    uint32_t component_id;
    np_status_t status;
    uint32_t flags;
} np_boot_event_t;
```

Das Protokoll muss nach Start des Userspace an den Diagnosedienst übergeben werden können.

Telemetrie bleibt standardmäßig lokal. Eine externe Übertragung darf nur nach ausdrücklicher Konfiguration erfolgen.

## 41. Speicherfreigabe nach dem Boot

Nach erfolgreichem Systemstart müssen nicht mehr benötigte Bereiche freigegeben werden:

- temporäre Seitentabellen,
- Early-Allocator-Metadaten,
- ersetzte CPU-Startsequenzen,
- nicht mehr benötigte Bootloader-Daten,
- freigabefähige Initialisierungsfunktionen,
- temporäre Modulabbildungen.

Benötigte Boot-Handoff-Informationen werden zuvor in reguläre Kernelobjekte kopiert.

## 42. Sicherheitsanforderungen

Die Bootsequenz muss folgende Sicherheitsregeln erfüllen:

- Bootdaten werden vor ihrer Verwendung validiert.
- Ausführbarer Speicher ist nicht gleichzeitig beschreibbar.
- Nicht benötigte Firmware- und Bootloader-Abbildungen werden entfernt.
- Kernel ASLR wird vor Aufbau des endgültigen Adressraums angewendet.
- Stack Protector wird vor komplexer Verarbeitung aktiviert.
- Capabilities werden vor dem Start von Userspace und Userspace-Treibern aktiviert.
- Debugschnittstellen bleiben ohne Autorisierung deaktiviert.
- Schlüsselmaterial wird nach Gebrauch sicher gelöscht.
- Der Sicherheitsstatus bleibt für autorisierte Diagnosedienste nachvollziehbar.

## 43. Performance-Anforderungen

Die Bootsequenz soll unnötige serielle Arbeit vermeiden.

Für Optimierungen gelten folgende Regeln:

- Sicherheit und Datenintegrität haben Vorrang vor Startzeit.
- Geräte dürfen verzögert initialisiert werden, wenn sie nicht bootkritisch sind.
- optionale Dienste sollen erst nach dem Init-Prozess gestartet werden.
- Bootmessungen müssen reproduzierbar und lokal auswertbar sein.
- Optimierungen dürfen keine impliziten Initialisierungsabhängigkeiten erzeugen.

## 44. Testanforderungen

Mindestens folgende Tests sind erforderlich:

1. erfolgreicher Start über BIOS,
2. erfolgreicher Start über UEFI,
3. Ablehnung einer ungültigen Handoff-Magic,
4. Ablehnung einer inkompatiblen Protokollversion,
5. Behandlung überlappender Speicherkarteneinträge,
6. Start mit minimal verfügbarem Arbeitsspeicher,
7. Aktivierung des endgültigen virtuellen Adressraums,
8. Start mit nur einem Prozessor,
9. Start mit mehreren Prozessoren,
10. Ausfall einer sekundären CPU,
11. fehlendes optionales Gerät,
12. fehlgeschlagener kritischer Treiber,
13. beschädigtes Root-Dateisystem,
14. Start der Recovery-Generation,
15. ungültiges Initial-RAM-Dateisystem,
16. Start auf Hardware ohne optionale Sicherheitsfunktionen,
17. Start des ersten Userspace-Prozesses,
18. Fehler des Init-Prozesses,
19. Freigabe temporärer Bootressourcen,
20. Übergabe des Bootprotokolls an den Diagnosedienst.

## 45. Verbindliche Invarianten

1. Interrupts werden erst nach Installation gültiger Handler aktiviert.
2. Dynamische Kernelallokationen erfolgen erst nach Initialisierung eines geeigneten Allocators.
3. Boot-Handoff-Zeiger werden niemals ungeprüft verwendet.
4. Userspace wird erst nach Aktivierung von Speicherisolation und Capability-System gestartet.
5. Jede aktive CPU besitzt einen gültigen Kernel-Stack und einen Idle-Thread.
6. Das Root-Dateisystem wird vor dem Start des Init-Prozesses validiert.
7. Kritische Bootfehler führen nicht zur unkontrollierten Fortsetzung.
8. Die Recovery-Umgebung bleibt von fehlgeschlagenen normalen Bootversuchen getrennt.
9. Nicht mehr benötigtes Schlüsselmaterial wird sicher entfernt.
10. Jede Bootphase besitzt einen eindeutig nachvollziehbaren Status.

## 46. Referenzsequenz

```text
Bootloader
  -> Kernel Entry
  -> Boot-Handoff validieren
  -> Early Console und Security
  -> CPU und Plattform erkennen
  -> Speicherkarte normalisieren
  -> Early Allocator
  -> Physical Memory Manager
  -> Virtual Memory Manager
  -> endgültigen Kerneladressraum aktivieren
  -> Kernel Heap
  -> Kernel Object Graph
  -> Capability-System
  -> Event Core
  -> Interrupts und Timer
  -> Scheduler
  -> SMP
  -> System Calls und IPC
  -> Device Manager
  -> Treiber
  -> VFS und Initial-RAM-Dateisystem
  -> endgültiges Root-Dateisystem
  -> Init-Prozess
  -> normaler Systembetrieb
```

## 47. Zusammenfassung

Die NovaOS Kernel Boot Sequence führt das System kontrolliert vom architekturspezifischen Kernel-Einstieg bis zum ersten Userspace-Prozess.

Die Initialisierung erfolgt in eindeutig definierten Phasen. Bootdaten werden validiert, Speicher- und Sicherheitsmechanismen früh aktiviert und Kernelressourcen in den einheitlichen Kernel Object Graph überführt. Erst danach werden Interrupts, Scheduler, Geräte, Dateisysteme und Userspace gestartet.

Fehlerhafte oder unsichere Zustände führen abhängig von ihrer Kritikalität zu einem eingeschränkten Start, zum Recovery-System oder zu einem sicheren Bootabbruch.