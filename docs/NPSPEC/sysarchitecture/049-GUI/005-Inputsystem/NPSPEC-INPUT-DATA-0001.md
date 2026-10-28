# NPSPEC-INPUT-DATA-0001 – Nova Clipboard Management

**Status:** Angenommen  
**Bereich:** Input / Data / Clipboard  
**System:** NovaOS

## 1. Zweck

Nova Clipboard Management definiert die zentrale Verwaltung der Zwischenablage innerhalb von NovaOS.

Ziel ist ein sicherer, effizienter und formatunabhängiger Datenaustausch zwischen Anwendungen, Capabilities und Systemkomponenten.

Die Zwischenablage unterstützt unterschiedliche Datentypen, ohne deren Inhalte unnötig zu kopieren oder zu konvertieren.

## 2. Architektur

Das Subsystem umfasst folgende Komponenten:

| Komponente | Aufgabe |
|---|---|
| Clipboard Manager | Zentrale Verwaltung der Zwischenablage |
| Clipboard Data Store | Verwaltung bereitgestellter Daten |
| Clipboard Format Registry | Registrierung unterstützter Datenformate |
| Clipboard Transfer Controller | Steuerung der Datenübertragung |
| Clipboard Security Controller | Berechtigungen und Zugriffsschutz |
| Clipboard Event Provider | Benachrichtigung über Änderungen |

Die Zwischenablage arbeitet unabhängig von Nova UI und dem verwendeten Grafikbackend.

## 3. Datenmodell

Ein Clipboard-Eintrag besitzt mindestens:

- Eindeutige Eintragskennung.
- Eigentümer beziehungsweise Datenquelle.
- Verfügbare Datenformate.
- Erstellungszeitpunkt.
- Gültigkeitsstatus.
- Sicherheitsklassifikation.
- Datenreferenz oder Dateninhalt.

Ein Eintrag DARF mehrere Repräsentationen desselben Inhalts enthalten.

Beispielsweise kann eine kopierte Tabelle gleichzeitig als strukturierte Tabellendaten, HTML und Klartext verfügbar sein.

Die verschiedenen Repräsentationen MÜSSEN demselben logischen Clipboard-Eintrag zugeordnet bleiben.

## 4. Unterstützte Datenformate

NovaOS unterstützt mindestens folgende Datenkategorien:

| Kategorie | Beschreibung |
|---|---|
| Text | Unicode-Text |
| Rich Text | Formatierter Text |
| Image | Raster- und Vektorgrafiken |
| File Reference | Dateien und Verzeichnisse |
| Structured Data | Strukturierte Anwendungsdaten |
| Binary Data | Binäre Daten |
| Custom Format | Registrierte anwendungsspezifische Formate |

Jedes Format wird über eine eindeutige Formatkennung identifiziert.

Anwendungen DÜRFEN eigene Formate registrieren.

Unbekannte Formate MÜSSEN ohne Interpretation übertragen werden können, sofern der Empfänger deren Verarbeitung unterstützt.

## 5. Clipboard-Operationen

NovaOS stellt mindestens folgende Operationen bereit:

- `SetClipboard()`
- `GetClipboard()`
- `GetAvailableFormats()`
- `ClearClipboard()`
- `HasFormat()`
- `RequestClipboardData()`

Die API MUSS sowohl synchrone Metadatenabfragen als auch asynchrone Datenübertragungen unterstützen können.

Das Schreiben eines neuen Clipboard-Eintrags ersetzt den bisherigen aktiven Eintrag atomar.

Fehlgeschlagene Schreibvorgänge DÜRFEN den vorherigen gültigen Eintrag nicht beschädigen.

## 6. Datenübertragung

NovaOS unterstützt unterschiedliche Übertragungsverfahren:

### Direkte Datenübertragung

Kleine Datenmengen können unmittelbar in der Zwischenablage gespeichert werden.

### Referenzbasierte Übertragung

Größere Datenmengen können über kontrollierte Datenreferenzen bereitgestellt werden.

### Lazy Data Provision

