# NPSPEC-LIQUID-REFLECTIONCACHE-0001 – Nova Liquid Glass Reflection Caching

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Material Engine / Nova Liquid Glass

## 1. Zweck

Nova Liquid Glass Reflection Caching definiert die Zwischenspeicherung und inkrementelle Aktualisierung bereits berechneter Reflexionseffekte innerhalb von Liquid-Glass-Materialien.

Ziel ist die Vermeidung redundanter GPU-Berechnungen durch Wiederverwendung gültiger Reflexionsinformationen und bereits berechneter Effektergebnisse.

Unveränderte Reflexionen SOLLEN ohne erneute Berechnung dargestellt werden.

## 2. Architekturprinzipien

- **Incremental:** Nur betroffene Reflexionsbereiche werden aktualisiert.
- **Dependency-Driven:** Änderungen relevanter Eingaben bestimmen die Invalidierung.
- **Spatial:** Reflexionsquellen und räumliche Abhängigkeiten werden berücksichtigt.
- **Retained:** Gültige Reflexionsergebnisse bleiben erhalten.
- **Cache-Aware:** Berechnete Zwischenergebnisse werden wiederverwendet.
- **Shared:** Kompatible Reflexionsdaten können gemeinsam genutzt werden.
- **Adaptive:** Reflexionsauflösung und Berechnungsqualität sind anpassbar.
- **Deterministic:** Identische Eingaben erzeugen reproduzierbare Cache-Entscheidungen.

## 3. Architektur

Nova Liquid Glass Reflection Caching verwendet Nova Material Effect Caching als zentrale Cache-Infrastruktur.

Liquid-Glass-spezifische Komponenten sind:

| Komponente | Verantwortung |
|---|---|
| Reflection Cache Resolver | Ermittlung vorhandener Reflexionsergebnisse |
| Reflection Dependency Mapper | Zuordnung relevanter Eingabeänderungen |
| Reflection Region Mapper | Berechnung betroffener Eingabe- und Ausgaberegionen |
| Reflection Cache Validator | Prüfung der Ergebnisgültigkeit |
| Reflection Update Planner | Planung notwendiger Neuberechnungen |

Eine eigenständige GPU-Speicherverwaltung DARF nicht entstehen.

## 4. Reflection Cache Model

Jeder Reflection-Cache-Eintrag MUSS mindestens folgende Informationen besitzen:

| Eigenschaft | Beschreibung |
|---|---|
| Cache ID | Eindeutige Cache-Identifikation |
| Material Reference | Zugehörige Liquid-Glass-Instanz |
| Reflection Source | Verwendete Reflexionsquelle |
| Input Version | Version relevanter Eingabedaten |
| Reflection Parameters | Reflexionsstärke, Richtung und Effektparameter |
| Reflection Mapping | Optionale vorberechnete Reflexionskoordinaten |
| Quality Level | Verwendete Qualitätsstufe |
| Resolution | Auflösung des Ergebnisses |
| Valid Regions | Gültige Ergebnisbereiche |
| Invalid Regions | Neu zu berechnende Bereiche |
| Resource Reference | Zugehörige GPU-Ressource |
| Cache Version | Version des Cache-Zustands |

Cache-Schlüssel MÜSSEN alle ergebnisrelevanten Parameter berücksichtigen.

## 5. Cacheable Reflection Data

NovaOS SOLL zwischen zwei Arten von Reflection-Ergebnissen unterscheiden.

### 5.1 Reflection Mapping Cache

Speichert vorberechnete Reflexionsinformationen:

- Reflexionsrichtungen.
- Oberflächennormalen.
- Reflexionskoordinaten.
- Geometrieabhängige Reflexionsparameter.
- Statische Highlight-Masken.

Diese Daten können unabhängig vom aktuellen Inhalt einer Reflexionsquelle gültig bleiben.

### 5.2 Reflection Result Cache

Speichert bereits berechnete visuelle Reflexionsergebnisse.

Diese hängen zusätzlich von den verwendeten Reflexionsquellen, Materialparametern und gegebenenfalls Beleuchtungsinformationen ab.

Änderungen einer Reflexionsquelle MÜSSEN deshalb nicht automatisch den Reflection Mapping Cache invalidieren.

## 6. Reflection Dependency Tracking

Nova Liquid Glass Reflection Caching MUSS mit Nova Liquid Glass Dependency Tracking zusammenarbeiten.

Relevante Abhängigkeiten umfassen:

- Reflexionsquellen.
- Glasgeometrie.
- Oberflächenkrümmung.
- Reflexionsstärke.
- Oberflächenrauheit.
- Transformationen.
- Lichtquellen und Highlights.
- Sampling-Konfiguration.
- Qualitätsstufe.
- Zeitabhängige Effektparameter.

