# ADR-BOOT-0006 – Secure / Measured / Verified Boot Integration

## Status

Angenommen

## Kategorie

Boot / Security / Secure Boot / Verified Boot / Measured Boot / TPM / Trust / Attestation

## Kontext

Mit `ADR-BOOT-0005_Vertrauenswürdige_Bootkette` ist festgelegt, dass NovaOS eine durchgängige vertrauenswürdige Bootkette verwendet.

Für die konkrete technische Umsetzung müssen jedoch drei unterschiedliche Sicherheitsmechanismen sauber voneinander getrennt und anschließend kontrolliert integriert werden:

```text
Secure Boot
Verified Boot
Measured Boot
```

Diese Begriffe werden häufig vermischt, obwohl sie unterschiedliche Aufgaben erfüllen.

Vereinfacht:

```text
Secure Boot
    schützt den Einstieg in die Bootkette

Verified Boot
    entscheidet, ob ein Artefakt ausgeführt werden darf

Measured Boot
    dokumentiert, was tatsächlich ausgeführt wurde
```

Keiner dieser Mechanismen ersetzt die anderen vollständig.

UEFI Secure Boot kann beispielsweise verhindern, dass ein nicht autorisierter EFI-Loader gestartet wird, überprüft aber nicht automatisch das vollständige NovaOS-Systemmodell.

Verified Boot kann Kernel und Bootartefakte verifizieren, erzeugt aber allein keine hardwaregestützte Historie darüber, was tatsächlich gestartet wurde.

Measured Boot kann Bootartefakte in TPM-PCRs messen, verhindert aber allein nicht deren Ausführung.

NovaOS benötigt deshalb ein gemeinsames Modell, das alle drei Mechanismen miteinander verbindet, ohne ihre Semantik zu vermischen.

---

## Entscheidung

NovaOS integriert Secure Boot, Verified Boot und Measured Boot als drei **orthogonale, aber koordinierte Sicherheitsmechanismen** innerhalb derselben Boot Trust Architecture.

Das Grundmodell lautet:

```text
Platform Root of Trust
        │
        ▼
Secure Boot
        │
        ▼
Nova Boot Entry
        │
        ▼
Verified Boot
        │
        ├──────────────┐
        │              │
        ▼              ▼
Authorized         Measurement
Execution              │
        │               ▼
        │          Measured Boot
        │               │
        │               ▼
        │          TPM / Event Log
        │
        ▼
Kernel Handoff
        │
        ▼
Nova.Trust
```

Dabei gilt:

```text
Secure Boot protects entry.

Verified Boot grants execution authority.

Measured Boot records execution state.
```

---

## Grundprinzip

Es gilt:

```text
Verify before execution.

Measure what is executed.

Use platform Secure Boot
when available,
but never depend on it alone.
```

---

## Orthogonale Sicherheitsdimensionen

NovaOS modelliert die drei Mechanismen getrennt.

Konzeptionell:

```text
BootSecurityState {
    secureBoot
    verifiedBoot
    measuredBoot
}
```

Ein Mechanismus darf nicht allein aus dem Zustand eines anderen abgeleitet werden.

Beispiel:

```text
SecureBoot = Enabled
```

bedeutet nicht automatisch:

```text
VerifiedBoot = Valid
```

---

## Secure Boot

Secure Boot bezeichnet den firmwaregestützten Schutz des initialen Bootpfads.

Auf UEFI-Systemen kann dies insbesondere:

```text
UEFI Secure Boot
```

sein.

Das Modell lautet:

```text
Firmware Trust Store
        ↓
Verify EFI Image
        ↓
NovaOS EFI Loader
```

Damit wird sichergestellt, dass der erste NovaOS-Code nur gestartet wird, wenn er gemäß Firmwarepolicy autorisiert ist.

---

## Rolle von Secure Boot

Secure Boot schützt primär die Grenze:

```text
Firmware
    ↓
NovaOS Boot Entry
```

Es schützt nicht automatisch jeden nachfolgenden NovaOS-Bootbestandteil.

Daher endet die NovaOS-Verifikation nicht nach dem erfolgreichen Start des EFI-Loaders.

---

## Verified Boot

Verified Boot bildet die zentrale NovaOS-Ausführungsentscheidung für Bootartefakte.

Das Modell lautet:

```text
Boot Artifact
      ↓
Identify
      ↓
Integrity Check
      ↓
Signature Check
      ↓
Trust Evaluation
      ↓
Policy Evaluation
      ↓
Authorize / Reject
```

Nur ein autorisiertes Artefakt darf in einem entsprechenden Secure Boot Mode ausgeführt werden.

---

## Measured Boot

Measured Boot erzeugt eine kryptografisch verkettete Dokumentation des tatsächlich ausgeführten Bootzustands.

Konzeptionell:

```text
Artifact
   ↓
Hash
   ↓
Measurement Event
   ↓
PCR Extend
```

Die Messung wird zusätzlich in einem strukturierten Event Log dokumentiert.

---

## Kein Autoritätsgewinn durch Messung

Measured Boot ist keine Autorisierung.

Es gilt:

```text
Measured
≠
Trusted
```

und:

```text
Measured
≠
Authorized
```

Ein Artefakt darf gemessen und anschließend trotzdem abgelehnt werden.

---

## Reihenfolge

NovaOS bevorzugt für ausführbare Artefakte den logischen Ablauf:

```text
Locate
  ↓
Identify
  ↓
Measure Candidate
  ↓
Verify
  ↓
Authorize
  ↓
Execute
```

oder, abhängig vom konkreten Messmodell:

```text
Locate
  ↓
Verify
  ↓
Measure Authorized Artifact
  ↓
Execute
```

Die genaue Reihenfolge muss in der jeweiligen NPSPEC eindeutig definiert werden.

Entscheidend ist, dass der Measurement Log erkennen lässt, ob ein Artefakt:

```text
attempted
authorized
executed
rejected
```

wurde.

---

## Execution Measurement

Mindestens alle tatsächlich ausgeführten oder in den privilegierten Bootzustand übernommenen sicherheitsrelevanten Artefakte sollen messbar sein.

Dazu gehören insbesondere:

- Nova Boot Entry,
- Nova Boot Core,
- Boot Manifest,
- Bootkonfiguration,
- Kernel,
- initiale Kernelmodule,
- Recovery Environment,
- NovaDOS.

---

## Candidate Measurement

NovaOS darf zusätzlich abgelehnte Kandidaten messen.

Dies ist optional und muss semantisch vom tatsächlich ausgeführten Bootpfad unterscheidbar sein.

