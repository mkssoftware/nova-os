# ADR-0013: Modulares Make-basiertes Build-System

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Foundation Architecture
- **Erstellt:** 2026-07-11
- **Gültig für:** Nova Platform
- **Ersetzt:** Keine

---

## 1. Kontext

Nova besteht aus Bootloader, Kernel, Bibliotheken, Tools, Assets, Images und Tests.

## 2. Problemstellung

Ein monolithisches Makefile wird schnell unübersichtlich.

## 3. Betrachtete Alternativen

- Ein einzelnes Makefile
- CMake
- Meson
- GNU Make mit eingebundenen Moduldateien

## 4. Entscheidung

Die erste Toolchain verwendet GNU Make mit modularen .mk-Dateien. Host-Werkzeuge dürfen eigene Buildsysteme verwenden.

## 5. Begründung

- Transparente Befehle
- Gute Eignung für freestanding Builds
- Einfaches QEMU- und Image-Target
- Geringe Hostabhängigkeit

## 6. Positive Konsequenzen

- Nachvollziehbarer Build
- Leicht debugbar
- Modulare Targets

## 7. Negative Konsequenzen und Risiken

- Abhängigkeitsverwaltung muss sorgfältig gepflegt werden
- Plattformübergreifende Host-Unterstützung braucht Skripte

## 8. Auswirkungen auf andere Module

Betrifft gesamtes Repository.

## 9. Umsetzungsregeln

- Die Entscheidung ist für neue Implementierungen verbindlich.
- Abweichungen benötigen ein neues ADR oder eine dokumentierte Ersetzung dieses ADRs.
- Öffentliche APIs und persistente Formate müssen die festgelegten ABI- und Versionsregeln beachten.
- Tests und Dokumentation sind zusammen mit der Implementierung zu aktualisieren.

## 10. Referenzen

- RFC-0001

## 11. Statushistorie

- 2026-07-11: Version 1.0 angenommen.
