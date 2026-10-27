# NPSPEC-GRAPHICS-PERF-0001 – Nova Graphics Caching

**Status:** Angenommen  
**Bereich:** Graphics / Performance / Caching  
**System:** NovaOS

## 1. Zweck

Nova Graphics Caching definiert die zentrale Architektur zur Zwischenspeicherung und Wiederverwendung grafischer Ressourcen und berechneter Darstellungsergebnisse.

Ziel ist die Reduzierung unnötiger Berechnungen, Speicherübertragungen und Rendering-Operationen.

Das Caching soll die Reaktionsgeschwindigkeit von NovaOS verbessern, ohne die Speicherökonomie, Darstellungsqualität oder Systemstabilität zu beeinträchtigen.

## 2. Architektur

Das Subsystem besteht aus folgenden Komponenten:

| Komponente | Aufgabe |
|---|---|
| Graphics Cache Manager | Zentrale Koordination der Grafik-Caches |
| Cache Registry | Registrierung und Verwaltung von Cache-Einträgen |
| Cache Policy Manager | Steuerung von Prioritäten und Speicherlimits |
| Cache Dependency Tracker | Überwachung von Ressourcenabhängigkeiten |
| Cache Invalidation Manager | Gezielte Ungültigkeitserklärung |
| Cache Memory Manager | Verwaltung des Cache-Speichers |
| Cache Statistics | Erfassung von Nutzung und Effizienz |

Die eigentlichen Cache-Daten verbleiben bei den jeweiligen Grafiksubsystemen.

Der Graphics Cache Manager koordiniert deren Ressourcenverbrauch und Lebenszyklus.

## 3. Cache-Kategorien

Nova Graphics Caching unterstützt:

- **Texture Cache:** Texturen und vorbereitete Bilddaten.
- **Glyph Cache:** Rasterisierte Schriftzeichen und Glyph Atlases.
- **Shader Cache:** Kompilierte Shader und Pipeline-Varianten.
- **Geometry Cache:** Vorbereitete Geometrien und Tessellierungsergebnisse.
- **Render Cache:** Wiederverwendbare Rendering-Ergebnisse.
- **Surface Cache:** Zwischengespeicherte Oberflächeninhalte.
- **Effect Cache:** Berechnete Effekte und Filterergebnisse.
- **Pipeline Cache:** Wiederverwendbare Rendering-Pipeline-Zustände.

Jedes Subsystem verwaltet seine spezifischen Cache-Inhalte über eine gemeinsame Cache-Schnittstelle.

## 4. Cache-Modell

Ein Cache-Eintrag besitzt mindestens:

- Eindeutigen Cache-Schlüssel.
- Ressourcentyp.
- Speicherbedarf.
- Speicherort.
- Gültigkeitszustand.
- Abhängigkeiten.
- Nutzungsinformationen.
- Priorität.
- Referenz- und Lebenszyklusinformationen.

Cache-Schlüssel MÜSSEN sämtliche Parameter berücksichtigen, die das gespeicherte Ergebnis beeinflussen.

Identische Eingaben und Konfigurationen SOLLEN vorhandene Cache-Ergebnisse wiederverwenden.

## 5. Cache-Hierarchie

NovaOS unterscheidet:

1. **CPU Cache:** Zwischenspeicherung im Arbeitsspeicher.
2. **GPU Cache:** Wiederverwendbare Ressourcen im Grafikspeicher.
3. **Shared Cache:** Gemeinsam nutzbare CPU-/GPU-Ressourcen.

Ein optionaler persistenter Cache kann beispielsweise kompilierte Shader oder Pipeline-Daten speichern.

Persistente Cache-Einträge MÜSSEN auf Kompatibilität mit Softwareversion, Backend und Hardware geprüft werden.

Eine dauerhafte Speicherung temporärer Rendering-Ergebnisse ist nicht erforderlich.

## 6. Cache-Invalidierung

Cache-Einträge werden ungültig, wenn sich relevante Eingaben oder Abhängigkeiten ändern.

Dazu gehören:

- Änderungen an Grafikressourcen.
- Änderungen an Transformationsparametern.
- Änderungen an Shadern oder Materialien.
- Änderungen an Schrift- und Textparametern.
- Änderungen an Rendering-Einstellungen.
- Änderungen an Backend- oder Gerätezuständen.

Die Invalidierung MUSS möglichst gezielt erfolgen.

Unabhängige Cache-Einträge DÜRFEN durch lokale Änderungen nicht unnötig verworfen werden.

Veraltete Ergebnisse DÜRFEN nicht als gültige Rendering-Ergebnisse verwendet werden.

## 7. Speicherverwaltung

Der Graphics Cache Manager verwaltet definierte Speicherbudgets.

Unterstützt werden:

- Globale und subsystembezogene Speicherlimits.
- Prioritätsbasierte Cache-Freigabe.
- LRU-basierte Verdrängungsstrategien.
- Bedarfsgerechte Speicherbereinigung.
- Anpassung an verfügbaren Arbeitsspeicher.
- Anpassung an verfügbaren GPU-Speicher.

Cache-Speicher besitzt grundsätzlich eine niedrigere Priorität als aktiv benötigte Grafikressourcen.

Bei Speicherdruck SOLLEN entbehrliche Cache-Einträge automatisch freigegeben werden.

Aktiv verwendete Ressourcen DÜRFEN nicht vorzeitig freigegeben werden.

