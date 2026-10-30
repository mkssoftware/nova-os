# NPSPEC-MATERIAL-TRANSITION-0001 – Nova Material Transitions

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Material Engine

## 1. Zweck

Nova Material Transitions definiert den kontrollierten Übergang zwischen unterschiedlichen Materialien, Materialeigenschaften und Darstellungszuständen innerhalb von NovaOS.

Das System ermöglicht flüssige Wechsel zwischen Acrylic, Liquid Glass, Frosted Glass, Crystal und Solid.

Ziel ist eine visuell konsistente, unterbrechbare und ressourcenschonende Materialtransformation ohne Neuerstellung der zugehörigen UI-Elemente.

## 2. Architektur

Nova Material Transitions ist Bestandteil der Nova Material Engine und verwendet das bestehende NovaOS Animation System.

Die Zuständigkeiten sind:

- **Material System:** Bereitstellung der Ausgangs- und Zielmaterialien.
- **Material Transitions:** Bestimmung der Übergangsstrategie und des resultierenden Materialzustands.
- **Animation System:** Zeitsteuerung und Interpolation.
- **Material Renderer:** Darstellung des Übergangsergebnisses.
- **Compositor:** Komposition und Damage Tracking.
- **Graphics Engine:** Ausführung der Rendering-Operationen.

Material Transitions implementiert keine eigenständige Animationslaufzeit oder Rendering-Pipeline.

## 3. Eigenschaften

Materialübergänge unterstützen folgende Eigenschaften:

- **SourceMaterial:** Ausgangsmaterial.
- **TargetMaterial:** Zielmaterial.
- **TransitionMode:** Verwendetes Übergangsverfahren.
- **Duration:** Dauer des Übergangs.
- **Easing:** Interpolationsverhalten.
- **Progress:** Fortschritt zwischen 0 und 1.
- **BlendMode:** Verfahren zur visuellen Materialmischung.
- **InterruptMode:** Verhalten bei Unterbrechungen.
- **Reversible:** Unterstützung umkehrbarer Übergänge.

Alle Eigenschaften müssen definierte Datentypen, Wertebereiche und Standardwerte besitzen.

## 4. Übergangsmodell

Materialübergänge verändern den visuellen Laufzeitzustand einer Materialinstanz.

Verbindliche Anforderungen:

- Ausgangs- und Zielmaterial müssen eindeutig definiert sein.
- Gemeinsame Materialdefinitionen bleiben unverändert.
- Der Übergangsfortschritt wird durch das NovaOS Animation System gesteuert.
- Materialwechsel dürfen keine Neuerstellung des UI-Elements erfordern.
- Übergänge müssen unterbrechbar und umkehrbar sein.
- Nach Abschluss muss ausschließlich der definierte Zielzustand aktiv bleiben.
- Die funktionale Bedienbarkeit darf durch einen Materialwechsel nicht beeinträchtigt werden.

## 5. Übergangsverfahren

Nova Material Transitions unterstützt folgende Verfahren:

- **Property Interpolation:** Direkte Interpolation kompatibler Materialeigenschaften.
- **Crossfade:** Überblendung zweier Materialdarstellungen.
- **Hybrid:** Kombination aus Eigenschaftsinterpolation und Überblendung.
- **Instant:** Unmittelbarer Materialwechsel ohne Animation.

Die Übergangsstrategie wird anhand der Materialkompatibilität und der konfigurierten Darstellungsvorgaben ausgewählt.

## 6. Eigenschaftsinterpolation

Kompatible Materialeigenschaften können direkt interpoliert werden.

Dazu gehören:

- Transparenz und Deckkraft
- Materialfarben und Farbverläufe
- Hintergrundunschärfe
- Reflexionsintensität
- Lichtbrechungsintensität
- Glanzlichter und Beleuchtung
- Oberflächenverzerrungen
- Verformungsparameter

Anforderungen:

- Interpolationen müssen typsicher erfolgen.
- Eigenschaften ohne kompatible Interpolation verwenden definierte Umschalt- oder Überblendungsverfahren.
- Fehlende Eigenschaften erhalten definierte neutrale Werte oder werden überblendet.
- Ungültige Zwischenwerte müssen verhindert werden.
- Die Interpolation muss mit den Wertebereichen der beteiligten Materialeffekte kompatibel sein.

## 7. Materialüberblendung

Bei inkompatiblen Materialstrukturen kann eine visuelle Überblendung erfolgen.

Verbindliche Anforderungen:

- Ausgangs- und Zielmaterial können während des Übergangs gleichzeitig ausgewertet werden.
- Die Überblendung muss Alpha-Komposition und Farbmanagement berücksichtigen.
- Doppelte Anwendung von Transparenz oder Hintergrundeffekten muss vermieden werden.
- Hintergrundabhängige Materialien müssen dieselben autorisierten Compositor-Schnittstellen verwenden.
- Zusätzliche Rendering-Buffer dürfen nur bei tatsächlichem Bedarf angelegt werden.
- Nach Abschluss werden nicht mehr benötigte Übergangsressourcen freigegeben.

## 8. Zustandswechsel

Materialübergänge können durch folgende Änderungen ausgelöst werden:

- Wechsel des Materialtyps
- Änderung eines Material-Presets
- Hover- und Pressed-Zustände
- Fokusänderungen
- Aktivierung oder Deaktivierung eines Fensters
- Wechsel des System-Themes
- Änderungen systemweiter Darstellungseinstellungen
- Programmatische Änderungen der Materialinstanz

Die Auslösung erfolgt über die bestehenden UI-, Theme- und Materialsysteme.

