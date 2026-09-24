# NPSPEC-BOOT-SECURE-0001 – Nova Secure Boot

## Status

Entwurf

## Kategorie

Boot / Security / Integrity / Trust

## Zweck

NovaOS definiert eine verifizierte Boot-Kette, bei der sicherheitskritische Boot-Komponenten vor ihrer Ausführung auf Integrität und Authentizität geprüft werden.

```text
Firmware
   ↓
Bootloader
   ↓
Kernel
   ↓
Critical System Components
```

Jede vertrauensrelevante Stufe darf nur verifizierte nächste Stufen laden.

## Grundprinzip

```text
Verify
  ↓
Trust Decision
  ↓
Execute
```

Secure Boot schützt die Boot-Kette vor unautorisierten oder manipulierten Komponenten.

Dabei gilt:

```text
Signed ≠ Trusted
Trusted ≠ Authorized
```

Eine gültige Signatur allein reicht nicht aus. Schlüssel, Policy und Autorisierung müssen ebenfalls gültig sein.

## Chain of Trust

Die Vertrauenskette beginnt an einem definierten Trust Anchor.

```text
Trust Anchor
     ↓
Firmware / Boot Entry
     ↓
Nova Bootloader
     ↓
Nova Kernel
```

Jede Stufe überprüft die nächste, bevor Kontrolle übertragen wird.

## Verifikation

Eine Boot-Komponente kann anhand folgender Informationen geprüft werden:

```text
Cryptographic Hash
Digital Signature
Signer Identity
Key Status
Version
Security Policy
```

Fehlgeschlagene Prüfungen müssen eindeutig erkannt werden.

## Schlüssel

Boot-Schlüssel müssen kontrolliert verwaltet werden.

Unterstützt werden sollen:

```text
Trusted Keys
Key Rotation
Key Revocation
Recovery Keys
Platform Keys
```

Kompromittierte oder widerrufene Schlüssel dürfen nicht weiter als vertrauenswürdig behandelt werden.

## UEFI Secure Boot

Auf UEFI-Systemen soll NovaOS vorhandenes UEFI Secure Boot verwenden können.

```text
UEFI Secure Boot
      ↓
Nova EFI Loader
      ↓
Nova Verification
      ↓
Kernel
```

UEFI Secure Boot ersetzt jedoch nicht die NovaOS-eigene Verifikation der weiteren Boot-Kette.

## BIOS

Auf BIOS-Systemen ohne firmwareseitigen Secure Boot kann NovaOS die kryptografische Prüfung ab der frühestmöglichen kontrollierten Boot-Stufe durchführen.

Die fehlende Hardware-Root-of-Trust muss dabei als reduzierte Vertrauensstufe erkennbar bleiben.

## Anti-Rollback

Sicherheitskritische Komponenten können Mindestversionen oder Sicherheitsgenerationen definieren.

```text
Installed Version
      ↓
Minimum Allowed Version
      ↓
Allow / Reject
```

Bekanntermaßen unsichere ältere Versionen dürfen dadurch vom Boot ausgeschlossen werden.

## Recovery

Ein Verifikationsfehler darf einen kontrollierten Recovery-Pfad ermöglichen.

```text
Verification Failed
        ↓
Block Normal Boot
        ↓
Recovery Environment
```

Recovery darf Secure Boot nicht unkontrolliert umgehen.

## Messung

Zusätzlich zur Verifikation kann NovaOS Boot-Komponenten messen und die Ergebnisse für spätere Trust-Entscheidungen bereitstellen.

```text
Boot Component
      ↓
Measure
      ↓
Boot Measurement
```

Measured Boot und Secure Boot sind getrennte Mechanismen und können kombiniert werden.

## Normative Anforderungen

1. NovaOS MUSS sicherheitskritische Boot-Komponenten kryptografisch verifizieren können.
2. Eine vertrauenswürdige Boot-Kette MUSS von einem definierten Trust Anchor ausgehen.
3. Eine gültige Signatur DARF NICHT automatisch mit Autorisierung gleichgesetzt werden.
4. Widerrufene Schlüssel MÜSSEN abgelehnt werden.
5. Schlüsselrotation MUSS unterstützt werden können.
6. Kritische Verifikationsfehler MÜSSEN den normalen Bootpfad stoppen.
7. UEFI Secure Boot SOLL unterstützt werden.
8. NovaOS SOLL zusätzlich eine eigene Verifikation der Nova-Boot-Komponenten durchführen können.
9. Anti-Rollback SOLL für sicherheitskritische Komponenten unterstützt werden.
10. Recovery DARF die Secure-Boot-Policy nicht unkontrolliert umgehen.
11. Measured Boot SOLL mit Secure Boot kombinierbar sein.
12. Systeme ohne vollständige Hardware-Trust-Kette DÜRFEN NICHT dieselbe Vertrauensstufe vortäuschen.

## Abhängigkeiten

- `NPSPEC-BOOT-ARCH-0001`
- `NPSPEC-BOOT-CHAIN-0001`
- `NPSPEC-BOOT-UEFI-0001`
- `NPSPEC-BOOT-BIOS-0001`
- `NPSPEC-HAL-FIRMWARE-0001`
- `ADR-BOOT-0005`
- `ADR-TRUST-0001_Trust_als_Evidence_basiertes_System`
- `ADR-TRUST-0006_Software_und_Supply_Chain_Provenance`

## Ergebnis

```text
Trust Anchor
     ↓
Verified Boot Chain
     ↓
Verified Nova Kernel
     ↓
Trusted Boot State
```

NovaOS erhält damit eine überprüfbare Boot-Kette, bei der manipulierte, widerrufene oder nicht autorisierte Komponenten nicht unbemerkt in den normalen Systemstart gelangen können.