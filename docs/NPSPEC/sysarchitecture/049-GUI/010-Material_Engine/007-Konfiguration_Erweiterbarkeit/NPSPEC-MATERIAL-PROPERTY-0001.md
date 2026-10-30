# NPSPEC-MATERIAL-PROPERTY-0001 – Nova Material Properties

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Material Engine

## 1. Zweck

Nova Material Properties definiert das einheitliche Eigenschaftssystem für sämtliche Materialien und Materialeffekte innerhalb von NovaOS.

Das System ermöglicht die deklarative Definition, Validierung, Bindung und dynamische Veränderung von Materialeigenschaften.

Ziel ist eine konsistente, typsichere und ressourcenschonende Verwaltung aller Materialparameter.

## 2. Architektur

Material Properties ist Bestandteil des Nova Material Systems und integriert sich in das bestehende Nova UI Property System.

Die Zuständigkeiten sind:

- **Material System:** Registrierung und Verwaltung von Materialeigenschaften.
- **Material Properties:** Definition von Datentypen, Standardwerten und Änderungsregeln.
- **Nova UI Property System:** Datenbindung, Vererbung und Eigenschaftsauflösung.
- **Material Styles und Templates:** Bereitstellung wiederverwendbarer Eigenschaftswerte.
- **Nova Theme System:** Bereitstellung themeabhängiger Eigenschaften.
- **Material Renderer:** Verwendung der aufgelösten Eigenschaften.
- **Material Damage Tracking:** Verarbeitung visuell relevanter Änderungen.

Es wird kein unabhängiges UI-Eigenschaftssystem implementiert.

## 3. Eigenschaftsmodell

Jede Materialeigenschaft besitzt eine eindeutige Definition.

Mindestens vorgesehen sind:

- **PropertyId:** Eindeutige Eigenschaftskennung.
- **Name:** Bezeichnung der Eigenschaft.
- **DataType:** Datentyp.
- **DefaultValue:** Standardwert.
- **ValueRange:** Zulässiger Wertebereich.
- **Flags:** Verhalten und Eigenschaften der Property.
- **Validation:** Regeln zur Werteprüfung.
- **ChangeImpact:** Auswirkungen einer Änderung.

Eigenschaftsdefinitionen müssen unveränderlich und zwischen Materialinstanzen wiederverwendbar sein.

## 4. Unterstützte Datentypen

Material Properties unterstützt mindestens:

- Boolean
- Integer
- Float
- Double
- Color
- Vector2
- Vector3
- Vector4
- Matrix
- Rectangle
- Enumeration
- Resource Reference
- Material Reference
- Texture Reference

Weitere Datentypen können über das bestehende NovaOS-Typsystem ergänzt werden.

Alle Datentypen müssen eindeutig definiert und validierbar sein.

## 5. Standardmaterialeigenschaften

Materialtypen können folgende gemeinsame Eigenschaften bereitstellen:

- **Opacity:** Deckkraft.
- **TintColor:** Materialfarbe.
- **TintIntensity:** Intensität der Einfärbung.
- **BlurRadius:** Unschärferadius.
- **RefractionStrength:** Lichtbrechungsintensität.
- **ReflectionIntensity:** Reflexionsintensität.
- **HighlightIntensity:** Glanzlichtintensität.
- **DistortionStrength:** Verzerrungsintensität.
- **Roughness:** Oberflächenrauheit.
- **Metallic:** Metallischer Oberflächenanteil.
- **LightingIntensity:** Beleuchtungsintensität.
- **BlendMode:** Mischverfahren.
- **Quality:** Bevorzugte Darstellungsqualität.

Nicht jeder Materialtyp muss sämtliche Eigenschaften unterstützen.

Materialtypspezifische Eigenschaften werden durch die jeweilige Materialdefinition festgelegt.

## 6. Eigenschaftsregistrierung

Materialtypen und Materialeffekte müssen ihre unterstützten Eigenschaften registrieren.

Verbindliche Anforderungen:

- Jede Eigenschaft muss innerhalb ihres Definitionsbereichs eindeutig identifizierbar sein.
- Datentypen und Standardwerte müssen festgelegt werden.
- Zulässige Wertebereiche müssen definiert sein.
- Eigenschaften können als animierbar, bindbar oder schreibgeschützt gekennzeichnet werden.
- Die Registrierung muss vor der Verwendung abgeschlossen sein.
- Ungültige oder doppelte Registrierungen müssen kontrolliert behandelt werden.

