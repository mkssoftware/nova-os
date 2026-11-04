# NPSPEC-MATERIAL-CACHE-0001 – Nova Material Effect Caching

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Material Engine / Nova Render Engine

## 1. Zweck

Nova Material Effect Caching definiert die Zwischenspeicherung und Wiederverwendung bereits berechneter Materialeffekte.

Ziel ist die Vermeidung redundanter GPU-Berechnungen bei unveränderten Materialeigenschaften, Hintergrundinhalten und Effektabhängigkeiten.

Insbesondere aufwendige Effekte wie Blur, Refraction, Reflection und Liquid Glass SOLLEN von der Wiederverwendung gültiger Zwischenergebnisse profitieren.

## 2. Architekturprinzipien

- **Result-Oriented:** Berechnete Effektergebnisse werden wiederverwendet.
- **Dependency-Aware:** Cache-Gültigkeit basiert auf tatsächlichen Eingabeabhängigkeiten.
- **Region-Based:** Teilbereiche können unabhängig gültig oder ungültig sein.
- **Incremental:** Nur betroffene Cache-Bereiche werden aktualisiert.
- **Shared:** Kompatible Effektergebnisse können gemeinsam verwendet werden.
- **Budget-Aware:** Cache-Speicherverbrauch wird kontrolliert.
- **Versioned:** Cache-Einträge besitzen nachvollziehbare Zustandsversionen.
- **Deterministic:** Identische Eingaben führen zu reproduzierbaren Cache-Entscheidungen.

## 3. Cache Architecture

Nova Material Effect Caching besteht aus folgenden Komponenten:

| Komponente | Verantwortung |
|---|---|
| Material Cache Manager | Zentrale Verwaltung der Effektergebnisse |
| Cache Registry | Registrierung vorhandener Cache-Einträge |
| Cache Key Generator | Eindeutige Identifikation von Ergebnissen |
| Cache Validator | Prüfung der Ergebnisgültigkeit |
| Region Cache Manager | Verwaltung gültiger und ungültiger Teilbereiche |
| Cache Dependency Tracker | Zuordnung relevanter Eingabeabhängigkeiten |
| Cache Eviction Controller | Freigabe nicht benötigter Cache-Einträge |

Die Cache-Verwaltung MUSS von der eigentlichen Effektberechnung getrennt bleiben.

## 4. Cache Entry Model

Jeder Cache-Eintrag besitzt mindestens:

| Eigenschaft | Beschreibung |
|---|---|
| Cache ID | Eindeutige Identifikation |
| Material ID | Zugehöriges Material |
| Effect ID | Zugehöriger Materialeffekt |
| Effect Version | Version der Effektimplementierung |
| Input Signature | Identifikation der relevanten Eingaben |
| Output Resource | Gespeichertes Effektergebnis |
| Valid Regions | Gültige Ergebnisbereiche |
| Invalid Regions | Neu zu berechnende Bereiche |
| Resource State | GPU-Ressourcenstatus |
| Last Used | Letzte Verwendung |
| Cache State | Aktueller Cache-Zustand |

Cache-Einträge MÜSSEN eindeutig identifizierbar und validierbar sein.

## 5. Unterstützte Cache-Typen

NovaOS SOLL folgende Materialergebnisse zwischenspeichern:

| Cache-Typ | Gespeichertes Ergebnis |
|---|---|
| Backdrop Cache | Vorbereitete Hintergrundbereiche |
| Blur Cache | Gefilterte Hintergrundbereiche |
| Refraction Cache | Berechnete Lichtbrechung |
| Reflection Cache | Berechnete Reflexionen |
| Lighting Cache | Beleuchtungs- und Highlight-Ergebnisse |
| Material Intermediate Cache | Zwischenstufen einer Material-Pipeline |
| Material Result Cache | Fertig berechnete Materialdarstellung |

Nicht jeder Effekt MUSS zwischengespeichert werden.

Die Cache-Verwendung SOLL vom erwarteten Wiederverwendungsnutzen abhängen.

## 6. Cache Identification

Cache-Schlüssel MÜSSEN alle für die Ergebnisgültigkeit relevanten Eigenschaften berücksichtigen.

Dazu gehören:

- Material- und Effektversion.
- Effektparameter.
- Eingaberessourcen und deren Versionen.
- Verwendete Shader- und Pipeline-Konfigurationen.
- Ausgabeformat und Auflösung.
- Qualitätsprofil.
- Relevante Backend-Eigenschaften.

