# ADR-IDENTITY-0013 – Identity Lifecycle und Revocation

## Status

Angenommen

## Kategorie

Identity Architecture / Lifecycle / Revocation

## Kontext

NovaOS-Identitäten besitzen unterschiedliche Lebensdauern.

Benutzer-, Application-, Service- oder Device-Identities können langfristig bestehen, während Workload Identities bewusst kurzlebig sind.

Identitäten müssen erzeugt, aktiviert, geändert, deaktiviert und endgültig widerrufen werden können, ohne Identity, Credential und Authority miteinander zu vermischen.

## Entscheidung

NovaOS definiert einen **expliziten Identity Lifecycle mit systemweiter Revocation-Semantik**.

Konzeptionell:

```text
Created
   ↓
Active
   ↓
Suspended
   ↓
Active
   ↓
Revoked
```

`Revoked` ist ein terminaler Zustand für die betreffende Identity.

Die konkreten Zustandsübergänge werden in den NPSPECs definiert.

## Grundprinzip

```text
Identity Lifecycle
≠
Credential Lifecycle
≠
Capability Lifecycle
```

Der Widerruf eines Credentials widerruft nicht automatisch die Identity.

Der Widerruf einer Identity muss dagegen bei allen sicherheitsrelevanten Bindings dieser Identity berücksichtigt werden.

## Suspension und Revocation

NovaOS unterscheidet zwischen temporärer Deaktivierung und endgültigem Widerruf.

```text
Suspended → temporär nicht verwendbar
Revoked   → dauerhaft ungültig
```

Eine suspendierte Identity kann nach erfolgreicher Policy-Prüfung wieder aktiviert werden.

Eine widerrufene Identity darf nicht reaktiviert oder einer neuen Entität zugewiesen werden.

## Auswirkungen der Revocation

Identity Revocation muss auf abhängige Sicherheitskontexte wirken können.

Dazu gehören insbesondere:

- aktive Authentication Sessions,
- Identity Bindings,
- Delegated Contexts,
- gebundene Workload Identities,
- Identity-gebundene Capabilities,
- Federation Bindings.

Die genaue Propagation wird durch die jeweiligen Lifecycle- und Security-Contracts bestimmt.

## Stabile und ephemere Identities

Stabile Identities besitzen einen explizit verwalteten Lifecycle.

Ephemeral Workload Identities enden grundsätzlich mit ihrem zugehörigen Workload-Lifecycle.

```text
Stable Identity    → managed lifecycle
Ephemeral Identity → workload-bound lifecycle
```

## Verteilte Systeme

Revocation muss auch bei verteilten NovaOS-Komponenten berücksichtigt werden.

Zwischengespeicherte Identity-Informationen dürfen eine bekannte Revocation nicht dauerhaft ignorieren.

Leases, Epochs oder vergleichbare Mechanismen können verwendet werden, um verteilte Revocation sicher umzusetzen.

## Identity History

Eine widerrufene Identity darf nicht gelöscht und anschließend für eine andere Entität wiederverwendet werden, wenn dadurch historische Audit-, Provenance- oder Security-Beziehungen mehrdeutig würden.

Historische Referenzen müssen weiterhin eindeutig auf die frühere Identity zeigen können.

## Normative Anforderungen

1. NovaOS MUSS einen expliziten Lifecycle für Identities definieren.
2. Identity Lifecycle MUSS vom Credential- und Capability-Lifecycle getrennt sein.
3. Identities MÜSSEN deaktiviert und widerrufen werden können.
4. Suspension MUSS von endgültiger Revocation unterscheidbar sein.
5. Eine widerrufene Identity DARF NICHT reaktiviert werden.
6. Widerrufene Identity-Identifier DÜRFEN NICHT einer anderen Entität neu zugewiesen werden.
7. Identity Revocation MUSS relevante abhängige Security Contexts invalidieren können.
8. Ephemeral Workload Identities MÜSSEN an ihren Workload-Lifecycle gebunden sein.
9. Verteilte Komponenten MÜSSEN Identity Revocation berücksichtigen.
10. Revocation DARF durch veraltete Caches nicht unbegrenzt umgangen werden.
11. Historische Identity-Referenzen MÜSSEN eindeutig bleiben.
12. Lifecycle- und Revocation-Ereignisse MÜSSEN kontrolliert auditierbar und introspektierbar sein.

## Konsequenzen

### Positive Konsequenzen

- klar definierter Identity Lifecycle,
- sichere Stilllegung kompromittierter oder nicht mehr gültiger Identities,
- historische Identitäten bleiben eindeutig,
- verteilte Revocation wird Teil der Systemarchitektur,
- saubere Trennung von Identity-, Credential- und Capability-Revocation.

### Negative Konsequenzen

- abhängige Identity Bindings müssen Revocation berücksichtigen,
- verteilte Systeme benötigen geeignete Revocation- und Cache-Mechanismen.

## Verworfene Alternativen

### Identity durch Löschen widerrufen

Verworfen, da historische Referenzen und Audit-Beziehungen verloren gehen könnten.

### Credential Revocation als Identity Revocation behandeln

Verworfen, da eine Identity mehrere austauschbare Credentials besitzen kann.

### Wiederverwendung alter Identity-Identifier

Verworfen, da dadurch historische und sicherheitsrelevante Beziehungen mehrdeutig würden.

## Abhängigkeiten

- `ADR-IDENTITY-0002_Stabile_Systemidentitäten`
- `ADR-IDENTITY-0003_Ephemeral_Workload_Identity`
- `ADR-IDENTITY-0012_Identity_Recovery`
- `ADR-CAPABILITY-0009_Capability_Revocation`
- `ADR-SECURITY-0010_Capability_Revocation`
- `ADR-SECURITY-0011_Capability_Leases_und_Epochs`

## Zugehörige NPSPECs

- `NPSPEC-IDENTITY-LIFECYCLE-0001`
- `NPSPEC-IDENTITY-REVOCATION-0001`
- `NPSPEC-IDENTITY-SUSPENSION-0001`
- `NPSPEC-IDENTITY-REVOCATION-PROPAGATION-0001`
- `NPSPEC-IDENTITY-LIFECYCLE-TEST-0001`

## Ergebnis

NovaOS behandelt Identitäten als lifecyclefähige Systemobjekte:

```text
Identity
   ↓
Lifecycle
   ↓
Active / Suspended / Revoked
   ↓
Bindings + Security Contexts
```

Damit können Identities kontrolliert stillgelegt und widerrufen werden, ohne Credential-, Capability- und Identity-Lifecycle miteinander zu vermischen.