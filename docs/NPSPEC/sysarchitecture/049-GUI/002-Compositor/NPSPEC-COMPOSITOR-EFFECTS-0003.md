# NPSPEC-COMPOSITOR-EFFECTS-0003 – Nova Blur, Acrylic and Glass Effects

**Status:** Angenommen  
**Bereich:** Compositor / Hintergrundeffekte  
**System:** NovaOS

## 1. Zweck

Nova Blur, Acrylic and Glass Effects definiert die Darstellung von Unschärfe-, Acrylic- und Glaseffekten innerhalb des Nova Compositors.

Ziel ist eine moderne, an Fluent Design angelehnte Benutzeroberfläche mit räumlicher Tiefe, guter Lesbarkeit und geringem Ressourcenverbrauch.

## 2. Architektur

- Integration in Nova Compositor Pipeline und Layer Composition.
- GPU-beschleunigte Effektverarbeitung mit Software-Fallback.
- Verwendung nativer Nova UI Materialdefinitionen.
- Trennung von Materialeigenschaften und Rendering-Implementierung.
- Unterstützung von Hintergrundunschärfe und Oberflächentransparenz.
- Integration in Nova Color Management.
- Kombination mit Damage Tracking und Occlusion Culling.
- Zwischenspeicherung unveränderter Effektergebnisse.
- Adaptive Qualitätssteuerung abhängig von Systemressourcen.
- Keine Abhängigkeit von HTML, CSS oder JavaScript.

## 3. Kernfunktionen

- Background Blur für Fenster und UI-Elemente.
- Acrylic-Materialien mit Unschärfe, Farbüberlagerung und optionaler Textur.
- Glass-Materialien mit Transparenz und subtilen Lichtreflexionen.
- Einstellbare Unschärferadien.
- Konfigurierbare Materialfarben und Deckkraft.
- Unterstützung abgerundeter Konturen.
- Animierte Materialübergänge.
- Anpassung an Dark- und Light-Themes.
- Wiederverwendung berechneter Hintergrundbereiche.
- Automatische Qualitätsreduzierung bei hoher Systemlast.
- Deaktivierung dekorativer Effekte im Energiesparmodus.

## 4. Normative Anforderungen

- EFFECTS-001: Der Compositor MUSS Hintergrundunschärfe für definierte Surface-Bereiche unterstützen.
- EFFECTS-002: Acrylic und Glass MÜSSEN als eigenständige Materialtypen definiert werden.
- EFFECTS-003: Materialeigenschaften MÜSSEN unabhängig vom Rendering-Backend beschrieben werden.
- EFFECTS-004: Hintergrundeffekte MÜSSEN die gültige Ebenenreihenfolge berücksichtigen.
- EFFECTS-005: Blur DARF ausschließlich auf dafür freigegebene Hintergrundinhalte zugreifen.
- EFFECTS-006: Geschützte Inhalte MÜSSEN bei der Effektberechnung vor unzulässiger Offenlegung geschützt werden.
- EFFECTS-007: Unschärferadius, Farbüberlagerung und Deckkraft MÜSSEN konfigurierbar sein.
- EFFECTS-008: Effekte MÜSSEN Clipping, Transformationen und abgerundete Konturen berücksichtigen.
- EFFECTS-009: Änderungen des Hintergrunds MÜSSEN die betroffenen Effektbereiche invalidieren.
- EFFECTS-010: Damage-Regionen MÜSSEN entsprechend dem Einflussbereich des Unschärfefilters erweitert werden.
- EFFECTS-011: Unveränderte Effektergebnisse SOLLEN zwischengespeichert werden.
- EFFECTS-012: Mehrfachberechnungen identischer Hintergrundbereiche SOLLEN vermieden werden.
- EFFECTS-013: Effektberechnungen DÜRFEN keine zyklischen Abhängigkeiten innerhalb der Kompositionspipeline erzeugen.
- EFFECTS-014: Die Lesbarkeit von Text und Bedienelementen MUSS durch geeignete Materialparameter gewährleistet werden.
- EFFECTS-015: Bei Ressourcenknappheit MÜSSEN dekorative Effekte reduziert oder deaktiviert werden können.
- EFFECTS-016: Bei fehlender GPU-Beschleunigung MUSS eine vereinfachte Darstellung verfügbar sein.
- EFFECTS-017: Effektanimationen MÜSSEN mit Nova Compositor Scheduling abgestimmt werden.
- EFFECTS-018: Effekte DÜRFEN die Eingabeverarbeitung und Fensterverwaltung nicht blockieren.

