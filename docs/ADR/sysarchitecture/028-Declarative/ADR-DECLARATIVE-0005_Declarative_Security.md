# ADR-DECLARATIVE-0005 – Declarative Security

## Status

Angenommen

## Kategorie

Declarative Architecture / Security

## Kontext

Sicherheitszustände werden traditionell häufig durch einzelne imperative Operationen konfiguriert:

```text
Add Permission
Enable Firewall Rule
Revoke Credential
Configure Encryption
```

Dadurch beschreibt die Konfiguration primär einzelne Änderungen statt des tatsächlich gewünschten Sicherheitszustands.

NovaOS benötigt ein deklaratives Modell, das beschreibt, **welcher Sicherheitszustand gelten soll**.

## Entscheidung

NovaOS unterstützt **Declarative Security**.

Sicherheitsanforderungen werden als Desired Security State beschrieben.

```text
Desired Security State
        ↓
Security Reconciliation
        ↓
Actual Security State
        ↓
Verification
```

## Desired Security State

Deklarative Sicherheitsregeln können beispielsweise beschreiben:

```text
Permission = Denied
Encryption = Required
Network Exposure = LocalOnly
Trust Level = Required
Capability = Revoked
Credential = Disabled
```

Die Definition beschreibt das gewünschte Ergebnis, nicht die dafür notwendigen Einzelschritte.

## Actual Security State

NovaOS ermittelt den tatsächlich wirksamen Sicherheitszustand unabhängig vom Desired State.

```text
Desired:
    Encryption = Required

Actual:
    Encryption = Disabled
```

Eine solche Abweichung wird als Security Drift behandelt.

## Security Reconciliation

Security Reconciliation folgt dem allgemeinen Reconciliation-Modell:

```text
Observe
   ↓
Compare
   ↓
Evaluate Policy
   ↓
Plan
   ↓
Execute
   ↓
Verify
```

Sicherheitsänderungen müssen über autorisierte und kontrollierte Systemoperationen erfolgen.

## Hard Constraints

Security Policies können harte Constraints darstellen.

```text
Security Constraint
       ↓
Mandatory
```

Adaptive Optimierung, Performance oder Benutzerpräferenzen dürfen solche Constraints nicht überschreiben.

## Konflikte

Widersprechen sich mehrere Sicherheitsanforderungen, gilt keine implizite Abschwächung.

```text
Policy A
   ↓
Conflict
   ↑
Policy B
```

Der Konflikt muss anhand definierter Security- und Policy-Regeln aufgelöst werden.

Kann keine sichere Entscheidung getroffen werden, gilt Fail-Safe Default.

## Drift

Verändert sich der Actual Security State außerhalb des vorgesehenen Zustands:

```text
Desired Security State
        ≠
Actual Security State
```

muss NovaOS die Abweichung erkennen und entsprechend der geltenden Policy reagieren können.

## Verification

Eine ausgeführte Sicherheitsoperation gilt erst nach Verification als erfolgreich.

```text
Security Change
      ↓
Observe
      ↓
Verify
      ↓
Desired == Actual
```

Ein erfolgreicher API-Aufruf allein reicht nicht aus.

## Normative Anforderungen

1. NovaOS MUSS Sicherheitszustände deklarativ beschreiben können.
2. Desired Security State und Actual Security State MÜSSEN getrennt behandelt werden.
3. Security Drift MUSS erkennbar sein.
4. Sicherheitsänderungen MÜSSEN über autorisierte Mechanismen erfolgen.
5. Security Constraints DÜRFEN nicht durch adaptive oder leistungsbezogene Policies abgeschwächt werden.
6. Sicherheitskonflikte MÜSSEN explizit behandelt werden.
7. Bei nicht eindeutig sicherer Entscheidung MUSS Fail-Safe Default gelten.
8. Sicherheitsänderungen MÜSSEN nach ihrer Ausführung verifiziert werden.
9. Ein gewünschter Sicherheitszustand DARF erst nach Verification als erreicht gelten.

## Konsequenzen

### Positive Konsequenzen

- Sicherheitszustände werden systemweit konsistent beschreibbar,
- Security Drift kann automatisch erkannt werden,
- Self-Healing kann Sicherheitszustände wiederherstellen,
- imperative Einzelkonfigurationen werden reduziert.

### Negative Konsequenzen

- tatsächliche Sicherheitszustände müssen zuverlässig beobachtbar sein,
- Konflikte zwischen Policies benötigen eindeutige Regeln.

## Abhängigkeiten

- `ADR-DECLARATIVE-0001_Desired_State_und_Actual_State`
- `ADR-DECLARATIVE-0002_State_Reconciliation`
- `ADR-DECLARATIVE-0003_Declarative_Resources`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-SECURITY-0016_Data_Sovereignty_als_Hard_Constraint`
- `ADR-OBJECT-0005_Object_Permissions`

## Zugehörige NPSPECs

- `NPSPEC-DECLARATIVE-SECURITY-0001`
- `NPSPEC-DECLARATIVE-SECURITY-STATE-0001`
- `NPSPEC-DECLARATIVE-SECURITY-RECONCILIATION-0001`

## Ergebnis

NovaOS behandelt Sicherheit als deklarativ verwalteten Systemzustand:

```text
Security Policy
      ↓
Desired Security State
      ↓
Reconciliation
      ↓
Actual Security State
      ↓
Verification
```

Der zentrale Grundsatz lautet:

```text
Sicherheit beschreibt nicht,
welche Befehle ausgeführt werden sollen,

sondern welcher sichere Zustand
dauerhaft gelten muss.
```