Beispiel:

```text
Artifact A:
    Rejected

Artifact B:
    Authorized
    Executed
```

---

## Unified Boot Security State

NovaOS bildet die kombinierten Ergebnisse in einem gemeinsamen Security State ab.

Beispiel:

```text
BootSecurityState {
    secureBoot = Enabled
    verifiedBoot = Verified
    measuredBoot = Active
    trustMode = Production
}
```

Weitere mögliche Zustände:

```text
SecureBoot:
    Enabled
    Disabled
    Unsupported
    Unknown

VerifiedBoot:
    Verified
    Failed
    Development
    Override
    Unknown

MeasuredBoot:
    Active
    Unavailable
    Failed
    RequiredButUnavailable
```

---

## Keine boolesche Vereinfachung

Der kombinierte Bootzustand darf nicht allein als:

```text
Secure = true / false
```

modelliert werden.

Die einzelnen Sicherheitsdimensionen müssen getrennt introspektierbar bleiben.

---

## UEFI Secure Boot Integration

Wenn UEFI Secure Boot verfügbar ist, soll NovaOS den EFI-Bootloader so integrieren, dass der Firmware Trust Path verwendet werden kann.

Konzeptionell:

```text
UEFI Secure Boot
        ↓ verifies
NovaOS EFI Loader
        ↓
NovaOS Verified Boot
```

Die interne NovaOS Trust Chain beginnt danach weiterhin explizit.

---

## BIOS-Systeme

BIOS besitzt typischerweise kein direkt äquivalentes standardisiertes Secure-Boot-Modell.

Auf solchen Systemen kann daher gelten:

```text
SecureBoot = Unsupported
VerifiedBoot = Verified
MeasuredBoot = Active
```

wenn TPM und NovaOS-Verifikation verfügbar sind.

Ein fehlendes Secure-Boot-Feature bedeutet somit nicht automatisch einen unsicheren Bootzustand.

---

## Secure-Boot-Abhängigkeit

Eine Systempolicy darf jedoch explizit verlangen:

```text
SecureBoot = Required
```

Dann darf ein BIOS-System oder ein UEFI-System mit deaktiviertem Secure Boot nicht normal booten, sofern keine autorisierte alternative Hardware Root of Trust Policy definiert wurde.

---

## Secure Boot Policy

NovaOS unterscheidet mindestens:

```text
Required
Preferred
Optional
Forbidden
```

Beispiel:

```text
SecureBootPolicy = Preferred
```

kann auf BIOS-Systemen weiterhin Verified Boot erlauben.

---

## Verified Boot Policy

Analog kann Verified Boot als:

```text
Required
Development
RecoveryRestricted
```

modelliert werden.

Für normale produktive NovaOS-Installationen gilt grundsätzlich:

```text
VerifiedBoot = Required
```

---

## Measured Boot Policy

Measured Boot kann abhängig von Zielsystem und Sicherheitsprofil:

```text
Required
Preferred
Optional
```

sein.

Ein Desktop ohne TPM kann beispielsweise:

```text
VerifiedBoot = Required
MeasuredBoot = Optional
```

verwenden.

Ein Enterprise-System kann dagegen verlangen:

```text
MeasuredBoot = Required
```

---

## Policy-Kombination

Die drei Dimensionen werden unabhängig ausgewertet.

Beispiel:

```text
SecureBoot = Preferred
VerifiedBoot = Required
MeasuredBoot = Required
```

Dies erlaubt feinere Sicherheitsprofile als ein einzelner globaler Secure-Boot-Schalter.

---

## Hard Requirements

Ist ein Mechanismus als `Required` markiert und nicht verfügbar oder fehlgeschlagen, darf der normale Bootpfad nicht fortgesetzt werden.

Beispiel:

```text
MeasuredBoot = Required
TPM unavailable
    ↓
Policy Failure
```

---

## Soft Requirements

Bei:

```text
Preferred
```

darf ein definierter Fallback erfolgen, sofern alle Hard Security Requirements weiterhin erfüllt bleiben.

Der degradierte Sicherheitszustand muss introspektierbar und auditierbar sein.

---

## TPM Integration

Measured Boot soll TPM 2.0 beziehungsweise zukünftige kompatible Hardware Trust Provider unterstützen.

Das logische Modell lautet:

```text
Measurement
    ↓
Hardware Trust Provider
    ↓
Protected Measurement Register
```

TPM ist der primäre PC-Provider, aber nicht die einzige zukünftige mögliche Implementierung.

---

## Hardware Trust Provider

Die Architektur soll nicht direkt überall TPM-spezifische Annahmen enthalten.

Konzeptionell:

```text
BootMeasurementProvider
├── TPM2Provider
├── VirtualTPMProvider
└── FutureHardwareTrustProvider
```

Dadurch bleibt die Semantik evolvierbar.

---

## PCRs

TPM-PCRs werden über Extend-Operationen aktualisiert.

Konzeptionell:

```text
PCRnew =
    Hash(PCRold || Measurement)
```

Dadurch hängt der Endzustand von Reihenfolge und Inhalt der Messungen ab.

---

## PCR-Zuordnung

NovaOS definiert eine stabile und versionierte PCR-Verwendungsrichtlinie.

Diese kann beispielsweise Kategorien unterscheiden wie:

```text
Bootloader
Boot Policy
Kernel
Modules
Recovery
NovaOS Trust State
```

Die exakten PCR-Indizes werden in einer NPSPEC festgelegt.

---

## Keine zufällige PCR-Nutzung

Bootkomponenten dürfen PCRs nicht frei und inkonsistent auswählen.

Die Zuordnung muss zentral definiert und versioniert sein.

---

## Measurement Event

Jede Messung besitzt neben dem Digest semantische Metadaten.

Beispiel:

```text
BootMeasurementEvent {
    sequence
    phase
    artifactIdentity
    eventType
    algorithm
    digest
    targetRegister
    decision
}
```

---

## Event Log

Alle relevanten Messungen werden in einem strukturierten Boot Measurement Log erfasst.

Das Event Log muss mit den tatsächlichen PCR-Operationen korrelierbar sein.

---

## Log-Integrität

Das Measurement Log selbst darf nicht als allein vertrauenswürdige Quelle behandelt werden.

Seine Plausibilität kann gegen die finalen Hardware-Registerzustände geprüft werden.

---

## Replay-Verifikation

Ein Verifier soll aus:

```text
Event Log
+
initial PCR state
```

den erwarteten PCR-Endzustand rekonstruieren können.

