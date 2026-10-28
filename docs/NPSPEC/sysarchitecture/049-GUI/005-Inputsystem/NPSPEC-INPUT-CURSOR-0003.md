# NPSPEC-INPUT-CURSOR-0003 – Nova Cursor Themes

**Status:** Angenommen  
**Bereich:** Input / Cursor / Themes  
**System:** NovaOS

## 1. Zweck

Nova Cursor Themes definiert die Verwaltung, Anpassung und Verwendung austauschbarer Cursor-Designs innerhalb von NovaOS.

Ziel ist ein einheitliches, skalierbares und ressourcenschonendes Cursor-System, das unterschiedliche Designs, Größen und Animationen unterstützt.

Cursor-Themes werden unabhängig von Anwendungen und dem verwendeten Grafikbackend verwaltet.

## 2. Architektur

Das Subsystem umfasst folgende Komponenten:

| Komponente | Aufgabe |
|---|---|
| Cursor Theme Manager | Verwaltung und Auswahl von Cursor-Themes |
| Cursor Theme Loader | Laden und Validieren von Theme-Ressourcen |
| Cursor Asset Manager | Verwaltung und Zwischenspeicherung von Cursorbildern |
| Cursor Style Resolver | Auflösung angeforderter Cursorformen |
| Cursor Theme Configuration | Verwaltung benutzerspezifischer Einstellungen |

Die Darstellung erfolgt über Nova Hardware and Software Cursor.

Der Cursor Theme Manager stellt ausschließlich die benötigten Cursorressourcen und Darstellungsinformationen bereit.

## 3. Theme-Modell

Ein Cursor-Theme besteht aus einer deklarativen Beschreibung und den zugehörigen Grafikressourcen.

Ein Theme enthält mindestens:

- Eindeutige Theme-ID.
- Theme-Name.
- Versionsinformation.
- Cursorform-Zuordnungen.
- Cursorbilder.
- Hotspot-Definitionen.
- Unterstützte Größen und Skalierungsfaktoren.

Optional können enthalten sein:

- Animierte Cursor.
- Farbvarianten.
- Kontrastvarianten.
- Autoreninformationen.
- Verweise auf ein Basis-Theme.

Die Theme-Beschreibung MUSS unabhängig vom verwendeten Grafikbackend interpretierbar sein.

## 4. Cursorformen

Ein Theme SOLL mindestens folgende Cursorformen bereitstellen:

| Cursorform | Verwendung |
|---|---|
| Default | Standardzeiger |
| Text | Texteingabe und Textauswahl |
| Hand | Interaktive Elemente |
| Crosshair | Präzisionsauswahl |
| Move | Verschieben |
| ResizeHorizontal | Horizontale Größenänderung |
| ResizeVertical | Vertikale Größenänderung |
| ResizeDiagonal1 | Diagonale Größenänderung |
| ResizeDiagonal2 | Diagonale Größenänderung |
| Wait | Warten |
| Progress | Hintergrundaktivität |
| NotAllowed | Nicht erlaubte Aktion |
| Help | Kontexthilfe |

Fehlende Cursorformen MÜSSEN durch geeignete Standardressourcen ersetzt werden.

Anwendungen fordern Cursorformen über semantische Bezeichner an und sind nicht von einem bestimmten Theme abhängig.

## 5. Cursorbilder und Hotspots

Cursorbilder können als Raster- oder Vektorgrafiken bereitgestellt werden, sofern das entsprechende Format unterstützt wird.

Dabei gilt:

- Transparenz MUSS unterstützt werden.
- Jeder Cursor besitzt einen definierten Hotspot.
- Hotspots werden bei Skalierung korrekt transformiert.
- Unterschiedliche Cursorgrößen DÜRFEN eigene Bildressourcen besitzen.
- Vektorgrafiken SOLLEN für unterschiedliche Auflösungen rasterisiert werden können.

Die Rasterisierung erfolgt nur bei Bedarf.

Bereits aufbereitete Cursorressourcen SOLLEN wiederverwendet werden.

## 6. Skalierung und Barrierefreiheit

NovaOS unterstützt benutzerspezifische Cursorgrößen.

Die Skalierung berücksichtigt:

- Display-Skalierung.
- Benutzerdefinierte Cursorgröße.
- Unterschiedliche Displayauflösungen.
- Hochauflösende Displays.
- Barrierefreiheitseinstellungen.

Cursor-Themes DÜRFEN spezielle Varianten für hohen Kontrast bereitstellen.

Bei fehlenden Varianten MUSS eine geeignete Standarddarstellung verfügbar bleiben.

Die Cursorgröße MUSS unabhängig von der allgemeinen UI-Skalierung angepasst werden können.

## 7. Animierte Cursor

Ein Cursor-Theme DARF animierte Cursor enthalten.

Eine Animation besteht aus:

- Einer definierten Bildfolge.
- Einer Anzeigedauer je Bild.
- Optionalen Wiederholungsinformationen.

Die Animationssteuerung erfolgt über Nova Cursor Management und Nova Frame Pacing.

