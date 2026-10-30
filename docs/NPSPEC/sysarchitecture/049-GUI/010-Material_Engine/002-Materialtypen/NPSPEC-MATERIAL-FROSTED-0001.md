# NPSPEC-MATERIAL-FROSTED-0001 – Nova Frosted Glass Material

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Material Engine

## 1. Zweck

Nova Frosted Glass definiert ein halbtransparentes Milchglasmaterial mit ausgeprägter Hintergrundunschärfe und diffuser Lichtstreuung.

Das Material ermöglicht ruhige, kontrastreiche Oberflächen, bei denen Hintergrundstrukturen sichtbar bleiben, ohne die Lesbarkeit der Vordergrundinhalte zu beeinträchtigen.

## 2. Materialeigenschaften

Nova Frosted Glass verwendet das gemeinsame Materialmodell der Nova Material Engine.

Unterstützte Eigenschaften:

- **Color:** Grundfarbe des Materials.
- **Opacity:** Deckkraft der Oberfläche.
- **Tint:** Farbüberlagerung.
- **Blur:** Stärke der Hintergrundunschärfe.
- **Saturation:** Farbsättigung des Hintergrundes.
- **Diffusion:** Intensität der diffusen Lichtstreuung.
- **Roughness:** Virtuelle Oberflächenrauheit.
- **Noise:** Dezente Oberflächenstruktur.
- **Brightness:** Helligkeitsanpassung des Hintergrundes.

Alle Eigenschaften müssen individuell konfigurierbar sein.

## 3. Darstellung

Frosted Glass kombiniert starke Hintergrundunschärfe mit diffuser Lichtstreuung und einer halbtransparenten Farbüberlagerung.

Verbindliche Anforderungen:

- Hintergrundstrukturen werden weichgezeichnet.
- Die Unschärfe muss unabhängig von der Deckkraft einstellbar sein.
- Diffuse Lichtstreuung erzeugt eine gleichmäßige Milchglasoptik.
- Farbüberlagerungen müssen die Lesbarkeit unterstützen.
- Abgerundete Ecken und individuelle Masken werden unterstützt.
- Materialeffekte dürfen nicht außerhalb ihrer definierten Ausgabebereiche sichtbar werden.

Frosted Glass verwendet standardmäßig keine ausgeprägte Lichtbrechung oder dynamische Oberflächenverformung.

## 4. Hintergrundverarbeitung

Die Hintergrundverarbeitung erfolgt über den Nova Compositor.

Anforderungen:

- Hintergrunddaten berücksichtigen die korrekte Ebenenreihenfolge.
- Der Blur-Radius bestimmt den erweiterten Abtastbereich.
- Hintergrundänderungen invalidieren betroffene Materialbereiche.
- Unveränderte Hintergrundinformationen können wiederverwendet werden.
- Geschützte Bildinhalte müssen bestehende Sicherheitsrichtlinien einhalten.

## 5. Lichtstreuung

Die diffuse Lichtstreuung bildet neben der Hintergrundunschärfe den charakteristischen Effekt von Frosted Glass.

Anforderungen:

- Die Streuungsintensität wird über Diffusion gesteuert.
- Die Streuung muss mit Transparenz und Farbüberlagerung kombinierbar sein.
- Die Oberflächenrauheit kann das Streuungsverhalten beeinflussen.
- Statische Streuungseffekte dürfen zwischengespeichert werden.
- Eine physikalisch vollständige Lichtsimulation ist nicht erforderlich.

## 6. Dynamisches Verhalten

Frosted Glass unterstützt animierbare Materialeigenschaften.

Dazu gehören:

- Transparenzänderungen
- Anpassungen der Hintergrundunschärfe
- Farb- und Helligkeitsübergänge
- Änderungen der Lichtstreuung
- Übergänge zwischen Materialzuständen

Animationen verwenden das bestehende NovaOS-Animationssystem.

Ohne Zustands- oder Hintergrundänderungen darf keine kontinuierliche Neuberechnung erfolgen.

## 7. Integration

Nova Frosted Glass kann systemweit verwendet werden.

Typische Einsatzbereiche:

- Dialogfenster
- Seitenleisten
- Einstellungsbereiche
- Benachrichtigungen
- Sperrbildschirm
- Hintergrundflächen von Bedienelementen
- Systemüberlagerungen

Die Materialzuweisung erfolgt über das Nova Material System und das Nova UI Style System.

## 8. Performance

Nova Frosted Glass muss ressourcenschonend dargestellt werden.

Verbindliche Anforderungen:

- Hintergrundunschärfe wird nach Möglichkeit GPU-beschleunigt.
- Unveränderte Materialbereiche werden nicht erneut verarbeitet.
- Zwischenergebnisse können wiederverwendet werden.
- Blur-Berechnungen bleiben auf benötigte Abtastbereiche begrenzt.
- Unsichtbare Materialbereiche werden nicht gerendert.
- Die Effektqualität kann abhängig von verfügbaren Ressourcen angepasst werden.

Die Verarbeitung muss mit dem Damage Tracking des Nova Compositors kompatibel sein.

## 9. Fallback und Barrierefreiheit

Frosted Glass unterstützt folgende Darstellungsstufen:

1. **Full:** Hintergrundunschärfe, diffuse Lichtstreuung und Farbüberlagerung.
2. **Balanced:** Reduzierte Unschärfe und vereinfachte Lichtstreuung.
3. **Reduced:** Halbtransparente Farbfläche ohne aufwendige Streuung.
4. **Fallback:** Undurchsichtige Nova Solid-Oberfläche.

Systemweite Einstellungen zur Reduzierung von Transparenz und Animationen müssen berücksichtigt werden.

Texte und Bedienelemente müssen unabhängig von der Hintergrunddarstellung ausreichend kontrastreich bleiben.

## 10. Ergebnis

Nova Frosted Glass stellt ein halbtransparentes Milchglasmaterial mit ausgeprägter Hintergrundunschärfe und diffuser Lichtstreuung bereit.

Durch die Integration in die Nova Material Engine entsteht eine einheitliche, anpassbare und ressourcenschonende Darstellung.

Das Material eignet sich besonders für ruhige Benutzeroberflächen, bei denen Lesbarkeit und visuelle Trennung im Vordergrund stehen.