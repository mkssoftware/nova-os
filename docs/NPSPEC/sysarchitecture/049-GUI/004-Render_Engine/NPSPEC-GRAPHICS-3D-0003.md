# NPSPEC-GRAPHICS-3D-0003 – Nova Shader Management

**Status:** Angenommen  
**Bereich:** Graphics / 3D / Shader Management  
**System:** NovaOS

## 1. Zweck

Nova Shader Management definiert die Verwaltung, Kompilierung und Ausführung programmierbarer Grafikprogramme innerhalb der Nova Graphics Architecture.

Ziel ist eine einheitliche, sichere und backendunabhängige Shader-Verarbeitung für 2D- und 3D-Grafik, Materialien, Beleuchtung und GPU-basierte Berechnungen.

Das System ist Bestandteil des portablen Nova Framework.

## 2. Architektur

Das Shader-System besteht aus folgenden Komponenten:

| Komponente | Aufgabe |
|---|---|
| Shader Manager | Verwaltung von Shader-Ressourcen |
| Shader Compiler | Übersetzung von Shader-Programmen |
| Shader Validator | Prüfung von Shader-Code und Ressourcenanforderungen |
| Shader Reflection | Ermittlung von Eingaben, Ausgaben und Ressourcenbindungen |
| Shader Cache | Speicherung kompilierter Shader |
| Shader Pipeline Manager | Integration in Rendering-Pipelines |
| Backend Adapter | Übersetzung in backendabhängige Shader-Formate |

Die Shader-Verwaltung verwendet die Nova Graphics API und die Graphics Backend Abstraction.

Shader werden unabhängig von Materialien, Meshes und Rendering-Zielen verwaltet.

## 3. Shader-Modell

Ein Shader ist eine programmierbare Verarbeitungseinheit innerhalb einer Grafik- oder Compute-Pipeline.

Unterstützte Shader-Stufen:

| Shader | Aufgabe |
|---|---|
| Vertex Shader | Verarbeitung von Vertex-Daten |
| Fragment Shader | Berechnung von Fragmentfarben und Ausgabewerten |
| Compute Shader | Allgemeine parallele Berechnungen |
| Geometry Shader | Optionale Verarbeitung geometrischer Primitive |
| Tessellation Shader | Optionale Unterteilung geometrischer Oberflächen |

Weitere Shader-Stufen, beispielsweise für Raytracing, können über Backend-Capabilities ergänzt werden.

Nicht unterstützte Shader-Stufen MÜSSEN eindeutig erkennbar sein.

## 4. Shader-Repräsentation

NovaOS unterscheidet drei Repräsentationen:

- **Shader Source:** Lesbarer Shader-Quellcode.
- **Shader IR:** Backendunabhängige Zwischenrepräsentation.
- **Shader Binary:** Für ein bestimmtes Backend erzeugter ausführbarer Shader.

Die Shader IR dient als gemeinsame Grundlage für Validierung, Optimierung und Backendübersetzung.

Die Architektur MUSS unterschiedliche Shader-Quellsprachen ermöglichen.

Eine eigene Nova Shader Language kann später ergänzt werden, ohne die Shader-Management-Schnittstellen zu verändern.

## 5. Kompilierung

Die Shader-Kompilierung erfolgt über eine definierte Verarbeitungskette:

1. Shader-Quelle oder IR laden.
2. Syntax und Struktur prüfen.
3. Ressourcenanforderungen validieren.
4. Backendunabhängige IR erzeugen oder übernehmen.
5. Optimierungen durchführen.
6. Backendabhängigen Shader erzeugen.
7. Shader-Ressource bereitstellen.

Die Kompilierung MUSS unabhängig von der eigentlichen Rendering-Ausführung möglich sein.

Asynchrone Kompilierung SOLL unterstützt werden, um Blockierungen der Benutzeroberfläche zu vermeiden.

## 6. Shader Reflection

Shader Reflection stellt Informationen über die Schnittstellen eines Shaders bereit.

