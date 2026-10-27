# NPSPEC-WINDOW-MANAGEMENT-0004 – Nova Window Minimization and Maximization

**Status:** Angenommen  
**Bereich:** Window Manager / Minimierung und Maximierung  
**System:** NovaOS

## 1. Zweck

Nova Window Minimization and Maximization definiert das Minimieren, Maximieren und Wiederherstellen von Fenstern.

Ziel ist eine schnelle, konsistente und ressourcenschonende Fensterverwaltung, die den Benutzerfluss nicht unterbricht.

## 2. Architektur

- Zentrale Zustandsverwaltung durch den Nova Window Manager.
- Integration in Nova Window State Model und Window Lifecycle.
- Trennung zwischen logischem Fensterzustand und grafischer Darstellung.
- Asynchrone Abstimmung mit Anwendungen und Compositor.
- Berücksichtigung von Arbeitsbereichen, Displays und DPI-Skalierung.
- Unterstützung optionaler Fensteranimationen.
- Erhaltung der ursprünglichen Fenstergeometrie.
- Ressourcenoptimierung minimierter Fenster.

## 3. Kernfunktionen

- Minimieren einzelner Fenster.
- Maximieren innerhalb des verfügbaren Arbeitsbereichs.
- Wiederherstellen vorheriger Fensterzustände.
- Minimieren aller regulären Fenster.
- Wiederherstellen minimierter Fenster.
- Maximieren durch Doppelklick oder unterstützte Eingabegesten.
- Unterstützung von Tastaturbefehlen.
- Berücksichtigung mehrerer Displays.
- Integration in Taskbar und Fensterübersicht.
- Optionale Animationen und Übergangseffekte.

## 4. Normative Anforderungen

- WINDOW-001: Minimierung und Maximierung MÜSSEN über den Window Manager erfolgen.
- WINDOW-002: Zustandsänderungen MÜSSEN atomar verarbeitet werden.
- WINDOW-003: Die ursprüngliche Fenstergeometrie MUSS vor einer Maximierung gespeichert werden.
- WINDOW-004: Maximierte Fenster MÜSSEN den zulässigen Arbeitsbereich des zugeordneten Displays verwenden.
- WINDOW-005: Reservierte Bereiche wie Taskbar und Systemleisten MÜSSEN berücksichtigt werden.
- WINDOW-006: Minimierte Fenster DÜRFEN keine regulären Eingabeereignisse empfangen.
- WINDOW-007: Bei Minimierung eines aktiven Fensters MUSS der Fokus gemäß den Aktivierungsregeln neu vergeben werden.
- WINDOW-008: Minimierte Fenster MÜSSEN ihre logische Identität und ihren Anwendungszustand behalten.
- WINDOW-009: Anwendungen MÜSSEN über relevante Zustandsänderungen informiert werden.
- WINDOW-010: Anwendungen DÜRFEN Minimierung und Maximierung nicht eigenmächtig erzwingen.
- WINDOW-011: Nicht maximierbare Fenster DÜRFEN nicht ohne entsprechende Systemberechtigung maximiert werden.
- WINDOW-012: Die Wiederherstellung MUSS den vorherigen wiederherstellbaren Darstellungsmodus berücksichtigen.
- WINDOW-013: Gespeicherte Fenstergeometrien MÜSSEN vor der Wiederherstellung validiert werden.
- WINDOW-014: Änderungen der Displaykonfiguration MÜSSEN bei maximierten Fenstern automatisch berücksichtigt werden.
- WINDOW-015: Minimierte Fenster SOLLEN keine unnötigen Rendering-Ressourcen beanspruchen.
- WINDOW-016: Animationen DÜRFEN Zustandsänderungen nicht blockieren.
- WINDOW-017: Nicht reagierende Anwendungen DÜRFEN die Minimierung oder Maximierung anderer Fenster nicht beeinträchtigen.
- WINDOW-018: Zustandsänderungen MÜSSEN auch ohne aktivierten Compositor-Effekt zuverlässig funktionieren.

## 5. Minimierung

Beim Minimieren eines Fensters:

1. Anforderung empfangen und autorisieren.
2. Aktuellen Darstellungsmodus sichern.
3. Fensterzustand auf `Minimized` setzen.
4. Fenster aus der regulären Darstellung entfernen.
5. Eingabefokus gegebenenfalls neu vergeben.
6. Compositor und Taskbar benachrichtigen.
7. Rendering-Aktivität nach Möglichkeit reduzieren.

