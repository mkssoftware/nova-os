# NPSPEC-MATERIAL-INTERACTION-0001 – Nova Interactive Material Response

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Material Engine

## 1. Zweck

Nova Interactive Material Response definiert die visuelle Reaktion von Materialoberflächen auf Benutzerinteraktionen innerhalb von NovaOS.

Das System ermöglicht dynamische Änderungen von Transparenz, Beleuchtung, Reflexionen, Glanzlichtern und Oberflächenverformungen.

Ziel ist eine natürliche, unmittelbare und ressourcenschonende Rückmeldung auf Benutzeraktionen, ohne die Bedienbarkeit oder das UI-Layout zu beeinträchtigen.

## 2. Architektur

Nova Interactive Material Response ist Bestandteil der Nova Material Engine und verwendet die bestehenden Ereignis- und Animationssysteme.

Die Zuständigkeiten sind:

- **Nova UI Event System:** Bereitstellung von Eingabe- und Interaktionsereignissen.
- **Material System:** Definition der interaktionsabhängigen Materialeigenschaften.
- **Interactive Material Response:** Zuordnung von Ereignissen zu Materialreaktionen.
- **Animation System:** Zeitsteuerung und Interpolation.
- **Material Renderer:** Darstellung des resultierenden Materialzustands.
- **Compositor:** Komposition und Damage Tracking.

Interactive Material Response implementiert keine eigenständige Eingabeverarbeitung oder Animationslaufzeit.

## 3. Interaktionseigenschaften

Das System unterstützt folgende Eigenschaften:

- **InteractionEnabled:** Aktivierung interaktiver Materialreaktionen.
- **HoverResponse:** Reaktion auf Pointer-Hover.
- **PressResponse:** Reaktion auf Pointer-Press.
- **FocusResponse:** Reaktion auf Fokusänderungen.
- **DragResponse:** Reaktion auf Drag-Bewegungen.
- **InteractionIntensity:** Stärke der Materialreaktion.
- **InteractionRadius:** Räumlicher Wirkungsbereich.
- **InteractionPosition:** Position des Interaktionsimpulses.
- **ResponseDuration:** Dauer der Materialreaktion.
- **ResponseEasing:** Interpolationsverhalten.
- **RecoveryDuration:** Dauer der Rückkehr zum Ausgangszustand.

Alle Eigenschaften müssen definierte Datentypen, Wertebereiche und Standardwerte besitzen.

## 4. Interaktionsmodell

Materialreaktionen werden deklarativ durch die jeweilige Materialinstanz beschrieben.

Verbindliche Anforderungen:

- Interaktionen verändern ausschließlich den visuellen Laufzeitzustand.
- Gemeinsame Materialdefinitionen bleiben unverändert.
- Reaktionen können einzelne oder mehrere Materialeigenschaften beeinflussen.
- Unterschiedliche Interaktionszustände können individuelle Reaktionen besitzen.
- Materialreaktionen dürfen keine funktionalen UI-Ereignisse ersetzen.
- Die tatsächliche UI-Geometrie bleibt unverändert, sofern das UI-System keine Änderung anordnet.

## 5. Unterstützte Ereignisse

Nova Interactive Material Response unterstützt mindestens folgende Ereignisse:

- **PointerEnter:** Pointer betritt den Interaktionsbereich.
- **PointerLeave:** Pointer verlässt den Interaktionsbereich.
- **PointerMove:** Pointer bewegt sich innerhalb des Elements.
- **PointerPressed:** Pointer-Taste wird gedrückt.
- **PointerReleased:** Pointer-Taste wird losgelassen.
- **FocusGained:** Element erhält den Fokus.
- **FocusLost:** Element verliert den Fokus.
- **DragStarted:** Drag-Interaktion beginnt.
- **DragUpdated:** Drag-Position verändert sich.
- **DragCompleted:** Drag-Interaktion endet.
- **StateChanged:** Funktionaler UI-Zustand verändert sich.

Weitere Ereignisse können über das Nova UI Event System ergänzt werden.

## 6. Materialreaktionen

Interaktionsereignisse können folgende Effekte auslösen:

- Änderungen der Materialtransparenz
- Dynamische Lichtakzente
- Bewegliche Glanzlichter
- Änderungen der Reflexionsintensität
- Lokale Oberflächenverformungen
- Flüssige Wellenbewegungen
- Farb- und Helligkeitsänderungen
- Änderungen der Hintergrundunschärfe
- Übergänge zwischen Materialzuständen

Mehrere Effekte können innerhalb einer Materialreaktion kombiniert werden.

## 7. Positionsabhängige Reaktionen

Materialreaktionen können die Position einer Benutzerinteraktion berücksichtigen.

Anforderungen:

- Interaktionspositionen werden in ein definiertes lokales Materialkoordinatensystem überführt.
- Die Position kann Lichtquellen, Glanzlichter und Verformungsimpulse beeinflussen.
- Der Wirkungsbereich muss begrenzbar sein.
- Bewegungen können kontinuierliche Änderungen auslösen.
- Die Reaktion muss mit Materialmasken und Clipping kompatibel sein.
- Eingabekoordinaten dürfen nicht durch rein visuelle Materialverformungen verändert werden.

## 8. Zustandsverwaltung

Eine Materialinstanz kann mehrere Interaktionszustände besitzen.

Mindestens unterstützt werden:

- **Normal:** Regulärer Materialzustand.
- **Hovered:** Pointer befindet sich über dem Element.
- **Pressed:** Element wird gedrückt.
- **Focused:** Element besitzt den Fokus.
- **Dragging:** Aktive Drag-Interaktion.
- **Disabled:** Interaktionen sind deaktiviert.

Zustandskombinationen müssen eindeutig aufgelöst werden.

