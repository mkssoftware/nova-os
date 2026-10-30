# NPSPEC-MATERIAL-THEME-0001 – Nova Material Theme Integration

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Material Engine

## 1. Zweck

Nova Material Theme Integration definiert die Einbindung der Nova Material Engine in das bestehende Nova Theme System.

Materialien sollen sich automatisch an das aktive Systemdesign, Benutzerpräferenzen und Barrierefreiheitseinstellungen anpassen können.

Ziel ist eine konsistente, dynamische und ressourcenschonende Materialdarstellung innerhalb der gesamten NovaOS-Oberfläche.

## 2. Architektur

Die Theme-Integration verwendet die bestehenden Systeme von NovaOS.

Die Zuständigkeiten sind:

- **Nova Theme System:** Verwaltung von Themes und systemweiten Darstellungsvorgaben.
- **Material System:** Verwaltung von Materialdefinitionen und Materialinstanzen.
- **Material Styles:** Anwendung wiederverwendbarer Materialkonfigurationen.
- **Material Theme Integration:** Auflösung themeabhängiger Materialeigenschaften.
- **Material Renderer:** Darstellung der resultierenden Materialien.
- **Compositor:** Komposition und Damage Tracking.

Die Material Engine implementiert keine eigenständige Theme-Verwaltung.

## 3. Theme-Ressourcen

Themes können Materialressourcen bereitstellen.

Unterstützt werden:

- Materialtypen
- Material-Presets
- Material Styles
- Material Templates
- Materialfarben
- Transparenzwerte
- Unschärfeparameter
- Reflexions- und Beleuchtungseigenschaften
- Animations- und Übergangsvorgaben

Theme-Ressourcen müssen eindeutig referenzierbar und wiederverwendbar sein.

## 4. Semantische Materialressourcen

NovaOS unterstützt semantische Materialreferenzen.

Beispiele:

- **WindowMaterial:** Standardmaterial für Fenster.
- **DialogMaterial:** Material für Dialogfenster.
- **PanelMaterial:** Material für Bedienfelder.
- **ToolbarMaterial:** Material für Werkzeugleisten.
- **TaskbarMaterial:** Material der Taskbar.
- **MenuMaterial:** Material für Menüs.
- **PopupMaterial:** Material für schwebende Elemente.
- **ControlMaterial:** Standardmaterial für Bedienelemente.

Die konkrete Materialimplementierung wird durch das aktive Theme bestimmt.

UI-Elemente müssen dadurch unabhängig vom verwendeten Materialtyp bleiben können.

## 5. Theme-Auflösung

Materialeigenschaften können aus unterschiedlichen Quellen stammen.

Die Auflösung muss die bestehende Prioritätsordnung des Nova UI Style Systems berücksichtigen.

Verbindliche Anforderungen:

- Theme-Ressourcen werden über das bestehende Resource System aufgelöst.
- Lokale Materialeigenschaften können Theme-Vorgaben überschreiben.
- Zustandsregeln und Animationen werden entsprechend der definierten Property-Prioritäten angewendet.
- Fehlende Theme-Ressourcen verwenden definierte Standardwerte.
- Die Auflösung muss deterministisch erfolgen.
- Sicherheits- und Barrierefreiheitsvorgaben besitzen Vorrang vor dekorativen Einstellungen.

## 6. Hell- und Dunkelmodus

Materialien müssen unterschiedliche Systemfarbschemata unterstützen.

Anforderungen:

- Materialien können auf helle und dunkle Themes reagieren.
- Materialfarben und Transparenzwerte können abhängig vom Farbschema variieren.
- Reflexionen und Glanzlichter müssen an die jeweilige Darstellung angepasst werden können.
- Hintergrundabhängige Materialien müssen ausreichende Kontraste ermöglichen.
- Der Wechsel des Farbschemas darf keine Neuerstellung von UI-Elementen erfordern.

## 7. Akzentfarben

Materialien können die systemweite Akzentfarbe verwenden.

Verbindliche Anforderungen:

- Akzentfarben werden über das Nova Theme System bereitgestellt.
- Materialfarben können direkt oder indirekt an Akzentfarben gebunden werden.
- Änderungen der Akzentfarbe aktualisieren betroffene Materialinstanzen.
- Individuelle Materialfarben müssen weiterhin möglich sein.
- Die Farbanpassung muss mit dem NovaOS Color Management kompatibel sein.

## 8. Dynamischer Theme-Wechsel

Das aktive Theme kann zur Laufzeit gewechselt werden.

Anforderungen:

- Bestehende Materialinstanzen müssen aktualisiert werden.
- Unveränderte Materialeigenschaften bleiben erhalten.
- Theme-Wechsel dürfen keine unnötigen Layoutänderungen verursachen.
- Visuelle Übergänge verwenden Nova Material Transitions.
- Laufende Animationen müssen kontrolliert angepasst oder beendet werden.
- Nach Abschluss des Wechsels dürfen keine veralteten Theme-Ressourcen verwendet werden.

## 9. Benutzerdefinierte Themes

NovaOS unterstützt benutzerdefinierte Materialdarstellungen.

Anforderungen:

- Benutzer können Materialtypen und Materialeigenschaften innerhalb zulässiger Grenzen anpassen.
- Benutzerdefinierte Themes können bestehende Materialressourcen überschreiben.
- Gemeinsame Materialdefinitionen dürfen nicht unbeabsichtigt verändert werden.
- Änderungen müssen speicherbar und wiederherstellbar sein.
- Themes müssen über die vorgesehenen NovaOS-Konfigurationsmechanismen portierbar sein.
- Ungültige Konfigurationen müssen kontrolliert behandelt werden.

## 10. Systemweite Darstellungsvorgaben

Das Nova Theme System kann globale Materialvorgaben bereitstellen.

Dazu gehören:

- Transparenzintensität
- Hintergrundunschärfe
- Reflexionsintensität
- Glanzlichtintensität
- Materialanimationen
- Kontrastverhalten
- Bevorzugte Materialqualität

Diese Vorgaben müssen mit individuellen Materialeigenschaften kombinierbar sein.

## 11. Damage Tracking

Theme-Änderungen müssen mit dem Damage Tracking des Nova Compositors kompatibel sein.

Verbindliche Anforderungen:

- Ausschließlich betroffene Materialinstanzen werden aktualisiert.
- Unveränderte Materialbereiche dürfen nicht erneut verarbeitet werden.
- Änderungen gemeinsam genutzter Theme-Ressourcen müssen gezielt propagiert werden.
- Hintergrundabhängige Effekte müssen ihre Abtastbereiche berücksichtigen.
- Unsichtbare Materialinstanzen dürfen keine unnötigen Rendering-Operationen auslösen.
- Theme-Wechsel dürfen keine permanente vollständige Bildschirmaktualisierung verursachen.

## 12. Performance

Die Theme-Integration muss ressourcenschonend erfolgen.

Anforderungen:

- Gemeinsame Theme-Ressourcen werden wiederverwendet.
- Aufgelöste Materialeigenschaften dürfen zwischengespeichert werden.
- Änderungen werden ausschließlich an abhängige Materialinstanzen weitergegeben.
- Identische Materialkonfigurationen dürfen Rendering-Ressourcen gemeinsam verwenden.
- Nicht mehr verwendete Theme-Ressourcen müssen freigegeben werden können.
- Theme-Wechsel dürfen die funktionale Eingabeverarbeitung nicht blockieren.

## 13. Barrierefreiheit

Systemweite Barrierefreiheitseinstellungen müssen unabhängig vom aktiven Theme wirksam bleiben.

Verbindliche Anforderungen:

- Hohe Kontrasteinstellungen besitzen Vorrang.
- Transparenz und dekorative Materialeffekte müssen reduzierbar sein.
- Animationen müssen entsprechend den Systemeinstellungen angepasst werden.
- Texte und Bedienelemente müssen lesbar bleiben.
- Fokus- und Interaktionszustände müssen eindeutig erkennbar sein.
- Eine funktional gleichwertige Solid-Darstellung muss verfügbar sein.

## 14. Sicherheit und Fehlerbehandlung

Verbindliche Anforderungen:

- Theme-Ressourcen und Materialreferenzen müssen validiert werden.
- Benutzerdefinierte Themes dürfen ausschließlich autorisierte Materialfunktionen verwenden.
- Theme-Definitionen dürfen keine Sicherheitsgrenzen des Material Systems oder Compositors umgehen.
- Zyklische Ressourcenreferenzen müssen erkannt werden.
- Fehlerhafte Theme-Ressourcen dürfen die Rendering-Pipeline nicht destabilisieren.
- Bei nicht auflösbaren Materialressourcen wird ein definiertes Standardmaterial verwendet.
- Ein fehlerhaftes Theme muss durch ein funktionsfähiges System-Theme ersetzbar sein.

## 15. Ergebnis

Nova Material Theme Integration verbindet die Nova Material Engine mit dem bestehenden Nova Theme System.

Durch semantische Materialressourcen, dynamische Theme-Auflösung und systemweite Darstellungsvorgaben können sämtliche NovaOS-Materialien konsistent an das aktive Erscheinungsbild angepasst werden.

Die Integration ermöglicht individuelle, barrierefreie und ressourcenschonende Materialdarstellungen ohne direkte Bindung der UI-Elemente an konkrete Materialtypen.