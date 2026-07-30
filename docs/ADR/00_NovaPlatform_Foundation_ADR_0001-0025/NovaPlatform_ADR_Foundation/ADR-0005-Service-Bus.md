# ADR-0005: Services über Registry und Message Bus

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Foundation Architecture
- **Erstellt:** 2026-07-11
- **Gültig für:** Nova Platform
- **Ersetzt:** Keine

---

## 1. Kontext

Anwendungen und Komponenten benötigen Zugriff auf Storage, VFS, Assets, Themes, Logging, Tasks und weitere Systemfunktionen. Direkte Modulabhängigkeiten würden das System stark koppeln.

## 2. Problemstellung

Systemdienste sollen auffindbar, austauschbar und später auch über IPC oder Plugins verfügbar sein.

## 3. Betrachtete Alternativen

- Direkte Funktionsaufrufe zwischen Modulen
- Globaler Service Locator ohne Messaging
- Message Bus für alle Operationen
- Kombination aus Service Registry und Message Bus

## 4. Entscheidung

Nova verwendet eine Service Registry für synchrone, klar typisierte APIs und einen Message Bus für lose gekoppelte, asynchrone Kommunikation.

## 5. Begründung

- Direkte APIs bleiben für performancekritische Pfade möglich.
- Asynchrone Ereignisse und Tasks können über Nachrichten laufen.
- Dienste lassen sich ersetzen oder mocken.
- Plugins können neue Services registrieren.

## 6. Positive Konsequenzen

- Geringere Kopplung
- Bessere Testbarkeit
- Grundlage für IPC und Plugins
- Klare Servicegrenzen

## 7. Negative Konsequenzen und Risiken

- Zusätzliche Infrastruktur
- Fehler bei nicht registrierten Diensten müssen behandelt werden
- Nachrichtenformate benötigen Versionierung

## 8. Auswirkungen auf andere Module

Betrifft Core Services, Anwendungen, Plugins, Event System, IPC und Tests.

## 9. Umsetzungsregeln

- Die Entscheidung ist für neue Implementierungen verbindlich.
- Abweichungen benötigen ein neues ADR oder eine dokumentierte Ersetzung dieses ADRs.
- Öffentliche APIs und persistente Formate müssen die festgelegten ABI- und Versionsregeln beachten.
- Tests und Dokumentation sind zusammen mit der Implementierung zu aktualisieren.

## 10. Referenzen

- RFC-0001

## 11. Statushistorie

- 2026-07-11: Version 1.0 angenommen.
