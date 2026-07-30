# ADR-1010: Datengetriebenes Bootmenü mit sicherer Eingabe

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Boot Architecture
- **Erstellt:** 2026-07-11
- **Gültig für:** Nova Boot Manager
- **Ersetzt:** Keine

---

## 1. Kontext

Das Bootmenü soll Nova Rescue, NovaOS und Diagnoseoptionen anzeigen und per Tastatur sowie später Maus bedienbar sein.

## 2. Problemstellung

Fest kodierte Menüeinträge und direkte Startaktionen erschweren Konfiguration und sichere Bestätigung gefährlicher Optionen.

## 3. Betrachtete Alternativen

- Fest codiertes Menü
- Konfigurationsgesteuertes Menü
- Vollständige Desktop-UI im Bootloader
- Nur Textmenü

## 4. Entscheidung

Das Bootmenü wird aus einer validierten Boot-Konfiguration aufgebaut. Es verwendet ein begrenztes Widgetset, Fokusnavigation, Tastatursteuerung und optionale Mausunterstützung.

## 5. Begründung

- Einträge können ohne Codeänderung angepasst werden
- Gleiche UI-Logik für BIOS und UEFI
- Gefährliche Aktionen können gesondert bestätigt werden
- Fallback auf Tastatur bleibt möglich

## 6. Positive Konsequenzen

- Flexible Konfiguration
- Barriereärmer
- Robuste Bedienung
- Klare Trennung von Darstellung und Bootzielen

## 7. Negative Konsequenzen und Risiken

- Konfigurationsparser und Zustandsautomat erforderlich
- Mausunterstützung erhöht Treiberumfang

## 8. Auswirkungen auf andere Module

Betrifft Boot Config, Input, Theme, Asset Loader und Kernel Loader.

## 9. Umsetzungsregeln

- Die Entscheidung ist für neue Boot-Implementierungen verbindlich.
- Eingaben aus Firmware, Konfiguration und Assets müssen vor Verwendung validiert werden.
- Der Bootpfad muss einen sicheren Fehler- und Fallbackzustand besitzen.
- Änderungen an öffentlichen Strukturen benötigen eine neue ABI- oder Formatversion.
- BIOS- und UEFI-spezifischer Code bleibt hinter gemeinsamen Boot-Core-Schnittstellen gekapselt.

## 10. Referenzen

- ADR-1006
- ADR-1007
- ADR-0022

## 11. Statushistorie

- 2026-07-11: Version 1.0 angenommen.
