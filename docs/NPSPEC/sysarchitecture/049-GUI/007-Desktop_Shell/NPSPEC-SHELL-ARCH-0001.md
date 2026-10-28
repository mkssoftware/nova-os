# NPSPEC-SHELL-ARCH-0001 – Nova Desktop Shell Architecture

**Status:** Angenommen  
**Bereich:** Shell / Architecture  
**System:** NovaOS

## 1. Zweck

Nova Desktop Shell Architecture definiert die Architektur der grafischen Desktopumgebung von NovaOS.

Die Desktop Shell stellt die zentrale Benutzerumgebung bereit und verbindet Fensterverwaltung, Navigation, Systemfunktionen, Benachrichtigungen und Anwendungen zu einer einheitlichen Oberfläche.

Ziel ist eine schnelle, ressourcenschonende und anpassbare Desktopumgebung, die sich vertraut bedienen lässt und den Benutzer möglichst wenig ablenkt.

Die Desktop Shell ist kein Bestandteil des Kernels und MUSS unabhängig von diesem gestartet, beendet und wiederhergestellt werden können.

## 2. Architektur

Die Desktop Shell besteht aus folgenden Komponenten:

| Komponente | Aufgabe |
|---|---|
| Shell Manager | Verwaltung und Lebenszyklus der Desktop Shell |
| Desktop Manager | Verwaltung des Desktops und seiner Elemente |
| Taskbar Manager | Taskleiste und laufende Anwendungen |
| Start Manager | Startmenü und Zugriff auf Systemfunktionen |
| Shell Navigation | Navigation zwischen Anwendungen und Systembereichen |
| Notification Center | Verwaltung und Darstellung von Benachrichtigungen |
| System Tray | Statusanzeigen und Hintergrundfunktionen |
| Shell Integration Service | Integration von Anwendungen und Capabilities |
| Shell State Manager | Speicherung und Wiederherstellung des Shell-Zustands |

Die Komponenten MÜSSEN über definierte Schnittstellen miteinander kommunizieren.

## 3. Systemgrenzen

NovaOS trennt folgende Verantwortlichkeiten:

| Subsystem | Verantwortung |
|---|---|
| Display Server | Bildschirmausgabe und Displayprotokoll |
| Compositor | Zusammensetzung und Darstellung von Oberflächen |
| Window Manager | Fensterzustände, Positionierung und Fensteroperationen |
| UI Framework | Controls, Layouts, Styles und Interaktionen |
| Desktop Shell | Benutzerumgebung und Systemnavigation |

Dabei gilt:

- Die Shell MUSS das native Nova UI Framework verwenden.
- Die Shell DARF keine eigene Fensterverwaltung implementieren.
- Die Shell DARF keine Aufgaben des Compositors übernehmen.
- Anwendungen MÜSSEN grundsätzlich unabhängig von der Desktop Shell ausführbar bleiben.
- Der Ausfall der Shell DARF nicht zum Ausfall des Kernels führen.

## 4. Shell-Lebenszyklus

Die Desktop Shell besitzt folgende Zustände:

1. Created
2. Initializing
3. Ready
4. Active
5. Suspended
6. Recovering
7. Stopped

Dabei gilt:

- Die Shell MUSS nach Bereitstellung der erforderlichen Grafik- und Eingabekomponenten startbar sein.
- Nicht benötigte Dienste SOLLEN nach dem Anzeigen des Desktops initialisiert werden.
- Ein Neustart der Shell MUSS ohne vollständigen Systemneustart möglich sein.
- Fehlerhafte Shell-Komponenten SOLLEN isoliert wiederhergestellt werden.
- Der Benutzerzustand SOLL nach einer Wiederherstellung erhalten bleiben.

## 5. Desktop

Der Desktop stellt den zentralen Arbeitsbereich bereit.

Unterstützt werden:

- Desktop-Hintergrund.
- Verknüpfungen und Elemente.
- Kontextmenüs.
- Drag & Drop.
- Auswahl und Anordnung.
- Mehrere Monitore.
- Benutzerdefinierte Layouts.

Dabei gilt:

- Desktop-Elemente MÜSSEN über das Nova UI Framework dargestellt werden.
- Änderungen SOLLEN inkrementell verarbeitet werden.
- Der Desktop DARF keine permanente Neuzeichnung verursachen.
- Benutzerdefinierte Anordnungen MÜSSEN gespeichert werden können.

## 6. Taskleiste und Startmenü

Die Taskleiste ermöglicht den Zugriff auf laufende und häufig verwendete Anwendungen.

Das Startmenü stellt Anwendungen, Capabilities, Solutions und Systemfunktionen bereit.

Dabei gilt:

- Laufende Anwendungen MÜSSEN über den Window Manager ermittelt werden.
- Fensterwechsel MÜSSEN über definierte Window-Management-Schnittstellen erfolgen.
- Angeheftete Elemente MÜSSEN benutzerbezogen gespeichert werden.
- Startmenü und Taskleiste MÜSSEN unabhängig voneinander aktualisiert werden können.
- Die Darstellung MUSS durch Themes und Layouts anpassbar sein.

## 7. Capability-Integration

Die Desktop Shell integriert das NovaOS-Capability-System.

Dabei gilt:

- Capabilities MÜSSEN über definierte Shell-Schnittstellen erreichbar sein.
- Kontextabhängige Funktionen DÜRFEN in Menüs, Ribbons und anderen UI-Bereichen angeboten werden.
- Die Shell DARF Capabilities nicht eigenmächtig Berechtigungen erteilen.
- Solutions MÜSSEN ihre vorgesehenen Shell-Integrationen deklarativ registrieren können.
- Fehlende Capabilities DÜRFEN die grundlegende Desktopfunktion nicht beeinträchtigen.

