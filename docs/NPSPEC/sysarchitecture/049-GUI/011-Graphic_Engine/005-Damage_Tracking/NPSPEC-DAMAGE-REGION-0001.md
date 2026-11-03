# NPSPEC-DAMAGE-REGION-0001 – Nova Damage Region Management

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Damage Tracking

## 1. Zweck

Nova Damage Region Management definiert die Verwaltung räumlich begrenzter Änderungen innerhalb der NovaOS-Grafikarchitektur.

Ziel ist die präzise Erfassung, Kombination und Optimierung betroffener Bildbereiche, damit Render Engine und Compositor ausschließlich notwendige Aktualisierungen durchführen.

Die Verwaltung erfolgt unabhängig von der tatsächlichen Rendering-Ausführung.

## 2. Architekturprinzipien

- **Region-Based:** Änderungen werden als geometrische Bereiche beschrieben.
- **Conservative:** Sämtliche betroffenen Pixel müssen erfasst bleiben.
- **Coordinate-Aware:** Unterschiedliche Koordinatensysteme werden berücksichtigt.
- **Incremental:** Neue Änderungen werden bestehenden Damage Regions hinzugefügt.
- **Merge-Aware:** Überlappende und benachbarte Bereiche können zusammengefasst werden.
- **Clip-Aware:** Regionen werden anhand gültiger Begrenzungen verarbeitet.
- **Cost-Aware:** Verwaltungsaufwand und Rendering-Kosten werden gemeinsam berücksichtigt.
- **Deterministic:** Identische Eingaben erzeugen reproduzierbare Regionen.

## 3. Damage Region Model

Eine Damage Region beschreibt einen oder mehrere betroffene Bildbereiche.

Sie enthält mindestens:

- **Region ID:** Eindeutige Identifikation.
- **Target ID:** Zugehörige Surface, Layer oder Ausgabe.
- **Bounds:** Geometrische Begrenzung.
- **Coordinate Space:** Verwendetes Koordinatensystem.
- **State Version:** Zugehörige Zustandsversion.
- **Region Type:** Partial oder Full Damage.

Eine Damage Region DARF mehrere getrennte Teilbereiche enthalten.

Die interne Darstellung SOLL rechteckbasierte Regionen effizient unterstützen.

## 4. Region Operations

NovaOS MUSS folgende Operationen unterstützen:

| Operation | Funktion |
|---|---|
| Add | Fügt einen Damage-Bereich hinzu |
| Union | Vereinigt mehrere Regionen |
| Intersect | Ermittelt gemeinsame Bereiche |
| Subtract | Entfernt definierte Teilbereiche |
| Clip | Begrenzt Regionen auf einen gültigen Bereich |
| Transform | Überführt Regionen in ein anderes Koordinatensystem |
| Expand | Erweitert Regionen um erforderliche Ränder |
| Normalize | Vereinheitlicht die Regionsdarstellung |
| Clear | Entfernt erfolgreich verarbeiteten Damage |

Die Operationen MÜSSEN konservativ korrekt arbeiten.

## 5. Region Generation

Damage Regions entstehen durch grafische Zustandsänderungen.

Dabei MÜSSEN insbesondere berücksichtigt werden:

- Änderungen von Position und Größe
- Änderungen sichtbarer Inhalte
- Transformationen und Animationen
- Änderungen von Clipping und Sichtbarkeit
- Änderungen an Surfaces und Layers
- Material- und Effektänderungen

Bei geometrischen Änderungen MÜSSEN sowohl die vorherigen als auch die neuen sichtbaren Bereiche berücksichtigt werden.

## 6. Coordinate Management

NovaOS MUSS Damage Regions zwischen unterschiedlichen Koordinatensystemen übertragen können.

Dazu gehören:

- Lokale Node-Koordinaten
- Scene-Koordinaten
- Surface-Koordinaten
- Output-Koordinaten
- Physische Pixelkoordinaten

Transformationen MÜSSEN Skalierung, Rotation und Verschiebung berücksichtigen.

Bei der Umrechnung in Pixelkoordinaten MUSS konservativ nach außen gerundet werden.

Nicht exakt darstellbare Bereiche DÜRFEN durch größere Begrenzungsrechtecke beschrieben werden.

## 7. Region Merging

NovaOS SOLL überlappende oder benachbarte Damage Regions zusammenfassen.

Dabei gilt:

- Vollständig überlappende Bereiche können vereinigt werden.
- Benachbarte Bereiche dürfen zusammengefasst werden.
- Getrennte Bereiche sollen erhalten bleiben, wenn dies effizienter ist.
- Die Zusammenfassung darf keine erforderlichen Aktualisierungen ausschließen.

Das Merging SOLL anhand der zusätzlichen Rendering-Fläche und der eingesparten Verwaltungskosten bewertet werden.

## 8. Region Fragmentation

Viele kleine Damage Regions können den Verwaltungsaufwand erhöhen.

NovaOS SOLL deshalb die Fragmentierung begrenzen.

Mögliche Maßnahmen:

- Zusammenfassung kleiner Regionen
- Verwendung größerer Begrenzungsrechtecke
- Begrenzung der maximalen Regionsanzahl
- Wechsel zu Full Damage

Die Entscheidung SOLL anhand der erwarteten Gesamtkosten erfolgen.

## 9. Region Clipping

