# NPSPEC-MATERIAL-ACRYLIC-0001 – Nova Acrylic Material

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Material Engine

## 1. Zweck

Nova Acrylic definiert ein halbtransparentes Oberflächenmaterial mit Hintergrundunschärfe, dezenter Farbüberlagerung und leichten Lichtreflexionen.

Es ermöglicht moderne, ruhige und visuell abgestimmte Benutzeroberflächen bei geringem Ressourcenverbrauch.

## 2. Materialeigenschaften

Nova Acrylic verwendet das gemeinsame Materialmodell der Nova Material Engine.

Unterstützte Eigenschaften:

- **Color:** Grundfarbe des Materials.
- **Opacity:** Deckkraft der Oberfläche.
- **Tint:** Farbüberlagerung des Hintergrundes.
- **Blur:** Stärke der Hintergrundunschärfe.
- **Saturation:** Farbsättigung des Hintergrundes.
- **Noise:** Dezente Oberflächenstruktur zur Reduzierung von Banding.
- **Reflection:** Intensität leichter Lichtreflexionen.
- **Highlight:** Stärke der Kanten- und Glanzlichter.

Alle Eigenschaften müssen individuell konfigurierbar sein.

## 3. Darstellung

Das Acrylic-Material kombiniert Hintergrundunschärfe mit einer transparenten Farbüberlagerung.

Verbindliche Anforderungen:

- Der Hintergrund wird innerhalb des Materialbereichs weichgezeichnet.
- Die Unschärfe muss unabhängig von der Deckkraft einstellbar sein.
- Farbüberlagerungen dürfen die Lesbarkeit nicht beeinträchtigen.
- Reflexionen und Glanzlichter müssen dezent bleiben.
- Abgerundete Ecken und individuelle Masken werden unterstützt.
- Materialeffekte dürfen nicht außerhalb der definierten Oberfläche sichtbar werden.

Acrylic verwendet standardmäßig keine Lichtbrechung oder dynamische Oberflächenverformung.

## 4. Hintergrundverarbeitung

Nova Acrylic verwendet die Hintergrundabtastung des Nova Compositors.

Anforderungen:

- Die Hintergrundabtastung berücksichtigt die korrekte Ebenenreihenfolge.
- Der Blur-Radius bestimmt den benötigten erweiterten Abtastbereich.
- Änderungen des Hintergrundes invalidieren betroffene Materialbereiche.
- Unveränderte Hintergrundinformationen können zwischengespeichert werden.
- Geschützte Bildinhalte müssen bestehende Sicherheitsrichtlinien einhalten.

## 5. Dynamisches Verhalten

Acrylic unterstützt animierbare Materialeigenschaften.

Dazu gehören:

- Transparenzänderungen
- Anpassungen der Unschärfe
- Farb- und Helligkeitsübergänge
- Dezente Reflexionsänderungen
- Übergänge zwischen Materialzuständen

Animationen verwenden das bestehende NovaOS-Animationssystem.

Ohne Zustands- oder Hintergrundänderungen darf Acrylic keine kontinuierliche Neuberechnung verursachen.

## 6. Integration

Nova Acrylic kann systemweit verwendet werden.

Typische Einsatzbereiche:

- Fensterhintergründe
- Taskbar und Startmenü
- Kontextmenüs
- Dialogfenster
- Seitenleisten
- Benachrichtigungen
- Schwebende Bedienelemente

Die Materialzuweisung erfolgt über das Nova Material System und das Nova UI Style System.

## 7. Performance

Nova Acrylic muss ressourcenschonend dargestellt werden.

Verbindliche Anforderungen:

- Hintergrundunschärfe wird nach Möglichkeit GPU-beschleunigt berechnet.
- Unveränderte Materialbereiche werden nicht erneut verarbeitet.
- Zwischenergebnisse können wiederverwendet werden.
- Blur-Berechnungen müssen auf benötigte Bereiche begrenzt bleiben.
- Unsichtbare Acrylic-Oberflächen werden nicht gerendert.
- Die Effektqualität kann abhängig von verfügbaren Ressourcen reduziert werden.

## 8. Fallback und Barrierefreiheit

Bei eingeschränkter Grafikleistung oder deaktivierten Transparenzeffekten muss Acrylic auf eine vereinfachte Darstellung zurückfallen können.

Fallback-Stufen:

1. Vollständiges Acrylic mit Blur, Transparenz und Reflexionen.
2. Vereinfachtes Acrylic mit reduziertem Blur.
3. Transparente Farbfläche ohne Blur.
4. Undurchsichtige Solid-Oberfläche.

Die Darstellung muss ausreichende Kontraste für Texte und Bedienelemente gewährleisten.

Systemweite Einstellungen zur Reduzierung von Transparenz und Animationen müssen berücksichtigt werden.

## 9. Ergebnis

Nova Acrylic stellt ein leichtgewichtiges, halbtransparentes Oberflächenmaterial für NovaOS bereit.

Durch Hintergrundunschärfe, dezente Farbüberlagerungen und leichte Reflexionen entsteht eine moderne und zurückhaltende Glasoptik.

Die Integration in die Nova Material Engine ermöglicht eine einheitliche, anpassbare und ressourcenschonende Darstellung.