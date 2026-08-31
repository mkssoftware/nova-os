# ADR-BOOT-0005 – Vertrauenswürdige Bootkette

## Status

Angenommen

## Kategorie

Boot / Security / Trust / Verified Boot / Measured Boot / Integrität / Recovery

## Kontext

Der Bootprozess besitzt eine besondere Sicherheitsstellung.

Bevor der NovaOS-Kernel seine eigenen Sicherheitsmechanismen, Capability-Prüfungen, Isolation, Treiberregeln und Nova.Trust vollständig aktivieren kann, müssen bereits mehrere Komponenten ausgeführt werden.

Dazu gehören abhängig von Plattform und Firmware:

- Firmware,
- BIOS- oder UEFI-Boot Entry,
- frühe Bootstufen,
- Nova Boot Core,
- Bootkonfiguration,
- Kernelimage,
- Bootmodule,
- Recovery-Komponenten,
- NovaDOS.

Wenn eine dieser Komponenten manipuliert wird, kann sie nachfolgende Sicherheitsmechanismen umgehen oder verfälschen.

Ein später vollständig vertrauenswürdiger Kernel kann einen kompromittierten früheren Bootpfad nicht automatisch rückwirkend absichern.

Deshalb benötigt NovaOS eine explizite **vertrauenswürdige Bootkette**.

Dabei müssen mehrere unterschiedliche Konzepte getrennt werden:

```text
Integrity
Authenticity
Trust
Authorization
Measurement
Execution
```

Ein unverändertes Artefakt ist nicht automatisch vertrauenswürdig.

Ein korrekt signiertes Artefakt ist nicht automatisch für jeden Bootkontext autorisiert.

Eine TPM-Messung verhindert nicht automatisch die Ausführung.

Secure Boot allein ersetzt kein vollständiges NovaOS-Trust-Modell.

---

## Entscheidung

NovaOS implementiert eine durchgängige, überprüfbare und firmwareunabhängige **Trusted Boot Chain**.

Jede sicherheitsrelevante Bootstufe muss den nächsten auszuführenden Bootbestandteil anhand expliziter Identitäts-, Integritäts-, Signatur-, Trust- und Policy-Regeln validieren, bevor Kontrolle oder Autorität übertragen wird.

Das Grundmodell lautet:

```text
Root of Trust
     │
     ▼
Boot Entry
     │
     ▼
Boot Stage
     │
     ▼
Nova Boot Core
     │
     ▼
Boot Configuration
     │
     ▼
Kernel / Modules / Recovery
     │
     ▼
Kernel Trust Initialization
```

Für jeden Übergang gilt:

```text
Identify
   ↓
Verify Integrity
   ↓
Verify Authenticity
   ↓
Evaluate Trust
   ↓
Evaluate Policy
   ↓
Authorize Execution
   ↓
Transfer Control
```

Damit gilt:

```text
Execution is the result of trust evaluation.

Presence is not trust.

Signature is not authority.
```

---

## Grundprinzip

Die zentrale Regel lautet:

```text
No executable boot artifact
receives control
without explicit validation.
```

und:

```text
Trust must propagate through the boot chain.

It must never be assumed.
```

---

## Beziehung zu Nova.Trust

Die Bootkette verwendet dieselben grundlegenden Vertrauenskonzepte wie die spätere Nova.Trust-Architektur.

Das konzeptionelle Modell lautet:

```text
Identity
   ↓
Signature
   ↓
Provenance
   ↓
Trust
   ↓
Permission
   ↓
Capability
   ↓
Audit
```

Während des frühen Bootvorgangs steht jedoch möglicherweise noch nicht die vollständige spätere Trust-Infrastruktur zur Verfügung.

Der Bootloader benötigt daher eine minimale eigenständige Trust-Implementierung, deren Semantik mit Nova.Trust kompatibel bleibt.

---

## Boot Trust Root

Jede vertrauenswürdige Bootkette benötigt mindestens einen definierten Ausgangspunkt.

Dieser wird als:

```text
Root of Trust
```

bezeichnet.

Mögliche Roots of Trust sind beispielsweise:

- fest eingebaute NovaOS Public Keys,
- Plattformschlüssel,
- UEFI Secure Boot Trust Store,
- TPM-gebundene Schlüssel,
- administrativ provisionierte Root Keys,
- hardwaregestützte Platform Roots.

NovaOS darf mehrere Roots of Trust unterstützen.

---

## Keine einzelne zwingende Root-of-Trust-Technologie

NovaOS darf die gesamte Bootarchitektur nicht von einer einzigen Technologie abhängig machen.

Insbesondere darf die Vertrauenskette nicht ausschließlich abhängen von:

```text
UEFI Secure Boot
```

oder:

```text
TPM
```

Diese Mechanismen können die Bootkette stärken, sind aber nicht deren alleinige semantische Grundlage.

---

## Firmwareunabhängige Vertrauensprüfung

Die grundlegende NovaOS-Artefaktvalidierung muss sowohl unter:

```text
BIOS
```

als auch unter:

```text
UEFI
```

funktionieren.

Damit gilt:

```text
UEFI Secure Boot available
    ↓
additional protection

UEFI Secure Boot unavailable
    ↓
NovaOS verification still required
```

---

## Vertrauenskette

Konzeptionell:

```text
Trusted Root
      ↓ verifies
Boot Component A
      ↓ verifies
Boot Component B
      ↓ verifies
Boot Configuration
      ↓ verifies
Kernel
      ↓ verifies / initializes
System Components
```

Jede Stufe übernimmt nur dann Kontrolle, wenn ihre Vorgängerstufe die erforderlichen Prüfungen erfolgreich abgeschlossen hat.

---

## Chain-of-Trust-Eigenschaft

Eine spätere Stufe darf nicht automatisch als vertrauenswürdig gelten, nur weil eine frühere Stufe vertrauenswürdig war.

Jeder relevante neue Code oder sicherheitskritische Datensatz muss separat validiert werden.

Beispiel:

```text
Trusted Bootloader
```

bedeutet nicht automatisch:

```text
Any kernel loaded by this bootloader is trusted.
```

---

## Bootartefakte

Mindestens folgende Artefakte müssen in das Trust-Modell integrierbar sein:

```text
Bootloader stages
Nova Boot Core
Boot configuration
Kernel image
Initial modules
Recovery image
NovaDOS image
Trust metadata
Update metadata
Known-Good metadata
```

Weitere Bootartefakte können ergänzt werden.

---

## Artefaktidentität

Jedes vertrauensrelevante Bootartefakt soll eine eindeutige logische Identität besitzen.

Beispiel:

```text
ArtifactIdentity {
    type
    name
    version
    buildId
    architecture
}
```

Identität und Dateiname sind nicht dasselbe.

---

## Integrität

Integrität beantwortet die Frage:

```text
Are these bytes unchanged?
```

Sie wird typischerweise über kryptografische Hashes abgesichert.

Beispiele:

```text
SHA-256
SHA-384
```

Die konkret zulässigen Algorithmen werden separat spezifiziert.

---

## Authentizität

Authentizität beantwortet:

```text
Who authorized or produced this artifact?
```

Sie wird typischerweise durch digitale Signaturen nachgewiesen.

Beispiel:

```text
Artifact
   ↓
Cryptographic Hash
   ↓
Signature
   ↓
Public Key
```

---

## Trust

Ein gültiger kryptografischer Signaturnachweis bedeutet nicht automatisch:

```text
trusted for execution
```

NovaOS bewertet zusätzlich, ob der Signierer beziehungsweise die Provenance für den konkreten Bootkontext vertrauenswürdig ist.

---

## Authorization

Nach Trust Evaluation folgt die Frage:

```text
May this artifact execute here?
```

Beispiel:

Ein gültig signierter Development Kernel darf auf einem Production-System trotzdem nicht autorisiert sein.

---

## Trust Policy

Eine Boot Trust Policy kann konzeptionell enthalten:

```text
BootTrustPolicy {
    trustedRoots
    allowedSigners
    requiredSignatures
    allowedArtifactTypes
    minimumVersions
    revokedKeys
    revokedArtifacts
    developmentMode
    recoveryRules
}
```

Die konkrete Struktur wird in NPSPECs definiert.

---

## Signaturpflicht

Sicherheitskritische native Bootartefakte sollen im normalen vertrauenswürdigen Bootmodus signiert sein.

Mindestens der produktive:

```text
Nova Boot Core
Kernel
Recovery Environment
```

muss durch die konfigurierte Trust Policy prüfbar sein.

---

## Mehrfachsignaturen

NovaOS darf mehrere Signaturen pro Artefakt unterstützen.

Dies ermöglicht beispielsweise:

```text
Vendor Signature
+
Release Signature
+
Enterprise Signature
```

Die Policy entscheidet, welche Kombination erforderlich ist.

---

## Threshold Trust

Zukünftig darf eine Policy verlangen:

```text
2 of 3 trusted signatures
```

oder ähnliche Threshold-Modelle.

Dies ist besonders für kritische System- oder Enterprise-Deployments sinnvoll.

---

## Key Rotation

Boot Trust Roots und Signing Keys müssen rotierbar sein.

Der Prozess muss ermöglichen:

```text
Old Key
   ↓
Trust Transition
   ↓
New Key
```

ohne die Bootfähigkeit legitimer Systeme unnötig zu zerstören.

---

## Schlüsselversionierung

Keys sollen Identität und Version beziehungsweise Generation besitzen.

Dadurch können ältere Schlüssel gezielt:

- deprecated,
- eingeschränkt,
- widerrufen

werden.

---

## Key Revocation

Kompromittierte Schlüssel müssen widerrufbar sein.

Eine Revocation List beziehungsweise semantisch äquivalente Struktur muss vor dem Boot relevanter Artefakte ausgewertet werden können.

---

## Artifact Revocation

Nicht nur Schlüssel, sondern auch bestimmte Artefakte oder Builds müssen widerrufbar sein.

Beispiel:

```text
Kernel Build X
    signed correctly
    but known vulnerable
```

kann trotzdem blockiert werden.

---

## Minimum Version

Eine Trust Policy darf Mindestversionen definieren.

Beispiel:

```text
MinimumKernelSecurityVersion = 42
```

Dadurch können bekannte unsichere ältere Generationen trotz gültiger Signatur abgelehnt werden.

---

## Anti-Rollback

NovaOS soll optional Schutz vor unautorisiertem Downgrade unterstützen.

Ein Angreifer darf nicht einfach eine ältere, korrekt signierte, aber verwundbare Kernelversion booten, wenn die Policy dies verbietet.

---

## Autorisierter Rollback

Known-Good- und Recovery-Funktionen benötigen gleichzeitig kontrollierten Rollback.

Daher gilt:

```text
Rollback
```

ist nicht generell verboten.

Er muss:

- explizit autorisiert,
- policykonform,
- nachvollziehbar

sein.

---

## Known-Good State

Known-Good-Generationen müssen selbst vollständig durch die Trust Policy validiert werden.

Es gilt nicht:

```text
Known-Good
=
skip verification
```

sondern:

```text
Known-Good
=
previously validated candidate
that must still satisfy current policy
```

---

## Recovery Trust

Recovery-Komponenten besitzen hohe Autorität und müssen deshalb mindestens genauso streng behandelt werden wie der normale Bootpfad.

Ein Recovery-System darf nicht als unsignierter Hintereingang existieren.

---

## NovaDOS Trust

NovaDOS wird ebenfalls als privilegiertes Boot Target betrachtet.

Es muss:

- identifizierbar,
- integritätsgeschützt,
- authentifizierbar,
- policyautorisiert

sein.

---

## Boot Configuration Trust

Nicht nur ausführbare Images können die Sicherheit beeinflussen.

Eine Bootkonfiguration kann beispielsweise:

- alternativen Kernel wählen,
- Sicherheitsoptionen deaktivieren,
- Debugmodi aktivieren,
- Module hinzufügen,
- Recoverypfade ändern.

Daher muss auch sicherheitsrelevante Bootkonfiguration in das Trust-Modell einbezogen werden.

---

## Signierte Konfiguration

Produktive Bootkonfiguration darf signiert oder durch ein signiertes Manifest geschützt werden.

Dadurch wird verhindert, dass ein Angreifer zwar keinen Kernel verändert, aber die Auswahl auf einen anderen unautorisierten Kernel umbiegt.

---

## Manifest-Modell

NovaOS darf ein signiertes Boot Manifest verwenden.

Konzeptionell:

```text
BootManifest {
    version
    policy
    kernelHash
    moduleHashes
    configurationHash
    generation
    architecture
}
```

Signiert wird dann das Manifest beziehungsweise eine kanonische Darstellung davon.

---

## Direkte Artefaktsignatur vs Manifest

NovaOS darf beide Ansätze unterstützen:

```text
Artifact Signature
```

und:

```text
Signed Manifest
```

Die Policy kann definieren, welche Variante verwendet wird.

---

## Bindung aller kritischen Bestandteile

Bei Manifest-basiertem Boot müssen alle sicherheitsrelevanten Bestandteile kryptografisch an das Manifest gebunden sein.

