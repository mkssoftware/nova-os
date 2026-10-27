# NPSPEC-WINDOW-MANAGEMENT-0007 – Nova Modal Window Management

**Status:** Angenommen  
**Bereich:** Window Manager / Modale Fensterverwaltung  
**System:** NovaOS

## 1. Zweck

Nova Modal Window Management definiert die Verwaltung modaler Fenster und Dialoge innerhalb der NovaOS-Desktopumgebung.

Ziel ist eine sichere, konsistente und benutzerfreundliche Steuerung von Interaktionen, bei denen bestimmte Fenster vorübergehend die Bedienung anderer Fenster einschränken.

## 2. Architektur

- Zentrale Modalitätsverwaltung durch den Nova Window Manager.
- Integration in Window Focus Management und Z-Order Management.
- Unterstützung anwendungsbezogener und fensterbezogener Modalität.
- Trennung zwischen Modalitätsregeln und grafischer Darstellung.
- Capability-basierte Autorisierung über Nova.Trust.
- Unterstützung verschachtelter modaler Dialoge.
- Ereignisgesteuerte Aktivierung und Aufhebung der Modalität.
- Asynchrone Kommunikation über Nova IPC und Display Protocol.

## 3. Modalitätsmodell

NovaOS unterscheidet folgende Modalitätsarten:

| Modalität | Beschreibung |
|---|---|
| Window Modal | Blockiert die Interaktion mit einem bestimmten Besitzerfenster und dessen betroffener Fenstergruppe |
| Application Modal | Blockiert die reguläre Interaktion mit Fenstern derselben Anwendung |
| System Modal | Beschränkt systemweit bestimmte Interaktionen; ausschließlich für autorisierte Systemkomponenten |
| Non-Modal | Keine Einschränkung anderer Fenster |

Modalität ist eine Eigenschaft des Fensters und kein eigenständiger Lebenszykluszustand.

Ein modales Fenster bleibt Bestandteil der regulären Fensterverwaltung.

## 4. Kernfunktionen

- Erstellung modaler Fensterbeziehungen.
- Festlegung des Modalitätsbereichs.
- Blockierung unzulässiger Eingaben.
- Automatische Aktivierung modaler Dialoge.
- Verwaltung verschachtelter Dialoge.
- Wiederherstellung vorheriger Fokuszustände.
- Integration in die Fensterstapelverwaltung.
- Aufhebung der Modalität bei Dialogabschluss.
- Automatische Bereinigung bei Anwendungsabstürzen.
- Unterstützung standardisierter NovaOS-Systemdialoge.

## 5. Normative Anforderungen

- WINDOW-001: Modalitätsregeln MÜSSEN zentral durch den Window Manager verwaltet werden.
- WINDOW-002: Jedes modale Fenster MUSS einen eindeutig definierten Modalitätsbereich besitzen.
- WINDOW-003: Anwendungen DÜRFEN ausschließlich autorisierte Modalitätsbereiche festlegen.
- WINDOW-004: Systemweite Modalität MUSS privilegierten Systemkomponenten vorbehalten bleiben.
- WINDOW-005: Modale Fenster MÜSSEN innerhalb ihres Geltungsbereichs die reguläre Eingabe blockierter Fenster verhindern.
- WINDOW-006: Nicht betroffene Anwendungen MÜSSEN weiterhin bedienbar bleiben.
- WINDOW-007: Modale Fenster MÜSSEN innerhalb ihrer zulässigen Stapelgruppe über den blockierten Fenstern angeordnet werden.
- WINDOW-008: Fokusänderungen MÜSSEN bestehende Modalitätsbeschränkungen berücksichtigen.
- WINDOW-009: Verschachtelte Modalität MUSS unterstützt werden.
- WINDOW-010: Zyklische oder widersprüchliche Modalitätsbeziehungen MÜSSEN verhindert werden.
- WINDOW-011: Das Schließen eines modalen Fensters MUSS dessen Modalitätsbeschränkungen zuverlässig aufheben.
- WINDOW-012: Bei Anwendungsabsturz MÜSSEN zugehörige Modalitätsbeziehungen automatisch bereinigt werden.
- WINDOW-013: Modale Fenster DÜRFEN keine Sicherheitsgrenzen umgehen.
- WINDOW-014: Die Modalitätsverwaltung DARF den Window Manager nicht durch blockierende Anwendungsaufrufe anhalten.
- WINDOW-015: Die Fokuswiederherstellung MUSS nach Aufhebung der Modalität anhand gültiger Fensterzustände erfolgen.
- WINDOW-016: Modalitätsänderungen MÜSSEN atomar verarbeitet werden.
- WINDOW-017: Geschützte Systeminteraktionen MÜSSEN gegenüber regulären Anwendungsdialogen durchsetzbar bleiben.
- WINDOW-018: Modalitätsregeln MÜSSEN unabhängig von Animationen und KI deterministisch funktionieren.

