# NPSPEC-MATERIAL-REFLECTION-0001 – Nova Surface Reflections

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Material Engine

## 1. Zweck

Nova Surface Reflections definiert die Darstellung optischer Reflexionen auf Materialoberflächen innerhalb von NovaOS.

Der Effekt simuliert Licht- und Umgebungsreflexionen auf virtuellen Glas- und Kristalloberflächen.

Ziel ist eine realistische, dynamische und ressourcenschonende Reflexionsdarstellung ohne vollständige physikalische Strahlverfolgung.

## 2. Architektur

Nova Surface Reflections ist ein Effekt der Nova Material Engine und verwendet die bestehende Material Rendering Pipeline.

Die Zuständigkeiten sind:

- **Material System:** Definition der Reflexionseigenschaften.
- **Material Renderer:** Berechnung und Kombination der Reflexionseffekte.
- **Compositor:** Bereitstellung freigegebener Bild- und Umgebungsinformationen.
- **Graphics Engine:** Ausführung der Rendering-Operationen.

Reflexionen werden unabhängig von der Materialdefinition berechnet und können von mehreren Materialtypen verwendet werden.

## 3. Eigenschaften

Der Effekt unterstützt folgende Eigenschaften:

- **Reflection:** Intensität der Reflexionen.
- **Roughness:** Oberflächenrauheit.
- **Reflectivity:** Grundreflexionsvermögen.
- **Fresnel:** Blickwinkelabhängige Reflexionsverstärkung.
- **EdgeIntensity:** Intensität der Kantenreflexionen.
- **EnvironmentIntensity:** Einfluss der Umgebungsinformationen.
- **ReflectionTint:** Farbüberlagerung der Reflexionen.
- **NormalMap:** Optionale Oberflächennormalen.
- **Quality:** Qualitätsstufe der Berechnung.

Alle Eigenschaften müssen definierte Datentypen, Wertebereiche und Standardwerte besitzen.

Eine Reflection-Intensität von 0 deaktiviert den Effekt.

## 4. Reflexionsmodell

Nova Surface Reflections verwendet ein bildraumbasiertes, vereinfachtes Reflexionsmodell.

Verbindliche Anforderungen:

- Reflexionen berücksichtigen die virtuelle Oberflächengeometrie.
- Oberflächennormalen beeinflussen die Reflexionsrichtung.
- Die Oberflächenrauheit bestimmt die Schärfe der Reflexionen.
- Das Reflexionsvermögen beeinflusst die sichtbare Reflexionsintensität.
- Blickwinkelabhängige Reflexionen werden über einen Fresnel-Ansatz unterstützt.
- Reflexionen müssen mit Transparenz, Lichtbrechung und Farbüberlagerungen kombinierbar sein.

Die Darstellung muss ohne Raytracing-Hardware möglich sein.

## 5. Reflexionsquellen

Nova Surface Reflections unterstützt unterschiedliche Reflexionsquellen.

Vorgesehen sind:

- **Environment Reflection:** Reflexionen aus verfügbaren Umgebungsinformationen.
- **Procedural Reflection:** Berechnete Reflexionsmuster und Lichtverläufe.
- **Surface Reflection:** Reflexionen anhand virtueller Oberflächengeometrie.
- **Edge Reflection:** Verstärkte Reflexionen an Materialkanten.

Reflexionsquellen können kombiniert werden.

Die Verwendung tatsächlicher Bildinhalte anderer Oberflächen unterliegt den Sicherheitsrichtlinien des Nova Compositors.

## 6. Fresnel-Reflexion

Nova Surface Reflections unterstützt blickwinkelabhängige Reflexionsintensitäten.

Anforderungen:

- Flache Betrachtungswinkel können stärkere Reflexionen erzeugen.
- Die Intensität muss über Materialeigenschaften konfigurierbar sein.
- Virtuelle Oberflächennormalen bestimmen den lokalen Reflexionswinkel.
- Der Effekt muss auch bei vereinfachter Oberflächengeometrie funktionieren.
- Die Berechnung darf keine vollständige dreidimensionale Szenengeometrie voraussetzen.

## 7. Oberflächenrauheit

Die Oberflächenrauheit beeinflusst das Reflexionsverhalten.

Anforderungen:

- Glatte Oberflächen erzeugen schärfere Reflexionen.
- Raue Oberflächen erzeugen weichere, diffusere Reflexionen.
- Die Rauheit muss unabhängig von der Reflexionsintensität einstellbar sein.
- Statische Rauheitsinformationen dürfen zwischengespeichert werden.
- Vereinfachte Reflexionsverfahren dürfen bei reduzierter Qualität verwendet werden.

## 8. Dynamische Reflexionen

Reflexionseigenschaften können zur Laufzeit verändert werden.

Unterstützt werden:

- Änderungen der Reflexionsintensität
- Animierte Oberflächennormalen
- Dynamische Kantenreflexionen
- Änderungen der Umgebungsbeleuchtung
- Interaktionsabhängige Reflexionsänderungen
- Übergänge zwischen Materialzuständen

Animationen verwenden das bestehende NovaOS-Animationssystem.

Ohne relevante Änderungen darf keine kontinuierliche Neuberechnung erfolgen.

## 9. Integration

Nova Surface Reflections kann von verschiedenen Materialien verwendet werden.

Insbesondere:

- **Acrylic:** Dezente Oberflächen- und Kantenreflexionen.
- **Liquid Glass:** Dynamische Reflexionen auf verformbaren Glasoberflächen.
- **Crystal:** Scharfe Reflexionen mit ausgeprägtem Fresnel-Verhalten.
- **Frosted Glass:** Stark abgeschwächte, diffuse Reflexionen.

Die Kombination mit anderen Materialeffekten erfolgt über die Nova Material Rendering Pipeline.

## 10. Damage Tracking

Nova Surface Reflections muss mit dem Damage Tracking des Nova Compositors kompatibel sein.

Verbindliche Anforderungen:

- Änderungen der Reflexionsparameter invalidieren betroffene Materialbereiche.
- Änderungen verwendeter Umgebungsinformationen invalidieren abhängige Reflexionsbereiche.
- Unveränderte Reflexionsergebnisse dürfen wiederverwendet werden.
- Statische Reflexionsinformationen können zwischengespeichert werden.
- Unsichtbare Materialbereiche werden nicht verarbeitet.
- Änderungen außerhalb relevanter Reflexionsquellen dürfen keine unnötigen Neuberechnungen auslösen.

## 11. Performance

Die Reflexionsberechnung muss ressourcenschonend erfolgen.

Verbindliche Anforderungen:

- GPU-Beschleunigung wird bevorzugt.
- Statische Reflexionsinformationen dürfen vorberechnet werden.
- Kompatible Reflexionsoperationen können zusammengefasst werden.
- Wiederverwendbare Reflexionsdaten werden gemeinsam genutzt.
- Temporäre Rendering-Ressourcen werden nach Möglichkeit wiederverwendet.
- Unnötige CPU-GPU-Datenübertragungen werden vermieden.
- Die Berechnung bleibt auf sichtbare und benötigte Materialbereiche begrenzt.

## 12. Qualitätsstufen und Fallback

Nova Surface Reflections unterstützt folgende Qualitätsstufen:

1. **Full:** Dynamische Reflexionen mit Oberflächennormalen, Rauheit und Fresnel-Verhalten.
2. **Balanced:** Vereinfachte Reflexionsberechnung mit reduzierter Umgebungsabtastung.
3. **Reduced:** Statische Reflexionsverläufe und vereinfachte Kantenreflexionen.
4. **Fallback:** Materialdarstellung ohne Reflexionen.

Die Qualitätsstufe kann abhängig von Hardwareleistung, Energiezustand und Systemeinstellungen angepasst werden.

Ein Qualitätswechsel darf keine Neuerstellung des zugehörigen UI-Elements erfordern.

## 13. Barrierefreiheit

Nova Surface Reflections muss systemweite Einstellungen zur Barrierefreiheit berücksichtigen.

Anforderungen:

- Reflexionen müssen reduzierbar oder deaktivierbar sein.
- Dynamische Reflexionsbewegungen müssen abschaltbar sein.
- Reflexionen dürfen die Lesbarkeit von Texten und Bedienelementen nicht beeinträchtigen.
- Bei deaktivierten Effekten muss eine funktional gleichwertige Darstellung erhalten bleiben.

## 14. Determinismus und Sicherheit

Nova Surface Reflections muss bei identischen Eingaben und Rendering-Bedingungen reproduzierbare Ergebnisse innerhalb definierter numerischer Toleranzen liefern.

Anforderungen:

- Reflexionsparameter müssen validiert und begrenzt werden.
- Ungültige Oberflächennormalen müssen kontrolliert behandelt werden.
- Reflexionsquellen dürfen keine Sicherheitsgrenzen des Compositors umgehen.
- Anwendungen dürfen keine unberechtigten Bildinformationen anderer Oberflächen erhalten.
- Fehlerhafte Reflexionsberechnungen dürfen die Rendering-Pipeline nicht destabilisieren.
- Bei Berechnungsfehlern wird der Reflexionseffekt deaktiviert oder ein definiertes Fallback verwendet.

## 15. Ergebnis

Nova Surface Reflections stellt eine einheitliche Reflexionsverarbeitung für die Nova Material Engine bereit.

Durch Oberflächennormalen, Rauheit, Fresnel-Verhalten und unterschiedliche Reflexionsquellen entstehen anpassbare Glas- und Kristallreflexionen.

Die Integration in die bestehende Grafikarchitektur ermöglicht hochwertige Reflexionseffekte bei kontrolliertem Ressourcenverbrauch.