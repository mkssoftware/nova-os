# NPSPEC-SHELL-SYSTEM-0005 – Nova Global Command Bar

**Status:** Angenommen  
**Bereich:** Shell / System / Global Command Bar  
**System:** NovaOS

## 1. Zweck

Nova Global Command Bar definiert eine optionale, systemweite Befehlsoberfläche zur schnellen Ausführung von Anwendungen, Capabilities, Aktionen und Systemfunktionen.

Sie ermöglicht dem Benutzer, Aufgaben direkt über Befehle oder natürliche Sprache zu formulieren, ohne zunächst die zuständige Anwendung öffnen zu müssen.

Die Global Command Bar MUSS eine Ergänzung zur klassischen Bedienung darstellen und DARF diese nicht ersetzen.

**Grundprinzip:** Der Benutzer beschreibt, was er erreichen möchte. NovaOS ermittelt die passende Funktion und führt sie nach den geltenden Berechtigungen aus.

## 2. Architektur

Das Subsystem besteht aus folgenden Komponenten:

| Komponente | Aufgabe |
|---|---|
| Command Bar Manager | Verwaltung und Lebenszyklus |
| Command Bar View | Darstellung und Benutzerinteraktion |
| Command Registry | Registrierung verfügbarer Befehle |
| Command Resolver | Auflösung von Befehlen und Intents |
| Command Context Provider | Bereitstellung des aktuellen Arbeitskontextes |
| Command Execution Controller | Koordination der Befehlsausführung |
| Command History Manager | Optionale Verwaltung des Befehlsverlaufs |

Die Global Command Bar verwendet das native Nova UI Framework.

Die eigentliche Ausführung MUSS durch die zuständigen Systemdienste, Anwendungen oder Capabilities erfolgen.

## 3. Aktivierung und Bedienung

Die Global Command Bar MUSS über folgende Wege erreichbar sein können:

- Konfigurierbare Tastenkombination.
- Optionales Symbol in der Desktop Shell.
- Startmenü.
- Autorisierte Shell-Aktionen.

Dabei gilt:

- Die Command Bar MUSS vollständig deaktivierbar sein.
- Sämtliche grundlegenden NovaOS-Funktionen MÜSSEN auch ohne Command Bar erreichbar bleiben.
- Die Aktivierung DARF laufende Anwendungen nicht unterbrechen.
- Der Benutzer MUSS die Command Bar jederzeit schließen können.
- Die Oberfläche SOLL schnell und ohne unnötige Animationen erscheinen.

## 4. Befehlsmodell

Ein registrierter Befehl besitzt mindestens:

- Eindeutige Befehls-ID.
- Anzeigename.
- Beschreibung.
- Eigentümeridentität.
- Zugehörige Capability oder Aktion.
- Parameterschema.
- Erforderliche Berechtigungen.

Optional können enthalten sein:

- Schlüsselwörter und Synonyme.
- Kontextbedingungen.
- Standardparameter.
- Ergebnistyp.
- Vorschauinformationen.
- Rückgängig-Funktion.

Dabei gilt:

- Befehle MÜSSEN strukturiert registriert werden.
- Befehlsidentitäten MÜSSEN unabhängig von ihrer sprachlichen Bezeichnung sein.
- Parameter MÜSSEN typisiert und validierbar sein.
- Nicht verfügbare Befehle DÜRFEN nicht als ausführbar dargestellt werden.

## 5. Befehlseingabe und Interpretation

Die Command Bar unterstützt:

- Direkte Befehlsnamen.
- Schlüsselwörter.
- Suchbegriffe.
- Parameterisierte Befehle.
- Natürliche Sprache.
- Kontextbezogene Aktionen.

Dabei gilt:

- Eine deterministische Befehlsauflösung MUSS ohne KI funktionieren.
- Natürliche Sprache MUSS mindestens über registrierte Begriffe, Synonyme und definierte Regeln verarbeitet werden können.
- Eine KI-basierte Intent-Erkennung DARF optional ergänzt werden.
- Mehrdeutige Befehle MÜSSEN eine Auswahl oder Rückfrage ermöglichen.
- Befehle DÜRFEN nicht allein aufgrund einer unsicheren Interpretation ausgeführt werden.

## 6. Kontextintegration

Die Command Bar DARF den aktuellen Arbeitskontext berücksichtigen.

Dazu gehören:

- Aktive Anwendung.
- Aktives Fenster.
- Ausgewählte Dateien oder Objekte.
- Aktueller Workspace.
- Verfügbare Capabilities.
- Semantische Datentypen.

Dabei gilt:

- Kontextinformationen MÜSSEN über autorisierte Schnittstellen bereitgestellt werden.
- Kontextabhängige Befehle MÜSSEN ihre Voraussetzungen deklarieren.
- Nicht verfügbare Aktionen MÜSSEN deaktiviert oder ausgeblendet werden.
- Die Command Bar DARF keine geschützten Anwendungsdaten ohne Berechtigung auslesen.
- Kontextinformationen DÜRFEN nicht ohne Zustimmung an externe Dienste übertragen werden.

## 7. Capability-Integration

Die Global Command Bar MUSS das Capability-basierte Bedienkonzept von NovaOS unterstützen.

Dabei gilt:

- Capabilities MÜSSEN ausführbare Befehle registrieren können.
- Befehle MÜSSEN unabhängig von ihrer konkreten Implementierung aufrufbar sein.
- Mehrere geeignete Capability-Implementierungen MÜSSEN nach definierten Regeln aufgelöst werden können.
- Verkettete Aktionen DÜRFEN über das Nova Capability- und Pipeline-System ausgeführt werden.
- Die Command Bar DARF keine eigene Capability Runtime implementieren.

## 8. Vorschläge und Autovervollständigung

Die Command Bar SOLL während der Eingabe passende Befehle anbieten.

Dabei gilt:

- Vorschläge MÜSSEN aus autorisierten und verfügbaren Befehlen stammen.
- Exakte Übereinstimmungen SOLLEN bevorzugt werden.
- Kontextbezogene Vorschläge DÜRFEN ergänzt werden.
- Vorschläge MÜSSEN nachvollziehbar und deterministisch erzeugt werden können.
- Personalisierte Empfehlungen MÜSSEN deaktivierbar sein.
- Vorschläge DÜRFEN keine Befehle eigenständig ausführen.

## 9. Befehlsausführung

Ein Befehlsaufruf umfasst:

1. Entgegennahme der Eingabe.
2. Auflösung des Befehls.
3. Ermittlung und Validierung der Parameter.
4. Prüfung von Kontext und Berechtigungen.
5. Gegebenenfalls Bestätigung durch den Benutzer.
6. Übergabe an die zuständige Ausführungsumgebung.
7. Rückmeldung des Ergebnisses.

Dabei gilt:

- Befehle MÜSSEN asynchron ausführbar sein.
- Die UI Event Loop DARF nicht blockiert werden.
- Sicherheitskritische und destruktive Aktionen MÜSSEN erforderliche Bestätigungen berücksichtigen.
- Laufende Operationen MÜSSEN ihren Status bereitstellen können.
- Fehler MÜSSEN verständlich dargestellt werden.
- Rückgängig-Funktionen SOLLEN angeboten werden, sofern die jeweilige Operation dies unterstützt.

## 10. Darstellung und Benutzerinteraktion

Die Command Bar MUSS eine kompakte, adaptive Oberfläche bereitstellen.

Unterstützt werden:

- Eingabefeld.
- Vorschlagsliste.
- Befehlsbeschreibung.
- Parameterfelder.
- Kontextinformationen.
- Ausführungsstatus.
- Ergebnisdarstellung.

Dabei gilt:

- Die Oberfläche MUSS vollständig per Tastatur bedienbar sein.
- Die Darstellung MUSS das aktive Theme berücksichtigen.
- High Contrast und Reduced Motion MÜSSEN unterstützt werden.
- Die Command Bar DARF den Tastaturfokus nicht ungefragt übernehmen.
- Unveränderte Bereiche DÜRFEN nicht permanent neu gerendert werden.
- Damage Tracking MUSS unterstützt werden.

## 11. Verlauf und Benutzerkontrolle

Der Command History Manager verwaltet optional ausgeführte Befehle.

Dabei gilt:

- Der Befehlsverlauf MUSS deaktivierbar sein.
- Der Benutzer MUSS gespeicherte Einträge löschen können.
- Sensible Parameter DÜRFEN nicht ungeschützt gespeichert werden.
- Wiederholte Befehle MÜSSEN vor ihrer erneuten Ausführung die aktuellen Berechtigungen prüfen.
- Die Command Bar DARF aus dem Verlauf keine Aktionen eigenständig ausführen.
- Benutzereinstellungen MÜSSEN einen Shell-Neustart überstehen.

## 12. Performance und Ausfallsicherheit

Nova Global Command Bar MUSS ressourcenschonend arbeiten.

Dabei gilt:

- Die grundlegende Befehlsauflösung MUSS ohne Netzwerkverbindung funktionieren.
- Registrierte Befehlsmetadaten SOLLEN zwischengespeichert werden.
- Such- und Vorschlagsanfragen MÜSSEN asynchron verarbeitet werden.
- Veraltete Suchanfragen SOLLEN abgebrochen werden.
- Optionale Erweiterungen SOLLEN bedarfsgesteuert geladen werden.
- Fehlerhafte Befehlsanbieter DÜRFEN die Desktop Shell nicht beeinträchtigen.
- Ein Ausfall der Command Bar DARF die klassische Bedienung von NovaOS nicht einschränken.

