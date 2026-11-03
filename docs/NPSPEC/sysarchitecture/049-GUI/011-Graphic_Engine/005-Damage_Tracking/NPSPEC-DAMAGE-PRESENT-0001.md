# NPSPEC-DAMAGE-PRESENT-0001 – Nova Partial Frame Presentation

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Damage Tracking

## 1. Zweck

Nova Partial Frame Presentation definiert die teilweise Aktualisierung dargestellter Frames anhand tatsächlich veränderter Bildbereiche.

Ziel ist die Reduzierung unnötiger Speichertransfers, GPU-Operationen und Bildschirmaktualisierungen.

Unveränderte Bildbereiche SOLLEN erhalten bleiben, ohne erneut gerendert oder kopiert werden zu müssen.

## 2. Architekturprinzipien

- **Damage-Driven:** Präsentationen berücksichtigen ausschließlich relevante Änderungen.
- **Region-Based:** Aktualisierungen werden räumlich begrenzt.
- **Incremental:** Unveränderte Bildinhalte bleiben erhalten.
- **Buffer-Aware:** Die Gültigkeit vorhandener Framebuffer-Inhalte wird berücksichtigt.
- **Synchronization-Safe:** Unvollständige Frames dürfen nicht präsentiert werden.
- **Backend-Aware:** Hardwareabhängige Präsentationsverfahren werden unterstützt.
- **Adaptive:** Partial und Full Presentation werden kostenbasiert ausgewählt.
- **Deterministic:** Identische Zustände ermöglichen reproduzierbare Präsentationsentscheidungen.

## 3. Presentation Model

Eine Partial Frame Presentation beschreibt die Aktualisierung einer Ausgabe anhand definierter Damage Regions.

Sie enthält mindestens:

- **Output ID:** Betroffene Bildschirmausgabe.
- **Frame ID:** Eindeutige Frame-Identifikation.
- **Buffer ID:** Zu präsentierender Framebuffer.
- **Damage Regions:** Aktualisierte Bildbereiche.
- **Content Version:** Version des dargestellten Inhalts.
- **Synchronization State:** Abschluss erforderlicher Rendering-Operationen.
- **Presentation Mode:** Partial oder Full.

Damage Regions MÜSSEN dem tatsächlich präsentierten Buffer zugeordnet sein.

## 4. Presentation Pipeline

Die Verarbeitung erfolgt grundsätzlich in folgenden Schritten:

1. Ausstehenden Damage übernehmen.
2. Betroffene Ausgabe bestimmen.
3. Gültigkeit des Zielbuffers prüfen.
4. Erforderliche Rendering-Operationen abschließen.
5. Präsentationsregionen optimieren.
6. Geeigneten Präsentationsmodus auswählen.
7. Frame synchronisiert präsentieren.
8. Erfolgreiche Präsentation bestätigen.

Die Präsentation MUSS auf einem konsistenten Frame-Zustand basieren.

## 5. Partial Presentation

NovaOS SOLL ausschließlich veränderte Bildbereiche zur Aktualisierung bereitstellen.

Dabei gilt:

- Unveränderte Pixel müssen gültig erhalten bleiben.
- Damage Regions müssen vollständig berücksichtigt werden.
- Erforderliche Speicher- und Cache-Synchronisation muss erfolgen.
- Die Ausgabe darf keine unvollständigen Bildinhalte anzeigen.

Partial Rendering und Partial Presentation MÜSSEN unabhängig voneinander unterstützt werden können.

Ein teilweise gerenderter Frame DARF vollständig präsentiert werden, wenn dies das Backend erfordert.

## 6. Buffer Age und Content Validity

Bei Double oder Triple Buffering können verschiedene Framebuffer unterschiedliche Bildstände enthalten.

NovaOS MUSS deshalb die Inhaltsgültigkeit des jeweiligen Buffers berücksichtigen.

Dazu gehören:

- Zuletzt vollständig gültige Frame-Version
- Seitdem entstandene Damage Regions
- Bereits aktualisierte Bereiche
- Gültigkeit wiederverwendeter Bildinhalte

Ein Buffer DARF nur teilweise aktualisiert werden, wenn sämtliche außerhalb der Aktualisierungsregion benötigten Inhalte nachweislich gültig sind.

Andernfalls MUSS NovaOS fehlende Bereiche ergänzen oder einen vollständigen Frame erzeugen.

## 7. Damage History

NovaOS SOLL eine begrenzte Historie präsentierter Damage Regions verwalten können.

Diese ermöglicht die Wiederherstellung älterer Buffer-Inhalte.

Dabei gilt:

- Damage Regions müssen Frame-Versionen zugeordnet sein.
- Mehrere zurückliegende Änderungen können vereinigt werden.
- Veraltete oder unvollständige Historien dürfen nicht verwendet werden.
- Nicht rekonstruierbare Buffer-Zustände erfordern Full Damage.

Die Damage History SOLL speicher- und kostenbewusst begrenzt werden.

## 8. Hardware Integration

NovaOS SOLL verfügbare Hardwarefunktionen für partielle Präsentation nutzen können.

Dazu gehören:

- Partial Buffer Updates
- Damage-Region-Hints
- Dirty Rectangles
- Hardware Overlays
- Direct Scanout
- Display-spezifische Teilaktualisierung

