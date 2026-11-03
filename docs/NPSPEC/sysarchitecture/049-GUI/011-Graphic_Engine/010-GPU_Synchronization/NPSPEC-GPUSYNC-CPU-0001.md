# NPSPEC-GPUSYNC-CPU-0001 – Nova CPU-GPU Synchronization

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova GPU Synchronization

## 1. Zweck

Nova CPU-GPU Synchronization definiert die Koordination von CPU- und GPU-Operationen einschließlich Ausführungsabhängigkeiten, Speicherzugriffen und Abschlussbenachrichtigungen.

Ziel ist eine möglichst asynchrone Zusammenarbeit beider Verarbeitungseinheiten ohne unnötige Blockierungen, Datenkonflikte oder Speichertransfers.

## 2. Architekturprinzipien

- **Asynchronous:** CPU und GPU arbeiten möglichst unabhängig.
- **Non-Blocking:** Aktives Warten wird vermieden.
- **Event-Driven:** Abschlüsse werden bevorzugt über Ereignisse gemeldet.
- **Explicit Dependencies:** Ausführungsabhängigkeiten sind eindeutig definiert.
- **Memory-Coherent:** Speicherzugriffe werden entsprechend der Hardwarearchitektur synchronisiert.
- **Resource-Safe:** Ressourcen bleiben bis zum Abschluss aller relevanten Zugriffe geschützt.
- **Backend-Independent:** Einheitliche Schnittstellen für unterschiedliche GPU-Treiber.
- **Deterministic:** Logische Synchronisationsabläufe bleiben reproduzierbar.

## 3. Synchronization Model

NovaOS unterscheidet folgende Synchronisationsrichtungen:

| Richtung | Funktion |
|---|---|
| CPU → GPU | Bereitstellung von Daten und Freigabe zur GPU-Verarbeitung |
| GPU → CPU | Abschlussmeldung und Bereitstellung von GPU-Ergebnissen |
| Bidirectional | Koordinierter Zugriff auf gemeinsam verwendete Ressourcen |

Eine Synchronisationsanforderung beschreibt mindestens:

- Betroffene Ressource oder Submission
- Zugriffsrichtung
- Erforderliche Abschlussbedingung
- Speicher- und Cache-Anforderungen
- Optionalen Timeout
- Fehlerstatus

## 4. CPU → GPU Synchronization

Vor GPU-Zugriffen MUSS NovaOS sicherstellen, dass erforderliche CPU-Schreiboperationen abgeschlossen und für die GPU sichtbar sind.

Dabei gilt:

1. CPU-seitige Datenänderungen werden abgeschlossen.
2. Erforderliche Cache- und Speicheroperationen werden durchgeführt.
3. Die Ressource wird für den GPU-Zugriff freigegeben.
4. Die GPU-Submission wird unter Einhaltung ihrer Abhängigkeiten ausgeführt.

CPU-Schreibzugriffe DÜRFEN nicht unkoordiniert mit inkompatiblen GPU-Zugriffen überlappen.

## 5. GPU → CPU Synchronization

CPU-Zugriffe auf GPU-Ergebnisse DÜRFEN erst erfolgen, wenn die erforderlichen GPU-Operationen abgeschlossen und ihre Ergebnisse für die CPU sichtbar sind.

NovaOS MUSS dafür unterstützen:

- GPU Fences
- Timeline Synchronization
- Nicht blockierende Statusabfragen
- Ereignisgesteuerte Abschlussbenachrichtigungen
- Blockierende Warteoperationen mit Timeout

Eine erfolgreiche Submission bedeutet keinen abgeschlossenen GPU-Zugriff.

## 6. Shared Memory Synchronization

NovaOS MUSS unterschiedliche Speicherarchitekturen berücksichtigen.

Dazu gehören:

- Kohärenter gemeinsam genutzter Speicher
- Nicht kohärenter gemeinsam genutzter Speicher
- Dedizierter GPU-Speicher
- CPU-sichtbare GPU-Buffer
- Staging- und Transfer-Buffer

Erforderliche Cache Flushes, Invalidierungen und Speicherbarrieren MÜSSEN durch das GPU-Backend korrekt umgesetzt werden.

Ein GPU-Fence allein garantiert nicht auf jeder Hardware die erforderliche CPU-Speichersichtbarkeit.

## 7. Resource Access Management

Gemeinsam verwendete Ressourcen MÜSSEN kontrollierte Zugriffsphasen besitzen.

Dabei gilt:

- Gleichzeitige Lesezugriffe können zulässig sein.
- Schreibzugriffe erfordern geeignete Synchronisation.
- CPU-Mapping und GPU-Zugriffe MÜSSEN auf Kompatibilität geprüft werden.
- Ressourcen DÜRFEN nicht vorzeitig freigegeben werden.
- Mehrere GPU-Queues und Consumer MÜSSEN berücksichtigt werden.

Unnötige Datenkopien SOLLEN vermieden werden.

## 8. Asynchronous Completion

NovaOS SOLL GPU-Abschlüsse ereignisgesteuert verarbeiten.

