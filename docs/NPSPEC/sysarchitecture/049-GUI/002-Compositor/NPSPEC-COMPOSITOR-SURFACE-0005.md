# NPSPEC-COMPOSITOR-SURFACE-0005 – Nova Surface Damage Tracking

**Status:** Angenommen  
**Bereich:** Compositor / Surface-Aktualisierung  
**System:** NovaOS

## 1. Zweck

Nova Surface Damage Tracking definiert die Erkennung, Verwaltung und Verarbeitung veränderter Bildbereiche grafischer Oberflächen.

Ziel ist die Minimierung unnötiger Rendering-Operationen, Speichertransfers und GPU-Auslastung durch gezielte Aktualisierung ausschließlich betroffener Bildbereiche.

## 2. Architektur

- Integration in Nova Surface Management und Nova Compositor Pipeline.
- Ereignisgesteuerte Erfassung von Surface-Änderungen.
- Verwaltung von Damage-Regionen pro Surface.
- Unterstützung von Surface- und Buffer-Koordinaten.
- Hierarchische Weitergabe von Damage-Regionen.
- Integration in Scene Manager und Frame Scheduler.
- Unterstützung partieller Bildkomposition.
- Hardwareunabhängige Damage-Verarbeitung.

## 3. Kernfunktionen

- Erfassung veränderter Surface-Bereiche.
- Verwaltung mehrerer Damage-Rechtecke.
- Zusammenfassung überlappender Damage-Regionen.
- Transformation lokaler Damage-Koordinaten.
- Berücksichtigung von Skalierung und Rotation.
- Erkennung von Änderungen an Position und Sichtbarkeit.
- Weitergabe von Damage-Regionen an die Kompositionspipeline.
- Optimierung verdeckter und unveränderter Bereiche.
- Unterstützung partieller Buffer-Aktualisierungen.
- Verwaltung von Damage-Historien.
- Automatischer Full-Damage-Fallback bei Bedarf.

## 4. Normative Anforderungen

- SURFACE-001: Jede Surface MUSS Damage-Regionen verwalten können.
- SURFACE-002: Damage-Regionen MÜSSEN eindeutig einem Surface-Zustand zugeordnet werden.
- SURFACE-003: Änderungen MÜSSEN spätestens beim Surface-Commit übernommen werden.
- SURFACE-004: Surface- und Buffer-Koordinaten MÜSSEN eindeutig unterschieden werden.
- SURFACE-005: Damage-Regionen MÜSSEN bei Transformationen korrekt umgerechnet werden.
- SURFACE-006: Positionierungsänderungen MÜSSEN sowohl bisherige als auch neue Bildbereiche berücksichtigen.
- SURFACE-007: Änderungen der Sichtbarkeit MÜSSEN entsprechende Damage-Regionen erzeugen.
- SURFACE-008: Änderungen von Transparenz und Effekten MÜSSEN berücksichtigt werden.
- SURFACE-009: Damage-Regionen MÜSSEN auf gültige Verarbeitungsbereiche begrenzt werden.
- SURFACE-010: Überlappende Damage-Regionen SOLLEN zusammengefasst werden.
- SURFACE-011: Vollständig verdeckte Bereiche SOLLEN keine unnötige Bildkomposition verursachen.
- SURFACE-012: Hierarchische Surface-Änderungen MÜSSEN korrekt an die Szenenverwaltung weitergegeben werden.
- SURFACE-013: Bei unbekanntem oder ungültigem Damage-Zustand MUSS eine vollständige Aktualisierung möglich sein.
- SURFACE-014: Die Wiederverwendung älterer Buffer MUSS deren Aktualisierungshistorie berücksichtigen.
- SURFACE-015: Damage-Optimierungen DÜRFEN keine sichtbaren Darstellungsfehler verursachen.
- SURFACE-016: Die Anzahl verwalteter Damage-Regionen MUSS begrenzbar sein.
- SURFACE-017: Unveränderte Surfaces SOLLEN keine unnötigen Rendering-Aufgaben auslösen.
- SURFACE-018: Damage Tracking MUSS mit Nova Surface Synchronization kompatibel sein.

## 5. Damage-Arten

### Surface Damage
Veränderte Bereiche innerhalb des lokalen Surface-Koordinatensystems.

### Buffer Damage
Veränderte Pixelbereiche innerhalb des zugeordneten Grafikbuffers.

### Geometry Damage
Änderungen durch Positionierung, Skalierung, Rotation oder Größenanpassung.

### Visibility Damage
Änderungen durch Einblenden, Ausblenden oder Überlagerung von Surfaces.

### Effect Damage
Zusätzliche betroffene Bereiche durch Schatten, Unschärfe und andere Grafikeffekte.

### Full Damage
Vollständige Aktualisierung einer Surface oder eines betroffenen Ausgabebereichs.

## 6. Damage-Verarbeitung

1. **Detect:** Veränderungen einer Surface erfassen.
2. **Collect:** Damage-Regionen sammeln.
3. **Transform:** Regionen in den erforderlichen Koordinatenraum übertragen.
4. **Merge:** Überlappende oder benachbarte Regionen zusammenfassen.
5. **Clip:** Regionen auf relevante Bildbereiche begrenzen.
6. **Analyze:** Sichtbarkeit und Überlagerungen berücksichtigen.
7. **Schedule:** Erforderliche Rendering-Operationen planen.
8. **Render:** Betroffene Bereiche aktualisieren.
9. **Commit:** Aktualisierungszustand nach erfolgreicher Verarbeitung bestätigen.

Damage-Informationen DÜRFEN erst verworfen werden, wenn die zugehörige Aktualisierung nicht mehr benötigt wird.

## 7. Optimierungsstrategien

### Region Merging
Zusammenfassung mehrerer Damage-Rechtecke zur Reduzierung des Verwaltungsaufwands.

### Partial Rendering
Neuberechnung ausschließlich betroffener Bildbereiche.

### Occlusion Optimization
Ausschluss vollständig verdeckter Bereiche von unnötigen Rendering-Operationen.

### Damage History
Berücksichtigung vorheriger Aktualisierungen bei der Wiederverwendung von Grafikbuffern.

### Adaptive Full Damage
Umschaltung auf vollständige Aktualisierung, wenn die Verwaltung einzelner Regionen aufwendiger als ein vollständiges Rendering wird.

## 8. Performance und Zuverlässigkeit

- Minimierung unnötiger GPU- und CPU-Operationen.
- Reduzierung von Speicherbandbreite und Energieverbrauch.
- Effiziente Speicherung von Damage-Regionen.
- Vermeidung unnötiger vollständiger Frame-Aktualisierungen.
- Unterstützung hardwarebeschleunigter partieller Komposition.
- Begrenzung komplexer Damage-Strukturen.
- Kontrollierter Full-Damage-Fallback.
- Korrekte Behandlung verlorener oder wiederverwendeter Buffer.
- Wiederherstellung gültiger Damage-Zustände nach Compositor-Fehlern.
- Diagnose von Damage-Flächen und eingesparten Rendering-Operationen.

## 9. Ergebnis

NovaOS erhält ein effizientes und zuverlässiges System zur Erkennung und Verarbeitung veränderter Bildbereiche.

Der Compositor aktualisiert bevorzugt nur tatsächlich betroffene Regionen und vermeidet unnötige Berechnungen.

Dadurch werden Rendering-Aufwand, Speicherbandbreite und Energieverbrauch reduziert, während eine konsistente und flüssige Darstellung gewährleistet bleibt.