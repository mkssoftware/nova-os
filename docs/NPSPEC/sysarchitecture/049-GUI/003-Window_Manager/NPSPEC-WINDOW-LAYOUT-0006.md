# NPSPEC-WINDOW-LAYOUT-0006 – Nova Window Layout Persistence

**Status:** Angenommen  
**Bereich:** Window Manager / Layout-Persistenz  
**System:** NovaOS

## 1. Zweck

Nova Window Layout Persistence definiert die dauerhafte Speicherung und Wiederherstellung von Fensteranordnungen innerhalb der NovaOS-Desktopumgebung.

Ziel ist, dass Benutzer ihre gewohnte Arbeitsumgebung nach Neustarts, Sitzungswechseln oder Änderungen der Displaykonfiguration möglichst unverändert vorfinden.

## 2. Architektur

- Zentrale Layoutverwaltung durch den Nova Window Manager.
- Persistenz über den NovaOS State Store.
- Trennung zwischen Laufzeitzustand und gespeichertem Layout.
- Unterstützung benutzer-, workspace- und anwendungsbezogener Layouts.
- Integration in Window Positioning, Resizing und State Model.
- Unterstützung von Snap-, Tiling- und Fenstergruppen.
- Berücksichtigung virtueller Desktops und mehrerer Displays.
- Transaktionale Speicherung und Wiederherstellung.
- Keine Abhängigkeit von Cloud-Diensten oder KI.

## 3. Persistenzmodell

NovaOS unterscheidet folgende Layouttypen:

| Typ | Beschreibung |
|---|---|
| Session Layout | Fensteranordnung einer Benutzersitzung |
| Workspace Layout | Anordnung innerhalb eines virtuellen Desktops |
| Application Layout | Gespeicherte Fensteranordnung einer Anwendung |
| Group Layout | Anordnung zusammengehöriger Fenster |
| User Layout | Benutzerdefinierte und benannte Layoutvorlage |
| System Layout | Systemseitig vorgegebenes Standardlayout |

Gespeicherte Layouts beschreiben Fensteranordnungen und begründen keine Berechtigung zum Starten von Anwendungen oder zum Zugriff auf deren Inhalte.

## 4. Layoutdaten

Ein gespeichertes Layout enthält bei Bedarf:

| Eigenschaft | Beschreibung |
|---|---|
| Layout ID | Eindeutige Layoutidentität |
| Schema Version | Version des Datenformats |
| Owner Context | Besitzer und Sicherheitskontext |
| Window Reference | Wiederherstellbare Fensterreferenz |
| Geometry | Position und Größe |
| Window Mode | Normal, Maximized oder Fullscreen |
| Restore Geometry | Wiederherstellungsgeometrie |
| Display Reference | Bevorzugtes Display |
| Workspace Reference | Zugeordneter virtueller Desktop |
| Z-Order | Relative Stapelreihenfolge |
| Group Reference | Optionale Fenstergruppe |
| Layout Rules | Snap-, Tiling- und Platzierungsregeln |

Temporäre Zustände wie laufende Animationen, aktive Resize-Vorgänge oder ausstehende IPC-Anforderungen werden nicht persistiert.

## 5. Normative Anforderungen

- WINDOW-001: Layouts MÜSSEN unabhängig vom aktuellen Compositor-Zustand gespeichert werden können.
- WINDOW-002: Gespeicherte Layouts MÜSSEN versioniert sein.
- WINDOW-003: Schreibvorgänge MÜSSEN atomar erfolgen.
- WINDOW-004: Unvollständige Schreibvorgänge DÜRFEN gültige Layoutdaten nicht beschädigen.
- WINDOW-005: Fensterreferenzen MÜSSEN bei der Wiederherstellung neu validiert werden.
- WINDOW-006: Flüchtige Window IDs DÜRFEN nicht als alleinige persistente Fensteridentität verwendet werden.
- WINDOW-007: Layoutdaten MÜSSEN dem jeweiligen Benutzer- und Sicherheitskontext zugeordnet sein.
- WINDOW-008: Die Wiederherstellung DARF keine Sicherheits- oder Fensterberechtigungen umgehen.
- WINDOW-009: Gespeicherte Geometrien MÜSSEN an die aktuelle Displaykonfiguration angepasst werden können.
- WINDOW-010: Nicht verfügbare Anwendungen oder Fenster DÜRFEN die Wiederherstellung anderer Fenster nicht blockieren.
- WINDOW-011: Benutzerdefinierte Layouts MÜSSEN ausdrücklich gespeichert und gelöscht werden können.
- WINDOW-012: Automatische Layoutspeicherung MUSS deaktivierbar sein.
- WINDOW-013: Manuelle Layoutänderungen des Benutzers MÜSSEN Vorrang vor automatischen Wiederherstellungsregeln besitzen.
- WINDOW-014: Die Wiederherstellung MUSS ohne KI und ohne Netzwerkverbindung funktionieren.
- WINDOW-015: Fehlerhafte Layoutdaten MÜSSEN kontrolliert zurückgewiesen werden.
- WINDOW-016: Ein gültiges Standardlayout MUSS als Fallback verfügbar sein.

