# NPSPEC-SHELL-NOTIFY-0003 – Nova Notification Persistence

**Status:** Angenommen  
**Bereich:** Shell / Notifications / Persistence  
**System:** NovaOS

## 1. Zweck

Nova Notification Persistence definiert die dauerhafte Speicherung, Wiederherstellung und Verwaltung von Benachrichtigungen innerhalb von NovaOS.

Benachrichtigungen MÜSSEN unabhängig vom Lebenszyklus der Desktop Shell verwaltet werden können.

Ziel ist ein zuverlässiger Benachrichtigungsverlauf, der relevante Informationen erhält, ohne unnötig Speicherplatz zu verbrauchen oder die Privatsphäre des Benutzers zu beeinträchtigen.

## 2. Architektur

Das Subsystem besteht aus folgenden Komponenten:

| Komponente | Aufgabe |
|---|---|
| Notification Persistence Manager | Koordination der dauerhaften Speicherung |
| Notification Store | Speicherung der Benachrichtigungsdaten |
| Notification State Manager | Verwaltung von Lese- und Bearbeitungszuständen |
| Notification Retention Manager | Aufbewahrung und automatische Bereinigung |
| Notification Recovery Manager | Wiederherstellung nach Neustarts und Fehlern |

Die Persistenzverwaltung MUSS unabhängig von der grafischen Desktop Shell arbeiten.

Sie MUSS über definierte Schnittstellen mit dem zentralen Notification Manager kommunizieren.

## 3. Persistenzmodell

Eine gespeicherte Benachrichtigung enthält mindestens:

- Eindeutige Benachrichtigungs-ID.
- Absenderidentität.
- Titel und Inhalt.
- Erstellungszeitpunkt.
- Priorität.
- Benachrichtigungstyp.
- Bearbeitungszustand.
- Persistenzrichtlinie.

Optional können gespeichert werden:

- Gruppierungs-ID.
- Ablaufzeitpunkt.
- Fortschrittszustand.
- Aktionsreferenzen.
- Zusätzliche Metadaten.

Dabei gilt:

- Gespeicherte Benachrichtigungen MÜSSEN eindeutig identifizierbar sein.
- Datenformate MÜSSEN versionierbar sein.
- Nicht mehr gültige Aktionsreferenzen DÜRFEN nicht ausführbar bleiben.
- Persistente Daten MÜSSEN unabhängig von ihrer visuellen Darstellung gespeichert werden.

## 4. Persistenzrichtlinien

NovaOS unterstützt folgende Speicherklassen:

| Klasse | Verhalten |
|---|---|
| Transient | Nur während der aktuellen Laufzeit verfügbar |
| Session | Bis zum Ende der Benutzersitzung verfügbar |
| Persistent | Über Sitzungen und Neustarts hinweg gespeichert |
| Critical | Dauerhaft gespeichert, bis die geltende Aufbewahrungsrichtlinie eine Entfernung erlaubt |

Dabei gilt:

- Die Speicherklasse MUSS durch Systemrichtlinien überprüfbar sein.
- Anwendungen DÜRFEN keine unbegrenzte Speicherung erzwingen.
- Kritische Systemereignisse MÜSSEN entsprechend ihrer Sicherheitsrichtlinien behandelt werden.
- Der Benutzer MUSS reguläre gespeicherte Benachrichtigungen löschen können.

## 5. Benachrichtigungszustände

Folgende Zustände werden unterstützt:

- **Unread:** Noch nicht gelesen.
- **Read:** Vom Benutzer gelesen.
- **Acknowledged:** Bestätigt.
- **Dismissed:** Aus der aktiven Ansicht entfernt.
- **Archived:** Archiviert.
- **Expired:** Aufbewahrungsdauer abgelaufen.

Dabei gilt:

- Zustandsänderungen MÜSSEN dauerhaft gespeichert werden können.
- Lesen, Bestätigen und Löschen MÜSSEN getrennte Operationen sein.
- Das Schließen eines Toasts DARF nicht automatisch zum Löschen der Benachrichtigung führen.
- Zustandsänderungen MÜSSEN ereignisgesteuert an das Notification Center übertragen werden.

