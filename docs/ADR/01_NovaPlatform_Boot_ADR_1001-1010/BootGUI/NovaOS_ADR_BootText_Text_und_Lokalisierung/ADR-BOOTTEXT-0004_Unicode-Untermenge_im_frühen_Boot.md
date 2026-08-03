# ADR-BOOTTEXT-0004 – Unicode-Untermenge im frühen Boot

| Feld | Wert |
|---|---|
| Dokument-ID | ADR-BOOTTEXT-0004 |
| Kategorie | NovaOS Bootmanager / Text und Lokalisierung |
| Status | Angenommen |
| Version | 1.0 |
| Datum | 2026-08-03 |

## 1. Entscheidungskontext

Diese ADR beschreibt die Architekturentscheidung für **Unicode-Untermenge im frühen Boot** innerhalb des Text- und Lokalisierungssystems des NovaOS-Bootmanagers.

## 2. Entscheidung

Die Funktion wird als Bestandteil der Boot-Text-Engine implementiert und bildet die Grundlage für eine konsistente Darstellung und Lokalisierung aller Texte im Bootmanager.

## 3. Technische Ausgestaltung

- Plattformunabhängige Text-Engine
- Unterstützung für BIOS/VBE und UEFI/GOP
- Deterministische Textdarstellung
- Integration in Rendering, Layout und UI
- Definierte Fallbacks
- Versionierte Ressourcen

## 4. Konsequenzen

### Vorteile
- Einheitliche Textdarstellung
- Internationale Erweiterbarkeit
- Gute Wartbarkeit
- Hohe Testbarkeit

### Nachteile
- Zusätzlicher Implementierungsaufwand
- Größerer Ressourcenbedarf für Schriftarten

## 5. Validierung

- Rendering-Tests
- Zeichensatz-Tests
- BIOS-/UEFI-Kompatibilität
- Performance-Tests
- Regressionstests

## 6. Abhängigkeiten

- ADR-BOOTUI-0001 bis 0012
- ADR-BOOTRESOURCE-0001 bis 0010
- NPSPEC-BOOTTEXT-0001 ff.

## 7. Revisionsverlauf

| Version | Datum | Änderung |
|---|---|---|
|1.0|2026-08-03|Erstfassung|
