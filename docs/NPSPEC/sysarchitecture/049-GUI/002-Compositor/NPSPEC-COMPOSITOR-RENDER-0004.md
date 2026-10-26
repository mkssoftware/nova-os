# NPSPEC-COMPOSITOR-RENDER-0004 – Nova Partial Screen Updates

**Status:** Angenommen  
**Bereich:** Compositor / Partielle Bildaktualisierung  
**System:** NovaOS

## 1. Zweck

Nova Partial Screen Updates definiert die gezielte Aktualisierung einzelner Bildschirmbereiche, ohne den gesamten Frame neu berechnen zu müssen.

Ziel ist die Reduzierung von Rendering-Aufwand, Speicherbandbreite und Energieverbrauch bei gleichbleibender Bildqualität und geringer Darstellungslatenz.

## 2. Architektur

- Integration in Nova Compositor Pipeline und Frame Scheduling.
- Nutzung von Nova Surface Damage Tracking.
- Kombination mit Nova Occlusion Culling.
- Verwaltung von Update-Regionen pro Display.
- Unterstützung partieller GPU- und CPU-Komposition.
- Berücksichtigung von Buffer-Historien.
- Unterstützung hardwareabhängiger Partial-Present-Verfahren.
- Automatischer Fallback auf vollständige Bildaktualisierung.

## 3. Kernfunktionen

- Ermittlung veränderter Bildschirmbereiche.
- Zusammenführung von Damage-Regionen mehrerer Surfaces.
- Transformation in Display-Koordinaten.
- Berücksichtigung von Fensterbewegungen und Überlagerungen.
- Partielle Neuberechnung des Desktopbildes.
- Wiederverwendung unveränderter Bildbereiche.
- Verwaltung von Update-Historien bei Double und Triple Buffering.
- Unterstützung partieller Präsentation.
- Dynamische Entscheidung zwischen Partial und Full Update.
- Erkennung hardwarebedingter Einschränkungen.

## 4. Normative Anforderungen

- RENDER-001: Der Compositor MUSS partielle Bildaktualisierungen unterstützen.
- RENDER-002: Update-Regionen MÜSSEN aus gültigen Damage-Informationen abgeleitet werden.
- RENDER-003: Surface-Koordinaten MÜSSEN korrekt in Display-Koordinaten transformiert werden.
- RENDER-004: Fensterbewegungen MÜSSEN bisherige und neue Bildbereiche berücksichtigen.
- RENDER-005: Transparenz, Schatten und Blur MÜSSEN bei der Berechnung betroffener Regionen berücksichtigt werden.
- RENDER-006: Vollständig verdeckte Änderungen SOLLEN keine unnötige Bildschirmaktualisierung verursachen.
- RENDER-007: Unveränderte Bildbereiche SOLLEN wiederverwendet werden.
- RENDER-008: Wiederverwendete Framebuffer MÜSSEN einen gültigen Inhalt außerhalb der Update-Regionen besitzen.
- RENDER-009: Buffer-Wechsel MÜSSEN die jeweilige Aktualisierungshistorie berücksichtigen.
- RENDER-010: Partielle Aktualisierungen DÜRFEN keine veralteten oder undefinierten Bildbereiche erzeugen.
- RENDER-011: Update-Regionen MÜSSEN auf gültige Display-Grenzen begrenzt werden.
- RENDER-012: Überlappende Update-Regionen SOLLEN zusammengefasst werden.
- RENDER-013: Die Anzahl verwalteter Update-Regionen MUSS begrenzbar sein.
- RENDER-014: Bei hohem Aktualisierungsaufwand SOLL automatisch auf Full Update gewechselt werden.
- RENDER-015: Hardwareabhängige Partial-Present-Verfahren SOLLEN verwendet werden, sofern verfügbar und zuverlässig.
- RENDER-016: Fehlt Hardwareunterstützung für Partial Present, MUSS die korrekte vollständige Präsentation gewährleistet bleiben.
- RENDER-017: Mehrere Displays MÜSSEN unabhängig aktualisiert werden können.
- RENDER-018: Die Verarbeitung MUSS mit Nova Buffer Synchronization abgestimmt sein.
- RENDER-019: Bei ungültiger Damage- oder Buffer-Historie MUSS ein Full Update erfolgen.
- RENDER-020: Partielle Aktualisierungen DÜRFEN die Darstellungssicherheit geschützter Systemoberflächen nicht beeinträchtigen.

