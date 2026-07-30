# ADR-1004: Versionierte BootInfo-ABI

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Boot Architecture
- **Erstellt:** 2026-07-11
- **Gültig für:** Nova Boot Manager
- **Ersetzt:** Keine

---

## 1. Kontext

Der Bootloader muss Kernel und späteren Modulen Hardware- und Firmwareinformationen übergeben.

## 2. Problemstellung

Unversionierte Strukturen würden bei Erweiterungen zu Binärinkompatibilität führen.

## 3. Betrachtete Alternativen

- Globale feste Speicheradressen ohne Header
- Multiboot übernehmen
- Eigene versionierte BootInfo-Struktur
- Registerübergabe einzelner Werte

## 4. Entscheidung

Nova definiert eine eigene versionierte BootInfo-Struktur mit Magic, StructSize, AbiVersion, Flags und Zeigern auf versionierte Unterstrukturen.

## 5. Begründung

- Erweiterbar ohne alte Felder zu verschieben
- BIOS und UEFI können dieselbe ABI bedienen
- Kernel kann Eingaben validieren
- Keine Abhängigkeit von externer Boot-Spezifikation

## 6. Positive Konsequenzen

- Stabile Schnittstelle
- Gute Diagnose
- Modular erweiterbar

## 7. Negative Konsequenzen und Risiken

- Eigene Spezifikation muss gepflegt werden
- Alle Pointer und Speicherbereiche müssen validiert werden

## 8. Auswirkungen auf andere Module

Betrifft Bootloader, Kernel Entry, Memory Map, Framebuffer, ACPI und Module.

## 9. Umsetzungsregeln

- Die Entscheidung ist für neue Boot-Implementierungen verbindlich.
- Eingaben aus Firmware, Konfiguration und Assets müssen vor Verwendung validiert werden.
- Der Bootpfad muss einen sicheren Fehler- und Fallbackzustand besitzen.
- Änderungen an öffentlichen Strukturen benötigen eine neue ABI- oder Formatversion.
- BIOS- und UEFI-spezifischer Code bleibt hinter gemeinsamen Boot-Core-Schnittstellen gekapselt.

## 10. Referenzen

- ADR-0015
- ADR-1003

## 11. Statushistorie

- 2026-07-11: Version 1.0 angenommen.
