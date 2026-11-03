# NPSPEC-COMPOSITOR-INCREMENTAL-0001 – Nova Incremental Composition

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Compositor

## 1. Zweck

Nova Incremental Composition definiert die schrittweise Aktualisierung des zusammengesetzten Ausgabebildes.

Ziel ist, ausschließlich tatsächlich veränderte oder von Änderungen abhängige Bildbereiche neu zusammenzusetzen.

Unveränderte Kompositionsergebnisse SOLLEN erhalten bleiben, um CPU-, GPU- und Speicherbandbreitenbedarf zu reduzieren.

## 2. Architekturprinzipien

- **Retained Composition:** Der Kompositionszustand bleibt zwischen Frames erhalten.
- **Damage-Driven:** Änderungen bestimmen den Aktualisierungsbedarf.
- **Region-Based:** Die Verarbeitung erfolgt auf betroffenen Bildbereichen.
- **Dependency-Aware:** Indirekte Auswirkungen werden berücksichtigt.
- **Cache-Aware:** Gültige Kompositionsergebnisse werden wiederverwendet.
- **Visibility-Aware:** Verdeckung und Sichtbarkeit beeinflussen die Verarbeitung.
- **Event-Driven:** Ohne relevante Änderungen erfolgt keine unnötige Komposition.
- **Deterministic:** Identische Eingaben erzeugen reproduzierbare Ergebnisse.

## 3. Incremental Composition Model

Eine inkrementelle Komposition verwendet:

- **Composition State:** Aktueller Zustand des Composition Tree.
- **Previous State:** Zuletzt gültiger Kompositionszustand.
- **Surface Versions:** Versionen der verwendeten Oberflächen.
- **Damage Regions:** Betroffene Ausgabebereiche.
- **Dependency Regions:** Zusätzlich erforderliche Effektbereiche.
- **Cached Results:** Wiederverwendbare Kompositionsergebnisse.
- **Output Version:** Version des erzeugten Ausgabebildes.

Alle verwendeten Zustände MÜSSEN konsistent und versionssicher sein.

## 4. Change Detection

NovaOS MUSS folgende Änderungen erkennen und berücksichtigen:

- Surface-Inhaltsänderungen
- Position und Größe
- Transformationen
- Z-Order
- Opacity und Clipping
- Sichtbarkeit
- Material- und Backdrop-Abhängigkeiten
- Änderungen der Ausgabeparameter

Geometrieänderungen MÜSSEN sowohl vorherige als auch neue Ausgabebereiche berücksichtigen.

Änderungen der Z-Order MÜSSEN betroffene Überlappungsbereiche invalidieren.

## 5. Incremental Composition Pipeline

Die Verarbeitung erfolgt grundsätzlich in folgenden Schritten:

1. Konsistenten Composition State übernehmen.
2. Änderungen gegenüber dem vorherigen Zustand bestimmen.
3. Damage Regions und Abhängigkeiten ermitteln.
4. Sichtbarkeit und Occlusion berücksichtigen.
5. Damage Regions optimieren und zusammenfassen.
6. Gültige Kompositionsergebnisse identifizieren.
7. Erforderliche Composition Operations planen.
8. Betroffene Ausgabebereiche aktualisieren.
9. Ergebnis synchronisiert zur Präsentation bereitstellen.

Unveränderte Regionen SOLLEN nicht erneut zusammengesetzt werden.

## 6. Region-Based Composition

NovaOS MUSS die Komposition auf definierte Damage Regions begrenzen können.

Dabei gilt:

- Nur relevante Layer werden verarbeitet.
- Clipping und Scissor Rendering SOLLEN verwendet werden.
- Unveränderte Pixel bleiben erhalten.
- Materialabhängige Eingabebereiche werden berücksichtigt.
- Überlappende Damage Regions DÜRFEN zusammengefasst werden.

Eine regionale Aktualisierung MUSS alle zur korrekten Darstellung erforderlichen Layer berücksichtigen.

## 7. Composition Result Reuse

NovaOS SOLL gültige Kompositionsergebnisse wiederverwenden.

Dazu gehören:

- Unveränderte Ausgaberegionen
- Zwischengespeicherte Layer-Gruppen
- Bereits berechnete Blending-Ergebnisse
- Material- und Effekt-Zwischenergebnisse

Die Wiederverwendung MUSS anhand relevanter Zustands- und Ressourcenabhängigkeiten validiert werden.

Ein unveränderter Surface-Inhalt garantiert nicht, dass sein Kompositionsergebnis weiterhin gültig ist.

## 8. Material-Aware Composition

Nova Incremental Composition MUSS Materialabhängigkeiten berücksichtigen.

Insbesondere bei:

- Acrylic
- Liquid Glass
- Frosted Glass
- Crystal
- Blur
- Refraction
- Reflection

Änderungen hinter transparenten Materialien können eine erneute Komposition auslösen.

NovaOS SOLL ausschließlich betroffene Material- und Effektbereiche aktualisieren.

Gültige unabhängige Effektergebnisse SOLLEN erhalten bleiben.