## 7. Eigenschaftswerte

Materialdefinitionen und Materialinstanzen verwenden dasselbe Eigenschaftsmodell.

Anforderungen:

- Materialdefinitionen enthalten gemeinsame Standardkonfigurationen.
- Materialinstanzen können individuelle Eigenschaftswerte besitzen.
- Individuelle Änderungen dürfen gemeinsame Definitionen nicht verändern.
- Nicht überschriebene Eigenschaften verwenden ihre aufgelösten Standardwerte.
- Eigenschaftswerte müssen zur Laufzeit lesbar sein.
- Änderungen müssen über definierte Schnittstellen erfolgen.

## 8. Eigenschaftsprioritäten

Die Auflösung von Materialeigenschaften muss mit dem Nova UI Property System kompatibel sein.

Die Prioritäten werden durch die gemeinsame Property-Auflösung festgelegt.

Dabei werden berücksichtigt:

1. Materialstandardwerte
2. Material Templates
3. Material Styles
4. Theme-Ressourcen
5. Lokale Materialeigenschaften
6. Aktive Zustandsregeln
7. Aktive Animationen

Die tatsächliche Priorität muss den verbindlichen Regeln des Nova UI Property Systems entsprechen.

Sicherheits- und Barrierefreiheitsvorgaben werden als übergeordnete Einschränkungen angewendet.

## 9. Datenbindung

Materialeigenschaften müssen das bestehende Nova UI Data Binding unterstützen.

Verbindliche Anforderungen:

- Eigenschaften können an Datenquellen gebunden werden.
- Änderungen gebundener Werte müssen automatisch verarbeitet werden.
- Bindungen dürfen keine unkontrollierten zyklischen Aktualisierungen erzeugen.
- Ungültige Bindungswerte müssen kontrolliert behandelt werden.
- Nicht veränderte Werte dürfen keine erneute Rendering-Aktualisierung auslösen.
- Bindungen müssen ohne direkte GPU-Zugriffe funktionieren.

## 10. Animationen

Animierbare Materialeigenschaften verwenden das bestehende NovaOS Animation System.

Anforderungen:

- Animierbare Eigenschaften müssen entsprechend gekennzeichnet sein.
- Unterstützte Datentypen müssen geeignete Interpolationsverfahren besitzen.
- Animationen dürfen gemeinsame Materialdefinitionen nicht verändern.
- Animationswerte werden über die reguläre Eigenschaftsauflösung angewendet.
- Änderungen müssen gezielte Damage-Regionen erzeugen.
- Nach Abschluss einer Animation dürfen keine unnötigen Rendering-Aktualisierungen erfolgen.

## 11. Änderungsverarbeitung

Änderungen von Materialeigenschaften müssen gezielt verarbeitet werden.

Jede Eigenschaft kann folgende Auswirkungen deklarieren:

- **None:** Keine visuelle Auswirkung.
- **Uniform:** Aktualisierung von Rendering-Parametern.
- **Effect:** Neuberechnung betroffener Materialeffekte.
- **Geometry:** Änderung der visuellen Materialgeometrie.
- **Backdrop:** Änderung der Hintergrundabtastung.
- **Composition:** Änderung der Materialebenenkomposition.
- **Resource:** Änderung benötigter Rendering-Ressourcen.

Verbindliche Anforderungen:

- Änderungen müssen anhand ihrer tatsächlichen Auswirkungen verarbeitet werden.
- Unveränderte effektive Werte dürfen keine Invalidierung auslösen.
- Mehrere Änderungen dürfen zusammengefasst werden.
- Abhängige Eigenschaften müssen berücksichtigt werden.
- Die Änderungsverarbeitung muss deterministisch erfolgen.

## 12. Validierung

Materialeigenschaften müssen vor ihrer Verwendung validiert werden.

Anforderungen:

- Datentypen müssen geprüft werden.
- Wertebereiche müssen eingehalten werden.
- Ungültige numerische Werte müssen erkannt werden.
- Nicht unterstützte Eigenschaften müssen kontrolliert behandelt werden.
- Ressourcenreferenzen müssen gültig und autorisiert sein.
- Fehlerhafte Werte dürfen die Rendering-Pipeline nicht destabilisieren.

