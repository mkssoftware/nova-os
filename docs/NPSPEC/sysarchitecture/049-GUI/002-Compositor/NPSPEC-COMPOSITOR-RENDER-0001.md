# NPSPEC-COMPOSITOR-RENDER-0001 – Nova Window Composition

**Status:** Angenommen  
**Bereich:** Compositor / Fensterkomposition  
**System:** NovaOS

## 1. Zweck

Nova Window Composition definiert die Zusammensetzung grafischer Fenster und ihrer Oberflächen zum endgültigen Desktopbild.

Ziel ist eine flüssige, latenzarme und ressourcenschonende Fensterdarstellung mit Unterstützung moderner visueller Effekte.

## 2. Architektur

- Integration in Nova Compositor Pipeline und Scene Manager.
- Trennung zwischen Fensterverwaltung und Bildkomposition.
- Window Manager bestimmt Fensterzustände, Positionen und Ebenenregeln.
- Compositor verarbeitet die daraus resultierende Szenenstruktur.
- Fensterinhalte werden über Nova Surface Management bereitgestellt.
- GPU-beschleunigte Komposition mit Software-Fallback.
- Unterstützung mehrerer Displays und virtueller Desktops.
- Integration in Nova Color Management.
- Unabhängigkeit von Nova UI und einzelnen Anwendungen.

## 3. Kernfunktionen

- Zusammensetzen mehrerer Fensteroberflächen.
- Verwaltung der visuellen Fensterreihenfolge.
- Darstellung von Fensterrahmen und Dekorationen.
- Unterstützung von Transparenz und Alpha-Blending.
- Darstellung von Schatten, Blur und Acrylic-Effekten.
- Verarbeitung von Fenstertransformationen.
- Unterstützung von Popups, Dialogen und Overlays.
- Clipping und Occlusion Culling.
- Partielle Aktualisierung veränderter Fensterbereiche.
- Optimierung maximierter und vollflächiger Fenster.
- Unterstützung von Direct Scanout und Hardware-Overlays.
- Farbkorrekte Komposition von SDR- und HDR-Inhalten.

## 4. Normative Anforderungen

- RENDER-001: Fenster MÜSSEN entsprechend der vom Window Manager festgelegten Ebenenreihenfolge dargestellt werden.
- RENDER-002: Der Compositor DARF Fensterpositionen und Verwaltungsrichtlinien nicht eigenständig verändern.
- RENDER-003: Fensterinhalte MÜSSEN über gültige Surface-Referenzen eingebunden werden.
- RENDER-004: Zusammengehörige Fenster- und Surface-Änderungen MÜSSEN konsistent übernommen werden.
- RENDER-005: Transparenz und Alpha-Blending MÜSSEN unterstützt werden.
- RENDER-006: Fensterdekorationen MÜSSEN unabhängig vom Anwendungsinhalt darstellbar sein.
- RENDER-007: Vollständig verdeckte Fenster SOLLEN keine unnötigen Kompositionsoperationen verursachen.
- RENDER-008: Unveränderte Fensterbereiche SOLLEN nicht erneut gerendert werden.
- RENDER-009: Fenstertransformationen MÜSSEN bei Clipping und Damage Tracking berücksichtigt werden.
- RENDER-010: Popups und Dialoge MÜSSEN ihre festgelegten Hierarchie- und Ebenenregeln einhalten.
- RENDER-011: Sicherheitskritische Systemoberflächen DÜRFEN nicht durch unberechtigte Fenster überlagert werden.
- RENDER-012: Grafische Effekte MÜSSEN bei Ressourcenknappheit kontrolliert reduzierbar sein.
- RENDER-013: Direct Scanout SOLL bei geeigneten Vollbildfenstern bevorzugt werden.
- RENDER-014: Unterschiedliche Display-Skalierungen und Farbräume MÜSSEN berücksichtigt werden.
- RENDER-015: Fensteränderungen DÜRFEN keine unnötige vollständige Desktopkomposition erzwingen.
- RENDER-016: Fehlerhafte Fensterinhalte DÜRFEN den Compositor nicht destabilisieren.
- RENDER-017: Die Komposition MUSS mit Nova Compositor Scheduling und Buffer Synchronization abgestimmt sein.
- RENDER-018: Bei fehlender GPU-Beschleunigung MUSS eine grundlegende Fensterkomposition möglich bleiben.

## 5. Kompositionsmodell

### Window Content
Grafische Inhalte, die von Anwendungen über Surfaces bereitgestellt werden.

### Window Decoration
Fensterrahmen, Titelleisten und weitere systemseitige Darstellungselemente.

### Window Layer
Definiert die visuelle Reihenfolge entsprechend den Regeln des Window Managers.

### Window Effects
Zusätzliche Darstellungseffekte wie Schatten, Transparenz, Blur und Acrylic.

### Window Group
Zusammengehörige Fenster und Oberflächen mit definierten Hierarchie- und Darstellungsbeziehungen.

### System Overlay
Geschützte Systemoberflächen mit gesonderten Darstellungs- und Sicherheitsregeln.

## 6. Kompositionsablauf

1. **Collect:** Fensterzustände und Surface-Änderungen erfassen.
2. **Update:** Szenenstruktur und Ebenenreihenfolge aktualisieren.
3. **Analyze:** Sichtbarkeit, Überlagerungen und Damage-Regionen bestimmen.
4. **Optimize:** Direct Scanout, Hardware-Overlays oder reguläre Komposition auswählen.
5. **Compose:** Sichtbare Fensterinhalte und Dekorationen zusammensetzen.
6. **Effects:** Erforderliche visuelle Effekte verarbeiten.
7. **Color:** Farbtransformationen für das Ziel-Display durchführen.
8. **Synchronize:** Buffer- und GPU-Operationen synchronisieren.
9. **Present:** Fertigen Frame zur Bildausgabe übergeben.

Nicht erforderliche Verarbeitungsschritte SOLLEN übersprungen werden.

## 7. Performance und Zuverlässigkeit

- GPU-beschleunigte Fensterkomposition.
- Zero-Copy-Verarbeitung kompatibler Grafikbuffer.
- Occlusion Culling für verdeckte Fensterbereiche.
- Partielle Aktualisierung durch Damage Tracking.
- Wiederverwendung unveränderter Fensterinhalte.
- Minimierung zusätzlicher Zwischenbuffer.
- Optimierung von Vollbild- und maximierten Fenstern.
- Adaptive Qualität aufwendiger Grafikeffekte.
- Priorisierung interaktiver Fensteraktualisierungen.
- Unabhängige Komposition pro Display.
- Kontrollierter Software-Fallback.
- Wiederherstellung gültiger Fensterdarstellungen nach Compositor-Fehlern.

## 8. Ergebnis

NovaOS erhält eine leistungsfähige und flexible Fensterkomposition.

Fenster, Dialoge, Popups und Systemoberflächen werden entsprechend ihrer Hierarchie, Sichtbarkeit und Ebenenreihenfolge effizient zusammengesetzt.

Die Architektur ermöglicht moderne Fluent- und Acrylic-Oberflächen bei gleichzeitig geringer Eingabelatenz, hoher Bildqualität und ressourcenschonender Darstellung.