Damage Regions SOLLEN auf tatsächlich relevante Bereiche begrenzt werden.

Dabei können berücksichtigt werden:

- Surface Bounds
- Window Bounds
- Scene Clipping
- Output Bounds
- Sichtbare Rendering-Bereiche

Clipping DARF erforderliche Eingabebereiche für Materialeffekte nicht entfernen.

Die Begrenzung der Ausgabe und die Begrenzung benötigter Eingabedaten MÜSSEN unterscheidbar bleiben.

## 10. Material-Aware Region Expansion

Bestimmte Effekte benötigen zusätzliche Bildinformationen außerhalb des ursprünglich veränderten Bereichs.

NovaOS MUSS die Erweiterung von Damage Regions unterstützen.

Dazu gehören insbesondere:

- Blur Radius
- Refraction Displacement
- Reflection Sampling
- Shadow Extents
- Filter Kernel Bounds

Die Erweiterung MUSS anhand der tatsächlichen Effektabhängigkeiten erfolgen.

Unbekannte oder nicht begrenzbare Abhängigkeiten MÜSSEN konservativ behandelt werden.

## 11. Partial und Full Damage

NovaOS unterscheidet:

**Partial Damage**
- Nur bestimmte Bereiche benötigen eine Aktualisierung.
- Unveränderte Bereiche bleiben erhalten.

**Full Damage**
- Die gesamte betroffene Surface oder Ausgabe muss aktualisiert werden.
- Eine detaillierte Regionsverwaltung ist nicht erforderlich.

NovaOS DARF zwischen beiden Verfahren wechseln.

Ein Wechsel zu Full Damage MUSS erfolgen können, wenn eine präzise Regionsbestimmung nicht zuverlässig möglich ist.

## 12. Incremental Region Management

Neue Damage Regions SOLLEN bestehenden ausstehenden Regionen hinzugefügt werden.

Dabei MUSS NovaOS:

- Mehrere Änderungen zusammenfassen können.
- Zustandsversionen berücksichtigen.
- Bereits verarbeitete Bereiche von ausstehenden unterscheiden.
- Während der Verarbeitung neu entstehende Änderungen erhalten.

Ein laufender Rendering-Vorgang DARF neu hinzugekommenen Damage nicht versehentlich löschen.

## 13. Damage Resolution

Damage Regions dürfen erst nach erfolgreicher Verarbeitung als erledigt markiert werden.

Dabei gilt:

- Erfolgreich aktualisierte Bereiche können entfernt werden.
- Fehlgeschlagene Bereiche müssen erhalten bleiben.
- Neuere Zustandsänderungen dürfen nicht durch ältere Abschlüsse bestätigt werden.
- Teilweise erfolgreiche Aktualisierungen dürfen entsprechend berücksichtigt werden.

Die Bestätigung MUSS einer eindeutig zugeordneten Zustandsversion beziehungsweise Aktualisierung entsprechen.

## 14. Integration

Nova Damage Region Management stellt normalisierte Damage Regions bereit für:

- **Scene Graph:** Hierarchische Änderungsverfolgung
- **Render Graph:** Auswahl notwendiger Rendering-Operationen
- **Render Engine:** Partial Rendering und Scissor Rendering
- **Compositor:** Inkrementelle Surface Composition
- **Frame Scheduler:** Bedarfsgesteuerte Frame-Erzeugung
- **Display Server:** Unterstützung partieller Präsentation

Die jeweiligen Subsysteme bleiben für ihre eigenen Ausführungsentscheidungen verantwortlich.

## 15. Verbindliche Anforderungen

1. NovaOS MUSS Damage Regions unabhängig von der Rendering-Ausführung verwalten.
2. Rechteckbasierte Damage Regions MÜSSEN unterstützt werden.
3. Mehrere getrennte Teilbereiche MÜSSEN darstellbar sein.
4. Union, Intersection, Subtraction und Clipping MÜSSEN unterstützt werden.
5. Koordinatentransformationen MÜSSEN konservativ korrekt erfolgen.
6. Alte und neue Objektbereiche MÜSSEN bei geometrischen Änderungen berücksichtigt werden.
7. Überlappende Regionen SOLLEN effizient zusammengefasst werden.
8. Die Regionsfragmentierung SOLL begrenzt werden.
9. Materialabhängige Regionserweiterungen MÜSSEN möglich sein.
10. Partial und Full Damage MÜSSEN unterstützt werden.
11. Neue Änderungen MÜSSEN während laufender Aktualisierungen erhalten bleiben.
12. Damage DARF erst nach erfolgreicher Verarbeitung bestätigt werden.
13. Zustandsversionen MÜSSEN bei der Damage-Verwaltung berücksichtigt werden.
14. Bei unvollständigen Informationen MUSS eine konservative Invalidierung erfolgen.
15. Identische Eingaben MÜSSEN im deterministischen Modus reproduzierbare Ergebnisse erzeugen.

## 16. Ergebnis

Nova Damage Region Management stellt eine präzise und effiziente Verwaltung räumlicher Grafikänderungen bereit.

Durch Regionsoperationen, konservative Koordinatentransformationen, adaptive Zusammenfassung und versionssichere Verarbeitung werden unnötige Aktualisierungen minimiert.

Damit bildet das Subsystem die geometrische Grundlage für Partial Rendering, Incremental Composition und bedarfsgesteuerte Frame-Erzeugung in NovaOS.