Ein Angreifer darf beispielsweise kein initiales Modul austauschen, während nur der Kernelhash geprüft wird.

---

## Bootmodule

Initiale Kernelmodule, Initramfs-ähnliche Images oder frühe Capability Provider müssen in die Vertrauenskette aufgenommen werden, wenn sie vor vollständiger Kernel-Sicherheitsinitialisierung geladen werden.

---

## Vertrauensstatus im BootInfo

Der Kernel erhält den festgestellten Boot Trust State über `BootInfo`.

Konzeptionell:

```text
BootTrustState {
    mode
    verified
    measured
    secureBootState
    rootIdentity
    manifestIdentity
    violations
}
```

Die konkrete Struktur wird separat definiert.

---

## Kernel prüft Übergabe

Der Kernel darf nicht blind jedem vom Bootloader behaupteten Trust Status vertrauen.

Soweit technisch möglich muss der Kernel:

- BootInfo-Struktur validieren,
- Trust-Metadaten prüfen,
- Messwerte übernehmen,
- eigene Trust Initialization durchführen.

Die Grenze zwischen Bootloader- und Kernelvertrauen wird explizit definiert.

---

## Measured Boot

NovaOS unterstützt neben Verified Boot auch **Measured Boot**.

Das Modell lautet:

```text
Boot Artifact
      ↓
Hash
      ↓
Measurement Register / Log
```

Measured Boot dokumentiert, was tatsächlich gestartet wurde.

---

## Verified Boot vs Measured Boot

Die beiden Mechanismen besitzen unterschiedliche Aufgaben.

```text
Verified Boot:
    decides whether execution is allowed

Measured Boot:
    records what was executed
```

Ein gemessenes Artefakt kann trotzdem nicht vertrauenswürdig sein.

Eine Messung allein darf keine Ausführungsautorität erzeugen.

---

## TPM

Wenn ein TPM verfügbar ist, kann NovaOS PCRs beziehungsweise entsprechende vertrauenswürdige Register für Bootmessungen verwenden.

Beispiel:

```text
Bootloader hash
    ↓ extend
PCR

Kernel hash
    ↓ extend
PCR
```

Die genaue PCR-Zuordnung wird separat spezifiziert.

---

## TPM nicht verfügbar

Ein fehlendes TPM darf den grundlegenden Verified-Boot-Pfad nicht zwangsläufig verhindern.

Der Trust State muss jedoch transparent anzeigen können:

```text
MeasuredBoot = Unavailable
```

wenn eine Policy keine TPM-Pflicht besitzt.

---

## TPM als Policy Requirement

High-Security-Deployments dürfen dagegen explizit verlangen:

```text
TPM Required
```

Dann führt fehlendes oder fehlerhaftes TPM zu einem Policy Failure.

---

## Measurement Log

Bootmessungen sollen zusätzlich zu hardwaregestützten Registern in einem strukturierten Eventlog aufgezeichnet werden können.

Beispiel:

```text
BootMeasurement {
    sequence
    artifact
    hashAlgorithm
    digest
    measurementTarget
}
```

---

## Attestation

Measured Boot kann später für Remote oder Local Attestation verwendet werden.

Beispiel:

```text
Platform
    ↓
Attestation Evidence
    ↓
Verifier
```

Attestation ist jedoch kein zwingender Bestandteil des lokalen Baseline-Boots.

---

## UEFI Secure Boot

Auf UEFI-Systemen darf Secure Boot als vorgelagerte Trust-Schicht genutzt werden.

Konzeptionell:

```text
UEFI Secure Boot
      ↓
NovaOS EFI Loader
      ↓
NovaOS Trusted Boot Chain
```

Secure Boot schützt damit insbesondere den ersten NovaOS-Einstieg.

Die interne NovaOS-Verifikation bleibt zusätzlich bestehen.

---

## Kein Ersatz durch Secure Boot

Es gilt ausdrücklich nicht:

```text
UEFI Secure Boot enabled
    ↓
skip NovaOS kernel verification
```

Der NovaOS Loader muss seine eigenen Artefakt- und Policyprüfungen weiterhin durchführen.

---

## BIOS Boot

Unter BIOS existiert typischerweise keine standardisierte Secure-Boot-Funktion.

NovaOS muss deshalb seine eigene Trust Chain ab dem frühestmöglichen vertrauenswürdigen Loaderpunkt etablieren.

---

## BIOS Stage 1

Auf klassischen BIOS-Systemen kann der allererste Bootsektor aufgrund seiner Größe nur begrenzte kryptografische Funktionalität besitzen.

Daher darf das Vertrauensmodell gestuft aufgebaut werden.

Beispiel:

```text
Minimal immutable/trusted bootstrap
      ↓
load verification-capable stage
      ↓
verify Shared Boot Core
```

Die genaue Root-of-Trust-Ausgestaltung wird plattformspezifisch definiert.

---

## Hardware Root of Trust

Zukünftige Systeme dürfen hardwaregestützte Roots of Trust verwenden.

Beispiele:

- Boot ROM,
- TPM,
- Platform Security Processor,
- Secure Element,
- hardwaregestützte Schlüsselablage.

Diese werden hinter gemeinsamen Trust-Semantiken integriert.

---

## Fail-Safe Default

Wenn die erforderliche Vertrauensprüfung nicht erfolgreich abgeschlossen werden kann, gilt:

```text
Do not execute.
```

Nicht:

```text
Could not verify
    ↓
assume valid
```

---

## Unknown Trust

Ein unbekannter Trust-Zustand wird nicht mit `Trusted` gleichgesetzt.

Mögliche semantische Zustände sind:

```text
Trusted
Untrusted
Unknown
Revoked
Invalid
```

`Unknown` ist keine Berechtigung.

---

## Security Downgrade

Ein fehlgeschlagener vertrauenswürdiger Boot darf nicht automatisch auf einen schwächeren ungeprüften Pfad wechseln.

Beispiel:

```text
Signature invalid
    ↓
boot unsigned fallback
```

ist im normalen Secure Mode verboten.

---

## Recovery bei Trust Failure

Bei Trust Failure darf NovaOS stattdessen:

- Known-Good-System auswählen,
- Recovery Environment starten,
- Diagnose anzeigen,
- explizite administrative Freigabe verlangen,
- Boot verweigern.

Die zulässige Aktion hängt von der Policy ab.

---

## Development Mode

NovaOS darf einen Development Mode unterstützen, in dem weniger strikte Signing-Regeln gelten.

Dieser Modus muss:

- explizit aktiviert,
- sichtbar,
- auditierbar,
- von Production Trust getrennt