Dies ermöglicht eine Konsistenzprüfung zwischen Log und Hardware.

---

## Hash Algorithm Agility

Measured Boot darf nicht dauerhaft auf einen einzigen Hashalgorithmus festgelegt sein.

Mögliche Hashbanken können beispielsweise umfassen:

```text
SHA-256
SHA-384
```

Die Policy bestimmt erlaubte Algorithmen.

---

## Mehrere Hashbanken

Wenn der Hardwareprovider mehrere Hashbanken unterstützt, darf NovaOS Messungen in mehreren Banken parallel führen.

Dies erleichtert spätere Algorithm Migration.

---

## Verified-Boot-Hashing

Verified Boot und Measured Boot dürfen denselben berechneten Digest wiederverwenden, sofern:

- derselbe Algorithmus,
- dieselbe Byte-Repräsentation,
- dieselbe Canonicalization

verwendet wird.

Dadurch werden unnötige Mehrfachberechnungen vermieden.

---

## Keine semantische Kopplung durch Hash-Reuse

Die Wiederverwendung eines Digests darf die logische Trennung nicht aufheben.

Ein Digest kann gleichzeitig verwendet werden für:

```text
Signature Verification
Measurement
Artifact Identity
```

ohne dass diese Mechanismen semantisch gleich werden.

---

## Signaturprüfung

Verified Boot verwendet die in `ADR-BOOT-0005` definierte Trust Chain.

Beispiel:

```text
Digest
   ↓
Signature
   ↓
Signer Identity
   ↓
Trust Store
   ↓
Policy
   ↓
Authorization
```

---

## Secure-Boot-Signatur vs NovaOS-Signatur

UEFI Secure Boot und NovaOS Verified Boot dürfen unterschiedliche Signaturformate und Trust Stores besitzen.

Beispiel:

```text
UEFI DB
    authorizes EFI Loader

Nova Trust Store
    authorizes kernel
```

Dies ist beabsichtigt.

---

## Gemeinsame Trust Roots

Optional dürfen beide Schichten auf gemeinsame organisatorische Root Keys zurückgeführt werden.

Dies ist jedoch keine architektonische Voraussetzung.

---

## Double Verification

Ein Artefakt darf durch mehrere Ebenen überprüft werden.

Beispiel:

```text
UEFI verifies NovaOS EFI loader
NovaOS Boot Core verifies itself / next-stage metadata
NovaOS verifies kernel
```

Diese Redundanz ist bei klar definierten Grenzen erwünscht.

---

## Boot Manifest

Verified Boot soll bevorzugt mit einem signierten Boot Manifest zusammenarbeiten.

Konzeptionell:

```text
Signed Boot Manifest
        │
        ├── Kernel Hash
        ├── Module Hashes
        ├── Config Hash
        ├── Generation
        └── Security Policy
```

Das Manifest verbindet die autorisierten Bestandteile einer Bootgeneration.

---

## Measurement des Manifests

Das tatsächlich verwendete Boot Manifest soll selbst gemessen werden.

Dadurch kann später nachvollzogen werden, welche Boot Policy und Artefaktzusammenstellung verwendet wurde.

---

## Messung der Konfiguration

Sicherheitsrelevante Bootkonfiguration muss ebenfalls in den Measurement State einfließen können.

Beispiel:

```text
debug = false
recovery = false
deterministic = true
```

Änderungen an sicherheitsrelevanten Bootoptionen müssen somit attestierbar sein.

---

## Nicht sicherheitsrelevante UI-Einstellungen

Nicht jede rein visuelle Booteinstellung muss gemessen werden.

Die Spezifikation muss zwischen:

```text
security-relevant configuration
```

und:

```text
presentation-only configuration
```

unterscheiden.

---

## Boot Mode Measurement

Der ausgewählte Bootmodus soll gemessen werden können.

Beispiele:

```text
Normal
Recovery
NovaDOS
Development
Diagnostic
```

Ein Remote Verifier kann dadurch unterscheiden, ob ein System im normalen Produktionsmodus gestartet wurde.

---

## Development Mode

Development Mode muss im Verified- und Measured-Boot-State deutlich erkennbar sein.

Beispiel:

```text
VerifiedBoot = Development
BootMode = Development
```

Dieser Zustand muss in Measurement Events einfließen.

---

## Trust Override

Ein manueller Trust Override muss ebenfalls messbar sein.

Es gilt:

```text
Override used
```

darf nicht denselben gemessenen Zustand erzeugen wie:

```text
Fully verified production boot
```

---

## Recovery Mode

Recovery und NovaDOS erhalten eigene messbare Bootzustände.

Dies ermöglicht beispielsweise eine Policy:

```text
Disk secret only available
during normal verified boot
```

aber nicht automatisch im Recovery Mode.

---

## Sealed Secrets

Hardware Trust Provider können Secrets an einen erwarteten Bootzustand binden.

Beispiel:

```text
Disk Key
    ↓ sealed to
PCR Policy
    ↓
released only on authorized state
```

---

## Policy statt starrem PCR-Wert

Secrets sollten möglichst nicht ausschließlich an einen einzigen unveränderlichen exakten PCR-Snapshot gebunden werden.

Andernfalls können legitime Updates die Freigabe verhindern.

Stattdessen sollen autorisierte Zustände und Generationen in einer Update-fähigen Policy modellierbar sein.

---

## Update Transition

Für Updates kann beispielsweise gelten:

```text
Generation N
    authorized

Generation N+1
    staged
    signed
    measured
    authorized

After successful health validation:
    N+1 becomes known-good
```

Sealed-Secret-Policies müssen diesen Übergang unterstützen können.

---

## Remote Attestation

Measured Boot bildet die Grundlage für Remote Attestation.

Konzeptionell:

```text
Nonce from Verifier
      ↓
TPM Quote
      +
Measurement Log
      +
Platform Identity
      ↓
Verifier
```

Der Verifier kann daraus den Bootzustand bewerten.

---

## Local Attestation

Auch lokale Systemkomponenten dürfen einen attestierten Bootzustand verwenden.

Beispiel:

```text
High-value service
    starts only if
Production Verified Boot
```

Dies muss über Nova.Trust kontrolliert werden.

---

## Attestation und Datenschutz

Remote Attestation kann Informationen über:

- Softwareversion,
- Bootmodus,
- Plattformzustand,
- Hardwareidentität

offenlegen.

Daher muss Attestation Data-Sovereignty-, Privacy- und Capability-Regeln beachten.

---

## Keine ungefragte Remote Attestation

NovaOS darf nicht automatisch detaillierte Attestation Evidence an beliebige externe Systeme übertragen.

