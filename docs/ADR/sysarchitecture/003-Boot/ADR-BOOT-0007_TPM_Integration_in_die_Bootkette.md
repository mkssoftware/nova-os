# ADR-BOOT-0007 – TPM Integration in die Bootkette

## Status

Angenommen

## Kategorie

Boot / Security / TPM / Measured Boot / Trust / Attestation / Key Sealing / Hardware Root of Trust

## Kontext

Mit:

- `ADR-BOOT-0005_Vertrauenswürdige_Bootkette`
- `ADR-BOOT-0006_Secure_Measured_Verified_Boot_Integration`

ist festgelegt, dass NovaOS eine vertrauenswürdige Bootkette sowie die getrennte Integration von Secure Boot, Verified Boot und Measured Boot verwendet.

Für hardwaregestützte Messungen, Attestation und die kontrollierte Freigabe von Geheimnissen benötigt NovaOS auf klassischen PC-Plattformen eine standardisierte Trusted-Computing-Schnittstelle.

Hierfür ist TPM 2.0 der primäre Mechanismus.

Ein TPM kann insbesondere:

- kryptografische Schlüssel geschützt erzeugen und speichern,
- Platform Configuration Registers bereitstellen,
- Bootzustände durch PCR Extend messen,
- Messzustände kryptografisch signieren,
- Geheimnisse an Plattformzustände binden,
- Anti-Rollback- und Monotonic-State-Mechanismen unterstützen,
- Hardwareentropie bereitstellen.

Ein TPM ist jedoch kein allgemeiner Sicherheitsautomat.

Insbesondere gilt nicht:

```text
TPM present
=
system trusted
```

Ein TPM entscheidet nicht selbst, ob ein Kernel vertrauenswürdig ist.

Es stellt kryptografische Mechanismen und hardwaregestützte Evidenz bereit.

Die eigentliche Vertrauensentscheidung bleibt Aufgabe von:

```text
Nova.Trust
+
Boot Trust Policy
+
Verified Boot
```

NovaOS benötigt deshalb eine klar definierte TPM-Integration, die das TPM als kontrollierten Hardware Trust Provider verwendet, ohne die gesamte Bootarchitektur direkt an TPM-spezifische Details zu koppeln.

---

## Entscheidung

NovaOS integriert **TPM 2.0 als primären Hardware Trust Provider für PC-Plattformen**.

Das TPM wird insbesondere verwendet für:

```text
Measured Boot
Attestation
Secret Sealing
Protected Key Operations
Anti-Rollback Support
Hardware-backed Trust Evidence
```

Die höhere Bootarchitektur verwendet jedoch einen abstrakten Hardware-Trust-Contract.

Das Grundmodell lautet:

```text
Nova Boot Security
        │
        ▼
Hardware Trust Provider
        │
        ├── TPM 2.0
        ├── Virtual TPM
        └── Future Trust Provider
```

Für PC-Systeme gilt:

```text
TPM 2.0
    =
preferred hardware-backed
boot measurement provider
```

TPM-Verfügbarkeit allein erzeugt jedoch keine Bootautorität.

---

## Grundprinzip

Es gilt:

```text
Use TPM as evidence and protection.

Do not use TPM as policy.
```

und:

```text
TPM provides hardware-backed state.

NovaOS decides what that state means.
```

---

## Architekturposition

Die TPM-Integration befindet sich logisch zwischen Plattform-/Firmwareabstraktion und Boot Security.

```text
Firmware / Platform
        │
        ▼
TPM Discovery
        │
        ▼
TPM Provider
        │
        ▼
Boot Measurement Service
        │
        ├── PCR
        ├── Event Log
        ├── Quote
        └── Sealing
        │
        ▼
Boot Security / Nova.Trust
```

Höhere Bootkomponenten sollen nicht direkt auf TPM-Register oder Transportprotokolle zugreifen.

---

## Hardware Trust Provider

NovaOS definiert einen abstrahierten Hardware Trust Provider.

Konzeptionell:

```text
HardwareTrustProvider {
    GetCapabilities()
    Measure()
    ReadMeasurementState()
    Quote()
    Seal()
    Unseal()
    CreateProtectedKey()
    Random()
}
```

Nicht jeder Provider muss jede Funktion unterstützen.

Fähigkeiten werden explizit gemeldet.

---

## TPM Provider

Die TPM-spezifische Implementierung bildet diesen Contract auf TPM 2.0 ab.

Konzeptionell:

```text
HardwareTrustProvider
        │
        ▼
TPM2Provider
        │
        ▼
TPM 2.0 Command Interface
        │
        ▼
TPM Hardware
```

---

## TPM-Version

TPM 2.0 ist der primäre unterstützte Standard.

TPM 1.2 darf für Legacy-Systeme separat betrachtet werden, ist jedoch keine Voraussetzung der nativen NovaOS-Architektur.

Neue Implementierungen sollen nicht auf TPM-1.2-spezifischen Annahmen aufbauen.

---

## TPM Discovery

Die Bootumgebung muss erkennen können:

```text
TPM unavailable
TPM present
TPM disabled
TPM inaccessible
TPM malfunctioning
TPM virtualized
```

Diese Zustände dürfen nicht zu einem einfachen:

```text
TPM = true / false
```

reduziert werden.

---

## TPM Capability Discovery

Nach erfolgreicher Erkennung müssen relevante Fähigkeiten abgefragt werden.

Beispiele:

```text
supported algorithms
PCR banks
available commands
NV capabilities
key types
manufacturer
firmware version
```

Nur explizit unterstützte Funktionen dürfen verwendet werden.

---

## Firmware Discovery

Auf PC-Plattformen kann TPM Discovery unter anderem über Firmwareinformationen erfolgen.

Beispielsweise:

```text
ACPI TPM2 Table
```

oder andere definierte Plattformmechanismen.

Diese Informationen werden über den Platform Provider normalisiert.

Höhere Bootlogik soll ACPI-spezifische Tabellen nicht direkt auswerten.

---

## TPM Transport

TPM-Zugriff kann plattformabhängig über unterschiedliche Transportmechanismen erfolgen.

Beispiele:

```text
CRB
FIFO / TIS
Firmware mediated interface
Virtual TPM interface
```

Der höhere Boot Security Code bleibt davon unabhängig.

---

## TPM Locality

TPM Locality beziehungsweise vergleichbare Autoritätsmechanismen müssen kontrolliert verwendet werden.

Bootkomponenten dürfen nicht willkürlich TPM-Autoritätsbereiche beanspruchen.

Die genaue Nutzung wird plattformspezifisch spezifiziert.

---

## TPM Ownership

Das TPM wird als gemeinsame sicherheitskritische Systemressource betrachtet.

Der Bootloader verwendet es nur innerhalb eines klar definierten Boot Security Scopes.

Nach Kernel Handoff wird die weitere Verwaltung an den Kernel beziehungsweise Nova.Trust übergeben.

