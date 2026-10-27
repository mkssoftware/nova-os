# NPSPEC-GRAPHICS-3D-0004 – Nova Texture Management

**Status:** Angenommen  
**Bereich:** Graphics / 3D / Texture Management  
**System:** NovaOS

## 1. Zweck

Nova Texture Management definiert die Verwaltung, Speicherung und Verwendung von Texturen innerhalb der Nova Graphics Architecture.

Ziel ist eine effiziente, sichere und backendunabhängige Texturverarbeitung für 2D- und 3D-Grafik, Materialien, Rendering-Pipelines und GPU-basierte Berechnungen.

Das System ist Bestandteil des portablen Nova Framework und verwendet den Nova Graphics Resource Lifecycle.

## 2. Architektur

Das Textursystem besteht aus folgenden Komponenten:

| Komponente | Aufgabe |
|---|---|
| Texture Manager | Verwaltung von Texturressourcen |
| Texture Resource | Beschreibung und Speicherung von Texturdaten |
| Texture View | Zugriff auf definierte Texturbereiche |
| Texture Sampler | Abtastung und Filterung |
| Texture Upload Manager | Übertragung zwischen CPU- und GPU-Speicher |
| Texture Cache | Wiederverwendung von Texturressourcen |
| Backend Adapter | Hardwareabhängige Umsetzung |

Texturressourcen werden unabhängig von Shadern, Materialien und Szenenobjekten verwaltet.

## 3. Texturmodell

Eine Textur wird durch einen eindeutigen Ressourcenhandle und einen unveränderlichen Descriptor beschrieben.

Der Descriptor enthält mindestens:

- Texturtyp und Dimensionen.
- Pixelformat und Farbraum.
- Anzahl der Mip-Level.
- Anzahl der Array-Layer.
- Sample-Anzahl.
- Zulässige Verwendungsarten.
- Speicher- und Zugriffsanforderungen.

Texturinhalt und Texturbeschreibung werden getrennt verwaltet.

Texturen können von mehreren Materialien und Rendering-Pipelines gemeinsam verwendet werden.

## 4. Unterstützte Texturtypen

| Texturtyp | Beschreibung |
|---|---|
| Texture1D | Eindimensionale Textur |
| Texture2D | Zweidimensionale Textur |
| Texture3D | Dreidimensionale Volumentextur |
| TextureArray | Mehrere gleichartige Texturschichten |
| CubeMap | Sechsseitige Umgebungstextur |
| Multisample Texture | Textur mit mehreren Samples pro Pixel |

Texture2D MUSS durch alle grundlegenden Grafik-Backends unterstützt werden.

Weitere Texturtypen werden entsprechend den Backend-Capabilities bereitgestellt.

## 5. Pixelformate

Das Textursystem unterstützt ein erweiterbares Formatmodell.

Grundlegende Formatklassen:

- Unorm- und Snorm-Formate.
- Ganzzahlformate.
- Fließkommaformate.
- RGB- und RGBA-Formate.
- Tiefen- und Stencil-Formate.
- Komprimierte Texturformate.
- sRGB-Formate.

Die tatsächliche Unterstützung einzelner Formate wird über Backend-Capabilities ermittelt.

Formatkonvertierungen MÜSSEN explizit und kontrolliert erfolgen.

## 6. Texture Sampling

Texture Sampler definieren die Abtastung von Texturdaten.

Unterstützt werden:

- Nearest Filtering.
- Linear Filtering.
- Mipmapping.
- Trilinear Filtering.
- Anisotropic Filtering, sofern verfügbar.

Adressierungsmodi:

- Repeat.
- Mirrored Repeat.
- Clamp to Edge.
- Clamp to Border.

Sampler und Texturressourcen MÜSSEN unabhängig voneinander definiert und wiederverwendet werden können.

## 7. Mipmapping

Mipmaps ermöglichen die effiziente Darstellung von Texturen bei unterschiedlichen Skalierungen.

Das System unterstützt:

- Explizit bereitgestellte Mip-Level.
- Automatische Mipmap-Generierung.
- Auswahl geeigneter Mip-Level.
- Begrenzung des verwendbaren Mip-Bereichs.

Die automatische Generierung MUSS Pixelformat, Farbraum und Alpha-Modus berücksichtigen.

Nicht unterstützte Mipmap-Verfahren werden über die Backend-Capabilities erkannt.

## 8. Texture Views

Texture Views ermöglichen unterschiedliche Zugriffe auf dieselbe Texturressource.

Eine View kann definieren:

- Verwendete Mip-Level.
- Verwendete Array-Layer.
- Zulässige Formatinterpretation.
- Art des Ressourcenzugriffs.

Texture Views DÜRFEN keine Zugriffe außerhalb der zugrunde liegenden Texturressource ermöglichen.

Formatinterpretationen MÜSSEN mit den Regeln des jeweiligen Backends kompatibel sein.

## 9. Speicherverwaltung

Texturen werden gemäß Nova Graphics Resource Lifecycle verwaltet.

Unterstützt werden:

- CPU-lokaler Speicher.
- GPU-lokaler Speicher.
- Gemeinsam nutzbarer Speicher.
- Staging Buffer.
- Asynchrone Datenübertragung.
- Partielle Texturaktualisierung.
- Zero-Copy bei geeigneter Hardware.

Unnötige Speicherduplikate und CPU-GPU-Übertragungen SOLLEN vermieden werden.

