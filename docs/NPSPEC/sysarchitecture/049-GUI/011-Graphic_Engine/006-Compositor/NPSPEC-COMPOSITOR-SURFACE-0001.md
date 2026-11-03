# NPSPEC-COMPOSITOR-SURFACE-0001 – Nova Surface Composition

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Compositor

## 1. Zweck

Nova Surface Composition definiert die Zusammensetzung mehrerer grafischer Surfaces zu einem vollständigen Ausgabebild.

Ziel ist eine effiziente, inkrementelle und synchronisierte Darstellung von Anwendungsfenstern, Systemoberflächen und extern gerenderten Inhalten.

Unveränderte Surfaces und Bildbereiche SOLLEN ohne erneute Berechnung wiederverwendet werden.

## 2. Architekturprinzipien

- **Surface-Based:** Grafische Inhalte werden über eigenständige Surfaces bereitgestellt.
- **Retained:** Der Kompositionszustand bleibt zwischen Frames erhalten.
- **Damage-Driven:** Nur relevante Änderungen lösen eine Aktualisierung aus.
- **Zero-Copy:** Unnötige Kopien von Surface-Inhalten werden vermieden.
- **Layer-Aware:** Z-Order, Transparenz und Sichtbarkeit werden berücksichtigt.
- **Material-Aware:** Material- und Backdrop-Abhängigkeiten werden unterstützt.
- **Synchronized:** Surface-Inhalte werden ausschließlich in gültigen Zuständen verwendet.
- **Backend-Independent:** Die Komposition bleibt unabhängig von einer bestimmten GPU-API.

## 3. Surface Composition Model

Eine Surface Composition beschreibt die Einbindung einer grafischen Oberfläche in den Composition Tree.

Sie enthält mindestens:

- **Surface ID:** Eindeutige Identifikation.
- **Buffer Reference:** Aktueller grafischer Inhalt.
- **Content Version:** Version des Surface-Inhalts.
- **Transform:** Position, Skalierung und Rotation.
- **Z-Order:** Darstellungsreihenfolge.
- **Opacity:** Transparenz.
- **Clip Region:** Sichtbare Begrenzung.
- **Damage Regions:** Veränderte Bildbereiche.
- **Synchronization State:** Verfügbarkeit des Inhalts.

Eine Surface DARF mehrfach innerhalb einer Komposition referenziert werden.

## 4. Composition Process

Die Zusammensetzung erfolgt grundsätzlich in folgenden Schritten:

1. Konsistenten Composition State übernehmen.
2. Verfügbare Surface-Buffer bestimmen.
3. Transformationen und Z-Order auswerten.
4. Sichtbare und verdeckte Bereiche ermitteln.
5. Damage Regions und Materialabhängigkeiten berücksichtigen.
6. Erforderliche Composition Operations planen.
7. Betroffene Ausgabebereiche zusammensetzen.
8. Ergebnis an den Display Server übergeben.

Die Komposition MUSS auf gültigen Surface- und Buffer-Zuständen basieren.

## 5. Surface Transformations

NovaOS MUSS folgende Transformationen unterstützen:

- Translation
- Skalierung
- Rotation
- Clipping
- Opacity

Transformationen MÜSSEN bei der Berechnung sichtbarer Bereiche und Damage Regions berücksichtigt werden.

Änderungen der Surface-Geometrie MÜSSEN sowohl die vorherige als auch die neue Position invalidieren.

Die Transformation DARF nicht automatisch eine Neuberechnung des ursprünglichen Surface-Inhalts erzwingen.

## 6. Surface Blending

Der Compositor MUSS die korrekte Zusammensetzung transparenter und opaker Surfaces unterstützen.

Dabei sind insbesondere zu berücksichtigen:

- Alpha Blending
- Premultiplied Alpha
- Opacity
- Farbformate
- Farbräume
- Materialeffekte

Blending MUSS entsprechend den definierten Farb- und Alpha-Konventionen erfolgen.

Opake Bereiche SOLLEN ohne unnötige Alpha-Berechnung verarbeitet werden.

## 7. Incremental Surface Composition

NovaOS MUSS inkrementelle Surface Composition unterstützen.

Eine erneute Zusammensetzung ist insbesondere erforderlich bei:

- Änderungen des Surface-Inhalts
- Positions- oder Größenänderungen
- Änderungen der Z-Order
- Änderungen von Opacity oder Clipping
- Material- und Backdrop-Änderungen

Unveränderte Bereiche SOLLEN ihre gültigen Kompositionsergebnisse behalten.

Eine vollständige Zusammensetzung DARF erfolgen, wenn sie erforderlich oder effizienter ist.

## 8. Occlusion und Visibility

NovaOS SOLL vollständig verdeckte Surface-Bereiche von unnötiger Verarbeitung ausschließen.

Dabei gilt:

- Nur nachweislich opake Bereiche dürfen vollständige Verdeckung begründen.
- Teilweise sichtbare Bereiche sollen räumlich begrenzt werden.
- Transparente Surfaces müssen korrekt verarbeitet werden.
- Backdrop-abhängige Materialien können verdeckte Eingaben benötigen.

Occlusion DARF erforderliche Material- oder Effektberechnungen nicht unterdrücken.

## 9. Zero-Copy Composition

NovaOS SOLL Surface-Inhalte möglichst direkt aus vorhandenen Buffern verwenden.

Dazu können gehören:

- Shared GPU Textures
- Direct Surface Sampling
- Hardware Overlays
- Direct Scanout

Eine zusätzliche Kopie SOLL nur erfolgen, wenn sie technisch erforderlich oder insgesamt effizienter ist.

Zero-Copy MUSS die Zugriffsrechte, Speicherlebenszeiten und Synchronisationsanforderungen einhalten.

## 10. Surface Synchronization

Surface-Buffer können unabhängig vom Compositor durch Anwendungen aktualisiert werden.

NovaOS MUSS deshalb sicherstellen, dass:

- Nur vollständig freigegebene Buffer verwendet werden.
- Schreibende und lesende Zugriffe korrekt synchronisiert sind.
- Buffer während aktiver Verwendung nicht unzulässig überschrieben werden.
- Ressourcen erst nach Abschluss ihrer Verwendung freigegeben werden.

Die Synchronisation erfolgt in Zusammenarbeit mit Nova Display Surface Synchronization und Nova GPU Synchronization.

## 11. Material Integration

Surfaces DÜRFEN mit Materialeffekten kombiniert werden.

Dazu gehören:

- Acrylic
- Liquid Glass
- Frosted Glass
- Crystal
- Solid

Der Compositor stellt die erforderlichen Surface-, Layer- und Backdrop-Informationen bereit.

Die eigentliche Materialberechnung erfolgt über die Nova Material Engine und Render Engine.

Materialabhängige Damage Regions MÜSSEN berücksichtigt werden.

## 12. Multi-Display Composition

Eine Surface DARF auf mehreren Ausgaben dargestellt werden.

Dabei MÜSSEN ausgabespezifische Eigenschaften berücksichtigt werden:

- Auflösung
- Skalierung
- Transformation
- Farbraum
- Sichtbarkeit
- Damage Regions

Gemeinsame Surface-Inhalte SOLLEN wiederverwendet werden, sofern ihre Darstellung kompatibel ist.

## 13. Fehlerbehandlung

NovaOS MUSS ungültige Surface-Zustände erkennen können.

Dazu gehören:

- Fehlende oder ungültige Buffer
- Nicht abgeschlossene Schreiboperationen
- Ungültige Transformationen
- Nicht unterstützte Pixelformate
- Veraltete Surface-Versionen

Bei fehlenden Inhalten DARF ein zuvor gültiger Buffer weiterverwendet werden, sofern dessen Verwendung sicher und zulässig ist.

Andernfalls MUSS ein definierter Fallback erfolgen.

Ungültige Buffer DÜRFEN nicht gelesen werden.

## 14. Verbindliche Anforderungen

1. NovaOS MUSS mehrere Surfaces zu einer Ausgabe zusammensetzen können.
2. Surface-Inhalte und Composition State MÜSSEN getrennt verwaltet werden.
3. Transformationen, Z-Order, Clipping und Opacity MÜSSEN unterstützt werden.
4. Transparente und opake Surfaces MÜSSEN korrekt zusammengesetzt werden.
5. Damage-Driven Composition MUSS unterstützt werden.
6. Unveränderte Surface-Inhalte SOLLEN wiederverwendet werden.
7. Geometrieänderungen MÜSSEN alte und neue Ausgabebereiche berücksichtigen.
8. Occlusion Culling SOLL unnötige Verarbeitung vermeiden.
9. Material- und Backdrop-Abhängigkeiten MÜSSEN berücksichtigt werden.
10. Zero-Copy Composition SOLL bevorzugt werden.
11. Surface-Buffer MÜSSEN korrekt synchronisiert werden.
12. Mehrere Bildschirmausgaben MÜSSEN unterstützt werden.
13. Ungültige Surface-Buffer DÜRFEN nicht verwendet werden.
14. Ein Software-Fallback MUSS verfügbar sein.
15. Identische Eingaben MÜSSEN im deterministischen Modus reproduzierbare Kompositionsergebnisse ermöglichen.

## 15. Ergebnis

Nova Surface Composition ermöglicht die effiziente Zusammensetzung unabhängiger grafischer Oberflächen innerhalb des Nova Compositor.

Durch retained Composition, Damage Tracking, Occlusion Culling und Zero-Copy werden ausschließlich notwendige Bildbereiche verarbeitet.

Damit bildet das Subsystem die Grundlage für eine flüssige und ressourcenschonende Darstellung von Fenstern, Systemoberflächen und komplexen Materialeffekten in NovaOS.