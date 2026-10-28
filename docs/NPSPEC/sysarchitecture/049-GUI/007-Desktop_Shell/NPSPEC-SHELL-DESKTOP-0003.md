# NPSPEC-SHELL-DESKTOP-0003 – Nova Desktop Icons

**Status:** Angenommen  
**Bereich:** Shell / Desktop / Icons  
**System:** NovaOS

## 1. Zweck

Nova Desktop Icons definiert die Darstellung, Verwaltung und Interaktion von Symbolen auf dem NovaOS-Desktop.

Desktop-Symbole ermöglichen den direkten Zugriff auf Dateien, Ordner, Anwendungen, Solutions, Capabilities und Systemfunktionen.

Ziel ist eine vertraute, anpassbare und ressourcenschonende Symbolverwaltung, die vollständig in das native Nova UI Framework integriert ist.

## 2. Architektur

Das Subsystem besteht aus folgenden Komponenten:

| Komponente | Aufgabe |
|---|---|
| Desktop Icon Manager | Verwaltung der Desktop-Symbole |
| Icon Provider | Bereitstellung von Symbolgrafiken |
| Icon Resolver | Ermittlung des passenden Symbols |
| Icon View | Darstellung und Benutzerinteraktion |
| Icon Layout Adapter | Anbindung an das Desktop-Layout |
| Icon State Manager | Verwaltung von Symbolzuständen |

Die Verwaltung der zugrunde liegenden Dateien und Objekte bleibt Aufgabe der jeweiligen Subsysteme.

## 3. Symboltypen

NovaOS unterstützt Desktop-Symbole für:

- Dateien und Ordner.
- Anwendungen.
- Solutions.
- Capabilities.
- Verknüpfungen.
- Laufwerke und Speicherorte.
- Systemobjekte.
- Benutzerdefinierte Desktop-Elemente.

Dabei gilt:

- Jedes Desktop-Element MUSS eine eindeutige Identität besitzen.
- Symbole MÜSSEN unabhängig von der referenzierten Ressource verwaltet werden.
- Der Symboltyp bestimmt die verfügbaren Aktionen, nicht allein die verwendete Grafik.
- Neue Symboltypen MÜSSEN über registrierte Provider integrierbar sein.

## 4. Symbolauflösung

Der Icon Resolver bestimmt das darzustellende Symbol anhand des referenzierten Objekts.

Dabei gilt:

- Individuell zugewiesene Symbole MÜSSEN unterstützt werden.
- Anwendungen und Solutions SOLLEN ihre Symbole über ihre Metadaten bereitstellen.
- Dateisymbole MÜSSEN anhand registrierter Dateitypen aufgelöst werden können.
- Fehlende Symbole MÜSSEN durch geeignete Standardsymbole ersetzt werden.
- Änderungen an Symbolzuordnungen MÜSSEN automatisch berücksichtigt werden können.

## 5. Symboldarstellung

Nova Desktop Icons unterstützt:

- Unterschiedliche Symbolgrößen.
- Hochauflösende und skalierbare Symbole.
- Transparenz.
- Beschriftungen.
- Mehrzeilige Beschriftungen.
- Auswahlmarkierungen.
- Statusindikatoren.
- Optionale Miniaturansichten.

Dabei gilt:

- Symbole MÜSSEN unterschiedliche DPI-Skalierungen unterstützen.
- Die Darstellung MUSS sich an das aktive Theme anpassen können.
- Beschriftungen MÜSSEN auch auf wechselnden Hintergründen lesbar bleiben.
- Statusindikatoren DÜRFEN das ursprüngliche Symbol nicht dauerhaft verändern.
- Die Darstellung MUSS über das native Nova UI Framework erfolgen.

## 6. Symbolzustände

Desktop-Symbole unterstützen mindestens folgende Zustände:

| Zustand | Bedeutung |
|---|---|
| Normal | Standarddarstellung |
| Hover | Zeiger befindet sich über dem Symbol |
| Selected | Symbol ist ausgewählt |
| Focused | Symbol besitzt den Tastaturfokus |
| Disabled | Symbol ist nicht verfügbar |
| Dragging | Symbol wird verschoben |
| Loading | Zugehörige Ressource wird geladen |
| Error | Ressource oder Aktion ist fehlerhaft |

