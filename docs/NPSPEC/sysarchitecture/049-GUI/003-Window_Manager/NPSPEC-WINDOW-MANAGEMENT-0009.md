# NPSPEC-WINDOW-MANAGEMENT-0009 – Nova Fullscreen Management

**Status:** Angenommen  
**Bereich:** Window Manager / Vollbildverwaltung  
**System:** NovaOS

## 1. Zweck

Nova Fullscreen Management definiert die Verwaltung von Fenstern im Vollbildmodus.

Ziel ist eine störungsarme, performante und sichere Vollbilddarstellung für Anwendungen, Spiele, Medienwiedergabe und andere bildschirmfüllende Inhalte.

## 2. Architektur

- Zentrale Vollbildverwaltung durch den Nova Window Manager.
- Integration in Nova Window State Model und Window Focus Management.
- Trennung zwischen Vollbildzustand, Displaymodus und Rendering.
- Unterstützung mehrerer Displays.
- Integration in Nova Display Output Management.
- Unterstützung von Direct Scanout durch den Compositor.
- Capability-basierte Kontrolle sicherheitsrelevanter Vollbildoperationen.
- Wiederherstellung des vorherigen Fensterzustands.

## 3. Vollbildmodell

NovaOS unterscheidet zwei Vollbildverfahren:

| Verfahren | Beschreibung |
|---|---|
| Composited Fullscreen | Vollbilddarstellung über den regulären Compositor |
| Direct Scanout | Direkte Ausgabe eines geeigneten Surface-Buffers ohne zusätzliche Komposition |

Beide Verfahren verwenden denselben logischen Fensterzustand `Fullscreen`.

Der Compositor entscheidet anhand technischer Voraussetzungen über das tatsächliche Ausgabeverfahren.

Ein Wechsel des Ausgabeverfahrens DARF den logischen Vollbildzustand nicht verändern.

## 4. Kernfunktionen

- Aktivieren und Verlassen des Vollbildmodus.
- Auswahl eines Ziel-Displays.
- Bildschirmfüllende Fensterdarstellung.
- Automatisches Ausblenden regulärer Desktopoberflächen.
- Unterstützung von Direct Scanout.
- Umschaltung zwischen Komposition und direkter Ausgabe.
- Unterstützung unterschiedlicher Auflösungen und Bildwiederholraten.
- Verwaltung von Fokus und Eingabe.
- Wiederherstellung vorheriger Fenstergeometrien.
- Behandlung von Displaywechseln und Hotplug.
- Sichere Einblendung privilegierter Systemoberflächen.

## 5. Normative Anforderungen

- WINDOW-001: Der Window Manager MUSS den verbindlichen Vollbildzustand verwalten.
- WINDOW-002: Vollbildanforderungen MÜSSEN autorisiert und validiert werden.
- WINDOW-003: Vollbild und Maximierung MÜSSEN getrennte Darstellungsmodi bleiben.
- WINDOW-004: Ein Vollbildfenster MUSS einem gültigen Ziel-Display zugeordnet sein.
- WINDOW-005: Die Vollbildgeometrie MUSS den vorgesehenen Displaybereich abdecken.
- WINDOW-006: Die vorherige Fenstergeometrie und der wiederherstellbare Darstellungsmodus MÜSSEN erhalten bleiben.
- WINDOW-007: Reguläre Desktopleisten DÜRFEN den Vollbildinhalt nicht dauerhaft verdecken.
- WINDOW-008: Geschützte Systemoberflächen MÜSSEN auch während der Vollbilddarstellung erreichbar bleiben.
- WINDOW-009: Anwendungen DÜRFEN sicherheitskritische Systemeingaben nicht abfangen oder blockieren.
- WINDOW-010: Direct Scanout DARF ausschließlich bei erfüllten technischen und sicherheitsrelevanten Voraussetzungen aktiviert werden.
- WINDOW-011: Der Wechsel zwischen Direct Scanout und regulärer Komposition MUSS ohne Änderung des logischen Fensterzustands möglich sein.
- WINDOW-012: Vollbildfenster MÜSSEN bei Fokusverlust entsprechend den geltenden Systemrichtlinien behandelt werden.
- WINDOW-013: Änderungen der Displaykonfiguration MÜSSEN eine Neubewertung der Vollbilddarstellung auslösen.
- WINDOW-014: Ein fehlgeschlagener Vollbildwechsel MUSS den vorherigen gültigen Zustand erhalten oder wiederherstellen.
- WINDOW-015: Anwendungen DÜRFEN Displayauflösung und Bildwiederholrate nur über autorisierte Schnittstellen ändern.
- WINDOW-016: Vollbildoperationen DÜRFEN den Window Manager nicht durch blockierende Anwendungsaufrufe anhalten.
- WINDOW-017: Der Benutzer MUSS einen regulären Vollbildmodus über definierte Systemfunktionen verlassen können.
- WINDOW-018: Die Vollbildverwaltung MUSS unabhängig von optionalen Animationen und KI funktionieren.

