# NPSPEC-SHELL-TASKBAR-0005 – Nova System Tray

**Status:** Angenommen  
**Bereich:** Shell / Taskbar / System Tray  
**System:** NovaOS

## 1. Zweck

Nova System Tray definiert den Infobereich der NovaOS-Taskleiste zur Darstellung von Systemzuständen, Hintergrunddiensten und registrierten Anwendungen.

Ziel ist ein zentraler, unaufdringlicher und individuell anpassbarer Bereich für Statusinformationen und Schnellaktionen.

Der System Tray MUSS unabhängig von der eigentlichen Ausführung der registrierten Dienste und Anwendungen arbeiten.

## 2. Architektur

Das Subsystem besteht aus folgenden Komponenten:

| Komponente | Aufgabe |
|---|---|
| Tray Manager | Verwaltung des System Tray |
| Tray Registry | Registrierung von Tray-Einträgen |
| Tray Item Manager | Verwaltung und Aktualisierung der Einträge |
| Tray Status Provider | Bereitstellung von Statusinformationen |
| Tray Interaction Controller | Verarbeitung von Benutzeraktionen |
| Tray Layout Manager | Anordnung und Sichtbarkeit |
| Tray State Manager | Speicherung der Einstellungen |

Die Darstellung erfolgt über das native Nova UI Framework.

## 3. Tray-Elemente

Der System Tray unterstützt:

- Systemstatusanzeigen.
- Hintergrunddienste.
- Anwendungsstatus.
- Capability-Status.
- Schnellaktionen.
- Temporäre Statusindikatoren.
- Registrierte Shell-Erweiterungen.

Jeder Eintrag besitzt mindestens:

- Eindeutige ID.
- Eigentümeridentität.
- Symbol.
- Anzeigename.
- Status.
- Optionale Aktionen.
- Sichtbarkeitseinstellung.

Ein Tray-Eintrag MUSS eindeutig seinem registrierten Eigentümer zugeordnet sein.

## 4. Registrierung

Anwendungen, Dienste und Capabilities können Tray-Einträge über definierte Schnittstellen registrieren.

Dabei gilt:

- Die Registrierung MUSS über den Tray Manager erfolgen.
- Einträge MÜSSEN eindeutig identifizierbar sein.
- Anwendungen DÜRFEN ausschließlich eigene Einträge verändern.
- Registrierung und Entfernung MÜSSEN dynamisch möglich sein.
- Ungültige oder verwaiste Einträge MÜSSEN automatisch bereinigt werden können.
- Die Registrierung DARF keine zusätzlichen Systemberechtigungen verleihen.

## 5. Systemstatus

NovaOS SOLL standardisierte Statusanzeigen bereitstellen für:

- Netzwerkverbindung.
- Lautstärke und Audiogeräte.
- Energieversorgung und Akku.
- Datum und Uhrzeit.
- Sicherheitsstatus.
- Angeschlossene Geräte.
- Hintergrundaktivitäten.

Dabei gilt:

- Statusinformationen MÜSSEN von den zuständigen Systemdiensten stammen.
- Der Tray DARF Systemzustände nicht selbst ermitteln oder verwalten müssen.
- Statusänderungen MÜSSEN ereignisgesteuert übertragen werden.
- Nicht verfügbare Systemfunktionen SOLLEN ausgeblendet oder entsprechend gekennzeichnet werden.

## 6. Benutzerinteraktion

Tray-Einträge unterstützen:

- Einfachklick.
- Doppelklick.
- Kontextmenüs.
- Tastaturaktivierung.
- Tooltips.
- Optionale Schnellbedienelemente.

Dabei gilt:

- Aktionen MÜSSEN über definierte Commands oder Capabilities ausgeführt werden.
- Kontextmenüs MÜSSEN das Nova Shell Context Menu System verwenden.
- Systemkritische Aktionen MÜSSEN erforderliche Berechtigungen prüfen.
- Alle grundlegenden Funktionen MÜSSEN per Tastatur erreichbar sein.

