# NPSPEC-INPUT-EVENT-0006 – Nova Global Shortcuts

**Status:** Angenommen  
**Bereich:** Input / Event / Global Shortcuts  
**System:** NovaOS

## 1. Zweck

Nova Global Shortcuts definiert die systemweite Registrierung, Erkennung und Ausführung von Tastenkombinationen innerhalb von NovaOS.

Ziel ist eine einheitliche, sichere und konfigurierbare Verwaltung globaler Tastenkürzel, die unabhängig von der aktuell aktiven Anwendung funktionieren.

Globale Tastenkombinationen werden zentral verarbeitet, bevor die betroffenen Tastaturereignisse an reguläre Anwendungen weitergeleitet werden.

## 2. Architektur

Das Subsystem umfasst folgende Komponenten:

| Komponente | Aufgabe |
|---|---|
| Global Shortcut Manager | Zentrale Verwaltung globaler Tastenkombinationen |
| Shortcut Registry | Speicherung registrierter Tastenkombinationen |
| Shortcut Matcher | Erkennung passender Tastenkombinationen |
| Shortcut Conflict Resolver | Auflösung konkurrierender Registrierungen |
| Shortcut Action Dispatcher | Auslösung zugeordneter Aktionen |
| Shortcut Configuration Manager | Verwaltung benutzerspezifischer Einstellungen |

Die Erkennung erfolgt auf Grundlage normalisierter Keyboard Events.

Die Ausführung wird über das Nova Capability System beziehungsweise autorisierte Systemfunktionen abgewickelt.

## 3. Shortcut-Modell

Ein globaler Shortcut besitzt mindestens:

- Eindeutige Shortcut-ID.
- Tastenkombination.
- Zugeordnete Aktion.
- Registrierenden Eigentümer.
- Gültigkeitsbereich.
- Priorität.
- Aktivierungszustand.
- Erforderliche Berechtigungen.

Unterstützte Gültigkeitsbereiche:

| Bereich | Beschreibung |
|---|---|
| System | Systemweite Funktionen |
| Session | Aktuelle Benutzersitzung |
| User | Benutzerbezogene Tastenkombinationen |
| Context | Bestimmter Systemkontext |

Anwendungslokale Tastenkombinationen werden weiterhin durch die jeweilige Anwendung beziehungsweise Nova UI verarbeitet.

## 4. Tastenkombinationen

NovaOS unterstützt Kombinationen aus:

- Einer logischen Taste.
- Einer oder mehreren Modifikatortasten.
- Optional einer definierten Tastensequenz.

Beispiele:

- `Ctrl + C`
- `Ctrl + Shift + Esc`
- `Super + E`
- `Alt + Tab`

Die Beispiele beschreiben mögliche Kombinationen und legen keine verbindliche Standardbelegung fest.

Die Erkennung MUSS linke und rechte Modifikatortasten bei Bedarf unterscheiden können.

Tastensequenzen MÜSSEN definierte Zeitlimits besitzen.

Physische Tastencodes DÜRFEN für layoutunabhängige Systemfunktionen verwendet werden.

## 5. Prioritäten

NovaOS verwendet folgende Prioritätsreihenfolge:

1. Sicherheitskritische Systemkombinationen.
2. Reservierte Betriebssystemkombinationen.
3. Autorisierte globale Systemfunktionen.
4. Benutzerdefinierte globale Shortcuts.
5. Anwendungslokale Tastenkombinationen.

Höher priorisierte Kombinationen besitzen Vorrang.

Anwendungen DÜRFEN sicherheitskritische oder reservierte Systemkombinationen nicht überschreiben.

Die Konfliktauflösung MUSS deterministisch erfolgen.

## 6. Registrierung und Konfliktbehandlung

Globale Shortcuts werden über eine zentrale API registriert.

Dabei gilt:

- Jede Registrierung besitzt einen eindeutig identifizierbaren Eigentümer.
- Registrierungen werden vor der Aktivierung validiert.
- Berechtigungen werden durch Nova.Trust überprüft.
- Konflikte werden vor der Aktivierung erkannt.
- Nicht auflösbare Konflikte führen zur Ablehnung der Registrierung.
- Registrierungen werden bei Beendigung ihres Eigentümers automatisch freigegeben.

Eine Registrierung DARF bestehende höher priorisierte Shortcuts nicht unbemerkt ersetzen.

## 7. Ereignisverarbeitung

Die Verarbeitung erfolgt in folgender Reihenfolge:

1. Empfang normalisierter Keyboard Events.
2. Aktualisierung des Tastaturzustands.
3. Prüfung systemreservierter Kombinationen.
4. Abgleich mit registrierten globalen Shortcuts.
5. Prüfung von Kontext und Berechtigungen.
6. Auslösung der zugeordneten Aktion.
7. Weiterleitung nicht beanspruchter Eingaben an das reguläre Event Routing.

Erfolgreich beanspruchte Tastenkombinationen werden nicht zusätzlich als reguläre Anwendungsaktion ausgelöst.

Bereits begonnene Eingabesequenzen MÜSSEN konsistent abgeschlossen oder kontrolliert abgebrochen werden.

Die Verarbeitung DARF keine dauerhaft gedrückten Tasten in Anwendungen verursachen.

## 8. Aktionen und Capabilities

Ein globaler Shortcut kann auslösen:

- Systemfunktionen.
- Fensterverwaltung.
- Desktopfunktionen.
- Barrierefreiheitsfunktionen.
- Autorisierte Capabilities.
- Benutzerdefinierte Aktionen.

Die Shortcut-Registrierung verleiht keine Berechtigung zur Ausführung der zugeordneten Aktion.

Die erforderlichen Capabilities MÜSSEN bei der Ausführung weiterhin geprüft werden.

Fehlgeschlagene Aktionen DÜRFEN die Eingabeverarbeitung nicht blockieren.

## 9. Benutzerkonfiguration

NovaOS ermöglicht die Anpassung globaler Tastenkombinationen.

Dazu gehören:

- Tastenkombination ändern.
- Shortcut aktivieren oder deaktivieren.
- Eigene Shortcuts erstellen.
- Konflikte anzeigen.
- Standardbelegung wiederherstellen.

Benutzereinstellungen SOLLEN dauerhaft und portierbar gespeichert werden können.

Sicherheitskritische Systemkombinationen bleiben von regulären Benutzeranpassungen ausgeschlossen.

## 10. Sicherheit und Isolation

Nova Global Shortcuts unterliegt Nova Input Security und Nova.Trust.

Festlegungen:

- Globale Registrierungen erfordern entsprechende Berechtigungen.
- Anwendungen DÜRFEN keine globalen Tastatureingaben unberechtigt überwachen.
- Registrierte Shortcuts erhalten ausschließlich ihre Auslöseereignisse.
- Geschützte Eingabekontexte können reguläre globale Shortcuts unterdrücken.
- Sicherheitskritische Systemkombinationen bleiben jederzeit verfügbar.
- Virtuelle und injizierte Eingaben DÜRFEN geschützte Systemaktionen nicht unberechtigt auslösen.
- Sitzungsübergreifende Shortcut-Ausführungen sind ohne ausdrückliche Berechtigung unzulässig.

Die Registrierung eines Shortcuts DARF nicht als allgemeiner Tastatur-Hook verwendet werden.

## 11. Fehlerbehandlung und Performance

Die Shortcut-Erkennung erfolgt ereignisgesteuert.

Dabei gilt:

- Der Abgleich MUSS ohne unnötige Verzögerung erfolgen.
- Registrierte Kombinationen SOLLEN in einer effizienten Suchstruktur verwaltet werden.
- Fehlerhafte Registrierungen werden zurückgewiesen.
- Nicht erreichbare Aktionsempfänger werden kontrolliert behandelt.
- Zeitüberschreitungen bei Tastensequenzen werden automatisch bereinigt.
- Fehler einzelner Shortcuts DÜRFEN andere Eingaben nicht beeinträchtigen.

