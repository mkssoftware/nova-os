# NPSPEC-LIQUID-REFRACTIONCACHE-0001 – Nova Liquid Glass Refraction Caching

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Material Engine / Nova Liquid Glass

## 1. Zweck

Nova Liquid Glass Refraction Caching definiert die Zwischenspeicherung und inkrementelle Aktualisierung bereits berechneter Lichtbrechungseffekte innerhalb von Liquid-Glass-Materialien.

Ziel ist die Vermeidung redundanter GPU-Berechnungen durch Wiederverwendung gültiger Refraction-Daten und bereits berechneter Effektergebnisse.

Unveränderte Lichtbrechungseffekte SOLLEN ohne erneute Berechnung dargestellt werden.

## 2. Architekturprinzipien

- **Incremental:** Nur betroffene Refraction-Bereiche werden aktualisiert.
- **Dependency-Driven:** Änderungen relevanter Eingaben bestimmen die Invalidierung.
- **Spatial:** Abtastverschiebungen und räumliche Abhängigkeiten werden berücksichtigt.
- **Retained:** Gültige Refraction-Ergebnisse bleiben erhalten.
- **Cache-Aware:** Berechnete Zwischenergebnisse werden wiederverwendet.
- **Shared:** Kompatible Refraction-Daten können gemeinsam genutzt werden.
- **Adaptive:** Berechnungsauflösung und Sampling-Qualität sind anpassbar.
- **Deterministic:** Identische Eingaben erzeugen reproduzierbare Cache-Entscheidungen.

## 3. Architektur

Nova Liquid Glass Refraction Caching verwendet Nova Material Effect Caching als zentrale Cache-Infrastruktur.

Liquid-Glass-spezifische Komponenten sind:

| Komponente | Verantwortung |
|---|---|
| Refraction Cache Resolver | Ermittlung vorhandener Refraction-Ergebnisse |
| Refraction Dependency Mapper | Zuordnung relevanter Eingabeänderungen |
| Refraction Region Mapper | Berechnung betroffener Eingabe- und Ausgaberegionen |
| Refraction Cache Validator | Prüfung der Ergebnisgültigkeit |
| Refraction Update Planner | Planung notwendiger Neuberechnungen |

Eine eigenständige GPU-Speicherverwaltung DARF nicht entstehen.

## 4. Refraction Cache Model

Jeder Refraction-Cache-Eintrag MUSS mindestens folgende Informationen besitzen:

| Eigenschaft | Beschreibung |
|---|---|
| Cache ID | Eindeutige Cache-Identifikation |
| Material Reference | Zugehörige Liquid-Glass-Instanz |
| Backdrop Reference | Verwendete Hintergrundquelle |
| Input Version | Version relevanter Eingabedaten |
| Refraction Parameters | Brechungsstärke und Effektparameter |
| Distortion Map | Optionale Verschiebungsinformationen |
| Quality Level | Verwendete Qualitätsstufe |
| Resolution | Auflösung des Ergebnisses |
| Valid Regions | Gültige Ergebnisbereiche |
| Invalid Regions | Neu zu berechnende Bereiche |
| Resource Reference | Zugehörige GPU-Ressource |
| Cache Version | Version des Cache-Zustands |

Cache-Schlüssel MÜSSEN alle ergebnisrelevanten Parameter berücksichtigen.

## 5. Cacheable Refraction Data

NovaOS SOLL zwischen zwei Arten von Refraction-Ergebnissen unterscheiden.

### 5.1 Refraction Mapping Cache

Speichert vorberechnete Informationen zur Lichtbrechung:

- Verschiebungsvektoren.
- Refraction-Koordinaten.
- Oberflächennormalen.
- Verzerrungsparameter.

Diese Daten können unabhängig vom aktuellen Hintergrundinhalt gültig bleiben.

### 5.2 Refraction Result Cache

Speichert das bereits berechnete visuelle Refraction-Ergebnis.

Dieses hängt zusätzlich vom abgetasteten Hintergrundinhalt ab.

Eine Änderung des Backdrops MUSS deshalb nicht automatisch den Refraction Mapping Cache invalidieren.

## 6. Refraction Dependency Tracking

Nova Liquid Glass Refraction Caching MUSS mit Nova Liquid Glass Dependency Tracking zusammenarbeiten.