Änderungen MÜSSEN nur diejenigen Cache-Ergebnisse invalidieren, deren Eingaben tatsächlich betroffen sind.

## 7. Spatial Reflection Mapping

Reflexionen können Bildinformationen außerhalb der sichtbaren Glasregion verwenden.

NovaOS MUSS deshalb unterscheiden zwischen:

- **Output Region:** Sichtbarer Reflexionsbereich.
- **Source Region:** Benötigter Bereich der Reflexionsquelle.
- **Source Damage:** Veränderte Pixel der Reflexionsquelle.
- **Invalid Region:** Betroffene Reflexions-Ausgabepixel.

Die räumliche Abbildung MUSS die verwendeten Reflexionskoordinaten, Transformationen und Sampling-Filter berücksichtigen.

Eine Änderung der Reflexionsquelle MUSS alle Ausgabepixel invalidieren, deren Abtastbereich diese Änderung berührt.

Bei nicht sicher bestimmbaren Reflexionsabhängigkeiten MUSS konservativ invalidiert werden.

## 8. Incremental Reflection Updates

NovaOS MUSS partielle Reflexionsaktualisierungen unterstützen.

Der logische Ablauf lautet:

`Input Change → Dependency Analysis → Spatial Mapping → Cache Invalidation → Partial Reflection Rendering → Cache Update`

Dabei gilt:

1. Änderungen relevanter Eingaben werden erkannt.
2. Betroffene Reflection-Cache-Einträge werden ermittelt.
3. Räumliche Auswirkungen werden berechnet.
4. Ungültige Ergebnisregionen werden markiert.
5. Gültige Reflexionsdaten bleiben erhalten.
6. Nur notwendige Bereiche werden neu berechnet.

Eine vollständige Neuberechnung bleibt zulässig, wenn sie effizienter oder für die Korrektheit erforderlich ist.

## 9. Static und Dynamic Reflections

NovaOS SOLL statische und dynamische Reflexionen unterscheiden.

### Statische Reflexionen

- Unveränderte Reflexionsquellen SOLLEN vollständig wiederverwendet werden.
- Statische Reflexionsparameter SOLLEN vorberechnet werden.
- Wiederholte GPU-Berechnungen SOLLEN vermieden werden.

### Dynamische Reflexionen

- Änderungen der Reflexionsquelle MÜSSEN erkannt werden.
- Nur betroffene Reflexionsregionen SOLLEN aktualisiert werden.
- Animationen MÜSSEN ihre zeitabhängigen Abhängigkeiten deklarieren.
- Unveränderte dynamische Zustände DÜRFEN keine unnötigen Aktualisierungen auslösen.

Eine Reflexion gilt nicht allein deshalb als dynamisch, weil sie Bestandteil eines animierbaren Materials ist.

## 10. Shared Reflection Caching

NovaOS SOLL kompatible Reflexionsdaten gemeinsam verwenden können.

Dabei gilt:

- Identische Reflection Maps können wiederverwendet werden.
- Gemeinsame Oberflächenparameter können mehrfach genutzt werden.
- Reflexionsergebnisse benötigen kompatible Reflexionsquellen und Eingabeversionen.
- Unterschiedliche Glaspositionen und Transformationen MÜSSEN berücksichtigt werden.
- Gemeinsame Ressourcen MÜSSEN korrekt synchronisiert werden.

Geschützte Oberflächeninhalte DÜRFEN nicht über gemeinsam verwendete Cache-Ressourcen offengelegt werden.

## 11. Adaptive Reflection Quality

Nova Liquid Glass Reflection Caching MUSS unterschiedliche Qualitätsstufen unterstützen.

Anpassbare Eigenschaften umfassen:

- Reflexionsauflösung.
- Sampling-Dichte.
- Filterqualität.
- Genauigkeit der Reflexionsberechnung.
- Komplexität der Reflection Maps.
- Anzahl und Qualität zusätzlicher Reflexionsdetails.

Qualitätsänderungen MÜSSEN bei der Cache-Validierung berücksichtigt werden.

Die Auswahl erfolgt über Nova Adaptive Material Quality.

## 12. Render Graph Integration

Nova Liquid Glass Reflection Caching MUSS mit Nova Render Graph zusammenarbeiten.

Dabei gilt:

- Gültige Reflexionsergebnisse können Render-Passes ersetzen.
- Vorhandene Reflection Maps SOLLEN wiederverwendet werden.
- Ungültige Regionen erzeugen notwendige Renderoperationen.
- Abhängigkeiten zwischen Reflexionsquellen und Materialeffekten MÜSSEN korrekt geordnet werden.
- GPU Resource Barriers und Synchronisation bleiben verbindlich.

Nicht benötigte Reflexionsberechnungen SOLLEN vollständig entfallen.

