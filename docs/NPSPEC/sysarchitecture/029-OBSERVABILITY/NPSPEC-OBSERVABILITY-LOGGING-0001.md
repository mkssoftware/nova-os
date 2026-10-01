# NPSPEC-OBSERVABILITY-LOGGING-0001 – Nova Observability Logging

## Status

Angenommen

## Kategorie

Observability / Logging / Diagnostics

## Zweck

NovaOS definiert ein systemweites strukturiertes Logging-Modell für Kernel, Treiber, Dienste, Anwendungen und verteilte Komponenten.

Logs sollen Fehleranalyse, Debugging, Audit-Korrelation und Systemdiagnose ermöglichen, ohne Logging mit Tracing, Auditing oder Telemetrie gleichzusetzen.

```text
System Event
    ↓
Structured Log Event
    ↓
Logging Pipeline
    ↓
Buffer / Storage / Consumer
```

## Grundprinzipien

```text
Log ≠ Trace
Log ≠ Audit
Log ≠ Metric
Log ≠ Telemetry

Logging ≠ Authority
Logging Failure ≠ System Failure
Sensitive Data ≠ Default Log Data
```

Logging darf kritische Ausführungspfade nicht unnötig blockieren.

## Log Event Model

Ein Log-Eintrag wird strukturiert beschrieben:

```text
LogEvent
├── EventID
├── Timestamp
├── Severity
├── ComponentID
├── EventType
└── Message
```

Optional:

```text
ExecutionID
TraceID
TransactionID
TaskID
ProcessID
ObjectID
ProviderID
NodeID
ErrorCode
Structured Fields
Security Label
```

## Severity

NovaOS definiert mindestens:

```text
TRACE
DEBUG
INFO
NOTICE
WARNING
ERROR
CRITICAL
PANIC
```

Severity beschreibt die Bedeutung eines Ereignisses und nicht automatisch die gewünschte Reaktion.

## Structured Logging

Logs sollen primär maschinenlesbar strukturiert sein.

```text
EventType = Storage.ReadFailure
ObjectID  = ...
ErrorCode = ...
DeviceID  = ...
```

Freitext kann ergänzend verwendet werden.

Die Bedeutung kritischer Felder darf nicht ausschließlich aus Textanalyse abgeleitet werden müssen.

## Logging Pipeline

```text
Producer
   ↓
Local Log Buffer
   ↓
Filtering
   ↓
Routing
   ↓
Authorized Consumer / Storage
```

Producer sollen nicht von einem einzelnen zentralen Logging-Dienst abhängig sein.

## Kernel Logging

Der Kernel benötigt einen minimalen Logging-Pfad, der bereits während früher Bootphasen funktioniert.

```text
Early Boot
   ↓
Reserved Log Buffer
   ↓
Normal Logging Infrastructure
```

Frühe Einträge sollen nach Initialisierung der regulären Infrastruktur übernommen werden können.

## Ring Buffer

Für laufende Systemlogs können begrenzte Ringpuffer verwendet werden.

```text
Newest Events
     ↓
Ring Buffer
     ↓
Oldest Events overwritten
```

Speicherverbrauch muss begrenzt bleiben.

## Backpressure

Logging darf bei hoher Ereignisrate keine unkontrollierten Ressourcen verbrauchen.

Mögliche Maßnahmen:

```text
Bounded Buffers
Rate Limiting
Sampling
Aggregation
Severity Filtering
Controlled Dropping
```

Kritische Ereignisse sollen gegenüber Debug-Informationen bevorzugt werden können.

## Failure Isolation

```text
Logging Backend Failure
        ≠
Application Failure
```

Ein defekter Log-Consumer darf den Kernel oder andere unabhängige Komponenten nicht blockieren.

Für sicherheitskritische Audit-Ereignisse können strengere Regeln gelten.

## Sensitive Data

Standardmäßig dürfen Logs keine Geheimnisse enthalten.

Insbesondere:

```text
Passwords
Private Keys
Capability Tokens
Session Secrets
Authentication Secrets
Raw Credentials
Encryption Keys
```

dürfen nicht protokolliert werden.

```text
CapabilityID ≠ Capability Secret
```

Sichere Referenzen können verwendet werden, wenn dies die Policy erlaubt.

## Privacy

Personenbezogene oder anderweitig sensible Daten müssen:

```text
Minimized
Labeled
Protected
Retained by Policy
```

werden.

Logging darf keine alternative Datenablage zur Umgehung von Privacy- oder Retention-Regeln darstellen.

## Security

Lesen, Exportieren, Löschen und Konfigurieren von Logs benötigt passende Capabilities.

```text
LogRead
LogConfigure
LogExport
LogClear
```

Diese Rechte müssen getrennt delegierbar sein.

