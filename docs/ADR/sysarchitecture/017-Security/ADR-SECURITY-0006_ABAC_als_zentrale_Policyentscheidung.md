# ADR-SECURITY-0006 – ABAC als zentrale Policyentscheidung

## Status

Angenommen

## Kategorie

Security / Authorization / Policy / ABAC

## Kontext

NovaOS verwendet Capabilities als natives Authority-Modell, DAC für Kompatibilität und MAC für harte systemweite Sicherheitsgrenzen.

Viele Autorisierungsentscheidungen hängen jedoch zusätzlich vom aktuellen Kontext ab. Beispiele sind:

- Identität und Rolle des Subjects,
- Zielobjekt und dessen Eigenschaften,
- angeforderte Operation,
- Execution Domain,
- Trust Level,
- Datenklassifikation,
- Standort,
- Zeit,
- Systemzustand,
- Data-Sovereignty-Anforderungen.

Statische Rollen oder ACLs können solche Entscheidungen nur eingeschränkt ausdrücken.

NovaOS benötigt deshalb eine einheitliche kontextabhängige Policyentscheidung.

---

## Entscheidung

NovaOS verwendet **Attribute-Based Access Control (ABAC)** als zentrale Policyentscheidungslogik für kontextabhängige Autorisierung.

ABAC ersetzt nicht das Capability-Modell, sondern entscheidet innerhalb der bestehenden Authority- und Sicherheitsgrenzen.

```text
Capability Authority
        ↓
ABAC Policy Decision
        ↓
MAC / Hard Constraints
        ↓
Effective Authorization
```

ABAC kann Authority einschränken oder bestimmen, unter welchen Bedingungen vorhandene Authority verwendet werden darf.

ABAC darf jedoch keine Authority erzeugen, die nicht durch das Capability-Modell zulässig ist.

---

## Grundprinzip

```text
Capabilities define authority.
ABAC decides whether that authority may be exercised
in the current context.
```

---

## Attribute

Policyentscheidungen können Attribute aus mehreren Bereichen verwenden:

```text
Subject
Resource
Action
Environment
Context
```

Beispielsweise:

```text
Subject
 ├── Identity
 ├── Role
 ├── Trust
 └── Execution Domain

Resource
 ├── Type
 ├── Owner
 ├── Classification
 └── Sovereignty Policy

Action
 └── Requested Operation

Environment
 ├── Location
 ├── Time
 └── System State
```

Die konkrete Attributstruktur wird in NPSPECs definiert.

---

## Zentrale Policyentscheidung

ABAC bildet die gemeinsame Policylogik für native NovaOS-Autorisierungsentscheidungen.

Konzeptionell:

```text
Authorization Request
        ↓
Policy Decision
        ↓
Permit / Deny
```

Die Policyentscheidung kann von unterschiedlichen Enforcement Points verwendet werden, beispielsweise:

- Kernel,
- Systemdiensten,
- IPC,
- `Nova.Call`,
- Storage,
- Netzwerk,
- Gerätezugriff.

Dies bedeutet nicht, dass jede Operation synchron einen einzelnen zentralen Policy-Dienst aufrufen muss.

Die Architektur darf verteilte, gecachte oder lokal auswertbare Policyzustände verwenden, solange deren Semantik konsistent bleibt.

---

## Verhältnis zu Capabilities

Capabilities bestimmen die maximal verfügbare Authority.

ABAC bewertet deren konkrete Verwendung.

```text
Capability missing
    → Deny

Capability present
ABAC denies
    → Deny

Capability present
ABAC permits
    → Continue with remaining hard constraints
```

ABAC darf keine fehlende Capability ersetzen.

---

## Verhältnis zu DAC

DAC-Regeln können als Attribute oder Policy-Eingaben in die Autorisierungsentscheidung einfließen.

Beispielsweise:

```text
Owner
Group
ACL Entry
```

DAC bleibt damit ein Kompatibilitätsmodell und benötigt keinen unabhängigen Enforcement-Stack.

