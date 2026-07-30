# ADR-0011: Vorverarbeitete Nova Assets

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Foundation Architecture
- **Erstellt:** 2026-07-11
- **Gültig für:** Nova Platform
- **Ersetzt:** Keine

---

## 1. Kontext

PNG, TTF, SVG und Themes sollen in Bootmanager, Rescue und NovaOS verwendet werden.

## 2. Problemstellung

Direktes Parsen komplexer Quellformate in frühen Bootphasen erhöht Codegröße und Fehlerrisiko.

## 3. Betrachtete Alternativen

- Quellformate direkt laden
- Assets als C-Arrays einbetten
- Buildzeit-Konvertierung in Nova-Formate
- Nur rohe Bitmaps verwenden

## 4. Entscheidung

Host-Werkzeuge konvertieren Quellassets in versionierte Nova-Formate wie NIMG, NFONT, NICON und NPAK.

## 5. Begründung

- Kleine Laufzeitdecoder
- Schneller Start
- Einheitliche Formate
- Validierung kann im Build erfolgen

## 6. Positive Konsequenzen

- Weniger Kernelkomplexität
- Schnellere Ladezeiten
- Gemeinsame Pipeline für alle Produkte

## 7. Negative Konsequenzen und Risiken

- Buildschritt erforderlich
- Quellassets können nicht ohne Neubuild geändert werden

## 8. Auswirkungen auf andere Module

Betrifft NAS, NGE, Boot Manager, UI und Buildsystem.

## 9. Umsetzungsregeln

- Die Entscheidung ist für neue Implementierungen verbindlich.
- Abweichungen benötigen ein neues ADR oder eine dokumentierte Ersetzung dieses ADRs.
- Öffentliche APIs und persistente Formate müssen die festgelegten ABI- und Versionsregeln beachten.
- Tests und Dokumentation sind zusammen mit der Implementierung zu aktualisieren.

## 10. Referenzen

- RFC-0001

## 11. Statushistorie

- 2026-07-11: Version 1.0 angenommen.
