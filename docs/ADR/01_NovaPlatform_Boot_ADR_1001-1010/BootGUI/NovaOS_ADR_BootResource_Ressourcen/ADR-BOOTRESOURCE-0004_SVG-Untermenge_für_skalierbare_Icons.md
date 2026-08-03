# ADR-BOOTRESOURCE-0004 – SVG-Untermenge für skalierbare Icons

| Feld | Wert |
|---|---|
| Dokument-ID | ADR-BOOTRESOURCE-0004 |
| Kategorie | NovaOS Bootmanager / Ressourcen |
| Status | Angenommen |
| Version | 1.0 |
| Datum | 2026-08-03 |

## 1. Entscheidungskontext

Diese ADR beschreibt die Architekturentscheidung für **SVG-Untermenge für skalierbare Icons** innerhalb des Ressourcen- und Asset-Managements des NovaOS-Bootmanagers.

## 2. Entscheidung

Die Funktion wird als Bestandteil des Boot-Resource-Systems implementiert und steht allen UI-Komponenten einheitlich zur Verfügung.

## 3. Technische Ausgestaltung

- Plattformunabhängige Ressourcenverwaltung
- Integration in Rendering und UI
- Deterministische Lade- und Prüfprozesse
- Unterstützung für BIOS/VBE und UEFI/GOP
- Definierte Fallbacks
- Versionierte Ressourcen

## 4. Konsequenzen

### Vorteile
- Konsistente Ressourcenverwaltung
- Hohe Zuverlässigkeit
- Wiederverwendbare Architektur
- Gute Testbarkeit

### Nachteile
- Zusätzlicher Implementierungsaufwand
- Erhöhter Speicherbedarf für kritische Assets

## 5. Validierung

- Integritätstests
- BIOS-/UEFI-Kompatibilität
- Lade- und Performance-Tests
- Regressionstests

## 6. Abhängigkeiten

- ADR-BOOTUI-0001 bis 0012
- ADR-BOOTGFX-0001 bis 0012
- NPSPEC-BOOTRESOURCE-0001 ff.

## 7. Revisionsverlauf

| Version | Datum | Änderung |
|---|---|---|
|1.0|2026-08-03|Erstfassung|
