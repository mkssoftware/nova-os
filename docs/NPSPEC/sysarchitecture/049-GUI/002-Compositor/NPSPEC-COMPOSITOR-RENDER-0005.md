# NPSPEC-COMPOSITOR-RENDER-0005 – Nova Vertical Synchronization

**Status:** Angenommen  
**Bereich:** Compositor / Bildsynchronisation  
**System:** NovaOS

## 1. Zweck

Nova Vertical Synchronization definiert die zeitliche Abstimmung der Bildausgabe mit dem Aktualisierungszyklus eines Displays.

Ziel ist die Vermeidung von Screen Tearing bei möglichst geringer Eingabelatenz, gleichmäßiger Frame-Ausgabe und effizienter Ressourcennutzung.

## 2. Architektur

- Integration in Nova Compositor Scheduling und Pipeline.
- Synchronisation über Nova Display Server und Grafiktreiber.
- Unterstützung von VSync und Variable Refresh Rate (VRR).
- Unabhängige Synchronisation pro Display.
- Nutzung hardwarebasierter Präsentationsereignisse.
- Unterstützung von Double und Triple Buffering.
- Asynchrone Frame-Präsentation.
- Hardwareunabhängige Schnittstelle mit gerätespezifischen Backends.

## 3. Kernfunktionen

- Synchronisation der Frame-Ausgabe mit dem Display.
- Ermittlung des nächsten Präsentationszeitpunkts.
- Verarbeitung von VBlank- und Präsentationsereignissen.
- Unterstützung unterschiedlicher Bildwiederholraten.
- Vermeidung von Screen Tearing.
- Unterstützung von VRR.
- Verwaltung ausstehender Präsentationen.
- Erkennung verpasster Präsentationszeitpunkte.
- Bereitstellung von Präsentationsfeedback.
- Anpassung der Rendering-Zeitpunkte an die Display-Frequenz.

## 4. Normative Anforderungen

- RENDER-001: Der Compositor MUSS synchronisierte Frame-Präsentation unterstützen.
- RENDER-002: VSync MUSS standardmäßig aktiviert sein, sofern die Hardware dies unterstützt.
- RENDER-003: Die Synchronisation MUSS auf tatsächlichen Display- oder Treiberereignissen basieren können.
- RENDER-004: Unterschiedliche Displays MÜSSEN unabhängig synchronisiert werden.
- RENDER-005: Unterschiedliche Bildwiederholraten MÜSSEN berücksichtigt werden.
- RENDER-006: VRR SOLL bei kompatibler Hardware unterstützt werden.
- RENDER-007: Frame-Präsentationen MÜSSEN mit Nova Buffer Synchronization abgestimmt werden.
- RENDER-008: Unvollständig gerenderte Buffer DÜRFEN nicht präsentiert werden.
- RENDER-009: Der Compositor SOLL Frames möglichst nahe am vorgesehenen Präsentationszeitpunkt fertigstellen.
- RENDER-010: Unnötige Frame-Warteschlangen MÜSSEN vermieden werden.
- RENDER-011: Veraltete, noch nicht präsentierte Frames SOLLEN verworfen werden können.
- RENDER-012: Verpasste Präsentationszeitpunkte MÜSSEN erkannt werden.
- RENDER-013: Präsentationsfeedback MUSS tatsächliche und geschätzte Zeitpunkte unterscheiden.
- RENDER-014: Direct Scanout MUSS die geltenden Synchronisationsregeln einhalten.
- RENDER-015: Änderungen der Display-Frequenz MÜSSEN dynamisch berücksichtigt werden.
- RENDER-016: Bei fehlender VSync-Unterstützung MUSS ein definierter Fallback verfügbar sein.
- RENDER-017: Synchronisationsfehler DÜRFEN keine unkontrollierte Buffer-Wiederverwendung verursachen.
- RENDER-018: Die Synchronisation MUSS ohne KI deterministisch funktionieren.

## 5. Synchronisationsverfahren

### Fixed VSync
Frames werden entsprechend dem festen Aktualisierungszyklus des Displays präsentiert.

### Variable Refresh Rate
Das Display passt seinen Aktualisierungszeitpunkt innerhalb des unterstützten Frequenzbereichs an die Frame-Bereitstellung an.

### Presentation Scheduling
Der Frame Scheduler bestimmt geeignete Rendering- und Präsentationszeitpunkte.

### Frame Pacing
Gleichmäßige zeitliche Verteilung der Frame-Ausgabe zur Reduzierung sichtbaren Ruckelns.

### Presentation Feedback
Rückmeldung über erfolgreiche, verspätete oder ausgefallene Präsentationen.

## 6. Synchronisationsablauf

1. **Query:** Display-Frequenz und Synchronisationsfähigkeiten ermitteln.
2. **Predict:** Nächsten geeigneten Präsentationszeitpunkt bestimmen.
3. **Schedule:** Rendering entsprechend der Deadline planen.
4. **Render:** Frame erzeugen.
5. **Synchronize:** Fertigstellung aller Buffer-Zugriffe sicherstellen.
6. **Queue:** Frame zur Präsentation bereitstellen.
7. **Present:** Frame zum geeigneten Zeitpunkt ausgeben.
8. **Feedback:** Tatsächlichen Präsentationsstatus erfassen.
9. **Adjust:** Zukünftige Zeitplanung anhand der Messwerte anpassen.

## 7. Latenzoptimierung

- Just-in-Time-Rendering zur Reduzierung der Eingabelatenz.
- Begrenzung ausstehender Frames.
- Bevorzugung aktueller gegenüber veralteten Bildinhalten.
- Vermeidung unnötiger CPU-GPU-Synchronisation.
- Nutzung hardwarebasierter Präsentationszeitstempel.
- Anpassung an schwankende Rendering-Zeiten.
- Keine kontinuierliche Frame-Erzeugung bei unveränderten Bildinhalten.
- Unabhängige Zeitplanung für Displays mit unterschiedlichen Frequenzen.

## 8. Fehlerbehandlung und Zuverlässigkeit

- Erkennung ausbleibender VBlank-Ereignisse.
- Behandlung verpasster Präsentationsdeadlines.
- Kontrollierter Wechsel zwischen VSync- und VRR-Betrieb.
- Wiederherstellung nach Display-Moduswechseln.
- Neuinitialisierung nach GPU- oder Display-Resets.
- Vermeidung blockierender Warteschleifen.
- Fallback auf treiberseitige oder zeitbasierte Präsentationsplanung.
- Erfassung von Frame-Drops, Jitter und Präsentationslatenzen.
- Integration in Nova Compositor Fault Recovery.

## 9. Ergebnis

NovaOS erhält eine zuverlässige und latenzoptimierte Synchronisation zwischen Compositor und Display-Hardware.

VSync verhindert Screen Tearing, während VRR bei kompatibler Hardware eine flexible und gleichmäßige Bildausgabe ermöglicht.

Durch präzises Frame-Scheduling und begrenzte Präsentationswarteschlangen werden flüssige Animationen, geringe Eingabelatenzen und eine energieeffiziente Desktopdarstellung unterstützt.