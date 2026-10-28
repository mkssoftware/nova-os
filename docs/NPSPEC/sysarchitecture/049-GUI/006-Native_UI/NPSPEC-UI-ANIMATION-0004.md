# NPSPEC-UI-ANIMATION-0004 – Nova Animation Scheduling

**Status:** Angenommen  
**Bereich:** UI / Animation / Scheduling  
**System:** NovaOS

## 1. Zweck

Nova Animation Scheduling definiert die zeitliche Planung, Synchronisation und Ausführung nativer UI-Animationen.

Ziel ist eine flüssige, deterministische und ressourcenschonende Darstellung, bei der ausschließlich tatsächlich benötigte Animationsaktualisierungen ausgeführt werden.

Das Scheduling arbeitet bedarfsgesteuert und berücksichtigt die Bildwiederholrate, Sichtbarkeit, Systemauslastung und Benutzerpräferenzen.

## 2. Architektur

Das Subsystem umfasst folgende Komponenten:

| Komponente | Aufgabe |
|---|---|
| Animation Scheduler | Planung aktiver Animationen |
| Animation Clock | Monotone Zeitbasis |
| Frame Scheduler | Planung erforderlicher Frames |
| Animation Evaluator | Berechnung aktueller Animationswerte |
| Visibility Tracker | Ermittlung sichtbarer Animationen |
| Frame Synchronizer | Abstimmung mit der Bildausgabe |
| Animation Budget Manager | Begrenzung des Ressourcenverbrauchs |

Die Animation Engine verwaltet die Animationen. Das Scheduling bestimmt, wann ihre Werte aktualisiert und dargestellt werden.

## 3. Bedarfsgesteuerte Ausführung

NovaOS verwendet kein dauerhaft laufendes Animations- oder Renderverfahren.

Dabei gilt:

- Ohne aktive Animationen werden keine Animationsframes angefordert.
- Animationen werden nur zu erforderlichen Zeitpunkten ausgewertet.
- Unveränderte Animationswerte lösen keine Renderinvalidierung aus.
- Abgeschlossene Animationen werden aus der aktiven Planung entfernt.
- Neue Animationen aktivieren das Scheduling automatisch.

Eine permanente Aktualisierung der gesamten Benutzeroberfläche ist nicht zulässig.

## 4. Zeitmodell

Nova Animation Scheduling verwendet eine monotone Zeitquelle.

Dabei gilt:

- Animationsfortschritt wird anhand der verstrichenen Zeit berechnet.
- Die Animationsgeschwindigkeit ist unabhängig von der tatsächlichen Framerate.
- Verzögerte Frames verändern nicht die definierte Animationsdauer.
- Pausierte Animationen behalten ihren zeitlichen Fortschritt.
- Im Deterministic Mode MUSS eine reproduzierbare Zeitsteuerung möglich sein.

Die Zeitquelle MUSS für Tests und Simulationen austauschbar sein.

## 5. Frame Scheduling

Frames werden nur angefordert, wenn eine sichtbare Aktualisierung erforderlich ist.

Dabei gilt:

- Mehrere Animationen SOLLEN innerhalb eines gemeinsamen Frames ausgewertet werden.
- Frameanforderungen SOLLEN zusammengefasst werden.
- Die Frameplanung SOLL mit der Bildwiederholrate des Ausgabegeräts synchronisiert werden.
- Nicht benötigte Zwischenframes DÜRFEN übersprungen werden.
- Die Berechnung MUSS den tatsächlichen Animationszeitpunkt berücksichtigen.

Das Scheduling DARF keine unnötigen Frames erzeugen, nur um eine feste Framerate aufrechtzuerhalten.

## 6. Synchronisation mit der Bildausgabe

Nova Animation Scheduling arbeitet mit Display Server und Compositor zusammen.

Dabei gilt:

- Verfügbare VSync- oder Presentation-Timing-Informationen SOLLEN verwendet werden.
- Animationswerte SOLLEN für den vorgesehenen Präsentationszeitpunkt berechnet werden.
- Mehrere Animationen eines Frames verwenden eine gemeinsame Zeitreferenz.
- Unterschiedliche Bildwiederholraten mehrerer Monitore MÜSSEN berücksichtigt werden.
- Fehlende Synchronisationsinformationen DÜRFEN die Animation Engine nicht blockieren.

Die Animation Engine bleibt unabhängig von konkreter Displayhardware.

## 7. Frame Drops

Kann ein Frame nicht rechtzeitig dargestellt werden, wird der Animationsfortschritt anhand der tatsächlichen Zeit neu berechnet.

