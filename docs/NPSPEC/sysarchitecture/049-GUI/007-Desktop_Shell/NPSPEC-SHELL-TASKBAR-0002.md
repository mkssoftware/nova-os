# NPSPEC-SHELL-TASKBAR-0002 – Nova Taskbar Application Management

**Status:** Angenommen  
**Bereich:** Shell / Taskbar / Application Management  
**System:** NovaOS

## 1. Zweck

Nova Taskbar Application Management definiert die Verwaltung und Darstellung laufender sowie angehefteter Anwendungen innerhalb der NovaOS-Taskleiste.

Ziel ist ein einheitlicher Zugriff auf Anwendungen, Solutions und deren Fenster, unabhängig von ihrer technischen Implementierung.

Die Taskleiste MUSS Anwendungen verwalten können, ohne deren eigentlichen Lebenszyklus oder Fensterverwaltung selbst zu übernehmen.

## 2. Architektur

Das Subsystem besteht aus folgenden Komponenten:

| Komponente | Aufgabe |
|---|---|
| Application Tracker | Erkennung laufender Anwendungen |
| Application Resolver | Zuordnung von Fenstern zu Anwendungen |
| Taskbar Item Manager | Verwaltung der Taskleisteneinträge |
| Application Group Manager | Gruppierung zusammengehöriger Fenster |
| Pinned Application Manager | Verwaltung angehefteter Anwendungen |
| Application Action Controller | Ausführung von Anwendungsaktionen |

Die Verwaltung der Anwendungsprozesse verbleibt beim zuständigen Prozess- und Anwendungsmanagement von NovaOS.

## 3. Anwendungsmodell

Ein Taskbar-Anwendungseintrag besitzt mindestens:

- Eindeutige Anwendungsidentität.
- Anzeigename.
- Symbol.
- Anwendungstyp.
- Zugeordnete Fenster.
- Startreferenz.
- Aktuellen Zustand.
- Optionalen Workspace- und Monitorbezug.

Dabei gilt:

- Anwendungen MÜSSEN unabhängig von ihren Fenstern identifizierbar sein.
- Mehrere Prozesse DÜRFEN einer Anwendung zugeordnet sein.
- Mehrere Fenster MÜSSEN einer Anwendung zugeordnet werden können.
- Anwendungen ohne sichtbares Fenster MÜSSEN nicht automatisch in der Taskleiste erscheinen.
- Die Taskleiste DARF Anwendungsidentitäten nicht ausschließlich anhand von Prozessnamen bestimmen.

## 4. Anwendungserkennung

Der Application Tracker ermittelt laufende Anwendungen über definierte Systemereignisse.

Dabei gilt:

- Anwendungsstarts und -beendigungen MÜSSEN erkannt werden.
- Fensteränderungen MÜSSEN über den Nova Window Manager gemeldet werden.
- Anwendungen MÜSSEN anhand registrierter Identitäten zugeordnet werden.
- Unbekannte Anwendungen MÜSSEN mit geeigneten Standardinformationen dargestellt werden können.
- Die Erkennung MUSS ereignisgesteuert erfolgen.

Permanentes Abfragen sämtlicher Prozesse oder Fenster SOLL vermieden werden.

## 5. Angeheftete Anwendungen

Der Pinned Application Manager verwaltet dauerhaft verfügbare Taskbar-Einträge.

Dabei gilt:

- Anwendungen und Solutions MÜSSEN angeheftet werden können.
- Angeheftete Elemente MÜSSEN unabhängig vom Ausführungszustand sichtbar bleiben können.
- Die Reihenfolge MUSS veränderbar sein.
- Angeheftete Einträge MÜSSEN benutzerbezogen gespeichert werden.
- Nicht mehr verfügbare Anwendungen MÜSSEN kontrolliert behandelt werden.

Eine angeheftete Anwendung SOLL bei ihrer Ausführung mit dem vorhandenen Taskbar-Eintrag zusammengeführt werden.

