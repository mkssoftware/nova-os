# ADR-3006: Audio-Subsystem

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Hardware Abstraction Layer
- **Erstellt:** 2026-07-11

## 1. Kontext

Dieses ADR beschreibt die Architekturentscheidung für **Audio-Subsystem** innerhalb der HAL. Ziel ist eine hardwareunabhängige API zwischen Kernel und Gerätetreibern.

## 2. Problemstellung

Direkte Hardwarezugriffe aus Kernelmodulen führen zu starker Kopplung und erschweren Portierungen auf neue Architekturen.

## 3. Entscheidung

Die HAL kapselt hardwarespezifische Details hinter einer stabilen API. Treiber implementieren HAL-Schnittstellen; Kernel und höhere Schichten greifen ausschließlich auf diese Abstraktionen zu.

## 4. Begründung

- Trennung von Kernel und Hardwaredetails
- Einfachere Portierung
- Austauschbare Treiber
- Bessere Testbarkeit

## 5. Konsequenzen

### Positiv
- Saubere Architektur
- Wiederverwendbare Treiberstruktur
- Klare Verantwortlichkeiten

### Risiken
- Zusätzliche Abstraktionsschicht
- API muss langfristig stabil gehalten werden

## 6. Auswirkungen

Alle Treiber und Kernelmodule, die Audio-Subsystem verwenden, orientieren sich an dieser HAL-Schnittstelle.

## 7. Referenzen

- RFC-0001
- ADR-0015
- ADR-0019

## 8. Statushistorie

- 2026-07-11: Version 1.0 angenommen.
