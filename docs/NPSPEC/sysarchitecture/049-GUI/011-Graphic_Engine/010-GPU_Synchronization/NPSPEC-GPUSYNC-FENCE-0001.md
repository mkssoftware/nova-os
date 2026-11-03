# NPSPEC-GPUSYNC-FENCE-0001 – Nova GPU Fences

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova GPU Synchronization

## 1. Zweck

Nova GPU Fences definiert die Synchronisation zwischen CPU und GPU zur zuverlässigen Erkennung abgeschlossener GPU-Operationen.

Ziel ist die asynchrone Überwachung von GPU-Arbeit, ohne die CPU unnötig zu blockieren.

Fences ermöglichen insbesondere die sichere Wiederverwendung von Command Buffers, GPU-Ressourcen und temporären Speicherbereichen.

## 2. Architekturprinzipien

- **Completion-Based:** Fences signalisieren definierte GPU-Abschlusspunkte.
- **Asynchronous:** GPU-Arbeit kann ohne blockierendes CPU-Warten ausgeführt werden.
- **Event-Driven:** Abschlussbenachrichtigungen erfolgen möglichst ereignisgesteuert.
- **Resource-Safe:** Ressourcen bleiben bis zum tatsächlichen Abschluss geschützt.
- **Non-Blocking:** Statusabfragen benötigen keine aktiven Warteschleifen.
- **Backend-Independent:** Einheitliches Fence-Modell für unterschiedliche GPU-Treiber.
- **Deterministic:** Logische Fence-Zustandsübergänge bleiben nachvollziehbar und reproduzierbar.

## 3. Fence Model

Ein GPU Fence besitzt mindestens folgende Eigenschaften:

| Eigenschaft | Beschreibung |
|---|---|
| Fence ID | Eindeutige Identifikation |
| Owner | Zugehöriger Prozess oder Systemdienst |
| Backend Handle | Hardwareabhängige Fence-Referenz |
| State | Aktueller Synchronisationszustand |
| Submission ID | Zugehörige GPU-Submission |
| Generation | Version zur sicheren Wiederverwendung |
| Completion Event | Optionale Abschlussbenachrichtigung |

Fence-Handles MÜSSEN unabhängig von direkten Hardwareadressen verwaltet werden.

## 4. Fence Lifecycle

NovaOS definiert folgende logische Zustände:

| Zustand | Beschreibung |
|---|---|
| Unsignaled | Abschlusspunkt wurde noch nicht erreicht |
| Pending | Fence überwacht eingereichte GPU-Arbeit |
| Signaled | Zugehöriger GPU-Abschlusspunkt wurde bestätigt |
| Failed | Zugehörige Operation konnte nicht erfolgreich abgeschlossen werden |
| Invalid | Fence ist nicht mehr verwendbar |

Dabei gilt:

1. Ein Fence darf nur für eine zulässige Submission aktiviert werden.
2. Ein Fence wird erst nach bestätigtem Abschluss signalisiert.
3. Ein Fence DARF nicht zurückgesetzt werden, solange seine vorherige Verwendung aktiv ist.
4. Wiederverwendung MUSS durch einen kontrollierten Reset oder eine neue Generation erfolgen.
5. Ein fehlgeschlagener Fence DARF nicht als erfolgreich signalisiert gelten.

## 5. Fence Operations

NovaOS MUSS folgende logische Operationen bereitstellen:

| Operation | Funktion |
|---|---|
| Create | Fence erstellen |
| GetStatus | Aktuellen Zustand abfragen |
| Wait | Auf einen Abschluss warten |
| RegisterEvent | Abschlussbenachrichtigung registrieren |
| Reset | Fence für zulässige Wiederverwendung zurücksetzen |
| Destroy | Fence kontrolliert freigeben |

`Wait` MUSS optionale Timeouts unterstützen.

`GetStatus` DARF keine blockierende GPU-Warteoperation erfordern.

## 6. CPU-GPU Synchronization

GPU Fences dienen der CPU-seitigen Überwachung von GPU-Ausführungsabschlüssen.

Dabei gilt:

- Die CPU SOLL während laufender GPU-Arbeit andere Aufgaben ausführen können.
- Abschlussereignisse SOLLEN blockierende Warteoperationen ersetzen.
- Mehrere ausstehende Fences MÜSSEN unabhängig überwacht werden können.
- Ein Timeout bedeutet nicht, dass die GPU-Ausführung fehlgeschlagen oder abgeschlossen ist.
- Die Freigabe abhängiger Ressourcen DARF erst nach gesichertem Abschluss erfolgen.

GPU-seitige Abhängigkeiten SOLLEN bevorzugt durch Semaphores oder vergleichbare Mechanismen umgesetzt werden.

## 7. Resource Lifetime Management

Nova GPU Fences MUSS mit Nova GPU Resource Management zusammenarbeiten.

Fences schützen insbesondere:

- Command Buffers
- GPU-Buffer
- Texturen
- Render Targets
- Temporäre Ressourcen
- Staging Buffers
- Shared Surfaces

Eine Ressource DARF erst wiederverwendet oder freigegeben werden, wenn sämtliche relevanten GPU-Zugriffe abgeschlossen sind.

Bei mehreren unabhängigen Verbrauchern MÜSSEN alle erforderlichen Abschlussbedingungen berücksichtigt werden.

