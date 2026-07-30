# ADR-0024: Laufzeit-Inspector für Objekte und Services

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Foundation Architecture
- **Erstellt:** 2026-07-11
- **Gültig für:** Nova Platform
- **Ersetzt:** Keine

---

## 1. Kontext

Komplexe Objekt-, Komponenten- und Servicegraphen sind ohne Werkzeug schwer zu verstehen.

## 2. Problemstellung

Logs allein zeigen nicht zuverlässig aktuellen Zustand und Beziehungen.

## 3. Betrachtete Alternativen

- Nur Logs
- Debugger-only
- Integrierter Read-only Inspector
- Vollständige Laufzeitmanipulation

## 4. Entscheidung

Nova entwickelt einen zunächst read-only Debug Inspector für Objekte, Komponenten, Services, Tasks, Speicher und Logs.

## 5. Begründung

- Sicherer als direkte Manipulation
- Hilft bei UI- und Serviceproblemen
- Nutzt ObjectIDs und Registry

## 6. Positive Konsequenzen

- Schnellere Diagnose
- Bessere Architekturtransparenz

## 7. Negative Konsequenzen und Risiken

- Zusätzliche Metadaten und Debug-Code
- Release-Build muss Umfang begrenzen

## 8. Auswirkungen auf andere Module

Betrifft Object Manager, Component System, Services, Tasks und UI.

## 9. Umsetzungsregeln

- Die Entscheidung ist für neue Implementierungen verbindlich.
- Abweichungen benötigen ein neues ADR oder eine dokumentierte Ersetzung dieses ADRs.
- Öffentliche APIs und persistente Formate müssen die festgelegten ABI- und Versionsregeln beachten.
- Tests und Dokumentation sind zusammen mit der Implementierung zu aktualisieren.

## 10. Referenzen

- RFC-0001.1
- RFC-0001.2

## 11. Statushistorie

- 2026-07-11: Version 1.0 angenommen.
