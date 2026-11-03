# NPSPEC-DAMAGE-MATERIAL-0001 – Nova Material-Aware Damage Tracking

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Damage Tracking

## 1. Zweck

Nova Material-Aware Damage Tracking definiert die Erkennung und Verarbeitung grafischer Änderungen unter Berücksichtigung der Materialeigenschaften von NovaOS.

Ziel ist die präzise Invalidierung materialabhängiger Bildbereiche, insbesondere bei Acrylic, Liquid Glass, Frosted Glass, Crystal und Solid.

Unveränderte Materialergebnisse SOLLEN wiederverwendet werden, während tatsächlich betroffene Effekte gezielt aktualisiert werden.

## 2. Architekturprinzipien

- **Material-Aware:** Damage berücksichtigt die Eigenschaften des verwendeten Materials.
- **Dependency-Driven:** Änderungen werden anhand tatsächlicher Effektabhängigkeiten verarbeitet.
- **Backdrop-Aware:** Änderungen hinter transparenten Materialien werden erkannt.
- **Region-Based:** Aktualisierungen bleiben räumlich begrenzt.
- **Incremental:** Nur betroffene Materialkomponenten werden invalidiert.
- **Cache-Aware:** Gültige Material- und Effekt-Caches bleiben erhalten.
- **Adaptive:** Der Aktualisierungsaufwand wird anhand der Effektkomplexität optimiert.
- **Deterministic:** Identische Zustände erzeugen reproduzierbare Damage-Ergebnisse.

## 3. Material Damage Model

Jedes Material MUSS seine relevanten grafischen Abhängigkeiten beschreiben können.

Dazu gehören mindestens:

- **Material ID:** Eindeutige Materialidentifikation.
- **Material Version:** Aktueller Materialzustand.
- **Effect Dependencies:** Verwendete Effekte und deren Eingaben.
- **Sampling Regions:** Benötigte Eingabebereiche.
- **Output Regions:** Beeinflusste Ausgabebereiche.
- **Backdrop Dependencies:** Abhängigkeiten von Hintergrundinhalten.
- **Cache Dependencies:** Abhängigkeiten zwischengespeicherter Ergebnisse.

Die Material Engine stellt diese Informationen dem Damage Tracking bereit.

## 4. Material Damage Types

NovaOS unterscheidet folgende Änderungsarten:

| Damage Type | Beschreibung |
|---|---|
| Property Damage | Änderung von Materialeigenschaften |
| Geometry Damage | Änderung der Materialgeometrie |
| Backdrop Damage | Änderung benötigter Hintergrundinhalte |
| Effect Damage | Änderung eines Materialeffekts |
| Lighting Damage | Änderung relevanter Beleuchtungsinformationen |
| Animation Damage | Änderung animierter Materialparameter |
| Resource Damage | Änderung verwendeter Texturen oder Ressourcen |

Eine Änderung DARF mehrere Damage Types gleichzeitig auslösen.

## 5. Materialabhängige Verarbeitung

NovaOS MUSS die unterschiedlichen Abhängigkeiten seiner Materialfamilien berücksichtigen.

| Material | Typische Damage-Abhängigkeiten |
|---|---|
| Solid | Farbe, Geometrie, Texturen |
| Acrylic | Transparenz, Tint, Blur, Backdrop |
| Frosted Glass | Blur, Lichtstreuung, Backdrop |
| Crystal | Licht, Reflection, Refraction |
| Liquid Glass | Backdrop, Blur, Refraction, Reflection, Lighting |

Die tatsächlichen Abhängigkeiten MÜSSEN aus der jeweiligen Materialkonfiguration abgeleitet werden.

Eine Materialfamilie allein DARF keine unnötige vollständige Invalidierung erzwingen.

## 6. Backdrop Damage Tracking

Transparente Materialien können von darunterliegenden Bildinhalten abhängen.

NovaOS MUSS Änderungen innerhalb relevanter Backdrop-Sampling-Bereiche erkennen.

Dabei gilt:

1. Eine Hintergrundänderung erzeugt zunächst gewöhnlichen Damage.
2. Die Materialabhängigkeiten werden geprüft.
3. Betroffene Material-Ausgabebereiche werden bestimmt.
4. Nur notwendige Materialergebnisse werden invalidiert.
5. Gültige unabhängige Effektergebnisse bleiben erhalten.

Eine unveränderte Materialgeometrie bedeutet nicht automatisch, dass das Materialergebnis unverändert ist.

## 7. Effect Dependency Mapping

Materialeffekte MÜSSEN ihre räumlichen Eingabe- und Ausgabeabhängigkeiten beschreiben können.

Insbesondere:

- **Blur:** Berücksichtigung des Filterradius.
- **Refraction:** Berücksichtigung versetzter Sampling-Koordinaten.
- **Reflection:** Berücksichtigung reflektierter Eingabebereiche.
- **Shadow:** Berücksichtigung von Ausdehnung und Weichzeichnung.
- **Lighting:** Berücksichtigung relevanter Lichtquellen und Beleuchtungsbereiche.

Die Abbildung MUSS sowohl die Ermittlung benötigter Eingaben als auch die Bestimmung betroffener Ausgaben ermöglichen.

Bei unbekannten Abhängigkeiten MUSS eine konservative Invalidierung erfolgen.

## 8. Incremental Material Damage

NovaOS SOLL Materialeffekte unabhängig voneinander invalidieren können.

Beispielsweise DARF eine Änderung der Tint-Farbe nicht automatisch eine erneute Berechnung eines weiterhin gültigen Blur-Ergebnisses erzwingen.

