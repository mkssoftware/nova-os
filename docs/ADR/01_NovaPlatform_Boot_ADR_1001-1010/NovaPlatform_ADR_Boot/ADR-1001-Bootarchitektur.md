# ADR-1001: Mehrstufige Nova-Bootarchitektur

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Boot Architecture
- **Erstellt:** 2026-07-11
- **Gültig für:** Nova Boot Manager
- **Ersetzt:** Keine

---

## 1. Kontext

Nova Rescue und NovaOS sollen dieselbe Boot-Infrastruktur verwenden. Der Bootpfad muss klein beginnen, aber später BIOS, UEFI, Konfiguration, Grafik und mehrere Kernelziele unterstützen.

## 2. Problemstellung

Ein monolithischer Bootloader wäre schwer wartbar und müsste viele Aufgaben in einer einzigen Binärdatei lösen.

## 3. Betrachtete Alternativen

- Monolithischer Bootloader
- Stage 1 und direkt Kernel
- Mehrstufige Architektur mit gemeinsamem Boot Core
- Externer Standardbootloader

## 4. Entscheidung

Nova verwendet eine mehrstufige Bootarchitektur: Firmware → Stage 1 → Stage 2 / Boot Core → Nova Boot Manager → Kernel Loader → Nova Kernel.

## 5. Begründung

- Stage 1 bleibt minimal und robust
- Komplexe Funktionen liegen außerhalb der 512-Byte-Grenze
- BIOS und UEFI können später denselben Boot Core nutzen
- Nova Rescue und NovaOS teilen dieselben Loader- und BootInfo-Schnittstellen

## 6. Positive Konsequenzen

- Klare Verantwortlichkeiten
- Leichtere Tests
- Gemeinsame Plattformbasis
- Einfachere Erweiterung

## 7. Negative Konsequenzen und Risiken

- Mehr Übergabepunkte
- BootInfo und Speicherlayout müssen exakt definiert sein

## 8. Auswirkungen auf andere Module

Betrifft Stage 1, Stage 2, Boot Core, Kernel Loader und BootInfo ABI.

## 9. Umsetzungsregeln

- Die Entscheidung ist für neue Boot-Implementierungen verbindlich.
- Eingaben aus Firmware, Konfiguration und Assets müssen vor Verwendung validiert werden.
- Der Bootpfad muss einen sicheren Fehler- und Fallbackzustand besitzen.
- Änderungen an öffentlichen Strukturen benötigen eine neue ABI- oder Formatversion.
- BIOS- und UEFI-spezifischer Code bleibt hinter gemeinsamen Boot-Core-Schnittstellen gekapselt.

## 10. Referenzen

- RFC-0001
- ADR-0015

## 11. Statushistorie

- 2026-07-11: Version 1.0 angenommen.
