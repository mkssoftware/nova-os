# ADR-BOOT-0003 – Mehrstufige Bootarchitektur

## Status

Angenommen

## Kategorie

Boot / Bootloader / Bootphasen / Firmware / Kernel Handoff / Recovery / Plattformabstraktion

## Kontext

Der Start eines Betriebssystems umfasst mehrere technisch sehr unterschiedliche Aufgaben.

Dazu gehören unter anderem:

- Kontrolle von der Firmware übernehmen,
- minimale Plattforminitialisierung,
- Zugriff auf das Bootmedium,
- Laden komplexerer Bootkomponenten,
- Speichererkennung,
- Grafikinitialisierung,
- Bootkonfiguration,
- Integritäts- und Signaturprüfung,
- Recovery-Auswahl,
- Kernel-Loading,
- CPU-Moduswechsel,
- Aufbau von Page Tables,
- Erzeugung von `BootInfo`,
- Übergabe der Systemkontrolle an den Kernel.

Diese Aufgaben besitzen unterschiedliche Anforderungen hinsichtlich:

- verfügbarem Speicher,
- CPU-Modus,
- Firmwarediensten,
- Dateisystemzugriff,
- Fehlerbehandlung,
- Sicherheitsprüfung,
- Größe und Komplexität.

Insbesondere beim BIOS-Boot ist der initial verfügbare Bootcode stark eingeschränkt.

Aber auch bei UEFI ist es architektonisch sinnvoll, Firmware Entry, gemeinsame Bootlogik und Kernel Handoff voneinander zu trennen.

NovaOS soll deshalb keine monolithische Bootloader-Komponente besitzen, die sämtliche Bootaufgaben gleichzeitig übernimmt.

---

## Entscheidung

NovaOS verwendet eine **mehrstufige Bootarchitektur**.

Der Bootprozess wird in klar abgegrenzte Phasen beziehungsweise Stages aufgeteilt.

Das konzeptionelle Modell lautet:

```text
Firmware
   │
   ▼
Stage 0
Firmware Entry
   │
   ▼
Stage 1
Minimal Loader
   │
   ▼
Stage 2
Nova Boot Core
   │
   ▼
Stage 3
Boot Planning / Verification / Recovery
   │
   ▼
Stage 4
Kernel Preparation
   │
   ▼
Stage 5
Kernel Handoff
   │
   ▼
NovaOS Kernel
```

Nicht jeder Firmwarepfad muss jede physische Stage als eigene Binärdatei besitzen.

Die **logische Phasentrennung** ist jedoch verbindlich.

---

## Grundprinzip

Es gilt:

```text
Boot complexity grows by stages.

Privilege and responsibility
must remain explicit.
```

und:

```text
Start small.

Validate early.

Add capability only when needed.

Hand off cleanly.
```

---

## Ziel der Stufentrennung

Die mehrstufige Architektur soll insbesondere:

- BIOS- und UEFI-Pfade vereinheitlichen,
- frühen Bootcode klein halten,
- Fehlerdomänen begrenzen,
- Testbarkeit verbessern,
- Recovery integrieren,
- Verified Boot erleichtern,
- Bootlogik wiederverwendbar machen,
- Firmwareabhängigkeiten früh isolieren,
- Kernel Handoff eindeutig definieren.

---

## Logische Bootphasen

NovaOS definiert mindestens folgende logische Phasen:

```text
Firmware Entry
Minimal Bootstrap
Boot Core
Boot Planning
Kernel Preparation
Kernel Handoff
```

Spezifische Implementierungen dürfen Unterstufen ergänzen.

---

## Stage 0 – Firmware Entry

Stage 0 bezeichnet den unmittelbaren Einstieg aus der Firmware.

Je nach Plattform kann dies beispielsweise sein:

```text
BIOS boot sector entry
```

oder:

```text
UEFI application entry
```

Stage 0 ist firmwareabhängig.

Seine Aufgabe besteht primär darin, eine kontrollierte NovaOS-Bootumgebung herzustellen.

---

## Stage 0 Aufgaben

Stage 0 darf insbesondere:

- Firmwaretyp bestimmen,
- grundlegenden CPU-Zustand erfassen,
- Firmwarezugriffe initialisieren,
- Bootmedium identifizieren,
- Stage 1 lokalisieren,
- minimale Fehlerausgabe bereitstellen.

Stage 0 soll keine umfangreiche Systempolicy enthalten.

---

## Stage 1 – Minimal Loader

Stage 1 besitzt nur die Funktionen, die notwendig sind, um eine leistungsfähigere Bootumgebung zu laden.

Beim BIOS-Pfad kann Stage 1 beispielsweise:

```text
MBR / Boot Sector
      ↓
Load additional sectors
      ↓
Transfer to Stage 2
```

umsetzen.

Unter UEFI kann Stage 1 logisch Bestandteil des EFI Loaders sein.

---

## Minimalitätsprinzip

Stage 1 soll möglichst klein bleiben.

Es soll insbesondere vermieden werden, hier bereits umfangreiche Funktionen zu implementieren wie:

- komplexe Dateisysteme,
- vollständige Grafik-UI,
- Recovery-Logik,
- umfangreiche Kryptografie,
- Treiberframeworks.

Diese Funktionen gehören in spätere Stages.

---

