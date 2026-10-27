# NPSPEC-WINDOW-ARCH-0001 – Nova Window Manager Architecture

**Status:** Angenommen  
**Bereich:** Window Manager / Architektur  
**System:** NovaOS

## 1. Zweck

Nova Window Manager definiert die zentrale Architektur zur Verwaltung von Fenstern, deren Zuständen, Positionen, Größen, Fokus und Anordnung innerhalb der NovaOS-Desktopumgebung.

Ziel ist eine schnelle, zuverlässige und benutzerfreundliche Fensterverwaltung mit klarer Trennung zwischen Fensterlogik und grafischer Darstellung.

## 2. Architektur

Der Nova Window Manager ist ein eigenständiges Systemmodul und arbeitet mit folgenden Komponenten zusammen:

- **Nova Display Server:** Verwaltung der Display-Ausgabe und grafischen Ressourcen.
- **Nova Display Protocol:** Kommunikation zwischen Anwendungen und Display-System.
- **Nova Compositor:** Zusammensetzung und Darstellung der Fensteroberflächen.
- **Nova Input System:** Verarbeitung von Maus-, Tastatur- und Touch-Eingaben.
- **Nova UI:** Native Darstellung von Fensterinhalten und Bedienelementen.
- **Nova Capability Security:** Kontrolle von Fensteroperationen und Zugriffsrechten.
- **Nova IPC:** Kommunikation zwischen Systemkomponenten und Anwendungen.

Der Window Manager bestimmt den logischen Fensterzustand. Der Compositor übernimmt ausschließlich die grafische Zusammensetzung.

## 3. Kernfunktionen

- Erstellen, Registrieren und Entfernen von Fenstern.
- Verwaltung von Fensterpositionen und -größen.
- Minimieren, Maximieren und Wiederherstellen.
- Verwaltung von Fokus und aktivem Fenster.
- Fensteranordnung und Z-Order.
- Verschieben und Größenänderung.
- Fenstergruppen und Eltern-Kind-Beziehungen.
- Modale Dialoge und Popup-Fenster.
- Fensterandocken und Snap-Layouts.
- Verwaltung virtueller Desktops.
- Unterstützung mehrerer Monitore.
- Verwaltung von Fensterdekorationen.
- Persistenz von Fensterzuständen.
- Unterstützung unterschiedlicher Eingabemethoden.

## 4. Normative Anforderungen

- WINDOW-001: Der Window Manager MUSS als eigenständiges Systemmodul implementiert werden.
- WINDOW-002: Fensterzustand und grafische Darstellung MÜSSEN getrennt verwaltet werden.
- WINDOW-003: Jedes Fenster MUSS eine eindeutige Identität besitzen.
- WINDOW-004: Fenster MÜSSEN einer Anwendung oder einer autorisierten Systemkomponente zugeordnet sein.
- WINDOW-005: Fensteroperationen MÜSSEN über definierte Schnittstellen erfolgen.
- WINDOW-006: Zustandsänderungen MÜSSEN atomar und konsistent verarbeitet werden.
- WINDOW-007: Der Window Manager MUSS Fensterposition, Größe, Sichtbarkeit und Zustand verwalten.
- WINDOW-008: Fokus und Z-Order MÜSSEN zentral kontrolliert werden.
- WINDOW-009: Anwendungen DÜRFEN den Fokus anderer Anwendungen nicht eigenmächtig übernehmen.
- WINDOW-010: Geschützte Systemfenster MÜSSEN besondere Sicherheitsregeln besitzen.
- WINDOW-011: Fenster MÜSSEN mehreren Displays zugeordnet und zwischen ihnen verschoben werden können.
- WINDOW-012: Unterschiedliche DPI- und Skalierungsfaktoren MÜSSEN berücksichtigt werden.
- WINDOW-013: Fensteroperationen DÜRFEN den Compositor nicht blockieren.
- WINDOW-014: Der Window Manager MUSS unabhängig von dekorativen Effekten funktionieren.
- WINDOW-015: Fehlerhafte Anwendungen DÜRFEN die Fensterverwaltung nicht destabilisieren.
- WINDOW-016: Fensterzustände SOLLEN bei geeigneten Anwendungen wiederherstellbar sein.
- WINDOW-017: Der Window Manager MUSS ohne KI-Abhängigkeit funktionieren.
- WINDOW-018: Erweiterungen DÜRFEN grundlegende Sicherheits- und Fokusregeln nicht umgehen.

