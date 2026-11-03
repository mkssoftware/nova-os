# NPSPEC-DISPLAY-ARCH-0001 – Nova Display Architecture

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Display Server

## 1. Zweck

Nova Display Architecture definiert die Architektur des Nova Display Server zur Verwaltung grafischer Ausgaben, Display-Surfaces und der Frame-Präsentation.

Ziel ist eine hardwareunabhängige, effiziente und sichere Schnittstelle zwischen Anwendungen, Compositor und physischer Bildschirmausgabe.

Der Display Server verwaltet die Ausgabe grafischer Inhalte, ohne selbst für deren Rendering oder Fensteranordnung verantwortlich zu sein.

## 2. Architekturprinzipien

- **Hardware-Abstraction:** Einheitliche Schnittstellen für unterschiedliche Grafikgeräte.
- **Surface-Based:** Grafische Inhalte werden über kontrollierte Surfaces bereitgestellt.
- **Zero-Copy:** Unnötige Buffer-Kopien werden vermieden.
- **Atomic:** Änderungen des Ausgabezustands erfolgen konsistent.
- **Damage-Aware:** Partielle Aktualisierungen werden unterstützt.
- **Event-Driven:** Ausgabeänderungen und Präsentationen werden ereignisgesteuert verarbeitet.
- **Multi-Display:** Mehrere unabhängige Bildschirmausgaben werden unterstützt.
- **Secure:** Buffer-Zugriffe und Display-Ressourcen unterliegen definierten Berechtigungen.

## 3. Architekturmodell

Der Nova Display Server besteht logisch aus folgenden Komponenten:

| Komponente | Verantwortung |
|---|---|
| Display Manager | Erkennung und Verwaltung von Bildschirmausgaben |
| Output Manager | Auflösung, Bildwiederholrate, Skalierung und Ausgabezustand |
| Surface Manager | Verwaltung grafischer Surfaces |
| Buffer Manager | Import, Freigabe und Lebensdauer grafischer Buffer |
| Display Protocol | Kommunikation mit Anwendungen und Systemdiensten |
| Synchronization Manager | Koordination von Buffer- und Präsentationszugriffen |
| Presentation Manager | Übergabe vollständiger Frames an die Ausgabe |
| Hardware Backend | Ansteuerung der jeweiligen Display-Hardware |

Die Komponenten MÜSSEN über definierte interne Schnittstellen zusammenarbeiten.

## 4. Verantwortlichkeiten

Die Grafikarchitektur trennt folgende Aufgaben:

- **Window Manager:** Fensterposition, Fokus, Größe und Z-Order.
- **Scene Graph:** Logischer Zustand grafischer Elemente.
- **Render Engine:** Berechnung grafischer Inhalte.
- **Compositor:** Zusammensetzung mehrerer Surfaces.
- **Frame Scheduler:** Planung erforderlicher Frames und Präsentationszeitpunkte.
- **Display Server:** Verwaltung von Surfaces, Buffern, Ausgaben und Präsentation.
- **GPU-/Display-Treiber:** Hardwarezugriff und Gerätesteuerung.

Der Display Server DARF keine eigenständigen Entscheidungen über Fensterfokus oder Anwendungslogik treffen.

## 5. Display Output Model

Jede Bildschirmausgabe MUSS als eigenständiges Output-Objekt verwaltet werden können.

Ein Output enthält mindestens:

- **Output ID:** Eindeutige Identifikation.
- **Display Mode:** Auflösung und Bildwiederholrate.
- **Pixel Format:** Unterstütztes Ausgabeformat.
- **Scale Factor:** Logische Skalierung.
- **Orientation:** Ausrichtung.
- **Color Configuration:** Farbraum und Ausgabeparameter.
- **Presentation State:** Aktueller Präsentationszustand.
- **Output Version:** Version der Ausgabekonfiguration.

Änderungen der Ausgabekonfiguration MÜSSEN konsistent veröffentlicht werden.

## 6. Surface und Buffer Management

Der Display Server MUSS grafische Surfaces unabhängig von ihren Buffer-Inhalten verwalten.

Dabei gilt:

1. Eine Surface besitzt eine eindeutige Identität.
2. Buffer werden über kontrollierte Referenzen eingebunden.
3. Surface-Zustandsänderungen werden atomar übernommen.
4. Buffer-Zugriffe werden synchronisiert.
5. Ressourcen bleiben bis zum Abschluss ihrer Verwendung gültig.

Shared Buffers und Zero-Copy SOLLEN unterstützt werden.

Der Display Server MUSS ungültige oder unberechtigte Buffer-Zugriffe verhindern.

## 7. Frame Presentation

Der Display Server MUSS vollständige Frames zur Ausgabe bereitstellen können.

Unterstützt werden SOLLEN:

- VSync-synchronisierte Präsentation
- Asynchrone Präsentationsverfahren, soweit zulässig
- Partial Frame Presentation
- Direct Scanout
- Hardware Overlays
- Presentation Feedback

