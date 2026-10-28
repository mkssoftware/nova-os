# NPSPEC-UI-LAYOUT-0001 – Nova Layout Engine

**Status:** Angenommen  
**Bereich:** UI / Layout  
**System:** NovaOS

## 1. Zweck

Nova Layout Engine definiert die Berechnung von Größe, Position und Anordnung nativer UI-Elemente innerhalb von NovaOS.

Ziel ist ein flexibles, deterministisches und ressourcenschonendes Layoutsystem, das unterschiedliche Bildschirmgrößen, Skalierungen und dynamische Benutzeroberflächen unterstützt.

Die Layout Engine arbeitet unabhängig vom verwendeten Renderbackend und bildet die Grundlage für sämtliche Nova UI Controls.

## 2. Architektur

Die Layout Engine umfasst folgende Komponenten:

| Komponente | Aufgabe |
|---|---|
| Layout Manager | Koordination der Layoutberechnung |
| Layout Context | Verwaltung der verfügbaren Layoutbedingungen |
| Measure Engine | Ermittlung benötigter Elementgrößen |
| Arrange Engine | Berechnung endgültiger Positionen und Größen |
| Layout Constraint Resolver | Verarbeitung von Größenbeschränkungen |
| Layout Invalidation Manager | Erkennung und Verwaltung notwendiger Neuberechnungen |
| Layout Cache | Wiederverwendung gültiger Berechnungsergebnisse |

Die Layout Engine verarbeitet die visuellen Elemente der Nova UI Runtime.

Sie erzeugt Layoutinformationen, jedoch keine direkten Zeichenoperationen.

## 3. Layoutmodell

Jedes layoutfähige UI-Element besitzt mindestens:

- Gewünschte Größe.
- Tatsächliche Größe.
- Position.
- Mindestgröße.
- Maximalgröße.
- Außenabstände.
- Ausrichtung.
- Sichtbarkeitszustand.
- Layoutabhängigkeiten.

Die Layoutberechnung erfolgt in logischen UI-Koordinaten.

Die Umrechnung in physische Pixel erfolgt durch die zuständigen Grafikkomponenten.

## 4. Measure- und Arrange-Verfahren

Nova UI verwendet ein zweistufiges Layoutverfahren.

### Measure

Die Measure-Phase ermittelt den benötigten Platz eines Elements anhand der verfügbaren Größe und seiner Layoutbedingungen.

Dabei werden die Größenanforderungen untergeordneter Elemente berücksichtigt.

### Arrange

Die Arrange-Phase bestimmt die endgültige Position und Größe innerhalb des zugewiesenen Bereichs.

Dabei gilt:

- Measure und Arrange MÜSSEN getrennte Verarbeitungsschritte sein.
- Layoutberechnungen MÜSSEN reproduzierbare Ergebnisse erzeugen.
- Unveränderte Elemente SOLLEN vorhandene Berechnungsergebnisse wiederverwenden.
- Eine erneute Measure-Phase DARF nur erfolgen, wenn sich relevante Größenabhängigkeiten verändert haben.

## 5. Layoutarten

Nova UI unterstützt mindestens folgende Layoutkonzepte:

| Layout | Beschreibung |
|---|---|
| Canvas | Absolute Positionierung |
| Stack | Horizontale oder vertikale Anordnung |
| Grid | Zeilen- und spaltenbasierte Anordnung |
| Dock | Ausrichtung an Containerseiten |
| Wrap | Automatischer Zeilen- oder Spaltenumbruch |
| Overlay | Überlagerung mehrerer Elemente |
| Flexible Layout | Verteilung verfügbaren Platzes |

Weitere Layoutcontainer DÜRFEN durch das Nova UI Control Framework ergänzt werden.

Alle Layoutcontainer verwenden dieselbe grundlegende Measure- und Arrange-Schnittstelle.

## 6. Größenberechnung

Die Layout Engine unterstützt:

- Feste Größen.
- Automatische Größen.
- Prozentuale Größen, sofern der Layoutkontext dies eindeutig definiert.
- Proportionale Größenverteilung.
- Mindest- und Maximalgrößen.
- Größenanpassung an Inhalte.
- Flexible Nutzung verfügbaren Platzes.

Größenbeschränkungen MÜSSEN konsistent ausgewertet werden.

