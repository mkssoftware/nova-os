# ADR-BOOTINPUT-0002 – Tastatur als vollständig unterstützter Primärweg

| Feld | Wert |
|---|---|
| Dokument-ID | ADR-BOOTINPUT-0002 |
| Kategorie | NovaOS Bootmanager / Eingabe |
| Status | Angenommen |
| Version | 1.0 |
| Datum | 2026-08-03 |

## 1. Entscheidungskontext

Diese ADR definiert die Architekturentscheidung für **Tastatur als vollständig unterstützter Primärweg** innerhalb des Eingabesystems des NovaOS-Bootmanagers.

## 2. Entscheidung

Die Funktion wird als verpflichtender Bestandteil der Boot-Input-Architektur implementiert und stellt auf BIOS/VBE sowie UEFI/GOP ein einheitliches Eingabeverhalten bereit.

## 3. Technische Ausgestaltung

- Einheitliches Eventmodell
- Integration in Scene Graph und Fokusverwaltung
- Plattformunabhängige Abstraktion
- Deterministische Verarbeitung
- Unterstützung für Animationen und Dialoge
- Definierte Fehler- und Fallbackpfade

## 4. Konsequenzen

### Vorteile
- Konsistente Bedienung
- Wiederverwendbare Architektur
- Gute Testbarkeit
- Einfache Erweiterbarkeit

### Nachteile
- Zusätzlicher Implementierungsaufwand
- Größere Testmatrix

## 5. Validierung

- BIOS-/UEFI-Kompatibilität
- Tastatur-, Maus- und Touchtests
- Fokus- und Navigationstests
- Performance-Tests
- Regressionstests

## 6. Abhängigkeiten

- ADR-BOOTUI-0001 bis 0012
- ADR-BOOTCONTROL-0001 bis 0007
- ADR-BOOTNAV-0001 bis 0004
- NPSPEC-BOOTINPUT-0001 ff.

## 7. Revisionsverlauf

| Version | Datum | Änderung |
|---|---|---|
|1.0|2026-08-03|Erstfassung|
