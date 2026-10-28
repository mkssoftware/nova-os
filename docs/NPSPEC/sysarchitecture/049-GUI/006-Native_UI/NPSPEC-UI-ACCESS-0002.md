# NPSPEC-UI-ACCESS-0002 – Nova Accessibility Tree

**Status:** Angenommen  
**Bereich:** UI / Accessibility / Semantic Tree  
**System:** NovaOS

## 1. Zweck

Nova Accessibility Tree definiert die semantische Repräsentation der nativen NovaOS-Benutzeroberfläche.

Ziel ist es, unterstützenden Technologien eine einheitliche, strukturierte und rendererunabhängige Sicht auf UI-Elemente, deren Eigenschaften, Zustände und Bedienmöglichkeiten bereitzustellen.

Der Accessibility Tree bildet die für die Bedienung relevante Struktur ab, nicht zwangsläufig die vollständige visuelle oder logische UI-Hierarchie.

## 2. Architektur

Das Subsystem umfasst folgende Komponenten:

| Komponente | Aufgabe |
|---|---|
| Accessibility Tree Manager | Verwaltung der semantischen Hierarchie |
| Accessibility Node | Repräsentation eines zugänglichen UI-Elements |
| Accessibility Tree Builder | Aufbau der semantischen Struktur |
| Accessibility Node Resolver | Zuordnung zwischen UI-Elementen und Accessibility Nodes |
| Accessibility Change Tracker | Erkennung relevanter Änderungen |
| Accessibility Query Interface | Abfrage der semantischen Informationen |

Der Accessibility Tree wird aus dem Nova UI Object Model und den Accessibility Providern der Controls abgeleitet.

## 3. Accessibility Nodes

Ein Accessibility Node beschreibt ein semantisch relevantes UI-Element.

Ein Node kann folgende Informationen bereitstellen:

- Eindeutige Identität innerhalb seines UI-Kontexts.
- Semantische Rolle.
- Zugänglichen Namen und Beschreibung.
- Aktuellen Wert und Zustand.
- Verfügbare Bedienaktionen.
- Beziehungen zu anderen Nodes.
- Position und sichtbaren Bereich.
- Fokus- und Auswahlstatus.

Die Informationen MÜSSEN dem aktuellen UI-Zustand entsprechen.

## 4. Semantische Hierarchie

Der Accessibility Tree bildet die für den Benutzer relevante Bedienstruktur ab.

Beispiel:

```text
Window: Einstellungen
 ├── Navigation
 │    ├── Button: Allgemein
 │    └── Button: Darstellung
 └── Group: Darstellung
      ├── Text: Design
      ├── ComboBox: Farbschema
      └── CheckBox: Animationen reduzieren
```

Dabei gilt:

- Die Hierarchie MUSS logisch navigierbar sein.
- Rein dekorative Elemente DÜRFEN ausgeschlossen werden.
- Technische Template-Elemente MÜSSEN nicht automatisch als eigene Nodes erscheinen.
- Semantisch zusammengehörige Elemente SOLLEN entsprechend gruppiert werden.
- Die Hierarchie MUSS unabhängig vom Renderbackend funktionieren.

## 5. Rollen und Zustände

Accessibility Nodes verwenden standardisierte semantische Rollen.

Beispiele:

- Window.
- Dialog.
- Button.
- Text.
- TextBox.
- CheckBox.
- ComboBox.
- List.
- ListItem.
- Tree.
- TreeItem.
- Slider.
- ProgressBar.

Zustände können unter anderem `Focused`, `Selected`, `Checked`, `Disabled`, `Expanded` und `Busy` umfassen.

Rollen und Zustände MÜSSEN durch das Nova Accessibility Framework eindeutig definiert werden.

## 6. Beziehungen zwischen Nodes

Neben der hierarchischen Struktur unterstützt der Accessibility Tree semantische Beziehungen.

Dazu gehören:

- Beschriftung eines Elements.
- Beschreibung eines Elements.
- Zugehörigkeit zu einer Gruppe.
- Steuerung eines anderen Elements.
- Bezug zwischen Statusmeldung und betroffenem Element.

Diese Beziehungen MÜSSEN unabhängig von der visuellen Position verfügbar sein.

## 7. Dynamische Aktualisierung

Der Accessibility Tree wird bei relevanten UI-Änderungen aktualisiert.

Dabei gilt:

- Änderungen werden inkrementell verarbeitet.
- Unveränderte Nodes behalten ihre Identität.
- Neue und entfernte Elemente werden kontrolliert übernommen.
- Änderungen von Namen, Werten und Zuständen werden erkannt.
- Relevante Änderungen werden über Accessibility Events gemeldet.
- Abfragen DÜRFEN keine unnötige vollständige Neuberechnung erzwingen.

## 8. Virtualisierte Controls

Virtualisierte Controls MÜSSEN ihre Inhalte semantisch zugänglich machen.

Dabei gilt:

- Nicht sichtbare Datenelemente MÜSSEN nicht dauerhaft als vollständige UI-Objekte existieren.
- Accessibility Provider MÜSSEN relevante virtuelle Elemente zugänglich machen können.
- Navigation und Auswahl MÜSSEN auch bei großen Datenmengen funktionieren.
- Die Identität virtueller Elemente MUSS während ihrer gültigen Lebensdauer stabil bleiben.
- Das Realisieren eines Elements DARF nicht unnötig die gesamte Datenmenge laden.

