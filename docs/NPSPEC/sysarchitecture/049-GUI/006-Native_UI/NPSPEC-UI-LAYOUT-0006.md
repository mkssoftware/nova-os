# NPSPEC-UI-LAYOUT-0006 – Nova Layout Measurement and Arrangement

**Status:** Angenommen  
**Bereich:** UI / Layout / Measurement and Arrangement  
**System:** NovaOS

## 1. Zweck

Nova Layout Measurement and Arrangement definiert das einheitliche Verfahren zur Berechnung von Größe und Position nativer UI-Elemente.

Das Verfahren bildet die Grundlage sämtlicher Layoutcontainer und Steuerelemente innerhalb von Nova UI.

Ziel ist eine deterministische, effiziente und bedarfsorientierte Layoutberechnung ohne unnötige Neuberechnungen.

## 2. Architektur

Das Subsystem umfasst folgende Komponenten:

| Komponente | Aufgabe |
|---|---|
| Layout Pass Manager | Koordination der Layoutdurchläufe |
| Measure Processor | Berechnung des benötigten Platzes |
| Arrange Processor | Berechnung endgültiger Positionen und Größen |
| Constraint Resolver | Verarbeitung von Größenbeschränkungen |
| Layout Dependency Tracker | Ermittlung betroffener Layoutabhängigkeiten |
| Layout Invalidation Controller | Planung notwendiger Neuberechnungen |

Die Verarbeitung erfolgt innerhalb der Nova Layout Engine und ist unabhängig vom Renderbackend.

## 3. Layoutverfahren

Die Layoutberechnung besteht aus zwei grundlegenden Phasen:

1. **Measure:** Ermittlung der gewünschten Größe.
2. **Arrange:** Zuweisung der endgültigen Position und Größe.

Jedes layoutfähige UI-Element MUSS beide Operationen unterstützen.

Die Berechnung erfolgt grundsätzlich hierarchisch.

Ein übergeordnetes Element bestimmt die verfügbaren Layoutbedingungen seiner untergeordneten Elemente.

## 4. Measure

Die Measure-Phase ermittelt den Platzbedarf eines UI-Elements.

Eingabe:

- Verfügbare Breite und Höhe.
- Mindest- und Maximalgrößen.
- Inhalt und Eigenschaften.
- Layoutbedingungen.
- Größenanforderungen untergeordneter Elemente.

Ergebnis:

- Gewünschte Breite.
- Gewünschte Höhe.

Die gewünschte Größe MUSS anhand der gültigen Layoutbedingungen berechnet werden.

Measure DARF keine direkten Zeichenoperationen ausführen.

## 5. Arrange

Die Arrange-Phase bestimmt die endgültige Geometrie eines UI-Elements.

Eingabe:

- Zugewiesener Layoutbereich.
- Ergebnis der Measure-Phase.
- Ausrichtung.
- Abstände.
- Layoutregeln des übergeordneten Containers.

Ergebnis:

- Endgültige Position.
- Tatsächliche Breite.
- Tatsächliche Höhe.

Die Arrange-Phase MUSS die gültigen Größenbeschränkungen berücksichtigen.

Die tatsächliche Größe DARF von der gewünschten Größe abweichen.

## 6. Größenbeschränkungen

Nova UI unterstützt folgende Größenbedingungen:

- Feste Breite und Höhe.
- Automatische Größen.
- Mindestbreite und Mindesthöhe.
- Maximalbreite und Maximalhöhe.
- Verfügbarer Layoutbereich.
- Innen- und Außenabstände.

Die Auswertung MUSS eindeutig definiert sein.

Ungültige Werte wie negative Größen oder NaN MÜSSEN kontrolliert behandelt werden.

Unbegrenzte Größenbedingungen DÜRFEN verwendet werden, sofern der jeweilige Layoutcontainer sie unterstützt.

## 7. Hierarchische Berechnung

Die Layoutberechnung erfolgt entlang der UI-Hierarchie.

Dabei gilt:

- Übergeordnete Elemente übergeben Größenbedingungen an ihre Kinder.
- Untergeordnete Elemente melden ihren Platzbedarf.
- Übergeordnete Elemente berechnen daraus ihre gewünschte Größe.
- In der Arrange-Phase werden endgültige Bereiche zugewiesen.

