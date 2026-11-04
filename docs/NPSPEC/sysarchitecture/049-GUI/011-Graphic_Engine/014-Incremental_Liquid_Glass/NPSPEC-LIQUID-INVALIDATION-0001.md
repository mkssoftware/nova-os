# NPSPEC-LIQUID-INVALIDATION-0001 – Nova Liquid Glass Cache Invalidation

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Material Engine / Nova Liquid Glass

## 1. Zweck

Nova Liquid Glass Cache Invalidation definiert die gezielte Ungültigsetzung zwischengespeicherter Liquid-Glass-Ergebnisse bei Änderungen ihrer Eingaben oder Abhängigkeiten.

Ziel ist, ausschließlich tatsächlich betroffene Cache-Einträge und Pixelregionen zu invalidieren, während unveränderte Ergebnisse erhalten bleiben.

Die Invalidierung MUSS korrekt, inkrementell und ohne unnötige vollständige Neuberechnungen erfolgen.

## 2. Architekturprinzipien

- **Dependency-Driven:** Invalidierungen basieren auf tatsächlichen Abhängigkeiten.
- **Region-Based:** Nur betroffene Ergebnisregionen werden ungültig.
- **Effect-Specific:** Einzelne Effektstufen können unabhängig invalidiert werden.
- **Incremental:** Unveränderte Cache-Bereiche bleiben gültig.
- **Versioned:** Eingaben und Cache-Ergebnisse besitzen nachvollziehbare Versionen.
- **Conservative:** Unbekannte Abhängigkeiten führen zu sicherer Invalidierung.
- **Coalesced:** Mehrere Änderungen können zusammengefasst werden.
- **Deterministic:** Identische Eingaben erzeugen reproduzierbare Invalidierungsentscheidungen.

## 3. Architektur

Nova Liquid Glass Cache Invalidation verwendet Nova Material Effect Caching und Nova Liquid Glass Dependency Tracking.

Liquid-Glass-spezifische Komponenten sind:

| Komponente | Verantwortung |
|---|---|
| Liquid Invalidation Controller | Koordination der Cache-Invalidierung |
| Dependency Resolver | Ermittlung betroffener Effektabhängigkeiten |
| Region Invalidation Mapper | Berechnung ungültiger Ergebnisregionen |
| Cache State Validator | Prüfung der Cache-Gültigkeit |
| Invalidation Aggregator | Zusammenfassung mehrerer Änderungen |
| Update Request Generator | Anforderung notwendiger Neuberechnungen |

Eine zweite unabhängige Cache- oder Dependency-Graph-Infrastruktur DARF nicht entstehen.

## 4. Invalidation Model

Jede Invalidierungsanforderung MUSS mindestens folgende Informationen enthalten:

| Eigenschaft | Beschreibung |
|---|---|
| Material ID | Betroffene Liquid-Glass-Instanz |
| Source ID | Geänderte Eingabe oder Ressource |
| Source Version | Version der geänderten Eingabe |
| Change Type | Art der Änderung |
| Source Region | Räumlicher Änderungsbereich |
| Affected Effects | Betroffene Effektstufen |
| Invalid Regions | Ungültige Ergebnisbereiche |
| Invalidation Version | Version des Invalidierungszustands |

Invalidierungen MÜSSEN eindeutig einem konsistenten Material- und Eingabezustand zugeordnet werden können.

## 5. Invalidation Triggers

NovaOS MUSS folgende Änderungen berücksichtigen:

| Änderung | Mögliche Auswirkungen |
|---|---|
| Backdrop Change | Blur, Refraction, Composition |
| Geometry Change | Backdrop, Blur, Refraction, Reflection, Composition |
| Blur Parameter Change | Blur und abhängige Effektstufen |
| Refraction Parameter Change | Refraction und abhängige Effektstufen |
| Reflection Source Change | Reflection und abhängige Effektstufen |
| Lighting Change | Lighting und abhängige Effektstufen |
| Tint Change | Tint und Composition |
| Quality Change | Betroffene Effektauflösungen und Pipelines |
| Shader/Pipeline Change | Abhängige Effektergebnisse |
| Resource Replacement | Betroffene ressourcenabhängige Cache-Einträge |

Die tatsächlichen Auswirkungen MÜSSEN aus den definierten Materialabhängigkeiten abgeleitet werden.

## 6. Effect-Specific Invalidation

NovaOS MUSS Cache-Ergebnisse einzelner Effektstufen unabhängig invalidieren können.

Dabei gilt:

- Eine Tint-Änderung MUSS nicht automatisch Blur-Ergebnisse invalidieren.
- Eine Backdrop-Änderung MUSS nur tatsächlich abhängige Ergebnisse betreffen.
- Änderungen einer Reflection Map MÜSSEN nicht zwangsläufig Refraction Maps invalidieren.
- Änderungen einer Effektstufe MÜSSEN an abhängige nachfolgende Stufen weitergegeben werden.
- Unabhängige Zwischenergebnisse SOLLEN gültig bleiben.

Die Invalidierung MUSS der tatsächlichen Material-Pipeline entsprechen.

## 7. Spatial Invalidation

NovaOS MUSS räumlich begrenzte Cache-Invalidierung unterstützen.

Dabei gilt:

- Eingabe-Damage und Ausgabe-Invalidierung MÜSSEN unterschieden werden.
- Blur-Filterradien MÜSSEN berücksichtigt werden.
- Refraction-Abtastverschiebungen MÜSSEN berücksichtigt werden.
- Reflection-Mappings MÜSSEN berücksichtigt werden.
- Transformationen und Clipping MÜSSEN korrekt verarbeitet werden.
- Alte und neue Geometrien MÜSSEN bei Positions- und Größenänderungen berücksichtigt werden.

Eine Änderung außerhalb der sichtbaren Glasfläche kann eine Invalidierung innerhalb der Glasfläche erfordern.

## 8. Invalidation Propagation

Der logische Ablauf lautet:

`Source Change → Dependency Analysis → Spatial Mapping → Cache Invalidation → Render Request`

Dabei gilt:

1. Eine Eingabeänderung wird erfasst.
2. Betroffene Liquid-Glass-Effekte werden ermittelt.
3. Räumliche Auswirkungen werden berechnet.
4. Betroffene Cache-Regionen werden invalidiert.
5. Änderungen werden an abhängige Effektstufen weitergegeben.
6. Notwendige Renderoperationen werden angefordert.

Die Weitergabe MUSS transitiv und vollständig erfolgen.

## 9. Partial und Full Invalidation

NovaOS MUSS zwischen partieller und vollständiger Invalidierung unterscheiden.

### Partial Invalidation

Wird verwendet, wenn betroffene Ergebnisregionen zuverlässig bestimmbar sind.

Unveränderte Regionen bleiben gültig.

### Full Invalidation

Wird verwendet, wenn:

- Abhängigkeiten nicht sicher bestimmbar sind.
- Effektparameter eine vollständige Neuberechnung erfordern.
- Ressourcenformate oder Auflösungen inkompatibel geändert wurden.
- Cache-Zustände inkonsistent sind.
- Eine vollständige Aktualisierung effizienter ist.

Eine vollständige Invalidierung MUSS jederzeit als sicherer Fallback verfügbar sein.

## 10. Invalidation Coalescing

NovaOS SOLL mehrere Invalidierungsanforderungen zusammenfassen.

Dabei gilt:

- Überlappende Regionen können vereinigt werden.
- Mehrere Änderungen derselben Eingabe können gemeinsam verarbeitet werden.
- Redundante Invalidierungen SOLLEN vermieden werden.
- Keine erforderliche Invalidierung DARF verloren gehen.
- Zusammengefasste Anforderungen MÜSSEN alle relevanten Eingabeversionen berücksichtigen.

Die Zusammenfassung DARF die Korrektheit der Materialdarstellung nicht beeinträchtigen.

## 11. Cache State Management

NovaOS definiert folgende Cache-Zustände:

| Zustand | Beschreibung |
|---|---|
| Valid | Ergebnis vollständig gültig |
| Partially Invalid | Einzelne Ergebnisregionen ungültig |
| Invalid | Gesamtes Ergebnis ungültig |
| Updating | Aktualisierung wird ausgeführt |
| Obsolete | Ergebnis gehört zu einer veralteten Konfiguration |
| Evicted | Cache-Ressource wurde freigegeben |

Cache-Zustände MÜSSEN versioniert und konsistent verwaltet werden.

Ungültige Regionen DÜRFEN nicht als gültige Ergebnisse verwendet werden.

## 12. In-Flight Rendering

NovaOS MUSS Änderungen während laufender GPU-Berechnungen korrekt behandeln.

Dabei gilt:

- Laufende Berechnungen verwenden unveränderliche Eingabeversionen.
- Neue Änderungen erzeugen neue Invalidierungszustände.
- Veraltete GPU-Ergebnisse DÜRFEN aktuelle Cache-Versionen nicht überschreiben.
- Noch ausstehende Änderungen MÜSSEN erhalten bleiben.
- Ressourcen DÜRFEN erst nach Abschluss aller Zugriffe freigegeben werden.

Die Veröffentlichung aktualisierter Cache-Ergebnisse MUSS atomar hinsichtlich ihrer logischen Zustandsversion erfolgen.

## 13. Integration

Nova Liquid Glass Cache Invalidation MUSS mit folgenden Subsystemen zusammenarbeiten:

| Subsystem | Verantwortung |
|---|---|
| Liquid Glass Dependency Tracking | Ermittlung betroffener Abhängigkeiten |
| Nova Damage Tracking | Räumliche Änderungsverfolgung |
| Nova Material Effect Caching | Cache-Gültigkeit und Cache-Zustände |
| Liquid Glass Blur Caching | Blur-spezifische Invalidierung |
| Liquid Glass Refraction Caching | Refraction-spezifische Invalidierung |
| Liquid Glass Reflection Caching | Reflection-spezifische Invalidierung |
| Nova Render Graph | Planung erforderlicher Neuberechnungen |
| Nova Frame Scheduling | Bedarfsgesteuerte Frame-Erzeugung |

Cache-Invalidierung MUSS von der eigentlichen GPU-Ausführung getrennt bleiben.

## 14. Performance und Ressourcen

NovaOS SOLL Invalidierungen anhand ihrer erwarteten Kosten optimieren.

Dabei gilt:

- Kleine Änderungen SOLLEN bevorzugt partiell verarbeitet werden.
- Stark fragmentierte Regionen DÜRFEN zusammengefasst werden.
- Bei hohen Teilaktualisierungskosten DARF vollständig neu berechnet werden.
- Cache-Invalidierung DARF nicht automatisch eine sofortige GPU-Ausführung erzwingen.
- Unveränderte Materialien DÜRFEN keine neuen Frames auslösen.

Die Invalidierungsverwaltung MUSS auch bei vielen gleichzeitig vorhandenen Glasflächen effizient arbeiten.

## 15. Sicherheit und Fehlerbehandlung

NovaOS MUSS folgende Fehler kontrolliert behandeln:

- Ungültige Cache-Referenzen.
- Fehlende Abhängigkeitsinformationen.
- Inkonsistente Eingabeversionen.
- Nicht auflösbare räumliche Abhängigkeiten.
- Fehlgeschlagene Cache-Aktualisierungen.
- Ressourcenverlust.
- GPU-Geräteverlust.

Bei unklarer Cache-Gültigkeit MUSS konservativ invalidiert werden.

Geschützte Hintergrundinhalte DÜRFEN nicht durch veraltete oder gemeinsam verwendete Cache-Ergebnisse offengelegt werden.

## 16. Verbindliche Anforderungen

1. NovaOS MUSS eine zentrale Liquid-Glass-Cache-Invalidierung unterstützen.
2. Nova Material Effect Caching MUSS als Cache-Infrastruktur verwendet werden.
3. Nova Liquid Glass Dependency Tracking MUSS integriert werden.
4. Änderungen relevanter Eingaben MÜSSEN erkannt werden.
5. Effektstufen MÜSSEN unabhängig invalidierbar sein.
6. Räumlich begrenzte Invalidierung MUSS unterstützt werden.
7. Blur-, Refraction- und Reflection-Abhängigkeiten MÜSSEN berücksichtigt werden.
8. Invalidierungen MÜSSEN transitiv weitergegeben werden.
9. Partielle und vollständige Invalidierung MÜSSEN möglich sein.
10. Unveränderte Cache-Ergebnisse SOLLEN gültig bleiben.
11. Mehrere Invalidierungen SOLLEN zusammengefasst werden.
12. Cache-Zustände MÜSSEN versioniert sein.
13. Laufende GPU-Berechnungen MÜSSEN berücksichtigt werden.
14. Veraltete Ergebnisse DÜRFEN aktuelle Cache-Versionen nicht überschreiben.
15. Nova Damage Tracking MUSS integriert werden.
16. Nova Render Graph MUSS erforderliche Neuberechnungen erhalten.
17. Nova Frame Scheduling MUSS bedarfsgesteuert arbeiten.
18. Ungültige Cache-Ergebnisse DÜRFEN nicht verwendet werden.
19. Geschützte Oberflächeninhalte MÜSSEN isoliert bleiben.
20. Bei unvollständigen Abhängigkeiten MUSS konservativ invalidiert werden.
21. Im deterministischen Modus MÜSSEN Invalidierungsentscheidungen bei identischen Eingaben reproduzierbar sein.

## 17. Ergebnis

Nova Liquid Glass Cache Invalidation ermöglicht die präzise und kontrollierte Ungültigsetzung zwischengespeicherter Liquid-Glass-Effekte.

Durch effektabhängige Invalidierung, räumliche Änderungsanalyse, Versionierung und inkrementelle Aktualisierung bleiben unveränderte Ergebnisse erhalten.

Damit verhindert NovaOS unnötige Neuberechnungen komplexer Glasmaterialien und gewährleistet gleichzeitig eine konsistente, sichere und leistungsfähige Liquid-Glass-Darstellung.