# NPSPEC-FRAME-PACING-0001 – Nova Frame Pacing

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Frame Scheduler

## 1. Zweck

Nova Frame Pacing definiert die zeitliche Verteilung und Koordination grafischer Frames innerhalb der NovaOS-Grafikpipeline.

Ziel ist eine gleichmäßige, latenzarme Bilddarstellung ohne unnötige Frame-Erzeugung, übermäßige Warteschlangen oder unregelmäßige Präsentationsintervalle.

Frame Pacing steuert die zeitliche Ausführung tatsächlich benötigter Frames. Es erzeugt selbst keinen zusätzlichen Aktualisierungsbedarf.

## 2. Architekturprinzipien

- **Demand-Driven:** Nur erforderliche Frames werden verarbeitet.
- **Presentation-Aware:** Tatsächliche Ausgabezeitpunkte bestimmen die Planung.
- **Deadline-Based:** Rendering beginnt passend zum angestrebten Präsentationszeitpunkt.
- **Latency-Aware:** Unnötige Wartezeiten werden minimiert.
- **Feedback-Driven:** Präsentationsdaten verbessern zukünftige Entscheidungen.
- **Adaptive:** Unterstützung unterschiedlicher Bildwiederholraten und Systemlasten.
- **Multi-Display:** Unabhängiges Frame Pacing je Ausgabe.
- **Deterministic:** Reproduzierbare Entscheidungen bei definierten Eingaben.

## 3. Frame Pacing Model

Jede Bildschirmausgabe besitzt einen eigenen Pacing-Zustand.

| Eigenschaft | Beschreibung |
|---|---|
| Output ID | Zugehörige Bildschirmausgabe |
| Target Interval | Angestrebtes Präsentationsintervall |
| Last Presentation | Letzter bestätigter Präsentationszeitpunkt |
| Next Target | Nächster geplanter Präsentationszeitpunkt |
| Estimated Render Time | Geschätzte Rendering-Dauer |
| Estimated Composition Time | Geschätzte Kompositionsdauer |
| Safety Margin | Zeitreserve für Schwankungen |
| Queue Depth | Anzahl ausstehender Frames |
| Pacing Mode | Aktuelle Scheduling-Strategie |

Prognostizierte und tatsächlich gemessene Werte MÜSSEN unterscheidbar bleiben.

## 4. Frame Timing

NovaOS SOLL den Beginn der Frame-Verarbeitung anhand des angestrebten Präsentationszeitpunkts bestimmen.

Grundlage:

`Frame Start = Target Presentation - Estimated Processing Time - Safety Margin`

Dabei gilt:

1. Rendering- und Kompositionszeiten werden berücksichtigt.
2. Erforderliche GPU-Synchronisation wird einbezogen.
3. Die Zeitreserve kann adaptiv angepasst werden.
4. Zu frühe Frame-Berechnungen SOLLEN vermieden werden.
5. Zeitkritische Aktualisierungen DÜRFEN bevorzugt verarbeitet werden.

Die Berechnung MUSS auf einer monotonen Zeitbasis beruhen.

## 5. Presentation Interval Management

Frame Pacing SOLL gleichmäßige Präsentationsintervalle ermöglichen.

Dabei gilt:

- VSync-Zeitpunkte werden berücksichtigt.
- Variable Refresh Rate kann genutzt werden.
- Schwankungen der Rendering-Dauer werden kompensiert.
- Verpasste Deadlines werden erkannt.
- Nicht erforderliche Zwischenframes werden vermieden.

Gleichmäßige Intervalle sind nur für tatsächlich fortlaufend aktualisierte Inhalte anzustreben.

Statische Inhalte benötigen keine regelmäßigen neuen Frames.

## 6. Frame Queue Management

NovaOS MUSS ausstehende Frames kontrolliert verwalten.

Dabei gilt:

- Die Anzahl gleichzeitig ausstehender Frames SOLL begrenzt werden.
- Veraltete, noch nicht präsentierte Frames DÜRFEN ersetzt werden.
- Erforderliche Zustandsänderungen und Damage MÜSSEN erhalten bleiben.
- Bereits verwendete Buffer DÜRFEN nicht vorzeitig freigegeben werden.
- Die Queue-Tiefe SOLL an Latenz und Systemlast angepasst werden.

Eine größere Frame Queue DARF nicht automatisch als Leistungsverbesserung betrachtet werden.

## 7. Adaptive Pacing

NovaOS SOLL Frame Pacing dynamisch anpassen können.

Relevante Faktoren:

- Tatsächliche Präsentationsintervalle
- CPU- und GPU-Auslastung
- Rendering-Kosten
- Verpasste Deadlines
- Bildwiederholrate
- VRR-Fähigkeiten
- Eingabelatenz
- Energiezustand

Häufige unnötige Anpassungen SOLLEN durch Hysteresis und geeignete Messwertglättung vermieden werden.

## 8. Latency Optimization

Nova Frame Pacing SOLL die Zeit zwischen einer relevanten Zustandsänderung und ihrer sichtbaren Darstellung minimieren.

Dazu gehören:

- Vermeidung unnötiger Frame-Warteschlangen.
- Rechtzeitige statt übermäßig frühe Berechnung.
- Bevorzugung zeitkritischer Eingabeänderungen.
- Zusammenfassung veralteter Zwischenzustände.
- Nutzung geeigneter Präsentationsmodi.

