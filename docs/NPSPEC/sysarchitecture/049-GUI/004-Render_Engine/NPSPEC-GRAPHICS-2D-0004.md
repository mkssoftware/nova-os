# NPSPEC-GRAPHICS-2D-0004 – Nova Image Rendering

**Status:** Angenommen  
**Bereich:** Graphics / 2D / Image Rendering  
**System:** NovaOS

## 1. Zweck

Nova Image Rendering definiert die Darstellung und Verarbeitung rasterbasierter Bilder innerhalb der Nova 2D Rendering Engine.

Ziel ist eine schnelle, speichereffiziente und qualitativ hochwertige Bilddarstellung für Nova UI, Anwendungen, Desktopoberflächen und grafische Inhalte.

Die Bilddarstellung erfolgt unabhängig vom verwendeten CPU- oder GPU-Backend und unterstützt unterschiedliche Auflösungen, Pixelformate und Farbräume.

## 2. Architektur

Das Image Rendering System besteht aus folgenden Komponenten:

| Komponente | Aufgabe |
|---|---|
| Image Renderer | Darstellung rasterbasierter Bilder |
| Image Resource | Backendunabhängige Repräsentation von Bilddaten |
| Image Sampler | Abtastung und Filterung von Bildpixeln |
| Image Transformer | Skalierung und geometrische Transformationen |
| Image Compositor | Transparenz und Bildüberlagerung |
| Image Cache | Wiederverwendung vorbereiteter Bilddaten |
| Image Upload Manager | Übertragung zwischen CPU- und GPU-Speicher |

Die Engine verwendet die Nova Graphics API und die Graphics Backend Abstraction.

Das Decodieren und Encodieren von Bilddateien erfolgt über separate Komponenten des Nova Framework.

## 3. Bildmodell

Ein Bild besitzt mindestens folgende Eigenschaften:

| Eigenschaft | Beschreibung |
|---|---|
| Image ID | Eindeutige Ressourcenidentität |
| Width | Bildbreite |
| Height | Bildhöhe |
| Pixel Format | Format der Bilddaten |
| Color Space | Verwendeter Farbraum |
| Alpha Mode | Transparenzdarstellung |
| Row Stride | Speicherabstand zwischen Bildzeilen |
| Storage Type | CPU-, GPU- oder gemeinsam nutzbarer Speicher |
| Resource State | Aktueller Ressourcenzustand |

Bildressourcen werden gemäß Nova Graphics Resource Lifecycle verwaltet.

Bilddaten und deren Darstellungseinstellungen MÜSSEN unabhängig voneinander verwaltet werden können.

## 4. Pixelformate

Das Image Rendering System unterstützt ein erweiterbares Pixelformatmodell.

Grundlegende Formate:

- R8
- RGB565
- RGB888
- BGR888
- RGBA8888
- BGRA8888
- RGB16F
- RGBA16F

Weitere Formate können über Backend-Capabilities bereitgestellt werden.

Nicht unterstützte Formate MÜSSEN erkannt und bei Bedarf kontrolliert konvertiert werden.

Die Kanalreihenfolge und das Speicherlayout DÜRFEN nicht implizit vorausgesetzt werden.

## 5. Bilddarstellung

Die Engine unterstützt:

- Darstellung vollständiger Bilder.
- Darstellung einzelner Bildbereiche.
- Positionierung in logischen Koordinaten.
- Skalierung.
- Rotation.
- Spiegelung.
- Zuschneiden.
- Transparenz.
- Farbmodulation.
- Clipping.
- Überlagerung mehrerer Bilder.

Quell- und Zielbereiche werden unabhängig voneinander definiert.

Zeichenoperationen MÜSSEN die Transformationen und Clipping-Regeln des Drawing Context berücksichtigen.

## 6. Skalierung und Filterung

Für die Bildskalierung werden folgende Filterverfahren unterstützt:

| Verfahren | Verwendung |
|---|---|
| Nearest Neighbor | Pixelgenaue Skalierung |
| Bilinear | Schnelle, geglättete Skalierung |
| Bicubic | Hochwertige Skalierung |
| Lanczos | Qualitätsorientierte Skalierung |
| Mipmapping | Effiziente Darstellung verkleinerter Bilder |

