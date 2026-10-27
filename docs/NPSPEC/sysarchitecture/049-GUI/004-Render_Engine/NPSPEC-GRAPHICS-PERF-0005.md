# NPSPEC-GRAPHICS-PERF-0005 – Nova Rendering Performance Diagnostics

**Status:** Angenommen  
**Bereich:** Graphics / Performance / Diagnostics  
**System:** NovaOS

## 1. Zweck

Nova Rendering Performance Diagnostics definiert die Überwachung, Analyse und Diagnose der Grafikleistung innerhalb von NovaOS.

Ziel ist die frühzeitige Erkennung von Leistungsproblemen, Engpässen und ineffizienter Ressourcennutzung.

Das Subsystem stellt Messdaten für Entwickler, Systemdiagnose und automatische Leistungsoptimierungen bereit, ohne den normalen Rendering-Betrieb wesentlich zu beeinträchtigen.

## 2. Architektur

Das Diagnosesystem besteht aus folgenden Komponenten:

| Komponente | Aufgabe |
|---|---|
| Performance Monitor | Erfassung grundlegender Leistungsdaten |
| Frame Profiler | Analyse einzelner Frames |
| GPU Profiler | Messung GPU-bezogener Verarbeitung |
| Resource Monitor | Überwachung grafischer Ressourcen |
| Bottleneck Analyzer | Erkennung möglicher Leistungsengpässe |
| Diagnostics Recorder | Aufzeichnung von Messdaten |
| Diagnostics Interface | Bereitstellung der Diagnoseergebnisse |

Die Komponenten verwenden gemeinsame Zeitstempel und einheitliche Messdatenformate.

Das Diagnosesystem arbeitet unabhängig von der eigentlichen Rendering-Pipeline.

## 3. Leistungsmetriken

NovaOS erfasst folgende Metriken:

### Frame Performance
- Frames per Second (FPS).
- Frame Time.
- Durchschnittliche Frame Time.
- Minimum und Maximum.
- Perzentile der Frame Time.
- Frame Drops.
- Präsentationsverzögerungen.

### CPU Performance
- CPU-Rendering-Zeit.
- Command-Buffer-Erstellung.
- Draw-Call-Anzahl.
- Batch-Anzahl.
- Rendering-bezogene CPU-Auslastung.

### GPU Performance
- GPU-Verarbeitungszeit.
- GPU-Auslastung, sofern verfügbar.
- Warteschlangenlatenz.
- GPU-Speicherbelegung.
- Synchronisationswartezeiten.

### Ressourcen
- Anzahl aktiver Grafikressourcen.
- Speicherallokationen.
- Cache-Hit- und Miss-Raten.
- CPU-GPU-Datenübertragungen.
- Speicherbudget-Auslastung.

Nicht verfügbare Hardwaremetriken MÜSSEN als nicht unterstützt gekennzeichnet werden.

## 4. Frame Profiling

Der Frame Profiler analysiert die Verarbeitung einzelner Frames.

Er unterscheidet:

1. Frame Scheduling.
2. CPU Rendering.
3. GPU Command Submission.
4. GPU Execution.
5. Compositing.
6. Presentation.

Jede Phase kann mit Zeitstempeln und Dauerinformationen versehen werden.

Asynchrone CPU- und GPU-Operationen MÜSSEN zeitlich korrekt zugeordnet werden.

Überlappende Verarbeitungszeiten DÜRFEN nicht als ausschließlich sequenzielle Ausführungszeiten interpretiert werden.

## 5. GPU Profiling

GPU-Messungen erfolgen über die Nova Graphics Backend Abstraction.

Unterstützt werden, sofern verfügbar:

- GPU-Timestamps.
- Pipeline Statistics.
- Command-Queue-Auslastung.
- Shader-Ausführungszeiten.
- Speichertransferraten.
- GPU-Synchronisationszeiten.

GPU-Profiling MUSS die tatsächlichen Backend-Capabilities berücksichtigen.

Hardwareabhängige Messungen DÜRFEN nicht als allgemeingültige Leistungswerte dargestellt werden.

## 6. Bottleneck Analysis

