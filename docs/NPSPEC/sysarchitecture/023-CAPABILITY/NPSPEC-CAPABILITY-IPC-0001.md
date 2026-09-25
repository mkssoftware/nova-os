# NPSPEC-CAPABILITY-IPC-0001 – Nova Capability IPC

## Status

Angenommen

## Kategorie

Capability / IPC / Security

## Zweck

NovaOS definiert die sichere Nutzung, Übertragung und Delegation von Capabilities über Inter-Process Communication.

```text
Process A
   ↓
IPC Channel
   ↓
Capability Transfer
   ↓
Process B
```

IPC darf Capabilities ausschließlich explizit übertragen. Normale Nachrichtendaten dürfen keine implizite Autorität erzeugen.

## Grundprinzipien

```text
Message ≠ Capability
IPC Access ≠ Resource Authority
Handle Value ≠ Transferable Authority
Serialization ≠ Capability Transfer
Channel Access ≠ Unlimited Delegation
Sender Authority ≠ Receiver Authority
Capability Transfer ≠ Capability Copy
```

## IPC-Modell

Capability-bezogene IPC verwendet:

```text
CapabilityIPCTransfer
├── TransferID
├── Source CapabilityID
├── Sender Identity
├── Receiver Identity
├── IPC Channel
├── Effective Rights
├── Constraints
└── State
```

Optional:

```text
Derived CapabilityID
Purpose
Expiration
Delegation Depth
TransactionID
ExecutionContract
```

## Explizite Übertragung

Capabilities dürfen nicht durch das Kopieren eines Handle-Werts übertragen werden.

```text
Process A
Handle 42
   ↓
Kernel-mediated Transfer
   ↓
Process B
Handle 17
```

Beide Handle-Werte sind ausschließlich lokal gültig.

```text
Handle(A) ≠ Handle(B)
```

## Kernel-Vermittlung

Der Kernel kontrolliert Capability Transfers.

```text
Sender
  ↓
Transfer Request
  ↓
Source Validation
  ↓
Delegation Policy
  ↓
Attenuation
  ↓
Receiver Capability Table
```

Der Empfänger erhält nur die tatsächlich delegierte Autorität.

## Attenuation

IPC-Übertragung soll Capability Attenuation unterstützen.

```text
Sender:
Read + Write

     ↓ IPC

Receiver:
Read
```

Es gilt:

```text
Authority(Receiver Capability)
⊆ Authority(Sender Capability)
```

## Typed IPC

Capability-Übertragung soll Bestandteil typisierter IPC-Nachrichten sein können.

```text
Request {
    document: Capability<Document, Read>
    output:   Capability<File, Write>
}
```

Capability-Felder müssen vom IPC-System als Autoritätsobjekte und nicht als normale serialisierte Daten behandelt werden.

## Channel Authority

Die Capability für einen IPC Channel ist von den über ihn übertragenen Capabilities getrennt.

```text
Channel Capability
       ≠
Transferred Capability
```

Der Zugriff auf einen Channel erzeugt keine automatische Berechtigung für alle darüber erreichbaren Ressourcen.

## Request/Response

Temporäre Capabilities können an einzelne IPC-Aufrufe gebunden werden.

```text
Request
  ↓
Temporary Capability
  ↓
Operation
  ↓
Response
  ↓
Release / Expire
```

Dies reduziert dauerhaft gehaltene Autorität.

## Structured Concurrency

Capability-Lebenszeiten können an strukturierte IPC-Operationen gebunden werden.

```text
Task
 └── IPC Call
      └── Temporary Capability
```

Bei Cancellation oder Task-Ende können temporäre Delegationen automatisch freigegeben oder widerrufen werden.

## Revocation

Revocation muss über IPC übertragene Capabilities erfassen.

```text
Source Capability Revoked
       ↓
Delegation Dependency
       ↓
Receiver Capability
       ↓
Re-Evaluation
```

Bereits übertragene Handles dürfen Revocation nicht umgehen.

## Zero-Copy

Zero-Copy IPC darf Capability Isolation nicht schwächen.

```text
Shared Buffer
     ≠
Shared Capability Table
```

Die Freigabe eines Buffers muss unabhängig von anderen Capability-Rechten erfolgen.

## Fehlerverhalten

Fehler während eines Transfers dürfen keine teilweise gewährte Autorität hinterlassen.

```text
Prepare
  ↓
Validate
  ↓
Transfer
  ↓
Commit
```

Bei Fehlschlag:

```text
Rollback / No Authority Granted
```

Capability Transfers sollen daher mit transaktionalen IPC-Mechanismen integrierbar sein.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
TransferID
Sender
Receiver
Source CapabilityID
Derived CapabilityID
Effective Rights
Constraints
Channel
State
```

Capability Tokens dürfen nicht offengelegt werden.

## Normative Anforderungen

1. NovaOS MUSS Capability-Übertragung über IPC explizit behandeln.
2. Normale Nachrichtendaten DÜRFEN keine implizite Capability-Übertragung erzeugen.
3. Handle-Werte DÜRFEN NICHT direkt zwischen Capability Tables übernommen werden.
4. Capability Transfers MÜSSEN durch den Kernel oder einen äquivalent geschützten Mechanismus validiert werden.
5. Empfangene Autorität DARF die delegierte Quellautorität NICHT überschreiten.
6. Typed IPC SOLL Capability-Felder explizit kennzeichnen.
7. Channel Authority MUSS von transportierter Autorität getrennt bleiben.
8. Revocation MUSS auf über IPC delegierte Capabilities wirken können.
9. Zero-Copy IPC DARF Capability Isolation NICHT umgehen.
10. Fehlgeschlagene Transfers DÜRFEN keine teilweise gewährte Autorität hinterlassen.

## Abhängigkeiten

- `NPSPEC-CAPABILITY-IDENTITY-0001`
- `NPSPEC-CAPABILITY-HANDLE-0001`
- `NPSPEC-CAPABILITY-DELEGATION-0001`
- `NPSPEC-CAPABILITY-ATTENUATION-0001`
- `NPSPEC-CAPABILITY-REVOCATION-0001`
- `NPSPEC-CAPABILITY-ISOLATION-0001`
- `NPSPEC-IPC-CAPABILITY-0001`
- `NPSPEC-IPC-TYPED-0001`
- `NPSPEC-IPC-ZEROCOPY-0001`
- `NPSPEC-ARCH-STRUCTUREDCONCURRENCY-0001`
- `NPSPEC-ARCH-TRANSACTIONS-0001`
- `ADR-CAPABILITY-0012`

## Ergebnis

```text
Sender Capability
       ↓
Explicit IPC Transfer
       ↓
Validation + Attenuation
       ↓
Receiver-local Capability
       ↓
Receiver Handle
```

NovaOS erhält damit einen sicheren Capability-IPC-Mechanismus, bei dem Autorität explizit, attenuiert und nachvollziehbar zwischen isolierten Prozessen übertragen werden kann, ohne Handle-Namensräume, normale Nachrichtendaten oder Zero-Copy-Pfade als implizite Autoritätskanäle zu verwenden.