Daten können erst bei tatsächlicher Anforderung erzeugt oder konvertiert werden.

### Zero-Copy Transfer

Geeignete Daten können über gemeinsam genutzte Speicherbereiche übertragen werden, sofern Speicherisolation und Berechtigungen gewährleistet bleiben.

Die Auswahl des Übertragungsverfahrens erfolgt anhand von Datengröße, Format und verfügbaren Systemressourcen.

## 7. Formatverhandlung

Ein Clipboard-Eintrag kann mehrere Datenformate anbieten.

Der Empfänger wählt ein unterstütztes Format aus.

Dabei gilt:

- Native strukturierte Formate SOLLEN bevorzugt werden, wenn beide Seiten diese unterstützen.
- Eine automatische Konvertierung DARF erfolgen, sofern ein geeigneter Konverter verfügbar ist.
- Konvertierungen MÜSSEN kontrolliert und nachvollziehbar sein.
- Datenverluste durch Formatkonvertierung SOLLEN vermieden werden.
- Der ursprüngliche Clipboard-Eintrag bleibt unverändert.

Die Formatverhandlung erfolgt unabhängig von der Benutzeroberfläche.

## 8. Lebenszyklus

Ein Clipboard-Eintrag durchläuft folgende Zustände:

1. **Created:** Eintrag wurde erstellt.
2. **Available:** Daten stehen zur Verfügung.
3. **Requested:** Ein Empfänger fordert Daten an.
4. **Transferred:** Daten wurden übertragen.
5. **Expired:** Eintrag ist nicht mehr gültig.
6. **Cleared:** Eintrag wurde entfernt.

Clipboard-Daten können unabhängig vom ursprünglichen Eigentümer bestehen bleiben, sofern NovaOS die erforderlichen Daten oder Ressourcen übernommen hat.

Referenzbasierte und verzögert bereitgestellte Daten MÜSSEN bei Verlust ihrer Quelle kontrolliert ungültig werden oder durch eine gültige gespeicherte Repräsentation ersetzt werden.

## 9. Zwischenablage und Sitzungen

Jede Benutzersitzung besitzt eine logisch getrennte Zwischenablage.

Dabei gilt:

- Anwendungen derselben Sitzung können entsprechend ihrer Berechtigungen Daten austauschen.
- Andere Benutzersitzungen besitzen keinen automatischen Zugriff.
- Geschützte Eingabekontexte können Clipboard-Zugriffe einschränken.
- Sitzungsübergreifende Übertragungen erfordern eine ausdrückliche Berechtigung.

NovaOS DARF mehrere benannte Zwischenablagen unterstützen.

## 10. Sicherheit und Datenschutz

Nova Clipboard Management unterliegt Nova.Trust.

Festlegungen:

- Schreib- und Lesezugriffe MÜSSEN autorisiert werden.
- Anwendungen DÜRFEN Clipboard-Inhalte nicht unberechtigt im Hintergrund auslesen.
- Vertrauliche Inhalte können als geschützt gekennzeichnet werden.
- Geschützte Inhalte DÜRFEN nicht automatisch in eine Clipboard-Historie übernommen werden.
- Clipboard-Daten DÜRFEN nicht ohne Zustimmung an externe Systeme übertragen werden.
- Gemeinsame Speicherbereiche MÜSSEN gegen unberechtigte Zugriffe abgesichert sein.
- Datenreferenzen DÜRFEN keine Dateisystemberechtigungen umgehen.

Das Lesen eines Clipboard-Eintrags verleiht keine zusätzlichen Berechtigungen auf dessen ursprüngliche Datenquelle.

## 11. Clipboard-Ereignisse

NovaOS stellt mindestens folgende Ereignisse bereit:

| Ereignis | Beschreibung |
|---|---|
| `ClipboardChanged` | Aktiver Eintrag wurde geändert |
| `ClipboardCleared` | Zwischenablage wurde geleert |
| `ClipboardDataRequested` | Daten wurden angefordert |
| `ClipboardDataUnavailable` | Angeforderte Daten sind nicht verfügbar |

