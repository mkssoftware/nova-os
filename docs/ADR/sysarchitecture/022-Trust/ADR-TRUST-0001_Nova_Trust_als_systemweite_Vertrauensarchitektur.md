# ADR-TRUST-0001 – Nova.Trust als systemweite Vertrauensarchitektur

## Status

Angenommen

## Kategorie

Trust Architecture / Security / Provenance

## Kontext

NovaOS muss Vertrauen für unterschiedliche Entitäten bewerten können, darunter:

- Software und Systemkomponenten,
- Anwendungen und Dienste,
- Capability Provider,
- Treiber und Geräte,
- KI-Agenten,
- Dokumente und NovaFiles,
- Medien und externe Inhalte.

Traditionell werden Code Signing, Device Trust, Medienauthentizität und Agent Governance häufig als getrennte Systeme behandelt. Dadurch entstehen unterschiedliche Vertrauensmodelle und redundante Prüfmechanismen.

## Entscheidung

NovaOS führt **Nova.Trust** als gemeinsame systemweite Vertrauensarchitektur ein.

Das grundlegende Modell lautet:

```text
Identity
   ↓
Signature
   ↓
Provenance
   ↓
Permission
   ↓
Capability
   ↓
Audit
```

Nova.Trust stellt gemeinsame Trust-Informationen und Trust-Entscheidungen für NovaOS bereit, unabhängig davon, ob das bewertete Objekt Software, Hardware, Daten oder eine aktive Systementität ist.

## Grundprinzip

```text
Trust is evaluated, not assumed.
```

Trust entsteht aus überprüfbaren Eigenschaften und Kontext.

Eine einzelne Eigenschaft wie eine gültige Signatur reicht nicht automatisch aus.

```text
Signed ≠ Trusted
Trusted ≠ Authorized
```

## Gemeinsames Trust-Modell

Nova.Trust stellt eine gemeinsame Architektur für unterschiedliche Trust Domains bereit.

```text
                    Nova.Trust
                        │
       ┌────────────────┼────────────────┐
       ↓                ↓                ↓
 Software Trust     Device Trust      Data Trust
       ↓                ↓                ↓
 Services          Hardware         Documents
 Drivers           Platforms        Media
 Agents             Devices          AI Content
```

Spezialisierte Trust Policies können auf demselben gemeinsamen Modell aufbauen.

## Trust Context

Trust wird kontextabhängig bewertet.

Eine Entität kann für einen bestimmten Zweck ausreichend vertrauenswürdig sein, ohne universell vertrauenswürdig zu sein.

Die Bewertung kann insbesondere berücksichtigen:

- Identity,
- Signaturen,
- Provenance,
- Attestation,
- Herkunft,
- Integrity State,
- Trust Domain,
- aktuelle System- und Policy-Anforderungen.

Die konkrete Trust-Berechnung wird in NPSPECs definiert.

## Trust und Authority

Nova.Trust ersetzt nicht das Capability-basierte Security-Modell.

```text
Trust      = Should this entity be trusted in this context?
Authority  = What is this entity allowed to do?
```

Eine vertrauenswürdige Entität benötigt weiterhin die erforderliche Authority.

Umgekehrt kann eine Capability durch Trust Policies zusätzlich eingeschränkt werden.

## Provenance

Provenance ist ein zentraler Bestandteil von Nova.Trust.

NovaOS soll nachvollziehen können:

```text
Where did it come from?
Who created or modified it?
How was it transformed?
What trust evidence exists?
```

Dadurch kann dasselbe Trust-Modell für Software, Daten, Dokumente, Medien und KI-generierte Inhalte verwendet werden.

## Dynamische Trust-Bewertung

Trust ist keine zwingend permanente Eigenschaft.

Änderungen an:

- Signaturstatus,
- Provenance,
- Revocation,
- Device State,
- Software Integrity,
- Security Policy,
- Trust Evidence

können eine erneute Bewertung auslösen.

Nova.Trust muss daher dynamische Neubewertung unterstützen.

## Integration

Nova.Trust wird als gemeinsame Vertrauensquelle insbesondere integriert mit:

```text
Identity
Capabilities
Code Integrity
Secure / Measured Boot
Device Identity
NovaFile Provenance
Remote Services
AI Agents
Capability Resolution
Execution Contracts
```

`Nova.ExecutionContract` kann einen erforderlichen `Trust Requirement` enthalten.

Provider, die diese Anforderung nicht erfüllen, dürfen nicht ausgewählt werden.

## Normative Anforderungen

1. NovaOS MUSS `Nova.Trust` als systemweite Trust-Architektur bereitstellen.
2. Trust MUSS von Identity, Authentication und Authority getrennt bleiben.
3. Eine gültige Signatur DARF nicht automatisch vollständiges Vertrauen erzeugen.
4. Trust MUSS kontextabhängig bewertbar sein.
5. Nova.Trust MUSS unterschiedliche Entitätstypen unter einem gemeinsamen Modell unterstützen.
6. Provenance MUSS als Trust-Eingabe verwendbar sein.
7. Trust Policies MÜSSEN spezialisierte Trust Domains unterstützen können.
8. Trust-Änderungen MÜSSEN eine Neubewertung ermöglichen.
9. Capability Authorization DARF durch Trust nicht ersetzt werden.
10. Execution Contracts MÜSSEN Trust Requirements ausdrücken können.
11. Unbekannter Trust-Status DARF nicht automatisch als vertrauenswürdig gelten.
12. Sicherheitsrelevante Trust-Entscheidungen MÜSSEN kontrolliert introspektierbar und auditierbar sein.

## Konsequenzen

### Positive Konsequenzen

- einheitliches Trust-Modell für Software, Hardware, Dienste, KI und Daten,
- gemeinsame Provenance- und Signaturinfrastruktur,
- Trust kann direkt in Capability Resolution und Execution Contracts einfließen,
- weniger voneinander getrennte Vertrauenssysteme,
- dynamische Neubewertung bei verändertem Trust-Zustand.

### Negative Konsequenzen

- Nova.Trust wird eine zentrale sicherheitsrelevante Systemarchitektur,
- unterschiedliche Trust Domains benötigen spezialisierte Policies und Evidence Provider.

## Verworfene Alternativen

### Separate Trust-Systeme für jede Domäne

Verworfen, da dadurch redundante und inkonsistente Vertrauensmodelle entstehen würden.

### Signatur gleich Vertrauen

Verworfen, da eine Signatur lediglich bestimmte Aussagen über Identität und Integrität bestätigt.

### Trust direkt als Authority verwenden

Verworfen, da Vertrauen und Berechtigung unterschiedliche Sicherheitskonzepte sind.

## Abhängigkeiten

- `ADR-IDENTITY-0001_Identity_getrennt_von_Display_Name_Credential_Process_und_Address`
- `ADR-IDENTITY-0002_Stabile_Systemidentitäten`
- `ADR-SECURITY-0001_Capability_Based_Security_als_zentrales_Autorisierungsmodell`
- `ADR-SECURITY-0013_Code_Integrity_und_Code_Signing`
- `ADR-SECURITY-0014_Secure_und_Measured_Boot_komplementär`
- `ADR-STORAGE-0008_Data_Provenance_im_Storage_Modell`
- `ADR-ARCH-0014_Explicit_Contracts`

## Zugehörige NPSPECs

- `NPSPEC-TRUST-CORE-0001`
- `NPSPEC-TRUST-CONTEXT-0001`
- `NPSPEC-TRUST-EVIDENCE-0001`
- `NPSPEC-TRUST-POLICY-0001`
- `NPSPEC-TRUST-PROVENANCE-0001`
- `NPSPEC-TRUST-EVALUATION-0001`
- `NPSPEC-TRUST-TEST-0001`

## Ergebnis

NovaOS erhält mit `Nova.Trust` eine gemeinsame Vertrauensarchitektur:

```text
Identity → Signature → Provenance → Permission → Capability → Audit
                         │
                         ↓
                    Nova.Trust
                         ↓
                Contextual Trust
```

Damit werden Softwarevertrauen, Device Trust, KI-Governance und Daten- beziehungsweise Medienauthentizität nicht als voneinander isolierte Systeme aufgebaut, sondern auf einem gemeinsamen, überprüfbaren Trust-Modell.