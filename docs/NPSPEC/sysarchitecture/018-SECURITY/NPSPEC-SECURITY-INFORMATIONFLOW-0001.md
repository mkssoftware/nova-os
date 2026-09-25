# NPSPEC-SECURITY-INFORMATIONFLOW-0001 – Nova Information Flow Security

## Status

Angenommen

## Kategorie

Security / Information Flow / Data Protection

## Zweck

NovaOS definiert Information Flow Security zur Kontrolle, wohin Daten innerhalb des Systems übertragen werden dürfen.

```text
Source Data
    ↓
Information Flow Policy
    ↓
Allowed Destination
```

Die Kontrolle ergänzt klassische Zugriffsrechte um Regeln für die weitere Verwendung und Weitergabe bereits zugänglicher Daten.

## Grundprinzipien

```text
Can Read ≠ Can Share
Can Receive ≠ Can Forward
Access Permission ≠ Information Flow Permission
Encryption ≠ Flow Authorization
```

Eine Komponente darf Daten lesen können, ohne deshalb berechtigt zu sein, diese beliebig weiterzugeben.

## Informationsfluss-Modell

Ein kontrollierter Datenfluss besitzt mindestens:

```text
InformationFlow
├── Source
├── Destination
├── Data Classification
├── Operation
└── Policy
```

Optional:

```text
Security Domain
Trust Level
Sovereignty Policy
Purpose
Declassification Rule
```

## Klassifikation

Daten dürfen Sicherheitsklassifikationen besitzen:

```text
Public
Internal
Sensitive
Secret
```

Zusätzliche Compartments oder anwendungsspezifische Klassen müssen möglich sein.

Die Klassifikation soll an die logische Datenidentität gebunden bleiben und nicht nur an Speicherort oder Dateipfad.

## Flow Policy

Eine Policy entscheidet, ob ein Informationsfluss zulässig ist.

```text
Source Classification
        +
Destination Trust
        +
Security Domain
        +
Operation
        ↓
Allow / Deny
```

Beispiele:

```text
Secret → Public Network     = Deny
Internal → Trusted Service  = Allow
Sensitive → Clipboard       = Policy
```

## Propagation

Sicherheitsrelevante Informationsattribute sollen bei Verarbeitung weitergegeben werden können.

```text
Sensitive Input
      ↓
Processing
      ↓
Derived Output
      ↓
Sensitive
```

Eine Verarbeitung darf eine Klassifikation nicht automatisch entfernen.

## Declassification

Eine Herabstufung muss explizit autorisiert sein.

```text
Sensitive
    ↓
Authorized Declassification
    ↓
Public
```

Declassification darf nur durch dafür autorisierte Komponenten oder Policies erfolgen.

## Systemgrenzen

Information Flow Security muss auf relevante Datenwege anwendbar sein:

```text
IPC
Storage
Network
Clipboard
Shared Memory
Messaging
Remote Services
Devices
```

Dadurch sollen verbotene Datenflüsse unabhängig vom verwendeten Transport kontrollierbar sein.

## Sovereignty

Informationsfluss darf mit Data-Sovereignty-Regeln kombiniert werden.

```text
Data
 ↓
Allowed Region / Provider / Trust Domain
 ↓
Network Intent
 ↓
Destination
```

Ein technisch erreichbares Ziel ist nicht automatisch ein zulässiges Ziel.

## Capability- und MAC-Integration

Die Autorisierungsentscheidung kann kombinieren:

```text
Capability
+ MAC
+ Information Flow Policy
        ↓
Allowed Operation
```

Eine gültige Capability darf eine verbindliche Information-Flow-Regel nicht automatisch umgehen.

## Introspection und Audit

NovaOS soll mindestens darstellen können:

```text
Source
Destination
Classification
Flow Decision
Policy
Decision Reason
Declassification
```

Verweigerte oder sicherheitskritische Datenflüsse sollen auditierbar sein.

## Normative Anforderungen

1. NovaOS MUSS kontrollierte Informationsflussregeln unterstützen können.
2. Lesezugriff DARF NICHT automatisch Weitergaberecht bedeuten.
3. Daten MÜSSEN Sicherheitsklassifikationen besitzen können.
4. Klassifikationen SOLLEN über Verarbeitungsschritte propagierbar sein.
5. Declassification MUSS explizit autorisiert sein.
6. Information-Flow-Regeln SOLLEN auf IPC, Storage und Netzwerk anwendbar sein.
7. Capabilities DÜRFEN verbindliche Flow-Regeln NICHT automatisch umgehen.
8. Sovereignty-Anforderungen MÜSSEN integrierbar sein.
9. Unbekannte sicherheitskritische Flow-Zustände DÜRFEN NICHT implizit erlaubt werden.
10. Flow-Entscheidungen SOLLEN introspektierbar und auditierbar sein.

## Abhängigkeiten

- `NPSPEC-SECURITY-CAPABILITY-0001`
- `NPSPEC-SECURITY-MAC-0001`
- `NPSPEC-SECURITY-ABAC-0001`
- `NPSPEC-SECURITY-ISOLATION-0001`
- `NPSPEC-NETWORK-SOVEREIGNTY-0001`
- `NPSPEC-VERIFY-INFORMATIONFLOW-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`
- `ADR-SECURITY-0011`

## Ergebnis

```text
Data + Classification
        ↓
Information Flow Policy
        ↓
Authorized Destination
        ↓
Controlled Data Flow
```

NovaOS kontrolliert damit nicht nur, wer Daten lesen darf, sondern auch, wohin diese Daten anschließend fließen dürfen.