Unterschiedliche Eingaben DÜRFEN nicht fälschlich als identisches gültiges Ergebnis behandelt werden.

## 7. Dependency-Based Cache Validation

Nova Material Effect Caching MUSS mit Nova Material Dependency Tracking zusammenarbeiten.

Dabei gilt:

1. Eingabeänderungen werden erfasst.
2. Betroffene Effektergebnisse werden ermittelt.
3. Räumliche Auswirkungen werden berechnet.
4. Betroffene Cache-Bereiche werden invalidiert.
5. Unveränderte Bereiche bleiben gültig.
6. Ungültige Bereiche werden bei Bedarf neu berechnet.

Bei unvollständigen Abhängigkeitsinformationen MUSS konservativ invalidiert werden.

## 8. Region-Based Caching

NovaOS MUSS die teilweise Gültigkeit von Effektergebnissen unterstützen.

Dabei gilt:

- Cache-Einträge können mehrere gültige Regionen besitzen.
- Änderungen invalidieren nur betroffene Bereiche.
- Unveränderte Regionen SOLLEN wiederverwendet werden.
- Filterradien und räumliche Abhängigkeiten MÜSSEN berücksichtigt werden.
- Unterschiedliche Ausgabeauflösungen MÜSSEN korrekt behandelt werden.

Eine vollständige Invalidierung MUSS möglich bleiben.

## 9. Incremental Cache Updates

NovaOS SOLL Cache-Ergebnisse inkrementell aktualisieren.

Der logische Ablauf lautet:

`Input Change → Dependency Analysis → Region Invalidation → Partial Rendering → Cache Update`

Dabei gilt:

- Nur ungültige Ergebnisbereiche werden neu berechnet.
- Unveränderte Cache-Inhalte bleiben erhalten.
- Teilaktualisierungen MÜSSEN mit der Effektimplementierung kompatibel sein.
- Fehlgeschlagene Aktualisierungen DÜRFEN bestehende gültige Ergebnisse nicht beschädigen.

## 10. Shared Effect Caching

NovaOS SOLL kompatible Materialeffekte gemeinsam zwischenspeichern können.

Beispiele:

- Mehrere Glasflächen verwenden denselben vorbereiteten Backdrop.
- Mehrere Materialien nutzen identische Blur-Zwischenergebnisse.
- Gemeinsame Lighting-Daten werden wiederverwendet.

Dabei gilt:

- Gemeinsame Ergebnisse benötigen kompatible Eingaben und Effektparameter.
- Unterschiedliche Materialinstanzen DÜRFEN keine unzulässigen Seiteneffekte verursachen.
- Prozessübergreifende Wiederverwendung MUSS die NovaOS-Capability-Architektur respektieren.
- Geschützte Oberflächeninhalte DÜRFEN nicht über gemeinsame Cache-Einträge offengelegt werden.

## 11. Liquid Glass Caching

Nova Material Effect Caching MUSS Liquid-Glass-Effekte unterstützen.

Dabei SOLLEN insbesondere folgende Ergebnisse wiederverwendet werden:

- Backdrop Sampling.
- Blur.
- Refraction.
- Reflection.
- Lighting.
- Material-Zwischenergebnisse.

Änderungen einzelner Effektparameter SOLLEN nur abhängige Cache-Ergebnisse invalidieren.

Änderungen hinter einer Glasoberfläche MÜSSEN entsprechend ihrer räumlichen Auswirkungen berücksichtigt werden.

## 12. Cache Memory Management

Nova Material Effect Caching MUSS mit Nova GPU Resource Management zusammenarbeiten.

Dabei gilt:

- Cache-Speicherverbrauch MUSS begrenzbar sein.
- GPU Memory Budgets MÜSSEN berücksichtigt werden.
- Nicht benötigte Cache-Einträge SOLLEN freigegeben werden.
- Aktive GPU-Ressourcen DÜRFEN nicht vorzeitig freigegeben werden.
- Cache-Eviction MUSS unter Speicherbelastung möglich sein.

Die Entfernung eines Cache-Eintrags DARF die funktionale Korrektheit nicht beeinträchtigen.

Fehlende Ergebnisse werden bei Bedarf neu berechnet.

## 13. Cache Eviction

NovaOS SOLL Cache-Einträge anhand folgender Kriterien freigeben:

- Letzte Verwendung.
- Speicherverbrauch.
- Wiederberechnungskosten.
- Erwartete Wiederverwendung.
- Aktuelle GPU Memory Pressure.
- Priorität des zugehörigen Materials.

Systemkritische Ressourcen SOLLEN bevorzugt geschützt werden.

Die Cache-Freigabe MUSS aktive GPU-Zugriffe und Ressourcenreferenzen berücksichtigen.

## 14. Render Graph Integration

Nova Material Effect Caching MUSS mit Nova Render Graph zusammenarbeiten.

Dabei gilt:

- Gültige Cache-Ergebnisse können Render-Passes ersetzen.
- Ungültige Regionen erzeugen notwendige Renderoperationen.
- Ressourcenabhängigkeiten bleiben verbindlich.
- GPU Resource Barriers MÜSSEN korrekt berücksichtigt werden.
- Cache-Ressourcen MÜSSEN bis zum Abschluss ihrer Verwendung gültig bleiben.

Ein übersprungener Render Pass MUSS durch ein nachweislich gültiges Ergebnis ersetzt werden.

## 15. Cache Lifecycle

NovaOS definiert folgende Zustände:

`Created → Valid → Partially Invalid → Invalid → Updating → Valid`

Zusätzliche Zustände:

- **Evicted:** Cache-Ressource wurde freigegeben.
- **Obsolete:** Ergebnis gehört zu einer veralteten Konfiguration.
- **Failed:** Cache-Aktualisierung fehlgeschlagen.
- **Retired:** Eintrag wartet auf sichere Freigabe.

Veraltete oder ungültige Ergebnisse DÜRFEN nicht als aktuelle Materialergebnisse veröffentlicht werden.

## 16. Sicherheit und Fehlerbehandlung

NovaOS MUSS folgende Fehler kontrolliert behandeln:

- Ungültige Cache-Schlüssel.
- Veraltete Ressourcenreferenzen.
- Fehlende Eingaberessourcen.
- Inkonsistente Abhängigkeitsversionen.
- Fehlgeschlagene GPU-Berechnungen.
- Speicherknappheit.
- GPU-Geräteverlust.

Bei Cache-Fehlern MUSS eine direkte Neuberechnung oder ein geeigneter Material-Fallback möglich sein.

Cache-Optimierungen DÜRFEN keine Sicherheits- oder Synchronisationsanforderungen umgehen.

## 17. Verbindliche Anforderungen

1. NovaOS MUSS Material Effect Caching unterstützen.
2. Cache-Einträge MÜSSEN eindeutig identifizierbar sein.
3. Cache-Gültigkeit MUSS anhand relevanter Eingaben geprüft werden.
4. Nova Material Dependency Tracking MUSS integriert werden.
5. Räumlich begrenzte Cache-Invalidierung MUSS unterstützt werden.
6. Inkrementelle Cache-Aktualisierung SOLL möglich sein.
7. Unveränderte Effektergebnisse SOLLEN wiederverwendet werden.
8. Gemeinsame Effektergebnisse SOLLEN unterstützt werden.
9. Liquid-Glass-Effekte MÜSSEN integrierbar sein.
10. Nova Render Graph MUSS Cache-Ergebnisse verwenden können.
11. GPU Resource Management MUSS integriert werden.
12. Cache-Speicherverbrauch MUSS begrenzbar sein.
13. Cache Eviction MUSS unterstützt werden.
14. Aktive GPU-Ressourcen DÜRFEN nicht vorzeitig freigegeben werden.
15. Ungültige Cache-Ergebnisse DÜRFEN nicht verwendet werden.
16. Geschützte Ressourcen MÜSSEN isoliert bleiben.
17. Cache-Fehler MÜSSEN eine sichere Neuberechnung oder einen Fallback ermöglichen.
18. Im deterministischen Modus MÜSSEN Cache-Entscheidungen bei identischen Eingaben und Zuständen reproduzierbar sein.

## 18. Ergebnis

Nova Material Effect Caching ermöglicht die effiziente Wiederverwendung bereits berechneter Materialeffekte.

Durch Dependency Tracking, räumliche Invalidierung, inkrementelle Aktualisierung und kontrollierte Speicherverwaltung werden redundante GPU-Berechnungen vermieden.

Damit reduziert NovaOS den Rechenaufwand komplexer Materialdarstellungen und ermöglicht eine ressourceneffiziente Umsetzung von Liquid Glass, Blur, Refraction und Reflection.