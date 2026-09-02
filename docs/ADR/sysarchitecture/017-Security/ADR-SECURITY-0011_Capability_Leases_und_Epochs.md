# ADR-SECURITY-0011 – Capability Leases und Epochs

## Status

Angenommen

## Kategorie

Security / Authorization / Capabilities / Lifecycle

## Kontext

Capability Revocation ermöglicht NovaOS, bereits vergebene Authority wieder zu entziehen.

Bei kurzlebiger, dynamischer oder stark verteilter Authority kann eine permanente individuelle Revocation-Verwaltung jedoch teuer werden.

NovaOS benötigt deshalb ergänzende Mechanismen, mit denen Capabilities automatisch auslaufen oder ganze Gruppen veralteter Authority effizient ungültig werden können.

---

## Entscheidung

NovaOS unterstützt **Capability Leases und Epochs** als ergänzende Lifecycle-Mechanismen des Capability-Modells.

```text
Capability
 ├── Lease
 └── Epoch
```

Ein Lease begrenzt Authority zeitlich.

Eine Epoch bindet Authority an eine bestimmte Generation ihres Authority-Kontexts.

---

## Grundprinzip

```text
Authority is valid only
for its permitted time and generation.
```

---

## Capability Leases

Eine Capability kann nur für einen begrenzten Zeitraum gültig sein.

```text
Issued
  ↓
Active Lease
  ↓
Expired
```

Nach Ablauf des Leases ist die Capability nicht mehr gültig.

Eine weitere Verwendung erfordert:

- Renewal,
- Revalidation oder
- eine neue Capability.

Die konkrete Zeitrepräsentation und Renewal-Semantik wird in NPSPECs definiert.

---

## Lease Renewal

Leases können erneuerbar sein.

```text
Capability
    ↓
Lease expires soon
    ↓
Revalidation
    ↓
Renew / Deny
```

Eine Erneuerung ist eine neue Autorisierungsentscheidung und darf nicht automatisch vorausgesetzt werden.

Dabei können aktuelle:

- Security Policies,
- Trust States,
- Capability Policies,
- Systemzustände

erneut berücksichtigt werden.

---

## Epochs

Eine Capability kann an eine Epoch gebunden werden.

```text
Authority Domain
Epoch = 42
```

Capability:

```text
Authority Domain
Epoch = 42
```

Wird die Domain auf:

```text
Epoch = 43
```

weitergeschaltet, sind Capabilities der vorherigen Epoch entsprechend ihrer definierten Semantik nicht mehr gültig.

---

## Revocation über Epoch-Wechsel

Epochs ermöglichen die effiziente Invalidierung größerer Capability-Gruppen.

```text
Epoch 7
 ├── Capability A
 ├── Capability B
 └── Capability C

        ↓ Epoch Change

Epoch 8
```

Die alten Capabilities müssen dafür nicht einzeln gesucht oder physisch zurückgeholt werden.

Epochs ergänzen damit gezielte individuelle Revocation.

---

## Revocation Domains

Epochs können an Revocation Domains gebunden werden.

Beispiele:

```text
Session
Application Instance
Service
Execution Domain
Task Group
Security Context
```

Ein Epoch-Wechsel kann dadurch Authority eines bestimmten Kontexts invalidieren, ohne andere Capability-Domains zu beeinflussen.

---

## Delegation und Attenuation

Delegierte oder attenuierte Capabilities dürfen Lease- und Epoch-Grenzen ihrer Quelle nicht abschwächen.

Es gilt:

```text
Lease(Derived) ≤ Lease(Source)
```

und eine abgeleitete Capability darf nicht unabhängig von der erforderlichen Source-Epoch gültig bleiben.

---

## Kombination von Lease, Epoch und Revocation

Die Mechanismen ergänzen sich:

```text
Capability Valid
    =
Authority valid
AND Lease valid
AND Epoch valid
AND Not revoked
AND Policy permits
```

NovaOS schreibt nicht vor, dass jede Capability alle Mechanismen gleichzeitig verwenden muss.

Der Capability-Typ bestimmt die erforderliche Lifecycle-Semantik.

---

## Lokale und entfernte Capabilities

Lease- und Epoch-Semantik muss unabhängig davon gelten, ob eine Capability:

- lokal,
- über IPC,
- oder über `Nova.Call`

