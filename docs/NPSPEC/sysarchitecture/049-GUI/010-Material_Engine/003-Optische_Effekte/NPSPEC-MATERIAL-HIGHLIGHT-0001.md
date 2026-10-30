# NPSPEC-MATERIAL-HIGHLIGHT-0001 – Nova Specular Highlights

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Material Engine

## 1. Zweck

Nova Specular Highlights definiert die Darstellung gerichteter Glanzlichter auf virtuellen Materialoberflächen innerhalb von NovaOS.

Der Effekt simuliert die Reflexion von Lichtquellen auf glatten, rauen oder gewölbten Oberflächen.

Ziel ist eine hochwertige, dynamische und ressourcenschonende Darstellung von Glanzlichtern und beleuchteten Materialkanten.

## 2. Architektur

Nova Specular Highlights ist ein Effekt der Nova Material Engine und verwendet die bestehende Material Rendering Pipeline.

Die Zuständigkeiten sind:

- **Material System:** Definition der Glanzlichteigenschaften.
- **Material Renderer:** Berechnung und Kombination der Glanzlichter.
- **Dynamic Lighting:** Bereitstellung virtueller Lichtquellen.
- **Surface Reflections:** Abstimmung reflektierter Lichtanteile.
- **Graphics Engine:** Ausführung der Rendering-Operationen.

Specular Highlights implementiert keinen eigenständigen Renderer.

## 3. Eigenschaften

Der Effekt unterstützt folgende Eigenschaften:

- **HighlightIntensity:** Intensität der Glanzlichter.
- **HighlightColor:** Farbe der Glanzlichter.
- **HighlightSharpness:** Schärfe der Glanzlichter.
- **HighlightSize:** Ausdehnung der Glanzlichter.
- **Roughness:** Oberflächenrauheit.
- **SpecularPower:** Konzentration der gerichteten Lichtreflexion.
- **EdgeHighlight:** Intensität der Kantenglanzlichter.
- **Fresnel:** Blickwinkelabhängige Verstärkung.
- **NormalMap:** Optionale Oberflächennormalen.
- **LightResponse:** Reaktion auf Beleuchtungsänderungen.

Alle Eigenschaften müssen definierte Datentypen, Wertebereiche und Standardwerte besitzen.

Eine HighlightIntensity von 0 deaktiviert den Effekt.

## 4. Glanzlichtmodell

Nova Specular Highlights verwendet ein vereinfachtes, oberflächenbasiertes Beleuchtungsmodell.

Verbindliche Anforderungen:

- Glanzlichter berücksichtigen die virtuelle Oberflächengeometrie.
- Oberflächennormalen beeinflussen Position und Intensität.
- Die Oberflächenrauheit bestimmt die Schärfe und Ausdehnung.
- Glanzlichter können blickwinkelabhängig berechnet werden.
- Mehrere Lichtquellen können berücksichtigt werden.
- Glanzlichter müssen mit Transparenz, Lichtbrechung und Reflexionen kombinierbar sein.

Eine vollständige physikalische Lichtsimulation ist nicht erforderlich.

## 5. Glanzlichttypen

Nova Specular Highlights unterstützt folgende Darstellungsformen:

- **Surface Highlight:** Gerichtetes Glanzlicht auf der Materialoberfläche.
- **Edge Highlight:** Lichtbetonung entlang der Materialkanten.
- **Soft Highlight:** Weiches, großflächiges Glanzlicht.
- **Sharp Highlight:** Kleines, intensiv reflektiertes Glanzlicht.

Die Darstellungsformen können kombiniert werden.

## 6. Kantenbeleuchtung

Kantenglanzlichter unterstützen die räumliche Wahrnehmung transparenter Materialien.

Anforderungen:

- Kanten werden anhand der Materialgeometrie oder definierter Masken ermittelt.
- Die Intensität muss unabhängig von zentralen Glanzlichtern konfigurierbar sein.
- Abgerundete und individuell geformte Oberflächen werden unterstützt.
- Die Kantenbeleuchtung darf keine sichtbaren Artefakte an Materialgrenzen erzeugen.
- Glanzlichter müssen innerhalb definierter Clipping-Bereiche bleiben.

## 7. Dynamisches Verhalten

Glanzlichteigenschaften können zur Laufzeit verändert werden.

Unterstützt werden:

- Bewegliche Glanzlichter
- Änderungen der Lichtintensität
- Farb- und Helligkeitsübergänge
- Interaktionsabhängige Lichtreaktionen
- Änderungen der Oberflächengeometrie
- Übergänge zwischen Materialzuständen

Animationen verwenden das bestehende NovaOS-Animationssystem.

Ohne relevante Änderungen darf keine kontinuierliche Neuberechnung erfolgen.

## 8. Materialintegration

Nova Specular Highlights kann von verschiedenen Materialien verwendet werden.

- **Acrylic:** Dezente, weiche Glanzlichter.
- **Liquid Glass:** Dynamische Glanzlichter auf verformbaren Oberflächen.
- **Frosted Glass:** Schwache, diffuse Glanzlichter.
- **Crystal:** Scharfe und ausgeprägte Glanzlichter.
- **Solid:** Optionale dekorative Lichtakzente.

Die Effektkombination erfolgt über die Nova Material Rendering Pipeline.

## 9. Damage Tracking

Nova Specular Highlights muss mit dem Damage Tracking des Nova Compositors kompatibel sein.

Verbindliche Anforderungen:

- Änderungen der Lichtquellen invalidieren betroffene Materialbereiche.
- Änderungen der Oberflächengeometrie invalidieren abhängige Glanzlichter.
- Statische Glanzlichtinformationen dürfen zwischengespeichert werden.
- Unveränderte Bereiche werden nicht erneut verarbeitet.
- Unsichtbare Materialbereiche werden nicht berechnet.
- Bewegliche Glanzlichter müssen ihre bisherigen und neuen Wirkungsbereiche berücksichtigen.

## 10. Performance

Die Glanzlichtberechnung muss ressourcenschonend erfolgen.

Verbindliche Anforderungen:

- GPU-Beschleunigung wird bevorzugt.
- Statische Glanzlichtinformationen können vorberechnet werden.
- Gleichartige Berechnungen können zusammengefasst werden.
- Vorhandene Oberflächennormalen und Beleuchtungsinformationen werden wiederverwendet.
- Die Berechnung bleibt auf sichtbare Materialbereiche begrenzt.
- Unnötige temporäre Rendering-Buffer werden vermieden.

## 11. Qualitätsstufen und Fallback

Nova Specular Highlights unterstützt folgende Qualitätsstufen:

1. **Full:** Dynamische Glanzlichter mit Oberflächennormalen, Rauheit und Fresnel-Verhalten.
2. **Balanced:** Vereinfachte gerichtete Glanzlichter.
3. **Reduced:** Statische Lichtverläufe und Kantenbetonungen.
4. **Fallback:** Materialdarstellung ohne Glanzlichter.

Die Qualitätsstufe kann abhängig von Hardwareleistung, Energiezustand und Systemeinstellungen angepasst werden.

## 12. Barrierefreiheit

Nova Specular Highlights muss systemweite Einstellungen zur Barrierefreiheit berücksichtigen.

Anforderungen:

- Glanzlichter müssen reduzierbar oder deaktivierbar sein.
- Starke Helligkeitswechsel müssen vermeidbar sein.
- Glanzlichter dürfen Texte und Bedienelemente nicht überstrahlen.
- Systemweite Kontrast- und Animationseinstellungen müssen berücksichtigt werden.

## 13. Determinismus und Sicherheit

Nova Specular Highlights muss bei identischen Eingaben und Rendering-Bedingungen reproduzierbare Ergebnisse innerhalb definierter numerischer Toleranzen liefern.

Anforderungen:

- Effektparameter müssen validiert und begrenzt werden.
- Ungültige Oberflächennormalen müssen kontrolliert behandelt werden.
- Glanzlichtberechnungen dürfen keine Sicherheitsgrenzen des Compositors umgehen.
- Fehlerhafte Berechnungen dürfen die Rendering-Pipeline nicht destabilisieren.
- Bei Berechnungsfehlern wird der Effekt deaktiviert oder ein definiertes Fallback verwendet.

## 14. Ergebnis

Nova Specular Highlights stellt eine einheitliche Verarbeitung gerichteter Glanzlichter innerhalb der Nova Material Engine bereit.

Durch Oberflächennormalen, Rauheit, Fresnel-Verhalten und dynamische Lichtquellen entstehen realistische Lichtakzente und präzise Glaskanten.

Die Integration in die bestehende Material Rendering Pipeline ermöglicht hochwertige Glanzlichteffekte bei kontrolliertem Ressourcenverbrauch.