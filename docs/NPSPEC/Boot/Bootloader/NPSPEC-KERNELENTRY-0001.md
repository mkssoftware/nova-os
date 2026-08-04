# NPSPEC-KERNELENTRY-0001: Nova Kernel Entry

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Kernel Architecture
- **Erstellt:** 2026-07-23
- **Gültig für:** Nova Bootloader, Nova Kernel Loader und Nova Kernel
- **Ersetzt:** Keine

---

# 1. Einleitung

Diese Spezifikation definiert den **Nova Kernel Entry (NKE)**.

Der Kernel Entry bildet den ersten ausführbaren Code des Nova Kernels nach der Übergabe durch den Nova Kernel Loader.

Er ist für die Übernahme der vollständigen Kontrolle über das System verantwortlich und stellt den Übergang von der Bootumgebung zur eigentlichen Kernelinitialisierung dar.

Der Kernel Entry besitzt ausschließlich Aufgaben der frühen Initialisierung.

---

# 2. Zielsetzung

Der Nova Kernel Entry soll:

- möglichst klein sein
- deterministisch arbeiten
- architekturunabhängig aufgebaut sein
- reproduzierbare Kernelstarts ermöglichen
- keine Hardware erneut initialisieren
- den Boot Information Block übernehmen
- die Kernelinitialisierung vorbereiten
- alle Fehler früh erkennen

---

# 3. Architektur

```text
Firmware

↓

Nova Bootloader

↓

Nova Kernel Loader

↓

Boot Handoff Protocol

↓

Nova Kernel Entry

↓

Kernel Initialisierung

↓

Kernel Main

↓

Scheduler
```

Der Kernel Entry stellt die erste Kernelkomponente dar.

---

# 4. Verantwortlichkeiten

Der Kernel Entry übernimmt:

- Boot Information Block übernehmen
- CPU-Zustand prüfen
- Kernelumgebung initialisieren
- Stack übernehmen
- frühe Speicherinitialisierung
- frühe CPU-Initialisierung
- Kernel Main aufrufen

Nach Abschluss dieser Aufgaben endet der Kernel Entry.

---

# 5. Übergabebedingungen

Vor dem Aufruf des Kernel Entry gelten folgende Voraussetzungen:

- Kernel vollständig geladen
- Boot Information Block gültig
- Stack initialisiert
- CPU im vorgesehenen Betriebsmodus
- Interrupts deaktiviert
- Kernel-Speicher reserviert
- Bootloader beendet

Der Bootloader übergibt den Kernel in einem definierten CPU-Zustand und mit einem Zeiger auf die Bootparameter. Dieses Prinzip entspricht etablierten Boot-Protokollen. :contentReference[oaicite:0]{index=0}

---

# 6. Kernel Entry Point

Der Einstiegspunkt besitzt genau eine Aufgabe:

```text
Kernel Entry

↓

Boot Information Block übernehmen

↓

Kernel Context erzeugen

↓

Kernel Main starten
```

Es findet keinerlei komplexe Initialisierung statt.

---

# 7. Registerzustand

Beim Eintritt gelten folgende Regeln:

| Register | Bedeutung |
|----------|-----------|
| Instruction Pointer | Kernel Entry |
| Stack Pointer | Kernel Stack |
| Boot Parameter Register | Zeiger auf Boot Information Block |
| Interrupt Flag | deaktiviert |

Die konkrete Registerbelegung ist architekturabhängig.

---

# 8. Boot Information Block

Der Kernel Entry übernimmt ausschließlich einen Zeiger auf den Boot Information Block.

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
└── Boot Options
```

Der Kernel Entry verändert diese Struktur nicht.

---

# 9. Kernel Context

Aus dem Boot Information Block wird ein interner Kernel Context erzeugt.

```text
Kernel Context

├── CPU
├── Memory
├── Graphics
├── Storage
├── Security
├── Timing
├── Features
└── Configuration
```

Alle weiteren Kernelkomponenten greifen ausschließlich auf den Kernel Context zu.

---

# 10. Frühe Initialisierung

Der Kernel Entry initialisiert ausschließlich:

- internen Stack
- Kernel Context
- frühes Logging
- frühe Fehlerbehandlung
- Architekturabstraktion

Treiber werden nicht geladen.

---

# 11. Speicherinitialisierung

Der Kernel Entry übernimmt:

- Memory Map
- reservierte Bereiche
- Kernelbereich
- Framebuffer
- ACPI
- MMIO

Die eigentliche Speicherverwaltung wird später durch den Memory Manager aufgebaut.

---

# 12. CPU-Initialisierung

Der Kernel Entry übernimmt:

- CPU-Features
- Architekturinformationen
- APIC-Informationen
- TSC
- unterstützte Erweiterungen

Weitere Prozessoren bleiben deaktiviert.

---

# 13. Interrupts

Während des gesamten Kernel Entry gelten:

```text
Interrupts

↓

