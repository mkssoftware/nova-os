# NPSPEC-SHELL-NOTIFY-0004 – Nova Do Not Disturb

**Status:** Angenommen  
**Bereich:** Shell / Notifications / Do Not Disturb  
**System:** NovaOS

## 1. Zweck

Nova Do Not Disturb definiert die zentrale Steuerung von Benachrichtigungsunterbrechungen innerhalb von NovaOS.

Ziel ist es, den Benutzer während konzentrierter Arbeit, Präsentationen oder anderer Aktivitäten vor unnötigen Unterbrechungen zu schützen.

Benachrichtigungen sollen weiterhin zugestellt und gespeichert werden können, ohne unmittelbar die Aufmerksamkeit des Benutzers zu beanspruchen.

**Grundprinzip:** Do Not Disturb unterdrückt Unterbrechungen, nicht die eigentliche Benachrichtigungszustellung.

## 2. Architektur

Das Subsystem besteht aus folgenden Komponenten:

| Komponente | Aufgabe |
|---|---|
| DND Manager | Zentrale Verwaltung des Ruhemodus |
| DND Policy Engine | Bewertung von Unterbrechungsregeln |
| DND Context Monitor | Erkennung relevanter Systemzustände |
| DND Schedule Manager | Zeitgesteuerte Aktivierung |
| DND Exception Manager | Verwaltung erlaubter Ausnahmen |
| DND State Manager | Speicherung und Wiederherstellung |

Nova Do Not Disturb MUSS unabhängig von der grafischen Desktop Shell funktionieren.

Die Entscheidung über die Darstellung einer Benachrichtigung erfolgt vor ihrer Übergabe an die Toast-Darstellung.

## 3. Betriebsmodi

NovaOS unterstützt folgende Modi:

| Modus | Verhalten |
|---|---|
| Off | Normale Benachrichtigungsdarstellung |
| On | Unterdrückung nicht ausgenommener Unterbrechungen |
| Scheduled | Aktivierung nach definiertem Zeitplan |
| Contextual | Aktivierung anhand definierter Systemzustände |

Dabei gilt:

- Der Benutzer MUSS Do Not Disturb jederzeit manuell aktivieren und deaktivieren können.
- Eine zeitlich begrenzte Aktivierung MUSS unterstützt werden.
- Automatische Aktivierungen MÜSSEN nachvollziehbar sein.
- Manuelle Benutzereinstellungen MÜSSEN Vorrang vor automatischen Empfehlungen besitzen.
- Der aktuelle Modus MUSS eindeutig erkennbar sein.

## 4. Unterbrechungsregeln

Die DND Policy Engine entscheidet über die unmittelbare Darstellung einer Benachrichtigung.

Mögliche Ergebnisse:

- **Allow:** Normale Darstellung.
- **Silent:** Zustellung ohne akustische oder visuelle Unterbrechung.
- **Defer:** Verschiebung einer zulässigen temporären Darstellung.
- **Suppress:** Keine temporäre Darstellung; reguläre Zustellung bleibt erhalten.

Dabei gilt:

- Die Entscheidung MUSS deterministisch erfolgen.
- Benachrichtigungspriorität und Absenderrichtlinien MÜSSEN berücksichtigt werden.
- Anwendungen DÜRFEN Do Not Disturb nicht eigenmächtig umgehen.
- Unterdrückte Benachrichtigungen MÜSSEN entsprechend ihrer Persistenzrichtlinie verfügbar bleiben.
- Die DND Policy Engine DARF keine Benachrichtigungen eigenständig löschen.

## 5. Automatische Aktivierung

NovaOS DARF Do Not Disturb anhand definierter Kontexte automatisch aktivieren.

Unterstützte Kontexte können sein:

- Vollbildanwendungen.
- Präsentationen.
- Medienwiedergabe.
- Konzentrierte Arbeitsphasen.
- Benutzerdefinierte Zeiträume.
- Registrierte Arbeitsumgebungen und Solutions.

Dabei gilt:

- Automatische Regeln MÜSSEN konfigurierbar und deaktivierbar sein.
- Kontextänderungen MÜSSEN ereignisgesteuert verarbeitet werden.
- Der Benutzer MUSS erkennen können, weshalb Do Not Disturb aktiviert wurde.
- Kontextabhängige Regeln DÜRFEN keine dauerhaften Einstellungen ungefragt verändern.
- Eine KI-basierte Kontexterkennung DARF ausschließlich optional sein.

## 6. Zeitplanung

Der DND Schedule Manager unterstützt:

- Einmalige Ruhezeiten.
- Wiederkehrende Zeitpläne.
- Wochentagsabhängige Regeln.
- Zeitlich begrenzte manuelle Aktivierungen.

Dabei gilt:

- Zeitpläne MÜSSEN die Nova Time Services verwenden.
- Änderungen der Systemzeit und Zeitzone MÜSSEN berücksichtigt werden.
- Überschneidende Zeitpläne MÜSSEN deterministisch aufgelöst werden.
- Nach Ablauf einer zeitlich begrenzten Aktivierung MUSS der zuvor geltende Regelzustand wiederhergestellt werden.

## 7. Ausnahmen

Der DND Exception Manager verwaltet zulässige Unterbrechungen.

Unterstützte Ausnahmen:

- Kritische System- und Sicherheitsereignisse.
- Ausgewählte Anwendungen.
- Bestimmte Benachrichtigungskategorien.
- Benutzerdefinierte Prioritätsregeln.
- Ausdrücklich zugelassene Ereignisse.

Dabei gilt:

- Ausnahmen MÜSSEN durch den Benutzer kontrollierbar sein.
- Anwendungen DÜRFEN sich nicht selbst als Ausnahme autorisieren.
- Kritische Systemereignisse MÜSSEN anhand vertrauenswürdiger Systemrichtlinien bewertet werden.
- Sicherheitskritische Warnungen DÜRFEN nicht allein aufgrund einer gewöhnlichen Anwendungseinstufung den Ruhemodus umgehen.
- Zwingend erforderliche Unterbrechungen MÜSSEN auf das notwendige Maß begrenzt bleiben.

## 8. Verhalten unterdrückter Benachrichtigungen

Während Do Not Disturb aktiv ist:

- Bleibt die reguläre Benachrichtigungszustellung erhalten.
- Werden nicht ausgenommene Toasts unterdrückt.
- Werden zugehörige Benachrichtigungen gemäß ihrer Persistenzrichtlinie gespeichert.
- Können Benachrichtigungen weiterhin im Notification Center eingesehen werden.
- Werden zulässige Hintergrundaktionen nicht blockiert.

Nach dem Deaktivieren:

- MÜSSEN unterdrückte Benachrichtigungen weiterhin auffindbar sein, sofern ihre Aufbewahrungsfrist nicht abgelaufen ist.
- DÜRFEN nicht sämtliche unterdrückten Toasts gleichzeitig nachträglich erscheinen.
- SOLL eine dezente Zusammenfassung verpasster Benachrichtigungen angeboten werden können.

## 9. Benutzeroberfläche

Do Not Disturb MUSS über die Desktop Shell steuerbar sein.

Vorgesehene Zugriffspunkte:

- Notification Center.
- System Tray.
- Systemeinstellungen.
- Optionale Tastenkombinationen.
- Autorisierte Quick Actions.

Dabei gilt:

- Der aktuelle Zustand MUSS eindeutig dargestellt werden.
- Der Benutzer MUSS den Ruhemodus schnell umschalten können.
- Die verbleibende Dauer einer zeitlich begrenzten Aktivierung SOLL angezeigt werden.
- Automatische Aktivierungsgründe MÜSSEN einsehbar sein.
- Alle Funktionen MÜSSEN barrierefrei bedienbar sein.

## 10. Zustand und Wiederherstellung

Der DND State Manager speichert benutzerbezogen:

- Aktivierungsmodus.
- Zeitpläne.
- Kontextregeln.
- Ausnahmen.
- Manuelle Übersteuerungen.

Dabei gilt:

- Einstellungen MÜSSEN einen Shell-Neustart überstehen.
- Der aktuelle Zustand MUSS unabhängig von der Shell verwaltet werden.
- Nach einem Systemneustart MÜSSEN gültige Zeit- und Kontextregeln erneut ausgewertet werden.
- Fehlerhafte Konfigurationen DÜRFEN die Benachrichtigungszustellung nicht verhindern.
- Der Benutzer MUSS automatische Regeln zurücksetzen können.