## 9. Fokus und Navigation

Der Accessibility Tree unterstützt die semantische Navigation.

Dabei gilt:

- Der aktuell fokussierte Node MUSS ermittelbar sein.
- Fokusänderungen MÜSSEN nachvollziehbar bleiben.
- Die semantische Reihenfolge SOLL der logischen Bedienreihenfolge entsprechen.
- Unterstützende Technologien MÜSSEN Eltern-, Kind- und Geschwisterelemente abfragen können.
- Verfügbare Aktionen werden über die zuständigen Accessibility Provider ausgeführt.

Die Fokusverwaltung verbleibt beim Nova UI Framework.

## 10. Lebenszyklus und Ressourcen

Accessibility Nodes unterliegen dem Nova UI Component Lifecycle.

Dabei gilt:

- Nodes werden bei Bedarf erstellt.
- Nicht mehr gültige Nodes werden entfernt.
- Entfernte Nodes DÜRFEN keine ungültigen UI-Referenzen behalten.
- Abfragen auf entfernte Elemente MÜSSEN kontrolliert beantwortet werden.
- Ressourcen MÜSSEN nach dem Entfernen eines UI-Kontexts freigegeben werden.

## 11. Sicherheit

Nova Accessibility Tree unterliegt Nova.Trust.

Dabei gilt:

- Unterstützende Technologien erhalten ausschließlich autorisierte Informationen.
- Geschützte Eingaben DÜRFEN nicht als Klartext offengelegt werden.
- Sicherheitsgrenzen zwischen Anwendungen MÜSSEN berücksichtigt werden.
- Accessibility Nodes DÜRFEN keine zusätzlichen Capabilities verleihen.
- Aktionen MÜSSEN durch die zuständigen Sicherheitsprüfungen autorisiert werden.

## 12. NovaStudio

NovaStudio unterstützt:

- Visuelle Darstellung des Accessibility Tree.
- Prüfung semantischer Rollen.
- Bearbeitung zugänglicher Namen und Beschreibungen.
- Analyse der Navigationsreihenfolge.
- Erkennung fehlender oder widersprüchlicher Informationen.
- Prüfung virtualisierter Controls.
- Live-Diagnose von Accessibility-Änderungen.

## 13. Normative Anforderungen

- **UI-001:** NovaOS MUSS einen semantischen Accessibility Tree bereitstellen.
- **UI-002:** Der Tree MUSS unabhängig von der visuellen Renderhierarchie funktionieren.
- **UI-003:** Accessibility Nodes MÜSSEN eindeutig identifizierbar sein.
- **UI-004:** Nodes MÜSSEN Rollen, Namen und Zustände bereitstellen können.
- **UI-005:** Semantische Beziehungen MÜSSEN unterstützt werden.
- **UI-006:** Rein dekorative Elemente DÜRFEN ausgeschlossen werden.
- **UI-007:** Änderungen MÜSSEN inkrementell verarbeitet werden.
- **UI-008:** Relevante Änderungen MÜSSEN über Accessibility Events gemeldet werden.
- **UI-009:** Virtualisierte Controls MÜSSEN semantisch zugänglich bleiben.
- **UI-010:** Fokus und Navigationsbeziehungen MÜSSEN abfragbar sein.
- **UI-011:** Entfernte Nodes MÜSSEN kontrolliert freigegeben werden.
- **UI-012:** Accessibility-Abfragen DÜRFEN die UI Event Loop nicht dauerhaft blockieren.
- **UI-013:** NovaStudio MUSS den Accessibility Tree darstellen und prüfen können.
- **UI-014:** Geschützte Informationen MÜSSEN gemäß Nova.Trust behandelt werden.
- **UI-015:** Nova Accessibility Tree MUSS vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 14. Integration

- **Nova Accessibility Architecture:** Übergeordnete Architektur.
- **Nova UI Runtime:** Verwaltung der UI-Objekte.
- **Nova UI Property System:** Semantische Eigenschaften.
- **Nova Control Framework:** Rollen und Zustände.
- **Nova Control Templates:** Trennung visueller und semantischer Struktur.
- **Nova Virtualized Controls:** Virtuelle Accessibility Nodes.
- **Nova UI Event System:** Änderungsbenachrichtigungen.
- **Nova UI Component Lifecycle:** Lebenszyklus der Nodes.
- **Nova UI Accessibility Provider:** Bereitstellung von Informationen und Aktionen.
- **NovaStudio:** Semantischer Tree-Inspector.
- **Nova.Trust:** Zugriffsschutz und Isolation.

## 15. Ergebnis

NovaOS erhält eine einheitliche semantische Repräsentation seiner Benutzeroberfläche.

Unterstützende Technologien können UI-Elemente, Zustände, Beziehungen und Bedienmöglichkeiten zuverlässig erkennen, ohne von der visuellen Implementierung abhängig zu sein.

Durch inkrementelle Aktualisierung und Virtualisierung bleibt der Accessibility Tree auch bei komplexen Benutzeroberflächen ressourcenschonend.

**Grundsatz:** Der Accessibility Tree beschreibt nicht, wie die Benutzeroberfläche aussieht, sondern was ihre Elemente bedeuten und wie sie bedient werden können.