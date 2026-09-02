# ADR-SECURITY-0007 – RBAC für administrative Rollen

## Status

Angenommen

## Kategorie

Security / Authorization / Administration / RBAC

## Kontext

NovaOS verwendet Capabilities als natives Authority-Modell und ABAC für kontextabhängige Policyentscheidungen.

Für die Systemadministration ist es dennoch sinnvoll, zusammengehörige administrative Verantwortlichkeiten über verständliche Rollen abzubilden.

Beispiele sind:

```text
System Administrator
Security Administrator
Device Administrator
Network Administrator
Storage Administrator
User Administrator
Auditor
```

Eine Rolle darf dabei nicht selbst zu einem Root-ähnlichen universellen Privileg werden.

---

## Entscheidung

NovaOS unterstützt **Role-Based Access Control (RBAC)** für administrative Rollen.

RBAC dient dabei der strukturierten Zuweisung von administrativen Capability-Sets und Policies.

```text
Administrative Role
        ↓
Assigned Capabilities
        ↓
ABAC / MAC / Hard Constraints
        ↓
Authorized Operation
```

Die Rolle selbst ist keine native Authority.

---

## Grundprinzip

```text
Roles organize authority.
Capabilities represent authority.
```

---

## Administrative Rollen

Rollen beschreiben administrative Verantwortungsbereiche.

Beispielsweise:

```text
Network Administrator
 ├── Configure Network
 ├── Manage Network Providers
 └── Modify Network Policies
```

oder:

```text
Security Administrator
 ├── Manage Security Policies
 └── Manage Trust Configuration
```

Die konkreten Capabilities einer Rolle müssen explizit definiert sein.

---

## Keine universelle Administratorrolle

NovaOS definiert keine Rolle, die automatisch sämtliche Sicherheitsgrenzen umgehen kann.

Auch eine breit angelegte administrative Rolle besteht lediglich aus einem definierten Capability-Set.

```text
Administrator
    ≠
Universal Authority
```

Dies gilt auch für interne Systemrollen.

---

## Rollen und Capabilities

Die Aktivierung oder Zuweisung einer Rolle kann zur Bereitstellung der zugehörigen Capabilities führen.

```text
Identity
   ↓
Role Assignment
   ↓
Policy Evaluation
   ↓
Capability Set
```

Capabilities können dabei zusätzlich:

- zeitlich begrenzt,
- kontextgebunden,
- zielgebunden,
- delegationsbeschränkt

sein.

---

## Rollen und ABAC

RBAC wird in die ABAC-Policyarchitektur integriert.

Eine Rolle kann als Attribut einer Policyentscheidung verwendet werden:

```text
Subject.Role = NetworkAdministrator
```

Die Rolle allein muss jedoch nicht ausreichen.

Weitere Bedingungen wie Trust, Execution Domain, Systemzustand oder Zielressource können zusätzlich erforderlich sein.

---

## Separation of Duties

Administrative Verantwortlichkeiten sollen auf mehrere Rollen verteilt werden können.

Beispielsweise können:

```text
Security Policy Administration
Trust Anchor Administration
Audit Administration
Capability Issuance
```

getrennte Rollen besitzen.

Für besonders kritische Operationen kann NovaOS mehrere unabhängige Authorities verlangen.

---

## Temporäre Rollenaktivierung

Administrative Rollen müssen nicht dauerhaft aktiv sein.

NovaOS soll eine temporäre Aktivierung unterstützen:

```text
Normal Context
      ↓
Authorized Elevation
      ↓
Administrative Role
      ↓
Temporary Capabilities
      ↓
Expiration / Revocation
```

Damit wird dauerhafte administrative Ambient Authority reduziert.

---

## Rollenvererbung

Rollenhierarchien dürfen unterstützt werden, müssen aber explizit definiert sein.

Eine Rollenbeziehung darf keine unbeabsichtigte Erweiterung von Authority verursachen.

Komplexe oder implizite Rollenvererbung soll vermieden werden.

---

## Legacy Compatibility

Klassische Administrator-, Gruppen- oder Rollenmodelle anderer Betriebssysteme können innerhalb von Compatibility Domains auf NovaOS-Rollen und Capabilities abgebildet werden.

