# ADR-BOOTUI-0005 – Double- und Triple-Buffering

| Feld | Wert |
|---|---|
| Dokument-ID | ADR-BOOTUI-0005 |
| Titel | Double- und Triple-Buffering |
| Kategorie | NovaOS Bootmanager / Grafikarchitektur |
| Status | Angenommen |
| Version | 1.0 |
| Datum | 2026-08-03 |
| Verantwortlich | NovaOS Architecture |
| Gültigkeitsbereich | BIOS- und UEFI-Bootmanager |

## 1. Entscheidungskontext

Direktes Zeichnen in den sichtbaren Framebuffer erzeugt Flackern, Tearing und teilweise aufgebaute Frames. Hochwertige Animationen und transparente Dialoge benötigen eine konsistente Frame-Darstellung.

## 2. Entscheidung

Die Boot-UI verwendet mindestens Double-Buffering. Triple-Buffering wird als optionales Leistungsprofil unterstützt, wenn genügend Speicher verfügbar ist und das Ausgabe-Backend davon profitiert.

Standardablauf:

1. Szene in einen Backbuffer rendern.
2. Rendering vollständig abschließen.
3. Frame atomar oder in einem kontrollierten Kopiervorgang präsentieren.
4. Pufferrollen wechseln oder Backbuffer wiederverwenden.

## 3. Technische Ausgestaltung

### Double-Buffering

Double-Buffering ist der Standard für alle grafischen Modi. Bei einem reinen linearen Framebuffer besteht der Frontbuffer aus dem sichtbaren Firmware-Framebuffer und der Backbuffer aus reserviertem Arbeitsspeicher.

### Triple-Buffering

Triple-Buffering ist nur zulässig, wenn:

- das Speicherbudget eingehalten wird,
- ein asynchroner Present-Pfad existiert oder
- die Renderpipeline messbar davon profitiert.

### Speicherberechnung

Der benötigte Speicher wird vor Aktivierung berechnet:

`Breite × Höhe × BytesProPixel × Pufferanzahl`

Übersteigt der Bedarf das Profilbudget, wird automatisch auf weniger Puffer oder eine geringere Auflösung zurückgeschaltet.

### Präsentation

Wo kein echter Page-Flip möglich ist, wird der fertige Backbuffer in den sichtbaren Framebuffer kopiert. Dirty Regions dürfen die Kopiermenge reduzieren.

## 4. Konsequenzen

### Positive Folgen

- kein sichtbarer Zwischenzustand,
- deutlich weniger Flackern und Tearing,
- stabile Grundlage für Animationen und Compositing,
- klarer Zeitpunkt für Frame-Diagnosen.

### Negative Folgen

- zusätzlicher Speicherbedarf,
- vollständige Framekopien können teuer sein,
- Triple-Buffering erhöht Latenz und Komplexität.

## 5. Verworfene Alternativen

- **Direktes Frontbuffer-Rendering:** nicht hochwertig genug.
- **Nur Dirty-Region-Zeichnen ohne Backbuffer:** weiterhin anfällig für sichtbare Zwischenzustände.
- **Immer Triple-Buffering:** unnötig hoher Speicherverbrauch.

## 6. Validierung und Abnahmekriterien

- Flacker- und Tearing-Tests,
- Speicherbudgettests für mehrere Auflösungen,
- Messung von Render-, Copy- und Present-Zeit,
- korrekte Degradation bei zu wenig Speicher,
- Prüfung von Framekonsistenz während Dialoganimationen.

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
