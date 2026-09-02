# ADR-SECURITY-0013 – Code Integrity und Code Signing

## Status

Angenommen

## Kategorie

Security / Code Integrity / Trust / Software Execution

## Kontext

NovaOS muss sicherstellen können, dass ausführbarer Code seit seiner autorisierten Erstellung oder Veröffentlichung nicht unbemerkt verändert wurde und seine Herkunft überprüfbar ist.

Dies betrifft insbesondere:

- Kernel-Komponenten,
- Treiber,
- Systemdienste,
- Anwendungen,
- Libraries,
- Plugins,
- Updates,
- Boot- und Recovery-Komponenten.

Eine Signatur allein bedeutet jedoch nicht, dass Code automatisch vertrauenswürdig ist oder ausgeführt werden darf.

---

## Entscheidung

NovaOS integriert **Code Integrity und Code Signing als systemweite Sicherheitsmechanismen**.

```text
Code
 ↓
Integrity Verification
 ↓
Signature Verification
 ↓
Identity + Provenance
 ↓
Trust / Security Policy
 ↓
Execution Decision
```

Code Signing bestätigt kryptographisch Herkunft und Integrität.

Die tatsächliche Ausführungsberechtigung wird zusätzlich durch Trust-, Capability- und Security-Policies bestimmt.

---

## Grundprinzip

```text
Signed ≠ Trusted ≠ Authorized
```

Diese Eigenschaften werden getrennt behandelt.

---

## Code Integrity

Ausführbare Komponenten müssen eine überprüfbare Integritätsidentität besitzen können.

NovaOS muss erkennen können, ob Code gegenüber seinem erwarteten Zustand verändert wurde.

Die Integritätsprüfung darf unabhängig davon funktionieren, ob der Code signiert ist.

Damit können auch lokale, selbst entwickelte oder bewusst unsignierte Komponenten kontrolliert behandelt werden.

---

## Code Signing

Signaturen binden Code kryptographisch an eine überprüfbare Signer Identity.

```text
Code
 +
Signature
 +
Signer Identity
 =
Verifiable Provenance
```

NovaOS darf mehrere Signatur- und Trust-Modelle unterstützen.

Die Architektur wird nicht dauerhaft an einen einzelnen Signaturalgorithmus oder eine einzelne Zertifikatsinfrastruktur gebunden.

---

## Nova.Trust Integration

Code Signing wird in das gemeinsame Trust-Modell von NovaOS integriert.

```text
Identity
   ↓
Signature
   ↓
Provenance
   ↓
Trust
   ↓
Permission
   ↓
Capability
   ↓
Audit
```

Eine gültige Signatur liefert dabei Evidenz für Identität und Provenance, aber keine automatische Authority.

---

## Unterschiedliche Trust-Anforderungen

Unterschiedliche Codeklassen können unterschiedliche Anforderungen besitzen.

Beispielsweise können für:

```text
Kernel Component
Driver
System Service
Application
Plugin
Development Build
```

unterschiedliche Trust- und Signing-Policies gelten.

Besonders privilegierter Code darf strengeren Anforderungen unterliegen als normale Anwendungen.

---

## Laufzeitintegrität

Code Integrity endet nicht zwingend beim Laden einer Komponente.

NovaOS muss Integritätsmechanismen auch mit:

- Memory Protection,
- Executable Memory Policies,
- Hot Replacement,
- Live Evolution,
- Module Loading

kombinieren können.

Nachträglich geladener oder ersetzter Code muss denselben relevanten Sicherheitsregeln unterliegen.

---

## Updates und Hot Replacement

Softwareupdates und Live Replacement dürfen Code-Integrity-Prüfungen nicht umgehen.

```text
New Code
   ↓
Integrity / Signature / Trust Validation
   ↓
Authorized Replacement
   ↓
Activation
```

Die Aktivierung einer neuen Version muss von deren erfolgreicher Überprüfung getrennt bleiben.

---

## Revocation

NovaOS muss Signer, Zertifikate, Schlüssel oder andere Trust-Beziehungen widerrufen können.

Eine kryptographisch weiterhin korrekte Signatur kann dadurch sicherheitspolitisch ungültig werden.

```text
Valid Signature
      +
Revoked Trust
      =
Not Trusted
```

---

## Entwicklungsmodus

NovaOS darf die Ausführung selbst entwickelten oder unsignierten Codes unterstützen.

Dies muss jedoch als expliziter Security Context behandelt werden und darf keine globalen Code-Integrity-Garantien unbemerkt deaktivieren.

Entwicklungsfreiheit und Produktionssicherheit bleiben damit getrennte Policies.

---

## Compatibility Domains

Legacy-Code kann innerhalb geeigneter Compatibility Domains mit abweichenden Signing-Anforderungen ausgeführt werden.

Dadurch dürfen jedoch keine nativen Sicherheitsgrenzen von NovaOS umgangen werden.