## 8. Rendering-Optimierung

Das Caching ermöglicht:

- Wiederverwendung unveränderter Oberflächenbereiche.
- Vermeidung wiederholter Glyphenrasterisierung.
- Wiederverwendung vorbereiteter Texturen.
- Reduzierung wiederholter Shader-Kompilierung.
- Wiederverwendung berechneter Geometrien.
- Vermeidung unnötiger CPU-GPU-Übertragungen.
- Reduzierung redundanter Rendering-Operationen.

Der Compositor kann unveränderte Oberflächeninhalte wiederverwenden.

Dynamische Inhalte werden nur dann gecacht, wenn ein tatsächlicher Leistungsvorteil zu erwarten ist.

## 9. Parallelität und Synchronisation

Cache-Zugriffe können aus mehreren Threads und Rendering-Kontexten erfolgen.

Dabei gilt:

- Gemeinsame Cache-Strukturen MÜSSEN threadsicher sein.
- Gleichzeitige identische Berechnungen SOLLEN zusammengeführt werden.
- Cache-Einträge DÜRFEN erst nach vollständiger Erstellung veröffentlicht werden.
- GPU-Ressourcen DÜRFEN erst nach Abschluss ihrer Verwendung freigegeben werden.
- Synchronisation MUSS unnötige Blockierungen vermeiden.

Die GPU-Synchronisation erfolgt über Nova GPU Command Submission.

## 10. Fehlerbehandlung und Recovery

Cache-Daten gelten grundsätzlich als rekonstruierbare Ressourcen.

Bei Fehlern gilt:

- Beschädigte Cache-Einträge werden verworfen.
- Fehlgeschlagene Cache-Allokationen dürfen das Rendering nicht grundsätzlich verhindern.
- Cache-Misses lösen die reguläre Berechnung aus.
- GPU-Verlust invalidiert betroffene GPU-Cache-Einträge.
- Nach einer Gerätewiederherstellung werden benötigte Ressourcen erneut aufgebaut.

Ein Cache-Fehler DARF keine gültigen Originalressourcen beschädigen.

## 11. Integration

- **Nova Graphics Resource Lifecycle:** Speicherverwaltung und Ressourcenfreigabe.
- **Nova Graphics Backend Abstraction:** CPU-/GPU-spezifische Cache-Verwaltung.
- **Nova Texture Management:** Textur-Caching.
- **Nova Shader Management:** Shader- und Pipeline-Caching.
- **Nova Font Rasterization:** Glyphen-Caching.
- **Nova 2D/3D Rendering:** Wiederverwendung vorberechneter Ressourcen.
- **Nova Compositor:** Wiederverwendung unveränderter Oberflächen.
- **Nova Graphics Driver Recovery:** Wiederaufbau nach GPU-Ausfällen.

Das Caching bleibt eine Optimierung und DARF keine funktionale Voraussetzung für korrektes Rendering darstellen.

## 12. Normative Anforderungen

- **GRAPHICS-001:** NovaOS MUSS eine zentrale Koordination grafischer Caches bereitstellen.
- **GRAPHICS-002:** Grafiksubsysteme MÜSSEN eigene Cache-Implementierungen registrieren können.
- **GRAPHICS-003:** Cache-Schlüssel MÜSSEN alle ergebnisrelevanten Parameter berücksichtigen.
- **GRAPHICS-004:** Cache-Einträge MÜSSEN eindeutig identifizierbar sein.
- **GRAPHICS-005:** Änderungen relevanter Ressourcen MÜSSEN betroffene Cache-Einträge invalidieren.
- **GRAPHICS-006:** CPU- und GPU-Caches MÜSSEN unterstützt werden.
- **GRAPHICS-007:** Cache-Speicher MUSS durch definierte Budgets begrenzt werden.
- **GRAPHICS-008:** Entbehrliche Cache-Einträge MÜSSEN bei Speicherdruck freigegeben werden können.
- **GRAPHICS-009:** Aktiv verwendete Ressourcen DÜRFEN nicht vorzeitig freigegeben werden.
- **GRAPHICS-010:** Gemeinsame Cache-Zugriffe MÜSSEN threadsicher erfolgen.
- **GRAPHICS-011:** Cache-Einträge DÜRFEN erst nach erfolgreicher Erstellung verwendet werden.
- **GRAPHICS-012:** GPU-Cache-Ressourcen MÜSSEN korrekt synchronisiert werden.
- **GRAPHICS-013:** Cache-Ausfälle DÜRFEN die Korrektheit des Renderings nicht beeinträchtigen.
- **GRAPHICS-014:** Cache-Statistiken SOLLEN für Diagnose und Optimierung verfügbar sein.
- **GRAPHICS-015:** Das Caching MUSS ohne Netzwerk- oder KI-Abhängigkeit funktionieren.

## 13. Ergebnis

NovaOS erhält eine einheitliche und ressourcenschonende Cache-Architektur für sämtliche Grafiksubsysteme.

Wiederkehrende Berechnungen und Speicherübertragungen werden reduziert, während Speicherverbrauch und Ressourcenlebenszyklen zentral kontrollierbar bleiben.

Die Architektur unterstützt sowohl CPU-basiertes Rendering als auch moderne GPU-Beschleunigung.

**Grundsatz:** NovaOS berechnet grafische Inhalte nur dann erneut, wenn sich ihre relevanten Eingaben geändert haben oder eine Wiederverwendung keinen Vorteil bietet.