# NPSPEC-WINDOW-MANAGEMENT-0003 – Nova Window Resizing

**Status:** Angenommen  
**Bereich:** Window Manager / Fenstergrößenänderung  
**System:** NovaOS

## 1. Zweck

Nova Window Resizing definiert die interaktive und programmatische Größenänderung von Fenstern innerhalb der NovaOS-Desktopumgebung.

Ziel ist eine präzise, flüssige und zuverlässige Fenstergrößenänderung unter Berücksichtigung von Größenbeschränkungen, DPI-Skalierung und Anwendungsreaktionszeiten.

## 2. Architektur

- Zentrale Verwaltung durch den Nova Window Manager.
- Integration in Nova Window State Model und Window Positioning.
- Unterstützung von Maus-, Touch- und Tastatureingaben.
- Trennung zwischen Fenstergeometrie und Surface-Buffer-Größe.
- Asynchrone Größenabstimmung mit Anwendungen.
- Integration in das native Nova UI Layoutsystem.
- Synchronisation mit Nova Compositor und Display Server.
- Unterstützung unterschiedlicher DPI-Skalierungen.
- Transaktionale Übernahme von Größenänderungen.

## 3. Kernfunktionen

- Größenänderung über Fensterkanten und Ecken.
- Programmatische Größenanforderungen.
- Unterstützung minimaler und maximaler Fenstergrößen.
- Optionale Beibehaltung eines Seitenverhältnisses.
- Gleichzeitige Anpassung von Position und Größe.
- Dynamische Anpassung des Fensterinhalts.
- Größenänderung über mehrere Displays hinweg.
- Unterstützung von Snap- und Andocklayouts.
- Wiederherstellung vorheriger Fenstergrößen.
- Flüssige Darstellung während interaktiver Größenänderungen.
- Behandlung verzögert reagierender Anwendungen.

## 4. Normative Anforderungen

- WINDOW-001: Der Window Manager MUSS Fenstergrößen zentral verwalten.
- WINDOW-002: Fenstergrößen MÜSSEN in logischen, DPI-unabhängigen Einheiten definiert werden.
- WINDOW-003: Minimale und maximale Fenstergrößen MÜSSEN berücksichtigt werden.
- WINDOW-004: Größenänderungen MÜSSEN durch Fensterberechtigungen und Zustandsregeln autorisiert werden.
- WINDOW-005: Nicht skalierbare Fenster DÜRFEN nicht ohne ausdrückliche Systemberechtigung frei skaliert werden.
- WINDOW-006: Interaktive Größenänderungen MÜSSEN ohne blockierendes Warten auf die Anwendung möglich sein.
- WINDOW-007: Fenstergeometrie und Surface-Buffer-Größe MÜSSEN unabhängig verwaltet werden.
- WINDOW-008: Neue Größen MÜSSEN der Anwendung über definierte Konfigurationsereignisse mitgeteilt werden.
- WINDOW-009: Asynchrone Größenbestätigungen MÜSSEN eindeutig einer Konfigurationsversion zugeordnet werden.
- WINDOW-010: Veraltete Größenbestätigungen DÜRFEN neuere Fensterzustände nicht überschreiben.
- WINDOW-011: Der Compositor MUSS während ausstehender Buffer-Aktualisierungen eine definierte Darstellung gewährleisten.
- WINDOW-012: Zusammengehörige Positions- und Größenänderungen MÜSSEN atomar übernommen werden können.
- WINDOW-013: Unterschiedliche DPI-Skalierungen MÜSSEN bei Größenänderungen berücksichtigt werden.
- WINDOW-014: Größenänderungen MÜSSEN die betroffenen Damage-Regionen aktualisieren.
- WINDOW-015: Maximierte und Vollbildfenster DÜRFEN nicht entgegen ihren Zustandsregeln frei skaliert werden.
- WINDOW-016: Die Wiederherstellungsgröße MUSS bei Maximierung und Minimierung erhalten bleiben.
- WINDOW-017: Nicht reagierende Anwendungen DÜRFEN die Größenänderung anderer Fenster nicht beeinträchtigen.
- WINDOW-018: Größenänderungen MÜSSEN unabhängig von dekorativen Animationen funktionieren.

## 5. Größenmodell

Jedes skalierbare Fenster besitzt folgende Eigenschaften:

| Eigenschaft | Beschreibung |
|---|---|
| Width | Aktuelle logische Breite |
| Height | Aktuelle logische Höhe |
| Min Size | Minimale Fenstergröße |
| Max Size | Maximale Fenstergröße |
| Aspect Ratio | Optionales Seitenverhältnis |
| Resize Mode | Zulässige Größenänderungen |
| Resize Anchor | Fixierter Bezugspunkt |
| Restore Size | Wiederherstellungsgröße |
| Configure Version | Version der Größenkonfiguration |

Größenbeschränkungen MÜSSEN vor der Übernahme einer neuen Fenstergeometrie geprüft werden.

## 6. Größenänderungsverfahren

### Interactive Resize

Der Benutzer verändert die Fenstergröße über Kanten, Ecken oder unterstützte Tastaturbefehle.

### Programmatic Resize

Eine Anwendung fordert eine neue Fenstergröße an. Der Window Manager entscheidet über deren Zulässigkeit.

### Layout Resize

Die Fenstergröße wird durch Snap-Layouts, Andockbereiche oder andere Layoutregeln bestimmt.

### State Resize

Die Fenstergröße ändert sich durch Maximieren, Wiederherstellen oder den Wechsel in den Vollbildmodus.

### Constrained Resize

Die Größenänderung berücksichtigt Mindestgröße, Maximalgröße und gegebenenfalls ein festgelegtes Seitenverhältnis.

## 7. Asynchrone Größenabstimmung

Der Window Manager DARF eine neue Fenstergeometrie bereits darstellen, während die Anwendung einen passenden Surface-Buffer vorbereitet.

Dabei gelten folgende Regeln:

- Jede Konfigurationsanforderung erhält eine eindeutige Version.
- Anwendungen können veraltete Zwischenkonfigurationen überspringen.
- Neue Buffer werden erst nach abgeschlossener Synchronisation verwendet.
- Vorhandene Buffer können vorübergehend skaliert oder innerhalb definierter Grenzen dargestellt werden.
- Bei verzögerter Reaktion bleibt die Fensterverwaltung bedienbar.
- Die endgültige Darstellung verwendet den zur bestätigten Konfiguration passenden Buffer.

Temporäre Skalierung DARF keine unzulässigen Speicherzugriffe oder undefinierten Bildinhalte verursachen.

## 8. Verarbeitungsablauf

1. **Request:** Größenänderung empfangen.
2. **Authorize:** Berechtigungen und Fensterzustand prüfen.
3. **Resolve:** Zielgröße und Resize-Anker bestimmen.
4. **Constrain:** Größenbeschränkungen anwenden.
5. **Configure:** Neue Geometrie mit einer Konfigurationsversion an die Anwendung übermitteln.
6. **Commit:** Gültige Fenstergeometrie übernehmen.
7. **Compose:** Vorhandene oder aktualisierte Surface darstellen.
8. **Synchronize:** Neuen Buffer nach Fertigstellung übernehmen.
9. **Complete:** Größenänderung abschließen und Zustand aktualisieren.

## 9. Performance und Zuverlässigkeit

- Flüssige Fenstergrößenänderung unabhängig von der Anwendungsreaktionszeit.
- Keine blockierenden IPC-Aufrufe im interaktiven Resize-Pfad.
- Wiederverwendung vorhandener Surface-Buffer.
- Zusammenfassung überholter Konfigurationsanforderungen.
- Partielle Aktualisierung betroffener Bildschirmbereiche.
- Vermeidung unnötiger Buffer-Neuzuweisungen.
- Unterstützung unterschiedlicher Display-Skalierungen.
- Kontrollierter Fallback bei verzögerter Buffer-Bereitstellung.
- Wiederherstellung gültiger Geometrien nach Fehlern.
- Diagnose von Resize-Latenzen und ausstehenden Konfigurationen.

## 10. Ergebnis

NovaOS erhält eine zuverlässige und reaktionsschnelle Fenstergrößenverwaltung.

Fenster können interaktiv, programmatisch oder durch Layoutregeln skaliert werden, ohne die Desktopbedienung zu blockieren.

Die Trennung zwischen logischer Fenstergeometrie und grafischen Buffern ermöglicht flüssige Größenänderungen auch bei verzögert reagierenden Anwendungen.

**Grundsatz:** Die Fenstergröße folgt unmittelbar der Benutzerinteraktion. Die Anwendung aktualisiert ihren Inhalt unabhängig davon.