Die Shell stellt Funktionen bereit, ohne deren Implementierung selbst übernehmen zu müssen.

## 8. Benachrichtigungen und Systemstatus

Die Shell stellt zentrale Bereiche für Benachrichtigungen und Systeminformationen bereit.

Dabei gilt:

- Benachrichtigungen MÜSSEN über eine einheitliche Schnittstelle bereitgestellt werden.
- Anwendungen DÜRFEN nicht unkontrolliert eigene systemweite Benachrichtigungsflächen erzeugen.
- Nicht kritische Meldungen SOLLEN dezent dargestellt werden.
- Der Benutzer MUSS Benachrichtigungen verwalten und deaktivieren können.
- Sicherheitskritische Meldungen MÜSSEN eindeutig erkennbar bleiben.

## 9. Anpassbarkeit und Benutzerzustand

NovaOS ermöglicht die individuelle Anpassung der Desktopumgebung.

Dazu gehören:

- Desktop-Layouts.
- Taskleistenposition und -verhalten.
- Startmenü-Anordnung.
- Themes und Hintergrundbilder.
- Angeheftete Elemente.
- Benachrichtigungseinstellungen.
- Kontextabhängige Shell-Regeln.

Dabei gilt:

- Einstellungen MÜSSEN benutzerbezogen gespeichert werden.
- Layouts SOLLEN exportierbar und wiederherstellbar sein.
- Änderungen MÜSSEN rückgängig gemacht werden können.
- Automatische Anpassungen DÜRFEN die Benutzerkontrolle nicht einschränken.

## 10. Performance und Fast Boot

Die Desktop Shell MUSS auf eine möglichst kurze wahrgenommene Startzeit ausgelegt sein.

Dabei gilt:

- Nur unmittelbar benötigte Komponenten werden vor der ersten Desktopdarstellung geladen.
- Weitere Shell-Komponenten SOLLEN bedarfsgesteuert im Hintergrund initialisiert werden.
- Der Desktop MUSS bereits während der Initialisierung optionaler Dienste bedienbar bleiben.
- Rendering und Layoutberechnung MÜSSEN Damage Tracking berücksichtigen.
- Unveränderte Desktopbereiche DÜRFEN nicht permanent neu gezeichnet werden.
- Hintergrundaufgaben DÜRFEN die Reaktionsfähigkeit der Shell nicht dauerhaft beeinträchtigen.

## 11. Sicherheit und Wiederherstellung

Die Desktop Shell unterliegt Nova.Trust.

Dabei gilt:

- Shell-Komponenten MÜSSEN innerhalb definierter Sicherheitskontexte arbeiten.
- Anwendungen DÜRFEN keine privilegierten Shell-Funktionen ohne entsprechende Capabilities ausführen.
- Systemweite Shell-Erweiterungen MÜSSEN autorisiert sein.
- Fehlerhafte Erweiterungen SOLLEN isoliert deaktiviert werden können.
- Die Shell MUSS nach einem Absturz kontrolliert neu gestartet werden können.
- Ein Shell-Neustart DARF laufende Anwendungen nicht unnötig beenden.

## 12. Barrierefreiheit

Die Desktop Shell MUSS die Nova Accessibility Architecture vollständig unterstützen.

Dazu gehören:

- Tastaturbedienung.
- Screenreader-Unterstützung.
- High Contrast.
- Reduced Motion.
- Alternative Eingabemethoden.
- Skalierbare Benutzeroberfläche.

Alle grundlegenden Shell-Funktionen MÜSSEN ohne Maus erreichbar sein.

## 13. Normative Anforderungen

- **SHELL-001:** NovaOS MUSS eine eigenständige Desktop Shell bereitstellen.
- **SHELL-002:** Die Shell MUSS das native Nova UI Framework verwenden.
- **SHELL-003:** Window Manager, Compositor und Display Server MÜSSEN architektonisch getrennt bleiben.
- **SHELL-004:** Die Shell MUSS unabhängig vom Kernel neu gestartet werden können.
- **SHELL-005:** Desktop, Taskleiste und Startmenü MÜSSEN modular aufgebaut sein.
- **SHELL-006:** Anwendungen und Capabilities MÜSSEN über definierte Schnittstellen integrierbar sein.
- **SHELL-007:** Benachrichtigungen MÜSSEN zentral verwaltet werden.
- **SHELL-008:** Benutzerdefinierte Layouts und Einstellungen MÜSSEN gespeichert werden können.
- **SHELL-009:** Nicht benötigte Komponenten SOLLEN verzögert initialisiert werden.
- **SHELL-010:** Die Shell MUSS bedarfsgesteuertes Rendering und Damage Tracking verwenden.
- **SHELL-011:** Shell-Erweiterungen MÜSSEN Nova.Trust berücksichtigen.
- **SHELL-012:** Die Shell MUSS nach Fehlern kontrolliert wiederherstellbar sein.
- **SHELL-013:** Die grundlegende Desktopbedienung MUSS barrierefrei möglich sein.
- **SHELL-014:** Die Desktop Shell MUSS vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 14. Ergebnis

NovaOS erhält eine modulare, wiederherstellbare und ressourcenschonende Desktopumgebung.

Die Desktop Shell verbindet Anwendungen, Capabilities und Systemfunktionen zu einer einheitlichen Benutzeroberfläche, ohne Aufgaben des Window Managers, Compositors oder Display Servers zu übernehmen.

Durch bedarfsgesteuerte Initialisierung, native UI-Komponenten und konsequente Trennung der Subsysteme bleibt die Desktopumgebung schnell, anpassbar und zuverlässig.

**Grundsatz:** Die Desktop Shell organisiert die Arbeit des Benutzers – sie ist nicht das Betriebssystem selbst und darf dessen Stabilität nicht bestimmen.