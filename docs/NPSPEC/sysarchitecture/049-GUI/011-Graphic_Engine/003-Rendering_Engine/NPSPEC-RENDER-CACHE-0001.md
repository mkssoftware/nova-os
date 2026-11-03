# NPSPEC-RENDER-CACHE-0001 – Nova Render Result Caching

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Render Engine

## 1. Zweck

Nova Render Result Caching definiert die Zwischenspeicherung und Wiederverwendung bereits berechneter grafischer Darstellungsergebnisse.

Ziel ist die Vermeidung redundanter Rendering-Operationen durch die Wiederverwendung gültiger Pixel-, Effekt- und Render-Pass-Ergebnisse.

Das Caching MUSS unabhängig vom persistenten Nova Scene Graph funktionieren.

## 2. Architekturprinzipien

- **Result-Oriented:** Gespeichert werden berechnete Rendering-Ergebnisse.
- **Dependency-Aware:** Die Gültigkeit hängt von relevanten Eingaben und Abhängigkeiten ab.
- **Incremental:** Nur ungültige Ergebnisse werden neu berechnet.
- **Region-Aware:** Ergebnisse können räumlich begrenzt gespeichert werden.
- **Versioned:** Cache-Einträge besitzen nachvollziehbare Zustandsinformationen.
- **Resource-Aware:** Speicherverbrauch und Ressourcenlebensdauer werden kontrolliert.
- **Adaptive:** Caching erfolgt abhängig von Kosten und Wiederverwendungspotenzial.
- **Disposable:** Cache-Inhalte können jederzeit verworfen und neu erzeugt werden.

## 3. Cache-Modell

Ein Render Cache Entry beschreibt ein wiederverwendbares Rendering-Ergebnis.

Er enthält mindestens:

- **Cache ID:** Eindeutige Identifikation.
- **Result Type:** Art des gespeicherten Ergebnisses.
- **Target:** Zugehöriges Rendering-Ziel.
- **Region:** Gültiger räumlicher Bereich.
- **State Version:** Relevante grafische Zustandsversion.
- **Dependency Signature:** Signatur der verwendeten Eingaben.
- **Resource References:** Benötigte Grafikressourcen.
- **Cached Result:** Gespeichertes Rendering-Ergebnis.
- **Validity State:** Aktueller Gültigkeitszustand.

Die konkrete interne Datenstruktur bleibt implementierungsabhängig.

## 4. Cache-Kategorien

NovaOS SOLL folgende Rendering-Ergebnisse zwischenspeichern können:

- **Surface Cache:** Gerenderte Oberflächeninhalte.
- **Region Cache:** Ergebnisse einzelner Bildbereiche.
- **Render Pass Cache:** Ausgaben abgeschlossener Render Passes.
- **Effect Cache:** Vorberechnete grafische Effekte.
- **Material Cache:** Materialspezifische Zwischenergebnisse.
- **Text Cache:** Gerasterte Textdarstellungen.
- **Geometry Raster Cache:** Bereits gerasterte Geometrie.

Die Speicherung erfolgt nur, wenn eine spätere Wiederverwendung sinnvoll ist.

## 5. Cache-Gültigkeit

Ein Cache-Eintrag DARF ausschließlich wiederverwendet werden, wenn sämtliche relevanten Eingaben unverändert oder nachweislich kompatibel sind.

Dazu gehören insbesondere:

- Grafische Eigenschaften
- Geometrie und Transformationen
- Ressourceninhalte
- Materialparameter
- Shader- und Pipeline-Konfiguration
- Render-Target-Format und Auflösung
- Farbraum und Ausgabeparameter
- Grafische Effektabhängigkeiten

Eine neue globale Szenenversion MUSS nicht automatisch sämtliche Cache-Einträge invalidieren.

Entscheidend ist die Gültigkeit ihrer tatsächlichen Abhängigkeiten.

## 6. Dependency-Based Invalidation

Nova Graphics Dependency Tracking bestimmt, welche Rendering-Ergebnisse von einer Änderung betroffen sind.

NovaOS MUSS:

- Direkte und transitive Abhängigkeiten berücksichtigen.
- Betroffene Cache-Einträge gezielt invalidieren.
- Unabhängige Ergebnisse erhalten.
- Ressourcenänderungen erkennen.
- Material- und Backdrop-Abhängigkeiten berücksichtigen.

Bei unvollständigen Abhängigkeitsinformationen MUSS eine konservative Invalidierung erfolgen.

## 7. Region-Based Caching

Rendering-Ergebnisse DÜRFEN in räumlich begrenzten Bereichen gespeichert werden.

Bei einer Änderung SOLLEN ausschließlich betroffene Regionen invalidiert werden.

Dabei MUSS NovaOS:

- Damage Regions berücksichtigen.
- Die Gültigkeit einzelner Regionen verwalten.
- Effektabhängigkeiten über Regionsgrenzen hinweg beachten.
- Unveränderte Bereiche wiederverwenden.

Regionale Cache-Einträge DÜRFEN zusammengefasst oder aufgeteilt werden.

## 8. Material- und Effekt-Caching

Grafische Effekte können besonders rechenintensiv sein.

NovaOS SOLL Zwischenergebnisse insbesondere für folgende Effekte wiederverwenden:

- Blur
- Schatten
- Refraction
- Reflection
- Acrylic
- Frosted Glass
- Liquid Glass
- Crystal

Bei backdropabhängigen Materialien MUSS die Gültigkeit des verwendeten Hintergrundinhalts berücksichtigt werden.

Eine unveränderte Materialeigenschaft allein garantiert keine Gültigkeit des Effektergebnisses.

## 9. Cache Lifecycle

Ein Cache-Eintrag kann folgende Zustände besitzen:

- **Missing:** Kein Ergebnis vorhanden.
- **Valid:** Ergebnis vollständig wiederverwendbar.
- **Invalid:** Ergebnis muss neu berechnet werden.
- **Computing:** Neuberechnung läuft.
- **Evictable:** Ergebnis kann freigegeben werden.

Ein Ergebnis DARF erst nach erfolgreichem Abschluss aller erforderlichen Rendering-Operationen als gültig veröffentlicht werden.

## 10. Cache Reuse

Vor der Ausführung eines Rendering-Auftrags SOLL die Render Engine prüfen, ob ein gültiges Ergebnis vorhanden ist.

Bei einem Cache Hit DARF die erneute Rendering-Ausführung entfallen.

Bei einem Cache Miss wird das Ergebnis regulär berechnet.

Nicht jedes Rendering-Ergebnis MUSS zwischengespeichert werden.

Die Entscheidung DARF anhand von Berechnungskosten, Speicherbedarf und erwarteter Wiederverwendung erfolgen.

## 11. Speicherverwaltung

Der Nova GPU Resource Manager verwaltet die zugrunde liegenden Grafikressourcen.

NovaOS MUSS:

- Speicherbudgets berücksichtigen.
- Cache-Ressourcen kontrolliert freigeben können.
- Aktive GPU-Zugriffe berücksichtigen.
- Ressourcenreferenzen gültig halten.
- Unnötige Datenkopien vermeiden.

Bei Speicherknappheit DÜRFEN Cache-Einträge verdrängt werden.

Die Verdrängung DARF den persistenten Szenenzustand nicht verändern.

## 12. Cache Eviction

NovaOS DARF unterschiedliche Verdrängungsstrategien verwenden.

Mögliche Kriterien sind:

- Letzter Zugriff
- Wiederverwendungshäufigkeit
- Speicherbedarf
- Berechnungskosten
- Sichtbarkeit
- Ressourcenpriorität

Cache Eviction MUSS unabhängig von der Lebensdauer der ursprünglichen Scene Nodes möglich sein.

Benötigte Ressourcen DÜRFEN nicht während laufender GPU-Zugriffe freigegeben werden.

## 13. Render Graph Integration

Der Nova Render Graph SOLL gültige Cache-Ergebnisse als bereits verfügbare Pass-Ausgaben berücksichtigen.

Dadurch können:

- Unnötige Render Passes entfallen.
- Vorhandene Ergebnisse als Eingaberessourcen verwendet werden.
- Abhängige Passes gezielt aktualisiert werden.
- Temporäre Ressourcen eingespart werden.

