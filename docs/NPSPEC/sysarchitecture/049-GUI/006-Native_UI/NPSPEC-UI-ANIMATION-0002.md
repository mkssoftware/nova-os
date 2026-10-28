# NPSPEC-UI-ANIMATION-0002 – Nova Property Animations

**Status:** Angenommen  
**Bereich:** UI / Animation / Property Animations  
**System:** NovaOS

## 1. Zweck

Nova Property Animations definiert die zeitgesteuerte Veränderung von Eigenschaften nativer UI-Elemente.

Ziel ist die flüssige, typsichere und ressourcenschonende Animation einzelner Eigenschaften, ohne deren ursprüngliche Werte oder bestehende Datenbindungen dauerhaft zu verändern.

Property Animations verwenden die Nova Animation Engine und funktionieren unabhängig vom Renderbackend.

## 2. Architektur

Das Subsystem umfasst folgende Komponenten:

| Komponente | Aufgabe |
|---|---|
| Property Animation | Definition einer Eigenschaftsanimation |
| Property Animation Resolver | Auflösung der Zieleigenschaft |
| Property Interpolator | Berechnung von Zwischenwerten |
| Animated Value Provider | Bereitstellung des animierten Eigenschaftswerts |
| Animation Conflict Resolver | Auflösung konkurrierender Animationen |
| Property Animation Tracker | Ermittlung erforderlicher Aktualisierungen |

Die Zeitsteuerung und Ausführung erfolgen über die Nova Animation Engine.

## 3. Animationsmodell

Eine Property Animation besitzt:

- Zielelement.
- Zieleigenschaft.
- Optionalen Startwert.
- Zielwert.
- Dauer.
- Optionalen Startzeitversatz.
- Interpolationsverfahren.
- Wiederholungsverhalten.
- Abschlussverhalten.

Die Zieleigenschaft MUSS über das Nova UI Property System auflösbar sein.

Der Datentyp der Animation MUSS mit dem Eigenschaftstyp kompatibel sein.

## 4. Animierbare Eigenschaften

NovaOS unterstützt insbesondere folgende Eigenschaftstypen:

| Eigenschaftstyp | Beispiele |
|---|---|
| Numerisch | Opacity, Width, Height |
| Farbe | Foreground, Background |
| Geometrie | Position, Größe, Koordinaten |
| Transformation | Translation, Scale, Rotation |
| Vektor | Positionen und Richtungen |
| Effektparameter | BlurRadius, ShadowOpacity |

Nicht interpolierbare Eigenschaften können diskrete Zustandswechsel unterstützen.

Animierbare Eigenschaften MÜSSEN durch Metadaten gekennzeichnet werden können.

## 5. Deklarative Definition

Property Animations können im Nova Declarative UI Model definiert werden.

Konzeptionelles Beispiel:

```xml
<PropertyAnimation
    Target="Panel"
    Property="Opacity"
    From="0"
    To="1"
    Duration="300ms"
    Easing="EaseOut" />
```

Animationen können Bestandteil von Styles, Control Templates, Visual States und Storyboards sein.

Die konkrete Syntax wird durch das deklarative UI-Schema festgelegt.

## 6. Start- und Zielwerte

Property Animations unterstützen:

- Explizite Start- und Zielwerte.
- Animation vom aktuellen effektiven Wert.
- Relative Wertänderungen.
- Animation zu einem definierten Zielwert.

Wird kein Startwert angegeben, MUSS der Startwert zum definierten Aktivierungszeitpunkt ermittelt werden.

Relative Animationen MÜSSEN die Typ- und Wertebereichsregeln der Zieleigenschaft berücksichtigen.

## 7. Interpolation

Zwischenwerte werden anhand der Animationszeit berechnet.

Dabei gilt:

- Die Interpolation MUSS zum Eigenschaftstyp passen.
- Numerische Werte werden mit definierten Rundungsregeln verarbeitet.
- Farbinterpolation MUSS einen definierten Farbraum verwenden.
- Transformationen MÜSSEN geeignete mathematische Interpolationen verwenden.
- Ungültige Ergebnisse MÜSSEN kontrolliert behandelt werden.

Die Berechnung MUSS bei identischen Eingaben deterministisch sein.

## 8. Eigenschaftspriorität

Animationen beeinflussen den effektiven Wert einer Eigenschaft, ohne deren zugrunde liegenden Basiswert zu überschreiben.

Dabei gilt:

- Lokale Werte, Styles und Bindings bleiben erhalten.
- Animierte Werte werden über eine definierte Animationsschicht bereitgestellt.
- Die Priorität dieser Schicht MUSS im Nova UI Property System eindeutig festgelegt sein.
- Nach Abschluss oder Abbruch wird der effektive Wert neu aufgelöst.

