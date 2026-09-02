# ADR-CRYPTO-0011 – Lazy Cryptographic Migration

## Status

Angenommen

## Kategorie

Cryptography / Migration / Key Management / Evolvability

## Kontext

Crypto Agility ermöglicht NovaOS, kryptographische Algorithmen, Schlüssel, Parameter und Provider im Laufe der Zeit zu ersetzen.

Eine sofortige Migration sämtlicher gespeicherter Objekte kann bei großen Datenbeständen jedoch erhebliche I/O-, Compute- und Energie-Kosten verursachen.

NovaOS benötigt deshalb zusätzlich zur aktiven Massenmigration ein Verfahren, bei dem kryptographische Daten schrittweise bei ihrer normalen Nutzung auf einen aktuellen Schutzstand gebracht werden können.

---

## Entscheidung

NovaOS unterstützt **Lazy Cryptographic Migration** als native Migrationsstrategie.

```text
Stored Object
     ↓
Current Crypto State prüfen
     ↓
Still compliant?
 ├── Yes → normal verwenden
 └── No
       ↓
   Migration erforderlich
       ↓
   Read / Modify / Access
       ↓
   Re-Encrypt / Re-Wrap / Re-Sign
       ↓
   Current Crypto State
```

Migration kann damit bei Zugriff, Änderung oder anderen geeigneten Lifecycle-Ereignissen erfolgen.

Lazy Migration ist eine Strategie und kein Ersatz für zwingend sofortige Migration.

---

## Grundprinzip

```text
Migrate when touched,
unless security requires migration now.
```

---

## Crypto State

Kryptographisch geschützte Objekte müssen ihren relevanten Crypto State eindeutig bestimmen lassen.

Dieser kann beispielsweise umfassen:

```text
Algorithm
Parameters
Key Generation
Crypto Policy Version
Protection Backend
Format Version
Migration State
```

Damit kann NovaOS feststellen, ob ein Objekt weiterhin der aktuellen Policy entspricht.

---

## Migrationsarten

Lazy Migration kann unterschiedliche kryptographische Änderungen durchführen:

```text
Key Re-Wrapping
Key Rotation
Re-Encryption
Re-Signing
Algorithm Migration
Parameter Migration
Provider Migration
Format Migration
```

Welche Operation notwendig ist, hängt vom aktuellen und gewünschten Crypto State ab.

---

## Policygesteuerte Dringlichkeit

Crypto Policies müssen zwischen unterschiedlichen Migrationsanforderungen unterscheiden können.

Konzeptionell:

```text
Optional
Lazy
Scheduled
Required Before Next Use
Immediate
Forbidden Legacy State
```

Die konkreten Zustände und Fristen werden in NPSPECs definiert.

Ein als unsicher eingestufter kryptographischer Zustand darf nicht allein aus Performancegründen weiterhin verwendet werden.

---

## Read und Write Migration

NovaOS kann Migration an unterschiedliche Ereignisse koppeln.

```text
Read
Modify
Write
Copy
Move
Replication
Backup
Restore
```

Nicht jeder reine Lesezugriff muss zwingend einen Write-Back auslösen.

Die Policy entscheidet, wann Migration erforderlich und wann sie aufgeschoben werden darf.

---

## Hintergrundmigration

Lazy Migration kann mit kontrollierter Hintergrundmigration kombiniert werden.

```text
Lazy Migration
      +
Background Migration
      ↓
Progressive Crypto Upgrade
```

Selten verwendete Objekte können dadurch ebenfalls schrittweise migriert werden, ohne eine sofortige globale Migration zu erzwingen.

Hintergrundmigration unterliegt der systemweiten Resource Economy.

---

## Transaktionale Migration

Die Migration eines Objekts muss einen konsistenten Zustand erhalten.

```text
Old Crypto State
       ↓
Migration Transaction
       ↓
New Crypto State
```

Ein Absturz während der Migration darf nicht zu einem Objekt führen, das weder mit dem alten noch mit dem neuen Crypto State zuverlässig verwendbar ist.

---

## Key Generations

Lazy Migration wird mit Key Rotation und Key Generations integriert.

Während einer Übergangsphase können mehrere Generationen kontrolliert parallel existieren.

```text
Generation N
     ↓
Lazy Migration
     ↓
Generation N + 1
```

Alte Generationen dürfen erst entfernt werden, wenn keine weiterhin benötigten Objekte oder Recovery-Pfade davon abhängig sind.

---

## Post-Quantum Migration

Lazy Migration kann auch für den Übergang zwischen:

```text
Classical
    ↓
Hybrid
    ↓
Post-Quantum
```

verwendet werden.

Wenn eine Crypto Policy aufgrund des Bedrohungsmodells eine sofortige Post-Quantum-Migration verlangt, darf Lazy Migration diese Anforderung jedoch nicht verzögern.

---

## Resource Economy

Kryptographische Migration verbraucht:

- CPU,
- Storage-I/O,
- Speicher,
- Energie,
- Hardware-Crypto-Ressourcen.

