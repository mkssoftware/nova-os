# NPSPEC-GRAPHICS-DRIVER-0004 – Nova GPU Command Submission

**Status:** Angenommen  
**Bereich:** Graphics / GPU Command Submission  
**System:** NovaOS

## 1. Zweck

Nova GPU Command Submission definiert die sichere, asynchrone und effiziente Übermittlung von Grafikbefehlen an unterstützte Grafikprozessoren.

Ziel ist eine einheitliche Befehlsverarbeitung für unterschiedliche GPU-Architekturen, ohne Anwendungen an hardwarespezifische Befehlsformate zu binden.

Die Architektur unterstützt parallele Ausführung, Priorisierung, Synchronisation und kontrollierte Fehlerbehandlung.

## 2. Architektur

Die Befehlsübermittlung erfolgt über folgende Komponenten:

| Komponente | Aufgabe |
|---|---|
| Command Manager | Verwaltung von Grafikbefehlen |
| Command Buffer | Speicherung aufgezeichneter Befehle |
| Command Validator | Prüfung von Befehlen und Ressourcen |
| Command Scheduler | Planung der GPU-Ausführung |
| Command Queue | Logische Warteschlange für GPU-Aufträge |
| Driver Submission Interface | Übergabe an den Grafiktreiber |
| Completion Tracker | Überwachung abgeschlossener Aufträge |

Der Graphics Core verwaltet die logischen Befehle.

Das Rendering-Backend übersetzt diese in backendabhängige Operationen.

Der Grafiktreiber übernimmt die hardwarespezifische Übermittlung an die GPU.

## 3. Command Buffer

Ein Command Buffer enthält eine definierte Folge von Grafikoperationen.

Unterstützte Zustände:

| Zustand | Beschreibung |
|---|---|
| Initial | Erstellt, noch nicht aufgezeichnet |
| Recording | Befehlsaufzeichnung aktiv |
| Executable | Aufzeichnung abgeschlossen |
| Pending | Zur GPU-Ausführung übermittelt |
| Completed | Ausführung abgeschlossen |
| Invalid | Nicht mehr verwendbar |

Ein Command Buffer DARF während seiner laufenden Ausführung nicht verändert werden.

Die Wiederverwendung erfolgt ausschließlich nach Abschluss der erforderlichen Synchronisation.

## 4. Command Queues

NovaOS unterscheidet folgende logische Queue-Typen:

- **Graphics Queue:** Rendering- und Zeichenoperationen.
- **Compute Queue:** Allgemeine GPU-Berechnungen.
- **Transfer Queue:** Speicher- und Ressourcenübertragungen.
- **Presentation Queue:** Übergabe fertiger Bildinhalte, sofern vom Gerät unterstützt.

Ein Grafikgerät MUSS nicht für jeden Queue-Typ eine separate Hardware-Queue bereitstellen.

Mehrere logische Queues DÜRFEN auf dieselbe Hardware-Queue abgebildet werden.

Die tatsächlich verfügbaren Queue-Funktionen werden über die GPU-Capabilities ermittelt.

## 5. Befehlsübermittlung

Die Übermittlung erfolgt in folgenden Schritten:

1. Command Buffer erstellen.
2. Grafikbefehle aufzeichnen.
3. Aufzeichnung abschließen.
4. Ressourcen und Berechtigungen validieren.
5. Abhängigkeiten und Synchronisationsobjekte festlegen.
6. Auftrag in die geeignete Command Queue einreihen.
7. Auftrag an das GPU-Backend übergeben.
8. Hardwareausführung starten.
9. Abschluss oder Fehler erfassen.
10. Ressourcen und Synchronisationsobjekte aktualisieren.

Die Übermittlung MUSS grundsätzlich asynchron erfolgen können.

Die CPU DARF nicht unnötig auf die vollständige GPU-Ausführung warten.

## 6. Command Validation

Vor der Ausführung werden mindestens folgende Eigenschaften geprüft:

- Gültigkeit des Command Buffers.
- Unterstützte Befehle und GPU-Funktionen.
- Ressourcenhandles und Generationen.
- Zugriffsberechtigungen.
- Buffergrenzen und Speicherbereiche.
- Pipeline- und Shaderzustände.
- Ressourcenabhängigkeiten.
- Synchronisationsanforderungen.

Die Validierung erfolgt entsprechend den Verantwortlichkeiten von Graphics Core, Backend und Grafiktreiber.

Ungültige Befehle MÜSSEN vor der Hardwareausführung zurückgewiesen werden.

## 7. Command Scheduling

Der Command Scheduler verwaltet die Ausführungsreihenfolge der GPU-Aufträge.

Unterstützte Prioritätsklassen:

| Priorität | Verwendung |
|---|---|
| Critical | Zeitkritische Systemgrafik |
| High | Desktop-Compositing und interaktive Darstellung |
| Normal | Reguläre Anwendungen |
| Background | Nicht zeitkritische Grafikoperationen |

Die Priorisierung berücksichtigt:

- Auftragspriorität.
- Ressourcenabhängigkeiten.
- Verfügbare Hardware-Queues.
- GPU-Auslastung.
- Ausführungsfristen.
- Fairness zwischen Grafikclients.

Prioritäten sind Planungshinweise und garantieren keine hardwareseitige Unterbrechbarkeit.

Dauerhafte Verdrängung niedriger priorisierter Aufträge SOLL verhindert werden.

## 8. Synchronisation