---

## TPM Lifecycle

Konzeptionell:

```text
Discover
   ↓
Validate
   ↓
Initialize Provider
   ↓
Query Capabilities
   ↓
Use for Boot Measurements
   ↓
Prepare Handoff
   ↓
Transfer Ownership
   ↓
Kernel TPM Service
```

---

## Kein unnötiger TPM Reset

Der Bootloader darf TPM-Zustände nicht unkontrolliert zurücksetzen oder löschen.

Insbesondere dürfen:

- persistente Schlüssel,
- NV-Daten,
- Provisioning State

nicht verändert werden, nur weil NovaOS bootet.

---

## PCRs

Platform Configuration Registers bilden den zentralen Mechanismus für Measured Boot.

Ein PCR speichert keinen einfachen frei schreibbaren Wert.

Stattdessen wird sein Zustand durch Extend-Operationen aufgebaut.

Konzeptionell:

```text
PCR_new =
    Hash(
        PCR_old
        ||
        measurement
    )
```

Dadurch entsteht eine reihenfolgeabhängige kryptografische Messkette.

---

## PCR-Semantik

PCRs enthalten keine direkte Aussage wie:

```text
Kernel trusted
```

Sie enthalten lediglich einen kryptografisch abgeleiteten Zustand.

Die Interpretation erfolgt anhand von:

```text
Measurement Log
Boot Policy
Known Measurements
Attestation Policy
```

---

## PCR Layout

NovaOS definiert eine versionierte PCR-Verwendungsrichtlinie.

Dabei muss berücksichtigt werden, dass bestimmte PCRs bereits von:

- Firmware,
- UEFI,
- Secure Boot,
- Plattformkomponenten

verwendet werden können.

NovaOS darf solche Zustände nicht unkontrolliert überschreiben oder semantisch falsch interpretieren.

---

## Firmware-PCRs

Firmwaremessungen können Bestandteil der Gesamt-Bootkette sein.

Beispiel:

```text
Firmware
    ↓
PCR measurements

Nova Boot
    ↓
additional measurements
```

Die Herkunft der Messungen muss im Event Log erkennbar bleiben.

---

## NovaOS-PCR-Nutzung

NovaOS kann zusätzliche PCRs beziehungsweise definierte Measurement Domains für eigene Zustände verwenden.

Zu messende Klassen können umfassen:

```text
Nova Boot Core
Boot Manifest
Boot Policy
Kernel
Initial Modules
Boot Mode
Recovery State
NovaDOS State
Security Overrides
```

Die konkrete PCR-Zuordnung wird in einer NPSPEC festgelegt.

---

## PCR Bank

TPM 2.0 kann mehrere PCR-Banken mit unterschiedlichen Hashalgorithmen bereitstellen.

Beispiele:

```text
SHA-256
SHA-384
```

NovaOS muss die verfügbaren Banken erkennen.

---

## Algorithmusauswahl

Die Auswahl einer PCR Bank erfolgt gemäß Security Policy.

Ein veralteter oder verbotener Algorithmus darf nicht allein deshalb verwendet werden, weil er vom TPM unterstützt wird.

---

## Mehrere PCR-Banken

NovaOS darf relevante Messungen parallel in mehreren unterstützten PCR-Banken fortführen.

Dies erleichtert:

- Kryptografie-Migration,
- Kompatibilität,
- langfristige Algorithm Agility.

---

## Measurement Operation

Die höhere Bootarchitektur erzeugt ein semantisches Measurement Event.

Beispiel:

```text
MeasurementRequest {
    artifact
    eventType
    digest
    securityContext
}
```

Der TPM Provider übersetzt dies in die entsprechenden TPM-Operationen.

---

## Event Log

Jede relevante PCR-Messung muss mit einem strukturierten Event Log korrelieren können.

Beispiel:

```text
BootMeasurementEvent {
    sequence
    source
    phase
    artifactIdentity
    eventType
    hashAlgorithm
    digest
    pcr
    bootGeneration
}
```

---

## Event-Log-Konsistenz

Das Event Log allein ist kein Hardwarebeweis.

Ein Verifier kann das Log verwenden, um:

```text
expected PCR state
```

zu rekonstruieren und diesen mit dem tatsächlichen TPM-Zustand zu vergleichen.

---

## Messreihenfolge

PCR Extend ist reihenfolgeabhängig.

Daher muss NovaOS die Reihenfolge sicherheitsrelevanter Measurements deterministisch definieren.

Es gilt:

```text
A → B
```

erzeugt nicht denselben PCR-Zustand wie:

```text
B → A
```

---

## Parallelität

Hashing darf parallel erfolgen.

PCR Commit beziehungsweise Extend muss jedoch gemäß definierter Measurement Order erfolgen.

Beispiel:

```text
Hash Kernel ───────┐
Hash Manifest ─────┼─→ Ordered Measurement Commit
Hash Modules ──────┘
```

---

## Bootgeneration

Die aktive Systemgeneration muss eindeutig messbar sein.

Beispiel:

```text
GenerationId
ManifestHash
KernelHash
```

Dadurch kann Attestation zwischen verschiedenen NovaOS-Generationen unterscheiden.

---

## Bootmodus

Der aktive Bootmodus soll in den hardwaregestützten Bootzustand einfließen.

Beispiele:

```text
Normal
Recovery
NovaDOS
Development
Diagnostic
Safe Mode
```

Ein Recovery Boot darf nicht denselben Measurement State wie ein normaler Production Boot erzeugen.

---

## Trust Override

Wurde ein expliziter Trust Override verwendet, muss dieser Zustand messbar sein.

Es gilt:

```text
fully verified production boot
```

und:

```text
manual override boot
```

müssen unterschiedliche Measurement States erzeugen.

---

## Verified Boot Integration

TPM-Messungen ergänzen Verified Boot.

Konzeptionell:

```text
Artifact
    ↓
Hash
    ├──────────────→ Signature Verification
    │
    └──────────────→ PCR Measurement
```

Der Digest darf bei identischer Byte-Repräsentation wiederverwendet werden.

---

## Keine doppelte Autorität

Der TPM-Zustand entscheidet nicht, ob ein gerade geladenes Artefakt ausgeführt werden darf.

Diese Entscheidung bleibt beim Verified-Boot-/Trust-Policy-System.

---

## Measurement vor Ausführung

Sicherheitsrelevante Artefakte müssen vor oder spätestens unmittelbar vor ihrer Übernahme in den aktiven Bootzustand gemessen werden.

Die exakte Reihenfolge zwischen:

```text
Verify
Measure
Execute
```

wird so definiert, dass:

- nur autorisierte Artefakte ausgeführt werden,
- der tatsächliche Bootzustand korrekt messbar bleibt.

---

## Kandidaten

Ein Artefakt, das lediglich geprüft, aber nicht ausgeführt wird, darf nicht ununterscheidbar als aktiv ausgeführter Zustand gemessen werden.

