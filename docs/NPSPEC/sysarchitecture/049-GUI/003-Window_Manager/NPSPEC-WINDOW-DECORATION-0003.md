# NPSPEC-WINDOW-DECORATION-0003 – Nova Window Borders

**Status:** Angenommen  
**Bereich:** Window Manager / Fensterrahmen  
**System:** NovaOS

## 1. Zweck

Nova Window Borders definiert Darstellung, Verhalten und Interaktionsbereiche der Fensterrahmen innerhalb von NovaOS.

Ziel ist eine einheitliche, anpassbare und ressourcenschonende Fensterbegrenzung, die Fenster visuell voneinander trennt und eine intuitive Größenänderung ermöglicht.

## 2. Architektur

- Verwaltung der Fensterrahmen über Nova Window Decorations.
- Darstellung durch Nova UI und den Compositor.
- Fenstergeometrie und Größenänderungen werden durch den Window Manager verwaltet.
- Trennung zwischen sichtbarem Rahmen und interaktivem Resize-Bereich.
- DPI-unabhängige Rahmengeometrie.
- Unterstützung systemseitiger und anwendungsseitiger Dekorationen.
- Integration in Window Resizing, Snapping und Tiling.
- Deklarative Gestaltung über Styles und Templates.

## 3. Rahmenmodell

NovaOS unterscheidet folgende Rahmentypen:

| Typ | Beschreibung |
|---|---|
| Standard Border | Klassischer sichtbarer Fensterrahmen |
| Thin Border | Dezente, schmale Fensterbegrenzung |
| Invisible Border | Unsichtbarer Rahmen mit aktivem Resize-Bereich |
| Custom Border | Benutzerdefinierte Rahmengestaltung |
| Borderless | Fenster ohne sichtbaren Rahmen |

Der sichtbare Rahmen und der interaktive Resize-Bereich werden unabhängig voneinander definiert.

Ein unsichtbarer Rahmen DARF weiterhin Größenänderungen ermöglichen.

## 4. Rahmengeometrie

Jeder Fensterrahmen kann folgende Eigenschaften besitzen:

| Eigenschaft | Beschreibung |
|---|---|
| Border Thickness | Sichtbare Rahmenstärke |
| Resize Thickness | Größe des interaktiven Resize-Bereichs |
| Corner Radius | Abrundung der Fensterecken |
| Border Color | Rahmenfarbe |
| Border Opacity | Transparenz |
| Border Style | Darstellungsstil |
| Active State | Darstellung bei aktivem Fenster |
| Resize Policy | Zulässige Größenänderungen |

Die Rahmengeometrie MUSS unabhängig von der physischen Displayauflösung berechnet werden.

## 5. Normative Anforderungen

- WINDOW-001: NovaOS MUSS ein einheitliches System zur Darstellung von Fensterrahmen bereitstellen.
- WINDOW-002: Sichtbare Rahmen und interaktive Resize-Bereiche MÜSSEN getrennt definiert werden können.
- WINDOW-003: Rahmen MÜSSEN DPI-unabhängig skalierbar sein.
- WINDOW-004: Größenänderungen MÜSSEN durch den Window Manager verarbeitet werden.
- WINDOW-005: Resize-Bereiche MÜSSEN die Größenbeschränkungen des jeweiligen Fensters berücksichtigen.
- WINDOW-006: Nicht skalierbare Fenster DÜRFEN keine aktiven Resize-Bereiche besitzen.
- WINDOW-007: Fensterrahmen MÜSSEN unterschiedliche Fensterzustände unterstützen.
- WINDOW-008: Anwendungen DÜRFEN eigene Rahmen gestalten, sofern die Systemrichtlinien dies erlauben.
- WINDOW-009: Benutzerdefinierte Rahmen DÜRFEN keine unberechtigten Fensteroperationen auslösen.
- WINDOW-010: Resize-Bereiche MÜSSEN eindeutig gegenüber interaktiven Fensterinhalten abgegrenzt sein.
- WINDOW-011: Rahmen MÜSSEN unterschiedliche Display-Skalierungsfaktoren berücksichtigen.
- WINDOW-012: Fensterrahmen MÜSSEN auch ohne Transparenz-, Schatten- oder Blur-Effekte funktionieren.
- WINDOW-013: Änderungen von Theme, DPI oder Fensterzustand MÜSSEN eine korrekte Aktualisierung der Rahmen ermöglichen.
- WINDOW-014: Die Rahmenverwaltung MUSS deterministisch und ohne KI-Abhängigkeit funktionieren.

