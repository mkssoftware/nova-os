# ADR-0023: Strukturierte Kernel Panic Reports

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Foundation Architecture
- **Erstellt:** 2026-07-11
- **Gültig für:** Nova Platform
- **Ersetzt:** Keine

---

## 1. Kontext

Nova Rescue soll Fehler des installierten Systems analysieren können.

## 2. Problemstellung

Ein einfacher Panic-Text reicht nicht für spätere Diagnose.

## 3. Betrachtete Alternativen

- Nur Bildschirmmeldung
- Serieller Dump
- Strukturierter Panic Report
- Vollständiger Core Dump

## 4. Entscheidung

Nova erzeugt einen versionierten Panic Report mit Registern, Stacktrace, Logs, BootInfo, Modulen, Speicherstatus und letztem NovaError. Ein vollständiger Core Dump bleibt optional.

## 5. Begründung

- Rescue kann Berichte analysieren
- Kleinere Dateien als Core Dumps
- Automatische Diagnose möglich

## 6. Positive Konsequenzen

- Bessere Fehlersuche
- Grundlage für Selbstheilung
- Nutzerfreundliche Reports

## 7. Negative Konsequenzen und Risiken

- Schreibpfad im Panic-Zustand ist riskant
- Format und Datenschutz müssen beachtet werden

## 8. Auswirkungen auf andere Module

Betrifft Kernel Panic, Logging, Nova Rescue und Storage.

## 9. Umsetzungsregeln

- Die Entscheidung ist für neue Implementierungen verbindlich.
- Abweichungen benötigen ein neues ADR oder eine dokumentierte Ersetzung dieses ADRs.
- Öffentliche APIs und persistente Formate müssen die festgelegten ABI- und Versionsregeln beachten.
- Tests und Dokumentation sind zusammen mit der Implementierung zu aktualisieren.

## 10. Referenzen

- RFC-0001

## 11. Statushistorie

- 2026-07-11: Version 1.0 angenommen.