## Integrity

Für relevante Logs können Integritätsschutzmechanismen eingesetzt werden.

Beispiele:

```text
Checksums
Authenticated Records
Signed Segments
Append Protection
Sequence Numbers
```

Normales Debug Logging benötigt nicht automatisch dieselben Garantien wie Security Audit Logging.

## Correlation

Logs können mit anderen Observability-Daten korreliert werden.

```text
ExecutionID
TraceID
TransactionID
ObjectID
NodeID
```

Dadurch kann beispielsweise ein Fehler über:

```text
Execution
→ IPC
→ Provider
→ Storage
```

nachvollzogen werden.

## Distributed Logging

Verteilte Systeme dürfen lokale Logs erzeugen und später zusammenführen.

```text
Node A ─┐
Node B ─┼→ Correlated Observation
Node C ─┘
```

Eine zentrale Logging-Infrastruktur ist keine Voraussetzung für lokale Funktionsfähigkeit.

Zeitabweichungen zwischen Nodes müssen berücksichtigt werden.

## Determinismus

Deterministische Ausführungen können relevante Logging-Ereignisse mit:

```text
ExecutionID
Event Sequence
Deterministic Timestamp / Logical Time
```

korrelieren.

Logging selbst darf deterministische Ausführung nicht unnötig verändern.

## Crash und Panic

Bei schweren Fehlern muss ein minimaler Logging-Pfad verfügbar bleiben.

```text
Failure
  ↓
Reserved Buffer
  ↓
Panic Reporter
  ↓
Crash Dump
```

Der Panic-Pfad darf nicht von komplexen Logging-Diensten abhängig sein.

## Retention

Logs benötigen explizite Aufbewahrungsregeln.

```text
Log Class
   ↓
Retention Policy
   ↓
Expire / Rotate / Preserve
```

Unbegrenzte Log-Aufbewahrung ist zu vermeiden.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
Active Log Sources
Buffer Usage
Dropped Events
Severity Distribution
Logging Health
Storage Usage
Retention State
Filtering Rules
Consumer State
```

## Normative Anforderungen

1. NovaOS MUSS ein strukturiertes systemweites Logging-Modell bereitstellen.
2. Logging MUSS von Tracing, Auditing, Metrics und Telemetry getrennt bleiben.
3. Log Events MÜSSEN stabile Event- und Component-Identifikatoren unterstützen.
4. Kernel Logging MUSS bereits während früher Bootphasen möglich sein.
5. Logging-Puffer MÜSSEN begrenzbar sein.
6. Logging MUSS Backpressure und kontrolliertes Dropping unterstützen können.
7. Logging-Fehler DÜRFEN unabhängige Systemkomponenten NICHT unnötig blockieren.
8. Secrets, Schlüssel und übertragbare Capability Tokens DÜRFEN NICHT protokolliert werden.
9. Privacy- und Retention-Policies MÜSSEN auf Logdaten anwendbar sein.
10. Zugriff auf Logs MUSS capabilitybasiert kontrollierbar sein.
11. Kritische Logs SOLLEN Integritätsschutz unterstützen.
12. Logs MÜSSEN über ExecutionID, TraceID und andere stabile IDs korrelierbar sein können.
13. Distributed Logging DARF keine permanente zentrale Infrastruktur voraussetzen.
14. Panic Logging MUSS einen minimalen unabhängigen Pfad besitzen.
15. Logging MUSS Rotation und Retention unterstützen.
16. Logging-Zustand MUSS autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-ARCH-INTROSPECTION-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `NPSPEC-EXECUTION-CONTRACT-0001`
- `NPSPEC-DISTRIBUTED-EXECUTION-0001`
- `NPSPEC-DISTRIBUTED-IPC-0001`
- `NPSPEC-DISTCOMM-TRACE-0001`
- `NPSPEC-SECURITY-AUDIT-0001`
- `NPSPEC-SECURITY-LABEL-0001`
- `NPSPEC-PRIVACY-MINIMIZATION-0001`
- `NPSPEC-PRIVACY-RETENTION-0001`
- `NPSPEC-PRIVACY-EXPIRATION-0001`
- `NPSPEC-CAPABILITY-IDENTITY-0001`
- `ADR-ARCH-0070`

## Ergebnis

```text
System Events
      ↓
Structured Logging
      ↓
Bounded Buffers
      ↓
Filtering + Protection
      ↓
Correlation
      ↓
Diagnostics / Observation
```

NovaOS erhält damit eine strukturierte, ressourcenbegrenzte und sicherheitsorientierte Logging-Grundlage, die vom frühen Kernelstart bis zu verteilten Ausführungen einheitlich funktioniert und gleichzeitig Secrets, Privacy, Isolation und Systemstabilität schützt.