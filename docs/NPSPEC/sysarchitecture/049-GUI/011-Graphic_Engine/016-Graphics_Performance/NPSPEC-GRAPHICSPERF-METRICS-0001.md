# NPSPEC-GRAPHICSPERF-METRICS-0001 – Nova Graphics Performance Metrics

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Graphics / Nova Graphics Performance

## 1. Zweck

Nova Graphics Performance Metrics definiert die einheitliche Erfassung, Darstellung und Bereitstellung von Leistungskennzahlen innerhalb der NovaOS-Grafikarchitektur.

Ziel ist eine präzise Bewertung von CPU-, GPU-, Speicher-, Rendering- und Display-Leistung.

Alle Kennzahlen MÜSSEN eindeutig definiert, vergleichbar und den verantwortlichen Subsystemen zuordenbar sein.

## 2. Architekturprinzipien

- **Unified:** Einheitliches Kennzahlenmodell für alle Grafiksubsysteme.
- **Low-Overhead:** Möglichst geringer Messaufwand.
- **Frame-Aware:** Zuordnung von Messwerten zu einzelnen Frames.
- **Subsystem-Aware:** Eindeutige Zuordnung zu Grafikkomponenten.
- **Time-Based:** Verwendung definierter Zeitintervalle.
- **Hardware-Aware:** Nutzung verfügbarer Hardwarezähler.
- **Adaptive:** Anpassbarer Messumfang.
- **Reliable:** Unterscheidung zwischen Messwerten, Schätzungen und nicht verfügbaren Daten.

## 3. Architektur

Nova Graphics Performance Metrics verwendet die zentrale Nova Graphics Performance Architecture.

| Komponente | Verantwortung |
|---|---|
| Metrics Registry | Registrierung verfügbarer Kennzahlen |
| Metrics Collector | Erfassung der Messwerte |
| Metrics Aggregator | Zusammenfassung von Messdaten |
| Metrics Validator | Prüfung der Messwertgültigkeit |
| Metrics Snapshot Provider | Bereitstellung konsistenter Messzustände |
| Metrics Export Interface | Zugriff für autorisierte Analyse- und Optimierungssysteme |

Die Erfassung MUSS über definierte Schnittstellen erfolgen.

Eine zweite Performance-Infrastruktur DARF nicht entstehen.

## 4. Metrics Data Model

Jede Kennzahl MUSS folgende Eigenschaften besitzen:

| Eigenschaft | Beschreibung |
|---|---|
| Metric ID | Eindeutige Kennzahl |
| Metric Name | Lesbare Bezeichnung |
| Metric Type | Counter, Gauge, Duration oder Rate |
| Unit | Maßeinheit |
| Subsystem ID | Zugehöriges Grafiksubsystem |
| Timestamp | Messzeitpunkt |
| Interval | Zugehöriger Messzeitraum |
| Value | Erfasster Wert |
| Source | Herkunft der Messung |
| Validity | Gültigkeitsstatus |
| Frame ID | Zugehöriger Frame, sofern vorhanden |

Optionale Dimensionen wie GPU, Queue, Render Pass, Surface oder Output MÜSSEN unterstützt werden können.

## 5. CPU Metrics

NovaOS SOLL folgende CPU-Kennzahlen erfassen:

| Kennzahl | Beschreibung |
|---|---|
| CPU Render Time | CPU-Zeit für Rendering-Vorbereitung |
| Scene Update Time | Dauer der Scene-Graph-Aktualisierung |
| Damage Analysis Time | Aufwand der Damage-Berechnung |
| Render Graph Build Time | Dauer der Render-Graph-Erstellung |
| Command Recording Time | Dauer der GPU-Befehlsaufzeichnung |
| Submission CPU Time | CPU-Aufwand für GPU-Submission |
| CPU Wait Time | Wartezeit auf Synchronisation |

CPU-Zeiten MÜSSEN von GPU-Ausführungszeiten getrennt werden.

## 6. GPU Metrics

NovaOS SOLL folgende GPU-Kennzahlen erfassen:

| Kennzahl | Beschreibung |
|---|---|
| GPU Execution Time | Dauer der GPU-Ausführung |
| GPU Queue Time | Wartezeit vor GPU-Ausführung |
| Render Pass Time | Ausführungszeit einzelner Render-Passes |
| Compute Time | Dauer von Compute-Operationen |
| Transfer Time | Dauer von GPU-Transferoperationen |
| GPU Utilization | GPU-Auslastung, sofern verfügbar |
| Shader Execution Metrics | Hardwareabhängige Shader-Kennzahlen |
| GPU Stall Metrics | Erkennbare GPU-Wartezustände |

