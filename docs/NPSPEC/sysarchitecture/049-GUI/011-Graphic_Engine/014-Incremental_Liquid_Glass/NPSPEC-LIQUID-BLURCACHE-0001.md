# NPSPEC-LIQUID-BLURCACHE-0001 – Nova Liquid Glass Blur Caching

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Material Engine / Nova Liquid Glass

## 1. Zweck

Nova Liquid Glass Blur Caching definiert die Zwischenspeicherung und inkrementelle Aktualisierung bereits berechneter Blur-Ergebnisse für Liquid-Glass-Materialien.

Ziel ist die Vermeidung redundanter GPU-Berechnungen, indem unveränderte Hintergrundbereiche und gültige Blur-Ergebnisse wiederverwendet werden.

Eine erneute Blur-Berechnung DARF nur erfolgen, wenn relevante Eingaben verändert wurden, ein gültiges Ergebnis fehlt oder eine Neuberechnung aus anderen Gründen erforderlich ist.

## 2. Architekturprinzipien

- **Incremental:** Nur betroffene Blur-Bereiche werden aktualisiert.
- **Region-Based:** Blur-Ergebnisse besitzen räumlich begrenzte Gültigkeitsbereiche.
- **Dependency-Driven:** Hintergrundänderungen bestimmen die Invalidierung.
- **Cache-Aware:** Bereits berechnete Ergebnisse werden wiederverwendet.
- **Filter-Aware:** Filterradien und Sampling-Bereiche werden berücksichtigt.
- **Shared:** Kompatible Blur-Ergebnisse können gemeinsam genutzt werden.
- **Adaptive:** Auflösung und Filterqualität können angepasst werden.
- **Deterministic:** Identische Eingaben erzeugen reproduzierbare Cache-Entscheidungen.

## 3. Architektur

Nova Liquid Glass Blur Caching verwendet Nova Material Effect Caching als zentrale Cache-Infrastruktur.

Liquid-Glass-spezifische Komponenten sind:

| Komponente | Verantwortung |
|---|---|
| Blur Cache Resolver | Ermittlung vorhandener Blur-Ergebnisse |
| Blur Dependency Mapper | Zuordnung relevanter Hintergrundänderungen |
| Blur Region Mapper | Berechnung betroffener Eingabe- und Ausgaberegionen |
| Blur Cache Validator | Prüfung der Ergebnisgültigkeit |
| Blur Update Planner | Planung erforderlicher Neuberechnungen |

Eine eigenständige GPU-Speicherverwaltung DARF nicht entstehen.

## 4. Blur Cache Model

Jeder Blur-Cache-Eintrag MUSS mindestens folgende Informationen besitzen:

| Eigenschaft | Beschreibung |
|---|---|
| Cache ID | Eindeutige Cache-Identifikation |
| Backdrop Reference | Zugehörige Hintergrundquelle |
| Backdrop Version | Version der relevanten Hintergrunddaten |
| Blur Parameters | Filterradius, Filterverfahren und Sampling-Einstellungen |
| Quality Level | Verwendete Qualitätsstufe |
| Resolution | Auflösung des Blur-Ergebnisses |
| Color Format | Farbformat und Farbraum |
| Valid Regions | Gültige Ergebnisbereiche |
| Invalid Regions | Neu zu berechnende Bereiche |
| Resource Reference | GPU-Ressource des Blur-Ergebnisses |
| Cache Version | Version des Cache-Zustands |

Cache-Schlüssel MÜSSEN alle ergebnisrelevanten Parameter berücksichtigen.

## 5. Blur Dependency Tracking

Nova Liquid Glass Blur Caching MUSS mit Nova Liquid Glass Dependency Tracking zusammenarbeiten.

Dabei gilt:

- Hintergrundänderungen MÜSSEN betroffene Blur-Ergebnisse invalidieren.
- Änderungen des Filterradius MÜSSEN berücksichtigt werden.
- Änderungen der Blur-Qualität können neue Cache-Ergebnisse erfordern.
- Änderungen der Glasgeometrie MÜSSEN auf die benötigten Sampling-Regionen abgebildet werden.
- Unabhängige Blur-Ergebnisse SOLLEN gültig bleiben.

