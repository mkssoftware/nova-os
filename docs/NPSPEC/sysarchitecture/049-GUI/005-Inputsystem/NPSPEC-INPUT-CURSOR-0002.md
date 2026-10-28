# NPSPEC-INPUT-CURSOR-0002 – Nova Hardware and Software Cursor

**Status:** Angenommen  
**Bereich:** Input / Cursor / Rendering  
**System:** NovaOS

## 1. Zweck

Nova Hardware and Software Cursor definiert die Darstellung des Systemcursors über Hardware-Cursor-Ebenen oder softwarebasierte Komposition.

Ziel ist eine möglichst latenzarme, flüssige und ressourcenschonende Cursordarstellung.

Cursorbewegungen MÜSSEN unabhängig von der vollständigen Neuberechnung der Desktopoberfläche möglich sein.

## 2. Architektur

Das Subsystem umfasst folgende Komponenten:

| Komponente | Aufgabe |
|---|---|
| Cursor Backend Manager | Auswahl des geeigneten Darstellungsbackends |
| Hardware Cursor Backend | Darstellung über GPU- oder Display-Hardware |
| Software Cursor Backend | Darstellung über den Nova Compositor |
| Cursor Surface Manager | Verwaltung der Cursorbilder und Hotspots |
| Cursor Presentation Controller | Aktualisierung von Position und Darstellung |

Die Verwaltung von Cursorposition, Cursorform und Sichtbarkeit erfolgt durch Nova Cursor Management.

Dieses Subsystem ist ausschließlich für die technische Darstellung verantwortlich.

## 3. Hardware-Cursor

Der Hardware-Cursor verwendet eine dedizierte Cursor-Ebene des Grafiksystems.

Sofern unterstützt, werden folgende Funktionen verwendet:

- Cursorposition setzen.
- Cursorbild ändern.
- Cursor ein- und ausblenden.
- Cursor-Hotspot berücksichtigen.
- Hardwareunterstützte Cursorformate verwenden.

Die Cursorposition wird direkt über das Grafikbackend aktualisiert.

Eine Bewegung des Hardware-Cursors DARF keine vollständige Desktop-Neuberechnung auslösen.

Die Hardware-Cursor-Unterstützung wird über die Nova Graphics Driver Interface bereitgestellt.

## 4. Software-Cursor

Ist kein geeigneter Hardware-Cursor verfügbar, erfolgt die Darstellung über den Nova Compositor.

Dabei gilt:

- Der Cursor wird als eigenständiges Compositor-Element behandelt.
- Die bisherige Cursorregion wird als verändert markiert.
- Die neue Cursorregion wird als verändert markiert.
- Ausschließlich betroffene Bereiche werden neu komponiert.
- Unveränderte Desktopbereiche werden nicht erneut gerendert.

Der Software-Cursor MUSS mit Nova Damage Tracking and Partial Redraw zusammenarbeiten.

Die Darstellung DARF keine dauerhafte vollständige Neuberechnung des Desktops erfordern.

## 5. Backend-Auswahl

Der Cursor Backend Manager wählt das geeignete Backend anhand folgender Kriterien:

- Verfügbarkeit eines Hardware-Cursors.
- Unterstützte Cursorgröße.
- Unterstütztes Pixelformat.
- Transparenzunterstützung.
- Display-Skalierung.
- Display-Rotation.
- Aktive Grafikeffekte.
- Verfügbarkeit des Grafiktreibers.

**Priorität:**

1. Hardware-Cursor, sofern alle Anforderungen erfüllt werden.
2. Software-Cursor als Fallback.

Ein Backendwechsel MUSS während des Betriebs möglich sein.

## 6. Cursor-Surfaces

Cursorbilder werden als eigenständige Grafikressourcen verwaltet.

Eine Cursor-Surface enthält:

- Bilddaten.
- Breite und Höhe.
- Pixelformat.
- Transparenzinformationen.
- Hotspot.
- Optionalen Skalierungsfaktor.

Cursor-Surfaces SOLLEN zwischen Anwendungen und Darstellungsbackends wiederverwendet werden können.

Unveränderte Cursorbilder DÜRFEN nicht bei jeder Cursorbewegung erneut hochgeladen oder rasterisiert werden.

## 7. Cursorposition und Präsentation

Cursorposition und Cursorbild werden unabhängig voneinander aktualisiert.

Dabei gilt:

- Positionsänderungen benötigen keinen erneuten Upload des Cursorbildes.
- Die aktuellste verfügbare Position SOLL dargestellt werden.
- Veraltete Zwischenpositionen DÜRFEN übersprungen werden.
- Cursorbewegungen DÜRFEN nicht durch langsame Anwendungsframes blockiert werden.
- Die Darstellung MUSS mit den Koordinatentransformationen des Display-Systems übereinstimmen.

Bei Hardware-Cursorn SOLL die Positionsaktualisierung unabhängig vom regulären Compositor-Frame erfolgen.

## 8. Multi-Display

Jeder Display-Output kann unterschiedliche Cursorfähigkeiten besitzen.

NovaOS berücksichtigt:

- Unterschiedliche Hardware-Cursor-Unterstützung.
- Verschiedene Skalierungsfaktoren.
- Unterschiedliche Cursorgrößen.
- Display-Rotation.
- Übergänge zwischen Displays.

Ein Cursor DARF beim Wechsel zwischen Displays automatisch das Darstellungsbackend wechseln.

Die logische Cursorposition MUSS dabei erhalten bleiben.

## 9. Backendwechsel und Recovery

Ein Backendwechsel kann erforderlich werden bei:

- Grafiktreiberwechsel.
- GPU-Reset.
- Änderung der Display-Konfiguration.
- Nicht unterstütztem Cursorformat.
- Wechsel in einen anderen Grafikmodus.
- Ausfall der Hardware-Cursor-Funktion.

Dabei gilt:

- Die bisherige Darstellung wird kontrolliert deaktiviert.
- Der Cursorzustand bleibt erhalten.
- Das neue Backend übernimmt Position, Bild und Sichtbarkeit.
- Doppelte Cursorbilder MÜSSEN vermieden werden.
- Bei Hardwarefehlern wird auf Software-Rendering zurückgegriffen.

Die Cursorbedienung SOLL auch während der Grafiktreiber-Recovery verfügbar bleiben, sofern ein funktionsfähiger Ausgabepfad existiert.

## 10. Performance und Ressourcenökonomie

NovaOS verfolgt folgende Optimierungsprinzipien:

- Hardware-Cursor bevorzugen.
- Cursorposition ohne vollständigen Desktop-Render aktualisieren.
- Cursorbilder zwischenspeichern.
- Unnötige GPU-Uploads vermeiden.
- Software-Cursor ausschließlich über Damage Regions aktualisieren.
- Keine permanente Render-Schleife allein für einen unveränderten Cursor.
- Animationen nur bei tatsächlichem Bedarf aktualisieren.

Ein ruhender Cursor DARF keine fortlaufende Rendering-Aktivität verursachen.

## 11. Normative Anforderungen

- **INPUT-001:** NovaOS MUSS Hardware- und Software-Cursor unterstützen.
- **INPUT-002:** Hardware-Cursor SOLLEN bevorzugt verwendet werden, sofern sie die erforderlichen Funktionen unterstützen.
- **INPUT-003:** Ein Software-Cursor MUSS als Fallback verfügbar sein.
- **INPUT-004:** Cursorposition und Cursorbild MÜSSEN unabhängig aktualisierbar sein.
- **INPUT-005:** Hardware-Cursor-Bewegungen DÜRFEN keine vollständige Desktop-Neuberechnung erfordern.
- **INPUT-006:** Software-Cursor MÜSSEN Damage Tracking und Partial Redraw verwenden.
- **INPUT-007:** Unveränderte Cursorbilder DÜRFEN nicht bei jeder Bewegung erneut übertragen werden.
- **INPUT-008:** Backendwechsel MÜSSEN während des Betriebs möglich sein.
- **INPUT-009:** Backendwechsel DÜRFEN keine dauerhaft doppelten oder verschwundenen Cursor verursachen.
- **INPUT-010:** Cursor-Hotspots und Transparenz MÜSSEN korrekt berücksichtigt werden.
- **INPUT-011:** Unterschiedliche Display-Skalierungen und Rotationen MÜSSEN unterstützt werden.
- **INPUT-012:** Bei Ausfall des Hardware-Cursors MUSS ein Software-Fallback versucht werden.
- **INPUT-013:** Cursorbewegungen SOLLEN unabhängig von der regulären Compositor-Framerate dargestellt werden können.
- **INPUT-014:** Ein unveränderter Cursor DARF keine permanente Rendering-Aktivität verursachen.
- **INPUT-015:** Die Cursor-Darstellung MUSS ohne KI-Abhängigkeit und vollständig offline funktionieren.

## 12. Integration

- **Nova Cursor Management:** Cursorzustand, Position und Sichtbarkeit.
- **Nova Graphics Driver Interface:** Hardware-Cursor-Funktionen.
- **Nova Graphics Backend Abstraction:** Auswahl und Nutzung des Grafikbackends.
- **Nova Compositor:** Software-Cursor-Darstellung.
- **Nova Damage Tracking:** Partielle Aktualisierung der Cursorregion.
- **Nova Display System:** Display-Zuordnung und Koordinatentransformation.
- **Nova Frame Pacing:** Präsentationssteuerung und Animationen.
- **Nova Graphics Driver Recovery:** Wiederherstellung nach Grafikfehlern.

## 13. Ergebnis

NovaOS erhält eine flexible Cursor-Darstellung mit automatischer Auswahl zwischen Hardware- und Software-Rendering.

Hardware-Cursor ermöglichen besonders latenzarme Bewegungen ohne erneute Komposition der Desktopoberfläche.

Software-Cursor gewährleisten durch Damage Tracking eine effiziente Darstellung auf Systemen ohne geeignete Hardwareunterstützung.

**Grundsatz:** Die Cursorposition wird so direkt wie möglich aktualisiert. Nur wenn tatsächlich Bildinhalte verändert werden müssen, erfolgt eine entsprechende grafische Neuberechnung.