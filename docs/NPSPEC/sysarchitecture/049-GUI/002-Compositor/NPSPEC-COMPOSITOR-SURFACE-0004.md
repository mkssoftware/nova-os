# NPSPEC-COMPOSITOR-SURFACE-0004 – Nova Surface Synchronization

**Status:** Angenommen  
**Bereich:** Compositor / Surface-Synchronisation  
**System:** NovaOS

## 1. Zweck

Nova Surface Synchronization definiert die Synchronisation von Surface-Zuständen, Grafikbuffern und Rendering-Operationen zwischen Anwendungen, Compositor und Grafikhardware.

Ziel ist eine konsistente, latenzarme und fehlerfreie Bildkomposition ohne unnötige Blockierungen oder Speicherkopien.

## 2. Architektur

- Integration in Nova Surface Management und Nova Compositor Pipeline.
- Trennung zwischen Zustands-, Buffer- und Präsentationssynchronisation.
- Unterstützung expliziter und impliziter Synchronisation.
- Fence-basierte Koordination von CPU- und GPU-Zugriffen.
- Asynchrone Verarbeitung ohne globale Rendering-Sperren.
- Atomare Surface-Commits.
- Unterstützung synchronisierter Parent-Child-Surfaces.
- Integration in Nova Compositor Scheduling.
- Hardwareunabhängige Synchronisationsschnittstellen.

## 3. Kernfunktionen

- Synchronisation von Surface-Zustandsänderungen.
- Koordination von Buffer-Produzenten und -Konsumenten.
- Unterstützung von Acquire- und Release-Fences.
- Verwaltung ausstehender GPU-Operationen.
- Atomare Übernahme zusammengehöriger Surface-Änderungen.
- Synchronisierte Aktualisierung hierarchischer Surfaces.
- Vermeidung gleichzeitiger inkompatibler Buffer-Zugriffe.
- Bereitstellung von Frame-Callbacks.
- Rückmeldung über abgeschlossene Buffer-Nutzung.
- Erkennung blockierter Synchronisationsoperationen.
- Unterstützung von Zero-Copy-Buffer-Übergaben.

## 4. Normative Anforderungen

- SURFACE-001: Surface-Zustandsänderungen MÜSSEN atomar übernommen werden.
- SURFACE-002: Pending State und Committed State MÜSSEN getrennt verwaltet werden.
- SURFACE-003: Grafikbuffer DÜRFEN nicht gleichzeitig inkompatiblen Lese- und Schreibzugriffen ausgesetzt werden.
- SURFACE-004: Explizite Synchronisation MUSS über definierte Fence-Mechanismen unterstützt werden.
- SURFACE-005: Acquire-Fences MÜSSEN vor dem Lesen betroffener Buffer berücksichtigt werden.
- SURFACE-006: Release-Fences MÜSSEN die abgeschlossene Nutzung eines Buffers zuverlässig signalisieren.
- SURFACE-007: Buffer DÜRFEN erst nach Freigabe durch alle relevanten Nutzer wiederverwendet werden.
- SURFACE-008: Synchronisationsoperationen SOLLEN asynchron erfolgen.
- SURFACE-009: Langsame Clients DÜRFEN die Verarbeitung unabhängiger Surfaces nicht blockieren.
- SURFACE-010: Zusammengehörige Parent-Child-Änderungen MÜSSEN synchronisiert übernommen werden können.
- SURFACE-011: Nicht synchronisierte Child-Surfaces MÜSSEN unabhängig aktualisiert werden können.
- SURFACE-012: Synchronisationsabhängigkeiten MÜSSEN auf Deadlocks überprüfbar sein.
- SURFACE-013: Blockierte Synchronisationsoperationen MÜSSEN durch Timeouts oder Fehlerüberwachung erkannt werden.
- SURFACE-014: Fehlerhafte Fences MÜSSEN kontrolliert behandelt werden.
- SURFACE-015: Frame-Callbacks DÜRFEN nicht mit der tatsächlichen Präsentationsbestätigung gleichgesetzt werden.
- SURFACE-016: Präsentationsfeedback MUSS den tatsächlichen Präsentationsstatus abbilden können.
- SURFACE-017: Synchronisationsressourcen MÜSSEN pro Client begrenzbar sein.
- SURFACE-018: Synchronisationsmechanismen DÜRFEN Capability- und Speicherisolationsgrenzen nicht umgehen.

## 5. Synchronisationsmodelle

### State Synchronization
Atomare Übernahme zusammengehöriger Surface-Eigenschaften durch Commit-Operationen.

### Buffer Synchronization
Koordination von Buffer-Zugriffen über Acquire- und Release-Fences.

### Hierarchical Synchronization
Gemeinsame Übernahme von Änderungen innerhalb synchronisierter Surface-Hierarchien.

### Frame Synchronization
Abstimmung der Surface-Aktualisierung mit dem Nova Compositor Scheduling.

### Presentation Synchronization
Rückmeldung über die tatsächliche Darstellung oder das Verwerfen eines Frames.

## 6. Buffer-Synchronisation

1. **Render:** Client erzeugt neue Bildinhalte.
2. **Signal:** Client stellt einen Acquire-Fence bereit.
3. **Attach:** Buffer wird einer Surface zugeordnet.
4. **Commit:** Surface-Änderungen werden übernommen.
5. **Wait:** Compositor berücksichtigt den Acquire-Fence.
6. **Compose:** Buffer wird für die Bildkomposition verwendet.
7. **Release:** Nach Abschluss aller relevanten Zugriffe wird die Freigabe signalisiert.
8. **Reuse:** Client darf den Buffer erneut verwenden.

Bei Direct Scanout MUSS die Freigabe auch die tatsächliche Nutzung durch die Display-Hardware berücksichtigen.

## 7. Hierarchische Synchronisation

- Child-Surfaces können synchronisiert oder unabhängig betrieben werden.
- Synchronisierte Child-Commits werden bis zur Freigabe durch den übergeordneten Commit zurückgehalten.
- Zusammengehörige Änderungen werden atomar in die Szenenstruktur übernommen.
- Unabhängige Child-Surfaces dürfen eigene Commit-Zyklen besitzen.
- Hierarchische Synchronisation DARF keine unbegrenzten Warteketten erzeugen.
- Änderungen der Parent-Child-Beziehungen MÜSSEN bestehende Synchronisationsabhängigkeiten berücksichtigen.

## 8. Performance und Fehlerbehandlung

- Bevorzugung nicht blockierender Synchronisationsverfahren.
- Vermeidung unnötiger CPU-GPU-Warteoperationen.
- Minimierung globaler Sperren.
- Unterstützung paralleler Surface-Aktualisierungen.
- Wiederverwendung geeigneter Synchronisationsobjekte.
- Begrenzung ausstehender Commits und Fences.
- Erkennung von Deadlocks und Synchronisations-Timeouts.
- Kontrollierte Behandlung verlorener GPU-Kontexte.
- Bereinigung verwaister Synchronisationsressourcen.
- Integration in Nova Compositor Fault Recovery.

## 9. Ergebnis

NovaOS erhält eine sichere und effiziente Synchronisationsarchitektur für grafische Oberflächen.

Surface-Zustände und Grafikbuffer können unabhängig verarbeitet und zum richtigen Zeitpunkt konsistent zusammengeführt werden.

Fence-basierte Synchronisation, atomare Commits und nicht blockierende Verarbeitung ermöglichen eine latenzarme Bildkomposition ohne Datenrennen, beschädigte Frames oder unnötige Wartezeiten.