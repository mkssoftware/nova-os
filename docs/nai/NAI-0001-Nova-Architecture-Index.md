# NAI-0001: Nova Architecture Index

- **Status:** Angenommen
- **Version:** 0.1
- **Dokumenttyp:** Nova Architecture Index
- **Erstellt:** 2026-07-11
- **Gültig für:** Nova Platform Documentation System
- **Autor:** Nova Project
- **Ersetzt:** Keine
- **Primäre Referenz:** NPA-0001

---

## 1. Zweck

Dieses Dokument ist der zentrale Einstiegspunkt in die technische Dokumentation der Nova Platform.

Es beantwortet:

- Welche Dokumentarten existieren?
- Wo befindet sich welche Information?
- Welche Dokumente sind verbindlich?
- Wie hängen Architektur, APIs, Spezifikationen, Tests und Implementierung zusammen?
- In welcher Reihenfolge sollte ein neuer Entwickler die Dokumentation lesen?
- Welche Subsysteme gehören zur Nova Platform?
- Welche Dokumente existieren bereits und welche fehlen noch?

NAI-0001 ist kein Architekturentscheidungsdokument und keine Implementierungsspezifikation. Es dient ausschließlich als Navigations-, Ordnungs- und Referenzsystem.

---

## 2. Dokumentationshierarchie

```text
Nova Platform Documentation System
│
├── NPA      Master Architecture
├── NAI      Architekturindex und Navigation
├── RFC      Architekturregeln
├── ADR      Architekturentscheidungen
├── NPSPEC   Verhaltensspezifikationen
├── DES      Detaildesigns
├── API      Öffentliche API-Dokumentation
├── NIS      Interface Specifications
├── NFD      Nova File Format Documentation
├── SEC      Sicherheitsrichtlinien
├── TST      Testkonzepte
├── STYLE    Coding- und Designstandards
├── DAH      Developer Architecture Handbooks
├── ROADMAP  Meilensteine und Planung
└── DIAGRAMS Diagramme und visuelle Referenzen
```

---

## 3. Dokumenttypen

### 3.1 NPA – Nova Platform Architecture

NPA-Dokumente definieren die Gesamtarchitektur.

Beispiel:

```text
NPA-0001 – Nova Platform Master Architecture
```

NPA-Dokumente stehen oberhalb einzelner Subsysteme und beschreiben:

- Schichtenmodell
- Systemgrenzen
- Produktbeziehungen
- zentrale Architekturprinzipien
- übergreifende Abhängigkeiten
- Initialisierungsreihenfolgen

---

### 3.2 NAI – Nova Architecture Index

NAI-Dokumente strukturieren die gesamte Dokumentation.

Geplante Dokumente:

```text
NAI-0001 – Nova Architecture Index
NAI-0002 – Dependency Matrix
NAI-0003 – Complete Module Tree
NAI-0004 – Boot Flow Index
NAI-0005 – Data Flow Index
NAI-0006 – Glossary and Terminology Index
NAI-0007 – API and ABI Index
NAI-0008 – File Format Index
```

---

### 3.3 RFC – Request for Comments

RFCs definieren verbindliche Architekturregeln und Systemverträge.

Beispiele:

```text
RFC-0001   – Nova Platform Architektur
RFC-0001.1 – Nova Object Model
RFC-0001.2 – Nova Component System
```

RFCs beschreiben:

- Ziel
- Geltungsbereich
- verbindliche Regeln
- Schnittstellen
- Datenstrukturen
- Kompatibilitätsanforderungen

---

### 3.4 ADR – Architecture Decision Record

ADRs dokumentieren konkrete Architekturentscheidungen.

Sie beantworten:

> Warum wurde diese Lösung gewählt?

ADRs enthalten:

- Kontext
- Problemstellung
- Alternativen
- Entscheidung
- Begründung
- Konsequenzen
- Auswirkungen
- Referenzen

Die ADR-Nummern sind nach Themenbereichen gegliedert.

---

