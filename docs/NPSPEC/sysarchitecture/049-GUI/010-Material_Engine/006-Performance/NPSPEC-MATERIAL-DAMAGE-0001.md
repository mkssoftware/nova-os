# NPSPEC-MATERIAL-DAMAGE-0001 – Nova Material Damage Tracking

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Material Engine

## 1. Zweck

Nova Material Damage Tracking definiert die Erkennung und Verwaltung veränderter Materialbereiche innerhalb von NovaOS.

Ziel ist, ausschließlich tatsächlich betroffene Bereiche neu zu berechnen und darzustellen.

Unveränderte Materialbereiche dürfen keine unnötigen Rendering-Operationen verursachen.

## 2. Architektur

Material Damage Tracking erweitert das bestehende Damage Tracking des Nova Compositors um materialabhängige Änderungsinformationen.

Die Zuständigkeiten sind:

- **Material System:** Erkennung geänderter Materialeigenschaften.
- **Material Renderer:** Bestimmung betroffener Effektbereiche.
- **Material Damage Tracking:** Berechnung und Weitergabe materialabhängiger Damage-Regionen.
- **Compositor:** Zusammenführung aller Damage-Regionen und Steuerung der Bildaktualisierung.
- **Graphics Engine:** Rendering der freigegebenen Aktualisierungsbereiche.

Material Damage Tracking implementiert keinen eigenständigen Compositor oder Rendering-Scheduler.

## 3. Grundprinzip

Jede visuelle Änderung einer Materialinstanz erzeugt eine definierte Damage-Region.

Verbindliche Anforderungen:

- Unveränderte Materialzustände erzeugen keinen Damage.
- Änderungen werden auf die tatsächlich betroffenen Bereiche begrenzt.
- Mehrere Damage-Regionen können zusammengeführt werden.
- Materialeffekte müssen ihre räumlichen Abhängigkeiten berücksichtigen.
- Damage-Regionen müssen in Compositor-Koordinaten überführbar sein.
- Der Compositor entscheidet über die endgültige Aktualisierung.

Eine permanente vollständige Bildschirmaktualisierung ist nicht zulässig.

## 4. Änderungsquellen

Material Damage Tracking berücksichtigt mindestens:

- Änderungen von Materialeigenschaften
- Änderungen von Material Styles und Themes
- Materialanimationen und Übergänge
- Änderungen der Materialgeometrie
- Oberflächenverformungen
- Änderungen von Beleuchtung und Reflexionen
- Änderungen relevanter Hintergrundinhalte
- Änderungen der Sichtbarkeit
- Änderungen von Materialmasken und Ebenen

Nicht visuell wirksame Änderungen dürfen keine Rendering-Aktualisierung auslösen.

## 5. Damage-Regionen

Damage-Regionen beschreiben die betroffenen Bereiche einer Materialinstanz.

Anforderungen:

- Rechteckige Damage-Regionen müssen unterstützt werden.
- Mehrere Regionen können als Regionensammlung verwaltet werden.
- Regionen müssen auf gültige Rendering-Bereiche begrenzt werden.
- Transformationen, Skalierungen und Clipping müssen berücksichtigt werden.
- Leere Regionen dürfen keine Rendering-Operationen auslösen.
- Bei zu hoher Fragmentierung dürfen Regionen zusammengefasst werden.

## 6. Effektabhängigkeiten

Materialeffekte müssen ihre räumlichen Abhängigkeiten bereitstellen.

Insbesondere:

- **Backdrop Blur:** Erweiterung entsprechend dem Filterradius.
- **Light Refraction:** Berücksichtigung maximaler Abtastverschiebungen.
- **Surface Distortion:** Berücksichtigung des Verzerrungsbereichs.
- **Surface Reflections:** Berücksichtigung reflektierter Bildbereiche.
- **Dynamic Lighting:** Berücksichtigung des Wirkungsbereichs von Lichtquellen.
- **Specular Highlights:** Berücksichtigung veränderter Glanzlichtbereiche.
- **Fluid Deformation:** Berücksichtigung der maximalen Oberflächenausdehnung.

Die Berechnung muss zwischen dem veränderten Ausgabebereich und den dafür benötigten Eingabebereichen unterscheiden.

## 7. Hintergrundabhängiger Damage

Hintergrundabhängige Materialien müssen auf relevante Änderungen hinter ihrer Oberfläche reagieren.

Verbindliche Anforderungen:

- Hintergrundabhängigkeiten werden über Nova Backdrop Sampling ermittelt.
- Änderungen außerhalb des tatsächlichen Abtastbereichs dürfen keine Aktualisierung auslösen.
- Änderungen innerhalb des Abtastbereichs invalidieren ausschließlich abhängige Materialpixel.
- Die Ebenenreihenfolge des Compositors muss berücksichtigt werden.
- Hintergrundabhängigkeiten dürfen keine zyklischen Invalidierungen verursachen.
- Geschützte Oberflächen dürfen keine unberechtigten Datenzugriffe ermöglichen.

## 8. Bewegungen und Verformungen

Bei Änderungen der visuellen Materialgeometrie müssen bisherige und neue Wirkungsbereiche berücksichtigt werden.

