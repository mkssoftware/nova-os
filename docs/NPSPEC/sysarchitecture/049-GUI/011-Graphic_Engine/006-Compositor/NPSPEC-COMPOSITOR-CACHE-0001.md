# NPSPEC-COMPOSITOR-CACHE-0001 – Nova Composition Cache

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Compositor

## 1. Zweck

Nova Composition Cache definiert die Zwischenspeicherung und Wiederverwendung bereits berechneter Kompositionsergebnisse.

Ziel ist die Vermeidung unnötiger Layer-Zusammensetzungen, Blending-Operationen und GPU-Berechnungen.

Unveränderte Kompositionsergebnisse SOLLEN wiederverwendet werden, solange ihre grafischen und technischen Abhängigkeiten gültig bleiben.

## 2. Architekturprinzipien

- **Retained:** Berechnete Kompositionsergebnisse bleiben über mehrere Frames erhalten.
- **Dependency-Aware:** Cache-Gültigkeit basiert auf tatsächlichen Abhängigkeiten.
- **Region-Based:** Ergebnisse können für einzelne Bildbereiche gespeichert werden.
- **Incremental:** Änderungen invalidieren ausschließlich betroffene Cache-Bereiche.
- **Material-Aware:** Transparenz und Materialeffekte werden berücksichtigt.
- **Versioned:** Cache-Einträge besitzen eindeutige Zustandsversionen.
- **Budget-Aware:** Speicherverbrauch wird kontrolliert.
- **Adaptive:** Caching erfolgt abhängig von Wiederverwendungswahrscheinlichkeit und Kosten.

## 3. Cache Model

Ein Composition Cache Entry enthält mindestens:

- **Cache ID:** Eindeutige Identifikation.
- **Target ID:** Zugehörige Surface, Layer-Gruppe oder Ausgabe.
- **Cached Resource:** Gespeichertes Kompositionsergebnis.
- **Valid Regions:** Gültige Bildbereiche.
- **Dependency Signature:** Relevante Eingabe- und Zustandsabhängigkeiten.
- **Content Version:** Version des gespeicherten Ergebnisses.
- **Resource State:** Speicher- und Synchronisationszustand.
- **Memory Cost:** Speicherbedarf.

Cache-Einträge MÜSSEN unabhängig vom logischen Scene Cache verwaltet werden.

## 4. Cache Granularity

NovaOS SOLL unterschiedliche Cache-Ebenen unterstützen.

| Cache-Ebene | Verwendung |
|---|---|
| Region Cache | Einzelne Ausgabebereiche |
| Layer Cache | Zusammengesetzte Layer-Inhalte |
| Group Cache | Ergebnisse mehrerer Layer |
| Effect Composition Cache | Zusammengesetzte Material- und Effektresultate |
| Output Cache | Wiederverwendbare Ausgabebereiche |

Die Cache-Ebene SOLL anhand der tatsächlichen Wiederverwendung und Berechnungskosten ausgewählt werden.

## 5. Cache Validity

Ein Cache-Eintrag DARF nur wiederverwendet werden, wenn sämtliche relevanten Abhängigkeiten gültig sind.

Dazu gehören insbesondere:

- Surface-Inhaltsversionen
- Layer-Reihenfolge
- Transformationen
- Opacity und Clipping
- Material- und Effektparameter
- Backdrop-Abhängigkeiten
- Ausgabeformat und Farbraum
- Ressourcenlebensdauer

Eine unveränderte Surface-Version allein garantiert keine gültige Komposition.

## 6. Incremental Cache Invalidation

NovaOS MUSS regionale Cache-Invalidierung unterstützen.

Dabei gilt:

1. Änderungen erzeugen Damage Regions.
2. Abhängige Cache-Einträge werden ermittelt.
3. Betroffene Bereiche werden invalidiert.
4. Unabhängige Cache-Bereiche bleiben gültig.
5. Fehlende Ergebnisse werden bei Bedarf neu berechnet.

Eine vollständige Cache-Invalidierung SOLL nur erfolgen, wenn eine präzise Invalidierung nicht möglich oder unwirtschaftlich ist.

## 7. Material und Backdrop Dependencies

Nova Composition Cache MUSS materialabhängige Änderungen berücksichtigen.

Insbesondere bei:

- Acrylic
- Liquid Glass
- Frosted Glass
- Crystal
- Blur
- Refraction
- Reflection

Änderungen an Hintergrundinhalten können gespeicherte Kompositionsergebnisse invalidieren.

Die Material Engine stellt die erforderlichen Abhängigkeiten bereit.

Gültige unabhängige Material-Zwischenergebnisse SOLLEN erhalten bleiben.

## 8. Cache Reuse

NovaOS SOLL Cache-Ergebnisse ohne erneute Komposition verwenden können.

Dabei MUSS sichergestellt sein, dass:

- Der gespeicherte Inhalt vollständig gültig ist.
- Die Zielregion korrekt zugeordnet wird.
- Ressourcenformate kompatibel sind.
- Erforderliche GPU-Synchronisation abgeschlossen ist.
- Keine unberücksichtigten Abhängigkeiten bestehen.

