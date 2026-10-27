# NPSPEC-GRAPHICS-PERF-0002 – Nova Render Batching

**Status:** Angenommen  
**Bereich:** Graphics / Performance / Render Batching  
**System:** NovaOS

## 1. Zweck

Nova Render Batching definiert die Zusammenfassung kompatibler Rendering-Operationen zu größeren Verarbeitungseinheiten.

Ziel ist die Reduzierung von Draw Calls, Zustandswechseln, CPU-Overhead und GPU-Kommandos.

Das System optimiert 2D- und 3D-Rendering, ohne die visuelle Korrektheit, Ressourcenisolation oder deterministische Ausführung zu beeinträchtigen.

## 2. Architektur

Das Subsystem besteht aus folgenden Komponenten:

| Komponente | Aufgabe |
|---|---|
| Batch Manager | Koordination der Rendering-Batches |
| Batch Builder | Zusammenfassung kompatibler Zeichenoperationen |
| State Tracker | Überwachung relevanter Rendering-Zustände |
| Batch Scheduler | Planung der Batch-Ausführung |
| Instance Manager | Verwaltung instanzierter Zeichenoperationen |
| Batch Buffer Manager | Verwaltung temporärer Vertex- und Indexdaten |

Die Komponenten arbeiten oberhalb der Nova Graphics Backend Abstraction.

Die eigentliche Ausführung erfolgt über Nova GPU Command Submission beziehungsweise den CPU Renderer.

## 3. Batch-Modell

Ein Rendering-Batch enthält mehrere kompatible Zeichenoperationen.

Ein Batch besitzt mindestens:

- Rendering-Pipeline.
- Render Target.
- Ressourcenbindungen.
- Geometrie- beziehungsweise Instanzdaten.
- Rendering-Zustände.
- Zeichenoperationen.
- Abhängigkeits- und Synchronisationsinformationen.

Die Zusammenfassung DARF keine sichtbaren Unterschiede gegenüber der korrekten Einzelausführung verursachen.

## 4. Batching-Verfahren

NovaOS unterstützt folgende Verfahren:

| Verfahren | Beschreibung |
|---|---|
| Dynamic Batching | Zusammenfassung kleiner dynamischer Geometrien |
| Static Batching | Wiederverwendung vorbereiteter Geometriegruppen |
| Instanced Rendering | Mehrfache Darstellung gleicher Geometrien |
| Sprite Batching | Gemeinsame Verarbeitung von Sprites und Bildern |
| Glyph Batching | Zusammenfassung von Textglyphen |
| Primitive Batching | Gemeinsame Verarbeitung einfacher 2D-Primitiven |

Die tatsächlich verwendeten Verfahren werden anhand der Rendering-Anforderungen und Backend-Capabilities ausgewählt.

## 5. Batch-Kompatibilität

Zeichenoperationen dürfen zusammengefasst werden, wenn ihre relevanten Zustände kompatibel sind.

Dazu gehören:

- Rendering-Pipeline und Shader.
- Render Target und Pixelformat.
- Texturen und Sampler.
- Blend- und Depth-/Stencil-Zustände.
- Clipping und Scissor.
- Vertex- und Indexformate.
- Ressourcenabhängigkeiten.

Unterschiedliche Parameter können über Instanzdaten oder geeignete Buffer übergeben werden.

Nicht kompatible Zustände erzwingen eine Batch-Grenze.

## 6. Zeichenreihenfolge

Die ursprüngliche Darstellungssemantik MUSS erhalten bleiben.

Festlegungen:

- Transparente Zeichenoperationen behalten ihre erforderliche Reihenfolge.
- Abhängige Rendering-Operationen DÜRFEN nicht unzulässig umgeordnet werden.
- Zustandsbasierte Sortierung ist nur innerhalb entsprechend freigegebener Bereiche zulässig.
- Explizite Synchronisationspunkte bilden Batch-Grenzen.
- Die Reihenfolge von UI-Elementen MUSS erhalten bleiben.

Optimierungen DÜRFEN keine unbeabsichtigten Überlagerungs- oder Blending-Fehler verursachen.

## 7. Instanced Rendering

Instanced Rendering ermöglicht die mehrfache Darstellung identischer Geometrien mit unterschiedlichen Parametern.

Instanzdaten können enthalten:

- Transformationsmatrizen.
- Farben.
- Materialparameter.
- Texturkoordinaten.
- Objektkennungen.

Instancing SOLL verwendet werden, wenn dadurch weniger Rendering-Kommandos oder geringerer Verarbeitungsaufwand entstehen.

Die maximale Instanzanzahl richtet sich nach den Backend-Capabilities.

## 8. Buffer Management

Batch-Daten werden über wiederverwendbare Speicherbereiche bereitgestellt.

Unterstützt werden:

- Dynamische Vertex- und Indexbuffer.
- Instanzbuffer.
- Ringbuffer.
- Wiederverwendbare Staging-Bereiche.
- Bedarfsgerechte Buffer-Erweiterung.
- Synchronisierte Buffer-Wiederverwendung.

Speicherallokationen innerhalb zeitkritischer Rendering-Pfade SOLLEN minimiert werden.