sein.

---

## Keine stille Aktivierung

Development Mode darf nicht automatisch aktiviert werden, nur weil eine Signaturprüfung fehlschlägt.

---

## Development Keys

Development Builds dürfen eigene Development Trust Roots besitzen.

Diese dürfen auf einem Production-System standardmäßig nicht vertrauenswürdig sein.

---

## User-Authorized Boot

Ein System darf optional einen manuellen Bootpfad erlauben, bei dem ein autorisierter Benutzer bewusst ein ansonsten nicht freigegebenes Image startet.

Dies ist eine separate Policyentscheidung.

Dabei muss der unsichere Zustand deutlich markiert und im Trust State erfasst werden.

---

## Physical Presence

Für besonders kritische Trust Overrides darf physische Anwesenheit verlangt werden.

Beispiele:

```text
local console confirmation
firmware confirmation
hardware button
```

Dies reduziert das Risiko rein entfernter Trust-Downgrades.

---

## Trust Override

Ein Trust Override muss:

- explizit,
- zeitlich beziehungsweise scope-begrenzt,
- protokolliert,
- nicht automatisch persistent

sein, sofern die Policy nichts anderes vorsieht.

---

## Schlüsselablage

Private Signing Keys gehören grundsätzlich nicht auf das Zielsystem.

Der Bootloader benötigt primär:

- Public Keys,
- Certificates,
- Revocation Data,
- Policy Metadata.

Private Schlüssel verbleiben in kontrollierten Signing-Umgebungen.

---

## Vertrauensspeicher

Der Boot Trust Store muss manipulationsgeschützt gespeichert werden.

Mögliche Speicherorte sind:

- signierte Trust-Daten,
- Firmware Trust Store,
- TPM/NV Storage,
- hardwaregeschützte Plattformbereiche,
- transaktional geschützte NovaOS-Systemmetadaten.

---

## Trust-Store-Update

Änderungen am Trust Store sind sicherheitskritische Transaktionen.

Sie müssen:

```text
Authenticate
    ↓
Validate
    ↓
Stage
    ↓
Commit
    ↓
Verify
```

durchlaufen.

Ein unvollständiges Update darf den Trust Store nicht in einen undefinierten Zustand versetzen.

---

## Transaktionale Key Rotation

Key Rotation soll transaktional erfolgen.

Beispiel:

```text
Trust old + new key
      ↓
deploy new artifacts
      ↓
verify successful boot
      ↓
revoke old key
```

Damit wird ein unbeabsichtigtes Aussperren verhindert.

---

## Recovery Keys

Optional können spezielle Recovery Signing Keys existieren.

Diese müssen enger begrenzte Autorität besitzen.

Beispiel:

```text
RecoveryKey
    may sign Recovery Environment
    may not sign Production Kernel
```

Dies folgt dem Least-Authority-Prinzip.

---

## Key Scope

Signing Keys sollen nach Verwendungszweck getrennt werden können.

Beispiele:

```text
Bootloader Signing
Kernel Signing
Recovery Signing
Development Signing
Enterprise Signing
```

Ein kompromittierter Recovery-Key soll nicht automatisch Production-Kernel autorisieren.

---

## Algorithm Agility

Die Bootkette darf nicht dauerhaft auf einen einzigen kryptografischen Algorithmus festgelegt werden.

Unterstützte:

- Hashverfahren,
- Signaturalgorithmen,
- Schlüssellängen

müssen versionierbar und evolvierbar sein.

---

## Kein unsicherer Algorithmus-Fallback

Wenn ein erforderlicher moderner Algorithmus nicht unterstützt wird, darf NovaOS nicht automatisch auf einen kryptografisch schwächeren Algorithmus wechseln.

---

## Algorithm Policy

Die Trust Policy darf definieren:

```text
AllowedAlgorithms
MinimumSecurityLevel
DeprecatedAlgorithms
ForbiddenAlgorithms
```

---

## Canonicalization

Signierte Metadaten müssen eine eindeutige kanonische Darstellung besitzen.

Andernfalls könnten semantisch ähnliche, aber binär unterschiedlich interpretierte Strukturen Signaturprobleme oder Ambiguitäten erzeugen.

---

## Parser Security

Alle Trust-Metadaten sind als potenziell feindliche Eingaben zu behandeln.

Parser müssen:

- Längen prüfen,
- Offsets prüfen,
- Integer Overflow verhindern,
- unbekannte kritische Felder sicher behandeln,
- keine unbounded Strukturen akzeptieren.

---

## Critical und Optional Fields

Versionierte Trust-Metadaten sollen zwischen:

```text
Critical field
```

und:

```text
Optional field
```

unterscheiden können.

Ein unbekanntes kritisches Feld führt zur Ablehnung.

Ein unbekanntes optionales Feld darf kontrolliert ignoriert werden.

---

## Boot Generation Binding

Eine Signatur beziehungsweise ein Manifest kann an eine bestimmte Systemgeneration gebunden sein.

Damit kann verhindert werden, dass Artefakte aus verschiedenen Generationen unsicher vermischt werden.

---

## Mix-and-Match-Schutz

NovaOS muss verhindern können:

```text
Kernel from generation A
+
modules from generation B
+
configuration from generation C
```

wenn diese Kombination nicht explizit autorisiert ist.

---

## Provenance

Bootartefakte sollen Provenance-Informationen besitzen können.

Beispiel:

```text
built by
signed by
released by
derived from
```

Diese Informationen unterstützen Trust Evaluation und Audit.

---

## Build Identity

Jedes produktive Kernel- und Bootloaderartefakt soll eine eindeutige Build Identity besitzen.

Beispiel:

```text
BuildId
Version
SourceRevision
ReleaseChannel
```

Nicht alle Felder müssen für alle Benutzer offengelegt werden.

---

## Reproducible Builds

Reproduzierbare Builds sind mit der Trust Chain kompatibel und erwünscht.

Ein reproduzierbarer Build erlaubt unabhängige Prüfung:

```text
Source
  ↓ build
Expected Hash
  ==
Released Artifact Hash
```

Dies stärkt Provenance, ersetzt aber keine Signatur- oder Policyprüfung.

---

## Runtime Trust Continuity

Die Boot Trust Chain endet nicht einfach am Kernel Entry.

Der Kernel übernimmt den Trust State und setzt ihn in der laufenden Nova.Trust-Architektur fort.

Konzeptionell:

```text
Boot Trust
    ↓
Kernel Trust Initialization
    ↓
System Trust Graph
```

---

## Initial Capability Authority

