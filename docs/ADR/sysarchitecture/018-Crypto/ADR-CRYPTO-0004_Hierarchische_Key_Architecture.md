# ADR-CRYPTO-0004 – Hierarchische Key Architecture

## Status

Angenommen

## Kategorie

Cryptography / Key Management / Security Architecture

## Kontext

NovaOS benötigt kryptographische Schlüssel für zahlreiche unterschiedliche Schutzbereiche, darunter:

- Storage und Per-Object Encryption,
- Benutzer- und Execution-Domain-Daten,
- Systemdienste,
- Kommunikation,
- Code Signing,
- Geräteidentitäten,
- Backups und Recovery.

Eine flache Sammlung unabhängiger Schlüssel wäre schwer skalierbar und würde Rotation, Isolation, Recovery und Revocation unnötig komplex machen.

Gleichzeitig darf die Kompromittierung eines einzelnen Schlüssels nicht automatisch große Teile des Systems gefährden.

---

## Entscheidung

NovaOS verwendet eine **hierarchische Key Architecture**.

Schlüssel werden in getrennten kryptographischen Domains organisiert und können über kontrollierte Key-Derivation-, Wrapping- oder Protection-Beziehungen miteinander verbunden werden.

```text
Hardware / Root Key
        ↓
System Key Domain
        ↓
Domain Keys
   ┌────┼─────┐
   ↓    ↓     ↓
User   App   Storage
             ↓
        Object Keys
```

Die Hierarchie beschreibt Schutz- und Ableitungsbeziehungen, nicht automatisch Authority.

---

## Grundprinzip

```text
Compromise should remain local.
```

und:

```text
Key hierarchy
    ≠
Authority hierarchy
```

Der Besitz oder die Verwendung eines übergeordneten Schlüssels darf nicht automatisch uneingeschränkte Authority über alle darunterliegenden Daten erzeugen.

---

## Key Domains

NovaOS kann getrennte Key Domains für unterschiedliche Sicherheitsbereiche verwenden.

Beispiele:

```text
System
User
Execution Domain
Application
Storage
Object
Network
Backup
Recovery
Device
```

Zwischen diesen Domains müssen explizite kryptographische Beziehungen bestehen.

---

## Root Keys

Besonders wichtige Root- oder Domain-Schlüssel sollen möglichst durch geeignete Hardware Backends geschützt werden.

```text
TPM / Secure Element / HSM
          ↓
      Root Key
          ↓
     Domain Keys
```

Root Keys sollen möglichst nicht exportierbar sein.

Sie dienen primär zum Ableiten, Entsperren oder Schützen untergeordneter Schlüssel.

---

## Key Derivation

Untergeordnete Schlüssel können aus übergeordnetem Schlüsselmaterial kryptographisch abgeleitet werden.

```text
Parent Key
    +
Domain Context
    +
Key Identity
    ↓
Derived Key
```

Die Ableitung muss Domain Separation sicherstellen.

Ein für einen Zweck erzeugter Schlüssel darf nicht unbeabsichtigt in einem anderen kryptographischen Kontext wiederverwendet werden.

---

## Key Wrapping

Nicht jede Hierarchie muss auf direkter Schlüsselableitung basieren.

NovaOS unterstützt auch:

```text
Key Encryption Key
        ↓
Wrapped Child Key
```

Dadurch können Child Keys unabhängig erzeugt, gespeichert, rotiert oder migriert werden, ohne den übergeordneten Schutzanker offenzulegen.

---

## Per-Object Encryption

Per-Object Encryption wird in die Key-Hierarchie integriert.

```text
Storage Domain Key
        ↓
Object Key Context
        ↓
Object Encryption
```

NovaOS darf dabei skalierbare Ableitungs- oder Wrapping-Verfahren verwenden, anstatt zwingend jeden Object Key separat im Klartext zu speichern.

---

## Isolation

Kryptographische Domains müssen soweit möglich voneinander isoliert bleiben.

Die Kompromittierung eines:

```text
Object Key
```

darf beispielsweise nicht automatisch die Rekonstruktion von:

```text
Domain Key
Root Key
Other Object Keys
```

ermöglichen.

Die konkrete kryptographische Konstruktion wird in NPSPECs definiert.

---

## Rotation

Schlüsselhierarchien müssen kontrollierte Rotation unterstützen.

Rotation eines Parent Keys darf nicht zwingend verlangen, sämtliche geschützten Nutzdaten unmittelbar neu zu verschlüsseln.

Key Wrapping und mehrstufige Schlüsselhierarchien können verwendet werden, um Rotation effizient zu ermöglichen.

---

## Recovery und Backup

Recovery-Schlüssel und Backup-Schlüssel werden als explizite Key Domains behandelt.

```text
Primary Key Domain
        ↓
Authorized Recovery Relationship
        ↓
Recovery Domain
```

Recovery darf keine versteckte universelle Hintertür erzeugen.

