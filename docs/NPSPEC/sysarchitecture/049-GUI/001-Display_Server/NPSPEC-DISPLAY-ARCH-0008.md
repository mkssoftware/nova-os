# NPSPEC-DISPLAY-ARCH-0008 – Nova Display Recovery

**Status:** Angenommen  
**Bereich:** Display / Fehlerbehandlung und Wiederherstellung  
**System:** NovaOS

## 1. Zweck

Nova Display Recovery definiert die automatische Erkennung, Behandlung und Wiederherstellung von Fehlern innerhalb des grafischen Subsystems.

Ziel ist die Aufrechterhaltung oder schnellstmögliche Wiederherstellung einer funktionsfähigen Bildausgabe, möglichst ohne Unterbrechung der Benutzersitzung.

## 2. Architektur

- Integration in das NovaOS-Self-Healing-System.
- Überwachung von Display Server, Compositor, Window Manager und Grafiktreibern.
- Mehrstufige, zustandsbasierte Wiederherstellung.
- Isolation fehlerhafter Komponenten.
- Wiederherstellung einzelner Dienste ohne vollständigen Systemneustart.
- Persistente Speicherung der letzten gültigen Display-Konfiguration.
- Unterstützung eines unabhängigen minimalen Grafikmodus.
- Ereignisgesteuerte Fehlerbehandlung mit begrenzten Wiederholungsversuchen.

## 3. Kernfunktionen

- Erkennung abgestürzter oder blockierter Grafikkomponenten.
- Überwachung von Frame-Ausgabe und Display-Verfügbarkeit.
- Erkennung von GPU-Timeouts und Treiberfehlern.
- Neustart ausgefallener Display-Dienste.
- Wiederherstellung von Display-Konfigurationen.
- Reinitialisierung verlorener Grafikressourcen.
- Wiederherstellung von Client-Verbindungen.
- Umschaltung auf Software-Rendering bei Bedarf.
- Aktivierung eines minimalen Grafikmodus.
- Protokollierung und Analyse wiederkehrender Fehler.

## 4. Wiederherstellungsstufen

1. **Retry:** Fehlgeschlagene Operation erneut ausführen.
2. **Component Recovery:** Betroffene Komponente zurücksetzen oder neu starten.
3. **Resource Recovery:** Verlorene Grafikressourcen neu erstellen.
4. **Session Recovery:** Display-Verbindungen und grafische Sitzung wiederherstellen.
5. **Driver Recovery:** Grafiktreiber kontrolliert reinitialisieren.
6. **Fallback Recovery:** Auf Software-Rendering oder minimalen Grafikmodus wechseln.
7. **Critical Recovery:** Fehler an das NovaOS-Recovery-System übergeben.

Die Wiederherstellung SOLL mit der geringstmöglichen Eingriffstiefe beginnen.

## 5. Normative Anforderungen

- DISPLAY-001: Kritische Display-Komponenten MÜSSEN auf Ausfälle überwacht werden.
- DISPLAY-002: Erkannte Fehler MÜSSEN eindeutig klassifiziert werden.
- DISPLAY-003: Wiederherstellungsmaßnahmen MÜSSEN auf die betroffenen Komponenten begrenzt werden, sofern möglich.
- DISPLAY-004: Ein Compositor-Absturz DARF nicht zwangsläufig die Benutzersitzung beenden.
- DISPLAY-005: Verlorene Grafikressourcen MÜSSEN erkannt und als ungültig markiert werden.
- DISPLAY-006: Betroffene Clients MÜSSEN über notwendige Ressourcen-Neuerstellungen informiert werden.
- DISPLAY-007: Die letzte gültige Display-Konfiguration MUSS wiederherstellbar sein.
- DISPLAY-008: Wiederholte Wiederherstellungsversuche MÜSSEN begrenzt werden.
- DISPLAY-009: Bei fehlgeschlagener Hardwarebeschleunigung SOLL Software-Rendering verwendet werden.
- DISPLAY-010: Bei schwerwiegenden Fehlern SOLL eine minimale Bildausgabe verfügbar bleiben.
- DISPLAY-011: Wiederherstellungsmaßnahmen DÜRFEN keine Sicherheits- oder Capability-Prüfungen umgehen.
- DISPLAY-012: Fehler und Wiederherstellungsmaßnahmen MÜSSEN protokolliert werden.
- DISPLAY-013: Nicht betroffene Anwendungen SOLLEN während der Wiederherstellung weiterarbeiten können.
- DISPLAY-014: Die Wiederherstellung MUSS den definierten Display-Lebenszyklus berücksichtigen.
- DISPLAY-015: Fehlgeschlagene Wiederherstellungen MÜSSEN kontrolliert an die nächsthöhere Wiederherstellungsstufe übergeben werden.

## 6. Sitzungs- und Ressourcenwiederherstellung

- Erhaltung gültiger Fenster- und Sitzungszustände.
- Wiederverwendung weiterhin gültiger Ressourcen.
- Neuerstellung verlorener Surfaces und Buffer.
- Wiederherstellung der Display-Zuordnung und Skalierung.
- Erneute Synchronisierung zwischen Display Server und Compositor.
- Kontrollierte Wiederverbindung betroffener Clients.
- Vermeidung unnötiger Neuzeichnungen und Ressourceninitialisierungen.

Nicht rekonstruierbare Inhalte MÜSSEN durch die zuständigen Anwendungen neu bereitgestellt werden.

## 7. Sicherheit und Zuverlässigkeit

- Fail-Safe-Verhalten bei unbekannten Fehlerzuständen.
- Keine automatische Freigabe zusätzlicher Berechtigungen.
- Schutz vor endlosen Neustartschleifen.
- Zeitlimits für Wiederherstellungsoperationen.
- Isolation wiederholt fehlerhafter Komponenten.
- Erhaltung sicherheitskritischer Systemoberflächen, soweit technisch möglich.
- Diagnoseinformationen für das NovaOS-Self-Healing-System.
- Benutzerbenachrichtigung nur bei relevanten Einschränkungen oder erforderlichen Eingriffen.

## 8. Ergebnis

NovaOS erhält eine mehrstufige, automatische Wiederherstellungsarchitektur für sein grafisches Subsystem.

Ausfälle einzelner Komponenten können erkannt und möglichst ohne Verlust der Benutzersitzung behoben werden.

Bei schwerwiegenden Fehlern gewährleistet ein unabhängiger Fallback-Mechanismus die bestmögliche Wiederherstellung der Bildausgabe, ohne unnötig das gesamte Betriebssystem neu starten zu müssen.