Attestation benötigt eine explizite autorisierte Beziehung beziehungsweise Policy.

---

## Privacy-Preserving Attestation

Zukünftige Attestation-Mechanismen dürfen pseudonyme oder datensparsame Identitätsmodelle verwenden.

Die Bootarchitektur darf keine unnötige globale Hardwareidentität erzwingen.

---

## Measured Boot ohne Remote Attestation

Measured Boot bleibt auch dann sinnvoll, wenn Remote Attestation vollständig deaktiviert ist.

Lokale Diagnose, Secret Sealing und Forensik profitieren weiterhin von Messwerten.

---

## Firmware Measurements

Wenn Firmware bereits eigene Messungen erzeugt, darf NovaOS diese übernehmen beziehungsweise in den Gesamtzustand integrieren.

Beispiel:

```text
Firmware Measurements
        ↓
Nova Boot Measurements
        ↓
Kernel Measurements
```

Die Herkunft jedes Measurement Events muss klar bleiben.

---

## Kein blindes Vertrauen in Firmware Event Logs

Firmware-generierte Logs werden defensiv behandelt.

Hardware-PCR-Zustand und weitere Plausibilitätsprüfungen dürfen zur Validierung verwendet werden.

---

## Measurement Continuity

Messungen dürfen nach dem Kernel Handoff fortgesetzt werden.

Der Kernel übernimmt:

```text
Measurement Provider
Measurement Log
Current Measurement State
```

und kann weitere frühe Systemkomponenten messen.

---

## Trust Handoff

`BootInfo` enthält einen versionierten Boot Security Handoff.

Beispiel:

```text
BootSecurityInfo {
    version
    secureBootState
    verifiedBootState
    measuredBootState
    policyId
    trustRootId
    manifestId
    measurementLog
}
```

---

## Keine rohen internen Pointer

Der Kernel Handoff darf keine unsicheren Bootloader-internen Pointer auf temporäre Trust-Strukturen enthalten.

Persistente Daten müssen in klar definierten Speicherbereichen und Formaten übergeben werden.

---

## Kernel Revalidation

Der Kernel validiert die Struktur und Konsistenz des Boot Security Handoff erneut.

Er muss mindestens erkennen können:

- ungültige Version,
- beschädigte Längen,
- unbekannte kritische Flags,
- inkonsistente Zustände.

---

## Beispiel inkonsistenter Zustand

Beispiel:

```text
MeasuredBoot = Active
MeasurementLog = Missing
```

kann je nach Vertrag als:

```text
Invalid / Partial
```

behandelt werden.

---

## Security State Provenance

Der Kernel muss unterscheiden können, welche Zustände:

```text
Firmware-reported
Bootloader-verified
Hardware-backed
Kernel-confirmed
```

sind.

Dies verhindert das Gleichsetzen unterschiedlich starker Nachweise.

---

## Evidence Strength

Optional kann NovaOS Evidence nach Stärke klassifizieren.

Beispiel:

```text
SoftwareReported
CryptographicallyVerified
HardwareMeasured
HardwareAttested
```

Diese Klassifikation darf in Nova.Trust einfließen.

---

## Boot Trust Continuity

Nach Kernelstart wird der Boot Security State in Nova.Trust integriert.

Konzeptionell:

```text
BootSecurityState
      ↓
Nova.Trust
      ↓
Capabilities / Policies / Audit
```

Damit kann der laufende Systemzustand auf dem nachgewiesenen Bootzustand aufbauen.

---

## Kein permanenter Sonderweg

Secure/Measured/Verified Boot sollen nicht als vollständig isolierte Sicherheitsinseln neben Nova.Trust bestehen bleiben.

Der Boot Trust State wird in das gemeinsame Trust-Modell überführt.

---

## Boot Security Profiles

NovaOS darf vordefinierte Sicherheitsprofile anbieten.

Beispiel:

```text
Standard
    SecureBoot = Preferred
    VerifiedBoot = Required
    MeasuredBoot = Preferred

HighSecurity
    SecureBoot = Required
    VerifiedBoot = Required
    MeasuredBoot = Required

Legacy
    SecureBoot = Optional
    VerifiedBoot = Required
    MeasuredBoot = Optional

Development
    SecureBoot = Optional
    VerifiedBoot = Development
    MeasuredBoot = Preferred
```

Diese Profile sind Policy-Vorlagen, keine unveränderlichen Mechanismen.

---

## Enterprise Policy

Enterprise-Systeme dürfen Profile zentral definieren und signieren.

Ein lokaler Benutzer darf Hard Security Requirements solcher Policies nicht ohne entsprechende Autorität abschwächen.

---

## Offline-Fähigkeit

Verified Boot muss vollständig offline funktionieren.

Measured Boot muss ebenfalls ohne Netzwerk möglich sein.

Secure Boot ist firmwarelokal.

Remote Attestation ist optional und separat.

---

## Keine Cloudabhängigkeit

Es gilt ausdrücklich:

```text
No network
```

darf einen normalen bereits provisionierten Verified/Measured Boot nicht verhindern, sofern keine explizite externe Policy dies verlangt.

---

## Bootzeit

Die zusätzlichen Sicherheitsprüfungen erhöhen potenziell die Bootzeit.

Der Bootloader darf deshalb:

- Hashberechnungen zusammenführen,
- Hardwarebeschleunigung verwenden,
- verifizierte immutable Artefaktmetadaten cachen,
- Mess- und Verifikationshashes wiederverwenden,

sofern die Sicherheitssemantik unverändert bleibt.

---

## Kein Cache ohne Bindung

Ein gecachtes Verifikationsergebnis darf nur verwendet werden, wenn sicher nachgewiesen werden kann, dass das zugrunde liegende Artefakt unverändert ist.

Beispielsweise über:

- immutable content address,
- geschützte Generation,
- cryptographic identity.

---

## Zero-Copy

Große Bootartefakte dürfen möglichst ohne unnötige Mehrfachkopien gehasht, verifiziert und geladen werden.

Zero-Copy darf jedoch niemals die Integritäts- oder TOCTOU-Garantien schwächen.

---

## TOCTOU-Schutz

Zwischen:

```text
Verification
Measurement
Execution
```

darf das zugrunde liegende Artefakt nicht unkontrolliert veränderbar sein.

Bevorzugt wird:

```text
load immutable representation
        ↓
hash once
        ↓
verify
        ↓
measure
        ↓
execute from same representation
```

---

## DMA-Schutz

