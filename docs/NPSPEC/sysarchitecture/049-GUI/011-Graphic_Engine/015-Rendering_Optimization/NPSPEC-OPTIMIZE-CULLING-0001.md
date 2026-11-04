# NPSPEC-OPTIMIZE-CULLING-0001 – Nova Render Culling

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Graphics / Nova Graphics Optimization

## 1. Zweck

Nova Render Culling definiert die gezielte Entfernung nicht benötigter Rendering-Operationen aus der Grafikverarbeitung.

Ziel ist, ausschließlich diejenigen Scene Nodes, Draw Commands, Render-Passes und Effektberechnungen auszuführen, die für das korrekte Endergebnis erforderlich sind.

Render Culling MUSS vor der GPU-Ausführung erfolgen und SOLL unnötige CPU- und GPU-Arbeit frühzeitig verhindern.

## 2. Architekturprinzipien

- **Visibility-Aware:** Nicht sichtbare Inhalte werden ausgeschlossen.
- **Dependency-Aware:** Indirekt benötigte Inhalte bleiben erhalten.
- **Region-Based:** Culling berücksichtigt sichtbare und beschädigte Regionen.
- **Hierarchical:** Ganze Szenenbereiche können gemeinsam ausgeschlossen werden.
- **Incremental:** Unveränderte Culling-Ergebnisse werden wiederverwendet.
- **Cost-Based:** Der Aufwand der Analyse wird berücksichtigt.
- **Conservative:** Unsichere Entscheidungen führen nicht zu fehlenden Bildinhalten.
- **Deterministic:** Identische Eingaben erzeugen reproduzierbare Culling-Ergebnisse.

## 3. Architektur

Nova Render Culling verwendet bestehende Informationen aus Scene Graph, Visibility Optimization, Damage Tracking und Render Graph.

| Komponente | Verantwortung |
|---|---|
| Render Culling Controller | Koordination der Culling-Verfahren |
| Scene Culling Resolver | Ausschluss nicht benötigter Scene Nodes |
| Region Culling Resolver | Prüfung relevanter Renderregionen |
| Command Culling Resolver | Ausschluss unnötiger Zeichenbefehle |
| Pass Culling Resolver | Ermittlung nicht benötigter Render-Passes |
| Dependency Validator | Prüfung indirekter Rendering-Abhängigkeiten |

Eine zweite Sichtbarkeits- oder Szenenverwaltung DARF nicht entstehen.

## 4. Culling Model

NovaOS MUSS folgende Culling-Zustände unterscheiden:

| Zustand | Bedeutung |
|---|---|
| Required | Rendering erforderlich |
| Partially Required | Nur Teilbereiche erforderlich |
| Culled | Rendering nicht erforderlich |
| Dependency Required | Indirekt für andere Berechnungen erforderlich |
| Unknown | Notwendigkeit nicht sicher bestimmbar |

Ein Renderobjekt DARF nur ausgeschlossen werden, wenn seine Entfernung das erforderliche Endergebnis nicht verändert.

## 5. Culling Strategies

NovaOS SOLL folgende Verfahren unterstützen:

| Verfahren | Beschreibung |
|---|---|
| Viewport Culling | Ausschluss von Inhalten außerhalb des Viewports |
| Occlusion Culling | Ausschluss vollständig verdeckter Inhalte |
| Hierarchical Culling | Ausschluss vollständiger Scene-Graph-Teilbäume |
| Damage-Based Culling | Ausschluss unveränderter Renderregionen |
| Region Culling | Begrenzung auf benötigte Pixelbereiche |
| Command Culling | Entfernung unnötiger Draw Commands |
| Pass Culling | Entfernung nicht benötigter Render-Passes |
| Effect Culling | Ausschluss nicht benötigter Materialeffekte |
| State-Based Culling | Ausschluss deaktivierter oder wirkungsloser Operationen |

Mehrere Verfahren DÜRFEN kombiniert werden.

## 6. Scene Graph Culling

NovaOS MUSS Scene Nodes anhand ihrer tatsächlichen Rendering-Relevanz prüfen können.

Dabei gilt:

- Unsichtbare Nodes SOLLEN ausgeschlossen werden.
- Vollständig außerhalb des Viewports liegende Nodes SOLLEN ausgeschlossen werden.
- Vollständig verdeckte Nodes SOLLEN ausgeschlossen werden, sofern keine indirekten Abhängigkeiten bestehen.
- Ganze Teilbäume DÜRFEN gemeinsam ausgeschlossen werden.
- Transformationen, Clipping und Materialeigenschaften MÜSSEN berücksichtigt werden.

