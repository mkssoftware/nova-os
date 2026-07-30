# ADR-0021: Explizite statt implizite Thread-Sicherheit

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Foundation Architecture
- **Erstellt:** 2026-07-11
- **Gültig für:** Nova Platform
- **Ersetzt:** Keine

---

## 1. Kontext

Frühe Nova-Versionen starten weitgehend single-threaded; später kommen Scheduler und Multicore hinzu.

## 2. Problemstellung

Automatische Locks in jeder Foundation-Funktion wären früh unnötig und später möglicherweise ineffizient.

## 3. Betrachtete Alternativen

- Alles thread-safe
- Nichts thread-safe ohne Dokumentation
- Explizite Thread-Safety-Verträge
- Lock-free überall

## 4. Entscheidung

Jede API dokumentiert ihren Thread-Safety-Vertrag. Foundation-Container sind zunächst nicht intern synchronisiert, bieten aber spätere Lock- oder Concurrent-Varianten.

## 5. Begründung

- Kein unnötiger Overhead
- Klare Verantwortlichkeit
- Spätere Optimierung möglich

## 6. Positive Konsequenzen

- Einfachere frühe Implementierung
- Gezielte Synchronisation

## 7. Negative Konsequenzen und Risiken

- Aufrufer müssen Verträge beachten
- Fehler bei falscher Nutzung möglich

## 8. Auswirkungen auf andere Module

Betrifft Collections, Object Manager, Services, Logging und Scheduler.

## 9. Umsetzungsregeln

- Die Entscheidung ist für neue Implementierungen verbindlich.
- Abweichungen benötigen ein neues ADR oder eine dokumentierte Ersetzung dieses ADRs.
- Öffentliche APIs und persistente Formate müssen die festgelegten ABI- und Versionsregeln beachten.
- Tests und Dokumentation sind zusammen mit der Implementierung zu aktualisieren.

## 10. Referenzen

- RFC-0001

## 11. Statushistorie

- 2026-07-11: Version 1.0 angenommen.