Der Bootloader darf dem Kernel nur die Autorität beziehungsweise Ressourcen übergeben, die explizit Teil des Handoff Contracts sind.

Ein validierter Kernel erhält nicht automatisch ungeprüfte Firmwareautorität.

---

## Boot Measurement Continuation

Der Kernel darf die Bootmessung nach Handoff fortsetzen.

Beispiele:

```text
critical drivers
system manifests
early system services
```

Damit kann eine umfassendere Attestation Chain entstehen.

---

## Secrets und Sealing

TPM oder andere Hardware Trust Roots dürfen Geheimnisse an einen bestimmten Bootzustand binden.

Beispiel:

```text
Disk Key
    sealed to
expected PCR state
```

Freigabe erfolgt nur bei passender Bootmessung beziehungsweise Policy.

---

## Kein Availability-Desaster

Sealing darf nicht so verwendet werden, dass legitime Updates das System irreparabel aussperren.

Key Migration, Recovery und neue autorisierte Bootgenerationen müssen berücksichtigt werden.

---

## Disk Encryption Integration

Die Trusted Boot Chain kann zur Freigabe verschlüsselter Systemvolumes beitragen.

Konzeptionell:

```text
Trusted Boot State
      ↓
Key Release Policy
      ↓
Decrypt System Volume
```

Der konkrete Verschlüsselungsmechanismus wird separat definiert.

---

## Boot Audit

Relevante Trust-Ereignisse sollen auditierbar sein.

Beispiele:

```text
signature accepted
signer identity
revoked artifact rejected
development override used
recovery key used
anti-rollback policy triggered
```

---

## Boot Trust Log

Der Bootloader soll einen strukturierten Trust Log führen können.

Beispiel:

```text
Artifact:
    kernel.elf

Identity:
    NovaOS Kernel 0.9

Integrity:
    Valid

Signature:
    Valid

Signer:
    Nova Release Key 4

Policy:
    Allowed

Result:
    Authorized
```

---

## Architecture Introspection

Nach dem Start soll der System Trust State introspektierbar sein.

Beispiel:

```text
Boot Trust:
    Verified

Measured Boot:
    Active

Firmware Secure Boot:
    Enabled

Kernel:
    Authorized

Boot Override:
    None
```

---

## Informationsfilterung

Nicht jede Anwendung darf vollständige:

- Key IDs,
- Zertifikatsketten,
- PCR-Werte,
- Sicherheitsmetadaten

sehen.

Architecture Introspection muss Capability- und Security-Filter berücksichtigen.

---

## Fehlerklassen

Trust-Fehler sollen typisiert sein.

Beispiele:

```text
IntegrityMismatch
SignatureMissing
SignatureInvalid
UnknownSigner
SignerRevoked
ArtifactRevoked
VersionRollbackDenied
PolicyRejected
MeasurementFailure
TrustStoreInvalid
UnsupportedAlgorithm
ManifestMismatch
```

---

## Kein generisches „Boot failed“

Die Fehlerursache soll intern klar differenziert werden.

Benutzerausgabe darf aus Sicherheitsgründen abstrahiert werden, während privilegierte Diagnose detaillierte Informationen erhält.

---

## Deterministic Mode

Trust Evaluation muss deterministisch sein.

Bei identischen:

- Artefakten,
- Trust Store,
- Policy,
- Zeit-/Versionseingaben

muss dieselbe Autorisierungsentscheidung entstehen.

KI oder probabilistische Modelle dürfen keine Bootautorität vergeben.

---

## Zeitabhängige Zertifikate

Falls Zertifikate mit Zeitgültigkeit verwendet werden, muss die Vertrauenswürdigkeit der verfügbaren Zeitquelle berücksichtigt werden.

Eine unzuverlässige Firmwareuhr darf nicht unkontrolliert zu falscher Vertrauensentscheidung führen.

---

## Offline-Boot

Der Baseline Trusted Boot muss vollständig offline funktionieren.

Eine Netzwerkverbindung darf nicht erforderlich sein, um einen bereits provisionierten lokalen NovaOS-Kernel zu validieren.

---

## Online Revocation

Online-Dienste dürfen zusätzliche Revocation- oder Transparency-Daten bereitstellen.

Sie dürfen die lokale Baseline jedoch nicht zur zwingenden Cloudabhängigkeit machen, sofern die Policy dies nicht explizit verlangt.

---

## Data Sovereignty

Boot Trust Data darf Data-Sovereignty-Anforderungen respektieren.

Enterprise- oder souveräne Systeme dürfen beispielsweise ausschließlich lokale Trust Roots und lokale Revocation-Daten verwenden.

---

## Recovery ohne Netzwerk

Known-Good und NovaDOS müssen auch ohne Netzwerk trust-verifiziert gestartet werden können.

---

## Boot Transparency

NovaOS darf zukünftig Transparency Logs für Release-Artefakte unterstützen.

Dadurch kann nachvollzogen werden, dass veröffentlichte Kernel- oder Bootloadergenerationen in einem unveränderbaren Veröffentlichungslog registriert wurden.

Dies ist optional und nicht Voraussetzung für den Baseline-Boot.

---

## Update Integration

Das Update-System muss neue Bootartefakte vor Aktivierung verifizieren.

Konzeptionell:

```text
Download / Receive
      ↓
Verify
      ↓
Stage
      ↓
Register Generation
      ↓
Boot Candidate
      ↓
Successful Boot
      ↓
Mark Known-Good
```

---

## Keine Aktivierung ungeprüfter Updates

Ein Update darf nicht allein deshalb in den aktiven Bootpfad gelangen, weil das Kopieren erfolgreich war.

Trust Validation und Generation Activation bleiben getrennt.

---

## First Boot Validation

Eine neu installierte Generation kann zunächst als:

```text
Candidate
```

gelten.

Nach erfolgreichem Boot und Health Check kann sie zu:

```text
Known-Good
```

werden.

Die Signaturprüfung bleibt bei jedem Boot beziehungsweise entsprechend der unveränderlichen Artefaktgarantie weiterhin gültig.

---

## Hot Replacement

Bootkomponenten, die außerhalb des laufenden Bootprozesses aktualisiert werden, folgen den allgemeinen Evolution- und Transaktionsprinzipien.

Die aktive Boot Trust Chain selbst wird während eines laufenden Bootvorgangs nicht unkontrolliert ausgetauscht.

---

## Fail-Safe Trust Store

Ist der Trust Store beschädigt oder inkonsistent, darf NovaOS nicht einfach alle Signaturen akzeptieren.

Mögliche Reaktionen:

```text
Use protected known-good trust store
Recovery
Administrative repair
Boot refusal
```

---

## Trust Store Generation

