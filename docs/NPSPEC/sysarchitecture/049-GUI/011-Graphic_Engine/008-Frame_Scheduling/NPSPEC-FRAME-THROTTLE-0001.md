# NPSPEC-FRAME-THROTTLE-0001 – Nova Frame Rate Throttling

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Frame Scheduler

## 1. Zweck

Nova Frame Rate Throttling definiert die kontrollierte Begrenzung der Frame-Erzeugungsrate innerhalb der NovaOS-Grafikpipeline.

Ziel ist die Reduzierung unnötiger CPU- und GPU-Auslastung, des Speicherbandbreitenbedarfs und des Energieverbrauchs, ohne die grafische Korrektheit oder notwendige Benutzerinteraktionen zu beeinträchtigen.

Frame Rate Throttling begrenzt ausschließlich die Verarbeitung erforderlicher Frames und erzeugt selbst keinen Aktualisierungsbedarf.

## 2. Architekturprinzipien

- **Demand-Driven:** Frames entstehen ausschließlich bei tatsächlichem Aktualisierungsbedarf.
- **Rate-Limited:** Die Frame-Erzeugungsrate kann gezielt begrenzt werden.
- **Priority-Aware:** Zeitkritische Aktualisierungen werden bevorzugt.
- **Visibility-Aware:** Sichtbarkeit beeinflusst die Aktualisierungsfrequenz.
- **Power-Aware:** Unnötige CPU- und GPU-Aktivität wird reduziert.
- **Adaptive:** Begrenzungen können dynamisch angepasst werden.
- **Multi-Display:** Unabhängige Steuerung je Bildschirmausgabe.
- **Deterministic:** Reproduzierbare Entscheidungen anhand definierter Eingaben.

## 3. Throttling Model

NovaOS verwaltet Throttling-Zustände für Ausgaben, Surfaces und geeignete grafische Aktivitäten.

| Eigenschaft | Beschreibung |
|---|---|
| Target ID | Betroffene Ausgabe, Surface oder Aktivität |
| Maximum Frame Rate | Zulässige maximale Frame-Frequenz |
| Minimum Interval | Mindestabstand zwischen Aktualisierungen |
| Priority | Priorität der Aktualisierung |
| Visibility State | Sichtbarkeitszustand |
| Last Update | Letzter relevanter Aktualisierungszeitpunkt |
| Pending Damage | Ausstehende grafische Änderungen |
| Throttling Mode | Aktuelle Begrenzungsstrategie |

Throttling-Zustände MÜSSEN unabhängig vom eigentlichen grafischen Zustand verwaltet werden.

## 4. Throttling Strategies

NovaOS SOLL folgende Strategien unterstützen:

| Strategie | Verhalten |
|---|---|
| Unlimited | Keine zusätzliche softwareseitige Begrenzung |
| Fixed Limit | Begrenzung auf eine definierte maximale Frame Rate |
| Adaptive Limit | Dynamische Begrenzung anhand aktueller Bedingungen |
| Visibility-Based | Reduzierung bei eingeschränkter Sichtbarkeit |
| Background | Begrenzung nicht fokussierter Aktivitäten |
| Power-Saving | Reduzierte Aktualisierungsfrequenz |
| Suspended | Keine Frame-Erzeugung bis zum nächsten relevanten Ereignis |

Die Strategie DARF während des Betriebs gewechselt werden.

## 5. Frame Rate Limiting

NovaOS SOLL die maximale Frame-Erzeugungsrate über ein Mindestintervall begrenzen.

Grundlage:

`Minimum Interval = 1 / Maximum Frame Rate`

Dabei gilt:

1. Neue Frame-Anforderungen werden auf ihren frühesten zulässigen Ausführungszeitpunkt geprüft.
2. Mehrere Änderungen innerhalb des Intervalls DÜRFEN zusammengefasst werden.
3. Ausstehender Damage MUSS erhalten bleiben.
4. Zeitkritische Aktualisierungen DÜRFEN entsprechend ihrer Priorität bevorzugt werden.
5. Eine Begrenzung DARF keine unnötigen Zwischenframes erzeugen.

Das Mindestintervall beschreibt eine Obergrenze der Aktualisierungsfrequenz, keine garantierte regelmäßige Frame-Erzeugung.

## 6. Visibility-Aware Throttling

NovaOS SOLL die Sichtbarkeit grafischer Inhalte berücksichtigen.

Dabei gilt:

- Vollständig sichtbare Inhalte erhalten die erforderliche Aktualisierungsfrequenz.
- Teilweise verdeckte Inhalte können reduziert aktualisiert werden.
- Vollständig verdeckte Inhalte können ihre Frame-Erzeugung aussetzen.
- Hintergrundaktivitäten können begrenzt werden.
- Beim erneuten Sichtbarwerden MUSS der aktuelle gültige Zustand dargestellt werden.

Die Reduzierung grafischer Aktualisierungen DARF nicht automatisch die Ausführung der gesamten Anwendung anhalten.

## 7. Adaptive Throttling

NovaOS SOLL Frame-Limits dynamisch anpassen können.

Berücksichtigt werden:

- CPU- und GPU-Auslastung
- Energiezustand
- Bildwiederholrate
- Sichtbarkeit
- Anwendungspriorität
- Rendering-Kosten
- Presentation Feedback

Häufige unnötige Änderungen SOLLEN durch Hysteresis vermieden werden.

