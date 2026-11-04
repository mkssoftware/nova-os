# NPSPEC-OPTIMIZE-OCCLUSION-0001 – Nova Visibility and Occlusion Optimization

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Graphics / Nova Graphics Optimization

## 1. Zweck

Nova Visibility and Occlusion Optimization definiert die effiziente Ermittlung sichtbarer, teilweise verdeckter und vollständig verdeckter Grafikinhalte.

Ziel ist die Vermeidung unnötiger Rendering- und Kompositionsoperationen für Bildbereiche, die keinen Einfluss auf das sichtbare Endergebnis besitzen.

Die Optimierung MUSS Transparenz, Materialeffekte und indirekte Bildabhängigkeiten berücksichtigen.

## 2. Architekturprinzipien

- **Visibility-Driven:** Nur für das Endergebnis relevante Inhalte werden verarbeitet.
- **Occlusion-Aware:** Vollständige und teilweise Verdeckung werden unterschieden.
- **Region-Based:** Sichtbarkeit wird räumlich ausgewertet.
- **Dependency-Aware:** Indirekte Abhängigkeiten bleiben erhalten.
- **Incremental:** Sichtbarkeitsänderungen werden gezielt aktualisiert.
- **Material-Aware:** Transparenz und Hintergrundeffekte werden berücksichtigt.
- **Conservative:** Unsichere Sichtbarkeitsentscheidungen dürfen keine Bildfehler verursachen.
- **Deterministic:** Identische Szenenzustände erzeugen reproduzierbare Entscheidungen.

## 3. Architektur

Nova Visibility and Occlusion Optimization verwendet die bestehenden Scene-Graph-, Damage-Tracking- und Compositor-Strukturen.

| Komponente | Verantwortung |
|---|---|
| Visibility Resolver | Ermittlung sichtbarer Inhalte |
| Occlusion Analyzer | Analyse verdeckender Oberflächen |
| Visible Region Manager | Verwaltung sichtbarer Pixelregionen |
| Dependency Validator | Prüfung indirekter Bildabhängigkeiten |
| Visibility Change Tracker | Erkennung von Sichtbarkeitsänderungen |
| Occlusion Strategy Selector | Auswahl geeigneter Optimierungsverfahren |

Eine zweite unabhängige Szenen- oder Kompositionshierarchie DARF nicht entstehen.

## 4. Visibility Model

NovaOS MUSS folgende Sichtbarkeitszustände unterscheiden:

| Zustand | Bedeutung |
|---|---|
| Fully Visible | Inhalt vollständig sichtbar |
| Partially Visible | Inhalt teilweise sichtbar |
| Fully Occluded | Inhalt vollständig verdeckt |
| Outside Viewport | Inhalt außerhalb des sichtbaren Bereichs |
| Effect-Dependent | Inhalt wird indirekt für einen Effekt benötigt |
| Unknown | Sichtbarkeit nicht sicher bestimmbar |

Ein vollständig verdeckter Inhalt DARF nur dann von der Verarbeitung ausgeschlossen werden, wenn keine relevante indirekte Abhängigkeit besteht.

## 5. Occlusion Analysis

NovaOS MUSS Verdeckung anhand der tatsächlichen Szenen- und Kompositionsreihenfolge bestimmen.

Dabei gilt:

- Vollständig opake Oberflächen können darunterliegende Inhalte verdecken.
- Teilweise transparente Oberflächen dürfen nicht als vollständig verdeckend behandelt werden.
- Clipping und Transformationen MÜSSEN berücksichtigt werden.
- Alte und neue Oberflächenpositionen MÜSSEN bei Änderungen berücksichtigt werden.
- Sichtbarkeitsinformationen MÜSSEN an definierte Szenenversionen gebunden sein.

Die Analyse SOLL hierarchische Sichtbarkeitsinformationen wiederverwenden.

## 6. Region-Based Visibility

NovaOS MUSS Sichtbarkeit auf Pixelregionen begrenzen können.

Dabei gilt:

- Sichtbare Regionen werden durch Schnittmengen und Differenzen bestimmt.
- Vollständig verdeckte Regionen SOLLEN von unnötigen Renderoperationen ausgeschlossen werden.
- Teilweise sichtbare Inhalte SOLLEN nur in benötigten Regionen verarbeitet werden.
- Komplexe Regionen DÜRFEN konservativ vereinfacht werden.
- Unsichere Regionen MÜSSEN als potenziell sichtbar behandelt werden.

Die Regionsermittlung MUSS mit Nova Damage Region Management kompatibel sein.

## 7. Material-Aware Occlusion

NovaOS MUSS Materialeigenschaften bei der Verdeckungsanalyse berücksichtigen.

Insbesondere gilt:

- Solid-Materialien können bei vollständiger Opazität verdeckend wirken.
- Acrylic- und Frosted-Glass-Materialien können Hintergrundinformationen benötigen.
- Liquid Glass kann Backdrop-, Blur-, Refraction- und Reflection-Abhängigkeiten besitzen.
- Transparente Materialien DÜRFEN benötigte Hintergrundinhalte nicht fälschlich ausschließen.
- Materialeffekte MÜSSEN ihre räumlichen Eingabeabhängigkeiten deklarieren.

Sichtbare Ausgaberegion und benötigte Eingaberegion MÜSSEN unterscheidbar bleiben.

## 8. Incremental Visibility Updates

NovaOS MUSS Sichtbarkeitsinformationen inkrementell aktualisieren können.

Der logische Ablauf lautet:

`Scene Change → Visibility Analysis → Occlusion Update → Dependency Validation → Render Optimization`

