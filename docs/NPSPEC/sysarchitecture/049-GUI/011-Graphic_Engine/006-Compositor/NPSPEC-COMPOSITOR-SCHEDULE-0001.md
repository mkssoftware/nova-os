# NPSPEC-COMPOSITOR-SCHEDULE-0001 – Nova Composition Scheduling

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Compositor

## 1. Zweck

Nova Composition Scheduling definiert die bedarfsgesteuerte Planung und Ausführung von Kompositionsaufträgen innerhalb des Nova Compositor.

Ziel ist die effiziente Verarbeitung grafischer Änderungen mit möglichst geringer Latenz, ohne unveränderte Bildinhalte permanent neu zusammenzusetzen.

Die Kompositionsplanung arbeitet ereignisgesteuert und wird mit Nova Frame Scheduling synchronisiert.

## 2. Architekturprinzipien

- **Event-Driven:** Komposition erfolgt aufgrund relevanter Ereignisse.
- **Damage-Driven:** Nur betroffene Bildbereiche werden berücksichtigt.
- **Incremental:** Unveränderte Kompositionsergebnisse bleiben erhalten.
- **Dependency-Aware:** Grafische Abhängigkeiten bestimmen die Ausführungsreihenfolge.
- **Deadline-Aware:** Kompositionsaufträge berücksichtigen Präsentationszeitpunkte.
- **Adaptive:** Arbeitsumfang und Ausführungsstrategie werden dynamisch optimiert.
- **Parallel:** Unabhängige Aufgaben können parallel verarbeitet werden.
- **Deterministic:** Identische Zustände ermöglichen reproduzierbare Ausführungspläne.

## 3. Composition Job Model

Ein Composition Job beschreibt einen geplanten Kompositionsauftrag.

Er enthält mindestens:

- **Job ID:** Eindeutige Auftragsidentifikation.
- **Output ID:** Betroffene Bildschirmausgabe.
- **State Version:** Zugehöriger Composition State.
- **Damage Regions:** Zu aktualisierende Bereiche.
- **Surface Dependencies:** Benötigte Surface-Inhalte.
- **Resource Dependencies:** Erforderliche GPU-Ressourcen.
- **Priority:** Ausführungspriorität.
- **Deadline:** Angestrebter Fertigstellungszeitpunkt.
- **Synchronization State:** Status abhängiger Operationen.

Ein Job MUSS auf einem konsistenten Kompositionszustand basieren.

## 4. Scheduling Triggers

NovaOS MUSS Kompositionsaufträge durch relevante Änderungen auslösen können.

Dazu gehören:

- Surface-Inhaltsänderungen
- Fensterbewegungen und Größenänderungen
- Änderungen der Z-Order
- Änderungen von Opacity oder Clipping
- Material- und Backdrop-Änderungen
- Animationen
- Änderungen der Bildschirmausgabe
- Explizite Aktualisierungsanforderungen

Ohne relevanten Damage SOLL keine neue Komposition geplant werden.

## 5. Job Coalescing

Mehrere Änderungen DÜRFEN zu einem gemeinsamen Composition Job zusammengefasst werden.

Dabei gilt:

1. Änderungen werden innerhalb eines geeigneten Zeitfensters gesammelt.
2. Damage Regions werden vereinigt.
3. Abhängigkeiten werden aktualisiert.
4. Veraltete, noch nicht ausgeführte Planungszustände können ersetzt werden.
5. Der Auftrag wird für die nächste geeignete Präsentation vorbereitet.

Das Zusammenfassen DARF keine erforderlichen Änderungen verlieren.

## 6. Dependency Scheduling

Kompositionsaufträge MÜSSEN ihre Ressourcen- und Ausführungsabhängigkeiten berücksichtigen.

Insbesondere:

- Surface-Buffer-Verfügbarkeit
- Material- und Effektberechnungen
- Render-Graph-Abhängigkeiten
- GPU-Synchronisation
- Composition Cache
- Zielbuffer-Verfügbarkeit

Ein Job DARF erst auf Ressourcen zugreifen, wenn deren erforderliche Synchronisationsbedingungen erfüllt sind.

Unabhängige Operationen DÜRFEN parallel ausgeführt werden.

## 7. Deadline-Aware Scheduling

NovaOS SOLL Kompositionsaufträge anhand des nächsten vorgesehenen Präsentationszeitpunkts planen.

Dabei werden berücksichtigt:

- Erwartete Kompositionsdauer
- Verfügbare CPU- und GPU-Kapazität
- Surface-Verfügbarkeit
- Frame-Pacing-Vorgaben
- VSync
- Eingabe- und Darstellungslatenz

Die Planung SOLL unnötig frühe Berechnungen vermeiden, ohne die rechtzeitige Fertigstellung zu gefährden.

## 8. Adaptive Scheduling

NovaOS SOLL die Kompositionsstrategie anhand des tatsächlichen Arbeitsaufwands auswählen.

Mögliche Strategien:

| Strategie | Verwendung |
|---|---|
| Immediate | Zeitkritische Änderungen |
| Deferred | Zusammenfassbare Änderungen |
| Incremental | Begrenzte Damage Regions |
| Full Composition | Große oder komplexe Änderungen |
| Parallel | Unabhängige Kompositionsaufgaben |
| Reuse | Gültige zwischengespeicherte Ergebnisse |

