# NPSPEC-BOOTHANDOFF-0001: Boot Handoff Protocol

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Boot Architecture
- **Erstellt:** 2026-07-23
- **Gültig für:** Nova Bootloader, Nova Kernel Loader und Nova Kernel
- **Ersetzt:** Keine

---

# 1. Einleitung

Diese Spezifikation definiert das **Nova Boot Handoff Protocol (NBHP)**.

Das Boot Handoff Protocol beschreibt die standardisierte Übergabe sämtlicher Informationen zwischen dem Nova Kernel Loader und dem Nova Kernel.

Der Kernel darf niemals direkt auf interne Datenstrukturen des Bootloaders zugreifen.

Stattdessen erhält der Kernel genau einen Zeiger auf einen standardisierten Boot Information Block.

Dieses Vorgehen orientiert sich an etablierten Boot-Protokollen, bei denen der Bootloader eine definierte Datenstruktur aufbaut und anschließend die Kontrolle an den Kernel übergibt. :contentReference[oaicite:0]{index=0}

---

# 2. Zielsetzung

Das Boot Handoff Protocol soll:

- BIOS und UEFI unterstützen
- architekturunabhängig sein
- versionierbar sein
- vollständig dokumentiert sein
- binär kompatibel bleiben
- zukünftige Erweiterungen ermöglichen
- keine Bootloader-Interna offenlegen
- deterministische Kernelstarts ermöglichen

---

# 3. Architektur

```text
Firmware

↓

Nova Bootloader

↓

Nova Kernel Loader

↓

Boot Information Block

↓

Nova Kernel

↓

Kernel Initialisierung
```

Der Kernel kommuniziert ausschließlich über den Boot Information Block.

---

# 4. Verantwortlichkeiten

## Bootloader

Der Bootloader ist verantwortlich für:

- Firmwareinitialisierung
- Kernel laden
- Speicher vorbereiten
- Hardwareinformationen sammeln
- Boot Information Block erzeugen
- Kontrolle übergeben

---

## Kernel

Der Kernel übernimmt anschließend:

- Initialisierung
- Speicherverwaltung
- Treiberinitialisierung
- Scheduler
- Prozessverwaltung

Die Verantwortung des Bootloaders endet unmittelbar nach der Übergabe.

---

# 5. Übergabepunkt

Der Übergabepunkt besitzt exakt folgende Reihenfolge:

```text
Kernel vollständig geladen

↓

Boot Information Block erzeugt

↓

CPU vorbereitet

↓

Interrupts deaktiviert

↓

Register setzen

↓

Jump zum Kernel Entry Point
```

Nach diesem Sprung darf der Bootloader keinen weiteren Code mehr ausführen.

---

# 6. Registerübergabe

Beim Kernelstart gelten folgende Register:

| Register | Bedeutung |
|----------|-----------|
| RIP/EIP | Kernel Entry Point |
| RSP/ESP | Kernel Stack |
| RDI/EBX* | Zeiger auf Boot Information Block |
| CR3 | Aktive Seitentabellen (falls Paging aktiv) |

\* Die konkrete Registerbelegung ist architekturabhängig.

---

# 7. Boot Information Block

Der Boot Information Block (BIB) ist die einzige definierte Kommunikationsschnittstelle.

```text
Boot Information Block

├── Header
├── Firmware
├── CPU
├── Memory
├── Graphics
├── Storage
├── Kernel
├── Security
├── ACPI
├── Device Tree
├── Boot Options
├── Modules
├── Timing
└── Reserved
```

---

# 8. Header

Jeder Boot Information Block beginnt mit einem Header.

```text
Header

├── Magic Number
├── Version
├── Size
├── Checksum
├── Architecture
├── Flags
└── Reserved
```

Die Magic Number dient der sicheren Erkennung des Datenformats.

---

# 9. Firmwareinformationen

Folgende Firmwaredaten werden übergeben:

- BIOS
- UEFI
- Firmware-Version
- Bootmodus
- Secure Boot Status
- Firmware Vendor
- Runtime Services verfügbar

---

# 10. CPU-Informationen

Der Kernel erhält unter anderem:

- CPU-Hersteller
- Architektur
- Family
- Model
- Stepping
- Anzahl Kerne
- Threads
- APIC
- TSC
- unterstützte Features

---

# 11. Speicherinformationen

Übergeben werden:

- vollständige Memory Map
- reservierte Bereiche
- Kernelbereich
- Framebuffer
- ACPI
- MMIO
- DMA-Bereiche

```text
Memory Map

↓

Available

Reserved

ACPI

Framebuffer

MMIO

Kernel
```

Eine vollständige Speicherkarte gehört zu den grundlegenden Informationen, die ein Bootloader dem Kernel zur Verfügung stellen muss. :contentReference[oaicite:1]{index=1}

---

# 12. Grafik

Der Bootloader übergibt:

- Framebuffer-Adresse
- Breite
- Höhe
- Farbtiefe
- Pitch
- Pixelformat
- Monitorinformationen (optional)

Dadurch kann der Kernel sofort grafische Ausgaben erzeugen.

---

# 13. Bootmedium

Folgende Informationen werden übergeben:

- Bootgerät
- Partitions-ID
- GPT UUID
- Storage-Typ
- Dateisystem
- Kernelpfad

