# NPSPEC-COMPOSITOR-BUFFER-0004 – Nova Buffer Synchronization

**Status:** Angenommen  
**Bereich:** Compositor / Buffer-Synchronisation  
**System:** NovaOS

## 1. Zweck

Nova Buffer Synchronization definiert die Synchronisation grafischer Speicherbuffer zwischen Anwendungen, Compositor, GPU und Display-Hardware.

Ziel ist eine sichere, asynchrone und latenzarme Buffer-Nutzung ohne Datenrennen, beschädigte Frames oder unnötige Wartezeiten.

## 2. Architektur

- Integration in Nova Graphics Buffer Management.
- Abstimmung mit Nova Surface Synchronization.
- Hardwareunabhängige Synchronisationsschnittstelle.
- Unterstützung expliziter und impliziter Synchronisation.
- Fence-basierte Koordination von CPU-, GPU- und Display-Zugriffen.
- Unterstützung von Binary Fences und Timeline-Synchronisation.
- Asynchrone Verarbeitung ohne globale GPU-Sperren.
- Integration in Nova Compositor Pipeline und Scheduling.
- Unterstützung von Zero-Copy und Direct Scanout.

## 3. Kernfunktionen

- Synchronisation paralleler Buffer-Zugriffe.
- Verwaltung von Acquire- und Release-Fences.
- Koordination von Rendering, Komposition und Präsentation.
- Unterstützung mehrerer Buffer-Produzenten und -Verbraucher.
- Verwaltung von GPU-Completion-Events.
- Synchronisation gemeinsam genutzter Speicherressourcen.
- Behandlung von Cache-Kohärenz und Speicherbarrieren.
- Erkennung blockierter Synchronisationsoperationen.
- Kontrollierte Freigabe nicht mehr verwendeter Buffer.
- Überwachung von Synchronisationslatenzen.

## 4. Normative Anforderungen

- BUFFER-001: Jeder Buffer MUSS einen eindeutig verwalteten Synchronisationszustand besitzen.
- BUFFER-002: Konkurrierende inkompatible Speicherzugriffe MÜSSEN verhindert werden.
- BUFFER-003: Explizite Synchronisation MUSS unterstützt werden.
- BUFFER-004: Acquire-Fences MÜSSEN vor abhängigen Buffer-Zugriffen berücksichtigt werden.
- BUFFER-005: Release-Fences DÜRFEN erst nach Abschluss aller zugehörigen Zugriffe signalisiert werden.
- BUFFER-006: Binary Fences MÜSSEN unterstützt werden.
- BUFFER-007: Timeline-Synchronisation SOLL unterstützt werden.
- BUFFER-008: Synchronisationsoperationen SOLLEN asynchron erfolgen.
- BUFFER-009: Unabhängige Buffer DÜRFEN sich nicht unnötig gegenseitig blockieren.
- BUFFER-010: CPU- und GPU-Zugriffe MÜSSEN erforderliche Speicherbarrieren und Cache-Kohärenz berücksichtigen.
- BUFFER-011: Buffer DÜRFEN vor Abschluss aktiver Zugriffe nicht wiederverwendet werden.
- BUFFER-012: Direct Scanout MUSS die tatsächliche Nutzungsdauer durch die Display-Hardware berücksichtigen.
- BUFFER-013: Synchronisationsobjekte MÜSSEN gegen unberechtigte Zugriffe geschützt werden.
- BUFFER-014: Fehlerhafte oder ungültige Fences MÜSSEN kontrolliert behandelt werden.
- BUFFER-015: Blockierte Synchronisationsoperationen MÜSSEN erkannt werden können.
- BUFFER-016: Synchronisationsressourcen MÜSSEN pro Client begrenzbar sein.
- BUFFER-017: Bei GPU-Fehlern MÜSSEN betroffene Synchronisationsobjekte kontrolliert invalidiert werden.
- BUFFER-018: Synchronisationsfehler DÜRFEN keine unkontrollierte Freigabe aktiver Speicherressourcen verursachen.

## 5. Synchronisationsmechanismen

### Acquire Fence
Signalisiert, wann ein Buffer für einen nachfolgenden Verbraucher verwendet werden darf.

### Release Fence
Signalisiert, wann ein Verbraucher seine Zugriffe abgeschlossen hat.

### Binary Fence
Repräsentiert ein einzelnes Synchronisationsereignis.

### Timeline Fence
Ermöglicht die Synchronisation mehrerer Operationen über monoton steigende Fortschrittswerte.

### Memory Barrier
Stellt die erforderliche Speicherreihenfolge und Sichtbarkeit zwischen beteiligten Verarbeitungseinheiten sicher.

### Buffer Reservation
Verwaltet ausstehende Zugriffsabhängigkeiten einer gemeinsam genutzten Speicherressource.

## 6. Buffer-Synchronisationsablauf

1. **Acquire:** Buffer und erforderliche Zugriffsrechte anfordern.
2. **Validate:** Synchronisationszustand und Abhängigkeiten prüfen.
3. **Wait:** Erforderliche Fences asynchron berücksichtigen.
4. **Access:** Rendering-, Kompositions- oder Scanout-Operation durchführen.
5. **Barrier:** Erforderliche Speicher- und Cache-Synchronisation ausführen.
6. **Signal:** Abschluss der Operation signalisieren.
7. **Release:** Buffer-Nutzung durch den Verbraucher beenden.
8. **Reuse:** Buffer nach Abschluss aller relevanten Zugriffe freigeben.

## 7. Fehlerbehandlung

- Erkennung ungültiger Synchronisationsobjekte.
- Überwachung überfälliger Fence-Signale.
- Erkennung zyklischer Synchronisationsabhängigkeiten.
- Kontrollierte Behandlung von GPU-Timeouts.
- Invalidierung betroffener Ressourcen nach GPU-Resets.
- Schutz vor vorzeitiger Buffer-Wiederverwendung.
- Bereinigung verwaister Synchronisationsobjekte.
- Eskalation nicht behebbarer Fehler an Nova Compositor Fault Recovery.

## 8. Performance und Zuverlässigkeit

- Bevorzugung GPU-seitiger gegenüber blockierenden CPU-Warteoperationen.
- Minimierung unnötiger Synchronisationspunkte.
- Vermeidung globaler GPU-Stalls.
- Parallele Verarbeitung unabhängiger Buffer.
- Wiederverwendung geeigneter Synchronisationsressourcen.
- Unterstützung effizienter Timeline-Fences.
- Begrenzung ausstehender Synchronisationsoperationen.
- Minimierung von CPU-GPU-Roundtrips.
- Erfassung von Wartezeiten, Timeouts und Synchronisationsengpässen.
- Deterministischer Fallback bei fehlender Hardwareunterstützung.

## 9. Ergebnis

NovaOS erhält eine zentrale und hardwareunabhängige Buffer-Synchronisation.

Grafikbuffer können sicher zwischen Anwendungen, Compositor und Display-Hardware ausgetauscht werden.

Durch explizite Fences, asynchrone Verarbeitung und kontrollierte Speicherzugriffe werden Datenrennen, unnötige Blockierungen und Darstellungsfehler vermieden.