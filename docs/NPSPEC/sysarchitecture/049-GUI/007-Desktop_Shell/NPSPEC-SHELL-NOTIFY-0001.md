# NPSPEC-SHELL-NOTIFY-0001 – Nova Notification Center

**Status:** Angenommen  
**Bereich:** Shell / Notifications  
**System:** NovaOS

## 1. Zweck

Nova Notification Center definiert die zentrale Verwaltung und Darstellung von Benachrichtigungen innerhalb der NovaOS Desktop Shell.

Ziel ist ein einheitliches, unaufdringliches und benutzerkontrolliertes Benachrichtigungssystem.

NovaOS MUSS wichtige Informationen zuverlässig bereitstellen, ohne den Benutzer bei seiner Arbeit unnötig zu unterbrechen.

## 2. Architektur

Das Subsystem besteht aus folgenden Komponenten:

| Komponente | Aufgabe |
|---|---|
| Notification Manager | Zentrale Verwaltung von Benachrichtigungen |
| Notification Registry | Registrierung autorisierter Absender |
| Notification Router | Zustellung und Verteilung |
| Notification Policy Manager | Priorisierung und Unterbrechungsregeln |
| Notification Center View | Darstellung und Benutzerinteraktion |
| Notification History Manager | Speicherung und Verlauf |
| Notification Action Controller | Ausführung von Benachrichtigungsaktionen |

Die Darstellung erfolgt über das native Nova UI Framework.

Der Notification Manager MUSS unabhängig von der grafischen Desktop Shell arbeiten können.

## 3. Benachrichtigungsmodell

Eine Benachrichtigung besitzt mindestens:

- Eindeutige ID.
- Absenderidentität.
- Titel.
- Inhalt.
- Zeitstempel.
- Priorität.
- Status.
- Optionale Aktionen.

Zusätzlich können Symbole, Fortschrittsinformationen, Gruppierungen und Ablaufbedingungen angegeben werden.

Benachrichtigungen MÜSSEN strukturiert über definierte Schnittstellen bereitgestellt werden.

## 4. Benachrichtigungstypen

NovaOS unterstützt folgende Typen:

| Typ | Verwendung |
|---|---|
| Information | Allgemeine Hinweise |
| Success | Erfolgreich abgeschlossene Vorgänge |
| Warning | Zustände, die Aufmerksamkeit benötigen |
| Error | Fehler und fehlgeschlagene Vorgänge |
| Progress | Laufende Operationen |
| Action Required | Erforderliche Benutzerentscheidung |

Der Typ beschreibt den Inhalt und DARF nicht automatisch eine hohe Unterbrechungspriorität erzwingen.

## 5. Prioritäten

Benachrichtigungen unterstützen folgende Prioritätsstufen:

- **Low:** Hintergrundinformationen ohne unmittelbaren Handlungsbedarf.
- **Normal:** Reguläre Benachrichtigungen.
- **High:** Wichtige Ereignisse mit zeitnahem Handlungsbedarf.
- **Critical:** Sicherheitskritische oder unmittelbar relevante Systemereignisse.

Dabei gilt:

- Die Priorität MUSS durch Systemrichtlinien überprüfbar sein.
- Anwendungen DÜRFEN kritische Prioritäten nicht uneingeschränkt verwenden.
- Niedrige Prioritäten SOLLEN ohne Unterbrechung im Notification Center erscheinen.
- Kritische Ereignisse MÜSSEN auch bei aktivierten Ruhefunktionen angemessen behandelt werden.

## 6. Darstellung

Benachrichtigungen können dargestellt werden als:

- Temporäre Desktop-Einblendung.
- Eintrag im Notification Center.
- Statusanzeige in der Taskleiste.
- Gruppierte Benachrichtigung.
- Fortschrittsanzeige.

Dabei gilt:

- Die Darstellung MUSS das aktive Theme berücksichtigen.
- Benachrichtigungen DÜRFEN wichtige Bedienelemente nicht dauerhaft verdecken.
- Animationen SOLLEN dezent und optional sein.
- High Contrast und Reduced Motion MÜSSEN unterstützt werden.
- Die Darstellung MUSS Damage Tracking verwenden.

