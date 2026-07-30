# ADR-0012: Datengetriebenes Theme-System

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Foundation Architecture
- **Erstellt:** 2026-07-11
- **Gültig für:** Nova Platform
- **Ersetzt:** Keine

---

## 1. Kontext

Nova Rescue, Boot Manager und NovaOS sollen konsistent, aber anpassbar aussehen.

## 2. Problemstellung

Fest kodierte Farben, Abstände und Fonts erschweren Wiederverwendung und Barrierefreiheit.

## 3. Betrachtete Alternativen

- Fest kodierte Styles
- Themes als C-Strukturen
- Externe Theme-Dateien
- CSS-kompatible Engine

## 4. Entscheidung

Nova verwendet versionierte Theme-Assets mit Design Tokens für Farben, Typografie, Abstände, Radien, Schatten und Animationen.

## 5. Begründung

- Trennung von Logik und Design
- Gemeinsames Designsystem
- High-Contrast und alternative Themes möglich
- Keine CSS-Engine im Kernel nötig

## 6. Positive Konsequenzen

- Konsistentes UI
- Einfachere Designänderungen
- Wiederverwendbar in allen Produkten

## 7. Negative Konsequenzen und Risiken

- Theme-Parser und Validierung nötig
- Widgets müssen Token statt direkter Werte verwenden

## 8. Auswirkungen auf andere Module

Betrifft NUI, NGE, NAS und alle grafischen Anwendungen.

## 9. Umsetzungsregeln

- Die Entscheidung ist für neue Implementierungen verbindlich.
- Abweichungen benötigen ein neues ADR oder eine dokumentierte Ersetzung dieses ADRs.
- Öffentliche APIs und persistente Formate müssen die festgelegten ABI- und Versionsregeln beachten.
- Tests und Dokumentation sind zusammen mit der Implementierung zu aktualisieren.

## 10. Referenzen

- RFC-0001

## 11. Statushistorie

- 2026-07-11: Version 1.0 angenommen.
