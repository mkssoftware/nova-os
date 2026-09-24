# NPSPEC-IPC-CHANNEL-0001 – Nova IPC Channels

## Status

Entwurf

## Kategorie

IPC / Channel / Communication

## Zweck

NovaOS definiert IPC Channels als kontrollierte Kommunikationsverbindungen zwischen Endpoints.

```text
Endpoint A
    ↓
 Channel
    ↓
Endpoint B
```

Channels bündeln Kommunikationszustand, Berechtigungen, Queues und Lifecycle zu einer gemeinsamen IPC-Verbindung.

## Grundprinzip

```text
Channel ≠ Endpoint
Channel ≠ Shared Memory
Channel Identity ≠ Permission
Connected ≠ Authorized for Everything
```

Ein Channel verbindet Endpoints, erweitert jedoch nicht automatisch deren Berechtigungen.

## Channel-Modell

Ein Channel wird als eigenständiges IPC-Objekt behandelt.

```text
IPCChannel
├── ChannelID
├── Endpoint A
├── Endpoint B
├── Interface
├── State
└── Security Context
```

Optional:

```text
Queue Limits
QoS
Priority
Deadline
Buffer Configuration
```

## Channel-Arten

NovaOS soll verschiedene Kommunikationsrichtungen unterstützen.

```text
Unidirectional
Bidirectional
```

Ein bidirektionaler Channel kann logisch aus zwei unabhängigen Richtungen bestehen:

```text
A → B
A ← B
```

Dadurch können Queueing, Flow Control und Berechtigungen je Richtung getrennt verwaltet werden.

## Lifecycle

```text
Created
   ↓
Connecting
   ↓
Active
   ↓
Closing
   ↓
Closed
```

Fehlerzustände müssen zusätzlich eindeutig darstellbar sein.

Ein Channel darf erst `Active` werden, wenn die erforderlichen Endpoint-, Interface- und Capability-Prüfungen erfolgreich waren.

## Nachrichten

Channels transportieren IPC-Nachrichten über definierte Queues.

```text
Sender
  ↓
Send Queue
  ↓
Channel
  ↓
Receive Queue
  ↓
Receiver
```

Nachrichten müssen eindeutig einem Channel und Kommunikationskontext zugeordnet werden können.

## Typed IPC

Ein Channel darf an ein typisiertes Interface gebunden werden.

```text
Channel
   ↓
InterfaceID + Version
   ↓
Typed Messages
```

Nachrichten, die nicht mit dem ausgehandelten Interface kompatibel sind, müssen abgewiesen werden können.

## Capabilities

Die Erstellung oder Verwendung eines Channels muss durch passende IPC-Capabilities autorisiert werden.

```text
Caller
  ↓
IPC Capability
  ↓
Open Channel
  ↓
Endpoint
```

Ein bestehender Channel darf keine zusätzlichen Rechte erzeugen, die nicht durch seine Autorisierung gedeckt sind.

## Shared Memory und Zero-Copy

Channels dürfen Shared-Memory- oder Shared-Buffer-Referenzen transportieren.

```text
Channel Message
      ↓
Buffer Reference
      ↓
Shared Memory
```

Der Channel selbst gewährt keinen Zugriff auf den referenzierten Speicher.

Die jeweilige Buffer- oder Shared-Memory-Capability muss separat gültig sein.

## Backpressure

Channel-Queues müssen begrenzt sein.

```text
Producer
   ↓
Bounded Queue
   ↓
Consumer
```

Bei Überlastung müssen definierte Mechanismen möglich sein:

```text
Wait
Throttle
Reject
Drop
```

`Drop` darf nur verwendet werden, wenn die Channel-Semantik dies ausdrücklich erlaubt.

## Flow Control

Bidirektionale Channels sollen Flow Control pro Richtung unterstützen.

```text
A → B : Available Capacity
A ← B : Independent Capacity
```

Ein langsamer Empfänger darf nicht automatisch unbegrenztes Queue-Wachstum verursachen.

## Schließen

Ein Channel muss kontrolliert geschlossen werden können.

```text
Active
  ↓
Close Requested
  ↓
Drain / Cancel
  ↓
Closed
```

Abhängig von der Channel-Semantik dürfen ausstehende Nachrichten abgeschlossen, verworfen oder als Fehler zurückgegeben werden.

## Endpoint-Verlust

Wenn ein Endpoint verschwindet:

```text
Endpoint Failure
      ↓
Channel Broken
      ↓
Pending Operations
      ↓
Failure / Cancellation
```

Andere Teilnehmer dürfen nicht unbegrenzt auf den ausgefallenen Endpoint warten.

## Ressourcensteuerung

Channels müssen in die NovaOS-Ressourcenökonomie integriert sein.

Begrenzbar sein sollen insbesondere:

```text
Channel Count
Queue Depth
Message Size
Buffered Bytes
Shared Buffers
Bandwidth
```

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
ChannelID
Endpoints
Interface
State
Queue Depth
Pending Messages
Transferred Bytes
Errors
Latency
```

Nachrichteninhalte dürfen ohne entsprechende Berechtigung nicht offengelegt werden.

## Normative Anforderungen

1. NovaOS MUSS IPC Channels als kontrollierte Kommunikationsobjekte unterstützen.
2. Channels MÜSSEN eindeutig identifizierbar sein.
3. Channel-Identität und Zugriffsberechtigung MÜSSEN getrennt bleiben.
4. Channels DÜRFEN bidirektionale Kommunikation unterstützen.
5. Ein Channel DARF erst nach erfolgreicher Autorisierung aktiviert werden.
6. Channel-Queues MÜSSEN begrenzbar sein.
7. Channels MÜSSEN Backpressure unterstützen können.
8. Bidirektionale Channels SOLLEN Flow Control pro Richtung ermöglichen.
9. Typed Channels MÜSSEN Interface- und Nachrichtenkompatibilität prüfen können.
10. Shared-Memory-Referenzen DÜRFEN durch einen Channel keine impliziten Speicherrechte erhalten.
11. Endpoint-Verlust MUSS für den Kommunikationspartner erkennbar sein.
12. Channel-Zustand und Ressourcenverbrauch SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-IPC-0001`
- `NPSPEC-IPC-TYPED-0001`
- `NPSPEC-IPC-CAPABILITY-0001`
- `NPSPEC-IPC-ZEROCOPY-0001`
- `NPSPEC-IPC-SHAREDMEMORY-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `ADR-IPC-0006`

## Ergebnis

```text
Endpoint A
    ↓
Capability
    ↓
IPC Channel
    ↓
Typed / Bounded Communication
    ↓
Endpoint B
```

NovaOS erhält damit einen einheitlichen Channel-Mechanismus für zustandsbehaftete, kontrollierte und effiziente IPC-Verbindungen mit klaren Sicherheits-, Queue- und Lifecycle-Grenzen.