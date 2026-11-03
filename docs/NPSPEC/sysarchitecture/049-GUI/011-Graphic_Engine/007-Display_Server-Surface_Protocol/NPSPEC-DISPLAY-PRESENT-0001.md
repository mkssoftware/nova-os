# NPSPEC-DISPLAY-PRESENT-0001 – Nova Frame Presentation

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Display Server

## 1. Zweck

Nova Frame Presentation definiert die Übergabe fertiggestellter Frames an die physische Bildschirmausgabe.

Ziel ist eine effiziente, synchronisierte und möglichst latenzarme Darstellung grafischer Inhalte ohne unnötige Bildaktualisierungen.

Die Frame-Präsentation ist von Rendering, Komposition und Frame Scheduling getrennt.

## 2. Architekturprinzipien

- **Event-Driven:** Frames werden nur bei tatsächlichem Präsentationsbedarf übergeben.
- **Synchronization-Aware:** Unvollständige Buffer werden nicht präsentiert.
- **Atomic:** Zusammengehörige Ausgabeänderungen werden konsistent übernommen.
- **Damage-Aware:** Partielle Präsentation wird unterstützt.
- **Zero-Copy:** Direkte Buffer-Verwendung wird bevorzugt.
- **Adaptive:** Präsentationsverfahren richten sich nach Hardwarefähigkeiten.
- **Multi-Display:** Jede Ausgabe besitzt einen unabhängigen Präsentationszustand.

## 3. Presentation Model

Ein Presentation Request enthält mindestens:

| Eigenschaft | Beschreibung |
|---|---|
| Presentation ID | Eindeutige Identifikation |
| Output ID | Zielausgabe |
| Frame ID | Zugehöriger Frame |
| Buffer Reference | Darzustellender Buffer |
| Damage Regions | Aktualisierte Bildbereiche |
| Acquire Fence | Synchronisation des Frame-Inhalts |
| Target Time | Angestrebter Präsentationszeitpunkt |
| Presentation Mode | Gewähltes Präsentationsverfahren |

Jede Präsentation MUSS einem konsistenten Ausgabezustand zugeordnet sein.

## 4. Presentation Lifecycle

Eine Präsentation durchläuft folgende Zustände:

`Queued → Ready → Submitted → Presented → Retired`

Zusätzliche Zustände:

- **Rejected:** Ungültige Präsentationsanforderung.
- **Superseded:** Durch eine neuere Anforderung ersetzt.
- **Failed:** Präsentation fehlgeschlagen.

Ein Frame gilt erst dann als präsentiert, wenn die entsprechende Bestätigung des Display-Backends vorliegt.

Die Freigabe eines Scanout-Buffers erfolgt erst, wenn die Hardware ihn nicht mehr verwendet.

## 5. Presentation Modes

NovaOS SOLL folgende Präsentationsverfahren unterstützen:

| Verfahren | Beschreibung |
|---|---|
| FIFO / VSync | Synchronisierte Präsentation in geordneter Reihenfolge |
| Mailbox | Neuere Frames können noch ausstehende Frames ersetzen |
| Immediate | Unmittelbare Übergabe, sofern unterstützt |
| Direct Scanout | Direkte Ausgabe eines geeigneten Buffers |
| Partial Presentation | Aktualisierung unterstützter Teilbereiche |

Die tatsächliche Verfügbarkeit hängt vom Display-Backend ab.

Die Auswahl MUSS die Anforderungen an Bildqualität, Latenz und Synchronisation berücksichtigen.

## 6. Frame Scheduling Integration

Nova Frame Presentation MUSS mit Nova Frame Scheduling zusammenarbeiten.

Dabei gilt:

- Der Frame Scheduler bestimmt den angestrebten Präsentationszeitpunkt.
- Der Compositor stellt den erforderlichen Frame bereit.
- Der Display Server prüft die Präsentationsfähigkeit.
- Das Display-Backend führt die tatsächliche Ausgabe durch.

Der Display Server DARF keinen permanenten Rendering-Takt erzwingen.

Ohne neue Inhalte oder erforderliche Ausgabeänderungen SOLL der zuletzt präsentierte Frame erhalten bleiben.

## 7. Synchronization

Vor der Präsentation MÜSSEN alle erforderlichen Schreiboperationen abgeschlossen sein.

Dabei gilt:

1. Acquire Fences werden berücksichtigt.
2. Der Präsentationsbuffer muss vollständig gültig sein.
3. Erforderliche GPU- und Speicherbarrieren müssen erfüllt sein.
4. Die Hardware übernimmt den Buffer über einen kontrollierten Übergang.
5. Die Freigabe erfolgt nach Abschluss der tatsächlichen Verwendung.

Ein akzeptierter Presentation Request bedeutet nicht automatisch, dass der Frame bereits sichtbar ist.

## 8. Partial Presentation

NovaOS SOLL partielle Frame-Präsentation unterstützen, sofern das Display-Backend entsprechende Funktionen bereitstellt.

Dabei gilt:

- Damage Regions beschreiben aktualisierte Ausgabebereiche.
- Unveränderte Bereiche müssen weiterhin gültig sein.
- Buffer Age und vorherige Präsentationen müssen berücksichtigt werden.
- Nicht unterstützte Teilaktualisierungen werden durch vollständige Präsentation ersetzt.

Partial Rendering bedeutet nicht automatisch Partial Scanout.

Bei klassischem Scanout kann weiterhin der gesamte Framebuffer ausgelesen werden, obwohl nur einzelne Bereiche neu gerendert wurden.

## 9. Direct Scanout und Hardware Overlays

Nova Frame Presentation MUSS Direct Scanout und Hardware Overlays integrieren können.

Voraussetzungen sind:

- Kompatible Buffer-Formate
- Unterstützte Speicherlayouts
- Gültige Plane-Konfigurationen
- Korrekte Synchronisation
- Erfüllte Farb- und Skalierungsanforderungen

Nicht unterstützte Konfigurationen MÜSSEN auf reguläre Komposition zurückfallen können.

## 10. Multi-Display Presentation

Jede Bildschirmausgabe MUSS unabhängig präsentiert werden können.

Dabei gilt:

- Unterschiedliche Bildwiederholraten werden unterstützt.
- Jede Ausgabe besitzt eine eigene Presentation Queue.
- Präsentationszeitpunkte können unabhängig sein.
- Gemeinsame Buffer müssen korrekt synchronisiert werden.
- Fehler einer Ausgabe sollen andere Ausgaben nicht unnötig blockieren.

Eine bildschirmübergreifend atomare Präsentation wird nur garantiert, wenn das verwendete Backend dies ausdrücklich unterstützt.

## 11. Presentation Feedback

NovaOS MUSS Rückmeldungen über Präsentationsvorgänge bereitstellen.

Mindestens erforderlich sind:

- Presentation ID
- Frame ID
- Output ID
- Presentation Status
- Tatsächlicher Präsentationszeitpunkt, sofern ermittelbar
- Verwendetes Präsentationsverfahren
- Fehlerstatus

Presentation Feedback SOLL dem Frame Scheduler zur Verbesserung von Frame Pacing und Latenzsteuerung zur Verfügung stehen.

Geschätzte Zeitpunkte MÜSSEN von hardwareseitig bestätigten Zeitpunkten unterscheidbar sein.

## 12. Fehlerbehandlung und Fallback

NovaOS MUSS fehlgeschlagene Präsentationen kontrolliert behandeln.

Mögliche Fehler:

- Ungültiger Buffer
- Fehlgeschlagene Synchronisation
- Nicht unterstützter Präsentationsmodus
- Fehlgeschlagener Display-Commit
- Geräteverlust
- Deaktivierte Bildschirmausgabe

Ein fehlgeschlagener Request DARF nicht als erfolgreich präsentiert gemeldet werden.

Wenn möglich, SOLL der zuletzt gültige Ausgabezustand erhalten bleiben.

Bei Bedarf MUSS ein vollständiger Frame über ein alternatives Präsentationsverfahren ausgegeben werden können.

## 13. Verbindliche Anforderungen

1. NovaOS MUSS eine zentrale Frame-Präsentationsschnittstelle bereitstellen.
2. Rendering und Präsentation MÜSSEN getrennt bleiben.
3. Präsentationsanforderungen MÜSSEN eindeutig identifizierbar sein.
4. Buffer MÜSSEN vor ihrer Verwendung vollständig verfügbar sein.
5. Acquire- und Release-Synchronisation MÜSSEN berücksichtigt werden.
6. Präsentationszustände MÜSSEN eindeutig verwaltet werden.
7. VSync-synchronisierte Präsentation SOLL unterstützt werden.
8. Unterschiedliche Präsentationsmodi SOLLEN möglich sein.
9. Partial Presentation SOLL bei geeigneter Hardware unterstützt werden.
10. Direct Scanout und Hardware Overlays SOLLEN integrierbar sein.
11. Mehrere Bildschirmausgaben MÜSSEN unabhängig verwaltet werden können.
12. Presentation Feedback MUSS bereitgestellt werden.
13. Tatsächliche und geschätzte Präsentationszeitpunkte MÜSSEN unterscheidbar sein.
14. Aktive Scanout-Buffer DÜRFEN nicht vorzeitig freigegeben werden.
15. Fehlgeschlagene Präsentationen MÜSSEN sicher behandelt werden.
16. Ohne relevanten Aktualisierungsbedarf DÜRFEN keine unnötigen Frames erzwungen werden.
17. Im deterministischen Modus MÜSSEN Präsentationsentscheidungen bei identischen Eingaben reproduzierbar sein; tatsächliche Hardwarezeitpunkte sind davon ausgenommen.

## 14. Ergebnis

Nova Frame Presentation bildet die abschließende Stufe der NovaOS-Grafikpipeline.

Durch synchronisierte Buffer-Übergabe, adaptive Präsentationsverfahren, Partial Presentation und Direct Scanout werden Darstellungslatenz und unnötige Speichertransfers reduziert.

Damit ermöglicht NovaOS eine effiziente, ereignisgesteuerte Bildschirmausgabe, bei der unveränderte Inhalte ohne permanente Neuberechnung sichtbar bleiben.