# NPSPEC-GRAPHICS-3D-0002 – Nova 3D Graphics API

**Status:** Angenommen  
**Bereich:** Graphics / 3D / API  
**System:** NovaOS

## 1. Zweck

Nova 3D Graphics API definiert die einheitliche Programmierschnittstelle für dreidimensionale Grafikoperationen innerhalb von NovaOS und des portablen Nova Framework.

Ziel ist eine leistungsfähige, hardwareunabhängige API für 3D-Anwendungen, CAD, Simulationen und grafische Benutzeroberflächen.

Die API abstrahiert die zugrunde liegenden Grafik-Backends, ohne deren leistungsfähige Funktionen unnötig einzuschränken.

## 2. Architektur

Die Nova 3D Graphics API baut auf der Nova Graphics API und der Graphics Backend Abstraction auf.

Sie stellt folgende Schnittstellen bereit:

| Schnittstelle | Aufgabe |
|---|---|
| Graphics3DDevice | Zugriff auf 3D-Funktionen und Gerätefähigkeiten |
| RenderContext3D | Verwaltung eines Rendering-Kontexts |
| CommandBuffer3D | Aufzeichnung von Rendering-Kommandos |
| RenderPipeline3D | Konfiguration der Rendering-Pipeline |
| Mesh | Beschreibung dreidimensionaler Geometrien |
| Material | Verwaltung von Oberflächeneigenschaften |
| Shader | Programmierbare Grafikverarbeitung |
| Camera3D | Kamera- und Projektionsparameter |
| RenderTarget3D | Ziel einer Rendering-Operation |

Die Schnittstellen verwenden die zentrale Grafikressourcenverwaltung.

Backendabhängige Implementierungsdetails bleiben gekapselt.

## 3. API-Modell

Die API unterstützt zwei Zugriffsebenen:

### High-Level API

Stellt vereinfachte Funktionen bereit für:

- Darstellung von Meshes.
- Materialien und Texturen.
- Kameras und Projektionen.
- Lichtquellen.
- Szenen und Objekttransformationen.

### Low-Level API

Ermöglicht direkten Zugriff auf:

- Vertex- und Indexbuffer.
- Shader und Pipeline States.
- Render Passes.
- Command Buffers.
- Ressourcenbindungen.
- Synchronisation.

Beide Ebenen verwenden dieselben grundlegenden Grafikressourcen.

Die High-Level API DARF keine zwingende Voraussetzung für die Low-Level API sein.

## 4. Geräte und Capabilities

`Graphics3DDevice` stellt die verfügbaren Funktionen des Rendering-Backends bereit.

Dazu gehören:

- Unterstützte Shader-Funktionen.
- Maximale Textur- und Buffergrößen.
- Unterstützte Pixelformate.
- Multisampling.
- Instancing.
- Compute-Funktionen.
- Hardware-Raytracing, sofern vorhanden.
- Speicher- und Ressourcenlimits.

Anwendungen MÜSSEN benötigte Funktionen vor deren Verwendung abfragen können.

Nicht unterstützte Funktionen werden eindeutig gemeldet.

## 5. Geometrie-API

Die API unterstützt:

- Vertex Buffer.
- Index Buffer.
- Vertex Layouts.
- Meshes.
- Primitive Topologies.
- Instanced Drawing.
- Indexed Drawing.

Grundlegende Primitive Topologies:

- Point List.
- Line List.
- Line Strip.
- Triangle List.
- Triangle Strip.

Vertex-Attribute werden über explizite Formate und Speicherlayouts beschrieben.

Die API MUSS Geometrien unabhängig vom verwendeten Rendering-Backend verwalten können.

## 6. Transformationen und Kameras

Die API verwendet das Koordinaten- und Matrixmodell der Nova 3D Rendering Architecture.

Bereitgestellt werden:

- 4×4-Transformationsmatrizen.
- Translation, Rotation und Skalierung.
- Hierarchische Transformationen.
- View-Matrizen.
- Projektionsmatrizen.
- Perspektivische Projektion.
- Orthografische Projektion.

Kamera- und Transformationsfunktionen sind unabhängig von der Szenenverwaltung verwendbar.

Backendabhängige Unterschiede im Clip-Space werden durch die Grafikabstraktion berücksichtigt.

## 7. Shader und Materialien

Die API stellt Schnittstellen zur Verwaltung von Shadern und Materialien bereit.

Unterstützt werden:

- Shader-Module.
- Shader-Parameter.
- Ressourcenbindungen.
- Texturen und Sampler.
- Materialeigenschaften.
- Pipeline States.

Shader werden über das Nova Shader System verarbeitet.

Die API definiert die Verwendung von Shadern, nicht deren Programmiersprache oder Compilerarchitektur.

Materialien MÜSSEN unabhängig von einzelnen Meshes wiederverwendbar sein.

## 8. Rendering-Kommandos

Rendering-Operationen werden über Command Buffers beschrieben.

Grundlegende Operationen:

- `BeginRenderPass`
- `EndRenderPass`
- `SetPipeline`
- `BindVertexBuffer`
- `BindIndexBuffer`
- `BindResources`
- `SetViewport`
- `SetScissor`
- `Draw`
- `DrawIndexed`
- `DrawInstanced`
- `Submit`

Die Kommandos werden über Nova GPU Command Submission ausgeführt.

