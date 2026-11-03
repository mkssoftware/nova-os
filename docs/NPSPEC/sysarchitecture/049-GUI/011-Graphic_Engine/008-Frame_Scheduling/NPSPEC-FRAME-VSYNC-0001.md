# NPSPEC-FRAME-VSYNC-0001 – Nova Vertical Synchronization

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Frame Scheduler

## 1. Zweck

Nova Vertical Synchronization definiert die zeitliche Abstimmung der Frame-Präsentation mit der Bildwiederholrate der Display-Hardware.

Ziel ist die Vermeidung von Screen Tearing, die Reduzierung unnötiger Frame-Berechnungen und eine gleichmäßige Darstellung bei möglichst geringer Latenz.

VSync steuert die Präsentationszeitpunkte, nicht die grundsätzliche Erzeugung neuer Frames.

## 2. Architekturprinzipien

- **Presentation-Driven:** Synchronisation erfolgt anhand der Display-Ausgabe.
- **Event-Driven:** Keine permanente Frame-Erzeugung durch VSync.
- **Deadline-Aware:** Rendering wird rechtzeitig vor der Präsentation geplant.
- **Latency-Aware:** Unnötige Wartezeiten werden vermieden.
- **Adaptive:** Unterstützung unterschiedlicher Bildwiederholraten.
- **VRR-Aware:** Variable Refresh Rate wird berücksichtigt.
- **Multi-Display:** Unabhängige Synchronisation je Bildschirmausgabe.
- **Fallback-Safe:** Sichere Funktion ohne Hardware-VSync-Unterstützung.

## 3. VSync Model

Jede Bildschirmausgabe besitzt einen eigenen Synchronisationszustand.

| Eigenschaft | Beschreibung |
|---|---|
| Output ID | Zugehörige Bildschirmausgabe |
| Refresh Rate | Aktuelle Bildwiederholrate |
| Refresh Interval | Erwartetes Aktualisierungsintervall |
| VSync Counter | Zähler bestätigter Synchronisationsereignisse |
| Last VSync Time | Letzter bestätigter Synchronisationszeitpunkt |
| Next VSync Time | Prognostizierter nächster Zeitpunkt |
| VSync Source | Hardware- oder Software-Zeitquelle |
| VRR State | Status variabler Bildwiederholraten |

Bestätigte und prognostizierte Zeitpunkte MÜSSEN unterscheidbar sein.

## 4. VSync Event Processing

Der Display Server SOLL VSync- oder Page-Flip-Ereignisse des Display-Backends bereitstellen.

Der Frame Scheduler verwendet diese zur:

1. Bestimmung zukünftiger Präsentationszeitpunkte.
2. Berechnung geeigneter Rendering-Deadlines.
3. Synchronisation ausstehender Frames.
4. Aktualisierung der Frame-Pacing-Informationen.
5. Auswertung tatsächlicher Präsentationsintervalle.

Ein VSync-Ereignis DARF allein keinen neuen Rendering-Auftrag erzwingen.

## 5. Frame Synchronization

Bei aktiviertem VSync SOLL ein fertiger Frame zum nächsten geeigneten Präsentationszeitpunkt ausgegeben werden.

Dabei gilt:

- Nur vollständig berechnete und synchronisierte Buffer dürfen präsentiert werden.
- Die Präsentation MUSS die Fähigkeiten des Display-Backends berücksichtigen.
- Verpasste Präsentationszeitpunkte DÜRFEN nicht zu ungültigen Buffer-Wechseln führen.
- Bereits sichtbare Inhalte bleiben bis zur gültigen nächsten Präsentation erhalten.

Die tatsächliche Präsentation erfolgt durch den Display Server.

## 6. Rendering Deadlines

NovaOS SOLL die Rendering-Deadline anhand des erwarteten Präsentationszeitpunkts berechnen.

Grundlage:

`Render Deadline = Target VSync - Estimated Render Time - Safety Margin`

Dabei gilt:

- Rendering- und Kompositionskosten werden berücksichtigt.
- GPU-Synchronisationszeiten SOLLEN einbezogen werden.
- Der Sicherheitsabstand kann adaptiv angepasst werden.
- Presentation Feedback verbessert die Zeitprognosen.

Eine prognostizierte Deadline stellt keine Garantie für rechtzeitige Präsentation dar.

## 7. VSync Modes

NovaOS SOLL folgende Synchronisationsverfahren unterstützen:

| Modus | Verhalten |
|---|---|
| VSync On | Synchronisierte Präsentation ohne absichtliches Tearing |
| VSync Off | Unmittelbare Präsentation, sofern unterstützt und erlaubt |
| Adaptive VSync | Dynamische Auswahl geeigneter Synchronisationsverfahren |
| VRR | Variable Präsentationsintervalle innerhalb der Hardwaregrenzen |

Die Verfügbarkeit hängt von Display-Backend, Hardware und Benutzerkonfiguration ab.

Die automatische Auswahl DARF ausdrückliche Benutzereinstellungen nicht überschreiben.

## 8. Variable Refresh Rate

NovaOS SOLL Variable Refresh Rate unterstützen, sofern die Hardware dies ermöglicht.

