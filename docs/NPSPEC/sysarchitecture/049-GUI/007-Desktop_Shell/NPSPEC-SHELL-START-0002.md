# NPSPEC-SHELL-START-0002 – Nova Application Launcher

**Status:** Angenommen  
**Bereich:** Shell / Start / Application Launcher  
**System:** NovaOS

## 1. Zweck

Nova Application Launcher definiert den zentralen Mechanismus zum Starten von Anwendungen, Solutions und ausführbaren Capabilities innerhalb der NovaOS Desktop Shell.

Der Launcher MUSS unabhängig vom Startmenü verwendbar sein und unterschiedliche Einstiegspunkte unterstützen.

Ziel ist ein schneller, sicherer und einheitlicher Startvorgang, unabhängig davon, wo und wie der Benutzer eine Funktion aufruft.

## 2. Architektur

Das Subsystem besteht aus folgenden Komponenten:

| Komponente | Aufgabe |
|---|---|
| Launcher Manager | Koordination von Startanforderungen |
| Launch Resolver | Auflösung von Anwendungs- und Funktionsreferenzen |
| Launch Registry | Bereitstellung registrierter Starteinträge |
| Launch Policy Manager | Prüfung von Startbedingungen |
| Launch Controller | Übergabe an die zuständige Ausführungsumgebung |
| Launch State Tracker | Verfolgung des Startvorgangs |

Der Launcher DARF keine eigene Prozessverwaltung oder Capability-Runtime implementieren.

## 3. Unterstützte Startziele

Nova Application Launcher unterstützt:

- Native NovaOS-Anwendungen.
- Portable Anwendungen.
- Solutions.
- Ausführbare Capabilities.
- Registrierte Systemwerkzeuge.
- Dokumente mit zugeordneten Anwendungen.
- Verknüpfungen zu unterstützten Startzielen.

Dabei gilt:

- Jedes Startziel MUSS eindeutig identifizierbar sein.
- Startreferenzen MÜSSEN unabhängig vom jeweiligen UI-Element auflösbar sein.
- Dokumente MÜSSEN über ihre registrierte Zuordnung geöffnet werden können.
- Nicht verfügbare Startziele MÜSSEN kontrolliert behandelt werden.

## 4. Startanforderung

Eine Startanforderung enthält mindestens:

- Identität des Startziels.
- Aufrufenden Sicherheitskontext.
- Optionale Startparameter.
- Optionales Dokument oder Datenobjekt.
- Gewünschten Workspace und Monitor, sofern angegeben.
- Eindeutige Anforderungs-ID.

Dabei gilt:

- Startanforderungen MÜSSEN strukturiert übergeben werden.
- Parameter MÜSSEN typisiert oder eindeutig serialisiert sein.
- Unvertrauenswürdige Eingaben MÜSSEN validiert werden.
- Startanforderungen DÜRFEN keine Berechtigungen des Aufrufers umgehen.

## 5. Startauflösung

Der Launch Resolver ermittelt die geeignete Ausführungsumgebung.

Dabei gilt:

- Native Anwendungen MÜSSEN an das Nova Application Management übergeben werden.
- Portable Programme MÜSSEN entsprechend ihrer unterstützten Zielplattform gestartet werden.
- Solutions MÜSSEN über die Nova Solution Runtime aktiviert werden.
- Capabilities MÜSSEN über die zuständige Capability Runtime aufgerufen werden.
- Nicht unterstützte oder fehlende Laufzeitumgebungen MÜSSEN eindeutig gemeldet werden.
- Die Auswahl der Ausführungsumgebung MUSS deterministisch erfolgen.

## 6. Startverhalten

Der Launcher unterstützt:

- Start einer neuen Instanz.
- Aktivierung einer vorhandenen Instanz.
- Öffnen eines Dokuments.
- Übergabe von Startparametern.
- Start in einem bestimmten Workspace.
- Start mit definiertem Benutzerkontext.

Dabei gilt:

- Anwendungen MÜSSEN ihr unterstütztes Instanzverhalten deklarieren können.
- Vorhandene Instanzen DÜRFEN nur entsprechend der jeweiligen Startpolicy wiederverwendet werden.
- Der Launcher MUSS zwischen Starten und Aktivieren unterscheiden.
- Fensteraktivierungen MÜSSEN über den Nova Window Manager erfolgen.

## 7. Startablauf

Der Startvorgang umfasst:

1. Entgegennahme der Startanforderung.
2. Auflösung des Startziels.
3. Prüfung von Identität, Signatur und Berechtigungen.
4. Auswahl der Ausführungsumgebung.
5. Übergabe der Startanforderung.
6. Rückmeldung des Startstatus.

Der Launcher MUSS Startvorgänge asynchron ausführen können.

Die Desktop Shell DARF während eines Startvorgangs nicht blockiert werden.

## 8. Startstatus und Rückmeldung

Folgende Zustände werden unterstützt:

| Zustand | Bedeutung |
|---|---|
| Requested | Start wurde angefordert |
| Resolving | Startziel wird aufgelöst |
| Authorizing | Sicherheitsprüfung läuft |
| Launching | Start wird ausgeführt |
| Started | Start wurde bestätigt |
| Failed | Start fehlgeschlagen |
| Cancelled | Start wurde abgebrochen |

Dabei gilt:

- Statusänderungen MÜSSEN ereignisgesteuert bereitgestellt werden.
- Die Taskleiste SOLL laufende Startvorgänge visualisieren können.
- Fehlermeldungen MÜSSEN verständlich sein.
- Ein bestätigter Prozessstart DARF nicht automatisch als vollständig betriebsbereite Anwendung interpretiert werden.
- Startanforderungen MÜSSEN eindeutig nachverfolgbar sein.

## 9. Integration in die Desktop Shell

Nova Application Launcher MUSS über mehrere Einstiegspunkte erreichbar sein:

- Startmenü.
- Taskleiste.
- Desktop-Symbole.
- Dateimanager.
- Suchfunktion.
- Tastenkombinationen.
- Solutions und autorisierte Capabilities.

Alle Einstiegspunkte MÜSSEN denselben grundlegenden Startmechanismus verwenden.

Die jeweilige Oberfläche DARF keine eigene konkurrierende Startlogik implementieren.

## 10. Performance und Fehlerbehandlung

Der Launcher MUSS ressourcenschonend arbeiten.

Dabei gilt:

- Registrierte Startinformationen SOLLEN zwischengespeichert werden.
- Startanforderungen MÜSSEN ohne unnötige Wartezeiten verarbeitet werden.
- Langsame Ausführungsumgebungen DÜRFEN die Shell nicht blockieren.
- Fehlgeschlagene Starts DÜRFEN keine verwaisten Launcher-Zustände hinterlassen.
- Wiederholte Startanforderungen MÜSSEN entsprechend der Instanzpolicy behandelt werden.
- Fehler MÜSSEN über Nova Diagnostics nachvollziehbar sein.

## 11. Sicherheit

Nova Application Launcher unterliegt Nova.Trust.

Dabei gilt:

- Startziele MÜSSEN entsprechend ihrer Sicherheitsrichtlinien überprüft werden.
- Signaturen und Herkunftsinformationen MÜSSEN berücksichtigt werden.
- Erforderliche Capabilities MÜSSEN vor der Ausführung autorisiert werden.
- Anwendungen DÜRFEN durch den Launcher keine zusätzlichen Berechtigungen erhalten.
- Erhöhte Rechte MÜSSEN über die vorgesehenen Authentifizierungs- und Berechtigungsmechanismen angefordert werden.
- Nicht vertrauenswürdige Startparameter DÜRFEN keine unkontrollierte Befehlsausführung ermöglichen.

## 12. Normative Anforderungen

- **START-LAUNCH-001:** NovaOS MUSS einen zentralen Application Launcher bereitstellen.
- **START-LAUNCH-002:** Der Launcher MUSS unabhängig vom Startmenü verwendbar sein.
- **START-LAUNCH-003:** Anwendungen, Solutions und Capabilities MÜSSEN unterstützt werden.
- **START-LAUNCH-004:** Startanforderungen MÜSSEN strukturiert und eindeutig identifizierbar sein.
- **START-LAUNCH-005:** Startziele MÜSSEN über registrierte Identitäten aufgelöst werden.
- **START-LAUNCH-006:** Die Ausführungsumgebung MUSS deterministisch ausgewählt werden.
- **START-LAUNCH-007:** Startvorgänge MÜSSEN asynchron ausführbar sein.
- **START-LAUNCH-008:** Instanzrichtlinien MÜSSEN berücksichtigt werden.
- **START-LAUNCH-009:** Sicherheitsprüfungen MÜSSEN vor der Ausführung erfolgen.
- **START-LAUNCH-010:** Startstatus und Fehler MÜSSEN nachvollziehbar sein.
- **START-LAUNCH-011:** Die Desktop Shell DARF durch Startvorgänge nicht blockiert werden.
- **START-LAUNCH-012:** Der Launcher DARF keine eigene Prozess- oder Fensterverwaltung implementieren.
- **START-LAUNCH-013:** Alle Shell-Einstiegspunkte MÜSSEN denselben Startmechanismus verwenden können.
- **START-LAUNCH-014:** Nova Application Launcher MUSS vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 13. Integration

- **Nova Start Menu:** Starten registrierter Elemente.
- **Nova Taskbar Application Management:** Anwendungsstart und Statusanzeige.
- **Nova Desktop Icons:** Starten über Desktop-Verknüpfungen.
- **Nova Application Management:** Anwendungsidentitäten und Ausführung.
- **Nova Process Management:** Prozesserzeugung und Lebenszyklus.
- **Nova Solution Runtime:** Aktivierung von Solutions.
- **Nova Capability System:** Ausführung autorisierter Funktionen.
- **Nova Window Manager:** Fensteraktivierung und Workspace-Zuordnung.
- **Nova VFS:** Auflösung von Dateien und Startreferenzen.
- **Nova.Trust:** Identität, Signaturen und Berechtigungen.
- **Nova Diagnostics:** Fehleranalyse und Startprotokollierung.

## 14. Ergebnis

NovaOS erhält einen zentralen, sicheren und erweiterbaren Mechanismus zum Starten von Anwendungen, Solutions und Capabilities.

Unterschiedliche Oberflächen können Startanforderungen einheitlich übergeben, während die tatsächliche Ausführung durch die zuständigen Systemkomponenten erfolgt.

**Grundsatz:** Der Application Launcher entscheidet, wie ein Startziel aufgerufen wird – die zuständige Runtime entscheidet, wie es ausgeführt wird.