Scene Nodes bleiben Bestandteil des persistenten Scene Graph, auch wenn ihre Rendering-Operationen ausgeschlossen werden.

## 7. Damage-Based Culling

Nova Render Culling MUSS mit Nova Damage Tracking zusammenarbeiten.

Dabei gilt:

- Renderoperationen ohne Schnittmenge mit relevanten Damage Regions SOLLEN entfallen.
- Unveränderte Inhalte SOLLEN gültige Render-Ergebnisse wiederverwenden.
- Filterradien und räumliche Effektabhängigkeiten MÜSSEN berücksichtigt werden.
- Alte und neue Objektpositionen MÜSSEN bei Änderungen berücksichtigt werden.
- Ungültige Cache-Regionen DÜRFEN nicht fälschlich ausgeschlossen werden.

Damage-Based Culling DARF nur angewendet werden, wenn die benötigten Zielpixel bereits gültig sind oder anderweitig korrekt erzeugt werden.

## 8. Hierarchical Culling

NovaOS SOLL hierarchisches Culling unterstützen.

Dabei gilt:

- Sichtbarkeitsinformationen können auf Teilbaumebene gespeichert werden.
- Bounding Regions SOLLEN zur frühzeitigen Prüfung verwendet werden.
- Vollständig irrelevante Teilbäume können übersprungen werden.
- Teilweise relevante Teilbäume werden weiter untersucht.
- Änderungen an übergeordneten Transformationen MÜSSEN berücksichtigt werden.

Hierarchisches Culling SOLL unnötige Prüfungen einzelner Scene Nodes vermeiden.

## 9. Render Command Culling

NovaOS MUSS unnötige Render Commands vor der GPU-Ausführung entfernen können.

Dabei gilt:

- Nicht sichtbare Draw Commands SOLLEN entfallen.
- Zeichenbefehle außerhalb relevanter Renderregionen SOLLEN entfallen.
- Wirkungslos gewordene Operationen DÜRFEN entfernt werden.
- Ressourcenabhängigkeiten und Synchronisation MÜSSEN erhalten bleiben.
- Befehle mit relevanten Seiteneffekten DÜRFEN nicht allein aufgrund fehlender sichtbarer Pixel entfernt werden.

Command Culling MUSS mit Nova GPU Command Architecture kompatibel sein.

## 10. Render Pass Culling

NovaOS MUSS nicht benötigte Render-Passes erkennen können.

Dabei gilt:

- Render-Passes ohne benötigte Ausgaben SOLLEN entfallen.
- Unveränderte Ergebnisse können durch gültige Cache-Ressourcen ersetzt werden.
- Abhängige Render-Passes MÜSSEN berücksichtigt werden.
- Render-Passes mit notwendigen Seiteneffekten DÜRFEN nicht entfernt werden.
- GPU Resource Barriers und Ressourcenlebenszeiten MÜSSEN korrekt bleiben.

Die endgültige Pass-Entfernung erfolgt über Nova Render Graph Optimization.

## 11. Material-Aware Culling

NovaOS MUSS Materialabhängigkeiten berücksichtigen.

Insbesondere gilt:

- Solid-Materialien können vollständig verdeckte Inhalte ausschließen.
- Transparente Materialien benötigen gegebenenfalls darunterliegende Bildinformationen.
- Acrylic und Frosted Glass können Backdrop-Abhängigkeiten besitzen.
- Liquid Glass kann Blur-, Refraction- und Reflection-Eingaben benötigen.
- Nicht sichtbare Effekt-Ausgaben können weiterhin als Eingaben anderer Effekte erforderlich sein.

Materialeffekte MÜSSEN ihre relevanten Abhängigkeiten deklarieren.

## 12. Incremental Culling

NovaOS MUSS Culling-Entscheidungen inkrementell aktualisieren können.

Der logische Ablauf lautet:

`Scene Change → Visibility Analysis → Damage Analysis → Dependency Validation → Culling Decision → Render Graph`

Dabei gilt:

1. Relevante Änderungen werden erkannt.
2. Vorhandene Sichtbarkeitsinformationen werden geprüft.
3. Betroffene Renderregionen werden ermittelt.
4. Indirekte Abhängigkeiten werden validiert.
5. Nicht benötigte Operationen werden ausgeschlossen.
6. Erforderliche Operationen werden an die Rendering-Pipeline übergeben.

Unveränderte Culling-Ergebnisse SOLLEN wiederverwendet werden.