## 7. Sichtbarkeit und Anordnung

Der Benutzer MUSS die Darstellung der Tray-Einträge anpassen können.

Unterstützt werden:

- Dauerhaft sichtbare Einträge.
- Ausgeblendete Einträge.
- Automatische Sichtbarkeit bei relevanten Zuständen.
- Benutzerdefinierte Reihenfolge.
- Erweiterter Infobereich.

Dabei gilt:

- Benutzerdefinierte Einstellungen MÜSSEN Vorrang vor automatischen Vorschlägen haben.
- Sicherheitskritische Hinweise DÜRFEN nicht unbemerkt unterdrückt werden.
- Anwendungen DÜRFEN ihre Sichtbarkeit nicht eigenmächtig erzwingen.
- Die Anordnung MUSS bei Größenänderungen der Taskleiste angepasst werden.

## 8. Statusänderungen und Aufmerksamkeit

Tray-Einträge können unterschiedliche Zustände darstellen:

| Zustand | Bedeutung |
|---|---|
| Normal | Regulärer Betriebszustand |
| Active | Aktive Hintergrundfunktion |
| Busy | Laufender Vorgang |
| Warning | Zustand erfordert Aufmerksamkeit |
| Error | Fehlerzustand |
| Disabled | Funktion nicht verfügbar |

Dabei gilt:

- Statusänderungen MÜSSEN nachvollziehbar sein.
- Animationen SOLLEN nur bei tatsächlichem Bedarf verwendet werden.
- Dauerhaft blinkende oder aufdringliche Anzeigen SOLLEN vermieden werden.
- Benachrichtigungen MÜSSEN über das Nova Notification System erfolgen.
- Der System Tray DARF keine konkurrierende Benachrichtigungsverwaltung implementieren.

## 9. Mehrmonitorbetrieb

Der System Tray MUSS Mehrmonitorbetrieb berücksichtigen.

Dabei gilt:

- Der Tray MUSS auf einer primären Taskleiste verfügbar sein.
- Eine Anzeige auf weiteren Taskleisten DARF konfigurierbar sein.
- Tray-Einträge MÜSSEN unabhängig von der Anzahl ihrer Darstellungen verwaltet werden.
- Aktionen DÜRFEN durch mehrere Darstellungen nicht mehrfach ausgeführt werden.
- Monitoränderungen MÜSSEN dynamisch verarbeitet werden.

## 10. Performance

Der System Tray MUSS ressourcenschonend arbeiten.

Dabei gilt:

- Statusänderungen MÜSSEN ereignisgesteuert verarbeitet werden.
- Unveränderte Symbole DÜRFEN nicht permanent neu gerendert werden.
- Damage Tracking MUSS berücksichtigt werden.
- Symbolressourcen SOLLEN zwischengespeichert werden.
- Langsame Status Provider DÜRFEN die UI Event Loop nicht blockieren.
- Aktualisierungsraten MÜSSEN bei Bedarf begrenzbar sein.
- Nicht sichtbare Einträge SOLLEN keine unnötige Renderingaktivität verursachen.

## 11. Zustand und Wiederherstellung

Der Tray State Manager speichert:

- Sichtbarkeitseinstellungen.
- Reihenfolge der Einträge.
- Benutzerdefinierte Anzeigeoptionen.
- Monitorbezogene Einstellungen.

Dabei gilt:

- Einstellungen MÜSSEN benutzerbezogen gespeichert werden.
- Ein Shell-Neustart DARF die Konfiguration nicht verlieren.
- Laufende Dienste und Anwendungen MÜSSEN ihre Tray-Einträge erneut bereitstellen können.
- Der Tray Manager SOLL vorhandene Registrierungen nach einem Neustart wiederherstellen können.
- Fehlerhafte Einträge DÜRFEN den Start der Taskleiste nicht verhindern.

## 12. Sicherheit und Barrierefreiheit

