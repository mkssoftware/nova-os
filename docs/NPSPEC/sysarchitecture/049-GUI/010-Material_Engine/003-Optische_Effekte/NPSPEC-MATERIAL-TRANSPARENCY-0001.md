# NPSPEC-MATERIAL-TRANSPARENCY-0001 – Nova Adaptive Transparency

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Material Engine

## 1. Zweck

Nova Adaptive Transparency definiert die dynamische Anpassung der Transparenz visueller Oberflächenmaterialien innerhalb von NovaOS.

Der Effekt ermöglicht die automatische Anpassung von Deckkraft, Hintergrunddurchlässigkeit und Kontrast abhängig von Hintergrundinhalten, Materialzuständen und Systemeinstellungen.

Ziel ist eine optimale Balance zwischen visueller Transparenz, Lesbarkeit und Ressourcenverbrauch.

## 2. Architektur

Nova Adaptive Transparency ist Bestandteil der Nova Material Engine und verwendet die bestehende Material Rendering Pipeline.

Die Zuständigkeiten sind:

- **Material System:** Definition der Transparenzeigenschaften.
- **Material Renderer:** Berechnung der effektiven Transparenz.
- **Compositor:** Bereitstellung der Hintergrundinformationen und Alpha-Komposition.
- **Graphics Engine:** Ausführung der Rendering-Operationen.
- **Nova Theme System:** Bereitstellung systemweiter Transparenz- und Kontrastvorgaben.

Adaptive Transparency implementiert keinen eigenständigen Renderer.

## 3. Eigenschaften

Der Effekt unterstützt folgende Eigenschaften:

- **Opacity:** Grundlegende Deckkraft des Materials.
- **MinOpacity:** Minimal zulässige Deckkraft.
- **MaxOpacity:** Maximal zulässige Deckkraft.
- **AdaptiveOpacity:** Aktivierung der automatischen Transparenzanpassung.
- **BackgroundLuminance:** Ermittelte Hintergrundhelligkeit.
- **BackgroundContrast:** Ermittelte Hintergrundkontraste.
- **ContrastTarget:** Angestrebter Kontrastwert.
- **AdaptationStrength:** Stärke der automatischen Anpassung.
- **AdaptationSpeed:** Geschwindigkeit der Transparenzänderung.
- **TransparencyMode:** Verwendetes Anpassungsverfahren.

Alle Eigenschaften müssen definierte Datentypen, Wertebereiche und Standardwerte besitzen.

## 4. Transparenzmodell

Nova Adaptive Transparency verwendet ein einheitliches Modell zur Berechnung der effektiven Materialdeckkraft.

Verbindliche Anforderungen:

- Die effektive Deckkraft wird aus Materialeigenschaften und optionalen Umgebungsinformationen bestimmt.
- Die berechnete Deckkraft muss innerhalb der definierten Grenzen liegen.
- Transparenzanpassungen dürfen die grundlegende Materialdefinition nicht verändern.
- Die Transparenz muss unabhängig von Hintergrundunschärfe und Lichtbrechung konfigurierbar bleiben.
- Materialzustände können unterschiedliche Transparenzwerte verwenden.
- Die Berechnung muss mit der bestehenden Alpha-Komposition kompatibel sein.

Die Anpassung erfolgt auf Ebene der Materialinstanz.

## 5. Anpassungsverfahren

Nova Adaptive Transparency unterstützt folgende Verfahren:

- **Static:** Feste Deckkraft ohne automatische Anpassung.
- **Luminance Adaptive:** Anpassung anhand der Hintergrundhelligkeit.
- **Contrast Adaptive:** Anpassung anhand der Hintergrundkontraste.
- **Content Adaptive:** Anpassung anhand der visuellen Hintergrundkomplexität.
- **State Adaptive:** Anpassung abhängig vom aktuellen Materialzustand.

Mehrere Verfahren können kombiniert werden.

Die Auswahl erfolgt über die Materialeigenschaften und systemweiten Darstellungsvorgaben.

## 6. Hintergrundanalyse

Die adaptive Transparenz kann verfügbare Hintergrundinformationen auswerten.

Anforderungen:

- Hintergrunddaten werden ausschließlich über den Nova Compositor bereitgestellt.
- Die Analyse berücksichtigt die korrekte Ebenenreihenfolge.
- Helligkeit und Kontrast können über vereinfachte statistische Verfahren ermittelt werden.
- Die Analyse muss auf relevante Materialbereiche begrenzt bleiben.
- Unveränderte Analyseergebnisse dürfen zwischengespeichert werden.
- Geschützte Inhalte dürfen nicht durch die Analyse offengelegt werden.

Eine vollständige Bildanalyse ist nicht erforderlich.

## 7. Kontrast und Lesbarkeit

Adaptive Transparency muss die Lesbarkeit von Vordergrundinhalten unterstützen.

Verbindliche Anforderungen:

- Hintergrundkontraste dürfen die Erkennbarkeit von Texten und Bedienelementen nicht beeinträchtigen.
- Bei unzureichendem Kontrast kann die Materialdeckkraft erhöht werden.
- Die Anpassung muss mit dem Nova Theme System zusammenarbeiten.
- Systemweite Kontrastvorgaben haben Vorrang vor dekorativen Transparenzeffekten.
- Falls Transparenzanpassungen nicht ausreichen, muss eine undurchsichtige Ersatzdarstellung möglich sein.

Die Transparenzanpassung allein garantiert keinen ausreichenden Textkontrast. Die endgültige Kontrastbewertung muss Vordergrundfarben und weitere Materialeffekte berücksichtigen.

## 8. Dynamisches Verhalten

Transparenzwerte können zur Laufzeit angepasst werden.