## 13. Resource Management

Reflection-Cache-Ressourcen MÜSSEN über Nova GPU Resource Management verwaltet werden.

Dabei gilt:

- GPU-Speicherbudgets MÜSSEN berücksichtigt werden.
- Temporäre Ressourcen SOLLEN wiederverwendet werden.
- Häufig verwendete Reflection Maps SOLLEN bevorzugt erhalten bleiben.
- Nicht benötigte Cache-Einträge können freigegeben werden.
- Aktive GPU-Ressourcen DÜRFEN nicht vorzeitig freigegeben werden.

Cache Eviction DARF die funktionale Korrektheit nicht beeinträchtigen.

## 14. Synchronisation und Konsistenz

NovaOS MUSS konsistente Reflexionsergebnisse gewährleisten.

Dabei gilt:

- Cache-Einträge sind an definierte Eingabeversionen gebunden.
- Laufende GPU-Operationen verwenden gültige Ressourcenreferenzen.
- Veraltete Berechnungen DÜRFEN neuere Cache-Versionen nicht überschreiben.
- Partielle Aktualisierungen MÜSSEN bestehende gültige Regionen erhalten.
- Neue Ergebnisse DÜRFEN erst nach erfolgreicher Berechnung und erforderlicher Synchronisation als verwendbar gelten.
- Unterschiedliche Reflexionsversionen DÜRFEN keine inkonsistente sichtbare Materialdarstellung erzeugen.

## 15. Fehlerbehandlung und Fallback

NovaOS MUSS folgende Fehler kontrolliert behandeln:

- Ungültige Reflection Maps.
- Fehlende Reflexionsquellen.
- Unvollständige Damage-Informationen.
- Nicht unterstützte Reflexionsverfahren.
- Fehlgeschlagene GPU-Berechnungen.
- Ressourcenmangel.
- GPU-Geräteverlust.

Bei unklarer Cache-Gültigkeit MUSS eine sichere Neuberechnung erfolgen.

Ist Reflection nicht verfügbar, MUSS Nova Material Rendering Fallback eine geeignete Ersatzdarstellung bereitstellen.

## 16. Verbindliche Anforderungen

1. NovaOS MUSS Reflexionsergebnisse zwischenspeichern können.
2. Nova Material Effect Caching MUSS als zentrale Cache-Infrastruktur verwendet werden.
3. Reflection Mapping Cache und Reflection Result Cache MÜSSEN unterscheidbar sein.
4. Reflection-Cache-Einträge MÜSSEN eindeutig identifizierbar und versionierbar sein.
5. Änderungen von Reflexionsquellen MÜSSEN relevante Cache-Ergebnisse invalidieren.
6. Änderungen der Reflexionsparameter MÜSSEN berücksichtigt werden.
7. Räumliche Reflexionsabhängigkeiten MÜSSEN korrekt ausgewertet werden.
8. Partielle Cache-Invalidierung MUSS unterstützt werden.
9. Inkrementelle Reflexionsberechnung MUSS möglich sein.
10. Unveränderte Reflexionsdaten SOLLEN wiederverwendet werden.
11. Statische und dynamische Reflexionen SOLLEN unterschieden werden.
12. Gemeinsame Reflection Maps SOLLEN unterstützt werden.
13. Unterschiedliche Qualitätsstufen und Auflösungen MÜSSEN berücksichtigt werden.
14. Nova Liquid Glass Dependency Tracking MUSS integriert werden.
15. Nova Damage Tracking MUSS relevante Änderungen bereitstellen.
16. Nova Render Graph MUSS gültige Cache-Ergebnisse verwenden können.
17. GPU-Ressourcen und Synchronisation MÜSSEN korrekt verwaltet werden.
18. Ungültige oder veraltete Cache-Ergebnisse DÜRFEN nicht verwendet werden.
19. Geschützte Reflexionsquellen MÜSSEN isoliert bleiben.
20. Bei fehlgeschlagener Reflexionsberechnung MUSS ein geeigneter Fallback verfügbar sein.
21. Im deterministischen Modus MÜSSEN Cache- und Invalidierungsentscheidungen bei identischen Eingaben reproduzierbar sein.

## 17. Ergebnis

Nova Liquid Glass Reflection Caching ermöglicht die effiziente Wiederverwendung bereits berechneter Reflexionsinformationen und Reflexionsergebnisse.

Durch die Trennung von Reflection Mapping und Reflection Result Caching, räumliche Abhängigkeitsanalyse sowie inkrementelle Aktualisierung werden redundante GPU-Berechnungen vermieden.

Damit kann NovaOS komplexe Reflexionseffekte innerhalb seiner Liquid-Glass-Oberfläche darstellen, ohne unveränderte Reflexionsbereiche permanent neu berechnen zu müssen.