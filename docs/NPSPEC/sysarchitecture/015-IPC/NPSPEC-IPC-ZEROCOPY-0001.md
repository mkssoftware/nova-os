# NPSPEC-IPC-ZEROCOPY-0001 – Nova IPC Zero-Copy

## Status

Entwurf

## Kategorie

IPC / Data Movement / Zero-Copy

## Zweck

NovaOS definiert Zero-Copy-IPC für die effiziente Übertragung größerer Datenmengen zwischen Prozessen, Diensten und Treibern, ohne unnötige Kopien zwischen Adressräumen zu erzeugen.

```text
Process A
    ↓
Shared / Mapped Buffer
    ↓
Process B
```

Zero-Copy ist eine Optimierung des Datentransports und darf weder IPC-Semantik noch Sicherheitsgrenzen verändern.

## Grundprinzip

```text
Zero-Copy ≠ Zero Data Movement
Shared Buffer ≠ Shared Authority
Buffer Reference ≠ Buffer Access
Zero-Copy ≠ Mandatory
```

NovaOS muss jederzeit auf einen sicheren Copy-Pfad zurückfallen können.

## Zero-Copy Transfer

Große IPC-Payloads dürfen durch Referenzen auf kontrollierte Buffer ersetzt werden.

```text
IPC Message
├── Metadata
└── Buffer Reference
        ↓
   Shared Buffer
```

Die eigentlichen Daten müssen dadurch nicht Bestandteil der IPC-Nachricht sein.

## Buffer-Modell

Zero-Copy-IPC baut auf dem Nova Shared Buffer Model auf.

```text
SharedBuffer
├── BufferID
├── Size
├── Owner
├── Access Rights
├── Lifetime
└── Mapping State
```

Der Buffer bleibt ein eigenständiges Ressourcenobjekt.

## Zugriff

Der Empfänger benötigt explizite Berechtigung für den Buffer.

```text
Sender
   ↓
Buffer Capability
   ↓
IPC Transfer
   ↓
Receiver
```

Mögliche Rechte:

```text
Read
Write
ReadWrite
Transfer
Map
```

Eine Buffer-ID allein darf keinen Zugriff ermöglichen.

## Ownership

Ownership und Zugriffsrechte müssen getrennt behandelt werden.

```text
Owner
  ↓
Shared Access
  ├── Process B
  └── Process C
```

Das Teilen eines Buffers überträgt nicht automatisch dessen Ownership.

Ownership-Wechsel müssen explizit erfolgen.

## Lifetime

Der Buffer muss mindestens so lange gültig bleiben, wie autorisierte Nutzer darauf zugreifen können.

```text
Create
  ↓
Map / Share
  ↓
Use
  ↓
Release
  ↓
Destroy
```

NovaOS muss verhindern, dass ein Buffer freigegeben wird, während noch gültige Zugriffe bestehen.

## Mapping

Ein Buffer darf in unterschiedliche virtuelle Adressräume eingeblendet werden.

```text
Physical / Backing Memory
        ↓
     Buffer
      ↙   ↘
Process A Process B
```

Virtuelle Adressen dürfen zwischen Prozessen unterschiedlich sein.

IPC darf daher keine prozesslokalen Pointer als übertragbare Referenzen verwenden.

## Typed IPC

Zero-Copy-Payloads dürfen typisiert werden.

```text
Buffer<ImageData>
Buffer<AudioData>
Buffer<StorageBlock>
```

Der Typ beschreibt die Semantik der Daten, ersetzt aber keine Zugriffs- oder Lifetime-Prüfung.

## Schreibzugriffe

Bei gemeinsam beschreibbaren Buffern müssen Ownership und Synchronisation eindeutig definiert sein.

NovaOS soll bevorzugen:

```text
Single Writer
Multiple Readers
```

Komplexere Sharing-Modelle dürfen verwendet werden, wenn Synchronisation explizit definiert ist.

## Copy-Fallback

Zero-Copy darf nur verwendet werden, wenn es sicher und wirtschaftlich sinnvoll ist.

```text
IPC Transfer
   ↓
Can Zero-Copy?
   ├── Yes → Shared / Mapped Buffer
   └── No  → Copy
```

Ein Copy-Fallback kann erforderlich sein bei:

```text
Security Boundary
Incompatible Memory
Small Payload
Remote Endpoint
Alignment Constraint
Provider Limitation
```

## Location Transparency

Bei entfernten Endpoints kann echtes Shared-Memory-Zero-Copy unmöglich sein.

```text
Logical IPC
   ↓
Local  → Zero-Copy möglich
Remote → Transport / Copy
```

Die IPC-Semantik darf dadurch nicht verändert werden.

## Ressourcensteuerung

Zero-Copy-Buffer müssen in die Ressourcenökonomie integriert sein.

Begrenzbar sein sollen insbesondere:

```text
Pinned Memory
Mapped Memory
Buffer Count
Total Shared Memory
Lifetime
```

Damit darf Zero-Copy nicht zu unbegrenztem Speicherverbrauch führen.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
BufferID
Owner
Size
Mappings
Access Rights
Reference Count
Lifetime State
Transfer Mode
```

Buffer-Inhalte dürfen dabei nicht ohne entsprechende Berechtigung offengelegt werden.

## Normative Anforderungen

1. NovaOS SOLL Zero-Copy für IPC unterstützen.
2. Zero-Copy DARF NICHT Voraussetzung für korrekte IPC sein.
3. Ein sicherer Copy-Fallback MUSS verfügbar sein.
4. Buffer-Referenzen DÜRFEN NICHT automatisch Zugriff auf den Buffer gewähren.
5. Buffer-Zugriff MUSS explizit autorisiert werden.
6. Ownership und Zugriffsrechte MÜSSEN getrennt behandelt werden.
7. Prozesslokale Pointer DÜRFEN NICHT als IPC-Buffer-Referenzen verwendet werden.
8. Buffer-Lifetime MUSS aktive autorisierte Zugriffe berücksichtigen.
9. Schreibbare Shared Buffers MÜSSEN definierte Ownership- und Synchronisationsregeln besitzen.
10. Typed IPC DARF Buffer semantisch typisieren, MUSS aber normale Sicherheitsprüfungen beibehalten.
11. Zero-Copy-Ressourcen MÜSSEN begrenzbar und abrechenbar sein.
12. Location Transparency DARF NICHT voraussetzen, dass Zero-Copy zwischen allen Endpoints möglich ist.

## Abhängigkeiten

- `NPSPEC-IPC-0001`
- `NPSPEC-IPC-TYPED-0001`
- `NPSPEC-IPC-CAPABILITY-0001`
- `NPSPEC-DATAMOVE-ZEROCOPY-0001`
- `NPSPEC-DATAMOVE-SHAREDBUFFER-0001`
- `NPSPEC-MEMORY-MAPPING-0001`
- `NPSPEC-MEMORY-SHARED-0001`
- `NPSPEC-ARCH-ZEROCOPY-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `ADR-IPC-0004`

## Ergebnis

```text
IPC Message
    ↓
Buffer Reference
    ↓
Capability Check
    ↓
Shared Mapping
    ↓
Zero-Copy Data Access
```

NovaOS erhält damit einen sicheren Zero-Copy-IPC-Pfad für große Datenmengen, bei dem unnötige Kopien vermieden werden können, ohne Ownership, Speicherisolation, Capability-Grenzen oder IPC-Semantik aufzugeben.