Speicherbelegung und Ressourcenlimits unterliegen der zentralen Grafikressourcenverwaltung.

## 10. Texturzustände und Synchronisation

Texturen können für unterschiedliche Operationen verwendet werden:

- Shader Sampling.
- Render Target.
- Depth/Stencil Target.
- Transfer Source.
- Transfer Destination.
- Storage Texture.

Die zulässigen Verwendungsarten werden bei der Ressourcenerstellung festgelegt.

Zustandswechsel und Synchronisation erfolgen über die Nova Graphics API.

Gleichzeitige Lese- und Schreibzugriffe MÜSSEN entsprechend den Ressourcen- und Synchronisationsregeln koordiniert werden.

## 11. Caching und Streaming

Das System unterstützt:

- Wiederverwendung identischer Texturressourcen.
- Caching häufig verwendeter Texturen.
- Bedarfsgerechtes Laden von Mip-Leveln.
- Partielle Texturübertragungen.
- Freigabe nicht benötigter Ressourcen.
- Asynchrone Texturbereitstellung.

Texture Streaming SOLL große Texturen verarbeiten können, ohne deren vollständigen Inhalt gleichzeitig im GPU-Speicher vorzuhalten.

Die Entscheidung über Speicherfreigaben MUSS aktive Ressourcenreferenzen berücksichtigen.

## 12. Integration

- **Nova 3D Graphics API:** Erstellung, Bindung und Verwendung von Texturen.
- **Nova Shader Management:** Bereitstellung von Texturen für Shader.
- **Nova Image Rendering:** Gemeinsame Nutzung geeigneter Bild- und Texturressourcen.
- **Nova Graphics Backend Abstraction:** Hardwareabhängige Texturverarbeitung.
- **Nova Graphics Resource Lifecycle:** Ressourcenverwaltung und Freigabe.
- **Nova Graphics Color System:** Farbraum- und Farbformatverarbeitung.
- **Nova Framework:** Portable Texturschnittstellen.

Das Decodieren von Bilddateien ist nicht Bestandteil des Texture Management.

## 13. Normative Anforderungen

- **GRAPHICS-001:** NovaOS MUSS eine zentrale, backendunabhängige Texturverwaltung bereitstellen.
- **GRAPHICS-002:** Texturen MÜSSEN über eindeutige Ressourcenhandles verwaltet werden.
- **GRAPHICS-003:** Zweidimensionale Texturen MÜSSEN unterstützt werden.
- **GRAPHICS-004:** Weitere Texturtypen MÜSSEN über Backend-Capabilities erkennbar sein.
- **GRAPHICS-005:** Pixelformate, Dimensionen und Verwendungsarten MÜSSEN explizit definiert werden.
- **GRAPHICS-006:** Texturen und Sampler MÜSSEN unabhängig voneinander verwaltet werden können.
- **GRAPHICS-007:** Nearest- und Linear-Filtering MÜSSEN unterstützt werden.
- **GRAPHICS-008:** Mipmapping MUSS unterstützt werden.
- **GRAPHICS-009:** Texture Views MÜSSEN definierte Teilbereiche einer Ressource adressieren können.
- **GRAPHICS-010:** Partielle Texturaktualisierungen MÜSSEN möglich sein.
- **GRAPHICS-011:** Ressourcenübertragungen und Zustandswechsel MÜSSEN synchronisiert werden.
- **GRAPHICS-012:** Texturressourcen MÜSSEN gemeinsam verwendet werden können.
- **GRAPHICS-013:** Texture Streaming SOLL unterstützt werden.
- **GRAPHICS-014:** Ungültige Speicherzugriffe MÜSSEN verhindert werden.
- **GRAPHICS-015:** Texturen MÜSSEN dem Nova Graphics Resource Lifecycle entsprechen.
- **GRAPHICS-016:** Die Texturschnittstellen MÜSSEN Bestandteil des portablen Nova Framework sein.
- **GRAPHICS-017:** Die Verwaltung MUSS ohne Netzwerk- oder KI-Abhängigkeit funktionieren.

## 14. Fehlerbehandlung

- Ungültige Dimensionen und Pixelformate werden zurückgewiesen.
- Nicht unterstützte Texturtypen werden eindeutig gemeldet.
- Speicher- und Größenüberläufe MÜSSEN erkannt werden.
- Ungültige Texture Views und Ressourcenbindungen werden zurückgewiesen.
- Fehlgeschlagene Datenübertragungen DÜRFEN keine inkonsistenten Ressourcen als gültig veröffentlichen.
- Bei GPU-Ausfall erfolgt die Wiederherstellung gemäß Nova Graphics Driver Recovery.
- Wiederherstellbare Texturen SOLLEN aus verfügbaren CPU-Daten oder anderen gültigen Quellen erneut bereitgestellt werden.

## 15. Ergebnis

NovaOS erhält eine einheitliche und ressourcenschonende Texturverwaltung für 2D- und 3D-Grafik.

Texturen können unabhängig vom verwendeten Grafik-Backend erstellt, übertragen, gefiltert, gemeinsam genutzt und wiederverwendet werden.

Die Architektur unterstützt moderne GPU-Funktionen, große Texturen und effiziente Speicherverwaltung.

**Grundsatz:** Texturdaten werden unabhängig von ihrer Verwendung verwaltet und nur in dem Umfang bereitgestellt, den die jeweilige Rendering-Aufgabe benötigt.