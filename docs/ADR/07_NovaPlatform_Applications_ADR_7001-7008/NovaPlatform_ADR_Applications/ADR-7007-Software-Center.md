# ADR-7007: Software Center

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Anwendungen
- **Erstellt:** 2026-07-11

## 1. Kontext

Dieses ADR beschreibt die Architekturentscheidung für **Software Center** als Bestandteil der Nova Platform.

## 2. Problemstellung

Die Anwendung muss sowohl unter NovaOS als auch – soweit sinnvoll – innerhalb von Nova Rescue auf denselben Foundation-, Kernel-, NVFS-, NGE- und NUI-Schnittstellen aufbauen.

## 3. Entscheidung

Die Anwendung wird als eigenständiges Modul entwickelt und verwendet ausschließlich öffentliche APIs. Direkte Zugriffe auf Kernel-Interna oder Dateisystemtreiber sind nicht zulässig.

## 4. Begründung

- Wiederverwendbarkeit zwischen NovaOS und Nova Rescue
- Klare Trennung zwischen Anwendung und Systemdiensten
- Bessere Testbarkeit
- Austauschbare Implementierungen

## 5. Konsequenzen

### Positiv

- Konsistente Benutzererfahrung
- Geringere Kopplung
- Langfristig stabile Architektur

### Risiken

- Neue Funktionen müssen zunächst über öffentliche APIs bereitgestellt werden.
- Zusätzliche Abstraktionsschichten können Entwicklungsaufwand erhöhen.

## 6. Auswirkungen

Dieses ADR betrifft alle Komponenten, die mit **Software Center** interagieren.

## 7. Referenzen

- RFC-0001
- ADR-0005
- ADR-0015
- ADR-0019
- ADR-6001

## 8. Statushistorie

- 2026-07-11: Version 1.0 angenommen.
