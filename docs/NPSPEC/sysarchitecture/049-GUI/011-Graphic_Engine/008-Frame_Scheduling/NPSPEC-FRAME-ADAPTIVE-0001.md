# NPSPEC-FRAME-ADAPTIVE-0001 – Nova Adaptive Frame Scheduling

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Frame Scheduler

## 1. Zweck

Nova Adaptive Frame Scheduling definiert die dynamische Anpassung der Frame-Planung an Aktualisierungsbedarf, Systemauslastung, Hardwarefähigkeiten und Präsentationsbedingungen.

Ziel ist eine möglichst geringe Darstellungslatenz bei gleichzeitig effizienter Nutzung von CPU, GPU, Speicherbandbreite und Energie.

Die Anpassung erfolgt automatisch, ohne die grafische Korrektheit oder Benutzerkontrolle zu beeinträchtigen.

## 2. Architekturprinzipien

- **Demand-Driven:** Frames werden ausschließlich bei tatsächlichem Bedarf erzeugt.
- **Load-Aware:** CPU- und GPU-Auslastung beeinflussen die Planung.
- **Deadline-Aware:** Präsentationsfristen werden berücksichtigt.
- **Latency-Aware:** Interaktive Aktualisierungen erhalten angemessene Priorität.
- **Feedback-Driven:** Tatsächliche Präsentationsdaten verbessern zukünftige Entscheidungen.
- **Power-Aware:** Unnötige Aktivität wird vermieden.
- **Adaptive:** Scheduling-Strategien können dynamisch wechseln.
- **Deterministic:** Entscheidungen beruhen auf definierten Regeln und Messwerten.

## 3. Adaptive Scheduling Model

Der Frame Scheduler bewertet mindestens folgende Informationen:

| Parameter | Bedeutung |
|---|---|
| Damage State | Vorliegender Aktualisierungsbedarf |
| Frame Priority | Dringlichkeit des Frames |
| CPU Load | Aktuelle Prozessorbelastung |
| GPU Load | Aktuelle Grafikprozessorbelastung |
| Render Cost | Geschätzter Rendering-Aufwand |
| Target Time | Angestrebter Präsentationszeitpunkt |
| Refresh Rate | Bildwiederholrate der Ausgabe |
| Presentation Feedback | Tatsächliche Präsentationsergebnisse |
| Power State | Aktueller Energiezustand |

Die Bewertung MUSS ausgabespezifisch erfolgen können.

## 4. Scheduling Strategies

NovaOS SOLL folgende Strategien unterstützen:

| Strategie | Verhalten |
|---|---|
| Immediate | Schnellstmögliche Verarbeitung zeitkritischer Änderungen |
| VSync-Aligned | Planung passend zum nächsten geeigneten VSync |
| Deadline-Based | Ausführung anhand berechneter Fertigstellungsfristen |
| Deferred | Verzögerung nicht dringender Aktualisierungen |
| Coalesced | Zusammenfassung mehrerer Aktualisierungen |
| Throttled | Begrenzung der Aktualisierungsfrequenz |
| Power-Saving | Reduzierte Aktivität bei geringem Aktualisierungsbedarf |
| Idle | Keine Frame-Erzeugung ohne Bedarf |

Die Strategie DARF während des Betriebs gewechselt werden.

Ein Strategiewechsel DARF keine erforderlichen Aktualisierungen verlieren.

## 5. Adaptive Deadline Calculation

NovaOS SOLL Rendering-Deadlines anhand erwarteter Verarbeitungskosten bestimmen.

Grundlage:

`Render Deadline = Target Presentation Time - Estimated Processing Time - Safety Margin`

Dabei gilt:

- Die geschätzte Verarbeitungszeit berücksichtigt erforderliche Rendering- und Kompositionsarbeiten.
- Synchronisations- und Übergabekosten SOLLEN einbezogen werden.
- Der Sicherheitsabstand DARF dynamisch angepasst werden.
- Tatsächliche Laufzeiten SOLLEN zur Verbesserung der Schätzungen verwendet werden.

Eine Deadline ist ein Planungsziel und keine Garantie für die tatsächliche Präsentation.

## 6. Load-Aware Scheduling

Bei hoher CPU- oder GPU-Auslastung SOLL NovaOS die Frame-Planung anpassen.

Mögliche Maßnahmen:

- Zusammenfassung mehrerer Frame-Anforderungen.
- Verschiebung nicht dringender Aktualisierungen.
- Vermeidung überflüssiger Zwischenframes.
- Anpassung der Rendering-Deadlines.
- Priorisierung interaktiver Änderungen.

Eine hohe Systemauslastung DARF nicht automatisch zum Verlust notwendiger grafischer Zustandsänderungen führen.

## 7. Latency Optimization

Interaktive Aktualisierungen SOLLEN bevorzugt verarbeitet werden.

Dazu gehören insbesondere:

- Maus- und Touch-Interaktionen.
- Fensterbewegungen.
- Größenänderungen.
- Texteingaben.
- Direkte UI-Rückmeldungen.

