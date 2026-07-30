# ADR-BOOT-1012: Eigenes Boot-Handoff-Protokoll

- **ADR-ID:** ADR-BOOT-1012
- **Status:** Accepted
- **Version:** 1.1
- **Datum:** 2026-07-24
- **Autor:** NovaOS Architektur
- **Kategorie:** Bootloader / Kernel
- **Betroffene Komponenten:**
  - Bootloader
  - BIOS Loader
  - UEFI Loader
  - Kernel Loader
  - Kernel Entry
  - HAL
- **Ersetzt:** Keine

---

# 1. Kontext

Der Übergang zwischen Bootloader und Kernel gehört zu den kritischsten Bereichen eines Betriebssystems.

Viele bestehende Betriebssysteme verwenden externe Standards wie:

- Multiboot 1
- Multiboot 2
- Stivale
- Limine Boot Protocol

Diese Standards bieten zwar eine hohe Kompatibilität mit vorhandenen Bootloadern, definieren jedoch auch die interne Architektur des Betriebssystems mit.

Da NovaOS vollständig unabhängig entwickelt werden soll, würde die dauerhafte Abhängigkeit von einem externen Bootprotokoll die langfristige Weiterentwicklung einschränken.

---

# 2. Problem

Es muss entschieden werden, ob NovaOS:

- ein bestehendes Boot-Handoff-Protokoll übernimmt,
- mehrere externe Protokolle dauerhaft unterstützt,
- oder ein vollständig eigenes Übergabeprotokoll zwischen Bootloader und Kernel definiert.

---

# 3. Entscheidung

**NovaOS definiert mit dem Nova Boot Handoff Protocol (NBHP) ein eigenes Boot-Handoff-Protokoll.**

Der Nova-Bootloader erzeugt einen standardisierten, TLV-basierten Boot Information Block (BIB) und übergibt dem Kernel genau einen Zeiger auf diesen Block.

BIOS- und UEFI-Bootloader erzeugen dieselbe Datenstruktur.

Der Kernel arbeitet ausschließlich mit NBHP/BIB und darf nicht auf interne Datenstrukturen des Bootloaders zugreifen.

---

# 4. Begründung

Diese Entscheidung bietet:

- vollständige Kontrolle über den Bootprozess
- keine externen Abhängigkeiten
- einfache Erweiterbarkeit
- stabile Kernel-API
- identisches Verhalten unter BIOS und UEFI
- langfristige Wartbarkeit

Neue Informationen können jederzeit ergänzt werden, ohne auf Spezifikationen Dritter warten zu müssen.

---

# 5. Zielarchitektur

```text
Firmware

↓

Nova Bootloader

↓

Nova Boot Handoff

↓

Kernel Entry

↓

HAL

↓

Nova Kernel
```

---

# 6. Architektur des Boot-Handoffs

```text
Bootloader

↓

Hardware erkennen

↓

Speicher erkennen

↓

Grafik initialisieren

↓

ACPI finden

↓

Dateisystem vorbereiten

↓

Kernel laden

↓

TLV-basierten BIB erzeugen

↓

Kernel Entry
```

---

# 7. Übergabestruktur

Der Kernel erhält genau einen Zeiger auf einen zusammenhängenden Boot Information Block.

Der BIB besteht aus einem festen, versionierten Header und einer Folge von TLV-Einträgen:

```text
Boot Information Block (BIB)

├── Header
│   ├── Magic: "NBHPBIB\0"
│   ├── Version
│   ├── Header Size
│   ├── Total Size
│   ├── Flags
│   └── Checksum
│
└── TLV Entries
    ├── Type
    ├── Flags
    ├── Length
    └── Value
```

Für NBHP/BIB Version 1 sind mindestens folgende Eintragstypen definiert:

```text
Firmware
Memory
Graphics
Kernel
Security
Boot Options
```

Weitere Eintragstypen wie CPU, Storage, ACPI, InitRD, TPM oder Hypervisor können rückwärtskompatibel ergänzt werden. Unbekannte optionale TLV-Einträge werden anhand ihrer Länge übersprungen.

