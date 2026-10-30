# NPSPEC-MATERIAL-LAYER-0001 – Nova Material Layer Composition

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Material Engine

## 1. Zweck

Nova Material Layer Composition definiert die Zusammensetzung mehrerer visueller Materialebenen innerhalb einer Materialinstanz.

Das System ermöglicht die Kombination von Grundmaterialien, Farbüberlagerungen, Reflexionen, Glanzlichtern und weiteren Materialeffekten.

Ziel ist eine flexible, deterministische und ressourcenschonende Materialkomposition ohne zusätzliche Fenster- oder Compositor-Ebenen.

## 2. Architektur

Material Layer Composition ist Bestandteil der Nova Material Engine und verwendet die bestehende Material Rendering Pipeline.

Die Zuständigkeiten sind:

- **Material System:** Definition und Verwaltung der Materialebenen.
- **Material Layer Composition:** Auflösung der Ebenenreihenfolge und Mischoperationen.
- **Material Renderer:** Verarbeitung der einzelnen Materialebenen.
- **Graphics Engine:** Ausführung der Rendering-Operationen.
- **Compositor:** Abschließende Komposition der resultierenden Oberfläche.

Materialebenen sind interne Bestandteile einer Materialinstanz und keine eigenständigen Compositor-Surfaces.

## 3. Ebenenmodell

Eine Materialinstanz kann aus mehreren geordneten Materialebenen bestehen.

Jede Ebene besitzt mindestens:

- **LayerId:** Eindeutige Kennung innerhalb der Materialinstanz.
- **LayerType:** Typ der Materialebene.
- **Enabled:** Aktivierungszustand.
- **Opacity:** Deckkraft.
- **BlendMode:** Mischverfahren.
- **Mask:** Optionale Ebenenmaske.
- **Transform:** Optionale visuelle Transformation.
- **Effects:** Zugeordnete Materialeffekte.

Die Ebenenreihenfolge muss eindeutig definiert sein.

## 4. Ebenentypen

Nova Material Layer Composition unterstützt folgende Ebenentypen:

- **Base Layer:** Grundlegende Materialdarstellung.
- **Tint Layer:** Farbüberlagerungen und Farbverläufe.
- **Effect Layer:** Materialeffekte wie Unschärfe oder Verzerrung.
- **Lighting Layer:** Beleuchtung und Glanzlichter.
- **Reflection Layer:** Oberflächenreflexionen.
- **Overlay Layer:** Zusätzliche dekorative Materialbestandteile.

Weitere Ebenentypen können über das Material System ergänzt werden.

## 5. Ebenenkomposition

Die Materialebenen werden entsprechend ihrer definierten Reihenfolge verarbeitet.

Verbindliche Anforderungen:

- Die Kompositionsreihenfolge muss deterministisch sein.
- Jede Ebene verarbeitet ausschließlich ihre zugewiesenen Eigenschaften und Effekte.
- Ebenen können transparent oder vollständig deckend sein.
- Deaktivierte Ebenen dürfen keine Rendering-Operationen auslösen.
- Ebenen dürfen keine zyklischen Abhängigkeiten bilden.
- Die resultierende Materialdarstellung wird als einheitliches Ergebnis an den Compositor übergeben.

## 6. Mischverfahren

Materialebenen unterstützen die Mischverfahren der Nova Graphics Engine.

Mindestens vorgesehen sind:

- **Normal:** Standardmäßige Alpha-Komposition.
- **Multiply:** Multiplikative Farbmischung.
- **Screen:** Aufhellende Farbmischung.
- **Overlay:** Kontrastabhängige Farbmischung.
- **Additive:** Additive Farbüberlagerung.

Die Mischoperationen müssen das NovaOS Color Management berücksichtigen.

Alpha-Werte müssen konsistent verarbeitet werden. Mehrfache unbeabsichtigte Anwendung derselben Transparenz ist unzulässig.

## 7. Ebenenmasken

Jede Materialebene kann eine eigene Maske besitzen.

Anforderungen:

- Masken begrenzen die sichtbaren Bereiche einer Ebene.
- Geometrische und texturbasierte Masken müssen unterstützt werden.
- Masken können mit der Materialgeometrie kombiniert werden.
- Änderungen einer Maske invalidieren ausschließlich betroffene Bereiche.
- Masken dürfen keine Änderungen an der funktionalen UI-Geometrie verursachen.

## 8. Effektintegration

Materialebenen können bestehende Effekte der Nova Material Engine verwenden.

Verbindliche Anforderungen:

- Effekte werden über die gemeinsame Material Rendering Pipeline ausgeführt.
- Mehrere Ebenen dürfen gemeinsame Effektressourcen verwenden.
- Effektabhängigkeiten müssen eindeutig aufgelöst werden.
- Hintergrundabhängige Effekte verwenden ausschließlich Nova Backdrop Sampling.
- Bereits berechnete Effektergebnisse dürfen wiederverwendet werden.
- Die Ebenenkomposition darf keine eigenständigen Implementierungen bestehender Materialeffekte enthalten.

## 9. Hintergrundabhängige Ebenen

Materialebenen können Hintergrundinformationen benötigen.

Anforderungen:

- Hintergrunddaten werden ausschließlich durch den Nova Compositor autorisiert bereitgestellt.
- Die Hintergrundabtastung berücksichtigt die tatsächliche Compositor-Ebenenreihenfolge.
- Materialebenen dürfen keine eigenen oder nachfolgenden Kompositionsergebnisse als Hintergrundquelle verwenden.
- Mehrere Ebenen können gemeinsame Hintergrunddaten nutzen.
- Geschützte Oberflächen dürfen nicht unberechtigt abgetastet werden.

