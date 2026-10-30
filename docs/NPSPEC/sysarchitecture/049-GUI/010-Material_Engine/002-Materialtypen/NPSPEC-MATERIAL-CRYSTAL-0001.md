# NPSPEC-MATERIAL-CRYSTAL-0001 – Nova Crystal Material

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Material Engine

## 1. Zweck

Nova Crystal definiert ein klares, transparentes Glasmaterial mit ausgeprägter Lichtbrechung, scharfen Reflexionen und präzisen Glanzlichtern.

Das Material erzeugt den Eindruck einer hochwertigen, festen Kristallglasoberfläche mit optischer Tiefe und klar definierten Kanten.

Im Gegensatz zu Nova Liquid Glass besitzt Crystal standardmäßig keine flüssigen Oberflächenverformungen.

## 2. Materialeigenschaften

Nova Crystal verwendet das gemeinsame Materialmodell der Nova Material Engine.

Unterstützte Eigenschaften:

- **Color:** Grundfarbe des Kristallglases.
- **Opacity:** Deckkraft des Materials.
- **Tint:** Farbüberlagerung.
- **Refraction:** Intensität der Lichtbrechung.
- **Reflection:** Stärke der Oberflächenreflexionen.
- **Highlight:** Intensität der Glanzlichter.
- **Roughness:** Oberflächenrauheit.
- **Thickness:** Virtuelle Glasdicke.
- **EdgeIntensity:** Intensität der Glaskanten.
- **Dispersion:** Stärke der chromatischen Lichtzerlegung.
- **Clarity:** Optische Klarheit des Materials.
- **LightResponse:** Reaktion auf Beleuchtungsänderungen.

Alle Eigenschaften müssen individuell konfigurierbar und innerhalb definierter Wertebereiche validiert werden.

## 3. Darstellung

Crystal kombiniert hohe Transparenz mit präziser Lichtbrechung und ausgeprägten Glaskanten.

Verbindliche Anforderungen:

- Hintergrundinhalte bleiben weitgehend klar erkennbar.
- Die Lichtbrechung berücksichtigt die virtuelle Glasdicke und Oberflächenform.
- Glaskanten können eine stärkere Lichtbrechung als die Materialmitte aufweisen.
- Reflexionen und Glanzlichter müssen die virtuelle Oberflächengeometrie berücksichtigen.
- Die Oberflächenrauheit beeinflusst die Schärfe der Reflexionen.
- Abgerundete Ecken und individuelle Masken werden unterstützt.
- Materialeffekte bleiben auf ihre definierten Ausgabebereiche begrenzt.

Crystal verwendet standardmäßig keine ausgeprägte Hintergrundunschärfe.

## 4. Lichtbrechung

Die Lichtbrechung bildet den zentralen visuellen Effekt von Nova Crystal.

Anforderungen:

- Hintergrundinformationen werden über den Nova Compositor bereitgestellt.
- Die Brechung erfolgt anhand einer berechneten Oberflächen- oder Normalenstruktur.
- Materialdicke und Brechungsintensität beeinflussen die Abtastkoordinaten.
- Kantenbereiche können eine erhöhte Brechungsintensität besitzen.
- Die Lichtbrechung muss mit Clipping und Maskierung kompatibel sein.
- Abtastungen außerhalb verfügbarer Hintergrundbereiche müssen definiert behandelt werden.

Eine physikalisch vollständige Strahlverfolgung ist nicht erforderlich.

## 5. Reflexionen und Glanzlichter

Nova Crystal unterstützt scharfe Oberflächenreflexionen und gerichtete Glanzlichter.

Verbindliche Anforderungen:

- Reflexionen verwenden verfügbare Umgebungs- und Beleuchtungsinformationen.
- Glanzlichter berücksichtigen die virtuelle Oberflächenform.
- Kantenreflexionen können unabhängig konfiguriert werden.
- Die Oberflächenrauheit beeinflusst die Reflexionsschärfe.
- Statische Reflexionsinformationen dürfen zwischengespeichert werden.
- Beleuchtungsänderungen invalidieren ausschließlich betroffene Materialbereiche.

## 6. Chromatische Dispersion

Crystal kann die unterschiedliche Brechung einzelner Farbkanäle simulieren.

Anforderungen:

- Die Intensität wird über die Eigenschaft Dispersion gesteuert.
- Der Effekt muss vollständig deaktivierbar sein.
- Die Dispersion darf die Lesbarkeit von Vordergrundinhalten nicht beeinträchtigen.
- Farbverschiebungen müssen auf definierte Materialbereiche begrenzt bleiben.
- Bei reduzierter Rendering-Qualität kann die Dispersion entfallen.

Chromatische Dispersion ist ein optionaler Effekt und keine Voraussetzung für die grundlegende Crystal-Darstellung.

## 7. Dynamisches Verhalten

Crystal unterstützt animierbare Materialeigenschaften.

Dazu gehören:

- Transparenz und Farbüberlagerung
- Lichtbrechung
- Reflexionen und Glanzlichter
- Virtuelle Glasdicke
- Chromatische Dispersion
- Übergänge zwischen Materialzuständen

Animationen verwenden das bestehende NovaOS-Animationssystem.

Crystal besitzt standardmäßig eine feste Oberflächenform.

Ohne Zustands-, Beleuchtungs- oder Hintergrundänderungen darf keine kontinuierliche Neuberechnung erfolgen.

## 8. Hintergrundverarbeitung

Nova Crystal verwendet die Hintergrundabtastung des Nova Compositors.

Anforderungen:

- Die korrekte Ebenenreihenfolge muss berücksichtigt werden.
- Lichtbrechungsbedingte Abtastbereiche werden bei der Damage-Berechnung berücksichtigt.
- Hintergrundänderungen invalidieren abhängige Materialbereiche.
- Unveränderte Hintergrundinformationen können wiederverwendet werden.
- Geschützte Bildinhalte dürfen nicht durch Lichtbrechung offengelegt werden.
- Zyklische Hintergrundabhängigkeiten müssen ausgeschlossen werden.

## 9. Integration

Nova Crystal kann innerhalb des nativen Nova UI-Systems verwendet werden.

Typische Einsatzbereiche:

- Schaltflächen und interaktive Controls
- Fensterdekorationen
- Symbolhintergründe
- Schwebende Bedienelemente
- Dialogelemente
- Statusanzeigen
- Dekorative Oberflächen
- Systemüberlagerungen

Die Materialzuweisung erfolgt über das Nova Material System und das Nova UI Style System.

## 10. Performance

Nova Crystal muss die vorhandene Rendering-Infrastruktur effizient verwenden.

Verbindliche Anforderungen:

- GPU-Beschleunigung wird bevorzugt.
- Lichtbrechung und Reflexionen werden nur bei Bedarf berechnet.
- Unveränderte Materialbereiche verwenden zwischengespeicherte Ergebnisse.
- Effektbedingte Abtastbereiche werden begrenzt.
- Unsichtbare Materialien werden nicht verarbeitet.
- Kompatible Effektoperationen können zusammengefasst werden.
- Temporäre Rendering-Ressourcen werden wiederverwendet.
- Unnötige CPU-GPU-Datenübertragungen werden vermieden.

Die Verarbeitung muss mit dem Damage Tracking des Nova Compositors kompatibel sein.

## 11. Qualitätsstufen und Fallback

Nova Crystal unterstützt unterschiedliche Darstellungsqualitäten.

Definierte Qualitätsstufen:

1. **Full:** Lichtbrechung, Reflexionen, Glanzlichter und chromatische Dispersion.
2. **Balanced:** Vereinfachte Lichtbrechung und reduzierte Reflexionsberechnung.
3. **Reduced:** Transparenz, vereinfachte Glaskanten und statische Glanzlichter.
4. **Fallback:** Darstellung über Nova Acrylic oder Nova Solid.

Die Qualitätsstufe kann abhängig von Hardwareleistung, Energiezustand und Systemeinstellungen angepasst werden.

Ein Qualitätswechsel darf keine Neuerstellung des zugehörigen UI-Elements erfordern.

## 12. Barrierefreiheit

Nova Crystal muss systemweite Einstellungen zur Barrierefreiheit berücksichtigen.

Anforderungen:

- Lichtbrechung und Dispersion müssen reduzierbar oder deaktivierbar sein.
- Transparenz muss unabhängig konfigurierbar bleiben.
- Texte und Bedienelemente müssen ausreichend kontrastreich dargestellt werden.
- Dekorative Reflexionen dürfen die Bedienbarkeit nicht beeinträchtigen.
- Bei deaktivierten visuellen Effekten muss eine funktional gleichwertige Darstellung erhalten bleiben.

## 13. Determinismus und Sicherheit

Nova Crystal muss bei identischen Eingaben und Rendering-Bedingungen reproduzierbare Ergebnisse innerhalb definierter numerischer Toleranzen liefern.

Anforderungen:

- Materialparameter müssen validiert werden.
- Effektberechnungen müssen definierte Wertebereiche einhalten.
- Fehlerhafte Effekte dürfen die Rendering-Pipeline nicht destabilisieren.
- Materialinstanzen dürfen keine Sicherheitsgrenzen des Compositors umgehen.
- Bei Rendering-Fehlern wird ein definiertes Fallback-Material verwendet.

## 14. Ergebnis

Nova Crystal stellt ein klares, hochwertiges Glasmaterial mit ausgeprägter Lichtbrechung, präzisen Reflexionen und optionaler chromatischer Dispersion bereit.

Im Gegensatz zu Nova Liquid Glass liegt der Schwerpunkt auf einer festen, kristallartigen Oberflächenstruktur ohne flüssige Verformungen.

Durch die Integration in die Nova Material Engine ermöglicht Crystal eine einheitliche, anpassbare und ressourcenschonende Darstellung transparenter Glasoberflächen innerhalb von NovaOS.