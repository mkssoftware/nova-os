# NPSPEC-DAMAGE-OCCLUSION-0001 – Nova Occlusion-Aware Damage Tracking

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Damage Tracking

## 1. Zweck

Nova Occlusion-Aware Damage Tracking definiert die sichtbarkeitsabhängige Verarbeitung grafischer Änderungen.

Ziel ist, die Neuberechnung vollständig verdeckter Bildbereiche zu vermeiden, ohne die korrekte Darstellung bei späterer Sichtbarkeit oder bei Materialeffekten zu beeinträchtigen.

## 2. Architekturprinzipien

- **Visibility-Aware:** Damage wird anhand tatsächlicher Sichtbarkeit bewertet.
- **Region-Based:** Verdeckung wird räumlich begrenzt analysiert.
- **Incremental:** Änderungen der Sichtbarkeit werden gezielt verarbeitet.
- **Material-Aware:** Transparenz und Hintergrundabhängigkeiten bleiben berücksichtigt.
- **Deferred:** Verdeckte Aktualisierungen können zurückgestellt werden.
- **Conservative:** Unsichere Sichtbarkeitsinformationen führen zu vollständiger Berücksichtigung.
- **Deterministic:** Identische Zustände erzeugen reproduzierbare Ergebnisse.

## 3. Occlusion Model

NovaOS unterscheidet:

| Zustand | Bedeutung |
|---|---|
| Visible | Bereich ist sichtbar und benötigt gegebenenfalls Aktualisierung |
| Partially Occluded | Bereich ist teilweise verdeckt |
| Fully Occluded | Bereich ist vollständig verdeckt |
| Effect-Dependent | Bereich wird von einem sichtbaren Effekt benötigt |
| Unknown | Sichtbarkeit kann nicht zuverlässig bestimmt werden |

Die Sichtbarkeit MUSS für die jeweilige Surface, Layer oder Ausgabe bewertet werden.

## 4. Occlusion Analysis

Die Analyse berücksichtigt:

- Position und Größe von Surfaces
- Z-Order und Layer-Reihenfolge
- Clipping und Transformationen
- Opacity und Alpha-Masken
- Materialeigenschaften
- Sichtbare Ausgabegrenzen

Nur nachweislich vollständig opake Bereiche DÜRFEN darunterliegende Inhalte vollständig verdecken.

Transparente oder effektabhängige Oberflächen MÜSSEN gesondert behandelt werden.

## 5. Damage Classification

NovaOS MUSS Damage anhand seiner Sichtbarkeit klassifizieren können.

Dabei gilt:

1. Sichtbarer Damage wird zur Aktualisierung weitergegeben.
2. Teilweise verdeckter Damage wird auf relevante Bereiche begrenzt.
3. Vollständig verdeckter Damage DARF zurückgestellt werden.
4. Effektabhängiger Damage MUSS entsprechend seiner Abhängigkeiten verarbeitet werden.
5. Unbekannte Sichtbarkeit MUSS konservativ behandelt werden.

Die Klassifizierung DARF keine erforderlichen Aktualisierungen unterdrücken.

## 6. Deferred Damage

Vollständig verdeckte Änderungen DÜRFEN von der aktuellen Rendering-Ausführung ausgeschlossen werden.

Dabei MUSS NovaOS zwischen zwei Fällen unterscheiden:

- **Retained Content:** Die verdeckte Surface benötigt bei erneuter Sichtbarkeit eine Aktualisierung.
- **Invalidated Cache:** Ein gespeichertes Rendering-Ergebnis ist ungültig und darf nicht unverändert wiederverwendet werden.

Zurückgestellter Damage MUSS erhalten bleiben, bis der betroffene Inhalt korrekt aktualisiert wurde.

Eine reine Verdeckung DARF nicht zur Bestätigung oder Löschung ausstehenden Damage führen.

## 7. Visibility Changes

Änderungen der Sichtbarkeit MÜSSEN neue Damage-Bereiche erzeugen können.

Dies betrifft insbesondere:

- Verschieben oder Schließen von Fenstern
- Änderungen der Z-Order
- Änderungen von Opacity
- Änderungen von Clipping
- Ein- und Ausblenden von Surfaces
- Änderungen an Materialeigenschaften

Bei erneut sichtbar werdenden Bereichen MUSS NovaOS prüfen, ob vorhandene Inhalte noch gültig sind.

Ungültige Inhalte MÜSSEN vor ihrer sichtbaren Verwendung aktualisiert werden.

## 8. Material-Aware Occlusion

NovaOS MUSS zwischen visueller Verdeckung und tatsächlicher Datenabhängigkeit unterscheiden.