Die interne Materialebenenreihenfolge verändert nicht die globale Compositor-Ebenenreihenfolge.

## 10. Dynamische Ebenen

Materialebenen können zur Laufzeit verändert werden.

Unterstützt werden:

- Aktivieren und Deaktivieren von Ebenen
- Änderungen der Ebenenreihenfolge
- Änderungen der Deckkraft
- Änderungen von Mischverfahren
- Austausch von Ebeneninhalten
- Animation einzelner Ebeneneigenschaften

Animationen verwenden das bestehende NovaOS Animation System.

Änderungen dürfen keine Neuerstellung des zugehörigen UI-Elements erfordern.

## 11. Styles und Templates

Materialebenen müssen mit Nova Material Styles and Templates kompatibel sein.

Anforderungen:

- Templates können die Ebenenstruktur definieren.
- Styles können Eigenschaften einzelner Ebenen überschreiben.
- Gemeinsame Ebenendefinitionen müssen wiederverwendbar sein.
- Materialinstanzen können individuelle Ebeneneigenschaften besitzen.
- Theme-Wechsel müssen betroffene Ebenen aktualisieren.
- Gemeinsam genutzte Definitionen dürfen durch individuelle Änderungen nicht verändert werden.

## 12. Damage Tracking

Material Layer Composition muss vollständig mit dem Damage Tracking des Nova Compositors kompatibel sein.

Verbindliche Anforderungen:

- Änderungen einzelner Ebenen invalidieren ausschließlich betroffene Materialbereiche.
- Änderungen einer Ebene müssen abhängige nachfolgende Ebenen berücksichtigen.
- Effektabhängige Abtastbereiche müssen in die Damage-Berechnung einfließen.
- Unveränderte Ebenenergebnisse dürfen wiederverwendet werden.
- Unsichtbare und deaktivierte Ebenen dürfen keine unnötigen Rendering-Operationen auslösen.
- Abgeschlossene Animationen dürfen keine weiteren Bildaktualisierungen verursachen.

Eine permanente vollständige Bildschirmaktualisierung ist nicht zulässig.

## 13. Performance

Die Ebenenkomposition muss ressourcenschonend erfolgen.

Verbindliche Anforderungen:

- Kompatible Ebenenoperationen sollen zusammengefasst werden.
- Zusätzliche Rendering-Durchläufe sind nach Möglichkeit zu vermeiden.
- Unveränderte Zwischenergebnisse dürfen zwischengespeichert werden.
- Temporäre Rendering-Buffer müssen wiederverwendbar sein.
- Die Anzahl aktiver Ebenen muss begrenzbar sein.
- GPU-Beschleunigung wird bevorzugt.
- Eine vereinfachte CPU-basierte Komposition muss verfügbar sein.
- Nicht sichtbare Ebenen werden von unnötigen Berechnungen ausgeschlossen.

## 14. Qualitätsstufen und Fallback

Nova Material Layer Composition unterstützt folgende Qualitätsstufen:

1. **Full:** Vollständige Ebenenkomposition mit sämtlichen unterstützten Effekten.
2. **Balanced:** Optimierte Komposition mit vereinfachten Effektberechnungen.
3. **Reduced:** Reduzierte Ebenenanzahl und grundlegende Mischoperationen.
4. **Fallback:** Vereinfachte Materialdarstellung mit einer Basisebene.

Die Qualitätsstufe kann abhängig von Hardwareleistung, Energiezustand und Systemeinstellungen angepasst werden.

Die funktionale Darstellung muss erhalten bleiben.

## 15. Barrierefreiheit

Material Layer Composition muss systemweite Einstellungen zur Barrierefreiheit berücksichtigen.

Anforderungen:

- Dekorative Ebenen müssen reduzierbar oder deaktivierbar sein.
- Kontrastvorgaben besitzen Vorrang vor visuellen Effekten.
- Texte und Bedienelemente dürfen durch Ebenenüberlagerungen nicht unlesbar werden.
- Animierte Ebenen müssen die Einstellung für reduzierte Bewegung berücksichtigen.
- Eine funktional gleichwertige Darstellung ohne komplexe Ebenenkomposition muss verfügbar sein.

## 16. Determinismus und Sicherheit

Die Materialebenenkomposition muss bei identischen Eingaben und Rendering-Bedingungen reproduzierbare Ergebnisse innerhalb definierter numerischer Toleranzen liefern.

Verbindliche Anforderungen:

- Ebenendefinitionen und Eigenschaften müssen validiert werden.
- Zyklische Ebenen- und Effektabhängigkeiten sind unzulässig.
- Die Anzahl und Komplexität von Materialebenen muss begrenzbar sein.
- Ebenen dürfen ausschließlich autorisierte Materialressourcen verwenden.
- Speicherzugriffe außerhalb gültiger Rendering-Buffer sind ausgeschlossen.
- Fehlerhafte Ebenen dürfen die Rendering-Pipeline nicht destabilisieren.
- Bei Kompositionsfehlern wird eine definierte vereinfachte Materialdarstellung verwendet.

## 17. Ergebnis

Nova Material Layer Composition stellt eine einheitliche Verarbeitung mehrschichtiger Materialoberflächen innerhalb der Nova Material Engine bereit.

Durch geordnete Materialebenen, Mischverfahren, Masken und gemeinsame Effektressourcen können komplexe Materialien flexibel zusammengesetzt werden.

Die Integration in die bestehende Material Rendering Pipeline ermöglicht hochwertige, dynamische und ressourcenschonende Materialdarstellungen ohne zusätzliche Compositor-Surfaces.