# ADR-TRUST-0002 – Identity, Signature, Provenance, Permission, Capability, Audit

## Status

Angenommen

## Kategorie

Trust Architecture / Trust Chain

## Kontext

Vertrauen darf in NovaOS nicht aus einem einzelnen Signal wie einer Signatur oder bekannten Identity abgeleitet werden.

Für belastbare Trust-Entscheidungen müssen Identität, Integrität, Herkunft, Berechtigung, tatsächliche Authority und Nachvollziehbarkeit gemeinsam betrachtet werden.

## Entscheidung

Nova.Trust verwendet die folgende systemweite Vertrauenskette als grundlegendes Architekturmodell:

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

Jede Stufe beantwortet eine eigene Fragestellung und darf nicht mit einer anderen Stufe gleichgesetzt werden.

## Grundprinzip

```text
Identity   → Wer oder was ist die Entität?
Signature  → Ist die signierte Aussage kryptographisch verifizierbar?
Provenance → Woher stammt die Entität und was geschah mit ihr?
Permission → Ist die beabsichtigte Nutzung policyseitig zulässig?
Capability → Welche konkrete Authority besteht?
Audit      → Was wurde tatsächlich entschieden und ausgeführt?
```

Die Kette bildet keinen automatischen Vertrauensaufstieg.

```text
Known Identity
≠ Signed
≠ Trusted
≠ Permitted
≠ Authorized
```

## Identity

Identity stellt die stabile Referenz auf die betrachtete Entität bereit.

Nova.Trust verwendet das native NovaOS-Identity-Modell und erzeugt keine parallele Trust-Identity.

## Signature

Signaturen liefern kryptographisch überprüfbare Aussagen über Integrität und Signierer.

Eine gültige Signatur bestätigt jedoch nur das, was tatsächlich signiert und durch die verwendete Identity beziehungsweise Credential-Bindung nachgewiesen wurde.

```text
Valid Signature ≠ Trusted Content
```

## Provenance

Provenance beschreibt Herkunft und relevante Transformationen.

Sie verbindet beispielsweise:

```text
Creator
   ↓
Original Object
   ↓
Transformation
   ↓
Derived Object
```

Dadurch kann Nova.Trust nicht nur den aktuellen Zustand, sondern auch die Herkunftskette berücksichtigen.

## Permission

Permission beschreibt, ob die beabsichtigte Nutzung unter den geltenden Policies zulässig ist.

Dabei können insbesondere Security-, Privacy-, Trust-, Sovereignty- und Information-Flow-Regeln berücksichtigt werden.

Permission erzeugt jedoch keine Authority.

## Capability

Capabilities stellen die tatsächlich delegierte Authority für eine Operation bereit.

```text
Permission + Required Capability
              ↓
        Authorized Operation
```

Trust und Permission dürfen fehlende Capabilities nicht ersetzen.

## Audit

Audit dokumentiert sicherheitsrelevante Entscheidungen und Operationen so, dass die Trust- und Authority-Kette nachvollziehbar bleibt.

Audit ist damit Bestandteil des Trust-Modells, aber keine Voraussetzung dafür, eine ansonsten unzulässige Operation zu erlauben.

## Anwendung auf unterschiedliche Entitäten

Das Modell gilt systemweit und kann unter anderem auf folgende Entitäten angewendet werden:

```text
Software
Drivers
Services
Devices
AI Agents
NovaFiles
Documents
Media
Remote Providers
```

Nicht jede Entität muss für jede Stufe dieselbe technische Implementierung verwenden.

Die semantische Trennung bleibt jedoch erhalten.

## Normative Anforderungen

1. Nova.Trust MUSS Identity, Signature, Provenance, Permission, Capability und Audit als getrennte Konzepte behandeln.
2. Eine bekannte Identity DARF nicht automatisch als vertrauenswürdig gelten.
3. Eine gültige Signature DARF nicht automatisch Permission oder Authority erzeugen.
4. Provenance MUSS unabhängig von Signature bewertbar sein.
5. Permission MUSS relevante System-Policies berücksichtigen können.
6. Permission DARF fehlende Capability Authority nicht ersetzen.
7. Capability MUSS die konkrete Authority einer Operation bestimmen können.
8. Trust-Entscheidungen DÜRFEN Capability-basierte Authorization nicht umgehen.
9. Audit MUSS relevante Trust- und Authorization-Entscheidungen nachvollziehbar machen können.
10. Die Vertrauenskette MUSS auf unterschiedliche NovaOS-Entitätstypen anwendbar sein.
11. Unbekannte oder nicht verifizierbare Zustände DÜRFEN nicht automatisch positiv interpretiert werden.
12. Die relevanten Bestandteile einer Trust-Entscheidung MÜSSEN kontrolliert introspektierbar sein.

## Konsequenzen

### Positive Konsequenzen

- klare Trennung zwischen Vertrauen und Authority,
- gemeinsame Trust-Semantik für unterschiedliche Systembereiche,
- Provenance wird gleichwertiger Bestandteil der Vertrauensbewertung,
- nachvollziehbare Trust- und Security-Entscheidungen,
- Signaturen werden nicht mit vollständigem Vertrauen verwechselt.

### Negative Konsequenzen

- Trust-Entscheidungen benötigen Informationen aus mehreren Systemschichten,
- die beteiligten Komponenten müssen gemeinsame Trust-Kontexte austauschen können.

## Verworfene Alternativen

### Signaturbasierter Trust

Verworfen, da eine gültige Signatur allein keine Aussage über Permission oder Authority trifft.

### Identity-basierter Trust

Verworfen, da bekannte Identität nicht automatisch vertrauenswürdiges Verhalten bedeutet.

### Trust und Authorization zusammenführen

Verworfen, da Vertrauen und konkrete Authority unterschiedliche Aufgaben erfüllen.

## Abhängigkeiten

- `ADR-TRUST-0001_Nova_Trust_als_systemweite_Vertrauensarchitektur`
- `ADR-IDENTITY-0001_Identity_getrennt_von_Display_Name_Credential_Process_und_Address`
- `ADR-SECURITY-0001_Capability_Based_Security_als_zentrales_Autorisierungsmodell`
- `ADR-SECURITY-0013_Code_Integrity_und_Code_Signing`
- `ADR-STORAGE-0008_Data_Provenance_im_Storage_Modell`
- `ADR-CAPABILITY-0001_Capabilities_als_primäre_Systemfähigkeiten`

## Zugehörige NPSPECs

- `NPSPEC-TRUST-CHAIN-0001`
- `NPSPEC-TRUST-SIGNATURE-0001`
- `NPSPEC-TRUST-PROVENANCE-0001`
- `NPSPEC-TRUST-PERMISSION-0001`
- `NPSPEC-TRUST-AUDIT-0001`
- `NPSPEC-TRUST-CHAIN-TEST-0001`

## Ergebnis

Nova.Trust verwendet eine klar getrennte Vertrauenskette:

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

Keine einzelne Stufe erzeugt automatisch die nächste. Dadurch bleiben Identität, kryptographischer Nachweis, Herkunft, Policy-Zulässigkeit, konkrete Authority und Nachvollziehbarkeit als eigenständige Sicherheitskonzepte erhalten.