Candidate Measurements und Execution Measurements müssen semantisch unterscheidbar sein.

---

## TPM Quotes

Für Attestation kann ein TPM ausgewählte PCR-Zustände kryptografisch signieren.

Konzeptionell:

```text
Verifier Nonce
      +
PCR Selection
      ↓
TPM Quote
      ↓
Signed Evidence
```

Dadurch kann ein Verifier prüfen, dass die Evidenz frisch ist und aus einem bestimmten TPM stammt.

---

## Nonce

Remote oder lokale Attestation muss Challenge-/Nonce-Mechanismen unterstützen.

Dadurch werden Replay-Angriffe alter Quotes verhindert.

---

## Attestation Key

TPM Quotes werden über geeignete Attestation Keys signiert.

Diese Schlüssel sollen innerhalb des TPM geschützt sein.

Private Key Material darf das TPM normalerweise nicht verlassen.

---

## Endorsement Key

Ein TPM kann einen Endorsement Key beziehungsweise vergleichbare Herstelleridentität besitzen.

NovaOS darf diesen zur Provisionierung oder Vertrauensherstellung verwenden.

Er darf jedoch nicht automatisch als globale Benutzer- oder Geräteidentität veröffentlicht werden.

---

## Privacy

TPM-Identitäten können zur langfristigen Wiedererkennung eines Geräts verwendet werden.

NovaOS muss daher Privacy und Data Sovereignty berücksichtigen.

Insbesondere darf ein Hardwareidentifier nicht automatisch an externe Dienste übertragen werden.

---

## Pseudonyme Attestation

Wo möglich, sollen Attestation Keys und Policies so gestaltet werden, dass nicht unnötig der permanente Hardware Root Identifier offengelegt wird.

---

## Lokale Attestation

Lokale NovaOS-Komponenten dürfen TPM-Evidenz über Nova.Trust verwenden.

Beispiel:

```text
Sensitive Service
      ↓
requires
      ↓
Verified Production Boot
      +
Expected Measurement State
```

---

## Remote Attestation

Remote Attestation ist optional.

Sie darf nur nach:

- expliziter Policy,
- Benutzerfreigabe,
- Enterprise-Vertrauensbeziehung,
- oder anderer autorisierter Regel

durchgeführt werden.

---

## Quote-Verifikation

Ein Quote allein reicht nicht.

Ein Verifier muss mindestens berücksichtigen:

```text
Quote Signature
Nonce
PCR Selection
PCR Values
Measurement Log
Attestation Key Trust
Policy
```

---

## Attestation Result

Das TPM liefert Evidence.

Es liefert nicht direkt das Ergebnis:

```text
Trusted
```

Der Verifier beziehungsweise Nova.Trust erzeugt aus der Evidence eine Trust Decision.

---

## Secret Sealing

Das TPM kann Daten beziehungsweise Schlüsselmaterial an bestimmte PCR-Zustände oder Policies binden.

Konzeptionell:

```text
Secret
    ↓
Seal to Policy
    ↓
TPM Protected Object
```

Unsealing erfolgt nur bei erfüllter Policy.

---

## Disk Encryption

Ein wichtiger Einsatz ist die Freigabe eines Datenträgerverschlüsselungsschlüssels.

Beispiel:

```text
Verified Boot State
      +
Expected PCR State
      ↓
TPM Policy satisfied
      ↓
Release Disk Key
```

---

## Kein direktes Speichern großer Secrets

Das TPM ist nicht als allgemeiner Massenspeicher für große Geheimnisse gedacht.

Typischerweise schützt es kleine Schlüssel, die wiederum größere Daten entschlüsseln.

Konzeptionell:

```text
TPM protects Key Encryption Key
        ↓
Key Encryption Key decrypts
        ↓
Volume Key
```

---

## Policy-basiertes Sealing

NovaOS soll Secret Release nicht ausschließlich an einen einzigen festen PCR-Digest koppeln.

Stattdessen sollen autorisierte Policies möglich sein.

Beispiel:

```text
Generation N
OR
Generation N+1
```

Damit bleiben legitime Updates möglich.

---

## Updatefähigkeit

Vor einem Systemupdate muss berücksichtigt werden, dass sich Boot Measurements verändern.

Der Updateprozess muss neue autorisierte Measurement States vorbereiten können.

---

## Updateablauf

Konzeptionell:

```text
Current Generation N
      ↓
Prepare N+1
      ↓
Verify N+1
      ↓
Authorize N+1 Measurement Policy
      ↓
Boot N+1
      ↓
Health Validation
      ↓
Mark Known-Good
```

Erst danach kann die alte Policy optional entfernt werden.

---

## Kein Update Lockout

NovaOS muss verhindern, dass ein korrekt signiertes Systemupdate allein wegen ungeplanter PCR-Änderungen den Zugriff auf benötigte Secrets dauerhaft verliert.

---

## Recovery Secrets

Recovery darf eigene Secret-Release-Regeln besitzen.

Beispielsweise kann ein Recovery Environment:

- keinen automatischen Zugriff auf Benutzergeheimnisse,
- nur Zugriff nach Recovery Credential,
- oder begrenzte Recovery Keys

erhalten.

Recovery darf nicht automatisch dieselben Secret-Policies wie Normal Boot erben.

---

## NovaDOS

NovaDOS erhält einen eigenen Measurement State.

Das TPM kann dadurch verhindern, dass bestimmte Secrets automatisch in NovaDOS freigegeben werden.

Dies ist besonders wichtig, da NovaDOS privilegierte Offline-Wartungsfunktionen besitzt.

---

## Development Mode

Development Mode soll standardmäßig keine produktionsgebundenen TPM-Secrets automatisch erhalten, sofern die entsprechende Policy dies nicht ausdrücklich erlaubt.

---

## Protected Keys

NovaOS darf kryptografische Schlüssel innerhalb des TPM erzeugen und verwenden.

Beispiele:

```text
Attestation Key
Device Trust Key
Storage Protection Key
Enterprise Device Key
```

Die konkrete Autorität jedes Schlüssels wird über Nova.Trust geregelt.

---

## Kein TPM als Universal-Key-Store

Nicht jeder NovaOS-Schlüssel muss im TPM gespeichert werden.

Die Wahl hängt ab von:

- Sicherheitsniveau,
- Performance,
- Portabilität,
- Backupbedarf,
- Hardwareverfügbarkeit.

---

## Key Hierarchies

TPM-Key-Hierarchien dürfen verwendet werden, um unterschiedliche Zwecke zu trennen.

Beispiel:

```text
Platform
Storage
Endorsement
Attestation
Application-specific
```

Die genaue Zuordnung wird separat spezifiziert.

---

## TPM NV Storage

TPM NV Storage darf für kleine sicherheitskritische Zustände verwendet werden.

Beispiele:

```text
anti-rollback counter
trust generation
provisioning state
policy metadata
```