## 6. Modale Fensterbeziehungen

Jedes modale Fenster besitzt mindestens folgende Informationen:

| Eigenschaft | Beschreibung |
|---|---|
| Window ID | Identität des modalen Fensters |
| Owner ID | Besitzer der Fensterressource |
| Parent Window | Zugeordnetes Besitzerfenster |
| Modality Type | Art der Modalität |
| Modal Scope | Betroffener Fensterbereich |
| Previous Focus | Vorheriges Fokusziel |
| Modal Parent | Optional übergeordneter modaler Dialog |
| State Version | Version der Modalitätskonfiguration |

Modalitätsbeziehungen werden unabhängig von grafischen Eltern-Kind-Beziehungen verwaltet.

Die Existenz einer Fensterhierarchie begründet keine automatische Modalitätsberechtigung.

## 7. Aktivierung modaler Fenster

Die Aktivierung erfolgt in folgenden Schritten:

1. **Request:** Anwendung fordert modale Darstellung an.
2. **Authorize:** Sicherheitskontext und Modalitätsberechtigung prüfen.
3. **Validate:** Besitzerfenster und Modalitätsbereich prüfen.
4. **Resolve:** Betroffene Fenster und Fokusregeln bestimmen.
5. **Register:** Modalitätsbeziehung atomar registrieren.
6. **Activate:** Modales Fenster zur Darstellung freigeben.
7. **Focus:** Zulässigen Eingabefokus zuweisen.
8. **Notify:** Betroffene Komponenten informieren.

Die Modalitätsbeschränkung MUSS wirksam sein, bevor das modale Fenster reguläre Eingaben entgegennimmt.

## 8. Verschachtelte Modalität

NovaOS unterstützt mehrere aufeinanderfolgende modale Dialoge.

Beispiel:

- Hauptfenster öffnet einen Einstellungsdialog.
- Einstellungsdialog öffnet eine Dateiauswahl.
- Dateiauswahl öffnet einen Bestätigungsdialog.

Dabei gelten folgende Regeln:

- Jeder Dialog besitzt einen eindeutig definierten Modalitätsbereich.
- Der jeweils oberste zulässige Dialog erhält den Eingabefokus.
- Übergeordnete Dialoge bleiben während der untergeordneten Modalität blockiert.
- Beim Schließen eines Dialogs werden ausschließlich dessen eigene Modalitätsbeschränkungen aufgehoben.
- Verbleibende Modalitätsbeziehungen bleiben gültig.
- Zirkuläre Abhängigkeiten sind unzulässig.

## 9. Aufhebung und Fehlerbehandlung

Beim Schließen eines modalen Fensters:

1. Modalitätsbeziehung deaktivieren.
2. Betroffene Fenster wieder für reguläre Eingaben freigeben, sofern keine weiteren Beschränkungen bestehen.
3. Fenster aus der modalen Stapelgruppe entfernen.
4. Vorheriges Fokusziel validieren.
5. Geeignetes Fenster aktivieren.
6. Modalitätsressourcen freigeben.

Zusätzlich gelten folgende Regeln:

- Nicht reagierende Dialoge DÜRFEN andere Anwendungen nicht dauerhaft blockieren.
- Der Benutzer MUSS blockierende Anwendungsdialoge über autorisierte Systemfunktionen beenden können.
- Bei Entfernung eines Besitzerfensters MÜSSEN abhängige Modalitätsbeziehungen konsistent aufgelöst werden.
- Ungültige Modalitätsreferenzen werden zurückgewiesen.
- Ein Compositor-Neustart verändert bestehende Modalitätsregeln nicht.
- Sicherheitskritische Systemdialoge bleiben auch bei Fehlern regulärer Anwendungen geschützt.

## 10. Ergebnis

NovaOS erhält eine sichere und deterministische Verwaltung modaler Fenster.

Dialoge können gezielt einzelne Fenster, Anwendungen oder autorisierte Systeminteraktionen einschränken, ohne unnötig den gesamten Desktop zu blockieren.

Verschachtelte Dialoge, Fokuswiederherstellung und automatische Fehlerbereinigung gewährleisten eine zuverlässige Bedienung.

**Grundsatz:** Modalität beschränkt nur die Interaktionen, die für den jeweiligen Vorgang tatsächlich erforderlich sind.