# NPSPEC-COMPOSITOR-ARCH-0001 – Nova Compositor Architecture

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Compositor

## 1. Zweck

Nova Compositor definiert die zentrale Architektur zur Zusammensetzung grafischer Oberflächen innerhalb von NovaOS.

Ziel ist die effiziente Darstellung von Fenstern, UI-Elementen, Materialeffekten und anderen grafischen Inhalten unter Berücksichtigung von Transparenz, Sichtbarkeit und räumlichen Änderungen.

Der Compositor arbeitet zustandsbasiert und aktualisiert ausschließlich erforderliche Bildbereiche.

## 2. Architekturprinzipien

- **Retained Composition:** Der aktuelle Kompositionszustand bleibt dauerhaft erhalten.
- **Damage-Driven:** Nur relevante Änderungen lösen eine neue Zusammensetzung aus.
- **Layer-Based:** Grafische Inhalte werden als geordnete Ebenen verarbeitet.
- **Surface-Based:** Anwendungen stellen ihre Inhalte über Surfaces bereit.
- **Zero-Copy:** Unnötige Speicher- und Pixelkopien werden vermieden.
- **Material-Aware:** Transparenz, Blur, Refraction und Liquid Glass werden berücksichtigt.
- **Hardware-Accelerated:** GPU-Beschleunigung wird bevorzugt.
- **Fallback-Safe:** Softwarebasierte Komposition bleibt möglich.

## 3. Architekturmodell

Der Nova Compositor verarbeitet grafische Inhalte aus unterschiedlichen Quellen.

Die grundlegende Architektur besteht aus:

1. **Surface Manager:** Verwaltet referenzierte grafische Oberflächen.
2. **Composition Tree:** Beschreibt Ebenen, Transformationen und Reihenfolge.
3. **Visibility Manager:** Ermittelt sichtbare und verdeckte Bereiche.
4. **Damage Integration:** Bestimmt erforderliche Aktualisierungen.
5. **Composition Planner:** Erstellt den Kompositionsplan.
6. **Composition Executor:** Führt die Zusammensetzung über die Render Engine aus.
7. **Output Integration:** Übergibt fertige Ergebnisse an den Display Server.

Der Compositor MUSS unabhängig von einer bestimmten GPU-API bleiben.

## 4. Composition Model

Ein Composition Node beschreibt einen darzustellenden grafischen Inhalt.

Er enthält mindestens:

- **Node ID:** Eindeutige Identifikation.
- **Surface Reference:** Zugehörige Oberfläche.
- **Transform:** Position, Skalierung und Rotation.
- **Z-Order:** Darstellungsreihenfolge.
- **Opacity:** Transparenzwert.
- **Clip Region:** Sichtbare Begrenzung.
- **Material Reference:** Optionale Materialeigenschaften.
- **State Version:** Zugehörige Zustandsversion.

Composition Nodes DÜRFEN hierarchisch organisiert werden.

Der Composition Tree ist vom logischen Nova Scene Graph zu unterscheiden.

## 5. Surface Composition

NovaOS MUSS mehrere Surfaces zu einer gemeinsamen Ausgabe zusammensetzen können.

Dabei werden berücksichtigt:

- Fensterinhalte
- Systemoberflächen
- Taskbar und Startmenü
- Kontextmenüs und Dialoge
- Overlays und Cursor
- Extern gerenderte Inhalte

Die Reihenfolge der Darstellung MUSS durch den gültigen Kompositionszustand bestimmt werden.

## 6. Layer Management

Der Compositor MUSS unterschiedliche grafische Ebenen unterstützen.

Dazu gehören:

- Normale Fenster
- Systemfenster
- Popups und Kontextmenüs
- System-Overlays
- Cursor-Ebenen

Die Ebenenreihenfolge wird durch Window Manager und Systemrichtlinien bestimmt.

Der Compositor setzt diese Vorgaben um, ohne selbst über Fensterfokus oder Benutzerinteraktionen zu entscheiden.

## 7. Incremental Composition

NovaOS MUSS inkrementelle Komposition unterstützen.

Dabei gilt:

- Unveränderte Surfaces SOLLEN nicht erneut verarbeitet werden.
- Unveränderte Composition Regions SOLLEN erhalten bleiben.
- Damage Regions MÜSSEN berücksichtigt werden.
- Gültige Composition-Caches DÜRFEN wiederverwendet werden.
- Änderungen an Position, Größe und Z-Order MÜSSEN korrekt verarbeitet werden.

Eine vollständige Neuberechnung DARF erfolgen, wenn sie erforderlich oder effizienter ist.

## 8. Visibility und Occlusion

Der Compositor MUSS sichtbare und verdeckte Bereiche bestimmen können.

Dabei gilt:

- Vollständig verdeckte opake Bereiche SOLLEN nicht unnötig verarbeitet werden.
- Teilweise sichtbare Bereiche SOLLEN räumlich begrenzt werden.
- Transparente Oberflächen MÜSSEN korrekt berücksichtigt werden.
- Materialabhängige Hintergrundinformationen MÜSSEN verfügbar bleiben.

