# NPSPEC-WINDOW-ARCH-0002 – Nova Window Lifecycle

**Status:** Angenommen  
**Bereich:** Window Manager / Fensterlebenszyklus  
**System:** NovaOS

## 1. Zweck

Nova Window Lifecycle definiert den vollständigen Lebenszyklus eines Fensters von seiner Erstellung bis zur endgültigen Freigabe.

Ziel ist eine konsistente, sichere und ressourcenschonende Fensterverwaltung mit kontrollierten Zustandsübergängen und zuverlässiger Fehlerbehandlung.

## 2. Architektur

- Zentrale Lebenszyklusverwaltung durch den Nova Window Manager.
- Trennung zwischen Fensterexistenz, Sichtbarkeit und Darstellungszustand.
- Ereignisgesteuerte Zustandsübergänge.
- Atomare Übernahme zusammengehöriger Fensteränderungen.
- Asynchrone Kommunikation mit Anwendungen.
- Synchronisation mit Nova Display Server und Compositor.
- Capability-basierte Autorisierung von Fensteroperationen.
- Kontrollierte Ressourcenfreigabe bei Fenster- und Anwendungsbeendigung.

## 3. Lebenszykluszustände

| Zustand | Beschreibung |
|---|---|
| Created | Fensterobjekt wurde angelegt und registriert. |
| Configuring | Fensterparameter und Surface-Zuordnung werden eingerichtet. |
| Ready | Fenster ist vollständig konfiguriert und darstellungsbereit. |
| Visible | Fenster ist zur Darstellung freigegeben. |
| Hidden | Fenster existiert, ist jedoch nicht sichtbar. |
| Closing | Die kontrollierte Beendigung wurde eingeleitet. |
| Destroying | Fensterbeziehungen und Ressourcen werden aufgelöst. |
| Destroyed | Fenster wurde endgültig entfernt. |

Minimiert, maximiert und Vollbild sind eigenständige Darstellungszustände und keine Lebenszykluszustände.

## 4. Kernfunktionen

- Erstellung und Registrierung neuer Fenster.
- Initialisierung von Fensterparametern.
- Zuordnung von Anwendungen und Surfaces.
- Aktivierung und Deaktivierung der Sichtbarkeit.
- Verwaltung von Fensterzustandsänderungen.
- Verarbeitung von Schließanforderungen.
- Unterstützung abbrechbarer Schließvorgänge.
- Automatische Bereinigung bei Anwendungsabstürzen.
- Synchronisierte Freigabe grafischer Ressourcen.
- Wiederherstellung geeigneter Fensterzustände.

## 5. Normative Anforderungen

- WINDOW-001: Jedes Fenster MUSS einen eindeutig definierten Lebenszyklus besitzen.
- WINDOW-002: Fenster MÜSSEN vor ihrer Verwendung registriert werden.
- WINDOW-003: Die Fensteridentität MUSS während des gesamten Lebenszyklus eindeutig bleiben.
- WINDOW-004: Lebenszyklusübergänge MÜSSEN auf gültige Zustandswechsel beschränkt sein.
- WINDOW-005: Ungültige Zustandswechsel MÜSSEN kontrolliert zurückgewiesen werden.
- WINDOW-006: Fenster DÜRFEN erst nach erfolgreicher Initialisierung als darstellungsbereit gelten.
- WINDOW-007: Sichtbarkeit und Darstellungszustand MÜSSEN unabhängig vom Lebenszyklus verwaltet werden.
- WINDOW-008: Anwendungen MÜSSEN Schließanforderungen erhalten können.
- WINDOW-009: Anwendungen DÜRFEN reguläre Schließanforderungen innerhalb definierter Regeln ablehnen.
- WINDOW-010: Systembedingte Zwangsbeendigungen MÜSSEN ohne Zustimmung der Anwendung möglich sein.
- WINDOW-011: Schließvorgänge DÜRFEN die Fensterverwaltung nicht dauerhaft blockieren.
- WINDOW-012: Fensterressourcen MÜSSEN nach ihrer letzten Verwendung freigegeben werden.
- WINDOW-013: Aktive Scanout- oder Rendering-Buffer DÜRFEN nicht vorzeitig freigegeben werden.
- WINDOW-014: Beim Entfernen eines Fensters MÜSSEN Fokus, Z-Order und Fensterhierarchie aktualisiert werden.
- WINDOW-015: Der Absturz einer Anwendung MUSS die kontrollierte Bereinigung ihrer Fenster auslösen.
- WINDOW-016: Zerstörte Fensteridentitäten DÜRFEN keine weiteren Operationen akzeptieren.
- WINDOW-017: Lebenszyklusereignisse MÜSSEN in definierter Reihenfolge verarbeitet werden.
- WINDOW-018: Der Fensterlebenszyklus MUSS unabhängig von visuellen Animationen funktionieren.