## 9. Unterbrechungen und Umkehrung

Laufende Übergänge müssen kontrolliert unterbrochen werden können.

Anforderungen:

- Ein neuer Übergang kann den aktuellen Übergang ersetzen.
- Der aktuell sichtbare Materialzustand muss als neuer Ausgangszustand verwendbar sein.
- Umkehrbare Übergänge müssen ohne sichtbaren Sprung zurückgeführt werden können.
- Gleichzeitige Übergangsanforderungen müssen eindeutig aufgelöst werden.
- Abgebrochene Übergänge dürfen keine veralteten Materialzustände wiederherstellen.
- Nicht mehr benötigte Zwischenzustände müssen freigegeben werden.

## 10. Integration mit Materialeffekten

Material Transitions verwendet die vorhandenen Effekte der Nova Material Engine.

Insbesondere:

- Backdrop Blur
- Light Refraction
- Surface Reflections
- Dynamic Lighting
- Specular Highlights
- Surface Distortion
- Adaptive Transparency
- Material Tinting
- Fluid Deformation

Die Effekte müssen während eines Übergangs konsistente Materialparameter erhalten.

Material Transitions darf keine eigenen Implementierungen dieser Effekte bereitstellen.

## 11. Damage Tracking

Nova Material Transitions muss vollständig mit dem Damage Tracking des Nova Compositors kompatibel sein.

Verbindliche Anforderungen:

- Ausschließlich visuell veränderte Materialbereiche werden invalidiert.
- Ausgangs- und Zielmaterial müssen bei der Bestimmung des Wirkungsbereichs berücksichtigt werden.
- Änderungen von Unschärfe, Lichtbrechung und Verzerrung müssen ihre erweiterten Abtastbereiche berücksichtigen.
- Unveränderte Zwischenergebnisse dürfen wiederverwendet werden.
- Unsichtbare Übergänge dürfen keine unnötigen Rendering-Operationen auslösen.
- Abgeschlossene Übergänge dürfen keine weiteren Bildaktualisierungen verursachen.

Eine permanente vollständige Bildschirmaktualisierung ist nicht zulässig.

## 12. Performance

Materialübergänge müssen ressourcenschonend ausgeführt werden.

Verbindliche Anforderungen:

- Direkte Eigenschaftsinterpolation wird gegenüber vollständiger Materialüberblendung bevorzugt, sofern das Ergebnis gleichwertig ist.
- Gemeinsame Rendering-Ergebnisse müssen wiederverwendbar sein.
- Doppelte Hintergrundabtastungen sollen vermieden werden.
- Gleichartige Übergangsoperationen können zusammengefasst werden.
- GPU-Beschleunigung wird bevorzugt.
- Temporäre Rendering-Ressourcen müssen begrenzt werden.
- Die Berechnung bleibt auf sichtbare und betroffene Materialbereiche beschränkt.

## 13. Qualitätsstufen und Fallback

Nova Material Transitions unterstützt folgende Qualitätsstufen:

1. **Full:** Vollständige Materialtransformation mit Eigenschaftsinterpolation und Überblendung.
2. **Balanced:** Vereinfachte Übergänge mit reduzierter Effektkomplexität.
3. **Reduced:** Grundlegende Farb-, Transparenz- und Unschärfeübergänge.
4. **Fallback:** Sofortiger Wechsel zum Zielmaterial.

Ein Qualitätswechsel darf den logischen Zielzustand nicht verändern.

## 14. Barrierefreiheit

Nova Material Transitions muss systemweite Einstellungen zur Barrierefreiheit berücksichtigen.

Anforderungen:

- Materialübergänge müssen reduzierbar oder deaktivierbar sein.
- Die Einstellung für reduzierte Bewegung muss berücksichtigt werden.
- Übergänge dürfen keine störenden Helligkeitswechsel verursachen.
- Texte und Bedienelemente müssen während des Übergangs lesbar bleiben.
- Funktionale Zustandsänderungen müssen auch ohne Animation erkennbar sein.
- Bei deaktivierten Animationen erfolgt ein unmittelbarer Wechsel zum Zielzustand.

## 15. Determinismus und Sicherheit

Nova Material Transitions muss bei identischen Ausgangszuständen, Zielzuständen und Zeitwerten reproduzierbare Ergebnisse innerhalb definierter numerischer Toleranzen liefern.

Verbindliche Anforderungen:

- Übergangsparameter müssen validiert und begrenzt werden.
- Ungültige Materialkombinationen müssen kontrolliert behandelt werden.
- Übergänge dürfen keine Sicherheitsgrenzen des Material Systems oder Compositors umgehen.
- Hintergrundabhängige Übergänge dürfen keine geschützten Bildinformationen offenlegen.
- Die Anzahl gleichzeitig aktiver Übergänge muss kontrollierbar sein.
- Fehlerhafte Übergänge dürfen die Rendering-Pipeline nicht destabilisieren.
- Bei Berechnungsfehlern wird das definierte Zielmaterial ohne Animation aktiviert.

## 16. Ergebnis

Nova Material Transitions stellt eine einheitliche Verarbeitung von Materialwechseln und Materialzustandsübergängen innerhalb der Nova Material Engine bereit.

Durch Eigenschaftsinterpolation, Materialüberblendung und kontrollierte Zustandswechsel können Oberflächen flüssig zwischen unterschiedlichen Materialien wechseln.

Die Integration in das bestehende NovaOS Animation System und das Damage Tracking ermöglicht konsistente Materialtransformationen bei kontrolliertem Ressourcenverbrauch.