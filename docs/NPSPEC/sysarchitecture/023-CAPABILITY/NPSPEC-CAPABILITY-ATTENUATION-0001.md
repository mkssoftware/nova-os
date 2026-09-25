# NPSPEC-CAPABILITY-ATTENUATION-0001 – Nova Capability Attenuation

## Status

Angenommen

## Kategorie

Capability / Attenuation / Security

## Zweck

NovaOS definiert Capability Attenuation als kontrollierte Einschränkung bestehender Autorität.

```text
Capability A
     ↓
Attenuation
     ↓
Capability B
```

Dabei gilt zwingend:

```text
Authority(B) ⊆ Authority(A)
```

Attenuation darf Autorität ausschließlich erhalten oder reduzieren, niemals erweitern.

## Grundprinzipien

```text
Attenuation ≠ Delegation
Attenuation ≠ Revocation
Attenuation ≠ Authorization
Attenuation ≠ Trust
Restriction ≠ New Authority
Derived Capability ≠ Expanded Capability
```

## Attenuation-Modell

Eine Attenuation kann mehrere Dimensionen gleichzeitig einschränken:

```text
Capability
├── Rights
├── Target Scope
├── Purpose
├── Time
├── Location
├── Security Domain
├── Resource Budget
├── Delegation
└── Trust Requirements
```

## Rechte

Beispiel:

```text
Original:
Read + Write + Delete

        ↓

Attenuated:
Read + Write
```

Nicht erlaubt:

```text
Original:
Read

        ↓

Derived:
Read + Write
```

## Scope

Der gültige Objektbereich kann reduziert werden.

```text
Original:
Read(System:/Documents/*)

        ↓

Attenuated:
Read(System:/Documents/report.pdf)
```

Eine Ableitung darf den Scope nicht erweitern.

## Zeit

Capabilities können zeitlich eingeschränkt werden.

```text
Original:
Valid until 18:00

        ↓

Attenuated:
Valid until 15:00
```

Eine Attenuation darf die ursprüngliche Gültigkeit nicht verlängern.

## Zweckbindung

Eine allgemeine Capability kann auf eine konkrete Aufgabe begrenzt werden.

```text
Original:
Write(File X)

        ↓

Attenuated:
Write(File X)
Purpose = "Document Conversion"
```

Die Zweckbindung darf anschließend nicht wieder entfernt werden.

## Delegierbarkeit

Attenuation kann Delegation einschränken.

```text
Original:
Delegable

        ↓

Attenuated:
NonDelegable
```

Nicht zulässig ist:

```text
NonDelegable → Delegable
```

## Resource Budget

Capabilities können an Ressourcenlimits gebunden werden.

```text
CPU ≤ 500 ms
Memory ≤ 64 MiB
Network ≤ 10 MiB
Storage ≤ 100 MiB
```

Abgeleitete Budgets dürfen bestehende Hard Limits nicht überschreiten.

## Constraint-Komposition

Mehrere Attenuationsschritte müssen monoton restriktiv bleiben.

```text
Capability A
    ↓
Capability B
    ↓
Capability C
```

Es gilt:

```text
Authority(C)
⊆ Authority(B)
⊆ Authority(A)
```

Constraints dürfen auf keiner Stufe gelockert werden.

## Delegation

Attenuation ist der zentrale Mechanismus für sichere Capability Delegation.

```text
Source Capability
       ↓
Attenuation
       ↓
Derived Capability
       ↓
Delegation
```

Dadurch erhält der Empfänger nur die tatsächlich benötigte Autorität.

## Composition

Auch zusammengesetzte Capabilities müssen Attenuation unterstützen.

```text
Composite Capability
├── Read(File A)
├── Write(File B)
└── Use(Service C)

        ↓

Attenuated Composite
├── Read(File A)
└── Use(Service C)
```

Attenuation darf keine neuen Komponenten hinzufügen.

## ExecutionContract

Attenuation kann aus einem `Nova.ExecutionContract` abgeleitet werden.

```text
ExecutionContract
       ↓
Required Authority
       ↓
Attenuation
       ↓
Minimal Capability
```

Damit kann NovaOS für eine konkrete Operation möglichst exakt begrenzte Autorität erzeugen.

## Revocation

Attenuation ersetzt keine Revocation.

```text
Attenuation → reduziert Autorität
Revocation  → entzieht Autorität
```

Wird eine Quell-Capability widerrufen, müssen abhängige attenuierte Capabilities entsprechend der Revocation-Policy neu bewertet werden.

## Introspection

Autorisierte Komponenten sollen nachvollziehen können:

```text
Source CapabilityID
Derived CapabilityID
Removed Rights
Added Constraints
Effective Authority
Attenuation Chain
State
```

Damit bleibt die Herkunft effektiver Autorität nachvollziehbar.

## Normative Anforderungen

1. Attenuation DARF Autorität ausschließlich erhalten oder reduzieren.
2. Eine attenuierte Capability MUSS innerhalb der Autorität ihrer Quelle liegen.
3. Rechte DÜRFEN NICHT erweitert werden.
4. Target Scope DARF NICHT erweitert werden.
5. Zeitliche Gültigkeit DARF NICHT verlängert werden.
6. Constraints DÜRFEN NICHT gelockert werden.
7. Nicht delegierbare Autorität DARF NICHT delegierbar gemacht werden.
8. Mehrstufige Attenuation MUSS monoton restriktiv bleiben.
9. Attenuation SOLL mit Delegation, Composition und `Nova.ExecutionContract` integrierbar sein.
10. Attenuation-Ketten SOLLEN introspektierbar und auditierbar sein.

## Abhängigkeiten

- `NPSPEC-CAPABILITY-IDENTITY-0001`
- `NPSPEC-CAPABILITY-TOKEN-0001`
- `NPSPEC-CAPABILITY-COMPOSITION-0001`
- `NPSPEC-CAPABILITY-NEGOTIATION-0001`
- `NPSPEC-CAPABILITY-DELEGATION-0001`
- `NPSPEC-SECURITY-CAPABILITY-0001`
- `NPSPEC-SECURITY-DELEGATION-0001`
- `NPSPEC-SECURITY-REVOCATION-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `ADR-CAPABILITY-0008`

## Ergebnis

```text
Broad Capability
       ↓
Attenuation
       ↓
Minimal Required Authority
       ↓
Purpose-bound Capability
       ↓
Operation
```

NovaOS erhält damit einen einheitlichen Mechanismus zur monotonen Einschränkung von Capabilities und kann Autorität präzise auf die Rechte, Ressourcen, Ziele und Zeiträume begrenzen, die für eine konkrete Operation tatsächlich benötigt werden.