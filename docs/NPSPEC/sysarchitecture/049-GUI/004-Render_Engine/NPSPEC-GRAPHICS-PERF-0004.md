# NPSPEC-GRAPHICS-PERF-0004 – Nova Frame Pacing

**Status:** Angenommen  
**Bereich:** Graphics / Performance / Frame Pacing  
**System:** NovaOS

## 1. Zweck

Nova Frame Pacing definiert die zeitliche Steuerung der Bildberechnung und Bildausgabe innerhalb von NovaOS.

Ziel ist eine gleichmäßige, reaktionsschnelle und ressourcenschonende Darstellung ohne unnötiges Ruckeln, Tearing oder erhöhte Eingabelatenz.

Die Architektur berücksichtigt unterschiedliche Bildwiederholraten, variable Rendering-Zeiten und die Priorität interaktiver Benutzeroberflächen.

## 2. Architektur

Das Subsystem besteht aus folgenden Komponenten:

| Komponente | Aufgabe |
|---|---|
| Frame Pacing Controller | Zentrale Steuerung der Bildfrequenz |
| Frame Scheduler | Zeitliche Planung der Frame-Verarbeitung |
| Presentation Coordinator | Abstimmung mit der Bildausgabe |
| Frame Timing Monitor | Erfassung relevanter Zeitpunkte |
| Latency Controller | Begrenzung der Eingabe- und Ausgabelatenz |
| Frame Budget Manager | Verwaltung verfügbarer Frame-Zeitbudgets |

Die Komponenten arbeiten mit dem Nova Compositor, dem Display-System und Nova GPU Command Submission zusammen.

Der Compositor bleibt für die Zusammenstellung der sichtbaren Oberflächen verantwortlich.

## 3. Frame-Lebenszyklus

Ein Frame durchläuft folgende Phasen:

1. **Input:** Verarbeitung relevanter Eingabeereignisse.
2. **Update:** Aktualisierung des Darstellungszustands.
3. **Render:** Berechnung der benötigten Bildinhalte.
4. **Compose:** Zusammenstellung der Oberflächen.
5. **Present:** Übergabe des fertigen Bildes an die Bildausgabe.
6. **Feedback:** Erfassung der tatsächlichen Präsentationszeit.

Nicht jeder Frame muss sämtliche Phasen erneut durchlaufen.

Unveränderte Bildinhalte SOLLEN wiederverwendet werden.

## 4. Frame Timing

Das System verwendet eine monotone Zeitquelle aus Nova Time.

Erfasst werden mindestens:

- Geplanter Präsentationszeitpunkt.
- Beginn und Ende der Frame-Verarbeitung.
- CPU-Rendering-Zeit.
- GPU-Verarbeitungszeit, sofern verfügbar.
- Zeitpunkt der Bildübergabe.
- Tatsächlicher Präsentationszeitpunkt, sofern verfügbar.

Zeitmessungen MÜSSEN zwischen unterschiedlichen Zeitdomänen korrekt zugeordnet werden.

Die Frame-Steuerung DARF nicht von der Systemuhr für Datum und Uhrzeit abhängen.

## 5. Bildwiederholrate

NovaOS unterstützt:

- Feste Bildwiederholraten.
- Variable Refresh Rate (VRR), sofern verfügbar.
- Unterschiedliche Bildwiederholraten mehrerer Displays.
- Dynamische Anpassung der Rendering-Frequenz.
- Bedarfsgesteuerte Frame-Erzeugung.

Die tatsächlichen Ausgabefähigkeiten werden über das Display-System ermittelt.

Eine Anwendung DARF eine bevorzugte Bildfrequenz angeben.

Die endgültige Präsentationsplanung erfolgt durch das zuständige Display- beziehungsweise Compositor-Subsystem.

## 6. Synchronisation

Das System unterstützt folgende Präsentationsstrategien:

| Strategie | Verhalten |
|---|---|
| VSync | Synchronisation mit dem Display-Refresh |
| Adaptive Sync | Anpassung an variable Bildwiederholraten |
| Immediate | Präsentation ohne Refresh-Synchronisation, sofern zulässig |
| Mailbox | Bevorzugung des neuesten vollständig gerenderten Frames |
| FIFO | Präsentation in definierter Reihenfolge |

Die verfügbaren Strategien werden über Backend-Capabilities ermittelt.

NovaOS MUSS mindestens eine synchronisierte Präsentationsstrategie unterstützen.

Tearing SOLL bei der normalen Desktopdarstellung vermieden werden.

## 7. Frame Scheduling

Der Frame Scheduler plant die Verarbeitung anhand von:

- Erwartetem Präsentationszeitpunkt.
- Aktueller Rendering-Dauer.
- Verfügbarer CPU- und GPU-Leistung.
- Eingabeaktivität.
- Sichtbarkeit der Oberfläche.
- Display-Bildwiederholrate.
- Systemweiter Ressourcenbelastung.

Die Verarbeitung SOLL möglichst spät beginnen, solange der geplante Präsentationszeitpunkt zuverlässig eingehalten werden kann.

Dadurch werden unnötige Wartezeiten und Eingabelatenzen reduziert.

## 8. Latenzoptimierung

NovaOS priorisiert eine direkte Reaktion auf Benutzereingaben.

Festlegungen:

- Eingaben SOLLEN möglichst im nächsten geeigneten Frame berücksichtigt werden.
- Unnötig lange Frame-Warteschlangen SOLLEN vermieden werden.
- Veraltete, noch nicht präsentierte Frames DÜRFEN verworfen werden, sofern keine semantischen Abhängigkeiten verletzt werden.
- Die Anzahl gleichzeitig vorbereiteter Frames MUSS begrenzbar sein.
- Interaktive Oberflächen SOLLEN gegenüber nicht sichtbaren Hintergrundanimationen priorisiert werden.

Die Optimierung DARF keine Eingabeereignisse oder erforderlichen Zustandsänderungen verwerfen.

## 9. Frame Budget

Für jede Bildwiederholrate ergibt sich ein zeitliches Frame-Budget.

Beispiele:

| Bildwiederholrate | Frame-Budget |
|---|---|
| 30 Hz | 33,33 ms |
| 60 Hz | 16,67 ms |
| 90 Hz | 11,11 ms |
| 120 Hz | 8,33 ms |
| 144 Hz | 6,94 ms |

Das Budget umfasst die für eine rechtzeitige Präsentation relevanten Verarbeitungsschritte.

Der Frame Budget Manager überwacht dessen Einhaltung.

Bei drohender Überschreitung können optionale Rendering-Arbeiten reduziert oder verschoben werden.

## 10. Ressourcenökonomie

NovaOS erzeugt Frames bevorzugt bedarfsgesteuert.

Dabei gilt:

- Unveränderte Desktopbereiche SOLLEN nicht unnötig neu berechnet werden.
- Verdeckte Fenster SOLLEN ihre Rendering-Aktivität reduzieren.
- Hintergrundanimationen DÜRFEN mit geringerer Frequenz verarbeitet werden.
- Inaktive Oberflächen SOLLEN keine dauerhafte Frame-Erzeugung verursachen.
- Energiesparmodi DÜRFEN die bevorzugte Bildfrequenz reduzieren.

Die Steuerung MUSS die Anforderungen zeitkritischer Anwendungen berücksichtigen.

## 11. Überlastung und Frame Drops

Kann ein Frame nicht rechtzeitig fertiggestellt werden, reagiert das System kontrolliert.

Mögliche Maßnahmen:

- Wiederverwendung des zuletzt vollständig präsentierbaren Frames.
- Überspringen veralteter Präsentationskandidaten.
- Reduzierung optionaler Effekte.
- Anpassung der Rendering-Frequenz.
- Verschiebung nicht zeitkritischer Grafikoperationen.

Unvollständige Frames DÜRFEN nicht als vollständig gerendert präsentiert werden.

Das System SOLL wiederholte Frame-Time-Spitzen erkennen und deren Ursachen diagnostizierbar machen.

