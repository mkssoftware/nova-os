# NPSPEC-WINDOW-LAYOUT-0004 – Nova Virtual Desktops

**Status:** Angenommen  
**Bereich:** Window Manager / Virtuelle Desktops  
**System:** NovaOS

## 1. Zweck

Nova Virtual Desktops definiert die Verwaltung mehrerer virtueller Arbeitsflächen innerhalb einer NovaOS-Benutzersitzung.

Ziel ist die übersichtliche Trennung unterschiedlicher Arbeitsbereiche, ohne Anwendungen mehrfach starten oder Fenster schließen zu müssen.

## 2. Architektur

- Zentrale Verwaltung durch den Nova Window Manager.
- Integration in Window State Model, Focus Management und Z-Order Management.
- Trennung zwischen virtuellen Desktops und physischen Displays.
- Unterstützung mehrerer virtueller Desktops pro Benutzersitzung.
- Verwaltung von Fensterzuordnungen und desktopbezogenen Layouts.
- Integration in Nova Compositor und Input System.
- Unterstützung mehrerer Monitore.
- Optionale Persistenz der Desktopkonfiguration.
- Unabhängigkeit von grafischen Übergangseffekten.

## 3. Desktop-Modell

Jeder virtuelle Desktop besitzt eine eindeutige Identität und eine eigene Fensteranordnung.

| Eigenschaft | Beschreibung |
|---|---|
| Desktop ID | Eindeutige Desktopidentität |
| Name | Benutzerdefinierter Desktopname |
| Window Set | Zugeordnete Fenster |
| Active Window | Zuletzt aktives Fenster |
| Layout State | Gespeicherte Fensteranordnung |
| Display Scope | Zugeordnete Displays |
| Desktop Policy | Verhalten und Darstellungsregeln |
| State Version | Version des Desktopzustands |

NovaOS unterscheidet:

- **Active Desktop:** Aktuell angezeigte virtuelle Arbeitsfläche.
- **Inactive Desktop:** Vorhandene, momentan nicht angezeigte Arbeitsfläche.
- **Global Window:** Fenster, das auf mehreren oder allen virtuellen Desktops angezeigt werden darf.

Virtuelle Desktops sind organisatorische Arbeitsbereiche und keine eigenständigen Sicherheitskontexte.

## 4. Kernfunktionen

- Erstellen und Entfernen virtueller Desktops.
- Wechsel zwischen virtuellen Desktops.
- Verschieben von Fenstern zwischen Desktops.
- Anzeigen einzelner Fenster auf allen Desktops.
- Individuelle Fensteranordnung pro Desktop.
- Desktopbezogene Fokus- und Z-Order-Verwaltung.
- Unterstützung von Snap-, Tiling- und Fenstergruppen.
- Desktopwechsel über Tastatur, Maus und Touch.
- Integration in Taskbar und Desktopübersicht.
- Speicherung und Wiederherstellung von Desktopkonfigurationen.
- Unterstützung unterschiedlicher Multi-Monitor-Konfigurationen.

## 5. Normative Anforderungen

- WINDOW-001: Virtuelle Desktops MÜSSEN zentral durch den Window Manager verwaltet werden.
- WINDOW-002: Jeder virtuelle Desktop MUSS eine eindeutige Identität besitzen.
- WINDOW-003: Jede Benutzersitzung MUSS mindestens einen virtuellen Desktop besitzen.
- WINDOW-004: Reguläre Fenster MÜSSEN mindestens einem gültigen Desktop zugeordnet sein.
- WINDOW-005: Desktopwechsel MÜSSEN atomar erfolgen.
- WINDOW-006: Fenster inaktiver Desktops DÜRFEN keine regulären Eingabeereignisse empfangen.
- WINDOW-007: Der Fensterzustand MUSS bei Desktopwechseln erhalten bleiben.
- WINDOW-008: Desktopwechsel DÜRFEN Anwendungen nicht automatisch beenden oder anhalten.
- WINDOW-009: Fokus und Z-Order MÜSSEN beim Desktopwechsel konsistent aktualisiert werden.
- WINDOW-010: Anwendungen DÜRFEN fremde Fenster nicht ohne Berechtigung zwischen Desktops verschieben.
- WINDOW-011: Globale Fenster MÜSSEN ausdrücklich autorisiert oder durch den Benutzer freigegeben sein.
- WINDOW-012: Geschützte Systemoberflächen MÜSSEN unabhängig vom aktiven Desktop erreichbar bleiben.
- WINDOW-013: Das Entfernen eines Desktops DARF keine zugeordneten Fenster unbeabsichtigt zerstören.
- WINDOW-014: Multi-Monitor-Konfigurationen MÜSSEN berücksichtigt werden.
- WINDOW-015: Desktopbezogene Fensterlayouts SOLLEN gespeichert werden können.
- WINDOW-016: Desktopwechsel MÜSSEN auch ohne Animationen funktionieren.
- WINDOW-017: Inaktive Desktops SOLLEN keine unnötigen Rendering-Ressourcen beanspruchen.
- WINDOW-018: Die Desktopverwaltung MUSS deterministisch und ohne KI-Abhängigkeit funktionieren.