NovaOS unterstützt folgende Synchronisationsmechanismen:

- Fences.
- Semaphores.
- Timeline Semaphores, sofern unterstützt.
- Memory Barriers.
- Queue-Abhängigkeiten.
- Completion Events.

Synchronisationsobjekte besitzen einen definierten Eigentümer und Lebenszyklus.

Die Ausführungsreihenfolge abhängiger Operationen MUSS gewährleistet sein.

Unabhängige Operationen DÜRFEN parallel ausgeführt werden, sofern die Hardware dies unterstützt.

## 9. Ressourcenverwaltung

Alle durch einen GPU-Auftrag verwendeten Ressourcen bleiben bis zum Abschluss der relevanten Zugriffe gültig.

Dabei gilt:

- Ressourcen werden während der Übermittlung referenziert.
- Laufende GPU-Zugriffe verhindern eine vorzeitige Freigabe.
- Schreib- und Leseabhängigkeiten werden berücksichtigt.
- Speicherübertragungen werden synchronisiert.
- Abgeschlossene Aufträge geben ihre Ressourcenreferenzen frei.

Die Verwaltung erfolgt gemäß NPSPEC-GRAPHICS-ARCH-0005 – Nova Graphics Resource Lifecycle.

## 10. Fehlerbehandlung und Recovery

Mögliche Fehler:

- Ungültiger Command Buffer.
- Nicht unterstützter Befehl.
- Ungültige Ressourcenreferenz.
- Fehlgeschlagene Übermittlung.
- GPU-Timeout.
- GPU-Hang.
- Geräteverlust.

Bei Fehlern MUSS der betroffene Auftrag einen eindeutig definierten Abschluss- oder Fehlerzustand erhalten.

Bei einem Geräteverlust werden ausstehende Aufträge kontrolliert abgebrochen und betroffene Ressourcen invalidiert.

Ein GPU-Reset DARF nur über die vorgesehenen Treiberschnittstellen erfolgen.

Eine automatische Wiederholung fehlgeschlagener Aufträge ist nur zulässig, wenn deren sichere Wiederholbarkeit gewährleistet ist.

## 11. Normative Anforderungen

- GRAPHICS-001: NovaOS MUSS eine einheitliche GPU-Befehlsübermittlung bereitstellen.
- GRAPHICS-002: GPU-Befehle MÜSSEN über definierte Command Buffers verarbeitet werden.
- GRAPHICS-003: Command Buffers MÜSSEN einen kontrollierten Lebenszyklus besitzen.
- GRAPHICS-004: Laufende Command Buffers DÜRFEN nicht unkontrolliert verändert werden.
- GRAPHICS-005: Befehle und Ressourcen MÜSSEN vor der Hardwareausführung validiert werden.
- GRAPHICS-006: Die Befehlsübermittlung MUSS asynchron erfolgen können.
- GRAPHICS-007: Mehrere logische Command Queues MÜSSEN unterstützt werden können.
- GRAPHICS-008: Queue-Funktionen MÜSSEN anhand der Gerätefähigkeiten ermittelt werden.
- GRAPHICS-009: Der Command Scheduler MUSS Auftragsprioritäten berücksichtigen können.
- GRAPHICS-010: Ressourcenabhängigkeiten MÜSSEN korrekt synchronisiert werden.
- GRAPHICS-011: Ressourcen DÜRFEN während laufender GPU-Zugriffe nicht freigegeben werden.
- GRAPHICS-012: Auftragsabschlüsse MÜSSEN eindeutig erkennbar sein.
- GRAPHICS-013: Fehlgeschlagene GPU-Aufträge MÜSSEN kontrolliert behandelt werden.
- GRAPHICS-014: GPU-Timeouts MÜSSEN erkannt werden können.
- GRAPHICS-015: Grafikclients DÜRFEN keine unautorisierten GPU-Befehle ausführen.
- GRAPHICS-016: Die Befehlsübermittlung MUSS unterschiedliche GPU-Architekturen unterstützen können.
- GRAPHICS-017: Die Ausführungsplanung MUSS ohne KI-Abhängigkeit funktionieren.

## 12. Integration in NovaOS

### Graphics Core

Verwaltet Command Buffers, Ressourcenreferenzen und logische Befehlszustände.

### Graphics Backend

Übersetzt Grafikoperationen in backendabhängige GPU-Befehle.

### Graphics Driver Interface

Übernimmt die hardwarespezifische Befehlsübermittlung.

### Graphics Memory Manager

Verwaltet Speicherressourcen und deren Zugriffsabhängigkeiten.

### Nova Compositor

Übermittelt zeitkritische Rendering-Aufträge über die vorgesehenen Grafik-Queues.

### Nova.Trust

Kontrolliert Berechtigungen und den Zugriff auf geschützte Grafikressourcen.

## 13. Ergebnis

NovaOS erhält eine einheitliche und sichere Architektur zur Übermittlung und Ausführung von GPU-Befehlen.

Durch Command Buffers, logische Queues, Priorisierung und asynchrone Verarbeitung können Grafikoperationen effizient und parallel ausgeführt werden.

Definierte Synchronisations- und Fehlermechanismen gewährleisten einen kontrollierten Umgang mit Ressourcen und GPU-Ausfällen.

**Grundsatz:** GPU-Befehle werden kontrolliert vorbereitet, validiert, geplant und ausgeführt. Die Hardware bestimmt die Ausführungsmöglichkeiten, NovaOS behält die Kontrolle über deren Nutzung.