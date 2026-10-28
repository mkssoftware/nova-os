# NPSPEC-INPUT-FOCUS-0001 – Nova Focus Manager

**Status:** Angenommen  
**Bereich:** Input / Focus Management  
**System:** NovaOS

## 1. Zweck

Nova Focus Manager definiert die zentrale Verwaltung des Eingabefokus innerhalb von NovaOS.

Ziel ist eine sichere, deterministische und konsistente Zuordnung von Eingaben zu Fenstern, Anwendungen und Benutzeroberflächenelementen.

Der Fokus bestimmt, welcher Eingabekontext Tastatur- und andere fokusabhängige Eingaben erhält.

Die Fokusverwaltung arbeitet unabhängig von Nova UI und dem verwendeten Grafikbackend.

## 2. Architektur

Das Subsystem umfasst folgende Komponenten:

| Komponente | Aufgabe |
|---|---|
| Focus Manager | Zentrale Verwaltung des Eingabefokus |
| Focus State Controller | Verwaltung aktueller Fokuszustände |
| Focus Request Handler | Verarbeitung von Fokusanforderungen |
| Focus Policy Controller | Prüfung von Fokusregeln und Berechtigungen |
| Focus History Manager | Verwaltung vorheriger Fokusziele |
| Focus Event Provider | Bereitstellung von Fokusänderungsereignissen |

Der Focus Manager arbeitet mit Nova Event Routing und dem Nova Window Manager zusammen.

Die Verwaltung des UI-internen Elementfokus erfolgt durch Nova UI.

## 3. Fokusmodell

NovaOS unterscheidet folgende Fokusebenen:

| Fokusebene | Beschreibung |
|---|---|
| Session Focus | Aktive Benutzersitzung |
| Window Focus | Fenster mit aktivem Eingabefokus |
| Keyboard Focus | Empfänger von Tastatureingaben |
| Pointer Focus | Aktuelles Ziel zeigerbasierter Eingaben |
| UI Element Focus | Fokussiertes Steuerelement innerhalb eines Fensters |
| Secure Focus | Geschützter Eingabekontext |

Jeder Input Seat besitzt einen eigenen Fokuszustand.

Die verschiedenen Fokusebenen MÜSSEN unabhängig verwaltet werden können.

Ein sichtbares oder im Vordergrund befindliches Fenster besitzt nicht automatisch den Tastaturfokus.

## 4. Fokuszustände

Ein Fokusziel kann folgende Zustände besitzen:

- **Inactive:** Kein aktiver Eingabefokus.
- **Focused:** Ziel besitzt den entsprechenden Fokus.
- **Pending:** Fokuswechsel wird geprüft.
- **Suspended:** Fokus ist vorübergehend ausgesetzt.
- **Invalid:** Ziel ist nicht mehr verfügbar oder berechtigt.

Fokusänderungen erfolgen über definierte Zustandsübergänge.

Ein ungültiges Fokusziel DARF keine weiteren regulären Eingaben erhalten.

## 5. Fokusanforderungen

Fokusänderungen können ausgelöst werden durch:

- Benutzerinteraktionen.
- Fensteraktivierung.
- Tastaturnavigation.
- Sitzungswechsel.
- Systemdialoge.
- Autorisierte Anwendungen.
- Sicherheitskritische Systemfunktionen.

Jede Fokusanforderung wird anhand des aktuellen Eingabekontexts geprüft.

Anwendungen DÜRFEN den globalen Eingabefokus nicht ohne entsprechende Berechtigung erzwingen.

Fokusanforderungen im Hintergrund SOLLEN keine unbeabsichtigte Unterbrechung der Benutzerarbeit verursachen.

## 6. Fokuswechsel

Ein Fokuswechsel erfolgt in folgender Reihenfolge:

1. Fokusanforderung empfangen.
2. Ziel und Berechtigungen überprüfen.
3. Aktuellen Fokuszustand ermitteln.
4. Bestehende Fokus- und Capture-Regeln berücksichtigen.
5. Fokuswechsel durchführen.
6. Fokuszustände aktualisieren.
7. Fokusereignisse erzeugen.
8. Nachfolgende Eingaben an das neue Ziel routen.

Der Wechsel MUSS atomar aus Sicht des Event Routings erfolgen.

Ein fehlgeschlagener Fokuswechsel DARF keinen inkonsistenten Fokuszustand hinterlassen.

## 7. Fokusereignisse

NovaOS stellt mindestens folgende Ereignisse bereit:

| Ereignis | Beschreibung |
|---|---|
| `FocusRequested` | Fokuswechsel wurde angefordert |
| `FocusGained` | Eingabefokus wurde erhalten |
| `FocusLost` | Eingabefokus wurde verloren |
| `FocusChanged` | Aktives Fokusziel wurde geändert |
| `FocusSuspended` | Fokus wurde vorübergehend ausgesetzt |
| `FocusRestored` | Vorheriger Fokus wurde wiederhergestellt |

Fokusereignisse enthalten:

- Input-Seat-Kennung.
- Betroffene Fokusebene.
- Vorheriges Fokusziel.
- Neues Fokusziel.
- Monotonen Zeitstempel.
- Grund der Fokusänderung.

Die Ereignisse werden über Nova Event Dispatcher zugestellt.

## 8. Fokus und Eingabegeräte

Die Fokusverwaltung berücksichtigt unterschiedliche Eingabeverfahren.

### Tastatur

Tastatureingaben werden an den aktiven Keyboard Focus geroutet.

### Maus

Zeigereingaben verwenden grundsätzlich Hit Testing oder aktives Pointer Capture.

Pointer Focus und Keyboard Focus DÜRFEN unterschiedlich sein.

### Touch und Stift

Aktive Kontakte behalten ihre gültige Eingabezuordnung.

Ein Fokuswechsel DARF laufende Kontaktsequenzen nicht unkontrolliert an andere Empfänger übertragen.

### Mehrere Eingabegeräte

Mehrere Geräte können denselben Input Seat verwenden.

Unabhängige Input Seats besitzen getrennte Fokuszustände.

## 9. Fokusverlauf und Wiederherstellung

NovaOS SOLL vorherige Fokusziele verwalten.

Dadurch können beispielsweise nach dem Schließen eines Dialogs vorherige Eingabekontexte wiederhergestellt werden.

Dabei gilt:

- Ungültige Fokusziele werden nicht wiederhergestellt.
- Sitzungsgrenzen bleiben erhalten.
- Sicherheitsrichtlinien besitzen Vorrang.
- Die Wiederherstellung erfolgt nur bei weiterhin gültigem Eingabekontext.

Der Fokusverlauf MUSS nicht dauerhaft gespeichert werden.

## 10. Modale und geschützte Eingabekontexte

Modale Fenster können den zulässigen Eingabefokus innerhalb ihres Zuständigkeitsbereichs einschränken.

Geschützte Eingabekontexte besitzen Vorrang vor regulären Anwendungen.

Dabei gilt:

- Nicht berechtigte Fenster können den geschützten Fokus nicht übernehmen.
- Globale Shortcuts können während geschützter Eingaben eingeschränkt werden.
- Fokusänderungen werden anhand der Sicherheitsrichtlinien geprüft.
- Nach Beendigung eines geschützten Kontexts wird ein zulässiges Fokusziel wiederhergestellt.

Sicherheitskritische Systemfunktionen MÜSSEN den Fokus unabhängig von regulären Anwendungen übernehmen können.

## 11. Sicherheit und Fehlerbehandlung

Nova Focus Manager unterliegt Nova Input Security und Nova.Trust.

Festlegungen:

- Anwendungen DÜRFEN keine fremden Sitzungen fokussieren.
- Fokusanforderungen MÜSSEN autorisiert werden.
- Hintergrundanwendungen DÜRFEN den Benutzer nicht unkontrolliert unterbrechen.
- Ungültige Fokusziele werden zurückgewiesen.
- Bei Anwendungsabsturz wird der zugehörige Fokus kontrolliert freigegeben.
- Fehlerhafte Fokusereignisse DÜRFEN keine Sicherheitsgrenzen umgehen.
- Geschützte Eingabekontexte MÜSSEN gegen unberechtigte Fokusübernahme abgesichert sein.

## 12. Normative Anforderungen

- **INPUT-001:** NovaOS MUSS eine zentrale Fokusverwaltung bereitstellen.
- **INPUT-002:** Jeder Input Seat MUSS einen unabhängigen Fokuszustand besitzen.
- **INPUT-003:** Window Focus, Keyboard Focus und Pointer Focus MÜSSEN unterscheidbar sein.
- **INPUT-004:** Fokusanforderungen MÜSSEN anhand definierter Regeln geprüft werden.
- **INPUT-005:** Fokuswechsel MÜSSEN aus Sicht des Event Routings atomar erfolgen.
- **INPUT-006:** Ungültige Fokusziele DÜRFEN keine regulären Eingaben erhalten.
- **INPUT-007:** Fokusänderungen MÜSSEN über definierte Ereignisse bereitgestellt werden.
- **INPUT-008:** Hintergrundanwendungen DÜRFEN den Fokus nicht unberechtigt übernehmen.
- **INPUT-009:** Modale Eingabekontexte MÜSSEN unterstützt werden.
- **INPUT-010:** Geschützte Eingabekontexte MÜSSEN Vorrang vor regulären Fokusanforderungen besitzen.
- **INPUT-011:** Bei Anwendungsabsturz MUSS der zugehörige Fokus kontrolliert freigegeben werden.
- **INPUT-012:** Vorherige Fokusziele SOLLEN wiederhergestellt werden können.
- **INPUT-013:** Fokuswechsel DÜRFEN laufende Touch- und Stiftkontakte nicht unkontrolliert umleiten.
- **INPUT-014:** Fokusänderungen MÜSSEN den Sicherheitsrichtlinien von Nova.Trust entsprechen.
- **INPUT-015:** Die Fokusverwaltung MUSS unabhängig von Nova UI und der Rendering-Frequenz funktionieren.
- **INPUT-016:** Identische Fokusanforderungen und Ausgangszustände MÜSSEN reproduzierbare Ergebnisse liefern.
- **INPUT-017:** Nova Focus Manager MUSS vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 13. Integration

- **Nova Input Architecture:** Zentrale Eingabeverarbeitung.
- **Nova Event Routing:** Fokusabhängige Zielermittlung.
- **Nova Event Dispatcher:** Zustellung von Fokusereignissen.
- **Nova Keyboard Events:** Tastaturfokus.
- **Nova Pointer Events:** Pointer Focus und Capture.
- **Nova Multi-Device Input:** Unabhängige Input Seats.
- **Nova Global Shortcuts:** Behandlung systemweiter Tastenkombinationen.
- **Nova Window Manager:** Fensteraktivierung und Fensterfokus.
- **Nova UI:** Fokusverwaltung innerhalb von Steuerelementen.
- **Nova Input Security:** Geschützte Eingabekontexte.
- **Nova.Trust:** Berechtigungen und Sicherheitsrichtlinien.

## 14. Ergebnis

NovaOS erhält eine zentrale und sichere Fokusverwaltung für Fenster, Anwendungen und Eingabekontexte.

Durch getrennte Fokusebenen, atomare Fokuswechsel und kontrollierte Wiederherstellung bleibt die Eingabezuordnung auch bei mehreren Geräten, Fenstern und Benutzersitzungen konsistent.

**Grundsatz:** Der Eingabefokus folgt der tatsächlichen Benutzerinteraktion und den gültigen Sicherheitsregeln – nicht den ungeprüften Anforderungen einzelner Anwendungen.