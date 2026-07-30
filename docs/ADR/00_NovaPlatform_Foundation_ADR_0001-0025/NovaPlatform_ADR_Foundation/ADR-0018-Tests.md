# ADR-0018: Mehrstufige Teststrategie

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Foundation Architecture
- **Erstellt:** 2026-07-11
- **Gültig für:** Nova Platform
- **Ersetzt:** Keine

---

## 1. Kontext

Kernelcode ist schwer ausschließlich auf echter Hardware zu testen.

## 2. Problemstellung

Fehler in Algorithmen, Parsern und Collections sollen früh erkannt werden.

## 3. Betrachtete Alternativen

- Nur manuelle QEMU-Tests
- Nur Hardwaretests
- Host-Unit-Tests plus QEMU-Integration
- Formale Verifikation

## 4. Entscheidung

Nova verwendet Host-Unit-Tests für portable Logik, QEMU-Integrationstests für Boot/Kernel und ausgewählte Hardwaretests.

## 5. Begründung

- Schnelle Rückmeldung
- Reproduzierbare Boottests
- Hardwareabhängige Pfade bleiben abgedeckt

## 6. Positive Konsequenzen

- Weniger Regressionen
- Bessere Refactorings
- Automatisierbar

## 7. Negative Konsequenzen und Risiken

- Test-Harness und Mocks müssen gepflegt werden

## 8. Auswirkungen auf andere Module

Betrifft alle Module.

## 9. Umsetzungsregeln

- Die Entscheidung ist für neue Implementierungen verbindlich.
- Abweichungen benötigen ein neues ADR oder eine dokumentierte Ersetzung dieses ADRs.
- Öffentliche APIs und persistente Formate müssen die festgelegten ABI- und Versionsregeln beachten.
- Tests und Dokumentation sind zusammen mit der Implementierung zu aktualisieren.

## 10. Referenzen

- RFC-0001

## 11. Statushistorie

- 2026-07-11: Version 1.0 angenommen.