Ihre Reihenfolge und Ressourcenabhängigkeiten MÜSSEN eindeutig definiert sein.

## 9. Render Targets und Rendering-Pipelines

Die API unterstützt:

- Rendering in Fenster-Surfaces.
- Offscreen-Rendering.
- Rendering in Texturen.
- Mehrere Color Attachments.
- Depth- und Stencil-Buffer.
- Multisampling.
- Mehrere Render Passes.

Pipeline States beschreiben insbesondere:

- Shader-Konfiguration.
- Primitive Topology.
- Rasterizer State.
- Depth- und Stencil State.
- Blending.
- Render-Target-Formate.

Pipeline States SOLLEN als wiederverwendbare Ressourcen verwaltet werden.

## 10. Ressourcen und Synchronisation

Alle GPU-Ressourcen unterliegen dem Nova Graphics Resource Lifecycle.

Die API unterstützt:

- Explizite Ressourcenhandles.
- Kontrollierte Ressourcenfreigabe.
- CPU-GPU-Datenübertragung.
- Synchronisationsobjekte.
- Ressourcenstatus und Zugriffsrechte.
- Asynchrone Kommandoausführung.

Ungültige Ressourcenhandles MÜSSEN erkannt werden.

Die API DARF keine unkontrollierten Zugriffe auf fremde Grafikressourcen ermöglichen.

## 11. CPU- und GPU-Backends

Die API verwendet die Nova Graphics Backend Abstraction.

GPU-Backends können hardwarebeschleunigte Rendering-Funktionen bereitstellen.

Der CPU Renderer unterstützt einen definierten grundlegenden Funktionsumfang.

Anwendungen können über Capabilities erkennen, welche Funktionen verfügbar sind.

Ein Backendwechsel DARF keine Änderung der grundlegenden API-Schnittstellen erfordern.

## 12. Normative Anforderungen

- **GRAPHICS-001:** NovaOS MUSS eine einheitliche 3D Graphics API bereitstellen.
- **GRAPHICS-002:** Die API MUSS unabhängig von konkreten Grafiktreibern sein.
- **GRAPHICS-003:** High-Level- und Low-Level-Zugriffe MÜSSEN unterstützt werden.
- **GRAPHICS-004:** Die Low-Level API MUSS unabhängig von der Szenenverwaltung verwendbar sein.
- **GRAPHICS-005:** Vertex- und Indexbuffer MÜSSEN unterstützt werden.
- **GRAPHICS-006:** Instanced und Indexed Drawing MÜSSEN möglich sein.
- **GRAPHICS-007:** Transformationen und Projektionen MÜSSEN verfügbar sein.
- **GRAPHICS-008:** Shader, Materialien und Pipeline States MÜSSEN verwaltet werden können.
- **GRAPHICS-009:** Rendering-Kommandos MÜSSEN über Command Buffers ausführbar sein.
- **GRAPHICS-010:** Offscreen-Rendering und Render-to-Texture MÜSSEN unterstützt werden.
- **GRAPHICS-011:** Depth Testing und Blending MÜSSEN verfügbar sein.
- **GRAPHICS-012:** Backend-Capabilities MÜSSEN abfragbar sein.
- **GRAPHICS-013:** Ressourcen MÜSSEN gemäß Nova Graphics Resource Lifecycle verwaltet werden.
- **GRAPHICS-014:** Ressourcenabhängigkeiten und Synchronisation MÜSSEN eindeutig definiert sein.
- **GRAPHICS-015:** Ein grundlegender CPU-Rendering-Pfad MUSS verfügbar sein.
- **GRAPHICS-016:** Die API DARF keine zwingende Abhängigkeit vom Window Manager oder Compositor besitzen.
- **GRAPHICS-017:** Die Schnittstellen MÜSSEN Bestandteil des portablen Nova Framework sein.
- **GRAPHICS-018:** Die API MUSS deterministisch und ohne Netzwerk- oder KI-Abhängigkeit funktionieren.

## 13. Fehlerbehandlung

- Ungültige Geräte- und Ressourcenhandles werden zurückgewiesen.
- Nicht unterstützte Funktionen werden eindeutig gemeldet.
- Fehlerhafte Pipeline-Konfigurationen werden vor ihrer Ausführung erkannt.
- Ungültige Bufferbereiche DÜRFEN keine unkontrollierten Speicherzugriffe verursachen.
- Fehlgeschlagene Kommandoausführungen werden an den aufrufenden Kontext gemeldet.
- Bei GPU-Ausfall erfolgt die Wiederherstellung gemäß Nova Graphics Driver Recovery.
- Ein CPU-Fallback wird verwendet, sofern die angeforderten Funktionen unterstützt werden.

## 14. Ergebnis

NovaOS erhält eine einheitliche, portable und erweiterbare Programmierschnittstelle für dreidimensionale Grafik.

Anwendungen können Geometrien, Shader, Materialien und Rendering-Pipelines verwenden, ohne direkt von einer bestimmten GPU oder einem Grafiktreiber abhängig zu sein.

Die Trennung zwischen High-Level- und Low-Level-API ermöglicht sowohl einfache 3D-Anwendungen als auch leistungsoptimierte Rendering-Engines.

**Grundsatz:** Die Nova 3D Graphics API definiert, was und wie gerendert werden soll. Das Grafik-Backend entscheidet, wie diese Operationen auf der verfügbaren Hardware ausgeführt werden.