# NPSPEC-FRAME-DAMAGE-0001 – Nova Damage-Driven Frame Generation

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Frame Scheduler

## 1. Zweck

Nova Damage-Driven Frame Generation definiert die bedarfsgesteuerte Erzeugung grafischer Frames anhand tatsächlich veränderter Bildbereiche.

Ziel ist, neue Frames ausschließlich dann zu erzeugen, wenn sichtbare Änderungen oder notwendige Ausgabeaktualisierungen vorliegen.

Unveränderte Bildschirminhalte sollen ohne erneutes Rendering und ohne unnötige GPU-Aktivität erhalten bleiben.

## 2. Architekturprinzipien

- **Damage-Driven:** Räumliche Änderungen bestimmen den Frame-Bedarf.
- **Event-Driven:** Damage-Änderungen lösen Scheduling-Ereignisse aus.
- **Incremental:** Nur betroffene Bildbereiche werden aktualisiert.
- **State-Aware:** Änderungen werden anhand konsistenter Zustandsversionen verarbeitet.
- **Dependency-Aware:** Indirekt betroffene Bereiche werden berücksichtigt.
- **Coalescing:** Mehrere Damage-Ereignisse können zusammengefasst werden.
- **Idle-Safe:** Ohne Aktualisierungsbedarf erfolgt keine Frame-Erzeugung.
- **Fallback-Safe:** Vollständige Aktualisierung bleibt jederzeit möglich.

## 3. Damage-Driven Frame Model

Ein Damage-Driven Frame enthält mindestens:

| Eigenschaft | Beschreibung |
|---|---|
| Frame ID | Eindeutige Frame-Identifikation |
| Output ID | Betroffene Bildschirmausgabe |
| Scene Version | Verwendete Zustandsversion |
| Damage Regions | Zu aktualisierende Bereiche |
| Damage Version | Version des Damage-Zustands |
| Frame Priority | Aktualisierungspriorität |
| Target Time | Angestrebter Präsentationszeitpunkt |

Damage Regions MÜSSEN dem tatsächlich verwendeten grafischen Zustand entsprechen.

## 4. Frame Generation

Die Frame-Erzeugung erfolgt nach folgendem Ablauf:

1. Grafische Zustandsänderung erkennen.
2. Betroffene Damage Regions bestimmen.
3. Abhängige Bildbereiche berücksichtigen.
4. Damage der jeweiligen Ausgabe zuordnen.
5. Bereits ausstehende Frame-Anforderungen prüfen.
6. Damage gegebenenfalls zusammenfassen.
7. Erforderlichen Frame planen.
8. Rendering und Komposition ausführen.
9. Ergebnis zur Präsentation übergeben.

Ein Damage-Ereignis MUSS nicht zwangsläufig einen eigenständigen Frame erzeugen.

## 5. Damage Evaluation

NovaOS MUSS vor der Frame-Erzeugung prüfen, ob eine Ausgabe tatsächlich aktualisiert werden muss.

Dabei sind zu berücksichtigen:

- Sichtbarkeit der betroffenen Inhalte
- Verdeckte Bildbereiche
- Fensterposition und Z-Order
- Transformationen und Clipping
- Material- und Backdrop-Abhängigkeiten
- Bereits ausstehende Aktualisierungen

Vollständig verdeckte Änderungen DÜRFEN zurückgestellt werden, sofern ihre spätere Darstellung sichergestellt bleibt.

## 6. Damage Coalescing

Mehrere Damage-Ereignisse SOLLEN zu gemeinsamen Frame-Anforderungen zusammengefasst werden.

Dabei gilt:

- Damage Regions werden konservativ vereinigt.
- Überlappende Bereiche werden zusammengeführt.
- Neuere Zustandsversionen können ältere ersetzen.
- Bereits geplante Frames dürfen aktualisiert oder ersetzt werden.
- Keine erforderliche Änderung darf verloren gehen.

Das Zusammenfassen SOLL unnötige Zwischenframes vermeiden.

## 7. Incremental Frame Generation

NovaOS SOLL ausschließlich die durch Damage betroffenen Bereiche neu berechnen.

Dabei gilt:

- Unveränderte Render-Ergebnisse werden wiederverwendet.
- Gültige Composition Caches bleiben erhalten.
- Materialeffekte können zusätzliche Bereiche invalidieren.
- Buffer-Gültigkeit und Buffer Age werden berücksichtigt.
- Unvollständige oder ungültige Ausgabebuffer müssen vollständig hergestellt werden.

Damage-Driven Frame Generation bestimmt den Aktualisierungsbedarf.

Die konkrete partielle Berechnung erfolgt durch Render Engine und Compositor.

## 8. Frame Completion und Damage Lifetime

Damage DARF nicht bereits beim Einplanen eines Frames endgültig verworfen werden.

NovaOS MUSS sicherstellen, dass:

- Jeder Frame einen definierten Damage-Snapshot verwendet.
- Während des Renderings eintreffender Damage erhalten bleibt.
- Ersetzte oder abgebrochene Frames ihren noch erforderlichen Damage weitergeben.
- Fehlgeschlagene Präsentationen keine notwendigen Aktualisierungen verlieren.
- Ein gültiger Ausgabezustand hergestellt wird.

Die Freigabe von Damage MUSS anhand des tatsächlichen Verarbeitungs- und Ausgabezustands erfolgen.

## 9. Animationen und zeitabhängige Inhalte

Animationen MÜSSEN ihre erforderlichen Aktualisierungen über explizite Ereignisse oder Deadlines anmelden.

Dabei gilt:

- Nur tatsächlich veränderte Animationsbereiche erzeugen Damage.
- Nicht sichtbare Animationen SOLLEN keine unnötigen Frames auslösen.
- Zeitabhängige Materialeffekte werden berücksichtigt.
- Beendete Animationen dürfen keine weiteren Aktualisierungen erzwingen.

Ein permanenter globaler Rendering-Takt ist nicht erforderlich.

## 10. Multi-Display Support

Damage-Driven Frame Generation MUSS mehrere Bildschirmausgaben unabhängig unterstützen.

Dabei gilt:

- Damage wird den betroffenen Ausgaben zugeordnet.
- Jede Ausgabe besitzt einen eigenen ausstehenden Damage-Zustand.
- Unterschiedliche Bildwiederholraten werden berücksichtigt.
- Gemeinsame grafische Ressourcen dürfen wiederverwendet werden.
- Änderungen einer Ausgabe dürfen andere Ausgaben nicht unnötig aktualisieren.

## 11. Idle Frame Suppression

Liegt kein erforderlicher Damage und keine andere notwendige Ausgabeaktualisierung vor, MUSS NovaOS die Frame-Erzeugung aussetzen können.

Dabei gilt:

- Keine neuen Rendering-Aufträge.
- Keine unnötigen Kompositionsdurchläufe.
- Keine künstlichen GPU-Submissions.
- Keine periodischen Frame-Anforderungen ohne Bedarf.

Der zuletzt gültige Frame bleibt sichtbar.

Die physische Bildschirmausgabe kann unabhängig davon weiterhin ihren vorhandenen Buffer auslesen.

## 12. Fehlerbehandlung und Fallback

NovaOS MUSS bei unvollständigen Damage-Informationen eine konservative Aktualisierung ermöglichen.

Ein vollständiger Redraw ist insbesondere zulässig bei:

- Ungültiger Damage-Historie
- Verlust erforderlicher Buffer-Inhalte
- Nicht auflösbaren Abhängigkeiten
- Fehlerhaften Zustandsversionen
- Display-Rekonfiguration
- GPU- oder Compositor-Recovery

Grafische Korrektheit MUSS Vorrang vor der Minimierung des Rendering-Aufwands haben.

## 13. Verbindliche Anforderungen

1. NovaOS MUSS Frame-Bedarf anhand von Damage bestimmen können.
2. Damage-Änderungen MÜSSEN ereignisgesteuert verarbeitet werden.
3. Damage MUSS ausgabespezifisch verwaltet werden.
4. Jeder Frame MUSS einen konsistenten Damage-Snapshot verwenden.
5. Mehrere Damage-Ereignisse SOLLEN zusammengefasst werden.
6. Unveränderte Bereiche SOLLEN nicht erneut berechnet werden.
7. Material- und Backdrop-Abhängigkeiten MÜSSEN berücksichtigt werden.
8. Verdeckter Damage DARF nur bei gesicherter späterer Aktualisierung zurückgestellt werden.
9. Während des Renderings eintreffender Damage MUSS erhalten bleiben.
10. Abgebrochene oder ersetzte Frames DÜRFEN erforderlichen Damage nicht verlieren.
11. Ohne Aktualisierungsbedarf DÜRFEN keine unnötigen Frames erzeugt werden.
12. Mehrere Bildschirmausgaben MÜSSEN unabhängig unterstützt werden.
13. Bei ungültigem Damage-Zustand MUSS ein vollständiger Redraw möglich sein.
14. Im deterministischen Modus MÜSSEN Damage-Zuordnung und Frame-Entscheidungen reproduzierbar sein.

## 14. Ergebnis

Nova Damage-Driven Frame Generation verbindet die räumliche Änderungserkennung mit der zeitlichen Frame-Planung.

Neue Frames entstehen nur bei tatsächlichem Aktualisierungsbedarf. Unveränderte Inhalte werden wiederverwendet, während betroffene Bereiche gezielt neu berechnet werden.

Damit reduziert NovaOS unnötige Rendering- und Kompositionsarbeit und ermöglicht eine effiziente, vollständig bedarfsgesteuerte Grafikpipeline.