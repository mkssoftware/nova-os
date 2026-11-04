# NPSPEC-LIQUID-DEPENDENCY-0001 – Nova Liquid Glass Dependency Tracking

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Material Engine / Nova Liquid Glass

## 1. Zweck

Nova Liquid Glass Dependency Tracking definiert die Erfassung und Auswertung sämtlicher Abhängigkeiten, die das Erscheinungsbild einer Liquid-Glass-Oberfläche beeinflussen.

Ziel ist die präzise Ermittlung betroffener Effektstufen und Pixelregionen, damit NovaOS ausschließlich notwendige Liquid-Glass-Berechnungen erneut ausführt.

## 2. Architekturprinzipien

- **Dependency-Driven:** Aktualisierungen basieren auf tatsächlichen Abhängigkeiten.
- **Backdrop-Aware:** Veränderungen hinter Glasoberflächen werden berücksichtigt.
- **Spatial:** Abhängigkeiten werden räumlich ausgewertet.
- **Fine-Grained:** Einzelne Effekte können unabhängig invalidiert werden.
- **Incremental:** Unveränderte Ergebnisse bleiben gültig.
- **Versioned:** Abhängigkeiten und Eingaben besitzen nachvollziehbare Versionen.
- **Cache-Aware:** Abhängigkeiten bestimmen die Gültigkeit zwischengespeicherter Effekte.
- **Deterministic:** Identische Eingaben erzeugen reproduzierbare Invalidierungen.

## 3. Architektur

Nova Liquid Glass Dependency Tracking verwendet die bestehende Infrastruktur von Nova Material Dependency Tracking.

Liquid-Glass-spezifische Komponenten sind:

| Komponente | Verantwortung |
|---|---|
| Liquid Dependency Resolver | Auswertung der Glasabhängigkeiten |
| Backdrop Dependency Tracker | Erfassung relevanter Hintergrundänderungen |
| Effect Dependency Mapper | Zuordnung von Eingaben zu Effektstufen |
| Spatial Dependency Mapper | Berechnung räumlicher Auswirkungen |
| Liquid Invalidation Controller | Weitergabe betroffener Ergebnisse |

Eine unabhängige zweite Dependency-Graph-Infrastruktur DARF nicht entstehen.

## 4. Dependency Model

Jede Liquid-Glass-Instanz MUSS folgende Abhängigkeiten beschreiben können:

| Abhängigkeit | Beschreibung |
|---|---|
| Geometry | Position, Größe und Transformation |
| Backdrop | Sichtbare und für Effekte benötigte Hintergrundinhalte |
| Blur | Filterparameter und Eingaberegionen |
| Refraction | Brechungsparameter und abgetastete Hintergrundbereiche |
| Reflection | Reflexionsquellen und Reflexionsparameter |
| Lighting | Lichtquellen, Highlights und Beleuchtungsparameter |
| Tint | Materialfarbe und Farbüberlagerung |
| Transparency | Transparenz- und Alpha-Eigenschaften |
| Animation | Zeitabhängige Materialzustände |
| Pipeline | Shader-, Qualitäts- und Pipeline-Konfiguration |

Abhängigkeiten MÜSSEN eindeutig identifizierbar und versionierbar sein.

## 5. Dependency Graph

NovaOS MUSS Liquid-Glass-Abhängigkeiten in den Material Dependency Graph integrieren.

Dabei gilt:

- Materialeingaben und Effektergebnisse bilden logische Knoten.
- Abhängigkeiten werden als gerichtete Verbindungen beschrieben.
- Änderungen werden transitiv an betroffene Ergebnisse weitergegeben.
- Unabhängige Effekte bleiben gültig.
- Gemeinsame Eingaben DÜRFEN von mehreren Glasinstanzen verwendet werden.

Zyklische Abhängigkeiten MÜSSEN erkannt und sicher behandelt werden.

## 6. Backdrop Dependencies

Liquid Glass MUSS Veränderungen hinter seiner Oberfläche erkennen können.

