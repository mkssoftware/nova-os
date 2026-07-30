# ADR-0014: Semantische Versionierung mit Produkt- und ABI-Versionen

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Foundation Architecture
- **Erstellt:** 2026-07-11
- **Gültig für:** Nova Platform
- **Ersetzt:** Keine

---

## 1. Kontext

Plattform, Produkte, Module und Dateiformate entwickeln sich unabhängig.

## 2. Problemstellung

Eine einzelne Versionsnummer reicht nicht für API-, ABI- und Formatkompatibilität.

## 3. Betrachtete Alternativen

- Nur Buildnummer
- Kalenderbasierte Versionierung
- SemVer für alles
- SemVer plus separate ABI- und Formatversionen

## 4. Entscheidung

Produkte und Module verwenden SemVer. ABI und persistente Formate besitzen eigene explizite Versionsfelder.

## 5. Begründung

- Klare Kompatibilitätsaussagen
- Module können unabhängig fortschreiten
- Dateiformate bleiben prüfbar

## 6. Positive Konsequenzen

- Bessere Releases
- Saubere Migrationen
- Kompatibilitätsprüfungen möglich

## 7. Negative Konsequenzen und Risiken

- Mehrere Versionsangaben müssen gepflegt werden

## 8. Auswirkungen auf andere Module

Betrifft alle öffentlichen APIs, Module und Asset-/Dateiformate.

## 9. Umsetzungsregeln

- Die Entscheidung ist für neue Implementierungen verbindlich.
- Abweichungen benötigen ein neues ADR oder eine dokumentierte Ersetzung dieses ADRs.
- Öffentliche APIs und persistente Formate müssen die festgelegten ABI- und Versionsregeln beachten.
- Tests und Dokumentation sind zusammen mit der Implementierung zu aktualisieren.

## 10. Referenzen

- RFC-0001

## 11. Statushistorie

- 2026-07-11: Version 1.0 angenommen.
