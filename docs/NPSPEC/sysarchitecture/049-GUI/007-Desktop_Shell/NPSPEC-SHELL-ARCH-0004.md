# NPSPEC-SHELL-ARCH-0004 – Nova Shell Recovery

**Status:** Angenommen  
**Bereich:** Shell / Architecture / Recovery  
**System:** NovaOS

## 1. Zweck

Nova Shell Recovery definiert die automatische Fehlererkennung und Wiederherstellung der NovaOS Desktop Shell.

Ziel ist, Fehler möglichst unbemerkt zu beheben, ohne laufende Anwendungen, Benutzerdaten oder die gesamte grafische Sitzung zu beeinträchtigen.

Die Wiederherstellung MUSS unabhängig von einem vollständigen Betriebssystemneustart möglich sein.

## 2. Architektur

Das Recovery-System besteht aus folgenden Komponenten:

| Komponente | Aufgabe |
|---|---|
| Shell Health Monitor | Überwachung des Shell-Zustands |
| Shell Recovery Manager | Steuerung der Wiederherstellung |
| Component Recovery | Wiederherstellung einzelner Shell-Komponenten |
| Shell State Recovery | Wiederherstellung gespeicherter Zustände |
| Recovery Supervisor | Unabhängige Überwachung der Shell |
| Recovery Diagnostics | Fehlerprotokollierung und Diagnose |

Der Recovery Supervisor MUSS außerhalb des überwachten Shell-Prozesses arbeiten, damit ein vollständiger Shell-Absturz erkannt werden kann.

## 3. Fehlererkennung

Nova Shell Recovery erkennt insbesondere:

- Abstürze der Desktop Shell.
- Nicht reagierende Shell-Komponenten.
- Fehlerhafte Erweiterungen.
- Ungültige Shell-Zustände.
- Wiederholte Initialisierungsfehler.
- Übermäßigen Ressourcenverbrauch.
- Fehlgeschlagene Komponentenstarts.

Dabei gilt:

- Fehler MÜSSEN eindeutig einer Komponente zugeordnet werden können.
- Kurzzeitige Verzögerungen DÜRFEN nicht automatisch als Absturz bewertet werden.
- Wiederholte Fehler MÜSSEN erkannt werden.
- Die Überwachung DARF keine permanente grafische Aktualisierung verursachen.

## 4. Wiederherstellungsstufen

NovaOS verwendet eine abgestufte Wiederherstellung.

| Stufe | Maßnahme |
|---|---|
| 1 | Fehlgeschlagene Operation erneut ausführen |
| 2 | Betroffene Komponente zurücksetzen |
| 3 | Fehlerhafte Erweiterung isolieren oder deaktivieren |
| 4 | Betroffene Shell-Komponente neu starten |
| 5 | Gesamte Desktop Shell neu starten |
| 6 | Minimale Recovery Shell aktivieren |

Die jeweils geringstmögliche wirksame Wiederherstellungsstufe SOLL verwendet werden.

Ein vollständiger Shell-Neustart DARF nicht die Standardreaktion auf jeden Komponentenfehler sein.

## 5. Automatische Wiederherstellung

NovaOS SOLL behebbare Shell-Fehler automatisch korrigieren.

Dabei gilt:

- Unkritische Reparaturen DÜRFEN ohne Benutzerinteraktion erfolgen.
- Änderungen an Berechtigungen oder sicherheitsrelevanten Einstellungen MÜSSEN gesondert autorisiert werden.
- Wiederherstellungsversuche MÜSSEN begrenzt werden.
- Wiederholt fehlschlagende Komponenten SOLLEN vorübergehend deaktiviert werden.
- Erfolgreiche Wiederherstellungen MÜSSEN protokolliert werden.

Der Benutzer SOLL nur dann unterbrochen werden, wenn seine Entscheidung erforderlich ist.

## 6. Erhaltung laufender Anwendungen

Die Desktop Shell ist von Anwendungen und der grundlegenden Fensterverwaltung getrennt.

Dabei gilt:

- Ein Shell-Neustart DARF laufende Anwendungen nicht automatisch beenden.
- Der Window Manager SOLL bestehende Fenster weiter verwalten.
- Display Server und Compositor SOLLEN unabhängig weiterarbeiten.
- Bestehende Anwendungszustände DÜRFEN nicht durch Shell Recovery verändert werden.
- Shell-Integrationen MÜSSEN nach einem Neustart erneut verbunden werden können.

Kann eine Anwendung nicht erhalten werden, MUSS die Ursache nachvollziehbar dokumentiert werden.

## 7. Zustandswiederherstellung

Nova Shell Recovery verwendet den Nova Shell State Manager.

Wiederherstellbare Informationen umfassen:

- Desktop-Layout.
- Taskleistenkonfiguration.
- Angeheftete Elemente.
- Startmenü-Einstellungen.
- Shell-Navigation.
- Benachrichtigungseinstellungen.
- Benutzerdefinierte Anpassungen.

Dabei gilt:

- Zustände MÜSSEN konsistent gespeichert werden.
- Beschädigte Zustandsdaten MÜSSEN erkannt werden.
- Der letzte gültige Zustand SOLL wiederhergestellt werden.
- Ein sicherer Standardzustand MUSS verfügbar sein.
- Fehlerhafte Zustandsdaten DÜRFEN keine endlose Neustartschleife verursachen.

## 8. Minimale Recovery Shell

Kann die reguläre Desktop Shell nicht wiederhergestellt werden, startet NovaOS eine minimale Recovery Shell.

