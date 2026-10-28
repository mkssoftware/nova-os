# NPSPEC-UI-ARCH-0004 – Nova UI Renderer Abstraction

**Status:** Angenommen  
**Bereich:** UI / Architecture / Rendering  
**System:** NovaOS

## 1. Zweck

Nova UI Renderer Abstraction definiert die einheitliche Schnittstelle zwischen dem nativen UI-System und der grafischen Darstellung.

Ziel ist die vollständige Trennung von UI-Logik, Layout und Rendering.

Nova UI soll unterschiedliche Renderbackends verwenden können, ohne dass UI-Definitionen, Steuerelemente oder Anwendungslogik angepasst werden müssen.

Die Architektur unterstützt zunächst die vorhandene NovaOS-Grafikbibliothek und später spezialisierte CPU- und GPU-Renderer.

## 2. Architektur

Die Renderer-Abstraktion umfasst folgende Komponenten:

| Komponente | Aufgabe |
|---|---|
| UI Renderer Interface | Einheitliche Schnittstelle für Renderbackends |
| Render Tree Builder | Überführung visueller UI-Elemente in eine renderbare Struktur |
| Render Command Generator | Erzeugung backendunabhängiger Zeichenoperationen |
| Render Resource Manager | Verwaltung grafischer Ressourcen |
| Render Backend Manager | Auswahl und Verwaltung des aktiven Renderbackends |
| Render Update Controller | Verarbeitung gezielter Darstellungsänderungen |

Die Renderer-Abstraktion ist Bestandteil von Nova UI und verwendet die Nova Graphics Architecture für die tatsächliche Darstellung.

## 3. Rendererunabhängiges Darstellungsmodell

Nova UI verwendet ein eigenes, backendunabhängiges Darstellungsmodell.

Dieses beschreibt:

- Geometrien und Zeichenoperationen.
- Farben und Brushes.
- Bilder und Text.
- Transformationen.
- Clipping und Masking.
- Transparenz und Effekte.
- Ebenen und Zeichenreihenfolge.
- Sichtbare und veränderte Bereiche.

Das Darstellungsmodell DARF keine spezifischen GPU- oder Grafiktreiberfunktionen voraussetzen.

Renderbackends MÜSSEN die unterstützten Operationen über definierte Schnittstellen umsetzen.

## 4. Render Tree

Der Render Tree bildet die für die Darstellung benötigte visuelle Struktur ab.

Dabei gilt:

- Der Render Tree wird aus dem Visual Tree abgeleitet.
- Nicht darzustellende Elemente können ausgeschlossen werden.
- Unveränderte Teilstrukturen SOLLEN wiederverwendet werden.
- Transformationen und Clipping werden berücksichtigt.
- Die Zeichenreihenfolge MUSS eindeutig definiert sein.

Änderungen am UI-Zustand DÜRFEN nur die betroffenen Bereiche des Render Trees aktualisieren.

Der Render Tree MUSS unabhängig vom verwendeten Backend bleiben.

## 5. Render Commands

Nova UI erzeugt backendunabhängige Render Commands.

Diese umfassen mindestens:

- Draw Rectangle.
- Draw Geometry.
- Draw Image.
- Draw Text.
- Apply Transform.
- Apply Clip.
- Apply Opacity.
- Begin Layer.
- End Layer.

Weitere Operationen DÜRFEN ergänzt werden.

Render Commands MÜSSEN eine eindeutig definierte Reihenfolge besitzen.

Das Backend entscheidet über die konkrete Umsetzung und Optimierung.

## 6. Renderbackends

Nova UI unterstützt austauschbare Renderbackends.

Vorgesehene Implementierungen:

| Backend | Beschreibung |
|---|---|
| Nova Graphics Backend | Verwendung der vorhandenen NovaOS-Grafikbibliothek |
| CPU Renderer | Softwarebasierte Darstellung |
| GPU Renderer | Hardwarebeschleunigte Darstellung |
| Specialized Renderer | Optimierte Darstellung für spezielle Geräte oder Anforderungen |

Ein Backend MUSS seine unterstützten Funktionen und Einschränkungen bereitstellen.

Fehlende Funktionen SOLLEN durch kompatible Fallbacks ersetzt werden können.

Die Auswahl eines Backends DARF keine Änderung der UI-Definition erfordern.

