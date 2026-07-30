# ADR-BOOT-1015: 64-Bit als langfristige Zielarchitektur

* **Dokumenttyp:** Architecture Decision Record (ADR)
* **Dokument-ID:** ADR-BOOT-1015
* **Titel:** 64-Bit als langfristige Zielarchitektur
* **Version:** 1.0.0
* **Status:** Angenommen
* **Kategorie:** Boot Architecture / Kernel Architecture
* **Entscheidungsdatum:** 2026-07-29
* **Gültig für:** NovaOS Bootloader, Kernel, Build-System und Systemarchitektur
* **Verantwortlich:** Nova Platform Architecture
* **Ersetzt:** Keine
* **Ersetzt durch:** Keine

## Referenzen

* ADR-BOOT-0001 – BIOS und UEFI parallel unterstützen
* ADR-BOOT-0004 – ELF als Kernel-Format
* ADR-BOOT-1012 – Eigenes Boot-Handoff-Protokoll
* ADR-BOOT-1013 – Stage-1 bleibt minimal
* NPSPEC-KERNELENTRY-0001
* NPSPEC-KERNELLOADER-0001
* NPSPEC-BOOTFS-0001
* x86-64 Long Mode Architektur
* AMD64/x64 Architekturübersicht

---

# 1. Kontext

NovaOS wird zunächst mit einem 32-Bit-Kernel entwickelt.

Diese Entscheidung dient ausschließlich der Vereinfachung der frühen Entwicklungsphase.

Die langfristige Vision von NovaOS umfasst:

* moderne Desktop-Systeme
* leistungsfähige Workstations
* Server
* Robotik
* KI-Anwendungen
* Virtualisierung
* Hochleistungsrechner
* verteilte Systeme

Diese Einsatzgebiete profitieren erheblich von einer nativen 64-Bit-Architektur. Moderne x86-Prozessoren stellen hierfür den sogenannten **Long Mode** bereit, der 64-Bit-Register, zusätzliche Register sowie einen erweiterten Adressraum bietet.

---

# 2. Problemstellung

Es muss entschieden werden, welche Prozessorarchitektur NovaOS langfristig als primäre Zielplattform verwendet.

Zur Auswahl stehen unter anderem:

* dauerhaft 32 Bit
* 32- und 64-Bit parallel
* ausschließlich 64 Bit
* mehrere Architekturen mit gemeinsamer Kernelbasis

---

# 3. Entscheidung

Die langfristige Zielarchitektur von NovaOS ist:

```text
x86-64 (AMD64 / Intel 64)
```

Der 32-Bit-Kernel dient ausschließlich:

* der frühen Entwicklung,
* dem Bootstrapping,
* der Hardwarevalidierung,
* und dem schrittweisen Aufbau der Kernelarchitektur.

Nach Erreichen einer stabilen Kernelbasis wird die Hauptentwicklung auf einen nativen 64-Bit-Kernel verlagert.

---

# 4. Ziele

Die Entscheidung verfolgt folgende Ziele:

* langfristige Zukunftssicherheit
* Unterstützung moderner Hardware
* großer virtueller Adressraum
* effizientere Speicherverwaltung
* mehr CPU-Register
* bessere Compileroptimierungen
* Unterstützung großer Anwendungen
* Vorbereitung auf Virtualisierung
* Vorbereitung auf KI-Workloads
* langfristig stabile Kernel-ABI

---

# 5. Gründe für die Entscheidung

Die Wahl einer 64-Bit-Architektur basiert auf mehreren technischen Vorteilen.

Dazu gehören:

* größerer virtueller Adressraum
* zusätzliche Register
* moderne Aufrufkonventionen
* effizientere Codegenerierung
* bessere Unterstützung aktueller Compiler
* vereinfachte Speicherverwaltung
* Unterstützung großer RAM-Konfigurationen
* langfristige Hardwarekompatibilität

x86-64 erweitert die klassischen acht Register auf sechzehn allgemeine Register und führt den Long Mode als nativen Betriebsmodus ein.

---

# 6. Warum zunächst 32 Bit?

Die erste Kernelgeneration verwendet dennoch 32 Bit.

Gründe:

* geringere Komplexität
* einfacherer Protected Mode
* einfachere Seitentabellen
* leichteres Debugging
* kleinere Einstiegshürde
* einfachere Assemblerentwicklung

Der frühe Kernel dient als Architekturprototyp.

---

# 7. Langfristige Architektur

Der langfristige Bootpfad lautet:

```text
BIOS / UEFI

↓

Bootloader

↓

64-Bit Kernel Loader

↓

Long Mode

↓

Nova Kernel x64
```

Der Wechsel in den Long Mode erfolgt vor dem eigentlichen Kernelstart. Der Prozessor startet bei BIOS-Systemen zunächst im Real Mode; der Wechsel in den Long Mode erfolgt erst nach Vorbereitung von Paging, GDT und CPU-Zustand.

---

# 8. Übergangsstrategie

NovaOS verwendet folgende Entwicklungsphasen:

```text
Phase 1

32-Bit Kernel

↓

Phase 2

Hybridentwicklung

↓

Phase 3

64-Bit Kernel

↓

Phase 4

64-Bit Standard
```

---

# 9. Kernel-ABI

Bereits während der 32-Bit-Entwicklung soll darauf geachtet werden, dass:

* interne Schnittstellen
* Kernelobjekte
* IPC
* Dateisysteme
* Treiberarchitektur