Nicht jedes Backend MUSS sämtliche Filter hardwarebeschleunigt unterstützen.

Die Auswahl erfolgt anhand der gewünschten Darstellungsqualität und der verfügbaren Funktionen.

Pixelgenaue Skalierung MUSS ohne Interpolation möglich sein.

## 7. Transparenz und Blending

Das Image Rendering System unterstützt:

- Bilder ohne Alphakanal.
- Straight Alpha.
- Premultiplied Alpha.
- Globale Transparenz.
- Alpha-Blending.
- Unterschiedliche Blending-Modi.

Die Verarbeitung MUSS den jeweiligen Alpha-Modus berücksichtigen.

Unterschiedliche Alpha-Repräsentationen DÜRFEN nicht ohne erforderliche Konvertierung miteinander vermischt werden.

Blending-Operationen müssen die definierten Farbraumregeln berücksichtigen.

## 8. Farbraumverarbeitung

Die Bilddarstellung verwendet das zentrale Nova Graphics Color System.

Unterstützt werden:

- Farbraumzuordnung.
- Farbkonvertierung.
- Darstellung unterschiedlicher Farbtiefen.
- Lineare Farbverarbeitung.
- HDR-Bilddaten, sofern unterstützt.
- Anpassung an das Rendering-Ziel.

Die ursprünglichen Bilddaten SOLLEN bei Farbkonvertierungen unverändert bleiben.

Konvertierungen werden vorzugsweise während der Darstellung oder über geeignete zwischengespeicherte Ressourcen durchgeführt.

## 9. CPU- und GPU-Rendering

### CPU Rendering

Der CPU Renderer übernimmt:

- Softwarebasierte Bilddarstellung.
- Skalierung und Filterung.
- Farbkonvertierung.
- Alpha-Blending.
- Speicherbasierte Bildoperationen.

### GPU Rendering

Der GPU Renderer ermöglicht:

- Texturbasierte Bilddarstellung.
- Hardwarebeschleunigte Skalierung.
- Shaderbasierte Bildverarbeitung.
- Parallele Bildoperationen.
- Hardwarebeschleunigtes Blending.

Die Auswahl erfolgt über Nova GPU and CPU Rendering.

Ein GPU-Ausfall DARF nicht grundsätzlich verhindern, dass unterstützte Bilder über die CPU dargestellt werden.

## 10. Bildspeicher und Caching

Das System unterstützt:

- CPU-lokale Bildressourcen.
- GPU-Texturen.
- Gemeinsam nutzbare Speicherbereiche.
- Zero-Copy bei geeigneter Hardware.
- Asynchrone Bildübertragungen.
- Wiederverwendung konvertierter Bilddaten.
- Caching skalierter Bilder.
- Partielle Bildaktualisierungen.

Caches MÜSSEN bei Änderungen der zugrunde liegenden Bilddaten invalidiert werden.

Speicherverbrauch und Cachegröße unterliegen den zentralen Ressourcenrichtlinien.

Unnötige CPU-GPU-Übertragungen SOLLEN vermieden werden.

## 11. Große Bilder und Teilbereiche

Die Engine unterstützt die Darstellung von Bildern, deren vollständige Verarbeitung hohe Speicheranforderungen verursachen würde.

Dafür werden vorgesehen:

- Teilbereichsrendering.
- Tile-basierte Verarbeitung.
- Sichtbarkeitsprüfung.
- Partielle Speicherübertragung.
- Bedarfsgerechte Ressourcenbereitstellung.

Die Engine SOLL nur die für die aktuelle Darstellung erforderlichen Bildbereiche verarbeiten.

Das Laden und Decodieren einzelner Bildbereiche erfolgt über geeignete Bildverarbeitungskomponenten des Nova Framework.

## 12. Integration in NovaOS

### Nova 2D Rendering Engine

Stellt die grundlegenden Zeichenoperationen und Drawing Contexts bereit.

### Nova UI

