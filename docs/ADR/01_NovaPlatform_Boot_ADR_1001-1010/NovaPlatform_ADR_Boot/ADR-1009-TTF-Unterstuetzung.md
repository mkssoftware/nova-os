# ADR-1009: TTF wird buildseitig in Bitmap-Fonts konvertiert

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Boot Architecture
- **Erstellt:** 2026-07-11
- **Gültig für:** Nova Boot Manager
- **Ersetzt:** Keine

---

## 1. Kontext

Nova benötigt mindestens zwei Schriftarten beziehungsweise Schriftschnitte im Bootmenü.

## 2. Problemstellung

TrueType-Parsing und Rasterisierung sind für den Bootpfad zu komplex und würden FreeType-ähnliche Funktionalität erfordern.

## 3. Betrachtete Alternativen

- TTF direkt rendern
- Ein fest eingebauter Bitmap-Font
- Buildzeit-Konvertierung zu NFONT
- Text vollständig als Bild rendern

## 4. Entscheidung

TTF/OTF wird auf dem Host in NFONT konvertiert. Der Boot Manager unterstützt mehrere NFONT-Ressourcen, mindestens Title und UI.

## 5. Begründung

- Zwei oder mehr Fonts ohne TTF-Runtime
- Kerning und Glyphen können vorverarbeitet werden
- Kleine Laufzeitroutinen
- Unicode-Subset kann gezielt eingebettet werden

## 6. Positive Konsequenzen

- Flexible Typografie
- Kompakter Renderer
- Vorhersagbare Speicherbelegung

## 7. Negative Konsequenzen und Risiken

- Nur eingebettete Glyphen verfügbar
- Fontgröße oder Zeichensatzänderung erfordert Neubuild

## 8. Auswirkungen auf andere Module

Betrifft Font Builder, NAS, Boot Asset Pack und Text Renderer.

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
