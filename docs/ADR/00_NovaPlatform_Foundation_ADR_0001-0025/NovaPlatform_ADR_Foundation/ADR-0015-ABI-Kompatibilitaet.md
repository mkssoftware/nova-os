# ADR-0015: Explizite ABI-Stabilitätsregeln

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Foundation Architecture
- **Erstellt:** 2026-07-11
- **Gültig für:** Nova Platform
- **Ersetzt:** Keine

---

## 1. Kontext

Module, Plugins, BootInfo und persistente Formate benötigen stabile Binärschnittstellen.

## 2. Problemstellung

Unkontrollierte Enum-Änderungen oder Strukturverschiebungen brechen Binärkompatibilität.

## 3. Betrachtete Alternativen

- Keine ABI-Garantie
- Compilerabhängige Strukturen
- Versionierte Strukturen und explizite Werte
- Nur Quellcodekompatibilität

## 4. Entscheidung

Öffentliche Enums erhalten explizite Werte. Öffentliche Strukturen enthalten StructSize und AbiVersion. Neue Felder werden angehängt.

## 5. Begründung

- Vorwärtskompatible Validierung
- Plugins und Module können sicher prüfen
- Compilerlayout wird kontrollierbarer

## 6. Positive Konsequenzen

- Stabilere Module
- Bessere Fehlerdiagnose
- Sichere Erweiterbarkeit

## 7. Negative Konsequenzen und Risiken

- Strukturen werden größer
- Änderungen benötigen mehr Disziplin

## 8. Auswirkungen auf andere Module

Betrifft SDK, Plugins, BootInfo, Services und Dateiformate.

## 9. Umsetzungsregeln

- Die Entscheidung ist für neue Implementierungen verbindlich.
- Abweichungen benötigen ein neues ADR oder eine dokumentierte Ersetzung dieses ADRs.
- Öffentliche APIs und persistente Formate müssen die festgelegten ABI- und Versionsregeln beachten.
- Tests und Dokumentation sind zusammen mit der Implementierung zu aktualisieren.

## 10. Referenzen

- RFC-0001
- RFC-0001.1

## 11. Statushistorie

- 2026-07-11: Version 1.0 angenommen.