Dabei gilt:

1. Änderungen an Geometrie, Ebenenreihenfolge oder Material werden erkannt.
2. Betroffene Sichtbarkeitsregionen werden ermittelt.
3. Verdeckungsinformationen werden aktualisiert.
4. Indirekte Abhängigkeiten werden geprüft.
5. Nicht benötigte Rendering-Operationen werden ausgeschlossen.
6. Neu sichtbare Bereiche werden bei Bedarf aktualisiert.

Unveränderte Sichtbarkeitsinformationen SOLLEN wiederverwendet werden.

## 9. Occluded Damage Handling

NovaOS MUSS Änderungen vollständig verdeckter Inhalte korrekt behandeln.

Dabei gilt:

- Verdeckte Änderungen DÜRFEN nicht verloren gehen.
- Nicht sichtbare Aktualisierungen DÜRFEN aufgeschoben werden.
- Cache-Gültigkeit und tatsächliche Inhaltsversion MÜSSEN unterscheidbar bleiben.
- Beim Wiedererscheinen MUSS der sichtbare Inhalt korrekt dargestellt werden.
- Abhängige Materialeffekte können eine Aktualisierung trotz direkter Verdeckung erfordern.

Eine aufgeschobene Aktualisierung DARF keine veralteten Bildinhalte sichtbar machen.

## 10. Render Graph und Compositor Integration

Nova Visibility and Occlusion Optimization MUSS mit Nova Render Graph und Nova Compositor zusammenarbeiten.

Dabei gilt:

- Nicht benötigte Renderoperationen SOLLEN entfallen.
- Sichtbare Teilregionen SOLLEN für Partial Rendering verwendet werden.
- Kompositionsabhängigkeiten MÜSSEN erhalten bleiben.
- Hardware Overlays und Direct Scanout MÜSSEN bei der Sichtbarkeitsbewertung berücksichtigt werden.
- GPU-Synchronisation und Ressourcenlebenszeiten bleiben verbindlich.

Die endgültige Kompositionsreihenfolge bleibt Aufgabe des Compositors.

## 11. Performance Optimization

NovaOS SOLL die Kosten der Verdeckungsanalyse gegen mögliche Einsparungen abwägen.

Dabei gilt:

- Hierarchische Occlusion-Daten SOLLEN wiederverwendet werden.
- Unveränderte Regionen SOLLEN nicht erneut analysiert werden.
- Kleine Szenen DÜRFEN vereinfachte Verfahren verwenden.
- Komplexe Szenen SOLLEN regionenbasierte Verfahren nutzen.
- Eine aufwendige Analyse DARF übersprungen werden, wenn ihre Kosten den erwarteten Nutzen übersteigen.

Die Auswahl MUSS die funktionale Korrektheit erhalten.

## 12. Sicherheit und Fallback

NovaOS MUSS folgende Situationen sicher behandeln:

- Unbekannte Materialtransparenz.
- Nicht auflösbare Effektabhängigkeiten.
- Ungültige Geometrieinformationen.
- Inkonsistente Szenenversionen.
- Fehlende Occlusion-Daten.
- Fehlerhafte Cache-Zustände.

Bei unsicherer Verdeckung MUSS der betroffene Inhalt konservativ als relevant behandelt werden.

Geschützte Oberflächeninhalte DÜRFEN nicht durch Occlusion-Optimierungen offengelegt werden.

## 13. Verbindliche Anforderungen

1. NovaOS MUSS sichtbare, teilweise sichtbare und vollständig verdeckte Inhalte unterscheiden.
2. Sichtbarkeitsinformationen MÜSSEN versioniert werden.
3. Vollständig verdeckte Inhalte SOLLEN nicht unnötig gerendert werden.
4. Region-Based Visibility MUSS unterstützt werden.
5. Transparenz und Materialeffekte MÜSSEN berücksichtigt werden.
6. Indirekte Backdrop-Abhängigkeiten DÜRFEN nicht verloren gehen.
7. Sichtbarkeitsänderungen MÜSSEN inkrementell verarbeitet werden.
8. Verdeckte Damage-Informationen MÜSSEN erhalten bleiben.
9. Neu sichtbare Inhalte MÜSSEN korrekt aktualisiert werden.
10. Nova Scene Graph MUSS integriert werden.
11. Nova Damage Tracking MUSS berücksichtigt werden.
12. Nova Compositor MUSS die Sichtbarkeitsinformationen verwenden können.
13. Nova Render Graph MUSS unnötige Operationen ausschließen können.
14. Hardware Overlays und Direct Scanout MÜSSEN kompatibel bleiben.
15. Die Analyse SOLL kostenbasiert optimiert werden.
16. Unsichere Verdeckungsentscheidungen MÜSSEN konservativ behandelt werden.
17. Geschützte Oberflächeninhalte MÜSSEN isoliert bleiben.
18. Im deterministischen Modus MÜSSEN Sichtbarkeitsentscheidungen bei identischen Eingaben reproduzierbar sein.

## 14. Ergebnis

Nova Visibility and Occlusion Optimization reduziert unnötige Grafikoperationen durch präzise Ermittlung sichtbarer und verdeckter Inhalte.

Durch regionenbasierte Verdeckungsanalyse, inkrementelle Aktualisierung und Berücksichtigung indirekter Materialabhängigkeiten können nicht benötigte Renderoperationen entfallen.

Damit verarbeitet NovaOS bevorzugt diejenigen Bildinhalte, die tatsächlich zur korrekten Darstellung des sichtbaren Desktops erforderlich sind.