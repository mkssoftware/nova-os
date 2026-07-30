# ADR-KERNEL-0002: Hardware Abstraction Layer (HAL)

* **ADR-ID:** ADR-KERNEL-0002
* **Titel:** Hardware Abstraction Layer (HAL)
* **Status:** Akzeptiert
* **Version:** 1.0.0
* **Datum:** 2026-07-29
* **Autor:** NovaOS Architekturgruppe
* **Kategorie:** Kernel-Architektur
* **Gültigkeitsbereich:** Gesamter NovaOS-Kernel
* **Ersetzt:** –
* **Ersetzt durch:** –
* **Abhängigkeiten:** ADR-KERNEL-0001, ADR-BOOT-1020, RFC-BOOT-0002, RFC-BOOT-0005, NPSPEC-KERNEL-0001

---

# 1. Zusammenfassung

NovaOS verwendet eine **Hardware Abstraction Layer (HAL)** als eigenständiges Kernelsubsystem.

Die HAL kapselt sämtliche architektur- und plattformspezifischen Funktionen und stellt dem übrigen Kernel eine einheitliche, stabile Programmierschnittstelle bereit. Dadurch bleibt der Großteil des Kernels unabhängig von CPU, Mainboard und Firmware. Dieses Schichtenmodell verbessert die Portierbarkeit und Wartbarkeit des Betriebssystems.

---

# 2. Kontext

Moderne Betriebssysteme unterstützen unterschiedliche:

* Prozessorarchitekturen
* Interrupt-Controller
* Timer
* Firmware
* Speichercontroller
* Plattformen

Direkte Hardwarezugriffe in sämtlichen Kernelkomponenten würden die Wartbarkeit erheblich verschlechtern.

---

# 3. Problemstellung

Ohne Hardwareabstraktion müsste nahezu jedes Kernelsubsystem:

* Register kennen
* CPU-spezifischen Code enthalten
* unterschiedliche Firmware berücksichtigen
* verschiedene Interruptcontroller unterstützen

Dadurch würden Portierungen erheblich erschwert. Das Ziel einer HAL besteht darin, hardwarespezifische Details vom übrigen Kernel zu entkoppeln.

---

# 4. Entscheidung

NovaOS implementiert eine zentrale Hardware Abstraction Layer.

Die HAL bildet die einzige Schicht des Kernels, die unmittelbar mit der Hardware kommuniziert.

Alle übrigen Kernelkomponenten verwenden ausschließlich die öffentlichen HAL-Schnittstellen.

---

# 5. Motivation

Diese Architektur bietet:

* bessere Portierbarkeit
* klar getrennte Verantwortlichkeiten
* weniger Architekturcode
* einfachere Wartung
* bessere Testbarkeit
* geringere Fehleranfälligkeit

---

# 6. Designprinzipien

Die HAL basiert auf folgenden Prinzipien:

* strikte Kapselung
* minimale Schnittstellen
* keine Geschäftslogik
* deterministisches Verhalten
* Architekturunabhängigkeit
* Erweiterbarkeit

---

# 7. Architekturübersicht

```text
                Kernel

+------------------------------------------------------+
| Scheduler | VFS | IPC | Netzwerk | Speicher | usw.   |
+---------------------------+--------------------------+
                            |
                            v
+------------------------------------------------------+
|             Hardware Abstraction Layer               |
+------------------------------------------------------+
| CPU | Timer | Interrupt | MMU | PCI | ACPI | UEFI    |
+------------------------------------------------------+
                            |
                            v
                      Physische Hardware
```

---

# 8. Verantwortungsbereich

Die HAL übernimmt ausschließlich hardwarenahe Aufgaben.

Dazu gehören:

* CPU
* Interrupts
* MMU
* Timer
* Firmware
* Plattforminitialisierung
* I/O-Grundfunktionen

---

# 9. Nicht Bestandteil der HAL

Nicht Bestandteil der HAL sind:

* Scheduler
* Prozesse
* Threads
* Dateisysteme
* Netzwerkstack
* Speicherverwaltung auf hoher Ebene
* Sicherheitsrichtlinien

---

# 10. Kommunikationsmodell

Kernelkomponenten greifen niemals direkt auf Hardware zu.

Jeder Zugriff erfolgt ausschließlich über die HAL.

---

# 11. CPU-Abstraktion

Die CPU-Abstraktion kapselt:

* Registerzugriffe
* Kontrollregister
* Cache-Steuerung
* CPU-Features
* Kontextwechsel
* Architekturinformationen

---

# 12. Interrupt-Abstraktion

Die HAL stellt ein einheitliches Interruptmodell bereit.

Unabhängig davon, ob die Plattform beispielsweise APIC, GIC oder einen zukünftigen Interruptcontroller verwendet.

---

# 13. Timer-Abstraktion

Zeitquellen werden vollständig abstrahiert.

Unterstützte Timer können beispielsweise sein:

* PIT
* HPET
* APIC Timer
* ARM Generic Timer
* zukünftige Hardwaretimer

---

# 14. Speicherverwaltung

Die HAL stellt ausschließlich primitive Speicheroperationen bereit.

Beispiele:

* Seitentabellen initialisieren
* MMU aktivieren
* TLB invalidieren
* Cache synchronisieren

Komplexe Speicherverwaltung erfolgt außerhalb der HAL.

---