### 3.5 NPSPEC – Nova Platform Specification

NPSPEC-Dokumente definieren das beobachtbare Verhalten eines Systems.

Beispiele:

```text
NPSPEC-0001 – Object System Specification
NPSPEC-0002 – Component System Specification
NPSPEC-0003 – Service Bus Specification
NPSPEC-1001 – BootInfo Specification
NPSPEC-5001 – Window Manager Behavior
NPSPEC-6001 – NVFS Path Semantics
```

Eine NPSPEC beschreibt nicht primär die interne Implementierung, sondern:

- Zustände
- Eingaben
- Ausgaben
- Fehlerfälle
- Garantien
- Randbedingungen
- Kompatibilitätsregeln

---

### 3.6 DES – Detail Design

DES-Dokumente beschreiben die konkrete interne Implementierung.

Beispiele:

```text
DES-2001 – Physical Page Allocator
DES-2002 – Kernel Heap
DES-4001 – Software Renderer
DES-6001 – FAT32 Driver
```

DES-Dokumente enthalten:

- Algorithmen
- interne Datenstrukturen
- Speicherlayouts
- Sequenzdiagramme
- Zustandsautomaten
- Performanceüberlegungen
- Implementierungsgrenzen

---

### 3.7 API – Public API Documentation

API-Dokumente beschreiben öffentliche Programmierschnittstellen.

Beispiele:

```text
API-0001 – Nova Foundation API
API-4001 – NGE Public API
API-5001 – NUI Public API
API-6001 – NVFS Public API
API-8001 – Nova SDK API
```

API-Dokumente enthalten:

- Funktionssignaturen
- Datentypen
- Vorbedingungen
- Nachbedingungen
- Rückgabewerte
- Fehlercodes
- Thread-Safety
- Besitzregeln

---

### 3.8 NIS – Nova Interface Specification

NIS-Dokumente definieren binäre oder modulübergreifende Schnittstellen.

Beispiele:

```text
NIS-0001 – Object Handle Interface
NIS-1001 – BootInfo ABI
NIS-2001 – Syscall ABI
NIS-8001 – Plugin ABI
NIS-8002 – Service Interface ABI
```

NIS ist verbindlicher als reine API-Dokumentation und betrifft insbesondere:

- Binärlayout
- Calling Convention
- Strukturgrößen
- ABI-Versionen
- Endianness
- Alignment
- Kompatibilitätsprüfungen

---

### 3.9 NFD – Nova File Format Documentation

NFD-Dokumente spezifizieren Nova-eigene Dateiformate.

Geplante Formate:

```text
NFD-0001 – NIMG
NFD-0002 – NFONT
NFD-0003 – NICON
NFD-0004 – NTHEME
NFD-0005 – NPAK
NFD-0006 – NBACKUP
NFD-0007 – NPANIC
NFD-0008 – NPLUGIN
```

Jede NFD enthält:

- Magic Number
- Version
- Header
- Byte-Reihenfolge
- Feldgrößen
- Prüfsummen
- Kompression
- Sicherheitsgrenzen
- Parserregeln
- Beispielstruktur

---

### 3.10 SEC – Security Specification

SEC-Dokumente definieren Sicherheitsanforderungen.

Beispiele:

```text
SEC-0001 – Secure Coding Rules
SEC-0002 – Input Validation
SEC-0003 – Plugin Isolation
SEC-6001 – Secure Wipe Safety
SEC-6002 – Restore Target Validation
```

---

### 3.11 TST – Test Specification

TST-Dokumente definieren, wie Systeme überprüft werden.

Beispiele:

```text
TST-0001 – Foundation Tests
TST-1001 – Boot Tests
TST-2001 – Kernel Tests
TST-4001 – Renderer Tests
TST-6001 – Filesystem Image Tests
```

---

### 3.12 STYLE – Style and Coding Standards

STYLE-Dokumente definieren:

- Coding Style
- Headerregeln
- Namenskonventionen
- Dokumentationsstil
- UI Design Rules
- Commit- und Reviewregeln

