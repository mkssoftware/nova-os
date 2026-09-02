# ADR-CAPABILITY-0006 – Capability Negotiation

## Status

Angenommen

## Kategorie

Capability Architecture / Negotiation / Contracts / Provider Selection

## Kontext

Eine angeforderte Capability und ein verfügbarer Provider müssen nicht in allen Eigenschaften exakt übereinstimmen.

Unterschiede können beispielsweise bestehen bei:

- Contract-Versionen,
- Semantic Types,
- unterstützten Formaten,
- Qualitätsstufen,
- deterministischer Ausführung,
- Hardwarebeschleunigung,
- Latenz,
- Ressourcenbedarf,
- Security und Trust,
- Data Sovereignty.

NovaOS benötigt deshalb einen kontrollierten Mechanismus, mit dem Consumer-Anforderungen und Provider-Eigenschaften vor der Ausführung auf Kompatibilität geprüft und auf einen konkreten Vertrag gebracht werden.

---

## Entscheidung

NovaOS führt **Capability Negotiation** zwischen Capability Requirement und möglichen Providern ein.

```text
Capability Requirement
        ↓
Discovery
        ↓
Candidate Providers
        ↓
Negotiation
        ↓
Agreed Capability Contract
        ↓
Execution
```

Negotiation bestimmt, ob und unter welchen zulässigen Bedingungen eine Capability ausgeführt werden kann.

---

## Grundprinzip

```text
Negotiate compatible choices.

Never negotiate away hard constraints.
```

---

## Negotiation Inputs

Die Negotiation kann insbesondere berücksichtigen:

```text
Capability Identity
Contract Version
Semantic Types
Input / Output Formats
ExecutionContract
Provider Capabilities
Security Requirements
Trust Requirements
Data Sovereignty
Resource Budget
Deadline
Determinism
Locality
```

Die konkreten Negotiation-Regeln werden in NPSPECs definiert.

---

## Hard Constraints

Hard Constraints sind nicht verhandelbar.

Beispielsweise:

```text
Security
Data Sovereignty
Required Trust
Required Determinism
Required Semantic Correctness
Hard Deadline / Realtime Requirements
```

Ein Provider, der einen solchen Constraint nicht erfüllen kann, wird ausgeschlossen.

```text
Candidate
    ↓
Hard Constraints
 ├── satisfied     → continue
 └── not satisfied → reject
```

---

## Negotiierbare Eigenschaften

Innerhalb der verbleibenden zulässigen Kandidaten können Eigenschaften ausgehandelt werden.

Beispiele:

```text
Image Format: PNG | WebP | AVIF
Resolution:   4K | 1440p | 1080p
Backend:      CPU | GPU
Quality:      100 | 90 | 80
```

Dabei dürfen nur Werte gewählt werden, die sowohl der Consumer Contract als auch der Provider Contract erlauben.

---

## Contract-Versionen

Negotiation muss unterschiedliche kompatible Contract-Versionen behandeln können.

```text
Consumer supports: V2, V3
Provider supports: V1, V2

Negotiated: V2
```

Eine gemeinsame Versionsnummer allein genügt jedoch nicht, wenn die tatsächlichen semantischen Verträge inkompatibel sind.

Contract-Kompatibilität wird explizit geprüft.

---

## Semantic Types

Semantic Types sind Bestandteil der Negotiation.

```text
Consumer requires:
Image<RGB, HDR>

Provider supports:
Image<RGB, SDR>
Image<RGB, HDR>
```

Die Negotiation darf nur eine semantisch kompatible Variante auswählen.

Implizite verlustbehaftete Konvertierungen dürfen nicht als semantische Gleichheit behandelt werden.

---

## Execution Contract

`Nova.ExecutionContract` begrenzt den zulässigen Negotiation-Raum.

```text
Capability Requirement
        +
ExecutionContract
        ↓
Negotiation
        ↓
Concrete Execution Agreement
```

Beispielsweise kann der Consumer automatische Provider-Auswahl erlauben, aber gleichzeitig verlangen:

```text
Deterministic = Required
Latency <= 10 ms
DataRegion = EU
Trust >= RequiredLevel
```

Nur Lösungen innerhalb dieser Grenzen sind gültig.

---

## Graceful Degradation

Wenn ein Consumer ausdrücklich Degradation erlaubt, darf Negotiation alternative Qualitätsstufen auswählen.

```text
Preferred: 4K
Allowed:   1440p
Minimum:   1080p
```

Kann 4K nicht erfüllt werden:

```text
4K unavailable
      ↓
1440p allowed?
      ↓
Yes → negotiate 1440p
```

Nicht deklarierte Degradation ist verboten.

---

## Provider-Wechsel

Bei Hotplug, Provider Failure, Migration oder Hot Replacement kann eine erneute Negotiation erforderlich sein.

```text
Current Provider unavailable
        ↓
Discovery
        ↓
Renegotiation
        ↓
Compatible Provider
```

Der neue Vertrag darf bestehende Hard Constraints nicht verletzen.

---

## Composition

Bei Capability Composition können Negotiation-Ergebnisse mehrerer Capabilities voneinander abhängig sein.

```text
Decoder
   ↓ format negotiation
Filter
   ↓ format negotiation
Encoder
```

NovaOS darf deshalb Negotiation für eine gesamte Pipeline planen, um unnötige Formatkonvertierungen oder Datenbewegungen zu vermeiden.

---

## Keine versteckten Downgrades

NovaOS darf keine Eigenschaft stillschweigend abschwächen.