Dabei gilt:

- Änderungen des Hintergrundinhalts können Blur, Refraction und Composition beeinflussen.
- Verschiebungen von Fenstern hinter Glasflächen MÜSSEN berücksichtigt werden.
- Änderungen der Ebenenreihenfolge können Backdrop-Abhängigkeiten verändern.
- Verdeckte Inhalte bleiben relevant, sofern sie tatsächlich in eine Effektberechnung eingehen.
- Unveränderte Hintergrundbereiche SOLLEN ihre gültigen Effektergebnisse behalten.

Backdrop-Abhängigkeiten MÜSSEN anhand des tatsächlich verwendeten Kompositionszustands ausgewertet werden.

## 7. Spatial Dependency Mapping

NovaOS MUSS räumliche Eingabe- und Ausgabeabhängigkeiten unterscheiden.

Dabei gilt:

- **Blur:** Eingaberegionen werden entsprechend dem Filterbereich erweitert.
- **Refraction:** Eingaberegionen berücksichtigen die möglichen Abtastverschiebungen.
- **Reflection:** Relevante Reflexionsquellen und deren Abbildungsbereiche werden berücksichtigt.
- **Lighting:** Änderungen werden auf betroffene Beleuchtungsbereiche abgebildet.
- **Geometry:** Alte und neue Materialgrenzen werden berücksichtigt.
- **Composition:** Änderungen werden auf betroffene Ausgabepixel übertragen.

Räumliche Abbildungen MÜSSEN konservativ korrekt sein.

Ist eine genaue Begrenzung nicht möglich, MUSS der betroffene Bereich erweitert oder vollständig invalidiert werden.

## 8. Change Propagation

Der logische Ablauf lautet:

`Source Change → Dependency Resolution → Spatial Mapping → Effect Invalidation → Damage Tracking`

Dabei gilt:

1. Eine Änderung wird mit ihrer Zustandsversion erfasst.
2. Betroffene Liquid-Glass-Instanzen werden ermittelt.
3. Abhängige Effektstufen werden bestimmt.
4. Räumliche Auswirkungen werden berechnet.
5. Betroffene Cache-Ergebnisse werden invalidiert.
6. Nova Damage Tracking erhält die erforderlichen Ausgaberegionen.

Mehrere Änderungen SOLLEN zusammengeführt werden, ohne notwendige Invalidierungen zu verlieren.

## 9. Effect Dependency Tracking

NovaOS MUSS die Abhängigkeiten einzelner Liquid-Glass-Effekte getrennt verfolgen.

Beispiele:

- Eine Tint-Änderung erfordert nicht grundsätzlich eine neue Blur-Berechnung.
- Eine Backdrop-Änderung kann Blur und Refraction invalidieren.
- Eine Lighting-Änderung muss nicht zwangsläufig Backdrop-Ergebnisse invalidieren.
- Eine Änderung der Glasgeometrie kann mehrere Effektstufen beeinflussen.

Die tatsächliche Invalidierung MUSS aus der jeweiligen Pipeline- und Effektdefinition abgeleitet werden.

## 10. Cache Integration

Nova Liquid Glass Dependency Tracking MUSS mit folgenden Komponenten zusammenarbeiten:

- Nova Material Effect Caching.
- Nova Liquid Glass Blur Caching.
- Nova Liquid Glass Refraction Caching.
- Nova Liquid Glass Reflection Caching.

Dabei gilt:

- Cache-Einträge MÜSSEN ihre relevanten Eingabeabhängigkeiten besitzen.
- Änderungen invalidieren ausschließlich betroffene Ergebnisse, soweit sicher bestimmbar.
- Teilweise Cache-Invalidierung SOLL unterstützt werden.
- Ungültige Ergebnisse DÜRFEN nicht wiederverwendet werden.

## 11. Render Graph und Damage Integration

Nova Liquid Glass Dependency Tracking MUSS mit Nova Render Graph und Nova Damage Tracking zusammenarbeiten.

