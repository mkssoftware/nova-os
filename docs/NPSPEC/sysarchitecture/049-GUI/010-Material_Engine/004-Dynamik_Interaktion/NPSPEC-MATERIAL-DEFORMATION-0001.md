# NPSPEC-MATERIAL-DEFORMATION-0001 – Nova Fluid Deformation

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Material Engine

## 1. Zweck

Nova Fluid Deformation definiert die dynamische Verformung virtueller Materialoberflächen innerhalb von NovaOS.

Das System ermöglicht flüssige, elastische und federnde Oberflächenbewegungen, insbesondere für Nova Liquid Glass.

Ziel ist eine natürliche, interaktive und ressourcenschonende Materialverformung ohne vollständige physikalische Flüssigkeitssimulation.

## 2. Architektur

Nova Fluid Deformation ist Bestandteil der Nova Material Engine und verwendet die bestehende Material Rendering Pipeline.

Die Zuständigkeiten sind:

- **Material System:** Definition der Verformungseigenschaften.
- **Fluid Deformation:** Berechnung des virtuellen Verformungszustands.
- **Material Renderer:** Darstellung der verformten Oberfläche.
- **Surface Distortion:** Optische Verzerrung der Hintergrundinhalte.
- **Animation System:** Zeitsteuerung und Zustandsübergänge.
- **Compositor:** Komposition und Damage Tracking.
- **Graphics Engine:** GPU- oder CPU-basierte Verarbeitung.

Fluid Deformation implementiert keinen eigenständigen Renderer oder eine separate Animationslaufzeit.

## 3. Eigenschaften

Der Effekt unterstützt folgende Eigenschaften:

- **Deformation:** Stärke der Oberflächenverformung.
- **Elasticity:** Elastizität des Materials.
- **Viscosity:** Dämpfung der Verformungsbewegung.
- **Stiffness:** Widerstand gegen Verformungen.
- **SurfaceTension:** Tendenz zur Wiederherstellung der ursprünglichen Form.
- **DeformationRadius:** Räumlicher Wirkungsbereich.
- **DeformationDirection:** Richtung der Verformung.
- **DeformationSpeed:** Geschwindigkeit der Verformung.
- **RecoverySpeed:** Geschwindigkeit der Rückkehr zum Ruhezustand.
- **MaxDeformation:** Maximal zulässige Verformung.
- **Quality:** Qualitätsstufe der Berechnung.

Alle Eigenschaften müssen definierte Datentypen, Wertebereiche und Standardwerte besitzen.

Eine Deformation von 0 deaktiviert den Effekt.

## 4. Verformungsmodell

Nova Fluid Deformation verwendet ein vereinfachtes, zweidimensionales Oberflächenmodell.

Verbindliche Anforderungen:

- Verformungen werden anhand virtueller Oberflächenpunkte oder mathematischer Funktionen berechnet.
- Lokale und globale Verformungen müssen unterstützt werden.
- Verformungen können sich über die Materialoberfläche ausbreiten.
- Elastizität und Dämpfung beeinflussen das Bewegungsverhalten.
- Die ursprüngliche Oberflächenform bleibt als Referenz erhalten.
- Verformungen müssen innerhalb definierter Grenzen bleiben.
- Eine vollständige physikalische Flüssigkeitssimulation ist nicht erforderlich.

## 5. Verformungsverfahren

Nova Fluid Deformation unterstützt folgende Verfahren:

- **Elastic:** Elastische Verformung mit Rückstellkraft.
- **Spring:** Federbasierte Oberflächenbewegung.
- **Wave:** Wellenförmige Verformung.
- **Ripple:** Kreisförmige Ausbreitung lokaler Impulse.
- **Drag:** Richtungsabhängige Verformung durch Bewegung.
- **Procedural:** Mathematisch definierte Verformungsmuster.

Mehrere Verfahren können kombiniert werden.

Die Kombination muss einen eindeutig definierten Verformungszustand ergeben.

