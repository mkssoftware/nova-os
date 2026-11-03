# NPSPEC-GRAPHICS-STATE-0001 – Nova State-Driven Graphics

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Graphics

## 1. Zweck

Nova State-Driven Graphics definiert die zustandsbasierte Grafikverarbeitung von NovaOS.

Anwendungen beschreiben ihren gewünschten grafischen Zustand, anstatt kontinuierlich Zeichenoperationen auszuführen.

NovaOS erkennt Zustandsänderungen und bestimmt eigenständig, welche grafischen Elemente, Effekte und Bildbereiche aktualisiert werden müssen.

Ziel ist die Vermeidung unnötiger Berechnungen bei gleichzeitiger Gewährleistung einer korrekten und konsistenten Darstellung.

## 2. Grundprinzipien

- **Persistent State:** Grafische Zustände bleiben zwischen Rendering-Zyklen erhalten.
- **Change-Driven:** Änderungen lösen gezielte Aktualisierungen aus.
- **Declarative:** Anwendungen definieren das gewünschte Erscheinungsbild.
- **Dependency-Aware:** Auswirkungen auf abhängige Elemente und Effekte werden berücksichtigt.
- **Transactional:** Zusammengehörige Änderungen werden atomar übernommen.
- **Incremental:** Nur notwendige Berechnungen werden durchgeführt.
- **Retained:** Unveränderte Strukturen und Ergebnisse werden wiederverwendet.
- **Adaptive:** NovaOS wählt die wirtschaftlichste zulässige Rendering-Strategie.

## 3. Grafisches Zustandsmodell

Jedes zustandsbasierte grafische Element besitzt eine eindeutige Identität und einen verwalteten Zustand.

Dieser kann umfassen:

- Geometrie und Position
- Sichtbarkeit und Transparenz
- Inhalt und Darstellung
- Material und Effekte
- Transformationen
- Stapelreihenfolge
- Interaktionszustände
- Abhängigkeiten zu anderen Elementen

Zustände werden im Nova Scene Graph verwaltet.

Änderungen erfolgen über definierte Schnittstellen und dürfen die Konsistenz des Scene Graph nicht verletzen.

## 4. Zustandsänderungen

Eine Zustandsänderung durchläuft folgende Verarbeitung:

1. Anwendung oder Systemkomponente verändert eine grafische Eigenschaft.
2. NovaOS erfasst die Änderung.
3. Die Änderung wird validiert und übernommen.
4. Betroffene Elemente und Abhängigkeiten werden ermittelt.
5. Erforderliche Damage Regions und Cache-Invalidierungen werden bestimmt.
6. Der Render Graph organisiert die notwendigen Operationen.
7. Die Render Engine aktualisiert die betroffenen Inhalte.
8. Der Compositor verarbeitet die aktualisierten Oberflächen.
9. Der Display Server präsentiert das Ergebnis.

Mehrere zusammengehörige Änderungen können innerhalb einer State Transaction gemeinsam übernommen werden.

## 5. Change Tracking

NovaOS MUSS Änderungen an verwalteten grafischen Zuständen erkennen.

Das Change Tracking unterscheidet insbesondere:

- Inhaltsänderungen
- Geometrieänderungen
- Layoutänderungen
- Materialänderungen
- Transformationsänderungen
- Sichtbarkeitsänderungen
- Änderungen grafischer Abhängigkeiten

Unveränderte Eigenschaften DÜRFEN keine unnötigen Aktualisierungen auslösen.

Mehrere Änderungen DÜRFEN zusammengefasst werden, sofern das endgültige Ergebnis erhalten bleibt.

## 6. Dependency Tracking

Grafische Elemente können voneinander abhängig sein.

NovaOS MUSS solche Abhängigkeiten bei der Änderungsverarbeitung berücksichtigen.

Beispiele sind:

- Layoutabhängigkeiten zwischen UI-Elementen
- Transparente Oberflächen über anderen Fenstern
- Hintergrundunschärfe und Lichtbrechung
- Schatten und Reflexionen
- Transformationen übergeordneter Elemente

