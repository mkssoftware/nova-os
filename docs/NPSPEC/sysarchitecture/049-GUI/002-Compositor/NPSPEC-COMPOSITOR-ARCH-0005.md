# NPSPEC-COMPOSITOR-ARCH-0005 – Nova Compositor Fault Recovery

**Status:** Angenommen  
**Bereich:** Compositor / Fehlerbehandlung und Wiederherstellung  
**System:** NovaOS

## 1. Zweck

Nova Compositor Fault Recovery definiert die Erkennung, Isolation und automatische Wiederherstellung von Fehlern innerhalb des Nova Compositors.

Ziel ist die schnellstmögliche Wiederherstellung der grafischen Darstellung ohne unnötige Unterbrechung oder Verlust der Benutzersitzung.

## 2. Architektur

- Integration in Nova Display Recovery und das NovaOS-Self-Healing-System.
- Unabhängige Überwachung durch einen externen Supervisor.
- Mehrstufige, zustandsbasierte Wiederherstellung.
- Isolation fehlerhafter Komponenten und Clients.
- Kontrollierter Neustart einzelner Compositor-Komponenten.
- Wiederherstellung von Surface-, Szenen- und Rendering-Zuständen.
- Unterstützung alternativer Rendering-Backends.
- Unabhängiger minimaler Grafikmodus als letzter Fallback.

## 3. Kernfunktionen

- Erkennung von Abstürzen, Deadlocks und Timeouts.
- Überwachung der Frame-Verarbeitung und GPU-Ausführung.
- Erkennung ungültiger oder verlorener Grafikressourcen.
- Neustart fehlerhafter Rendering-Komponenten.
- Wiederherstellung des Szenengraphen.
- Wiederverbindung zum Nova Display Server.
- Neuerstellung verlorener Surfaces und Buffer.
- Umschaltung auf alternative Rendering-Backends.
- Wiederaufnahme der Frame-Komposition.
- Protokollierung und Klassifizierung von Fehlern.

## 4. Wiederherstellungsstufen

1. **Operation Recovery:** Fehlgeschlagene Operation kontrolliert wiederholen.
2. **Resource Recovery:** Ungültige Grafikressourcen neu erstellen.
3. **Component Recovery:** Betroffene Compositor-Komponente zurücksetzen.
4. **Backend Recovery:** Rendering-Backend neu initialisieren oder wechseln.
5. **Compositor Recovery:** Compositor-Prozess kontrolliert neu starten.
6. **Fallback Recovery:** Minimale Bildkomposition oder Software-Rendering aktivieren.
7. **System Recovery:** Nicht behebbare Fehler an Nova Display Recovery eskalieren.

Die Wiederherstellung MUSS mit der geringstmöglichen Eingriffstiefe beginnen.

## 5. Normative Anforderungen

- COMPOSITOR-001: Kritische Compositor-Komponenten MÜSSEN auf Ausfälle überwacht werden.
- COMPOSITOR-002: Fehler MÜSSEN erkannt und eindeutig klassifiziert werden.
- COMPOSITOR-003: Wiederherstellungsmaßnahmen MÜSSEN möglichst auf betroffene Komponenten begrenzt bleiben.
- COMPOSITOR-004: Der Compositor MUSS unabhängig vom Window Manager neu gestartet werden können.
- COMPOSITOR-005: Ein Compositor-Absturz DARF nicht automatisch die Benutzersitzung beenden.
- COMPOSITOR-006: Verlorene Grafikressourcen MÜSSEN erkannt und invalidiert werden.
- COMPOSITOR-007: Ressourcen DÜRFEN erst nach Abschluss oder Abbruch ausstehender Zugriffe freigegeben werden.
- COMPOSITOR-008: Wiederherstellbare Szenen- und Fensterzustände SOLLEN erhalten bleiben.
- COMPOSITOR-009: Betroffene Clients MÜSSEN über erforderliche Ressourcen-Neuerstellungen informiert werden.
- COMPOSITOR-010: Bei GPU-Fehlern MUSS ein alternativer Rendering-Pfad verfügbar sein.
- COMPOSITOR-011: Wiederherstellungsversuche MÜSSEN zeitlich und zahlenmäßig begrenzt werden.
- COMPOSITOR-012: Fehlerhafte Clients DÜRFEN keine unkontrollierten Neustartschleifen verursachen.
- COMPOSITOR-013: Wiederherstellungsmaßnahmen DÜRFEN keine Capability- oder Sicherheitsprüfungen umgehen.
- COMPOSITOR-014: Sicherheitskritische Oberflächen MÜSSEN nach der Wiederherstellung mit ihren ursprünglichen Schutzanforderungen bereitgestellt werden.
- COMPOSITOR-015: Nicht betroffene Systemkomponenten SOLLEN während der Wiederherstellung weiterarbeiten.
- COMPOSITOR-016: Fehler und Wiederherstellungsmaßnahmen MÜSSEN protokolliert werden.
- COMPOSITOR-017: Der Wiederherstellungsprozess MUSS mit Nova Compositor Lifecycle abgestimmt sein.
- COMPOSITOR-018: Wiederholte Fehler MÜSSEN an das NovaOS-Self-Healing-System gemeldet werden.