NovaOS SOLL unnötige Warteschlangen und zusätzliche Buffer-Verzögerungen vermeiden.

Eine bevorzugte Verarbeitung DARF andere Ausgaben oder Anwendungen nicht dauerhaft blockieren.

## 8. Adaptive Frame Rate

NovaOS DARF die tatsächliche Frame-Erzeugungsrate dynamisch verändern.

Dabei gilt:

- Statische Inhalte benötigen keine neuen Frames.
- Animationen können eigene Aktualisierungsintervalle besitzen.
- Nicht sichtbare Inhalte SOLLEN reduziert aktualisiert werden.
- Zeitkritische Inhalte SOLLEN möglichst rechtzeitig präsentiert werden.
- Die Aktualisierungsrate DARF unterhalb der physischen Display-Refresh-Rate liegen.

Die physische Bildwiederholrate und die tatsächliche Frame-Erzeugungsrate sind unabhängig voneinander zu betrachten.

## 9. Presentation Feedback Integration

Nova Adaptive Frame Scheduling MUSS Nova Presentation Feedback auswerten können.

Relevante Informationen:

- Tatsächliche Präsentationsintervalle.
- Verpasste Deadlines.
- Ersetzte Frames.
- Rendering- und Präsentationslatenzen.
- Veränderungen der Bildwiederholrate.

Die gewonnenen Informationen SOLLEN zur Anpassung zukünftiger Scheduling-Entscheidungen verwendet werden.

Geschätzte und hardwarebestätigte Messwerte MÜSSEN unterscheidbar bleiben.

## 10. Stability und Hysteresis

NovaOS SOLL häufige unnötige Strategiewechsel vermeiden.

Dazu dienen:

- Definierte Umschaltschwellen.
- Hysteresis.
- Geglättete Performance-Messwerte.
- Mindesthaltezeiten für geeignete Strategien.

Die Stabilisierung DARF notwendige unmittelbare Reaktionen auf kritische Ereignisse nicht verhindern.

## 11. Multi-Display Scheduling

Jede Bildschirmausgabe MUSS unabhängig optimiert werden können.

Dabei gilt:

- Unterschiedliche Refresh Rates werden berücksichtigt.
- Jede Ausgabe besitzt eigene Deadlines.
- VRR-fähige Ausgaben können angepasste Präsentationszeitpunkte verwenden.
- Gemeinsame Rendering-Ressourcen werden koordiniert.
- Langsame Ausgaben SOLLEN schnellere Ausgaben nicht unnötig blockieren.

## 12. Fehlerbehandlung

Bei unzuverlässigen Messwerten oder fehlendem Presentation Feedback MUSS NovaOS auf eine sichere Scheduling-Strategie zurückfallen können.

Dabei gilt:

- Ausstehender Damage bleibt erhalten.
- Fehlerhafte Kostenschätzungen dürfen keine dauerhafte Blockierung verursachen.
- Nicht präsentierte Frames müssen korrekt behandelt werden.
- Der zuletzt gültige Ausgabezustand SOLL erhalten bleiben.

## 13. Verbindliche Anforderungen

1. NovaOS MUSS adaptive Frame-Planung unterstützen.
2. Scheduling-Entscheidungen MÜSSEN auf tatsächlichem Aktualisierungsbedarf beruhen.
3. CPU- und GPU-Auslastung SOLLEN berücksichtigt werden.
4. Rendering-Kosten SOLLEN geschätzt werden können.
5. Frame-Deadlines SOLLEN dynamisch berechnet werden.
6. Interaktive Aktualisierungen SOLLEN bevorzugt werden.
7. Nicht dringende Aktualisierungen DÜRFEN zusammengefasst werden.
8. Unnötige Zwischenframes SOLLEN vermieden werden.
9. Presentation Feedback MUSS integrierbar sein.
10. Adaptive Strategiewechsel SOLLEN durch Hysteresis stabilisiert werden.
11. Mehrere Bildschirmausgaben MÜSSEN unabhängig planbar sein.
12. Adaptive Entscheidungen DÜRFEN erforderlichen Damage nicht verlieren.
13. Ohne Aktualisierungsbedarf DÜRFEN keine zusätzlichen Frames erzwungen werden.
14. Bei fehlenden Messwerten MUSS ein sicherer Fallback verfügbar sein.
15. Im deterministischen Modus MÜSSEN Scheduling-Entscheidungen bei identischen Eingaben reproduzierbar sein.

## 14. Ergebnis

Nova Adaptive Frame Scheduling ermöglicht eine dynamische, last- und latenzabhängige Steuerung der Frame-Erzeugung.

Durch adaptive Deadlines, Presentation Feedback, Priorisierung und stabile Strategiewechsel wird die Grafikpipeline an den tatsächlichen Bedarf angepasst.

Damit verbindet NovaOS eine reaktionsschnelle Benutzeroberfläche mit geringer CPU- und GPU-Auslastung sowie effizientem Energieverbrauch.