Geplante Dokumente:

```text
STYLE-0001 – C Coding Style
STYLE-0002 – Assembly Coding Style
STYLE-0003 – Documentation Style
STYLE-5001 – NUI Design Language
```

---

### 3.13 DAH – Developer Architecture Handbook

DAH-Dokumente verbinden viele Einzelentscheidungen zu lesbaren Entwicklerhandbüchern.

Geplante Bände:

```text
DAH-01 – Systemarchitektur
DAH-02 – Boot Manager
DAH-03 – Kernel
DAH-04 – HAL
DAH-05 – Graphics
DAH-06 – UI
DAH-07 – Storage & Rescue
DAH-08 – SDK & Plugins
DAH-09 – Applications
DAH-10 – Build & Development
```

---

## 4. ADR-Nummernbereiche

```text
ADR-0001 bis ADR-0999   Foundation
ADR-1000 bis ADR-1999   Boot Manager
ADR-2000 bis ADR-2999   Kernel
ADR-3000 bis ADR-3999   HAL
ADR-4000 bis ADR-4999   Graphics
ADR-5000 bis ADR-5999   UI
ADR-6000 bis ADR-6999   Storage & VFS
ADR-7000 bis ADR-7999   Applications
ADR-8000 bis ADR-8999   SDK & Interfaces
ADR-9000 bis ADR-9999   Build & Toolchain
```

---

## 5. Aktuell vorhandene RFCs

```text
RFC-0001
Nova Platform – Projektphilosophie und Systemarchitektur

RFC-0001.1
Nova Object Model

RFC-0001.2
Nova Component System
```

Geplante nächste RFCs:

```text
RFC-0002 – Nova Object Manager
RFC-0003 – Nova Collections Library
RFC-0004 – Nova String System
RFC-0005 – Nova Service Model
RFC-1001 – Nova BootInfo ABI
RFC-2001 – Nova Kernel Architecture
RFC-4001 – Nova Graphics Engine
RFC-5001 – Nova UI Framework
RFC-6001 – Nova Virtual File System
RFC-8001 – Nova SDK
```

---

## 6. Aktuell vorhandene ADR-Gruppen

### 6.1 Foundation

```text
ADR-0001 bis ADR-0025
```

Themen:

- C als Systemsprache
- ObjectID Handles
- Object Model
- Component System
- Service Bus
- Status- und Fehlersystem
- Logging
- Speicherarchitektur
- Collections
- Strings
- Asset Pipeline
- Theme System
- Buildsystem
- Versionierung
- ABI
- Modulstruktur
- Headerregeln
- Tests
- Dokumentation
- Namenskonventionen
- Thread-Safety
- Konfiguration
- Panic Reports
- Debug Inspector
- Plugins

---

### 6.2 Boot

```text
ADR-1001 bis ADR-1010
```

Themen:

- mehrstufige Bootarchitektur
- BIOS zuerst
- UEFI als separates Frontend
- BootInfo ABI
- Grafikinitialisierung
- Asset Loader
- Theme Loader
- PNG-Pipeline
- TTF-Pipeline
- Bootmenü

---

### 6.3 Kernel

```text
ADR-2001 bis ADR-2015
```

Themen:

- Physical Memory
- Virtual Memory
- Kernel Heap
- Scheduler
- IPC
- Timer
- Interrupts
- Object Manager
- Component Manager
- Service Manager
- Processes
- Threads
- Security
- Kernel Panic
- Module Loader

---

### 6.4 HAL

```text
ADR-3001 bis ADR-3010
```

Themen:

- PCI
- USB
- SATA/AHCI
- NVMe
- HID
- Audio
- Video
- Network
- DMA
- IRQ Routing

---

### 6.5 Graphics

```text
ADR-4001 bis ADR-4011
```

Themen:

- Renderer
- Double Buffer
- Surface
- Alpha
- Blur
- Shadow
- Animation
- Image Formats
- Vector Graphics
- Font Engine
- GPU Abstraction

