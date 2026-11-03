# NPSPEC-DAMAGE-HIERARCHY-0001 – Nova Hierarchical Damage Tracking

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Damage Tracking

## 1. Zweck

Nova Hierarchical Damage Tracking definiert die hierarchische Erfassung und Verarbeitung grafischer Änderungen innerhalb des Nova Scene Graph.

Ziel ist, Änderungen möglichst lokal zu behandeln und ausschließlich betroffene Teilbäume, Surfaces und Rendering-Bereiche zu invalidieren.

Unveränderte Teilbäume sollen ihre gültigen Zustände und Rendering-Ergebnisse behalten.

## 2. Architekturprinzipien

- **Hierarchy-Aware:** Damage wird entsprechend der Scene-Graph-Hierarchie verwaltet.
- **Local-First:** Änderungen werden zunächst am verursachenden Node erfasst.
- **Incremental:** Nur betroffene Hierarchiebereiche werden aktualisiert.
- **Dependency-Aware:** Abhängigkeiten außerhalb der direkten Hierarchie werden berücksichtigt.
- **Transform-Aware:** Transformationen und Koordinatensysteme werden korrekt verarbeitet.
- **Cache-Aware:** Gültige Zwischenergebnisse bleiben erhalten.
- **Versioned:** Damage ist eindeutig einem Scene-Zustand zugeordnet.
- **Deterministic:** Identische Zustände erzeugen reproduzierbare Damage-Ergebnisse.

## 3. Hierarchical Damage Model

Jeder relevante Scene Node MUSS folgende Informationen bereitstellen können:

- **Node ID:** Eindeutige Identifikation.
- **Parent Reference:** Übergeordneter Node.
- **Local Damage:** Änderungen innerhalb des lokalen Koordinatensystems.
- **Subtree Damage:** Zusammengefasste Änderungen des Teilbaums.
- **Bounds:** Lokale und transformierte Begrenzungen.
- **State Version:** Zugehörige Zustandsversion.
- **Damage State:** Clean, Partial oder Full.

Die Damage-Verwaltung DARF zusätzliche Metadaten zur Optimierung verwenden.

## 4. Damage States

NovaOS unterscheidet drei grundlegende Zustände:

| Zustand | Bedeutung |
|---|---|
| Clean | Keine ausstehende relevante Änderung |
| Partial | Bestimmte Bereiche benötigen eine Aktualisierung |
| Full | Der gesamte relevante Node- oder Teilbaumbereich benötigt eine Aktualisierung |

Ein übergeordneter Node DARF Partial Damage besitzen, obwohl einzelne untergeordnete Nodes vollständig invalidiert sind.

Full Damage eines Nodes bedeutet nicht automatisch, dass sämtliche unabhängigen Geschwister-Nodes neu gerendert werden müssen.

## 5. Local Damage Generation

Grafische Änderungen werden zunächst am betroffenen Node erfasst.

Dazu gehören:

- Änderungen von Eigenschaften
- Änderungen der Geometrie
- Änderungen von Transformationen
- Änderungen der Sichtbarkeit
- Änderungen von Clipping
- Hinzufügen oder Entfernen von Child Nodes
- Änderungen der Zeichenreihenfolge

Bei geometrischen Änderungen MÜSSEN die vorherigen und neuen sichtbaren Bereiche berücksichtigt werden.

## 6. Hierarchical Propagation

Lokaler Damage wird entlang der betroffenen Hierarchie weitergegeben.

Dabei gilt:

1. Der verursachende Node erzeugt Local Damage.
2. Die Region wird in das Koordinatensystem des Parent Nodes transformiert.
3. Der Parent aktualisiert seinen Subtree Damage.
4. Die Weitergabe erfolgt bis zur erforderlichen Rendering-Grenze.
5. Betroffene Surfaces und Ausgaben werden invalidiert.

Die Weitergabe SOLL beendet werden, sobald keine zusätzliche relevante Änderung entsteht.

Unabhängige Teilbäume DÜRFEN unverändert bleiben.

## 7. Transform und Clip Handling

NovaOS MUSS hierarchische Transformationen berücksichtigen.

Dazu gehören:

- Translation
- Skalierung
- Rotation
- Verschachtelte Transformationen
- Clipping
- Opacity und Sichtbarkeit

Damage Regions MÜSSEN konservativ in das jeweilige Zielkoordinatensystem übertragen werden.

Bei Änderungen einer Transformation MÜSSEN sowohl die alte als auch die neue Darstellung berücksichtigt werden.

Clipping DARF notwendige Effekt- oder Backdrop-Abhängigkeiten nicht entfernen.

## 8. Hierarchy Changes

Änderungen der Scene-Graph-Struktur können mehrere Teilbäume betreffen.

NovaOS MUSS insbesondere behandeln:

- Hinzufügen von Nodes
- Entfernen von Nodes
- Verschieben zwischen Parent Nodes
- Änderungen der Child-Reihenfolge
- Austausch vollständiger Teilbäume