# 15. Firmware-Abstraktion

Die HAL kapselt unterschiedliche Firmwaremodelle.

Beispiele:

* BIOS
* UEFI
* zukünftige Firmware

Dadurch bleiben Kernelkomponenten firmwareunabhängig.

---

# 16. Plattforminitialisierung

Die frühe Plattforminitialisierung erfolgt vollständig innerhalb der HAL.

Hierzu gehören:

* CPU-Erkennung
* Interruptcontroller
* Timer
* MMU
* Firmwareinformationen

---

# 17. Plattforminformationen

Die HAL stellt standardisierte Informationen bereit.

Beispiele:

* CPU-Anzahl
* Cachegrößen
* Seitengröße
* Taktquellen
* verfügbare Speicherbereiche

---

# 18. Gerätezugriff

Treiber greifen nicht direkt auf CPU-spezifische Mechanismen zu.

Die HAL stellt standardisierte Funktionen bereit.

---

# 19. Mehrarchitektur-Unterstützung

Die HAL unterstützt unterschiedliche Architekturen.

Beispiele:

* x86
* x86_64
* ARM64
* RISC-V

Neue Architekturen werden ausschließlich durch Erweiterung der HAL integriert.

---

# 20. Verzeichnisstruktur

Empfohlene Projektstruktur:

```text
kernel/

    hal/

        common/

        x86/

        x86_64/

        arm64/

        riscv/
```

---

# 21. Initialisierung

Die HAL wird unmittelbar nach dem Kernelstart initialisiert.

Danach stehen sämtliche Hardwaredienste dem Kernel zur Verfügung.

---

# 22. Schnittstellen

Alle öffentlichen HAL-Funktionen besitzen:

* dokumentierte Parameter
* definierte Rückgabewerte
* stabile ABI innerhalb einer Kernelversion

---

# 23. Fehlerbehandlung

Hardwarefehler werden standardisiert gemeldet.

Mögliche Rückgaben:

* SUCCESS
* NOT_SUPPORTED
* INVALID_PARAMETER
* DEVICE_ERROR
* INITIALIZATION_FAILED

---

# 24. Synchronisation

Die HAL selbst besitzt keine Schedulerlogik.

Synchronisationsmechanismen werden nur dort eingesetzt, wo paralleler Hardwarezugriff erforderlich ist.

---

# 25. Performance

Die HAL muss möglichst wenig Overhead verursachen.

Abstraktion darf nicht zu unnötigen Funktionsketten oder Speicherkopien führen.

---

# 26. Sicherheit

Direkte Hardwarezugriffe außerhalb der HAL sind unzulässig.

Dies vereinfacht:

* Codeaudits
* Sicherheitsprüfungen
* Architekturwechsel

---

# 27. Erweiterbarkeit

Neue Hardware wird ausschließlich durch Erweiterung der HAL integriert.

Bestehende Kernelkomponenten bleiben unverändert.

---

# 28. Auswirkungen

Positive Auswirkungen:

* hohe Portierbarkeit
* geringere Komplexität
* zentrale Hardwareverwaltung
* klar definierte Verantwortlichkeiten
* einfachere Tests

Negative Auswirkungen:

* zusätzlicher Implementierungsaufwand
* stabile HAL-API erforderlich
* geringe zusätzliche Abstraktionskosten

---

# 29. Verworfene Alternativen

## Direkter Hardwarezugriff

Vorteile:

* minimaler Overhead

Nachteile:

* schlechte Wartbarkeit
* schwierige Portierung
* hoher Architekturanteil

---

## Verteilte Hardwareabstraktion

Jedes Subsystem implementiert eigene Hardwarezugriffe.

Diese Lösung wurde verworfen, da sie zu Redundanz, inkonsistenten Schnittstellen und höherem Wartungsaufwand führt.

---

# 30. Implementierungsrichtlinien

Alle neuen Hardwarefunktionen müssen:

* innerhalb der HAL implementiert werden,
* dokumentierte APIs besitzen,
* architekturabhängige Implementierungen kapseln,
* keine Kernelgeschäftslogik enthalten,
* testbar sein.

---

# 31. Teststrategie

Die HAL wird geprüft durch:

* Unit-Tests
* Architekturtests
* Emulatoren
* Hardwaretests
* Integrationstests
* Regressionstests

---

# 32. Konsequenzen

Die Hardware Abstraction Layer bildet die verbindliche Schnittstelle zwischen Kernel und physischer Hardware.

Alle zukünftigen Kernelkomponenten müssen ausschließlich über die HAL auf hardwareabhängige Funktionen zugreifen. Neue Prozessorarchitekturen oder Plattformen werden durch Ergänzungen innerhalb der HAL unterstützt, ohne Änderungen an den darüberliegenden Kernelsubsystemen zu erzwingen.

---

# 33. Zusammenfassung

NovaOS verwendet eine **Hardware Abstraction Layer (HAL)** als zentrale Hardware-Schnittstelle des Kernels. Sie kapselt sämtliche plattform- und architekturabhängigen Funktionen und stellt dem restlichen Kernel eine einheitliche API bereit. Dieses Architekturprinzip verbessert Portierbarkeit, Wartbarkeit und Testbarkeit und ermöglicht die Unterstützung zukünftiger Hardwareplattformen, ohne die Kernarchitektur grundlegend ändern zu müssen.