---

# 8. Versionsverwaltung

Der BIB-Header besitzt eine Protokollversion. Jeder TLV-Eintrag besitzt einen expliziten Typ, definierte Flags und eine definierte Länge.

```text
Version 1

↓

Version 2

↓

Version 3
```

Der Kernel kann ältere kompatible Versionen weiterhin interpretieren.

Neue Informationen werden vorzugsweise als neue TLV-Typen ergänzt. Bestehende TLV-Typen dürfen nur kompatibel erweitert werden; ihre bestehende Bedeutung darf nicht verändert werden.

---

# 9. Firmware-Unabhängigkeit

Nach der Übergabe kennt der Kernel nur noch:

```text
NBHP/BIB
```

Nicht mehr:

- BIOS
- UEFI
- Limine
- Multiboot

Dadurch bleibt der Kernel vollständig firmwareneutral.

---

# 10. Enthaltene Informationen

Der Boot Information Block kann unter anderem folgende Informationen enthalten:

- Firmwaretyp
- Speicherkarte
- Framebuffer
- Kerneladresse
- Kernelgröße
- ACPI
- RSDP
- Initial RAM Disk
- Bootzeit
- CPU-Informationen
- verfügbare Features

---

# 11. Erweiterbarkeit

Neue Informationen werden durch neue TLV-Eintragstypen oder durch kompatible, längenbasierte Erweiterungen bestehender TLV-Werte ergänzt.

Beispiele:

```text
NUMA

GPU

TPM

Hypervisor

Secure Boot

Virtualisierung

Boot Device

Disk UUID
```

Bestehende Typnummern, Feldbedeutungen und Kodierungen dürfen nicht verändert werden. Unbekannte optionale Einträge müssen übersprungen werden können. Unbekannte als erforderlich markierte Einträge führen zu einem kontrollierten Bootabbruch.

---

# 12. Speicherlayout

```text
+--------------------------+
| Bootloader               |
+--------------------------+
| BIB Header               |
| TLV Header + Firmware    |
| TLV Header + Memory      |
| TLV Header + Graphics    |
| TLV Header + Kernel      |
| TLV Header + Security    |
| TLV Header + Optionen    |
+--------------------------+
| Kernel                   |
+--------------------------+
```

Der BIB ist zusammenhängend, Little-Endian kodiert und gemäß NBHP ausgerichtet. Speicherbereiche, auf die TLV-Werte verweisen, bleiben bis zur vollständigen Kernelinitialisierung gültig.

---

# 13. Speicherverwaltung

Der Boot Information Block und alle von ihm referenzierten Übergabedaten liegen in reserviertem Speicher.

Der Kernel übernimmt diesen Speicher erst nach Abschluss der Initialisierung.

Danach kann:

- kopiert
- freigegeben
- archiviert

werden.

---

# 14. Integritätsprüfung

Vor der Übergabe prüft der Bootloader mindestens:

- Magic Number
- Protokollversion
- Headergröße
- Gesamtgröße
- BIB-Prüfsumme
- TLV-Header
- TLV-Längen und Alignment
- unbekannte erforderliche TLV-Typen
- Strukturgröße
- Zeigerbereiche
- Speichergrenzen
- Kernelgröße

Ungültige Bootinformationen führen zu einem kontrollierten Bootabbruch.

---

# 15. Prüfsummen

Der BIB-Header enthält eine Prüfsumme über den vollständigen Boot Information Block. TLV-Nutzdaten mit eigener Integritätsanforderung können zusätzlich typabhängige Prüfsummen oder Hashwerte enthalten.

Sie schützt vor:

- Speicherfehlern
- beschädigten Bootloadern
- Übertragungsfehlern
- fehlerhaften Erweiterungen

---

# 16. Vorteile

- vollständige Kontrolle
- keine externen Spezifikationen
- einfache Erweiterbarkeit
- stabile, erweiterbare TLV-ABI
- identisches Verhalten auf BIOS und UEFI
- geringere Komplexität
- klar definierte Kernel-Schnittstelle

