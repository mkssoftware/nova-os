# NPSPEC-BOOT-VERIFIED-0001 – Nova Verified Boot

## Status

Entwurf

## Kategorie

Boot / Security / Integrity / Verification

## Zweck

NovaOS überprüft sicherheitskritische Boot-Komponenten und Systemdaten kryptografisch, bevor sie für den weiteren Systemstart verwendet werden.

```text
Boot Component
      ↓
Verification
      ↓
Valid / Invalid / Unknown
      ↓
Boot Decision
```

## Grundprinzip

Verified Boot stellt sicher, dass geladene Komponenten dem erwarteten und autorisierten Zustand entsprechen.

```text
Expected State
      ↕
Verification
      ↕
Actual State
```

Dabei gilt:

```text
Unknown ≠ Verified
```

Eine nicht erfolgreich überprüfte Komponente darf nicht als verifiziert behandelt werden.

## Verifikationsobjekte

Verified Boot kann unter anderem prüfen:

```text
Bootloader
Kernel
Kernel Modules
Boot Configuration
Critical System Images
Recovery Environment
Boot Metadata
```

Welche Komponenten verpflichtend verifiziert werden, wird durch die Boot- und Security-Policy bestimmt.

## Verifikationsmethoden

Je nach Objekt können verwendet werden:

```text
Cryptographic Hash
Digital Signature
Merkle Tree
Authenticated Metadata
Version Information
Trusted Manifest
```

Die konkrete Methode darf austauschbar und kryptografisch evolvierbar bleiben.

## Trusted Manifest

Ein vertrauenswürdiges Manifest kann erwartete Komponenten und deren Eigenschaften beschreiben.

```text
ComponentID
Version
Hash
Signature Policy
Dependencies
Security Generation
```

Das Manifest selbst muss ebenfalls verifiziert werden.

## Boot-Ablauf

```text
Load Component
      ↓
Read Verification Metadata
      ↓
Verify Integrity / Authenticity
      ↓
Apply Policy
      ↓
Execute / Reject / Recover
```

Eine Komponente darf erst nach erfolgreicher erforderlicher Verifikation aktiviert werden.

## Secure Boot

Secure Boot und Verified Boot ergänzen sich.

```text
Secure Boot
   → schützt die Vertrauenskette

Verified Boot
   → überprüft konkrete Boot- und Systemkomponenten
```

Verified Boot darf nicht davon ausgehen, dass eine durch Secure Boot gestartete Komponente automatisch alle weiteren Daten korrekt geladen hat.

## Measured Boot

Verified Boot kann mit Measured Boot kombiniert werden.

```text
Verify
  ↓
Measure
  ↓
Execute
```

Verifikation entscheidet über Zulässigkeit.

Messung dokumentiert den tatsächlich gestarteten Zustand.

## Anti-Rollback

Verified Boot soll bekannte unsichere ältere Versionen ablehnen können.

```text
Component Version
       ↓
Security Generation
       ↓
Policy
       ↓
Allow / Reject
```

Ein kryptografisch gültiges, aber widerrufenes oder zu altes Image darf dadurch abgelehnt werden.

## Recovery

Bei fehlgeschlagener Verifikation kann NovaOS einen verifizierten Recovery-Pfad verwenden.

```text
Verification Failed
        ↓
Normal Boot Blocked
        ↓
Verified Recovery
```

Recovery darf die Verifikationsregeln nicht unkontrolliert umgehen.

## Normative Anforderungen

1. NovaOS MUSS sicherheitskritische Boot-Komponenten verifizieren können.
2. Erforderliche Verifikation MUSS vor Aktivierung der jeweiligen Komponente erfolgen.
3. `Unknown` DARF NICHT als `Verified` behandelt werden.
4. Hashes, Signaturen und Verifikationsmetadaten MÜSSEN kryptografisch geeignet sein.
5. Trusted Manifests MÜSSEN selbst verifiziert werden.
6. Verifikationsfehler MÜSSEN bei kritischen Komponenten den normalen Bootpfad stoppen.
7. Verified Boot MUSS mit Secure Boot kombinierbar sein.
8. Verified Boot SOLL mit Measured Boot kombinierbar sein.
9. Anti-Rollback SOLL für sicherheitskritische Komponenten unterstützt werden.
10. Widerrufene Komponenten oder Schlüssel MÜSSEN ablehnbar sein.
11. Recovery-Komponenten MÜSSEN denselben definierten Verifikationsregeln unterliegen.
12. Verifikationsmechanismen MÜSSEN kryptografisch evolvierbar bleiben.

## Abhängigkeiten

- `NPSPEC-BOOT-ARCH-0001`
- `NPSPEC-BOOT-CHAIN-0001`
- `NPSPEC-BOOT-SECURE-0001`
- `NPSPEC-BOOT-MEASURED-0001`
- `ADR-BOOT-0007`
- `ADR-TRUST-0001_Trust_als_Evidence_basiertes_System`
- `ADR-TRUST-0006_Software_und_Supply_Chain_Provenance`

## Ergebnis

```text
Boot Components
       ↓
Cryptographic Verification
       ↓
Policy Evaluation
       ↓
Verified Boot State
       ↓
Nova Kernel
```

NovaOS kann damit sicherstellen, dass für den Systemstart verwendete kritische Komponenten dem erwarteten und autorisierten Zustand entsprechen.