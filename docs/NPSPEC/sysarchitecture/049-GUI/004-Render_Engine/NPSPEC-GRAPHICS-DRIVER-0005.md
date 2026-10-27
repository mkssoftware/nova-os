# NPSPEC-GRAPHICS-DRIVER-0005 – Nova Graphics Driver Recovery

**Status:** Angenommen  
**Bereich:** Graphics / Driver Recovery  
**System:** NovaOS

## 1. Zweck

Nova Graphics Driver Recovery definiert die Erkennung, Behandlung und Wiederherstellung von Fehlern innerhalb der Grafiktreiber und GPU-Hardware.

Ziel ist die größtmögliche Verfügbarkeit der grafischen Benutzeroberfläche, ohne bei einem Grafikfehler das gesamte Betriebssystem neu starten zu müssen.

NovaOS soll Grafikprobleme möglichst automatisch im Hintergrund beheben und den Benutzer nur dann informieren, wenn dies erforderlich oder sinnvoll ist.

## 2. Architektur

Die Wiederherstellung erfolgt über folgende Komponenten:

| Komponente | Aufgabe |
|---|---|
| Driver Health Monitor | Überwachung des Treiberzustands |
| GPU Watchdog | Erkennung blockierter GPU-Ausführungen |
| Recovery Manager | Koordination der Wiederherstellung |
| Device Reset Controller | Kontrollierter GPU-Reset |
| Resource Recovery | Wiederherstellung grafischer Ressourcen |
| Backend Fallback | Wechsel auf einen funktionsfähigen Renderer |
| Recovery Journal | Protokollierung von Fehlern und Maßnahmen |

Der Recovery Manager koordiniert Graphics Core, Graphics Driver Interface, Backend Manager und Graphics Resource Manager.

Die Wiederherstellung MUSS ohne KI-Abhängigkeit funktionieren.

## 3. Fehlerklassen

NovaOS unterscheidet folgende Grafikfehler:

| Fehlerklasse | Beschreibung |
|---|---|
| Driver Initialization Failure | Treiber kann nicht initialisiert werden |
| Driver Crash | Grafiktreiber fällt aus |
| GPU Hang | GPU verarbeitet keine Befehle mehr |
| Command Timeout | Grafikauftrag überschreitet seine zulässige Ausführungszeit |
| Memory Fault | Fehler beim Zugriff auf Grafikspeicher |
| Device Lost | Grafikgerät ist nicht mehr verwendbar |
| Display Failure | Bildausgabe fällt aus |
| Recovery Failure | Wiederherstellungsmaßnahme schlägt fehl |

Fehler MÜSSEN nach Ursache, betroffenem Gerät und Schweregrad klassifiziert werden.

Ein Command Timeout allein DARF nicht automatisch als vollständiger GPU-Ausfall gewertet werden.

## 4. Fehlererkennung

Der Driver Health Monitor überwacht:

- Treiberzustände.
- GPU-Command-Queues.
- Ausstehende Grafikoperationen.
- Abschlussmeldungen und Interrupts.
- Geräteantworten.
- Speicherfehler.
- Wiederholte Treiberabstürze.

Der GPU Watchdog verwendet definierte Zeitlimits und Fortschrittsindikatoren.

Zeitlimits MÜSSEN an die jeweilige Hardware und Auftragsklasse angepasst werden können.

Fehlererkennung und Wiederherstellung DÜRFEN den regulären Grafikbetrieb nicht unnötig beeinträchtigen.

## 5. Recovery-Zustände

| Zustand | Beschreibung |
|---|---|
| Healthy | Regulärer Grafikbetrieb |
| Degraded | Eingeschränkter Grafikbetrieb |
| FaultDetected | Fehler erkannt |
| Recovering | Wiederherstellung läuft |
| Fallback | Ersatzbackend aktiv |
| Restored | Grafikbetrieb wiederhergestellt |
| Failed | Automatische Wiederherstellung fehlgeschlagen |

Zustandsübergänge werden durch den Recovery Manager kontrolliert.

Während einer Wiederherstellung DÜRFEN keine neuen Aufträge an ein als nicht verwendbar eingestuftes Gerät übermittelt werden.

## 6. Wiederherstellungsstrategie

NovaOS verwendet eine abgestufte Wiederherstellung.

