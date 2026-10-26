# NPSPEC-COMPOSITOR-RENDER-0002 – Nova Layer Composition

**Status:** Angenommen  
**Bereich:** Compositor / Ebenenkomposition  
**System:** NovaOS

## 1. Zweck

Nova Layer Composition definiert die Verwaltung und Zusammensetzung grafischer Ebenen innerhalb des Nova Compositors.

Ziel ist eine effiziente, flexible und sichere Darstellung überlagerter Bildinhalte unter Berücksichtigung von Transparenz, Transformationen, Sichtbarkeit und Ebenenreihenfolge.

## 2. Architektur

- Integration in Nova Compositor Pipeline und Scene Manager.
- Trennung zwischen Surface-Hierarchie und visueller Ebenenstruktur.
- Verwaltung von Layern innerhalb eines Szenengraphen.
- Unterstützung hierarchischer und unabhängiger Layer.
- GPU-beschleunigte Komposition mit Software-Fallback.
- Unterstützung von Hardware-Overlays und Direct Scanout.
- Integration in Nova Surface Damage Tracking.
- Unabhängige Ebenenkomposition pro Display.
- Capability-basierte Kontrolle geschützter Ebenen.

## 3. Kernfunktionen

- Erstellung und Verwaltung grafischer Layer.
- Zuordnung von Surfaces zu Layern.
- Verwaltung von Ebenenreihenfolge und Sichtbarkeit.
- Unterstützung von Transparenz und Alpha-Blending.
- Verarbeitung von Skalierung, Rotation und Transformationen.
- Unterstützung von Clipping und Maskierung.
- Gruppierung zusammengehöriger Layer.
- Darstellung von Hintergrund-, Fenster- und Overlay-Ebenen.
- Erkennung vollständig verdeckter Bereiche.
- Partielle Aktualisierung veränderter Layer.
- Nutzung von Hardware-Overlay-Ebenen.
- Optimierung statischer und dynamischer Inhalte.

## 4. Normative Anforderungen

- RENDER-001: Jeder Layer MUSS eine eindeutige Identität besitzen.
- RENDER-002: Die Ebenenreihenfolge MUSS eindeutig definiert sein.
- RENDER-003: Layer MÜSSEN unabhängig von einzelnen Grafikbuffern verwaltet werden können.
- RENDER-004: Surfaces MÜSSEN über definierte Referenzen in Layer eingebunden werden.
- RENDER-005: Änderungen der Ebenenstruktur MÜSSEN atomar übernommen werden können.
- RENDER-006: Transparenz und Alpha-Blending MÜSSEN unterstützt werden.
- RENDER-007: Layer MÜSSEN eigene Transformationen und Clipping-Bereiche besitzen können.
- RENDER-008: Hierarchische Transformationen MÜSSEN korrekt berücksichtigt werden.
- RENDER-009: Vollständig verdeckte Layer SOLLEN von unnötigen Kompositionsoperationen ausgeschlossen werden.
- RENDER-010: Unveränderte Layer SOLLEN nicht unnötig neu berechnet werden.
- RENDER-011: Geschützte Systemlayer DÜRFEN nicht durch unberechtigte Clients manipuliert oder überlagert werden.
- RENDER-012: Hardware-Overlays SOLLEN bei geeigneten Layern bevorzugt werden.
- RENDER-013: Nicht unterstützte Hardware-Overlays MÜSSEN durch reguläre Komposition ersetzt werden können.
- RENDER-014: Layer-Änderungen MÜSSEN mit Nova Surface Damage Tracking abgestimmt werden.
- RENDER-015: Farbverarbeitung MUSS die Eigenschaften des jeweiligen Layers und Ziel-Displays berücksichtigen.
- RENDER-016: Layer-Gruppen MÜSSEN konsistent verarbeitet werden.
- RENDER-017: Die Anzahl und Komplexität von Layern MÜSSEN ressourcenabhängig begrenzbar sein.
- RENDER-018: Fehlerhafte Layer DÜRFEN den Compositor nicht destabilisieren.

## 5. Layer-Modell

### Background Layer
Darstellung von Hintergrundbildern und grundlegenden Desktopinhalten.

### Application Layer
Darstellung regulärer Anwendungsfenster und ihrer Oberflächen.

### Overlay Layer
Darstellung zusätzlicher Oberflächen wie Popups, Menüs und Benachrichtigungen.

### System Layer
Darstellung geschützter Systemoberflächen mit definierten Prioritäts- und Sicherheitsregeln.

### Effect Layer
Verarbeitung zusätzlicher visueller Effekte und zwischengespeicherter Kompositionsergebnisse.

### Group Layer
Logische Zusammenfassung mehrerer Layer zur gemeinsamen Verarbeitung.

Die Zuordnung zu einem Layertyp DARF keine bestehenden Capability- oder Fensterverwaltungsregeln umgehen.

## 6. Ebeneneigenschaften

Jeder Layer kann folgende Eigenschaften besitzen:

| Eigenschaft | Beschreibung |
|---|---|
| Layer ID | Eindeutige Identität |
| Surface | Zugeordnete grafische Oberfläche |
| Parent | Optionale übergeordnete Ebene |
| Order | Reihenfolge innerhalb des Darstellungskontexts |
| Transform | Position, Skalierung und Rotation |
| Opacity | Deckkraft |
| Blend Mode | Verfahren zur Farbüberlagerung |
| Clip | Sichtbarkeitsbegrenzung |
| Visibility | Sichtbarkeitszustand |
| Damage | Veränderte Bildbereiche |
| Security | Zugriffs- und Schutzanforderungen |

## 7. Kompositionsablauf

1. **Collect:** Layer und zugehörige Surface-Zustände erfassen.
2. **Validate:** Berechtigungen und Ebenenbeziehungen prüfen.
3. **Update:** Transformationen und Ebenenreihenfolge aktualisieren.
4. **Analyze:** Sichtbarkeit, Überlagerungen und Damage-Regionen bestimmen.
5. **Optimize:** Hardware-Overlays und mögliche Kompositionsvereinfachungen auswählen.
6. **Compose:** Sichtbare Layer in definierter Reihenfolge zusammensetzen.
7. **Process:** Transparenz, Effekte und Farbtransformationen anwenden.
8. **Synchronize:** Buffer-Zugriffe und Rendering-Operationen synchronisieren.
9. **Present:** Ergebnis an die Bildausgabe übergeben.

## 8. Performance und Zuverlässigkeit

- GPU-beschleunigte Ebenenkomposition.
- Vermeidung unnötiger Zwischenbuffer.
- Zero-Copy-Verarbeitung kompatibler Layer.
- Hardware-Overlay-Nutzung bei geeigneten Inhalten.
- Occlusion Culling für verdeckte Ebenen.
- Partielle Aktualisierung durch Damage Tracking.
- Zwischenspeicherung unveränderter Kompositionsergebnisse.
- Effiziente Verarbeitung hierarchischer Layer.
- Reduzierung unnötiger Alpha-Blending-Operationen.
- Begrenzung komplexer Layer-Strukturen.
- Software-Fallback bei fehlender Hardwarebeschleunigung.
- Wiederherstellung gültiger Ebenenzustände nach Compositor-Fehlern.

## 9. Ergebnis

NovaOS erhält eine flexible und leistungsfähige Architektur zur Komposition grafischer Ebenen.

Fenster, Hintergründe, Overlays und visuelle Effekte können unabhängig organisiert und effizient zusammengesetzt werden.

Durch hierarchische Layer, Hardware-Overlays und gezielte Aktualisierungen wird eine moderne Desktopdarstellung mit geringem Ressourcenverbrauch ermöglicht.