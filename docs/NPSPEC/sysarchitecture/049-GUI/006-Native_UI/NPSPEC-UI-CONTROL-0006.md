# NPSPEC-UI-CONTROL-0006 – Nova Virtualized Controls

**Status:** Angenommen  
**Bereich:** UI / Controls / Virtualization  
**System:** NovaOS

## 1. Zweck

Nova Virtualized Controls definiert die effiziente Darstellung großer Datenmengen innerhalb nativer UI-Steuerelemente.

Ziel ist es, ausschließlich die aktuell benötigten visuellen Elemente zu erzeugen und zu verwalten, ohne für jeden Datensatz eine eigene Control-Instanz erstellen zu müssen.

Die Virtualisierung erfolgt unabhängig von Datenquelle, Layout und Renderbackend.

## 2. Architektur

Das Subsystem umfasst folgende Komponenten:

| Komponente | Aufgabe |
|---|---|
| Virtualization Manager | Koordination der Virtualisierung |
| Viewport Tracker | Ermittlung des sichtbaren Bereichs |
| Item Realization Manager | Erstellung benötigter UI-Elemente |
| Item Recycling Manager | Wiederverwendung vorhandener Control-Instanzen |
| Virtualization Cache | Verwaltung vorbereiteter Elemente |
| Item Position Resolver | Berechnung virtueller Elementpositionen |
| Data Range Provider | Bereitstellung benötigter Datenbereiche |

Die Komponenten verwenden das Nova Control Framework und die Nova Layout Engine.

## 3. Virtualisierungsmodell

NovaOS unterscheidet zwei grundlegende Virtualisierungsarten.

### UI-Virtualisierung

Nur aktuell sichtbare oder unmittelbar benötigte UI-Elemente werden erzeugt.

Nicht sichtbare Elemente werden nicht dauerhaft als vollständige visuelle Objekte vorgehalten.

### Datenvirtualisierung

Nur die tatsächlich benötigten Datenbereiche werden aus der Datenquelle angefordert.

Die Datenquelle DARF größer sein als der verfügbare Arbeitsspeicher.

Beide Verfahren MÜSSEN unabhängig voneinander verwendbar sein.

## 4. Unterstützte Controls

Virtualisierung wird insbesondere für folgende Controls vorgesehen:

- ListBox.
- ListView.
- TreeView.
- DataGrid.
- ItemsControl.
- ComboBox.
- Weitere datenbasierte Custom Controls.

Controls mit großen Elementmengen SOLLEN Virtualisierung standardmäßig verwenden.

Die Virtualisierung DARF für besondere Anwendungsfälle deaktiviert werden.

## 5. Viewport-Verwaltung

Der Viewport beschreibt den aktuell sichtbaren Bereich eines Controls.

Der Viewport Tracker berücksichtigt:

- Scrollposition.
- Sichtbare Breite und Höhe.
- Elementgrößen.
- Skalierung.
- Änderungen des Layouts.
- Sichtbarkeit des Controls.

Aus diesen Informationen wird der benötigte Elementbereich ermittelt.

Elemente außerhalb des Viewports MÜSSEN nicht vollständig realisiert werden.

## 6. Item Realization

Item Realization bezeichnet die Erstellung einer visuellen Control-Instanz für ein Datenelement.

Dabei gilt:

1. Der benötigte Datenbereich wird ermittelt.
2. Vorhandene Control-Instanzen werden geprüft.
3. Fehlende Instanzen werden erstellt oder wiederverwendet.
4. Datenbindungen werden zugewiesen.
5. Die Elemente werden in Layout und Darstellung integriert.

Die Anzahl realisierter Elemente SOLL sich am sichtbaren Bereich orientieren.

Die vollständige Elementmenge DARF keine Voraussetzung für die Darstellung sein.

## 7. Item Recycling

Nicht mehr benötigte Control-Instanzen können wiederverwendet werden.

Dabei gilt:

- Alte Datenbindungen werden kontrolliert gelöst.
- Temporäre Ereignisbindungen werden entfernt.
- Elementbezogene Zustände werden zurückgesetzt.
- Neue Daten werden zugewiesen.
- Layout und Darstellung werden gezielt aktualisiert.

Die Wiederverwendung DARF keine Zustände zwischen unterschiedlichen Datenelementen unbeabsichtigt übertragen.

Nicht wiederverwendbare Ressourcen MÜSSEN ordnungsgemäß freigegeben werden.

## 8. Elementzustände

Funktionale Zustände wie Auswahl, Fokus oder Bearbeitungsstatus MÜSSEN unabhängig von der Lebensdauer einer visuellen Control-Instanz verwaltet werden können.

Dabei gilt:

- Auswahlzustände bleiben beim Scrollen erhalten.
- Datenbezogene Zustände werden über stabile Elementidentitäten zugeordnet.
- Recycelte Controls übernehmen keine fremden Zustände.
- Fokuswechsel werden kontrolliert verarbeitet.
- Nicht sichtbare Elemente bleiben über ihre Datenidentität adressierbar.