Während der kritischen Verifikations- und Messphase darf DMA die geprüften Bootartefakte nicht unkontrolliert verändern können.

Soweit Hardware verfügbar ist, soll IOMMU-Schutz früh aktiviert werden.

Alternativ müssen Bootartefakte in ausreichend geschützten Speicherbereichen verarbeitet werden.

---

## Memory Ownership

Verified und gemessene Bootartefakte erhalten explizite Memory Ownership.

Nach erfolgreicher Verifikation darf derselbe Speicher nicht unbemerkt durch andere Bootkomponenten überschrieben werden.

---

## Measurement Failure

Ein Measurement Failure muss vom Verification Failure unterschieden werden.

Beispiel:

```text
Kernel verified successfully
TPM extend failed
```

Die Policy entscheidet:

```text
MeasuredBoot Required
    ↓
Boot rejected

MeasuredBoot Preferred
    ↓
Boot may continue degraded
```

---

## Secure Boot Failure

Wenn UEFI Secure Boot das NovaOS-Image bereits vor dessen Start ablehnt, kann die interne NovaOS-Bootkette nicht ausgeführt werden.

Recovery erfolgt dann auf Firmwareebene oder über einen anderen autorisierten EFI-Pfad.

---

## Verified Boot Failure

Bei Verified-Boot-Fehlern kann NovaOS:

- Known-Good auswählen,
- Recovery starten,
- NovaDOS starten,
- administrativen Override anbieten,
- Boot verweigern,

abhängig von der Trust Policy.

---

## Measured Boot Degradation

Wenn Measured Boot nur `Preferred` ist und der TPM ausfällt, kann der Boot fortgesetzt werden.

Der Zustand muss dann beispielsweise lauten:

```text
VerifiedBoot = Verified
MeasuredBoot = Failed
SecurityDegradation = Present
```

---

## Kein stiller Fallback

Ein solcher degradierter Zustand darf nicht als vollständig gemessener Boot dargestellt werden.

---

## Fail-Safe Defaults

Für Required Security Controls gilt:

```text
Unknown
=
Not satisfied
```

und:

```text
Failure
≠
Permission
```

---

## Recovery Policy

Recovery kann eine andere Security Policy als Normal Boot verwenden, muss aber explizit definiert sein.

Beispiel:

```text
Normal:
    Production kernel only

Recovery:
    Recovery-signed image only
```

Nicht zulässig:

```text
Recovery:
    execute anything
```

---

## Attestation Policy

Attestation-Auswertung gehört nicht allein in den Bootloader.

Der Bootloader erzeugt Evidence.

Nova.Trust beziehungsweise autorisierte Verifier entscheiden, welche Bedeutung diese Evidence besitzt.

---

## Architekturintrospektion

Nach dem Start soll ein autorisierter Benutzer mindestens erkennen können:

```text
Secure Boot:
    Enabled

Verified Boot:
    Verified

Measured Boot:
    Active

Hardware Trust:
    TPM 2.0

Boot Mode:
    Normal

Trust Override:
    None
```

---

## Detailintrospektion

Privilegierte Diagnose darf zusätzlich anzeigen:

```text
Policy
Trust Root
Manifest ID
Measurement Algorithms
PCR Bank
Boot Generation
Attestation Capability
```

Sicherheitsrelevante Details müssen Capability-basiert gefiltert werden.

---

## Audit

Relevante Ereignisse müssen auditierbar sein.

Beispiele:

```text
Secure Boot detected
Kernel signature verified
Boot manifest accepted
Kernel measured
TPM unavailable
Measured Boot degraded
Development override used
Recovery mode entered
```

---

## Boot Security Decision Record

NovaOS darf für den gesamten Boot eine strukturierte Zusammenfassung erzeugen.

Beispiel:

```text
BootSecurityDecision {
    profile = Standard
    secureBoot = Enabled
    verifiedBoot = Verified
    measuredBoot = Active
    fallbackUsed = false
    overrideUsed = false
    result = Authorized
}
```

---

## Deterministic Mode

Bei identischen:

- Bootartefakten,
- Policy,
- Trust Store,
- Messreihenfolge,
- Hardware Trust Provider,

muss die Verified-Boot-Entscheidung deterministisch sein.

Die Measurement Sequence muss ebenfalls stabil definiert sein.

---

## Measurement Ordering

Messungen sicherheitsrelevanter Objekte müssen in einer deterministischen Reihenfolge erfolgen.

Unkontrollierte Parallelität darf nicht zu unterschiedlichen PCR-Endzuständen führen.

---

## Parallel Verification

Hashing oder Verifikation darf intern parallelisiert werden.

Für gemessene Reihenfolgezustände muss jedoch ein deterministischer Commit in die Measurement Chain erfolgen.

---

## Structured Concurrency

Parallele Boot Security Tasks gehören in einen gemeinsamen Boot Execution Scope.

Bei Abbruch eines Bootplans müssen zugehörige:

- Hashing Tasks,
- Verification Tasks,
- Measurement Preparation Tasks

kontrolliert beendet werden.

---

## Transactional Boot

Secure/Verified/Measured Boot integriert sich in die transaktionale Bootplanung.

Beispiel:

```text
Select Candidate
      ↓
Validate
      ↓
Verify
      ↓
Measure
      ↓
Prepare Handoff
      ↓
Commit Boot
```

Vor dem Commit kann noch auf einen anderen autorisierten Bootplan gewechselt werden.

---

## Measurement von abgebrochenen Plänen

Wenn ein Bootplan nach Messungen verworfen wird, muss das Event Log dies korrekt ausdrücken.

Messungen in PCRs sind nicht rückgängig zu machen.

Daher muss die PCR-Policy solche Übergänge berücksichtigen.

---

## Kein PCR-Rollback

PCR Extend ist grundsätzlich append-only innerhalb des jeweiligen Bootzyklus.

NovaOS darf daher nicht versuchen, einen Messzustand logisch „zurückzusetzen“, nachdem ein Artefakt gemessen wurde.

---

## Planungsregel

Deshalb sollen endgültige Execution Measurements möglichst erst dann erfolgen, wenn ein Artefakt tatsächlich Teil des aktivierten Bootplans ist.

Candidate Measurements können in separaten Events oder Registern behandelt werden.

---

## Bootgenerationen

Jede NovaOS-Systemgeneration soll eindeutig messbar sein.

Beispiel:

```text
Generation ID
Manifest Hash
Kernel Hash
```

Damit lassen sich Known-Good- und Updatezustände klar attestieren.

---

## Self-Healing

Self-Healing darf den Bootplan reparieren oder auf Known-Good wechseln.

