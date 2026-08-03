# ADR-BOOTUI-0007 – Dirty-Region-Rendering

| Feld | Wert |
|---|---|
| Dokument-ID | ADR-BOOTUI-0007 |
| Titel | Dirty-Region-Rendering |
| Kategorie | NovaOS Bootmanager / Grafikarchitektur |
| Status | Angenommen |
| Version | 1.0 |
| Datum | 2026-08-03 |
| Verantwortlich | NovaOS Architecture |
| Gültigkeitsbereich | BIOS- und UEFI-Bootmanager |

## 1. Entscheidungskontext

Vollständiges Neuzeichnen und Kopieren jedes Frames ist bei hohen Auflösungen und Software-Rendering teuer. Viele UI-Änderungen betreffen nur kleine Bereiche, etwa Fokusindikatoren, Countdown-Texte, Fortschrittsanzeigen oder Dialoganimationen.

## 2. Entscheidung

Die Boot-UI implementiert Damage Tracking und Dirty-Region-Rendering. Geänderte Szenenknoten markieren ihre bisherigen und neuen Bildschirmbereiche als beschädigt. Nur diese Bereiche werden neu gerendert und präsentiert, sofern das aktuelle Qualitätsprofil und der Effektaufbau dies erlauben.

## 3. Technische Ausgestaltung

### Dirty-Quellen

- Geometrieänderungen,
- Style- und Farbänderungen,
- Textänderungen,
- Sichtbarkeitsänderungen,
- Animationen,
- Fokuswechsel,
- Änderungen an Hintergrundeffekten,
- neu geladene Ressourcen.

### Regionenverwaltung

Beschädigte Rechtecke werden gesammelt, beschnitten und zusammengeführt. Überschreitet ihre Anzahl oder Gesamtfläche einen Schwellwert, wird auf vollständiges Frame-Rendering gewechselt.

### Effektausweitung

Schatten, Blur und Filter vergrößern die Dirty Region um ihren Effekt-Radius. Bewegte transparente Objekte beschädigen sowohl ihre alte als auch ihre neue Position.

### Kompatibilität

Bestimmte Fullscreen-Effekte dürfen Dirty Rendering deaktivieren. Die Entscheidung erfolgt pro Frame.

## 4. Konsequenzen

### Positive Folgen

- deutlich weniger Pixelarbeit,
- geringerer Energie- und Zeitbedarf,
- flüssigere Animation kleiner Elemente,
- reduzierte Frontbuffer-Kopien.

### Negative Folgen

- komplexe Invalidierungslogik,
- Fehler können visuelle Artefakte hinterlassen,
- Blur und große Transparenzflächen reduzieren den Nutzen.

## 5. Verworfene Alternativen

- **Immer vollständiges Rendering:** einfacher, aber langsamer.
- **Nur manuell definierte Updatebereiche:** fehleranfällig.
- **Tile-basiertes Rendering:** möglich, aber für die erste Version komplexer.

## 6. Validierung und Abnahmekriterien

- Vergleich von Dirty-Frames mit vollständigem Referenzrendering,
- Tests für Bewegung, Schatten, Blur und Transparenz,
- erzwungener Wechsel zwischen Teil- und Vollrendering,
- Messung von Regionenzahl und aktualisierter Pixelmenge.

## 7. Sicherheits- und Robustheitsanforderungen

- Ein Fehler innerhalb der grafischen Oberfläche darf den eigentlichen Bootvorgang nicht dauerhaft blockieren.
- Alle extern oder aus dem Dateisystem geladenen Daten müssen vor der Nutzung validiert werden.
- Speicherzugriffe müssen auf reservierte und überprüfte Bereiche begrenzt bleiben.
- Für jede optionale Funktion muss ein definierter Fallback existieren.
- Diagnoseinformationen dürfen keine geheimen Schlüssel, Passwörter oder geschützten Bootparameter offenlegen.

## 8. Abhängigkeiten

- ADR-BOOT-0001 – BIOS und UEFI parallel unterstützen
- ADR-BOOT-1012 – Eigenes Boot-Handoff-Protokoll
- NPSPEC-BOOTUI-0001 – Bootmanager UI Architecture
- NPSPEC-BOOTRENDER-0001 – 2D Rendering Engine
- NPSPEC-BOOTDESIGN-0001 – Boot Design Language
- NPSPEC-BOOTPERF-0001 – Boot UI Performance Requirements

## 9. Revisionsverlauf

| Version | Datum | Änderung |
|---|---|---|
| 1.0 | 2026-08-03 | Erstfassung |
