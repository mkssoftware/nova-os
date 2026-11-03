# NPSPEC-DAMAGE-ARCH-0001 – Nova Damage Tracking Architecture

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Damage Tracking

## 1. Zweck

Nova Damage Tracking definiert die zentrale Architektur zur Erkennung, Verwaltung und Weitergabe grafischer Änderungen innerhalb von NovaOS.

Ziel ist, ausschließlich tatsächlich betroffene Bildbereiche neu zu berechnen, zu rendern und darzustellen.

Unveränderte Bildbereiche sollen ohne erneute Verarbeitung erhalten bleiben.

## 2. Architekturprinzipien

- **Change-Driven:** Grafische Verarbeitung wird durch tatsächliche Änderungen ausgelöst.
- **Region-Based:** Änderungen werden als räumlich begrenzte Bereiche erfasst.
- **Hierarchical:** Damage wird innerhalb der Scene-Graph-Hierarchie verwaltet.
- **Dependency-Aware:** Auswirkungen auf abhängige Inhalte werden berücksichtigt.
- **Material-Aware:** Blur, Refraction, Reflection und Liquid Glass werden unterstützt.
- **Occlusion-Aware:** Vollständig verdeckte Änderungen können zurückgestellt werden.
- **Incremental:** Unveränderte Inhalte und gültige Zwischenergebnisse bleiben erhalten.
- **Event-Driven:** Ohne relevante Änderungen ist keine neue Bildberechnung erforderlich.

## 3. Architekturmodell

Nova Damage Tracking arbeitet zwischen der grafischen Zustandsverwaltung und der tatsächlichen Rendering-Ausführung.

Die Verarbeitung erfolgt grundsätzlich über:

1. **Change Detection:** Erkennung relevanter Zustandsänderungen.
2. **Damage Generation:** Bestimmung betroffener Bildbereiche.
3. **Damage Propagation:** Weitergabe an abhängige Inhalte.
4. **Damage Optimization:** Zusammenfassung und Begrenzung der Bereiche.
5. **Damage Distribution:** Übergabe an Render Engine, Compositor und Frame Scheduler.
6. **Damage Resolution:** Bestätigung erfolgreich aktualisierter Bereiche.

Damage Tracking beschreibt, **welche Bereiche aktualisiert werden müssen**, nicht wie diese gerendert werden.

## 4. Damage Model

Ein Damage-Eintrag enthält mindestens:

- **Source ID:** Verursachender Scene Node, Surface oder Effekt.
- **Damage Region:** Betroffener geometrischer Bereich.
- **Coordinate Space:** Zugehöriges Koordinatensystem.
- **Damage Type:** Art der Änderung.
- **State Version:** Zugehörige Zustandsversion.
- **Dependencies:** Betroffene abhängige Inhalte.
- **Target ID:** Betroffene Ausgabe oder Rendering-Ressource.

Damage Regions DÜRFEN aus mehreren Rechtecken oder komplexeren Regionen bestehen.

Eine konservative rechteckige Begrenzung MUSS möglich sein.

## 5. Damage Sources

NovaOS MUSS insbesondere folgende Änderungsquellen berücksichtigen:

- Scene-Node- und Property-Änderungen
- Positions-, Größen- und Transformationsänderungen
- Änderungen der Sichtbarkeit und Ebenenreihenfolge
- Aktualisierte Fenster- und Surface-Inhalte
- Animationen und Materialeffekte
- Änderungen von Hintergrundinhalten
- Cursor- und Overlay-Änderungen
- Änderungen an Ausgabeparametern

Nicht jede Zustandsänderung MUSS eine sichtbare Bildänderung verursachen.

## 6. Damage Region Management

NovaOS MUSS Damage Regions erfassen, kombinieren und begrenzen können.

Dabei gilt:

- Alte und neue Objektbereiche MÜSSEN bei geometrischen Änderungen berücksichtigt werden.
- Transformationen MÜSSEN korrekt angewendet werden.
- Clipping und Ausgabebegrenzungen MÜSSEN berücksichtigt werden.
- Benachbarte oder überlappende Bereiche DÜRFEN zusammengefasst werden.
- Bei zu hoher Fragmentierung DARF auf größere Regionen oder Full Damage gewechselt werden.

Die Optimierung DARF erforderliche Aktualisierungsbereiche nicht ausschließen.

## 7. Hierarchical Damage Tracking

Damage MUSS innerhalb des Scene Graph hierarchisch verarbeitet werden können.

Änderungen an übergeordneten Nodes können untergeordnete Inhalte beeinflussen.

Dabei werden insbesondere berücksichtigt:

- Transformationen
- Clipping
- Sichtbarkeit
- Opacity
- Layout
- Effektabhängigkeiten

Unabhängige Teilbäume SOLLEN von unnötiger Invalidierung ausgenommen bleiben.

## 8. Dependency Propagation

Damage MUSS entlang tatsächlicher grafischer Abhängigkeiten weitergegeben werden.

Dazu gehören:

- Scene Dependencies
- Render Pass Dependencies
- Surface Dependencies
- Material Dependencies
- Backdrop Dependencies
- Composition Dependencies

Die Weitergabe MUSS so lange erfolgen, bis sämtliche betroffenen Ausgaben erfasst sind.

Zyklische oder mehrfache Abhängigkeiten DÜRFEN keine unkontrollierte Wiederholung verursachen.

## 9. Material-Aware Damage

Materialeffekte können Bildbereiche beeinflussen, die außerhalb der ursprünglichen Änderung liegen.

NovaOS MUSS deshalb erweiterte Abhängigkeitsbereiche berücksichtigen.

Beispiele:

- Blur benötigt zusätzliche benachbarte Bildinformationen.
- Refraction kann versetzte Hintergrundbereiche abtasten.
- Reflection kann entfernte Bildinformationen verwenden.
- Liquid Glass kann auf Änderungen hinter transparenten Oberflächen reagieren.

Damage MUSS anhand der tatsächlichen Effektparameter erweitert werden.

Eine Änderung hinter einem Liquid-Glass-Fenster DARF nicht allein aufgrund unveränderter Fenstergeometrie ignoriert werden.

## 10. Occlusion-Aware Damage

NovaOS SOLL vollständig verdeckte Änderungen erkennen.

Nicht sichtbare Damage Regions DÜRFEN von der aktuellen Bildberechnung ausgeschlossen werden.

Dabei MUSS sichergestellt sein, dass:

- Verdeckte Inhalte bei erneuter Sichtbarkeit korrekt dargestellt werden.
- Transparente Oberflächen berücksichtigt werden.
- Materialeffekte verdeckte Inhalte weiterhin benötigen können.
- Erforderliche Cache-Invalidierungen erhalten bleiben.

Occlusion DARF keine notwendigen Aktualisierungen unterdrücken.

## 11. Incremental Rendering Integration

Nova Damage Tracking stellt der Render Engine die erforderlichen Aktualisierungsbereiche bereit.

Die Render Engine entscheidet anhand dieser Informationen über:

- Partial Rendering
- Region-Based Rendering
- Scissor Rendering
- Cache Reuse
- Full Rendering

Damage Tracking selbst führt keine Rendering-Operationen aus.

## 12. Compositor Integration

Der Nova Compositor verwendet Damage-Informationen zur inkrementellen Zusammensetzung von Surfaces und Layers.

Dabei MUSS zwischen folgenden Änderungen unterschieden werden können:

- Inhalt einer Surface
- Position oder Größe einer Surface
- Ebenenreihenfolge
- Transparenz und Materialeffekte
- Sichtbarkeit und Verdeckung

Unveränderte Composition Regions SOLLEN erhalten bleiben.

## 13. Frame Scheduling Integration

Nova Frame Scheduling erhält Informationen über ausstehende sichtbare Änderungen.

Dabei gilt:

- Relevanter Damage kann eine neue Frame-Erzeugung auslösen.
- Mehrere Änderungen DÜRFEN zu einem Frame zusammengefasst werden.
- Ohne ausstehenden Damage und ohne sonstigen Präsentationsbedarf SOLL kein neuer Frame erzeugt werden.
- Kontinuierliche Animationen erzeugen Damage entsprechend ihrem tatsächlichen Zustandsfortschritt.

Ein periodisches vollständiges Neuzeichnen ist nicht erforderlich.

## 14. Damage Lifecycle

Ein Damage-Eintrag durchläuft folgende Zustände:

1. **Generated:** Änderung wurde erkannt.
2. **Propagated:** Abhängige Bereiche wurden bestimmt.
3. **Optimized:** Regionen wurden zusammengefasst und begrenzt.
4. **Scheduled:** Aktualisierung wurde eingeplant.
5. **Processed:** Betroffene Inhalte wurden neu berechnet.
6. **Resolved:** Aktualisierung wurde erfolgreich übernommen.

Damage DARF erst als erledigt gelten, wenn die zugehörige Aktualisierung erfolgreich abgeschlossen wurde.

Bei fehlgeschlagener Verarbeitung MUSS erforderlicher Damage erhalten bleiben.

## 15. Performance und Fallback

NovaOS SOLL den Verwaltungsaufwand des Damage Tracking begrenzen.

Dazu gehören:

- Zusammenfassung kleiner Regionen
- Wiederverwendung unveränderter Damage-Strukturen
- Begrenzung der Regionsfragmentierung
- Vermeidung redundanter Abhängigkeitsanalysen
- Adaptive Entscheidung zwischen Partial und Full Damage

Wenn eine präzise Damage-Bestimmung nicht zuverlässig möglich ist, MUSS eine konservative Invalidierung erfolgen.

Korrektheit besitzt Vorrang vor eingesparter Rendering-Arbeit.

## 16. Verbindliche Anforderungen

1. NovaOS MUSS grafische Änderungen räumlich erfassen können.
2. Damage Tracking MUSS unabhängig von der eigentlichen Rendering-Ausführung arbeiten.
3. Alte und neue Objektbereiche MÜSSEN bei geometrischen Änderungen berücksichtigt werden.
4. Damage MUSS hierarchisch verarbeitet werden können.
5. Grafische Abhängigkeiten MÜSSEN bei der Damage-Weitergabe berücksichtigt werden.
6. Material- und Backdrop-Abhängigkeiten MÜSSEN unterstützt werden.
7. Vollständig verdeckte Änderungen DÜRFEN zurückgestellt werden, sofern ihre spätere Korrektheit gewährleistet bleibt.
8. Damage Regions SOLLEN effizient zusammengefasst werden.
9. Render Engine und Compositor MÜSSEN Damage-Informationen verwenden können.
10. Frame Scheduling MUSS ausstehende Aktualisierungen erkennen können.
11. Unveränderte Bildbereiche SOLLEN nicht erneut verarbeitet werden.
12. Damage MUSS zustands- und versionsbezogen verwaltet werden.
13. Fehlgeschlagene Aktualisierungen DÜRFEN erforderlichen Damage nicht verlieren.
14. Bei unvollständiger Änderungsinformation MUSS ein konservativer Fallback verfügbar sein.
15. Identische Zustandsänderungen MÜSSEN im deterministischen Modus reproduzierbare Damage-Ergebnisse liefern.

## 17. Ergebnis

Nova Damage Tracking bildet die zentrale Grundlage für eine zustandsabhängige und inkrementelle Grafikverarbeitung in NovaOS.

Durch präzise Änderungsverfolgung, hierarchische Abhängigkeitsanalyse und materialbewusste Damage-Weitergabe werden ausschließlich erforderliche Bildbereiche aktualisiert.

Damit ermöglicht NovaOS eine ruhige, ressourcenschonende Grafikarchitektur, die ohne permanente vollständige Neuberechnung des Bildschirms auskommt.