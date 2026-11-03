# NPSPEC-COMPOSITOR-OCCLUSION-0001 – Nova Occlusion Culling

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Compositor

## 1. Zweck

Nova Occlusion Culling definiert die Erkennung und Auslassung vollständig verdeckter grafischer Bereiche während der Komposition.

Ziel ist die Reduzierung unnötiger Rendering-Operationen, Speicherzugriffe und GPU-Belastung.

Nur tatsächlich sichtbare oder für sichtbare Effekte benötigte Inhalte SOLLEN verarbeitet werden.

## 2. Architekturprinzipien

- **Visibility-Driven:** Die Verarbeitung richtet sich nach der tatsächlichen Sichtbarkeit.
- **Region-Based:** Verdeckung wird räumlich analysiert.
- **Hierarchical:** Layer-Hierarchien werden berücksichtigt.
- **Incremental:** Unveränderte Sichtbarkeitsinformationen bleiben erhalten.
- **Material-Aware:** Transparenz und Effektabhängigkeiten werden berücksichtigt.
- **Damage-Aware:** Sichtbarkeitsänderungen erzeugen gezielte Aktualisierungen.
- **Conservative:** Unsichere Bereiche werden nicht ausgeschlossen.
- **Deterministic:** Identische Zustände erzeugen reproduzierbare Ergebnisse.

## 3. Occlusion Model

Der Compositor MUSS für jeden relevanten Layer folgende Informationen bestimmen können:

- **Visible Region:** Tatsächlich sichtbarer Bereich.
- **Occluded Region:** Durch andere Layer verdeckter Bereich.
- **Opaque Region:** Nachweislich vollständig opaker Bereich.
- **Effect Dependency Region:** Für sichtbare Effekte benötigter Bereich.
- **Visibility State:** Sichtbarkeitsstatus.
- **State Version:** Zugehörige Kompositionsversion.

Die Sichtbarkeit MUSS im Koordinatensystem der jeweiligen Ausgabe korrekt abgebildet werden.

## 4. Occlusion Analysis

Die Analyse erfolgt grundsätzlich von den vorderen zu den hinteren Layern.

Dabei gilt:

1. Die Layer werden entsprechend ihrer Z-Order betrachtet.
2. Sichtbare und opake Bereiche werden bestimmt.
3. Vollständig verdeckte Regionen werden identifiziert.
4. Effekt- und Backdrop-Abhängigkeiten werden berücksichtigt.
5. Die verbleibenden Kompositionsbereiche werden ermittelt.

Transformationen, Clipping, Alpha-Masken und Gruppen-Opacity MÜSSEN berücksichtigt werden.

Nur nachweislich opake Bereiche DÜRFEN zur vollständigen Verdeckung darunterliegender Inhalte verwendet werden.

## 5. Visibility Classification

NovaOS unterscheidet:

| Zustand | Verarbeitung |
|---|---|
| Fully Visible | Vollständige relevante Komposition |
| Partially Visible | Komposition sichtbarer Teilbereiche |
| Fully Occluded | Komposition DARF entfallen |
| Effect-Dependent | Verarbeitung erforderlicher Effekt-Eingaben |
| Unknown | Konservative Komposition |

Eine vollständig verdeckte Surface DARF von der aktuellen Komposition ausgeschlossen werden, sofern keine sichtbaren Effektabhängigkeiten bestehen.

## 6. Hierarchical Occlusion

NovaOS SOLL Verdeckung innerhalb des Composition Tree hierarchisch analysieren.

Dabei gilt:

- Vollständig verdeckte Layer-Gruppen können gemeinsam ausgeschlossen werden.
- Sichtbare Teilbereiche werden an untergeordnete Layer weitergegeben.
- Transformationen und Clipping begrenzen die relevanten Bereiche.
- Änderungen übergeordneter Layer invalidieren betroffene Sichtbarkeitsinformationen.

Die Analyse SOLL unnötige Prüfungen einzelner Layer vermeiden.

## 7. Material-Aware Occlusion

NovaOS MUSS zwischen visueller Verdeckung und grafischer Datenabhängigkeit unterscheiden.

Insbesondere bei:

- Acrylic
- Liquid Glass
- Frosted Glass
- Crystal
- Blur
- Refraction
- Reflection

können verdeckte Inhalte weiterhin als Eingaben für sichtbare Materialeffekte benötigt werden.

Solche Inhalte DÜRFEN nicht aus der erforderlichen Effektverarbeitung ausgeschlossen werden.