Änderungsbenachrichtigungen DÜRFEN keine geschützten Dateninhalte offenlegen.

Anwendungen erhalten ausschließlich Ereignisse, für die sie autorisiert sind.

## 12. Performance und Ressourcenverwaltung

Die Zwischenablage arbeitet ressourcenschonend.

Dabei gilt:

- Unnötige Datenkopien SOLLEN vermieden werden.
- Große Datenmengen SOLLEN referenzbasiert oder verzögert übertragen werden.
- Nicht mehr benötigte Ressourcen MÜSSEN freigegeben werden.
- Datenübertragungen DÜRFEN die zentrale Eingabeverarbeitung nicht blockieren.
- Speicherlimits MÜSSEN berücksichtigt werden.
- Fehlerhafte Datenanbieter DÜRFEN andere Anwendungen nicht blockieren.

Clipboard-Operationen erfolgen unabhängig von der Rendering-Frequenz.

## 13. Normative Anforderungen

- **INPUT-001:** NovaOS MUSS eine zentrale Zwischenablage bereitstellen.
- **INPUT-002:** Die Zwischenablage MUSS mehrere Datenformate pro Eintrag unterstützen.
- **INPUT-003:** Unicode-Text MUSS unterstützt werden.
- **INPUT-004:** Strukturierte und binäre Daten MÜSSEN übertragbar sein.
- **INPUT-005:** Anwendungen MÜSSEN eigene Datenformate registrieren können.
- **INPUT-006:** Clipboard-Schreibvorgänge MÜSSEN atomar erfolgen.
- **INPUT-007:** Formatverhandlungen MÜSSEN unterstützt werden.
- **INPUT-008:** Große Datenmengen MÜSSEN ohne zwingende vollständige Datenkopie übertragen werden können.
- **INPUT-009:** Verzögerte Datenbereitstellung MUSS unterstützt werden können.
- **INPUT-010:** Clipboard-Einträge MÜSSEN einen definierten Lebenszyklus besitzen.
- **INPUT-011:** Benutzersitzungen MÜSSEN logisch getrennte Zwischenablagen besitzen.
- **INPUT-012:** Clipboard-Zugriffe MÜSSEN den Berechtigungsregeln von Nova.Trust entsprechen.
- **INPUT-013:** Geschützte Clipboard-Inhalte DÜRFEN nicht unberechtigt ausgelesen oder protokolliert werden.
- **INPUT-014:** Änderungen der Zwischenablage MÜSSEN über Ereignisse bereitgestellt werden können.
- **INPUT-015:** Datenreferenzen DÜRFEN keine Sicherheitsgrenzen umgehen.
- **INPUT-016:** Die Zwischenablage MUSS unabhängig von Nova UI funktionieren.
- **INPUT-017:** Nova Clipboard Management MUSS vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 14. Integration

- **Nova Input Architecture:** Einbindung in die Eingabe- und Dateninteraktion.
- **Nova Event Dispatcher:** Clipboard-Ereignisse.
- **Nova UI:** Kopieren, Ausschneiden und Einfügen.
- **Nova Framework:** Einheitliche Clipboard-API.
- **Nova Capability System:** Datenaustausch zwischen Capabilities.
- **Nova Memory Management:** Speicherverwaltung und Zero-Copy.
- **Nova IPC:** Prozessübergreifende Datenübertragung.
- **Nova VFS:** Datei- und Verzeichnisreferenzen.
- **NovaFile:** Strukturierte Nova-Datencontainer.
- **Nova.Trust:** Berechtigungen und Datenschutz.

## 15. Ergebnis

NovaOS erhält eine zentrale, sichere und formatunabhängige Zwischenablage.

Anwendungen und Capabilities können Texte, Bilder, Dateien und strukturierte Daten effizient austauschen, ohne unnötige Konvertierungen oder Datenkopien zu verursachen.

**Grundsatz:** Die Zwischenablage transportiert Daten und ihre Bedeutung, nicht nur eine vereinfachte Darstellung ihres Inhalts.