# NPSPEC-CRYPTO-RNG-0001 – Nova Cryptographic Random Number Generation

## Status

Angenommen

## Kategorie

Cryptography / Randomness / CSPRNG

## Zweck

NovaOS definiert eine zentrale Infrastruktur für kryptografisch sichere Zufallszahlen.

```text
Entropy Sources
      ↓
Entropy Collection
      ↓
CSPRNG
      ↓
Cryptographic Random Data
```

Sicherheitskritische Komponenten dürfen nicht auf ungeeignete Pseudozufallsquellen angewiesen sein.

## Grundprinzipien

```text
Random ≠ Unpredictable
PRNG ≠ CSPRNG
Entropy Source ≠ Random Generator
Hardware RNG ≠ Automatically Trusted
More Entropy Sources ≠ Automatically Secure
```

## Architektur

Die RNG-Infrastruktur besteht logisch aus:

```text
Nova.Crypto.Random
├── Entropy Sources
├── Entropy Pool
├── CSPRNG
├── Health Monitoring
└── Provider Interface
```

## Entropiequellen

NovaOS darf mehrere Entropiequellen kombinieren:

```text
CPU RNG
Firmware RNG
TPM
Hardware RNG
Timing Jitter
Device Events
Platform-specific Sources
```

Keine einzelne externe Quelle muss automatisch vollständig vertraut werden.

## Entropie-Pool

Entropie aus mehreren Quellen soll in einem geschützten internen Zustand kombiniert werden.

```text
Source A ─┐
Source B ─┼→ Entropy Pool → CSPRNG
Source C ─┘
```

Der interne Zustand darf nicht über normale APIs, Logs oder Introspection offengelegt werden.

## CSPRNG

NovaOS muss mindestens einen systemweiten kryptografisch sicheren Zufallszahlengenerator bereitstellen.

Dieser muss geeignet sein für:

```text
Cryptographic Keys
Nonces
Salts
Tokens
Session IDs
Capability Secrets
Protocol Challenges
```

Nicht sicherheitskritische Zufallsfunktionen dürfen getrennte APIs verwenden.

## Initialisierung

Sicherheitskritische Zufallswerte dürfen erst erzeugt werden, wenn der CSPRNG ausreichend initialisiert wurde.

```text
Boot
 ↓
Entropy Collection
 ↓
CSPRNG Ready
 ↓
Secure Random Available
```

Ist sichere Initialisierung nicht möglich, muss die Operation fehlschlagen oder warten.

```text
Insufficient Entropy ≠ Weak Random Fallback
```

## Reseeding

Der CSPRNG muss kontrolliertes Reseeding unterstützen.

Auslöser dürfen sein:

```text
New Entropy
Time
Generated Data Volume
Security Event
Resume
Migration
```

## Fork, Snapshot und Restore

Kopierte Systemzustände dürfen nicht dauerhaft identische Zufallsfolgen erzeugen.

Nach:

```text
Checkpoint
Restore
VM Clone
Process Duplication
Migration
```

muss der RNG-Zustand soweit erforderlich diversifiziert oder neu initialisiert werden.

## Hardware RNG

Hardware-Zufallsquellen dürfen verwendet werden, sollen aber durch die Nova-RNG-Infrastruktur verarbeitet werden.

```text
Hardware RNG
     ↓
Validation / Mixing
     ↓
Entropy Pool
```

Hardwareausgabe soll nicht unnötig direkt als kryptografischer Schlüssel verwendet werden.

## Fehlererkennung

Entropy- und RNG-Provider sollen Health Checks unterstützen.

Mögliche Zustände:

```text
Ready
Degraded
Unavailable
Failed
```

Fehlerhafte Quellen müssen deaktivierbar sein, ohne automatisch die gesamte RNG-Infrastruktur unbrauchbar zu machen.

## Ressourcensteuerung

Zufallsanforderungen müssen begrenzt und effizient verarbeitet werden.

Sehr große Anforderungen dürfen intern segmentiert werden.

RNG-Aufrufe dürfen nicht unbegrenzt Kernelressourcen blockieren.

## Introspection

NovaOS soll sicher darstellen können:

```text
RNG State
Provider
Active Entropy Sources
Health State
Reseed State
```

Nicht sichtbar sein dürfen:

```text
Internal CSPRNG State
Entropy Pool Content
Generated Secret Values
```

## Normative Anforderungen

1. NovaOS MUSS einen zentralen CSPRNG bereitstellen.
2. Sicherheitskritische Komponenten MÜSSEN kryptografisch sichere Zufallsquellen verwenden.
3. Mehrere Entropiequellen MÜSSEN kombinierbar sein.
4. Einzelne Hardware-RNGs DÜRFEN NICHT automatisch als vollständig vertrauenswürdig gelten.
5. Unzureichende Entropie DARF NICHT zu einem stillen schwachen Fallback führen.
6. Der CSPRNG MUSS Reseeding unterstützen.
7. Snapshot-, Restore- und Migrationsszenarien MÜSSEN RNG-Zustände berücksichtigen.
8. Fehlerhafte Entropiequellen MÜSSEN isolierbar sein.
9. Interne RNG- und Entropiezustände DÜRFEN NICHT offengelegt werden.
10. RNG-Provider MÜSSEN mit der Nova Crypto Architecture integrierbar sein.

## Abhängigkeiten

- `NPSPEC-CRYPTO-ARCH-0001`
- `NPSPEC-CRYPTO-AGILITY-0001`
- `NPSPEC-CRYPTO-KDF-0001`
- `NPSPEC-CRYPTO-KEYSTORE-0001`
- `NPSPEC-PROCESS-CHECKPOINT-0001`
- `NPSPEC-PROCESS-MIGRATION-0001`
- `ADR-CRYPTO-0009`

## Ergebnis

```text
Multiple Entropy Sources
          ↓
Protected Entropy Pool
          ↓
        CSPRNG
          ↓
Secure Random Values
```

NovaOS erhält damit eine zentrale und robuste Zufallsinfrastruktur für kryptografische Schlüssel, Nonces, Tokens und andere sicherheitskritische Zufallswerte.