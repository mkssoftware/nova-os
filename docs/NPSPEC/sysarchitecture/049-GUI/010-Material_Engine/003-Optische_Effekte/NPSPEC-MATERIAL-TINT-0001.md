# NPSPEC-MATERIAL-TINT-0001 – Nova Material Tinting

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Material Engine

## 1. Zweck

Nova Material Tinting definiert die Farbüberlagerung und farbliche Anpassung visueller Oberflächenmaterialien innerhalb von NovaOS.

Der Effekt ermöglicht die gezielte Veränderung der Materialfarbe, ohne die zugrunde liegenden Materialeigenschaften oder Hintergrundinhalte dauerhaft zu verändern.

Ziel ist eine einheitliche, dynamische und ressourcenschonende Farbgestaltung sämtlicher NovaOS-Materialien.

## 2. Architektur

Nova Material Tinting ist ein Effekt der Nova Material Engine und verwendet die bestehende Material Rendering Pipeline.

Die Zuständigkeiten sind:

- **Material System:** Definition der Farbeigenschaften.
- **Material Renderer:** Berechnung und Anwendung der Farbüberlagerung.
- **Graphics Engine:** Ausführung der Farboperationen.
- **Compositor:** Integration in die endgültige Bildkomposition.
- **Nova Theme System:** Bereitstellung systemweiter Farbwerte.

Material Tinting implementiert keinen eigenständigen Renderer.

## 3. Eigenschaften

Der Effekt unterstützt folgende Eigenschaften:

- **TintColor:** Farbe der Materialüberlagerung.
- **TintOpacity:** Deckkraft der Farbüberlagerung.
- **TintMode:** Verwendetes Mischverfahren.
- **TintIntensity:** Stärke der Farbwirkung.
- **Brightness:** Helligkeitsanpassung.
- **Saturation:** Farbsättigung.
- **Contrast:** Kontrastanpassung.
- **HueShift:** Verschiebung des Farbtons.
- **TintGradient:** Optionaler Farbverlauf.
- **AdaptiveTint:** Aktivierung der automatischen Farbanpassung.

Alle Eigenschaften müssen definierte Datentypen, Wertebereiche und Standardwerte besitzen.

Eine TintOpacity von 0 deaktiviert die Farbüberlagerung.

## 4. Farbmodell

Nova Material Tinting verwendet das gemeinsame Farbmodell der Nova Graphics Engine.

Verbindliche Anforderungen:

- Farbwerte müssen mit dem NovaOS Color Management kompatibel sein.
- Farbüberlagerungen dürfen die ursprüngliche Materialdefinition nicht verändern.
- Die Farbverarbeitung muss Transparenz und Alpha-Komposition berücksichtigen.
- Farbmischungen erfolgen im jeweils definierten Farbraum.
- Die Verarbeitung muss mit SDR- und HDR-Ausgaben kompatibel sein.
- Farboperationen müssen unabhängig von konkreten Grafiktreibern definiert werden.

## 5. Mischverfahren

Nova Material Tinting unterstützt folgende Mischverfahren:

- **Normal:** Standardmäßige Alpha-Farbüberlagerung.
- **Multiply:** Multiplikative Farbmischung.
- **Screen:** Aufhellende Farbmischung.
- **Overlay:** Kontrastabhängige Farbmischung.
- **Additive:** Additive Farbüberlagerung.

Weitere Mischverfahren können ergänzt werden.

Die Mischverfahren müssen mit der bestehenden Material Rendering Pipeline kompatibel sein.

## 6. Farbverläufe

Material Tinting unterstützt optionale Farbverläufe.

Anforderungen:

- Lineare und radiale Farbverläufe werden unterstützt.
- Farbverläufe können mehrere Farbstopps besitzen.
- Positionen und Farben der Farbstopps müssen konfigurierbar sein.
- Farbverläufe müssen mit Materialmasken und Clipping kompatibel sein.
- Farbverläufe können animiert werden.
- Statische Farbverläufe dürfen zwischengespeichert werden.

## 7. Adaptive Farbüberlagerung

Materialfarben können abhängig von Hintergrundinformationen und Systemeinstellungen angepasst werden.

Anforderungen:

- Hintergrundhelligkeit und Kontrast können berücksichtigt werden.
- Die Hintergrundanalyse erfolgt ausschließlich über freigegebene Compositor-Schnittstellen.
- Automatische Anpassungen dürfen definierte Farbgrenzen nicht überschreiten.
- Systemweite Kontrastvorgaben besitzen Vorrang.
- Unveränderte Analyseergebnisse dürfen wiederverwendet werden.
- Adaptive Farbänderungen müssen deaktivierbar sein.

## 8. Dynamisches Verhalten

Farbeigenschaften können zur Laufzeit verändert werden.

Unterstützt werden:

- Farbwechsel
- Helligkeitsübergänge
- Änderungen der Farbsättigung
- Animierte Farbverläufe
- Interaktionsabhängige Farbänderungen
- Übergänge zwischen Materialzuständen
- Anpassungen an das aktive Systemdesign

Animationen verwenden das bestehende NovaOS-Animationssystem.

Ohne relevante Änderungen darf keine kontinuierliche Neuberechnung erfolgen.

## 9. Materialintegration

Nova Material Tinting kann von sämtlichen Standardmaterialien verwendet werden.

- **Acrylic:** Transparente Farbüberlagerungen und dezente Farbanpassungen.
- **Liquid Glass:** Dynamische Glastönung.
- **Frosted Glass:** Milchglasfärbung und Helligkeitsanpassung.
- **Crystal:** Transparente Kristallfärbung.
- **Solid:** Vollständige Oberflächenfärbung und Farbverläufe.

Die Farbüberlagerung erfolgt über das gemeinsame Materialmodell.

## 10. Theme-Integration

Nova Material Tinting muss mit dem Nova Theme System zusammenarbeiten.

Verbindliche Anforderungen:

- Materialien können systemweite Akzentfarben verwenden.
- Farbwerte können durch Themes überschrieben werden.
- Materialinstanzen dürfen individuelle Farbwerte besitzen.
- Theme-Wechsel müssen ohne Neuerstellung der UI-Elemente möglich sein.
- Änderungen systemweiter Farben invalidieren ausschließlich betroffene Materialbereiche.
- Benutzerdefinierte Farbeinstellungen müssen berücksichtigt werden.

## 11. Damage Tracking

Nova Material Tinting muss mit dem Damage Tracking des Nova Compositors kompatibel sein.

Anforderungen:

- Farbänderungen invalidieren betroffene Materialbereiche.
- Änderungen animierter Farbverläufe erzeugen gezielte Damage-Regionen.
- Unveränderte Farbergebnisse dürfen wiederverwendet werden.
- Unsichtbare Materialbereiche werden nicht verarbeitet.
- Änderungen systemweiter Farbwerte dürfen keine unnötigen vollständigen Bildschirmaktualisierungen verursachen.

## 12. Performance

Material Tinting muss ressourcenschonend arbeiten.

Verbindliche Anforderungen:

- Einfache Farbüberlagerungen werden nach Möglichkeit direkt während der Materialkomposition ausgeführt.
- Zusätzliche Rendering-Durchläufe sollen vermieden werden.
- Kompatible Farboperationen können zusammengefasst werden.
- Statische Farbverläufe dürfen zwischengespeichert werden.
- GPU-Beschleunigung wird bevorzugt.
- CPU-basierte Farbverarbeitung muss für grundlegende Funktionen verfügbar sein.
- Unnötige temporäre Rendering-Buffer werden vermieden.

## 13. Barrierefreiheit

Nova Material Tinting muss systemweite Einstellungen zur Barrierefreiheit berücksichtigen.

Anforderungen:

- Farbüberlagerungen dürfen die Lesbarkeit nicht beeinträchtigen.
- Systemweite Kontrastvorgaben müssen berücksichtigt werden.
- Benutzerdefinierte Farbprofile müssen unterstützt werden.
- Dynamische Farbwechsel müssen reduzierbar oder deaktivierbar sein.
- Farben dürfen nicht die einzige Möglichkeit zur Unterscheidung wichtiger UI-Zustände darstellen.
- Bei unzureichendem Kontrast muss eine geeignete Ersatzdarstellung möglich sein.

## 14. Determinismus und Sicherheit

Nova Material Tinting muss bei identischen Eingaben und Rendering-Bedingungen reproduzierbare Ergebnisse innerhalb definierter numerischer Toleranzen liefern.

Anforderungen:

- Farbparameter müssen validiert und begrenzt werden.
- Ungültige Farbwerte müssen kontrolliert behandelt werden.
- Farbberechnungen dürfen keine Sicherheitsgrenzen des Compositors umgehen.
- Hintergrundabhängige Farbanpassungen dürfen keine geschützten Bildinformationen offenlegen.
- Fehlerhafte Farboperationen dürfen die Rendering-Pipeline nicht destabilisieren.
- Bei Berechnungsfehlern wird eine definierte Standardfarbe verwendet.

## 15. Ergebnis

Nova Material Tinting stellt eine einheitliche Farbüberlagerung und Farbanpassung innerhalb der Nova Material Engine bereit.

Durch unterschiedliche Mischverfahren, Farbverläufe, adaptive Farbanpassungen und Theme-Integration können sämtliche NovaOS-Materialien individuell gestaltet werden.

Die Integration in die bestehende Material Rendering Pipeline ermöglicht eine konsistente, dynamische und ressourcenschonende Farbverarbeitung.