Ungültige oder widersprüchliche Größenangaben MÜSSEN kontrolliert behandelt werden.

## 7. Ausrichtung und Abstände

UI-Elemente unterstützen:

- Horizontale Ausrichtung.
- Vertikale Ausrichtung.
- Innenabstände.
- Außenabstände.
- Elementabstände.
- Größenanpassung an den verfügbaren Bereich.

Die Layout Engine MUSS Abstände und Ausrichtungen unabhängig vom Renderbackend berechnen.

## 8. Verschachtelte Layouts

Layoutcontainer können beliebig verschachtelt werden, soweit Ressourcenlimits eingehalten werden.

Dabei gilt:

- Übergeordnete Container bestimmen den verfügbaren Bereich.
- Untergeordnete Elemente melden ihren Platzbedarf.
- Layoutabhängigkeiten werden hierarchisch ausgewertet.
- Änderungen werden nur entlang tatsächlich betroffener Abhängigkeiten weitergegeben.

Zyklische Layoutabhängigkeiten MÜSSEN erkannt und kontrolliert behandelt werden.

## 9. Layout Invalidation

Die Layout Engine verwendet ein invalidierungsbasiertes Berechnungsmodell.

Dabei wird zwischen folgenden Änderungen unterschieden:

- Measure Invalidation.
- Arrange Invalidation.
- Visual Invalidation.

Eine reine Darstellungsänderung DARF keine unnötige Layoutberechnung auslösen.

Mehrere Invalidierungen SOLLEN zusammengefasst werden.

Unveränderte Layoutbereiche MÜSSEN wiederverwendet werden können.

## 10. Layout Caching

Die Layout Engine unterstützt das Zwischenspeichern gültiger Berechnungsergebnisse.

Gespeichert werden können:

- Gewünschte Größen.
- Berechnete Positionen.
- Tatsächliche Größen.
- Layoutbedingungen.
- Abhängigkeitsinformationen.

Cacheeinträge MÜSSEN ungültig werden, sobald sich relevante Eingabewerte ändern.

Die Cacheverwaltung MUSS den Speicherverbrauch berücksichtigen.

## 11. Dynamische Layoutänderungen

Nova UI unterstützt Änderungen der Layoutstruktur zur Laufzeit.

Beispiele:

- Hinzufügen oder Entfernen von Elementen.
- Größenänderungen.
- Änderungen von Styles.
- Wechsel der Bildschirmauflösung.
- Änderungen der DPI-Skalierung.
- Dynamische Inhalte.

Änderungen MÜSSEN konsistent verarbeitet werden.

Zusammengehörige Änderungen SOLLEN transaktional angewendet werden können.

Die Layout Engine DARF unveränderte UI-Teilstrukturen nicht unnötig neu berechnen.

## 12. Skalierung und DPI

Die Layout Engine verwendet logische, geräteunabhängige Koordinaten.

Dabei gilt:

- Layoutberechnungen erfolgen unabhängig von der physischen Pixelauflösung.
- DPI-Änderungen werden über definierte Skalierungsparameter berücksichtigt.
- Text- und Elementgrößen müssen konsistent skaliert werden können.
- Unterschiedliche Monitorskalierungen MÜSSEN unterstützt werden können.

Die Umrechnung in physische Pixel erfolgt an der definierten Schnittstelle zur grafischen Darstellung.

## 13. Layout und Rendering

Die Layout Engine berechnet ausschließlich geometrische Informationen.

Der Renderer verwendet diese Informationen zur Darstellung.

Dabei gilt:

- Layout und Rendering bleiben getrennt.
- Layoutänderungen erzeugen gezielte Renderinvalidierungen.
- Unveränderte Elemente werden nicht unnötig neu gezeichnet.
- Änderungen an Position oder Größe MÜSSEN sowohl bisherige als auch neue betroffene Darstellungsbereiche berücksichtigen.

Die abschließende Bildschirmaktualisierung erfolgt über den Compositor und dessen Damage Tracking.

## 14. Threading und Determinismus

Layoutberechnungen erfolgen auf Grundlage eines konsistenten UI-Zustands.

Dabei gilt:

- Änderungen am UI-Objektmodell werden über den zuständigen UI-Ausführungskontext koordiniert.
- Parallele Layoutberechnungen DÜRFEN erfolgen, sofern keine veränderlichen Abhängigkeiten bestehen.
- Veraltete Berechnungsergebnisse DÜRFEN neuere Zustände nicht überschreiben.
- Identische Layoutbedingungen MÜSSEN identische Ergebnisse erzeugen.