## 9. Occlusion Integration

NovaOS SOLL vollständig verdeckte Bereiche von unnötiger Komposition ausschließen.

Dabei gilt:

- Sichtbare Damage Regions werden verarbeitet.
- Verdeckter Damage kann zurückgestellt werden.
- Neu sichtbar werdende Bereiche müssen gültige Inhalte erhalten.
- Effektabhängige Eingaben dürfen nicht fälschlich ausgeschlossen werden.

Occlusion Culling MUSS mit Nova Damage Tracking abgestimmt sein.

## 10. Adaptive Composition

NovaOS SOLL zwischen inkrementeller und vollständiger Komposition kostenbasiert entscheiden.

Berücksichtigt werden:

- Anzahl und Größe der Damage Regions
- Anteil betroffener Ausgabepixel
- Anzahl relevanter Layer
- Material- und Effektkosten
- Cache-Gültigkeit
- Speicherbandbreite
- GPU- und CPU-Verwaltungsaufwand

Eine vollständige Komposition DARF erfolgen, wenn sie effizienter oder zur Sicherstellung der Korrektheit erforderlich ist.

## 11. Synchronization und Buffer Validity

Inkrementelle Komposition setzt gültige Zielbuffer voraus.

NovaOS MUSS sicherstellen, dass:

- Unveränderte Pixel im Zielbuffer gültig sind.
- Surface-Buffer vollständig bereitgestellt wurden.
- GPU-Zugriffe korrekt synchronisiert sind.
- Laufende Kompositionen konsistente Zustandsversionen verwenden.
- Buffer nicht während unzulässiger gleichzeitiger Verwendung überschrieben werden.

Bei ungültigem Zielbuffer MUSS eine vollständige Rekonstruktion möglich sein.

## 12. Frame Scheduling

Nova Incremental Composition arbeitet ereignisgesteuert mit Nova Frame Scheduling zusammen.

Dabei gilt:

- Mehrere Änderungen DÜRFEN zu einer Aktualisierung zusammengefasst werden.
- Ohne relevanten Damage SOLL keine neue Komposition erfolgen.
- Animationen erzeugen Aktualisierungen entsprechend tatsächlicher Zustandsänderungen.
- Präsentationszeitpunkt und Kompositionsausführung bleiben getrennte Verantwortlichkeiten.

Ein permanentes Neuzeichnen unveränderter Desktop-Inhalte ist nicht vorgesehen.

## 13. Fehlerbehandlung

NovaOS MUSS ungültige oder unvollständige Kompositionszustände erkennen können.

Dazu gehören:

- Fehlende Damage Regions
- Ungültige Cache-Ergebnisse
- Veraltete Surface-Versionen
- Ungültige Zielbuffer
- Nicht auflösbare Materialabhängigkeiten
- Fehlgeschlagene Synchronisation

Kann eine korrekte inkrementelle Aktualisierung nicht gewährleistet werden, MUSS NovaOS auf vollständige Komposition zurückfallen.

Ausstehender Damage DARF bei fehlgeschlagener Komposition nicht verloren gehen.

## 14. Verbindliche Anforderungen

1. NovaOS MUSS inkrementelle Komposition unterstützen.
2. Der Composition State MUSS zwischen Frames erhalten bleiben.
3. Änderungen MÜSSEN gezielt Damage Regions erzeugen.
4. Unveränderte Ausgabebereiche SOLLEN wiederverwendet werden.
5. Geometrie- und Z-Order-Änderungen MÜSSEN korrekt berücksichtigt werden.
6. Material- und Backdrop-Abhängigkeiten MÜSSEN erhalten bleiben.
7. Occlusion Culling SOLL unnötige Verarbeitung vermeiden.
8. Gültige Composition-Caches SOLLEN wiederverwendet werden.
9. Zielbuffer MÜSSEN vor inkrementeller Aktualisierung auf Inhaltsgültigkeit geprüft werden.
10. Die Komposition MUSS versionssicher und synchronisiert erfolgen.
11. Ohne relevanten Damage SOLL keine unnötige Komposition ausgelöst werden.
12. Zwischen Partial und Full Composition SOLL kostenbasiert entschieden werden.
13. Ein Full-Composition-Fallback MUSS verfügbar sein.
14. Fehlgeschlagene Kompositionen DÜRFEN ausstehenden Damage nicht löschen.
15. Identische Eingaben MÜSSEN im deterministischen Modus reproduzierbare Ergebnisse erzeugen.

## 15. Ergebnis

Nova Incremental Composition ermöglicht die gezielte Aktualisierung veränderter Bildbereiche innerhalb des Nova Compositor.

Durch retained Composition, Damage Tracking, Occlusion Culling und die Wiederverwendung gültiger Ergebnisse werden unnötige Kompositionsoperationen vermieden.

Damit kann NovaOS seine Desktop-Oberfläche effizient und bedarfsgesteuert darstellen, ohne unveränderte Fenster, Materialien oder Bildbereiche permanent neu zusammensetzen zu müssen.