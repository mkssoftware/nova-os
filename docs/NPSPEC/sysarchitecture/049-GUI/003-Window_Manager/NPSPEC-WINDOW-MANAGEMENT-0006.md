# NPSPEC-WINDOW-MANAGEMENT-0006 – Nova Window Focus Management

**Status:** Angenommen  
**Bereich:** Window Manager / Fokusverwaltung  
**System:** NovaOS

## 1. Zweck

Nova Window Focus Management definiert die Verwaltung des Eingabefokus und der Fensteraktivierung innerhalb der NovaOS-Desktopumgebung.

Ziel ist eine vorhersehbare, sichere und benutzerfreundliche Fokussteuerung, die unbeabsichtigte Fokuswechsel verhindert und die Kontrolle des Benutzers gewährleistet.

## 2. Architektur

- Zentrale Fokusverwaltung durch den Nova Window Manager.
- Trennung zwischen Fensteraktivierung und Tastaturfokus.
- Integration in Nova Input System und Display Protocol.
- Capability-basierte Kontrolle von Fokusanforderungen.
- Unterstützung mehrerer Eingabegeräte und Eingabekontexte.
- Berücksichtigung von Fensterhierarchien und modalen Dialogen.
- Integration in Window State Model und Z-Order Management.
- Ereignisgesteuerte und transaktionale Fokusänderungen.

## 3. Fokusmodell

NovaOS unterscheidet folgende Zustände:

| Zustand | Beschreibung |
|---|---|
| Active | Aktives Fenster eines Eingabekontexts |
| Inactive | Nicht aktives Fenster |
| Focused | Fenster mit zugewiesenem Tastaturfokus |
| Unfocused | Fenster ohne Tastaturfokus |
| Attention Requested | Fenster fordert Aufmerksamkeit an |

Aktivierung und Tastaturfokus sind getrennte Eigenschaften.

Ein aktives Fenster besitzt nicht zwangsläufig den Tastaturfokus, beispielsweise während einer systemseitigen Eingabeinteraktion.

Der Fokus innerhalb eines Fensters wird durch das jeweilige UI-System verwaltet.

## 4. Kernfunktionen

- Aktivieren und Deaktivieren von Fenstern.
- Zuweisen und Entziehen des Tastaturfokus.
- Fokuswechsel durch Maus, Touch und Tastatur.
- Fokuswechsel zwischen Anwendungen.
- Wiederherstellung vorheriger Fokuszustände.
- Verwaltung modaler Fokusbeschränkungen.
- Unterstützung von Fokusanforderungen durch Anwendungen.
- Schutz vor unberechtigter Fokusübernahme.
- Unterstützung mehrerer Eingabekontexte.
- Benachrichtigung über Fokusänderungen.

## 5. Normative Anforderungen

- WINDOW-001: Der Window Manager MUSS den verbindlichen Fensterfokus zentral verwalten.
- WINDOW-002: Fensteraktivierung und Tastaturfokus MÜSSEN getrennt verwaltet werden.
- WINDOW-003: Pro Eingabekontext DARF höchstens ein Fenster den Tastaturfokus besitzen.
- WINDOW-004: Fokusänderungen MÜSSEN atomar erfolgen.
- WINDOW-005: Anwendungen DÜRFEN den Tastaturfokus nicht eigenmächtig übernehmen.
- WINDOW-006: Fokusanforderungen MÜSSEN anhand von Sicherheitsrichtlinien und Benutzerinteraktionen geprüft werden.
- WINDOW-007: Benutzerinitiierte Fokuswechsel MÜSSEN Vorrang vor konkurrierenden automatischen Fokusanforderungen besitzen.
- WINDOW-008: Minimierte, ausgeblendete oder zerstörte Fenster DÜRFEN keinen regulären Tastaturfokus besitzen.
- WINDOW-009: Modale Dialoge MÜSSEN die geltenden Fokusbeschränkungen durchsetzen.
- WINDOW-010: Fokuswechsel MÜSSEN dem Input System konsistent mitgeteilt werden.
- WINDOW-011: Anwendungen MÜSSEN über relevante Fokusänderungen informiert werden.
- WINDOW-012: Fokusänderungen DÜRFEN nicht automatisch eine unzulässige Z-Order-Änderung auslösen.
- WINDOW-013: Geschützte Systemoberflächen MÜSSEN gegen unberechtigte Fokusübernahme geschützt werden.
- WINDOW-014: Nicht reagierende Anwendungen DÜRFEN den Fokuswechsel zu anderen Fenstern nicht blockieren.
- WINDOW-015: Nach Fensterentfernung MUSS der Fokus anhand definierter Regeln neu bestimmt werden.
- WINDOW-016: Veraltete Fokusereignisse DÜRFEN neuere Fokuszustände nicht überschreiben.
- WINDOW-017: Fokuswechsel MÜSSEN ohne visuelle Animationen funktionieren.
- WINDOW-018: Die Fokusverwaltung MUSS ohne KI-Abhängigkeit deterministisch arbeiten.

