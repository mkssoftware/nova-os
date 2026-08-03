# ADR-BOOTCONTROL-0006 – Datengebundene Menüeinträge

| Feld | Wert |
|---|---|
| Dokument-ID | ADR-BOOTCONTROL-0006 |
| Kategorie | NovaOS Bootmanager / Bedienelemente |
| Status | Angenommen |
| Version | 1.0 |
| Datum | 2026-08-03 |

## 1. Entscheidungskontext

Diese ADR beschreibt die Architekturentscheidung für **Datengebundene Menüeinträge** innerhalb des Control-Frameworks des NovaOS-Bootmanagers.

## 2. Entscheidung

Die Funktion wird als verpflichtender Bestandteil des Boot-Control-Frameworks implementiert und steht allen Oberflächen des Bootmanagers konsistent zur Verfügung.

## 3. Technische Ausgestaltung

- Integration in Scene Graph und Layout Engine
- Einheitliches Rendering
- Einheitliches Fokus- und Eingabemodell
- Unterstützung von Animationen und Glass-Design
- Plattformunabhängig (BIOS/VBE und UEFI/GOP)
- Deterministische Ausführung

## 4. Konsequenzen

### Vorteile
- Wiederverwendbare UI-Komponenten
- Konsistente Bedienung
- Einfachere Wartbarkeit
- Gute Testbarkeit

### Nachteile
- Höherer Implementierungsaufwand
- Größere Testmatrix

## 5. Validierung

- Rendering-Tests
- Fokus- und Eingabetests
- BIOS-/UEFI-Kompatibilität
- Performance-Tests
- Regressionstests

## 6. Abhängigkeiten

- ADR-BOOTUI-0001 bis 0012
- ADR-BOOTDIALOG-0001 bis 0005
- ADR-BOOTNAV-0001 bis 0004
- NPSPEC-BOOTCONTROL-0001 ff.

## 7. Revisionsverlauf

| Version | Datum | Änderung |
|---|---|---|
|1.0|2026-08-03|Erstfassung|
