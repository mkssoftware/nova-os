# NPSPEC-MATERIAL-BLUR-0001 – Nova Backdrop Blur

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Material Engine

## 1. Zweck

Nova Backdrop Blur definiert die Hintergrundunschärfe für transparente und halbtransparente Materialien innerhalb von NovaOS.

Der Effekt ermöglicht eine weiche Darstellung hinterliegender Bildinhalte, ohne die Vordergrundelemente zu beeinflussen.

Ziel ist eine hochwertige, effiziente und ressourcenschonende Hintergrundunschärfe.

## 2. Architektur

Backdrop Blur ist ein Effekt der Nova Material Engine und verwendet die bestehende Nova Graphics Engine.

Die Zuständigkeiten sind:

- **Material System:** Definiert die Unschärfeparameter.
- **Material Renderer:** Verarbeitet den Blur-Effekt.
- **Compositor:** Stellt die benötigten Hintergrunddaten bereit.
- **Graphics Engine:** Führt die Unschärfeberechnung aus.

Backdrop Blur implementiert keinen eigenständigen Renderer.

## 3. Eigenschaften

Der Effekt unterstützt folgende Eigenschaften:

- **BlurRadius:** Stärke beziehungsweise Radius der Unschärfe.
- **BlurQuality:** Qualitätsstufe der Berechnung.
- **BlurType:** Verwendetes Unschärfeverfahren.
- **BlurScale:** Auflösungsskalierung der Berechnung.
- **EdgeMode:** Behandlung der Abtastgrenzen.

Alle Parameter müssen validiert werden.

Ein BlurRadius von 0 deaktiviert die Unschärfe.

## 4. Unschärfeverfahren

Nova Backdrop Blur unterstützt unterschiedliche Berechnungsverfahren.

Vorgesehen sind:

- **Gaussian Blur:** Gleichmäßige, natürliche Hintergrundunschärfe.
- **Box Blur:** Vereinfachte und schnelle Unschärfeberechnung.
- **Kawase Blur:** Mehrstufige, GPU-optimierte Hintergrundunschärfe.

Das Rendering-Backend wählt anhand der Materialanforderungen und verfügbaren Ressourcen ein geeignetes Verfahren.

Die Auswahl darf die Materialdefinition nicht verändern.

## 5. Hintergrundverarbeitung

Die Hintergrunddaten werden ausschließlich über den Nova Compositor bereitgestellt.

Verbindliche Anforderungen:

- Die Hintergrundabtastung berücksichtigt die korrekte Ebenenreihenfolge.
- Nur unterhalb des Materials liegende Inhalte werden berücksichtigt.
- Der benötigte Abtastbereich wird entsprechend dem Blur-Radius erweitert.
- Clipping und Materialmasken müssen unterstützt werden.
- Geschützte Inhalte dürfen nicht unzulässig offengelegt werden.
- Selbstreferenzierende Hintergrundabtastungen sind ausgeschlossen.

Die Unschärfe darf keine Inhalte oberhalb des Materials beeinflussen.

## 6. Rendering

Die Berechnung erfolgt innerhalb der bestehenden Material Rendering Pipeline.

Anforderungen:

- Die Unschärfe wird vor der endgültigen Materialkomposition berechnet.
- Vordergrundinhalte bleiben von der Hintergrundunschärfe unbeeinflusst.
- Mehrstufige Berechnungen dürfen temporäre Buffer verwenden.
- Wiederverwendbare Zwischenergebnisse sollen gemeinsam genutzt werden.
- Die Berechnung muss mit Transparenz, Farbüberlagerungen und weiteren Materialeffekten kombinierbar sein.
- Farbraum und Alpha-Verarbeitung müssen mit der Nova Graphics Engine kompatibel bleiben.

Die Reihenfolge kombinierter Effekte wird durch die Material Rendering Pipeline festgelegt.

## 7. Damage Tracking

Backdrop Blur muss das bestehende Damage Tracking des Nova Compositors verwenden.

