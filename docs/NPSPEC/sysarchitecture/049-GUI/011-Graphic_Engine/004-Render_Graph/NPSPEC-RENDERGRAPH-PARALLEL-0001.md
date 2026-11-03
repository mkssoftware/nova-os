# NPSPEC-RENDERGRAPH-PARALLEL-0001 – Nova Parallel Render Execution

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Render Graph

## 1. Zweck

Nova Parallel Render Execution definiert die parallele Vorbereitung und Ausführung unabhängiger Rendering-Operationen innerhalb des Nova Render Graph.

Ziel ist die effiziente Auslastung mehrerer CPU-Kerne und verfügbarer GPU-Ausführungseinheiten bei möglichst geringer Rendering-Latenz.

Parallelisierung erfolgt ausschließlich unter Einhaltung sämtlicher Ressourcenabhängigkeiten und Synchronisationsanforderungen.

## 2. Architekturprinzipien

- **Dependency-Driven:** Nur unabhängig ausführbare Operationen werden parallelisiert.
- **Task-Based:** Rendering-Arbeit wird in ausführbare Aufgaben zerlegt.
- **Multi-Core:** Mehrere CPU-Kerne können gleichzeitig Rendering-Befehle vorbereiten.
- **Multi-Queue:** Unterstützte GPU Queues können parallel verwendet werden.
- **Asynchronous:** CPU- und GPU-Verarbeitung können überlappend erfolgen.
- **Cost-Aware:** Parallelisierung erfolgt nur bei ausreichendem Nutzen.
- **Incremental:** Unveränderte Aufgabenstrukturen können wiederverwendet werden.
- **Deterministic:** Ein reproduzierbarer Ausführungsmodus wird unterstützt.

## 3. Parallel Execution Model

NovaOS unterscheidet drei Ebenen der Parallelisierung:

| Ebene | Aufgabe |
|---|---|
| CPU Parallelism | Gleichzeitige Vorbereitung unabhängiger Rendering-Aufträge |
| GPU Parallelism | Gleichzeitige Ausführung unabhängiger GPU-Operationen |
| CPU-GPU Overlap | Überlappung von Befehlsvorbereitung und GPU-Ausführung |

Die tatsächliche Parallelität hängt von Hardware, Backend und Ressourcenabhängigkeiten ab.

Eine parallele Ausführungsplanung garantiert keine gleichzeitige Hardwareausführung.

## 4. Execution Tasks

Ein Parallel Execution Task besitzt mindestens:

- **Task ID:** Eindeutige Identifikation.
- **Pass Reference:** Zugehöriger Render Pass.
- **Dependencies:** Vorausgesetzte Aufgaben.
- **Resource Access:** Verwendete Ressourcen und Zugriffsarten.
- **Execution Type:** CPU Recording, Graphics, Compute oder Copy.
- **Priority:** Optionale Ausführungspriorität.
- **Estimated Cost:** Erwarteter Verarbeitungsaufwand.

Tasks DÜRFEN mehrere kompatible Render Passes enthalten.

## 5. Dependency-Based Parallelism

Der Nova Render Graph bestimmt unabhängig ausführbare Aufgaben.

Dabei gilt:

- Aufgaben ohne gegenseitige Abhängigkeiten dürfen parallel ausgeführt werden.
- RAW-, WAR- und WAW-Abhängigkeiten müssen berücksichtigt werden.
- Explizite Ausführungsbedingungen müssen erhalten bleiben.
- Überlappende Ressourcenzugriffe müssen kontrolliert werden.

Unabhängigkeit MUSS anhand der tatsächlichen Zugriffs- und Synchronisationsanforderungen festgestellt werden.

## 6. CPU Parallel Command Recording

NovaOS SOLL Rendering-Befehle auf mehreren CPU-Kernen vorbereiten können.

Dazu können unabhängige Aufgaben auf Worker Threads verteilt werden.

Dabei MÜSSEN:

- Thread-Sicherheitsanforderungen des Backends eingehalten werden.
- Gemeinsame Datenstrukturen vor konkurrierenden Änderungen geschützt werden.
- Command Buffers korrekt verwaltet werden.
- Ressourcenreferenzen während der Aufzeichnung gültig bleiben.

Unnötige globale Sperren SOLLEN vermieden werden.

## 7. GPU Parallel Execution

NovaOS SOLL mehrere GPU-Ausführungsqueues unterstützen können.

Dazu gehören:

- Graphics Queues
- Compute Queues
- Transfer Queues

Unabhängige Aufgaben DÜRFEN unterschiedlichen Queues zugewiesen werden.

Die Zuweisung MUSS die Fähigkeiten des jeweiligen GPU-Backends berücksichtigen.

Ein Backend mit nur einer geeigneten Queue MUSS weiterhin unterstützt werden.

## 8. Asynchronous Compute

Compute-Operationen DÜRFEN parallel zu Graphics-Operationen ausgeführt werden, sofern Hardware und Abhängigkeiten dies erlauben.

NovaOS SOLL:

- Unabhängige Compute-Aufgaben erkennen.
- Geeignete GPU Queues auswählen.
- Ressourcenübergänge berücksichtigen.
- Queue-übergreifende Synchronisation bereitstellen.

Asynchronous Compute DARF nicht erzwungen werden, wenn die erwarteten Synchronisationskosten den Nutzen übersteigen.

## 9. Task Scheduling

Nova Render Graph Scheduling bestimmt die zulässige Ausführungsreihenfolge.

Dabei SOLLEN berücksichtigt werden:

- Abhängigkeiten
- Kritische Ausführungspfade
- Geschätzte Verarbeitungskosten
- Verfügbare Worker Threads
- GPU Queue Capabilities
- Ressourcenverfügbarkeit