Occlusion Culling DARF keine erforderlichen Effektberechnungen unterdrücken.

## 9. Material Integration

Nova Compositor MUSS die Nova Material Engine integrieren können.

Unterstützte Materialfamilien sind:

- Acrylic
- Liquid Glass
- Frosted Glass
- Crystal
- Solid

Materialeffekte werden durch die Material Engine beschrieben und über die Render Engine ausgeführt.

Der Compositor stellt die erforderlichen Layer-, Backdrop- und Kompositionsinformationen bereit.

## 10. Zero-Copy Composition

NovaOS SOLL unnötige Kopien von Surface-Inhalten vermeiden.

Dazu können verwendet werden:

- Shared GPU Resources
- Direct Surface Sampling
- Hardware Overlays
- Direct Scanout
- Gemeinsame Buffer-Referenzen

Zero-Copy DARF nur verwendet werden, wenn Speicherzugriff, Ressourcenlebensdauer und Synchronisation korrekt gewährleistet sind.

## 11. Composition Scheduling

Die Komposition erfolgt bedarfsgesteuert.

Nova Frame Scheduling bestimmt, wann ein neuer Ausgabezustand benötigt wird.

Dabei gilt:

- Mehrere Änderungen DÜRFEN zusammengefasst werden.
- Unveränderte Ausgaben SOLLEN keine neue Komposition auslösen.
- Animationen erzeugen Aktualisierungen entsprechend ihrem tatsächlichen Fortschritt.
- Synchronisation mit dem Display Server MUSS gewährleistet sein.

Der Compositor benötigt keinen permanenten eigenen Rendering-Takt.

## 12. Multi-Display Support

Nova Compositor MUSS mehrere Bildschirmausgaben unterstützen können.

Jede Ausgabe kann besitzen:

- Eigenen Composition State
- Eigene Damage Regions
- Eigene Skalierung
- Eigene Farb- und Ausgabeparameter
- Eigenen Frame-Zeitplan

Gemeinsam verwendete Surfaces DÜRFEN ihre Rendering-Ergebnisse ausgabeseitig wiederverwenden, sofern diese kompatibel sind.

## 13. GPU und Software Fallback

GPU-beschleunigte Komposition SOLL bevorzugt werden.

Bei fehlender GPU-Unterstützung MUSS eine softwarebasierte Komposition möglich sein.

Der Fallback MUSS mindestens unterstützen:

- Surface Composition
- Z-Order
- Clipping
- Transparenz
- Grundlegende Transformationen

Komplexe Materialeffekte DÜRFEN entsprechend definierter Qualitätsstufen vereinfacht werden.

Die visuelle und funktionale Korrektheit MUSS erhalten bleiben.

## 14. Sicherheit und Isolation

Der Compositor MUSS Surface-Zugriffe kontrollieren.

Dabei gilt:

- Anwendungen DÜRFEN nicht unberechtigt auf fremde Surface-Inhalte zugreifen.
- Buffer-Referenzen MÜSSEN validiert werden.
- Ressourcenlebenszeiten MÜSSEN berücksichtigt werden.
- GPU-Synchronisation MUSS eingehalten werden.
- Fehlerhafte Anwendungen DÜRFEN die Integrität anderer Surfaces nicht beeinträchtigen.

Der Compositor SOLL als isolierter Systemdienst im Userspace betrieben werden können.

## 15. Verbindliche Anforderungen

1. NovaOS MUSS einen zentralen Compositor bereitstellen.
2. Der Compositor MUSS mehrere Surfaces zusammensetzen können.
3. Die Komposition MUSS zustandsbasiert erfolgen.
4. Damage-Driven Composition MUSS unterstützt werden.
5. Layer-Reihenfolge und Transformationen MÜSSEN berücksichtigt werden.
6. Transparenz und Materialeffekte MÜSSEN integrierbar sein.
7. Occlusion Culling SOLL unnötige Verarbeitung vermeiden.
8. Gültige Composition-Ergebnisse SOLLEN wiederverwendet werden.
9. Zero-Copy Composition SOLL unterstützt werden.
10. GPU-Beschleunigung SOLL bevorzugt werden.
11. Ein Software-Fallback MUSS verfügbar sein.
12. Mehrere Bildschirmausgaben MÜSSEN unterstützt werden.
13. Die Komposition MUSS mit Nova Frame Scheduling zusammenarbeiten.
14. Surface-Zugriffe MÜSSEN kontrolliert und synchronisiert erfolgen.
15. Der Compositor MUSS unabhängig von einer bestimmten GPU-API bleiben.
16. Ein deterministischer Kompositionsmodus MUSS unterstützt werden können.

## 16. Ergebnis

Nova Compositor bildet die zentrale Kompositionsebene der NovaOS-Grafikarchitektur.

Durch retained Composition, Damage Tracking, Occlusion Culling, Zero-Copy und GPU-Beschleunigung werden grafische Oberflächen effizient zu vollständigen Ausgaben zusammengesetzt.

Damit ermöglicht NovaOS eine flüssige, ressourcenschonende und materialbasierte Desktop-Oberfläche, ohne unveränderte Bildbereiche permanent neu berechnen zu müssen.