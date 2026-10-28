# NPSPEC-INPUT-DATA-0004 – Nova Secure Data Transfer

**Status:** Angenommen  
**Bereich:** Input / Data / Security  
**System:** NovaOS

## 1. Zweck

Nova Secure Data Transfer definiert die Sicherheitsregeln für den Datenaustausch zwischen Anwendungen, Capabilities, Prozessen und Benutzersitzungen.

Ziel ist die kontrollierte Übertragung von Daten unter Wahrung ihrer Vertraulichkeit, Integrität und Herkunft.

Das Subsystem schützt insbesondere Clipboard-, Drag-and-Drop- und andere benutzerinitiierte Datenübertragungen.

## 2. Architektur

Nova Secure Data Transfer besteht aus folgenden Komponenten:

| Komponente | Aufgabe |
|---|---|
| Transfer Security Manager | Zentrale Sicherheitsprüfung |
| Transfer Authorization Controller | Prüfung von Zugriffsrechten |
| Data Provenance Validator | Überprüfung der Datenherkunft |
| Transfer Isolation Controller | Durchsetzung von Prozess- und Sitzungsgrenzen |
| Secure Transfer Context | Verwaltung des Sicherheitskontexts |
| Transfer Audit Provider | Nachvollziehbarkeit sicherheitsrelevanter Vorgänge |

Die Sicherheitsentscheidungen erfolgen über Nova.Trust.

Nova Secure Data Transfer verwaltet keine eigenen Benutzerberechtigungen, sondern verwendet die zentrale Sicherheitsarchitektur.

## 3. Sicherheitsmodell

Jede Datenübertragung besitzt einen Sicherheitskontext mit mindestens:

- Eindeutiger Transfer-ID.
- Identität der Datenquelle.
- Identität des Empfängers.
- Benutzersitzung.
- Datenformat und Datenklassifikation.
- Angeforderter Operation.
- Erforderlichen Capabilities.
- Gültigkeitsdauer der Autorisierung.

Die Übertragung MUSS vor dem Zugriff auf geschützte Daten autorisiert werden.

Eine erfolgreiche Autorisierung gilt ausschließlich für den genehmigten Übertragungskontext.

## 4. Datenklassifikation

NovaOS unterstützt mindestens folgende Sicherheitsklassen:

| Klasse | Beschreibung |
|---|---|
| Public | Öffentlich zugängliche Daten |
| Internal | Daten innerhalb eines berechtigten Kontexts |
| Confidential | Vertrauliche Daten mit eingeschränktem Zugriff |
| Protected | Besonders geschützte Daten |
| Restricted | Daten mit ausdrücklich begrenzter Weitergabe |

Die Klassifikation kann durch Datenquelle, Systemrichtlinie oder autorisierten Benutzer festgelegt werden.

Eine Anwendung DARF die Sicherheitsklassifikation fremder Daten nicht eigenmächtig herabsetzen.

Die Klassifikation MUSS bei der Übertragung erhalten bleiben, sofern keine ausdrücklich autorisierte Änderung erfolgt.

## 5. Autorisierung

Vor einer Datenübertragung prüft NovaOS:

1. Identität und Vertrauensstatus der Quelle.
2. Identität und Berechtigungen des Empfängers.
3. Zulässigkeit des Datenformats.
4. Sicherheitsklassifikation.
5. Angeforderte Übertragungsoperation.
6. Gültigkeit des Sicherheitskontexts.

Eine Benutzeraktion wie Kopieren, Einfügen oder Drag and Drop kann eine gezielte Übertragung autorisieren.

Sie DARF jedoch keine darüber hinausgehenden Berechtigungen erzeugen.

Bei fehlender Berechtigung wird die Übertragung verweigert oder eine Benutzerentscheidung angefordert, sofern die Systemrichtlinie dies erlaubt.

## 6. Capability-basierte Datenfreigabe

Datenzugriffe erfolgen über ausdrücklich autorisierte Capabilities.

Dabei gilt:

- Berechtigungen werden nach dem Least-Privilege-Prinzip vergeben.
- Freigaben gelten nur für die benötigten Daten und Operationen.
- Temporäre Freigaben besitzen eine begrenzte Lebensdauer.
- Freigaben MÜSSEN widerrufbar sein.
- Weitergabe von Zugriffsrechten erfordert eine entsprechende Berechtigung.

Eine Datenreferenz allein stellt keine Zugriffsberechtigung dar.

## 7. Sichere Datenübertragung

NovaOS unterstützt folgende Übertragungsverfahren:

- Kontrollierte Datenkopie.
- Geschützte Speicherfreigabe.
- Zero-Copy über autorisierte Speicherbereiche.
- Streambasierte Übertragung.
- Referenzbasierte Übertragung.

Dabei gilt:

- Speicherisolation MUSS erhalten bleiben.
- Datenzugriffe MÜSSEN auf autorisierte Bereiche begrenzt sein.
- Schreibrechte DÜRFEN nicht unbeabsichtigt übertragen werden.
- Freigegebene Speicherbereiche MÜSSEN nach Abschluss oder Widerruf kontrolliert freigegeben werden.
- Übertragene Daten MÜSSEN gegen unberechtigte Veränderung geschützt werden.

Zero-Copy DARF ausschließlich verwendet werden, wenn die erforderlichen Sicherheitsgarantien gewährleistet sind.

## 8. Herkunft und Integrität

NovaOS verwaltet die Herkunft sicherheitsrelevanter Datenübertragungen.

Dazu gehören:

- Ursprüngliche Datenquelle.
- Übertragende Komponente.
- Datenformat.
- Durchgeführte Konvertierungen.
- Sicherheitsklassifikation.

Bei Bedarf MÜSSEN Integritätsprüfungen unterstützt werden.

Formatkonvertierungen DÜRFEN die Herkunftsinformationen nicht unbemerkt entfernen.

Unverifizierte Herkunft MUSS als solche erkennbar bleiben.

## 9. Geschützte Daten

Besonders schützenswerte Daten können zusätzlichen Einschränkungen unterliegen.

Dazu gehören beispielsweise:

- Passwörter.
- Authentifizierungstoken.
- Private Schlüssel.
- Vertrauliche Dokumente.
- Geschützte Anwendungsdaten.

Für solche Daten gilt:

- Automatische Clipboard-Übernahmen können untersagt werden.
- Drag and Drop kann eingeschränkt werden.
- Unberechtigte Hintergrundzugriffe werden verhindert.
- Eine dauerhafte Speicherung in Clipboard-Historien ist standardmäßig untersagt.
- Temporäre Daten MÜSSEN nach Ablauf ihrer Gültigkeit kontrolliert freigegeben werden.

Geheimnisse SOLLEN nach Möglichkeit über dedizierte Sicherheits-Capabilities statt über allgemeine Datenübertragungen bereitgestellt werden.

## 10. Sitzungs- und Prozessisolation

Datenübertragungen bleiben grundsätzlich auf autorisierte Sicherheitsbereiche beschränkt.

Dabei gilt:

- Prozesse besitzen getrennte Adressräume.
- Benutzersitzungen sind voneinander isoliert.
- Sitzungsübergreifende Übertragungen erfordern eine ausdrückliche Freigabe.
- Systemprozesse erhalten keine pauschale Ausnahme von Datenzugriffsregeln.
- Geschützte Eingabekontexte können Datenübertragungen unterbinden.

Die Übertragung zwischen Prozessen erfolgt über die autorisierte Nova-IPC-Infrastruktur.

## 11. Widerruf und Fehlerbehandlung

Eine Datenübertragung MUSS kontrolliert beendet werden können bei:

- Entzug einer Berechtigung.
- Beendigung der Quelle oder des Empfängers.
- Ablauf einer temporären Freigabe.
- Sitzungswechsel.
- Verletzung einer Sicherheitsrichtlinie.
- Integritätsfehlern.

Dabei gilt:

- Noch nicht freigegebene Daten DÜRFEN nicht zugänglich werden.
- Temporäre Zugriffsrechte MÜSSEN aufgehoben werden.
- Ressourcen MÜSSEN kontrolliert freigegeben werden.
- Bereits vollständig übertragene Daten können nicht durch bloßen Widerruf zurückgerufen werden.

