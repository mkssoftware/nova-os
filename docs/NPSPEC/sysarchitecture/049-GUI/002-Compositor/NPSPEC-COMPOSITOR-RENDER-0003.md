# NPSPEC-COMPOSITOR-RENDER-0003 – Nova Occlusion Culling

**Status:** Angenommen  
**Bereich:** Compositor / Sichtbarkeitsoptimierung  
**System:** NovaOS

## 1. Zweck

Nova Occlusion Culling definiert die Erkennung und Ausblendung vollständig oder teilweise verdeckter grafischer Inhalte während der Bildkomposition.

Ziel ist die Reduzierung unnötiger Rendering-Operationen, Speicherzugriffe und GPU-Auslastung, ohne die korrekte Darstellung zu beeinträchtigen.

## 2. Architektur

- Integration in Nova Compositor Pipeline und Scene Manager.
- Sichtbarkeitsanalyse auf Basis der aktuellen Szenenstruktur.
- Verarbeitung von Surfaces, Layern und Fenstergruppen.
- Unterstützung hierarchischer Sichtbarkeitsberechnungen.
- Berücksichtigung von Transparenz, Transformationen und Clipping.
- Kombination mit Nova Surface Damage Tracking.
- Unabhängige Sichtbarkeitsanalyse pro Display.
- Hardwareunabhängige Implementierung mit optionaler GPU-Beschleunigung.

## 3. Kernfunktionen

- Erkennung vollständig verdeckter Oberflächen.
- Berechnung teilweise sichtbarer Bereiche.
- Ausschluss unsichtbarer Inhalte von der Komposition.
- Verwaltung von Occlusion-Regionen.
- Unterstützung hierarchischer Surface- und Layer-Strukturen.
- Berücksichtigung von Alpha-Blending und Transparenz.
- Verarbeitung von Clipping und geometrischen Transformationen.
- Aktualisierung der Sichtbarkeit bei Fensterbewegungen.
- Optimierung von Damage-Regionen.
- Erkennung vollständig verdeckter Teilbäume.
- Wiederaktivierung zuvor verdeckter Inhalte.

## 4. Normative Anforderungen

- RENDER-001: Der Compositor MUSS vollständig verdeckte Oberflächen erkennen können.
- RENDER-002: Vollständig verdeckte Bereiche SOLLEN von unnötigen Kompositionsoperationen ausgeschlossen werden.
- RENDER-003: Teilweise sichtbare Oberflächen MÜSSEN korrekt verarbeitet werden.
- RENDER-004: Occlusion Culling MUSS die aktuelle Ebenenreihenfolge berücksichtigen.
- RENDER-005: Transparente Oberflächen DÜRFEN darunterliegende Inhalte nicht fälschlich vollständig verdecken.
- RENDER-006: Nur nachweislich vollständig deckende Bereiche DÜRFEN als vollständige Occluder verwendet werden.
- RENDER-007: Clipping und Transformationen MÜSSEN bei der Sichtbarkeitsberechnung berücksichtigt werden.
- RENDER-008: Änderungen der Szenenstruktur MÜSSEN betroffene Sichtbarkeitsinformationen invalidieren.
- RENDER-009: Hierarchische Surface-Strukturen MÜSSEN unterstützt werden.
- RENDER-010: Vollständig verdeckte Teilbäume SOLLEN gemeinsam übersprungen werden.
- RENDER-011: Damage-Regionen MÜSSEN mit den berechneten Sichtbarkeitsbereichen abgestimmt werden.
- RENDER-012: Beim Aufdecken zuvor verdeckter Bereiche MUSS eine korrekte Aktualisierung gewährleistet sein.
- RENDER-013: Schatten, Blur und andere Effekte MÜSSEN hinsichtlich ihrer sichtbaren Auswirkungen berücksichtigt werden.
- RENDER-014: Occlusion-Optimierungen DÜRFEN keine sichtbaren Darstellungsfehler verursachen.
- RENDER-015: Bei unsicherer Sichtbarkeitsbewertung MUSS die korrekte Darstellung Vorrang haben.
- RENDER-016: Die Komplexität der Sichtbarkeitsberechnung MUSS begrenzbar sein.
- RENDER-017: Unterschiedliche Displays MÜSSEN unabhängig analysiert werden können.
- RENDER-018: Occlusion Culling MUSS ohne KI-Abhängigkeit funktionieren.

