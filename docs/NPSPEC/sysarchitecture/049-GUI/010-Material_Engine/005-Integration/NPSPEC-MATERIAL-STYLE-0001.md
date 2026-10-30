# NPSPEC-MATERIAL-STYLE-0001 – Nova Material Styles and Templates

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Material Engine

## 1. Zweck

Nova Material Styles and Templates definiert die wiederverwendbare Gestaltung und Konfiguration von Materialien innerhalb von NovaOS.

Das System ermöglicht die zentrale Definition von Materialeigenschaften, die Wiederverwendung gemeinsamer Darstellungsregeln und die Anpassung von Materialien an unterschiedliche UI-Zustände.

Ziel ist eine konsistente, deklarative und ressourcenschonende Materialgestaltung.

## 2. Architektur

Material Styles and Templates verwendet das bestehende Nova UI Style System und das Nova Material System.

Die Zuständigkeiten sind:

- **Material System:** Verwaltung von Materialdefinitionen und Materialinstanzen.
- **Material Styles:** Wiederverwendbare Konfiguration von Materialeigenschaften.
- **Material Templates:** Definition der Materialstruktur und Effektkombination.
- **Nova UI Style System:** Anwendung von Styles auf UI-Elemente.
- **Nova Theme System:** Bereitstellung systemweiter Darstellungsvorgaben.
- **Material Renderer:** Darstellung der resultierenden Materialien.

Es wird keine eigenständige Style- oder Template-Laufzeit implementiert.

## 3. Material Styles

Ein Material Style beschreibt eine wiederverwendbare Konfiguration von Materialeigenschaften.

Verbindliche Anforderungen:

- Styles müssen deklarativ definierbar sein.
- Styles können auf bestehende Materialdefinitionen verweisen.
- Mehrere Materialinstanzen können denselben Style verwenden.
- Styles können individuelle Materialeigenschaften festlegen.
- Styles müssen zur Laufzeit austauschbar sein.
- Änderungen eines Styles müssen betroffene Materialinstanzen aktualisieren.
- Gemeinsame Style-Definitionen bleiben bei individuellen Änderungen unverändert.

## 4. Material Templates

Ein Material Template beschreibt die strukturelle Zusammensetzung eines Materials.

Templates können definieren:

- Basismaterial
- Aktivierte Materialeffekte
- Effektparameter
- Materialmasken
- Effektkombinationen
- Zustandsabhängige Darstellungsregeln
- Standardwerte für Materialeigenschaften

Verbindliche Anforderungen:

- Templates müssen deklarativ definiert werden.
- Templates dürfen keine eigene Rendering-Pipeline implementieren.
- Templates verwenden ausschließlich registrierte Materialeffekte.
- Templates müssen wiederverwendbar sein.
- Änderungen eines Templates dürfen keine Neuerstellung des zugehörigen UI-Elements erfordern.
- Ungültige Effektkombinationen müssen kontrolliert behandelt werden.

## 5. Style-Vererbung

Material Styles unterstützen die Ableitung bestehender Styles.

Anforderungen:

- Ein Style kann auf einem anderen Style basieren.
- Abgeleitete Styles übernehmen nicht überschriebene Eigenschaften.
- Einzelne Eigenschaften können gezielt überschrieben werden.
- Die Auflösung der Vererbung muss eindeutig und deterministisch erfolgen.
- Zyklische Vererbungsbeziehungen sind unzulässig.
- Änderungen eines Basis-Styles müssen abhängige Styles aktualisieren.

## 6. Eigenschaften und Prioritäten

Materialeigenschaften können aus mehreren Quellen stammen.

Die Auflösung erfolgt in folgender Priorität, von niedrig nach hoch:

1. Standardwerte des Materialtyps
2. Material Template
3. Material Style
4. Theme-Vorgaben
5. Lokale Eigenschaften der Materialinstanz
6. Aktive Zustandsregeln
7. Aktive Animationen

Systemweite Sicherheits- und Barrierefreiheitsvorgaben besitzen unabhängig davon Vorrang.

Die effektiven Materialeigenschaften müssen jederzeit eindeutig bestimmbar sein.

## 7. Zustandsabhängige Styles

Material Styles können unterschiedliche Darstellungen für UI-Zustände definieren.

Mindestens unterstützt werden:

- Normal
- Hovered
- Pressed
- Focused
- Selected
- Disabled
- Active
- Inactive

Anforderungen:

- Zustandsregeln dürfen einzelne Materialeigenschaften überschreiben.
- Mehrere gleichzeitig aktive Zustände müssen eindeutig aufgelöst werden.
- Zustandsänderungen können Materialanimationen auslösen.
- Die Zustandsverwaltung erfolgt über das bestehende Nova UI-System.
- Funktionale UI-Zustände dürfen nicht durch Material Styles verändert werden.

## 8. Ressourcenverwaltung

Styles und Templates werden über das bestehende Nova UI Resource System bereitgestellt.

Verbindliche Anforderungen:

- Ressourcen müssen eindeutig referenzierbar sein.
- Gemeinsame Ressourcen dürfen von mehreren UI-Elementen verwendet werden.
- Ressourcen können lokal oder systemweit definiert werden.
- Änderungen gemeinsam genutzter Ressourcen müssen abhängige Materialinstanzen aktualisieren.
- Nicht mehr verwendete Ressourcen müssen freigegeben werden können.
- Individuelle Materialzustände dürfen gemeinsam genutzte Definitionen nicht verändern.