Unterstützt werden:

- Änderungen der Hintergrundhelligkeit
- Änderungen der Hintergrundkontraste
- Hover- und Pressed-Zustände
- Active- und Inactive-Zustände
- Fokusänderungen
- Übergänge zwischen Materialzuständen
- Änderungen systemweiter Darstellungseinstellungen

Anpassungen können über das bestehende NovaOS-Animationssystem interpoliert werden.

Zur Vermeidung sichtbaren Flackerns müssen geeignete Schwellwerte und Hysterese unterstützt werden.

Ohne relevante Änderungen darf keine kontinuierliche Neuberechnung erfolgen.

## 9. Materialintegration

Nova Adaptive Transparency kann von unterschiedlichen Materialien verwendet werden.

- **Acrylic:** Anpassung der Deckkraft an Hintergrundhelligkeit und Kontrast.
- **Liquid Glass:** Dynamische Transparenz bei wechselnden Hintergrundinhalten.
- **Frosted Glass:** Kontrastabhängige Anpassung der Milchglaswirkung.
- **Crystal:** Anpassung der Transparenz zur Verbesserung der Lesbarkeit.
- **Solid:** Vollständig undurchsichtige Darstellung ohne adaptive Transparenz.

Die Verwendung erfolgt über das gemeinsame Materialmodell.

## 10. Damage Tracking

Adaptive Transparency muss mit dem Damage Tracking des Nova Compositors kompatibel sein.

Verbindliche Anforderungen:

- Änderungen der effektiven Deckkraft invalidieren betroffene Materialbereiche.
- Hintergrundänderungen lösen nur bei relevanten Abhängigkeiten eine erneute Analyse aus.
- Unveränderte Analyseergebnisse können wiederverwendet werden.
- Änderungen außerhalb relevanter Hintergrundbereiche dürfen keine unnötigen Berechnungen auslösen.
- Unsichtbare Materialbereiche werden nicht analysiert.
- Änderungen systemweiter Transparenzeinstellungen invalidieren ausschließlich betroffene Oberflächen.

Die Hintergrundanalyse darf keine permanente vollständige Bildschirmaktualisierung verursachen.

## 11. Performance

Adaptive Transparency muss ressourcenschonend arbeiten.

Verbindliche Anforderungen:

- Hintergrundanalysen werden auf notwendige Bereiche begrenzt.
- Vereinfachte Helligkeits- und Kontrastberechnungen werden bevorzugt.
- Analyseergebnisse können zwischengespeichert werden.
- Gleichartige Analysen dürfen gemeinsam verwendet werden.
- GPU-Beschleunigung kann bei verfügbaren Rendering-Backends genutzt werden.
- Unnötige CPU-GPU-Datenübertragungen werden vermieden.
- Anpassungen dürfen keine zusätzlichen permanenten Rendering-Schleifen erzeugen.

## 12. Qualitätsstufen und Fallback

Nova Adaptive Transparency unterstützt folgende Qualitätsstufen:

1. **Full:** Dynamische Anpassung anhand von Helligkeit, Kontrast und Hintergrundkomplexität.
2. **Balanced:** Vereinfachte Hintergrundanalyse mit reduzierter Aktualisierung.
3. **Reduced:** Anpassung anhand grundlegender Helligkeitswerte und Materialzustände.
4. **Fallback:** Feste Deckkraft oder undurchsichtige Nova Solid-Oberfläche.

Die Qualitätsstufe kann abhängig von Hardwareleistung, Energiezustand und Systemeinstellungen angepasst werden.

Ein Qualitätswechsel darf keine Neuerstellung des zugehörigen UI-Elements erfordern.

## 13. Barrierefreiheit

Adaptive Transparency muss systemweite Einstellungen zur Barrierefreiheit berücksichtigen.

Anforderungen:

- Transparenz muss reduzierbar oder vollständig deaktivierbar sein.
- Hohe Kontrasteinstellungen müssen Vorrang besitzen.
- Dynamische Transparenzübergänge müssen abschaltbar sein.
- Die Lesbarkeit darf nicht zugunsten dekorativer Effekte reduziert werden.
- Benutzerdefinierte Transparenzeinstellungen müssen berücksichtigt werden.
- Eine undurchsichtige Darstellung muss jederzeit verfügbar sein.

## 14. Determinismus und Sicherheit

Nova Adaptive Transparency muss bei identischen Eingaben und Rendering-Bedingungen reproduzierbare Ergebnisse innerhalb definierter numerischer Toleranzen liefern.

Anforderungen:

- Transparenzparameter müssen validiert und begrenzt werden.
- Ungültige Analysewerte müssen kontrolliert behandelt werden.
- Hintergrundanalysen dürfen keine Sicherheitsgrenzen des Compositors umgehen.
- Anwendungen dürfen keine unberechtigten Bildinformationen anderer Oberflächen erhalten.
- Analyseergebnisse dürfen keine geschützten Hintergrundinformationen offenlegen.
- Fehlerhafte Berechnungen dürfen die Rendering-Pipeline nicht destabilisieren.
- Bei Berechnungsfehlern wird eine definierte, kontrastgerechte Fallback-Darstellung verwendet.

## 15. Ergebnis

Nova Adaptive Transparency stellt eine einheitliche, dynamische Transparenzsteuerung innerhalb der Nova Material Engine bereit.

Durch die Berücksichtigung von Hintergrundhelligkeit, Kontrast, Materialzuständen und Systemeinstellungen können transparente Oberflächen automatisch an ihre visuelle Umgebung angepasst werden.

Die Integration in die bestehende Grafikarchitektur ermöglicht eine lesbare, anpassbare und ressourcenschonende Darstellung transparenter NovaOS-Materialien.