Eine Änderung der Materialfarbe MUSS nicht automatisch eine erneute Blur-Berechnung auslösen, sofern die Material-Pipeline diese erst nach dem Blur verarbeitet.

## 6. Filter-Aware Region Mapping

Blur-Filter benötigen Eingabepixel außerhalb der eigentlichen Ausgaberegion.

NovaOS MUSS deshalb zwischen folgenden Bereichen unterscheiden:

- **Output Region:** Pixelbereich des benötigten Blur-Ergebnisses.
- **Sampling Region:** Hintergrundbereich, der für die Berechnung benötigt wird.
- **Damage Region:** Tatsächlich veränderter Hintergrundbereich.
- **Invalid Region:** Blur-Ausgabebereich, der durch die Änderung betroffen ist.

Die Invalidierung MUSS den effektiven Filtereinfluss einschließlich mehrstufiger Filterverfahren berücksichtigen.

Bei nicht sicher bestimmbaren Filtergrenzen MUSS konservativ invalidiert werden.

## 7. Incremental Blur Updates

NovaOS MUSS partielle Blur-Aktualisierungen unterstützen.

Der logische Ablauf lautet:

`Backdrop Damage → Blur Dependency Analysis → Region Expansion → Cache Invalidation → Partial Blur Rendering → Cache Update`

Dabei gilt:

1. Hintergrundänderungen werden erfasst.
2. Betroffene Blur-Cache-Einträge werden ermittelt.
3. Die Auswirkungen des Filters werden räumlich berechnet.
4. Nur betroffene Ausgaberegionen werden invalidiert.
5. Gültige Blur-Bereiche bleiben erhalten.
6. Ungültige Bereiche werden bei Bedarf neu berechnet.

Eine vollständige Neuberechnung bleibt zulässig, wenn sie effizienter oder für die Korrektheit erforderlich ist.

## 8. Shared Blur Caching

NovaOS SOLL identische Blur-Ergebnisse zwischen mehreren Liquid-Glass-Instanzen wiederverwenden.

Voraussetzungen sind:

- Kompatible Hintergrundquelle.
- Identischer relevanter Hintergrundzustand.
- Kompatible Blur-Parameter.
- Gleiche oder kompatible Auflösung.
- Kompatibler Farbraum.
- Zulässige Ressourcenfreigabe.

Gemeinsame Blur-Ergebnisse MÜSSEN bei Änderungen ihrer Eingaben korrekt invalidiert werden.

Geschützte Hintergrundinhalte DÜRFEN nicht über gemeinsam verwendete Cache-Ressourcen offengelegt werden.

## 9. Multi-Resolution Blur Caching

NovaOS SOLL Blur-Ergebnisse unterschiedlicher Auflösungen unterstützen.

Dabei gilt:

- Reduzierte Auflösungen können den Berechnungsaufwand verringern.
- Unterschiedliche Auflösungen benötigen unterscheidbare Cache-Konfigurationen.
- Skalierungs- und Filterparameter MÜSSEN berücksichtigt werden.
- Qualitätswechsel MÜSSEN betroffene Cache-Ergebnisse invalidieren.
- Bereits vorhandene kompatible Ergebnisse SOLLEN wiederverwendet werden.

Die Auswahl erfolgt über Nova Adaptive Material Quality.

## 10. Render Graph Integration

Nova Liquid Glass Blur Caching MUSS mit Nova Render Graph zusammenarbeiten.

Dabei gilt:

- Gültige Blur-Ergebnisse können entsprechende Render-Passes ersetzen.
- Partielle Aktualisierungen erzeugen nur notwendige Blur-Operationen.
- Mehrstufige Blur-Verfahren MÜSSEN ihre internen Abhängigkeiten berücksichtigen.
- Temporäre Ressourcen SOLLEN wiederverwendet werden.
- GPU Resource Barriers und Synchronisation bleiben verbindlich.

Ein übersprungener Blur-Pass MUSS durch ein nachweislich gültiges Ergebnis ersetzt werden.

## 11. Resource Management

Blur-Cache-Ressourcen MÜSSEN über Nova GPU Resource Management verwaltet werden.

Dabei gilt:

- Speicherbudgets MÜSSEN eingehalten werden.
- Nicht benötigte Cache-Einträge können freigegeben werden.
- Häufig verwendete Ergebnisse SOLLEN bevorzugt erhalten bleiben.
- Aktive GPU-Ressourcen DÜRFEN nicht vorzeitig freigegeben werden.
- Cache Eviction DARF die funktionale Korrektheit nicht beeinträchtigen.

Ein fehlendes Cache-Ergebnis MUSS bei Bedarf neu berechnet werden können.

## 12. Synchronisation und Konsistenz

NovaOS MUSS konsistente Blur-Ergebnisse gewährleisten.

Dabei gilt:

- Cache-Einträge sind an definierte Eingabeversionen gebunden.
- Laufende GPU-Operationen verwenden gültige Ressourcenreferenzen.
- Veraltete Berechnungen DÜRFEN neuere Cache-Versionen nicht überschreiben.
- Partielle Aktualisierungen MÜSSEN bestehende gültige Regionen erhalten.
- Neue Ergebnisse DÜRFEN erst nach erfolgreicher Berechnung und erforderlicher Synchronisation als verwendbar gelten.

## 13. Fehlerbehandlung und Fallback

NovaOS MUSS folgende Fehler kontrolliert behandeln:

- Ungültige Cache-Einträge.
- Fehlende Backdrop-Ressourcen.
- Unvollständige Damage-Informationen.
- Nicht unterstützte Blur-Verfahren.
- Fehlgeschlagene GPU-Berechnungen.
- Ressourcenmangel.
- GPU-Geräteverlust.

Bei unklarer Cache-Gültigkeit MUSS eine sichere Neuberechnung erfolgen.

Ist Blur nicht verfügbar, MUSS Nova Material Rendering Fallback eine geeignete Ersatzdarstellung bereitstellen.

## 14. Verbindliche Anforderungen

1. NovaOS MUSS Blur-Ergebnisse zwischenspeichern können.
2. Nova Material Effect Caching MUSS als zentrale Cache-Infrastruktur verwendet werden.
3. Blur-Cache-Einträge MÜSSEN eindeutig identifizierbar und versionierbar sein.
4. Hintergrundänderungen MÜSSEN relevante Cache-Ergebnisse invalidieren.
5. Filterradien und Sampling-Abhängigkeiten MÜSSEN berücksichtigt werden.
6. Partielle Cache-Invalidierung MUSS unterstützt werden.
7. Inkrementelle Blur-Berechnung MUSS möglich sein.
8. Unveränderte Blur-Ergebnisse SOLLEN wiederverwendet werden.
9. Gemeinsame Blur-Ergebnisse SOLLEN unterstützt werden.
10. Unterschiedliche Qualitätsstufen und Auflösungen MÜSSEN berücksichtigt werden.
11. Nova Liquid Glass Dependency Tracking MUSS integriert werden.
12. Nova Damage Tracking MUSS relevante Änderungen bereitstellen.
13. Nova Render Graph MUSS gültige Cache-Ergebnisse verwenden können.
14. GPU-Ressourcen und Synchronisation MÜSSEN korrekt verwaltet werden.
15. Ungültige oder veraltete Cache-Ergebnisse DÜRFEN nicht verwendet werden.
16. Geschützte Hintergrundinhalte MÜSSEN isoliert bleiben.
17. Bei fehlgeschlagener Blur-Berechnung MUSS ein geeigneter Fallback verfügbar sein.
18. Im deterministischen Modus MÜSSEN Cache- und Invalidierungsentscheidungen bei identischen Eingaben reproduzierbar sein.

## 15. Ergebnis

Nova Liquid Glass Blur Caching ermöglicht die effiziente Wiederverwendung bereits berechneter Blur-Ergebnisse.

Durch filterabhängige Damage-Analyse, räumliche Cache-Invalidierung, inkrementelle Aktualisierung und gemeinsame Nutzung kompatibler Ergebnisse werden redundante GPU-Berechnungen vermieden.

Damit können Liquid-Glass-Oberflächen auch bei mehreren überlappenden Fenstern effizient dargestellt werden, ohne unveränderte Hintergrundbereiche permanent neu weichzeichnen zu müssen.