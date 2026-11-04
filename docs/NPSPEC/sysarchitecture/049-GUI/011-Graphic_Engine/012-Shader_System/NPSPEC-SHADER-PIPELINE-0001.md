# NPSPEC-SHADER-PIPELINE-0001 – Nova Shader Pipeline

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Shader System

## 1. Zweck

Nova Shader Pipeline definiert die Erstellung, Konfiguration, Validierung und Verwaltung ausführbarer GPU-Pipelines innerhalb von NovaOS.

Ziel ist die effiziente Kombination von Shadern, Ressourcenbindungen und GPU-Zuständen zu wiederverwendbaren Ausführungseinheiten.

Die Architektur MUSS unabhängig von einer konkreten Grafik-API funktionieren und mit Nova Render Graph sowie Nova GPU Command Architecture zusammenarbeiten.

## 2. Architekturprinzipien

- **Pipeline-Oriented:** GPU-Ausführung basiert auf validierten Pipeline-Konfigurationen.
- **Immutable:** Veröffentlichte Pipeline-Konfigurationen sind unveränderlich.
- **Backend-Independent:** Einheitliches logisches Modell für unterschiedliche GPU-Backends.
- **Cache-First:** Bereits erstellte kompatible Pipelines werden wiederverwendet.
- **Asynchronous:** Pipeline-Erstellung erfolgt möglichst ohne Blockierung.
- **State-Aware:** Shader, Ressourcenlayouts und GPU-Zustände werden gemeinsam validiert.
- **Lifetime-Safe:** Aktive Pipelines bleiben bis zum Abschluss ihrer GPU-Verwendung gültig.
- **Deterministic:** Identische Konfigurationen erzeugen reproduzierbare logische Pipeline-Definitionen.

## 3. Pipeline Architecture

Nova Shader Pipeline besteht aus folgenden Komponenten:

| Komponente | Verantwortung |
|---|---|
| Pipeline Manager | Zentrale Pipeline-Verwaltung |
| Pipeline Descriptor | Deklarative Pipeline-Beschreibung |
| Pipeline Validator | Prüfung der Pipeline-Konfiguration |
| Pipeline Layout Manager | Verwaltung der Ressourcenlayouts |
| Pipeline Compiler | Erstellung ausführbarer GPU-Pipelines |
| Pipeline Cache | Wiederverwendung vorhandener Pipelines |
| Pipeline Backend | Hardwareabhängige Umsetzung |

Pipeline-Beschreibung und hardwareabhängiges Pipeline-Objekt MÜSSEN getrennt verwaltet werden.

## 4. Pipeline Types

NovaOS MUSS folgende grundlegende Pipeline-Typen unterstützen:

| Pipeline | Verwendung |
|---|---|
| Graphics Pipeline | Geometrie- und Fragmentverarbeitung |
| Compute Pipeline | Allgemeine GPU-Berechnungen |

Optionale Hardwarefunktionen wie Mesh Shading DÜRFEN über entsprechende Graphics-Pipeline-Erweiterungen integriert werden.

Nicht unterstützte Pipeline-Funktionen MÜSSEN erkannt werden.

## 5. Pipeline Model

Jede Pipeline besitzt mindestens folgende Eigenschaften:

| Eigenschaft | Beschreibung |
|---|---|
| Pipeline ID | Eindeutige logische Identifikation |
| Pipeline Type | Graphics oder Compute |
| Shader Stages | Zugeordnete Shader |
| Shader Versions | Versionen der verwendeten Shader |
| Pipeline Layout | Ressourcen- und Konstantenlayout |
| Fixed Function State | Relevante GPU-Zustände |
| Render Target Configuration | Ausgabeformate und Sample-Anforderungen |
| Backend Target | Ziel-GPU und Backend |
| Pipeline Version | Version der Pipeline-Konfiguration |
| Compilation State | Erstellungszustand |

Nicht relevante Eigenschaften DÜRFEN abhängig vom Pipeline-Typ entfallen.

## 6. Pipeline Lifecycle

NovaOS definiert folgenden Lebenszyklus:

`Defined → Validating → Compiling → Ready → Retired → Released`

Fehlerhafte Pipelines wechseln in den Zustand `Failed`.

Dabei gilt:

1. Eine Pipeline wird deklarativ beschrieben.
2. Shader und Ressourcenlayouts werden validiert.
3. Die Backend-Kompatibilität wird geprüft.
4. Ein vorhandener Cache-Eintrag wird gesucht.
5. Bei Bedarf wird die Pipeline kompiliert.
6. Die fertige Pipeline wird veröffentlicht.
7. Nicht mehr benötigte Pipelines werden nach Abschluss aktiver GPU-Zugriffe freigegeben.

Nur Pipelines im Zustand `Ready` DÜRFEN zur GPU-Ausführung verwendet werden.

## 7. Pipeline Layout

NovaOS MUSS ein einheitliches Pipeline-Layout-Modell bereitstellen.

Dieses beschreibt:

- Shader-Ressourcenbindungen.
- Descriptor Sets beziehungsweise Binding Groups.
- Uniform- und Storage-Ressourcen.
- Texture- und Sampler-Bindungen.
- Push Constants oder vergleichbare Parameterbereiche.
- Zugriffsarten und Shader-Sichtbarkeit.

Pipeline Layouts MÜSSEN vor ihrer Verwendung validiert werden.

Kompatible Layouts SOLLEN zwischen Pipelines wiederverwendet werden.

## 8. Graphics Pipeline State

Graphics Pipelines MÜSSEN die erforderlichen GPU-Zustände beschreiben können.

Dazu gehören:

- Vertex Input.
- Primitive Topology.
- Rasterization.
- Culling.
- Depth/Stencil.
- Multisampling.
- Color Blending.
- Render Target Formats.

NovaOS MUSS zwischen statischen und dynamischen Pipeline-Zuständen unterscheiden.

Dynamische Zustände SOLLEN verwendet werden, wenn dadurch unnötige Pipeline-Varianten vermieden werden können und das Backend diese unterstützt.

## 9. Pipeline Validation

Vor der Erstellung MUSS NovaOS folgende Eigenschaften prüfen:

- Kompatibilität der Shader-Stufen.
- Gültigkeit der Shader-Versionen.
- Übereinstimmung der Ressourcenlayouts.
- Unterstützte GPU-Funktionen.
- Render-Target-Kompatibilität.
- Zulässige Pipeline-Zustände.
- Hardwareabhängige Ressourcenlimits.

Ungültige Pipeline-Konfigurationen DÜRFEN nicht zur GPU-Ausführung gelangen.

## 10. Pipeline Compilation und Caching

Nova Shader Pipeline MUSS mit Nova Shader Compilation und Nova Shader Caching zusammenarbeiten.

Dabei gilt:

- Pipeline-Erstellung SOLL asynchron erfolgen.
- Kompatible Pipeline-Objekte SOLLEN wiederverwendet werden.
- Cache-Schlüssel MÜSSEN alle relevanten Shader-, Layout-, Zustands- und Backend-Eigenschaften berücksichtigen.
- Änderungen an Shadern oder Pipeline-Zuständen MÜSSEN betroffene Cache-Einträge invalidieren.
- Nicht kompatible Cache-Einträge DÜRFEN nicht verwendet werden.

Häufig benötigte System- und Material-Pipelines SOLLEN vorab erstellt werden können.

## 11. Pipeline Binding

Nova GPU Command Architecture MUSS fertige Pipelines an GPU-Kommandos binden können.

Dabei gilt:

- Pipeline und Command Queue MÜSSEN kompatibel sein.
- Ressourcenbindungen MÜSSEN zum Pipeline Layout passen.
- Erforderliche dynamische Zustände MÜSSEN gesetzt sein.
- GPU Resource Hazard Tracking MUSS relevante Ressourcenzugriffe berücksichtigen.
- Pipeline-Wechsel SOLLEN minimiert werden, sofern die Ausführungsreihenfolge erhalten bleibt.

## 12. Pipeline Versioning

NovaOS MUSS Pipeline-Versionen unterstützen.

Dabei gilt:

- Änderungen erzeugen eine neue logische Pipeline-Version.
- Veröffentlichte Pipeline-Versionen bleiben unveränderlich.
- Bereits aufgezeichnete GPU-Kommandos behalten ihre gültigen Pipeline-Referenzen.
- Alte Pipeline-Versionen werden erst nach Abschluss aller relevanten GPU-Zugriffe freigegeben.
- Ungültige oder fehlgeschlagene neue Versionen DÜRFEN eine noch gültige Version nicht beschädigen.

Damit wird kontrolliertes Pipeline Hot Replacement ermöglicht.

## 13. Render Graph und Material Integration

Nova Shader Pipeline MUSS mit Nova Render Graph und Nova Material Engine zusammenarbeiten.

Dabei gilt:

- Render Passes referenzieren geeignete Pipelines.
- Pipeline-Anforderungen werden bei der Render-Graph-Planung berücksichtigt.
- Materialeffekte können gemeinsame Pipeline-Konfigurationen verwenden.
- Materialparameter SOLLEN ohne Pipeline-Neukompilierung aktualisierbar sein.
- Pipeline-Varianten SOLLEN auf tatsächlich benötigte Konfigurationen begrenzt werden.

Insbesondere Liquid Glass, Acrylic, Blur, Refraction und Reflection SOLLEN gemeinsame Shader- und Pipeline-Bausteine verwenden können.

## 14. Fehlerbehandlung und Fallback

NovaOS MUSS folgende Fehler kontrolliert behandeln:

- Ungültige Pipeline-Konfigurationen.
- Inkompatible Shader-Stufen.
- Nicht unterstützte GPU-Funktionen.
- Fehlgeschlagene Pipeline-Kompilierung.
- Ungültige Ressourcenlayouts.
- Fehlende Pipeline-Cache-Einträge.
- GPU-Geräteverlust.

Bei fehlgeschlagener Pipeline-Erstellung MUSS ein geeigneter Fallback möglich sein.

Dieser kann eine vereinfachte GPU-Pipeline oder eine softwarebasierte Umsetzung verwenden.

## 15. Verbindliche Anforderungen

1. NovaOS MUSS eine zentrale Shader Pipeline Verwaltung bereitstellen.
2. Graphics- und Compute-Pipelines MÜSSEN unterstützt werden.
3. Pipeline-Beschreibung und Backend-Implementierung MÜSSEN getrennt bleiben.
4. Veröffentlichte Pipeline-Konfigurationen MÜSSEN unveränderlich sein.
5. Shader- und Pipeline-Layouts MÜSSEN validiert werden.
6. Pipeline Compilation MUSS unterstützt werden.
7. Asynchrone Pipeline-Erstellung SOLL bevorzugt werden.
8. Pipeline Caching MUSS integriert werden.
9. Pipeline-Versionierung MUSS unterstützt werden.
10. GPU Command Architecture MUSS Pipelines verwenden können.
11. Render Graph MUSS Pipeline-Anforderungen berücksichtigen können.
12. Nova Material Engine MUSS integrierbar sein.
13. Aktive Pipeline-Versionen DÜRFEN nicht vorzeitig freigegeben werden.
14. Hardwarefähigkeiten MÜSSEN vor der Pipeline-Erstellung geprüft werden.
15. Geeignete Fallback-Strategien MÜSSEN verfügbar sein.
16. Im deterministischen Modus MÜSSEN logische Pipeline-Konfigurationen und Auswahlentscheidungen bei identischen Eingaben reproduzierbar sein.

## 16. Ergebnis

Nova Shader Pipeline stellt die zentrale Infrastruktur zur Erstellung und Verwaltung ausführbarer GPU-Pipelines innerhalb von NovaOS bereit.

Durch deklarative Pipeline-Beschreibungen, unveränderliche Konfigurationen, asynchrone Erstellung, Caching und Versionierung werden GPU-Zustandswechsel und wiederholte Kompilierungen reduziert.

Damit entsteht eine effiziente Grundlage für die Nova Render Engine, den Render Graph, GPU Compute und die Nova Material Engine.