Die Virtualisierung DARF keine unbeabsichtigten Datenänderungen verursachen.

## 9. Größen und Positionen

Virtualisierte Controls müssen Positionen auch für nicht realisierte Elemente bestimmen können.

Unterstützte Verfahren:

- Feste Elementgrößen.
- Geschätzte Elementgrößen.
- Dynamisch gemessene Elementgrößen.
- Positionsindizes.
- Bereichsbasierte Layoutberechnung.

Größenschätzungen MÜSSEN bei Vorliegen genauer Messwerte aktualisiert werden können.

Änderungen DÜRFEN keine unkontrollierten Scrollsprünge verursachen.

## 10. Scrollverhalten

Virtualisierte Controls unterstützen effizientes Scrollen.

Dabei gilt:

- Neue Elemente werden bedarfsgerecht realisiert.
- Nicht mehr benötigte Elemente werden freigegeben oder recycelt.
- Schnelle Scrollbewegungen MÜSSEN unterstützt werden.
- Veraltete Datenanforderungen DÜRFEN aktuelle Ergebnisse nicht überschreiben.
- Die Scrollposition MUSS möglichst stabil bleiben.

Die Virtualisierung DARF keine vollständige Neuerstellung der Elementliste bei jeder Scrollbewegung erzwingen.

## 11. Datenbereitstellung

Datenvirtualisierung verwendet definierte Bereichsanfragen.

Eine Datenquelle kann bereitstellen:

- Gesamtanzahl der Elemente, sofern bekannt.
- Datenbereich ab einem Index.
- Stabile Elementidentitäten.
- Änderungsbenachrichtigungen.
- Asynchrone Datenanforderungen.
- Abbruch laufender Anforderungen.

Unbekannte oder dynamische Datenmengen MÜSSEN unterstützt werden können.

Datenquellen DÜRFEN lokal oder über autorisierte NovaOS-Capabilities angebunden werden.

## 12. Asynchrone Verarbeitung

Datenanforderungen können asynchron erfolgen.

Dabei gilt:

- Die UI Event Loop DARF nicht durch lang laufende Datenoperationen blockiert werden.
- Anforderungen MÜSSEN abbrechbar sein können.
- Veraltete Ergebnisse MÜSSEN erkannt werden.
- Ergebnisse werden über den zuständigen UI-Ausführungskontext übernommen.
- Ladezustände können über Control States dargestellt werden.

Asynchrone Verarbeitung MUSS mit dem Nova UI Threading Model übereinstimmen.

## 13. Prefetching und Caching

Nova Virtualized Controls unterstützt das vorbereitende Laden benachbarter Datenbereiche.

Dabei gilt:

- Prefetching ist optional.
- Die Größe des vorbereiteten Bereichs MUSS begrenzbar sein.
- Cacheeinträge MÜSSEN bei Datenänderungen invalidiert werden.
- Speicherverbrauch MUSS kontrollierbar bleiben.
- Nicht benötigte Daten und Controls MÜSSEN freigegeben werden können.

Prefetching DARF die Verarbeitung aktuell sichtbarer Elemente nicht unangemessen beeinträchtigen.

## 14. Dynamische Datenänderungen

Virtualisierte Controls unterstützen Änderungen der Datenmenge zur Laufzeit.

Dazu gehören:

- Hinzufügen von Elementen.
- Entfernen von Elementen.
- Verschieben von Elementen.
- Aktualisieren einzelner Datensätze.
- Sortieren.
- Filtern.
- Gruppieren.

Änderungen MÜSSEN gezielt verarbeitet werden.

Unveränderte Datenbereiche SOLLEN ihre gültigen Layout- und Cacheinformationen behalten.

## 15. Barrierefreiheit

Virtualisierte Controls MÜSSEN die Nova UI Accessibility Architecture unterstützen.

Dabei gilt:

- Nicht realisierte Elemente MÜSSEN über geeignete virtuelle Accessibility-Repräsentationen erreichbar sein können.
- Auswahl und Fokus MÜSSEN semantisch korrekt dargestellt werden.
- Assistive Technologien DÜRFEN nicht auf die aktuell realisierten UI-Elemente beschränkt sein.
- Angeforderte Elemente können bei Bedarf realisiert werden.

Die Virtualisierung DARF die grundlegende Zugänglichkeit der Daten nicht verhindern.

## 16. Performance und Ressourcen

Nova Virtualized Controls arbeitet bedarfsorientiert.

Dabei gilt:

- Nur benötigte UI-Elemente werden realisiert.
- Control-Instanzen SOLLEN wiederverwendet werden.
- Datenanforderungen SOLLEN auf benötigte Bereiche begrenzt werden.
- Layoutberechnungen MÜSSEN gezielte Invalidierungen verwenden.
- Unveränderte Elemente DÜRFEN keine unnötigen Renderoperationen verursachen.
- Speicherverbrauch MUSS durch definierte Grenzen kontrollierbar bleiben.