Explizite Benutzereinstellungen MÜSSEN berücksichtigt werden.

## 8. Animation und Background Processing

Animationen MÜSSEN mit Frame Rate Throttling zusammenarbeiten können.

Dabei gilt:

- Sichtbare Animationen werden entsprechend ihrem Aktualisierungsbedarf geplant.
- Hintergrundanimationen DÜRFEN reduziert werden.
- Vollständig verdeckte Animationen DÜRFEN ihre grafische Aktualisierung aussetzen.
- Zeitabhängige Zustände MÜSSEN beim Fortsetzen korrekt berechnet werden.

Ausgesetzte Zwischenframes DÜRFEN nicht zu einer fehlerhaften Animation oder einem inkonsistenten Scene State führen.

## 9. Frame Scheduling Integration

Nova Frame Rate Throttling MUSS mit folgenden Komponenten zusammenarbeiten:

- **Nova Event-Driven Frame Scheduling:** Erkennung erforderlicher Frames.
- **Nova Damage-Driven Frame Generation:** Verwaltung betroffener Bildbereiche.
- **Nova Adaptive Frame Scheduling:** Dynamische Scheduling-Strategien.
- **Nova Frame Pacing:** Zeitliche Verteilung der Frames.
- **Nova Vertical Synchronization:** Abstimmung mit der Display-Hardware.
- **Nova Presentation Feedback:** Auswertung tatsächlicher Präsentationen.

Throttling bestimmt, wann eine erforderliche Aktualisierung frühestens verarbeitet werden darf.

Frame Pacing bestimmt den geeigneten tatsächlichen Ausführungszeitpunkt.

## 10. Multi-Display Throttling

Jede Bildschirmausgabe MUSS unabhängig begrenzt werden können.

Dabei gilt:

- Unterschiedliche Bildwiederholraten werden berücksichtigt.
- Ausgabespezifische Frame-Limits werden unterstützt.
- Gemeinsame Surfaces können unterschiedliche Präsentationsanforderungen besitzen.
- Langsame Ausgaben SOLLEN schnellere Ausgaben nicht unnötig begrenzen.

Eine globale Frame-Begrenzung DARF nur verwendet werden, wenn sie ausdrücklich erforderlich ist.

## 11. Idle und Power Management

Ohne Aktualisierungsbedarf MUSS die Frame-Erzeugung vollständig inaktiv bleiben können.

Dabei gilt:

- Keine periodischen Rendering-Aufträge.
- Keine künstlichen Frame-Anforderungen.
- Keine unnötigen GPU-Submissions.
- Keine erzwungene Aktualisierung statischer Inhalte.

Frame Rate Throttling DARF nicht durch einen permanenten globalen Rendering-Timer umgesetzt werden.

Zeitgesteuerte Aktualisierungen SOLLEN über geeignete Deadlines erfolgen.

## 12. Fehlerbehandlung

NovaOS MUSS fehlerhafte oder widersprüchliche Throttling-Konfigurationen sicher behandeln.

Dabei gilt:

- Ungültige Frame-Limits MÜSSEN zurückgewiesen werden.
- Ausstehender Damage DARF nicht verloren gehen.
- Überfällige Aktualisierungen MÜSSEN erneut eingeplant werden können.
- Fehlerhafte Zeitinformationen DÜRFEN keine dauerhafte Blockierung verursachen.
- Ein sicherer Scheduling-Fallback MUSS verfügbar sein.

## 13. Verbindliche Anforderungen

1. NovaOS MUSS Frame Rate Throttling unterstützen.
2. Throttling DARF keinen eigenständigen Aktualisierungsbedarf erzeugen.
3. Maximale Frame-Raten MÜSSEN konfigurierbar sein.
4. Begrenzungen SOLLEN für Ausgaben und grafische Aktivitäten unabhängig verwaltbar sein.
5. Mehrere Aktualisierungen DÜRFEN zusammengefasst werden.
6. Ausstehender Damage MUSS erhalten bleiben.
7. Sichtbarkeit SOLL bei der Begrenzung berücksichtigt werden.
8. Hintergrundanimationen SOLLEN reduziert werden können.
9. Zeitkritische Aktualisierungen SOLLEN bevorzugt werden.
10. Adaptive Begrenzungen SOLLEN unterstützt werden.
11. Benutzereinstellungen MÜSSEN berücksichtigt werden.
12. Frame Pacing und VSync MÜSSEN integrierbar sein.
13. Mehrere Bildschirmausgaben MÜSSEN unabhängig unterstützt werden.
14. Ohne Aktualisierungsbedarf DÜRFEN keine zusätzlichen Frames erzeugt werden.
15. Fehlerhafte Begrenzungen DÜRFEN keine dauerhafte Aktualisierungssperre verursachen.
16. Im deterministischen Modus MÜSSEN Throttling-Entscheidungen anhand kontrollierter Zeit- und Zustandsinformationen reproduzierbar sein.

## 14. Ergebnis

Nova Frame Rate Throttling ermöglicht die gezielte Begrenzung grafischer Aktualisierungsfrequenzen.

Durch adaptive Frame-Limits, Sichtbarkeitsprüfung, Priorisierung und Damage-Erhaltung werden unnötige Rendering-Operationen reduziert.

Damit verbessert NovaOS die Energieeffizienz und Ressourcennutzung, ohne die korrekte Darstellung oder notwendige Benutzerinteraktionen zu beeinträchtigen.