## 5. Sichtbarkeitsmodell

### Fully Visible
Die Oberfläche ist innerhalb ihres relevanten Darstellungsbereichs vollständig sichtbar.

### Partially Occluded
Ein Teil der Oberfläche wird durch darüberliegende Inhalte verdeckt.

### Fully Occluded
Die Oberfläche ist vollständig durch deckende Inhalte verdeckt.

### Hidden
Die Oberfläche ist unabhängig von Überlagerungen ausdrücklich ausgeblendet.

### Unknown
Die Sichtbarkeit kann nicht zuverlässig bestimmt werden. In diesem Fall wird konservativ gerendert.

## 6. Occlusion-Verfahren

### Front-to-Back Analysis
Die Szenenstruktur wird von vorne nach hinten analysiert. Deckende Bereiche werden in einer Occlusion-Region gesammelt.

### Region Subtraction
Bereits verdeckte Bereiche werden von den sichtbaren Regionen nachfolgender Oberflächen abgezogen.

### Hierarchical Culling
Vollständig verdeckte Surface- oder Layer-Teilbäume werden ohne weitere Einzelprüfung übersprungen.

### Damage-Aware Culling
Nur sichtbare und tatsächlich veränderte Bereiche werden zur erneuten Komposition eingeplant.

### Conservative Culling
Bei komplexen Transparenzen, Transformationen oder Effekten wird zugunsten korrekter Darstellung auf unsichere Optimierungen verzichtet.

## 7. Verarbeitungsablauf

1. **Collect:** Aktuelle Szenenstruktur und Ebenenreihenfolge erfassen.
2. **Transform:** Geometrien in den Zielkoordinatenraum übertragen.
3. **Analyze:** Sichtbarkeit von vorne nach hinten bestimmen.
4. **Occlude:** Nachweislich deckende Regionen sammeln.
5. **Subtract:** Verdeckte Bereiche aus den sichtbaren Regionen entfernen.
6. **Optimize:** Vollständig verdeckte Oberflächen und Teilbäume ausschließen.
7. **Damage:** Sichtbare Regionen mit Damage-Informationen kombinieren.
8. **Compose:** Verbleibende Inhalte in korrekter Reihenfolge komponieren.

## 8. Performance und Zuverlässigkeit

- Vermeidung unnötiger GPU-Zeichenoperationen.
- Reduzierung von Speicherbandbreite und Energieverbrauch.
- Zwischenspeicherung gültiger Sichtbarkeitsinformationen.
- Aktualisierung ausschließlich betroffener Szenenbereiche.
- Effiziente Verarbeitung rechteckiger Occlusion-Regionen.
- Begrenzung komplexer Regionsberechnungen.
- Zusammenfassung benachbarter Occlusion-Bereiche.
- Konservativer Fallback bei komplexen Geometrien.
- Vermeidung unnötiger vollständiger Szenenanalysen.
- Messung übersprungener Flächen und eingesparter Kompositionsoperationen.

## 9. Ergebnis

NovaOS erhält eine effiziente und zuverlässige Sichtbarkeitsoptimierung innerhalb des Compositors.

Vollständig verdeckte Oberflächen und nicht sichtbare Bildbereiche werden von unnötigen Kompositionsoperationen ausgeschlossen.

Dadurch sinken GPU-Auslastung, Speicherbandbreite und Energieverbrauch, während Bildqualität und Darstellungskonsistenz erhalten bleiben.