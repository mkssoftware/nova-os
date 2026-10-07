# NPSPEC-TIME-TRUSTED-0001 – Nova Trusted Time

## Status

Angenommen

## Kategorie

Time / Trusted Time

## Zweck

NovaOS definiert Trusted Time als Zeitinformation, deren Herkunft, Integrität, Qualität und Vertrauenskette ausreichend geprüft wurden, um sie für sicherheitskritische Entscheidungen verwenden zu können.

Trusted Time ist keine eigene physische Uhr und nicht automatisch mit Wall Clock, RTC oder Netzwerkzeit identisch.

## Grundprinzipien

```text
Trusted Time ≠ Wall Clock
Trusted Time ≠ RTC
Trusted Time ≠ NTP
Trusted Time ≠ PTP
Authenticated Source ≠ Accurate Time
Accurate Time ≠ Trusted Time
Unknown ≠ Trusted
```

## Modell

```text
TrustedTime
├── TrustedTimeID
├── Instant
├── SourceID
├── Provenance
├── TrustLevel
├── Uncertainty
├── LastValidation
├── MonotonicEvidence
└── State
```

## Vertrauenszustände

```text
Trusted
Degraded
Untrusted
Unknown
Unavailable
```

`Unknown` darf niemals als `Trusted` behandelt werden.

## Architektur

```text
Time Sources
     ↓
Synchronization
     ↓
Source Validation
     ↓
Trust Verification
     ↓
Consistency Checks
     ↓
Trusted Time
```

Mögliche Quellen sind:

```text
Trusted Hardware
TPM / Secure Hardware
Authenticated Time Provider
Attested Platform
Validated NTP/PTP Provider
Secure Network Time
Persisted Trusted State
```

Keine Quelle ist allein aufgrund ihres Typs automatisch vertrauenswürdig.

## Provenance

Trusted Time muss ihre Herkunft nachvollziehbar machen können:

```text
Source
  ↓
Authentication
  ↓
Validation
  ↓
Transformation
  ↓
Current Trusted Time
```

Jede relevante Transformation muss die Vertrauensbewertung erhalten oder neu bewerten.

## Unsicherheit

Trusted Time muss eine Unsicherheit ausdrücken können:

```text
Trusted Instant ± Uncertainty
```

Sicherheitsentscheidungen dürfen diese Unsicherheit berücksichtigen.

Eine hohe Vertrauenswürdigkeit bedeutet nicht automatisch hohe Präzision.

## Rollback-Erkennung

NovaOS muss verdächtige Rückwärtsbewegungen sicherheitsrelevanter Zeit erkennen können.

```text
Previously Trusted Time
          ↓
New Time < Expected Range
          ↓
Rollback Detection
          ↓
Reject / Degrade / Verify
```

Eine manipulierte RTC oder Wall Clock darf dadurch nicht ohne Weiteres Sicherheitszustände zurücksetzen.

## Holdover

Fällt die vertrauenswürdige Referenz aus:

```text
Trusted Reference Lost
        ↓
Trusted Holdover
        ↓
Local Stable Clock
        ↓
Growing Uncertainty
```

Mit wachsender Unsicherheit darf der Vertrauensstatus degradiert werden.

## Sicherheitsanwendungen

Trusted Time darf verwendet werden für:

```text
Certificate Validation
Credential Expiration
Token Lifetime
Secure Update Validation
Rollback Protection
Audit Ordering
Attestation
Security Policy
```

Die jeweilige Sicherheitskomponente entscheidet, welches TrustLevel und welche maximale Unsicherheit akzeptabel sind.

## Monotone Evidenz

Trusted Time darf mit monotoner Zeit gekoppelt werden:

```text
Trusted UTC Anchor
       +
Monotonic Clock
       ↓
Trusted Time Estimate
```

Dadurch kann zwischen externen Synchronisationen ein kontrollierter Zeitfortschritt erhalten werden.

## Manipulation

Änderungen der normalen Wall Clock dürfen Trusted-Time-Zustände nicht automatisch überschreiben.

```text
User changes clock
        ≠
Trusted Time reset
```

Administrative Berechtigung zur Wall-Clock-Änderung bedeutet nicht automatisch Authority über Trusted Time.

## Normative Anforderungen

1. NovaOS MUSS Trusted Time getrennt von normaler Wall Clock behandeln.
2. Zeitquellen DÜRFEN nicht allein aufgrund ihres Typs als vertrauenswürdig gelten.
3. Trusted Time MUSS Provenance besitzen.
4. TrustLevel und Zeitgenauigkeit MÜSSEN getrennt bewertet werden.
5. Trusted Time MUSS Unsicherheit ausdrücken können.
6. `Unknown` DARF nicht als `Trusted` behandelt werden.
7. Zeit-Rollbacks MÜSSEN für sicherheitskritische Verwendung erkennbar sein.
8. Trusted Time SOLL mit monotoner Zeit verankerbar sein.
9. Verlust einer Referenz MUSS kontrollierten Holdover ermöglichen können.
10. Unsicherheit MUSS während Holdover wachsen können.
11. Änderungen der normalen Wall Clock DÜRFEN Trusted Time nicht automatisch überschreiben.
12. Quelle, Provenance, TrustLevel, Unsicherheit, Validierungszeitpunkt und Zustand MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-TIME-ARCH-0001`
- `NPSPEC-TIME-MONOTONIC-0001`
- `NPSPEC-TIME-RTC-0001`
- `NPSPEC-TIME-SYNCHRONIZATION-0001`
- `NPSPEC-TIME-NTP-0001`
- `NPSPEC-TIME-PTP-0001`
- `NPSPEC-SYSTEM-TRUST-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS besitzt eine von der normalen Systemzeit getrennte Trusted-Time-Schicht. Sicherheitskritische Komponenten können dadurch Zeitinformationen anhand von Herkunft, Vertrauenskette, Unsicherheit und Rollback-Evidenz bewerten, anstatt einer beliebig veränderbaren Wall Clock oder einzelnen externen Zeitquelle blind zu vertrauen.