---

## Verhältnis zu MAC

MAC definiert harte systemweite Sicherheitsgrenzen.

ABAC kann zur Auswertung solcher Regeln verwendet werden, darf deren Verbindlichkeit jedoch nicht abschwächen.

```text
ABAC Permit
    +
MAC Deny
    =
Deny
```

Hard Security Constraints besitzen Vorrang vor kontextabhängigen permissiven Entscheidungen.

---

## Policy-Priorität

ABAC muss sich in die allgemeine NovaOS-Prioritätsordnung einfügen.

Insbesondere haben:

```text
Safety
Security
Data Sovereignty
Hard Realtime
Determinism
Correctness
```

Vorrang vor weichen Optimierungs- oder Präferenzentscheidungen.

Eine ABAC-Policy darf keine höher priorisierte Hard Constraint überschreiben.

---

## Policy Decision und Enforcement

NovaOS trennt:

```text
Policy Decision
      ↓
Policy Enforcement
```

Der Policy Decision Point bestimmt die Entscheidung.

Der Policy Enforcement Point setzt sie am tatsächlichen Zugriff durch.

Dadurch kann dieselbe Policyarchitektur über unterschiedliche Subsysteme hinweg verwendet werden.

---

## Policy Context

ABAC-Entscheidungen müssen den relevanten Ausführungskontext berücksichtigen können.

Dazu gehören insbesondere:

- Capability Context,
- Caller Identity,
- Delegated Authority,
- Trust Context,
- Execution Domain,
- Causation Context,
- Data Classification,
- Sovereignty Context.

Damit bleibt die Policyentscheidung auch über IPC- und Distributed-Communication-Grenzen konsistent.

---

## Fail-Safe Verhalten

Fehlen Attribute, die für eine harte Sicherheitsentscheidung erforderlich sind, gilt:

```text
Unknown
    ≠
Allowed
```

Eine Policy darf fehlende sicherheitskritische Informationen nicht automatisch als permissiv interpretieren.

Optionale Attribute können gemäß expliziter Policy Fallback-Semantik besitzen.

---

## Policyänderungen

ABAC-Policies sind sicherheitskritischer Systemzustand.

Änderungen müssen:

- explizit autorisiert,
- validiert,
- versionierbar,
- auditierbar

sein.

Wo mehrere Policyänderungen zusammengehören, sollen sie transaktional aktiviert werden können.

---

## Performance

ABAC darf nicht voraussetzen, dass jede sicherheitsrelevante Operation eine teure globale Policyberechnung ausführt.

NovaOS darf deshalb:

- vorkompilierte Policies,
- lokale Policyzustände,
- sichere Caches,
- spezialisierte Fast Paths

verwenden.

Optimierungen dürfen das Ergebnis der maßgeblichen Policy nicht verändern.

---

## Audit und Introspection

NovaOS muss relevante ABAC-Entscheidungen nachvollziehbar machen können.

Beispielsweise:

```text
Subject
Action
Resource
Relevant Attributes
Applicable Policy
Decision
Reason
```

Sicherheitskritische oder geheime Attribute dürfen dabei nicht unnötig offengelegt werden.

---

## Normative Anforderungen