---

### 6.6 UI

```text
ADR-5001 bis ADR-5010
```

Themen:

- Window Manager
- Desktop
- Widgets
- Layout
- Focus
- Events
- Drag & Drop
- Clipboard
- Notifications
- Theme Engine

---

### 6.7 Storage

```text
ADR-6001 bis ADR-6010
```

Themen:

- NVFS
- FAT32
- exFAT
- NTFS
- ext4
- GPT
- MBR
- Backup
- Restore
- Secure Wipe

---

### 6.8 Applications

```text
ADR-7001 bis ADR-7008
```

Themen:

- Explorer
- Backup Center
- Restore Center
- Hardware Center
- Settings
- Installer
- Software Center
- Terminal

---

### 6.9 SDK

```text
ADR-8001 bis ADR-8005
```

Themen:

- SDK Architecture
- API Stability
- Plugin API
- UI API
- Service API

---

### 6.10 Build

```text
ADR-9001 bis ADR-9005
```

Themen:

- Build System
- Cross Compiler
- Asset Builder
- Image Builder
- CI & Tests

---

## 7. Plattformmodule

```text
Nova Platform
│
├── Boot
│   ├── BIOS Frontend
│   ├── UEFI Frontend
│   ├── Boot Core
│   ├── Boot Manager
│   ├── Boot Asset Loader
│   └── Kernel Loader
│
├── Foundation
│   ├── Types
│   ├── Status
│   ├── Error
│   ├── Logging
│   ├── Assert
│   ├── Memory API
│   ├── Strings
│   ├── Collections
│   ├── Objects
│   └── Components
│
├── Kernel
│   ├── Memory
│   ├── Interrupts
│   ├── Timer
│   ├── Scheduler
│   ├── Threads
│   ├── Processes
│   ├── Syscalls
│   ├── IPC
│   └── Panic
│
├── HAL
│   ├── CPU
│   ├── PCI
│   ├── USB
│   ├── Storage
│   ├── Input
│   ├── Video
│   ├── Audio
│   └── Network
│
├── Runtime
│   ├── Object Manager
│   ├── Component Manager
│   ├── Event Manager
│   ├── Service Registry
│   ├── Message Bus
│   ├── Task Manager
│   └── Plugin Runtime
│
├── Services
│   ├── Storage
│   ├── NVFS
│   ├── Assets
│   ├── Themes
│   ├── Logging
│   ├── Tasks
│   ├── Notifications
│   └── Security
│
├── Libraries
│   ├── NGE
│   ├── NUI
│   ├── NAS
│   ├── NVFS
│   ├── NCrypto
│   ├── NCompress
│   └── NCodec
│
├── Applications
│   ├── Explorer
│   ├── Backup
│   ├── Restore
│   ├── Hardware
│   ├── Settings
│   ├── Installer
│   ├── Software Center
│   └── Terminal
│
└── Tools
    ├── Asset Builder
    ├── Font Builder
    ├── Image Builder
    ├── Theme Builder
    ├── Package Builder
    ├── Disk Image Builder
    ├── Test Runner
    └── Documentation Builder
```

---

## 8. Lesereihenfolge für neue Entwickler

Empfohlene Reihenfolge:

1. NPA-0001 – Master Architecture
2. NAI-0001 – Architecture Index
3. RFC-0001 – Platform Architecture
4. RFC-0001.1 – Object Model
5. RFC-0001.2 – Component System
6. ADR-0001 bis ADR-0025 – Foundation
7. subsystembezogene ADRs
8. zugehörige NPSPEC
9. zugehörige DES
10. API-Dokumentation
11. Tests
12. Quellcode

---

## 9. Dokumentstatus

Erlaubte Statuswerte:

```text
Entwurf
In Prüfung
Angenommen
Implementiert
Ersetzt
Verworfen
Veraltet
```

### Entwurf

Das Dokument ist noch nicht verbindlich.