## 6. Desktopwechsel

Ein Desktopwechsel erfolgt in folgenden Schritten:

1. **Request:** Desktopwechsel anfordern.
2. **Authorize:** Benutzerkontext und Berechtigung prüfen.
3. **Validate:** Ziel-Desktop prüfen.
4. **Save:** Aktuellen Fokus- und Layoutzustand sichern.
5. **Resolve:** Fenster des Ziel-Desktops bestimmen.
6. **Commit:** Aktiven Desktop atomar wechseln.
7. **Focus:** Geeignetes Fokusziel wiederherstellen.
8. **Notify:** Compositor und Input System informieren.
9. **Compose:** Neue Desktopdarstellung aktualisieren.

Anwendungen bleiben während des Desktopwechsels grundsätzlich aktiv.

Der Compositor DARF optionale Übergangsanimationen verwenden.

## 7. Fensterzuordnung

### Fenster verschieben

Fenster können zwischen virtuellen Desktops übertragen werden.

Dabei bleiben erhalten:

- Fensteridentität und Sicherheitskontext.
- Anwendungszustand.
- Darstellungsmodus.
- Fenstergeometrie, soweit gültig.
- Bestehende zulässige Fensterbeziehungen.

### Globale Fenster

Globale Fenster können auf mehreren virtuellen Desktops sichtbar sein.

Ihre Darstellung MUSS die geltenden Fokus-, Z-Order- und Sicherheitsregeln berücksichtigen.

### Fensterhierarchien

Zusammengehörige Parent-Child-Fenster MÜSSEN bei Desktopwechseln konsistent behandelt werden.

Modale Beziehungen DÜRFEN durch unterschiedliche Desktopzuordnungen nicht umgangen werden.

## 8. Multi-Monitor-Verhalten

NovaOS unterstützt zwei Desktopstrategien:

| Strategie | Beschreibung |
|---|---|
| Unified Desktops | Ein virtueller Desktop umfasst alle Displays |
| Independent Desktops | Displays können unterschiedliche virtuelle Desktops anzeigen |

Die verwendete Strategie ist konfigurierbar.

Dabei gelten folgende Regeln:

- Display- und Desktopidentitäten bleiben getrennt.
- Fensterzuordnungen werden unabhängig von physischen Displaypositionen verwaltet.
- Bei Displayverlust werden betroffene Fenster erreichbar gehalten.
- Änderungen der Displaykonfiguration dürfen keine virtuellen Desktops unbeabsichtigt löschen.
- Gespeicherte Desktoplayouts werden an verfügbare Displays angepasst.

## 9. Ressourcenverwaltung

- Inaktive Desktops bleiben logisch vollständig erhalten.
- Nicht sichtbare Fenster SOLLEN keine unnötigen Compositor-Aktualisierungen verursachen.
- Surface-Buffer DÜRFEN entsprechend den geltenden Ressourcenrichtlinien freigegeben werden.
- Hintergrundanwendungen laufen unabhängig von der Desktop-Sichtbarkeit weiter.
- Der Window Manager MUSS auch bei vielen virtuellen Desktops reaktionsfähig bleiben.
- Desktopwechsel SOLLEN ohne vollständige Neuerstellung der Fenster erfolgen.

## 10. Fehlerbehandlung und Wiederherstellung

- Ungültige Desktopreferenzen werden zurückgewiesen.
- Beim Entfernen eines Desktops werden dessen Fenster einem gültigen Ersatzdesktop zugeordnet.
- Fehlgeschlagene Desktopwechsel verändern den bisherigen aktiven Desktop nicht.
- Bei Anwendungsabsturz werden betroffene Fensterreferenzen bereinigt.
- Nicht verfügbare Displays führen zur Anpassung der Desktopzuordnung.
- Gespeicherte Desktopkonfigurationen werden vor der Wiederherstellung validiert.
- Ein Compositor-Neustart verändert die logische Desktopstruktur nicht.
- Ist kein gültiger Fokus verfügbar, bleibt der Tastaturfokus unbelegt.
- Sicherheits- und Modalitätsregeln bleiben über Desktopgrenzen hinweg verbindlich.

## 11. Ergebnis

NovaOS erhält eine flexible Verwaltung virtueller Desktops zur Organisation unterschiedlicher Arbeitsbereiche.

Fenster und Anwendungen können zwischen Desktops verschoben werden, während ihre Zustände und Ressourcen erhalten bleiben.

Durch die Trennung virtueller Desktops von physischen Displays unterstützt NovaOS sowohl klassische als auch unabhängige Multi-Monitor-Arbeitsflächen.

**Grundsatz:** Virtuelle Desktops schaffen Ordnung, ohne Anwendungen zu isolieren oder den Benutzer in seiner Arbeitsweise einzuschränken.