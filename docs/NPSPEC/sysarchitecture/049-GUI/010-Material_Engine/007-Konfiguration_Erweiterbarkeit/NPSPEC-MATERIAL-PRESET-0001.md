# NPSPEC-MATERIAL-PRESET-0001 – Nova Material Presets

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Material Engine

## 1. Zweck

Nova Material Presets definiert wiederverwendbare, vordefinierte Materialkonfigurationen innerhalb von NovaOS.

Presets ermöglichen die schnelle Anwendung abgestimmter Materialeigenschaften, ohne sämtliche Parameter einzeln konfigurieren zu müssen.

Ziel ist eine konsistente, flexible und ressourcenschonende Materialgestaltung.

## 2. Architektur

Material Presets ist Bestandteil des Nova Material Systems und verwendet die bestehenden Material Properties.

Die Zuständigkeiten sind:

- **Material System:** Registrierung und Verwaltung der Presets.
- **Material Properties:** Definition und Validierung der Eigenschaften.
- **Material Presets:** Bereitstellung vordefinierter Materialkonfigurationen.
- **Material Styles:** Wiederverwendung von Presets innerhalb der UI-Gestaltung.
- **Nova Theme System:** Bereitstellung themeabhängiger Presets.
- **Material Renderer:** Darstellung der resultierenden Materialien.

Material Presets implementiert keinen eigenständigen Renderer oder Style-Manager.

## 3. Preset-Modell

Ein Material Preset beschreibt eine benannte Materialkonfiguration.

Jedes Preset besitzt mindestens:

- **PresetId:** Eindeutige Kennung.
- **Name:** Bezeichnung des Presets.
- **MaterialType:** Zugehöriger Materialtyp.
- **Properties:** Vordefinierte Materialeigenschaften.
- **Version:** Version der Preset-Definition.
- **Description:** Optionale Beschreibung.

Presets müssen deklarativ definiert und eindeutig referenzierbar sein.

## 4. Standard-Presets

NovaOS kann vordefinierte Presets für die integrierten Materialtypen bereitstellen.

Beispiele:

### Acrylic

- AcrylicLight
- AcrylicDark
- AcrylicSubtle
- AcrylicStrong

### Liquid Glass

- LiquidGlassClear
- LiquidGlassSoft
- LiquidGlassDynamic
- LiquidGlassMinimal

### Frosted Glass

- FrostedLight
- FrostedDark
- FrostedSoft
- FrostedStrong

### Crystal

- CrystalClear
- CrystalSoft
- CrystalReflective
- CrystalMinimal

### Solid

- SolidLight
- SolidDark
- SolidAccent
- SolidTransparent

Die konkreten Eigenschaftswerte werden durch die jeweiligen Preset-Definitionen festgelegt.

Die genannten Bezeichnungen sind vorgesehene Standardressourcen und keine zusätzlichen Materialtypen.

## 5. Preset-Eigenschaften

Presets können sämtliche vom jeweiligen Materialtyp unterstützten Eigenschaften definieren.

Dazu gehören:

- Deckkraft
- Materialfarbe
- Hintergrundunschärfe
- Lichtbrechung
- Reflexionen
- Glanzlichter
- Oberflächenrauheit
- Beleuchtungsintensität
- Verzerrung
- Materialebenen
- Effektparameter

Nicht definierte Eigenschaften verwenden die reguläre Material-Property-Auflösung.

## 6. Preset-Anwendung

Materialinstanzen können ein Preset referenzieren.

Verbindliche Anforderungen:

- Ein Preset muss ohne Neuerstellung des UI-Elements angewendet werden können.
- Materialinstanzen können individuelle Eigenschaften überschreiben.
- Individuelle Änderungen dürfen die gemeinsame Preset-Definition nicht verändern.
- Presets müssen zur Laufzeit austauschbar sein.
- Nicht unterstützte Eigenschaften müssen kontrolliert behandelt werden.
- Die Anwendung muss über das bestehende Material Property System erfolgen.

## 7. Preset-Vererbung

Presets können auf bestehenden Presets aufbauen.

Anforderungen:

- Ein Preset kann ein Basis-Preset referenzieren.
- Nicht überschriebene Eigenschaften werden übernommen.
- Einzelne Eigenschaften können gezielt ersetzt werden.
- Die Auflösung muss deterministisch erfolgen.
- Zyklische Preset-Abhängigkeiten sind unzulässig.
- Änderungen eines Basis-Presets müssen abhängige Presets aktualisieren.

## 8. Styles und Templates

Material Presets müssen mit Nova Material Styles and Templates kompatibel sein.

Verbindliche Anforderungen:

- Styles können Presets referenzieren.
- Templates können Presets als Ausgangskonfiguration verwenden.
- Preset-Eigenschaften können durch Styles und lokale Materialeigenschaften überschrieben werden.
- Die endgültige Eigenschaftsauflösung erfolgt nach den Regeln des Nova UI Property Systems.
- Gemeinsam genutzte Presets bleiben unverändert.

## 9. Theme-Integration

Presets müssen mit dem Nova Theme System zusammenarbeiten.

Anforderungen:

- Themes können eigene Presets bereitstellen.
- Presets können semantische Theme-Ressourcen referenzieren.
- Theme-Wechsel müssen abhängige Materialinstanzen aktualisieren.
- Benutzerdefinierte Presets können Theme-Werte verwenden.
- Systemweite Barrierefreiheitsvorgaben besitzen Vorrang.
- Fehlende Theme-Ressourcen müssen kontrolliert ersetzt werden.

## 10. Benutzerdefinierte Presets

NovaOS unterstützt benutzerdefinierte Material Presets.

Verbindliche Anforderungen:

- Benutzer können eigene Presets erstellen und bearbeiten.
- Presets müssen speicherbar und wiederherstellbar sein.
- Benutzerdefinierte Presets können auf System-Presets basieren.
- Presets müssen über die vorgesehenen NovaOS-Konfigurationsmechanismen portierbar sein.
- Änderungen dürfen integrierte System-Presets nicht unbeabsichtigt verändern.
- Ungültige Preset-Definitionen müssen kontrolliert behandelt werden.

## 11. Dynamische Aktualisierung

Presets können zur Laufzeit geändert oder ausgetauscht werden.

Anforderungen:

- Änderungen müssen an abhängige Materialinstanzen weitergegeben werden.
- Nicht betroffene Eigenschaften bleiben unverändert.
- Änderungen dürfen keine unnötigen Layoutoperationen auslösen.
- Materialübergänge können über Nova Material Transitions erfolgen.
- Die Aktualisierung muss mit Material Damage Tracking kompatibel sein.
- Änderungen dürfen keine permanenten Rendering-Schleifen verursachen.

## 12. Ressourcenverwaltung

Material Presets werden über das bestehende NovaOS Resource System bereitgestellt.

Verbindliche Anforderungen:

- Preset-Definitionen müssen gemeinsam nutzbar sein.
- Identische Presets dürfen keine unnötigen Ressourcenduplikate erzeugen.
- Preset-Referenzen müssen eindeutig aufgelöst werden.
- Nicht mehr verwendete Ressourcen müssen freigegeben werden können.
- Die Ressourcenverwaltung muss mit Nova Material Caching kompatibel sein.

## 13. Performance

Die Verarbeitung von Material Presets muss ressourcenschonend erfolgen.

Anforderungen:

- Preset-Definitionen werden nach Möglichkeit unveränderlich gespeichert.
- Aufgelöste Preset-Eigenschaften dürfen zwischengespeichert werden.
- Änderungen invalidieren ausschließlich betroffene Materialbereiche.
- Identische Materialkonfigurationen dürfen Rendering-Ressourcen gemeinsam verwenden.
- Preset-Wechsel dürfen keine unnötigen vollständigen Bildschirmaktualisierungen verursachen.
- Die Verarbeitung darf die funktionale Eingabeverarbeitung nicht unnötig blockieren.

## 14. Adaptive Qualität und Fallback

Presets müssen mit Nova Adaptive Material Quality und Nova Material Rendering Fallback kompatibel sein.

Verbindliche Anforderungen:

- Presets beschreiben die gewünschte Materialdarstellung unabhängig von der verfügbaren Rendering-Qualität.
- Die tatsächliche Effektqualität wird durch das Quality Management bestimmt.
- Nicht verfügbare Effekte müssen kontrolliert vereinfacht oder ersetzt werden.
- Preset-Definitionen bleiben bei Qualitätsreduzierungen unverändert.
- Eine funktional gleichwertige Fallback-Darstellung muss verfügbar sein.

## 15. Designer-Integration

Material Presets müssen im Nova UI Designer verfügbar sein.

Anforderungen:

- Verfügbare Presets müssen aufgelistet werden können.
- Presets müssen visuell auswählbar sein.
- Eigenschaftswerte müssen bearbeitet werden können.
- Benutzerdefinierte Presets müssen erstellt und gespeichert werden können.
- Änderungen müssen mit der deklarativen Materialdefinition synchronisiert werden.
- Die Vorschau muss die aktive Theme- und Qualitätskonfiguration berücksichtigen.

## 16. Determinismus und Sicherheit

Verbindliche Anforderungen:

- Preset-Definitionen müssen validiert werden.
- Eigenschaftswerte müssen den registrierten Material Properties entsprechen.
- Zyklische Preset-Abhängigkeiten müssen erkannt werden.
- Ressourcenreferenzen müssen gültig und autorisiert sein.
- Benutzerdefinierte Presets dürfen keine Sicherheitsgrenzen der Material Engine umgehen.
- Fehlerhafte Presets dürfen die Rendering-Pipeline nicht destabilisieren.
- Bei ungültigen Presets muss eine definierte Standardkonfiguration verwendet werden.
- Die Preset-Auflösung muss deterministisch erfolgen.

## 17. Ergebnis

Nova Material Presets stellt eine einheitliche Grundlage für wiederverwendbare Materialkonfigurationen innerhalb der Nova Material Engine bereit.

Durch vordefinierte Eigenschaften, Preset-Vererbung, Theme-Integration und individuelle Anpassungsmöglichkeiten können Materialien schnell und konsistent gestaltet werden.

Die Integration in Material Properties, Styles, Templates und den Nova UI Designer ermöglicht eine flexible, ressourcenschonende und benutzerfreundliche Materialkonfiguration.