Der funktionale Zustand des UI-Elements besitzt Vorrang vor dekorativen Materialreaktionen.

## 9. Animationsintegration

Interaktionsabhängige Materialänderungen verwenden das bestehende NovaOS Animation System.

Verbindliche Anforderungen:

- Reaktionen können unmittelbar oder animiert erfolgen.
- Laufende Übergänge müssen unterbrechbar sein.
- Neue Ereignisse können bestehende Animationen ersetzen oder beeinflussen.
- Mehrere gleichzeitige Reaktionen müssen deterministisch kombiniert werden.
- Nach Ende einer Interaktion muss ein definierter Materialzustand erreicht werden.
- Abgeschlossene Reaktionen dürfen keine weiteren Animationen auslösen.

## 10. Materialintegration

Nova Interactive Material Response unterstützt sämtliche Standardmaterialien.

- **Acrylic:** Änderungen von Transparenz, Farbe und Beleuchtung.
- **Liquid Glass:** Flüssige Verformungen, Wellen und dynamische Lichtreaktionen.
- **Frosted Glass:** Änderungen von Unschärfe und Helligkeit.
- **Crystal:** Bewegliche Reflexionen und Glanzlichter.
- **Solid:** Farb-, Helligkeits- und Zustandsänderungen.

Die Verarbeitung erfolgt über das gemeinsame Materialmodell.

## 11. Damage Tracking

Interactive Material Response muss vollständig mit dem Damage Tracking des Nova Compositors kompatibel sein.

Verbindliche Anforderungen:

- Interaktionsereignisse invalidieren ausschließlich visuell betroffene Bereiche.
- Positionsabhängige Effekte berücksichtigen bisherige und neue Wirkungsbereiche.
- Änderungen abhängiger Materialeffekte müssen in die Damage-Berechnung einfließen.
- Unveränderte Materialbereiche dürfen nicht erneut verarbeitet werden.
- Unsichtbare Materialreaktionen dürfen keine unnötigen Rendering-Operationen auslösen.
- Nach Abschluss einer Reaktion dürfen keine weiteren interaktionsbedingten Bildaktualisierungen erfolgen.

Eine permanente vollständige Bildschirmaktualisierung ist nicht zulässig.

## 12. Performance

Interaktive Materialreaktionen müssen mit geringer Latenz und kontrolliertem Ressourcenverbrauch ausgeführt werden.

Anforderungen:

- Eingabeereignisse dürfen nicht durch aufwendige Materialberechnungen blockiert werden.
- Häufige Pointer-Bewegungen müssen effizient zusammengefasst werden können.
- Veraltete Zwischenzustände dürfen übersprungen werden.
- Gleichartige Materialreaktionen können gemeinsam verarbeitet werden.
- GPU-Beschleunigung wird bevorzugt.
- Die Berechnung bleibt auf relevante Materialbereiche begrenzt.
- Nicht sichtbare Effekte dürfen ausgesetzt werden.

Die funktionale Eingabeverarbeitung muss unabhängig von der Rendering-Leistung bleiben.

## 13. Qualitätsstufen und Fallback

Nova Interactive Material Response unterstützt folgende Qualitätsstufen:

1. **Full:** Vollständige dynamische Materialreaktionen einschließlich Verformung und Beleuchtung.
2. **Balanced:** Vereinfachte Effekte bei unverändertem Interaktionsverhalten.
3. **Reduced:** Grundlegende Farb-, Transparenz- und Helligkeitsänderungen.
4. **Fallback:** Unmittelbarer Wechsel zwischen statischen Materialzuständen.

Ein Qualitätswechsel darf die funktionale Bedienung nicht verändern.

## 14. Barrierefreiheit

Interactive Material Response muss systemweite Einstellungen zur Barrierefreiheit berücksichtigen.

Anforderungen:

- Materialreaktionen müssen reduzierbar oder deaktivierbar sein.
- Die Einstellung für reduzierte Bewegung muss berücksichtigt werden.
- Fokuszustände müssen unabhängig von dekorativen Effekten erkennbar bleiben.
- Interaktive Zustände dürfen nicht ausschließlich durch Farbe vermittelt werden.
- Tastatur-, Touch- und alternative Eingabeverfahren müssen gleichwertig unterstützt werden.
- Die Bedienbarkeit muss auch ohne Materialanimationen vollständig erhalten bleiben.

## 15. Determinismus und Sicherheit

Nova Interactive Material Response muss bei identischen Ereignisfolgen, Materialzuständen und Zeitwerten reproduzierbare Ergebnisse innerhalb definierter numerischer Toleranzen liefern.

Verbindliche Anforderungen:

- Interaktionsparameter müssen validiert und begrenzt werden.
- Ereignisse dürfen ausschließlich autorisierte Materialinstanzen beeinflussen.
- Materialreaktionen dürfen keine zusätzlichen Eingaberechte erzeugen.
- Die Anzahl gleichzeitig aktiver Reaktionen muss kontrollierbar sein.
- Fehlerhafte Effektparameter dürfen die Rendering-Pipeline nicht destabilisieren.
- Bei Berechnungsfehlern wird ein definierter statischer Materialzustand verwendet.

## 16. Ergebnis

Nova Interactive Material Response stellt eine einheitliche Verarbeitung interaktionsabhängiger Materialreaktionen innerhalb der Nova Material Engine bereit.

Durch die Verbindung von UI-Ereignissen, Materialeigenschaften und Animationen entstehen natürliche und unmittelbar reagierende Oberflächen.

Die Integration in die bestehende NovaOS-Architektur ermöglicht hochwertige Interaktionseffekte bei geringer Latenz und kontrolliertem Ressourcenverbrauch.