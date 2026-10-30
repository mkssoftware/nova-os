# NPSPEC-MATERIAL-BACKDROP-0001 – Nova Backdrop Sampling

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Material Engine

## 1. Zweck

Nova Backdrop Sampling definiert die kontrollierte Bereitstellung und Abtastung von Hintergrundinhalten für transparente und hintergrundabhängige Materialien innerhalb von NovaOS.

Das System ermöglicht Effekte wie Hintergrundunschärfe, Lichtbrechung, Reflexionen und optische Verzerrungen.

Ziel ist eine einheitliche, sichere und ressourcenschonende Hintergrundverarbeitung ohne redundante Bildkopien oder unnötige Rendering-Durchläufe.

## 2. Architektur

Backdrop Sampling ist die Schnittstelle zwischen der Nova Material Engine und dem Nova Compositor.

Die Zuständigkeiten sind:

- **Compositor:** Verwaltung der Oberflächen, Ebenenreihenfolge und Hintergrundbereitstellung.
- **Backdrop Sampling:** Definition und Verwaltung der benötigten Abtastbereiche.
- **Material Renderer:** Verarbeitung der Hintergrundinformationen.
- **Graphics Engine:** Ausführung der Abtastoperationen.
- **Damage Tracking:** Erkennung relevanter Hintergrundänderungen.

Backdrop Sampling implementiert keinen eigenständigen Renderer.

## 3. Grundprinzip

Hintergrundabhängige Materialien erhalten eine kontrollierte Ansicht der bereits komponierten, zulässigen Bildinhalte hinter ihrer Oberfläche.

Verbindliche Anforderungen:

- Die Abtastung berücksichtigt die tatsächliche Ebenenreihenfolge.
- Vordergrundinhalte dürfen nicht Bestandteil des Hintergrunds sein.
- Materialien dürfen ihren eigenen Rendering-Ausgang nicht erneut abtasten.
- Zyklische Abhängigkeiten zwischen Materialoberflächen sind ausgeschlossen.
- Die Hintergrundbereitstellung erfolgt ausschließlich über den Compositor.
- Geschützte Inhalte dürfen nicht unberechtigt zugänglich werden.

## 4. Sampling-Eigenschaften

Backdrop Sampling unterstützt folgende Eigenschaften:

- **SamplingRegion:** Benötigter Hintergrundbereich.
- **SamplingOffset:** Verschiebung der Abtastkoordinaten.
- **SamplingScale:** Skalierung der Hintergrundabtastung.
- **SamplingFilter:** Verwendetes Filterverfahren.
- **SamplingQuality:** Qualitätsstufe der Abtastung.
- **SamplingExpansion:** Erweiterung des Abtastbereichs.
- **SamplingMask:** Optionale Begrenzungsmaske.
- **SamplingResolution:** Auflösung der Hintergrunddaten.

Alle Eigenschaften müssen definierte Datentypen, Wertebereiche und Standardwerte besitzen.

## 5. Abtastbereiche

Der benötigte Hintergrundbereich wird aus der Materialgeometrie und den aktiven Effekten bestimmt.

Verbindliche Anforderungen:

- Der sichtbare Materialbereich bildet die Grundlage der Abtastung.
- Unschärfe erweitert den benötigten Bereich entsprechend ihrem Filterradius.
- Lichtbrechung und Verzerrung berücksichtigen maximale Koordinatenverschiebungen.
- Kombinierte Effekte müssen ihre tatsächlichen Abtastabhängigkeiten berücksichtigen.
- Abtastbereiche werden auf gültige und autorisierte Bildbereiche begrenzt.
- Nicht benötigte Hintergrundbereiche dürfen nicht verarbeitet werden.

## 6. Sampling-Verfahren

Nova Backdrop Sampling unterstützt mindestens:

- **Nearest:** Abtastung des nächstgelegenen Pixels.
- **Linear:** Bilineare Interpolation.
- **Filtered:** Abtastung über definierte Filterverfahren.
- **Downsampled:** Abtastung einer reduzierten Hintergrundauflösung.

Die Auswahl richtet sich nach Materialeffekt, Qualitätsstufe und verfügbarer Hardware.

Effektspezifische Filter werden durch die jeweiligen Materialeffekte definiert.

## 7. Hintergrundkomposition

Der Compositor stellt den Hintergrund entsprechend der festgelegten Kompositionsreihenfolge bereit.

Anforderungen:

- Hintergrunddaten müssen den zum Rendering-Zeitpunkt gültigen Kompositionszustand repräsentieren.
- Transparente darunterliegende Oberflächen müssen korrekt berücksichtigt werden.
- Verschachtelte Materialoberflächen müssen eindeutig aufgelöst werden.
- Hintergrundabhängige Effekte dürfen keine rekursiven Rendering-Schleifen erzeugen.
- Der Compositor bestimmt die zulässigen Sampling-Quellen.
- Nicht autorisierte Oberflächen werden gemäß der Sicherheitsrichtlinie ausgeschlossen oder durch definierte Ersatzinhalte dargestellt.

## 8. Koordinatensystem

Backdrop Sampling verwendet ein eindeutig definiertes Koordinatenmodell.

Verbindliche Anforderungen:

- Materialkoordinaten müssen in Compositor-Koordinaten transformierbar sein.
- Fensterposition, Skalierung und Transformationen müssen berücksichtigt werden.
- Unterschiedliche Display-Skalierungen müssen unterstützt werden.
- Abtastungen außerhalb gültiger Bildbereiche müssen definiert behandelt werden.
- Sampling-Koordinaten dürfen keine ungültigen Speicherzugriffe verursachen.

## 9. Integration mit Materialeffekten

Backdrop Sampling stellt Hintergrundinformationen für folgende Effekte bereit:

- **Backdrop Blur:** Hintergrundunschärfe.
- **Light Refraction:** Lichtbrechung.
- **Surface Reflections:** Hintergrundabhängige Reflexionsanteile.
- **Surface Distortion:** Optische Hintergrundverzerrung.
- **Adaptive Transparency:** Analyse von Hintergrundhelligkeit und Kontrast.
- **Material Tinting:** Hintergrundabhängige Farbanpassung.

Mehrere Effekte derselben Materialinstanz sollen gemeinsame Hintergrunddaten wiederverwenden.

Die eigentliche Effektberechnung verbleibt beim jeweiligen Materialeffekt.

## 10. Damage Tracking

Backdrop Sampling muss vollständig mit dem Damage Tracking des Nova Compositors kompatibel sein.

Verbindliche Anforderungen:

- Hintergrundänderungen invalidieren ausschließlich abhängige Materialbereiche.
- Die Abhängigkeiten müssen anhand der tatsächlichen Sampling-Bereiche bestimmt werden.
- Erweiterte Abtastbereiche müssen berücksichtigt werden.
- Änderungen der Materialposition aktualisieren die Sampling-Abhängigkeiten.
- Unveränderte Hintergrundbereiche dürfen wiederverwendet werden.
- Unsichtbare Materialien dürfen keine unnötigen Hintergrundabtastungen auslösen.
- Hintergrundänderungen dürfen keine zyklischen Invalidierungen verursachen.

Eine permanente vollständige Bildschirmaktualisierung ist nicht zulässig.

## 11. Caching und Ressourcenverwaltung

Hintergrunddaten dürfen zur Vermeidung redundanter Berechnungen zwischengespeichert werden.

Anforderungen:

- Cache-Einträge müssen ihrem gültigen Kompositionszustand zugeordnet sein.
- Änderungen relevanter Hintergrundbereiche invalidieren betroffene Cache-Einträge.
- Gemeinsame Hintergrundbereiche dürfen zwischen kompatiblen Materialeffekten wiederverwendet werden.
- GPU-Ressourcen sollen nach Möglichkeit ohne CPU-Kopien verarbeitet werden.
- Temporäre Buffer müssen wiederverwendbar und begrenzbar sein.
- Nicht mehr benötigte Ressourcen müssen freigegeben werden.

## 12. Performance

Backdrop Sampling muss ressourcenschonend erfolgen.

Verbindliche Anforderungen:

- GPU-basierte Hintergrundabtastung wird bevorzugt.
- Zero-Copy-Verfahren sollen verwendet werden, sofern technisch und sicherheitstechnisch möglich.
- Unnötige vollständige Bildschirmkopien sind zu vermeiden.
- Abtastoperationen werden auf notwendige Bereiche begrenzt.
- Gleichartige Sampling-Anforderungen können zusammengefasst werden.
- Reduzierte Sampling-Auflösungen müssen unterstützt werden.
- Unnötige CPU-GPU-Synchronisationen sind zu vermeiden.

## 13. Qualitätsstufen und Fallback

Nova Backdrop Sampling unterstützt folgende Qualitätsstufen:

1. **Full:** Vollständige Hintergrundabtastung mit effektspezifischer Filterung.
2. **Balanced:** Optimierte Abtastung mit gegebenenfalls reduzierter Auflösung.
3. **Reduced:** Vereinfachte Hintergrundabtastung mit begrenztem Sampling-Bereich.
4. **Fallback:** Keine Hintergrundabtastung; Verwendung definierter Ersatzfarben oder Solid-Materialien.

Die Qualitätsstufe kann abhängig von Hardwareleistung, Energiezustand und Systemeinstellungen angepasst werden.

## 14. Barrierefreiheit

Backdrop Sampling muss systemweite Einstellungen zur Barrierefreiheit berücksichtigen.

Anforderungen:

- Hintergrundabhängige Effekte müssen reduzierbar oder deaktivierbar sein.
- Hohe Kontrasteinstellungen müssen berücksichtigt werden.
- Bei deaktivierter Transparenz darf keine unnötige Hintergrundabtastung erfolgen.
- Die Lesbarkeit von Vordergrundinhalten muss erhalten bleiben.
- Eine funktional gleichwertige Darstellung ohne Hintergrundabtastung muss verfügbar sein.

## 15. Determinismus und Sicherheit

Backdrop Sampling muss bei identischen Eingaben und Kompositionszuständen reproduzierbare Ergebnisse innerhalb definierter numerischer Toleranzen liefern.

Verbindliche Anforderungen:

- Sampling-Parameter müssen validiert und begrenzt werden.
- Speicherzugriffe außerhalb gültiger Buffer sind ausgeschlossen.
- Materialinstanzen dürfen keine direkten Zugriffsrechte auf fremde Surface-Buffer erhalten.
- Geschützte Inhalte dürfen weder über Sampling-Ergebnisse noch über abgeleitete Analysewerte unberechtigt offengelegt werden.
- Sampling-Ressourcen müssen gemäß den Sicherheitsgrenzen des Compositors isoliert werden.
- Fehlerhafte Sampling-Anforderungen dürfen die Rendering-Pipeline nicht destabilisieren.
- Bei Berechnungs- oder Berechtigungsfehlern wird ein definierter sicherer Ersatzhintergrund verwendet.

## 16. Ergebnis

Nova Backdrop Sampling stellt eine einheitliche und sichere Hintergrundabtastung für die Nova Material Engine bereit.

Durch kontrollierte Sampling-Bereiche, gemeinsame Hintergrundressourcen und die Integration in das Damage Tracking können transparente und hintergrundabhängige Materialien effizient dargestellt werden.

Die Verantwortung für Ebenenreihenfolge, Hintergrundfreigabe und Sicherheitsgrenzen verbleibt vollständig beim Nova Compositor.