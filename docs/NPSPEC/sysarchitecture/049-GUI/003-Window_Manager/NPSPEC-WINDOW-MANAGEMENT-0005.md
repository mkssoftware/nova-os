# NPSPEC-WINDOW-MANAGEMENT-0005 – Nova Window Z-Order Management

**Status:** Angenommen  
**Bereich:** Window Manager / Fensterstapelverwaltung  
**System:** NovaOS

## 1. Zweck

Nova Window Z-Order Management definiert die Stapelreihenfolge von Fenstern innerhalb der NovaOS-Desktopumgebung.

Ziel ist eine deterministische, sichere und effiziente Verwaltung der Fensterüberlagerung unter Berücksichtigung von Aktivierung, Fensterhierarchien, Workspaces und geschützten Systemoberflächen.

## 2. Architektur

- Zentrale Z-Order-Verwaltung durch den Nova Window Manager.
- Trennung zwischen logischer Stapelreihenfolge und grafischer Darstellung.
- Integration in Nova Window State Model und Window Security.
- Unterstützung definierter Fenster- und Prioritätsebenen.
- Berücksichtigung von Fensterhierarchien und modalen Dialogen.
- Integration in Fokusverwaltung und Input Routing.
- Synchronisation mit Nova Compositor.
- Transaktionale Verarbeitung von Reihenfolgeänderungen.

## 3. Z-Order-Modell

Die Z-Order bestimmt, welche Fenster andere Fenster überlagern.

NovaOS verwendet eine hierarchische Stapelverwaltung mit definierten Ebenen.

### 3.1 Fensterebenen

| Ebene | Beschreibung |
|---|---|
| Desktop | Desktop-Hintergrund und Desktopobjekte |
| Normal | Reguläre Anwendungsfenster |
| Always on Top | Autorisierte dauerhaft überlagernde Fenster |
| System | Systemleisten, Systemmenüs und Systemoberflächen |
| Secure | Geschützte sicherheitskritische Oberflächen |

Die Reihenfolge innerhalb einer Ebene wird durch den Window Manager verwaltet.

Die Ebenenzuordnung unterliegt den geltenden Sicherheitsrichtlinien.

### 3.2 Fensterhierarchien

- Untergeordnete Fenster können an die Stapelgruppe ihres Besitzfensters gebunden werden.
- Modale Dialoge werden innerhalb ihres zulässigen Kontexts über den betroffenen Fenstern angeordnet.
- Fensterhierarchien dürfen keine Sicherheitsgrenzen umgehen.
- Änderungen an einer Fenstergruppe müssen deren interne Stapelregeln erhalten.

## 4. Kernfunktionen

- Verwaltung der Fensterstapelreihenfolge.
- Anheben und Absenken von Fenstern.
- Aktivierungsabhängige Z-Order-Anpassung.
- Unterstützung von Always-on-Top-Fenstern.
- Verwaltung modaler Fenster.
- Gruppierung zusammengehöriger Fenster.
- Berücksichtigung von Workspaces.
- Verwaltung geschützter Systemebenen.
- Bereitstellung der Stapelreihenfolge für den Compositor.
- Unterstützung effizienter Sichtbarkeitsberechnungen.

## 5. Normative Anforderungen

- WINDOW-001: Der Window Manager MUSS die verbindliche Z-Order zentral verwalten.
- WINDOW-002: Jedes darstellbare Fenster MUSS einer definierten Stapelgruppe zugeordnet sein.
- WINDOW-003: Änderungen der Stapelreihenfolge MÜSSEN atomar erfolgen.
- WINDOW-004: Anwendungen DÜRFEN Z-Order-Änderungen ausschließlich über autorisierte Schnittstellen anfordern.
- WINDOW-005: Anwendungen DÜRFEN ihre Fenster nicht eigenmächtig in privilegierte Ebenen verschieben.
- WINDOW-006: Geschützte Systemoberflächen MÜSSEN gegen unberechtigte Überlagerung geschützt werden.
- WINDOW-007: Die Aktivierung eines Fensters DARF dessen Stapelposition gemäß den geltenden Fokusregeln verändern.
- WINDOW-008: Fokus und Z-Order MÜSSEN als getrennte Zustandsinformationen verwaltet werden.
- WINDOW-009: Always-on-Top-Eigenschaften MÜSSEN durch die geltenden Sicherheitsrichtlinien begrenzt werden.
- WINDOW-010: Modale Fenster MÜSSEN innerhalb ihres zulässigen Geltungsbereichs korrekt angeordnet werden.
- WINDOW-011: Fensterhierarchien MÜSSEN bei Z-Order-Änderungen konsistent bleiben.
- WINDOW-012: Minimierte und ausgeblendete Fenster DÜRFEN die Eingabezielbestimmung nicht beeinflussen.
- WINDOW-013: Die Z-Order MUSS pro Workspace konsistent verwaltet werden.
- WINDOW-014: Änderungen MÜSSEN dem Compositor in einer eindeutig definierten Reihenfolge übermittelt werden.
- WINDOW-015: Veraltete Z-Order-Aktualisierungen DÜRFEN neuere Zustände nicht überschreiben.
- WINDOW-016: Fensterüberlagerungen MÜSSEN bei der Eingabezielbestimmung berücksichtigt werden.
- WINDOW-017: Ein Compositor-Neustart DARF die verbindliche Stapelreihenfolge nicht verändern.
- WINDOW-018: Z-Order-Operationen MÜSSEN ohne KI-Abhängigkeit deterministisch erfolgen.