TPM NV Storage ist kein Ersatz für das normale Dateisystem.

---

## Anti-Rollback

TPM-geschützte monotone oder geschützte Zustände können verwendet werden, um unautorisierte Downgrades zu erkennen.

Beispiel:

```text
Installed Security Generation = 42
Attempted Generation = 38
    ↓
Rollback denied
```

---

## Autorisierter Rollback

Known-Good Recovery muss weiterhin möglich sein.

Anti-Rollback bedeutet deshalb:

```text
No unauthorized downgrade
```

nicht:

```text
No rollback ever
```

---

## NV-Schreibverschleiß

TPM NV Storage besitzt begrenzte Schreibcharakteristika.

NovaOS darf keine hochfrequenten normalen Laufzeitdaten in TPM-NV-Strukturen schreiben.

Writes müssen sparsam und transaktional geplant werden.

---

## TPM Random Number Generator

Der TPM RNG darf als zusätzliche Entropiequelle verwendet werden.

Er darf mit anderen geprüften Systementropiequellen kombiniert werden.

NovaOS soll nicht zwingend ausschließlich von einer einzigen Hardware-RNG-Quelle abhängen.

---

## Entropie während des Boots

Frühe Bootphasen können TPM Random verwenden, beispielsweise für:

- Nonces,
- temporäre Schlüssel,
- ASLR Seed Material.

Die konkrete Nutzung muss die Verfügbarkeit und Qualität der Plattform berücksichtigen.

---

## TPM Clock und Counter

TPM-Zeit- oder Counterinformationen dürfen als zusätzliche vertrauenswürdige Zustandsinformationen verwendet werden.

Sie ersetzen jedoch nicht automatisch die gesamte Systemzeitarchitektur.

---

## TPM Failure

TPM-Fehler müssen typisiert behandelt werden.

Beispiele:

```text
TpmUnavailable
TpmDisabled
TpmCommandFailed
TpmTimeout
TpmProtocolError
TpmPcrUnavailable
TpmAlgorithmUnsupported
TpmNvFailure
TpmPolicyFailure
TpmQuoteFailure
TpmUnsealFailure
```

---

## Policyabhängiges Fehlerverhalten

Ein TPM Failure bedeutet nicht in jedem Sicherheitsprofil dasselbe.

Beispiel:

```text
MeasuredBoot = Required
TPM failure
    ↓
Boot blocked
```

gegen:

```text
MeasuredBoot = Preferred
TPM failure
    ↓
Verified Boot continues
    ↓
Security degradation recorded
```

---

## Kein stilles Ignorieren

Ein erwarteter TPM-Pfad darf bei Fehlern nicht stillschweigend als erfolgreich behandelt werden.

Der resultierende Security State muss den Fehler darstellen.

---

## TPM Lockout

TPM kann Schutz gegen wiederholte fehlerhafte Autorisierungsversuche besitzen.

NovaOS muss Lockout-Zustände erkennen und darf sie nicht durch unkontrollierte Wiederholungsversuche verschärfen.

---

## Retry

TPM-Operationen dürfen nur kontrolliert wiederholt werden.

Es gelten:

- begrenzte Retries,
- Command-spezifische Fehlerbehandlung,
- keine Endlosschleifen,
- kein aggressives Polling.

---

## Timeouts

Jede TPM-Operation muss begrenzte Timeouts besitzen.

Ein defektes TPM darf den Bootprozess nicht unbegrenzt blockieren.

---

## Structured Concurrency

TPM-Aufgaben gehören in einen kontrollierten Boot Security Scope.

Wird ein Bootplan verworfen, müssen abhängige Operationen kontrolliert abgeschlossen oder abgebrochen werden.

---

## Transaktionen

TPM-NV- oder Policy-Updates sollen mit den allgemeinen transaktionalen NovaOS-Prinzipien kombiniert werden.

Ein kritischer Zustand darf nicht halb aktualisiert werden.

---

## TPM Provisioning

NovaOS unterscheidet:

```text
Unprovisioned
Provisioned
Managed
RecoveryRequired
```

beziehungsweise äquivalente Zustände.

Provisionierung darf nicht unbemerkt destruktiv erfolgen.

---

## Neuinstallation

Eine NovaOS-Neuinstallation darf TPM-Zustände nicht automatisch vollständig löschen.

Insbesondere könnten dort:

- Schlüssel,
- Enterprise-Provisioning,
- andere Betriebssysteminformationen

vorhanden sein.

Destruktive TPM-Operationen erfordern explizite Autorisierung.

---

## TPM Clear

Ein TPM Clear ist eine hochkritische administrative Operation.

Es darf niemals Teil eines normalen Bootvorgangs oder automatischen Self-Healing sein.

---

## Multi-Boot-Systeme

NovaOS muss davon ausgehen, dass ein TPM möglicherweise von mehreren Betriebssystemen oder Firmwarekomponenten verwendet wird.

Es darf nicht exklusiven Besitz über alle TPM-Ressourcen voraussetzen.

---

## PCR-Koordination

Insbesondere bei Multi-Boot-Systemen müssen Firmware- und standardisierte PCR-Konventionen respektiert werden.

NovaOS-eigene Measurement Domains sollen Konflikte vermeiden.

---

## Virtual TPM

Virtuelle Maschinen dürfen einen vTPM als Hardware Trust Provider verwenden.

Der Provider State muss kennzeichnen:

```text
TrustBacking = Virtual
```

beziehungsweise eine vergleichbare Herkunft.

---

## Kein falscher Hardwareclaim

Ein vTPM darf nicht automatisch als physisch hardwaregestütztes TPM dargestellt werden.

Die Evidence Provenance muss erhalten bleiben.

---

## Snapshot-Risiken

Bei virtualisierten TPMs müssen VM-Snapshots und Rollbacks berücksichtigt werden.

Sie können monotone Zustände oder Anti-Rollback-Garantien beeinflussen.

Der Hypervisor-/vTPM-Vertrauenskontext muss daher Teil der Bewertung sein.

---

## Confidential Computing

Zukünftige Confidential-Computing-Technologien dürfen zusätzliche Hardware Measurements liefern.

Diese werden nicht zwangsläufig über TPM abgebildet, sollen aber in dasselbe übergeordnete Evidence-Modell integrierbar sein.

---

## DICE und zukünftige Provider

NovaOS soll zukünftig auch andere Trust-Technologien integrieren können, beispielsweise:

```text
DICE
Secure Enclave
Platform Security Processor
TEE-based measurement
```

Der TPM Provider ist daher eine Implementierung und nicht die Definition der gesamten Trust-Architektur.

---

## BootInfo Handoff

Der Kernel erhält einen strukturierten TPM-/Hardware-Trust-Handoff.

Konzeptionell:

```text
HardwareTrustInfo {
    providerType
    providerVersion
    capabilities
    measuredBootState
    activePcrBanks
    measurementLog
    evidenceState
}
```