## 6. Aktivierung des Vollbildmodus

1. **Request:** Anwendung oder Benutzer fordert Vollbild an.
2. **Authorize:** Berechtigungen und Systemrichtlinien prüfen.
3. **Resolve:** Ziel-Display bestimmen.
4. **Save:** Vorherigen Darstellungsmodus und Wiederherstellungsgeometrie sichern.
5. **Configure:** Vollbildgeometrie berechnen und Anwendung informieren.
6. **Commit:** Fensterzustand atomar auf `Fullscreen` setzen.
7. **Compose:** Vollbilddarstellung aktivieren.
8. **Optimize:** Direct Scanout gegebenenfalls einschalten.

Der Wechsel in den Vollbildmodus MUSS auch ohne Direct Scanout möglich sein.

## 7. Direct Scanout

Direct Scanout ermöglicht die direkte Ausgabe eines Surface-Buffers über die Display-Hardware.

### Voraussetzungen

- Kompatibles Bufferformat.
- Geeignete Auflösung und Skalierung.
- Unterstützte Farb- und HDR-Eigenschaften.
- Abgeschlossene Buffer-Synchronisation.
- Keine zwingend zu komponierenden Überlagerungen.
- Erfüllte Sicherheits- und Ausgaberichtlinien.

### Verhalten

- Der Compositor entscheidet über die Aktivierung.
- Bei erforderlichen Systemoverlays wird reguläre Komposition aktiviert, sofern keine sichere Hardware-Overlay-Lösung verfügbar ist.
- Buffer-Synchronisation und Lebensdauer bleiben verbindlich.
- Bei ungeeigneten Voraussetzungen erfolgt automatisch ein Fallback auf Composited Fullscreen.

Direct Scanout ist eine Optimierung und keine Voraussetzung für Vollbilddarstellung.

## 8. Fokus und Systeminteraktionen

- Vollbildfenster unterliegen der regulären Fokusverwaltung.
- Ein Vollbildfenster DARF den Tastaturfokus nicht dauerhaft erzwingen.
- Systemweite Tastenkombinationen bleiben verfügbar.
- Sicherheitsdialoge und Authentifizierungsoberflächen besitzen Vorrang.
- Benachrichtigungen werden entsprechend den Benutzer- und Systemrichtlinien dargestellt oder zurückgestellt.
- Fokusverlust führt nicht automatisch zum Verlassen des Vollbildmodus.
- Anwendungen können über relevante Fokus- und Sichtbarkeitsänderungen informiert werden.

## 9. Wiederherstellung und Fehlerbehandlung

Beim Verlassen des Vollbildmodus:

1. Vollbilddarstellung deaktivieren.
2. Gegebenenfalls Direct Scanout beenden.
3. Vorherigen Darstellungsmodus ermitteln.
4. Wiederherstellungsgeometrie validieren.
5. Fensterzustand und Geometrie atomar aktualisieren.
6. Reguläre Desktopdarstellung wiederherstellen.
7. Compositor und Anwendung benachrichtigen.

Zusätzlich gelten folgende Regeln:

- Bei Displayverlust wird ein geeignetes Ersatzdisplay bestimmt oder der Vollbildmodus kontrolliert verlassen.
- Ungültige Wiederherstellungsgeometrien werden korrigiert.
- Nicht reagierende Anwendungen dürfen das Verlassen des Vollbildmodus nicht verhindern.
- Bei Compositor-Neustart bleibt der verbindliche Vollbildzustand erhalten.
- Fehlgeschlagene Displaymoduswechsel werden zurückgenommen.
- Ressourcen werden erst nach Abschluss ihrer letzten Verwendung freigegeben.

## 10. Ergebnis

NovaOS erhält eine sichere und performante Vollbildverwaltung.

Anwendungen können Displays vollständig nutzen, während der Compositor abhängig von den technischen Voraussetzungen zwischen regulärer Komposition und Direct Scanout wechseln kann.

Systemfunktionen, Sicherheitsoberflächen und Benutzerkontrolle bleiben jederzeit gewährleistet.

**Grundsatz:** Vollbild bedeutet maximale Nutzung des Displays, nicht maximale Kontrolle über das System.