---

# 17. Nachteile

- eigener Wartungsaufwand
- Dokumentation muss vollständig selbst gepflegt werden
- externe Bootloader können das Protokoll nicht ohne Erweiterung verwenden

---

# 18. Verworfene Alternativen

## Alternative A – Multiboot 2

**Vorteile**

- weit verbreitet
- gute Dokumentation

**Nachteile**

- externe Spezifikation
- eingeschränkte Erweiterbarkeit
- Abhängigkeit vom Standard

---

## Alternative B – Limine Protocol

**Vorteile**

- modern
- flexibel

**Nachteile**

- externe Abhängigkeit
- Änderungen außerhalb des Nova-Projekts

---

## Alternative C – Stivale

**Vorteile**

- einfach

**Nachteile**

- geringe Verbreitung
- nicht vollständig an NovaOS angepasst

---

Diese Alternativen werden verworfen.

---

# 19. Konsequenzen

Durch diese Entscheidung:

- besitzt NovaOS eine dauerhaft stabile interne Boot-API,
- kann der Kernel vollständig unabhängig entwickelt werden,
- bleiben Firmware und Kernel klar voneinander getrennt.

---

# 20. Auswirkungen

Diese ADR beeinflusst unmittelbar:

- ADR-BOOT-0001 – BIOS und UEFI parallel unterstützen
- NPSPEC-BOOTHANDOFF-0001
- NPSPEC-KERNELENTRY-0001
- NPSPEC-KERNELLOADER-0001
- HAL
- Bootloader
- Kernel Entry

---

# 21. Umsetzungsregeln

- Der Kernel DARF ausschließlich NBHP/BIB als Boot-Handoff-Schnittstelle verwenden.
- Der Kernel MUSS genau einen Zeiger auf den Boot Information Block erhalten.
- BIOS- und UEFI-Bootloader MÜSSEN denselben BIB-Header und dieselben TLV-Definitionen verwenden.
- Der Boot Information Block MUSS versioniert sein.
- Jeder TLV-Eintrag MUSS Typ, Flags, Länge und Nutzdaten besitzen.
- Neue Informationen MÜSSEN als rückwärtskompatible TLV-Erweiterungen ergänzt werden.
- Bestehende TLV-Typen und Feldbedeutungen DÜRFEN nicht inkompatibel verändert werden.
- Unbekannte optionale TLV-Einträge MÜSSEN übersprungen werden können.
- Alle Größen, Offsets, Speicheradressen und Ausrichtungen MÜSSEN validiert werden.
- Der vollständige BIB MUSS eine Integritätsprüfung unterstützen.
- Alle NBHP/BIB-Strukturen MÜSSEN Little-Endian kodiert sein.
- Firmware-spezifische Informationen DÜRFEN nicht direkt im Kernel verarbeitet werden.

---

# 22. Langfristige Perspektive

Das Nova Boot-Handoff-Protokoll bildet die langfristig stabile Schnittstelle zwischen Firmware und Kernel.

Es ermöglicht zukünftig:

- zusätzliche Firmwaretypen
- Secure Boot
- TPM
- Remote Boot
- Netzwerkboot
- Hypervisor-Unterstützung
- Cloud-Instanzen
- Embedded-Systeme

ohne Änderungen an der eigentlichen Kernelarchitektur.

---

# 23. Referenzen

- ADR-BOOT-0001 – BIOS und UEFI parallel unterstützen
- NPSPEC-BOOTHANDOFF-0001
- NPSPEC-KERNELENTRY-0001
- NPSPEC-KERNELLOADER-0001
- UEFI Specification 2.x
- Multiboot 2 Specification
- Limine Boot Protocol
- Stivale Boot Protocol

---

# 24. Änderungsverlauf

| Version | Datum | Änderung |
|----------|------------|--------------------------------|
| 1.0 | 2026-07-24 | Erstfassung |
| 1.1 | 2026-07-27 | Übergabemodell auf das TLV-basierte NBHP/BIB-Protokoll vereinheitlicht |
