# ADR-BOOTGFX-0010 – Hintergrundunschärfe für Glass-Dialoge

| Feld | Wert |
|---|---|
| Dokument-ID | ADR-BOOTGFX-0010 |
| Kategorie | NovaOS Bootmanager / Grafik und Compositing |
| Status | Angenommen |
| Version | 1.0 |
| Datum | 2026-08-03 |

## 1. Ziel

Diese ADR definiert die Architekturentscheidung für **Hintergrundunschärfe für Glass-Dialoge** innerhalb der Boot-Grafikpipeline von NovaOS.

## 2. Entscheidung

Die Funktion wird als fester Bestandteil der Boot-Rendering-Engine implementiert und muss sowohl unter BIOS/VBE als auch UEFI/GOP konsistent arbeiten.

## 3. Anforderungen

- Deterministisches Verhalten
- Plattformunabhängige Implementierung
- Kompatibel mit Software-Rendering
- Optional durch Hardwarebeschleunigung nutzbar
- Definierter Fallback
- Reproduzierbare Ausgabe

## 4. Technische Ausgestaltung

- Bestandteil der gemeinsamen Render-Pipeline
- Unterstützung durch Design-Tokens und Rendering-API
- Integration in Dirty-Region-Rendering
- Kompatibel mit Animationen und Glass-Design
- Optimiert für geringen Speicherverbrauch

## 5. Konsequenzen

### Vorteile

- Einheitliches Verhalten
- Hochwertige Darstellung
- Wiederverwendbare Architektur
- Gute Testbarkeit

### Nachteile

- Höherer Implementierungsaufwand
- Zusätzliche Tests erforderlich

## 6. Validierung

- Referenz-Renderbilder
- BIOS- und UEFI-Tests
- Performance-Messungen
- Speicherbudget einhalten
- Automatisierte Regressionstests

## 7. Abhängigkeiten

- ADR-BOOTUI-0001 bis 0012
- NPSPEC-BOOTRENDER-0001 ff.
- NPSPEC-BOOTCOMPOSITOR-0001 ff.

## 8. Revisionsverlauf

| Version | Datum | Änderung |
|---|---|---|
|1.0|2026-08-03|Erstfassung|
