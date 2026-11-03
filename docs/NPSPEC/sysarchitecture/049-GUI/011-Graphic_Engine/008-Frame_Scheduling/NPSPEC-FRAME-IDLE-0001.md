# NPSPEC-FRAME-IDLE-0001 – Nova Idle Frame Suppression

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Frame Scheduler

## 1. Zweck

Nova Idle Frame Suppression definiert die vollständige Unterdrückung unnötiger Frame-Erzeugung bei unveränderten grafischen Inhalten.

Ziel ist, Rendering, Komposition und Frame-Präsentation nur dann auszuführen, wenn tatsächlich neue Inhalte oder notwendige Ausgabeänderungen vorliegen.

Ein unveränderter Bildschirm MUSS ohne kontinuierliche Rendering-Schleife dargestellt bleiben können.

## 2. Architekturprinzipien

- **Event-Driven:** Neue Frames entstehen ausschließlich bei Aktualisierungsbedarf.
- **State-Aware:** Grafische Zustandsänderungen bestimmen die Aktivierung.
- **Damage-Aware:** Ohne relevanten Damage ist kein Redraw erforderlich.
- **Zero-Work Idle:** Keine unnötigen Rendering- oder Kompositionsaufträge.
- **Timer-Free Idle:** Keine periodischen Rendering-Timer ohne tatsächlichen Bedarf.
- **Wake-on-Change:** Reaktivierung durch relevante Ereignisse.
- **Power-Aware:** Unterstützung energiesparender CPU- und GPU-Zustände.
- **Deterministic:** Reproduzierbare Idle-Entscheidungen.

## 3. Idle State Model

NovaOS unterscheidet folgende Zustände:

| Zustand | Beschreibung |
|---|---|
| Active | Frame-Verarbeitung erforderlich |
| Pending | Aktualisierung vorhanden, Verarbeitung noch nicht gestartet |
| Waiting | Warten auf Deadline, Synchronisation oder Präsentation |
| Idle | Kein weiterer Frame erforderlich |
| Suspended | Grafische Aktualisierung vorübergehend ausgesetzt |

Die Zustände MÜSSEN unabhängig für jede Bildschirmausgabe verwaltet werden können.

Ein wartender Frame DARF nicht mit einem vollständig inaktiven Zustand verwechselt werden.

## 4. Idle Detection

Der Frame Scheduler MUSS vor der Erzeugung neuer Frames den tatsächlichen Aktualisierungsbedarf prüfen.

Dabei werden berücksichtigt:

- Ausstehender Damage
- Scene- und Surface-Änderungen
- Aktive Animationen
- Zeitabhängige Materialeffekte
- Ausstehende Frame-Anforderungen
- Erforderliche Display-Konfigurationsänderungen
- Explizite Aktualisierungsanforderungen

Liegt kein relevanter Aktualisierungsbedarf vor, MUSS die Frame-Erzeugung unterbleiben.

## 5. Frame Suppression

Im Idle-Zustand SOLL NovaOS sämtliche unnötigen Grafikoperationen vermeiden.

Dazu gehören:

1. Keine neuen Render Jobs.
2. Keine unnötigen Composition Jobs.
3. Keine künstlichen Surface Commits.
4. Keine periodischen GPU-Submissions.
5. Keine wiederholte Präsentation identischer Frames durch Software.
6. Keine CPU-Beschäftigung durch aktive Warteschleifen.

Die Display-Hardware DARF den zuletzt gültigen Scanout-Buffer weiterhin mit ihrer regulären Bildwiederholrate auslesen.

## 6. Wake-on-Change

NovaOS MUSS die Grafikpipeline bei relevanten Änderungen automatisch reaktivieren.

Mögliche Auslöser:

- Neue Surface-Inhalte
- Änderungen am Scene Graph
- Fensterbewegungen und Größenänderungen
- Änderungen der Sichtbarkeit
- Eingaben mit grafischen Auswirkungen
- Animation Deadlines
- Änderungen der Display-Konfiguration

Die Reaktivierung MUSS ereignisgesteuert erfolgen.

Ereignisse, die während des Übergangs in den Idle-Zustand eintreffen, DÜRFEN nicht verloren gehen.

## 7. Animation und Timer Integration

Aktive Animationen DÜRFEN zukünftige Aktualisierungszeitpunkte registrieren.

Dabei gilt:

- Nur tatsächlich erforderliche Animation Deadlines werden eingeplant.
- Abgeschlossene Animationen entfernen ihre Aktualisierungsanforderungen.
- Nicht sichtbare Animationen SOLLEN ausgesetzt oder reduziert werden.
- Zeitabhängige Effekte MÜSSEN ihren Aktualisierungsbedarf ausdrücklich anmelden.

Idle Frame Suppression DARF keine permanenten globalen Rendering-Timer benötigen.

## 8. Damage und Cache Management

Nova Idle Frame Suppression MUSS mit Nova Damage Tracking zusammenarbeiten.

Dabei gilt:

- Ausstehender Damage verhindert einen unzulässigen Idle-Übergang.
- Verdeckter Damage DARF zurückgestellt werden.
- Beim erneuten Sichtbarwerden MUSS erforderlicher Damage berücksichtigt werden.
- Gültige Render- und Composition Caches bleiben erhalten.
- Ungültige Buffer-Inhalte MÜSSEN vor ihrer Präsentation wiederhergestellt werden.

Idle DARF nicht zum Verlust erforderlicher grafischer Aktualisierungen führen.

## 9. Presentation Integration

Nova Idle Frame Suppression MUSS mit Nova Frame Presentation zusammenarbeiten.

Dabei gilt:

- Bereits eingereichte Präsentationen dürfen ordnungsgemäß abgeschlossen werden.
- Presentation Feedback muss weiterhin verarbeitet werden können.
- Aktive Scanout-Buffer bleiben gültig.
- Buffer dürfen nicht vorzeitig freigegeben werden.
- Ein unveränderter Ausgabezustand erfordert keinen neuen Presentation Request.

Idle Frame Suppression bedeutet nicht das Abschalten des Bildschirms.

## 10. Multi-Display Idle

Jede Bildschirmausgabe MUSS unabhängig in den Idle-Zustand wechseln können.

Dabei gilt:

- Unveränderte Ausgaben bleiben inaktiv.
- Änderungen einer Ausgabe aktivieren nicht automatisch andere Ausgaben.
- Gemeinsame Ressourcen werden korrekt synchronisiert.
- Unterschiedliche Bildwiederholraten beeinflussen nicht den grundsätzlichen Idle-Bedarf.

Eine Ausgabe DARF aktiv sein, während andere Ausgaben vollständig im Idle-Zustand verbleiben.

## 11. Power Management

NovaOS SOLL Idle-Zustände zur Reduzierung des Energieverbrauchs nutzen.

Mögliche Maßnahmen:

- Reduzierung der CPU-Aktivität.
- Vermeidung unnötiger GPU-Arbeit.
- Freigabe nicht benötigter temporärer Ressourcen.
- Unterstützung energiesparender GPU-Zustände.
- Vermeidung unnötiger Speichertransfers.

Die Aktivierung energiesparender Zustände DARF notwendige Reaktivierungen nicht dauerhaft verzögern.

## 12. Fehlerbehandlung

NovaOS MUSS fehlerhafte Idle-Übergänge sicher behandeln.

Dabei gilt:

- Verlorene Wake-up-Ereignisse MÜSSEN durch geeignete Synchronisation verhindert werden.
- Ausstehender Damage MUSS erhalten bleiben.
- Ungültige Zustandsversionen MÜSSEN erkannt werden.
- Fehlgeschlagene Präsentationen dürfen nicht als gültiger Abschluss behandelt werden.
- Bei unbekanntem Ausgabezustand MUSS ein vollständiger Redraw möglich sein.

## 13. Verbindliche Anforderungen

1. NovaOS MUSS unnötige Frame-Erzeugung im Idle-Zustand vollständig unterdrücken können.
2. Idle-Erkennung MUSS auf tatsächlichem Aktualisierungsbedarf beruhen.
3. Ohne Damage oder andere Aktualisierungsanforderungen DÜRFEN keine neuen Frames erzwungen werden.
4. Permanente Rendering-Schleifen DÜRFEN für den Idle-Betrieb nicht erforderlich sein.
5. Relevante Änderungen MÜSSEN die Grafikpipeline automatisch reaktivieren.
6. Wake-up-Ereignisse DÜRFEN nicht verloren gehen.
7. Ausstehender Damage MUSS erhalten bleiben.
8. Animation Deadlines MÜSSEN berücksichtigt werden.
9. Aktive Präsentationen und Buffer-Lebensdauern MÜSSEN korrekt abgeschlossen werden.
10. Mehrere Bildschirmausgaben MÜSSEN unabhängig inaktiv werden können.
11. Idle-Zustände SOLLEN energiesparende Hardwarezustände ermöglichen.
12. Der zuletzt gültige Frame MUSS ohne erneutes Rendering sichtbar bleiben können.
13. Bei inkonsistentem Zustand MUSS ein sicherer Redraw-Fallback verfügbar sein.
14. Im deterministischen Modus MÜSSEN Idle-Übergänge anhand definierter Ereignisse reproduzierbar sein.

## 14. Ergebnis

Nova Idle Frame Suppression ermöglicht einen vollständig ereignisgesteuerten Ruhezustand der NovaOS-Grafikpipeline.

Bei unveränderten Bildschirminhalten werden keine neuen Frames berechnet, komponiert oder unnötig präsentiert.

Erst relevante Änderungen aktivieren die erforderlichen Komponenten erneut.

Damit reduziert NovaOS CPU- und GPU-Auslastung, Speicherbandbreite und Energieverbrauch, ohne die Reaktionsfähigkeit oder grafische Korrektheit zu beeinträchtigen.