# NPSPEC-FRAME-LATENCY-0001 – Nova Frame Latency Management

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Frame Scheduler

## 1. Zweck

Nova Frame Latency Management definiert die Erfassung, Steuerung und Optimierung der Verzögerungen innerhalb der NovaOS-Grafikpipeline.

Ziel ist eine möglichst geringe Zeit zwischen einer relevanten Zustandsänderung und ihrer tatsächlichen Darstellung auf dem Bildschirm.

Die Latenzoptimierung MUSS mit Frame Scheduling, Rendering, Komposition und Präsentation zusammenarbeiten, ohne unnötige Frames zu erzeugen.

## 2. Architekturprinzipien

- **Latency-Aware:** Verzögerungen werden über die gesamte Grafikpipeline berücksichtigt.
- **Event-Driven:** Verarbeitung erfolgt ausschließlich bei tatsächlichem Aktualisierungsbedarf.
- **Deadline-Aware:** Frames werden passend zu Präsentationszeitpunkten geplant.
- **Queue-Aware:** Unnötige Warteschlangen werden vermieden.
- **Feedback-Driven:** Tatsächliche Laufzeiten verbessern zukünftige Entscheidungen.
- **Adaptive:** Anpassung an Systemlast und Hardwarefähigkeiten.
- **Priority-Aware:** Interaktive Aktualisierungen werden bevorzugt.
- **Deterministic:** Entscheidungen bleiben anhand definierter Eingaben reproduzierbar.

## 3. Frame Latency Model

NovaOS unterscheidet folgende Latenzkomponenten:

| Komponente | Beschreibung |
|---|---|
| Event Latency | Ereignis bis zur Verarbeitung |
| Scheduling Latency | Verarbeitung bis zum Rendering-Beginn |
| Rendering Latency | Dauer der grafischen Berechnung |
| Composition Latency | Dauer der Frame-Komposition |
| Queue Latency | Wartezeit vor der Präsentation |
| Presentation Latency | Übergabe bis zur bestätigten Präsentation |

Die gesamte Frame-Latenz ergibt sich aus der zeitlichen Differenz zwischen dem relevanten Ausgangsereignis und der bestätigten Präsentation.

Überlappende Verarbeitungsschritte DÜRFEN bei der Berechnung nicht doppelt gezählt werden.

## 4. Latency Tracking

NovaOS SOLL für jeden relevanten Frame folgende Zeitpunkte erfassen können:

- **Event Timestamp:** Zeitpunkt des auslösenden Ereignisses.
- **Schedule Timestamp:** Zeitpunkt der Frame-Planung.
- **Render Start:** Beginn des Renderings.
- **Render Complete:** Abschluss des Renderings.
- **Composition Complete:** Abschluss der Komposition.
- **Presentation Submit:** Übergabe an den Display Server.
- **Presentation Complete:** Bestätigter Präsentationszeitpunkt.

Zeitstempel MÜSSEN einer definierten monotonen Zeitbasis zugeordnet sein.

Geschätzte und tatsächlich gemessene Werte MÜSSEN unterscheidbar bleiben.

## 5. Latency Optimization

NovaOS SOLL unnötige Verzögerungen innerhalb der Grafikpipeline reduzieren.

Dazu gehören:

1. Frühzeitige Erkennung relevanter Zustandsänderungen.
2. Vermeidung unnötiger Scheduling-Verzögerungen.
3. Priorisierung zeitkritischer Rendering-Aufträge.
4. Wiederverwendung unveränderter Render-Ergebnisse.
5. Reduzierung überflüssiger Frame-Warteschlangen.
6. Rechtzeitige Übergabe fertiger Frames an den Display Server.

Die Optimierung DARF keine erforderlichen Synchronisationsschritte umgehen.

## 6. Interactive Latency

Interaktive Aktualisierungen SOLLEN bevorzugt behandelt werden.

Dazu gehören:

- Maus- und Touch-Eingaben
- Tastatureingaben
- Fensterbewegungen
- Größenänderungen
- Direkte UI-Rückmeldungen
- Interaktive Animationen

NovaOS DARF veraltete Zwischenzustände überspringen, sofern der aktuelle grafische Zustand korrekt dargestellt wird.

Eine bevorzugte Verarbeitung DARF andere Anwendungen nicht dauerhaft blockieren.

## 7. Frame Queue Management

Nova Frame Latency Management MUSS mit der Frame Queue des Frame Schedulers zusammenarbeiten.

Dabei gilt:

- Unnötige Warteschlangen SOLLEN vermieden werden.
- Die Anzahl ausstehender Frames SOLL begrenzt werden.
- Veraltete, noch nicht präsentierte Frames DÜRFEN ersetzt werden.
- Erforderliche Zustandsänderungen und Damage MÜSSEN erhalten bleiben.
- Bereits verwendete Buffer DÜRFEN nicht vorzeitig freigegeben werden.

Die Queue-Tiefe SOLL entsprechend den tatsächlichen Latenzanforderungen angepasst werden.