## 7. Backend-Auswahl und Wechsel

Der Render Backend Manager verwaltet das aktive Backend.

Die Auswahl kann anhand folgender Kriterien erfolgen:

- Verfügbare Grafikhardware.
- Unterstützte Renderfunktionen.
- Leistungsfähigkeit.
- Speicherbedarf.
- Energieverbrauch.
- Systemrichtlinien.

Ein Backendwechsel MUSS kontrolliert erfolgen.

Dabei SOLLEN vorhandene UI-Zustände und backendunabhängige Ressourcen erhalten bleiben.

Bei einem Backendfehler SOLL ein funktionsfähiger Fallback verwendet werden können.

## 8. Damage Tracking und Partial Redraw

Nova UI verwendet ein invalidierungsbasiertes Aktualisierungsmodell.

Dabei gilt:

- Änderungen erzeugen gezielte Invalidierungen.
- Unveränderte Bereiche werden nicht unnötig neu gezeichnet.
- Mehrere Invalidierungen DÜRFEN zusammengefasst werden.
- Der Renderer verarbeitet ausschließlich erforderliche Aktualisierungen, soweit das Backend dies unterstützt.
- Der Compositor erhält Informationen über tatsächlich veränderte Oberflächenbereiche.

Nova UI ist für die Ermittlung UI-bezogener Änderungen verantwortlich.

Die abschließende Zusammensetzung und Aktualisierung des Bildschirms erfolgt durch den Compositor.

Die Zusammenarbeit richtet sich nach **NPSPEC-COMPOSITOR-DAMAGE-0001 – Nova Damage Tracking and Partial Redraw**.

## 9. Render Scheduling

Renderoperationen werden bedarfsorientiert geplant.

Ein Renderdurchlauf wird beispielsweise erforderlich durch:

- Änderungen visueller Eigenschaften.
- Layoutänderungen.
- Animationen.
- Änderungen grafischer Ressourcen.
- Erforderliche Wiederherstellung einer Oberfläche.

Eine sichtbare, unveränderte Oberfläche DARF keine permanente Neuzeichnung verursachen.

Renderanforderungen SOLLEN zusammengefasst und mit der Bildausgabe synchronisiert werden.

Die UI Event Loop DARF nicht durch lang laufende Renderoperationen blockiert werden.

## 10. Ressourcenverwaltung

Die Renderer-Abstraktion verwaltet grafische Ressourcen über backendunabhängige Referenzen.

Dazu gehören:

- Bilder.
- Schriftressourcen.
- Brushes.
- Geometrien.
- Textlayouts.
- Render Targets.
- Zwischengespeicherte Darstellungen.

Backendabhängige Ressourcen werden durch das jeweilige Backend verwaltet.

Gemeinsam nutzbare Ressourcen SOLLEN wiederverwendet werden.

Nicht mehr benötigte Ressourcen MÜSSEN kontrolliert freigegeben werden.

Bei einem Backendwechsel MÜSSEN erforderliche Ressourcen wiederhergestellt werden können.

## 11. Skalierung und Ausgabe

Die Renderer-Abstraktion unterstützt unterschiedliche Ausgabeparameter.

Dazu gehören:

- Auflösung.
- DPI-Skalierung.
- Farbraum.
- Transparenz.
- Pixelformat.
- Ausrichtung.

UI-Koordinaten werden grundsätzlich in logischen Einheiten verarbeitet.

Die Umrechnung in physische Pixel erfolgt unter Berücksichtigung der Ausgabeparameter.

Änderungen der Skalierung DÜRFEN keine Anpassung der Anwendungslogik erfordern.

## 12. Fehlerbehandlung

Renderfehler MÜSSEN kontrolliert behandelt werden.

Dabei gilt:

- Fehlerhafte Render Commands DÜRFEN keine Speichergrenzen verletzen.
- Ungültige Ressourcenreferenzen MÜSSEN erkannt werden.
- Backendfehler DÜRFEN die UI-Objektstruktur nicht beschädigen.
- Fehlerhafte Renderoperationen SOLLEN isoliert behandelt werden.
- Ein funktionsfähiger Renderer SOLL nach Möglichkeit wiederhergestellt werden.

Diagnoseinformationen werden über die NovaOS-Diagnoseinfrastruktur bereitgestellt.