---

## Audit und Introspection

NovaOS muss für ausführbaren Code nachvollziehbar machen können:

```text
Code Identity
Integrity State
Signer Identity
Signature State
Provenance
Trust State
Execution Policy
```

Dadurch kann die Herkunft und Vertrauensentscheidung einer laufenden Komponente analysiert werden.

---

## Normative Anforderungen

1. NovaOS MUSS systemweite Code-Integrity-Prüfungen unterstützen.
2. NovaOS MUSS kryptographisches Code Signing unterstützen.
3. Integrität, Signatur, Trust und Authority MÜSSEN getrennte Konzepte bleiben.
4. Eine gültige Signatur DARF NICHT automatisch Ausführungsauthority erzeugen.
5. Code-Signing MUSS in `Nova.Trust` integrierbar sein.
6. Unterschiedliche Codeklassen MÜSSEN unterschiedliche Trust- und Signing-Policies besitzen können.
7. Sicherheitskritischer Code MUSS strengeren Ausführungsanforderungen unterliegen können.
8. Nachträglich geladener oder ersetzter Code MUSS den relevanten Integrity- und Trust-Prüfungen unterliegen.
9. Updates und Hot Replacement DÜRFEN Code-Integrity-Prüfungen NICHT umgehen.
10. NovaOS MUSS Revocation von Signern oder Trust-Beziehungen unterstützen können.
11. Die Architektur DARF NICHT dauerhaft an einen einzelnen kryptographischen Algorithmus gebunden sein.
12. Unsignierter Entwicklungs-Code MUSS durch explizite Policy kontrollierbar sein.
13. Compatibility Domains DÜRFEN native Code-Integrity-Grenzen NICHT umgehen.
14. Code-Integrity- und Signing-Zustände MÜSSEN kontrolliert auditierbar und introspektierbar sein.
15. Nicht zuverlässig validierbarer sicherheitskritischer Code MUSS fail-safe behandelt werden.

---

## Konsequenzen

### Positive Konsequenzen

- überprüfbare Softwareherkunft,
- Schutz vor unbemerkter Code-Manipulation,
- einheitliches Trust-Modell für Kernel, Treiber und Anwendungen,
- sichere Grundlage für Updates und Live Replacement,
- Entwicklungsfreiheit bleibt kontrolliert möglich.

### Negative Konsequenzen

- Schlüssel- und Trust-Verwaltung erzeugen zusätzlichen Aufwand,
- Revocation und Offline-Verifikation benötigen klare Regeln,
- strengere Policies können inkompatible Legacy-Software einschränken.

---

## Verworfene Alternativen

### Signatur bedeutet automatisch vertrauenswürdig

Verworfen.

Eine Signatur bestätigt Herkunft und Integrität, nicht automatisch Vertrauenswürdigkeit oder Authority.

### Nur Kernel und Treiber prüfen

Verworfen.

Code Integrity soll systemweit verfügbar sein.

### Ausschließlich signierten Code zulassen

Verworfen.

Dies würde lokale Entwicklung und kontrollierte alternative Softwarequellen unnötig einschränken.

### Fester Signaturalgorithmus für die gesamte Lebensdauer von NovaOS

Verworfen.

Kryptographische Verfahren müssen langfristig austauschbar bleiben.

---

## Abhängigkeiten

- `ADR-ARCH-0012_Hot_Replacement_Live_Evolution`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-BOOT-0005_Trusted_Boot_Chain`
- `ADR-BOOT-0006_Secure_Measured_Verified_Boot`
- `ADR-SECURITY-0001_Capability_Based_Security_als_zentrales_Autorisierungsmodell`
- `ADR-SECURITY-0005_MAC_für_systemweite_harte_Sicherheitsregeln`
- `ADR-SECURITY-0012_Sandboxing_als_standardisierte_Isolationsschicht`

---

## Zugehörige NPSPECs

- `NPSPEC-SECURITY-CODE-INTEGRITY-0001`
- `NPSPEC-SECURITY-CODE-SIGNING-0001`
- `NPSPEC-SECURITY-CODE-IDENTITY-0001`
- `NPSPEC-SECURITY-SIGNER-TRUST-0001`
- `NPSPEC-SECURITY-CODE-REVOCATION-0001`
- `NPSPEC-SECURITY-CODE-INTEGRITY-TEST-0001`

---

## Ergebnis

NovaOS trennt kryptographische Nachweisbarkeit konsequent von Vertrauen und Authority:

```text
Integrity
   ↓
Signature
   ↓
Provenance
   ↓
Trust Policy
   ↓
Authorization
   ↓
Execution
```

Damit bildet Code Signing keinen pauschalen Vertrauensstempel, sondern einen überprüfbaren Bestandteil der umfassenderen `Nova.Trust`- und Sicherheitsarchitektur.