## 6. Speicherung

NovaOS unterstützt zwei Speicherverfahren.

### Automatische Speicherung

- Bei regulärer Sitzungsbeendigung.
- Nach abgeschlossenen relevanten Layoutänderungen.
- Bei Änderungen virtueller Desktops.
- Bei ausdrücklich aktivierter kontinuierlicher Zustandssicherung.

Häufige Änderungen SOLLEN zusammengefasst werden, um unnötige Schreibzugriffe zu vermeiden.

### Manuelle Speicherung

Der Benutzer kann aktuelle Fensteranordnungen als benannte Layoutvorlagen speichern.

Diese Vorlagen können später ausgewählt, geändert, dupliziert oder gelöscht werden.

## 7. Wiederherstellung

Die Wiederherstellung erfolgt in folgenden Schritten:

1. **Load:** Gespeichertes Layout laden.
2. **Validate:** Datenformat, Integrität und Berechtigungen prüfen.
3. **Resolve:** Anwendungen und Fensterreferenzen zuordnen.
4. **Map:** Displays und virtuelle Desktops zuordnen.
5. **Calculate:** Gültige Fenstergeometrien berechnen.
6. **Restore:** Fensterzustände und Layoutgruppen wiederherstellen.
7. **Commit:** Zusammengehörige Layoutänderungen atomar übernehmen.
8. **Notify:** Window Manager, Anwendungen und Compositor synchronisieren.

Die Wiederherstellung DARF bereits aktive Benutzerinteraktionen nicht unkontrolliert überschreiben.

## 8. Display- und Workspace-Anpassung

Bei abweichender Displaykonfiguration:

- Vorhandene Displayidentitäten werden bevorzugt wiederverwendet.
- Fehlende Displays werden durch geeignete verfügbare Displays ersetzt.
- Fenstergeometrien werden an gültige Arbeitsbereiche angepasst.
- Maximierte Fenster verwenden den aktuellen Arbeitsbereich.
- Snap- und Tiling-Layouts werden neu berechnet.
- Virtuelle Desktops werden anhand gültiger gespeicherter Zuordnungen wiederhergestellt.

Die ursprüngliche Layoutkonfiguration SOLL erhalten bleiben, damit sie bei erneuter Verbindung der Displays wiederverwendet werden kann.

## 9. Sitzungs- und Anwendungszustand

Layout-Persistenz und Anwendungszustand bleiben getrennte Verantwortlichkeiten.

- Der Window Manager speichert Fensteranordnungen.
- Anwendungen verwalten ihre fachlichen Daten und internen Zustände.
- Das Sitzungsmanagement koordiniert gegebenenfalls den Neustart von Anwendungen.
- Eine gespeicherte Fensterreferenz DARF keine Anwendung ohne entsprechende Berechtigung starten.
- Nicht wiederherstellbare Fenster werden übersprungen.

NovaOS SOLL Anwendungen ermöglichen, stabile Wiederherstellungsreferenzen für ihre Fenster bereitzustellen.

## 10. Fehlerbehandlung

- Beschädigte Layoutdaten werden nicht übernommen.
- Unvollständige Wiederherstellungen dürfen keine ungültigen Fensterzustände hinterlassen.
- Fehlende Anwendungen werden ohne Blockierung anderer Fenster behandelt.
- Ungültige Fensterpositionen werden automatisch korrigiert.
- Nicht unterstützte Layoutversionen werden über definierte Migrationsregeln verarbeitet oder zurückgewiesen.
- Bei fehlgeschlagener Wiederherstellung bleibt ein bedienbarer Desktop verfügbar.
- Der letzte gültige Speicherstand SOLL für eine Wiederherstellung verfügbar bleiben.

## 11. Ergebnis

NovaOS erhält eine zuverlässige, lokale und transaktionale Speicherung von Fensterlayouts.

Benutzer können ihre Arbeitsumgebung automatisch wiederherstellen oder individuelle Layoutvorlagen verwenden.

Fensteranordnungen bleiben auch bei veränderten Displaykonfigurationen, Sitzungswechseln und Systemneustarts möglichst erhalten.

**Grundsatz:** NovaOS bewahrt die Arbeitsumgebung des Benutzers, ohne ihm eine bestimmte Fensteranordnung aufzuzwingen.