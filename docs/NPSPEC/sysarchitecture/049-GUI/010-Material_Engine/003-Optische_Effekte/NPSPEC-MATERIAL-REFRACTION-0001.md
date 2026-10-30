# NPSPEC-MATERIAL-REFRACTION-0001 – Nova Light Refraction

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Material Engine

## 1. Zweck

Nova Light Refraction definiert die optische Lichtbrechung transparenter Materialien innerhalb von NovaOS.

Der Effekt simuliert die Ablenkung von Hintergrundinhalten durch virtuelle Glasoberflächen und ermöglicht realistische Tiefenwirkung, Glaskanten und optische Verzerrungen.

Die Berechnung erfolgt ressourcenschonend innerhalb der bestehenden Material Rendering Pipeline.

## 2. Architektur

Nova Light Refraction ist ein Effekt der Nova Material Engine.

Die Zuständigkeiten sind:

- **Material System:** Definiert die Brechungsparameter.
- **Material Renderer:** Berechnet die Lichtbrechung.
- **Compositor:** Stellt die benötigten Hintergrunddaten bereit.
- **Graphics Engine:** Führt die Rendering-Operationen aus.

Die Lichtbrechung wird durch bildraumbasierte Hintergrundabtastung simuliert und benötigt keine vollständige physikalische Strahlverfolgung.

## 3. Eigenschaften

Der Effekt unterstützt folgende Eigenschaften:

- **Refraction:** Intensität der Lichtbrechung.
- **RefractiveIndex:** Virtueller Brechungsindex.
- **Thickness:** Virtuelle Materialdicke.
- **NormalMap:** Optionale Oberflächennormalen.
- **EdgeIntensity:** Verstärkung der Lichtbrechung an Materialkanten.
- **Distortion:** Zusätzliche optische Verzerrung.
- **Dispersion:** Optionale chromatische Lichtzerlegung.
- **Quality:** Qualitätsstufe der Berechnung.

Alle Parameter müssen definierte Datentypen, Wertebereiche und Standardwerte besitzen.

Eine Refraction-Intensität von 0 deaktiviert den Effekt.

## 4. Lichtbrechungsmodell

Die Lichtbrechung erfolgt anhand der virtuellen Oberflächengeometrie und der Materialeigenschaften.

Verbindliche Anforderungen:

- Hintergrundpixel werden anhand berechneter Brechungsvektoren versetzt abgetastet.
- Der Brechungsindex beeinflusst die Richtung und Intensität der Ablenkung.
- Die virtuelle Materialdicke beeinflusst den resultierenden Abtastversatz.
- Oberflächennormalen bestimmen die lokale Brechungsrichtung.
- Materialkanten können eine stärkere Brechung als die Materialmitte aufweisen.
- Die Berechnung muss mit transparenten und halbtransparenten Materialien kompatibel sein.

Die verwendete Näherung muss bei identischen Eingaben reproduzierbare Ergebnisse liefern.

## 5. Oberflächengeometrie

Die Lichtbrechung berücksichtigt die virtuelle Form des Materials.

Unterstützt werden:

- Ebene Glasoberflächen
- Abgerundete Glaskanten
- Gewölbte Oberflächen
- Individuelle Normal Maps
- Dynamisch verformte Oberflächen

Die virtuelle Geometrie darf unabhängig von der tatsächlichen UI-Layoutgeometrie definiert werden.

Verformungen dürfen die Eingabe- und Layoutbereiche nicht unbeabsichtigt verändern.

## 6. Hintergrundabtastung

Die benötigten Hintergrunddaten werden ausschließlich über den Nova Compositor bereitgestellt.

Anforderungen:

- Die Hintergrundabtastung berücksichtigt die korrekte Ebenenreihenfolge.
- Es werden ausschließlich darunterliegende, für den Effekt freigegebene Bildinhalte verwendet.
- Der maximale Brechungsversatz bestimmt den erweiterten Abtastbereich.
- Abtastungen außerhalb verfügbarer Hintergrundbereiche müssen definiert behandelt werden.
- Selbstreferenzierende und zyklische Abhängigkeiten sind ausgeschlossen.
- Geschützte Inhalte dürfen nicht durch Lichtbrechung offengelegt werden.

Die Material Engine erhält keinen direkten Zugriff auf fremde Surface-Buffer.

## 7. Chromatische Dispersion

Nova Light Refraction unterstützt optional die unterschiedliche Brechung einzelner Farbkanäle.

Anforderungen:

- Die Intensität wird über Dispersion gesteuert.
- Farbkanäle können unterschiedliche Abtastversätze verwenden.
- Der Effekt muss vollständig deaktivierbar sein.
- Die Dispersion darf die Lesbarkeit von Vordergrundinhalten nicht beeinträchtigen.
- Bei reduzierter Rendering-Qualität darf die Dispersion entfallen.