## 6. Anwendungsgruppierung

Mehrere Fenster derselben Anwendung können zu einer Gruppe zusammengefasst werden.

Dabei gilt:

- Gruppierung MUSS anhand der Anwendungsidentität erfolgen.
- Der Benutzer MUSS die Gruppierung konfigurieren können.
- Fenster innerhalb einer Gruppe MÜSSEN einzeln erreichbar bleiben.
- Fenstergruppen MÜSSEN ihren Aktivitätszustand darstellen können.
- Die Gruppierung DARF Fenster unterschiedlicher Anwendungen nicht unbeabsichtigt zusammenführen.

## 7. Anwendungszustände

Taskbar-Einträge unterstützen mindestens folgende Zustände:

| Zustand | Bedeutung |
|---|---|
| Pinned | Anwendung ist angeheftet |
| Running | Anwendung wird ausgeführt |
| Active | Zugehöriges Fenster ist aktiv |
| Inactive | Anwendung läuft im Hintergrund |
| Attention | Anwendung benötigt Aufmerksamkeit |
| Launching | Anwendung wird gestartet |
| Unavailable | Anwendung ist nicht verfügbar |

Zustände MÜSSEN über das Nova UI Visual State System dargestellt werden.

## 8. Anwendungsaktionen

Die Taskleiste unterstützt folgende Aktionen:

- Anwendung starten.
- Vorhandenes Fenster aktivieren.
- Zwischen Anwendungsfenstern wechseln.
- Fenster minimieren und wiederherstellen.
- Neue Anwendungsinstanz starten, sofern unterstützt.
- Anwendung anheften oder lösen.
- Anwendungskontextmenü öffnen.
- Anwendung schließen.

Dabei gilt:

- Fensteraktionen MÜSSEN über den Window Manager erfolgen.
- Anwendungsstarts MÜSSEN über das zuständige Anwendungsmanagement erfolgen.
- Schließen und Beenden MÜSSEN unterschieden werden.
- Aktionen MÜSSEN die jeweiligen Capabilities und Berechtigungen berücksichtigen.

## 9. Fenster- und Workspace-Integration

Die Taskleiste MUSS Fenster unterschiedlicher Workspaces und Monitore berücksichtigen.

Dabei gilt:

- Fenster MÜSSEN ihrem Workspace und Monitor zugeordnet werden können.
- Die Anzeige von Fenstern anderer Workspaces MUSS konfigurierbar sein.
- Ein Workspace-Wechsel MUSS unmittelbar berücksichtigt werden.
- Fenster DÜRFEN über definierte Aktionen in andere Workspaces verschoben werden.
- Die Taskleiste DARF keine eigene Workspace- oder Fensterverwaltung implementieren.

## 10. Fenster- und Anwendungsvorschau

NovaOS SOLL Vorschauen laufender Anwendungsfenster unterstützen.

Dabei gilt:

- Vorschauen MÜSSEN über autorisierte Schnittstellen des Window Managers beziehungsweise Compositors bereitgestellt werden.
- Nicht sichtbare Vorschauen DÜRFEN keine unnötigen Aktualisierungen verursachen.
- Geschützte Fensterinhalte MÜSSEN berücksichtigt werden.
- Vorschauen SOLLEN bedarfsgesteuert erzeugt werden.
- Reduced Motion MUSS berücksichtigt werden.

## 11. Zustand und Wiederherstellung

Der Pinned Application Manager speichert dauerhaft:

- Angeheftete Anwendungen.
- Reihenfolge.
- Gruppierungseinstellungen.
- Anzeigeoptionen.
- Benutzerdefinierte Zuordnungen.

Dabei gilt:

- Ein Shell-Neustart DARF angeheftete Elemente nicht verlieren.
- Laufende Anwendungen MÜSSEN nach einem Shell-Neustart erneut erkannt werden.
- Bestehende Fenster DÜRFEN durch den Neustart nicht automatisch beendet werden.
- Beschädigte Konfigurationen MÜSSEN durch gültige Standardwerte ersetzt werden können.

