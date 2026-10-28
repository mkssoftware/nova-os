# NPSPEC-SHELL-START-0003 – Nova Application Search

**Status:** Angenommen  
**Bereich:** Shell / Start / Application Search  
**System:** NovaOS

## 1. Zweck

Nova Application Search definiert die Suche nach Anwendungen, Solutions, Capabilities und Systemfunktionen innerhalb der NovaOS Desktop Shell.

Ziel ist eine schnelle, intuitive und vollständig offlinefähige Suche, die verfügbare Funktionen unabhängig von ihrem Speicherort oder ihrer technischen Implementierung auffindbar macht.

Die Suche MUSS ohne KI funktionieren. Eine optionale KI-gestützte Interpretation DARF die deterministische Suchfunktion ergänzen.

## 2. Architektur

Das Subsystem besteht aus folgenden Komponenten:

| Komponente | Aufgabe |
|---|---|
| Application Search Manager | Koordination von Suchanfragen |
| Search Registry | Verwaltung durchsuchbarer Einträge |
| Search Index | Bereitstellung indexierter Suchdaten |
| Query Processor | Normalisierung und Verarbeitung von Suchanfragen |
| Search Matcher | Ermittlung passender Einträge |
| Result Ranker | Bewertung und Sortierung |
| Search Result Provider | Bereitstellung der Ergebnisse |

Nova Application Search MUSS über definierte Schnittstellen in unterschiedliche Shell-Oberflächen integrierbar sein.

## 3. Durchsuchbare Inhalte

Die Suche unterstützt:

- Installierte Anwendungen.
- Portable Anwendungen mit Registrierung.
- Solutions.
- Capabilities.
- Systemeinstellungen.
- Systemfunktionen.
- Registrierte Benutzerwerkzeuge.
- Benutzerdefinierte Verknüpfungen.

Dabei gilt:

- Jeder Suchtreffer MUSS eine eindeutige Identität besitzen.
- Suchbare Einträge MÜSSEN unabhängig von ihrer Darstellung registrierbar sein.
- Capabilities MÜSSEN anhand ihrer Funktionen auffindbar sein können.
- Nicht verfügbare oder nicht autorisierte Einträge MÜSSEN entsprechend behandelt werden.
- Eine allgemeine Dateiinhalts- und Websuche gehört nicht zum Kern dieses Subsystems.

## 4. Suchmetadaten

Ein registrierter Sucheintrag enthält mindestens:

- Eindeutige ID.
- Anzeigename.
- Elementtyp.
- Start- oder Aktionsreferenz.
- Registrierten Eigentümer.

Optional können enthalten sein:

- Beschreibung.
- Symbol.
- Schlüsselwörter.
- Synonyme.
- Kategorien.
- Lokalisierte Bezeichnungen.
- Unterstützte Aktionen.

Suchmetadaten MÜSSEN strukturiert und unabhängig von der jeweiligen Benutzeroberfläche bereitgestellt werden.

## 5. Suchverarbeitung

Der Query Processor verarbeitet Benutzereingaben.

Unterstützt werden:

- Groß- und Kleinschreibungsunabhängige Suche.
- Präfixsuche.
- Teilwortsuche.
- Suche nach mehreren Begriffen.
- Schlüsselwortsuche.
- Synonyme.
- Toleranz gegenüber einfachen Schreibfehlern.

Dabei gilt:

- Suchanfragen MÜSSEN deterministisch verarbeitet werden können.
- Die Suche MUSS bereits während der Eingabe Ergebnisse bereitstellen können.
- Neue Suchanfragen MÜSSEN veraltete Suchvorgänge ersetzen können.
- Die Verarbeitung DARF die UI Event Loop nicht blockieren.

## 6. Ergebnisbewertung

Der Result Ranker sortiert Suchtreffer anhand definierter Kriterien.

Berücksichtigt werden können:

1. Exakte Übereinstimmung.
2. Übereinstimmung am Wortanfang.
3. Teilweise Namensübereinstimmung.
4. Schlüsselwörter und Synonyme.
5. Kontextbezogene Relevanz.
6. Optional die Nutzungshäufigkeit.

Dabei gilt:

- Die Bewertung MUSS nachvollziehbar und deterministisch sein.
- Exakte Treffer SOLLEN gegenüber unscharfen Treffern bevorzugt werden.
- Benutzerdefinierte Prioritäten DÜRFEN berücksichtigt werden.
- Personalisierte Bewertungen MÜSSEN deaktivierbar sein.
- Gleiche Bewertungen MÜSSEN durch eine stabile Sortierreihenfolge aufgelöst werden.

## 7. Capability-Suche

Nova Application Search MUSS das Capability-basierte Bedienkonzept von NovaOS unterstützen.

Dabei gilt:

- Capabilities MÜSSEN über ihren Namen und ihre beschriebene Funktion gefunden werden können.
- Suchbegriffe DÜRFEN auf semantische Funktionsbeschreibungen abgebildet werden.
- Die Zuordnung MUSS auch ohne KI über registrierte Begriffe und Regeln funktionieren.
- Verfügbare Aktionen MÜSSEN anhand des Benutzerkontextes ermittelt werden.
- Die Suche DARF Capabilities nicht selbst ausführen.

Beispielsweise kann eine registrierte Capability zum Umwandeln von Bildern über Begriffe wie „Bild konvertieren“ oder „Bildformat ändern“ gefunden werden.

## 8. Suchergebnisse und Aktionen

Ein Suchergebnis kann enthalten:

- Symbol.
- Name.
- Beschreibung.
- Kategorie.
- Primäre Aktion.
- Optionale Kontextaktionen.

Dabei gilt:

- Anwendungen MÜSSEN über Nova Application Launcher gestartet werden.
- Solutions MÜSSEN über ihre zuständige Runtime aktiviert werden.
- Capabilities MÜSSEN über autorisierte Capability-Schnittstellen aufgerufen werden.
- Kontextaktionen MÜSSEN über das Nova Command-System bereitgestellt werden.
- Suchergebnisse DÜRFEN keine eigenen Ausführungsmechanismen implementieren.

## 9. Suchindex und Aktualisierung

Der Search Index verwaltet die registrierten Suchinformationen.

Dabei gilt:

- Registrierungen und Änderungen MÜSSEN ereignisgesteuert übernommen werden.
- Installationen und Deinstallationen MÜSSEN den Index aktualisieren können.
- Der Index SOLL inkrementell aktualisiert werden.
- Häufig benötigte Suchdaten SOLLEN im Arbeitsspeicher verfügbar sein.
- Beschädigte Indexdaten MÜSSEN neu aufgebaut werden können.
- Eine Suche MUSS auch bei fehlendem Index über die Registry möglich bleiben.

## 10. Benutzeroberfläche

Nova Application Search verwendet das native Nova UI Framework.

Dabei gilt:

- Die Suche MUSS in das Startmenü integrierbar sein.
- Eine eigenständige Suchoberfläche DARF bereitgestellt werden.
- Ergebnisse MÜSSEN per Tastatur auswählbar sein.
- Unterschiedliche Ergebnistypen MÜSSEN erkennbar sein.
- High Contrast und Screenreader MÜSSEN unterstützt werden.
- Suchergebnisse SOLLEN ohne unnötige Animationen aktualisiert werden.

## 11. Performance und Ressourcenverwaltung

Die Suche MUSS für eine unmittelbare Benutzerinteraktion optimiert sein.

Dabei gilt:

- Suchanfragen MÜSSEN asynchron verarbeitet werden können.
- Veraltete Suchvorgänge SOLLEN abgebrochen werden.
- Suchergebnisse SOLLEN schrittweise bereitgestellt werden können.
- Wiederholte Anfragen DÜRFEN geeignete Zwischenspeicher verwenden.
- Nicht benötigte Suchressourcen MÜSSEN freigegeben werden können.
- Die Suche DARF den Desktopstart nicht unnötig verzögern.

## 12. Sicherheit und Datenschutz

Nova Application Search unterliegt Nova.Trust.

Dabei gilt:

- Suchergebnisse MÜSSEN die Berechtigungen des Benutzers berücksichtigen.
- Nicht autorisierte Informationen DÜRFEN nicht über Suchmetadaten offengelegt werden.
- Suchanbieter DÜRFEN nur autorisierte Inhalte registrieren.
- Suchanfragen DÜRFEN nicht ohne Zustimmung an externe Dienste übertragen werden.
- Ein Suchverlauf MUSS deaktivierbar und löschbar sein, sofern er gespeichert wird.
- Die lokale Suche MUSS vollständig ohne Netzwerkverbindung funktionieren.

## 13. Normative Anforderungen

- **START-SEARCH-001:** NovaOS MUSS eine zentrale Application Search bereitstellen.
- **START-SEARCH-002:** Anwendungen, Solutions und Capabilities MÜSSEN durchsuchbar sein.
- **START-SEARCH-003:** Sucheinträge MÜSSEN strukturiert registriert werden.
- **START-SEARCH-004:** Die Suche MUSS ohne KI funktionieren.
- **START-SEARCH-005:** Suchanfragen MÜSSEN deterministisch verarbeitet werden können.
- **START-SEARCH-006:** Präfix-, Teilwort- und Schlüsselwortsuche MÜSSEN unterstützt werden.
- **START-SEARCH-007:** Suchergebnisse MÜSSEN nachvollziehbar sortiert werden.
- **START-SEARCH-008:** Änderungen an Registrierungen MÜSSEN ereignisgesteuert berücksichtigt werden.
- **START-SEARCH-009:** Suchvorgänge DÜRFEN die UI Event Loop nicht blockieren.
- **START-SEARCH-010:** Die Suche MUSS auch ohne vorhandenen Index funktionsfähig bleiben.
- **START-SEARCH-011:** Aktionen MÜSSEN an die zuständigen Launcher- und Capability-Systeme delegiert werden.
- **START-SEARCH-012:** Suchergebnisse MÜSSEN Berechtigungen und Sicherheitsgrenzen berücksichtigen.
- **START-SEARCH-013:** Die Suche MUSS vollständig per Tastatur bedienbar sein.
- **START-SEARCH-014:** Nova Application Search MUSS vollständig offline funktionieren.

## 14. Integration

- **Nova Start Menu:** Integration der Suchoberfläche.
- **Nova Application Launcher:** Starten gefundener Anwendungen.
- **Nova Application Management:** Anwendungsregistrierungen.
- **Nova Capability System:** Suche nach verfügbaren Funktionen.
- **Nova Solutions:** Registrierung und Aktivierung von Solutions.
- **Nova Search:** Gemeinsame Suchinfrastruktur.
- **Nova UI Framework:** Darstellung und Benutzerinteraktion.
- **Nova Shell Extension Model:** Erweiterbare Suchanbieter.
- **Nova.Trust:** Berechtigungen und Datenschutz.

## 15. Ergebnis

NovaOS erhält eine schnelle, deterministische und erweiterbare Suche nach Anwendungen, Solutions, Capabilities und Systemfunktionen.

Durch strukturierte Registrierungen, lokale Indizierung und nachvollziehbare Ergebnisbewertung bleiben Funktionen auch ohne KI zuverlässig auffindbar.

**Grundsatz:** Der Benutzer sucht nach dem, was er tun möchte – NovaOS findet die passende Anwendung oder Fähigkeit.