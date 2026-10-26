# NPSPEC-COMPOSITOR-EFFECTS-0001 – Nova Transparency and Alpha Composition

**Status:** Angenommen  
**Bereich:** Compositor / Transparenz und Alpha-Komposition  
**System:** NovaOS

## 1. Zweck

Nova Transparency and Alpha Composition definiert die Verarbeitung transparenter und halbtransparenter Oberflächen innerhalb des Nova Compositors.

Ziel ist eine visuell korrekte, performante und einheitliche Darstellung von Transparenz für Fenster, Nova UI Controls, Overlays und Systemoberflächen.

## 2. Architektur

- Integration in Nova Compositor Pipeline und Layer Composition.
- Unterstützung von Alpha-Blending auf Surface- und Layerebene.
- Verwendung von Premultiplied Alpha als internes Standardformat.
- Unterstützung globaler Deckkraft und pixelbasierter Transparenz.
- GPU-beschleunigte Verarbeitung mit Software-Fallback.
- Integration in Nova Color Management.
- Berücksichtigung von Occlusion Culling und Damage Tracking.
- Unterstützung transparenter Fenster und hierarchischer UI-Elemente.
- Trennung zwischen Transparenz und Hintergrundeffekten wie Acrylic oder Blur.

## 3. Kernfunktionen

- Pixelbasierte Transparenz.
- Globale Deckkraft für Surfaces und Layer.
- Alpha-Blending mehrerer übereinanderliegender Oberflächen.
- Transparente Fenster und UI-Elemente.
- Unterstützung von Premultiplied und Straight Alpha.
- Transparenzvererbung innerhalb hierarchischer Layer.
- Transparente Clipping- und Maskierungsbereiche.
- Optimierung vollständig transparenter Bereiche.
- Erkennung vollständig deckender Bereiche.
- Unterstützung animierter Transparenz.
- Korrekte Verarbeitung von SDR- und HDR-Inhalten.

## 4. Normative Anforderungen

- EFFECTS-001: Der Compositor MUSS pixelbasierte und globale Transparenz unterstützen.
- EFFECTS-002: Premultiplied Alpha MUSS als internes Standardformat verwendet werden.
- EFFECTS-003: Straight-Alpha-Buffer MÜSSEN bei Bedarf korrekt konvertiert werden.
- EFFECTS-004: Alpha-Werte MÜSSEN im gültigen Bereich von 0 bis 1 verarbeitet werden.
- EFFECTS-005: Vollständig transparente Pixel DÜRFEN das sichtbare Ergebnis nicht verändern.
- EFFECTS-006: Vollständig deckende Pixel MÜSSEN darunterliegende Inhalte entsprechend der Ebenenreihenfolge verdecken.
- EFFECTS-007: Globale Deckkraft MUSS mit pixelbasierten Alpha-Werten kombinierbar sein.
- EFFECTS-008: Hierarchische Deckkraft MUSS konsistent auf die jeweilige Layer-Gruppe angewendet werden.
- EFFECTS-009: Alpha-Blending MUSS in einem geeigneten linearen Farbraum erfolgen.
- EFFECTS-010: Farb- und Alpha-Werte MÜSSEN bei Farbraumkonvertierungen korrekt behandelt werden.
- EFFECTS-011: Transparente Oberflächen DÜRFEN beim Occlusion Culling nicht fälschlich als vollständig deckend gelten.
- EFFECTS-012: Transparenzänderungen MÜSSEN die betroffenen Damage-Regionen aktualisieren.
- EFFECTS-013: Alpha-Komposition MUSS mit GPU- und Software-Rendering konsistente Ergebnisse liefern.
- EFFECTS-014: Transparenzanimationen SOLLEN ohne unnötige Neuberechnung unveränderter Surface-Inhalte möglich sein.
- EFFECTS-015: Ungültige Alpha- oder Farbwerte MÜSSEN definiert behandelt werden.
- EFFECTS-016: Geschützte Systemoberflächen DÜRFEN durch Transparenzeinstellungen nicht unzulässig manipuliert werden.
- EFFECTS-017: Bei fehlender Hardwarebeschleunigung MUSS eine Softwareimplementierung verfügbar sein.

## 5. Transparenzmodell

### Opaque

Die Oberfläche ist vollständig deckend.

- Alpha = 1
- Keine Alpha-Komposition erforderlich.
- Optimierung durch Occlusion Culling möglich.

### Transparent

Die Oberfläche ist vollständig durchsichtig.

- Alpha = 0
- Keine sichtbare Farbwirkung.
- Kompositionsoperationen können entfallen.

### Translucent

Die Oberfläche ist teilweise transparent.

- Alpha zwischen 0 und 1.
- Hintergrundinhalte bleiben anteilig sichtbar.
- Alpha-Blending erforderlich.

### Global Opacity

Ein zusätzlicher Deckkraftfaktor wird auf eine gesamte Surface oder Layer-Gruppe angewendet.

### Per-Pixel Alpha

Jeder Pixel besitzt einen individuellen Transparenzwert.

## 6. Alpha-Komposition

NovaOS verwendet standardmäßig das Porter-Duff-Verfahren **Source Over**.

Für Premultiplied Alpha gilt:

`C_result = C_source + C_destination × (1 - A_source)`

`A_result = A_source + A_destination × (1 - A_source)`

Dabei bezeichnet:

- `C_source` die vormultiplizierte Quellfarbe.
- `C_destination` die vormultiplizierte Zielfarbe.
- `A_source` den Quell-Alpha-Wert.
- `A_destination` den Ziel-Alpha-Wert.

Die Berechnung erfolgt in einem geeigneten linearen Farbraum.

Weitere Blend-Modi DÜRFEN über definierte Erweiterungen bereitgestellt werden.

## 7. Verarbeitung

1. **Collect:** Sichtbare Surfaces und Layer erfassen.
2. **Validate:** Alpha-Format und Transparenzeigenschaften prüfen.
3. **Convert:** Farb- und Alpha-Formate bei Bedarf konvertieren.
4. **Optimize:** Vollständig transparente und deckende Bereiche erkennen.
5. **Transform:** Globale und hierarchische Deckkraft berücksichtigen.
6. **Blend:** Oberflächen entsprechend ihrer Ebenenreihenfolge zusammensetzen.
7. **Color:** Ergebnis für den Ziel-Farbraum aufbereiten.
8. **Present:** Fertigen Frame an die Bildausgabe übergeben.

## 8. Performance und Zuverlässigkeit

- GPU-beschleunigtes Alpha-Blending.
- Optimierte Verarbeitung vollständig deckender Oberflächen.
- Überspringen vollständig transparenter Bereiche.
- Vermeidung unnötiger Zwischenbuffer.
- Wiederverwendung unveränderter Surface-Inhalte.
- Unterstützung partieller Bildaktualisierungen.
- Begrenzung unnötiger Transparenzoperationen.
- Software-Fallback bei fehlender GPU-Unterstützung.
- Definierte Behandlung ungültiger Farb- und Alpha-Werte.
- Vermeidung sichtbarer Farbsäume an transparenten Kanten.

## 9. Ergebnis

NovaOS erhält eine einheitliche und leistungsfähige Transparenzverarbeitung für sämtliche grafischen Oberflächen.

Fenster, Nova UI Controls, Overlays und Systemelemente können mit pixelgenauer Transparenz, globaler Deckkraft und hierarchischem Alpha-Blending dargestellt werden.

Die Architektur bildet zugleich die Grundlage für weiterführende visuelle Effekte wie Acrylic, Blur, Schatten und transparente Animationen.