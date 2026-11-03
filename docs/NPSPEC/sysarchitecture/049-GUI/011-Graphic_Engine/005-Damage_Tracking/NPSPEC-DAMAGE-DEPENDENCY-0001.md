# NPSPEC-DAMAGE-DEPENDENCY-0001 – Nova Damage Dependency Analysis

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Damage Tracking

## 1. Zweck

Nova Damage Dependency Analysis definiert die Analyse grafischer Abhängigkeiten zur Bestimmung sämtlicher Inhalte, die durch eine Änderung beeinflusst werden können.

Ziel ist die präzise Ermittlung betroffener Scene Nodes, Surfaces, Rendering-Ressourcen und Materialeffekte, ohne unabhängige Inhalte unnötig zu invalidieren.

Die Analyse bestimmt, **welche Abhängigkeiten betroffen sind**. Die tatsächliche Weitergabe erfolgt durch Nova Damage Propagation.

## 2. Architekturprinzipien

- **Dependency-Driven:** Änderungen werden anhand tatsächlicher grafischer Abhängigkeiten analysiert.
- **Region-Aware:** Abhängigkeiten berücksichtigen räumliche Bereiche.
- **Hierarchical:** Scene-Graph-Beziehungen werden einbezogen.
- **Material-Aware:** Effekt- und Backdrop-Abhängigkeiten werden unterstützt.
- **Incremental:** Nur veränderte Abhängigkeiten werden erneut analysiert.
- **Cache-Aware:** Gültige Abhängigkeitsergebnisse können wiederverwendet werden.
- **Conservative:** Unbekannte Auswirkungen führen zu sicherer Invalidierung.
- **Deterministic:** Identische Eingaben erzeugen reproduzierbare Analyseergebnisse.

## 3. Dependency Model

Eine Damage Dependency beschreibt, wie eine grafische Eingabe das Ergebnis einer anderen Komponente beeinflusst.

Sie enthält mindestens:

- **Source ID:** Abhängige Eingabe.
- **Target ID:** Betroffenes Ergebnis.
- **Dependency Type:** Art der Abhängigkeit.
- **Source Region:** Relevanter Eingabebereich.
- **Target Region:** Potenziell beeinflusster Ausgabebereich.
- **Mapping:** Räumliche Abbildung zwischen Eingabe und Ausgabe.
- **State Version:** Zugehörige Zustandsversion.

Eine Abhängigkeit DARF mehrere Eingabe- und Ausgabebereiche umfassen.

## 4. Dependency Types

NovaOS MUSS folgende Abhängigkeiten analysieren können:

| Typ | Beschreibung |
|---|---|
| Scene | Abhängigkeiten zwischen Scene Nodes |
| Hierarchy | Parent-Child- und Transformationsabhängigkeiten |
| Surface | Abhängigkeiten zwischen Oberflächen |
| Resource | Gemeinsam verwendete Rendering-Ressourcen |
| Material | Material- und Effektabhängigkeiten |
| Backdrop | Abhängigkeiten von darunterliegenden Bildinhalten |
| Composition | Abhängigkeiten zwischen Composition Layers |
| Output | Abhängigkeiten von Ausgabeparametern |

Die Analyse MUSS tatsächliche Datenabhängigkeiten von bloßen räumlichen Überschneidungen unterscheiden.

## 5. Dependency Analysis

Die Analyse erfolgt grundsätzlich in folgenden Schritten:

1. Änderungsquelle und Zustandsversion bestimmen.
2. Direkte grafische Abhängigkeiten ermitteln.
3. Betroffene Eingabebereiche prüfen.
4. Auswirkungen auf abhängige Ausgabebereiche berechnen.
5. Indirekte Abhängigkeiten berücksichtigen.
6. Nicht betroffene Abhängigkeiten ausschließen.
7. Analyseergebnisse für Damage Propagation bereitstellen.

Die Analyse MUSS sämtliche relevanten Auswirkungen konservativ erfassen.

## 6. Spatial Dependency Analysis

NovaOS MUSS räumliche Abhängigkeiten berücksichtigen.

Dabei werden insbesondere analysiert:

- Geometrische Überschneidungen
- Transformationen
- Clipping
- Sampling-Bereiche
- Effektbegrenzungen
- Sichtbarkeit und Verdeckung

Eine Änderung MUSS nur diejenigen Ausgabegebiete invalidieren, deren Darstellung tatsächlich beeinflusst werden kann.

Ist eine exakte Bestimmung nicht möglich, MUSS eine konservative Begrenzung verwendet werden.

## 7. Material Dependency Analysis

Materialeffekte können von Bildinformationen außerhalb ihrer eigenen Geometrie abhängen.

NovaOS MUSS insbesondere analysieren:

- Blur-Kernel und Filterradien
- Refraction-Sampling
- Reflection-Abhängigkeiten
- Schattenausdehnung
- Liquid-Glass-Backdrop
- Mehrstufige Effektketten

Für jeden Effekt SOLL die Abbildung zwischen benötigten Eingabebereichen und beeinflussten Ausgabebereichen bestimmbar sein.

Änderungen hinter transparenten Materialien MÜSSEN berücksichtigt werden, sofern sie das Materialergebnis beeinflussen können.

