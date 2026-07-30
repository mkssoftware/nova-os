# ADR-1006: Minimaler Asset Loader im Boot Manager

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Boot Architecture
- **Erstellt:** 2026-07-11
- **Gültig für:** Nova Boot Manager
- **Ersetzt:** Keine

---

## 1. Kontext

Das Bootmenü benötigt Hintergrund, Icons und Fonts, soll aber keine vollständige Betriebssystem-Asset-Engine enthalten.

## 2. Problemstellung

Ein zu komplexer Asset Loader vergrößert den Bootloader und erhöht das Fehlerrisiko.

## 3. Betrachtete Alternativen

- Assets fest in Binärdatei einbetten
- PNG/TTF direkt laden
- Nur vorverarbeitete Nova-Assets
- Vollständiges NAS im Bootloader

## 4. Entscheidung

Der Boot Manager lädt ausschließlich eine kleine, fest definierte Teilmenge vorverarbeiteter Nova-Assets aus einem Boot Asset Pack.

## 5. Begründung

- Kleine Parser
- Schneller Start
- Keine komplexen PNG- oder TTF-Decoder notwendig
- Gemeinsame Build-Pipeline mit NovaOS

## 6. Positive Konsequenzen

- Kompakter Bootcode
- Einfach validierbar
- Design bleibt austauschbar

## 7. Negative Konsequenzen und Risiken

- Asset Pack muss vorab erzeugt werden
- Nur unterstützte Formate und Features verfügbar

## 8. Auswirkungen auf andere Module

Betrifft NAS, Boot Image Builder, Bootmenü und Theme Loader.

## 9. Umsetzungsregeln

- Die Entscheidung ist für neue Boot-Implementierungen verbindlich.
- Eingaben aus Firmware, Konfiguration und Assets müssen vor Verwendung validiert werden.
- Der Bootpfad muss einen sicheren Fehler- und Fallbackzustand besitzen.
- Änderungen an öffentlichen Strukturen benötigen eine neue ABI- oder Formatversion.
- BIOS- und UEFI-spezifischer Code bleibt hinter gemeinsamen Boot-Core-Schnittstellen gekapselt.

## 10. Referenzen

- ADR-0011

## 11. Statushistorie

- 2026-07-11: Version 1.0 angenommen.
