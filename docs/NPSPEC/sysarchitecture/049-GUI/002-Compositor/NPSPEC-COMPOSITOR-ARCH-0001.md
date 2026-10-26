# NPSPEC-COMPOSITOR-ARCH-0001 – Nova Compositor Architecture

**Status:** Angenommen  
**Bereich:** Compositor / Grafiksystem  
**System:** NovaOS

## 1. Zweck

Nova Compositor Architecture definiert die Architektur der Bildkomposition innerhalb von NovaOS.

Ziel ist eine performante, latenzarme und ressourcenschonende Darstellung sämtlicher grafischer Oberflächen mit Unterstützung moderner visueller Effekte.

## 2. Architektur

- Eigenständiger, modularer Systemdienst außerhalb des Kernels.
- Zusammenarbeit mit Nova Display Server und Window Manager.
- Trennung zwischen Fensterverwaltung, Rendering und Bildkomposition.
- Hardwarebeschleunigte Komposition mit Software-Fallback.
- Asynchrone, ereignisgesteuerte Verarbeitung.
- Unterstützung mehrerer Displays und Grafikkarten.
- Verwaltung grafischer Oberflächen über das Nova Display Protocol.
- Unabhängigkeit von Nova UI und einzelnen Anwendungen.
- Austauschbare Rendering-Backends.
- Integration in Nova Display Lifecycle und Recovery.

## 3. Kernfunktionen

- Zusammensetzen mehrerer grafischer Oberflächen.
- Verwaltung von Surface-Reihenfolge und Sichtbarkeit.
- Verarbeitung von Transparenz und Alpha-Blending.
- Darstellung von Schatten, Unschärfe und Acrylic-Effekten.
- Unterstützung von Transformationen und Animationen.
- Verwaltung von Clipping und sichtbaren Bereichen.
- Optimierung durch Dirty Regions und Occlusion Culling.
- Synchronisierung der Frame-Ausgabe.
- Unterstützung von Direct Scanout und Hardware-Overlays.
- Unabhängige Bildkomposition pro Display.
- Farbkorrekte Verarbeitung von SDR- und HDR-Inhalten.
- Verwaltung von GPU- und CPU-Rendering-Pfaden.

## 4. Normative Anforderungen

- COMPOSITOR-001: Der Compositor MUSS unabhängig vom Window Manager betrieben werden können.
- COMPOSITOR-002: Grafische Oberflächen MÜSSEN über definierte Ressourcen-Handles eingebunden werden.
- COMPOSITOR-003: Die Komposition MUSS unterschiedliche Transparenz- und Ebeneneigenschaften unterstützen.
- COMPOSITOR-004: Die Reihenfolge grafischer Oberflächen MUSS eindeutig definiert sein.
- COMPOSITOR-005: Hardwarebeschleunigte Komposition SOLL bevorzugt werden.
- COMPOSITOR-006: Ein funktionsfähiger Software-Fallback MUSS verfügbar sein.
- COMPOSITOR-007: Unveränderte Bildbereiche SOLLEN nicht unnötig neu berechnet werden.
- COMPOSITOR-008: Vollständig verdeckte Oberflächen SOLLEN keine unnötigen Kompositionsoperationen verursachen.
- COMPOSITOR-009: Die Frame-Ausgabe MUSS mit dem jeweiligen Display synchronisiert werden können.
- COMPOSITOR-010: Unterschiedliche Bildwiederholraten MÜSSEN unabhängig unterstützt werden.
- COMPOSITOR-011: Direct Scanout SOLL verwendet werden, wenn keine Komposition erforderlich ist und alle Sicherheits- und Ausgabeanforderungen erfüllt sind.
- COMPOSITOR-012: Grafische Effekte MÜSSEN bei Ressourcenknappheit kontrolliert reduziert werden können.
- COMPOSITOR-013: SDR- und HDR-Oberflächen MÜSSEN farbkonsistent verarbeitet werden.
- COMPOSITOR-014: Anwendungen DÜRFEN keine fremden Oberflächen ohne entsprechende Capability manipulieren.
- COMPOSITOR-015: Fehler einzelner Clients DÜRFEN den Compositor nicht destabilisieren.
- COMPOSITOR-016: Der Compositor MUSS nach einem Ausfall kontrolliert wiederhergestellt werden können.
- COMPOSITOR-017: Kritische Systemoberflächen MÜSSEN gegenüber normalen Anwendungsoberflächen priorisiert werden.
- COMPOSITOR-018: Die Bildkomposition MUSS ohne zwingende KI-Abhängigkeit funktionieren.

## 5. Architekturkomponenten

### Surface Manager
Verwaltet eingebundene Oberflächen, deren Eigenschaften und Lebenszyklen.

### Scene Manager
Verwaltet die darzustellende Szenenstruktur, Ebenen und Transformationen.

### Composition Engine
Berechnet die endgültige Bildkomposition aus sichtbaren Oberflächen.

### Effect Engine
Verarbeitet Transparenz, Schatten, Blur, Acrylic und weitere visuelle Effekte.

### Frame Scheduler
Koordiniert Rendering, Frame-Timing und Präsentationszeitpunkte.

### Damage Tracker
Ermittelt veränderte Bildbereiche und reduziert unnötige Aktualisierungen.

### Rendering Backend
Stellt hardwarebeschleunigte oder softwarebasierte Kompositionsverfahren bereit.

### Output Manager
Übergibt fertig komponierte Frames an den Nova Display Server.

## 6. Kompositionsablauf

1. **Collect:** Verfügbare Oberflächen und Änderungen erfassen.
2. **Update:** Szenenstruktur und Oberflächeneigenschaften aktualisieren.
3. **Analyze:** Sichtbarkeit, Überlagerungen und veränderte Bereiche bestimmen.
4. **Schedule:** Erforderliche Kompositionsoperationen planen.
5. **Compose:** Oberflächen und Effekte zusammensetzen.
6. **Synchronize:** GPU-Operationen und Buffer-Zugriffe synchronisieren.
7. **Present:** Fertigen Frame an den Display Server übergeben.

Nicht erforderliche Verarbeitungsschritte SOLLEN übersprungen werden.

## 7. Performance und Zuverlässigkeit

- Zero-Copy-Verarbeitung, sofern technisch möglich.
- GPU-beschleunigte Bildkomposition.
- Wiederverwendung unveränderter Bildbereiche.
- Minimierung von Speichertransfers und Zwischenbuffern.
- Adaptive Effektqualität bei hoher Systemlast.
- Unabhängiges Frame-Scheduling pro Display.
- Vermeidung unnötiger Frames bei statischen Oberflächen.
- Priorisierung sichtbarer und interaktiver Inhalte.
- Kontrollierte Wiederherstellung verlorener Grafikressourcen.
- Software-Fallback bei Ausfall der Hardwarebeschleunigung.
- Begrenzung des Ressourcenverbrauchs einzelner Clients.
- Minimierung von Eingabe- und Präsentationslatenzen.

## 8. Ergebnis

NovaOS erhält einen modularen, leistungsfähigen und ausfallsicheren Compositor.

Die Architektur ermöglicht moderne Fluent- und Acrylic-Oberflächen, flüssige Animationen sowie eine effiziente Bildkomposition über mehrere Displays.

Durch adaptive Verarbeitung, Direct Scanout und bedarfsgerechtes Rendering bleibt die grafische Oberfläche auch auf leistungsschwächerer Hardware ressourcenschonend und reaktionsschnell.