Anforderungen:

- Bewegungen invalidieren freigelegte und neu belegte Bereiche.
- Verformungen berücksichtigen die maximale visuelle Ausdehnung.
- Änderungen der Oberflächennormalen invalidieren abhängige Beleuchtungs- und Reflexionseffekte.
- Unveränderte Teilbereiche dürfen wiederverwendet werden.
- Visuelle Materialverformungen dürfen das UI-Layout nicht eigenständig verändern.

## 9. Animationen

Materialanimationen erzeugen Damage ausschließlich bei visuellen Zustandsänderungen.

Verbindliche Anforderungen:

- Die Aktualisierung erfolgt anhand des tatsächlich dargestellten Animationszustands.
- Identische aufeinanderfolgende Zustände erzeugen keinen Damage.
- Nicht sichtbare Animationen dürfen ohne Rendering fortgeschrieben oder ausgesetzt werden.
- Veraltete Zwischenzustände dürfen übersprungen werden.
- Nach Abschluss einer Animation dürfen keine weiteren animationsbedingten Aktualisierungen erfolgen.
- Eine Animation darf keine permanente Rendering-Schleife hinterlassen.

## 10. Materialebenen

Material Damage Tracking muss mehrschichtige Materialien unterstützen.

Anforderungen:

- Änderungen einzelner Ebenen erzeugen gezielte Damage-Regionen.
- Abhängige nachfolgende Ebenen müssen berücksichtigt werden.
- Unveränderte Ebenenergebnisse dürfen wiederverwendet werden.
- Deaktivierte oder vollständig verdeckte Ebenen dürfen keine unnötigen Berechnungen auslösen.
- Die interne Materialebenenstruktur darf die globale Compositor-Ebenenreihenfolge nicht verändern.

## 11. Cache-Invalidierung

Damage Tracking muss mit Nova Material Caching zusammenarbeiten.

Verbindliche Anforderungen:

- Änderungen invalidieren ausschließlich betroffene Cache-Einträge oder Teilbereiche.
- Cache-Abhängigkeiten müssen eindeutig bestimmbar sein.
- Unveränderte Zwischenergebnisse bleiben gültig.
- Hintergrundänderungen müssen abhängige Cache-Ergebnisse invalidieren.
- Veraltete Ergebnisse dürfen nicht erneut verwendet werden.
- Cache-Invalidierungen dürfen keine unnötigen Rendering-Aktualisierungen auslösen.

## 12. Performance

Die Damage-Verarbeitung muss ressourcenschonend erfolgen.

Anforderungen:

- Damage-Regionen werden vor dem Rendering zusammengeführt und optimiert.
- Redundante Änderungsmeldungen dürfen zusammengefasst werden.
- Die Anzahl verwalteter Regionen muss begrenzbar sein.
- Bei hoher Fragmentierung kann eine größere zusammenhängende Region verwendet werden.
- Die Berechnung von Damage darf nicht aufwendiger werden als ein sinnvoller vollständiger Neuaufbau des betroffenen Bereichs.
- GPU- und CPU-Rendering müssen dieselben Damage-Informationen verwenden können.

## 13. Vollständige Aktualisierung

Eine vollständige Aktualisierung ist ausschließlich bei tatsächlicher Notwendigkeit zulässig.

Mögliche Ursachen:

- Erstmalige Darstellung einer Materialinstanz
- Vollständige Änderung der Materialdefinition
- Verlust erforderlicher Rendering-Ressourcen
- Ungültige oder nicht rekonstruierbare Cache-Zustände
- Änderungen, deren Wirkungsbereich die gesamte Materialoberfläche umfasst

Eine vollständige Materialaktualisierung bedeutet nicht automatisch eine vollständige Bildschirmaktualisierung.

Die endgültige Entscheidung verbleibt beim Compositor.

## 14. Determinismus und Sicherheit

Material Damage Tracking muss bei identischen Materialzuständen und Änderungsereignissen reproduzierbare Damage-Regionen erzeugen.

Verbindliche Anforderungen:

- Damage-Koordinaten und Regionsgrößen müssen validiert werden.
- Überläufe und ungültige Geometrien müssen kontrolliert behandelt werden.
- Effektabhängigkeiten dürfen keine Sicherheitsgrenzen des Compositors umgehen.
- Zyklische Abhängigkeiten müssen erkannt und verhindert werden.
- Fehlerhafte Damage-Informationen dürfen die Rendering-Pipeline nicht destabilisieren.
- Kann ein betroffener Bereich nicht zuverlässig bestimmt werden, muss eine sichere vollständige Aktualisierung der betroffenen Materialoberfläche erfolgen.

## 15. Ergebnis

Nova Material Damage Tracking erweitert das bestehende Damage Tracking des Nova Compositors um materialabhängige Änderungs- und Effektinformationen.

Durch präzise Damage-Regionen, Hintergrundabhängigkeiten und gezielte Cache-Invalidierung werden ausschließlich tatsächlich betroffene Materialbereiche aktualisiert.

Das Ergebnis ist eine reaktionsschnelle, energieeffiziente Materialdarstellung ohne unnötige permanente Neuberechnung.