## Stage 1 Vertrauensgrenze

Stage 1 muss den nächsten Bootbestandteil identifizieren und, soweit technisch möglich, validieren.

Die exakte Vertrauenskette kann plattformspezifisch beginnen, muss jedoch spätestens mit dem gemeinsamen Nova Boot Core eine klare Verified-Boot-Grenze erreichen.

---

## Stage 2 – Nova Boot Core

Stage 2 bildet den gemeinsamen firmwareunabhängigen Bootkern.

Hier konvergieren BIOS und UEFI soweit möglich.

Das Modell lautet:

```text
BIOS Entry ─┐
            ├── Stage 2: Nova Boot Core
UEFI Entry ─┘
```

Der Boot Core übernimmt gemeinsame Bootmechanismen.

---

## Stage 2 Aufgaben

Der Nova Boot Core kann insbesondere enthalten:

- Boot Storage Abstraction,
- Dateisystemzugriff,
- Boot Resource Manager,
- Boot Configuration Loader,
- Kernel Image Loader,
- Modul-Loading,
- Boot Logging,
- grundlegende Boot-UI,
- Recovery Infrastructure,
- Signatur- und Integritätsprüfung.

---

## Firmware Provider

Firmwareabhängige Funktionen werden dem Boot Core über abstrahierte Provider angeboten.

Beispiel:

```text
BootFirmwareProvider
├── BiosBootProvider
└── UefiBootProvider
```

Der Shared Boot Core soll möglichst nicht direkt von BIOS- oder UEFI-Sonderfällen durchzogen sein.

---

## Stage 3 – Boot Planning

Stage 3 trifft aus der verfügbaren Konfiguration einen konkreten Bootplan.

Beispiel:

```text
Boot Configuration
      +
Available Generations
      +
Trust State
      +
Recovery State
      ↓
Boot Plan
```

Der Plan beschreibt, was tatsächlich gestartet werden soll.

---

## Boot Plan

Ein Boot Plan kann beispielsweise enthalten:

```text
BootPlan {
    mode
    kernel
    systemGeneration
    modules
    commandLine
    recoveryPolicy
    trustPolicy
    graphicsMode
}
```

Die genaue Struktur wird nachgelagert spezifiziert.

---

## Boot Planning und Mechanism/Policy

Die Trennung folgt `ADR-ARCH-0002_Mechanism_Policy_Separation`.

Der Bootloader stellt Mechanismen bereit.

Boot Policy entscheidet beispielsweise:

```text
normal boot
known-good boot
recovery
NovaDOS
diagnostic mode
```

---

## Stage 3 – Verifikation

Bevor ein Boot Plan aktiviert wird, müssen dessen relevanten Artefakte validiert werden.

Typischer Ablauf:

```text
Locate
  ↓
Parse
  ↓
Validate Structure
  ↓
Verify Integrity
  ↓
Verify Signature
  ↓
Evaluate Trust Policy
  ↓
Accept
```

Ein nicht validierter Kernel darf nicht allein deshalb gestartet werden, weil er gefunden wurde.

---

## Recovery-Entscheidung

Recovery ist Bestandteil der Bootplanung und keine vollständig getrennte spätere Notlösung.

Beispiel:

```text
Current Generation
      ↓ invalid
Known-Good Generation
      ↓ valid
Select Known-Good
```

oder:

```text
Normal Boot unavailable
      ↓
Recovery Environment
```

---

## Stage 4 – Kernel Preparation

Nach Auswahl und Validierung eines Boot Plans wird der konkrete Kernelstart vorbereitet.

Diese Phase umfasst insbesondere:

- Kernelimage laden,
- Segmente platzieren,
- Speicher reservieren,
- Bootmodule laden,
- initiale Page Tables vorbereiten,
- Kernel Stack vorbereiten,
- Framebufferinformationen finalisieren,
- Firmwareinformationen sichern,
- `BootInfo` erzeugen.

---

## Kernel Image Loading

Der Loader muss das native Kernel-Image strukturiert verarbeiten.

Bei ELF beispielsweise:

```text
ELF Header
    ↓
Program Headers
    ↓
Loadable Segments
    ↓
Memory Placement
    ↓
Entry Point
```

Header- und Segmentinformationen müssen vor Nutzung validiert werden.

---

## Speicherreservierung

Alle Bootkomponenten müssen ihre Speicherbereiche explizit reservieren.

Beispiele:

```text
Bootloader Code
Bootloader Data
Kernel Image
Kernel Stack
BootInfo
Modules
Framebuffer
Page Tables
Firmware Structures
```

Der Kernel erhält diese Informationen, damit keine Bereiche vorzeitig überschrieben werden.

---

## Stage 4 – CPU Preparation

Die Kernel Preparation stellt den für den Zielkernel erforderlichen Prozessorzustand her.

Für x86 kann dies beispielsweise umfassen:

```text
Real Mode
   ↓
Protected Mode
   ↓
Long Mode
```

abhängig vom ursprünglichen Bootpfad.

---

## Architekturabhängige Vorbereitung

Stage 4 besitzt einen architekturspezifischen Anteil.

Beispiele:

```text
x86_64:
    GDT
    page tables
    long mode
    control registers

ARM64:
    exception level transition
    translation tables
    system registers

RISC-V:
    privilege transition
    page tables
    SBI handoff
```

