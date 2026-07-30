# ADR-BOOT-1011: BIOS und UEFI parallel unterstützen

- **ADR-ID:** ADR-BOOT-1011
- **Status:** Accepted
- **Version:** 1.1
- **Datum:** 2026-07-24
- **Autor:** NovaOS Architektur
- **Kategorie:** Bootloader
- **Betroffene Komponenten:**
  - Bootloader Stage 1
  - Bootloader Stage 2
  - Kernel Loader
  - Hardware Abstraction Layer (HAL)
  - Build-System
- **Ersetzt:** Keine

---

# 1. Kontext

NovaOS soll ein vollständig eigenständiges Betriebssystem werden und sowohl auf älteren als auch auf modernen Rechnern lauffähig sein.

Aktuell existieren zwei Firmware-Standards:

- Legacy BIOS
- UEFI

Viele ältere Industrie-PCs, Embedded-Systeme und virtuelle Maschinen verwenden weiterhin BIOS, während nahezu alle aktuellen Desktop- und Notebook-Systeme ausschließlich UEFI unterstützen. UEFI hat BIOS als Standard weitgehend abgelöst, dennoch existiert in der Praxis weiterhin eine große installierte Basis von BIOS-Systemen.

Eine ausschließliche Unterstützung von UEFI würde einen Teil der Zielplattformen ausschließen. Eine ausschließliche Unterstützung von BIOS würde moderne Systeme nicht mehr vollständig unterstützen.

---

# 2. Problem

Es muss entschieden werden, ob NovaOS:

- ausschließlich BIOS unterstützt,
- ausschließlich UEFI unterstützt,
- oder beide Bootverfahren parallel unterstützt.

---

# 3. Entscheidung

**NovaOS unterstützt BIOS und UEFI parallel.**

Beide Bootpfade besitzen eine eigene Initialisierung.

Nach Abschluss der Firmware-spezifischen Initialisierung erfolgt die Übergabe an eine gemeinsame Boot-API.

Ab diesem Zeitpunkt arbeitet der restliche Kernel firmwareunabhängig.

---

# 4. Begründung

Diese Entscheidung bietet folgende Vorteile:

- maximale Hardwarekompatibilität
- Unterstützung alter Rechner
- Unterstützung moderner Systeme
- identischer Kernel
- vereinfachte Wartung
- zukünftige Erweiterbarkeit

Der größte Teil des Betriebssystems muss nicht wissen, ob der Rechner ursprünglich per BIOS oder UEFI gestartet wurde.

---

# 5. Zielarchitektur

```text
                 Firmware

        +----------------------+
        |                      |
   Legacy BIOS             UEFI Firmware
        |                      |
        |                      |
  BIOS Bootloader        UEFI Bootloader
        |                      |
        +----------+-----------+
                   |
          Boot Abstraction Layer
                   |
           Kernel Loader API
                   |
            Kernel Entry Point
                   |
          Hardware Abstraction Layer
                   |
               Nova Kernel
```

---

# 6. Bootpfad BIOS

```text
Power On

↓

BIOS

↓

MBR

↓

Bootloader Stage 1

↓

Bootloader Stage 2

↓

Protected Mode

↓

Kernel Loader

↓

Kernel Entry
```

---

# 7. Bootpfad UEFI

```text
Power On

↓

UEFI Firmware

↓

EFI Application

↓

Boot Services

↓

Memory Map

↓

ExitBootServices()

↓

Kernel Loader

↓

Kernel Entry
```

---

# 8. Gemeinsame Kernel-API

Beide Bootpfade erzeugen denselben TLV-basierten Boot Information Block gemäß Nova Boot Handoff Protocol (NBHP/BIB).

```text
NBHP/BIB

├── versionierter Header
│   ├── Magic: "NBHPBIB\0"
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

Der Kernel erhält genau einen Zeiger auf den zusammenhängenden BIB. Firmwareabhängige Quelldaten werden durch den jeweiligen Loader normalisiert und ausschließlich als definierte TLV-Nutzdaten übergeben.

Dadurch benötigt der Kernel keinerlei BIOS- oder UEFI-spezifischen Code. Unbekannte optionale TLV-Einträge können übersprungen werden; unbekannte erforderliche Einträge führen zu einem kontrollierten Bootabbruch.

---

# 9. Firmware-Typen

Nova definiert folgende Firmwaretypen:

```text
NOVA_FW_UNKNOWN

NOVA_FW_BIOS

NOVA_FW_UEFI
```

Spätere Erweiterungen:

```text
NOVA_FW_COREBOOT

NOVA_FW_OPENBIOS

