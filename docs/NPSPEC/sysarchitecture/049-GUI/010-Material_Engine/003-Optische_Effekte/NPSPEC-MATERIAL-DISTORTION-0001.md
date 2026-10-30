# NPSPEC-MATERIAL-DISTORTION-0001 – Nova Surface Distortion

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Material Engine

## 1. Zweck

Nova Surface Distortion definiert die optische Verzerrung von Hintergrundinhalten durch virtuelle Materialoberflächen innerhalb von NovaOS.

Der Effekt ermöglicht räumliche Verschiebungen, Wellenbewegungen und dynamische Oberflächenverformungen.

Er bildet insbesondere die Grundlage für flüssige Glasoberflächen wie Nova Liquid Glass.

## 2. Architektur

Nova Surface Distortion ist ein Effekt der Nova Material Engine und verwendet die bestehende Material Rendering Pipeline.

Die Zuständigkeiten sind:

- **Material System:** Definition der Verzerrungseigenschaften.
- **Material Renderer:** Berechnung und Anwendung der Verzerrung.
- **Compositor:** Bereitstellung der benötigten Hintergrundinformationen.
- **Graphics Engine:** Ausführung der Rendering-Operationen.
- **Animation System:** Steuerung zeitabhängiger Verzerrungen.

Surface Distortion implementiert keinen eigenständigen Renderer.

## 3. Eigenschaften

Der Effekt unterstützt folgende Eigenschaften:

- **Distortion:** Intensität der Verzerrung.
- **DistortionScale:** Räumliche Skalierung.
- **DistortionMap:** Optionale Verzerrungstextur.
- **DistortionDirection:** Bevorzugte Verzerrungsrichtung.
- **WaveAmplitude:** Amplitude von Wellenbewegungen.
- **WaveFrequency:** Räumliche Wellenfrequenz.
- **Fluidity:** Stärke des flüssigen Verformungsverhaltens.
- **EdgeDistortion:** Verzerrungsintensität an Materialkanten.
- **DistortionSpeed:** Geschwindigkeit animierter Verzerrungen.
- **Quality:** Qualitätsstufe der Berechnung.

Alle Eigenschaften müssen definierte Datentypen, Wertebereiche und Standardwerte besitzen.

Eine Distortion-Intensität von 0 deaktiviert den Effekt.

## 4. Verzerrungsmodell

Nova Surface Distortion verwendet bildraumbasierte Verzerrungen durch veränderte Abtastkoordinaten.

Verbindliche Anforderungen:

- Hintergrundpixel werden anhand berechneter Verschiebungsvektoren abgetastet.
- Verzerrungen können horizontal und vertikal erfolgen.
- Die Verzerrungsintensität muss unabhängig von der Materialtransparenz einstellbar sein.
- Verzerrungen müssen mit Lichtbrechung, Unschärfe und Reflexionen kombinierbar sein.
- Die tatsächliche UI-Layoutgeometrie bleibt unverändert.
- Der maximale Abtastversatz muss begrenzbar sein.

Die Verzerrung betrifft ausschließlich die visuelle Darstellung.

## 5. Verzerrungsverfahren

Nova Surface Distortion unterstützt folgende Verfahren:

- **Displacement Map:** Verzerrung anhand einer Verschiebungstextur.
- **Normal-Based Distortion:** Verzerrung anhand virtueller Oberflächennormalen.
- **Wave Distortion:** Wellenförmige Verschiebungen.
- **Procedural Distortion:** Mathematisch erzeugte Verzerrungsmuster.
- **Interaction Distortion:** Verzerrung durch Benutzerinteraktionen.

Mehrere Verfahren können kombiniert werden.

Die Kombination erfolgt über die gemeinsame Material Rendering Pipeline.

## 6. Oberflächenverformung

Surface Distortion unterstützt die visuelle Simulation verformbarer Oberflächen.

Anforderungen:

- Verformungen können lokal oder über die gesamte Materialoberfläche erfolgen.
- Materialkanten können unabhängig von der Materialmitte verzerrt werden.
- Abgerundete Ecken und individuelle Masken müssen unterstützt werden.
- Verformungen dürfen keine unbeabsichtigten Änderungen an Eingabe- oder Layoutbereichen verursachen.
- Die Verformungsintensität muss begrenzbar sein.
- Nach Abschluss einer Animation muss ein definierter stabiler Zustand erreicht werden.

Eine tatsächliche Veränderung der Oberflächengeometrie erfolgt nur über die dafür vorgesehenen UI- und Layoutsysteme.

## 7. Dynamische Verzerrung

Verzerrungsparameter können zur Laufzeit verändert werden.

Unterstützt werden:

- Flüssige Wellenbewegungen
- Interaktionsabhängige Verzerrungen
- Animierte Verschiebungstexturen
- Änderungen der Verzerrungsintensität
- Übergänge zwischen Materialzuständen
- Reaktionen auf Bewegungen und Größenänderungen

Animationen verwenden das bestehende NovaOS-Animationssystem.

Ohne aktive Animationen oder relevante Änderungen darf keine kontinuierliche Neuberechnung erfolgen.

## 8. Hintergrundverarbeitung

Die Hintergrundabtastung erfolgt ausschließlich über den Nova Compositor.

Verbindliche Anforderungen:

- Die korrekte Ebenenreihenfolge muss berücksichtigt werden.
- Verzerrungen verwenden ausschließlich freigegebene Hintergrundinformationen.
- Der maximale Verschiebungsvektor bestimmt den erweiterten Abtastbereich.
- Abtastungen außerhalb verfügbarer Hintergrundbereiche müssen definiert behandelt werden.
- Selbstreferenzierende und zyklische Hintergrundabhängigkeiten sind ausgeschlossen.
- Geschützte Bildinhalte dürfen nicht durch Verzerrungen offengelegt werden.

Die Material Engine erhält keinen direkten Zugriff auf fremde Surface-Buffer.

## 9. Materialintegration

Nova Surface Distortion kann von unterschiedlichen Materialien verwendet werden.

- **Liquid Glass:** Flüssige Verformungen und dynamische Hintergrundverzerrungen.
- **Crystal:** Optionale statische Verzerrungen durch virtuelle Oberflächenstrukturen.
- **Acrylic:** Optionale dezente Verzerrungseffekte.
- **Frosted Glass:** Optionale Verzerrungen in Kombination mit Hintergrundunschärfe.

Die Verwendung erfolgt über das gemeinsame Materialmodell.

## 10. Damage Tracking

Surface Distortion muss mit dem Damage Tracking des Nova Compositors kompatibel sein.

Verbindliche Anforderungen:

- Änderungen der Verzerrungsparameter invalidieren betroffene Materialbereiche.
- Hintergrundänderungen invalidieren alle Materialpixel, deren Abtastbereiche betroffen sind.
- Der maximale Verschiebungsbereich wird bei der Damage-Berechnung berücksichtigt.
- Bewegliche Verzerrungen müssen bisherige und neue Wirkungsbereiche berücksichtigen.
- Unveränderte Verzerrungsergebnisse dürfen wiederverwendet werden.
- Unsichtbare Materialbereiche werden nicht verarbeitet.

Eine permanente vollständige Bildschirmaktualisierung ist nicht zulässig.

## 11. Performance

Die Verzerrungsberechnung muss ressourcenschonend erfolgen.

Verbindliche Anforderungen:

- GPU-Beschleunigung wird bevorzugt.
- Statische Verzerrungstexturen dürfen zwischengespeichert werden.
- Verschiebungsvektoren können vorberechnet werden.
- Gleichartige Verzerrungsoperationen können zusammengefasst werden.
- Temporäre Rendering-Ressourcen werden nach Möglichkeit wiederverwendet.
- Hintergrundabtastungen bleiben auf benötigte Bereiche begrenzt.
- Unnötige CPU-GPU-Datenübertragungen werden vermieden.

## 12. Qualitätsstufen und Fallback

Nova Surface Distortion unterstützt folgende Qualitätsstufen:

1. **Full:** Dynamische Verzerrungen mit mehreren kombinierten Verfahren.
2. **Balanced:** Vereinfachte Verzerrungsberechnung mit reduzierter Abtastkomplexität.
3. **Reduced:** Statische oder vereinfachte Verschiebungsvektoren.
4. **Fallback:** Materialdarstellung ohne Verzerrung.

Die Qualitätsstufe kann abhängig von Hardwareleistung, Energiezustand und Systemeinstellungen angepasst werden.

Ein Qualitätswechsel darf keine Neuerstellung des zugehörigen UI-Elements erfordern.

## 13. Barrierefreiheit

Nova Surface Distortion muss systemweite Einstellungen zur Barrierefreiheit berücksichtigen.

Anforderungen:

- Dynamische Verzerrungen müssen reduzierbar oder deaktivierbar sein.
- Starke Wellenbewegungen müssen vermeidbar sein.
- Vordergrundtexte und Bedienelemente dürfen nicht durch Verzerrungen unlesbar werden.
- Systemweite Einstellungen zur Reduzierung von Animationen müssen berücksichtigt werden.
- Bei deaktivierten Effekten muss eine funktional gleichwertige Darstellung erhalten bleiben.

## 14. Determinismus und Sicherheit

Nova Surface Distortion muss bei identischen Eingaben und Rendering-Bedingungen reproduzierbare Ergebnisse innerhalb definierter numerischer Toleranzen liefern.

Anforderungen:

- Verzerrungsparameter müssen validiert und begrenzt werden.
- Ungültige Verschiebungsvektoren müssen kontrolliert behandelt werden.
- Speicherzugriffe außerhalb gültiger Abtastbereiche sind ausgeschlossen.
- Prozedurale Verzerrungen müssen reproduzierbare Parameter verwenden.
- Materialinstanzen dürfen keine Sicherheitsgrenzen des Compositors umgehen.
- Fehlerhafte Effektberechnungen dürfen die Rendering-Pipeline nicht destabilisieren.
- Bei Berechnungsfehlern wird der Effekt deaktiviert oder ein definiertes Fallback verwendet.

## 15. Ergebnis

Nova Surface Distortion stellt eine einheitliche Verarbeitung optischer Oberflächenverzerrungen innerhalb der Nova Material Engine bereit.

Durch Verschiebungstexturen, Oberflächennormalen, Wellenbewegungen und interaktionsabhängige Effekte entstehen dynamische und flüssig wirkende Materialoberflächen.

Die Integration in die bestehende Grafikarchitektur ermöglicht hochwertige Verzerrungseffekte bei kontrolliertem Ressourcenverbrauch.