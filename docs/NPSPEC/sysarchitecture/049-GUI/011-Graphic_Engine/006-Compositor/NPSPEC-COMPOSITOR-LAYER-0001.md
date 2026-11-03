# NPSPEC-COMPOSITOR-LAYER-0001 – Nova Composition Layer Management

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Compositor

## 1. Zweck

Nova Composition Layer Management definiert die Verwaltung, Anordnung und Verarbeitung grafischer Ebenen innerhalb des Nova Compositor.

Ziel ist eine effiziente, konsistente und inkrementelle Zusammensetzung von Fenstern, Systemoberflächen, Overlays und Materialeffekten.

Änderungen einzelner Layer SOLLEN ausschließlich die tatsächlich betroffenen Ausgabebereiche aktualisieren.

## 2. Architekturprinzipien

- **Layer-Based:** Grafische Inhalte werden in eigenständigen Ebenen organisiert.
- **Retained:** Layer-Zustände bleiben zwischen Frames erhalten.
- **Hierarchical:** Layer können hierarchisch angeordnet werden.
- **Damage-Driven:** Änderungen erzeugen gezielte Aktualisierungen.
- **Visibility-Aware:** Verdeckte Bereiche werden berücksichtigt.
- **Material-Aware:** Transparenz und Effektabhängigkeiten werden unterstützt.
- **Incremental:** Unveränderte Layer werden wiederverwendet.
- **Deterministic:** Identische Zustände erzeugen reproduzierbare Ergebnisse.

## 3. Layer Model

Ein Composition Layer beschreibt eine grafische Ebene innerhalb des Composition Tree.

Er enthält mindestens:

- **Layer ID:** Eindeutige Identifikation.
- **Surface Reference:** Optionale referenzierte Surface.
- **Parent Layer:** Übergeordnete Ebene.
- **Z-Order:** Reihenfolge innerhalb der Ebene.
- **Transform:** Position, Skalierung und Rotation.
- **Opacity:** Transparenzwert.
- **Clip Region:** Sichtbare Begrenzung.
- **Material Reference:** Optionale Materialeigenschaften.
- **Visibility State:** Aktueller Sichtbarkeitszustand.
- **State Version:** Version des Layer-Zustands.

Layer DÜRFEN ohne eigene Surface existieren und ausschließlich zur Gruppierung oder Transformation dienen.

## 4. Layer Hierarchy

NovaOS MUSS hierarchische Layer-Strukturen unterstützen.

Dabei gilt:

1. Untergeordnete Layer übernehmen relevante Transformationen ihrer Eltern.
2. Clipping kann durch übergeordnete Layer begrenzt werden.
3. Sichtbarkeitsänderungen können gesamte Teilbäume betreffen.
4. Gruppen-Opacity MUSS entsprechend der definierten Compositing-Semantik verarbeitet werden.
5. Änderungen eines Eltern-Layers MÜSSEN ihre Auswirkungen auf untergeordnete Layer berücksichtigen.

Hierarchische Gruppen DÜRFEN bei Bedarf in temporäre Zwischenflächen gerendert werden.

## 5. Layer Ordering

Die Darstellung erfolgt anhand einer eindeutig definierten Layer-Reihenfolge.

NovaOS MUSS unterstützen:

- Z-Order innerhalb einer Layer-Gruppe
- Übergeordnete Layer-Kategorien
- Hierarchische Reihenfolge
- Dynamische Änderungen der Layer-Position

Die Reihenfolge MUSS auch bei identischen Z-Werten eindeutig und deterministisch sein.

Der Window Manager bestimmt die zulässige Fensterreihenfolge.

Der Compositor setzt diese Vorgaben um.

## 6. Layer Categories

NovaOS SOLL folgende logische Layer-Kategorien unterstützen:

| Kategorie | Verwendung |
|---|---|
| Desktop | Desktop-Hintergrund und Desktop-Elemente |
| Application | Normale Anwendungsfenster |
| System | Taskbar, Startmenü und Systemoberflächen |
| Popup | Kontextmenüs, Tooltips und Popups |
| Overlay | System-Overlays und besondere Einblendungen |
| Cursor | Mauszeiger und vergleichbare Eingabeelemente |

Die Kategorien definieren keine unveränderliche globale Z-Order.

Ihre Prioritäten und zulässigen Beziehungen werden durch Systemrichtlinien festgelegt.

## 7. Layer State Management

NovaOS MUSS den aktuellen Layer-Zustand persistent im Speicher verwalten.

Änderungen können betreffen:

- Surface-Zuordnung
- Position und Größe
- Transformation
- Z-Order
- Opacity
- Clipping
- Sichtbarkeit
- Materialeigenschaften

Änderungen MÜSSEN versionssicher veröffentlicht werden.

Der Compositor MUSS während einer Komposition einen konsistenten Layer-Zustand verwenden.

## 8. Incremental Layer Updates

NovaOS MUSS inkrementelle Layer-Aktualisierungen unterstützen.

Dabei gilt:

- Unveränderte Layer SOLLEN nicht erneut verarbeitet werden.
- Änderungen der Geometrie MÜSSEN alte und neue Ausgabebereiche berücksichtigen.
- Änderungen der Z-Order MÜSSEN betroffene Überlappungsbereiche invalidieren.
- Änderungen der Opacity MÜSSEN relevante Blending-Ergebnisse invalidieren.
- Änderungen an Gruppen MÜSSEN abhängige Unterebenen berücksichtigen.