---

## TPM Handles

Temporäre TPM Handles dürfen nicht unkontrolliert über den Kernel Handoff hinaus gültig angenommen werden.

Persistente beziehungsweise übergebbare Objekte müssen explizit dokumentiert werden.

---

## Session Cleanup

Nicht mehr benötigte TPM Sessions und transient Objects sollen vor Handoff kontrolliert freigegeben werden.

Dies reduziert Ressourcenverbrauch und unerwartete Autoritätsreste.

---

## Kernel Ownership

Nach Kernel Entry übernimmt ein dedizierter NovaOS TPM-/Hardware-Trust-Service die weitere Verwaltung.

Bootloader und Kernel dürfen nicht konkurrierend dieselben TPM-Ressourcen verwenden.

---

## Measurement Log Handoff

Der Boot Measurement Log wird an den Kernel übergeben.

Der Kernel kann anschließend:

- Log validieren,
- PCR Replay durchführen,
- eigene Measurements anhängen,
- Attestation bereitstellen.

---

## Measurement Continuity

Der Übergang muss eine kontinuierliche Messkette ermöglichen:

```text
Firmware
    ↓
Bootloader
    ↓
Kernel
    ↓
Critical Early Services
```

Dabei muss jede Quelle identifizierbar bleiben.

---

## Kernel Revalidation

Der Kernel muss TPM-Handoff-Daten defensiv validieren.

Er darf nicht allein aufgrund eines BootInfo-Flags annehmen:

```text
TPM trusted
```

Die tatsächliche Hardwareverfügbarkeit und relevante Zustände sollen soweit sinnvoll erneut bestätigt werden.

---

## Architecture Introspection

Autorisierte Diagnose soll mindestens anzeigen können:

```text
Hardware Trust Provider:
    TPM 2.0

TPM State:
    Available

PCR Banks:
    SHA-256

Measured Boot:
    Active

Attestation:
    Available

Sealed Storage:
    Available
```

---

## Sensitive Introspection

Nicht jeder Prozess darf sehen:

- Endorsement-Key-Informationen,
- Attestation-Key-Identitäten,
- genaue PCR-Policies,
- TPM-NV-Inhalte.

Diese Informationen werden capabilitybasiert geschützt.

---

## Audit

Relevante TPM-Ereignisse sollen auditierbar sein.

Beispiele:

```text
TPM discovered
PCR bank selected
kernel measurement committed
quote generated
secret unsealed
TPM unavailable
TPM policy failure
anti-rollback triggered
```

---

## Keine Secret-Werte im Audit

Audit Logs dürfen keine entschlüsselten Secret-Werte oder private Schlüssel enthalten.

---

## Performance

TPM-Operationen können deutlich langsamer als normale RAM- oder CPU-Operationen sein.

NovaOS soll deshalb:

- unnötige TPM-Kommandos vermeiden,
- Digests außerhalb des TPM vorberechnen, wenn sicher,
- TPM nur für hardwaregebundene Funktionen verwenden,
- Operationen sinnvoll bündeln.

---

## Kein Security Bypass aus Performancegründen

Performanceoptimierung darf niemals:

- Measurements auslassen,
- Required Policies umgehen,
- Secret Policies schwächen.

---

## Resource Economy

TPM-Kommandos werden als begrenzte sicherheitskritische Ressource behandelt.

Dies betrifft insbesondere:

- Command Queue,
- Sessions,
- transient Objects,
- NV Writes,
- Bootzeitbudget.

---

## Deterministic Mode

Bei gleichem Bootplan muss die Reihenfolge der PCR-Measurements deterministisch sein.

Zufällige Nonces oder Attestation Challenges dürfen davon getrennt sein.

---

## Reproduzierbarkeit

Ein deterministischer Boot mit identischen:

- Artefakten,
- Policy,
- Generation,
- Measurement-Reihenfolge

soll dieselben artefaktbezogenen PCR-Werte erzeugen, sofern vorgelagerte Plattformzustände ebenfalls identisch sind.

---

## Unterschiede durch Firmware

NovaOS darf nicht erwarten, dass komplette PCR-Endzustände auf unterschiedlichen Hardwareplattformen identisch sind.

Firmware-Measurements können unterschiedlich sein.

Daher soll Policy möglichst semantisch beziehungsweise anhand bekannter Measurement-Komponenten und nicht ausschließlich über global hartcodierte Komplettwerte definiert werden.

---

## Fail-Safe Defaults

Für TPM-basierte Required Controls gilt:

```text
Unknown
=
Not satisfied
```

und:

```text
TPM failure
≠
trust
```

---

## Self-Healing

Self-Healing darf TPM-basierte Boot Policies aktualisieren oder reparieren, wenn es dafür explizit autorisiert ist.

Es darf:

- TPM nicht automatisch löschen,
- Anti-Rollback nicht umgehen,
- Sealing Policies nicht unautorisiert schwächen.

---

## Recovery

Wenn TPM-basierte Secret-Freigabe fehlschlägt, muss ein definierter Recoverypfad existieren können.

Beispiele:

```text
Recovery Key
Administrative Credential
Offline Recovery Token
Enterprise Recovery Authority
```

Recovery darf jedoch keine generelle Umgehung der Boot Trust Policy darstellen.

---

## Keine Einzelabhängigkeit

Kritische Benutzer- oder Unternehmensdaten sollten nicht so an ein einzelnes TPM gebunden werden, dass ein Hardwaredefekt zwangsläufig permanenten Datenverlust verursacht.

Geeignete Recovery- oder Escrow-Mechanismen müssen abhängig vom Sicherheitsprofil möglich sein.

---

## Hardwarewechsel

Mainboard- oder TPM-Wechsel verändert Hardware Trust Identity und Measurement State.

NovaOS muss diesen Fall explizit behandeln können.

Mögliche Reaktionen:

```text
Recovery authentication
Re-provision trust
Re-seal secrets
Re-register device identity
```

---

## TPM Firmware Update

Ein TPM-Firmwareupdate kann:

- Capabilities,
- Measurements,
- Attestation State

beeinflussen.

NovaOS muss solche Änderungen als kontrollierte Platform Evolution behandeln.

---

## Trust-Level

TPM-Verfügbarkeit darf als zusätzliche Evidence Strength in Nova.Trust einfließen.

Beispiel:

```text
SoftwareVerified
HardwareMeasured
HardwareAttested
```

Diese Stufen sind semantisch getrennt.

---

## Keine universelle Vertrauenszahl

NovaOS soll TPM-Trust nicht auf einen einzelnen numerischen „Security Score“ reduzieren.

Trust bleibt kontext- und policyabhängig.

---

## Sicherheitsgrenzen

TPM schützt unter anderem gegen bestimmte Manipulationen von:

- Bootzuständen,
- Schlüsseln,
- Measurement Evidence.

Es schützt nicht vollständig gegen:

- kompromittierte CPU,
- manipulierte Firmware vor Root of Trust,
- invasive Hardwareangriffe,
- fehlerhafte Trust Policy,
- legitimen, aber kompromittierten Signierer.

---

## AI-Unabhängigkeit

Alle TPM-bezogenen Sicherheitsentscheidungen müssen ohne KI funktionieren.

KI darf optional:

- Measurement Logs erklären,
- ungewöhnliche PCR-Abweichungen analysieren,
- Recoveryoptionen vorschlagen.

KI darf nicht:

- PCR Policies ignorieren,
- Sealed Secrets freigeben,
- Attestation ersetzen,
- Anti-Rollback überschreiben,
- TPM Failure als vertrauenswürdig deklarieren.

---

## Normative Anforderungen

1. NovaOS MUSS TPM 2.0 als primären Hardware Trust Provider für PC-Plattformen unterstützen.
2. TPM MUSS hinter einem abstrakten Hardware-Trust-Provider-Contract gekapselt werden.
3. Höhere Boot Security Komponenten SOLLEN NICHT direkt von TPM-Transportdetails abhängen.
4. Die Architektur MUSS zukünftige alternative Hardware Trust Provider unterstützen können.
5. NovaOS MUSS TPM-Verfügbarkeit und TPM-Zustand explizit erkennen können.
6. `Unavailable`, `Disabled`, `Failed` und `Virtualized` MÜSSEN unterscheidbar sein.
7. TPM-Fähigkeiten MÜSSEN vor Verwendung relevanter Funktionen abgefragt werden.
8. TPM-Transportmechanismen MÜSSEN plattformspezifisch gekapselt werden.
9. TPM 1.2 DARF NICHT Grundlage der nativen neuen NovaOS-Architektur sein.
10. TPM DARF NICHT als alleinige Trust Policy behandelt werden.
11. TPM-Präsenz DARF NICHT automatisch `Trusted` bedeuten.
12. Verified Boot MUSS die Ausführungsautorität bestimmen.
13. TPM Measurements MÜSSEN als Evidence und nicht als Permission behandelt werden.
14. TPM-PCRs MÜSSEN für Measured Boot nutzbar sein.
15. PCR Extend MUSS gemäß TPM-Semantik verwendet werden.
16. NovaOS MUSS eine versionierte PCR-Nutzungsrichtlinie definieren.
17. Firmware-reservierte beziehungsweise standardisierte PCR-Verwendung MUSS berücksichtigt werden.
18. Bootkomponenten DÜRFEN PCRs NICHT unkoordiniert verwenden.
19. Verfügbare PCR-Banken MÜSSEN erkannt werden.
20. Die Auswahl der PCR-Bank MUSS Security Policy berücksichtigen.
21. Verbotene Hashalgorithmen DÜRFEN NICHT verwendet werden, nur weil das TPM sie unterstützt.
22. Mehrere PCR-Banken SOLLEN parallel unterstützt werden können.
23. Measurement Ordering MUSS deterministisch definiert sein.
24. Parallel Hashing DARF zu keiner nichtdeterministischen PCR-Reihenfolge führen.
25. Relevante PCR-Operationen MÜSSEN mit einem strukturierten Event Log korrelierbar sein.
26. Measurement Events MÜSSEN Quelle, Artefakt, Digest, Algorithmus und PCR identifizieren können.
27. Event Logs SOLLEN gegen PCR-Endzustände replay-verifizierbar sein.
28. Das Event Log DARF NICHT als alleiniger Hardwarebeweis behandelt werden.
29. Der aktive NovaOS-Kernel MUSS messbar sein.
30. Das aktive Boot Manifest SOLL messbar sein.
31. Sicherheitsrelevante Bootkonfiguration SOLL messbar sein.
32. Initiale sicherheitsrelevante Module SOLLEN messbar sein.
33. Die aktive Systemgeneration SOLL eindeutig messbar sein.
34. Bootmodi MÜSSEN unterschiedliche Measurement States erzeugen können.
35. Recovery und Normal Boot DÜRFEN NICHT zwangsläufig denselben Measurement State erzeugen.
36. NovaDOS MUSS als eigener messbarer Bootzustand darstellbar sein.
37. Development Mode MUSS messbar sein.
38. Trust Overrides MÜSSEN messbar sein.
39. Vollständig verifizierter Production Boot und Override Boot MÜSSEN unterscheidbar sein.
40. Digest-Berechnungen DÜRFEN zwischen Verification und Measurement wiederverwendet werden, wenn die zugrunde liegende Repräsentation identisch ist.
41. Digest-Reuse DARF Verification und Measurement semantisch NICHT verschmelzen.
42. TPM Quotes MÜSSEN für Attestation unterstützt werden können.
43. Attestation MUSS Nonce-basierte Freshness unterstützen.
44. Attestation Keys SOLLEN hardwaregeschützt sein.
45. Private Attestation-Key-Materialien SOLLEN das TPM nicht verlassen.
46. Endorsement-Identität DARF NICHT ungefragt als globale externe Geräteidentität offengelegt werden.
47. Attestation MUSS Privacy und Data Sovereignty respektieren.
48. Remote Attestation MUSS explizit autorisiert sein.
49. Lokale Attestation MUSS unterstützt werden können.
50. Ein Quote DARF NICHT ohne Signature-, Nonce-, PCR- und Policyprüfung als vertrauenswürdig gelten.
51. Das TPM MUSS Secret Sealing unterstützen können.
52. Secret Release MUSS an explizite Policies gebunden werden können.
53. Datenträgerverschlüsselung DARF TPM-basiertes Key Sealing verwenden.
54. Große Secrets SOLLEN NICHT direkt im TPM gespeichert werden.
55. TPM SOLL bevorzugt kleine Schlüssel beziehungsweise Key-Wrapping-Material schützen.
56. Sealing Policies MÜSSEN legitime Systemupdates berücksichtigen können.
57. Systemupdates DÜRFEN nicht allein durch unveränderliche starre PCR-Bindung unbrauchbar werden.
58. Neue autorisierte Bootgenerationen MÜSSEN vor Aktivierung in Secret-Release-Policies integrierbar sein.
59. Recovery MUSS eigene Secret-Release-Regeln besitzen können.
60. Recovery DARF NICHT automatisch dieselbe Secret Authority wie Normal Boot erhalten.
61. NovaDOS DARF standardmäßig nicht automatisch alle produktiven TPM-Secrets erhalten.
62. Development Mode SOLL produktionsgebundene Secrets standardmäßig nicht automatisch erhalten.
63. TPM-geschützte Schlüssel MÜSSEN nach Zweck trennbar sein.
64. TPM DARF NICHT als verpflichtender Universal-Key-Store für alle NovaOS-Schlüssel verwendet werden.
65. TPM NV Storage DARF für kleine sicherheitskritische Zustände verwendet werden.
66. TPM NV Storage DARF NICHT als allgemeines Dateisystem verwendet werden.
67. Anti-Rollback-State MUSS hardwaregeschützt gespeichert werden können.
68. Anti-Rollback DARF autorisierten Known-Good-Rollback NICHT grundsätzlich verhindern.
69. TPM-NV-Schreiboperationen MÜSSEN sparsam verwendet werden.
70. TPM RNG DARF als zusätzliche Entropiequelle verwendet werden.
71. NovaOS SOLL für kritische Entropie nicht unnötig ausschließlich von einer einzelnen TPM-RNG-Quelle abhängen.
72. TPM-Fehler MÜSSEN typisiert behandelbar sein.
73. TPM Failure MUSS unabhängig von Verified Boot Failure dargestellt werden.
74. Das Verhalten bei TPM Failure MUSS von der Security Policy abhängen.
75. Ist Measured Boot `Required`, MUSS ein nicht verfügbarer erforderlicher TPM Provider den normalen Boot blockieren.
76. Ist Measured Boot nur `Preferred`, DARF ein verifizierter Boot kontrolliert degradiert fortgesetzt werden.
77. Eine solche Degradation MUSS protokolliert und introspektierbar sein.
78. TPM-Kommandos MÜSSEN begrenzte Timeouts besitzen.
79. TPM-Retries MÜSSEN kontrolliert und begrenzt sein.
80. TPM Lockout-Zustände MÜSSEN berücksichtigt werden.
81. NovaOS DARF durch unkontrollierte Wiederholungsversuche TPM Lockout nicht unnötig verschärfen.
82. TPM Provisioning MUSS vom normalen Bootbetrieb getrennt sein.
83. Ein normaler Boot DARF das TPM NICHT automatisch clearen.
84. Self-Healing DARF das TPM NICHT automatisch clearen.
85. Ein TPM Clear MUSS explizite administrative Autorisierung erfordern.
86. NovaOS MUSS Multi-Boot-Nutzung des TPM berücksichtigen.
87. NovaOS DARF NICHT exklusiven Besitz aller TPM-Ressourcen voraussetzen.
88. Virtual TPM MUSS unterstützt werden können.
89. Virtualisierte TPM Evidence MUSS als virtualisiert erkennbar sein.
90. vTPM DARF NICHT automatisch als physisch hardwaregestützte Evidence dargestellt werden.
91. Snapshot- und Rollback-Risiken virtueller TPMs MÜSSEN bei Anti-Rollback-Policies berücksichtigt werden.
92. TPM-Handoff an den Kernel MUSS versioniert sein.
93. Temporäre TPM-Ressourcen SOLLEN vor Kernel Handoff freigegeben werden.
94. Bootloader und Kernel DÜRFEN TPM-Ressourcen nach Ownership Transfer NICHT unkoordiniert parallel verwalten.
95. Der Boot Measurement Log MUSS an den Kernel übergebbar sein.
96. Measurement Continuity über Firmware, Bootloader und Kernel MUSS möglich sein.
97. Der Kernel MUSS TPM-Handoff-Daten defensiv validieren.
98. Hardware Trust Evidence MUSS in Nova.Trust integrierbar sein.
99. Hardware-backed und software-reported Trust Evidence MÜSSEN unterscheidbar sein.
100. TPM-bezogene Detailintrospektion MUSS capabilitybasiert geschützt werden.
101. Relevante TPM-Sicherheitsereignisse MÜSSEN auditierbar sein.
102. Private Schlüssel oder entschlüsselte Secrets DÜRFEN NICHT in normalen Audit Logs erscheinen.
103. TPM-Nutzung SOLL unnötige Bootzeitkosten vermeiden.
104. Performanceoptimierung DARF Required Measurements oder Policies NICHT umgehen.
105. TPM-Ressourcen SOLLEN in die systemweite Resource Economy integrierbar sein.
106. TPM-PCR-Reihenfolge MUSS im Deterministic Mode reproduzierbar sein.
107. NovaOS DARF NICHT voraussetzen, dass vollständige PCR-Endwerte hardwareübergreifend identisch sind.
108. TPM-basierte Policies SOLLEN Firmware- und Plattformunterschiede berücksichtigen können.
109. Ein TPM-Hardwaredefekt DARF bei geeigneter Recovery Policy nicht zwangsläufig permanenten Datenverlust verursachen.
110. Hardwarewechsel MUSS über einen expliziten Re-Provisioning- beziehungsweise Recoverypfad behandelbar sein.
111. TPM-Firmwareupdates MÜSSEN als mögliche Veränderung der Hardware Trust Platform behandelt werden.
112. TPM-Evidence DARF NICHT auf einen einzelnen universellen Security Score reduziert werden.
113. TPM-Funktionen MÜSSEN vollständig ohne KI korrekt funktionieren.
114. KI DARF PCR-, Sealing-, Attestation- oder Anti-Rollback-Policy NICHT überschreiben.
115. KI DARF TPM Failure NICHT als Trust-Nachweis interpretieren.
116. NovaOS MUSS TPM als kontrollierten Hardware Trust Provider und nicht als versteckte globale Sicherheitsautorität behandeln.

