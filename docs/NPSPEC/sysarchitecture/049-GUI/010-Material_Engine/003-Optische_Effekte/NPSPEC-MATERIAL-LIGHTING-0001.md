# NPSPEC-MATERIAL-LIGHTING-0001 – Nova Dynamic Lighting

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Material Engine

## 1. Zweck

Nova Dynamic Lighting definiert die dynamische Beleuchtung visueller Oberflächenmaterialien innerhalb von NovaOS.

Der Effekt ermöglicht realistische Lichtreaktionen, Helligkeitsverläufe und beleuchtungsabhängige Materialeigenschaften.

Ziel ist eine hochwertige, anpassbare und ressourcenschonende Beleuchtung ohne vollständige physikalische Lichtsimulation.

## 2. Architektur

Nova Dynamic Lighting ist ein Effekt der Nova Material Engine und verwendet die bestehende Material Rendering Pipeline.

Die Zuständigkeiten sind:

- **Material System:** Definition der Beleuchtungseigenschaften.
- **Material Renderer:** Berechnung der materialabhängigen Lichtreaktionen.
- **Graphics Engine:** Ausführung der Beleuchtungsoperationen.
- **Compositor:** Integration der beleuchteten Materialien in die endgültige Bildkomposition.
- **Nova Theme System:** Bereitstellung systemweiter Beleuchtungsvorgaben.

Dynamic Lighting implementiert keinen eigenständigen Renderer.

## 3. Eigenschaften

Der Effekt unterstützt folgende Eigenschaften:

- **LightIntensity:** Intensität der Beleuchtung.
- **LightColor:** Farbe des einfallenden Lichts.
- **LightDirection:** Richtung des Lichteinfalls.
- **LightPosition:** Virtuelle Position der Lichtquelle.
- **AmbientLight:** Stärke der Umgebungsbeleuchtung.
- **DiffuseIntensity:** Intensität der diffusen Beleuchtung.
- **SpecularIntensity:** Intensität gerichteter Lichtreflexionen.
- **LightFalloff:** Abnahme der Lichtintensität mit der Entfernung.
- **LightResponse:** Stärke der materialabhängigen Lichtreaktion.
- **NormalMap:** Optionale Oberflächennormalen.

Alle Eigenschaften müssen definierte Datentypen, Wertebereiche und Standardwerte besitzen.

## 4. Beleuchtungsmodell

Nova Dynamic Lighting verwendet ein vereinfachtes, materialbasiertes Beleuchtungsmodell.

Verbindliche Anforderungen:

- Beleuchtung berücksichtigt die virtuelle Oberflächengeometrie.
- Oberflächennormalen beeinflussen die Lichtverteilung.
- Materialien können unterschiedlich auf dieselbe Lichtquelle reagieren.
- Diffuse und gerichtete Lichtanteile müssen unabhängig konfigurierbar sein.
- Beleuchtung muss mit Transparenz, Reflexionen und Lichtbrechung kombinierbar sein.
- Die Berechnung darf keine vollständige dreidimensionale Szenengeometrie voraussetzen.

## 5. Lichtquellen

Nova Dynamic Lighting unterstützt folgende virtuelle Lichtquellen:

- **Ambient Light:** Gleichmäßige Umgebungsbeleuchtung.
- **Directional Light:** Gerichtetes Licht ohne entfernungsabhängige Abschwächung.
- **Point Light:** Punktförmige Lichtquelle mit definierter Reichweite.
- **Area Light:** Vereinfachte flächige Beleuchtung.

Mehrere Lichtquellen können kombiniert werden.

Die Anzahl gleichzeitig ausgewerteter Lichtquellen muss begrenzbar sein.

## 6. Materialabhängige Beleuchtung

Die Lichtreaktion wird durch die Eigenschaften des jeweiligen Materials bestimmt.

Vorgesehen sind:

- **Acrylic:** Dezente Beleuchtung und weiche Lichtverläufe.
- **Liquid Glass:** Dynamische Lichtreaktionen auf verformbaren Glasoberflächen.
- **Frosted Glass:** Diffuse und gleichmäßig gestreute Beleuchtung.
- **Crystal:** Ausgeprägte Lichtreaktionen an Oberflächen und Glaskanten.
- **Solid:** Optionale einfache Helligkeits- und Farbmodulation.

Die Material Engine muss eine einheitliche Beleuchtungsverarbeitung bereitstellen.

## 7. Dynamische Beleuchtung

Beleuchtungsparameter können zur Laufzeit verändert werden.

Unterstützt werden:

- Bewegliche Lichtquellen
- Änderungen der Lichtintensität
- Farb- und Helligkeitsübergänge
- Interaktionsabhängige Beleuchtung
- Änderungen der virtuellen Oberflächenform
- Übergänge zwischen Beleuchtungszuständen

Animationen verwenden das bestehende NovaOS-Animationssystem.