## 5. Materialmodell

### Blur

Weichzeichnung der hinter einer Oberfläche liegenden Bildinhalte.

Eigenschaften:
- Blur Radius
- Blur Quality
- Clip Region

### Acrylic

Halbtransparentes Material mit weichgezeichnetem Hintergrund und farblicher Tönung.

Eigenschaften:
- Blur Radius
- Tint Color
- Tint Opacity
- Material Opacity
- Optional Noise Texture

### Glass

Transparentes Material mit dezenter Hintergrundunschärfe und optionalen Licht- und Kanteneffekten.

Eigenschaften:
- Transparency
- Blur Radius
- Tint Color
- Edge Highlight
- Reflection Intensity

### Solid Fallback

Deckendes oder halbtransparentes Ersatzmaterial ohne aufwendige Hintergrundberechnung.

## 6. Effektverarbeitung

1. **Collect:** Sichtbare Materialflächen und Effektparameter erfassen.
2. **Resolve:** Zulässige Hintergrundinhalte bestimmen.
3. **Capture:** Benötigte Hintergrundbereiche bereitstellen.
4. **Blur:** Hintergrund mit dem gewählten Filter weichzeichnen.
5. **Material:** Farbüberlagerung, Transparenz und optionale Texturen anwenden.
6. **Clip:** Ergebnis auf die Materialkontur begrenzen.
7. **Compose:** Effektfläche in die Szene integrieren.
8. **Cache:** Wiederverwendbare Ergebnisse speichern.
9. **Present:** Fertigen Frame ausgeben.

Die Hintergrundberechnung MUSS auf einem eindeutig definierten Kompositionszustand basieren.

## 7. Rendering-Optimierung

### Adaptive Blur

Der Unschärferadius und die Berechnungsauflösung können abhängig von Hardwareleistung und Systemlast angepasst werden.

### Downsampled Blur

Große Unschärferadien können durch Berechnung auf reduzierter Auflösung effizient umgesetzt werden.

### Region Caching

Unveränderte Hintergrundbereiche werden wiederverwendet.

### Shared Blur

Mehrere Materialflächen können gemeinsame Zwischenergebnisse nutzen, sofern ihre Hintergrundgrundlage kompatibel ist.

### Damage-Aware Rendering

Nur tatsächlich betroffene Effektregionen werden neu berechnet.

## 8. Performance und Zuverlässigkeit

- GPU-beschleunigte Filterverarbeitung.
- Begrenzung maximaler Unschärferadien.
- Reduzierung unnötiger Zwischenbuffer.
- Wiederverwendung berechneter Effektregionen.
- Partielle Aktualisierung statt vollständiger Neuberechnung.
- Adaptive Qualität bei hoher GPU-Auslastung.
- Bevorzugung statischer Ersatzmaterialien bei Ressourcenknappheit.
- Automatische Wiederherstellung nach GPU- oder Compositor-Fehlern.
- Deaktivierbarkeit sämtlicher dekorativer Effekte.
- Messung von Effektkosten, Cache-Treffern und Rendering-Zeiten.

## 9. Ergebnis

NovaOS erhält eine native Material- und Effektarchitektur für Blur, Acrylic und Glass.

Fenster, Dialoge, Menüs und Nova UI Controls können moderne transparente Oberflächen mit räumlicher Tiefe darstellen.

Durch GPU-Beschleunigung, Caching und adaptive Qualitätsstufen bleibt die Oberfläche flüssig und ressourcenschonend.

Die Effekte sind vollständig optional und beeinträchtigen weder die grundlegende Bedienbarkeit noch die Stabilität des Desktops.