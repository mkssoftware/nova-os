# NPSPEC-COMPOSITOR-DAMAGE-0001 – Nova Damage Tracking and Partial Redraw

**Status:** Angenommen  
**Bereich:** Compositor / Damage Tracking / Rendering Optimization  
**System:** NovaOS

## 1. Zweck

Nova Damage Tracking and Partial Redraw definiert die Erkennung und gezielte Neuberechnung veränderter Bildbereiche innerhalb des Nova Compositors.

Ziel ist es, ausschließlich diejenigen Bereiche einer Oberfläche neu zu zeichnen, deren sichtbarer Inhalt sich tatsächlich verändert hat.

Unveränderte Bildbereiche werden wiederverwendet, um CPU- und GPU-Auslastung, Speicherbandbreite und Energieverbrauch zu reduzieren.

NovaOS verwendet grundsätzlich ereignisgesteuertes statt permanentes Rendering.

## 2. Architektur

Das Subsystem besteht aus folgenden Komponenten:

| Komponente | Aufgabe |
|---|---|
| Damage Tracker | Erfassung veränderter Oberflächenbereiche |
| Damage Region Manager | Verwaltung und Zusammenfassung von Damage-Regionen |
| Surface Damage Tracker | Überwachung einzelner Oberflächen |
| Scene Damage Tracker | Ermittlung sichtbarer Änderungen innerhalb der Szene |
| Damage Propagator | Übertragung von Änderungen in den Ausgabekoordinatenraum |
| Partial Redraw Controller | Steuerung der partiellen Neuberechnung |

Die Komponenten sind Bestandteil des Nova Compositors.

Der Display Server übermittelt Oberflächenänderungen. Der Window Manager meldet Änderungen an Fensterposition, Größe, Sichtbarkeit und Stapelreihenfolge.

Die Render Engine führt ausschließlich die vom Compositor angeforderten Zeichenoperationen aus.

## 3. Damage-Modell

Jede Oberfläche besitzt einen Damage-Zustand.

Dieser enthält:

- Oberflächenkennung.
- Oberflächendimensionen.
- Liste veränderter Regionen.
- Gültigkeitszustand des bisherigen Inhalts.
- Zugehörige Transformationsinformationen.
- Versionskennung des Oberflächenzustands.

Damage-Regionen werden als Rechtecke oder komplexere Regionen beschrieben.

Die Koordinaten werden zunächst im lokalen Koordinatensystem der Oberfläche erfasst.

Der Compositor überführt sie bei Bedarf in den jeweiligen Ausgabekoordinatenraum.

## 4. Damage-Erkennung

Damage entsteht insbesondere durch:

- Änderungen des Oberflächeninhalts.
- Fensterbewegungen.
- Größenänderungen.
- Änderungen der Fensterreihenfolge.
- Ein- und Ausblenden von Oberflächen.
- Animationen.
- Änderungen von Transparenz und Effekten.
- Änderungen von Clipping und Transformationen.
- Freilegen zuvor verdeckter Bereiche.

Bei Fensterbewegungen MÜSSEN sowohl die bisherige als auch die neue sichtbare Position berücksichtigt werden.

Verdeckte Oberflächenbereiche müssen nicht neu komponiert werden, solange sie unsichtbar bleiben.

Werden diese Bereiche sichtbar, MUSS ihre korrekte Darstellung sichergestellt werden.

## 5. Damage-Regionen

Der Damage Region Manager verwaltet mehrere veränderte Bereiche.

Unterstützt werden:

- Hinzufügen neuer Damage-Rechtecke.
- Zusammenführen überlappender Regionen.
- Entfernen vollständig enthaltener Regionen.
- Begrenzung auf sichtbare Ausgabebereiche.
- Transformation in andere Koordinatensysteme.
- Zusammenfassung benachbarter Regionen.

Die Zusammenfassung DARF zusätzliche Pixel einschließen, wenn dadurch die Verarbeitung effizienter wird.

Die tatsächlichen Änderungen MÜSSEN jedoch vollständig abgedeckt bleiben.

## 6. Partial Redraw

Der Compositor berechnet ausschließlich die als beschädigt markierten sichtbaren Ausgabebereiche neu.

Dabei gilt:

- Unveränderte Bildbereiche bleiben erhalten.
- Neue Zeichenoperationen werden auf Damage-Regionen begrenzt.
- Unveränderte Oberflächeninhalte werden wiederverwendet.
- Bereits berechnete Ergebnisse können aus dem Graphics Cache übernommen werden.
- Ein vollständiger Redraw erfolgt nur bei entsprechender Notwendigkeit.

Die Render Engine erhält die erforderlichen Damage-Regionen über den Rendering-Kontext.

Partial Redraw MUSS sowohl mit CPU-Rendering als auch mit geeigneten GPU-Backends funktionieren.

## 7. Effekte und Transformationen

Grafische Effekte können Änderungen über die ursprüngliche Damage-Region hinaus verursachen.

Dazu gehören:

- Schatten.
- Blur.
- Transparenz.
- Skalierung.
- Rotation.
- Farbfilter.
- Hintergrundunschärfe.

Der Compositor MUSS Damage-Regionen entsprechend dem tatsächlichen Einflussbereich eines Effekts erweitern.

Bei Hintergrundunschärfe MÜSSEN Änderungen der darunterliegenden Oberflächen berücksichtigt werden.

Unbekannte oder nicht zuverlässig begrenzbare Effekte erzwingen eine konservative Damage-Erweiterung oder einen vollständigen Redraw.

## 8. Ereignisgesteuertes Rendering

NovaOS verwendet kein dauerhaftes Neuzeichnen unveränderter Desktopinhalte.

Ein neuer Compositor-Frame wird insbesondere ausgelöst durch:

- Neue Surface-Damage-Meldungen.
- Änderungen der Fenstergeometrie.
- Sichtbarkeitsänderungen.
- Aktive Animationen.
- Änderungen von Display-Einstellungen.
- Erforderliche Präsentations- oder Synchronisationsereignisse.

Liegen keine relevanten Änderungen vor, SOLL keine erneute Bildberechnung stattfinden.

Zeitabhängige Inhalte melden ihre Änderungen über definierte Aktualisierungsereignisse.

Die Frame-Ausgabe erfolgt in Abstimmung mit Nova Frame Pacing.

## 9. Buffer-Verwaltung

Partial Redraw setzt voraus, dass unveränderte Bildbereiche weiterhin gültig verfügbar sind.

Der Compositor berücksichtigt:

- Single Buffering.
- Double Buffering.
- Triple Buffering.
- Buffer Age.
- Erhaltene Buffer-Inhalte.
- Historische Damage-Regionen.

Bei mehreren Ausgabebuffern MUSS berücksichtigt werden, welche Bereiche des jeweiligen Buffers seit seiner letzten Verwendung verändert wurden.

Ist der Inhalt eines Buffers unbekannt oder ungültig, MUSS ein vollständiger Redraw erfolgen.

Die Wiederverwendung unveränderter Bereiche DARF keine veralteten Bildinhalte verursachen.

## 10. Multi-Display-Unterstützung

Damage Tracking erfolgt für jeden Display-Ausgabebereich getrennt.

Dabei gilt:

- Jedes Display besitzt einen eigenen Damage-Zustand.
- Unterschiedliche Auflösungen und Skalierungsfaktoren werden berücksichtigt.
- Fenster über mehreren Displays erzeugen Damage in den jeweils betroffenen Ausgabebereichen.
- Änderungen eines Displays DÜRFEN keine unnötige Neuberechnung unabhängiger Displays erzwingen.

Die Präsentationsplanung erfolgt entsprechend den jeweiligen Display-Eigenschaften.

## 11. Optimierung

Das Subsystem unterstützt:

- Zusammenfassung kleiner Damage-Regionen.
- Reduzierung unnötiger Zeichenoperationen.
- Wiederverwendung unveränderter Surface-Inhalte.
- Vermeidung redundanter Speicherübertragungen.
- Begrenzung der Rendering-Arbeit auf sichtbare Bereiche.
- Adaptive Entscheidung zwischen Partial und Full Redraw.

Übersteigt der Aufwand der partiellen Verarbeitung den Aufwand eines vollständigen Redraws, DARF der Compositor auf vollständiges Rendering wechseln.

Diese Entscheidung erfolgt anhand definierter, reproduzierbarer Kriterien.