Dabei gilt:

- Animationen werden nur bei aktiver Verwendung verarbeitet.
- Unveränderte Bilder verursachen keine unnötigen Aktualisierungen.
- Animationsressourcen werden nach Möglichkeit zwischengespeichert.
- Animationen DÜRFEN keine permanente vollständige Desktop-Neuberechnung verursachen.

Bei nicht unterstützten Animationen MUSS ein statisches Ersatzbild verwendet werden können.

## 8. Theme-Wechsel und Konfiguration

Cursor-Themes können während des laufenden Betriebs gewechselt werden.

Dabei gilt:

- Ein Theme-Wechsel erfordert keinen Systemneustart.
- Die aktuelle Cursorposition bleibt erhalten.
- Cursorform und Hotspot werden aktualisiert.
- Nicht mehr benötigte Ressourcen werden kontrolliert freigegeben.
- Fehlgeschlagene Theme-Wechsel verändern das aktive Theme nicht.

Benutzereinstellungen SOLLEN dauerhaft gespeichert werden.

NovaOS MUSS jederzeit auf das integrierte Standard-Theme zurückgreifen können.

## 9. Sicherheit und Ressourcenverwaltung

Cursor-Themes gelten grundsätzlich als nicht vertrauenswürdige Daten.

Festlegungen:

- Theme-Dateien MÜSSEN vor der Verwendung validiert werden.
- Ungültige Bildgrößen und Hotspots werden zurückgewiesen.
- Speicherverbrauch und Animationsressourcen MÜSSEN begrenzt werden können.
- Theme-Dateien DÜRFEN keinen ausführbaren Code voraussetzen.
- Anwendungen DÜRFEN das globale Cursor-Theme nicht ohne Berechtigung verändern.
- Fehlerhafte Themes DÜRFEN die Cursorbedienung nicht dauerhaft beeinträchtigen.

Die Installation und Verwaltung von Themes unterliegt den entsprechenden Berechtigungen von Nova.Trust.

## 10. Normative Anforderungen

- **INPUT-001:** NovaOS MUSS austauschbare Cursor-Themes unterstützen.
- **INPUT-002:** Cursor-Themes MÜSSEN unabhängig vom Grafikbackend definiert sein.
- **INPUT-003:** Cursorformen MÜSSEN über semantische Bezeichner aufgelöst werden.
- **INPUT-004:** Fehlende Cursorformen MÜSSEN durch geeignete Fallback-Ressourcen ersetzt werden.
- **INPUT-005:** Cursorbilder MÜSSEN Transparenz und Hotspots unterstützen.
- **INPUT-006:** Cursorgrößen MÜSSEN benutzerspezifisch konfigurierbar sein.
- **INPUT-007:** Unterschiedliche Display-Skalierungen MÜSSEN berücksichtigt werden.
- **INPUT-008:** Animierte Cursor MÜSSEN unterstützt werden.
- **INPUT-009:** Cursorressourcen SOLLEN zwischengespeichert werden.
- **INPUT-010:** Theme-Wechsel MÜSSEN ohne Systemneustart möglich sein.
- **INPUT-011:** Fehlgeschlagene Theme-Wechsel DÜRFEN das aktive Theme nicht beschädigen.
- **INPUT-012:** Ein integriertes Standard-Theme MUSS jederzeit verfügbar sein.
- **INPUT-013:** Theme-Dateien MÜSSEN vor ihrer Verwendung validiert werden.
- **INPUT-014:** Cursor-Themes DÜRFEN keinen ausführbaren Code benötigen.
- **INPUT-015:** Globale Theme-Änderungen MÜSSEN den Berechtigungen von Nova.Trust entsprechen.
- **INPUT-016:** Nova Cursor Themes MUSS vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 11. Integration

- **Nova Cursor Management:** Auswahl und Verwaltung aktiver Cursorformen.
- **Nova Hardware and Software Cursor:** Darstellung der Cursorressourcen.
- **Nova Graphics 2D:** Rasterisierung und Bildverarbeitung.
- **Nova Graphics Caching:** Zwischenspeicherung aufbereiteter Cursorbilder.
- **Nova Display System:** Skalierung und Displayeigenschaften.
- **Nova Frame Pacing:** Steuerung animierter Cursor.
- **Nova UI:** Semantische Cursoranforderungen und Benutzereinstellungen.
- **Nova.Trust:** Berechtigungen für Installation und Konfiguration.

## 12. Ergebnis

NovaOS erhält ein einheitliches und erweiterbares Cursor-Theme-System.

Cursorformen, Größen, Farben und Animationen können unabhängig vom verwendeten Grafikbackend angepasst werden.

Durch semantische Cursorbezeichner, Ressourcen-Caching und ein integriertes Standard-Theme bleibt die Cursorbedienung konsistent, effizient und zuverlässig.

**Grundsatz:** Das Aussehen des Cursors ist frei anpassbar, seine Funktion und Zuverlässigkeit bleiben davon unabhängig.