Der tatsächlich gewählte Endzustand muss anschließend erneut:

```text
verify
measure
authorize
```

werden.

Self-Healing darf keinen Vertrauensstatus erben, der für einen anderen Artefaktsatz galt.

---

## Hot Replacement

Diese ADR betrifft primär den statischen Bootpfad.

Nach Kernelstart gelten für Hot Replacement eigene Trust- und Measurement-Regeln.

Boot Evidence bleibt jedoch als Provenance des initialen Systemzustands erhalten.

---

## Virtuelle Maschinen

Virtualisierte NovaOS-Systeme dürfen:

```text
Virtual Secure Boot
Virtual TPM
```

verwenden.

Der Security State muss erkennen lassen, ob Evidence:

```text
Physical Hardware Backed
```

oder:

```text
Virtualized
```

ist.

---

## Nested Virtualization

Bei verschachtelter Virtualisierung müssen Trust-Layer voneinander unterscheidbar bleiben.

Ein Gast darf einen vTPM nicht automatisch als physischen Plattformnachweis interpretieren.

---

## Confidential Computing

Zukünftige Confidential-Computing-Plattformen können zusätzliche Launch Measurements und Attestation Evidence bereitstellen.

Diese sollen in dasselbe Evidence-Modell integrierbar sein.

---

## Evolvierbarkeit

Die Integration muss zukünftige Mechanismen unterstützen können wie:

- neue TPM-Versionen,
- neue Secure-Boot-Provider,
- DICE,
- measured launch environments,
- confidential-computing evidence,
- hardware roots of trust,
- transparency proofs.

Die grundlegenden Semantiken:

```text
Authorize
Measure
Attest
```

bleiben getrennt.

---

## KI-Unabhängigkeit

Secure Boot, Verified Boot und Measured Boot müssen vollständig ohne KI funktionieren.

KI darf optional:

- Boot Security Logs analysieren,
- Attestation-Daten erklären,
- Anomalien erkennen,
- Recovery-Empfehlungen geben.

KI darf nicht:

- Signaturen ersetzen,
- PCR-Werte bewerten und dadurch eigenständig Autorität vergeben,
- fehlgeschlagene Required Policies überschreiben,
- unbekannte Artefakte autorisieren.

---

## Normative Anforderungen