Diese Unterschiede dürfen nicht in die höheren gemeinsamen Bootmechanismen lecken.

---

## Stage 5 – Kernel Handoff

Stage 5 ist der kontrollierte Eigentums- und Kontrolltransfer vom Bootsystem an den Kernel.

Das Modell lautet:

```text
Boot Environment
      ↓
Final Validation
      ↓
Freeze Boot State
      ↓
Finalize BootInfo
      ↓
Release / Exit Firmware Services
      ↓
Transfer Control
      ↓
Kernel Entry
```

---

## Kernel Entry Contract

Der Kernel Handoff muss eine klar definierte ABI besitzen.

Konzeptionell:

```text
KernelEntry(BootInfo*)
```

Die tatsächliche Calling Convention ist architekturspezifisch.

---

## Handoff-Invarianten

Vor Kernel Entry müssen mindestens folgende Eigenschaften definiert sein:

- CPU-Modus,
- Stackzustand,
- Interruptzustand,
- Pagingzustand,
- Registerkonvention,
- Speicherreservierungen,
- `BootInfo`,
- Firmwarezustand,
- Boot Resource Ownership.

Der Kernel darf keine undokumentierten Annahmen über frühere Stages benötigen.

---

## Firmwareabschluss

Unter UEFI gehört zu Stage 5 insbesondere:

```text
Acquire final memory map
      ↓
ExitBootServices()
      ↓
Kernel Handoff
```

Nach erfolgreichem `ExitBootServices()` dürfen keine Boot Services mehr verwendet werden.

---

## BIOS-Handoff

Beim BIOS-Pfad müssen BIOS-Dienste spätestens dann verlassen werden, wenn der Kernelmodus dies erfordert.

Der Kernel darf nicht auf permanente BIOS-Interruptdienste angewiesen sein.

---

## Bootloader Retirement

Nach erfolgreichem Handoff gilt der Bootloader als logisch beendet.

Es soll keine versteckte residente Bootloaderlogik geben, die neben dem Kernel weiterläuft.

---

## Ownership Transfer

Ressourcen besitzen während des Bootvorgangs einen expliziten Owner.

Beispiel:

```text
Firmware
    ↓
Bootloader
    ↓
Kernel
```

Der Übergang muss für relevante Ressourcen nachvollziehbar sein.

---

## Boot Resource Manager

Der Boot Resource Manager verwaltet Ressourcen über mehrere Stages hinweg.

Beispiel:

```text
BootResource {
    id
    type
    owner
    base
    size
    lifetime
    state
}
```

Damit kann der Handoff sauber dokumentieren, welche Ressourcen:

```text
Transferred
Reclaimable
Persistent
Reserved
```

sind.

---

## Stage Contracts

Jede Stage besitzt definierte Eingaben und Ausgaben.

Beispiel:

```text
Stage N Input
    ↓
Validation
    ↓
Stage N
    ↓
Stage N Output
```

Die nächste Stage darf sich nur auf vertraglich garantierte Informationen verlassen.

---

## Keine impliziten globalen Zustände

Informationen zwischen Stages sollen nicht primär über unstrukturierte globale Variablen übertragen werden.

Stattdessen werden explizite Context-Strukturen verwendet.

Beispiel:

```text
BootContext
```

---

## BootContext

Ein interner `BootContext` kann während der Bootphasen wachsen.

Beispiel:

```text
BootContext {
    firmware
    architecture
    platform
    storage
    memory
    framebuffer
    configuration
    trust
    selectedPlan
    resources
    log
}
```

Vor Kernel Handoff wird daraus die stabile `BootInfo` erzeugt.

---

## BootContext vs BootInfo

`BootContext` ist eine interne Bootloaderstruktur.

`BootInfo` ist der versionierte Kernel-Handoff-Contract.

Es gilt:

```text
BootContext
    mutable
    internal
    bootloader-specific
```

gegen:

```text
BootInfo
    finalized
    versioned
    kernel-facing
```

---

## Keine Bootloader-internen Strukturen im Kernel ABI

Interne Pointer, Container oder Implementation Details des Bootloaders dürfen nicht unkontrolliert Bestandteil von `BootInfo` werden.

Nur explizit versionierte Daten dürfen übergeben werden.

---

## Stage-spezifische Fehler

Fehler werden der jeweiligen Bootphase zugeordnet.

Beispiele:

```text
Stage0FirmwareError
Stage1LoadFailure
Stage2StorageFailure
Stage3VerificationFailure
Stage4KernelPreparationFailure
Stage5HandoffFailure
```

Die genaue Fehlerstruktur darf semantischer statt nummernbasiert implementiert werden.

---

## Fehlerdomänen

Ein Fehler in einer späteren Stage soll nicht automatisch frühe Initialisierung wiederholen müssen.

Beispiel:

```text
Kernel signature invalid
```

muss nicht zwangsläufig:

```text
restart firmware entry
```

bedeuten.

Recovery kann innerhalb der vorhandenen Bootumgebung erfolgen.

---

## Boot Retry

Retry darf nur eingesetzt werden, wenn die Operation:

- sicher wiederholbar,
- idempotent,
- oder explizit recoveryfähig

ist.

Blindes Wiederholen unbekannter Bootoperationen ist nicht zulässig.

---