Die Virtualisierung MUSS ohne permanente Aktualisierungsschleife funktionieren.

## 17. Sicherheit

Virtualisierte Controls unterliegen Nova.Trust.

Dabei gilt:

- Datenanforderungen MÜSSEN bestehende Berechtigungen einhalten.
- Nicht sichtbare Daten DÜRFEN nicht automatisch vollständig geladen werden.
- Recycelte Controls DÜRFEN keine vertraulichen Daten vorheriger Elemente offenlegen.
- Datenquellen MÜSSEN innerhalb ihres autorisierten Sicherheitskontexts arbeiten.
- Fehler einzelner Datenquellen DÜRFEN andere UI-Instanzen nicht beeinträchtigen.

Virtualisierung DARF keine Capability-Grenzen umgehen.

## 18. Normative Anforderungen

- **UI-001:** NovaOS MUSS UI-Virtualisierung für datenbasierte Controls unterstützen.
- **UI-002:** Datenvirtualisierung MUSS unabhängig von UI-Virtualisierung verwendbar sein.
- **UI-003:** Virtualisierte Controls MÜSSEN den sichtbaren Viewport berücksichtigen.
- **UI-004:** Nicht benötigte UI-Elemente DÜRFEN keine vollständige Realisierung erfordern.
- **UI-005:** Control Recycling MUSS unterstützt werden.
- **UI-006:** Recycelte Controls MÜSSEN ihre elementbezogenen Zustände korrekt zurücksetzen.
- **UI-007:** Auswahl und Fokus MÜSSEN unabhängig von visuellen Control-Instanzen verwaltet werden können.
- **UI-008:** Feste und variable Elementgrößen MÜSSEN unterstützt werden können.
- **UI-009:** Schnelles Scrollen MUSS ohne vollständige Neuerstellung der Elementmenge möglich sein.
- **UI-010:** Asynchrone Datenanforderungen MÜSSEN unterstützt werden können.
- **UI-011:** Veraltete Datenanforderungen DÜRFEN aktuelle Zustände nicht überschreiben.
- **UI-012:** Prefetching und Caching MÜSSEN begrenzbar sein.
- **UI-013:** Dynamische Datenänderungen MÜSSEN gezielt verarbeitet werden.
- **UI-014:** Virtualisierte Controls MÜSSEN mit der Nova Layout Engine zusammenarbeiten.
- **UI-015:** Virtualisierte Controls MÜSSEN unabhängig vom Renderbackend funktionieren.
- **UI-016:** Virtualisierung MUSS mit der Nova UI Accessibility Architecture zusammenarbeiten.
- **UI-017:** Ressourcenverbrauch MUSS kontrollierbar bleiben.
- **UI-018:** Datenzugriffe MÜSSEN die Sicherheitsregeln von Nova.Trust einhalten.
- **UI-019:** Fehlerhafte Datenquellen MÜSSEN kontrolliert behandelt werden.
- **UI-020:** Nova Virtualized Controls MUSS vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 19. Integration

- **Nova Control Framework:** Grundlegende Control-Architektur.
- **Nova Standard Controls:** Virtualisierbare Steuerelemente.
- **Nova Custom Controls:** Benutzerdefinierte Virtualisierung.
- **Nova UI Runtime:** Verwaltung realisierter Elemente.
- **Nova UI Property System:** Eigenschaften und Zustände.
- **Nova UI Data Binding:** Verbindung zwischen Daten und Controls.
- **Nova Layout Engine:** Größen- und Positionsberechnung.
- **Nova UI Renderer Abstraction:** Darstellung sichtbarer Elemente.
- **Nova UI Component Lifecycle:** Erstellung, Wiederverwendung und Freigabe.
- **Nova UI Threading Model:** Asynchrone Verarbeitung.
- **Nova Input Architecture:** Scrollen und Interaktion.
- **Nova UI Accessibility:** Zugänglichkeit virtueller Elemente.
- **Nova Memory Management:** Cache- und Ressourcenverwaltung.
- **Nova.Trust:** Berechtigungen und Isolation.

## 20. Ergebnis

NovaOS erhält ein natives Virtualisierungssystem zur effizienten Darstellung großer und dynamischer Datenmengen.

UI-Elemente werden ausschließlich bei Bedarf erstellt und nach Möglichkeit wiederverwendet.

Durch Viewport-Verwaltung, Datenvirtualisierung, Recycling und gezielte Layoutaktualisierungen bleiben Speicherverbrauch und Rechenaufwand auch bei sehr großen Elementmengen kontrollierbar.

**Grundsatz:** Nova Virtualized Controls verwaltet die gesamte Datenmenge, erzeugt aber nur die visuellen Elemente, die tatsächlich benötigt werden.