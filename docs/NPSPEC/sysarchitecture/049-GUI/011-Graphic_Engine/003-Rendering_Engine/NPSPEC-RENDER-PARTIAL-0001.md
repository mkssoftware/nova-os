# NPSPEC-RENDER-PARTIAL-0001 – Nova Partial Rendering

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Render Engine

## 1. Zweck

Nova Partial Rendering definiert die gezielte Neuberechnung veränderter Bildbereiche innerhalb grafischer Oberflächen.

Anstatt bei jeder Änderung den vollständigen Inhalt einer Oberfläche neu zu rendern, verarbeitet NovaOS ausschließlich die Bereiche, deren Darstellungsergebnis aktualisiert werden muss.

Ziel ist die Reduzierung von GPU- und CPU-Auslastung, Speicherbandbreite und Energieverbrauch bei unveränderter visueller Korrektheit.

## 2. Architekturprinzipien

- **Damage-Driven:** Rendering basiert auf ermittelten Damage Regions.
- **Incremental:** Unveränderte Bildbereiche bleiben erhalten.
- **Dependency-Aware:** Grafische Abhängigkeiten beeinflussen den Aktualisierungsumfang.
- **Region-Based:** Rendering-Operationen werden räumlich begrenzt.
- **Retained:** Gültige Darstellungsergebnisse werden wiederverwendet.
- **Adaptive:** Partial Rendering wird nur verwendet, wenn es technisch sinnvoll ist.
- **Deterministic:** Identische Zustände und Eingaben erzeugen identische logische Darstellungsergebnisse.
- **Fallback-Safe:** Full Rendering bleibt jederzeit als Alternative verfügbar.

## 3. Rendering-Modell

Partial Rendering verarbeitet eine Menge betroffener Bildbereiche innerhalb eines definierten Rendering-Ziels.

Ein Partial Rendering Request enthält mindestens:

- **Target:** Zieloberfläche oder Render Target.
- **State Version:** Zugehörige grafische Zustandsversion.
- **Damage Regions:** Zu aktualisierende Bildbereiche.
- **Render Dependencies:** Relevante grafische Abhängigkeiten.
- **Preservation State:** Gültigkeit unveränderter Bildbereiche.

Die Render Engine MUSS sicherstellen, dass das resultierende Bild dem korrekten vollständigen Rendering-Ergebnis entspricht.

## 4. Damage Regions

Nova Damage Tracking stellt die betroffenen Bildbereiche bereit.

Dabei werden insbesondere berücksichtigt:

- Änderungen grafischer Inhalte
- Geometrie- und Transformationsänderungen
- Sichtbarkeitsänderungen
- Überlagerungen
- Clipping
- Schatten und andere Effekte
- Transparenz und Backdrop-Abhängigkeiten

Damage Regions MÜSSEN sämtliche Pixel umfassen, deren endgültiges Darstellungsergebnis verändert sein kann.

## 5. Erhaltung unveränderter Bildbereiche

Partial Rendering setzt voraus, dass unveränderte Bildbereiche weiterhin gültig sind.

NovaOS MUSS:

- Die Gültigkeit vorhandener Buffer-Inhalte prüfen.
- Unveränderte Pixel erhalten.
- Veraltete oder undefinierte Buffer-Inhalte erkennen.
- Die Lebensdauer benötigter Render Targets sicherstellen.

Bei Verwendung mehrerer Buffer MUSS die Gültigkeit der erhaltenen Bildbereiche für jeden Buffer gesondert berücksichtigt werden.

Ein neuer oder vollständig ungültiger Buffer MUSS vor seiner Präsentation vollständig initialisiert oder korrekt rekonstruiert werden.

## 6. Räumliche Begrenzung

Die Render Engine SOLL Rendering-Operationen auf die erforderlichen Bereiche begrenzen.

Dafür können verwendet werden:

- Scissor Rectangles
- Clip Regions
- Region-Based Rendering
- Tile-Based Rendering
- Selektive Render Passes

Die Begrenzung MUSS Transformationen, Clipping und relevante Effektausdehnungen berücksichtigen.

## 7. Effektabhängigkeiten

Grafische Effekte können Pixel außerhalb der unmittelbar geänderten Geometrie beeinflussen.

Dies betrifft insbesondere:

- Blur
- Schatten
- Refraction
- Reflection
- Transparenz
- Liquid Glass
- Backdrop Sampling

NovaOS MUSS zwischen dem zu aktualisierenden Ausgabebereich und dem dafür benötigten Eingabebereich unterscheiden.

Ein Effekt DARF zusätzliche Bildbereiche lesen, ohne dass diese vollständig neu gerendert werden müssen.

Fehlende oder unvollständige Effektabhängigkeiten MÜSSEN durch konservative Erweiterung der Damage Regions ausgeglichen werden.

## 8. Partial Rendering Ablauf

Die Verarbeitung erfolgt grundsätzlich in folgenden Schritten:

1. Übernahme eines konsistenten grafischen Zustands.
2. Ermittlung relevanter Damage Regions.
3. Prüfung der Gültigkeit vorhandener Buffer-Inhalte.
4. Auswertung räumlicher und grafischer Abhängigkeiten.
5. Auswahl zwischen Partial Rendering und Full Rendering.
6. Erstellung erforderlicher Render Passes.
7. Begrenzte Ausführung der Rendering-Operationen.
8. Aktualisierung der Buffer-Gültigkeitsinformationen.
9. Synchronisierte Bereitstellung des Ergebnisses.

