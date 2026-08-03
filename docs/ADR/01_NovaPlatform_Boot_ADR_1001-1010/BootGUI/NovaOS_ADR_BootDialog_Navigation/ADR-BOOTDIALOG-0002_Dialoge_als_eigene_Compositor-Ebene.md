# ADR-BOOTDIALOG-0002 – Dialoge als eigene Compositor-Ebene

| Feld | Wert |
|---|---|
| Dokument-ID | ADR-BOOTDIALOG-0002 |
| Kategorie | NovaOS Bootmanager / Dialoge |
| Status | Angenommen |
| Version | 1.0 |
| Datum | 2026-08-03 |

## 1. Entscheidungskontext

Diese ADR beschreibt die Architekturentscheidung für **Dialoge als eigene Compositor-Ebene** innerhalb der Dialog- und Navigationsarchitektur des NovaOS-Bootmanagers.

## 2. Entscheidung

Die Funktion wird als fester Bestandteil der Boot-UI implementiert und arbeitet einheitlich auf BIOS/VBE- sowie UEFI/GOP-Systemen.

## 3. Technische Ausgestaltung

- Integration in Scene Graph und Compositor
- Deterministisches Verhalten
- Unterstützung von Animationen und Glass-Design
- Einheitliches Fokus- und Eingabemodell
- Definierte Fehler- und Fallbackpfade

## 4. Konsequenzen

### Vorteile
- Konsistente Bedienung
- Wiederverwendbare Architektur
- Einfache Erweiterbarkeit
- Gute Testbarkeit

### Nachteile
- Zusätzlicher Implementierungsaufwand
- Umfangreichere Testmatrix

## 5. Validierung

- BIOS- und UEFI-Tests
- Navigationstests
- Dialog- und Fokus-Tests
- Regressionstests
- Performance-Messungen

## 6. Abhängigkeiten

- ADR-BOOTUI-0001 bis 0012
- ADR-BOOTMOTION-0001 bis 0008
- NPSPEC-BOOTDIALOG-0001 ff.
- NPSPEC-BOOTNAV-0001 ff.

## 7. Revisionsverlauf

| Version | Datum | Änderung |
|---|---|---|
|1.0|2026-08-03|Erstfassung|
