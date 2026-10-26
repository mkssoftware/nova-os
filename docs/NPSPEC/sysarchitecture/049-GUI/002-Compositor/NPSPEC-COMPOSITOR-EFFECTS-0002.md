# NPSPEC-COMPOSITOR-EFFECTS-0002 – Nova Window Shadows

**Status:** Angenommen  
**Bereich:** Compositor / Fensterschatten  
**System:** NovaOS

## 1. Zweck

Nova Window Shadows definiert die Erzeugung und Darstellung dynamischer Fensterschatten innerhalb des Nova Compositors.

Ziel ist eine dezente, moderne Tiefenwirkung zur visuellen Unterscheidung überlagerter Fenster bei möglichst geringem Ressourcenverbrauch.

## 2. Architektur

- Integration in Nova Layer Composition und Compositor Pipeline.
- Schatten als eigenständige visuelle Effekte einer Surface.
- GPU-beschleunigtes Rendering mit Software-Fallback.
- Unterstützung rechteckiger und frei geformter Fensterkonturen.
- Integration in Nova Surface Damage Tracking.
- Berücksichtigung von Occlusion Culling.
- Zwischenspeicherung wiederverwendbarer Schattenmasken.
- Konfiguration über das native Nova UI Designsystem.

## 3. Kernfunktionen

- Dynamische Schatten für Fenster und Popups.
- Einstellbare Schattenfarbe und Deckkraft.
- Konfigurierbarer Versatz, Radius und Ausbreitung.
- Unterstützung abgerundeter Fensterecken.
- Unterschiedliche Schatten für aktive und inaktive Fenster.
- Anpassung an Fenstergröße und Fensterzustand.
- Weiche Übergänge bei Zustandsänderungen.
- Wiederverwendung zwischengespeicherter Schatten.
- Automatische Deaktivierung bei geeigneten Vollbilddarstellungen.
- Unterstützung ressourcenschonender Darstellungsmodi.

## 4. Normative Anforderungen

- EFFECTS-001: Der Compositor MUSS Fensterschatten unabhängig vom Fensterinhalt darstellen können.
- EFFECTS-002: Schatten MÜSSEN der Position, Größe und sichtbaren Kontur ihres Fensters folgen.
- EFFECTS-003: Schatten MÜSSEN die festgelegte Ebenenreihenfolge berücksichtigen.
- EFFECTS-004: Schatten DÜRFEN keine Eingabeereignisse abfangen.
- EFFECTS-005: Schattenfarbe, Deckkraft, Versatz und Unschärferadius MÜSSEN konfigurierbar sein.
- EFFECTS-006: Abgerundete und nicht rechteckige Fensterkonturen SOLLEN unterstützt werden.
- EFFECTS-007: Änderungen der Schattengeometrie MÜSSEN die betroffenen Damage-Regionen aktualisieren.
- EFFECTS-008: Vollständig verdeckte Schattenbereiche SOLLEN nicht gerendert werden.
- EFFECTS-009: Unveränderte Schattenmasken SOLLEN wiederverwendet werden.
- EFFECTS-010: Schattenanimationen MÜSSEN mit dem Compositor Scheduling synchronisiert werden.
- EFFECTS-011: Schatten DÜRFEN keine unzulässigen Informationen geschützter Oberflächen offenlegen.
- EFFECTS-012: Bei Ressourcenknappheit MÜSSEN Schatten vereinfacht oder deaktiviert werden können.
- EFFECTS-013: Bei fehlender GPU-Beschleunigung MUSS ein Software-Fallback verfügbar sein.
- EFFECTS-014: Fensterschatten DÜRFEN die Stabilität des Compositors nicht beeinträchtigen.

## 5. Schattenmodell

Jeder Schatten besitzt folgende Eigenschaften:

| Eigenschaft | Beschreibung |
|---|---|
| Enabled | Aktivierungszustand |
| Color | Schattenfarbe |
| Opacity | Deckkraft |
| Offset | Horizontaler und vertikaler Versatz |
| Blur Radius | Stärke der Weichzeichnung |
| Spread | Ausdehnung der Schattenkontur |
| Shape | Zugrunde liegende Fensterkontur |
| Quality | Qualitätsstufe der Berechnung |

Die Eigenschaften werden durch das Designsystem vorgegeben und können innerhalb definierter Grenzen angepasst werden.

## 6. Rendering-Verfahren

### Shadow Mask

Aus der Fensterkontur wird eine Schattenmaske erzeugt.

### Blur Processing

Die Schattenmaske wird durch einen geeigneten Weichzeichnungsalgorithmus verarbeitet.

### Shadow Composition

Der Schatten wird entsprechend der Ebenenreihenfolge hinter der zugehörigen Fensteroberfläche komponiert.

### Shadow Caching

Berechnete Schattenmasken werden bei unveränderter Geometrie wiederverwendet.

### Adaptive Quality

Bei hoher Systemlast kann die Schattenqualität reduziert werden, ohne die Fensterdarstellung zu beeinträchtigen.

## 7. Verarbeitungsablauf

1. **Collect:** Fenstergeometrie und Schatteneigenschaften erfassen.
2. **Validate:** Schattenparameter und Ressourcen prüfen.
3. **Generate:** Schattenmaske erzeugen oder aus dem Cache laden.
4. **Blur:** Erforderliche Weichzeichnung anwenden.
5. **Transform:** Schattenposition und Ausdehnung bestimmen.
6. **Cull:** Verdeckte Schattenbereiche ausschließen.
7. **Compose:** Schatten hinter dem zugehörigen Fenster darstellen.
8. **Cache:** Wiederverwendbare Ergebnisse speichern.

## 8. Performance und Zuverlässigkeit

- GPU-beschleunigte Schattenberechnung.
- Caching unveränderter Schattenmasken.
- Wiederverwendung bei reinen Fensterbewegungen.
- Begrenzung der Schattenausdehnung.
- Partielle Aktualisierung betroffener Bildschirmbereiche.
- Vermeidung unnötiger Berechnungen bei verdeckten Fenstern.
- Adaptive Schattenqualität bei hoher Systemlast.
- Deaktivierung nicht benötigter Schatten bei Vollbilddarstellung.
- Software-Fallback und ressourcenschonende Ersatzdarstellung.

## 9. Ergebnis

NovaOS erhält eine einheitliche, performante und konfigurierbare Fensterschattendarstellung.

Fenster und Popups werden durch dezente Tiefeneffekte visuell voneinander getrennt.

Durch Schatten-Caching, Occlusion Culling und adaptive Qualitätsstufen bleibt die Darstellung auch auf leistungsschwächerer Hardware ressourcenschonend.