## Known-Good Fallback

Spätere Stages dürfen auf alternative bekannte Bootartefakte zurückgreifen.

Beispiel:

```text
Stage 3:
    Current Kernel invalid
        ↓
    Known-Good Kernel selected
```

Dies ist gegenüber einem vollständigen Neustart oft vorzuziehen.

---

## Recovery Environment

Die Recovery Environment wird aus dem gemeinsamen Boot Core gestartet.

Sie ist kein vollständig unabhängiger Firmwarepfad.

Beispiel:

```text
Stage 2 Boot Core
      ↓
Stage 3 Recovery Plan
      ↓
NovaDOS / Recovery Kernel
```

---

## NovaDOS

NovaDOS wird als alternativer Boot Target behandelt.

Konzeptionell:

```text
Boot Plan
    target = NovaDOS
```

Damit kann derselbe Bootmechanismus:

- normalen NovaOS-Kernel,
- Recovery-System,
- NovaDOS

laden.

---

## Bootmenü

Die grafische oder textuelle Bootoberfläche gehört in eine spätere Stage und nicht in Stage 0 oder Stage 1.

Damit bleibt der kritische Bootstrap klein.

---

## Boot-UI Fehler

Ein Fehler der grafischen Bootoberfläche darf den grundlegenden Bootpfad nicht zwangsläufig blockieren.

Fallback:

```text
Graphical Boot UI
      ↓ failure
Text UI
      ↓ failure
Minimal diagnostic boot path
```

---

## Separation of Concerns

Bootsubsysteme sollen logisch getrennt sein.

Beispiel:

```text
Firmware
Storage
Filesystem
Verification
Configuration
UI
Recovery
Kernel Loader
Handoff
```

Keine Stage soll unnötig alle Verantwortlichkeiten selbst implementieren.

---

## Treibermodell im Bootloader

Der Bootloader darf minimal notwendige Boottreiber besitzen.

Er soll jedoch kein vollständiges zweites NovaOS-Treibersystem aufbauen.

Treiber sollen auf das für Boot notwendige Minimum begrenzt werden.

---

## BIOS und UEFI

Die mehrstufige Architektur gilt für beide Firmwarepfade.

Beispiel BIOS:

```text
BIOS
 ↓
Stage 0 Boot Sector
 ↓
Stage 1 Loader
 ↓
Stage 2 Shared Boot Core
 ↓
Stage 3 Boot Plan
 ↓
Stage 4 Kernel Preparation
 ↓
Stage 5 Handoff
```

Beispiel UEFI:

```text
UEFI
 ↓
Stage 0/1 EFI Entry
 ↓
Stage 2 Shared Boot Core
 ↓
Stage 3 Boot Plan
 ↓
Stage 4 Kernel Preparation
 ↓
Stage 5 ExitBootServices + Handoff
```

Physische Stage-Grenzen dürfen sich unterscheiden.

Die semantischen Phasen bleiben gleich.

---

## Kein erzwungenes identisches Binary Layout

Diese ADR verlangt nicht:

```text
stage0.bin
stage1.bin
stage2.bin
stage3.bin
stage4.bin
stage5.bin
```

Die Implementierung darf mehrere logische Stages in einer Binärdatei zusammenfassen.

Entscheidend ist die architektonische Trennung der Verantwortlichkeiten und Contracts.

---

## Verifikation zwischen Stages

Kritische Übergänge sollen validiert werden.

Beispiele:

```text
Stage 1 → Stage 2
    image integrity

Stage 3 → Stage 4
    boot plan valid

Stage 4 → Stage 5
    kernel memory layout valid

Stage 5 → Kernel
    handoff invariants valid
```

---

## Chain of Trust

Die Stages bilden eine Vertrauenskette.

Konzeptionell:

```text
Stage 0
  ↓ verifies / establishes trust
Stage 1
  ↓
Stage 2
  ↓
Selected Boot Artifacts
  ↓
Kernel
```

Die genaue Root-of-Trust-Quelle hängt von Plattform und Bootpolicy ab.

---

## Kein Sicherheitsdowngrade zwischen Stages

Eine spätere Stage darf Sicherheitsanforderungen einer früheren Stage nicht stillschweigend abschwächen.

Beispiel:

```text
Verification Required
```

darf nicht zu:

```text
Verification failed
    ↓
continue anyway
```

führen.

---

## Boot State Machine

Die Bootstufen bilden eine explizite State Machine.

Beispiel:

```text
FirmwareEntry
    ↓
Bootstrap
    ↓
BootCoreReady
    ↓
PlanSelected
    ↓
KernelPrepared
    ↓
HandoffReady
    ↓
KernelEntered
```

Failure States dürfen separat existieren.

---

## Deklaratives Bootziel

Das gewünschte Bootziel wird unabhängig vom konkreten Mechanismus beschrieben.

Beispiel:

```text
DesiredBootTarget {
    systemGeneration
    mode
    recovery
}
```

Die Stages arbeiten darauf hin, dieses Ziel sicher zu realisieren.

---

## Transaktionale Bootplanung

Bootplanung und Aktivierung sollen soweit möglich transaktional aufgebaut sein.

Beispiel:

```text
Plan
  ↓
Validate
  ↓
Stage
  ↓
Prepare
  ↓
Verify
  ↓
Commit to Handoff
```

