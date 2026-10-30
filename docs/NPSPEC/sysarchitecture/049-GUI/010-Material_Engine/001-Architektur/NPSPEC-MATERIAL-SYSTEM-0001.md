# NPSPEC-MATERIAL-SYSTEM-0001 – Nova Material System

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Material Engine

## 1. Zweck

Das Nova Material System definiert, verwaltet und stellt visuelle Oberflächenmaterialien innerhalb von NovaOS bereit.

Es ermöglicht die einheitliche Verwendung, Konfiguration und Wiederverwendung von Materialien unabhängig von UI-Controls, Fenstern und Rendering-Implementierungen.

## 2. Architektur

Das Material System ist Bestandteil der Nova Material Engine.

Es besteht aus folgenden Komponenten:

- **Material Registry:** Registrierung und Bereitstellung verfügbarer Materialien.
- **Material Definition:** Deklarative Beschreibung eines Materials.
- **Material Instance:** Konkrete Verwendung eines Materials mit individuellen Eigenschaften.
- **Material Properties:** Einheitliches Eigenschaftsmodell.
- **Material Resources:** Verwaltung gemeinsam genutzter Ressourcen.
- **Material State:** Verwaltung dynamischer Materialzustände.

Materialdefinitionen und Materialinstanzen sind voneinander getrennt.

## 3. Standardmaterialien

NovaOS stellt folgende integrierte Materialtypen bereit:

| Material | Eigenschaften |
|----------|---------------|
| Acrylic | Transparenz, Blur und dezente Lichtreflexionen |
| Liquid Glass | Lichtbrechung, Reflexionen und dynamische Verformung |
| Frosted Glass | Milchglas mit ausgeprägter Hintergrundunschärfe |
| Crystal | Klares Glas mit intensiver Lichtbrechung |
| Solid | Undurchsichtige Oberfläche ohne aufwendige Effekte |

Alle Standardmaterialien verwenden dasselbe Materialmodell.

Weitere Materialien können registriert werden.

## 4. Materialdefinition

Eine Materialdefinition beschreibt die Eigenschaften und das Verhalten eines Materials.

Sie enthält:

- Eindeutige Material-ID
- Materialtyp
- Standardwerte der Materialeigenschaften
- Verwendete Effekte
- Unterstützte Zustände
- Optionale Animationsparameter
- Qualitätsanforderungen

Materialdefinitionen sind unveränderlich und können von mehreren Materialinstanzen gemeinsam verwendet werden.

## 5. Materialinstanzen

Eine Materialinstanz repräsentiert die konkrete Verwendung einer Materialdefinition.

Sie kann individuelle Eigenschaftswerte besitzen, ohne die zugrunde liegende Definition zu verändern.

Anforderungen:

- Mehrere Instanzen dürfen dieselbe Materialdefinition verwenden.
- Eigenschaften können zur Laufzeit überschrieben werden.
- Änderungen einer Instanz dürfen andere Instanzen nicht beeinflussen.
- Nicht überschriebene Eigenschaften verwenden die Werte der Materialdefinition.
- Materialinstanzen müssen ohne Neuerstellung des zugehörigen UI-Elements austauschbar sein.

## 6. Materialeigenschaften

Das Material System unterstützt ein einheitliches Eigenschaftsmodell.

Grundlegende Eigenschaften:

- **Color:** Grundfarbe des Materials.
- **Opacity:** Deckkraft.
- **Tint:** Farbüberlagerung.
- **Blur:** Stärke der Hintergrundunschärfe.
- **Refraction:** Intensität der Lichtbrechung.
- **Reflection:** Stärke der Reflexionen.
- **Highlight:** Intensität der Glanzlichter.
- **Distortion:** Stärke der Oberflächenverzerrung.
- **Roughness:** Oberflächenrauheit.

Eigenschaften besitzen definierte Datentypen, Wertebereiche und Standardwerte.

Nicht unterstützte Eigenschaften dürfen die Materialverarbeitung nicht beeinträchtigen.

## 7. Materialzustände

Materialien können unterschiedliche visuelle Zustände besitzen.

Unterstützte Zustände umfassen:

- Normal
- Hover
- Pressed
- Focused
- Disabled
- Active
- Inactive

Zustände können Materialeigenschaften überschreiben.

Zustandswechsel können über das bestehende NovaOS-Animationssystem interpoliert werden.

Das Material System verwaltet die Zustandswerte, während das UI-System die Interaktionszustände bereitstellt.

## 8. Material Registry

Die Material Registry verwaltet alle verfügbaren Materialdefinitionen.

Anforderungen:

- Materialien werden über eindeutige IDs identifiziert.
- Standardmaterialien sind systemweit verfügbar.
- Zusätzliche Materialien können registriert werden.
- Materialdefinitionen können von mehreren Komponenten referenziert werden.
- Ungültige oder nicht verfügbare Materialien müssen durch ein definiertes Fallback ersetzt werden.

Die Registry ist unabhängig von konkreten Grafiktreibern.

## 9. Ressourcenverwaltung

Materialdefinitionen können gemeinsam genutzte Ressourcen referenzieren.

Dazu gehören:

- Texturen
- Effektparameter
- Shader
- Oberflächenstrukturen
- Vorberechnete Effektinformationen

Ressourcen werden zentral verwaltet und nach Möglichkeit wiederverwendet.

Nicht mehr benötigte Ressourcen müssen freigegeben werden können.

## 10. Integration

Das Material System stellt Materialdefinitionen und Materialinstanzen für folgende Subsysteme bereit:

- Nova UI
- Nova Window Manager
- Nova Theme System
- Nova Material Renderer
- Nova Material Manager

Die Darstellung erfolgt ausschließlich über die vorhandene Grafik- und Compositor-Infrastruktur.

Das Material System selbst führt keine Rendering-Operationen aus.

## 11. Performance und Sicherheit

Verbindliche Anforderungen:

- Materialdefinitionen werden nach Möglichkeit gemeinsam genutzt.
- Instanzänderungen dürfen keine unnötigen Ressourcen-Neuerstellungen verursachen.
- Materialänderungen müssen gezielt an die Rendering-Pipeline gemeldet werden.
- Ungültige Eigenschaftswerte müssen abgefangen werden.
- Ressourcenverbrauch muss begrenzbar sein.
- Nicht vertrauenswürdige Materialdefinitionen dürfen keinen direkten Zugriff auf GPU, Speicher oder Systemressourcen erhalten.

## 12. Erweiterbarkeit

Das Material System muss benutzerdefinierte Materialien unterstützen.

Neue Materialien können bestehende Eigenschaften und Effekte kombinieren oder zusätzliche Eigenschaften definieren.

Erweiterungen müssen mit dem gemeinsamen Materialmodell kompatibel bleiben.

## 13. Ergebnis

Das Nova Material System stellt ein einheitliches, deklaratives und erweiterbares Materialmodell für NovaOS bereit.

Materialdefinitionen können systemweit wiederverwendet, individuell instanziiert und dynamisch angepasst werden.

Die Trennung zwischen Materialbeschreibung, Zustandsverwaltung und Rendering ermöglicht eine flexible und ressourcenschonende Darstellung sämtlicher NovaOS-Oberflächen.