# NPSPEC-MATERIAL-ARCH-0001 – Nova Material Engine Architecture

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Material Engine

## 1. Zweck

Die Nova Material Engine stellt ein einheitliches System zur Darstellung visueller Oberflächenmaterialien innerhalb von NovaOS bereit.

Sie ermöglicht Transparenz, Unschärfe, Lichtbrechung, Reflexionen und dynamische Materialeffekte.

Materialien werden unabhängig von UI-Controls definiert und können systemweit verwendet werden.

## 2. Architektur

Die Nova Material Engine ist ein Subsystem der Nova Graphics Engine und arbeitet mit dem Compositor und dem nativen Nova UI-System zusammen.

Die Architektur besteht aus:

- **Material System:** Verwaltung und Definition von Materialien.
- **Material Renderer:** Umsetzung der Materialeigenschaften in Rendering-Operationen.
- **Effect Pipeline:** Verarbeitung optischer Materialeffekte.
- **Material Manager:** Verwaltung von Materialinstanzen und Ressourcen.
- **Material Cache:** Wiederverwendung berechneter Materialergebnisse.

Die Material Engine verwendet die vorhandene Rendering-Infrastruktur und implementiert keinen eigenständigen Grafikrenderer.

## 3. Materialtypen

NovaOS stellt folgende Standardmaterialien bereit:

| Material | Eigenschaften |
|----------|---------------|
| Acrylic | Transparenz, Blur und dezente Lichtreflexionen |
| Liquid Glass | Lichtbrechung, Reflexionen und dynamische Verformung |
| Frosted Glass | Milchglas mit ausgeprägter Hintergrundunschärfe |
| Crystal | Klares Glas mit intensiver Lichtbrechung |
| Solid | Undurchsichtige Oberfläche ohne aufwendige Effekte |

Alle Materialien verwenden ein gemeinsames Materialmodell.

Weitere Materialtypen können ergänzt werden.

## 4. Materialmodell

Ein Material beschreibt seine visuellen Eigenschaften deklarativ.

Unterstützte Eigenschaften umfassen:

- Grundfarbe und Farbton
- Transparenz und Deckkraft
- Hintergrundunschärfe
- Lichtbrechung und Verzerrung
- Reflexionen und Glanzlichter
- Oberflächenstruktur
- Beleuchtungsverhalten
- Animierbare Eigenschaften

Materialdefinitionen sind von konkreten Rendering-Implementierungen unabhängig.

## 5. Rendering

Die Material Engine verarbeitet Materialien innerhalb der bestehenden Grafikpipeline.

Dabei gelten folgende Anforderungen:

- Der Compositor stellt erforderliche Hintergrundinformationen bereit.
- Die Render Engine führt die grafischen Effekte aus.
- Materialeffekte werden ausschließlich innerhalb ihrer definierten Wirkungsbereiche angewendet.
- Unveränderte Bereiche werden nicht unnötig neu berechnet.
- Gemeinsame Effekte und Zwischenergebnisse können wiederverwendet werden.
- Materialoperationen müssen mit dem Damage Tracking des Compositors kompatibel sein.

Hintergrundabhängige Effekte müssen Änderungen der zugrunde liegenden Bildinhalte berücksichtigen.

## 6. Dynamische Materialien

Materialeigenschaften können zur Laufzeit verändert und animiert werden.

Unterstützt werden:

- Übergänge zwischen Materialzuständen
- Dynamische Lichtreflexionen
- Interaktionsabhängige Verformungen
- Änderungen von Transparenz und Unschärfe
- Materialwechsel ohne Neuerstellung des UI-Elements

Animationen verwenden das bestehende NovaOS-Animationssystem.

## 7. Integration

Die Material Engine stellt ihre Funktionen folgenden Subsystemen bereit:

- **Nova UI:** Verwendung von Materialien für Controls und Oberflächen.
- **Nova Window Manager:** Materialzuweisung für Fenster und Dekorationen.
- **Nova Compositor:** Hintergrundzugriff und Ebenenkomposition.
- **Nova Graphics Engine:** Ausführung der Rendering-Operationen.
- **Nova Theme System:** Zentrale Materialkonfiguration.

Materialien dürfen unabhängig vom verwendeten UI-Control eingesetzt werden.

## 8. Performance

Die Material Engine muss ressourcenschonend arbeiten.

Verbindliche Anforderungen:

- GPU-Beschleunigung verwenden, sofern verfügbar.
- CPU-Fallback für unterstützte Grundfunktionen bereitstellen.
- Unveränderte Materialbereiche nicht erneut berechnen.
- Materialressourcen und Zwischenergebnisse nach Möglichkeit gemeinsam nutzen.
- Rendering-Qualität abhängig von verfügbaren Ressourcen anpassen.
- Unsichtbare Materialien nicht rendern.
- Animationen ohne sichtbare Auswirkungen aussetzen.

Die Qualitätsreduzierung darf die grundlegende Bedienbarkeit nicht beeinträchtigen.

## 9. Erweiterbarkeit

Die Architektur muss zusätzliche Materialtypen und Rendering-Verfahren ermöglichen.

Neue Materialien können durch Kombination vorhandener Effekte oder durch zusätzliche Materialimplementierungen entstehen.

Materialdefinitionen dürfen keine direkten Abhängigkeiten zu bestimmten Grafiktreibern besitzen.

## 10. Ergebnis

Die Nova Material Engine bildet die zentrale Grundlage für einheitliche, dynamische und ressourcenschonende Oberflächenmaterialien in NovaOS.

Sie ermöglicht moderne Glas- und Materialeffekte, ohne die bestehende Grafikarchitektur zu duplizieren.

Alle Materialien werden über ein gemeinsames Modell definiert und durch die vorhandene Rendering-Infrastruktur dargestellt.