Dabei gilt:

- Versäumte Frames MÜSSEN nicht nachträglich einzeln berechnet werden.
- Animationen DÜRFEN bei Verzögerungen nicht unkontrolliert beschleunigen.
- Die definierte Animationsdauer bleibt erhalten.
- Der letzte gültige Zustand bleibt bis zur nächsten Darstellung verfügbar.
- Abschlussereignisse MÜSSEN auch bei übersprungenen Frames korrekt ausgelöst werden.

Zeitkritische Animationen SOLLEN gegenüber rein dekorativen Animationen priorisiert werden.

## 8. Sichtbarkeitsabhängiges Scheduling

Die Sichtbarkeit eines UI-Elements beeinflusst die erforderliche Animationsaktualisierung.

Dabei gilt:

- Vollständig verdeckte Animationen SOLLEN keine unnötigen Renderframes erzeugen.
- Nicht sichtbare Animationen DÜRFEN pausiert oder zeitlich weitergeführt werden.
- Das jeweilige Verhalten MUSS eindeutig definiert sein.
- Beim erneuten Sichtbarwerden MUSS der korrekte Animationszustand hergestellt werden.
- Animationen mit funktionaler Bedeutung DÜRFEN nicht allein aufgrund fehlender Sichtbarkeit ihre erforderliche Logik verlieren.

Sichtbarkeit wird unter Berücksichtigung der verfügbaren Informationen des Compositors bestimmt.

## 9. Prioritäten und Ressourcenbudget

NovaOS kann Animationen nach ihrer Bedeutung priorisieren.

| Priorität | Beispiele |
|---|---|
| Hoch | Direkte Benutzerinteraktion, Drag & Drop |
| Normal | Fensterübergänge, Control States |
| Niedrig | Dekorative Effekte, Hintergrundanimationen |

Dabei gilt:

- Prioritäten MÜSSEN deterministisch ausgewertet werden.
- Dekorative Animationen DÜRFEN bei Ressourcenmangel reduziert werden.
- Direkte Benutzerinteraktionen SOLLEN bevorzugt behandelt werden.
- Das Scheduling DARF die Reaktionsfähigkeit der UI Event Loop nicht beeinträchtigen.
- Ressourcenbegrenzungen MÜSSEN kontrolliert und nachvollziehbar wirken.

## 10. UI Thread und Compositor

Nova Animation Scheduling unterscheidet zwischen UI-seitigen und compositorseitigen Animationen.

**UI-seitige Animationen:**
- Werden über den zuständigen UI Dispatcher koordiniert.
- Können Property-, Layout- und Renderinvalidierungen auslösen.
- Unterliegen dem Nova UI Threading Model.

**Compositorseitige Animationen:**
- Können unabhängig von der UI Event Loop ausgewertet werden.
- Verwenden geeignete Transformations- und Effektparameter.
- Benötigen nicht für jeden Frame eine erneute Layoutberechnung.

Der Übergang zwischen beiden Ausführungswegen MUSS synchronisiert erfolgen.

## 11. Damage Tracking

Animation Scheduling arbeitet mit dem Damage Tracking des Compositors zusammen.

Dabei gilt:

- Nur tatsächlich veränderte Bereiche werden invalidiert.
- Mehrere Invalidierungen innerhalb eines Frames SOLLEN zusammengefasst werden.
- Transformationsanimationen berücksichtigen vorherige und neue sichtbare Bereiche.
- Unveränderte Oberflächen behalten ihre gültigen Renderinformationen.
- Vollständige Neuzeichnungen erfolgen ausschließlich bei technischer Notwendigkeit.

## 12. Animation Lifecycle

Das Scheduling berücksichtigt folgende Animationszustände:

- Created.
- Running.
- Paused.
- Completed.
- Cancelled.

Dabei gilt:

- Nur aktive Animationen werden regelmäßig eingeplant.
- Pausierte Animationen benötigen keine fortlaufende Auswertung.
- Abgeschlossene und abgebrochene Animationen werden aus der Planung entfernt.
- Ressourcen und Zeitgeber MÜSSEN kontrolliert freigegeben werden.
- Änderungen des UI Component Lifecycle MÜSSEN berücksichtigt werden.

## 13. Barrierefreiheit und Energieeffizienz

Nova Animation Scheduling berücksichtigt Benutzer- und Systemeinstellungen.

Dabei gilt:

- Reduzierte Animationen MÜSSEN unterstützt werden.
- Deaktivierte Animationen DÜRFEN keine Animationsframes erzeugen.
- Im Energiesparmodus DÜRFEN dekorative Animationen reduziert werden.
- Bei hoher Systemauslastung DÜRFEN nicht notwendige Aktualisierungen begrenzt werden.
- Funktionale Zustandsänderungen MÜSSEN unabhängig von der Animation wirksam bleiben.

Die Bedienbarkeit besitzt Vorrang vor visuellen Effekten.

## 14. NovaStudio und Diagnose

NovaStudio unterstützt die Analyse des Animation Scheduling.

Dazu gehören:

- Anzeige aktiver Animationen.
- Darstellung geplanter und tatsächlich präsentierter Frames.
- Erkennung übersprungener Frames.
- Analyse von Animationslatenzen.
- Darstellung von UI- und Compositor-Ausführung.
- Erkennung unnötiger Invalidierungen.
- Simulation unterschiedlicher Bildwiederholraten.

Diagnosefunktionen DÜRFEN im regulären Betrieb keine dauerhafte zusätzliche Frameverarbeitung erzwingen.

## 15. Normative Anforderungen

- **UI-001:** NovaOS MUSS bedarfsgesteuertes Animation Scheduling bereitstellen.
- **UI-002:** Ohne aktive Animationen DÜRFEN keine unnötigen Animationsframes erzeugt werden.
- **UI-003:** Das Scheduling MUSS eine monotone Zeitquelle verwenden.
- **UI-004:** Animationsfortschritt MUSS unabhängig von der Framerate berechnet werden.
- **UI-005:** Mehrere Animationen SOLLEN innerhalb gemeinsamer Frames verarbeitet werden.
- **UI-006:** Die Frameplanung SOLL mit der Bildausgabe synchronisiert werden.
- **UI-007:** Unterschiedliche Monitor-Bildwiederholraten MÜSSEN berücksichtigt werden.
- **UI-008:** Versäumte Frames DÜRFEN ohne nachträgliche Einzelberechnung übersprungen werden.
- **UI-009:** Sichtbarkeit MUSS bei der Frameplanung berücksichtigt werden.
- **UI-010:** Animationsprioritäten MÜSSEN unterstützt werden können.
- **UI-011:** UI- und compositorseitige Animationen MÜSSEN koordiniert werden.
- **UI-012:** Animationen MÜSSEN gezielte Renderinvalidierungen verwenden.
- **UI-013:** Pausierte und abgeschlossene Animationen DÜRFEN keine unnötige Rechenzeit beanspruchen.
- **UI-014:** Der Deterministic Mode MUSS reproduzierbares Scheduling ermöglichen.
- **UI-015:** Reduzierte Animationen und Energiesparoptionen MÜSSEN berücksichtigt werden.
- **UI-016:** NovaStudio MUSS das Animation Scheduling diagnostizieren können.
- **UI-017:** Das Scheduling MUSS unabhängig vom Renderbackend funktionieren.
- **UI-018:** Nova Animation Scheduling MUSS vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 16. Integration

- **Nova Animation Engine:** Verwaltung und Auswertung von Animationen.
- **Nova Property Animations:** Aktualisierung animierter Eigenschaften.
- **Nova Transitions:** Automatische Zustandsübergänge.
- **Nova UI Threading Model:** UI Dispatcher und Event Loop.
- **Nova UI Component Lifecycle:** Aktivierung und Freigabe.
- **Nova UI Property System:** Bereitstellung animierter Werte.
- **Nova Layout Engine:** Layoutabhängige Animationen.
- **Nova UI Renderer Abstraction:** Rendererunabhängige Darstellung.
- **Nova Compositor:** Compositor-Animationen und Damage Tracking.
- **Nova Display Server:** Präsentationszeitpunkte und Bildwiederholraten.
- **Nova UI Accessibility:** Reduzierte Animationen.
- **NovaStudio:** Performanceanalyse und Diagnose.

## 17. Ergebnis

NovaOS erhält ein deterministisches und ressourcenschonendes Scheduling für native UI-Animationen.

Animationswerte werden zeitbasiert berechnet, Frames bedarfsgesteuert geplant und sichtbare Änderungen gezielt dargestellt.

Durch die Zusammenarbeit mit Compositor, Display Server und Damage Tracking werden unnötige Berechnungen und vollständige Neuzeichnungen vermieden.

**Grundsatz:** Nicht jeder Zeitpunkt benötigt einen neuen Frame. NovaOS berechnet und präsentiert nur dann einen Animationszustand, wenn dieser tatsächlich benötigt wird.