## 12. Performance und Sicherheit

Das Application Management MUSS ressourcenschonend und sicher arbeiten.

Dabei gilt:

- Änderungen MÜSSEN ereignisgesteuert verarbeitet werden.
- Unveränderte Taskbar-Einträge DÜRFEN nicht permanent neu gerendert werden.
- Symbole und Vorschauen SOLLEN zwischengespeichert werden.
- Langsame Anwendungen DÜRFEN die Taskleiste nicht blockieren.
- Anwendungen DÜRFEN keine fremden Taskbar-Einträge unautorisiert verändern.
- Anwendungsaktionen MÜSSEN Nova.Trust berücksichtigen.
- Alle grundlegenden Funktionen MÜSSEN barrierefrei bedienbar sein.

## 13. Normative Anforderungen

- **TASKBAR-001:** NovaOS MUSS laufende und angeheftete Anwendungen verwalten können.
- **TASKBAR-002:** Anwendungen MÜSSEN unabhängig von ihren Prozessen und Fenstern identifizierbar sein.
- **TASKBAR-003:** Anwendungs- und Fensteränderungen MÜSSEN ereignisgesteuert erkannt werden.
- **TASKBAR-004:** Mehrere Fenster einer Anwendung MÜSSEN gruppierbar sein.
- **TASKBAR-005:** Angeheftete Anwendungen MÜSSEN dauerhaft gespeichert werden können.
- **TASKBAR-006:** Laufende und angeheftete Einträge derselben Anwendung SOLLEN zusammengeführt werden.
- **TASKBAR-007:** Fensteraktionen MÜSSEN über den Window Manager erfolgen.
- **TASKBAR-008:** Anwendungsstarts MÜSSEN über das zuständige Anwendungsmanagement erfolgen.
- **TASKBAR-009:** Workspace- und Monitorzuordnungen MÜSSEN berücksichtigt werden.
- **TASKBAR-010:** Fenster- und Anwendungsvorschauen SOLLEN unterstützt werden.
- **TASKBAR-011:** Ein Shell-Neustart DARF laufende Anwendungen nicht automatisch beenden.
- **TASKBAR-012:** Änderungen MÜSSEN Damage Tracking berücksichtigen.
- **TASKBAR-013:** Anwendungsaktionen MÜSSEN Nova.Trust berücksichtigen.
- **TASKBAR-014:** Die Bedienung MUSS barrierefrei möglich sein.
- **TASKBAR-015:** Nova Taskbar Application Management MUSS vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 14. Integration

- **Nova Taskbar Architecture:** Übergeordnete Taskleistenarchitektur.
- **Nova Desktop Shell Architecture:** Integration in die Desktop Shell.
- **Nova Shell Lifecycle:** Initialisierung und Wiederherstellung.
- **Nova Window Manager:** Fensterzustände und Fensteraktionen.
- **Nova Process Management:** Prozessinformationen.
- **Nova Application Management:** Anwendungsidentitäten und Anwendungsstarts.
- **Nova Desktop Workspace Management:** Workspace-Zuordnung.
- **Nova Compositor:** Fenstervorschauen und Damage Tracking.
- **Nova UI Framework:** Darstellung und Benutzerinteraktion.
- **Nova Capability System:** Anwendungsaktionen.
- **Nova.Trust:** Berechtigungen und Sicherheit.

## 15. Ergebnis

NovaOS erhält ein einheitliches Application Management für seine Taskleiste.

Laufende und angeheftete Anwendungen werden zuverlässig erkannt, gruppiert und dargestellt. Fenster bleiben unabhängig von der Shell verwaltet und können auch nach einem Shell-Neustart erneut eingebunden werden.

**Grundsatz:** Die Taskleiste verwaltet den Zugriff auf Anwendungen und ihre Fenster – nicht die Anwendungen selbst.