## 6. Fokusverfahren

### Click to Focus

Ein Fenster erhält durch eine gültige Benutzerinteraktion die Aktivierung und gegebenenfalls den Tastaturfokus.

### Keyboard Focus

Der Benutzer wechselt über definierte Tastaturbefehle zwischen Fenstern.

### Programmatic Focus Request

Eine Anwendung fordert die Aktivierung ihres Fensters an.

Die tatsächliche Aktivierung entscheidet der Window Manager anhand der geltenden Sicherheits- und Fokusregeln.

### Modal Focus

Ein modaler Dialog beschränkt die Fokusvergabe innerhalb seines definierten Geltungsbereichs.

### Focus Restoration

Nach dem Schließen, Minimieren oder Ausblenden eines Fensters wird ein geeignetes vorheriges Fenster aktiviert.

### Attention Request

Eine Anwendung kann Aufmerksamkeit anfordern, ohne den aktuellen Fokus zu übernehmen.

## 7. Fokusprioritäten

NovaOS verwendet folgende Prioritätsregeln:

1. Geschützte Systeminteraktionen.
2. Explizite Benutzerinteraktionen.
3. Gültige modale Fokusbeschränkungen.
4. Autorisierte systemseitige Fokusanforderungen.
5. Reguläre Anwendungsanforderungen.
6. Automatische Fokuswiederherstellung.

Eine höhere Priorität begründet keine zusätzlichen Zugriffsrechte.

Die Sicherheitsprüfung bleibt für jede Fokusoperation verbindlich.

## 8. Fokuswechsel

Ein Fokuswechsel erfolgt in folgenden Schritten:

1. **Request:** Fokusanforderung empfangen.
2. **Identify:** Eingabekontext und Zielfenster bestimmen.
3. **Authorize:** Berechtigungen und Benutzerinteraktion prüfen.
4. **Validate:** Fensterzustand und modale Beschränkungen prüfen.
5. **Resolve:** Zulässiges Fokusziel bestimmen.
6. **Commit:** Fokuszustand atomar aktualisieren.
7. **Notify:** Input System und betroffene Anwendungen informieren.
8. **Update:** Visuelle Fokuskennzeichnung aktualisieren.

Der bisherige Fokus bleibt erhalten, wenn die Anforderung nicht zulässig ist.

## 9. Fokuswiederherstellung und Fehlerbehandlung

- Nach dem Schließen eines aktiven Fensters wird ein geeignetes vorheriges Fenster berücksichtigt.
- Minimierte und ausgeblendete Fenster werden bei der regulären Fokusvergabe ausgeschlossen.
- Bei Workspace-Wechsel wird der Fokus innerhalb des neuen Workspaces bestimmt.
- Beim Entfernen eines Displays werden betroffene Fokuszuordnungen aktualisiert.
- Ungültige Fokusreferenzen werden zurückgewiesen.
- Bei Anwendungsabsturz wird der Fokus kontrolliert freigegeben.
- Ein Compositor-Neustart verändert den verbindlichen Fokuszustand nicht.
- Ist kein geeignetes Fenster verfügbar, darf der Tastaturfokus unbelegt bleiben.
- Fokusereignisse werden in einer definierten Reihenfolge verarbeitet.

## 10. Ergebnis

NovaOS erhält eine zentrale, sichere und deterministische Fokusverwaltung.

Fensteraktivierung und Tastaturfokus werden unabhängig voneinander gesteuert, während Benutzerinteraktionen, modale Dialoge und Sicherheitsrichtlinien zuverlässig berücksichtigt werden.

Anwendungen können Aufmerksamkeit anfordern, ohne den Benutzer durch unerwartete Fokuswechsel zu unterbrechen.

**Grundsatz:** Der Benutzer entscheidet, womit er arbeitet. Anwendungen dürfen Aufmerksamkeit anfordern, aber nicht ungefragt übernehmen.