Insbesondere bei:

- Acrylic
- Frosted Glass
- Liquid Glass
- Blur
- Refraction
- Reflection

können darunterliegende Inhalte trotz optischer Verdeckung für die Berechnung sichtbarer Effekte erforderlich sein.

Solche Abhängigkeiten MÜSSEN durch Nova Damage Dependency Analysis berücksichtigt werden.

Damage DARF nicht zurückgestellt werden, wenn dadurch ein sichtbares Materialergebnis veraltet.

## 9. Incremental Occlusion Tracking

NovaOS SOLL Sichtbarkeitsinformationen zwischen Scene-Aktualisierungen wiederverwenden.

Eine Neubewertung ist insbesondere erforderlich bei:

- Geometrieänderungen
- Änderungen der Ebenenreihenfolge
- Transparenzänderungen
- Materialänderungen
- Änderungen der Ausgabegeometrie

Unveränderte Sichtbarkeitsbereiche SOLLEN nicht erneut analysiert werden.

## 10. Integration

Nova Occlusion-Aware Damage Tracking arbeitet mit:

- **Scene Graph:** Geometrie und Hierarchie
- **Window Manager:** Fensterpositionen und Z-Order
- **Compositor:** Sichtbarkeit und Layer-Zusammensetzung
- **Damage Dependency Analysis:** Effektabhängigkeiten
- **Damage Propagation:** Weitergabe relevanter Änderungen
- **Render Engine:** Vermeidung unnötiger Rendering-Operationen
- **Frame Scheduler:** Bedarfsgesteuerte Frame-Erzeugung

Die tatsächliche Occlusion-Culling-Ausführung verbleibt beim zuständigen Rendering- beziehungsweise Compositor-Subsystem.

## 11. Versioning und Synchronization

Sichtbarkeitsinformationen MÜSSEN einem konsistenten Grafikzustand zugeordnet sein.

Dabei gilt:

- Occlusion-Ergebnisse müssen zur verwendeten Scene-Version passen.
- Neuere Änderungen dürfen nicht durch ältere Ergebnisse überschrieben werden.
- Zurückgestellter Damage muss versionssicher erhalten bleiben.
- Laufende Rendering-Aufträge dürfen keine ungültigen Sichtbarkeitsannahmen verwenden.

## 12. Fallback

Kann eine vollständige Verdeckung nicht sicher nachgewiesen werden, MUSS der betroffene Bereich als potenziell sichtbar behandelt werden.

NovaOS DARF in diesem Fall auf gewöhnliches Damage Tracking ohne Occlusion-Optimierung zurückfallen.

Grafische Korrektheit besitzt Vorrang vor eingesparter Rendering-Arbeit.

## 13. Verbindliche Anforderungen

1. NovaOS MUSS Damage anhand der tatsächlichen Sichtbarkeit bewerten können.
2. Vollständig und teilweise verdeckte Bereiche MÜSSEN unterscheidbar sein.
3. Nur nachweislich opake Bereiche DÜRFEN vollständige Verdeckung begründen.
4. Vollständig verdeckter Damage DARF zurückgestellt werden.
5. Zurückgestellter Damage MUSS bis zur erfolgreichen Aktualisierung erhalten bleiben.
6. Änderungen der Sichtbarkeit MÜSSEN erforderliche Aktualisierungen auslösen.
7. Transparente Materialien und Backdrop-Abhängigkeiten MÜSSEN berücksichtigt werden.
8. Effektabhängiger Damage DARF nicht fälschlich unterdrückt werden.
9. Gültige Sichtbarkeitsinformationen SOLLEN inkrementell wiederverwendet werden.
10. Occlusion-Ergebnisse MÜSSEN versionssicher verarbeitet werden.
11. Unbekannte Sichtbarkeit MUSS konservativ behandelt werden.
12. Ein Fallback ohne Occlusion-Optimierung MUSS verfügbar sein.
13. Identische Eingaben MÜSSEN im deterministischen Modus reproduzierbare Ergebnisse erzeugen.

## 14. Ergebnis

Nova Occlusion-Aware Damage Tracking verhindert unnötige Aktualisierungen vollständig verdeckter Bildbereiche.

Durch sichtbarkeitsabhängige Damage-Klassifizierung, versionssichere Zurückstellung und materialbewusste Abhängigkeitsanalyse bleiben Rendering-Ergebnisse korrekt.

Damit reduziert NovaOS die CPU- und GPU-Belastung, insbesondere bei überlappenden Fenstern und komplexen Desktop-Oberflächen.