1. NovaOS MUSS ABAC als zentrale Policyentscheidungslogik für kontextabhängige Autorisierung unterstützen.
2. ABAC DARF das Capability-Modell NICHT als native Authority-Grundlage ersetzen.
3. ABAC DARF keine Authority erzeugen, die nicht durch vorhandene Capabilities zulässig ist.
4. ABAC MUSS Subject-, Resource-, Action- und Context-Attribute berücksichtigen können.
5. DAC-Informationen MÜSSEN als Policy-Eingaben integrierbar sein.
6. MAC- und andere Hard Constraints DÜRFEN durch ABAC NICHT abgeschwächt werden.
7. Policy Decision und Policy Enforcement MÜSSEN architektonisch trennbar sein.
8. ABAC MUSS über unterschiedliche NovaOS-Subsysteme konsistent einsetzbar sein.
9. Policyentscheidungen MÜSSEN Capability-, Trust-, Delegation- und Sovereignty-Kontexte berücksichtigen können.
10. Fehlende sicherheitskritische Attribute MÜSSEN fail-safe behandelt werden.
11. Policyänderungen MÜSSEN explizit autorisiert, validierbar und auditierbar sein.
12. ABAC MUSS effiziente lokale oder gecachte Policyentscheidungen ermöglichen können.
13. Policy-Optimierungen DÜRFEN die semantische Autorisierungsentscheidung NICHT verändern.
14. Relevante Policyentscheidungen MÜSSEN kontrolliert auditierbar und introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- einheitliche kontextabhängige Policyentscheidungen,
- flexible Kombination von Identity, Trust, Ressourcen- und Umgebungsinformationen,
- DAC, MAC und Capabilities lassen sich in einer konsistenten Entscheidungsarchitektur verbinden,
- Policies können subsystemübergreifend verwendet werden,
- gute Grundlage für Data Sovereignty und dynamische Sicherheitskontexte.

### Negative Konsequenzen

- Policyentscheidungen werden komplexer,
- Attribute benötigen vertrauenswürdige Herkunft und klare Semantik,
- Policy-Caching und verteilte Auswertung benötigen konsistente Invalidierungsregeln.

---

## Verworfene Alternativen

### RBAC als zentrale Policyarchitektur

Verworfen.

Rollen sind nützlich, aber für kontext-, objekt- und umgebungsabhängige Entscheidungen zu grob.

### ABAC als Ersatz für Capabilities

Verworfen.

Policyentscheidung und konkrete Authority bleiben getrennte Konzepte.

### Separate Policy Engines pro Subsystem

Verworfen.

Dies würde unterschiedliche Sicherheitssemantiken für Storage, Netzwerk, IPC und andere Bereiche erzeugen.

### Ausschließlich zentraler synchroner Policy-Dienst

Verworfen.

Dies würde einen unnötigen Performance-, Verfügbarkeits- und Fehlerengpass erzeugen.

---

## Abhängigkeiten

- `ADR-ARCH-0002_Mechanism_Policy_Separation`
- `ADR-ARCH-0013_Architecture_Introspection`
- `ADR-ARCH-0014_Explicit_Contracts`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-SECURITY-0001_Capability_Based_Security_als_zentrales_Autorisierungsmodell`
- `ADR-SECURITY-0003_Least_Privilege_und_Privilege_Separation`
- `ADR-SECURITY-0004_DAC_als_Kompatibilitätsmodell`
- `ADR-SECURITY-0005_MAC_für_systemweite_harte_Sicherheitsregeln`

---

## Zugehörige NPSPECs

- `NPSPEC-SECURITY-ABAC-0001`
- `NPSPEC-SECURITY-ABAC-ATTRIBUTE-0001`
- `NPSPEC-SECURITY-ABAC-POLICY-0001`
- `NPSPEC-SECURITY-ABAC-DECISION-0001`
- `NPSPEC-SECURITY-ABAC-ENFORCEMENT-0001`
- `NPSPEC-SECURITY-ABAC-TEST-0001`

---

## Ergebnis

NovaOS verwendet eine klar getrennte Sicherheitsarchitektur:

```text
Capabilities
    ↓
Available Authority
    ↓
ABAC
    ↓
Contextual Policy Decision
    ↓
MAC / Hard Constraints
    ↓
Enforcement
```

Capabilities definieren die vorhandene Authority.

ABAC entscheidet anhand des aktuellen Kontexts, ob diese Authority verwendet werden darf.

MAC und andere Hard Constraints setzen Grenzen, die auch eine positive ABAC-Entscheidung nicht überschreiten kann.

Damit erhält NovaOS eine einheitliche, kontextabhängige und subsystemübergreifende Policyarchitektur, ohne das Capability-basierte Autorisierungsmodell zu ersetzen.