Bei einer Verschiebung zwischen Parent Nodes MÜSSEN die bisherigen und neuen Hierarchiepfade berücksichtigt werden.

Entfernte Nodes MÜSSEN ihren zuvor sichtbaren Bereich weiterhin invalidieren können.

## 9. Dependency-Aware Tracking

Nicht alle grafischen Abhängigkeiten entsprechen der Parent-Child-Hierarchie.

NovaOS MUSS deshalb externe Abhängigkeiten berücksichtigen können.

Dazu gehören:

- Materialeffekte
- Backdrop Sampling
- Gemeinsame Rendering-Ressourcen
- Surface-Abhängigkeiten
- Reflection und Refraction

Die hierarchische Damage-Verwaltung MUSS mit Nova Damage Dependency Analysis zusammenarbeiten.

Eine rein hierarchische Weitergabe DARF erforderliche externe Invalidierungen nicht ersetzen.

## 10. Cache Integration

NovaOS SOLL gültige Cache-Ergebnisse unveränderter Teilbäume erhalten.

Dabei gilt:

- Local Damage invalidiert nur tatsächlich betroffene Ergebnisse.
- Subtree Damage kann abhängige Rendering-Caches invalidieren.
- Unabhängige Cache-Einträge bleiben gültig.
- Änderungen an übergeordneten Transformationen erfordern nicht grundsätzlich eine Neuberechnung aller lokalen Inhalte.

Die Wiederverwendung MUSS sämtliche relevanten Zustands- und Ressourcenabhängigkeiten berücksichtigen.

## 11. Incremental Processing

NovaOS SOLL hierarchische Damage-Strukturen inkrementell aktualisieren.

Dabei werden:

- Unveränderte Nodes übersprungen.
- Bereits berechnete Transformationen wiederverwendet.
- Betroffene Teilbäume gezielt verarbeitet.
- Damage Regions bei Bedarf zusammengefasst.

Eine vollständige Hierarchieanalyse DARF erfolgen, wenn die inkrementelle Verarbeitung nicht zuverlässig oder wirtschaftlich ist.

## 12. Versioning und Synchronization

Damage MUSS eindeutig einem veröffentlichten Scene-Zustand zugeordnet werden.

Dabei gilt:

- Rendering verwendet konsistente Scene Snapshots.
- Laufende Aktualisierungen dürfen nicht durch neue Änderungen überschrieben werden.
- Damage darf erst nach erfolgreicher Verarbeitung bestätigt werden.
- Neuere Änderungen müssen unabhängig von älteren Rendering-Aufträgen erhalten bleiben.

Die Verwaltung MUSS konkurrierende Scene-Updates und Rendering-Ausführungen sicher unterstützen.

## 13. Fallback

Wenn die betroffene Hierarchie nicht zuverlässig bestimmt werden kann, MUSS NovaOS eine konservative Invalidierung durchführen.

Mögliche Fallback-Stufen:

1. Vollständiger Node
2. Vollständiger Teilbaum
3. Vollständige Surface
4. Vollständige Ausgabe

Die kleinstmögliche sichere Invalidierungsstufe SOLL bevorzugt werden.

## 14. Verbindliche Anforderungen

1. NovaOS MUSS Damage hierarchisch innerhalb des Scene Graph verwalten können.
2. Local Damage und Subtree Damage MÜSSEN unterscheidbar sein.
3. Clean, Partial und Full Damage MÜSSEN unterstützt werden.
4. Änderungen SOLLEN zunächst lokal erfasst werden.
5. Damage MUSS entlang betroffener Hierarchiepfade weitergegeben werden.
6. Alte und neue Objektbereiche MÜSSEN bei geometrischen Änderungen berücksichtigt werden.
7. Hierarchische Transformationen und Clipping MÜSSEN korrekt verarbeitet werden.
8. Strukturänderungen MÜSSEN die betroffenen Hierarchiepfade invalidieren.
9. Externe grafische Abhängigkeiten MÜSSEN berücksichtigt werden.
10. Unabhängige Teilbäume SOLLEN unverändert bleiben.
11. Gültige Cache-Ergebnisse SOLLEN wiederverwendet werden.
12. Damage MUSS versionssicher verarbeitet werden.
13. Neuere Änderungen DÜRFEN durch ältere Rendering-Abschlüsse nicht verloren gehen.
14. Bei unvollständigen Informationen MUSS eine konservative Invalidierung erfolgen.
15. Identische Eingaben MÜSSEN im deterministischen Modus reproduzierbare Ergebnisse erzeugen.

## 15. Ergebnis

Nova Hierarchical Damage Tracking ermöglicht die gezielte Verarbeitung grafischer Änderungen innerhalb der Scene-Graph-Hierarchie.

Durch lokale Änderungserfassung, hierarchische Weitergabe und inkrementelle Aktualisierung werden unnötige Berechnungen unabhängiger Teilbäume vermieden.

Damit unterstützt NovaOS eine effiziente, skalierbare und zustandsabhängige Grafikverarbeitung, bei der ausschließlich tatsächlich betroffene Hierarchiebereiche aktualisiert werden.