Trust Stores können generationenbasiert verwaltet werden.

Beispiel:

```text
TrustStoreGeneration N
      ↓
Validated Update
      ↓
TrustStoreGeneration N+1
```

Dadurch werden transaktionale Updates und Rollback erleichtert.

---

## Root-Key-Compromise

Für den extremen Fall eines kompromittierten Root Keys muss ein Recovery- beziehungsweise Root-Rotation-Modell vorgesehen werden.

Dieses darf nicht ausschließlich von demselben kompromittierten Schlüssel abhängig sein.

Mögliche Mechanismen werden separat spezifiziert.

---

## Break-Glass Recovery

High-Security-Systeme dürfen einen speziellen Break-Glass-Recovery-Pfad besitzen.

Dieser muss:

- bewusst aktiviert,
- stark geschützt,
- auditierbar,
- eingeschränkt

sein.

Er darf keinen normalen versteckten Bypass darstellen.

---

## Physischer Angriff

Die Trusted Boot Chain reduziert Manipulation von Softwareartefakten.

Sie schützt nicht vollständig gegen:

- manipulierte Hardware,
- invasive physische Angriffe,
- kompromittierte CPU-/Firmwareimplementierungen.

Solche Risiken werden über zusätzliche Hardware- und Plattform-Trust-Mechanismen adressiert.

---

## Firmware Trust

NovaOS darf Firmware nicht automatisch vollständig vertrauen.

Selbst wenn Firmware den ersten Code lädt, werden Firmwaredaten gemäß HAL-Regeln validiert.

Eine vertrauenswürdige Bootkette bedeutet nicht:

```text
all firmware input is correct
```

---

## DMA während des Boots

Geräte mit DMA-Fähigkeiten können eine Sicherheitsbedrohung für die Bootkette darstellen.

Soweit Plattform und Bootphase dies erlauben, sollen:

- unnötige Bus Master deaktiviert,
- IOMMU-Schutz aktiviert,
- DMA-Rechte begrenzt

werden.

Die vollständige DMA-/IOMMU-Architektur übernimmt später die HAL.

---

## Boot UI und Trust

Die grafische Bootoberfläche ist keine Trust Authority.

Eine UI darf einen Trust State anzeigen oder eine explizit erlaubte Benutzerentscheidung erfassen.

Sie darf jedoch Trust-Prüfungen nicht selbst definieren oder umgehen.

---

## Social Engineering Resistance

Security-Meldungen müssen klar zwischen:

```text
Verified
Unverified
Revoked
Development Mode
Override
```

unterscheiden.

Ein kritischer Trust Failure darf nicht als harmloser Hinweis dargestellt werden.

---

## AI-Unabhängigkeit

Die komplette Boot Trust Chain muss ohne KI funktionieren.

KI darf optional:

- Trust Logs analysieren,
- Anomalien korrelieren,
- Recoveryvorschläge erzeugen,
- kompromittierte Muster erkennen.

KI darf nicht:

- Signaturen ersetzen,
- Trust Roots erzeugen,
- unbekannte Artefakte autorisieren,
- Revocation ignorieren,
- Hard Security Policy überschreiben.

---

## Normative Anforderungen

