# NPSPEC-SHELL-TASKBAR-0001 – Nova Taskbar Architecture

**Status:** Angenommen  
**Bereich:** Shell / Taskbar / Architecture  
**System:** NovaOS

## 1. Zweck

Nova Taskbar Architecture definiert die Architektur und grundlegenden Funktionen der Taskleiste innerhalb der NovaOS Desktop Shell.

Die Taskleiste ermöglicht den schnellen Zugriff auf Anwendungen, Solutions, Capabilities, laufende Fenster und zentrale Systemfunktionen.

Ziel ist eine vertraute, moderne, anpassbare und ressourcenschonende Taskleiste, die sich an die Arbeitsweise des Benutzers anpasst, ohne unnötig Aufmerksamkeit zu beanspruchen.

## 2. Architektur

Die Taskleiste besteht aus folgenden Komponenten:

| Komponente | Aufgabe |
|---|---|
| Taskbar Manager | Verwaltung und Lebenszyklus |
| Taskbar View | Darstellung über das Nova UI Framework |
| Taskbar Layout Manager | Anordnung und Positionierung |
| Taskbar Item Manager | Verwaltung angehefteter und laufender Elemente |
| Taskbar Window Tracker | Erkennung und Zuordnung von Anwendungsfenstern |
| Taskbar Interaction Manager | Benutzerinteraktion und Kontextaktionen |
| Taskbar Extension Host | Integration zusätzlicher Funktionen |
| Taskbar State Manager | Speicherung und Wiederherstellung |

Die Komponenten MÜSSEN über definierte Schnittstellen zusammenarbeiten.

## 3. Grundstruktur

Die Taskleiste unterstützt folgende Bereiche:

- Start-Schaltfläche.
- Angeheftete Anwendungen und Solutions.
- Laufende Anwendungen und Fenster.
- Optionale Capability-Verknüpfungen.
- System Tray.
- Uhrzeit und Datum.
- Benachrichtigungsanzeige.
- Optionale Schnellaktionen.

Die Bereiche MÜSSEN unabhängig voneinander aktualisierbar sein.

Die Anordnung SOLL benutzerdefiniert angepasst werden können.

## 4. Taskbar-Elemente

Ein Taskbar-Element besitzt mindestens:

- Eindeutige Identität.
- Elementtyp.
- Symbol.
- Anzeigename.
- Zugeordnete Aktion oder Ressource.
- Aktuellen Darstellungszustand.

Unterstützte Elementtypen:

- Anwendungsverknüpfung.
- Laufende Anwendung.
- Fenstergruppe.
- Solution.
- Capability.
- Systemfunktion.
- Registrierte Shell-Erweiterung.

Darstellung und zugrunde liegende Funktion MÜSSEN voneinander getrennt bleiben.

## 5. Fensterintegration

Die Taskleiste verwendet den Nova Window Manager zur Verwaltung laufender Fenster.

Dabei gilt:

- Laufende Fenster MÜSSEN über definierte Window-Manager-Schnittstellen ermittelt werden.
- Fensteraktivierung, Minimierung und Wiederherstellung MÜSSEN über den Window Manager erfolgen.
- Mehrere Fenster einer Anwendung MÜSSEN gruppierbar sein.
- Fensterzustände MÜSSEN korrekt dargestellt werden.
- Änderungen MÜSSEN ereignisgesteuert übernommen werden.

Die Taskleiste DARF keine eigene Fensterverwaltung implementieren.

## 6. Benutzerinteraktion

Die Taskleiste unterstützt:

- Aktivieren laufender Anwendungen.
- Starten angehefteter Anwendungen.
- Wechseln zwischen Fenstern.
- Minimieren und Wiederherstellen.
- Anheften und Lösen von Elementen.
- Drag & Drop zur Anordnung.
- Kontextmenüs.
- Fenster- und Anwendungsvorschauen.

Dabei gilt:

- Aktionen MÜSSEN über definierte Commands und Capabilities ausgeführt werden.
- Drag & Drop MUSS das Nova Data Transfer System verwenden.
- Die Bedienung MUSS vollständig per Tastatur möglich sein.
- Interaktionen DÜRFEN die UI Event Loop nicht blockieren.

## 7. Positionierung und Layout

Die Taskleiste MUSS unterschiedliche Bildschirmkonfigurationen unterstützen.

Vorgesehene Eigenschaften:

- Position am oberen, unteren, linken oder rechten Bildschirmrand.
- Unterschiedliche Größen.
- Automatisches Ausblenden.
- Frei konfigurierbare Elementanordnung.
- Unterschiedliche Symbolgrößen.
- Mehrmonitorbetrieb.

Dabei gilt:

- Die Taskleiste MUSS ihre benötigte Bildschirmfläche beim Window Manager anmelden.
- Maximierte Fenster MÜSSEN reservierte Arbeitsbereiche berücksichtigen.
- Layoutänderungen MÜSSEN ohne Shell-Neustart möglich sein.
- Der Benutzer MUSS die Taskleistenposition verändern können.

## 8. Mehrmonitorbetrieb

Nova Taskbar unterstützt mehrere Monitore.

Dabei gilt:

- Jeder Monitor MUSS eine eigene Taskleiste besitzen können.
- Die Anzeige laufender Fenster MUSS konfigurierbar sein.
- Fenster können auf allen Taskleisten oder nur auf der Taskleiste ihres Monitors angezeigt werden.
- Unterschiedliche Bildschirmauflösungen und Skalierungen MÜSSEN berücksichtigt werden.
- Änderungen der Monitorkonfiguration MÜSSEN dynamisch verarbeitet werden.

## 9. Workspace-Integration

Die Taskleiste integriert Nova Desktop Workspace Management.

Dabei gilt:

- Fenster MÜSSEN ihrem Workspace zugeordnet werden können.
- Die Anzeige von Fenstern anderer Workspaces MUSS konfigurierbar sein.
- Workspace-Wechsel MÜSSEN unmittelbar berücksichtigt werden.
- Fenster DÜRFEN über definierte Aktionen zwischen Workspaces verschoben werden.
- Die Taskleiste DARF keine eigene Workspace-Verwaltung implementieren.

## 10. Erweiterbarkeit

Die Taskleiste unterstützt Erweiterungen über das Nova Shell Extension Model.

Erweiterungen können beispielsweise bereitstellen:

- Zusätzliche Statusanzeigen.
- Schnellaktionen.
- Capability-Verknüpfungen.
- Benutzerdefinierte Taskbar-Elemente.
- Kontextabhängige Funktionen.

Dabei gilt:

- Erweiterungen MÜSSEN deklarativ registrierbar sein.
- Erweiterungen DÜRFEN keine internen Taskbar-Strukturen direkt verändern.
- Fehlerhafte Erweiterungen MÜSSEN isoliert werden können.
- Der Benutzer MUSS optionale Erweiterungen deaktivieren können.

## 11. Darstellung und Verhalten

Die Taskleiste verwendet das native Nova UI Framework.

Dabei gilt:

- Styles und Themes MÜSSEN unterstützt werden.
- Transparenz, Blur und Acrylic-Effekte DÜRFEN verwendet werden.
- Selten benötigte Bedienelemente DÜRFEN visuell zurückgenommen werden.
- Hover- und Fokuszustände MÜSSEN eindeutig erkennbar sein.
- High Contrast und Reduced Motion MÜSSEN berücksichtigt werden.
- Die grundlegende Bedienbarkeit DARF nicht von visuellen Effekten abhängen.

## 12. Performance

Die Taskleiste MUSS bedarfsgesteuert arbeiten.

Dabei gilt:

- Änderungen an Fenstern und Anwendungen MÜSSEN ereignisgesteuert verarbeitet werden.
- Unveränderte Bereiche DÜRFEN nicht permanent neu gerendert werden.
- Damage Tracking MUSS verwendet werden.
- Symbole und Vorschauressourcen SOLLEN zwischengespeichert werden.
- Animationen SOLLEN nach Möglichkeit über den Compositor ausgeführt werden.
- Nicht sichtbare Vorschauen DÜRFEN keine unnötige Renderingaktivität verursachen.
- Optionale Erweiterungen SOLLEN bedarfsgesteuert geladen werden.

## 13. Zustand und Wiederherstellung

Der Taskbar State Manager verwaltet:

- Position und Größe.
- Angeheftete Elemente.
- Elementreihenfolge.
- Gruppierungseinstellungen.
- Automatisches Ausblenden.
- Monitorzuordnungen.
- Benutzerdefinierte Erweiterungen.

Dabei gilt:

- Einstellungen MÜSSEN benutzerbezogen gespeichert werden.
- Ein Shell-Neustart DARF die Taskbar-Konfiguration nicht verlieren.
- Laufende Fenster MÜSSEN nach einem Neustart erneut ermittelt werden.
- Beschädigte Einstellungen DÜRFEN den Start der Taskleiste nicht verhindern.
- Eine funktionsfähige Standardkonfiguration MUSS verfügbar sein.

## 14. Sicherheit und Barrierefreiheit

Die Taskleiste unterliegt Nova.Trust und der Nova Accessibility Architecture.

Dabei gilt:

- Taskbar-Aktionen MÜSSEN die Berechtigungen des Benutzers beachten.
- Erweiterungen DÜRFEN keine zusätzlichen Capabilities durch ihre Registrierung erhalten.
- Systemkritische Funktionen MÜSSEN geschützt bleiben.
- Alle grundlegenden Funktionen MÜSSEN per Tastatur erreichbar sein.
- Elemente und Zustände MÜSSEN für Screenreader verfügbar sein.
- Die Taskleiste MUSS vollständig offline funktionieren.

## 15. Normative Anforderungen

- **TASKBAR-001:** NovaOS MUSS eine native, modulare Taskleiste bereitstellen.
- **TASKBAR-002:** Die Taskleiste MUSS das Nova UI Framework verwenden.
- **TASKBAR-003:** Laufende Fenster MÜSSEN über den Window Manager ermittelt werden.
- **TASKBAR-004:** Anwendungen, Solutions und Capabilities MÜSSEN integrierbar sein.
- **TASKBAR-005:** Elemente MÜSSEN angeheftet und neu angeordnet werden können.
- **TASKBAR-006:** Mehrere Fenster einer Anwendung MÜSSEN gruppierbar sein.
- **TASKBAR-007:** Position und Größe der Taskleiste MÜSSEN anpassbar sein.
- **TASKBAR-008:** Mehrmonitorbetrieb MUSS unterstützt werden.
- **TASKBAR-009:** Workspace-Zuordnungen MÜSSEN berücksichtigt werden.
- **TASKBAR-010:** Erweiterungen MÜSSEN über definierte Schnittstellen integrierbar sein.
- **TASKBAR-011:** Die Taskleiste MUSS Damage Tracking verwenden.
- **TASKBAR-012:** Änderungen MÜSSEN ereignisgesteuert verarbeitet werden.
- **TASKBAR-013:** Einstellungen MÜSSEN gespeichert und wiederhergestellt werden können.
- **TASKBAR-014:** Ein Shell-Neustart DARF laufende Anwendungen nicht beenden.
- **TASKBAR-015:** Die Taskleiste MUSS Nova.Trust und Accessibility berücksichtigen.
- **TASKBAR-016:** Nova Taskbar MUSS vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 16. Integration

- **Nova Desktop Shell Architecture:** Übergeordnete Shell-Architektur.
- **Nova Shell Lifecycle:** Initialisierung und Beendigung.
- **Nova Shell Recovery:** Wiederherstellung nach Fehlern.
- **Nova Desktop Workspace Management:** Workspace-Zuordnung.
- **Nova Window Manager:** Fensterzustände und Fensteraktionen.
- **Nova Compositor:** Darstellung und Animationen.
- **Nova Display Server:** Mehrmonitorbetrieb.
- **Nova UI Framework:** Native Taskbar-Oberfläche.
- **Nova Shell Extension Model:** Erweiterbare Funktionen.
- **Nova Capability System:** Funktionsintegration.
- **Nova Input System:** Benutzerinteraktion.
- **Nova Data Transfer:** Drag & Drop.
- **Nova Notification System:** Benachrichtigungen.
- **Nova.Trust:** Sicherheit und Berechtigungen.

## 17. Ergebnis

NovaOS erhält eine modulare, anpassbare und ressourcenschonende Taskleiste.

Sie verbindet klassische Funktionen wie Anwendungsstart, Fensterwechsel und Systemstatus mit dem Capability-basierten Architekturmodell von NovaOS.

Durch ereignisgesteuerte Aktualisierung, Damage Tracking und unabhängige Komponenten bleibt die Taskleiste schnell und zuverlässig.

**Grundsatz:** Die Taskleiste stellt die wichtigsten Funktionen jederzeit erreichbar bereit, ohne die Aufmerksamkeit des Benutzers unnötig zu beanspruchen.