Die Wiederverwendung DARF keine veralteten Bildinhalte erzeugen.

## 9. Cache Budget und Eviction

Der Speicherverbrauch MUSS begrenzt werden können.

NovaOS SOLL Cache-Einträge anhand folgender Kriterien verwalten:

- Speicherbedarf
- Wiederverwendungshäufigkeit
- Letzte Verwendung
- Rekonstruktionskosten
- Aktueller Speicherdruck

Nicht mehr benötigte Cache-Einträge DÜRFEN automatisch entfernt werden.

Die Entfernung eines Cache-Eintrags DARF den zugrunde liegenden Composition State nicht verändern.

## 10. Adaptive Caching

NovaOS SOLL entscheiden können, ob die Zwischenspeicherung eines Ergebnisses wirtschaftlich ist.

Dabei werden berücksichtigt:

- Berechnungskosten
- Speicherverbrauch
- Erwartete Wiederverwendung
- Häufigkeit der Invalidierung
- GPU-Speicherbandbreite

Kurzlebige oder häufig veränderte Ergebnisse DÜRFEN ohne Cache verarbeitet werden.

Aufwendige, selten veränderte Kompositionen SOLLEN bevorzugt zwischengespeichert werden.

## 11. Synchronization und Resource Lifetime

Cache-Ressourcen MÜSSEN versionssicher verwaltet werden.

Dabei gilt:

- Laufende GPU-Zugriffe müssen berücksichtigt werden.
- Verwendete Ressourcen dürfen nicht vorzeitig freigegeben werden.
- Ungültige Einträge dürfen nicht erneut veröffentlicht werden.
- Cache-Eviction darf laufende Rendering-Aufträge nicht beschädigen.

Die Synchronisation erfolgt über Nova GPU Synchronization und GPU Resource Management.

## 12. Integration

Nova Composition Cache arbeitet mit:

- **Nova Incremental Composition:** Wiederverwendung bestehender Kompositionsergebnisse.
- **Nova Damage Tracking:** Regionale Invalidierung.
- **Nova Composition Layer Management:** Layer- und Gruppenabhängigkeiten.
- **Nova Material Engine:** Material- und Backdrop-Abhängigkeiten.
- **Nova Render Graph:** Einbindung gespeicherter Ergebnisse.
- **Nova GPU Resource Management:** Speicherverwaltung.
- **Nova Frame Scheduler:** Bedarfsgesteuerte Aktualisierung.

Der Composition Cache speichert fertige Kompositionsergebnisse und ersetzt weder den Scene Cache noch den Render Command Cache.

## 13. Fehlerbehandlung

NovaOS MUSS ungültige Cache-Zustände erkennen können.

Dazu gehören:

- Veraltete Abhängigkeitsversionen
- Fehlende GPU-Ressourcen
- Ungültige Bildbereiche
- Nicht abgeschlossene Synchronisation
- Inkompatible Ausgabeparameter

Bei ungültigen Cache-Einträgen MUSS das Ergebnis regulär neu berechnet werden können.

Cache-Fehler DÜRFEN nicht zu dauerhaft fehlerhaften Bildinhalten führen.

## 14. Verbindliche Anforderungen

1. NovaOS MUSS Composition Caching unterstützen.
2. Cache-Einträge MÜSSEN eindeutig identifizierbar und versioniert sein.
3. Regionale Cache-Invalidierung MUSS möglich sein.
4. Gültige Kompositionsergebnisse SOLLEN wiederverwendet werden.
5. Surface-, Layer- und Materialabhängigkeiten MÜSSEN berücksichtigt werden.
6. Backdrop-Änderungen MÜSSEN abhängige Cache-Bereiche invalidieren.
7. Cache-Gültigkeit MUSS vor jeder Wiederverwendung geprüft werden.
8. Speicherverbrauch MUSS begrenzbar sein.
9. Nicht benötigte Cache-Einträge DÜRFEN automatisch entfernt werden.
10. Cache-Entscheidungen SOLLEN kostenbasiert erfolgen.
11. GPU-Ressourcen MÜSSEN korrekt synchronisiert werden.
12. Cache-Eviction DARF laufende Kompositionen nicht beeinträchtigen.
13. Ein Fallback ohne Composition Cache MUSS verfügbar sein.
14. Identische Eingaben MÜSSEN im deterministischen Modus reproduzierbare Ergebnisse erzeugen.

## 15. Ergebnis

Nova Composition Cache ermöglicht die effiziente Wiederverwendung bereits zusammengesetzter grafischer Inhalte.

Durch regionale Invalidierung, versionsbasierte Abhängigkeitsprüfung und adaptive Speicherverwaltung werden unnötige Kompositionsberechnungen vermieden.

Damit reduziert NovaOS den CPU-, GPU- und Speicherbandbreitenbedarf und unterstützt eine flüssige, ressourcenschonende Desktop-Oberfläche.