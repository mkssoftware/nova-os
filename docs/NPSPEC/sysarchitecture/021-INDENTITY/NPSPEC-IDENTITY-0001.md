# NPSPEC-IDENTITY-0001 – Nova Identity Architecture

## Status

Angenommen

## Kategorie

Identity / Architecture / Core

## Zweck

NovaOS definiert ein einheitliches Identitätsmodell für Benutzer, Prozesse, Dienste, Geräte und andere identifizierbare Systemobjekte.

```text
Entity
  ↓
Stable Identity
  ↓
Authentication / Policy
  ↓
Security Context
```

Identität beschreibt, **wer oder was eine Entität ist**, erzeugt aber allein keine Autorität.

## Grundprinzipien

```text
Identity ≠ Authority
Identity ≠ Capability
Authentication ≠ Authorization
Name ≠ Identity
Location ≠ Identity
Account ≠ Person
```

## Identitätsmodell

Eine Identität besitzt mindestens:

```text
Identity
├── IdentityID
├── Type
└── State
```

Optional:

```text
Display Name
Attributes
Credentials
Security Domain
Trust Information
Provider
Parent / Organization
Metadata
```

`IdentityID` muss eine stabile logische Identität darstellen.

## Identitätstypen

NovaOS muss unterschiedliche Identitätstypen unterstützen können:

```text
User
Service
Application
Process
Device
Machine
Organization
Remote Principal
System Component
```

Neue Identitätstypen müssen erweiterbar sein.

## Identität und Name

Anzeigenamen und technische Namen dürfen geändert werden, ohne die Identität zu verändern.

```text
IdentityID = Stable

Name
Location
Provider
Device
      ↓
may change
```

Mehrere Namen oder Aliase dürfen auf dieselbe Identität verweisen.

## Identität und Autorität

Eine erfolgreich identifizierte Entität erhält nicht automatisch Zugriff auf Ressourcen.

```text
Identity
   ↓
Authentication
   ↓
Security Principal
   ↓
Policy + Capabilities
   ↓
Authority
```

Die tatsächliche Autorität wird durch die NovaOS-Sicherheitsarchitektur bestimmt.

## Lokale und entfernte Identitäten

Das Identitätsmodell muss lokale und entfernte Identitäten unterstützen.

```text
Local Identity
Remote Identity
Federated Identity
```

Eine Remote Identity darf nicht allein aufgrund ihrer Herkunft als vertrauenswürdig gelten.

```text
Remote Identity ≠ Trusted Identity
```

## Identitätsprovider

Identitäten dürfen von unterschiedlichen Providern stammen:

```text
Local NovaOS
Organization
External Identity Provider
Hardware Identity
Remote System
```

Provider müssen über definierte Trust Policies eingebunden werden.

Die logische Identität soll nicht unnötig an einen einzelnen Provider gebunden sein.

## Attribute

Identitäten dürfen Attribute besitzen:

```text
User Type
Organization
Device Class
Security Domain
Role References
Privacy Properties
```

Attribute liefern Kontext für Policies, stellen jedoch selbst keine Capabilities dar.

## Lebenszyklus

Identitäten müssen einen kontrollierten Lebenszyklus besitzen:

```text
Create
  ↓
Active
  ↓
Suspended
  ↓
Disabled
  ↓
Retired
```

Die Deaktivierung einer Identität muss bestehende Sessions, Credentials und delegierte Autorität berücksichtigen können.

## Privacy

NovaOS soll unnötige globale Identifizierbarkeit vermeiden.

Wo eine vollständige Identität nicht benötigt wird, sollen verwendet werden können:

```text
Pseudonymous Identity
Scoped Identity
Selective Disclosure
Derived Claims
```

Dadurch kann beispielsweise ein Dienst eine Eigenschaft prüfen, ohne die vollständige Identität zu erhalten.

## Introspection

NovaOS soll mindestens darstellen können:

```text
IdentityID
Type
State
Provider
Security Domain
Trust State
```

Sensible Identitätsattribute dürfen nur bei entsprechender Autorisierung sichtbar sein.

## Normative Anforderungen

1. NovaOS MUSS ein einheitliches Identitätsmodell bereitstellen.
2. Jede Identität MUSS eine stabile `IdentityID` besitzen können.
3. Name, Standort und Provider DÜRFEN NICHT mit der Identität gleichgesetzt werden.
4. Identität DARF NICHT automatisch Autorität erzeugen.
5. Lokale, entfernte und föderierte Identitäten MÜSSEN unterstützt werden können.
6. Identitätsprovider MÜSSEN über Trust Policies kontrollierbar sein.
7. Identitätsattribute DÜRFEN NICHT automatisch als Capabilities behandelt werden.
8. Identitäten MÜSSEN deaktivierbar und widerrufbar sein.
9. Pseudonyme und zweckgebundene Identitäten SOLLEN unterstützt werden.
10. Identitätszustände SOLLEN sicher introspektierbar und auditierbar sein.

## Abhängigkeiten

- `NPSPEC-SECURITY-CAPABILITY-0001`
- `NPSPEC-SECURITY-RBAC-0001`
- `NPSPEC-SECURITY-ABAC-0001`
- `NPSPEC-SECURITY-REVOCATION-0001`
- `NPSPEC-PRIVACY-MINIMIZATION-0001`
- `NPSPEC-PRIVACY-SELECTIVEDISCLOSURE-0001`
- `ADR-IDENTITY-0001`

## Ergebnis

```text
Entity
  ↓
Stable IdentityID
  ↓
Authentication + Trust
  ↓
Security Context
  ↓
Policy + Capabilities
  ↓
Authorized Operation
```

NovaOS erhält damit ein einheitliches, providerunabhängiges Identitätsmodell, bei dem Identität klar von Authentifizierung, Vertrauen und tatsächlicher Autorität getrennt bleibt.