1. **Auftragsebene:** Fehlerhaften Auftrag abbrechen, sofern unterstützt.
2. **Queue-Ebene:** Betroffene Command Queue zurücksetzen, sofern unterstützt.
3. **Geräteebene:** GPU kontrolliert zurücksetzen.
4. **Treiberebene:** Grafiktreiber neu initialisieren.
5. **Backend-Ebene:** Auf ein anderes Rendering-Backend wechseln.
6. **Fallback-Ebene:** Software-Rendering aktivieren.

Es SOLL zunächst die am wenigsten eingreifende geeignete Maßnahme verwendet werden.

Jede Maßnahme MUSS anhand definierter Erfolgsbedingungen überprüft werden.

Fehlgeschlagene Maßnahmen DÜRFEN nicht unbegrenzt wiederholt werden.

## 7. GPU-Reset

Ein GPU-Reset erfolgt ausschließlich über die vorgesehenen Treiberschnittstellen.

Ablauf:

1. Betroffenes Grafikgerät isolieren.
2. Neue Command Submissions sperren.
3. Laufende Aufträge abschließen oder abbrechen.
4. Betroffene Ressourcen als nicht verwendbar kennzeichnen.
5. Hardware-Reset ausführen.
6. Gerät und Treiber neu initialisieren.
7. Gerätefähigkeiten erneut ermitteln.
8. Ressourcen wiederherstellen oder invalidieren.
9. Grafikbetrieb fortsetzen.

Ein Reset DARF andere Grafikgeräte nicht unnötig beeinträchtigen.

Kann der Reset nicht sicher durchgeführt werden, MUSS eine alternative Wiederherstellungsstrategie gewählt werden.

## 8. Ressourcenwiederherstellung

Der Graphics Resource Manager verwaltet die Wiederherstellung betroffener Ressourcen.

Ressourcen werden unterschieden in:

- **Reconstructable:** Aus vorhandenen Daten erneut erstellbar.
- **Reloadable:** Aus einer verfügbaren Quelle erneut ladbar.
- **Transient:** Temporär und neu erzeugbar.
- **Non-Recoverable:** Nicht automatisch wiederherstellbar.

Ressourcen MÜSSEN nach einem Geräteverlust auf ihre Gültigkeit geprüft werden.

Nicht wiederherstellbare Ressourcen werden invalidiert und den betroffenen Grafikclients gemeldet.

Eine automatische Wiederherstellung DARF keine veralteten oder unvollständigen Inhalte als gültig ausgeben.

## 9. Software-Fallback

Bei nicht wiederherstellbarer GPU-Beschleunigung wechselt NovaOS auf ein geeignetes Software-Backend.

Dabei gilt:

- Der Graphics Backend Manager koordiniert den Wechsel.
- Der Compositor verwendet anschließend verfügbare CPU-Rendering-Funktionen.
- Nicht unterstützte Effekte werden deaktiviert oder vereinfacht.
- Die grundlegende Desktopbedienung bleibt nach Möglichkeit verfügbar.
- Anwendungen werden über verlorene Ressourcen informiert.

Ein Software-Fallback setzt einen gültigen und erreichbaren Display- beziehungsweise Framebuffer-Ausgabepfad voraus.

Ist auch dieser nicht verfügbar, MUSS NovaOS den Fehler über einen anderen verfügbaren Diagnose- oder Recovery-Kanal behandeln.

## 10. Automatische Wiederherstellung

NovaOS SOLL Grafikfehler möglichst ohne Benutzerinteraktion beheben.

Dabei gelten folgende Regeln:

- Wiederherstellungsmaßnahmen erfolgen nach definierten Richtlinien.
- Wiederholte Fehler werden erkannt.
- Fehlgeschlagene Treiber können vorübergehend deaktiviert werden.
- Erfolgreiche Maßnahmen werden protokolliert.
- Der vorherige Grafikzustand wird soweit möglich wiederhergestellt.
- Benutzeränderungen und laufende Arbeiten SOLLEN erhalten bleiben.

Die Wiederherstellung DARF keine nicht autorisierten Änderungen an Benutzerdateien oder Anwendungseinstellungen durchführen.

## 11. Benutzerinformation und Diagnose

NovaOS informiert den Benutzer zurückhaltend.

- Erfolgreich behobene, unkritische Fehler können ohne Unterbrechung protokolliert werden.
- Bei spürbaren Einschränkungen SOLL eine dezente Benachrichtigung erfolgen.
- Erforderliche Benutzerentscheidungen werden ausdrücklich angefordert.
- Wiederkehrende Probleme werden in der zentralen Wartungskonsole angezeigt.
- Fehlerursache, Wiederherstellungsmaßnahme und Ergebnis bleiben nachvollziehbar.

