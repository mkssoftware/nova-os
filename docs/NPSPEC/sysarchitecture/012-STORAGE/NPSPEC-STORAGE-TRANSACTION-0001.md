# NPSPEC-STORAGE-TRANSACTION-0001 – Nova Storage Transactions

## Status

Angenommen

## Kategorie

Storage / Transactions / Consistency / Recovery

## Zweck

NovaOS definiert Storage Transactions als Mechanismus, um zusammengehörige Änderungen kontrolliert und konsistent auszuführen.

```text
Begin
  ↓
Prepare Changes
  ↓
Validate
  ↓
Commit
  ↓
Verify
```

Unvollständige Operationen dürfen keinen undefinierten Storage-Zustand hinterlassen.

## Grundprinzip

```text
Operation Started ≠ Operation Committed
Commit ≠ Verified
Transaction ≠ Global ACID
```

Transaktionen sollen nur den tatsächlich benötigten Konsistenzbereich umfassen.

## Transaction-Modell

Eine Storage Transaction besitzt mindestens:

```text
TransactionID
State
Operations
Affected Objects
```

Mögliche Zustände:

```text
Created
Preparing
Prepared
Committing
Committed
Aborted
Failed
```

## Operationen

Eine Transaktion darf mehrere Storage-Operationen zusammenfassen.

Beispiel:

```text
Create Object
Update Metadata
Update Directory
Update Relationships
Commit
```

Die Änderungen werden logisch als zusammengehörige Operation behandelt.

## Commit

Ein erfolgreicher Commit bedeutet, dass der definierte Storage-Zustand dauerhaft übernommen wurde.

```text
Prepared State
     ↓
Commit
     ↓
Committed State
```

Teilweise sichtbare Zustände sollen bei atomar definierten Operationen verhindert werden.

## Abort

Vor dem erfolgreichen Commit muss eine Transaktion abgebrochen werden können.

```text
Transaction
    ↓
Failure / Cancel
    ↓
Abort
    ↓
Previous Consistent State
```

Bereits reservierte Ressourcen müssen anschließend kontrolliert freigegeben werden.

## Crash Recovery

Nach einem Absturz muss NovaOS feststellen können, ob eine Transaktion:

```text
Committed
Not Committed
Unknown
```

ist.

```text
Unknown ≠ Committed
```

Recovery muss einen konsistenten Zustand herstellen, bevor betroffene Daten regulär verwendet werden.

## Object Model

Transaktionen dürfen Änderungen an mehreren Bestandteilen eines Storage-Objekts zusammenfassen.

```text
Object
├── Payload
├── Metadata
├── Version
└── Relationships
```

Dadurch können beispielsweise Payload und Metadaten konsistent aktualisiert werden.

## Content Addressing

Neue Inhalte dürfen während der Vorbereitung bereits als Content gespeichert werden.

```text
New Payload
    ↓
ContentID
    ↓
Prepare
    ↓
Commit Object Reference
```

Nicht referenzierte vorbereitete Inhalte dürfen später reclaimt werden.

## Isolation

Parallele Transaktionen dürfen keine unzulässigen Zwischenzustände voneinander beobachten.

Die konkrete Isolation darf abhängig vom Storage-Provider unterschiedlich umgesetzt werden.

## Legacy-Dateisysteme

Nicht jedes Dateisystem bietet native Transaktionen.

Das VFS darf deshalb:

```text
Native Transaction
Journal-assisted Operation
Emulated Transaction
Limited Transaction
```

verwenden.

Nicht verfügbare Garantien müssen explizit erkennbar sein.

## Scope

NovaOS unterscheidet Transaktionen nach ihrem Gültigkeitsbereich.

```text
Object
Directory
Filesystem
Volume
Multi-Object
```

Verteilte oder volumeübergreifende Transaktionen sind nicht automatisch Bestandteil dieses Standards.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
TransactionID
State
Scope
Affected Objects
Commit Result
Failure Reason
Recovery State
```

## Normative Anforderungen

1. NovaOS MUSS transaktionale Storage-Operationen unterstützen können.
2. Jede Transaktion MUSS einen eindeutig bestimmbaren Zustand besitzen.
3. `Started` DARF NICHT als `Committed` behandelt werden.
4. Ein Commit MUSS die definierten Änderungen konsistent übernehmen.
5. Vor dem Commit MUSS eine Transaktion kontrolliert abbrechbar sein.
6. Nach einem Crash MUSS der Commit-Zustand rekonstruierbar oder als unbekannt erkennbar sein.
7. `Unknown` DARF NICHT automatisch als erfolgreicher Commit behandelt werden.
8. Transaktionen SOLLEN nur den notwendigen Konsistenzbereich umfassen.
9. Parallelität DARF keine unzulässigen Zwischenzustände sichtbar machen.
10. Nicht unterstützte Transaktionsgarantien MÜSSEN explizit erkennbar sein.
11. Legacy-Dateisysteme DÜRFEN eingeschränkte oder emulierte Transaktionen bereitstellen.
12. Transaktionsstatus und Recovery-Entscheidungen SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-STORAGE-VFS-0001`
- `NPSPEC-STORAGE-OBJECT-0001`
- `NPSPEC-STORAGE-CONTENTADDRESS-0001`
- `NPSPEC-ARCH-TRANSACTIONS-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`
- `ADR-STORAGE-0004`

## Ergebnis

```text
Storage Changes
      ↓
Transaction
      ↓
Prepare → Validate → Commit
                    ↘ Abort
      ↓
Consistent Storage State
```

NovaOS erhält damit eine einheitliche Grundlage für konsistente, überprüfbare und crash-resistente Storage-Änderungen, ohne globale Transaktionen für jede Operation vorauszusetzen.