Buffer DÜRFEN nicht überschrieben werden, solange ihre Daten noch verwendet werden.

## 9. Automatisches Batching

NovaOS kann kompatible Rendering-Operationen automatisch zusammenfassen.

Dabei berücksichtigt das System:

- Anzahl der Draw Calls.
- Größe der Geometrien.
- Kosten von Zustandswechseln.
- Speicherbedarf.
- Verfügbare Backend-Funktionen.
- Aufwand der Batch-Erstellung.

Batching SOLL unterbleiben, wenn dessen Verwaltungsaufwand den erwarteten Nutzen übersteigt.

Anwendungen können explizite Batch-Grenzen definieren.

## 10. CPU- und GPU-Rendering

### GPU-Rendering

GPU-Backends nutzen geeignete Verfahren zur Reduzierung von Draw Calls und Zustandswechseln.

### CPU-Rendering

Der CPU Renderer kann kompatible Operationen gemeinsam vorbereiten und verarbeiten.

Beide Varianten verwenden dieselben Regeln für Kompatibilität, Zeichenreihenfolge und Ressourcenabhängigkeiten.

Eine identische interne Batch-Struktur ist nicht erforderlich.

## 11. Integration

- **Nova Graphics API:** Bereitstellung der Zeichenoperationen.
- **Nova Graphics Backend Abstraction:** Auswahl geeigneter Batching-Verfahren.
- **Nova GPU Command Submission:** Übermittlung zusammengefasster Kommandos.
- **Nova Graphics Resource Lifecycle:** Verwaltung temporärer Buffer.
- **Nova Graphics Caching:** Wiederverwendung vorbereiteter Batch-Daten.
- **Nova 2D Rendering Engine:** Sprite-, Primitive- und Glyph-Batching.
- **Nova 3D Rendering Architecture:** Geometry Batching und Instancing.
- **Nova Compositor:** Optimierung kompatibler Oberflächenoperationen.

Das Render Batching bleibt eine interne Optimierung und verändert nicht die öffentlichen Rendering-Ergebnisse.

## 12. Normative Anforderungen

- **GRAPHICS-001:** NovaOS MUSS kompatible Rendering-Operationen zu Batches zusammenfassen können.
- **GRAPHICS-002:** Batch-Kompatibilität MUSS anhand relevanter Rendering-Zustände geprüft werden.
- **GRAPHICS-003:** Die korrekte Zeichenreihenfolge MUSS erhalten bleiben.
- **GRAPHICS-004:** Transparenz, Clipping und Blending MÜSSEN berücksichtigt werden.
- **GRAPHICS-005:** Nicht kompatible Operationen MÜSSEN getrennt verarbeitet werden.
- **GRAPHICS-006:** Explizite Batch-Grenzen MÜSSEN unterstützt werden.
- **GRAPHICS-007:** Sprite- und Primitive-Batching MÜSSEN verfügbar sein.
- **GRAPHICS-008:** Glyph-Batching MUSS unterstützt werden.
- **GRAPHICS-009:** Instanced Rendering SOLL bei geeigneten Backend-Capabilities verwendet werden.
- **GRAPHICS-010:** Temporäre Batch-Buffer MÜSSEN wiederverwendbar sein.
- **GRAPHICS-011:** Buffer-Zugriffe MÜSSEN korrekt synchronisiert werden.
- **GRAPHICS-012:** Batch-Größen MÜSSEN durch Ressourcenlimits begrenzt werden.
- **GRAPHICS-013:** Automatisches Batching SOLL den tatsächlichen Verarbeitungsaufwand berücksichtigen.
- **GRAPHICS-014:** Batching MUSS auch für CPU-basiertes Rendering möglich sein.
- **GRAPHICS-015:** Die Optimierung DARF die Korrektheit des Renderings nicht verändern.
- **GRAPHICS-016:** Die Verarbeitung MUSS deterministisch und ohne Netzwerk- oder KI-Abhängigkeit funktionieren.

## 13. Fehlerbehandlung

- Nicht kompatible Operationen werden in getrennten Batches verarbeitet.
- Überschrittene Buffer-Limits führen zur kontrollierten Aufteilung.
- Fehlgeschlagene Batch-Allokationen lösen eine kleinere Batch-Größe oder reguläre Einzelverarbeitung aus.
- Ungültige Ressourcenbindungen werden vor der Ausführung erkannt.
- GPU-Fehler werden gemäß Nova Graphics Driver Recovery behandelt.
- Fehlerhafte Batches DÜRFEN andere Rendering-Ressourcen nicht beschädigen.

## 14. Ergebnis

NovaOS erhält eine zentrale Architektur zur effizienten Zusammenfassung von Rendering-Operationen.

Draw Calls, Zustandswechsel und temporäre Speicheroperationen können reduziert werden, während die ursprüngliche Darstellungssemantik erhalten bleibt.

Die Architektur unterstützt sowohl CPU-Rendering als auch moderne GPU-Techniken wie Instanced Rendering.

**Grundsatz:** NovaOS fasst Rendering-Arbeit zusammen, wo dies effizient möglich ist, ohne die Korrektheit oder Reihenfolge der Darstellung zu verändern.