## 5. Fenstermodell

Jedes Fenster besitzt einen definierten logischen Zustand.

| Eigenschaft | Beschreibung |
|---|---|
| Window ID | Eindeutige Fensteridentität |
| Owner | Zugehörige Anwendung oder Systemkomponente |
| Parent | Optionales übergeordnetes Fenster |
| Surface ID | Zugeordnete grafische Oberfläche |
| Position | Fensterposition |
| Size | Fensterabmessungen |
| Min/Max Size | Zulässige Größenbegrenzungen |
| State | Normal, minimiert, maximiert oder Vollbild |
| Visibility | Sichtbarkeitszustand |
| Focus | Fokusstatus |
| Z-Order | Stapelreihenfolge |
| Workspace | Zugehöriger virtueller Desktop |
| Display | Zugeordnetes Display |
| Capabilities | Zulässige Fensteroperationen |

Die Fensteridentität bleibt während der gesamten Lebensdauer erhalten.

## 6. Fensterhierarchie

NovaOS unterscheidet folgende Fenstertypen:

### Normal Window
Reguläres Anwendungsfenster mit eigenständiger Position und Größe.

### Dialog Window
Fenster für Benutzerinteraktionen, optional modal.

### Popup Window
Temporäres Fenster für Menüs, Auswahllisten und ähnliche Inhalte.

### Tool Window
Untergeordnetes Werkzeugfenster einer Anwendung.

### Fullscreen Window
Fenster, das den vorgesehenen Displaybereich vollständig nutzt.

### System Window
Geschütztes Fenster für systemrelevante Funktionen.

Die Fenstertypen bestimmen Verhalten und zulässige Operationen, jedoch nicht automatisch die Sicherheitsberechtigungen.

## 7. Zustandsverwaltung

Der Window Manager unterstützt folgende grundlegende Fensterzustände:

- **Normal:** Frei positionierbares und skalierbares Fenster.
- **Minimized:** Fenster bleibt registriert, wird jedoch nicht regulär dargestellt.
- **Maximized:** Fenster nutzt den verfügbaren Arbeitsbereich.
- **Fullscreen:** Fenster nutzt den vorgesehenen Displaybereich ohne reguläre Fensterdekoration.
- **Hidden:** Fenster ist vorübergehend unsichtbar.
- **Closing:** Fenster befindet sich in der kontrollierten Beendigung.

Zustandswechsel erfolgen über definierte Transaktionen.

Die Wiederherstellungsgeometrie MUSS bei Minimierung und Maximierung erhalten bleiben.

## 8. Zusammenarbeit mit dem Compositor

Der Window Manager übermittelt dem Compositor:

- Fenstergeometrie.
- Sichtbarkeit.
- Z-Order.
- Surface-Zuordnung.
- Transformationen.
- Fensterdekorationen.
- Zustandsänderungen.
- Vorgaben für Fensterübergänge.

Der Compositor DARF den verbindlichen Fensterzustand nicht eigenständig verändern.

Grafische Effekte, Animationen und Buffer-Verarbeitung bleiben Aufgabe des Compositors.

## 9. Performance und Zuverlässigkeit

- Ereignisgesteuerte Verarbeitung von Fensteroperationen.
- Keine unnötigen Aktualisierungen unveränderter Fenster.
- Asynchrone Kommunikation mit Anwendungen.
- Begrenzung blockierender Operationen.
- Atomare Übernahme zusammengehöriger Zustandsänderungen.
- Effiziente Verwaltung großer Fensterzahlen.
- Wiederherstellung nach Anwendungs- oder Compositor-Fehlern.
- Unterstützung des frühen Desktopstarts.
- Verzögertes Laden optionaler Verwaltungsfunktionen.
- Diagnose von Fensterzuständen und fehlgeschlagenen Operationen.

## 10. Ergebnis

NovaOS erhält eine eigenständige, sichere und leistungsfähige Fensterverwaltungsarchitektur.

Der Window Manager kontrolliert Fensterzustände, Fokus, Positionierung und Anordnung, während der Compositor die grafische Darstellung übernimmt.

Die klare Trennung ermöglicht eine modulare Desktopumgebung mit flexiblen Fensterlayouts, mehreren Monitoren, virtuellen Desktops und zuverlässiger Zustandswiederherstellung.

**Grundsatz:** Der Window Manager entscheidet, wie Fenster organisiert werden. Der Compositor entscheidet, wie sie dargestellt werden.