Die Auswahl MUSS grafische Korrektheit und Ressourcenabhängigkeiten erhalten.

## 9. Multi-Display Scheduling

Kompositionsaufträge MÜSSEN für mehrere Bildschirmausgaben unabhängig planbar sein.

Dabei gilt:

- Jede Ausgabe besitzt eigene Damage Regions.
- Unterschiedliche Bildwiederholraten werden berücksichtigt.
- Präsentationszeitpunkte können voneinander abweichen.
- Gemeinsame Rendering-Ergebnisse dürfen wiederverwendet werden.
- Langsame Ausgaben SOLLEN unabhängige Ausgaben nicht unnötig blockieren.

Gemeinsam verwendete Ressourcen MÜSSEN korrekt synchronisiert werden.

## 10. Frame Scheduling Integration

Nova Composition Scheduling und Nova Frame Scheduling besitzen getrennte Verantwortlichkeiten.

**Nova Composition Scheduling:**
- Bestimmt erforderliche Kompositionsarbeiten.
- Organisiert deren Ausführung.
- Berücksichtigt Abhängigkeiten und Ressourcen.
- Meldet die Fertigstellung eines Kompositionsergebnisses.

**Nova Frame Scheduling:**
- Bestimmt den Bedarf und den Zeitpunkt neuer Frames.
- Koordiniert Frame Pacing und VSync.
- Verwaltet Präsentationsfristen.
- Veranlasst die Übergabe an den Display Server.

Der Compositor DARF keinen unabhängigen permanenten Frame-Takt erzwingen.

## 11. Synchronization und Job Lifecycle

Ein Composition Job durchläuft folgende Zustände:

`Pending → Ready → Running → Completed → Presented`

Zusätzlich sind möglich:

- **Superseded:** Durch einen neueren Auftrag ersetzt.
- **Cancelled:** Vor Ausführung abgebrochen.
- **Failed:** Ausführung fehlgeschlagen.

Ein abgeschlossener Job DARF erst präsentiert werden, wenn alle erforderlichen GPU- und Buffer-Operationen abgeschlossen sind.

Ausstehender Damage MUSS bis zur erfolgreichen Verarbeitung erhalten bleiben.

## 12. Performance und Power Management

NovaOS SOLL unnötige CPU- und GPU-Aktivität vermeiden.

Dazu gehören:

- Zusammenfassung kleiner Änderungen
- Wiederverwendung gültiger Kompositionsergebnisse
- Vermeidung leerer Composition Jobs
- Begrenzung unnötiger Synchronisationswartezeiten
- Reduzierung von GPU-Aktivierungen im Leerlauf

Bei unverändertem Desktop SOLL der Compositor keine periodischen Kompositionsaufträge erzeugen.

## 13. Fehlerbehandlung

NovaOS MUSS fehlerhafte oder nicht ausführbare Composition Jobs erkennen können.

Dazu gehören:

- Ungültige Zustandsversionen
- Fehlende Surface-Buffer
- Nicht erfüllte Ressourcenabhängigkeiten
- Fehlgeschlagene GPU-Operationen
- Ungültige Zielbuffer

Fehlgeschlagene Jobs DÜRFEN erforderlichen Damage nicht verlieren.

Bei Bedarf MUSS eine vollständige Neukomposition möglich sein.

## 14. Verbindliche Anforderungen

1. NovaOS MUSS ereignisgesteuertes Composition Scheduling unterstützen.
2. Kompositionsaufträge MÜSSEN auf konsistenten Zustandsversionen basieren.
3. Ohne relevanten Damage SOLLEN keine unnötigen Jobs erzeugt werden.
4. Mehrere Änderungen SOLLEN zusammengefasst werden können.
5. Ressourcen- und Ausführungsabhängigkeiten MÜSSEN berücksichtigt werden.
6. Unabhängige Aufgaben DÜRFEN parallel ausgeführt werden.
7. Präsentationsfristen SOLLEN bei der Planung berücksichtigt werden.
8. Inkrementelle und vollständige Komposition MÜSSEN unterstützt werden.
9. Mehrere Bildschirmausgaben MÜSSEN unabhängig planbar sein.
10. Nova Frame Scheduling MUSS die Präsentationszeitpunkte koordinieren.
11. GPU- und Buffer-Synchronisation MUSS eingehalten werden.
12. Veraltete Aufträge DÜRFEN ersetzt werden, sofern keine Änderungen verloren gehen.
13. Fehlgeschlagene Jobs DÜRFEN ausstehenden Damage nicht löschen.
14. Ein sicherer Full-Composition-Fallback MUSS verfügbar sein.
15. Im deterministischen Modus MUSS die Auftragsplanung bei identischen Eingaben reproduzierbar sein.

## 15. Ergebnis

Nova Composition Scheduling ermöglicht eine bedarfsgesteuerte und ressourcenschonende Ausführung grafischer Kompositionsaufträge.

Durch Damage-Driven Scheduling, Job Coalescing, Abhängigkeitsanalyse und Deadline-Awareness werden unnötige Berechnungen und Darstellungslatenzen reduziert.

Damit arbeitet der Nova Compositor effizient mit Nova Frame Scheduling zusammen, ohne unveränderte Desktop-Inhalte permanent neu zusammensetzen zu müssen.