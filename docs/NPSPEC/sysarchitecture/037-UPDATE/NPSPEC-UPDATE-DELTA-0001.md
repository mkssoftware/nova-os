# NPSPEC-UPDATE-DELTA-0001 – Nova Delta Update

## Status

Angenommen

## Kategorie

Update / Delta / Distribution Optimization

## Zweck

NovaOS definiert Delta Updates zur Übertragung ausschließlich der Unterschiede zwischen einem bekannten Ausgangszustand und einer Zielversion.

```text
Base Version
     +
Delta
     ↓
Target Version
```

Ziel ist die Reduzierung von Downloadgröße, Bandbreite, Updatezeit und Repository-Speicher, ohne Integrität, Verifikation oder Rollback-Sicherheit zu schwächen.

## Grundprinzipien

```text
Delta ≠ Full Package
Smaller ≠ Safer
Base Version Match ≠ Base Content Match
Delta Applied ≠ Target Valid
Patch Success ≠ Update Success
Delta ≠ In-Place Modification Requirement
Optimization ≠ Security Exception
```

## Delta Model

```text
DeltaPackage
├── DeltaID
├── PackageID
├── BaseVersion
├── BaseContentID
├── TargetVersion
├── TargetContentID
├── DeltaPayload
└── Integrity
```

Optional:

```text
Algorithm
Dependencies
RequiredStorage
VerificationPlan
FallbackPackage
ProvenanceID
```

## Base Identification

Ein Delta darf nur auf exakt geeignete Ausgangsdaten angewendet werden.

```text
Expected BaseContentID
        =
Actual BaseContentID?
```

Versionsnummern allein reichen nicht aus.

```text
Same Version ≠ Same Content
```

Bei Abweichung muss das Delta verworfen werden.

## Delta-Erzeugung

```text
Base Artifact
      +
Target Artifact
      ↓
Delta Generator
      ↓
Delta Payload
```

Das erzeugte Delta muss eindeutig an Base- und Target-ContentID gebunden sein.

## Delta-Anwendung

```text
Validate Base
     ↓
Validate Delta
     ↓
Apply to Staging
     ↓
Generate Target
     ↓
Verify TargetContentID
```

Das aktive Artefakt soll nicht direkt verändert werden.

## Target Verification

Nach Rekonstruktion muss das Ergebnis unabhängig vom Delta verifiziert werden.

```text
Generated Target
      ↓
Hash
      ↓
Expected TargetContentID
```

Nur bei Übereinstimmung darf das erzeugte Artefakt in den weiteren Update-Prozess übernommen werden.

## Signaturen

Delta-Pakete müssen denselben Trust-Anforderungen wie vollständige Update-Pakete unterliegen.

Zusätzlich muss das rekonstruierte Ziel gegen die autorisierte Zielidentität geprüft werden.

```text
Trusted Delta
≠
Automatically Trusted Target
```

## Full-Package-Fallback

Ist ein Delta nicht anwendbar, muss NovaOS auf ein vollständiges Paket zurückfallen können.

Beispiele:

```text
Base Missing
Base Modified
BaseContentID Mismatch
Delta Corrupt
Unsupported Algorithm
Insufficient Temporary Storage
Reconstruction Failure
```

```text
Delta Failed
    ↓
Full Package
```

Ein Delta-Fehler darf ein verfügbares Full Update nicht unnötig blockieren.

## Delta Chains

Mehrere Deltas dürfen verkettet werden:

```text
v1 → v2 → v3 → v4
```

Der Update Manager muss entscheiden können, ob eine Delta-Kette gegenüber einem vollständigen Paket sinnvoll ist.

Zu berücksichtigen sind:

```text
Total Download Size
Reconstruction Cost
Failure Risk
Temporary Storage
CPU Cost
Verification Cost
Network Cost
```

Sehr lange Delta-Ketten sollen vermieden werden.

## Dependency Integration

Delta-Auswahl erfolgt erst innerhalb eines gültigen Update Plans.

```text
Dependency Resolution
       ↓
Target Version
       ↓
Select Transfer Method
      ↙              ↘
   Delta          Full Package
```

Das Delta verändert nicht die Dependency-Semantik der Zielversion.

## Atomic Integration

Die Rekonstruktion erfolgt im Staging-Bereich.

```text
Active Version
      ↓
Reconstruct Target
      ↓
Verify
      ↓
Atomic Update
```

Dadurch bleibt die aktive Version während der Delta-Anwendung unverändert.

## Transaction Integration

Delta-Verarbeitung ist Teil der Prepare-Phase:

```text
Begin Transaction
      ↓
Validate Base
      ↓
Acquire Delta
      ↓
Reconstruct Target
      ↓
Verify Target
      ↓
Prepared
```

Erst danach darf der normale Commit-Prozess beginnen.

## A/B Integration

Bei A/B-Updates kann ein Delta direkt zur Vorbereitung des inaktiven Slots verwendet werden.

