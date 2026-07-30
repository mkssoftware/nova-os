# ADR-0006: Trennung von NovaStatus und NovaError

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Foundation Architecture
- **Erstellt:** 2026-07-11
- **Gültig für:** Nova Platform
- **Ersetzt:** Keine

---

## 1. Kontext

Systemfunktionen benötigen kompakte Rückgabecodes, während Diagnose und Benutzeroberfläche detaillierte Fehlerinformationen benötigen.

## 2. Problemstellung

Ein einziger boolescher Rückgabewert ist zu ungenau; vollständige Fehlerobjekte für jeden Aufruf wären zu teuer.

## 3. Betrachtete Alternativen

- bool-Rückgaben
- errno-artiger globaler Zustand
- nur strukturierte Fehlerobjekte
- NovaStatus plus optionaler NovaError

## 4. Entscheidung

Öffentliche APIs geben NovaStatus zurück. Detaillierte Informationen werden bei Bedarf in NovaError oder NovaErrorObject erfasst.

## 5. Begründung

- Schneller Standardpfad
- Präzise Statussemantik
- Detaillierte Diagnose nur bei Bedarf
- Geeignet für Kernel und Benutzeroberfläche

## 6. Positive Konsequenzen

- Klare Fehlerbehandlung
- Geringer Overhead
- Gute Logs und Panic Reports

## 7. Negative Konsequenzen und Risiken

- Fehlerdetails können verloren gehen, wenn sie nicht rechtzeitig kopiert werden
- Thread-lokaler oder kontextbezogener Fehlerzustand wird später benötigt

## 8. Auswirkungen auf andere Module

Betrifft alle öffentlichen APIs, Logging, Panic Reports und UI-Fehlerdialoge.

## 9. Umsetzungsregeln

- Die Entscheidung ist für neue Implementierungen verbindlich.
- Abweichungen benötigen ein neues ADR oder eine dokumentierte Ersetzung dieses ADRs.
- Öffentliche APIs und persistente Formate müssen die festgelegten ABI- und Versionsregeln beachten.
- Tests und Dokumentation sind zusammen mit der Implementierung zu aktualisieren.

## 10. Referenzen

- RFC-0001

## 11. Statushistorie

- 2026-07-11: Version 1.0 angenommen.