Zustände MÜSSEN über das Nova UI Visual State System dargestellt werden.

## 7. Benutzerinteraktion

Unterstützt werden:

- Einfachklick.
- Doppelklick.
- Mehrfachauswahl.
- Rechteckauswahl.
- Kontextmenüs.
- Drag & Drop.
- Tastaturnavigation.
- Umbenennen.
- Aktivieren und Öffnen.

Dabei gilt:

- Aktionen MÜSSEN über das Nova Command- und Capability-System ausgeführt werden können.
- Drag & Drop MUSS das Nova Data Transfer System verwenden.
- Die Auswahl mehrerer Symbole MUSS unterstützt werden.
- Eingaben MÜSSEN über das Nova Input System verarbeitet werden.
- Das Entfernen einer Verknüpfung DARF die referenzierte Ressource nicht löschen.

## 8. Anordnung und Positionierung

Die Positionierung erfolgt über den Nova Desktop Layout Manager.

Unterstützt werden:

- Freie Anordnung.
- Rasterausrichtung.
- Automatische Sortierung.
- Benutzerdefinierte Gruppierung.
- Unterschiedliche Symbolgrößen.
- Mehrmonitorbetrieb.

Dabei gilt:

- Symbolpositionen MÜSSEN dauerhaft gespeichert werden können.
- Änderungen der Bildschirmauflösung DÜRFEN Symbole nicht unerreichbar machen.
- Manuelle Anordnungen DÜRFEN nicht ohne Benutzerentscheidung überschrieben werden.
- Die Symbolverwaltung DARF keine eigene konkurrierende Layout-Engine implementieren.

## 9. Miniaturansichten

NovaOS KANN für geeignete Dateitypen Miniaturansichten anzeigen.

Dabei gilt:

- Miniaturansichten MÜSSEN über registrierte Thumbnail Provider bereitgestellt werden.
- Die Generierung SOLL asynchron erfolgen.
- Miniaturansichten SOLLEN zwischengespeichert werden.
- Nicht sichtbare Symbole SOLLEN keine unnötige Generierung auslösen.
- Fehlerhafte Provider DÜRFEN die Desktop Shell nicht beeinträchtigen.
- Der Benutzer MUSS Miniaturansichten deaktivieren können.

## 10. Kontextabhängige Funktionen

Desktop-Symbole integrieren das NovaOS-Capability-System.

Dabei gilt:

- Verfügbare Aktionen MÜSSEN anhand des referenzierten Objekts ermittelt werden können.
- Kontextmenüs MÜSSEN dynamisch zusammengestellt werden können.
- Nicht verfügbare Funktionen SOLLEN ausgeblendet oder deaktiviert werden.
- Shell-Erweiterungen DÜRFEN zusätzliche Aktionen registrieren.
- Berechtigungen MÜSSEN vor der Ausführung geprüft werden.

## 11. Performance und Rendering

Nova Desktop Icons MUSS ressourcenschonend arbeiten.

Dabei gilt:

- Symbolgrafiken SOLLEN zentral zwischengespeichert werden.
- Identische Symbolressourcen SOLLEN gemeinsam verwendet werden.
- Nur sichtbare oder benötigte Symbole SOLLEN aufwendig verarbeitet werden.
- Änderungen MÜSSEN über Damage Tracking gezielt invalidiert werden.
- Unveränderte Symbole DÜRFEN nicht permanent neu gerendert werden.
- Symbolauflösung und Thumbnail-Generierung DÜRFEN die UI Event Loop nicht blockieren.

## 12. Zustand und Wiederherstellung

Der Desktop Icon Manager arbeitet mit dem Nova Desktop State Manager zusammen.

Dabei gilt:

- Symbolpositionen und individuelle Einstellungen MÜSSEN gespeichert werden können.
- Auswahlzustände KÖNNEN während eines Shell-Neustarts erhalten bleiben.
- Änderungen an referenzierten Ressourcen MÜSSEN erkannt werden können.
- Nicht mehr verfügbare Ressourcen MÜSSEN kontrolliert behandelt werden.
- Beschädigte Symboldaten DÜRFEN den Desktopstart nicht verhindern.