## 12. Normative Anforderungen

- **COMPOSITOR-001:** NovaOS MUSS Damage Tracking für Compositor-Oberflächen bereitstellen.
- **COMPOSITOR-002:** Der Compositor MUSS veränderte Bildbereiche gezielt neu berechnen können.
- **COMPOSITOR-003:** Unveränderte Bildbereiche SOLLEN ohne erneutes Rendering wiederverwendet werden.
- **COMPOSITOR-004:** Damage-Regionen MÜSSEN zusammengeführt und begrenzt werden können.
- **COMPOSITOR-005:** Fensterbewegungen MÜSSEN bisherige und neue sichtbare Bereiche berücksichtigen.
- **COMPOSITOR-006:** Transformationen und Effekte MÜSSEN bei der Damage-Berechnung berücksichtigt werden.
- **COMPOSITOR-007:** Der Compositor MUSS bei Bedarf einen vollständigen Redraw durchführen können.
- **COMPOSITOR-008:** Ungültige oder unbekannte Buffer-Inhalte MÜSSEN vollständig neu berechnet werden.
- **COMPOSITOR-009:** Unterschiedliche Ausgabebuffer MÜSSEN hinsichtlich ihres Damage-Zustands korrekt behandelt werden.
- **COMPOSITOR-010:** Damage Tracking MUSS mehrere Displays unabhängig unterstützen.
- **COMPOSITOR-011:** Ohne relevante Änderungen SOLL keine unnötige Frame-Berechnung stattfinden.
- **COMPOSITOR-012:** Aktive Animationen MÜSSEN ihre veränderten Bereiche aktualisieren können.
- **COMPOSITOR-013:** Partial Redraw MUSS mit CPU-Rendering funktionieren.
- **COMPOSITOR-014:** GPU-basiertes Partial Redraw SOLL bei geeigneten Backend-Capabilities unterstützt werden.
- **COMPOSITOR-015:** Die Optimierung DARF keine sichtbaren Darstellungsfehler verursachen.
- **COMPOSITOR-016:** Damage Tracking MUSS deterministisch und ohne KI-Abhängigkeit funktionieren.

## 13. Fehlerbehandlung

- Ungültige Damage-Koordinaten werden zurückgewiesen oder sicher begrenzt.
- Unbekannte Oberflächenzustände lösen einen vollständigen Redraw aus.
- Fehlerhafte Transformationsinformationen dürfen keine unvollständige Aktualisierung verursachen.
- Bei Verlust des Ausgabebuffers wird dessen Inhalt vollständig neu aufgebaut.
- Fehlgeschlagene Partial-Redraw-Operationen lösen eine kontrollierte vollständige Neuberechnung aus.
- Damage-Fehler DÜRFEN keine fremden Speicherbereiche beeinflussen.

## 14. Integration

- **Nova Display Server:** Übermittlung von Surface-Damage und Buffer-Aktualisierungen.
- **Nova Window Manager:** Fenstergeometrie, Sichtbarkeit und Stapelreihenfolge.
- **Nova Compositor:** Berechnung und Verwaltung sichtbarer Damage-Regionen.
- **Nova Render Engine:** Ausführung partieller Zeichenoperationen.
- **Nova Graphics Caching:** Wiederverwendung unveränderter Rendering-Ergebnisse.
- **Nova Frame Pacing:** Bedarfsgesteuerte Frame-Planung.
- **Nova Graphics Memory Optimization:** Effiziente Buffer- und Speicherverwaltung.
- **Nova Graphics Backend Abstraction:** CPU- und GPU-spezifische Umsetzung.

## 15. Ergebnis

NovaOS erhält ein ereignisgesteuertes Rendering-System, das veränderte Bildbereiche erkennt und gezielt aktualisiert.

Unveränderte Desktopbereiche müssen nicht permanent neu berechnet werden.

Dadurch werden CPU- und GPU-Auslastung, Speicherbandbreite und Energieverbrauch reduziert, während eine flüssige und reaktionsschnelle Darstellung erhalten bleibt.

**Grundsatz:** NovaOS zeichnet nur das neu, was sich tatsächlich verändert hat. Unveränderte Bildinhalte bleiben erhalten und verursachen keine unnötige Rendering-Arbeit.