# ADR-BOOTUI-0003 – Software-Rendering als garantierter Fallback

| Feld | Wert |
|---|---|
| Dokument-ID | ADR-BOOTUI-0003 |
| Titel | Software-Rendering als garantierter Fallback |
| Kategorie | NovaOS Bootmanager / Grafikarchitektur |
| Status | Angenommen |
| Version | 1.0 |
| Datum | 2026-08-03 |
| Verantwortlich | NovaOS Architecture |
| Gültigkeitsbereich | BIOS- und UEFI-Bootmanager |

## 1. Entscheidungskontext

Im frühen Bootprozess stehen normalerweise keine vollständigen GPU-Treiber zur Verfügung. Firmwarefunktionen können unvollständig, fehlerhaft oder auf bestimmten Geräten nicht nutzbar sein. Die grafische Oberfläche muss dennoch zuverlässig funktionieren.

## 2. Entscheidung

Jede verpflichtende UI-Funktion wird durch einen eigenen Software-Renderer unterstützt. Hardwarebeschleunigung ist optional und darf niemals Voraussetzung für die Anzeige oder Bedienung des Bootmanagers sein.

Der Softwarepfad unterstützt mindestens:

- Pixel- und Flächenfüllung,
- Linien und Rechtecke,
- abgerundete Geometrie,
- Alpha-Blending,
- Bilder,
- Text,
- Gradienten,
- Clipping,
- Offscreen-Surfaces,
- vereinfachte Schatten,
- degradierbare Glass-Effekte.

## 3. Technische Ausgestaltung

### Garantierter Funktionsumfang

Der Fallback muss alle sicherheits- und bootrelevanten Oberflächen vollständig darstellen können. Dazu gehören Bootauswahl, Recovery, Passworteingabe, Warnungen, Fortschritt und Fehlermeldungen.

### Qualitätsstufen

- **Full:** alle unterstützten Effekte.
- **Balanced:** reduzierte Blur-Radien, Schatten und Animationen.
- **Basic:** Transparenz ohne aufwendige Unschärfe.
- **Safe:** opake Flächen, einfache Übergänge, minimale Icons.
- **Text:** letzter Notfallpfad ohne grafische UI.

### Aktivierung

Der Softwarepfad ist Standard und Referenzimplementierung. Ein beschleunigter Pfad muss gegen ihn validiert werden. Bei Initialisierungsfehlern, Zeitüberschreitungen oder Renderfehlern erfolgt automatisch der Rückfall.

## 4. Konsequenzen

### Positive Folgen

- verlässliche Anzeige auf nahezu jeder unterstützten Hardware,
- reproduzierbares Referenzrendering,
- geringere Abhängigkeit von Firmwarequalität,
- einfachere Fehlerdiagnose.

### Negative Folgen

- höherer CPU-Verbrauch,
- aufwendige Effekte können langsam sein,
- eigene Optimierungen für Speicherbandbreite und Pixeloperationen sind erforderlich.

## 5. Verworfene Alternativen

- **Nur Firmware-Ausgabe:** zu wenig kontrollierbar.
- **Nur Hardwarebeschleunigung:** nicht überall verfügbar.
- **Verzicht auf hochwertige Effekte:** widerspricht der Produktvision.

## 6. Validierung und Abnahmekriterien

- Test ohne GPU- oder Firmwarebeschleunigung,
- Test mit simuliert langsamer CPU,
- definierte Mindestbildrate pro Qualitätsprofil,
- visuelle Referenztests,
- erzwungene Fallback-Umschaltung während der Initialisierung.

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