Ein Cache Hit DARF erforderliche Synchronisations- und Ressourcenübergänge nicht umgehen.

## 14. Parallelität und Synchronisation

Cache-Einträge DÜRFEN von mehreren Rendering-Aufträgen gemeinsam verwendet werden.

NovaOS MUSS:

- Gleichzeitige Zugriffe koordinieren.
- Unvollständige Ergebnisse vor Verwendung schützen.
- Ressourcenlebenszeiten gewährleisten.
- Veraltete Berechnungsergebnisse erkennen.
- CPU- und GPU-Synchronisation berücksichtigen.

Mehrere gleichzeitige Berechnungen desselben Ergebnisses SOLLEN nach Möglichkeit vermieden werden.

## 15. Abgrenzung

Die Verantwortlichkeiten sind eindeutig getrennt:

- **Scene Cache:** Speichert abgeleitete logische Szeneninformationen.
- **Render Result Cache:** Speichert berechnete grafische Darstellungsergebnisse.
- **Render Command Reuse:** Verwendet gültige Rendering-Befehlsfolgen erneut.
- **GPU Resource Cache:** Verwaltet wiederverwendbare GPU-Ressourcen.
- **Composition Cache:** Speichert Ergebnisse der Oberflächenkomposition.

Diese Caches DÜRFEN gemeinsame Ressourcen verwenden, müssen jedoch ihre jeweiligen Gültigkeitsregeln einhalten.

## 16. Fehlerbehandlung

Bei ungültigen oder fehlenden Cache-Ergebnissen MUSS NovaOS eine reguläre Neuberechnung ermöglichen.

Fehlgeschlagene Cache-Operationen DÜRFEN keine ungültigen Darstellungsergebnisse veröffentlichen.

Der Verlust sämtlicher Render Cache Entries DARF die grundsätzliche Rendering-Funktion nicht beeinträchtigen.

## 17. Verbindliche Anforderungen

1. NovaOS MUSS Render Result Caching unterstützen.
2. Cache-Einträge MÜSSEN eindeutig identifizierbar sein.
3. Cache-Ergebnisse MÜSSEN vor ihrer Wiederverwendung validiert werden.
4. Grafische Abhängigkeiten MÜSSEN bei der Invalidierung berücksichtigt werden.
5. Unabhängige Cache-Einträge SOLLEN erhalten bleiben.
6. Region-Based Caching MUSS unterstützt werden können.
7. Material- und Backdrop-Abhängigkeiten MÜSSEN berücksichtigt werden.
8. Gültige Cache-Ergebnisse SOLLEN erneute Rendering-Operationen vermeiden.
9. Cache-Einträge MÜSSEN unabhängig vom persistenten Scene Graph freigegeben werden können.
10. Speicherbudgets MÜSSEN berücksichtigt werden.
11. Aktive GPU-Zugriffe DÜRFEN durch Cache Eviction nicht beeinträchtigt werden.
12. Der Render Graph MUSS gültige Cache-Ergebnisse verwenden können.
13. Unvollständige Ergebnisse DÜRFEN nicht als gültig veröffentlicht werden.
14. Bei Cache Miss oder Invalidierung MUSS eine Neuberechnung möglich sein.
15. Cache-Optimierungen DÜRFEN die visuelle Korrektheit nicht beeinträchtigen.

## 18. Ergebnis

Nova Render Result Caching ermöglicht die effiziente Wiederverwendung bereits berechneter grafischer Darstellungsergebnisse.

Durch abhängigkeitsbasierte Invalidierung, regionale Zwischenspeicherung und kontrollierte Ressourcenverwaltung reduziert NovaOS redundante Rendering-Operationen.

Das Caching bleibt vollständig von der persistenten Szenenbeschreibung getrennt und kann bei Bedarf ohne Verlust des grafischen Zustands verworfen werden.

Damit bildet Render Result Caching eine wesentliche Grundlage für Incremental Rendering, Partial Rendering und die ressourcenschonende Darstellung komplexer Materialeffekte.