Verwendet Image Rendering für Symbole, Hintergrundbilder, Controls und grafische Inhalte.

### Nova Compositor

Verwendet Bildressourcen für geeignete Kompositions- und Darstellungsvorgänge.

### Nova Graphics Resource Manager

Verwaltet Bildressourcen und deren Lebenszyklus.

### Nova Framework

Stellt die Bilddarstellung plattformübergreifend bereit.

Bilddateiformate, Bilddecodierung und Bildbearbeitung werden durch separate Framework-Komponenten ergänzt.

## 13. Normative Anforderungen

- GRAPHICS-001: NovaOS MUSS eine backendunabhängige Bilddarstellung bereitstellen.
- GRAPHICS-002: Bildressourcen MÜSSEN eindeutig identifizierbar sein.
- GRAPHICS-003: Die Engine MUSS unterschiedliche Pixelformate unterstützen.
- GRAPHICS-004: Pixelformate und Speicherlayouts MÜSSEN explizit berücksichtigt werden.
- GRAPHICS-005: Bilder MÜSSEN skaliert, rotiert und zugeschnitten dargestellt werden können.
- GRAPHICS-006: Nearest-Neighbor- und bilineare Filterung MÜSSEN unterstützt werden.
- GRAPHICS-007: Transparenz und Alpha-Blending MÜSSEN unterstützt werden.
- GRAPHICS-008: Unterschiedliche Alpha-Modi MÜSSEN korrekt verarbeitet werden.
- GRAPHICS-009: Die Darstellung MUSS die zentrale Farbverwaltung berücksichtigen.
- GRAPHICS-010: Bilder MÜSSEN ohne GPU-Beschleunigung darstellbar sein.
- GRAPHICS-011: GPU-Beschleunigung SOLL bei geeigneter Hardware verwendet werden.
- GRAPHICS-012: Bildressourcen MÜSSEN gemäß Nova Graphics Resource Lifecycle verwaltet werden.
- GRAPHICS-013: Bilddaten und Darstellungseinstellungen MÜSSEN getrennt verwaltet werden können.
- GRAPHICS-014: Partielle Bildaktualisierungen SOLLEN unterstützt werden.
- GRAPHICS-015: Unnötige Speicherübertragungen SOLLEN vermieden werden.
- GRAPHICS-016: Ungültige Bilddaten DÜRFEN keine unkontrollierten Speicherzugriffe verursachen.
- GRAPHICS-017: Die Bilddarstellung MUSS Bestandteil des portablen Nova Framework sein können.
- GRAPHICS-018: Image Rendering MUSS deterministisch und ohne Netzwerk- oder KI-Abhängigkeit funktionieren.

## 14. Fehlerbehandlung

- Ungültige Bildabmessungen werden zurückgewiesen.
- Nicht unterstützte Pixelformate werden eindeutig gemeldet.
- Speichergrenzen und Row Strides MÜSSEN vor Bildzugriffen validiert werden.
- Fehlgeschlagene Bildressourcen werden nicht als gültig veröffentlicht.
- Fehlerhafte Bilddaten DÜRFEN andere Grafikressourcen nicht beschädigen.
- Bei GPU-Ausfall SOLL die Darstellung über den CPU Renderer fortgesetzt werden.
- Nicht wiederherstellbare Bildressourcen werden den betroffenen Grafikclients gemeldet.

## 15. Ergebnis

NovaOS erhält eine leistungsfähige und portable Bilddarstellung für rasterbasierte Grafiken.

Bilder können unabhängig von ihrer ursprünglichen Auflösung, ihrem Pixelformat und dem verwendeten Rendering-Backend dargestellt, transformiert und kombiniert werden.

Durch GPU-Beschleunigung, effiziente Speicherverwaltung und bedarfsgerechte Bildverarbeitung wird eine flüssige Darstellung bei möglichst geringem Ressourcenverbrauch ermöglicht.

**Grundsatz:** Bilddaten bleiben unabhängig von ihrer Darstellung. NovaOS verarbeitet nur die erforderlichen Inhalte und nutzt die verfügbare Hardware möglichst effizient.