```text
Active A
   ↓
Delta Reconstruction
   ↓
Inactive B
   ↓
Verify B
   ↓
Boot Switch
```

Der aktive Slot bleibt unverändert.

## Resource Economy

Delta Updates sollen nur verwendet werden, wenn sie gegenüber einem Full Package sinnvoll sind.

Der Update Manager darf berücksichtigen:

```text
Bandwidth
Storage
CPU
Memory
Energy
Latency
Temporary Space
```

```text
Smallest Download
≠
Lowest Total Cost
```

## Algorithm Agility

Das Delta-Format darf nicht dauerhaft an einen einzelnen Patch-Algorithmus gebunden sein.

```text
Delta Interface
      ↓
Algorithm Provider
```

Neue Verfahren müssen versioniert ergänzt werden können.

## Reproduzierbarkeit

Delta-Erzeugung und Zielrekonstruktion sollen nachvollziehbar sein.

```text
BaseContentID
      +
DeltaID
      ↓
TargetContentID
```

Das Ergebnis muss unabhängig davon dieselbe TargetContentID besitzen, ob es über Delta oder Full Package bezogen wurde.

## Provenance

NovaOS soll nachvollziehen können:

```text
DeltaID
BaseContentID
TargetContentID
Algorithm
PackageID
RepositoryID
ReconstructionResult
VerificationResult
FallbackReason
```

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
Delta Available
Base Version
BaseContentID
Target Version
TargetContentID
Delta Size
Full Package Size
Algorithm
Estimated Cost
Verification State
Fallback Availability
```

## Normative Anforderungen

1. NovaOS SOLL Delta Updates unterstützen.
2. Jedes Delta MUSS eine eindeutige Base- und Target-Identität besitzen.
3. BaseContentID MUSS vor Anwendung geprüft werden.
4. Versionsgleichheit DARF NICHT als ausreichende Base-Validierung gelten.
5. Delta-Payload MUSS gegen Manipulation geschützt sein.
6. Delta-Pakete MÜSSEN den normalen Update-Trust-Regeln unterliegen.
7. Delta-Rekonstruktion SOLL außerhalb des aktiven Artefakts erfolgen.
8. Das rekonstruierte Ziel MUSS vor Verwendung vollständig verifiziert werden.
9. TargetContentID MUSS unabhängig vom verwendeten Transferweg identisch sein.
10. Fehlgeschlagene Delta-Rekonstruktion DARF NICHT committed werden.
11. Ein Full-Package-Fallback MUSS unterstützt werden können.
12. Delta-Fehler DÜRFEN ein verfügbares vollständiges Update NICHT unnötig blockieren.
13. Delta-Ketten MÜSSEN hinsichtlich Gesamtkosten bewertbar sein.
14. Sehr lange oder ineffiziente Delta-Ketten SOLLEN durch Full Packages ersetzt werden.
15. Delta-Auswahl DARF Dependency- oder Compatibility-Regeln NICHT verändern.
16. Delta-Rekonstruktion MUSS in die Prepare-Phase der Update-Transaktion integrierbar sein.
17. Delta Updates MÜSSEN mit atomaren Updates kompatibel sein.
18. Delta Updates MÜSSEN mit A/B-Updates kombinierbar sein.
19. Resource Economy SOLL bei der Wahl zwischen Delta und Full Package berücksichtigt werden.
20. Kleinere Downloadgröße DARF NICHT automatisch als optimale Update-Methode gelten.
21. Das Delta-System MUSS Algorithm Agility unterstützen.
22. Delta-Verarbeitung MUSS nachvollziehbare Provenance besitzen.
23. Delta- und Verification-State MÜSSEN autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-UPDATE-MANAGER-0001`
- `NPSPEC-UPDATE-PACKAGE-0001`
- `NPSPEC-UPDATE-DEPENDENCY-0001`
- `NPSPEC-UPDATE-SIGNING-0001`
- `NPSPEC-UPDATE-ATOMIC-0001`
- `NPSPEC-UPDATE-TRANSACTIONAL-0001`
- `NPSPEC-UPDATE-AB-0001`
- `NPSPEC-VERIFY-REPRODUCIBLE-0001`
- `NPSPEC-STATE-VERSIONING-0001`
- `NPSPEC-RESOURCE-ECONOMY-0001`
- `ADR-ARCH-0173`

## Ergebnis

```text
Target Update
     ↓
Delta Available?
   ↙           ↘
 Yes            No
  ↓              ↓
Validate Base   Full Package
  ↓
Apply Delta to Staging
  ↓
Verify TargetContentID
 ↙              ↘
Valid           Invalid
 ↓                ↓
Continue       Full Package
 ↓
Atomic / Transactional Update
```

NovaOS erhält damit ein sicheres Delta-Update-Verfahren, das Übertragungs- und Speicheraufwand reduzieren kann, ohne die Zielversion anders zu behandeln als ein vollständiges Update-Paket. Entscheidend bleibt stets die verifizierte Zielidentität und nicht der Weg, über den das Zielartefakt erzeugt wurde.