# NPSPEC-SHELL-TASKBAR-0004 – Nova Taskbar Preview

**Status:** Angenommen  
**Bereich:** Shell / Taskbar / Preview  
**System:** NovaOS

## 1. Zweck

Nova Taskbar Preview definiert die Darstellung und Interaktion mit Fenstervorschauen innerhalb der NovaOS-Taskleiste.

Fenstervorschauen ermöglichen es dem Benutzer, geöffnete Fenster schnell zu erkennen, auszuwählen und zu verwalten, ohne diese zunächst aktivieren zu müssen.

Ziel ist eine schnelle, übersichtliche und ressourcenschonende Vorschaufunktion, die sich nahtlos in die Taskleiste integriert.

## 2. Architektur

Das Subsystem besteht aus folgenden Komponenten:

| Komponente | Aufgabe |
|---|---|
| Preview Manager | Verwaltung der Fenstervorschauen |
| Preview Controller | Öffnen, Aktualisieren und Schließen |
| Preview Source Resolver | Ermittlung der zugehörigen Fenster |
| Preview Surface Provider | Bereitstellung der Vorschaubilder |
| Preview View | Darstellung und Benutzerinteraktion |
| Preview Cache | Zwischenspeicherung von Vorschauressourcen |

Die Vorschauverwaltung erfolgt innerhalb der Desktop Shell.

Die eigentlichen Fensterinhalte werden über autorisierte Schnittstellen des Nova Compositors bereitgestellt.

## 3. Vorschautypen

NovaOS unterstützt folgende Vorschautypen:

- Einzelne Fenstervorschau.
- Gruppierte Fenstervorschau.
- Statische Miniaturansicht.
- Dynamisch aktualisierte Vorschau.
- Vorschau mit Fensterinformationen.
- Geschützte oder eingeschränkte Vorschau.

Dabei gilt:

- Jedes darstellbare Fenster MUSS eine Vorschau oder eine geeignete Ersatzdarstellung besitzen können.
- Gruppierte Anwendungen MÜSSEN mehrere Vorschauen anzeigen können.
- Die Vorschauart MUSS anhand der verfügbaren Ressourcen und Berechtigungen bestimmt werden.
- Dynamische Vorschauen MÜSSEN deaktivierbar sein.

## 4. Vorschauinhalt

Eine Fenstervorschau kann enthalten:

- Miniaturansicht des Fensterinhalts.
- Anwendungssymbol.
- Fenstertitel.
- Anwendungsname.
- Fensterstatus.
- Schaltfläche zum Schließen.
- Optionale kontextabhängige Aktionen.

Dabei gilt:

- Vorschauen MÜSSEN eindeutig einem Fenster zugeordnet sein.
- Fenstertitel und Symbole MÜSSEN aus autorisierten Fensterinformationen stammen.
- Änderungen des Fenstertitels SOLLEN unmittelbar übernommen werden.
- Geschützte Inhalte DÜRFEN nicht unautorisiert dargestellt werden.
- Die Vorschau DARF den Zustand des ursprünglichen Fensters nicht verändern.

## 5. Vorschauerzeugung

Der Preview Surface Provider verwendet vorhandene Fensteroberflächen des Nova Compositors.

Dabei gilt:

- Vorschauen SOLLEN vorhandene Surface- oder Texturressourcen wiederverwenden.
- Unnötige Kopien von Fensterinhalten SOLLEN vermieden werden.
- Zero-Copy-Verfahren SOLLEN verwendet werden, sofern technisch möglich.
- Skalierung MUSS das Seitenverhältnis des Fensters berücksichtigen.
- Die Vorschauerzeugung DARF keine vollständige Neuberechnung des ursprünglichen Fensterinhalts erzwingen.
- Nicht verfügbare Fensterinhalte MÜSSEN durch eine geeignete Ersatzdarstellung ersetzt werden.

## 6. Aktualisierung und Damage Tracking

Fenstervorschauen MÜSSEN bedarfsgesteuert aktualisiert werden.

Dabei gilt:

- Änderungen SOLLEN anhand der Damage-Informationen des Compositors erkannt werden.
- Unveränderte Vorschauen DÜRFEN nicht permanent neu gerendert werden.
- Dynamische Vorschauen MÜSSEN ihre Aktualisierungsrate begrenzen können.
- Nicht sichtbare Vorschauen MÜSSEN ihre Aktualisierung aussetzen können.
- Statische Vorschauen SOLLEN bis zu ihrer Invalidierung wiederverwendet werden.
- Das Schließen eines Fensters MUSS zugehörige Vorschauressourcen freigeben.

Die Taskleiste DARF keine eigene kontinuierliche Erfassung sämtlicher Fensterinhalte implementieren.

## 7. Benutzerinteraktion

Fenstervorschauen können durch folgende Aktionen geöffnet werden:

- Hover über einem Taskbar-Element.
- Tastaturfokus.
- Klick auf eine Fenstergruppe.
- Alternative Eingabemethoden.

Unterstützte Aktionen:

- Fenster aktivieren.
- Fenster schließen.
- Zwischen Gruppenfenstern wechseln.
- Vorschau schließen.
- Optional weitere Fensteraktionen aufrufen.

Dabei gilt:

- Die Aktivierung MUSS über den Nova Window Manager erfolgen.
- Die Vorschau MUSS vollständig per Tastatur bedienbar sein.
- Aktionen MÜSSEN eindeutig dem jeweiligen Fenster zugeordnet sein.
- Das bloße Anzeigen einer Vorschau DARF keine Fensteraktivierung auslösen.

## 8. Darstellung und Positionierung

Die Darstellung erfolgt über das native Nova UI Framework.

Dabei gilt:

- Vorschauen MÜSSEN das aktive Theme berücksichtigen.
- Größe und Position MÜSSEN an den verfügbaren Bildschirmplatz angepasst werden.
- Vorschauen DÜRFEN nicht außerhalb des sichtbaren Bildschirmbereichs erscheinen.
- Gruppierte Vorschauen MÜSSEN übersichtlich angeordnet werden.
- Unterschiedliche DPI-Skalierungen MÜSSEN unterstützt werden.
- Animationen SOLLEN über den Compositor ausgeführt werden.
- High Contrast und Reduced Motion MÜSSEN berücksichtigt werden.

## 9. Mehrmonitor- und Workspace-Integration

Nova Taskbar Preview MUSS Mehrmonitorbetrieb und virtuelle Workspaces berücksichtigen.

Dabei gilt:

- Vorschauen MÜSSEN dem korrekten Fenster zugeordnet bleiben.
- Die Darstellung MUSS auf dem jeweiligen Monitor korrekt positioniert werden.
- Fenster anderer Workspaces DÜRFEN entsprechend der Taskbar-Konfiguration angezeigt werden.
- Workspace-Wechsel MÜSSEN berücksichtigt werden.
- Änderungen der Monitorkonfiguration DÜRFEN keine ungültigen Vorschaupositionen verursachen.

## 10. Performance und Ressourcenverwaltung

Die Vorschauverwaltung MUSS ressourcenschonend arbeiten.

Dabei gilt:

- Vorschauen SOLLEN erst bei Bedarf erzeugt werden.
- Vorschauressourcen SOLLEN zwischengespeichert werden.
- Nicht mehr benötigte Ressourcen MÜSSEN freigegeben werden.
- Die Anzahl gleichzeitig aktualisierter Vorschauen MUSS begrenzbar sein.
- Bei Ressourcenknappheit MUSS auf statische Vorschauen oder Symbole zurückgegriffen werden können.
- Die Vorschaufunktion DARF die Reaktionsfähigkeit der Taskleiste nicht beeinträchtigen.

## 11. Sicherheit und Datenschutz

Nova Taskbar Preview unterliegt Nova.Trust.

Dabei gilt:

- Fensterinhalte DÜRFEN ausschließlich über autorisierte Schnittstellen bereitgestellt werden.
- Geschützte Fenster MÜSSEN ihre Vorschau einschränken oder vollständig unterbinden können.
- Inhalte anderer Benutzersitzungen DÜRFEN nicht unautorisiert angezeigt werden.
- Vorschauressourcen DÜRFEN nicht unkontrolliert an Erweiterungen weitergegeben werden.
- Die Darstellung einer Vorschau DARF keine zusätzlichen Berechtigungen verleihen.
- Fehlerhafte Vorschauquellen DÜRFEN die Desktop Shell nicht beeinträchtigen.