Chromatische Dispersion ist keine Voraussetzung für die grundlegende Lichtbrechung.

## 8. Dynamische Lichtbrechung

Die Brechungsparameter können zur Laufzeit verändert werden.

Unterstützt werden:

- Änderungen der virtuellen Materialdicke
- Animierte Oberflächennormalen
- Flüssige Oberflächenverformungen
- Interaktionsabhängige Brechungsänderungen
- Übergänge zwischen Materialzuständen

Animationen verwenden das bestehende NovaOS-Animationssystem.

Ohne Änderungen der Materialeigenschaften oder Hintergrundinhalte darf keine kontinuierliche Neuberechnung erfolgen.

## 9. Damage Tracking

Nova Light Refraction muss mit dem Damage Tracking des Nova Compositors zusammenarbeiten.

Verbindliche Anforderungen:

- Änderungen des Hintergrundes invalidieren abhängige Brechungsbereiche.
- Der maximale Abtastversatz wird bei der Damage-Berechnung berücksichtigt.
- Änderungen der Oberflächengeometrie invalidieren betroffene Materialbereiche.
- Unveränderte Brechungsergebnisse können wiederverwendet werden.
- Änderungen außerhalb relevanter Abtastbereiche dürfen keine Neuberechnung auslösen.
- Unsichtbare Materialbereiche werden nicht verarbeitet.

Eine vollständige Bildschirmaktualisierung darf nur erfolgen, wenn eine partielle Aktualisierung nicht möglich oder unwirtschaftlich ist.

## 10. Performance

Die Lichtbrechung muss ressourcenschonend berechnet werden.

Verbindliche Anforderungen:

- GPU-Beschleunigung wird bevorzugt.
- Brechungsvektoren können vorberechnet und zwischengespeichert werden.
- Statische Normal Maps und Geometrieinformationen dürfen wiederverwendet werden.
- Hintergrundabtastungen bleiben auf benötigte Bereiche begrenzt.
- Kompatible Effektoperationen können zusammengefasst werden.
- Temporäre Rendering-Ressourcen werden nach Möglichkeit wiederverwendet.
- Unnötige CPU-GPU-Datenübertragungen werden vermieden.

Die Berechnung muss mit der bestehenden Material Rendering Pipeline kompatibel bleiben.

## 11. Qualitätsstufen und Fallback

Nova Light Refraction unterstützt folgende Qualitätsstufen:

1. **Full:** Hochwertige Lichtbrechung mit Oberflächennormalen und optionaler Dispersion.
2. **Balanced:** Vereinfachte Brechungsberechnung ohne aufwendige Zusatzoperationen.
3. **Reduced:** Statische oder vereinfachte Brechungsvektoren.
4. **Fallback:** Transparente oder undurchsichtige Materialdarstellung ohne Lichtbrechung.

Die Qualitätsstufe kann abhängig von Hardwareleistung, Energiezustand und Systemeinstellungen angepasst werden.

Ein Qualitätswechsel darf keine Neuerstellung des zugehörigen UI-Elements erfordern.

## 12. Barrierefreiheit

Nova Light Refraction muss systemweite Einstellungen zur Barrierefreiheit berücksichtigen.

Anforderungen:

- Lichtbrechung muss reduzierbar oder deaktivierbar sein.
- Dynamische Verzerrungen müssen abschaltbar sein.
- Vordergrundtexte und Bedienelemente dürfen nicht durch Hintergrundbrechung unlesbar werden.
- Bei deaktivierten Effekten muss eine funktional gleichwertige Materialdarstellung erhalten bleiben.

## 13. Determinismus und Sicherheit

Nova Light Refraction muss bei identischen Eingaben und Rendering-Bedingungen reproduzierbare Ergebnisse innerhalb definierter numerischer Toleranzen liefern.

Anforderungen:

- Brechungsparameter müssen validiert und begrenzt werden.
- Ungültige Normalen und Berechnungswerte müssen kontrolliert behandelt werden.
- Speicherzugriffe außerhalb gültiger Abtastbereiche sind ausgeschlossen.
- Materialinstanzen dürfen keine Sicherheitsgrenzen des Compositors umgehen.
- Fehlerhafte Effektberechnungen dürfen die Rendering-Pipeline nicht destabilisieren.
- Bei Berechnungsfehlern wird ein definiertes Fallback verwendet.

## 14. Ergebnis

Nova Light Refraction stellt eine einheitliche, hardwareunabhängige Lichtbrechung für transparente Materialien innerhalb von NovaOS bereit.

Durch virtuelle Oberflächengeometrie, Hintergrundabtastung und optionale chromatische Dispersion entstehen realistische Glas- und Kristalleffekte.

Die Integration in die Nova Material Engine ermöglicht die gemeinsame Nutzung durch Liquid Glass, Crystal und weitere Materialien bei kontrolliertem Ressourcenverbrauch.