Fehler einer Datenübertragung DÜRFEN die zentrale Eingabeverarbeitung nicht blockieren.

## 12. Audit und Datenschutz

Sicherheitsrelevante Übertragungen können durch Nova.Trust protokolliert werden.

Auditinformationen umfassen bei Bedarf:

- Zeitpunkt.
- Quelle und Empfänger.
- Datenklassifikation.
- Übertragungsoperation.
- Autorisierungsergebnis.
- Sicherheitsrelevante Fehler.

Dateninhalte DÜRFEN nicht standardmäßig im Audit gespeichert werden.

Die Protokollierung MUSS datensparsam erfolgen und darf keine geschützten Informationen offenlegen.

## 13. Normative Anforderungen

- **INPUT-001:** NovaOS MUSS Datenübertragungen anhand eines definierten Sicherheitskontexts autorisieren.
- **INPUT-002:** Quelle und Empfänger MÜSSEN eindeutig identifizierbar sein.
- **INPUT-003:** Datenklassifikationen MÜSSEN bei Übertragungen berücksichtigt werden.
- **INPUT-004:** Sicherheitsklassifikationen DÜRFEN nicht unberechtigt herabgesetzt werden.
- **INPUT-005:** Datenzugriffe MÜSSEN dem Least-Privilege-Prinzip entsprechen.
- **INPUT-006:** Temporäre Datenfreigaben MÜSSEN widerrufbar sein.
- **INPUT-007:** Datenreferenzen DÜRFEN keine Zugriffsberechtigungen umgehen.
- **INPUT-008:** Prozess- und Sitzungsisolation MÜSSEN erhalten bleiben.
- **INPUT-009:** Zero-Copy-Übertragungen MÜSSEN die Speicherisolation gewährleisten.
- **INPUT-010:** Geschützte Daten MÜSSEN gegen unberechtigte Hintergrundzugriffe abgesichert werden.
- **INPUT-011:** Datenherkunft und Integrität MÜSSEN bei sicherheitsrelevanten Übertragungen überprüfbar sein.
- **INPUT-012:** Bei Berechtigungsentzug MÜSSEN zukünftige Datenzugriffe unterbunden werden.
- **INPUT-013:** Fehlgeschlagene Übertragungen MÜSSEN kontrolliert beendet werden.
- **INPUT-014:** Auditinformationen DÜRFEN geschützte Dateninhalte nicht standardmäßig enthalten.
- **INPUT-015:** Sicherheitsentscheidungen MÜSSEN über Nova.Trust erfolgen.
- **INPUT-016:** Nova Secure Data Transfer MUSS unabhängig von Nova UI funktionieren.
- **INPUT-017:** Nova Secure Data Transfer MUSS vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 14. Integration

- **Nova Clipboard Management:** Schutz von Zwischenablagedaten.
- **Nova Drag and Drop:** Autorisierung von Zieh- und Ablageoperationen.
- **Nova Data Transfer Formats:** Datenformate und Sicherheitsmetadaten.
- **Nova IPC:** Geschützte prozessübergreifende Übertragung.
- **Nova Memory Management:** Speicherisolation und sichere Speicherfreigaben.
- **Nova VFS:** Kontrollierter Zugriff auf Dateien und Verzeichnisse.
- **Nova Capability System:** Berechtigungsgebundene Datenzugriffe.
- **Nova Input Security:** Schutz sensibler Eingabekontexte.
- **Nova.Trust:** Identität, Herkunft, Berechtigungen und Audit.

## 15. Ergebnis

NovaOS erhält eine einheitliche Sicherheitsarchitektur für den Datenaustausch zwischen Anwendungen, Capabilities und Systemkomponenten.

Daten können effizient übertragen werden, ohne ihre Sicherheitsklassifikation, Herkunft oder bestehenden Zugriffsgrenzen zu verlieren.

**Grundsatz:** Die Möglichkeit, Daten zu übertragen, bedeutet nicht automatisch die Berechtigung, sie zu lesen, zu verändern oder weiterzugeben.