Verbindliche Anforderungen:

- Hintergrundänderungen invalidieren abhängige Blur-Bereiche.
- Der tatsächliche Abtastbereich des Blur-Verfahrens wird bei der Invalidierung berücksichtigt.
- Änderungen außerhalb des relevanten Abtastbereichs dürfen keine Neuberechnung auslösen.
- Unveränderte Blur-Ergebnisse können zwischengespeichert werden.
- Änderungen des Blur-Radius invalidieren die betroffenen Materialbereiche.
- Unsichtbare Blur-Bereiche werden nicht verarbeitet.

Eine permanente vollständige Neuberechnung des Bildschirms ist nicht zulässig.

## 8. Performance

Backdrop Blur muss für eine ressourcenschonende Darstellung optimiert sein.

Anforderungen:

- GPU-Beschleunigung wird bevorzugt.
- Separierbare Filter dürfen zur Reduzierung des Berechnungsaufwands verwendet werden.
- Mehrstufige Verfahren können mit reduzierter Auflösung arbeiten.
- Temporäre Rendering-Buffer werden nach Möglichkeit wiederverwendet.
- Identische Hintergrundbereiche und kompatible Blur-Parameter können gemeinsame Zwischenergebnisse nutzen.
- Unnötige CPU-GPU-Datenübertragungen werden vermieden.
- Die Berechnung bleibt auf tatsächlich benötigte Bereiche begrenzt.

Optimierungen dürfen keine sichtbaren Artefakte an Materialgrenzen verursachen.

## 9. Qualitätsstufen und Fallback

Backdrop Blur unterstützt folgende Qualitätsstufen:

1. **Full:** Hochwertige Unschärfe mit voller vorgesehener Qualität.
2. **Balanced:** Optimierte Unschärfe mit reduziertem Berechnungsaufwand.
3. **Reduced:** Vereinfachte Unschärfe mit reduzierter Auflösung.
4. **Fallback:** Farbüberlagerung ohne Hintergrundunschärfe.

Die Qualitätsstufe kann abhängig von Hardwareleistung, Energiezustand und Systemeinstellungen angepasst werden.

Bei fehlender GPU-Beschleunigung kann ein CPU-basiertes Verfahren verwendet werden.

## 10. Barrierefreiheit

Backdrop Blur muss systemweite Einstellungen zur Barrierefreiheit berücksichtigen.

Anforderungen:

- Hintergrundunschärfe muss reduzierbar oder deaktivierbar sein.
- Die Lesbarkeit von Vordergrundinhalten hat Vorrang vor dekorativen Effekten.
- Kontraste müssen durch geeignete Materialeigenschaften gewährleistet werden.
- Bei deaktivierter Transparenz muss eine undurchsichtige Ersatzdarstellung möglich sein.

## 11. Determinismus und Sicherheit

Backdrop Blur muss bei identischen Eingaben und Rendering-Bedingungen reproduzierbare Ergebnisse innerhalb definierter numerischer Toleranzen liefern.

Verbindliche Anforderungen:

- Blur-Parameter müssen validiert und begrenzt werden.
- Abtastungen außerhalb gültiger Speicherbereiche sind ausgeschlossen.
- Ungültige Effektparameter dürfen die Rendering-Pipeline nicht destabilisieren.
- Anwendungen erhalten keinen direkten Zugriff auf fremde Surface-Buffer.
- Bei Berechnungsfehlern wird ein definiertes Fallback verwendet.

## 12. Ergebnis

Nova Backdrop Blur stellt eine einheitliche und effiziente Hintergrundunschärfe für die Nova Material Engine bereit.

Der Effekt unterstützt unterschiedliche Unschärfeverfahren, GPU-Beschleunigung, Damage Tracking und adaptive Qualitätsstufen.

Durch die Integration in die bestehende Grafikarchitektur können Acrylic, Frosted Glass, Liquid Glass und weitere Materialien Hintergrundunschärfe verwenden, ohne eigene Blur-Implementierungen zu benötigen.