### In Prüfung

Das Dokument wird technisch geprüft.

### Angenommen

Die Entscheidung oder Spezifikation ist verbindlich.

### Implementiert

Die beschriebene Funktion ist im Code umgesetzt.

### Ersetzt

Ein neueres Dokument ersetzt dieses Dokument.

### Verworfen

Der Entwurf wurde nicht übernommen.

### Veraltet

Das Dokument ist historisch relevant, aber nicht mehr gültig.

---

## 10. Versionsregeln für Dokumente

Dokumentversionen verwenden:

```text
Major.Minor
```

Beispiele:

```text
0.1 – früher Entwurf
0.9 – Freigabekandidat
1.0 – erste angenommene Version
1.1 – kompatible Erweiterung
2.0 – grundlegende Änderung
```

Eine Major-Erhöhung ist erforderlich bei:

- geänderter Semantik
- inkompatibler API
- geänderter ABI
- geänderten Sicherheitsgarantien
- geänderten Dateiformaten

---

## 11. Querverweise

Dokumente sollen nach Möglichkeit Querverweise enthalten.

Beispiel:

```text
NPA-0001
    -> RFC-0001
    -> ADR-0002
    -> NPSPEC-0001
    -> NIS-0001
    -> DES-2008
    -> TST-2008
```

Querverweise sollen Dokument-ID und Titel enthalten.

---

## 12. Verbindlichkeit

Die Verbindlichkeit steigt grundsätzlich in dieser Reihenfolge:

```text
ROADMAP
    <
ADR
    <
RFC
    <
NPSPEC / NIS / NFD
    <
implementierte und getestete API
```

NPA-0001 besitzt übergreifende Verbindlichkeit.

Bei Konflikten gilt:

1. Sicherheitsdokument
2. aktuelle Spezifikation
3. aktuelle Interface Specification
4. aktuelles RFC
5. aktuelles ADR
6. Detaildesign
7. Roadmap

Konflikte müssen durch ein neues Dokument oder eine dokumentierte Revision aufgelöst werden.

---

## 13. Dokumentverzeichnis im Repository

```text
docs/
├── architecture/
│   └── NPA/
├── index/
│   └── NAI/
├── rfc/
├── adr/
│   ├── foundation/
│   ├── boot/
│   ├── kernel/
│   ├── hal/
│   ├── graphics/
│   ├── ui/
│   ├── storage/
│   ├── applications/
│   ├── sdk/
│   └── build/
├── specification/
│   └── NPSPEC/
├── design/
│   └── DES/
├── api/
├── interfaces/
│   └── NIS/
├── fileformats/
│   └── NFD/
├── security/
│   └── SEC/
├── tests/
│   └── TST/
├── style/
│   └── STYLE/
├── handbook/
│   └── DAH/
├── diagrams/
└── roadmap/
```

---

## 14. Noch zu erstellende Kerndokumente

Priorität 1:

```text
NAI-0002 – Dependency Matrix
NAI-0003 – Complete Module Tree
NAI-0004 – Boot Flow Index
NAI-0005 – Data Flow Index
NAI-0006 – Glossary

RFC-0002 – Object Manager
RFC-0003 – Collections Library
RFC-0004 – String System
RFC-0005 – Service Model

NPSPEC-0001 – Object System
NPSPEC-0002 – Component System
NPSPEC-1001 – BootInfo
NPSPEC-6001 – NVFS Path Semantics
```

Priorität 2:

```text
NIS-1001 – BootInfo ABI
NIS-2001 – Syscall ABI
NIS-8001 – Plugin ABI

NFD-0001 – NIMG
NFD-0002 – NFONT
NFD-0003 – NTHEME
NFD-0004 – NPAK

STYLE-0001 – C Coding Style
STYLE-0002 – Assembly Style
STYLE-0003 – Documentation Style
```

---

## 15. Statushistorie

- 2026-07-11: Version 0.1 als erster zentraler Architekturindex erstellt.