Dazu gehören:

- Completion Events
- Callback-Registrierung
- Scheduler-Benachrichtigungen
- Asynchrone Ressourcenfreigaben
- Fortsetzung wartender Tasks

Wartende CPU-Threads SOLLEN durch den NovaOS-Scheduler blockiert und nach Erfüllung ihrer Bedingungen wieder aktiviert werden.

CPU-seitiges Busy Waiting SOLL vermieden werden.

## 9. Command Submission Integration

Nova CPU-GPU Synchronization MUSS mit Nova GPU Command Submission zusammenarbeiten.

Dabei gilt:

- Submissions können CPU-seitige Abschlussbedingungen besitzen.
- GPU-Arbeit wird möglichst ohne synchrones CPU-Warten eingereicht.
- Ressourcen bleiben während ausstehender GPU-Zugriffe gültig.
- Completion Events werden erst nach bestätigtem Abschluss ausgelöst.
- Fehlgeschlagene Submissions MÜSSEN wartende CPU-Tasks kontrolliert benachrichtigen.

## 10. Performance und Latenz

NovaOS SOLL CPU-GPU-Synchronisation auf geringe Latenz und niedrigen Verwaltungsaufwand optimieren.

Dazu gehören:

- Vermeidung unnötiger Synchronisationspunkte.
- Reduzierung blockierender GPU-Abfragen.
- Nutzung von Timeline-Werten für mehrere Submissions.
- Wiederverwendung geeigneter Speicherbereiche.
- Vermeidung unnötiger CPU-GPU-Datenübertragungen.
- Begrenzung gleichzeitig ausstehender GPU-Aufträge.

Synchronisationsoptimierungen DÜRFEN keine Datenkonflikte verursachen.

## 11. Sicherheit und Isolation

CPU-GPU-Synchronisation MUSS die NovaOS-Sicherheitsarchitektur einhalten.

Dabei gilt:

- Ressourcen und Synchronisationsobjekte MÜSSEN capability-basiert geschützt werden.
- Prozessübergreifende Speicherfreigaben erfordern ausdrückliche Berechtigungen.
- Anwendungen DÜRFEN keine fremden GPU-Ressourcen unautorisiert verwenden.
- Speicher-Mappings MÜSSEN kontrolliert eingerichtet und aufgehoben werden.
- Ungültige Synchronisationshandles MÜSSEN zurückgewiesen werden.

## 12. Fehlerbehandlung

NovaOS MUSS folgende Fehler kontrolliert behandeln:

- GPU-Timeouts
- Fehlgeschlagene Submissions
- Ungültige Speicherzugriffe
- Nicht erfüllbare Synchronisationsbedingungen
- Geräteverlust
- Unbekannte GPU-Abschlusszustände

Ein Timeout DARF nicht als erfolgreicher Abschluss interpretiert werden.

Bei unbekanntem GPU-Zustand MÜSSEN betroffene Ressourcen bis zur sicheren Freigabe oder kontrollierten Wiederherstellung geschützt bleiben.

## 13. Verbindliche Anforderungen

1. NovaOS MUSS CPU-GPU-Synchronisation bereitstellen.
2. CPU→GPU- und GPU→CPU-Abhängigkeiten MÜSSEN unterstützt werden.
3. GPU Fences und Timeline Synchronization MÜSSEN integrierbar sein.
4. Nicht blockierende Abschlussabfragen MÜSSEN möglich sein.
5. Ereignisgesteuerte Abschlussbenachrichtigungen SOLLEN unterstützt werden.
6. CPU-seitiges Busy Waiting SOLL vermieden werden.
7. Speicher- und Cache-Kohärenz MÜSSEN berücksichtigt werden.
8. CPU- und GPU-Zugriffe auf gemeinsame Ressourcen MÜSSEN koordiniert werden.
9. Ressourcen DÜRFEN nicht vorzeitig wiederverwendet werden.
10. Unnötige Speichertransfers SOLLEN vermieden werden.
11. GPU Command Submission MUSS integriert sein.
12. Warteoperationen MÜSSEN Timeouts unterstützen.
13. Fehlerhafte GPU-Operationen DÜRFEN keine erfolgreichen Abschlussmeldungen erzeugen.
14. Ressourcen und Synchronisationsobjekte MÜSSEN capability-basiert geschützt werden.
15. Ein sicherer Fallback für unterschiedliche Speicherarchitekturen MUSS verfügbar sein.
16. Im deterministischen Modus MÜSSEN logische Synchronisationsabhängigkeiten bei identischen Eingaben reproduzierbar sein.

## 14. Ergebnis

Nova CPU-GPU Synchronization ermöglicht die sichere und effiziente Zusammenarbeit zwischen CPU und GPU.

Durch asynchrone Abschlussverarbeitung, kontrollierte Speicherzugriffe und hardwaregerechte Cache-Synchronisation werden unnötige Wartezeiten und Datenkonflikte vermieden.

Damit erhält NovaOS eine leistungsfähige Synchronisationsgrundlage für Rendering, Compute, Ressourcenverwaltung und Display-Ausgabe.