# NPSPEC-SCENE-CACHE-0001 – Nova Scene Cache Management

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Scene Graph

## 1. Zweck

Nova Scene Cache Management definiert die Zwischenspeicherung, Wiederverwendung und Invalidierung berechneter Szenenergebnisse innerhalb des Nova Scene Graph.

Ziel ist die Vermeidung redundanter Berechnungen bei unveränderten Szenenzuständen sowie die effiziente Nutzung von Arbeitsspeicher und CPU-Ressourcen.

Der Scene Cache speichert abgeleitete Szeneninformationen, jedoch keine fertigen Pixel- oder GPU-Rendering-Ergebnisse.

## 2. Architekturprinzipien

- **Retained:** Gültige Berechnungsergebnisse bleiben zwischen Aktualisierungen erhalten.
- **Incremental:** Nur betroffene Cache-Einträge werden invalidiert.
- **Dependency-Aware:** Die Cache-Gültigkeit berücksichtigt relevante Abhängigkeiten.
- **Versioned:** Cache-Ergebnisse sind definierten Zustandsversionen zugeordnet.
- **Lazy:** Ungültige Ergebnisse werden möglichst erst bei Bedarf neu berechnet.
- **Reusable:** Unveränderte Ergebnisse können zwischen Szenenversionen wiederverwendet werden.
- **Resource-Aware:** Die Speichernutzung berücksichtigt verfügbare Ressourcen.
- **Deterministic:** Cache-Nutzung verändert keine logischen Szenenergebnisse.

## 3. Cache-Modell

Ein Scene Cache Entry enthält mindestens:

- **Cache ID:** Eindeutige Identität des Eintrags.
- **Owner:** Zugehöriger Scene Node oder Szenenkontext.
- **Result Type:** Art des gespeicherten Ergebnisses.
- **State Version:** Zugehörige Zustandsversion.
- **Dependency Signature:** Gültigkeitsnachweis relevanter Abhängigkeiten.
- **Cached Result:** Gespeichertes Berechnungsergebnis.
- **Validity State:** Aktueller Gültigkeitszustand.

Optional können Speicherverbrauch, Zugriffshäufigkeit und letzte Verwendung erfasst werden.

## 4. Cache-Kategorien

NovaOS unterstützt insbesondere:

- **Property Cache:** Effektive Eigenschaftswerte.
- **Transform Cache:** Berechnete Transformationsmatrizen.
- **Geometry Cache:** Abgeleitete Geometrieinformationen.
- **Bounds Cache:** Lokale und globale Begrenzungen.
- **Layout Cache:** Berechnete Layout-Ergebnisse.
- **Visibility Cache:** Effektive Sichtbarkeitsinformationen.
- **Hierarchy Cache:** Abgeleitete Hierarchieinformationen.
- **Material State Cache:** Aufgelöste Materialparameter.

Weitere Cache-Kategorien DÜRFEN ergänzt werden.

GPU-Ressourcen, Render Targets und fertige Rendering-Ergebnisse werden durch die zuständigen Grafikkomponenten verwaltet.

## 5. Cache-Gültigkeit

Ein Cache-Eintrag ist gültig, wenn sämtliche für sein Ergebnis relevanten Eingaben unverändert geblieben sind.

Die Gültigkeitsprüfung MUSS berücksichtigen:

- Identität des zugehörigen Elements
- Relevante Property-Versionen
- Abhängige Scene Nodes
- Hierarchieänderungen
- Ressourcenänderungen
- Verwendete Berechnungsparameter

Eine neue Szenenversion DARF gültige Cache-Ergebnisse älterer Versionen wiederverwenden.

Die bloße Änderung der globalen Szenenversion MUSS keine vollständige Cache-Invalidierung auslösen.

## 6. Cache-Invalidierung

Invalidierungen werden durch Scene Change Tracking und den Scene Dependency Graph ausgelöst.

NovaOS MUSS:

1. Relevante Zustandsänderungen erkennen.
2. Betroffene Cache-Abhängigkeiten bestimmen.
3. Ausschließlich potenziell ungültige Ergebnisse markieren.
4. Unveränderte Cache-Einträge erhalten.
5. Erforderliche Ergebnisse bei Bedarf neu berechnen.

Mehrfache Invalidierungen desselben Eintrags SOLLEN zusammengefasst werden.

Eine Invalidierung MUSS nicht unmittelbar eine Neuberechnung auslösen.

## 7. Cache-Wiederverwendung

NovaOS SOLL unveränderte Ergebnisse über mehrere Szenenversionen hinweg wiederverwenden.

Dazu können verwendet werden:

- Versionsvergleiche
- Dependency Signatures
- Structural Sharing
- Immutable Cache Results
- Wiederverwendung identischer Berechnungsergebnisse

Die Wiederverwendung MUSS unabhängig von der Identität einer globalen Szenenversion möglich sein, sofern alle relevanten Abhängigkeiten gültig sind.

## 8. Cache-Lebenszyklus

