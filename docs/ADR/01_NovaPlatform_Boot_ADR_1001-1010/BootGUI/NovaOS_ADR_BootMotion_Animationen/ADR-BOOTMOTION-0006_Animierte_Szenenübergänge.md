# ADR-BOOTMOTION-0006 – Animierte Szenenübergänge

| Feld | Wert |
|---|---|
| Dokument-ID | ADR-BOOTMOTION-0006 |
| Kategorie | NovaOS Bootmanager / Animationen |
| Status | Angenommen |
| Version | 1.0 |
| Datum | 2026-08-03 |

## 1. Entscheidungskontext

Der Bootmanager soll bereits vor dem Kernelstart hochwertige, flüssige und konsistente Animationen bereitstellen. Gleichzeitig müssen Speicherverbrauch, Determinismus und Bootzeit kontrollierbar bleiben.

## 2. Entscheidung

**Animierte Szenenübergänge** wird als verbindlicher Bestandteil des NovaOS-Boot-Motion-Systems implementiert und sowohl im Software-Renderer als auch optional in hardwarebeschleunigten Backends unterstützt.

## 3. Technische Ausgestaltung

- Bestandteil der zentralen Motion Engine
- Deterministische Zeitbasis
- Integration in Scene Graph und Rendering
- Unterstützt Dialoge, Glass-Design und Navigation
- Definierte Qualitätsprofile und Fallbacks
- Keine Blockierung des Bootprozesses

## 4. Konsequenzen

### Vorteile
- Konsistente Benutzererfahrung
- Wiederverwendbare Architektur
- Hohe Testbarkeit
- Einheitliche Übergänge

### Nachteile
- Zusätzlicher Implementierungsaufwand
- Erhöhter Testumfang

## 5. Validierung

- Referenzanimationen
- BIOS- und UEFI-Tests
- Performance-Messungen
- Frame-Time-Analyse
- Regressionstests

## 6. Abhängigkeiten

- ADR-BOOTUI-0001 bis 0012
- ADR-BOOTGFX-0001 bis 0012
- NPSPEC-BOOTMOTION-0001 ff.

## 7. Revisionsverlauf

| Version | Datum | Änderung |
|---|---|---|
|1.0|2026-08-03|Erstfassung|