Die Existenz eines Recovery-Mechanismus muss Teil der jeweiligen Key Policy sein.

---

## Hardwarebindung und Migration

Key Policies können bestimmen, ob Schlüssel:

```text
Hardware-Bound
Migratable
Recoverable
Non-Exportable
```

sind.

Migration darf die ursprünglichen Schutzanforderungen nicht abschwächen.

---

## Secret Use und Export

Die Key-Hierarchie verändert nicht die Trennung zwischen Secret Use und Secret Export.

Ein Parent-, Domain- oder Root-Key kann für interne kryptographische Operationen verwendet werden, ohne dass sein Schlüsselmaterial exportiert werden darf.

---

## Normative Anforderungen

1. NovaOS MUSS eine hierarchische Key Architecture unterstützen.
2. Schlüssel MÜSSEN in getrennte kryptographische Domains organisierbar sein.
3. Key-Hierarchie und Authority-Hierarchie MÜSSEN getrennte Konzepte bleiben.
4. Root- und Domain-Schlüssel SOLLEN möglichst stark geschützt werden.
5. Hardwaregeschützte und nicht exportierbare Root Keys MÜSSEN unterstützt werden.
6. Key Derivation MUSS kryptographische Domain Separation gewährleisten.
7. Key Wrapping MUSS als Alternative oder Ergänzung zu Key Derivation unterstützt werden können.
8. Die Kompromittierung eines Child Keys DARF nicht automatisch Parent Keys oder andere Child Keys kompromittieren.
9. Per-Object Encryption MUSS in die Key-Hierarchie integrierbar sein.
10. Die Architektur MUSS kontrollierte Key Rotation unterstützen.
11. Recovery und Backup MÜSSEN explizite Key Policies und Key Domains verwenden.
12. Migration DARF bestehende Hardware- oder Security-Anforderungen NICHT stillschweigend abschwächen.
13. Secret Use und Secret Export MÜSSEN auch innerhalb der Key-Hierarchie getrennte Authorities bleiben.
14. Key-Beziehungen und Protection States MÜSSEN kontrolliert introspektierbar sein, ohne Schlüsselmaterial offenzulegen.

---

## Konsequenzen

### Positive Konsequenzen

- skalierbares Key Management,
- begrenzte Auswirkungen einzelner Key-Kompromittierungen,
- effiziente Rotation und Per-Object Encryption,
- Integration hardwaregeschützter Root Keys,
- kontrollierbare Backup-, Recovery- und Migrationsmodelle.

### Negative Konsequenzen

- zusätzliche Komplexität bei Key Lifecycle und Recovery,
- Parent-Key-Verlust kann abhängige Key Domains betreffen,
- Key-Derivation- und Wrapping-Beziehungen müssen dauerhaft konsistent verwaltet werden.

---

## Verworfene Alternativen

### Ein globaler Systemschlüssel

Verworfen.

Eine Kompromittierung hätte einen zu großen Schutzbereich.

### Vollständig flache Key-Struktur

Verworfen.

Rotation, Recovery und Verwaltung würden mit wachsender Objektzahl schlecht skalieren.

### Hierarchie ausschließlich durch direkte Key Derivation

Verworfen.

Unabhängig rotierbare oder migrierbare Schlüssel benötigen teilweise Key Wrapping oder andere Schutzbeziehungen.

---

## Abhängigkeiten

- `ADR-CRYPTO-0001_Crypto_Agility`
- `ADR-CRYPTO-0002_Verschlüsselung_at_Rest_und_in_Transit`
- `ADR-CRYPTO-0003_Per_Object_Encryption`
- `ADR-SECURITY-0019_Secret_Use_und_Secret_Export_getrennt`
- `ADR-SECURITY-0020_Hardware_Backend_für_schützenswerte_Schlüssel`

---

## Zugehörige NPSPECs

- `NPSPEC-CRYPTO-KEY-ARCHITECTURE-0001`
- `NPSPEC-CRYPTO-KEY-DOMAIN-0001`
- `NPSPEC-CRYPTO-KEY-DERIVATION-0001`
- `NPSPEC-CRYPTO-KEY-WRAPPING-0001`
- `NPSPEC-CRYPTO-KEY-ROTATION-0001`
- `NPSPEC-CRYPTO-KEY-RECOVERY-0001`
- `NPSPEC-CRYPTO-KEY-ARCHITECTURE-TEST-0001`

---

## Ergebnis

NovaOS organisiert kryptographische Schlüssel in kontrollierten, voneinander isolierten Hierarchien:

```text
Protected Root
      ↓
Domain Keys
      ↓
Service / Storage / User Keys
      ↓
Object Keys
```

Damit können Hardware-Schutz, Domain Separation, Per-Object Encryption, Rotation, Backup und Recovery gemeinsam umgesetzt werden, ohne einen einzelnen globalen Schlüssel zum kryptographischen Single Point of Failure zu machen.