Layoutcontainer DÜRFEN eigene Anordnungsregeln implementieren.

Die grundlegenden Measure- und Arrange-Verträge MÜSSEN erhalten bleiben.

## 8. Layout Invalidation

Nova UI unterscheidet zwei grundlegende Layoutinvalidierungen:

### InvalidateMeasure

Wird ausgelöst, wenn sich der benötigte Platz eines Elements ändern kann.

Eine Measure-Invalidierung macht auch abhängige Arrange-Ergebnisse ungültig.

### InvalidateArrange

Wird ausgelöst, wenn sich die Position oder endgültige Größe ändern kann, ohne dass der Platzbedarf neu ermittelt werden muss.

Invalidierungen MÜSSEN nur entlang tatsächlich betroffener Abhängigkeiten weitergegeben werden.

Mehrere Invalidierungen SOLLEN zusammengefasst werden.

## 9. Layout Caching

Measure- und Arrange-Ergebnisse DÜRFEN zwischengespeichert werden.

Ein Cacheeintrag bleibt gültig, solange:

- Die relevanten Eigenschaften unverändert sind.
- Die verfügbaren Größenbedingungen unverändert sind.
- Die abhängigen Kindgrößen gültig bleiben.
- Keine relevante Layoutinvalidierung vorliegt.

Gültige Berechnungsergebnisse SOLLEN wiederverwendet werden.

Eine reine Farbänderung DARF keine Measure- oder Arrange-Invalidierung verursachen.

## 10. Layoutabhängigkeiten

Die Layout Engine verwaltet Abhängigkeiten zwischen Elementen.

Beispiele:

- Inhaltsänderungen beeinflussen automatische Größen.
- Größenänderungen eines Containers beeinflussen seine Kinder.
- Änderungen an Margin oder Padding beeinflussen die Layoutgeometrie.
- Änderungen an der Schriftgröße können Textmaße verändern.
- Änderungen der Sichtbarkeit können den Platzbedarf beeinflussen.

Abhängigkeiten MÜSSEN gezielt ausgewertet werden.

Nicht betroffene Layoutbereiche DÜRFEN keine unnötigen Neuberechnungen verursachen.

## 11. Dynamische Layoutänderungen

Layoutänderungen können während der Laufzeit auftreten.

Dabei gilt:

- Änderungen werden über die Nova UI Runtime koordiniert.
- Zusammengehörige Änderungen SOLLEN transaktional verarbeitet werden.
- Layoutberechnungen verwenden einen konsistenten UI-Zustand.
- Veraltete Berechnungsergebnisse DÜRFEN neuere Zustände nicht überschreiben.

Mehrere Änderungen innerhalb eines Aktualisierungszyklus SOLLEN zu einem gemeinsamen Layoutdurchlauf zusammengefasst werden.

## 12. Layoutstabilität

Die Layout Engine MUSS instabile Berechnungen kontrolliert behandeln.

Dazu gehören:

- Zyklische Größenabhängigkeiten.
- Wiederholte gegenseitige Invalidierungen.
- Nicht konvergierende Layoutberechnungen.
- Übermäßig tiefe Elementhierarchien.

Layoutdurchläufe MÜSSEN begrenzt und diagnostizierbar sein.

Eine fehlerhafte Komponente DARF keine unbegrenzte Layoutschleife verursachen.

## 13. Koordinaten und Skalierung

Measure und Arrange verwenden logische UI-Koordinaten.

Dabei gilt:

- Layoutberechnungen sind unabhängig von der physischen Pixelauflösung.
- DPI-Skalierung wird über definierte Ausgabeparameter berücksichtigt.
- Rundungsregeln MÜSSEN reproduzierbar sein.
- Unterschiedliche Monitorskalierungen MÜSSEN unterstützt werden können.

Die endgültige Pixelzuordnung erfolgt über die zuständige Grafikarchitektur.

## 14. Rendering und Damage Tracking

Measure und Arrange erzeugen ausschließlich Layoutinformationen.

Die grafische Darstellung erfolgt anschließend über die Nova UI Renderer Abstraction.

Dabei gilt:

- Layoutänderungen erzeugen erforderliche Renderinvalidierungen.
- Positionsänderungen berücksichtigen bisherige und neue Darstellungsbereiche.
- Unveränderte Elemente werden nicht unnötig neu gezeichnet.
- Damage Tracking erfolgt in Zusammenarbeit mit dem Compositor.

Ein gültiges Layout allein MUSS keinen Renderdurchlauf auslösen.

## 15. Threading und Determinismus

Layoutberechnungen erfolgen auf Grundlage eines konsistenten UI-Zustands.

Dabei gilt:

- UI-Zustandsänderungen werden über den zuständigen UI-Ausführungskontext koordiniert.
- Parallele Berechnungen sind zulässig, sofern ihre Abhängigkeiten dies erlauben.
- Ergebnisse werden kontrolliert übernommen.
- Identische Eingaben MÜSSEN identische Layoutresultate erzeugen.

Die Layout Engine DARF die UI Event Loop nicht dauerhaft blockieren.

## 16. Normative Anforderungen

- **UI-001:** NovaOS MUSS ein einheitliches Measure- und Arrange-Verfahren bereitstellen.
- **UI-002:** Jedes layoutfähige UI-Element MUSS beide Operationen unterstützen.
- **UI-003:** Measure MUSS die gewünschte Größe eines Elements bestimmen.
- **UI-004:** Arrange MUSS die endgültige Position und Größe bestimmen.
- **UI-005:** Layoutberechnungen MÜSSEN definierte Größenbeschränkungen berücksichtigen.
- **UI-006:** Measure und Arrange MÜSSEN unabhängig vom Renderbackend funktionieren.
- **UI-007:** Layoutinvalidierungen MÜSSEN gezielt verarbeitet werden.
- **UI-008:** Measure-Invalidierungen MÜSSEN abhängige Arrange-Ergebnisse ungültig machen.
- **UI-009:** Gültige Layoutberechnungen MÜSSEN wiederverwendet werden können.
- **UI-010:** Unveränderte Layoutbereiche DÜRFEN keine unnötigen Neuberechnungen verursachen.
- **UI-011:** Dynamische Änderungen MÜSSEN konsistent verarbeitet werden.
- **UI-012:** Nicht konvergierende Layoutberechnungen MÜSSEN erkannt und begrenzt werden.
- **UI-013:** Layoutberechnungen MÜSSEN logische UI-Koordinaten verwenden.
- **UI-014:** Identische Layoutbedingungen MÜSSEN reproduzierbare Ergebnisse erzeugen.
- **UI-015:** Layoutänderungen MÜSSEN mit dem Damage Tracking zusammenarbeiten.
- **UI-016:** Fehler einzelner Layoutkomponenten DÜRFEN andere UI-Instanzen nicht beeinträchtigen.
- **UI-017:** Nova Layout Measurement and Arrangement MUSS vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 17. Integration

- **Nova Layout Engine:** Zentrale Layoutverwaltung.
- **Nova Grid Layout:** Zeilen- und Spaltenberechnung.
- **Nova Stack Layout:** Lineare Größen- und Positionsberechnung.
- **Nova Dock Layout:** Berechnung angedockter Bereiche.
- **Nova Adaptive Layout:** Dynamische Layoutbedingungen.
- **Nova UI Runtime:** Koordination der Aktualisierungen.
- **Nova UI Property System:** Layoutrelevante Eigenschaften.
- **Nova UI Component Lifecycle:** Lebenszyklus der Layoutelemente.
- **Nova UI Threading Model:** Konsistente Berechnung.
- **Nova UI Renderer Abstraction:** Verarbeitung der Layoutgeometrie.
- **Nova Damage Tracking:** Gezielte grafische Aktualisierung.

## 18. Ergebnis

NovaOS erhält ein einheitliches, deterministisches und effizientes Verfahren zur Größen- und Positionsberechnung nativer UI-Elemente.

Alle Layoutcontainer verwenden dieselben grundlegenden Measure- und Arrange-Verträge.

Durch gezielte Invalidierungen, Abhängigkeitsverfolgung und Caching werden ausschließlich erforderliche Layoutberechnungen ausgeführt.

**Grundsatz:** Measure bestimmt, wie viel Platz ein Element benötigt. Arrange bestimmt, welchen Platz es tatsächlich erhält. Bereits gültige Ergebnisse werden nicht unnötig neu berechnet.