## 7. Notification Center

Das Notification Center stellt eine zentrale Übersicht bereit.

Unterstützt werden:

- Anzeige aktueller Benachrichtigungen.
- Gruppierung nach Anwendung oder Kategorie.
- Chronologische Sortierung.
- Lesen und Ausblenden.
- Entfernen einzelner Einträge.
- Leeren des Verlaufs.
- Ausführen angebotener Aktionen.

Dabei gilt:

- Das Notification Center MUSS unabhängig von temporären Einblendungen verwendbar sein.
- Benachrichtigungen MÜSSEN auch nach dem Schließen einer Einblendung verfügbar bleiben können.
- Die Benutzeroberfläche MUSS vollständig per Tastatur bedienbar sein.
- Der Benutzer MUSS den Benachrichtigungsverlauf kontrollieren können.

## 8. Benachrichtigungsaktionen

Benachrichtigungen können kontextabhängige Aktionen anbieten.

Beispiele:

- Öffnen.
- Wiederholen.
- Rückgängig machen.
- Details anzeigen.
- Einstellungen öffnen.
- Bestätigen.

Dabei gilt:

- Aktionen MÜSSEN über das Nova Command- und Capability-System ausgeführt werden.
- Berechtigungen MÜSSEN vor der Ausführung geprüft werden.
- Nicht mehr verfügbare Aktionen MÜSSEN deaktiviert werden.
- Sicherheitskritische Aktionen DÜRFEN nicht ohne erforderliche Bestätigung ausgeführt werden.

## 9. Ruhemodus und Unterbrechungsregeln

NovaOS MUSS benutzerdefinierte Unterbrechungsregeln unterstützen.

Dazu gehören:

- Benachrichtigungen stummschalten.
- Zeitgesteuerter Ruhemodus.
- Anwendungsspezifische Einstellungen.
- Prioritätsabhängige Ausnahmen.
- Kontextabhängige Unterdrückung temporärer Einblendungen.

Dabei gilt:

- Der Benutzer MUSS die Regeln jederzeit verändern können.
- Unterdrückte Benachrichtigungen SOLLEN im Notification Center erhalten bleiben.
- Automatische Anpassungen MÜSSEN nachvollziehbar und abschaltbar sein.
- NovaOS DARF Benachrichtigungseinstellungen nicht eigenmächtig dauerhaft verändern.

## 10. Gruppierung und Aktualisierung

Zusammengehörige Benachrichtigungen MÜSSEN gruppierbar sein.

Dabei gilt:

- Anwendungen MÜSSEN bestehende Benachrichtigungen über ihre ID aktualisieren können.
- Fortschrittsmeldungen SOLLEN vorhandene Einträge aktualisieren statt neue zu erzeugen.
- Wiederholte identische Meldungen SOLLEN zusammengefasst werden.
- Gruppierungsregeln MÜSSEN deterministisch sein.
- Fehlerhafte oder übermäßig häufige Meldungen MÜSSEN begrenzt werden können.

## 11. Speicherung und Wiederherstellung

Der Notification History Manager verwaltet den Benachrichtigungsverlauf.

Dabei gilt:

- Benachrichtigungen MÜSSEN benutzerbezogen verwaltet werden.
- Persistente Benachrichtigungen MÜSSEN einen Shell-Neustart überstehen können.
- Temporäre Benachrichtigungen DÜRFEN nach Ablauf entfernt werden.
- Aufbewahrungsdauer und Verlauf MÜSSEN konfigurierbar sein.
- Der Benutzer MUSS gespeicherte Benachrichtigungen löschen können.
- Beschädigte Verlaufsdaten DÜRFEN den Start der Desktop Shell nicht verhindern.

## 12. Performance und Fehlerbehandlung

Das Benachrichtigungssystem MUSS ereignisgesteuert arbeiten.

Dabei gilt:

- Die Zustellung DARF die UI Event Loop nicht blockieren.
- Benachrichtigungen MÜSSEN asynchron verarbeitet werden können.
- Häufige Statusänderungen SOLLEN zusammengefasst werden.
- Nicht sichtbare Benachrichtigungen DÜRFEN keine unnötige Renderingaktivität verursachen.
- Fehlerhafte Absender DÜRFEN den Notification Manager nicht beeinträchtigen.
- Bei einem Shell-Neustart MUSS die Zustellung weiterhin möglich bleiben.

## 13. Sicherheit und Datenschutz

Nova Notification Center unterliegt Nova.Trust.

Dabei gilt:

- Absender MÜSSEN eindeutig identifizierbar sein.
- Anwendungen DÜRFEN keine fremden Benachrichtigungen manipulieren.
- Kritische Systemmeldungen MÜSSEN von gewöhnlichen Anwendungsmeldungen unterscheidbar sein.
- Sensible Inhalte MÜSSEN auf gesperrten Bildschirmen geschützt werden.
- Benachrichtigungsaktionen MÜSSEN autorisiert werden.
- Benachrichtigungsinhalte DÜRFEN nicht ohne Zustimmung an externe Dienste übertragen werden.
- Das System MUSS vollständig offline funktionieren.

## 14. Normative Anforderungen

- **NOTIFY-001:** NovaOS MUSS einen zentralen Notification Manager bereitstellen.
- **NOTIFY-002:** Der Notification Manager MUSS unabhängig von der Desktop Shell arbeiten können.
- **NOTIFY-003:** Benachrichtigungen MÜSSEN eindeutige Identitäten und autorisierte Absender besitzen.
- **NOTIFY-004:** Benachrichtigungen MÜSSEN priorisiert werden können.
- **NOTIFY-005:** Temporäre Einblendungen und ein zentraler Verlauf MÜSSEN unterstützt werden.
- **NOTIFY-006:** Benachrichtigungen MÜSSEN aktualisiert und gruppiert werden können.
- **NOTIFY-007:** Fortschrittsmeldungen SOLLEN vorhandene Einträge aktualisieren.
- **NOTIFY-008:** Benutzerdefinierte Ruhe- und Unterbrechungsregeln MÜSSEN unterstützt werden.
- **NOTIFY-009:** Aktionen MÜSSEN über autorisierte Commands und Capabilities erfolgen.
- **NOTIFY-010:** Persistente Benachrichtigungen MÜSSEN nach einem Shell-Neustart verfügbar bleiben können.
- **NOTIFY-011:** Die Darstellung MUSS Damage Tracking berücksichtigen.
- **NOTIFY-012:** Die Zustellung DARF die UI Event Loop nicht blockieren.
- **NOTIFY-013:** Sensible Benachrichtigungsinhalte MÜSSEN geschützt werden.
- **NOTIFY-014:** Das Notification Center MUSS barrierefrei bedienbar sein.
- **NOTIFY-015:** Nova Notification Center MUSS vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 15. Integration

- **Nova Desktop Shell Architecture:** Einbindung in die Desktopumgebung.
- **Nova Shell Lifecycle:** Initialisierung und Wiederherstellung.
- **Nova Taskbar Architecture:** Benachrichtigungsanzeige.
- **Nova System Tray:** Statusindikatoren.
- **Nova UI Framework:** Darstellung und Interaktion.
- **Nova Compositor:** Einblendungen und Damage Tracking.
- **Nova Capability System:** Benachrichtigungsaktionen.
- **Nova System Services:** Systemereignisse.
- **Nova Shell Recovery:** Fehlerbehandlung und Wiederherstellung.
- **Nova.Trust:** Absenderidentität, Berechtigungen und Datenschutz.

## 16. Ergebnis

NovaOS erhält ein zentrales, sicheres und individuell steuerbares Benachrichtigungssystem.

Anwendungen, Capabilities und Systemdienste können Informationen strukturiert bereitstellen, während NovaOS deren Darstellung, Priorisierung und Zustellung koordiniert.

**Grundsatz:** NovaOS informiert so wenig wie möglich und so viel wie nötig. Benachrichtigungen sollen helfen, nicht stören.