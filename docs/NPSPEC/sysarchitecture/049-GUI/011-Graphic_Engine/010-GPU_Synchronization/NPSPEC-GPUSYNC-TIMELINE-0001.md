# NPSPEC-GPUSYNC-TIMELINE-0001 – Nova Timeline Synchronization

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova GPU Synchronization

## 1. Zweck

Nova Timeline Synchronization definiert die Synchronisation von CPU- und GPU-Operationen anhand monoton steigender Synchronisationswerte.

Ziel ist die effiziente Verwaltung mehrerer GPU-Abschlusspunkte ohne die Erstellung separater Synchronisationsobjekte für jede Submission.

Timeline Synchronization MUSS unabhängig von der verwendeten GPU-API modelliert werden und einen sicheren Fallback für Hardware ohne native Timeline-Unterstützung ermöglichen.

## 2. Architekturprinzipien

- **Monotonic:** Synchronisationswerte steigen ausschließlich an.
- **Value-Based:** Abhängigkeiten beziehen sich auf definierte Timeline-Werte.
- **Asynchronous:** CPU und GPU arbeiten möglichst unabhängig.
- **Multi-Submission:** Eine Timeline kann mehrere GPU-Submissions koordinieren.
- **Cross-Queue:** Unterschiedliche GPU-Queues können synchronisiert werden.
- **Non-Blocking:** Unnötige CPU-Warteoperationen werden vermieden.
- **Resource-Safe:** Ressourcen bleiben bis zum erforderlichen Abschluss geschützt.
- **Backend-Independent:** Einheitliches Modell für unterschiedliche GPU-Treiber.

## 3. Timeline Model

Eine Timeline besitzt mindestens folgende Eigenschaften:

| Eigenschaft | Beschreibung |
|---|---|
| Timeline ID | Eindeutige Identifikation |
| Owner | Zugehöriger Prozess oder Systemdienst |
| Current Value | Zuletzt bestätigter Synchronisationswert |
| Pending Signals | Noch ausstehende Signalwerte |
| Wait Dependencies | Registrierte Wartebedingungen |
| Backend Handle | Hardwareabhängige Referenz |
| Generation | Version zur Lebenszyklusverwaltung |

Timeline-Werte MÜSSEN als vorzeichenlose 64-Bit-Werte oder durch ein gleichwertiges Modell repräsentiert werden.

Ein Timeline-Wert beschreibt einen Synchronisationsfortschritt, keinen Zeitpunkt.

## 4. Timeline Operations

NovaOS MUSS folgende logische Operationen unterstützen:

| Operation | Funktion |
|---|---|
| Create | Timeline erstellen |
| Signal | Definierten Fortschrittswert signalisieren |
| Wait | Auf einen bestimmten Timeline-Wert warten |
| GetValue | Aktuell bestätigten Wert abfragen |
| RegisterEvent | Abschlussbenachrichtigung registrieren |
| Destroy | Timeline kontrolliert freigeben |

Dabei gilt:

1. Signalwerte MÜSSEN größer als der bereits bestätigte Wert sein.
2. Ein Wait auf Wert `N` gilt als erfüllt, sobald der bestätigte Wert mindestens `N` beträgt.
3. Signaloperationen DÜRFEN den bestätigten Wert nicht reduzieren.
4. Noch ausstehende Signaloperationen DÜRFEN nicht als abgeschlossen gemeldet werden.
5. Überläufe MÜSSEN durch kontrollierte Lebenszyklusverwaltung verhindert werden.

## 5. Timeline Lifecycle

Eine Timeline besitzt folgende logische Zustände:

- **Created:** Timeline wurde erstellt.
- **Active:** Signal- oder Wait-Operationen sind möglich.
- **Pending:** Mindestens eine GPU-Signaloperation steht aus.
- **Failed:** Ein Fehler verhindert die ordnungsgemäße Fortsetzung.
- **Invalid:** Timeline ist nicht mehr verwendbar.

Der aktuelle Timeline-Wert bleibt während des gültigen Lebenszyklus monoton.

Ein Reset auf einen kleineren Wert ist nicht zulässig.

Für einen Neustart MUSS eine neue Timeline oder eine eindeutig getrennte Generation verwendet werden.

