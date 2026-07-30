# ADR-0001: C als primäre Systemsprache

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Foundation Architecture
- **Erstellt:** 2026-07-11
- **Gültig für:** Nova Platform
- **Ersetzt:** Keine

---

## 1. Kontext

Die Nova Platform benötigt eine Systemsprache für Bootloader-nahe Komponenten, Kernel, HAL, Treiber, Core Services und grundlegende Bibliotheken. Diese Bereiche müssen ohne Host-Runtime, Standardbibliothek oder Betriebssystemabhängigkeiten funktionieren.

## 2. Problemstellung

Die Sprache muss direkten Speicher- und Hardwarezugriff erlauben, binär kontrollierbar sein, auf x86 und später weiteren Architekturen funktionieren und mit Assembler sowie freestanding Toolchains harmonieren.

## 3. Betrachtete Alternativen

- C17 als primäre Systemsprache
- C++ ohne Runtime, Exceptions und RTTI
- Rust mit eigener Runtime- und Toolchain-Integration
- Assembler für große Teile des Systems

## 4. Entscheidung

Kernelnahe Komponenten werden primär in C17 implementiert. Architekturabhängige Start- und Low-Level-Routinen werden in NASM-Assembler geschrieben. Host-Werkzeuge dürfen Python, C++ oder Rust verwenden.

## 5. Begründung

- C bietet direkten und vorhersehbaren Zugriff auf Speicher, Register und Binärlayouts.
- Freestanding-Compiler und Cross-Toolchains sind breit verfügbar.
- Die Sprache lässt sich sauber mit Assembler und Linkerskripten kombinieren.
- C vermeidet eine verpflichtende Laufzeitumgebung.
- Der entstehende Code bleibt auch für Bootloader, Kernel und Firmware-nahe Module verwendbar.

## 6. Positive Konsequenzen

- Kleine, kontrollierbare Binärdateien
- Hohe Portierbarkeit zwischen Compilern und Architekturen
- Klare ABI-Grenzen
- Geringe Abhängigkeit von externen Runtimes

## 7. Negative Konsequenzen und Risiken

- Manuelle Speicherverwaltung
- Keine automatische Typsicherheit für Besitzverhältnisse
- Mehr Disziplin bei Fehlerbehandlung und Lebenszyklen erforderlich

## 8. Auswirkungen auf andere Module

Betrifft Bootloader, Kernel, HAL, Treiber, Foundation, Core Services und systemnahe Bibliotheken.

## 9. Umsetzungsregeln

- Die Entscheidung ist für neue Implementierungen verbindlich.
- Abweichungen benötigen ein neues ADR oder eine dokumentierte Ersetzung dieses ADRs.
- Öffentliche APIs und persistente Formate müssen die festgelegten ABI- und Versionsregeln beachten.
- Tests und Dokumentation sind zusammen mit der Implementierung zu aktualisieren.

## 10. Referenzen

- RFC-0001
- RFC-0001.1
- RFC-0001.2

## 11. Statushistorie

- 2026-07-11: Version 1.0 angenommen.
