# NPSPEC-COMPOSITOR-SURFACE-0001 – Nova Surface Management

**Status:** Angenommen  
**Bereich:** Compositor / Surface-Verwaltung  
**System:** NovaOS

## 1. Zweck

Nova Surface Management definiert die Verwaltung grafischer Oberflächen innerhalb des Nova Compositors.

Ziel ist eine sichere, effiziente und ressourcenschonende Bereitstellung von Surfaces als Grundlage für Fenster, Bedienelemente, Animationen und weitere grafische Inhalte.

## 2. Architektur

- Zentraler Surface Manager innerhalb des Nova Compositors.
- Trennung zwischen Surface, Grafikbuffer und Fenster.
- Verwaltung über eindeutige Surface-Handles.
- Kommunikation über das Nova Display Protocol.
- Capability-basierte Zugriffssteuerung.
- Unterstützung hierarchischer Surface-Strukturen.
- Unabhängigkeit vom verwendeten Rendering-Backend.
- Integration in Nova Compositor Pipeline und Lifecycle.
- Unterstützung von Zero-Copy und GPU-basierten Buffern.

## 3. Kernfunktionen

- Erstellen und Freigeben grafischer Surfaces.
- Zuordnung von Surfaces zu Clients.
- Verwaltung von Position, Größe und Transformation.
- Unterstützung von Parent- und Child-Surfaces.
- Verwaltung von Sichtbarkeit und Transparenz.
- Zuordnung und Austausch von Grafikbuffern.
- Verarbeitung von Surface-Änderungen.
- Verwaltung von Clipping- und Damage-Regionen.
- Synchronisierung von Buffer-Zugriffen.
- Atomare Übernahme zusammengehöriger Änderungen.
- Überwachung des Surface-Lebenszyklus.
- Wiederherstellung nach Ressourcenverlust.

## 4. Normative Anforderungen

- SURFACE-001: Jede Surface MUSS eine eindeutige, gegen veraltete Referenzen geschützte Identität besitzen.
- SURFACE-002: Surfaces MÜSSEN unabhängig von Fenstern existieren können.
- SURFACE-003: Erstellung und Manipulation MÜSSEN durch Capabilities kontrolliert werden.
- SURFACE-004: Clients DÜRFEN fremde Surfaces nicht ohne Berechtigung verändern.
- SURFACE-005: Surface-Eigenschaften MÜSSEN atomar aktualisiert werden können.
- SURFACE-006: Änderungen MÜSSEN über definierte Commit-Operationen übernommen werden.
- SURFACE-007: Parent-Child-Beziehungen MÜSSEN eindeutig und zyklenfrei sein.
- SURFACE-008: Grafikbuffer MÜSSEN unabhängig vom Surface-Lebenszyklus verwaltet werden.
- SURFACE-009: Buffer DÜRFEN während aktiver Nutzung nicht vorzeitig freigegeben werden.
- SURFACE-010: Surface-Änderungen MÜSSEN an die Compositor Pipeline übermittelt werden.
- SURFACE-011: Damage-Regionen SOLLEN zur Reduzierung unnötiger Rendering-Operationen verwendet werden.
- SURFACE-012: Unsichtbare Surfaces SOLLEN keine unnötige Bildkomposition verursachen.
- SURFACE-013: Surface-Ressourcen MÜSSEN pro Client begrenzbar sein.
- SURFACE-014: Ungültige Surface-Operationen MÜSSEN kontrolliert zurückgewiesen werden.
- SURFACE-015: Beim Beenden eines Clients MÜSSEN dessen Ressourcen kontrolliert bereinigt werden.
- SURFACE-016: Verlorene Grafikressourcen MÜSSEN erkannt und gegebenenfalls neu angefordert werden.
- SURFACE-017: Surface-Zustände MÜSSEN mit dem Nova Compositor Lifecycle kompatibel sein.
- SURFACE-018: Die Surface-Verwaltung MUSS ohne KI-Abhängigkeit funktionieren.

## 5. Surface-Modell

Jede Surface besitzt folgende grundlegende Eigenschaften:

| Eigenschaft | Beschreibung |
|---|---|
| Surface ID | Eindeutige Identität |
| Owner | Zugehöriger Client |
| Parent | Optionale übergeordnete Surface |
| Geometry | Größe und lokale Position |
| Transform | Skalierung, Rotation und Transformation |
| Buffer | Zugeordneter Grafikbuffer |
| Opacity | Transparenzwert |
| Visibility | Sichtbarkeitszustand |
| Damage | Veränderte Bildbereiche |
| Clip | Begrenzung des sichtbaren Inhalts |
| State | Aktueller Lebenszykluszustand |

Die globale Positionierung und Ebenenreihenfolge werden durch Scene Manager und Window Manager entsprechend ihrer Zuständigkeiten bestimmt.

## 6. Surface-Lebenszyklus

- **Created:** Surface wurde angelegt.
- **Configured:** Grundlegende Eigenschaften wurden festgelegt.
- **Attached:** Ein gültiger Grafikbuffer wurde zugeordnet.
- **Committed:** Änderungen wurden zur Verarbeitung übernommen.
- **Active:** Surface ist Bestandteil der aktiven Szene.
- **Hidden:** Surface ist vorübergehend nicht sichtbar.
- **Destroyed:** Surface wurde logisch entfernt; verbleibende Ressourcen werden nach Abschluss laufender Zugriffe freigegeben.

Buffer-Wechsel und Surface-Änderungen DÜRFEN unabhängig voneinander erfolgen.

## 7. Surface-Verarbeitung

1. **Create:** Surface erstellen und Berechtigungen prüfen.
2. **Configure:** Eigenschaften und Beziehungen festlegen.
3. **Attach:** Grafikbuffer zuordnen.
4. **Commit:** Änderungen atomar übernehmen.
5. **Validate:** Surface-Zustand und Ressourcen prüfen.
6. **Update:** Szenenstruktur aktualisieren.
7. **Compose:** Sichtbare Inhalte bei Bedarf komponieren.
8. **Release:** Nicht mehr benötigte Ressourcen freigeben.

## 8. Performance und Zuverlässigkeit

- Zero-Copy-Verarbeitung, sofern möglich.
- Wiederverwendung vorhandener Grafikbuffer.
- Minimierung unnötiger Surface-Aktualisierungen.
- Effiziente Verwaltung hierarchischer Surfaces.
- Unterstützung partieller Bildaktualisierungen.
- Begrenzung des Speicherverbrauchs pro Client.
- Vermeidung blockierender Client-Operationen.
- Kontrollierte Behandlung ungültiger Buffer.
- Asynchrone Ressourcenfreigabe.
- Wiederherstellung gültiger Surface-Zustände nach Compositor-Fehlern.

## 9. Ergebnis

NovaOS erhält eine zentrale, sichere und leistungsfähige Surface-Verwaltung.

Grafische Oberflächen können unabhängig von Fenstern erstellt, hierarchisch organisiert und effizient aktualisiert werden.

Die Architektur bildet die Grundlage für die Szenenverwaltung, Fensterdarstellung, Animationen und hardwarebeschleunigte Bildkomposition innerhalb von NovaOS.