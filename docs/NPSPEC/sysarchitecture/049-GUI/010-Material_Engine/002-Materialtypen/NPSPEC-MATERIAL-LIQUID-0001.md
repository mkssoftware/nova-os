# NPSPEC-MATERIAL-LIQUID-0001 – Nova Liquid Glass Material

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Material Engine

## 1. Zweck

Nova Liquid Glass definiert ein dynamisches, lichtbrechendes Oberflächenmaterial mit realistischen Glasreflexionen, transparenter Darstellung und flüssigen Verformungen.

Das Material erzeugt den Eindruck einer dreidimensionalen Glasoberfläche, die auf Hintergrundinhalte, Beleuchtung und Benutzerinteraktionen reagiert.

Ziel ist eine hochwertige, lebendige und zugleich ressourcenschonende Glasdarstellung innerhalb von NovaOS.

## 2. Materialeigenschaften

Nova Liquid Glass verwendet das gemeinsame Materialmodell der Nova Material Engine.

Unterstützte Eigenschaften:

- **Color:** Grundfarbe des Glases.
- **Opacity:** Deckkraft des Materials.
- **Tint:** Farbüberlagerung.
- **Blur:** Stärke der Hintergrundunschärfe.
- **Refraction:** Intensität der Lichtbrechung.
- **Reflection:** Stärke der Oberflächenreflexionen.
- **Highlight:** Intensität der Glanzlichter.
- **Distortion:** Stärke der optischen Verzerrung.
- **Roughness:** Oberflächenrauheit.
- **Thickness:** Virtuelle Glasdicke.
- **EdgeIntensity:** Intensität der Glaskanten.
- **Fluidity:** Stärke des flüssigen Verformungsverhaltens.
- **LightResponse:** Reaktion auf Beleuchtungsänderungen.

Alle Eigenschaften müssen individuell konfigurierbar und innerhalb definierter Wertebereiche validiert werden.

## 3. Darstellung

Liquid Glass kombiniert transparente Oberflächen mit Lichtbrechung, Reflexionen und dynamischen Glaskanten.

Verbindliche Anforderungen:

- Hintergrundinhalte werden entsprechend der Materialeigenschaften optisch verzerrt.
- Die Lichtbrechung berücksichtigt die virtuelle Glasdicke und Oberflächenform.
- Glaskanten erhalten definierte Lichtreflexionen und Glanzlichter.
- Transparenz und Farbüberlagerung bleiben unabhängig konfigurierbar.
- Reflexionen reagieren auf verfügbare Beleuchtungsinformationen.
- Abgerundete Ecken und individuelle Oberflächenmasken werden unterstützt.
- Materialeffekte bleiben auf die definierten Ausgabebereiche begrenzt.

Die Darstellung muss auch ohne physikalisch vollständige Lichtsimulation möglich sein.

## 4. Lichtbrechung

Die Lichtbrechung bildet den zentralen visuellen Effekt von Liquid Glass.

Anforderungen:

- Hintergrundinformationen werden über den Nova Compositor bereitgestellt.
- Die Verzerrung erfolgt anhand einer berechneten Oberflächen- oder Normalenstruktur.
- Materialdicke und Brechungsintensität beeinflussen die Abtastkoordinaten.
- Glaskanten können eine stärkere Lichtbrechung als die Materialmitte aufweisen.
- Die Lichtbrechung muss mit Clipping und Maskierung kompatibel sein.
- Abtastbereiche außerhalb verfügbarer Hintergrunddaten müssen definiert behandelt werden.

Die Lichtbrechung darf keine direkten Zugriffe auf fremde Surface-Buffer erfordern.

## 5. Reflexionen und Beleuchtung

Liquid Glass unterstützt dynamische Reflexionen und Glanzlichter.

Verbindliche Anforderungen:

- Reflexionen können aus verfügbaren Umgebungs- und Beleuchtungsinformationen berechnet werden.
- Glanzlichter berücksichtigen die virtuelle Oberflächenform.
- Kantenreflexionen können unabhängig von zentralen Reflexionen konfiguriert werden.
- Beleuchtungsänderungen invalidieren ausschließlich betroffene Materialbereiche.
- Statische Beleuchtungszustände dürfen zwischengespeichert werden.

Eine permanente Neuberechnung ohne sichtbare Änderungen ist nicht zulässig.

## 6. Flüssige Verformung

Liquid Glass unterstützt dynamische Veränderungen seiner virtuellen Oberflächenform.

Mögliche Auslöser:

- Maus- und Touchinteraktionen
- Hover- und Pressed-Zustände
- Größenänderungen
- Bewegungen und Übergänge
- Änderungen der Materialgeometrie

Anforderungen:

- Verformungen müssen weich und zeitlich kontinuierlich erfolgen.
- Die Intensität wird durch die Materialeigenschaft Fluidity gesteuert.
- Verformungen dürfen die tatsächliche Eingabe- und Layoutgeometrie nicht unbeabsichtigt verändern.
- Animationen müssen unterbrechbar und reproduzierbar sein.
- Nach Abschluss einer Animation muss ein stabiler Materialzustand erreicht werden.

Die Berechnung verwendet das bestehende NovaOS-Animationssystem.

## 7. Hintergrundverarbeitung

Liquid Glass benötigt Zugriff auf die hinter dem Material liegenden Bildinhalte.

