# ADR-BOOTUI-0008 – Auflösungsunabhängiges Koordinatensystem

| Feld | Wert |
|---|---|
| Dokument-ID | ADR-BOOTUI-0008 |
| Titel | Auflösungsunabhängiges Koordinatensystem |
| Kategorie | NovaOS Bootmanager / Grafikarchitektur |
| Status | Angenommen |
| Version | 1.0 |
| Datum | 2026-08-03 |
| Verantwortlich | NovaOS Architecture |
| Gültigkeitsbereich | BIOS- und UEFI-Bootmanager |

## 1. Entscheidungskontext

Bootmanager müssen auf sehr unterschiedlichen Auflösungen funktionieren. Pixelgenau festgelegte Layouts führen auf kleinen Displays zu Überläufen und auf großen Displays zu unpassend kleinen Bedienelementen.

## 2. Entscheidung

Die Boot-UI verwendet logische, auflösungsunabhängige Einheiten. Layout, Größen, Abstände, Radien und Bewegungsdistanzen werden in Device-Independent Units beschrieben und erst im Rendering in physische Pixel umgerechnet.

Die Referenzbasis beträgt 96 logische Einheiten pro Zoll, sofern keine verlässliche physische Pixeldichte bekannt ist.

## 3. Technische Ausgestaltung

### Umrechnung

`physische Pixel = logische Einheit × Skalierungsfaktor`

Der Skalierungsfaktor wird zentral berechnet und für Layout, Rendering, Hit-Testing und Eingabekoordinaten identisch verwendet.

### Viewport

Die UI arbeitet mit einem logischen Viewport. Seitenverhältnisse und sichere Ränder werden berücksichtigt. Das Layout darf definierte Mindest- und Maximalbreiten verwenden.

### Rundung

Positionen und Größen werden deterministisch gerundet. Textgrundlinien und scharfe Kanten dürfen optional auf Pixelgrenzen ausgerichtet werden.

### Ressourcen

Rasterbilder werden in mehreren Größen oder ausreichend hoher Auflösung bereitgestellt. Icons und geometrische Elemente sollen möglichst vektorbasiert sein.

## 4. Konsequenzen

### Positive Folgen

- konsistente Größenwirkung über viele Auflösungen,
- einfachere responsive Layouts,
- bessere Wiederverwendung der Design-Tokens,
- verlässliches Hit-Testing.

### Negative Folgen

- Rundungsfehler und unscharfe Kanten möglich,
- zusätzliche Umrechnungsschritte,
- physische DPI-Werte sind im Boot häufig unzuverlässig.

## 5. Verworfene Alternativen

- **Feste Pixelkoordinaten:** nur für eine kleine Zahl von Modi brauchbar.
- **Pro Auflösung eigene Layouts:** hoher Pflegeaufwand.
- **Nur prozentuale Koordinaten:** ungeeignet für Typografie und Mindestgrößen.

## 6. Validierung und Abnahmekriterien

- Layouttests von 640×480 bis 4K,
- verschiedene Seitenverhältnisse,
- Prüfung von Rundung und Pixelalignment,
- identisches Hit-Testing bei Skalierung,
- keine abgeschnittenen kritischen Controls.

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
