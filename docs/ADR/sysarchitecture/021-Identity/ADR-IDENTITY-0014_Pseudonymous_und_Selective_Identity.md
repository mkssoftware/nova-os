# ADR-IDENTITY-0014 – Pseudonymous und Selective Identity

## Status

Angenommen

## Kategorie

Identity Architecture / Privacy / Identity Disclosure

## Kontext

Nicht jede Interaktion benötigt die vollständige oder global stabile Identity eines Benutzers, Geräts, einer Anwendung oder eines Dienstes.

Die unnötige Offenlegung stabiler Identities ermöglicht Korrelation und Tracking über Anwendungen, Dienste und Systemgrenzen hinweg.

NovaOS benötigt deshalb Identity-Repräsentationen, die nur die für einen bestimmten Kontext notwendigen Identitätsinformationen offenlegen.

## Entscheidung

NovaOS unterstützt **Pseudonymous Identity** und **Selective Identity Disclosure** als native Bestandteile des Identity-Modells.

```text
Underlying Identity
        ↓
Identity Policy
        ↓
Pseudonym / Selective Identity View
        ↓
Consumer
```

Die zugrunde liegende Identity muss dem Consumer dabei nicht offengelegt werden.

## Grundprinzip

```text
Prove what is required,
not who you are globally.
```

Identity Disclosure folgt damit denselben Data-Minimization-Prinzipien wie andere sensible Daten.

## Pseudonymous Identity

NovaOS kann kontextgebundene Pseudonyme erzeugen, die eine Identity innerhalb eines definierten Scopes repräsentieren.

Beispielsweise:

```text
Identity
 ├── Pseudonym for Service A
 ├── Pseudonym for Service B
 └── Pseudonym for Application C
```

Unterschiedliche Pseudonyme sollen ohne entsprechende Authority nicht unnötig miteinander korrelierbar sein.

## Selective Identity

Ein Consumer kann anstelle der vollständigen Identity nur benötigte Identity Claims erhalten.

Beispiele:

```text
Age >= 18
Member of Organization X
Device is trusted
Application signed by Authority X
Credential strength >= required level
```

Der Consumer erhält damit die erforderliche Aussage, ohne zwangsläufig die vollständige zugrunde liegende Identity zu erfahren.

## Scope

Pseudonyme und Selective Identity Views können an einen definierten Kontext gebunden werden, beispielsweise:

- Application,
- Service,
- Session,
- Purpose,
- Trust Domain,
- Remote Party.

Globale stabile Identifier sollen nur offengelegt werden, wenn dies tatsächlich erforderlich und autorisiert ist.

## Authority und Trust

Ein Pseudonym oder Identity Claim erzeugt keine zusätzliche Authority.

```text
Identity Claim ≠ Capability
Pseudonym      ≠ Authority
```

Capabilities und Policies bestimmen weiterhin, welche Operationen erlaubt sind.

## Federation

Federated Identity Provider können über NovaOS ebenfalls pseudonyme oder selektive Identity Views bereitstellen.

Externe Provider dürfen dadurch nicht automatisch Zugriff auf die vollständige lokale NovaOS-Identity erhalten.

## De-Pseudonymisierung

Die Auflösung eines Pseudonyms auf die zugrunde liegende Identity muss separat autorisiert werden.

```text
Pseudonym
    ↓
Explicit Authority + Policy
    ↓
Underlying Identity
```

Wo keine solche Auflösung erforderlich ist, soll sie technisch vermieden werden.

## Normative Anforderungen

1. NovaOS MUSS pseudonyme Identity-Repräsentationen unterstützen können.
2. NovaOS MUSS selektive Offenlegung von Identity Claims unterstützen können.
3. Die vollständige stabile Identity SOLL nur offengelegt werden, wenn sie erforderlich ist.
4. Pseudonyme MÜSSEN an definierte Scopes bindbar sein.
5. Unterschiedliche Pseudonyme SOLLEN ohne entsprechende Authority nicht unnötig korrelierbar sein.
6. Selective Identity MUSS einzelne Claims ohne vollständige Identity-Offenlegung ermöglichen können.
7. Pseudonyme und Claims DÜRFEN keine automatische Authority erzeugen.
8. De-Pseudonymisierung MUSS separat autorisierbar sein.
9. Federation MUSS pseudonyme und selektive Identity Disclosure unterstützen können.
10. Identity Disclosure MUSS Data-Minimization- und Privacy-Policies berücksichtigen.
11. Anwendungen DÜRFEN nicht automatisch Zugriff auf globale stabile Identity-Identifier erhalten.
12. Sicherheitsrelevante Identity-Disclosure-Vorgänge MÜSSEN kontrolliert auditierbar sein.

## Konsequenzen

### Positive Konsequenzen

- weniger systemübergreifendes Tracking,
- geringere Offenlegung stabiler Identifier,
- datenschutzfreundliche lokale und federierte Identity,
- Identity Claims können ohne vollständige Identität verwendet werden,
- bessere Integration mit Data Minimization und Selective Disclosure.

### Negative Konsequenzen

- Pseudonym-Scopes und Bindings müssen verwaltet werden,
- bestimmte Legacy-Systeme erwarten globale stabile Identifier.

## Verworfene Alternativen

### Globale Identity überall offenlegen

Verworfen, da dies unnötige Korrelation und Tracking ermöglicht.

### Vollständig anonyme Nutzung als einziges Privacy-Modell

Verworfen, da viele Operationen überprüfbare Eigenschaften einer Identity benötigen.

### Anwendungsspezifische Ad-hoc-Pseudonyme

Verworfen, da Identity Privacy systemweit konsistent durchgesetzt werden soll.

## Abhängigkeiten

- `ADR-IDENTITY-0001_Identity_getrennt_von_Display_Name_Credential_Process_und_Address`
- `ADR-IDENTITY-0002_Stabile_Systemidentitäten`
- `ADR-IDENTITY-0007_Federated_Identity`
- `ADR-PRIVACY-0001_Data_Minimization`
- `ADR-PRIVACY-0005_Selective_Disclosure`
- `ADR-PRIVACY-0006_Privacy_Budgets`

## Zugehörige NPSPECs

- `NPSPEC-IDENTITY-PSEUDONYMOUS-IDENTITY-0001`
- `NPSPEC-IDENTITY-SELECTIVE-IDENTITY-0001`
- `NPSPEC-IDENTITY-IDENTITY-CLAIM-0001`
- `NPSPEC-IDENTITY-PSEUDONYM-SCOPE-0001`
- `NPSPEC-IDENTITY-DEPSEUDONYMIZATION-0001`
- `NPSPEC-IDENTITY-PRIVACY-TEST-0001`

## Ergebnis

NovaOS kann eine Identity kontextabhängig repräsentieren, ohne ihre globale stabile Identität offenzulegen:

```text
Stable Identity
      ↓
Privacy Policy
      ↓
Pseudonym / Selected Claims
      ↓
Consumer
```

Damit wird Identity selbst zu einem selektiv offenlegbaren und datenschutzkontrollierten Bestandteil der NovaOS-Architektur.