Bis zum finalen Handoff soll ein sicherer Wechsel auf Recovery oder Known-Good möglich bleiben.

---

## Commit Point

Der Kernel Handoff bildet einen wesentlichen Commit Point.

Vor diesem Punkt gehört die Bootkontrolle dem Boot Environment.

Danach dem Kernel.

Dieser Übergang muss eindeutig sein.

---

## Deterministic Boot

Bootstufen müssen deterministisch ausführbar sein, wenn die Bootkonfiguration fixiert ist.

Nicht deterministische Heuristiken dürfen nicht Voraussetzung für Bootkorrektheit sein.

---

## Boot-Reproduzierbarkeit

Für Diagnose und Tests sollen relevante Inputs protokollierbar sein:

```text
Firmware type
Bootloader version
Configuration version
Kernel hash
Selected generation
Boot mode
Stage results
```

Dadurch kann ein Bootvorgang nachvollzogen werden.

---

## Boot Logging

Jede Stage darf strukturierte Logeinträge erzeugen.

Beispiel:

```text
[Stage0] UEFI detected
[Stage1] Boot Core loaded
[Stage2] Storage initialized
[Stage3] Kernel verified
[Stage4] Kernel mapped
[Stage5] ExitBootServices successful
```

Der Log kann an den Kernel übergeben werden.

---

## Stage Timing

Optional darf die Dauer jeder Stage gemessen werden.

Beispiel:

```text
Firmware Entry       42 ms
Bootstrap             8 ms
Boot Core           115 ms
Verification         31 ms
Kernel Preparation   22 ms
Handoff               3 ms
```

Diese Daten unterstützen Bootoptimierung.

---

## Resource Economy

Auch der Bootloader soll Ressourcen bewusst verwalten.

Insbesondere:

- Speicher,
- temporäre Buffer,
- Grafikressourcen,
- Cache,
- geladene Images

müssen klaren Lifetimes zugeordnet sein.

---

## Früh freigebbare Ressourcen

Ressourcen, die nur in einer früheren Stage benötigt wurden, sollen später freigegeben oder als reclaimable markiert werden.

Beispiel:

```text
Stage 2 parser buffer
      ↓
not required after BootInfo finalization
      ↓
Reclaimable
```

---

## Persistente Bootressourcen

Bestimmte Ressourcen müssen über den Kernel Handoff hinaus bestehen.

Beispiele:

```text
Framebuffer
BootInfo
Initial Modules
Firmware Tables
```

Diese werden entsprechend markiert.

---

## Parallelisierung

Spätere Bootphasen dürfen unabhängige Aufgaben parallelisieren, wenn:

- Architektur dies erlaubt,
- Sicherheit gewährleistet ist,
- Determinism Contracts erfüllt bleiben,
- Komplexitätsgewinn gerechtfertigt ist.

Der frühe Bootstrap soll möglichst einfach bleiben.

---

## Kein verfrühtes Multithreading

Stage 0 und Stage 1 sollen nicht unnötig von SMP oder komplexer Parallelität abhängen.

Der Baseline-Bootpfad muss auch mit minimaler CPU-Initialisierung funktionieren.

---

## Self-Healing

Self-Healing kann ab einer ausreichend leistungsfähigen Bootstage eingebunden werden.

Beispiel:

```text
Boot verification failed
      ↓
Known-good metadata available
      ↓
repair / rollback plan
```

Self-Healing darf dabei Fail-Safe Defaults nicht umgehen.

---

## Architecture Introspection

Nach erfolgreichem Start soll nachvollziehbar sein, welche Bootstufen ausgeführt wurden.

Beispiel:

```text
Boot Path:
    UEFI

Stages:
    Firmware Entry        OK
    Bootstrap             OK
    Boot Core             OK
    Verification          OK
    Kernel Preparation    OK
    Handoff               OK
```

---

## Boot Failure Introspection

Bei einem fehlgeschlagenen Boot sollen Stage und Ursache erkennbar sein.

Beispiel:

```text
Failure Stage:
    Kernel Preparation

Reason:
    invalid ELF segment overlap
```

Dies erleichtert Recovery und Diagnose.

---

## Versionierung

Stage Contracts müssen versionierbar sein.

Dabei ist zwischen:

```text
internal stage contract
```

und:

```text
stable kernel handoff contract
```

zu unterscheiden.

Interne Contracts dürfen sich schneller entwickeln als `BootInfo`.

---

## Updatefähigkeit

Bootstufen sollen unabhängig aktualisierbar sein, soweit dies sicher möglich ist.

Beispiel:

```text
Firmware Entry
Boot Core
Recovery Image
```

können unterschiedliche Versionen besitzen.

Kompatibilität muss über Contracts geprüft werden.

---

## Boot Component Compatibility

Vor Übergabe zwischen separat versionierten Komponenten muss Kompatibilität validiert werden.

Beispiel:

```text
Stage1 supports BootCore ABI:
    3..5

BootCore ABI:
    4

Compatible
```

---

## Known-Good Boot Components

Nicht nur Kernelgenerationen, sondern auch Bootkomponenten dürfen Known-Good-Versionen besitzen.

Dies verhindert, dass ein fehlerhaftes Bootloaderupdate die einzige Startmöglichkeit zerstört.

---

## Rollback

