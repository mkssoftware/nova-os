# ADR-BOOTACCESS-0001 – Tastaturbedienbarkeit aller Funktionen

| Feld | Wert |
|---|---|
| Dokument-ID | ADR-BOOTACCESS-0001 |
| Kategorie | NovaOS Bootmanager / Barrierefreiheit |
| Status | Angenommen |
| Version | 1.0 |
| Datum | 2026-08-03 |

## 1. Entscheidungskontext

Diese ADR beschreibt die Architekturentscheidung für **Tastaturbedienbarkeit aller Funktionen** innerhalb des Design- und Accessibility-Systems des NovaOS-Bootmanagers.

## 2. Entscheidung

Die Funktion wird als verbindlicher Bestandteil der Bootoberfläche implementiert und gilt für BIOS/VBE und UEFI/GOP gleichermaßen.

## 3. Technische Ausgestaltung

- Integration in das Design-System
- Einheitliche Design-Tokens
- Plattformunabhängige Umsetzung
- Deterministische Darstellung
- Definierte Fallbacks
- Hohe Wartbarkeit

## 4. Konsequenzen

### Vorteile
- Konsistente Benutzeroberfläche
- Verbesserte Zugänglichkeit
- Wiederverwendbare Architektur
- Gute Testbarkeit

### Nachteile
- Zusätzlicher Implementierungsaufwand
- Erweiterte Testmatrix

## 5. Validierung

- Design-Reviews
- Accessibility-Tests
- BIOS-/UEFI-Kompatibilität
- Performance-Tests
- Regressionstests

## 6. Abhängigkeiten

- ADR-BOOTUI-0001 bis 0012
- ADR-BOOTRESOURCE-0001 bis 0010
- NPSPEC-BOOTDESIGN-0001 ff.
- NPSPEC-BOOTACCESS-0001 ff.

## 7. Revisionsverlauf

| Version | Datum | Änderung |
|---|---|---|
|1.0|2026-08-03|Erstfassung|
