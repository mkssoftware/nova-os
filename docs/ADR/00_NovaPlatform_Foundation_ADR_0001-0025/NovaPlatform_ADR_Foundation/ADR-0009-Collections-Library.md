# ADR-0009: Gemeinsame Collections Library

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Foundation Architecture
- **Erstellt:** 2026-07-11
- **Gültig für:** Nova Platform
- **Ersetzt:** Keine

---

## 1. Kontext

Viele Subsysteme benötigen Vector, Queue, Stack, Ringbuffer, Bitmap und HashMap.

## 2. Problemstellung

Mehrfache Eigenimplementierungen führen zu inkonsistentem Verhalten und Fehlern.

## 3. Betrachtete Alternativen

- Jedes Modul implementiert eigene Container
- Nur feste Arrays
- Gemeinsame freestanding Collections Library
- Übernahme einer externen STL-ähnlichen Bibliothek

## 4. Entscheidung

Nova entwickelt eine kleine, freestanding Nova Collections Library mit klaren Besitzregeln und optionalen Custom Allocators.

## 5. Begründung

- Wiederverwendung
- Einheitliche Fehlersemantik
- Hostbasierte Tests möglich
- Keine externe Runtime nötig

## 6. Positive Konsequenzen

- Weniger doppelter Code
- Bessere Testabdeckung
- Einheitliche APIs

## 7. Negative Konsequenzen und Risiken

- Foundation wird größer
- Generische Container in C erfordern void* oder Makros

## 8. Auswirkungen auf andere Module

Betrifft Object Manager, Event Queue, Scheduler, Plugins, UI und Storage.

## 9. Umsetzungsregeln

- Die Entscheidung ist für neue Implementierungen verbindlich.
- Abweichungen benötigen ein neues ADR oder eine dokumentierte Ersetzung dieses ADRs.
- Öffentliche APIs und persistente Formate müssen die festgelegten ABI- und Versionsregeln beachten.
- Tests und Dokumentation sind zusammen mit der Implementierung zu aktualisieren.

## 10. Referenzen

- RFC-0001

## 11. Statushistorie

- 2026-07-11: Version 1.0 angenommen.