## 12. Zustand und Fehlerbehandlung

Der Preview Manager MUSS ungültige oder nicht verfügbare Vorschauquellen kontrolliert behandeln.

Dabei gilt:

- Geschlossene Fenster MÜSSEN aus aktiven Vorschauen entfernt werden.
- Ungültige Surface-Referenzen DÜRFEN nicht weiterverwendet werden.
- Fehlerhafte Vorschauinhalte MÜSSEN durch eine Ersatzdarstellung ersetzt werden können.
- Ein Shell-Neustart MUSS vorhandene Fenster erneut für Vorschauen registrieren können.
- Temporäre Vorschauzustände MÜSSEN nicht dauerhaft gespeichert werden.

## 13. Normative Anforderungen

- **TASKBAR-PREVIEW-001:** NovaOS MUSS Fenstervorschauen innerhalb der Taskleiste unterstützen.
- **TASKBAR-PREVIEW-002:** Vorschauen MÜSSEN eindeutig ihren Fenstern zugeordnet sein.
- **TASKBAR-PREVIEW-003:** Gruppierte Fenster MÜSSEN gemeinsam dargestellt werden können.
- **TASKBAR-PREVIEW-004:** Vorschauinhalte MÜSSEN über autorisierte Compositor-Schnittstellen bereitgestellt werden.
- **TASKBAR-PREVIEW-005:** Vorhandene Surface-Ressourcen SOLLEN wiederverwendet werden.
- **TASKBAR-PREVIEW-006:** Vorschauen MÜSSEN Damage Tracking berücksichtigen.
- **TASKBAR-PREVIEW-007:** Nicht sichtbare Vorschauen DÜRFEN keine unnötigen Aktualisierungen verursachen.
- **TASKBAR-PREVIEW-008:** Fensteraktionen MÜSSEN über den Window Manager erfolgen.
- **TASKBAR-PREVIEW-009:** Vorschauen MÜSSEN vollständig per Tastatur bedienbar sein.
- **TASKBAR-PREVIEW-010:** Mehrmonitorbetrieb und Workspaces MÜSSEN berücksichtigt werden.
- **TASKBAR-PREVIEW-011:** Geschützte Fensterinhalte DÜRFEN nicht unautorisiert angezeigt werden.
- **TASKBAR-PREVIEW-012:** Bei Ressourcenknappheit MUSS eine vereinfachte Darstellung möglich sein.
- **TASKBAR-PREVIEW-013:** Fehlerhafte Vorschauquellen DÜRFEN die Shell nicht beeinträchtigen.
- **TASKBAR-PREVIEW-014:** Nova Taskbar Preview MUSS vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 14. Integration

- **Nova Taskbar Architecture:** Einbindung in die Taskleiste.
- **Nova Taskbar Application Management:** Fenster- und Anwendungszuordnung.
- **Nova Taskbar Grouping:** Gruppierte Fenstervorschauen.
- **Nova Window Manager:** Fensterinformationen und Fensteraktionen.
- **Nova Compositor:** Surface-Bereitstellung und Damage Tracking.
- **Nova Render Engine:** Skalierung und Bildverarbeitung.
- **Nova UI Framework:** Darstellung und Benutzerinteraktion.
- **Nova Desktop Workspace Management:** Workspace-Zuordnung.
- **Nova Display Server:** Monitorauflösung und Skalierung.
- **Nova.Trust:** Sicherheit und Datenschutz.

## 15. Ergebnis

NovaOS erhält eine integrierte, performante und sichere Fenstervorschau für seine Taskleiste.

Einzelne und gruppierte Fenster können schnell erkannt, ausgewählt und verwaltet werden, ohne ihre Ausführung oder ihren Zustand zu beeinflussen.

Durch die Wiederverwendung vorhandener Fensteroberflächen, Damage Tracking und bedarfsgesteuerte Aktualisierung bleibt die Vorschaufunktion ressourcenschonend.

**Grundsatz:** Eine Fenstervorschau zeigt nur so viel wie nötig, aktualisiert nur so oft wie erforderlich und verändert niemals unbeabsichtigt das ursprüngliche Fenster.