Vor dem finalen Handoff können Bootkomponenten auf einen Known-Good-Pfad zurückfallen.

Ein Rollback nach tatsächlichem Kernel Handoff gehört dagegen in Kernel-/Recovery-Mechanismen.

---

## Architekturportabilität

Das mehrstufige Modell ist nicht auf x86 beschränkt.

Andere Plattformen dürfen eigene Firmware Entry Stages besitzen.

Beispiel:

```text
ARM Firmware
    ↓
Stage 0
    ↓
Nova Boot Core
```

oder:

```text
RISC-V Firmware / SBI
    ↓
Stage 0
    ↓
Nova Boot Core
```

---

## Hypervisor- und Virtual-Boot

Virtuelle Plattformen dürfen eigene Entry Provider besitzen.

Beispiel:

```text
Virtual Firmware
      ↓
Virtual Boot Provider
      ↓
Shared Boot Core
```

Die höhere Bootarchitektur bleibt gleich.

---

## Testbarkeit

Jede Stage muss soweit möglich isoliert testbar sein.

Beispiele:

```text
ELF loader test
BootConfig parser test
Verification test
BootPlan test
BootInfo builder test
Handoff invariant test
```

Firmware Entry Stages werden zusätzlich in Emulatoren und realer Hardware getestet.

---

## Mock Provider

Der Shared Boot Core soll mit Mock Providern getestet werden können.

Beispiel:

```text
MockFirmwareProvider
MockStorageProvider
MockFramebufferProvider
```

Damit können viele Bootfunktionen ohne realen Firmwarestart geprüft werden.

---

## Fault Injection

Boottests sollen gezielte Fehler injizieren können.

Beispiele:

```text
corrupted kernel
invalid memory map
missing framebuffer
storage timeout
signature mismatch
failed ExitBootServices
```

Dadurch werden Recovery und Fail-Safe-Verhalten geprüft.

---

## KI-Unabhängigkeit

Keine Bootstage darf KI für ihre fundamentale Korrektheit benötigen.

KI darf optional eingesetzt werden für:

- Fehleranalyse,
- Bootzeitoptimierung,
- Diagnose,
- Recovery-Empfehlungen.

Stage Transition, Verifikation und Kernel Handoff müssen deterministisch implementierbar bleiben.

---

## Normative Anforderungen