Eine vollständige Neuberechnung des Composition Tree SOLL vermieden werden, solange eine korrekte inkrementelle Aktualisierung möglich ist.

## 9. Visibility und Occlusion

Der Compositor MUSS die Sichtbarkeit einzelner Layer bestimmen können.

Dabei werden berücksichtigt:

- Layer-Reihenfolge
- Geometrie und Transformation
- Clipping
- Opacity
- Alpha-Masken
- Materialabhängigkeiten

Vollständig verdeckte Layer-Bereiche SOLLEN nicht unnötig zusammengesetzt werden.

Transparente und backdrop-abhängige Layer DÜRFEN nicht ohne Prüfung ihrer Abhängigkeiten ausgeschlossen werden.

## 10. Layer Grouping

Mehrere Layer DÜRFEN zu einer Composition Group zusammengefasst werden.

Gruppen ermöglichen:

- Gemeinsame Transformationen
- Gemeinsames Clipping
- Gruppen-Opacity
- Gemeinsame Materialeffekte
- Zwischenspeicherung zusammengesetzter Inhalte

Eine Gruppe DARF als eigenständiges Rendering-Ergebnis zwischengespeichert werden.

Die Cache-Gültigkeit MUSS von den relevanten Zuständen ihrer untergeordneten Layer abhängen.

## 11. Material Integration

Composition Layers DÜRFEN Materialeffekte verwenden.

Unterstützt werden insbesondere:

- Acrylic
- Liquid Glass
- Frosted Glass
- Crystal
- Solid

Der Compositor MUSS erforderliche Backdrop- und Layer-Abhängigkeiten bereitstellen.

Materialeffekte DÜRFEN die Zusammensetzung zusätzlicher Zwischenflächen erfordern.

Die eigentliche Effektberechnung verbleibt bei Nova Material Engine und Render Engine.

## 12. Hardware Layer Integration

NovaOS SOLL geeignete Layer direkt über Hardwarefunktionen darstellen können.

Dazu gehören:

- Hardware Overlay Planes
- Direct Scanout
- Hardware Cursor Planes

Die Verwendung solcher Funktionen MUSS die Anforderungen an Format, Skalierung, Transparenz, Synchronisation und Z-Order erfüllen.

Nicht unterstützte Layer MÜSSEN weiterhin über die reguläre Komposition darstellbar sein.

## 13. Sicherheit und Isolation

Layer-Zugriffe MÜSSEN durch den Compositor kontrolliert werden.

Dabei gilt:

- Anwendungen dürfen ihre autorisierten Layer-Inhalte bereitstellen.
- System-Layer dürfen nicht unberechtigt verändert werden.
- Z-Order-Beschränkungen müssen durchgesetzt werden.
- Fremde Surface-Inhalte dürfen nicht unberechtigt gelesen werden.
- Ungültige Layer-Referenzen müssen abgewiesen werden.

Die Verwaltung privilegierter Layer MUSS durch vertrauenswürdige Systemkomponenten erfolgen.

## 14. Fehlerbehandlung

NovaOS MUSS ungültige Layer-Zustände erkennen können.

Dazu gehören:

- Ungültige Parent-Referenzen
- Zyklische Layer-Hierarchien
- Ungültige Transformationen
- Fehlende Surface-Referenzen
- Inkonsistente Zustandsversionen

Zyklische Hierarchien MÜSSEN verhindert werden.

Bei fehlerhaften Layer-Zuständen MUSS ein definierter Fallback erfolgen, ohne die Integrität des gesamten Composition Tree zu gefährden.

## 15. Verbindliche Anforderungen

1. NovaOS MUSS ein hierarchisches Composition Layer Management bereitstellen.
2. Layer MÜSSEN unabhängig von ihren Surface-Inhalten verwaltet werden können.
3. Transformationen, Z-Order, Opacity und Clipping MÜSSEN unterstützt werden.
4. Layer-Gruppen MÜSSEN möglich sein.
5. Die Layer-Reihenfolge MUSS eindeutig und deterministisch sein.
6. Änderungen MÜSSEN versionssicher verarbeitet werden.
7. Inkrementelle Layer-Aktualisierungen MÜSSEN unterstützt werden.
8. Geometrie- und Z-Order-Änderungen MÜSSEN korrekten Damage erzeugen.
9. Occlusion Culling SOLL unnötige Komposition vermeiden.
10. Material- und Backdrop-Abhängigkeiten MÜSSEN berücksichtigt werden.
11. Gültige Gruppen- und Layer-Caches SOLLEN wiederverwendet werden.
12. Hardware Overlays SOLLEN bei geeigneter Unterstützung verwendet werden.
13. System-Layer MÜSSEN gegen unberechtigte Änderungen geschützt werden.
14. Zyklische Layer-Hierarchien DÜRFEN nicht zugelassen werden.
15. Ein softwarebasierter Fallback MUSS verfügbar sein.

## 16. Ergebnis

Nova Composition Layer Management stellt die strukturierte Verwaltung aller grafischen Ebenen innerhalb des Nova Compositor bereit.

Durch hierarchische Layer, deterministische Reihenfolgen, inkrementelle Zustandsänderungen und materialbewusste Verarbeitung werden ausschließlich notwendige Kompositionsbereiche aktualisiert.

Damit bildet das Subsystem die Grundlage für eine flexible, effiziente und sichere Ebenenverwaltung der NovaOS-Oberfläche.