## 6. Zustandswiederherstellung

### Scene Recovery
Wiederherstellung der Szenenstruktur, Ebenenreihenfolge, Transformationen und Sichtbarkeit.

### Surface Recovery
Wiederherstellung gültiger Surface-Zuordnungen und Anforderung verlorener Inhalte.

### Buffer Recovery
Neuerstellung ungültiger Buffer unter Berücksichtigung laufender Synchronisationsoperationen.

### Backend Recovery
Neustart des Grafik-Backends oder Wechsel auf einen alternativen Rendering-Pfad.

### Session Recovery
Wiederherstellung der Client-Verbindungen und Fortsetzung der bestehenden Benutzersitzung.

### Fallback Recovery
Aktivierung einer grundlegenden Bildkomposition ohne optionale Grafikeffekte.

## 7. Wiederherstellungsablauf

1. **Detect:** Fehler oder ungewöhnlichen Zustand erkennen.
2. **Classify:** Ursache und betroffene Komponenten bestimmen.
3. **Isolate:** Fehlerhafte Ressourcen oder Komponenten isolieren.
4. **Preserve:** Verfügbare gültige Zustände sichern.
5. **Recover:** Geeignete Wiederherstellungsmaßnahme ausführen.
6. **Validate:** Funktionsfähigkeit und Sicherheitszustand überprüfen.
7. **Resume:** Reguläre Bildkomposition wieder aufnehmen.
8. **Report:** Fehler und Ergebnis protokollieren.

Schlägt eine Wiederherstellung fehl, MUSS kontrolliert zur nächsten geeigneten Wiederherstellungsstufe gewechselt werden.

## 8. Performance und Zuverlässigkeit

- Minimierung sichtbarer Bildunterbrechungen.
- Wiederverwendung weiterhin gültiger Grafikressourcen.
- Vermeidung unnötiger Client- und Desktop-Neustarts.
- Begrenzung von Wiederherstellungszeiten.
- Schutz vor wiederholten Absturz- und Neustartschleifen.
- Automatische Reduzierung optionaler Grafikeffekte bei instabiler Hardware.
- Erhaltung des letzten gültigen Bildes, sofern sicher und technisch möglich.
- Priorisierung der Wiederherstellung grundlegender Systemoberflächen.
- Asynchrone Wiederherstellung nicht kritischer Ressourcen.
- Diagnose wiederkehrender Fehler zur langfristigen Systemstabilisierung.

## 9. Ergebnis

NovaOS erhält eine mehrstufige, automatische Fehlerbehandlung für seinen Compositor.

Fehlerhafte Komponenten, Grafikressourcen und Rendering-Backends können gezielt wiederhergestellt werden, ohne unnötig die gesamte Desktopumgebung neu zu starten.

Die Integration in das NovaOS-Self-Healing-System ermöglicht einen stabilen, ausfallsicheren Grafikbetrieb mit möglichst unterbrechungsfreier Benutzererfahrung.