## 6. Oberflächengeometrie

Fluid Deformation verändert die virtuelle Darstellungsgeometrie einer Materialoberfläche.

Anforderungen:

- Rechteckige, abgerundete und individuell geformte Materialoberflächen müssen unterstützt werden.
- Materialmasken und Clipping-Bereiche müssen berücksichtigt werden.
- Oberflächenkanten können unabhängig von inneren Bereichen verformt werden.
- Die Verformung darf keine unbeabsichtigten Änderungen am UI-Layout verursachen.
- Eingabebereiche bleiben unverändert, sofern das UI-System keine ausdrückliche Aktualisierung vorgibt.
- Die maximale visuelle Ausdehnung muss für Clipping und Damage Tracking bestimmbar sein.

## 7. Flüssigkeitsverhalten

Nova Fluid Deformation ermöglicht die Simulation flüssig wirkender Oberflächen.

Verbindliche Anforderungen:

- Bewegungsimpulse können lokale Verformungen auslösen.
- Verformungen können sich gedämpft über die Oberfläche ausbreiten.
- Die Oberfläche muss nach Wegfall äußerer Einflüsse in einen definierten Ruhezustand zurückkehren.
- Mehrere Impulse können gleichzeitig verarbeitet werden.
- Überlagerte Verformungen müssen stabil bleiben.
- Instabile oder unbegrenzte Schwingungen müssen verhindert werden.

Die Simulation darf keine dauerhaft aktive Berechnung benötigen, sobald der Ruhezustand erreicht ist.

## 8. Interaktionsabhängige Verformung

Verformungen können durch Benutzerinteraktionen ausgelöst werden.

Unterstützt werden:

- Pointer Enter und Pointer Leave
- Pointer-Bewegungen
- Pressed und Released
- Drag-Bewegungen
- Fokusänderungen
- Änderungen des Materialzustands

Die Ereignisse werden über das bestehende Nova UI Event System bereitgestellt.

Fluid Deformation implementiert keine eigenständige Eingabeverarbeitung.

## 9. Integration mit Materialeffekten

Fluid Deformation stellt den aktuellen Verformungszustand anderen Materialeffekten zur Verfügung.

Insbesondere:

- **Surface Distortion:** Berechnung optischer Hintergrundverzerrungen.
- **Light Refraction:** Anpassung der Lichtbrechung.
- **Surface Reflections:** Anpassung virtueller Reflexionen.
- **Specular Highlights:** Bewegung und Veränderung von Glanzlichtern.
- **Dynamic Lighting:** Beleuchtung anhand veränderter Oberflächennormalen.

Gemeinsame Verformungsdaten müssen wiederverwendbar sein.

Die einzelnen Effekte dürfen keine voneinander unabhängigen, widersprüchlichen Oberflächengeometrien berechnen.

## 10. Animation und Zeitsteuerung

Fluid Deformation verwendet das bestehende NovaOS Animation System.

Anforderungen:

- Die Berechnung verwendet eine monotone Zeitbasis.
- Die Verformungsgeschwindigkeit darf nicht von der Bildwiederholrate abhängen.
- Simulationen müssen mit definierten Zeitschritten oder stabilen zeitabhängigen Verfahren arbeiten.
- Unterbrechungen und längere Frame-Abstände müssen kontrolliert behandelt werden.
- Verformungen können pausiert, fortgesetzt und beendet werden.
- Nach Erreichen des Ruhezustands wird die aktive Simulation beendet.

## 11. Damage Tracking

Fluid Deformation muss vollständig mit dem Damage Tracking des Nova Compositors kompatibel sein.

Verbindliche Anforderungen:

- Ausschließlich betroffene Materialbereiche werden invalidiert.
- Bisherige und neue Verformungsbereiche müssen berücksichtigt werden.
- Änderungen der Oberflächennormalen invalidieren abhängige Materialeffekte.
- Erweiterte Abtastbereiche für Lichtbrechung und Hintergrundverzerrung müssen berücksichtigt werden.
- Unveränderte Oberflächenbereiche dürfen nicht erneut verarbeitet werden.
- Unsichtbare Materialoberflächen dürfen keine unnötigen Rendering-Operationen auslösen.
- Nach Erreichen des Ruhezustands dürfen keine weiteren verformungsbedingten Bildaktualisierungen erfolgen.

Eine permanente vollständige Bildschirmaktualisierung ist nicht zulässig.

## 12. Performance

Fluid Deformation muss ressourcenschonend arbeiten.

Verbindliche Anforderungen:

- GPU-Beschleunigung wird bevorzugt.
- Vereinfachte mathematische Verfahren müssen verfügbar sein.
- Die Anzahl virtueller Oberflächenpunkte muss begrenzbar sein.
- Verformungsdaten dürfen zwischen Materialeffekten gemeinsam verwendet werden.
- Unveränderte Berechnungsergebnisse dürfen zwischengespeichert werden.
- Die Berechnung muss auf aktive und relevante Oberflächenbereiche begrenzt bleiben.
- Unnötige CPU-GPU-Datenübertragungen werden vermieden.

## 13. Qualitätsstufen und Fallback

Nova Fluid Deformation unterstützt folgende Qualitätsstufen:

1. **Full:** Dynamische Verformung mit mehreren Impulsen und detaillierter Oberflächenberechnung.
2. **Balanced:** Vereinfachte Simulation mit reduzierter Oberflächenauflösung.
3. **Reduced:** Mathematische Verformung ohne komplexe Wellenausbreitung.
4. **Fallback:** Statische Materialoberfläche ohne Verformung.

Die Qualitätsstufe kann abhängig von Hardwareleistung, Energiezustand und Systemeinstellungen angepasst werden.

Ein Qualitätswechsel darf keine Neuerstellung des zugehörigen UI-Elements erfordern.

## 14. Barrierefreiheit

Nova Fluid Deformation muss systemweite Einstellungen zur Barrierefreiheit berücksichtigen.

Anforderungen:

- Flüssige Oberflächenbewegungen müssen reduzierbar oder deaktivierbar sein.
- Die Einstellung für reduzierte Bewegung muss berücksichtigt werden.
- Starke Schwingungen und schnelle Verformungen müssen vermeidbar sein.
- Texte und funktionale Bedienelemente dürfen nicht unlesbar werden.
- Bei deaktivierter Verformung muss die vollständige Bedienbarkeit erhalten bleiben.

## 15. Determinismus und Sicherheit

Nova Fluid Deformation muss bei identischen Eingaben, Anfangszuständen und Simulationszeiten reproduzierbare Ergebnisse innerhalb definierter numerischer Toleranzen liefern.

Anforderungen:

- Verformungsparameter müssen validiert und begrenzt werden.
- Maximale Auslenkung, Geschwindigkeit und Simulationskomplexität müssen kontrollierbar sein.
- Numerisch instabile Zustände müssen erkannt und abgefangen werden.
- Ungültige Simulationswerte dürfen nicht an nachgelagerte Materialeffekte weitergegeben werden.
- Verformungen dürfen keine Sicherheitsgrenzen des Compositors umgehen.
- Fehlerhafte Berechnungen dürfen die Rendering-Pipeline nicht destabilisieren.
- Bei Berechnungsfehlern wird die unverformte Materialoberfläche verwendet.

## 16. Ergebnis

Nova Fluid Deformation stellt eine einheitliche Verarbeitung flüssiger und elastischer Oberflächenverformungen innerhalb der Nova Material Engine bereit.

Durch vereinfachte Oberflächensimulationen, interaktionsabhängige Impulse und gemeinsame Verformungsdaten entstehen natürlich wirkende Materialbewegungen.

Die Integration in die bestehende Grafikarchitektur ermöglicht dynamische Liquid-Glass-Effekte bei kontrolliertem Ressourcenverbrauch.