Dabei gilt:

- Der Dependency Graph bestimmt betroffene Effekte.
- Damage Tracking bestimmt betroffene Pixelregionen.
- Der Render Graph organisiert notwendige Verarbeitungsschritte.
- Die Render Engine führt diese Verarbeitungsschritte aus.
- Unveränderte Effekte SOLLEN nicht erneut berechnet werden.

GPU Resource Hazard Tracking bleibt für GPU-Speicherzugriffe und Synchronisation zuständig.

## 12. Versionierung und Konsistenz

NovaOS MUSS konsistente Zustandsversionen verwenden.

Dabei gilt:

- Abhängigkeitsanalysen beziehen sich auf definierte Material- und Kompositionszustände.
- Änderungen während laufender Berechnungen erzeugen neue Zustandsversionen.
- Veraltete Ergebnisse DÜRFEN aktuelle Cache-Einträge nicht überschreiben.
- Ausstehende Änderungen DÜRFEN bei Frame-Zusammenfassung nicht verloren gehen.
- Die Veröffentlichung neuer Ergebnisse MUSS konsistent erfolgen.

## 13. Sicherheit und Fehlerbehandlung

NovaOS MUSS folgende Fehler behandeln:

- Ungültige Abhängigkeitsreferenzen.
- Fehlende Backdrop-Ressourcen.
- Nicht auflösbare Abhängigkeitszyklen.
- Ungültige räumliche Abbildungen.
- Veraltete Eingabeversionen.
- Unvollständige Effektinformationen.

Bei unklaren Abhängigkeiten MUSS konservativ invalidiert werden.

Geschützte Oberflächeninhalte DÜRFEN durch Backdrop Sampling oder gemeinsam genutzte Cache-Ergebnisse nicht unautorisiert offengelegt werden.

## 14. Verbindliche Anforderungen

1. Liquid-Glass-Abhängigkeiten MÜSSEN über Nova Material Dependency Tracking verwaltet werden.
2. Eine redundante Dependency-Graph-Infrastruktur DARF nicht entstehen.
3. Backdrop-Abhängigkeiten MÜSSEN unterstützt werden.
4. Blur-, Refraction-, Reflection- und Lighting-Abhängigkeiten MÜSSEN getrennt auswertbar sein.
5. Räumliche Eingabe- und Ausgabeabhängigkeiten MÜSSEN berücksichtigt werden.
6. Alte und neue Materialgeometrien MÜSSEN bei Änderungen berücksichtigt werden.
7. Änderungen MÜSSEN transitiv weitergegeben werden.
8. Unveränderte Effektergebnisse SOLLEN gültig bleiben.
9. Cache-Invalidierungen MÜSSEN aus den Abhängigkeiten ableitbar sein.
10. Nova Damage Tracking MUSS integriert werden.
11. Nova Render Graph MUSS betroffene Effektstufen ermitteln können.
12. Abhängigkeitszustände MÜSSEN versioniert sein.
13. Ungültige oder veraltete Ergebnisse DÜRFEN nicht verwendet werden.
14. Geschützte Oberflächeninhalte MÜSSEN isoliert bleiben.
15. Bei unvollständigen Abhängigkeiten MUSS konservativ invalidiert werden.
16. Im deterministischen Modus MÜSSEN Abhängigkeitsanalyse und Invalidierung bei identischen Eingaben reproduzierbar sein.

## 15. Ergebnis

Nova Liquid Glass Dependency Tracking ermöglicht die präzise Erkennung und Weitergabe von Änderungen, die Liquid-Glass-Effekte beeinflussen.

Durch die Integration von Material Dependency Tracking, räumlicher Abhängigkeitsanalyse und gezielter Cache-Invalidierung bleiben unveränderte Effektergebnisse erhalten.

Damit schafft NovaOS die Grundlage für eine effiziente, inkrementelle Liquid-Glass-Darstellung, bei der ausschließlich tatsächlich betroffene Effekte und Pixelregionen neu berechnet werden.