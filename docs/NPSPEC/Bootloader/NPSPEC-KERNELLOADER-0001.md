# NPSPEC-KERNELLOADER-0001: Nova Kernel Loader

- **Status:** Angenommen
- **Version:** 1.1
- **Kategorie:** Boot Architecture
- **Erstellt:** 2026-07-23
- **Gültig für:** Nova Bootloader, Nova Kernel und Nova OS
- **Ersetzt:** Keine

---

# 1. Einleitung

Diese Spezifikation definiert den **Nova Kernel Loader (NKL)**.

Der Kernel Loader bildet die Schnittstelle zwischen dem Bootloader und dem eigentlichen Nova-Kernel.

Seine Hauptaufgabe besteht darin,

- den Kernel zu finden,
- zu validieren,
- vollständig in den Arbeitsspeicher zu laden,
- die Bootumgebung vorzubereiten und
- die Kontrolle sicher an den Kernel zu übergeben.

Der Kernel Loader ist ausschließlich für den frühen Startprozess verantwortlich.

---

# 2. Zielsetzung

Der Nova Kernel Loader soll:

- BIOS und UEFI unterstützen
- unabhängig vom Dateisystem arbeiten
- mehrere Kernelversionen verwalten
- sichere Kernelstarts ermöglichen
- Recovery unterstützen
- Rollbacks ermöglichen
- zukünftige Mehrkernsysteme vorbereiten
- reproduzierbare Bootvorgänge gewährleisten

---

# 3. Architekturübersicht

```text
Firmware

↓

BIOS / UEFI

↓

Nova Bootloader

↓

Nova Kernel Loader

↓

Nova Kernel

↓

Init System

↓

Userspace
```

Der Kernel Loader stellt die letzte Bootloader-Komponente vor dem Kernel dar.

---

# 4. Verantwortlichkeiten

Der Kernel Loader übernimmt folgende Aufgaben:

- Kernel auswählen
- Kerneldatei lesen
- Kernel validieren
- Kernel in den Speicher laden
- Speicherlayout vorbereiten
- Bootinformationen erzeugen
- Kernelparameter übergeben
- CPU vorbereiten
- Kontrolle an den Kernel übergeben

Die grundlegende Aufgabe eines Betriebssystem-Loaders besteht darin, den Kernel zu lokalisieren, in den Speicher zu laden und anschließend die Ausführung an den Kernel zu übergeben. :contentReference[oaicite:0]{index=0}

---

# 5. Unterstützte Kernelformate

Der Kernel Loader MUSS zunächst folgende Produktionsformate erkennen, validieren und laden können:

- Nova Kernel Image (NKI)
- ELF32
- ELF64

Zusätzlich DARF er unterstützen:

- Raw Binary (Debug)

Langfristig können weitere Formate ergänzt werden.

Das bevorzugte Produktionsformat ist:

**Nova Kernel Image (NKI)**

NKI bleibt das native Nova-Verteilungs- und Produktionsformat, weil es Versionierung, Metadaten, Kompression, Prüfsummen und Signaturen gemeinsam kapselt.

ELF32 und ELF64 sind dennoch vollwertige, direkt ladbare Kernelformate. Der Loader darf eine gültige ELF-Datei nicht allein deshalb ablehnen, weil sie nicht in einen NKI-Container eingebettet ist. Bei ELF werden ELF-Header, Program Header, Architektur, Endianness, Segmente, Speichergrenzen, Alignment und Einstiegspunkt validiert.

Die Formaterkennung MUSS über eindeutige Magic-Werte erfolgen. Ein Format darf nicht anhand des Dateinamens oder der Dateiendung angenommen werden.

---

# 6. Kernel Image (NKI)

Ein NKI besteht aus folgenden Komponenten:

```text
NKI

├── Header
├── Kernel Image
├── Metadata
├── Symbolinformationen (optional)
├── Signatur
└── Prüfsummen
```

Der Header enthält sämtliche Informationen, die zum Laden erforderlich sind.

Das im NKI enthaltene Kernel-Image kann ein Nova-eigenes Payload oder ein ELF32-/ELF64-Image sein. Ist das Payload als ELF gekennzeichnet, MUSS nach erfolgreicher NKI-Validierung zusätzlich die vollständige ELF-Validierung und das segmentbasierte Laden ausgeführt werden.

---

# 6.1 Direkte ELF-Unterstützung

Der Kernel Loader MUSS ELF32 und ELF64 direkt verstehen und laden können.

Mindestens zu prüfen sind:

