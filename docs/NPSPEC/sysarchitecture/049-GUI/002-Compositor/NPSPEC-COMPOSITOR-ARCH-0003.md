# NPSPEC-COMPOSITOR-ARCH-0003 – Nova Compositor Scheduling

**Status:** Angenommen  
**Bereich:** Compositor / Frame-Scheduling  
**System:** NovaOS

## 1. Zweck

Nova Compositor Scheduling definiert die zeitliche Planung und Priorisierung der Bildkomposition.

Ziel ist eine flüssige, latenzarme und energieeffiziente Darstellung durch bedarfsgerechte Frame-Verarbeitung und optimale Nutzung verfügbarer Systemressourcen.

## 2. Architektur

- Ereignisgesteuerter Frame Scheduler innerhalb des Nova Compositors.
- Unabhängige Zeitplanung pro Display.
- Integration in Nova Compositor Pipeline und Nova Display Server.
- Unterstützung fester und variabler Bildwiederholraten.
- Deadline-orientierte Planung von Rendering und Präsentation.
- Asynchrone CPU- und GPU-Verarbeitung.
- Dynamische Priorisierung interaktiver Oberflächen.
- Integration in das NovaOS-Scheduling- und Ressourcenmanagement.

## 3. Kernfunktionen

- Planung und Auslösung von Kompositionszyklen.
- Berechnung geeigneter Rendering-Startzeitpunkte.
- Synchronisierung mit VSync und Variable Refresh Rate (VRR).
- Verwaltung von Frame-Deadlines.
- Priorisierung interaktiver und sichtbarer Inhalte.
- Zusammenfassung mehrerer Surface-Aktualisierungen.
- Erkennung und Behandlung verspäteter Frames.
- Begrenzung ausstehender Frames.
- Adaptive Anpassung an CPU- und GPU-Auslastung.
- Reduzierung unnötiger Frame-Erzeugung.
- Bereitstellung von Frame-Callbacks und Präsentationsfeedback.
- Erfassung von Rendering-Zeiten und Präsentationslatenzen.

## 4. Normative Anforderungen

- COMPOSITOR-001: Jedes Display MUSS unabhängig geplante Kompositionszyklen unterstützen.
- COMPOSITOR-002: Frame-Scheduling MUSS die aktuelle Bildwiederholrate berücksichtigen.
- COMPOSITOR-003: Rendering SOLL möglichst spät beginnen, sofern die Präsentationsdeadline zuverlässig eingehalten werden kann.
- COMPOSITOR-004: Unveränderte Bildschirminhalte DÜRFEN keine kontinuierliche Neuberechnung erfordern.
- COMPOSITOR-005: Mehrere Surface-Aktualisierungen SOLLEN innerhalb eines Frames zusammengefasst werden.
- COMPOSITOR-006: Interaktive Inhalte SOLLEN gegenüber nicht zeitkritischen Aktualisierungen priorisiert werden.
- COMPOSITOR-007: Langsame Clients DÜRFEN andere Clients nicht blockieren.
- COMPOSITOR-008: Veraltete, noch nicht präsentierte Frames SOLLEN verworfen werden können.
- COMPOSITOR-009: Bereits verwendete Buffer DÜRFEN nicht vor Abschluss ihrer Nutzung freigegeben werden.
- COMPOSITOR-010: Frame-Deadlines MÜSSEN überwacht werden.
- COMPOSITOR-011: Unterschiedliche Display-Frequenzen DÜRFEN sich nicht unnötig gegenseitig begrenzen.
- COMPOSITOR-012: VRR MUSS bei der Präsentationsplanung berücksichtigt werden, sofern verfügbar.
- COMPOSITOR-013: Die Anzahl gleichzeitig ausstehender Frames MUSS begrenzbar sein.
- COMPOSITOR-014: Anwendungen MÜSSEN Frame-Callbacks und Präsentationsfeedback erhalten können.
- COMPOSITOR-015: Der Scheduler MUSS auf Änderungen der Display-Konfiguration reagieren.
- COMPOSITOR-016: Bei Ressourcenknappheit SOLL die Reaktionsfähigkeit gegenüber visuellen Effekten priorisiert werden.
- COMPOSITOR-017: Sicherheitskritische Systemoberflächen MÜSSEN zuverlässig und mit angemessener Priorität dargestellt werden.
- COMPOSITOR-018: Scheduling-Entscheidungen MÜSSEN ohne KI deterministisch möglich sein.

## 5. Scheduling-Strategien

### Deadline Scheduling
Rendering-Aufgaben werden anhand des nächsten möglichen Präsentationszeitpunkts geplant.

### Just-in-Time Rendering
Frames werden möglichst spät erzeugt, um Eingabelatenzen und unnötige Wartezeiten zu reduzieren.

### Adaptive Scheduling
Rendering-Zeitpunkte werden anhand gemessener CPU-, GPU- und Präsentationszeiten angepasst.

### Priority Scheduling
Interaktive und sicherheitskritische Oberflächen erhalten eine angemessene Priorisierung.

### Frame Coalescing
Mehrere Aktualisierungen werden zu einem gemeinsamen Kompositionszyklus zusammengefasst.

### Idle Scheduling
Bei unveränderten Bildinhalten werden unnötige Kompositionszyklen ausgesetzt.

## 6. Scheduling-Ablauf

1. **Observe:** Surface-Änderungen und Display-Ereignisse erfassen.
2. **Predict:** Voraussichtliche Rendering-Dauer bestimmen.
3. **Schedule:** Rendering-Startzeitpunkt und Deadline festlegen.
4. **Collect:** Aktuelle verfügbare Surface-Zustände übernehmen.
5. **Compose:** Erforderliche Bildbereiche rendern.
6. **Synchronize:** GPU-Operationen und Buffer synchronisieren.
7. **Present:** Frame zum vorgesehenen Zeitpunkt ausgeben.
8. **Measure:** Tatsächliche Latenzen und Verarbeitungszeiten erfassen.

Die gemessenen Werte SOLLEN zur kontinuierlichen Anpassung zukünftiger Scheduling-Entscheidungen verwendet werden.

## 7. Performance und Zuverlässigkeit

- Minimierung der Eingabe-zu-Anzeige-Latenz.
- Vermeidung unnötiger CPU- und GPU-Wartezeiten.
- Begrenzung der Frame-Queue zur Vermeidung zusätzlicher Latenz.
- Dynamische Anpassung an schwankende Rendering-Zeiten.
- Priorisierung aktueller Bildinhalte.
- Vermeidung unnötiger Rendering-Aufgaben bei verdeckten Oberflächen.
- Energieeinsparung durch bedarfsgerechte Frame-Erzeugung.
- Unabhängiges Scheduling mehrerer Displays.
- Kontrollierte Behandlung verpasster Präsentationszeitpunkte.
- Diagnose von Frame-Drops, Jitter und Scheduling-Verzögerungen.

## 8. Ergebnis

NovaOS erhält einen adaptiven, deadline-orientierten Frame Scheduler für seinen Compositor.

Die Bildkomposition erfolgt nur bei Bedarf und möglichst nahe am tatsächlichen Präsentationszeitpunkt.

Dadurch werden Eingabelatenzen, unnötige Berechnungen und Energieverbrauch reduziert, während eine flüssige und reaktionsschnelle Desktopoberfläche gewährleistet wird.