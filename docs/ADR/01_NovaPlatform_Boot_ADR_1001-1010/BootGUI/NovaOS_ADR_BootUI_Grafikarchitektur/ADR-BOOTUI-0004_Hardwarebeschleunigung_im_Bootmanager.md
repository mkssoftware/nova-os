# ADR-BOOTUI-0004 – Hardwarebeschleunigung im Bootmanager

| Feld | Wert |
|---|---|
| Dokument-ID | ADR-BOOTUI-0004 |
| Titel | Hardwarebeschleunigung im Bootmanager |
| Kategorie | NovaOS Bootmanager / Grafikarchitektur |
| Status | Vorgeschlagen |
| Version | 1.0 |
| Datum | 2026-08-03 |
| Verantwortlich | NovaOS Architecture |
| Gültigkeitsbereich | BIOS- und UEFI-Bootmanager |

## 1. Entscheidungskontext

Glass-Effekte, große Flächen, Animationen und hohe Auflösungen können den Software-Renderer stark belasten. Gleichzeitig ist im frühen Bootprozess nicht garantiert, dass eine portable und sichere Beschleunigungsschnittstelle verfügbar ist.

## 2. Entscheidung

Hardwarebeschleunigung wird ausschließlich als optionale Backend-Erweiterung vorgesehen. Sie darf nur verwendet werden, wenn sie ohne vollständigen Gerätetreiber zuverlässig initialisiert werden kann und sämtliche Sicherheits- und Fallback-Anforderungen erfüllt.

Beschleunigte Operationen können umfassen:

- Blits und Flächenkopien,
- Flächenfüllung,
- Alpha-Compositing,
- Skalierung,
- einfache Filter,
- Present beziehungsweise Buffer-Swap.

Die UI- und Szenenlogik bleibt vollständig unabhängig vom beschleunigten Backend.

## 3. Technische Ausgestaltung

### Zulässige Quellen

- standardisierte Firmware- oder Plattformmechanismen,
- einfache, dokumentierte Framebuffer-Beschleuniger,
- später optional ein Nova-spezifisches Preboot-Grafikprotokoll.

Herstellerspezifische GPU-Treiber im Bootmanager sind nicht Bestandteil der Baseline.

### Auswahl

Das Backend wird über Capability-Erkennung ausgewählt. Fehlende oder widersprüchliche Eigenschaften führen zur Deaktivierung. Eine Beschleunigung wird nur aktiviert, wenn ein Selbsttest erfolgreich abgeschlossen wurde.

### Sicherheitsgrenzen

- keine unvalidierten DMA-Bereiche,
- keine Ausführung externer Shader,
- keine dynamischen Binärtreiber,
- feste Zeitlimits für Initialisierung und Renderoperationen,
- sofortiger Rückfall auf Software-Rendering.

## 4. Konsequenzen

### Positive Folgen

- höhere Bildrate bei großen Auflösungen,
- aufwendigere Effekte möglich,
- geringerer CPU- und Speicherbandbreitenbedarf.

### Negative Folgen

- zusätzliche Komplexität,
- schwer testbare Hardwarevarianten,
- mögliche Firmware- und Plattformfehler,
- begrenzter praktischer Nutzen auf vielen Systemen.

## 5. Verworfene Alternativen

- **Keine Hardwarebeschleunigung:** maximal robust, aber eingeschränkte Effekte.
- **Vollständige GPU-Treiber im Bootmanager:** zu groß und zu riskant.
- **Herstellerspezifische Pfade:** hoher Wartungsaufwand und geringe Portabilität.

## 6. Validierung und Abnahmekriterien

- Backend-Selbsttest vor Aktivierung,
- Vergleich gegen Software-Referenz,
- erzwungene Timeout- und Fehlerfälle,
- Test auf mehreren Firmware- und Geräteklassen,
- Nachweis, dass jederzeit auf Software zurückgeschaltet werden kann.

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
