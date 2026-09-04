# ADR-TRUST-0010 – Workload Attestation vor Secret Release

## Status

Angenommen

## Kategorie

Trust Architecture / Workload Attestation / Secret Protection

## Kontext

NovaOS verarbeitet schützenswerte Secrets wie kryptographische Schlüssel, Credentials, Tokens oder andere vertrauliche Daten.

Eine gültige Workload Identity allein beweist nicht, dass sich die zugehörige Ausführungsumgebung in einem vertrauenswürdigen Zustand befindet.

Insbesondere bei isolierten oder entfernten Workloads muss verhindert werden, dass Secrets an kompromittierte oder unerwartete Ausführungsumgebungen freigegeben werden.

## Entscheidung

NovaOS unterstützt **Workload Attestation vor der Freigabe schützenswerter Secrets**.

```text
Workload
    ↓
Identity + Attestation Evidence
    ↓
Nova.Trust
    ↓
Secret Release Policy
    ↓
Secret Use
```

Eine Secret Release Policy kann verlangen, dass die aktuelle Workload vor der Nutzung eines Secrets erfolgreich attestiert wurde.

## Grundprinzip

```text
Known Workload Identity
≠
Trusted Workload State
```

und:

```text
Secret Authority
+
Required Attestation
=
Permitted Secret Use
```

Die erforderliche Capability allein reicht nicht aus, wenn die Secret Policy zusätzlich einen bestimmten Trust-Zustand verlangt.

## Workload Binding

Attestation Evidence muss eindeutig an die konkrete Workload beziehungsweise deren Execution Environment gebunden sein.

```text
Stable Service Identity
        ↓
Workload Identity
        ↓
Execution Environment
        ↓
Attestation Evidence
```

Eine Attestation einer anderen Workload darf nicht für die Secret-Freigabe wiederverwendet werden.

## Secret Release Policy

Secrets können Anforderungen an ihre Freigabe definieren.

Beispiele:

```text
Required Workload Identity
Required Trust Domain
Required Integrity State
Required Software Provenance
Required Attestation Provider
Required Freshness
Required Execution Environment
```

Die konkrete Policy-Struktur wird in NPSPECs definiert.

## Secret Use statt Secret Export

Workload Attestation wird mit der Trennung zwischen Secret Use und Secret Export kombiniert.

Wenn möglich:

```text
Workload
    ↓
Attestation
    ↓
Authorized Secret Operation
    ↓
Protected Key Backend
    ↓
Result
```

Das eigentliche Secret muss die geschützte Umgebung nicht verlassen.

Eine erfolgreiche Attestation erzeugt keine automatische Exportberechtigung.

## Freshness

Attestation Evidence muss für den jeweiligen Secret-Release-Kontext ausreichend aktuell sein.

```text
Fresh Attestation
      ↓
Release Window
      ↓
Secret Operation
```

Abgelaufene oder nicht ausreichend aktuelle Evidence darf bei entsprechenden Hard Requirements nicht zur Secret-Freigabe verwendet werden.

## Zustandsänderungen

Ändert sich ein relevanter Workload-Zustand, kann eine erneute Attestation erforderlich werden.

Beispiele:

- Code Replacement,
- Configuration Change,
- Execution Domain Change,
- Integrity State Change,
- Security Policy Change,
- Trust Revocation.

Bereits freigegebene langlebige Secret Handles müssen bei relevanten Trust-Änderungen erneut überprüfbar oder widerrufbar sein.

## Lokale und entfernte Workloads

Das Modell gilt sowohl für lokale als auch für entfernte Workloads.

```text
Local Workload  ─┐
                  ├→ Workload Attestation → Secret Policy
Remote Workload ─┘
```

Remote Workloads können zusätzlich Remote Attestation benötigen.

Location Transparency darf die tatsächliche Trust-Grenze nicht verbergen.

## Fail Closed

Kann eine vorgeschriebene Attestation nicht erfolgreich durchgeführt oder verifiziert werden, wird die Secret-Nutzung nicht freigegeben.

```text
Required Attestation
        ↓
Unknown / Invalid / Stale
        ↓
Deny Secret Use
```

## Authority

Attestation ersetzt keine Capability-Prüfung.

Für eine Secret-Operation müssen alle relevanten Bedingungen erfüllt sein:

```text
Identity
+
Capability
+
Trust
+
Attestation
+
Secret Policy
=
Permitted Secret Operation
```

## Normative Anforderungen

