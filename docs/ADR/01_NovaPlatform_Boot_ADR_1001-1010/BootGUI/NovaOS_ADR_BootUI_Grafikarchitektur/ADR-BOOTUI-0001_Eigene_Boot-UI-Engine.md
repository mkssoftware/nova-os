# ADR-BOOTUI-0001 – Eigene Boot-UI-Engine

| Feld | Wert |
|---|---|
| Dokument-ID | ADR-BOOTUI-0001 |
| Titel | Eigene Boot-UI-Engine |
| Kategorie | NovaOS Bootmanager / Grafikarchitektur |
| Status | Angenommen |
| Version | 1.0 |
| Datum | 2026-08-03 |
| Verantwortlich | NovaOS Architecture |
| Gültigkeitsbereich | BIOS- und UEFI-Bootmanager |

## 1. Entscheidungskontext

Der NovaOS-Bootmanager soll bereits vor dem Start des Kernels eine hochwertige, animierte und visuell konsistente Benutzeroberfläche bereitstellen. Standardlösungen wie textbasierte Bootmenüs, einfache Firmware-Widgets oder externe GUI-Frameworks erfüllen die Anforderungen an Glass-Materialien, Animationen, Dialog-Overlays, einheitliche Bedienelemente und kontrollierte Fallback-Pfade nicht.

Die Ausführungsumgebung ist stark eingeschränkt. Es stehen weder das vollständige Betriebssystem noch dessen Grafiktreiber, Speicherverwaltung, Threading-Infrastruktur oder Laufzeitbibliotheken zur Verfügung. Gleichzeitig muss die Oberfläche sowohl auf BIOS/VBE- als auch auf UEFI/GOP-Systemen funktionieren und darf den eigentlichen Bootvorgang nicht gefährden.

## 2. Entscheidung

NovaOS erhält eine eigene, freestanding implementierte Boot-UI-Engine. Sie bildet eine klar begrenzte Teilmenge der späteren Nova UI ab und wird speziell für die Bedingungen des frühen Bootprozesses entwickelt.

Die Engine umfasst mindestens:

- eine abstrahierte Framebuffer-Ausgabe,
- primitive 2D-Zeichenoperationen,
- Offscreen-Surfaces,
- Alpha-Compositing,
- abgerundete Rechtecke,
- Gradienten,
- Schatten und vereinfachte Unschärfeeffekte,
- Text- und Icon-Rendering,
- ein Retained-Mode-Elementmodell,
- Layout, Fokus und Eingabe,
- zeitbasierte Animationen,
- Overlay- und Dialogebenen,
- ein Design-Token-System,
- definierte Qualitäts- und Fallback-Profile.

Die Boot-UI-Engine bleibt technisch unabhängig vom Kernel und vom vollständigen NovaOS-Desktop. Gemeinsame Konzepte und Datenformate dürfen übernommen werden, jedoch keine Laufzeitabhängigkeiten.

## 3. Technische Ausgestaltung

### Architekturgrenzen

Die Engine wird in folgende Module getrennt:

1. **Platform Backend** für BIOS/VBE, UEFI/GOP und Testumgebungen.
2. **Graphics Core** für Pixeloperationen, Flächen, Bilder, Text und Compositing.
3. **Scene und Control Layer** für UI-Elemente, Layout und Zustände.
4. **Input Layer** für Tastatur, Maus und optional Touch.
5. **Motion Layer** für Zeitachsen, Übergänge und Easing.
6. **Resource Layer** für Fonts, Icons, Bilder, Themes und Lokalisierung.
7. **Application Layer** für Bootauswahl, Recovery, Diagnose und Dialoge.

### Nichtziele

- keine vollständige Desktop-Compositor-Implementierung,
- keine beliebige Drittanbieter-App-Ausführung,
- kein dynamisches Plugin-System,
- keine Abhängigkeit von POSIX, libc, GPU-Treibern oder Kernel-Diensten,
- keine vollständige Unicode- oder Web-Layout-Engine.

### Implementierungsvorgaben

- freestanding C oder eine gleichwertig kontrollierbare Systemsprache,
- keine versteckten Speicherallokationen,
- reproduzierbarer Ressourcenverbrauch,
- statisch gebundene Kernressourcen,
- klar definierter Textmodus-Fallback.

## 4. Konsequenzen

### Positive Folgen

- vollständige Kontrolle über Darstellung, Verhalten und Fehlerbehandlung,
- visuelle Kontinuität zwischen Bootmanager und NovaOS,
- keine Abhängigkeit von fremden Firmware-UI-Komponenten,
- gezielte Optimierung auf kleine Binärgröße und deterministisches Verhalten,
- hochwertige Dialoge und Animationen bereits vor dem Kernelstart.

### Negative Folgen

- hoher Entwicklungs- und Testaufwand,
- eigene Implementierung vieler Grafik- und UI-Grundlagen,
- langfristige Wartung mehrerer Firmware- und Hardwarepfade,
- Risiko einer zu großen Bootmanager-Komplexität.

### Risikobegrenzung

Die Engine wird strikt modularisiert. Jede visuelle Funktion muss deaktivierbar sein. Ein minimalistischer, nicht animierter Softwarepfad bleibt dauerhaft erhalten.

## 5. Verworfene Alternativen

- **Textbasiertes Bootmenü:** zu geringe visuelle Qualität und keine Designkontinuität.
- **Direkte Nutzung von Firmware-Widgets:** uneinheitlich und funktional zu eingeschränkt.
- **Portierung eines Desktop-GUI-Frameworks:** zu groß, zu abhängig und für den frühen Boot ungeeignet.
- **Einfaches Immediate-Mode-Menü:** für komplexe Dialoge, Animationen und Zustandsverwaltung langfristig zu unstrukturiert.

## 6. Validierung und Abnahmekriterien

- Start auf BIOS/VBE und UEFI/GOP.
- Vollständige Bedienbarkeit ohne Hardwarebeschleunigung.
- Definierte Speicherobergrenze.
- Fehler in der UI dürfen den Bootvorgang nicht blockieren.
- Referenztests für Rendering, Layout, Dialoge und Animationen.

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
