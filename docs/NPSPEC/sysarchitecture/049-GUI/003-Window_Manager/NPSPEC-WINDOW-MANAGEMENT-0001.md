# NPSPEC-WINDOW-MANAGEMENT-0001 – Nova Window Creation and Destruction

**Status:** Angenommen  
**Bereich:** Window Manager / Fenstererstellung und -entfernung  
**System:** NovaOS

## 1. Zweck

Nova Window Creation and Destruction definiert die kontrollierte Erstellung, Registrierung und Entfernung von Fenstern innerhalb des Nova Window Managers.

Ziel ist eine sichere, effiziente und transaktionale Verwaltung von Fensterobjekten und deren Ressourcen.

## 2. Architektur

- Zentrale Fenstererstellung und -entfernung durch den Nova Window Manager.
- Integration in Nova Window Lifecycle und Window State Model.
- Capability-basierte Autorisierung über Nova.Trust.
- Kommunikation über Nova IPC und Display Protocol.
- Trennung zwischen logischem Fensterobjekt und grafischer Surface.
- Asynchrone Initialisierung und kontrollierte Ressourcenfreigabe.
- Atomare Registrierung und Entfernung von Fensterobjekten.
- Automatische Bereinigung bei Anwendungsabstürzen.

## 3. Kernfunktionen

- Erstellung neuer Fensterobjekte.
- Vergabe eindeutiger Fensteridentitäten.
- Zuordnung von Besitzer und Sicherheitskontext.
- Initialisierung von Fenstereigenschaften.
- Registrierung von Fensterhierarchien.
- Zuordnung grafischer Surfaces.
- Verwaltung von Ressourcenlimits.
- Reguläre und erzwungene Fensterbeendigung.
- Entfernung aus Fokus-, Eingabe- und Z-Order-Verwaltung.
- Sichere Freigabe aller Fensterressourcen.

## 4. Normative Anforderungen

- WINDOW-001: Fenster MÜSSEN ausschließlich über autorisierte Schnittstellen erstellt werden.
- WINDOW-002: Jede Fenstererstellung MUSS eine eindeutige Fensteridentität erzeugen.
- WINDOW-003: Jedes Fenster MUSS einem gültigen Besitzer und Sicherheitskontext zugeordnet werden.
- WINDOW-004: Berechtigungen und Ressourcenlimits MÜSSEN vor der Erstellung geprüft werden.
- WINDOW-005: Fensterobjekte MÜSSEN vor ihrer Veröffentlichung vollständig initialisiert sein.
- WINDOW-006: Fehlerhafte Initialisierungen MÜSSEN vollständig zurückgenommen werden.
- WINDOW-007: Fenster und grafische Surfaces MÜSSEN unabhängig voneinander verwaltet werden können.
- WINDOW-008: Ein Fenster DARF erst nach gültiger Konfiguration zur Darstellung freigegeben werden.
- WINDOW-009: Fensterbeziehungen MÜSSEN bei der Erstellung validiert werden.
- WINDOW-010: Fenster DÜRFEN ausschließlich durch autorisierte Sicherheitskontexte entfernt werden.
- WINDOW-011: Reguläre Schließanforderungen MÜSSEN vom tatsächlichen Entfernen des Fensters unterschieden werden.
- WINDOW-012: Erzwungene Beendigungen MÜSSEN ohne Mitwirkung der Anwendung möglich sein.
- WINDOW-013: Fensterentfernungen MÜSSEN für andere Systemkomponenten konsistent sichtbar werden.
- WINDOW-014: Zerstörte Fenster DÜRFEN keine weiteren Operationen akzeptieren.
- WINDOW-015: Aktive Rendering- und Scanout-Ressourcen DÜRFEN nicht vorzeitig freigegeben werden.
- WINDOW-016: Fensterressourcen MÜSSEN nach ihrer letzten Verwendung vollständig freigegeben werden.
- WINDOW-017: Anwendungsabstürze MÜSSEN eine kontrollierte Bereinigung zugehöriger Fenster auslösen.
- WINDOW-018: Fenstererstellung und -entfernung DÜRFEN den Window Manager nicht dauerhaft blockieren.

## 5. Fenstererstellung

Die Erstellung erfolgt in folgenden Schritten:

1. **Request:** Anwendung übermittelt eine Fensteranforderung.
2. **Authorize:** Sicherheitskontext und Capabilities prüfen.
3. **Reserve:** Benötigte Ressourcen reservieren.
4. **Allocate:** Fensterobjekt und eindeutige Identität erzeugen.
5. **Initialize:** Fenstereigenschaften und Standardzustand festlegen.
6. **Register:** Fenster in der zentralen Fensterverwaltung registrieren.
7. **Configure:** Surface-Zuordnung und Fenstergeometrie abstimmen.
8. **Ready:** Fenster als darstellungsbereit kennzeichnen.
9. **Notify:** Anwendung über die erfolgreiche Erstellung informieren.

Die Registrierung MUSS entweder vollständig erfolgreich sein oder ohne verbleibende Teilzustände zurückgenommen werden.

## 6. Fensterentfernung

### Reguläre Entfernung

1. Schließanforderung empfangen.
2. Anwendung über die Anforderung informieren.
3. Zustimmung oder zulässige Ablehnung verarbeiten.
4. Fenster in den Zustand `Closing` versetzen.
5. Neue Fensteroperationen entsprechend dem Lebenszyklus einschränken.
6. Fenster aus Fokus-, Eingabe- und Z-Order-Verwaltung entfernen.
7. Grafische Darstellung kontrolliert beenden.
8. Surface- und Buffer-Referenzen freigeben, sobald ihre Nutzung abgeschlossen ist.
9. Fensterregistrierung endgültig entfernen.
10. Zustand `Destroyed` bestätigen.

### Erzwungene Entfernung

Bei Anwendungsabsturz, Sicherheitsverletzung oder administrativer Beendigung DARF NovaOS die Zustimmung der Anwendung umgehen.

Die Entfernung MUSS auch dann abgeschlossen werden können, wenn die Anwendung nicht mehr reagiert.

## 7. Ressourcenverwaltung

Jedes Fenster kann folgende Ressourcen referenzieren:

| Ressource | Verwaltung |
|---|---|
| Window Object | Window Manager |
| Window ID | Window Registry |
| Security Context | Nova.Trust |
| Surface | Display Server |
| Graphics Buffer | Graphics Buffer Management |
| Input Registration | Nova Input System |
| Focus and Z-Order | Window Manager |
| Parent/Child Relations | Window Hierarchy |
| Transition Resources | Compositor Effects |

Die Entfernung eines Fensters MUSS alle zugehörigen Referenzen kontrolliert auflösen.

Gemeinsam genutzte Ressourcen DÜRFEN erst freigegeben werden, wenn keine gültige Nutzung mehr besteht.

## 8. Fehlerbehandlung und Wiederherstellung

- Fehlgeschlagene Erstellungen hinterlassen keine registrierten Teilobjekte.
- Ressourcenlimits verhindern unkontrollierte Fenstererstellung.
- Ungültige Fensteridentitäten werden zurückgewiesen.
- Doppelte Entfernungsanforderungen werden sicher behandelt.
- Veraltete Referenzen können keine neuen Fenster beeinflussen.
- Verwaiste Fenster werden automatisch erkannt und bereinigt.
- Fensterhierarchien bleiben bei der Entfernung einzelner Fenster konsistent.
- Ein Compositor-Neustart erfordert keine Neuerstellung gültiger logischer Fenster.
- Fehlgeschlagene Ressourcenfreigaben werden diagnostiziert und kontrolliert nachbehandelt.

## 9. Performance

- Effiziente Vergabe und Verwaltung von Fensteridentitäten.
- Ressourcenreservierung nur bei tatsächlichem Bedarf.
- Asynchrone Initialisierung grafischer Komponenten.
- Keine unnötige Surface-Erstellung für unsichtbare Fenster.
- Minimierung blockierender IPC-Operationen.
- Verzögerte Freigabe noch verwendeter Grafikbuffer.
- Begrenzung gleichzeitig ausstehender Erstellungsanforderungen.
- Unterstützung des frühen Desktopstarts.
- Wiederverwendung geeigneter interner Verwaltungsstrukturen.

## 10. Ergebnis

NovaOS erhält eine sichere und transaktionale Fenstererstellung und -entfernung.

Fenster werden eindeutig registriert, autorisierten Anwendungen zugeordnet und kontrolliert mit grafischen Ressourcen verbunden.

Bei regulärer Beendigung, Anwendungsabsturz oder Systemfehler werden Fenster und ihre Ressourcen zuverlässig bereinigt.

**Grundsatz:** Ein Fenster wird entweder vollständig erstellt oder gar nicht. Seine Entfernung erfolgt kontrolliert und ohne zurückbleibende ungültige Ressourcen.