## 13. Sicherheit und Barrierefreiheit

Nova Desktop Icons unterliegt Nova.Trust und der Nova Accessibility Architecture.

Dabei gilt:

- Symbole DÜRFEN keine zusätzlichen Berechtigungen verleihen.
- Verknüpfungen MÜSSEN vor der Ausführung sicher aufgelöst werden.
- Icon- und Thumbnail Provider MÜSSEN innerhalb autorisierter Sicherheitskontexte arbeiten.
- Symbole MÜSSEN über Tastatur und alternative Eingabemethoden bedienbar sein.
- Beschriftungen und Zustände MÜSSEN für Screenreader verfügbar sein.
- High Contrast und Reduced Motion MÜSSEN berücksichtigt werden.

## 14. Normative Anforderungen

- **DESKTOP-001:** NovaOS MUSS ein natives Desktop Icon System bereitstellen.
- **DESKTOP-002:** Desktop-Symbole MÜSSEN unterschiedliche Objekttypen repräsentieren können.
- **DESKTOP-003:** Symbole MÜSSEN unabhängig von ihren referenzierten Ressourcen verwaltet werden.
- **DESKTOP-004:** Icon Provider MÜSSEN registrierbar sein.
- **DESKTOP-005:** Fehlende Symbole MÜSSEN durch Standardsymbole ersetzt werden.
- **DESKTOP-006:** Unterschiedliche Symbolgrößen und DPI-Skalierungen MÜSSEN unterstützt werden.
- **DESKTOP-007:** Auswahl, Mehrfachauswahl und Tastaturnavigation MÜSSEN unterstützt werden.
- **DESKTOP-008:** Drag & Drop MUSS über das Nova Data Transfer System erfolgen.
- **DESKTOP-009:** Symbolpositionen MÜSSEN gespeichert und wiederhergestellt werden können.
- **DESKTOP-010:** Kontextabhängige Aktionen MÜSSEN über das Capability-System integrierbar sein.
- **DESKTOP-011:** Miniaturansichten MÜSSEN optional und asynchron bereitgestellt werden können.
- **DESKTOP-012:** Symbolressourcen SOLLEN zwischengespeichert werden.
- **DESKTOP-013:** Die Darstellung MUSS Damage Tracking unterstützen.
- **DESKTOP-014:** Fehlerhafte Provider DÜRFEN die Desktop Shell nicht beeinträchtigen.
- **DESKTOP-015:** Desktop-Symbole MÜSSEN Nova.Trust und Accessibility berücksichtigen.
- **DESKTOP-016:** Nova Desktop Icons MUSS vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 15. Integration

- **Nova Desktop Environment:** Verwaltung des Desktoparbeitsbereichs.
- **Nova Desktop Background:** Hintergrunddarstellung.
- **Nova Desktop Layout Manager:** Positionierung und Anordnung.
- **Nova Shell Extension Model:** Erweiterbare Symbolaktionen.
- **Nova UI Framework:** Darstellung und Interaktion.
- **Nova Visual State System:** Symbolzustände.
- **Nova Compositor:** Darstellung und Damage Tracking.
- **Nova Input System:** Benutzerinteraktion.
- **Nova Data Transfer:** Drag & Drop.
- **Nova VFS:** Dateien und Ordner.
- **Nova Capability System:** Objektbezogene Aktionen.
- **Nova Shell Recovery:** Wiederherstellung des Desktopzustands.
- **Nova.Trust:** Berechtigungen und Sicherheit.

## 16. Ergebnis

NovaOS erhält ein einheitliches und erweiterbares System zur Darstellung und Verwaltung von Desktop-Symbolen.

Symbole repräsentieren Dateien, Anwendungen, Solutions, Capabilities und weitere Systemobjekte, ohne deren eigentliche Implementierung zu übernehmen.

Durch zentrale Symbolauflösung, Zwischenspeicherung und bedarfsgesteuertes Rendering bleibt die Desktopdarstellung effizient und anpassbar.

**Grundsatz:** Ein Desktop-Symbol ist die visuelle Repräsentation eines Objekts – nicht das Objekt selbst.