---

# 14. Kernelinformationen

Der Kernel Loader ergänzt:

- Kernelversion
- Build-ID
- Imagegröße
- Kompressionsalgorithmus
- Entry Point
- Feature Flags

---

# 15. Sicherheitsinformationen

Übergeben werden:

- Secure Boot
- TPM Status
- Trust Backend
- Signaturstatus
- Kernelvalidierung
- Recovery-Modus

---

# 16. Bootoptionen

Der Bootloader übergibt:

- Kernelparameter
- Debugmodus
- Safe Mode
- Recovery
- Logging
- Verbosity
- Testmodus

---

# 17. Module

Zusätzliche Bootmodule können registriert werden.

```text
Module

↓

InitRAMFS

↓

Recovery

↓

Treiber

↓

Debug

↓

Diagnose
```

Die Reihenfolge bleibt deterministisch.

---

# 18. Erweiterbarkeit

Der Boot Information Block verwendet TLV-Einträge (Type-Length-Value).

```text
Entry

├── Type
├── Length
└── Data
```

Neue Einträge können ergänzt werden, ohne ältere Kernel zu verändern.

Dieses Prinzip ähnelt modernen Boot-Protokollen, die neue Informationen über versionierte oder erweiterbare Datenstrukturen bereitstellen. :contentReference[oaicite:2]{index=2}

---

# 19. Versionierung

Das Handoff-Protokoll besitzt eine eigene Version.

```text
Major

Minor

Patch
```

Neue Felder dürfen ausschließlich Minor-Versionen erweitern.

Breaking Changes erfordern eine neue Major-Version.

---

# 20. Validierung

Vor der Übergabe prüft der Kernel Loader:

- Magic Number
- Versionsnummer
- Größe
- Prüfsumme
- Pointer
- Alignment
- Speicherbereiche

Ungültige Strukturen verhindern den Kernelstart.

---

# 21. Fehlerbehandlung

Fehler während der Übergabe:

```text
BIB erzeugen

↓

Validierung

↓

Fehler

↓

Recovery

↓

Bootmenü
```

Ein fehlerhafter Boot Information Block darf niemals an den Kernel übergeben werden.

---

# 22. Positive Konsequenzen

Die Architektur bietet zahlreiche Vorteile.

## Klare Schnittstelle

Bootloader und Kernel bleiben vollständig entkoppelt.

---

## Erweiterbarkeit

Neue Informationen können jederzeit ergänzt werden.

---

## Portabilität

Das Protokoll bleibt unabhängig von BIOS, UEFI oder CPU-Architektur.

---

## Stabilität

Der Kernel besitzt eine eindeutig definierte Startumgebung.

---

## Wartbarkeit

Alle Bootinformationen befinden sich an einer zentralen Stelle.

---

# 23. Negative Konsequenzen und Risiken

## Größerer Verwaltungsaufwand

Der Boot Information Block muss versioniert und gepflegt werden.

---

## Strikte Kompatibilität

Änderungen am Protokoll müssen langfristig unterstützt werden.

---

## Validierungsaufwand

Alle Einträge müssen vor der Übergabe geprüft werden.

---

# 24. Auswirkungen auf andere Module

Diese Spezifikation betrifft insbesondere:

- Nova Bootloader
- Nova Kernel Loader
- Nova Kernel
- Memory Manager
- Security Manager
- Recovery Manager
- ACPI Manager
- Device Manager
- Graphics Manager
- Nova SDK

Alle zukünftigen Bootinformationen MÜSSEN ausschließlich über das Boot Handoff Protocol an den Kernel übergeben werden.

---

# 25. Umsetzungsregeln

Für das Nova Boot Handoff Protocol gelten folgende verbindliche Regeln:

- Der Kernel MUSS genau einen Zeiger auf den Boot Information Block erhalten.
- Der Boot Information Block MUSS versioniert sein.
- Jede Struktur MUSS eine definierte Größe besitzen.
- Neue Einträge MÜSSEN rückwärtskompatibel ergänzt werden.
- Alle Speicheradressen MÜSSEN validiert werden.
- Der Bootloader DARF nach der Übergabe keinen Code mehr ausführen.
- Der Kernel DARF niemals auf interne Bootloader-Daten zugreifen.
- Alle Datenstrukturen MÜSSEN Little-Endian kodiert sein.
- Der Boot Information Block MUSS vollständig dokumentiert sein.

---

# 26. Referenzen

- NPSPEC-BOOT-0001
- NPSPEC-BOOTFS-0001
- NPSPEC-KERNELLOADER-0001
- ADR-6038 – API-Design von NovaFS
- Linux x86 Boot Protocol – Boot Parameters und Handover Protocol :contentReference[oaicite:3]{index=3}
- Linux Kernel Documentation – EFI Handover Protocol :contentReference[oaicite:4]{index=4}

---

# 27. Statushistorie

- **2026-07-23:** Version 1.0 erstellt.
- **2026-07-23:** Nova Boot Handoff Protocol als verbindliche Übergabeschnittstelle zwischen Bootloader und Kernel definiert.
- **Zukünftige Änderungen:** Nur durch eine neue Version dieser Spezifikation oder eine ersetzende NPSPEC zulässig.