verwendet wird.

Ein Location- oder Transportwechsel darf abgelaufene oder veraltete Authority nicht wieder gültig machen.

---

## Fail-Safe Verhalten

Kann die erforderliche Lease- oder Epoch-Gültigkeit nicht zuverlässig bestimmt werden, gilt für sicherheitskritische Authority:

```text
Unknown
    ≠
Valid
```

NovaOS muss dann eine erneute Validierung verlangen oder die Operation ablehnen.

---

## Normative Anforderungen

1. NovaOS MUSS zeitlich begrenzte Capability Leases unterstützen.
2. NovaOS MUSS Epoch-basierte Capability-Gültigkeit unterstützen.
3. Abgelaufene Leases DÜRFEN keine Authority mehr bereitstellen.
4. Lease Renewal MUSS als erneute Autorisierungsentscheidung behandelbar sein.
5. Epoch-Wechsel MÜSSEN Capability-Gruppen effizient invalidieren können.
6. Epochs MÜSSEN an definierte Authority- oder Revocation-Domains bindbar sein.
7. Delegierte Capabilities DÜRFEN Lease-Grenzen ihrer Quelle NICHT erweitern.
8. Attenuation DARF Epoch-Bindungen NICHT abschwächen.
9. Lease, Epoch und individuelle Revocation MÜSSEN kombinierbar sein.
10. Nicht jede Capability MUSS zwingend Lease und Epoch gleichzeitig verwenden.
11. Lokale und entfernte Capabilities MÜSSEN dieselbe Lease- und Epoch-Semantik besitzen.
12. Transport- oder Location-Wechsel DÜRFEN abgelaufene Authority NICHT wiederherstellen.
13. Unbekannter Lease- oder Epoch-Zustand MUSS für sicherheitskritische Authority fail-safe behandelt werden.
14. Lease- und Epoch-Zustände MÜSSEN kontrolliert auditierbar und introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- automatische Begrenzung langlebiger Authority,
- effiziente gruppenweise Revocation,
- weniger individueller Revocation-Zustand,
- gute Unterstützung für Sessions, Services und Task Groups,
- geeignet für lokale und verteilte Capabilities.

### Negative Konsequenzen

- zusätzliche Lifecycle-Zustände,
- Lease Renewal benötigt erneute Validierung,
- Epoch-Wechsel können viele Capabilities gleichzeitig invalidieren.

---

## Verworfene Alternativen

### Ausschließlich individuelle Revocation

Verworfen.

Für große oder dynamische Capability-Gruppen kann individuelle Verwaltung unnötig teuer sein.

### Ausschließlich Capability Leases

Verworfen.

Sofortige gruppenweite Invalidierung wäre nur eingeschränkt möglich.

### Ausschließlich Epochs

Verworfen.

Epochs ersetzen keine individuelle oder zeitlich begrenzte Revocation.

---

## Abhängigkeiten

- `ADR-ARCH-0010_Structured_Concurrency_systemweit`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-SECURITY-0001_Capability_Based_Security_als_zentrales_Autorisierungsmodell`
- `ADR-SECURITY-0008_Capability_Attenuation`
- `ADR-SECURITY-0009_Capability_Delegation`
- `ADR-SECURITY-0010_Capability_Revocation`

---

## Zugehörige NPSPECs

- `NPSPEC-SECURITY-CAPABILITY-LEASE-0001`
- `NPSPEC-SECURITY-CAPABILITY-LEASE-RENEWAL-0001`
- `NPSPEC-SECURITY-CAPABILITY-EPOCH-0001`
- `NPSPEC-SECURITY-CAPABILITY-EPOCH-DOMAIN-0001`
- `NPSPEC-SECURITY-CAPABILITY-LEASE-EPOCH-TEST-0001`

---

## Ergebnis

NovaOS ergänzt Capability Revocation um zeitliche und generationenbasierte Gültigkeit:

```text
Capability
    ↓
Lease + Epoch
    ↓
Current Authority
    ↓
Expiration / Epoch Change / Revocation
    ↓
Invalid Authority
```

Leases begrenzen Authority zeitlich.

Epochs ermöglichen die effiziente Invalidierung ganzer Authority-Generationen.

Gemeinsam mit Revocation bilden sie das Lifecycle-Modell für dynamische und verteilte Capabilities in NovaOS.