Die Aufgabenverteilung DARF dynamisch erfolgen.

Die Anzahl paralleler Aufgaben SOLL an die verfügbaren Ressourcen angepasst werden.

## 10. Synchronization

Parallele Rendering-Aufgaben benötigen kontrollierte Synchronisation.

NovaOS MUSS insbesondere berücksichtigen:

- CPU-Thread-Synchronisation
- GPU Fences
- GPU Semaphores
- Timeline Synchronization
- Resource Barriers
- Queue Ownership Transfers

Ausführungsabhängigkeiten und Speicher-Sichtbarkeit MÜSSEN korrekt gewährleistet sein.

Unnötige Synchronisationspunkte SOLLEN vermieden werden.

## 11. Resource Safety

Parallele Aufgaben DÜRFEN keine unkontrollierten konkurrierenden Ressourcenzugriffe verursachen.

Dabei gilt:

- Ressourcenlebenszeiten müssen erhalten bleiben.
- Gleichzeitige Schreibzugriffe benötigen definierte Synchronisation.
- Resource Aliasing muss parallele Nutzungsintervalle berücksichtigen.
- Freigaben dürfen erst nach Abschluss relevanter Zugriffe erfolgen.

Eine Ressource DARF nicht wiederverwendet werden, solange ausstehende Aufgaben sie benötigen.

## 12. Incremental Parallel Execution

NovaOS SOLL unveränderte Aufgabenstrukturen zwischen Graph-Aktualisierungen wiederverwenden.

Bei Änderungen werden ausschließlich betroffene Aufgaben und Abhängigkeiten aktualisiert.

Nicht benötigte Rendering-Aufgaben SOLLEN entfallen.

Gültige Cache-Ergebnisse DÜRFEN die erneute Ausführung einzelner Aufgaben ersetzen.

## 13. Cost-Based Parallelization

Nova Rendering Cost Estimation SOLL den Nutzen paralleler Verarbeitung bewerten.

Berücksichtigt werden:

- Task-Erzeugungskosten
- Scheduling-Aufwand
- Synchronisationskosten
- Erwartete Ausführungsdauer
- Ressourcenverbrauch
- Auswirkungen auf andere GPU-Aufgaben

Kleine Rendering-Aufgaben DÜRFEN zusammengefasst oder sequenziell ausgeführt werden, wenn dies effizienter ist.

## 14. Deterministic Execution

NovaOS MUSS einen deterministischen Ausführungsmodus unterstützen können.

Dabei gilt:

- Aufgabenabhängigkeiten bleiben eindeutig.
- Die logische Ausführungsplanung ist reproduzierbar.
- Nicht deterministische Optimierungsentscheidungen können deaktiviert werden.
- Rendering-Ergebnisse müssen unabhängig von zulässigen Ausführungsreihenfolgen korrekt bleiben.

Eine identische tatsächliche zeitliche Ausführung auf unterschiedlicher Hardware wird nicht vorausgesetzt.

## 15. Fehlerbehandlung

NovaOS MUSS Fehler paralleler Rendering-Aufgaben kontrolliert behandeln.

Dazu gehören:

- Fehlgeschlagene Task-Ausführung
- Ungültige Ressourcenreferenzen
- Nicht erfüllte Abhängigkeiten
- Synchronisationsfehler
- Nicht unterstützte Queue-Konfigurationen

Abhängige Aufgaben DÜRFEN nicht mit ungültigen Eingaben fortgesetzt werden.

Bei fehlgeschlagener Parallelisierung MUSS eine konservative sequenzielle Ausführung möglich sein, sofern die Ressourcen- und Synchronisationsanforderungen erfüllt werden.

## 16. Verbindliche Anforderungen

1. NovaOS MUSS unabhängige Rendering-Aufgaben parallel vorbereiten können.
2. GPU-Parallelisierung SOLL bei entsprechender Hardwareunterstützung möglich sein.
3. Sämtliche Render-Pass-Abhängigkeiten MÜSSEN erhalten bleiben.
4. CPU Command Recording MUSS thread-sicher erfolgen.
5. GPU Queue Capabilities MÜSSEN berücksichtigt werden.
6. Asynchronous Compute DARF verwendet werden.
7. Erforderliche Synchronisationsoperationen MÜSSEN gewährleistet sein.
8. Ressourcenlebenszeiten MÜSSEN bei paralleler Ausführung berücksichtigt werden.
9. Resource Aliasing DARF keine konkurrierenden Speicherzugriffe verursachen.
10. Die Aufgabenverteilung SOLL verfügbare CPU- und GPU-Ressourcen berücksichtigen.
11. Parallelisierung SOLL anhand ihrer erwarteten Gesamtkosten bewertet werden.
12. Unnötige Synchronisationspunkte SOLLEN vermieden werden.
13. Unveränderte Aufgabenstrukturen SOLLEN wiederverwendet werden.
14. Ein deterministischer Ausführungsmodus MUSS unterstützt werden können.
15. Ungültige Rendering-Ergebnisse DÜRFEN nicht veröffentlicht werden.
16. Eine konservative sequenzielle Ausführung MUSS als Fallback verfügbar sein.

## 17. Ergebnis

Nova Parallel Render Execution ermöglicht die parallele Vorbereitung und Ausführung unabhängiger Rendering-Aufgaben auf mehreren CPU-Kernen und unterstützten GPU Queues.

Durch abängigkeitsbasierte Aufgabenverteilung, asynchrone Verarbeitung und kontrollierte Synchronisation kann NovaOS verfügbare Hardware effizient nutzen.

Damit reduziert das Subsystem die Rendering-Latenz und verbessert die Skalierbarkeit des Nova Render Graph, ohne die Korrektheit oder Ressourcenintegrität zu beeinträchtigen.