Dazu MÜSSEN Effektabhängigkeiten und Cache-Gültigkeit getrennt bewertet werden.

Mehrstufige Effektketten MÜSSEN ihre indirekten Abhängigkeiten berücksichtigen.

## 9. Material Cache Integration

NovaOS SOLL gültige Materialergebnisse wiederverwenden.

Die Cache-Gültigkeit hängt insbesondere ab von:

- Materialversion
- Effektparametern
- Eingaberessourcen
- Backdrop-Versionen und betroffenen Regionen
- Transformationen
- Ausgabeparametern

Eine Änderung MUSS ausschließlich diejenigen Cache-Einträge invalidieren, deren Ergebnisse betroffen sein können.

Veraltete Materialergebnisse DÜRFEN nicht als gültig veröffentlicht werden.

## 10. Occlusion und Visibility

Material-Aware Damage Tracking MUSS mit Nova Occlusion-Aware Damage Tracking zusammenarbeiten.

Dabei gilt:

- Opake Materialien können darunterliegende Inhalte vollständig verdecken.
- Transparente Materialien können weiterhin Hintergrundinformationen benötigen.
- Refraction und Reflection können außerhalb der sichtbaren Materialgrenzen liegende Eingaben verwenden.
- Verdeckte Materialbereiche DÜRFEN nur bei gewährleisteter Korrektheit zurückgestellt werden.

Visuelle Verdeckung DARF tatsächliche Materialabhängigkeiten nicht aufheben.

## 11. Animation und dynamische Effekte

Animierte Materialien erzeugen Damage entsprechend ihrer tatsächlichen Zustandsänderungen.

NovaOS SOLL dabei:

- Unveränderte Effektparameter erkennen.
- Betroffene Regionen gezielt aktualisieren.
- Gültige Zwischenergebnisse wiederverwenden.
- Unnötige vollständige Materialberechnungen vermeiden.

Eine Animation MUSS nicht automatisch die gesamte Surface invalidieren.

Ohne relevante Zustandsänderung SOLL keine neue Materialberechnung ausgelöst werden.

## 12. Integration

Nova Material-Aware Damage Tracking arbeitet mit:

- **Nova Material Engine:** Materialzustände und Effektabhängigkeiten.
- **Nova Damage Dependency Analysis:** Analyse betroffener Eingabe- und Ausgabebereiche.
- **Nova Damage Propagation:** Weitergabe der Materialänderungen.
- **Nova Render Graph:** Planung erforderlicher Effektberechnungen.
- **Nova Render Engine:** Inkrementelle Materialdarstellung.
- **Nova Compositor:** Verarbeitung transparenter und effektabhängiger Layers.
- **Nova Frame Scheduler:** Bedarfsgesteuerte Aktualisierung.

Die Material Engine definiert die Effekte. Damage Tracking bestimmt deren Aktualisierungsbedarf.

## 13. Fehlerbehandlung

NovaOS MUSS unvollständige oder ungültige Materialabhängigkeiten erkennen können.

Bei nicht zuverlässig bestimmbaren Auswirkungen MUSS eine konservative Invalidierung erfolgen.

Mögliche Fallback-Stufen:

1. Betroffener Effektbereich
2. Vollständiges Materialelement
3. Vollständige Surface
4. Vollständige Ausgabe

Die kleinste nachweislich sichere Invalidierungsstufe SOLL bevorzugt werden.

## 14. Verbindliche Anforderungen

1. NovaOS MUSS materialabhängiges Damage Tracking unterstützen.
2. Materialeigenschaften und Effektabhängigkeiten MÜSSEN berücksichtigt werden.
3. Acrylic, Liquid Glass, Frosted Glass, Crystal und Solid MÜSSEN unterstützt werden.
4. Backdrop-Änderungen MÜSSEN bei abhängigen Materialien erkannt werden.
5. Effektabhängigkeiten MÜSSEN räumlich abbildbar sein.
6. Blur-, Refraction- und Reflection-Abhängigkeiten MÜSSEN berücksichtigt werden.
7. Unabhängige Materialeffekte SOLLEN getrennt invalidiert werden.
8. Gültige Material-Caches SOLLEN wiederverwendet werden.
9. Veraltete Cache-Ergebnisse DÜRFEN nicht verwendet werden.
10. Materialanimationen SOLLEN ausschließlich betroffene Regionen aktualisieren.
11. Occlusion DARF erforderliche Materialaktualisierungen nicht unterdrücken.
12. Materialänderungen MÜSSEN versionssicher verarbeitet werden.
13. Unbekannte Effektabhängigkeiten MÜSSEN konservativ behandelt werden.
14. Ein sicherer Full-Material- oder Full-Surface-Fallback MUSS verfügbar sein.
15. Identische Eingaben MÜSSEN im deterministischen Modus reproduzierbare Damage-Ergebnisse erzeugen.

## 15. Ergebnis

Nova Material-Aware Damage Tracking ermöglicht die präzise Aktualisierung komplexer Materialeffekte innerhalb der NovaOS-Grafikarchitektur.

Durch die getrennte Analyse von Materialeigenschaften, Effektketten und Backdrop-Abhängigkeiten werden unnötige Neuberechnungen vermieden.

Damit können insbesondere Liquid-Glass-Oberflächen effizient dargestellt werden, ohne bei jeder Hintergrundänderung sämtliche Materialeffekte oder vollständige Fenster neu berechnen zu müssen.