## 13. Cost-Based Culling

NovaOS SOLL den Aufwand der Culling-Analyse berücksichtigen.

Dabei gilt:

- Einfache Prüfungen SOLLEN frühzeitig erfolgen.
- Aufwendige Analysen SOLLEN nur bei erwartetem Nutzen eingesetzt werden.
- Kleine Renderregionen DÜRFEN vereinfachte Verfahren verwenden.
- Wiederverwendbare Sichtbarkeitsinformationen SOLLEN bevorzugt werden.
- Bei höheren Analysekosten DARF auf zusätzliche Culling-Optimierungen verzichtet werden.

Der Verzicht auf eine Optimierung DARF die Rendering-Korrektheit nicht beeinträchtigen.

## 14. Integration

Nova Render Culling MUSS mit folgenden Subsystemen zusammenarbeiten:

| Subsystem | Verantwortung |
|---|---|
| Nova Scene Graph | Szenenstruktur und Node-Zustände |
| Nova Visibility Optimization | Sichtbarkeits- und Verdeckungsinformationen |
| Nova Damage Tracking | Betroffene Pixelregionen |
| Nova Render Engine | Erzeugung notwendiger Renderoperationen |
| Nova Render Graph | Pass-Abhängigkeiten und Ausführungsplanung |
| Nova GPU Command Architecture | GPU-Befehle |
| Nova Material Engine | Effektabhängigkeiten |
| Nova Graphics Optimization | Übergreifende Optimierungsentscheidungen |

Render Culling entscheidet über die Notwendigkeit von Operationen, nicht über deren physische GPU-Ausführung.

## 15. Sicherheit und Fallback

NovaOS MUSS folgende Situationen sicher behandeln:

- Unbekannte Sichtbarkeit.
- Ungültige Bounding Regions.
- Fehlende Damage-Informationen.
- Nicht auflösbare Materialabhängigkeiten.
- Inkonsistente Szenenversionen.
- Ungültige Cache-Ergebnisse.
- Fehlerhafte Culling-Entscheidungen.

Bei unklarer Rendering-Relevanz MUSS die betreffende Operation erhalten bleiben.

Culling DARF keine geschützten Inhalte offenlegen oder notwendige Synchronisationsoperationen entfernen.

## 16. Verbindliche Anforderungen

1. NovaOS MUSS Render Culling unterstützen.
2. Nicht benötigte Renderoperationen SOLLEN vor der GPU-Ausführung ausgeschlossen werden.
3. Viewport Culling MUSS unterstützt werden.
4. Occlusion Culling MUSS integriert werden.
5. Hierarchical Culling SOLL unterstützt werden.
6. Damage-Based Culling MUSS möglich sein.
7. Region-Based Culling MUSS unterstützt werden.
8. Render Command Culling MUSS möglich sein.
9. Render Pass Culling MUSS integriert werden.
10. Materialabhängigkeiten MÜSSEN berücksichtigt werden.
11. Indirekt benötigte Inhalte DÜRFEN nicht ausgeschlossen werden.
12. Culling-Entscheidungen MÜSSEN versionierbar sein.
13. Inkrementelle Aktualisierung MUSS unterstützt werden.
14. Gültige Culling-Ergebnisse SOLLEN wiederverwendet werden.
15. Nova Scene Graph MUSS integriert werden.
16. Nova Damage Tracking MUSS berücksichtigt werden.
17. Nova Render Graph MUSS Culling-Ergebnisse verwenden können.
18. GPU-Synchronisation und Ressourcenlebenszeiten MÜSSEN erhalten bleiben.
19. Unsichere Culling-Entscheidungen MÜSSEN konservativ behandelt werden.
20. Die Culling-Analyse SOLL kostenbasiert optimiert werden.
21. Im deterministischen Modus MÜSSEN Culling-Entscheidungen bei identischen Eingaben reproduzierbar sein.

## 17. Ergebnis

Nova Render Culling verhindert die Ausführung unnötiger Rendering-Operationen durch frühzeitige Prüfung ihrer tatsächlichen Relevanz.

Durch hierarchisches, sichtbarkeitsbasiertes, regionenbasiertes und änderungsgesteuertes Culling werden unnötige Scene-Graph-Verarbeitung, Draw Commands und Render-Passes reduziert.

Damit verarbeitet NovaOS ausschließlich die für das korrekte Endergebnis erforderlichen Grafikoperationen und reduziert CPU-Last, GPU-Auslastung sowie Energieverbrauch.