Bei ungültigen Werten wird die Änderung zurückgewiesen oder ein definierter Ersatzwert verwendet.

## 13. Styles, Templates und Themes

Material Properties bildet die gemeinsame Grundlage für Material Styles, Templates und Themes.

Verbindliche Anforderungen:

- Styles können Eigenschaftswerte bereitstellen.
- Templates können Eigenschaften und Standardwerte definieren.
- Themes können semantische Materialeigenschaften überschreiben.
- Lokale Materialinstanzen können zulässige individuelle Werte verwenden.
- Änderungen gemeinsam genutzter Ressourcen müssen abhängige Eigenschaften aktualisieren.
- Die Eigenschaftsauflösung darf keine unbeabsichtigten Änderungen gemeinsamer Definitionen verursachen.

## 14. Damage Tracking und Caching

Material Properties muss mit Nova Material Damage Tracking und Nova Material Caching zusammenarbeiten.

Anforderungen:

- Visuell relevante Änderungen erzeugen gezielte Damage-Informationen.
- Änderungen müssen ihre Effektabhängigkeiten berücksichtigen.
- Betroffene Cache-Einträge müssen invalidiert werden.
- Unveränderte Cache-Ergebnisse dürfen wiederverwendet werden.
- Nicht visuelle Änderungen dürfen keine Rendering-Operationen auslösen.
- Änderungen einzelner Eigenschaften dürfen keine unnötige vollständige Bildschirmaktualisierung verursachen.

## 15. Performance

Die Eigenschaftsverwaltung muss ressourcenschonend erfolgen.

Verbindliche Anforderungen:

- Eigenschaftsdefinitionen werden gemeinsam genutzt.
- Materialinstanzen speichern ausschließlich notwendige individuelle Werte.
- Aufgelöste Eigenschaftswerte dürfen zwischengespeichert werden.
- Änderungen müssen effizient an abhängige Komponenten weitergegeben werden.
- Wiederholte identische Werte dürfen keine redundanten Berechnungen auslösen.
- Eigenschaftsänderungen dürfen die funktionale Eingabeverarbeitung nicht unnötig blockieren.

## 16. Introspection und Designer

Materialeigenschaften müssen für den Nova UI Designer und die bestehenden NovaOS-Diagnosesysteme abfragbar sein.

Mindestens verfügbar sind:

- Eigenschaftsname
- Datentyp
- Standardwert
- Aktueller effektiver Wert
- Zulässiger Wertebereich
- Eigenschaftsflags
- Herkunft des effektiven Wertes
- Änderungs- und Animationsfähigkeit

Der Nova UI Designer muss unterstützte Eigenschaften anzeigen und bearbeiten können.

Die Darstellung und Bearbeitung erfolgen über das bestehende Nova UI Property System.

## 17. Determinismus und Sicherheit

Verbindliche Anforderungen:

- Die Eigenschaftsauflösung muss deterministisch erfolgen.
- Identische Eingaben müssen identische effektive Werte ergeben.
- Ungültige Eigenschaften dürfen keine unkontrollierten Speicherzugriffe verursachen.
- Ressourcenreferenzen müssen Sicherheitsgrenzen berücksichtigen.
- Benutzerdefinierte Materialien dürfen ausschließlich registrierte und autorisierte Eigenschaften verwenden.
- Fehlerhafte Bindungen oder Eigenschaftsänderungen dürfen die Rendering-Pipeline nicht destabilisieren.
- Ein deterministischer Betriebsmodus muss reproduzierbare Eigenschaftszustände ermöglichen.

## 18. Ergebnis

Nova Material Properties stellt ein einheitliches, typsicheres und deklaratives Eigenschaftssystem für sämtliche Materialien und Materialeffekte bereit.

Durch gemeinsame Eigenschaftsdefinitionen, Datenbindung, Animation, Validierung und gezielte Änderungsverarbeitung können Materialien flexibel konfiguriert und effizient aktualisiert werden.

Die Integration in das bestehende Nova UI Property System gewährleistet eine konsistente Verwaltung von Materialeigenschaften innerhalb der gesamten NovaOS-Oberfläche.