## 8. Submission Integration

Nova GPU Fences MUSS mit Nova GPU Command Submission zusammenarbeiten.

Dabei gilt:

1. Eine Submission kann einen Fence als Abschlusssignal verwenden.
2. Der Fence wird dem definierten GPU-Abschlusspunkt zugeordnet.
3. Die erfolgreiche Annahme einer Submission signalisiert den Fence nicht.
4. Erst die bestätigte GPU-Fertigstellung erfüllt die Fence-Bedingung.
5. Fehlgeschlagene Submissions MÜSSEN einen definierten Fehlerzustand erzeugen.

Die konkrete Fence-Semantik wird durch das GPU-Backend umgesetzt.

## 9. Frame und Presentation Integration

Nova GPU Fences MUSS mit Nova Frame Scheduling und Nova Display Synchronization zusammenarbeiten.

Dabei gilt:

- Rendering-Abschlüsse können durch Fences überwacht werden.
- Composition Buffers bleiben bis zum Ende ihrer GPU-Verwendung geschützt.
- Presentation- und Scanout-Freigaben werden gesondert berücksichtigt.
- Ein abgeschlossener Rendering-Fence bedeutet nicht automatisch eine erfolgte Bildschirmpräsentation.
- Ein präsentierter Frame bedeutet nicht automatisch, dass sein Buffer bereits freigegeben werden darf.

## 10. Performance und Optimierung

NovaOS SOLL den Verwaltungsaufwand von Fences minimieren.

Dazu gehören:

- Wiederverwendung abgeschlossener Fence-Objekte.
- Ereignisgesteuerte Abschlussbenachrichtigungen.
- Vermeidung unnötiger CPU-Warteoperationen.
- Zusammenfassung geeigneter Abschlussbedingungen.
- Nutzung von Timeline Synchronization bei umfangreichen Submission-Folgen.

Fence-Pooling DARF keine alten Signalzustände auf neue GPU-Arbeit übertragen.

## 11. Sicherheit und Isolation

GPU Fences MÜSSEN die NovaOS-Sicherheitsarchitektur einhalten.

Dabei gilt:

- Fence-Handles MÜSSEN validiert werden.
- Eigentümer und Zugriffsrechte MÜSSEN definiert sein.
- Prozessübergreifende Fence-Freigaben erfordern entsprechende Capabilities.
- Anwendungen DÜRFEN fremde Fences nicht unautorisiert zurücksetzen oder signalisieren.
- Hardwareabhängige Fence-Operationen erfolgen über autorisierte GPU-Treiber.

## 12. Fehlerbehandlung

NovaOS MUSS folgende Fehler kontrolliert behandeln:

- Ungültige Fence-Handles.
- Unzulässige Reset-Operationen.
- Fehlgeschlagene Submissions.
- Fence-Timeouts.
- GPU-Timeouts.
- Geräteverlust.
- Unbekannte Abschlusszustände.

Dabei gilt:

- Timeouts DÜRFEN keine erfolgreiche Signalisierung auslösen.
- Fehlerhafte Fences MÜSSEN eindeutig gekennzeichnet werden.
- Ressourcen DÜRFEN bei unbekanntem GPU-Ausführungszustand nicht unsicher freigegeben werden.
- Bei Geräteverlust MUSS eine kontrollierte Invalidierung und Wiederherstellung möglich sein.

## 13. Verbindliche Anforderungen

1. NovaOS MUSS GPU Fences zur Überwachung abgeschlossener GPU-Arbeit bereitstellen.
2. Fence-Zustände MÜSSEN eindeutig unterscheidbar sein.
3. Erfolgreiche Submission und GPU-Abschluss MÜSSEN getrennt behandelt werden.
4. Fences DÜRFEN nicht vorzeitig signalisiert werden.
5. Nicht blockierende Statusabfragen MÜSSEN unterstützt werden.
6. Warteoperationen MÜSSEN Timeouts unterstützen.
7. Ereignisgesteuerte Abschlussbenachrichtigungen SOLLEN verfügbar sein.
8. Fence-Wiederverwendung MUSS kontrolliert erfolgen.
9. Aktive Fences DÜRFEN nicht unzulässig zurückgesetzt werden.
10. Ressourcenlebensdauern MÜSSEN durch Abschlussbedingungen geschützt werden.
11. GPU Command Submission MUSS integrierbar sein.
12. Frame- und Presentation-Synchronisation MÜSSEN berücksichtigt werden.
13. CPU-seitiges Busy Waiting SOLL vermieden werden.
14. Fence-Handles MÜSSEN capability-basiert geschützt werden.
15. Fehlerhafte oder unbekannte Fence-Zustände MÜSSEN sicher behandelt werden.
16. Im deterministischen Modus MÜSSEN logische Fence-Zustandsübergänge anhand kontrollierter Abschlussereignisse reproduzierbar sein.

## 14. Ergebnis

Nova GPU Fences ermöglicht die zuverlässige und asynchrone Erkennung abgeschlossener GPU-Operationen.

Durch kontrollierte Fence-Zustände, ereignisgesteuerte Benachrichtigungen und sichere Ressourcenfreigaben werden unnötige CPU-Wartezeiten reduziert.

Damit erhält NovaOS eine effiziente Synchronisationsgrundlage für Command Submission, GPU Resource Management, Rendering und Composition.