Diese MUSS mindestens ermöglichen:

- Anzeige des Fehlerzustands.
- Neustart der regulären Shell.
- Deaktivierung fehlerhafter Erweiterungen.
- Wiederherstellung einer gültigen Shell-Konfiguration.
- Zugriff auf grundlegende Diagnosefunktionen.
- Kontrollierte Abmeldung oder Systemneustart.

Die minimale Recovery Shell MUSS möglichst wenige Abhängigkeiten besitzen und DARF keine optionalen Shell-Erweiterungen benötigen.

## 9. Fehlerdiagnose

Alle Wiederherstellungsvorgänge werden zentral dokumentiert.

Erfasst werden SOLLEN:

- Betroffene Komponente.
- Fehlerursache.
- Zeitpunkt.
- Ausgeführte Wiederherstellungsmaßnahme.
- Ergebnis.
- Anzahl wiederholter Fehler.

Die Informationen MÜSSEN über die NovaOS-Wartungskonsole zugänglich sein.

Der Benutzer SOLL erkennen können, welcher Fehler aufgetreten ist und wie NovaOS ihn behoben hat.

## 10. Benutzerinteraktion

Nova Shell Recovery folgt der NovaOS-Philosophie einer möglichst unaufdringlichen Benutzerumgebung.

Dabei gilt:

- Erfolgreich behobene unkritische Fehler SOLLEN keine störenden Dialoge erzeugen.
- Relevante Wiederherstellungen DÜRFEN dezent mitgeteilt werden.
- Wiederkehrende Probleme SOLLEN dem Benutzer angezeigt werden.
- Kritische Fehler MÜSSEN eindeutig kommuniziert werden.
- Automatische Änderungen MÜSSEN nachvollziehbar und, soweit technisch möglich, rückgängig zu machen sein.

## 11. Sicherheit

Nova Shell Recovery unterliegt Nova.Trust.

Dabei gilt:

- Recovery-Komponenten MÜSSEN gegen Manipulation geschützt sein.
- Wiederherstellungen DÜRFEN keine zusätzlichen Berechtigungen verleihen.
- Fehlerhafte Erweiterungen MÜSSEN isoliert werden können.
- Wiederhergestellte Konfigurationen MÜSSEN validiert werden.
- Recovery-Vorgänge MÜSSEN nachvollziehbar protokolliert werden.
- Ein Shell-Absturz DARF keine Sicherheitsgrenzen aufheben.

## 12. Normative Anforderungen

- **SHELL-001:** NovaOS MUSS einen unabhängigen Shell Recovery Supervisor bereitstellen.
- **SHELL-002:** Shell-Abstürze und nicht reagierende Komponenten MÜSSEN erkannt werden.
- **SHELL-003:** Wiederherstellungen MÜSSEN abgestuft erfolgen.
- **SHELL-004:** Einzelne Komponenten SOLLEN unabhängig wiederhergestellt werden.
- **SHELL-005:** Fehlerhafte Erweiterungen MÜSSEN isoliert werden können.
- **SHELL-006:** Ein Shell-Neustart DARF laufende Anwendungen nicht automatisch beenden.
- **SHELL-007:** Gültige Shell-Zustände MÜSSEN wiederherstellbar sein.
- **SHELL-008:** Beschädigte Zustandsdaten DÜRFEN keine endlosen Neustartschleifen verursachen.
- **SHELL-009:** Eine minimale Recovery Shell MUSS verfügbar sein.
- **SHELL-010:** Wiederherstellungsversuche MÜSSEN begrenzt werden.
- **SHELL-011:** Fehler und Wiederherstellungen MÜSSEN protokolliert werden.
- **SHELL-012:** Unkritische Fehler SOLLEN möglichst ohne Benutzerunterbrechung behoben werden.
- **SHELL-013:** Sicherheitsrelevante Änderungen MÜSSEN autorisiert werden.
- **SHELL-014:** Recovery-Vorgänge MÜSSEN Nova.Trust berücksichtigen.
- **SHELL-015:** Nova Shell Recovery MUSS vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 13. Integration

- **Nova Desktop Shell Architecture:** Grundlegende Shell-Architektur.
- **Nova Shell Lifecycle:** Zustandsübergänge und Neustart.
- **Nova Shell Extension Model:** Isolation fehlerhafter Erweiterungen.
- **Nova Shell State Manager:** Zustandsspeicherung und Wiederherstellung.
- **Nova Window Manager:** Erhaltung bestehender Fenster.
- **Nova Compositor:** Fortsetzung der grafischen Darstellung.
- **Nova Display Server:** Unabhängige Grafikausgabe.
- **Nova Recovery System:** Systemweite Wiederherstellungsmechanismen.
- **Nova Diagnostics:** Fehleranalyse und Wartungskonsole.
- **Nova.Trust:** Sicherheitsprüfung und Isolation.

## 14. Ergebnis

NovaOS erhält ein automatisches und abgestuftes Wiederherstellungssystem für seine Desktop Shell.

Fehlerhafte Komponenten können isoliert, neu gestartet oder deaktiviert werden, ohne unmittelbar die gesamte Desktopumgebung oder laufende Anwendungen zu beeinträchtigen.

Bei schwerwiegenden Fehlern kann die Shell unabhängig vom Kernel neu gestartet oder durch eine minimale Recovery Shell ersetzt werden.

**Grundsatz:** Ein Fehler der Desktop Shell darf nicht zum Verlust der Arbeit führen. NovaOS soll Probleme selbstständig beheben und den Benutzer nur dann einbeziehen, wenn es notwendig ist.