- ELF-Magic und ELF-Version
- ELF-Klasse
- Little-Endian-Kodierung für die aktuelle Referenzplattform
- Zielarchitektur
- Dateityp
- Program-Header-Größe und -Anzahl
- alle Datei- und Speichergrenzen
- `PT_LOAD`-Segmente
- `p_filesz <= p_memsz`
- Segmentausrichtung und Segmentüberlappungen
- BSS-Nullinitialisierung
- ausführbarer und gültiger Einstiegspunkt

Für den initialen Loader sind `ELF32`, `ET_EXEC`, `EM_386` und `PT_LOAD` verpflichtend. ELF64 wird entsprechend der verfügbaren 64-Bit-Loaderphase unterstützt. Nicht unterstützte zwingende ELF-Eigenschaften führen zu einem kontrollierten Ladefehler.

---

# 7. NKI-Header

Der Header enthält mindestens:

- Magic Number
- Version
- CPU-Architektur
- Kernel-Version
- Build-ID
- Entry Point
- Image Size
- Compression
- Signature
- Feature Flags

```text
NKI Header

├── Magic
├── Version
├── Architecture
├── Entry Point
├── Image Size
├── Compression
├── Flags
└── Signature
```

---

# 8. Kernel-Auswahl

Der Kernel Loader unterstützt:

- Standardkernel
- Backupkernel
- Recoverykernel
- Testkernel
- Entwicklerkernel

Beispiel:

```text
Kernel List

↓

Stable

↓

Recovery

↓

Experimental

↓

Developer
```

Der Benutzer kann im Bootmenü den gewünschten Kernel auswählen.

---

# 9. Speicherinitialisierung

Vor dem Start des Kernels bereitet der Kernel Loader den Arbeitsspeicher vor.

Hierzu gehören:

- Speicherkarte übernehmen
- reservierte Bereiche markieren
- Bootloader-Speicher reservieren
- Kernel-Speicher reservieren
- Framebuffer reservieren
- ACPI-/UEFI-Daten übernehmen

Ein Bootloader muss mindestens den RAM initialisieren und eine geeignete Speicherumgebung schaffen, bevor die Kontrolle an den Kernel übergeben wird. :contentReference[oaicite:1]{index=1}

---

# 10. Kernel laden

Der Ladevorgang erfolgt in mehreren Schritten.

```text
Kernel finden

↓

Image lesen

↓

Header prüfen

↓

Signatur prüfen

↓

Speicher reservieren

↓

Kernel laden

↓

Validierung

↓

Startbereit
```

---

# 11. Kompression

Der Kernel Loader unterstützt komprimierte Kernel.

Unterstützte Verfahren:

- NONE
- LZ4
- ZSTD
- GZIP

Standard:

**ZSTD**

Ist ein Kernel komprimiert, wird er vor der Ausführung in den Zielspeicher entpackt. Viele Bootloader übernehmen diese Aufgabe für komprimierte Kernelabbilder. :contentReference[oaicite:2]{index=2}

---

# 12. Kernelparameter

Der Kernel Loader übergibt strukturierte Bootparameter.

Beispiele:

- Speicherkarte
- Framebuffer
- Bootgerät
- Bootmodus
- Bootoptionen
- Kernelversion
- Recovery-Modus
- Secure-Boot-Status
- TPM-Status

---

# 13. Boot Information Block (BIB)

Alle Informationen werden gemäß NBHP in einem zusammenhängenden, versionierten und TLV-basierten Boot Information Block gespeichert.

```text
NBHP/BIB

├── Header
│   ├── Magic
│   ├── Version
│   ├── Headergröße
│   ├── Gesamtgröße
│   ├── Prüfsumme
│   ├── Architektur
│   └── Flags
│
└── TLV-Einträge
    ├── Firmware
    ├── Memory
    ├── Graphics
    ├── Kernel
    ├── Security
    └── Boot Options
```

Der Kernel erhält ausschließlich einen Zeiger auf diesen Block.

---

# 14. Sicherheitsprüfung

Vor dem Start werden mindestens geprüft:

- Imagegröße
- Header
- Prüfsumme
- digitale Signatur
- Feature-Kompatibilität
- Architektur
- Kernelversion

Bei einem Fehler wird der Kernel nicht gestartet.

---

# 15. Recovery

Kann der Standardkernel nicht gestartet werden:

```text
Kernel

↓

Fehler

↓

Backup Kernel

↓

Recovery Kernel

↓

Bootmenü
```

Dadurch bleibt das System auch nach fehlgeschlagenen Updates startfähig.

---

# 16. Mehrkernsysteme

Initial startet ausschließlich die Bootstrap-CPU.

Weitere Prozessoren werden erst durch den Kernel aktiviert.

```text
CPU0

↓

Kernel Start

↓

Kernel Initialisierung

↓

CPU1

↓

CPU2

↓

CPU3
```