Die Anwendung bleibt grundsätzlich aktiv. Eine Minimierung bedeutet keine Prozessunterbrechung.

### Ressourcenverhalten

- Nicht benötigte Renderzyklen SOLLEN ausgesetzt werden.
- Surface-Buffer DÜRFEN bei Bedarf freigegeben werden.
- Anwendungszustand und Fensteridentität bleiben erhalten.
- Hintergrundaufgaben laufen entsprechend den geltenden Systemrichtlinien weiter.
- Bei Wiederherstellung werden erforderliche Grafikressourcen erneut bereitgestellt.

## 6. Maximierung

Beim Maximieren eines Fensters:

1. Anforderung empfangen und autorisieren.
2. Wiederherstellungsgeometrie sichern, sofern erforderlich.
3. Ziel-Display und verfügbaren Arbeitsbereich bestimmen.
4. Maximierte Fenstergeometrie berechnen.
5. Zustand auf `Maximized` setzen.
6. Neue Geometrie an die Anwendung übermitteln.
7. Darstellung durch den Compositor aktualisieren.

Maximierung und Vollbildmodus bleiben getrennte Zustände.

Ein maximiertes Fenster berücksichtigt reguläre Desktopbereiche. Ein Vollbildfenster unterliegt den gesonderten Vollbildregeln.

## 7. Wiederherstellung

### Minimized → Normal / Maximized / Fullscreen

- Vorherigen Darstellungsmodus ermitteln.
- Fenster erneut zur Darstellung freigeben.
- Geometrie gegebenenfalls aktualisieren.
- Aktivierung entsprechend den Fokusregeln durchführen.

### Maximized → Normal

- Gespeicherte Wiederherstellungsgeometrie laden.
- Geometrie anhand der aktuellen Displaykonfiguration validieren.
- Fensterposition und Größe wiederherstellen.
- Zustand auf `Normal` setzen.

### Sonderfälle

- Ist das ursprüngliche Display nicht verfügbar, wird ein geeignetes Display gewählt.
- Liegt die gespeicherte Geometrie außerhalb erreichbarer Bereiche, wird sie korrigiert.
- Fehlt eine gültige Wiederherstellungsgeometrie, bestimmt der Window Manager eine geeignete Standardgeometrie.

## 8. Multi-Monitor-Verhalten

- Maximierung erfolgt auf dem zugeordneten Display.
- Unterschiedliche DPI-Skalierungen werden berücksichtigt.
- Arbeitsbereichsänderungen lösen eine Neuberechnung maximierter Fenster aus.
- Beim Entfernen eines Displays werden betroffene Fenster auf verfügbare Displays umgeordnet.
- Die ursprüngliche Wiederherstellungsgeometrie bleibt erhalten, solange sie gültig ist.
- Minimierte Fenster behalten ihre Display- und Workspace-Zuordnung, soweit diese weiterhin gültig ist.

## 9. Performance und Zuverlässigkeit

- Zustandsänderungen werden ohne blockierendes Warten auf Anwendungen verarbeitet.
- Minimierte Fenster verursachen möglichst keine unnötigen Compositor-Aktualisierungen.
- Bestehende Grafikbuffer werden nach Möglichkeit wiederverwendet.
- Überholte Konfigurationsanforderungen können zusammengefasst werden.
- Fensteranimationen sind optional und jederzeit deaktivierbar.
- Bei Compositor-Neustart werden die verbindlichen Fensterzustände erneut übertragen.
- Fehlgeschlagene Zustandsänderungen hinterlassen keine inkonsistenten Fenstergeometrien.
- Sicherheits- und Fokusregeln bleiben bei allen Übergängen verbindlich.

## 10. Ergebnis

NovaOS erhält eine einheitliche Verwaltung für minimierte, maximierte und wiederhergestellte Fenster.

Fenster behalten ihren logischen Zustand, während ihre Darstellung und Ressourcennutzung entsprechend angepasst werden.

Durch die Trennung von Fensterzustand, Anwendungsinhalt und Compositor bleiben Zustandswechsel schnell, zuverlässig und unabhängig von der Reaktionszeit einzelner Anwendungen.

**Grundsatz:** Minimieren reduziert die sichtbare Präsenz eines Fensters, Maximieren nutzt den verfügbaren Arbeitsbereich und Wiederherstellen bringt das Fenster in seinen vorherigen Zustand zurück.