Eine Legacy-Administratorrolle erzeugt dadurch keine native universelle NovaOS-Authority.

---

## Audit und Introspection

NovaOS muss administrative Authority auf ihre Herkunft zurückführen können.

```text
Identity
   ↓
Role
   ↓
Capability
   ↓
Operation
```

Damit muss erkennbar sein, welche Rolle zur Bereitstellung einer administrativen Capability geführt hat.

---

## Normative Anforderungen

1. NovaOS MUSS RBAC für administrative Rollen unterstützen.
2. Rollen DÜRFEN NICHT selbst als native Authority behandelt werden.
3. Administrative Authority MUSS durch konkrete Capabilities repräsentiert werden.
4. NovaOS DARF keine universelle Administratorrolle mit allgemeiner Security-Bypass-Authority definieren.
5. Rollen MÜSSEN explizit definierte Capability-Sets besitzen.
6. RBAC MUSS in die ABAC-Policyarchitektur integrierbar sein.
7. MAC- und andere Hard Constraints DÜRFEN durch Rollen NICHT umgangen werden.
8. Administrative Rollen MÜSSEN nach Verantwortungsbereichen trennbar sein.
9. Separation of Duties MUSS durch getrennte Rollen unterstützt werden können.
10. Administrative Rollen SOLLEN temporär aktivierbar sein.
11. Aus Rollen bereitgestellte Capabilities SOLLEN zeitlich und kontextuell begrenzbar sein.
12. Rollenvererbung MUSS explizit und kontrollierbar sein.
13. Legacy-Administratorrollen DÜRFEN keine universelle native NovaOS-Authority erzeugen.
14. Rollenzuweisungen und daraus resultierende administrative Authority MÜSSEN auditierbar und introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- verständliche administrative Verantwortungsbereiche,
- einfache Verwaltung zusammengehöriger Capabilities,
- Unterstützung von Separation of Duties,
- keine Rückkehr zu einem universellen Root-Modell,
- gute Integration mit ABAC und Capability-Based Security.

### Negative Konsequenzen

- Rollen und Capability-Sets müssen konsistent gepflegt werden,
- zu breite Rollen können Least Privilege schwächen,
- komplexe Rollenhierarchien können schwer nachvollziehbar werden.

---

## Verworfene Alternativen

### RBAC als primäres Autorisierungsmodell

Verworfen.

Rollen sind für die allgemeine feingranulare Authority von NovaOS zu grob.

### Universelle Administratorrolle

Verworfen.

Dies würde dem Grundsatz widersprechen, dass NovaOS kein universelles Root-Privileg besitzt.

### Ausschließlich individuelle Capabilities ohne Rollen

Verworfen.

Dies würde die Verwaltung wiederkehrender administrativer Verantwortlichkeiten unnötig erschweren.

---

## Abhängigkeiten

- `ADR-SECURITY-0001_Capability_Based_Security_als_zentrales_Autorisierungsmodell`
- `ADR-SECURITY-0002_Kein_universelles_Root_Privileg`
- `ADR-SECURITY-0003_Least_Privilege_und_Privilege_Separation`
- `ADR-SECURITY-0005_MAC_für_systemweite_harte_Sicherheitsregeln`
- `ADR-SECURITY-0006_ABAC_als_zentrale_Policyentscheidung`

---

## Zugehörige NPSPECs

- `NPSPEC-SECURITY-RBAC-0001`
- `NPSPEC-SECURITY-ADMIN-ROLE-0001`
- `NPSPEC-SECURITY-ROLE-ASSIGNMENT-0001`
- `NPSPEC-SECURITY-ROLE-ACTIVATION-0001`
- `NPSPEC-SECURITY-SEPARATION-OF-DUTIES-0001`
- `NPSPEC-SECURITY-RBAC-TEST-0001`

---

## Ergebnis

RBAC bildet in NovaOS administrative Verantwortlichkeiten ab, ohne ein zusätzliches Authority-Modell einzuführen.

```text
Identity
   ↓
Administrative Role
   ↓
Capabilities
   ↓
ABAC / MAC
   ↓
Operation
```

Rollen organisieren administrative Authority.

Die tatsächliche Authority bleibt capability-basiert und unterliegt weiterhin Least Privilege sowie den verbindlichen Policies von NovaOS.