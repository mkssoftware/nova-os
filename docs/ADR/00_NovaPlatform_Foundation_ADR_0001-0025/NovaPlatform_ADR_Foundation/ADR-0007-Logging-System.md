# ADR-0007: Zentrales mehrkanaliges Logging

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Foundation Architecture
- **Erstellt:** 2026-07-11
- **Gültig für:** Nova Platform
- **Ersetzt:** Keine

---

## 1. Kontext

Bootloader, Kernel, Treiber und Anwendungen benötigen konsistente Diagnoseausgaben.

## 2. Problemstellung

printf ist im freestanding Umfeld nicht verfügbar und ein einzelner Ausgabekanal reicht für frühe Bootphasen und Laufzeitdiagnose nicht aus.

## 3. Betrachtete Alternativen

- Direkte Bildschirmmeldungen
- Nur serielle Ausgabe
- Modulspezifische Logger
- Zentraler Logger mit Sinks

## 4. Entscheidung

Nova verwendet einen zentralen Logger mit Log-Level, Modul-ID und austauschbaren Sinks für seriell, Ringpuffer, Bildschirm, Datei und Netzwerk.

## 5. Begründung

- Einheitliches Format
- Frühe Bootdiagnose
- Mehrere Ziele gleichzeitig
- Spätere Filterung und Export möglich

## 6. Positive Konsequenzen

- Bessere Fehlersuche
- Panic Report kann letzte Logs übernehmen
- Module bleiben unabhängig vom Ausgabemedium

## 7. Negative Konsequenzen und Risiken

- Logger muss in frühen Bootphasen minimal funktionieren
- Reentrancy und Interrupt-Sicherheit müssen später berücksichtigt werden

## 8. Auswirkungen auf andere Module

Betrifft alle Module.

## 9. Umsetzungsregeln

- Die Entscheidung ist für neue Implementierungen verbindlich.
- Abweichungen benötigen ein neues ADR oder eine dokumentierte Ersetzung dieses ADRs.
- Öffentliche APIs und persistente Formate müssen die festgelegten ABI- und Versionsregeln beachten.
- Tests und Dokumentation sind zusammen mit der Implementierung zu aktualisieren.

## 10. Referenzen

- RFC-0001

## 11. Statushistorie

- 2026-07-11: Version 1.0 angenommen.
