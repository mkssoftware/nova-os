# NPSPEC-DISPLAY-OUTPUT-0004 – Nova Refresh Rate Management

**Status:** Angenommen  
**Bereich:** Display / Bildwiederholrate  
**System:** NovaOS

## 1. Zweck

Nova Refresh Rate Management definiert die Verwaltung und Synchronisierung der Bildwiederholraten sämtlicher Display-Ausgänge.

Ziel ist eine flüssige, latenzarme und energieeffiziente Bildausgabe bei unterschiedlichen Bildwiederholraten und variabler Display-Aktualisierung.

## 2. Architektur

- Integration in den Nova Display Server.
- Unabhängige Verwaltung der Bildwiederholrate pro Display.
- Hardwareunabhängige Schnittstelle zu den Grafiktreibern.
- Zusammenarbeit mit Compositor und Frame Scheduler.
- Unterstützung fester und variabler Bildwiederholraten.
- Ereignisgesteuerte Synchronisierung statt permanentem Polling.
- Dynamische Anpassung an Anwendungslast und Energiezustand.
- Trennung zwischen Display-Refresh-Rate und Anwendungs-Framerate.

## 3. Kernfunktionen

- Erkennung unterstützter Bildwiederholraten.
- Auswahl und Änderung der aktiven Bildwiederholrate.
- Unterstützung unterschiedlicher Frequenzen pro Display.
- Verwaltung von VSync und Präsentationszeitpunkten.
- Unterstützung von Variable Refresh Rate (VRR).
- Adaptive Anpassung der Bildwiederholrate.
- Synchronisierung von Frame-Übergaben.
- Erkennung verspäteter und ausgelassener Frames.
- Bereitstellung präziser Frame-Timing-Informationen.
- Energieoptimierung bei statischen Bildschirminhalten.
- Unterstützung von Direct Scanout.
- Wiederherstellung gültiger Frequenzeinstellungen.

## 4. Normative Anforderungen

- DISPLAY-001: Unterstützte Bildwiederholraten MÜSSEN über die Grafiktreiber ermittelt werden.
- DISPLAY-002: Jedes Display MUSS seine Bildwiederholrate unabhängig verwalten können.
- DISPLAY-003: Unterschiedliche Bildwiederholraten MÜSSEN gleichzeitig unterstützt werden.
- DISPLAY-004: Bildwiederholraten MÜSSEN mit ausreichender Präzision einschließlich nicht ganzzahliger Frequenzen dargestellt werden.
- DISPLAY-005: Der Compositor MUSS Informationen über die aktuellen Präsentationszeitpunkte erhalten können.
- DISPLAY-006: VSync MUSS zur Vermeidung von Tearing unterstützt werden.
- DISPLAY-007: VRR SOLL verwendet werden, sofern Display, Grafiktreiber und Ausgabepfad dies unterstützen.
- DISPLAY-008: Die Anwendungs-Framerate DARF nicht grundsätzlich an die Bildwiederholrate eines anderen Displays gebunden werden.
- DISPLAY-009: Frame-Scheduling MUSS unterschiedliche Display-Frequenzen berücksichtigen.
- DISPLAY-010: Unveränderte Bildschirminhalte SOLLEN keine unnötigen Renderoperationen verursachen.
- DISPLAY-011: Adaptive Frequenzänderungen DÜRFEN die Bildausgabe nicht destabilisieren.
- DISPLAY-012: Frequenzänderungen MÜSSEN vor ihrer Aktivierung validiert werden.
- DISPLAY-013: Fehlgeschlagene Frequenzänderungen MÜSSEN kontrolliert zurückgesetzt werden.
- DISPLAY-014: Anwendungen SOLLEN Präsentationsfeedback und Frame-Timing-Informationen erhalten können.
- DISPLAY-015: Energieoptimierungen DÜRFEN die vom Benutzer festgelegten Anzeigepräferenzen nicht unkontrolliert überschreiben.
- DISPLAY-016: Die Verwaltung MUSS mit mehreren Grafikkarten und virtuellen Displays kompatibel sein.

## 5. Betriebsmodi

### Fixed Refresh Rate
Betrieb mit einer festgelegten Bildwiederholrate.

### Variable Refresh Rate
Dynamische Anpassung der Display-Aktualisierung an die Frame-Ausgabe innerhalb unterstützter Hardwaregrenzen.

### Adaptive Refresh Rate
Automatische Auswahl geeigneter Bildwiederholraten anhand von Aktivität, Energiezustand und Benutzerpräferenzen.

### Power Saving
Reduzierung der Bildwiederholrate bei geringer Aktivität, sofern technisch möglich und zulässig.

### Performance
Bevorzugung hoher Bildwiederholraten für besonders flüssige Darstellung und geringe Eingabelatenz.

## 6. Frame-Synchronisierung

- Der Display Server verwaltet die verfügbaren Präsentationszeitpunkte.
- Der Compositor plant die Frame-Ausgabe entsprechend dem jeweiligen Display.
- Anwendungen erhalten Frame-Callbacks zur bedarfsgerechten Aktualisierung.
- Synchronisationsobjekte verhindern die vorzeitige Wiederverwendung von Buffern.
- Verspätete Frames werden kontrolliert behandelt.
- Unterschiedliche Display-Frequenzen werden unabhängig koordiniert.
- VRR wird nur innerhalb der unterstützten Frequenzgrenzen aktiviert.

## 7. Performance und Zuverlässigkeit

- Minimierung von Eingabe- und Präsentationslatenzen.
- Vermeidung unnötiger Frame-Wiederholungen.
- Reduzierung von Tearing und sichtbarem Ruckeln.
- Unabhängiges Frame-Timing pro Display.
- Adaptive Reduzierung unnötiger Renderlast.
- Energieeffiziente Verarbeitung statischer Inhalte.
- Wiederherstellung stabiler Frequenzeinstellungen bei Fehlern.
- Diagnose von Frame-Drops, Timing-Abweichungen und Präsentationsverzögerungen.

## 8. Ergebnis

NovaOS erhält eine flexible und energieeffiziente Verwaltung der Bildwiederholraten.

Displays können unabhängig mit festen oder variablen Frequenzen betrieben werden, während Compositor und Anwendungen ihre Frame-Ausgabe optimal synchronisieren.

Die Architektur ermöglicht flüssige Animationen, geringe Eingabelatenzen und einen ressourcenschonenden Desktopbetrieb.