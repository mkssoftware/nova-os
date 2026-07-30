# ADR-1005: Framebuffer-Initialisierung im Bootloader

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Boot Architecture
- **Erstellt:** 2026-07-11
- **Gültig für:** Nova Boot Manager
- **Ersetzt:** Keine

---

## 1. Kontext

Nova Boot Manager und früher Kernel sollen eine grafische Oberfläche darstellen können.

## 2. Problemstellung

Der Kernel sollte nicht direkt von BIOS- oder UEFI-Firmwareaufrufen abhängen.

## 3. Betrachtete Alternativen

- Textmodus bis zum Grafiktreiber
- Kernel initialisiert VBE/GOP
- Bootloader initialisiert einen linearen Framebuffer
- Nur serielle Ausgabe

## 4. Entscheidung

Das Firmware-Frontend wählt einen geeigneten linearen Framebuffer-Modus und übergibt dessen Beschreibung über BootInfo. Der Kernel behandelt ihn als einfachen Boot-Framebuffer.

## 5. Begründung

- Firmwaredienste sind vor Kernelstart verfügbar
- Kernel bleibt firmwareunabhängig
- Frühe Panic- und UI-Ausgabe möglich

## 6. Positive Konsequenzen

- Frühe Grafik
- Gemeinsame VBE/GOP-Abstraktion
- Einfacher Kernelstart

## 7. Negative Konsequenzen und Risiken

- Moduswahl muss robust sein
- Framebufferformat und Pitch müssen exakt übergeben werden

## 8. Auswirkungen auf andere Module

Betrifft VBE, GOP, BootInfo, NGE-Framebuffer-Backend und Panic-Ausgabe.

## 9. Umsetzungsregeln

- Die Entscheidung ist für neue Boot-Implementierungen verbindlich.
- Eingaben aus Firmware, Konfiguration und Assets müssen vor Verwendung validiert werden.
- Der Bootpfad muss einen sicheren Fehler- und Fallbackzustand besitzen.
- Änderungen an öffentlichen Strukturen benötigen eine neue ABI- oder Formatversion.
- BIOS- und UEFI-spezifischer Code bleibt hinter gemeinsamen Boot-Core-Schnittstellen gekapselt.

## 10. Referenzen

- ADR-1003
- ADR-1004

## 11. Statushistorie

- 2026-07-11: Version 1.0 angenommen.