## 8. Dependency Graph Integration

Nova Damage Dependency Analysis verwendet die relevanten Abhängigkeitsinformationen aus:

- Nova Scene Dependency Graph
- Nova Render Graph
- Nova Material Engine
- Nova Compositor
- Nova Graphics Dependency Tracking

Die Analyse SOLL vorhandene Abhängigkeitsinformationen wiederverwenden.

Eine unnötige parallele Verwaltung identischer Abhängigkeitsstrukturen SOLL vermieden werden.

Damage-spezifische räumliche Zuordnungen DÜRFEN ergänzend gespeichert werden.

## 9. Incremental Dependency Analysis

NovaOS SOLL Abhängigkeitsanalysen inkrementell aktualisieren.

Eine erneute Analyse ist insbesondere erforderlich bei:

- Geänderten Scene-Beziehungen
- Veränderten Transformationen
- Neuen oder entfernten Surfaces
- Geänderten Materialparametern
- Veränderten Rendering-Ressourcen
- Änderungen der Ebenenreihenfolge

Unveränderte Abhängigkeiten SOLLEN ihre gültigen Analyseergebnisse behalten.

## 10. Dependency Cycles

Grafische Abhängigkeiten können mehrfach verschachtelt sein oder zyklische Beziehungen enthalten.

NovaOS MUSS:

- Zyklen erkennen.
- Mehrfachanalysen begrenzen.
- Bereits untersuchte Zustände berücksichtigen.
- Eine terminierende Analyse gewährleisten.

Nicht eindeutig auflösbare Abhängigkeitszyklen MÜSSEN konservativ invalidiert oder durch definierte Graphregeln ausgeschlossen werden.

## 11. Cache und Versioning

Analyseergebnisse SOLLEN zwischengespeichert werden können.

Ihre Gültigkeit hängt mindestens ab von:

- Abhängigkeitsstruktur
- Ressourcen- und Scene-Versionen
- Geometrischen Transformationen
- Materialparametern
- Relevanten Ausgabeparametern

Veraltete Analyseergebnisse DÜRFEN nicht zur Unterdrückung erforderlicher Aktualisierungen verwendet werden.

## 12. Occlusion Awareness

NovaOS SOLL Sichtbarkeitsinformationen bei der Abhängigkeitsanalyse berücksichtigen.

Dabei gilt:

- Vollständig verdeckte Inhalte können für die aktuelle Ausgabe irrelevant sein.
- Transparente Materialien können weiterhin verdeckte Eingaben benötigen.
- Backdrop- und Reflection-Abhängigkeiten bleiben maßgeblich.
- Erneute Sichtbarkeit muss erforderliche Aktualisierungen auslösen.

Occlusion DARF tatsächliche grafische Abhängigkeiten nicht ungültig machen.

## 13. Fehlerbehandlung

Bei unvollständigen oder widersprüchlichen Abhängigkeitsinformationen MUSS NovaOS eine konservative Analyse durchführen.

Mögliche Fallback-Stufen sind:

1. Erweiterter betroffener Bereich
2. Vollständiger Scene-Teilbaum
3. Vollständige Surface
4. Vollständige Ausgabe

Die kleinste nachweislich sichere Invalidierungsstufe SOLL bevorzugt werden.

## 14. Verbindliche Anforderungen

1. NovaOS MUSS grafische Abhängigkeiten für Damage Tracking analysieren können.
2. Direkte und indirekte Abhängigkeiten MÜSSEN berücksichtigt werden.
3. Räumliche Eingabe- und Ausgabeabhängigkeiten MÜSSEN unterscheidbar sein.
4. Scene-, Surface-, Resource- und Composition-Abhängigkeiten MÜSSEN unterstützt werden.
5. Material- und Backdrop-Abhängigkeiten MÜSSEN analysiert werden.
6. Unabhängige Inhalte SOLLEN von unnötiger Invalidierung ausgeschlossen bleiben.
7. Vorhandene Dependency-Graph-Informationen SOLLEN wiederverwendet werden.
8. Abhängigkeitsanalysen SOLLEN inkrementell aktualisiert werden.
9. Analyseergebnisse MÜSSEN versionsbezogen gültig bleiben.
10. Zyklische Abhängigkeiten MÜSSEN sicher behandelt werden.
11. Occlusion DARF notwendige Effektabhängigkeiten nicht unterdrücken.
12. Die Analyse MUSS konservativ korrekt sein.
13. Bei unbekannten Auswirkungen MUSS ein sicherer Fallback erfolgen.
14. Die Ergebnisse MÜSSEN Nova Damage Propagation bereitgestellt werden.
15. Identische Eingaben MÜSSEN im deterministischen Modus reproduzierbare Ergebnisse erzeugen.

## 15. Ergebnis

Nova Damage Dependency Analysis ermittelt präzise, welche grafischen Inhalte durch Zustandsänderungen beeinflusst werden können.

Durch räumliche Abhängigkeitsanalyse, Materialbewusstsein und inkrementelle Wiederverwendung werden unnötige Invalidierungen vermieden.

Damit bildet das Subsystem die analytische Grundlage für Nova Damage Propagation und ermöglicht korrektes, effizientes Partial Rendering innerhalb der NovaOS-Grafikarchitektur.