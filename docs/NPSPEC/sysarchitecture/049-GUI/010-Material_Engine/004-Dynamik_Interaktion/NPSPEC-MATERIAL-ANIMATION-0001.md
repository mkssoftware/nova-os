# NPSPEC-MATERIAL-ANIMATION-0001 – Nova Material Animation

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Material Engine

## 1. Zweck

Nova Material Animation definiert die zeitabhängige Veränderung visueller Materialeigenschaften innerhalb von NovaOS.

Das System ermöglicht flüssige Übergänge, dynamische Oberflächeneffekte und interaktionsabhängige Materialreaktionen.

Ziel ist eine einheitliche, performante und deterministisch steuerbare Animation sämtlicher NovaOS-Materialien.

## 2. Architektur

Nova Material Animation ist Bestandteil der Nova Material Engine und verwendet das bestehende NovaOS Animation System.

Die Zuständigkeiten sind:

- **Material System:** Bereitstellung animierbarer Materialeigenschaften.
- **Animation System:** Zeitsteuerung, Interpolation und Animationszustände.
- **Material Renderer:** Darstellung der aktuellen Materialzustände.
- **Graphics Engine:** Ausführung der Rendering-Operationen.
- **Compositor:** Aktualisierung und Komposition veränderter Materialbereiche.

Nova Material Animation implementiert keine eigenständige Animationslaufzeit.

## 3. Animierbare Eigenschaften

Grundsätzlich können alle dafür freigegebenen Materialeigenschaften animiert werden.

Dazu gehören:

- Transparenz und Deckkraft
- Hintergrundunschärfe
- Lichtbrechung und Reflexionen
- Glanzlichter und Beleuchtung
- Farbüberlagerungen und Farbverläufe
- Oberflächenverzerrungen
- Virtuelle Oberflächennormalen
- Materialübergänge
- Materialabhängige Effektparameter

Nicht animierbare Eigenschaften müssen ausdrücklich gekennzeichnet sein.

## 4. Animationsmodell

Materialanimationen verändern ausschließlich den Laufzeitzustand einer Materialinstanz.

Verbindliche Anforderungen:

- Gemeinsame Materialdefinitionen bleiben unverändert.
- Animationen werden deklarativ beschrieben.
- Mehrere Eigenschaften können gleichzeitig animiert werden.
- Animationen können gestartet, pausiert, fortgesetzt und beendet werden.
- Animationen müssen wiederholbar und umkehrbar sein.
- Der aktuelle Animationszustand muss eindeutig bestimmbar sein.
- Animationen dürfen keine Änderungen an der tatsächlichen UI-Geometrie erzwingen.

Geometrische Änderungen erfolgen ausschließlich über die dafür vorgesehenen UI- und Layoutsysteme.

## 5. Animationsverfahren

Nova Material Animation unterstützt folgende Verfahren:

- **Linear:** Gleichmäßige Interpolation.
- **Easing:** Beschleunigte oder verzögerte Übergänge.
- **Keyframe:** Animation anhand definierter Zwischenzustände.
- **Spring:** Federbasierte Übergänge.
- **Procedural:** Mathematisch definierte Animationen.
- **State Transition:** Übergänge zwischen Materialzuständen.

Die Verfahren werden durch das bestehende NovaOS Animation System bereitgestellt.

## 6. Zeitsteuerung

Materialanimationen verwenden eine gemeinsame monotone Zeitbasis.

Anforderungen:

- Animationswerte werden anhand der aktuellen Animationszeit berechnet.
- Die Animationsgeschwindigkeit darf nicht von der Bildwiederholrate abhängen.
- Variable Bildwiederholraten müssen unterstützt werden.
- Animationen können zeitlich synchronisiert werden.
- Unterbrochene Animationen müssen kontrolliert fortgesetzt oder beendet werden.
- Bei identischem Animationszustand und identischer Zeit müssen reproduzierbare Ergebnisse entstehen.

Die Darstellung erfolgt ausschließlich bei erforderlichen Bildaktualisierungen.

## 7. Animationszustände

Eine Materialanimation besitzt mindestens folgende Zustände:

- **Idle:** Animation ist nicht aktiv.
- **Running:** Animation wird ausgeführt.
- **Paused:** Animation ist angehalten.
- **Completed:** Animation wurde abgeschlossen.
- **Cancelled:** Animation wurde abgebrochen.

Zustandswechsel müssen eindeutig definiert sein.

Nach Abschluss einer Animation muss der konfigurierte Endzustand erhalten bleiben.

## 8. Interaktionsabhängige Animation

Materialanimationen können durch Ereignisse ausgelöst werden.

Unterstützt werden:

- Pointer Enter und Pointer Leave
- Pressed und Released
- Focus und Focus Lost
- Aktivierung und Deaktivierung
- Änderungen von Materialeigenschaften
- Änderungen des aktiven Themes
- Systemweite Zustandsänderungen

Die Ereignisverarbeitung erfolgt über das bestehende Nova UI Event System.

Materialanimationen dürfen keine eigenständige Eingabeverarbeitung implementieren.

## 9. Animationskomposition