## 8. Adaptive Latency Management

NovaOS SOLL Latenzentscheidungen anhand aktueller Messwerte anpassen.

Relevante Faktoren:

- CPU- und GPU-Auslastung
- Rendering- und Kompositionsdauer
- Frame Queue Depth
- Präsentationsintervalle
- Verpasste Deadlines
- VSync- und VRR-Fähigkeiten
- Eingabepriorität

Häufige unnötige Strategiewechsel SOLLEN durch Hysteresis vermieden werden.

## 9. Presentation Integration

Nova Frame Latency Management MUSS mit Nova Presentation Feedback zusammenarbeiten.

Dabei gilt:

- Tatsächliche Präsentationszeitpunkte werden berücksichtigt.
- Verpasste Präsentationsziele werden erkannt.
- VSync- und VRR-Verhalten wird einbezogen.
- Direct Scanout und Hardware Overlays können zur Latenzreduzierung genutzt werden.

Die bestätigte Frame-Präsentation ist nicht grundsätzlich mit dem physikalischen Sichtbarwerden aller Pixel gleichzusetzen.

Nicht messbare Display-Latenzen MÜSSEN als solche gekennzeichnet werden.

## 10. Multi-Display Latency

Jede Bildschirmausgabe MUSS unabhängig bewertet werden können.

Dabei gilt:

- Unterschiedliche Bildwiederholraten werden berücksichtigt.
- Jede Ausgabe besitzt eigene Latenzmesswerte.
- Gemeinsame Ressourcen werden koordiniert.
- Langsame Ausgaben SOLLEN andere Ausgaben nicht unnötig blockieren.

Eine gemeinsame Frame ID bedeutet nicht automatisch identische Präsentationslatenzen auf mehreren Bildschirmen.

## 11. Idle und Power Management

Latenzoptimierung DARF keine permanente Frame-Erzeugung verursachen.

Ohne Aktualisierungsbedarf:

- Werden keine neuen Frames benötigt.
- Bleiben Rendering und Komposition inaktiv.
- Werden keine künstlichen GPU-Submissions erzeugt.
- Bleibt der zuletzt gültige Frame sichtbar.

Bei neuen Ereignissen SOLL die Grafikpipeline ohne unnötige Verzögerung aktiviert werden.

## 12. Fehlerbehandlung

Bei fehlenden Messwerten, verpassten Deadlines oder fehlerhaften Zeitprognosen MUSS NovaOS einen sicheren Betriebszustand erhalten.

Dabei gilt:

- Ungültige Messwerte DÜRFEN nicht als bestätigte Werte verwendet werden.
- Unvollständige Frames DÜRFEN nicht präsentiert werden.
- Erforderlicher Damage MUSS erhalten bleiben.
- Veraltete Frames DÜRFEN kontrolliert ersetzt werden.
- Ein konservativer Scheduling-Modus MUSS verfügbar sein.

## 13. Verbindliche Anforderungen

1. NovaOS MUSS Frame-Latenzen erfassen und auswerten können.
2. Latenzkomponenten MÜSSEN eindeutig unterscheidbar sein.
3. Zeitmessungen MÜSSEN eine definierte monotone Zeitbasis verwenden.
4. Geschätzte und bestätigte Zeitpunkte MÜSSEN unterscheidbar bleiben.
5. Interaktive Aktualisierungen SOLLEN bevorzugt verarbeitet werden.
6. Unnötige Frame-Warteschlangen SOLLEN vermieden werden.
7. Veraltete Frames DÜRFEN ersetzt werden, sofern erforderliche Änderungen erhalten bleiben.
8. Rendering- und Präsentations-Deadlines SOLLEN berücksichtigt werden.
9. Presentation Feedback MUSS integrierbar sein.
10. VSync, VRR und Direct Scanout SOLLEN berücksichtigt werden.
11. Mehrere Bildschirmausgaben MÜSSEN unabhängig bewertet werden können.
12. Latenzoptimierungen DÜRFEN Synchronisation und grafische Korrektheit nicht beeinträchtigen.
13. Ohne Aktualisierungsbedarf DÜRFEN keine zusätzlichen Frames erzeugt werden.
14. Bei fehlenden Messwerten MUSS ein sicherer Fallback verfügbar sein.
15. Im deterministischen Modus MÜSSEN Latenzentscheidungen anhand kontrollierter Mess- und Zeitereignisse reproduzierbar sein.

## 14. Ergebnis

Nova Frame Latency Management ermöglicht die gezielte Reduzierung von Verzögerungen innerhalb der gesamten NovaOS-Grafikpipeline.

Durch präzise Zeitmessungen, adaptive Scheduling-Entscheidungen, begrenzte Warteschlangen und Presentation Feedback werden interaktive Inhalte möglichst schnell dargestellt.

Damit verbindet NovaOS eine reaktionsschnelle Benutzeroberfläche mit effizienter Ressourcennutzung und bedarfsgesteuerter Frame-Erzeugung.