Dazu gehören:

- Eingabe- und Ausgabeattribute.
- Uniform- und Parameterbuffer.
- Texturen und Sampler.
- Ressourcenbindungen.
- Datentypen und Speicherlayouts.
- Verwendete Shader-Stufen.
- Erforderliche Backend-Capabilities.

Diese Informationen dienen der Validierung von Materialien, Ressourcenbindungen und Pipeline-Konfigurationen.

Fehlerhafte Bindungen MÜSSEN vor der Kommandoausführung erkannt werden können.

## 7. Shader-Parameter und Ressourcenbindungen

Shader unterstützen:

- Konstanten und Uniforms.
- Strukturierte Buffer.
- Texturen.
- Sampler.
- Speicherressourcen.
- Push Constants, sofern unterstützt.

Ressourcenbindungen werden über explizite Binding-Layouts beschrieben.

Die API MUSS eine backendunabhängige Zuordnung von Shader-Parametern zu Grafikressourcen ermöglichen.

Zugriffe auf nicht freigegebene Ressourcen DÜRFEN nicht möglich sein.

## 8. Shader-Varianten

Ein Shader kann mehrere Varianten besitzen.

Varianten entstehen beispielsweise durch:

- Unterschiedliche Materialeigenschaften.
- Aktivierte oder deaktivierte Beleuchtungsfunktionen.
- Unterschiedliche Qualitätsstufen.
- Backendabhängige Funktionen.
- Spezialisierungskonstanten.

Shader-Varianten MÜSSEN eindeutig identifizierbar sein.

Die Engine SOLL nur tatsächlich benötigte Varianten kompilieren und vorhalten.

## 9. Shader-Caching

Das Shader-System unterstützt die Wiederverwendung kompilierter Shader.

Ein Cache-Eintrag berücksichtigt mindestens:

- Shader-Quellcode oder IR.
- Compiler-Version.
- Kompilierungsoptionen.
- Shader-Stufe.
- Ziel-Backend.
- Relevante Treiber- und Geräteinformationen.

Ungültige oder inkompatible Cache-Einträge MÜSSEN erkannt und verworfen werden.

Das Shader-Caching DARF keine zwingende Netzwerkverbindung benötigen.

## 10. Pipeline-Integration

Shader werden über die Nova 3D Graphics API mit Rendering-Pipelines verbunden.

Dabei gilt:

- Shader-Stufen müssen zur Pipeline passen.
- Ein- und Ausgabeschnittstellen müssen kompatibel sein.
- Ressourcenbindungen müssen validiert werden.
- Pipeline States können wiederverwendet werden.
- Shader-Ressourcen bleiben unabhängig von einzelnen Pipeline-Instanzen.

Ein Shader DARF nicht verändert werden, während eine laufende GPU-Ausführung auf eine inkonsistente Version zugreifen könnte.

## 11. Hot Reload und Austauschbarkeit

Das Shader-System unterstützt den kontrollierten Austausch von Shader-Versionen.

Dabei gilt:

- Neue Versionen werden vor ihrer Aktivierung kompiliert und validiert.
- Laufende Rendering-Kommandos behalten gültige Ressourcenreferenzen.
- Der Austausch erfolgt an einem definierten Synchronisationspunkt.
- Bei fehlgeschlagener Kompilierung bleibt die bisherige gültige Version erhalten.

Hot Reload ist optional und insbesondere für NovaStudio und Entwicklungswerkzeuge vorgesehen.

## 12. Sicherheit und Ressourcen

Shader können auf der GPU ausgeführt werden und unterliegen deshalb besonderen Sicherheitsanforderungen.

Festlegungen:

- Shader-Code MUSS vor seiner Verwendung validiert werden.
- Ressourcenbindungen MÜSSEN auf gültige Berechtigungen geprüft werden.
- Speicherzugriffe MÜSSEN durch Backend und Treiber isoliert werden.
- Ressourcenlimits MÜSSEN berücksichtigt werden.
- Shader-Ausführung darf keine direkten Kernelzugriffe ermöglichen.
- GPU-Fehler werden über Nova Graphics Driver Recovery behandelt.

