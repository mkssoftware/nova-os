# NPSPEC-GPU-COMMANDBUFFER-0001 – Nova GPU Command Buffers

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova GPU Command Processing

## 1. Zweck

Nova GPU Command Buffers definiert die Aufzeichnung, Verwaltung, Wiederverwendung und Übergabe von GPU-Befehlssequenzen.

Ziel ist die effiziente Verarbeitung grafischer und allgemeiner GPU-Operationen bei geringem CPU-Overhead, kontrollierter Ressourcennutzung und sicherer Synchronisation.

Command Buffers trennen die Erstellung von GPU-Befehlen von deren tatsächlicher Ausführung.

## 2. Architekturprinzipien

- **Recorded Execution:** GPU-Befehle werden vor ihrer Ausführung aufgezeichnet.
- **Immutable Submission:** Eingereichte Befehlssequenzen bleiben unveränderlich.
- **Reusable:** Gültige Befehlssequenzen können wiederverwendet werden.
- **Parallel Recording:** Unabhängige Command Buffers können parallel erstellt werden.
- **Resource-Aware:** Ressourcenabhängigkeiten und Lebensdauern werden berücksichtigt.
- **Asynchronous:** CPU und GPU können unabhängig arbeiten.
- **Backend-Independent:** Einheitliches logisches Modell für unterschiedliche GPU-Treiber.
- **Deterministic:** Reproduzierbare Befehlsaufzeichnung bei identischen Eingaben.

## 3. Command Buffer Model

Ein Command Buffer enthält mindestens:

| Eigenschaft | Beschreibung |
|---|---|
| Buffer ID | Eindeutige Identifikation |
| Buffer Type | Primärer oder sekundärer Command Buffer |
| Queue Type | Vorgesehene GPU-Queue |
| State | Aktueller Lebenszykluszustand |
| Command Stream | Aufgezeichnete GPU-Befehle |
| Resource References | Verwendete GPU-Ressourcen |
| Dependency References | Erforderliche Ausführungsabhängigkeiten |
| Recording Version | Version der Befehlsaufzeichnung |
| Execution Flags | Eigenschaften der Wiederverwendung und Ausführung |

Das Modell MUSS unabhängig von der konkreten GPU-API bleiben.

## 4. Command Buffer Lifecycle

NovaOS definiert folgende Zustände:

`Initial → Recording → Executable → Pending → Executable`

Zusätzliche Zustände:

- **Invalid:** Buffer ist nicht mehr ausführbar.
- **Reset:** Buffer wird zur erneuten Aufzeichnung vorbereitet.
- **Destroyed:** Buffer wurde freigegeben.

Dabei gilt:

1. Nur ein Buffer im Zustand `Recording` darf verändert werden.
2. Ein Buffer MUSS vor der Submission abgeschlossen werden.
3. Ein eingereichter Buffer DARF nicht unzulässig verändert werden.
4. Eine erneute Submission ist nur bei gültigen Ressourcen und zulässigem Ausführungszustand erlaubt.
5. Die Freigabe DARF erst nach Abschluss aller relevanten GPU-Zugriffe erfolgen.

## 5. Command Recording

GPU-Befehle werden über einen Command Encoder aufgezeichnet.

Unterstützte Kategorien:

- Graphics Commands
- Compute Commands
- Transfer Commands
- Clear Commands
- Resource Barriers
- Pipeline State Changes
- Resource Bindings

Die Aufzeichnung MUSS die zulässige Befehlsreihenfolge und die Anforderungen des jeweiligen Ausführungskontexts berücksichtigen.

Ungültige Befehlssequenzen MÜSSEN vor der Submission erkannt werden.

## 6. Primary und Secondary Command Buffers

NovaOS SOLL zwei logische Command-Buffer-Typen unterstützen.

### Primary Command Buffer

- Kann direkt zur GPU-Ausführung eingereicht werden.
- Definiert den übergeordneten Ausführungskontext.
- Kann geeignete sekundäre Befehlssequenzen einbinden.

### Secondary Command Buffer

- Enthält wiederverwendbare oder unabhängig aufgezeichnete Befehlssequenzen.
- Wird innerhalb eines kompatiblen Ausführungskontexts verwendet.
- Unterstützt parallele Command-Aufzeichnung.

Die konkrete Umsetzung hängt von den Fähigkeiten des GPU-Backends ab.

## 7. Command Buffer Reuse

NovaOS SOLL unveränderte Command Buffers wiederverwenden können.

Voraussetzungen:

- Gültige Pipeline-Zustände.
- Gültige Ressourcenreferenzen.
- Kompatible Render Targets.
- Unveränderte erforderliche Bindings.
- Erfüllte Synchronisationsbedingungen.
- Zulässiger Ausführungszustand.

Die Wiederverwendung vermeidet erneute CPU-seitige Befehlsaufzeichnung, nicht zwangsläufig erneute GPU-Ausführung.

Änderungen an abhängigen Ressourcen oder Zuständen MÜSSEN eine erneute Validierung auslösen.

## 8. Parallel Command Recording

Mehrere CPU-Threads SOLLEN unabhängige Command Buffers gleichzeitig aufzeichnen können.

Dabei gilt:

- Jeder Encoder besitzt einen definierten Aufzeichnungszustand.
- Gemeinsame Ressourcen werden kontrolliert verwendet.
- Thread-lokale Command Pools SOLLEN unterstützt werden.
- Die Zusammenführung erfolgt anhand definierter Abhängigkeiten.
- Die Ausführungsreihenfolge bleibt explizit steuerbar.

Parallele Aufzeichnung DARF keine unkontrollierten Datenrennen verursachen.

## 9. Command Pools und Speicherverwaltung

