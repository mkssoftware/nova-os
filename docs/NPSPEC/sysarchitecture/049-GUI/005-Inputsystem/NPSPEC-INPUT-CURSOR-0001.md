# NPSPEC-INPUT-CURSOR-0001 – Nova Cursor Management

**Status:** Angenommen  
**Bereich:** Input / Cursor Management  
**System:** NovaOS

## 1. Zweck

Nova Cursor Management definiert die zentrale Verwaltung, Darstellung und Steuerung von Mauszeigern innerhalb von NovaOS.

Ziel ist eine präzise, latenzarme und ressourcenschonende Cursorsteuerung, die unabhängig von Anwendungen und Eingabegeräten funktioniert.

Cursorbewegungen sollen möglichst ohne vollständige Neuberechnung der Desktopoberfläche dargestellt werden.

## 2. Architektur

Das Subsystem umfasst folgende Komponenten:

| Komponente | Aufgabe |
|---|---|
| Cursor Manager | Zentrale Verwaltung des Cursors |
| Cursor State Controller | Verwaltung von Position, Sichtbarkeit und Zustand |
| Cursor Theme Manager | Verwaltung von Cursorformen und Designs |
| Cursor Renderer | Darstellung über Hardware- oder Software-Rendering |
| Cursor Animation Controller | Steuerung animierter Cursor |
| Cursor Constraint Manager | Verwaltung zulässiger Bewegungsbereiche |

Die Cursorverwaltung gehört zur Input-Architektur.

Die Darstellung erfolgt über den Nova Compositor beziehungsweise die Hardware-Cursor-Funktionen des Grafikbackends.

Der Window Manager bestimmt den relevanten Fensterkontext.

## 3. Cursor-Modell

Ein Cursor besitzt mindestens:

- Cursor-ID.
- Position.
- Cursorform.
- Hotspot.
- Sichtbarkeitszustand.
- Skalierungsfaktor.
- Zugeordneten Input Seat.
- Optionalen Animationszustand.

Die Cursorposition wird in einem definierten logischen Koordinatensystem verwaltet.

Der Hotspot bezeichnet den Punkt, an dem die eigentliche Zeigerinteraktion stattfindet.

Cursorposition und sichtbare Cursorgeometrie MÜSSEN getrennt behandelt werden.

## 4. Cursorformen

NovaOS unterstützt mindestens folgende Cursorformen:

- Standardzeiger.
- Textauswahl.
- Hand beziehungsweise Link.
- Verschieben.
- Größenänderung horizontal.
- Größenänderung vertikal.
- Größenänderung diagonal.
- Warten.
- Hintergrundaktivität.
- Nicht erlaubt.
- Präzisionsauswahl.
- Benutzerdefinierter Cursor.

Anwendungen DÜRFEN innerhalb ihres zulässigen Eingabekontexts eine Cursorform anfordern.

Systemreservierte Cursorzustände besitzen Vorrang.

## 5. Cursorposition und Bewegung

Die Cursorposition wird durch die zentrale Input-Architektur aktualisiert.

Dabei werden berücksichtigt:

- Relative Mausbewegungen.
- Absolute Zeigereingaben.
- Display-Anordnung.
- Display-Skalierung.
- Display-Rotation.
- Zulässige Bewegungsbereiche.
- Aktiver Input Seat.

Die Cursorbewegung MUSS unabhängig von der Rendering-Frequenz verarbeitet werden.

Die Darstellung SOLL die aktuellste verfügbare Cursorposition verwenden.

Veraltete, noch nicht dargestellte Zwischenpositionen DÜRFEN übersprungen werden.

## 6. Hardware- und Software-Cursor

NovaOS unterstützt zwei Darstellungsarten.

### Hardware-Cursor

Bei geeigneter Hardware wird der Cursor über eine dedizierte Cursor-Ebene des Grafiksystems dargestellt.

Vorteile:

- Geringe Eingabelatenz.
- Keine vollständige Desktop-Neuberechnung.
- Geringerer Ressourcenverbrauch.
- Unabhängige Aktualisierung der Cursorposition.

### Software-Cursor

Ist kein geeigneter Hardware-Cursor verfügbar, übernimmt der Nova Compositor die Darstellung.

Dabei gilt:

- Die bisherige Cursorposition wird als Damage markiert.
- Die neue Cursorposition wird als Damage markiert.
- Ausschließlich betroffene Bereiche werden neu komponiert.
- Unveränderte Desktopbereiche bleiben erhalten.

Der Wechsel zwischen beiden Darstellungsarten MUSS ohne sichtbare Cursorduplikate erfolgen.

## 7. Cursor-Themes und Skalierung

NovaOS unterstützt austauschbare Cursor-Themes.

Ein Theme kann enthalten:

- Cursorbilder.
- Unterschiedliche Cursorgrößen.
- Hotspot-Definitionen.
- Animationsinformationen.
- Varianten für unterschiedliche Skalierungsfaktoren.

Cursor-Themes werden unabhängig von Anwendungen verwaltet.

Die Cursorgröße MUSS an Display-Skalierung und Benutzereinstellungen angepasst werden können.

Bei fehlenden oder fehlerhaften Cursorressourcen MUSS ein funktionsfähiger Standardcursor verfügbar bleiben.

## 8. Cursoranimationen

Animierte Cursor bestehen aus mehreren Bildern mit definierten Anzeigedauern.

Dabei gilt:

- Animationen werden über Nova Frame Pacing zeitlich gesteuert.
- Unveränderte Cursorbilder verursachen keine unnötigen Aktualisierungen.
- Hardwareanimationen DÜRFEN verwendet werden, sofern unterstützt.
- Softwareanimationen aktualisieren ausschließlich die betroffene Cursorregion.
- Animationen MÜSSEN bei Deaktivierung oder Unsichtbarkeit angehalten werden können.

