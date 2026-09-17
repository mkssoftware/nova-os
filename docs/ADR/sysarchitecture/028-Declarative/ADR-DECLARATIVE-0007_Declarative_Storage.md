# ADR-DECLARATIVE-0007 – Declarative Storage

## Status

Angenommen

## Kategorie

Declarative Architecture / Storage

## Kontext

Speicher wird traditionell häufig durch imperative Einzeloperationen verwaltet:

```text
Create Volume
Format Filesystem
Mount Volume
Enable Encryption
Create Snapshot
```

NovaOS soll stattdessen beschreiben, **welcher Speicherzustand gelten soll**, ohne die dafür notwendigen Einzelschritte vorzugeben.

## Entscheidung

NovaOS unterstützt **Declarative Storage**.

Speicherressourcen werden über einen Desired Storage State beschrieben.

```text
Desired Storage State
        ↓
Storage Reconciliation
        ↓
Actual Storage State
        ↓
Verification
```

## Desired Storage State

Deklarative Storage-Definitionen können beispielsweise enthalten:

```text
Capacity
Availability
Encryption
Redundancy
Integrity
Versioning
Retention
Performance
Locality
```

Die Definition beschreibt Anforderungen und Zielzustand, nicht die konkrete Storage-Implementierung.

## Actual Storage State

NovaOS beobachtet den tatsächlich vorhandenen Speicherzustand.

```text
Desired:
    Encryption = Required

Actual:
    Encryption = Disabled
```

Abweichungen werden dem Reconciliation-System bereitgestellt.

## Storage Resources

Deklarativ verwaltbare Ressourcen können beispielsweise sein:

```text
Volume
Object Store
Filesystem
Snapshot
Storage Pool
Replica
```

Sie werden über stabile Objektidentitäten referenziert.

## Reconciliation

```text
Observe Storage
      ↓
Compare
      ↓
Plan
      ↓
Execute
      ↓
Verify
```

Der Reconciler berücksichtigt dabei Storage-, Security-, Resource- und Recovery-Contracts.

## Provider-Unabhängigkeit

Der Desired State soll keine konkrete Storage-Technologie erzwingen, sofern dies nicht ausdrücklich erforderlich ist.

```text
Desired:
    Encrypted
    Redundant
    Persistent

        ↓

Suitable Storage Provider
```

NovaOS kann einen geeigneten Provider anhand der geltenden Contracts bestimmen.

## Transaktionen

Änderungen am Storage State müssen soweit erforderlich transaktional erfolgen.

```text
Current State
      ↓
Prepare
      ↓
Commit
      ↓
Verify
```

Teilweise angewendete Storage-Konfigurationen dürfen keinen undefinierten Zustand erzeugen.

## Security

Storage Policies können Sicherheitsanforderungen enthalten.

```text
Encryption = Required
Integrity = Required
Location = LocalOnly
```

Security- und Data-Sovereignty-Constraints sind harte Grenzen für die Reconciliation.

## Recovery

Snapshots, Versionen und andere Recovery-Zustände können Bestandteil des Desired Storage State sein.

```text
Desired:
    Snapshots = Enabled
    Retention = Defined
    Recovery State = Available
```

Recovery-Anforderungen müssen mit den geltenden RTO/RPO-Contracts vereinbar sein.

## Normative Anforderungen

1. NovaOS MUSS Storage-Zustände deklarativ beschreiben können.
2. Desired Storage State und Actual Storage State MÜSSEN getrennt behandelt werden.
3. Storage-Ressourcen MÜSSEN durch State Reconciliation verwaltbar sein.
4. Storage Definitions SOLLEN von konkreten Providern unabhängig bleiben.
5. Security- und Data-Sovereignty-Constraints MÜSSEN berücksichtigt werden.
6. Kritische Storage-Änderungen MÜSSEN transaktional ausführbar sein.
7. Storage-Reconciliation DARF keinen undefinierten Teilzustand hinterlassen.
8. Recovery-, Snapshot- und Retention-Anforderungen MÜSSEN deklarativ beschreibbar sein.
9. Ein Desired Storage State DARF erst nach Verification als erreicht gelten.

## Konsequenzen

### Positive Konsequenzen

- Storage wird über Anforderungen statt Befehlsfolgen verwaltet,
- Storage Provider bleiben austauschbar,
- Security und Recovery werden Bestandteil des gewünschten Zustands,
- Storage Drift kann automatisch erkannt und korrigiert werden.

### Negative Konsequenzen

- Storage-Zustände müssen zuverlässig beobachtbar sein,
- komplexe Storage-Änderungen benötigen transaktionale Reconciliation.

## Abhängigkeiten

- `ADR-DECLARATIVE-0001_Desired_State_und_Actual_State`
- `ADR-DECLARATIVE-0002_State_Reconciliation`
- `ADR-DECLARATIVE-0003_Declarative_Resources`
- `ADR-DECLARATIVE-0005_Declarative_Security`
- `ADR-STORAGE-0004_Transactional_Storage`
- `ADR-STORAGE-0005_Versioned_Storage`
- `ADR-STORAGE-0014_Snapshots_und_Rollback`
- `ADR-STORAGE-0016_Encryption_als_Storage_Policy`
- `ADR-RESILIENCE-0017_RTO_und_RPO_als_Recovery_Contracts`

## Zugehörige NPSPECs

- `NPSPEC-DECLARATIVE-STORAGE-0001`
- `NPSPEC-DECLARATIVE-STORAGE-STATE-0001`
- `NPSPEC-DECLARATIVE-STORAGE-RECONCILIATION-0001`

## Ergebnis

NovaOS verwaltet Speicher über seinen gewünschten Zustand:

```text
Desired Storage State
        ↓
Observe
        ↓
Reconcile
        ↓
Transactional Change
        ↓
Verify
        ↓
Actual Storage State
```

Der zentrale Grundsatz lautet:

```text
Nicht:
Richte diesen Speicher so ein.

Sondern:
Diese Eigenschaften
muss der Speicher erfüllen.
```