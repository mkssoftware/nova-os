# ADR-1003: UEFI als separates Firmware-Frontend

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Boot Architecture
- **Erstellt:** 2026-07-11
- **Gültig für:** Nova Boot Manager
- **Ersetzt:** Keine

---

## 1. Kontext

Moderne Systeme verwenden UEFI und GOP statt BIOS und VBE.

## 2. Problemstellung

Ein UEFI-Pfad darf nicht dazu führen, dass Boot Core, Konfiguration und Kernel Loader doppelt implementiert werden.

## 3. Betrachtete Alternativen

- Vollständig getrennte BIOS- und UEFI-Bootloader
- Nur UEFI
- Gemeinsamer Boot Core mit Firmware-Adaptern

## 4. Entscheidung

UEFI wird als separates Firmware-Frontend implementiert. BIOS- und UEFI-Frontend normalisieren Firmwaredaten in gemeinsame Nova-Strukturen und übergeben an denselben Boot Core.

## 5. Begründung

- Weniger doppelter Code
- Gemeinsame BootInfo ABI
- GOP und VBE werden hinter einer gemeinsamen Grafikbeschreibung abstrahiert

## 6. Positive Konsequenzen

- Einheitliche Kernelübergabe
- Leichter zu warten
- Firmwareunabhängige Bootlogik

## 7. Negative Konsequenzen und Risiken

- Abstraktionsschicht erforderlich
- UEFI-Speicher- und ExitBootServices-Regeln sind komplex

## 8. Auswirkungen auf andere Module

Betrifft Boot Core, Grafikabstraktion, Speicherkarte und Kernel Loader.

## 9. Umsetzungsregeln

- Die Entscheidung ist für neue Boot-Implementierungen verbindlich.
- Eingaben aus Firmware, Konfiguration und Assets müssen vor Verwendung validiert werden.
- Der Bootpfad muss einen sicheren Fehler- und Fallbackzustand besitzen.
- Änderungen an öffentlichen Strukturen benötigen eine neue ABI- oder Formatversion.
- BIOS- und UEFI-spezifischer Code bleibt hinter gemeinsamen Boot-Core-Schnittstellen gekapselt.

## 10. Referenzen

- ADR-1001
- ADR-1004

## 11. Statushistorie

- 2026-07-11: Version 1.0 angenommen.