Die Erkennung erfolgt unabhängig von Nova UI und der Rendering-Frequenz.

## 12. Normative Anforderungen

- **INPUT-001:** NovaOS MUSS eine zentrale Verwaltung globaler Tastenkombinationen bereitstellen.
- **INPUT-002:** Globale Shortcuts MÜSSEN unabhängig vom aktiven Anwendungsfenster funktionieren.
- **INPUT-003:** Die Erkennung MUSS normalisierte Keyboard Events verwenden.
- **INPUT-004:** Sicherheitskritische Systemkombinationen MÜSSEN höchste Priorität besitzen.
- **INPUT-005:** Anwendungen DÜRFEN reservierte Systemkombinationen nicht überschreiben.
- **INPUT-006:** Globale Shortcut-Registrierungen MÜSSEN autorisiert werden.
- **INPUT-007:** Konflikte zwischen Registrierungen MÜSSEN deterministisch behandelt werden.
- **INPUT-008:** Registrierungen MÜSSEN eindeutig einem Eigentümer zugeordnet sein.
- **INPUT-009:** Registrierungen MÜSSEN bei Beendigung ihres Eigentümers automatisch freigegeben werden.
- **INPUT-010:** Erfolgreich beanspruchte Shortcuts DÜRFEN keine unbeabsichtigten zusätzlichen Anwendungsaktionen auslösen.
- **INPUT-011:** Benutzerdefinierte globale Shortcuts MÜSSEN unterstützt werden.
- **INPUT-012:** Die Ausführung einer Shortcut-Aktion MUSS die erforderlichen Capabilities überprüfen.
- **INPUT-013:** Geschützte Eingabekontexte MÜSSEN reguläre globale Shortcuts unterdrücken können.
- **INPUT-014:** Virtuelle Eingaben DÜRFEN keine Sicherheitsrichtlinien umgehen.
- **INPUT-015:** Die Shortcut-Erkennung MUSS unabhängig von Nova UI und der Rendering-Frequenz funktionieren.
- **INPUT-016:** Identische Eingabeereignisse und Konfigurationen MÜSSEN reproduzierbare Erkennungsergebnisse liefern.
- **INPUT-017:** Nova Global Shortcuts MUSS vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 13. Integration

- **Nova Input Architecture:** Zentrale Eingabeverarbeitung.
- **Nova Keyboard Events:** Normalisierte Tastaturereignisse.
- **Nova Input Mapping Engine:** Logische Tastenzuordnung.
- **Nova Multi-Device Input:** Gemeinsame Tastenzustände.
- **Nova Event Routing:** Weiterleitung nicht beanspruchter Eingaben.
- **Nova Event Dispatcher:** Ereigniszustellung.
- **Nova Input Focus Manager:** Eingabefokus und geschützte Kontexte.
- **Nova Window Manager:** Fensterbezogene Systemaktionen.
- **Nova Capability System:** Ausführung autorisierter Funktionen.
- **Nova UI:** Konfiguration und Darstellung von Shortcuts.
- **Nova.Trust:** Berechtigungen und Sicherheitsrichtlinien.

## 14. Ergebnis

NovaOS erhält eine zentrale und sichere Architektur für globale Tastenkombinationen.

Systemfunktionen, Capabilities und benutzerdefinierte Aktionen können unabhängig von der aktiven Anwendung über einheitliche Shortcuts ausgelöst werden.

Durch definierte Prioritäten, kontrollierte Registrierung und deterministische Konfliktauflösung bleibt die Tastaturbedienung konsistent und zuverlässig.

**Grundsatz:** Globale Tastenkombinationen erleichtern die Bedienung des Systems, ohne Anwendungen unkontrollierten Zugriff auf Tastatureingaben oder geschützte Systemfunktionen zu gewähren.