Ein Cache-Eintrag besitzt folgende logische Zustände:

- **Missing:** Kein Ergebnis vorhanden.
- **Valid:** Ergebnis ist verwendbar.
- **Invalid:** Ergebnis muss vor erneuter Verwendung aktualisiert werden.
- **Computing:** Ergebnis wird berechnet.
- **Evictable:** Ergebnis kann bei Bedarf freigegeben werden.

Einträge DÜRFEN bei Speicherbedarf entfernt werden.

Aktive Berechnungen und verwendete Snapshots MÜSSEN vorzeitige Freigaben verhindern.

## 9. Speicherverwaltung

Der Scene Cache MUSS kontrolliert mit verfügbarem Arbeitsspeicher umgehen.

NovaOS SOLL:

- Cache-Einträge nach Nutzen und Kosten bewerten.
- Selten verwendete Ergebnisse bevorzugt freigeben.
- Große Cache-Einträge bei Speicherengpässen berücksichtigen.
- Redundante Datenhaltung vermeiden.
- Speicherfreigaben ohne Beeinträchtigung aktiver Zugriffe ermöglichen.

Cache-Einträge sind grundsätzlich rekonstruierbar.

Die Freigabe eines Cache-Eintrags DARF keinen Verlust des eigentlichen Szenenzustands verursachen.

## 10. Parallelität

Unabhängige Cache-Einträge DÜRFEN parallel gelesen oder berechnet werden.

Dabei MUSS NovaOS:

- Konsistente Ergebnisse gewährleisten.
- Gleichzeitige widersprüchliche Veröffentlichungen verhindern.
- Aktive Cache-Referenzen schützen.
- Veraltete Berechnungsergebnisse erkennen.
- Doppelte Berechnungen nach Möglichkeit vermeiden.

Ein für eine ältere Szenenversion berechnetes Ergebnis DARF nur übernommen werden, wenn seine Abhängigkeiten weiterhin gültig sind.

## 11. Integration

Nova Scene Cache Management arbeitet mit folgenden Komponenten zusammen:

- **Scene Property System:** Bereitstellung und Auflösung von Eigenschaften.
- **Scene Change Tracking:** Erkennung relevanter Änderungen.
- **Scene Dependency Graph:** Bestimmung betroffener Cache-Abhängigkeiten.
- **Incremental Scene Updates:** Bedarfsgerechte Neuberechnung.
- **Persistent Scene State:** Verwaltung konsistenter Szenenversionen.
- **Graphics Resource Management:** Berücksichtigung übergeordneter Ressourcenbudgets.

Die Verwaltung fertiger Rendering-Ergebnisse verbleibt beim Render Cache beziehungsweise den zuständigen Rendering-Komponenten.

## 12. Fehlerbehandlung

Bei ungültigen, fehlenden oder beschädigten Cache-Einträgen MUSS NovaOS das Ergebnis aus dem gültigen Szenenzustand neu berechnen können.

Bei Speicherengpässen DARF der Cache reduziert oder vollständig verworfen werden.

Cache-Fehler DÜRFEN:

- Keine inkonsistenten Szenenzustände erzeugen.
- Keine veralteten Ergebnisse als gültig bereitstellen.
- Keine erforderlichen Szenenaktualisierungen dauerhaft verhindern.

## 13. Verbindliche Anforderungen

1. Abgeleitete Szenenergebnisse MÜSSEN zwischengespeichert werden können.
2. Cache-Einträge MÜSSEN eindeutig identifizierbar sein.
3. Die Cache-Gültigkeit MUSS anhand relevanter Zustände und Abhängigkeiten überprüfbar sein.
4. Zustandsänderungen MÜSSEN gezielte Invalidierungen ermöglichen.
5. Unveränderte Ergebnisse SOLLEN zwischen Szenenversionen wiederverwendet werden.
6. Invalidierte Ergebnisse DÜRFEN nicht als gültig verwendet werden.
7. Cache-Einträge MÜSSEN bei Bedarf rekonstruierbar sein.
8. Cache-Speicher MUSS kontrolliert freigegeben werden können.
9. Aktive Snapshots und Berechnungen DÜRFEN durch Cache-Freigaben nicht beschädigt werden.
10. Parallele Cache-Zugriffe MÜSSEN konsistent erfolgen.
11. Cache-Nutzung DARF das logische Szenenergebnis nicht verändern.
12. Das Cache-System MUSS unabhängig von konkreten Rendering-Backends funktionieren.

## 14. Ergebnis

Nova Scene Cache Management stellt eine versions- und abhängigkeitsbasierte Zwischenspeicherung abgeleiteter Szenenergebnisse bereit.

Durch gezielte Invalidierung, Lazy Evaluation und die Wiederverwendung unveränderter Ergebnisse reduziert NovaOS redundante Berechnungen und unnötige Speicherzugriffe.

Damit unterstützt das System eine effiziente Szenenverwaltung und bildet eine wichtige Grundlage für inkrementelle Grafikverarbeitung und ressourcenschonendes Rendering.