Änderungen MÜSSEN an alle tatsächlich betroffenen Abhängigkeiten weitergegeben werden.

## 7. Incremental Rendering

Nach einer Zustandsänderung bestimmt NovaOS den erforderlichen Rendering-Aufwand.

Dabei SOLLEN:

- Unveränderte Elemente erhalten bleiben.
- Vorhandene Render-Ergebnisse wiederverwendet werden.
- Betroffene Bildbereiche gezielt aktualisiert werden.
- Unnötige Layout- und Effektberechnungen entfallen.
- Gültige Grafik-Caches erhalten bleiben.

Wenn eine vollständige Neuberechnung effizienter oder zur korrekten Darstellung erforderlich ist, DARF NovaOS Full Rendering verwenden.

## 8. State Transactions

Zusammengehörige grafische Zustandsänderungen MÜSSEN atomar übernehmbar sein.

Eine State Transaction gewährleistet:

- Konsistente Zustandsübergänge
- Keine Darstellung unvollständiger Zwischenzustände
- Gemeinsame Validierung zusammengehöriger Änderungen
- Zusammengefasste Änderungsverarbeitung
- Eindeutige Zustandsversionen

Fehlgeschlagene Transaktionen DÜRFEN keinen teilweise übernommenen grafischen Zustand hinterlassen.

## 9. Rendering und Frame Scheduling

Die Grafikverarbeitung erfolgt ereignisgesteuert.

Ein neuer Rendering-Zyklus wird nur ausgelöst, wenn eine relevante Änderung oder eine erforderliche zeitabhängige Aktualisierung vorliegt.

Dazu gehören:

- Zustandsänderungen
- Animationen
- Video- und Medieninhalte
- Änderungen von Anzeigeparametern
- Erforderliche Wiederherstellungen oder Neudarstellungen

Ohne relevante Änderungen SOLLEN keine neuen Frames berechnet werden.

Die physische Bildschirmausgabe kann unabhängig davon das zuletzt bereitgestellte Bild weiterhin anzeigen.

## 10. Integration externer Renderer

Anwendungen mit eigenen Rendering-Verfahren MÜSSEN über das Nova Surface Protocol integrierbar sein.

Sie können eigene Grafikbuffer bereitstellen und deren Aktualisierung signalisieren.

NovaOS verwaltet dabei:

- Surface-Zustände
- Buffer-Lebenszyklen
- Synchronisation
- Damage Regions
- Komposition und Präsentation

Die interne Zeichenlogik solcher Anwendungen muss nicht dem zustandsbasierten Nova Scene Graph entsprechen.

## 11. Verbindliche Anforderungen

1. Grafische Zustände MÜSSEN persistent verwaltbar sein.
2. Zustandsänderungen MÜSSEN eindeutig erfasst und verarbeitet werden.
3. Zusammengehörige Änderungen MÜSSEN atomar übernehmbar sein.
4. Abhängige Elemente und Effekte MÜSSEN korrekt aktualisiert werden.
5. Unveränderte Zustände SOLLEN keine erneute Berechnung verursachen.
6. Rendering MUSS bedarfsorientiert ausgelöst werden können.
7. Cache-Ergebnisse DÜRFEN ausschließlich bei gültigen Abhängigkeiten wiederverwendet werden.
8. Vollständiges Rendering MUSS als Fallback verfügbar sein.
9. Externe Renderer MÜSSEN über definierte Surface-Schnittstellen integrierbar sein.
10. Optimierungen DÜRFEN weder Zustandskonsistenz noch visuelle Korrektheit beeinträchtigen.

## 12. Ergebnis

Nova State-Driven Graphics bildet die Grundlage der zustandsbasierten Grafikverarbeitung von NovaOS.

Anwendungen definieren grafische Zustände, während NovaOS Änderungen, Abhängigkeiten, Rendering-Aufwand und Aktualisierungen eigenständig verwaltet.

Dadurch werden unnötige Berechnungen reduziert, grafische Ergebnisse wiederverwendet und konsistente, effiziente Darstellungen ermöglicht.