1. NovaOS MUSS Secure Boot, Verified Boot und Measured Boot als getrennte Sicherheitsmechanismen modellieren.
2. Die drei Mechanismen MÜSSEN in einen gemeinsamen Boot Security State integrierbar sein.
3. Secure Boot MUSS als Schutz des Firmware-zu-Bootloader-Einstiegs modelliert werden können.
4. Verified Boot MUSS für NovaOS die primäre Ausführungsautorität von Bootartefakten bestimmen.
5. Measured Boot MUSS den tatsächlich relevanten Bootzustand kryptografisch dokumentieren können.
6. Measured Boot DARF KEINE Ausführungsautorität erzeugen.
7. Secure Boot DARF Verified Boot NICHT ersetzen.
8. Verified Boot DARF Measured Boot NICHT implizit als erfolgreich markieren.
9. Ein aktiviertes UEFI Secure Boot SOLL in die NovaOS Boot Trust Chain integriert werden.
10. BIOS-Systeme MÜSSEN Verified Boot auch ohne UEFI Secure Boot unterstützen können.
11. Secure Boot, Verified Boot und Measured Boot MÜSSEN unabhängig als Required, Preferred oder Optional policyfähig sein.
12. Ein als Required definierter Mechanismus MUSS bei Fehler oder Nichtverfügbarkeit den normalen Bootpfad blockieren.
13. Preferred-Mechanismen DÜRFEN kontrolliert degradieren, sofern keine Hard Security Requirement verletzt wird.
14. Sicherheitsdegradation MUSS introspektierbar sein.
15. TPM 2.0 SOLL als primärer PC Measurement Provider unterstützt werden.
16. Die Architektur MUSS weitere Hardware Measurement Provider zulassen.
17. TPM-spezifische Implementierungsdetails SOLLEN hinter einem Measurement Provider abstrahiert werden.
18. PCR-Nutzung MUSS zentral definiert und versioniert sein.
19. Bootkomponenten DÜRFEN PCRs NICHT unkoordiniert verwenden.
20. Relevante Measurements MÜSSEN in einem strukturierten Event Log abbildbar sein.
21. Measurement Events MÜSSEN mindestens Artefaktidentität, Digest, Algorithmus und Zielregister identifizieren können.
22. Das Event Log SOLL gegen den finalen Hardware Measurement State überprüfbar sein.
23. Ein Verifier SOLL PCR-Zustände aus dem Event Log rekonstruieren können.
24. Measured Boot MUSS Algorithm Agility unterstützen.
25. Mehrere Hashbanken SOLLEN parallel unterstützt werden können.
26. Verifikation und Measurement DÜRFEN denselben Digest wiederverwenden, wenn die Byte-Repräsentation identisch ist.
27. Digest-Reuse DARF die semantische Trennung der Mechanismen NICHT aufheben.
28. Boot Manifest und verwendete sicherheitsrelevante Bootkonfiguration SOLLEN gemessen werden.
29. Der tatsächlich gestartete Kernel MUSS messbar sein.
30. Frühe sicherheitsrelevante Module SOLLEN messbar sein.
31. Recovery Environment MUSS einen eigenen messbaren Zustand erzeugen können.
32. NovaDOS MUSS einen eigenen messbaren Zustand erzeugen können.
33. Development Mode MUSS im Measurement State erkennbar sein.
34. Trust Overrides MÜSSEN im Measurement State erkennbar sein.
35. Ein vollständig verifizierter Production Boot DARF NICHT denselben Security State wie ein Override Boot erzeugen.
36. Bootmodus und sicherheitsrelevante Policy-Auswahl SOLLEN messbar sein.
37. Presentation-only-Konfiguration MUSS NICHT zwingend gemessen werden.
38. Hardware Sealing MUSS auf Basis autorisierter Measurement States unterstützt werden können.
39. Secret Sealing MUSS legitime Systemupdates berücksichtigen.
40. Bootgenerationen MÜSSEN eindeutig messbar sein können.
41. Known-Good-Generationen MÜSSEN weiterhin verifiziert und gemessen werden.
42. Remote Attestation SOLL auf Grundlage hardwaregestützter Measurements möglich sein.
43. Local Attestation SOLL unterstützt werden können.
44. Attestation MUSS Data-Sovereignty- und Privacy-Regeln beachten.
45. Attestation Evidence DARF NICHT ungefragt an beliebige externe Parteien übertragen werden.
46. Measured Boot MUSS auch ohne Remote Attestation sinnvoll nutzbar bleiben.
47. Firmware Measurements DÜRFEN in den NovaOS Measurement State übernommen werden.
48. Firmware Measurement Logs DÜRFEN NICHT blind als alleinige Wahrheit behandelt werden.
49. Measurement Continuity MUSS über den Kernel Handoff hinweg möglich sein.
50. BootInfo MUSS einen versionierten Boot Security Handoff bereitstellen können.
51. Der Kernel MUSS den Boot Security Handoff erneut strukturell validieren.
52. Der Kernel MUSS Provenance verschiedener Security-State-Quellen unterscheiden können.
53. Hardware-backed und software-reported Evidence MÜSSEN unterscheidbar sein.
54. Der Boot Security State MUSS in Nova.Trust integrierbar sein.
55. Boot Security DARF nach Kernel Entry NICHT als isoliertes Parallelsystem verbleiben.
56. Sicherheitsprofile SOLLEN Kombinationen aus Secure-, Verified- und Measured-Boot-Policies definieren können.
57. Enterprise Policies MÜSSEN Hard Security Requirements festlegen können.
58. Der Baseline Verified Boot MUSS offline funktionieren.
59. Measured Boot MUSS offline funktionieren.
60. Remote Attestation DARF optional sein.
61. Secure/Verified/Measured Boot DÜRFEN keine implizite Cloudabhängigkeit erzeugen.
62. Hashing für Measurement und Verification SOLL möglichst zusammengeführt werden können.
63. Optimierungen DÜRFEN die Trust- und TOCTOU-Semantik NICHT schwächen.
64. Verifizierte Artefakte MÜSSEN gegen unkontrollierte Veränderung zwischen Verification und Execution geschützt werden.
65. DMA-Risiken für Boot Security Artefakte SOLLEN soweit möglich begrenzt werden.
66. Verified Boot Failure und Measurement Failure MÜSSEN getrennte Fehlerklassen sein.
67. Ein Measurement Failure MUSS gemäß seiner Policy unabhängig bewertet werden.
68. Ein Verified Boot Failure DARF NICHT durch erfolgreiches Measurement legitimiert werden.
69. Ein Secure-Boot-Fehler DARF NICHT durch interne NovaOS-Policy umgangen werden, wenn die Firmware die Ausführung bereits verweigert.
70. Recovery bei Verified-Boot-Fehlern MUSS policygesteuert erfolgen.
71. Sicherheitsfallbacks DÜRFEN Hard Security Requirements NICHT abschwächen.
72. Unknown Security State MUSS bei Required Controls als nicht erfüllt gelten.
73. Boot Security Entscheidungen MÜSSEN auditierbar sein.
74. Secure-, Verified- und Measured-Boot-Zustände MÜSSEN introspektierbar sein.
75. Detailintrospektion MUSS capabilitybasiert filterbar sein.
76. Verified-Boot-Entscheidungen MÜSSEN deterministisch sein.
77. Die Reihenfolge relevanter Measurements MUSS deterministisch definiert sein.
78. Parallel Verification DARF nicht zu nichtdeterministischen PCR-Sequenzen führen.
79. Boot Security Tasks SOLLEN Structured Concurrency verwenden.
80. Secure/Verified/Measured Boot MUSS in die transaktionale Bootplanung integrierbar sein.
81. Ein Bootplan DARF erst nach erfolgreicher Required Verification committed werden.
82. PCR Extend Operationen DÜRFEN NICHT als rollbackfähig behandelt werden.
83. Abgebrochene oder verworfene gemessene Bootpläne MÜSSEN im Measurement Model korrekt darstellbar sein.
84. Self-Healing MUSS neue Bootartefakte erneut verifizieren und messen.
85. Self-Healing DARF Trust- oder Measurement-Ergebnisse eines alten Artefaktsatzes NICHT auf einen neuen Satz übertragen.
86. Virtuelle Secure-Boot- und TPM-Provider MÜSSEN als virtualisierte Evidence erkennbar sein.
87. Die Architektur MUSS zukünftige Hardware Trust- und Attestation-Mechanismen integrieren können.
88. Secure Boot, Verified Boot und Measured Boot MÜSSEN ohne KI vollständig korrekt funktionieren.
89. KI DARF Required Security Policy NICHT überschreiben.
90. KI DARF keine Bootautorität allein aus probabilistischer Bewertung erzeugen.

---

## Konsequenzen

### Positive Konsequenzen

- klare Trennung zwischen Firmwarevertrauen, Ausführungsautorität und Messung,
- UEFI Secure Boot kann genutzt werden, ohne die gesamte NovaOS-Sicherheit daran zu koppeln,
- BIOS bleibt mit Verified und optional Measured Boot absicherbar,
- TPM-basierte Attestation wird möglich,
- Recovery, NovaDOS und Development Mode werden eindeutig unterscheidbar,
- Sealed Secrets können an autorisierte Bootzustände gebunden werden,
- Enterprise- und High-Security-Profile lassen sich fein konfigurieren,
- Boot Trust kann direkt in Nova.Trust fortgeführt werden,
- Systemgenerationen und Known-Good-Zustände werden attestierbar,
- zukünftige Hardware Trust Provider können integriert werden.

### Negative Konsequenzen

- zusätzliche Komplexität durch drei getrennte Security States,
- TPM- und Secure-Boot-Integration erzeugen plattformspezifischen Code,
- PCR-Layout und Measurement Ordering müssen langfristig stabil gepflegt werden,
- legitime Updates müssen mit Sealing- und Attestation-Policies koordiniert werden,
- Measurement Logs und Attestation erhöhen die Menge sicherheitskritischer Metadaten,
- Privacy und Data Sovereignty müssen bei Remote Attestation besonders berücksichtigt werden.

---

## Verworfene Alternativen

### Secure Boot als vollständige Boot Security

Verworfen.

Secure Boot schützt primär den Firmwareeinstieg und ersetzt nicht die interne NovaOS-Verifikation.

### Verified Boot ohne Measured Boot

Als vollständige Zielarchitektur verworfen.

Verified Boot bleibt zwar auch ohne TPM funktionsfähig, NovaOS soll zusätzlich hardwaregestützte Measurements unterstützen.

### Measured Boot als Autorisierungsmechanismus

Verworfen.

Messung dokumentiert Zustand, vergibt aber keine Autorität.

### TPM als zwingende Voraussetzung für jeden NovaOS-Boot

