# NPSPEC-COMPOSITOR-FALLBACK-0001 – Nova Composition Fallback

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Compositor

## 1. Zweck

Nova Composition Fallback definiert alternative Kompositionsverfahren bei fehlender Hardwareunterstützung, Ressourcenmangel oder Fehlern innerhalb der Grafikpipeline.

Ziel ist die Aufrechterhaltung einer funktionsfähigen und korrekten grafischen Oberfläche, auch wenn bevorzugte GPU- oder Kompositionsverfahren nicht verfügbar sind.

Der Wechsel zwischen den Verfahren SOLL möglichst ohne sichtbare Unterbrechung erfolgen.

## 2. Architekturprinzipien

- **Graceful Degradation:** Grafikfunktionen werden kontrolliert vereinfacht.
- **Capability-Based:** Verfügbare Hardware- und Softwarefähigkeiten bestimmen das Verfahren.
- **Correctness-First:** Grafische Korrektheit besitzt Vorrang vor Optimierung.
- **Adaptive:** Kompositionsverfahren können zur Laufzeit gewechselt werden.
- **Damage-Aware:** Inkrementelle Aktualisierungen bleiben nach Möglichkeit erhalten.
- **Recovery-Oriented:** Nach Fehlern kann ein funktionsfähiger Zustand wiederhergestellt werden.
- **Deterministic:** Fallback-Entscheidungen sind anhand definierter Bedingungen reproduzierbar.

## 3. Fallback Levels

NovaOS MUSS abgestufte Kompositionsverfahren unterstützen.

| Stufe | Verfahren | Beschreibung |
|---|---|---|
| 0 | Optimized GPU | Vollständige GPU-Komposition mit allen unterstützten Optimierungen |
| 1 | Standard GPU | GPU-Komposition ohne optionale Optimierungen |
| 2 | Reduced GPU | GPU-Komposition mit vereinfachten Materialeffekten |
| 3 | Software Composition | CPU-basierte Komposition |
| 4 | Minimal Composition | Reduzierte grafische Darstellung grundlegender Systemfunktionen |

Die Stufen beschreiben funktionale Rückfallmöglichkeiten und müssen nicht auf jeder Hardware einzeln implementiert sein.

Ein Wechsel MUSS zu einem tatsächlich verfügbaren und funktionsfähigen Verfahren erfolgen.

## 4. Fallback Triggers

Ein Fallback DARF insbesondere ausgelöst werden durch:

- Fehlende GPU-Funktionen
- GPU-Reset oder Geräteverlust
- Fehlgeschlagene Rendering-Operationen
- Nicht unterstützte Surface-Formate
- Fehlgeschlagene Hardware-Overlay-Zuweisungen
- GPU-Speichermangel
- Nicht verfügbare Shader oder Materialeffekte
- Wiederholte Synchronisationsfehler

Vorübergehende Performance-Schwankungen SOLLEN nicht unmittelbar einen vollständigen Wechsel des Kompositionsverfahrens auslösen.

## 5. Capability Detection

Der Compositor MUSS die verfügbaren Grafikfähigkeiten über definierte Schnittstellen ermitteln können.

Dazu gehören:

- Unterstützte Rendering-Backends
- GPU- und CPU-Kompositionsfähigkeiten
- Surface- und Texturformate
- Blending und Transformationen
- Material- und Shader-Unterstützung
- Hardware Overlays
- Synchronisationsmechanismen

Nicht verfügbare Funktionen MÜSSEN durch geeignete Alternativen ersetzt oder kontrolliert deaktiviert werden.

## 6. Software Composition

NovaOS MUSS einen GPU-unabhängigen Kompositionspfad bereitstellen können.

Dieser MUSS mindestens unterstützen:

- Surface Composition
- Layer-Reihenfolge
- Positionierung und Clipping
- Grundlegende Transformationen
- Alpha Blending
- Damage-Driven Updates

Komplexe Materialeffekte DÜRFEN vereinfacht werden.

Der Software-Compositor SOLL dieselben logischen Composition States wie der GPU-Compositor verwenden.

## 7. Material Fallback

Die Nova Material Engine MUSS definierte Fallback-Darstellungen bereitstellen können.

| Material | Möglicher Fallback |
|---|---|
| Liquid Glass | Acrylic oder vereinfachte Transparenz |
| Acrylic | Frosted Glass oder Solid |
| Frosted Glass | Vereinfachter Blur oder Solid |
| Crystal | Reduzierte Licht- und Reflexionseffekte |
| Solid | Einfache opake Darstellung |

Die Auswahl MUSS die Lesbarkeit und Bedienbarkeit der Oberfläche erhalten.

Sicherheitsrelevante Inhalte DÜRFEN durch vereinfachte Transparenz nicht unbeabsichtigt sichtbar werden.

## 8. Runtime Switching

NovaOS SOLL Kompositionsverfahren zur Laufzeit wechseln können.

Dabei gilt:

1. Ein Fehler oder eine Einschränkung wird erkannt.
2. Ein kompatibles Fallback-Verfahren wird ausgewählt.
3. Laufende Ressourcenverwendungen werden sicher abgeschlossen oder verworfen.
4. Der gültige Composition State wird übernommen.
5. Erforderliche Ressourcen werden neu erstellt.
6. Betroffene Ausgaben werden vollständig invalidiert.
7. Die Komposition wird mit dem neuen Verfahren fortgesetzt.

Ein Backend-Wechsel MUSS keine Neuerstellung des logischen Composition Tree erfordern.

## 9. State Preservation

Der Compositor MUSS den logischen Kompositionszustand unabhängig vom verwendeten Rendering-Backend verwalten.

Erhalten bleiben MÜSSEN insbesondere:

- Layer-Hierarchie
- Surface-Zuordnungen
- Z-Order
- Transformationen
- Opacity und Clipping
- Materialreferenzen
- Relevante Zustandsversionen

Backendabhängige Caches DÜRFEN beim Wechsel verworfen werden.

Ungültige GPU-Ressourcen DÜRFEN nicht weiterverwendet werden.

## 10. Recovery

NovaOS SOLL nach Wiederherstellung der Hardwarefunktionen zum bevorzugten Kompositionsverfahren zurückkehren können.

Dabei gilt:

- Die Hardwareverfügbarkeit muss erneut geprüft werden.
- Benötigte Ressourcen müssen erfolgreich initialisiert sein.
- Ein Wechsel darf laufende Präsentationen nicht beschädigen.
- Wiederholte Wechsel zwischen Verfahren sollen vermieden werden.

Ein automatischer Wiederherstellungsversuch DARF die Stabilität der Oberfläche nicht gefährden.

## 11. Integration

Nova Composition Fallback arbeitet mit:

- **Nova Compositor:** Verwaltung des Composition State.
- **Nova Render Engine:** Auswahl des Rendering-Backends.
- **Nova Material Engine:** Vereinfachung nicht verfügbarer Effekte.
- **Nova GPU Resource Management:** Erkennung von Ressourcenproblemen.
- **Nova Display Server:** Ausgabe und Präsentation.
- **Nova Damage Tracking:** Invalidierung betroffener Bereiche.
- **Nova Frame Scheduling:** Koordination der Wiederaufnahme.

Die Entscheidung über einen Backend-Wechsel erfolgt durch den Compositor in Abstimmung mit den betroffenen Systemkomponenten.

## 12. Fehlerbehandlung und Diagnose

Fallback-Ereignisse MÜSSEN diagnostizierbar sein.

NovaOS SOLL mindestens erfassen:

- Fehlerursache
- Vorheriges Kompositionsverfahren
- Gewähltes Fallback-Verfahren
- Betroffene Ausgabe
- Wiederherstellungsstatus

Nicht kritische Fehler SOLLEN ohne unnötige Benutzerunterbrechung behandelt werden.

Ein dauerhaft eingeschränkter Grafikbetrieb SOLL über die zentrale NovaOS-Systemdiagnose nachvollziehbar sein.

## 13. Verbindliche Anforderungen

1. NovaOS MUSS alternative Kompositionsverfahren unterstützen.
2. Ein GPU-unabhängiger Software-Compositor MUSS verfügbar sein.
3. Der logische Composition State MUSS backendunabhängig bleiben.
4. Fehlende Hardwarefähigkeiten MÜSSEN erkannt werden.
5. Fallback-Verfahren MÜSSEN die grafische Korrektheit erhalten.
6. Komplexe Materialeffekte DÜRFEN kontrolliert vereinfacht werden.
7. Die Bedienbarkeit der Oberfläche MUSS erhalten bleiben.
8. Backend-Wechsel MÜSSEN Ressourcenlebenszeiten und Synchronisation berücksichtigen.
9. Ungültige GPU-Ressourcen DÜRFEN nicht wiederverwendet werden.
10. Nach einem Backend-Wechsel MUSS eine vollständige Invalidierung betroffener Ausgaben möglich sein.
11. Eine automatische Wiederherstellung SOLL unterstützt werden.
12. Wiederholte unnötige Backend-Wechsel SOLLEN vermieden werden.
13. Fallback-Ereignisse MÜSSEN diagnostizierbar sein.
14. Der deterministische Modus MUSS reproduzierbare Fallback-Entscheidungen ermöglichen.

## 14. Ergebnis

Nova Composition Fallback gewährleistet die Funktionsfähigkeit der grafischen Oberfläche auch bei eingeschränkter oder ausgefallener GPU-Unterstützung.

Durch abgestufte Kompositionsverfahren, backendunabhängige Zustandsverwaltung und kontrollierte Wiederherstellung bleibt NovaOS bedienbar und stabil.

Damit wird die Grafikarchitektur fehlertolerant, hardwareunabhängiger und für unterschiedliche Leistungsklassen geeignet.