Mehrere Animationen können dieselbe Materialinstanz beeinflussen.

Verbindliche Anforderungen:

- Gleichzeitige Animationen unterschiedlicher Eigenschaften müssen unterstützt werden.
- Konflikte bei derselben Eigenschaft müssen eindeutig aufgelöst werden.
- Animationsprioritäten müssen definierbar sein.
- Ersetzende und additive Animationen müssen unterscheidbar sein.
- Der resultierende Materialzustand muss deterministisch berechnet werden.
- Ungültige Kombinationen müssen kontrolliert behandelt werden.

## 10. Materialintegration

Nova Material Animation unterstützt sämtliche Standardmaterialien.

- **Acrylic:** Transparenz-, Farb- und Unschärfeübergänge.
- **Liquid Glass:** Flüssige Oberflächenbewegungen und dynamische Lichtreaktionen.
- **Frosted Glass:** Änderungen der Unschärfe und Lichtstreuung.
- **Crystal:** Animierte Reflexionen und Glanzlichter.
- **Solid:** Farb-, Helligkeits- und Zustandsübergänge.

Die Animation erfolgt über das gemeinsame Materialmodell.

## 11. Damage Tracking

Nova Material Animation muss vollständig mit dem Damage Tracking des Nova Compositors kompatibel sein.

Verbindliche Anforderungen:

- Ausschließlich visuell veränderte Bereiche werden invalidiert.
- Der bisherige und der neue Wirkungsbereich einer Animation müssen berücksichtigt werden.
- Effektabhängige Abtast- und Ausdehnungsbereiche müssen in die Damage-Berechnung einfließen.
- Unveränderte Materialbereiche dürfen nicht erneut verarbeitet werden.
- Unsichtbare Animationen müssen ausgesetzt oder ohne Rendering fortgeschrieben werden.
- Abgeschlossene Animationen dürfen keine weiteren Bildaktualisierungen auslösen.

Eine permanente vollständige Bildschirmaktualisierung ist nicht zulässig.

## 12. Performance

Materialanimationen müssen ressourcenschonend ausgeführt werden.

Verbindliche Anforderungen:

- Animationen werden nach Möglichkeit direkt über Materialparameter verarbeitet.
- Unnötige Änderungen an UI-Layout und Oberflächengeometrie werden vermieden.
- Gleichartige Animationsoperationen können zusammengefasst werden.
- GPU-basierte Parameteranimationen werden bevorzugt.
- Statische Rendering-Ergebnisse dürfen wiederverwendet werden.
- Nicht sichtbare Animationen dürfen reduziert oder ausgesetzt werden.
- Die Animationsfrequenz kann an Hardwareleistung und Energiezustand angepasst werden.

## 13. Qualitätsstufen und Fallback

Nova Material Animation unterstützt folgende Qualitätsstufen:

1. **Full:** Vollständige Materialanimationen mit sämtlichen verfügbaren Effekten.
2. **Balanced:** Vereinfachte Effektberechnung bei unverändertem Animationsverlauf.
3. **Reduced:** Reduzierte Animationen und vereinfachte Materialübergänge.
4. **Fallback:** Sofortiger Wechsel zwischen definierten Materialzuständen.

Ein Qualitätswechsel darf die logische Zustandsverwaltung nicht beeinträchtigen.

## 14. Barrierefreiheit

Materialanimationen müssen systemweite Einstellungen zur Barrierefreiheit berücksichtigen.

Anforderungen:

- Animationen müssen global reduzierbar oder deaktivierbar sein.
- Die Einstellung für reduzierte Bewegung muss berücksichtigt werden.
- Starke Helligkeitswechsel und störende Bewegungen müssen vermeidbar sein.
- Funktionale Zustandsänderungen müssen auch ohne Animation erkennbar bleiben.
- Dekorative Animationen dürfen die Bedienbarkeit nicht beeinträchtigen.

## 15. Determinismus und Sicherheit

Nova Material Animation muss bei identischen Eingaben, Zeitwerten und Rendering-Bedingungen reproduzierbare Ergebnisse innerhalb definierter numerischer Toleranzen liefern.

Anforderungen:

- Animationsparameter müssen validiert und begrenzt werden.
- Ungültige Zeitwerte und Interpolationsergebnisse müssen kontrolliert behandelt werden.
- Animationen dürfen keine Sicherheitsgrenzen des Material Systems oder Compositors umgehen.
- Die Anzahl gleichzeitig aktiver Animationen muss kontrollierbar sein.
- Fehlerhafte Animationen dürfen die Rendering-Pipeline nicht destabilisieren.
- Bei Animationsfehlern wird ein definierter Materialzustand verwendet.

## 16. Ergebnis

Nova Material Animation stellt eine einheitliche Steuerung dynamischer Materialeigenschaften innerhalb der Nova Material Engine bereit.

Durch deklarative Animationen, gemeinsame Zeitsteuerung, Interpolation und ereignisabhängige Übergänge entstehen flüssige und interaktive Materialoberflächen.

Die Integration in das bestehende NovaOS Animation System und das Damage Tracking ermöglicht hochwertige Animationen bei kontrolliertem Ressourcenverbrauch.