## 12. Multi-Display-Unterstützung

Jedes Display kann eine eigene Bildwiederholrate und Präsentationszeitbasis besitzen.

Der Frame Pacing Controller MUSS:

- Unterschiedliche Refresh-Raten berücksichtigen.
- Präsentationszeitpunkte pro Display verwalten.
- Unnötige Synchronisationsabhängigkeiten zwischen Displays vermeiden.
- Gemeinsame Oberflächen korrekt koordinieren.

Ein langsameres Display DARF die Bildausgabe eines schnelleren Displays nicht grundsätzlich begrenzen.

## 13. Integration

- **Nova Time:** Monotone Zeitmessung.
- **Nova Compositor:** Frame-Zusammenstellung und Präsentationsplanung.
- **Nova Display System:** Refresh-Raten, VSync und VRR.
- **Nova Graphics Backend Abstraction:** Backend-spezifische Präsentationsfunktionen.
- **Nova GPU Command Submission:** GPU-Ausführung und Synchronisation.
- **Nova Graphics Caching:** Wiederverwendung unveränderter Inhalte.
- **Nova Render Batching:** Reduzierung der Rendering-Kosten.
- **Nova Graphics Memory Optimization:** Ressourcenökonomische Frame-Verarbeitung.
- **Nova UI:** Bedarfsgesteuerte Aktualisierung interaktiver Oberflächen.

## 14. Normative Anforderungen

- **GRAPHICS-001:** NovaOS MUSS eine zentrale Frame-Pacing-Steuerung bereitstellen.
- **GRAPHICS-002:** Frame-Timing MUSS auf einer monotonen Zeitquelle basieren.
- **GRAPHICS-003:** Geplante und tatsächliche Präsentationszeitpunkte MÜSSEN unterscheidbar sein.
- **GRAPHICS-004:** Synchronisierte Bildausgabe MUSS unterstützt werden.
- **GRAPHICS-005:** Variable Refresh Rate SOLL bei geeigneter Hardware unterstützt werden.
- **GRAPHICS-006:** Unterschiedliche Display-Bildwiederholraten MÜSSEN unabhängig verwaltet werden können.
- **GRAPHICS-007:** Die Anzahl gleichzeitig vorbereiteter Frames MUSS begrenzbar sein.
- **GRAPHICS-008:** Interaktive Eingaben SOLLEN mit möglichst geringer Latenz dargestellt werden.
- **GRAPHICS-009:** Unveränderte Inhalte SOLLEN ohne unnötige Neuberechnung wiederverwendet werden.
- **GRAPHICS-010:** Nicht sichtbare Oberflächen SOLLEN ihre Rendering-Aktivität reduzieren.
- **GRAPHICS-011:** Frame-Budget-Überschreitungen MÜSSEN erkennbar sein.
- **GRAPHICS-012:** Unvollständige Frames DÜRFEN nicht als gültige fertige Frames präsentiert werden.
- **GRAPHICS-013:** Präsentationsstrategien MÜSSEN anhand der Backend-Capabilities ausgewählt werden.
- **GRAPHICS-014:** Frame-Timing und Latenz SOLLEN diagnostizierbar sein.
- **GRAPHICS-015:** Die grundlegende Frame-Steuerung MUSS auch ohne GPU-Beschleunigung funktionieren.
- **GRAPHICS-016:** Die Frame-Steuerung MUSS ohne Netzwerk- oder KI-Abhängigkeit funktionieren.

## 15. Ergebnis

NovaOS erhält eine zentrale Architektur zur gleichmäßigen und effizienten Bildausgabe.

Frame Scheduling, Präsentationssynchronisation und Latenzoptimierung ermöglichen eine flüssige Desktopdarstellung bei kontrolliertem Ressourcenverbrauch.

Die Architektur unterstützt feste und variable Bildwiederholraten sowie unterschiedliche Display-Konfigurationen.

**Grundsatz:** NovaOS berechnet und präsentiert Bilder dann, wenn sie benötigt werden – rechtzeitig, gleichmäßig und mit möglichst geringer Verzögerung.