# ADR-VERIFY-0005 – Capability Safety

## Status

Angenommen

## Kategorie

Verification / Capability Security / Authorization / Correctness

## Kontext

NovaOS verwendet Capabilities als zentrale Grundlage für Zugriff und Autorisierung.

Fehlerhafte Capability-Behandlung kann dazu führen, dass Komponenten mehr Rechte erhalten als vorgesehen, Rechte unkontrolliert weitergeben oder bereits entzogene Berechtigungen weiterhin verwenden.

Capability Safety muss daher als überprüfbare Systemeigenschaft behandelt werden.

## Entscheidung

NovaOS definiert **Capability Safety als Menge verbindlicher Invarianten**, die für Erzeugung, Nutzung, Delegation, Attenuation und Revocation von Capabilities gelten.

```text
Authority
   ↓
Capability
   ↓
Validated Operation
   ↓
Authorized Resource
```

Eine Operation darf nur die Autorität ausüben, die durch eine gültige Capability explizit gewährt wurde.

## Zentrale Invariante

Eine Komponente darf ihre Autorität nicht selbstständig erweitern.

```text
Authority(after)
⊆
Authority(before)
+
Explicitly Granted Authority
```

Neue Rechte können ausschließlich über einen autorisierten Grant entstehen.

## Capability Validation

Vor sicherheitsrelevanter Nutzung müssen mindestens folgende Eigenschaften prüfbar sein:

```text
Identity
Validity
Scope
Rights
Lifetime
Revocation State
Target
Constraints
```

Eine strukturell gültige Capability ist nicht automatisch aktuell autorisiert.

## Delegation

Delegation darf keine stärkere Capability erzeugen als die Ausgangs-Capability erlaubt.

```text
Parent Capability
       ↓
Delegation
       ↓
Equal or Reduced Authority
```

Eine Erweiterung der Rechte erfordert eine separate autorisierte Capability.

## Attenuation

Capabilities müssen gezielt eingeschränkt werden können.

Beispiel:

```text
Read + Write + Delete
        ↓
Attenuation
        ↓
Read
```

Attenuation darf niemals zusätzliche Autorität erzeugen.

## Revocation

Entzogene Capabilities dürfen nach wirksamer Revocation nicht weiter als gültige Autorisierung akzeptiert werden.

Das Revocation-Modell muss definieren, wann die Entziehung wirksam wird.

## Capability Boundaries

Capability-Prüfungen müssen an tatsächlichen Authority Boundaries stattfinden.

Dazu gehören insbesondere:

```text
Syscalls
IPC
Service Calls
Driver Interfaces
Remote Capability Calls
Delegation
```

Interne Annahmen dürfen externe Capability-Prüfungen nicht ersetzen.

## Verification

Kritische Capability-Mechanismen sollen besonders überprüft werden.

Geeignete Verfahren sind:

```text
Formal Specification
Model Checking
Static Analysis
Property Testing
Fuzzing
Runtime Assertions
```

Besonders relevant sind Delegation, Attenuation und Revocation.

## Normative Anforderungen

1. Jede privilegierte Operation MUSS durch gültige Autorität gedeckt sein.
2. Komponenten DÜRFEN ihre Autorität nicht selbstständig erweitern.
3. Capability-Nutzung MUSS Scope, Rights und Gültigkeit berücksichtigen.
4. Delegation DARF keine zusätzliche Autorität erzeugen.
5. Attenuation DARF Rechte ausschließlich erhalten oder reduzieren.
6. Revocation MUSS definierte Capabilities wirksam ungültig machen können.
7. Capability-Prüfungen MÜSSEN an Authority Boundaries erfolgen.
8. Kritische Capability-Invarianten SOLLEN formal spezifizierbar sein.
9. Fehler bei der Capability-Prüfung MÜSSEN nach Fail-Safe-Default behandelt werden.

## Konsequenzen

### Positiv

- verhindert unbeabsichtigte Rechteausweitung,
- Capability Delegation wird überprüfbar,
- zentrale Security-Eigenschaften können formal getestet werden,
- klare Grundlage für Sandboxen, Services und Agents.

### Negativ

- Capability-Lifecycle und Revocation benötigen zusätzliche Prüfmechanismen,
- Authority-Flows müssen konsequent modelliert werden.

## Abhängigkeiten

- `ADR-VERIFY-0001_Formale_Spezifikation_für_kritische_Komponenten`
- `ADR-VERIFY-0002_Model_Checking_für_kritische_State_Machines`
- `ADR-VERIFY-0004_Type_Safety`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-CAPABILITY-0001`
- `ADR-CAPABILITY-0009_Delegation`
- `ADR-CAPABILITY-0010_Attenuation`
- `ADR-CAPABILITY-0011_Revocation`
- `ADR-SECURITY-0001`

## Ergebnis

NovaOS behandelt Capability Safety als überprüfbare Sicherheitsinvariante:

```text
No Authority
Without Capability

No Authority Expansion
Without Explicit Grant
```

Damit bleibt nachvollziehbar und überprüfbar, welche Komponente welche Autorität besitzt und wie diese Autorität weitergegeben oder eingeschränkt werden darf.