## 6. Z-Order-Operationen

### Raise

Verschiebt ein Fenster innerhalb seiner zulässigen Stapelgruppe nach oben.

### Lower

Verschiebt ein Fenster innerhalb seiner zulässigen Stapelgruppe nach unten.

### Bring to Front

Fordert die oberste zulässige Position innerhalb der zugeordneten Ebene an.

### Send to Back

Verschiebt ein Fenster an die unterste zulässige Position seiner Ebene.

### Set Always on Top

Aktiviert oder deaktiviert die dauerhafte Überlagerung regulärer Fenster, sofern die entsprechende Berechtigung vorliegt.

### Reorder Group

Verändert die Stapelposition einer zusammengehörigen Fenstergruppe unter Beibehaltung ihrer internen Regeln.

Keine dieser Operationen DARF bestehende Sicherheitsgrenzen überschreiten.

## 7. Aktivierung und Fokus

Aktivierung und Stapelreihenfolge sind voneinander unabhängige Eigenschaften.

- Ein aktives Fenster muss nicht das oberste Fenster des gesamten Desktops sein.
- Always-on-Top-Fenster können über aktiven regulären Fenstern liegen.
- Eine Fokusanforderung führt nicht automatisch zur Aktivierung.
- Anwendungen dürfen den Fokus nicht durch wiederholte Z-Order-Anforderungen erzwingen.
- Benutzerinitiierte Aktivierungen besitzen Vorrang vor konkurrierenden automatischen Anordnungsanforderungen.

Die Fokusverwaltung entscheidet über die Eingabeaktivierung. Die Z-Order-Verwaltung entscheidet über die logische Überlagerung.

## 8. Integration in den Compositor

Der Window Manager übermittelt dem Compositor:

- Fensteridentität.
- Zugeordnete Stapelgruppe.
- Verbindliche Stapelposition.
- Sichtbarkeitszustand.
- Fensterhierarchie.
- Zugeordneten Workspace.
- Version der Stapelkonfiguration.

Der Compositor verwendet diese Informationen für:

- Reihenfolge der Surface-Komposition.
- Sichtbarkeits- und Überdeckungsberechnung.
- Occlusion Culling.
- Damage Tracking.
- Optimierung verdeckter Fenster.
- Korrekte Darstellung von Überlagerungen.

Der Compositor DARF die verbindliche Z-Order nicht eigenständig verändern.

## 9. Verarbeitung und Fehlerbehandlung

Eine Z-Order-Änderung erfolgt nach folgendem Ablauf:

1. **Request:** Änderungsanforderung empfangen.
2. **Authorize:** Berechtigungen prüfen.
3. **Validate:** Stapelgruppe und Fensterbeziehungen prüfen.
4. **Resolve:** Neue zulässige Position bestimmen.
5. **Commit:** Stapelreihenfolge atomar aktualisieren.
6. **Notify:** Compositor und Input System informieren.
7. **Update:** Darstellung und Eingabezielbestimmung aktualisieren.

Dabei gelten folgende Regeln:

- Ungültige Anforderungen werden ohne Zustandsänderung zurückgewiesen.
- Geschlossene Fenster werden aus der Stapelverwaltung entfernt.
- Fenstergruppen bleiben bei der Entfernung einzelner Fenster konsistent.
- Fehlerhafte Anwendungen dürfen keine fremden Stapelgruppen manipulieren.
- Bei Synchronisationsfehlern bleibt der verbindliche Zustand im Window Manager erhalten.
- Die aktuelle Stapelreihenfolge kann nach einem Compositor-Neustart vollständig rekonstruiert werden.

## 10. Ergebnis

NovaOS erhält eine sichere und deterministische Verwaltung der Fensterstapelreihenfolge.

Fenster können angehoben, abgesenkt und innerhalb definierter Ebenen organisiert werden, ohne Fokusregeln oder Sicherheitsgrenzen zu verletzen.

Die Trennung zwischen logischer Z-Order und grafischer Komposition ermöglicht eine effiziente Darstellung und zuverlässige Eingabeverarbeitung.

**Grundsatz:** Der Window Manager bestimmt, welches Fenster über welchem liegt. Der Compositor stellt diese Reihenfolge dar.