## 6. Fenstererstellung

Die Erstellung erfolgt in folgenden Schritten:

1. **Request:** Anwendung fordert ein neues Fenster an.
2. **Authorize:** Berechtigungen und Ressourcenlimits prüfen.
3. **Create:** Fensterobjekt und eindeutige Identität erzeugen.
4. **Configure:** Fensterparameter und Beziehungen festlegen.
5. **Attach:** Surface zuordnen.
6. **Validate:** Konfiguration prüfen.
7. **Ready:** Fenster als darstellungsbereit markieren.
8. **Show:** Fenster bei entsprechender Anforderung sichtbar machen.

Ein Fenster DARF bereits vor der ersten Darstellung vollständig registriert sein.

## 7. Fensterbeendigung

### Reguläre Beendigung

1. Schließanforderung empfangen.
2. Anwendung über die Anforderung informieren.
3. Antwort innerhalb einer definierten Frist abwarten.
4. Bei Zustimmung den Zustand `Closing` aktivieren.
5. Fenster aus Fokus- und Eingabeverwaltung entfernen.
6. Darstellung kontrolliert beenden.
7. Ressourcen nach Abschluss ihrer Nutzung freigeben.
8. Fenster endgültig entfernen.

### Erzwungene Beendigung

Bei Anwendungsabsturz, Sicherheitsverletzung oder administrativer Beendigung DARF NovaOS den regulären Schließdialog umgehen.

Die Bereinigung MUSS auch ohne funktionierende Anwendung abgeschlossen werden können.

### Schließanimationen

Optionale Animationen DÜRFEN die Ressourcenfreigabe verzögern, solange dies kontrolliert und zeitlich begrenzt erfolgt.

Bei deaktivierten Effekten oder Fehlern MUSS die Beendigung unmittelbar fortgesetzt werden.

## 8. Ereignismodell

Der Window Manager stellt folgende grundlegende Ereignisse bereit:

| Ereignis | Bedeutung |
|---|---|
| WindowCreated | Fenster wurde registriert. |
| WindowReady | Fenster ist darstellungsbereit. |
| WindowShown | Fenster wurde sichtbar. |
| WindowHidden | Fenster wurde ausgeblendet. |
| WindowStateChanged | Darstellungszustand wurde geändert. |
| WindowCloseRequested | Schließanforderung wurde ausgelöst. |
| WindowClosing | Beendigung wurde eingeleitet. |
| WindowDestroyed | Fenster wurde endgültig entfernt. |

Ereignisse MÜSSEN eindeutig einem Fenster und dessen gültiger Lebenszyklusgeneration zugeordnet werden.

Verspätete Ereignisse zerstörter Fenster DÜRFEN keine neuen Fenster beeinflussen.

## 9. Fehlerbehandlung und Wiederherstellung

- Fehlerhafte Initialisierungen führen zur kontrollierten Rücknahme der Erstellung.
- Nicht reagierende Anwendungen blockieren keine anderen Fenster.
- Verwaiste Fenster werden erkannt und bereinigt.
- Surface- und Buffer-Referenzen werden sicher aufgelöst.
- Fensterhierarchien bleiben bei der Entfernung einzelner Fenster konsistent.
- Nach einem Compositor-Neustart können gültige Fenster erneut angebunden werden.
- Persistierte Fensterzustände können bei einem Anwendungsneustart wiederhergestellt werden.
- Lebenszyklusfehler werden über NovaOS-Diagnosefunktionen erfasst.

## 10. Ergebnis

NovaOS erhält einen klar definierten und zuverlässigen Fensterlebenszyklus.

Fenster werden kontrolliert erstellt, konfiguriert, angezeigt, ausgeblendet und beendet.

Die Trennung von Lebenszyklus, Darstellungszustand und visuellen Effekten ermöglicht eine stabile Fensterverwaltung mit sicherer Ressourcenfreigabe und zuverlässiger Fehlerbehandlung.