Insbesondere dürfen folgende Änderungen nicht implizit erfolgen:

```text
Secure → Insecure
Trusted → Untrusted
Deterministic → Non-Deterministic
Sovereignty-Compliant → Non-Compliant
Lossless → Lossy
Authorized → Unauthorized
```

Ist keine gültige Einigung möglich, schlägt die Negotiation explizit fehl.

---

## Negotiation Result

Das Ergebnis muss als konkreter, nachvollziehbarer Vertrag darstellbar sein.

Beispielsweise:

```text
Capability
Provider
Contract Version
Semantic Types
Selected Formats
Execution Properties
Resource Agreement
Security / Trust Context
```

Dieses Ergebnis bildet die Grundlage der anschließenden Ausführung.

---

## Introspection

Capability Negotiation wird in Architecture Introspection integriert.

NovaOS soll nachvollziehbar machen können:

```text
Requested Contract
Candidate Providers
Rejected Candidates
Negotiable Properties
Selected Properties
Final Contract
Negotiation Reason
```

Sensitive Informationen bleiben entsprechend der jeweiligen Authority geschützt.

---

## Normative Anforderungen

1. NovaOS MUSS Capability Negotiation unterstützen.
2. Negotiation MUSS zwischen Consumer-Anforderungen und Provider-Eigenschaften erfolgen können.
3. Hard Constraints DÜRFEN durch Negotiation NICHT abgeschwächt werden.
4. Provider, die Hard Constraints nicht erfüllen, MÜSSEN ausgeschlossen werden.
5. Contract-Versionen MÜSSEN auf tatsächliche Kompatibilität geprüft werden.
6. Semantic Types MÜSSEN Bestandteil der Negotiation sein können.
7. `Nova.ExecutionContract` MUSS den zulässigen Negotiation-Raum begrenzen.
8. Graceful Degradation DARF nur erfolgen, wenn sie explizit erlaubt wurde.
9. Security-, Trust- und Sovereignty-Anforderungen DÜRFEN NICHT stillschweigend heruntergestuft werden.
10. Negotiation MUSS bei Provider-Wechsel erneut durchgeführt werden können.
11. Capability Composition MUSS pipelineweite Negotiation unterstützen können.
12. Ist keine zulässige Einigung möglich, MUSS die Negotiation explizit fehlschlagen.
13. Das Negotiation-Ergebnis MUSS als konkreter Execution Contract darstellbar sein.
14. Negotiation-Entscheidungen MÜSSEN kontrolliert introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- Consumer und Provider müssen nicht exakt dieselben Implementierungsdetails besitzen,
- Contract-Evolution wird erleichtert,
- Hardware- und Providerunterschiede können ausgeglichen werden,
- Qualitäts- und Formatvarianten können kontrolliert gewählt werden,
- Graceful Degradation wird explizit statt implizit,
- Capability Pipelines können als Ganzes optimiert werden.

### Negative Konsequenzen

- Negotiation erhöht die Komplexität der Capability Resolution,
- Verträge müssen klar zwischen Required, Preferred und Optional unterscheiden,
- dynamische Provider-Wechsel können Renegotiation auslösen.

---

## Verworfene Alternativen

### Nur exakt identische Capability Contracts zulassen

Verworfen.

Dies würde Evolvability und Provider-Vielfalt unnötig einschränken.

### Provider entscheidet eigenständig über Downgrades

Verworfen.

Der Provider darf Anforderungen des Consumers nicht eigenmächtig abschwächen.

### Alle Eigenschaften sind verhandelbar

Verworfen.

Hard Constraints wie Security und Data Sovereignty müssen unveränderlich bleiben.

### Negotiation und Provider-Auswahl als identischer Vorgang

Verworfen.

Negotiation bestimmt kompatible Vereinbarungen; Resolution wählt anschließend die geeignete ausführbare Lösung.

---

## Abhängigkeiten

- `ADR-CAPABILITY-0001_Capabilities_als_primäre_Systemfähigkeiten`
- `ADR-CAPABILITY-0003_Capability_Registry`
- `ADR-CAPABILITY-0004_Capability_Discovery`
- `ADR-CAPABILITY-0005_Capability_Composition`
- `ADR-ARCH-0014_Explicit_Contracts`
- `ADR-ARCH-0015_Graceful_Degradation`
- `ADR-ARCH-0019_Evolvable_System_Architecture`

---

## Zugehörige NPSPECs

- `NPSPEC-CAPABILITY-NEGOTIATION-0001`
- `NPSPEC-CAPABILITY-CONTRACT-COMPATIBILITY-0001`
- `NPSPEC-CAPABILITY-SEMANTIC-NEGOTIATION-0001`
- `NPSPEC-CAPABILITY-DEGRADATION-0001`
- `NPSPEC-CAPABILITY-RENEGOTIATION-0001`
- `NPSPEC-CAPABILITY-NEGOTIATION-TEST-0001`

---

## Ergebnis

Capability Negotiation bildet die kontrollierte Verbindung zwischen abstrakter Anforderung und konkreter ausführbarer Capability:

```text
Requirement
    ↓
Discovery
    ↓
Candidates
    ↓
Hard Constraints
    ↓
Negotiation
    ↓
Agreed Contract
    ↓
Resolution / Execution
```

Damit kann NovaOS unterschiedliche Provider, Versionen, Formate und Qualitätsstufen flexibel zusammenführen, ohne Security, Korrektheit, Data Sovereignty oder andere verbindliche Anforderungen zugunsten einer erfolgreichen Ausführung abzuschwächen.