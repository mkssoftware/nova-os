# ADR-1008: PNG nur in der Host-Asset-Pipeline

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Boot Architecture
- **Erstellt:** 2026-07-11
- **Gültig für:** Nova Boot Manager
- **Ersetzt:** Keine

---

## 1. Kontext

Der Hintergrund und weitere Quellbilder liegen bequem als PNG vor.

## 2. Problemstellung

Ein vollständiger PNG-Decoder im frühen Bootpfad benötigt DEFLATE, Filter und umfangreiche Validierung.

## 3. Betrachtete Alternativen

- PNG direkt im Bootloader dekodieren
- Unkomprimierte BMP-Dateien
- Buildzeit-Konvertierung zu NIMG
- Bilder als C-Arrays

## 4. Entscheidung

Der Boot Manager dekodiert kein PNG. PNG wird durch den Asset Builder in ein validiertes NIMG- oder Boot-NIMG-Format konvertiert.

## 5. Begründung

- Kleinerer Bootloader
- Weniger Angriffsfläche
- Deterministische Ladezeit
- Pixelkonvertierung erfolgt bereits beim Build

## 6. Positive Konsequenzen

- Robuster Start
- Einfacher Renderer
- Beliebige PNG-Quellen weiterhin nutzbar

## 7. Negative Konsequenzen und Risiken

- Assetänderung erfordert erneute Konvertierung
- Zusätzliches Host-Tool erforderlich

## 8. Auswirkungen auf andere Module

Betrifft Asset Builder, NAS, Boot Asset Pack und NGE.

## 9. Umsetzungsregeln

- Die Entscheidung ist für neue Boot-Implementierungen verbindlich.
- Eingaben aus Firmware, Konfiguration und Assets müssen vor Verwendung validiert werden.
- Der Bootpfad muss einen sicheren Fehler- und Fallbackzustand besitzen.
- Änderungen an öffentlichen Strukturen benötigen eine neue ABI- oder Formatversion.
- BIOS- und UEFI-spezifischer Code bleibt hinter gemeinsamen Boot-Core-Schnittstellen gekapselt.

## 10. Referenzen

- ADR-0011
- ADR-1006

## 11. Statushistorie

- 2026-07-11: Version 1.0 angenommen.