Anforderungen:

- Die Hintergrundabtastung erfolgt ausschließlich über den Nova Compositor.
- Die korrekte Ebenenreihenfolge muss berücksichtigt werden.
- Verzerrungsbedingte Abtastbereiche werden bei der Ressourcen- und Damage-Berechnung berücksichtigt.
- Hintergrundänderungen invalidieren abhängige Materialbereiche.
- Unveränderte Hintergrundinformationen können wiederverwendet werden.
- Geschützte Bildinhalte dürfen nicht durch Lichtbrechung oder Verzerrung offengelegt werden.

Zyklische Hintergrundabhängigkeiten müssen ausgeschlossen werden.

## 8. Dynamisches Verhalten

Liquid Glass unterstützt animierbare Materialeigenschaften.

Dazu gehören:

- Lichtbrechung und Verzerrung
- Reflexionen und Glanzlichter
- Transparenz und Farbüberlagerung
- Virtuelle Glasdicke
- Flüssige Oberflächenverformungen
- Übergänge zwischen Materialzuständen

Animationen werden ausschließlich bei aktiven Zustandsänderungen ausgeführt.

Die Material Engine darf keine eigenständige permanente Rendering-Schleife erzeugen.

## 9. Integration

Nova Liquid Glass kann innerhalb des nativen Nova UI-Systems verwendet werden.

Typische Einsatzbereiche:

- Schwebende Bedienelemente
- Fensterdekorationen
- Dialogfenster
- Kontextmenüs
- Taskbar und Startmenü
- Schaltflächen und interaktive Controls
- Benachrichtigungen
- Systemüberlagerungen

Die Materialzuweisung erfolgt über das Nova Material System und das Nova UI Style System.

Liquid Glass darf nicht als Voraussetzung für die grundlegende Bedienbarkeit von NovaOS dienen.

## 10. Performance

Liquid Glass muss die vorhandene Rendering-Infrastruktur effizient verwenden.

Verbindliche Anforderungen:

- GPU-Beschleunigung wird bevorzugt.
- Lichtbrechung und Reflexionen werden nur bei Bedarf berechnet.
- Unveränderte Materialbereiche verwenden vorhandene Zwischenergebnisse.
- Effektbedingte Abtastbereiche werden begrenzt.
- Unsichtbare Materialien werden nicht verarbeitet.
- Kompatible Effektoperationen können zusammengefasst werden.
- Temporäre Rendering-Ressourcen werden nach Möglichkeit wiederverwendet.
- CPU-GPU-Datenübertragungen werden minimiert.

Die Materialverarbeitung muss mit dem Damage Tracking des Nova Compositors kompatibel sein.

## 11. Qualitätsstufen und Fallback

Liquid Glass unterstützt unterschiedliche Darstellungsqualitäten.

Definierte Qualitätsstufen:

1. **Full:** Lichtbrechung, Reflexionen, Glanzlichter und flüssige Verformungen.
2. **Balanced:** Vereinfachte Lichtbrechung und reduzierte Reflexionsberechnung.
3. **Reduced:** Transparenz, Hintergrundunschärfe und statische Glaskanten.
4. **Fallback:** Darstellung über Nova Acrylic oder Nova Solid.

Die Qualitätsstufe kann abhängig von Hardwareleistung, Energiezustand und Systemeinstellungen angepasst werden.

Ein Qualitätswechsel darf keine Neuerstellung des zugehörigen UI-Elements erfordern.

## 12. Barrierefreiheit

Liquid Glass muss die systemweiten Einstellungen zur Barrierefreiheit berücksichtigen.

Anforderungen:

- Transparenz und Hintergrundverzerrung müssen reduzierbar sein.
- Flüssige Animationen müssen deaktivierbar sein.
- Texte und Bedienelemente müssen ausreichend kontrastreich bleiben.
- Die Lesbarkeit hat Vorrang vor dekorativen Materialeffekten.
- Bei deaktivierten visuellen Effekten muss eine funktional gleichwertige Darstellung erhalten bleiben.

## 13. Determinismus und Sicherheit

Liquid Glass muss bei identischen Eingaben und Rendering-Bedingungen reproduzierbare Ergebnisse innerhalb definierter numerischer Toleranzen liefern.

Anforderungen:

- Animationen verwenden definierte Zeit- und Zustandsparameter.
- Zufällige Oberflächenveränderungen dürfen nur mit reproduzierbaren Parametern erfolgen.
- Materialeigenschaften und Effektparameter müssen validiert werden.
- Fehlerhafte Effekte dürfen die Rendering-Pipeline nicht destabilisieren.
- Materialinstanzen dürfen keine Sicherheitsgrenzen des Compositors umgehen.
- Bei Rendering-Fehlern wird ein definiertes Fallback-Material verwendet.

## 14. Ergebnis

Nova Liquid Glass stellt ein dynamisches Glasmaterial mit Lichtbrechung, Reflexionen, Glanzlichtern und flüssigen Oberflächenverformungen bereit.

Die Integration in die Nova Material Engine ermöglicht eine systemweit einheitliche Verwendung innerhalb des nativen NovaOS-UI-Systems.

Durch GPU-Beschleunigung, Damage Tracking, Caching und adaptive Qualitätsstufen entsteht eine hochwertige Glasdarstellung bei kontrolliertem Ressourcenverbrauch.