## 13. Sicherheit und Datenschutz

Nova Global Command Bar unterliegt Nova.Trust.

Dabei gilt:

- Befehlsanbieter MÜSSEN eindeutig identifizierbar sein.
- Befehle MÜSSEN vor ihrer Ausführung autorisiert werden.
- Die Command Bar DARF keine Berechtigungen eigenständig erweitern.
- Nicht vertrauenswürdige Erweiterungen MÜSSEN isoliert werden können.
- Befehlsparameter MÜSSEN validiert werden.
- Sensible Kontextinformationen MÜSSEN geschützt bleiben.
- KI-basierte Interpretationen DÜRFEN Sicherheitsprüfungen nicht ersetzen.
- Die Command Bar MUSS vollständig offline und ohne KI funktionieren.

## 14. Normative Anforderungen

- **SYSTEM-CMD-001:** NovaOS MUSS eine optionale Global Command Bar unterstützen.
- **SYSTEM-CMD-002:** Die klassische Bedienung MUSS ohne Command Bar vollständig möglich bleiben.
- **SYSTEM-CMD-003:** Befehle MÜSSEN strukturiert registrierbar sein.
- **SYSTEM-CMD-004:** Die Befehlsauflösung MUSS deterministisch und ohne KI funktionieren.
- **SYSTEM-CMD-005:** Anwendungen, Solutions und Capabilities MÜSSEN integrierbar sein.
- **SYSTEM-CMD-006:** Befehlsparameter MÜSSEN typisiert und validiert werden.
- **SYSTEM-CMD-007:** Kontextbezogene Befehle MÜSSEN unterstützt werden.
- **SYSTEM-CMD-008:** Mehrdeutige Befehle DÜRFEN nicht ungeprüft ausgeführt werden.
- **SYSTEM-CMD-009:** Befehlsausführungen MÜSSEN autorisiert werden.
- **SYSTEM-CMD-010:** Die Command Bar DARF keine eigene Prozess- oder Capability Runtime implementieren.
- **SYSTEM-CMD-011:** Die Ausführung MUSS asynchron möglich sein.
- **SYSTEM-CMD-012:** Vorschläge DÜRFEN keine eigenständige Ausführung auslösen.
- **SYSTEM-CMD-013:** Der Befehlsverlauf MUSS deaktivierbar und löschbar sein.
- **SYSTEM-CMD-014:** Die Darstellung MUSS das native Nova UI Framework verwenden.
- **SYSTEM-CMD-015:** Damage Tracking und Accessibility MÜSSEN unterstützt werden.
- **SYSTEM-CMD-016:** Nova Global Command Bar MUSS vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 15. Integration

- **Nova Desktop Shell Architecture:** Integration in die Desktopumgebung.
- **Nova Start Menu:** Alternativer Einstiegspunkt.
- **Nova Application Launcher:** Starten von Anwendungen und Solutions.
- **Nova Application Search:** Suche und Befehlsvorschläge.
- **Nova Capability System:** Bereitstellung und Ausführung von Funktionen.
- **Nova Command System:** Strukturierte Befehle und Aktionen.
- **Nova Semantic Types:** Typisierte Parameter und Kontextdaten.
- **Nova Data Transfer:** Übergabe ausgewählter Objekte.
- **Nova Window Manager:** Aktiver Fenster- und Workspace-Kontext.
- **Nova UI Framework:** Darstellung und Benutzerinteraktion.
- **Nova System Dialog Integration:** Rückfragen und Bestätigungen.
- **Nova Notification Center:** Ergebnis- und Statusmeldungen.
- **Nova Agent Runtime:** Optionale komplexe Aufgabenplanung.
- **Nova.Trust:** Identität, Berechtigungen und Sicherheit.

## 16. Ergebnis

NovaOS erhält eine optionale, systemweite Befehlsoberfläche, über die Anwendungen, Solutions, Capabilities und Systemfunktionen schnell gefunden und ausgeführt werden können.

Die Command Bar verbindet klassische Befehle mit kontextabhängigen Aktionen und optionaler natürlichsprachlicher Interpretation.

Sie erweitert die Bedienmöglichkeiten von NovaOS, ohne bestehende Bedienkonzepte zu ersetzen oder eine KI-Abhängigkeit einzuführen.

**Grundsatz:** Die Global Command Bar bietet einen direkten Weg vom Benutzerwunsch zur passenden Funktion. Sie ist ein zusätzliches Werkzeug – niemals eine Voraussetzung für die Bedienung von NovaOS.