1. NovaOS MUSS Workload Attestation als Bedingung für Secret Use unterstützen können.
2. Workload Identity und Workload Attestation MÜSSEN getrennte Konzepte bleiben.
3. Attestation Evidence MUSS an die konkrete Workload oder Execution Environment bindbar sein.
4. Attestation einer anderen Workload DARF nicht zur Secret-Freigabe wiederverwendet werden.
5. Secret Policies MÜSSEN Trust- und Attestation-Anforderungen ausdrücken können.
6. Attestation Freshness MUSS für Secret Use überprüfbar sein.
7. Bei vorgeschriebener Attestation MUSS fehlende, ungültige oder veraltete Evidence zu `Deny` führen.
8. Relevante Workload-Zustandsänderungen MÜSSEN eine erneute Attestation auslösen können.
9. Secret Use und Secret Export MÜSSEN getrennte Authorities bleiben.
10. Erfolgreiche Attestation DARF keine automatische Secret-Export-Authority erzeugen.
11. Workload Attestation MUSS für lokale und entfernte Workloads verwendbar sein.
12. Attestation DARF Capability-, Security-, Trust- oder Data-Sovereignty-Prüfungen NICHT ersetzen.
13. Secret Handles MÜSSEN bei relevanten Trust- oder Revocation-Ereignissen invalidierbar sein können.
14. Sicherheitsrelevante Secret-Release-Entscheidungen MÜSSEN kontrolliert auditierbar sein.

## Konsequenzen

### Positive Konsequenzen

- Secrets werden an den tatsächlichen Workload-Zustand gebunden,
- kompromittierte Workloads können trotz gültiger Identity ausgeschlossen werden,
- hardwaregeschützte Secret Stores lassen sich integrieren,
- lokale und entfernte Workloads verwenden dasselbe Trust-Modell,
- Secret Use kann ohne Offenlegung des eigentlichen Secrets erfolgen.

### Negative Konsequenzen

- Secret Use benötigt zusätzliche Trust- und Attestation-Prüfungen,
- Zustandsänderungen können erneute Attestation erforderlich machen,
- langlebige Secret Handles benötigen Revocation-Unterstützung.

## Verworfene Alternativen

### Workload Identity allein reicht für Secret Use

Verworfen, da Identity keine Aussage über den aktuellen Ausführungszustand trifft.

### Capability allein reicht für Secret Use

Verworfen, da besonders schützenswerte Secrets zusätzlich an Trust- und Attestation-Anforderungen gebunden sein können.

### Secret nach erfolgreicher Attestation vollständig exportieren

Verworfen, da Secret Use nach Möglichkeit ohne Offenlegung des Secret-Materials erfolgen soll.

## Abhängigkeiten

- `ADR-TRUST-0001_Nova_Trust_als_systemweite_Vertrauensarchitektur`
- `ADR-TRUST-0009_Remote_Attestation`
- `ADR-IDENTITY-0003_Ephemeral_Workload_Identity`
- `ADR-SECURITY-0018_Fail_Closed_für_Sicherheitsentscheidungen`
- `ADR-SECURITY-0019_Secret_Use_und_Secret_Export_getrennt`
- `ADR-SECURITY-0020_Hardware_Backend_für_schützenswerte_Schlüssel`
- `ADR-CAPABILITY-0009_Capability_Revocation`

## Zugehörige NPSPECs

- `NPSPEC-TRUST-WORKLOAD-ATTESTATION-0001`
- `NPSPEC-TRUST-SECRET-RELEASE-0001`
- `NPSPEC-TRUST-SECRET-RELEASE-POLICY-0001`
- `NPSPEC-TRUST-ATTESTATION-FRESHNESS-0001`
- `NPSPEC-TRUST-SECRET-HANDLE-REVOCATION-0001`
- `NPSPEC-TRUST-WORKLOAD-SECRET-TEST-0001`

## Ergebnis

NovaOS kann Secret-Nutzung an einen verifizierten Workload-Zustand binden:

```text
Workload Identity
       +
Attestation Evidence
       ↓
Nova.Trust
       ↓
Secret Release Policy
       ↓
Capability Check
       ↓
Protected Secret Use
```

Damit wird ein Secret nicht allein deshalb verwendbar, weil eine Workload bekannt oder autorisiert ist. Die Workload muss zusätzlich den für das Secret vorgeschriebenen vertrauenswürdigen Ausführungszustand nachweisen.