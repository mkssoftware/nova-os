# ADR-0004: Komposition durch ein Component System

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Foundation Architecture
- **Erstellt:** 2026-07-11
- **Gültig für:** Nova Platform
- **Ersetzt:** Keine

---

## 1. Kontext

Komplexe Objekte wie Fenster, Buttons, Datenträger oder Anwendungen benötigen optionale Fähigkeiten. Monolithische Strukturen würden viele ungenutzte Felder und starke Kopplung erzeugen.

## 2. Problemstellung

Fähigkeiten wie Rendern, Input, Theme, Layout, Audit oder Health sollen modular ergänzt und unabhängig getestet werden können.

## 3. Betrachtete Alternativen

- Tiefe Vererbungshierarchien
- Monolithische Objekte
- Komponentenbasierte Komposition
- Vollständiges datenorientiertes ECS

## 4. Entscheidung

Nova verwendet ein komponentenbasiertes System. NovaObject liefert Identität und Lebenszyklus; Komponenten liefern Fähigkeiten und spezialisierten Zustand.

## 5. Begründung

- Komposition ist flexibler als tiefe Vererbung.
- Komponenten können in mehreren Objekttypen wiederverwendet werden.
- Optionale Fähigkeiten verursachen keinen Ballast in jedem Objekt.
- Plugins können neue Komponentenarten registrieren.

## 6. Positive Konsequenzen

- Hohe Wiederverwendbarkeit
- Klare Verantwortlichkeiten
- Einfachere Tests
- Laufzeitkomposition möglich

## 7. Negative Konsequenzen und Risiken

- Lookup- und Dispatch-Overhead
- Lebenszyklus und Abhängigkeiten müssen streng definiert sein
- Nicht jede Low-Level-Struktur sollte Komponenten verwenden

## 8. Auswirkungen auf andere Module

Betrifft UI Framework, Window Manager, Anwendungen, Services, Storage-Objekte und Plugin-System.

## 9. Umsetzungsregeln

- Die Entscheidung ist für neue Implementierungen verbindlich.
- Abweichungen benötigen ein neues ADR oder eine dokumentierte Ersetzung dieses ADRs.
- Öffentliche APIs und persistente Formate müssen die festgelegten ABI- und Versionsregeln beachten.
- Tests und Dokumentation sind zusammen mit der Implementierung zu aktualisieren.

## 10. Referenzen

- RFC-0001.2

## 11. Statushistorie

- 2026-07-11: Version 1.0 angenommen.