Die Layout Engine DARF die UI Event Loop nicht durch unnötige Neuberechnungen dauerhaft blockieren.

## 15. Fehlerbehandlung

Die Layout Engine MUSS fehlerhafte Layoutbedingungen kontrolliert behandeln.

Dazu gehören:

- Ungültige Größen.
- Widersprüchliche Beschränkungen.
- Zyklische Abhängigkeiten.
- Übermäßig tiefe Hierarchien.
- Nicht berechenbare Layoutbedingungen.
- Ressourcenüberschreitungen.

Fehler einzelner Layoutcontainer DÜRFEN andere UI-Instanzen nicht beeinträchtigen.

Diagnoseinformationen SOLLEN über die NovaOS-Diagnoseinfrastruktur bereitgestellt werden.

## 16. Normative Anforderungen

- **UI-001:** NovaOS MUSS eine zentrale Layout Engine bereitstellen.
- **UI-002:** Die Layout Engine MUSS unabhängig vom Renderbackend funktionieren.
- **UI-003:** Layoutberechnungen MÜSSEN in logischen UI-Koordinaten erfolgen.
- **UI-004:** Measure und Arrange MÜSSEN als getrennte Verarbeitungsschritte unterstützt werden.
- **UI-005:** Unterschiedliche Layoutcontainer MÜSSEN über ein einheitliches Schnittstellenmodell arbeiten.
- **UI-006:** Feste, automatische und flexible Größen MÜSSEN unterstützt werden.
- **UI-007:** Mindest- und Maximalgrößen MÜSSEN berücksichtigt werden.
- **UI-008:** Verschachtelte Layouts MÜSSEN unterstützt werden.
- **UI-009:** Layoutänderungen MÜSSEN gezielte Invalidierungen auslösen.
- **UI-010:** Unveränderte Layoutbereiche MÜSSEN wiederverwendet werden können.
- **UI-011:** Layout Caching MUSS unterstützt werden.
- **UI-012:** Dynamische Änderungen MÜSSEN konsistent verarbeitet werden.
- **UI-013:** Unterschiedliche DPI-Skalierungen MÜSSEN unterstützt werden können.
- **UI-014:** Layout und Rendering MÜSSEN voneinander getrennt bleiben.
- **UI-015:** Layoutberechnungen MÜSSEN deterministische Ergebnisse erzeugen.
- **UI-016:** Fehlerhafte Layoutbedingungen MÜSSEN kontrolliert behandelt werden.
- **UI-017:** Die Layout Engine MUSS die Sicherheits- und Ressourcenlimits von NovaOS berücksichtigen.
- **UI-018:** Nova Layout Engine MUSS vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 17. Integration

- **Nova UI Architecture:** Übergeordnete UI-Architektur.
- **Nova UI Runtime:** Koordination der Layoutberechnung.
- **Nova Declarative UI Model:** Deklarative Layoutdefinitionen.
- **Nova UI Component Lifecycle:** Lebenszyklus der Layoutobjekte.
- **Nova UI Property System:** Layoutrelevante Eigenschaften.
- **Nova UI Control Framework:** Layoutcontainer und Steuerelemente.
- **Nova UI Renderer Abstraction:** Übergabe der Layoutinformationen.
- **Nova UI Threading Model:** Synchronisation und Ausführungsplanung.
- **Nova Graphics Architecture:** Koordinaten und Skalierung.
- **Nova Compositor:** Darstellung der berechneten Oberflächen.
- **Nova Damage Tracking:** Gezielte Aktualisierung veränderter Bereiche.
- **Nova Memory Management:** Layout Cache und Ressourcenverwaltung.

## 18. Ergebnis

NovaOS erhält eine native, flexible und deterministische Layout Engine.

Benutzeroberflächen können sich automatisch an unterschiedliche Bildschirmgrößen, Inhalte und Skalierungen anpassen.

Durch Measure und Arrange, gezielte Invalidierungen und Layout Caching werden ausschließlich notwendige Berechnungen durchgeführt.

**Grundsatz:** Nova Layout Engine berechnet nur das neu, was sich durch veränderte Layoutbedingungen tatsächlich ändern muss.