## 6. GPU Submission Integration

Nova Timeline Synchronization MUSS mit Nova GPU Command Submission zusammenarbeiten.

Eine Submission kann:

- Auf einen oder mehrere Timeline-Werte warten.
- Nach Abschluss einen Timeline-Wert signalisieren.
- Abhängigkeiten zu anderen Submissions definieren.
- Mehrere Ressourcenfreigaben koordinieren.

Die erfolgreiche Annahme einer Submission DARF ihren Signalwert nicht vorzeitig als erreicht markieren.

Die Signalreihenfolge MUSS den Synchronisationsregeln des jeweiligen GPU-Backends entsprechen.

## 7. Cross-Queue Synchronization

Timeline Synchronization MUSS unterschiedliche GPU-Queues koordinieren können.

Beispiel:

1. Transfer Queue schreibt eine Textur.
2. Transfer Queue signalisiert Timeline-Wert `10`.
3. Graphics Queue wartet auf Wert `10`.
4. Graphics Queue verwendet die Textur.
5. Graphics Queue signalisiert Wert `11`.

Erforderliche Resource Barriers und Queue Ownership Transfers MÜSSEN zusätzlich berücksichtigt werden.

Eine Timeline allein ersetzt keine notwendigen Speicher- oder Layoutübergänge.

## 8. CPU-GPU Synchronization

NovaOS SOLL Timeline-Werte zur asynchronen Überwachung von GPU-Fortschritt verwenden.

Dabei gilt:

- Die CPU kann den bestätigten Wert nicht blockierend abfragen.
- Warteoperationen MÜSSEN Timeouts unterstützen.
- Ereignisgesteuerte Benachrichtigungen SOLLEN verfügbar sein.
- Mehrere CPU-Threads können auf unterschiedliche Werte warten.
- Ein Timeout DARF den Timeline-Wert nicht verändern.

CPU-seitiges Busy Waiting SOLL vermieden werden.

## 9. Resource Lifetime Management

Timeline-Werte SOLLEN zur Verwaltung von GPU-Ressourcenlebensdauern verwendet werden.

Eine Ressource kann einem erforderlichen Abschlusswert zugeordnet werden.

Beispiel:

`Resource.ReleaseAfter(TimelineID, Value)`

Dabei gilt:

- Ressourcen bleiben bis zum erforderlichen Abschluss gültig.
- Mehrere relevante Queues oder Consumer MÜSSEN berücksichtigt werden.
- Ressourcen DÜRFEN nicht aufgrund lediglich eingereichter Signaloperationen freigegeben werden.
- Fehlgeschlagene GPU-Operationen MÜSSEN eine sichere Ressourcenbehandlung auslösen.

## 10. Render Graph Integration

Nova Timeline Synchronization MUSS mit Nova Render Graph zusammenarbeiten.

Dabei gilt:

- Render Pass Dependencies können Timeline-Abhängigkeiten erzeugen.
- Cross-Queue-Ausführung wird über geeignete Signal- und Wait-Werte koordiniert.
- Unabhängige Render Passes bleiben parallel ausführbar.
- Redundante Synchronisationsoperationen SOLLEN vermieden werden.
- Resource Barriers werden unabhängig von Timeline-Werten korrekt geplant.

Die Timeline-Planung MUSS mit GPU Execution Scheduling abgestimmt werden.

## 11. Timeline Optimization

NovaOS SOLL Timeline Synchronization zur Reduzierung des Synchronisationsaufwands einsetzen.

Mögliche Optimierungen:

- Verwendung einer Timeline für mehrere Submissions.
- Zusammenfassung geeigneter Abschlussbedingungen.
- Reduzierung einzelner Fence- und Semaphore-Objekte.
- Ereignisgesteuerte Verarbeitung erreichter Timeline-Werte.
- Effiziente Verwaltung ausstehender Ressourcenfreigaben.

Eine gemeinsame Timeline DARF keine unnötige Serialisierung unabhängiger GPU-Arbeit verursachen.

## 12. Hardware Fallback

NovaOS MUSS einen Fallback für GPUs ohne native Timeline-Unterstützung bereitstellen.

Mögliche Implementierungen:

- Binary Semaphores.
- GPU Fences.
- Treiberverwaltete Submission-Abschlusswerte.
- Softwareseitige Fortschrittsverwaltung.

Dabei gilt:

- Die logische Timeline-Semantik MUSS erhalten bleiben.
- Abhängigkeiten DÜRFEN nicht vorzeitig erfüllt werden.
- Fehlende Hardwarefunktionen DÜRFEN keine unsicheren Synchronisationszustände verursachen.
- Reduzierte Parallelität ist zulässig.

Ein Software-Fallback DARF GPU-Abschlüsse nur anhand tatsächlich bestätigter Hardwareereignisse veröffentlichen.

## 13. Sicherheit und Isolation

Timeline-Objekte MÜSSEN die NovaOS-Sicherheitsarchitektur einhalten.

Dabei gilt:

- Timeline-Handles MÜSSEN validiert werden.
- Eigentümer und Berechtigungen MÜSSEN definiert sein.
- Prozessübergreifende Freigaben erfolgen capability-basiert.
- Anwendungen DÜRFEN fremde Timelines nicht unautorisiert signalisieren.
- Ungültige oder manipulierte Timeline-Werte MÜSSEN zurückgewiesen werden.

## 14. Fehlerbehandlung

NovaOS MUSS folgende Fehler kontrolliert behandeln:

- Ungültige Timeline-Handles.
- Unzulässige Signalwerte.
- Nicht erfüllbare Wait-Abhängigkeiten.
- Zirkuläre Synchronisationsabhängigkeiten.
- Timeline-Wertüberläufe.
- Fehlgeschlagene GPU-Submissions.
- GPU-Timeouts.
- Geräteverlust.

Dabei gilt:

- Fehler DÜRFEN keine erfolgreiche Signalisierung vortäuschen.
- Abhängige Operationen MÜSSEN kontrolliert behandelt werden.
- Ressourcen MÜSSEN bis zur sicheren Freigabe geschützt bleiben.
- Fehlerhafte Timelines MÜSSEN invalidiert oder kontrolliert wiederhergestellt werden können.

## 15. Verbindliche Anforderungen

1. NovaOS MUSS ein logisches Timeline-Synchronisationsmodell bereitstellen.
2. Timeline-Werte MÜSSEN monoton steigen.
3. Signal- und Wait-Operationen MÜSSEN unterstützt werden.
4. Nicht blockierende Wertabfragen MÜSSEN möglich sein.
5. CPU-Warteoperationen MÜSSEN Timeouts unterstützen.
6. Ereignisgesteuerte Abschlussbenachrichtigungen SOLLEN verfügbar sein.
7. Mehrere GPU-Submissions MÜSSEN über eine Timeline koordinierbar sein.
8. Cross-Queue Synchronization MUSS unterstützt werden.
9. GPU Resource Barriers MÜSSEN berücksichtigt werden.
10. Timeline-Werte DÜRFEN nicht vorzeitig als erreicht gelten.
11. GPU Resource Management MUSS Timeline-Abschlusswerte verwenden können.
12. Render Graph MUSS Timeline-Abhängigkeiten integrieren können.
13. Unnötige CPU-Warteoperationen SOLLEN vermieden werden.
14. Timeline-Überläufe MÜSSEN verhindert werden.
15. Ein sicherer Fallback ohne native Hardwareunterstützung MUSS verfügbar sein.
16. Timeline-Handles MÜSSEN capability-basiert geschützt werden.
17. Fehlerhafte Synchronisationszustände MÜSSEN kontrolliert behandelt werden.
18. Im deterministischen Modus MÜSSEN die logischen Signal- und Wait-Abhängigkeiten bei identischen Eingaben reproduzierbar sein.

## 16. Ergebnis

Nova Timeline Synchronization ermöglicht die effiziente Verwaltung fortlaufender GPU-Synchronisationspunkte über mehrere Submissions und Queues hinweg.

Durch monotone Fortschrittswerte, asynchrone Abschlussüberwachung und die Integration mit GPU Resource Management können Ressourcen sicher verwaltet und unnötige Synchronisationsobjekte reduziert werden.

Damit erhält NovaOS eine skalierbare Synchronisationsgrundlage für paralleles Rendering, Compute, Transfer und GPU-Ausführungsplanung.