möglichst unabhängig von der Wortbreite entworfen werden.

---

# 10. Zeigertypen

Neue Kernelkomponenten sollen grundsätzlich:

```c
uintptr_t

intptr_t

size_t

ptrdiff_t
```

anstelle fester Integergrößen verwenden.

Dadurch wird die spätere Migration erleichtert.

---

# 11. Datentypen

Folgende Typen dürfen nicht implizit von einer festen Architekturgröße abhängen:

* Handles
* Objekt-IDs
* Speicheradressen
* Dateioffsets
* Kernelreferenzen

---

# 12. Speicherverwaltung

Die Speicherverwaltung wird so entworfen, dass sie später:

* 4-Level-Paging
* 5-Level-Paging
* große Seitentabellen
* große physische Speicherbereiche

unterstützen kann.

---

# 13. Virtueller Adressraum

Der Kernel soll langfristig einen großen virtuellen Adressraum nutzen.

Beispielsweise:

```text
Kernel Space

User Space

Shared Memory

Memory Mapped Files

Device Mapping

Kernel Heap
```

Die genaue Aufteilung wird separat spezifiziert.

---

# 14. ELF-Unterstützung

Der langfristige Standard lautet:

```text
ELF64
```

Während der Übergangsphase bleiben sowohl ELF32 als auch ELF64 zulässig.

---

# 15. Long Mode

Der Kernel Loader aktiviert vor dem Kernelstart:

* PAE
* Paging
* Long Mode
* 64-Bit-GDT

anschließend erfolgt der Sprung in den 64-Bit-Kernel.

---

# 16. Kompatibilität

NovaOS soll langfristig dennoch folgende Szenarien unterstützen:

* 32-Bit Bootloader
* 64-Bit Kernel
* BIOS
* UEFI

Dadurch bleibt der Bootpfad flexibel.

---

# 17. Build-System

Das Build-System muss später erzeugen können:

```text
nova-kernel-x86.elf

nova-kernel-x86_64.elf
```

Beide Builds sollen aus derselben Codebasis entstehen.

---

# 18. Treiber

Treiber dürfen keine Annahmen über Pointergrößen treffen.

Architekturabhängiger Code ist zu kapseln.

---

# 19. Compiler

Der Kernel muss langfristig mit einer Cross-Toolchain erzeugt werden.

Beispiele:

```text
i686-elf

↓

x86_64-elf
```

---

# 20. Konsequenzen

## Positive Konsequenzen

* moderne Architektur
* hohe Zukunftssicherheit
* bessere Performance
* größere Speicherbereiche
* bessere Optimierungsmöglichkeiten
* Vorbereitung auf Server und KI

## Negative Konsequenzen

* Long-Mode-Einstieg komplexer
* aufwendigere Speicherverwaltung
* größere Seitentabellen
* aufwendigere frühe Bootphase

---

# 21. Alternativen

### Dauerhaft 32 Bit

**Abgelehnt**

Gründe:

* begrenzter Adressraum
* schlechtere Zukunftsfähigkeit
* Einschränkungen bei großen Anwendungen

### Sofort ausschließlich 64 Bit

**Abgelehnt**

Gründe:

* höhere Einstiegskomplexität
* erschwertes frühes Debugging

Die schrittweise Migration bietet das beste Verhältnis aus Entwicklungsaufwand und langfristigem Nutzen.

---

# 22. Implementierungsstrategie

### Phase 1

* BIOS
* Stage-1
* Stage-2
* 32-Bit Kernel

### Phase 2

* gemeinsamer Kernelcode
* Architekturabstraktion

### Phase 3

* Long Mode
* ELF64
* 64-Bit Kernel

### Phase 4

* vollständige x86-64-Unterstützung
* Optimierungen
* Kernelmodule
* Virtualisierung

---

# 23. Konformitätskriterien

Eine Implementierung erfüllt dieses ADR, wenn:

* 32 Bit ausschließlich als Übergangsarchitektur behandelt wird,
* neue Kernelkomponenten architekturunabhängig entworfen werden,
* Pointergrößen nicht fest angenommen werden,
* das Build-System einen späteren ELF64-Build unterstützt,
* der Long Mode Bestandteil der Bootarchitektur ist,
* und x86-64 als primäre Zielplattform definiert bleibt.

---

# 24. Entscheidungsergebnis

NovaOS verfolgt langfristig eine native 64-Bit-Architektur.

Die aktuelle 32-Bit-Implementierung dient ausschließlich als Entwicklungs- und Migrationsschritt.

Alle neuen Komponenten sollen bereits heute so entworfen werden, dass die spätere Umstellung auf einen nativen 64-Bit-Kernel mit minimalem Aufwand möglich ist.

---

# 25. Zusammenfassung

NovaOS entwickelt seinen Kernel zunächst bewusst im 32-Bit-Protected-Mode, um die frühe Entwicklung zu vereinfachen. Diese Entscheidung ist jedoch ausdrücklich temporär.

Die langfristige Zielarchitektur ist **x86-64 im Long Mode**. Dadurch erhält NovaOS Zugriff auf einen größeren Adressraum, zusätzliche Register und moderne Architekturmerkmale, die für zukünftige Desktop-, Server-, Robotik- und KI-Anwendungen erforderlich sind. Durch eine frühzeitige Architekturabstraktion und die Verwendung portabler Datentypen wird der Übergang auf einen nativen 64-Bit-Kernel vorbereitet, ohne die frühe Entwicklung unnötig zu verkomplizieren.