Nicht vertrauenswürdige Shader DÜRFEN keine höheren Berechtigungen erhalten als der aufrufende Grafikclient.

## 13. Normative Anforderungen

- **GRAPHICS-001:** NovaOS MUSS eine zentrale Shader-Verwaltung bereitstellen.
- **GRAPHICS-002:** Vertex- und Fragment-Shader MÜSSEN unterstützt werden.
- **GRAPHICS-003:** Compute Shader MÜSSEN bei geeigneten Backend-Capabilities verwendbar sein.
- **GRAPHICS-004:** Shader-Quellcode, Zwischenrepräsentation und Backend-Binärdateien MÜSSEN unterscheidbar sein.
- **GRAPHICS-005:** Die Shader-Architektur MUSS mehrere Quellsprachen ermöglichen.
- **GRAPHICS-006:** Shader MÜSSEN vor ihrer Ausführung validiert werden.
- **GRAPHICS-007:** Shader Reflection MUSS verfügbar sein.
- **GRAPHICS-008:** Ressourcenbindungen MÜSSEN eindeutig definiert und überprüfbar sein.
- **GRAPHICS-009:** Shader MÜSSEN unabhängig von Materialien und Meshes verwaltet werden können.
- **GRAPHICS-010:** Shader-Varianten MÜSSEN unterstützt werden.
- **GRAPHICS-011:** Kompilierte Shader MÜSSEN zwischengespeichert werden können.
- **GRAPHICS-012:** Inkompatible Cache-Einträge MÜSSEN erkannt werden.
- **GRAPHICS-013:** Asynchrone Shader-Kompilierung SOLL unterstützt werden.
- **GRAPHICS-014:** Shader-Austausch MUSS ohne inkonsistente Ressourcenreferenzen erfolgen.
- **GRAPHICS-015:** Shader-Ressourcen MÜSSEN dem Nova Graphics Resource Lifecycle entsprechen.
- **GRAPHICS-016:** Nicht unterstützte Shader-Funktionen MÜSSEN eindeutig gemeldet werden.
- **GRAPHICS-017:** Shader DÜRFEN keine unautorisierten Speicher- oder Kernelzugriffe ermöglichen.
- **GRAPHICS-018:** Das Shader Management MUSS über das portable Nova Framework verfügbar sein.
- **GRAPHICS-019:** Shader-Kompilierung und -Verwaltung MÜSSEN ohne Netzwerk- oder KI-Abhängigkeit funktionieren.

## 14. Fehlerbehandlung

- Syntax- und Kompilierungsfehler werden mit Diagnoseinformationen gemeldet.
- Ungültige Shader-Schnittstellen werden zurückgewiesen.
- Fehlende Backend-Funktionen werden vor der Ausführung erkannt.
- Fehlerhafte Ressourcenbindungen verhindern die Aktivierung betroffener Pipelines.
- Fehlgeschlagene Shader-Updates DÜRFEN bestehende gültige Shader-Versionen nicht beschädigen.
- Bei GPU-Ausfällen erfolgt die Wiederherstellung gemäß Nova Graphics Driver Recovery.

## 15. Ergebnis

NovaOS erhält ein einheitliches, portables und erweiterbares Shader-System.

Shader können unabhängig vom Grafik-Backend beschrieben, kompiliert, validiert, zwischengespeichert und in Rendering-Pipelines eingebunden werden.

Die Architektur unterstützt moderne GPU-Funktionen und ermöglicht die spätere Integration einer eigenen Nova Shader Language.

**Grundsatz:** Shader beschreiben programmierbare Grafikoperationen. NovaOS übernimmt deren sichere Verwaltung, Validierung und Übersetzung für die verfügbare Hardware.