Dabei gilt:

- Präsentationszeitpunkte können innerhalb des unterstützten VRR-Bereichs variieren.
- Neue Frames werden weiterhin nur bei tatsächlichem Bedarf erzeugt.
- Minimale und maximale Aktualisierungsintervalle werden berücksichtigt.
- Erforderliche Refresh-Vorgänge zur Einhaltung von Hardwaregrenzen dürfen ohne erneutes Rendering erfolgen.

VRR DARF keine unnötige permanente Frame-Erzeugung verursachen.

## 9. Frame Pacing und Latenz

Nova Vertical Synchronization MUSS mit Nova Frame Pacing zusammenarbeiten.

Dabei gilt:

- Gleichmäßige Präsentationsintervalle SOLLEN bevorzugt werden.
- Unnötig früh berechnete Frames SOLLEN vermieden werden.
- Überflüssige Warteschlangen SOLLEN reduziert werden.
- Zeitkritische Aktualisierungen erhalten geeignete Priorität.

Die Optimierung der Latenz DARF die korrekte Buffer-Synchronisation nicht umgehen.

## 10. Multi-Display Synchronization

Jede Bildschirmausgabe MUSS unabhängig synchronisiert werden können.

Dabei gilt:

- Unterschiedliche Bildwiederholraten werden unterstützt.
- Jede Ausgabe besitzt eigene VSync-Zeitpunkte.
- Rendering-Deadlines werden ausgabespezifisch berechnet.
- Gemeinsame Buffer werden korrekt synchronisiert.
- Langsame Ausgaben SOLLEN schnellere Ausgaben nicht unnötig blockieren.

Eine bildschirmübergreifende Synchronisation DARF nur garantiert werden, wenn die Hardware dies unterstützt.

## 11. Idle und Power Management

VSync DARF nicht als permanenter Rendering-Timer verwendet werden.

Wenn keine sichtbaren Änderungen vorliegen:

- Werden keine neuen Frames benötigt.
- Bleiben bestehende Scanout-Buffer gültig.
- Können Rendering und Komposition inaktiv bleiben.
- Können CPU und GPU energiesparende Zustände nutzen.

Die Display-Hardware kann den vorhandenen Frame weiterhin mit ihrer regulären Bildwiederholrate ausgeben.

## 12. Fehlerbehandlung und Fallback

NovaOS MUSS fehlende oder fehlerhafte VSync-Informationen sicher behandeln.

Mögliche Situationen:

- Fehlende Hardware-VSync-Ereignisse
- Ungültige Zeitstempel
- Änderungen der Bildwiederholrate
- Fehlgeschlagene Page Flips
- Display-Rekonfiguration
- Geräteverlust

Bei fehlender Hardwareunterstützung DARF eine softwarebasierte Zeitprognose verwendet werden.

Geschätzte Zeitpunkte DÜRFEN nicht als hardwarebestätigte Präsentationen ausgegeben werden.

Ein sicherer Präsentationspfad MUSS erhalten bleiben.

## 13. Verbindliche Anforderungen

1. NovaOS MUSS VSync-basierte Präsentationsplanung unterstützen können.
2. VSync DARF keine permanente Frame-Erzeugung erzwingen.
3. Jede Ausgabe MUSS einen eigenen Synchronisationszustand besitzen.
4. Hardwarebestätigte und prognostizierte Zeitpunkte MÜSSEN unterscheidbar sein.
5. Rendering-Deadlines SOLLEN anhand erwarteter VSync-Zeitpunkte berechnet werden.
6. Buffer MÜSSEN vor der Präsentation vollständig synchronisiert sein.
7. Verpasste VSync-Zeitpunkte DÜRFEN keine ungültigen Präsentationen verursachen.
8. Unterschiedliche Bildwiederholraten MÜSSEN unterstützt werden.
9. VRR SOLL bei geeigneter Hardware unterstützt werden.
10. Frame Pacing und Presentation Feedback MÜSSEN integrierbar sein.
11. Unnötige Frame-Warteschlangen SOLLEN vermieden werden.
12. Mehrere Ausgaben MÜSSEN unabhängig synchronisiert werden können.
13. Ausdrückliche Benutzereinstellungen MÜSSEN berücksichtigt werden.
14. Bei fehlender Hardwareunterstützung MUSS ein sicherer Fallback möglich sein.
15. Im deterministischen Modus MÜSSEN Scheduling-Entscheidungen anhand kontrollierter Zeitereignisse reproduzierbar sein.

## 14. Ergebnis

Nova Vertical Synchronization verbindet die bedarfsgesteuerte Frame-Erzeugung mit den tatsächlichen Präsentationsmöglichkeiten der Display-Hardware.

Durch VSync-Ereignisse, adaptive Rendering-Deadlines, VRR-Unterstützung und Presentation Feedback werden gleichmäßige und möglichst latenzarme Bildaktualisierungen ermöglicht.

Dabei bleibt die zentrale NovaOS-Eigenschaft erhalten: **Ein unveränderter Bildschirm benötigt keine neuen Rendering-Frames.**