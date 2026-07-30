# ADR-1002: BIOS/MBR als erste Referenzimplementierung

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Boot Architecture
- **Erstellt:** 2026-07-11
- **Gültig für:** Nova Boot Manager
- **Ersetzt:** Keine

---

## 1. Kontext

Die erste Nova-Version benötigt einen schnell testbaren Bootpfad in QEMU und auf älterer Hardware.

## 2. Problemstellung

BIOS und UEFI parallel vollständig zu implementieren würde die frühe Entwicklung verlangsamen.

## 3. Betrachtete Alternativen

- Nur UEFI
- BIOS und UEFI gleichzeitig
- BIOS zuerst, UEFI danach
- GRUB als Übergangslösung

## 4. Entscheidung

Die erste Referenzimplementierung startet über BIOS/MBR. UEFI wird anschließend als eigener Firmware-Frontend-Pfad ergänzt.

## 5. Begründung

- Einfacher Einstieg mit NASM und INT 13h/INT 10h
- Sehr gut in QEMU testbar
- Passt zu älterer Zielhardware
- Boot Core und BootInfo können später wiederverwendet werden

## 6. Positive Konsequenzen

- Schnelle frühe Ergebnisse
- Unterstützung älterer Systeme
- Klare Lern- und Testbasis

## 7. Negative Konsequenzen und Risiken

- BIOS ist technisch veraltet
- CHS/LBA- und VBE-Sonderfälle müssen behandelt werden

## 8. Auswirkungen auf andere Module

Betrifft Stage 1, Stage 2, Disk Loader und VBE.

## 9. Umsetzungsregeln

- Die Entscheidung ist für neue Boot-Implementierungen verbindlich.
- Eingaben aus Firmware, Konfiguration und Assets müssen vor Verwendung validiert werden.
- Der Bootpfad muss einen sicheren Fehler- und Fallbackzustand besitzen.
- Änderungen an öffentlichen Strukturen benötigen eine neue ABI- oder Formatversion.
- BIOS- und UEFI-spezifischer Code bleibt hinter gemeinsamen Boot-Core-Schnittstellen gekapselt.

## 10. Referenzen

- ADR-1001

## 11. Statushistorie

- 2026-07-11: Version 1.0 angenommen.