Leere Damage Regions SOLLEN keine Rendering-Ausführung auslösen.

## 9. Render Graph Integration

Der Nova Render Graph berücksichtigt die räumlichen Anforderungen des Partial Rendering.

Er MUSS:

- Betroffene Render Passes bestimmen können.
- Unnötige Passes vermeiden.
- Ressourcenabhängigkeiten erhalten.
- Erforderliche Eingabebereiche berücksichtigen.
- Die korrekte Ausführungsreihenfolge gewährleisten.

Render Passes DÜRFEN zusammengefasst werden, sofern die visuelle Korrektheit erhalten bleibt.

## 10. Adaptive Strategieauswahl

Partial Rendering ist nicht grundsätzlich effizienter als Full Rendering.

Nova Adaptive Graphics Processing DARF Full Rendering bevorzugen, wenn:

- Damage Regions einen großen Teil der Oberfläche betreffen.
- Die Anzahl einzelner Regionen zu hoch ist.
- Der Verwaltungsaufwand den Einsparungseffekt übersteigt.
- Vorhandene Buffer-Inhalte nicht gültig sind.
- Grafische Effekte eine nahezu vollständige Neuberechnung erfordern.
- Hardware oder Rendering-Backend keine effiziente Teilaktualisierung ermöglichen.

Die Auswahl SOLL auf definierten Kostenmodellen oder Messwerten basieren.

## 11. Buffer- und Ressourcenverwaltung

Partial Rendering MUSS mit der Nova GPU Resource Management Architecture zusammenarbeiten.

Dabei gilt:

- Benötigte Buffer-Inhalte MÜSSEN gültig bleiben.
- Lese- und Schreibzugriffe MÜSSEN korrekt synchronisiert werden.
- Render Targets DÜRFEN wiederverwendet werden.
- Unnötige Buffer-Kopien SOLLEN vermieden werden.
- Zero-Copy-Verfahren SOLLEN genutzt werden, sofern technisch möglich.

Die Verwendung von Partial Rendering DARF keine uninitialisierten Bildbereiche sichtbar machen.

## 12. Integration mit dem Compositor

Partial Rendering aktualisiert Inhalte einzelner Rendering-Ziele.

Der Nova Compositor entscheidet unabhängig davon, welche Bereiche des zusammengesetzten Bildes aktualisiert werden müssen.

Dabei gilt:

- **Render Engine:** Aktualisiert betroffene Inhalte einer Oberfläche.
- **Compositor:** Aktualisiert betroffene Bereiche der Gesamtkomposition.
- **Display Server:** Verwaltet die Präsentation der fertigen Buffer.

Partial Rendering bedeutet nicht automatisch Partial Presentation.

Die finale Bildschirmausgabe DARF weiterhin vollständige Buffer präsentieren.

## 13. Fehlerbehandlung

Bei ungültigen Damage Regions, fehlenden Buffer-Inhalten oder unvollständigen Abhängigkeiten MUSS NovaOS auf eine korrekte konservative Aktualisierung zurückfallen.

Ein fehlgeschlagenes Partial Rendering DARF keine unvollständigen oder beschädigten Bildinhalte veröffentlichen.

Der zuletzt gültige Darstellungszustand SOLL bis zur erfolgreichen Aktualisierung erhalten bleiben.

## 14. Verbindliche Anforderungen

1. NovaOS MUSS Partial Rendering unterstützen.
2. Partial Rendering MUSS auf gültigen Damage Regions basieren.
3. Unveränderte Bildbereiche SOLLEN erhalten bleiben.
4. Die Gültigkeit vorhandener Buffer-Inhalte MUSS überprüfbar sein.
5. Undefinierte oder veraltete Buffer-Inhalte DÜRFEN nicht als gültig behandelt werden.
6. Grafische Effektabhängigkeiten MÜSSEN berücksichtigt werden.
7. Rendering-Operationen SOLLEN räumlich begrenzt werden.
8. Leere Damage Regions SOLLEN keine Rendering-Ausführung auslösen.
9. Partial Rendering MUSS in den Nova Render Graph integrierbar sein.
10. Full Rendering MUSS als Fallback verfügbar bleiben.
11. Die Strategieauswahl SOLL den tatsächlichen Verarbeitungsaufwand berücksichtigen.
12. Das resultierende Bild MUSS visuell korrekt sein.
13. Rendering und Komposition MÜSSEN getrennte Verantwortlichkeiten behalten.
14. Die Implementierung MUSS unterschiedliche Rendering-Backends unterstützen.

## 15. Ergebnis

Nova Partial Rendering ermöglicht die gezielte Aktualisierung veränderter Bildbereiche, während unveränderte Darstellungsergebnisse erhalten bleiben.

Durch Damage Tracking, räumliche Begrenzung, Buffer-Wiederverwendung und abhängigkeitsbasierte Verarbeitung reduziert NovaOS unnötige Rendering-Operationen.

Damit bildet Partial Rendering eine zentrale Grundlage für eine leistungsfähige, energieeffiziente und reaktionsschnelle grafische Oberfläche.