NovaOS SOLL Command Pools zur effizienten Speicherverwaltung verwenden.

Dabei gilt:

- Command Buffers werden aus geeigneten Pools bereitgestellt.
- Speicher kann nach Abschluss der GPU-Ausführung wiederverwendet werden.
- Häufige Einzelallokationen SOLLEN vermieden werden.
- Speicherbudgets MÜSSEN berücksichtigt werden.
- Nicht mehr benötigte Command Buffers SOLLEN freigegeben werden.

Ein Pool DARF nicht zurückgesetzt werden, solange noch abhängige GPU-Operationen aktiv sind.

## 10. Resource Lifetime und Synchronization

Nova GPU Command Buffers MUSS mit Nova GPU Synchronization zusammenarbeiten.

Dabei gilt:

- Referenzierte Ressourcen MÜSSEN während ihrer gesamten Verwendung gültig bleiben.
- Read/Write-Abhängigkeiten MÜSSEN berücksichtigt werden.
- Erforderliche Barriers werden aufgezeichnet oder durch das Backend ergänzt.
- GPU-Fences und Semaphores können den Ausführungsabschluss absichern.
- Command Buffers dürfen erst nach zulässigem Abschluss zurückgesetzt oder freigegeben werden.

Eine erfolgreiche Submission bedeutet nicht, dass die GPU-Ausführung abgeschlossen ist.

## 11. Render Graph Integration

Nova GPU Command Buffers MUSS mit Nova Render Graph zusammenarbeiten.

Dabei gilt:

- Render Passes erzeugen geeignete Befehlssequenzen.
- Unabhängige Passes können parallel aufgezeichnet werden.
- Ressourcenabhängigkeiten bestimmen erforderliche Synchronisation.
- Unveränderte Befehlssequenzen können wiederverwendet werden.
- Nicht benötigte Render Passes erzeugen keine GPU-Befehle.

Der Render Graph bestimmt die logische Arbeitsstruktur.

Command Buffers enthalten deren ausführbare Befehlsrepräsentation.

## 12. Submission und Execution

Die Ausführung erfolgt über Nova GPU Command Submission.

Dabei gilt:

1. Command Buffer validieren.
2. Ressourcen und Abhängigkeiten prüfen.
3. Erforderliche Synchronisation vorbereiten.
4. Command Buffer an eine geeignete Queue übergeben.
5. Ausführungsabschluss überwachen.
6. Ressourcen und Command Buffer zur Wiederverwendung freigeben.

Mehrere Command Buffers DÜRFEN zu einer Submission zusammengefasst werden.

## 13. Sicherheit und Isolation

Command Buffers MÜSSEN die NovaOS-Sicherheitsarchitektur einhalten.

Insbesondere:

- Ressourcenreferenzen MÜSSEN autorisiert sein.
- Ungültige GPU-Adressen DÜRFEN nicht akzeptiert werden.
- Prozessübergreifende Ressourcenfreigaben MÜSSEN explizit erfolgen.
- Command Buffers DÜRFEN keine unzulässigen Speicherzugriffe ermöglichen.
- Privilegierte GPU-Operationen bleiben dem autorisierten Treiber vorbehalten.

Die Validierung MUSS an der Vertrauensgrenze zwischen Anwendungen und GPU-Backend erfolgen.

## 14. Fehlerbehandlung

NovaOS MUSS folgende Fehler kontrolliert behandeln:

- Ungültige Befehlssequenzen
- Nicht verfügbare Ressourcen
- Inkompatible Pipeline-Zustände
- Fehlgeschlagene Submission
- Synchronisationsfehler
- GPU-Timeouts
- Geräteverlust

Fehlgeschlagene Command Buffers DÜRFEN nicht als erfolgreich ausgeführt gelten.

Bei Geräteverlust MÜSSEN betroffene Command Buffers und Ressourcen kontrolliert invalidiert werden können.

## 15. Verbindliche Anforderungen

1. NovaOS MUSS GPU Command Buffers bereitstellen.
2. Aufzeichnung und Ausführung MÜSSEN getrennt sein.
3. Command Buffers MÜSSEN einen definierten Lebenszyklus besitzen.
4. Eingereichte Befehlssequenzen DÜRFEN nicht unzulässig verändert werden.
5. Ungültige Befehlssequenzen MÜSSEN erkannt werden.
6. Ressourcenreferenzen MÜSSEN während der Ausführung gültig bleiben.
7. Command Buffer Reuse SOLL unterstützt werden.
8. Parallele Command-Aufzeichnung SOLL möglich sein.
9. Command Pools SOLLEN unnötige Speicherallokationen reduzieren.
10. GPU Synchronization MUSS integriert sein.
11. Render Graph MUSS Command Buffers erzeugen können.
12. Mehrere Command Buffers SOLLEN gemeinsam eingereicht werden können.
13. Aktive Command Buffers DÜRFEN nicht vorzeitig freigegeben werden.
14. GPU-Ressourcen MÜSSEN capability-basiert geschützt werden.
15. Fehlerhafte Command Buffers MÜSSEN kontrolliert behandelt werden.
16. Im deterministischen Modus MÜSSEN Befehlsaufzeichnung und logische Reihenfolge bei identischen Eingaben reproduzierbar sein.

## 16. Ergebnis

Nova GPU Command Buffers stellt die zentrale Struktur zur effizienten Aufzeichnung und Verwaltung ausführbarer GPU-Befehle bereit.

Durch parallele Aufzeichnung, Command Pools, Wiederverwendung und asynchrone Submission wird der CPU-Overhead reduziert.

Damit erhält NovaOS eine skalierbare und hardwareunabhängige Grundlage für die sichere und effiziente GPU-Befehlsverarbeitung.