Relevante Abhängigkeiten umfassen:

- Hintergrundinhalt.
- Glasgeometrie.
- Oberflächenkrümmung.
- Refraction-Stärke.
- Distortion-Parameter.
- Transformationen.
- Sampling-Konfiguration.
- Qualitätsstufe.

Änderungen MÜSSEN nur diejenigen Cache-Ergebnisse invalidieren, deren Eingaben tatsächlich betroffen sind.

## 7. Spatial Refraction Mapping

Refraction kann Hintergrundpixel außerhalb der sichtbaren Glasregion abtasten.

NovaOS MUSS deshalb unterscheiden zwischen:

- **Output Region:** Sichtbarer Refraction-Ausgabebereich.
- **Sampling Region:** Tatsächlich benötigter Hintergrundbereich.
- **Backdrop Damage:** Veränderte Hintergrundpixel.
- **Invalid Region:** Betroffene Refraction-Ausgabepixel.

Die räumliche Abbildung MUSS die verwendeten Verschiebungsvektoren und Sampling-Filter berücksichtigen.

Eine Hintergrundänderung MUSS alle Ausgabepixel invalidieren, deren Abtastbereich diese Änderung berührt.

Bei unbekannten Verschiebungsgrenzen MUSS konservativ invalidiert werden.

## 8. Incremental Refraction Updates

NovaOS MUSS partielle Refraction-Aktualisierungen unterstützen.

Der logische Ablauf lautet:

`Input Change → Dependency Analysis → Spatial Mapping → Cache Invalidation → Partial Refraction Rendering → Cache Update`

Dabei gilt:

1. Änderungen relevanter Eingaben werden erkannt.
2. Betroffene Refraction-Cache-Einträge werden ermittelt.
3. Räumliche Auswirkungen werden berechnet.
4. Ungültige Ergebnisregionen werden markiert.
5. Gültige Refraction-Daten bleiben erhalten.
6. Nur notwendige Bereiche werden neu berechnet.

Eine vollständige Neuberechnung bleibt zulässig, wenn sie effizienter oder für die Korrektheit erforderlich ist.

## 9. Shared Refraction Caching

NovaOS SOLL kompatible Refraction-Daten gemeinsam verwenden können.

Dabei gilt:

- Identische Distortion Maps können wiederverwendet werden.
- Gemeinsame Oberflächenparameter können mehrfach genutzt werden.
- Refraction-Ergebnisse benötigen kompatible Hintergrundzustände.
- Unterschiedliche Glaspositionen MÜSSEN bei der Cache-Validierung berücksichtigt werden.
- Gemeinsame Ressourcen MÜSSEN korrekt synchronisiert werden.

Geschützte Hintergrundinhalte DÜRFEN nicht über gemeinsam verwendete Cache-Ressourcen offengelegt werden.

## 10. Adaptive Refraction Quality

Nova Liquid Glass Refraction Caching MUSS unterschiedliche Qualitätsstufen unterstützen.

Anpassbare Eigenschaften umfassen:

- Refraction-Auflösung.
- Sampling-Dichte.
- Filterqualität.
- Genauigkeit der Verschiebungsberechnung.
- Komplexität der Distortion Map.

Qualitätsänderungen MÜSSEN bei der Cache-Validierung berücksichtigt werden.

Die Auswahl erfolgt über Nova Adaptive Material Quality.

## 11. Render Graph Integration

Nova Liquid Glass Refraction Caching MUSS mit Nova Render Graph zusammenarbeiten.

Dabei gilt:

- Gültige Refraction-Ergebnisse können Render-Passes ersetzen.
- Vorhandene Refraction Maps SOLLEN wiederverwendet werden.
- Ungültige Regionen erzeugen notwendige Renderoperationen.
- Backdrop- und Refraction-Abhängigkeiten MÜSSEN korrekt geordnet werden.
- GPU Resource Barriers und Synchronisation bleiben verbindlich.

Nicht benötigte Refraction-Berechnungen SOLLEN vollständig entfallen.

## 12. Resource Management

Refraction-Cache-Ressourcen MÜSSEN über Nova GPU Resource Management verwaltet werden.

Dabei gilt:

- GPU-Speicherbudgets MÜSSEN berücksichtigt werden.
- Temporäre Ressourcen SOLLEN wiederverwendet werden.
- Häufig verwendete Refraction Maps SOLLEN bevorzugt erhalten bleiben.
- Nicht benötigte Cache-Einträge können freigegeben werden.
- Aktive GPU-Ressourcen DÜRFEN nicht vorzeitig freigegeben werden.

Cache Eviction DARF die funktionale Korrektheit nicht beeinträchtigen.

## 13. Synchronisation und Konsistenz

NovaOS MUSS konsistente Refraction-Ergebnisse gewährleisten.

Dabei gilt:

- Cache-Einträge sind an definierte Eingabeversionen gebunden.
- Laufende GPU-Operationen verwenden gültige Ressourcenreferenzen.
- Veraltete Berechnungen DÜRFEN neuere Cache-Versionen nicht überschreiben.
- Partielle Aktualisierungen MÜSSEN bestehende gültige Regionen erhalten.
- Neue Ergebnisse DÜRFEN erst nach erfolgreicher Berechnung und erforderlicher Synchronisation als verwendbar gelten.

## 14. Fehlerbehandlung und Fallback

NovaOS MUSS folgende Fehler kontrolliert behandeln:

- Ungültige Refraction Maps.
- Fehlende Backdrop-Ressourcen.
- Unvollständige Damage-Informationen.
- Nicht unterstützte Refraction-Verfahren.
- Fehlgeschlagene GPU-Berechnungen.
- Ressourcenmangel.
- GPU-Geräteverlust.

Bei unklarer Cache-Gültigkeit MUSS eine sichere Neuberechnung erfolgen.

Ist Refraction nicht verfügbar, MUSS Nova Material Rendering Fallback eine geeignete Ersatzdarstellung bereitstellen.

## 15. Verbindliche Anforderungen

1. NovaOS MUSS Refraction-Ergebnisse zwischenspeichern können.
2. Nova Material Effect Caching MUSS als zentrale Cache-Infrastruktur verwendet werden.
3. Refraction Mapping Cache und Refraction Result Cache MÜSSEN unterscheidbar sein.
4. Refraction-Cache-Einträge MÜSSEN eindeutig identifizierbar und versionierbar sein.
5. Hintergrundänderungen MÜSSEN relevante Refraction-Ergebnisse invalidieren.
6. Änderungen der Refraction-Parameter MÜSSEN berücksichtigt werden.
7. Räumliche Abtastverschiebungen MÜSSEN korrekt ausgewertet werden.
8. Partielle Cache-Invalidierung MUSS unterstützt werden.
9. Inkrementelle Refraction-Berechnung MUSS möglich sein.
10. Unveränderte Refraction-Daten SOLLEN wiederverwendet werden.
11. Gemeinsame Refraction Maps SOLLEN unterstützt werden.
12. Unterschiedliche Qualitätsstufen und Auflösungen MÜSSEN berücksichtigt werden.
13. Nova Liquid Glass Dependency Tracking MUSS integriert werden.
14. Nova Damage Tracking MUSS relevante Änderungen bereitstellen.
15. Nova Render Graph MUSS gültige Cache-Ergebnisse verwenden können.
16. GPU-Ressourcen und Synchronisation MÜSSEN korrekt verwaltet werden.
17. Ungültige oder veraltete Cache-Ergebnisse DÜRFEN nicht verwendet werden.
18. Geschützte Hintergrundinhalte MÜSSEN isoliert bleiben.
19. Bei fehlgeschlagener Refraction-Berechnung MUSS ein geeigneter Fallback verfügbar sein.
20. Im deterministischen Modus MÜSSEN Cache- und Invalidierungsentscheidungen bei identischen Eingaben reproduzierbar sein.

## 16. Ergebnis

Nova Liquid Glass Refraction Caching ermöglicht die effiziente Wiederverwendung bereits berechneter Lichtbrechungsinformationen und Refraction-Ergebnisse.

Durch die Trennung von Refraction Mapping und Refraction Result Caching, räumliche Abhängigkeitsanalyse sowie inkrementelle Aktualisierung werden redundante GPU-Berechnungen vermieden.

Damit kann NovaOS komplexe Lichtbrechungseffekte innerhalb seiner Liquid-Glass-Oberfläche darstellen, ohne unveränderte Refraction-Bereiche permanent neu berechnen zu müssen.