# NPSPEC-MATERIAL-UI-0001 – Nova UI Material Integration

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Material Engine

## 1. Zweck

Nova UI Material Integration definiert die Einbindung der Nova Material Engine in das native, deklarative UI-System von NovaOS.

Materialien ermöglichen die Darstellung von Acrylic, Liquid Glass, Frosted Glass, Crystal und Solid auf UI-Elementen.

Ziel ist eine einheitliche, deklarative und ressourcenschonende Materialverwendung ohne direkte Abhängigkeit der UI-Elemente von Rendering-Implementierungen.

## 2. Architektur

Die Integration erfolgt über das bestehende Nova UI-System.

Die Zuständigkeiten sind:

- **Nova UI:** Definition von Elementen, Layout und visuellen Eigenschaften.
- **Material System:** Verwaltung von Materialdefinitionen und Materialinstanzen.
- **UI Style System:** Zuweisung von Materialien über Styles und Templates.
- **Animation System:** Animation von Materialeigenschaften.
- **Material Renderer:** Verarbeitung der Materialeffekte.
- **Compositor:** Darstellung, Ebenenkomposition und Damage Tracking.

Nova UI implementiert keine eigenen Materialeffekte.

## 3. Materialzuweisung

Jedes geeignete UI-Element kann eine Materialreferenz besitzen.

Verbindliche Anforderungen:

- Materialien müssen deklarativ zuweisbar sein.
- Materialien müssen zur Laufzeit austauschbar sein.
- Materialdefinitionen können von mehreren UI-Elementen gemeinsam verwendet werden.
- Jedes Element kann individuelle Materialeigenschaften überschreiben.
- Materialwechsel dürfen keine Neuerstellung des UI-Elements erfordern.
- Elemente ohne explizite Materialzuweisung verwenden die Vorgaben ihres Styles oder Themes.

## 4. Materialeigenschaft

Das Nova UI-System stellt eine einheitliche Materialeigenschaft bereit.

Die Eigenschaft **Material** unterstützt:

- Referenzen auf bestehende Materialdefinitionen
- Material-Presets
- Inline-Materialdefinitionen
- Dynamische Materialbindungen
- Materialinstanzen mit individuellen Eigenschaften

Die Materialeigenschaft muss mit dem bestehenden UI-Property-System kompatibel sein.

## 5. Deklarative Definition

Materialien müssen innerhalb der nativen Nova UI-Beschreibung definiert werden können.

Beispiel einer konzeptionellen Deklaration:

```xml
<Button Content="Start">
    <Button.Material>
        <LiquidGlass
            Opacity="0.75"
            BlurRadius="16"
            Refraction="0.4"
            HighlightIntensity="0.6" />
    </Button.Material>
</Button>
```

Die konkrete Serialisierung richtet sich nach der Nova UI-Sprachdefinition.

Die Material Engine darf keine Abhängigkeit von HTML, CSS oder JavaScript voraussetzen.

## 6. Materialbindung

Materialeigenschaften müssen das bestehende Nova UI Data Binding unterstützen.

Anforderungen:

- Materialeigenschaften können an Datenquellen gebunden werden.
- Änderungen gebundener Werte aktualisieren die betroffene Materialinstanz.
- Bindungen müssen mit dem UI-Property-System zusammenarbeiten.
- Ungültige Bindungswerte müssen kontrolliert behandelt werden.
- Änderungen dürfen keine vollständige Neuerstellung des UI-Elements erzwingen.

## 7. UI-Zustände

Materialien können auf Zustände von UI-Elementen reagieren.

Mindestens unterstützt werden:

- Normal
- Hovered
- Pressed
- Focused
- Selected
- Disabled
- Active
- Inactive

Die Zustandsverwaltung erfolgt durch Nova UI.

Materialreaktionen verwenden Nova Interactive Material Response und Nova Material Animation.

## 8. Styles und Templates

Materialien müssen mit dem Nova UI Style System kompatibel sein.

Verbindliche Anforderungen:

- Styles können Materialien definieren.
- Templates können Materialeigenschaften verwenden.
- Materialdefinitionen können als gemeinsame Ressourcen bereitgestellt werden.
- Individuelle Eigenschaften können Style-Vorgaben überschreiben.
- Änderungen eines Styles müssen betroffene Materialinstanzen aktualisieren.
- Materialien dürfen nicht fest an bestimmte Control-Typen gebunden sein.

## 9. Layout und Geometrie

