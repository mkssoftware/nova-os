# NPSPEC-GRAPHICS-DRIVER-0002 – Nova Framebuffer Backend

**Status:** Angenommen  
**Bereich:** Graphics / Framebuffer Backend  
**System:** NovaOS

## 1. Zweck

Nova Framebuffer Backend definiert die grundlegende Grafikausgabe über einen linearen Framebuffer.

Ziel ist eine zuverlässige, hardwareunabhängige Bildausgabe ohne vollständigen GPU-Treiber.

Das Backend bildet die Grundlage für Bootgrafik, frühe Desktopdarstellung, Software-Rendering und grafische Wiederherstellungsumgebungen.

## 2. Architektur

Das Framebuffer Backend ist Bestandteil der Nova Graphics Backend Abstraction.

| Komponente | Aufgabe |
|---|---|
| Framebuffer Device | Repräsentation des verfügbaren Framebuffers |
| Framebuffer Driver | Zugriff auf den physischen Bildspeicher |
| Framebuffer Renderer | Softwarebasierte Grafikoperationen |
| Framebuffer Memory | Verwaltung des Bildspeichers |
| Framebuffer Presenter | Übertragung fertiger Bildinhalte |
| Framebuffer Synchronization | Koordination von Schreib- und Ausgabeoperationen |

Das Backend verwendet den durch die Firmware oder einen geeigneten Displaytreiber bereitgestellten Framebuffer.

Es ist unabhängig von Nova UI, Window Manager und Compositor.

## 3. Unterstützte Framebuffer-Quellen

NovaOS unterstützt folgende Quellen:

| Quelle | Beschreibung |
|---|---|
| BIOS VBE | Linearer Framebuffer über VESA BIOS Extensions |
| UEFI GOP | Framebuffer über das Graphics Output Protocol |
| Native Display Driver | Framebuffer eines nativen Displaytreibers |
| Virtual Display | Framebuffer virtualisierter Grafikhardware |

BIOS- und UEFI-Firmwaredienste werden ausschließlich während der dafür zulässigen Bootphase verwendet.

Nach dem Kernel-Handoff erfolgt der Zugriff über die übergebenen Framebufferinformationen und die Speicherverwaltung von NovaOS.

Ein Framebuffer ist nur verwendbar, wenn seine Speicheradresse und sein Format gültig sind.

## 4. Framebuffer-Modell

Jeder Framebuffer besitzt mindestens folgende Eigenschaften:

| Eigenschaft | Beschreibung |
|---|---|
| Framebuffer ID | Eindeutige Identität |
| Physical Address | Physische Speicheradresse |
| Virtual Address | Zugeordnete virtuelle Speicheradresse |
| Width | Breite in Pixeln |
| Height | Höhe in Pixeln |
| Pitch | Bytes pro Bildzeile |
| Bits Per Pixel | Speichergröße eines Pixels |
| Pixel Format | Farbkanalformat |
| Memory Size | Größe des zugeordneten Speicherbereichs |
| Access Mode | Zulässige Speicherzugriffe |
| Device Reference | Zugeordnetes Ausgabegerät |

Der Pitch MUSS unabhängig von der sichtbaren Bildbreite behandelt werden.

Pixeladressen MÜSSEN anhand des tatsächlichen Speicherlayouts berechnet werden.

## 5. Pixelformate

Das Backend unterstützt ein erweiterbares Formatmodell.

Initial vorgesehene Formate:

- RGB565
- RGB888
- BGR888
- XRGB8888
- XBGR8888
- ARGB8888
- ABGR8888

Die tatsächlich unterstützten Formate werden durch die jeweilige Framebufferquelle bestimmt.

Nicht unterstützte Formate MÜSSEN erkannt werden.

Farbkanalreihenfolge, Kanalbreite und Speicherlayout DÜRFEN nicht implizit vorausgesetzt werden.

## 6. Rendering

Das Framebuffer Backend verwendet CPU-basiertes Rendering.

Grundfunktionen:

- Pixel schreiben und lesen, sofern der Speicherzugriff dies unterstützt.
- Rechtecke und Flächen zeichnen.
- Linien und geometrische Formen darstellen.
- Bilder kopieren und skalieren.
- Alpha-Blending.
- Clipping.
- Farbkonvertierung.
- Übertragung vollständiger oder partieller Bildbereiche.

Komplexere Grafikoperationen werden durch den Nova Software Renderer bereitgestellt.

Das Backend MUSS keine hardwarebeschleunigte 3D-Grafik implementieren.

## 7. Buffer-Verwaltung

NovaOS unterscheidet:

| Verfahren | Beschreibung |
|---|---|
| Direct Rendering | Direkte Schreibzugriffe auf den sichtbaren Framebuffer |
| Single Buffer | Rendering unmittelbar in einen Ausgabebuffer |
| Double Buffer | Rendering in einen separaten Backbuffer |
| Multi Buffer | Verwendung mehrerer Rendering-Buffer |

Double Buffering SOLL für die reguläre Desktopdarstellung verwendet werden.

Die Verwendung zusätzlicher Buffer richtet sich nach verfügbarem Speicher und Leistungsanforderungen.

Der sichtbare Framebuffer DARF nicht ohne erforderliche Synchronisation gleichzeitig durch mehrere Komponenten beschrieben werden.

## 8. Bildausgabe

Die Ausgabe erfolgt über den Framebuffer Presenter.

Ablauf:

1. Rendering-Ziel bestimmen.
2. Grafikoperationen ausführen.
3. Geänderte Bildbereiche ermitteln.
4. Erforderliche Farbkonvertierungen durchführen.
5. Bilddaten in den sichtbaren Framebuffer übertragen.
6. Abschluss der Ausgabe signalisieren.

Dirty-Region-Updates SOLLEN unterstützt werden.

Unveränderte Bildbereiche SOLLEN nicht unnötig übertragen werden.

Falls keine zuverlässige VSync-Schnittstelle verfügbar ist, DARF das Backend keine synchronisierte Bildausgabe garantieren.

## 9. Speicherverwaltung

Der Framebuffer wird über den Nova Memory Manager eingebunden.

Dabei gilt:

- Physische Speicherbereiche MÜSSEN eindeutig reserviert sein.
- Virtuelle Mappings MÜSSEN gültige Speicherattribute verwenden.
- Schreibzugriffe MÜSSEN auf den zulässigen Framebufferbereich begrenzt werden.
- Speicherbereiche DÜRFEN nicht versehentlich als gewöhnlicher RAM verwaltet werden.
- Buffergrößen MÜSSEN unter Berücksichtigung von Pitch und Höhe berechnet werden.
- Speicherüberläufe MÜSSEN verhindert werden.

Geeignete Speicherattribute SOLLEN die Schreibgeschwindigkeit optimieren.

## 10. Initialisierung

Die Initialisierung erfolgt mehrstufig.

### Early Boot

- Framebufferinformationen vom Bootloader übernehmen.
- Speicherbereich validieren.
- Grundlegende Pixelausgabe aktivieren.
- Bootgrafik und Systemmeldungen darstellen.

### Kernel Graphics

- Framebuffer in den virtuellen Adressraum einbinden.
- Framebuffer Device registrieren.
- Graphics Backend initialisieren.
- Software-Rendering bereitstellen.

### Desktop Graphics

- Backbuffer bereitstellen.
- Compositor anbinden.
- Desktopdarstellung aktivieren.
- Optional auf ein hardwarebeschleunigtes Backend wechseln.

Der Desktop MUSS nicht auf die Initialisierung eines nativen GPU-Treibers warten.

## 11. Integration in NovaOS

### Bootloader

Übergibt die verfügbaren Framebufferinformationen an den Kernel.

### Graphics Core

Registriert das Framebuffer Backend als verfügbare Rendering-Implementierung.

### Software Renderer

Verwendet den Framebuffer oder einen separaten Backbuffer als Rendering-Ziel.