## 11. Performance und Sicherheit

Nova Do Not Disturb MUSS ressourcenschonend und sicher arbeiten.

Dabei gilt:

- Regeländerungen MÜSSEN ereignisgesteuert verarbeitet werden.
- Die Bewertung einer Benachrichtigung DARF die UI Event Loop nicht blockieren.
- Kontextüberwachung DARF keine permanente ressourcenintensive Abfrage erfordern.
- Die Regelverarbeitung MUSS auch ohne grafische Benutzeroberfläche funktionieren.
- Anwendungen DÜRFEN keine fremden DND-Einstellungen verändern.
- Sicherheitsrelevante Ausnahmen MÜSSEN Nova.Trust berücksichtigen.
- Das Subsystem MUSS vollständig offline und ohne KI funktionieren.

## 12. Normative Anforderungen

- **NOTIFY-DND-001:** NovaOS MUSS einen zentralen Do-Not-Disturb-Modus bereitstellen.
- **NOTIFY-DND-002:** Do Not Disturb MUSS unabhängig von der Desktop Shell funktionieren.
- **NOTIFY-DND-003:** Der Benutzer MUSS den Ruhemodus manuell aktivieren und deaktivieren können.
- **NOTIFY-DND-004:** Zeitgesteuerte und kontextabhängige Aktivierungen MÜSSEN unterstützt werden.
- **NOTIFY-DND-005:** Unterbrechungsregeln MÜSSEN deterministisch ausgewertet werden.
- **NOTIFY-DND-006:** Anwendungen DÜRFEN den Ruhemodus nicht eigenmächtig umgehen.
- **NOTIFY-DND-007:** Unterdrückte Benachrichtigungen MÜSSEN gemäß ihrer Persistenzrichtlinie erhalten bleiben.
- **NOTIFY-DND-008:** Ausnahmen MÜSSEN konfigurierbar und autorisiert sein.
- **NOTIFY-DND-009:** Automatische Aktivierungen MÜSSEN nachvollziehbar und abschaltbar sein.
- **NOTIFY-DND-010:** Der aktuelle Zustand MUSS in der Shell erkennbar sein.
- **NOTIFY-DND-011:** Nachträgliche Toast-Einblendungen DÜRFEN den Benutzer nicht überfluten.
- **NOTIFY-DND-012:** Einstellungen MÜSSEN gespeichert und wiederhergestellt werden.
- **NOTIFY-DND-013:** Die Regelverarbeitung DARF die UI Event Loop nicht blockieren.
- **NOTIFY-DND-014:** Nova.Trust und Accessibility MÜSSEN berücksichtigt werden.
- **NOTIFY-DND-015:** Nova Do Not Disturb MUSS vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 13. Integration

- **Nova Notification Center:** Benachrichtigungsverwaltung und Benutzeroberfläche.
- **Nova Toast Notifications:** Unterdrückung temporärer Einblendungen.
- **Nova Notification Persistence:** Speicherung unterdrückter Benachrichtigungen.
- **Nova Notification Policy Manager:** Priorisierung und Unterbrechungsregeln.
- **Nova System Tray:** Zustandsanzeige und Schnellzugriff.
- **Nova Window Manager:** Vollbild- und Fensterkontexte.
- **Nova Time Services:** Zeitplanung.
- **Nova Solutions:** Kontextabhängige Arbeitsumgebungen.
- **Nova Shell State Manager:** Benutzereinstellungen.
- **Nova.Trust:** Autorisierung und Sicherheitsrichtlinien.

## 14. Ergebnis

NovaOS erhält eine zentrale, flexible und benutzerkontrollierte Ruhefunktion.

Benachrichtigungen werden weiterhin zuverlässig verarbeitet, während unnötige Unterbrechungen entsprechend den persönlichen Einstellungen und dem aktuellen Arbeitskontext vermieden werden.

**Grundsatz:** NovaOS respektiert die Aufmerksamkeit des Benutzers. Nicht jede Information muss sofort sichtbar sein, nur weil sie gerade entsteht.