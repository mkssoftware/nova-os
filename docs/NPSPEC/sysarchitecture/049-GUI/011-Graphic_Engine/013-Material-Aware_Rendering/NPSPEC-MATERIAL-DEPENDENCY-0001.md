# NPSPEC-MATERIAL-DEPENDENCY-0001 – Nova Material Dependency Tracking

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Material Engine / Nova Render Engine

## 1. Zweck

Nova Material Dependency Tracking definiert die Erfassung, Verwaltung und Auswertung sämtlicher Abhängigkeiten zwischen Materialien, ihren Eigenschaften, Eingaberessourcen und grafischen Effekten.

Ziel ist die präzise Ermittlung, welche Materialergebnisse durch eine Änderung tatsächlich betroffen sind.

Unveränderte Materialergebnisse SOLLEN erhalten bleiben, um unnötige GPU-Berechnungen und vollständige Neuberechnungen zu vermeiden.

## 2. Architekturprinzipien

- **Dependency-Driven:** Aktualisierungen erfolgen anhand tatsächlicher Abhängigkeiten.
- **Declarative:** Materialabhängigkeiten werden unabhängig von ihrer Ausführung beschrieben.
- **Fine-Grained:** Abhängigkeiten werden möglichst auf Eigenschaften, Ressourcen und Regionen begrenzt.
- **Incremental:** Nur betroffene Ergebnisse werden invalidiert.
- **Spatial-Aware:** Räumliche Auswirkungen von Effekten werden berücksichtigt.
- **Cache-Aware:** Abhängigkeiten bestimmen die Gültigkeit zwischengespeicherter Ergebnisse.
- **Versioned:** Änderungen und Abhängigkeitszustände werden versioniert.
- **Deterministic:** Identische Änderungen erzeugen identische logische Invalidierungen.

## 3. Dependency Architecture

Nova Material Dependency Tracking besteht aus folgenden Komponenten:

| Komponente | Verantwortung |
|---|---|
| Dependency Manager | Zentrale Verwaltung der Materialabhängigkeiten |
| Dependency Registry | Registrierung von Eingaben und Ausgaben |
| Dependency Graph | Darstellung der Abhängigkeitsbeziehungen |
| Change Analyzer | Ermittlung geänderter Eingaben |
| Dependency Resolver | Bestimmung betroffener Materialergebnisse |
| Spatial Dependency Mapper | Berechnung räumlicher Auswirkungen |
| Invalidation Controller | Weitergabe notwendiger Invalidierungen |

Die Material Engine beschreibt die Abhängigkeiten.

Nova Damage Tracking verarbeitet die daraus resultierenden räumlichen Änderungen.

## 4. Dependency Model

Jede Materialabhängigkeit besitzt mindestens:

| Eigenschaft | Beschreibung |
|---|---|
| Dependency ID | Eindeutige Identifikation |
| Source ID | Abhängige Eingabe oder Ressource |
| Target ID | Betroffenes Materialergebnis |
| Dependency Type | Art der Abhängigkeit |
| Source Version | Version der Eingabe |
| Spatial Mapping | Räumliche Zuordnung |
| Dependency State | Aktueller Gültigkeitszustand |

Abhängigkeiten MÜSSEN eindeutig identifizierbar und versionierbar sein.

## 5. Dependency Types

NovaOS MUSS folgende Abhängigkeitstypen unterstützen:

| Typ | Beschreibung |
|---|---|
| Property | Materialparameter und Eigenschaften |
| Geometry | Position, Größe und Transformation |
| Texture | Texturen und Texture Views |
| Buffer | GPU-Buffer und Parameterdaten |
| Backdrop | Hintergrundinhalte |
| Blur | Räumliche Filterabhängigkeiten |
| Refraction | Abhängigkeiten durch Lichtbrechung |
| Reflection | Reflexionsquellen |
| Lighting | Licht- und Highlight-Daten |
| Animation | Zeitabhängige Materialzustände |
| Pipeline | Shader- und Pipeline-Konfigurationen |

Weitere Abhängigkeitstypen DÜRFEN ergänzt werden.

## 6. Material Dependency Graph

NovaOS MUSS Materialabhängigkeiten in einem gerichteten Graphen verwalten können.

Dabei gilt:

- Eingaben und abgeleitete Ergebnisse bilden Graphknoten.
- Abhängigkeiten werden durch gerichtete Verbindungen beschrieben.
- Änderungen werden entlang betroffener Verbindungen ausgewertet.
- Unabhängige Materialergebnisse bleiben gültig.
- Mehrfachabhängigkeiten MÜSSEN korrekt zusammengeführt werden.

Zyklische Abhängigkeiten MÜSSEN erkannt werden.

Nicht auflösbare Zyklen MÜSSEN zurückgewiesen oder konservativ behandelt werden.

## 7. Change Detection

NovaOS MUSS Änderungen relevanter Materialeingaben erkennen.

Dazu gehören:

- Änderungen einzelner Materialparameter.
- Änderungen von Textur- oder Buffer-Inhalten.
- Änderungen der Materialgeometrie.
- Änderungen des Hintergrunds.
- Änderungen von Lichtquellen.
- Änderungen von Shadern oder Pipelines.
- Animationsbedingte Zustandsänderungen.

Änderungen MÜSSEN anhand gültiger Zustandsversionen nachvollziehbar sein.

Ein unveränderter Eingang DARF keine unnötige Invalidierung verursachen.

## 8. Dependency Propagation

Die Auswertung erfolgt nach folgendem Prinzip:

`Source Change → Dependency Analysis → Affected Results → Spatial Mapping → Invalidation`

Dabei gilt:

1. Geänderte Eingaben werden identifiziert.
2. Der Dependency Graph wird ausgewertet.
3. Betroffene Materialergebnisse werden ermittelt.
4. Räumliche Auswirkungen werden berechnet.
5. Notwendige Invalidierungen werden weitergegeben.
6. Unveränderte Ergebnisse bleiben erhalten.

Die Weitergabe MUSS sämtliche transitiv betroffenen Ergebnisse berücksichtigen.

## 9. Spatial Dependency Tracking

NovaOS MUSS räumliche Materialabhängigkeiten unterstützen.

Dabei gilt:

- Änderungen werden möglichst auf betroffene Pixelregionen begrenzt.
- Blur-Abhängigkeiten berücksichtigen den Filterbereich.
- Refraction-Abhängigkeiten berücksichtigen die tatsächlich abgetasteten Hintergrundregionen.
- Reflection-Abhängigkeiten berücksichtigen relevante Reflexionsquellen.
- Transformationen und Clipping MÜSSEN berücksichtigt werden.
- Nicht sicher begrenzbare Abhängigkeiten führen zu konservativer Invalidierung.

Die räumliche Auswertung MUSS mit Nova Damage Tracking zusammenarbeiten.

## 10. Backdrop Dependency Tracking

Backdrop-abhängige Materialien benötigen eine besondere Behandlung.

NovaOS MUSS erkennen, wenn sich Inhalte hinter einer Materialoberfläche verändern.

Dabei gilt:

- Änderungen hinter transparenten Materialien können deren Darstellung beeinflussen.
- Vollständig unveränderte Hintergrundbereiche SOLLEN wiederverwendet werden.
- Blur- und Refraction-Effekte können über die sichtbaren Materialgrenzen hinausgehende Eingaben benötigen.
- Verdeckte Hintergrundinhalte DÜRFEN nicht grundsätzlich als irrelevant betrachtet werden.
- Geschützte Oberflächeninhalte MÜSSEN von unautorisierten Materialzugriffen ausgeschlossen bleiben.

## 11. Incremental Dependency Updates

NovaOS SOLL den Dependency Graph inkrementell aktualisieren.

Dabei gilt:

- Unveränderte Abhängigkeiten bleiben erhalten.
- Neue Materialeigenschaften erzeugen nur notwendige Graphänderungen.
- Entfernte Materialeffekte entfernen ihre nicht mehr benötigten Abhängigkeiten.
- Änderungen einzelner Parameter SOLLEN keine vollständige Neuanalyse erzwingen.
- Abhängigkeitsversionen MÜSSEN konsistent veröffentlicht werden.

## 12. Cache Invalidation

Nova Material Dependency Tracking MUSS die Gültigkeit materialabhängiger Cache-Ergebnisse bestimmen können.

Dabei gilt:

- Cache-Einträge besitzen nachvollziehbare Eingabeabhängigkeiten.
- Änderungen relevanter Eingaben invalidieren betroffene Ergebnisse.
- Unveränderte Zwischenergebnisse SOLLEN wiederverwendet werden.
- Teilweise Invalidierung SOLL unterstützt werden.
- Ungültige Cache-Ergebnisse DÜRFEN nicht erneut verwendet werden.