Animationen DÜRFEN bestehende Datenbindungen nicht unbeabsichtigt entfernen.

## 9. Gleichzeitige Property Animations

Mehrere Animationen können dieselbe Eigenschaft beeinflussen.

Unterstützte Strategien:

- **Replace:** Neue Animation ersetzt die bisherige.
- **Compose:** Kompatible Animationen werden kombiniert.
- **Additive:** Animationswerte werden relativ zum Basiswert addiert.
- **Queue:** Animationen werden nacheinander ausgeführt.

Die Strategie MUSS eindeutig definiert sein.

Nicht kompatible Animationen MÜSSEN kontrolliert zurückgewiesen oder nach festgelegten Regeln aufgelöst werden.

## 10. Abschlussverhalten

Nach Abschluss einer Animation sind folgende Verhaltensweisen möglich:

| Verhalten | Bedeutung |
|---|---|
| Restore | Rückkehr zum aktuellen Basiswert |
| Hold | Letzten Animationswert vorübergehend beibehalten |
| Commit | Endwert ausdrücklich als neuen Basiswert übernehmen |

`Restore` ist das Standardverhalten.

`Hold` DARF den ursprünglichen Eigenschaftswert nicht verändern.

`Commit` MUSS die Schreibberechtigungen und Regeln des Nova UI Property System einhalten.

## 11. Unterbrechung und Neustart

Property Animations können unterbrochen oder neu gestartet werden.

Dabei gilt:

- Eine neue Animation kann den aktuellen visuellen Wert übernehmen.
- Unterbrechungen MÜSSEN kontrolliert erfolgen.
- Abgebrochene Animationen MÜSSEN ihre Ressourcen freigeben.
- Ein Neustart MUSS einen eindeutig definierten Ausgangszustand besitzen.
- Veraltete Animationswerte DÜRFEN neuere Zustände nicht überschreiben.

## 12. Layout- und Renderinvalidierung

Property Animations unterscheiden zwischen layoutabhängigen und rein visuellen Eigenschaften.

| Änderung | Aktualisierung |
|---|---|
| Opacity | Rendering beziehungsweise Compositing |
| Farbe | Rendering |
| Translation | Transformation beziehungsweise Compositing |
| Scale | Transformation beziehungsweise Compositing |
| Width / Height | Layout und Rendering |
| Margin / Padding | Layout und Rendering |
| BlurRadius | Effekt- und Renderinvalidierung |

Dabei gilt:

- Nur tatsächlich veränderte Eigenschaften werden aktualisiert.
- Layoutberechnungen erfolgen ausschließlich bei Bedarf.
- Renderinvalidierungen verwenden Damage Tracking.
- Unveränderte Werte erzeugen keine unnötigen Frames.

## 13. Compositor-Ausführung

Geeignete Property Animations SOLLEN direkt durch den Compositor ausgeführt werden können.

Dies betrifft insbesondere:

- Opacity.
- Translation.
- Scale.
- Rotation.
- Geeignete Effektparameter.

Dabei gilt:

- Die Animationsdefinition bleibt rendererunabhängig.
- Compositorseitige Animationen SOLLEN ohne erneute Layoutberechnung erfolgen.
- Nicht unterstützte Eigenschaften werden durch die reguläre Animation Engine verarbeitet.
- Der Wechsel des Ausführungswegs MUSS visuell konsistent bleiben.

## 14. Datenbindung und Observable Properties

Property Animations arbeiten mit Nova Data Binding und Nova Observable Properties zusammen.

Dabei gilt:

- Animationen verändern standardmäßig nur den effektiven Darstellungswert.
- Der Basiswert bleibt für Datenbindungen erhalten.
- Änderungen des Basiswerts während einer Animation MÜSSEN berücksichtigt werden.
- Animationswerte DÜRFEN keine unbeabsichtigten TwoWay-Binding-Rückschreibungen auslösen.
- Der Abschluss einer Animation MUSS die aktuelle Eigenschaftspriorität berücksichtigen.

## 15. Performance und Ressourcen

Nova Property Animations arbeitet bedarfsgesteuert.

Dabei gilt:

- Zwischenwerte werden nur während aktiver Animationen berechnet.
- Unveränderte Werte lösen keine Aktualisierung aus.
- Geeignete Interpolationsinformationen SOLLEN zwischengespeichert werden.
- Nicht sichtbare Animationen SOLLEN reduziert oder pausiert werden können.
- Nicht mehr benötigte Animationsressourcen werden freigegeben.

Eine permanente Neuberechnung inaktiver Eigenschaften ist nicht zulässig.

## 16. NovaStudio und Logic Graph

NovaStudio unterstützt:

- Auswahl animierbarer Eigenschaften.
- Definition von Start- und Zielwerten.
- Einstellung von Dauer und Interpolation.
- Vorschau von Property Animations.
- Konfiguration von Abschlussverhalten.
- Erkennung konkurrierender Animationen.
- Anzeige von Layout- und Renderauswirkungen.

Logic Graph kann Property Animations starten, stoppen und ihren Abschluss auswerten.

## 17. Sicherheit

Nova Property Animations unterliegt Nova.Trust.

Dabei gilt:

- Animationen DÜRFEN keine zusätzlichen Capabilities verleihen.
- Geschützte Eigenschaften DÜRFEN nicht unautorisiert verändert werden.
- Animationswerte MÜSSEN validiert werden.
- Benutzerdefinierte Interpolationen MÜSSEN innerhalb ihres autorisierten Kontexts ausgeführt werden.
- Sicherheitsrelevante UI-Zustände DÜRFEN durch Animationen nicht unzulässig manipuliert werden.

## 18. Normative Anforderungen

- **UI-001:** NovaOS MUSS Property Animations unterstützen.
- **UI-002:** Animationen MÜSSEN typsicher definiert werden können.
- **UI-003:** Property Animations MÜSSEN deklarativ und programmatisch verwendbar sein.
- **UI-004:** Start- und Zielwerte MÜSSEN unterstützt werden.
- **UI-005:** Animationen vom aktuellen effektiven Wert MÜSSEN möglich sein.
- **UI-006:** Interpolationen MÜSSEN deterministisch erfolgen.
- **UI-007:** Der ursprüngliche Basiswert MUSS standardmäßig erhalten bleiben.
- **UI-008:** Animationsprioritäten MÜSSEN eindeutig definiert sein.
- **UI-009:** Gleichzeitige Animationen MÜSSEN kontrolliert aufgelöst werden.
- **UI-010:** Animationen MÜSSEN unterbrechbar sein.
- **UI-011:** Abschlussverhalten MUSS konfigurierbar sein.
- **UI-012:** Datenbindungen DÜRFEN nicht unbeabsichtigt überschrieben werden.
- **UI-013:** Änderungen MÜSSEN gezielte Layout- und Renderinvalidierungen auslösen.
- **UI-014:** Geeignete Property Animations SOLLEN compositorseitig ausgeführt werden können.
- **UI-015:** Animationen MÜSSEN die Zeitsteuerung der Nova Animation Engine verwenden.
- **UI-016:** Reduzierte Animationen MÜSSEN berücksichtigt werden.
- **UI-017:** NovaStudio MUSS Property Animations visuell bearbeiten können.
- **UI-018:** Logic Graph MUSS Property Animations steuern können.
- **UI-019:** Property Animations MÜSSEN unabhängig vom Renderbackend definiert werden können.
- **UI-020:** Property Animations MÜSSEN die Sicherheitsregeln von Nova.Trust einhalten.
- **UI-021:** Nova Property Animations MUSS vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 19. Integration

- **Nova Animation Engine:** Zeitsteuerung und Animationsausführung.
- **Nova UI Property System:** Animierbare Eigenschaften und Wertprioritäten.
- **Nova Declarative UI Model:** Deklarative Animationsdefinitionen.
- **Nova Data Binding:** Erhaltung bestehender Bindungen.
- **Nova Observable Properties:** Eigenschaftsänderungen.
- **Nova Control Framework:** Animation nativer Controls.
- **Nova Control Templates:** Templatebasierte Animationen.
- **Nova Visual State Styling:** Zustandsabhängige Animationen.
- **Nova Layout Engine:** Layoutabhängige Eigenschaften.
- **Nova UI Renderer Abstraction:** Darstellung animierter Werte.
- **Nova Compositor:** Transformationsanimationen und Damage Tracking.
- **Nova UI Accessibility:** Reduzierte Animationen.
- **NovaLang:** Programmatische Animationssteuerung.
- **NovaStudio:** Property-Animation-Editor.
- **Nova Logic Graph:** Visuelle Animationssteuerung.
- **Nova.Trust:** Sicherheit und Isolation.

## 20. Ergebnis

NovaOS erhält ein natives und typsicheres System zur Animation einzelner UI-Eigenschaften.

Eigenschaften können flüssig verändert werden, ohne ihre ursprünglichen Werte oder Datenbindungen dauerhaft zu überschreiben.

Durch gezielte Invalidierungen und compositorseitige Ausführung geeigneter Animationen bleibt die Benutzeroberfläche reaktionsschnell und ressourcenschonend.

**Grundsatz:** Eine Property Animation verändert den sichtbaren Wert einer Eigenschaft über die Zeit, ohne ihren eigentlichen Zustand unnötig zu verändern.