---

## Konsequenzen

### Positive Konsequenzen

- Measured Boot erhält eine hardwaregestützte Grundlage.
- Bootzustände können kryptografisch attestiert werden.
- Disk- und Systemschlüssel können an autorisierte Bootzustände gebunden werden.
- Anti-Rollback kann hardwaregestützt verstärkt werden.
- Firmware-, Bootloader- und Kernelmessungen können zu einer durchgehenden Measurement Chain verbunden werden.
- Enterprise- und High-Security-Szenarien werden unterstützt.
- TPM-Details bleiben hinter einem abstrakten Provider gekapselt.
- zukünftige Hardware Trust Provider können dasselbe übergeordnete Modell verwenden.
- virtuelle Systeme können über vTPM integriert werden.
- Nova.Trust kann Hardware Evidence als zusätzliche Vertrauensgrundlage verwenden.

### Negative Konsequenzen

- TPM-Kommandos und Provisioning erhöhen die Bootkomplexität.
- TPM-Operationen können relativ langsam sein.
- PCR- und Sealing-Policies müssen sorgfältig mit Updates koordiniert werden.
- Hardwaredefekte können Recoveryprozesse auslösen.
- Privacy muss bei Attestation explizit berücksichtigt werden.
- Virtualisierung erschwert die Interpretation von Hardware Evidence.
- Multi-Boot-Systeme erfordern koordinierte TPM-Nutzung.
- TPM-NV-Speicher ist begrenzt und darf nicht unnötig beschrieben werden.

---

## Verworfene Alternativen

### TPM als Voraussetzung für jeden NovaOS-Boot

Verworfen.

Verified Boot muss bei entsprechender Policy auch auf Systemen ohne TPM funktionieren können.

### TPM direkt aus allen Bootkomponenten ansprechen

Verworfen.