NOVA_FW_U_BOOT
```

---

# 10. BIOS-spezifische Aufgaben

Der BIOS-Bootloader übernimmt:

- MBR laden
- Stage 2 laden
- Speicher erkennen (E820)
- Protected Mode aktivieren
- GDT laden
- Kernel laden
- Bootinformationen erzeugen

---

# 11. UEFI-spezifische Aufgaben

Der UEFI-Bootloader übernimmt:

- EFI-Anwendung starten
- GOP (Graphics Output Protocol)
- ACPI finden
- Memory Map erzeugen
- Runtime Services erfassen
- ExitBootServices()
- Kernel laden
- Bootinformationen erzeugen

---

# 12. Gemeinsame Komponenten

Gemeinsam genutzt werden:

- ELF Loader
- Kernel Loader
- Dateisystemzugriff
- Bootlogo
- Konfiguration
- Kompression
- Integritätsprüfung
- Kryptographie
- Bootparameter

---

# 13. Verzeichnisstruktur

```text
boot/

├── bios/
│   ├── stage1.asm
│   ├── stage2.asm
│   └── bios_memory.asm
│
├── uefi/
│   ├── main.c
│   ├── graphics.c
│   ├── memory.c
│   └── filesystem.c
│
├── common/
│   ├── elf.c
│   ├── config.c
│   ├── loader.c
│   ├── compression.c
│   ├── crypto.c
│   └── bootinfo.c
│
└── include/
```

---

# 14. Vorteile

- maximale Hardwareunterstützung
- nur ein Kernel
- saubere Architektur
- einfache Wartbarkeit
- gute Testbarkeit
- geringer Code-Duplikatanteil
- einfache Portierung

---

# 15. Nachteile

- zwei Bootloader müssen gepflegt werden
- höherer Testaufwand
- komplexeres Build-System
- mehr Dokumentation notwendig

---

# 16. Verworfene Alternativen

## Alternative A – Nur BIOS

**Vorteile**

- einfach

**Nachteile**

- moderne Hardware wird ausgeschlossen
- keine Secure-Boot-Unterstützung
- keine UEFI-Dienste
- langfristig nicht zukunftsfähig

---

## Alternative B – Nur UEFI

**Vorteile**

- moderner Standard
- weniger Bootcode

**Nachteile**

- ältere Rechner werden ausgeschlossen
- viele Industrie-PCs können NovaOS nicht starten
- erschwerte Nutzung in Legacy-Umgebungen

---

## Alternative C – Zwei getrennte Kernel

**Vorteile**

- firmwareoptimierter Code

**Nachteile**

- doppelte Wartung
- doppelte Tests
- höheres Fehlerrisiko

Diese Alternative wird verworfen.

---

# 17. Konsequenzen

Durch diese Entscheidung muss:

- eine Boot-Abstraktionsschicht implementiert werden,
- die Kernel-Einstiegsschnittstelle stabil bleiben,
- sämtlicher Firmware-Code auf den Bootloader beschränkt werden.

Der Kernel darf keine direkten BIOS- oder UEFI-Abhängigkeiten besitzen.

---

# 18. Auswirkungen

Diese ADR beeinflusst unmittelbar:

- RFC-BOOT-0001
- RFC-KERNEL-0001
- NPSPEC-BOOT-0001
- NPSPEC-KERNEL-0001
- HAL
- Bootloader
- Installer
- Recovery-System

---

# 19. Umsetzungsregeln

- BIOS und UEFI MÜSSEN denselben Kernel laden.
- Firmware-spezifischer Code DARF ausschließlich im Bootloader liegen.
- BIOS und UEFI MÜSSEN denselben NBHP/BIB-Header und dieselben TLV-Definitionen verwenden.
- Der Kernel MUSS genau einen Zeiger auf den Boot Information Block erhalten.
- Firmwareinformationen MÜSSEN vor der Übergabe in firmwareunabhängige TLV-Nutzdaten normalisiert werden.
- Unbekannte optionale TLV-Einträge MÜSSEN anhand ihrer Länge übersprungen werden können.
- Neue Firmwaretypen MÜSSEN über die Boot-Abstraktionsschicht integriert werden.
- Kernelcode DARF keine direkten BIOS-Interrupts oder UEFI-Boot-Services verwenden.

---

# 20. Langfristige Perspektive

Die Architektur ermöglicht zukünftig die Unterstützung weiterer Firmware-Implementierungen, ohne den Kernel selbst anzupassen, beispielsweise:

- Coreboot
- Libreboot
- U-Boot
- OpenBIOS
- virtuelle Firmware für Emulatoren

Damit bleibt NovaOS langfristig portierbar und unabhängig von einem einzelnen Firmware-Standard.

---

# 21. Referenzen

- ADR-Prinzipien und Best Practices
- Microsoft UEFI/Windows Boot Architecture
- UEFI Specification 2.x
- Intel BIOS Boot Specification
- NPSPEC-BOOT-0001 *(geplant)*
- NPSPEC-KERNEL-0001 *(geplant)*

---

# 22. Änderungsverlauf

| Version | Datum | Änderung |
|----------|------------|--------------------------------|
| 1.0 | 2026-07-24 | Erstfassung |
| 1.1 | 2026-07-29 | Gemeinsame Kernel-API auf das TLV-basierte NBHP/BIB-Handoff vereinheitlicht |
