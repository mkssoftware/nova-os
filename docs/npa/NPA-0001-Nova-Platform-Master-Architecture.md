# NPA-0001: Nova Platform Master Architecture

- **Status:** Angenommen
- **Version:** 0.1
- **Dokumenttyp:** Nova Platform Architecture
- **Erstellt:** 2026-07-11
- **Gültig für:** Nova Platform
- **Autor:** Nova Project
- **Ersetzt:** Keine
- **Verwandte Dokumente:** RFC-0001, RFC-0001.1, RFC-0001.2, ADR-0001 bis ADR-9005

---

## 1. Zweck

Dieses Dokument definiert die übergeordnete Architektur der Nova Platform.

Es beschreibt:

- die Plattformprodukte,
- die Kernschichten,
- die Verantwortungsgrenzen,
- die Abhängigkeitsregeln,
- die gemeinsamen Laufzeitdienste,
- den Boot- und Initialisierungsablauf,
- die Beziehungen zwischen Nova Rescue und NovaOS,
- sowie die verbindlichen Architekturprinzipien.

Dieses Dokument ist die zentrale technische Referenz für alle weiteren RFCs, ADRs, Spezifikationen, Detaildesigns und Implementierungen.

---

## 2. Plattformvision

Nova Platform ist keine einzelne Anwendung und kein isoliertes Betriebssystem.

Sie ist eine gemeinsame technische Plattform für:

- Nova Boot Manager,
- Nova Rescue,
- NovaOS,
- Nova Installer,
- Nova Deploy,
- zukünftige Nova-Produkte.

Alle Produkte verwenden dieselben grundlegenden Subsysteme, Bibliotheken, Dienste, Treiber und Werkzeuge, soweit ihre jeweilige Laufzeitumgebung dies erlaubt.

---

## 3. Hauptziele

Nova Platform verfolgt folgende Ziele:

1. Wiederverwendbarkeit
2. Klare Modulgrenzen
3. Stabile APIs und ABIs
4. Hohe Datenintegrität
5. Gute Diagnosefähigkeit
6. Schrittweise Erweiterbarkeit
7. Hardwareunabhängigkeit höherer Schichten
8. Gemeinsame Codebasis für Nova Rescue und NovaOS
9. Lokale und datenschutzfreundliche Systemdienste
10. Langfristige Wartbarkeit

---

## 4. Nicht-Ziele der ersten Plattformversion

Die erste Plattformversion muss nicht sofort bereitstellen:

- vollständige POSIX-Kompatibilität,
- vollständige Windows-Kompatibilität,
- vollständige GPU-Beschleunigung,
- vollständige Netzwerkunterstützung,
- vollständige Unterstützung aller Dateisysteme,
- dynamische Kernelmodule von Drittanbietern,
- vollständige Multicore-Optimierung,
- vollständige Benutzer- und Rechteverwaltung.

Diese Funktionen können später ergänzt werden, müssen jedoch die in diesem Dokument definierten Schichtgrenzen einhalten.

---

## 5. Plattformprodukte

### 5.1 Nova Boot Manager

Der Nova Boot Manager ist für den Startpfad verantwortlich.

Aufgaben:

- Firmwareübergang von BIOS oder UEFI,
- Laden des Boot Core,
- Hardwaregrundinformationen erfassen,
- Bootkonfiguration lesen,
- Bootmenü darstellen,
- Kernel und Module laden,
- BootInfo erzeugen,
- kontrollierte Übergabe an den Kernel.

Der Boot Manager enthält keine anwendungsspezifischen Funktionen aus Explorer, Backup oder Restore.

---

### 5.2 Nova Rescue

Nova Rescue ist eine eigenständige Rettungs- und Wartungsumgebung.

Kernfunktionen:

- Dateizugriff,
- Backup,
- Restore,
- Hardwarediagnose,
- Partitionierungsanalyse,
- Bootreparatur,
- Secure Wipe,
- Log- und Panic-Analyse,
- spätere Wiederherstellung beschädigter NovaOS-Installationen.

Nova Rescue muss unabhängig von einer funktionsfähigen NovaOS-Installation starten können.

---

### 5.3 NovaOS

NovaOS ist das vollständige Desktop-Betriebssystem.

Zusätzliche Funktionen gegenüber Nova Rescue:

- Benutzerkonten,
- dauerhafte Systemdienste,
- Desktop-Umgebung,
- Softwareverwaltung,
- Benachrichtigungen,
- Multimedia,
- lokale KI-Dienste,
- Anwendungsplattform.

