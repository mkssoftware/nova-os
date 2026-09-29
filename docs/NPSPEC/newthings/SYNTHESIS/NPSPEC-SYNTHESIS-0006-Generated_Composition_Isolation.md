# NPSPEC-SYNTHESIS-0006 – Generated Composition Isolation

## Status

Angenommen

## Zweck

Diese Spezifikation definiert die Isolation automatisch erzeugter Capability-Kompositionen.

Ziel ist, dass eine durch `Nova.Synthesis` erzeugte Pipeline nur auf Ressourcen, Daten und Rechte zugreifen kann, die für den zugrunde liegenden Intent erforderlich und erlaubt sind.

## Grundprinzip

```text
Intent
    ↓
Synthesized Composition
    ↓
Isolation Boundary
    ↓
Capabilities
    ↓
kontrollierter Ressourcenzugriff
```

Eine automatisch erzeugte Komposition erhält keine zusätzlichen Rechte allein dadurch, dass ihre einzelnen Capabilities verfügbar sind.

## Isolation Boundary

Jede synthetisierte Pipeline besitzt einen definierten Sicherheitskontext.

Dieser kann mindestens umfassen:

```text
identity
permissions
policies
data_access
device_access
network_access
resource_limits
```

Die Pipeline darf diese Grenzen nicht selbst erweitern.

## Least Privilege

Jede Capability erhält nur die Rechte, die sie für ihre konkrete Aufgabe benötigt.

Beispiel:

```text
Audio Decoder:
    read Media.Audio

Speech Recognizer:
    read decoded audio

Document Writer:
    write target document
```

Der Speech Recognizer benötigt dadurch keinen allgemeinen Dateisystemzugriff.

## Capability Isolation

Capabilities innerhalb derselben Pipeline dürfen voneinander isoliert werden.

Kommunikation erfolgt über definierte:

```text
Graph Edges
IPC Channels
Shared Objects
Data Streams
```

Eine Capability darf nicht automatisch:

- Speicher anderer Capabilities lesen
- fremde Handles verwenden
- andere Nodes manipulieren
- zusätzliche Ressourcen öffnen

## Datenfluss

Daten dürfen nur entlang zulässiger Verbindungen übertragen werden.

Beispiel:

```text
Node A
    ↓ Media.Audio
Node B
```

Zusätzliche Einschränkungen können durch `Nova.InformationFlow` erzwungen werden.

## Seiteneffekte

Seiteneffekte benötigen explizite Berechtigungen.

Beispiele:

```text
filesystem_write
network_access
device_control
external_publish
```

Eine reine Compute-Capability erhält diese Rechte nicht automatisch.

## Ressourcenisolation

Eine Pipeline muss hinsichtlich Ressourcen begrenzbar sein.

Beispiele:

```text
memory
CPU time
GPU time
storage
bandwidth
open_handles
device_usage
```

Die Grenzen können aus Intent Constraints und Execution Contracts abgeleitet werden.

## Fehlerisolation

Ein Fehler innerhalb einer Capability soll nicht automatisch andere unabhängige Teile der Pipeline beschädigen.

Beispiel:

```text
Node B crashes
    ↓
isolate failure
    ↓
retry / replace / replan
```

NovaOS darf eine fehlerhafte Capability ersetzen, sofern der Composition Graph dadurch semantisch gültig bleibt.

## Trust-Grenzen

Capabilities unterschiedlicher Trust-Level müssen voneinander isolierbar sein.

Beispiel:

```text
System Capability
    ↓ controlled interface
Third-Party Capability
```

Eine weniger vertrauenswürdige Capability darf keine Rechte einer höher privilegierten Capability übernehmen.

## Temporäre Rechte

Für eine Pipeline gewährte Rechte sollen an deren Lebensdauer gebunden sein.

```text
Composition starts
    ↓
temporary capability grants
    ↓
Composition ends
    ↓
grants revoked
```

Persistente Berechtigungen benötigen eine separate Autorisierung.

## Child Processes und Sub-Capabilities

Eine Capability darf ihre Rechte nicht automatisch vollständig an erzeugte Prozesse oder Sub-Capabilities weitergeben.

Delegation muss explizit und begrenzt erfolgen.

## Isolation bei Replanning

Wird eine Capability ersetzt, muss der Sicherheitskontext neu geprüft werden.

Beispiel:

```text
Local Capability
    ↓ unavailable
Remote Capability
```

Die Remote-Capability darf nicht verwendet werden, wenn beispielsweise:

```text
remote_execution: denied
```

gilt.

## Beispiel

Pipeline:

```text
Media.Audio
    ↓
Decoder
    ↓
SpeechRecognizer
    ↓
DocumentWriter
```

Isolation:

```text
Decoder:
    read source

SpeechRecognizer:
    read decoded stream
    no filesystem
    no network

DocumentWriter:
    write target
```

Damit erhält jede Capability nur die Rechte, die für ihren Teil des Graphen notwendig sind.

## Normative Anforderungen

1. Jede synthetisierte Pipeline MUSS einen definierbaren Sicherheitskontext besitzen.
2. Capabilities SOLLEN nach dem Least-Privilege-Prinzip ausgeführt werden.
3. Datenflüsse MÜSSEN auf zulässige Graphverbindungen begrenzbar sein.
4. Seiteneffekte MÜSSEN explizit autorisiert sein.
5. Ressourcenverbrauch MUSS pro Pipeline oder Capability begrenzbar sein.
6. Unterschiedliche Trust-Level MÜSSEN isolierbar sein.
7. Temporäre Rechte SOLLEN nach Abschluss der Pipeline entzogen werden.
8. Replanning MUSS eine erneute Prüfung der Isolation und Berechtigungen auslösen.

## Abgrenzung

Diese NPSPEC definiert:

- Isolation synthetisierter Pipelines
- Least Privilege
- Daten- und Ressourcenisolation
- Trust-Grenzen
- temporäre Rechte

Nicht Bestandteil sind:

- allgemeines Prozess-Sandboxing
- Capability Composition Planning
- Information-Flow-Regeln im Detail
- Berechtigungsverwaltung
- Scheduling

## Zugehörige NPSPECs

- `NPSPEC-SYNTHESIS-0001 – Capability Semantic Descriptor`
- `NPSPEC-SYNTHESIS-0002 – Capability Composition Graph`
- `NPSPEC-SYNTHESIS-0003 – Synthesis Planning`
- `NPSPEC-SYNTHESIS-0004 – Constraint-Aware Composition`
- `NPSPEC-SYNTHESIS-0005 – Synthesized Pipeline Verification`
- `NPSPEC-SYNTHESIS-0007 – Composition Cache & Reuse`
- `NPSPEC-INFOFLOW-0001 – Information Labels`
- `NPSPEC-INTENT-0005 – Intent Constraints & Policies`