Die Material Engine MUSS die dafür notwendigen Abhängigkeitsinformationen bereitstellen.

## 8. Incremental Occlusion Culling

Sichtbarkeitsinformationen SOLLEN zwischen Frames wiederverwendet werden.

Eine Neubewertung ist insbesondere erforderlich bei:

- Änderungen der Layer-Geometrie
- Änderungen der Z-Order
- Änderungen von Opacity oder Clipping
- Änderungen der Materialeigenschaften
- Änderungen der Ausgabegeometrie

Unveränderte Layer-Bereiche SOLLEN ihre gültigen Occlusion-Ergebnisse behalten.

## 9. Damage Integration

Nova Occlusion Culling MUSS mit Nova Damage Tracking zusammenarbeiten.

Dabei gilt:

- Verdeckter Damage DARF für die aktuelle Komposition zurückgestellt werden.
- Sichtbar werdende Bereiche MÜSSEN bei Bedarf aktualisiert werden.
- Änderungen der Verdeckung MÜSSEN betroffene Ausgabebereiche invalidieren.
- Zurückgestellter Damage DARF nicht verloren gehen.

Occlusion Culling bestimmt die erforderliche Kompositionsarbeit.

Nova Occlusion-Aware Damage Tracking verwaltet die sichtbarkeitsabhängige Invalidierung.

## 10. Optimization

NovaOS SOLL folgende Optimierungen unterstützen:

- Region-Based Occlusion
- Hierarchical Occlusion
- Opaque Region Accumulation
- Early Layer Rejection
- Incremental Visibility Caching
- Damage-Restricted Occlusion Analysis

Die Optimierung MUSS den Verwaltungsaufwand berücksichtigen.

Bei hoher Fragmentierung DARF eine gröbere Sichtbarkeitsanalyse verwendet werden.

## 11. Integration

Nova Occlusion Culling arbeitet mit:

- **Nova Composition Layer Management:** Layer-Hierarchie und Z-Order.
- **Nova Surface Composition:** Zusammensetzung sichtbarer Inhalte.
- **Nova Damage Tracking:** Verwaltung verdeckter Änderungen.
- **Nova Material Engine:** Transparenz und Effektabhängigkeiten.
- **Nova Render Graph:** Ausschluss unnötiger Kompositionsoperationen.
- **Nova Frame Scheduler:** Bedarfsgesteuerte Frame-Erzeugung.

## 12. Fehlerbehandlung

Kann eine vollständige Verdeckung nicht zuverlässig bestimmt werden, MUSS der betroffene Bereich als potenziell sichtbar behandelt werden.

Bei ungültigen Occlusion-Daten MUSS NovaOS auf reguläre Komposition ohne Occlusion-Optimierung zurückfallen können.

Grafische Korrektheit besitzt Vorrang vor Performance-Optimierung.

## 13. Verbindliche Anforderungen

1. NovaOS MUSS Occlusion Culling unterstützen.
2. Vollständig und teilweise verdeckte Bereiche MÜSSEN unterscheidbar sein.
3. Nur nachweislich opake Regionen DÜRFEN vollständige Verdeckung begründen.
4. Layer-Hierarchien und Transformationen MÜSSEN berücksichtigt werden.
5. Vollständig verdeckte Bereiche SOLLEN nicht unnötig komponiert werden.
6. Material- und Backdrop-Abhängigkeiten MÜSSEN erhalten bleiben.
7. Sichtbarkeitsinformationen SOLLEN inkrementell wiederverwendet werden.
8. Änderungen der Verdeckung MÜSSEN korrekten Damage erzeugen.
9. Zurückgestellter Damage DARF nicht verloren gehen.
10. Occlusion-Ergebnisse MÜSSEN versionssicher verarbeitet werden.
11. Die Analyse SOLL auf relevante Damage Regions begrenzt werden.
12. Ein Fallback ohne Occlusion Culling MUSS verfügbar sein.
13. Identische Eingaben MÜSSEN im deterministischen Modus reproduzierbare Ergebnisse erzeugen.

## 14. Ergebnis

Nova Occlusion Culling reduziert den Kompositionsaufwand durch die gezielte Auslassung vollständig verdeckter Bildbereiche.

Durch hierarchische Sichtbarkeitsanalyse, inkrementelle Aktualisierung und Berücksichtigung von Materialabhängigkeiten bleibt die Darstellung korrekt.

Damit werden CPU-, GPU- und Speicherbandbreitenbedarf insbesondere bei überlappenden Fenstern und komplexen Liquid-Glass-Oberflächen reduziert.