1. NovaOS MUSS eine mehrstufige Bootarchitektur verwenden.
2. Der Bootprozess MUSS in logisch getrennte Phasen mit definierten Verantwortlichkeiten gegliedert sein.
3. Firmware Entry MUSS von gemeinsamer höherer Bootlogik getrennt sein.
4. Frühe Bootstufen SOLLEN möglichst klein und einfach bleiben.
5. Stage 0 MUSS firmwareabhängige Entry-Aufgaben kapseln.
6. Stage 1 SOLL nur den für das Laden leistungsfähigerer Bootkomponenten notwendigen Bootstrap bereitstellen.
7. Der gemeinsame Nova Boot Core MUSS möglichst firmwareunabhängig implementiert sein.
8. BIOS und UEFI SOLLEN in denselben Shared Boot Core konvergieren.
9. Firmwarezugriffe SOLLEN hinter Boot Provider Contracts gekapselt werden.
10. Boot Planning MUSS von Low-Level-Firmwaremechanismen getrennt sein.
11. Kernel- und Recovery-Auswahl MUSS als expliziter Boot Plan darstellbar sein.
12. Bootartefakte MÜSSEN vor Aktivierung strukturell validiert werden.
13. Sicherheitsrelevante Artefakte MÜSSEN gemäß Trust Policy verifiziert werden.
14. Ein fehlgeschlagenes Required Verification DARF NICHT stillschweigend ignoriert werden.
15. Kernel Preparation MUSS eine eigene logisch definierte Phase bilden.
16. Kernel-Loading MUSS Image-Strukturen vor Nutzung validieren.
17. Alle vom Bootloader belegten Speicherbereiche MÜSSEN nachvollziehbar sein.
18. Bootressourcen MÜSSEN explizite Lifetimes besitzen können.
19. Boot Resource Ownership MUSS beim Kernel Handoff übertragbar sein.
20. BootInfo MUSS von internen Bootloaderstrukturen getrennt sein.
21. BootInfo MUSS versioniert sein.
22. Interne BootContext-Strukturen DÜRFEN veränderlich sein.
23. Bootloader-interne Implementation Details DÜRFEN NICHT ungeprüft in den Kernel ABI gelangen.
24. Der Kernel Handoff MUSS als explizite Stage modelliert werden.
25. Der Kernel Entry Contract MUSS eindeutig definiert sein.
26. CPU-Modus und Pagingzustand MÜSSEN beim Handoff definiert sein.
27. Stack- und Registerzustand MÜSSEN beim Handoff definiert sein.
28. Der Interruptzustand MUSS beim Handoff definiert sein.
29. UEFI Boot Services MÜSSEN vor dem UEFI-Kernel-Handoff kontrolliert beendet werden.
30. BIOS-Dienste DÜRFEN NICHT als dauerhafte Kernelvoraussetzung bestehen bleiben.
31. Der Bootloader MUSS nach erfolgreichem Handoff logisch beendet sein.
32. Es DARF keine unbeabsichtigte residente Bootloaderausführung nach Kernel Entry geben.
33. Jede Stage SOLL definierte Inputs und Outputs besitzen.
34. Stage Transitionen SOLLEN validiert werden.
35. Bootstufen SOLLEN keine unnötigen impliziten globalen Zustände austauschen.
36. Fehler MÜSSEN einer Bootphase zuordenbar sein.
37. Recovery MUSS aus einer ausreichend leistungsfähigen gemeinsamen Bootstage erreichbar sein.
38. Known-Good-Fallbacks SOLLEN vor einem vollständigen Neustart verwendet werden können.
39. NovaDOS MUSS als alternativer Boot Target integrierbar sein.
40. Boot-UI DARF NICHT Bestandteil des minimalen Stage-0-/Stage-1-Korrektheitspfades sein.
41. Ein Fehler der grafischen Boot-UI SOLL einen minimalen Fallbackpfad ermöglichen.
42. Bootloader-Treiber SOLLEN auf für den Bootvorgang notwendige Funktionen begrenzt bleiben.
43. Die mehrstufige Architektur MUSS BIOS und UEFI gleichermaßen unterstützen.
44. Physische Binärdateigrenzen DÜRFEN von logischen Stage-Grenzen abweichen.
45. Die Architektur DARF NICHT voraussetzen, dass jede logische Stage eine separate Datei ist.
46. Bootstufen MÜSSEN Teil einer klaren Vertrauenskette sein können.
47. Sicherheitsanforderungen DÜRFEN zwischen Stages NICHT stillschweigend abgeschwächt werden.
48. Der Bootprozess SOLL als explizite State Machine modellierbar sein.
49. Bootziele SOLLEN deklarativ beschreibbar sein.
50. Bootplanung SOLL transaktionale Vorbereitung und Validierung unterstützen.
51. Der Kernel Handoff MUSS einen eindeutigen Commit Point bilden.
52. Bootstufen MÜSSEN ohne KI deterministisch ausführbar sein.
53. Relevante Stage-Ergebnisse SOLLEN protokolliert werden.
54. Stage Timing DARF für Performanceanalyse erfasst werden.
55. Früh nicht mehr benötigte Bootressourcen SOLLEN freigebbar oder als reclaimable markierbar sein.
56. Persistente Bootressourcen MÜSSEN entsprechend markiert werden.
57. Frühe Stages SOLLEN nicht unnötig von SMP abhängen.
58. Self-Healing DARF in spätere Bootstufen integriert werden.
59. Self-Healing DARF Sicherheits- und Fail-Safe-Anforderungen NICHT umgehen.
60. Der ausgeführte Stage-Pfad MUSS introspektierbar sein.
61. Bootfehler SOLLEN Stage und Ursache introspektierbar machen.
62. Stage Contracts MÜSSEN versionierbar sein.
63. Separat aktualisierbare Bootkomponenten MÜSSEN Kompatibilität prüfen.
64. Known-Good-Versionen SOLLEN auch für Bootkomponenten unterstützt werden können.
65. Die Stage-Architektur MUSS für weitere CPU- und Firmwarearchitekturen erweiterbar sein.
66. Virtuelle Plattformen MÜSSEN in dasselbe höhere Stage-Modell integrierbar sein.
67. Gemeinsame Bootkomponenten SOLLEN isoliert testbar sein.
68. Mock Firmware Provider SOLLEN für Tests unterstützt werden.
69. Fault Injection SOLL in Boottests möglich sein.
70. Keine Bootstage DARF KI als Voraussetzung für fundamentale Bootkorrektheit verwenden.
71. KI DARF Diagnose und Optimierung unterstützen, aber keine Stage-Invarianten ersetzen.
72. NovaOS MUSS den Bootprozess als kontrollierte Folge zunehmender Fähigkeiten und klarer Übergaben behandeln.

---

## Konsequenzen

### Positive Konsequenzen

- klarere Trennung von Firmware, Bootstrap, Boot Core und Kernel Handoff,
- gemeinsame Architektur für BIOS und UEFI,
- leichter testbare Bootkomponenten,
- kleinere frühe Bootstufen,
- bessere Fehlerdiagnose,
- saubere Verified-Boot-Kette,
- Recovery und NovaDOS integrieren sich natürlich,
- Bootloaderupdates können granularer behandelt werden,
- Kernel erhält einen wohldefinierten Startzustand,
- zukünftige Plattformen können leichter integriert werden.

### Negative Konsequenzen

- mehrere Stage Contracts erhöhen den initialen Architekturaufwand,
- BootContext und Resource Ownership müssen sauber gepflegt werden,
- Kompatibilität zwischen separat versionierten Bootkomponenten muss geprüft werden,
- mehr Fehlerzustände und Transitionen müssen getestet werden,
- ein übermäßig fein aufgeteiltes Stage-Modell könnte unnötige Komplexität erzeugen.

---

## Verworfene Alternativen

### Monolithischer Bootloader

Verworfen.

Firmwarezugriff, Recovery, UI, Verification und Kernel Handoff wären unnötig stark gekoppelt.

### Vollständige Bootlogik bereits im ersten Bootsektor

Verworfen.

Dies ist insbesondere bei BIOS technisch unpraktisch und architektonisch schwer wartbar.

### Unterschiedliche Stage-Architekturen für BIOS und UEFI