Ohne Änderungen der Lichtquellen, Materialeigenschaften oder Oberflächengeometrie darf keine kontinuierliche Neuberechnung erfolgen.

## 8. Integration

Nova Dynamic Lighting kann systemweit verwendet werden.

Typische Einsatzbereiche:

- Fensterdekorationen
- Schaltflächen und interaktive Controls
- Taskbar und Startmenü
- Dialogfenster
- Schwebende Bedienelemente
- Systemüberlagerungen
- Dekorative Oberflächen

Die Beleuchtung muss mit dem Nova Material System, dem Nova UI Style System und dem Nova Theme System kompatibel sein.

## 9. Damage Tracking

Nova Dynamic Lighting muss das bestehende Damage Tracking des Nova Compositors verwenden.

Verbindliche Anforderungen:

- Änderungen von Lichtquellen invalidieren ausschließlich betroffene Materialbereiche.
- Änderungen der Oberflächengeometrie invalidieren abhängige Beleuchtungsbereiche.
- Statische Beleuchtungsergebnisse dürfen zwischengespeichert werden.
- Unveränderte Materialbereiche werden nicht erneut verarbeitet.
- Unsichtbare Materialien werden nicht beleuchtet.
- Bewegliche Lichtquellen müssen ihre bisherigen und neuen Wirkungsbereiche berücksichtigen.

Eine permanente vollständige Bildschirmaktualisierung ist nicht zulässig.

## 10. Performance

Die Beleuchtungsberechnung muss ressourcenschonend erfolgen.

Verbindliche Anforderungen:

- GPU-Beschleunigung wird bevorzugt.
- Statische Beleuchtungsinformationen dürfen vorberechnet werden.
- Gleichartige Beleuchtungsoperationen können zusammengefasst werden.
- Die Anzahl ausgewerteter Lichtquellen wird auf notwendige Quellen begrenzt.
- Wiederverwendbare Lichtinformationen werden gemeinsam genutzt.
- Temporäre Rendering-Ressourcen werden nach Möglichkeit wiederverwendet.
- Unnötige CPU-GPU-Datenübertragungen werden vermieden.

## 11. Qualitätsstufen und Fallback

Nova Dynamic Lighting unterstützt folgende Qualitätsstufen:

1. **Full:** Dynamische Beleuchtung mit mehreren Lichtquellen und Oberflächennormalen.
2. **Balanced:** Vereinfachte Beleuchtung mit begrenzter Anzahl aktiver Lichtquellen.
3. **Reduced:** Statische Lichtverläufe und einfache Helligkeitsmodulation.
4. **Fallback:** Materialdarstellung ohne dynamische Beleuchtung.

Die Qualitätsstufe kann abhängig von Hardwareleistung, Energiezustand und Systemeinstellungen angepasst werden.

Ein Qualitätswechsel darf keine Neuerstellung des zugehörigen UI-Elements erfordern.

## 12. Barrierefreiheit

Nova Dynamic Lighting muss systemweite Einstellungen zur Barrierefreiheit berücksichtigen.

Anforderungen:

- Dynamische Beleuchtung muss reduzierbar oder deaktivierbar sein.
- Schnelle Helligkeitswechsel und störende Lichtanimationen müssen vermeidbar sein.
- Beleuchtung darf die Lesbarkeit von Texten und Bedienelementen nicht beeinträchtigen.
- Systemweite Kontrast- und Farbeinstellungen müssen berücksichtigt werden.
- Bei deaktivierten Effekten muss eine funktional gleichwertige Darstellung erhalten bleiben.

## 13. Determinismus und Sicherheit

Nova Dynamic Lighting muss bei identischen Eingaben und Rendering-Bedingungen reproduzierbare Ergebnisse innerhalb definierter numerischer Toleranzen liefern.

Anforderungen:

- Beleuchtungsparameter müssen validiert und begrenzt werden.
- Ungültige Lichtquellen und Oberflächennormalen müssen kontrolliert behandelt werden.
- Die Anzahl und Komplexität von Lichtquellen muss begrenzbar sein.
- Beleuchtungsoperationen dürfen keine Sicherheitsgrenzen des Compositors umgehen.
- Fehlerhafte Berechnungen dürfen die Rendering-Pipeline nicht destabilisieren.
- Bei Berechnungsfehlern wird ein definiertes Fallback verwendet.

## 14. Ergebnis

Nova Dynamic Lighting stellt eine einheitliche, dynamische Beleuchtungsverarbeitung für die Nova Material Engine bereit.

Durch virtuelle Lichtquellen, Oberflächennormalen und materialabhängige Lichtreaktionen entstehen hochwertige Beleuchtungseffekte.

Die Integration in die bestehende Grafikarchitektur ermöglicht dynamische Lichtdarstellungen bei kontrolliertem Ressourcenverbrauch.