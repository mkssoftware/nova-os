# NPSPEC-COMPOSITOR-SURFACE-0003 – Nova Surface Hierarchy

**Status:** Angenommen  
**Bereich:** Compositor / Surface-Hierarchie  
**System:** NovaOS

## 1. Zweck

Nova Surface Hierarchy definiert die hierarchische Organisation grafischer Oberflächen innerhalb des Nova Compositors.

Ziel ist die effiziente Verwaltung zusammengehöriger Surfaces einschließlich ihrer Positionierung, Transformationen, Sichtbarkeit und Darstellung.

## 2. Architektur

- Hierarchische Surface-Struktur als zyklenfreier Baum.
- Unterstützung mehrerer unabhängiger Surface-Bäume.
- Parent-Child-Beziehungen zwischen Surfaces.
- Trennung von Surface-Hierarchie und globaler Fensterverwaltung.
- Relative Positionierung innerhalb übergeordneter Surfaces.
- Vererbung geometrischer Transformationen.
- Atomare Aktualisierung hierarchischer Änderungen.
- Integration in Nova Surface Management und Scene Manager.
- Capability-basierte Kontrolle hierarchischer Operationen.

## 3. Kernfunktionen

- Erstellung und Verwaltung von Child-Surfaces.
- Dynamisches Hinzufügen und Entfernen von Surfaces.
- Änderung übergeordneter Surface-Zuordnungen.
- Relative Positionierung und Transformation.
- Verwaltung der lokalen Ebenenreihenfolge.
- Unterstützung verschachtelter Oberflächen.
- Berechnung effektiver Sichtbarkeit.
- Hierarchisches Clipping.
- Weitergabe von Damage-Regionen.
- Optimierung vollständig verdeckter Teilbäume.
- Atomare Änderungen zusammengehöriger Surface-Strukturen.

## 4. Normative Anforderungen

- SURFACE-001: Surface-Hierarchien MÜSSEN zyklenfrei sein.
- SURFACE-002: Jede Surface DARF höchstens eine direkte Parent-Surface besitzen.
- SURFACE-003: Surfaces ohne Parent MÜSSEN als Root-Surfaces behandelt werden.
- SURFACE-004: Child-Surfaces MÜSSEN relativ zu ihrer Parent-Surface positioniert werden können.
- SURFACE-005: Transformationen übergeordneter Surfaces MÜSSEN bei der Berechnung der endgültigen Geometrie berücksichtigt werden.
- SURFACE-006: Parent-Child-Beziehungen MÜSSEN atomar geändert werden können.
- SURFACE-007: Hierarchieänderungen MÜSSEN vor ihrer Übernahme validiert werden.
- SURFACE-008: Unberechtigte Änderungen fremder Surface-Hierarchien MÜSSEN verhindert werden.
- SURFACE-009: Die lokale Ebenenreihenfolge von Child-Surfaces MUSS eindeutig definiert sein.
- SURFACE-010: Die globale Ebenenreihenfolge MUSS weiterhin durch die zuständige Szenen- und Fensterverwaltung kontrolliert werden.
- SURFACE-011: Child-Surfaces MÜSSEN unabhängig von ihrer Parent-Surface eigene Grafikbuffer besitzen können.
- SURFACE-012: Child-Surfaces DÜRFEN nicht zwingend auf die geometrischen Grenzen ihrer Parent-Surface beschränkt sein.
- SURFACE-013: Clipping-Regeln MÜSSEN explizit definiert und konsistent angewendet werden.
- SURFACE-014: Änderungen übergeordneter Surfaces MÜSSEN bei betroffenen Child-Surfaces berücksichtigt werden.
- SURFACE-015: Vollständig unsichtbare Teilbäume SOLLEN von unnötigen Kompositionsoperationen ausgeschlossen werden.
- SURFACE-016: Die maximale Hierarchietiefe und Anzahl von Child-Surfaces MÜSSEN begrenzbar sein.
- SURFACE-017: Die Zerstörung einer Parent-Surface MUSS ihre Child-Surfaces nach einer definierten Lebenszyklusrichtlinie behandeln.
- SURFACE-018: Ungültige Hierarchieoperationen MÜSSEN kontrolliert zurückgewiesen werden.

## 5. Hierarchiemodell

### Root Surface
Oberste Surface eines unabhängigen Surface-Baums.

### Parent Surface
Surface mit einer oder mehreren untergeordneten Surfaces.

### Child Surface
Untergeordnete Surface mit eigener Geometrie, eigenem Zustand und optionalem Grafikbuffer.

### Surface Subtree
Zusammengehöriger Teilbaum aus einer Surface und ihren untergeordneten Surfaces.

### Local Stacking
Definiert die Darstellungsreihenfolge innerhalb einer Parent-Child-Struktur.

Eine Surface DARF gleichzeitig Parent und Child sein.

## 6. Hierarchische Eigenschaften

### Position und Transformation
Child-Surfaces verwenden lokale Koordinaten. Ihre endgültige Transformation ergibt sich aus der Verkettung mit den Transformationen ihrer Vorfahren.

### Sichtbarkeit
Die effektive Sichtbarkeit berücksichtigt den eigenen Zustand sowie relevante Zustände übergeordneter Surfaces.

### Transparenz
Transparenz MUSS entsprechend den festgelegten Kompositionsregeln verarbeitet werden. Eine automatische Gruppen-Deckkraft DARF nicht ohne explizite Definition angenommen werden.

### Clipping
Clipping-Bereiche können lokal definiert und entsprechend der Hierarchie transformiert werden.

### Damage Tracking
Änderungen untergeordneter Surfaces werden in die entsprechenden Koordinatenräume der Kompositionspipeline übertragen.

## 7. Hierarchieoperationen

1. **Create:** Surface erstellen.
2. **Attach:** Optional einer Parent-Surface zuordnen.
3. **Configure:** Position, Transformation und Eigenschaften festlegen.
4. **Validate:** Berechtigungen und Hierarchiekonsistenz prüfen.
5. **Commit:** Änderungen atomar übernehmen.
6. **Update:** Betroffene Transformationen und Sichtbarkeiten aktualisieren.
7. **Compose:** Sichtbare Surface-Teilbäume verarbeiten.
8. **Detach:** Beziehungen bei Bedarf kontrolliert auflösen.

## 8. Performance und Zuverlässigkeit

- Effiziente Traversierung hierarchischer Surface-Strukturen.
- Zwischenspeicherung berechneter Transformationen.
- Aktualisierung ausschließlich betroffener Teilbäume.
- Vermeidung unnötiger vollständiger Hierarchiedurchläufe.
- Optimierung durch hierarchisches Occlusion Culling.
- Weitergabe von Damage-Regionen ohne unnötige Neuberechnung.
- Schutz vor zyklischen Referenzen.
- Begrenzung übermäßig tiefer Hierarchien.
- Kontrollierte Bereinigung verwaister Child-Surfaces.
- Konsistente Wiederherstellung nach Compositor-Fehlern.

## 9. Ergebnis

NovaOS erhält eine flexible und effiziente Surface-Hierarchie.

Grafische Oberflächen können unabhängig voneinander organisiert, verschachtelt und transformiert werden.

Die Architektur ermöglicht komplexe Fensterinhalte, Popups, Overlays und zusammengesetzte Benutzeroberflächen, ohne die globale Fensterverwaltung mit der internen Surface-Struktur zu vermischen.