### Nova Compositor

Übergibt fertige Bildinhalte an den Framebuffer Presenter.

### Display Subsystem

Verwaltet die logische Zuordnung zum Ausgabegerät.

Änderungen von Auflösung oder Bildwiederholfrequenz sind nicht Aufgabe des generischen Framebuffer Backends.

## 12. Normative Anforderungen

- GRAPHICS-001: NovaOS MUSS einen linearen Framebuffer als grundlegenden Grafik-Ausgabepfad unterstützen.
- GRAPHICS-002: Das Backend MUSS ohne vollständigen GPU-Treiber funktionieren.
- GRAPHICS-003: BIOS/VBE- und UEFI/GOP-Framebuffer MÜSSEN unterstützt werden können.
- GRAPHICS-004: Framebufferinformationen MÜSSEN vor der Verwendung validiert werden.
- GRAPHICS-005: Pitch und Pixelformat MÜSSEN explizit berücksichtigt werden.
- GRAPHICS-006: Framebufferzugriffe MÜSSEN auf gültige Speicherbereiche beschränkt sein.
- GRAPHICS-007: Das Backend MUSS grundlegende Software-Rendering-Ausgaben ermöglichen.
- GRAPHICS-008: Double Buffering MUSS unterstützt werden.
- GRAPHICS-009: Partielle Bildaktualisierungen SOLLEN unterstützt werden.
- GRAPHICS-010: Framebuffer und Rendering-Buffer MÜSSEN getrennt verwaltet werden können.
- GRAPHICS-011: Das Backend DARF keine GPU-Beschleunigung voraussetzen.
- GRAPHICS-012: Die Initialisierung MUSS bereits vor dem vollständigen Desktopstart möglich sein.
- GRAPHICS-013: Ein späterer Wechsel auf ein anderes Rendering-Backend MUSS unterstützt werden.
- GRAPHICS-014: Framebufferressourcen MÜSSEN kontrolliert verwaltet werden.
- GRAPHICS-015: Nicht unterstützte Pixelformate MÜSSEN eindeutig zurückgewiesen werden.
- GRAPHICS-016: Kritische Systemausgaben SOLLEN bei Ausfall des GPU-Renderers weiterhin möglich sein, sofern ein gültiger Framebuffer verfügbar bleibt.
- GRAPHICS-017: Die Framebuffer-Ausgabe MUSS ohne Netzwerk- und KI-Abhängigkeit funktionieren.

## 13. Fehlerbehandlung und Wiederherstellung

- Ungültige Framebufferadressen werden zurückgewiesen.
- Nicht unterstützte Pixelformate verhindern die Aktivierung des betroffenen Backends.
- Speicherfehler werden an den Graphics Core gemeldet.
- Fehlgeschlagene Bufferzuweisungen führen zu einer ressourcenschonenderen Ausgabekonfiguration.
- Bei Ausfall eines GPU-Backends SOLL der Framebuffer als Fallback verwendet werden.
- Ein Backendwechsel DARF den Framebuffer nicht unkontrolliert invalidieren.
- Bei Verlust des zugrunde liegenden Displays MUSS das Backend deaktiviert oder neu initialisiert werden.
- Ein nicht mehr gültiger Firmware-Framebuffer DARF nicht weiter beschrieben werden.

## 14. Ergebnis

NovaOS erhält ein einfaches, zuverlässiges und hardwareunabhängiges Framebuffer Backend.

Es ermöglicht grafische Ausgaben bereits während des Bootvorgangs und stellt die Grundlage für einen frühzeitig verfügbaren Desktop bereit.

Durch Software-Rendering, Buffer-Verwaltung und definierte Fallback-Mechanismen bleibt die grafische Bedienbarkeit auch ohne vollständige GPU-Beschleunigung erhalten.

**Grundsatz:** Die grafische Grundfunktionalität von NovaOS darf nicht von der Verfügbarkeit eines hardwarebeschleunigten Grafiktreibers abhängen.