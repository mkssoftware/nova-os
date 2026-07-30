# ADR-9004: Image Builder

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Build & Toolchain
- **Erstellt:** 2026-07-11

## 1. Kontext

Dieses ADR definiert die Architekturentscheidung für **Image Builder** innerhalb der Nova-Entwicklungswerkzeuge.

## 2. Problemstellung

Die Plattform besteht aus Bootloader, Kernel, HAL, Bibliotheken, Anwendungen, Assets und Dokumentation. Der Buildprozess muss reproduzierbar, automatisierbar und plattformübergreifend sein.

## 3. Entscheidung

Die Build- und Toolchain wird modular aufgebaut. Quellcode, Assets, Dokumentation und Tests werden über klar definierte Buildschritte verarbeitet. Alle Artefakte sind versioniert und reproduzierbar erzeugbar.

## 4. Begründung

- Reproduzierbare Builds
- Automatisierte Tests
- Einheitliche Entwicklungsumgebung
- Klare Trennung der Buildschritte

## 5. Konsequenzen

### Positiv

- Nachvollziehbare Releases
- Geringere Fehlerquote
- Gute CI/CD-Unterstützung

### Risiken

- Zusätzlicher Wartungsaufwand für Buildskripte
- Toolchain-Versionen müssen kontrolliert werden

## 6. Auswirkungen

Dieses ADR betrifft alle Buildwerkzeuge, Generatoren und automatisierten Testprozesse.

## 7. Referenzen

- RFC-0001
- ADR-0013
- ADR-0018
- ADR-0019

## 8. Statushistorie

- 2026-07-11: Version 1.0 angenommen.