---

### 5.4 Nova Installer

Der Nova Installer installiert NovaOS und Nova Rescue.

Aufgaben:

- Zielsystem analysieren,
- Partitionen vorbereiten,
- Dateien übertragen,
- Boot Manager installieren,
- Recovery-Umgebung einrichten,
- erste Konfiguration schreiben.

---

## 6. Gesamtarchitektur

```text
+------------------------------------------------------------+
| Anwendungen                                                |
| Explorer, Backup, Restore, Hardware, Settings, Terminal     |
+------------------------------------------------------------+
| Nova Development Kit / öffentliche APIs                    |
+------------------------------------------------------------+
| Nova User Interface Framework                              |
+------------------------------------------------------------+
| Nova Graphics Engine                                       |
+------------------------------------------------------------+
| Nova Core Services                                         |
| Storage, NVFS, Assets, Themes, Tasks, Logging, IPC          |
+------------------------------------------------------------+
| Nova Runtime                                               |
| Objects, Components, Events, Services, Tasks, Messages      |
+------------------------------------------------------------+
| Nova Kernel                                                |
| Memory, Interrupts, Timer, Scheduler, Processes, Syscalls   |
+------------------------------------------------------------+
| Nova Hardware Abstraction Layer                            |
+------------------------------------------------------------+
| Gerätetreiber                                              |
+------------------------------------------------------------+
| Hardware                                                   |
+------------------------------------------------------------+
```

---

## 7. Foundation

Die Foundation ist die gemeinsame Basis aller Schichten.

Sie enthält:

- Basistypen,
- Statuscodes,
- Fehlerobjekte,
- Logging,
- Assertions,
- Speichergrundfunktionen,
- Collections,
- Strings,
- Versionierung,
- Object Model,
- Component System.

Die Foundation darf keine Abhängigkeit zu UI, Dateisystem oder konkreter Hardware besitzen.

---

## 8. Nova Runtime

Die Nova Runtime stellt gemeinsame Laufzeitmechanismen bereit.

Subsysteme:

- Object Manager,
- Component Manager,
- Event Manager,
- Message Bus,
- Service Registry,
- Task Manager,
- Plugin Runtime,
- Modulregistrierung.

Die Runtime ist die Verbindung zwischen Kernelmechanismen und höherwertigen Diensten.

---

## 9. Nova Kernel

Der Kernel verwaltet grundlegende Systemressourcen.

Verantwortlichkeiten:

- physischer Speicher,
- virtueller Speicher,
- Kernel-Heap,
- Interrupts,
- Timer,
- Scheduler,
- Threads,
- Prozesse,
- Synchronisation,
- Systemaufrufe,
- Panic Handling.

Der Kernel kennt keine Explorer-, Backup- oder UI-spezifische Geschäftslogik.

---

## 10. Hardware Abstraction Layer

Die HAL trennt konkrete Treiber von Kernel und Diensten.

Beispiel:

```text
Storage Service
    |
    v
Storage HAL
    |
    +-- AHCI-Treiber
    +-- NVMe-Treiber
    +-- USB-Massenspeicher
```

Höhere Schichten dürfen konkrete Treiber nicht direkt ansprechen.

---

## 11. Core Services

Core Services stellen systemweite Funktionen bereit.

Geplante Dienste:

- Storage Service,
- NVFS Service,
- Asset Service,
- Theme Service,
- Logging Service,
- Task Service,
- Notification Service,
- Security Service,
- Plugin Service,
- IPC Service.

Services werden über öffentliche, versionierte Schnittstellen registriert und verwendet.

---

## 12. Nova Graphics Engine

Die Graphics Engine stellt Renderingfunktionen bereit.

Subsysteme:

- Framebuffer,
- Surfaces,
- Renderer,
- Primitive,
- Bilder,
- Fonts,
- Clipping,
- Alpha-Blending,
- Effekte,
- spätere GPU-Backends.

Anwendungen zeichnen nicht direkt in den Framebuffer.

---

## 13. Nova User Interface Framework

Das UI Framework stellt interaktive Oberflächen bereit.

Subsysteme:

- Window Manager,
- Widgets,
- Layout Engine,
- Event Dispatch,
- Fokusverwaltung,
- Themes,
- Animationen,
- Drag & Drop,
- Clipboard,
- Accessibility.

NUI verwendet ausschließlich NGE und öffentliche Core Services.