## 6. Größenänderung

NovaOS unterstützt acht Resize-Richtungen:

- Links
- Rechts
- Oben
- Unten
- Oben links
- Oben rechts
- Unten links
- Unten rechts

Beim Beginn einer Größenänderung bestimmt der Window Manager den betroffenen Resize-Bereich.

Die anschließende Größenänderung erfolgt gemäß `NPSPEC-WINDOW-MANAGEMENT-0003 – Nova Window Resizing`.

Mindestgrößen, Maximalgrößen und Seitenverhältnisse MÜSSEN berücksichtigt werden.

## 7. Zustandsabhängige Rahmen

### Normal

- Sichtbarer Rahmen entsprechend dem aktiven Theme.
- Resize-Bereiche entsprechend den Fenstereigenschaften.
- Optionale abgerundete Ecken.

### Maximized

- Keine unnötigen äußeren Rahmenabstände.
- Resize-Bereiche standardmäßig deaktiviert.
- Rahmengeometrie an den Arbeitsbereich angepasst.

### Fullscreen

- Reguläre Fensterrahmen ausgeblendet.
- Keine aktiven Standard-Resize-Bereiche.

### Minimized

- Keine sichtbare Fensterrahmendarstellung.
- Logische Rahmeneigenschaften bleiben erhalten.

### Tiled

- Rahmen werden an das Tiling-Layout angepasst.
- Gemeinsame Fenstergrenzen DÜRFEN spezielle Resize-Bereiche besitzen.
- Größenänderungen MÜSSEN die Tiling-Regeln berücksichtigen.

## 8. Visuelle Gestaltung

NovaOS unterstützt:

- Dezente Rahmenfarben.
- Anpassbare Rahmenstärken.
- Abgerundete Fensterecken.
- Unterschiedliche Darstellungen aktiver und inaktiver Fenster.
- Optionale Transparenz.
- Themeabhängige Rahmenfarben.
- Barrierefreie Kontrastmodi.
- Reduzierte visuelle Hervorhebung inaktiver Fenster.

Die Gestaltung SOLL die Fenstergrenzen erkennbar machen, ohne unnötige visuelle Unruhe zu erzeugen.

## 9. Eingabe und Hit-Testing

Der Window Manager MUSS zwischen folgenden Bereichen unterscheiden:

- Fensterinhalt.
- Titelleiste.
- Fensterrahmen.
- Resize-Kanten.
- Resize-Ecken.
- Nicht interaktive Dekorationsbereiche.

Die Ermittlung des betroffenen Bereichs erfolgt über definierte Hit-Testing-Regeln.

Bei überlappenden Eingabebereichen MUSS eine eindeutige Priorität gelten.

Touch-Eingaben DÜRFEN größere interaktive Resize-Bereiche verwenden als Mauseingaben.

Die visuelle Rahmenstärke bleibt davon unabhängig.

## 10. Fehlerbehandlung und Performance

- Ungültige Rahmenkonfigurationen werden durch sichere Standardwerte ersetzt.
- Nicht verfügbare visuelle Effekte werden deaktiviert.
- Fehlerhafte Custom Borders dürfen die grundlegende Fensterbedienung nicht beeinträchtigen.
- Rahmenänderungen SOLLEN ausschließlich betroffene Darstellungsbereiche aktualisieren.
- Wiederverwendbare Rahmenressourcen SOLLEN gemeinsam genutzt werden.
- Größenänderungen dürfen nicht durch aufwendige Rahmeneffekte blockiert werden.
- Ein Compositor-Neustart verändert die logische Rahmen- und Fenstergeometrie nicht.

## 11. Ergebnis

NovaOS erhält ein flexibles und einheitliches System für Fensterrahmen.

Sichtbare Rahmen und interaktive Größenänderungsbereiche werden unabhängig voneinander verwaltet.

Dadurch können Fenster klassisch, minimalistisch oder vollständig rahmenlos dargestellt werden, ohne die grundlegende Bedienbarkeit einzuschränken.

**Grundsatz:** Fensterrahmen sollen Orientierung und Bedienbarkeit bieten, ohne unnötig Platz oder Aufmerksamkeit zu beanspruchen.