NovaOS darf nicht dringende Migrationen deshalb entsprechend vorhandener Ressourcen planen.

```text
Security Deadline
      ↓
Resource Budget
      ↓
Migration Scheduling
```

Security Deadlines haben Vorrang vor weichen Ressourcenoptimierungen.

---

## Fail-Closed

Ein Objekt, dessen Crypto State gemäß aktueller Policy nicht mehr verwendet werden darf, darf nicht lediglich deshalb geöffnet werden, weil seine Migration noch nicht erfolgt ist.

```text
Legacy Crypto State
       ↓
Policy Evaluation
       ↓
Forbidden?
   ├── No  → Use / Lazy Migrate
   └── Yes → Migrate before Use / Fail
```

Ein stiller Security Downgrade ist unzulässig.

---

## Normative Anforderungen

1. NovaOS MUSS Lazy Cryptographic Migration unterstützen.
2. Kryptographische Objekte MÜSSEN ihren relevanten Crypto State eindeutig bestimmen lassen.
3. Crypto Policies MÜSSEN die Dringlichkeit einer Migration festlegen können.
4. Lazy Migration DARF zwingend erforderliche sofortige Migration NICHT verzögern.
5. Migration MUSS Re-Wrapping, Re-Encryption, Re-Signing und Algorithmusmigration unterstützen können.
6. Lazy Migration MUSS mit Key Generations und Key Rotation integrierbar sein.
7. Hintergrundmigration MUSS ergänzend möglich sein.
8. Nicht dringende Migrationen SOLLEN durch die Resource Economy planbar sein.
9. Security-Anforderungen MÜSSEN Vorrang vor Performance- und Energieoptimierungen haben.
10. Objektmigration MUSS transaktional oder gleichwertig absturzsicher erfolgen.
11. Alte Key Generations DÜRFEN nicht entfernt werden, solange erforderliche Objekte oder Recovery-Pfade davon abhängen.
12. Post-Quantum- und Hybridmigration MÜSSEN unterstützt werden können.
13. Ein gemäß aktueller Policy verbotener Crypto State DARF NICHT ohne erfolgreiche Migration weiterverwendet werden.
14. Migrationszustand und verwendete Crypto Generation MÜSSEN kontrolliert introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- keine zwingende sofortige Migration großer Datenbestände,
- geringere I/O-, Compute- und Energiespitzen,
- kryptographische Migration kann schrittweise erfolgen,
- gute Integration mit Key Rotation und Crypto Agility,
- langfristige Post-Quantum-Migration wird praktikabler.

### Negative Konsequenzen

- mehrere Crypto Generations können längere Zeit parallel existieren,
- selten verwendete Objekte können lange im alten Format verbleiben,
- Key Lifecycle und Recovery müssen alte Generationen berücksichtigen.

---

## Verworfene Alternativen

### Jede Crypto-Änderung erzwingt sofortige globale Migration

Verworfen.

Dies kann bei großen Datenbeständen unverhältnismäßige Ressourcenbelastungen verursachen.

### Ausschließlich Lazy Migration

Verworfen.

Kompromittierte oder verbotene kryptographische Verfahren können eine sofortige Migration verlangen.

### Unbegrenzte Unterstützung alter Crypto States

Verworfen.

Crypto Agility benötigt die Möglichkeit, unsichere Verfahren vollständig außer Betrieb zu nehmen.

---

## Abhängigkeiten

- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0019_Evolvable_System_Architecture`
- `ADR-CRYPTO-0001_Crypto_Agility`
- `ADR-CRYPTO-0003_Per_Object_Encryption`
- `ADR-CRYPTO-0004_Hierarchische_Key_Architecture`
- `ADR-CRYPTO-0005_Key_Wrapping_und_Key_Rotation`
- `ADR-CRYPTO-0008_Post_Quantum_Agility_und_Hybridverfahren`
- `ADR-SECURITY-0018_Fail_Closed_für_Sicherheitsentscheidungen`
- `ADR-STORAGE-0013_Transaktionale_Dateioperationen`

---

## Zugehörige NPSPECs

- `NPSPEC-CRYPTO-LAZY-MIGRATION-0001`
- `NPSPEC-CRYPTO-MIGRATION-POLICY-0001`
- `NPSPEC-CRYPTO-MIGRATION-STATE-0001`
- `NPSPEC-CRYPTO-BACKGROUND-MIGRATION-0001`
- `NPSPEC-CRYPTO-TRANSACTIONAL-MIGRATION-0001`
- `NPSPEC-CRYPTO-LAZY-MIGRATION-TEST-0001`

---

## Ergebnis

NovaOS kann kryptographische Datenbestände schrittweise auf neue Schlüssel, Algorithmen und Schutzmechanismen migrieren:

```text
Legacy Crypto Object
        ↓
Access / Background Work
        ↓
Policy Evaluation
        ↓
Crypto Migration
        ↓
Current Crypto Object
```

Lazy Cryptographic Migration reduziert die Kosten großer kryptographischer Umstellungen, ohne verbindliche Security-Anforderungen oder Migrationsfristen abzuschwächen.