---

# 17. Architekturunabhängigkeit

Der Kernel Loader trennt:

- Firmware
- CPU-Architektur
- Dateisystem
- Kernelformat

Dadurch können zukünftige Architekturen ergänzt werden.

Beispiele:

- x86
- x86-64
- ARM64
- RISC-V

---

# 18. Fehlerbehandlung

Tritt ein Fehler auf:

```text
Kernel laden

↓

Fehler

↓

Log erzeugen

↓

Recovery

↓

Bootmenü
```

Alle Fehler werden protokolliert.

---

# 19. Schnittstelle zum Kernel

Der Kernel Loader übergibt:

- Boot Information Block
- Einstiegspunkt
- CPU-Zustand
- Stack
- Speicherlayout

Danach endet seine Verantwortung vollständig.

---

# 20. Positive Konsequenzen

Die Architektur bietet zahlreiche Vorteile.

## Klare Trennung

Bootloader und Kernel bleiben vollständig entkoppelt.

---

## Mehrere Kernel

Mehrere Kernelversionen können parallel installiert werden.

---

## Sichere Updates

Neue Kernel können getestet werden, ohne den alten Kernel zu entfernen.

---

## Recovery

Automatische Rollbacks bleiben möglich.

---

## Erweiterbarkeit

Neue Kernelformate und Architekturen können ergänzt werden.

---

# 21. Negative Konsequenzen und Risiken

## Höhere Komplexität

Der Kernel Loader übernimmt deutlich mehr Aufgaben als ein minimalistischer Loader.

---

## Größere Testmatrix

Alle unterstützten Kernelformate und Architekturen müssen validiert werden.

---

## Signaturverwaltung

Digitale Signaturen und Schlüssel benötigen eine langfristige Verwaltung.

---

# 22. Auswirkungen auf andere Module

Diese Spezifikation betrifft insbesondere:

- Nova Bootloader
- Boot Manager
- Recovery Manager
- Security Manager
- Memory Manager
- Nova Kernel
- Installer
- Update Manager
- Boot Configuration Manager

Alle zukünftigen Kernelstarts MÜSSEN über den Nova Kernel Loader erfolgen.

---

# 23. Umsetzungsregeln

Für den Nova Kernel Loader gelten folgende verbindliche Regeln:

- Der Kernel Loader MUSS vollständig vom Kernel getrennt sein.
- Der Kernel Loader MUSS mehrere Kernelversionen unterstützen.
- NKI MUSS als bevorzugtes Nova-Produktionsformat unterstützt werden.
- Der Kernel Loader MUSS gültige ELF32- und ELF64-Kernel direkt erkennen, validieren und laden können.
- Ein NKI mit ELF-Payload MUSS sowohl als NKI als auch als ELF validiert werden.
- Die Formaterkennung MUSS über Magic-Werte und DARF NICHT ausschließlich über Dateiendungen erfolgen.
- Jeder Kernel MUSS vor dem Start validiert werden.
- Der Boot Information Block MUSS versioniert sein.
- Alle Kernelparameter MÜSSEN strukturiert übergeben werden.
- Der Kernel Loader MUSS BIOS und UEFI unterstützen.
- Komprimierte Kernel SOLLEN unterstützt werden.
- Recovery-Kernel MÜSSEN unabhängig vom Standardkernel startbar sein.
- Nach der Übergabe der Kontrolle DARF der Kernel Loader keinen weiteren Code ausführen.

---

# 24. Referenzen

- NPSPEC-BOOT-0001
- NPSPEC-BOOTFS-0001
- NPSPEC-NOVAFS-0001
- ADR-6031 – TPM als optionale Sicherheitskomponente
- ADR-6039 – On-Disk-Kompatibilität und Feature-Flags
- Linux boot(7) – Boot Sequence Overview :contentReference[oaicite:3]{index=3}
- Linux Kernel Documentation – x86 Boot Protocol :contentReference[oaicite:4]{index=4}
- Linux Kernel Documentation – ARM64 Boot Requirements :contentReference[oaicite:5]{index=5}
- Android Bootloader Overview :contentReference[oaicite:6]{index=6}

---

# 25. Statushistorie

- 2026-07-29: Version 1.1 – NKI als bevorzugtes Produktionsformat bestätigt und direkte ELF32-/ELF64-Unterstützung verbindlich präzisiert.

- **2026-07-23:** Version 1.0 erstellt.
- **2026-07-23:** Nova Kernel Loader als verbindliche Kernel-Ladeschnittstelle für Nova OS definiert.
- **Zukünftige Änderungen:** Nur durch eine neue Version dieser Spezifikation oder eine ersetzende NPSPEC zulässig.