## 6. Speicherung und Aktualisierung

Der Notification Store verwaltet die dauerhaften Daten.

Dabei gilt:

- Schreiboperationen MÜSSEN konsistent und gegen unvollständige Aktualisierungen abgesichert sein.
- Bestehende Benachrichtigungen MÜSSEN anhand ihrer ID aktualisiert werden können.
- Fortschrittsmeldungen SOLLEN vorhandene Datensätze aktualisieren.
- Wiederholte identische Meldungen SOLLEN entsprechend definierter Regeln zusammengefasst werden.
- Speicheroperationen DÜRFEN die UI Event Loop nicht blockieren.
- Eine vollständige Datenbank MUSS keine zwingende Voraussetzung sein.

## 7. Wiederherstellung

Der Notification Recovery Manager stellt gespeicherte Benachrichtigungen nach einem Neustart wieder bereit.

Dabei gilt:

- Persistente Benachrichtigungen MÜSSEN einen Neustart der Desktop Shell überstehen.
- Gespeicherte Daten MÜSSEN nach einem Systemneustart wiederherstellbar sein.
- Ungültige oder beschädigte Einträge MÜSSEN isoliert werden können.
- Ein beschädigter Verlauf DARF den Start der Desktop Shell nicht verhindern.
- Wiederhergestellte Benachrichtigungen DÜRFEN nicht automatisch erneut als Toast erscheinen, sofern keine ausdrückliche Richtlinie dies vorsieht.
- Der ursprüngliche Lese- und Bestätigungszustand MUSS erhalten bleiben.

## 8. Aufbewahrung und Bereinigung

Der Notification Retention Manager verwaltet die Lebensdauer gespeicherter Benachrichtigungen.

Dabei gilt:

- Aufbewahrungsfristen MÜSSEN konfigurierbar sein.
- Abgelaufene Benachrichtigungen SOLLEN automatisch entfernt werden.
- Speichergrenzen MÜSSEN unterstützt werden.
- Der Benutzer MUSS den regulären Verlauf vollständig löschen können.
- Gesetzlich oder sicherheitsbedingt erforderliche Protokolle MÜSSEN getrennt vom regulären Benachrichtigungsverlauf verwaltet werden.
- Eine Benachrichtigung DARF nicht allein aufgrund ihrer hohen Priorität unbegrenzt gespeichert werden.

## 9. Benutzer- und Sitzungsisolation

Benachrichtigungen MÜSSEN dem jeweiligen Benutzerkontext zugeordnet sein.

Dabei gilt:

- Benutzer DÜRFEN keine Benachrichtigungen anderer Benutzer einsehen.
- Sitzungswechsel MÜSSEN berücksichtigt werden.
- Benutzerbezogene Daten MÜSSEN getrennt gespeichert werden.
- Systemweite Benachrichtigungen MÜSSEN über definierte Berechtigungen zugestellt werden.
- Gesperrte Sitzungen DÜRFEN keine unautorisierten Benachrichtigungsinhalte offenlegen.

## 10. Performance und Ressourcenverwaltung

Nova Notification Persistence MUSS ressourcenschonend arbeiten.

Dabei gilt:

- Speicheroperationen SOLLEN asynchron erfolgen.
- Mehrere Aktualisierungen DÜRFEN kontrolliert zusammengefasst werden.
- Der Verlauf MUSS abschnittsweise geladen werden können.
- Große Benachrichtigungsverläufe DÜRFEN den Desktop nicht verlangsamen.
- Nicht benötigte Daten SOLLEN aus dem Arbeitsspeicher freigegeben werden.
- Die Speicherung MUSS ohne permanente Hintergrundabfragen funktionieren.

## 11. Sicherheit und Datenschutz

Nova Notification Persistence unterliegt Nova.Trust.

Dabei gilt:

- Gespeicherte Benachrichtigungen MÜSSEN vor unautorisiertem Zugriff geschützt werden.
- Absender DÜRFEN ausschließlich ihre eigenen Benachrichtigungen entsprechend ihrer Berechtigungen verändern.
- Sensible Inhalte MÜSSEN gemäß den geltenden Datenschutzrichtlinien geschützt werden.
- Persistente Aktionsreferenzen MÜSSEN vor ihrer Ausführung erneut autorisiert werden.
- Der Benutzer MUSS Kontrolle über den regulären Benachrichtigungsverlauf besitzen.
- Benachrichtigungsdaten DÜRFEN nicht ohne Zustimmung an externe Dienste übertragen werden.

## 12. Normative Anforderungen

- **NOTIFY-PERSIST-001:** NovaOS MUSS eine zentrale Benachrichtigungspersistenz bereitstellen.
- **NOTIFY-PERSIST-002:** Die Speicherung MUSS unabhängig von der Desktop Shell funktionieren.
- **NOTIFY-PERSIST-003:** Persistente Benachrichtigungen MÜSSEN Neustarts überstehen.
- **NOTIFY-PERSIST-004:** Benachrichtigungen MÜSSEN eindeutige Identitäten besitzen.
- **NOTIFY-PERSIST-005:** Unterschiedliche Persistenzrichtlinien MÜSSEN unterstützt werden.
- **NOTIFY-PERSIST-006:** Lese-, Bestätigungs- und Löschzustände MÜSSEN getrennt verwaltet werden.
- **NOTIFY-PERSIST-007:** Speicheroperationen MÜSSEN konsistent erfolgen.
- **NOTIFY-PERSIST-008:** Beschädigte Verlaufsdaten DÜRFEN den Shell-Start nicht verhindern.
- **NOTIFY-PERSIST-009:** Aufbewahrungsfristen und Speichergrenzen MÜSSEN unterstützt werden.
- **NOTIFY-PERSIST-010:** Benutzerbezogene Daten MÜSSEN isoliert werden.
- **NOTIFY-PERSIST-011:** Speicheroperationen DÜRFEN die UI Event Loop nicht blockieren.
- **NOTIFY-PERSIST-012:** Gespeicherte Aktionen MÜSSEN vor der Ausführung erneut autorisiert werden.
- **NOTIFY-PERSIST-013:** Der Benutzer MUSS seinen regulären Benachrichtigungsverlauf verwalten und löschen können.
- **NOTIFY-PERSIST-014:** Nova Notification Persistence MUSS vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 13. Integration

- **Nova Notification Center:** Darstellung und Verwaltung gespeicherter Benachrichtigungen.
- **Nova Notification Manager:** Zustellung und Statusänderungen.
- **Nova Toast Notifications:** Temporäre Darstellung ohne eigenständige Persistenz.
- **Nova Notification Policy Manager:** Aufbewahrungs- und Prioritätsrichtlinien.
- **Nova Shell Lifecycle:** Wiederherstellung nach Shell-Neustarts.
- **Nova VFS / Storage:** Dauerhafte Datenspeicherung.
- **Nova User Management:** Benutzer- und Sitzungszuordnung.
- **Nova.Trust:** Zugriffsschutz und Datenschutz.
- **Nova Diagnostics:** Fehleranalyse bei Speicher- und Wiederherstellungsproblemen.

## 14. Ergebnis

NovaOS erhält eine zuverlässige und ressourcenschonende Persistenzverwaltung für Benachrichtigungen.

Benachrichtigungen bleiben entsprechend ihrer Aufbewahrungsrichtlinien erhalten, können nach Neustarts wiederhergestellt und jederzeit durch den Benutzer verwaltet werden.

Die Speicherung erfolgt unabhängig von der grafischen Darstellung und schützt benutzerbezogene Informationen.

**Grundsatz:** Eine Benachrichtigung darf verschwinden, wenn sie nicht mehr benötigt wird – aber wichtige Informationen dürfen nicht allein deshalb verloren gehen, weil ihre Darstellung beendet wurde.