## 5. Aktualisierungsverfahren

### Partial Rendering
Nur veränderte Bildschirmbereiche werden neu komponiert.

### Region Merging
Überlappende oder benachbarte Update-Regionen werden zusammengefasst.

### Buffer Age Tracking
Ermittelt anhand der Buffer-Historie, welche Bereiche eines wiederverwendeten Framebuffers aktualisiert werden müssen.

### Preserved Framebuffer
Unveränderte Bildbereiche bleiben im Zielbuffer erhalten.

### Partial Present
Nur bestimmte Display-Regionen werden aktualisiert, sofern die Hardware dies unterstützt.

### Full Update
Der gesamte Frame wird neu berechnet, wenn partielle Aktualisierung nicht möglich oder unwirtschaftlich ist.

## 6. Aktualisierungsablauf

1. **Collect:** Damage-Regionen aller relevanten Surfaces erfassen.
2. **Transform:** Regionen in Display-Koordinaten übertragen.
3. **Expand:** Zusätzliche Bereiche durch Effekte und Geometrieänderungen berücksichtigen.
4. **Occlude:** Vollständig verdeckte Änderungen ausschließen.
5. **Merge:** Update-Regionen zusammenfassen.
6. **Validate:** Buffer-Historie und vorhandene Bildinhalte prüfen.
7. **Select:** Partial Update oder Full Update auswählen.
8. **Render:** Erforderliche Bildbereiche aktualisieren.
9. **Present:** Frame unter Berücksichtigung der Hardwarefähigkeiten ausgeben.
10. **Record:** Aktualisierungshistorie nach erfolgreicher Verarbeitung fortschreiben.

## 7. Adaptive Aktualisierung

Die Entscheidung zwischen Partial und Full Update berücksichtigt:

- Gesamtfläche der veränderten Regionen.
- Anzahl und Komplexität der Update-Rechtecke.
- Aufwand für Clipping und Regionsverwaltung.
- Kosten zusätzlicher Rendering-Operationen.
- Gültigkeit der Buffer-Historie.
- Eigenschaften des Rendering-Backends.
- Unterstützte Präsentationsverfahren.
- Aktuelle CPU- und GPU-Auslastung.

Die Auswahl SOLL anhand messbarer Kosten erfolgen und DARF nicht ausschließlich von der Größe der Update-Fläche abhängen.

## 8. Performance und Zuverlässigkeit

- Minimierung unnötiger GPU- und CPU-Arbeit.
- Reduzierung von Speicherbandbreite und Energieverbrauch.
- Wiederverwendung gültiger Framebuffer-Inhalte.
- Vermeidung unnötiger vollständiger Desktopkomposition.
- Effiziente Verwaltung von Update-Regionen.
- Unterstützung unterschiedlicher Display-Frequenzen.
- Keine unnötige Frame-Erzeugung bei unverändertem Desktop.
- Kontrollierter Full-Update-Fallback.
- Wiederherstellung nach Verlust der Buffer-Historie.
- Erfassung aktualisierter Flächen, Rendering-Zeiten und eingesparter Operationen.

## 9. Ergebnis

NovaOS erhält eine effiziente Architektur für partielle Bildschirmaktualisierungen.

Der Compositor berechnet bevorzugt nur tatsächlich veränderte Bildbereiche und verwendet unveränderte Inhalte weiter.

Durch die Kombination aus Damage Tracking, Occlusion Culling und Buffer-Historien werden Rendering-Aufwand, Speicherbandbreite und Energieverbrauch reduziert, ohne die korrekte Darstellung zu beeinträchtigen.