1. NovaOS MUSS eine explizite vertrauenswürdige Bootkette implementieren.
2. Jeder sicherheitsrelevante Bootübergang MUSS auf validierten Trust-Entscheidungen basieren können.
3. Bootartefakte MÜSSEN eindeutig identifizierbar sein.
4. Integrität, Authentizität, Trust und Authorization MÜSSEN semantisch getrennt werden.
5. Ein gültiger Hash allein DARF KEINE Ausführungsautorität erzeugen.
6. Eine gültige Signatur allein DARF KEINE Ausführungsautorität erzeugen.
7. Trust MUSS gegen eine explizite Policy ausgewertet werden.
8. Produktive Kernelimages MÜSSEN durch eine Trust Policy verifizierbar sein.
9. Der Nova Boot Core MUSS durch die Trust Chain abgesichert sein können.
10. Recovery-Komponenten MÜSSEN trust-verifiziert werden können.
11. NovaDOS MUSS trust-verifiziert werden können.
12. Sicherheitsrelevante Bootkonfiguration MUSS in das Integritäts- und Trust-Modell integrierbar sein.
13. Initiale sicherheitsrelevante Module MÜSSEN in die Trust Chain integrierbar sein.
14. Die Trust Chain MUSS unter BIOS und UEFI funktionieren.
15. UEFI Secure Boot DARF als zusätzliche Schutzschicht verwendet werden.
16. UEFI Secure Boot DARF die interne NovaOS-Artefaktprüfung NICHT ersetzen.
17. Fehlendes Secure Boot DARF Verified Boot NICHT automatisch deaktivieren.
18. NovaOS MUSS mindestens eine definierte Root-of-Trust-Quelle besitzen.
19. Mehrere Roots of Trust MÜSSEN unterstützt werden können.
20. Die Bootarchitektur DARF NICHT ausschließlich von TPM oder UEFI Secure Boot abhängen.
21. Public-Key-basierte Artefaktverifikation MUSS unterstützt werden können.
22. Private Signing Keys DÜRFEN NICHT Bestandteil normaler Zielsysteme sein.
23. Signing Keys MÜSSEN widerrufbar sein.
24. Bestimmte Artefakte oder Builds MÜSSEN widerrufbar sein können.
25. Trust Policies MÜSSEN Mindestversionen definieren können.
26. Anti-Rollback MUSS policygesteuert unterstützt werden können.
27. Autorisierter Known-Good-Rollback MUSS trotz Anti-Rollback möglich bleiben.
28. Known-Good-Artefakte MÜSSEN weiterhin die aktuelle Trust Policy erfüllen.
29. Development Mode MUSS explizit vom Production Trust getrennt sein.
30. Development Mode DARF NICHT automatisch bei Trust Failure aktiviert werden.
31. Development Keys DÜRFEN standardmäßig keine Production-Autorität besitzen.
32. Trust Overrides MÜSSEN explizit sein.
33. Trust Overrides MÜSSEN auditierbar sein.
34. Trust Overrides SOLLEN scope- oder zeitlich begrenzbar sein.
35. Physische Anwesenheit DARF für kritische Overrides verlangt werden.
36. Boot Trust Stores MÜSSEN manipulationsgeschützt verwaltet werden.
37. Trust-Store-Updates MÜSSEN transaktional ausführbar sein.
38. Key Rotation MUSS ohne unnötigen Verlust legitimer Bootfähigkeit möglich sein.
39. Recovery Keys SOLLEN auf begrenzte Artefaktklassen einschränkbar sein.
40. Signing-Key-Autorität MUSS nach Zweck separierbar sein.
41. Die kryptografische Bootarchitektur MUSS Algorithm Agility unterstützen.
42. Unsichere kryptografische Downgrades DÜRFEN NICHT automatisch erfolgen.
43. Die Policy MUSS erlaubte und verbotene Algorithmen bestimmen können.
44. Signierte Metadaten MÜSSEN kanonisch interpretierbar sein.
45. Trust-Metadatenparser MÜSSEN defensiv implementiert sein.
46. Unbekannte kritische Trust-Metadaten MÜSSEN zur Ablehnung führen.
47. Unknown Trust DARF NICHT als Trusted behandelt werden.
48. Ein fehlgeschlagener Trust Check DARF NICHT automatisch einen ungeprüften Fallback starten.
49. Bei Trust Failure MUSS Recovery oder Bootverweigerung möglich sein.
50. Bootmanifeste MÜSSEN sicherheitsrelevante Artefakte eindeutig binden können.
51. Mix-and-Match zwischen nicht kompatiblen Systemgenerationen MUSS verhinderbar sein.
52. Bootartefakte SOLLEN Build Identity und Provenance tragen können.
53. Reproducible Builds SOLLEN mit dem Trust-Modell kompatibel sein.
54. Verified Boot und Measured Boot MÜSSEN als getrennte Mechanismen behandelt werden.
55. Measured Boot DARF KEINE Ausführungsautorität erzeugen.
56. TPM-basierte Messungen SOLLEN unterstützt werden können.
57. Fehlendes TPM DARF nur dann den Boot verhindern, wenn die Policy TPM verlangt.
58. Bootmessungen SOLLEN in einem strukturierten Measurement Log erfasst werden können.
59. Attestation SOLL auf Grundlage der Bootmessungen möglich sein können.
60. Der Boot Trust State MUSS an den Kernel übergeben werden können.
61. Der Kernel MUSS den übernommenen Trust State weiter in Nova.Trust integrieren können.
62. Boot Trust DARF nicht am Kernel Entry semantisch verloren gehen.
63. TPM-/Hardware-Sealing DARF an autorisierte Bootzustände gebunden werden.
64. Key-Sealing MUSS Update- und Recovery-Szenarien berücksichtigen.
65. Verschlüsselte Systemvolumes DÜRFEN Trust State zur Key-Freigabe verwenden.
66. Relevante Trust-Entscheidungen MÜSSEN auditierbar sein.
67. Trust-Fehler MÜSSEN typisiert behandelbar sein.
68. Trust Evaluation MUSS deterministisch sein.
69. Zeitabhängige Trust-Prüfung MUSS die Vertrauenswürdigkeit der Zeitquelle berücksichtigen.
70. Der Baseline Trusted Boot MUSS offline funktionieren.
71. Online-Revocation DARF optional ergänzt werden.
72. Netzwerkzugriff DARF NICHT implizite Voraussetzung für normalen lokalen Verified Boot sein.
73. Recovery und NovaDOS MÜSSEN offline verifizierbar sein.
74. Bootupdates MÜSSEN vor Aktivierung trust-verifiziert werden.
75. Kopieren eines Updates DARF NICHT automatisch dessen Bootautorität erzeugen.
76. Neue Systemgenerationen SOLLEN zunächst als Candidate behandelt werden können.
77. Ein erfolgreicher und verifizierter Candidate SOLL als Known-Good markiert werden können.
78. Ein beschädigter Trust Store DARF NICHT zu Allow-All-Verhalten führen.
79. Trust Stores SOLLEN generationenbasiert verwaltbar sein.
80. Root-Key-Recovery MUSS architektonisch möglich bleiben.
81. Break-Glass-Mechanismen MÜSSEN explizit, eingeschränkt und auditierbar sein.
82. Firmwaredaten MÜSSEN trotz vertrauenswürdigem Boot defensiv validiert werden.
83. DMA-Risiken während des Boots SOLLEN soweit möglich begrenzt werden.
84. Boot-UI DARF NICHT Trust Authority sein.
85. Kritische Trust-Zustände MÜSSEN eindeutig darstellbar sein.
86. Die Trusted Boot Chain MUSS ohne KI vollständig korrekt funktionieren.
87. KI DARF Hard Trust Policy NICHT überschreiben.
88. KI DARF unbekannte oder ungültige Bootartefakte NICHT autorisieren.
89. NovaOS MUSS Trust während des Boots als explizite Kette von überprüften Autoritätsübergängen behandeln.
90. Kein Bootartefakt DARF allein aufgrund seiner Existenz oder Erreichbarkeit Ausführungsrecht erhalten.

---

## Konsequenzen

### Positive Konsequenzen

- Manipulation von Kernel und Bootkomponenten wird früh erkannt.
- BIOS und UEFI erhalten ein gemeinsames NovaOS-Trust-Modell.
- Secure Boot kann genutzt werden, ohne NovaOS daran zu koppeln.
- Recovery und NovaDOS sind keine unkontrollierten Sicherheits-Bypässe.
- Key Rotation und Revocation sind von Anfang an architektonisch vorgesehen.
- Known-Good und Anti-Rollback lassen sich konsistent verbinden.
- Measured Boot und Attestation können später ergänzt werden.
- Boot Trust kann direkt in Nova.Trust übergehen.
- Enterprise- und High-Security-Policies lassen sich auf derselben Grundlage umsetzen.
- Verschlüsselung und Key Sealing können sicher an den Bootzustand gebunden werden.

### Negative Konsequenzen

- Bootloader und Update-Infrastruktur benötigen kryptografische Funktionen.
- Key- und Trust-Store-Management erhöhen die Komplexität.
- verlorene oder falsch konfigurierte Trust Keys können die Bootfähigkeit beeinträchtigen.
- Recoverypfade müssen ebenso streng verwaltet werden wie normale Bootpfade.
- Key Rotation und Anti-Rollback benötigen sorgfältige transaktionale Prozesse.
- TPM- und Secure-Boot-Integration erzeugen zusätzliche plattformspezifische Testfälle.

---

## Verworfene Alternativen

### Nur Hash-Prüfung

Verworfen.

Ein Hash weist Integrität nach, aber nicht, wer ein Artefakt autorisiert hat.

### Nur digitale Signaturen ohne Trust Policy

Verworfen.

Ein kryptografisch gültiger Signierer ist nicht automatisch für jeden Bootkontext autorisiert.

### Ausschließlich UEFI Secure Boot

Verworfen.

NovaOS unterstützt BIOS und soll außerdem eine eigene firmwareunabhängige Trust Chain besitzen.