Dies würde TPM-Transport, Policy und Trust-Mechanismen unnötig koppeln.

### TPM als alleinige Vertrauensentscheidung verwenden

Verworfen.

TPM liefert Mechanismen und Evidence, nicht die NovaOS Trust Policy.

### Nur exakte PCR-Werte für Secret Sealing verwenden

Verworfen.

Legitime Updates würden dadurch unnötig schwer oder unmöglich.

### Alle Schlüssel im TPM speichern

Verworfen.

Nicht jeder Schlüssel benötigt Hardwarebindung, und Portabilität sowie Recovery müssen berücksichtigt werden.

### TPM-NV als allgemeine sichere Datenbank verwenden

Verworfen.

NV Storage ist klein, vergleichsweise langsam und besitzt begrenzte Schreibcharakteristika.

### Endorsement Key als globale Gerätekennung verwenden

Verworfen.

Dies würde unnötige Privacy- und Tracking-Risiken erzeugen.

### Remote Attestation automatisch aktivieren

Verworfen.

Attestation Evidence kann sensible Plattforminformationen offenlegen.

### TPM Clear als Standard-Recoverymaßnahme

Verworfen.

Ein Clear kann dauerhaft wichtige Schlüssel und Provisioning-Daten zerstören.

### KI-basierte Interpretation anstelle kryptografischer TPM-Policy

Verworfen.

Hardware-Trust-Entscheidungen müssen deterministisch und überprüfbar bleiben.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-BOOT-0001_Parallele_BIOS_und_UEFI_Bootunterstützung`
- `ADR-BOOT-0002_UEFI_bevorzugen_mit_BIOS_Fallback`
- `ADR-BOOT-0003_Mehrstufige_Bootarchitektur`
- `ADR-BOOT-0004_ELF_als_native_Kernel_Ladeform`
- `ADR-BOOT-0005_Vertrauenswürdige_Bootkette`
- `ADR-BOOT-0006_Secure_Measured_Verified_Boot_Integration`
- `ADR-HAL-0003_Firmware_über_Platform_Provider_abstrahieren`
- `ADR-HAL-0004_Hardwaretopologie_als_Systemgraph`
- `ADR-HAL-0006_DMA_und_IOMMU_als_kontrollierte_Systemressourcen`
- `ADR-ARCH-0002_Mechanism_Policy_Separation`
- `ADR-ARCH-0003_Deklaratives_Systemmodell_als_Architekturgrundlage`
- `ADR-ARCH-0004_Transaktionale_Systemoperationen`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
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

- `NPSPEC-BOOT-TPM-0001`
- `NPSPEC-BOOT-TPM-DISCOVERY-0001`
- `NPSPEC-BOOT-TPM-PROVIDER-0001`
- `NPSPEC-BOOT-TPM-TRANSPORT-0001`
- `NPSPEC-BOOT-TPM-CAPABILITY-0001`
- `NPSPEC-BOOT-TPM-PCR-0001`
- `NPSPEC-BOOT-TPM-PCR-POLICY-0001`
- `NPSPEC-BOOT-TPM-MEASUREMENT-0001`
- `NPSPEC-BOOT-TPM-EVENTLOG-0001`
- `NPSPEC-BOOT-TPM-QUOTE-0001`
- `NPSPEC-BOOT-TPM-ATTESTATION-0001`
- `NPSPEC-BOOT-TPM-KEY-0001`
- `NPSPEC-BOOT-TPM-SEALING-0001`
- `NPSPEC-BOOT-TPM-NV-0001`
- `NPSPEC-BOOT-TPM-ANTIROLLBACK-0001`
- `NPSPEC-BOOT-TPM-RNG-0001`
- `NPSPEC-BOOT-TPM-PROVISIONING-0001`
- `NPSPEC-BOOT-TPM-RECOVERY-0001`
- `NPSPEC-BOOT-TPM-HANDOFF-0001`
- `NPSPEC-BOOT-VTPM-0001`
- `NPSPEC-BOOT-TPM-INTROSPECTION-0001`
- `NPSPEC-BOOT-TPM-AUDIT-0001`
- `NPSPEC-BOOT-TPM-TEST-0001`

---

## Verifikation

Die Umsetzung muss mindestens folgende Szenarien prüfen:

- physisches TPM 2.0 vorhanden,
- kein TPM vorhanden,
- TPM vorhanden aber deaktiviert,
- TPM antwortet nicht,
- TPM-Command-Timeout,
- CRB-Transport,
- TIS/FIFO-Transport,
- vTPM,
- Capability Discovery,
- SHA-256-PCR-Bank,
- mehrere PCR-Banken,
- nicht unterstützter Hashalgorithmus,
- deterministische Measurement-Reihenfolge,
- PCR Extend,
- Event-Log-Replay,
- manipuliertes Event Log,
- Kernel Measurement,
- Manifest Measurement,
- Bootmode Measurement,
- Recovery Measurement,
- NovaDOS Measurement,
- Development Mode Measurement,
- Trust Override Measurement,
- TPM Quote mit gültiger Nonce,
- Quote Replay mit alter Nonce,
- ungültige Quote-Signatur,
- Local Attestation,
- Remote Attestation,
- Secret Seal,
- erfolgreiches Unseal,
- Unseal bei verändertem Bootzustand,
- Disk-Key-Freigabe,
- Systemupdate mit neuer autorisierter PCR-Policy,
- Known-Good-Rollback,
- blockierter unautorisierter Downgrade,
- TPM NV Read/Write,
- NV-Fehler,
- TPM Lockout,
- Recovery bei TPM-Ausfall,
- Mainboard-/TPM-Wechsel,
- TPM-Firmwareupdate,
- Multi-Boot-Nutzung,
- vTPM-Snapshot-Rollback,
- Measurement Log Handoff an den Kernel,
- Kernel-Revalidierung des TPM-Zustands,
- Measurement Continuity nach Kernelstart.

---

## Ergebnis

NovaOS integriert TPM 2.0 als hardwaregestützten Vertrauens- und Messprovider, ohne die Bedeutung von Trust, Policy und Autorität an das TPM selbst zu delegieren.

Das Architekturmodell lautet:

```text
Boot Artifact
      ↓
Verified Boot
      ↓
Digest
      ↓
TPM Measurement
      ↓
PCR State
      +
Measurement Log
      ↓
Attestation Evidence
      ↓
Nova.Trust
```

Für geschützte Geheimnisse gilt:

```text
Authorized Boot State
      ↓
TPM Policy
      ↓
Unseal
      ↓
Secret available
```

Dabei bleibt die zentrale Trennung erhalten:

```text
TPM measures.

TPM protects.

TPM attests.

NovaOS decides.
```

Die zentrale Architekturregel lautet:

```text
Use hardware-backed trust evidence
where available.

Keep policy explicit.

Keep measurements deterministic.

Keep secret release controlled.

And never confuse
hardware evidence
with automatic authority.
```