Der Bottleneck Analyzer untersucht mögliche Leistungsengpässe.

Er berücksichtigt:

- CPU-Limitierung.
- GPU-Limitierung.
- Speicherbandbreite.
- GPU-Speichermangel.
- Übermäßige Draw Calls.
- Ineffizientes Batching.
- Häufige Cache-Misses.
- Unnötige Ressourcenübertragungen.
- Synchronisationswartezeiten.
- Unregelmäßige Frame Times.

Diagnoseergebnisse werden anhand messbarer Indikatoren erstellt.

Vermutete Ursachen MÜSSEN von nachgewiesenen Ursachen unterscheidbar sein.

Automatische Optimierungsvorschläge DÜRFEN keine unkontrollierten Systemänderungen auslösen.

## 7. Diagnosemodi

Das System unterstützt drei Diagnosemodi:

| Modus | Beschreibung |
|---|---|
| Basic | Kontinuierliche Erfassung grundlegender Leistungsdaten |
| Detailed | Erweiterte Messungen und Frame-Analyse |
| Profiling | Gezielte, zeitlich begrenzte Detailaufzeichnung |

Der Basic-Modus MUSS einen möglichst geringen Ressourcenverbrauch besitzen.

Aufwendige Profiling-Funktionen werden nur bei Bedarf aktiviert.

Anwendungen und Systemwerkzeuge können Diagnosesitzungen über definierte Schnittstellen anfordern.

## 8. Aufzeichnung und Auswertung

Diagnosedaten können in einem lokalen Ringbuffer gespeichert werden.

Unterstützt werden:

- Zeitlich begrenzte Aufzeichnungen.
- Ereignisbasierte Aufzeichnungen.
- Export strukturierter Messdaten.
- Vergleich mehrerer Messintervalle.
- Zuordnung zu Rendering-Kontexten.
- Erfassung von Leistungswarnungen.

Die Aufzeichnung MUSS durch konfigurierbare Speicher- und Zeitlimits begrenzt sein.

Eine dauerhafte Speicherung erfolgt nur gemäß den geltenden Diagnoseeinstellungen.

## 9. Echtzeitdiagnose

NovaOS stellt aktuelle Leistungsinformationen bereit.

Dazu gehören:

- FPS und Frame Time.
- CPU- und GPU-Zeiten.
- Speicherverbrauch.
- Cache-Effizienz.
- Frame Drops.
- Erkannte Leistungsengpässe.

Die Informationen können durch NovaStudio, Systemdiagnose oder optionale Performance-Overlays angezeigt werden.

Diagnoseanzeigen DÜRFEN die normale Desktopoberfläche nicht ungefragt überlagern.

## 10. Performance-Warnungen

Das System kann definierte Leistungsprobleme erkennen.

Beispiele:

- Wiederholte Frame-Time-Überschreitungen.
- Ungewöhnlich hoher Grafikspeicherverbrauch.
- Übermäßige GPU-Synchronisation.
- Häufige Speicherallokationen.
- Ungewöhnlich viele Draw Calls.
- Anhaltende Cache-Miss-Raten.

Warnungen werden mit Messwerten, Zeitstempeln und betroffenen Komponenten versehen.

NovaOS SOLL wiederkehrende Warnungen zusammenfassen, statt den Benutzer mit einzelnen Meldungen zu belasten.

## 11. Sicherheit und Datenschutz

Diagnosedaten können Informationen über Anwendungen und deren Ressourcenverbrauch enthalten.

Deshalb gilt:

- Anwendungsbezogene Detaildaten unterliegen Nova.Trust.
- Unberechtigte Anwendungen DÜRFEN keine fremden Prozessdetails auslesen.
- Messdaten werden standardmäßig lokal verarbeitet.
- Netzwerkübertragungen erfolgen nicht automatisch.
- Diagnoseaufzeichnungen MÜSSEN kontrolliert gelöscht werden können.
- Die Diagnose DARF keine geschützten Grafik- oder Speicherinhalte offenlegen.

## 12. Integration

