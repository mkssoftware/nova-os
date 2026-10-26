# NPSPEC-COMPOSITOR-RENDER-0006 – Nova Variable Refresh Rate

**Status:** Angenommen  
**Bereich:** Compositor / Variable Bildwiederholrate  
**System:** NovaOS

## 1. Zweck

Nova Variable Refresh Rate definiert die Unterstützung variabler Bildwiederholraten (VRR) innerhalb des Nova Compositors.

Ziel ist eine flüssige, tearingfreie und latenzarme Bildausgabe durch dynamische Anpassung des Display-Aktualisierungszeitpunkts an die Verfügbarkeit neuer Frames.

## 2. Architektur

- Integration in Nova Vertical Synchronization und Compositor Scheduling.
- Steuerung über Nova Display Server und Grafiktreiber.
- Unabhängige VRR-Verwaltung pro Display.
- Hardwareabhängige Aktivierung über definierte Schnittstellen.
- Unterstützung von Direct Scanout und regulärer Komposition.
- Automatischer Wechsel zwischen VRR und fester Bildwiederholrate.
- Berücksichtigung von Display-Frequenzgrenzen.
- Keine Abhängigkeit von bestimmten GPU-Herstellern.

## 3. Kernfunktionen

- Erkennung VRR-fähiger Displays.
- Ermittlung unterstützter Frequenzbereiche.
- Aktivierung und Deaktivierung von VRR.
- Dynamische Planung von Präsentationszeitpunkten.
- Anpassung an schwankende Frame-Zeiten.
- Unterstützung von Vollbild- und geeigneten Fensteranwendungen.
- Verwaltung unterschiedlicher VRR-Modi pro Display.
- Unterstützung von Low Framerate Compensation (LFC), sofern verfügbar.
- Vermeidung unnötiger Frame-Warteschlangen.
- Automatischer Fallback auf Fixed VSync.
- Bereitstellung von VRR-Status und Diagnoseinformationen.

## 4. Normative Anforderungen

- RENDER-001: VRR MUSS bei kompatibler Hardware unterstützt werden.
- RENDER-002: Die VRR-Fähigkeiten eines Displays MÜSSEN vor der Aktivierung geprüft werden.
- RENDER-003: Der unterstützte minimale und maximale Frequenzbereich MUSS berücksichtigt werden.
- RENDER-004: VRR DARF nicht außerhalb der gültigen Hardwaregrenzen betrieben werden.
- RENDER-005: VRR MUSS pro Display unabhängig aktiviert werden können.
- RENDER-006: Die Aktivierung MUSS mit Nova Display Mode Management abgestimmt werden.
- RENDER-007: Frame-Präsentationen MÜSSEN mit Nova Buffer Synchronization synchronisiert werden.
- RENDER-008: Unvollständig gerenderte Frames DÜRFEN nicht präsentiert werden.
- RENDER-009: Der Compositor SOLL aktuelle Frames mit möglichst geringer zusätzlicher Verzögerung präsentieren.
- RENDER-010: Unnötige Frame-Warteschlangen SOLLEN vermieden werden.
- RENDER-011: Direct Scanout SOLL VRR unterstützen, sofern die Hardware dies ermöglicht.
- RENDER-012: VRR DARF die korrekte Komposition mehrerer Fenster nicht beeinträchtigen.
- RENDER-013: Bei inkompatiblen Präsentationsbedingungen MUSS auf einen geeigneten festen Synchronisationsmodus gewechselt werden können.
- RENDER-014: LFC SOLL verwendet werden, sofern Hardware und Treiber dies unterstützen.
- RENDER-015: Statische Bildinhalte DÜRFEN keine unnötige kontinuierliche Neuberechnung erfordern.
- RENDER-016: Änderungen des Display-Modus MÜSSEN eine erneute Prüfung der VRR-Fähigkeiten auslösen.
- RENDER-017: VRR-Status und tatsächliche Präsentationszeiten MÜSSEN diagnostizierbar sein.
- RENDER-018: VRR MUSS ohne KI-Abhängigkeit deterministisch gesteuert werden können.

## 5. VRR-Betriebsmodi

### Disabled
VRR ist deaktiviert. Die Bildausgabe verwendet einen festen Synchronisationsmodus.

### Automatic
NovaOS aktiviert VRR automatisch, wenn Display, Anwendung und Präsentationsbedingungen geeignet sind.

### Enabled
VRR wird bevorzugt verwendet, solange die technischen Voraussetzungen erfüllt sind.

### Fallback
Bei fehlender Unterstützung oder ungeeigneten Bedingungen erfolgt die Rückkehr zu Fixed VSync.

Der Modus Enabled DARF keine inkompatiblen Hardwarezustände erzwingen.

## 6. Präsentationssteuerung

### Variable Frame Timing
Neue Frames werden innerhalb des unterstützten VRR-Bereichs möglichst zeitnah präsentiert.

### Minimum Refresh Interval
Die maximal zulässige Bildwiederholrate begrenzt den frühestmöglichen nächsten Aktualisierungszeitpunkt.

### Maximum Refresh Interval
Die minimale unterstützte Bildwiederholrate begrenzt den Abstand zwischen erforderlichen Display-Aktualisierungen.

### Low Framerate Compensation
Bei niedrigen Frameraten können Frames mehrfach ausgegeben werden, um innerhalb des unterstützten Frequenzbereichs zu bleiben.

### Frame Pacing
Unregelmäßige Frame-Abstände werden berücksichtigt, um sichtbares Ruckeln und unnötige Verzögerungen zu reduzieren.

## 7. Verarbeitungsablauf

1. **Detect:** VRR-Fähigkeiten des Displays ermitteln.
2. **Validate:** Frequenzbereich und Präsentationsbedingungen prüfen.
3. **Configure:** Geeigneten VRR-Modus aktivieren.
4. **Schedule:** Frame-Verarbeitung anhand aktueller Rendering-Zeiten planen.
5. **Render:** Neuen Frame erzeugen.
6. **Synchronize:** Buffer- und GPU-Operationen abschließen.
7. **Present:** Frame innerhalb des zulässigen VRR-Zeitfensters ausgeben.
8. **Measure:** Präsentationszeitpunkt und Frame-Abstände erfassen.
9. **Adjust:** Zeitplanung und Betriebsmodus bei Bedarf anpassen.

## 8. Performance und Zuverlässigkeit

- Minimierung der Eingabe-zu-Anzeige-Latenz.
- Vermeidung von Screen Tearing.
- Reduzierung sichtbaren Ruckelns bei schwankenden Frameraten.
- Begrenzung ausstehender Frames.
- Vermeidung unnötiger GPU-Wartezeiten.
- Unabhängige Steuerung mehrerer Displays.
- Energieeffiziente Verarbeitung statischer Inhalte.
- Kontrollierter Wechsel zwischen VRR und Fixed VSync.
- Wiederherstellung nach Display- oder GPU-Resets.
- Diagnose von Frame-Zeiten, Jitter und VRR-Fallbacks.

## 9. Ergebnis

NovaOS erhält eine hardwareunabhängige Architektur zur Unterstützung variabler Bildwiederholraten.

Kompatible Displays können ihre Aktualisierungszeitpunkte dynamisch an die Frame-Ausgabe anpassen.

Dadurch werden Screen Tearing, sichtbares Ruckeln und unnötige Präsentationslatenzen reduziert, während ein zuverlässiger Fallback auf feste Bildwiederholraten gewährleistet bleibt.