Nova System Tray unterliegt Nova.Trust und der Nova Accessibility Architecture.

Dabei gilt:

- Tray-Einträge MÜSSEN ihrem registrierten Eigentümer zugeordnet bleiben.
- Anwendungen DÜRFEN keine fremden Systemstatusanzeigen manipulieren.
- Geschützte Systemaktionen MÜSSEN autorisiert werden.
- Fehlerhafte Provider DÜRFEN die Desktop Shell nicht beeinträchtigen.
- Symbole und Zustände MÜSSEN für Screenreader verfügbar sein.
- High Contrast und Reduced Motion MÜSSEN unterstützt werden.
- Der System Tray MUSS vollständig offline funktionieren.

## 13. Normative Anforderungen

- **TASKBAR-TRAY-001:** NovaOS MUSS einen nativen System Tray bereitstellen.
- **TASKBAR-TRAY-002:** Anwendungen, Dienste und Capabilities MÜSSEN Tray-Einträge registrieren können.
- **TASKBAR-TRAY-003:** Tray-Einträge MÜSSEN eindeutige Identitäten besitzen.
- **TASKBAR-TRAY-004:** Einträge DÜRFEN nur durch autorisierte Eigentümer verändert werden.
- **TASKBAR-TRAY-005:** Systemstatus MÜSSEN über zuständige Systemdienste bereitgestellt werden.
- **TASKBAR-TRAY-006:** Statusänderungen MÜSSEN ereignisgesteuert verarbeitet werden.
- **TASKBAR-TRAY-007:** Sichtbarkeit und Reihenfolge MÜSSEN benutzerdefiniert anpassbar sein.
- **TASKBAR-TRAY-008:** Tray-Aktionen MÜSSEN das Command- und Capability-System verwenden können.
- **TASKBAR-TRAY-009:** Benachrichtigungen MÜSSEN über das Nova Notification System erfolgen.
- **TASKBAR-TRAY-010:** Mehrmonitorbetrieb MUSS berücksichtigt werden.
- **TASKBAR-TRAY-011:** Die Darstellung MUSS Damage Tracking unterstützen.
- **TASKBAR-TRAY-012:** Einstellungen MÜSSEN gespeichert und wiederhergestellt werden können.
- **TASKBAR-TRAY-013:** Fehlerhafte Provider DÜRFEN die Shell nicht beeinträchtigen.
- **TASKBAR-TRAY-014:** Nova.Trust und Accessibility MÜSSEN berücksichtigt werden.
- **TASKBAR-TRAY-015:** Nova System Tray MUSS vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 14. Integration

- **Nova Taskbar Architecture:** Einbindung in die Taskleiste.
- **Nova Desktop Shell Architecture:** Übergeordnete Shell-Verwaltung.
- **Nova Shell Extension Model:** Erweiterbare Tray-Funktionen.
- **Nova Shell Recovery:** Wiederherstellung nach Fehlern.
- **Nova UI Framework:** Darstellung und Interaktion.
- **Nova Compositor:** Rendering und Damage Tracking.
- **Nova Notification System:** Benachrichtigungen.
- **Nova Capability System:** Statusaktionen und Schnellfunktionen.
- **Nova System Services:** Bereitstellung von Systemzuständen.
- **Nova.Trust:** Sicherheit und Berechtigungen.

## 15. Ergebnis

NovaOS erhält einen einheitlichen und erweiterbaren System Tray zur Darstellung von Systemzuständen, Hintergrunddiensten und Anwendungsfunktionen.

Statusinformationen werden ereignisgesteuert bereitgestellt und können vom Benutzer individuell angeordnet werden.

Durch die Trennung von Statusbereitstellung, Darstellung und Aktionsausführung bleibt der System Tray modular, sicher und ressourcenschonend.

**Grundsatz:** Der System Tray informiert über relevante Zustände und bietet schnellen Zugriff auf Funktionen, ohne den Benutzer unnötig abzulenken.