- **Nova Frame Pacing:** Frame-Timing und Präsentationsanalyse.
- **Nova Render Batching:** Draw-Call- und Batch-Statistiken.
- **Nova Graphics Caching:** Cache-Effizienz.
- **Nova Graphics Memory Optimization:** Speicherbelegung und Ressourcenbudgets.
- **Nova GPU Command Submission:** GPU-Warteschlangen und Synchronisation.
- **Nova Graphics Backend Abstraction:** Hardwareabhängige Messdaten.
- **Nova Compositor:** Compositing- und Präsentationszeiten.
- **NovaStudio:** Entwicklerprofiling und Performance-Analyse.
- **Nova System Diagnostics:** Zentrale Darstellung von Diagnoseergebnissen.

## 13. Normative Anforderungen

- **GRAPHICS-001:** NovaOS MUSS eine zentrale Grafikleistungsdiagnose bereitstellen.
- **GRAPHICS-002:** Frame Times und FPS MÜSSEN messbar sein.
- **GRAPHICS-003:** CPU- und GPU-Verarbeitungszeiten MÜSSEN unterscheidbar sein.
- **GRAPHICS-004:** GPU-spezifische Metriken MÜSSEN über Backend-Capabilities ermittelt werden.
- **GRAPHICS-005:** Nicht unterstützte Messwerte MÜSSEN eindeutig gekennzeichnet werden.
- **GRAPHICS-006:** Frame Drops und Präsentationsverzögerungen MÜSSEN erkennbar sein.
- **GRAPHICS-007:** Grafikressourcen und Speicherverbrauch MÜSSEN überwacht werden können.
- **GRAPHICS-008:** Draw Calls und Rendering-Batches MÜSSEN statistisch erfassbar sein.
- **GRAPHICS-009:** Diagnoseergebnisse MÜSSEN mit einer monotonen Zeitquelle korrelierbar sein.
- **GRAPHICS-010:** Unterschiedliche Diagnosemodi MÜSSEN unterstützt werden.
- **GRAPHICS-011:** Aufzeichnungen MÜSSEN durch Ressourcenlimits begrenzt sein.
- **GRAPHICS-012:** Diagnosedaten MÜSSEN über definierte Schnittstellen abrufbar sein.
- **GRAPHICS-013:** Leistungsengpässe SOLLEN automatisch erkannt werden.
- **GRAPHICS-014:** Diagnosefunktionen DÜRFEN die Rendering-Korrektheit nicht verändern.
- **GRAPHICS-015:** Anwendungsbezogene Diagnosedaten MÜSSEN den Berechtigungsregeln entsprechen.
- **GRAPHICS-016:** Die grundlegende Diagnose MUSS ohne GPU-Beschleunigung funktionieren.
- **GRAPHICS-017:** Die Diagnose MUSS vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 14. Fehlerbehandlung

- Nicht verfügbare Hardwarezähler werden als nicht unterstützt gemeldet.
- Fehlgeschlagene Messungen DÜRFEN das Rendering nicht unterbrechen.
- Beschädigte Diagnoseaufzeichnungen werden kontrolliert verworfen.
- Bei Speicherknappheit werden Detailaufzeichnungen reduziert oder beendet.
- Zeitstempelüberläufe und ungültige Messintervalle MÜSSEN erkannt werden.
- GPU-Ausfälle werden an Nova Graphics Driver Recovery weitergeleitet.
- Diagnosefehler DÜRFEN keine Grafikressourcen beschädigen.

## 15. Ergebnis

NovaOS erhält eine zentrale Infrastruktur zur Überwachung und Analyse seiner Grafikleistung.

Entwickler und Systemkomponenten können Frame-Zeiten, Ressourcenverbrauch und mögliche Rendering-Engpässe nachvollziehen.

Die Architektur ermöglicht gezielte Leistungsoptimierungen und unterstützt die automatische Erkennung wiederkehrender Performance-Probleme.

**Grundsatz:** NovaOS soll nicht nur schnell rendern, sondern auch nachvollziehbar machen können, wo Zeit und Ressourcen verbraucht werden – ohne den Benutzer unnötig damit zu beschäftigen.