Die Diagnoseinformationen enthalten mindestens:

- Zeitpunkt und Fehlerklasse.
- Betroffenes Grafikgerät und Treiber.
- Fehlerzustand.
- Durchgeführte Maßnahmen.
- Wiederherstellungsergebnis.
- Anzahl wiederholter Fehler.

Diagnosedaten bleiben standardmäßig lokal.

## 12. Normative Anforderungen

- GRAPHICS-001: NovaOS MUSS Grafiktreiber- und GPU-Fehler erkennen können.
- GRAPHICS-002: Die Fehlerbehandlung MUSS über einen zentralen Recovery Manager koordiniert werden.
- GRAPHICS-003: GPU-Timeouts MÜSSEN anhand definierter Kriterien erkannt werden.
- GRAPHICS-004: Fehlerhafte Grafikgeräte MÜSSEN für neue Befehlsübermittlungen gesperrt werden können.
- GRAPHICS-005: Wiederherstellungsmaßnahmen MÜSSEN kontrolliert und nachvollziehbar erfolgen.
- GRAPHICS-006: GPU-Resets DÜRFEN ausschließlich über autorisierte Treiberschnittstellen erfolgen.
- GRAPHICS-007: Verlorene Grafikressourcen MÜSSEN invalidiert oder gültig wiederhergestellt werden.
- GRAPHICS-008: Nicht wiederherstellbare Ressourcen MÜSSEN den betroffenen Grafikclients gemeldet werden.
- GRAPHICS-009: Ein Software-Fallback MUSS bei verfügbarem Ausgabepfad möglich sein.
- GRAPHICS-010: Fehler eines Grafikgeräts DÜRFEN andere Geräte nicht unkontrolliert beeinträchtigen.
- GRAPHICS-011: Wiederherstellungsversuche MÜSSEN zeitlich und hinsichtlich ihrer Wiederholungsanzahl begrenzt sein.
- GRAPHICS-012: Wiederholte Treiberfehler MÜSSEN erkannt werden.
- GRAPHICS-013: Kritische Recovery-Operationen MÜSSEN mit dem Graphics Resource Lifecycle synchronisiert werden.
- GRAPHICS-014: Erfolgreiche und fehlgeschlagene Wiederherstellungsmaßnahmen MÜSSEN protokolliert werden.
- GRAPHICS-015: Der Benutzer SOLL bei automatisch behobenen Fehlern nicht unnötig unterbrochen werden.
- GRAPHICS-016: Die Wiederherstellung MUSS ohne Netzwerkverbindung funktionieren.
- GRAPHICS-017: Die Recovery-Logik MUSS deterministisch und ohne KI-Abhängigkeit arbeiten.

## 13. Integration in NovaOS

### Graphics Core

Erkennt ungültige Grafikzustände und koordiniert die betroffenen Grafikressourcen.

### Graphics Driver Interface

Stellt gerätespezifische Fehler-, Reset- und Initialisierungsfunktionen bereit.

### Graphics Backend Manager

Übernimmt den Wechsel zwischen GPU- und Software-Rendering.

### Nova Compositor

Stellt die Bildkomposition nach Möglichkeit über ein funktionsfähiges Backend wieder her.

### Nova.Trust

Kontrolliert die Berechtigungen für Treiberneustarts, Geräte-Resets und privilegierte Recovery-Operationen.

### Nova Recovery

Übernimmt übergeordnete Wiederherstellungsmaßnahmen, wenn die lokale Grafiktreiber-Recovery nicht ausreicht.

## 14. Ergebnis

NovaOS erhält eine abgestufte und weitgehend automatische Wiederherstellung für Grafiktreiber und GPU-Hardware.

Treiberabstürze, blockierte GPU-Aufträge und Geräteverluste können erkannt, isoliert und nach Möglichkeit ohne vollständigen Systemneustart behoben werden.

Durch kontrollierte GPU-Resets, Ressourcenwiederherstellung und Software-Fallback bleibt die grafische Benutzeroberfläche möglichst verfügbar.

**Grundsatz:** NovaOS versucht Grafikprobleme selbstständig und unauffällig zu beheben. Der Benutzer behält die Kontrolle und kann jederzeit nachvollziehen, was passiert ist und welche Maßnahmen durchgeführt wurden.