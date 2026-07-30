# ADR-0022: Trennung von Build- und Laufzeitkonfiguration

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Foundation Architecture
- **Erstellt:** 2026-07-11
- **Gültig für:** Nova Platform
- **Ersetzt:** Keine

---

## 1. Kontext

Einige Optionen beeinflussen Binärumfang, andere sollen zur Laufzeit änderbar sein.

## 2. Problemstellung

Alle Einstellungen in C-Defines zu halten erfordert Neubuilds; alles dynamisch zu machen vergrößert frühe Bootpfade.

## 3. Betrachtete Alternativen

- Nur Compile-Time-Defines
- Nur Laufzeitkonfiguration
- Getrennte Build- und Laufzeitkonfiguration
- Externe Registry für alles

## 4. Entscheidung

Buildoptionen werden aus Konfigurationsdateien in generierte Header überführt. Laufzeitoptionen werden über versionierte Konfigurationsassets geladen.

## 5. Begründung

- Kleine Binärdateien
- Änderbare Themes und Einstellungen
- Reproduzierbare Builds

## 6. Positive Konsequenzen

- Saubere Trennung
- Automatisierbar
- Weniger Hardcoding

## 7. Negative Konsequenzen und Risiken

- Generator und Schema nötig
- Fehlerhafte Konfiguration muss validiert werden

## 8. Auswirkungen auf andere Module

Betrifft Buildsystem, Boot Manager, Kernel und UI.

## 9. Umsetzungsregeln

- Die Entscheidung ist für neue Implementierungen verbindlich.
- Abweichungen benötigen ein neues ADR oder eine dokumentierte Ersetzung dieses ADRs.
- Öffentliche APIs und persistente Formate müssen die festgelegten ABI- und Versionsregeln beachten.
- Tests und Dokumentation sind zusammen mit der Implementierung zu aktualisieren.

## 10. Referenzen

- RFC-0001

## 11. Statushistorie

- 2026-07-11: Version 1.0 angenommen.