Verworfen.

Firmware Entry darf unterschiedlich sein, die höhere Bootsemantik soll jedoch gemeinsam bleiben.

### Jede logische Stage zwingend als separate Datei

Verworfen.

Die logische Verantwortlichkeit ist wichtiger als die physische Binäraufteilung.

### Recovery als vollständig separater Bootloader

Verworfen.

Recovery soll dieselben grundlegenden Loading-, Verification- und Handoff-Mechanismen wiederverwenden.

### Kernel übernimmt noch wesentliche Bootloaderaufgaben ohne definierten Handoff

Verworfen.

Der Übergang zwischen Boot Environment und Kernel muss klar abgegrenzt sein.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-BOOT-0001_Parallele_BIOS_und_UEFI_Bootunterstützung`
- `ADR-BOOT-0002_UEFI_bevorzugen_mit_BIOS_Fallback`
- `ADR-HAL-0001_Einheitliche_Hardware_Abstraction_Layer`
- `ADR-HAL-0002_Plattformabstraktion_für_x86_ARM_und_zukünftige_Architekturen`
- `ADR-HAL-0003_Firmware_über_Platform_Provider_abstrahieren`
- `ADR-ARCH-0002_Mechanism_Policy_Separation`
- `ADR-ARCH-0003_Deklaratives_Systemmodell_als_Architekturgrundlage`
- `ADR-ARCH-0004_Transaktionale_Systemoperationen`
- `ADR-ARCH-0013_Architecture_Introspection`
- `ADR-ARCH-0014_Explizite_Contracts_zwischen_Systemkomponenten`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-ARCH-0019_Evolvierbare_Systemarchitektur`
- `ADR-ARCH-0020_KI_nicht_als_Korrektheitsvoraussetzung_des_Kernels`
- `ADR-TRUST-0001_Nova_Trust`

---

## Zugehörige NPSPECs

- `NPSPEC-BOOT-STAGE-ARCH-0001`
- `NPSPEC-BOOT-STAGE0-0001`
- `NPSPEC-BOOT-STAGE1-0001`
- `NPSPEC-BOOT-CORE-0001`
- `NPSPEC-BOOT-PLAN-0001`
- `NPSPEC-BOOT-CONTEXT-0001`
- `NPSPEC-BOOT-STAGE-CONTRACT-0001`
- `NPSPEC-BOOT-RESOURCE-0001`
- `NPSPEC-BOOT-STATE-MACHINE-0001`
- `NPSPEC-BOOT-KERNEL-LOADER-0001`
- `NPSPEC-BOOT-KERNEL-PREPARE-0001`
- `NPSPEC-BOOT-KERNEL-HANDOFF-0001`
- `NPSPEC-BOOT-BOOTINFO-0001`
- `NPSPEC-BOOT-VERIFICATION-0001`
- `NPSPEC-BOOT-RECOVERY-0001`
- `NPSPEC-BOOT-KNOWNGOOD-0001`
- `NPSPEC-BOOT-LOG-0001`
- `NPSPEC-BOOT-STAGE-TIMING-0001`
- `NPSPEC-BOOT-STAGE-COMPATIBILITY-0001`
- `NPSPEC-BOOT-STAGE-TEST-0001`
- `NPSPEC-BOOT-FAULT-INJECTION-0001`

---

## Verifikation

Die Umsetzung muss mindestens folgende Szenarien prüfen:

- BIOS-Start durch alle definierten logischen Bootphasen,
- UEFI-Start durch alle definierten logischen Bootphasen,
- korrekte Konvergenz in den Shared Boot Core,
- fehlgeschlagener Stage-1-Load,
- beschädigter Boot Core,
- ungültige Bootkonfiguration,
- ungültiges Kernelimage,
- Signaturfehler,
- Known-Good-Fallback,
- Recovery-Auswahl,
- NovaDOS-Auswahl,
- fehlerhafte Speicherkarte,
- Kernel Preparation Failure,
- Page-Table-Aufbau,
- UEFI `ExitBootServices()`-Fehler,
- korrekter BIOS-Kernel-Handoff,
- korrekter UEFI-Kernel-Handoff,
- Boot Resource Ownership Transfer,
- Reclaimable Boot Resources,
- strukturierter Bootlog,
- Stage Timing,
- Kompatibilitätsfehler zwischen Bootkomponenten,
- Mock-Provider-Test des Shared Boot Core,
- Fault-Injection pro Bootphase.

---

## Ergebnis

NovaOS behandelt den Bootprozess als klar strukturierte Folge von Phasen mit zunehmender Funktionalität und expliziten Übergaben.

Das Architekturmodell lautet:

```text
Firmware
   ↓
Minimal Entry
   ↓
Bootstrap
   ↓
Shared Boot Core
   ↓
Boot Plan
   ↓
Verification
   ↓
Kernel Preparation
   ↓
Kernel Handoff
   ↓
NovaOS
```

Dabei gilt:

```text
Early stages establish control.

Middle stages establish trust and intent.

Late stages establish the kernel environment.

The handoff transfers ownership.
```

Die zentrale Architekturregel lautet:

```text
Do not build one giant bootloader.

Build a controlled sequence
of explicit boot stages.

Each stage receives known state,
adds only the capability it needs,
and hands forward a validated result.
```