GPU-Zeitmessungen MÜSSEN verfügbare Hardware-Zeitstempel und deren Auflösung berücksichtigen.

## 7. Frame Metrics

NovaOS MUSS folgende Frame-Kennzahlen unterstützen:

| Kennzahl | Beschreibung |
|---|---|
| Frame Interval | Zeit zwischen aufeinanderfolgenden Präsentationen |
| Frame Processing Time | Dauer der Frame-Verarbeitung |
| Frame Latency | Zeit zwischen definiertem Auslöser und Präsentation |
| Frame Pacing Deviation | Abweichung vom geplanten Präsentationszeitpunkt |
| Frames Submitted | Anzahl eingereichter Frames |
| Frames Presented | Anzahl bestätigter Präsentationen |
| Frames Dropped | Anzahl verworfener Frames |
| Frames Reused | Anzahl wiederverwendeter Bildzustände |

Frame-Latenz MUSS mit eindeutig definiertem Start- und Endereignis gemessen werden.

Die Anzahl präsentierter Frames DARF nicht mit der Anzahl gerenderter Frames gleichgesetzt werden.

## 8. Memory Metrics

NovaOS SOLL folgende Speicherkennzahlen erfassen:

| Kennzahl | Beschreibung |
|---|---|
| GPU Memory Used | Belegter GPU-Speicher |
| GPU Memory Budget | Verfügbares Speicherbudget |
| Resource Allocation Count | Anzahl der Ressourcenallokationen |
| Allocation Time | Dauer von Speicherallokationen |
| Resource Evictions | Anzahl verdrängter Ressourcen |
| Cache Memory Usage | Speicherverbrauch der Grafik-Caches |
| Transient Memory Usage | Speicherbedarf temporärer Ressourcen |
| Memory Pressure Level | Aktueller Speicherbelastungszustand |

Speicherkennzahlen MÜSSEN zwischen logischer Ressourcengröße und tatsächlicher physischer Belegung unterscheiden können.

## 9. Bandwidth Metrics

NovaOS SOLL folgende Kennzahlen unterstützen:

- CPU-GPU-Upload-Volumen.
- GPU-CPU-Readback-Volumen.
- GPU-interne Transfermengen, soweit messbar.
- Speicherbandbreitenauslastung, sofern verfügbar.
- Anzahl vermiedener Datenkopien.
- Übertragungsdauer.
- Effektive Transferrate.

Geschätzte Datenmengen DÜRFEN nicht als tatsächlich gemessener Hardwaredurchsatz ausgewiesen werden.

## 10. Rendering und Damage Metrics

NovaOS SOLL folgende Kennzahlen erfassen:

| Kennzahl | Beschreibung |
|---|---|
| Draw Calls | Anzahl ausgeführter Zeichenbefehle |
| Batched Draw Calls | Durch Batching zusammengefasste Operationen |
| Render Pass Count | Anzahl ausgeführter Render-Passes |
| Damage Area | Größe der beschädigten Bildregionen |
| Rendered Area | Tatsächlich neu berechnete Bildfläche |
| Culled Operations | Anzahl entfallener Zeichenoperationen |
| Reused Results | Anzahl wiederverwendeter Ergebnisse |
| Cache Hit Rate | Anteil erfolgreicher Cache-Zugriffe |

Damage Area und Rendered Area MÜSSEN getrennt erfasst werden können.

## 11. Compositor und Display Metrics

NovaOS SOLL folgende Kennzahlen unterstützen:

- Composition Time.
- Surface Commit Latency.
- Presentation Latency.
- Direct Scanout Usage.
- Hardware Overlay Usage.
- Buffer Reuse Count.
- Partial Presentation Usage.
- Display Refresh Rate.

Bestätigte Präsentationszeitpunkte MÜSSEN von geschätzten Zeitpunkten unterscheidbar sein.

## 12. Metrics Aggregation

NovaOS MUSS Kennzahlen über definierte Zeiträume zusammenfassen können.

Unterstützte Aggregationen SOLLEN umfassen:

- Minimum.
- Maximum.
- Mittelwert.
- Median.
- Perzentile, insbesondere P95 und P99.
- Summe.
- Ereignisanzahl.
- Rate pro Zeitintervall.

Aggregationen MÜSSEN die Bedeutung und Einheit der jeweiligen Kennzahl berücksichtigen.

Zeitlich überlappende CPU- und GPU-Operationen DÜRFEN nicht unzulässig zu einer Gesamtdauer addiert werden.

## 13. Sampling und Overhead

NovaOS MUSS den Messaufwand begrenzen.

Dabei gilt:

- Messintervalle MÜSSEN konfigurierbar sein.
- Aufwendige Hardwareabfragen SOLLEN nur bei Bedarf erfolgen.
- GPU-Messwerte SOLLEN asynchron ausgelesen werden.
- Messpuffer MÜSSEN begrenzt sein.
- Bei Überlast DÜRFEN optionale Messwerte verworfen werden.
- Datenverluste MÜSSEN erkennbar bleiben.

Die Messung DARF kritische Rendering-Pfade nicht unnötig blockieren.

## 14. Metrics Feedback

Nova Graphics Performance Metrics MUSS Messwerte für folgende Systeme bereitstellen können:

- Nova Graphics Profiling.
- Nova Rendering Cost Estimation.
- Nova Adaptive Rendering Strategy.
- Nova Adaptive Rendering Quality.
- Nova Adaptive Frame Scheduling.
- Nova Graphics Memory Bandwidth Optimization.
- Nova Power-Aware Graphics Optimization.

Optimierungsentscheidungen MÜSSEN mit gültigen und ausreichend aktuellen Messwerten arbeiten.

## 15. Sicherheit und Konsistenz

NovaOS MUSS folgende Anforderungen erfüllen:

- Messwerte MÜSSEN eindeutig typisiert sein.
- Ungültige oder fehlende Werte MÜSSEN erkennbar bleiben.
- Zeitstempel MÜSSEN einer definierten Zeitbasis zugeordnet sein.
- Messdaten MÜSSEN vor unautorisiertem Zugriff geschützt werden.
- Prozessübergreifende Kennzahlen DÜRFEN nur autorisiert bereitgestellt werden.
- Performance-Erfassung DARF die Rendering-Korrektheit nicht beeinflussen.

## 16. Verbindliche Anforderungen

1. NovaOS MUSS ein einheitliches Graphics Performance Metrics Model bereitstellen.
2. Kennzahlen MÜSSEN eindeutig identifizierbar und typisiert sein.
3. Maßeinheiten MÜSSEN verbindlich definiert sein.
4. CPU-, GPU-, Frame- und Speicherkennzahlen MÜSSEN unterstützt werden.
5. Rendering-, Damage- und Cache-Kennzahlen SOLLEN unterstützt werden.
6. Kennzahlen MÜSSEN Subsystemen zugeordnet werden können.
7. Frame-bezogene Kennzahlen MÜSSEN Frame IDs unterstützen.
8. Messwerte, Schätzungen und nicht verfügbare Daten MÜSSEN unterscheidbar sein.
9. Unterschiedliche Zeitbasen MÜSSEN korrekt behandelt werden.
10. Messintervalle MÜSSEN konfigurierbar sein.
11. Aggregationen MÜSSEN unterstützt werden.
12. Perzentile SOLLEN für Latenz- und Zeitmessungen verfügbar sein.
13. Messungen MÜSSEN ressourcenbegrenzt sein.
14. GPU-Messungen SOLLEN asynchron erfolgen.
15. Bestehende Optimierungssysteme MÜSSEN Kennzahlen verwenden können.
16. Messdaten MÜSSEN vor unautorisiertem Zugriff geschützt werden.
17. Die funktionale Rendering-Korrektheit MUSS unabhängig von der Messwerterfassung bleiben.

## 17. Ergebnis

Nova Graphics Performance Metrics stellt ein einheitliches Kennzahlensystem für die gesamte NovaOS-Grafikarchitektur bereit.

Durch präzise CPU-, GPU-, Frame-, Speicher- und Rendering-Messwerte können Leistungsengpässe nachvollziehbar erkannt und Optimierungen objektiv bewertet werden.

Damit erhält NovaOS eine zuverlässige Datengrundlage für Performance-Analyse, Profiling und adaptive Grafikoptimierung.