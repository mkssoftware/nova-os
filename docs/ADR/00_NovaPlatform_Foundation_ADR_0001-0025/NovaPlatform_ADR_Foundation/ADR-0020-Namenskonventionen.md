# ADR-0020: Modulpräfixe und englische technische Bezeichner

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Foundation Architecture
- **Erstellt:** 2026-07-11
- **Gültig für:** Nova Platform
- **Ersetzt:** Keine

---

## 1. Kontext

Das Projekt umfasst viele Bibliotheken und öffentliche APIs.

## 2. Problemstellung

Unpräfixierte Namen kollidieren leicht; gemischte Sprachen erschweren Konsistenz.

## 3. Betrachtete Alternativen

- Beliebige Namen
- C++-Namespaces
- Modulpräfixe in C
- Nur sehr kurze Präfixe

## 4. Entscheidung

Öffentliche APIs verwenden Modulpräfixe wie NGE_, NUI_, NAS_, NVFS_. Technische Identifier sind Englisch; Kommentare und Dokumentation dürfen Deutsch sein.

## 5. Begründung

- Keine Namenskollisionen
- Klare Modulzuordnung
- Bessere internationale Lesbarkeit

## 6. Positive Konsequenzen

- Konsistente API
- Einfachere Suche
- Saubere Symboltabellen

## 7. Negative Konsequenzen und Risiken

- Längere Funktionsnamen

## 8. Auswirkungen auf andere Module

Betrifft alle öffentlichen Symbole.

## 9. Umsetzungsregeln

- Die Entscheidung ist für neue Implementierungen verbindlich.
- Abweichungen benötigen ein neues ADR oder eine dokumentierte Ersetzung dieses ADRs.
- Öffentliche APIs und persistente Formate müssen die festgelegten ABI- und Versionsregeln beachten.
- Tests und Dokumentation sind zusammen mit der Implementierung zu aktualisieren.

## 10. Referenzen

- RFC-0001

## 11. Statushistorie

- 2026-07-11: Version 1.0 angenommen.