Cursoranimationen DÜRFEN keine permanente Neuberechnung der gesamten Desktopoberfläche verursachen.

## 9. Sichtbarkeit und Cursor Constraints

Der Cursor Manager unterstützt:

- Cursor einblenden.
- Cursor ausblenden.
- Cursorposition abfragen.
- Cursorposition kontrolliert setzen.
- Cursor auf einen Bereich begrenzen.
- Cursorbegrenzungen aufheben.

Cursor Constraints können beispielsweise für Spiele, CAD-Anwendungen und spezielle Eingabekontexte verwendet werden.

Anwendungen DÜRFEN Cursorbegrenzungen nur innerhalb ihrer Berechtigungen aktivieren.

Sicherheitskritische Systeminteraktionen MÜSSEN bestehende Cursorbegrenzungen aufheben können.

## 10. Multi-Display und Multi-Device

Die Cursorverwaltung berücksichtigt mehrere Displays und Eingabegeräte.

Dabei gilt:

- Cursorpositionen werden displayübergreifend korrekt transformiert.
- Unterschiedliche Skalierungsfaktoren werden berücksichtigt.
- Mehrere Zeigegeräte können einen gemeinsamen Cursor steuern.
- Jeder Input Seat besitzt einen unabhängig verwaltbaren Cursorzustand.
- Mehrere gleichzeitig sichtbare Cursor DÜRFEN unterstützt werden.

Die Darstellung eines Cursors DARF keine unnötige Aktualisierung unabhängiger Displays verursachen.

## 11. Sicherheit und Fehlerbehandlung

Nova Cursor Management unterliegt Nova Input Security und Nova.Trust.

- Anwendungen DÜRFEN den Cursor nicht ohne Berechtigung global steuern.
- Cursorbegrenzungen und Positionsänderungen MÜSSEN kontrollierbar sein.
- Ungültige Cursorressourcen werden zurückgewiesen.
- Fehlerhafte Hotspots und Koordinaten werden sicher begrenzt.
- Bei Ausfall des Hardware-Cursors MUSS ein Software-Fallback möglich sein.
- Fehler einer Anwendung DÜRFEN den Systemcursor nicht dauerhaft blockieren.

## 12. Normative Anforderungen

- **INPUT-001:** NovaOS MUSS eine zentrale Cursorverwaltung bereitstellen.
- **INPUT-002:** Cursorposition, Cursorform und Sichtbarkeit MÜSSEN unabhängig verwaltet werden.
- **INPUT-003:** Cursorbewegungen MÜSSEN unabhängig von der Rendering-Frequenz verarbeitet werden.
- **INPUT-004:** Hardware-Cursor SOLLEN bei geeigneter Hardware bevorzugt verwendet werden.
- **INPUT-005:** Ein Software-Cursor MUSS als Fallback verfügbar sein.
- **INPUT-006:** Software-Cursor MÜSSEN Damage Tracking und Partial Redraw unterstützen.
- **INPUT-007:** Cursorbewegungen DÜRFEN keine unnötige vollständige Desktop-Neuberechnung erzwingen.
- **INPUT-008:** Cursor-Themes und Cursorgrößen MÜSSEN konfigurierbar sein.
- **INPUT-009:** Cursor-Hotspots MÜSSEN korrekt berücksichtigt werden.
- **INPUT-010:** Animierte Cursor MÜSSEN unterstützt werden.
- **INPUT-011:** Unterschiedliche Display-Skalierungen MÜSSEN berücksichtigt werden.
- **INPUT-012:** Jeder Input Seat MUSS einen eigenen Cursorzustand besitzen können.
- **INPUT-013:** Cursor Constraints MÜSSEN kontrolliert und widerrufbar sein.
- **INPUT-014:** Anwendungen DÜRFEN globale Cursorfunktionen nicht ohne entsprechende Berechtigung verwenden.
- **INPUT-015:** Bei fehlerhaften Cursorressourcen MUSS ein Standardcursor verfügbar bleiben.
- **INPUT-016:** Nova Cursor Management MUSS vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 13. Integration

- **Nova Input Architecture:** Verarbeitung von Zeigereingaben.
- **Nova Mouse Input:** Relative Mausbewegungen und Tasten.
- **Nova Multi-Device Input:** Gemeinsame und getrennte Cursorsteuerung.
- **Nova Input Security:** Berechtigungen und Cursor Constraints.
- **Nova Window Manager:** Cursorformen und Fensterkontexte.
- **Nova Display System:** Koordinaten und Display-Skalierung.
- **Nova Compositor:** Software-Cursor-Darstellung.
- **Nova Damage Tracking:** Partielle Aktualisierung bei Cursorbewegungen.
- **Nova Graphics Backend Abstraction:** Hardware-Cursor-Unterstützung.
- **Nova Frame Pacing:** Cursoranimationen und Präsentationssteuerung.
- **Nova UI:** Cursoranforderungen durch Steuerelemente.

## 14. Ergebnis

NovaOS erhält eine zentrale, hardwareunabhängige Cursorverwaltung.

Durch die bevorzugte Nutzung von Hardware-Cursorn und die Integration von Damage Tracking werden Cursorbewegungen mit geringer Latenz und minimalem Ressourcenverbrauch dargestellt.

Cursor-Themes, Animationen, Skalierung und mehrere Eingabekontexte ermöglichen eine flexible Anpassung an unterschiedliche Benutzer und Geräte.

**Grundsatz:** Der Cursor muss unmittelbar reagieren, ohne dass dafür die gesamte Desktopoberfläche neu gezeichnet werden muss.