## 13. Sicherheit

Die Renderer-Abstraktion unterliegt Nova.Trust.

- Renderbackends MÜSSEN innerhalb ihrer autorisierten Berechtigungen arbeiten.
- Grafikressourcen DÜRFEN keine Prozess- oder Speicherisolation umgehen.
- Ressourcenfreigaben MÜSSEN kontrolliert erfolgen.
- Render Commands MÜSSEN vor der Verarbeitung auf Gültigkeit geprüft werden.
- Fehlerhafte oder manipulierte UI-Daten DÜRFEN keine unautorisierten Speicherzugriffe ermöglichen.

Die Darstellung eines UI-Elements DARF keine zusätzlichen Capabilities verleihen.

## 14. Normative Anforderungen

- **UI-001:** NovaOS MUSS eine einheitliche Renderer-Abstraktion bereitstellen.
- **UI-002:** UI-Logik und Rendering MÜSSEN voneinander getrennt sein.
- **UI-003:** Der Render Tree MUSS backendunabhängig definiert sein.
- **UI-004:** Render Commands MÜSSEN über eine einheitliche Schnittstelle verarbeitet werden.
- **UI-005:** Unterschiedliche Renderbackends MÜSSEN unterstützt werden können.
- **UI-006:** Ein Backendwechsel DARF keine Änderung der UI-Definition erfordern.
- **UI-007:** Renderbackends MÜSSEN ihre unterstützten Funktionen bereitstellen.
- **UI-008:** Fehlende Renderfunktionen SOLLEN durch geeignete Fallbacks ersetzt werden können.
- **UI-009:** Unveränderte UI-Bereiche DÜRFEN keine unnötigen Renderoperationen verursachen.
- **UI-010:** Damage Tracking und Partial Redraw MÜSSEN unterstützt werden.
- **UI-011:** Renderanforderungen MÜSSEN bedarfsorientiert geplant werden.
- **UI-012:** Die UI Event Loop DARF nicht dauerhaft durch Rendering blockiert werden.
- **UI-013:** Grafikressourcen MÜSSEN kontrolliert verwaltet und freigegeben werden.
- **UI-014:** Logische UI-Koordinaten MÜSSEN unabhängig von der physischen Ausgabeauflösung sein.
- **UI-015:** Backendfehler DÜRFEN die UI-Objektstruktur nicht beschädigen.
- **UI-016:** Die Renderer-Abstraktion MUSS die Sicherheitsregeln von Nova.Trust einhalten.
- **UI-017:** Nova UI Renderer Abstraction MUSS vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 15. Integration

- **Nova UI Architecture:** Übergeordnete UI-Architektur.
- **Nova UI Runtime:** Lebenszyklus und Aktualisierungsplanung.
- **Nova Declarative UI Model:** Beschreibung der Benutzeroberflächen.
- **Nova UI Layout Engine:** Positionierung und Größenberechnung.
- **Nova UI Property System:** Änderungsbenachrichtigungen.
- **Nova UI Control Framework:** Visuelle Steuerelemente.
- **Nova Graphics Architecture:** Grafische Grundfunktionen.
- **Nova 2D Rendering Engine:** Zeichenoperationen.
- **Nova Text Rendering Engine:** Textdarstellung.
- **Nova Graphics Backend Abstraction:** Hardware- und Softwarebackends.
- **Nova Compositor:** Zusammensetzung der Oberflächen.
- **Nova Damage Tracking:** Verwaltung veränderter Darstellungsbereiche.
- **Nova Display Server:** Bildschirmausgabe.
- **Nova.Trust:** Sicherheit und Isolation.

## 16. Ergebnis

NovaOS erhält eine eigenständige und austauschbare Rendering-Abstraktion für native Benutzeroberflächen.

Die UI bleibt unabhängig von der verwendeten Grafiktechnologie und kann sowohl über Software- als auch über hardwarebeschleunigte Renderer dargestellt werden.

Durch Render Trees, gezielte Invalidierungen und bedarfsorientierte Aktualisierungen werden unnötige Zeichenoperationen vermieden.

**Grundsatz:** Nova UI bestimmt, was dargestellt werden muss. Das Renderbackend entscheidet, wie es effizient gezeichnet wird. Der Compositor bestimmt, welche Änderungen auf dem Bildschirm sichtbar werden.