Deaktiviert

↓

Kernel Main

↓

Interrupt Manager

↓

Aktivierung
```

Interrupts werden erst nach erfolgreicher Initialisierung des Interrupt Managers freigegeben. Dieses Vorgehen entspricht den Anforderungen moderner Kernel-Bootprotokolle. :contentReference[oaicite:1]{index=1}

---

# 14. Grafik

Ist ein Framebuffer vorhanden, übernimmt der Kernel Entry:

- Framebuffer-Adresse
- Auflösung
- Pixelformat
- Farbtiefe

Dadurch kann bereits sehr früh eine grafische Fehlerausgabe erfolgen.

---

# 15. Fehlerbehandlung

Tritt ein Fehler auf:

```text
Kernel Entry

↓

Fehler

↓

Panic Handler

↓

Recovery

↓

Bootmenü
```

Der Kernel Entry versucht niemals, einen inkonsistenten Zustand fortzusetzen.

---

# 16. Architekturunabhängigkeit

Der Kernel Entry besitzt eine gemeinsame Schnittstelle.

Architekturspezifische Implementierungen existieren für:

- x86
- x86-64
- ARM64
- RISC-V

Alle verwenden dieselbe logische Initialisierungsreihenfolge.

---

# 17. Kernel Main

Nach erfolgreicher Initialisierung erfolgt:

```text
Kernel Entry

↓

Kernel Main

↓

Subsysteme

↓

Memory Manager

↓

Scheduler

↓

Treiber

↓

Userspace
```

Kernel Main übernimmt anschließend die vollständige Kontrolle.

---

# 18. Sicherheitsmodell

Der Kernel Entry überprüft mindestens:

- Version des Boot Information Block
- Magic Number
- Prüfsumme
- Architektur
- Kernel-Konfiguration
- Sicherheitsstatus

Ungültige Bootinformationen führen unmittelbar zu einem Kernel Panic.

---

# 19. Positive Konsequenzen

Die Architektur bietet zahlreiche Vorteile.

## Klare Verantwortlichkeiten

Der Kernel Entry übernimmt ausschließlich frühe Initialisierung.

---

## Deterministischer Start

Alle Kernelstarts folgen derselben Reihenfolge.

---

## Gute Wartbarkeit

Früher Kernelcode bleibt klein und übersichtlich.

---

## Portabilität

Neue Architekturen können dieselbe Struktur verwenden.

---

## Stabilität

Bootloader und Kernel bleiben sauber getrennt.

---

# 20. Negative Konsequenzen und Risiken

## Strikte Reihenfolge

Die Initialisierung muss exakt eingehalten werden.

---

## Hohe Anforderungen

Fehler im Kernel Entry verhindern grundsätzlich den Systemstart.

---

## Architekturpflege

Jede unterstützte CPU-Architektur benötigt eine eigene Entry-Implementierung.

---

# 21. Auswirkungen auf andere Module

Diese Spezifikation betrifft insbesondere:

- Nova Kernel Loader
- Boot Handoff Protocol
- Kernel Main
- Memory Manager
- CPU Manager
- Interrupt Manager
- Graphics Manager
- Security Manager
- Scheduler
- Recovery Manager

Alle Kernelinitialisierungen MÜSSEN über den Nova Kernel Entry erfolgen.

---

# 22. Umsetzungsregeln

Für den Nova Kernel Entry gelten folgende verbindliche Regeln:

- Der Kernel Entry MUSS der erste ausgeführte Kernelcode sein.
- Der Kernel Entry DARF keine Treiber initialisieren.
- Der Kernel Entry MUSS den Boot Information Block validieren.
- Der Kernel Entry MUSS einen Kernel Context erzeugen.
- Interrupts MÜSSEN deaktiviert bleiben.
- Kernel Main DARF erst nach erfolgreicher Initialisierung gestartet werden.
- Alle Fehler MÜSSEN deterministisch behandelt werden.
- Der Kernel Entry MUSS architekturunabhängig spezifiziert werden.
- Der Kernel Entry MUSS vollständig reproduzierbar arbeiten.

---

# 23. Referenzen

- NPSPEC-BOOT-0001
- NPSPEC-BOOTFS-0001
- NPSPEC-KERNELLOADER-0001
- NPSPEC-BOOTHANDOFF-0001
- ADR-6038 – API-Design von NovaFS
- Linux x86 Boot Protocol – Kernel Entry Requirements :contentReference[oaicite:2]{index=2}
- Linux Kernel Documentation – x86 Boot Protocol :contentReference[oaicite:3]{index=3}

---

# 24. Statushistorie

- **2026-07-23:** Version 1.0 erstellt.
- **2026-07-23:** Nova Kernel Entry als verbindlicher Einstiegspunkt des Nova Kernels definiert.
- **Zukünftige Änderungen:** Nur durch eine neue Version dieser Spezifikation oder eine ersetzende NPSPEC zulässig.