---

## 14. Nova Storage System

Das Storage System umfasst:

- Geräteerkennung,
- Blockgeräte,
- Partitionstabellen,
- Volumes,
- NVFS,
- Dateisystemtreiber,
- Backup,
- Restore,
- Secure Wipe,
- Image-Verarbeitung.

Sicherheitsprinzip:

> In Nova Rescue hat Datenintegrität Vorrang vor Geschwindigkeit und Komfort.

Dateisysteme werden standardmäßig read-only eingebunden, sofern keine geprüfte Schreibimplementierung vorliegt.

---

## 15. Nova Asset System

Das Asset System verwaltet:

- Bilder,
- Fonts,
- Icons,
- Themes,
- Cursor,
- Lokalisierungsdaten,
- spätere Soundressourcen.

Quellformate werden durch Host-Werkzeuge in optimierte Nova-Formate umgewandelt.

Beispiele:

- PNG → NIMG
- TTF/OTF → NFONT
- SVG → NICON oder NVG
- Theme-Quelle → NTHEME
- mehrere Assets → NPAK

---

## 16. Nova Object Model

Verwaltete Laufzeitentitäten verwenden NovaObject.

NovaObject liefert:

- Identität,
- Typ,
- Zustand,
- Flags,
- Name,
- Lebenszyklus,
- optionale Callbacks.

Nicht jede Struktur ist ein Objekt.

Werttypen wie NovaRect oder NovaColor bleiben einfache Strukturen.

---

## 17. Nova Component System

Komponenten liefern Fähigkeiten.

Beispiel:

```text
Button
├── Transform
├── Render
├── Input
├── Focus
├── Theme
└── Animation
```

NOM beantwortet:

> Was ist dieses Objekt?

NCS beantwortet:

> Welche Fähigkeiten besitzt dieses Objekt?

---

## 18. Service- und Nachrichtenmodell

Nova verwendet zwei Kommunikationsformen.

### 18.1 Direkte Service-API

Für:

- synchrone Operationen,
- performancekritische Pfade,
- klar typisierte Funktionen.

### 18.2 Message Bus

Für:

- asynchrone Operationen,
- Events,
- lose gekoppelte Module,
- Tasks,
- Plugins,
- spätere IPC-Kommunikation.

---

## 19. Abhängigkeitsregeln

Erlaubt:

```text
Explorer
    -> Nova SDK
    -> NVFS Service
    -> NVFS
    -> Dateisystemtreiber
    -> Storage Service
    -> HAL
    -> Gerätetreiber
```

Nicht erlaubt:

```text
Explorer -> NVMe-Treiber
```

Nicht erlaubt:

```text
Button -> SATA-Controller
```

Nicht erlaubt:

```text
Dateisystemtreiber -> Explorer
```

Zyklische Abhängigkeiten sind nicht zulässig.

---

## 20. Bootablauf

```text
Firmware
    |
    v
BIOS- oder UEFI-Frontend
    |
    v
Boot Core
    |
    v
Bootkonfiguration und Assets
    |
    v
Nova Boot Manager
    |
    v
Kernel Loader
    |
    v
BootInfo
    |
    v
Kernel Entry
    |
    v
Foundation
    |
    v
Memory
    |
    v
Interrupts und Timer
    |
    v
HAL und Treiber
    |
    v
Core Services
    |
    v
Graphics und UI
    |
    v
Nova Rescue oder NovaOS
```

---

## 21. Kernel-Initialisierungsreihenfolge

1. BootInfo validieren
2. frühes Logging initialisieren
3. CPU-Grundzustand prüfen
4. Memory Map übernehmen
5. Physical Memory Manager starten
6. Virtual Memory Manager starten
7. Kernel Heap initialisieren
8. Interruptsystem laden
9. Timer initialisieren
10. Scheduler vorbereiten
11. HAL initialisieren
12. Treiber registrieren
13. Core Services starten
14. Graphics Engine starten
15. Asset- und Theme-System starten
16. UI Framework starten
17. Produktanwendung starten

---

## 22. Fehlerbehandlung

Nova trennt:

- NovaStatus,
- NovaError,
- NovaErrorObject,
- Kernel Panic.

### NovaStatus

Kompakter Rückgabewert.

### NovaError

Strukturierte Fehlerbeschreibung.

### NovaErrorObject

Verwalteter, transportierbarer Fehler.

### Kernel Panic

Nicht wiederherstellbarer Kernelzustand.