Verworfen.

NovaOS soll auch auf Systemen ohne TPM Verified Boot bereitstellen können, sofern die konfigurierte Policy dies erlaubt.

### Nur einen globalen `Secure = true/false` Zustand

Verworfen.

Damit könnten Secure Boot, Verified Boot, Measurement und Overrides nicht korrekt unterschieden werden.

### Measurement nur des Kernelimages

Verworfen.

Bootmanifest, sicherheitsrelevante Konfiguration und frühe Module beeinflussen den tatsächlichen Systemzustand ebenfalls.

### Remote Attestation als Pflicht

Verworfen.

Der lokale Boot muss offline und ohne externe Infrastruktur funktionieren.

### Cloudbasierte zentrale Bootfreigabe

Verworfen.

Dies widerspricht Offline-Fähigkeit und Data Sovereignty als Baseline.

### KI-basierte Sicherheitsfreigabe

Verworfen.

Kryptografische Bootautorität muss deterministisch und policybasiert bleiben.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-BOOT-0001_Parallele_BIOS_und_UEFI_Bootunterstützung`
- `ADR-BOOT-0002_UEFI_bevorzugen_mit_BIOS_Fallback`
- `ADR-BOOT-0003_Mehrstufige_Bootarchitektur`
- `ADR-BOOT-0004_ELF_als_native_Kernel_Ladeform`
- `ADR-BOOT-0005_Vertrauenswürdige_Bootkette`
- `ADR-HAL-0003_Firmware_über_Platform_Provider_abstrahieren`
- `ADR-HAL-0006_DMA_und_IOMMU_als_kontrollierte_Systemressourcen`
- `ADR-ARCH-0002_Mechanism_Policy_Separation`
- `ADR-ARCH-0003_Deklaratives_Systemmodell_als_Architekturgrundlage`
- `ADR-ARCH-0004_Transaktionale_Systemoperationen`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0006_Zero_Copy_als_Architekturprinzip`
- `ADR-ARCH-0010_Structured_Concurrency`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0013_Architecture_Introspection`
- `ADR-ARCH-0014_Explizite_Contracts_zwischen_Systemkomponenten`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-ARCH-0019_Evolvierbare_Systemarchitektur`
- `ADR-ARCH-0020_KI_nicht_als_Korrektheitsvoraussetzung_des_Kernels`
- `ADR-TRUST-0001_Nova_Trust`

---

## Zugehörige NPSPECs

- `NPSPEC-BOOT-SECURITY-INTEGRATION-0001`
- `NPSPEC-BOOT-SECUREBOOT-0001`
- `NPSPEC-BOOT-VERIFIED-BOOT-0001`
- `NPSPEC-BOOT-MEASURED-BOOT-0001`
- `NPSPEC-BOOT-SECURITY-STATE-0001`
- `NPSPEC-BOOT-SECURITY-POLICY-0001`
- `NPSPEC-BOOT-SECURITY-PROFILE-0001`
- `NPSPEC-BOOT-TPM-0001`
- `NPSPEC-BOOT-TPM-PCR-0001`
- `NPSPEC-BOOT-MEASUREMENT-PROVIDER-0001`
- `NPSPEC-BOOT-MEASUREMENT-EVENT-0001`
- `NPSPEC-BOOT-MEASUREMENT-LOG-0001`
- `NPSPEC-BOOT-MEASUREMENT-ORDERING-0001`
- `NPSPEC-BOOT-MEASUREMENT-HASH-0001`
- `NPSPEC-BOOT-ATTESTATION-0001`
- `NPSPEC-BOOT-LOCAL-ATTESTATION-0001`
- `NPSPEC-BOOT-REMOTE-ATTESTATION-0001`
- `NPSPEC-BOOT-SECRET-SEALING-0001`
- `NPSPEC-BOOT-SECURITY-HANDOFF-0001`
- `NPSPEC-BOOT-SECURITY-INTROSPECTION-0001`
- `NPSPEC-BOOT-SECURITY-AUDIT-0001`
- `NPSPEC-BOOT-SECURITY-TEST-0001`

---

## Verifikation

Die Umsetzung muss mindestens folgende Szenarien prüfen:

- UEFI Secure Boot aktiviert,
- UEFI Secure Boot deaktiviert,
- Plattform ohne Secure Boot,
- BIOS mit Verified Boot,
- BIOS mit TPM-basiertem Measured Boot,
- gültig signierter Kernel,
- ungültige Kernel-Signatur,
- manipuliertes Bootmanifest,
- verifizierter Kernel mit erfolgreicher Messung,
- verifizierter Kernel mit fehlgeschlagenem TPM Extend,
- `MeasuredBoot = Required` ohne TPM,
- `MeasuredBoot = Preferred` ohne TPM,
- TPM 2.0 verfügbar,
- vTPM verfügbar,
- mehrere Hashbanken,
- deterministische PCR-Reihenfolge,
- Measurement Log Replay gegen PCR-Endzustand,
- manipuliertes Measurement Log,
- Bootkonfiguration verändert,
- Development Mode,
- manueller Trust Override,
- Recovery Boot,
- NovaDOS Boot,
- Known-Good-Generation,
- Candidate-Generation,
- Self-Healing-Wechsel auf Known-Good,
- Sealed Disk Key,
- Update auf neue autorisierte Generation,
- Remote Attestation mit Nonce,
- Local Attestation,
- Offline Verified/Measured Boot,
- deaktivierte Netzwerkverbindung,
- Measurement Continuity nach Kernel Handoff,
- DMA-Manipulationsversuch zwischen Verification und Execution,
- paralleles Hashing mit deterministischem Measurement Commit,
- unbekannter Hardware Measurement Provider,
- inkonsistenter Boot Security Handoff.

---

## Ergebnis

NovaOS behandelt Secure Boot, Verified Boot und Measured Boot nicht als Synonyme, sondern als drei miteinander kombinierbare Sicherheitsmechanismen.

Das Gesamtmodell lautet:

```text
Firmware
   ↓
Secure Boot
   ↓
Nova Boot Entry
   ↓
Verified Boot
   ↓
Authorized Boot Plan
   ↓
Measured Boot
   ↓
Hardware Measurement State
   ↓
Kernel Handoff
   ↓
Nova.Trust
```

Dabei gilt:

```text
Secure Boot protects
who may enter.

Verified Boot decides
what may execute.

Measured Boot records
what actually happened.
```

Die zentrale Architekturregel lautet:

```text
Authorize explicitly.

Measure deterministically.

Attest when required.

Never confuse evidence
with permission.
```