Die Präsentation MUSS auf einem gültigen und vollständig verfügbaren Buffer-Zustand basieren.

Der Display Server bestimmt die technisch zulässige Präsentationsmethode anhand der Hardwarefähigkeiten.

## 8. Atomic Display State

Änderungen zusammengehöriger Ausgabeparameter MÜSSEN atomar übernommen werden können.

Dies betrifft insbesondere:

- Display Mode
- Scanout Buffer
- Hardware Overlay Planes
- Cursor Plane
- Ausgabegeometrie
- Unterstützte Farbparameter

Eine ungültige Konfiguration DARF nicht teilweise als erfolgreich übernommen gelten.

Falls die Hardware keine atomaren Änderungen unterstützt, MUSS das Backend einen definierten sicheren Aktualisierungspfad bereitstellen.

## 9. Multi-Display Support

NovaOS MUSS mehrere Bildschirmausgaben unabhängig verwalten können.

Dabei gilt:

- Jede Ausgabe besitzt einen eigenen Präsentationszustand.
- Unterschiedliche Auflösungen und Bildwiederholraten werden unterstützt.
- Ausgaben können unabhängig aktiviert oder deaktiviert werden.
- Gemeinsame Surfaces dürfen auf mehreren Ausgaben verwendet werden.
- Änderungen einer Ausgabe sollen andere Ausgaben nicht unnötig blockieren.

Die logische Desktop-Anordnung wird durch die zuständigen Systemkomponenten festgelegt.

## 10. Synchronization

Nova Display Server MUSS die Synchronisation zwischen Anwendungen, Compositor, GPU und Display-Hardware gewährleisten.

Dazu gehören:

- Buffer Ownership
- Acquire- und Release-Synchronisation
- GPU Fences
- Presentation Completion
- Ressourcenlebensdauer

Ein Buffer DARF nicht freigegeben oder unzulässig überschrieben werden, solange er für Rendering oder Scanout benötigt wird.

## 11. Sicherheit und Isolation

Der Display Server SOLL als isolierter Systemdienst im Ring 3 betrieben werden.

Privilegierte Hardwarezugriffe erfolgen über kontrollierte Kernel- und Treiberschnittstellen.

Dabei gilt:

- Anwendungen erhalten ausschließlich autorisierte Surface- und Buffer-Rechte.
- Fremde Buffer dürfen nicht unberechtigt gelesen werden.
- Systemausgaben und privilegierte Display-Funktionen müssen geschützt sein.
- Ungültige Protokollnachrichten müssen sicher abgewiesen werden.
- Fehlerhafte Anwendungen dürfen andere Display-Clients nicht kompromittieren.

## 12. Hardware Backend und Fallback

Nova Display Server MUSS unterschiedliche Hardware-Backends unterstützen können.

Dazu gehören perspektivisch:

- UEFI GOP Framebuffer
- VBE-/Legacy-Framebuffer
- Virtuelle Grafikadapter
- Native GPU- und Display-Treiber

Ein einfacher Framebuffer-Fallback MUSS verfügbar sein, sofern ein nutzbarer Framebuffer bereitsteht.

Nicht unterstützte Hardwarefunktionen DÜRFEN deaktiviert werden, ohne grundlegende Bildschirmausgabe zu verhindern.

## 13. Verbindliche Anforderungen

1. NovaOS MUSS einen zentralen Display Server bereitstellen.
2. Der Display Server MUSS hardwareunabhängige Schnittstellen anbieten.
3. Surfaces und Buffer MÜSSEN getrennt verwaltet werden.
4. Mehrere Bildschirmausgaben MÜSSEN unterstützt werden.
5. Ausgabezustände MÜSSEN konsistent und versioniert sein.
6. Buffer-Zugriffe MÜSSEN synchronisiert und kontrolliert erfolgen.
7. Atomic Surface Commit MUSS unterstützt werden.
8. Atomare Display-Konfigurationen SOLLEN hardwareseitig unterstützt werden.
9. Partial Presentation und Zero-Copy SOLLEN unterstützt werden.
10. Direct Scanout und Hardware Overlays SOLLEN bei geeigneter Hardware nutzbar sein.
11. Presentation Feedback MUSS bereitgestellt werden können.
12. Der Display Server DARF keine Aufgaben des Window Managers übernehmen.
13. Der Display Server SOLL als isolierter Ring-3-Systemdienst ausgeführt werden.
14. Ein einfacher Framebuffer-Fallback MUSS vorgesehen sein.
15. Ein deterministischer Modus MUSS reproduzierbare Zustandsübergänge ermöglichen.

## 14. Ergebnis

Nova Display Architecture bildet die zentrale Ausgabeschicht der NovaOS-Grafikarchitektur.

Durch die Trennung von Rendering, Komposition und Präsentation können grafische Inhalte effizient, synchronisiert und hardwareunabhängig ausgegeben werden.

Damit schafft der Nova Display Server die Grundlage für Multi-Display, Zero-Copy, Partial Presentation und eine sichere, modular erweiterbare Grafikpipeline.