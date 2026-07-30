# ADR-0016: Einheitliche Modulstruktur mit include/src/tests/docs

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Foundation Architecture
- **Erstellt:** 2026-07-11
- **Gültig für:** Nova Platform
- **Ersetzt:** Keine

---

## 1. Kontext

Viele Subsysteme sollen unabhängig entwickelt und getestet werden.

## 2. Problemstellung

Uneinheitliche Verzeichnisse erschweren Navigation und Buildintegration.

## 3. Betrachtete Alternativen

- Freie Struktur je Modul
- Flache Repository-Struktur
- Einheitliches Modul-Template
- Monorepo mit nur globalen Ordnern

## 4. Entscheidung

Jedes größere Modul verwendet include/, src/, tests/ und docs/. Öffentliche Header liegen ausschließlich in include/.

## 5. Begründung

- Vorhersagbare Struktur
- Klare API-Grenzen
- Automatisierbarer Build
- Einfachere Dokumentation

## 6. Positive Konsequenzen

- Gute Wartbarkeit
- Leichtere Einarbeitung
- Weniger interne Header-Leaks

## 7. Negative Konsequenzen und Risiken

- Mehr Verzeichnisse und Boilerplate

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
