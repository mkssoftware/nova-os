# ADR-1007: Eingeschränktes Boot-Theme

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Boot Architecture
- **Erstellt:** 2026-07-11
- **Gültig für:** Nova Boot Manager
- **Ersetzt:** Keine

---

## 1. Kontext

Das Bootmenü soll dem Nova-Design entsprechen, aber nicht die vollständige NovaOS-Theme-Engine benötigen.

## 2. Problemstellung

Eine vollständige Theme-Engine mit dynamischen Widgets und komplexen Effekten ist für den Bootpfad zu groß.

## 3. Betrachtete Alternativen

- Fest kodiertes Design
- Vollständige NUI-Theme-Engine
- Kompaktes Boot-Theme mit Design Tokens

## 4. Entscheidung

Der Boot Manager verwendet ein kompaktes Boot-Theme mit festen Tokenklassen für Farben, Fonts, Abstände, Radien, Hintergrund und ausgewählte Zustände.

## 5. Begründung

- Designänderungen ohne Quellcode
- Kleiner Parser
- Konsistenz mit Nova Rescue und NovaOS

## 6. Positive Konsequenzen

- Austauschbares Design
- Überschaubarer Bootcode
- High-Contrast-Variante möglich

## 7. Negative Konsequenzen und Risiken

- Nicht alle späteren NUI-Effekte verfügbar
- Themeformat muss separat versioniert werden

## 8. Auswirkungen auf andere Module

Betrifft Boot Asset Pack, Bootmenü, NDS und NAS.

## 9. Umsetzungsregeln

- Die Entscheidung ist für neue Boot-Implementierungen verbindlich.
- Eingaben aus Firmware, Konfiguration und Assets müssen vor Verwendung validiert werden.
- Der Bootpfad muss einen sicheren Fehler- und Fallbackzustand besitzen.
- Änderungen an öffentlichen Strukturen benötigen eine neue ABI- oder Formatversion.
- BIOS- und UEFI-spezifischer Code bleibt hinter gemeinsamen Boot-Core-Schnittstellen gekapselt.

## 10. Referenzen

- ADR-0012
- ADR-1006

## 11. Statushistorie

- 2026-07-11: Version 1.0 angenommen.