### Ausschließlich TPM Measured Boot

Verworfen.

Measured Boot dokumentiert Ausführung, verhindert aber allein keine unautorisierte Ausführung.

### Recovery ohne Signaturprüfung

Verworfen.

Recovery besitzt besonders hohe Autorität und wäre sonst ein direkter Sicherheits-Bypass.

### Automatischer Unsigned Fallback bei Signaturfehler

Verworfen.

Ein Failure darf keine Autorität erzeugen.

### Ein permanenter unveränderlicher einzelner Signing Key

Verworfen.

Key Rotation und Revocation sind für langfristige Systemsicherheit erforderlich.

### Cloudpflicht für jeden Boot

Verworfen.

Der Baseline-Boot muss offline und souverän funktionieren.

### KI-basierte Vertrauensentscheidung

Verworfen.

Bootautorität muss auf deterministischen kryptografischen und policybasierten Mechanismen beruhen.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-BOOT-0001_Parallele_BIOS_und_UEFI_Bootunterstützung`
- `ADR-BOOT-0002_UEFI_bevorzugen_mit_BIOS_Fallback`
- `ADR-BOOT-0003_Mehrstufige_Bootarchitektur`
- `ADR-BOOT-0004_ELF_als_native_Kernel_Ladeform`
- `ADR-HAL-0003_Firmware_über_Platform_Provider_abstrahieren`
- `ADR-HAL-0006_DMA_und_IOMMU_als_kontrollierte_Systemressourcen`
- `ADR-ARCH-0002_Mechanism_Policy_Separation`
- `ADR-ARCH-0003_Deklaratives_Systemmodell_als_Architekturgrundlage`
- `ADR-ARCH-0004_Transaktionale_Systemoperationen`
- `ADR-ARCH-0013_Architecture_Introspection`
- `ADR-ARCH-0014_Explizite_Contracts_zwischen_Systemkomponenten`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-ARCH-0019_Evolvierbare_Systemarchitektur`
- `ADR-ARCH-0020_KI_nicht_als_Korrektheitsvoraussetzung_des_Kernels`
- `ADR-TRUST-0001_Nova_Trust`

---

## Zugehörige NPSPECs

- `NPSPEC-BOOT-TRUST-0001`
- `NPSPEC-BOOT-CHAIN-OF-TRUST-0001`
- `NPSPEC-BOOT-ROOT-OF-TRUST-0001`
- `NPSPEC-BOOT-ARTIFACT-IDENTITY-0001`
- `NPSPEC-BOOT-INTEGRITY-0001`
- `NPSPEC-BOOT-SIGNATURE-0001`
- `NPSPEC-BOOT-TRUST-POLICY-0001`
- `NPSPEC-BOOT-TRUST-STORE-0001`
- `NPSPEC-BOOT-KEY-MANAGEMENT-0001`
- `NPSPEC-BOOT-KEY-ROTATION-0001`
- `NPSPEC-BOOT-REVOCATION-0001`
- `NPSPEC-BOOT-ANTIROLLBACK-0001`
- `NPSPEC-BOOT-MANIFEST-0001`
- `NPSPEC-BOOT-VERIFIED-BOOT-0001`
- `NPSPEC-BOOT-MEASURED-BOOT-0001`
- `NPSPEC-BOOT-TPM-0001`
- `NPSPEC-BOOT-MEASUREMENT-LOG-0001`
- `NPSPEC-BOOT-ATTESTATION-0001`
- `NPSPEC-BOOT-SECUREBOOT-0001`
- `NPSPEC-BOOT-DEVELOPMENT-TRUST-0001`
- `NPSPEC-BOOT-TRUST-OVERRIDE-0001`
- `NPSPEC-BOOT-RECOVERY-TRUST-0001`
- `NPSPEC-BOOT-NOVADOS-TRUST-0001`
- `NPSPEC-BOOT-TRUST-HANDOFF-0001`
- `NPSPEC-BOOT-TRUST-AUDIT-0001`
- `NPSPEC-BOOT-TRUST-INTROSPECTION-0001`
- `NPSPEC-BOOT-TRUST-TEST-0001`

---

## Verifikation

Die Umsetzung muss mindestens folgende Szenarien prüfen:

- gültig signierter Kernel,
- manipuliertes Kernelimage,
- fehlende Signatur,
- ungültige Signatur,
- unbekannter Signierer,
- widerrufener Signierer,
- widerrufenes Kernelartefakt,
- Mindestversion unterschritten,
- blockierter Rollback,
- autorisierter Known-Good-Rollback,
- signiertes Bootmanifest,
- manipulierte Bootkonfiguration,
- manipuliertes Initialmodul,
- Mix-and-Match verschiedener Systemgenerationen,
- gültige Key Rotation,
- abgebrochene Key Rotation,
- beschädigter Trust Store,
- Known-Good Trust Store,
- BIOS Verified Boot,
- UEFI Verified Boot,
- UEFI Secure Boot aktiv,
- UEFI Secure Boot nicht verfügbar,
- TPM vorhanden,
- TPM nicht vorhanden,
- TPM als Required Policy,
- korrekte PCR-/Measurement-Erweiterung,
- Measurement Log,
- Development Mode,
- Development Key auf Production-System,
- expliziter Trust Override,
- Recovery Trust,
- NovaDOS Trust,
- offline Verified Boot,
- Key Sealing für Systemverschlüsselung,
- Trust State Übergabe an den Kernel,
- Trust Audit,
- unbekanntes kritisches Manifestfeld,
- ungültiger kryptografischer Algorithmus,
- Security-Downgrade-Versuch,
- Fault Injection in Trust-Store- und Manifestparser.

---

## Ergebnis

NovaOS behandelt den Bootprozess nicht nur als technische Kette von Loadern, sondern als Kette expliziter Vertrauens- und Autoritätsübergänge.

Das Modell lautet:

```text
Root of Trust
      ↓
Trusted Boot Entry
      ↓
Verified Boot Stage
      ↓
Verified Nova Boot Core
      ↓
Verified Configuration
      ↓
Verified Kernel + Modules
      ↓
Kernel Trust Initialization
      ↓
Nova.Trust
```

Dabei gilt:

```text
Integrity proves bytes.

Signature proves origin.

Trust evaluates the signer.

Policy grants authority.

Measurement records what happened.
```

Die zentrale Architekturregel lautet:

```text
Never transfer boot authority
without verification.

Never treat a signature
as automatic permission.

Never let failure
weaken the trust boundary.

Trust must be established,
propagated,
measured
and auditable
from the first boot stage
to the running system.
```