Panic Reports können später von Nova Rescue analysiert werden.

---

## 23. Sicherheitsprinzipien

1. Alle externen Daten gelten als nicht vertrauenswürdig.
2. Offsets, Größen und Prüfsummen werden validiert.
3. Schreibende Datenträgeroperationen benötigen eindeutige Zielbestätigung.
4. Gefährliche Operationen werden protokolliert.
5. Plugins erhalten keinen ungeprüften Kernelzugriff.
6. Öffentliche APIs validieren Strukturgröße und ABI-Version.
7. Read-only ist in Rescue der Standardzustand.

---

## 24. API- und ABI-Regeln

- Öffentliche Enums besitzen explizite Werte.
- Öffentliche Strukturen enthalten StructSize und AbiVersion.
- Neue Felder werden am Ende ergänzt.
- Entfernte Enum-Werte bleiben reserviert.
- Persistente Formate verwenden keine Speicherzeiger.
- Endianness wird pro Format festgelegt.
- APIs verwenden Modulpräfixe.

---

## 25. Repositorystruktur

```text
NovaPlatform/
├── boot/
├── foundation/
├── kernel/
├── hal/
├── drivers/
├── runtime/
├── services/
├── libs/
│   ├── nge/
│   ├── nui/
│   ├── nas/
│   ├── nvfs/
│   ├── ncrypto/
│   └── ncompress/
├── applications/
├── sdk/
├── assets/
├── generated/
├── tools/
├── tests/
├── docs/
└── build/
```

---

## 26. Dokumentationssystem

Nova verwendet folgende Dokumenttypen:

- NPA – Master Architecture
- NAI – Architecture Index
- RFC – Architekturregeln
- ADR – Architekturentscheidungen
- NPSPEC – Verhaltensspezifikationen
- DES – Detaildesign
- API – öffentliche API-Dokumentation
- NIS – Interface Specification
- NFD – File Format Documentation
- SEC – Sicherheitsrichtlinien
- TST – Testkonzepte
- STYLE – Coding- und Designrichtlinien
- ROADMAP – Meilensteine und Planung
- DAH – Developer Architecture Handbook

---

## 27. Entwicklungsprozess

Für neue Subsysteme gilt:

1. Problem beschreiben
2. RFC oder Änderungs-RFC erstellen
3. ADR für zentrale Entscheidungen verfassen
4. Spezifikation definieren
5. öffentliche API festlegen
6. Detaildesign erstellen
7. Tests definieren
8. Implementierung durchführen
9. Integration testen
10. Dokumentation aktualisieren

---

## 28. Plattformmeilensteine

### Nova Platform 0.1

- Bootfähig
- Kernel Entry
- Logging
- Memory-Grundlagen
- Framebuffer
- einfache UI

### Nova Platform 0.2

- Input
- Window Manager
- Asset System
- Theme System
- Tasks

### Nova Platform 0.3

- Storage Manager
- FAT32
- NVFS
- einfacher Explorer

### Nova Platform 0.4

- Backup
- Restore
- Hardwarediagnose
- Secure Wipe

### Nova Rescue 1.0

- vollständige Rettungsumgebung

### NovaOS 1.0

- vollständige Desktop-Plattform

---

## 29. Verbindliche Architekturgrundsätze

1. Datenintegrität vor Geschwindigkeit.
2. Korrektheit vor optischen Effekten.
3. Öffentliche API vor direkter interner Kopplung.
4. Komposition vor tiefer Vererbung.
5. Services vor direkten Modulabhängigkeiten.
6. Hardwarezugriff nur über HAL und Treiber.
7. Anwendungen sprechen keine Kernelinternas an.
8. Wiederverwendung zwischen Nova Rescue und NovaOS.
9. Änderungen an ABI oder Formaten werden versioniert.
10. Dokumentation und Tests gehören zur Implementierung.

---

## 30. Offene Punkte

Folgende Punkte benötigen weitere Spezifikationen:

- genaue BootInfo-Struktur,
- ObjectID-Bitlayout,
- Service- und Message-Formate,
- Speicherverwaltung,
- Syscall-ABI,
- Prozessmodell,
- Sicherheitsmodell,
- NIMG- und NFONT-Formate,
- Plugin-Binärformat,
- NVFS-Pfadregeln,
- Backup-Containerformat.

---

## 31. Statushistorie

- 2026-07-11: Version 0.1 als erster Master-Architecture-Entwurf erstellt.