Die eigentliche Speicherung erfolgt durch Nova Material Effect Caching.

## 13. Render Graph Integration

Nova Material Dependency Tracking MUSS mit Nova Render Graph zusammenarbeiten.

Dabei gilt:

- Betroffene Materialoperationen werden dem Render Graph gemeldet.
- Unveränderte Verarbeitungsschritte SOLLEN übersprungen werden.
- Ressourcenabhängigkeiten werden an die zuständigen Subsysteme übergeben.
- Die Reihenfolge abhängiger Render-Passes MUSS erhalten bleiben.
- GPU-Synchronisation und Resource Barriers bleiben verbindlich.

Material Dependency Tracking ersetzt nicht die GPU Resource Hazard Analysis.

## 14. Liquid Glass Integration

NovaOS MUSS komplexe Liquid-Glass-Abhängigkeiten unterstützen.

Dazu gehören:

- Backdrop Sampling.
- Blur.
- Refraction.
- Reflection.
- Lighting.
- Tint.
- Transparency.
- Animation.

Dabei gilt:

- Änderungen einzelner Effekte SOLLEN nur abhängige Verarbeitungsschritte invalidieren.
- Gemeinsame Hintergrundberechnungen SOLLEN wiederverwendet werden.
- Änderungen hinter Glasflächen MÜSSEN korrekt weitergegeben werden.
- Unveränderte Liquid-Glass-Ergebnisse SOLLEN erhalten bleiben.

## 15. Sicherheit und Fehlerbehandlung

NovaOS MUSS folgende Fehler kontrolliert behandeln:

- Ungültige Abhängigkeitsreferenzen.
- Veraltete Ressourcenversionen.
- Nicht auflösbare Abhängigkeitszyklen.
- Fehlende Eingaberessourcen.
- Ungültige räumliche Zuordnungen.
- Inkonsistente Materialzustände.

Bei unvollständigen oder nicht eindeutig bestimmbaren Abhängigkeiten MUSS eine konservative Invalidierung erfolgen.

Optimierungen DÜRFEN niemals zur Verwendung nachweislich ungültiger Materialergebnisse führen.

## 16. Verbindliche Anforderungen

1. NovaOS MUSS Materialabhängigkeiten zentral verwalten.
2. Abhängigkeiten MÜSSEN versioniert werden.
3. Ein gerichteter Dependency Graph MUSS unterstützt werden.
4. Änderungen einzelner Materialeigenschaften MÜSSEN erkennbar sein.
5. Transitive Abhängigkeiten MÜSSEN berücksichtigt werden.
6. Räumliche Materialabhängigkeiten MÜSSEN unterstützt werden.
7. Backdrop-Änderungen MÜSSEN korrekt erfasst werden.
8. Blur-, Refraction- und Reflection-Abhängigkeiten MÜSSEN berücksichtigt werden.
9. Inkrementelle Graphaktualisierungen SOLLEN unterstützt werden.
10. Unveränderte Materialergebnisse SOLLEN gültig bleiben.
11. Nova Damage Tracking MUSS integriert werden.
12. Nova Material Effect Caching MUSS Invalidierungsinformationen erhalten.
13. Nova Render Graph MUSS betroffene Verarbeitungsschritte ermitteln können.
14. Zyklen und ungültige Abhängigkeiten MÜSSEN erkannt werden.
15. Bei Unsicherheit MUSS konservativ invalidiert werden.
16. Geschützte Ressourcen DÜRFEN nicht unautorisiert verarbeitet werden.
17. Im deterministischen Modus MÜSSEN Abhängigkeitsanalyse und Invalidierung bei identischen Eingaben reproduzierbar sein.

## 17. Ergebnis

Nova Material Dependency Tracking ermöglicht die präzise Ermittlung von Änderungen und deren Auswirkungen auf Materialeffekte.

Durch versionierte Abhängigkeitsgraphen, räumliche Änderungsanalyse und inkrementelle Invalidierung können unveränderte Materialergebnisse erhalten bleiben.

Damit bildet NovaOS die Grundlage für effizientes Material-Rendering und die gezielte Aktualisierung komplexer Liquid-Glass-Effekte, ohne bei jeder Änderung sämtliche Materialberechnungen erneut auszuführen.