Eine Latenzoptimierung DARF weder Synchronisationsanforderungen noch die grafische Korrektheit verletzen.

## 9. VSync und VRR Integration

Nova Frame Pacing MUSS mit Nova Vertical Synchronization zusammenarbeiten.

Bei fester Bildwiederholrate werden geeignete VSync-Zeitpunkte verwendet.

Bei VRR-fähiger Hardware DÜRFEN Präsentationsintervalle innerhalb der unterstützten Grenzen angepasst werden.

Die Auswahl des Präsentationsmodus erfolgt entsprechend den Hardwarefähigkeiten und Benutzereinstellungen.

Frame Pacing DARF keine regelmäßigen Rendering-Aufträge ausschließlich zur Einhaltung der physischen Display-Refresh-Rate erzeugen.

## 10. Multi-Display Pacing

Jede Bildschirmausgabe MUSS unabhängig gesteuert werden können.

Dabei gilt:

- Unterschiedliche Bildwiederholraten werden unterstützt.
- Jede Ausgabe besitzt eigene Präsentationsziele.
- Gemeinsame Ressourcen werden synchronisiert.
- Langsame Ausgaben SOLLEN schnellere Ausgaben nicht unnötig blockieren.
- Bildschirmübergreifende Synchronisation erfolgt nur bei ausdrücklichem Bedarf und entsprechender Unterstützung.

## 11. Presentation Feedback

Nova Frame Pacing MUSS Nova Presentation Feedback auswerten können.

Insbesondere:

- Tatsächliche Präsentationszeitpunkte
- Verpasste Präsentationsziele
- Ersetzte Frames
- Präsentationslatenzen
- Änderungen der Refresh Rate

Die Informationen SOLLEN zur Verbesserung von Verarbeitungsschätzungen und Scheduling-Entscheidungen verwendet werden.

Fehlende oder unzuverlässige Messwerte MÜSSEN erkannt werden.

## 12. Idle und Power Management

Ohne erforderlichen Aktualisierungsbedarf MUSS Frame Pacing inaktiv bleiben können.

Dabei gilt:

- Keine künstlichen Frame-Anforderungen.
- Keine periodischen Rendering-Aufträge.
- Keine unnötigen GPU-Submissions.
- Keine erzwungenen Präsentationen unveränderter Inhalte.

Der zuletzt gültige Frame bleibt sichtbar.

Die Display-Hardware kann diesen weiterhin unabhängig von der Frame-Erzeugung ausgeben.

## 13. Fehlerbehandlung

Bei verpassten Deadlines oder fehlerhaften Zeitprognosen MUSS NovaOS einen gültigen Ausgabezustand erhalten.

Dabei gilt:

- Unvollständige Frames DÜRFEN nicht präsentiert werden.
- Veraltete Frames können kontrolliert ersetzt werden.
- Ausstehender Damage MUSS erhalten bleiben.
- Bei fehlendem Feedback MUSS ein sicherer Pacing-Modus verfügbar sein.
- Fehlerhafte Timing-Daten DÜRFEN keine dauerhafte Blockierung verursachen.

## 14. Verbindliche Anforderungen

1. NovaOS MUSS Frame Pacing als Bestandteil des Frame Schedulers bereitstellen.
2. Frame Pacing DARF keinen eigenständigen Aktualisierungsbedarf erzeugen.
3. Präsentationsziele MÜSSEN ausgabespezifisch verwaltet werden.
4. Rendering-Deadlines SOLLEN anhand geschätzter Verarbeitungskosten berechnet werden.
5. Eine monotone Zeitbasis MUSS verwendet werden.
6. Unnötig frühe Frame-Berechnungen SOLLEN vermieden werden.
7. Frame-Warteschlangen SOLLEN begrenzt werden.
8. Veraltete Frames DÜRFEN ersetzt werden, sofern erforderliche Änderungen erhalten bleiben.
9. VSync und VRR MÜSSEN integrierbar sein.
10. Presentation Feedback MUSS berücksichtigt werden können.
11. Adaptive Anpassungen SOLLEN stabilisiert werden.
12. Interaktive Aktualisierungen SOLLEN geringe Latenz erhalten.
13. Mehrere Bildschirmausgaben MÜSSEN unabhängig unterstützt werden.
14. Ohne Aktualisierungsbedarf DÜRFEN keine unnötigen Frames erzeugt werden.
15. Bei fehlerhaften Timing-Daten MUSS ein sicherer Fallback verfügbar sein.
16. Im deterministischen Modus MÜSSEN Pacing-Entscheidungen anhand kontrollierter Zeit- und Feedback-Ereignisse reproduzierbar sein.

## 15. Ergebnis

Nova Frame Pacing gewährleistet eine zeitlich koordinierte Verarbeitung und Präsentation tatsächlich benötigter Frames.

Durch adaptive Deadlines, begrenzte Warteschlangen, VSync-/VRR-Integration und Presentation Feedback werden unregelmäßige Frame-Abstände und unnötige Darstellungslatenzen reduziert.

Damit ermöglicht NovaOS eine gleichmäßige, reaktionsschnelle und energieeffiziente Bilddarstellung, ohne unveränderte Inhalte permanent neu berechnen zu müssen.