# NPSPEC-GRAPHICS-ARCH-0002 – Nova Graphics API

**Status:** Angenommen  
**Bereich:** Graphics / Graphics API  
**System:** NovaOS

## 1. Zweck

Nova Graphics API definiert die einheitliche Programmierschnittstelle für zweidimensionale und dreidimensionale Grafikoperationen innerhalb von NovaOS.

Ziel ist eine leistungsfähige, hardwareunabhängige und erweiterbare Grafik-API, die Software-Rendering und GPU-Beschleunigung über dieselben grundlegenden Abstraktionen ermöglicht.

Die API bildet die Schnittstelle zwischen Anwendungen, Nova Framework, Nova UI und dem Graphics Core.

## 2. Architektur

Die Graphics API basiert auf folgenden Komponenten:

| Komponente | Aufgabe |
|---|---|
| Graphics Device API | Zugriff auf verfügbare Grafikgeräte |
| Graphics Resource API | Verwaltung von Buffern, Texturen und Renderzielen |
| Graphics Command API | Aufzeichnung und Übermittlung von Grafikbefehlen |
| Graphics 2D API | Zweidimensionale Grafikoperationen |
| Graphics 3D API | Dreidimensionale Grafikoperationen |
| Graphics Pipeline API | Konfiguration von Rendering-Pipelines |
| Graphics Synchronization API | Synchronisation von Grafikoperationen |
| Graphics Presentation API | Übergabe fertiger Bildinhalte an das Darstellungssystem |

Die API ist unabhängig von konkreten Grafiktreibern und Rendering-Backends.

Die Implementierung erfolgt über den Nova Graphics Core.

## 3. API-Prinzipien

- **Hardwareunabhängigkeit:** Einheitliche Schnittstellen für unterschiedliche Grafikgeräte.
- **Explizite Ressourcenverwaltung:** Kontrollierte Erstellung und Freigabe grafischer Ressourcen.
- **Asynchrone Ausführung:** Grafikbefehle können unabhängig von der CPU verarbeitet werden.
- **Zero-Copy:** Gemeinsame Nutzung geeigneter Speicherressourcen ohne unnötige Kopien.
- **Capability-Based Access:** Zugriff ausschließlich auf autorisierte Grafikressourcen.
- **Determinismus:** Eindeutig definierte Befehlsreihenfolge und Synchronisation.
- **Portabilität:** Implementierbarkeit auf NovaOS und anderen unterstützten Plattformen.
- **Erweiterbarkeit:** Neue Grafikfunktionen können über versionierte Erweiterungen bereitgestellt werden.

Die API MUSS ohne KI und ohne Netzwerkverbindung funktionieren.

## 4. API-Ebenen

NovaOS unterscheidet drei Zugriffsebenen.

### High-Level Graphics API

Stellt komfortable Grafikfunktionen bereit:

- Formen und Linien.
- Farben und Farbverläufe.
- Bilder und Texturen.
- Transformationen.
- Clipping.
- Textdarstellung über das Text-Subsystem.
- Einfache 2D- und 3D-Operationen.

Diese Ebene wird insbesondere durch Nova UI und Anwendungen verwendet.

### Low-Level Graphics API

Ermöglicht direkten Zugriff auf abstrakte Grafikressourcen:

- Buffers.
- Textures.
- Command Buffers.
- Render Passes.
- Pipelines.
- Shaders.
- Synchronization Objects.

Sie richtet sich an Anwendungen mit speziellen Rendering-Anforderungen.

### Internal Graphics API

Stellt privilegierte Funktionen für Systemkomponenten bereit:

- Grafikgeräteverwaltung.
- Treiberinitialisierung.
- Speicherverwaltung.
- GPU-Reset.
- Gerätespezifische Funktionen.

Der Zugriff MUSS durch Nova.Trust autorisiert werden.

## 5. Graphics Device API

Die Device API ermöglicht:

- Auflisten verfügbarer Grafikgeräte.
- Abfragen unterstützter Funktionen.
- Auswahl geeigneter Grafikgeräte.
- Erstellen logischer Graphics Devices.
- Abfragen von Ressourcenlimits.
- Verwaltung von Graphics Queues.

Jedes Gerät MUSS seine unterstützten Funktionen und Einschränkungen bereitstellen.

Anwendungen DÜRFEN keine nicht unterstützten Hardwarefunktionen voraussetzen.

## 6. Graphics Resource API

Die API unterstützt folgende Ressourcen:

| Ressource | Beschreibung |
|---|---|
| GraphicsBuffer | Allgemeiner Grafikspeicher |
| Texture | Bild- und Texturdaten |
| RenderTarget | Rendering-Ziel |
| DepthStencilBuffer | Tiefen- und Maskierungsdaten |
| ShaderModule | Programmierbare Grafikoperationen |
| GraphicsPipeline | Konfigurierte Rendering-Pipeline |
| Sampler | Texturabtastung |
| CommandBuffer | Aufgezeichnete Grafikbefehle |
| Fence | Synchronisationsobjekt |

Ressourcen werden über gültige Handles referenziert.

Die API MUSS Eigentümerschaft, Zugriffsrechte und Lebenszyklen berücksichtigen.

Ressourcenfreigaben DÜRFEN keine noch laufenden Grafikoperationen ungültig machen.

## 7. Graphics 2D API

Die 2D-API unterstützt:

- Pixeloperationen.
- Linien und geometrische Formen.
- Pfade und Kurven.
- Flächenfüllungen.
- Farbverläufe.
- Bildskalierung und Rotation.
- Alpha-Blending.
- Clipping und Maskierung.
- Transformationen.
- Offscreen-Rendering.

Die 2D-API MUSS mit einem Software-Renderer implementierbar sein.

Hardwarebeschleunigung SOLL automatisch genutzt werden können, sofern sie verfügbar und geeignet ist.

## 8. Graphics 3D API

Die 3D-API unterstützt:

- Vertex- und Index-Buffer.
- Dreidimensionale Transformationen.
- Rasterisierung.
- Texturierung.
- Depth- und Stencil-Tests.
- Shader.
- Rendering-Pipelines.
- Render Passes.
- Mehrstufige Rendering-Verfahren.

Zusätzliche Funktionen wie Compute Shader, Raytracing oder Mesh Shader DÜRFEN über optionale Erweiterungen bereitgestellt werden.

Nicht unterstützte Funktionen MÜSSEN eindeutig erkennbar sein.

## 9. Command API

Grafikoperationen werden über Command Buffers aufgezeichnet.

Ablauf:

1. Command Buffer erstellen.
2. Grafikbefehle aufzeichnen.
3. Ressourcenabhängigkeiten definieren.
4. Command Buffer abschließen.
5. Befehle an eine Graphics Queue übergeben.
6. Ausführung synchronisieren.
7. Ressourcen wiederverwenden oder freigeben.

Die API MUSS sowohl synchrone als auch asynchrone Nutzung ermöglichen.

Die tatsächliche Ausführung erfolgt durch das gewählte Rendering-Backend.

## 10. Synchronisation

Die Graphics API stellt folgende Mechanismen bereit:

| Mechanismus | Funktion |
|---|---|
| Fence | Abschluss einer Grafikoperation signalisieren |
| Semaphore | Abhängigkeiten zwischen Grafikoperationen koordinieren |
| Barrier | Speicher- und Ausführungsabhängigkeiten definieren |
| Timeline | Fortschritt mehrerer Operationen verfolgen |

Synchronisationsoperationen MÜSSEN eindeutig definierte Zustände besitzen.

CPU und GPU DÜRFEN nicht unnötig gegenseitig blockiert werden.

## 11. Integration in Nova Framework

Die Graphics API ist Bestandteil des portablen Nova Framework.

Dabei gilt:

- Öffentliche Grafiktypen und grundlegende API-Semantik bleiben plattformunabhängig.
- NovaOS verwendet den nativen Graphics Core.
- Fremdsysteme verwenden geeignete plattformspezifische Backends.
- NovaLang erhält direkten Zugriff auf die öffentlichen Grafikfunktionen.
- Nova UI verwendet die High-Level Graphics API.
- Anwendungen können bei Bedarf die Low-Level Graphics API nutzen.

NovaOS-spezifische privilegierte Funktionen bleiben außerhalb der portablen öffentlichen API.

Die Graphics API DARF keine zwingende Abhängigkeit von Nova UI oder dem Window Manager besitzen.

## 12. Normative Anforderungen

- GRAPHICS-001: NovaOS MUSS eine einheitliche öffentliche Graphics API bereitstellen.
- GRAPHICS-002: Die API MUSS unabhängig von konkreten Grafiktreibern sein.
- GRAPHICS-003: Die API MUSS 2D- und 3D-Grafikoperationen unterstützen.
- GRAPHICS-004: Grundlegende 2D-Funktionen MÜSSEN ohne GPU-Beschleunigung verfügbar sein.
- GRAPHICS-005: Grafikressourcen MÜSSEN über kontrollierte Handles verwaltet werden.
- GRAPHICS-006: Ressourcenbesitz und Zugriffsrechte MÜSSEN geprüft werden.
- GRAPHICS-007: Grafikbefehle MÜSSEN vor ihrer Ausführung validiert werden.
- GRAPHICS-008: Asynchrone Grafikoperationen MÜSSEN unterstützt werden.
- GRAPHICS-009: Synchronisationsmechanismen MÜSSEN explizit verfügbar sein.
- GRAPHICS-010: Hardwarefunktionen MÜSSEN vor ihrer Verwendung abgefragt werden können.
- GRAPHICS-011: Optionale Funktionen DÜRFEN nicht als verpflichtende Grundfunktionen vorausgesetzt werden.
- GRAPHICS-012: Die API MUSS versionierbar und erweiterbar sein.
- GRAPHICS-013: Grafikressourcen MÜSSEN kontrolliert freigegeben werden können.
- GRAPHICS-014: Die API MUSS unterschiedliche Pixelformate und Farbräume berücksichtigen.
- GRAPHICS-015: Rendering-Backends MÜSSEN austauschbar bleiben.
- GRAPHICS-016: Die öffentliche API MUSS für eine plattformübergreifende Implementierung geeignet sein.
- GRAPHICS-017: Privilegierte Grafikoperationen MÜSSEN durch Nova.Trust geschützt werden.
- GRAPHICS-018: Die Graphics API MUSS ohne KI-Abhängigkeit funktionieren.

## 13. Fehlerbehandlung

Die API verwendet definierte Fehlerzustände für:

- Ungültige Handles.
- Nicht unterstützte Funktionen.
- Fehlende Berechtigungen.
- Ungültige Ressourcenparameter.
- Speichermangel.
- Fehlgeschlagene Command-Übermittlung.
- Verlorene Grafikgeräte.
- Synchronisationsfehler.

Fehler MÜSSEN eindeutig an den Aufrufer zurückgemeldet werden.

Ein Geräteverlust MUSS erkennbar sein und eine kontrollierte Neuerstellung betroffener Ressourcen ermöglichen.

Fehler einzelner Anwendungen DÜRFEN andere Grafikclients nicht unkontrolliert beeinträchtigen.

## 14. Ergebnis

NovaOS erhält eine einheitliche, hardwareunabhängige Graphics API für 2D- und 3D-Grafik.

Die API unterstützt sowohl komfortable High-Level-Funktionen als auch leistungsfähige Low-Level-Operationen.

Durch die Integration in das portable Nova Framework können Anwendungen dieselben grundlegenden Grafikfunktionen auf unterschiedlichen Betriebssystemen verwenden.

**Grundsatz:** Eine einheitliche Grafik-API stellt leistungsfähige Grafikfunktionen bereit, ohne Anwendungen an bestimmte Hardware, Treiber oder Rendering-Technologien zu binden.