Damage-Region-Hints garantieren keine physisch partielle Bildschirmaktualisierung.

Die tatsächliche Präsentationsmethode wird durch den Nova Display Server und das jeweilige Backend bestimmt.

## 9. Synchronization

NovaOS MUSS sicherstellen, dass präsentierte Bildbereiche vollständig verarbeitet wurden.

Dabei werden berücksichtigt:

- GPU Fences
- Rendering Completion
- Buffer Ownership
- Frame Presentation Synchronization
- VSync und Frame Pacing

Ein Buffer DARF nicht während einer unzulässigen gleichzeitigen Verwendung verändert werden.

Die Freigabe oder Wiederverwendung MUSS die tatsächliche Nutzung durch GPU und Display berücksichtigen.

## 10. Adaptive Presentation

NovaOS SOLL zwischen Partial und Full Presentation anhand der erwarteten Gesamtkosten entscheiden.

Berücksichtigt werden:

- Anzahl der Damage Regions
- Anteil veränderter Pixel
- Speicherbandbreite
- Buffer-Rekonstruktionskosten
- Präsentations-Overhead
- Hardwareunterstützung

Eine vollständige Präsentation DARF gewählt werden, wenn sie effizienter oder technisch erforderlich ist.

## 11. Multi-Display Support

Damage und Präsentationszustände MÜSSEN für jede Ausgabe getrennt verwaltet werden können.

Dabei gilt:

- Jede Ausgabe besitzt eigene Damage Regions.
- Unterschiedliche Auflösungen und Skalierungen werden berücksichtigt.
- Ausgabeabhängige Buffer-Versionen bleiben getrennt.
- Änderungen einer Ausgabe dürfen unabhängige Ausgaben nicht unnötig aktualisieren.

Bei gespiegelten Ausgaben DÜRFEN gemeinsame Rendering-Ergebnisse wiederverwendet werden.

## 12. Frame Scheduling Integration

Nova Frame Scheduling bestimmt den Zeitpunkt der Präsentation.

Nova Partial Frame Presentation stellt hierfür die erforderlichen Damage- und Buffer-Informationen bereit.

Dabei gilt:

- Mehrere Änderungen dürfen zu einem Frame zusammengefasst werden.
- Ohne relevante Änderungen SOLL keine unnötige neue Präsentation erfolgen.
- Explizite Präsentationsanforderungen müssen berücksichtigt werden.
- Kontinuierliche Animationen erzeugen Frames entsprechend ihrem tatsächlichen Aktualisierungsbedarf.

Die Präsentationsfrequenz MUSS nicht an einen permanenten Rendering-Takt gebunden sein.

## 13. Fehlerbehandlung

NovaOS MUSS ungültige Präsentationszustände erkennen können.

Dazu gehören:

- Unvollständige Framebuffer-Inhalte
- Ungültige Damage History
- Fehlgeschlagene GPU-Synchronisation
- Veraltete Buffer-Versionen
- Nicht unterstützte Partial-Presentation-Verfahren

Bei unvollständiger Inhaltsgültigkeit MUSS ein vollständiger Frame erzeugt werden können.

Fehlgeschlagene Präsentationen DÜRFEN erforderlichen Damage nicht verlieren.

## 14. Verbindliche Anforderungen

1. NovaOS MUSS Damage Regions für die Frame-Präsentation bereitstellen.
2. Partial Presentation SOLL bei entsprechender Backendunterstützung verwendet werden.
3. Partial Rendering und Partial Presentation MÜSSEN getrennt behandelbar sein.
4. Die Inhaltsgültigkeit jedes Framebuffers MUSS berücksichtigt werden.
5. Buffer Age und Damage History SOLLEN unterstützt werden.
6. Unveränderte Bildbereiche SOLLEN ohne erneute Berechnung erhalten bleiben.
7. Präsentierte Frames MÜSSEN vollständig und konsistent sein.
8. GPU- und Display-Synchronisation MÜSSEN eingehalten werden.
9. Die Präsentation MUSS versionssicher erfolgen.
10. Partial und Full Presentation MÜSSEN unterstützt werden.
11. Die Auswahl des Präsentationsmodus SOLL kostenbasiert erfolgen.
12. Mehrere Bildschirmausgaben MÜSSEN unabhängig verwaltet werden können.
13. Fehlgeschlagene Präsentationen DÜRFEN ausstehenden Damage nicht löschen.
14. Bei ungültigem Buffer-Zustand MUSS ein Full-Frame-Fallback verfügbar sein.
15. Identische Eingaben MÜSSEN im deterministischen Modus reproduzierbare Präsentationsentscheidungen ermöglichen.

## 15. Ergebnis

Nova Partial Frame Presentation ermöglicht die effiziente Darstellung veränderter Bildbereiche ohne unnötige vollständige Frame-Aktualisierungen.

Durch Damage History, Buffer-Gültigkeitsprüfung und hardwareabhängige Präsentationsverfahren werden Speicherbandbreite und Verarbeitungsaufwand reduziert.

Damit vervollständigt das Subsystem die inkrementelle Grafikpipeline von NovaOS – von der Erkennung einer Änderung bis zur tatsächlichen Darstellung auf dem Bildschirm.