## 9. Dynamische Anpassung

Material Styles und Templates müssen zur Laufzeit verändert oder ausgetauscht werden können.

Unterstützt werden:

- Wechsel zwischen Material Styles
- Austausch von Material Templates
- Änderungen gemeinsamer Style-Eigenschaften
- Zustandsabhängige Materialanpassungen
- Theme-Wechsel
- Benutzerdefinierte Darstellungsregeln

Änderungen müssen über das bestehende Material-Property-System verarbeitet werden.

Visuelle Übergänge verwenden Nova Material Transitions und Nova Material Animation.

## 10. UI-Integration

Material Styles und Templates müssen vollständig mit dem nativen Nova UI-System kompatibel sein.

Anforderungen:

- Styles können über UI-Elemente referenziert werden.
- Templates können innerhalb von UI-Styles verwendet werden.
- Materialeigenschaften müssen Datenbindungen unterstützen.
- Der visuelle Nova UI Designer muss Material Styles und Templates bearbeiten können.
- Änderungen im Designer müssen mit der deklarativen Definition synchronisiert werden.
- Material Styles dürfen keine Abhängigkeit von HTML, CSS oder JavaScript voraussetzen.

## 11. Theme-Integration

Material Styles und Templates müssen mit dem Nova Theme System zusammenarbeiten.

Verbindliche Anforderungen:

- Styles können systemweite Farb- und Materialressourcen referenzieren.
- Themes können Materialeigenschaften und Styles bereitstellen.
- Theme-Wechsel müssen bestehende Materialinstanzen aktualisieren.
- Benutzerdefinierte Styles müssen mit systemweiten Themes kombinierbar sein.
- Sicherheits- und Barrierefreiheitsvorgaben dürfen nicht durch Themes überschrieben werden.

## 12. Damage Tracking

Änderungen von Styles und Templates müssen das Damage Tracking des Nova Compositors berücksichtigen.

Anforderungen:

- Ausschließlich tatsächlich betroffene Materialbereiche werden invalidiert.
- Änderungen gemeinsam genutzter Styles aktualisieren nur abhängige Materialinstanzen.
- Unveränderte Rendering-Ergebnisse dürfen wiederverwendet werden.
- Änderungen von Effektparametern müssen deren Wirkungsbereiche berücksichtigen.
- Unsichtbare Materialinstanzen dürfen keine unnötigen Rendering-Operationen auslösen.
- Style- und Template-Änderungen dürfen keine permanente vollständige Bildschirmaktualisierung verursachen.

## 13. Performance

Die Verarbeitung muss ressourcenschonend erfolgen.

Verbindliche Anforderungen:

- Styles und Templates werden nach Möglichkeit als unveränderliche Definitionen gemeinsam genutzt.
- Aufgelöste Eigenschaften dürfen zwischengespeichert werden.
- Änderungen müssen ausschließlich betroffene Eigenschaften neu auswerten.
- Identische Materialkonfigurationen dürfen Rendering-Ressourcen gemeinsam verwenden.
- Vererbungsbeziehungen müssen effizient aufgelöst werden.
- Nicht verwendete Style- und Template-Ressourcen müssen freigegeben werden können.

## 14. Barrierefreiheit

Material Styles und Templates müssen systemweite Einstellungen zur Barrierefreiheit berücksichtigen.

Anforderungen:

- Transparenz und Animationen müssen reduzierbar sein.
- Kontrastvorgaben müssen Vorrang besitzen.
- Fokuszustände müssen eindeutig erkennbar bleiben.
- Benutzerdefinierte Barrierefreiheitseinstellungen müssen berücksichtigt werden.
- Bei deaktivierten Materialeffekten muss eine funktional gleichwertige Darstellung erhalten bleiben.

## 15. Determinismus und Sicherheit

Die Auflösung von Material Styles und Templates muss deterministisch erfolgen.

Verbindliche Anforderungen:

- Style- und Template-Definitionen müssen validiert werden.
- Zyklische Vererbungs- und Referenzbeziehungen müssen erkannt werden.
- Ungültige Materialeigenschaften müssen kontrolliert behandelt werden.
- Styles dürfen ausschließlich autorisierte Materialeffekte verwenden.
- Benutzerdefinierte Templates dürfen keine Sicherheitsgrenzen des Material Systems umgehen.
- Fehlerhafte Definitionen dürfen die Rendering-Pipeline nicht destabilisieren.
- Bei nicht auflösbaren Styles oder Templates wird ein definiertes Material-Fallback verwendet.

## 16. Ergebnis

Nova Material Styles and Templates stellt eine einheitliche Grundlage für wiederverwendbare Materialgestaltungen innerhalb der Nova Material Engine bereit.

Durch deklarative Styles, strukturelle Templates, Vererbung und zustandsabhängige Eigenschaften können Materialien zentral definiert und individuell angepasst werden.

Die Integration in das bestehende Nova UI Style System ermöglicht konsistente, dynamische und ressourcenschonende Oberflächen.