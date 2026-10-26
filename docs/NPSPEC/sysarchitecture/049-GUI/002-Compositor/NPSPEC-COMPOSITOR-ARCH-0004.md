# NPSPEC-COMPOSITOR-ARCH-0004 – Nova Compositor Lifecycle

**Status:** Angenommen  
**Bereich:** Compositor / Lebenszyklusverwaltung  
**System:** NovaOS

## 1. Zweck

Nova Compositor Lifecycle definiert die Initialisierung, den Betrieb, die Zustandsübergänge und die kontrollierte Beendigung des Nova Compositors.

Ziel ist eine frühzeitige Desktopdarstellung, ein stabiler Grafikbetrieb und die möglichst unterbrechungsfreie Wiederherstellung nach Fehlern.

## 2. Architektur

- Eigenständiger Lebenszyklus innerhalb des NovaOS-Displaysystems.
- Integration in Nova Display Lifecycle und NovaOS-Service-Management.
- Zustandsbasierte und ereignisgesteuerte Steuerung.
- Stufenweise Initialisierung der notwendigen Komponenten.
- Asynchrones Nachladen optionaler Funktionen.
- Unabhängiger Neustart des Compositors.
- Unterstützung dynamischer Display- und Ressourcenänderungen.
- Integration in Nova Display Recovery und das Self-Healing-System.

## 3. Lebenszykluszustände

- **Uninitialized:** Compositor noch nicht initialisiert.
- **Initializing:** Initialisierung der Kernkomponenten.
- **BasicReady:** Grundlegende Bildkomposition verfügbar.
- **Ready:** Rendering-Backend und Frame-Scheduling betriebsbereit.
- **Running:** Regulärer Kompositionsbetrieb.
- **Suspending:** Vorbereitung auf einen Energiesparzustand.
- **Suspended:** Komposition vorübergehend angehalten.
- **Resuming:** Wiederaufnahme der Bildkomposition.
- **Recovering:** Wiederherstellung nach einem Fehler.
- **Stopping:** Kontrollierte Beendigung.
- **Stopped:** Compositor vollständig beendet.
- **Failed:** Nicht automatisch behebbarer Fehler.

## 4. Kernfunktionen

- Initialisierung der Compositor-Komponenten.
- Auswahl und Aktivierung geeigneter Rendering-Backends.
- Aufbau der Verbindung zum Nova Display Server.
- Initialisierung von Surface Manager und Scene Manager.
- Aktivierung von Frame Scheduler und Composition Engine.
- Verwaltung von Zustandsübergängen.
- Verarbeitung von Suspend- und Resume-Ereignissen.
- Behandlung dynamischer Display-Änderungen.
- Kontrollierter Neustart nach Fehlern.
- Freigabe und Wiederherstellung grafischer Ressourcen.

## 5. Normative Anforderungen

- COMPOSITOR-001: Der Compositor MUSS einen eindeutig definierten Lebenszyklus besitzen.
- COMPOSITOR-002: Zustandsübergänge MÜSSEN kontrolliert und nachvollziehbar erfolgen.
- COMPOSITOR-003: Die Initialisierung MUSS Abhängigkeiten zwischen Komponenten berücksichtigen.
- COMPOSITOR-004: Eine grundlegende Bildkomposition MUSS ohne optionale Grafikeffekte möglich sein.
- COMPOSITOR-005: Der Zustand BasicReady SOLL möglichst früh erreicht werden.
- COMPOSITOR-006: Nicht kritische Funktionen SOLLEN nach Erreichen der Desktopbereitschaft im Hintergrund initialisiert werden.
- COMPOSITOR-007: Der Compositor MUSS unabhängig vom Window Manager gestartet und neu gestartet werden können.
- COMPOSITOR-008: Zustandsänderungen MÜSSEN betroffenen Systemkomponenten mitgeteilt werden.
- COMPOSITOR-009: Display-Hotplug-Ereignisse MÜSSEN während des Betriebs verarbeitet werden.
- COMPOSITOR-010: Suspend und Resume MÜSSEN definierte Zustandsübergänge besitzen.
- COMPOSITOR-011: Verlorene Grafikressourcen MÜSSEN erkannt und kontrolliert wiederhergestellt werden.
- COMPOSITOR-012: Ein Compositor-Neustart SOLL die bestehende Benutzersitzung erhalten.
- COMPOSITOR-013: Fehlgeschlagene Initialisierungen MÜSSEN kontrolliert behandelt werden.
- COMPOSITOR-014: Bei fehlender Hardwarebeschleunigung MUSS ein Software-Fallback verfügbar sein.
- COMPOSITOR-015: Ressourcen MÜSSEN beim Beenden kontrolliert freigegeben werden.
- COMPOSITOR-016: Wiederholte Neustartversuche MÜSSEN begrenzt werden.
- COMPOSITOR-017: Sicherheitskritische Oberflächen MÜSSEN während der Wiederherstellung geschützt bleiben.
- COMPOSITOR-018: Der Lebenszyklus MUSS ohne KI-Abhängigkeit funktionieren.

## 6. Initialisierungsreihenfolge

1. **Connect:** Verbindung zum Nova Display Server herstellen.
2. **Initialize:** Kernkomponenten und Ressourcenverwaltung initialisieren.
3. **Backend:** Geeignetes Rendering-Backend aktivieren.
4. **Scene:** Surface- und Szenenverwaltung bereitstellen.
5. **Scheduler:** Frame-Scheduling initialisieren.
6. **BasicReady:** Grundlegende Bildkomposition ermöglichen.
7. **Ready:** Regulären Kompositionsbetrieb vorbereiten.
8. **Running:** Desktopdarstellung aktivieren.
9. **Background:** Optionale Effekte und Optimierungen nachladen.

Bereits verfügbare Ressourcen SOLLEN weiterverwendet werden.

## 7. Fehlerbehandlung und Wiederherstellung

- Überwachung kritischer Compositor-Komponenten.
- Erkennung blockierter Rendering-Operationen.
- Kontrollierter Neustart fehlerhafter Komponenten.
- Wiederherstellung gültiger Surface- und Szenenzustände.
- Neuerstellung verlorener Grafikressourcen.
- Wiederverbindung zum Nova Display Server.
- Umschaltung auf Software-Rendering bei Bedarf.
- Erhaltung der Benutzersitzung, sofern technisch möglich.
- Rückfall auf grundlegende Bildkomposition bei schwerwiegenden Fehlern.
- Eskalation nicht behebbarer Fehler an Nova Display Recovery.

## 8. Performance und Zuverlässigkeit

- Minimale Initialisierung bis zur ersten Desktopdarstellung.
- Asynchrones Laden optionaler Komponenten.
- Vermeidung unnötiger Ressourcen-Neuinitialisierungen.
- Wiederverwendung bestehender Grafikbuffer.
- Priorisierung der grundlegenden Bildausgabe.
- Kontrollierte Wiederaufnahme nach Suspend.
- Minimierung sichtbarer Unterbrechungen bei Neustarts.
- Begrenzung von Wiederherstellungszeiten und Neustartversuchen.
- Protokollierung relevanter Zustandsübergänge.
- Energieeffizienter Betrieb bei inaktiven Displays.

## 9. Ergebnis

NovaOS erhält eine klar definierte Lebenszyklusverwaltung für seinen Compositor.

Die grundlegende Desktopdarstellung wird möglichst früh bereitgestellt, während optionale Grafikfunktionen im Hintergrund initialisiert werden.

Fehler, Hardwareänderungen und Energiesparzustände können kontrolliert verarbeitet werden, ohne die gesamte Desktopumgebung unnötig neu starten zu müssen.