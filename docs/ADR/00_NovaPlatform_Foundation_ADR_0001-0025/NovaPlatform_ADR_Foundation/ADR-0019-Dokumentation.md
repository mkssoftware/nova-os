# ADR-0019: RFC, ADR, DES und API-Dokumentation

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Foundation Architecture
- **Erstellt:** 2026-07-11
- **Gültig für:** Nova Platform
- **Ersetzt:** Keine

---

## 1. Kontext

Nova ist ein langfristiges Plattformprojekt mit vielen Architekturentscheidungen.

## 2. Problemstellung

Entscheidungen nur im Chat oder Code sind später schwer nachvollziehbar.

## 3. Betrachtete Alternativen

- Nur README-Dateien
- Nur Codekommentare
- RFC/ADR/DES-System
- Externe Wiki ohne Versionskontrolle

## 4. Entscheidung

Architekturregeln werden als RFC, Entscheidungen als ADR, internes Design als DES und APIs separat dokumentiert.

## 5. Begründung

- Versionierbar im Repository
- Klare Rollen der Dokumentarten
- Langfristige Nachvollziehbarkeit

## 6. Positive Konsequenzen

- Bessere Wartung
- Einfachere Einarbeitung
- Architekturbrüche werden sichtbar

## 7. Negative Konsequenzen und Risiken

- Dokumentationsaufwand
- Dokumente müssen mit Code synchron gehalten werden

## 8. Auswirkungen auf andere Module

Betrifft gesamte Projektorganisation.

## 9. Umsetzungsregeln

- Die Entscheidung ist für neue Implementierungen verbindlich.
- Abweichungen benötigen ein neues ADR oder eine dokumentierte Ersetzung dieses ADRs.
- Öffentliche APIs und persistente Formate müssen die festgelegten ABI- und Versionsregeln beachten.
- Tests und Dokumentation sind zusammen mit der Implementierung zu aktualisieren.

## 10. Referenzen

- RFC-0001

## 11. Statushistorie

- 2026-07-11: Version 1.0 angenommen.