Materialien verwenden die vom UI-System bereitgestellte Geometrie.

Anforderungen:

- Materialeffekte berücksichtigen Größe, Position und Form des UI-Elements.
- Abgerundete Ecken und individuelle Geometrien müssen unterstützt werden.
- Clipping und Materialmasken müssen berücksichtigt werden.
- Materialeffekte dürfen das Layout nicht eigenständig verändern.
- Visuelle Verformungen dürfen die Eingabegeometrie nicht unbeabsichtigt beeinflussen.
- Änderungen der Elementgeometrie müssen abhängige Materialeffekte aktualisieren.

## 10. Rendering-Integration

Nova UI übergibt Materialreferenzen und die benötigten Darstellungsinformationen an die bestehende Rendering-Pipeline.

Verbindliche Anforderungen:

- UI-Elemente dürfen keine direkten GPU-Operationen ausführen.
- Materialeffekte werden ausschließlich durch die Material Rendering Pipeline verarbeitet.
- Hintergrundabhängige Effekte verwenden autorisierte Compositor-Schnittstellen.
- Die korrekte Ebenenreihenfolge muss erhalten bleiben.
- Materialien müssen mit dem bestehenden Graphics Backend kompatibel sein.
- GPU- und CPU-Rendering müssen über dieselbe Materialabstraktion erreichbar sein.

## 11. Damage Tracking

Die Materialintegration muss vollständig mit dem Damage Tracking des Nova Compositors kompatibel sein.

Anforderungen:

- Änderungen von Materialeigenschaften invalidieren ausschließlich betroffene Bereiche.
- Änderungen der UI-Geometrie aktualisieren abhängige Materialbereiche.
- Hintergrundabhängige Materialien berücksichtigen ihre erweiterten Abtastbereiche.
- Unveränderte Materialergebnisse dürfen wiederverwendet werden.
- Unsichtbare UI-Elemente dürfen keine unnötigen Materialberechnungen auslösen.
- Materialanimationen dürfen keine permanente vollständige Bildschirmaktualisierung verursachen.

## 12. Performance

Die Integration muss ressourcenschonend erfolgen.

Verbindliche Anforderungen:

- Gemeinsame Materialdefinitionen werden wiederverwendet.
- Materialinstanzen speichern ausschließlich notwendige individuelle Zustände.
- Materialänderungen dürfen keine unnötigen Layoutberechnungen auslösen.
- Rendering-Ressourcen können zwischen kompatiblen Materialinstanzen geteilt werden.
- Nicht sichtbare Materialien dürfen von der aktiven Verarbeitung ausgeschlossen werden.
- Die Material Engine muss die bestehenden Qualitätsstufen und Fallback-Verfahren unterstützen.

## 13. Barrierefreiheit

Die Materialintegration muss systemweite Einstellungen zur Barrierefreiheit berücksichtigen.

Anforderungen:

- Transparenz, Animationen und dekorative Effekte müssen reduzierbar sein.
- Kontrastvorgaben müssen Vorrang vor dekorativen Materialeigenschaften besitzen.
- Fokus- und Interaktionszustände müssen unabhängig von Materialeffekten erkennbar bleiben.
- UI-Elemente müssen auch ohne GPU-beschleunigte Materialeffekte bedienbar bleiben.
- Bei deaktivierten Materialeffekten muss eine funktional gleichwertige Darstellung erhalten bleiben.

## 14. Sicherheit und Fehlerbehandlung

Verbindliche Anforderungen:

- Materialreferenzen und Eigenschaften müssen validiert werden.
- UI-Elemente dürfen ausschließlich autorisierte Materialressourcen verwenden.
- Materialien dürfen keine Sicherheitsgrenzen des Compositors umgehen.
- Fehlerhafte Materialdefinitionen dürfen das UI-System nicht destabilisieren.
- Nicht unterstützte Effekte müssen kontrolliert ersetzt oder deaktiviert werden.
- Bei Materialfehlern wird ein definiertes Solid-Fallback verwendet.

## 15. Ergebnis

Nova UI Material Integration verbindet die Nova Material Engine mit dem nativen, deklarativen UI-System von NovaOS.

Materialien können über Eigenschaften, Styles, Templates und Datenbindungen verwendet und dynamisch verändert werden.

Die Trennung zwischen UI-Definition, Materialverarbeitung und Rendering ermöglicht einheitliche, anpassbare und performante Oberflächen ohne direkte Abhängigkeit von einer bestimmten Grafikimplementierung.